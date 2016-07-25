#include <assert.h>
#include "cmm_tm.h"

T_UDP_SK_INFO SK_CMM_TM;

int __cmm_tm_communicate(uint8_t *buf, uint32_t len)
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
	T_UDP_SK_INFO *sk = &SK_CMM_TM;

	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;
	uint16_t msgType = req->HEADER.usMsgType;

	sendn = sendto(sk->sk, buf, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		fprintf(stderr, "ERROR: cmm communicate with tm sendto failed\n");
		return -1;
	}

	while(1)
	{
		// initialize file descriptor set
		FD_ZERO(&fdsr);
		FD_SET(sk->sk, &fdsr);

		// timeout setting
		tv.tv_sec = 15;
		tv.tv_usec = 0;
		
		maxsock = sk->sk;

		ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
		if( ret <= 0 )
		{
			fprintf(stderr, "ERROR: cmm communicate with tm select failed\n");
			return -1;
		}
		
		// check whether a new connection comes
		if (FD_ISSET(sk->sk, &fdsr))
		{
			FromAddrSize = sizeof(from);
			rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);

			if ( -1 == rev_len )
			{
				fprintf(stderr, "ERROR: cmm communicate with tm recvfrom failed\n");
				return -1;
			}
			else
			{
				r = (T_COM_MSG_HEADER_ACK *)buf;
				if( r->usMsgType != msgType )
				{
					fprintf(stderr, "WARNNING: cmm communicate with tm msgType[%d != %d], [continue] !\n", 
						r->usMsgType, msgType);
					continue;
				}
				if( r->result )
				{
					fprintf(stderr, "ERROR: cmm communicate with tm result = %d\n", r->result);
				}
				return r->result;
			}			
		}
		else
		{
			fprintf(stderr, "ERROR: cmm communicate with tm FD_ISSET failed\n");
			return -1;
		}
	}
}

int cmmTmGetCnuProfile(uint16_t cltId, uint16_t cnuId, st_dbsProfile* profile)
{
	uint32_t len = 0;
	uint8_t buf[MAX_UDP_SIZE] = {0};

	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;	
	stTmUserInfo *tmUser = (stTmUserInfo *)(req->BUF);

	T_COM_MSG_PACKET_ACK *ack = (T_COM_MSG_PACKET_ACK *)buf;	

	req->HEADER.usSrcMID = MID_CMM;
	req->HEADER.usDstMID = MID_TEMPLATE;
	req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
	req->HEADER.ulRequestID = 0;
	req->HEADER.ucFlag = 0;
	req->HEADER.usMsgType = TM_CORE_GET_USER_CONF;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);

	tmUser->clt = cltId;
	tmUser->cnu = cnuId;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	if( CMM_SUCCESS == __cmm_tm_communicate(buf, len) )
	{
		memcpy(profile, ack->BUF, ack->HEADER.ulBodyLength);
		return CMM_SUCCESS;
	}
	else
	{
		return CMM_FAILED;
	}
}

int cmmTmWriteCnuProfile(uint16_t cltId, uint16_t cnuId, st_dbsProfile* profile)
{
	uint32_t len = 0;
	uint8_t buf[MAX_UDP_SIZE] = {0};

	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;	

	req->HEADER.usSrcMID = MID_CMM;
	req->HEADER.usDstMID = MID_TEMPLATE;
	req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
	req->HEADER.ulRequestID = 0;
	req->HEADER.ucFlag = 0;
	req->HEADER.usMsgType = TM_CORE_WRITE_USER_CONF;
	req->HEADER.ulBodyLength = sizeof(st_dbsProfile);

	memcpy(req->BUF, profile, req->HEADER.ulBodyLength);

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	return __cmm_tm_communicate(buf, len);
}

int cmmTmDoCnuPermit(uint16_t cltId, uint16_t cnuId)
{
	uint32_t len = 0;
	uint8_t buf[MAX_UDP_SIZE] = {0};

	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;	
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	req->HEADER.usSrcMID = MID_CMM;
	req->HEADER.usDstMID = MID_TEMPLATE;
	req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
	req->HEADER.ulRequestID = 0;
	req->HEADER.ucFlag = 0;
	req->HEADER.usMsgType = TM_CORE_USER_ADD;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);

	req_data->clt = cltId;
	req_data->cnu = cnuId;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	return __cmm_tm_communicate(buf, len);
}

int cmmTmUndoCnuPermit(uint16_t cltId, uint16_t cnuId)
{
	uint32_t len = 0;
	uint8_t buf[MAX_UDP_SIZE] = {0};

	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;	
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	req->HEADER.usSrcMID = MID_CMM;
	req->HEADER.usDstMID = MID_TEMPLATE;
	req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
	req->HEADER.ulRequestID = 0;
	req->HEADER.ucFlag = 0;
	req->HEADER.usMsgType = TM_CORE_USER_DEL;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);

	req_data->clt = cltId;
	req_data->cnu = cnuId;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	return __cmm_tm_communicate(buf, len);
}

int cmmTmDeleteCnu(uint16_t cltId, uint16_t cnuId)
{
	uint32_t len = 0;
	uint8_t buf[MAX_UDP_SIZE] = {0};

	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;	
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	req->HEADER.usSrcMID = MID_CMM;
	req->HEADER.usDstMID = MID_TEMPLATE;
	req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
	req->HEADER.ulRequestID = 0;
	req->HEADER.ucFlag = 0;
	req->HEADER.usMsgType = TM_CORE_DEL_USER;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);

	req_data->clt = cltId;
	req_data->cnu = cnuId;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	return __cmm_tm_communicate(buf, len);
}

int cmmTmDoWlistControl(uint16_t status)
{
	uint32_t len = 0;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;	

	req->HEADER.usSrcMID = MID_CMM;
	req->HEADER.usDstMID = MID_TEMPLATE;
	req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
	req->HEADER.ulRequestID = 0;
	req->HEADER.ucFlag = 0;
	req->HEADER.usMsgType = status?TM_CORE_SET_WLIST_ON:TM_CORE_SET_WLIST_OFF;
	req->HEADER.ulBodyLength = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __cmm_tm_communicate(buf, len);
}

int cmmTmDumpCnuReg(uint16_t cltId, uint16_t cnuId)
{
	uint32_t len = 0;
	uint8_t buf[MAX_UDP_SIZE] = {0};

	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;	
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	req->HEADER.usSrcMID = MID_CMM;
	req->HEADER.usDstMID = MID_TEMPLATE;
	req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
	req->HEADER.ulRequestID = 0;
	req->HEADER.ucFlag = 0;
	req->HEADER.usMsgType = TM_CORE_DUMP_CNU_REG;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);

	req_data->clt = cltId;
	req_data->cnu = cnuId;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	return __cmm_tm_communicate(buf, len);
}

int cmmTmDumpCnuMod(uint16_t cltId, uint16_t cnuId)
{
	uint32_t len = 0;
	uint8_t buf[MAX_UDP_SIZE] = {0};

	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;	
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	req->HEADER.usSrcMID = MID_CMM;
	req->HEADER.usDstMID = MID_TEMPLATE;
	req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
	req->HEADER.ulRequestID = 0;
	req->HEADER.ucFlag = 0;
	req->HEADER.usMsgType = TM_CORE_DUMP_CNU_MOD;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);

	req_data->clt = cltId;
	req_data->cnu = cnuId;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	return __cmm_tm_communicate(buf, len);
}

int cmmTmDumpCnuPib(uint16_t cltId, uint16_t cnuId)
{
	uint32_t len = 0;
	uint8_t buf[MAX_UDP_SIZE] = {0};

	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;	
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	req->HEADER.usSrcMID = MID_CMM;
	req->HEADER.usDstMID = MID_TEMPLATE;
	req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
	req->HEADER.ulRequestID = 0;
	req->HEADER.ucFlag = 0;
	req->HEADER.usMsgType = TM_CORE_DUMP_CNU_PIB;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);

	req_data->clt = cltId;
	req_data->cnu = cnuId;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	return __cmm_tm_communicate(buf, len);
}

int destroy_cmm_tm(void)
{
	T_UDP_SK_INFO *sk = &SK_CMM_TM;
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int init_cmm_tm(void)
{
	T_UDP_SK_INFO *sk = &SK_CMM_TM;
	
	if( ( sk->sk = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}
	sk->skaddr.sin_family = AF_INET;
	sk->skaddr.sin_port = htons(TM_CORE_LISTEN_PORT);		/* 目的端口号*/
	sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");		/* 目的地址*/
	return CMM_SUCCESS;
}


