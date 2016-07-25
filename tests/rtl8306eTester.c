/********************************************************************************
* msApiInit.c
*
* DESCRIPTION:
*       MS API initialization routine
*
* DEPENDENCIES:   Platform
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/mii.h>

#include <public.h>
#include <dbsapi.h>
#include <wecplatform.h>
#include <rtk_api.h>


static T_DBS_DEV_INFO *dbsdev = NULL;

void rtl8306eTester_usage(void)
{
	printf("\nParameter error:\n");
	printf("\nUsage:\n");
	printf("rtl8306eTester <case>\n");
	printf("  --case/0/1: init vlan\n");	
	
	printf("\n\n");
}

void rtl8306eTester_signalProcessHandle(int n)
{
	printf("\n\n==================================================================\n");
	fprintf(stderr, "INFO: rtl8306eTester_signalProcessHandle close progress !\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "rtl8306eTester_signalProcessHandle : module rtl8306eTester exit");
	dbsClose(dbsdev);		
	exit(0);
}

int main(int argc, char *argv[])
{
	if( argc != 2 )
	{
		rtl8306eTester_usage();
		return 0;
	}

	dbsdev = dbsNoWaitOpen(MID_DSDT_TESTER);
	if( NULL == dbsdev )
	{
		return 0;
	}

	/* ×¢²áÒì³£ÍË³ö¾ä±úº¯Êý*/
	signal(SIGTERM, rtl8306eTester_signalProcessHandle);
	signal(SIGINT, rtl8306eTester_signalProcessHandle);

	dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting module rtl8306eTester success");
	printf("Starting module rtl8306eTester	......		[OK]\n\n");
	printf("\n==================================================================\n\n");

	if( strcmp(argv[1], "case/0/1") == 0)
	{
		printf("\n");
		
		if( 0 != rtk_vlan_init() )
		{
			goto DSDT_END;
		}	
		
	}
	else
	{
		rtl8306eTester_usage();
		goto DSDT_END;
	}
	
DSDT_END:
	
	printf("\n\n==================================================================\n\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "module rtl8306eTester exit");
	dbsClose(dbsdev);
	
	return 0;
	
}

