#ifndef __DBS_H__
#define __DBS_H__

#include <public.h>

#define DBS_TBL_NAME_MAX_LEN		32
#define DBS_TBL_COL_NAME_MAX_LEN	32
#define DBS_SQL_SMALL_LEN			256
#define DBS_SQL_MEDIUM_LEN		512
#define DBS_SQL_BIG_LEN			1024
#define DBS_SQL_LARGE_LEN			2048

#define DBS_LOG_DATABASE	"/var/tmp/log.db"
#define DBS_SYS_DATABASE	"/var/tmp/system.db"
#define DBS_LOG_DATABASE_FFS	"/usr/mnt/config/databases/log.db"
#define DBS_SYS_DATABASE_FFS	"/usr/mnt/config/databases/system.db"
#define DBS_LOG_DATABASE_FACTORY	"/etc/config/databases/log.db"
#define DBS_SYS_DATABASE_FACTORY	"/etc/config/databases/system.db"

#pragma pack (push, 1)

typedef struct
{
	/* 数据表索引号，总数量不要超过256 */
	uint8_t	tbl_id;	
	/* 数据表名称*/
	uint8_t	tbl_name[DBS_TBL_NAME_MAX_LEN];
	/* 数据表总列数*/
	uint16_t	tbl_cols;							
}DBS_DB_DESC;

typedef struct
{
	/* 数据表总列数不要超过256 */
	uint8_t	col_id;	
	/* 数据表列数据类型*/
	uint16_t	col_type;	
	/* 数据表列总长度*/
	uint16_t	col_len;			
	/* 该列是否为主键*/
	uint8_t	isPrimaryKey;	
	/* 该列是否允许为空*/
	uint8_t	allowNull;	
	/* 该列的名称*/
	uint8_t	col_name[DBS_TBL_COL_NAME_MAX_LEN];	
}DBS_TBL_DESIGN;

#pragma pack (pop)

#endif

