/*****************************************************************************************
  文件名称 : reg_alarm.c
  文件描述 : 注册模块与告警管理模块连接的处理函数
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
#include "reg_alarm.h"
#include "reg_dbs.h"

T_UDP_SK_INFO SK_ALARM;

void __alarm_notification(T_ALARM_DESC *alarm)
{
	assert( NULL != alarm );	

	T_UDP_SK_INFO *sk = &SK_ALARM;

	int len = 0;
	uint8_t buffer[MAX_UDP_SIZE];
	T_Msg_Header_CMM h;	
	
	h.usSrcMID=MID_REGISTER;
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

void clt_sts_transition_notification(uint32_t id, BOOLEAN sts)
{
	T_ALARM_DESC alarm_desc;
	
	switch(sts)
	{
		case BOOL_FALSE:
			alarm_desc.AlarmCode = 200901;
			alarm_desc.CltIndex = id;
			alarm_desc.CnuIndex = 0;
			alarm_desc.PortIndex = 0;
			alarm_desc.AlarmStatus = ALARM_STS_MINOR;
			alarm_desc.AlarmValue = DEV_STS_OFFLINE;
			__alarm_notification(&alarm_desc);
			break;
		case BOOL_TRUE:
			alarm_desc.AlarmCode = 200901;
			alarm_desc.CltIndex = id;
			alarm_desc.CnuIndex = 0;
			alarm_desc.PortIndex = 0;
			alarm_desc.AlarmStatus = ALARM_STS_MAJOR;
			alarm_desc.AlarmValue = DEV_STS_ONLINE;
			__alarm_notification(&alarm_desc);
			break;
		default:
			break;
	}
}

void cnu_sts_transition_notification(uint32_t clt, uint32_t cnu, BOOLEAN sts)
{
	T_ALARM_DESC alarm_desc;

#if 0
	switch(sts)
	{
		case BOOL_FALSE:
			alarm_desc.AlarmCode = 200902;
			alarm_desc.CltIndex = clt;
			alarm_desc.CnuIndex = cnu;
			alarm_desc.PortIndex = 0;
			alarm_desc.AlarmStatus = ALARM_STS_MINOR;
			alarm_desc.AlarmValue = DEV_STS_OFFLINE;
			__alarm_notification(&alarm_desc);
			break;
		case BOOL_TRUE:
			alarm_desc.AlarmCode = 200902;
			alarm_desc.CltIndex = clt;
			alarm_desc.CnuIndex = cnu;
			alarm_desc.PortIndex = 0;
			alarm_desc.AlarmStatus = ALARM_STS_MAJOR;
			alarm_desc.AlarmValue = DEV_STS_ONLINE;
			__alarm_notification(&alarm_desc);
			break;
		default:
			break;
	}
#endif
}

void lllegal_cnu_register_notification(uint32_t clt, uint8_t mac[])
{
	T_ALARM_DESC alarm_desc;
	alarm_desc.AlarmCode = 200912;
	alarm_desc.CltIndex = clt;
	alarm_desc.CnuIndex = 0;
	alarm_desc.PortIndex = 0;
	alarm_desc.AlarmStatus = ALARM_STS_MAJOR;
	alarm_desc.AlarmValue = DEV_STS_ONLINE;
	sprintf(alarm_desc.AlarmInfo, "%02X:%02X:%02X:%02X:%02X:%02X", 
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	__alarm_notification(&alarm_desc);
}

void cnu_exceed_notification(uint32_t clt)
{
	T_ALARM_DESC alarm_desc;

	memset(&alarm_desc, 0, sizeof(T_ALARM_DESC));
	
	alarm_desc.AlarmCode = 200913;
	alarm_desc.CltIndex = clt;
	__alarm_notification(&alarm_desc);
}

void cnu_block_notification(uint32_t clt, uint32_t cnu, uint8_t mac[], BOOLEAN sts)
{
	T_ALARM_DESC alarm_desc;
	
	switch(sts)
	{
		case BOOL_FALSE:
			alarm_desc.AlarmCode = 200914;
			alarm_desc.CltIndex = clt;
			alarm_desc.CnuIndex = cnu;
			alarm_desc.PortIndex = 0;
			alarm_desc.AlarmStatus = ALARM_STS_MINOR;
			alarm_desc.AlarmValue = sts;
			sprintf(alarm_desc.AlarmInfo, "%02X:%02X:%02X:%02X:%02X:%02X", 
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
			__alarm_notification(&alarm_desc);
			break;
		case BOOL_TRUE:
			alarm_desc.AlarmCode = 200914;
			alarm_desc.CltIndex = clt;
			alarm_desc.CnuIndex = cnu;
			alarm_desc.PortIndex = 0;
			alarm_desc.AlarmStatus = ALARM_STS_MAJOR;
			alarm_desc.AlarmValue = sts;
			sprintf(alarm_desc.AlarmInfo, "%02X:%02X:%02X:%02X:%02X:%02X", 
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
			__alarm_notification(&alarm_desc);
			break;
		default:
			break;
	}
}

void cnu_auto_config_notification(uint32_t clt, uint32_t cnu, uint32_t tid, uint32_t cKey, BOOLEAN sts)
{
	T_ALARM_DESC alarm_desc;

	if( cKey == 0 )
	{
		alarm_desc.AlarmCode = 200915;
	}
	else
	{
		alarm_desc.AlarmCode = 200916;
	}

	switch(sts)
	{
		case BOOL_FALSE:
			alarm_desc.CltIndex = clt;
			alarm_desc.CnuIndex = cnu;
			alarm_desc.PortIndex = 0;
			alarm_desc.AlarmStatus = ALARM_STS_MINOR;
			alarm_desc.AlarmValue = sts;
			sprintf(alarm_desc.AlarmInfo, "(%d)", tid);
			__alarm_notification(&alarm_desc);
			break;
		case BOOL_TRUE:
			alarm_desc.CltIndex = clt;
			alarm_desc.CnuIndex = cnu;
			alarm_desc.PortIndex = 0;
			alarm_desc.AlarmStatus = ALARM_STS_MAJOR;
			alarm_desc.AlarmValue = sts;
			sprintf(alarm_desc.AlarmInfo, "(%d)", tid);
			__alarm_notification(&alarm_desc);
			break;
		default:
			break;
	}
}

void cnu_abort_auto_config_notification(uint32_t clt, uint32_t cnu)
{
	T_ALARM_DESC alarm_desc;

	memset(&alarm_desc, 0, sizeof(T_ALARM_DESC));
	
	alarm_desc.AlarmCode = 200917;
	alarm_desc.CltIndex = clt;
	alarm_desc.CnuIndex = cnu;
	alarm_desc.AlarmStatus = ALARM_STS_MAJOR;
	__alarm_notification(&alarm_desc);
}

void lllegal_cnu_kick_off_notification(uint32_t clt, uint8_t mac[], BOOLEAN sts)
{
	T_ALARM_DESC alarm_desc;

	memset(&alarm_desc, 0, sizeof(T_ALARM_DESC));
	
	alarm_desc.AlarmCode = 200918;
	alarm_desc.CltIndex = clt;
	alarm_desc.CnuIndex = 0;
	alarm_desc.AlarmStatus = ALARM_STS_MAJOR;
	alarm_desc.AlarmValue = sts;
	sprintf(alarm_desc.AlarmInfo, "%02X:%02X:%02X:%02X:%02X:%02X", 
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	__alarm_notification(&alarm_desc);
}

void cnu_re_register_notification(uint32_t clt, uint32_t cnu)
{
	T_ALARM_DESC alarm_desc;

	memset(&alarm_desc, 0, sizeof(T_ALARM_DESC));
	
	alarm_desc.AlarmCode = 200919;
	alarm_desc.CltIndex = clt;
	alarm_desc.CnuIndex = cnu;
	alarm_desc.AlarmStatus = ALARM_STS_MAJOR;
	__alarm_notification(&alarm_desc);
}

void clt_heartbeat_loss_notification(uint32_t clt, uint32_t count)
{
	T_ALARM_DESC alarm_desc;

	memset(&alarm_desc, 0, sizeof(T_ALARM_DESC));
	
	alarm_desc.AlarmCode = 200922;
	alarm_desc.CltIndex = clt;
	alarm_desc.CnuIndex = 0;
	alarm_desc.AlarmStatus = ALARM_STS_MAJOR;
	alarm_desc.AlarmValue = count;
	sprintf(alarm_desc.AlarmInfo, "loss heartbeat with clt/%d for %d times", clt, count);
	__alarm_notification(&alarm_desc);
}

void clt_cannot_finded_notification(uint32_t clt)
{
	T_ALARM_DESC alarm_desc;

	memset(&alarm_desc, 0, sizeof(T_ALARM_DESC));
	
	alarm_desc.AlarmCode = 200923;
	alarm_desc.CltIndex = clt;
	alarm_desc.CnuIndex = 0;
	alarm_desc.AlarmStatus = ALARM_STS_MAJOR;
	alarm_desc.AlarmValue = 1;
	sprintf(alarm_desc.AlarmInfo, "can not find clt/%d", clt);
	__alarm_notification(&alarm_desc);
}

void cbat_system_sts_notification(uint8_t status)
{
	T_ALARM_DESC alarm_desc;
	DB_INTEGER_V iDevType;
	DB_TEXT_V sCbatIpaddr;

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

	memset(&alarm_desc, 0, sizeof(T_ALARM_DESC));
	
	alarm_desc.AlarmCode = (status?200920:200921);
	alarm_desc.CltIndex = 0;
	alarm_desc.CnuIndex = 0;
	alarm_desc.AlarmStatus = (status?ALARM_STS_MAJOR:ALARM_STS_MINOR);
	alarm_desc.AlarmValue = (status?1:0);
	if(status)
	{
		sprintf(alarm_desc.AlarmInfo, "[%s][%d] cbat system start", 
				sCbatIpaddr.text, 
				boardapi_mapDevModel(iDevType.integer)
		);
	}
	else
	{
		sprintf(alarm_desc.AlarmInfo, "[%s][%d] cbat system stop", 
				sCbatIpaddr.text, 
				boardapi_mapDevModel(iDevType.integer)
		);
	}
	
	__alarm_notification(&alarm_desc);
}

void reg2alarm_send_heartbeat_notification(void)
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

int msg_alarm_destroy(void)
{
	T_UDP_SK_INFO *sk = &SK_ALARM;
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int msg_alarm_init(void)
{
	T_UDP_SK_INFO *sk = &SK_ALARM;
	
	if( ( sk->sk = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}
	sk->skaddr.sin_family = AF_INET;
	sk->skaddr.sin_port = htons(ALARM_LISTEN_PORT);		/* 目的端口号*/
	sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");		/* 目的地址*/	
	return CMM_SUCCESS;
}



