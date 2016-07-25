/****************************************************************************
 �ļ���  : cli_cmd.h
 ����    : 
 �汾    :
 �������:
 �ļ�����:  ��������ģ�������ע�ắ����ʵ�ֺ���������
 �޸���ʷ:
        1. �޸���   :
           ʱ��     :
           �汾     :
           �޸�ԭ�� :
 ****************************************************************************/

#ifndef _CLI_CMD_INC_
#define _CLI_CMD_INC_

#include "../util/cli_private.h"

ULONG  CLI_SysCmdReg(_VOID   );
ULONG  CLI_CmdSetMonitorOn();
ULONG  CLI_CmdSetMonitorOff();
ULONG CLI_CmdTemplate();


/* ****************** ���º�ĺ�������****************** */
ULONG CLI_CmdClearScreen();
ULONG CLI_CmdExit();
ULONG CLI_CmdSetScroll();
ULONG CLI_CmdHelp();
ULONG CLI_Cmd_Show_Optlog();
ULONG CLI_Cmd_Show_alarmlog();
ULONG CLI_Cmd_ShowSyslog();
ULONG CLI_Cmd_ShowTopology();
ULONG CLI_Cmd_ShowWlistUsers();
ULONG CLI_Cmd_ShowNetworkInfo();
ULONG CLI_Cmd_ShowSysInfo();
ULONG CLI_Cmd_ShowCurProfile();
ULONG CLI_Cmd_ShowSnmpInfo();
ULONG CLI_Cmd_ShowFtpServer();
ULONG CLI_Cmd_SetCliPwd();
ULONG CLI_Cmd_SnmpConfig();
ULONG CLI_Cmd_Debug();
ULONG CLI_Cmd_Reboot();
ULONG CLI_Cmd_SaveConfig();
ULONG CLI_Cmd_RestoreDefault();
ULONG CLI_Cmd_SetIpAddress();
ULONG CLI_Cmd_SetIpGateway();
ULONG CLI_Cmd_SetMgmtVlan();
ULONG CLI_Cmd_UndoMgmtVlan();
ULONG CLI_Cmd_UndoIpAddress();
ULONG CLI_Cmd_UndoIpGateway();
ULONG CLI_Cmd_SetFtpServer();
ULONG CLI_Cmd_DoCnuVlanConfig();
ULONG CLI_Cmd_UndoCnuVlanConfig();
ULONG CLI_Cmd_DoRateLimiting();
ULONG CLI_Cmd_UndoRateLimiting();
ULONG CLI_Cmd_DoStromFilter();
ULONG CLI_Cmd_UndoStromFilter();
ULONG CLI_Cmd_Shutdown();
ULONG CLI_Cmd_UndoShutdown();
ULONG CLI_Cmd_MacLimit();
ULONG CLI_Cmd_UndoMacLimit();
ULONG CLI_Cmd_DoAgingTimeConfig();
ULONG CLI_Cmd_UndoAgingTimeConfig();
ULONG CLI_Cmd_SelectQoSType();
ULONG CLI_Cmd_DoQosMap();
ULONG CLI_Cmd_UndoQos();
ULONG CLI_CmdAr8236SmiReg();
ULONG CLI_CmdAr8236SmiPhy();
ULONG CLI_CmdUpgrade();
ULONG CLI_Cmd_ReloadProfile();
ULONG CLI_Cmd_DoCnuPermit();
ULONG CLI_Cmd_UndoCnuPermit();
ULONG CLI_Cmd_DeleteCnu();
ULONG CLI_Cmd_DoWlistControl();
ULONG CLI_Cmd_DoWdtControl();
ULONG CLI_Cmd_DoHBControl();
ULONG CLI_Cmd_CreateCnu();
ULONG CLI_Cmd_Dump();
ULONG CLI_Cmd_DoDsdtPortStats();
ULONG CLI_Cmd_DoDsdtRgmiiTimingDelay();
ULONG CLI_Cmd_DoDsdtPortMirror();
ULONG CLI_CmdMmeMdio();
ULONG CLI_CmdCnuSwitch();
ULONG CLI_Cmd_DoDsdtMacBinding();
ULONG CLI_CmdCnuAclDropMme();
ULONG CLI_CmdUndoCnuAclDropMme();
/* ****************** ���º�ĺ�������****************** */


#endif


