/*****************************************************************************************
  文件名称 : dbs.c
  文件描述 : 数据库访问模块主入口函数
  				主要处理来自外部模块的数据库读写请求。
  修订记录 :
           1 创建 : frank
             日期 : 2010-11-03
             描述 : 创建文件

 *****************************************************************************************/
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <sqlite3.h>
#include <dbs.h>
#include <dbsUnderlayer.h>
#include <dbsLog.h>
#include <dbsUtils.h>
//#include <mtd-abi.h>

#include <boardapi.h>

BBLOCK_QUEUE bblock;
uint8_t gBuf_dbs[MAX_UDP_SIZE] = {0};

static int DBS_ACCESS_DEBUG_ENABLE = 0;

int dbs_register(void);
int dbs_destroy(void);
int dbs_syslog(uint32_t priority, const char *message);
void dbs_msg_dump(const unsigned char memory [], size_t length, FILE *fp);
void dbs_signal_term(int n);
void dbs_signal_int(int n);
void dbs_signal_kill(int n);
void dbs_ack(BBLOCK_QUEUE *this);
void dbs_process(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_err(BBLOCK_QUEUE *this, uint32_t ErrorCode);
BBLOCK_QUEUE * dbs_getLogCount(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getSyslog(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getOptlog(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getAlarmlog(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_writeSyslog(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_writeAlarmlog(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_writeOptlog(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_debugMsgEnable(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_debugMsgDisable(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_debugSQLEnable(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_debugSQLDisable(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getColumn(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_updateColumn(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getRowCliRole(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getRowClt(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getRowCltconf(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getRowCnu(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getRowDepro(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getRowTemplate(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getRowNetwork(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getRowProfile(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getRowSnmp(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getRowSwmgmt(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getRowSysinfo(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_updateRowCliRole(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_updateRowClt(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_updateRowCltconf(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_updateRowCnu(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_updateRowDepro(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_updateRowNetwork(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_updateRowProfile(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_updateRowSnmp(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_updateRowSwmgmt(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_updateRowSysinfo(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_destroyRowClt(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_destroyRowCltconf(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_destroyRowCnu(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_destroyRowProfile(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_createSuProfileForCnu(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_createDewlProfileForCnu(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_createDeblProfileForCnu(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_createSuProfileForWec701Cnu(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_createDewlProfileForWec701Cnu(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_createDeblProfileForWec701Cnu(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_getModuleStatus(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_registerModule(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_destroyModule(BBLOCK_QUEUE *this);
BBLOCK_QUEUE * dbs_fflush(BBLOCK_QUEUE *this);
int dbs_board_init(void);
int dbs_check_databases(void);
int dbs_init_socket(BBLOCK_QUEUE *this);
int dbs_close_socket(BBLOCK_QUEUE *this);


/********************************************************************************************
*	函数名称:dbs_msg_dump
*	函数功能:调试用API，以十六进制的方式将缓冲区的内容输出到
*				   文件指针fp指向的设备文件
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void dbs_msg_dump(const unsigned char memory [], size_t length, FILE *fp)
{
	if(DBS_ACCESS_DEBUG_ENABLE)
	{
		printf("----------------------------------------------------------------------\n");
		hexdump(memory, length, fp);
		printf("\n----------------------------------------------------------------------\n");
	}
}

int dbs_register(void)
{
	st_dbsMslog mslog;
	mslog.mid = MID_DBS;
	mslog.status = 1;
	return dbs_write_mslog(&mslog);
}

int dbs_destroy(void)
{
	st_dbsMslog mslog;
	mslog.mid = MID_DBS;
	mslog.status = 0;
	return dbs_write_mslog(&mslog);
}

int dbs_syslog(uint32_t priority, const char *message)
{
	st_dbsSyslog v;
	time_t b_time;
	
	time(&b_time);
	
	v.time = b_time;
	v.who = MID_DBS;
	v.level = priority;

	if( strlen(message) < DBS_COL_MAX_LEN )
	{
		strcpy( v.log, message);		
	}
	else
	{
		strncpy(v.log, message, DBS_COL_MAX_LEN-1);
		v.log[DBS_COL_MAX_LEN] = '\0';
	}
	
	return dbs_write_syslog(&v);
}

/********************************************************************************************
*	函数名称:dbs_signalProcessHandle
*	函数功能:异常处理句柄函数，当进程退出时该函数被调度
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void dbs_signal_term(int n)
{
	//BBLOCK_QUEUE *this = &bblock;
	fprintf(stderr, "INFO: dbs skipped SIGTERM\n");
}

void dbs_signal_int(int n)
{
	//BBLOCK_QUEUE *this = &bblock;
	fprintf(stderr, "INFO: dbs skipped SIGINT\n");
}

void dbs_signal_kill(int n)
{
	BBLOCK_QUEUE *this = &bblock;
	fprintf(stderr, "INFO: dbs_sigkillProcessHandle close dbs !\n");
	dbs_syslog(DBS_LOG_INFO, "dbs_sigkillProcessHandle close dbs");	
	dbs_destroy();
	dbs_underlayer_close();
	dbs_log_close();
	dbs_close_socket(this);	
	exit(0);
}

int dbs_close_socket(BBLOCK_QUEUE *this)
{
	if( this->sk.sk != 0)
	{
		close(this->sk.sk);
		this->sk.sk = 0;
	}
	return CMM_SUCCESS;
}

int dbs_init_socket(BBLOCK_QUEUE *this)
{
	struct sockaddr_in dbsServerAddr;

	/*创建UDP SOCKET接口*/
	if( ( this->sk.sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}
	else
	{		
		bzero((char *)&(this->sk.skaddr), sizeof(struct sockaddr_in));
       	dbsServerAddr.sin_family = PF_INET;
       	dbsServerAddr.sin_port = htons(DBS_LISTEN_PORT);	/* 目的端口号*/
       	dbsServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);	/* 目的地址*/
		if (bind(this->sk.sk, (struct sockaddr *)&dbsServerAddr, sizeof(dbsServerAddr))<0)
		{
			dbs_close_socket(this);
			return CMM_CREATE_SOCKET_ERROR;
		}		
	}
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:dbs_ack
*	函数功能:该函数为外部应答函数，请求处理
*				  完成之后通过该函数将处理结果返回给请求者
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void dbs_ack(BBLOCK_QUEUE *this)
{
	assert(NULL != this);	
	
	int sendn = 0;
	T_DB_MSG_HEADER_REQ *h = (T_DB_MSG_HEADER_REQ *)(this->b);

	/* 打印发送的数据报文*/
	if( DBS_ACCESS_DEBUG_ENABLE )
	{
		fprintf(stderr, "\n<==== DB_ACCESS SEND MASSAGE [MID %d PORT %d]:\n", h->usDstMID, ntohs(this->sk.skaddr.sin_port));
		dbs_msg_dump(this->b, this->blen, stderr);
	}	
	
	/* 将处理信息发送给请求者 */
	sendn = sendto(this->sk.sk, this->b, this->blen, 0, (struct sockaddr *)&(this->sk.skaddr), sizeof(this->sk.skaddr));
	if( sendn <= 0 )
	{
		fprintf(stderr, "ERROR: dbs_ack->sendto !\n");
	}
}

/********************************************************************************************
*	函数名称:dbs_err
*	函数功能:
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
BBLOCK_QUEUE * dbs_err(BBLOCK_QUEUE *this, uint32_t ErrorCode)
{
	assert(NULL != this);
	
	T_DB_MSG_HEADER_ACK rh;
	T_DB_MSG_HEADER_REQ *h = (T_DB_MSG_HEADER_REQ *)(this->b);

	rh.usSrcMID = MID_DBS;
	rh.usDstMID = h->usSrcMID;
	rh.usMsgType = h->usMsgType;
	rh.fragment = 0;
	rh.result = ErrorCode;
	rh.ulBodyLength = 0;
	
	this->blen = sizeof(rh);
	memcpy(this->b, &rh, this->blen);
	
	return this;
}

BBLOCK_QUEUE * dbs_getLogCount(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;
	uint32_t *iCount = (uint32_t *)(ack->BUF);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *tbl = (uint16_t *)(req->BUF);

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = sizeof(uint32_t);
	ack->HEADER.result = CMM_SUCCESS;

	/* 获取数据库的数据*/
	switch(*tbl)
	{
		case DBS_LOG_TBL_ID_ALARM:
		{
			if( CMM_SUCCESS != dbs_count_alarmlog(iCount))
			{
				dbs_err(this, CMM_DB_ACCESS_ERROR);
				return this;
			}
			break;
		}
		case DBS_LOG_TBL_ID_OPT:
		{
			if( CMM_SUCCESS != dbs_count_optlog(iCount))
			{
				dbs_err(this, CMM_DB_ACCESS_ERROR);
				return this;
			}
			break;
		}
		case DBS_LOG_TBL_ID_SYS:
		{
			if( CMM_SUCCESS != dbs_count_syslog(iCount))
			{
				dbs_err(this, CMM_DB_ACCESS_ERROR);
				return this;
			}
			break;
		}
		default:
		{
			dbs_err(this, CMM_DB_ACCESS_ERROR);
			return this;
			break;
		}
	}

	ack->HEADER.ulBodyLength = sizeof(uint32_t);
	ack->HEADER.result = CMM_SUCCESS;
	this->blen = (sizeof(ack->HEADER) + sizeof(uint32_t));
	memcpy(this->b, ack, this->blen);
	return this;
}

BBLOCK_QUEUE * dbs_getSyslog(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;
	st_dbsSyslog *plog = (st_dbsSyslog *)(ack->BUF);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint32_t *row = (uint32_t *)(req->BUF);

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_get_syslog(*row, plog) )
	{
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = (sizeof(ack->HEADER) + sizeof(st_dbsSyslog));
		if( this->blen > MAX_UDP_SIZE )
		{	
			dbs_syslog(DBS_LOG_ALERT, "dbs_getSyslog: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			ack->HEADER.ulBodyLength = sizeof(st_dbsSyslog);
			ack->HEADER.result = CMM_SUCCESS;
			memcpy(this->b, ack, this->blen);	
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_getOptlog(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;
	st_dbsOptlog *plog = (st_dbsOptlog *)(ack->BUF);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint32_t *row = (uint32_t *)(req->BUF);

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_get_optlog(*row, plog) )
	{
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = (sizeof(ack->HEADER) + sizeof(st_dbsOptlog));
		if( this->blen > MAX_UDP_SIZE )
		{	
			dbs_syslog(DBS_LOG_ALERT, "dbs_getOptlog: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			ack->HEADER.ulBodyLength = sizeof(st_dbsOptlog);
			ack->HEADER.result = CMM_SUCCESS;
			memcpy(this->b, ack, this->blen);	
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_getAlarmlog(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;
	st_dbsAlarmlog *plog = (st_dbsAlarmlog *)(ack->BUF);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint32_t *row = (uint32_t *)(req->BUF);

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_get_alarmlog(*row, plog) )
	{
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = (sizeof(ack->HEADER) + sizeof(st_dbsAlarmlog));
		if( this->blen > MAX_UDP_SIZE )
		{	
			dbs_syslog(DBS_LOG_ALERT, "dbs_getOptlog: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			ack->HEADER.ulBodyLength = sizeof(st_dbsAlarmlog);
			ack->HEADER.result = CMM_SUCCESS;
			memcpy(this->b, ack, this->blen);	
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_writeSyslog(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsSyslog *pSyslog = (st_dbsSyslog *)(req->BUF);
	
	st_dbsSyslog v;
	memcpy(&v, pSyslog, sizeof(st_dbsSyslog));

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_SUCCESS;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_write_syslog(&v) )
	{
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER);
		memcpy(this->b, ack, this->blen);
	}

	return this;
}

BBLOCK_QUEUE * dbs_writeOptlog(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsOptlog *pOptlog = (st_dbsOptlog *)(req->BUF);
	
	st_dbsOptlog v;
	memcpy(&v, pOptlog, sizeof(st_dbsOptlog));

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_SUCCESS;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_write_optlog(&v) )
	{
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER);
		memcpy(this->b, ack, this->blen);
	}

	return this;
}

BBLOCK_QUEUE * dbs_writeAlarmlog(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsAlarmlog *pAlarmlog = (st_dbsAlarmlog *)(req->BUF);
	
	st_dbsAlarmlog v;
	memcpy(&v, pAlarmlog, sizeof(st_dbsAlarmlog));

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_SUCCESS;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_write_alarmlog(&v) )
	{
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER);
		memcpy(this->b, ack, this->blen);
	}
	return this;
}


BBLOCK_QUEUE * dbs_debugMsgEnable(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	T_DB_MSG_HEADER_ACK rh;
	T_DB_MSG_HEADER_REQ *h = (T_DB_MSG_HEADER_REQ *)(this->b);

	DBS_ACCESS_DEBUG_ENABLE = 1;

	rh.usSrcMID = MID_DBS;
	rh.usDstMID = h->usSrcMID;
	rh.usMsgType = h->usMsgType;
	rh.fragment = 0;
	rh.result = CMM_SUCCESS;
	rh.ulBodyLength = 0;
	
	this->blen = sizeof(rh);
	memcpy(this->b, &rh, this->blen);
	
	return this;
}

BBLOCK_QUEUE * dbs_debugMsgDisable(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	T_DB_MSG_HEADER_ACK rh;
	T_DB_MSG_HEADER_REQ *h = (T_DB_MSG_HEADER_REQ *)(this->b);

	DBS_ACCESS_DEBUG_ENABLE = 0;

	rh.usSrcMID = MID_DBS;
	rh.usDstMID = h->usSrcMID;
	rh.usMsgType = h->usMsgType;
	rh.fragment = 0;
	rh.result = CMM_SUCCESS;
	rh.ulBodyLength = 0;
	
	this->blen = sizeof(rh);
	memcpy(this->b, &rh, this->blen);
	
	return this;
}

BBLOCK_QUEUE * dbs_debugSQLEnable(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	T_DB_MSG_HEADER_ACK rh;
	T_DB_MSG_HEADER_REQ *h = (T_DB_MSG_HEADER_REQ *)(this->b);

	dbs_underlayer_debug_enable(1);
	dbs_log_debug_enable(1);

	rh.usSrcMID = MID_DBS;
	rh.usDstMID = h->usSrcMID;
	rh.usMsgType = h->usMsgType;
	rh.fragment = 0;
	rh.result = CMM_SUCCESS;
	rh.ulBodyLength = 0;
	
	this->blen = sizeof(rh);
	memcpy(this->b, &rh, this->blen);
	
	return this;
}

BBLOCK_QUEUE * dbs_debugSQLDisable(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	T_DB_MSG_HEADER_ACK rh;
	T_DB_MSG_HEADER_REQ *h = (T_DB_MSG_HEADER_REQ *)(this->b);

	dbs_underlayer_debug_enable(0);
	dbs_log_debug_enable(0);

	rh.usSrcMID = MID_DBS;
	rh.usDstMID = h->usSrcMID;
	rh.usMsgType = h->usMsgType;
	rh.fragment = 0;
	rh.result = CMM_SUCCESS;
	rh.ulBodyLength = 0;
	
	this->blen = sizeof(rh);
	memcpy(this->b, &rh, this->blen);
	
	return this;
}

BBLOCK_QUEUE * dbs_getColumn(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	uint8_t mmsg[256] = {0};
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	DB_COL_VAR *ack_cell = (DB_COL_VAR *)(ack->BUF);

	memcpy(&(ack_cell->ci), req->BUF, sizeof(DB_COL_INFO));

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_get_column(ack_cell) )
	{
		sprintf(mmsg, "dbs_getColumn(%d, %d, %d): CMM_DB_ACCESS_ERROR", 
			ack_cell->ci.tbl, ack_cell->ci.row, ack_cell->ci.col);
		dbs_syslog(DBS_LOG_ALERT, mmsg);
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.ulBodyLength = sizeof(DB_COL_VAR) + ack_cell->len;
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		
		if( this->blen > MAX_UDP_SIZE )
		{	
			dbs_syslog(DBS_LOG_ALERT, "dbs_getColumn: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			memcpy(this->b, ack, this->blen);			
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_updateColumn(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	uint8_t mmsg[256] = {0};
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	DB_COL_VAR *req_cell = (DB_COL_VAR *)(req->BUF);

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_update_column(req_cell) )
	{
		sprintf(mmsg, "dbs_updateColumn(%d, %d, %d): CMM_DB_ACCESS_ERROR", 
			req_cell->ci.tbl, req_cell->ci.row, req_cell->ci.col);
		dbs_syslog(DBS_LOG_ALERT, mmsg);
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_getRowCliRole(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsCliRole *ack_cell = (st_dbsCliRole *)(ack->BUF);

	ack_cell->id = *req_cell;

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_get_row_clirole(ack_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_getRowCliRole: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.ulBodyLength = sizeof(st_dbsCliRole);
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		
		if( this->blen > MAX_UDP_SIZE )
		{
			dbs_syslog(DBS_LOG_ALERT, "dbs_getRowCliRole: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			memcpy(this->b, ack, this->blen);			
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_getRowClt(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsClt *ack_cell = (st_dbsClt *)(ack->BUF);

	ack_cell->id = *req_cell;

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_get_row_clt(ack_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_getRowClt: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.ulBodyLength = sizeof(st_dbsClt);
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		
		if( this->blen > MAX_UDP_SIZE )
		{	
			dbs_syslog(DBS_LOG_ALERT, "dbs_getRowClt: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			memcpy(this->b, ack, this->blen);			
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_getRowCltconf(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsCltConf *ack_cell = (st_dbsCltConf *)(ack->BUF);

	ack_cell->id = *req_cell;

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_get_row_cltconfig(ack_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_getRowCltconf: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.ulBodyLength = sizeof(st_dbsCltConf);
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		
		if( this->blen > MAX_UDP_SIZE )
		{
			dbs_syslog(DBS_LOG_ALERT, "dbs_getRowCltconf: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			memcpy(this->b, ack, this->blen);			
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_getRowCnu(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsCnu *ack_cell = (st_dbsCnu *)(ack->BUF);

	ack_cell->id = *req_cell;

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_get_row_cnu(ack_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_getRowCnu: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.ulBodyLength = sizeof(st_dbsCnu);
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		
		if( this->blen > MAX_UDP_SIZE )
		{
			dbs_syslog(DBS_LOG_ALERT, "dbs_getRowCnu: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			memcpy(this->b, ack, this->blen);			
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_getRowDepro(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsCnuDefaultProfile *ack_cell = (st_dbsCnuDefaultProfile *)(ack->BUF);

	ack_cell->id = *req_cell;

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_get_row_depro(ack_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_getRowDepro: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.ulBodyLength = sizeof(st_dbsCnuDefaultProfile);
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		
		if( this->blen > MAX_UDP_SIZE )
		{
			dbs_syslog(DBS_LOG_ALERT, "dbs_getRowDepro: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			memcpy(this->b, ack, this->blen);			
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_getRowTemplate(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsTemplate *ack_cell = (st_dbsTemplate *)(ack->BUF);

	ack_cell->id = *req_cell;

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_get_row_template(ack_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_getRowTemplate: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.ulBodyLength = sizeof(st_dbsTemplate);
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		
		if( this->blen > MAX_UDP_SIZE )
		{
			dbs_syslog(DBS_LOG_ALERT, "dbs_getRowTemplate: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			memcpy(this->b, ack, this->blen);			
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_getRowNetwork(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsNetwork *ack_cell = (st_dbsNetwork *)(ack->BUF);

	ack_cell->id = *req_cell;

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_get_row_network(ack_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_getRowNetwork: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.ulBodyLength = sizeof(st_dbsNetwork);
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		
		if( this->blen > MAX_UDP_SIZE )
		{	
			dbs_syslog(DBS_LOG_ALERT, "dbs_getRowNetwork: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			memcpy(this->b, ack, this->blen);			
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_getRowProfile(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsProfile *ack_cell = (st_dbsProfile *)(ack->BUF);

	ack_cell->id = *req_cell;

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_get_row_profile(ack_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_getRowProfile: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.ulBodyLength = sizeof(st_dbsProfile);
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		
		if( this->blen > MAX_UDP_SIZE )
		{
			dbs_syslog(DBS_LOG_ALERT, "dbs_getRowProfile: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			memcpy(this->b, ack, this->blen);			
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_getRowSnmp(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsSnmp *ack_cell = (st_dbsSnmp *)(ack->BUF);

	ack_cell->id = *req_cell;

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_get_row_snmp(ack_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_getRowSnmp: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.ulBodyLength = sizeof(st_dbsSnmp);
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		
		if( this->blen > MAX_UDP_SIZE )
		{	
			dbs_syslog(DBS_LOG_ALERT, "dbs_getRowSnmp: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			memcpy(this->b, ack, this->blen);			
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_getRowSwmgmt(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsSwmgmt *ack_cell = (st_dbsSwmgmt *)(ack->BUF);

	ack_cell->id = *req_cell;

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_get_row_swmgmt(ack_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_getRowSwmgmt: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.ulBodyLength = sizeof(st_dbsSwmgmt);
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		
		if( this->blen > MAX_UDP_SIZE )
		{
			dbs_syslog(DBS_LOG_ALERT, "dbs_getRowSwmgmt: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			memcpy(this->b, ack, this->blen);			
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_getRowSysinfo(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	st_dbsSysinfo *ack_cell = (st_dbsSysinfo *)(ack->BUF);

	ack_cell->id = *req_cell;

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_get_row_sysinfo(ack_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_getRowSysinfo: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.ulBodyLength = sizeof(st_dbsSysinfo);
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		
		if( this->blen > MAX_UDP_SIZE )
		{	
			dbs_syslog(DBS_LOG_ALERT, "dbs_getRowSysinfo: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			memcpy(this->b, ack, this->blen);			
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_SelectCnuIndexByMac(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	char macaddr[32] = {0};
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint8_t *req_cell = (uint8_t *)(req->BUF);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	
	stCnuNode *ack_cell = (stCnuNode *)(ack->BUF);

	memcpy(macaddr, req_cell, req->HEADER.ulBodyLength);
	macaddr[req->HEADER.ulBodyLength] = '\0';

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_select_cnu_index_by_mac(macaddr, ack_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_SelectCnuIndexByMac: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.ulBodyLength = sizeof(stCnuNode);
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		
		if( this->blen > MAX_UDP_SIZE )
		{
			dbs_syslog(DBS_LOG_ALERT, "dbs_SelectCnuIndexByMac: CMM_BUFFER_OVERFLOW");
			dbs_err(this, CMM_BUFFER_OVERFLOW);
		}
		else
		{
			memcpy(this->b, ack, this->blen);			
		}
	}	
	return this;
}

BBLOCK_QUEUE * dbs_updateRowCliRole(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	st_dbsCliRole *req_cell = (st_dbsCliRole *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_update_row_clirole(req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_updateRowCliRole: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}


BBLOCK_QUEUE * dbs_updateRowClt(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	st_dbsClt *req_cell = (st_dbsClt *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_update_row_clt(req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_updateRowClt: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_updateRowCltconf(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	st_dbsCltConf *req_cell = (st_dbsCltConf *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_update_row_cltconfig(req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_updateRowCltconf: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_updateRowCnu(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	st_dbsCnu *req_cell = (st_dbsCnu *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_update_row_cnu(req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_updateRowCnu: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_updateRowDepro(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	st_dbsCnuDefaultProfile *req_cell = (st_dbsCnuDefaultProfile *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_update_row_depro(req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_updateRowDepro: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_updateRowNetwork(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	st_dbsNetwork *req_cell = (st_dbsNetwork *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_update_row_network(req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_updateRowNetwork: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_updateRowProfile(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	st_dbsProfile *req_cell = (st_dbsProfile *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_update_row_profile(req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_updateRowProfile: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_updateRowTemplate(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	st_dbsTemplate  *req_cell = (st_dbsTemplate *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_update_row_template(req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_updateRowTemplate: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}
	
BBLOCK_QUEUE * dbs_updateRowSnmp(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	st_dbsSnmp *req_cell = (st_dbsSnmp *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_update_row_snmp(req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_updateRowSnmp: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_updateRowSwmgmt(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	st_dbsSwmgmt *req_cell = (st_dbsSwmgmt *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_update_row_swmgmt(req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_updateRowSwmgmt: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_updateRowSysinfo(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	st_dbsSysinfo *req_cell = (st_dbsSysinfo *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_update_row_sysinfo(req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_updateRowSysinfo: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_destroyRowClt(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_destory_row_clt(*req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_destroyRowClt: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_destroyRowCltconf(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_destory_row_cltconf(*req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_destroyRowCltconf: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_destroyRowCnu(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_destory_row_cnu(*req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_destroyRowCnu: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_destroyRowProfile(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);
	
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_destory_row_profile(*req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_destroyRowProfile: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_createSuProfileForCnu(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_create_su_profile(*req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_createSuProfileForCnu: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_createDewlProfileForCnu(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_create_dewl_profile(*req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_createDewlProfileForCnu: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_createDeblProfileForCnu(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_create_debl_profile(*req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_createDeblProfileForCnu: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_createSuProfileForWec701Cnu(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_create_su2_profile(*req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_createSuProfileForCnu: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_createDewlProfileForWec701Cnu(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_create_dewl2_profile(*req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_createDewlProfileForCnu: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_createDeblProfileForWec701Cnu(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_cell = (uint16_t *)(req->BUF);

	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;	

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_underlayer_create_debl2_profile(*req_cell) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_createDeblProfileForCnu: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		ack->HEADER.result = CMM_SUCCESS;
		memcpy(this->b, ack, this->blen);
	}	
	return this;
}

BBLOCK_QUEUE * dbs_getModuleStatus(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;
	uint16_t *ack_ms = (uint16_t *)(ack->BUF);

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_mid = (uint16_t *)(req->BUF);

	st_dbsMslog mslog;
	mslog.mid = *req_mid;
	mslog.status = 0;

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	/* 获取数据库的数据*/
	if( SQLITE_OK != dbs_get_mslog(&mslog) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_getModuleStatus: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.ulBodyLength = sizeof(uint16_t);
		ack->HEADER.result = CMM_SUCCESS;
		*ack_ms = mslog.status;
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		memcpy(this->b, ack, this->blen);	
	}	
	return this;
}

BBLOCK_QUEUE * dbs_registerModule(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_mid = (uint16_t *)(req->BUF);

	st_dbsMslog mslog;

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	mslog.mid = *req_mid;
	mslog.status = 1;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_write_mslog(&mslog) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_registerModule: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.result = CMM_SUCCESS;
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		memcpy(this->b, ack, this->blen);	
	}	
	return this;
}

BBLOCK_QUEUE * dbs_destroyModule(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;

	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);
	uint16_t *req_mid = (uint16_t *)(req->BUF);

	st_dbsMslog mslog;

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_FAILED;

	mslog.mid = *req_mid;
	mslog.status = 0;

	/* 更新数据库的数据*/
	if( SQLITE_OK != dbs_write_mslog(&mslog) )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_destroyModule: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);
	}
	else
	{
		ack->HEADER.result = CMM_SUCCESS;
		this->blen = sizeof(ack->HEADER) + ack->HEADER.ulBodyLength;
		memcpy(this->b, ack, this->blen);	
	}	
	return this;
}

BBLOCK_QUEUE * dbs_fflush(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint8_t *buf = gBuf_dbs;
	bzero(buf, MAX_UDP_SIZE);
	
	T_DB_MSG_PACKET_ACK *ack = (T_DB_MSG_PACKET_ACK *)buf;
	T_DB_MSG_PACKET_REQ *req = (T_DB_MSG_PACKET_REQ *)(this->b);

	ack->HEADER.usSrcMID = MID_DBS;
	ack->HEADER.usDstMID = req->HEADER.usSrcMID;
	ack->HEADER.usMsgType = req->HEADER.usMsgType;
	ack->HEADER.fragment = 0;
	ack->HEADER.ulBodyLength = 0;
	ack->HEADER.result = CMM_SUCCESS;

	if( SQLITE_OK != dbs_underlayer_fflush() )
	{
		dbs_syslog(DBS_LOG_ALERT, "dbs_underlayer_fflush: CMM_DB_ACCESS_ERROR");
		dbs_err(this, CMM_DB_ACCESS_ERROR);	
	}
	else
	{
		this->blen = sizeof(ack->HEADER);
		memcpy(this->b, ack, this->blen);
	}

	return this;
}

int dbs_board_init(void)
{
	char str_devmodel[32] = {0};
	char str_wdt[32] = {0};
	//uint8_t *buf = gBuf_dbs;	
	//bzero(buf, MAX_UDP_SIZE);		
	//DB_COL_VAR *dbsColValue = (DB_COL_VAR *)buf;	
	//uint8_t *cbat_mac = NULL;
	st_dbsSysinfo rowSysinfo;
	st_dbsNetwork rowNetinfo;

	if( CMM_SUCCESS != nvm_get_parameter("devmodel", str_devmodel) )
	{
		return CMM_FAILED;
	}
	else if( CMM_SUCCESS != nvm_get_parameter("watchdog", str_wdt) )
	{
		return CMM_FAILED;
	}

	/* 初始化tbl_sysinfo *//* col_model */
	/* 初始化tbl_sysinfo *//* app_version */
	rowSysinfo.id = 1;
	if( CMM_SUCCESS != dbs_underlayer_get_row_sysinfo(&rowSysinfo) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(rowSysinfo.col_appver, SYSINFO_APP_VERSION);
		strcpy(rowSysinfo.col_bver, SYSINFO_BOOT_VERSION);
		strcpy(rowSysinfo.col_kver, SYSINFO_KERNEL_VERSION);
		strcpy(rowSysinfo.col_hwver, SYSINFO_HW_VERSION);
		strcpy(rowSysinfo.col_apphash, SYSINFO_APP_HASH);
		rowSysinfo.col_model = atoi(str_devmodel);
		/*
		if( SYSINFO_DEVICE_MODEL != rowSysinfo.col_model )
		{
			fprintf(stderr,"WARNNING: device model is not matched in nvm\n");
		}
		*/
		if( strcmp(str_wdt, "on") == 0 )
		{
			rowSysinfo.col_wdt = 1;
		}
		else
		{
			rowSysinfo.col_wdt = 0;
		}
		rowSysinfo.col_flashsize = SYSINFO_FLASH_SIZE;	
		rowSysinfo.col_ramsize = SYSINFO_RAM_SIZE;
		rowSysinfo.col_maxclt = MAX_CLT_AMOUNT_LIMIT;
		if( CMM_SUCCESS != dbs_underlayer_update_row_sysinfo(&rowSysinfo) )
		{
			return CMM_FAILED;
		}
	}

	/* 初始化tbl_network *//* cbat_mac */
	rowNetinfo.id = 1;
	if( CMM_SUCCESS != dbs_underlayer_get_row_network(&rowNetinfo) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(rowNetinfo.col_mac, boardapi_getMacAddress());
		
		if( CMM_SUCCESS != dbs_underlayer_update_row_network(&rowNetinfo) )
		{
			return CMM_FAILED;
		}
	}	

	return CMM_SUCCESS;
}

int dbs_check_databases(void)
{
	int flag = 0;
	uint8_t cmd[128] = {0};

	if( access(DBS_SYS_DATABASE, 0) )
	{
		fprintf(stderr, "WARNNING: can not find %s\n", DBS_SYS_DATABASE);
		if( !access(DBS_SYS_DATABASE_FACTORY, 0) )
		{
			fprintf(stderr, "copy from %s\n", DBS_SYS_DATABASE_FACTORY);
			sprintf(cmd, "cp %s %s", DBS_SYS_DATABASE_FACTORY, DBS_SYS_DATABASE_FFS);
			system(cmd);
			flag++;
		}
		else
		{
			fprintf(stderr, "ERROR: can not find %s\n", DBS_SYS_DATABASE_FACTORY);
		}
	}
	if( access(DBS_LOG_DATABASE, 0) )
	{
		fprintf(stderr, "WARNNING: can not find %s\n", DBS_LOG_DATABASE);
		if( !access(DBS_LOG_DATABASE_FACTORY, 0) )
		{
			fprintf(stderr, "copy from %s\n", DBS_LOG_DATABASE_FACTORY);
			sprintf(cmd, "cp %s %s", DBS_LOG_DATABASE_FACTORY, DBS_LOG_DATABASE_FFS);
			system(cmd);
			flag++;
		}
		else
		{
			fprintf(stderr, "ERROR: can not find %s\n", DBS_LOG_DATABASE_FACTORY);
		}
	}
	return flag;
}

void dbs_process(BBLOCK_QUEUE *this)
{
	T_DB_MSG_HEADER_REQ *h = NULL;
	int FromAddrSize = sizeof(this->sk.skaddr);

	while(1)
	{
		bzero(this->b, MAX_UDP_SIZE);
		this->blen = recvfrom(this->sk.sk, this->b, MAX_UDP_SIZE, 0, 
					(struct sockaddr *)&(this->sk.skaddr), &FromAddrSize);

		h = (T_DB_MSG_HEADER_REQ *)(this->b);
		
		/* 打印接收到的报文*/
		if( DBS_ACCESS_DEBUG_ENABLE )
		{
			fprintf(stderr, "\n====> DBS RECIEVED MASSAGE [MID %d PORT %d]:\n", h->usSrcMID, ntohs(this->sk.skaddr.sin_port));
			dbs_msg_dump(this->b, this->blen, stderr);
		}
		
		if( h->usDstMID != MID_DBS)
		{
			fprintf(stderr, "WARNNING: dbs_process->usDstMID [%d] [continue] !\n", h->usDstMID);
			dbs_syslog(DBS_LOG_WARNING, "dbs_process->usDstMID [NOT MID_DBS] [continue]");
			dbs_ack(dbs_err(this, CMM_UNKNOWN_MTYPE));
			continue;
		}

		/* 该分支语句中的任意一个case的处理函数都不应该阻塞 */
		switch(h->usMsgType)
		{
			case DB_GET_LOG_COUNT:
				dbs_ack(dbs_getLogCount(this));
				break;
			case DB_GET_SYSLOG:
				dbs_ack(dbs_getSyslog(this));
				break;
			case DB_GET_OPTLOG:
				dbs_ack(dbs_getOptlog(this));
				break;
			case DB_GET_ALARMLOG:
				dbs_ack(dbs_getAlarmlog(this));
				break;
			case DB_WRITE_SYSLOG:
				dbs_ack(dbs_writeSyslog(this));
				break;
			case DB_WRITE_OPTLOG:
				dbs_ack(dbs_writeOptlog(this));
				break;
			case DB_WRITE_ALARMLOG:
				dbs_ack(dbs_writeAlarmlog(this));
				break;
			case DB_MSG_DEBUG_ENABLE:
				dbs_ack(dbs_debugMsgEnable(this));
				break;
			case DB_MSG_DEBUG_DISABLE:
				dbs_ack(dbs_debugMsgDisable(this));
				break;
			case DB_SQL_DEBUG_ENABLE:
				dbs_ack(dbs_debugSQLEnable(this));
				break;
			case DB_SQL_DEBUG_DISABLE:
				dbs_ack(dbs_debugSQLDisable(this));
				break;
			case DB_GET_COL_VALUE:
				dbs_ack(dbs_getColumn(this));
				break;
			case DB_UPDATE_COL_VALUE:
				dbs_ack(dbs_updateColumn(this));
				break;
			case DB_GET_MODULE_STS:
				dbs_ack(dbs_getModuleStatus(this));
				break;
			case DB_REGISTER_MODULE:
				dbs_ack(dbs_registerModule(this));
				break;
			case DB_DESTROY_MODULE:
				dbs_ack(dbs_destroyModule(this));
				break;
			case DB_GET_ROW_CLI_ROLE:
				dbs_ack(dbs_getRowCliRole(this));
				break;
			case DB_UPDATE_ROW_CLI_ROLE:
				dbs_ack(dbs_updateRowCliRole(this));
				break;
			case DB_GET_ROW_CLT:
				dbs_ack(dbs_getRowClt(this));
				break;
			case DB_UPDATE_ROW_CLT:
				dbs_ack(dbs_updateRowClt(this));
				break;
			case DB_GET_ROW_CLTCONF:
				dbs_ack(dbs_getRowCltconf(this));
				break;
			case DB_UPDATE_ROW_CLTCONF:
				dbs_ack(dbs_updateRowCltconf(this));
				break;
			case DB_GET_ROW_CNU:
				dbs_ack(dbs_getRowCnu(this));
				break;
			case DB_UPDATE_ROW_CNU:
				dbs_ack(dbs_updateRowCnu(this));
				break;
			case DB_GET_ROW_DEPRO:
				dbs_ack(dbs_getRowDepro(this));
				break;
			case DB_GET_ROW_TEMPLATE:	
				dbs_ack(dbs_getRowTemplate(this));
				break;
			case DB_UPDATE_ROW_DEPRO:
				dbs_ack(dbs_updateRowDepro(this));
				break;
			case DB_GET_ROW_NETWORK:
				dbs_ack(dbs_getRowNetwork(this));
				break;
			case DB_UPDATE_ROW_NETWORK:
				dbs_ack(dbs_updateRowNetwork(this));
				break;
			case DB_GET_ROW_PROFILE:
				dbs_ack(dbs_getRowProfile(this));
				break;
			case DB_UPDATE_ROW_PROFILE:
				dbs_ack(dbs_updateRowProfile(this));
				break;
			case DB_UPDATE_ROW_TEMPLATE:
				dbs_ack(dbs_updateRowTemplate(this));
				break;
			case DB_GET_ROW_SNMP:
				dbs_ack(dbs_getRowSnmp(this));
				break;
			case DB_UPDATE_ROW_SNMP:
				dbs_ack(dbs_updateRowSnmp(this));
				break;
			case DB_GET_ROW_SWMGMT:
				dbs_ack(dbs_getRowSwmgmt(this));
				break;
			case DB_UPDATE_ROW_SWMGMT:
				dbs_ack(dbs_updateRowSwmgmt(this));
				break;
			case DB_GET_ROW_SYSINFO:
				dbs_ack(dbs_getRowSysinfo(this));
				break;
			case DB_UPDATE_ROW_SYSINFO:
				dbs_ack(dbs_updateRowSysinfo(this));
				break;
			case DB_DESTROY_ROW_TBL_CLT:
				dbs_ack(dbs_destroyRowClt(this));
				break;
			case DB_DESTROY_ROW_TBL_CLTCONF:
				dbs_ack(dbs_destroyRowCltconf(this));
				break;
			case DB_DESTROY_ROW_TBL_CNU:
				dbs_ack(dbs_destroyRowCnu(this));
				break;
			case DB_DESTROY_ROW_TBL_PROFILE:
				dbs_ack(dbs_destroyRowProfile(this));
				break;
			case DB_CREATE_SU_PROFILE:
				dbs_ack(dbs_createSuProfileForCnu(this));
				break;
			case DB_CREATE_WL_DE_PROFILE:
				dbs_ack(dbs_createDewlProfileForCnu(this));
				break;
			case DB_CREATE_BL_DE_PROFILE:
				dbs_ack(dbs_createDeblProfileForCnu(this));
				break;
			case DB_CREATE_SU2_PROFILE:
				dbs_ack(dbs_createSuProfileForWec701Cnu(this));
				break;
			case DB_CREATE_WL_DE2_PROFILE:
				dbs_ack(dbs_createDewlProfileForWec701Cnu(this));
				break;
			case DB_CREATE_BL_DE2_PROFILE:
				dbs_ack(dbs_createDeblProfileForWec701Cnu(this));
				break;
			case DB_ONLINE_BACKUP:
				dbs_ack(dbs_fflush(this));
				break;
			case DB_SELECT_CNU_INDEX_BY_MAC:
				dbs_ack(dbs_SelectCnuIndexByMac(this));
				break;
			default:
				/* 对于不支持的消息类型应该给予应答以便让请求者知道 */
				dbs_syslog(DBS_LOG_ERR, "dbs_process CMM_UNKNOWN_MMTYPE");
				dbs_ack(dbs_err(this, CMM_UNKNOWN_MMTYPE));
				break;
		}
	}
}

/********************************************************************************************
*	函数名称:main
*	函数功能:CMM主入口函数，接收来自外部模块的请求，
*				   完成业务逻辑的处理
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
int main(void)
{	
	BBLOCK_QUEUE *this = &bblock;

	/* 解决旧版本数据库文件不兼容的问题*/
	if( dbs_check_databases() )
	{
		sleep(3);
		system("reboot");
	}

	/* 打开数据库链接*/
	if( SQLITE_OK != dbs_underlayer_open() )
	{
		fprintf(stderr, "ERROR: dbs->dbs_underlayer_open !\n");
		return CMM_DB_ACCESS_ERROR;
	}
	/* 打开数据库链接*/
	if( SQLITE_OK != dbs_log_open() )
	{
		fprintf(stderr, "ERROR: dbs->dbs_log_open !\n");
		return CMM_DB_ACCESS_ERROR;
	}

	/* 初始化主控板配置*/
	if( dbs_board_init() != CMM_SUCCESS )
	{
		fprintf(stderr, "ERROR: dbs->dbs_board_init !\n");
		dbs_syslog(DBS_LOG_EMERG, "dbs_board_init error, exit");
		dbs_underlayer_close();
		dbs_log_close();
		return CMM_FAILED;
	}
	
	/* 创建外部通讯UDP SOCKET */
	if( dbs_init_socket(this) != CMM_SUCCESS )
	{
		fprintf(stderr, "ERROR: dbs->dbs_init_socket !\n");
		dbs_syslog(DBS_LOG_EMERG, "dbs_init_socket error, exit");
		dbs_underlayer_close();
		dbs_log_close();
		return CMM_CREATE_SOCKET_ERROR;
	}	

	/* 注册DBS 模块*/
	if( SQLITE_OK != dbs_register() )
	{		
		printf("\nStarting module dbs	......		[FAILED]\n");
		dbs_syslog(DBS_LOG_EMERG, "dbs_register error, exit");
		dbs_underlayer_close();
		dbs_log_close();
		dbs_close_socket(this);
		return CMM_DB_ACCESS_ERROR;
	}

	/* 写入系统日志*/
	dbs_syslog(DBS_LOG_INFO, "starting module dbs success");
	//fprintf(stderr, "\n\n====================================================================\n");
	printf("Current app version: %s\n", SYSINFO_APP_VERSION);
	printf("Starting module dbs		......		[OK]\n");
	
	/* 注册异常退出句柄函数*/
	signal(SIGTERM, dbs_signal_term);
	signal(SIGINT, dbs_signal_int);
	/* cannot capture SIGKILL by linux app */
	//signal(SIGKILL, dbs_signal_kill);

	/* 循环处理外部请求*/
	dbs_process(this);

	/* 不要在这个后面添加代码，执行不到滴*/
	dbs_syslog(DBS_LOG_INFO, "exiting module dbs success");
	dbs_destroy();
	dbs_underlayer_close();
	dbs_log_close();
	dbs_close_socket(this);
	return 0;
}


