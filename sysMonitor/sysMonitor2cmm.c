#include "sysMonitor2cmm.h"

//static T_UDP_SK_INFO SK_SYSMONITOR2CMM;

int __sysMonitor2cmm_comm(T_UDP_SK_INFO *sk, uint8_t *buf, uint32_t len)
{
	//T_UDP_SK_INFO *sk = &SK_SYSMONITOR2CMM;
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
		//dbs_mutex_sys_log(DBS_LOG_CRIT, "sysMonitor2cmm sendto failed");
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
			//dbs_mutex_sys_log(DBS_LOG_CRIT, "sysMonitor2cmm select failed");
			return CMM_FAILED;
		}
		// check whether a new connection comes
		if (FD_ISSET(sk->sk, &fdsr))
		{
			FromAddrSize = sizeof(from);
			rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);
			if ( -1 == rev_len )
			{
				//dbs_mutex_sys_log(DBS_LOG_CRIT, "sysMonitor2cmm recvfrom failed");
				return CMM_FAILED;
			}
			else
			{
				ack = (T_REQ_Msg_CMM *)buf;
				if( msgType != ack->HEADER.usMsgType )
				{
					fprintf(stderr, "WARNNING: sysMonitor2cmm: msgType[%d!=%d], [continue] !\n", 
						ack->HEADER.usMsgType, msgType);
					//dbs_mutex_sys_log(DBS_LOG_WARNING, "sysMonitor2cmm received non-mached msg type");
					continue;
				}
				else if( ack->result != CMM_SUCCESS )
				{	
					//dbs_mutex_sys_log(DBS_LOG_CRIT, "sysMonitor2cmm recvfrom result error");
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
			//dbs_mutex_sys_log(DBS_LOG_CRIT, "sysMonitor2cmm FD_ISSET failed");
			return CMM_FAILED;
		}
	}
}

int sysMonitor2cmm_sysReboot(T_UDP_SK_INFO *sk)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_SYSMONITOR;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_CBAT_RESET;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __sysMonitor2cmm_comm(sk, buf, len);
}

int sysMonitor2cmm_restoreDefault(T_UDP_SK_INFO *sk)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_Msg_CMM *req = (T_Msg_CMM *)buf;

	req->HEADER.usSrcMID = MID_SYSMONITOR;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_RESTORE_DEFAULT;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	return __sysMonitor2cmm_comm(sk, buf, len);
}

int sysMonitor2cmm_getCbatTemperature(T_UDP_SK_INFO *sk, st_temperature *temp_data)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_Msg_CMM *req = (T_Msg_CMM *)buf;
	
	T_REQ_Msg_CMM *ack = (T_REQ_Msg_CMM *)buf;
	st_temperature *ack_data = (st_temperature *)(ack->BUF);

	req->HEADER.usSrcMID = MID_SYSMONITOR;
	req->HEADER.usDstMID = MID_CMM;
	req->HEADER.usMsgType = CMM_GET_CBAT_TEMPERATURE;
	req->HEADER.ulBodyLength = 0;
	req->HEADER.fragment = 0;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
	if( len > MAX_UDP_SIZE )
	{
		printf("sysMonitor2cmm_getCbatTemperature memery error\n");
		return CMM_FAILED;
	}

	if( CMM_SUCCESS == __sysMonitor2cmm_comm(sk, buf, len) )
	{
		memcpy(temp_data, ack_data, sizeof(st_temperature));
		return CMM_SUCCESS;
	}	
	return CMM_FAILED;
}

int sysMonitor2cmm_destroy(T_UDP_SK_INFO *sk)
{
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int sysMonitor2cmm_init(T_UDP_SK_INFO *sk)
{
	if( ( sk->sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_FAILED;
	}	

	sk->skaddr.sin_family = PF_INET;
	sk->skaddr.sin_port = htons(CMM_LISTEN_PORT);		/* Ä¿µÄ¶Ë¿ÚºÅ*/
	sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	return CMM_SUCCESS;
} 

