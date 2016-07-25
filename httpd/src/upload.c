//*************************************************************************************
// Broadcom Corp. Confidential
// Copyright 2000, 2001, 2002 Broadcom Corp. All Rights Reserved.
//
// THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED
// SOFTWARE LICENSE AGREEMENT BETWEEN THE USER AND BROADCOM.
// YOU HAVE NO RIGHT TO USE OR EXPLOIT THIS MATERIAL EXCEPT
// SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
//
//**************************************************************************************
// File Name  : upload.c
//
// Description: perform a http firmware upload with image and tag validation system
//
// Created    : 02/28/2002  seanl
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
#define  RESULT_MSG_SIZE   120
#define  BUF_SIZE          2048
#define  FN_TOKEN          "filename="
#define  FN_TOKEN_LEN      sizeof(FN_TOKEN)
#define  END_MARK          "Content-Type:"
#define  UPLOAD_OVERHEAD   256
#define  MIN_UPLOAD_LEN    2 * 1024            // min. upload len if less than that, assume wrong file name.

#define FIRMWARE_BASE_ADDR 0x300000
#define FIRMWARE_BLOCK_SIZE 0x100000
#define SMLOAD_ERASE_SIZE (64*0x400)

unsigned char wecDigitalHeader[64] = 
{
	0x30, 0x71, 0xb2, 0x01, 0x00, 0x05, 0x07, 0x01, 0x08, 0x02, 0x05, 0x06, 0x05, 0x03, 0x00, 0x03,
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* HTTP upload image formats. */
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
    UPLOAD_OK,
    UPLOAD_FAIL_NO_MEM,
    UPLOAD_FAIL_NO_FILENAME,
    UPLOAD_FAIL_ILLEGAL_IMAGE,
    UPLOAD_FATAL,  // The state above this line will be mapped to a messge in uploadinfo.html
    UPLOAD_FAIL_FLASH = 20
} UPLOAD_RESULT;

static int imageLen = 0;
static int fNeedReset = FALSE;
static char *imagePtr = NULL;
static PARSE_RESULT imageType = NO_IMAGE_FORMAT;
extern int glbUploadMode;     // used for web page upload image or updating settings.

int checkCPU(void)
{
	union w
	{
		int a;
		char b;
	}c;
	c.a = 1;
	return (1 == c.b);
}

UINT32 psiGetCrc32(UINT8 *pdata, UINT32 size, UINT32 crc)
{
	return 0;
}

int glbUploadStatus;  // global variable that is used in cgimain.c

#define     DEBUG_UPLOAD            // if uncommented,  enable debug display
#ifdef DEBUG_UPLOAD

static const char hextable[16] = "0123456789ABCDEF";
void dumpHex(unsigned char *start, int len)
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

int extract_file(char *desFilePath, char *src, size_t len)
{
	FILE *fp = NULL;

	if( (fp = fopen(desFilePath, "w")) < 0 )
	{
		return CMM_FAILED;
	}
	else
	{
		fwrite(src, 1, len, fp);
		fclose(fp);
		return CMM_SUCCESS;
	}
}


// verify the tag
// return BOOL_FALSE: BOOL_TRUE
//
int verifyTag(char *stag, int len)
{
	if( memcmp(stag, wecDigitalHeader, 16) == 0 )
	{
		return BOOL_TRUE;
	}
	else
	{
		return BOOL_FALSE;
	}
}

int checkFile(void)
{
	md5_state_t state;
	md5_byte_t md5Tag[16] = {0};
	md5_byte_t md5Clc[16] = {0};
	//struct MD5Context md5c;
	
	if( imageLen < 80 )
	{
		do_upload_finnal();
		return BOOL_FALSE;
	}
	if( imagePtr == NULL )
	{
		do_upload_finnal();
		return BOOL_FALSE;
	}
	memcpy(md5Tag, imagePtr+64, 16);

	printf("boardapi_checkCpuEndian = %s\n", boardapi_checkCpuEndian()?"little-endian":"big-endian");
	
	//MD5Init( &md5c );
	//MD5Update( &md5c, imagePtr+80, imageLen-80 );
	//MD5Final( md5Clc, &md5c );

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)(imagePtr+80), (imageLen-80));
	md5_finish(&state, md5Clc);	

	printf("MD5 SRC:\n");
	dumpHex((unsigned char *)md5Tag, 16);
	printf("MD5 CLC:\n");
	dumpHex((unsigned char *)md5Clc, 16);

	if( memcmp(md5Tag, md5Clc, 16) == 0 )
	{
		if( CMM_SUCCESS != extract_file("/var/tmp/wec9720ek.tar.bz2", imagePtr+80, imageLen-80) )
		{
			do_upload_finnal();
			return BOOL_FALSE;
		}
		return BOOL_TRUE;
	}
	else
	{
		do_upload_finnal();
		return BOOL_FALSE;
	}    
}

int eraseFlash(void)
{
	printf("erase flash disk\n");
	printf("rm -rf /usr/mnt/app\n");
	system("rm -rf /usr/mnt/app &");
	printf("rm -rf /usr/mnt/html\n");
	system("rm -rf /usr/mnt/html &");
	return BOOL_TRUE;
}

// depending on the image type, do the brcm image or whole flash image
// return: UPLOAD_RESULT
UPLOAD_RESULT flashImage(char *imagePtr, PARSE_RESULT imageType, int imageLen)
{
	printf("tar jxvf /var/tmp/wec9720ek.tar.bz2 -C /usr/mnt/\n");
	system("tar jxvf /var/tmp/wec9720ek.tar.bz2 -C /usr/mnt/ >/dev/null");
	return UPLOAD_OK;
}


// parseImageData
// parse the image data to see if it Broadcom flash format or flash image format file
 
int parseImageData(void)
{	
    return BOOL_TRUE;
}


// upLoadImage
// process the image file uploading
// return: UPLOAD_RESULT
//
UPLOAD_RESULT upLoadImage(FILE *stream, int upload_len, int upload_type, char **imageBufPtrPtr,  
                          PARSE_RESULT *imageType, int *imageLen, int *fNeedReset)
{
    char buf[BUF_SIZE];
    char bound[BOUND_SIZE];
    char myTag[16] = {0};
    int boundSize = 0;
    unsigned int uploadSize = 0;
    int byteRd = 0;
    char *pdest = NULL;
    UPLOAD_RESULT result = UPLOAD_OK;
    char *curPtr = NULL;
    int flashSize = 0;

    bound[0] = '\0';
    buf[0] = '\0';
	
    fgets(bound, BOUND_SIZE, stream);

    boundSize = strlen(bound) - 2;

    if (boundSize <= 0)
    {
        printf("Failed to find bound !!!\n");
        return UPLOAD_FATAL;                 // fatal
    }

    bound[boundSize] = '\0';
    
    // get Content-Disposition: form-data; name="filename"; filename="test"
    // check filename, if "", user click no filename and report

    fgets(buf, BUF_SIZE, stream);   


    if ((pdest = strstr(buf, FN_TOKEN)) == NULL)
    {
        printf("No filename ? \n");
        return UPLOAD_FATAL;                 // fatal
    }
    else
    {
        pdest += (FN_TOKEN_LEN - 1);
        if (*pdest == '"' && *(pdest + 1) == '"')
        {
            printf("No filename selected\n");
            result = UPLOAD_FAIL_NO_FILENAME;
        }
        else
            printf("\nfilename = %s\n", pdest);
    }
    
    // get [Content-Type: application/octet-stream] and NL (cr/lf) and discard them

    fgets(buf, BUF_SIZE, stream);
    fgets(buf, BUF_SIZE, stream);
    //Get prevail digital signature
    fgets(buf, 17, stream);
    memcpy(myTag, buf, 16);

    // add some overhead for the w image and sub bound size from upload_len
    upload_len -= boundSize;
    flashSize = sysFlashSizeGet();
    if (upload_len > flashSize)
    {
        printf("Size is over by %d\n", upload_len - flashSize);
        result = UPLOAD_FAIL_ILLEGAL_IMAGE;
	 *fNeedReset = FALSE;
    }
    else if (upload_len < MIN_UPLOAD_LEN &&
             upload_type == WEB_UPLOAD_IMAGE)  // for fake file or just the path without file name.
    {
        if( UPLOAD_FAIL_NO_FILENAME != result )
	 {
		printf("Wrong file or illegal size [%d]\n", upload_len);
		result = UPLOAD_FAIL_ILLEGAL_IMAGE;
	 }
	 *fNeedReset = FALSE;
    }
    else if( verifyTag(myTag, 16) == BOOL_FALSE )
    {
        dumpHex((unsigned char *)myTag, 16);
        printf("Wrong file formet\n");
	 result = UPLOAD_FAIL_ILLEGAL_IMAGE;
	 *fNeedReset = FALSE;
    }
    else if (result == UPLOAD_OK)
    {
        printf("upload start with len = %d with flash size = %d ****\n", upload_len, flashSize);
        // only kill processes when upload image (but not for upload configurations)
        if ( upload_type == WEB_UPLOAD_IMAGE ) {
            bcmKillAllApps();
            printf("Done removing processes\n");
            *fNeedReset = TRUE;
        }
        printf("Allocating %d byte buffer\n", upload_len);
        curPtr = (char *) malloc(upload_len);
        if( curPtr == NULL )
        {
            printf("Failed to allocate memory for the image. Size required: [%d]\n", upload_len);
            result = UPLOAD_FAIL_NO_MEM;
        }
        else
        {
            *imageBufPtrPtr = curPtr;
            printf("Memory allocated\n");
	     memcpy(curPtr, myTag, 16);
            curPtr += 16;
	     uploadSize += 16;
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
                    uploadSize -= 2;
                    break;
                }
                else
                    printf("Failed finding bound ?\n");
            }
        }

        if (result != UPLOAD_OK)
            continue;               // just throw away the data and search for end of bound
        else  // move the data to the imageBuf
        {
            memcpy(curPtr, buf, byteRd);
            curPtr += byteRd;
            uploadSize += byteRd;
            if (uploadSize > upload_len)        // try not to over flow the buffer
            {
                printf("Failed on over sized image ?\n");
                result = UPLOAD_FAIL_ILLEGAL_IMAGE;
            }
        }
    }

    printf("upload size = %d\n", uploadSize);

#ifdef DEBUG_UPLOAD
    if( uploadSize > 0 )
    {
	if( *imageBufPtrPtr )
	{
		dumpHex((unsigned char *)*imageBufPtrPtr, (uploadSize<64)?uploadSize:64);
	}
    }  
#endif
  
    // parse the image data
    if (result == UPLOAD_OK)
    {
	*imageType = BROADCOM_IMAGE_FORMAT;
	*imageLen = uploadSize;
	*fNeedReset = TRUE;
    }
    if (result != UPLOAD_OK && *fNeedReset == TRUE)
        result = UPLOAD_FATAL;
    printf("result = %d\n", result);
    return result;
}



void endCfgUpdate(void)
{
   printf(" Resetting...\n");
   // un-initial network interface
  // BcmNtwk_unInit();
   //sysMipsSoftReset();
}

void sigUserCfgUpdate(char *xmlFileName) 
{
   
}



// do_upload
// do the image uploading.
// return: void
//
int do_upload_pre(FILE * stream, int upload_len, int upload_type)
{
	int status = WEB_STS_UPLOAD;
	httpd2sysmonitor_sysledCtrol(SYSLED_STS_BUSY);
	glbUploadStatus = upLoadImage(stream, upload_len, upload_type, &imagePtr, &imageType, &imageLen, &fNeedReset);
	glbUploadMode = upload_type;
	if (glbUploadStatus != UPLOAD_OK)     // processes are not killed.  no filename...
	{
		http2dbs_writeOptlog(-1, "upgrading firmware image");
		do_ej("/webs/uploadinfo.html", stream);
		status = WEB_STS_ERROR;
		httpd2sysmonitor_sysledCtrol(SYSLED_STS_NORMAL);
	}
	return status;
}

int do_upload_post(void)
{
	int ret = UPLOAD_FATAL;	
	
	if (glbUploadStatus == UPLOAD_OK)
	{
		if (imageType == BROADCOM_IMAGE_FORMAT || imageType == FLASH_IMAGE_FORMAT)
		{
			printf("do_upload_post()\n");
			ret = flashImage(imagePtr, imageType, imageLen);
		}
		else if (imageType == PSI_TEXT_FORMAT)
		{
			printf("BcmPsi_writeStreamToFlash");
		}		
	}
	do_upload_finnal();
	if (fNeedReset == TRUE)
	{
		//sysMipsSoftReset();
	}
	return ret;
}

void do_upload_finnal(void)
{
	if( NULL != imagePtr )
	{
		free(imagePtr);
		imagePtr = NULL;
		printf("do_upload_finnal\n");
		printf("uploading process done\n");
	}
	httpd2sysmonitor_sysledCtrol(SYSLED_STS_NORMAL);
}

