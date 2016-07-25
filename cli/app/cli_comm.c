/**********************************************************************
 Copyright (c), 1991-2007, Prevail ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称 : cli_comm.c
 文件描述 : 命令行进程的通信模块处理


 函数列表 :


 修订记录 :
          1 创建 : frank
            日期 : 2007-11-6
            描述 :

**********************************************************************/

#include "../cli_pub.h"
#include "cli_cmm.h"
#include <dbsapi.h>

extern T_DBS_DEV_INFO *dbsdev;

ULONG CLI_CommDestroy(void)
{
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "module cli exited");
	destroy_cli_cmm();
	dbsClose(dbsdev);
	return CMM_SUCCESS;
}

/*=========================================================================*/
/*  函数名称: CLI_CommInit                                                 */
/*  函数功能: 命令行通信初始化                                             */
/*  输入参数:                                                              */
/*  返  回  : 成功、失败                                                   */
/*  创  建  : frank / 2007-11-6                                         */
/*=========================================================================*/
ULONG CLI_CommInit()
{
	/* 打开数据库模块接口*/
	dbsdev = dbsNoWaitOpen(MID_CLI);
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: cli->dbsNoWaitOpen error, exited !\n");
		return CMM_FAILED;
	}
	/* 消息模块的注册 */
	if( init_cli_cmm() != CMM_SUCCESS )  /* 通讯初始化失败 */
	{
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module cli init_cli_cmm error, exited !");
		fprintf(stderr, "ERROR: CLI_CommInit->init_cli_cmm failed !\n");
		dbsClose(dbsdev);
		return CMM_FAILED;
	}
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting module cli success");
	return CMM_SUCCESS;
}

