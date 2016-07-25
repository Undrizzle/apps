#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#include <net/route.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/mii.h>
#include<ctype.h>
 
#include <md5.h>
//#include "dbsInterface.h"
//#include "smtraps.h"
//#include "smftp.h"
#include <public.h>
#include <dbsapi.h>
//#include <mtd-abi.h>

/* 与DBS  通讯的设备文件*/
extern T_DBS_DEV_INFO *dbsdev;
/* for debug */
//int test_return_code = 0;

extern T_DBS_DEV_INFO *dbsdev;

///////////////////////////////////////////////////////////////////////////////////////
unsigned char wecDigitalHeader[64] = 
{
	0x30, 0x71, 0xb2, 0x01, 0x00, 0x05, 0x07, 0x01, 0x08, 0x02, 0x05, 0x06, 0x05, 0x03, 0x00, 0x03,
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

///////////////////////////////////////////////////////////////////////////////////////
//UPLOAD_ERROR_CODE g_smload_status = RC_UPG_OK;
UPLOAD_ERROR_CODE  SM_LAST_UPLOAD_STATUS = RC_UPG_OK;


char *SMFTP_IMG_BUF_PTR = NULL;
size_t SMFTP_IMG_BUF_SIZE = 0;

#ifndef FTP_BUFSIZ
#define	FTP_BUFSIZ		0x400
#endif

sigjmp_buf context_sigalarm;

#define TIMEOUT_CONNECT 5

#define MAX_IMAGE_SIZE (2*1024*1024)

typedef struct ftp_host_info_s 
{
	char user[64];
	char password[64];
	struct sockaddr_in *s_in;
} ftp_host_info_t;

void sigalarm_handler(int useless)
{
	siglongjmp(context_sigalarm, 1);
}

int xconnect(struct sockaddr_in *s_addr)
{
	int s = socket(AF_INET, SOCK_STREAM, 0);
	signal(SIGALRM, sigalarm_handler);
	if (sigsetjmp(context_sigalarm, 1) == 0)
	{
		alarm(TIMEOUT_CONNECT);
		if (connect(s, (struct sockaddr*)s_addr, sizeof(struct sockaddr_in)) < 0)
		{
			alarm(0);
			//fprintf(stderr, "Unable to connect to remote host (%s)\n", inet_ntoa(s_addr->sin_addr));
	        	close(s);
	        	return -1;
		}
		alarm(0);
	}
	else
	{
		//fprintf(stderr, "Unable to connect to remote host, timeout catched (%s)\n", inet_ntoa(s_addr->sin_addr));
		close(s);
		return -1;
	}
	return s;
}

int ftpcmd(const char *s1, const char *s2, FILE *stream, char *buf)
{
	if (s1) 
	{
		if (s2) 
		{
			fprintf(stream, "%s%s\r\n", s1, s2);
		} 
		else 
		{
			fprintf(stream, "%s\r\n", s1);
		}
	}
	do 
	{
		char *buf_ptr;
		if (fgets(buf, 510, stream) == NULL) 
		{
			//printf("fgets()\n");
			return 0;
		}
		buf_ptr = strstr(buf, "\r\n");
		if (buf_ptr) 
		{
			*buf_ptr = '\0';
		}
	} while (! isdigit(buf[0]) || buf[3] != ' ');

	return atoi(buf);
}

int connect_ftpdata(ftp_host_info_t *server, const char *buf)
{
	char *buf_ptr;
	unsigned short port_num;

	buf_ptr = strrchr(buf, ',');
	*buf_ptr = '\0';
	port_num = atoi(buf_ptr + 1);

	buf_ptr = strrchr(buf, ',');
	*buf_ptr = '\0';
	port_num += atoi(buf_ptr + 1) * 256;

	server->s_in->sin_port=htons(port_num);
	return(xconnect(server->s_in));
}

unsigned int bb_copyfd_mem(int src_fd, const size_t filesize, int flag)
{
	char *buffer = NULL;
	char *p = NULL;
	size_t read_total = 0;
	int size;
	size_t read_try;
	ssize_t read_actual;
	//FILE *fp = NULL;

	SMFTP_IMG_BUF_SIZE = filesize;
	SMFTP_IMG_BUF_PTR = (char *)malloc(SMFTP_IMG_BUF_SIZE);
	if( NULL == SMFTP_IMG_BUF_PTR )
	{				
		SM_LAST_UPLOAD_STATUS = RC_UPG_NOT_ENOUGH_MEMORY;
		return 0;
	}
	

	buffer = (char *)malloc(FTP_BUFSIZ);
	p = SMFTP_IMG_BUF_PTR;
	
	if( NULL == buffer )
	{
		//printf("ERROR: bb_copyfd_mem malloc failed.\n");
		SM_LAST_UPLOAD_STATUS = RC_UPG_NOT_ENOUGH_MEMORY;
		return 0;
	}
	
	if (filesize) 
	{
#if 0
		size = filesize;
		printf("\nfilesize = %d\n", filesize);
		read_try = FTP_BUFSIZ;
		while( FTP_BUFSIZ == read_try ) 
		{
			if ( filesize - read_total > FTP_BUFSIZ ) 
			{
				read_try = FTP_BUFSIZ;
			} 
			else 
			{
				read_try = filesize - read_total;
			}

			read_actual = read(src_fd, (buffer), read_try);
			printf("\nread_actual = %d\n", read_actual);
			if( read_actual == read_try )
			{
				memcpy(p, buffer, read_actual);
				p += read_actual;
			}
			else
			{
				SM_LAST_UPLOAD_STATUS = RC_UPG_FILE_DOWNLOAD_ERROR;
				break;
			}			
			read_total += read_actual;
		}	
#else
		size = filesize;
		//printf("\nfilesize = %d\n", filesize);
		while( size > 0 )
		{
			read_actual = read(src_fd, (buffer), FTP_BUFSIZ);
			//printf("\nread_actual = %d\n", read_actual);
			if( read_actual > 0 )
			{
				memcpy(p, buffer, read_actual);
				p += read_actual;
				size -= read_actual;
				read_total += read_actual;
			}
		}
		if( 0 != size )
		{
			SM_LAST_UPLOAD_STATUS = RC_UPG_FILE_DOWNLOAD_ERROR;
		}
		else
		{
			SM_LAST_UPLOAD_STATUS = RC_UPG_OK;
		}
#endif
	}
	//printf("image_write total = 0x%x\n", read_total);
	//fclose(fp);
	free(buffer);
	return(read_total);
	//return(filesize);
}

FILE *ftp_login(ftp_host_info_t *server)
{
	FILE *control_stream = NULL;
	char buf[512];
	int fd;

	/* Connect to the command socket */
	if((fd = xconnect(server->s_in)) < 0)
		return NULL;
	
	if(!(control_stream = fdopen(fd, "r+")))
	{
		fprintf(stderr, "Couldnt open control stream\n");
		return NULL;
	}

	if (ftpcmd(NULL, NULL, control_stream, buf) != 220) 
	{
		//printf("%s\n", buf + 4);
		return NULL;
	}
	else
	{
		/*  Login to the server */
		switch (ftpcmd("USER ", server->user, control_stream, buf)) 
		{
			case 230:
				break;
			case 331:
				if (ftpcmd("PASS ", server->password, control_stream, buf) != 230) 
				{
					//printf("PASS error: %s\n", buf + 4);
					fclose(control_stream);
					control_stream = NULL;
				}
				break;
		default:
			//printf("USER error: %s\n", buf + 4);
			fclose(control_stream);
			control_stream = NULL;
		}

		if (control_stream)
			ftpcmd("TYPE I", NULL, control_stream, buf);

		return(control_stream);
	}
}

UPLOAD_ERROR_CODE ftp_receive(ftp_host_info_t *server, FILE *control_stream, const char *server_path, int flag)
{
	char buf[512];
	off_t filesize = 0;
	int fd_data;

	/* Connect to the data socket */
	if (ftpcmd("PASV", NULL, control_stream, buf) != 227) 
	{
		//printf("PASV error: %s\n", buf + 4);
		SM_LAST_UPLOAD_STATUS = RC_UPG_SERVER_UNREACHABLE;
		return SM_LAST_UPLOAD_STATUS;
	}
	else
	{
		fd_data = connect_ftpdata(server, buf);

		if (ftpcmd("SIZE ", server_path, control_stream, buf) == 213) 
		{
			filesize = strtoul(buf + 4,NULL,0);
			/* limit the max image size */
			if( filesize > MAX_IMAGE_SIZE)
			{
				SM_LAST_UPLOAD_STATUS = RC_UPG_FILE_INVALID;
				close(fd_data);
				return SM_LAST_UPLOAD_STATUS;
			}
		}
		if (ftpcmd("RETR ", server_path, control_stream, buf) > 150) 
		{
			SM_LAST_UPLOAD_STATUS = RC_UPG_FILE_DOESNT_EXIST;
			//printf("RETR error: %s\n", buf + 4);
			close(fd_data);
			return SM_LAST_UPLOAD_STATUS;
		}		
		
		/* Copy the file */
		if (bb_copyfd_mem(fd_data, filesize, flag) != filesize) 
		{
			printf("bb_copyfd_mem error\n");
			//SM_LAST_UPLOAD_STATUS = RC_UPG_GENERIC_ERROR;
			close(fd_data);
			return SM_LAST_UPLOAD_STATUS;
		}

		//printf("OK!\n");
		//printf("image_write total = 0x%x\n", filesize);

		/* close it all down */
		close(fd_data);
		if (ftpcmd(NULL, NULL, control_stream, buf) != 226) 
		{
			printf("ftp error: %s\n", buf + 4);
			SM_LAST_UPLOAD_STATUS = RC_UPG_GENERIC_ERROR;
			return SM_LAST_UPLOAD_STATUS;
		}
		if(!flag)
			ftpcmd("QUIT", NULL, control_stream, buf);

		return RC_UPG_OK;
	}
}

UPLOAD_ERROR_CODE ftp_download_image(void)
{
	ftp_host_info_t server;
	struct sockaddr_in s_in;
	FILE *control_stream;
	
	st_dbsSwmgmt ftp_info;
	ftp_info.id = 1;
	int try_times = 3;

	SM_LAST_UPLOAD_STATUS = RC_UPG_ON_GOING;

	/* 从数据库读取FTP 服务器的访问信息*/
	if( CMM_SUCCESS == dbsGetSwmgmt(dbsdev, 1, &ftp_info) )
	{		
		strcpy(server.user, ftp_info.col_user);
		memset(server.password, 0x00, 64);
		strcpy(server.password, ftp_info.col_pwd);
		server.s_in = &s_in;
		memset(&s_in, 0, sizeof(struct sockaddr_in));
		s_in.sin_family = AF_INET;
		s_in.sin_addr.s_addr = inet_addr(ftp_info.col_ip);
		s_in.sin_port = htons(ftp_info.col_port);
		/* 测试FTP 服务器的网络情况，如果连续3 次*
		** 与服务器握手失败，则自动启动正常模式*/
		while(try_times)
		{
			if ( NULL != ( control_stream = ftp_login(&server) ) )
			{
				break;
			}
			try_times--;
			sleep(3);
		}
		if(try_times)
		{
			/* 下载升级文件*/
			SM_LAST_UPLOAD_STATUS = ftp_receive(&server, control_stream, ftp_info.col_path, 0);
			/*
			if ( ftp_receive(&server, control_stream, ftp_info.col_path, 0) >= 0 )
			{
				SM_LAST_UPLOAD_STATUS = RC_UPG_OK;
			}
			else
			{
				SM_LAST_UPLOAD_STATUS = RC_UPG_FILE_DOESNT_EXIST;
				//fclose(control_stream);
				//return SM_LAST_UPLOAD_STATUS;
			}*/		
			fclose(control_stream);			
		}
		else
		{
			SM_LAST_UPLOAD_STATUS = RC_UPG_SERVER_UNREACHABLE;
		}
	}
	else
	{
		SM_LAST_UPLOAD_STATUS = RC_UPG_READ_DBS_ERROR;		
	}
	return SM_LAST_UPLOAD_STATUS;
}

char *get_ftp_imgptr(void)
{
	return SMFTP_IMG_BUF_PTR;
}

size_t get_ftp_imglen(void)
{
	return SMFTP_IMG_BUF_SIZE;
}

void free_ftp_imgptr(void)
{
	if( NULL != SMFTP_IMG_BUF_PTR )
		free(SMFTP_IMG_BUF_PTR);
	SMFTP_IMG_BUF_SIZE = 0;
}

int verify_image_tag(char *stag, int len)
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

UPLOAD_ERROR_CODE check_image(void)
{
	int i = 0;
	md5_state_t state;
	md5_byte_t digest[16];
	md5_byte_t digest_r[16];
	//FILE *fp = NULL;
	char *s = NULL;
	uint32_t len = 0;
	char abuf[33]={'\0'};
	char bbuf[33]={'\0'};
	char tmp[3]={'\0'};

	//printf("check_image\n");
	s = get_ftp_imgptr();
	if( NULL == s )
	{		
		//g_smload_status = RC_UPG_GENERIC_ERROR;
		return RC_UPG_GENERIC_ERROR;
	}
	else if( verify_image_tag(s, 16) == BOOL_FALSE )
	{		
		//g_smload_status = RC_UPG_FILE_TAG_ERROR;
		return RC_UPG_FILE_TAG_ERROR;
	}
	else
	{
		memcpy(digest_r, s + 64, 16);
		s += 80;
		len = get_ftp_imglen() - 80;
	}

	/* 文件长度必须是4的整数倍*/
	/*
	if( 0 != (len%4) )
	{
		//g_smload_status = RC_UPG_FILE_INVALID;
		return RC_UPG_FILE_INVALID;
	}	*/

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)s, len);
	md5_finish(&state, digest);	
	

	for( i=0; i<16; i++ )
	{
		sprintf(tmp,"%02x", digest[i] );
		strcat(abuf,tmp);
	}
	for( i=0; i<16; i++ )
	{
		sprintf(tmp,"%02x", digest_r[i] );
		strcat(bbuf,tmp);
	}

	if(memcmp(digest, digest_r, 16) == 0 )
	{
		if( CMM_SUCCESS != extract_file("/var/tmp/wec9720ek.tar.bz2", get_ftp_imgptr()+80, get_ftp_imglen()-80) )
		{
			return RC_UPG_FILE_INVALID;
		}
		return RC_UPG_OK;
	}
	else
	{
		//g_smload_status = RC_UPG_MD_CHECK_ERROR;
		return RC_UPG_MD_CHECK_ERROR;
	}	
}


int erase_flash(void)
{
	//printf("erase flash disk\n");
	//printf("rm -rf /usr/mnt/app\n");
	system("rm -rf /usr/mnt/app >/dev/null");
	//printf("rm -rf /usr/mnt/html\n");
	system("rm -rf /usr/mnt/html >/dev/null");
	return RC_UPG_OK;
}

int write_image_to_flash(void)
{
	//printf("write_image_to_flash\n");
	//printf("tar jxvf /var/tmp/wec9720ek.tar.bz2 -C /usr/mnt/\n");
	system("tar jxvf /var/tmp/wec9720ek.tar.bz2 -C /usr/mnt/ >/dev/null");
	return RC_UPG_OK;
}

int processCliUpgrade(void)
{
	int ret = CMM_SUCCESS;
	int opt_sts = RC_UPG_OK;

	/* 通知服务器，我已经开始升级*/
	//cmm2alarm_upgradeNotification(RC_UPG_ON_GOING);	

	/* 设置系统灯为忙状态*/
	if( CMM_SUCCESS != (ret = cmm2sysmonitor_sysledCtrol(SYSLED_STS_BUSY)) )
	{
		opt_sts = RC_UPG_SYS_GENERIC_IO_ERROR;
		goto processCliUpgrade_end;
	}

	/* kill processor */
	system("killall snmpd >/dev/null");
	system("killall httpd >/dev/null");
	system("killall register >/dev/null");
	system("killall mmead >/dev/null");
	system("killall tmcore >/dev/null");
	
	/* 从FTP  服务器下载升级文件*/
	if( CMM_SUCCESS != (ret = ftp_download_image()) )
	{
		opt_sts = ret;
		goto processCliUpgrade_end;
	}

	/* 校验电子签名并比较MD5 值*/
	if( CMM_SUCCESS != (ret = check_image()) )
	{
		opt_sts = ret;
		goto processCliUpgrade_end;
	}

	/* erase flash */
	if( CMM_SUCCESS != (ret = erase_flash()) )
	{
		opt_sts = RC_UPG_FLASH_ERASE_ERROR;
		goto processCliUpgrade_end;
	}

	/* 写入flash 镜像*/
	if( CMM_SUCCESS != (ret = write_image_to_flash()) )
	{
		opt_sts = RC_UPG_FLASH_WRITE_ERROR;
		goto processCliUpgrade_end;
	}

processCliUpgrade_end:

	/* free image buffer pointer */
	free_ftp_imgptr();
	
	/* 发送告警信息*/
	//cmm2alarm_upgradeNotification(ret);
	
	/* turn off led to indicate process done and the system is about to reset */
	cmm2sysmonitor_sysledCtrol(SYSLED_STS_RESET);
	
	return opt_sts;
	//return test_return_code++;
}

/*****************************************************************************************/
/* 函数功能 :从SNMP远程升级设备的后台处理函数                        */
/*****************************************************************************************/
int processSnmpUpgrade(void)
{
	int ret = CMM_SUCCESS;
	int opt_sts = RC_UPG_OK;
	
	/* 通知服务器，我已经开始升级*/
	cmm2alarm_upgradeNotification(RC_UPG_ON_GOING);
	
	/* 设置系统灯为忙状态*/
	if( CMM_SUCCESS != (ret = cmm2sysmonitor_sysledCtrol(SYSLED_STS_BUSY)) )
	{
		printf("processSnmpUpgrade()->cmm2sysmonitor_sysledCtrol() error.\n");
		opt_sts = RC_UPG_SYS_GENERIC_IO_ERROR;
		goto processSnmpUpgrade_end;
	}

	/* kill processor */
	system("killall cli >/dev/null");
	system("killall httpd >/dev/null");
	system("killall register >/dev/null");
	system("killall mmead >/dev/null");
	system("killall tmcore >/dev/null");
	
	/* 从FTP  服务器下载升级文件*/
	if( CMM_SUCCESS != (ret = ftp_download_image()) )
	{
		printf("processSnmpUpgrade()->ftp_download_image() error.\n");
		opt_sts = ret;
		goto processSnmpUpgrade_end;
	}

	/* 校验电子签名并比较MD5 值*/
	if( CMM_SUCCESS != (ret = check_image()) )
	{
		printf("processSnmpUpgrade()->check_image() error.\n");
		opt_sts = ret;
		goto processSnmpUpgrade_end;
	}

	/* erase flash */
	if( CMM_SUCCESS != (ret = erase_flash()) )
	{
		printf("processSnmpUpgrade()->erase_flash() error.\n");
		opt_sts = RC_UPG_FLASH_ERASE_ERROR;
		goto processSnmpUpgrade_end;
	}

	/* 写入flash 镜像*/
	if( CMM_SUCCESS != (ret = write_image_to_flash()) )
	{
		printf("processSnmpUpgrade()->write_image_to_flash() error.\n");
		opt_sts = RC_UPG_FLASH_WRITE_ERROR;
		goto processSnmpUpgrade_end;
	}
	
processSnmpUpgrade_end:

	/* free image buffer pointer */
	free_ftp_imgptr();
	
	/* 发送升级状态告警信息*/
	cmm2alarm_upgradeNotification(opt_sts);

	/* turn off led to indicate process done and the system is about to reset */
	cmm2sysmonitor_sysledCtrol(SYSLED_STS_RESET);

	/* restart system */
	cmm2alarm_sendCbatResetNotification();
	if( opt_sts )
	{
		printf("\nprocessSnmpUpgrade failed, error_code = %d.\n", opt_sts);
		printf("System is restarting now...\n");
	}
	else
	{
		printf("\nprocessSnmpUpgrade done success.\n");
		printf("System is restarting now...\n");
	}
	sleep(1);
	system("reboot");
	
	return opt_sts;
}

