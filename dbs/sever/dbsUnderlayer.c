#include <sqlite3.h>
#include <assert.h>

#include <dbsUnderlayer.h>
#include <boardapi.h>

uint8_t gBuf_dbsUnderlayer[DBS_SQL_LARGE_LEN] = {0};

/* sql 语句的调试开关 */
static int DBS_UNDERLAYER_DEBUG_ENABLE = 0;

/* 数据库操作句柄，该句柄在DBS模块初始化时打开，
** 直到进程结束才关闭*/
sqlite3 *dbsSystemFileHandle;

/* 描述数据库中数据表名称的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_DB_DESC db_system[DBS_SYS_TBL_TOTAL_NUM] = 
{
	{DBS_SYS_TBL_ID_CLIROLE, 	"tbl_cliroles",	DBS_SYS_TBL_COLS_CLIROLE},
	{DBS_SYS_TBL_ID_CLT,		"tbl_clt",		DBS_SYS_TBL_COLS_CLT},
	{DBS_SYS_TBL_ID_CLTPRO,	"tbl_cltconf",	DBS_SYS_TBL_COLS_CLTPRO},
	{DBS_SYS_TBL_ID_CNU,		"tbl_cnu",		DBS_SYS_TBL_COLS_CNU},
	{DBS_SYS_TBL_ID_CNUDEPRO,	"tbl_depro",	DBS_SYS_TBL_COLS_CNUDEPRO},
	{DBS_SYS_TBL_ID_NETWORK,	"tbl_network",	DBS_SYS_TBL_COLS_NETWORK},
	{DBS_SYS_TBL_ID_CNUPRO,	"tbl_profile",	DBS_SYS_TBL_COLS_CNUPRO},
	{DBS_SYS_TBL_ID_SNMPINFO,	"tbl_snmp",	DBS_SYS_TBL_COLS_SNMPINFO},
	{DBS_SYS_TBL_ID_SWMGMT,	"tbl_swmgmt",	DBS_SYS_TBL_COLS_SWMGMT},
	{DBS_SYS_TBL_ID_SYSINFO,	"tbl_sysinfo",	DBS_SYS_TBL_COLS_SYSINFO},
	{DBS_SYS_TBL_ID_TEMPLATE,	"tbl_template",	DBS_SYS_TBL_COLS_TEMPLATE}	
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_cliroles[DBS_SYS_TBL_COLS_CLIROLE] = 
{
	/*	类型，	长度，	主键，允许空，	列名*/
	{DBS_SYS_TBL_CLIROLES_COL_ID_ID,	SQLITE_INTEGER,	4,	BOOL_TRUE,	BOOL_FALSE,	"id"},
	{DBS_SYS_TBL_CLIROLES_COL_ID_USER,	SQLITE3_TEXT,	32,	BOOL_FALSE,	BOOL_TRUE,	"col_user"},
	{DBS_SYS_TBL_CLIROLES_COL_ID_PWD,	SQLITE3_TEXT,	16,	BOOL_FALSE,	BOOL_TRUE,	"col_pwd"}
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_clt[DBS_SYS_TBL_COLS_CLT] = 
{
	/*	类型，	长度，	主键，允许空，	列名*/
	{DBS_SYS_TBL_CLT_COL_ID_ID,		SQLITE_INTEGER,	4,	BOOL_TRUE,	BOOL_FALSE,	"id"},
	{DBS_SYS_TBL_CLT_COL_ID_MODEL,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_model"},
	{DBS_SYS_TBL_CLT_COL_ID_MAC,		SQLITE3_TEXT,	32,	BOOL_FALSE,	BOOL_TRUE,	"col_mac"},
	{DBS_SYS_TBL_CLT_COL_ID_STS,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_sts"},
	{DBS_SYS_TBL_CLT_COL_ID_MAXSTATS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_maxStas"},
	{DBS_SYS_TBL_CLT_COL_ID_NUMSTATS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_numStas"},
	{DBS_SYS_TBL_CLT_COL_ID_SWVER,	SQLITE3_TEXT,	128,	BOOL_FALSE,	BOOL_TRUE,	"col_swVersion"},
	{DBS_SYS_TBL_CLT_COL_ID_SYNCH,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_synch"},
	{DBS_SYS_TBL_CLT_COL_ID_ROWSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_FALSE,	"col_row_sts"}
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_cltconf[DBS_SYS_TBL_COLS_CLTPRO] = 
{
	/*	类型，	长度，	主键，允许空，	列名*/
	{DBS_SYS_TBL_CLTPRO_COL_ID_ID,			SQLITE_INTEGER,	4,	BOOL_TRUE,	BOOL_FALSE,	"id"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_ROWSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_FALSE,	"col_row_sts"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_BASE,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_base"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_MACLIMIT,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_macLimit"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_CURATE,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_curate"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_CDRATE,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cdrate"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_LOAGE,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_loagTime"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_REAGE,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_reagTime"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_IGMPPRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_igmpPri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_UNIPRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_unicastPri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_AVSPRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_avsPri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_MCPRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_mcastPri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_TBAPRISTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tbaPriSts"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_COSPRISTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cosPriSts"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_COS0PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos0pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_COS1PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos1pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_COS2PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos2pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_COS3PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos3pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_COS4PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos4pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_COS5PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos5pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_COS6PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos6pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_COS7PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos7pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_TOSPRISTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tosPriSts"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_TOS0PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos0pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_TOS1PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos1pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_TOS2PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos2pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_TOS3PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos3pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_TOS4PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos4pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_TOS5PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos5pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_TOS6PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos6pri"},
	{DBS_SYS_TBL_CLTPRO_COL_ID_TOS7PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos7pri"}
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_cnu[DBS_SYS_TBL_COLS_CNU] = 
{
	/*	类型，	长度，	主键，允许空，	列名*/
	{DBS_SYS_TBL_CNU_COL_ID_ID,		SQLITE_INTEGER,	4,	BOOL_TRUE,	BOOL_FALSE,	"id"},
	{DBS_SYS_TBL_CNU_COL_ID_MODEL,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_model"},
	{DBS_SYS_TBL_CNU_COL_ID_MAC,		SQLITE3_TEXT,	32,	BOOL_FALSE,	BOOL_TRUE,	"col_mac"},
	{DBS_SYS_TBL_CNU_COL_ID_STS,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_sts"},
	{DBS_SYS_TBL_CNU_COL_ID_AUTH,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_auth"},
	{DBS_SYS_TBL_CNU_COL_ID_VER,		SQLITE3_TEXT,	128,	BOOL_FALSE,	BOOL_TRUE,	"col_ver"},
	{DBS_SYS_TBL_CNU_COL_ID_RX,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_rx"},
	{DBS_SYS_TBL_CNU_COL_ID_TX,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tx"},
	{DBS_SYS_TBL_CNU_COL_ID_SNR,		SQLITE3_TEXT,	16,	BOOL_FALSE,	BOOL_TRUE,	"col_snr"},
	{DBS_SYS_TBL_CNU_COL_ID_BPC,		SQLITE3_TEXT,	16,	BOOL_FALSE,	BOOL_TRUE,	"col_bpc"},
	{DBS_SYS_TBL_CNU_COL_ID_ATT,		SQLITE3_TEXT,	16,	BOOL_FALSE,	BOOL_TRUE,	"col_att"},
	{DBS_SYS_TBL_CNU_COL_ID_SYNCH,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_synch"},
	{DBS_SYS_TBL_CNU_COL_ID_ROWSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_FALSE,	"col_row_sts"},
	{DBS_SYS_TBL_CNU_COL_ID_AUTOSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_FALSE,	"col_auto_sts"},
	{DBS_SYS_TBL_CNU_COL_ID_USERHFID, SQLITE3_TEXT,64,  BOOL_FALSE,   BOOL_TRUE,    "col_user_hfid"}
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_depro[DBS_SYS_TBL_COLS_CNUDEPRO] = 
{
	/*	类型，	长度，	主键，允许空，	列名*/
	{DBS_SYS_TBL_DEPRO_COL_ID_ID,			SQLITE_INTEGER,	4,	BOOL_TRUE,	BOOL_FALSE,	"id"},
	{DBS_SYS_TBL_DEPRO_COL_ID_BASE,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_base"},
	{DBS_SYS_TBL_DEPRO_COL_ID_MACLIMIT,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_macLimit"},
	{DBS_SYS_TBL_DEPRO_COL_ID_CURATE,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_curate"},
	{DBS_SYS_TBL_DEPRO_COL_ID_CDRATE,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cdrate"},
	{DBS_SYS_TBL_DEPRO_COL_ID_LOAGE,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_loagTime"},
	{DBS_SYS_TBL_DEPRO_COL_ID_REAGE,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_reagTime"},
	{DBS_SYS_TBL_DEPRO_COL_ID_IGMPPRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_igmpPri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_UNIPRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_unicastPri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_AVSPRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_avsPri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_MCPRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_mcastPri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_TBAPRISTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tbaPriSts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_COSPRISTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cosPriSts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_COS0PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos0pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_COS1PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos1pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_COS2PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos2pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_COS3PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos3pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_COS4PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos4pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_COS5PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos5pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_COS6PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos6pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_COS7PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos7pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_TOSPRISTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tosPriSts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_TOS0PRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos0pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_TOS1PRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos1pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_TOS2PRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos2pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_TOS3PRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos3pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_TOS4PRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos4pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_TOS5PRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos5pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_TOS6PRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos6pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_TOS7PRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos7pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_SFBSTS,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_sfbSts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_SFUSTS,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_sfuSts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_SFMSTS,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_sfmSts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_SFRATE,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_sfRate"},
	{DBS_SYS_TBL_DEPRO_COL_ID_VLANSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_vlanSts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH1VID,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth1vid"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH2VID,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth2vid"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH3VID,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth3vid"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH4VID,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth4vid"},
	{DBS_SYS_TBL_DEPRO_COL_ID_PORTPRISTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_portPriSts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH1PRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth1pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH2PRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth2pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH3PRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth3pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH4PRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth4pri"},
	{DBS_SYS_TBL_DEPRO_COL_ID_RXLIMITSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_rxLimitSts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_CPURX,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cpuPortRxRate"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH1RX,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth1rx"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH2RX,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth2rx"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH3RX,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth3rx"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH4RX,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth4rx"},
	{DBS_SYS_TBL_DEPRO_COL_ID_TXLIMITSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_txLimitSts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_CPUTX,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cpuPortTxRate"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH1TX,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth1tx"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH2TX,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth2tx"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH3TX,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth3tx"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH4TX,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth4tx"},
	{DBS_SYS_TBL_DEPRO_COL_ID_PSCTL,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_psctlSts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_CPUPSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cpuPortSts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH1STS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth1sts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH2STS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth2sts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH3STS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth3sts"},
	{DBS_SYS_TBL_DEPRO_COL_ID_ETH4STS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth4sts"}
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_network[DBS_SYS_TBL_COLS_NETWORK] = 
{
	/*	类型，	长度，	主键，允许空，	列名*/
	{DBS_SYS_TBL_NETWORK_COL_ID_ID,		SQLITE_INTEGER,	4,	BOOL_TRUE,	BOOL_FALSE,	"id"},
	{DBS_SYS_TBL_NETWORK_COL_ID_IP,		SQLITE3_TEXT,	32,	BOOL_FALSE,	BOOL_TRUE,	"col_ip"},
	{DBS_SYS_TBL_NETWORK_COL_ID_MASK,		SQLITE3_TEXT,	32,	BOOL_FALSE,	BOOL_TRUE,	"col_netmask"},
	{DBS_SYS_TBL_NETWORK_COL_ID_GW,		SQLITE3_TEXT,	32,	BOOL_FALSE,	BOOL_TRUE,	"col_gw"},
	{DBS_SYS_TBL_NETWORK_COL_ID_DHCP,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_dhcp"},
	{DBS_SYS_TBL_NETWORK_COL_ID_DNS,		SQLITE3_TEXT,	32,	BOOL_FALSE,	BOOL_TRUE,	"col_dns"},
	{DBS_SYS_TBL_NETWORK_COL_ID_MAC,		SQLITE3_TEXT,	32,	BOOL_FALSE,	BOOL_TRUE,	"col_mac"},
	{DBS_SYS_TBL_NETWORK_COL_ID_MVSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_mvlan_sts"},
	{DBS_SYS_TBL_NETWORK_COL_ID_MVID,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_mvlan_id"}
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_profile[DBS_SYS_TBL_COLS_CNUPRO] = 
{
	/*	类型，	长度，	主键，允许空，	列名*/
	{DBS_SYS_TBL_PROFILE_COL_ID_ID,			SQLITE_INTEGER,	4,	BOOL_TRUE,	BOOL_FALSE,	"id"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ROWSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_FALSE,	"col_row_sts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_BASE,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_base"},
	{DBS_SYS_TBL_PROFILE_COL_ID_MACLIMIT,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_macLimit"},
	{DBS_SYS_TBL_PROFILE_COL_ID_CURATE,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_curate"},
	{DBS_SYS_TBL_PROFILE_COL_ID_CDRATE,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cdrate"},
	{DBS_SYS_TBL_PROFILE_COL_ID_LOAGE,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_loagTime"},
	{DBS_SYS_TBL_PROFILE_COL_ID_REAGE,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_reagTime"},
	{DBS_SYS_TBL_PROFILE_COL_ID_IGMPPRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_igmpPri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_UNIPRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_unicastPri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_AVSPRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_avsPri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_MCPRI,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_mcastPri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_TBAPRISTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tbaPriSts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_COSPRISTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cosPriSts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_COS0PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos0pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_COS1PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos1pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_COS2PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos2pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_COS3PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos3pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_COS4PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos4pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_COS5PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos5pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_COS6PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos6pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_COS7PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cos7pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_TOSPRISTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tosPriSts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_TOS0PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos0pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_TOS1PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos1pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_TOS2PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos2pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_TOS3PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos3pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_TOS4PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos4pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_TOS5PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos5pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_TOS6PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos6pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_TOS7PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tos7pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_SFBSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_sfbSts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_SFUSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_sfuSts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_SFMSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_sfmSts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_SFRATE,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_sfRate"},
	{DBS_SYS_TBL_PROFILE_COL_ID_VLANSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_vlanSts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH1VID,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth1vid"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH2VID,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth2vid"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH3VID,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth3vid"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH4VID,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth4vid"},
	{DBS_SYS_TBL_PROFILE_COL_ID_PORTPRISTS,SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_portPriSts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH1PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth1pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH2PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth2pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH3PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth3pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH4PRI,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth4pri"},
	{DBS_SYS_TBL_PROFILE_COL_ID_RXLIMITSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_rxLimitSts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_CPURX,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cpuPortRxRate"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH1RX,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth1rx"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH2RX,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth2rx"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH3RX,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth3rx"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH4RX,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth4rx"},
	{DBS_SYS_TBL_PROFILE_COL_ID_TXLIMITSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_txLimitSts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_CPUTX,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cpuPortTxRate"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH1TX,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth1tx"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH2TX,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth2tx"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH3TX,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth3tx"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH4TX,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth4tx"},
	{DBS_SYS_TBL_PROFILE_COL_ID_PSCTL,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_psctlSts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_CPUPSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cpuPortSts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH1STS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth1sts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH2STS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth2sts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH3STS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth3sts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH4STS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth4sts"},
	{DBS_SYS_TBL_PROFILE_COL_ID_UPLINKVID,  SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_uplinkvid"},
	{DBS_SYS_TBL_PROFILE_COL_ID_UPLINKVMODE,  SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_uplinkVMode"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH1VMODE,  SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth1VMode"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH2VMODE,  SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth2VMode"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH3VMODE,  SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth3VMode"},
	{DBS_SYS_TBL_PROFILE_COL_ID_ETH4VMODE,  SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth4VMode"}
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_snmp[DBS_SYS_TBL_COLS_SNMPINFO] = 
{
	/*	类型，	长度，	主键，允许空，	列名*/
	{DBS_SYS_TBL_SNMP_COL_ID_ID,	SQLITE_INTEGER,	4,	BOOL_TRUE,	BOOL_FALSE,	"id"},
	{DBS_SYS_TBL_SNMP_COL_ID_RC,	SQLITE3_TEXT,	64,	BOOL_FALSE,	BOOL_TRUE,	"col_rdcom"},
	{DBS_SYS_TBL_SNMP_COL_ID_WC,	SQLITE3_TEXT,	64,	BOOL_FALSE,	BOOL_TRUE,	"col_wrcom"},
	{DBS_SYS_TBL_SNMP_COL_ID_TC,	SQLITE3_TEXT,	64,	BOOL_FALSE,	BOOL_TRUE,	"col_trapcom"},
	{DBS_SYS_TBL_SNMP_COL_ID_SA,	SQLITE3_TEXT,	32,	BOOL_FALSE,	BOOL_TRUE,	"col_sina"},
	{DBS_SYS_TBL_SNMP_COL_ID_SB,	SQLITE3_TEXT,	32,	BOOL_FALSE,	BOOL_TRUE,	"col_sinb"},
	{DBS_SYS_TBL_SNMP_COL_ID_PA,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tpa"},
	{DBS_SYS_TBL_SNMP_COL_ID_PB,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_tpb"}
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_swmgmt[DBS_SYS_TBL_COLS_SWMGMT] = 
{
	/*	类型，	长度，	主键，允许空，	列名*/
	{DBS_SYS_TBL_SWMGMT_COL_ID_ID,	SQLITE_INTEGER,	4,	BOOL_TRUE,	BOOL_FALSE,	"id"},
	{DBS_SYS_TBL_SWMGMT_COL_ID_IP,	SQLITE3_TEXT,	32,	BOOL_FALSE,	BOOL_TRUE,	"col_ip"},
	{DBS_SYS_TBL_SWMGMT_COL_ID_PORT,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_port"},
	{DBS_SYS_TBL_SWMGMT_COL_ID_USR,	SQLITE3_TEXT,	32,	BOOL_FALSE,	BOOL_TRUE,	"col_user"},
	{DBS_SYS_TBL_SWMGMT_COL_ID_PWD,	SQLITE3_TEXT,	16,	BOOL_FALSE,	BOOL_TRUE,	"col_pwd"},
	{DBS_SYS_TBL_SWMGMT_COL_ID_PATH,	SQLITE3_TEXT,	256,	BOOL_FALSE,	BOOL_TRUE,	"col_path"}
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_sysinfo[DBS_SYS_TBL_COLS_SYSINFO] = 
{
	/*	类型，	长度，	主键，允许空，	列名*/
	{DBS_SYS_TBL_SYSINFO_COL_ID_ID,		SQLITE_INTEGER,	4,	BOOL_TRUE,	BOOL_FALSE,	"id"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_MODEL,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_model"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_MAXCLT,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_maxclt"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_HWVER,	SQLITE3_TEXT,	64,	BOOL_FALSE,	BOOL_TRUE,	"col_hwver"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_BVER,		SQLITE3_TEXT,	64,	BOOL_FALSE,	BOOL_TRUE,	"col_bver"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_KVER,		SQLITE3_TEXT,	64,	BOOL_FALSE,	BOOL_TRUE,	"col_kver"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_APPVER,	SQLITE3_TEXT,	64,	BOOL_FALSE,	BOOL_TRUE,	"col_appver"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_FS,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_flashsize"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_RS,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_ramsize"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_WLCTL,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_wlctl"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_AU,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_autoud"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_AC,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_autoconf"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_WDT,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_wdt"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_MF,		SQLITE3_TEXT,	128,	BOOL_FALSE,	BOOL_TRUE,	"col_mfinfo"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_P6RXD,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_p6rxdelay"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_P6TXD,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_p6txdelay"},
	{DBS_SYS_TBL_SYSINFO_COL_ID_APPHASH, SQLITE3_TEXT,    64,  BOOL_FALSE,  BOOL_TRUE,   "col_apphash"}
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_template[DBS_SYS_TBL_COLS_TEMPLATE] = 
{
	/*	类型，	长度，	主键，允许空，	列名*/
	{DBS_SYS_TBL_TEMPLATE_COL_ID_ID,		SQLITE_INTEGER,	4,	BOOL_TRUE,	BOOL_FALSE,	"id"},
	{DBS_SYS_TBL_TEMPLATE_COL_ID_TEMPAUTOSTS,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_FALSE,	"col_tempAutoSts"},
	{DBS_SYS_TBL_TEMPLATE_COL_ID_CURTEMP,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_curTemp"},
	{DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANADDSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth1VlanAddSts"},
	{DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANSTART,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth1VlanStart"},
	{DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANSTOP,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth1VlanStop"},
	{DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANADDSTS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth2VlanAddSts"},
	{DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANSTART,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth2VlanStart"},
	{DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANSTOP,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth2VlanStop"},
	{DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANADDSTS,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eht3VlanAddSts"},
	{DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANSTART,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth3VlanStart"},
	{DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANSTOP,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth3VlanStop"},
	{DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANADDSTS,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth4VlanAddSts"},
	{DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANSTART,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth4VlanStart"},
	{DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANSTOP,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_eth4VlanStop"}
	
};


#define __dbsUnderlayerSQL(...);	\
{	\
	if( DBS_UNDERLAYER_DEBUG_ENABLE ) \
	{	\
		fprintf(stderr, __VA_ARGS__);	\
		fprintf(stderr, ";\n");	\
	} \
}

int __is_col_pk(uint16_t tbl, uint16_t col)
{
	/* 判断表是否存在*/
	if( tbl >= DBS_SYS_TBL_TOTAL_NUM )
	{
		return BOOL_FALSE;
	}
	/* 判断列数量是否超出定义*/
	if( col >= (db_system[tbl].tbl_cols) )
	{
		return BOOL_FALSE;
	}
	switch(db_system[tbl].tbl_id)
	{
		case DBS_SYS_TBL_ID_CLIROLE:
		{
			return ((tbl_cliroles[col].isPrimaryKey)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_CLT:
		{
			return ((tbl_clt[col].isPrimaryKey)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_CLTPRO:
		{
			return ((tbl_cltconf[col].isPrimaryKey)?BOOL_TRUE:BOOL_FALSE);
		}	
		case DBS_SYS_TBL_ID_CNU:
		{
			return ((tbl_cnu[col].isPrimaryKey)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_CNUDEPRO:
		{
			return ((tbl_depro[col].isPrimaryKey)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_NETWORK:
		{
			return ((tbl_network[col].isPrimaryKey)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_CNUPRO:
		{
			return ((tbl_profile[col].isPrimaryKey)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_SNMPINFO:
		{
			return ((tbl_snmp[col].isPrimaryKey)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_SWMGMT:
		{
			return ((tbl_swmgmt[col].isPrimaryKey)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_SYSINFO:
		{
			return ((tbl_sysinfo[col].isPrimaryKey)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_TEMPLATE:
		{
			return ((tbl_template[col].isPrimaryKey)?BOOL_TRUE:BOOL_FALSE);
		}
		default:
		{
			return BOOL_FALSE;
		}
	}
}

int __is_col_allow_null(uint16_t tbl, uint16_t col)
{
	/* 判断表是否存在*/
	if( tbl >= DBS_SYS_TBL_TOTAL_NUM )
	{
		return BOOL_TRUE;
	}
	/* 判断列数量是否超出定义*/
	if( col >= (db_system[tbl].tbl_cols) )
	{
		return BOOL_TRUE;
	}
	
	switch(db_system[tbl].tbl_id)
	{
		case DBS_SYS_TBL_ID_CLIROLE:
		{
			return ((tbl_cliroles[col].allowNull)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_CLT:
		{
			return ((tbl_clt[col].allowNull)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_CLTPRO:
		{
			return ((tbl_cltconf[col].allowNull)?BOOL_TRUE:BOOL_FALSE);
		}	
		case DBS_SYS_TBL_ID_CNU:
		{
			return ((tbl_cnu[col].allowNull)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_CNUDEPRO:
		{
			return ((tbl_depro[col].allowNull)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_NETWORK:
		{
			return ((tbl_network[col].allowNull)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_CNUPRO:
		{
			return ((tbl_profile[col].allowNull)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_SNMPINFO:
		{
			return ((tbl_snmp[col].allowNull)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_SWMGMT:
		{
			return ((tbl_swmgmt[col].allowNull)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_SYSINFO:
		{
			return ((tbl_sysinfo[col].allowNull)?BOOL_TRUE:BOOL_FALSE);
		}
		case DBS_SYS_TBL_ID_TEMPLATE:
		{
			return ((tbl_template[col].allowNull)?BOOL_TRUE:BOOL_FALSE);
		}
		default:
		{
			return BOOL_TRUE;
		}
	}
}

int __get_col_type(uint16_t tbl, uint16_t col)
{
	/* 判断表是否存在*/
	if( tbl >= DBS_SYS_TBL_TOTAL_NUM )
	{
		return SQLITE_NULL;
	}
	/* 判断列数量是否超出定义*/
	if( col >= (db_system[tbl].tbl_cols) )
	{
		return SQLITE_NULL;
	}
	
	switch(db_system[tbl].tbl_id)
	{
		case DBS_SYS_TBL_ID_CLIROLE:
		{
			return tbl_cliroles[col].col_type;
		}
		case DBS_SYS_TBL_ID_CLT:
		{
			return tbl_clt[col].col_type;
		}
		case DBS_SYS_TBL_ID_CLTPRO:
		{
			return tbl_cltconf[col].col_type;
		}	
		case DBS_SYS_TBL_ID_CNU:
		{
			return tbl_cnu[col].col_type;
		}
		case DBS_SYS_TBL_ID_CNUDEPRO:
		{
			return tbl_depro[col].col_type;
		}
		case DBS_SYS_TBL_ID_NETWORK:
		{
			return tbl_network[col].col_type;
		}
		case DBS_SYS_TBL_ID_CNUPRO:
		{
			return tbl_profile[col].col_type;
		}
		case DBS_SYS_TBL_ID_SNMPINFO:
		{
			return tbl_snmp[col].col_type;
		}
		case DBS_SYS_TBL_ID_SWMGMT:
		{
			return tbl_swmgmt[col].col_type;
		}
		case DBS_SYS_TBL_ID_SYSINFO:
		{
			return tbl_sysinfo[col].col_type;
		}
		case DBS_SYS_TBL_ID_TEMPLATE:
		{
			return tbl_template[col].col_type;
		}

		default:
		{
			return SQLITE_NULL;
		}
	}
}

int __dbs_underlayer_get_count(uint16_t tbl, uint32_t *count)
{
	int ret = SQLITE_ERROR;
	
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	
	sqlite3_stmt * stmt = NULL;
	const char *zErrMsg = NULL;
	
	assert( NULL != count );	
	
	/* 判断表是否存在*/
	if( tbl >= DBS_SYS_TBL_TOTAL_NUM )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_count() DBS_SYS_TBL_TOTAL_NUM !\n");
		return SQLITE_ERROR;
	}
	
	/* 组装SQL语句*/
	sprintf(sql, "SELECT COUNT(*) FROM [%s]", db_system[tbl].tbl_name);
	__dbsUnderlayerSQL(sql);

	do
	{
		ret = sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &zErrMsg);
		if( SQLITE_BUSY == ret )
		{
			usleep(10000);
			continue;
		}
		else if( SQLITE_OK != ret )
		{
			fprintf(stderr, "ERROR: __dbs_underlayer_get_count()->sqlite3_prepare() !\n");
			ret = SQLITE_ERROR;
			break;
		}
		else
		{
			if( SQLITE_ROW == sqlite3_step(stmt) )
			{
				*count = sqlite3_column_int(stmt, 0);
				ret = SQLITE_OK;
				break;
			}
			else
			{
				fprintf(stderr, "ERROR: __dbs_underlayer_get_count()->sqlite3_step() !\n");
				ret = SQLITE_ERROR;
				break;
			}
		}
	}while( SQLITE_BUSY == ret );

	sqlite3_finalize(stmt);
	return ret;
}

int __dbs_underlayer_sql_exec(uint8_t *sql)
{
	int ret = SQLITE_ERROR;
	char *err_msg = NULL;

	assert( NULL != sql );
	
	do
	{
		ret = sqlite3_exec(dbsSystemFileHandle, sql, 0, 0, &err_msg);	
		if(ret == SQLITE_BUSY)
		{
			usleep(10000);
			continue;
		}
		else if( ret != SQLITE_OK )
		{
			fprintf(stderr, "\r\n\r\n  ERROR: [__dbs_underlayer_sql_exec] [%s] [%s]\n", sql, err_msg);
			return SQLITE_ERROR;
		}		
	}while( SQLITE_BUSY == ret );
	
	return ret;
}


int __dbs_underlayer_SQLClearColumn(DB_COL_INFO *ci, uint8_t *sql)
{
	assert( NULL != ci );
	assert( NULL != sql );

	/* 判断表是否存在*/
	if( ci->tbl >= DBS_SYS_TBL_TOTAL_NUM )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLClearColumn : INVALID TBL !\n");
		return SQLITE_ERROR;
	}
	/* 判断列数量是否超出定义*/
	if( ci->col >= (db_system[ci->tbl].tbl_cols) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLClearColumn : INVALID COL !\n");
		return SQLITE_ERROR;
	}	

	/* 判断该列是否为主键，主键禁止修改*/
	if( BOOL_TRUE == __is_col_pk(ci->tbl, ci->col) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLClearColumn : COL IS PK !\n");
		return SQLITE_ERROR;
	}

	/* 判断该列是否允许为空*/
	if( BOOL_FALSE == __is_col_allow_null(ci->tbl, ci->col) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLClearColumn : NOT ALLOW NULL !\n");
		return SQLITE_ERROR;
	}

	/* 组装SQL语句*/
	switch(db_system[ci->tbl].tbl_id)
	{
		case DBS_SYS_TBL_ID_CLIROLE:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=null WHERE [id]=%d", 
				db_system[ci->tbl].tbl_name, tbl_cliroles[ci->col].col_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_CLT:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=null WHERE [id]=%d", 
				db_system[ci->tbl].tbl_name, tbl_clt[ci->col].col_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_CLTPRO:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=null WHERE [id]=%d", 
				db_system[ci->tbl].tbl_name, tbl_cltconf[ci->col].col_name, ci->row);
			break;
		}	
		case DBS_SYS_TBL_ID_CNU:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=null WHERE [id]=%d", 
				db_system[ci->tbl].tbl_name, tbl_cnu[ci->col].col_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_CNUDEPRO:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=null WHERE [id]=%d", 
				db_system[ci->tbl].tbl_name, tbl_depro[ci->col].col_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_NETWORK:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=null WHERE [id]=%d", 
				db_system[ci->tbl].tbl_name, tbl_network[ci->col].col_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_CNUPRO:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=null WHERE [id]=%d", 
				db_system[ci->tbl].tbl_name, tbl_profile[ci->col].col_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_SNMPINFO:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=null WHERE [id]=%d", 
				db_system[ci->tbl].tbl_name, tbl_snmp[ci->col].col_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_SWMGMT:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=null WHERE [id]=%d", 
				db_system[ci->tbl].tbl_name, tbl_swmgmt[ci->col].col_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_SYSINFO:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=null WHERE [id]=%d", 
				db_system[ci->tbl].tbl_name, tbl_sysinfo[ci->col].col_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_TEMPLATE:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=null WHERE [id]=%d", 
				db_system[ci->tbl].tbl_name, tbl_template[ci->col].col_name, ci->row);
			break;
		}
		default:
		{
			return SQLITE_ERROR;
		}
	}
	return SQLITE_OK;
}

int __dbs_underlayer_SQLGetInteger(DB_COL_INFO *ci, uint8_t *sql)
{
	assert( NULL != ci );
	assert( NULL != sql );

	/* 判断该列是否为整数*/
	if( SQLITE_INTEGER != __get_col_type(ci->tbl, ci->col) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetInteger(%d, %d) : NOT INTEGER !\n", 
			ci->tbl, ci->col);
		return SQLITE_ERROR;
	}

	/* 组装SQL语句*/	
	switch(db_system[ci->tbl].tbl_id)
	{
		case DBS_SYS_TBL_ID_CLIROLE:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_cliroles[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_CLT:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_clt[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_CLTPRO:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_cltconf[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}	
		case DBS_SYS_TBL_ID_CNU:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_cnu[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_CNUDEPRO:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_depro[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_NETWORK:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_network[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_CNUPRO:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_profile[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_SNMPINFO:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_snmp[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_SWMGMT:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_swmgmt[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_SYSINFO:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_sysinfo[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_TEMPLATE:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_template[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		default:
		{
			return SQLITE_ERROR;
		}
	}
	
	return SQLITE_OK;
}

int __dbs_underlayer_SQLGetText(DB_COL_INFO *ci, uint8_t *sql)
{
	assert( NULL != ci );
	assert( NULL != sql );

	/* 判断该列是否为TEXT */
	if( SQLITE3_TEXT != __get_col_type(ci->tbl, ci->col) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetText : NOT SQLITE3_TEXT !\n");
		return SQLITE_ERROR;
	}

	/* 组装SQL语句*/
	switch(db_system[ci->tbl].tbl_id)
	{
		case DBS_SYS_TBL_ID_CLIROLE:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_cliroles[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_CLT:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_clt[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_CLTPRO:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_cltconf[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}	
		case DBS_SYS_TBL_ID_CNU:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_cnu[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_CNUDEPRO:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_depro[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_NETWORK:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_network[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_CNUPRO:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_profile[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_SNMPINFO:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_snmp[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_SWMGMT:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_swmgmt[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_SYSINFO:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_sysinfo[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		case DBS_SYS_TBL_ID_TEMPLATE:
		{
			sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_template[ci->col].col_name, db_system[ci->tbl].tbl_name, ci->row);
			break;
		}
		default:
		{
			return SQLITE_ERROR;
		}
	}
	
	return SQLITE_OK;
}

int __dbs_underlayer_SQLUpdateInteger(DB_COL_VAR *v, uint8_t *sql)
{
	uint32_t col_value = 0;
	assert( NULL != v );
	assert( NULL != sql );

	if( SQLITE_INTEGER != v->ci.colType )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateInteger : NOT SQLITE_INTEGER !\n");
		return SQLITE_ERROR;
	}

	if( sizeof(uint32_t) != v->len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateInteger : SIZE !\n");
		return SQLITE_ERROR;
	}
	
	/* 判断该列是否为主键，主键禁止修改*/
	if( BOOL_TRUE == __is_col_pk(v->ci.tbl, v->ci.col) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateInteger : COL IS PK !\n");
		return SQLITE_ERROR;
	}

	/* 判断该列是否为整数*/
	if( SQLITE_INTEGER != __get_col_type(v->ci.tbl, v->ci.col) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateInteger : NOT INTEGER !\n");
		return SQLITE_ERROR;
	}

	col_value = *(uint32_t *)(v->data);

	/* 组装SQL语句*/
	switch(db_system[v->ci.tbl].tbl_id)
	{
		case DBS_SYS_TBL_ID_CLIROLE:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=%d WHERE [id]=%d", 
					db_system[v->ci.tbl].tbl_name, tbl_cliroles[v->ci.col].col_name, col_value, v->ci.row);
			break;
		}
		case DBS_SYS_TBL_ID_CLT:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=%d WHERE [id]=%d", 
					db_system[v->ci.tbl].tbl_name, tbl_clt[v->ci.col].col_name, col_value, v->ci.row);
			break;
		}
		case DBS_SYS_TBL_ID_CLTPRO:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=%d WHERE [id]=%d", 
					db_system[v->ci.tbl].tbl_name, tbl_cltconf[v->ci.col].col_name, col_value, v->ci.row);
			break;
		}	
		case DBS_SYS_TBL_ID_CNU:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=%d WHERE [id]=%d", 
					db_system[v->ci.tbl].tbl_name, tbl_cnu[v->ci.col].col_name, col_value, v->ci.row);
			break;
		}
		case DBS_SYS_TBL_ID_CNUDEPRO:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=%d WHERE [id]=%d", 
					db_system[v->ci.tbl].tbl_name, tbl_depro[v->ci.col].col_name, col_value, v->ci.row);
			break;
		}
		case DBS_SYS_TBL_ID_NETWORK:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=%d WHERE [id]=%d", 
					db_system[v->ci.tbl].tbl_name, tbl_network[v->ci.col].col_name, col_value, v->ci.row);
			break;
		}
		case DBS_SYS_TBL_ID_CNUPRO:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=%d WHERE [id]=%d", 
					db_system[v->ci.tbl].tbl_name, tbl_profile[v->ci.col].col_name, col_value, v->ci.row);
			break;
		}
		case DBS_SYS_TBL_ID_SNMPINFO:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=%d WHERE [id]=%d", 
					db_system[v->ci.tbl].tbl_name, tbl_snmp[v->ci.col].col_name, col_value, v->ci.row);
			break;
		}
		case DBS_SYS_TBL_ID_SWMGMT:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=%d WHERE [id]=%d", 
					db_system[v->ci.tbl].tbl_name, tbl_swmgmt[v->ci.col].col_name, col_value, v->ci.row);
			break;
		}
		case DBS_SYS_TBL_ID_SYSINFO:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=%d WHERE [id]=%d", 
					db_system[v->ci.tbl].tbl_name, tbl_sysinfo[v->ci.col].col_name, col_value, v->ci.row);
			break;
		}
		case DBS_SYS_TBL_ID_TEMPLATE:
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=%d WHERE [id]=%d", 
					db_system[v->ci.tbl].tbl_name, tbl_template[v->ci.col].col_name, col_value, v->ci.row);
			break;
		}
		default:
		{
			return SQLITE_ERROR;
		}
	}
	return SQLITE_OK;
}

int __dbs_underlayer_SQLUpdateText(DB_COL_VAR *v, uint8_t *sql)
{
	uint8_t col_value[DBS_COL_MAX_LEN] = {0};
	
	assert( NULL != v );
	assert( NULL != sql );

	if( SQLITE3_TEXT != v->ci.colType )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateText : CI NOT SQLITE_INTEGER !\n");
		return SQLITE_ERROR;
	}
	
	/* 判断该列是否为主键，主键禁止修改*/
	if( BOOL_TRUE == __is_col_pk(v->ci.tbl, v->ci.col) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateText : COL IS PK !\n");
		return SQLITE_ERROR;
	}

	/* 判断该列是否为TEXT */
	if( SQLITE3_TEXT != __get_col_type(v->ci.tbl, v->ci.col) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateText : COL NOT SQLITE3_TEXT !\n");
		return SQLITE_ERROR;
	}	

	/* 组装SQL语句*/
	switch(db_system[v->ci.tbl].tbl_id)
	{
		case DBS_SYS_TBL_ID_CLIROLE:
		{
			/* 判断文本是否超长*/
			if( v->len >= tbl_cliroles[v->ci.col].col_len )
			{
				fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateText : TEXT TOO LONG !\n");
				return SQLITE_ERROR;
			}			
			else
			{
				strncpy(col_value, v->data, v->len);
				col_value[v->len] = '\0';
				sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\" WHERE [id]=%d", 
				db_system[v->ci.tbl].tbl_name, tbl_cliroles[v->ci.col].col_name, col_value, v->ci.row);
			}
			break;
		}
		case DBS_SYS_TBL_ID_CLT:
		{
			/* 判断文本是否超长*/
			if( v->len >= tbl_clt[v->ci.col].col_len )
			{
				fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateText : TEXT TOO LONG !\n");
				return SQLITE_ERROR;
			}
			else
			{
				strncpy(col_value, v->data, v->len);
				col_value[v->len] = '\0';
				sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\" WHERE [id]=%d", 
				db_system[v->ci.tbl].tbl_name, tbl_clt[v->ci.col].col_name, col_value, v->ci.row);
			}
			break;
		}
		case DBS_SYS_TBL_ID_CLTPRO:
		{
			/* 判断文本是否超长*/
			if( v->len >= tbl_cltconf[v->ci.col].col_len )
			{
				fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateText : TEXT TOO LONG !\n");
				return SQLITE_ERROR;
			}
			else
			{
				strncpy(col_value, v->data, v->len);
				col_value[v->len] = '\0';
				sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\" WHERE [id]=%d", 
				db_system[v->ci.tbl].tbl_name, tbl_cltconf[v->ci.col].col_name, col_value, v->ci.row);
			}
			break;
		}	
		case DBS_SYS_TBL_ID_CNU:
		{
			/* 判断文本是否超长*/
			if( v->len >= tbl_cnu[v->ci.col].col_len )
			{
				fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateText : TEXT TOO LONG !\n");
				return SQLITE_ERROR;
			}
			else
			{
				strncpy(col_value, v->data, v->len);
				col_value[v->len] = '\0';
				sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\" WHERE [id]=%d", 
				db_system[v->ci.tbl].tbl_name, tbl_cnu[v->ci.col].col_name, col_value, v->ci.row);
			}
			break;
		}
		case DBS_SYS_TBL_ID_CNUDEPRO:
		{
			/* 判断文本是否超长*/
			if( v->len >= tbl_depro[v->ci.col].col_len )
			{
				fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateText : TEXT TOO LONG !\n");
				return SQLITE_ERROR;
			}
			else
			{
				strncpy(col_value, v->data, v->len);
				col_value[v->len] = '\0';
				sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\" WHERE [id]=%d", 
				db_system[v->ci.tbl].tbl_name, tbl_depro[v->ci.col].col_name, col_value, v->ci.row);
			}
			break;
		}
		case DBS_SYS_TBL_ID_NETWORK:
		{
			/* 判断文本是否超长*/
			if( v->len >= tbl_network[v->ci.col].col_len )
			{
				fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateText : TEXT TOO LONG !\n");
				return SQLITE_ERROR;
			}
			else
			{
				strncpy(col_value, v->data, v->len);
				col_value[v->len] = '\0';
				sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\" WHERE [id]=%d", 
				db_system[v->ci.tbl].tbl_name, tbl_network[v->ci.col].col_name, col_value, v->ci.row);
			}
			break;
		}
		case DBS_SYS_TBL_ID_CNUPRO:
		{
			/* 判断文本是否超长*/
			if( v->len >= tbl_profile[v->ci.col].col_len )
			{
				fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateText : TEXT TOO LONG !\n");
				return SQLITE_ERROR;
			}
			else
			{
				strncpy(col_value, v->data, v->len);
				col_value[v->len] = '\0';
				sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\" WHERE [id]=%d", 
				db_system[v->ci.tbl].tbl_name, tbl_profile[v->ci.col].col_name, col_value, v->ci.row);
			}
			break;
		}
		case DBS_SYS_TBL_ID_SNMPINFO:
		{
			/* 判断文本是否超长*/
			if( v->len >= tbl_snmp[v->ci.col].col_len )
			{
				fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateText : TEXT TOO LONG !\n");
				return SQLITE_ERROR;
			}
			else
			{
				strncpy(col_value, v->data, v->len);
				col_value[v->len] = '\0';
				sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\" WHERE [id]=%d", 
				db_system[v->ci.tbl].tbl_name, tbl_snmp[v->ci.col].col_name, col_value, v->ci.row);
			}
			break;
		}
		case DBS_SYS_TBL_ID_SWMGMT:
		{
			/* 判断文本是否超长*/
			if( v->len >= tbl_swmgmt[v->ci.col].col_len )
			{
				fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateText : TEXT TOO LONG !\n");
				return SQLITE_ERROR;
			}
			else
			{
				strncpy(col_value, v->data, v->len);
				col_value[v->len] = '\0';
				sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\" WHERE [id]=%d", 
				db_system[v->ci.tbl].tbl_name, tbl_swmgmt[v->ci.col].col_name, col_value, v->ci.row);
			}
			break;
		}
		case DBS_SYS_TBL_ID_SYSINFO:
		{
			/* 判断文本是否超长*/
			if( v->len >= tbl_sysinfo[v->ci.col].col_len )
			{
				fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateText : TEXT TOO LONG !\n");
				return SQLITE_ERROR;
			}
			else
			{
				strncpy(col_value, v->data, v->len);
				col_value[v->len] = '\0';
				sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\" WHERE [id]=%d", 
				db_system[v->ci.tbl].tbl_name, tbl_sysinfo[v->ci.col].col_name, col_value, v->ci.row);
			}
			break;
		}
		case DBS_SYS_TBL_ID_TEMPLATE:
		{
			/* 判断文本是否超长*/
			if( v->len >= tbl_template[v->ci.col].col_len )
			{
				fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateText : TEXT TOO LONG !\n");
				return SQLITE_ERROR;
			}
			else
			{
				strncpy(col_value, v->data, v->len);
				col_value[v->len] = '\0';
				sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\" WHERE [id]=%d", 
				db_system[v->ci.tbl].tbl_name, tbl_template[v->ci.col].col_name, col_value, v->ci.row);
			}
			break;
		}
		default:
		{
			return SQLITE_ERROR;
		}
	}
	return SQLITE_OK;
}

int __dbs_underlayer_SQLGetRowCliRole(uint32_t id, uint8_t *sql)
{
	assert( NULL != sql );
	int iCount = 0;

	if( id > 3 )
	{
		if( __dbs_underlayer_get_count(DBS_SYS_TBL_ID_CLIROLE, &iCount) != SQLITE_OK )
		{
			fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetRowCliRole : get_count !\n");
			return SQLITE_ERROR;
		}
		if( iCount < id )
		{
			fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetRowCliRole : id !\n");
			return SQLITE_ERROR;
		}
		else
		{
			sprintf(sql, "SELECT * FROM [%s] WHERE [id]=%d",
				db_system[DBS_SYS_TBL_ID_CLIROLE].tbl_name, id);
			return SQLITE_OK;
		}
	}
	else if( (id>=1)&&(id<=3))
	/* 判断行数量是否超出定义*/
	{
		sprintf(sql, "SELECT * FROM [%s] WHERE [id]=%d",
			db_system[DBS_SYS_TBL_ID_CLIROLE].tbl_name, id);
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetRowCliRole : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLGetRowClt(uint32_t id, uint8_t *sql)
{
	assert( NULL != sql );

	/* 判断行数量是否超出定义*/
	if( (id>=1)&&(id<=MAX_CLT_AMOUNT_LIMIT))
	{
		sprintf(sql, "SELECT * FROM [%s] WHERE [id]=%d",
			db_system[DBS_SYS_TBL_ID_CLT].tbl_name, id);
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetRowClt : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLGetRowCltConf(uint32_t id, uint8_t *sql)
{
	assert( NULL != sql );

	/* 判断行数量是否超出定义*/
	if( (id>=1)&&(id<=MAX_CLT_AMOUNT_LIMIT))
	{
		sprintf(sql, "SELECT * FROM [%s] WHERE [id]=%d",
			db_system[DBS_SYS_TBL_ID_CLTPRO].tbl_name, id);
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetRowCltConf : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLGetRowCnu(uint32_t id, uint8_t *sql)
{
	assert( NULL != sql );

	/* 判断行数量是否超出定义*/
	if( (id>=1)&&(id<=MAX_CNU_AMOUNT_LIMIT))
	{
		sprintf(sql, "SELECT * FROM [%s] WHERE [id]=%d",
			db_system[DBS_SYS_TBL_ID_CNU].tbl_name, id);
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetRowCnu : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLGetRowCnuDepro(uint32_t id, uint8_t *sql)
{
	assert( NULL != sql );

	/* 判断行数量是否超出定义*/
	if( (id>=1)&&(id<=12))
	{
		sprintf(sql, "SELECT * FROM [%s] WHERE [id]=%d",
			db_system[DBS_SYS_TBL_ID_CNUDEPRO].tbl_name, id);
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetRowCnuDepro : id=%d !\n", id);
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLGetRowCnuTemplate(uint32_t id, uint8_t *sql)
{
	assert( NULL != sql );

	/* 判断行数量是否超出定义*/
	if( (id>=1)&&(id<=1))
	{
		sprintf(sql, "SELECT * FROM [%s] WHERE [id]=%d",
			db_system[DBS_SYS_TBL_ID_TEMPLATE].tbl_name, id);
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetRowCnuTemplate : id=%d !\n", id);
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLGetRowNetwork(uint32_t id, uint8_t *sql)
{
	assert( NULL != sql );

	/* 判断行数量是否超出定义*/
	if( (id>=1)&&(id<=1))
	{
		sprintf(sql, "SELECT * FROM [%s] WHERE [id]=%d",
			db_system[DBS_SYS_TBL_ID_NETWORK].tbl_name, id);
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetRowNetwork : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLGetRowProfile(uint32_t id, uint8_t *sql)
{
	assert( NULL != sql );

	/* 判断行数量是否超出定义*/
	if( (id>=1)&&(id<=MAX_CNU_AMOUNT_LIMIT))
	{
		sprintf(sql, "SELECT * FROM [%s] WHERE [id]=%d",
			db_system[DBS_SYS_TBL_ID_CNUPRO].tbl_name, id);
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetRowProfile : id [%d]!\n", id);
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLGetRowSnmp(uint32_t id, uint8_t *sql)
{
	assert( NULL != sql );

	/* 判断行数量是否超出定义*/
	if( (id>=1)&&(id<=1))
	{
		sprintf(sql, "SELECT * FROM [%s] WHERE [id]=%d",
			db_system[DBS_SYS_TBL_ID_SNMPINFO].tbl_name, id);
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetRowSnmp : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLGetRowSwmgmt(uint32_t id, uint8_t *sql)
{
	assert( NULL != sql );

	/* 判断行数量是否超出定义*/
	if( (id>=1)&&(id<=1))
	{
		sprintf(sql, "SELECT * FROM [%s] WHERE [id]=%d",
			db_system[DBS_SYS_TBL_ID_SWMGMT].tbl_name, id);
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetRowSwmgmt : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLGetRowSysinfo(uint32_t id, uint8_t *sql)
{
	assert( NULL != sql );

	/* 判断行数量是否超出定义*/
	if( (id>=1)&&(id<=1))
	{
		sprintf(sql, "SELECT * FROM [%s] WHERE [id]=%d",
			db_system[DBS_SYS_TBL_ID_SYSINFO].tbl_name, id);
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetRowSysinfo : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLGetRowTemplate(uint32_t id, uint8_t *sql)
{
	assert( NULL != sql );

	/* 判断行数量是否超出定义*/
	if( (id>=1)&&(id<=1))
	{
		sprintf(sql, "SELECT * FROM [%s] WHERE [id]=%d",
			db_system[DBS_SYS_TBL_ID_TEMPLATE].tbl_name, id);
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLGetRowTemplate : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLSelectCnuByMac(char *mac, uint8_t *sql)
{
	assert( NULL != sql );
	assert( NULL != mac );

	sprintf(sql, "SELECT * FROM [%s] WHERE [col_mac]=\"%s\"",
			db_system[DBS_SYS_TBL_ID_CNU].tbl_name, mac);
	return SQLITE_OK;
}

int __dbs_underlayer_SQLUpdateRowCliRole(st_dbsCliRole *row, uint8_t *sql)
{
	assert( NULL != sql );
	assert( NULL != row );
	int iCount = 0;

	/* 判断SQLITE3_TEXT  类型列元素输入长度*/
	if( strlen(row->col_user) >= tbl_cliroles[DBS_SYS_TBL_CLIROLES_COL_ID_USER].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowCliRole : len col_user !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_pwd) >= tbl_cliroles[DBS_SYS_TBL_CLIROLES_COL_ID_PWD].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowCliRole : len col_pwd !\n");
		return SQLITE_ERROR;
	}

	/* 判断该行是否存在*//* 兼容之前的数据表设计*/
	if( row->id > 3 )
	{
		if( __dbs_underlayer_get_count(DBS_SYS_TBL_ID_CLIROLE, &iCount) != SQLITE_OK )
		{
			fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowCliRole : get_count !\n");
			return SQLITE_ERROR;
		}
		
		if( iCount < row->id )
		{
			if( (iCount + 1) == row->id )
			{
				sprintf(sql, "INSERT INTO [tbl_cliroles] VALUES ('%d','%s','%s')", 
					row->id, row->col_user, row->col_pwd);
				return SQLITE_OK;
			}
			else
			{
				fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowCliRole : insert !\n");
				return SQLITE_ERROR;
			}
		}
		else
		{
			sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\", [%s]=\"%s\" WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_CLIROLE].tbl_name, 
			tbl_cliroles[DBS_SYS_TBL_CLIROLES_COL_ID_USER].col_name, 
			row->col_user, 
			tbl_cliroles[DBS_SYS_TBL_CLIROLES_COL_ID_PWD].col_name, 
			row->col_pwd, 
			row->id );
			return SQLITE_OK;
		}		
	}	
	/* 判断行数量是否超出定义*/
	else if( (row->id >= 1) && (row->id <= 3))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\", [%s]=\"%s\" WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_CLIROLE].tbl_name, 
			tbl_cliroles[DBS_SYS_TBL_CLIROLES_COL_ID_USER].col_name, 
			row->col_user, 
			tbl_cliroles[DBS_SYS_TBL_CLIROLES_COL_ID_PWD].col_name, 
			row->col_pwd, 
			row->id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowCliRole : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLUpdateRowClt(st_dbsClt *row, uint8_t *sql)
{
	assert( NULL != sql );
	assert( NULL != row );

	/* 判断列元素输入长度*/
	if( strlen(row->col_mac) >= tbl_clt[DBS_SYS_TBL_CLT_COL_ID_MAC].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowClt : len col_mac !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_swVersion) >= tbl_clt[DBS_SYS_TBL_CLT_COL_ID_SWVER].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowClt : len col_swVersion !\n");
		return SQLITE_ERROR;
	}
	
	/* 判断行数量是否超出定义*/
	if( (row->id >= 1) && (row->id <= MAX_CLT_AMOUNT_LIMIT))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=%d, [%s]=\"%s\", [%s]=%d, [%s]=%d, [%s]=%d, [%s]=\"%s\", [%s]=%d, [%s]=%d WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_CLT].tbl_name, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_MODEL].col_name, 
			row->col_model, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_MAC].col_name, 
			row->col_mac, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_STS].col_name, 
			row->col_sts, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_MAXSTATS].col_name, 
			row->col_maxStas, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_NUMSTATS].col_name, 
			row->col_numStas, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_SWVER].col_name, 
			row->col_swVersion, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_SYNCH].col_name, 
			row->col_synch, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_ROWSTS].col_name, 
			row->col_row_sts, 
			row->id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowClt : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLUpdateRowCltConf(st_dbsCltConf *row, uint8_t *sql)
{
	assert( NULL != sql );
	assert( NULL != row );

	/* 判断行数量是否超出定义*/
	if( (row->id >= 1) && (row->id <= MAX_CLT_AMOUNT_LIMIT))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_CLTPRO].tbl_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_ROWSTS].col_name, 
			row->col_row_sts, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_BASE].col_name, 
			row->col_base, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_MACLIMIT].col_name, 
			row->col_macLimit, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_CURATE].col_name, 
			row->col_curate, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_CDRATE].col_name, 
			row->col_cdrate, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_LOAGE].col_name, 
			row->col_loagTime, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_REAGE].col_name, 
			row->col_reagTime, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_IGMPPRI].col_name, 
			row->col_igmpPri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_UNIPRI].col_name, 
			row->col_unicastPri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_AVSPRI].col_name, 
			row->col_avsPri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_MCPRI].col_name, 
			row->col_mcastPri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TBAPRISTS].col_name, 
			row->col_tbaPriSts, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COSPRISTS].col_name, 
			row->col_cosPriSts, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS0PRI].col_name, 
			row->col_cos0pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS1PRI].col_name, 
			row->col_cos1pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS2PRI].col_name, 
			row->col_cos2pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS3PRI].col_name, 
			row->col_cos3pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS4PRI].col_name, 
			row->col_cos4pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS5PRI].col_name, 
			row->col_cos5pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS6PRI].col_name, 
			row->col_cos6pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS7PRI].col_name, 
			row->col_cos7pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOSPRISTS].col_name, 
			row->col_tosPriSts, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS0PRI].col_name, 
			row->col_tos0pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS1PRI].col_name, 
			row->col_tos1pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS2PRI].col_name, 
			row->col_tos2pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS3PRI].col_name, 
			row->col_tos3pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS4PRI].col_name, 
			row->col_tos4pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS5PRI].col_name, 
			row->col_tos5pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS6PRI].col_name, 
			row->col_tos6pri, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS7PRI].col_name, 
			row->col_tos7pri, 
			row->id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowCltConf : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLUpdateRowCnu(st_dbsCnu *row, uint8_t *sql)
{
	assert( NULL != sql );
	assert( NULL != row );

	/* 判断SQLITE3_TEXT  类型列元素输入长度*/
	if( strlen(row->col_mac) >= tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_MAC].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowCnu : len col_mac !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_ver) >= tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_VER].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowCnu : len col_ver !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_snr) >= tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_SNR].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowCnu : len col_snr !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_bpc) >= tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_BPC].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowCnu : len col_bpc !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_att) >= tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_ATT].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowCnu : len col_att !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_user_hfid) >= tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_USERHFID].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowCnu : len col_userhfid !\n");
		return SQLITE_ERROR;
	}
	/* 判断行数量是否超出定义*/
	if( (row->id >= 1) && (row->id <= MAX_CNU_AMOUNT_LIMIT))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=%d, [%s]=\"%s\", [%s]=%d, [%s]=%d, [%s]=\"%s\", [%s]=%d, [%s]=%d, [%s]=\"%s\", [%s]=\"%s\", [%s]=\"%s\", [%s]=%d, [%s]=%d, [%s]=%d, [%s]=\"%s\" WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_CNU].tbl_name, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_MODEL].col_name, 
			row->col_model, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_MAC].col_name, 
			row->col_mac, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_STS].col_name, 
			row->col_sts, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_AUTH].col_name, 
			row->col_auth, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_VER].col_name, 
			row->col_ver, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_RX].col_name, 
			row->col_rx, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_TX].col_name, 
			row->col_tx, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_SNR].col_name, 
			row->col_snr, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_BPC].col_name, 
			row->col_bpc, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_ATT].col_name, 
			row->col_att, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_SYNCH].col_name, 
			row->col_synch, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_ROWSTS].col_name, 
			row->col_row_sts, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_AUTOSTS].col_name,
			row->col_auto_sts,
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_USERHFID].col_name, 
			row->col_user_hfid, 
			row->id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowCnu : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLUpdateRowCnuDepro(st_dbsCnuDefaultProfile *row, uint8_t *sql)
{
	assert( NULL != sql );
	assert( NULL != row );

	/* 判断行数量是否超出定义*/
	if( (row->id >= 1) && (row->id <= 2))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_CNUDEPRO].tbl_name, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_BASE].col_name, 
			row->col_base, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_MACLIMIT].col_name, 
			row->col_macLimit, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_CURATE].col_name, 
			row->col_curate, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_CDRATE].col_name, 
			row->col_cdrate, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_LOAGE].col_name, 
			row->col_loagTime, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_REAGE].col_name, 
			row->col_reagTime, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_IGMPPRI].col_name, 
			row->col_igmpPri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_UNIPRI].col_name, 
			row->col_unicastPri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_AVSPRI].col_name, 
			row->col_avsPri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_MCPRI].col_name, 
			row->col_mcastPri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_TBAPRISTS].col_name, 
			row->col_tbaPriSts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_COSPRISTS].col_name, 
			row->col_cosPriSts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_COS0PRI].col_name, 
			row->col_cos0pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_COS1PRI].col_name, 
			row->col_cos1pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_COS2PRI].col_name, 
			row->col_cos2pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_COS3PRI].col_name, 
			row->col_cos3pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_COS4PRI].col_name, 
			row->col_cos4pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_COS5PRI].col_name, 
			row->col_cos5pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_COS6PRI].col_name, 
			row->col_cos6pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_COS7PRI].col_name, 
			row->col_cos7pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_TOSPRISTS].col_name, 
			row->col_tosPriSts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_TOS0PRI].col_name, 
			row->col_tos0pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_TOS1PRI].col_name, 
			row->col_tos1pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_TOS2PRI].col_name, 
			row->col_tos2pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_TOS3PRI].col_name, 
			row->col_tos3pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_TOS4PRI].col_name, 
			row->col_tos4pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_TOS5PRI].col_name, 
			row->col_tos5pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_TOS6PRI].col_name, 
			row->col_tos6pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_TOS7PRI].col_name, 
			row->col_tos7pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_SFBSTS].col_name, 
			row->col_sfbSts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_SFUSTS].col_name, 
			row->col_sfuSts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_SFMSTS].col_name, 
			row->col_sfmSts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_SFRATE].col_name, 
			row->col_sfRate, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_VLANSTS].col_name, 
			row->col_vlanSts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH1VID].col_name, 
			row->col_eth1vid, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH2VID].col_name, 
			row->col_eth2vid, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH3VID].col_name, 
			row->col_eth3vid, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH4VID].col_name, 
			row->col_eth4vid, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_PORTPRISTS].col_name, 
			row->col_portPriSts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH1PRI].col_name, 
			row->col_eth1pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH2PRI].col_name, 
			row->col_eth2pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH3PRI].col_name, 
			row->col_eth3pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH4PRI].col_name, 
			row->col_eth4pri, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_RXLIMITSTS].col_name, 
			row->col_rxLimitSts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_CPURX].col_name, 
			row->col_cpuPortRxRate, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH1RX].col_name, 
			row->col_eth1rx, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH2RX].col_name, 
			row->col_eth2rx, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH3RX].col_name, 
			row->col_eth3rx, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH4RX].col_name, 
			row->col_eth4rx, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_TXLIMITSTS].col_name, 
			row->col_txLimitSts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_CPUTX].col_name, 
			row->col_cpuPortTxRate, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH1TX].col_name, 
			row->col_eth1tx, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH2TX].col_name, 
			row->col_eth2tx, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH3TX].col_name, 
			row->col_eth3tx, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH4TX].col_name, 
			row->col_eth4tx, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_PSCTL].col_name, 
			row->col_psctlSts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_CPUPSTS].col_name, 
			row->col_cpuPortSts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH1STS].col_name, 
			row->col_eth1sts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH2STS].col_name, 
			row->col_eth2sts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH3STS].col_name, 
			row->col_eth3sts, 
			tbl_depro[DBS_SYS_TBL_DEPRO_COL_ID_ETH4STS].col_name, 
			row->col_eth4sts, 
			row->id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowCnuDepro : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLUpdateRowNetwork(st_dbsNetwork *row, uint8_t *sql)
{
	assert( NULL != sql );
	assert( NULL != row );

	/* 判断SQLITE3_TEXT  类型列元素输入长度*/
	if( strlen(row->col_ip) >= tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_IP].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowNetwork : len col_ip !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_netmask) >= tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_MASK].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowNetwork : len col_netmask !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_gw) >= tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_GW].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowNetwork : len col_gw !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_dns) >= tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_DNS].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowNetwork : len col_dns !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_mac) >= tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_MAC].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowNetwork : len col_mac !\n");
		return SQLITE_ERROR;
	}	
	
	/* 判断行数量是否超出定义*/
	if( (row->id >= 1) && (row->id <= 1))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\", [%s]=\"%s\", [%s]=\"%s\", [%s]=%d, [%s]=\"%s\", [%s]=\"%s\", [%s]=%d, [%s]=%d WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_NETWORK].tbl_name, 
			tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_IP].col_name, 
			row->col_ip, 
			tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_MASK].col_name, 
			row->col_netmask, 
			tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_GW].col_name, 
			row->col_gw, 
			tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_DHCP].col_name, 
			row->col_dhcp, 
			tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_DNS].col_name, 
			row->col_dns, 
			tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_MAC].col_name, 
			row->col_mac, 
			tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_MVSTS].col_name, 
			row->col_mvlan_sts, 
			tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_MVID].col_name, 
			row->col_mvlan_id, 
			row->id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowNetwork : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLUpdateRowProfile(st_dbsProfile *row, uint8_t *sql)
{
	assert( NULL != sql );
	assert( NULL != row );

	/* 判断行数量是否超出定义*/
	if( (row->id >= 1) && (row->id <= MAX_CNU_AMOUNT_LIMIT))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_CNUPRO].tbl_name, 			
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ROWSTS].col_name, 
			row->col_row_sts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_BASE].col_name, 
			row->col_base, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_MACLIMIT].col_name, 
			row->col_macLimit, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_CURATE].col_name, 
			row->col_curate, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_CDRATE].col_name, 
			row->col_cdrate, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_LOAGE].col_name, 
			row->col_loagTime, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_REAGE].col_name, 
			row->col_reagTime, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_IGMPPRI].col_name, 
			row->col_igmpPri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_UNIPRI].col_name, 
			row->col_unicastPri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_AVSPRI].col_name, 
			row->col_avsPri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_MCPRI].col_name, 
			row->col_mcastPri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TBAPRISTS].col_name, 
			row->col_tbaPriSts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COSPRISTS].col_name, 
			row->col_cosPriSts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS0PRI].col_name, 
			row->col_cos0pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS1PRI].col_name, 
			row->col_cos1pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS2PRI].col_name, 
			row->col_cos2pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS3PRI].col_name, 
			row->col_cos3pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS4PRI].col_name, 
			row->col_cos4pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS5PRI].col_name, 
			row->col_cos5pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS6PRI].col_name, 
			row->col_cos6pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS7PRI].col_name, 
			row->col_cos7pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOSPRISTS].col_name, 
			row->col_tosPriSts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS0PRI].col_name, 
			row->col_tos0pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS1PRI].col_name, 
			row->col_tos1pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS2PRI].col_name, 
			row->col_tos2pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS3PRI].col_name, 
			row->col_tos3pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS4PRI].col_name, 
			row->col_tos4pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS5PRI].col_name, 
			row->col_tos5pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS6PRI].col_name, 
			row->col_tos6pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS7PRI].col_name, 
			row->col_tos7pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_SFBSTS].col_name, 
			row->col_sfbSts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_SFUSTS].col_name, 
			row->col_sfuSts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_SFMSTS].col_name, 
			row->col_sfmSts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_SFRATE].col_name, 
			row->col_sfRate, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_VLANSTS].col_name, 
			row->col_vlanSts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH1VID].col_name, 
			row->col_eth1vid, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH2VID].col_name, 
			row->col_eth2vid, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH3VID].col_name, 
			row->col_eth3vid, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH4VID].col_name, 
			row->col_eth4vid, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_PORTPRISTS].col_name, 
			row->col_portPriSts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH1PRI].col_name, 
			row->col_eth1pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH2PRI].col_name, 
			row->col_eth2pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH3PRI].col_name, 
			row->col_eth3pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH4PRI].col_name, 
			row->col_eth4pri, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_RXLIMITSTS].col_name, 
			row->col_rxLimitSts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_CPURX].col_name, 
			row->col_cpuPortRxRate, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH1RX].col_name, 
			row->col_eth1rx, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH2RX].col_name, 
			row->col_eth2rx, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH3RX].col_name, 
			row->col_eth3rx, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH4RX].col_name, 
			row->col_eth4rx, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TXLIMITSTS].col_name, 
			row->col_txLimitSts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_CPUTX].col_name, 
			row->col_cpuPortTxRate, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH1TX].col_name, 
			row->col_eth1tx, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH2TX].col_name, 
			row->col_eth2tx, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH3TX].col_name, 
			row->col_eth3tx, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH4TX].col_name, 
			row->col_eth4tx, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_PSCTL].col_name, 
			row->col_psctlSts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_CPUPSTS].col_name, 
			row->col_cpuPortSts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH1STS].col_name, 
			row->col_eth1sts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH2STS].col_name, 
			row->col_eth2sts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH3STS].col_name, 
			row->col_eth3sts,
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH4STS].col_name, 
			row->col_eth4sts, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_UPLINKVID].col_name, 
			row->col_uplinkvid,
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_UPLINKVMODE].col_name, 
			row->col_uplinkVMode,
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH1VMODE].col_name, 
			row->col_eth1VMode,
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH2VMODE].col_name, 
			row->col_eth2VMode,
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH3VMODE].col_name, 
			row->col_eth3VMode,
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH4VMODE].col_name, 
			row->col_eth4VMode,
			row->id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowProfile : id !\n");
		return SQLITE_ERROR;
	}
}


int __dbs_underlayer_SQLUpdateRowSnmp(st_dbsSnmp *row, uint8_t *sql)
{
	assert( NULL != sql );
	assert( NULL != row );

	/* 判断SQLITE3_TEXT  类型列元素输入长度*/
	if( strlen(row->col_rdcom) >= tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_RC].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSnmp : len col_rdcom !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_wrcom) >= tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_WC].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSnmp : len col_wrcom !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_trapcom) >= tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_TC].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSnmp : len col_trapcom !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_sina) >= tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_SA].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSnmp : len col_sina !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_sinb) >= tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_SB].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSnmp : len col_sinb !\n");
		return SQLITE_ERROR;
	}
	
	/* 判断行数量是否超出定义*/
	if( (row->id >= 1) && (row->id <= 1))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\", [%s]=\"%s\", [%s]=\"%s\", [%s]=\"%s\", [%s]=\"%s\", [%s]=%d, [%s]=%d WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_SNMPINFO].tbl_name, 			
			tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_RC].col_name, 
			row->col_rdcom, 
			tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_WC].col_name, 
			row->col_wrcom, 
			tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_TC].col_name, 
			row->col_trapcom, 
			tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_SA].col_name, 
			row->col_sina, 
			tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_SB].col_name, 
			row->col_sinb, 
			tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_PA].col_name, 
			row->col_tpa, 
			tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_PB].col_name, 
			row->col_tpb, 
			row->id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSnmp : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLUpdateRowSwmgmt(st_dbsSwmgmt *row, uint8_t *sql)
{
	assert( NULL != sql );
	assert( NULL != row );

	/* 判断SQLITE3_TEXT  类型列元素输入长度*/
	if( strlen(row->col_ip) >= tbl_swmgmt[DBS_SYS_TBL_SWMGMT_COL_ID_IP].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSwmgmt : len col_ip !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_user) >= tbl_swmgmt[DBS_SYS_TBL_SWMGMT_COL_ID_USR].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSwmgmt : len col_user !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_pwd) >= tbl_swmgmt[DBS_SYS_TBL_SWMGMT_COL_ID_PWD].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSwmgmt : len col_pwd !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_path) >= tbl_swmgmt[DBS_SYS_TBL_SWMGMT_COL_ID_PATH].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSwmgmt : len col_path !\n");
		return SQLITE_ERROR;
	}
	
	/* 判断行数量是否超出定义*/
	if( (row->id >= 1) && (row->id <= 1))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=\"%s\", [%s]=%d, [%s]=\"%s\", [%s]=\"%s\", [%s]=\"%s\" WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_SWMGMT].tbl_name, 			
			tbl_swmgmt[DBS_SYS_TBL_SWMGMT_COL_ID_IP].col_name, 
			row->col_ip, 
			tbl_swmgmt[DBS_SYS_TBL_SWMGMT_COL_ID_PORT].col_name, 
			row->col_port, 
			tbl_swmgmt[DBS_SYS_TBL_SWMGMT_COL_ID_USR].col_name, 
			row->col_user, 
			tbl_swmgmt[DBS_SYS_TBL_SWMGMT_COL_ID_PWD].col_name, 
			row->col_pwd, 
			tbl_swmgmt[DBS_SYS_TBL_SWMGMT_COL_ID_PATH].col_name, 
			row->col_path, 			
			row->id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSwmgmt : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLUpdateRowSysinfo(st_dbsSysinfo *row, uint8_t *sql)
{
	assert( NULL != sql );
	assert( NULL != row );

	/* 判断SQLITE3_TEXT  类型列元素输入长度*/
	if( strlen(row->col_hwver) >= tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_HWVER].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSysinfo : len col_hwver !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_bver) >= tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_BVER].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSysinfo : len col_bver !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_kver) >= tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_KVER].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSysinfo : len col_kver !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_appver) >= tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_APPVER].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSysinfo : len col_appver !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_mfinfo) >= tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_MF].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSysinfo : len col_mfinfo !\n");
		return SQLITE_ERROR;
	}
	if( strlen(row->col_apphash) >= tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_APPHASH].col_len )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSysinfo : len col_apphash !\n");
		return SQLITE_ERROR;
	}
	
	/* 判断行数量是否超出定义*/
	if( (row->id >= 1) && (row->id <= 1))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=%d, [%s]=%d, [%s]=\"%s\", [%s]=\"%s\", [%s]=\"%s\", [%s]=\"%s\", [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=\"%s\", [%s]=\"%s\" WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_SYSINFO].tbl_name, 			
			tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_MODEL].col_name, 
			row->col_model, 
			tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_MAXCLT].col_name, 
			row->col_maxclt, 
			tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_HWVER].col_name, 
			row->col_hwver, 
			tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_BVER].col_name, 
			row->col_bver, 
			tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_KVER].col_name, 
			row->col_kver, 
			tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_APPVER].col_name, 
			row->col_appver, 
			tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_FS].col_name, 
			row->col_flashsize, 
			tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_RS].col_name, 
			row->col_ramsize, 
			tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_WLCTL].col_name, 
			row->col_wlctl, 
			tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_AU].col_name, 
			row->col_autoud, 
			tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_AC].col_name, 
			row->col_autoconf, 
			tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_WDT].col_name, 
			row->col_wdt, 
			tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_MF].col_name, 
			row->col_mfinfo, 			
			tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_APPHASH].col_name, 
			row->col_apphash, 
			row->id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowSysinfo : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLUpdateRowTemplate(st_dbsTemplate *row, uint8_t *sql)
{
	assert( NULL != sql );
	assert( NULL != row );

	/* 判断行数量是否超出定义*/
	if( (row->id >= 1) && (row->id <= MAX_CNU_AMOUNT_LIMIT))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d, [%s]=%d  WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_TEMPLATE].tbl_name, 			
			tbl_template[DBS_SYS_TBL_TEMPLATE_COL_ID_TEMPAUTOSTS].col_name, 
			row->col_tempAutoSts, 
			tbl_template[DBS_SYS_TBL_TEMPLATE_COL_ID_CURTEMP].col_name, 
			row->col_curTemp, 
			tbl_template[DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANADDSTS].col_name, 
			row->col_eth1VlanAddSts, 
			tbl_template[DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANSTART].col_name, 
			row->col_eth1VlanStart, 
			tbl_template[DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANSTOP].col_name, 
			row->col_eth1VlanStop, 
			tbl_template[DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANADDSTS].col_name, 
			row->col_eth2VlanAddSts, 
			tbl_template[DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANSTART].col_name, 
			row->col_eth2VlanStart, 
			tbl_template[DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANSTOP].col_name, 
			row->col_eth2VlanStop, 
			tbl_template[DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANADDSTS].col_name, 
			row->col_eth3VlanAddSts, 
			tbl_template[DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANSTART].col_name, 
			row->col_eth3VlanStart, 
			tbl_template[DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANSTOP].col_name, 
			row->col_eth3VlanStop, 
			tbl_template[DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANADDSTS].col_name, 
			row->col_eth4VlanAddSts, 
			tbl_template[DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANSTART].col_name, 
			row->col_eth4VlanStart, 
			tbl_template[DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANSTOP].col_name, 
			row->col_eth4VlanStop, 
			row->id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLUpdateRowTemplate : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLDestroyRowClt(uint16_t id, uint8_t *sql)
{
	assert( NULL != sql );
	
	/* 判断行数量是否超出定义*/
	if( (id >= 1) && (id <= MAX_CLT_AMOUNT_LIMIT))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=0 WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_CLT].tbl_name, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_MODEL].col_name, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_MAC].col_name, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_STS].col_name, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_MAXSTATS].col_name, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_NUMSTATS].col_name, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_SWVER].col_name, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_SYNCH].col_name, 
			tbl_clt[DBS_SYS_TBL_CLT_COL_ID_ROWSTS].col_name, 
			id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLDestroyRowClt : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLDestroyRowCltconf(uint16_t id, uint8_t *sql)
{
	assert( NULL != sql );

	/* 判断行数量是否超出定义*/
	if( (id >= 1) && (id <= MAX_CLT_AMOUNT_LIMIT))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=0, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_CLTPRO].tbl_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_ROWSTS].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_BASE].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_MACLIMIT].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_CURATE].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_CDRATE].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_LOAGE].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_REAGE].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_IGMPPRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_UNIPRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_AVSPRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_MCPRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TBAPRISTS].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COSPRISTS].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS0PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS1PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS2PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS3PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS4PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS5PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS6PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_COS7PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOSPRISTS].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS0PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS1PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS2PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS3PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS4PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS5PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS6PRI].col_name, 
			tbl_cltconf[DBS_SYS_TBL_CLTPRO_COL_ID_TOS7PRI].col_name, 
			id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLDestroyRowCltconf : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLDestroyRowCnu(uint16_t id, uint8_t *sql)
{
	assert( NULL != sql );
	
	/* 判断行数量是否超出定义*/
	if( (id >= 1) && (id <= MAX_CNU_AMOUNT_LIMIT))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=0, [%s]=0, [%s]=null WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_CNU].tbl_name, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_MODEL].col_name, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_MAC].col_name, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_STS].col_name, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_AUTH].col_name, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_VER].col_name, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_RX].col_name, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_TX].col_name, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_SNR].col_name, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_BPC].col_name, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_ATT].col_name, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_SYNCH].col_name, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_ROWSTS].col_name,
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_AUTOSTS].col_name, 
			tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_USERHFID].col_name,
			id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLDestroyRowCnu : id !\n");
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_SQLDestroyRowProfile(uint16_t id, uint8_t *sql)
{
	assert( NULL != sql );

	/* 判断行数量是否超出定义*/
	if( (id >= 1) && (id <= MAX_CNU_AMOUNT_LIMIT))
	{
		sprintf(sql, "UPDATE [%s] SET [%s]=0, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null, [%s]=null WHERE [id]=%d", 
			db_system[DBS_SYS_TBL_ID_CNUPRO].tbl_name, 			
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ROWSTS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_BASE].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_MACLIMIT].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_CURATE].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_CDRATE].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_LOAGE].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_REAGE].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_IGMPPRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_UNIPRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_AVSPRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_MCPRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TBAPRISTS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COSPRISTS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS0PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS1PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS2PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS3PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS4PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS5PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS6PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_COS7PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOSPRISTS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS0PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS1PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS2PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS3PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS4PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS5PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS6PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TOS7PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_SFBSTS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_SFUSTS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_SFMSTS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_SFRATE].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_VLANSTS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH1VID].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH2VID].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH3VID].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH4VID].col_name,
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_PORTPRISTS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH1PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH2PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH3PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH4PRI].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_RXLIMITSTS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_CPURX].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH1RX].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH2RX].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH3RX].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH4RX].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_TXLIMITSTS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_CPUTX].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH1TX].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH2TX].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH3TX].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH4TX].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_PSCTL].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_CPUPSTS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH1STS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH2STS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH3STS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH4STS].col_name, 
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_UPLINKVID].col_name,	
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_UPLINKVMODE].col_name,
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH1VMODE].col_name,
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH2VMODE].col_name,
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH3VMODE].col_name,
			tbl_profile[DBS_SYS_TBL_PROFILE_COL_ID_ETH4VMODE].col_name,
			id );
		return SQLITE_OK;
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_SQLDestroyRowProfile : id !\n");
		return SQLITE_ERROR;
	}
}

/********************************************************************************************
*	函数名称:__dbs_underlayer_get_integer
*	函数功能:从数据库获取一个整型列元素
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
int __dbs_underlayer_get_integer(DB_COL_VAR *v)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != v );

	/* 填充默认值(NULL) */
	v->ci.colType = SQLITE_NULL;
	v->len = 0;	

	/* 组装SQL语句*/
	if( SQLITE_OK != __dbs_underlayer_SQLGetInteger(&(v->ci), sql) )
	{
		return SQLITE_ERROR;
	}
	
	__dbsUnderlayerSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_integer->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* 结果集中列的数量*/
		if( 1 != sqlite3_column_count(stmt) )
		{
			fprintf(stderr, "ERROR: __dbs_underlayer_get_integer->sqlite3_column_count !\n");
			ret = SQLITE_ERROR;
		}
		/* 判断该列是否为NULL */
		else if( SQLITE_NULL == sqlite3_column_type(stmt, 0) )
		{
			if( BOOL_FALSE == __is_col_allow_null(v->ci.tbl, v->ci.col) )
			{
				fprintf(stderr, "WARNNING: __dbs_underlayer_get_integer : COL IS NULL !\n");
			}
			ret = SQLITE_OK;
		}
		/* 获取列数据类型，列编号从0开始的*/
		else if( SQLITE_INTEGER == sqlite3_column_type(stmt, 0) )
		{
			v->ci.colType = SQLITE_INTEGER;
			v->len = sizeof(uint32_t);
			*(uint32_t *)(v->data) = sqlite3_column_int(stmt, 0);
			ret = SQLITE_OK;
		}
		else
		{
			fprintf(stderr, "ERROR: __dbs_underlayer_get_integer->sqlite3_column_type : NOT INTEGER !\n");
			ret = SQLITE_ERROR;
		}
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_integer->sqlite3_step\n");
		ret = SQLITE_ERROR;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

/********************************************************************************************
*	函数名称:__dbs_underlayer_get_text
*	函数功能:从数据库获取一个TEXT型列元素
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
int __dbs_underlayer_get_text(DB_COL_VAR *v)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	const unsigned char *pStr = NULL;
	
	assert( NULL != v );

	/* 填充默认值(NULL) */
	v->ci.colType = SQLITE_NULL;
	v->len = 0;				

	if( SQLITE_OK != __dbs_underlayer_SQLGetText(&(v->ci), sql) )
	{
		return SQLITE_ERROR;
	}
	
	__dbsUnderlayerSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_text->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* 结果集中列的数量*/
		if( 1 != sqlite3_column_count(stmt) )
		{
			fprintf(stderr, "ERROR: __dbs_underlayer_get_text->sqlite3_column_count !\n");
			ret = SQLITE_ERROR;
		}
		/* 判断该列是否为NULL */
		else if( SQLITE_NULL == sqlite3_column_type(stmt, 0) )
		{
			if( BOOL_FALSE == __is_col_allow_null(v->ci.tbl, v->ci.col) )
			{
				fprintf(stderr, "WARNNING: __dbs_underlayer_get_text : COL IS NULL !\n");
			}
			ret = SQLITE_OK;
		}
		/* 获取列数据类型，列编号从0开始的*/
		else if( SQLITE3_TEXT == sqlite3_column_type(stmt, 0) )
		{
			pStr = sqlite3_column_text(stmt, 0);
			if( strlen(pStr) >= DBS_COL_MAX_LEN )
			{
				fprintf(stderr, "ERROR: __dbs_underlayer_get_text : TEXT TOO LONG !\n");
				ret = SQLITE_ERROR;
			}
			else
			{
				v->ci.colType = SQLITE3_TEXT;
				v->len = strlen(pStr);
				strncpy(v->data, pStr, v->len);
				ret = SQLITE_OK;
			}					
		}
		else
		{
			fprintf(stderr, "ERROR: __dbs_underlayer_get_text : NOT SQLITE3_TEXT !\n");
			ret = SQLITE_ERROR;
		}
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_text->sqlite3_step !\n");
		ret = SQLITE_ERROR;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

int __dbs_underlayer_clear_column(DB_COL_INFO *ci)
{
	//char sql[DBS_SQL_MEDIUM_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	assert( NULL != ci );

	if( SQLITE_OK == __dbs_underlayer_SQLClearColumn(ci, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_update_integer(DB_COL_VAR *v)
{
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	assert( NULL != v );

	if( SQLITE_OK == __dbs_underlayer_SQLUpdateInteger(v, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_update_text(DB_COL_VAR *v)
{
	//char sql[DBS_SQL_MEDIUM_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	assert( NULL != v );

	if( SQLITE_OK == __dbs_underlayer_SQLUpdateText(v, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_get_row_clirole(st_dbsCliRole *row)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != row );

	/* 组装SQL语句*/
	if( SQLITE_OK != __dbs_underlayer_SQLGetRowCliRole(row->id, sql) )
	{
		return SQLITE_ERROR;
	}
	
	__dbsUnderlayerSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_clirole->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* SQLITE3_TEXT | col_user */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLIROLES_COL_ID_USER) )
		{
			row->col_user[0] = '\0';
		}
		else
		{
			strncpy(row->col_user, sqlite3_column_text(stmt, DBS_SYS_TBL_CLIROLES_COL_ID_USER), 
				tbl_cliroles[DBS_SYS_TBL_CLIROLES_COL_ID_USER].col_len);
		}
		
		/* SQLITE3_TEXT | col_pwd */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLIROLES_COL_ID_PWD) )
		{
			row->col_pwd[0] = '\0';
		}
		else
		{
			strncpy(row->col_pwd, sqlite3_column_text(stmt, DBS_SYS_TBL_CLIROLES_COL_ID_PWD), 
				tbl_cliroles[DBS_SYS_TBL_CLIROLES_COL_ID_PWD].col_len);
		}

		ret = SQLITE_OK;		
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_clirole->sqlite3_step\n");
		ret = SQLITE_ERROR;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

int __dbs_underlayer_get_row_clt(st_dbsClt *row)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != row );

	/* 组装SQL语句*/
	if( SQLITE_OK != __dbs_underlayer_SQLGetRowClt(row->id, sql) )
	{
		return SQLITE_ERROR;
	}
	
	__dbsUnderlayerSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_clt->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* SQLITE_INTEGER	| col_model */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLT_COL_ID_MODEL) )
		{
			row->col_model = 0;
		}
		else
		{
			row->col_model = sqlite3_column_int(stmt, DBS_SYS_TBL_CLT_COL_ID_MODEL);
		}		
		/* SQLITE3_TEXT		| col_mac */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLT_COL_ID_MAC) )
		{
			row->col_mac[0] = '\0';
		}
		else
		{
			strncpy(row->col_mac, sqlite3_column_text(stmt, DBS_SYS_TBL_CLT_COL_ID_MAC), 
				tbl_clt[DBS_SYS_TBL_CLT_COL_ID_MAC].col_len);
		}
		/* SQLITE_INTEGER	| col_sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLT_COL_ID_STS) )
		{
			row->col_sts = 0;
		}
		else
		{
			row->col_sts = sqlite3_column_int(stmt, DBS_SYS_TBL_CLT_COL_ID_STS);
		}	
		/* SQLITE_INTEGER	| col_maxStas */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLT_COL_ID_MAXSTATS) )
		{
			row->col_maxStas = 0;
		}
		else
		{
			row->col_maxStas = sqlite3_column_int(stmt, DBS_SYS_TBL_CLT_COL_ID_MAXSTATS);
		}	
		/* SQLITE_INTEGER	| col_numStas */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLT_COL_ID_NUMSTATS) )
		{
			row->col_numStas = 0;
		}
		else
		{
			row->col_numStas = sqlite3_column_int(stmt, DBS_SYS_TBL_CLT_COL_ID_NUMSTATS);
		}	
		/* SQLITE3_TEXT		| col_swVersion */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLT_COL_ID_SWVER) )
		{
			row->col_swVersion[0] = '\0';
		}
		else
		{
			strncpy(row->col_swVersion, sqlite3_column_text(stmt, DBS_SYS_TBL_CLT_COL_ID_SWVER), 
				tbl_clt[DBS_SYS_TBL_CLT_COL_ID_SWVER].col_len);
		}
		/* SQLITE_INTEGER	| col_synch */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLT_COL_ID_SYNCH) )
		{
			row->col_synch = 0;
		}
		else
		{
			row->col_synch = sqlite3_column_int(stmt, DBS_SYS_TBL_CLT_COL_ID_SYNCH);
		}	
		/* SQLITE_INTEGER	| col_row_sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLT_COL_ID_ROWSTS) )
		{
			row->col_row_sts = 0;
		}
		else
		{
			row->col_row_sts = sqlite3_column_int(stmt, DBS_SYS_TBL_CLT_COL_ID_ROWSTS);
		}		
		ret = SQLITE_OK;		
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_clt->sqlite3_step\n");
		ret = SQLITE_ERROR;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

int __dbs_underlayer_get_row_cltconfig(st_dbsCltConf *row)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != row );

	/* 组装SQL语句*/
	if( SQLITE_OK != __dbs_underlayer_SQLGetRowCltConf(row->id, sql) )
	{
		return SQLITE_ERROR;
	}
	
	__dbsUnderlayerSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_cltconfig->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* SQLITE_INTEGER	| col_row_sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_ROWSTS) )
		{
			row->col_row_sts = 0;
		}
		else
		{
			row->col_row_sts = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_ROWSTS);
		}
		/* SQLITE_INTEGER	| col_base */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_BASE) )
		{
			row->col_base = 0;
		}
		else
		{
			row->col_base = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_BASE);
		}
		/* SQLITE_INTEGER	| col_macLimit */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_MACLIMIT) )
		{
			row->col_macLimit = 0;
		}
		else
		{
			row->col_macLimit = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_MACLIMIT);
		}
		/* SQLITE_INTEGER	| col_curate */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_CURATE) )
		{
			row->col_curate = 0;
		}
		else
		{
			row->col_curate = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_CURATE);
		}
		/* SQLITE_INTEGER	| col_cdrate */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_CDRATE) )
		{
			row->col_cdrate = 0;
		}
		else
		{
			row->col_cdrate = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_CDRATE);
		}
		/* SQLITE_INTEGER	| col_loagTime */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_LOAGE) )
		{
			row->col_loagTime = 0;
		}
		else
		{
			row->col_loagTime = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_LOAGE);
		}
		/* SQLITE_INTEGER	| col_reagTime */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_REAGE) )
		{
			row->col_reagTime = 0;
		}
		else
		{
			row->col_reagTime = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_REAGE);
		}
		/* SQLITE_INTEGER	| col_igmpPri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_IGMPPRI) )
		{
			row->col_igmpPri = 0;
		}
		else
		{
			row->col_igmpPri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_IGMPPRI);
		}
		/* SQLITE_INTEGER	| col_unicastPri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_UNIPRI) )
		{
			row->col_unicastPri = 0;
		}
		else
		{
			row->col_unicastPri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_UNIPRI);
		}
		/* SQLITE_INTEGER	| col_avsPri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_AVSPRI) )
		{
			row->col_avsPri = 0;
		}
		else
		{
			row->col_avsPri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_AVSPRI);
		}
		/* SQLITE_INTEGER	| col_mcastPri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_MCPRI) )
		{
			row->col_mcastPri = 0;
		}
		else
		{
			row->col_mcastPri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_MCPRI);
		}
		/* SQLITE_INTEGER	| col_tbaPriSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TBAPRISTS) )
		{
			row->col_tbaPriSts = 0;
		}
		else
		{
			row->col_tbaPriSts = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TBAPRISTS);
		}
		/* SQLITE_INTEGER	| col_cosPriSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COSPRISTS) )
		{
			row->col_cosPriSts = 0;
		}
		else
		{
			row->col_cosPriSts = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COSPRISTS);
		}
		/* SQLITE_INTEGER	| col_cos0pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS0PRI) )
		{
			row->col_cos0pri = 0;
		}
		else
		{
			row->col_cos0pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS0PRI);
		}
		/* SQLITE_INTEGER	| col_cos1pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS1PRI) )
		{
			row->col_cos1pri = 0;
		}
		else
		{
			row->col_cos1pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS1PRI);
		}
		/* SQLITE_INTEGER	| col_cos2pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS2PRI) )
		{
			row->col_cos2pri = 0;
		}
		else
		{
			row->col_cos2pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS2PRI);
		}
		/* SQLITE_INTEGER	| col_cos3pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS3PRI) )
		{
			row->col_cos3pri = 0;
		}
		else
		{
			row->col_cos3pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS3PRI);
		}
		/* SQLITE_INTEGER	| col_cos4pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS4PRI) )
		{
			row->col_cos4pri = 0;
		}
		else
		{
			row->col_cos4pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS4PRI);
		}
		/* SQLITE_INTEGER	| col_cos5pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS5PRI) )
		{
			row->col_cos5pri = 0;
		}
		else
		{
			row->col_cos5pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS5PRI);
		}
		/* SQLITE_INTEGER	| col_cos6pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS6PRI) )
		{
			row->col_cos6pri = 0;
		}
		else
		{
			row->col_cos6pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS6PRI);
		}
		/* SQLITE_INTEGER	| col_cos7pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS7PRI) )
		{
			row->col_cos7pri = 0;
		}
		else
		{
			row->col_cos7pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_COS7PRI);
		}
		/* SQLITE_INTEGER	| col_tosPriSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOSPRISTS) )
		{
			row->col_tosPriSts = 0;
		}
		else
		{
			row->col_tosPriSts = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOSPRISTS);
		}
		/* SQLITE_INTEGER	| col_tos0pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS0PRI) )
		{
			row->col_tos0pri = 0;
		}
		else
		{
			row->col_tos0pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS0PRI);
		}
		/* SQLITE_INTEGER	| col_tos1pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS1PRI) )
		{
			row->col_tos1pri = 0;
		}
		else
		{
			row->col_tos1pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS1PRI);
		}
		/* SQLITE_INTEGER	| col_tos2pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS2PRI) )
		{
			row->col_tos2pri = 0;
		}
		else
		{
			row->col_tos2pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS2PRI);
		}
		/* SQLITE_INTEGER	| col_tos3pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS3PRI) )
		{
			row->col_tos3pri = 0;
		}
		else
		{
			row->col_tos3pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS3PRI);
		}
		/* SQLITE_INTEGER	| col_tos4pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS4PRI) )
		{
			row->col_tos4pri = 0;
		}
		else
		{
			row->col_tos4pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS4PRI);
		}
		/* SQLITE_INTEGER	| col_tos5pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS5PRI) )
		{
			row->col_tos5pri = 0;
		}
		else
		{
			row->col_tos5pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS5PRI);
		}
		/* SQLITE_INTEGER	| col_tos6pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS6PRI) )
		{
			row->col_tos6pri = 0;
		}
		else
		{
			row->col_tos6pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS6PRI);
		}
		/* SQLITE_INTEGER	| col_tos7pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS7PRI) )
		{
			row->col_tos7pri = 0;
		}
		else
		{
			row->col_tos7pri = sqlite3_column_int(stmt, DBS_SYS_TBL_CLTPRO_COL_ID_TOS7PRI);
		}
		
		ret = SQLITE_OK;		
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_cltconfig->sqlite3_step\n");
		ret = SQLITE_ERROR;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

int __dbs_underlayer_get_row_cnu(st_dbsCnu *row)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != row );

	/* 组装SQL语句*/
	if( SQLITE_OK != __dbs_underlayer_SQLGetRowCnu(row->id, sql) )
	{
		return SQLITE_ERROR;
	}
	
	__dbsUnderlayerSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_cnu->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* SQLITE_INTEGER	| col_model */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_MODEL) )
		{
			row->col_model = 0;
		}
		else
		{
			row->col_model = sqlite3_column_int(stmt, DBS_SYS_TBL_CNU_COL_ID_MODEL);
		}
		/* SQLITE3_TEXT		| col_mac */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_MAC) )
		{
			row->col_mac[0] = '\0';
		}
		else
		{
			strncpy(row->col_mac, sqlite3_column_text(stmt, DBS_SYS_TBL_CNU_COL_ID_MAC), 
				tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_MAC].col_len);
		}
		/* SQLITE_INTEGER	| col_sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_STS) )
		{
			row->col_sts = 0;
		}
		else
		{
			row->col_sts = sqlite3_column_int(stmt, DBS_SYS_TBL_CNU_COL_ID_STS);
		}
		/* SQLITE_INTEGER	| col_auth */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_AUTH) )
		{
			row->col_auth = 0;
		}
		else
		{
			row->col_auth = sqlite3_column_int(stmt, DBS_SYS_TBL_CNU_COL_ID_AUTH);
		}
		/* SQLITE3_TEXT		| col_ver */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_VER) )
		{
			row->col_ver[0] = '\0';
		}
		else
		{
			strncpy(row->col_ver, sqlite3_column_text(stmt, DBS_SYS_TBL_CNU_COL_ID_VER), 
				tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_VER].col_len);
		}
		/* SQLITE_INTEGER	| col_rx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_RX) )
		{
			row->col_rx = 0;
		}
		else
		{
			row->col_rx = sqlite3_column_int(stmt, DBS_SYS_TBL_CNU_COL_ID_RX);
		}
		/* SQLITE_INTEGER	| col_tx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_TX) )
		{
			row->col_tx = 0;
		}
		else
		{
			row->col_tx = sqlite3_column_int(stmt, DBS_SYS_TBL_CNU_COL_ID_TX);
		}
		/* SQLITE3_TEXT		| col_snr */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_SNR) )
		{
			row->col_snr[0] = '\0';
		}
		else
		{
			strncpy(row->col_snr, sqlite3_column_text(stmt, DBS_SYS_TBL_CNU_COL_ID_SNR), 
				tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_SNR].col_len);
		}
		/* SQLITE3_TEXT		| col_bpc */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_BPC) )
		{
			row->col_bpc[0] = '\0';
		}
		else
		{
			strncpy(row->col_bpc, sqlite3_column_text(stmt, DBS_SYS_TBL_CNU_COL_ID_BPC), 
				tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_BPC].col_len);
		}
		/* SQLITE3_TEXT		| col_att */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_ATT) )
		{
			row->col_att[0] = '\0';
		}
		else
		{
			strncpy(row->col_att, sqlite3_column_text(stmt, DBS_SYS_TBL_CNU_COL_ID_ATT), 
				tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_ATT].col_len);
		}
		/* SQLITE_INTEGER	| col_synch */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_SYNCH) )
		{
			row->col_synch = 0;
		}
		else
		{
			row->col_synch = sqlite3_column_int(stmt, DBS_SYS_TBL_CNU_COL_ID_SYNCH);
		}
		/* SQLITE_INTEGER	| col_row_sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_ROWSTS) )
		{
			row->col_row_sts = 0;
		}
		else
		{
			row->col_row_sts = sqlite3_column_int(stmt, DBS_SYS_TBL_CNU_COL_ID_ROWSTS);
		}
		/* SQLITE_INTEGER	| col_auto_sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_AUTOSTS) )
		{
			row->col_auto_sts = 0;
		}
		else
		{
			row->col_auto_sts = sqlite3_column_int(stmt, DBS_SYS_TBL_CNU_COL_ID_AUTOSTS);
		}
		/* SQLITE3_TEXT		| col_user_hfid */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_USERHFID) )
		{
			row->col_user_hfid[0] = '\0';
		}
		else
		{
			strncpy(row->col_user_hfid, sqlite3_column_text(stmt, DBS_SYS_TBL_CNU_COL_ID_USERHFID), 
				tbl_cnu[DBS_SYS_TBL_CNU_COL_ID_USERHFID].col_len);
		}
		
		ret = SQLITE_OK;		
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_cnu->sqlite3_step\n");
		ret = SQLITE_ERROR;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

int __dbs_underlayer_get_row_depro(st_dbsCnuDefaultProfile *row)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != row );

	/* 组装SQL语句*/
	if( SQLITE_OK != __dbs_underlayer_SQLGetRowCnuDepro(row->id, sql) )
	{
		return SQLITE_ERROR;
	}
	
	__dbsUnderlayerSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_depro->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* SQLITE_INTEGER	| col_base */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_BASE) )
		{
			row->col_base = 0;
		}
		else
		{
			row->col_base = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_BASE);
		}
		/* SQLITE_INTEGER	| col_macLimit */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_MACLIMIT) )
		{
			row->col_macLimit = 0;
		}
		else
		{
			row->col_macLimit = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_MACLIMIT);
		}
		/* SQLITE_INTEGER	| col_curate */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_CURATE) )
		{
			row->col_curate = 0;
		}
		else
		{
			row->col_curate = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_CURATE);
		}
		/* SQLITE_INTEGER	| col_cdrate */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_CDRATE) )
		{
			row->col_cdrate = 0;
		}
		else
		{
			row->col_cdrate = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_CDRATE);
		}
		/* SQLITE_INTEGER	| col_loagTime */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_LOAGE) )
		{
			row->col_loagTime = 0;
		}
		else
		{
			row->col_loagTime = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_LOAGE);
		}
		/* SQLITE_INTEGER	| col_reagTime */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_REAGE) )
		{
			row->col_reagTime = 0;
		}
		else
		{
			row->col_reagTime = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_REAGE);
		}
		/* SQLITE_INTEGER	| col_igmpPri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_IGMPPRI) )
		{
			row->col_igmpPri = 0;
		}
		else
		{
			row->col_igmpPri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_IGMPPRI);
		}
		/* SQLITE_INTEGER	| col_unicastPri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_UNIPRI) )
		{
			row->col_unicastPri = 0;
		}
		else
		{
			row->col_unicastPri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_UNIPRI);
		}
		/* SQLITE_INTEGER	| col_avsPri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_AVSPRI) )
		{
			row->col_avsPri = 0;
		}
		else
		{
			row->col_avsPri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_AVSPRI);
		}
		/* SQLITE_INTEGER	| col_mcastPri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_MCPRI) )
		{
			row->col_mcastPri = 0;
		}
		else
		{
			row->col_mcastPri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_MCPRI);
		}
		/* SQLITE_INTEGER	| col_tbaPriSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TBAPRISTS) )
		{
			row->col_tbaPriSts = 0;
		}
		else
		{
			row->col_tbaPriSts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TBAPRISTS);
		}
		/* SQLITE_INTEGER	| col_cosPriSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COSPRISTS) )
		{
			row->col_cosPriSts = 0;
		}
		else
		{
			row->col_cosPriSts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COSPRISTS);
		}
		/* SQLITE_INTEGER	| col_cos0pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS0PRI) )
		{
			row->col_cos0pri = 0;
		}
		else
		{
			row->col_cos0pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS0PRI);
		}
		/* SQLITE_INTEGER	| col_cos1pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS1PRI) )
		{
			row->col_cos1pri = 0;
		}
		else
		{
			row->col_cos1pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS1PRI);
		}
		/* SQLITE_INTEGER	| col_cos2pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS2PRI) )
		{
			row->col_cos2pri = 0;
		}
		else
		{
			row->col_cos2pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS2PRI);
		}
		/* SQLITE_INTEGER	| col_cos3pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS3PRI) )
		{
			row->col_cos3pri = 0;
		}
		else
		{
			row->col_cos3pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS3PRI);
		}
		/* SQLITE_INTEGER	| col_cos4pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS4PRI) )
		{
			row->col_cos4pri = 0;
		}
		else
		{
			row->col_cos4pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS4PRI);
		}
		/* SQLITE_INTEGER	| col_cos5pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS5PRI) )
		{
			row->col_cos5pri = 0;
		}
		else
		{
			row->col_cos5pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS5PRI);
		}
		/* SQLITE_INTEGER	| col_cos6pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS6PRI) )
		{
			row->col_cos6pri = 0;
		}
		else
		{
			row->col_cos6pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS6PRI);
		}
		/* SQLITE_INTEGER	| col_cos7pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS7PRI) )
		{
			row->col_cos7pri = 0;
		}
		else
		{
			row->col_cos7pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_COS7PRI);
		}
		/* SQLITE_INTEGER	| col_tosPriSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOSPRISTS) )
		{
			row->col_tosPriSts = 0;
		}
		else
		{
			row->col_tosPriSts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOSPRISTS);
		}
		/* SQLITE_INTEGER	| col_tos0pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS0PRI) )
		{
			row->col_tos0pri = 0;
		}
		else
		{
			row->col_tos0pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS0PRI);
		}
		/* SQLITE_INTEGER	| col_tos1pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS1PRI) )
		{
			row->col_tos1pri = 0;
		}
		else
		{
			row->col_tos1pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS1PRI);
		}
		/* SQLITE_INTEGER	| col_tos2pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS2PRI) )
		{
			row->col_tos2pri = 0;
		}
		else
		{
			row->col_tos2pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS2PRI);
		}
		/* SQLITE_INTEGER	| col_tos3pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS3PRI) )
		{
			row->col_tos3pri = 0;
		}
		else
		{
			row->col_tos3pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS3PRI);
		}
		/* SQLITE_INTEGER	| col_tos4pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS4PRI) )
		{
			row->col_tos4pri = 0;
		}
		else
		{
			row->col_tos4pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS4PRI);
		}
		/* SQLITE_INTEGER	| col_tos5pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS5PRI) )
		{
			row->col_tos5pri = 0;
		}
		else
		{
			row->col_tos5pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS5PRI);
		}
		/* SQLITE_INTEGER	| col_tos6pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS6PRI) )
		{
			row->col_tos6pri = 0;
		}
		else
		{
			row->col_tos6pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS6PRI);
		}
		/* SQLITE_INTEGER	| col_tos7pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS7PRI) )
		{
			row->col_tos7pri = 0;
		}
		else
		{
			row->col_tos7pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TOS7PRI);
		}
		/* SQLITE_INTEGER	| col_sfbSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_SFBSTS) )
		{
			row->col_sfbSts = 0;
		}
		else
		{
			row->col_sfbSts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_SFBSTS);
		}
		/* SQLITE_INTEGER	| col_sfuSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_SFUSTS) )
		{
			row->col_sfuSts = 0;
		}
		else
		{
			row->col_sfuSts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_SFUSTS);
		}
		/* SQLITE_INTEGER	| col_sfmSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_SFMSTS) )
		{
			row->col_sfmSts = 0;
		}
		else
		{
			row->col_sfmSts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_SFMSTS);
		}
		/* SQLITE_INTEGER	| col_sfRate */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_SFRATE) )
		{
			row->col_sfRate = 0;
		}
		else
		{
			row->col_sfRate = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_SFRATE);
		}
		/* SQLITE_INTEGER	| col_vlanSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_VLANSTS) )
		{
			row->col_vlanSts = 0;
		}
		else
		{
			row->col_vlanSts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_VLANSTS);
		}
		/* SQLITE_INTEGER	| col_eth1vid */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH1VID) )
		{
			row->col_eth1vid = 0;
		}
		else
		{
			row->col_eth1vid = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH1VID);
		}
		/* SQLITE_INTEGER	| col_eth2vid */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH2VID) )
		{
			row->col_eth2vid = 0;
		}
		else
		{
			row->col_eth2vid = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH2VID);
		}
		/* SQLITE_INTEGER	| col_eth3vid */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH3VID) )
		{
			row->col_eth3vid = 0;
		}
		else
		{
			row->col_eth3vid = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH3VID);
		}
		/* SQLITE_INTEGER	| col_eth4vid */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH4VID) )
		{
			row->col_eth4vid = 0;
		}
		else
		{
			row->col_eth4vid = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH4VID);
		}
		/* SQLITE_INTEGER	| col_portPriSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_PORTPRISTS) )
		{
			row->col_portPriSts = 0;
		}
		else
		{
			row->col_portPriSts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_PORTPRISTS);
		}
		/* SQLITE_INTEGER	| col_eth1pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH1PRI) )
		{
			row->col_eth1pri = 0;
		}
		else
		{
			row->col_eth1pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH1PRI);
		}
		/* SQLITE_INTEGER	| col_eth2pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH2PRI) )
		{
			row->col_eth2pri = 0;
		}
		else
		{
			row->col_eth2pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH2PRI);
		}
		/* SQLITE_INTEGER	| col_eth3pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH3PRI) )
		{
			row->col_eth3pri = 0;
		}
		else
		{
			row->col_eth3pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH3PRI);
		}
		/* SQLITE_INTEGER	| col_eth4pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH4PRI) )
		{
			row->col_eth4pri = 0;
		}
		else
		{
			row->col_eth4pri = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH4PRI);
		}
		/* SQLITE_INTEGER	| col_rxLimitSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_RXLIMITSTS) )
		{
			row->col_rxLimitSts = 0;
		}
		else
		{
			row->col_rxLimitSts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_RXLIMITSTS);
		}
		/* SQLITE_INTEGER	| col_cpuPortRxRate */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_CPURX) )
		{
			row->col_cpuPortRxRate = 0;
		}
		else
		{
			row->col_cpuPortRxRate = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_CPURX);
		}
		/* SQLITE_INTEGER	| col_eth1rx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH1RX) )
		{
			row->col_eth1rx = 0;
		}
		else
		{
			row->col_eth1rx = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH1RX);
		}
		/* SQLITE_INTEGER	| col_eth2rx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH2RX) )
		{
			row->col_eth2rx = 0;
		}
		else
		{
			row->col_eth2rx = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH2RX);
		}
		/* SQLITE_INTEGER	| col_eth3rx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH3RX) )
		{
			row->col_eth3rx = 0;
		}
		else
		{
			row->col_eth3rx = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH3RX);
		}
		/* SQLITE_INTEGER	| col_eth4rx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH4RX) )
		{
			row->col_eth4rx = 0;
		}
		else
		{
			row->col_eth4rx = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH4RX);
		}
		/* SQLITE_INTEGER	| col_txLimitSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TXLIMITSTS) )
		{
			row->col_txLimitSts = 0;
		}
		else
		{
			row->col_txLimitSts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_TXLIMITSTS);
		}
		/* SQLITE_INTEGER	| col_cpuPortTxRate */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_CPUTX) )
		{
			row->col_cpuPortTxRate = 0;
		}
		else
		{
			row->col_cpuPortTxRate = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_CPUTX);
		}
		/* SQLITE_INTEGER	| col_eth1tx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH1TX) )
		{
			row->col_eth1tx = 0;
		}
		else
		{
			row->col_eth1tx = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH1TX);
		}
		/* SQLITE_INTEGER	| col_eth2tx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH2TX) )
		{
			row->col_eth2tx = 0;
		}
		else
		{
			row->col_eth2tx = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH2TX);
		}
		/* SQLITE_INTEGER	| col_eth3tx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH3TX) )
		{
			row->col_eth3tx = 0;
		}
		else
		{
			row->col_eth3tx = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH3TX);
		}
		/* SQLITE_INTEGER	| col_eth4tx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH4TX) )
		{
			row->col_eth4tx = 0;
		}
		else
		{
			row->col_eth4tx = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH4TX);
		}
		/* SQLITE_INTEGER	| col_psctlSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_PSCTL) )
		{
			row->col_psctlSts = 0;
		}
		else
		{
			row->col_psctlSts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_PSCTL);
		}
		/* SQLITE_INTEGER	| col_cpuPortSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_CPUPSTS) )
		{
			row->col_cpuPortSts = 0;
		}
		else
		{
			row->col_cpuPortSts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_CPUPSTS);
		}
		/* SQLITE_INTEGER	| col_eth1sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH1STS) )
		{
			row->col_eth1sts = 0;
		}
		else
		{
			row->col_eth1sts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH1STS);
		}
		/* SQLITE_INTEGER	| col_eth2sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH2STS) )
		{
			row->col_eth2sts = 0;
		}
		else
		{
			row->col_eth2sts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH2STS);
		}
		/* SQLITE_INTEGER	| col_eth3sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH3STS) )
		{
			row->col_eth3sts = 0;
		}
		else
		{
			row->col_eth3sts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH3STS);
		}
		/* SQLITE_INTEGER	| col_eth4sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH4STS) )
		{
			row->col_eth4sts = 0;
		}
		else
		{
			row->col_eth4sts = sqlite3_column_int(stmt, DBS_SYS_TBL_DEPRO_COL_ID_ETH4STS);
		}
		
		ret = SQLITE_OK;		
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_depro->sqlite3_step\n");
		ret = SQLITE_ERROR;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

int __dbs_underlayer_get_row_template(st_dbsTemplate *row)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != row );

	/* 组装SQL语句*/
	if( SQLITE_OK != __dbs_underlayer_SQLGetRowCnuTemplate(row->id, sql) )
	{
		return SQLITE_ERROR;
	}
	
	__dbsUnderlayerSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_template->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* SQLITE_INTEGER	| col_tempAutoSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_TEMPAUTOSTS) )
		{
			row->col_tempAutoSts = 0;			
		}
		else
		{
			row->col_tempAutoSts = sqlite3_column_int(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_TEMPAUTOSTS);
		}
		/* SQLITE_INTEGER	| col_curTemp */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_CURTEMP) )
		{
			row->col_curTemp = 0;
		}
		else
		{
			row->col_curTemp = sqlite3_column_int(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_CURTEMP);
		}
		/* SQLITE_INTEGER	| col_eth1VlanAddSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANADDSTS) )
		{
			row->col_eth1VlanAddSts = 0;
		}
		else
		{
			row->col_eth1VlanAddSts = sqlite3_column_int(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANADDSTS);
		}
		/* SQLITE_INTEGER	| col_eth1VlanStart */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANSTART) )
		{
			row->col_eth1VlanStart = 0;
		}
		else
		{
			row->col_eth1VlanStart = sqlite3_column_int(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANSTART);
		}
		/* SQLITE_INTEGER	| col_eth1VlanStop */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANSTOP) )
		{
			row->col_eth1VlanStop = 0;
		}
		else
		{
			row->col_eth1VlanStop = sqlite3_column_int(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH1VLANSTOP);
		}
		/* SQLITE_INTEGER	| col_eth2VlanAddSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANADDSTS) )
		{
			row->col_eth2VlanAddSts = 0;
		}
		else
		{
			row->col_eth2VlanAddSts = sqlite3_column_int(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANADDSTS);
		}
		/* SQLITE_INTEGER	| col_eth2VlanStart */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANSTART) )
		{
			row->col_eth2VlanStart = 0;
		}
		else
		{
			row->col_eth2VlanStart = sqlite3_column_int(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANSTART);
		}
		/* SQLITE_INTEGER	| col_eth2VlanStop */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANSTOP) )
		{
			row->col_eth2VlanStop = 0;
		}
		else
		{
			row->col_eth2VlanStop = sqlite3_column_int(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH2VLANSTOP);
		}
		/* SQLITE_INTEGER	| col_eth3VlanAddSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANADDSTS) )
		{
			row->col_eth3VlanAddSts = 0;
		}
		else
		{
			row->col_eth3VlanAddSts = sqlite3_column_int(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANADDSTS);
		}
		/* SQLITE_INTEGER	| col_eth3VlanStart */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANSTART) )
		{
			row->col_eth3VlanStart = 0;
		}
		else
		{
			row->col_eth3VlanStart = sqlite3_column_int(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANSTART);
		}
		/* SQLITE_INTEGER	| col_eth3VlanStop */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANSTOP) )
		{
			row->col_eth3VlanStop = 0;
		}
		else
		{
			row->col_eth3VlanStop = sqlite3_column_int(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH3VLANSTOP);
		}
		/* SQLITE_INTEGER	| col_eth4VlanAddSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANADDSTS) )
		{
			row->col_eth4VlanAddSts = 0;
		}
		else
		{
			row->col_eth4VlanAddSts = sqlite3_column_int(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANADDSTS);
		}
		/* SQLITE_INTEGER	| col_eth4VlanStart */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANSTART) )
		{
			row->col_eth4VlanStart = 0;
		}
		else
		{
			row->col_eth4VlanStart = sqlite3_column_int(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANSTART);
		}
		/* SQLITE_INTEGER	| col_eth4VlanStop */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANSTOP) )
		{
			row->col_eth4VlanStop = 0;
		}
		else
		{
			row->col_eth4VlanStop = sqlite3_column_int(stmt, DBS_SYS_TBL_TEMPLATE_COL_ID_ETH4VLANSTOP);
		}
		
		ret = SQLITE_OK;		
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_template->sqlite3_step\n");
		ret = SQLITE_ERROR;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

int __dbs_underlayer_get_row_network(st_dbsNetwork *row)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != row );

	/* 组装SQL语句*/
	if( SQLITE_OK != __dbs_underlayer_SQLGetRowNetwork(row->id, sql) )
	{
		return SQLITE_ERROR;
	}
	
	__dbsUnderlayerSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_network->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* SQLITE3_TEXT		| col_ip */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_NETWORK_COL_ID_IP) )
		{
			row->col_ip[0] = '\0';
		}
		else
		{
			strncpy(row->col_ip, sqlite3_column_text(stmt, DBS_SYS_TBL_NETWORK_COL_ID_IP), 
				tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_IP].col_len);
		}
		/* SQLITE3_TEXT		| col_netmask */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_NETWORK_COL_ID_MASK) )
		{
			row->col_netmask[0] = '\0';
		}
		else
		{
			strncpy(row->col_netmask, sqlite3_column_text(stmt, DBS_SYS_TBL_NETWORK_COL_ID_MASK), 
				tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_MASK].col_len);
		}
		/* SQLITE3_TEXT		| col_gw */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_NETWORK_COL_ID_GW) )
		{
			row->col_gw[0] = '\0';
		}
		else
		{
			strncpy(row->col_gw, sqlite3_column_text(stmt, DBS_SYS_TBL_NETWORK_COL_ID_GW), 
				tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_GW].col_len);
		}
		/* SQLITE_INTEGER	| col_dhcp */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_NETWORK_COL_ID_DHCP) )
		{
			row->col_dhcp = 0;
		}
		else
		{
			row->col_dhcp = sqlite3_column_int(stmt, DBS_SYS_TBL_NETWORK_COL_ID_DHCP);
		}
		/* SQLITE3_TEXT		| col_dns */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_NETWORK_COL_ID_DNS) )
		{
			row->col_dns[0] = '\0';
		}
		else
		{
			strncpy(row->col_dns, sqlite3_column_text(stmt, DBS_SYS_TBL_NETWORK_COL_ID_DNS), 
				tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_DNS].col_len);
		}
		/* SQLITE3_TEXT		| col_mac */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_NETWORK_COL_ID_MAC) )
		{
			row->col_mac[0] = '\0';
		}
		else
		{
			strncpy(row->col_mac, sqlite3_column_text(stmt, DBS_SYS_TBL_NETWORK_COL_ID_MAC), 
				tbl_network[DBS_SYS_TBL_NETWORK_COL_ID_MAC].col_len);
		}
		/* SQLITE_INTEGER	| col_mvlan_sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_NETWORK_COL_ID_MVSTS) )
		{
			row->col_mvlan_sts = 0;
		}
		else
		{
			row->col_mvlan_sts = sqlite3_column_int(stmt, DBS_SYS_TBL_NETWORK_COL_ID_MVSTS);
		}
		/* SQLITE_INTEGER	| col_mvlan_id */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_NETWORK_COL_ID_MVID) )
		{
			row->col_mvlan_id = 0;
		}
		else
		{
			row->col_mvlan_id = sqlite3_column_int(stmt, DBS_SYS_TBL_NETWORK_COL_ID_MVID);
		}
		
		ret = SQLITE_OK;		
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_network->sqlite3_step\n");
		ret = SQLITE_ERROR;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

int __dbs_underlayer_get_row_profile(st_dbsProfile *row)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != row );

	/* 组装SQL语句*/
	if( SQLITE_OK != __dbs_underlayer_SQLGetRowProfile(row->id, sql) )
	{
		return SQLITE_ERROR;
	}
	
	__dbsUnderlayerSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_profile->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* SQLITE_INTEGER	| col_row_sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ROWSTS) )
		{
			row->col_row_sts = 0;
		}
		else
		{
			row->col_row_sts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ROWSTS);
		}
		/* SQLITE_INTEGER	| col_base */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_BASE) )
		{
			row->col_base = 0;
		}
		else
		{
			row->col_base = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_BASE);
		}
		/* SQLITE_INTEGER	| col_macLimit */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_MACLIMIT) )
		{
			row->col_macLimit = 0;
		}
		else
		{
			row->col_macLimit = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_MACLIMIT);
		}
		/* SQLITE_INTEGER	| col_curate */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_CURATE) )
		{
			row->col_curate = 0;
		}
		else
		{
			row->col_curate = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_CURATE);
		}
		/* SQLITE_INTEGER	| col_cdrate */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_CDRATE) )
		{
			row->col_cdrate = 0;
		}
		else
		{
			row->col_cdrate = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_CDRATE);
		}
		/* SQLITE_INTEGER	| col_loagTime */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_LOAGE) )
		{
			row->col_loagTime = 0;
		}
		else
		{
			row->col_loagTime = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_LOAGE);
		}
		/* SQLITE_INTEGER	| col_reagTime */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_REAGE) )
		{
			row->col_reagTime = 0;
		}
		else
		{
			row->col_reagTime = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_REAGE);
		}
		/* SQLITE_INTEGER	| col_igmpPri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_IGMPPRI) )
		{
			row->col_igmpPri = 0;
		}
		else
		{
			row->col_igmpPri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_IGMPPRI);
		}
		/* SQLITE_INTEGER	| col_unicastPri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_UNIPRI) )
		{
			row->col_unicastPri = 0;
		}
		else
		{
			row->col_unicastPri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_UNIPRI);
		}
		/* SQLITE_INTEGER	| col_avsPri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_AVSPRI) )
		{
			row->col_avsPri = 0;
		}
		else
		{
			row->col_avsPri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_AVSPRI);
		}
		/* SQLITE_INTEGER	| col_mcastPri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_MCPRI) )
		{
			row->col_mcastPri = 0;
		}
		else
		{
			row->col_mcastPri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_MCPRI);
		}
		/* SQLITE_INTEGER	| col_tbaPriSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TBAPRISTS) )
		{
			row->col_tbaPriSts = 0;
		}
		else
		{
			row->col_tbaPriSts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TBAPRISTS);
		}
		/* SQLITE_INTEGER	| col_cosPriSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COSPRISTS) )
		{
			row->col_cosPriSts = 0;
		}
		else
		{
			row->col_cosPriSts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COSPRISTS);
		}
		/* SQLITE_INTEGER	| col_cos0pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS0PRI) )
		{
			row->col_cos0pri = 0;
		}
		else
		{
			row->col_cos0pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS0PRI);
		}
		/* SQLITE_INTEGER	| col_cos1pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS1PRI) )
		{
			row->col_cos1pri = 0;
		}
		else
		{
			row->col_cos1pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS1PRI);
		}
		/* SQLITE_INTEGER	| col_cos2pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS2PRI) )
		{
			row->col_cos2pri = 0;
		}
		else
		{
			row->col_cos2pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS2PRI);
		}
		/* SQLITE_INTEGER	| col_cos3pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS3PRI) )
		{
			row->col_cos3pri = 0;
		}
		else
		{
			row->col_cos3pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS3PRI);
		}
		/* SQLITE_INTEGER	| col_cos4pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS4PRI) )
		{
			row->col_cos4pri = 0;
		}
		else
		{
			row->col_cos4pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS4PRI);
		}
		/* SQLITE_INTEGER	| col_cos5pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS5PRI) )
		{
			row->col_cos5pri = 0;
		}
		else
		{
			row->col_cos5pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS5PRI);
		}
		/* SQLITE_INTEGER	| col_cos6pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS6PRI) )
		{
			row->col_cos6pri = 0;
		}
		else
		{
			row->col_cos6pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS6PRI);
		}
		/* SQLITE_INTEGER	| col_cos7pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS7PRI) )
		{
			row->col_cos7pri = 0;
		}
		else
		{
			row->col_cos7pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_COS7PRI);
		}
		/* SQLITE_INTEGER	| col_tosPriSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOSPRISTS) )
		{
			row->col_tosPriSts = 0;
		}
		else
		{
			row->col_tosPriSts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOSPRISTS);
		}
		/* SQLITE_INTEGER	| col_tos0pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS0PRI) )
		{
			row->col_tos0pri = 0;
		}
		else
		{
			row->col_tos0pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS0PRI);
		}
		/* SQLITE_INTEGER	| col_tos1pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS1PRI) )
		{
			row->col_tos1pri = 0;
		}
		else
		{
			row->col_tos1pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS1PRI);
		}
		/* SQLITE_INTEGER	| col_tos2pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS2PRI) )
		{
			row->col_tos2pri = 0;
		}
		else
		{
			row->col_tos2pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS2PRI);
		}
		/* SQLITE_INTEGER	| col_tos3pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS3PRI) )
		{
			row->col_tos3pri = 0;
		}
		else
		{
			row->col_tos3pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS3PRI);
		}
		/* SQLITE_INTEGER	| col_tos4pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS4PRI) )
		{
			row->col_tos4pri = 0;
		}
		else
		{
			row->col_tos4pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS4PRI);
		}
		/* SQLITE_INTEGER	| col_tos5pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS5PRI) )
		{
			row->col_tos5pri = 0;
		}
		else
		{
			row->col_tos5pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS5PRI);
		}
		/* SQLITE_INTEGER	| col_tos6pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS6PRI) )
		{
			row->col_tos6pri = 0;
		}
		else
		{
			row->col_tos6pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS6PRI);
		}
		/* SQLITE_INTEGER	| col_tos7pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS7PRI) )
		{
			row->col_tos7pri = 0;
		}
		else
		{
			row->col_tos7pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TOS7PRI);
		}
		/* SQLITE_INTEGER	| col_sfbSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_SFBSTS) )
		{
			row->col_sfbSts = 0;
		}
		else
		{
			row->col_sfbSts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_SFBSTS);
		}
		/* SQLITE_INTEGER	| col_sfuSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_SFUSTS) )
		{
			row->col_sfuSts = 0;
		}
		else
		{
			row->col_sfuSts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_SFUSTS);
		}
		/* SQLITE_INTEGER	| col_sfmSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_SFMSTS) )
		{
			row->col_sfmSts = 0;
		}
		else
		{
			row->col_sfmSts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_SFMSTS);
		}
		/* SQLITE_INTEGER	| col_sfRate */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_SFRATE) )
		{
			row->col_sfRate = 0;
		}
		else
		{
			row->col_sfRate = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_SFRATE);
		}
		/* SQLITE_INTEGER	| col_vlanSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_VLANSTS) )
		{
			row->col_vlanSts = 0;
		}
		else
		{
			row->col_vlanSts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_VLANSTS);
		}
		/* SQLITE_INTEGER	| col_eth1vid */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH1VID) )
		{
			row->col_eth1vid = 0;
		}
		else
		{
			row->col_eth1vid = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH1VID);
		}
		/* SQLITE_INTEGER	| col_eth2vid */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH2VID) )
		{
			row->col_eth2vid = 0;
		}
		else
		{
			row->col_eth2vid = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH2VID);
		}
		/* SQLITE_INTEGER	| col_eth3vid */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH3VID) )
		{
			row->col_eth3vid = 0;
		}
		else
		{
			row->col_eth3vid = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH3VID);
		}
		/* SQLITE_INTEGER	| col_eth4vid */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH4VID) )
		{
			row->col_eth4vid = 0;
		}
		else
		{
			row->col_eth4vid = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH4VID);
		}
		/* SQLITE_INTEGER	| col_portPriSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_PORTPRISTS) )
		{
			row->col_portPriSts = 0;
		}
		else
		{
			row->col_portPriSts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_PORTPRISTS);
		}
		/* SQLITE_INTEGER	| col_eth1pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH1PRI) )
		{
			row->col_eth1pri = 0;
		}
		else
		{
			row->col_eth1pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH1PRI);
		}
		/* SQLITE_INTEGER	| col_eth2pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH2PRI) )
		{
			row->col_eth2pri = 0;
		}
		else
		{
			row->col_eth2pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH2PRI);
		}
		/* SQLITE_INTEGER	| col_eth3pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH3PRI) )
		{
			row->col_eth3pri = 0;
		}
		else
		{
			row->col_eth3pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH3PRI);
		}
		/* SQLITE_INTEGER	| col_eth4pri */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH4PRI) )
		{
			row->col_eth4pri = 0;
		}
		else
		{
			row->col_eth4pri = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH4PRI);
		}
		/* SQLITE_INTEGER	| col_rxLimitSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_RXLIMITSTS) )
		{
			row->col_rxLimitSts = 0;
		}
		else
		{
			row->col_rxLimitSts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_RXLIMITSTS);
		}
		/* SQLITE_INTEGER	| col_cpuPortRxRate */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_CPURX) )
		{
			row->col_cpuPortRxRate = 0;
		}
		else
		{
			row->col_cpuPortRxRate = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_CPURX);
		}
		/* SQLITE_INTEGER	| col_eth1rx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH1RX) )
		{
			row->col_eth1rx = 0;
		}
		else
		{
			row->col_eth1rx = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH1RX);
		}
		/* SQLITE_INTEGER	| col_eth2rx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH2RX) )
		{
			row->col_eth2rx = 0;
		}
		else
		{
			row->col_eth2rx = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH2RX);
		}
		/* SQLITE_INTEGER	| col_eth3rx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH3RX) )
		{
			row->col_eth3rx = 0;
		}
		else
		{
			row->col_eth3rx = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH3RX);
		}
		/* SQLITE_INTEGER	| col_eth4rx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH4RX) )
		{
			row->col_eth4rx = 0;
		}
		else
		{
			row->col_eth4rx = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH4RX);
		}
		/* SQLITE_INTEGER	| col_txLimitSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TXLIMITSTS) )
		{
			row->col_txLimitSts = 0;
		}
		else
		{
			row->col_txLimitSts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_TXLIMITSTS);
		}
		/* SQLITE_INTEGER	| col_cpuPortTxRate */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_CPUTX) )
		{
			row->col_cpuPortTxRate = 0;
		}
		else
		{
			row->col_cpuPortTxRate = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_CPUTX);
		}
		/* SQLITE_INTEGER	| col_eth1tx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH1TX) )
		{
			row->col_eth1tx = 0;
		}
		else
		{
			row->col_eth1tx = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH1TX);
		}
		/* SQLITE_INTEGER	| col_eth2tx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH2TX) )
		{
			row->col_eth2tx = 0;
		}
		else
		{
			row->col_eth2tx = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH2TX);
		}
		/* SQLITE_INTEGER	| col_eth3tx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH3TX) )
		{
			row->col_eth3tx = 0;
		}
		else
		{
			row->col_eth3tx = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH3TX);
		}
		/* SQLITE_INTEGER	| col_eth4tx */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH4TX) )
		{
			row->col_eth4tx = 0;
		}
		else
		{
			row->col_eth4tx = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH4TX);
		}
		/* SQLITE_INTEGER	| col_psctlSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_PSCTL) )
		{
			row->col_psctlSts = 0;
		}
		else
		{
			row->col_psctlSts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_PSCTL);
		}
		/* SQLITE_INTEGER	| col_cpuPortSts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_CPUPSTS) )
		{
			row->col_cpuPortSts = 0;
		}
		else
		{
			row->col_cpuPortSts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_CPUPSTS);
		}
		/* SQLITE_INTEGER	| col_eth1sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH1STS) )
		{
			row->col_eth1sts = 0;
		}
		else
		{
			row->col_eth1sts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH1STS);
		}
		/* SQLITE_INTEGER	| col_eth2sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH2STS) )
		{
			row->col_eth2sts = 0;
		}
		else
		{
			row->col_eth2sts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH2STS);
		}
		/* SQLITE_INTEGER	| col_eth3sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH3STS) )
		{
			row->col_eth3sts = 0;
		}
		else
		{
			row->col_eth3sts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH3STS);
		}
		/* SQLITE_INTEGER	| col_eth4sts */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH4STS) )
		{
			row->col_eth4sts = 0;
		}
		else
		{
			row->col_eth4sts = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH4STS);
		}
		/* SQLITE_INTEGER	| col_uplinkvid */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_UPLINKVID) )
		{
			row->col_uplinkvid = 0;
		}
		else
		{
			row->col_uplinkvid = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_UPLINKVID);
		}
		/* SQLITE_INTEGER	| col_uplinkvmod */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_UPLINKVMODE) )
		{
			row->col_uplinkVMode = 3;
		}
		else
		{
			row->col_uplinkVMode = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_UPLINKVMODE);
		}
		/* SQLITE_INTEGER	| col_eth1vmod */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH1VMODE) )
		{
			row->col_eth1VMode = 3;
		}
		else
		{
			row->col_eth1VMode = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH1VMODE);
		}
		/* SQLITE_INTEGER	| col_eth2vmod */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH2VMODE) )
		{
			row->col_eth2VMode = 3;
		}
		else
		{
			row->col_eth2VMode = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH2VMODE);
		}
			/* SQLITE_INTEGER	| col_eth3kvmod */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH3VMODE) )
		{
			row->col_eth3VMode = 3;
		}
		else
		{
			row->col_eth3VMode = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH3VMODE);
		}
			/* SQLITE_INTEGER	| col_eth1kvmod */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH4VMODE) )
		{
			row->col_eth4VMode = 3;
		}
		else
		{
			row->col_eth4VMode = sqlite3_column_int(stmt, DBS_SYS_TBL_PROFILE_COL_ID_ETH4VMODE);
		}
		
		ret = SQLITE_OK;		
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_profile->sqlite3_step\n");
		ret = SQLITE_ERROR;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

int __dbs_underlayer_get_row_snmp(st_dbsSnmp *row)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != row );

	/* 组装SQL语句*/
	if( SQLITE_OK != __dbs_underlayer_SQLGetRowSnmp(row->id, sql) )
	{
		return SQLITE_ERROR;
	}
	
	__dbsUnderlayerSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_snmp->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* SQLITE3_TEXT		| col_rdcom */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SNMP_COL_ID_RC) )
		{
			row->col_rdcom[0] = '\0';
		}
		else
		{
			strncpy(row->col_rdcom, sqlite3_column_text(stmt, DBS_SYS_TBL_SNMP_COL_ID_RC), 
				tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_RC].col_len);
		}
		/* SQLITE3_TEXT		| col_wrcom */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SNMP_COL_ID_WC) )
		{
			row->col_wrcom[0] = '\0';
		}
		else
		{
			strncpy(row->col_wrcom, sqlite3_column_text(stmt, DBS_SYS_TBL_SNMP_COL_ID_WC), 
				tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_WC].col_len);
		}
		/* SQLITE3_TEXT		| col_trapcom */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SNMP_COL_ID_TC) )
		{
			row->col_trapcom[0] = '\0';
		}
		else
		{
			strncpy(row->col_trapcom, sqlite3_column_text(stmt, DBS_SYS_TBL_SNMP_COL_ID_TC), 
				tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_TC].col_len);
		}
		/* SQLITE3_TEXT		| col_sina */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SNMP_COL_ID_SA) )
		{
			row->col_sina[0] = '\0';
		}
		else
		{
			strncpy(row->col_sina, sqlite3_column_text(stmt, DBS_SYS_TBL_SNMP_COL_ID_SA), 
				tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_SA].col_len);
		}
		/* SQLITE3_TEXT		| col_sinb */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SNMP_COL_ID_SB) )
		{
			row->col_sinb[0] = '\0';
		}
		else
		{
			strncpy(row->col_sinb, sqlite3_column_text(stmt, DBS_SYS_TBL_SNMP_COL_ID_SB), 
				tbl_snmp[DBS_SYS_TBL_SNMP_COL_ID_SB].col_len);
		}
		/* SQLITE_INTEGER	| col_tpa */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SNMP_COL_ID_PA) )
		{
			row->col_tpa = 0;
		}
		else
		{
			row->col_tpa = sqlite3_column_int(stmt, DBS_SYS_TBL_SNMP_COL_ID_PA);
		}
		/* SQLITE_INTEGER	| col_tpb */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SNMP_COL_ID_PB) )
		{
			row->col_tpb = 0;
		}
		else
		{
			row->col_tpb = sqlite3_column_int(stmt, DBS_SYS_TBL_SNMP_COL_ID_PB);
		}
		
		ret = SQLITE_OK;		
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_snmp->sqlite3_step\n");
		ret = SQLITE_ERROR;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

int __dbs_underlayer_get_row_swmgmt(st_dbsSwmgmt *row)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != row );

	/* 组装SQL语句*/
	if( SQLITE_OK != __dbs_underlayer_SQLGetRowSwmgmt(row->id, sql) )
	{
		return SQLITE_ERROR;
	}
	
	__dbsUnderlayerSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_swmgmt->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* SQLITE3_TEXT		| col_ip */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SWMGMT_COL_ID_IP) )
		{
			row->col_ip[0] = '\0';
		}
		else
		{
			strncpy(row->col_ip, sqlite3_column_text(stmt, DBS_SYS_TBL_SWMGMT_COL_ID_IP), 
				tbl_swmgmt[DBS_SYS_TBL_SWMGMT_COL_ID_IP].col_len);
		}
		/* SQLITE_INTEGER	| col_port */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SWMGMT_COL_ID_PORT) )
		{
			row->col_port = 0;
		}
		else
		{
			row->col_port = sqlite3_column_int(stmt, DBS_SYS_TBL_SWMGMT_COL_ID_PORT);
		}
		/* SQLITE3_TEXT		| col_user */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SWMGMT_COL_ID_USR) )
		{
			row->col_user[0] = '\0';
		}
		else
		{
			strncpy(row->col_user, sqlite3_column_text(stmt, DBS_SYS_TBL_SWMGMT_COL_ID_USR), 
				tbl_swmgmt[DBS_SYS_TBL_SWMGMT_COL_ID_USR].col_len);
		}
		/* SQLITE3_TEXT		| col_pwd */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SWMGMT_COL_ID_PWD) )
		{
			row->col_pwd[0] = '\0';
		}
		else
		{
			strncpy(row->col_pwd, sqlite3_column_text(stmt, DBS_SYS_TBL_SWMGMT_COL_ID_PWD), 
				tbl_swmgmt[DBS_SYS_TBL_SWMGMT_COL_ID_PWD].col_len);
		}
		/* SQLITE3_TEXT		| col_path */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SWMGMT_COL_ID_PATH) )
		{
			row->col_path[0] = '\0';
		}
		else
		{
			strncpy(row->col_path, sqlite3_column_text(stmt, DBS_SYS_TBL_SWMGMT_COL_ID_PATH), 
				tbl_swmgmt[DBS_SYS_TBL_SWMGMT_COL_ID_PATH].col_len);
		}
		
		ret = SQLITE_OK;		
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_swmgmt->sqlite3_step\n");
		ret = SQLITE_ERROR;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

int __dbs_underlayer_get_row_sysinfo(st_dbsSysinfo *row)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != row );

	/* 组装SQL语句*/
	if( SQLITE_OK != __dbs_underlayer_SQLGetRowSysinfo(row->id, sql) )
	{
		return SQLITE_ERROR;
	}
	
	__dbsUnderlayerSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_sysinfo->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* SQLITE_INTEGER	| col_model */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_MODEL) )
		{
			row->col_model = 0;
		}
		else
		{
			row->col_model = sqlite3_column_int(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_MODEL);
		}
		/* SQLITE_INTEGER	| col_maxclt */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_MAXCLT) )
		{
			row->col_maxclt = 0;
		}
		else
		{
			row->col_maxclt = sqlite3_column_int(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_MAXCLT);
		}
		/* SQLITE3_TEXT		| col_hwver */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_HWVER) )
		{
			row->col_hwver[0] = '\0';
		}
		else
		{
			strncpy(row->col_hwver, sqlite3_column_text(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_HWVER), 
				tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_HWVER].col_len);
		}
		/* SQLITE3_TEXT		| col_bver */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_BVER) )
		{
			row->col_bver[0] = '\0';
		}
		else
		{
			strncpy(row->col_bver, sqlite3_column_text(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_BVER), 
				tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_BVER].col_len);
		}
		/* SQLITE3_TEXT		| col_kver */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_KVER) )
		{
			row->col_kver[0] = '\0';
		}
		else
		{
			strncpy(row->col_kver, sqlite3_column_text(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_KVER), 
				tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_KVER].col_len);
		}
		/* SQLITE3_TEXT		| col_appver */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_APPVER) )
		{
			row->col_appver[0] = '\0';
		}
		else
		{
			strncpy(row->col_appver, sqlite3_column_text(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_APPVER), 
				tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_APPVER].col_len);
		}
		/* SQLITE_INTEGER	| col_flashsize */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_FS) )
		{
			row->col_flashsize = 0;
		}
		else
		{
			row->col_flashsize = sqlite3_column_int(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_FS);
		}
		/* SQLITE_INTEGER	| col_ramsize */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_RS) )
		{
			row->col_ramsize = 0;
		}
		else
		{
			row->col_ramsize = sqlite3_column_int(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_RS);
		}
		/* SQLITE_INTEGER	| col_wlctl */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_WLCTL) )
		{
			row->col_wlctl = 0;
		}
		else
		{
			row->col_wlctl = sqlite3_column_int(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_WLCTL);
		}
		/* SQLITE_INTEGER	| col_autoud */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_AU) )
		{
			row->col_autoud = 0;
		}
		else
		{
			row->col_autoud = sqlite3_column_int(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_AU);
		}
		/* SQLITE_INTEGER	| col_autoconf */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_AC) )
		{
			row->col_autoconf = 0;
		}
		else
		{
			row->col_autoconf = sqlite3_column_int(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_AC);
		}
		/* SQLITE_INTEGER	| col_wdt */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_WDT) )
		{
			row->col_wdt = 0;
		}
		else
		{
			row->col_wdt = sqlite3_column_int(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_WDT);
		}
		/* SQLITE3_TEXT		| col_mfinfo */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_MF) )
		{
			row->col_mfinfo[0] = '\0';
		}
		else
		{
			/*
			strncpy(row->col_mfinfo, sqlite3_column_text(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_MF), 
				tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_MF].col_len);*/
			strcpy(row->col_mfinfo, boardapi_getMenufactoryStr());
		}
		/* SQLITE_INTEGER	| col_p6rxdelay */
		/* 为了兼容老的数据表格式，AR7410和88E6171R是通过port6的RGMII接口
		** 连接的，AR7410为RGMII MAC，88E6171R为RGMII PHY，该接口通过88E6171R
		** 的寄存器配置了RGMII RX Delay，而AR7410没有配置TX Delay，这样存在
		** 一个问题，就是Port6需要软件初始化之后才能通，而一旦软件
		** 不能运行，马上就会影响到用户上网，现在在新的硬件设计中
		** 通过IO脚来使能了AR7410的RX 和TX Delay，而软件上不需要通过软件
		** 来配置88E6171R的Delay了，这样一来，即使软件出现异常也不会
		** 影响用户上网*/
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_P6RXD) )
		{
			/* 如果读取不到col_p6rxdelay这一列，则认为是老的机器，默认
			** 开启 88E6171R 的Port6 的RGMII RX Delay */
			row->col_p6rxdelay = 1;	/* enable dsdt port 6 rgmii rx delay */
		}
		else
		{
			row->col_p6rxdelay = sqlite3_column_int(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_P6RXD);
		}
		/* SQLITE_INTEGER	| col_p6txdelay */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_P6TXD) )
		{
			/* 如果读取不到col_p6txdelay这一列，则认为是老的机器，默认
			** 关闭88E6171R 的Port6 的RGMII TX Delay */
			row->col_p6txdelay = 0;	/* disable dsdt port 6 rgmii tx delay */
		}
		else
		{
			row->col_p6txdelay = sqlite3_column_int(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_P6TXD);
		}
		/* SQLITE3_TEXT		| col_apphash */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_APPHASH) )
		{
			row->col_apphash[0] = '\0';
		}
		else
		{
			strncpy(row->col_apphash, sqlite3_column_text(stmt, DBS_SYS_TBL_SYSINFO_COL_ID_APPHASH), 
				tbl_sysinfo[DBS_SYS_TBL_SYSINFO_COL_ID_APPHASH].col_len);
		}
		
		ret = SQLITE_OK;		
	}
	else
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_get_row_sysinfo->sqlite3_step\n");
		ret = SQLITE_ERROR;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

int __dbs_underlayer_update_row_clirole(st_dbsCliRole *row)
{
	//char sql[DBS_SQL_BIG_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	assert( NULL != row );

	if( SQLITE_OK == __dbs_underlayer_SQLUpdateRowCliRole(row, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_select_cnu_index_by_mac(char *mac, stCnuNode *index)
{
	int ret = SQLITE_ERROR;
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != mac );
	assert( NULL != index );

	/* 组装SQL语句*/
	if( SQLITE_OK != __dbs_underlayer_SQLSelectCnuByMac(mac, sql) )
	{
		return SQLITE_ERROR;
	}
	
	__dbsUnderlayerSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsSystemFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_select_cnu_index_by_mac->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* SQLITE_INTEGER	| id */
		if( SQLITE_NULL == sqlite3_column_type(stmt, DBS_SYS_TBL_CNU_COL_ID_ID) )
		{
			index->clt = 0;
			index->cnu = 0;
			ret = SQLITE_ERROR;
		}
		else
		{
			index->clt = (sqlite3_column_int(stmt, DBS_SYS_TBL_CNU_COL_ID_ID) -1) / MAX_CNUS_PER_CLT + 1;
			index->cnu = sqlite3_column_int(stmt, DBS_SYS_TBL_CNU_COL_ID_ID);
			ret = SQLITE_OK;
		}
				
	}
	else
	{
		/* can not select the cnu */
		index->clt = 0;
		index->cnu = 0;
		ret = SQLITE_OK;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

int __dbs_underlayer_update_row_clt(st_dbsClt *row)
{
	//char sql[DBS_SQL_BIG_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	assert( NULL != row );

	if( SQLITE_OK == __dbs_underlayer_SQLUpdateRowClt(row, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_update_row_cltconf(st_dbsCltConf *row)
{
	//char sql[DBS_SQL_BIG_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	assert( NULL != row );

	if( SQLITE_OK == __dbs_underlayer_SQLUpdateRowCltConf(row, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_update_row_cnu(st_dbsCnu *row)
{
	//char sql[DBS_SQL_BIG_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	assert( NULL != row );

	if( SQLITE_OK == __dbs_underlayer_SQLUpdateRowCnu(row, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_update_row_depro(st_dbsCnuDefaultProfile *row)
{
	//char sql[DBS_SQL_LARGE_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	assert( NULL != row );

	if( SQLITE_OK == __dbs_underlayer_SQLUpdateRowCnuDepro(row, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_update_row_network(st_dbsNetwork *row)
{
	//char sql[DBS_SQL_BIG_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	assert( NULL != row );

	if( SQLITE_OK == __dbs_underlayer_SQLUpdateRowNetwork(row, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_update_row_profile(st_dbsProfile *row)
{
	//char sql[DBS_SQL_LARGE_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	assert( NULL != row );

	if( SQLITE_OK == __dbs_underlayer_SQLUpdateRowProfile(row, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_update_row_template(st_dbsTemplate *row)
{
	//char sql[DBS_SQL_LARGE_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	assert( NULL != row );

	if( SQLITE_OK == __dbs_underlayer_SQLUpdateRowTemplate(row, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_update_row_snmp(st_dbsSnmp *row)
{
	//char sql[DBS_SQL_BIG_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	assert( NULL != row );

	if( SQLITE_OK == __dbs_underlayer_SQLUpdateRowSnmp(row, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_update_row_swmgmt(st_dbsSwmgmt *row)
{
	//char sql[DBS_SQL_BIG_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	assert( NULL != row );

	if( SQLITE_OK == __dbs_underlayer_SQLUpdateRowSwmgmt(row, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_update_row_sysinfo(st_dbsSysinfo *row)
{
	//char sql[DBS_SQL_BIG_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	assert( NULL != row );

	if( SQLITE_OK == __dbs_underlayer_SQLUpdateRowSysinfo(row, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_destory_row_clt(uint16_t id)
{
	//char sql[DBS_SQL_BIG_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	if( SQLITE_OK == __dbs_underlayer_SQLDestroyRowClt(id, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_destory_row_cltconf(uint16_t id)
{
	//char sql[DBS_SQL_BIG_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	if( SQLITE_OK == __dbs_underlayer_SQLDestroyRowCltconf(id, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_destory_row_cnu(uint16_t id)
{
	//char sql[DBS_SQL_BIG_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	if( SQLITE_OK == __dbs_underlayer_SQLDestroyRowCnu(id, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_destory_row_profile(uint16_t id)
{
	//char sql[DBS_SQL_LARGE_LEN] = {0};
	uint8_t *sql = gBuf_dbsUnderlayer;
	bzero(sql, DBS_SQL_LARGE_LEN);

	if( SQLITE_OK == __dbs_underlayer_SQLDestroyRowProfile(id, sql) )
	{
		__dbsUnderlayerSQL(sql);
		return __dbs_underlayer_sql_exec(sql);
	}
	else
	{
		return SQLITE_ERROR;
	}
}

int __dbs_underlayer_create_su_profile(uint16_t id)
{
	st_dbsCnuDefaultProfile st_tblDepro;
	st_dbsProfile st_tblProfile;

	/* 读取tbl_depro 表中的CNU 默认模板*/
	st_tblDepro.id = 1;
	if( SQLITE_OK != __dbs_underlayer_get_row_depro(&st_tblDepro) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_create_su_profile->__dbs_underlayer_get_row_depro !\n");
		return SQLITE_ERROR;
	}

	/* create_su_profile */
	st_tblProfile.id = id;
	st_tblProfile.col_row_sts = 1;
	st_tblProfile.col_base = st_tblDepro.col_base;
	st_tblProfile.col_macLimit = st_tblDepro.col_macLimit;
	st_tblProfile.col_curate = st_tblDepro.col_curate;
	st_tblProfile.col_cdrate = st_tblDepro.col_cdrate;
	st_tblProfile.col_loagTime = st_tblDepro.col_loagTime;
	st_tblProfile.col_reagTime = st_tblDepro.col_reagTime;
	st_tblProfile.col_igmpPri = st_tblDepro.col_igmpPri;
	st_tblProfile.col_unicastPri = st_tblDepro.col_unicastPri;
	st_tblProfile.col_avsPri = st_tblDepro.col_avsPri;
	st_tblProfile.col_mcastPri = st_tblDepro.col_mcastPri;
	st_tblProfile.col_tbaPriSts = st_tblDepro.col_tbaPriSts;
	st_tblProfile.col_cosPriSts = st_tblDepro.col_cosPriSts;
	st_tblProfile.col_cos0pri = st_tblDepro.col_cos0pri;
	st_tblProfile.col_cos1pri = st_tblDepro.col_cos1pri;
	st_tblProfile.col_cos2pri = st_tblDepro.col_cos2pri;
	st_tblProfile.col_cos3pri = st_tblDepro.col_cos3pri;
	st_tblProfile.col_cos4pri = st_tblDepro.col_cos4pri;
	st_tblProfile.col_cos5pri = st_tblDepro.col_cos5pri;
	st_tblProfile.col_cos6pri = st_tblDepro.col_cos6pri;
	st_tblProfile.col_cos7pri = st_tblDepro.col_cos7pri;
	st_tblProfile.col_tosPriSts = st_tblDepro.col_tosPriSts;
	st_tblProfile.col_tos0pri = st_tblDepro.col_tos0pri;
	st_tblProfile.col_tos1pri = st_tblDepro.col_tos1pri;
	st_tblProfile.col_tos2pri = st_tblDepro.col_tos2pri;
	st_tblProfile.col_tos3pri = st_tblDepro.col_tos3pri;
	st_tblProfile.col_tos4pri = st_tblDepro.col_tos4pri;
	st_tblProfile.col_tos5pri = st_tblDepro.col_tos5pri;
	st_tblProfile.col_tos6pri = st_tblDepro.col_tos6pri;
	st_tblProfile.col_tos7pri = st_tblDepro.col_tos7pri;
	st_tblProfile.col_sfbSts = st_tblDepro.col_sfbSts;
	st_tblProfile.col_sfuSts = st_tblDepro.col_sfuSts;
	st_tblProfile.col_sfmSts = st_tblDepro.col_sfmSts;
	st_tblProfile.col_sfRate = st_tblDepro.col_sfRate;
	st_tblProfile.col_vlanSts = 0;
	st_tblProfile.col_eth1vid = 1;
	st_tblProfile.col_eth2vid = 1;
	st_tblProfile.col_eth3vid = 1;
	st_tblProfile.col_eth4vid = 1;
	st_tblProfile.col_portPriSts = 0;
	st_tblProfile.col_eth1pri = 0;
	st_tblProfile.col_eth2pri = 0;
	st_tblProfile.col_eth3pri = 0;
	st_tblProfile.col_eth4pri = 0;
	st_tblProfile.col_rxLimitSts = 0;
	st_tblProfile.col_cpuPortRxRate = 0;
	st_tblProfile.col_eth1rx = 0;
	st_tblProfile.col_eth2rx = 0;
	st_tblProfile.col_eth3rx = 0;
	st_tblProfile.col_eth4rx = 0;
	st_tblProfile.col_txLimitSts = 0;
	st_tblProfile.col_cpuPortTxRate = 0;
	st_tblProfile.col_eth1tx = 0;
	st_tblProfile.col_eth2tx = 0;
	st_tblProfile.col_eth3tx = 0;
	st_tblProfile.col_eth4tx = 0;
	st_tblProfile.col_psctlSts = 1;
	st_tblProfile.col_cpuPortSts = 1;
	st_tblProfile.col_eth1sts = 1;
	st_tblProfile.col_eth2sts = 1;
	st_tblProfile.col_eth3sts = 1;
	st_tblProfile.col_eth4sts = 1;

	return __dbs_underlayer_update_row_profile(&st_tblProfile);
}

int __dbs_underlayer_create_dewl_profile(uint16_t id)
{
	st_dbsCnuDefaultProfile st_tblDepro;
	st_dbsProfile st_tblProfile;

	/* 读取tbl_depro 表中的CNU 默认模板*/
	st_tblDepro.id = 1;
	if( SQLITE_OK != __dbs_underlayer_get_row_depro(&st_tblDepro) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_create_dewl_profile->__dbs_underlayer_get_row_depro !\n");
		return SQLITE_ERROR;
	}

	/* create_su_profile */
	st_tblProfile.id = id;
	st_tblProfile.col_row_sts = 1;
	st_tblProfile.col_base = st_tblDepro.col_base;
	st_tblProfile.col_macLimit = st_tblDepro.col_macLimit;
	st_tblProfile.col_curate = st_tblDepro.col_curate;
	st_tblProfile.col_cdrate = st_tblDepro.col_cdrate;
	st_tblProfile.col_loagTime = st_tblDepro.col_loagTime;
	st_tblProfile.col_reagTime = st_tblDepro.col_reagTime;
	st_tblProfile.col_igmpPri = st_tblDepro.col_igmpPri;
	st_tblProfile.col_unicastPri = st_tblDepro.col_unicastPri;
	st_tblProfile.col_avsPri = st_tblDepro.col_avsPri;
	st_tblProfile.col_mcastPri = st_tblDepro.col_mcastPri;
	st_tblProfile.col_tbaPriSts = st_tblDepro.col_tbaPriSts;
	st_tblProfile.col_cosPriSts = st_tblDepro.col_cosPriSts;
	st_tblProfile.col_cos0pri = st_tblDepro.col_cos0pri;
	st_tblProfile.col_cos1pri = st_tblDepro.col_cos1pri;
	st_tblProfile.col_cos2pri = st_tblDepro.col_cos2pri;
	st_tblProfile.col_cos3pri = st_tblDepro.col_cos3pri;
	st_tblProfile.col_cos4pri = st_tblDepro.col_cos4pri;
	st_tblProfile.col_cos5pri = st_tblDepro.col_cos5pri;
	st_tblProfile.col_cos6pri = st_tblDepro.col_cos6pri;
	st_tblProfile.col_cos7pri = st_tblDepro.col_cos7pri;
	st_tblProfile.col_tosPriSts = st_tblDepro.col_tosPriSts;
	st_tblProfile.col_tos0pri = st_tblDepro.col_tos0pri;
	st_tblProfile.col_tos1pri = st_tblDepro.col_tos1pri;
	st_tblProfile.col_tos2pri = st_tblDepro.col_tos2pri;
	st_tblProfile.col_tos3pri = st_tblDepro.col_tos3pri;
	st_tblProfile.col_tos4pri = st_tblDepro.col_tos4pri;
	st_tblProfile.col_tos5pri = st_tblDepro.col_tos5pri;
	st_tblProfile.col_tos6pri = st_tblDepro.col_tos6pri;
	st_tblProfile.col_tos7pri = st_tblDepro.col_tos7pri;
	st_tblProfile.col_sfbSts = st_tblDepro.col_sfbSts;
	st_tblProfile.col_sfuSts = st_tblDepro.col_sfuSts;
	st_tblProfile.col_sfmSts = st_tblDepro.col_sfmSts;
	st_tblProfile.col_sfRate = st_tblDepro.col_sfRate;
	st_tblProfile.col_vlanSts = st_tblDepro.col_vlanSts;
	st_tblProfile.col_eth1vid = st_tblDepro.col_eth1vid;
	st_tblProfile.col_eth2vid = st_tblDepro.col_eth2vid;
	st_tblProfile.col_eth3vid = st_tblDepro.col_eth3vid;
	st_tblProfile.col_eth4vid = st_tblDepro.col_eth4vid;
	st_tblProfile.col_portPriSts = st_tblDepro.col_portPriSts;
	st_tblProfile.col_eth1pri = st_tblDepro.col_eth1pri;
	st_tblProfile.col_eth2pri = st_tblDepro.col_eth2pri;
	st_tblProfile.col_eth3pri = st_tblDepro.col_eth3pri;
	st_tblProfile.col_eth4pri = st_tblDepro.col_eth4pri;
	st_tblProfile.col_rxLimitSts = st_tblDepro.col_rxLimitSts;
	st_tblProfile.col_cpuPortRxRate = st_tblDepro.col_cpuPortRxRate;
	st_tblProfile.col_eth1rx = st_tblDepro.col_eth1rx;
	st_tblProfile.col_eth2rx = st_tblDepro.col_eth2rx;
	st_tblProfile.col_eth3rx = st_tblDepro.col_eth3rx;
	st_tblProfile.col_eth4rx = st_tblDepro.col_eth4rx;
	st_tblProfile.col_txLimitSts = st_tblDepro.col_txLimitSts;
	st_tblProfile.col_cpuPortTxRate = st_tblDepro.col_cpuPortTxRate;
	st_tblProfile.col_eth1tx = st_tblDepro.col_eth1tx;
	st_tblProfile.col_eth2tx = st_tblDepro.col_eth2tx;
	st_tblProfile.col_eth3tx = st_tblDepro.col_eth3tx;
	st_tblProfile.col_eth4tx = st_tblDepro.col_eth4tx;
	st_tblProfile.col_psctlSts = 1;
	st_tblProfile.col_cpuPortSts = 1;
	st_tblProfile.col_eth1sts = 1;
	st_tblProfile.col_eth2sts = 1;
	st_tblProfile.col_eth3sts = 1;
	st_tblProfile.col_eth4sts = 1;

	return __dbs_underlayer_update_row_profile(&st_tblProfile);
}

int __dbs_underlayer_create_debl_profile(uint16_t id)
{
	st_dbsCnuDefaultProfile st_tblDepro;
	st_dbsProfile st_tblProfile;

	/* 读取tbl_depro 表中的CNU 默认模板*/
	st_tblDepro.id = 1;
	if( SQLITE_OK != __dbs_underlayer_get_row_depro(&st_tblDepro) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_create_debl_profile->__dbs_underlayer_get_row_depro !\n");
		return SQLITE_ERROR;
	}

	/* create_su_profile */
	st_tblProfile.id = id;
	st_tblProfile.col_row_sts = 1;
	st_tblProfile.col_base = st_tblDepro.col_base;
	st_tblProfile.col_macLimit = st_tblDepro.col_macLimit;
	st_tblProfile.col_curate = st_tblDepro.col_curate;
	st_tblProfile.col_cdrate = st_tblDepro.col_cdrate;
	st_tblProfile.col_loagTime = st_tblDepro.col_loagTime;
	st_tblProfile.col_reagTime = st_tblDepro.col_reagTime;
	st_tblProfile.col_igmpPri = st_tblDepro.col_igmpPri;
	st_tblProfile.col_unicastPri = st_tblDepro.col_unicastPri;
	st_tblProfile.col_avsPri = st_tblDepro.col_avsPri;
	st_tblProfile.col_mcastPri = st_tblDepro.col_mcastPri;
	st_tblProfile.col_tbaPriSts = st_tblDepro.col_tbaPriSts;
	st_tblProfile.col_cosPriSts = st_tblDepro.col_cosPriSts;
	st_tblProfile.col_cos0pri = st_tblDepro.col_cos0pri;
	st_tblProfile.col_cos1pri = st_tblDepro.col_cos1pri;
	st_tblProfile.col_cos2pri = st_tblDepro.col_cos2pri;
	st_tblProfile.col_cos3pri = st_tblDepro.col_cos3pri;
	st_tblProfile.col_cos4pri = st_tblDepro.col_cos4pri;
	st_tblProfile.col_cos5pri = st_tblDepro.col_cos5pri;
	st_tblProfile.col_cos6pri = st_tblDepro.col_cos6pri;
	st_tblProfile.col_cos7pri = st_tblDepro.col_cos7pri;
	st_tblProfile.col_tosPriSts = st_tblDepro.col_tosPriSts;
	st_tblProfile.col_tos0pri = st_tblDepro.col_tos0pri;
	st_tblProfile.col_tos1pri = st_tblDepro.col_tos1pri;
	st_tblProfile.col_tos2pri = st_tblDepro.col_tos2pri;
	st_tblProfile.col_tos3pri = st_tblDepro.col_tos3pri;
	st_tblProfile.col_tos4pri = st_tblDepro.col_tos4pri;
	st_tblProfile.col_tos5pri = st_tblDepro.col_tos5pri;
	st_tblProfile.col_tos6pri = st_tblDepro.col_tos6pri;
	st_tblProfile.col_tos7pri = st_tblDepro.col_tos7pri;
	st_tblProfile.col_sfbSts = st_tblDepro.col_sfbSts;
	st_tblProfile.col_sfuSts = st_tblDepro.col_sfuSts;
	st_tblProfile.col_sfmSts = st_tblDepro.col_sfmSts;
	st_tblProfile.col_sfRate = st_tblDepro.col_sfRate;
	st_tblProfile.col_vlanSts = st_tblDepro.col_vlanSts;
	st_tblProfile.col_eth1vid = st_tblDepro.col_eth1vid;
	st_tblProfile.col_eth2vid = st_tblDepro.col_eth2vid;
	st_tblProfile.col_eth3vid = st_tblDepro.col_eth3vid;
	st_tblProfile.col_eth4vid = st_tblDepro.col_eth4vid;
	st_tblProfile.col_portPriSts = st_tblDepro.col_portPriSts;
	st_tblProfile.col_eth1pri = st_tblDepro.col_eth1pri;
	st_tblProfile.col_eth2pri = st_tblDepro.col_eth2pri;
	st_tblProfile.col_eth3pri = st_tblDepro.col_eth3pri;
	st_tblProfile.col_eth4pri = st_tblDepro.col_eth4pri;
	st_tblProfile.col_rxLimitSts = st_tblDepro.col_rxLimitSts;
	st_tblProfile.col_cpuPortRxRate = st_tblDepro.col_cpuPortRxRate;
	st_tblProfile.col_eth1rx = st_tblDepro.col_eth1rx;
	st_tblProfile.col_eth2rx = st_tblDepro.col_eth2rx;
	st_tblProfile.col_eth3rx = st_tblDepro.col_eth3rx;
	st_tblProfile.col_eth4rx = st_tblDepro.col_eth4rx;
	st_tblProfile.col_txLimitSts = st_tblDepro.col_txLimitSts;
	st_tblProfile.col_cpuPortTxRate = st_tblDepro.col_cpuPortTxRate;
	st_tblProfile.col_eth1tx = st_tblDepro.col_eth1tx;
	st_tblProfile.col_eth2tx = st_tblDepro.col_eth2tx;
	st_tblProfile.col_eth3tx = st_tblDepro.col_eth3tx;
	st_tblProfile.col_eth4tx = st_tblDepro.col_eth4tx;
	st_tblProfile.col_psctlSts = 1;
	st_tblProfile.col_cpuPortSts = 0;
	st_tblProfile.col_eth1sts = 1;
	st_tblProfile.col_eth2sts = 1;
	st_tblProfile.col_eth3sts = 1;
	st_tblProfile.col_eth4sts = 1;

	return __dbs_underlayer_update_row_profile(&st_tblProfile);
}

int __dbs_underlayer_create_su2_profile(uint16_t id)
{
	st_dbsCnuDefaultProfile st_tblDepro;
	st_dbsProfile st_tblProfile;

	/* 读取tbl_depro 表中的CNU 默认模板*/
	st_tblDepro.id = 2;
	if( SQLITE_OK != __dbs_underlayer_get_row_depro(&st_tblDepro) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_create_su_profile->__dbs_underlayer_get_row_depro !\n");
		return SQLITE_ERROR;
	}

	/* create_su_profile */
	st_tblProfile.id = id;
	st_tblProfile.col_row_sts = 1;
	st_tblProfile.col_base = st_tblDepro.col_base;
	st_tblProfile.col_macLimit = st_tblDepro.col_macLimit;
	st_tblProfile.col_curate = st_tblDepro.col_curate;
	st_tblProfile.col_cdrate = st_tblDepro.col_cdrate;
	st_tblProfile.col_loagTime = st_tblDepro.col_loagTime;
	st_tblProfile.col_reagTime = st_tblDepro.col_reagTime;
	st_tblProfile.col_igmpPri = st_tblDepro.col_igmpPri;
	st_tblProfile.col_unicastPri = st_tblDepro.col_unicastPri;
	st_tblProfile.col_avsPri = st_tblDepro.col_avsPri;
	st_tblProfile.col_mcastPri = st_tblDepro.col_mcastPri;
	st_tblProfile.col_tbaPriSts = st_tblDepro.col_tbaPriSts;
	st_tblProfile.col_cosPriSts = st_tblDepro.col_cosPriSts;
	st_tblProfile.col_cos0pri = st_tblDepro.col_cos0pri;
	st_tblProfile.col_cos1pri = st_tblDepro.col_cos1pri;
	st_tblProfile.col_cos2pri = st_tblDepro.col_cos2pri;
	st_tblProfile.col_cos3pri = st_tblDepro.col_cos3pri;
	st_tblProfile.col_cos4pri = st_tblDepro.col_cos4pri;
	st_tblProfile.col_cos5pri = st_tblDepro.col_cos5pri;
	st_tblProfile.col_cos6pri = st_tblDepro.col_cos6pri;
	st_tblProfile.col_cos7pri = st_tblDepro.col_cos7pri;
	st_tblProfile.col_tosPriSts = st_tblDepro.col_tosPriSts;
	st_tblProfile.col_tos0pri = st_tblDepro.col_tos0pri;
	st_tblProfile.col_tos1pri = st_tblDepro.col_tos1pri;
	st_tblProfile.col_tos2pri = st_tblDepro.col_tos2pri;
	st_tblProfile.col_tos3pri = st_tblDepro.col_tos3pri;
	st_tblProfile.col_tos4pri = st_tblDepro.col_tos4pri;
	st_tblProfile.col_tos5pri = st_tblDepro.col_tos5pri;
	st_tblProfile.col_tos6pri = st_tblDepro.col_tos6pri;
	st_tblProfile.col_tos7pri = st_tblDepro.col_tos7pri;
	st_tblProfile.col_sfbSts = st_tblDepro.col_sfbSts;
	st_tblProfile.col_sfuSts = st_tblDepro.col_sfuSts;
	st_tblProfile.col_sfmSts = st_tblDepro.col_sfmSts;
	st_tblProfile.col_sfRate = st_tblDepro.col_sfRate;
	st_tblProfile.col_vlanSts = 0;
	st_tblProfile.col_eth1vid = 1;
	st_tblProfile.col_eth2vid = 1;
	st_tblProfile.col_eth3vid = 1;
	st_tblProfile.col_eth4vid = 1;
	st_tblProfile.col_portPriSts = 0;
	st_tblProfile.col_eth1pri = 0;
	st_tblProfile.col_eth2pri = 0;
	st_tblProfile.col_eth3pri = 0;
	st_tblProfile.col_eth4pri = 0;
	st_tblProfile.col_rxLimitSts = 0;
	st_tblProfile.col_cpuPortRxRate = 0;
	st_tblProfile.col_eth1rx = 0;
	st_tblProfile.col_eth2rx = 0;
	st_tblProfile.col_eth3rx = 0;
	st_tblProfile.col_eth4rx = 0;
	st_tblProfile.col_txLimitSts = 0;
	st_tblProfile.col_cpuPortTxRate = 0;
	st_tblProfile.col_eth1tx = 0;
	st_tblProfile.col_eth2tx = 0;
	st_tblProfile.col_eth3tx = 0;
	st_tblProfile.col_eth4tx = 0;
	st_tblProfile.col_psctlSts = 1;
	st_tblProfile.col_cpuPortSts = 1;
	st_tblProfile.col_eth1sts = 1;
	st_tblProfile.col_eth2sts = 1;
	st_tblProfile.col_eth3sts = 1;
	st_tblProfile.col_eth4sts = 1;

	return __dbs_underlayer_update_row_profile(&st_tblProfile);
}

int __dbs_underlayer_create_dewl2_profile(uint16_t id)
{
	st_dbsCnuDefaultProfile st_tblDepro;
	st_dbsProfile st_tblProfile;

	/* 读取tbl_depro 表中的CNU 默认模板*/
	st_tblDepro.id = 2;
	if( SQLITE_OK != __dbs_underlayer_get_row_depro(&st_tblDepro) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_create_dewl_profile->__dbs_underlayer_get_row_depro !\n");
		return SQLITE_ERROR;
	}

	/* create_su_profile */
	st_tblProfile.id = id;
	st_tblProfile.col_row_sts = 1;
	st_tblProfile.col_base = st_tblDepro.col_base;
	st_tblProfile.col_macLimit = st_tblDepro.col_macLimit;
	st_tblProfile.col_curate = st_tblDepro.col_curate;
	st_tblProfile.col_cdrate = st_tblDepro.col_cdrate;
	st_tblProfile.col_loagTime = st_tblDepro.col_loagTime;
	st_tblProfile.col_reagTime = st_tblDepro.col_reagTime;
	st_tblProfile.col_igmpPri = st_tblDepro.col_igmpPri;
	st_tblProfile.col_unicastPri = st_tblDepro.col_unicastPri;
	st_tblProfile.col_avsPri = st_tblDepro.col_avsPri;
	st_tblProfile.col_mcastPri = st_tblDepro.col_mcastPri;
	st_tblProfile.col_tbaPriSts = st_tblDepro.col_tbaPriSts;
	st_tblProfile.col_cosPriSts = st_tblDepro.col_cosPriSts;
	st_tblProfile.col_cos0pri = st_tblDepro.col_cos0pri;
	st_tblProfile.col_cos1pri = st_tblDepro.col_cos1pri;
	st_tblProfile.col_cos2pri = st_tblDepro.col_cos2pri;
	st_tblProfile.col_cos3pri = st_tblDepro.col_cos3pri;
	st_tblProfile.col_cos4pri = st_tblDepro.col_cos4pri;
	st_tblProfile.col_cos5pri = st_tblDepro.col_cos5pri;
	st_tblProfile.col_cos6pri = st_tblDepro.col_cos6pri;
	st_tblProfile.col_cos7pri = st_tblDepro.col_cos7pri;
	st_tblProfile.col_tosPriSts = st_tblDepro.col_tosPriSts;
	st_tblProfile.col_tos0pri = st_tblDepro.col_tos0pri;
	st_tblProfile.col_tos1pri = st_tblDepro.col_tos1pri;
	st_tblProfile.col_tos2pri = st_tblDepro.col_tos2pri;
	st_tblProfile.col_tos3pri = st_tblDepro.col_tos3pri;
	st_tblProfile.col_tos4pri = st_tblDepro.col_tos4pri;
	st_tblProfile.col_tos5pri = st_tblDepro.col_tos5pri;
	st_tblProfile.col_tos6pri = st_tblDepro.col_tos6pri;
	st_tblProfile.col_tos7pri = st_tblDepro.col_tos7pri;
	st_tblProfile.col_sfbSts = st_tblDepro.col_sfbSts;
	st_tblProfile.col_sfuSts = st_tblDepro.col_sfuSts;
	st_tblProfile.col_sfmSts = st_tblDepro.col_sfmSts;
	st_tblProfile.col_sfRate = st_tblDepro.col_sfRate;
	st_tblProfile.col_vlanSts = st_tblDepro.col_vlanSts;
	st_tblProfile.col_eth1vid = st_tblDepro.col_eth1vid;
	st_tblProfile.col_eth2vid = st_tblDepro.col_eth2vid;
	st_tblProfile.col_eth3vid = st_tblDepro.col_eth3vid;
	st_tblProfile.col_eth4vid = st_tblDepro.col_eth4vid;
	st_tblProfile.col_portPriSts = st_tblDepro.col_portPriSts;
	st_tblProfile.col_eth1pri = st_tblDepro.col_eth1pri;
	st_tblProfile.col_eth2pri = st_tblDepro.col_eth2pri;
	st_tblProfile.col_eth3pri = st_tblDepro.col_eth3pri;
	st_tblProfile.col_eth4pri = st_tblDepro.col_eth4pri;
	st_tblProfile.col_rxLimitSts = st_tblDepro.col_rxLimitSts;
	st_tblProfile.col_cpuPortRxRate = st_tblDepro.col_cpuPortRxRate;
	st_tblProfile.col_eth1rx = st_tblDepro.col_eth1rx;
	st_tblProfile.col_eth2rx = st_tblDepro.col_eth2rx;
	st_tblProfile.col_eth3rx = st_tblDepro.col_eth3rx;
	st_tblProfile.col_eth4rx = st_tblDepro.col_eth4rx;
	st_tblProfile.col_txLimitSts = st_tblDepro.col_txLimitSts;
	st_tblProfile.col_cpuPortTxRate = st_tblDepro.col_cpuPortTxRate;
	st_tblProfile.col_eth1tx = st_tblDepro.col_eth1tx;
	st_tblProfile.col_eth2tx = st_tblDepro.col_eth2tx;
	st_tblProfile.col_eth3tx = st_tblDepro.col_eth3tx;
	st_tblProfile.col_eth4tx = st_tblDepro.col_eth4tx;
	st_tblProfile.col_psctlSts = 1;
	st_tblProfile.col_cpuPortSts = 1;
	st_tblProfile.col_eth1sts = 1;
	st_tblProfile.col_eth2sts = 1;
	st_tblProfile.col_eth3sts = 1;
	st_tblProfile.col_eth4sts = 1;

	return __dbs_underlayer_update_row_profile(&st_tblProfile);
}

int __dbs_underlayer_create_debl2_profile(uint16_t id)
{
	st_dbsCnuDefaultProfile st_tblDepro;
	st_dbsProfile st_tblProfile;

	/* 读取tbl_depro 表中的CNU 默认模板*/
	st_tblDepro.id = 2;
	if( SQLITE_OK != __dbs_underlayer_get_row_depro(&st_tblDepro) )
	{
		fprintf(stderr, "ERROR: __dbs_underlayer_create_debl_profile->__dbs_underlayer_get_row_depro !\n");
		return SQLITE_ERROR;
	}

	/* create_su_profile */
	st_tblProfile.id = id;
	st_tblProfile.col_row_sts = 1;
	st_tblProfile.col_base = st_tblDepro.col_base;
	st_tblProfile.col_macLimit = st_tblDepro.col_macLimit;
	st_tblProfile.col_curate = st_tblDepro.col_curate;
	st_tblProfile.col_cdrate = st_tblDepro.col_cdrate;
	st_tblProfile.col_loagTime = st_tblDepro.col_loagTime;
	st_tblProfile.col_reagTime = st_tblDepro.col_reagTime;
	st_tblProfile.col_igmpPri = st_tblDepro.col_igmpPri;
	st_tblProfile.col_unicastPri = st_tblDepro.col_unicastPri;
	st_tblProfile.col_avsPri = st_tblDepro.col_avsPri;
	st_tblProfile.col_mcastPri = st_tblDepro.col_mcastPri;
	st_tblProfile.col_tbaPriSts = st_tblDepro.col_tbaPriSts;
	st_tblProfile.col_cosPriSts = st_tblDepro.col_cosPriSts;
	st_tblProfile.col_cos0pri = st_tblDepro.col_cos0pri;
	st_tblProfile.col_cos1pri = st_tblDepro.col_cos1pri;
	st_tblProfile.col_cos2pri = st_tblDepro.col_cos2pri;
	st_tblProfile.col_cos3pri = st_tblDepro.col_cos3pri;
	st_tblProfile.col_cos4pri = st_tblDepro.col_cos4pri;
	st_tblProfile.col_cos5pri = st_tblDepro.col_cos5pri;
	st_tblProfile.col_cos6pri = st_tblDepro.col_cos6pri;
	st_tblProfile.col_cos7pri = st_tblDepro.col_cos7pri;
	st_tblProfile.col_tosPriSts = st_tblDepro.col_tosPriSts;
	st_tblProfile.col_tos0pri = st_tblDepro.col_tos0pri;
	st_tblProfile.col_tos1pri = st_tblDepro.col_tos1pri;
	st_tblProfile.col_tos2pri = st_tblDepro.col_tos2pri;
	st_tblProfile.col_tos3pri = st_tblDepro.col_tos3pri;
	st_tblProfile.col_tos4pri = st_tblDepro.col_tos4pri;
	st_tblProfile.col_tos5pri = st_tblDepro.col_tos5pri;
	st_tblProfile.col_tos6pri = st_tblDepro.col_tos6pri;
	st_tblProfile.col_tos7pri = st_tblDepro.col_tos7pri;
	st_tblProfile.col_sfbSts = st_tblDepro.col_sfbSts;
	st_tblProfile.col_sfuSts = st_tblDepro.col_sfuSts;
	st_tblProfile.col_sfmSts = st_tblDepro.col_sfmSts;
	st_tblProfile.col_sfRate = st_tblDepro.col_sfRate;
	st_tblProfile.col_vlanSts = st_tblDepro.col_vlanSts;
	st_tblProfile.col_eth1vid = st_tblDepro.col_eth1vid;
	st_tblProfile.col_eth2vid = st_tblDepro.col_eth2vid;
	st_tblProfile.col_eth3vid = st_tblDepro.col_eth3vid;
	st_tblProfile.col_eth4vid = st_tblDepro.col_eth4vid;
	st_tblProfile.col_portPriSts = st_tblDepro.col_portPriSts;
	st_tblProfile.col_eth1pri = st_tblDepro.col_eth1pri;
	st_tblProfile.col_eth2pri = st_tblDepro.col_eth2pri;
	st_tblProfile.col_eth3pri = st_tblDepro.col_eth3pri;
	st_tblProfile.col_eth4pri = st_tblDepro.col_eth4pri;
	st_tblProfile.col_rxLimitSts = st_tblDepro.col_rxLimitSts;
	st_tblProfile.col_cpuPortRxRate = st_tblDepro.col_cpuPortRxRate;
	st_tblProfile.col_eth1rx = st_tblDepro.col_eth1rx;
	st_tblProfile.col_eth2rx = st_tblDepro.col_eth2rx;
	st_tblProfile.col_eth3rx = st_tblDepro.col_eth3rx;
	st_tblProfile.col_eth4rx = st_tblDepro.col_eth4rx;
	st_tblProfile.col_txLimitSts = st_tblDepro.col_txLimitSts;
	st_tblProfile.col_cpuPortTxRate = st_tblDepro.col_cpuPortTxRate;
	st_tblProfile.col_eth1tx = st_tblDepro.col_eth1tx;
	st_tblProfile.col_eth2tx = st_tblDepro.col_eth2tx;
	st_tblProfile.col_eth3tx = st_tblDepro.col_eth3tx;
	st_tblProfile.col_eth4tx = st_tblDepro.col_eth4tx;
	st_tblProfile.col_psctlSts = 1;
	st_tblProfile.col_cpuPortSts = 0;
	st_tblProfile.col_eth1sts = 1;
	st_tblProfile.col_eth2sts = 1;
	st_tblProfile.col_eth3sts = 1;
	st_tblProfile.col_eth4sts = 1;

	return __dbs_underlayer_update_row_profile(&st_tblProfile);
}


int dbs_underlayer_get_column(DB_COL_VAR *v)
{
	int ret = SQLITE_ERROR;
	
	assert( NULL != v );
	
	switch(v->ci.colType)
	{
		case SQLITE_INTEGER:
		{
			ret = __dbs_underlayer_get_integer(v);
			break;
		}
		case SQLITE3_TEXT:
		{
			ret = __dbs_underlayer_get_text(v);
			break;
		}
		default:
		{
			v->ci.colType = SQLITE_NULL;
			v->len = 0;
			break;
		}
	}
	return ret;
}

int dbs_underlayer_update_column(DB_COL_VAR *v)
{
	int ret = SQLITE_ERROR;
	
	assert( NULL != v );
	
	switch(v->ci.colType)
	{
		case SQLITE_INTEGER:
		{
			ret = __dbs_underlayer_update_integer(v);
			break;
		}
		case SQLITE3_TEXT:
		{
			ret = __dbs_underlayer_update_text(v);
			break;
		}
		case SQLITE_NULL:
		{
			ret = __dbs_underlayer_clear_column(&(v->ci));
			break;
		}
		default:
		{
			break;
		}
	}
	return ret;
}

int dbs_underlayer_get_row_clirole(st_dbsCliRole *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_get_row_clirole(row);
}

int dbs_underlayer_get_row_clt(st_dbsClt *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_get_row_clt(row);
}

int dbs_underlayer_get_row_cltconfig(st_dbsCltConf *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_get_row_cltconfig(row);
}

int dbs_underlayer_get_row_cnu(st_dbsCnu *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_get_row_cnu(row);
}

int dbs_underlayer_get_row_depro(st_dbsCnuDefaultProfile *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_get_row_depro(row);
}


int dbs_underlayer_get_row_network(st_dbsNetwork *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_get_row_network(row);
}

int dbs_underlayer_get_row_profile(st_dbsProfile *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_get_row_profile(row);
}

int dbs_underlayer_get_row_snmp(st_dbsSnmp *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_get_row_snmp(row);
}

int dbs_underlayer_get_row_swmgmt(st_dbsSwmgmt *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_get_row_swmgmt(row);
}

int dbs_underlayer_get_row_sysinfo(st_dbsSysinfo *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_get_row_sysinfo(row);
}

int dbs_underlayer_get_row_template(st_dbsTemplate *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_get_row_template(row);
}

int dbs_underlayer_select_cnu_index_by_mac(char *mac, stCnuNode *index)
{
	assert( NULL != mac );
	assert( NULL != index );
	return __dbs_underlayer_select_cnu_index_by_mac(mac, index);
}

int dbs_underlayer_update_row_clirole(st_dbsCliRole *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_update_row_clirole(row);
}

int dbs_underlayer_update_row_clt(st_dbsClt *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_update_row_clt(row);
}

int dbs_underlayer_update_row_cltconfig(st_dbsCltConf *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_update_row_cltconf(row);
}

int dbs_underlayer_update_row_cnu(st_dbsCnu *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_update_row_cnu(row);
}

int dbs_underlayer_update_row_depro(st_dbsCnuDefaultProfile *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_update_row_depro(row);
}

int dbs_underlayer_update_row_network(st_dbsNetwork *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_update_row_network(row);
}

int dbs_underlayer_update_row_profile(st_dbsProfile *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_update_row_profile(row);
}

int dbs_underlayer_update_row_snmp(st_dbsSnmp *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_update_row_snmp(row);
}

int dbs_underlayer_update_row_swmgmt(st_dbsSwmgmt *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_update_row_swmgmt(row);
}

int dbs_underlayer_update_row_sysinfo(st_dbsSysinfo *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_update_row_sysinfo(row);
}

int dbs_underlayer_update_row_template(st_dbsTemplate *row)
{
	assert( NULL != row );	
	return __dbs_underlayer_update_row_template(row);
}

int dbs_underlayer_destory_row_clt(uint16_t id)
{
	return __dbs_underlayer_destory_row_clt(id);
}

int dbs_underlayer_destory_row_cltconf(uint16_t id)
{
	return __dbs_underlayer_destory_row_cltconf(id);
}

int dbs_underlayer_destory_row_cnu(uint16_t id)
{
	return __dbs_underlayer_destory_row_cnu(id);
}

int dbs_underlayer_destory_row_profile(uint16_t id)
{
	return __dbs_underlayer_destory_row_profile(id);
}

int dbs_underlayer_create_su_profile(uint16_t id)
{
	return __dbs_underlayer_create_su_profile(id);
}

int dbs_underlayer_create_dewl_profile(uint16_t id)
{
	return __dbs_underlayer_create_dewl_profile(id);
}

int dbs_underlayer_create_debl_profile(uint16_t id)
{
	return __dbs_underlayer_create_debl_profile(id);
}

int dbs_underlayer_create_su2_profile(uint16_t id)
{
	return __dbs_underlayer_create_su2_profile(id);
}

int dbs_underlayer_create_dewl2_profile(uint16_t id)
{
	return __dbs_underlayer_create_dewl2_profile(id);
}

int dbs_underlayer_create_debl2_profile(uint16_t id)
{
	return __dbs_underlayer_create_debl2_profile(id);
}

int dbs_underlayer_debug_enable(int enable)
{
	DBS_UNDERLAYER_DEBUG_ENABLE = (enable?1:0);
	return 0;
}

int dbs_underlayer_fflush(void)
{
	int rc;                   		/* Function return code */
	uint8_t *zFilename = DBS_SYS_DATABASE_FFS;
	sqlite3 *pFile;           		/* Database connection opened on zFilename */
	sqlite3_backup *pBackup;	/* Backup object used to copy data */
	sqlite3 *pTo;             		/* Database to copy to (pFile or pInMemory) */
	sqlite3 *pFrom;           		/* Database to copy from (pFile or pInMemory) */
	
	/* Open the database file identified by zFilename. Exit early if this fails for any reason. */
	rc = sqlite3_open(zFilename, &pFile);
  	if( rc == SQLITE_OK )
	{
		/* If this is a 'load' operation (isSave==0), then data is copied
		** from the database file just opened to database pInMemory. 
		** Otherwise, if this is a 'save' operation (isSave==1), then data
		** is copied from pInMemory to pFile.  Set the variables pFrom and
		** pTo accordingly. */
		pFrom = dbsSystemFileHandle;
		pTo = pFile;

		/* Set up the backup procedure to copy from the "main" database of 
		** connection pFile to the main database of connection pInMemory.
		** If something goes wrong, pBackup will be set to NULL and an error
		** code and  message left in connection pTo.
		**
		** If the backup object is successfully created, call backup_step()
		** to copy data from pFile to pInMemory. Then call backup_finish()
		** to release resources associated with the pBackup object.  If an
		** error occurred, then  an error code and message will be left in
		** connection pTo. If no error occurred, then the error code belonging
		** to pTo is set to SQLITE_OK.
		*/
		pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
		if( pBackup )
		{
			(void)sqlite3_backup_step(pBackup, -1);
			(void)sqlite3_backup_finish(pBackup);
    		}
		
		rc = sqlite3_errcode(pTo);
		/* Close the database connection opened on database file zFilename
		** and return the result of this function. */
		(void)sqlite3_close(pFile);
	}
	
	return rc;
}

int dbs_underlayer_open(void)
{
	/* 打开数据库*/
	if( sqlite3_open(DBS_SYS_DATABASE, &dbsSystemFileHandle) != SQLITE_OK )
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(dbsSystemFileHandle));
		return SQLITE_ERROR;
	}
	return SQLITE_OK;
}

int dbs_underlayer_close(void)
{
	return sqlite3_close(dbsSystemFileHandle);
}


