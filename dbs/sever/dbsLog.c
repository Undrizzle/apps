#include <sqlite3.h>
#include <assert.h>

#include <dbsLog.h>

#define DBS_ALARM_MAX_ROW		512		/*告警日志最大存储数量*/
#define DBS_OPTLOG_MAX_ROW		512		/*操作日志最大存储数量*/
#define DBS_SYSLOG_MAX_ROW		1024	/*系统日志最大存储数量*/

uint8_t gBuf_dbsLog[DBS_SQL_MEDIUM_LEN] = {0};

/* sql 语句的调试开关 */
static int DBS_LOG_DEBUG_ENABLE = 0;

/* 数据库操作句柄，该句柄在DBS模块初始化时打开，
** 直到进程结束才关闭*/
sqlite3 *dbsLogFileHandle;

/* 描述数据库中数据表名称的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_DB_DESC db_log[DBS_LOG_TBL_TOTAL_NUM] = 
{
	{DBS_LOG_TBL_ID_ALARM, 	"tbl_alarm",	DBS_LOG_TBL_COLS_ALARM},
	{DBS_LOG_TBL_ID_OPT,	"tbl_optlog",	DBS_LOG_TBL_COLS_OPT},
	{DBS_LOG_TBL_ID_SYS,	"tbl_syslog",	DBS_LOG_TBL_COLS_SYSTEM},
	{DBS_LOG_TBL_ID_MS,	"tbl_mslog",	DBS_LOG_TBL_COLS_MS}
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_alarmlog[DBS_LOG_TBL_COLS_ALARM] = 
{
	/*	类型，	长度，	主键，允许空，	列名*/
	{DBS_LOG_TBL_ALARM_COL_ID_SFLOW,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_serial"},
	{DBS_LOG_TBL_ALARM_COL_ID_TINFO,	SQLITE3_TEXT,	64,	BOOL_FALSE,	BOOL_TRUE,	"col_trapInfo"},
	{DBS_LOG_TBL_ALARM_COL_ID_TOID,	SQLITE3_TEXT,	64,	BOOL_FALSE,	BOOL_TRUE,	"col_trapOid"},
	{DBS_LOG_TBL_ALARM_COL_ID_ACODE,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_alarmCode"},
	{DBS_LOG_TBL_ALARM_COL_ID_ATIME,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_time"},
	{DBS_LOG_TBL_ALARM_COL_ID_INUM,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_itermNum"},
	{DBS_LOG_TBL_ALARM_COL_ID_CMAC,	SQLITE3_TEXT,	32,	BOOL_FALSE,	BOOL_TRUE,	"col_cbatMac"},
	{DBS_LOG_TBL_ALARM_COL_ID_CLT,		SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cltid"},
	{DBS_LOG_TBL_ALARM_COL_ID_CNU,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_cnuid"},
	{DBS_LOG_TBL_ALARM_COL_ID_ATYPE,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_alarmType"},
	{DBS_LOG_TBL_ALARM_COL_ID_AVAL,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_alarmValue"}
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_optlog[DBS_LOG_TBL_COLS_OPT] = 
{
	/*								类型，		长度，主键，允许空，列名*/
	{DBS_LOG_TBL_OPT_COL_ID_TIME,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_time"},
	{DBS_LOG_TBL_OPT_COL_ID_WHO,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_who"},
	{DBS_LOG_TBL_OPT_COL_ID_CMD,	SQLITE3_TEXT,	128,	BOOL_FALSE,	BOOL_TRUE,	"col_cmd"},
	{DBS_LOG_TBL_OPT_COL_ID_LEVEL,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_level"},
	{DBS_LOG_TBL_OPT_COL_ID_RET,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_result"}
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_syslog[DBS_LOG_TBL_COLS_SYSTEM] = 
{
	/*								类型，		长度，主键，允许空，列名*/
	{DBS_LOG_TBL_SYS_COL_ID_TIME,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_time"},
	{DBS_LOG_TBL_SYS_COL_ID_MOD,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_mod"},
	{DBS_LOG_TBL_SYS_COL_ID_LEVEL,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_level"},
	{DBS_LOG_TBL_SYS_COL_ID_INFO,	SQLITE3_TEXT,	256,	BOOL_FALSE,	BOOL_TRUE,	"col_info"}
};

/* 描述数据表设计的结构体，实际数据库设计
** 时需要与该表描述一致*/
DBS_TBL_DESIGN tbl_mslog[DBS_LOG_TBL_COLS_MS] = 
{
	/*							类型，	长度，主键，允许空，列名*/
	{DBS_LOG_TBL_MS_COL_ID_ID,	SQLITE_INTEGER,	4,	BOOL_TRUE,	BOOL_FALSE,	"id"},
	{DBS_LOG_TBL_MS_COL_ID_MS,	SQLITE_INTEGER,	4,	BOOL_FALSE,	BOOL_TRUE,	"col_ms"}
};

#define __dbsLogSQL(...);	\
{	\
	if( DBS_LOG_DEBUG_ENABLE ) \
	{	\
		fprintf(stderr, __VA_ARGS__);	\
		fprintf(stderr, ";\n");	\
	} \
}

int __dbs_log_sql_exec(uint8_t *sql)
{
	int ret = SQLITE_ERROR;
	char *err_msg = NULL;

	assert( NULL != sql );
	
	do
	{
		ret = sqlite3_exec(dbsLogFileHandle, sql, 0, 0, &err_msg);	
		if(ret == SQLITE_BUSY)
		{
			usleep(10000);
			continue;
		}
		else if( ret != SQLITE_OK )
		{
			fprintf(stderr, "ERROR: [dbs_log_sql_exec] [%s] [%s]\n", sql, err_msg);
			return SQLITE_ERROR;
		}
	}while( SQLITE_BUSY == ret );
	
	return ret;
}


int __dbs_log_get_count(uint16_t tbl, uint32_t *count)
{
	int ret = SQLITE_ERROR;
	
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsLog;
	bzero(sql, DBS_SQL_MEDIUM_LEN);
	
	sqlite3_stmt * stmt = NULL;
	const char *zErrMsg = NULL;
	
	assert( NULL != count );	
	
	/* 判断表是否存在*/
	if( tbl >= DBS_LOG_TBL_TOTAL_NUM )
	{
		fprintf(stderr, "ERROR: dbs_log_get_count() DBS_LOG_TBL_TOTAL_NUM !\n");
		return SQLITE_ERROR;
	}
	
	/* 组装SQL语句*/
	sprintf(sql, "SELECT COUNT(*) FROM [%s]", db_log[tbl].tbl_name);
	__dbsLogSQL(sql);

	do
	{
		ret = sqlite3_prepare(dbsLogFileHandle, sql, -1, &stmt, &zErrMsg);
		if( SQLITE_BUSY == ret )
		{
			usleep(10000);
			continue;
		}
		else if( SQLITE_OK != ret )
		{
			fprintf(stderr, "ERROR: dbs_log_get_count()->sqlite3_prepare() !\n");
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
				fprintf(stderr, "ERROR: dbs_log_get_count()->sqlite3_step() !\n");
				ret = SQLITE_ERROR;
				break;
			}
		}
	}while( SQLITE_BUSY == ret );

	sqlite3_finalize(stmt);
	return ret;
}

int __dbs_delete_alarmlog(void)
{
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsLog;
	bzero(sql, DBS_SQL_MEDIUM_LEN);

	/* 组装SQL语句*/
	sprintf(sql, "DELETE FROM [%s]", db_log[DBS_LOG_TBL_ID_ALARM].tbl_name);

	/* 调试打印SQL 语句 */
	__dbsLogSQL(sql);

	return __dbs_log_sql_exec(sql);	
}

int __dbs_persist_alarmlog(st_dbsAlarmlog *v)
{
	//char sql[DBS_SQL_MEDIUM_LEN] = {0};
	uint8_t *sql = gBuf_dbsLog;
	bzero(sql, DBS_SQL_MEDIUM_LEN);

	assert( NULL != v );

	/* 组装SQL 语句*/
	sprintf(sql, "INSERT INTO [tbl_alarm] VALUES (%d,'%s','%s',%d,%d,%d,'%s',%d,%d,%d,%d)",
		v->serialFlow, v->trap_info, v->oid, v->alarmCode, v->realTime,
		v->itemNum, v->cbatMac, v->cltId, v->cnuId, v->alarmType, v->alarmValue);	
	
	/* 调试打印SQL 语句 */
	__dbsLogSQL(sql);

	return __dbs_log_sql_exec(sql);	
}

int __dbs_delete_optlog(void)
{
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsLog;
	bzero(sql, DBS_SQL_MEDIUM_LEN);

	/* 组装SQL语句*/
	sprintf(sql, "DELETE FROM [%s]", db_log[DBS_LOG_TBL_ID_OPT].tbl_name);

	/* 调试打印SQL 语句 */
	__dbsLogSQL(sql);

	return __dbs_log_sql_exec(sql);	
}

int __dbs_persist_optlog(st_dbsOptlog *v)
{
	//uint8_t sql[DBS_SQL_MEDIUM_LEN] = {0};
	uint8_t *sql = gBuf_dbsLog;
	bzero(sql, DBS_SQL_MEDIUM_LEN);

	assert( NULL != v );

	/* 组装SQL 语句*/	
	sprintf(sql, "INSERT INTO [tbl_optlog] VALUES ('%d','%d','%s','%d','%d')", v->time, v->who, v->cmd, v->level, v->result);
	
	/* 调试打印SQL 语句 */
	__dbsLogSQL(sql);

	return __dbs_log_sql_exec(sql);	
}

int __dbs_delete_syslog(void)
{
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsLog;
	bzero(sql, DBS_SQL_MEDIUM_LEN);

	/* 组装SQL语句*/
	sprintf(sql, "DELETE FROM [%s]", db_log[DBS_LOG_TBL_ID_SYS].tbl_name);

	/* 调试打印SQL 语句 */
	__dbsLogSQL(sql);

	return __dbs_log_sql_exec(sql);	
}

int __dbs_persist_syslog(st_dbsSyslog *v)
{
	//uint8_t sql[DBS_SQL_MEDIUM_LEN] = {0};
	uint8_t *sql = gBuf_dbsLog;
	bzero(sql, DBS_SQL_MEDIUM_LEN);

	assert( NULL != v );

	/* 组装SQL 语句*/
	sprintf(sql, "INSERT INTO [tbl_syslog] VALUES ('%d','%d','%d','%s')", v->time, v->who, v->level, v->log);
	
	/* 调试打印SQL 语句 */
	__dbsLogSQL(sql);

	return __dbs_log_sql_exec(sql);	
}


/*************************************************************************/
/* ***************** 定义模块外部接口函数**********************/
/*************************************************************************/
int dbs_count_alarmlog(uint32_t *n)
{
	uint32_t iCount = 0;
	
	assert( NULL != n );

	*n = 0;

	/* 获取当前数据表的条目数量*/
	if( SQLITE_OK != __dbs_log_get_count(DBS_LOG_TBL_ID_ALARM, &iCount) )
	{
		return SQLITE_ERROR;
	}
	else
	{
		*n = iCount;
		return SQLITE_OK;
	}
}

int dbs_count_optlog(uint32_t *n)
{
	uint32_t iCount = 0;
	
	assert( NULL != n );

	*n = 0;

	/* 获取当前数据表的条目数量*/
	if( SQLITE_OK != __dbs_log_get_count(DBS_LOG_TBL_ID_OPT, &iCount) )
	{
		return SQLITE_ERROR;
	}
	else
	{
		*n = iCount;
		return SQLITE_OK;
	}
}

int dbs_count_syslog(uint32_t *n)
{
	uint32_t iCount = 0;
	
	assert( NULL != n );

	*n = 0;

	/* 获取当前数据表的条目数量*/
	if( SQLITE_OK != __dbs_log_get_count(DBS_LOG_TBL_ID_SYS, &iCount) )
	{
		return SQLITE_ERROR;
	}
	else
	{
		*n = iCount;
		return SQLITE_OK;
	}
}

int dbs_get_alarmlog(uint32_t row, st_dbsAlarmlog *v)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsLog;
	bzero(sql, DBS_SQL_MEDIUM_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != v );

	if( row < 1 )
	{
		fprintf(stderr, "ERROR: dbs_get_alarmlog row<1 !\n");
		return SQLITE_ERROR;
	}
	if( row > DBS_ALARM_MAX_ROW )
	{
		fprintf(stderr, "ERROR: dbs_get_alarmlog row>512 !\n");
		return SQLITE_ERROR;
	}

	/* 组装SQL语句*/
	sprintf(sql, "SELECT * FROM [tbl_alarm] LIMIT %d,1", row-1);
	__dbsLogSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsLogFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: dbs_get_alarmlog->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if( SQLITE_ROW == sqlite3_step(stmt) )
	{
		/* col_serial */		/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_ALARM_COL_ID_SFLOW) )
		{
			v->serialFlow = sqlite3_column_int(stmt, DBS_LOG_TBL_ALARM_COL_ID_SFLOW);
		}
		else
		{
			v->serialFlow = 0;
		}
		/* col_trapInfo */		/* TEXT */
		if( SQLITE3_TEXT == sqlite3_column_type(stmt, DBS_LOG_TBL_ALARM_COL_ID_TINFO) )
		{
			strcpy(v->trap_info, sqlite3_column_text(stmt, DBS_LOG_TBL_ALARM_COL_ID_TINFO));
		}
		else
		{
			v->trap_info[0] = '\0';
		}
		/* col_trapOid */		/* TEXT */
		if( SQLITE3_TEXT == sqlite3_column_type(stmt, DBS_LOG_TBL_ALARM_COL_ID_TOID) )
		{
			strcpy(v->oid, sqlite3_column_text(stmt, DBS_LOG_TBL_ALARM_COL_ID_TOID));
		}
		else
		{
			v->oid[0] = '\0';
		}
		/* col_alarmCode */	/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_ALARM_COL_ID_ACODE) )
		{
			v->alarmCode = sqlite3_column_int(stmt, DBS_LOG_TBL_ALARM_COL_ID_ACODE);
		}
		else
		{
			v->alarmCode = 0;
		}
		/* col_time */		/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_ALARM_COL_ID_ATIME) )
		{
			v->realTime = sqlite3_column_int(stmt, DBS_LOG_TBL_ALARM_COL_ID_ATIME);
		}
		else
		{
			v->realTime = 0;
		}
		/* col_itermNum */	/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_ALARM_COL_ID_INUM) )
		{
			v->itemNum = sqlite3_column_int(stmt, DBS_LOG_TBL_ALARM_COL_ID_INUM);
		}
		else
		{
			v->itemNum = 0;
		}
		/* col_cbatMac */		/* TEXT */
		if( SQLITE3_TEXT == sqlite3_column_type(stmt, DBS_LOG_TBL_ALARM_COL_ID_CMAC) )
		{
			strcpy(v->cbatMac, sqlite3_column_text(stmt, DBS_LOG_TBL_ALARM_COL_ID_CMAC));
		}
		else
		{
			v->cbatMac[0] = '\0';
		}
		/* col_cltid */			/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_ALARM_COL_ID_CLT) )
		{
			v->cltId = sqlite3_column_int(stmt, DBS_LOG_TBL_ALARM_COL_ID_CLT);
		}
		else
		{
			v->cltId = 0;
		}
		/* col_cnuid */		/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_ALARM_COL_ID_CNU) )
		{
			v->cnuId = sqlite3_column_int(stmt, DBS_LOG_TBL_ALARM_COL_ID_CNU);
		}
		else
		{
			v->cnuId = 0;
		}
		/* col_alarmType */	/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_ALARM_COL_ID_ATYPE) )
		{
			v->alarmType = sqlite3_column_int(stmt, DBS_LOG_TBL_ALARM_COL_ID_ATYPE);
		}
		else
		{
			v->alarmType = 0;
		}
		/* col_alarmValue */	/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_ALARM_COL_ID_AVAL) )
		{
			v->alarmValue = sqlite3_column_int(stmt, DBS_LOG_TBL_ALARM_COL_ID_AVAL);
		}
		else
		{
			v->alarmValue = 0;
		}		
		ret = SQLITE_OK;
	}
	else
	{
		//fprintf(stderr, "WARNNING: dbs_get_alarmlog->sqlite3_step !\n");
		ret = SQLITE_ERROR;
	}

	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 
	return ret;
}

int dbs_get_optlog(uint32_t row, st_dbsOptlog *v)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsLog;
	bzero(sql, DBS_SQL_MEDIUM_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != v );

	if( row < 1 )
	{
		fprintf(stderr, "ERROR: dbs_get_optlog row<1 !\n");
		return SQLITE_ERROR;
	}
	if( row > DBS_OPTLOG_MAX_ROW )
	{
		fprintf(stderr, "ERROR: dbs_get_optlog row>512 !\n");
		return SQLITE_ERROR;
	}

	/* 组装SQL语句*/
	sprintf(sql, "SELECT * FROM [tbl_optlog] LIMIT %d,1", row-1);
	__dbsLogSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsLogFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: dbs_get_optlog->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if( SQLITE_ROW == sqlite3_step(stmt) )
	{
		/* col_time */		/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_OPT_COL_ID_TIME) )
		{
			v->time = sqlite3_column_int(stmt, DBS_LOG_TBL_OPT_COL_ID_TIME);
		}
		else
		{
			v->time = 0;
		}
		/* col_who */			/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_OPT_COL_ID_WHO) )
		{
			v->who = sqlite3_column_int(stmt, DBS_LOG_TBL_OPT_COL_ID_WHO);
		}
		else
		{
			v->who = MID_OTHER;
		}
		/* col_cmd */			/* TEXT */
		if( SQLITE3_TEXT == sqlite3_column_type(stmt, DBS_LOG_TBL_OPT_COL_ID_CMD) )
		{
			strcpy(v->cmd, sqlite3_column_text(stmt, DBS_LOG_TBL_OPT_COL_ID_CMD));
		}
		else
		{
			v->cmd[0] = '\0';
		}
		/* col_level */		/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_OPT_COL_ID_LEVEL) )
		{
			v->level = sqlite3_column_int(stmt, DBS_LOG_TBL_OPT_COL_ID_LEVEL);
		}
		else
		{
			v->level = DBS_LOG_EMERG;
		}
		/* col_result */		/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_OPT_COL_ID_RET) )
		{
			v->result = sqlite3_column_int(stmt, DBS_LOG_TBL_OPT_COL_ID_RET);
		}
		else
		{
			v->result = CMM_FAILED;
		}
		
		ret = SQLITE_OK;
	}
	else
	{
		//fprintf(stderr, "WARNNING: dbs_get_optlog->sqlite3_step !\n");
		ret = SQLITE_ERROR;
	}

	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 
	return ret;
}

int dbs_get_syslog(uint32_t row, st_dbsSyslog *v)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsLog;
	bzero(sql, DBS_SQL_MEDIUM_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != v );

	if( row < 1 )
	{
		fprintf(stderr, "ERROR: dbs_get_syslog row<1 !\n");
		return SQLITE_ERROR;
	}
	if( row > DBS_SYSLOG_MAX_ROW )
	{
		fprintf(stderr, "ERROR: dbs_get_syslog row>1024 !\n");
		return SQLITE_ERROR;
	}

	/* 组装SQL语句*/
	sprintf(sql, "SELECT * FROM [tbl_syslog] LIMIT %d,1", row-1);
	__dbsLogSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsLogFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: dbs_get_syslog->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if( SQLITE_ROW == sqlite3_step(stmt) )
	{
		/* col_time */		/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_SYS_COL_ID_TIME) )
		{
			v->time = sqlite3_column_int(stmt, DBS_LOG_TBL_SYS_COL_ID_TIME);
		}
		else
		{
			v->time = 0;
		}
		/* col_mod */		/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_SYS_COL_ID_MOD) )
		{
			v->who = sqlite3_column_int(stmt, DBS_LOG_TBL_SYS_COL_ID_MOD);
		}
		else
		{
			v->who = MID_OTHER;
		}
		/* col_level */		/* INTEGER */
		if( SQLITE_INTEGER == sqlite3_column_type(stmt, DBS_LOG_TBL_SYS_COL_ID_LEVEL) )
		{
			v->level = sqlite3_column_int(stmt, DBS_LOG_TBL_SYS_COL_ID_LEVEL);
		}
		else
		{
			v->level = DBS_LOG_EMERG;
		}
		/* col_info */			/* TEXT */
		if( SQLITE3_TEXT == sqlite3_column_type(stmt, DBS_LOG_TBL_SYS_COL_ID_INFO) )
		{
			strcpy(v->log, sqlite3_column_text(stmt, DBS_LOG_TBL_SYS_COL_ID_INFO));
		}
		else
		{
			v->log[0] = '\0';
		}
		
		ret = SQLITE_OK;
	}
	else
	{
		//fprintf(stderr, "WARNNING: dbs_get_syslog->sqlite3_step !\n");
		ret = SQLITE_ERROR;
	}

	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 
	return ret;
}

int dbs_get_mslog(st_dbsMslog *ms)
{
	int ret = SQLITE_ERROR;
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsLog;
	bzero(sql, DBS_SQL_MEDIUM_LEN);
	sqlite3_stmt *stmt = NULL;
	const char *err_msg = NULL;
	
	assert( NULL != ms );

	ms->status = 0;

	/* 组装SQL语句*/
	sprintf(sql, "SELECT [%s] FROM [%s] WHERE [id]=%d", 
				tbl_mslog[DBS_LOG_TBL_MS_COL_ID_MS].col_name, 
				db_log[DBS_LOG_TBL_ID_MS].tbl_name, 
				ms->mid);	
	__dbsLogSQL(sql);

	if( SQLITE_OK != sqlite3_prepare(dbsLogFileHandle, sql, -1, &stmt, &err_msg))
	{
		fprintf(stderr, "ERROR: dbs_get_mslog->sqlite3_prepare !\n");
		ret = SQLITE_ERROR;
	}
	else if(SQLITE_ROW == sqlite3_step(stmt))
	{
		/* 结果集中列的数量*/
		if( 1 != sqlite3_column_count(stmt) )
		{
			fprintf(stderr, "ERROR: dbs_get_mslog->sqlite3_column_count !\n");
			ret = SQLITE_ERROR;
		}
		/* 判断该列是否为NULL */
		else if( SQLITE_NULL == sqlite3_column_type(stmt, 0) )
		{
			ret = SQLITE_OK;
		}
		/* 获取列数据类型，列编号从0开始的*/
		else if( SQLITE_INTEGER == sqlite3_column_type(stmt, 0) )
		{
			ms->status = (sqlite3_column_int(stmt, 0)?1:0);
			ret = SQLITE_OK;
		}
		else
		{
			fprintf(stderr, "ERROR: dbs_get_mslog->sqlite3_column_type : NOT INTEGER !\n");
			ret = SQLITE_ERROR;
		}
	}
	else
	{
		fprintf(stderr, "ERROR: dbs_get_mslog->sqlite3_step\n");
		ret = SQLITE_ERROR;
	}
	
	/* 释放sqlite3_stmt */
	sqlite3_finalize(stmt); 	
	return ret;
}

int dbs_write_alarmlog(st_dbsAlarmlog *v)
{
	uint32_t iCount = 0;

	/* 获取当前数据表的条目数量*/
	if( SQLITE_OK != __dbs_log_get_count(DBS_LOG_TBL_ID_ALARM, &iCount) )
	{
		return SQLITE_ERROR;
	}

	/* 如果超过最大记录数量，则先删除*/
	if( iCount >= DBS_ALARM_MAX_ROW )
	{
		if( SQLITE_OK != __dbs_delete_alarmlog() )
		{
			return SQLITE_ERROR;
		}
	}

	/* 插入一行数据*/
	return __dbs_persist_alarmlog(v);
}


int dbs_write_optlog(st_dbsOptlog *v)
{
	uint32_t iCount = 0;

	/* 获取当前数据表的条目数量*/
	if( SQLITE_OK != __dbs_log_get_count(DBS_LOG_TBL_ID_OPT, &iCount) )
	{
		return SQLITE_ERROR;
	}

	/* 如果超过最大记录数量，则先删除*/
	if( iCount >= DBS_OPTLOG_MAX_ROW )
	{
		if( SQLITE_OK != __dbs_delete_optlog() )
		{
			return SQLITE_ERROR;
		}
	}

	/* 插入一行数据*/
	return __dbs_persist_optlog(v);
}

int dbs_write_syslog(st_dbsSyslog *v)
{
	uint32_t iCount = 0;

	/* 获取当前数据表的条目数量*/
	if( SQLITE_OK != __dbs_log_get_count(DBS_LOG_TBL_ID_SYS, &iCount) )
	{
		return SQLITE_ERROR;
	}

	/* 如果超过最大记录数量，则先删除*/
	if( iCount >= DBS_SYSLOG_MAX_ROW )
	{
		if( SQLITE_OK != __dbs_delete_syslog() )
		{
			return SQLITE_ERROR;
		}
	}

	/* 插入一行数据*/
	return __dbs_persist_syslog(v);
}

int dbs_write_mslog(st_dbsMslog *ms)
{
	//char sql[DBS_SQL_SMALL_LEN] = {0};
	uint8_t *sql = gBuf_dbsLog;
	bzero(sql, DBS_SQL_MEDIUM_LEN);
	
	assert( NULL != ms );
	
	/* 组装SQL语句*/
	sprintf(sql, "UPDATE [%s] SET [%s]=%d WHERE [id]=%d", 
					db_log[DBS_LOG_TBL_ID_MS].tbl_name, 
					tbl_mslog[DBS_LOG_TBL_MS_COL_ID_MS].col_name, 
					(ms->status?1:0), 
					ms->mid);
	__dbsLogSQL(sql);
	return __dbs_log_sql_exec(sql);
}

int dbs_log_debug_enable(int enable)
{
	DBS_LOG_DEBUG_ENABLE = (enable?1:0);
	return 0;
}

int dbs_log_open(void)
{
	/* 打开数据库*/
	if( sqlite3_open(DBS_LOG_DATABASE, &dbsLogFileHandle) != SQLITE_OK )
	{
		fprintf(stderr, "ERROR: Can't open database: [%s]\n", sqlite3_errmsg(dbsLogFileHandle));
		return SQLITE_ERROR;
	}
	return SQLITE_OK;
}

int dbs_log_close(void)
{
	return sqlite3_close(dbsLogFileHandle);
}

