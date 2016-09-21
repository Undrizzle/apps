/********************************************************
**  �ļ���:public.h
**  ����:���ļ�������ģ��֮����ⲿ�ӿ�
**  ��:�����ĺ궨�壬ģ���ͨѶ�õ��Ĺ�
**         ����Ϣ��ṹ���Լ�����ͨ�����ݱ�
**         �ͷ���ֵ��
**  ����:frank
**  ʱ��:2010��7��20��
*********************************************************/

#ifndef __MID_PUBLIC_H__
#define __MID_PUBLIC_H__

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "wecplatform.h"

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#ifdef SEND_TRAP_BY_AGENT_TRAP
#undef SEND_TRAP_BY_AGENT_TRAP
#endif

#define DATABASES_PATH "/var/flashdisk/etc/databases/db_cbat.db"
#define LOCAL_INTERFACE0		"eth0"

#define FIRMWARE_DOWNLOADS_PATH "/var/downloads/firmware.img"
#define FIRMWARE_MD5_DOWNLOADS_PATH "/var/downloads/firmware.img.md5"

//#define WEC3502I_SOCKET_UN_ENABLE 1
/********************************************************/
enum
{
	CUSTOM_LOGO_DEMO = 0,
	CUSTOM_LOGO_PREVAIL,
	CUSTOM_LOGO_PX,
	CUSTOM_LOGO_ALCOTEL
};
#define CUSTOM_LOGO_ID CUSTOM_LOGO_DEMO
//#define CUSTOM_LOGO_ID CUSTOM_LOGO_PX
/********************************************************/

/********************************************************/
/* define __AT30TK175STK__ to enable temperature sensor driver */
//#undef __AT30TK175STK__
#define __AT30TK175STK__

#define SYS_TEMPERATURE_ALARM_HIHI	(60)
#define SYS_TEMPERATURE_ALARM_HI		(55)
#define SYS_TEMPERATURE_ALARM_LO		(5)		/* -5 */
#define SYS_TEMPERATURE_ALARM_LOLO	(15)		/* -15 */
/********************************************************/
//bootstrap.uboot.kernel.version-cr(Revised number)
/********************************************************/
#define SYSINFO_APP_VERSION		"v1.3.6.2-cr25"
#define SYSINFO_BOOT_VERSION		"U-boot-1.3.4"
#define SYSINFO_KERNEL_VERSION	"Linux-3.4.6"
#define SYSINFO_HW_VERSION		"v1.0.2"
#define SYSINFO_APP_HASH			"6802b21ad8725bdbe48b1cca609c765ac0f196a4"
#define SYSINFO_FLASH_SIZE			256
#define SYSINFO_RAM_SIZE			64
/* NMS management protocal version */
#define NMS_PROTOCAL_VERSION		2
/********************************************************/

/********************************************************/
#define SNMP_ALARM_PORT 			1225    /*�澯��Ϣ�����˿�*/
#define REG_CORE_LISTEN_PORT		2001    /*ע��ģ������˿�*/
#define MMEAD_LISTEN_PORT			2010
#define DB_ACCESS_LISTEN_PORT		2012
#define DBS_LISTEN_PORT			2013
#define SYSMONITOR_LISTEN_PORT	2014
#define TM_CORE_LISTEN_PORT		2016    /*ģ�����ģ������˿�*/
#define CMM_LISTEN_PORT			2018
#define SERV_PORT 					2250    /*��־ģ������˿�*/
#define ALARM_LISTEN_PORT 			2345    /*�澯ģ������˿�*/
/********************************************************/
#define MAX_ROW                                   512     /*������־���洢����*/
#define ALARM_MAX_ROW                      256     /*�澯��־���洢����*/
#define MAX_CNUS_PER_CLT			64		/* max cnus per clt supported */
#define MMEAD_MAX_CNU_NUM		MAX_CNUS_PER_CLT
#define MAX_CNU_AMOUNT_LIMIT		(MAX_CNUS_PER_CLT*MAX_CLT_AMOUNT_LIMIT)
#define MAX_CNU_PORTLIMIT			4		/*cnu port max num*/
#define MAX_CNU_AMOUNTLIMIT		2		/*the amount limitation of cnu ports*/
#define MAX_SQL_LEN 				512		/*sql �ַ�������*/
#define SIOCGMIIREG 					0x8948
#define SIOCSMIIREG 					0x8949	/*write PHY reg*/
#define QUREY_ROW_ONCE			5    	/*��־ÿ�β�ѯ����*/
#define DEV_MAX_PORT				4    	/*�豸��� �˿���*/
/********************************************************/

/********************************************************/
#define MAX_MME_SIZE    				1500
#define MAX_UDP_SIZE    				1472
#define MAX_BODY_SIZE				1280
/********************************************************/

/********************************************************/
#define MMEAD_MSG_ID				0xCC08
/********************************************************/

#define MAX_MODULE_NUMS 32
/* MID �����*/
/*#define MID_SNMP 1
#define MID_CLI     2
#define MID_HTTP  3
#define MID_CMM   4*/

typedef uint8_t BOOLEAN;

/* ������־�ȼ� */
enum
{
	DBS_LOG_EMERG = 0,			/* An emergency situation */
	DBS_LOG_ALERT,				/* High-priority problem, such as database corruption */
	DBS_LOG_CRIT,				/* Critical error, such as hardware failure */
	DBS_LOG_ERR,				/* Errors */
	DBS_LOG_WARNING,			/* Warning */
	DBS_LOG_NOTICE,			/* Special conditions requiring attention */
	DBS_LOG_INFO,				/* Informational messages */
	DBS_LOG_DEBUG				/* Debug messages */
};

/* ����ͨ�ô����� */
enum
{
	CMM_SUCCESS = 0,
	CMM_FAILED,
	CMM_OUT_OF_MEMERY,
	CMM_SYS_BUSY,
	CMM_CREATE_SOCKET_ERROR,
	CMM_MME_ERROR,
	CMM_UNKNOWN_PROG,
	CMM_UNKNOWN_MTYPE,
	CMM_UNKNOWN_DEVTYPE,
	CMM_UNKNOWN_MMTYPE,
	CMM_BUFFER_OVERFLOW,
	CMM_MORE_SLICE,
	CMM_DB_ACCESS_ERROR,
	CMM_DELETE_FILE_ERROR,	 /*ɾ���ļ�ʧ��*/
	CMM_OPEN_FILE_ERROR,	 /*���ļ�ʧ��*/
	CMM_GET_MAC_ERROR,
	CMM_DB_GETCNU_ERROR,   /*��ȡCNU ��Ϣ����*/
	CMM_DB_UPDATECNU_ERROR,   /*����CNU ��Ϣ����*/
	CMM_ANONYMOUS_CNU,	/*�����û�*/
	CMM_INVALID_CNUPORT,	/*��Ч�Ķ˿ں�*/
	CMM_OFFLINE_CNU,	/*���ߵ��û�*/
	CMM_VLAN_NOT_ENABLE,	/*VLANδʹ��*/
	CMM_CONFIG_REPEAT,	/*�ظ�������*/
	TM_DB_GETCNU_ERROR,   /*��ȡCNU ��Ϣ����*/
	TM_DB_GETTEMPLATE_ERROR,   /*��ȡTEMPLATE ��Ϣ����*/
	TM_DB_GETCOUNT_ERROR,   /*��ȡ������ ��Ϣ����*/
	TM_DB_UPDATE_TEMPLATE_ERROR,   /*����������Ϣ����*/
	TM_DB_INSERT_TEMPLATE_ERROR,   /*����������Ϣ����*/
	TM_DB_UPDATE_ROW_ERROR,   /*��������Ϣ����*/
	TM_DB_UPDATE_INT_ERROR,   /*����INT��Ϣ����*/
	TM_DB_GET_TEMPID_ERROR,   /*��ȡtempid ����*/
	TM_DB_DELET_ROW_DISABLE,   /*�޷�ɾ������*/
	TM_DB_MAX_ROW,   			/*���ݱ�ﵽ�������*/
	TM_DB_NO_ROW,   			/*�����ڵ���*/
	TM_CNUONLINE_DELERROR,   	/*CNU�����޷�ɾ��*/	
	TM_CNU_NOTEXIST,   	/*�����ڵ�CNU*/
	TM_PROFILE_NOT_COMPATIBLE,	/* PROFILE ��CNU������*/
	TM_WL_DISABLED,
	
	/* �����м��������������*/
	
	CMM_UNKNOWN_ERR = 0xFF
};

/* ����EoC�������� */
enum
{
	EOC_PROG_ATHEROS = 1,
	EOC_PROG_SPC,
	EOC_PROG_HOMEPNA,
	EOC_PROG_MOCA,

	/* �����м���������ö�ٶ��� */
	EOC_PROG_OTHER
};

enum
{
	DEV_CLT = 0,
	DEV_CNU,
	DEV_CBAT,

	DEV_OTHER
};

enum
{
	CNU_SWITCH_TYPE_AR8236 = 0,
	CNU_SWITCH_TYPE_RTL8306E = 1,

	CNU_SWITCH_TYPE_OTHER
};


/*����澯����*/
enum
{
	ALARM_CLT_ON_OFF=1,
	ALARM_CNU_ON_OFF,
	ALARM_CBAT_COLDSTART,
	ALARM_CBAT_DOWN,
	ALARM_TEMPERATOR,
	ALARM_CBAT_CPU,
	ALARM_CBAT_MEM,
	ALARM_CBAT_NOISE,
	ALARM_CBAT_ATT,
	ALARM_CBAT_PHY,
	ALARM_CBAT_AUTOUPDATE_FAILED,
	ALARM_CBAT_AUTOCONFIG_FAILED,
	ALARM_CBAT_INDEX
	
};

/* ����CLT/CNU Ӳ��ƽ̨ϵ��*/
enum
{
	CLT_INT6400_NOSW = 8,
	CNU_INT6400_NOSW = 2,	
	CNU_INT6400_RTL8306SD = 3,	/* WEC-3602I, WEC-3604I */
	CNU_INT6400_AR8236 = 5,		/* WEC-3702I, WEC-3703I, WEC-602, WEC-604 */
	CNU_INT6400_IP175D = 9,
	CNU_INT6400_88E6031 = 10,
	
	/* �����м���������ö�ٶ��� */

	CNU_OTHER_PLATFORM = 0xFF
};

/* �����豸���� */
enum
{
	WEC_3501I = 1,
	WEC_3601I,		/*1����INT6400�ն�*/
	WEC_3602I,		/*2����INT6400�ն�*/
	WEC_3604I,		/*4����INT6400�ն�*/
	WEC_3702I,		/*2����INT6400�ն�*/
	WEC_3703I,		/*2����INT6400�ն�*/
	WEC_3704I,		/*4����INT6400�ն�*/
	WEC_3801I,		/*ͨ��INT6400�߿�*/
	WEC_3502I,
	WEC_3504I,		/*10*/
	WEC_3501S,
	WEC_3502S,
	WEC_3504S,
	WEC_602,
	WEC_604,
	WEC_3501I_X7,
	WEC_3501I_E31,
	WEC_3501I_C22,
	WEC_3501I_S220,
	WEC_3501I_S60,	/*20*/
	WEC_3501I_Q31,
	WEC9720EK_C22,
	WEC9720EK_E31,
	WEC9720EK_Q31,
	WEC9720EK_S220,
	WEC9720EK_SD220,
	WEC701_M0,
	WEC701_C2,
	WEC701_C4,
	WEC_3501I_XD25,	/* 30: du channel */
	WEC9720EK_XD25,	/* du channel */
	WR1004JL,			/* 1*clt + 1*onu module */
	WR1004SJL,			/* 4*clt + 1*onu module */
	WEC_3702I_E4,		/* cnu ar6400+rtl8306e */
	WEC701_E4,			/* cnu ar7411+rtl8306e */
	WR1004JLD,			/* 2*clt + 1*onu module */
	WEC701_L4,			/* cnu ar7411L+rtl8306e */
	WEC701_W4,                      /* cnu ar7411L+AR9331+rtl8306e*/
	WEC701W_C4,
	/* �����м���������ö�ٶ��� */
	WEC_INVALID = 0xFE,
	WEC_XXXXX = 0xFF
};

/* ����CMM�Ĳ����� */
enum
{	
	CMM_TOPOLOGY_SHOW = 1,
	CMM_CNU_VLAN_SHOW,
	CMM_CNU_VLAN_DISABLE,
	CMM_CNU_VLAN_CONFIG,
	CMM_CBAT_RESET,
	CMM_CLT_RESET,
	CMM_CNU_RESET,
	CMM_ALL_RESET,
	CMM_SAVE_CONFIG,
	CMM_RESTORE_DEFAULT,			/* 10 */
	CMM_MGMT_VLAN_SHOW,
	CMM_MGMT_VLAN_DISABLE,
	CMM_MGMT_VLAN_CONFIG,
	CMM_NETWORK_INFO_SHOW,
	CMM_NETWORK_INFO_CONFIG,
	CMM_CNU_FIRMWARE_UPLOAD,
	CMM_OPT_LOG_SHOW,
	CMM_ALARM_LOG_SHOW,
	CMM_CLI_GET_USERINFO,
	CMM_CLI_SET_USERPASSWORD,	/* 20 */
	CMM_CLI_SET_IP,
	CMM_CLI_GET_IP,
	CMM_CLI_IP_UNDO,
	CMM_CLI_SET_IPGATEWAY,
	CMM_CLI_IPGATEWAY_UNDO,
	CMM_CLI_SET_MVLAN,
	CMM_CLI_GET_MVLAN,
	CMM_CLI_UNDO_MVLAN,
	CMM_CLI_GET_SYSINFO,
	CMM_CLI_GET_STORMINFO,
	CMM_CLI_GET_RATEINFO,
	CMM_CLI_GET_CUR_PROFILE,
	CMM_CLI_GET_PROFILE,
	CMM_CLI_MVLAN_UNDO,
	CMM_CLI_SET_FTPSERVER,
	CMM_CLI_GET_FTP,
	CMM_CLI_UPGRADE,
	CMM_CLI_FLOWCONTROL,
	CMM_CLI_SHUTDOWN,
	CMM_CLI_UNDOSHUTDOWN,		/* 40 */
	CMM_CLI_STROMCONTROL,
	CMM_CLI_MUTI_STROM_EN,
	CMM_CLI_UNI_STROM_EN,
	CMM_CLI_BROAD_STROM_EN,
	CMM_CLI_ACCESS_EN,
	CMM_CLI_SEND_CONFIG,
	CMM_CLI_ACCESS_DISABLE,
	CMM_CLI_DELETE_PRO,
	CMM_CLI_DELETE_USER,
	CMM_CLI_USER_NEW,
	CMM_CLI_USER_DEL,
	CMM_CLI_USER_EDIT,
	CMM_CLI_GET_CNU,
	CMM_CLI_GET_ANONYMOUS,
	CMM_CLI_GET_USERS,
	CMM_MODULE_DEBUG_CONTROL,
	CMM_SEND_ALARM_NOTIFICATION,
	CMM_AR8236_PHY_REG_READ,			/* read AR8236 internal PHY register*/
	CMM_AR8236_PHY_REG_WRITE,			/* write AR8236 internal PHY register*/
	CMM_AR8236_SW_REG_READ,				/* Read AR8236 internal register*/
	CMM_AR8236_SW_REG_WRITE,			/* write AR8236 internal register*/
	CMM_AR8236_VLAN_DISPALY,				/* display AR8236 vlan */
	CMM_AR8236_VLAN_ENABLE,				/* enable AR8236 vlan */
	CMM_AR8236_VLAN_DISABLE,				/* disable AR8236 vlan */
	CMM_AR8236_VLAN_SET,					/* set AR8236 vlan */
	CMM_ANONYMOUS_CONTROL,
	CMM_READ_SNMP_CONF,
	CMM_WRITE_SNMP_CONF,
	CMM_WRITE_NETWORK_CONF,
	CMM_EOC_ALL_RESET,					/* 70 */
	CMM_SYSLOG_SHOW,
	CMM_CLI_UNDO_IPADDR,
	CMM_CLI_UNDO_IP_GW,
	CMM_DO_WLIST_CONTROL,
	CMM_UNDO_WLIST_CONTROL,
	CMM_CREATE_CNU,
	CMM_DUMP_CNU_REG,
	CMM_DUMP_CNU_MOD,
	CMM_DO_MAC_LIMIT_CONFIG,
	CMM_CLI_DO_AGING_TIME_CONFIG,		/* 80 */
	CMM_CLI_DO_QOS_CONFIG,
	CMM_DUMP_CNU_PIB, 
	CMM_DO_WDT_CONTROL,
	CMM_UNDO_WDT_CONTROL,
	CMM_SEND_TOPOLOGY_HB_TRAP,
	CMM_DO_HB_TRAP_CTRL,
	CMM_UNDO_HB_TRAP_CTRL,
	CMM_DO_LINK_DIAG,
	CMM_GET_IP175D_PORT_PROPETY,
	CMM_GET_PORT_STAT,					/* 90 */
	CMM_CLEAR_PORT_STAT,
	CMM_DEBUG_PRINT_PORT_STAT,
	CMM_GET_DSDT_RGMII_DELAY,
	CMM_SET_DSDT_RGMII_DELAY,
	CMM_GET_CBAT_TEMPERATURE,
	CMM_SET_DSDT_PORT_MIRRORING,
	CMM_SYSMONITOR_LED_CONTROL,
	CMM_MME_MDIO_READ,			/* read  PHY register by mme*/
	CMM_MME_MDIO_WRITE,			/* write  PHY register by mme*/
	CMM_ADD_ATHEROS_ADDR,		/* 100 */
	CMM_DEL_ATHEROS_ADDR,		/* delete atheros multicast address from cable port */
	CMM_GET_CLT_PORT_LINK_STS,	/* get clt port link status to check if there is a clt connected to the switch port */
	CMM_CNU_SWITCH_READ,			/* read cnu switch register*/
	CMM_CNU_SWITCH_WRITE,		/* write cnu switch register*/
	CMM_CNU_SWITCH_CONFIG_READ, /* read rtl8306e configs */
	CMM_CNU_SWITCH_CONFIG_WRITE,/* write rtl8306e configs */
	CMM_DSDT_MAC_BINDING,
	CMM_DO_CNU_ACL_DROP_MME,
	CMM_UNDO_CNU_ACL_DROP_MME,
	CMM_ERASE_MOD_A,				/* erase mod id=0 */
	CMM_ERASE_MOD_B,				/* erase mod id=1 */
	CMM_GET_88E6171R_PORT_PROPETY_ALL,
	CMM_GET_88E6171R_PORT_STATS_ALL,
	CMM_USER_HFID_READ,
	CMM_GET_RTL8306E_PORT_STATUS_INFO,
	CMM_GET_CLT_FREQUENCY_BAND,
	CMM_SET_CLT_FREQUENCY_BAND,
	CMM_GET_HG_MANAGE,
	CMM_SET_HG_MANAGE,
	CMM_GET_HG_BUSINESS,
	CMM_SET_HG_BUSINESS,
	CMM_REBOOT_HG,
	CMM_RESET_HG,
	CMM_GET_HG_SSID_STATUS,
	CMM_SET_HG_SSID_STATUS,
	CMM_GET_HG_WAN_STATUS,
	/* �����м���������ö�ٶ��� */
	
	CMM_GET_SNMP_TABLE_ROW = 0xF0,
	CMM_GET_SNMP_TABLE_NODE = 0xF1,
	CMM_SET_SNMP_TABLE_NODE = 0xF2,
	CMM_GET_SNMP_LEAF = 0xF3,
	CMM_SET_SNMP_LEAF = 0xF4,
	CMM_GET_ONLINECNU_INDEX = 0xF5,
	CMM_CONNET = 0xF6,    /*���ܸĶ�,����WINDOWS����MAC�����޷�����ʹ��*/
	CMM_TOOL_MAC= 0xF7,  /*���ܸĶ�,����WINDOWS����MAC�����޷�����ʹ��*/
	CMM_TOOL_MAC2= 0xF8,  /*���ܸĶ�,����WINDOWS����MAC�����޷�����ʹ��*/
	CMM_UNKNOWN_OPT = 0xFF
};

/* ����register�Ĳ����� */
enum
{	
	REG_CNURESET = 1,
	REG_CNU_FORCE_REGISTRATION,
	REG_CNU_DELETE,
	REG_CLT_RESET,
	REG_CNU_CREATE,

	REG_UNKNOWN_OPT = 0xFF
};

/*==============================================================*/
/*                                    CMMģ���ⲿ��Ϣ�ӿڶ���                                      */
enum
{
	/* SNMPģ���ʶ�������1��ʼ*/
	MID_SNMP = 1,	
	MID_CLI,
	MID_HTTP,
	MID_CMM,
	MID_ALARM,
	MID_LLOG,
	MID_DBA,
	MID_REGISTER,
	MID_MMEAD,
	MID_SQL,			/*10*/
	MID_TEMPLATE,
	MID_DBS,
	MID_DBS_TESTER,
	MID_SYSMONITOR,
	MID_TM_TESTER,
	MID_SYSEVENT,
	MID_DSDT_TESTER,
	MID_AT91BTN,
	MID_ATM,
	MID_SYSINDI,		/*20*/
	MID_SYSLED,
	MID_WDTIMER,
	MID_REGIEVENT,
	MID_AUTOCONFIG,
	
	/* ����: ģ�鶨�岻Ҫ����32��*/
	MID_OTHER = 32
};

#define MF_SNMP		(1<<(MID_SNMP-1))
#define MF_CLI		(1<<(MID_CLI-1))
#define MF_HTTP		(1<<(MID_HTTP-1))
#define MF_CMM		(1<<(MID_CMM-1))
#define MF_ALARM	(1<<(MID_ALARM-1))
#define MF_REGI		(1<<(MID_REGISTER-1))
#define MF_MMEAD	(1<<(MID_MMEAD-1))
#define MF_TM		(1<<(MID_TEMPLATE-1))
#define MF_DBS		(1<<(MID_DBS-1))
#define MF_SM		(1<<(MID_SYSMONITOR-1))
#define MF_AUTOCONFIG		(1<<(MID_AUTOCONFIG-1))

enum
{
	MMEAD_GET_TOPOLOGY = 1,
	MMEAD_SET_CNU_VLAN,
	MMEAD_DISABLE_CNU_VLAN,
	MMEAD_RESET_DEV,
	MMEAD_UPLOAD_CNU_FIRMWARE,
	MMEAD_GET_CLT_MAC,
	MMEAD_GET_SOFTWARE_VERSION,
	MMEAD_GET_MANUFACTURER_INFO,
	MMEAD_MODULE_MSG_DEBUG_ENABLE,	/* ��MMEADģ����Ϣ���Խӿ�*/
	MMEAD_MODULE_MSG_DEBUG_DISABLE,	/* �ر�MMEADģ����Ϣ���Խӿ�*/
	MMEAD_AR8236_PHY_REG_READ,			/* read AR8236 internal PHY register*/
	MMEAD_AR8236_PHY_REG_WRITE,			/* write AR8236 internal PHY register*/
	MMEAD_AR8236_SW_REG_READ,			/* Read AR8236 internal register*/
	MMEAD_AR8236_SW_REG_WRITE,			/* write AR8236 internal register*/
	MMEAD_READ_MODULE_OPERATION,		/* ���豸��ȡ�Զ��������*/
	MMEAD_WRITE_MODULE_OPERATION,		/* ���Զ��������д���豸*/
	MMEAD_READ_MODULE_OPERATION_CRC,	/* ��ȡ�Զ���������CRC */
	MMEAD_READ_PIB_CRC,					/* ��ȡ6400 device pib crc */
	MMEAD_READ_PIB,						/* ��ȡ6400 device pib */
	MMEAD_WRITE_PIB,						/* д��6400 device pib */
	MMEAD_BLOCK_USER,						/* ��ֹ�û�����*/
	MMEAD_BOOT_OUT_CNU,					/* �߳��Ƿ��û�*/
	MMEAD_AUTHORIZATION_USER,			/* ��ȨCNU�û��˿�*/
	MMEAD_LINK_DIAG,
	MMEAD_GET_TOPOLOGY_STATS,
	MMEAD_GET_FREQUENCY_BAND_SELECTION,
	MMEAD_SET_FREQUENCY_BAND_SELECTION,
	MMEAD_GET_TX_GAIN,
	MMEAD_SET_TX_GAIN,
	MMEAD_MDIO_READ,			/* read register by mme*/
	MMEAD_MDIO_WRITE,			/* write register by mme*/
	MMEAD_GET_RTL8306E_CONFIG,	/* get rtl8306e configuration from cnu by mme mdio */
	MMEAD_WRITE_MOD,				/* write mod */
	MMEAD_ERASE_MOD,
	MMEAD_GET_USER_HFID,
	MMEAD_SET_USER_HFID,
	MMEAD_GET_RTL8306E_PORT_STATUS,
	MMEAD_GET_HG_SV,
	MMEAD_GET_HG_MANAGE,
	MMEAD_SET_HG_MANAGE,
	MMEAD_GET_HG_BUSINESS,
	MMEAD_SET_HG_BUSINESS,
	MMEAD_REBOOT_HG,
	MMEAD_RESET_HG,
	MMEAD_GET_HG_SSID_STATUS,
	MMEAD_SET_HG_SSID_STATUS,
	MMEAD_GET_HG_WAN_STATUS,
	/* �����м���������ö�ٶ��� */

	MMEAD_MAX_REQ = 0xff
};

/* ���������־�Ĳ����� */
enum
{
	OPT_SNMP = 0,
	OPT_CLI,
	OPT_WEB
};

enum
{
	MSG_ATTRIB_REQ = 0,		/* ucMsgAttrib����������*/
	MSG_ATTRIB_ACK			/* ucMsgAttribӦ��������*/
};

/* ��Ϣ���������ֶ���*/
enum
{
	TM_CORE_MSG_DEBUG_DISABLE = 0,	/* �ر�ģ����Կ���*/
	TM_CORE_MSG_DEBUG_ENABLE,		/* ��ģ����Կ���*/
	TM_CORE_GET_CRC,					/* ��ȡ����һ����У����*/
	TM_CORE_GEN_CONF,					/* �Զ���������*/
	TM_CORE_DESTROY_CONF,				/* �����Զ����ɵ�����*/
	TM_CORE_GET_ANONYMOUS_AUTH_STS,	/* ��ȡ�����û�����ʹ��״̬*/
	TM_CORE_SET_ANONYMOUS_AUTH_EN,		/* ���������û�����ʹ��*/
	TM_CORE_SET_ANONYMOUS_AUTH_DIS,	/* ���������û������ֹ*/
	TM_CORE_GET_GLOBAL_AUTO_CONFIG_STS,	/* ��ȡȫ���Զ�����ʹ��״̬*/
	TM_CORE_SET_GLOBAL_AUTO_CONFIG_EN,		/* ȫ���Զ�����ʹ��*/
	TM_CORE_SET_GLOBAL_AUTO_CONFIG_DIS,	/* ȫ���Զ����ý�ֹ*/
	TM_CORE_GET_USER_AUTO_CONFIG_STS,		/* ��ȡ�û��Զ�����ʹ��״̬*/
	TM_CORE_SET_USER_AUTO_CONFIG_EN,		/* �û��Զ�����ʹ��*/
	TM_CORE_SET_USER_AUTO_CONFIG_DIS,		/* �û��Զ����ý�ֹ*/
	TM_CORE_GET_TM_BY_ID,		/* ��ȡָ��ģ�����Ϣ*/
	TM_CORE_WRITE_TM_BY_ID,		/* �޸�ָ��ģ�����Ϣ*/
	TM_CORE_DEL_TM_BY_ID,		/* ɾ��ָ����ģ��*/
	TM_CORE_DEL_USER,				/* �����˱�ɾ��ָ�����豸*/
	TM_CORE_NEW_TM,				/* �½�һ��ģ��*/
	TM_CORE_GET_USER_CONF,		/* ��ȡ�û�������Ϣ*/
	TM_CORE_WRITE_USER_CONF,		/* �޸��û�������Ϣ*/
	TM_CORE_USER_ADD,				/* ���һ���������û�*/
	TM_CORE_USER_EDIT,			/* �޸��û�*/
	TM_CORE_USER_DEL,				/* �Ӱ�����ɾ��һ���û�*/
	TM_CORE_GET_CNU_TID,			/* ��ȡCNU��PROFILE ID */
	TM_CORE_GET_WLIST_STS,		/* ��ȡ���������ܵ�״̬*/
	TM_CORE_SET_WLIST_ON,
	TM_CORE_SET_WLIST_OFF,
	TM_CORE_DUMP_CNU_REG,
	TM_CORE_DUMP_CNU_MOD,
	TM_CORE_DUMP_CNU_PIB,
	TM_CORE_DUMP_CLT_PIB
};

enum
{
	TM_AUTO_GEN_PIB = 0,
	TM_AUTO_GEN_MOD,

	TM_AUTO_GEN_ALL = 0xFF
};

typedef enum 
{
	RC_UPG_OK = 0,
	RC_UPG_ON_GOING,
	RC_UPG_CANNOT_OPEN_DBS,
	RC_UPG_READ_DBS_ERROR,
	RC_UPG_INIT_NETWORK_ERROR,
	RC_UPG_FILE_DOESNT_EXIST,
	RC_UPG_MD5FILE_DOESNT_EXIST,
	RC_UPG_MD_CHECK_ERROR,
	RC_UPG_FLASH_EXHAUSTED,
	RC_UPG_NOT_ENOUGH_MEMORY,
	RC_UPG_SERVER_UNREACHABLE,
	RC_UPG_INVALID_PARAMETER,
	RC_UPG_FLASH_WRITE_ERROR,
	RC_UPG_FLASH_ERASE_ERROR, 
	RC_UPG_MTD_OPEN_ERROR,
	RC_UPG_IMAGE_TEST_ERROR,
	RC_UPG_GENERIC_ERROR,					/* 16: before this case is defined by wec3501i */
	/* adding new error code by wec9720ek, starting from 50 */
	RC_UPG_SYS_SOCKET_IO_ERROR = 50,		/* 50: system socket io error */
	RC_UPG_OPT_TIMEOUT,					/* 51: operation waiting time out */
	RC_UPG_SYS_GENERIC_IO_ERROR,			/* 52: system generic io error */
	RC_UPG_FILE_DOWNLOAD_ERROR,			/* 53: download image file error */
	RC_UPG_FILE_TAG_ERROR,				/* 54: image digital signature tag error */
	RC_UPG_FILE_INVALID,					/* 55: image file invalid */

	/* please add error before this case */
	RC_UPG_UNKNOWN_ERROR = 0xFF
}UPLOAD_ERROR_CODE;

enum
{
	SYSLED_STS_NORMAL = 0,
	SYSLED_STS_BUSY = 2,
	SYSLED_STS_RESET = 3,

	SYSLED_STS_OTHER = 0xFF
};


/*Prevail �ڵ�*/
#define OID_PREVAIL                                             1,3,6,1,4,1,36186
#define PREVAIL_INFO_NAME                                1
#define PREVAIL_INFO_CONTACT                          2
#define PREVAIL_INFO_EMAIL                               3
#define PREVAIL_INFO_DEVICETYPE                     4

/*==============================================================*/
/* �����豸�Ƿ�����*/
enum
{
	DEV_STS_OFFLINE = 0,
	DEV_STS_ONLINE = 1
};

/* �����澯״̬*/
enum
{
	ALARM_STS_NOMINAL = 1,
	ALARM_STS_HIHI,
	ALARM_STS_HI,
	ALARM_STS_LO,
	ALARM_STS_LOLO,
	ALARM_STS_MAJOR,
	ALARM_STS_MINOR,

	ALARM_STS_OTHER
};

typedef enum 
{
    SPEED_AUTO_NEGOTIATION,
    SPEED_10M,
    SPEED_100M,
    SPEED_1000M,
    SPEED_UNKNOWN,
} PORT_SPEED_OPTION;

typedef enum 
{
    DUPLEX_AUTO_NEGOTIATION,
    DUPLEX_HALF,
    DUPLEX_FULL,
} PORT_DULEX_OPTION;

typedef enum 
{
    PORT_LINK_DOWN,
    LINKUP_1000M_HALF_DUPLEX,
    LINKUP_1000M_FULL_DUPLEX,
    LINKUP_100M_HALF_DUPLEX,
    LINKUP_100M_FULL_DUPLEX,
    LINKUP_10M_HALF_DUPLEX,
    LINKUP_10M_FULL_DUPLEX,
} PORT_LINK_STATUS_OPTION;

/*==============================================================*/
/*                              DB_ACCESSģ���ⲿ��Ϣ�ӿڶ���[��ʼ]                       */
/*==============================================================*/
#define DBS_INTEGER  1
#define DBS_TEXT     3
#define DBS_NULL     5

#define DBS_COL_MAX_LEN	256

/* ����system.db�����ݱ������*/
#define DBS_SYS_TBL_TOTAL_NUM		11
/* ����log.db�����ݱ������*/
#define DBS_LOG_TBL_TOTAL_NUM		4

/* �������ݱ��������*/
#define DBS_SYS_TBL_COLS_CLIROLE		3
#define DBS_SYS_TBL_COLS_CLT			9
#define DBS_SYS_TBL_COLS_CLTPRO		31
#define DBS_SYS_TBL_COLS_CNU			15
#define DBS_SYS_TBL_COLS_CNUDEPRO	62
#define DBS_SYS_TBL_COLS_NETWORK		9
#define DBS_SYS_TBL_COLS_CNUPRO		69
#define DBS_SYS_TBL_COLS_SNMPINFO		8
#define DBS_SYS_TBL_COLS_SWMGMT		6
#define DBS_SYS_TBL_COLS_SYSINFO		17
#define DBS_SYS_TBL_COLS_TEMPLATE           15

/*      ����system.db�����ݱ��������   */
#define DBS_SYS_TBL_ID_CLIROLE		0
#define DBS_SYS_TBL_ID_CLT			1
#define DBS_SYS_TBL_ID_CLTPRO		2
#define DBS_SYS_TBL_ID_CNU			3
#define DBS_SYS_TBL_ID_CNUDEPRO	4
#define DBS_SYS_TBL_ID_NETWORK	5
#define DBS_SYS_TBL_ID_CNUPRO		6
#define DBS_SYS_TBL_ID_SNMPINFO	7
#define DBS_SYS_TBL_ID_SWMGMT	8
#define DBS_SYS_TBL_ID_SYSINFO		9
#define DBS_SYS_TBL_ID_TEMPLATE		10



/* �������ݱ���Ԫ������*/
#define DBS_SYS_TBL_CNU_COL_ID_ID		0
#define DBS_SYS_TBL_CNU_COL_ID_MODEL	1
#define DBS_SYS_TBL_CNU_COL_ID_MAC	2
#define DBS_SYS_TBL_CNU_COL_ID_STS	3
#define DBS_SYS_TBL_CNU_COL_ID_AUTH	4
#define DBS_SYS_TBL_CNU_COL_ID_VER	5
#define DBS_SYS_TBL_CNU_COL_ID_RX		6
#define DBS_SYS_TBL_CNU_COL_ID_TX		7
#define DBS_SYS_TBL_CNU_COL_ID_SNR	8
#define DBS_SYS_TBL_CNU_COL_ID_BPC	9
#define DBS_SYS_TBL_CNU_COL_ID_ATT	10
#define DBS_SYS_TBL_CNU_COL_ID_SYNCH	11
#define DBS_SYS_TBL_CNU_COL_ID_ROWSTS	12
#define DBS_SYS_TBL_CNU_COL_ID_AUTOSTS       13
#define DBS_SYS_TBL_CNU_COL_ID_USERHFID     14


/* �������ݱ���Ԫ������*/
#define DBS_SYS_TBL_CLIROLES_COL_ID_ID	0
#define DBS_SYS_TBL_CLIROLES_COL_ID_USER	1
#define DBS_SYS_TBL_CLIROLES_COL_ID_PWD	2

/* �������ݱ���Ԫ������*/
#define DBS_SYS_TBL_CLT_COL_ID_ID	0
#define DBS_SYS_TBL_CLT_COL_ID_MODEL	1
#define DBS_SYS_TBL_CLT_COL_ID_MAC	2
#define DBS_SYS_TBL_CLT_COL_ID_STS	3
#define DBS_SYS_TBL_CLT_COL_ID_MAXSTATS	4
#define DBS_SYS_TBL_CLT_COL_ID_NUMSTATS	5
#define DBS_SYS_TBL_CLT_COL_ID_SWVER	6
#define DBS_SYS_TBL_CLT_COL_ID_SYNCH	7
#define DBS_SYS_TBL_CLT_COL_ID_ROWSTS	8

/* �������ݱ���Ԫ������*/
#define DBS_SYS_TBL_CLTPRO_COL_ID_ID			0
#define DBS_SYS_TBL_CLTPRO_COL_ID_ROWSTS	1
#define DBS_SYS_TBL_CLTPRO_COL_ID_BASE		2
#define DBS_SYS_TBL_CLTPRO_COL_ID_MACLIMIT	3
#define DBS_SYS_TBL_CLTPRO_COL_ID_CURATE		4
#define DBS_SYS_TBL_CLTPRO_COL_ID_CDRATE		5
#define DBS_SYS_TBL_CLTPRO_COL_ID_LOAGE		6
#define DBS_SYS_TBL_CLTPRO_COL_ID_REAGE		7
#define DBS_SYS_TBL_CLTPRO_COL_ID_IGMPPRI	8
#define DBS_SYS_TBL_CLTPRO_COL_ID_UNIPRI		9
#define DBS_SYS_TBL_CLTPRO_COL_ID_AVSPRI		10
#define DBS_SYS_TBL_CLTPRO_COL_ID_MCPRI		11
#define DBS_SYS_TBL_CLTPRO_COL_ID_TBAPRISTS	12
#define DBS_SYS_TBL_CLTPRO_COL_ID_COSPRISTS	13
#define DBS_SYS_TBL_CLTPRO_COL_ID_COS0PRI	14
#define DBS_SYS_TBL_CLTPRO_COL_ID_COS1PRI	15
#define DBS_SYS_TBL_CLTPRO_COL_ID_COS2PRI	16
#define DBS_SYS_TBL_CLTPRO_COL_ID_COS3PRI	17
#define DBS_SYS_TBL_CLTPRO_COL_ID_COS4PRI	18
#define DBS_SYS_TBL_CLTPRO_COL_ID_COS5PRI	19
#define DBS_SYS_TBL_CLTPRO_COL_ID_COS6PRI	20
#define DBS_SYS_TBL_CLTPRO_COL_ID_COS7PRI	21
#define DBS_SYS_TBL_CLTPRO_COL_ID_TOSPRISTS	22
#define DBS_SYS_TBL_CLTPRO_COL_ID_TOS0PRI	23
#define DBS_SYS_TBL_CLTPRO_COL_ID_TOS1PRI	24
#define DBS_SYS_TBL_CLTPRO_COL_ID_TOS2PRI	25
#define DBS_SYS_TBL_CLTPRO_COL_ID_TOS3PRI	26
#define DBS_SYS_TBL_CLTPRO_COL_ID_TOS4PRI	27
#define DBS_SYS_TBL_CLTPRO_COL_ID_TOS5PRI	28
#define DBS_SYS_TBL_CLTPRO_COL_ID_TOS6PRI	29
#define DBS_SYS_TBL_CLTPRO_COL_ID_TOS7PRI	30

/* �������ݱ���Ԫ������*/
#define DBS_SYS_TBL_DEPRO_COL_ID_ID			0
#define DBS_SYS_TBL_DEPRO_COL_ID_BASE		1
#define DBS_SYS_TBL_DEPRO_COL_ID_MACLIMIT	2
#define DBS_SYS_TBL_DEPRO_COL_ID_CURATE		3
#define DBS_SYS_TBL_DEPRO_COL_ID_CDRATE		4
#define DBS_SYS_TBL_DEPRO_COL_ID_LOAGE		5
#define DBS_SYS_TBL_DEPRO_COL_ID_REAGE		6
#define DBS_SYS_TBL_DEPRO_COL_ID_IGMPPRI		7
#define DBS_SYS_TBL_DEPRO_COL_ID_UNIPRI		8
#define DBS_SYS_TBL_DEPRO_COL_ID_AVSPRI		9
#define DBS_SYS_TBL_DEPRO_COL_ID_MCPRI		10
#define DBS_SYS_TBL_DEPRO_COL_ID_TBAPRISTS	11
#define DBS_SYS_TBL_DEPRO_COL_ID_COSPRISTS	12
#define DBS_SYS_TBL_DEPRO_COL_ID_COS0PRI		13
#define DBS_SYS_TBL_DEPRO_COL_ID_COS1PRI		14
#define DBS_SYS_TBL_DEPRO_COL_ID_COS2PRI		15
#define DBS_SYS_TBL_DEPRO_COL_ID_COS3PRI		16
#define DBS_SYS_TBL_DEPRO_COL_ID_COS4PRI		17
#define DBS_SYS_TBL_DEPRO_COL_ID_COS5PRI		18
#define DBS_SYS_TBL_DEPRO_COL_ID_COS6PRI		19
#define DBS_SYS_TBL_DEPRO_COL_ID_COS7PRI		20
#define DBS_SYS_TBL_DEPRO_COL_ID_TOSPRISTS	21
#define DBS_SYS_TBL_DEPRO_COL_ID_TOS0PRI		22
#define DBS_SYS_TBL_DEPRO_COL_ID_TOS1PRI		23
#define DBS_SYS_TBL_DEPRO_COL_ID_TOS2PRI		24
#define DBS_SYS_TBL_DEPRO_COL_ID_TOS3PRI		25
#define DBS_SYS_TBL_DEPRO_COL_ID_TOS4PRI		26
#define DBS_SYS_TBL_DEPRO_COL_ID_TOS5PRI		27
#define DBS_SYS_TBL_DEPRO_COL_ID_TOS6PRI		28
#define DBS_SYS_TBL_DEPRO_COL_ID_TOS7PRI		29
#define DBS_SYS_TBL_DEPRO_COL_ID_SFBSTS		30
#define DBS_SYS_TBL_DEPRO_COL_ID_SFUSTS		31
#define DBS_SYS_TBL_DEPRO_COL_ID_SFMSTS		32
#define DBS_SYS_TBL_DEPRO_COL_ID_SFRATE		33
#define DBS_SYS_TBL_DEPRO_COL_ID_VLANSTS		34
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH1VID		35
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH2VID		36
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH3VID		37
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH4VID		38
#define DBS_SYS_TBL_DEPRO_COL_ID_PORTPRISTS	39
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH1PRI		40
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH2PRI		41
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH3PRI		42
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH4PRI		43
#define DBS_SYS_TBL_DEPRO_COL_ID_RXLIMITSTS	44
#define DBS_SYS_TBL_DEPRO_COL_ID_CPURX		45
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH1RX		46
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH2RX		47
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH3RX		48
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH4RX		49
#define DBS_SYS_TBL_DEPRO_COL_ID_TXLIMITSTS	50
#define DBS_SYS_TBL_DEPRO_COL_ID_CPUTX		51
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH1TX		52
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH2TX		53
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH3TX		54
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH4TX		55
#define DBS_SYS_TBL_DEPRO_COL_ID_PSCTL		56
#define DBS_SYS_TBL_DEPRO_COL_ID_CPUPSTS		57
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH1STS		58
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH2STS		59
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH3STS		60
#define DBS_SYS_TBL_DEPRO_COL_ID_ETH4STS		61

/* �������ݱ���Ԫ������*/
#define DBS_SYS_TBL_NETWORK_COL_ID_ID		0
#define DBS_SYS_TBL_NETWORK_COL_ID_IP		1
#define DBS_SYS_TBL_NETWORK_COL_ID_MASK		2
#define DBS_SYS_TBL_NETWORK_COL_ID_GW		3
#define DBS_SYS_TBL_NETWORK_COL_ID_DHCP		4
#define DBS_SYS_TBL_NETWORK_COL_ID_DNS		5
#define DBS_SYS_TBL_NETWORK_COL_ID_MAC		6
#define DBS_SYS_TBL_NETWORK_COL_ID_MVSTS	7
#define DBS_SYS_TBL_NETWORK_COL_ID_MVID		8

/* �������ݱ���Ԫ������*/
#define DBS_SYS_TBL_PROFILE_COL_ID_ID				0
#define DBS_SYS_TBL_PROFILE_COL_ID_ROWSTS		1
#define DBS_SYS_TBL_PROFILE_COL_ID_BASE			2
#define DBS_SYS_TBL_PROFILE_COL_ID_MACLIMIT		3
#define DBS_SYS_TBL_PROFILE_COL_ID_CURATE		4
#define DBS_SYS_TBL_PROFILE_COL_ID_CDRATE		5
#define DBS_SYS_TBL_PROFILE_COL_ID_LOAGE			6
#define DBS_SYS_TBL_PROFILE_COL_ID_REAGE			7
#define DBS_SYS_TBL_PROFILE_COL_ID_IGMPPRI		8
#define DBS_SYS_TBL_PROFILE_COL_ID_UNIPRI			9
#define DBS_SYS_TBL_PROFILE_COL_ID_AVSPRI		10
#define DBS_SYS_TBL_PROFILE_COL_ID_MCPRI			11
#define DBS_SYS_TBL_PROFILE_COL_ID_TBAPRISTS		12
#define DBS_SYS_TBL_PROFILE_COL_ID_COSPRISTS		13
#define DBS_SYS_TBL_PROFILE_COL_ID_COS0PRI		14
#define DBS_SYS_TBL_PROFILE_COL_ID_COS1PRI		15
#define DBS_SYS_TBL_PROFILE_COL_ID_COS2PRI		16
#define DBS_SYS_TBL_PROFILE_COL_ID_COS3PRI		17
#define DBS_SYS_TBL_PROFILE_COL_ID_COS4PRI		18
#define DBS_SYS_TBL_PROFILE_COL_ID_COS5PRI		19
#define DBS_SYS_TBL_PROFILE_COL_ID_COS6PRI		20
#define DBS_SYS_TBL_PROFILE_COL_ID_COS7PRI		21
#define DBS_SYS_TBL_PROFILE_COL_ID_TOSPRISTS		22
#define DBS_SYS_TBL_PROFILE_COL_ID_TOS0PRI		23
#define DBS_SYS_TBL_PROFILE_COL_ID_TOS1PRI		24
#define DBS_SYS_TBL_PROFILE_COL_ID_TOS2PRI		25
#define DBS_SYS_TBL_PROFILE_COL_ID_TOS3PRI		26
#define DBS_SYS_TBL_PROFILE_COL_ID_TOS4PRI		27
#define DBS_SYS_TBL_PROFILE_COL_ID_TOS5PRI		28
#define DBS_SYS_TBL_PROFILE_COL_ID_TOS6PRI		29
#define DBS_SYS_TBL_PROFILE_COL_ID_TOS7PRI		30
#define DBS_SYS_TBL_PROFILE_COL_ID_SFBSTS		31
#define DBS_SYS_TBL_PROFILE_COL_ID_SFUSTS		32
#define DBS_SYS_TBL_PROFILE_COL_ID_SFMSTS		33
#define DBS_SYS_TBL_PROFILE_COL_ID_SFRATE		34
#define DBS_SYS_TBL_PROFILE_COL_ID_VLANSTS		35
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH1VID		36
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH2VID		37
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH3VID		38
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH4VID		39
#define DBS_SYS_TBL_PROFILE_COL_ID_PORTPRISTS	40
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH1PRI		41
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH2PRI		42
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH3PRI		43
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH4PRI		44
#define DBS_SYS_TBL_PROFILE_COL_ID_RXLIMITSTS	45
#define DBS_SYS_TBL_PROFILE_COL_ID_CPURX		46
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH1RX		47
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH2RX		48
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH3RX		49
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH4RX		50
#define DBS_SYS_TBL_PROFILE_COL_ID_TXLIMITSTS	51
#define DBS_SYS_TBL_PROFILE_COL_ID_CPUTX			52
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH1TX		53
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH2TX		54
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH3TX		55
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH4TX		56
#define DBS_SYS_TBL_PROFILE_COL_ID_PSCTL			57
#define DBS_SYS_TBL_PROFILE_COL_ID_CPUPSTS		58
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH1STS		59
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH2STS		60
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH3STS		61
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH4STS		62
#define DBS_SYS_TBL_PROFILE_COL_ID_UPLINKVID	63
#define DBS_SYS_TBL_PROFILE_COL_ID_UPLINKVMODE	64
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH1VMODE	65
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH2VMODE	66
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH3VMODE	67
#define DBS_SYS_TBL_PROFILE_COL_ID_ETH4VMODE	68

/* �������ݱ���Ԫ������*/
#define DBS_SYS_TBL_SNMP_COL_ID_ID		0
#define DBS_SYS_TBL_SNMP_COL_ID_RC		1
#define DBS_SYS_TBL_SNMP_COL_ID_WC		2
#define DBS_SYS_TBL_SNMP_COL_ID_TC		3
#define DBS_SYS_TBL_SNMP_COL_ID_SA		4
#define DBS_SYS_TBL_SNMP_COL_ID_SB		5
#define DBS_SYS_TBL_SNMP_COL_ID_PA		6
#define DBS_SYS_TBL_SNMP_COL_ID_PB		7

/* �������ݱ���Ԫ������*/
#define DBS_SYS_TBL_SWMGMT_COL_ID_ID		0
#define DBS_SYS_TBL_SWMGMT_COL_ID_IP		1
#define DBS_SYS_TBL_SWMGMT_COL_ID_PORT	2
#define DBS_SYS_TBL_SWMGMT_COL_ID_USR	3
#define DBS_SYS_TBL_SWMGMT_COL_ID_PWD	4
#define DBS_SYS_TBL_SWMGMT_COL_ID_PATH	5

/* �������ݱ���Ԫ������*/
#define DBS_SYS_TBL_SYSINFO_COL_ID_ID			0
#define DBS_SYS_TBL_SYSINFO_COL_ID_MODEL		1
#define DBS_SYS_TBL_SYSINFO_COL_ID_MAXCLT	2
#define DBS_SYS_TBL_SYSINFO_COL_ID_HWVER	3
#define DBS_SYS_TBL_SYSINFO_COL_ID_BVER		4
#define DBS_SYS_TBL_SYSINFO_COL_ID_KVER		5
#define DBS_SYS_TBL_SYSINFO_COL_ID_APPVER	6
#define DBS_SYS_TBL_SYSINFO_COL_ID_FS			7
#define DBS_SYS_TBL_SYSINFO_COL_ID_RS			8
#define DBS_SYS_TBL_SYSINFO_COL_ID_WLCTL		9
#define DBS_SYS_TBL_SYSINFO_COL_ID_AU		10
#define DBS_SYS_TBL_SYSINFO_COL_ID_AC		11
#define DBS_SYS_TBL_SYSINFO_COL_ID_WDT		12
#define DBS_SYS_TBL_SYSINFO_COL_ID_MF		13
#define DBS_SYS_TBL_SYSINFO_COL_ID_P6RXD		14
#define DBS_SYS_TBL_SYSINFO_COL_ID_P6TXD		15
#define DBS_SYS_TBL_SYSINFO_COL_ID_APPHASH   16

/* �������ݱ���Ԫ������*/
#define DBS_SYS_TBL_TEMPLATE_COL_ID_ID			0
#define DBS_SYS_TBL_TEMPLATE_COL_ID_TEMPAUTOSTS	1
#define DBS_SYS_TBL_TEMPLATE_COL_ID_CURTEMP	2
#define DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANADDSTS		3
#define DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANSTART		4
#define DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANSTOP		5
#define DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANADDSTS		6
#define DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANSTART		7
#define DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANSTOP		8
#define DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANADDSTS		9
#define DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANSTART		10
#define DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANSTOP	        11
#define DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANADDSTS	        12
#define DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANSTART		13
#define DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANSTOP		14


/*      ����log.db�����ݱ��������   */
#define DBS_LOG_TBL_ID_ALARM		0
#define DBS_LOG_TBL_ID_OPT			1
#define DBS_LOG_TBL_ID_SYS			2
#define DBS_LOG_TBL_ID_MS			3

/* ����log.db���ݱ��������*/
#define DBS_LOG_TBL_COLS_ALARM		11
#define DBS_LOG_TBL_COLS_OPT			5
#define DBS_LOG_TBL_COLS_SYSTEM		4
#define DBS_LOG_TBL_COLS_MS			2

/* �������ݱ���Ԫ������*/
#define DBS_LOG_TBL_ALARM_COL_ID_SFLOW	0
#define DBS_LOG_TBL_ALARM_COL_ID_TINFO	1
#define DBS_LOG_TBL_ALARM_COL_ID_TOID	2
#define DBS_LOG_TBL_ALARM_COL_ID_ACODE	3
#define DBS_LOG_TBL_ALARM_COL_ID_ATIME	4
#define DBS_LOG_TBL_ALARM_COL_ID_INUM	5
#define DBS_LOG_TBL_ALARM_COL_ID_CMAC	6
#define DBS_LOG_TBL_ALARM_COL_ID_CLT		7
#define DBS_LOG_TBL_ALARM_COL_ID_CNU		8
#define DBS_LOG_TBL_ALARM_COL_ID_ATYPE	9
#define DBS_LOG_TBL_ALARM_COL_ID_AVAL	10

/* �������ݱ���Ԫ������*/
#define DBS_LOG_TBL_OPT_COL_ID_TIME	0
#define DBS_LOG_TBL_OPT_COL_ID_WHO	1
#define DBS_LOG_TBL_OPT_COL_ID_CMD	2
#define DBS_LOG_TBL_OPT_COL_ID_LEVEL	3
#define DBS_LOG_TBL_OPT_COL_ID_RET	4

/* �������ݱ���Ԫ������*/
#define DBS_LOG_TBL_SYS_COL_ID_TIME	0
#define DBS_LOG_TBL_SYS_COL_ID_MOD	1
#define DBS_LOG_TBL_SYS_COL_ID_LEVEL	2
#define DBS_LOG_TBL_SYS_COL_ID_INFO	3

/* �������ݱ���Ԫ������*/
#define DBS_LOG_TBL_MS_COL_ID_ID	0
#define DBS_LOG_TBL_MS_COL_ID_MS	1

/*********************	���ݿ�db_cbat.db�еı�tbl_clt�ĳ�ʼֵ:	*****
-----------------------------------------------------------------------------
| id	| DevType | mac | online | NumStas | MaxStas	| TempId	| AutoUpEnable | SwVersion	|
-----------------------------------------------------------------------------
| 1	| 0 | 00:00:00:00:00:00 | 0	| 0 | 0 | 0 | 0 | unknown |
-----------------------------------------------------------------------------
****************************************************************************/

/****************	���ݿ�db_cbat.db�еı�tbl_clt1cnu�ĳ�ʼֵ:	****
-------------------------------------------------------------------------
| id | DevType | mac | online | RxRate | TxRate | Attenuation | DwQuality | UpQuality 
| UserType | TempId | AccessEnable | AutoUpEnable | SwVersion | OnUsed |
-------------------------------------------------------------------------
|1   | 0 | 00:00:00:00:00:00 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | unknown | 0 |
-------------------------------------------------------------------------
|2   | 0 | 00:00:00:00:00:00 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | unknown | 0 |
-------------------------------------------------------------------------
|64  | 0 | 00:00:00:00:00:00 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | unknown | 0 |
-------------------------------------------------------------------------
****************************************************************************/

enum
{
	BOOL_FALSE = 0,
	BOOL_TRUE 
};

/* DB_ACCESSģ����Ϣ�����ֶ���*/
enum
{
	DB_MSG_DEBUG_ENABLE = 1,		/* �����ݿ�ģ�������Ϣ*/
	DB_MSG_DEBUG_DISABLE,		/* �ر����ݿ�ģ�������Ϣ*/
	DB_SQL_DEBUG_ENABLE,			/* ��SQL��������Ϣ*/
	DB_SQL_DEBUG_DISABLE,			/* �ر�SQL��������Ϣ*/
	DB_GET_NELIB_CLT,				/* ��ȡCLT ��Ԫ��*/
	DB_GET_NELIB_CNU,				/* ��ȡCNU ��Ԫ��*/
	DB_GET_INT_VAL,				/* ��ȡһ��������ֵ*/
	DB_UPDATE_INT_VAL,			/* ����һ��������ֵ*/
	DB_GET_ROW,					/* ��ȡ���һ������*/
	DB_INSERT_ROW,				/* �����ݱ����һ������*/
	DB_UPDATE_ROW,				/* ���±��һ�����ݵ�ĳЩ��*/
	DB_GET_TBL_COUNT,				/* ��ȡ���ݱ��������*/
	DB_GET_ROW_LOG,				/* ��ȡ��־����߸澯���һ��*/
	DB_INSERT_ROW_LOG,			/* ����־����߸澯�����һ��*/
	DB_ONLINE_BACKUP,				/* �����ݿ����ڴ汸����flash disk */
	DB_GET_TEXT_VAL,
	DB_UPDATE_TEXT_VAL,
	
	DB_WRITE_SYSLOG,
	DB_WRITE_OPTLOG,
	DB_WRITE_ALARMLOG,
	DB_GET_LOG_COUNT,
	DB_GET_SYSLOG,
	DB_GET_OPTLOG,
	DB_GET_ALARMLOG,
	DB_GET_COL_VALUE,
	DB_UPDATE_COL_VALUE,
	DB_GET_MODULE_STS,
	DB_REGISTER_MODULE,
	DB_DESTROY_MODULE,
	DB_GET_ROW_CLI_ROLE,
	DB_UPDATE_ROW_CLI_ROLE,
	DB_GET_ROW_CLT,
	DB_UPDATE_ROW_CLT,
	DB_GET_ROW_CLTCONF,
	DB_UPDATE_ROW_CLTCONF,
	DB_GET_ROW_CNU,
	DB_UPDATE_ROW_CNU,
	DB_GET_ROW_DEPRO,
	DB_UPDATE_ROW_DEPRO,
	DB_GET_ROW_NETWORK,
	DB_UPDATE_ROW_NETWORK,
	DB_GET_ROW_PROFILE,
	DB_UPDATE_ROW_PROFILE,
	DB_GET_ROW_SNMP,
	DB_UPDATE_ROW_SNMP,
	DB_GET_ROW_SWMGMT,
	DB_UPDATE_ROW_SWMGMT,
	DB_GET_ROW_SYSINFO,
	DB_UPDATE_ROW_SYSINFO,
	DB_DESTROY_ROW_TBL_CLT,
	DB_DESTROY_ROW_TBL_CLTCONF,
	DB_DESTROY_ROW_TBL_CNU,
	DB_DESTROY_ROW_TBL_PROFILE,
	DB_CREATE_SU_PROFILE,			/* ���������û���·����*/
	DB_CREATE_WL_DE_PROFILE,		/* �����������û�Ĭ����·����*/
	DB_CREATE_BL_DE_PROFILE,		/* �����������û�Ĭ����·����*/
	DB_SELECT_CNU_BY_MAC,
	DB_CREATE_SU2_PROFILE,
	DB_CREATE_WL_DE2_PROFILE,
	DB_CREATE_BL_DE2_PROFILE,
	DB_SELECT_CNU_INDEX_BY_MAC,
	DB_GET_ROW_TEMPLATE,
	DB_UPDATE_ROW_TEMPLATE,
	
	/* �����м��������������*/	
	DB_CMD_OTHER = 0xFF
};

#if 0
/* �������ݿ�������ֵ����*/
enum
{
	DB_TBL_CLT = 0,
	DB_TBL_CLT_A_CNU,
	DB_TBL_OPT_LOG,
	DB_TBL_CLI_ROLE,
	DB_TBL_NETWORK_INFO,
	DB_TBL_SYS_INFO,
	DB_TBL_ALARM,
	DB_TBL_FTP_SERVER,
	DB_TBL_TEMPLATE,
	DB_TBL_SNMP
};
#endif

/* ����SOCKET���õ�ö�ٱ���*/
enum
{
	DESC_SOCKET_MMEAD = 0,	/* ��MMEADģ��ͨѶ��SOCKE������*/
	DESC_SOCKET_AUTO_UPG,	/* ���Զ�����ģ��ͨѶ��SOCKE������*/
	DESC_SOCKET_AUTO_CONF,	/* ���Զ�����ģ��ͨѶ��SOCKE������*/
	DESC_SOCKET_ALARM,		/* ��澯����ģ��ͨѶ��SOCKE������*/
	DESC_SOCKET_DBS,			/* �����ݿ�ģ��ͨѶ��SOCKE������*/
	DESC_SOCKET_S_CMM,		/* CMMģ���ػ������׽���*/
	DESC_SOCKET_S_TM,			/* ��TEMPLATE ģ��ͨѶ��SOCKE������*/
	DESC_SOCKET_S_REG,			/* ��REGISTER ģ��ͨѶ��SOCKE������*/
	/* �����м��������������*/	
	DESC_SOCKET_OTHER
};

#define EOC_ENTERPRISE_OID 17409
#define EOC_PREVAIL_OID 1,3,6,1,4,1,36186
#define PREVAIL_EoC_CNU_TABLE_OID              1,3,6,1,4,1,36186,8,1
#define EOC_OID_PREFIX 1,3,6,1,4,1,EOC_ENTERPRISE_OID
#define EOC_OID_NSCRTV_EPONEOC_BASE 		EOC_OID_PREFIX,2

/*==============online cnu============*/
#define EOC_OID_CNU_ONLINECNU				EOC_OID_NSCRTV_EPONEOC_BASE,4,5,2
#define OID_ONLINECNU_ALLRESET				1
#define OID_ONLINECNU_AMOUNT					2
#define OID_ONLINECNU_LIMIT					3
/*==============cnu port============*/
#define EOC_OID_CNU_PORT					EOC_OID_NSCRTV_EPONEOC_BASE,4,5,4
#define OID_CNUPORT_PORTLIMIT					1
#define OID_CNUPORT_AMOUNTLIMIT				2

#define OID_CNUPORTTABLE_MASTERINDEX			1
#define OID_CNUPORTTABLE_CNUINDEX			2
#define OID_CNUPORTTABLE_PORTINDEX			3
#define OID_CNUPORTTABLE_PORTEN				4
#define OID_CNUPORTTABLE_PORTSERVICE			5

/*CNU ��ڵ� */
#define OID_ONLINECNU_TABLE_MASTERINDEX 		1
#define OID_ONLINECNU_TABLE_CNUINDEX 		2
#define OID_ONLINECNU_TABLE_MACADDRESS 		3
#define modEoCCNUOnlineStatus 			              4
#define OID_ONLINECNU_TABLE_MODELNO  		       5
#define OID_ONLINECNU_TABLE_PORTAMOUNT 		6
#define OID_ONLINECNU_TABLE_AUTHORSTATE 		7
#define OID_ONLINECNU_TABLE_SOFTWAREVER 	8
#define OID_ONLINECNU_TABLE_CNURESET  		9
#define OID_ONLINECNU_TABLE_RF 			       10
#define OID_ONLINECNU_TABLE_RFDOWNQ 			11
#define OID_ONLINECNU_TABLE_RFUPQ  		       12
#define OID_ONLINECNU_TABLE_PHYDOWNRATE 	13
#define OID_ONLINECNU_TABLE_PHYUPRATE  		14
#define OID_ONLINECNU_TABLE_CONFSAVE 		15
#define OID_ONLINECNU_TABLE_CONFRESULT           16

/*==============cnu prevail============*/
#define OID_PREVAIL_NAME						1
#define OID_PREVAIL_CONTACT					2
#define OID_PREVAIL_EAMIL						3
#define OID_PREVAIL_DEVICETYPE					4
/*table*/
#define OID_CNU_TABLE_MASTERINDEX			1
#define OID_CNU_TABLE_CNUINDEX 				2
#define OID_CNU_TABLE_MACADDRESS 			3
#define OID_CNU_TABLE_STATUS			              4
#define OID_CNU_TABLE_DEVTYPE  		       	5
#define OID_CNU_TABLE_SOFTWAREVER 			6

#define OID_CNU_TABLE_CNURESET  				7
#define OID_CNU_TABLE_RF 			      			8
#define OID_CNU_TABLE_RFDOWNQ 				9
#define OID_CNU_TABLE_RFUPQ  		       		10
#define OID_CNU_TABLE_USERTYPE	       		11
#define OID_CNU_TABLE_ACCESSEN		       	12
#define OID_CNU_TABLE_TEMPID		       		13
#define OID_CNU_TABLE_AUTOUPEN	       		14
#define OID_CNU_TABLE_PHYDOWNRATE 			15
#define OID_CNU_TABLE_PHYUPRATE  				16
#define OID_CNU_TABLE_ONUSED	       			17

/*template table*/
#define OID_TEMPLATE_INDEX						1
#define OID_TEMPLATE_IDLE						2
#define OID_TEMPLATE_EN						3
#define OID_TEMPLATE_DEVTYPE					4
#define OID_TEMPLATE_BASE						5
#define OID_TEMPLATE_NAME						6
#define OID_TEMPLATE_ISMACLIMITVALID			7
#define OID_TEMPLATE_MACLIMITEN				8
#define OID_TEMPLATE_MACLIMITNUM				9
#define OID_TEMPLATE_ISSTORMFILTERVALID		10
#define OID_TEMPLATE_BROADSTORMEN			11
#define OID_TEMPLATE_UNISTORMEN				12
#define OID_TEMPLATE_MULTISTORMEN			13
#define OID_TEMPLATE_STORMRATELEVEL			14
#define OID_TEMPLATE_ISVLANVALID				15
#define OID_TEMPLATE_VLANEN					16
#define OID_TEMPLATE_PORT0VID					17
#define OID_TEMPLATE_PORT1VID					18
#define OID_TEMPLATE_PORT2VID					19
#define OID_TEMPLATE_PORT3VID					20
#define OID_TEMPLATE_ISPORTPRIVALID			21
#define OID_TEMPLATE_PORT0PRI					22
#define OID_TEMPLATE_PORT1PRI					23
#define OID_TEMPLATE_PORT2PRI					24
#define OID_TEMPLATE_PORT3PRI					25
#define OID_TEMPLATE_ISRATELIMITVALID			26
#define OID_TEMPLATE_PORT0RXRATE				27
#define OID_TEMPLATE_PORT1RXRATE				28
#define OID_TEMPLATE_PORT2RXRATE				29
#define OID_TEMPLATE_PORT3RXRATE				30
#define OID_TEMPLATE_PORT0TXRATE				31
#define OID_TEMPLATE_PORT1TXRATE				32
#define OID_TEMPLATE_PORT2TXRATE				33
#define OID_TEMPLATE_PORT3TXRATE				34
#define OID_TEMPLATE_ISPORTENVALID			35
#define OID_TEMPLATE_PORT0EN					36
#define OID_TEMPLATE_PORT1EN					37
#define OID_TEMPLATE_PORT2EN					38
#define OID_TEMPLATE_PORT3EN					39
#define OID_TEMPLATE_PROGET					40
#define OID_TEMPLATE_SAVE						41
#define OID_TEMPLATE_RESULT					42

/*optentry*/
#define OID_SAVE_CONFIG						1
#define OID_REBOOT_CBAT						2
#define OID_RESTORE_CBAT						3


/*cbatdev_statusgroup�ڵ�*/
#define OID_CBATSTATUSGROUP                               1,3,6,1,4,1,17409,2,4,1,3
#define OID_CBAT_SAVECONFIG					8
/*test �ڵ�*/
#define OID_PREVAILTEST                                1,3,6,1,4,1,36186,7
#define PREVAIL_INFO_TEMPID                            1
#define PREVAIL_INFO_DEVTYPE                           2
#define PREVAIL_INFO_ONUSED                            3
#define PREVAIL_INFO_REFLESH                     	4

/* snmpGroup �ڵ�*/
#define OID_SNMP_VERSION						1
#define OID_SNMP_READ_COMMUNITY				2
#define OID_SNMP_WRITE_COMMUNITY				3
#define OID_SNMP_TRAP_VERSION					4
#define OID_SNMP_TRAP_COMMUNITY				5
#define OID_SNMP_TRAP_SERVER_IP_ADDRESS		6
#define OID_SNMP_TRAP_SERVER_PORT			7

/* snmpGroup �ڵ�*/
#define OID_PREVAIL_ENTERPRISE_NAME			1
#define OID_PREVAIL_ENTERPRISE_CONTACT		2
#define OID_PREVAIL_ENTERPRISE_EMAIL			3
#define OID_PREVAIL_ENTERPRISE_DEV_NAME		4

/* systemInfo �ڵ�*/
//#define OID_PREVAIL_SYSINFO_MAC_ADDRESS		1
#define OID_PREVAIL_SYSINFO_CLT_AMOUNT		1
#define OID_PREVAIL_SYSINFO_CNU_AMOUNT		2
#define OID_PREVAIL_SYSINFO_HW_VERSION		3
#define OID_PREVAIL_SYSINFO_SW_VERSION		4
#define OID_PREVAIL_SYSINFO_FLASH_SIZE		5
#define OID_PREVAIL_SYSINFO_SDRAM_SIZE		6
#define OID_PREVAIL_SYSINFO_MANUFACTURER	7
#define OID_PREVAIL_SYSINFO_DEVICE_MODEL		8

/* networkInfo �ڵ�*/
#define OID_PREVAIL_NETWORK_INFO_IP_ADDRESS			1
#define OID_PREVAIL_NETWORK_INFO_NET_MASK			2
#define OID_PREVAIL_NETWORK_INFO_GATEWAY			3
#define OID_PREVAIL_NETWORK_INFO_MGMT_VLAN_STS		4
#define OID_PREVAIL_NETWORK_INFO_MGMT_VLAN_ID		5
#define OID_PREVAIL_NETWORK_INFO_MAC_ADDRESS		6

/* cbatAction �ڵ�*/
#define OID_PREVAIL_CBAT_ACTION_MP_RESET				1
#define OID_PREVAIL_CBAT_ACTION_SAVE_CONFIG			2
//#define OID_PREVAIL_CBAT_ACTION_ALL_RESET			2
#define OID_PREVAIL_CBAT_ACTION_RESTORE_DEFAULT		3
#define OID_PREVAIL_CBAT_ACTION_SEND_HEARTBEAT		4


/* softwareMgmt �ڵ�*/
#define OID_SNMP_SW_MGMT_CUR_VER			1
#define OID_SNMP_SW_MGMT_SERV_IP				2
#define OID_SNMP_SW_MGMT_SERV_PORT			3
#define OID_SNMP_SW_MGMT_SERV_LOGIN			4
#define OID_SNMP_SW_MGMT_SERV_PWD			5
#define OID_SNMP_SW_MGMT_SERV_FILE			6
#define OID_SNMP_SW_MGMT_UPLOAD_ACTION		7
#define OID_SNMP_SW_MGMT_UPLOAD_RESULT		8

/*==============================================================*/

typedef struct
{
	uint8_t desc;		/* ����SOCKET���õ�ö�ٱ���*/
	int sk;
	struct sockaddr_in skaddr;
}T_UDP_SK_INFO;

typedef struct
{
	T_UDP_SK_INFO sk;
	uint32_t blen;
	uint8_t b[MAX_UDP_SIZE];
}BBLOCK_QUEUE;

/* DBS */
typedef struct
{
	T_UDP_SK_INFO channel;
	uint16_t srcmod;
	uint32_t blen;
	uint8_t buf[MAX_UDP_SIZE];
}T_DBS_DEV_INFO;

/*==============================================================*/

#pragma pack (push, 1)

typedef struct
{
	uint16_t usSrcMID;			/*Դģ��ID*/
	uint16_t usDstMID;			/*Ŀ��ģ��ID*/
	uint16_t usMsgType;			/*��Ϣ���ͣ���ʾ�������Ϣ*/
	uint8_t fragment;				/*�Ƿ���ڸ����Ƭ*/
	uint32_t ulBodyLength;		/*��Ϣ�峤��*/
}
T_Msg_Header_CMM;

typedef struct
{
	T_Msg_Header_CMM HEADER;
	uint8_t BUF[0];		/*��Ϣ������*/
}
T_Msg_CMM;

typedef struct
{
	T_Msg_Header_CMM HEADER;
	uint16_t result;		/*����״̬*/
	uint8_t BUF[0];		/*��Ϣ������*/
}
T_REQ_Msg_CMM;

/* CMM����ģ�������Ϣ���صĽṹ��*/
typedef struct
{
	uint16_t usMID;
	BOOLEAN enable;
}st_ModuleDebugCtl;

/* szTopology�ṹ�嶨���*/
typedef struct
{
	uint16_t usIndex;
	uint8_t ucMode;
	uint8_t ucDevType;
	uint16_t usCCoID;
	uint8_t ucStatus;
	uint8_t ucRxRate;
	uint8_t ucTxRate;
	uint8_t szMac[32];
	uint8_t ucOnused;
}T_szTopology;

/* ����ֻ֧�ֻ�ȡ1��CLT������ */
typedef struct
{
	T_szTopology clt;
	T_szTopology cnu[MAX_CNU_AMOUNT_LIMIT];
}T_szNELib;

typedef struct
{
	uint32_t clt;	/* CLT�������ţ���1��ʼ����*/
	uint32_t cnu;	/* CNU�������ţ���1��ʼ����*/
}
stCnuNode;

/*windows mactool*/
typedef struct
{
	char mac[18];
	uint32_t model;
}
stMTmsgInfo;

typedef struct
{
	unsigned int enable;
	unsigned char mac[6];
}st_nvram_mac;

typedef struct
{
	st_nvram_mac p_nvram_mac;
}st_nvram_info;

typedef struct
{
	uint32_t entryadd;
	uint32_t phyport;
	uint32_t action;
	uint32_t protocol;
	uint32_t priority;
	uint32_t data;
}
stAclEntryInfo;

/***********************************************************************/
typedef struct
{
	uint8_t  itemp;			//!< integer part of the temperature data
	uint16_t ftemp;			//!< fractional part of the temperature data
	uint8_t  sign;			//!< sign indicator		  
	uint16_t raw_value;		//!< raw data
}st_temperature;

typedef struct
{
	uint8_t MacLimitEn;		/* 0=Disabled; 1=Enabled;*/
	uint16_t MacLimitNum;	/* Limit number of Source Addresses allowed to transmit through Slave device;*/
}st_macLimit;

typedef struct
{
	uint8_t BroadStormEn;	/* �㲥�������ƹ���ʹ�ܡ�0�����ã�1�����á�*/
	uint8_t UniStormEn;		/* δ֪�������ƹ���ʹ�ܡ�0�����ã�1�����á�*/
	uint8_t MultiStormEn;		/* δ֪�鲥���ƹ���ʹ�ܡ�0�����ã�1�����á�*/
	uint8_t StormRateLevel;	/* Storm Filter�������Ƶȼ���1��11��*/
}st_stormFilter;

typedef struct
{
	uint8_t VlanEnable;	/* VLAN����ʹ�ܡ�0�����ã�1�����á�*/
	uint16_t vid[4];		/* �ö˿ڵ�PVID��1��4094��*/
}st_cnuVlan;

typedef struct
{
	uint8_t freqsts;
	uint16_t startfreq;
	uint16_t stopfreq;
}st_cltFreq;

typedef struct
{
	uint8_t cltid;
	uint8_t freqsts;
	uint16_t startfreq;
	uint16_t stopfreq;
}st_cltFreqReq;

typedef struct
{
	uint8_t portPriEn;
	uint8_t pri[4];		/* Port 0�˿����ȼ���0��3��*/	
}st_portPri;

typedef struct
{
	uint32_t rxStatus;
	uint32_t rxCpuRate;
	/* Rate is limited to times of 32kbps.Default 13'h1FFF is for disable rate limit for ingress. 
	** if these bits are set to 0, no frame should be received in from this port.*/
	uint32_t rxEthRate[4];	/* Ingress Rate Limit.*/
	/* Rate is limited to times of 32kbps.Default 13'h1FFF is for disable rate
	** limit for egress. If these bits are set to13'h0, no egress frame should 
	** be send out from this port.*/
}st_rxRateLimit;

typedef struct
{
	uint32_t txStatus;
	uint32_t txCpuRate;
	/* Rate is limited to times of 32kbps.Default 13'h1FFF is for disable rate limit for ingress. 
	** if these bits are set to 0, no frame should be received in from this port.*/
	uint32_t txEthRate[4];	/* Egress Rate Limit.*/
	/* Rate is limited to times of 32kbps.Default 13'h1FFF is for disable rate
	** limit for egress. If these bits are set to13'h0, no egress frame should 
	** be send out from this port.*/
}st_txRateLimit;

typedef struct
{
	uint8_t status;
	uint8_t cpSts;
	uint8_t upSts[4];	/* Port 0�˿�״̬ʹ�ܡ�0�����ã�1�����á�*/
}st_portCtl;

typedef struct
{
	uint16_t port;
	uint8_t rxdelay;
	uint8_t txdelay;
}st_dsdtRgmiiTimingDelay;

typedef struct
{
	uint16_t source;
	uint16_t dest;
}st_dsdtPortMirroring;

////////////////////////////////////////////////////////
typedef struct
{
	uint16_t pvid;
	uint8_t admit_control;
	uint8_t egress_mode;
}st_PortVlanInfo;

typedef struct
{
	uint8_t vlan_enable;
	uint8_t vlan_tag_aware;
	uint8_t ingress_filter;
	uint8_t g_admit_control;
	st_PortVlanInfo vlan_port[6];
	
}st_cnuSwitchVlanConfig;

typedef struct
{
	uint8_t bandwidth_control_enable;
	uint16_t bandwidth_value;	
}st_portRxBandwidthInfo;

typedef struct
{
	uint8_t bandwidth_control_enable;
	uint16_t bandwidth_value;	
}st_portTxBandwidthInfo;

typedef struct
{
	uint8_t g_rx_bandwidth_control_enable;
	uint8_t g_tx_bandwidth_control_enable;
	st_portRxBandwidthInfo rxPort[6];
	st_portTxBandwidthInfo txPort[6];
	
}st_cnuSwitchBandwidthConfig;

typedef struct
{
	/********************************************************
	* 1: Enable loop detection function
	* 0: Disable loop detection function
	*********************************************************/
	uint8_t status;

	/********************************************************
	* This bit should be set to 1 when loop detection function is enabled.
	*********************************************************/
	uint8_t ldmethod;

	/********************************************************
	* Loop detection Timer. 
	* The loop detection packets are transmitted in every LDTIMER.
	* 00: original 3~5 min
	* 01: 100s
	* 10: 10s
	* 11:1s
	*********************************************************/
	uint8_t ldtime;

	/********************************************************
	* Blink frequency of loop detection. 
	* 1: 880 msec.
	* 0: 440 msec
	*********************************************************/
	uint8_t ldbckfrq;

	/********************************************************
	* Loop detection status clearance. 
	* 1: clear all loop status
	* 0: do not effect
	*********************************************************/
	uint8_t ldsclr;

	/********************************************************
	* Select buzzer type. 
	* 1: passive buzzer
	* 0: active buzzer
	*********************************************************/
	uint8_t pabuzzer;

	/************************************************************************
	* Enable tagged loop frame. 
	* 1: enable tagged loop frame, the tagged loop frame will be treated as loop frame
	* 0: disable tagged loop frame, the tagged loop frame will not be treated as loop frame
	************************************************************************/
	uint8_t entaglf;
	
	/********************************************************
	* The initial value of the loop frame TTL field. 
	* The maximum value is 16(LPTTL_INIT[3:0]=4'b0000), 
	* and the minimum value is 1(LPTTL_INIT[3:0]=4'b0001).
	*********************************************************/
	uint8_t lpttlinit;

	/********************************************************
	* Loop Frame Priority assignment. 
	* The 2-bit value is the  priority assigned to the Loop Frame
	*********************************************************/
	uint8_t lpfpri;

	/********************************************************
	* Enable Loop Frame Priority
	* 1: Enable Loop Frame Priority
	* 0: Disable Loop Frame Priority
	* When the loop frame priority is disabled, it will never effect. 
	* The priority of the packet is determined by other rules.
	*********************************************************/
	uint8_t enlpfpri;

	/******************************************************************************
	* Disable filtering Loop Frame in storm filter. 
	* 1: disable filtering loop frame. Loop frame will never be counted into any storm and be filtered
	* 0: Enable filtering loop frame. Treat the loop frame as a normal broadcast
	*******************************************************************************/
	uint8_t disfltlf;

	/********************************************************
	* Enable TTL of loop frame
	* 1: Enable TTL of loop frame
	* 0: Disable TTL of loop frame
	*********************************************************/
	uint8_t enlpttl;

	/* switch source mac address*/
	uint8_t sid[6];
	
	/********************************************************
	* port loop status
	* 1: A loop has been detected on port x
	* 0: No loop exists on port x
	*********************************************************/
	uint8_t port_loop_status[6];
}st_cnuSwitchLoopDetect;

typedef struct
{
	/********************************************************
	* disable broadcast packet storm filter
	* 0: enable
	* 1: disable
	*********************************************************/
	uint8_t disable_broadcast;

	/********************************************************
	* disable multicast packet storm filter
	* 0: enable
	* 1: disable
	*********************************************************/
	uint8_t disable_multicast;

	/********************************************************
	* disable unknown destination unicast packet storm filter
	* 0: enable
	* 1: disable
	*********************************************************/
	uint8_t disable_unknown;

	/********************************************************
	* storm filter rules
	* 0: type 1
	* 1: type 2
	*********************************************************/
	uint8_t rule;

	/********************************************************
	* storm filter reset source
	* 0: both timer and other kind of the packets which is different
	*     from the filtered packet can reset storm filter
	* 1: only timer can reset storm filter
	*********************************************************/
	uint8_t reset_source;

	/********************************************************
	* Storm filter timer selection
	* 00: Storm filter timer is 800ms
	* 01: Storm filter timer is 400ms
	* 10: Storm filter timer is 200ms
	* 11: Storm filter timer is 100ms
	*********************************************************/
	uint16_t iteration;

	/********************************************************
	* 3-bit storm trigger counter threshold
	* 000: 64 broadcast, or multicast, or unknown DA
	*         unicast packets will trigger storm filter if the
	*         corresponding filters are enabled
	* 001: 32 broadcast, or multicast, or unknown DA
	*         unicast packets will trigger storm filter if the
	*         corresponding filters are enabled
	* 010: 16 broadcast, or multicast , or unknown DA
	*         unicast packets will trigger storm filter if the
	*         corresponding filters are enabled
	* 011: 8 broadcast, or multicast, or unknown DA unicast
	*         packets will trigger storm filter if the corresponding
	*         filters are enabled
	* 100: 128 broadcast, or multicast, or unknown DA
	*         unicast packets will trigger storm filter if the
	*         corresponding filters are enabled
	* 101: 256 broadcast, or multicast, or unknown DA
	*         unicast packets will trigger storm filter if the
	*         corresponding filters are enabled
	* 110~111: reserved
	*********************************************************/
	uint16_t thresholt;
}st_cnuSwitchStormFilter;

typedef struct
{
	uint8_t enable;
	uint16_t thresholt;
	uint16_t mport;	
	uint16_t counter;	
}st_systemMacLimit;

typedef struct
{
	uint8_t enable;
	uint16_t thresholt;	
	uint16_t counter;	
}st_portMacLimit;

typedef struct
{
	uint8_t action;
	st_systemMacLimit system;
	st_portMacLimit port[4];
}st_cnuSwitchMacLimit;

typedef struct
{
	uint8_t enable;
}st_portStatusInfo;

typedef struct
{
	st_portStatusInfo port[5];
}st_cnuSwitchPortControl;

typedef struct
{
	st_cnuSwitchVlanConfig vlanConfig;
	st_cnuSwitchBandwidthConfig bandwidthConfig;
	st_cnuSwitchLoopDetect loopDetect;
	st_cnuSwitchStormFilter stormFilter;
	st_cnuSwitchMacLimit macLimit;
	st_cnuSwitchPortControl portControl;
}st_rtl8306eSettings;

typedef struct {
	uint8_t id;
	uint8_t port1linkstatus;
	uint8_t port2linkstatus;
	uint8_t port3linkstatus;
	uint8_t port4linkstatus;
}st_rtl8306e_port_status;
typedef struct
{
	/* ��ʾ���������Ƿ���Ч */
	uint8_t	flag;	
	/* PHY Address */
	uint8_t	phy;	
	/* Register Address */
	uint8_t	reg;	
	/* page Address */
	uint8_t	page;	
	/* Register value */
	uint16_t	value;	
}RTL_REGISTER_DESIGN;

////////////////////////////////////////////////////////

#if 0
/* ���ݱ������Ϣ�ֶ�*/
typedef struct
{
	uint32_t id;			/* template id*/
	uint8_t idle;			/* ��ʶ������ģ���Ƿ��Ѿ���������*/
	uint8_t enable;		/* ��ʶ������ģ���Ƿ�����*/
	uint8_t DevType;		/* ��ʶ������ģ����ݵ�Ӳ������*/
	uint8_t base;			/* ��ʶ������ģ���BASE PIB */
	uint32_t btime;		/* ��ʶ������ģ��ı���ʱ��*/
	uint8_t tname[128];	/* ������ģ�����������*/	
}st_tm_head;

/* PIBҵ�������ֶ�*/
typedef struct
{
	stPibMacLimit pib_spec;
}st_pib_spec;

/* �Զ��������������Ϣ�ֶ�*/
typedef struct
{
	stModStormFilter stormFilter;
	stModVlan vlan;
	stModPortPri portPri;
	stModRateLimit rateLimit;
	stModPortEnable portAuth;
}st_mod_spec;

/* template table info*/
typedef struct
{
	st_tm_head tm_info;	/* ���ݱ������Ϣ�ֶ�*/
	st_pib_spec pib;		/* PIBҵ�������ֶ�*/
	st_mod_spec mod;	/* �Զ��������������Ϣ�ֶ�*/
}T_szTemplate;
#endif
/***********************************************************************/

typedef struct
{
	uint32_t phy;
	uint32_t reg;
	uint32_t value;
}T_szMdioPhy;

typedef struct
{
	uint32_t reg;
	uint32_t value;
}T_szMdioSw;

typedef struct
{
	uint32_t phy;
	uint32_t reg;
	uint32_t page;
	uint32_t value;
}T_szMdioRtl8306e;

/*==============================================================*/

/*==============================================================*/
/*                                    MMEADģ���ⲿ��Ϣ�ӿڶ���                                      */
typedef struct
{
	uint16_t M_TYPE;		/*��ʾMMEAD MSG��������,����Ϊ0xCC08*/
	uint16_t DEV_TYPE;	/*�豸����,���ֶ�����Ӳ�����ݴ���*/
	uint16_t MM_TYPE;	/*��Ϣ���ͣ���ʾ�������Ϣ*/
	uint8_t fragment;		/*�Ƿ���ڸ����Ƭ*/
	uint8_t ODA[6];		/*Ŀ��MAC��ַ*/
	uint32_t LEN;			/*��Ϣ��ĳ���*/
}
T_Msg_Header_MMEAD;

typedef struct
{
	T_Msg_Header_MMEAD HEADER;
	uint8_t BUF[0];		/*��Ϣ������*/
}
T_Msg_MMEAD;

typedef struct
{
	uint16_t M_TYPE;		/*��ʾMMEAD MSG��������,����Ϊ0xCC08*/
	uint16_t DEV_TYPE;	/*�豸����,���ֶ�����Ӳ�����ݴ���*/
	uint16_t MM_TYPE;	/*��Ϣ���ͣ���ʾ�������Ϣ*/
	uint8_t fragment;		/*�Ƿ���ڸ����Ƭ*/
	uint32_t LEN;			/*��Ϣ��ĳ���*/	
}
T_REQ_Msg_Header_MMEAD;

typedef struct
{
	T_REQ_Msg_Header_MMEAD HEADER;
	uint16_t result;		/*����״̬*/
	uint8_t BUF[0];		/*��Ϣ������*/
}
T_REQ_Msg_MMEAD;

/* new mmets struct defined by frank */
/*****************************************************************************************************/
/* mmets ������Ϣͷ����*/
typedef struct
{
	uint16_t M_TYPE;		/*��ʾMMEAD MSG��������,����Ϊ0xCC08*/
	uint16_t DEV_TYPE;	/*�豸����,���ֶ�����Ӳ�����ݴ���*/
	uint16_t MM_TYPE;	/*��Ϣ���ͣ���ʾ�������Ϣ*/
	uint8_t fragment;		/*�Ƿ���ڸ����Ƭ*/
	uint8_t ODA[6];		/*Ŀ��MAC��ַ*/
	uint32_t LEN;			/*��Ϣ��ĳ���*/
}
T_MMETS_REQ_HEADER;

/* mmets ������Ϣ�ṹ����*/
typedef struct
{
	T_MMETS_REQ_HEADER header;	/* ������Ϣͷ*/
	uint8_t body[0];				/* ������Ϣ������*/
}
T_MMETS_REQ_MSG;

/* mmets Ӧ����Ϣͷ����*/
typedef struct
{
	uint16_t M_TYPE;		/*��ʾMMEAD MSG��������,����Ϊ0xCC08*/
	uint16_t DEV_TYPE;	/*�豸����,���ֶ�����Ӳ�����ݴ���*/
	uint16_t MM_TYPE;	/*��Ϣ���ͣ���ʾ�������Ϣ*/
	uint8_t fragment;		/*�Ƿ���ڸ����Ƭ*/
	uint32_t LEN;			/*��Ϣ��ĳ��ȣ�����result + body �ĳ���*/
	uint16_t result;		/*����״̬*/
}
T_MMETS_ACK_HEADER;

/* mmets Ӧ����Ϣ�ṹ����*/
typedef struct
{
	T_MMETS_ACK_HEADER header;	/* Ӧ����Ϣͷ*/
	uint8_t body[0];				/* Ӧ����Ϣ������*/
}
T_MMETS_ACK_MSG;
/*****************************************************************************************************/

typedef struct 
{
	uint8_t dir;
	uint8_t ccoMac[6];
	uint8_t peerNodeMac[6];
	uint8_t chipser;
}T_MMEAD_LINK_DIAG_INFO;

typedef struct 
{
	uint8_t dir;
	uint8_t mac[6];
	uint8_t model;
	uint8_t peerNodeMac[6];
	uint8_t tei;	
	uint8_t ccoMac[6];
	uint8_t ccoTei;
	uint8_t ccoSnid;
	uint8_t ccoNid[7];	
	uint16_t rx;
	uint16_t tx;
	uint8_t bridgedMac[6];
	float bitRate;
	uint8_t attenuation;
	uint64_t MPDU_ACKD;
	uint64_t MPDU_COLL;
	uint64_t MPDU_FAIL;
	uint64_t PBS_PASS;
	uint64_t PBS_FAIL;	
}T_MMEAD_LINK_DIAG_RESULT;

typedef struct 
{
	uint8_t DevType;
	uint32_t CRC[2];
}T_MMEAD_DEV_INFO;

typedef struct
{
	uint8_t DevType;
	uint8_t NumStas;
	uint8_t Mac[6];
	uint32_t CRC[2];
}T_MMEAD_CLT_INFO;

typedef struct
{
	uint8_t DevType;
	uint8_t AvgPhyRx;
	uint8_t AvgPhyTx;
	uint8_t Tei;
	uint8_t Mac[6];
	uint32_t CRC[2];
}T_MMEAD_CNU_INFO;

typedef struct
{
	T_MMEAD_CLT_INFO clt;
	T_MMEAD_CNU_INFO cnu[MMEAD_MAX_CNU_NUM];
}T_MMEAD_TOPOLOGY;

typedef struct
{
	uint8_t FBSTATUS;
	uint16_t START_BAND;
	uint16_t STOP_BAND;	
}T_MMEAD_FBS;

typedef struct
{
	uint16_t MODULE_ID;
	uint16_t MODULE_SUB_ID;
	uint32_t LENGTH;
	uint32_t OFFSET;
}T_MMEAD_RD_MOD_REQ_INFO;

typedef struct
{
	uint16_t MODULE_ID;
	uint16_t MODULE_SUB_ID;
	uint32_t LENGTH;
	uint32_t OFFSET;
	uint8_t MODULE_DATA[1024];
}T_MMEAD_RD_MOD_ACK_INFO;

typedef struct
{
	uint16_t MODULE_ID;
	uint16_t MODULE_SUB_ID;
	uint32_t MODULE_LENGTH;
	uint8_t MODULE_DATA[1400];
}T_MMEAD_WR_MOD_REQ_INFO;

typedef struct
{
	uint16_t MODULE_ID;
	uint16_t MODULE_SUB_ID;
}T_MMEAD_ERASE_MOD_REQ_INFO;

typedef struct
{
	uint16_t MODULE_ID;
	uint16_t MODULE_SUB_ID;
	uint32_t MODULE_LENGTH;
	uint16_t MSTATUS;
	uint16_t ERR_REC_CODE;
}T_MMEAD_WR_MOD_ACK_INFO;

/* �澯����ģ����SNMPģ��֮��Ĺ������ݽṹ*/

typedef struct
{
	uint32_t ItemNum;
	char trap_info[64];
	char oid[64];
	uint32_t AlarmCode;
	uint32_t RealTime;
	uint32_t SerialFlow;
	uint8_t CbatMac[18];
	uint32_t CltIndex;
	uint32_t CnuIndex;
	uint32_t AlarmType;
	uint32_t AlarmValue;
}stAlarmInfo;

/* �澯����ģ��ɼ��澯��Ϣ��ͨ�����ݽṹ*/
typedef struct
{
	uint32_t AlarmCode;
	uint32_t CltIndex;
	uint32_t CnuIndex;
	uint32_t PortIndex;
	uint32_t AlarmStatus;
	uint32_t AlarmValue;
	uint8_t AlarmInfo[64];
}T_ALARM_DESC;

/*==============================================================*/

/*==============================================================*/
/*                                                       ��־��ʽ����                                                   */

typedef struct
{
	uint32_t serialNum;	/* ��¼��־����ˮ��*/
	time_t time;			/* ����������ʱ��*/
	uint8_t user;			/* ִָ�иò������û����� */
	uint16_t action;		/* �������� */	
	int errorCode;		/* �ò�����ִ��״̬ */
}
T_OPT_LOG;

/* ��־ģ��ṹ*/
typedef struct
{
	uint32_t logtime;
	uint32_t user;
	uint32_t loglevel;
	uint32_t logstatus;
	uint8_t loginfo[128];
	/*//�����жϣ����ս׶� 1 read,2 write ��������
	//���ͽ׶� 1 δ���� 0 ��������
	int  modes;*/
}syslog_r;  

/* CLI ��־��ѯ�ṹ���˽ṹ������CLI��CMMͨѶ������Ϣ*/
typedef struct
{
	uint32_t total_row;
	uint32_t count;
	syslog_r ACK_LOG[QUREY_ROW_ONCE];
}T_szSyslog;

/* CLI ��־��ѯ�ṹ�����˽ṹ������CLI��CMMͨѶ������Ϣ*/
typedef struct
{
	uint32_t total_row;
	uint32_t count;
	stAlarmInfo ACK_ALARMLOG[QUREY_ROW_ONCE];
}T_szAlarmlog;

/* CLI������־���߸澯��ѯʱ�����Ĵ���Ľṹ��*/
typedef struct
{
	/* ������־�������ܻ��Ƭ�����ֶ�
	�ж��Ƿ񽫸÷�Ƭ��Ϣ��¼��־*/
	uint8_t flag;			/* 1: ��Ҫ��¼��־��0: ����Ҫ��¼��־*/		
	uint32_t start_row;	/* ��ѯ��־����ʼ�У��кŴ�0��ʼ*/
	uint32_t count;		/* ��ʾ��Ҫһ�β�ѯ�ļ�¼������Ŀǰ�����QUREY_ROW_ONCE */
}T_szLogReqInfo;

/* CLI����AR8236 PHY �Ĵ�������ʱ��CMMͨѶ�Ľӿ�*/
typedef struct
{
	uint32_t clt;	/* CLT�������ţ���1��ʼ����*/
	uint32_t cnu;	/* CNU�������ţ���1��ʼ����*/
	T_szMdioPhy mdioPhy;
}T_szAr8236Phy;

/* CLI����AR8236 �Ĵ�������ʱ��CMMͨѶ�Ľӿ�*/
typedef struct
{
	uint32_t clt;	/* CLT�������ţ���1��ʼ����*/
	uint32_t cnu;	/* CNU�������ţ���1��ʼ����*/
	T_szMdioSw mdioReg;
}T_szAr8236Reg;

/* CLI����RTL8306E �Ĵ�������ʱ��CMMͨѶ�Ľӿ�*/
typedef struct
{
	uint32_t clt;	/* CLT�������ţ���1��ʼ����*/
	uint32_t cnu;	/* CNU�������ţ���1��ʼ����*/
	T_szMdioRtl8306e mdioInfo;
}T_szSwRtl8306eConfig;

/* CLI���ж�ȡCNU��HFIDʱ��CMMͨѶ�Ľӿ�*/
typedef struct
{
	uint8_t ODA[6];
	uint8_t pdata[64];
}T_szCnuUserHFID;

typedef struct
{
	uint8_t wan_name[255];
	uint8_t vlan[2];
	uint8_t priority;
	uint8_t ip_assigned_mode;
	uint8_t connection_protocol;
	uint8_t user_name[255];
	uint8_t password[255];
	uint8_t ipv4_addr[4];
	uint8_t ipv4_mask[4];
	uint8_t ipv4_gw[4];
	uint8_t dns_ipv4[4];
	uint8_t dns_ipv4_copy[4];
}T_szHgManage;

typedef struct
{
	uint8_t wan_name_1[100];
	uint8_t vlan_1[2];
	uint8_t priority_1;
	uint8_t connection_mode_1;
	uint8_t ip_assigned_mode_1;
	uint8_t service_type_1;
	uint8_t connection_protocol_1;
	uint8_t bind_interface_1[4];
	uint8_t user_name_1[100];
	uint8_t password_1[100];
	uint8_t ipv4_addr_1[4];
	uint8_t ipv4_mask_1[4];
	uint8_t ipv4_gw_1[4];
	uint8_t dns_ipv4_1[4];
	uint8_t dns_ipv4_copy_1[4];
	uint8_t wan_name_2[100];
	uint8_t vlan_2[2];
	uint8_t priority_2;
	uint8_t connection_mode_2;
	uint8_t ip_assigned_mode_2;
	uint8_t service_type_2;
	uint8_t connection_protocol_2;
	uint8_t bind_interface_2[4];
	uint8_t user_name_2[100];
	uint8_t password_2[100];
	uint8_t ipv4_addr_2[4];
	uint8_t ipv4_mask_2[4];
	uint8_t ipv4_gw_2[4];
	uint8_t dns_ipv4_2[4];
	uint8_t dns_ipv4_copy_2[4];
}T_szHgBusiness;

typedef struct
{
	uint8_t ssid_status;
	uint8_t ssid_name1[32];
	uint8_t ssid_name2[32];
	uint8_t ssid_name3[32];
	uint8_t ssid_name4[32];
	uint8_t ssid_pwd1[32];
	uint8_t ssid_pwd2[32];
	uint8_t ssid_pwd3[32];
	uint8_t ssid_pwd4[32];
}T_szHgSsid;

typedef struct
{	
	uint8_t wan_name_1[100];
	uint16_t vlan_1;
	uint8_t priority_1;
	uint8_t connection_mode_1;
	uint8_t ip_assigned_mode_1;
	uint8_t service_type_1;
	uint8_t connection_protocol_1;
	uint8_t bind_lan_1[4];
	uint8_t bind_ssid_1;
	uint8_t user_name_1[100];
	uint8_t password_1[100];
	uint8_t ipv4_addr_1[4];
	uint8_t ipv4_mask_1[4];
	uint8_t ipv4_gw_1[4];
	uint8_t dns_ipv4_1[4];
	uint8_t dns_ipv4_copy_1[4];
	uint8_t wan_name_2[100];
	uint16_t vlan_2;
	uint8_t priority_2;
	uint8_t connection_mode_2;
	uint8_t ip_assigned_mode_2;
	uint8_t service_type_2;
	uint8_t connection_protocol_2;
	uint8_t bind_lan_2[4];
	uint8_t bind_ssid_2;
	uint8_t user_name_2[100];
	uint8_t password_2[100];
	uint8_t ipv4_addr_2[4];
	uint8_t ipv4_mask_2[4];
	uint8_t ipv4_gw_2[4];
	uint8_t dns_ipv4_2[4];
	uint8_t dns_ipv4_copy_2[4];
}T_szNmsBusiness;

typedef struct
{
	uint8_t ssid_name1[100];
	uint8_t ssid_status1;
	uint8_t ssid_name2[100];
	uint8_t ssid_status2;
}T_szNmsSsid;

typedef struct
{
	uint8_t wan_name1[100];
	uint8_t wan_status1;
	uint8_t wan_name2[100];
	uint8_t wan_status2;
}T_szNmsWanStatus;

typedef struct 
{
	char wan_name[100];
	int vlan;
	int priority;
	int connection_mode;
	int ip_assigned_mode;
	int service_type;
	int connection_protocol;
	char bind_lan[5];
	int bind_ssid;
	char user_name[100];
	char password[100];
	char ipv4_addr[16];
	char ipv4_mask[16];
	char ipv4_gw[16];
	char dns_ipv4[16];
	char dns_ipv4_copy[16];
}T_szSetNmsBusiness;

typedef struct
{
	uint8_t ssid_index;
	uint8_t ssid_name[100];
	uint8_t ssid_status;
}T_szSetNmsSsid;

typedef struct
{
	uint8_t wan_name[100];
	uint8_t vlan[2];
	uint8_t priority;
	uint8_t connection_mode;
	uint8_t ip_assigned_mode;
	uint8_t service_type;
	uint8_t connection_protocol;
	uint8_t bind_interface[4];
	uint8_t user_name[100];
	uint8_t password[100];
	uint8_t ipv4_addr[4];
	uint8_t ipv4_mask[4];
	uint8_t ipv4_gw[4];
	uint8_t dns_ipv4[4];
	uint8_t dns_ipv4_copy[4];
}T_szSetHgBusiness;


typedef struct 
{
	uint32_t clt;
	uint32_t cnu;
	T_szHgManage wanInfo;
}T_szHgManageConfig;

typedef struct
{
	uint32_t clt;
	uint32_t cnu;
	T_szHgBusiness wanInfo;
}T_szHgBusinessConfig;

typedef struct
{
	uint32_t clt;
	uint32_t cnu;
	T_szSetHgBusiness wanInfo;
}T_szSetHgBusinessConfig;

typedef struct
{
	uint8_t ssid_index;
	uint8_t ssid_status;
	uint8_t ssid_name[32];
	uint8_t ssid_pwd[32];
}T_szSetHgSsid;


typedef struct
{
	uint32_t clt;
	uint32_t cnu;
	T_szSetHgSsid ssidInfo;
}T_szSetHgSsidStatus;

typedef struct
{
	uint8_t wan_name1[100];
	uint8_t wan_status1;
	uint8_t wan_name2[100];
	uint8_t wan_status2;
	uint8_t wan_name3[100];
	uint8_t wan_status3;
	uint8_t wan_name4[100];
	uint8_t wan_status4;
}T_szHgWanStatus;

/* CLI����set vlan ����ʱ��CMMͨѶ�Ľӿ�*/
typedef struct
{
	uint32_t clt;	/* CLT�������ţ���1��ʼ����*/
	uint32_t cnu;	/* CNU�������ţ���1��ʼ����*/
	uint16_t port;/*���õĶ˿ں�*/
	T_szMdioSw mdioReg;
}T_szAr8236Vlan;

/* CLI����flow-control ����ʱ��CMMͨѶ�Ľӿ�*/
typedef struct
{	
	uint32_t clt;	/* CLT�������ţ���1��ʼ����*/
	uint32_t cnu;	/* CNU�������ţ���1��ʼ����*/
	uint16_t port;/*���õĶ˿ں�*/
	uint32_t uprate;	/* ��������*/
	uint32_t downrate;	/* ��������*/
}T_szFlowControl;

typedef struct 
{
	uint8_t dir;
	uint32_t clt;
	uint32_t cnu;
}T_CMM_LINK_DIAG_INFO; 

typedef struct 
{
	uint8_t linkStatus;
	uint8_t speed;
	uint8_t duplex;
	uint8_t pri;
	uint8_t flowControl;
	uint8_t portStatus;
}T_CMM_PORT_PROPETY_INFO; 

typedef struct 
{
	uint32_t InBroadcasts;
	uint32_t InMulticasts;
	uint32_t InUnicasts;
	uint32_t rxCtr;
	uint32_t InGoodOctets;
	uint32_t OutBroadcasts;
	uint32_t OutMulticasts;
	uint32_t OutUnicasts;
	uint32_t txCtr;
	uint32_t OutGoodOctets;
}T_CMM_PORT_STATS_INFO; 
/*==============================================================*/


/*==============================================================*/
/*                                                       snmp ģ�鴫���ʽ                                                   */

/* snmp ��CMM ͨ�ŵ�Ӧ��ṹ��*/

typedef struct 
{
	uint16_t usSrcMID;
	uint16_t usDstMID;
	uint16_t usMsgType;
	uint8_t   fragment;
	uint32_t ulBodyLength;
	uint16_t Result;
}T_CMM_MSG_HEADER_ACK;

typedef struct 
{
	T_CMM_MSG_HEADER_ACK header;
	uint8_t BUF[0];
}T_CMM_MSG_ACK;

typedef struct 
{
	T_Msg_Header_CMM header;
	uint8_t BUF[0];	
}T_CMM_MSG_HEADER_REQ;

/*��ȡ����CNU ����*/
typedef struct
{
	uint16_t index[64]; 
}snmp_cnu_index_t;

/* szOidVariables �ṹ�嶨��  */
typedef struct
{
	uint32_t       variables_oid[64];
	uint32_t       variables_oid_len;
	uint8_t         magic;
	uint32_t       value;
}szOidVariables_t;

/* szTblVariables*/
typedef struct
{
	uint32_t  variables_tbl[64];
	uint32_t  variables_tbl_len;
	uint32_t    row;
	uint32_t data_len;
	uint8_t 	data[0];
}szTblVariables_t;

/* szTbloidVariables
typedef struct szTblOidVariables_s
{
	uint32_t  variables_tbl[64];
	uint32_t  variables_tbl_len;
	uint8_t    row;
	uint8_t    magic;
	uint32_t value;
}szTblOidVariables_t;
*/
/* szTbloidVariables template table*/
typedef struct
{
	uint32_t  variables_tbl[64];
	uint32_t  variables_tbl_len;
	uint8_t    row;
	uint8_t    magic;
	uint32_t    val;
	uint8_t 	BUF[0];
}szTblOidVariables_t;

#if 0
/* table_data*/
typedef struct
{
	uint8_t CltIndex;
	uint8_t CnuIndex;
	uint8_t mac[6];
	uint8_t online;
	uint8_t DevType;
	uint8_t UserType;
	uint8_t AuthorSts;
	uint8_t SwVer[256];
	uint32_t RfAtten;
	uint32_t RfDwQlt;
	uint32_t RfUpQlt;
	uint8_t RxRate;
	uint8_t TxRate;
}table_data_t;

typedef struct  {
	unsigned long master_index;
	unsigned long cnu_index;
	unsigned char mac[6];
	unsigned long devtype;
	unsigned long online;
	unsigned char sw_ver[256];
	unsigned long rfatt;
	unsigned long rfdownq;
	unsigned long rfupq;
	unsigned long phydownrate;
	unsigned long phyuprate;
	unsigned long usertype;
	unsigned long tempid;
	unsigned long accessenable;
	unsigned long autoupenable;
	unsigned long onused;

}cnu_table_data_t;
#endif

/* �ⲿģ���������ݿ������������ͷ����*/
typedef struct
{
	uint16_t usSrcMID;			/*Դģ��ID*/
	uint16_t usDstMID;			/*Ŀ��ģ��ID*/
	uint16_t usMsgType;			/*��Ϣ���ͣ���ʾ�������Ϣ*/
	uint32_t ulBodyLength;		/*��Ϣ�峤��*/
}
T_DB_MSG_HEADER_REQ;

/* �ⲿģ���������ݿ�����������Ķ���*/
typedef struct
{
	T_DB_MSG_HEADER_REQ HEADER;
	uint8_t BUF[0];				/*�������Ϣ������*/
}
T_DB_MSG_PACKET_REQ;

/* ���ݿ�ģ��Ӧ����ͷ����*/
typedef struct
{
	uint16_t usSrcMID;			/*Դģ��ID*/
	uint16_t usDstMID;			/*Ŀ��ģ��ID*/
	uint16_t usMsgType;			/*��Ϣ���ͣ���ʾ�������Ϣ*/
	uint8_t fragment;				/*�Ƿ���ڸ����Ƭ*/
	uint16_t result;				/*����״̬*/
	uint32_t ulBodyLength;		/*��Ϣ�峤��*/
}
T_DB_MSG_HEADER_ACK;

/* ���ݿ�ģ��Ӧ���Ķ���*/
typedef struct
{
	T_DB_MSG_HEADER_ACK HEADER;
	uint8_t BUF[0];				/*���ص���Ϣ������*/
}
T_DB_MSG_PACKET_ACK;

typedef struct
{
	uint8_t DevType;
	uint8_t Mac[6];
	BOOLEAN online;
	uint32_t NumStas;
	uint32_t MaxStas;
}T_CLT_INFO;

typedef struct
{
	uint8_t DevType;
	uint8_t Mac[6];
	BOOLEAN online;
	uint8_t RxRate;
	uint8_t TxRate;
	BOOLEAN OnUsed;
}T_CNU_INFO;

typedef struct
{
	T_CLT_INFO tb_clt[MAX_CLT_AMOUNT_LIMIT];
	T_CNU_INFO tb_cnu[MAX_CNU_AMOUNT_LIMIT];
}T_TOPOLOGY_INFO;

/*
typedef struct
{
	uint32_t id;
	uint32_t DevType;
	uint8_t mac[6];
	uint32_t online;
	uint32_t RxRate;
	uint32_t TxRate;
	uint32_t Attenuation;
	uint32_t DwQuality;
	uint32_t UpQuality;
	uint32_t UserType;
	uint32_t TempId;
	uint32_t AccessEnable;
	uint32_t AutoUpEnable;
	uint8_t SwVersion[128];
	uint32_t OnUsed;
	uint32_t csyncStatus;
}DB_ROW_TBL_CNU;*/

/***************************** DBS �ⲿ��Ϣ�ӿ�******************************/
/***************************** DBS �ⲿ��Ϣ�ӿ�******************************/
/***************************** DBS �ⲿ��Ϣ�ӿ�******************************/
/***************************** DBS �ⲿ��Ϣ�ӿ�******************************/

typedef struct
{
	uint32_t	id;
	uint8_t	col_user[32];
	uint8_t	col_pwd[16];
}st_dbsCliRole;

typedef struct
{
	st_dbsCliRole role[3];
}st_dbsCliRoles;

typedef struct
{
	uint32_t	id;
	uint32_t	col_model;
	uint8_t	col_mac[32];
	uint32_t	col_sts;
	uint32_t	col_maxStas;
	uint32_t	col_numStas;
	uint8_t	col_swVersion[128];
	uint32_t	col_synch;
	uint32_t	col_row_sts;
}st_dbsClt;

typedef struct
{
	uint32_t	id;
	uint32_t	col_row_sts;	
	uint32_t	col_base;
	uint32_t	col_macLimit;
	uint32_t	col_curate;
	uint32_t	col_cdrate;
	uint32_t	col_loagTime;
	uint32_t	col_reagTime;
	uint32_t	col_igmpPri;
	uint32_t	col_unicastPri;
	uint32_t	col_avsPri;
	uint32_t	col_mcastPri;
	uint32_t	col_tbaPriSts;
	uint32_t	col_cosPriSts;
	uint32_t	col_cos0pri;
	uint32_t	col_cos1pri;
	uint32_t	col_cos2pri;
	uint32_t	col_cos3pri;
	uint32_t	col_cos4pri;
	uint32_t	col_cos5pri;
	uint32_t	col_cos6pri;
	uint32_t	col_cos7pri;
	uint32_t	col_tosPriSts;
	uint32_t	col_tos0pri;
	uint32_t	col_tos1pri;
	uint32_t	col_tos2pri;
	uint32_t	col_tos3pri;
	uint32_t	col_tos4pri;
	uint32_t	col_tos5pri;
	uint32_t	col_tos6pri;
	uint32_t	col_tos7pri;
}st_dbsCltConf;

typedef struct
{
	uint32_t	id;
	uint32_t	col_model;
	uint8_t	col_mac[32];
	uint32_t	col_sts;
	uint32_t	col_auth;
	uint8_t	col_ver[128];
	uint32_t	col_rx;
	uint32_t	col_tx;
	uint8_t	col_snr[16];
	uint8_t	col_bpc[16];
	uint8_t	col_att[16];
	uint32_t	col_synch;
	uint32_t	col_row_sts;
	uint32_t	col_auto_sts;
	uint8_t     col_user_hfid[64];
}st_dbsCnu;

typedef struct
{
	uint32_t	id;
	uint32_t	col_base;
	uint32_t	col_macLimit;
	uint32_t	col_curate;
	uint32_t	col_cdrate;
	uint32_t	col_loagTime;
	uint32_t	col_reagTime;
	uint32_t	col_igmpPri;
	uint32_t	col_unicastPri;
	uint32_t	col_avsPri;
	uint32_t	col_mcastPri;
	uint32_t	col_tbaPriSts;
	uint32_t	col_cosPriSts;
	uint32_t	col_cos0pri;
	uint32_t	col_cos1pri;
	uint32_t	col_cos2pri;
	uint32_t	col_cos3pri;
	uint32_t	col_cos4pri;
	uint32_t	col_cos5pri;
	uint32_t	col_cos6pri;
	uint32_t	col_cos7pri;
	uint32_t	col_tosPriSts;
	uint32_t	col_tos0pri;
	uint32_t	col_tos1pri;
	uint32_t	col_tos2pri;
	uint32_t	col_tos3pri;
	uint32_t	col_tos4pri;
	uint32_t	col_tos5pri;
	uint32_t	col_tos6pri;
	uint32_t	col_tos7pri;
	uint32_t	col_sfbSts;
	uint32_t	col_sfuSts;
	uint32_t	col_sfmSts;
	uint32_t	col_sfRate;
	uint32_t	col_vlanSts;
	uint32_t	col_eth1vid;
	uint32_t	col_eth2vid;
	uint32_t	col_eth3vid;
	uint32_t	col_eth4vid;
	uint32_t	col_portPriSts;
	uint32_t	col_eth1pri;
	uint32_t	col_eth2pri;
	uint32_t	col_eth3pri;
	uint32_t	col_eth4pri;
	uint32_t	col_rxLimitSts;			/* CNU ��������*//* ��λΪKb */
	uint32_t	col_cpuPortRxRate;		/* CPU �˿���������Ĭ��4096Kb*/
	uint32_t	col_eth1rx;				/* �û��˿�Ĭ�����в����� */
	uint32_t	col_eth2rx;
	uint32_t	col_eth3rx;
	uint32_t	col_eth4rx;
	uint32_t	col_txLimitSts;			/* CNU ��������*//* ��λΪKb */
	uint32_t	col_cpuPortTxRate;		/* CPU �˿���������Ĭ��256Kb*/
	uint32_t	col_eth1tx;				/* �û��˿�Ĭ�����в����� */
	uint32_t	col_eth2tx;
	uint32_t	col_eth3tx;
	uint32_t	col_eth4tx;
	uint32_t	col_psctlSts;
	uint32_t	col_cpuPortSts;
	uint32_t	col_eth1sts;
	uint32_t	col_eth2sts;
	uint32_t	col_eth3sts;
	uint32_t	col_eth4sts;
}st_dbsCnuDefaultProfile;

typedef struct
{
	uint32_t	id;
	uint32_t	col_tempAutoSts;
	uint32_t	col_curTemp;
	uint32_t	col_eth1VlanAddSts;
	uint32_t	col_eth1VlanStart;
	uint32_t	col_eth1VlanStop;
	uint32_t	col_eth2VlanAddSts;
	uint32_t	col_eth2VlanStart;
	uint32_t	col_eth2VlanStop;
	uint32_t	col_eth3VlanAddSts;
	uint32_t	col_eth3VlanStart;
	uint32_t	col_eth3VlanStop;
	uint32_t	col_eth4VlanAddSts;
	uint32_t	col_eth4VlanStart;
	uint32_t	col_eth4VlanStop;
}st_dbsTemplate;



/* tbl_network */
typedef struct
{
	uint32_t	id;
	uint8_t	col_ip[32];
	uint8_t	col_netmask[32];
	uint8_t	col_gw[32];
	uint32_t	col_dhcp;	
	uint8_t	col_dns[32];
	uint8_t	col_mac[32];
	uint32_t	col_mvlan_sts;
	uint32_t	col_mvlan_id;
}st_dbsNetwork;

typedef struct
{
	uint32_t	id;
	uint32_t	col_row_sts;
	uint32_t	col_base;
	uint32_t	col_macLimit;
	uint32_t	col_curate;
	uint32_t	col_cdrate;
	uint32_t	col_loagTime;
	uint32_t	col_reagTime;
	uint32_t	col_igmpPri;
	uint32_t	col_unicastPri;
	uint32_t	col_avsPri;
	uint32_t	col_mcastPri;
	uint32_t	col_tbaPriSts;
	uint32_t	col_cosPriSts;
	uint32_t	col_cos0pri;
	uint32_t	col_cos1pri;
	uint32_t	col_cos2pri;
	uint32_t	col_cos3pri;
	uint32_t	col_cos4pri;
	uint32_t	col_cos5pri;
	uint32_t	col_cos6pri;
	uint32_t	col_cos7pri;
	uint32_t	col_tosPriSts;
	uint32_t	col_tos0pri;
	uint32_t	col_tos1pri;
	uint32_t	col_tos2pri;
	uint32_t	col_tos3pri;
	uint32_t	col_tos4pri;
	uint32_t	col_tos5pri;
	uint32_t	col_tos6pri;
	uint32_t	col_tos7pri;
	uint32_t	col_sfbSts;
	uint32_t	col_sfuSts;
	uint32_t	col_sfmSts;
	uint32_t	col_sfRate;
	uint32_t	col_vlanSts;
	uint32_t	col_eth1vid;
	uint32_t	col_eth2vid;
	uint32_t	col_eth3vid;
	uint32_t	col_eth4vid;
	uint32_t	col_portPriSts;
	uint32_t	col_eth1pri;
	uint32_t	col_eth2pri;
	uint32_t	col_eth3pri;
	uint32_t	col_eth4pri;
	uint32_t	col_rxLimitSts;
	uint32_t	col_cpuPortRxRate;
	uint32_t	col_eth1rx;
	uint32_t	col_eth2rx;
	uint32_t	col_eth3rx;
	uint32_t	col_eth4rx;
	uint32_t	col_txLimitSts;
	uint32_t	col_cpuPortTxRate;
	uint32_t	col_eth1tx;			/* 32 ������������λΪKbps��д��Ĵ���ǰ��Ҫ����32 */
	uint32_t	col_eth2tx;
	uint32_t	col_eth3tx;
	uint32_t	col_eth4tx;
	uint32_t	col_psctlSts;
	uint32_t	col_cpuPortSts;
	uint32_t	col_eth1sts;
	uint32_t	col_eth2sts;
	uint32_t	col_eth3sts;
	uint32_t	col_eth4sts;
	uint32_t    col_uplinkvid;
	uint32_t 	col_uplinkVMode;
	uint32_t   	col_eth1VMode;
	uint32_t    col_eth2VMode;
	uint32_t   	col_eth3VMode;
	uint32_t    col_eth4VMode;
}st_dbsProfile;

/* tbl_snmp */
typedef struct
{
	uint32_t	id;
	uint8_t	col_rdcom[64];
	uint8_t	col_wrcom[64];
	uint8_t	col_trapcom[64];
	uint8_t	col_sina[32];
	uint8_t	col_sinb[32];
	uint32_t	col_tpa;
	uint32_t	col_tpb;
}st_dbsSnmp;

/* tbl_swmgmt */
typedef struct
{
	uint32_t	id;
	uint8_t	col_ip[32];
	uint32_t	col_port;
	uint8_t	col_user[32];
	uint8_t	col_pwd[16];
	uint8_t	col_path[256];
}st_dbsSwmgmt;

/* tbl_sysinfo 	*/
typedef struct
{
	uint32_t	id;
	uint32_t	col_model;
	uint32_t	col_maxclt;
	uint8_t	col_hwver[64];
	uint8_t	col_bver[64];
	uint8_t	col_kver[64];
	uint8_t	col_appver[64];
	uint32_t	col_flashsize;
	uint32_t	col_ramsize;
	uint32_t	col_wlctl;
	uint32_t	col_autoud;
	uint32_t	col_autoconf;
	uint32_t	col_wdt;
	uint8_t	col_mfinfo[128];
	uint32_t	col_p6rxdelay;
	uint32_t	col_p6txdelay;
	uint8_t   col_apphash[64];
}st_dbsSysinfo;

#if 0
/*display sysinfo*/
typedef struct
{
	T_szSysinfo szSysinfo;
	uint8_t mac[6];
}T_sysinfo;
#endif

typedef struct
{
	uint32_t serialFlow;
	uint8_t trap_info[64];
	uint8_t oid[64];
	uint32_t alarmCode;
	uint32_t realTime;	
	uint32_t itemNum;
	uint8_t cbatMac[32];
	uint32_t cltId;
	uint32_t cnuId;
	uint32_t alarmType;
	uint32_t alarmValue;
}st_dbsAlarmlog;

typedef struct
{
	uint32_t time;
	uint32_t who;
	uint8_t cmd[128];
	uint32_t level;
	uint32_t result;
}st_dbsOptlog;

typedef struct
{
	uint32_t time;
	uint32_t who;
	uint32_t level;
	uint8_t log[256];
}st_dbsSyslog;

typedef struct
{
	uint16_t mid;
	uint16_t status;
}st_dbsMslog;

typedef struct
{
	uint16_t tbl;			/* ��Ҫ�����ı����Ƶ�����*/
	uint16_t row;			/* �ñ������е�Ψһ��������1��ʼ����*/
	uint16_t col;			/* ��id ����0��ʼ����*/
	uint16_t colType;		/* �е���������*/
}DB_COL_INFO;

typedef struct
{
	DB_COL_INFO ci;
	uint32_t len;			/* data �ĳ��� */
	uint8_t data[0];		/* ��ֵ*/
}DB_COL_VAR;

typedef struct
{
	DB_COL_INFO ci;
	uint32_t len;			/* len==0��ʾintegerΪnull*/
	int32_t integer;		/* ��ֵ*/
}DB_INTEGER_V;

typedef struct
{
	DB_COL_INFO ci;
	uint32_t len;							/* len==0��ʾtextΪnull*/
	uint8_t text[DBS_COL_MAX_LEN];		/* ��ֵ*/
}DB_TEXT_V;

#if 0
/* �ýṹ��ļ����Բ��ã�������*/
typedef struct
{
	uint16_t tbl;			/* ��Ҫ�����ı����Ƶ�����*/
	uint16_t key;			/* �ñ������е�Ψһ��������1��ʼ����*/
	uint16_t col;			/* ��id ����0��ʼ����*/
	int32_t value;		/* ��ֵ*/
}DB_INT_VAL;



typedef struct
{
	DB_COL_INFO ci;
	int32_t len;			/* len==0��ʾtextΪnull*/
	uint8_t text[0];		/* ��ֵ*/
}DB_TEXT_P;



typedef struct
{
	int32_t pri;
	int32_t len;			/* len==0��ʾtextΪnull*/
	uint8_t log[0];		/* ��ֵ*/
}DB_SYSLOG_P;


//typedef struct
//{
//	int32_t pri;
//	int32_t len;			/* len==0��ʾtextΪnull*/
//	uint8_t log[DBS_COL_MAX_LEN];		/* ��ֵ*/
//}DB_SYSLOG_V;

/* �������ݿ���ĳһ�е�������ʱ��Ҫ�õ��ýṹ��*/
typedef struct
{
	uint16_t tbl;			/* ��Ҫ�����ı����Ƶ�����*/
	uint16_t key;			/* �ñ������е�Ψһ��������1��ʼ����*/
	int32_t col_mask;		/* ��Ҫ�������е����룬�����Ҫ������λ������1 */
	int32_t len;			/* bufʵ�����ݳ���*/
	uint8_t buf[0];		/* �����д�����������Ҫ����������*/
}DB_COL_VAL;

/* ��ѯ���ݱ�һ���ж��������ݣ���ѯʱ�������ݱ��������
** ������Ӧ���ĵ�count�ֶ�Я������*/
typedef struct
{
	uint16_t tbl;			/* ��Ҫ�����ı����Ƶ�����*/
	uint32_t count;		/* ���ص�������*/
}DB_COUNT_VAL;

/* ��ѯ��ˮ���һ�����ݻ�������ˮ�����һ������ʱ��Ҫ�ýṹ��
** ϵͳĿǰʹ�õ���ˮ�����������־��͸澯��־��*/
typedef struct
{
	uint16_t tbl;			/* ��Ҫ��������ˮ�����Ƶ�����*/
	uint32_t row;			/* ��Ҫ�������кţ��ڲ�ѯʱ�����壬��0��ʼ����*/
	int32_t len;			/* bufʵ�����ݳ���*/
	uint8_t buf[0];		/* �����д�����������Ҫ���������ݣ���stAlarmInfo,syslog_r*/
}DB_WATER_TBL_COL_VAL;

typedef struct
{
	uint16_t index;
	uint8_t name[64];
	uint16_t col_num;
}DB_TBL_DESC;

typedef struct
{
	uint16_t col_type;
	uint16_t col_len;
	uint8_t isPrimaryKey;
	uint8_t allowNull;
	uint8_t col_name[32];
}DB_TBL_DESIGN;

#endif

/************************* DBS �ⲿ��Ϣ�ӿ�[  ����] **************************/
/************************* DBS �ⲿ��Ϣ�ӿ�[  ����] **************************/
/************************* DBS �ⲿ��Ϣ�ӿ�[  ����] **************************/

/*===============================================================*/
/*                         DB_ACCESSģ���ⲿ��Ϣ�ӿڶ���[  ����]                              */
/*===============================================================*/

/*==============================================================*/
/*                              ͨ���ⲿ��Ϣ�ӿڶ���[��ʼ]                       */
/*==============================================================*/

/* �ⲿģ��������Ϣ����ͷ����*/
typedef struct
{
	uint8_t ucMsgAttrib;			/* ��ʶ����Ϣ��������Ӧ��*/
	uint32_t ulRequestID;			/* ������ϢID*/
	uint16_t usSrcMID;			/* Դģ��ID*/
	uint16_t usDstMID;			/* Ŀ��ģ��ID*/
	uint16_t usMsgType;			/* ��Ϣ���ͣ���ʾ�������Ϣ*/
	uint8_t ucFlag;				/* �Ƿ������Ϣ��Ƭ*/
	uint32_t ulBodyLength;		/* ��Ϣ�峤��*/
}
T_COM_MSG_HEADER_REQ;

/* �ⲿģ�������Ķ���*/
typedef struct
{
	T_COM_MSG_HEADER_REQ HEADER;
	uint8_t BUF[0];				/*�������Ϣ������*/
}
T_COM_MSG_PACKET_REQ;

/* ģ��Ӧ����ͷ����*/
typedef struct
{
	uint8_t ucMsgAttrib;			/* ��ʶ����Ϣ��������Ӧ��*/
	uint32_t ulRequestID;			/* ������ϢID*/
	uint16_t usSrcMID;			/*Դģ��ID*/
	uint16_t usDstMID;			/*Ŀ��ģ��ID*/
	uint16_t usMsgType;			/*��Ϣ���ͣ���ʾ�������Ϣ*/
	uint8_t ucFlag;				/*�Ƿ���ڸ����Ƭ*/
	uint16_t result;				/*����״̬*/
	uint32_t ulBodyLength;		/*��Ϣ�峤��*/
}
T_COM_MSG_HEADER_ACK;

/* ģ��Ӧ���Ķ���*/
typedef struct
{
	T_COM_MSG_HEADER_ACK HEADER;
	uint8_t BUF[0];				/*���ص���Ϣ������*/
}
T_COM_MSG_PACKET_ACK;

typedef struct
{
	uint32_t clt;	/* CLT�������ţ���1��ʼ����*/
	uint32_t cnu;	/* CNU�������ţ���1��ʼ����*/
	uint8_t flag;	/* ��ʶPIB OR MOD CRC ����*/
	uint32_t crc;
}
stAutoGenCrc;

typedef struct
{
	uint32_t clt;	/* CLT�������ţ���1��ʼ����*/
	uint32_t cnu;	/* CNU�������ţ���1��ʼ����*/
	uint8_t flag;	/* ��ʶPIB OR MOD */
}
stAutoGenConfig;

typedef struct
{
	uint32_t clt;	/* CLT�������ţ���1��ʼ����*/
	uint32_t cnu;	/* CNU�������ţ���1��ʼ����*/
	uint8_t status;
}
stUserAutoConfigInfo;

#if 0
typedef struct
{
	uint32_t tid;
	T_szTemplate szTm;
}
stWrTemplateInfo;
#endif

typedef struct
{
	uint32_t clt;	/* CLT�������ţ���1��ʼ����*/
	uint32_t cnu;	/* CNU�������ţ���1��ʼ����*/
}
stTmUserInfo;

typedef struct
{
	stTmUserInfo node;
	st_rtl8306eSettings rtl8306eConfig;
}
rtl8306eWriteInfo;

typedef struct
{
	uint32_t clt;	/* CLT�������ţ���1��ʼ����*/
	uint32_t cnu;	/* CNU�������ţ���1��ʼ����*/
	uint32_t event;
}
stRegEvent;

typedef struct
{
	uint32_t portid;
	uint32_t dbNum;
	uint8_t mac[6];
}
stDsdtMacBinding;

#if 0
typedef struct
{
	uint32_t clt;	/* CLT�������ţ���1��ʼ����*/
	uint32_t cnu;	/* CNU�������ţ���1��ʼ����*/
	T_szTemplate szTm;
}
stTmUserConfigInfo;
#endif

typedef struct
{
	uint32_t clt;	/* CLT�������ţ���1��ʼ����*/
	uint32_t cnu;	/* CNU�������ţ���1��ʼ����*/
	uint32_t tid;
}
stTmNewUserInfo;

#pragma pack (pop)
/*===============================================================*/
/*                         ͨ���ⲿ��Ϣ�ӿڶ���[����]                              */
/*===============================================================*/


#endif

