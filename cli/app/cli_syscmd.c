/****************************************************************************
 文件名  : CLI_Syscmd.c
 作者    : 
 版本    :
 完成日期:
 文件描述: 注册命令行模块与系统信息的一些基本管理命令，
 			    在命令行模块的初始化
           过程中被引用
 修改历史:
        1. 修改者   :
           时间     :
           版本     :
           修改原因 :
 ****************************************************************************/

#include "../cli_comm.h"
#include "../util/cli_term.h"
#include "../util/cli_io.h"
#include "../util/cli_interpret.h"
#include "cli_cmd.h"
#include "cli_syscmd.h"
#include "cli_cmm.h"
#include <public.h>
#include <time.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <dbsapi.h>
#include <boardapi.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 与DBS  通讯的设备文件*/
T_DBS_DEV_INFO *dbsdev = NULL;

extern ST_CLI_MODE m_sCliSysModes[MAX_MODE_NUM];    /* 命令树 */

/* 命令行模块的命令信息库结构定义 */
ST_CLI_CMD_REG m_CliCmdPool[] =
{
	/*命令串                   对象1帮助信息         对象2帮助信息    对象3帮助信息 */
	{"cls",                   CMDHELP_GLB_CLS,      CLI_ML_NULL,       CLI_ML_NULL,
	/* 执行函数指针            命令等级              命令模式    */
	CLI_CmdClearScreen,      CLI_AL_QUERY,          CTM_GLOBAL },

	/* 退出命令行系统 */
	{"Exit",                  CMDHELP_GLB_EXIT,    CLI_ML_NULL,       CLI_ML_NULL,
	CLI_CmdExit,             CLI_AL_QUERY,          CTM_GLOBAL},

	/* 自动/手动滚屏的开关设置命令 */
	{CLI_CMD_FORMAT_SCROLL,  CMDHELP_GLB_SET_SCROLL,    CLI_ML_NULL,  CLI_ML_NULL,
	CLI_CmdSetScroll,      CLI_AL_QUERY,          CTM_GLOBAL },

	/* 显示命令树 */
	{"help",     CMDHELP_GLB_HELP,     CLI_ML_NULL, CLI_ML_NULL,
	CLI_CmdHelp,      CLI_AL_QUERY,          CTM_GLOBAL },

	/* 修改用户密码*/
	{CLI_CMD_FORMAT_SETPASSWORD,      CMDHELP_GLB_SETPASSWORD, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_SetCliPwd,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* debug */
	{CLI_CMD_FORMAT_DEBUG,      CMDHELP_GLB_DEBUG, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_Debug,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* display opt-log */
	{CLI_CMD_FORMAT_DIS_OPT,	CMDHELP_GLB_DISPLAY, CMDHELP_GLB_DIS_OPT,    CLI_ML_NULL,
	CLI_Cmd_Show_Optlog,      CLI_AL_QUERY,          CTM_GLOBAL },

	/* display alarm-log */
	{CLI_CMD_FORMAT_DIS_ALARM,	CMDHELP_GLB_DISPLAY, CMDHELP_GLB_DIS_ALARM,    CLI_ML_NULL,
	CLI_Cmd_Show_alarmlog,      CLI_AL_QUERY,          CTM_GLOBAL },

	/* display sys-log */
	{CLI_CMD_FORMAT_DIS_SYSLOG,	CMDHELP_GLB_DISPLAY, CMDHELP_GLB_DIS_SYSLOG,    CLI_ML_NULL,
	CLI_Cmd_ShowSyslog,      CLI_AL_QUERY,          CTM_GLOBAL },

	/* display topology */
	{CLI_CMD_FORMAT_DIS_TOP,	CMDHELP_GLB_DISPLAY, CMDHELP_GLB_DIS_TOP,    CLI_ML_NULL,
	CLI_Cmd_ShowTopology,      CLI_AL_QUERY,          CTM_GLOBAL },

	/* display user-white-list */
	{CLI_CMD_FORMAT_DIS_USERS,	CMDHELP_GLB_DISPLAY, CMDHELP_GLB_DIS_USERS,    CLI_ML_NULL,
	CLI_Cmd_ShowWlistUsers,      CLI_AL_QUERY,          CTM_GLOBAL },

	/* display network-info */
	{CLI_CMD_FORMAT_DIS_NETWORK,	CMDHELP_GLB_DISPLAY, CMDHELP_GLB_DIS_NETWORK,    CLI_ML_NULL,
	CLI_Cmd_ShowNetworkInfo,      CLI_AL_QUERY,          CTM_GLOBAL },

	/* display sysinfo */
	{CLI_CMD_FORMAT_DIS_SYSINFO,	CMDHELP_GLB_DISPLAY, CMDHELP_GLB_DIS_SYSINFO,    CLI_ML_NULL,
	CLI_Cmd_ShowSysInfo,      CLI_AL_QUERY,          CTM_GLOBAL },

	/* display snmp */
	{CLI_CMD_FORMAT_DIS_SNMP,	CMDHELP_GLB_DISPLAY, CMDHELP_GLB_DIS_SNMP,    CLI_ML_NULL,
	CLI_Cmd_ShowSnmpInfo,      CLI_AL_QUERY,          CTM_GLOBAL },

	/* display ftp-server*/
	{CLI_CMD_FORMAT_DIS_FTP,	CMDHELP_GLB_DISPLAY, CMDHELP_GLB_DIS_FTP,    CLI_ML_NULL,
	CLI_Cmd_ShowFtpServer,      CLI_AL_QUERY,          CTM_GLOBAL },

	/* display cur-profile *//* 该命令在CLT / CNU 模式下可执行*/
	{CLI_CMD_FORMAT_DISPLAY_CUR_PROFILE,  CMDHELP_GLB_DISPLAY, CMDHELP_GLB_DISPLAY_CUR_PROFILE,    CLI_ML_NULL,
	CLI_Cmd_ShowCurProfile,      CLI_AL_QUERY,          CTM_GLOBAL },

	/* 重启设备*//* 该命令在GLOABLE / CLT / CNU 模式下分别执行不同的动作*/
	{CLI_CMD_FORMAT_REBOOT,     CMDHELP_GLB_REBOOT,     CLI_ML_NULL, CLI_ML_NULL,
	CLI_Cmd_Reboot,      CLI_AL_OPERATOR,          CTM_GLOBAL },

	/* snmp set */
	{CLI_CMD_FORMAT_STE_SNMP,      CMDHELP_GLB_SET, CMDHELP_GLB_SET_SNMP_CONF,    CLI_ML_NULL,
	CLI_Cmd_SnmpConfig,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* save-config */
	{CLI_CMD_FORMAT_SAVE,      CMDHELP_GLB_SAVE, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_SaveConfig,      CLI_AL_OPERATOR,          CTM_GLOBAL },

	/* restore-default */
	{CLI_CMD_FORMAT_RESTORE,      CMDHELP_GLB_RESTORE, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_RestoreDefault,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* 配置管理局端IP */
	{CLI_CMD_FORMAT_MIP,      CMDHELP_GLB_SETIP, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_SetIpAddress,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* undo ip-address */
	{CLI_CMD_FORMAT_UNDO_ADDRESS,      CMDHELP_GLB_UNDO, CMDHELP_GLB_UNDOADDRESS,    CLI_ML_NULL,
	CLI_Cmd_UndoIpAddress,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* 配置管理局端网关*/
	{CLI_CMD_FORMAT_MIPG,      CMDHELP_GLB_SETIPG, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_SetIpGateway,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* undo ip-gateway */
	{CLI_CMD_FORMAT_UNDO_GATEWAY,      CMDHELP_GLB_UNDO, CMDHELP_GLB_UNDOGATEWAY,    CLI_ML_NULL,
	CLI_Cmd_UndoIpGateway,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* 配置管理VLAN */
	{CLI_CMD_FORMAT_SETMVLAN,      CMDHELP_GLB_MANAGEMENT_VLAN, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_SetMgmtVlan,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* undo mgmt-vlan*/
	{CLI_CMD_FORMAT_UNDO_VLAN,      CMDHELP_GLB_UNDO, CMDHELP_GLB_UNDOVALN,    CLI_ML_NULL,
	CLI_Cmd_UndoMgmtVlan,      CLI_AL_ADMIN,          CTM_GLOBAL },	

	/* set ftp-server */
	{CLI_CMD_FORMAT_FTP_SERVER, CMDHELP_GLB_SET     , CMDHELP_GLB_SET_FTP_SERVER,    CLI_ML_NULL,
	CLI_Cmd_SetFtpServer,      CLI_AL_OPERATOR,          CTM_GLOBAL },	

	/* vlan *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_VLAN,      CMDHELP_GLB_VLAN_CONFIG, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_DoCnuVlanConfig,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* undo vlan *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_UNDO_CNU_VLAN,      CMDHELP_GLB_UNDO, CMDHELP_GLB_UNDO_CNU_VLAN,    CLI_ML_NULL,
	CLI_Cmd_UndoCnuVlanConfig,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* rate-limit *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_RATELIMIT,      CMDHELP_GLB_FLOWCONTROL, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_DoRateLimiting,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* undo rate-limit *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_UNDO_CNU_RATE_LIMIT,      CMDHELP_GLB_UNDO, CMDHELP_GLB_UNDO_RATE_LIMIT,    CLI_ML_NULL,
	CLI_Cmd_UndoRateLimiting,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* storm-filter *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_STORM_FILTER,      CMDHELP_GLB_STROM_FILTER, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_DoStromFilter,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* undo storm-filter *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_UNDO_STORM_FILTER,      CMDHELP_GLB_UNDO, CMDHELP_GLB_UNDO_STORM_FILTER,    CLI_ML_NULL,
	CLI_Cmd_UndoStromFilter,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* shutdown *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_SHUTDOWN,      CMDHELP_GLB_SHUTDOWN, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_Shutdown,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* undo shutdown *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_UNDO_SHUTDOWN,      CMDHELP_GLB_UNDO, CMDHELP_GLB_UNDO_SHUTDOWN,    CLI_ML_NULL,
	CLI_Cmd_UndoShutdown,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* mac-limit *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_MAC_LIMIT,      CMDHELP_GLB_SET, CMDHELP_GLB_SET_MAC_LIMIT,    CLI_ML_NULL,
	CLI_Cmd_MacLimit,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* undo mac-limit *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_UNDO_MAC_LIMIT,      CMDHELP_GLB_UNDO, CMDHELP_GLB_UNDO_MAC_LIMIT,    CLI_ML_NULL,
	CLI_Cmd_UndoMacLimit,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* aging-time *//* 该命令仅在CLT 模式下可执行*/
	{CLI_CMD_FORMAT_AGING_TIME,      CMDHELP_GLB_SET, CMDHELP_GLB_SET_AGING_TIME,    CLI_ML_NULL,
	CLI_Cmd_DoAgingTimeConfig,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* undo aging-time *//* 该命令仅在CLT 模式下可执行*/
	{CLI_CMD_FORMAT_UNDO_AGING_TIME,      CMDHELP_GLB_UNDO, CMDHELP_GLB_UNDO_AGING_TIME,    CLI_ML_NULL,
	CLI_Cmd_UndoAgingTimeConfig,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* qos using *//* 该命令仅在CLT/CNU 模式下可执行*/
	{CLI_CMD_FORMAT_QOS_TYPE,      CMDHELP_GLB_QOS_USING, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_SelectQoSType,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* qos map *//* 该命令仅在CLT/CNU 模式下可执行*/
	{CLI_CMD_FORMAT_QOS_QUEUE_MAP,      CMDHELP_GLB_QOS_MAP, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_DoQosMap,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* undo qos *//* 该命令仅在CLT/CNU 模式下可执行*/
	{CLI_CMD_FORMAT_UNDO_QOS,      CMDHELP_GLB_UNDO, CMDHELP_GLB_UNDO_QOS,    CLI_ML_NULL,
	CLI_Cmd_UndoQos,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* Ar8236 SMI reg debug *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_AR8236_SMI_REG,      CMDHELP_GLB_AR8236_SMI_REG, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_CmdAr8236SmiReg,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* Ar8236 SMI phy debug *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_AR8236_SMI_PHY,      CMDHELP_GLB_AR8236_SMI_PHY, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_CmdAr8236SmiPhy,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* cnu switch debug *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_CNU_SWITCH,      CMDHELP_GLB_CNU_SWITCH, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_CmdCnuSwitch,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* cnu acl add *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_CNU_ACL,      CMDHELP_GLB_CNU_ACL, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_CmdCnuAclDropMme,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* undo acl drop mme */
	{CLI_CMD_FORMAT_UNDO_CNU_ACL,      CMDHELP_GLB_UNDO, CMDHELP_GLB_UNDO_CNU_ACL,    CLI_ML_NULL,
	CLI_CmdUndoCnuAclDropMme,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* MME mdio phy debug *//* 该命令仅在CLT或者CNU 模式下可执行*/
	{CLI_CMD_FORMAT_MME_MDIO,      CMDHELP_GLB_MME_MDIO, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_CmdMmeMdio,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* upgrade */
	{CLI_CMD_FORMAT_UPGRADE,      CMDHELP_GLB_UPGRADE, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_CmdUpgrade,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* reload profile *//* 该命令在CLT / CNU 模式下可执行*/
	{CLI_CMD_FORMAT_RELOAD_PROFILE,      CMDHELP_GLB_RELOAD_PROFILE, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_ReloadProfile,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* permit cnu */
	{CLI_CMD_FORMAT_USER_PERMIT,      CMDHELP_GLB_PERMIT_USER, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_DoCnuPermit,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* undo permit cnu */
	{CLI_CMD_FORMAT_UNDO_USER_PERMIT,      CMDHELP_GLB_UNDO, CMDHELP_GLB_UNDO_PERMIT,    CLI_ML_NULL,
	CLI_Cmd_UndoCnuPermit,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* delete cnu */
	{CLI_CMD_FORMAT_DELETE_CNU,      CMDHELP_GLB_DELETE_CNU, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_DeleteCnu,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* 白名单功能开关*/
	{CLI_CMD_FORMAT_WLIST_CONTROL,      CMDHELP_GLB_WLIST_CTL, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_DoWlistControl,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* create cnu */
	{CLI_CMD_FORMAT_CREATE_CNU,      CMDHELP_GLB_CREATE_CNU, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_CreateCnu,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* WDT 功能开关*/
	{CLI_CMD_FORMAT_WDT_CONTROL,      CMDHELP_GLB_WDT_CTL, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_DoWdtControl,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* HEARTBEAT 功能开关*/
	{CLI_CMD_FORMAT_HB_CONTROL,      CMDHELP_GLB_HB_CTL, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_DoHBControl,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* MV88E6171R Switch statistics */
	{CLI_CMD_FORMAT_DSDT_DBG_STAS,      CMDHELP_GLB_DSDT_DBG, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_DoDsdtPortStats,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* MV88E6171R RGMII Tining delay */
	{CLI_CMD_FORMAT_DSDT_TIMING_DELAY,      CMDHELP_GLB_DSDT_TIMING_DELAY, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_DoDsdtRgmiiTimingDelay,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* MV88E6171R port mirroring */
	{CLI_CMD_FORMAT_DSDT_PORT_MIRROR,      CMDHELP_GLB_DSDT_PORT_MIRROR, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_DoDsdtPortMirror,      CLI_AL_ADMIN,          CTM_GLOBAL },

	/* MV88E6171R mac binding */
	{CLI_CMD_FORMAT_DSDT_MAC_BINDING,      CMDHELP_GLB_DSDT_MAC_BIND, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_DoDsdtMacBinding,      CLI_AL_ADMIN,          CTM_GLOBAL },
	
	/* dump *//* 该命令仅在CNU 模式下可执行*/
	{CLI_CMD_FORMAT_DEBUG_DUMP,      CMDHELP_GLB_DEBUG_DUMP, CLI_ML_NULL,    CLI_ML_NULL,
	CLI_Cmd_Dump,      CLI_AL_ADMIN,          CTM_GLOBAL }

};

/********************************************************************************************
*	函数名称:__isIpv4Valid
*	函数功能:判断IPV4 网络地址是否合法的工具函数
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int __isIpv4Valid(uint8_t * var_val, uint32_t var_val_len)
{
	unsigned int n1,n2,n3,n4;
	char ipV4Address[32] = {0};

	if( NULL == var_val )
	{
		return 0;
	}
	if( var_val_len >= 32 )
	{
		return 0;
	}
	
	strcpy(ipV4Address, var_val);
	ipV4Address[var_val_len] = '\0';

	if(sscanf(ipV4Address, "%u.%u.%u.%u", &n1, &n2, &n3, &n4) != 4) 
	{
		return 0;
	}
	if( (n1 != 0) && (n1 < 255) && (n2 <= 255) && (n3 <= 255) && (n4 <= 255) ) 
	{
		char buf[32];
		sprintf(buf, "%u.%u.%u.%u", n1, n2, n3, n4);
		if(strcmp(buf, ipV4Address))
		{
			return 0;
		}
		return 1;
	}
	return 0;	
}

/********************************************************************************************
*	函数名称:__isIpv4NetmaskValid
*	函数功能:判断IPV4 网络地址子网掩码是否合法的工具函数
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int __isIpv4NetmaskValid(uint8_t * var_val, uint32_t var_val_len)
{
	int i = 0;
	int j = 0;
	int b = 0;
	uint32_t c[4] = {0x00};
	char ipV4Address[32] = {0};

	if( NULL == var_val )
	{
		return 0;
	}
	if( var_val_len >= 32 )
	{
		return 0;
	}

	strcpy(ipV4Address, var_val);
	ipV4Address[var_val_len] = '\0';

	if(sscanf(ipV4Address, "%u.%u.%u.%u", &c[3], &c[2], &c[1], &c[0]) != 4) 
	{
		return 0;
	}
	
	/* 判断子网掩码是否有效*/
	/* 找出最后一个bit(1)，若其之前的所有位都为1，则为有效的掩码*/
	for( i=0;i<4;i++ )
	{
		for( j=0;j<8;j++)
		{
			if( 0 == b )
			{
				/* find the first bit '1' */
				if( 0 != ( c[i] & (1<<j) ) )
				{
					b = 1;
					continue;
				}
			}
			else if( 0 == ( c[i] & (1<<j) ) )
			{
				return 0;
			}		
		}
	}	
	return 1;	
}

/********************************************************************************************
*	函数名称:isIpv4ValidBymask
*	函数功能:判断本地IPV4 网络地址是否合法的工具函数
*	注意:需要排除广播地址以及网络地址
*	作者:frank
*	时间:2010-08-19
*********************************************************************************************/
int isIpv4ValidByMask(uint8_t * sip, uint32_t sip_len, uint8_t * smask, uint32_t smask_len)
{
	uint32_t I_ip = 0;
	uint32_t I_mask = 0;
	
	if( !__isIpv4Valid(sip, sip_len) )
	{
		return BOOL_FALSE;
	}
	else if( !__isIpv4NetmaskValid(smask, smask_len) )
	{
		return BOOL_FALSE;
	}
	else
	{
		I_ip = inet_addr(sip);
		I_mask = inet_addr(smask);
		/*  广播地址不是合法的本地IP */
		if( (~I_mask) == (I_ip & (~I_mask)) )
		{
			return BOOL_FALSE;
		}
		/*  网络地址不是合法的本地IP */
		else if( 0 == (I_ip & (~I_mask)) )
		{
			return BOOL_FALSE;
		}
		else
		{
			return BOOL_TRUE;
		}
	}
}

int __clt_opt_log(uint16_t cmd, uint32_t result)
{
	time_t b_time;
	st_dbsOptlog log;

	/* 获取系统当前时间*/
	time(&b_time);

	log.who = MID_CLI;
	log.time = b_time;
	log.level = DBS_LOG_INFO;
	log.result = result;

	switch(cmd)
	{
		case CMM_CLI_SET_USERPASSWORD:
		{
			strcpy(log.cmd, "CLI_SET_CLI_PASSWORD");
			break;
		}
		case CMM_TOPOLOGY_SHOW:
		{
			strcpy(log.cmd, "CLI_DISPLAY_TOPOLOGY");
			break;
		}
		case CMM_CLI_GET_USERS:
		{
			strcpy(log.cmd, "CLI_DISPLAY_WHITE_LIST_USERS");
			break;
		}
		case CMM_OPT_LOG_SHOW:
		{
			strcpy(log.cmd, "CLI_DISPLAY_OPT_LOG");
			break;
		}
		case CMM_ALARM_LOG_SHOW:
		{
			strcpy(log.cmd, "CLI_DISPLAY_ALARM_LOG");
			break;
		}
		case CMM_SYSLOG_SHOW:
		{
			strcpy(log.cmd, "CLI_DISPLAY_SYS_LOG");
			break;
		}
		case CMM_NETWORK_INFO_SHOW:
		{
			strcpy(log.cmd, "CLI_DISPLAY_NETWORK_INFO");
			break;
		}
		case CMM_CLI_GET_SYSINFO:
		{
			strcpy(log.cmd, "CLI_DISPLAY_SYSTEM_INFO");
			break;
		}
		case CMM_CLI_GET_CUR_PROFILE:
		{
			strcpy(log.cmd, "CLI_DISPLAY_CUR_PROFILE");
			break;
		}
		case CMM_READ_SNMP_CONF:
		{
			strcpy(log.cmd, "CLI_DISPLAY_SNMP_INFO");
			break;
		}
		case CMM_CLI_GET_FTP:
		{
			strcpy(log.cmd, "CLI_DISPLAY_FTP_SERVER_INFO");
			break;
		}
		case CMM_WRITE_SNMP_CONF:
		{
			strcpy(log.cmd, "CLI_WRITE_SNMP_SETTINGS");
			break;
		}
		case CMM_SAVE_CONFIG:
		{
			strcpy(log.cmd, "CLI_SAVE_CONFIG");
			break;
		}
		case CMM_CLI_SET_IP:
		{
			strcpy(log.cmd, "CLI_DO_IP_CONFIG");
			break;
		}
		case CMM_CLI_UNDO_IPADDR:
		{
			strcpy(log.cmd, "CLI_UNDO_IP_ADDRESS");
			break;
		}
		case CMM_CLI_SET_IPGATEWAY:
		{
			strcpy(log.cmd, "CLI_DO_GATEWAY_CONFIG");
			break;
		}
		case CMM_CLI_UNDO_IP_GW:
		{
			strcpy(log.cmd, "CLI_UNDO_IP_GATEWAY");
			break;
		}
		case CMM_CLI_SET_MVLAN:
		{
			strcpy(log.cmd, "CLI_DO_MGMT_VLAN_CONFIG");
			break;
		}
		case CMM_CLI_UNDO_MVLAN:
		{
			strcpy(log.cmd, "CLI_UNDO_MGMT_VLAN");
			break;
		}
		case CMM_CLI_DO_AGING_TIME_CONFIG:
		{
			strcpy(log.cmd, "CLI_DO_AGING_TIME_CONFIG");
			break;
		}
		case CMM_CLI_SET_FTPSERVER:
		{
			strcpy(log.cmd, "CLI_SET_FTP_SERVER");
			break;
		}
		case CMM_CLI_DO_QOS_CONFIG:
		{
			strcpy(log.cmd, "CMM_CLI_DO_QOS_CONFIG");
			break;
		}
		default:
		{
			strcpy(log.cmd, "CLI_UNKNOWN_CMD");
			break;
		}
	}	
	return dbs_opt_log(dbsdev, &log);
}

/*********************************************************************/
/* 函数名称 : CLI_SysCmdReg()                                           */
/* 函数功能 : 命令行模块的命令注册                                   */
/* 输入参数 : 无                                                     */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/* 上层函数 : 命令行模块初始化函数                                   */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_SysCmdReg(_VOID   )
{
    ULONG  i;
    ULONG  ulRet = 0;
    for (i = 0; i < CLI_ARRAY_SIZE(m_CliCmdPool); i++)
    {
        ulRet += CLI_CmdRegist(m_CliCmdPool[i].szCmd,     //格式化串
                                  m_CliCmdPool[i].ucMode,    //命令所属模式
                                  (CLI_OPERAT_LEVEL_T)m_CliCmdPool[i].ucLevel,   //命令使用等级
                                  m_CliCmdPool[i].pFunc,     //命令执行函数
                                  m_CliCmdPool[i].strHelp1,   //第一个对象的帮助信息
                                  m_CliCmdPool[i].strHelp2,
                                  m_CliCmdPool[i].strHelp3    );

    }
    if (ulRet != 0)
    {
        STB_Printf("\n Some system command register failed !!");
        return TBS_FAILED;
    }
    return TBS_SUCCESS;
}

#if 0
/*********************************************************************/
/* 函数功能 : 从任何模式进入调试模式的命令实现   */
/*********************************************************************/
ULONG  CLI_EnterDiagMode()
{
    PST_CLI_WS pWS;
    char  *pParamNO;

    pWS = &m_stCliWS;

    IO_Print("Enter Diagnose Mode.");
    CLI_DELAY(50);
    if (pWS->ulCurrentMode == CTM_GENL)
        return TBS_SUCCESS;

    /* Write to pWS       */
    if(CLI_CmdPush(&(pWS->cmdStack), CTM_DIAG) != TBS_SUCCESS )
    {
        IO_Print(CLI_PUSH_CMD_FAILED);
        return TBS_FAILED;
    }
    pWS->ulCurrentMode = CTM_DIAG;
    pWS->ulParamIndex += m_sCliSysModes[CTM_DIAG].ulParamCount;

    pParamNO = CLI_GetModeParameter(CTM_DIAG, 2) ;
    CLI_EnterMode(CTM_DIAG, pParamNO) ;

    return TBS_SUCCESS;
}

#endif


/*※※※※※※※※※※※※※※*/
/* 命令行的的命令实现函数     */
/*※※※※※※※※※※※※※※*/
/*********************************************************************/
/* 函数功能 : 屏幕清除功能命令实现                                   */
/*********************************************************************/
ULONG  CLI_CmdClearScreen()
{
    ST_CLI_TERM* lpTermStruc;
    int  iSavePos;
    ULONG  ulLine;

    if ( (lpTermStruc = CLI_GetTermData()) == NULL )
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    for (ulLine = 0; ulLine <= LINE_PER_PAGE + 1; ulLine++)
    {
        IO_OutString("\r\n");
        lpTermStruc->ulDispLineCount = 0;
    }

    iSavePos = lpTermStruc->iCurrentPos;
    lpTermStruc->iCurrentPos = 0;
    CLI_MoveToCurrentPos(80*24);
    CLI_SendToTerm();
    lpTermStruc->iCurrentPos = iSavePos;

    IO_Print("\33[2J");
    return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数功能 : CLI_CmdExit                       */
/*********************************************************************/
ULONG  CLI_CmdExit()
{
    ST_CLI_TERM* lpTermStruc;

    if (IO_GetConfirm(CLI_LOGOUT_CONFIRM, INPUT_NO) != INPUT_YES)
        return CLI_CMD_FAILED_SILENT;

    if ( (lpTermStruc = CLI_GetTermData()) != NULL )
    {
        CLI_TermLogout();
    }

    return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数功能 : 设置手动滚屏的命令实现                                 */
/*********************************************************************/
ULONG  CLI_CmdSetScroll()
{
    ST_CLI_TERM* lpTermStruct;
    char  *pParam;

    if ( (lpTermStruct = CLI_GetTermData()) == NULL )
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    /* 透传命令的处理任务不支持滚屏方式的修改 */
    if (lpTermStruct->nTermType == CLI_TERMT_FOR_MML)
        return TBS_SUCCESS;

    if ((pParam = CLI_GetParamByName("automatic|manual")) == NULL)
    {
        MT_ERRLOG(0);
        return TBS_FAILED;
    }

    if (!STB_StriCmp(pParam, "automatic"))
    {
        lpTermStruct->ucScroll = SCROLL_AUTO;
        IO_Print(CLI_SCROLL_DISABLE);
        return TBS_SUCCESS;
    }
    else if (!STB_StriCmp(pParam, "manual"))
    {
        lpTermStruct->ucScroll = SCROLL_MANUAL;
        IO_Print(CLI_SCROLL_ENABLE);
        return TBS_SUCCESS;
    }

    return TBS_FAILED;
}


/*********************************************************************/
/* 函数功能 : 判断对象是否需要在help中显示                           */
/*********************************************************************/
BOOL __CLI_IsObjNeedShow(ULONG  ulMode, PST_CLI_CMDKEY pCmdKey, ULONG  ulLevel)
{
    PST_CLI_CMDKEY pObj;

    pObj = pCmdKey;
    if (pObj == NULL)
        return FALSE;

    if (pObj->action.rightLevel > (CLI_OPERAT_LEVEL_T)ulLevel)
        return FALSE;

    //如果即不是当前模式，又不是全局模式的对象，不予显示
    if (ulMode != pObj->action.ulMode && CTM_GLOBAL != pObj->action.ulMode)
    {
        return FALSE;
    }

    if (ulMode == pObj->action.ulMode)
        return TRUE;
    pObj = pObj->pSubCmdKey;

    while (pObj != NULL)
    {
        if (pObj->action.ulMode == ulMode
         && pObj->action.rightLevel <= (CLI_OPERAT_LEVEL_T)ulLevel)
            return TRUE;

        if (pObj->pSubCmdKey != 0)
            pObj = pObj->pSubCmdKey;
        else if (pObj->pNext != 0)
            pObj = pObj->pNext;
        else
        {
            if (pObj->pParent == pCmdKey)
                break;

            while (pObj->pParent->pNext == NULL)
            {
                pObj = pObj->pParent;
                if (pObj->pParent == pCmdKey)
                    return FALSE;
            }
            pObj = pObj->pParent->pNext;
        }
    }

    return FALSE;
}

/*********************************************************************/
/* 函数功能 : 在help命令中显示指定的对象信息                         */
/*********************************************************************/
ULONG  __CLI_ObjectShow(ULONG  ulMode, ULONG  ulLevel, PST_CLI_CMDKEY pCmdKey)
{
    ULONG  ulBlank[5] = {0, 0, 0, 0, 0};
    ULONG  ulBlankTotal = 0, i, j;
    ULONG  ulObjNum = 0;
    PST_CLI_CMDKEY pObj, pObjTmp;
    char  szInfo[256];
    char  szHelp[128];
    ULONG  ulPos = 0;
    char  *szOutput;
    ULONG  ulRet = TBS_SUCCESS;


    if (!__CLI_IsObjNeedShow(ulMode, pCmdKey, ulLevel))
    {
        return TBS_FAILED;
    }

    if ((szOutput = CLI_Malloc(4096))
         == NULL)
         return TBS_FAILED;

    szOutput[0] = '\0';
    szInfo[0] = '\0';

    pObj = pCmdKey;

    while (pObj != NULL)
    {
        if (pObj->action.rightLevel <= (CLI_OPERAT_LEVEL_T)ulLevel
            && (pObj->action.ulMode == ulMode || pObj->action.ulMode == CTM_GLOBAL)
            && (__CLI_IsObjNeedShow(ulMode, pObj, ulLevel)))
        {
            if (pObj == pCmdKey
             || (pObj->pPrevious == NULL && pObj->pNext == NULL))
            {
                STB_StrCat(szOutput, "  ");
            }
            else if (pObj->pPrevious == NULL)
            {
                STB_StrCat(szOutput, "+-");
            }
            else
            {
                i = 0, j = 0;
                //计算该对象的兄长对象
                pObjTmp = pObj->pPrevious;
                while (pObjTmp != NULL)
                {
                    if (__CLI_IsObjNeedShow(ulMode, pObjTmp, ulLevel))
                        i++;
                    pObjTmp = pObjTmp->pPrevious;
                }

                //计算该对象的弟弟对象
                pObjTmp = pObj->pNext;
                while (pObjTmp != NULL)
                {
                    if (__CLI_IsObjNeedShow(ulMode, pObjTmp, ulLevel))
                        j++;
                    pObjTmp = pObjTmp->pNext;
                }
                if ((i + j) == 0) //如果前后没有可显示的对象
                    STB_StrCat(szOutput, "  ");
                else if (i == 0) //如果该对象前的可显示对象个数为0，则该对象为第一个
                    STB_StrCat(szOutput, "+-");
                else
                    STB_StrCat(szOutput, "|-");
            }
            ulPos += 2;

            STB_Sprintf(szInfo, "%-7s", pObj->szName);
            STB_StrCat(szOutput, szInfo);
            ulPos += STB_StrLen(szInfo);

            if (!CLI_IsObjectExecutable(pObj, HAVEACTIONS))
            {
//                CLI_DELAY(1);
                CLI_GetObjectHelpString(pObj->action.strObjHelp, szHelp);

                if (ulPos <  38)
                {
                    for (i = 0; i < 38 - ulPos; i++)
                        STB_StrCat(szOutput, " ");
                }

                if (STB_StrLen(szHelp) > 40)
                {
                    ulBlankTotal = 0;
                    for (i = 0; i < 37; i++)
                    {
                        if ((UCHAR)szHelp[i] > 127)
                            ulBlankTotal ++;
                    }
                    if (ulBlankTotal % 2 == 0)
                        STB_StrCpy(szHelp + 37, "...");
                    else
                        STB_StrCpy(szHelp + 38, "...");
                }
                STB_StrCat(szOutput, szHelp);
            }
        }
        else if (pObj->action.ulMode != ulMode && ulMode == CTM_GLOBAL)
        {
            ulRet = 2;
        }

        if (pObj->pSubCmdKey != NULL)
        {
            ulBlank[ulObjNum++] = STB_StrLen(szInfo) + 2;
            pObj = pObj->pSubCmdKey;
        }
        else if (pObj == pCmdKey)
        {
            break;
        }
        else if (pObj->pNext != NULL)
        {
            if (pObj->action.rightLevel <= (CLI_OPERAT_LEVEL_T)ulLevel
                && (pObj->action.ulMode == ulMode || pObj->action.ulMode == CTM_GLOBAL)
                && (__CLI_IsObjNeedShow(ulMode, pObj, ulLevel)))
            {
                STB_StrCat(szOutput, "\r\n");
                ulPos = 0;
            }
            else if (pObj->action.ulMode != ulMode && ulMode == CTM_GLOBAL)
            {
                ulRet = 2;
            }

            ulBlankTotal = 0;
            for (i = 0; i < ulObjNum; i++)
            {
                ulBlankTotal += ulBlank[i];
            }
            for (i = 0; i < ulBlankTotal; i++)
            {

                if (pObj->action.rightLevel <= (CLI_OPERAT_LEVEL_T)ulLevel
                    && (pObj->action.ulMode == ulMode || pObj->action.ulMode == CTM_GLOBAL)
                    && (__CLI_IsObjNeedShow(ulMode, pObj, ulLevel)))
                {
                    if (ulObjNum == 2 && i == ulBlank[0] && pObj->pParent->pNext != NULL)
                        STB_StrCat(szOutput, "|");
                    else
                        STB_StrCat(szOutput, " ");
                    ulPos++;
                }
                else if (pObj->action.ulMode != ulMode && ulMode == CTM_GLOBAL)
                {
                    ulRet = 2;
                }
            }
            pObj = pObj->pNext;
        }
        else
        {
            while (pObj->pParent != pCmdKey
                && pObj->pParent->pNext == NULL)
            {
                pObj = pObj->pParent;
                --ulObjNum;

            }
            if (pObj->pParent == pCmdKey)
            {
                break;
            }

            if (pObj->pParent->pNext != NULL)
            {
                --ulObjNum;

                if (pObj->action.rightLevel <= (CLI_OPERAT_LEVEL_T)ulLevel
                    && (pObj->action.ulMode == ulMode || pObj->action.ulMode == CTM_GLOBAL)
                    && (__CLI_IsObjNeedShow(ulMode, pObj, ulLevel)))
                {
                    STB_StrCat(szOutput, "\r\n");
                    ulPos = 0;
                }
                else if (pObj->action.ulMode != ulMode && ulMode == CTM_GLOBAL)
                {
                    ulRet = 2;
                }
                ulBlankTotal = 0;

                for (i = 0; i < ulObjNum; i++)
                {
                    ulBlankTotal += ulBlank[i];
                }
                for (i = 0; i < ulBlankTotal; i++)
                {
                    if (pObj->action.rightLevel <= (CLI_OPERAT_LEVEL_T)ulLevel
                        && (pObj->action.ulMode == ulMode || pObj->action.ulMode == CTM_GLOBAL)
                        && (__CLI_IsObjNeedShow(ulMode, pObj, ulLevel)))
                    {
                        STB_StrCat(szOutput, " ");
                        ulPos++;
                    }
                    else if (pObj->action.ulMode != ulMode && ulMode == CTM_GLOBAL)
                    {
                        ulRet = 2;
                    }
                }
                pObj = pObj->pParent->pNext;
            }
            else
            {
                break;
            }
        }

    }
    STB_StrCat(szOutput, "\r\n");
    IO_OutString(szOutput);

    CLI_Free(szOutput);
    return ulRet;
}

/*********************************************************************/
/* 函数功能 : 命令树的列表显示                                       */
/*********************************************************************/
ULONG  CLI_CmdHelp()
{
    ULONG  ulMode;
    ULONG  ulObjIndex = 0;
    PST_CLI_CMDKEY pObj, pObjSave[5] = {NULL, NULL, NULL, NULL, NULL};
    ST_CLI_TERM* pTermStruct;
    ULONG  ulRet;

    pTermStruct = &m_stCliTerm;

    IO_Print(CLI_HELP_INFO);

    for (ulMode = 0; ulMode < CTM_TOTAL; ulMode ++)
    {
        if (m_sCliSysModes[ulMode].pCmdKey == NULL)
            continue;

        if (m_sCliSysModes[ulMode].rightLevel > pTermStruct->ucUserLevel)
            continue;

        pObj = m_sCliSysModes[ulMode].pCmdKey->pSubCmdKey;

        IO_OutString("\r\n\r\n --------------------------------\r\n");
        IO_Print(" Mode:%-15s  ", m_sCliSysModes[ulMode].szModeName);
        IO_OutString("\r\n --------------------------------\r\n");

        ulObjIndex = 0;

        while (pObj != NULL
            && (ulMode == CTM_GLOBAL || pObj != m_sCliSysModes[CTM_GLOBAL].pCmdKey->pSubCmdKey))
        {
            if (!STB_StriCmp(pObj->szName, "cm"))
            {
                pObj = pObj->pNext;
                continue;
            }

            if (IO_IsOutputBroken())
                return TBS_SUCCESS;

            //CLI_DELAY(1);

            //如果非全局模式下的命令大于全局模式的命令，
            //则先显示可能需要显示的全局模式命令
            if (ulMode != CTM_GLOBAL )
            {
                while (pObjSave[ulObjIndex] != NULL && STB_StriCmp(pObj->szName, pObjSave[ulObjIndex]->szName) > 0)
                {
                    __CLI_ObjectShow(ulMode, (ULONG )pTermStruct->ucUserLevel, pObjSave[ulObjIndex]);
                    ulObjIndex++;
                }
            }

            //显示当前分支
            ulRet = __CLI_ObjectShow(ulMode, (ULONG )pTermStruct->ucUserLevel, pObj);
            if (2 == ulRet && ulMode == CTM_GLOBAL)
            {
                if (ulObjIndex < 5)
                {
                    pObjSave[ulObjIndex++] = pObj;
                }
            }
            pObj = pObj->pNext;

            if (pObj == NULL && ulObjIndex < 5 && pObjSave[ulObjIndex] != 0)
            {
                //如果显示完全了，但还有全局模式的遗留命令没有输出，尝试输出
                while (ulObjIndex < 5 && pObjSave[ulObjIndex] != 0)
                {
                    __CLI_ObjectShow(ulMode, (ULONG )pTermStruct->ucUserLevel, pObjSave[ulObjIndex]);
                    ulObjIndex++;
                }
            }
        }
    }
    return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数功能 : 显示网络拓扑的命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_ShowTopology()
{
	int i = 0;
	int j = 0;
	int cnuid = 0;
	st_dbsClt clt;
	st_dbsCnu cnu;
	T_szCnuUserHFID cnuuserhfid;
	
	IO_Print("\r\n\r\n-------------------------------------------------------------------------------");
	IO_Print("\r\n    Index        DevType         MAC Address             RX/TX    Status"); 
	IO_Print("\r\n-------------------------------------------------------------------------------");

	for( j=1; j<=MAX_CLT_AMOUNT_LIMIT; j++ )
	{
		/* 获取CLT */
		if( CMM_SUCCESS != dbsGetClt(dbsdev, j, &clt) )
		{
			IO_Print("\r\n-+--CLT/%d        *               [ Not detected ]", j);
			continue;
		}
		/* 没有发现线卡*/
		else if( clt.col_row_sts == 0 )
		{
			IO_Print("\r\n-+--CLT/%d        *               [ Not detected ]", j);
			continue;
		}
		/* clt is detected */
		else
		{		
			IO_Print("\r\n-+--CLT/%d        *               [ %s ]    -/-       %d", clt.id, clt.col_mac, clt.col_sts);
			/* CNU */
			for( i=1; i<=MAX_CNUS_PER_CLT; i++ )
			{
				cnuid = (j-1)*MAX_CNUS_PER_CLT + i;
				if( CMM_SUCCESS == dbsGetCnu(dbsdev, cnuid, &cnu) )
				{
					if( 0 == cnu.col_row_sts )
					{
						continue;
					}
					else
					{
						if( boardapi_isAr6400Device(cnu.col_model) == 1)
						{
							IO_Print("\r\n    --CNU/%d/%-2d   %-16s[ %s ]  %3d/%-3d     %d",
								j, i, 
								boardapi_getDeviceModelStr(cnu.col_model),
								cnu.col_mac,
								cnu.col_rx,
								cnu.col_tx,
								cnu.col_sts
							);
						}
						else
						{
							if( cnu.col_sts == 1 )
							{
								if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, cnuuserhfid.ODA) )
								{
									return  CMM_FAILED;
								}
								if (CMM_SUCCESS != cli2cmm_readUserHFID(&cnuuserhfid))
								{
									return CMM_FAILED;
								}
									memcpy(cnu.col_user_hfid,cnuuserhfid.pdata,64);
									dbsUpdateCnu(dbsdev, cnuid, &cnu);
									dbsFflush(dbsdev);
							}
							IO_Print("\r\n    --CNU/%d/%-2d   %-16s[ %s ]  %3d/%-3d     %d",
								j, i, 
								boardapi_getDeviceModelStr(cnu.col_model) == "UNKNOWN" ? cnu.col_user_hfid : boardapi_getDeviceModelStr(cnu.col_model),
								cnu.col_mac,
								cnu.col_rx,
								cnu.col_tx,
								cnu.col_sts
							);
					     }
				       }
				}
			}
		}
	}
	/* 写日志*/
	__clt_opt_log(CMM_TOPOLOGY_SHOW, CMM_SUCCESS);
	return CMM_SUCCESS;	
}

/*********************************************************************/
/* 函数功能 :Display 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_ShowWlistUsers()
{
	int i = 0;
	int cltid = 0;
	int cnuid = 0;
	int num = 0;
	st_dbsSysinfo rowSysinfo;
	st_dbsCnu cnu;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1,  &rowSysinfo) )
	{
		IO_Print("\r\n  System Error !");
		__clt_opt_log(CMM_CLI_GET_USERS, CMM_FAILED);
		return CMM_FAILED;
	}
	else if( 0 == rowSysinfo.col_wlctl )
	{
		IO_Print("\r\n\r\n  White list control is disabled !");
		__clt_opt_log(CMM_CLI_GET_USERS, CMM_SUCCESS);
		return CMM_SUCCESS;
	}

	IO_Print("\r\n\r\n--------------------------------------------------------------------");
	IO_Print("\r\n%-10s  %-16s  %-20s  %-6s", "ID", "DevType", "MAC", "Status"); 
	IO_Print("\r\n--------------------------------------------------------------------");

	/* CNU */
	for( i=0; i<MAX_CNU_AMOUNT_LIMIT; i++ )
	{
		if( CMM_SUCCESS == dbsGetCnu(dbsdev, i+1, &cnu) )
		{
			if( 0 == cnu.col_row_sts )
			{
				continue;
			}
			else if( 0 == cnu.col_auth )
			{
				continue;
			}
			else
			{
				cltid = cnu.id/MAX_CNUS_PER_CLT + 1;
				cnuid = cnu.id%MAX_CNUS_PER_CLT;
				IO_Print("\r\nCNU/%d/%-4d  %-16s  %-20s  %-6d", cltid, cnuid, 
					boardapi_getDeviceModelStr(cnu.col_model), cnu.col_mac,	cnu.col_sts);
				num++;
			}
		}
	}
	if( !num )
	{
		IO_Print("\r\n  User white list is empty !");
	}

	/* 写日志*/
	__clt_opt_log(CMM_CLI_GET_USERS, CMM_SUCCESS);
	return CMM_SUCCESS;
}

/*********************************************************************/
/* 函数功能 :Display 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_Show_Optlog()
{
	int i = 0;
	time_t b_time;
	struct tm *tim;
	char timenow[32] = {0};
	st_dbsOptlog log;

	IO_Print("\r\n\r\n------------------------------------------------------------------------------");
	IO_Print("\r\n%-4s  %-16s  %-10s  %-32s  %s",
			"ID", "Time", "Who","CMD", "Result"); 
	IO_Print("\r\n------------------------------------------------------------------------------");

	/*操作日志最大存储数量=  512 */
	for( i=1; i<=512; i++ )
	{
		if( CMM_SUCCESS != dbsGetOptlog(dbsdev, i, &log) )
		{
			break;
		}
		else
		{
			/* 打印日志信息*/
			b_time = log.time;
			tim = localtime(&b_time);
			sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
				tim->tm_mday, tim->tm_hour, tim->tm_min
			);
			IO_Print("\r\n%-4d  %-16s  %-10s  %-32s  %s",
				i, timenow, boardapi_getModNameStr(log.who), log.cmd, (CMM_SUCCESS==log.result)?"SUCCESS":"FAILED"
			);	
		}
	}
	if( i <= 1 )
	{
		IO_Print("\r\n  No opt-log on the system !");
	}
	
	/* 写日志*/
	__clt_opt_log(CMM_OPT_LOG_SHOW, CMM_SUCCESS);
	return CMM_SUCCESS;
}

/*********************************************************************/
/* 函数功能 :Display 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_Show_alarmlog()
{
	int i = 0;
	time_t b_time;
	struct tm *tim;
	char timenow[32] = {0};
	st_dbsAlarmlog log;

	IO_Print("\r\n\r\n------------------------------------------------------------------------------");
	IO_Print("\r\n%-6s  %-16s  %-9s  %-6s  %s", "ID", "Time", "AlarmCode", "Node", "trap_info"); 
	IO_Print("\r\n------------------------------------------------------------------------------");

	/*告警日志最大存储数量==  512 */
	for( i=1; i<=512; i++ )
	{
		if( CMM_SUCCESS != dbsGetAlarmlog(dbsdev, i, &log) )
		{
			break;
		}
		else
		{
			/* 打印日志信息*/
			b_time = log.realTime;
			tim = localtime(&b_time);
			sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
				tim->tm_mday, tim->tm_hour, tim->tm_min
			);
			IO_Print("\r\n%-6d  %-16s  %-9d  %d.%-3d  %s",
						log.serialFlow,
						timenow,
						log.alarmCode,
						log.cltId,
						log.cnuId,
						log.trap_info
					);	
		}
	}
	if( i <= 1 )
	{
		IO_Print("\r\n  No alarm-log on the system !");
	}
	
	/* 写日志*/
	__clt_opt_log(CMM_ALARM_LOG_SHOW, CMM_SUCCESS);
	return CMM_SUCCESS;
}

/*********************************************************************/
/* 函数功能 :Display 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_ShowSyslog()
{
	int i = 0;
	time_t b_time;
	struct tm *tim;
	char timenow[32] = {0};
	st_dbsSyslog log;

	IO_Print("\r\n\r\n------------------------------------------------------------------------------");
	IO_Print("\r\n%-16s  %-10s  %-5s  %s", "Time","Module","Level", "Info"); 
	IO_Print("\r\n------------------------------------------------------------------------------");

	/*系统日志最大存储数量==  1024 */
	for( i=1; i<=1024; i++ )
	{
		if( CMM_SUCCESS != dbsGetSyslog(dbsdev, i, &log) )
		{
			break;
		}
		else
		{
			/* 打印日志信息*/
			b_time = log.time;
			tim = localtime(&b_time);
			sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
				tim->tm_mday, tim->tm_hour, tim->tm_min
			);			
			IO_Print("\r\n%-16s  %-10s  %-2d %s", timenow, boardapi_getModNameStr(log.who), log.level, log.log);	
		}
	}
	if( i <= 1 )
	{
		IO_Print("\r\n  No sys-log on the system !");
	}
	
	/* 写日志*/
	__clt_opt_log(CMM_SYSLOG_SHOW, CMM_SUCCESS);
	return CMM_SUCCESS;
}

/*********************************************************************/
/* 函数功能 :Display 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_ShowNetworkInfo()
{
	st_dbsNetwork networkInfo;

	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &networkInfo) )
	{
		/* 写日志*/
		__clt_opt_log(CMM_NETWORK_INFO_SHOW, CMM_FAILED);
		return CMM_FAILED;	
	}

	IO_Print("\r\n\r\n  MAC:			%s", networkInfo.col_mac);
	IO_Print("\r\n  DHCP:			%s", networkInfo.col_dhcp?"Yes":"No");
	IO_Print("\r\n  ip-address:		%s", networkInfo.col_ip);
	IO_Print("\r\n  netmask:		%s", networkInfo.col_netmask);
	IO_Print("\r\n  gateway:		%s", networkInfo.col_gw);
	IO_Print("\r\n  DNS:		%s", networkInfo.col_dns);	
	IO_Print("\r\n  MGMT-VLAN:		%s", networkInfo.col_mvlan_sts?"Enable":"Disable");
	IO_Print("\r\n  MGMT-VLAN-ID:		%d", networkInfo.col_mvlan_id);

	/* 写日志*/
	__clt_opt_log(CMM_NETWORK_INFO_SHOW, CMM_SUCCESS);
	return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数功能 :Display 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_ShowSysInfo()
{
	int ret = 0;
	st_dbsSysinfo szSysinfo;
	
#ifdef __AT30TK175STK__
	st_temperature temp_data;
	char temp[32] = {0};
#endif	

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &szSysinfo) )
	{
		/* 写日志*/
		__clt_opt_log(CMM_CLI_GET_SYSINFO, CMM_FAILED);
		return CMM_FAILED;	
	}
	
#ifdef __AT30TK175STK__
	ret = cli2cmm_getCbatTemperature(&temp_data);
#endif	

	//IO_Print("\r\n\r\n  Description:		WEC9720EK series EoC CBAT");
	IO_Print("\r\n\r\n  Device Model:		%s", boardapi_getDeviceModelStr(szSysinfo.col_model));
	IO_Print("\r\n  Maximum CLTs:		%d", MAX_CLT_AMOUNT_LIMIT);
	IO_Print("\r\n  Maximum CNUs:		%d*%d", MAX_CLT_AMOUNT_LIMIT, MAX_CNUS_PER_CLT);
	IO_Print("\r\n  White List Control:	%s", szSysinfo.col_wlctl?"Enable":"Disable");
	IO_Print("\r\n  Watch dog:		%s", szSysinfo.col_wdt?"Enable":"Disable");	
	IO_Print("\r\n  Hardware Version:	%s", szSysinfo.col_hwver);	
	IO_Print("\r\n  Bootloader Version:	%s", szSysinfo.col_bver);
	IO_Print("\r\n  Kernel Version:	%s", szSysinfo.col_kver);
	IO_Print("\r\n  APP Version:		%s", szSysinfo.col_appver);
	IO_Print("\r\n  Flash Size:		%dMB", szSysinfo.col_flashsize);
	IO_Print("\r\n  Sdram Size:		%dMB", szSysinfo.col_ramsize);
	/* demo version info */
	//IO_Print("\r\n  Manufacturer:		");
	IO_Print("\r\n  Manufacturer:		%s", szSysinfo.col_mfinfo);

#ifdef __AT30TK175STK__
	if( CMM_SUCCESS == ret )
	{
		if( 0 == temp_data.sign )
		{
			sprintf(temp, "+%d.%d", temp_data.itemp, temp_data.ftemp);
		}
		else
		{
			sprintf(temp, "-%d.%d", temp_data.itemp, temp_data.ftemp);
		}
		
		IO_Print("\r\n  temperature:		%s", temp);
	}
	else
	{
		IO_Print("\r\n  temperature:		unknown");
	}
#endif	

	/* 写日志*/
	__clt_opt_log(CMM_CLI_GET_SYSINFO, CMM_SUCCESS);
	return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数功能 :Display 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_ShowSnmpInfo()
{
	st_dbsSnmp snmp;

	/* 获取SNMP  的配置信息*/
	if( CMM_SUCCESS != dbsGetSnmp(dbsdev, 1, &snmp) )
	{
		/* 写日志*/
		__clt_opt_log(CMM_READ_SNMP_CONF, CMM_FAILED);
		return CMM_FAILED;
	}

	/* 打印*/
	IO_Print("\r\n");
	IO_Print("\r\n  snmp read-community:		%s", snmp.col_rdcom);
	IO_Print("\r\n  snmp write-community:		%s", snmp.col_wrcom);
	IO_Print("\r\n  first trap server address:	%s", snmp.col_sina);
	IO_Print("\r\n  first trap server port:	%d", snmp.col_tpa);
	IO_Print("\r\n  second trap server address:	%s", snmp.col_sinb);
	IO_Print("\r\n  second trap server port:	%d", snmp.col_tpb);

	/* 写日志*/
	__clt_opt_log(CMM_READ_SNMP_CONF, TBS_SUCCESS);
	return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数功能 :Display 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_ShowFtpServer()
{
	st_dbsSwmgmt ftpinfo;

	/* 获取FTP SERVER  的配置信息*/
	if( CMM_SUCCESS != dbsGetSwmgmt(dbsdev, 1, &ftpinfo) )
	{
		/* 写日志*/
		__clt_opt_log(CMM_CLI_GET_FTP, CMM_FAILED);
		return CMM_FAILED;
	}

	/* 打印*/
	IO_Print("\r\n");
	IO_Print("\r\n  remote ftp server ip address:	%s", ftpinfo.col_ip);
	IO_Print("\r\n  remote ftp server port:	%d", ftpinfo.col_port);
	IO_Print("\r\n  login username:		%s", ftpinfo.col_user);
	IO_Print("\r\n  login password:		%s", ftpinfo.col_pwd);
	IO_Print("\r\n  remote file path:		%s", ftpinfo.col_path);

	/* 写日志*/
	__clt_opt_log(CMM_CLI_GET_FTP, TBS_SUCCESS);
	return TBS_SUCCESS;
}

/**********************************************************************/
/* 函数功能 :CLI_Cmd_ShowCurProfile 命令辅助输出函数，只*/
/* 负责输出，不负责写日志								   */
/**********************************************************************/
ULONG __CLI_Cmd_ShowCltProfile(uint16_t id)
{
	st_dbsCltConf profile;
	
	if( (id<1)||(id>MAX_CLT_AMOUNT_LIMIT) )
	{
		return CMM_FAILED;
	}

	/* 获取CLT 的配置信息*/
	if( CMM_SUCCESS != dbsGetCltconf(dbsdev, id, &profile) )
	{
		return CMM_FAILED;
	}

	/* 打印*/
	IO_Print("\r\n");
	IO_Print("\r\n  Profile Descriptions:");
	IO_Print("\r\n  	clt profile id:		%d", profile.id);
	IO_Print("\r\n  	additional pib id:	%d", profile.col_base);
	IO_Print("\r\n  	profile status:		%s", (profile.col_row_sts)?"enable":"disable");
	IO_Print("\r\n");
	IO_Print("\r\n  Mac Address Limiting:");
	IO_Print("\r\n  	status:			%s", (profile.col_macLimit?"enable":"disable"));
	IO_Print("\r\n  	max bridged hosts:	%d", profile.col_macLimit);
	IO_Print("\r\n");
	IO_Print("\r\n  Cable Bandwidth Limiting:");
	IO_Print("\r\n  	uplink limiting status:		%s", (profile.col_curate?"enable":"disable"));
	IO_Print("\r\n  	uplink limit:			%d kbps", profile.col_curate);
	IO_Print("\r\n  	downlink limiting status:	%s", (profile.col_cdrate?"enable":"disable"));
	IO_Print("\r\n  	downlink limit:			%d kbps", profile.col_cdrate);
	IO_Print("\r\n");
	IO_Print("\r\n  Bridged Address Aging:");
	IO_Print("\r\n  	local bridged table aging:	%d minutes", profile.col_loagTime);
	IO_Print("\r\n  	remote bridged table aging:	%d minutes", profile.col_reagTime);
	IO_Print("\r\n");
	IO_Print("\r\n  Default CAP - Lowest Priority Classification:");
	IO_Print("\r\n  	igmp:				CAP %d", profile.col_igmpPri);
	IO_Print("\r\n  	unicast:			CAP %d", profile.col_unicastPri);
	IO_Print("\r\n  	igmp managed multicast stream:	CAP %d", profile.col_avsPri);
	IO_Print("\r\n  	multicast/broadcast:		CAP %d", profile.col_mcastPri);
	IO_Print("\r\n");
	IO_Print("\r\n  Tx Buffer Allocation Based On Priority:");
	IO_Print("\r\n  	status:	%s", (profile.col_tbaPriSts?"enable":"disable"));
	IO_Print("\r\n");
	IO_Print("\r\n  Assign Priority Using VLAN Tags [High]:");
	IO_Print("\r\n  	status:	%s", (profile.col_cosPriSts?"enable":"disable"));
	IO_Print("\r\n  	cos 0:	CAP %d", profile.col_cos0pri);
	IO_Print("\r\n  	cos 1:	CAP %d", profile.col_cos1pri);
	IO_Print("\r\n  	cos 2:	CAP %d", profile.col_cos2pri);
	IO_Print("\r\n  	cos 3:	CAP %d", profile.col_cos3pri);
	IO_Print("\r\n  	cos 4:	CAP %d", profile.col_cos4pri);
	IO_Print("\r\n  	cos 5:	CAP %d", profile.col_cos5pri);
	IO_Print("\r\n  	cos 6:	CAP %d", profile.col_cos6pri);
	IO_Print("\r\n  	cos 7:	CAP %d", profile.col_cos7pri);
	IO_Print("\r\n");
	IO_Print("\r\n  Assign Priority Using Traffic Class [Low]:");
	IO_Print("\r\n  	status:	%s", (profile.col_tosPriSts?"enable":"disable"));
	IO_Print("\r\n  	tos 0:	CAP %d", profile.col_tos0pri);
	IO_Print("\r\n  	tos 1:	CAP %d", profile.col_tos1pri);
	IO_Print("\r\n  	tos 2:	CAP %d", profile.col_tos2pri);
	IO_Print("\r\n  	tos 3:	CAP %d", profile.col_tos3pri);
	IO_Print("\r\n  	tos 4:	CAP %d", profile.col_tos4pri);
	IO_Print("\r\n  	tos 5:	CAP %d", profile.col_tos5pri);
	IO_Print("\r\n  	tos 6:	CAP %d", profile.col_tos6pri);
	IO_Print("\r\n  	tos 7:	CAP %d", profile.col_tos7pri);
	IO_Print("\r\n");
	
	//IO_Print("\r\n\r\n  __CLI_Cmd_ShowCltProfile(%d)", id);
	return CMM_SUCCESS;
}

/**********************************************************************/
/* 函数功能 :CLI_Cmd_ShowCurProfile 命令辅助输出函数，只*/
/* 负责输出，不负责写日志								   */
/**********************************************************************/
ULONG __CLI_Cmd_ShowCnuProfile(uint16_t id)
{
	st_dbsCnu cnu;
	st_dbsProfile profile;
	
	if( (id<1)||(id>MAX_CLT_AMOUNT_LIMIT*MAX_CNUS_PER_CLT) )
	{
		return CMM_FAILED;
	}

	/* 需要为不同型号的设备定制不同的配置信息*/
	/* 获取CNU的设备型号*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		return CMM_FAILED;
	}
	/* 获取CNU的配置信息*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id, &profile) )
	{
		return CMM_FAILED;
	}
	/* 打印*/
	IO_Print("\r\n");
	IO_Print("\r\n  Profile Descriptions:");
	IO_Print("\r\n  	cnu profile id:		%d", profile.id);
	IO_Print("\r\n  	additional pib id:	%d", profile.col_base);
	IO_Print("\r\n  	profile status:		%s", (profile.col_row_sts)?"enable":"disable");
	IO_Print("\r\n");
	IO_Print("\r\n  Mac Address Limiting:");
	IO_Print("\r\n  	status:			%s", (profile.col_macLimit?"enable":"disable"));
	if( 65 == profile.col_macLimit )
	{
		IO_Print("\r\n  	max bridged hosts:	0");
	}
	else
	{
		IO_Print("\r\n  	max bridged hosts:	%d", profile.col_macLimit);
	}
	IO_Print("\r\n");
	IO_Print("\r\n  Cable Bandwidth Limiting:");
	IO_Print("\r\n  	uplink limiting status:		%s", (profile.col_curate?"enable":"disable"));
	IO_Print("\r\n  	uplink limit:			%d kbps", profile.col_curate);
	IO_Print("\r\n  	downlink limiting status:	%s", (profile.col_cdrate?"enable":"disable"));
	IO_Print("\r\n  	downlink limit:			%d kbps", profile.col_cdrate);
	IO_Print("\r\n");
	IO_Print("\r\n  Bridged Address Aging:");
	IO_Print("\r\n  	local bridged table aging:	%d minutes", profile.col_loagTime);
	IO_Print("\r\n  	remote bridged table aging:	%d minutes", profile.col_reagTime);
	IO_Print("\r\n");
	IO_Print("\r\n  Default CAP - Lowest Priority Classification:");
	IO_Print("\r\n  	igmp:				CAP %d", profile.col_igmpPri);
	IO_Print("\r\n  	unicast:			CAP %d", profile.col_unicastPri);
	IO_Print("\r\n  	igmp managed multicast stream:	CAP %d", profile.col_avsPri);
	IO_Print("\r\n  	multicast/broadcast:		CAP %d", profile.col_mcastPri);
	IO_Print("\r\n");
	IO_Print("\r\n  Tx Buffer Allocation Based On Priority:");
	IO_Print("\r\n  	status:	%s", (profile.col_tbaPriSts?"enable":"disable"));
	IO_Print("\r\n");
	IO_Print("\r\n  Assign Priority Using VLAN Tags [High]:");
	IO_Print("\r\n  	status:	%s", (profile.col_cosPriSts?"enable":"disable"));
	IO_Print("\r\n  	cos 0:	CAP %d", profile.col_cos0pri);
	IO_Print("\r\n  	cos 1:	CAP %d", profile.col_cos1pri);
	IO_Print("\r\n  	cos 2:	CAP %d", profile.col_cos2pri);
	IO_Print("\r\n  	cos 3:	CAP %d", profile.col_cos3pri);
	IO_Print("\r\n  	cos 4:	CAP %d", profile.col_cos4pri);
	IO_Print("\r\n  	cos 5:	CAP %d", profile.col_cos5pri);
	IO_Print("\r\n  	cos 6:	CAP %d", profile.col_cos6pri);
	IO_Print("\r\n  	cos 7:	CAP %d", profile.col_cos7pri);
	IO_Print("\r\n");
	IO_Print("\r\n  Assign Priority Using Traffic Class [Low]:");
	IO_Print("\r\n  	status:	%s", (profile.col_tosPriSts?"enable":"disable"));
	IO_Print("\r\n  	tos 0:	CAP %d", profile.col_tos0pri);
	IO_Print("\r\n  	tos 1:	CAP %d", profile.col_tos1pri);
	IO_Print("\r\n  	tos 2:	CAP %d", profile.col_tos2pri);
	IO_Print("\r\n  	tos 3:	CAP %d", profile.col_tos3pri);
	IO_Print("\r\n  	tos 4:	CAP %d", profile.col_tos4pri);
	IO_Print("\r\n  	tos 5:	CAP %d", profile.col_tos5pri);
	IO_Print("\r\n  	tos 6:	CAP %d", profile.col_tos6pri);
	IO_Print("\r\n  	tos 7:	CAP %d", profile.col_tos7pri);
	IO_Print("\r\n");
	IO_Print("\r\n  Storm Filter Settings:");
	IO_Print("\r\n  	broadcast storm filter:	%s", (profile.col_sfbSts?"enable":"disable"));
	IO_Print("\r\n  	unicast storm filter:	%s", (profile.col_sfuSts?"enable":"disable"));
	IO_Print("\r\n  	multicast storm filter:	%s", (profile.col_sfmSts?"enable":"disable"));	
	if( 0 == profile.col_sfRate )
	{
		IO_Print("\r\n  	storm filter level:	disable");
	}
	else
	{
		IO_Print("\r\n  	storm filter level:	%d Kpps", (1<<(profile.col_sfRate-1)));
	}	
	IO_Print("\r\n");
	IO_Print("\r\n  802.1Q VLAN Settings:");
	IO_Print("\r\n  	status:		%s", (profile.col_vlanSts?"enable":"disable"));
	IO_Print("\r\n  	ETH1 PVID:	%d", profile.col_eth1vid);
	IO_Print("\r\n  	ETH2 PVID:	%d", profile.col_eth2vid);
	IO_Print("\r\n  	ETH3 PVID:	%d", profile.col_eth3vid);
	IO_Print("\r\n  	ETH4 PVID:	%d", profile.col_eth4vid);
	IO_Print("\r\n");
	IO_Print("\r\n  Port Priority Settings:");
	IO_Print("\r\n  	status:		%s", (profile.col_portPriSts?"enable":"disable"));
	IO_Print("\r\n  	ETH1 Priority:	%d", profile.col_eth1pri);
	IO_Print("\r\n  	ETH2 Priority:	%d", profile.col_eth2pri);
	IO_Print("\r\n  	ETH3 Priority:	%d", profile.col_eth3pri);
	IO_Print("\r\n  	ETH4 Priority:	%d", profile.col_eth4pri);
	IO_Print("\r\n");
	IO_Print("\r\n  Port Speed Limiting:");
	IO_Print("\r\n  	Port RX Limiting:	%s", (profile.col_rxLimitSts?"enable":"disable"));
	if( (0==profile.col_cpuPortRxRate)||(profile.col_cpuPortRxRate==(32*0x1fff)))
	{
		IO_Print("\r\n  	cpu port rx rate:	* Kbps");
	}
	else
	{
		IO_Print("\r\n  	cpu port rx rate:	%d Kbps", profile.col_cpuPortRxRate);
	}
	if( (0==profile.col_eth1rx)||(profile.col_eth1rx==(32*0x1fff)))
	{
		IO_Print("\r\n  	ETH1 rx rate:		* Kbps");
	}
	else
	{
		IO_Print("\r\n  	ETH1 rx rate:		%d Kbps", profile.col_eth1rx);
	}
	if( (0==profile.col_eth2rx)||(profile.col_eth2rx==(32*0x1fff)))
	{
		IO_Print("\r\n  	ETH2 rx rate:		* Kbps");
	}
	else
	{
		IO_Print("\r\n  	ETH2 rx rate:		%d Kbps", profile.col_eth2rx);
	}
	if( (0==profile.col_eth3rx)||(profile.col_eth3rx==(32*0x1fff)))
	{
		IO_Print("\r\n  	ETH3 rx rate:		* Kbps");
	}
	else
	{
		IO_Print("\r\n  	ETH3 rx rate:		%d Kbps", profile.col_eth3rx);
	}
	if( (0==profile.col_eth4rx)||(profile.col_eth4rx==(32*0x1fff)))
	{
		IO_Print("\r\n  	ETH4 rx rate:		* Kbps");
	}
	else
	{
		IO_Print("\r\n  	ETH4 rx rate:		%d Kbps", profile.col_eth4rx);
	}
	IO_Print("\r\n  	Port TX Limiting:	%s", (profile.col_txLimitSts?"enable":"disable"));
	if( (0==profile.col_cpuPortTxRate)||(profile.col_cpuPortTxRate==(32*0x1fff)))
	{
		IO_Print("\r\n  	cpu port tx rate:	* Kbps");
	}
	else
	{
		IO_Print("\r\n  	cpu port tx rate:	%d Kbps", profile.col_cpuPortTxRate);
	}
	if( (0==profile.col_eth1tx)||(profile.col_eth1tx==(32*0x1fff)))
	{
		IO_Print("\r\n  	ETH1 tx rate:		* Kbps");
	}
	else
	{
		IO_Print("\r\n  	ETH1 tx rate:		%d Kbps", profile.col_eth1tx);
	}
	if( (0==profile.col_eth2tx)||(profile.col_eth2tx==(32*0x1fff)))
	{
		IO_Print("\r\n  	ETH2 tx rate:		* Kbps");
	}
	else
	{
		IO_Print("\r\n  	ETH2 tx rate:		%d Kbps", profile.col_eth2tx);
	}
	if( (0==profile.col_eth3tx)||(profile.col_eth3tx==(32*0x1fff)))
	{
		IO_Print("\r\n  	ETH3 tx rate:		* Kbps");
	}
	else
	{
		IO_Print("\r\n  	ETH3 tx rate:		%d Kbps", profile.col_eth3tx);
	}
	if( (0==profile.col_eth4tx)||(profile.col_eth4tx==(32*0x1fff)))
	{
		IO_Print("\r\n  	ETH4 tx rate:		* Kbps");
	}
	else
	{
		IO_Print("\r\n  	ETH4 tx rate:		%d Kbps", profile.col_eth4tx);
	}
	IO_Print("\r\n");
	IO_Print("\r\n  Port Link Status Control:");
	IO_Print("\r\n  	status:			%s", (profile.col_psctlSts?"enable":"disable"));
	IO_Print("\r\n  	cpu port link status:	%s", (profile.col_cpuPortSts?"enable":"disable"));
	IO_Print("\r\n  	ETH1 link status:	%s", (profile.col_eth1sts?"enable":"disable"));
	IO_Print("\r\n  	ETH2 link status:	%s", (profile.col_eth2sts?"enable":"disable"));
	IO_Print("\r\n  	ETH3 link status:	%s", (profile.col_eth3sts?"enable":"disable"));
	IO_Print("\r\n  	ETH4 link status:	%s", (profile.col_eth4sts?"enable":"disable"));
	IO_Print("\r\n");
	
	//IO_Print("\r\n\r\n  __CLI_Cmd_ShowCnuProfile(%d)", id);
	return CMM_SUCCESS;
}

/*********************************************************************/
/* 函数功能 :Display 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_ShowCurProfile()
{
	ULONG ret = CMM_SUCCESS;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CLT/CNU模式下执行*/
	if( 1 == prevailMode )
	{
		/* interface clt 模式*/
		id = CLI_GetCltTidByMode(iMode);
		ret = __CLI_Cmd_ShowCltProfile(id);
	}
	else if( 2 == prevailMode )
	{
		/* interface cnu 模式*/
		id = CLI_GetCnuTidByMode(iMode);
		ret = __CLI_Cmd_ShowCnuProfile(id);
	}
	else
	{
		/* 全局模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return TBS_SUCCESS;
	}	
	
	/* 写日志*/
	__clt_opt_log(CMM_CLI_GET_CUR_PROFILE, (ret?CMM_FAILED:CMM_SUCCESS));
	return (ret?CMM_FAILED:CMM_SUCCESS);
}

/*********************************************************************/
/* 函数功能 :修改用户密码 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_SetCliPwd()
{
	char *szP;
	char *szU;
	uint16_t uid = 0;
	st_dbsCliRole cliRole;

	if((szP=CLI_GetParamByName("password"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	if( (szU = CLI_GetParamByName("admin|operator|user")) == NULL )
	{
		IO_Print("\r\n\r\n  Username Parameter Error.");
        	return TBS_FAILED;
	}

	/* 判断密码长度，不能超过16个字符*/
	if( strlen(szP) >= 16 )
	{
		/* 写日志*/
		__clt_opt_log(CMM_CLI_SET_USERPASSWORD, CMM_FAILED);
		return TBS_FAILED;
	}
	
	if( strcmp(szU, "admin") == 0 )
	{
		uid = 1;
		cliRole.id = 1;
		strcpy(cliRole.col_user, "admin");
		strcpy(cliRole.col_pwd, szP);
	}
	else if( strcmp(szU, "operator") == 0 )
	{
		uid = 2;
		cliRole.id = 2;
		strcpy(cliRole.col_user, "operator");
		strcpy(cliRole.col_pwd, szP);
	}
	else
	{
		uid = 3;
		cliRole.id = 3;
		strcpy(cliRole.col_user, "user");
		strcpy(cliRole.col_pwd, szP);
	}
	
	/* 写入数据库*/
	if( CMM_SUCCESS != dbsUpdateCliRole(dbsdev, uid, &cliRole) )
	{
		/* 写日志*/
		IO_Print("\r\n\r\n  Password Too Long !");
		__clt_opt_log(CMM_CLI_SET_USERPASSWORD, CMM_FAILED);
		return TBS_FAILED;
	}
	else
	{
		/* 写日志*/
		__clt_opt_log(CMM_CLI_SET_USERPASSWORD, TBS_SUCCESS);
		IO_Print("\r\n\r\n  Success !");
		return TBS_SUCCESS;
	}
}

/*********************************************************************/
/* 函数功能 :CLI_Cmd_SnmpConfig 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_SnmpConfig()
{
	char *pParam;
	char  *szP;
	st_dbsSnmp szSnmpConfig;	

	if((pParam=CLI_GetParamByName("read-community|write-community|trap-community|trap-server|trap-desport"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}

	/* 设置前先读取SNMP配置信息*/
	if( dbsGetSnmp(dbsdev, 1, &szSnmpConfig) != CMM_SUCCESS)
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_WRITE_SNMP_CONF, CMM_FAILED);
		return CMM_FAILED;
	}
	
	if(!STB_StriCmp(pParam, "read-community"))
	{		
		if((szP=CLI_GetParamByName("rc"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
		/* 判断长度，不能超过64 个字符*/
		else if( strlen(szP) >= 64 )
		{
			IO_Print("\r\n\r\n  Parameter Too Long !");
			__clt_opt_log(CMM_WRITE_SNMP_CONF, CMM_FAILED);
			return CMM_FAILED;
		}
		else
		{
			strcpy(szSnmpConfig.col_rdcom, szP);
		}
	}
	else if(!STB_StriCmp(pParam, "write-community"))
	{		
		if((szP=CLI_GetParamByName("wc"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
		/* 判断长度，不能超过64 个字符*/
		else if( strlen(szP) >= 64 )
		{
			IO_Print("\r\n\r\n  Parameter Too Long !");
			__clt_opt_log(CMM_WRITE_SNMP_CONF, CMM_FAILED);
			return CMM_FAILED;
		}
		else
		{
			strcpy(szSnmpConfig.col_wrcom, szP);
		}
	}
	else if(!STB_StriCmp(pParam, "trap-community"))
	{		
		if((szP=CLI_GetParamByName("tc"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
		/* 判断长度，不能超过64 个字符*/
		else if( strlen(szP) >= 64 )
		{
			IO_Print("\r\n\r\n  Parameter Too Long !");
			__clt_opt_log(CMM_WRITE_SNMP_CONF, CMM_FAILED);
			return CMM_FAILED;
		}
		else
		{
			strcpy(szSnmpConfig.col_trapcom, szP);
		}
	}
	else if(!STB_StriCmp(pParam, "trap-server"))
	{
		if((szP=CLI_GetParamByName("ts"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
		/* 判断IP 地址是否合法*/
		else if( !__isIpv4Valid(szP, strlen(szP)) )
		{
			IO_Print("\r\n\r\n  Ipv4 Address Invalid !");
			__clt_opt_log(CMM_WRITE_SNMP_CONF, CMM_FAILED);
			return CMM_FAILED;
		}
		else
		{
			strcpy(szSnmpConfig.col_sina, szP);
		}		
	}
	else if(!STB_StriCmp(pParam, "trap-desport"))
	{		
		if((szP=CLI_GetParamByName("port"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
		else
		{
			szSnmpConfig.col_tpa = atoi(szP);
		}
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}

	/* 写入数据库*/
	if( CMM_SUCCESS != dbsUpdateSnmp(dbsdev, 1, &szSnmpConfig) )
	{
		/* 写日志*/
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_WRITE_SNMP_CONF, CMM_FAILED);
		return TBS_FAILED;
	}
	else
	{
		/* 写日志*/
		__clt_opt_log(CMM_WRITE_SNMP_CONF, CMM_SUCCESS);
		IO_Print("\r\n\r\n  Success !");
		return CMM_SUCCESS;
	}
}

/*********************************************************************/
/* 函数功能 :save-config 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_SaveConfig()
{
	if( CMM_SUCCESS == dbsFflush(dbsdev) )
	{
		IO_Print("\r\n\r\n  Success !");
		__clt_opt_log(CMM_SAVE_CONFIG, CMM_SUCCESS);
		return CMM_SUCCESS;
	}
	else
	{
		__clt_opt_log(CMM_SAVE_CONFIG, CMM_FAILED);
		return CMM_FAILED;
	}
	
}

/*********************************************************************/
/* 函数功能 :配置局端IP 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_SetIpAddress()
{
	char  *szP;
	st_dbsNetwork szNetwork;	

	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &szNetwork) )
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_SET_IP, CMM_FAILED);
		return CMM_FAILED;
	}

	if((szP=CLI_GetParamByName("nipaddr"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	else
	{
		strcpy(szNetwork.col_ip, szP);
	}

	if((szP=CLI_GetParamByName("netmask"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	else
	{
		strcpy(szNetwork.col_netmask, szP);
	}

	if( !isIpv4ValidByMask(szNetwork.col_ip, strlen(szNetwork.col_ip), szNetwork.col_netmask, strlen(szNetwork.col_netmask)) )
	{
		IO_Print("\r\n\r\n  Invalid ipv4 address or subnet mask address!");
		__clt_opt_log(CMM_CLI_SET_IP, CMM_FAILED);
		return CMM_FAILED;
	}	
	
	if( CMM_SUCCESS == dbsUpdateNetwork(dbsdev, 1, &szNetwork))
	{
		IO_Print("\r\n\r\n  Success !");
		__clt_opt_log(CMM_CLI_SET_IP, CMM_SUCCESS);
		return CMM_SUCCESS;
	}
	else
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_SET_IP, CMM_FAILED);
		return CMM_FAILED;
	}
}

/*********************************************************************/
/* 函数功能 :Undo ip-address命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_UndoIpAddress()
{
	st_dbsNetwork szNetwork;

	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &szNetwork) )
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_UNDO_IPADDR, CMM_FAILED);
		return CMM_FAILED;
	}
	
	/*设置默认ip:192.168.223.1  netmask:255.255.255.0*/
	strcpy(szNetwork.col_ip, "192.168.223.1");
	strcpy(szNetwork.col_netmask, "255.255.255.0");
	szNetwork.col_dhcp = 0;
	
	if( CMM_SUCCESS == dbsUpdateNetwork(dbsdev, 1, &szNetwork))
	{
		IO_Print("\r\n\r\n  Success !");
		__clt_opt_log(CMM_CLI_UNDO_IPADDR, CMM_SUCCESS);
		return CMM_SUCCESS;
	}
	else
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_UNDO_IPADDR, CMM_FAILED);
		return CMM_FAILED;
	}
}

/*********************************************************************/
/* 函数功能 :配置局端IP gateway 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_SetIpGateway()
{
	uint32_t I_ip = 0;
	uint32_t I_mask = 0;
	uint32_t I_gw = 0;
	char  *szP;
	st_dbsNetwork szNetwork;	

	if((szP=CLI_GetParamByName("ipgateway"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}

	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &szNetwork) )
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_SET_IPGATEWAY, CMM_FAILED);
		return CMM_FAILED;
	}
	else
	{
		strcpy(szNetwork.col_gw, szP);
	}

	if( !isIpv4ValidByMask(szNetwork.col_gw, strlen(szNetwork.col_gw), szNetwork.col_netmask, strlen(szNetwork.col_netmask)) )
	{
		IO_Print("\r\n\r\n  IPV4 Address invalid !");
		__clt_opt_log(CMM_CLI_SET_IPGATEWAY, CMM_FAILED);
		return CMM_FAILED;
	}
	else
	{
		/* 网管要和IP在同一网段*/
		I_ip = inet_addr(szNetwork.col_ip);
		I_mask = inet_addr(szNetwork.col_netmask);
		I_gw = inet_addr(szNetwork.col_gw);
		if((I_ip & I_mask) != (I_gw & I_mask))
		{
			IO_Print("\r\n\r\n  Gateway and IP address must be the same subnet !");
			__clt_opt_log(CMM_CLI_SET_IPGATEWAY, CMM_FAILED);
			return CMM_FAILED;
		}
	}
	
	if( CMM_SUCCESS == dbsUpdateNetwork(dbsdev, 1, &szNetwork))
	{
		IO_Print("\r\n\r\n  Success !");
		__clt_opt_log(CMM_CLI_SET_IPGATEWAY, CMM_SUCCESS);
		return CMM_SUCCESS;
	}
	else
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_SET_IPGATEWAY, CMM_FAILED);
		return CMM_FAILED;
	}
}

/*********************************************************************/
/* 函数功能 :Undo ip-gateway命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_UndoIpGateway()
{
	st_dbsNetwork szNetwork;

	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &szNetwork) )
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_UNDO_IP_GW, CMM_FAILED);
		return CMM_FAILED;
	}
	
	/*设置默认gw 0.0.0.0*/
	strcpy(szNetwork.col_gw, "0.0.0.0");
	
	if( CMM_SUCCESS == dbsUpdateNetwork(dbsdev, 1, &szNetwork))
	{
		IO_Print("\r\n\r\n  Success !");
		__clt_opt_log(CMM_CLI_UNDO_IP_GW, CMM_SUCCESS);
		return CMM_SUCCESS;
	}
	else
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_UNDO_IP_GW, CMM_FAILED);
		return CMM_FAILED;
	}
}

/*********************************************************************/
/* 函数功能 :配置管理VLAN 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_SetMgmtVlan()
{
	char  *szP;
	st_dbsNetwork szNetwork;	

	if((szP=CLI_GetParamByName("vlan"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &szNetwork) )
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_SET_MVLAN, CMM_FAILED);
		return CMM_FAILED;
	}

	/* 判断VLAN ID 是否有效*/
	szNetwork.col_mvlan_sts = 1;
	szNetwork.col_mvlan_id = atoi(szP);
	if( (szNetwork.col_mvlan_id < 1) || (szNetwork.col_mvlan_id > 4095) )
	{
		IO_Print("\r\n\r\n  Vlan ID invalid !");
		__clt_opt_log(CMM_CLI_SET_MVLAN, CMM_FAILED);
		return CMM_FAILED;
	}

	/* 保存配置*/
	if( CMM_SUCCESS == dbsUpdateNetwork(dbsdev, 1, &szNetwork))
	{
		IO_Print("\r\n\r\n  Success !");
		__clt_opt_log(CMM_CLI_SET_MVLAN, CMM_SUCCESS);
		return CMM_SUCCESS;
	}
	else
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_SET_MVLAN, CMM_FAILED);
		return CMM_FAILED;
	}
}

/*********************************************************************/
/* 函数功能 :Undo mgmt-vlan命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_UndoMgmtVlan()
{
	st_dbsNetwork szNetwork;	

	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &szNetwork) )
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_UNDO_MVLAN, CMM_FAILED);
		return CMM_FAILED;
	}
	
	/*禁用管理VLAN*/
	szNetwork.col_mvlan_sts = 0;
	szNetwork.col_mvlan_id = 1;
	
	/* 保存配置*/
	if( CMM_SUCCESS == dbsUpdateNetwork(dbsdev, 1, &szNetwork))
	{
		IO_Print("\r\n\r\n  Success !");
		__clt_opt_log(CMM_CLI_UNDO_MVLAN, CMM_SUCCESS);
		return CMM_SUCCESS;
	}
	else
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_UNDO_MVLAN, CMM_FAILED);
		return CMM_FAILED;
	}
}

/*********************************************************************/
/* 函数功能 :set ftp_server 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_SetFtpServer()
{
	char *pParam;
	char  *szP;
	st_dbsSwmgmt szFtpserver;
	
	/* 获取输入参数并判断*/
	if((pParam=CLI_GetParamByName("ip-address|port|user-name|passwd|file-path"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}

	/* 设置之前先读取配置*/
	if( CMM_SUCCESS != dbsGetSwmgmt(dbsdev, 1, &szFtpserver) )
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_SET_FTPSERVER, CMM_FAILED);
		return CMM_FAILED;
	}

	if(!STB_StriCmp(pParam, "ip-address"))
	{		
		if((szP=CLI_GetParamByName("ip"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
		/* 判断IP 地址是否合法*/
		else if( !__isIpv4Valid(szP, strlen(szP)) )
		{
			IO_Print("\r\n\r\n  Ipv4 Address Invalid !");
			__clt_opt_log(CMM_CLI_SET_FTPSERVER, CMM_FAILED);
			return CMM_FAILED;
		}
		else
		{
			strcpy(szFtpserver.col_ip, szP);
		}
	}
	else if(!STB_StriCmp(pParam, "user-name"))
	{		
		if((szP=CLI_GetParamByName("username"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
		/* 判断长度，不能超过32 个字符*/
		else if( strlen(szP) >= 32 )
		{
			IO_Print("\r\n\r\n  Parameter Too Long !");
			__clt_opt_log(CMM_CLI_SET_FTPSERVER, CMM_FAILED);
			return CMM_FAILED;
		}
		else
		{
			strcpy(szFtpserver.col_user, szP);
		}
	}
	else if(!STB_StriCmp(pParam, "passwd"))
	{		
		if((szP=CLI_GetParamByName("password"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
		/* 判断长度，不能超过16 个字符*/
		else if( strlen(szP) >= 16 )
		{
			IO_Print("\r\n\r\n  Parameter Too Long !");
			__clt_opt_log(CMM_CLI_SET_FTPSERVER, CMM_FAILED);
			return CMM_FAILED;
		}
		else
		{
			strcpy(szFtpserver.col_pwd, szP);
		}
	}
	else if(!STB_StriCmp(pParam, "file-path"))
	{		
		if((szP=CLI_GetParamByName("filepath"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
		/* 判断长度，不能超过256 个字符*/
		else if( strlen(szP) >= 256 )
		{
			IO_Print("\r\n\r\n  Parameter Too Long !");
			__clt_opt_log(CMM_CLI_SET_FTPSERVER, CMM_FAILED);
			return CMM_FAILED;
		}
		else
		{
			strcpy(szFtpserver.col_path, szP);
		}
	}
	else if(!STB_StriCmp(pParam, "port"))
	{		
		if((szP=CLI_GetParamByName("sport"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
		szFtpserver.col_port = atoi(szP);
		/* 判断长度，不能超过256 个字符*/
		if( (szFtpserver.col_port < 1) || (szFtpserver.col_port > 65535) )
		{
			IO_Print("\r\n\r\n  Parameter Error !");
			__clt_opt_log(CMM_CLI_SET_FTPSERVER, CMM_FAILED);
			return CMM_FAILED;
		}		
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	/* 写入数据库*/
	if( CMM_SUCCESS != dbsUpdateSwmgmt(dbsdev, 1, &szFtpserver) )
	{
		/* 写日志*/
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_SET_FTPSERVER, CMM_FAILED);
		return CMM_FAILED;
	}
	else
	{
		/* 写日志*/
		__clt_opt_log(CMM_CLI_SET_FTPSERVER, CMM_SUCCESS);
		IO_Print("\r\n\r\n  Success !");
		return CMM_SUCCESS;
	}
}

/***********************************************************************/
/***********************************************************************/
/* 下面的命令通过CMM 进行代理，操作日志由CMM 记录*/
/***********************************************************************/
/***********************************************************************/

/***********************************************************************/
/* 函数功能 :Debug 命令实现               					                   */
/***********************************************************************/
ULONG CLI_Cmd_Debug()
{
	char *pParam;
	st_ModuleDebugCtl Moduledebug;

	if((pParam=CLI_GetParamByName("cmm|dbs |sql"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	if (!STB_StriCmp(pParam, "dbs"))
	{
		Moduledebug.usMID = MID_DBS;		
	}
	else if (!STB_StriCmp(pParam, "sql"))
	{
		Moduledebug.usMID = MID_SQL;		
	}
	else if (!STB_StriCmp(pParam, "cmm"))
	{
		Moduledebug.usMID = MID_CMM;		
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	if((pParam=CLI_GetParamByName("enable|disable"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	if (!STB_StriCmp(pParam, "enable"))
	{
		Moduledebug.enable = BOOL_TRUE;		
	}
	else if (!STB_StriCmp(pParam, "disable"))
	{
		Moduledebug.enable = BOOL_FALSE;		
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	if( CMM_SUCCESS == cli2cmm_debug(&Moduledebug) )
	{
		IO_Print("\r\n\r\n  Success !");
		return CMM_SUCCESS;
	}
	return CMM_FAILED;
}

/*********************************************************************/
/* 函数功能 : 重启CBAT 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_Reboot()
{
	ULONG ret = CMM_SUCCESS;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	if( 1 == prevailMode )
	{
		/* interface clt 模式*//* 重启CLT */		
		id = CLI_GetCltTidByMode(iMode);
		ret = cli2cmm_resetClt(id);
		if( CMM_SUCCESS == ret )
		{
			IO_Print("\r\n\r\n  Reboot CLT Success !");
		}
	}
	else if( 2 == prevailMode )
	{
		/* interface cnu 模式*//* 重启CNU */
		id = CLI_GetCnuTidByMode(iMode);
		ret = cli2cmm_resetCnu(id);
		if( CMM_SUCCESS == ret )
		{
			IO_Print("\r\n\r\n  Reboot CNU Success !");
		}
	}
	else
	{
		/* 全局模式*//* 重启MP */
		ret = cli2cmm_resetMp();
		if( CMM_SUCCESS == ret )
		{
			IO_Print("\r\n\r\n  Reboot CBAT Success !");
		}
	}
	return ret?CMM_FAILED:CMM_SUCCESS;
}

/*********************************************************************/
/* 函数功能 :restore default 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_RestoreDefault()
{
	if( CMM_SUCCESS == cli2cmm_restoreDefault() )
	{
		IO_Print("\r\n\r\n  Success !");
		return CMM_SUCCESS;
	}
	return CMM_FAILED;
}

/*********************************************************************/
/* 函数功能 :vlan 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_DoCnuVlanConfig()
{
	char *pParam;
	char  *szP;
	ULONG ret = CMM_SUCCESS;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t cltid = 0;
	uint16_t cnuid = 0;
	uint16_t port = 1;
	uint16_t vid = 1;
	st_dbsCnu cnu;
	st_dbsProfile profile;
	rtl8306eWriteInfo rtl8306eSettings;
	
	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);
	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	cltid = CLI_GetCltTidByMode(iMode);
	cnuid = CLI_GetCnuTidByMode(iMode);
	rtl8306eSettings.node.clt = cltid;
	rtl8306eSettings.node.cnu = cnuid;
	if((pParam=CLI_GetParamByName("set"))==NULL)
	{
		MT_ERRLOG(0);
		return CMM_FAILED;
	}

	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, cnuid, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	if( cnu.col_auth == 0)
	{
		cnu.col_auth = 1;
		dbsUpdateCnu(dbsdev, cnuid, &cnu);
	}
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, cnuid,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}
	
	if(!STB_StriCmp(pParam, "set"))
	{
		if((szP=CLI_GetParamByName("port"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
		else
		{
			if (!STB_StriCmp(szP, "eth1")) port = 1;
			else if (!STB_StriCmp(szP, "eth2")) port = 2;
			else if (!STB_StriCmp(szP, "eth3")) port = 3;
			else if (!STB_StriCmp(szP, "eth4")) port = 4;
			else
			{
				MT_ERRLOG(0);
				return TBS_FAILED;
			}
		}
		
		if((szP=CLI_GetParamByName("vid"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
		else
		{
			vid = atoi(szP);
		}

		if( (vid<1)||(vid>4094))
		{
			IO_Print("\r\n\r\n  Parameter Error !");
			return CMM_FAILED;
		}
		if( CNU_SWITCH_TYPE_AR8236 != boardapi_getCnuSwitchType(cnu.col_model))
		{
			if( cli2cmm_readSwitchSettings(&rtl8306eSettings) != CMM_SUCCESS)
			{
				printf(" read CNU switch config error!\n");
			}
		}
		switch(port)
		{
			case 1:
			{
				profile.col_eth1vid = vid;
				rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[0].pvid = vid;
				profile.col_eth1VMode = 1;
				rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[0].egress_mode = 1;
				break;
			}
			case 2:
			{
				profile.col_eth2vid = vid;
				rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[1].pvid = vid;
				profile.col_eth2VMode = 1;
				rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[1].egress_mode = 1;
				break;
			}
			case 3:
			{
				profile.col_eth3vid = vid;
				rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[2].pvid = vid;
				profile.col_eth3VMode = 1;
				rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[2].egress_mode = 1;
				break;
			}
			case 4:
			{
				profile.col_eth4vid = vid;
				rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[3].pvid = vid;
				profile.col_eth4VMode = 1;
				rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[3].egress_mode = 1;
				break;
			}
			default:
			{
				IO_Print("\r\n\r\n  Parameter Error !");
				return CMM_FAILED;
			}
		}	
		profile.col_vlanSts = 1;
		rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_enable = 1;
		profile.id = cnuid;
		if( CNU_SWITCH_TYPE_AR8236 == boardapi_getCnuSwitchType(cnu.col_model))
			ret = cli2cmm_vlanConfig1(&profile);
		else
		{
			rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[4].pvid = 1;		
			rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[4].egress_mode = 2;
			rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_tag_aware = 1;
			rtl8306eSettings.rtl8306eConfig.vlanConfig.ingress_filter = 0;
			rtl8306eSettings.rtl8306eConfig.vlanConfig.g_admit_control = 0;
			rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[0].admit_control = 0;
			rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[1].admit_control = 0;
			rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[2].admit_control = 0;
			rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[3].admit_control = 0;
			rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[4].admit_control = 0;
			ret = cli2cmm_vlanConfig2(&rtl8306eSettings, &profile);
		}
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}		
	
	/* 不需要在此处写日志，在CMM 中进行*/
	if( CMM_SUCCESS == ret )
	{
		IO_Print("\r\n\r\n  Success !");
	}
	return (ret?CMM_FAILED:CMM_SUCCESS);
}

/*********************************************************************/
/* 函数功能 :undo vlan 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_UndoCnuVlanConfig()
{
	ULONG ret = CMM_SUCCESS;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t cltid = 0;
	uint16_t cnuid = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;
	rtl8306eWriteInfo rtl8306eSettings;
	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	cltid = CLI_GetCltTidByMode(iMode);
	cnuid = CLI_GetCnuTidByMode(iMode);
	rtl8306eSettings.node.clt = cltid;
	rtl8306eSettings.node.cnu = cnuid;
	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, cnuid, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, cnuid,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}

	if( CNU_SWITCH_TYPE_AR8236 != boardapi_getCnuSwitchType(cnu.col_model))
	{
		if( cli2cmm_readSwitchSettings(&rtl8306eSettings) != CMM_SUCCESS)
		{
			printf(" read CNU switch config error!\n");
		}
	}
	
	profile.col_vlanSts = 0;
	rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_enable = 0;
	profile.col_eth1vid = 1;
	profile.col_eth2vid = 1;
	profile.col_eth3vid = 1;
	profile.col_eth4vid = 1;
	rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[0].pvid = 1;
	rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[0].egress_mode = 3;
	rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[1].pvid = 1;
	rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[1].egress_mode = 3;
	rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[2].pvid = 1;
	rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[2].egress_mode = 3;
	rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[3].pvid = 1;
	rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[3].egress_mode = 3;
	rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[4].pvid = 1;		
	rtl8306eSettings.rtl8306eConfig.vlanConfig.vlan_port[4].egress_mode = 3;
	
	if( CNU_SWITCH_TYPE_AR8236 == boardapi_getCnuSwitchType(cnu.col_model))
		ret = cli2cmm_vlanConfig1(&profile);	
	else
		ret = cli2cmm_vlanConfig2(&rtl8306eSettings, &profile);
	
	/* 不需要在此处写日志，在CMM 中进行*/
	if( CMM_SUCCESS == ret )
	{
		IO_Print("\r\n\r\n  Success !");
	}
	return (ret?CMM_FAILED:CMM_SUCCESS);
}


/*********************************************************************/
/* 函数功能 :flow-control 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_DoRateLimiting()
{	
	char  *szP=NULL;
	//char *pParam;
	ULONG ret = CMM_SUCCESS;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t cltid = 0;
	uint16_t cnuid = 0;
	uint16_t port = 1;
	uint32_t rate = 0;
	uint16_t unit = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;
	rtl8306eWriteInfo rtl8306eSettings;
	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	cltid = CLI_GetCltTidByMode(iMode);
	cnuid = CLI_GetCnuTidByMode(iMode);
	rtl8306eSettings.node.clt = cltid;
	rtl8306eSettings.node.cnu = cnuid;
	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, cnuid, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_FAILED;
	}

	/* 设置前先读取线路配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, cnuid, &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_FAILED;
	}

	/* 读取命令行参数*/
	/* 读取端口信息*/
	if((szP=CLI_GetParamByName("port"))==NULL)
	{
		MT_ERRLOG(0);
		return CMM_FAILED;
	}
	else
	{
		if(!STB_StriCmp(szP, "cpu-port"))
		{
			port = 0;
		}
		else if(!STB_StriCmp(szP, "eth1"))
		{				
			port = 1;
		}
		else if(!STB_StriCmp(szP, "eth2"))
		{				
			port = 2;
		}
		else if(!STB_StriCmp(szP, "eth3"))
		{				
			port = 3;
		}
		else if(!STB_StriCmp(szP, "eth4"))
		{				
			port = 4;
		}
		else
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
	}

	/* 读取uplink 速率单位*/
	if((szP=CLI_GetParamByName("upunit"))==NULL)
	{
		MT_ERRLOG(0);
		return CMM_FAILED;
	}
	else
	{
		if (!STB_StriCmp(szP, "Kb"))
		{
			unit = 0;
		}
		else if (!STB_StriCmp(szP, "Mb"))
		{
			unit = 1;
		}
		else
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
	}

	/* 读取uplink 端口速率并转化*/
	if((szP=CLI_GetParamByName("uprate"))==NULL)
	{
		MT_ERRLOG(0);
		return CMM_FAILED;
	}
	else
	{
		if(unit)
		{
			rate = atoi(szP) * 1024;
		}
		else
		{
			rate = atoi(szP);
		}
		/* 判断uplink 速率是否有效*/
		if( 0 != rate%32 )
		{
			IO_Print("\r\n\r\n  Rate must be times of 32Kb.");
			return TBS_FAILED;
		}
		else if( (rate < 0)||(rate > 100*1024) )
		{
			IO_Print("\r\n\r\n  Rate must be 0Kb~100Mb.");
			return TBS_FAILED;
		}
	}

	if( CNU_SWITCH_TYPE_AR8236 != boardapi_getCnuSwitchType(cnu.col_model))
	{
		if( cli2cmm_readSwitchSettings(&rtl8306eSettings) != CMM_SUCCESS)
		{
			printf(" read CNU switch config error!\n");
		}
	}

	/* 将上行限速参数写入数据结构*/
	switch(port)
	{
		/* cpu-port */
		case 0:
		{
			profile.col_cpuPortTxRate = rate;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[4].bandwidth_control_enable = 1;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[4].bandwidth_value = rate/64;
			break;
		}
		/* ETH1 */
		case 1:
		{
			profile.col_eth1rx = rate;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[0].bandwidth_control_enable = 1;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[0].bandwidth_value = rate/64;
			break;
		}
		/* ETH2 */
		case 2:
		{
			profile.col_eth2rx = rate;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[1].bandwidth_control_enable = 1;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[1].bandwidth_value = rate/64;
			break;
		}
		/* ETH3 */
		case 3:
		{
			profile.col_eth3rx = rate;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[2].bandwidth_control_enable = 1;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[2].bandwidth_value = rate/64;
			break;
		}
		/* ETH4 */
		case 4:
		{
			profile.col_eth4rx = rate;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[3].bandwidth_control_enable = 1;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[3].bandwidth_value = rate/64;
			break;
		}
		default:
		{
			MT_ERRLOG(0);
			return CMM_FAILED;
		}
	}

	/* 读取downlink 速率单位*/
	if((szP=CLI_GetParamByName("dwunit"))==NULL)
	{
		MT_ERRLOG(0);
		return CMM_FAILED;
	}
	else
	{
		if (!STB_StriCmp(szP, "Kb"))
		{
			unit = 0;
		}
		else if (!STB_StriCmp(szP, "Mb"))
		{
			unit = 1;
		}
		else
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
	}

	/* 读取downlink 端口速率并转化*/
	if((szP=CLI_GetParamByName("dwrate"))==NULL)
	{
		MT_ERRLOG(0);
		return CMM_FAILED;
	}
	else
	{
		if(unit)
		{
			rate = atoi(szP) * 1024;
		}
		else
		{
			rate = atoi(szP);
		}
		/* 判断downlink 速率是否有效*/
		if( 0 != rate%32 )
		{
			IO_Print("\r\n\r\n  Rate must be times of 32Kb.");
			return TBS_FAILED;
		}
		else if( (rate < 0)||(rate > 100*1024) )
		{
			IO_Print("\r\n\r\n  Rate must be 0Kb~100Mb.");
			return TBS_FAILED;
		}
	}

	/* 将下行限速参数写入数据结构*/
	switch(port)
	{
		/* cpu-port */
		case 0:
		{
			profile.col_cpuPortRxRate = rate;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[4].bandwidth_control_enable = 1;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[4].bandwidth_value = rate/64;
			break;
		}
		/* ETH1 */
		case 1:
		{
			profile.col_eth1tx = rate;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[0].bandwidth_control_enable = 1;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[0].bandwidth_value = rate/64;
			break;
		}
		/* ETH2 */
		case 2:
		{
			profile.col_eth2tx = rate;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[1].bandwidth_control_enable = 1;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[1].bandwidth_value = rate/64;
			break;
		}
		/* ETH3 */
		case 3:
		{
			profile.col_eth3tx = rate;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[3].bandwidth_control_enable = 1;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[2].bandwidth_value = rate/64;
			break;
		}
		/* ETH4 */
		case 4:
		{
			profile.col_eth4tx = rate;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[3].bandwidth_control_enable = 1;
			rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[3].bandwidth_value = rate/64;
			break;
		}
		default:
		{
			MT_ERRLOG(0);
			return CMM_FAILED;
		}
	}

	profile.col_rxLimitSts = 1;
	profile.col_txLimitSts = 1;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.g_rx_bandwidth_control_enable = 1;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.g_tx_bandwidth_control_enable = 1;
	if( CNU_SWITCH_TYPE_AR8236 == boardapi_getCnuSwitchType(cnu.col_model))
		ret = cli2cmm_rateLimitConfig1(&profile);
	else
		ret = cli2cmm_rateLimitConfig2(&rtl8306eSettings, &profile);
	/* 不需要在此处写日志，在CMM 中进行*/
	if( CMM_SUCCESS == ret )
	{
		IO_Print("\r\n\r\n  Success !");
	}
	return (ret?CMM_FAILED:CMM_SUCCESS);
}

/*********************************************************************/
/* 函数功能 :vlan 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_UndoRateLimiting()
{
	ULONG ret = CMM_SUCCESS;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t cltid = 0;
	uint16_t cnuid = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;
	rtl8306eWriteInfo rtl8306eSettings;
	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	cltid = CLI_GetCltTidByMode(iMode);
	cnuid = CLI_GetCnuTidByMode(iMode);
	rtl8306eSettings.node.clt = cltid;
	rtl8306eSettings.node.cnu = cnuid;
	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, cnuid, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, cnuid,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}

	profile.col_rxLimitSts = 0;
	profile.col_txLimitSts = 0;
	profile.col_cpuPortRxRate = 0;
	profile.col_cpuPortTxRate = 0;
	profile.col_eth1rx = 0;
	profile.col_eth1tx = 0;
	profile.col_eth2rx = 0;
	profile.col_eth2tx = 0;
	profile.col_eth3rx = 0;
	profile.col_eth3tx = 0;
	profile.col_eth4rx = 0;
	profile.col_eth4tx = 0;

	if( CNU_SWITCH_TYPE_AR8236 != boardapi_getCnuSwitchType(cnu.col_model))
	{
		if( cli2cmm_readSwitchSettings(&rtl8306eSettings) != CMM_SUCCESS)
		{
			printf(" read CNU switch config error!\n");
		}
	}
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.g_rx_bandwidth_control_enable = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.g_tx_bandwidth_control_enable = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[0].bandwidth_control_enable = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[1].bandwidth_control_enable = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[2].bandwidth_control_enable = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[3].bandwidth_control_enable = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[4].bandwidth_control_enable = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[0].bandwidth_control_enable = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[1].bandwidth_control_enable = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[2].bandwidth_control_enable = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[3].bandwidth_control_enable = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[4].bandwidth_control_enable = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[4].bandwidth_value = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[0].bandwidth_value = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[1].bandwidth_value = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[2].bandwidth_value = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[3].bandwidth_value = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.rxPort[4].bandwidth_value = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[0].bandwidth_value = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[1].bandwidth_value = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[2].bandwidth_value = 0;
	rtl8306eSettings.rtl8306eConfig.bandwidthConfig.txPort[3].bandwidth_value = 0;
	
	if( CNU_SWITCH_TYPE_AR8236 == boardapi_getCnuSwitchType(cnu.col_model))
		ret = cli2cmm_rateLimitConfig1(&profile);
	else
		ret = cli2cmm_rateLimitConfig2(&rtl8306eSettings, &profile);
	/* 不需要在此处写日志，在CMM 中进行*/
	if( CMM_SUCCESS == ret )
	{
		IO_Print("\r\n\r\n  Success !");
	}
	return (ret?CMM_FAILED:CMM_SUCCESS);
}

/*********************************************************************/
/* 函数功能 :strom-control 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_DoStromFilter()
{	
	char *pParam = NULL;
	ULONG ret = CMM_SUCCESS;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t cltid = 0;
	uint16_t cnuid = 0;
	uint16_t enable = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;
	rtl8306eWriteInfo rtl8306eSettings;
	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	cltid = CLI_GetCltTidByMode(iMode);
	cnuid = CLI_GetCnuTidByMode(iMode);
	rtl8306eSettings.node.clt = cltid;
	rtl8306eSettings.node.cnu = cnuid;
	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, cnuid, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, cnuid,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}

	/* 获取命令行参数*/
	if((pParam=CLI_GetParamByName("enable|disable"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	if(!STB_StriCmp(pParam, "enable"))
	{
		enable = 1;		
	}
	else
	{
		enable = 0;
	}
	
	if((pParam=CLI_GetParamByName("broadcast|unknown-unicast|unknown-multicast"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}

	if( CNU_SWITCH_TYPE_AR8236 != boardapi_getCnuSwitchType(cnu.col_model))
	{
		if( cli2cmm_readSwitchSettings(&rtl8306eSettings) != CMM_SUCCESS)
		{
			printf(" read CNU switch config error!\n");
		}
	}
	
	if(!STB_StriCmp(pParam, "broadcast"))
	{
		profile.col_sfbSts = enable;
		rtl8306eSettings.rtl8306eConfig.stormFilter.disable_broadcast = !enable;
	}
	else if(!STB_StriCmp(pParam, "unknown-unicast"))
	{
		profile.col_sfuSts = enable;
		rtl8306eSettings.rtl8306eConfig.stormFilter.disable_unknown = !enable;
	}
	else
	{
		profile.col_sfmSts = enable;
		rtl8306eSettings.rtl8306eConfig.stormFilter.disable_multicast = !enable;
	}

	/* 只要3类风暴抑制中任何一类启用，则抑制等级设置为1 */
	if(profile.col_sfbSts|profile.col_sfuSts|profile.col_sfmSts)
	{
		profile.col_sfRate = 1;
	}
	else
	{
		profile.col_sfRate = 0;
	}
		
	if( CNU_SWITCH_TYPE_AR8236 == boardapi_getCnuSwitchType(cnu.col_model))
		ret = cli2cmm_stormFilterConfig1(&profile);
	else
		ret = cli2cmm_stormFilterConfig2(&rtl8306eSettings, &profile);
	/* 不需要在此处写日志，在CMM 中进行*/
	if( CMM_SUCCESS == ret )
	{
		IO_Print("\r\n\r\n  Success !");
	}
	return (ret?CMM_FAILED:CMM_SUCCESS);
}

/*********************************************************************/
/* 函数功能 :vlan 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_UndoStromFilter()
{
	ULONG ret = CMM_SUCCESS;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t cltid = 0;
	uint16_t cnuid = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;
	rtl8306eWriteInfo rtl8306eSettings;
	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	cnuid = CLI_GetCnuTidByMode(iMode);
	cltid = CLI_GetCltTidByMode(iMode);
	rtl8306eSettings.node.clt = cltid;
	rtl8306eSettings.node.cnu = cnuid;
	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, cnuid, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, cnuid,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}

	profile.col_sfbSts = 0;
	profile.col_sfmSts = 0;
	profile.col_sfuSts = 0;
	profile.col_sfRate = 0;

	if( CNU_SWITCH_TYPE_AR8236 != boardapi_getCnuSwitchType(cnu.col_model))
	{
		if( cli2cmm_readSwitchSettings(&rtl8306eSettings) != CMM_SUCCESS)
		{
			printf(" read CNU switch config error!\n");
		}
	}
	rtl8306eSettings.rtl8306eConfig.stormFilter.disable_broadcast = 1;
	rtl8306eSettings.rtl8306eConfig.stormFilter.disable_multicast = 1;
	rtl8306eSettings.rtl8306eConfig.stormFilter.disable_unknown = 1;
	
	if( CNU_SWITCH_TYPE_AR8236 == boardapi_getCnuSwitchType(cnu.col_model))
		ret = cli2cmm_stormFilterConfig1(&profile);
	else
		ret = cli2cmm_stormFilterConfig2(&rtl8306eSettings, &profile);
	/* 不需要在此处写日志，在CMM 中进行*/
	if( CMM_SUCCESS == ret )
	{
		IO_Print("\r\n\r\n  Success !");
	}
	return (ret?CMM_FAILED:CMM_SUCCESS);
}

/*********************************************************************/
/* 函数功能 :strom-control 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_DoDsdtRgmiiTimingDelay()
{	
	char *pParam = NULL;
	ULONG ret = CMM_SUCCESS;
	uint16_t mycmd = 0;
	uint16_t portid = 0;
	uint16_t direction = 0;
	st_dsdtRgmiiTimingDelay mydelay;

	/* 获取命令行参数*/
	if((pParam=CLI_GetParamByName("get|enable|disable"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	else
	{
		if(!STB_StriCmp(pParam, "get"))
		{
			mycmd = 0;		
		}
		else if(!STB_StriCmp(pParam, "enable"))
		{
			mycmd = 1;
		}
		else
		{
			mycmd = 2;
		}
	}	
	
	if((pParam=CLI_GetParamByName("port5|port6"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	else
	{
		if(!STB_StriCmp(pParam, "port5"))
		{
			portid = 5;
		}
		else
		{
			portid = 6;
		}
	}

	if((pParam=CLI_GetParamByName("rx|tx|all"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	else
	{
		if(!STB_StriCmp(pParam, "rx"))
		{
			direction = 0;		
		}
		else if(!STB_StriCmp(pParam, "tx"))
		{
			direction = 1;
		}
		else
		{
			direction = 2;
		}
	}
	
	mydelay.port = portid;
	
	ret = cli2cmm_getRgmiiTimingDelay(&mydelay);
	
	if( CMM_SUCCESS == ret )
	{
		if( 0 == mycmd )	/* read */
		{			
			if( 0 == direction )
			{
				if( mydelay.rxdelay == 0 )
				{
					IO_Print("\r\n\r\n  RGMII RX delay:	disabled");
				}
				else
				{
					IO_Print("\r\n\r\n  RGMII RX delay:	enable");
				}
			}
			else if( 1 == direction )
			{
				if( mydelay.txdelay == 0 )
				{
					IO_Print("\r\n\r\n  RGMII TX delay:	disabled");
				}
				else
				{
					IO_Print("\r\n\r\n  RGMII TX delay:	enable");
				}
			}
			else
			{
				if( mydelay.rxdelay == 0 )
				{
					IO_Print("\r\n\r\n  RGMII RX delay:	disabled");
				}
				else
				{
					IO_Print("\r\n\r\n  RGMII RX delay:	enable");
				}
				if( mydelay.txdelay == 0 )
				{
					IO_Print("\r\n  RGMII TX delay:	disabled");
				}
				else
				{
					IO_Print("\r\n  RGMII TX delay:	enable");
				}
			}
		}
		else			/* set */
		{			
			if( 0 == direction )
			{
				if( 1 == mycmd )
				{
					mydelay.rxdelay = 1;
				}
				else
				{
					mydelay.rxdelay = 0;
				}
			}
			else if( 1 == direction )
			{
				if( 1 == mycmd )
				{
					mydelay.txdelay = 1;
				}
				else
				{
					mydelay.txdelay = 0;
				}
			}
			else
			{
				if( 1 == mycmd )
				{
					mydelay.rxdelay = 1;
					mydelay.txdelay = 1;
				}
				else
				{
					mydelay.rxdelay = 0;
					mydelay.txdelay = 0;
				}
			}
			ret = cli2cmm_setRgmiiTimingDelay(&mydelay);
			if( CMM_SUCCESS == ret )
			{
				IO_Print("\r\n\r\n  Success !");
			}
		}
	}	
	
	return (ret?CMM_FAILED:CMM_SUCCESS);
}

int __getCnuPortStatus(uint16_t id, uint16_t port)
{
	st_dbsCnu cnu;
	st_dbsProfile profile;

	if( (id < 1)||(id > MAX_CLT_AMOUNT_LIMIT*MAX_CNUS_PER_CLT) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return 0;
	}

	if( port > 4 )
	{
		IO_Print("\r\n\r\n  System Error !");
		return 0;
	}

	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return 0;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		return 0;
	}

	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return 0;
	}	
	else if( 0 == profile.col_row_sts )
	{
		return 0;
	}

	if( 0 == profile.col_psctlSts )
	{
		return 1;
	}
	else
	{
		switch(port)
		{
			case 0:
			{
				return profile.col_cpuPortSts?1:0;
			}
			case 1:
			{
				return profile.col_eth1sts?1:0;
			}
			case 2:
			{
				return profile.col_eth2sts?1:0;
			}
			case 3:
			{
				return profile.col_eth3sts?1:0;
			}
			case 4:
			{
				return profile.col_eth4sts?1:0;
			}
			default:
			{
				IO_Print("\r\n\r\n  System Error !");
				return 0;
			}
		}
	}
}

/*********************************************************************/
/* 函数功能 :shutdown 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_Shutdown()
{	
	char  *szP=NULL;
	ULONG ret = CMM_SUCCESS;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	id = CLI_GetCnuTidByMode(iMode);

	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}

	/* 读取命令行参数*/
	if((szP=CLI_GetParamByName("port"))==NULL)
	{
		MT_ERRLOG(0);
		return CMM_FAILED;
	}
	else
	{
		if(!STB_StriCmp(szP, "eth1"))
		{				
			profile.col_eth1sts = 0;
			profile.col_cpuPortSts = __getCnuPortStatus(profile.id, 0);
			profile.col_eth2sts = __getCnuPortStatus(profile.id, 2);
			profile.col_eth3sts = __getCnuPortStatus(profile.id, 3);
			profile.col_eth4sts = __getCnuPortStatus(profile.id, 4);			
		}
		else if(!STB_StriCmp(szP, "eth2"))
		{				
			profile.col_eth2sts = 0;
			profile.col_cpuPortSts = __getCnuPortStatus(profile.id, 0);
			profile.col_eth1sts = __getCnuPortStatus(profile.id, 1);
			profile.col_eth3sts = __getCnuPortStatus(profile.id, 3);
			profile.col_eth4sts = __getCnuPortStatus(profile.id, 4);	
		}
		else if(!STB_StriCmp(szP, "eth3"))
		{				
			profile.col_eth3sts = 0;
			profile.col_cpuPortSts = __getCnuPortStatus(profile.id, 0);
			profile.col_eth1sts = __getCnuPortStatus(profile.id, 1);
			profile.col_eth2sts = __getCnuPortStatus(profile.id, 2);
			profile.col_eth4sts = __getCnuPortStatus(profile.id, 4);	
		}
		else if(!STB_StriCmp(szP, "eth4"))
		{				
			profile.col_eth4sts = 0;
			profile.col_cpuPortSts = __getCnuPortStatus(profile.id, 0);
			profile.col_eth1sts = __getCnuPortStatus(profile.id, 1);
			profile.col_eth2sts = __getCnuPortStatus(profile.id, 2);
			profile.col_eth3sts = __getCnuPortStatus(profile.id, 3);	
		}
		else
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
	}
	
	profile.col_psctlSts = 1;
	
	ret = cli2cmm_shutdownConfig(&profile);
	/* 不需要在此处写日志，在CMM 中进行*/
	if( CMM_SUCCESS == ret )
	{
		IO_Print("\r\n\r\n  Success !");
	}
	return (ret?CMM_FAILED:CMM_SUCCESS);
}

/*********************************************************************/
/* 函数功能 :vlan 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_UndoShutdown()
{
	ULONG ret = CMM_SUCCESS;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	id = CLI_GetCnuTidByMode(iMode);

	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}

	profile.col_cpuPortSts = __getCnuPortStatus(profile.id, 0);
	profile.col_eth1sts = 1;
	profile.col_eth2sts = 1;
	profile.col_eth3sts = 1;
	profile.col_eth4sts = 1;
	
	profile.col_psctlSts = 1;

	ret = cli2cmm_shutdownConfig(&profile);
	/* 不需要在此处写日志，在CMM 中进行*/
	if( CMM_SUCCESS == ret )
	{
		IO_Print("\r\n\r\n  Success !");
	}
	return (ret?CMM_FAILED:CMM_SUCCESS);
}

/*********************************************************************/
/* 函数功能 :MacLimit 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_MacLimit()
{	
	char  *szP=NULL;
	ULONG ret = CMM_SUCCESS;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t cltid = 0;
	uint16_t cnuid = 0;
	uint32_t n = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;
	rtl8306eWriteInfo rtl8306eSettings;
	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	cltid = CLI_GetCltTidByMode(iMode);
	cnuid = CLI_GetCnuTidByMode(iMode);
	rtl8306eSettings.node.clt = cltid;
	rtl8306eSettings.node.cnu = cnuid;
	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, cnuid, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, cnuid,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}

	if( CNU_SWITCH_TYPE_AR8236 != boardapi_getCnuSwitchType(cnu.col_model))
	{
		if( cli2cmm_readSwitchSettings(&rtl8306eSettings) != CMM_SUCCESS)
		{
			printf(" read CNU switch config error!\n");
		}
	}

	/* 读取命令行参数*/
	if((szP=CLI_GetParamByName("hosts"))==NULL)
	{
		MT_ERRLOG(0);
		return CMM_FAILED;
	}
	else
	{
		n = atoi(szP);
		if( 0 == n )
		{
			profile.col_macLimit = 65;
		}
		else
		{
			profile.col_macLimit = n;
			rtl8306eSettings.rtl8306eConfig.macLimit.system.thresholt = n;
		}
	}
	rtl8306eSettings.rtl8306eConfig.macLimit.system.enable = 1;
	rtl8306eSettings.rtl8306eConfig.macLimit.action = 0;
	
	if( CNU_SWITCH_TYPE_AR8236 == boardapi_getCnuSwitchType(cnu.col_model))
		ret = cli2cmm_macLimitConfig1(&profile);
	else
		ret = cli2cmm_macLimitConfig2(&rtl8306eSettings, &profile);
	/* 不需要在此处写日志，在CMM 中进行*/
	if( CMM_SUCCESS == ret )
	{
		IO_Print("\r\n\r\n  Success !");
	}
	return (ret?CMM_FAILED:CMM_SUCCESS);
}

/*********************************************************************/
/* 函数功能 :UndoMacLimit 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_UndoMacLimit()
{
	ULONG ret = CMM_SUCCESS;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t cltid = 0;
	uint16_t cnuid = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;
	rtl8306eWriteInfo rtl8306eSettings;
	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	cltid = CLI_GetCltTidByMode(iMode);
	cnuid = CLI_GetCnuTidByMode(iMode);
	rtl8306eSettings.node.clt = cltid;
	rtl8306eSettings.node.cnu = cnuid;
	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, cnuid, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, cnuid,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}

	if( CNU_SWITCH_TYPE_AR8236 != boardapi_getCnuSwitchType(cnu.col_model))
	{
		if( cli2cmm_readSwitchSettings(&rtl8306eSettings) != CMM_SUCCESS)
		{
			printf(" read CNU switch config error!\n");
		}
	}

	profile.col_macLimit = 0;
	rtl8306eSettings.rtl8306eConfig.macLimit.action = 0;
	rtl8306eSettings.rtl8306eConfig.macLimit.system.thresholt = 0;
	rtl8306eSettings.rtl8306eConfig.macLimit.system.enable = 0;
	
	if( CNU_SWITCH_TYPE_AR8236 == boardapi_getCnuSwitchType(cnu.col_model))
		ret = cli2cmm_macLimitConfig1(&profile);
	else
		ret = cli2cmm_macLimitConfig2(&rtl8306eSettings, &profile);
	/* 不需要在此处写日志，在CMM 中进行*/
	if( CMM_SUCCESS == ret )
	{
		IO_Print("\r\n\r\n  Success !");
	}
	return (ret?CMM_FAILED:CMM_SUCCESS);
}

/*********************************************************************/
/* 函数功能 :DoAgingTimeConfig 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_DoAgingTimeConfig()
{	
	char  *szP=NULL;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;
	st_dbsClt clt;
	st_dbsCltConf profile;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CLT模式下执行*/
	if( 1 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface clt 模式*//* 获取该CLT 的索引号码*/
	id = CLI_GetCltTidByMode(iMode);

	/* 如果该CLT 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetClt(dbsdev, id, &clt) )
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_DO_AGING_TIME_CONFIG, CMM_FAILED);
		return CMM_FAILED;
	}	
	else if( 0 == clt.col_row_sts )
	{
		IO_Print("\r\n\r\n  CLT Interface Unreachable !");
		__clt_opt_log(CMM_CLI_DO_AGING_TIME_CONFIG, CMM_FAILED);
		return CMM_FAILED;
	}

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCltconf(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_DO_AGING_TIME_CONFIG, CMM_FAILED);
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CLT Profile Unreachable !");
		__clt_opt_log(CMM_CLI_DO_AGING_TIME_CONFIG, CMM_FAILED);
		return CMM_FAILED;
	}
	
	/* 读取命令行参数*/
	if((szP=CLI_GetParamByName("local|remote"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	if (!STB_StriCmp(szP, "local"))
	{
		if((szP=CLI_GetParamByName("time"))==NULL)
		{
			MT_ERRLOG(0);
			return CMM_FAILED;
		}
		else
		{
			profile.col_loagTime = atoi(szP);
		}
	}
	else if (!STB_StriCmp(szP, "remote"))
	{
		if((szP=CLI_GetParamByName("time"))==NULL)
		{
			MT_ERRLOG(0);
			return CMM_FAILED;
		}
		else
		{
			profile.col_reagTime = atoi(szP);
		}	
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}	
	
	if( CMM_SUCCESS != dbsUpdateCltconf(dbsdev, id, &profile) )
	{
		/* 写日志*/
		__clt_opt_log(CMM_CLI_DO_AGING_TIME_CONFIG, CMM_FAILED);
		return CMM_FAILED;	
	}
	else
	{
		/* 写日志*/
		__clt_opt_log(CMM_CLI_DO_AGING_TIME_CONFIG, CMM_SUCCESS);
		return CMM_SUCCESS;	
	}
}

/*********************************************************************/
/* 函数功能 :UndoAgingTimeConfig 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_UndoAgingTimeConfig()
{
	//ULONG ret = CMM_SUCCESS;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;
	st_dbsClt clt;
	st_dbsCltConf profile;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CLT模式下执行*/
	if( 1 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface clt 模式*//* 获取该CLT 的索引号码*/
	id = CLI_GetCltTidByMode(iMode);

	/* 如果该CLT 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetClt(dbsdev, id, &clt) )
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_DO_AGING_TIME_CONFIG, CMM_FAILED);
		return CMM_FAILED;
	}	
	else if( 0 == clt.col_row_sts )
	{
		IO_Print("\r\n\r\n  CLT Interface Unreachable !");
		__clt_opt_log(CMM_CLI_DO_AGING_TIME_CONFIG, CMM_FAILED);
		return CMM_FAILED;
	}

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCltconf(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		__clt_opt_log(CMM_CLI_DO_AGING_TIME_CONFIG, CMM_FAILED);
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CLT Profile Unreachable !");
		__clt_opt_log(CMM_CLI_DO_AGING_TIME_CONFIG, CMM_FAILED);
		return CMM_FAILED;
	}

	profile.col_loagTime = 15;
	profile.col_reagTime = 4;

	if( CMM_SUCCESS != dbsUpdateCltconf(dbsdev, id, &profile) )
	{
		/* 写日志*/
		__clt_opt_log(CMM_CLI_DO_AGING_TIME_CONFIG, CMM_FAILED);
		return CMM_FAILED;	
	}
	else
	{
		/* 写日志*/
		__clt_opt_log(CMM_CLI_DO_AGING_TIME_CONFIG, CMM_SUCCESS);
		return CMM_SUCCESS;	
	}
}

int __cmd_set_clt_qos_mode(uint16_t cltid, uint16_t mode)
{
	st_dbsClt clt;
	st_dbsCltConf profile;
	uint16_t id = cltid;

	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetClt(dbsdev, id, &clt) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == clt.col_row_sts )
	{
		IO_Print("\r\n\r\n  CLT Interface Unreachable !");
		return CMM_FAILED;
	}
	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCltconf(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CLT Profile Unreachable !");
		return CMM_FAILED;
	}
	/* 选择QoS  模式*/
	switch(mode)
	{
		case 0:
		{
			/* cos */
			profile.col_tbaPriSts = 1;
			profile.col_cosPriSts = 1;
			profile.col_tosPriSts = 0;
			break; 
		}
		case 1:
		{
			/* tos */
			profile.col_tbaPriSts = 1;
			profile.col_cosPriSts = 0;
			profile.col_tosPriSts = 1;
			break; 
		}
		default:
		{
			return CMM_FAILED;
		}
	}
	/* for debug */
	//IO_Print("\r\n\r\n  __cmd_set_clt_qos_mode(%d, %d)", id, mode);
	return dbsUpdateCltconf(dbsdev, id, &profile);
}

int __cmd_set_cnu_qos_mode(uint16_t cltid, uint16_t cnuid, uint16_t mode)
{
	st_dbsCnu cnu;
	st_dbsProfile profile;
	uint16_t id = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;

	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_FAILED;
	}
	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_FAILED;
	}
	/* 选择QoS  模式*/
	switch(mode)
	{
		case 0:
		{
			/* cos */
			profile.col_tbaPriSts = 1;
			profile.col_cosPriSts = 1;
			profile.col_tosPriSts = 0;
			break;
		}
		case 1:
		{
			/* tos */
			profile.col_tbaPriSts = 1;
			profile.col_cosPriSts = 0;
			profile.col_tosPriSts = 1;
			break; 
		}
		default:
		{
			return CMM_FAILED;
		}
	}
	/* for debug */
	//IO_Print("\r\n\r\n  __cmd_set_cnu_qos_mode(%d, %d)", id, mode);
	return dbsUpdateProfile(dbsdev, id, &profile);
}

/*********************************************************************/
/* 函数功能 :SelectQoSType 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_SelectQoSType()
{
	char  *szP=NULL;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;
	ULONG ret = CMM_SUCCESS;
	uint16_t qosMode = 0;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 读取命令行参数*/
	if((szP=CLI_GetParamByName("base"))==NULL)
	{
		MT_ERRLOG(0);
		return CMM_FAILED;
	}
	else
	{
		if(!STB_StriCmp(szP, "cos"))
		{				
			qosMode = 0;
		}
		else if(!STB_StriCmp(szP, "tos"))
		{				
			qosMode = 1;
		}
		else
		{
			MT_ERRLOG(0);
			return CMM_FAILED;
		}
	}

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CLT/CNU模式下执行*/
	if( 1 == prevailMode )
	{
		/* interface clt 模式*/
		id = CLI_GetCltTidByMode(iMode);	
		ret = __cmd_set_clt_qos_mode(id, qosMode);
		/* 写日志*/
		__clt_opt_log(CMM_CLI_DO_QOS_CONFIG, ret);
		return ret;
	}
	else if( 2 == prevailMode )
	{
		/* interface cnu 模式*/
		id = CLI_GetCnuTidByMode(iMode);
		ret = __cmd_set_cnu_qos_mode(1, id, qosMode);
		/* 写日志*/
		__clt_opt_log(CMM_CLI_DO_QOS_CONFIG, ret);
		return ret;
	}
	else
	{
		/* 全局模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
}

int __cmd_clt_cos_map(uint16_t cltid, uint8_t pri, uint8_t cap)
{
	DB_INTEGER_V iValue;
	uint16_t id = cltid;

	/* 如果该CLT 槽位无效则禁止配置*/
	iValue.ci.tbl = DBS_SYS_TBL_ID_CLT;
	iValue.ci.row = id;
	iValue.ci.col = DBS_SYS_TBL_CLT_COL_ID_ROWSTS;
	iValue.ci.colType = DBS_INTEGER;
	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iValue) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == iValue.integer )
	{
		IO_Print("\r\n\r\n  CLT Interface Unreachable !");
		return CMM_FAILED;
	}
	/* 如果该PROFILE 槽位无效则禁止配置*/
	iValue.ci.tbl = DBS_SYS_TBL_ID_CLTPRO;
	iValue.ci.row = id;
	iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_ROWSTS;
	iValue.ci.colType = DBS_INTEGER;
	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iValue) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == iValue.integer )
	{
		IO_Print("\r\n\r\n  CLT Profile Unreachable !");
		return CMM_FAILED;
	}
	/* cos queue mapping */
	iValue.ci.tbl = DBS_SYS_TBL_ID_CLTPRO;
	iValue.ci.row = id;
	iValue.ci.colType = DBS_INTEGER;
	iValue.len = sizeof(uint32_t);
	iValue.integer = cap;
	switch(pri)
	{
		case 0:
		{
			/* cos0 */			
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_COS0PRI;			
			break;
		}
		case 1:
		{
			/* cos1 */
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_COS1PRI;
			break;
		}
		case 2:
		{
			/* cos2 */
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_COS2PRI;
			break;
		}
		case 3:
		{
			/* cos3 */
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_COS3PRI;
			break;
		}
		case 4:
		{
			/* cos4 */
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_COS4PRI;
			break;
		}
		case 5:
		{
			/* cos5 */
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_COS5PRI;
			break;
		}
		case 6:
		{
			/* cos6 */
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_COS6PRI;
			break;
		}
		case 7:
		{
			/* cos7 */
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_COS7PRI;
			break;
		}		
		default:
		{
			return CMM_FAILED;
		}
	}
	/* for debug */
	//IO_Print("\r\n\r\n  __cmd_clt_cos_map(%d, %d, %d)", id, pri, cap);
	return dbsUpdateInteger(dbsdev, &iValue);	
}

int __cmd_cnu_cos_map(uint16_t cltid, uint16_t cnuid, uint8_t pri, uint8_t cap)
{
	DB_INTEGER_V iValue;
	uint16_t id = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;

	/* 如果该CNU 槽位无效则禁止配置*/
	iValue.ci.tbl = DBS_SYS_TBL_ID_CNU;
	iValue.ci.row = id;
	iValue.ci.col = DBS_SYS_TBL_CNU_COL_ID_ROWSTS;
	iValue.ci.colType = DBS_INTEGER;
	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iValue) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == iValue.integer )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_FAILED;
	}
	/* 如果该PROFILE 槽位无效则禁止配置*/
	iValue.ci.tbl = DBS_SYS_TBL_ID_CNUPRO;
	iValue.ci.row = id;
	iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_ROWSTS;
	iValue.ci.colType = DBS_INTEGER;
	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iValue) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == iValue.integer )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_FAILED;
	}
	/* cos queue mapping */
	iValue.ci.tbl = DBS_SYS_TBL_ID_CNUPRO;
	iValue.ci.row = id;
	iValue.ci.colType = DBS_INTEGER;
	iValue.len = sizeof(uint32_t);
	iValue.integer = cap;
	switch(pri)
	{
		case 0:
		{
			/* cos0 */			
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_COS0PRI;			
			break;
		}
		case 1:
		{
			/* cos1 */
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_COS1PRI;
			break;
		}
		case 2:
		{
			/* cos2 */
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_COS2PRI;
			break;
		}
		case 3:
		{
			/* cos3 */
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_COS3PRI;
			break;
		}
		case 4:
		{
			/* cos4 */
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_COS4PRI;
			break;
		}
		case 5:
		{
			/* cos5 */
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_COS5PRI;
			break;
		}
		case 6:
		{
			/* cos6 */
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_COS6PRI;
			break;
		}
		case 7:
		{
			/* cos7 */
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_COS7PRI;
			break;
		}		
		default:
		{
			return CMM_FAILED;
		}
	}
	/* for debug */
	//IO_Print("\r\n\r\n  __cmd_cnu_cos_map(%d, %d, %d)", id, pri, cap);
	return dbsUpdateInteger(dbsdev, &iValue);	
}

int __cmd_clt_tos_map(uint16_t cltid, uint8_t pri, uint8_t cap)
{
	DB_INTEGER_V iValue;
	uint16_t id = cltid;

	/* 如果该CLT 槽位无效则禁止配置*/
	iValue.ci.tbl = DBS_SYS_TBL_ID_CLT;
	iValue.ci.row = id;
	iValue.ci.col = DBS_SYS_TBL_CLT_COL_ID_ROWSTS;
	iValue.ci.colType = DBS_INTEGER;
	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iValue) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == iValue.integer )
	{
		IO_Print("\r\n\r\n  CLT Interface Unreachable !");
		return CMM_FAILED;
	}
	/* 如果该PROFILE 槽位无效则禁止配置*/
	iValue.ci.tbl = DBS_SYS_TBL_ID_CLTPRO;
	iValue.ci.row = id;
	iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_ROWSTS;
	iValue.ci.colType = DBS_INTEGER;
	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iValue) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == iValue.integer )
	{
		IO_Print("\r\n\r\n  CLT Profile Unreachable !");
		return CMM_FAILED;
	}
	/* tos queue mapping */
	iValue.ci.tbl = DBS_SYS_TBL_ID_CLTPRO;
	iValue.ci.row = id;
	iValue.ci.colType = DBS_INTEGER;
	iValue.len = sizeof(uint32_t);
	iValue.integer = cap;
	switch(pri)
	{
		case 0:
		{
			/* cos0 */			
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_TOS0PRI;			
			break;
		}
		case 1:
		{
			/* cos1 */
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_TOS1PRI;
			break;
		}
		case 2:
		{
			/* cos2 */
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_TOS2PRI;
			break;
		}
		case 3:
		{
			/* cos3 */
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_TOS3PRI;
			break;
		}
		case 4:
		{
			/* cos4 */
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_TOS4PRI;
			break;
		}
		case 5:
		{
			/* cos5 */
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_TOS5PRI;
			break;
		}
		case 6:
		{
			/* cos6 */
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_TOS6PRI;
			break;
		}
		case 7:
		{
			/* cos7 */
			iValue.ci.col = DBS_SYS_TBL_CLTPRO_COL_ID_TOS7PRI;
			break;
		}		
		default:
		{
			return CMM_FAILED;
		}
	}
	/* for debug */
	//IO_Print("\r\n\r\n  __cmd_clt_tos_map(%d, %d, %d)", id, pri, cap);
	return dbsUpdateInteger(dbsdev, &iValue);	
}

int __cmd_cnu_tos_map(uint16_t cltid, uint16_t cnuid, uint8_t pri, uint8_t cap)
{
	DB_INTEGER_V iValue;
	uint16_t id = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;

	/* 如果该CNU 槽位无效则禁止配置*/
	iValue.ci.tbl = DBS_SYS_TBL_ID_CNU;
	iValue.ci.row = id;
	iValue.ci.col = DBS_SYS_TBL_CNU_COL_ID_ROWSTS;
	iValue.ci.colType = DBS_INTEGER;
	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iValue) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == iValue.integer )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_FAILED;
	}
	/* 如果该PROFILE 槽位无效则禁止配置*/
	iValue.ci.tbl = DBS_SYS_TBL_ID_CNUPRO;
	iValue.ci.row = id;
	iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_ROWSTS;
	iValue.ci.colType = DBS_INTEGER;
	if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iValue) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == iValue.integer )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_FAILED;
	}
	/* tos queue mapping */
	iValue.ci.tbl = DBS_SYS_TBL_ID_CNUPRO;
	iValue.ci.row = id;
	iValue.ci.colType = DBS_INTEGER;
	iValue.len = sizeof(uint32_t);
	iValue.integer = cap;
	switch(pri)
	{
		case 0:
		{
			/* cos0 */			
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_TOS0PRI;			
			break;
		}
		case 1:
		{
			/* cos1 */
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_TOS1PRI;
			break;
		}
		case 2:
		{
			/* cos2 */
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_TOS2PRI;
			break;
		}
		case 3:
		{
			/* cos3 */
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_TOS3PRI;
			break;
		}
		case 4:
		{
			/* cos4 */
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_TOS4PRI;
			break;
		}
		case 5:
		{
			/* cos5 */
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_TOS5PRI;
			break;
		}
		case 6:
		{
			/* cos6 */
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_TOS6PRI;
			break;
		}
		case 7:
		{
			/* cos7 */
			iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_TOS7PRI;
			break;
		}		
		default:
		{
			return CMM_FAILED;
		}
	}
	/* for debug */
	//IO_Print("\r\n\r\n  __cmd_cnu_tos_map(%d, %d, %d)", id, pri, cap);
	return dbsUpdateInteger(dbsdev, &iValue);	
}

/*********************************************************************/
/* 函数功能 :reload profile 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_DoQosMap()
{
	char  *szP=NULL;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;
	ULONG ret = CMM_SUCCESS;
	uint8_t qType = 0;
	uint8_t bitValue = 0;
	uint8_t pri = 0;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 读取命令行参数*/
	if((szP=CLI_GetParamByName("cos|tos"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	if (!STB_StriCmp(szP, "cos"))
	{
		if((szP=CLI_GetParamByName("vlanpri"))==NULL)
		{
			MT_ERRLOG(0);
			return CMM_FAILED;
		}
		else
		{
			qType = 0;
			bitValue = atoi(szP);
		}
	}
	else if (!STB_StriCmp(szP, "tos"))
	{
		if((szP=CLI_GetParamByName("tc"))==NULL)
		{
			MT_ERRLOG(0);
			return CMM_FAILED;
		}
		else
		{
			qType = 1;
			bitValue = atoi(szP);
		}	
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	if((szP=CLI_GetParamByName("queue"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	else
	{
		pri = atoi(szP);
	}

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CLT/CNU模式下执行*/
	if( 1 == prevailMode )
	{
		/* interface clt 模式*/
		id = CLI_GetCltTidByMode(iMode);
		if( 0 == qType )
		{
			ret = __cmd_clt_cos_map(id, bitValue, pri);
		}
		else
		{
			ret = __cmd_clt_tos_map(id, bitValue, pri);
		}		
		/* 写日志*/
		__clt_opt_log(CMM_CLI_DO_QOS_CONFIG, ret);
		return ret;
	}
	else if( 2 == prevailMode )
	{
		/* interface cnu 模式*/
		id = CLI_GetCnuTidByMode(iMode);
		if( 0 == qType )
		{
			ret = __cmd_cnu_cos_map(1, id, bitValue, pri);
		}
		else
		{
			ret = __cmd_cnu_tos_map(1, id, bitValue, pri);
		}		
		/* 写日志*/
		__clt_opt_log(CMM_CLI_DO_QOS_CONFIG, ret);
		return ret;
	}
	else
	{
		/* 全局模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
}

int __cmd_undo_clt_qos(uint16_t cltid)
{
	st_dbsClt clt;
	st_dbsCltConf profile;
	uint16_t id = cltid;

	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetClt(dbsdev, id, &clt) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == clt.col_row_sts )
	{
		IO_Print("\r\n\r\n  CLT Interface Unreachable !");
		return CMM_FAILED;
	}
	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCltconf(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CLT Profile Unreachable !");
		return CMM_FAILED;
	}
	/* 写入默认的配置*/
	profile.col_tbaPriSts = 0;
	profile.col_cosPriSts = 1;
	profile.col_cos0pri = 1;
	profile.col_cos1pri = 0;
	profile.col_cos2pri = 0;
	profile.col_cos3pri = 1;
	profile.col_cos4pri = 2;
	profile.col_cos5pri = 2;
	profile.col_cos6pri = 3;
	profile.col_cos7pri = 3;
	profile.col_tosPriSts = 0;
	profile.col_tos0pri = 1;
	profile.col_tos1pri = 0;
	profile.col_tos2pri = 0;
	profile.col_tos3pri = 1;
	profile.col_tos4pri = 2;
	profile.col_tos5pri = 2;
	profile.col_tos6pri = 3;
	profile.col_tos7pri = 3;
	/* for debug */
	//IO_Print("\r\n\r\n  __cmd_undo_clt_qos(%d)", id);
	return dbsUpdateCltconf(dbsdev, id, &profile);
}

int __cmd_undo_cnu_qos(uint16_t cltid, uint16_t cnuid)
{
	st_dbsCnu cnu;
	st_dbsProfile profile;
	uint16_t id = (cltid-1)*MAX_CNUS_PER_CLT+cnuid;

	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_FAILED;
	}
	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_FAILED;
	}
	/* 写入默认的配置*/
	profile.col_tbaPriSts = 0;
	profile.col_cosPriSts = 1;
	profile.col_cos0pri = 1;
	profile.col_cos1pri = 0;
	profile.col_cos2pri = 0;
	profile.col_cos3pri = 1;
	profile.col_cos4pri = 2;
	profile.col_cos5pri = 2;
	profile.col_cos6pri = 3;
	profile.col_cos7pri = 3;
	profile.col_tosPriSts = 0;
	profile.col_tos0pri = 1;
	profile.col_tos1pri = 0;
	profile.col_tos2pri = 0;
	profile.col_tos3pri = 1;
	profile.col_tos4pri = 2;
	profile.col_tos5pri = 2;
	profile.col_tos6pri = 3;
	profile.col_tos7pri = 3;
	/* for debug */
	//IO_Print("\r\n\r\n  __cmd_undo_cnu_qos(%d)", id);
	return dbsUpdateProfile(dbsdev, id, &profile);
}

/*********************************************************************/
/* 函数功能 :reload profile 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_UndoQos()
{
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;
	ULONG ret = CMM_SUCCESS;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CLT/CNU模式下执行*/
	if( 1 == prevailMode )
	{
		/* interface clt 模式*/
		id = CLI_GetCltTidByMode(iMode);
		/*  要写日志*/
		ret = __cmd_undo_clt_qos(id);
		__clt_opt_log(CMM_CLI_DO_QOS_CONFIG, ret);
		return ret;
	}
	else if( 2 == prevailMode )
	{
		/* interface cnu 模式*/
		id = CLI_GetCnuTidByMode(iMode);
		/*  要写日志*/
		ret = __cmd_undo_cnu_qos(1, id);
		__clt_opt_log(CMM_CLI_DO_QOS_CONFIG, ret);
		return ret;
	}
	else
	{
		/* 全局模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}	
}

/*********************************************************************/
/* 函数功能 :CLI_CmdAr8236SmiReg 命令实现                                 */
/*********************************************************************/
ULONG CLI_CmdAr8236SmiReg()
{
	char  *szP=NULL;
	char *pParam;
	T_szAr8236Reg szAr8236Reg;

	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	id = CLI_GetCnuTidByMode(iMode);

	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}
	else if( 0 == cnu.col_sts )
	{
		IO_Print("\r\n\r\n	CNU Status Unreachable !");
		return CMM_SUCCESS;
	}

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}	

	szAr8236Reg.clt=1;
	szAr8236Reg.cnu=id;
	if((pParam=CLI_GetParamByName("read |write"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	if (!STB_StriCmp(pParam, "read"))
	{
		if((szP=CLI_GetParamByName("regad"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%x",&szAr8236Reg.mdioReg.reg);
		
		if( cli2cmm_readAr8236Reg(&szAr8236Reg) != TBS_SUCCESS )
		{
			return TBS_FAILED;
		}
		IO_Print("\r\n\r\n  Value:	%x",szAr8236Reg.mdioReg.value);
	}
	else if (!STB_StriCmp(pParam, "write"))
	{
		if((szP=CLI_GetParamByName("regvalue"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%x",&szAr8236Reg.mdioReg.value);

		if((szP=CLI_GetParamByName("regad"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%x",&szAr8236Reg.mdioReg.reg);
		
		if( cli2cmm_writeAr8236Reg(&szAr8236Reg) != TBS_SUCCESS )
		{
			return TBS_FAILED;
		}
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数功能 :CLI_CmdAr8236SmiPhy 命令实现                                 */
/*********************************************************************/
ULONG CLI_CmdAr8236SmiPhy()
{
	char  *szP=NULL;
	char *pParam;
	T_szAr8236Phy szAr8236Phy;

	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	id = CLI_GetCnuTidByMode(iMode);

	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}
	else if( 0 == cnu.col_sts )
	{
		IO_Print("\r\n\r\n  CNU Status Unreachable !");
		return CMM_SUCCESS;
	}

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}

	szAr8236Phy.clt=1;
	szAr8236Phy.cnu=id;
	if((pParam=CLI_GetParamByName("read |write"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	if (!STB_StriCmp(pParam, "read"))
	{
		if((szP=CLI_GetParamByName("phyad"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%x",&szAr8236Phy.mdioPhy.phy);

		if((szP=CLI_GetParamByName("regad"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%x",&szAr8236Phy.mdioPhy.reg);

		if( cli2cmm_readAr8236Phy(&szAr8236Phy) != TBS_SUCCESS )
		{
			return TBS_FAILED;
		}
		IO_Print("\r\n\r\n  Value:	%x",szAr8236Phy.mdioPhy.value);
	}
	else if (!STB_StriCmp(pParam, "write"))
	{
		if((szP=CLI_GetParamByName("regvalue"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%x",&szAr8236Phy.mdioPhy.value);

		if((szP=CLI_GetParamByName("phyad"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%x",&szAr8236Phy.mdioPhy.phy);
		
		if((szP=CLI_GetParamByName("regad"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%x",&szAr8236Phy.mdioPhy.reg);
		if( cli2cmm_writeAr8236Phy(&szAr8236Phy) != TBS_SUCCESS )
		{
			return TBS_FAILED;
		}
		
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	return TBS_SUCCESS;
}


/*********************************************************************/
/* 函数功能 :CLI_CmdCnuSwitch 命令实现                                 */
/*********************************************************************/
ULONG CLI_CmdCnuSwitch()
{
	char  *szP=NULL;
	char *pParam;
	T_szSwRtl8306eConfig rtl8306eSettings;

	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;
	uint16_t cltid;
	st_dbsCnu cnu;
	st_dbsProfile profile;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	id = CLI_GetCnuTidByMode(iMode);
	cltid = CLI_GetCltTidByMode(iMode);
	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}
	else if( 0 == cnu.col_sts )
	{
		IO_Print("\r\n\r\n  CNU Status Unreachable !");
		return CMM_SUCCESS;
	}

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}

	rtl8306eSettings.clt= cltid;
	rtl8306eSettings.cnu=id;
	if((pParam=CLI_GetParamByName("read |write"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	if (!STB_StriCmp(pParam, "read"))
	{
		if((szP=CLI_GetParamByName("phyad"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%d",&rtl8306eSettings.mdioInfo.phy);

		if((szP=CLI_GetParamByName("regad"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%d",&rtl8306eSettings.mdioInfo.reg);

		if((szP=CLI_GetParamByName("pageid"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%d",&rtl8306eSettings.mdioInfo.page);

		if( cli2cmm_readCnuSwitchRegister(&rtl8306eSettings) != TBS_SUCCESS )
		{
			IO_Print("\r\n\r\n  Read phy %d register %d page %d",rtl8306eSettings.mdioInfo.phy, rtl8306eSettings.mdioInfo.reg, rtl8306eSettings.mdioInfo.page);
			return TBS_FAILED;
		}
		IO_Print("\r\n\r\n  Value:	0x%04X",rtl8306eSettings.mdioInfo.value);
	}
	else if (!STB_StriCmp(pParam, "write"))
	{
		if((szP=CLI_GetParamByName("regvalue"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%x",&rtl8306eSettings.mdioInfo.value);
		
		if((szP=CLI_GetParamByName("phyad"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%d",&rtl8306eSettings.mdioInfo.phy);

		if((szP=CLI_GetParamByName("regad"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%d",&rtl8306eSettings.mdioInfo.reg);

		if((szP=CLI_GetParamByName("pageid"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%d",&rtl8306eSettings.mdioInfo.page);
		
		if( cli2cmm_writeCnuSwitchRegister(&rtl8306eSettings) != TBS_SUCCESS )
		{
			return TBS_FAILED;
		}
		
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数功能 :CLI_CmdCnuAclDropMme 命令实现                                 */
/*********************************************************************/
ULONG CLI_CmdCnuAclDropMme()
{
	uint16_t id = 0;
	uint16_t cltid = 0;
	uint16_t cnuid = 0;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;	
	st_dbsCnu cnu;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	id = CLI_GetCnuTidByMode(iMode);
	cltid= CLI_GetCltTidByMode(iMode);

	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}
	else if( 0 == cnu.col_sts )
	{
		IO_Print("\r\n\r\n  CNU Status Unreachable !");
		return CMM_SUCCESS;
	}
	else
	{
		cnuid = id;
		return cli2cmm_do_aclDropMme(cltid, cnuid);
	}
}

/*********************************************************************/
/* 函数功能 :CLI_CmdUndoCnuAclDropMme 命令实现                                 */
/*********************************************************************/
ULONG CLI_CmdUndoCnuAclDropMme()
{
	uint16_t id = 0;
	uint16_t cltid = 0;
	uint16_t cnuid = 0;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;	
	st_dbsCnu cnu;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	id = CLI_GetCnuTidByMode(iMode);
	cltid = CLI_GetCltTidByMode(iMode);
	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}
	else if( 0 == cnu.col_sts )
	{
		IO_Print("\r\n\r\n  CNU Status Unreachable !");
		return CMM_SUCCESS;
	}
	else
	{
		cnuid = id;
		return cli2cmm_undo_aclDropMme(cltid, cnuid);
	}
}

/*********************************************************************/
/* 函数功能 :CLI_CmdMmeMdio 命令实现                                 */
/*********************************************************************/
ULONG CLI_CmdMmeMdio()
{
	char  *szP=NULL;
	char *pParam;
	T_szAr8236Phy szAr8236Phy;

	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	/* 判断模式并显示打印信息*/
	/* 该命令只能在CLT模式下执行*/
	if( 1 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	
	/* interface clt 模式*//* 获取该clt 的索引号码*/
	id = CLI_GetCltTidByMode(iMode);	

	szAr8236Phy.clt = id;
	szAr8236Phy.cnu = 0;
	
	if((pParam=CLI_GetParamByName("read |write"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	if (!STB_StriCmp(pParam, "read"))
	{
		if((szP=CLI_GetParamByName("phyad"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%x",&szAr8236Phy.mdioPhy.phy);

		if((szP=CLI_GetParamByName("regad"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%x",&szAr8236Phy.mdioPhy.reg);

		/*此函数调用要修改*/
		if( cli2cmm_mdioReadPhy(&szAr8236Phy) != TBS_SUCCESS )
		{
			return TBS_FAILED;
		}
		IO_Print("\r\n\r\n  Value:	%x",szAr8236Phy.mdioPhy.value);
	}
	else if (!STB_StriCmp(pParam, "write"))
	{
		if((szP=CLI_GetParamByName("regvalue"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%x",&szAr8236Phy.mdioPhy.value);

		if((szP=CLI_GetParamByName("phyad"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%x",&szAr8236Phy.mdioPhy.phy);
		
		if((szP=CLI_GetParamByName("regad"))==NULL)
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}	
		sscanf(szP,"%x",&szAr8236Phy.mdioPhy.reg);
		/*此函数调用要修改*/
		if( cli2cmm_mdioWritePhy(&szAr8236Phy) != TBS_SUCCESS )
		{
			return TBS_FAILED;
		}
		
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	return TBS_SUCCESS;
}

/*********************************************************************/
/* 函数功能 :升级文件系统命令实现                                 */
/*********************************************************************/
ULONG CLI_CmdUpgrade()
{
	int ret = 0;
	IO_Print("\r\n\r\n  System is upgrading now, do not restart or turn off the power supply !");
	IO_Print("\r\n  Please waiting ...");
	fflush(stdout);

	ret = cli2cmm_upgradeApp();
	switch( ret )
	{
		case RC_UPG_OK:
		{
			IO_Print("\r\n  Operation done successfully, system is restarting now.");
			break;
		}
		case RC_UPG_CANNOT_OPEN_DBS:
		case RC_UPG_READ_DBS_ERROR:
		{
			IO_Print("\r\n  Databases access error, system is restarting now.");
			break;
		}
		case RC_UPG_INIT_NETWORK_ERROR:
		{
			IO_Print("\r\n  Init network error, system is restarting now.");
			break;
		}
		case RC_UPG_FILE_DOESNT_EXIST:
		{
			IO_Print("\r\n  Cannot find image file, system is restarting now.");
			break;
		}
		case RC_UPG_MD5FILE_DOESNT_EXIST:
		case RC_UPG_MD_CHECK_ERROR:
		{
			IO_Print("\r\n  Image MD5 error, system is restarting now.");
			break;
		}
		case RC_UPG_FLASH_EXHAUSTED:
		{
			IO_Print("\r\n  Flash exhuasted, system is restarting now.");
			break;
		}
		case RC_UPG_NOT_ENOUGH_MEMORY:
		{
			IO_Print("\r\n  Not enough memery, system is restarting now.");
			break;
		}
		case RC_UPG_SERVER_UNREACHABLE:
		{
			IO_Print("\r\n  FTP server is unreachable, system is restarting now.");
			break;
		}
		case RC_UPG_INVALID_PARAMETER:
		{
			IO_Print("\r\n  Parameter error, system is restarting now.");
			break;
		}
		case RC_UPG_FLASH_WRITE_ERROR:
		case RC_UPG_FLASH_ERASE_ERROR:
		case RC_UPG_MTD_OPEN_ERROR:
		{
			IO_Print("\r\n  Flash io error, system is restarting now.");
			break;
		}
		case RC_UPG_SYS_SOCKET_IO_ERROR:
		{
			IO_Print("\r\n  Socket io error, system is restarting now.");
			break;
		}
		case RC_UPG_OPT_TIMEOUT:
		{
			IO_Print("\r\n  Operating timeout, system is restarting now.");
			break;
		}
		case RC_UPG_SYS_GENERIC_IO_ERROR:
		{
			IO_Print("\r\n  System generic io error, system is restarting now.");
			break;
		}
		case RC_UPG_FILE_DOWNLOAD_ERROR:
		{
			IO_Print("\r\n  Image download error, system is restarting now.");
			break;
		}
		case RC_UPG_FILE_TAG_ERROR:
		{
			IO_Print("\r\n  Image tag error, system is restarting now.");
			break;
		}
		case RC_UPG_FILE_INVALID:
		{
			IO_Print("\r\n  Image file invalid, system is restarting now.");
			break;
		}
		default:
		{
			IO_Print("\r\n  Operating failed, system is restarting now.");
			break;
		}
	}

	return cli2cmm_resetMp();
}

/*********************************************************************/
/* 函数功能 :reload profile 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_ReloadProfile()
{
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;
	uint16_t cltid;
	st_dbsCnu cnu;
	st_dbsProfile profile;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	cltid = CLI_GetCltTidByMode(iMode);
	/* 判断模式并显示打印信息*/
	/* 该命令只能在CLT/CNU模式下执行*/
	if( 1 == prevailMode )
	{
		/* interface clt 模式*/
		id = CLI_GetCltTidByMode(iMode);
		return cli2cmm_reloadCltProfile(id);
	}
	else if( 2 == prevailMode )
	{
		/* interface cnu 模式*/
		id = CLI_GetCnuTidByMode(iMode);

		/* 如果该CNU 槽位无效则禁止配置*/
		if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
		{
			IO_Print("\r\n\r\n  System Error !");
			return CMM_FAILED;
		}	
		else if( 0 == cnu.col_row_sts )
		{
			IO_Print("\r\n\r\n  CNU Interface Unreachable !");
			return CMM_SUCCESS;
		}

		/* 如果该PROFILE 槽位无效则禁止配置*/
		if( CMM_SUCCESS != dbsGetProfile(dbsdev, id,  &profile) )
		{
			IO_Print("\r\n\r\n  System Error !");
			return CMM_FAILED;
		}	
		else if( 0 == profile.col_row_sts )
		{
			IO_Print("\r\n\r\n  CNU Profile Unreachable !");
			return CMM_SUCCESS;
		}		
	
		return cli2cmm_reloadCnuProfile(cltid, id);
	}
	else
	{
		/* 全局模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_SUCCESS;
	}
}

/*********************************************************************/
/* 函数功能 :permit 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_DoCnuPermit()
{	
	char  *szP=NULL;
	int a = 0;
	int b = 0;
	uint16_t cltid = 0;
	uint16_t cnuid = 0;
	uint16_t id = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;
	st_dbsSysinfo sysinfo;
	
	if((szP=CLI_GetParamByName("cnu"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	else if((szP=CLI_GetParamByName("index"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}

	if( 2 != sscanf(szP, "%d/%d", &a, &b) )
	{
		IO_Print("\r\n Invalid keyword value.");
		IO_Print("\r\n Invalid parameter value:%s(index).", szP);
		return CMM_SUCCESS;
	}
	
	/* 判断ID 是否有效*/
	if( (a < 1 )||( a > MAX_CLT_AMOUNT_LIMIT) )
	{
		IO_Print("\r\n Invalid keyword value.");
		IO_Print("\r\n Invalid parameter value:%s(index).", szP);
		return CMM_SUCCESS;
	}
	else if( (b < 1)||( b > MAX_CNUS_PER_CLT))
	{
		IO_Print("\r\n Invalid keyword value.");
		IO_Print("\r\n Invalid parameter value:%s(index).", szP);
		return CMM_SUCCESS;
	}
	else
	{
		cltid = a;
		cnuid = b;
		id = (a-1)*MAX_CNUS_PER_CLT+b;
	}	

	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}
	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}

	/* 如果白名单没有开启则禁止执行*/
	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &sysinfo) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}
	else if( 0 == sysinfo.col_wlctl )
	{
		IO_Print("\r\n\r\n  Operation forbidden: This command is only allowed when white list enabled !");
		return CMM_SUCCESS;
	}
	
	return cli2cmm_permitCnu(cltid, cnuid);
}

/*********************************************************************/
/* 函数功能 :delete 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_UndoCnuPermit()
{	
	char  *szP=NULL;
	int a = 0;
	int b = 0;
	uint16_t cltid = 0;
	uint16_t cnuid = 0;
	uint16_t id = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;
	st_dbsSysinfo sysinfo;
	
	if((szP=CLI_GetParamByName("cnu"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	else if((szP=CLI_GetParamByName("index"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}

	if( 2 != sscanf(szP, "%d/%d", &a, &b) )
	{
		IO_Print("\r\n Invalid keyword value.");
		IO_Print("\r\n Invalid parameter value:%s(index).", szP);
		return CMM_SUCCESS;
	}
	
	/* 判断ID 是否有效*/
	if( (a < 1 )||( a > MAX_CLT_AMOUNT_LIMIT) )
	{
		IO_Print("\r\n Invalid keyword value.");
		IO_Print("\r\n Invalid parameter value:%s(index).", szP);
		return CMM_SUCCESS;
	}
	else if( (b < 1)||( b > MAX_CNUS_PER_CLT))
	{
		IO_Print("\r\n Invalid keyword value.");
		IO_Print("\r\n Invalid parameter value:%s(index).", szP);
		return CMM_SUCCESS;
	}
	else
	{
		cltid = a;
		cnuid = b;
		id = (a-1)*MAX_CNUS_PER_CLT+b;
	}

	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}
	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}

	/* 如果白名单没有开启则禁止执行*/
	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &sysinfo) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}
	else if( 0 == sysinfo.col_wlctl )
	{
		IO_Print("\r\n\r\n  Operation forbidden: This command is only allowed when white list enabled !");
		return CMM_SUCCESS;
	}

	return cli2cmm_UndoPermitCnu(cltid, cnuid);
}

/*********************************************************************/
/* 函数功能 :delete 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_DeleteCnu()
{	
	char  *szP=NULL;
	int a = 0;
	int b = 0;
	uint16_t cltid = 0;
	uint16_t cnuid = 0;
	uint16_t id = 0;
	st_dbsCnu cnu;
	//st_dbsProfile profile;
	
	if((szP=CLI_GetParamByName("cnu"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	else if((szP=CLI_GetParamByName("index"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}

	if( 2 != sscanf(szP, "%d/%d", &a, &b) )
	{
		IO_Print("\r\n Invalid keyword value.");
		IO_Print("\r\n Invalid parameter value:%s(index).", szP);
		return CMM_SUCCESS;
	}
	
	/* 判断ID 是否有效*/
	if( (a < 1 )||( a > MAX_CLT_AMOUNT_LIMIT) )
	{
		IO_Print("\r\n Invalid keyword value.");
		IO_Print("\r\n Invalid parameter value:%s(index).", szP);
		return CMM_SUCCESS;
	}
	else if( (b < 1)||( b > MAX_CNUS_PER_CLT))
	{
		IO_Print("\r\n Invalid keyword value.");
		IO_Print("\r\n Invalid parameter value:%s(index).", szP);
		return CMM_SUCCESS;
	}
	else
	{
		cltid = a;
		cnuid = b;
		id = (a-1)*MAX_CNUS_PER_CLT+b;
	}

	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}
	else if( cnu.col_sts  )
	{
		/* 如果CNU 在线则禁止删除*/
		IO_Print("\r\n\r\n  Active cnu can not be removed !");
		return CMM_SUCCESS;
	}	

	return cli2cmm_DeleteCnu(cltid, cnuid);
}

/*********************************************************************/
/* 函数功能 :create cnu 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_CreateCnu()
{	
	char  *szP=NULL;
	uint8_t bMac[6] = {0};	
	
	if((szP=CLI_GetParamByName("cnu"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	else if((szP=CLI_GetParamByName("mac"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	else if( 0 != boardapi_macs2b(szP, bMac) )
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	else
	{
		if( CMM_SUCCESS != cli2cmm_CreateCnu(bMac) )
		{
			IO_Print("\r\n\r\n  Failed !");
			return TBS_FAILED;
		}
		else
		{
			return CMM_SUCCESS;
		}
	}
}


/*********************************************************************/
/* 函数功能 :匿名用户接入控制的 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_DoWlistControl()
{
	char *pParam;
	
	if((pParam=CLI_GetParamByName("enable|disable"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	if (!STB_StriCmp(pParam, "enable"))
	{
		return cli2cmm_DoWlistControl(1);
	}
	else if (!STB_StriCmp(pParam, "disable"))
	{
		return cli2cmm_DoWlistControl(0);
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
}

/*********************************************************************/
/* 函数功能 :匿名用户接入控制的 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_DoWdtControl()
{
	char *pParam;
	
	if((pParam=CLI_GetParamByName("enable|disable"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	if (!STB_StriCmp(pParam, "enable"))
	{
		return cli2cmm_DoWdtControl(1);
	}
	else if (!STB_StriCmp(pParam, "disable"))
	{
		return cli2cmm_DoWdtControl(0);
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
}

/*********************************************************************/
/* 函数功能 :匿名用户接入控制的 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_DoDsdtPortStats()
{
	int port = 0;
	char *pParam;
	
	if((pParam=CLI_GetParamByName("print|clear"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	if (!STB_StriCmp(pParam, "print"))
	{
		for( port=0; port<=6;port++)
		{
			if( CMM_SUCCESS != cli2cmm_DebugPrintAllDsdtPortStats(port) )
			{
				MT_ERRLOG(0);
				return TBS_FAILED;
			}
		}
		return CMM_SUCCESS;
	}
	else if (!STB_StriCmp(pParam, "clear"))
	{
		return cli2cmm_ClearDsdtPortStats();
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
}

/*********************************************************************/
/* 函数功能 :匿名用户接入控制的 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_DoDsdtPortMirror()
{
	st_dsdtPortMirroring stMirrorInfo;
	char *pParam;
	
	/* 读取命令行参数*/
	/* 读取端口信息*/
	if( (pParam = CLI_GetParamByName("sport")) == NULL )
	{
		MT_ERRLOG(0);
		return CMM_FAILED;
	}
	else
	{
		if(!STB_StriCmp(pParam, "p0"))
		{
			stMirrorInfo.source = 0;
		}
		else if(!STB_StriCmp(pParam, "p1"))
		{				
			stMirrorInfo.source = 1;
		}
		else if(!STB_StriCmp(pParam, "p2"))
		{				
			stMirrorInfo.source = 2;
		}
		else if(!STB_StriCmp(pParam, "p3"))
		{				
			stMirrorInfo.source = 3;
		}
		else if(!STB_StriCmp(pParam, "p4"))
		{				
			stMirrorInfo.source = 4;
		}
		else if(!STB_StriCmp(pParam, "p5"))
		{				
			stMirrorInfo.source = 5;
		}
		else if(!STB_StriCmp(pParam, "p6"))
		{				
			stMirrorInfo.source = 6;
		}
		else
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
	}

	if( (pParam = CLI_GetParamByName("deport")) == NULL )
	{
		MT_ERRLOG(0);
		return CMM_FAILED;
	}
	else
	{
		if(!STB_StriCmp(pParam, "p0"))
		{
			stMirrorInfo.dest = 0;
		}
		else if(!STB_StriCmp(pParam, "p1"))
		{				
			stMirrorInfo.dest = 1;
		}
		else if(!STB_StriCmp(pParam, "p2"))
		{				
			stMirrorInfo.dest = 2;
		}
		else if(!STB_StriCmp(pParam, "p3"))
		{				
			stMirrorInfo.dest = 3;
		}
		else if(!STB_StriCmp(pParam, "p4"))
		{				
			stMirrorInfo.dest = 4;
		}
		else if(!STB_StriCmp(pParam, "p5"))
		{				
			stMirrorInfo.dest = 5;
		}
		else if(!STB_StriCmp(pParam, "p6"))
		{				
			stMirrorInfo.dest = 6;
		}
		else
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
	}

	if( stMirrorInfo.dest == stMirrorInfo.source )
	{
		IO_Print("\r\n\r\n  Destination port and source port must not be the same port !");
		return TBS_FAILED;
	}

	return cli2cmm_DoPortMirroring(&stMirrorInfo);	
}

ULONG CLI_Cmd_DoDsdtMacBinding()
{
	stDsdtMacBinding macBindingInfo;
	char  *szP=NULL;
	char *pParam=NULL;
	
	/* 读取命令行参数*/
	/* 读取端口信息*/
	if((szP=CLI_GetParamByName("mac-address"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	else if((szP=CLI_GetParamByName("mac"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	else if( 0 != boardapi_macs2b(szP, macBindingInfo.mac) )
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	if( (pParam = CLI_GetParamByName("deport")) == NULL )
	{
		MT_ERRLOG(0);
		return CMM_FAILED;
	}
	else
	{
		if(!STB_StriCmp(pParam, "p0"))
		{
			macBindingInfo.portid = 0;
		}
		else if(!STB_StriCmp(pParam, "p1"))
		{				
			macBindingInfo.portid = 1;
		}
		else if(!STB_StriCmp(pParam, "p2"))
		{				
			macBindingInfo.portid = 2;
		}
		else if(!STB_StriCmp(pParam, "p3"))
		{				
			macBindingInfo.portid = 3;
		}
		else if(!STB_StriCmp(pParam, "p4"))
		{				
			macBindingInfo.portid = 4;
		}
		else if(!STB_StriCmp(pParam, "p5"))
		{				
			macBindingInfo.portid = 5;
		}
		else if(!STB_StriCmp(pParam, "p6"))
		{				
			macBindingInfo.portid = 6;
		}
		else
		{
			MT_ERRLOG(0);
			return TBS_FAILED;
		}
	}

	macBindingInfo.dbNum = 0;
	
	return cli2cmm_DoDsdtMacBinding(&macBindingInfo);	
}

/*********************************************************************/
/* 函数功能 :匿名用户接入控制的 命令实现                                 */
/*********************************************************************/
ULONG CLI_Cmd_DoHBControl()
{
	char *pParam;
	
	if((pParam=CLI_GetParamByName("enable|disable"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	if (!STB_StriCmp(pParam, "enable"))
	{
		return cli2cmm_DoHBControl(1);
	}
	else if (!STB_StriCmp(pParam, "disable"))
	{
		return cli2cmm_DoHBControl(0);
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
}


/*********************************************************************/
/* 函数功能 :                             */
/*********************************************************************/
ULONG CLI_Cmd_Dump()
{
	char *pParam;
	uint32_t iMode = 0;
	uint32_t prevailMode = 0;
	uint16_t id = 0;
	uint16_t cltid;
	st_dbsCnu cnu;
	st_dbsProfile profile;

	iMode = CLI_GetCurrentMode();
	prevailMode = CLI_GetPrevailMode(iMode);

	cltid = CLI_GetCltTidByMode(iMode);
	/* 判断模式并显示打印信息*/
	/* 该命令只能在CNU模式下执行*/
	if( 2 != prevailMode )
	{
		/* 其他模式下禁止执行该命令*/
		IO_Print("\r\n\r\n  Incorrect command");
		return CMM_FAILED;
	}
	/* interface cnu 模式*//* 获取该CNU 的索引号码*/
	id = CLI_GetCnuTidByMode(iMode);

	/* 如果该CNU 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetCnu(dbsdev, id, &cnu) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == cnu.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Interface Unreachable !");
		return CMM_SUCCESS;
	}	

	/* 如果该PROFILE 槽位无效则禁止配置*/
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id,  &profile) )
	{
		IO_Print("\r\n\r\n  System Error !");
		return CMM_FAILED;
	}	
	else if( 0 == profile.col_row_sts )
	{
		IO_Print("\r\n\r\n  CNU Profile Unreachable !");
		return CMM_SUCCESS;
	}
	
	if((pParam=CLI_GetParamByName("register|mod|pib"))==NULL)
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
	
	if (!STB_StriCmp(pParam, "register"))
	{
		return cli2cmm_Dump(cltid, id, 0);
	}
	else if (!STB_StriCmp(pParam, "mod"))
	{
		return cli2cmm_Dump(cltid, id, 1);
	}
	else if (!STB_StriCmp(pParam, "pib"))
	{
		return cli2cmm_Dump(cltid, id, 2);
	}
	else
	{
		MT_ERRLOG(0);
		return TBS_FAILED;
	}
}

#if 0

/*********************************************************************/
/* 函数功能 :查询用户信息 命令实现                                 */
/*********************************************************************/
ULONG CLI_CmdGetAnonymous()
{
	uint8_t mac[18] = {0};
	uint8_t *devtype = NULL;
	stUserAutoConfigInfo TmUserInfo;
	DB_ROW_TBL_CNU cnu;
	TmUserInfo.clt = CLT_INDEX;
	TmUserInfo.cnu = 1;
	TmUserInfo.status = 1;
	bzero((unsigned char *)&cnu, sizeof(DB_ROW_TBL_CNU));
	IO_Print("\r\n\r\n------------------------------------------------------------------------------");
	IO_Print("\r\n%-5s  %-12s  %-18s  %-8s  %-8s  %-8s  %-6s",
			"Index","DevType","MAC","Profile","AccessEn","AutoUpEn","Online"); 
	IO_Print("\r\n------------------------------------------------------------------------------");
	do
	{
		if( msg_get_anonymous(&TmUserInfo, &cnu) != TBS_SUCCESS )
		{
			return TBS_FAILED;
		}
		if((cnu.UserType == 0) && (cnu.OnUsed == 1))
		{
	 		sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",cnu.mac[0], cnu.mac[1], cnu.mac[2], cnu.mac[3],
	 			cnu.mac[4], cnu.mac[5]);	
			switch(cnu.DevType)
			{
				case WEC_3702I:
					devtype = "WEC-3702I L2";
					break;
				case WEC_3703I:
					devtype = "WEC-3702I L3";
					break;
				case WEC_602:
					devtype = "WEC-3702I C2";
					break;
				case WEC_604:
					devtype = "WEC-3702I C4";
					break;
				default:
					devtype = "UNKOWN";
					break;
			}
			IO_Print("\r\n%-5d  %-12s  %-18s  %-8d  %-8d  %-8d  %-6d",
				cnu.id, devtype, mac, cnu.TempId, cnu.AccessEnable, cnu.AutoUpEnable, cnu.online);
		}
		TmUserInfo.status = 0;
		TmUserInfo.cnu++;
		if(TmUserInfo.cnu >64)
		{
			return TBS_SUCCESS;
		}
	}while(cnu.DevType != 0);
	
	return TBS_SUCCESS;
	
}

#endif

#ifdef __cplusplus
}
#endif

