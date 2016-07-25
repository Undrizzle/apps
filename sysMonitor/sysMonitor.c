#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <public.h>
#include <boardapi.h>
#include <dbsapi.h>

//#include "sm2dbsMutex.h"
#include "sysMonitor.h"
#include "at91ButtonProcessor.h"
#include "sysindiProcessor.h"
#include "sysledProcessor.h"
#include "wdtProcessor.h"
#include "atmProcessor.h"

/* ��DBS  ͨѶ���豸�ļ�*/
static T_DBS_DEV_INFO *dbsdev = NULL;

/********************************************************************************************
*	��������:sysMonitorSignalHandler
*	��������:ģ���쳣����������
*	����:frank
*	ʱ��:2010-08-13
*********************************************************************************************/
void sysMonitorSignalHandler(int n)
{
	printf("sysMonitorSignalHandler : module sysMonitor exit !\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "sysMonitorSignalHandler : module sysMonitor exit");
	destroy_systemStatusLock();
	dbsClose(dbsdev);
	exit(0);
}

int main(void)
{	
	int ret = 0;
	int fd = 0;
#ifdef __AT30TK175STK__
	pthread_t thread_id[5];
#else
	pthread_t thread_id[4];
#endif		

	/* DBS��Ʊ����֧�ֶ��̲߳�������ʼ��ʱ��ÿ���߳��ж�����Ϣ�ӿ�*/
	/*���������ݿ�ģ�黥��ͨѶ���ⲿSOCKET�ӿ�*/
	dbsdev = dbsOpen(MID_SYSMONITOR);
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: sysMonitor->dbsOpen error, exited !\n");
		return CMM_CREATE_SOCKET_ERROR;
	}

	/* ��ʼ��g_systemStatus ������ʵĻ�����*/
	init_systemStatusLock();

	/* ע���쳣�˳��������*/
	signal(SIGTERM, sysMonitorSignalHandler);

	/* Waiting for all modus init */
	dbsWaitModule(dbsdev, MF_MMEAD|MF_ALARM|MF_TM|MF_CMM|MF_REGI);
	//dbsWaitModule(dbsdev, MF_MMEAD);

	/* ��������߳�*/
	ret = pthread_create( &thread_id[0], NULL, (void *)at91ButtonProcessor, NULL );
	if( ret == -1 )
	{
		fprintf(stderr, "Cannot create thread at91buttonProcessor\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "Cannot create thread at91buttonProcessor");
		destroy_systemStatusLock();
		dbsClose(dbsdev);
		return CMM_FAILED;
	}
	/* ϵͳ�ƹ����߳�*/
	ret = pthread_create( &thread_id[1], NULL, (void *)sysledProcessor, NULL );
	if( ret == -1 )
	{
		fprintf(stderr, "Cannot create thread sysledProcessor\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "Cannot create thread sysledProcessor");
		destroy_systemStatusLock();
		dbsClose(dbsdev);
		return CMM_FAILED;
	}
	/* WDT ����߳�*/
	ret = pthread_create( &thread_id[2], NULL, (void *)wdtProcessor, NULL );
	if( ret == -1 )
	{
		fprintf(stderr, "Cannot create thread wdtProcessor\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "Cannot create thread wdtProcessor");
		destroy_systemStatusLock();
		dbsClose(dbsdev);
		return CMM_FAILED;
	}	
	/* �ⲿ������Ϣ��Ӧ�߳�*/
	ret = pthread_create( &thread_id[3], NULL, (void *)sysindiProcessor, NULL );
	if( ret == -1 )
	{
		fprintf(stderr, "Cannot create thread sysindiProcessor\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "Cannot create thread sysindiProcessor");
		destroy_systemStatusLock();
		dbsClose(dbsdev);
		return CMM_FAILED;
	}

#ifdef __AT30TK175STK__
	/* ϵͳ������ؽ���*/
	ret = pthread_create( &thread_id[4], NULL, (void *)atmProcessor, NULL );
	if( ret == -1 )
	{
		fprintf(stderr, "Cannot create thread atmProcessor\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "Cannot create thread atmProcessor");
		destroy_systemStatusLock();
		dbsClose(dbsdev);
		return CMM_FAILED;
	}
#endif	

#ifdef __AT30TK175STK__
	pthread_join( thread_id[4], NULL );
#endif
	pthread_join( thread_id[3], NULL );
	pthread_join( thread_id[2], NULL );
	pthread_join( thread_id[0], NULL );
	pthread_join( thread_id[1], NULL );	

	/* ��Ҫ�����������Ӵ��룬ִ�в�����*/
	printf("module sysMonitor exit !\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "INFO: module sysMonitor exit");
	destroy_systemStatusLock();
	dbsClose(dbsdev);
	return 0;
}


