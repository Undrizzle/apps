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

/* ������ǰ�����¶ȸ澯״̬�ı���*/
st_atm_status g_cur_sts;

/* ���߳���CMMģ��ͨѶ����Ϣ�ӿ�*/
static T_UDP_SK_INFO atmCmmSk;

/* ���߳���Alarmģ��ͨѶ����Ϣ�ӿ�*/
static T_UDP_SK_INFO atmAlarmSk;

/* ��DBS  ͨѶ���豸�ļ�*/
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

/* ��ȡϵͳ�����¶ȣ������澯����*/
int atmHandler(void)
{
	st_temperature atm_data;
	T_UDP_SK_INFO *pAtm2CmmSk = &atmCmmSk;
	T_UDP_SK_INFO *pAtm2AlarmSk = &atmAlarmSk;
	
	if( CMM_SUCCESS == sysMonitor2cmm_getCbatTemperature(pAtm2CmmSk, &atm_data) )
	{
		/* ��λ�����¶ȶ�ȡ�����־λ*/
		g_cur_sts.atm_read_error = 0;

		/* ���֮ǰ�ĸ澯״̬Ϊ����̬*/
		if( ALARM_STS_NOMINAL == g_cur_sts.cur_atm_alarm_sts  )
		{
			/* ��ֻҪ��⵽������̬��Ҫ���͸澯*/
			if( get_atm_alarm_level(&atm_data) != ALARM_STS_NOMINAL )
			{
				/* �����µ��¶ȸ澯״̬��ֵ*/
				g_cur_sts.cur_atm_alarm_sts = get_atm_alarm_level(&atm_data);
				g_cur_sts.last_atm_value.itemp = atm_data.itemp;
				g_cur_sts.last_atm_value.ftemp = atm_data.ftemp;
				g_cur_sts.last_atm_value.sign = atm_data.sign;
				g_cur_sts.last_atm_value.raw_value = atm_data.raw_value;
				/* ���͸澯*/
				sysMonitor2alarm_send_temperature_notification(pAtm2AlarmSk, &atm_data);
			}
		}
		/* ���֮ǰ���ڸ澯״̬*/
		else
		{
			/* ����澯�ȼ������仯�������ǵȼ����߻��߽��ͣ����͸澯*/
			if( get_atm_alarm_level(&atm_data) != g_cur_sts.cur_atm_alarm_sts )
			{
				/* �����µ��¶ȸ澯״̬��ֵ*/
				g_cur_sts.cur_atm_alarm_sts = get_atm_alarm_level(&atm_data);
				g_cur_sts.last_atm_value.itemp = atm_data.itemp;
				g_cur_sts.last_atm_value.ftemp = atm_data.ftemp;
				g_cur_sts.last_atm_value.sign = atm_data.sign;
				g_cur_sts.last_atm_value.raw_value = atm_data.raw_value;
				/* ���͸澯*/
				sysMonitor2alarm_send_temperature_notification(pAtm2AlarmSk, &atm_data);
			}
			/* ����ֻ�е��²����1��ʱ�ŷ��͸澯*/
			else
			{
				if( atm_data.itemp > g_cur_sts.last_atm_value.itemp )
				{
					if( (atm_data.itemp - g_cur_sts.last_atm_value.itemp) > 1 )
					{
						/* �����µ��¶ȸ澯״̬��ֵ*/
						g_cur_sts.last_atm_value.itemp = atm_data.itemp;
						g_cur_sts.last_atm_value.ftemp = atm_data.ftemp;
						g_cur_sts.last_atm_value.sign = atm_data.sign;
						g_cur_sts.last_atm_value.raw_value = atm_data.raw_value;
						/* ���͸澯*/
						sysMonitor2alarm_send_temperature_notification(pAtm2AlarmSk, &atm_data);
					}
				}
			}
		}
	}
	else
	{
		/* ��ȡ�����¶�ʧ�ܵĸ澯ֻ����һ��*/
		if(0 == g_cur_sts.atm_read_error)
		{			
			/* �����¶ȸ澯ֵ����*/
			g_cur_sts.cur_atm_alarm_sts = ALARM_STS_NOMINAL;
			g_cur_sts.last_atm_value.ftemp = 0;
			g_cur_sts.last_atm_value.itemp = 0;
			g_cur_sts.last_atm_value.raw_value = 0;
			g_cur_sts.last_atm_value.sign = 0;
			
			/* ���Ͷ�ȡ�����¶�ʧ�ܵĸ澯*/
			sysMonitor2alarm_get_temperature_error_notification(pAtm2AlarmSk);			
		}

		/* �������־λ��1����ֹ�ظ����͸澯*/
		g_cur_sts.atm_read_error++;

		/* ��ȡ��������߼����ٶ�*/
		sleep(3*g_cur_sts.atm_read_error);
	}

	return g_cur_sts.atm_read_error;
}

void *atmProcessor(void)
{
	T_UDP_SK_INFO *pAtm2CmmSk = &atmCmmSk;
	T_UDP_SK_INFO *pAtm2AlarmSk = &atmAlarmSk;

	/* DBS��Ʊ����֧�ֶ��̲߳�������ʼ��ʱ��ÿ���߳��ж�����Ϣ�ӿ�*/
	/*���������ݿ�ģ�黥��ͨѶ���ⲿSOCKET�ӿ�*/
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

	/* ��ʼ����ǰ�¶ȸ澯״̬����*/
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
			/* �������3�ζ�ȡ�¶�ʧ�ܣ����˳����߳�*/
			fprintf(stderr, "\natmProcessor can not get cbat ambient temperature\n");
			dbs_sys_log(dbsdev, DBS_LOG_ERR, "atmProcessor can not get cbat ambient temperature");
			break;
		}
	}

	/* ��Ҫ�����������Ӵ��룬ִ�в�����*/
	printf("thread atmProcessor exit !\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "INFO: thread atmProcessor exit");
	sysMonitor2cmm_destroy(pAtm2CmmSk);
	sysMonitor2alarm_destroy(pAtm2AlarmSk);
	dbsClose(dbsdev);
	return (void *)0;
} 

