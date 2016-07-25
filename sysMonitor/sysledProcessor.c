#include<public.h>
#include <dbsapi.h>
#include <fcntl.h>
#include "sysMonitor.h"
#include "systemStsControl.h"
#include "sysledProcessor.h"
//#include "sm2dbsMutex.h"

static int sysled_fd = 0;

/* ��DBS  ͨѶ���豸�ļ�*/
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

	/* DBS��Ʊ����֧�ֶ��̲߳�������ʼ��ʱ��ÿ���߳��ж�����Ϣ�ӿ�*/
	/*���������ݿ�ģ�黥��ͨѶ���ⲿSOCKET�ӿ�*/
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
			/* ����״̬*/
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
			/* ��⵽ϵͳæ״̬ */
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
			/* ��⵽ϵͳֹͣ����״̬ */
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

	/* ��Ҫ�����������Ӵ��룬ִ�в�����*/
	printf("thread sysledProcessor exit !\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "INFO: thread sysledProcessor exit");
	destroy_sysled();
	dbsClose(dbsdev);
	return (void *)0;
}

