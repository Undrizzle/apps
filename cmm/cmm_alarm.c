/*****************************************************************************************
  文件名称 : cmm_alarm.c
  文件描述 : 
  修订记录 :
           1 创建 : frank
             日期 : 2010-12-07
             描述 : 创建文件

 *****************************************************************************************/
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dbsapi.h>
#include <boardapi.h>
#include "cmm_alarm.h"

T_UDP_SK_INFO SK_CMM2ALARM;

extern T_DBS_DEV_INFO *dbsdev;

void __alarm_notification(T_ALARM_DESC *alarm)
{
	assert( NULL != alarm );	

	T_UDP_SK_INFO *sk = &SK_CMM2ALARM;

	int len = 0;
	uint8_t buffer[MAX_UDP_SIZE];
	T_Msg_Header_CMM h;	
	
	h.usSrcMID=MID_CMM;
	h.usDstMID=MID_ALARM;
	h.fragment=0;
	h.usMsgType=CMM_SEND_ALARM_NOTIFICATION;
	h.ulBodyLength=sizeof(T_ALARM_DESC);
	
	memcpy(buffer, &h, sizeof(T_Msg_Header_CMM));
	memcpy(buffer+sizeof(T_Msg_Header_CMM), alarm, sizeof(T_ALARM_DESC));
	len = sizeof(T_Msg_Header_CMM)+sizeof(T_ALARM_DESC);
	
	if(sendto(sk->sk, buffer, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(sk->skaddr)) == -1)
	{
		perror("alarm send error!");
	}
}

void cmm2alarm_sendCbatResetNotification(void)
{
	T_ALARM_DESC alarm_desc;
	DB_INTEGER_V iDevType;
	DB_TEXT_V sCbatIpaddr;

	memset(&alarm_desc, 0, sizeof(T_ALARM_DESC));

	/* 获取CBAT IP 地址*/
	sCbatIpaddr.ci.tbl = DBS_SYS_TBL_ID_NETWORK;
	sCbatIpaddr.ci.row = 1;
	sCbatIpaddr.ci.col = DBS_SYS_TBL_NETWORK_COL_ID_IP;
	sCbatIpaddr.ci.colType = DBS_TEXT;
	if( CMM_SUCCESS != dbsGetText(dbsdev, &sCbatIpaddr) ) return;

	/* 获取CBAT 的设备类型*/
	iDevType.ci.tbl = DBS_SYS_TBL_ID_SYSINFO;
	iDevType.ci.row = 1;
	iDevType.ci.col = DBS_SYS_TBL_SYSINFO_COL_ID_MODEL;
	iDevType.ci.colType = DBS_INTEGER;
	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iDevType) ) return;	
	
	alarm_desc.AlarmCode = 200921;
	alarm_desc.CltIndex = 0;
	alarm_desc.CnuIndex = 0;
	alarm_desc.AlarmStatus = ALARM_STS_MINOR;
	alarm_desc.AlarmValue = 0;
	
	sprintf(alarm_desc.AlarmInfo, "[%s][%d] cbat system stop", 
		sCbatIpaddr.text, 
		boardapi_mapDevModel(iDevType.integer)
	);
	__alarm_notification(&alarm_desc);
}

void cmm2alarm_sendCbatTopHbTrap(void)
{
	T_ALARM_DESC alarm_desc;

	memset(&alarm_desc, 0, sizeof(T_ALARM_DESC));
	
	alarm_desc.AlarmCode = 200001;
	alarm_desc.CltIndex = 0;
	alarm_desc.CnuIndex = 0;
	alarm_desc.AlarmStatus = ALARM_STS_MAJOR;
	alarm_desc.AlarmValue = 1;
	sprintf(alarm_desc.AlarmInfo, "send cbat topology heartbeat");
	__alarm_notification(&alarm_desc);
}

void cmm2alarm_heartbeatEnable(uint32_t status)
{
	T_ALARM_DESC alarm_desc;

	memset(&alarm_desc, 0, sizeof(T_ALARM_DESC));
	
	alarm_desc.AlarmCode = 300000;
	alarm_desc.CltIndex = 0;
	alarm_desc.CnuIndex = 0;
	alarm_desc.AlarmStatus = ALARM_STS_OTHER;
	alarm_desc.AlarmValue = (status?1:0);	
	__alarm_notification(&alarm_desc);
}

void cmm2alarm_upgradeNotification(uint32_t status)
{
	T_ALARM_DESC alarm_desc;

	memset(&alarm_desc, 0, sizeof(T_ALARM_DESC));
	
	alarm_desc.AlarmCode = 200909;
	alarm_desc.CltIndex = 0;
	alarm_desc.CnuIndex = 0;
	alarm_desc.AlarmStatus = ALARM_STS_MAJOR;
	alarm_desc.AlarmValue = status;
	__alarm_notification(&alarm_desc);
}

int cmm2alarm_destroy(void)
{
	T_UDP_SK_INFO *sk = &SK_CMM2ALARM;
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int cmm2alarm_init(void)
{
	T_UDP_SK_INFO *sk = &SK_CMM2ALARM;
	
	if( ( sk->sk = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}
	sk->skaddr.sin_family = AF_INET;
	sk->skaddr.sin_port = htons(ALARM_LISTEN_PORT);		/* 目的端口号*/
	sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");		/* 目的地址*/
	return CMM_SUCCESS;
}

