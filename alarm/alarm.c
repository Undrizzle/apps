/*****************************************************************************************
  文件名称 : alarm.c
  文件描述 : alarm模块主入口函数
				处理来自其它模块的告警信息,并转发给SNMP 模块
  修订记录 :
           1 创建 : may2250
             日期 : 2010-10-14
             描述 : 创建文件

 *****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>

#include "alarm.h"

#include <dbsapi.h>
#include <boardapi.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#ifdef SEND_TRAP_BY_AGENT_TRAP
#undef SEND_TRAP_BY_AGENT_TRAP
#endif

T_UDP_SK_INFO SK_ALARM;
uint8_t g_hb_enabled = 1;

/* 与DBS  通讯的设备文件*/
static T_DBS_DEV_INFO *dbsdev = NULL;

#ifdef SEND_TRAP_BY_AGENT_TRAP
T_UDP_SK_INFO SK_TOSNMP;
#else
st_dbsNetwork g_networkInfo;
st_dbsSysinfo g_szSysinfo;

oid objid_sysuptime[] =	{ 1, 3, 6, 1, 2, 1, 1, 3, 0 };
oid objid_snmptrap[] =	{ 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };
oid objid_alarmcode[] =	{ 1, 3, 6, 1, 4, 1, 17409, 8080, 5, 1 };
oid objid_serialflow[] =	{ 1, 3, 6, 1, 4, 1, 17409, 8080, 5, 2 };
oid objid_hostmac[] =	{ 1, 3, 6, 1, 4, 1, 17409, 8080, 5, 3 };
oid objid_cltid[] =		{ 1, 3, 6, 1, 4, 1, 17409, 8080, 5, 4 };
oid objid_cnuid[] =		{ 1, 3, 6, 1, 4, 1, 17409, 8080, 5, 5 };
oid objid_alarmtype[] =	{ 1, 3, 6, 1, 4, 1, 17409, 8080, 5, 6 };
oid objid_alarmvalue[] =	{ 1, 3, 6, 1, 4, 1, 17409, 8080, 5, 7 };
oid objid_trapinfo[] =		{ 1, 3, 6, 1, 4, 1, 17409, 8080, 5, 8 };
oid objid_cbathb[] =		{ 1, 3, 6, 1, 4, 1, 17409, 8080, 6, 1 };
oid objid_cnuhb[] =		{ 1, 3, 6, 1, 4, 1, 17409, 8080, 6, 2 };

#endif

//T_UDP_SK_INFO SK_TODB;

//uint8_t CBAT_MAC[6];
	
uint8_t OSA [6] = 
{
	0x00,
	0xb0,
	0x52,
	0x00,
	0x00,
	0x01 
};

int ALARM_MSG_DEBUG_ENABLE = 0;
#ifdef SEND_TRAP_BY_AGENT_TRAP
int hb_flag = 0;
#endif
int serialFlowNumber = 0;

#define __alarm_debug(...);	\
{	\
	if( ALARM_MSG_DEBUG_ENABLE )	\
		fprintf(stderr, __VA_ARGS__);	\
}

//extern void hexdump (const unsigned char memory [], size_t length, FILE *fp) ;

/********************************************************************************************
*	函数名称:debug_dump_msg
*	函数功能:调试用API，以十六进制的方式将缓冲区的内容输出到
*				   文件指针fp指向的设备文件
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void debug_dump_msg(const unsigned char memory [], size_t length, FILE *fp)
{
	if(ALARM_MSG_DEBUG_ENABLE)
	{
		__alarm_debug("----------------------------------------------------------------------\n");
		hexdump(memory, length, fp);
		__alarm_debug("\n----------------------------------------------------------------------\n");
	}
}

int init_socket(T_UDP_SK_INFO *sk)
{
	int len;
	len=sizeof(sk->skaddr);
	bzero(&sk->skaddr, sizeof(sk->skaddr));
	switch(sk->desc)
	{
		case DESC_SOCKET_ALARM:			
			sk->sk= socket(AF_INET, SOCK_DGRAM, 0);
			sk->skaddr.sin_family = AF_INET;
			sk->skaddr.sin_port = htons(ALARM_LISTEN_PORT);
			sk->skaddr.sin_addr.s_addr= htonl(INADDR_ANY);
			
			if(bind(sk->sk ,(struct sockaddr *)&sk->skaddr,sizeof(sk->skaddr))<0)
			{
				perror("bind error");
				exit(0);
  			}
			break;
		case DESC_SOCKET_OTHER:		
			sk->sk= socket(AF_INET, SOCK_DGRAM, 0);
			sk->skaddr.sin_family = AF_INET;
			sk->skaddr.sin_port = htons(SNMP_ALARM_PORT);		
			sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
			break;
		case DESC_SOCKET_DBS:		
			sk->sk= socket(AF_INET, SOCK_DGRAM, 0);
			sk->skaddr.sin_family = AF_INET;
			sk->skaddr.sin_port = htons(DB_ACCESS_LISTEN_PORT);		
			sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
			break;
	}
	return CMM_SUCCESS;
}


void SignalProcessHandle(int n)
{
#ifdef SEND_TRAP_BY_AGENT_TRAP
	hb_flag = 0;
#endif
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "SignalProcessHandle : module alarm exit");
	/* 关闭socket接口 */
	close(SK_ALARM.sk);
#ifdef SEND_TRAP_BY_AGENT_TRAP
	close(SK_TOSNMP.sk);
#endif
	//close(SK_TODB.sk);	
	snmp_shutdown("snmpapp");
	dbsClose(dbsdev);
	exit(0);
}

int recieve_alarm_notification(stAlarmInfo *alarminfo)
{
	assert( NULL != alarminfo );
	
	uint32_t len = 0;
	int FromAddrSize = 0;	
	uint8_t buffer[MAX_UDP_SIZE];
	time_t b_time;
	T_Msg_CMM *msg = (T_Msg_CMM *)buffer;
	T_ALARM_DESC *alarm_desc = NULL;
	st_dbsCnu row_cnu;
	
	len = recvfrom(SK_ALARM.sk, buffer, MAX_UDP_SIZE, 0, (struct sockaddr *)&(SK_ALARM.skaddr), &FromAddrSize);

	if ( -1 == len )
	{
		return CMM_FAILED;
	}
	else
	{
		alarm_desc = (T_ALARM_DESC *)(msg->BUF);
		time(&b_time);
		
		alarminfo->ItemNum = 0;
		//alarminfo->oid 
		alarminfo->AlarmCode = alarm_desc->AlarmCode;
		alarminfo->RealTime = b_time;
		//alarminfo->SerialFlow = ++serialFlowNumber;
		//sprintf(alarminfo->CbatMac, "%02X:%02X:%02X:%02X:%02X:%02X", 
		//	CBAT_MAC[0], CBAT_MAC[1], CBAT_MAC[2], CBAT_MAC[3], CBAT_MAC[4], CBAT_MAC[5]);
		//memcpy(alarminfo->CbatMac, CBAT_MAC, sizeof(alarminfo->CbatMac));
		alarminfo->CltIndex = alarm_desc->CltIndex;
		alarminfo->CnuIndex = alarm_desc->CnuIndex;
		alarminfo->AlarmType = alarm_desc->AlarmStatus;
		alarminfo->AlarmValue = alarm_desc->AlarmValue;
		strcpy(alarminfo->CbatMac, g_networkInfo.col_mac);

		switch(alarminfo->AlarmCode)
		{
			case 200901:
			{
				if( DEV_STS_ONLINE == alarminfo->AlarmValue )
				{
					sprintf(alarminfo->trap_info,"clt/%d registered in", alarm_desc->CltIndex);
				}
				else
				{
					sprintf(alarminfo->trap_info,"clt/%d dropped", alarm_desc->CltIndex);
				}
				break;
			}			
			case 200902:	
			{
				if( CMM_SUCCESS != dbsGetCnu(dbsdev, alarminfo->CnuIndex, &row_cnu) ) return CMM_FAILED;
				if( DEV_STS_ONLINE == alarminfo->AlarmValue )
				{
					/* 禁止修改此告警信息的格式*/
					/* 该告警的格式已经用作和网管通讯的逻辑处理*/
					sprintf(alarminfo->trap_info,"%s[%d] cnu/%d/%d online", 
						row_cnu.col_mac,
						boardapi_mapDevModel(row_cnu.col_model), 
						alarm_desc->CltIndex, alarm_desc->CnuIndex);
				}
				else
				{
					/* 禁止修改此告警信息的格式*/
					/* 该告警的格式已经用作和网管通讯的逻辑处理*/
					sprintf(alarminfo->trap_info,"%s[%d] cnu/%d/%d offline", 
						row_cnu.col_mac,
						boardapi_mapDevModel(row_cnu.col_model), 
						alarm_desc->CltIndex, alarm_desc->CnuIndex);
				}
				break;
			}
			case 200903:
			case 200904:
			case 200905:
			case 200906:
			case 200907:
			case 200908:
			case 200910:
			case 200911:
			{
				strcpy(alarminfo->trap_info, alarm_desc->AlarmInfo);
				break;
			}
			case 200909:
			{
				st_dbsNetwork stNetworkInfo;
				if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &stNetworkInfo) )
				{
					return CMM_FAILED;
				}
				/* 禁止修改此告警信息的格式*/
				/* 该告警的格式已经用作和网管通讯的逻辑处理*/
				sprintf(alarminfo->trap_info, "[%s] cbat upgrade firmware notification", stNetworkInfo.col_ip);
				break;
			}
			case 200912:	
			{
				sprintf(alarminfo->trap_info, "illegal cnu try to register : %s", alarm_desc->AlarmInfo);
				break;
			}
			case 200913:	
			{
				sprintf(alarminfo->trap_info, "Reached the maximum number of cnus");
				break;
			}
			case 200914:	
			{
				if( BOOL_TRUE == alarminfo->AlarmValue )
				{
					sprintf(alarminfo->trap_info, "block cnu/%d/%d success", alarm_desc->CltIndex, alarm_desc->CnuIndex);
				}
				else
				{
					sprintf(alarminfo->trap_info, "block cnu/%d/%d failed", alarm_desc->CltIndex, alarm_desc->CnuIndex);
				}
				break;
			}
			case 200915:	
			{
				if( BOOL_TRUE == alarminfo->AlarmValue )
				{
					sprintf(alarminfo->trap_info, "auto config pib%s for cnu/%d/%d success", 
						alarm_desc->AlarmInfo, alarm_desc->CltIndex, alarm_desc->CnuIndex);
				}
				else
				{
					sprintf(alarminfo->trap_info, "auto config pib%s for cnu/%d/%d failed", 
						alarm_desc->AlarmInfo, alarm_desc->CltIndex, alarm_desc->CnuIndex);
				}
				break;
			}
			case 200916:	
			{
				if( BOOL_TRUE == alarminfo->AlarmValue )
				{
					sprintf(alarminfo->trap_info, "auto config mod%s for cnu/%d/%d success", 
						alarm_desc->AlarmInfo, alarm_desc->CltIndex, alarm_desc->CnuIndex);
				}
				else
				{
					sprintf(alarminfo->trap_info, "auto config mod%s for cnu/%d/%d failed", 
						alarm_desc->AlarmInfo, alarm_desc->CltIndex, alarm_desc->CnuIndex);
				}
				break;
			}
			case 200917:	
			{
				sprintf(alarminfo->trap_info, "cnu/%d/%d abort auto config", 
						alarm_desc->CltIndex, alarm_desc->CnuIndex);
				break;
			}
			case 200918:	
			{
				if( BOOL_TRUE == alarminfo->AlarmValue )
				{
					sprintf(alarminfo->trap_info, "kick off cnu %s success", alarm_desc->AlarmInfo);
				}else{
					sprintf(alarminfo->trap_info, "kick off cnu %s failed", alarm_desc->AlarmInfo);
				}
				break;
			}
			case 200919:
			{
				sprintf(alarminfo->trap_info, "cnu/%d/%d force re-registration", alarm_desc->CltIndex, alarm_desc->CnuIndex);
				break;
			}
			case 200920:
			{
			#ifdef SEND_TRAP_BY_AGENT_TRAP
				/* 禁止修改此告警信息的格式*/
				/* 该告警的格式已经用作和网管通讯的逻辑处理*/
				sprintf(alarminfo->trap_info, "[%s][%d] cbat system cold-start", g_networkInfo.col_ip, boardapi_mapDevModel(g_szSysinfo.col_model);			
				break;
			#endif
			}
			case 200921:
			{
			#ifdef SEND_TRAP_BY_AGENT_TRAP
				/* 禁止修改此告警信息的格式*/
				/* 该告警的格式已经用作和网管通讯的逻辑处理*/
				sprintf(alarminfo->trap_info, "[%s][%d] cbat system going down", g_networkInfo.col_ip, boardapi_mapDevModel(g_szSysinfo.col_model);			
				break;
			#endif
			}
			case 200922:
			case 200923:
			case 200924:		//can not get cbat temperature
			{
				strcpy(alarminfo->trap_info, alarm_desc->AlarmInfo);
				break;
			}
			case 200001:
			{
				break;
			}
			default:
			{
				strcpy(alarminfo->trap_info,"Unkonwn alarm type");
				break;
			}
		}
		/*printf("====> recieve_alarm_notification: AlarmCode = %d, AlarmValue = %d\n", 
			alarminfo->AlarmCode, alarminfo->AlarmValue);*/
		return CMM_SUCCESS;
	}
}

#ifdef SEND_TRAP_BY_AGENT_TRAP
void send_alarm_notification_to_snmp(stAlarmInfo *alarminfo)
{
	assert( NULL != alarminfo );
	
	uint32_t len = 0;
	uint32_t sendn = 0;
	uint8_t buffer[MAX_UDP_SIZE];
	T_Msg_Header_CMM rh;
	
	bzero(buffer, MAX_UDP_SIZE);
	rh.usSrcMID = MID_ALARM;
	rh.usDstMID = MID_SNMP;
	rh.fragment = 0;
	rh.ulBodyLength=sizeof(stAlarmInfo);
	
	memcpy(buffer, &rh, sizeof(T_Msg_Header_CMM));
	memcpy(buffer+sizeof(T_Msg_Header_CMM), alarminfo, sizeof(stAlarmInfo));
	len = sizeof(T_Msg_Header_CMM)+sizeof(stAlarmInfo);
	
	sendn = sendto(SK_TOSNMP.sk, buffer, len, 0, (struct sockaddr *)&(SK_TOSNMP.skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "send_alarm_notification_to_snmp call sendto error");
	}
}
#else

int snmp_input(int operation, netsnmp_session * session, int reqid, netsnmp_pdu *pdu, void *magic)
{
	return 1;
}

uint32_t init_snmptrap(void)
{
	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &g_networkInfo))
	{
		return CMM_FAILED;
	}
	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &g_szSysinfo))
	{
		return CMM_FAILED;
	}
	return CMM_SUCCESS;
}

void send_alarm_notification_to_snmp(stAlarmInfo *alarminfo)
{
	netsnmp_session session, *ss;	
	netsnmp_pdu    *pdu;
	long            sysuptime;
	char            csysuptime[20];
	char           *trap = NULL;
	//int             exitval = 0;
	int             status;
	uint32_t cbat_ip = 0;
	uint32_t cbat_mk = 0;
	uint32_t cbat_gw = 0;
	uint32_t cbat_ts = 0;
	st_dbsSnmp snmpInfo;
	uint8_t speername[32] = {0};
	uint8_t svalue[16] = {0};

	/* 告警及事件OID  定义*/
	uint8_t *trap_00_oid = "1.3.6.1.4.1.17409.2.1";			/* 未知告警*/
	uint8_t *trap_01_oid = "1.3.6.1.4.1.17409.8080.2.1";		/* CLT上下线*/
	uint8_t *trap_02_oid = "1.3.6.1.4.1.17409.8080.2.2";		/* CNU上下线*/
	uint8_t *trap_03_oid = "1.3.6.1.4.1.17409.8080.1.1";		/* 环境温度告警以及恢复*/
	uint8_t *trap_04_oid = "1.3.6.1.4.1.17409.8080.1.2";		/* CBAT管理CPU负载过高告警以及恢复*/
	uint8_t *trap_05_oid = "1.3.6.1.4.1.17409.8080.1.3";		/* CBAT内存利用率过高告警*/
	uint8_t *trap_06_oid = "1.3.6.1.4.1.17409.8080.1.4";		/* 噪声过高告警*/
	uint8_t *trap_07_oid = "1.3.6.1.4.1.17409.8080.1.5";		/* 链路衰减告警*/
	uint8_t *trap_08_oid = "1.3.6.1.4.1.17409.8080.1.6";		/* 物理层速率告警*/
	uint8_t *trap_09_oid = "1.3.6.1.4.1.17409.8080.2.3";		/*  升级状态告警标识OID */
	uint8_t *trap_10_oid = "1.3.6.1.4.1.17409.8080.2.4";		/* 自动配置失败事件*/
	uint8_t *trap_11_oid = "1.3.6.1.4.1.17409.8080.2.5";		/* INDEX重复告警以及恢复*/
	uint8_t *trap_12_oid = "1.3.6.1.4.1.17409.8080.2.6";		/* 非法CNU注册告警*/
	uint8_t *trap_13_oid = "1.3.6.1.4.1.17409.8080.2.7";		/* CNU用户数量超限告警*/
	uint8_t *trap_14_oid = "1.3.6.1.4.1.17409.8080.2.8";		/* block cnu */
	uint8_t *trap_15_oid = "1.3.6.1.4.1.17409.8080.2.9";		/* auto-config pib */
	uint8_t *trap_16_oid = "1.3.6.1.4.1.17409.8080.2.10";		/* auto-config mod */
	uint8_t *trap_17_oid = "1.3.6.1.4.1.17409.8080.2.11";		/* abort auto config */
	uint8_t *trap_18_oid = "1.3.6.1.4.1.17409.8080.2.12";		/* kick off cnu */
	uint8_t *trap_19_oid = "1.3.6.1.4.1.17409.8080.2.13";		/* force re-registration */
	uint8_t *trap_20_oid = "1.3.6.1.4.1.17409.8080.2.14";		/* CBAT ColdStart */
	uint8_t *trap_21_oid = "1.3.6.1.4.1.17409.8080.2.15";		/* CBAT down */
	uint8_t *trap_22_oid = "1.3.6.1.4.1.17409.8080.2.16";		/* clt heartbeat loss */
	uint8_t *trap_23_oid = "1.3.6.1.4.1.17409.8080.2.17";		/* can not find clt */
	uint8_t *trap_24_oid = "1.3.6.1.4.1.17409.8080.2.18";		/* 心跳TRAP-CBAT状态*/
	uint8_t *trap_25_oid = "1.3.6.1.4.1.17409.8080.2.19";		/* 心跳TRAP-CNU状态*/
	uint8_t *trap_26_oid = "1.3.6.1.4.1.17409.8080.2.20";		/* can not get cbat temperature */

	assert( NULL != alarminfo );

	snmpInfo.id = 1;
	if( CMM_SUCCESS != dbsGetSnmp(dbsdev, snmpInfo.id, &snmpInfo) )
	{
		fprintf(stderr, "ERROR: send_alarm_notification_to_snmp->dbsGetSnmp\n");
		return;
	}
	else
	{
		cbat_ip = inet_addr(g_networkInfo.col_ip);
		cbat_mk = inet_addr(g_networkInfo.col_netmask);
		cbat_gw = inet_addr(g_networkInfo.col_gw);
		cbat_ts = inet_addr(snmpInfo.col_sina);
		/* 如果TrapServer 跨网段，且没有配置网关，则不发送告警*/
		if((cbat_ip & cbat_mk) != (cbat_ts & cbat_mk))
		{
			if((cbat_ip & cbat_mk) != (cbat_gw & cbat_mk))
			{
				fprintf(stderr, "WARNNING: send_alarm_notification_to_snmp: trap server unreachable\n");
				return;
			}
		}
	}

	switch(alarminfo->AlarmCode)
	{
		case 200901:
		{
			strcpy(alarminfo->oid, trap_01_oid);
			break;
		}
		case 200902:
		{
			strcpy(alarminfo->oid, trap_02_oid);
			break;
		}
		case 200903:
		{
			strcpy(alarminfo->oid, trap_03_oid);
			break;
		}
		case 200904:
		{
			strcpy(alarminfo->oid, trap_04_oid);
			break;
		}
		case 200905:
		{
			strcpy(alarminfo->oid, trap_05_oid);
			break;
		}
		case 200906:
		{
			strcpy(alarminfo->oid, trap_06_oid);
			break;
		}
		case 200907:
		{
			strcpy(alarminfo->oid, trap_07_oid);
			break;
		}
		case 200908:
		{
			strcpy(alarminfo->oid, trap_08_oid);
			break;
		}
		case 200909:
		{
			strcpy(alarminfo->oid, trap_09_oid);
			break;
		}
		case 200910:
		{
			strcpy(alarminfo->oid, trap_10_oid);
			break;
		}
		case 200911:
		{
			strcpy(alarminfo->oid, trap_11_oid);
			break;
		}
		case 200912:
		{
			strcpy(alarminfo->oid, trap_12_oid);
			break;
		}
		case 200913:
		{
			strcpy(alarminfo->oid, trap_13_oid);
			break;
		}
		case 200914:
		{
			strcpy(alarminfo->oid, trap_14_oid);
			break;
		}
		case 200915:
		{
			strcpy(alarminfo->oid, trap_15_oid);
			break;
		}
		case 200916:
		{
			strcpy(alarminfo->oid, trap_16_oid);
			break;
		}
		case 200917:
		{
			strcpy(alarminfo->oid, trap_17_oid);
			break;
		}
		case 200918:
		{
			strcpy(alarminfo->oid, trap_18_oid);
			break;
		}
		case 200919:
		{
			strcpy(alarminfo->oid, trap_19_oid);
			break;
		}
		case 200920:
		{
			strcpy(alarminfo->oid, trap_20_oid);
			break;
		}
		case 200921:
		{
			strcpy(alarminfo->oid, trap_21_oid);
			break;
		}
		case 200922:
		{
			strcpy(alarminfo->oid, trap_22_oid);
			break;
		}
		case 200923:
		{
			strcpy(alarminfo->oid, trap_23_oid);
			break;
		}
		case 200000:
		{
			strcpy(alarminfo->oid, trap_24_oid);
			break;
		}
		case 200001:
		{
			strcpy(alarminfo->oid, trap_25_oid);
			break;
		}
		case 200924:
		{
			strcpy(alarminfo->oid, trap_26_oid);
			break;
		}
		default:
		{
			strcpy(alarminfo->oid, trap_00_oid);
			break;
		}
	}
	
	snmp_sess_init(&session);
	
	session.version = SNMP_VERSION_2c;
	sprintf(speername, "%s:%d", snmpInfo.col_sina, snmpInfo.col_tpa);
	session.peername = speername;
	session.community = snmpInfo.col_trapcom;
	session.community_len = strlen(snmpInfo.col_trapcom);
	snmp_enable_stderrlog();
	init_snmp("snmpapp");
	
	session.callback = snmp_input;
	session.callback_magic = NULL;
	
	
 	ss = snmp_add(&session, netsnmp_transport_open_client("snmptrap", session.peername), NULL, NULL);
	if (ss == NULL)
	{
        	snmp_sess_perror("snmptrap", &session);
	       return;
	}
    
	pdu = snmp_pdu_create(SNMP_MSG_TRAP2);
    	sysuptime = get_uptime();
    	sprintf(csysuptime, "%ld", sysuptime);
    	trap = csysuptime;
	/*  绑定告警时间*/
    	snmp_add_var(pdu, objid_sysuptime, sizeof(objid_sysuptime)/sizeof(oid), 't', trap);
    	/*  绑定告警标识OID  */
	snmp_add_var(pdu, objid_snmptrap, sizeof(objid_snmptrap) / sizeof(oid), 'o', alarminfo->oid);
	/*  绑定告警码*/
	sprintf(svalue, "%d", alarminfo->AlarmCode);
	snmp_add_var(pdu, objid_alarmcode, sizeof(objid_alarmcode) / sizeof(oid), 'i', svalue);
	/*  绑定告警信息*/
	snmp_add_var(pdu, objid_trapinfo, sizeof(objid_trapinfo) / sizeof(oid), 's', alarminfo->trap_info);
	/*  绑定告警流水号*/
	sprintf(svalue, "%d", alarminfo->SerialFlow);
	snmp_add_var(pdu, objid_serialflow, sizeof(objid_serialflow) / sizeof(oid), 'i', svalue);
	/*  绑定告警主机MAC  */
	snmp_add_var(pdu, objid_hostmac, sizeof(objid_hostmac) / sizeof(oid), 's', g_networkInfo.col_mac);
	/*  绑定CLT 索引号*/
	sprintf(svalue, "%d", alarminfo->CltIndex);
	snmp_add_var(pdu, objid_cltid, sizeof(objid_cltid) / sizeof(oid), 'i', svalue);
	/*  绑定CNU 索引号*/
	sprintf(svalue, "%d", alarminfo->CnuIndex);
	snmp_add_var(pdu, objid_cnuid, sizeof(objid_cnuid) / sizeof(oid), 'i', svalue);
	/*  绑定告警类型*/
	sprintf(svalue, "%d", alarminfo->AlarmType);
	snmp_add_var(pdu, objid_alarmtype, sizeof(objid_alarmtype) / sizeof(oid), 'i', svalue);
	/*  绑定告警参数值*/
	sprintf(svalue, "%d", alarminfo->AlarmValue);
	snmp_add_var(pdu, objid_alarmvalue, sizeof(objid_alarmvalue) / sizeof(oid), 'i', svalue);
        
	status = (snmp_send(ss, pdu) == 0);

	if (status)
	{
		snmp_sess_perror("snmptrap", ss);
		snmp_free_pdu(pdu);
	} 

	snmp_close(ss);
	
}

#endif

int Write_alarmlog(stAlarmInfo *alarminfo)
{
	assert( NULL != alarminfo );

	st_dbsAlarmlog log;
	log.alarmCode = alarminfo->AlarmCode;
	log.alarmType = alarminfo->AlarmType;
	log.alarmValue = alarminfo->AlarmValue;
	log.cltId = alarminfo->CltIndex;
	log.cnuId = alarminfo->CnuIndex;
	log.itemNum = alarminfo->ItemNum;
	log.realTime = alarminfo->RealTime;
	log.serialFlow = alarminfo->SerialFlow;
	strcpy(log.cbatMac, g_networkInfo.col_mac);
	strcpy(log.oid, alarminfo->oid);
	strcpy(log.trap_info, alarminfo->trap_info);
	return dbs_alarm_log(dbsdev, &log);
}

void send_cbat_topology_traps(void)
{
	netsnmp_session session, *ss;	
	netsnmp_pdu    *pdu;
	long            sysuptime;
	char           *trap = NULL;
	int             status;
	st_dbsSnmp snmpInfo;
	stAlarmInfo alarminfo;	
	int i = 0;	
	uint8_t cnu_nums = 0;
	uint32_t cbat_ip = 0;
	uint32_t cbat_mk = 0;
	uint32_t cbat_gw = 0;
	uint32_t cbat_ts = 0;	
	st_dbsCnu cnu;
	uint8_t tmp[128] = {0};
	uint8_t *svalue = NULL;
	
	/* 告警及事件OID  定义*//* 心跳TRAP-CBAT状态*/
	uint8_t *trap_24_oid = "1.3.6.1.4.1.17409.8080.2.18";	

	/* */
	if( CMM_SUCCESS != dbsGetSnmp(dbsdev, 1, &snmpInfo) )
	{
		fprintf(stderr, "ERROR: send_cbat_topology_traps->dbsGetSnmp\n");
		return;
	}
	else
	{
		cbat_ip = inet_addr(g_networkInfo.col_ip);
		cbat_mk = inet_addr(g_networkInfo.col_netmask);
		cbat_gw = inet_addr(g_networkInfo.col_gw);
		cbat_ts = inet_addr(snmpInfo.col_sina);
		/* 如果TrapServer 跨网段，且没有配置网关，则不发送告警*/
		if((cbat_ip & cbat_mk) != (cbat_ts & cbat_mk))
		{
			if((cbat_ip & cbat_mk) != (cbat_gw & cbat_mk))
			{
				fprintf(stderr, "WARNNING: send_cbat_topology_traps: trap server unreachable\n");
				return;
			}
		}
	}
	
	svalue = (uint8_t *)malloc(768);
	if( NULL == svalue) return;

	alarminfo.ItemNum = 0;
	alarminfo.AlarmCode = 200002;
	strcpy(alarminfo.oid, trap_24_oid);
	strcpy(alarminfo.trap_info, "cbat keepalive heartbeat");

	/***********************************************************************************************************/
	/*  发送分片1  */
	/***********************************************************************************************************/
	snmp_sess_init(&session);

	session.version = SNMP_VERSION_2c;
	sprintf(tmp, "%s:%d", snmpInfo.col_sina, snmpInfo.col_tpa);
	session.peername = tmp;
	session.community = snmpInfo.col_trapcom;
	session.community_len = strlen(snmpInfo.col_trapcom);
	snmp_enable_stderrlog();
	init_snmp("snmpapp");
	
	session.callback = snmp_input;
	session.callback_magic = NULL;

	ss = snmp_add(&session, netsnmp_transport_open_client("snmptrap", session.peername), NULL, NULL);
	if (ss == NULL)
	{
        	snmp_sess_perror("snmptrap", &session);
		free(svalue);
	       return;
	}
    
	pdu = snmp_pdu_create(SNMP_MSG_TRAP2);
    	sysuptime = get_uptime();
    	sprintf(tmp, "%ld", sysuptime);
    	trap = tmp;

	/*  绑定告警时间*/
    	snmp_add_var(pdu, objid_sysuptime, sizeof(objid_sysuptime)/sizeof(oid), 't', trap);
	/*  绑定告警标识OID  */
	snmp_add_var(pdu, objid_snmptrap, sizeof(objid_snmptrap) / sizeof(oid), 'o', alarminfo.oid);
	/*  绑定告警码*/
	sprintf(svalue, "%d", alarminfo.AlarmCode);
	snmp_add_var(pdu, objid_alarmcode, sizeof(objid_alarmcode) / sizeof(oid), 'i', svalue);
	/*  绑定告警信息*/
	snmp_add_var(pdu, objid_trapinfo, sizeof(objid_trapinfo) / sizeof(oid), 's', alarminfo.trap_info);
	/*  绑定局端基本信息*//* NMS_PROTOCAL_VERSION 被NMS用来进行配置兼容性设计*/
	//sprintf(svalue, "[%s|%s|%d|%d]", g_networkInfo.col_mac, g_networkInfo.col_ip, boardapi_mapDevModel(g_szSysinfo.col_model), NMS_PROTOCAL_VERSION);
	sprintf(svalue, "[%s|%s|%d]", g_networkInfo.col_mac, g_networkInfo.col_ip, boardapi_mapDevModel(g_szSysinfo.col_model));
	snmp_add_var(pdu, objid_cbathb, sizeof(objid_cbathb) / sizeof(oid), 's', svalue);
	/*  绑定终端基本信息分片1  */
	bzero(svalue, 768);
	for( i=1; i<=20; i++ )
	{
		if( CMM_SUCCESS == dbsGetCnu(dbsdev, i, &cnu) )
		{
			if( cnu.col_row_sts )
			{	
				bzero(tmp, 128);
				sprintf(tmp, "[%s|%d|%d|%d|%d|%s]", cnu.col_mac, boardapi_mapDevModel(cnu.col_model), 1, cnu.id, cnu.col_sts,cnu.col_user_hfid);
				strcat(svalue, tmp);
			}
		}
	}
	snmp_add_var(pdu, objid_cnuhb, sizeof(objid_cnuhb) / sizeof(oid), 's', svalue);

	status = (snmp_send(ss, pdu) == 0);

	if (status)
	{
		snmp_sess_perror("snmptrap", ss);
		snmp_free_pdu(pdu);
	}

	snmp_close(ss);

	/***********************************************************************************************************/
	/*  发送分片2  */
	/***********************************************************************************************************/
	cnu_nums = 0;
	snmp_sess_init(&session);

	session.version = SNMP_VERSION_2c;
	sprintf(tmp, "%s:%d", snmpInfo.col_sina, snmpInfo.col_tpa);
	session.peername = tmp;
	session.community = snmpInfo.col_trapcom;
	session.community_len = strlen(snmpInfo.col_trapcom);
	snmp_enable_stderrlog();
	init_snmp("snmpapp");
	
	session.callback = snmp_input;
	session.callback_magic = NULL;

	ss = snmp_add(&session, netsnmp_transport_open_client("snmptrap", session.peername), NULL, NULL);
	if (ss == NULL)
	{
        	snmp_sess_perror("snmptrap", &session);
		free(svalue);
	       return;
	}
    
	pdu = snmp_pdu_create(SNMP_MSG_TRAP2);
    	sysuptime = get_uptime();
    	sprintf(tmp, "%ld", sysuptime);
    	trap = tmp;

	/*  绑定告警时间*/
    	snmp_add_var(pdu, objid_sysuptime, sizeof(objid_sysuptime)/sizeof(oid), 't', trap);
	/*  绑定告警标识OID  */
	snmp_add_var(pdu, objid_snmptrap, sizeof(objid_snmptrap) / sizeof(oid), 'o', alarminfo.oid);
	/*  绑定告警码*/
	sprintf(svalue, "%d", alarminfo.AlarmCode);
	snmp_add_var(pdu, objid_alarmcode, sizeof(objid_alarmcode) / sizeof(oid), 'i', svalue);
	/*  绑定告警信息*/
	snmp_add_var(pdu, objid_trapinfo, sizeof(objid_trapinfo) / sizeof(oid), 's', alarminfo.trap_info);
	/*  绑定局端基本信息*/
	sprintf(svalue, "[%s|%s|%d]", g_networkInfo.col_mac, g_networkInfo.col_ip, boardapi_mapDevModel(g_szSysinfo.col_model));
	snmp_add_var(pdu, objid_cbathb, sizeof(objid_cbathb) / sizeof(oid), 's', svalue);
	/*  绑定终端基本信息分片1  */
	bzero(svalue, 768);
	for( i=21; i<=40; i++ )
	{
		if( CMM_SUCCESS == dbsGetCnu( dbsdev, i, &cnu) )
		{
			if( cnu.col_row_sts )
			{
				cnu_nums++;
				bzero(tmp, 128);
				sprintf(tmp, "[%s|%d|%d|%d|%d|%s]", cnu.col_mac, 	boardapi_mapDevModel(cnu.col_model), 1, cnu.id, cnu.col_sts,cnu.col_user_hfid);
				strcat(svalue, tmp);
			}
		}
	}
	snmp_add_var(pdu, objid_cnuhb, sizeof(objid_cnuhb) / sizeof(oid), 's', svalue);

	if( 0 != cnu_nums )
	{
		status = (snmp_send(ss, pdu) == 0);
		if (status)
		{
			snmp_sess_perror("snmptrap", ss);
			snmp_free_pdu(pdu);
		}
	}
	else
	{
		snmp_free_pdu(pdu);
	}	

	snmp_close(ss);

	/***********************************************************************************************************/
	/*  发送分片3  */
	/***********************************************************************************************************/
	cnu_nums = 0;
	snmp_sess_init(&session);

	session.version = SNMP_VERSION_2c;
	sprintf(tmp, "%s:%d", snmpInfo.col_sina, snmpInfo.col_tpa);
	session.peername = tmp;
	session.community = snmpInfo.col_trapcom;
	session.community_len = strlen(snmpInfo.col_trapcom);
	snmp_enable_stderrlog();
	init_snmp("snmpapp");
	
	session.callback = snmp_input;
	session.callback_magic = NULL;

	ss = snmp_add(&session, netsnmp_transport_open_client("snmptrap", session.peername), NULL, NULL);
	if (ss == NULL)
	{
        	snmp_sess_perror("snmptrap", &session);
		free(svalue);
	       return;
	}
    
	pdu = snmp_pdu_create(SNMP_MSG_TRAP2);
    	sysuptime = get_uptime();
    	sprintf(tmp, "%ld", sysuptime);
    	trap = tmp;

	/*  绑定告警时间*/
    	snmp_add_var(pdu, objid_sysuptime, sizeof(objid_sysuptime)/sizeof(oid), 't', trap);
	/*  绑定告警标识OID  */
	snmp_add_var(pdu, objid_snmptrap, sizeof(objid_snmptrap) / sizeof(oid), 'o', alarminfo.oid);
	/*  绑定告警码*/
	sprintf(svalue, "%d", alarminfo.AlarmCode);
	snmp_add_var(pdu, objid_alarmcode, sizeof(objid_alarmcode) / sizeof(oid), 'i', svalue);
	/*  绑定告警信息*/
	snmp_add_var(pdu, objid_trapinfo, sizeof(objid_trapinfo) / sizeof(oid), 's', alarminfo.trap_info);
	/*  绑定局端基本信息*/
	sprintf(svalue, "[%s|%s|%d]", g_networkInfo.col_mac, g_networkInfo.col_ip, boardapi_mapDevModel(g_szSysinfo.col_model));
	snmp_add_var(pdu, objid_cbathb, sizeof(objid_cbathb) / sizeof(oid), 's', svalue);
	/*  绑定终端基本信息分片1  */
	bzero(svalue, 768);
	for( i=41; i<=MAX_CNU_AMOUNT_LIMIT; i++ )
	{
		if( CMM_SUCCESS == dbsGetCnu(dbsdev,  i, &cnu) )
		{
			if( cnu.col_row_sts )
			{
				cnu_nums++;
				bzero(tmp, 128);
				sprintf(tmp, "[%s|%d|%d|%d|%d|%s]", cnu.col_mac, 	boardapi_mapDevModel(cnu.col_model), 1, cnu.id, cnu.col_sts,cnu.col_user_hfid);
				strcat(svalue, tmp);
			}
		}
	}
	snmp_add_var(pdu, objid_cnuhb, sizeof(objid_cnuhb) / sizeof(oid), 's', svalue);

	if( 0 != cnu_nums )
	{
		status = (snmp_send(ss, pdu) == 0);
		if (status)
		{
			snmp_sess_perror("snmptrap", ss);
			snmp_free_pdu(pdu);
		}
	}
	else
	{
		snmp_free_pdu(pdu);
	}
	
	snmp_close(ss);
	/***********************************************************************************************************/
	/***********************************************************************************************************/	
	free(svalue);
}

void send_top_heartbeat(void)
{	
#if 0
	/* 延迟发送*/
	int i = 0;	
	
	if( hb_flag < 10 )
	{
		hb_flag++;
		return;
	}	
	
	/* 上报CBAT 的状态*/
	send_cbat_status_heartbeat();
	for( i=1; i<=MAX_CNU_AMOUNT_LIMIT; i++ )
	{
		/* 上报CNU 的状态*/
		send_cnu_status_heartbeat(1, i);
		/* 防止操作数据库太快*/
		//usleep(5000);
	}
#else
	send_cbat_topology_traps();
#endif
}

void process_alarm(void)
{
	stAlarmInfo alarminfo;
	struct timeval start, end;
	
	while(1)
	{
		if( CMM_SUCCESS == recieve_alarm_notification(&alarminfo) )
		{
			if( 300000 == alarminfo.AlarmCode )
			{
				g_hb_enabled = (alarminfo.AlarmValue?1:0);
			}
			else if( 200001 == alarminfo.AlarmCode )
			{
				if(g_hb_enabled)
				{
					gettimeofday( &start, NULL );
					/* 处理心跳消息*/
					send_top_heartbeat();
					gettimeofday( &end, NULL );
					if( (int)(end.tv_sec - start.tv_sec) > 10 )
					{
						/* 借用alarminfo.trap_info 作为临时存储*/
						sprintf(alarminfo.trap_info, "heartbeat time is too long [%ds]", (int)(end.tv_sec - start.tv_sec));
						dbs_sys_log(dbsdev, DBS_LOG_WARNING, alarminfo.trap_info);
					}
				}
			}
			else
			{
				alarminfo.SerialFlow = ++serialFlowNumber;
				/*把处理过的消息发送给SNMP 模块*/
				send_alarm_notification_to_snmp(&alarminfo);
				/*将告警日志信息写入数据库*/
				Write_alarmlog(&alarminfo);
			}
		}
	}
}

int main(void)
{
	/*初始化与DB 模块通信套接字*/
	dbsdev = dbsOpen(MID_ALARM);
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: alarm->dbsOpen error, exited !\n");
		return -1;
	}
	
	/*初始化告警监听服务端套接字*/
	SK_ALARM.desc = DESC_SOCKET_ALARM;	
	if( CMM_SUCCESS != init_socket(&SK_ALARM) )
	{
		fprintf(stderr, "ERROR: alarm->init_socket[SK_ALARM], exit !\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "alarm init_socket[SK_ALARM] error, exited");
		dbsClose(dbsdev);
		return -1;
	}

#ifdef SEND_TRAP_BY_AGENT_TRAP
	/*初始化与SNMP 模块通信套接字*/
	SK_TOSNMP.desc = DESC_SOCKET_OTHER;
	if( CMM_SUCCESS != init_socket(&SK_TOSNMP) )
	{
		fprintf(stderr, "ERROR: alarm->init_socket[SK_TOSNMP], exit !\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "alarm init_socket[SK_TOSNMP] error, exited");
		dbsClose(dbsdev);
		return -1;
	}
#else
	/* 初始化SNMPTRAP 需要的信息*/
	if( CMM_SUCCESS != init_snmptrap())
	{
		fprintf(stderr, "ERROR: alarm->init_snmptrap, exit !\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "alarm init_snmptrap error, exited");
		dbsClose(dbsdev);
		return -1;
	}
#endif

	/*初始化与DB 模块通信套接字*/
	//SK_TODB.desc = DESC_SOCKET_DB;
	//init_socket(&SK_TODB);
	
	/* 注册异常退出句柄函数*/
	signal(SIGTERM, SignalProcessHandle);

	dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting module alarm success");
	printf("Starting module ALARM		......		[OK]\n");

	/* 循环处理外部请求*/
	process_alarm();

	/* 不要在这个后面添加代码，执行不到滴*/
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "module alarm exit");
	close(SK_ALARM.sk);
#ifdef SEND_TRAP_BY_AGENT_TRAP
	close(SK_TOSNMP.sk);
#endif
	//close(SK_TODB.sk);
	dbsClose(dbsdev);
	return 0;
}

