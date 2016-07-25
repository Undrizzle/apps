#include<public.h>
#include <dbsapi.h>
#include <fcntl.h>
#include "sysMonitor.h"
#include "systemStsControl.h"
#include "sysledProcessor.h"
//#include "sm2dbsMutex.h"

static int sysled_fd = 0;

/* 与DBS  通讯的设备文件*/
static T_DBS_DEV_INFO *dbsdev = NULL;

void set_sysled(int status)
{
	switch(status)
	{
		case 1:
		{
			/* turn on sysled */
			ioctl(sysled_fd, LED_ON_CMD, LEFT_LED);
			break;
		}
		default:
		{
			/* turn off sysled */
			ioctl(sysled_fd, LED_OFF_CMD, LEFT_LED);
			break;
		}
	}
}

void destroy_sysled(void)
{
	if( 0 != sysled_fd )
	{
		close(sysled_fd);
	}
	sysled_fd = 0;
}

int init_sysled(void)
{
	sysled_fd =  open( "/dev/at91btns", O_RDWR );
	if( sysled_fd > 0 )
	{
		return CMM_SUCCESS;
	}
	else
	{
		printf("init_sysled failed : cannot open device driver\n");
		return CMM_FAILED;
	}
}

void *sysledProcessor(void)
{
	int ledsts = 0;
	int fsgap = 800000;

	/* DBS设计变更，支持多线程操作，初始化时在每个线程中独享消息接口*/
	/*创建与数据库模块互斥通讯的外部SOCKET接口*/
	dbsdev = dbsNoWaitOpen(MID_SYSLED);
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: sysledProcessor->dbsOpen error, exited !\n");
		return (void *)0;
	}

	if( CMM_SUCCESS != init_sysled() )
	{
		fprintf(stderr, "sysledProcessor->init_sysled() failed\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "sysledProcessor->init_sysled() failed");
		dbsClose(dbsdev);
		return (void *)0;
	}
	
	fprintf(stderr, "Starting thread sysledProcessor 	......	[OK]\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting thread sysledProcessor success");
	
	for (;;)
	{
		switch(get_systemStatus())
		{
			/* 正常状态*/
			case SYSLED_STS_NORMAL:
			//case 1:
			{
				fsgap = 800000;
				if( 0 == ledsts )
				{
					set_sysled(1);
					ledsts = 1;
				}
				else
				{
					set_sysled(0);
					ledsts = 0;
				}				
				break;
			}
			/* 检测到系统忙状态 */
			case SYSLED_STS_BUSY:
			{
				fsgap = 100000;
				if( 0 == ledsts )
				{
					set_sysled(1);
					ledsts = 1;
				}
				else
				{
					set_sysled(0);
					ledsts = 0;
				}
				break;
			}
			/* 检测到系统停止运行状态 */
			case SYSLED_STS_RESET:
			{
				fsgap = 800000;
				set_sysled(0);
				ledsts = 0;
				break;
			}
		}
		usleep(fsgap);
	}

	/* 不要在这个后面添加代码，执行不到滴*/
	printf("thread sysledProcessor exit !\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "INFO: thread sysledProcessor exit");
	destroy_sysled();
	dbsClose(dbsdev);
	return (void *)0;
}

