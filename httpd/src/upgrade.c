//**************************************************************************************
// File Name  : upgrade.c
//
// Description: perform a http firmware upload with image and tag validation system
//
// Created    : 05/18/2015  seanl
//**************************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "bcmtypes.h"
#include "upload.h"
#include "httpd.h"
#include "syscall.h"
#include "http2dbs.h"
#include "md5.h"
#include "httpd2sysMonitor.h"
//#include <mtd-abi.h>

#define  BOUND_SIZE        256
#define  BUF_SIZE          2048
#define  FN_TOKEN          "filename="
#define  FN_TOKEN_LEN      sizeof(FN_TOKEN)
#define  MIN_UPGRADE_LEN    2 * 1024            // min. upgrade len if less than that, assume wrong file name.

typedef enum
{
    NO_IMAGE_FORMAT,
    BROADCOM_IMAGE_FORMAT,
    FLASH_IMAGE_FORMAT,
    PSI_TEXT_FORMAT,
    APP_IMAGE_FORMAT
} PARSE_RESULT;

typedef enum
{
    UPGRADE_OK,
    UPGRADE_FAIL_NO_MEM,
    UPGRADE_FAIL_NO_FILENAME,
    UPGRADE_FAIL_ILLEGAL_IMAGE,
    UPGRADE_FATAL,  // The state above this line will be mapped to a messge in uploadinfo.html
    UPGRADE_FAIL_FLASH = 20
} UPGRADE_RESULT;

static int imageLen = 0;
static int fNeedReset = FALSE;
static char *imagePtr = NULL;
static PARSE_RESULT imageType = NO_IMAGE_FORMAT;
extern int glbUpgradeMode;     // used for web page upload image or updating settings.
int glbUpgradeStatus;  // global variable that is used in cgimain.c
int glbstyle;
int gradestatus;
int gradejump;
extern WEB_NTWK_VAR glbWebVar;


#define     DEBUG_UPGRADE            // if uncommented,  enable debug display
#ifdef DEBUG_UPGRADE

static const char hextable[16] = "0123456789ABCDEF";
void dumpUpgradeHex(unsigned char *start, int len)
{
    unsigned char *ptr = start,
    *end = start + len;

    while (ptr < end)
    {
        long offset = ptr - start;
        unsigned char text[120],
        *p = text;
        while (ptr < end && p < &text[16 * 3])
        {
            *p++ = hextable[*ptr >> 4];
            *p++ = hextable[*ptr++ & 0xF];
            *p++ = ' ';
        }
        p[-1] = 0;
        printf("%4lX: %s\n", offset, text);
    }
}

#endif

int checkUpgradeFile(void)
{
	if( imageLen < 80 )
	{
		do_upgrade_finnal();
		return BOOL_FALSE;
	}
	if( imagePtr == NULL )
	{
		do_upgrade_finnal();
		return BOOL_FALSE;
	}
	if( gradestatus == 1 )
	{
		if( CMM_SUCCESS != extract_file("/var/tmp/ubifs.bin", imagePtr, imageLen))
		{
			printf("ubifs.bin is not exists\n");
			do_upgrade_finnal();
			return BOOL_FALSE;
		}
		return BOOL_TRUE;
	}
	else
	{
		if( CMM_SUCCESS != extract_file("/var/tmp/env.bin", imagePtr, imageLen) )
		{
			printf("env.bin is not exists\n");
			do_upgrade_finnal();
			return BOOL_FALSE;
		}
		return BOOL_TRUE;
	}
}

int eraseUpgradeFlash(void)
{
	if( gradestatus == 1 )
	{
		printf("erase flash disk->ubifs.bin");
		system("flash_erase /dev/mtd4 0 224 >/dev/null");
		return BOOL_TRUE;
	}
	else {
		printf("erase flash disk->env.bin\n");
		system("flash_erase /dev/mtd2 0 2 >/dev/null");
		return BOOL_TRUE;
	}
}

UPGRADE_RESULT flashUpgradeImage(char *imagePtr, PARSE_RESULT imageType, int imageLen)
{
	if( gradestatus == 1 )
	{
		printf("copy /var/tmp/ubifs.bin to /dev/mtd4\n");
		system("dd if=/var/tmp/ubifs.bin of=/dev/mtd4 >/dev/null");
		gradestatus = 0;
		gradejump = 1;
		return UPGRADE_OK;
	}
	else
	{
		printf("copy /var/tmp/env.bin to /dev/mtd2\n");
		system("dd if=/var/tmp/env.bin of=/dev/mtd2 bs=2048 >/dev/null");
		gradestatus = 1;
		return UPGRADE_OK;
	}
}

UPGRADE_RESULT upGradeImage(FILE *stream, int upgrade_len, int upgrade_type, char **imageBufPtrPtr,  
                          PARSE_RESULT *imageType, int *imageLen, int *fNeedReset)
{
	char buf[BUF_SIZE];
   	char bound[BOUND_SIZE];
    	int boundSize = 0;
    	unsigned int upgradeSize = 0;
    	int byteRd = 0;
    	char *pdest = NULL;
    	UPGRADE_RESULT result = UPGRADE_OK;
    	char *curPtr = NULL;
    	int flashSize = 0;

    	bound[0] = '\0';
    	buf[0] = '\0';
	glbstyle = 1;
	
    	fgets(bound, BOUND_SIZE, stream);

   	boundSize = strlen(bound) - 2;

    	if (boundSize <= 0)
    	{
        	printf("Failed to find bound !!!\n");
        	return UPGRADE_FATAL;                 // fatal
    	}

    	bound[boundSize] = '\0';
    
    	// get Content-Disposition: form-data; name="filename"; filename="test"
    	// check filename, if "", user click no filename and report

    	fgets(buf, BUF_SIZE, stream);   


    if ((pdest = strstr(buf, FN_TOKEN)) == NULL)
    {
        printf("No filename ? \n");
        return UPGRADE_FATAL;                 // fatal
    }
    else
    {
        pdest += (FN_TOKEN_LEN - 1);
        if (*pdest == '"' && *(pdest + 1) == '"')
        {
            printf("No filename selected\n");
            result = UPGRADE_FAIL_NO_FILENAME;
        }
        else
            printf("\nfilename = %s\n", pdest);
    }
    
    // get [Content-Type: application/octet-stream] and NL (cr/lf) and discard them

    fgets(buf, BUF_SIZE, stream);
    fgets(buf, BUF_SIZE, stream);

    // add some overhead for the w image and sub bound size from upload_len
    upgrade_len -= boundSize;
    flashSize = sysFlashSizeGet();
    if (upgrade_len > flashSize)
    {
        printf("Size is over by %d\n", upgrade_len - flashSize);
        result = UPGRADE_FAIL_ILLEGAL_IMAGE;
	 *fNeedReset = FALSE;
    }
    else if (upgrade_len < MIN_UPGRADE_LEN &&
             upgrade_type == WEB_UPLOAD_IMAGE)  // for fake file or just the path without file name.
    {
        if( UPGRADE_FAIL_NO_FILENAME != result )
	 {
		printf("Wrong file or illegal size [%d]\n", upgrade_len);
		result = UPGRADE_FAIL_ILLEGAL_IMAGE;
	 }
	 *fNeedReset = FALSE;
    }
    else if (result == UPGRADE_OK)
    {
        printf("upgrade start with len = %d with flash size = %d ****\n", upgrade_len, flashSize);
        // only kill processes when upload image (but not for upload configurations)
        if ( upgrade_type == WEB_UPLOAD_IMAGE ) {
            bcmKillAllApps();
            printf("Done removing processes\n");
            *fNeedReset = TRUE;
        }
        printf("Allocating %d byte buffer\n", upgrade_len);
        curPtr = (char *) malloc(upgrade_len);
        if( curPtr == NULL )
        {
            printf("Failed to allocate memory for the image. Size required: [%d]\n", upgrade_len);
            result = UPGRADE_FAIL_NO_MEM;
        }
        else
        {
            *imageBufPtrPtr = curPtr;
            printf("Memory allocated\n");
        }
    }

    while(fgets(buf, BUF_SIZE, stream))
    {    	 
        pdest = memchr(buf, 0xa, BUF_SIZE);
        if (pdest == NULL)
            byteRd = BUF_SIZE - 1;      // last byte not count!
        else
            byteRd = pdest - buf + 1;   // include 0xa

        // search for bound...
        if ((pdest = memchr(buf, '-', byteRd)) != NULL)
        {
            if (strstr(buf, bound) != NULL)
            {
                if (byteRd > (boundSize + 2) && *(buf + boundSize) == '-' && *(buf + boundSize + 1) == '-')
                {
                    //printf("bound is found in %s with len = %d.\n", buf, byteRd); 
                    upgradeSize -= 2;
                    break;
                }
                else
                    printf("Failed finding bound ?\n");
            }
        }

        if (result != UPGRADE_OK)
            continue;               // just throw away the data and search for end of bound
        else  // move the data to the imageBuf
        {
            memcpy(curPtr, buf, byteRd);
            curPtr += byteRd;
            upgradeSize += byteRd;
            if (upgradeSize > upgrade_len)        // try not to over flow the buffer
            {
                printf("Failed on over sized image ?\n");
                result = UPGRADE_FAIL_ILLEGAL_IMAGE;
            }
        }
    }

    printf("upgrade size = %d\n", upgradeSize);

#ifdef DEBUG_UPGRADE
    if( upgradeSize > 0 )
    {
	if( *imageBufPtrPtr )
	{
		dumpUpgradeHex((unsigned char *)*imageBufPtrPtr, (upgradeSize<64)?upgradeSize:64);
	}
    }  
#endif
  
    // parse the image data
    if (result == UPGRADE_OK)
    {
	*imageType = BROADCOM_IMAGE_FORMAT;
	*imageLen = upgradeSize;
	*fNeedReset = TRUE;
    }
    if (result != UPGRADE_OK && *fNeedReset == TRUE)
       result = UPGRADE_FATAL;
    printf("result = %d\n", result);
    return result;
}

int do_upgrade_pre1(FILE * stream, int upgrade_len, int upgrade_type)
{
	int status = WEB_STS_UPLOAD;
	httpd2sysmonitor_sysledCtrol(SYSLED_STS_BUSY);
	gradestatus = 0;
	gradejump = 0;
	glbUpgradeStatus = upGradeImage(stream, upgrade_len, upgrade_type, &imagePtr, &imageType, &imageLen, &fNeedReset);
	glbUpgradeMode = upgrade_type;
	if (glbUpgradeStatus != UPGRADE_OK)     // processes are not killed.  no filename...
	{
		http2dbs_writeOptlog(-1, "upgrading firmware image");
		//do_ej("/webs/uploadinfo.html", stream);
		status = WEB_STS_ERROR;
		httpd2sysmonitor_sysledCtrol(SYSLED_STS_NORMAL);
	}
	return status;
}

int do_upgrade_pre2(FILE * stream, int upgrade_len, int upgrade_type)
{
	int status = WEB_STS_UPLOAD;
	httpd2sysmonitor_sysledCtrol(SYSLED_STS_BUSY);
	gradestatus = 1;
	glbUpgradeStatus = upGradeImage(stream, upgrade_len, upgrade_type, &imagePtr, &imageType, &imageLen, &fNeedReset);
	glbUpgradeMode = upgrade_type;
	if (glbUpgradeStatus != UPGRADE_OK)     // processes are not killed.  no filename...
	{
		http2dbs_writeOptlog(-1, "upgrading firmware image");
		//do_ej("/webs/uploadinfo.html", stream);
		status = WEB_STS_ERROR;
		httpd2sysmonitor_sysledCtrol(SYSLED_STS_NORMAL);
	}
	return status;
}

int do_upgrade_pre3(FILE * stream, int upgrade_len, int upgrade_type)
{
	int status = WEB_STS_UPLOAD;
	httpd2sysmonitor_sysledCtrol(SYSLED_STS_BUSY);
	glbUpgradeStatus = upGradeImage(stream, upgrade_len, upgrade_type, &imagePtr, &imageType, &imageLen, &fNeedReset);
	glbUpgradeMode = upgrade_type;
	if (glbUpgradeStatus != UPGRADE_OK)     // processes are not killed.  no filename...
	{
		http2dbs_writeOptlog(-1, "upgrading firmware image");
		//do_ej("/webs/uploadinfo.html", stream);
		status = WEB_STS_ERROR;
		httpd2sysmonitor_sysledCtrol(SYSLED_STS_NORMAL);
	}
	return status;
}

int do_upgrade_post(void)
{
	int ret = UPGRADE_FATAL;	
	
	if (glbUpgradeStatus == UPGRADE_OK)
	{
		if (imageType == BROADCOM_IMAGE_FORMAT || imageType == FLASH_IMAGE_FORMAT)
		{
			printf("do_upgrade_post()\n");
			ret = flashUpgradeImage(imagePtr, imageType, imageLen);
		}
		else if (imageType == PSI_TEXT_FORMAT)
		{
			printf("BcmPsi_writeStreamToFlash");
		}		
	}
	do_upgrade_finnal();
	if (fNeedReset == TRUE)
	{
		//sysMipsSoftReset();
	}
	return ret;
}

void do_upgrade_finnal(void)
{
	if( NULL != imagePtr )
	{
		free(imagePtr);
		imagePtr = NULL;
		printf("do_upgrade_finnal\n");
		printf("upgrading process done\n");
	}
	httpd2sysmonitor_sysledCtrol(SYSLED_STS_NORMAL);
}

