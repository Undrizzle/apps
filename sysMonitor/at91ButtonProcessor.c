#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <public.h>
#include <dbsapi.h>
//#include "sm2dbsMutex.h"
#include "systemStsControl.h"
#include "sysMonitor2cmm.h"
#include "sysMonitor.h"
#include "at91ButtonProcessor.h"

static int at91btns_fd = 0;

/* 与DBS  通讯的设备文件*/
static T_DBS_DEV_INFO *dbsdev = NULL;

void at91btnsSyncHandler(int args)
{
	int tmp = 0;
	/* 通过系统调用获取系统状态*/
	if( ioctl(at91btns_fd, GET_KEY_STS_CMD, &tmp) >= 0 )
	{
		switch(tmp)
		{
			/* reset SIGIO */
			case 1:
			{
				set_systemStatus(SYSLED_STS_RESET);		/* sysled off */
				break;
			}
			/* restore-default SIGIO */
			case 2:
			{
				set_systemStatus(SYSLED_STS_BUSY);		/* sysled fast flash */
				break;
			}
			default:
			{
				set_systemStatus(SYSLED_STS_NORMAL);	/* sysled normal */
				break;
			}
		}		
		//printf("-->at91btnsSyncHandler : set_systemStatus(%d)\n", get_systemStatus());
	}
}

int init_at91btns(void)
{
	int oflags;

	at91btns_fd =  open( "/dev/at91btns", O_RDWR );
	if( at91btns_fd > 0 )
	{
		signal( SIGIO, &at91btnsSyncHandler );
		fcntl( at91btns_fd, F_SETOWN, getpid() );
		oflags = fcntl( at91btns_fd, F_GETFL );
		fcntl( at91btns_fd, F_SETFL, oflags|FASYNC );	
		return CMM_SUCCESS;
	}
	else
	{
		printf("init_at91btns failed : cannot open device driver\n");
		return CMM_FAILED;
	}
}

void *at91ButtonProcessor(void)
{
	int ret;
	int key_event = 0;
	fd_set rds;
	T_UDP_SK_INFO myCmmSk;

	/* DBS设计变更，支持多线程操作，初始化时在每个线程中独享消息接口*/
	/*创建与数据库模块互斥通讯的外部SOCKET接口*/
	dbsdev = dbsNoWaitOpen(MID_AT91BTN);
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: at91ButtonProcessor->dbsOpen error, exited !\n");
		return (void *)0;
	}
	
	if( CMM_SUCCESS != init_at91btns() )
	{
		fprintf(stderr, "at91buttonProcessor init_at91btns failed\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "at91buttonProcessor init_at91btns failed");
		dbsClose(dbsdev);
		return (void *)0;
	}
	else if( CMM_SUCCESS != sysMonitor2cmm_init(&myCmmSk) )
	{
		fprintf(stderr, "at91buttonProcessor sysMonitor2cmm_init failed\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "at91buttonProcessor sysMonitor2cmm_init failed");
		dbsClose(dbsdev);
		return (void *)0;
	}

	//printf("sysMonitor->at91buttonProcessor start\n");
	fprintf(stderr, "Starting thread at91ButtonProcessor	......	[OK]\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting thread at91buttonProcessor success");

	for (;;)
	{
		FD_ZERO(&rds);
		FD_SET(at91btns_fd, &rds);

		ret = select(at91btns_fd + 1, &rds, NULL, NULL, NULL);
		if (ret < 0) {
			perror("APP: select error\n");
			continue;
		}
		else if (ret == 0) {
			/*
			 * Actually it never come here, since we
			 * don't set timeout value in select()
			 */
			perror("APP: timeout\n");
			continue;
		}
		else if (FD_ISSET(at91btns_fd, &rds))
		{
			ret = read(at91btns_fd, &key_event, sizeof(key_event));
			if (ret != sizeof(key_event))
			{
				if (errno != EAGAIN)
					perror("APP: read key");
				continue;
			}
			else
			{
				/* 在这里添加按键事件处理逻辑*/
				//printf("\nat91buttonProcessor: key_event = %d\n", key_event);
				switch(key_event)
				{
					case 1:
					{
						/* reset system */
						set_systemStatus(SYSLED_STS_RESET);
						printf("\nat91buttonProcessor key_event: reset system.\n");
						if( CMM_SUCCESS != sysMonitor2cmm_sysReboot(&myCmmSk) )
						{
							perror("at91buttonProcessor->sysMonitor2cmm_sysReboot failed!\n");
						}
						break;
					}
					case 2:
					{
						/* restore default */
						set_systemStatus(SYSLED_STS_RESET);
						printf("\nat91buttonProcessor key_event: restore default settings.\n");
						if( CMM_SUCCESS != sysMonitor2cmm_restoreDefault(&myCmmSk) )
						{
							perror("at91buttonProcessor->sysMonitor2cmm_restoreDefault failed!\n");
						}
						break;
					}
					case 3:
					{
						/* start safe mode *//* do not support this case yet */
						set_systemStatus(SYSLED_STS_NORMAL);
						printf("\nat91buttonProcessor key_event: start safe mode.\n");
						break;
					}
					default:
					{
						/* do nothing */
						set_systemStatus(SYSLED_STS_NORMAL);
						printf("\nat91buttonProcessor key_event: nothing to do.\n");
						break;
					}
				}
				continue;
			}
		}
	}

	/* 不要在这个后面添加代码，执行不到滴*/
	printf("thread at91ButtonProcessor exit !\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "INFO: thread at91ButtonProcessor exit");	
	sysMonitor2cmm_destroy(&myCmmSk);
	dbsClose(dbsdev);
	return (void *)0;
}

