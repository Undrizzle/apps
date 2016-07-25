#include <assert.h>
#include "auto_mmead.h"

T_UDP_SK_INFO SK_MMEAD;

int __msg_auto_mmead_communicate(uint8_t *buf, uint32_t len)
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

int auto_mmead_get_rtl8306e_configs(uint8_t ODA[], st_rtl8306eSettings *rtl8306e)
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

	if( 0 == __msg_auto_mmead_communicate(buf, len) )
	{
		memcpy(rtl8306e, MMETS_ACK_DATA, sizeof(st_rtl8306eSettings));
		return CMM_SUCCESS;
	}
	return CMM_FAILED;
}

int msg_auto_mmead_destroy(void)
{
	T_UDP_SK_INFO *sk = &SK_MMEAD;
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int msg_auto_mmead_init(void)
{
	T_UDP_SK_INFO *sk = &SK_MMEAD;
	
	if( ( sk->sk = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}
	sk->skaddr.sin_family = AF_INET;
	sk->skaddr.sin_port = htons(MMEAD_LISTEN_PORT);		/* Ä¿µÄ¶Ë¿ÚºÅ*/
	sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");		/* Ä¿µÄµØÖ·*/
	return CMM_SUCCESS;
}


