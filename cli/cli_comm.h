/**********************************************************************
 Copyright (c), 1991-2007, Prevail ELECTRONICS(SHENTHEN) Co., Ltd.
 �ļ����� : cli_comm.h
 �ļ����� : ������ͨ�Ŵ����ͷ�ļ�


 �����б� :


 �޶���¼ :
          1 ���� : 
            ���� : 2007-11-6
            ���� :

**********************************************************************/

  
#ifndef __CLI_COMM_H__
#define __CLI_COMM_H__

#include "cli_pub.h"
#include "util/cli_private.h"

/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/


#define CLI_NODE__LEN       512     // �ڵ㳤�ȷ�Χ
#define CLI_LEAF__LEN       512      // Ҷ�ӳ��ȷ�Χ

#define CLI_MAX_OBJ_NUM     64      // ÿ��OBJECT������

#define CLI_MAX_RWOBJ_NUM  16       // ÿ�ζ�д�����������Ŀ


#define CLI_COMM_TIMEOUT    15

ULONG CLI_CommDestroy(void);
ULONG CLI_CommInit();


#endif /* __CLI_COMM_H__ */



