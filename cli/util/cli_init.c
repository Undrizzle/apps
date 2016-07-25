/****************************************************************************
 文件名  : CLI_Init.c
 作者    : liuzequn
 版本    :
 完成日期:
 文件描述:  本文件实现命令行模块主要部分的初始化操作
 修改历史:
        1. 修改者   :
           时间     :
           版本     :
           修改原因 :
 ****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "cli_private.h"
#include "cli_term.h"
#include "cli_interpret.h"
#include "cli_io.h"
#include "../app/cli_cmd.h"
#include "cli_cmdreg.h"
#include "../cli_comm.h"
#include <signal.h>

/* demo version info */
char               m_szHostName[32] = "CLI";             /* 主机设备名     */
//char               m_szHostName[32] = "WEC9720EK";             /* 主机设备名     */

// 模式注册信息
ST_CMD_MODE_INFO m_stCmdModeInfo[] =
  /* 模式       父模式       权限           模式名      提示符       进入命令执行操作与帮助信息 */
{{CTM_GLOBAL,  NULL_MODE,  CLI_AL_QUERY,   "Global",   ">>"       , NULL, CLI_ML_NULL},
 {CTM_GENL,    CTM_GLOBAL,  CLI_AL_QUERY,   "Root",     ">"       , NULL, CLI_ML_NULL},
 //{CTM_CONFIG,  CTM_GENL,   CLI_AL_QUERY,   "Config",   ">>config>", NULL, CMDHELP_GENL_CM_CONFIG},
 /* 尝试修改模式*/
 //{CTM_INTERFACE,  CTM_GENL,   CLI_AL_ADMIN,   "Interface",   ">>interface>", NULL, CMDHELP_GENL_CM_INTERFACE},
 {CTM_IF_1_CLT,  CTM_GENL,   CLI_AL_ADMIN,   "clt/1",   ">>clt/1>", NULL, CMDHELP_GENL_CM_IF_CLT},
 {CTM_IF_2_CLT,  CTM_GENL,   CLI_AL_ADMIN,   "clt/2",   ">>clt/2>", NULL, CMDHELP_GENL_CM_IF_CLT},
 {CTM_IF_3_CLT,  CTM_GENL,   CLI_AL_ADMIN,   "clt/3",   ">>clt/3>", NULL, CMDHELP_GENL_CM_IF_CLT},
 {CTM_IF_4_CLT,  CTM_GENL,   CLI_AL_ADMIN,   "clt/4",   ">>clt/4>", NULL, CMDHELP_GENL_CM_IF_CLT},
 {CTM_IF_1_1_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/1",   ">>cnu/1/1>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_2_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/2",   ">>cnu/1/2>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_3_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/3",   ">>cnu/1/3>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_4_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/4",   ">>cnu/1/4>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_5_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/5",   ">>cnu/1/5>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_6_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/6",   ">>cnu/1/6>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_7_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/7",   ">>cnu/1/7>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_8_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/8",   ">>cnu/1/8>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_9_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/9",   ">>cnu/1/9>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_10_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/10",   ">>cnu/1/10>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_11_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/11",   ">>cnu/1/11>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_12_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/12",   ">>cnu/1/12>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_13_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/13",   ">>cnu/1/13>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_14_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/14",   ">>cnu/1/14>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_15_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/15",   ">>cnu/1/15>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_16_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/16",   ">>cnu/1/16>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_17_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/17",   ">>cnu/1/17>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_18_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/18",   ">>cnu/1/18>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_19_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/19",   ">>cnu/1/19>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_20_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/20",   ">>cnu/1/20>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_21_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/21",   ">>cnu/1/21>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_22_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/22",   ">>cnu/1/22>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_23_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/23",   ">>cnu/1/23>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_24_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/24",   ">>cnu/1/24>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_25_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/25",   ">>cnu/1/25>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_26_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/26",   ">>cnu/1/26>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_27_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/27",   ">>cnu/1/27>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_28_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/28",   ">>cnu/1/28>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_29_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/29",   ">>cnu/1/29>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_30_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/30",   ">>cnu/1/30>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_31_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/31",   ">>cnu/1/31>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_32_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/32",   ">>cnu/1/32>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_33_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/33",   ">>cnu/1/33>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_34_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/34",   ">>cnu/1/34>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_35_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/35",   ">>cnu/1/35>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_36_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/36",   ">>cnu/1/36>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_37_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/37",   ">>cnu/1/37>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_38_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/38",   ">>cnu/1/38>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_39_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/39",   ">>cnu/1/39>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_40_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/40",   ">>cnu/1/40>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_41_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/41",   ">>cnu/1/41>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_42_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/42",   ">>cnu/1/42>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_43_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/43",   ">>cnu/1/43>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_44_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/44",   ">>cnu/1/44>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_45_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/45",   ">>cnu/1/45>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_46_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/46",   ">>cnu/1/46>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_47_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/47",   ">>cnu/1/47>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_48_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/48",   ">>cnu/1/48>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_49_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/49",   ">>cnu/1/49>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_50_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/50",   ">>cnu/1/50>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_51_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/51",   ">>cnu/1/51>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_52_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/52",   ">>cnu/1/52>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_53_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/53",   ">>cnu/1/53>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_54_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/54",   ">>cnu/1/54>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_55_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/55",   ">>cnu/1/55>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_56_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/56",   ">>cnu/1/56>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_57_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/57",   ">>cnu/1/57>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_58_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/58",   ">>cnu/1/58>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_59_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/59",   ">>cnu/1/59>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_60_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/60",   ">>cnu/1/60>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_61_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/61",   ">>cnu/1/61>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_62_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/62",   ">>cnu/1/62>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_63_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/63",   ">>cnu/1/63>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_1_64_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/1/64",   ">>cnu/1/64>", NULL, CMDHELP_GENL_CM_IF_CNU},

 {CTM_IF_2_1_CNU,    CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/1",    ">>cnu/2/1>",   NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_2_CNU,    CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/2",    ">>cnu/2/2>",   NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_3_CNU,    CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/3",    ">>cnu/2/3>",   NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_4_CNU,    CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/4",    ">>cnu/2/4>",   NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_5_CNU,    CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/5",    ">>cnu/2/5>",   NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_6_CNU,    CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/6",    ">>cnu/2/6>",   NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_7_CNU,    CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/7",    ">>cnu/2/7>",   NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_8_CNU,    CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/8",    ">>cnu/2/8>",   NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_9_CNU,    CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/9",    ">>cnu/2/9>",   NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_10_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/10",   ">>cnu/2/10>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_11_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/11",   ">>cnu/2/11>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_12_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/12",   ">>cnu/2/12>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_13_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/13",   ">>cnu/2/13>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_14_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/14",   ">>cnu/2/14>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_15_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/15",   ">>cnu/2/15>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_16_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/16",   ">>cnu/2/16>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_17_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/17",   ">>cnu/2/17>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_18_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/18",   ">>cnu/2/18>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_19_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/19",   ">>cnu/2/19>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_20_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/20",   ">>cnu/2/20>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_21_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/21",   ">>cnu/2/21>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_22_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/22",   ">>cnu/2/22>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_23_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/23",   ">>cnu/2/23>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_24_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/24",   ">>cnu/2/24>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_25_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/25",   ">>cnu/2/25>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_26_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/26",   ">>cnu/2/26>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_27_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/27",   ">>cnu/2/27>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_28_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/28",   ">>cnu/2/28>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_29_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/29",   ">>cnu/2/29>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_30_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/30",   ">>cnu/2/30>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_31_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/31",   ">>cnu/2/31>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_32_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/32",   ">>cnu/2/32>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_33_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/33",   ">>cnu/2/33>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_34_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/34",   ">>cnu/2/34>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_35_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/35",   ">>cnu/2/35>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_36_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/36",   ">>cnu/2/36>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_37_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/37",   ">>cnu/2/37>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_38_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/38",   ">>cnu/2/38>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_39_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/39",   ">>cnu/2/39>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_40_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/40",   ">>cnu/2/40>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_41_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/41",   ">>cnu/2/41>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_42_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/42",   ">>cnu/2/42>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_43_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/43",   ">>cnu/2/43>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_44_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/44",   ">>cnu/2/44>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_45_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/45",   ">>cnu/2/45>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_46_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/46",   ">>cnu/2/46>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_47_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/47",   ">>cnu/2/47>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_48_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/48",   ">>cnu/2/48>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_49_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/49",   ">>cnu/2/49>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_50_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/50",   ">>cnu/2/50>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_51_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/51",   ">>cnu/2/51>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_52_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/52",   ">>cnu/2/52>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_53_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/53",   ">>cnu/2/53>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_54_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/54",   ">>cnu/2/54>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_55_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/55",   ">>cnu/2/55>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_56_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/56",   ">>cnu/2/56>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_57_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/57",   ">>cnu/2/57>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_58_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/58",   ">>cnu/2/58>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_59_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/59",   ">>cnu/2/59>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_60_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/60",   ">>cnu/2/60>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_61_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/61",   ">>cnu/2/61>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_62_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/62",   ">>cnu/2/62>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_63_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/63",   ">>cnu/2/63>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_2_64_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/2/64",   ">>cnu/2/64>", NULL, CMDHELP_GENL_CM_IF_CNU},

 {CTM_IF_3_1_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/1",    ">>cnu/3/1>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_2_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/2",    ">>cnu/3/2>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_3_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/3",    ">>cnu/3/3>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_4_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/4",    ">>cnu/3/4>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_5_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/5",    ">>cnu/3/5>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_6_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/6",    ">>cnu/3/6>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_7_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/7",    ">>cnu/3/7>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_8_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/8",    ">>cnu/3/8>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_9_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/9",    ">>cnu/3/9>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_10_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/10",   ">>cnu/3/10>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_11_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/11",   ">>cnu/3/11>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_12_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/12",   ">>cnu/3/12>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_13_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/13",   ">>cnu/3/13>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_14_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/14",   ">>cnu/3/14>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_15_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/15",   ">>cnu/3/15>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_16_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/16",   ">>cnu/3/16>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_17_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/17",   ">>cnu/3/17>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_18_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/18",   ">>cnu/3/18>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_19_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/19",   ">>cnu/3/19>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_20_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/20",   ">>cnu/3/20>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_21_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/21",   ">>cnu/3/21>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_22_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/22",   ">>cnu/3/22>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_23_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/23",   ">>cnu/3/23>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_24_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/24",   ">>cnu/3/24>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_25_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/25",   ">>cnu/3/25>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_26_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/26",   ">>cnu/3/26>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_27_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/27",   ">>cnu/3/27>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_28_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/28",   ">>cnu/3/28>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_29_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/29",   ">>cnu/3/29>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_30_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/30",   ">>cnu/3/30>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_31_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/31",   ">>cnu/3/31>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_32_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/32",   ">>cnu/3/32>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_33_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/33",   ">>cnu/3/33>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_34_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/34",   ">>cnu/3/34>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_35_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/35",   ">>cnu/3/35>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_36_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/36",   ">>cnu/3/36>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_37_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/37",   ">>cnu/3/37>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_38_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/38",   ">>cnu/3/38>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_39_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/39",   ">>cnu/3/39>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_40_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/40",   ">>cnu/3/40>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_41_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/41",   ">>cnu/3/41>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_42_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/42",   ">>cnu/3/42>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_43_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/43",   ">>cnu/3/43>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_44_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/44",   ">>cnu/3/44>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_45_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/45",   ">>cnu/3/45>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_46_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/46",   ">>cnu/3/46>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_47_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/47",   ">>cnu/3/47>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_48_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/48",   ">>cnu/3/48>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_49_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/49",   ">>cnu/3/49>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_50_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/50",   ">>cnu/3/50>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_51_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/51",   ">>cnu/3/51>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_52_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/52",   ">>cnu/3/52>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_53_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/53",   ">>cnu/3/53>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_54_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/54",   ">>cnu/3/54>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_55_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/55",   ">>cnu/3/55>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_56_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/56",   ">>cnu/3/56>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_57_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/57",   ">>cnu/3/57>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_58_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/58",   ">>cnu/3/58>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_59_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/59",   ">>cnu/3/59>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_60_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/60",   ">>cnu/3/60>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_61_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/61",   ">>cnu/3/61>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_62_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/62",   ">>cnu/3/62>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_63_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/63",   ">>cnu/3/63>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_3_64_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/3/64",   ">>cnu/3/64>", NULL, CMDHELP_GENL_CM_IF_CNU},

 {CTM_IF_4_1_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/1",    ">>cnu/4/1>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_2_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/2",    ">>cnu/4/2>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_3_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/3",    ">>cnu/4/3>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_4_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/4",    ">>cnu/4/4>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_5_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/5",    ">>cnu/4/5>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_6_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/6",    ">>cnu/4/6>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_7_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/7",    ">>cnu/4/7>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_8_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/8",    ">>cnu/4/8>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_9_CNU,   CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/9",    ">>cnu/4/9>",  NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_10_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/10",   ">>cnu/4/10>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_11_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/11",   ">>cnu/4/11>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_12_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/12",   ">>cnu/4/12>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_13_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/13",   ">>cnu/4/13>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_14_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/14",   ">>cnu/4/14>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_15_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/15",   ">>cnu/4/15>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_16_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/16",   ">>cnu/4/16>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_17_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/17",   ">>cnu/4/17>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_18_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/18",   ">>cnu/4/18>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_19_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/19",   ">>cnu/4/19>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_20_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/20",   ">>cnu/4/20>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_21_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/21",   ">>cnu/4/21>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_22_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/22",   ">>cnu/4/22>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_23_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/23",   ">>cnu/4/23>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_24_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/24",   ">>cnu/4/24>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_25_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/25",   ">>cnu/4/25>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_26_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/26",   ">>cnu/4/26>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_27_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/27",   ">>cnu/4/27>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_28_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/28",   ">>cnu/4/28>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_29_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/29",   ">>cnu/4/29>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_30_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/30",   ">>cnu/4/30>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_31_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/31",   ">>cnu/4/31>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_32_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/32",   ">>cnu/4/32>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_33_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/33",   ">>cnu/4/33>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_34_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/34",   ">>cnu/4/34>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_35_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/35",   ">>cnu/4/35>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_36_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/36",   ">>cnu/4/36>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_37_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/37",   ">>cnu/4/37>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_38_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/38",   ">>cnu/4/38>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_39_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/39",   ">>cnu/4/39>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_40_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/40",   ">>cnu/4/40>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_41_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/41",   ">>cnu/4/41>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_42_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/42",   ">>cnu/4/42>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_43_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/43",   ">>cnu/4/43>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_44_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/44",   ">>cnu/4/44>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_45_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/45",   ">>cnu/4/45>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_46_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/46",   ">>cnu/4/46>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_47_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/47",   ">>cnu/4/47>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_48_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/48",   ">>cnu/4/48>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_49_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/49",   ">>cnu/4/49>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_50_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/50",   ">>cnu/4/50>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_51_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/51",   ">>cnu/4/51>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_52_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/52",   ">>cnu/4/52>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_53_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/53",   ">>cnu/4/53>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_54_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/54",   ">>cnu/4/54>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_55_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/55",   ">>cnu/4/55>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_56_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/56",   ">>cnu/4/56>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_57_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/57",   ">>cnu/4/57>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_58_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/58",   ">>cnu/4/58>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_59_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/59",   ">>cnu/4/59>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_60_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/60",   ">>cnu/4/60>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_61_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/61",   ">>cnu/4/61>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_62_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/62",   ">>cnu/4/62>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_63_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/63",   ">>cnu/4/63>", NULL, CMDHELP_GENL_CM_IF_CNU},
 {CTM_IF_4_64_CNU,  CTM_GENL,   CLI_AL_ADMIN,   "cnu/4/64",   ">>cnu/4/64>", NULL, CMDHELP_GENL_CM_IF_CNU},

 // TODO: 此处添加更多命令模式

 {NULL_MODE}
};

/*********************************************************************/
/* 函数名称 : CLI_ProcessInit()                                      */
/* 函数功能 : 模块分段初始化函数                                     */
/* 输入参数 :                                                        */
/* 输出参数 :                                                        */
/* 返回     :                                                        */
/* 上层函数 :                                                        */
/* 创建者   :                                                        */
/* 修改记录 :                                                        */
/*********************************************************************/
ULONG  CLI_ProcessInit()
{
    ULONG  ulRet = TBS_SUCCESS;

    signal(SIGINT, CLI_ProcForCtrlC);

    ulRet += CLI_CommInit();

    /* 模式结构初始化 */
    CLI_InitSysModes();
    /* 终端任务表初始化*/
    CLI_TermDataInit();
    /* 解析环境初始化  */
    ulRet += CLI_ResetEnviroment();

    /* 注册命令模式与模式对象 */
    ulRet += CLI_InitCmdTree(m_stCmdModeInfo);

    /* --------各模块命令注册开始位置--------{                  */

	/* 注册本模块命令 */
    ulRet += CLI_SysCmdReg();

    // TODO: 各模块命令注册

    /* --------各模块命令注册结束位置--------}                  */


    /* 将全局命令链接到所有模式 */
    ulRet += CLI_GlobalCmdLink();

    return ulRet;
}


void CLI_ProcessDestory()
{
	CLI_CommDestroy();	
	CLI_TermDestroy();
}

/*********************************************************************/
/* 函数名称 : CLI_GetHostName()                                      */
/* 函数功能 : 对外提供的主机名获取函数                               */
/* 输入参数 :                                                        */
/* 输出参数 :                                                        */
/* 返回     : 主机名字符串                                           */
/*********************************************************************/
char  *CLI_GetHostName(_VOID   )
{
    return m_szHostName;
}
    

#ifdef __cplusplus
}
#endif

