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
	/* ���ݱ������ţ���������Ҫ����256 */
	uint8_t	tbl_id;	
	/* ���ݱ�����*/
	uint8_t	tbl_name[DBS_TBL_NAME_MAX_LEN];
	/* ���ݱ�������*/
	uint16_t	tbl_cols;							
}DBS_DB_DESC;

typedef struct
{
	/* ���ݱ���������Ҫ����256 */
	uint8_t	col_id;	
	/* ���ݱ�����������*/
	uint16_t	col_type;	
	/* ���ݱ����ܳ���*/
	uint16_t	col_len;			
	/* �����Ƿ�Ϊ����*/
	uint8_t	isPrimaryKey;	
	/* �����Ƿ�����Ϊ��*/
	uint8_t	allowNull;	
	/* ���е�����*/
	uint8_t	col_name[DBS_TBL_COL_NAME_MAX_LEN];	
}DBS_TBL_DESIGN;

#pragma pack (pop)

#endif

