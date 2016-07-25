#include <public.h>
#include <dbsapi.h>
#include <fcntl.h>
#include <linux/watchdog.h>
//#include "sm2dbsMutex.h"
#include "wdtProcessor.h"

/* 与DBS  通讯的设备文件*/
static T_DBS_DEV_INFO *dbsdev = NULL;

#define WEC_WDT_TIMEOUT		15
#define WEC_WDT_FEED_INTVAL	5

static int wdt_fd = 0;

void destroy_wdt(void)
{
	if( 0 != wdt_fd )
	{
		close(wdt_fd);
	}
	wdt_fd = 0;
}

int init_wdt(void)
{
	wdt_fd =  open("/dev/watchdog", O_RDONLY );
	return (wdt_fd>0 )?CMM_SUCCESS:CMM_FAILED;
}

int wdt_set_timeout(int timeout)
{
	return ioctl(wdt_fd, WDIOC_SETTIMEOUT, &timeout);
}

void wdt_keep_alive(void)
{
	ioctl(wdt_fd, WDIOC_KEEPALIVE);
}

void *wdtProcessor(void)
{
	st_dbsSysinfo wecSysinfo;	

	/* DBS设计变更，支持多线程操作，初始化时在每个线程中独享消息接口*/
	/*创建与数据库模块互斥通讯的外部SOCKET接口*/
	dbsdev = dbsNoWaitOpen(MID_WDTIMER);
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: wdtProcessor->dbsOpen error, exited !\n");
		return (void *)0;
	}

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &wecSysinfo) )
	{
		fprintf(stderr, "wdtProcessor->dbsMutexGetSysinfo() failed\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "wdtProcessor->dbsMutexGetSysinfo() failed");
		dbsClose(dbsdev);
		return (void *)0;
	}

	if( !wecSysinfo.col_wdt )
	{
		fprintf(stderr, "INFO: wdtProcessor detect WDTimer disabled\n");
		dbs_sys_log(dbsdev, DBS_LOG_INFO, "wdtProcessor detect WDTimer disabled");
		dbsClose(dbsdev);
		return (void *)0;
	}

	if( CMM_SUCCESS != init_wdt() )
	{
		fprintf(stderr, "wdtProcessor->init_wdt() failed\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "wdtProcessor->init_wdt() failed");
		dbsClose(dbsdev);
		return (void *)0;
	}
	
	/* enable and set wdt timeout */
	if( wdt_set_timeout(WEC_WDT_TIMEOUT) < 0 )
	{
		fprintf(stderr,"wdtProcessor->wdt_set_timeout() failed\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "wdtProcessor->wdt_set_timeout() failed");
		destroy_wdt();
		dbsClose(dbsdev);
		return (void *)0;
	}

	fprintf(stderr, "Starting thread wdtProcessor 		......	[OK]\n");
	fprintf(stderr, "WDTimer enabled(timeout=%d)\n", WEC_WDT_TIMEOUT);
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting thread wdtProcessor success");
	/* read wdt timeout */
	//ioctl(wdt_fd,WDIOC_GETTIMEOUT, &wdt_timeout);		
	//printf("INFO: wdtProcoosr->WDIOC_GETTIMEOUT = %d\n", wdt_timeout);
	while(1)
	{
		wdt_keep_alive();
		sleep(WEC_WDT_FEED_INTVAL);
	}

	/* 不要在这个后面添加代码，执行不到滴*/
	printf("thread wdtProcessor exit !\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "INFO: thread wdtProcessor exit");
	destroy_wdt();
	dbsClose(dbsdev);
	return (void *)0;
} 


