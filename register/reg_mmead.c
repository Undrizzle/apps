/*****************************************************************************************
  文件名称 : reg_mmead.c
  文件描述 : 注册模块与MMEAD模块连接的处理函数
  修订记录 :
           1 创建 : frank
             日期 : 2010-12-07
             描述 : 创建文件

 *****************************************************************************************/
#include <assert.h>
#include "reg_mmead.h"

T_UDP_SK_INFO SK_MMEAD;

int __msg_reg_mmead_communicate(uint8_t *buf, uint32_t len)
{
	fd_set fdsr;
	int maxsock;
	struct timeval tv;
	int ret = 0;
	int sendn = 0;
	T_REQ_Msg_MMEAD *r = NULL;
	struct sockaddr_in from;
	int FromAddrSize = 0;
	int rev_len = 0;
	T_UDP_SK_INFO *sk = &SK_MMEAD;

	sendn = sendto(sk->sk, buf, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		perror("msg_reg_mmead_communicate call sendto error, continue.\n");
		return -1;
	}
	
	// initialize file descriptor set
	FD_ZERO(&fdsr);
	FD_SET(sk->sk, &fdsr);

	// timeout setting
	tv.tv_sec = 20;
	tv.tv_usec = 0;
	
	maxsock = sk->sk;

	ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
	if( ret <= 0 )
	{
		printf("msg_reg_mmead_communicate select time out\n");
		return -1;
	}
	
	// check whether a new connection comes
	if (FD_ISSET(sk->sk, &fdsr))
	{
		FromAddrSize = sizeof(from);
		rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);

		if ( -1 == rev_len )
		{
			perror("msg_reg_mmead_communicate call recvfrom error.\n");
			return -1;
		}
		else
		{
			r = (T_REQ_Msg_MMEAD *)buf;
			if( MMEAD_MSG_ID != r->HEADER.M_TYPE )
			{
				perror("Non-matched msg revieved by msg_reg_mmead_communicate.\n");
				return -1;
			}
			
			if( r->result )
			{
				//printf("msg_reg_mmead_communicate recieved server apply: result = %d\n", r->result);
			}
			return r->result;
		}			
	}
	else
	{
		printf("fd is not setted, continue\n");
		return -1;
	}
}

int msg_reg_mmead_enable_user(uint32_t devType, uint8_t ODA[])
{
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_MMEAD h;

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = devType;
	h.MM_TYPE = MMEAD_AUTHORIZATION_USER;
	h.fragment = 0;
	memcpy(h.ODA, ODA, 6);
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	return __msg_reg_mmead_communicate(buf, sizeof(h));
}

int msg_reg_mmead_block_user(uint32_t devType, uint8_t ODA[])
{
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_MMEAD h;

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = devType;
	h.MM_TYPE = MMEAD_BLOCK_USER;
	h.fragment = 0;
	memcpy(h.ODA, ODA, 6);
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	return __msg_reg_mmead_communicate(buf, sizeof(h));
}

int msg_reg_mmead_bootout_dev(uint8_t ODA[], uint8_t CNU_MAC[])
{
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_MMEAD *msg = (T_Msg_MMEAD *)buf;

	msg->HEADER.M_TYPE = 0xCC08;
	msg->HEADER.DEV_TYPE = WEC_3801I;
	msg->HEADER.MM_TYPE = MMEAD_BOOT_OUT_CNU;
	msg->HEADER.fragment = 0;
	memcpy(msg->HEADER.ODA, ODA, 6);
	msg->HEADER.LEN = 6;
	memcpy(msg->BUF, CNU_MAC, 6);

	return __msg_reg_mmead_communicate(buf, sizeof(T_Msg_Header_MMEAD)+6);
}

int msg_reg_mmead_wr_user_mod(uint32_t devType, uint8_t ODA[])
{
#if 0
	printf("ODA %02X:%02X:%02X:%02X:%02X:%02X write MOD\n", 
		ODA[0], ODA[1], ODA[2], ODA[3], ODA[4], ODA[5]);
	return 0;
#else
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_MMEAD h;

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = devType;
	h.MM_TYPE = MMEAD_WRITE_MODULE_OPERATION;
	h.fragment = 0;
	memcpy(h.ODA, ODA, 6);
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	return __msg_reg_mmead_communicate(buf, sizeof(h));
#endif
}

int msg_reg_mmead_wr_user_pib(uint32_t devType, uint8_t ODA[])
{
#if 0
	printf("ODA %02X:%02X:%02X:%02X:%02X:%02X write PIB\n", 
		ODA[0], ODA[1], ODA[2], ODA[3], ODA[4], ODA[5]);
	return 0;
#else
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_MMEAD h;

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = devType;
	h.MM_TYPE = MMEAD_WRITE_PIB;
	h.fragment = 0;
	memcpy(h.ODA, ODA, 6);
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	return __msg_reg_mmead_communicate(buf, sizeof(h));
#endif
}

int msg_reg_mmead_get_clt(T_CLT_INFO *clt)
{
	uint8_t null_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *confirm = (T_REQ_Msg_MMEAD *)buf;
	
	bzero(buf, MAX_UDP_SIZE);
	
	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_GET_CLT_MAC;
	h.fragment = 0;
	memcpy(h.ODA, null_mac, 6);
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));

	if( CMM_SUCCESS == __msg_reg_mmead_communicate(buf, sizeof(h)) )
	{
		clt->DevType = WEC_3801I;
		memcpy(clt->Mac, confirm->BUF, 6);
		return CMM_SUCCESS;
	}
	return CMM_FAILED;
}

int reg_mmead_get_rtl8306e_configs(uint8_t ODA[], st_rtl8306eSettings *rtl8306e)
{
	assert( NULL != rtl8306e );
	
	int len = 0;
	uint8_t buf[MAX_UDP_SIZE];
	
	T_MMETS_REQ_MSG *MMETS_REQ = (T_MMETS_REQ_MSG *)buf;
	T_MMETS_ACK_MSG *MMETS_ACK = (T_MMETS_ACK_MSG *)buf;
	st_rtl8306eSettings *MMETS_ACK_DATA = (st_rtl8306eSettings *)(MMETS_ACK->body);
	
	MMETS_REQ->header.M_TYPE = 0xCC08;
	MMETS_REQ->header.DEV_TYPE = WEC701_E4;	
	MMETS_REQ->header.MM_TYPE = MMEAD_GET_RTL8306E_CONFIG;
	MMETS_REQ->header.fragment = 0;
	MMETS_REQ->header.LEN = 0;
	memcpy(MMETS_REQ->header.ODA, ODA, 6);

	len = sizeof(T_MMETS_REQ_MSG) + MMETS_REQ->header.LEN;

	if( 0 == __msg_reg_mmead_communicate(buf, len) )
	{
		memcpy(rtl8306e, MMETS_ACK_DATA, sizeof(st_rtl8306eSettings));
		return CMM_SUCCESS;
	}
	return CMM_FAILED;
}

int reg_mmead_write_rtl8306e_mod(uint8_t ODA[], uint8_t *mod, uint32_t mod_len)
{
	assert( NULL != mod );
	
	int len = 0;
	uint8_t buf[MAX_UDP_SIZE];
	
	T_MMETS_REQ_MSG *MMETS_REQ = (T_MMETS_REQ_MSG *)buf;
	T_MMETS_ACK_MSG *MMETS_ACK = (T_MMETS_ACK_MSG *)buf;
	
	
	MMETS_REQ->header.M_TYPE = 0xCC08;
	MMETS_REQ->header.DEV_TYPE = WEC701_E4;	
	MMETS_REQ->header.MM_TYPE = MMEAD_WRITE_MOD;
	MMETS_REQ->header.fragment = 0;
	MMETS_REQ->header.LEN = mod_len;
	memcpy(MMETS_REQ->header.ODA, ODA, 6);

	memcpy(MMETS_REQ->body, mod, mod_len);

	len = sizeof(T_MMETS_REQ_MSG) + MMETS_REQ->header.LEN;

	return __msg_reg_mmead_communicate(buf, len);
}


int msg_reg_mmead_get_nelist(uint8_t ODA[], T_MMEAD_TOPOLOGY *plist)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = (T_REQ_Msg_MMEAD *)buf;
	assert( NULL != plist);
	//struct timeval start;
	//struct timeval end;
	bzero(plist, sizeof(T_MMEAD_TOPOLOGY));
	bzero(buf, MAX_UDP_SIZE);

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_GET_TOPOLOGY;
	h.fragment = 0;
	memcpy(h.ODA, ODA, 6);
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	//gettimeofday(&start, NULL);
	while( CMM_SUCCESS == __msg_reg_mmead_communicate(buf, sizeof(h)) )
	{
		/* 将数据写入plist */
		memcpy((void *)plist, (void *)(r->BUF), sizeof(T_MMEAD_TOPOLOGY));
		if( 0 == memcmp((const char *)ODA, (const char *)(plist->clt.Mac), 6) )
		{
			return CMM_SUCCESS;
		}
		//gettimeofday(&end, NULL);
		//printf("\r\n====>time %d\n", (end.tv_sec * 1000 + end.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000));
	}
	
	return CMM_FAILED;
}

int msg_reg_mmead_get_user_hfid(uint8_t ODA[], uint8_t *pdata)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = (T_REQ_Msg_MMEAD *)buf;
	bzero(pdata, sizeof(uint8_t));
	bzero(buf, MAX_UDP_SIZE);

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_GET_USER_HFID;
	h.fragment = 0;
	memcpy(h.ODA, ODA, 6);
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));

	if( CMM_SUCCESS == __msg_reg_mmead_communicate(buf, sizeof(h)) )
	{
		memcpy(pdata, (void *)r->BUF, 64);
		return CMM_SUCCESS;
	}
	return CMM_FAILED;
}
	
int msg_reg_mmead_reset_eoc(uint32_t devType, uint8_t ODA[])
{
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_MMEAD h;

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = devType;
	h.MM_TYPE = MMEAD_RESET_DEV;
	h.fragment = 0;
	memcpy(h.ODA, ODA, 6);
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	return __msg_reg_mmead_communicate(buf, sizeof(h));
}

int msg_mmead_destroy(void)
{
	T_UDP_SK_INFO *sk = &SK_MMEAD;
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int msg_mmead_init(void)
{
	T_UDP_SK_INFO *sk = &SK_MMEAD;
	
	if( ( sk->sk = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}
	sk->skaddr.sin_family = AF_INET;
	sk->skaddr.sin_port = htons(MMEAD_LISTEN_PORT);		/* 目的端口号*/
	sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");		/* 目的地址*/
	return CMM_SUCCESS;
}


