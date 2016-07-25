/*****************************************************************************************
  文件名称 : reg_tm.c
  文件描述 : 注册模块与tm模块连接的处理函数
  修订记录 :
           1 创建 : frank
             日期 : 2010-12-07
             描述 : 创建文件

 *****************************************************************************************/
#include <assert.h>
#include "reg_tm.h"

T_UDP_SK_INFO SK_TM;

int msg_tm_init(void)
{
	T_UDP_SK_INFO *sk = &SK_TM;
	
	if( ( sk->sk = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}
	sk->skaddr.sin_family = AF_INET;
	sk->skaddr.sin_port = htons(TM_CORE_LISTEN_PORT);		/* 目的端口号*/
	sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");		/* 目的地址*/
	return CMM_SUCCESS;
}

int msg_tm_destroy(void)
{
	T_UDP_SK_INFO *sk = &SK_TM;
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int msg_reg_tm_communicate(uint8_t *buf, uint32_t len)
{
	fd_set fdsr;
	int maxsock;
	struct timeval tv;
	int ret = 0;
	int sendn = 0;
	T_COM_MSG_HEADER_ACK *r = NULL;
	struct sockaddr_in from;
	int FromAddrSize = 0;
	int rev_len = 0;
	T_UDP_SK_INFO *sk = &SK_TM;	

	sendn = sendto(sk->sk, buf, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		perror("msg_reg_tm_communicate call sendto error.\n");
		return -1;
	}
	
	// initialize file descriptor set
	FD_ZERO(&fdsr);
	FD_SET(sk->sk, &fdsr);

	// timeout setting
	tv.tv_sec = 3;
	tv.tv_usec = 0;
	
	maxsock = sk->sk;

	ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
	if( ret <= 0 )
	{
		printf("msg_reg_tm_communicate select time out\n");
		return -1;
	}
	
	// check whether a new connection comes
	if (FD_ISSET(sk->sk, &fdsr))
	{
		FromAddrSize = sizeof(from);
		rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);

		if ( -1 == rev_len )
		{
			perror("msg_reg_tm_communicate call recvfrom error.\n");
			return -1;
		}
		else
		{
			r = (T_COM_MSG_HEADER_ACK *)buf;
			if( r->result )
			{
				printf("msg_reg_tm_communicate recieved server apply: result = %d\n", r->result);
			}
			return r->result;
		}			
	}
	else
	{
		printf("msg_reg_tm_communicate fd is not setted\n");
		return -1;
	}
}

#if 0
int tm_get_cnu_tid(uint32_t clt, uint32_t cnu, uint32_t *tid)
{
	int len = 0;
	int opt_sts = 0;
	T_COM_MSG_HEADER_REQ request;
	T_COM_MSG_PACKET_ACK *confirm = NULL;
	stTmNewUserInfo st_CnuTidInfo;
	stTmNewUserInfo *pValue = NULL;
	uint8_t buf[MAX_UDP_SIZE];

	request.ucMsgAttrib = MSG_ATTRIB_REQ;
	request.ulRequestID = 0;
	request.usSrcMID = MID_REGISTER;
	request.usDstMID = MID_TEMPLATE;
	request.usMsgType = TM_CORE_GET_CNU_TID;
	request.ucFlag = 0;
	request.ulBodyLength = sizeof(stTmNewUserInfo);

	st_CnuTidInfo.clt = clt;
	st_CnuTidInfo.cnu = cnu;
	st_CnuTidInfo.tid = 0;

	memcpy(buf, &request, sizeof(T_COM_MSG_HEADER_REQ));
	memcpy(buf+sizeof(T_COM_MSG_HEADER_REQ), &st_CnuTidInfo, sizeof(stTmNewUserInfo));
	len = sizeof(T_COM_MSG_HEADER_REQ) + sizeof(stTmNewUserInfo);

	opt_sts = msg_reg_tm_communicate(buf, len);
	if( CMM_SUCCESS == opt_sts )
	{
		confirm = (T_COM_MSG_PACKET_ACK *)buf;
		pValue = (stTmNewUserInfo *)confirm->BUF;
		*tid = pValue->tid;
	}
	else
	{
		*tid = 0;
	}
	return opt_sts;
}
#endif

int tm_get_user_pib_crc(uint32_t clt, uint32_t cnu, uint32_t *Crc)
{
	int len = 0;
	int opt_sts = 0;
	T_COM_MSG_HEADER_REQ request;
	T_COM_MSG_PACKET_ACK *confirm = NULL;
	stAutoGenCrc stCrc;
	stAutoGenCrc *pValue = NULL;
	uint8_t buf[MAX_UDP_SIZE];

	request.ucMsgAttrib = MSG_ATTRIB_REQ;
	request.ulRequestID = 0;
	request.usSrcMID = MID_REGISTER;
	request.usDstMID = MID_TEMPLATE;
	request.usMsgType = TM_CORE_GET_CRC;
	request.ucFlag = 0;
	request.ulBodyLength = sizeof(stAutoGenCrc);

	stCrc.clt = clt;
	stCrc.cnu = cnu;
	stCrc.flag = TM_AUTO_GEN_PIB;

	memcpy(buf, &request, sizeof(T_COM_MSG_HEADER_REQ));
	memcpy(buf+sizeof(T_COM_MSG_HEADER_REQ), &stCrc, sizeof(stAutoGenCrc));
	len = sizeof(T_COM_MSG_HEADER_REQ) + sizeof(stAutoGenCrc);

	opt_sts = msg_reg_tm_communicate(buf, len);
	if( CMM_SUCCESS == opt_sts )
	{
		confirm = (T_COM_MSG_PACKET_ACK *)buf;
		pValue = (stAutoGenCrc *)confirm->BUF;
		*Crc = pValue->crc;
	}
	else
	{
		*Crc = 0;
	}
	return opt_sts;
}

#if 0
int tm_get_user_mod_crc(uint32_t clt, uint32_t cnu, uint32_t *Crc)
{
	int len = 0;
	int opt_sts = 0;
	T_COM_MSG_HEADER_REQ request;
	T_COM_MSG_PACKET_ACK *confirm = NULL;
	stAutoGenCrc stCrc;
	stAutoGenCrc *pValue = NULL;
	uint8_t buf[MAX_UDP_SIZE];

	request.ucMsgAttrib = MSG_ATTRIB_REQ;
	request.ulRequestID = 0;
	request.usSrcMID = MID_REGISTER;
	request.usDstMID = MID_TEMPLATE;
	request.usMsgType = TM_CORE_GET_CRC;
	request.ucFlag = 0;
	request.ulBodyLength = sizeof(stAutoGenCrc);

	stCrc.clt = clt;
	stCrc.cnu = cnu;
	stCrc.flag = TM_AUTO_GEN_MOD;

	memcpy(buf, &request, sizeof(T_COM_MSG_HEADER_REQ));
	memcpy(buf+sizeof(T_COM_MSG_HEADER_REQ), &stCrc, sizeof(stAutoGenCrc));
	len = sizeof(T_COM_MSG_HEADER_REQ) + sizeof(stAutoGenCrc);

	opt_sts = msg_reg_tm_communicate(buf, len);
	if( CMM_SUCCESS == opt_sts )
	{
		confirm = (T_COM_MSG_PACKET_ACK *)buf;
		pValue = (stAutoGenCrc *)confirm->BUF;
		*Crc = pValue->crc;
	}
	else
	{
		*Crc = 0;
	}
	return opt_sts;
}
#endif

int tm_gen_user_pib(uint32_t clt, uint32_t cnu)
{
	int len = 0;
	//int opt_sts = 0;
	T_COM_MSG_HEADER_REQ request;
	//T_COM_MSG_PACKET_ACK *confirm = NULL;
	stAutoGenConfig stCon;
	uint8_t buf[MAX_UDP_SIZE];

	request.ucMsgAttrib = MSG_ATTRIB_REQ;
	request.ulRequestID = 0;
	request.usSrcMID = MID_REGISTER;
	request.usDstMID = MID_TEMPLATE;
	request.usMsgType = TM_CORE_GEN_CONF;
	request.ucFlag = 0;
	request.ulBodyLength = sizeof(stAutoGenConfig);

	stCon.clt = clt;
	stCon.cnu = cnu;
	stCon.flag = TM_AUTO_GEN_PIB;

	memcpy(buf, &request, sizeof(T_COM_MSG_HEADER_REQ));
	memcpy(buf+sizeof(T_COM_MSG_HEADER_REQ), &stCon, sizeof(stAutoGenConfig));
	len = sizeof(T_COM_MSG_HEADER_REQ) + sizeof(stAutoGenConfig);

	return msg_reg_tm_communicate(buf, len);
}

int tm_gen_user_mod(uint32_t clt, uint32_t cnu)
{
	int len = 0;
	//int opt_sts = 0;
	T_COM_MSG_HEADER_REQ request;
	stAutoGenConfig stCon;
	uint8_t buf[MAX_UDP_SIZE];

	request.ucMsgAttrib = MSG_ATTRIB_REQ;
	request.ulRequestID = 0;
	request.usSrcMID = MID_REGISTER;
	request.usDstMID = MID_TEMPLATE;
	request.usMsgType = TM_CORE_GEN_CONF;
	request.ucFlag = 0;
	request.ulBodyLength = sizeof(stAutoGenConfig);

	stCon.clt = clt;
	stCon.cnu = cnu;
	stCon.flag = TM_AUTO_GEN_MOD;

	memcpy(buf, &request, sizeof(T_COM_MSG_HEADER_REQ));
	memcpy(buf+sizeof(T_COM_MSG_HEADER_REQ), &stCon, sizeof(stAutoGenConfig));
	len = sizeof(T_COM_MSG_HEADER_REQ) + sizeof(stAutoGenConfig);

	return msg_reg_tm_communicate(buf, len);
}

int tm_distroy_user_pib(uint32_t clt, uint32_t cnu)
{
	int len = 0;
	//int opt_sts = 0;
	T_COM_MSG_HEADER_REQ request;
	stAutoGenConfig stConf;
	uint8_t buf[MAX_UDP_SIZE];

	request.ucMsgAttrib = MSG_ATTRIB_REQ;
	request.ulRequestID = 0;
	request.usSrcMID = MID_REGISTER;
	request.usDstMID = MID_TEMPLATE;
	request.usMsgType = TM_CORE_DESTROY_CONF;
	request.ucFlag = 0;
	request.ulBodyLength = sizeof(stAutoGenConfig);

	stConf.clt = clt;
	stConf.cnu = cnu;
	stConf.flag = TM_AUTO_GEN_PIB;

	memcpy(buf, &request, sizeof(T_COM_MSG_HEADER_REQ));
	memcpy(buf+sizeof(T_COM_MSG_HEADER_REQ), &stConf, sizeof(stAutoGenConfig));
	len = sizeof(T_COM_MSG_HEADER_REQ) + sizeof(stAutoGenConfig);

	return msg_reg_tm_communicate(buf, len);
}

int tm_distroy_user_mod(uint32_t clt, uint32_t cnu)
{
	int len = 0;
	//int opt_sts = 0;
	T_COM_MSG_HEADER_REQ request;
	stAutoGenConfig stConf;
	uint8_t buf[MAX_UDP_SIZE];

	request.ucMsgAttrib = MSG_ATTRIB_REQ;
	request.ulRequestID = 0;
	request.usSrcMID = MID_REGISTER;
	request.usDstMID = MID_TEMPLATE;
	request.usMsgType = TM_CORE_DESTROY_CONF;
	request.ucFlag = 0;
	request.ulBodyLength = sizeof(stAutoGenConfig);

	stConf.clt = clt;
	stConf.cnu = cnu;
	stConf.flag = TM_AUTO_GEN_MOD;

	memcpy(buf, &request, sizeof(T_COM_MSG_HEADER_REQ));
	memcpy(buf+sizeof(T_COM_MSG_HEADER_REQ), &stConf, sizeof(stAutoGenConfig));
	len = sizeof(T_COM_MSG_HEADER_REQ) + sizeof(stAutoGenConfig);

	return msg_reg_tm_communicate(buf, len);
}

