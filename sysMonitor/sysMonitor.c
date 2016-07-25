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

/* 与DBS  通讯的设备文件*/
static T_DBS_DEV_INFO *dbsdev = NULL;

/********************************************************************************************
*	函数名称:sysMonitorSignalHandler
*	函数功能:模块异常处理句柄函数
*	作者:frank
*	时间:2010-08-13
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

	/* DBS设计变更，支持多线程操作，初始化时在每个线程中独享消息接口*/
	/*创建与数据库模块互斥通讯的外部SOCKET接口*/
	dbsdev = dbsOpen(MID_SYSMONITOR);
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: sysMonitor->dbsOpen error, exited !\n");
		return CMM_CREATE_SOCKET_ERROR;
	}

	/* 初始化g_systemStatus 互斥访问的互斥锁*/
	init_systemStatusLock();

	/* 注册异常退出句柄函数*/
	signal(SIGTERM, sysMonitorSignalHandler);

	/* Waiting for all modus init */
	dbsWaitModule(dbsdev, MF_MMEAD|MF_ALARM|MF_TM|MF_CMM|MF_REGI);
	//dbsWaitModule(dbsdev, MF_MMEAD);

	/* 按键检测线程*/
	ret = pthread_create( &thread_id[0], NULL, (void *)at91ButtonProcessor, NULL );
	if( ret == -1 )
	{
		fprintf(stderr, "Cannot create thread at91buttonProcessor\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "Cannot create thread at91buttonProcessor");
		destroy_systemStatusLock();
		dbsClose(dbsdev);
		return CMM_FAILED;
	}
	/* 系统灯管理线程*/
	ret = pthread_create( &thread_id[1], NULL, (void *)sysledProcessor, NULL );
	if( ret == -1 )
	{
		fprintf(stderr, "Cannot create thread sysledProcessor\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "Cannot create thread sysledProcessor");
		destroy_systemStatusLock();
		dbsClose(dbsdev);
		return CMM_FAILED;
	}
	/* WDT 监管线程*/
	ret = pthread_create( &thread_id[2], NULL, (void *)wdtProcessor, NULL );
	if( ret == -1 )
	{
		fprintf(stderr, "Cannot create thread wdtProcessor\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "Cannot create thread wdtProcessor");
		destroy_systemStatusLock();
		dbsClose(dbsdev);
		return CMM_FAILED;
	}	
	/* 外部请求消息响应线程*/
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
	/* 系统环境监控进程*/
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

	/* 不要在这个后面添加代码，执行不到滴*/
	printf("module sysMonitor exit !\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "INFO: module sysMonitor exit");
	destroy_systemStatusLock();
	dbsClose(dbsdev);
	return 0;
}


