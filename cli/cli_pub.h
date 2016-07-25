/**********************************************************************
 Copyright (c), 1991-2007, Prevail ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称 : cli_private.h
 文件描述 :


 函数列表 :


 修订记录 :
          1 创建 : 
            日期 : 2007-10-30
            描述 :

**********************************************************************/

#ifndef __CLI_PUB_H__
#define __CLI_PUB_H__

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "cli_strres.h"

/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/

typedef unsigned char           UCHAR;
typedef unsigned short          USHORT;
typedef unsigned long           ULONG;

typedef char                    CHAR;
typedef short                   SHORT;
typedef long                    LONG;

typedef void                    _VOID;

typedef const char * PCSTR;
typedef char *       PSTR;
#define BOOL    unsigned int

#ifndef NULL
#define NULL                  (0)
#endif

#ifndef TRUE
#define TRUE                  ((BOOL)1)
#endif
#ifndef FALSE
#define FALSE                 ((BOOL)0)
#endif

/*--------------------错误码列表--------------------------*/
enum
{
        /*--------------------通用错误码返回值定义--------------------------*/
    TBS_SUCCESS                                 = 0,        /* 成功/正常*/
    TBS_FAILED                                    = -1,   /* 失败/一般错误*/
    TBS_MSG_SEND_ERR                        = -2,   /* 消息发送失败*/
    TBS_OUT_OF_MEM                            = -3,   /* 内存不足*/
    TBS_PARAM_ERR                                = -4,   /* 参数不正确*/
    TBS_OUT_OF_SRC                            = -5,   /* 其它资源不足, 如控制块...*/
    TBS_TIME_OUT                                = -6,   /* 超时*/
    TBS_COLLISION                                = -7,   /* 冲突, 如共享错误/非法重入...*/
    TBS_NULL_PTR                                = -8,   /* 空指针 */

    /* 下面是双链表的常用错误码 */
    TBS_LIST_NOT_EMPTY                    = -9,    /* List is not empty */
    TBS_LIST_EMPTY                            = -10,   /* List is empty */
    TBS_INVALID_INDEX                        = -11,   /* Invalid index, valid index range: 0x1 ~ 0xffffffff */
    TBS_INDEX_OVERFLOW                    = -12,   /* Index overflow, valid index range: 0x1 ~ 0xffffffff */
    TBS_CORRUPT_STRUCT                    = -13,   /* Corrupt struct, meaning struct is not initialized properly */

    TBS_NO_INSTANCE                    = -14,/*没有对应的实例*/
    TBS_MODULE_BUSY                       = -15 , /* 模块忙，暂不处理设置类消息*/

    /*--------------------TR069相关通用错误码返回值定义--------------------------*/
    TR069_ERRNO_UNSUP_FUNC            = 9000,  /* 方法不支持*/
    TR069_ERRNO_REJECT                    = 9001,  /* 拒绝请求, 未说明原因*/
    TR069_ERRNO_CPE_ERR                    = 9002,  /* 内部错误*/
    TR069_ERRNO_INVALID_PARAM        = 9003,  /* 无效参数*/
    TR069_ERRNO_NO_RESOURCE            = 9004,  /* 资源超限（当用于SetParameterValues的关联时，它不得用来指示参数错误）*/
    TR069_ERRNO_INVALID_NAME        = 9005,  /* 无效参数名（与Set/GetParameterValues，GetParameterNames，Set/GetParameterAttributes相关）*/
    TR069_ERRNO_INVALID_TYPE        = 9006,  /* 无效参数类型（与SetParameterValues相关）*/
    TR069_ERRNO_INVALID_VAL            = 9007,  /* 无效参数值（与SetParameterValues相关）*/
    TR069_ERRNO_WRITE_FORBID        = 9008,  /* 试图设置不可写的参数（与SetParameterValues相关）*/
    TR069_ERRNO_NOTI_REQ_REJ        = 9009,  /* 参数变更通知请求被拒绝（与SetParameterAttributes方法相关）*/
    TR069_ERRNO_DOWNLOAD_FAIL        = 9010,  /* 下载失败（与Download或TransferComplete方法相关）*/
    TR069_ERRNO_UPLOAD_FAIL            = 9011,  /* 上载失败（与Upload或TransferComplete方法相关）*/
    TR069_ERRNO_FS_AUTHFAIL            = 9012,  /* 文件传输服务器验证失败（与Upload或TransferComplete方法相关）*/
    TR069_ERRNO_FS_NOTSUPP            = 9013,  /* 不支持的文件传输协议（与Upload或TransferComplete方法相关）*/

    ERR_UNKNOWN                                                  /* 必须是最后一个，添加时在此前面添加错误码 */

};

#define CLI_ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

#define CLI_FAILED(ret) (ret != TBS_SUCCESS)

/* 要求用户对某个操作动作进行确认时的返回值的枚举定义*/
enum tag_EN_INPUT_TYPE
{
    INPUT_RETURN = 0,
    INPUT_YES,
    INPUT_NO,
    INPUT_TIMEOUT,
    INPUT_ERROR,
    INPUT_OTHER
} ;


/* 操作级别。 在命令对象和用户属性中均有引用。级别递增 */
typedef enum  CLI_ACTION_LEVEL_E{
    CLI_AL_QUERY = 0,
    CLI_AL_OPERATOR,
    CLI_AL_ADMIN,
    CLI_AL_SUPER,
    CLI_AL_DEBUG,
    CLI_AL_NULL
} CLI_OPERAT_LEVEL_T;


/* 命令执行函数格式定义 */
typedef ULONG  (*PFUN_CLI_CALLBACK_EXEC)();


/******************************************************************************
 *                                 ENUM                                       *
 ******************************************************************************/










/******************************************************************************
 *                                STRUCT                                      *
 ******************************************************************************/










/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/

/*-------------------------------------------------------------------*/
/* 函数名称 : CLI_SysCmdRegist                                       */
/* 功    能 : 命令注册函数接口                                       */
/* 输入参数 : 参数说明见函数定义                                     */
/* 输出参数 : 无                                                     */
/* 返回     : 注册成功/失败                                          */
/* 调用说明 : 调用者自己保证命令语法的正确性,详细说明可以参见命令行  */
/*            说明文档                                               */
/* 典型示例 : CLI_SysCmdRegist("cmdtest user-test: <type(u1~3)>",    */
/*             CTM_GLOBAL, CLI_AL_OPERATOR,  CLI_CmdTest4,           */
/*             CLI_ML_NULL,  CLI_ML_NULL,  CLI_ML_NULL );            */
/*-------------------------------------------------------------------*/
ULONG  CLI_CmdRegist
              (const char  *szCmd,              //命令格式化串
               USHORT  ucMode,                   //命令所属模式
               CLI_OPERAT_LEVEL_T   ucLevel,    //命令使用等级
               PFUN_CLI_CALLBACK_EXEC pFunc,    //命令执行函数
               PCSTR  strHelp1,                 //第一个对象的帮助信息
               PCSTR  strHelp2,                 //第二个对象的帮助信息
               PCSTR  strHelp3);

/*----------------------------------------------------------------------*/
/* 函数名称 : CLI_GetParamByName                                        */
/* 功    能 : 通过参数名获取参数                                        */
/* 输入参数 : szName: 参数名                                            */
/* 输出参数 : 无                                                        */
/* 返回     : 获取的参数字符串指针                                      */
/* 调用说明 : 调用者应对返回值进行判断. 详细说明可参见命令行说明文档.   */
/* 典型示例 : char *pParam;                                             */
/*            1) pParam = CLI_GetParamByName("userName/2"));            */
/*            2) pParam = CLI_GetParamByName("xmodem|tftp"));           */
/*            3) if ((pParam = CLI_GetParamByName("ip-mask/2"))         */
/*                  == G_NULL)                                          */
/*                   return G_FAILURE;                                  */
/*----------------------------------------------------------------------*/
char * CLI_GetParamByName(const char *szName);

/*-------------------------------------------------------------------*/
/* 函数名称 : CLI_GetLongByName                                      */
/* 功    能 : 通过参数名获取长整型参数                               */
/* 输入参数 : szName: 参数名                                         */
/* 输出参数 : lResult 获取结果                                       */
/* 返回     : 获取成功/失败                                          */
/* 调用说明 : 调用者应对返回值进行判断. 详细说明可参见命令行说明文档.*/
/* 典型示例 : CLI_GetLongByName("time", &Time);                      */
/*-------------------------------------------------------------------*/
ULONG CLI_GetLongByName(const char  *szName, int   *lResult);
ULONG CLI_GetUlongByName(const char *szName, ULONG *ulResult);

/*-------------------------------------------------------------------*/
/* 函数名称 : CLI_GetHexByName                                       */
/* 功    能 : 通过参数名获取十六进制参数                             */
/* 输入参数 : szName: 参数名                                         */
/* 输出参数 : lResult 获取结果                                       */
/* 返回     : 获取成功/失败                                          */
/* 调用说明 : 调用者应对返回值进行判断. 详细说明可参见命令行说明文档.*/
/* 典型示例 : CLI_GetHexByName("code", &Code);                       */
/*-------------------------------------------------------------------*/
ULONG CLI_GetHexByName(const char *szName, ULONG *lResult);

/*-------------------------------------------------------------------*/
/* 函数名称 : CLI_GetModeParameter                                    */
/* 功    能 : 获取模式参数                                           */
/* 输入参数 : szName: 参数名                                         */
/*            ulIndex: 索引，一般填2， 如果有多个则填 2*n            */
/* 返回     : 获取结果                                               */
/* 调用说明 : 调用者应对返回值进行判断.                              */
/* 典型示例 : CLI_GetModeParameter(CTM_IF, 2);                        */
/*-------------------------------------------------------------------*/
char * CLI_GetModeParameter( ULONG ulMode, ULONG ulIndex);





/*-------------------------------------------------------------------*/
/* 函数名称 : IO_Print                                               */
/* 功    能 : 将格式化串输出到当前终端                               */
/* 输入参数 : szFormat 格式化串                                      */
/* 输出参数 : 无                                                     */
/* 返回值   : 成功、失败                                             */
/* 调用说明 : 调用本函数需要确保ulInfoID在语种资源中已经有定义，且该 */
/*            资源字符串的最长不得超过2K                             */
/* 典型示例 : IO_Print(CLI_LOG_FAILED_TIME,3)                        */
/*            其中CLI_LOG_FAILED_TIME 对于中文语种信息为:            */
/*                            "\r\n  你已连续登录失败%d次! "         */
/*-------------------------------------------------------------------*/
ULONG   IO_Print(PCSTR szFormat, ...);

/*-------------------------------------------------------------------*/
/* 函数名称 : IO_OutString                                           */
/* 功    能 : 直接输出字符串到当前终端                               */
/* 输入参数 : szOutput 输出信息串                                    */
/* 输出参数 : 无                                                     */
/* 返回值   : 成功、失败                                             */
/* 调用说明 : 直接输出，该函数的调用者多为调试信息输出与特殊要求的   */
/*            输出,如果当前任务非终端任务,则输出到串口               */
/* 典型示例 : IO_OutString("\r\n  正确输出.")                        */
/*-------------------------------------------------------------------*/
ULONG   IO_OutString(const char  *szOutput);

/*********************************************************************/
/* 函数名称 : IO_OutToTerm()                                         */
/* 函数功能 : 组织输出字符串，并根据终端调用函数输出                 */
/* 输入参数 : szOutString  输出字符串                                */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/*********************************************************************/
ULONG  IO_OutToTerm(const char  *szOutString);

/*-------------------------------------------------------------------*/
/* 函数名称 : IO_GetChar                                             */
/* 功    能 : 从终端接收一个字符                                     */
/* 输入参数 : 无                                                     */
/* 输出参数 : 无                                                     */
/* 返回值   : 接收到的字符                                           */
/* 调用说明 : 该函数调用导致任务阻塞，直到终端有输入或超时。注意该函 */
/*            数的调用需要判断返回值，如果返回NULL则可能是终端异常， */
/*            建议程序释放当前资源，不再继续处理当前事件             */
/* 典型示例 : cTmp = IO_GetChar();                                   */
/*-------------------------------------------------------------------*/
char  IO_GetChar();

/*-------------------------------------------------------------------*/
/* 函数名称 : IO_GetChar                                             */
/* 功    能 : 从终端接收缓冲读取一个字符                             */
/* 输入参数 : 无                                                     */
/* 输出参数 : 无                                                     */
/* 返回值   : 读取到的字符                                           */
/* 调用说明 : 该调用直接读取当前终端的接收缓冲，如果无内容则返回空   */
/* 典型示例 : cTmp = IO_GetCharByPeek();                             */
/*-------------------------------------------------------------------*/
char   IO_GetCharByPeek();

/*-------------------------------------------------------------------*/
/* 函数名称 : IO_GetString                                           */
/* 功    能 : 从当前终端接收一个字符串                               */
/* 输入参数 : ulSize  :字符串允许的最大长度                          */
/*            bInvisable  :输入模式:FALSE－字符回显，TRUE－星号回显  */
/* 输出参数 : szInput :接收的的输入字符串                            */
/* 返回值   : 成功、失败                                             */
/* 调用说明 : 该调用导致终端任务阻塞，直到超时或接收到回车键. 调用者 */
/*            应对返回值进行判断, 如果失败,则认为接收到的内容无效，  */
/*            导致这种情况的最大可能是终端异常，程序应当立即返回，让 */
/*            系统去识别并处理这种异常                               */
/* 典型示例 : if (IO_GetString(&szUsername,16,FALSE) != TBS_SUCCESS) */
/*                return TBS_FAILURE;                                */
/*-------------------------------------------------------------------*/
ULONG   IO_GetString(char  *szInput, ULONG  ulSize, BOOL  bInvisable);

/*-------------------------------------------------------------------*/
/* 函数名称 : IO_GetConfirm                                          */
/* 功    能 : 等待用户输入确认信息                                   */
/* 输入参数 : szFormat       :确认的提示信息                         */
/*            ulDefaultChoice:缺省输入, 取值于 INPUT_NO/INPUT_YES    */
/* 输出参数 : 无                                                     */
/* 返回     : INPUT_NO/INPUT_YES/INPUT_TIMEOUT                       */
/* 调用说明 : 该调用导致终端任务阻塞，直到超时或接收到按键输入. 如果 */
/*            输入回车或三次均为'n'与'y'之外的内容, 返回缺省选项.    */
/*            超时时间统一定义为5秒                                  */
/* 典型示例 : if (IO_GetConfirm(CLI_KICK_OFF_ANTHOR, INPUT_NO)       */
/*                != INPUT_YES)                                      */
/*                ...... ;                                           */
/*-------------------------------------------------------------------*/
ULONG   IO_GetConfirm(PCSTR szFormat,  ULONG  ulDefaultChoice);

/*-------------------------------------------------------------------*/
/* 函数名称 : IO_SetScrollAuto                                       */
/* 功    能 : 设置当前终端为自动滚屏                                 */
/* 输入参数 : 无                                                     */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/* 调用说明 :                                                        */
/*-------------------------------------------------------------------*/
ULONG  IO_SetScrollAuto();

/*-------------------------------------------------------------------*/
/* 函数名称 : IO_SetScrollBack                                       */
/* 功    能 : 恢复当前终端的滚屏方式                                 */
/* 输入参数 : 无                                                     */
/* 输出参数 : 无                                                     */
/* 返回     : 成功、失败                                             */
/* 调用说明 : 该接口须与IO_SetScrollAuto配套使用                     */
/*-------------------------------------------------------------------*/
ULONG  IO_SetScrollBack();

/*-------------------------------------------------------------------*/
/* 函数名称 : IO_IsOutputBroken                                      */
/* 功    能 : 判断当前的输出是否被中止                               */
/* 输入参数 : ulTermID: 终端ID                                       */
/* 输出参数 : 无                                                     */
/* 返回     : TRUE/FALSE                                             */
/* 调用说明 : 该接口一般用于有较多的输出过程中调用                   */
/*-------------------------------------------------------------------*/
BOOL IO_IsOutputBroken();



/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/


#endif /* __CLI_PUB_H__ */



