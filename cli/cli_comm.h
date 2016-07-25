/**********************************************************************
 Copyright (c), 1991-2007, Prevail ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称 : cli_comm.h
 文件描述 : 命令行通信处理的头文件


 函数列表 :


 修订记录 :
          1 创建 : 
            日期 : 2007-11-6
            描述 :

**********************************************************************/

  
#ifndef __CLI_COMM_H__
#define __CLI_COMM_H__

#include "cli_pub.h"
#include "util/cli_private.h"

/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/


#define CLI_NODE__LEN       512     // 节点长度范围
#define CLI_LEAF__LEN       512      // 叶子长度范围

#define CLI_MAX_OBJ_NUM     64      // 每种OBJECT最大个数

#define CLI_MAX_RWOBJ_NUM  16       // 每次读写操作的最大数目


#define CLI_COMM_TIMEOUT    15

ULONG CLI_CommDestroy(void);
ULONG CLI_CommInit();


#endif /* __CLI_COMM_H__ */



