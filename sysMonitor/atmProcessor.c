#include <public.h>
#include <dbsapi.h>
//#include "sm2dbsMutex.h"
#include "sysMonitor2cmm.h"
#include "sysMonitor2alarm.h"
#include "atmProcessor.h"

typedef struct
{
	st_temperature last_atm_value;
	int cur_atm_alarm_sts;
	int atm_read_error;
}st_atm_status;

/* 描述当前环境温度告警状态的变量*/
st_atm_status g_cur_sts;

/* 该线程与CMM模块通讯的消息接口*/
static T_UDP_SK_INFO atmCmmSk;

/* 该线程与Alarm模块通讯的消息接口*/
static T_UDP_SK_INFO atmAlarmSk;

/* 与DBS  通讯的设备文件*/
static T_DBS_DEV_INFO *dbsdev = NULL;

int get_atm_alarm_level(st_temperature *curTemp)
{
	if( 0 == curTemp->sign )
	{
		if( curTemp->itemp >= SYS_TEMPERATURE_ALARM_HIHI )
		{
			return ALARM_STS_HIHI;
		}
		else if( curTemp->itemp >= SYS_TEMPERATURE_ALARM_HI )
		{
			return ALARM_STS_HI;
		}
		else
		{
			return ALARM_STS_NOMINAL;
		}
	}
	else
	{
		if( curTemp->itemp >= SYS_TEMPERATURE_ALARM_LOLO )
		{
			return ALARM_STS_LOLO;
		}
		else if( curTemp->itemp >= SYS_TEMPERATURE_ALARM_LO )
		{
			return ALARM_STS_LO;
		}
		else
		{
			return ALARM_STS_NOMINAL;
		}
	}
}

/* 读取系统环境温度，并做告警管理*/
int atmHandler(void)
{
	st_temperature atm_data;
	T_UDP_SK_INFO *pAtm2CmmSk = &atmCmmSk;
	T_UDP_SK_INFO *pAtm2AlarmSk = &atmAlarmSk;
	
	if( CMM_SUCCESS == sysMonitor2cmm_getCbatTemperature(pAtm2CmmSk, &atm_data) )
	{
		/* 复位环境温度读取错误标志位*/
		g_cur_sts.atm_read_error = 0;

		/* 如果之前的告警状态为正常态*/
		if( ALARM_STS_NOMINAL == g_cur_sts.cur_atm_alarm_sts  )
		{
			/* 则只要检测到非正常态则要发送告警*/
			if( get_atm_alarm_level(&atm_data) != ALARM_STS_NOMINAL )
			{
				/* 保存新的温度告警状态和值*/
				g_cur_sts.cur_atm_alarm_sts = get_atm_alarm_level(&atm_data);
				g_cur_sts.last_atm_value.itemp = atm_data.itemp;
				g_cur_sts.last_atm_value.ftemp = atm_data.ftemp;
				g_cur_sts.last_atm_value.sign = atm_data.sign;
				g_cur_sts.last_atm_value.raw_value = atm_data.raw_value;
				/* 发送告警*/
				sysMonitor2alarm_send_temperature_notification(pAtm2AlarmSk, &atm_data);
			}
		}
		/* 如果之前处于告警状态*/
		else
		{
			/* 如果告警等级发生变化，不论是等级升高或者降低，则发送告警*/
			if( get_atm_alarm_level(&atm_data) != g_cur_sts.cur_atm_alarm_sts )
			{
				/* 保存新的温度告警状态和值*/
				g_cur_sts.cur_atm_alarm_sts = get_atm_alarm_level(&atm_data);
				g_cur_sts.last_atm_value.itemp = atm_data.itemp;
				g_cur_sts.last_atm_value.ftemp = atm_data.ftemp;
				g_cur_sts.last_atm_value.sign = atm_data.sign;
				g_cur_sts.last_atm_value.raw_value = atm_data.raw_value;
				/* 发送告警*/
				sysMonitor2alarm_send_temperature_notification(pAtm2AlarmSk, &atm_data);
			}
			/* 否则，只有当温差大于1度时才发送告警*/
			else
			{
				if( atm_data.itemp > g_cur_sts.last_atm_value.itemp )
				{
					if( (atm_data.itemp - g_cur_sts.last_atm_value.itemp) > 1 )
					{
						/* 保存新的温度告警状态和值*/
						g_cur_sts.last_atm_value.itemp = atm_data.itemp;
						g_cur_sts.last_atm_value.ftemp = atm_data.ftemp;
						g_cur_sts.last_atm_value.sign = atm_data.sign;
						g_cur_sts.last_atm_value.raw_value = atm_data.raw_value;
						/* 发送告警*/
						sysMonitor2alarm_send_temperature_notification(pAtm2AlarmSk, &atm_data);
					}
				}
			}
		}
	}
	else
	{
		/* 读取环境温度失败的告警只发送一次*/
		if(0 == g_cur_sts.atm_read_error)
		{			
			/* 清理温度告警值变量*/
			g_cur_sts.cur_atm_alarm_sts = ALARM_STS_NOMINAL;
			g_cur_sts.last_atm_value.ftemp = 0;
			g_cur_sts.last_atm_value.itemp = 0;
			g_cur_sts.last_atm_value.raw_value = 0;
			g_cur_sts.last_atm_value.sign = 0;
			
			/* 发送读取环境温度失败的告警*/
			sysMonitor2alarm_get_temperature_error_notification(pAtm2AlarmSk);			
		}

		/* 将错误标志位置1，防止重复发送告警*/
		g_cur_sts.atm_read_error++;

		/* 读取错误后休眠几秒再读*/
		sleep(3*g_cur_sts.atm_read_error);
	}

	return g_cur_sts.atm_read_error;
}

void *atmProcessor(void)
{
	T_UDP_SK_INFO *pAtm2CmmSk = &atmCmmSk;
	T_UDP_SK_INFO *pAtm2AlarmSk = &atmAlarmSk;

	/* DBS设计变更，支持多线程操作，初始化时在每个线程中独享消息接口*/
	/*创建与数据库模块互斥通讯的外部SOCKET接口*/
	dbsdev = dbsNoWaitOpen(MID_ATM);
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: atmProcessor->dbsOpen error, exited !\n");
		return (void *)0;
	}

	if( CMM_SUCCESS != sysMonitor2cmm_init(pAtm2CmmSk) )
	{
		fprintf(stderr, "atmProcessor->sysMonitor2cmm_init() failed\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "atmProcessor->sysMonitor2cmm_init() failed");
		dbsClose(dbsdev);
		return (void *)0;
	}

	if( CMM_SUCCESS != sysMonitor2alarm_init(pAtm2AlarmSk) )
	{
		fprintf(stderr, "atmProcessor->sysMonitor2alarm_init() failed\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "atmProcessor->sysMonitor2alarm_init() failed");
		dbsClose(dbsdev);
		return (void *)0;
	}

	/* 初始化当前温度告警状态变量*/
	g_cur_sts.atm_read_error = 0;
	g_cur_sts.cur_atm_alarm_sts = ALARM_STS_NOMINAL;
	g_cur_sts.last_atm_value.ftemp = 0;
	g_cur_sts.last_atm_value.itemp = 0;
	g_cur_sts.last_atm_value.raw_value = 0;
	g_cur_sts.last_atm_value.sign = 0;

	fprintf(stderr, "Starting thread atmProcessor 	......		[OK]\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting thread atmProcessor success");

	for (;;)
	{
		sleep(3);
		if( atmHandler() >= 3 )
		{
			/* 如果连续3次读取温度失败，则退出子线程*/
			fprintf(stderr, "\natmProcessor can not get cbat ambient temperature\n");
			dbs_sys_log(dbsdev, DBS_LOG_ERR, "atmProcessor can not get cbat ambient temperature");
			break;
		}
	}

	/* 不要在这个后面添加代码，执行不到滴*/
	printf("thread atmProcessor exit !\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "INFO: thread atmProcessor exit");
	sysMonitor2cmm_destroy(pAtm2CmmSk);
	sysMonitor2alarm_destroy(pAtm2AlarmSk);
	dbsClose(dbsdev);
	return (void *)0;
} 

