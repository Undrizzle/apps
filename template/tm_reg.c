#include <assert.h>
#include "tm_reg.h"

T_UDP_SK_INFO SK_TM_REG;	/*负责与Register 模块通讯的套接字*/

void tm2reg_send_notification(stRegEvent *regEvent)
{
	T_UDP_SK_INFO *sk = &SK_TM_REG;
	stTmUserInfo TmUserInfo;
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_CMM *msg_cmm_regi = (T_Msg_CMM *)buf;


	bzero(buf, MAX_UDP_SIZE);

	TmUserInfo.clt = regEvent->clt;
	TmUserInfo.cnu = regEvent->cnu;
	//printf("   TmUserInfo.clt =%d\n",TmUserInfo.clt);
	//printf("   TmUserInfo.cnu =%d\n",TmUserInfo.cnu);

	msg_cmm_regi->HEADER.usSrcMID = MID_TEMPLATE;
	msg_cmm_regi->HEADER.usDstMID = MID_REGISTER;
	msg_cmm_regi->HEADER.usMsgType = regEvent->event;	
	msg_cmm_regi->HEADER.fragment = 0;
	msg_cmm_regi->HEADER.ulBodyLength = sizeof(stTmUserInfo);

	
	memcpy(msg_cmm_regi->BUF, &TmUserInfo, msg_cmm_regi->HEADER.ulBodyLength);
	
	/* 将处理信息发送给请求者 */
	sendto(sk->sk, (uint8_t *)buf, sizeof(T_Msg_Header_CMM) + sizeof(stTmUserInfo), 
		0, (struct sockaddr *)&(sk->skaddr), sizeof(sk->skaddr));
}

int tm2reg_destroy(void)
{
	T_UDP_SK_INFO *sk = &SK_TM_REG;
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int tm2reg_init(void)
{
	T_UDP_SK_INFO *sk = &SK_TM_REG;
	
	if( ( sk->sk = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}
	sk->skaddr.sin_family = AF_INET;
	sk->skaddr.sin_port = htons(REG_CORE_LISTEN_PORT);		/* 目的端口号*/
	sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");		/* 目的地址*/
	return CMM_SUCCESS;
}

