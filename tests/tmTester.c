/****************************************************************************************
*	文件名:tmTester.c
*	功能:模板管理 模块的测试程序
*	作者:frank
*	时间:2010-07-21
*
*****************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <linux/if.h>

#include "../include/public.h"
#include "../dbs/include/dbsapi.h"

static T_DBS_DEV_INFO *dev = NULL;

void __tmTester_usage(void)
{
	printf("\nParameter error:\n");
	printf("\nUsage:\n");
	printf("tmTester debug {enable|disable}\n");
	printf("tmTester test {1|2}\n");
	printf("	--1: module tm test calculate pib crc\n");
	printf("	--2: module tm test calculate mod crc\n");
	printf("	--3: module tm test gen pib\n");
	printf("	--4: module tm test gen mod\n");
	printf("	--5: module tm test calculate crc [loop]\n");
	printf("	--6: module tm test gen-desdroy [loop]\n");
	
	printf("\n\n");
}

void tmTester_signalProcessHandle(int n)
{
	printf("\n\n==================================================================\n");
	fprintf(stderr, "INFO: tmTester_signalProcessHandle close tmTester !\n");
	dbs_sys_log(dev, DBS_LOG_INFO, "tmTester_signalProcessHandle : module tmTester exit");
	dbsClose(dev);		
	exit(0);
}

int __tmTester_init_socket(T_UDP_SK_INFO *sk)
{
	/*创建外部SOCKET接口*/
	if( ( sk->sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		printf("\ncreate socket error, exited !\n");
		return -1;
	}
	
	sk->skaddr.sin_family = PF_INET;
	sk->skaddr.sin_port = htons(TM_CORE_LISTEN_PORT);
	sk->skaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	return 0;
}

int __tmTester_close_socket(T_UDP_SK_INFO *sk)
{
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return 0;
}

int __tmTester_communicate(T_UDP_SK_INFO *sk, uint8_t *buf, uint32_t len)
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

	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;
	uint16_t msgType = req->HEADER.usMsgType;

	sendn = sendto(sk->sk, buf, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		perror("tmTester_communicate call sendto error !\n");
		return -1;
	}

	while(1)
	{
		// initialize file descriptor set
		FD_ZERO(&fdsr);
		FD_SET(sk->sk, &fdsr);

		// timeout setting
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		
		maxsock = sk->sk;

		ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
		if( ret <= 0 )
		{
			printf("tmTester_communicate select time out !\n");
			return -1;
		}
		
		// check whether a new connection comes
		if (FD_ISSET(sk->sk, &fdsr))
		{
			FromAddrSize = sizeof(from);
			rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);

			if ( -1 == rev_len )
			{
				perror("tmTester_communicate call recvfrom error !\n");
				return -1;
			}
			else
			{
				r = (T_COM_MSG_HEADER_ACK *)buf;
				if( r->usMsgType != msgType )
				{
					fprintf(stderr, "WARNNING: tmTester_communicate: msgType[%d!=%d], [continue] !\n", 
						r->usMsgType, msgType);
					continue;
				}
				if( r->result )
				{
					printf("tmTester_communicate recieved server apply: result = %d\n", r->result);
				}
				return r->result;
			}			
		}
		else
		{
			printf("tmTester_communicate fd is not setted !\n");
			return -1;
		}
	}
}

int TEST_TM_MSG_DEBUG_ENABLE(T_UDP_SK_INFO *sk)
{
	T_COM_MSG_HEADER_REQ request;

	request.ucMsgAttrib = MSG_ATTRIB_REQ;
	request.ulRequestID = 0;
	request.usSrcMID = MID_TM_TESTER;
	request.usDstMID = MID_TEMPLATE;
	request.usMsgType = TM_CORE_MSG_DEBUG_ENABLE;
	request.ucFlag = 0;
	request.ulBodyLength = 0;

	return __tmTester_communicate(sk, (uint8_t *)&request, sizeof(T_COM_MSG_HEADER_REQ));
}

int TEST_TM_MSG_DEBUG_DISABLE(T_UDP_SK_INFO *sk)
{
	T_COM_MSG_HEADER_REQ request;

	request.ucMsgAttrib = MSG_ATTRIB_REQ;
	request.ulRequestID = 0;
	request.usSrcMID = MID_TM_TESTER;
	request.usDstMID = MID_TEMPLATE;
	request.usMsgType = TM_CORE_MSG_DEBUG_DISABLE;
	request.ucFlag = 0;
	request.ulBodyLength = 0;

	return __tmTester_communicate(sk, (uint8_t *)&request, sizeof(T_COM_MSG_HEADER_REQ));
}

int TEST_TM_GEN_PIB_CRC(T_UDP_SK_INFO *sk)
{
	//int ret = 0;
	int n = 0;
	int len = 0;
	int opt_sts = 0;
	T_COM_MSG_HEADER_REQ request;
	stAutoGenCrc stCrc;
	stAutoGenCrc *Crc;
	uint8_t buf[MAX_UDP_SIZE];

	request.ucMsgAttrib = MSG_ATTRIB_REQ;
	request.ulRequestID = 0;
	request.usSrcMID = MID_TM_TESTER;
	request.usDstMID = MID_TEMPLATE;
	request.usMsgType = TM_CORE_GET_CRC;
	request.ucFlag = 0;
	request.ulBodyLength = sizeof(stAutoGenCrc);

	printf("Please input clt id(1~4):");
	scanf("%d", &n);
	stCrc.clt = n;
	printf("Please input cnu id(1~64):");
	scanf("%d", &n);
	stCrc.cnu = n;	
	stCrc.flag = TM_AUTO_GEN_PIB;

	memcpy(buf, &request, sizeof(T_COM_MSG_HEADER_REQ));
	memcpy(buf+sizeof(T_COM_MSG_HEADER_REQ), &stCrc, sizeof(stAutoGenCrc));
	len = sizeof(T_COM_MSG_HEADER_REQ) + sizeof(stAutoGenCrc);

	opt_sts = __tmTester_communicate(sk, buf, len);
	if( CMM_SUCCESS == opt_sts)
	{
		Crc = (stAutoGenCrc *) (buf + sizeof(T_COM_MSG_HEADER_ACK));
		printf("\r\n  clt %d cnu %d crc: [%x]\n", stCrc.clt, stCrc.cnu, Crc->crc);
	}
	else
	{
		printf("ERROR: TEST_TM_GEN_PIB_CRC->__tmTester_communicate !");
	}
	
	return opt_sts;
}

int TEST_TM_GEN_MOD_CRC(T_UDP_SK_INFO *sk)
{
	//int ret = 0;
	int n = 0;
	int len = 0;
	int opt_sts = 0;
	T_COM_MSG_HEADER_REQ request;
	stAutoGenCrc stCrc;
	stAutoGenCrc *Crc;
	uint8_t buf[MAX_UDP_SIZE];

	request.ucMsgAttrib = MSG_ATTRIB_REQ;
	request.ulRequestID = 0;
	request.usSrcMID = MID_TM_TESTER;
	request.usDstMID = MID_TEMPLATE;
	request.usMsgType = TM_CORE_GET_CRC;
	request.ucFlag = 0;
	request.ulBodyLength = sizeof(stAutoGenCrc);

	stCrc.flag = TM_AUTO_GEN_MOD;
	printf("Please input clt id(1~4):");
	scanf("%d", &n);
	stCrc.clt = n;
	printf("Please input cnu id(1~64):");
	scanf("%d", &n);
	stCrc.cnu = n;

	memcpy(buf, &request, sizeof(T_COM_MSG_HEADER_REQ));
	memcpy(buf+sizeof(T_COM_MSG_HEADER_REQ), &stCrc, sizeof(stAutoGenCrc));
	len = sizeof(T_COM_MSG_HEADER_REQ) + sizeof(stAutoGenCrc);

	opt_sts = __tmTester_communicate(sk, buf, len);
	
	if( CMM_SUCCESS == opt_sts)
	{
		Crc = (stAutoGenCrc *) (buf + sizeof(T_COM_MSG_HEADER_ACK));
		printf("\r\n  clt %d cnu %d crc: [%x]\n", stCrc.clt, stCrc.cnu, Crc->crc);
	}
	else
	{
		printf("ERROR: TEST_TM_GEN_MOD_CRC->__tmTester_communicate !");
	}
	return opt_sts;
}

int TEST_TM_LOOP_A(T_UDP_SK_INFO *sk)
{
	int n = 0;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;
	stAutoGenCrc *req_crc = (stAutoGenCrc *)(req->BUF);

	//T_COM_MSG_PACKET_ACK *ack = (T_COM_MSG_PACKET_ACK *)buf;
	//stAutoGenCrc *ack_crc = (stAutoGenCrc *)(ack->BUF);

	stAutoGenCrc crcInput;

	printf("Please input clt id(1~4):");
	scanf("%d", &n);
	crcInput.clt = n;
	printf("Please input cnu id(1~64):");
	scanf("%d", &n);
	crcInput.cnu = n;	
	crcInput.crc = 0;

	while(1)
	{
		bzero(buf, MAX_UDP_SIZE);

		req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
		req->HEADER.ulRequestID = 0;
		req->HEADER.usSrcMID = MID_TM_TESTER;
		req->HEADER.usDstMID = MID_TEMPLATE;
		req->HEADER.usMsgType = TM_CORE_GET_CRC;
		req->HEADER.ucFlag = 0;
		req->HEADER.ulBodyLength = sizeof(stAutoGenCrc);

		req_crc->clt = crcInput.clt;
		req_crc->cnu = crcInput.cnu;
		req_crc->crc = 0;
		req_crc->flag = TM_AUTO_GEN_PIB;

		len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
		if( CMM_SUCCESS != __tmTester_communicate(sk, buf, len) )
		{
			printf("ERROR: TEST_TM_LOOP_A->__tmTester_communicate, exit !");
			break;
		}

		bzero(buf, MAX_UDP_SIZE);

		req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
		req->HEADER.ulRequestID = 0;
		req->HEADER.usSrcMID = MID_TM_TESTER;
		req->HEADER.usDstMID = MID_TEMPLATE;
		req->HEADER.usMsgType = TM_CORE_GET_CRC;
		req->HEADER.ucFlag = 0;
		req->HEADER.ulBodyLength = sizeof(stAutoGenCrc);

		req_crc->clt = crcInput.clt;
		req_crc->cnu = crcInput.cnu;
		req_crc->crc = 0;
		req_crc->flag = TM_AUTO_GEN_MOD;

		len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
		if( CMM_SUCCESS != __tmTester_communicate(sk, buf, len) )
		{
			printf("ERROR: TEST_TM_LOOP_A->__tmTester_communicate, exit !");
			break;
		}
	}
	return 0;
}

int TEST_TM_LOOP_B(T_UDP_SK_INFO *sk)
{
	int n = 0;
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;
	stAutoGenConfig *req_data = (stAutoGenConfig *)(req->BUF);


	stAutoGenConfig dataInput;

	printf("Please input clt id(1~4):");
	scanf("%d", &n);
	dataInput.clt = n;
	printf("Please input cnu id(1~64):");
	scanf("%d", &n);
	dataInput.cnu = n;	

	while(1)
	{
		/* gen pib */
		bzero(buf, MAX_UDP_SIZE);

		req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
		req->HEADER.ulRequestID = 0;
		req->HEADER.usSrcMID = MID_TM_TESTER;
		req->HEADER.usDstMID = MID_TEMPLATE;
		req->HEADER.usMsgType = TM_CORE_GEN_CONF;
		req->HEADER.ucFlag = 0;
		req->HEADER.ulBodyLength = sizeof(stAutoGenConfig);

		req_data->clt = dataInput.clt;
		req_data->cnu = dataInput.cnu;
		req_data->flag = TM_AUTO_GEN_PIB;

		len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
		if( CMM_SUCCESS != __tmTester_communicate(sk, buf, len) )
		{
			printf("ERROR: TEST_TM_LOOP_B->__tmTester_communicate, exit !");
			break;
		}

		/* destroy pib */
		bzero(buf, MAX_UDP_SIZE);

		req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
		req->HEADER.ulRequestID = 0;
		req->HEADER.usSrcMID = MID_TM_TESTER;
		req->HEADER.usDstMID = MID_TEMPLATE;
		req->HEADER.usMsgType = TM_CORE_DESTROY_CONF;
		req->HEADER.ucFlag = 0;
		req->HEADER.ulBodyLength = sizeof(stAutoGenConfig);

		req_data->clt = dataInput.clt;
		req_data->cnu = dataInput.cnu;
		req_data->flag = TM_AUTO_GEN_PIB;

		len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
		if( CMM_SUCCESS != __tmTester_communicate(sk, buf, len) )
		{
			printf("ERROR: TEST_TM_LOOP_B->__tmTester_communicate, exit !");
			break;
		}

		/* gen mod */
		bzero(buf, MAX_UDP_SIZE);

		req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
		req->HEADER.ulRequestID = 0;
		req->HEADER.usSrcMID = MID_TM_TESTER;
		req->HEADER.usDstMID = MID_TEMPLATE;
		req->HEADER.usMsgType = TM_CORE_GEN_CONF;
		req->HEADER.ucFlag = 0;
		req->HEADER.ulBodyLength = sizeof(stAutoGenConfig);

		req_data->clt = dataInput.clt;
		req_data->cnu = dataInput.cnu;
		req_data->flag = TM_AUTO_GEN_MOD;

		len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
		if( CMM_SUCCESS != __tmTester_communicate(sk, buf, len) )
		{
			printf("ERROR: TEST_TM_LOOP_B->__tmTester_communicate, exit !");
			break;
		}

		/* destroy mod */
		bzero(buf, MAX_UDP_SIZE);

		req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
		req->HEADER.ulRequestID = 0;
		req->HEADER.usSrcMID = MID_TM_TESTER;
		req->HEADER.usDstMID = MID_TEMPLATE;
		req->HEADER.usMsgType = TM_CORE_DESTROY_CONF;
		req->HEADER.ucFlag = 0;
		req->HEADER.ulBodyLength = sizeof(stAutoGenConfig);

		req_data->clt = dataInput.clt;
		req_data->cnu = dataInput.cnu;
		req_data->flag = TM_AUTO_GEN_MOD;

		len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;
		if( CMM_SUCCESS != __tmTester_communicate(sk, buf, len) )
		{
			printf("ERROR: TEST_TM_LOOP_B->__tmTester_communicate, exit !");
			break;
		}
	}
	return 0;
}

int TEST_TM_GEN_PIB(T_UDP_SK_INFO *sk)
{
	int n = 0;	
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;
	stAutoGenConfig *stCon = (stAutoGenConfig *)(req->BUF);

	req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
	req->HEADER.ulRequestID = 0;
	req->HEADER.usSrcMID = MID_TM_TESTER;
	req->HEADER.usDstMID = MID_TEMPLATE;
	req->HEADER.usMsgType = TM_CORE_GEN_CONF;
	req->HEADER.ucFlag = 0;
	req->HEADER.ulBodyLength = sizeof(stAutoGenConfig);

	printf("Please input clt id(1~4):");
	scanf("%d", &n);
	stCon->clt = n;
	printf("Please input cnu id(1~64):");
	scanf("%d", &n);
	stCon->cnu = n;	
	stCon->flag = TM_AUTO_GEN_PIB;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;	
	return __tmTester_communicate(sk, buf, len);
}

int TEST_TM_GEN_MOD(T_UDP_SK_INFO *sk)
{
	int n = 0;	
	uint8_t buf[MAX_UDP_SIZE] = {0};
	uint32_t len = 0;
	
	T_COM_MSG_PACKET_REQ *req = (T_COM_MSG_PACKET_REQ *)buf;
	stAutoGenConfig *stCon = (stAutoGenConfig *)(req->BUF);

	req->HEADER.ucMsgAttrib = MSG_ATTRIB_REQ;
	req->HEADER.ulRequestID = 0;
	req->HEADER.usSrcMID = MID_TM_TESTER;
	req->HEADER.usDstMID = MID_TEMPLATE;
	req->HEADER.usMsgType = TM_CORE_GEN_CONF;
	req->HEADER.ucFlag = 0;
	req->HEADER.ulBodyLength = sizeof(stAutoGenConfig);

	printf("Please input clt id(1~4):");
	scanf("%d", &n);
	stCon->clt = n;
	printf("Please input cnu id(1~64):");
	scanf("%d", &n);
	stCon->cnu = n;	
	stCon->flag = TM_AUTO_GEN_MOD;

	len = sizeof(req->HEADER) + req->HEADER.ulBodyLength;	
	return __tmTester_communicate(sk, buf, len);
}

int main(int argc, char *argv[])
{
	T_UDP_SK_INFO sk;
	
	if( argc != 3 )
	{
		__tmTester_usage();
		return 0;
	}

	dev = dbsNoWaitOpen(MID_DBS_TESTER);
	if( NULL == dev )
	{
		return 0;
	}
	
	if( 0 != __tmTester_init_socket(&sk) )
	{
		dbs_sys_log(dev, DBS_LOG_ERR, "tmTester_init_socket error");
		dbsClose(dev);
		return 0;
	}

	/* 注册异常退出句柄函数*/
	signal(SIGTERM, tmTester_signalProcessHandle);
	signal(SIGINT, tmTester_signalProcessHandle);

	
	if( strcmp(argv[1], "debug") == 0)
	{
		if( strcmp(argv[2], "enable") == 0)
		{
			TEST_TM_MSG_DEBUG_ENABLE(&sk);
		}
		else if( strcmp(argv[2], "disable") == 0)
		{
			TEST_TM_MSG_DEBUG_DISABLE(&sk);
		}
		else
		{
			__tmTester_usage();
		}
	}
	else if( strcmp(argv[1], "test") == 0)
	{
		int cmd = atoi(argv[2]);
		switch(cmd)
		{
			case 1:
				TEST_TM_GEN_PIB_CRC(&sk);
				break;
			case 2:
				TEST_TM_GEN_MOD_CRC(&sk);
				break;
			case 3:
				TEST_TM_GEN_PIB(&sk);
				break;
			case 4:
				TEST_TM_GEN_MOD(&sk);
				break;
			case 5:
				TEST_TM_LOOP_A(&sk);
				break;
			case 6:
				TEST_TM_LOOP_B(&sk);
				break;
			default:
				__tmTester_usage();
				break;
		}
	}
	else
	{
		__tmTester_usage();
	}
	printf("\n\n==================================================================\n");
	dbs_sys_log(dev, DBS_LOG_INFO, "module tmTester exit");
	dbsClose(dev);
	__tmTester_close_socket(&sk);
	return 0;
}


