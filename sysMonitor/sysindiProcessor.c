/*******************************************************************************************
#����ģ���ⲿ��������̣߳����ⲿ����ģ�������Ҫ����ʱ����
#������̷߳���һ�������Կ���ϵͳ�Ƶ�״̬���ﵽָʾ�û���Ŀ
#�ġ�
#���̰߳󶨲�����һ���׽��֣������ⲿ�������󵽴�ʱ��������
#������ϵͳ״̬��
*******************************************************************************************/
#include <assert.h>
#include <public.h>
#include <boardapi.h>
#include <dbsapi.h>
//#include "sm2dbsMutex.h"
#include "systemStsControl.h"
#include "sysindiProcessor.h"

/* ��DBS  ͨѶ���豸�ļ�*/
static T_DBS_DEV_INFO *dbsdev = NULL;

/* ���̵߳���Ϣ���Կ��ر���*/
static int SYSINDIPROCESSOR_MSG_DEBUG_ENABLE = 0;

/* ���̶߳������Ϣ�շ��ӿ�*/
static BBLOCK_QUEUE sysindi_bblock;

#define __sysindi_debug(...);	\
{	\
	if( SYSINDIPROCESSOR_MSG_DEBUG_ENABLE )	\
		fprintf(stderr, __VA_ARGS__);	\
}

/********************************************************************************************
*	��������:__sysindi_msg
*	��������:������API����ʮ�����Ƶķ�ʽ�������������������
*				   �ļ�ָ��fpָ����豸�ļ�
*	����:frank
*	ʱ��:2010-08-13
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
*	��������:sysindi_reply
*	��������:�ú���ΪCMM�ⲿӦ������CMM������
*				  ���֮��ͨ���ú��������������ظ�������
*	����:frank
*	ʱ��:2010-08-13
*********************************************************************************************/
void sysindi_reply(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	int sendn = 0;

	/* ��ӡ���͵����ݱ���*/
	__sysindi_debug("\n<==== SYSMONITOR SEND MASSAGE:\n");
	__sysindi_msg(this->b, this->blen, stderr);
	
	/* ��������Ϣ���͸������� */
	sendn = sendto(this->sk.sk, this->b, this->blen, 0, (struct sockaddr *)&(this->sk.skaddr), sizeof(this->sk.skaddr));
	if( sendn <= 0 )
	{
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "sysindi_reply sendto error");
	}
}

/********************************************************************************************
*	��������:sysindi_error
*	��������:�ú���Ϊ�쳣������������
*	����:frank
*	ʱ��:2010-12-07
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

	/*����UDP SOCKET�ӿ�*/
	if( ( sk->sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}
	else
	{
		bzero((char *)&(sk->skaddr), sizeof(struct sockaddr_in));
       	server_addr.sin_family = PF_INET;
       	server_addr.sin_port = htons(SYSMONITOR_LISTEN_PORT);		/* Ŀ�Ķ˿ں�*/
       	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);			/* Ŀ�ĵ�ַ*/
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

	/* DBS��Ʊ����֧�ֶ��̲߳�������ʼ��ʱ��ÿ���߳��ж�����Ϣ�ӿ�*/
	/*���������ݿ�ģ�黥��ͨѶ���ⲿSOCKET�ӿ�*/
	dbsdev = dbsNoWaitOpen(MID_SYSINDI);
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: sysindiProcessor->dbsOpen error, exited !\n");
		return (void *)0;
	}

	/* �����ⲿͨѶUDP SOCKET */
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
		
		/* ��ӡ���յ��ı���*/
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

		/* �÷�֧����е�����һ��case�Ĵ���������Ӧ������ */
		switch(h->usMsgType)
		{
			case CMM_SYSMONITOR_LED_CONTROL:
			{
				sysindi_reply(sysindi_ProcessSysledCtl(this));
				break;
			}			
			default:
			{
				/* ���ڲ�֧�ֵ���Ϣ����Ӧ�ø���Ӧ���Ա���������֪�� */
				dbs_sys_log(dbsdev, DBS_LOG_ERR, "sysMonitor->sysindiProcessor: CMM_UNKNOWN_MMTYPE");
				sysindi_reply(sysindi_error(this, CMM_UNKNOWN_MMTYPE));
				break;
			}
		}
	}	

	/* ��Ҫ�����������Ӵ��룬ִ�в�����*/
	printf("thread sysindiProcessor exit !\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "INFO: thread sysindiProcessor exit");
	sysindi_destroy();
	dbsClose(dbsdev);
	return (void *)0;
}

