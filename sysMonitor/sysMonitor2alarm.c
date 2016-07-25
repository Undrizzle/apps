/*****************************************************************************************
  文件名称 : reg_alarm.c
  文件描述 : 注册模块与告警管理模块连接的处理函数
  修订记录 :
           1 创建 : frank
             日期 : 2010-12-07
             描述 : 创建文件

 *****************************************************************************************/
#include <assert.h>
#include "sysMonitor2alarm.h"

void __sysMonitor2alarm_comm(T_UDP_SK_INFO *sk, T_ALARM_DESC *alarm)
{
	assert( NULL != alarm );	

	int len = 0;
	uint8_t buffer[MAX_UDP_SIZE];
	T_Msg_Header_CMM h;	
	
	h.usSrcMID=MID_SYSMONITOR;
	h.usDstMID=MID_ALARM;
	h.fragment=0;
	h.usMsgType=CMM_SEND_ALARM_NOTIFICATION;
	h.ulBodyLength=sizeof(T_ALARM_DESC);
	
	memcpy(buffer, &h, sizeof(T_Msg_Header_CMM));
	memcpy(buffer+sizeof(T_Msg_Header_CMM), alarm, sizeof(T_ALARM_DESC));
	len = sizeof(T_Msg_Header_CMM)+sizeof(T_ALARM_DESC);
	
	if(sendto(sk->sk, buffer, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(sk->skaddr)) == -1)
	{
		perror("__sysMonitor2alarm_comm sendto error!");
	}
}

void sysMonitor2alarm_get_temperature_error_notification(T_UDP_SK_INFO *sk)
{
	T_ALARM_DESC alarm_desc;

	alarm_desc.AlarmCode = 200924;
	alarm_desc.CltIndex = 0;
	alarm_desc.CnuIndex = 0;
	alarm_desc.PortIndex = 0;
	alarm_desc.AlarmValue = 0;
	alarm_desc.AlarmStatus = ALARM_STS_MAJOR;
	sprintf(alarm_desc.AlarmInfo, "can not get cbat ambient temperature");

	
	__sysMonitor2alarm_comm(sk, &alarm_desc);
}

void sysMonitor2alarm_send_temperature_notification(T_UDP_SK_INFO *sk, st_temperature *curTemp)
{
	T_ALARM_DESC alarm_desc;
	uint32_t tmp_sign;
	uint32_t tmp_itemp;
	uint32_t tmp_ftemp;
	
	tmp_sign = (curTemp->sign)&(0x000000ff);
	tmp_itemp = (curTemp->itemp)&(0x000000ff);
	tmp_ftemp = (curTemp->ftemp)&(0x0000ffff);
	
	alarm_desc.AlarmCode = 200903;
	alarm_desc.CltIndex = 0;
	alarm_desc.CnuIndex = 0;
	alarm_desc.PortIndex = 0;
	alarm_desc.AlarmValue = ((tmp_sign<<24)|(tmp_itemp<<16)|tmp_ftemp);

	if( 0 == curTemp->sign )
	{
		if( curTemp->itemp >= SYS_TEMPERATURE_ALARM_HIHI )
		{
			/* HIHI */
			alarm_desc.AlarmStatus = ALARM_STS_HIHI;
			sprintf(alarm_desc.AlarmInfo, "Ambient temperature HiHi notification");
		}
		else if( curTemp->itemp >= SYS_TEMPERATURE_ALARM_HI )
		{
			/* HI */
			alarm_desc.AlarmStatus = ALARM_STS_HI;
			sprintf(alarm_desc.AlarmInfo, "Ambient temperature Hi notification");
		}
		else
		{
			/* NORMAL */
			alarm_desc.AlarmStatus = ALARM_STS_NOMINAL;
			sprintf(alarm_desc.AlarmInfo, "Ambient temperature normal notification");
		}
	}
	else
	{
		if( curTemp->itemp >= SYS_TEMPERATURE_ALARM_LOLO )
		{
			/* LOLO */
			alarm_desc.AlarmStatus = ALARM_STS_LOLO;
			sprintf(alarm_desc.AlarmInfo, "Ambient temperature LoLo notification");
		}
		else if( curTemp->itemp >= SYS_TEMPERATURE_ALARM_LO )
		{
			/* LO */
			alarm_desc.AlarmStatus = ALARM_STS_LO;
			sprintf(alarm_desc.AlarmInfo, "Ambient temperature Lo notification");
		}
		else
		{
			/* NORMAL */
			alarm_desc.AlarmStatus = ALARM_STS_NOMINAL;
			sprintf(alarm_desc.AlarmInfo, "Ambient temperature normal notification");
		}
	}
	
	__sysMonitor2alarm_comm(sk, &alarm_desc);
}

int sysMonitor2alarm_destroy(T_UDP_SK_INFO *sk)
{
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int sysMonitor2alarm_init(T_UDP_SK_INFO *sk)
{
	if( ( sk->sk = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}
	sk->skaddr.sin_family = AF_INET;
	sk->skaddr.sin_port = htons(ALARM_LISTEN_PORT);		/* 目的端口号*/
	sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");		/* 目的地址*/	
	return CMM_SUCCESS;
}

