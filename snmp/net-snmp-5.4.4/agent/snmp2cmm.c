#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>

#include "snmp2cmm.h"

T_DBS_DEV_INFO *dbsdev = NULL;

static T_UDP_SK_INFO SK_AGENT_CMM;

int __snmp2cmm_communicate(uint8_t *buf, uint32_t len)
{
	int ret = 0;
	int sendn = 0;
	int rev_len = 0;
	int FromAddrSize = 0;
	T_UDP_SK_INFO *SK = &SK_AGENT_CMM;
	
	int maxsock;
	fd_set fdsr;
	struct timeval tv;
	struct sockaddr_in from;
	T_CMM_MSG_HEADER_ACK *r = NULL;
	T_Msg_Header_CMM *req = (T_Msg_Header_CMM *)buf;

	uint16_t msgType = req->usMsgType;
	
	sendn = sendto(SK->sk, buf, len, 0, (struct sockaddr *)&(SK->skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		return 1;
	}

	while(1)
	{
		// initialize file descriptor set
		FD_ZERO(&fdsr);
		FD_SET(SK->sk, &fdsr);

		// timeout setting
		tv.tv_sec = 10;
		tv.tv_usec = 0;

		maxsock = SK->sk;

		ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
		if( ret <= 0 )
		{
			return 2;
		}
		else
		{
			// check whether a new connection comes
			if (FD_ISSET(SK->sk, &fdsr))
			{
				FromAddrSize = sizeof(from);
				rev_len = recvfrom(SK->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);
				if ( rev_len <= 0 )
				{
					continue;
				}
				else
				{					
					r = (T_CMM_MSG_HEADER_ACK *)buf;
					if( msgType != r->usMsgType )
					{
						continue;
					}
					else
					{
						return r->Result;
					}
				}
			}
			else
			{
				return 3;
			}
		}
	}	
}

int snmp2cmm_resetCnu(uint16_t id)
{
	int ret = CMM_FAILED;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t buf_len = 0;

	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	uint16_t *index = (uint16_t *)(request->BUF);

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_CNU_RESET;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = sizeof(uint16_t);

	*index = id;
	
	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;	
	return __snmp2cmm_communicate(buf, buf_len);
}

int snmp2cmm_reloadCnuProfile(uint16_t id)
{
	int ret = CMM_FAILED;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t buf_len = 0;

	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	stTmUserInfo *szNode = (stTmUserInfo *)(request->BUF);

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_CLI_SEND_CONFIG;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	szNode->clt = 1;
	szNode->cnu = id;
	
	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;	
	return __snmp2cmm_communicate(buf, buf_len);
}

int snmp2cmm_permitCnu(uint16_t id)
{
	int ret = CMM_FAILED;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t buf_len = 0;

	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	stTmUserInfo *szNode = (stTmUserInfo *)(request->BUF);

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_CLI_USER_NEW;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	szNode->clt = 1;
	szNode->cnu = id;
	
	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;	
	return __snmp2cmm_communicate(buf, buf_len);
}

int snmp2cmm_UndoPermitCnu(uint16_t id)
{
	int ret = CMM_FAILED;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t buf_len = 0;

	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	stTmUserInfo *szNode = (stTmUserInfo *)(request->BUF);

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_CLI_USER_DEL;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	szNode->clt = 1;
	szNode->cnu = id;
	
	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;	
	return __snmp2cmm_communicate(buf, buf_len);
}

int snmp2cmm_DeleteCnu(uint16_t id)
{
	int ret = CMM_FAILED;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t buf_len = 0;

	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	stTmUserInfo *szNode = (stTmUserInfo *)(request->BUF);

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_CLI_DELETE_USER;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = sizeof(stTmUserInfo);
	szNode->clt = 1;
	szNode->cnu = id;
	
	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;	
	return __snmp2cmm_communicate(buf, buf_len);
}

int snmp2cmm_resetMp(void)
{
	int ret = CMM_FAILED;
	uint8_t buf[MAX_UDP_SIZE];
	uint32_t buf_len = 0;

	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	T_CMM_MSG_ACK *confirm = (T_CMM_MSG_ACK *)buf;

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_CBAT_RESET;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = 0;
	
	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;	

	return __snmp2cmm_communicate(buf, buf_len);
}

int snmp2cmm_restoreDefault(void)
{
	int ret = CMM_FAILED;
	uint8_t buf[MAX_UDP_SIZE];
	uint32_t buf_len = 0;

	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	T_CMM_MSG_ACK *confirm = (T_CMM_MSG_ACK *)buf;

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_RESTORE_DEFAULT;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = 0;
	
	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;	

	return __snmp2cmm_communicate(buf, buf_len);
}

int snmp2cmm_sendHeartbeat(void)
{
	int ret = CMM_FAILED;
	uint8_t buf[MAX_UDP_SIZE];
	uint32_t buf_len = 0;

	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	T_CMM_MSG_ACK *confirm = (T_CMM_MSG_ACK *)buf;

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_SEND_TOPOLOGY_HB_TRAP;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = 0;
	
	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;	

	return __snmp2cmm_communicate(buf, buf_len);
}


int snmp2cmm_uploadSoftware(void)
{
	int ret = CMM_FAILED;
	uint8_t buf[MAX_UDP_SIZE];
	uint32_t buf_len = 0;

	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	T_CMM_MSG_ACK *confirm = (T_CMM_MSG_ACK *)buf;

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_CLI_UPGRADE;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = 0;
	
	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;	

	return __snmp2cmm_communicate(buf, buf_len);
}

int snmp2cmm_mapDevModel(int model)
{
	switch(model)
	{
		case WEC_3501I_X7:
		{
			return 1;
		}
		case WEC_3501I_E31:
		{
			return 2;
		}
		case WEC_3501I_Q31:
		{
			return 3;
		}
		case WEC_3501I_C22:
		{
			return 4;
		}
		case WEC_3501I_S220:
		{
			return 5;
		}
		case WEC_3501I_S60:
		{
			return 6;
		}
		case WEC_3702I:
		{
			return 7;
		}
		case WEC_3703I:
		{
			return 8;
		}
		case WEC_602:
		{
			return 9;
		}
		case WEC_604:
		{
			return 10;
		}
		case WEC_3801I:
		{
			return 11;
		}
		case WEC9720EK_C22:
		{
			return 20;
		}
		case WEC9720EK_E31:
		{
			return 21;
		}
		case WEC9720EK_Q31:
		{
			return 22;
		}
		case WEC9720EK_S220:
		{
			return 23;
		}
		case WEC9720EK_SD220:
		{
			return 24;
		}
		case WEC701_M0:
		{
			return 36;
		}
		case WEC701_C2:
		{
			return 40;
		}
		case WEC701_C4:
		{
			return 41;
		}
	}
	return 256;
}

int snmp2cmm_isIpv4Valid(uint8_t * sip, uint32_t len)
{
	unsigned int n1,n2,n3,n4;
	char ipV4Address[32] = {0};

	if( NULL == sip )
	{
		return 0;
	}
	if( len >= 32 )
	{
		return 0;
	}
	
	strcpy(ipV4Address, sip);
	ipV4Address[len] = '\0';

	if(sscanf(ipV4Address, "%u.%u.%u.%u", &n1, &n2, &n3, &n4) != 4) 
	{
		return 0;
	}
	if( (n1 != 0) && (n1 < 255) && (n2 <= 255) && (n3 <= 255) && (n4 != 0) && (n4 < 255) ) 
	{
		char buf[32];
		sprintf(buf, "%u.%u.%u.%u", n1, n2, n3, n4);
		if(strcmp(buf, ipV4Address))
		{
			return 0;
		}
		return 1;
	}
	return 0;	
}

int snmp2cmm_isIpv4NetmaskValid(uint8_t * sip, uint32_t len)
{
	int i = 0;
	int j = 0;
	int b = 0;
	long c[4] = {0x00};
	char ipV4Address[32] = {0};

	if( NULL == sip )
	{
		return 0;
	}
	if( len >= 32 )
	{
		return 0;
	}

	strcpy(ipV4Address, sip);
	ipV4Address[len] = '\0';

	if(sscanf(ipV4Address, "%u.%u.%u.%u", &c[3], &c[2], &c[1], &c[0]) != 4) 
	{
		return 0;
	}
	//printf("c[3]=%u, c[2]=%u, c[1]=%u, c[0]=%u\n", c[3], c[2], c[1], c[0]);
	/* 判断子网掩码是否有效*/
	/* 找出最后一个bit(1)，若其之前的所有位都为1，则为有效的掩码*/
	for( i=0;i<4;i++ )
	{
		for( j=0;j<8;j++)
		{
			if( 0 == b )
			{
				/* find the first bit '1' */
				if( 0 != ( c[i] & (1<<j) ) )
				{
					//printf(">i=%d,j=%d\n", i, j);
					b = 1;
					continue;
				}
			}
			else if( 0 == ( c[i] & (1<<j) ) )
			{
				//printf("<i=%d,j=%d\n", i, j);
				return 0;
			}		
		}
	}	
	return 1;	
}

int snmp2cmm_macs2b(const char *str, uint8_t *bin)
{
	int result, i;
	unsigned int mac[6];
	if((str == NULL) || (bin == NULL))
		return -1;
	if( strlen(str) == 0 )
	{
		for(i = 0; i < 6; i++)
		{
			bin[i] = 0;
		}
		return 0;
	}
	result = sscanf(str,"%2X:%2X:%2X:%2X:%2X:%2X",
						mac + 0, mac + 1, mac + 2,
						mac + 3, mac + 4, mac + 5);
	for(i = 0; i < 6; i++)
		bin[i] = mac[i];

	if(result == 6)
		return 0;
	else
		return -1;
}


int snmp2cmm_destroy(void)
{
	T_UDP_SK_INFO *SK = &SK_AGENT_CMM;
	if( SK->sk != 0)
	{
		//printf("\n##snmp2cmm_destroy\n");
		close(SK->sk);
		SK->sk = 0;
	}
	return CMM_SUCCESS;
}

int snmp2cmm_init(void)
{
	T_UDP_SK_INFO *SK = &SK_AGENT_CMM;

	if( SK->sk > 0 )
	{
		printf("socket_agent_cmm_init : socket is already created\n");
		return CMM_SUCCESS;
	}
	else
	{
		if( ( SK->sk = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
		{
			return CMM_CREATE_SOCKET_ERROR;
		}
		SK->skaddr.sin_family = AF_INET;
		SK->skaddr.sin_port = htons(CMM_LISTEN_PORT);		/* 目的端口号*/
		SK->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");	/* 目的地址*/
		//printf("socket_agent_cmm_init : created socket %d\n", SK->sk);
		//printf("snmp2cmm_init\n");
		return CMM_SUCCESS;
	}	
}


#if 0

int snmp2cmm_saveConfig(void)
{
	int ret = CMM_FAILED;
	uint8_t buf[MAX_UDP_SIZE];
	uint32_t buf_len = 0;

	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	T_CMM_MSG_ACK *confirm = (T_CMM_MSG_ACK *)buf;

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_SAVE_CONFIG;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = 0;
	
	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;	

	return __snmp2cmm_communicate(buf, buf_len);
}


int msg_agc_do_network_config(T_szNetwork *p_networkInfo)
{
	int ret = CMM_FAILED;
	uint8_t buf[MAX_UDP_SIZE];
	uint32_t buf_len = 0;

	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	T_CMM_MSG_ACK *confirm = (T_CMM_MSG_ACK *)buf;

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_WRITE_NETWORK_CONF;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = sizeof(T_szNetwork);
	
	memcpy(request->BUF, p_networkInfo, request->HEADER.ulBodyLength);
	
	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;	

	return msg_agent_cmm_communicate(buf, buf_len);
}


int msg_agc_get_snmpConfig(T_szSnmpConf *snmpConfig)
{
	int ret = CMM_FAILED;
	uint8_t buf[MAX_UDP_SIZE];
	uint32_t buf_len = 0;

	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	T_CMM_MSG_ACK *confirm = (T_CMM_MSG_ACK *)buf;
	T_szSnmpConf *msgData = (T_szSnmpConf *)(confirm->BUF);

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_READ_SNMP_CONF;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = 0;

	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;

	ret = msg_agent_cmm_communicate(buf, buf_len);
	if( 0 == ret )
	{
		memcpy(snmpConfig, msgData, sizeof(T_szSnmpConf));
	}
	return ret;
}

int msg_agc_set_snmpConfig(T_szSnmpConf *snmpConfig)
{
	uint8_t buf[MAX_UDP_SIZE];
	uint32_t buf_len = 0;

	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	T_CMM_MSG_ACK *confirm = (T_CMM_MSG_ACK *)buf;

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_WRITE_SNMP_CONF;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = sizeof(T_szSnmpConf);
	
	memcpy(request->BUF, snmpConfig, request->HEADER.ulBodyLength);
	
	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;	

	return msg_agent_cmm_communicate(buf, buf_len);
}

int msg_agc_get_mib_tbl_row(szTblVariables_t *tbl, void *b, uint32_t len)
{
	int ret = CMM_FAILED;
	uint8_t buf[MAX_UDP_SIZE];
	uint32_t buf_len = 0;
	
	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	T_CMM_MSG_ACK *confirm = (T_CMM_MSG_ACK *)buf;
	szTblVariables_t *msgData = (szTblVariables_t *)(confirm->BUF);

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_GET_SNMP_TABLE_ROW;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = sizeof(szTblVariables_t);

	memcpy(request->BUF, tbl, request->HEADER.ulBodyLength);

	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;

	ret = msg_agent_cmm_communicate(buf, buf_len);
	if( 0 == ret )
	{
		if( len != msgData->data_len )
		{
			ret = 4;
		}
		else
		{
			memcpy(b, msgData->data, len);
		}
	}
	return ret;	
}

int msg_agc_reset_all(void)
{
	int ret = CMM_FAILED;
	uint8_t buf[MAX_UDP_SIZE];
	uint32_t buf_len = 0;

	T_Msg_CMM *request = (T_Msg_CMM *)buf;
	T_CMM_MSG_ACK *confirm = (T_CMM_MSG_ACK *)buf;

	bzero(buf, MAX_UDP_SIZE);
	
	request->HEADER.usSrcMID = MID_SNMP;
	request->HEADER.usDstMID = MID_CMM;
	request->HEADER.usMsgType = CMM_EOC_ALL_RESET;
	request->HEADER.fragment = 0;
	request->HEADER.ulBodyLength = 0;
	
	buf_len = sizeof(request->HEADER) + request->HEADER.ulBodyLength;	

	return msg_agent_cmm_communicate(buf, buf_len);
}

#endif




