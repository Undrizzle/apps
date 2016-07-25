#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "md5-little-endian.h"

//#define FILE_PATH "firmware.img"
#define TAR_FILE_PATH	"wec9720ek.tar.bz2"
#define IMAGE_NAME		"wec9720ek-s.web"

unsigned char wecDigitalHeader[64] = 
{
	0x30, 0x71, 0xb2, 0x01, 0x00, 0x05, 0x07, 0x01, 0x08, 0x02, 0x05, 0x06, 0x05, 0x03, 0x00, 0x03,
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

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
 
int main( int argc, char **argv )
{
	struct MD5Context md5c;
	char *s;
	uint32 len = 0;
	unsigned char ss[16] = {0};
	char buf[33]={'\0'};
	char tmp[3]={'\0'};
	int i;
	FILE *fp = NULL;   
	FILE *fpweb = NULL; 
	
	/* 判断文件是否存在*/
	if( access(TAR_FILE_PATH, 0) )
	{
		fprintf(stderr, "ERROR: file %s does not existed\n", TAR_FILE_PATH);
		return 0;
	}
	else if( (fp = fopen(TAR_FILE_PATH, "rb")) < 0 )
	{
		return 0;
	}
	else
	{
		fseek(fp, 0L, SEEK_END);
		len = ftell(fp);
		fseek(fp, 0L, SEEK_SET);

		s = (char *)malloc(len);
		if( NULL == s )
		{
			fclose(fp);
			return 0;
		}
		else if( fread(s, 1, len, fp) < len )
		{
			free(s);
			fclose(fp);
			return 0;	
		}
		else
		{
			MD5Init( &md5c );
		  	MD5Update( &md5c, s, len );
		  	MD5Final( ss, &md5c );
			for( i=0; i<16; i++ )
			{
				sprintf(tmp,"%02x", ss[i] );
				strcat(buf,tmp);
			}
			printf("checkCPU = %s\n", checkCPU()?"little-endian":"big-endian");
			printf("MD5: %s\n",buf);

			if( (fpweb = fopen(IMAGE_NAME, "w")) < 0 )
			{
				fprintf(stderr, "ERROR: file %s can not be created!\n", IMAGE_NAME);
			}
			else
			{
				fwrite(wecDigitalHeader, 1, 64, fpweb);
				fwrite(ss, 1, 16, fpweb);
				fwrite(s, 1, len, fpweb);
				fclose(fpweb);
			}
			
			free(s);
			fclose(fp);			
		}		
	}
	
	return 0;
}


