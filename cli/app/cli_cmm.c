#include <dbsapi.h>
#include "../cli_pub.h"
#include "../util/cli_io.h"
#include "cli_cmm.h"
#include <boardapi.h>

static T_UDP_SK_INFO SK_CLI2CMM;

extern T_DBS_DEV_INFO *dbsdev;
#if 0
int __cli2cmm_send(unsigned short MsgType, unsigned char *buf, unsigned int len)
{
	int sendn = 0;
	T_Msg_Header_CMM h;
	char b[MAX_UDP_SIZE] = {0};
	T_UDP_SK_INFO *sk = &SK_CLI2CMM;

	h.usSrcMID = MID_CLI;
	h.usDstMID = MID_CMM;
	h.usMsgType = MsgType;
	h.fragment = 0;
	h.ulBodyLength = len;

	sendn = sizeof(h)+len;
	if( sendn > MAX_UDP_SIZE )
	{
		perror("msg_send error: buffer is too big.\n");
		return TBS_FAILED;
	}
	bzero((unsigned char *)b, MAX_UDP_SIZE);
	memcpy(b, &h, sizeof(h));
	if( 0 != len)
	{
		memcpy(b+sizeof(h), buf, len);
	}
	if( -1 == sendto(sk->sk, b, sendn, 0, (struct sockaddr *)&(sk->skaddr), sizeof(struct sockaddr)) )
	{
		perror("msg_send sendto error\n");
		return TBS_FAILED;
	}
	return TBS_SUCCESS;
}

int __cli2cmm_receive(unsigned short MsgType, unsigned char *buf, int len)
{
	int ret = 0;
	fd_set fdsr;
	struct timeval tv;
	unsigned char buffer[MAX_UDP_SIZE];
	struct sockaddr_in from;
	int FromAddrSize = 0;
	int rev_len = 0;
	T_REQ_Msg_CMM *r = NULL;
	bzero(buffer, MAX_UDP_SIZE);
	T_UDP_SK_INFO *sk = &SK_CLI2CMM;

	// initialize file descriptor set
	FD_ZERO(&fdsr);
	FD_SET(sk->sk, &fdsr);

	// timeout setting
	tv.tv_sec = 15;
	tv.tv_usec = 0;

	//¼ì²âsocket
	ret = select(sk->sk + 1, &fdsr, NULL, NULL, &tv);
	if( ret <= 0 )
	{
		//perror("Select time out, continue\n");
		return CMM_SYS_BUSY<<8;
	}
	// check whether a new connection comes
	if (FD_ISSET(sk->sk, &fdsr))
	{
		FromAddrSize = sizeof(from);
		
		rev_len = recvfrom(sk->sk, buffer, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);
		if ( -1 == rev_len )
		{
			perror("msg call recvfrom error.\n");
			return CMM_FAILED<<8;
		}
		else
		{
			r = (T_REQ_Msg_CMM *)buffer;
			if( MsgType != r->HEADER.usMsgType )
			{
				return CMM_FAILED<<8;
			}
			else if( r->result != CMM_SUCCESS )
			{
				return (TBS_SUCCESS<<8)| (r->result);
			}
			else
			{
				if(len !=0)
				{
					bzero(buf, len);
					memcpy(buf, r->BUF, r->HEADER.ulBodyLength-sizeof(unsigned short));
					return TBS_SUCCESS;
				}
				else
				{
					return TBS_SUCCESS;
				}
			}
		}
	}
	else
	{
		dbs_sys_log(DBS_LOG_CRIT, "cli call __cli2cmm_receive FD_ISSET failed");
		return CMM_FAILED;
	}
}
#endif
int __cli2cmm_upgrade_comm(uint8_t *buf, uint32_t len)
{
	T_UDP_SK_INFO *sk = &SK_CLI2CMM;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint16_t msgType = req->HEADER.usMsgType;
	T_REQ_Msg_CMM *ack = NULL;

	fd_set fdsr;
	//int maxsock;
	struct timeval tv;	
	int ret = 0;
	int sendn = 0;	
	struct sockaddr_in from;	
	int FromAddrSize = 0;
	int rev_len = 0;

	int select_times = 0;

	sendn = sendto(sk->sk, buf, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		printf("\r\n  cli call __cli2cmm_upgrade_comm sendto failed");
		return RC_UPG_SYS_SOCKET_IO_ERROR;
	}

	while(1)
	{
		// initialize file descriptor set
		FD_ZERO(&fdsr);
		FD_SET(sk->sk, &fdsr);

		// timeout setting
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		select_times++;

		//¼ì²âsocket
		ret = select(sk->sk + 1, &fdsr, NULL, NULL, &tv);
		if( ret <= 0 )
		{
			printf(".");
			fflush(stdout);
			if( select_times > 30)
				return RC_UPG_OPT_TIMEOUT;
			else
				continue;
		}
		// check whether a new connection comes
		if (FD_ISSET(sk->sk, &fdsr))
		{
			FromAddrSize = sizeof(from);
			rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);
			if ( -1 == rev_len )
			{
				printf("\r\n  cli call __cli2cmm_upgrade_comm recvfrom failed");
				return RC_UPG_SYS_SOCKET_IO_ERROR;
			}
			else
			{
				ack = (T_REQ_Msg_CMM *)buf;
				if( msgType != ack->HEADER.usMsgType )
				{					
					continue;
				}
				else
				{
					return ack->result;
				}
			}
		}
		else
		{
			printf("\r\n  cli call __cli2cmm_comm FD_ISSET failed");			
			return RC_UPG_SYS_SOCKET_IO_ERROR;			
		}
	}
}

int __cli2cmm_comm(uint8_t *buf, uint32_t len)
{
	T_UDP_SK_INFO *sk = &SK_CLI2CMM;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint16_t msgType = req->HEADER.usMsgType;
	T_REQ_Msg_CMM *ack = NULL;

	fd_set fdsr;
	//int maxsock;
	struct timeval tv;	
	int ret = 0;
	int sendn = 0;	
	struct sockaddr_in from;	
	int FromAddrSize = 0;
	int rev_len = 0;

	sendn = sendto(sk->sk, buf, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		dbs_sys_log(dbsdev, DBS_LOG_CRIT, "cli call __cli2cmm_comm sendto failed");
		return CMM_FAILED;
	}

	while(1)
	{
		// initialize file descriptor set
		FD_ZERO(&fdsr);
		FD_SET(sk->sk, &fdsr);

		// timeout setting
		tv.tv_sec = 18;
		tv.tv_usec = 0;

		//¼ì²âsocket
		ret = select(sk->sk + 1, &fdsr, NULL, NULL, &tv);
		if( ret <= 0 )
		{
			return CMM_FAILED;
		}
		// check whether a new connection comes
		if (FD_ISSET(sk->sk, &fdsr))
		{
			FromAddrSize = sizeof(from);
			rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);
			if ( -1 == rev_len )
			{
				dbs_sys_log(dbsdev, DBS_LOG_CRIT, "cli call __cli2cmm_comm recvfrom failed");
				return CMM_FAILED;
			}
			else
			{
				ack = (T_REQ_Msg_CMM *)buf;
				if( msgType != ack->HEADER.usMsgType )
				{
					fprintf(stderr, "WARNNING: cli __cli2cmm_comm: msgType[%d!=%d], [continue] !\n", 
						ack->HEADER.usMsgType, msgType);
					dbs_sys_log(dbsdev, DBS_LOG_WARNING, "cli __cli2cmm_comm received non-mached msgtype");
					continue;
				}
				else if( ack->result != CMM_SUCCESS )
				{	
					dbs_sys_log(dbsdev, DBS_LOG_CRIT, "cli call __cli2cmm_comm recvfrom result error");
					return  ack->result ;
				}
				else
				{
					return ack->result;
				}
			}
		}
		else
		{
			dbs_sys_log(dbsdev, DBS_LOG_CRIT, "cli call __cli2cmm_comm FD_ISSET failed");
			return CMM_FAILED;
		}
	}
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_reloadCltProfile
*	º¯Êý¹¦ÄÜ:Ç¿ÖÆÖØÐÂ¼ÓÔØÅäÖÃÎÄ¼þ
*********************************************************************************************/
int cli2cmm_reloadCltProfile(uint16_t cltid)
{
	/* ÔÝÊ±Ã»ÓÐÊµÏÖ*/
	IO_Print("\r\n\r\n  CMD TBD !");
	return CMM_FAILED;
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_reloadCnuProfile
*	º¯Êý¹¦ÄÜ:Ç¿ÖÆÖØÐÂ¼ÓÔØÅäÖÃÎÄ¼þ
*********************************************************************************************/
int cli2cmm_reloadCnuProfile(uint16_t cltid, uint16_t cnuid)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	stTmUserInfo szNode;

	szNode.clt = cltid;
	szNode.cnu = cnuid;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_SEND_CONFIG;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, &szNode, req->HEADER.ulBodyLength);

	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_permitCnu
*	º¯Êý¹¦ÄÜ:
*********************************************************************************************/
int cli2cmm_permitCnu(uint16_t cltid, uint16_t cnuid)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	stTmUserInfo szNode;

	szNode.clt = cltid;
	szNode.cnu = cnuid;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_USER_NEW;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, &szNode, req->HEADER.ulBodyLength);

	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_UndoPermitCnu
*	º¯Êý¹¦ÄÜ:
*********************************************************************************************/
int cli2cmm_UndoPermitCnu(uint16_t cltid, uint16_t cnuid)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	stTmUserInfo szNode;

	szNode.clt = cltid;
	szNode.cnu = cnuid;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_USER_DEL;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, &szNode, req->HEADER.ulBodyLength);

	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_DeleteCnu
*	º¯Êý¹¦ÄÜ:
*********************************************************************************************/
int cli2cmm_DeleteCnu(uint16_t cltid, uint16_t cnuid)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	stTmUserInfo szNode;

	szNode.clt = cltid;
	szNode.cnu = cnuid;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_DELETE_USER;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, &szNode, req->HEADER.ulBodyLength);

	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_CreateCnu
*	º¯Êý¹¦ÄÜ:
*********************************************************************************************/
int cli2cmm_CreateCnu(uint8_t mac[])
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CREATE_CNU;
	req->HEADER.ulBodyLength = 6;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, mac, req->HEADER.ulBodyLength);

	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_DoWlistControl
*	º¯Êý¹¦ÄÜ:
*********************************************************************************************/
int cli2cmm_DoWlistControl(uint16_t status)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = status?CMM_DO_WLIST_CONTROL:CMM_UNDO_WLIST_CONTROL;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}
	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_DoWdtControl
*	º¯Êý¹¦ÄÜ:
*********************************************************************************************/
int cli2cmm_DoWdtControl(uint16_t status)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = status?CMM_DO_WDT_CONTROL:CMM_UNDO_WDT_CONTROL;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}
	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_DoHBControl
*	º¯Êý¹¦ÄÜ:
*********************************************************************************************/
int cli2cmm_DoHBControl(uint16_t status)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = (status?CMM_DO_HB_TRAP_CTRL:CMM_UNDO_HB_TRAP_CTRL);
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}
	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_readAr8236Reg
*	º¯Êý¹¦ÄÜ:»ñÈ¡AR8236¼Ä´æÆ÷Öµ
*********************************************************************************************/
int cli2cmm_getCbatTemperature(st_temperature *temp_data)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	st_temperature *ack_data = (st_temperature *)(ack->BUF);

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_GET_CBAT_TEMPERATURE;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	if( CMM_SUCCESS == __cli2cmm_comm(buf, len) )
	{
		memcpy(temp_data, ack_data, sizeof(st_temperature));
		return CMM_SUCCESS;
	}	
	return CMM_FAILED;
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_readAr8236Reg
*	º¯Êý¹¦ÄÜ:»ñÈ¡AR8236¼Ä´æÆ÷Öµ
*********************************************************************************************/
int cli2cmm_readAr8236Reg(T_szAr8236Reg *szAr8236Reg)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_szAr8236Reg *ack_data = (T_szAr8236Reg *)(ack->BUF);

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_AR8236_SW_REG_READ;
	req->HEADER.ulBodyLength = sizeof(T_szAr8236Reg);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, szAr8236Reg, req->HEADER.ulBodyLength);

	if( CMM_SUCCESS == __cli2cmm_comm(buf, len) )
	{
		szAr8236Reg->mdioReg.value = ack_data->mdioReg.value;
		return CMM_SUCCESS;
	}	
	return CMM_FAILED;
}

int cli2cmm_readUserHFID(T_szCnuUserHFID *cnuuserhfid )
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_szCnuUserHFID *ack_data = (T_szCnuUserHFID *)(ack->BUF);

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_USER_HFID_READ;
	req->HEADER.ulBodyLength = sizeof(T_szCnuUserHFID);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF,cnuuserhfid, req->HEADER.ulBodyLength);

	if( CMM_SUCCESS == __cli2cmm_comm(buf, len) )
	{
		memcpy(cnuuserhfid->pdata,ack_data->pdata,64);
		return CMM_SUCCESS;
	}
	return CMM_FAILED;
	
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_writeAr8236Reg
*	º¯Êý¹¦ÄÜ:»ñÈ¡AR8236¼Ä´æÆ÷Öµ
*********************************************************************************************/
int cli2cmm_writeAr8236Reg(T_szAr8236Reg *szAr8236Reg)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_AR8236_SW_REG_WRITE;
	req->HEADER.ulBodyLength = sizeof(T_szAr8236Reg);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, szAr8236Reg, req->HEADER.ulBodyLength);

	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_readAr8236Phy
*	º¯Êý¹¦ÄÜ:»ñÈ¡AR8236¼Ä´æÆ÷Öµ
*********************************************************************************************/
int cli2cmm_readAr8236Phy(T_szAr8236Phy *szAr8236Phy)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_szAr8236Phy *ack_data = (T_szAr8236Phy *)(ack->BUF);

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_AR8236_PHY_REG_READ;
	req->HEADER.ulBodyLength = sizeof(T_szAr8236Phy);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, szAr8236Phy, req->HEADER.ulBodyLength);

	if( CMM_SUCCESS == __cli2cmm_comm(buf, len) )
	{
		szAr8236Phy->mdioPhy.value = ack_data->mdioPhy.value;
		return CMM_SUCCESS;
	}	
	return CMM_FAILED;
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_writeAr8236Phy
*	º¯Êý¹¦ÄÜ:»ñÈ¡AR8236¼Ä´æÆ÷Öµ
*********************************************************************************************/
int cli2cmm_writeAr8236Phy(T_szAr8236Phy *szAr8236Phy)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_AR8236_PHY_REG_WRITE;
	req->HEADER.ulBodyLength = sizeof(T_szAr8236Phy);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, szAr8236Phy, req->HEADER.ulBodyLength);

	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_readCnuSwitchRegister
*	º¯Êý¹¦ÄÜ:»ñÈ¡AR8236¼Ä´æÆ÷Öµ
*********************************************************************************************/
int cli2cmm_readCnuSwitchRegister(T_szSwRtl8306eConfig *rtl8306eSettings)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_szSwRtl8306eConfig *ack_data = (T_szSwRtl8306eConfig *)(ack->BUF);

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CNU_SWITCH_READ;
	req->HEADER.ulBodyLength = sizeof(T_szSwRtl8306eConfig);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, rtl8306eSettings, req->HEADER.ulBodyLength);

	if( CMM_SUCCESS == __cli2cmm_comm(buf, len) )
	{
		rtl8306eSettings->mdioInfo.value = ack_data->mdioInfo.value;
		return CMM_SUCCESS;
	}	
	return CMM_FAILED;
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_writeCnuSwitchRegister
*	º¯Êý¹¦ÄÜ:»ñÈ¡AR8236¼Ä´æÆ÷Öµ
*********************************************************************************************/
int cli2cmm_writeCnuSwitchRegister(T_szSwRtl8306eConfig *rtl8306eSettings)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CNU_SWITCH_WRITE;
	req->HEADER.ulBodyLength = sizeof(T_szSwRtl8306eConfig);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, rtl8306eSettings, req->HEADER.ulBodyLength);

	return __cli2cmm_comm(buf, len);
}


/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_mdioReadPhy
*	º¯Êý¹¦ÄÜ:»ñÈ¡Íâ½ÓPHY¼Ä´æÆ÷Öµ
*********************************************************************************************/
int cli2cmm_mdioReadPhy(T_szAr8236Phy *szAr8236Phy)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	T_szAr8236Phy *ack_data = (T_szAr8236Phy *)(ack->BUF);

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_MME_MDIO_READ;
	req->HEADER.ulBodyLength = sizeof(T_szAr8236Phy);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, szAr8236Phy, req->HEADER.ulBodyLength);

	if( CMM_SUCCESS == __cli2cmm_comm(buf, len) )
	{
		szAr8236Phy->mdioPhy.value = ack_data->mdioPhy.value;
		return CMM_SUCCESS;
	}	
	return CMM_FAILED;
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_mdioWritePhy
*	º¯Êý¹¦ÄÜ:»ñÈ¡Íâ½ÓPHY¼Ä´æÆ÷Öµ
*********************************************************************************************/
int cli2cmm_mdioWritePhy(T_szAr8236Phy *szAr8236Phy)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_MME_MDIO_WRITE;
	req->HEADER.ulBodyLength = sizeof(T_szAr8236Phy);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, szAr8236Phy, req->HEADER.ulBodyLength);

	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_getRgmiiTimingDelay
*	º¯Êý¹¦ÄÜ:»ñÈ¡AR8236¼Ä´æÆ÷Öµ
*********************************************************************************************/
int cli2cmm_setRgmiiTimingDelay(st_dsdtRgmiiTimingDelay *pdelay)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_SET_DSDT_RGMII_DELAY;
	req->HEADER.ulBodyLength = sizeof(st_dsdtRgmiiTimingDelay);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, pdelay, req->HEADER.ulBodyLength);

	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_setRgmiiTimingDelay
*	º¯Êý¹¦ÄÜ:»ñÈ¡AR8236¼Ä´æÆ÷Öµ
*********************************************************************************************/
int cli2cmm_getRgmiiTimingDelay(st_dsdtRgmiiTimingDelay *pdelay)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	st_dsdtRgmiiTimingDelay *ack_data = (st_dsdtRgmiiTimingDelay *)(ack->BUF);

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_GET_DSDT_RGMII_DELAY;
	req->HEADER.ulBodyLength = sizeof(st_dsdtRgmiiTimingDelay);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, pdelay, req->HEADER.ulBodyLength);

	if( CMM_SUCCESS == __cli2cmm_comm(buf, len) )
	{
		pdelay->rxdelay = ack_data->rxdelay;
		pdelay->txdelay = ack_data->txdelay;
		return CMM_SUCCESS;
	}	
	return CMM_FAILED;
}

int cli2cmm_shutdownConfig(st_dbsProfile *profile)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	
	if( 0 == profile->col_row_sts )
	{
		return CMM_FAILED;
	}	

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_SHUTDOWN;
	req->HEADER.ulBodyLength = sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, profile, req->HEADER.ulBodyLength);
	
	return __cli2cmm_comm(buf, len);
}

int cli2cmm_macLimitConfig1(st_dbsProfile *profile)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	
	if( 0 == profile->col_row_sts )
	{
		return CMM_FAILED;
	}	

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_DO_MAC_LIMIT_CONFIG;
	req->HEADER.ulBodyLength = sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, profile, req->HEADER.ulBodyLength);
	
	return __cli2cmm_comm(buf, len);
}

int cli2cmm_macLimitConfig2(rtl8306eWriteInfo *req_data1, st_dbsProfile *profile)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;

	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	rtl8306eWriteInfo *req_data = (rtl8306eWriteInfo *)(req->BUF);
	memcpy(req_data,req_data1,sizeof(rtl8306eWriteInfo));
	
	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CNU_SWITCH_CONFIG_WRITE;
	req->HEADER.ulBodyLength = sizeof(rtl8306eWriteInfo) + sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE)
	{
		printf("cli send rtl8306 body and profile to cmm error\n");
		return CMM_FAILED;
	} 
	memcpy(req->BUF + sizeof(rtl8306eWriteInfo), profile, sizeof(st_dbsProfile));
	return __cli2cmm_comm(buf, len);
}

int cli2cmm_stormFilterConfig1(st_dbsProfile *profile)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	
	if( 0 == profile->col_row_sts )
	{
		return CMM_FAILED;
	}	

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_STROMCONTROL;
	req->HEADER.ulBodyLength = sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, profile, req->HEADER.ulBodyLength);
	
	return __cli2cmm_comm(buf, len);
}
int cli2cmm_stormFilterConfig2(rtl8306eWriteInfo *req_data1, st_dbsProfile *profile)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;

	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	rtl8306eWriteInfo *req_data = (rtl8306eWriteInfo *)(req->BUF);
	memcpy(req_data,req_data1,sizeof(rtl8306eWriteInfo));
	
	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CNU_SWITCH_CONFIG_WRITE;
	req->HEADER.ulBodyLength = sizeof(rtl8306eWriteInfo) + sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE)
	{
		printf("cli send rtl8306 body and profile to cmm error\n");
		return CMM_FAILED;
	} 
	memcpy(req->BUF + sizeof(rtl8306eWriteInfo), profile, sizeof(st_dbsProfile));
	return __cli2cmm_comm(buf, len);
}
int cli2cmm_rateLimitConfig1(st_dbsProfile *profile)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	
	if( 0 == profile->col_row_sts )
	{
		return CMM_FAILED;
	}	

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_FLOWCONTROL;
	req->HEADER.ulBodyLength = sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, profile, req->HEADER.ulBodyLength);
	return __cli2cmm_comm(buf, len);
}
int cli2cmm_rateLimitConfig2(rtl8306eWriteInfo *req_data1, st_dbsProfile *profile)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;

	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	rtl8306eWriteInfo *req_data = (rtl8306eWriteInfo *)(req->BUF);
	memcpy(req_data,req_data1,sizeof(rtl8306eWriteInfo));
	
	if( 0 == profile->col_row_sts )
	{
		return CMM_FAILED;
	}
	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CNU_SWITCH_CONFIG_WRITE;
	req->HEADER.ulBodyLength = sizeof(rtl8306eWriteInfo) + sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE)
	{
		printf("cli send rtl8306 body and profile to cmm error\n");
		return CMM_FAILED;
	} 
	memcpy(req->BUF + sizeof(rtl8306eWriteInfo), profile, sizeof(st_dbsProfile));
	return __cli2cmm_comm(buf, len);
}

int cli2cmm_vlanConfig1(st_dbsProfile *profile)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	
	if( 0 == profile->col_row_sts )
	{
		return CMM_FAILED;
	}
	
	/* ·ÀÖ¹¶Ë¿ÚPVID ±»ÉèÖÃÎª0 */
	if( 0 == profile->col_eth1vid )
	{
		profile->col_eth1vid = 1;
	}
	if( 0 == profile->col_eth2vid )
	{
		profile->col_eth2vid = 1;
	}
	if( 0 == profile->col_eth3vid )
	{
		profile->col_eth3vid = 1;
	}
	if( 0 == profile->col_eth4vid )
	{
		profile->col_eth4vid = 1;
	}

	/* CHECK VID */
	if( (profile->col_eth1vid < 1) || (profile->col_eth1vid > 4094) )
	{
		return CMM_FAILED;
	}
	if( (profile->col_eth2vid < 1) || (profile->col_eth2vid > 4094) )
	{
		return CMM_FAILED;
	}
	if( (profile->col_eth3vid < 1) || (profile->col_eth3vid > 4094) )
	{
		return CMM_FAILED;
	}
	if( (profile->col_eth4vid < 1) || (profile->col_eth4vid > 4094) )
	{
		return CMM_FAILED;
	}
	
	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CNU_VLAN_CONFIG;
	req->HEADER.ulBodyLength = sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{	
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, profile, req->HEADER.ulBodyLength);
	return __cli2cmm_comm(buf, len);	
}

int cli2cmm_vlanConfig2(rtl8306eWriteInfo *req_data1, st_dbsProfile *profile)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;

	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	rtl8306eWriteInfo *req_data = (rtl8306eWriteInfo *)(req->BUF);
	
	memcpy(req_data,req_data1,sizeof(rtl8306eWriteInfo));
	
	if( 0 == profile->col_row_sts )
	{
		return CMM_FAILED;
	}
	if( 0 == profile->col_eth1vid )
	{
		profile->col_eth1vid = 1;
		req_data->rtl8306eConfig.vlanConfig.vlan_port[0].pvid = 1;
	}
	if( 0 == profile->col_eth2vid )
	{
		profile->col_eth2vid = 1;
		req_data->rtl8306eConfig.vlanConfig.vlan_port[1].pvid = 1;
	}
	if( 0 == profile->col_eth3vid )
	{
		profile->col_eth3vid = 1;
		req_data->rtl8306eConfig.vlanConfig.vlan_port[2].pvid = 1;
	}
	if( 0 == profile->col_eth4vid )
	{
		profile->col_eth4vid = 1;
		req_data->rtl8306eConfig.vlanConfig.vlan_port[3].pvid = 1;
	}
	if( (profile->col_eth1vid < 1) || (profile->col_eth1vid > 4094) )
	{
		return CMM_FAILED;
	}
	if( (profile->col_eth2vid < 1) || (profile->col_eth2vid > 4094) )
	{
		return CMM_FAILED;
	}
	if( (profile->col_eth3vid < 1) || (profile->col_eth3vid > 4094) )
	{
		return CMM_FAILED;
	}
	if( (profile->col_eth4vid < 1) || (profile->col_eth4vid > 4094) )
	{
		return CMM_FAILED;
	}

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CNU_SWITCH_CONFIG_WRITE;
	req->HEADER.ulBodyLength = sizeof(rtl8306eWriteInfo) + sizeof(st_dbsProfile);
	req->HEADER.fragment = 0;
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE)
	{
		printf("cli send rtl8306 body and profile to cmm error\n");
		return CMM_FAILED;
	} 
	memcpy(req->BUF + sizeof(rtl8306eWriteInfo), profile, sizeof(st_dbsProfile));
	return __cli2cmm_comm(buf, len);
}

int cli2cmm_readSwitchSettings(rtl8306eWriteInfo *rtl8306eSettings )
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);
	
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	st_rtl8306eSettings *ack_data = (st_rtl8306eSettings *)(ack->BUF);

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CNU_SWITCH_CONFIG_READ;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	req->HEADER.fragment = 0;

	req_data->clt = rtl8306eSettings->node.clt;
	req_data->cnu = rtl8306eSettings->node.cnu;
	
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	if( CMM_SUCCESS == __cli2cmm_comm(buf, len) )
	{
			memcpy(&(rtl8306eSettings->rtl8306eConfig),(void *)ack_data,sizeof(st_rtl8306eSettings));
			return CMM_SUCCESS;
	}
	return CMM_FAILED;			
}


int cli2cmm_resetClt(uint16_t id)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint16_t *index = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLT_RESET;
	req->HEADER.ulBodyLength = sizeof(uint16_t);
	req->HEADER.fragment = 0;

	*index = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __cli2cmm_comm(buf, len);
}

int cli2cmm_resetCnu(uint16_t id)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint16_t *index = (uint16_t *)(req->BUF);

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CNU_RESET;
	req->HEADER.ulBodyLength = sizeof(uint16_t);
	req->HEADER.fragment = 0;

	*index = id;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __cli2cmm_comm(buf, len);
}

int cli2cmm_resetMp(void)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CBAT_RESET;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __cli2cmm_comm(buf, len);
}

int cli2cmm_restoreDefault(void)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_RESTORE_DEFAULT;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __cli2cmm_comm(buf, len);
}

int cli2cmm_debug(st_ModuleDebugCtl *debug)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;

	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	
	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_MODULE_DEBUG_CONTROL;
	req->HEADER.ulBodyLength = sizeof(st_ModuleDebugCtl);
	req->HEADER.fragment = 0;

	memcpy(req->BUF, debug, req->HEADER.ulBodyLength);
	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_Dump
*	º¯Êý¹¦ÄÜ:
*********************************************************************************************/
int cli2cmm_Dump(uint16_t cltid, uint16_t cunid, uint16_t flag)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	switch(flag)
	{
		case 0:
		{
			req->HEADER.usMsgType = CMM_DUMP_CNU_REG;
			break;
		}
		case 1:
		{
			req->HEADER.usMsgType = CMM_DUMP_CNU_MOD;
			break;
		}
		case 2:
		{
			req->HEADER.usMsgType = CMM_DUMP_CNU_PIB;
			break;
		}
		default:
		{
			return CMM_FAILED;
		}
	}
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	req->HEADER.fragment = 0;

	req_data->clt = cltid;
	req_data->cnu = cunid;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}
	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_DoWdtControl
*	º¯Êý¹¦ÄÜ:
*********************************************************************************************/
int cli2cmm_DebugPrintAllDsdtPortStats(int port)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	uint32_t *req_data = (uint32_t *)(req->BUF);

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_DEBUG_PRINT_PORT_STAT;
	req->HEADER.ulBodyLength = sizeof(uint32_t);
	req->HEADER.fragment = 0;

	*req_data = port;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}
	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_DoWdtControl
*	º¯Êý¹¦ÄÜ:
*********************************************************************************************/
int cli2cmm_ClearDsdtPortStats(void)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLEAR_PORT_STAT;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}
	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_DoPortMirroring
*	º¯Êý¹¦ÄÜ:ÉèÖÃ¶Ë¿Ú¾µÏñ
*********************************************************************************************/
int cli2cmm_DoPortMirroring(st_dsdtPortMirroring *pMirrorInfo)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_SET_DSDT_PORT_MIRRORING;
	req->HEADER.ulBodyLength = sizeof(st_dsdtPortMirroring);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, pMirrorInfo, req->HEADER.ulBodyLength);

	return __cli2cmm_comm(buf, len);
}

int cli2cmm_DoDsdtMacBinding(stDsdtMacBinding *macBindingInfo)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_DSDT_MAC_BINDING;
	req->HEADER.ulBodyLength = sizeof(stDsdtMacBinding);
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	memcpy(req->BUF, macBindingInfo, req->HEADER.ulBodyLength);

	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_do_aclDropMme
*	º¯Êý¹¦ÄÜ:
*********************************************************************************************/
int cli2cmm_do_aclDropMme(uint16_t cltid, uint16_t cnuid)
{
	uint32_t len = 0;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_DO_CNU_ACL_DROP_MME;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	req->HEADER.fragment = 0;
	req_data->clt = cltid;
	req_data->cnu = cnuid;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	return __cli2cmm_comm(buf, len);
}

/********************************************************************************************
*	º¯ÊýÃû³Æ:cli2cmm_undo_aclDropMme
*	º¯Êý¹¦ÄÜ:
*********************************************************************************************/
int cli2cmm_undo_aclDropMme(uint16_t cltid, uint16_t cnuid)
{
	uint32_t len = 0;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_UNDO_CNU_ACL_DROP_MME;
	req->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	req->HEADER.fragment = 0;
	req_data->clt = cltid;
	req_data->cnu = cnuid;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		IO_Print("\r\n\r\n	Memery Error !");
		return CMM_FAILED;
	}

	return __cli2cmm_comm(buf, len);
}


#if 0
int cli2cmm_upgradeApp(void)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;	

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_UPGRADE;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __cli2cmm_comm(buf, len);
}
#endif

int cli2cmm_upgradeApp(void)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;	

	req->HEADER.usSrcMID = MID_CLI;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CLI_UPGRADE;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __cli2cmm_upgrade_comm(buf, len);
}

int destroy_cli_cmm(void)
{
	T_UDP_SK_INFO *sk = &SK_CLI2CMM;
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int init_cli_cmm(void)
{
	if( ( SK_CLI2CMM.sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return TBS_FAILED;
	}	

	SK_CLI2CMM.skaddr.sin_family = PF_INET;
	SK_CLI2CMM.skaddr.sin_port = htons(CMM_LISTEN_PORT);		/* Ä¿µÄ¶Ë¿ÚºÅ*/
	SK_CLI2CMM.skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	return TBS_SUCCESS;
}

