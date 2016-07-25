#include <assert.h>
#include "httpd2sysMonitor.h"

T_UDP_SK_INFO SK_HTTPD_SYSMONITOR;

int __httpd_sysmonitor_communicate(uint8_t *buf, uint32_t len)
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
	T_UDP_SK_INFO *sk = &SK_HTTPD_SYSMONITOR;

	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;
	uint16_t msgType = req->HEADER.usMsgType;

	sendn = sendto(sk->sk, buf, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		fprintf(stderr, "ERROR: httpd communicate with sysMonitor sendto failed\n");
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
			fprintf(stderr, "ERROR: httpd communicate with sysMonitor select failed\n");
			return -1;
		}
		
		// check whether a new connection comes
		if (FD_ISSET(sk->sk, &fdsr))
		{
			FromAddrSize = sizeof(from);
			rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);

			if ( -1 == rev_len )
			{
				fprintf(stderr, "ERROR: httpd communicate with sysMonitor recvfrom failed\n");
				return -1;
			}
			else
			{
				r = (T_COM_MSG_HEADER_ACK *)buf;
				if( r->usMsgType != msgType )
				{
					fprintf(stderr, "WARNNING: httpd communicate with sysMonitor msgType[%d != %d], [continue] !\n", 
						r->usMsgType, msgType);
					continue;
				}
				if( r->result )
				{
					fprintf(stderr, "ERROR: httpd communicate with sysMonitor result = %d\n", r->result);
				}
				return r->result;
			}			
		}
		else
		{
			fprintf(stderr, "ERROR: httpd communicate with sysMonitor FD_ISSET failed\n");
			return -1;
		}
	}
}

int httpd2sysmonitor_sysledCtrol(uint8_t event)
{
	uint32_t len = 0;
	uint8_t buf[MAX_UDP_SIZE] = {0};

	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;	
	uint8_t *req_data = (uint8_t *)(req->BUF);

	//printf("\ncmm2sysmonitor_sysledCtrol()\n");

	req->HEADER.usSrcMID = MID_HTTP;
	req->HEADER.usDstMID = MID_SYSMONITOR;
	req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
	req->HEADER.ulRequestID = 0;
	req->HEADER.ucFlag = 0;
	req->HEADER.usMsgType = CMM_SYSMONITOR_LED_CONTROL;
	req->HEADER.ulBodyLength = sizeof(uint8_t);

	*req_data = event;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;

	return __httpd_sysmonitor_communicate(buf, len);
}

int httpd2sysmonitor_destroy(void)
{
	T_UDP_SK_INFO *sk = &SK_HTTPD_SYSMONITOR;
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

int httpd2sysmonitor_init(void)
{
	T_UDP_SK_INFO *sk = &SK_HTTPD_SYSMONITOR;
	
	if( ( sk->sk = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}
	sk->skaddr.sin_family = AF_INET;
	sk->skaddr.sin_port = htons(SYSMONITOR_LISTEN_PORT);		/* 目的端口号*/
	sk->skaddr.sin_addr.s_addr = inet_addr("127.0.0.1");		/* 目的地址*/
	return CMM_SUCCESS;
}

