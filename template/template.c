/*****************************************************************************************
  文件名称 : template.c
  文件描述 : 模板管理模块主入口函数
  				主要处理来自外部模块的数据库读写请求。
  修订记录 :
           1 创建 : frank
             日期 : 2010-11-03
             描述 : 创建文件

 *****************************************************************************************/
#include <assert.h>
#include <signal.h>
#include "template.h"
#include "tm_interface.h"
#include "tm_reg.h"
#include <dbsapi.h>
#include <boardapi.h>

BBLOCK_QUEUE bblock;
T_UDP_SK_INFO CMM_SK;
int TEMPLATE_DEBUG_ENABLE = 0;

/* 与DBS  通讯的设备文件*/
T_DBS_DEV_INFO *dbsdev = NULL;

/********************************************************************************************
*	函数名称:debug_dump_msg
*	函数功能:调试用API，以十六进制的方式将缓冲区的内容输出到
*				   文件指针fp指向的设备文件
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void __tm_msg(const unsigned char memory [], size_t length, FILE *fp)
{
	if(TEMPLATE_DEBUG_ENABLE)
	{
		__tm_debug("----------------------------------------------------------------------\n");
		hexdump(memory, length, fp);
		__tm_debug("\n----------------------------------------------------------------------\n");
	}
}

/********************************************************************************************
*	函数名称:TmCoreProcessAck
*	函数功能:该函数为外部应答函数，请求处理
*				  完成之后通过该函数将处理结果返回给请求者
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
void TmCoreProcessAck(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	int sendn = 0;

	/* 打印发送的数据报文*/
	__tm_debug("\n<==== TM CORE SEND MASSAGE:\n");
	__tm_msg(this->b, this->blen, stderr);
	
	/* 将处理信息发送给请求者 */
	sendn = sendto(this->sk.sk, this->b, this->blen, 0, (struct sockaddr *)&(this->sk.skaddr), sizeof(this->sk.skaddr));
	if( sendn <= 0 )
	{
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "tm->TmCoreProcessAck->sendto");
	}
}

/********************************************************************************************
*	函数名称:TmCoreError
*	函数功能:该函数为异常处理的组包函数
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreError(BBLOCK_QUEUE *this, uint32_t ErrorCode)
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

/********************************************************************************************
*	函数名称:TmCoreAutoGenCrc
*	函数功能:根据输入的CNU信息，自动生成该CNU的配置一致性校验码
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreAutoGenCrc(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint32_t ret = CMM_FAILED;
	T_COM_MSG_PACKET_REQ *request = (T_COM_MSG_PACKET_REQ *)(this->b);
	T_COM_MSG_HEADER_ACK confirm;	
	stAutoGenCrc stCrc;

	if( sizeof(stAutoGenCrc) == request->HEADER.ulBodyLength )
	{
		memcpy((void *)&stCrc, (void *)request->BUF, request->HEADER.ulBodyLength);
		ret = tm_gen_crc(&stCrc);
	}

	confirm.ucMsgAttrib = MSG_ATTRIB_ACK;
	confirm.ulRequestID = request->HEADER.ulRequestID;
	confirm.usSrcMID = request->HEADER.usDstMID;
	confirm.usDstMID = request->HEADER.usSrcMID;
	confirm.usMsgType = request->HEADER.usMsgType;
	confirm.ucFlag = 0;
	confirm.result = ret;
	confirm.ulBodyLength = sizeof(stAutoGenCrc);
	
	this->blen = sizeof(confirm)+sizeof(stAutoGenCrc);
	memcpy((void *)this->b, (void *)&confirm, sizeof(confirm));
	memcpy((void *)((this->b)+sizeof(confirm)), (void *)&stCrc, sizeof(stAutoGenCrc));
	
	return this;
}

/********************************************************************************************
*	函数名称:TmCoreAutoGenConfig
*	函数功能:根据输入的CNU信息，自动生成该CNU的配置
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreAutoGenConfig(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint32_t ret = CMM_FAILED;
	T_COM_MSG_PACKET_REQ *request = (T_COM_MSG_PACKET_REQ *)(this->b);
	T_COM_MSG_HEADER_ACK confirm;	
	stAutoGenConfig stConf;

	if( sizeof(stAutoGenConfig) == request->HEADER.ulBodyLength )
	{
		memcpy((void *)&stConf, (void *)request->BUF, request->HEADER.ulBodyLength);
		ret = tm_gen_conf(&stConf);
	}

	confirm.ucMsgAttrib = MSG_ATTRIB_ACK;
	confirm.ulRequestID = request->HEADER.ulRequestID;
	confirm.usSrcMID = request->HEADER.usDstMID;
	confirm.usDstMID = request->HEADER.usSrcMID;
	confirm.usMsgType = request->HEADER.usMsgType;
	confirm.ucFlag = 0;
	confirm.result = ret;
	confirm.ulBodyLength = sizeof(stAutoGenConfig);
	
	this->blen = sizeof(confirm)+sizeof(stAutoGenConfig);
	memcpy((void *)this->b, (void *)&confirm, sizeof(confirm));
	memcpy((void *)((this->b)+sizeof(confirm)), (void *)&stConf, sizeof(stAutoGenConfig));
	
	return this;
}

/********************************************************************************************
*	函数名称:TmCoreDestroyConfig
*	函数功能:根据输入的CNU信息，销毁之前为该CNU自动生成的配置
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreDestroyConfig(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint32_t ret = CMM_FAILED;
	T_COM_MSG_PACKET_REQ *request = (T_COM_MSG_PACKET_REQ *)(this->b);
	T_COM_MSG_HEADER_ACK confirm;	
	stAutoGenConfig stConf;

	if( sizeof(stAutoGenConfig) == request->HEADER.ulBodyLength )
	{
		memcpy((void *)&stConf, (void *)request->BUF, request->HEADER.ulBodyLength);
		ret = tm_destroy_conf(&stConf);
	}

	confirm.ucMsgAttrib = MSG_ATTRIB_ACK;
	confirm.ulRequestID = request->HEADER.ulRequestID;
	confirm.usSrcMID = request->HEADER.usDstMID;
	confirm.usDstMID = request->HEADER.usSrcMID;
	confirm.usMsgType = request->HEADER.usMsgType;
	confirm.ucFlag = 0;
	confirm.result = ret;
	confirm.ulBodyLength = sizeof(stAutoGenConfig);
	
	this->blen = sizeof(confirm)+sizeof(stAutoGenConfig);
	memcpy((void *)this->b, (void *)&confirm, sizeof(confirm));
	memcpy((void *)((this->b)+sizeof(confirm)), (void *)&stConf, sizeof(stAutoGenConfig));
	
	return this;
}

/********************************************************************************************
*	函数名称:TmCoreMsgDebugEnable
*	函数功能:打开模块的调试消息接口函数
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreMsgDebugEnable(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	T_COM_MSG_HEADER_ACK confirm;
	T_COM_MSG_HEADER_REQ *request = (T_COM_MSG_HEADER_REQ *)(this->b);

	TEMPLATE_DEBUG_ENABLE = 1;

	confirm.ucMsgAttrib = MSG_ATTRIB_ACK;
	confirm.ulRequestID = request->ulRequestID;
	confirm.usSrcMID = request->usDstMID;
	confirm.usDstMID = request->usSrcMID;
	confirm.usMsgType = request->usMsgType;
	confirm.ucFlag = 0;
	confirm.result = CMM_SUCCESS;
	confirm.ulBodyLength = 0;
	
	this->blen = sizeof(confirm);
	memcpy((void *)this->b, (void *)&confirm, this->blen);
	
	return this;
}

/********************************************************************************************
*	函数名称:TmCoreMsgDebugDisable
*	函数功能:关闭模块的调试消息接口函数
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreMsgDebugDisable(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	T_COM_MSG_HEADER_ACK confirm;
	T_COM_MSG_HEADER_REQ *request = (T_COM_MSG_HEADER_REQ *)(this->b);

	TEMPLATE_DEBUG_ENABLE = 0;

	confirm.ucMsgAttrib = MSG_ATTRIB_ACK;
	confirm.ulRequestID = request->ulRequestID;
	confirm.usSrcMID = request->usDstMID;
	confirm.usDstMID = request->usSrcMID;
	confirm.usMsgType = request->usMsgType;
	confirm.ucFlag = 0;
	confirm.result = CMM_SUCCESS;
	confirm.ulBodyLength = 0;
	
	this->blen = sizeof(confirm);
	memcpy((void *)this->b, (void *)&confirm, this->blen);
	
	return this;
}

/********************************************************************************************
*	函数名称:TmCoreNewUser
*	函数功能:根据输入的CNU信息，销毁之前为该CNU自动生成的配置
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreNewUser(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint32_t ret = CMM_FAILED;
	T_COM_MSG_PACKET_REQ *request = (T_COM_MSG_PACKET_REQ *)(this->b);
	T_COM_MSG_HEADER_ACK confirm;
	stTmUserInfo *szTmNewUser = (stTmUserInfo *)(request->BUF);

	ret = tm_add_cnu_into_wlist(szTmNewUser);

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

/********************************************************************************************
*	函数名称:TmCoreDeleteUser
*	函数功能:根据输入的CNU信息，销毁之前为该CNU自动生成的配置
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreDeleteUser(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint32_t ret = CMM_FAILED;
	T_COM_MSG_PACKET_REQ *request = (T_COM_MSG_PACKET_REQ *)(this->b);
	T_COM_MSG_HEADER_ACK confirm;
	stTmUserInfo *szTmUser = (stTmUserInfo *)(request->BUF);

	ret = tm_del_cnu_from_wlist(szTmUser);

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

/********************************************************************************************
*	函数名称:TmCoreDeleteCnu
*	函数功能:根据输入的CNU信息，删除长期不上线的用户
*	作者:may2250
*	时间:2010-12-30
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreDeleteCnu(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint16_t ret = CMM_FAILED;
	T_COM_MSG_PACKET_REQ *request = (T_COM_MSG_PACKET_REQ *)(this->b);
	T_COM_MSG_HEADER_ACK confirm;
	stTmUserInfo *szTmUser = (stTmUserInfo *)(request->BUF);

	ret = tm_del_cnu(szTmUser);

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

/********************************************************************************************
*	函数名称:TmCoreGetUserConfig
*	函数功能:根据输入的CNU信息，销毁之前为该CNU自动生成的配置
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreGetProfile(BBLOCK_QUEUE *this)
{
	assert(NULL != this);
	
	uint32_t ret = CMM_FAILED;
	
	T_COM_MSG_PACKET_REQ *request = (T_COM_MSG_PACKET_REQ *)(this->b);
	stTmUserInfo *tmUser = (stTmUserInfo *)(request->BUF);
	
	T_COM_MSG_HEADER_ACK confirm;
	
	st_dbsProfile profile;

	profile.id = (tmUser->clt - 1)*MAX_CNUS_PER_CLT + tmUser->cnu;

	ret = tm_get_profile(&profile);

	confirm.ucMsgAttrib = MSG_ATTRIB_ACK;
	confirm.ulRequestID = request->HEADER.ulRequestID;
	confirm.usSrcMID = request->HEADER.usDstMID;
	confirm.usDstMID = request->HEADER.usSrcMID;
	confirm.usMsgType = request->HEADER.usMsgType;
	confirm.ucFlag = 0;
	confirm.result = ret;
	confirm.ulBodyLength = sizeof(st_dbsProfile);
	
	this->blen = sizeof(confirm)+sizeof(st_dbsProfile);
	memcpy((void *)this->b, (void *)&confirm, sizeof(confirm));
	memcpy((void *)((this->b)+sizeof(confirm)), (void *)&profile, sizeof(st_dbsProfile));
	
	return this;
}

/********************************************************************************************
*	函数名称:TmCoreGetTmById
*	函数功能:根据输入的CNU信息，销毁之前为该CNU自动生成的配置
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreWriteProfile(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint32_t ret = CMM_FAILED;
	T_COM_MSG_PACKET_REQ *request = (T_COM_MSG_PACKET_REQ *)(this->b);
	st_dbsProfile *profile = (st_dbsProfile *)(request->BUF);
	
	T_COM_MSG_HEADER_ACK confirm;	

	ret = tm_write_profile(profile);

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

/********************************************************************************************
*	函数名称:TmCoreGetWlistSts
*	函数功能:根据输入的CNU信息，销毁之前为该CNU自动生成的配置
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreGetWlistSts(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint32_t ret = CMM_FAILED;
	T_COM_MSG_HEADER_REQ *request = (T_COM_MSG_HEADER_REQ *)(this->b);
	T_COM_MSG_HEADER_ACK confirm;
	uint32_t status = 0;

	ret = tm_get_wlist_status(&status);

	confirm.ucMsgAttrib = MSG_ATTRIB_ACK;
	confirm.ulRequestID = request->ulRequestID;
	confirm.usSrcMID = request->usDstMID;
	confirm.usDstMID = request->usSrcMID;
	confirm.usMsgType = request->usMsgType;
	confirm.ucFlag = 0;
	confirm.result = ret;
	confirm.ulBodyLength = sizeof(uint32_t);
	
	this->blen = sizeof(confirm)+sizeof(uint32_t);
	memcpy((void *)this->b, (void *)&confirm, sizeof(confirm));
	memcpy((void *)((this->b)+sizeof(confirm)), (void *)&status, sizeof(uint32_t));
	
	return this;
}

/********************************************************************************************
*	函数名称:TmCoreWlistEnable
*	函数功能:根据输入的CNU信息，销毁之前为该CNU自动生成的配置
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreWlistEnable(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint32_t ret = CMM_FAILED;
	T_COM_MSG_HEADER_REQ *request = (T_COM_MSG_HEADER_REQ *)(this->b);
	T_COM_MSG_HEADER_ACK confirm;

	ret = tm_enable_wlist();

	confirm.ucMsgAttrib = MSG_ATTRIB_ACK;
	confirm.ulRequestID = request->ulRequestID;
	confirm.usSrcMID = request->usDstMID;
	confirm.usDstMID = request->usSrcMID;
	confirm.usMsgType = request->usMsgType;
	confirm.ucFlag = 0;
	confirm.result = ret;
	confirm.ulBodyLength = 0;
	
	this->blen = sizeof(confirm);
	memcpy((void *)this->b, (void *)&confirm, sizeof(confirm));
	
	return this;
}

/********************************************************************************************
*	函数名称:TmCoreDestroyConfig
*	函数功能:根据输入的CNU信息，销毁之前为该CNU自动生成的配置
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreWlistDisable(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint32_t ret = CMM_FAILED;
	T_COM_MSG_HEADER_REQ *request = (T_COM_MSG_HEADER_REQ *)(this->b);
	T_COM_MSG_HEADER_ACK confirm;

	ret = tm_disable_wlist();

	confirm.ucMsgAttrib = MSG_ATTRIB_ACK;
	confirm.ulRequestID = request->ulRequestID;
	confirm.usSrcMID = request->usDstMID;
	confirm.usDstMID = request->usSrcMID;
	confirm.usMsgType = request->usMsgType;
	confirm.ucFlag = 0;
	confirm.result = ret;
	confirm.ulBodyLength = 0;
	
	this->blen = sizeof(confirm);
	memcpy((void *)this->b, (void *)&confirm, sizeof(confirm));
	
	return this;
}

/********************************************************************************************
*	函数名称:TmCoreDumpCnuRegister
*	函数功能:
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreDumpCnuRegister(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint32_t ret = CMM_FAILED;
	T_COM_MSG_PACKET_REQ *request = (T_COM_MSG_PACKET_REQ *)(this->b);
	stTmUserInfo *szTmUser = (stTmUserInfo *)(request->BUF);
	T_COM_MSG_HEADER_ACK confirm;

	ret = tm_dump_register(szTmUser->clt, szTmUser->cnu);

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

/********************************************************************************************
*	函数名称:TmCoreDumpCnuMod
*	函数功能:
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreDumpCnuMod(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint32_t ret = CMM_FAILED;
	T_COM_MSG_PACKET_REQ *request = (T_COM_MSG_PACKET_REQ *)(this->b);
	stTmUserInfo *szTmUser = (stTmUserInfo *)(request->BUF);
	T_COM_MSG_HEADER_ACK confirm;

	ret = tm_dump_mod(szTmUser->clt, szTmUser->cnu);

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

/********************************************************************************************
*	函数名称:TmCoreDumpCnuPib
*	函数功能:
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreDumpCnuPib(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint32_t ret = CMM_FAILED;
	T_COM_MSG_PACKET_REQ *request = (T_COM_MSG_PACKET_REQ *)(this->b);
	stTmUserInfo *szTmUser = (stTmUserInfo *)(request->BUF);
	T_COM_MSG_HEADER_ACK confirm;

	ret = tm_dump_cnu_pib(szTmUser->clt, szTmUser->cnu);

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

/********************************************************************************************
*	函数名称:TmCoreDumpCltPib
*	函数功能:
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
BBLOCK_QUEUE * TmCoreDumpCltPib(BBLOCK_QUEUE *this)
{
	assert(NULL != this);

	uint32_t ret = CMM_FAILED;
	T_COM_MSG_PACKET_REQ *request = (T_COM_MSG_PACKET_REQ *)(this->b);
	uint32_t *id = (uint32_t *)(request->BUF);
	T_COM_MSG_HEADER_ACK confirm;

	ret = tm_dump_clt_pib(*id);

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

/********************************************************************************************
*	函数名称:close_socket_ext
*	函数功能:销毁外部消息处理接口函数
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
int close_socket_ext(T_UDP_SK_INFO *sk)
{
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:init_socket_ext
*	函数功能:创建外部消息处理接口函数
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
int init_socket_ext(T_UDP_SK_INFO *sk)
{
	struct sockaddr_in server_addr;

	/*创建UDP SOCKET接口*/
	if( ( sk->sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}
	else
	{
		bzero((char *)&(sk->skaddr), sizeof(struct sockaddr_in));
       	server_addr.sin_family = PF_INET;
       	server_addr.sin_port = htons(TM_CORE_LISTEN_PORT);		/* 目的端口号*/
       	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);		/* 目的地址*/
		if (bind(sk->sk, (struct sockaddr *)&server_addr, sizeof(server_addr))<0)
		{
			close_socket_ext(sk);
			return CMM_CREATE_SOCKET_ERROR;
		}
	}
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:processManager
*	函数功能:模块主函数的循环处理消息函数
*	作者:frank
*	时间:2010-12-07
*********************************************************************************************/
void processManager(BBLOCK_QUEUE *this)
{
	T_COM_MSG_HEADER_REQ *h = NULL;
	int FromAddrSize = sizeof(this->sk.skaddr);

	while(1)
	{
		bzero(this->b, MAX_UDP_SIZE);
		this->blen = recvfrom(this->sk.sk, this->b, MAX_UDP_SIZE, 0, 
					(struct sockaddr *)&(this->sk.skaddr), &FromAddrSize);
		
		/* 打印接收到的报文*/
		__tm_debug("\n====> TM CORE RECIEVED MASSAGE:\n");
		__tm_msg(this->b, this->blen, stderr);

		h = (T_COM_MSG_HEADER_REQ *)(this->b);
		if( h->ucMsgAttrib != MSG_ATTRIB_REQ)
		{
			dbs_sys_log(dbsdev, DBS_LOG_WARNING, "tm->processManager: NOT MSG_ATTRIB_REQ");
			continue;
		}
		if( h->usDstMID != MID_TEMPLATE)
		{
			dbs_sys_log(dbsdev, DBS_LOG_WARNING, "tm->processManager: NOT MID_TEMPLATE");
			continue;
		}

		/* 该分支语句中的任意一个case的处理函数都不应该阻塞 */
		switch(h->usMsgType)
		{
			case TM_CORE_MSG_DEBUG_ENABLE:
				TmCoreProcessAck(TmCoreMsgDebugEnable(this));
				break;
			case TM_CORE_MSG_DEBUG_DISABLE:
				TmCoreProcessAck(TmCoreMsgDebugDisable(this));
				break;
			case TM_CORE_GET_CRC:
				TmCoreProcessAck(TmCoreAutoGenCrc(this));
				break;			
			case TM_CORE_GEN_CONF:
				TmCoreProcessAck(TmCoreAutoGenConfig(this));
				break;
			case TM_CORE_DESTROY_CONF:
				TmCoreProcessAck(TmCoreDestroyConfig(this));
				break;
			case TM_CORE_USER_ADD:
				TmCoreProcessAck(TmCoreNewUser(this));
				break;
			case TM_CORE_USER_DEL:
				TmCoreProcessAck(TmCoreDeleteUser(this));
				break;
			case TM_CORE_DEL_USER:
				TmCoreProcessAck(TmCoreDeleteCnu(this));
				break;
			case TM_CORE_GET_USER_CONF:
				TmCoreProcessAck(TmCoreGetProfile(this));
				break;
			case TM_CORE_WRITE_USER_CONF:
				TmCoreProcessAck(TmCoreWriteProfile(this));
				break;
			case TM_CORE_GET_WLIST_STS:
				TmCoreProcessAck(TmCoreGetWlistSts(this));
				break;
			case TM_CORE_SET_WLIST_ON:
				TmCoreProcessAck(TmCoreWlistEnable(this));
				break;
			case TM_CORE_SET_WLIST_OFF:
				TmCoreProcessAck(TmCoreWlistDisable(this));
				break;
			case TM_CORE_DUMP_CNU_REG:
				TmCoreProcessAck(TmCoreDumpCnuRegister(this));
				break;
			case TM_CORE_DUMP_CNU_MOD:
				TmCoreProcessAck(TmCoreDumpCnuMod(this));
				break;
			case TM_CORE_DUMP_CNU_PIB:
				TmCoreProcessAck(TmCoreDumpCnuPib(this));
				break;
			case TM_CORE_DUMP_CLT_PIB:
				TmCoreProcessAck(TmCoreDumpCltPib(this));
				break;
			#if 0
			case TM_CORE_GET_TM_BY_ID:
				TmCoreProcessAck(TmCoreGetTmById(this));
				break;
			case TM_CORE_WRITE_TM_BY_ID:
				TmCoreProcessAck(TmCoreWriteTmById(this));
				break;

			case TM_CORE_GET_GLOBAL_AUTO_CONFIG_STS:
				TmCoreProcessAck(TmCoreGetAutoConfigSts(this));
				break;
			case TM_CORE_SET_GLOBAL_AUTO_CONFIG_EN:
				TmCoreProcessAck(TmCoreSetAutoConfigEn(this));
				break;
			case TM_CORE_SET_GLOBAL_AUTO_CONFIG_DIS:
				TmCoreProcessAck(TmCoreSetAutoConfigDis(this));
				break;
			case TM_CORE_GET_USER_AUTO_CONFIG_STS:
				TmCoreProcessAck(TmCoreGetUserAutoConfigSts(this));
				break;
			case TM_CORE_SET_USER_AUTO_CONFIG_EN:
				TmCoreProcessAck(TmCoreSetUserAutoConfigEn(this));
				break;
			case TM_CORE_SET_USER_AUTO_CONFIG_DIS:
				TmCoreProcessAck(TmCoreSetUserAutoConfigDis(this));
				break;			
			case TM_CORE_DEL_TM_BY_ID:
				TmCoreProcessAck(TmCoreDeleteTmById(this));
				break;			
			case TM_CORE_NEW_TM:
				TmCoreProcessAck(TmCoreNewTm(this));
				break;			
			case TM_CORE_USER_EDIT:
				TmCoreProcessAck(TmCoreRedefinitionUser(this));
				break;
			
			case TM_CORE_GET_CNU_TID:
				TmCoreProcessAck(TmCoreGetCnuTid(this));
				break;
			#endif
			default:
				/* 对于不支持的消息类型应该给予应答以便让请求者知道 */
				dbs_sys_log(dbsdev, DBS_LOG_ERR, "tm->processManager: CMM_UNKNOWN_MMTYPE");
				TmCoreProcessAck(TmCoreError(this, CMM_UNKNOWN_MMTYPE));
				break;
		}
	}
}

/********************************************************************************************
*	函数名称:SignalProcessHandle
*	函数功能:异常处理句柄函数，当进程退出时该函数被调度
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void TmSignalProcessHandle(int n)
{
	BBLOCK_QUEUE *this = &bblock;
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "TmSignalProcessHandle: module tm exit");
	close_socket_ext(&(this->sk));
	tm2reg_destroy();
	dbsClose(dbsdev);	
	exit(0);
}

/********************************************************************************************
*	函数名称:main
*	函数功能:TM CORE主入口函数，接收来自外部模块的请求，
*				   完成业务逻辑的处理
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
int main(void)
{
	BBLOCK_QUEUE *this = &bblock;

	/*创建与数据库模块互斥通讯的外部SOCKET接口*/
	dbsdev = dbsOpen(MID_TEMPLATE);
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: tmCore->dbsOpen error, exited !\n");
		return CMM_CREATE_SOCKET_ERROR;
	}
	
	/* 创建外部通讯UDP SOCKET */
	if( CMM_SUCCESS != init_socket_ext(&(this->sk)) )
	{
		fprintf(stderr, "ERROR: tm->init_socket_ext, exit !\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "tm init_socket_ext error, exit");
		dbsClose(dbsdev);
		return CMM_CREATE_SOCKET_ERROR;
	}

	/* 创建与注册模块通讯的消息接口*/
	if( CMM_SUCCESS != tm2reg_init() )
	{
		fprintf(stderr, "ERROR: tm->tm2reg_init, exit !\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "tm tm2reg_init error, exit");
		close_socket_ext(&(this->sk));
		dbsClose(dbsdev);
		return CMM_CREATE_SOCKET_ERROR;
	}
	
	/* 注册异常退出句柄函数*/
	signal(SIGTERM, TmSignalProcessHandle);

	dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting module tm success");
	printf("Starting module TMCore		......		[OK]\n");

	/* 循环处理外部请求*/
	processManager(this);

	/* 不要在这个后面添加代码，执行不到滴*/
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "module tm exit");
	tm2reg_destroy();
	close_socket_ext(&(this->sk));
	dbsClose(dbsdev);	
	return 0;
}


