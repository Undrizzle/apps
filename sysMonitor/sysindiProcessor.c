/*******************************************************************************************
#处理模块外部请求的子线程，当外部其他模块进行重要操作时，可
#以向该线程发送一个请求以控制系统灯的状态来达到指示用户的目
#的。
#该线程绑定并监听一个套接字，当有外部进程请求到达时，解析请
#求并设置系统状态。
*******************************************************************************************/
#include <assert.h>
#include <public.h>
#include <boardapi.h>
#include <dbsapi.h>
//#include "sm2dbsMutex.h"
#include "systemStsControl.h"
#include "sysindiProcessor.h"

/* 与DBS  通讯的设备文件*/
static T_DBS_DEV_INFO *dbsdev = NULL;

/* 该线程的消息调试开关变量*/
static int SYSINDIPROCESSOR_MSG_DEBUG_ENABLE = 0;

/* 该线程独享的消息收发接口*/
static BBLOCK_QUEUE sysindi_bblock;

#define __sysindi_debug(...);	\
{	\
	if( SYSINDIPROCESSOR_MSG_DEBUG_ENABLE )	\
		fprintf(stderr, __VA_ARGS__);	\
}

/********************************************************************************************
*	函数名称:__sysindi_msg
*	函数功能:调试用API，以十六进制的方式将缓冲区的内容输出到
*				   文件指针fp指向的设备文件
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void __sysindi_msg(const unsigned char memory [], size_t length, FILE *fp)
{
	if(SYSINDIPROCESSOR_MSG_DEBUG_ENABLE)
	{
		printf("----------------------------------------------------------------------\n");
		hexdump(memory, length, fp);
		printf("\n----------------------------------------------------------------------\n");
	}
}

/********************************************************************************************
*	函数名称:sysindi_reply
*	函数功能:该函数为CMM外部应答函数，CMM请求处理
*				  完成之后通过该函数将处理结果返回给请求者
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void sysindi_reply(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	int sendn = 0;

	/* 打印发送的数据报文*/
	__sysindi_debug("\n<==== SYSMONITOR SEND MASSAGE:\n");
	__sysindi_msg(this->b, this->blen, stderr);
	
	/* 将处理信息发送给请求者 */
	sendn = sendto(this->sk.sk, this->b, this->blen, 0, (struct sockaddr *)&(this->sk.skaddr), sizeof(this->sk.skaddr));
	if( sendn <= 0 )
	{
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "sysindi_reply sendto error");
	}
}

/********************************************************************************************
*	函数名称:sysindi_error
*	函数功能:该函数为异常处理的组包函数
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * sysindi_error(BBLOCK_QUEUE *this, uint32_t ErrorCode)
{
	assert(NULL != this);
	
	T_COM_MSG_HEADER_ACK confirm;
	T_COM_MSG_HEADER_REQ *request = (T_COM_MSG_HEADER_REQ *)(this->b);

	confirm.ucMsgAttrib = MSG_ATTRIB_ACK;
	confirm.ulRequestID = request->ulRequestID;
	confirm.usSrcMID = request->usDstMID;
	confirm.usDstMID = request->usSrcMID;
	confirm.usMsgType = request->usMsgType;
	confirm.ucFlag = 0;
	confirm.result = ErrorCode;
	confirm.ulBodyLength = 0;
	
	this->blen = sizeof(confirm);
	memcpy((void *)this->b, (void *)&confirm, this->blen);
	
	return this;
}

BBLOCK_QUEUE * sysindi_ProcessSysledCtl(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint16_t ret = CMM_FAILED;
	
	T_COM_MSG_PACKET_REQ *request = (T_COM_MSG_PACKET_REQ *)(this->b);
	uint8_t sysledEvent = *(uint8_t *)(request->BUF);
	
	T_COM_MSG_HEADER_ACK confirm;	

	//printf("\nsysMonitor_ProcessSysledCtl()\n");

	switch(sysledEvent)
	{
		case SYSLED_STS_NORMAL:
		case SYSLED_STS_BUSY:
		case SYSLED_STS_RESET:
		{
			ret = CMM_SUCCESS;
			//TSVR_systemStatus = sysledEvent;
			set_systemStatus(sysledEvent);
			break;
		}
		default:
		{
			ret = CMM_FAILED;
			break;
		}
	}	

	confirm.ucMsgAttrib = MSG_ATTRIB_ACK;
	confirm.ulRequestID = request->HEADER.ulRequestID;
	confirm.usSrcMID = request->HEADER.usDstMID;
	confirm.usDstMID = request->HEADER.usSrcMID;
	confirm.usMsgType = request->HEADER.usMsgType;
	confirm.ucFlag = 0;
	confirm.result = ret;
	confirm.ulBodyLength = 0;
	this->blen = sizeof(confirm);
	memcpy((void *)this->b, (void *)&confirm, sizeof(confirm));
	
	return this;
}

int sysindi_init(void)
{
	struct sockaddr_in server_addr;
	BBLOCK_QUEUE *this = &sysindi_bblock;
	T_UDP_SK_INFO *sk = &(this->sk);

	/*创建UDP SOCKET接口*/
	if( ( sk->sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}
	else
	{
		bzero((char *)&(sk->skaddr), sizeof(struct sockaddr_in));
       	server_addr.sin_family = PF_INET;
       	server_addr.sin_port = htons(SYSMONITOR_LISTEN_PORT);		/* 目的端口号*/
       	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);			/* 目的地址*/
		if (bind(sk->sk, (struct sockaddr *)&server_addr, sizeof(server_addr))<0)
		{
			sysindi_destroy();
			return CMM_CREATE_SOCKET_ERROR;
		}
	}
	return CMM_SUCCESS;
}

int sysindi_destroy(void)
{
	BBLOCK_QUEUE *this = &sysindi_bblock;
	T_UDP_SK_INFO *sk = &(this->sk);
	
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

void *sysindiProcessor(void)
{	
	BBLOCK_QUEUE *this = &sysindi_bblock;
	
	T_COM_MSG_HEADER_REQ *h = NULL;
	int FromAddrSize = sizeof(this->sk.skaddr);

	assert( NULL != this );

	/* DBS设计变更，支持多线程操作，初始化时在每个线程中独享消息接口*/
	/*创建与数据库模块互斥通讯的外部SOCKET接口*/
	dbsdev = dbsNoWaitOpen(MID_SYSINDI);
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: sysindiProcessor->dbsOpen error, exited !\n");
		return (void *)0;
	}

	/* 创建外部通讯UDP SOCKET */
	if( CMM_SUCCESS != sysindi_init() )
	{
		fprintf(stderr, "sysMonitor init sysindiProcessor failed\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "sysMonitor init sysindiProcessor failed");
		dbsClose(dbsdev);
		return (void *)0;
	}

	fprintf(stderr, "Starting thread sysindiProcessor	......	[OK]\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting thread sysindiProcessor success");

	while(1)
	{
		bzero(this->b, MAX_UDP_SIZE);
		this->blen = recvfrom(this->sk.sk, this->b, MAX_UDP_SIZE, 0, (struct sockaddr *)&(this->sk.skaddr), &FromAddrSize);
		
		/* 打印接收到的报文*/
		__sysindi_debug("\n====> SYSMONITOR RECIEVED MASSAGE:\n");
		__sysindi_msg(this->b, this->blen, stderr);

		h = (T_COM_MSG_HEADER_REQ *)(this->b);
		if( h->ucMsgAttrib != MSG_ATTRIB_REQ)
		{
			dbs_sys_log(dbsdev, DBS_LOG_WARNING, "sysMonitor->sysindiProcessor: NOT MSG_ATTRIB_REQ");
			continue;
		}
		if( h->usDstMID != MID_SYSMONITOR)
		{
			dbs_sys_log(dbsdev, DBS_LOG_WARNING, "sysMonitor->sysindiProcessor: NOT MID_SYSMONITOR");
			continue;
		}

		/* 该分支语句中的任意一个case的处理函数都不应该阻塞 */
		switch(h->usMsgType)
		{
			case CMM_SYSMONITOR_LED_CONTROL:
			{
				sysindi_reply(sysindi_ProcessSysledCtl(this));
				break;
			}			
			default:
			{
				/* 对于不支持的消息类型应该给予应答以便让请求者知道 */
				dbs_sys_log(dbsdev, DBS_LOG_ERR, "sysMonitor->sysindiProcessor: CMM_UNKNOWN_MMTYPE");
				sysindi_reply(sysindi_error(this, CMM_UNKNOWN_MMTYPE));
				break;
			}
		}
	}	

	/* 不要在这个后面添加代码，执行不到滴*/
	printf("thread sysindiProcessor exit !\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "INFO: thread sysindiProcessor exit");
	sysindi_destroy();
	dbsClose(dbsdev);
	return (void *)0;
}

