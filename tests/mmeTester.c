/****************************************************************************************
*	文件名:tmmead.c
*	功能:mmead 模块的测试程序
*	作者:frank
*	时间:2010-11-30
*
*****************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <linux/if.h>
#include <public.h>
#include <boardapi.h>

uint8_t cltMac[6] = {0};
uint8_t oda_atheros[6] = {0x00, 0xb0, 0x52, 0x00, 0x00, 0x01};
uint8_t oda[6] = {0x00, 0xb0, 0x52, 0x00, 0x00, 0x01};
//uint8_t oda_clt[6] = {0x30, 0x71, 0xb2, 0x00, 0x00, 0x01};

void print_usage(void)
{
	printf("\nParameter error:\n");
	printf("\nUsage:\n");
	printf("mmeTester debug {on|off}\n");
	printf("	--on:	turn on mmead module debug massage\n");
	printf("	--off:	turn on mmead module debug massage\n");	
	printf("mmeTester test {1|2|...} <mac> [CNU_HFID]\n");
	printf("	--1:	test mmead case [MMEAD_READ_MODULE_OPERATION]\n");
	printf("	--2:	test mmead case [MMEAD_WRITE_MODULE_OPERATION]\n");
	printf("	--3:	test mmead case [MMEAD_GET_CLT_MAC]\n");
	printf("	--4:	test mmead case [MMEAD_GET_TOPOLOGY]\n");
	printf("	--5:	test mmead case [MMEAD_GET_SOFTWARE_VERSION]\n");
	printf("	--6:	test mmead case [MMEAD_GET_MANUFACTURER_INFO]\n");
	printf("	--7:	test mmead case [MMEAD_READ_MODULE_OPERATION_CRC]\n");
	printf("	--8:	test mmead case [MMEAD_RESET_DEV]\n");
	printf("	--9:	test mmead case [MMEAD_READ_PIB_CRC]\n");
	printf("	--10:	test mmead case [MMEAD_WRITE_PIB]\n");
	printf("	--11:	test mmead case [MMEAD_BOOT_OUT_CNU]\n");
	printf("	--12:	test mmead case [MMEAD_GET_TOPOLOGY] [LOOP]\n");
	printf("	--13:	test mmead case [MMEAD_LINK_DIAG:RX] \n");
	printf("	--14:	test mmead case [MMEAD_LINK_DIAG:TX] \n");
	printf("	--15:	test mmead read pib from cnu \n");
	printf("	--16:	test mmead case [MMEAD_GET_TOPOLOGY_STATS]\n");
	printf("	--17:	test mmead case [MMEAD_GET_FREQUENCY_BAND_SELECTION]\n");
	printf("	--18:	test mmead case [MMEAD_SET_FREQUENCY_BAND_SELECTION]\n");
	printf("		set to sub-band mode\n");
	printf("	--19:	test mmead case [MMEAD_SET_FREQUENCY_BAND_SELECTION]\n");
	printf("		set to full-band mode\n");
	printf("	--20:	test mmead case [MMEAD_SET_FREQUENCY_BAND_SELECTION]\n");
	printf("		disable frequency band selection\n");
	printf("	--21:	test mmead case [MMEAD_GET_TX_GAIN]\n");
	printf("		get eoc tx gain value\n");
	printf("	--22:	test mmead case [MMEAD_SET_TX_GAIN]\n");
	printf("		increase tx gain value by 1db\n");
	printf("	--23:	test mmead case [MMEAD_SET_TX_GAIN]\n");
	printf("		decrease tx gain value by 1db\n");
	printf("	--24:	test mmead case [MMEAD_GET_RTL8306E_CONFIG]\n");
	printf("	--25:	test mmead case [MMEAD_GET_CNU_HFID]\n");
	
	printf("\n\n");
}

int init_socket(T_UDP_SK_INFO *sk)
{
	/*创建外部SOCKET接口*/
	if( ( sk->sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		printf("\ncreate socket error, exited !\n");
		return -1;
	}
	
	sk->skaddr.sin_family = PF_INET;
	sk->skaddr.sin_port = htons(MMEAD_LISTEN_PORT);
	sk->skaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	return 0;
}

int close_socket(T_UDP_SK_INFO *sk)
{
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return 0;
}

int gen_mod_file_path(uint8_t *path, const uint8_t ODA[])
{
	//uint8_t path_mac[32] = {0};
	sprintf(path, "/var/tmp/%02X_%02X_%02X_%02X_%02X_%02X.BIN", 
		ODA[0], ODA[1], ODA[2], ODA[3], ODA[4], ODA[5]);
	//debug_printf("gen_mod_file_path = [%s]\n", path);
	return 0;
}

int msg_communicate(T_UDP_SK_INFO *sk, uint8_t *buf, uint32_t len)
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

	sendn = sendto(sk->sk, buf, len, 0, (struct sockaddr *)&(sk->skaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		perror("MOD tester call sendto error, continue.\n");
		return -1;
	}
	
	// initialize file descriptor set
	FD_ZERO(&fdsr);
	FD_SET(sk->sk, &fdsr);

	// timeout setting
	tv.tv_sec = 60;
	tv.tv_usec = 0;
	
	maxsock = sk->sk;

	ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
	if( ret <= 0 )
	{
		printf("Select time out\n");
		return -1;
	}
	
	// check whether a new connection comes
	if (FD_ISSET(sk->sk, &fdsr))
	{
		FromAddrSize = sizeof(from);
		rev_len = recvfrom(sk->sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);

		if ( -1 == rev_len )
		{
			perror("DB tester call recvfrom error, continue.\n");
			return -1;
		}
		else
		{
			r = (T_REQ_Msg_MMEAD *)buf;
			if( MMEAD_MSG_ID != r->HEADER.M_TYPE )
			{
				perror("Non-matched msg revieved by MMEAD tester, skip.\n");
				return -1;
			}
			
			if( r->result )
			{
				printf("MOD tester recieved server apply: result = %d\n", r->result);
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

int TEST_MODULE_OPERATION_READ(T_UDP_SK_INFO *sk)
{
	FILE *fp;
	uint8_t MOD_PATH[64] = {0};
	T_Msg_Header_MMEAD h;
	
	uint8_t buf[MAX_UDP_SIZE];
	T_MMEAD_RD_MOD_REQ_INFO p_mod_date; 
	T_REQ_Msg_MMEAD *r = (T_REQ_Msg_MMEAD *)buf;
	T_MMEAD_RD_MOD_ACK_INFO *comfirm = (T_MMEAD_RD_MOD_ACK_INFO *)(r->BUF);

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = MMEAD_READ_MODULE_OPERATION;
	h.fragment = 0;
	memcpy(h.ODA, oda, sizeof(oda));
	h.LEN = sizeof(p_mod_date);

	p_mod_date.MODULE_ID = 0x1000;
	p_mod_date.MODULE_SUB_ID = 0;
	p_mod_date.LENGTH = 1024;
	p_mod_date.OFFSET = 0;	

	memcpy(buf, &h, sizeof(h));
	memcpy(buf+sizeof(h), &p_mod_date, sizeof(p_mod_date));
	if( 0 == msg_communicate(sk, buf, sizeof(h)+sizeof(p_mod_date)) )
	{
		printf("Read Module Data Len : %x\n", comfirm->LENGTH);
		gen_mod_file_path(MOD_PATH, oda);
		if( (fp = fopen(MOD_PATH, "wb+")) < 0 )
		{
			perror("\r\n  create mod Error!");
			return 1;
		}

		if(fwrite(comfirm->MODULE_DATA, comfirm->LENGTH, 1, fp) != 1)
		{
			perror("\r\n  Pib Write Error!");
			
			return 1;
		}

		fclose(fp);
		return 0;
	}

	return -1;
}

int TEST_MMEAD_GET_MOD_CRC(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	
	uint8_t buf[MAX_UDP_SIZE];
	T_REQ_Msg_MMEAD *r = (T_REQ_Msg_MMEAD *)buf;

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = MMEAD_READ_MODULE_OPERATION_CRC;
	h.fragment = 0;
	memcpy(h.ODA, oda, sizeof(oda));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		printf("Read Module Data CRC : 0x%x\n", *(uint32_t *)(r->BUF));
		return 0;
	}

	return -1;
}

int TEST_MMEAD_GET_PIB_CRC(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	
	uint8_t buf[MAX_UDP_SIZE];
	T_REQ_Msg_MMEAD *r = (T_REQ_Msg_MMEAD *)buf;

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = MMEAD_READ_PIB_CRC;
	h.fragment = 0;
	memcpy(h.ODA, oda, sizeof(oda));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		printf("Read PIB CRC : 0x%x\n", *(uint32_t *)(r->BUF));
		return 0;
	}

	return -1;
}

int TEST_MMEAD_READ_PIB(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	
	uint8_t buf[MAX_UDP_SIZE];
	T_REQ_Msg_MMEAD *r = (T_REQ_Msg_MMEAD *)buf;

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = MMEAD_READ_PIB;
	h.fragment = 0;
	memcpy(h.ODA, oda, sizeof(oda));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		printf("\n  Success\n");
		return 0;
	}
	else
	{
		printf("\n  Failed\n");
		return -1;
	}	
}

int TEST_MODULE_OPERATION_WRITE(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	int r_success,r_failed,r_count;
	//T_MMEAD_WR_MOD_REQ_INFO p_mod_date;
	
	uint8_t buf[MAX_UDP_SIZE];
	/*
	uint8_t ar8236_mii_phy_mode_init_data[76] = {
		0x01, 0x03, 0x85, 0x81, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0xAD, 0x3F, 0x00, 0xFF, 0xFF, 0x05, 0xAF,
		0x3F, 0x7E, 0xFF, 0xFF, 0x85, 0x81, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0x85, 0x00, 0x05, 0xFF, 0xFF,
		0x05, 0x87, 0x00, 0x00, 0xFF, 0xFF, 0x85, 0x81, 0x00, 0x00, 0xFF, 0xFF, 0x45, 0x81, 0x7D, 0x00,
		0xFF, 0xFF, 0x45, 0x83, 0x00, 0x00, 0xFF, 0xFF, 0x85, 0x81, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0xB1,
		0xF2, 0x05, 0xFF, 0xFF, 0x05, 0xB3, 0xF0, 0x19, 0xFF, 0xFF, 0x00, 0x00		
	};*/
	 
	T_REQ_Msg_MMEAD *r = (T_REQ_Msg_MMEAD *)buf;
	//T_MMEAD_WR_MOD_ACK_INFO *comfirm = (T_MMEAD_WR_MOD_ACK_INFO *)(r->BUF);

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = MMEAD_WRITE_MODULE_OPERATION;
	h.fragment = 0;
	memcpy(h.ODA, oda, sizeof(oda));
	//h.LEN = sizeof(p_mod_date);
	h.LEN = 0;
	//printf("h.LEN = %d\n", h.LEN);
	/*
	p_mod_date.MODULE_ID = 0x1000;
	p_mod_date.MODULE_SUB_ID = 0;
	p_mod_date.MODULE_LENGTH = 76;

	if (p_mod_date.MODULE_LENGTH > 1400) 
	{
		printf("lseek error 1!\n");
	}
	if (p_mod_date.MODULE_LENGTH % sizeof (uint32_t)) 
	{
		printf("lseek error 2!\n");
	}
	*/
	
	//memcpy(p_mod_date.MODULE_DATA, ar8236_mii_phy_mode_init_data, 76);
	memcpy(buf, &h, sizeof(h));
	//memcpy(buf+sizeof(h), &p_mod_date, sizeof(p_mod_date));
	for(r_count = 0; r_count<1000; r_count++)
	{
		memcpy(buf, &h, sizeof(h));
		printf("=========> : %d\n", r_count);
		if( 0 == msg_communicate(sk, buf, sizeof(h)) )
		{
			printf("Write Module Data status : %d\n", r->result);
			if(r->result == 0)
			{
				r_success++;
			}
			else
			{
				r_failed++;
			}
			
		}
		else
		{
			//continue;
		}
		sleep(15);
		
	}
	
	printf("success : %d\n failed:%d\n", r_success,r_failed);
	return -1;
}

int TEST_MMEAD_GET_CLT_MAC(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = NULL;
	uint8_t buf[MAX_UDP_SIZE];
	
	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_GET_CLT_MAC;
	h.fragment = 0;
	memcpy(h.ODA, oda_atheros, sizeof(oda_atheros));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		r = (T_REQ_Msg_MMEAD *)buf;
		memcpy(cltMac, r->BUF, 6);
		printf("TEST_MMEAD_GET_CLT_MAC : MAC = [%02X:%02X:%02X:%02X:%02X:%02X]\n",
						cltMac[0], cltMac[1], cltMac[2], cltMac[3], cltMac[4], cltMac[5]);
		return 0;
	}
	return -1;
}

int TEST_MMEAD_GET_FBS(T_UDP_SK_INFO *sk)
{
	uint8_t buf[MAX_UDP_SIZE];
	T_MMETS_REQ_MSG *request = (T_MMETS_REQ_MSG *)buf;
	T_MMETS_ACK_MSG *reply = (T_MMETS_ACK_MSG *)buf;
	T_MMEAD_FBS *p = (T_MMEAD_FBS *)(reply->body);
	
	
	request->header.M_TYPE = 0xCC08;
	request->header.DEV_TYPE = WEC701_M0;
	request->header.MM_TYPE = MMEAD_GET_FREQUENCY_BAND_SELECTION;
	request->header.fragment = 0;
	memcpy(request->header.ODA, oda, sizeof(oda));
	request->header.LEN = 0;

	
	if( 0 == msg_communicate(sk, buf, sizeof(T_MMETS_REQ_MSG)) )
	{
		printf("	FBSTATUS = [%d]\n", p->FBSTATUS);
		printf("	START_BAND = [%d]\n", p->START_BAND);
		printf("	STOP_BAND = [%d]\n", p->STOP_BAND);
		return 0;
	}
	return -1;
}

int TEST_MMEAD_SET_FBS_SUBBAND(T_UDP_SK_INFO *sk)
{
	uint8_t buf[MAX_UDP_SIZE];
	int len = 0;
	T_MMETS_REQ_MSG *request = (T_MMETS_REQ_MSG *)buf;
	T_MMEAD_FBS *p = (T_MMEAD_FBS *)(request->body);
	
	T_MMETS_ACK_MSG *reply = (T_MMETS_ACK_MSG *)buf;	
	
	request->header.M_TYPE = 0xCC08;
	request->header.DEV_TYPE = WEC701_M0;
	request->header.MM_TYPE = MMEAD_SET_FREQUENCY_BAND_SELECTION;
	request->header.fragment = 0;
	memcpy(request->header.ODA, oda, sizeof(oda));
	request->header.LEN = sizeof(T_MMEAD_FBS);

	p->FBSTATUS = 1;
	p->START_BAND = 0;
	p->STOP_BAND = 1154;

	len = sizeof(request->header) + request->header.LEN;
	
	return msg_communicate(sk, buf, len);
}

int TEST_MMEAD_SET_FBS_FULLBAND(T_UDP_SK_INFO *sk)
{
	uint8_t buf[MAX_UDP_SIZE];
	int len = 0;
	T_MMETS_REQ_MSG *request = (T_MMETS_REQ_MSG *)buf;
	T_MMEAD_FBS *p = (T_MMEAD_FBS *)(request->body);
	
	T_MMETS_ACK_MSG *reply = (T_MMETS_ACK_MSG *)buf;	
	
	request->header.M_TYPE = 0xCC08;
	request->header.DEV_TYPE = WEC701_M0;
	request->header.MM_TYPE = MMEAD_SET_FREQUENCY_BAND_SELECTION;
	request->header.fragment = 0;
	memcpy(request->header.ODA, oda, sizeof(oda));
	request->header.LEN = sizeof(T_MMEAD_FBS);

	p->FBSTATUS = 1;
	p->START_BAND = 0;
	p->STOP_BAND = 2689;

	len = sizeof(request->header) + request->header.LEN;
	
	return msg_communicate(sk, buf, len);
}

int TEST_MMEAD_SET_FBS_DISABLE(T_UDP_SK_INFO *sk)
{
	uint8_t buf[MAX_UDP_SIZE];
	int len = 0;
	T_MMETS_REQ_MSG *request = (T_MMETS_REQ_MSG *)buf;
	T_MMEAD_FBS *p = (T_MMEAD_FBS *)(request->body);
	
	T_MMETS_ACK_MSG *reply = (T_MMETS_ACK_MSG *)buf;	
	
	request->header.M_TYPE = 0xCC08;
	request->header.DEV_TYPE = WEC701_M0;
	request->header.MM_TYPE = MMEAD_SET_FREQUENCY_BAND_SELECTION;
	request->header.fragment = 0;
	memcpy(request->header.ODA, oda, sizeof(oda));
	request->header.LEN = sizeof(T_MMEAD_FBS);

	p->FBSTATUS = 0;
	p->START_BAND = 0;
	p->STOP_BAND = 0;

	len = sizeof(request->header) + request->header.LEN;
	
	return msg_communicate(sk, buf, len);
}

int TEST_MMEAD_GET_TX_GAIN_VALUE(T_UDP_SK_INFO *sk)
{
	uint8_t buf[MAX_UDP_SIZE];
	T_MMETS_REQ_MSG *request = (T_MMETS_REQ_MSG *)buf;
	T_MMETS_ACK_MSG *reply = (T_MMETS_ACK_MSG *)buf;
	uint8_t *p = (uint8_t *)(reply->body);
	uint8_t temp = 0;
	int tx_gain = 0;
	
	request->header.M_TYPE = 0xCC08;
	request->header.DEV_TYPE = WEC701_M0;
	request->header.MM_TYPE = MMEAD_GET_TX_GAIN;
	request->header.fragment = 0;
	memcpy(request->header.ODA, oda, sizeof(oda));
	request->header.LEN = 0;

	
	if( 0 == msg_communicate(sk, buf, sizeof(T_MMETS_REQ_MSG)) )
	{
		temp = *p;
		tx_gain = ((temp>>7)==0)?temp:(temp|0xffffff00|temp);
		if( tx_gain < 0 )
		{
			printf("	TX GAIN : [%ddB]\n", tx_gain);
		}
		else
		{
			printf("	TX GAIN : [+%ddB]\n", tx_gain);
		}
		
		return 0;
	}
	
	return -1;
}

int TEST_MMEAD_INCREASE_TX_GAIN_VALUE(T_UDP_SK_INFO *sk)
{
	uint8_t buf[MAX_UDP_SIZE];
	int len = 0;
	T_MMETS_REQ_MSG *request = (T_MMETS_REQ_MSG *)buf;
	uint8_t *p = (uint8_t *)(request->body);
	
	T_MMETS_ACK_MSG *reply = (T_MMETS_ACK_MSG *)buf;	
	
	request->header.M_TYPE = 0xCC08;
	request->header.DEV_TYPE = WEC701_M0;
	request->header.MM_TYPE = MMEAD_SET_TX_GAIN;
	request->header.fragment = 0;
	memcpy(request->header.ODA, oda, sizeof(oda));
	request->header.LEN = sizeof(uint8_t);

	*p = 0x01;

	len = sizeof(request->header) + request->header.LEN;
	
	return msg_communicate(sk, buf, len);
}

int TEST_MMEAD_DECREASE_TX_GAIN_VALUE(T_UDP_SK_INFO *sk)
{
	uint8_t buf[MAX_UDP_SIZE];
	int len = 0;
	T_MMETS_REQ_MSG *request = (T_MMETS_REQ_MSG *)buf;
	uint8_t *p = (uint8_t *)(request->body);
	
	T_MMETS_ACK_MSG *reply = (T_MMETS_ACK_MSG *)buf;	
	
	request->header.M_TYPE = 0xCC08;
	request->header.DEV_TYPE = WEC701_M0;
	request->header.MM_TYPE = MMEAD_SET_TX_GAIN;
	request->header.fragment = 0;
	memcpy(request->header.ODA, oda, sizeof(oda));
	request->header.LEN = sizeof(uint8_t);

	*p = 0xff;

	len = sizeof(request->header) + request->header.LEN;
	
	return msg_communicate(sk, buf, len);
}

int TEST_MMEAD_GET_TOPOLOGY(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = NULL;
	uint8_t buf[MAX_UDP_SIZE];
	struct timeval start, end;
	
	/* 先获取线卡的MAC 地址*/
	TEST_MMEAD_GET_CLT_MAC(sk);

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_GET_TOPOLOGY;
	h.fragment = 0;
	memcpy(h.ODA, cltMac, 6);
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));

	start.tv_sec = 0;
	start.tv_usec = 0;
	end.tv_sec = 0;
	end.tv_usec = 0;

	gettimeofday( &start, NULL );
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		gettimeofday( &end, NULL );
		r = (T_REQ_Msg_MMEAD *)buf;
		T_MMEAD_TOPOLOGY *l = (T_MMEAD_TOPOLOGY *)(r->BUF);
		int i = 0;
		//printf("TEST_MMEAD_GET_TOPOLOGY\n");
		printf("TEST_MMEAD_GET_TOPOLOGY: [Time Used: %d Seconds %ul Microseconds]\n", 
			(int)(end.tv_sec - start.tv_sec),
			(uint32_t)(end.tv_usec - start.tv_usec)
		);
		printf("==========================================================================\n");
		printf( "clt.Mac = [%02X:%02X:%02X:%02X:%02X:%02X], clt.NumStas = [%d], clt.DevType = [%d]\n", 
			l->clt.Mac[0], l->clt.Mac[1], l->clt.Mac[2], l->clt.Mac[3], l->clt.Mac[4], l->clt.Mac[5], 
			l->clt.NumStas, l->clt.DevType );
		
		if( l->clt.NumStas > 0 )
		{
			for( i=0; i<l->clt.NumStas; i++ )
			{
				printf( "	-- cnu[%d].Mac = [%02X:%02X:%02X:%02X:%02X:%02X], TX/RX = [%d/%d], DevType = [%d]\n", 
					i, 
					l->cnu[i].Mac[0], l->cnu[i].Mac[1], l->cnu[i].Mac[2], 
					l->cnu[i].Mac[3], l->cnu[i].Mac[4], l->cnu[i].Mac[5], 
					l->cnu[i].AvgPhyTx, l->cnu[i].AvgPhyRx, l->cnu[i].DevType );
			}
		}
		printf("==========================================================================\n");
		return 0;
	}
	else
	{
		gettimeofday( &end, NULL );
		printf("TEST_MMEAD_GET_TOPOLOGY: [Time Used: %d Seconds %ul Microseconds]\n", 
			(int)(end.tv_sec - start.tv_sec),
			(uint32_t)(end.tv_usec - start.tv_usec)
		);
		printf("TEST_MMEAD_GET_TOPOLOGY: Failed\n");
		return -1;
	}
}

int TEST_MMEAD_GET_TOPOLOGY_STATS(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = NULL;
	uint8_t buf[MAX_UDP_SIZE];
	struct timeval start, end;
	
	/* 先获取线卡的MAC 地址*/
	TEST_MMEAD_GET_CLT_MAC(sk);

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_GET_TOPOLOGY_STATS;
	h.fragment = 0;
	memcpy(h.ODA, cltMac, 6);
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));

	start.tv_sec = 0;
	start.tv_usec = 0;
	end.tv_sec = 0;
	end.tv_usec = 0;

	gettimeofday( &start, NULL );
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		gettimeofday( &end, NULL );
		r = (T_REQ_Msg_MMEAD *)buf;
		T_MMEAD_TOPOLOGY *l = (T_MMEAD_TOPOLOGY *)(r->BUF);
		int i = 0;
		//printf("TEST_MMEAD_GET_TOPOLOGY_STATS\n");
		printf("TEST_MMEAD_GET_TOPOLOGY_STATS: [Time Used: %d Seconds %ul Microseconds]\n", 
			(int)(end.tv_sec - start.tv_sec),
			(uint32_t)(end.tv_usec - start.tv_usec)
		);
		printf("==========================================================================\n");
		printf( "clt.Mac = [%02X:%02X:%02X:%02X:%02X:%02X], clt.NumStas = [%d], clt.DevType = [%d]\n", 
			l->clt.Mac[0], l->clt.Mac[1], l->clt.Mac[2], l->clt.Mac[3], l->clt.Mac[4], l->clt.Mac[5], 
			l->clt.NumStas, l->clt.DevType );
		
		if( l->clt.NumStas > 0 )
		{
			for( i=0; i<l->clt.NumStas; i++ )
			{
				printf( "	-- cnu[%d].Mac = [%02X:%02X:%02X:%02X:%02X:%02X], TX/RX = [%d/%d], DevType = [%d]\n", 
					i, 
					l->cnu[i].Mac[0], l->cnu[i].Mac[1], l->cnu[i].Mac[2], 
					l->cnu[i].Mac[3], l->cnu[i].Mac[4], l->cnu[i].Mac[5], 
					l->cnu[i].AvgPhyTx, l->cnu[i].AvgPhyRx, l->cnu[i].DevType );
			}
		}
		printf("==========================================================================\n");
		return 0;
	}
	else
	{
		gettimeofday( &end, NULL );
		printf("TEST_MMEAD_GET_TOPOLOGY_STATS: [Time Used: %d Seconds %ul Microseconds]\n", 
			(int)(end.tv_sec - start.tv_sec),
			(uint32_t)(end.tv_usec - start.tv_usec)
		);
		printf("TEST_MMEAD_GET_TOPOLOGY_STATS: Failed\n");
		return -1;
	}
}



int TEST_MMEAD_GET_TOPOLOGY_LOOP(T_UDP_SK_INFO *sk)
{
	//int i = 0;
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = NULL;
	uint8_t buf[MAX_UDP_SIZE];
	T_MMEAD_TOPOLOGY *l = NULL;
	struct timeval start, end;
	
	/* 先获取线卡的MAC 地址*/
	TEST_MMEAD_GET_CLT_MAC(sk);

	while(1)
	{
		h.M_TYPE = 0xCC08;
		h.DEV_TYPE = WEC_3801I;
		h.MM_TYPE = MMEAD_GET_TOPOLOGY;
		h.fragment = 0;
		memcpy(h.ODA, cltMac, 6);
		h.LEN = 0;
		memcpy(buf, &h, sizeof(h));
		
		printf("=====================================================\n");
		
		start.tv_sec = 0;
		start.tv_usec = 0;
		end.tv_sec = 0;
		end.tv_usec = 0;
		
		gettimeofday( &start, NULL );
		if( 0 == msg_communicate(sk, buf, sizeof(h)) )
		{
			gettimeofday( &end, NULL );
			r = (T_REQ_Msg_MMEAD *)buf;
			l = (T_MMEAD_TOPOLOGY *)(r->BUF);			
			printf("TEST_MMEAD_GET_TOPOLOGY_LOOP: clt.NumStas = [%d]\n", l->clt.NumStas);
		}
		else
		{
			gettimeofday( &end, NULL );
			printf("TEST_MMEAD_GET_TOPOLOGY_LOOP: Failed\n");
		}
		printf("TEST_MMEAD_GET_TOPOLOGY_LOOP: [Time Used: %d Seconds %ul Microseconds]\n", 
			(int)(end.tv_sec - start.tv_sec),
			(uint32_t)(end.tv_usec - start.tv_usec)
		);
		printf("======================================================\n");
	}
	return 0;
}

int TEST_MMEAD_GET_SOFTWARE_VERSION(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = NULL;
	uint8_t buf[MAX_UDP_SIZE];

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_GET_SOFTWARE_VERSION;
	h.fragment = 0;
	memcpy(h.ODA, oda, sizeof(oda));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		r = (T_REQ_Msg_MMEAD *)buf;
		printf("TEST_MMEAD_GET_SOFTWARE_VERSION : Version String = [%s]\n", r->BUF);
		return 0;
	}
	return -1;
}

int TEST_MMEAD_GET_MANUFACTURER_INFO(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = NULL;
	uint8_t buf[MAX_UDP_SIZE];

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_GET_MANUFACTURER_INFO;
	h.fragment = 0;
	memcpy(h.ODA, oda, sizeof(oda));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		r = (T_REQ_Msg_MMEAD *)buf;
		printf("TEST_MMEAD_GET_MANUFACTURER_INFO : Mnufacturer Info = [%s]\n", r->BUF);
		return 0;
	}
	return -1;
}

int TEST_MMEAD_RESET_DEV(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	uint8_t buf[MAX_UDP_SIZE];

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = MMEAD_RESET_DEV;
	h.fragment = 0;
	memcpy(h.ODA, oda, sizeof(oda));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		printf("TEST_MMEAD_RESET_DEV : OK\n");
		return 0;
	}
	return -1;
}

int TEST_MMEAD_WRITE_PIB(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	uint8_t buf[MAX_UDP_SIZE];

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = MMEAD_WRITE_PIB;
	h.fragment = 0;
	memcpy(h.ODA, oda, sizeof(oda));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		printf("TEST_MMEAD_WRITE_PIB : OK\n");
		return 0;
	}
	return -1;
}

int TEST_MMEAD_BOOTOUT_DEV(T_UDP_SK_INFO *sk)
{	
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_MMEAD h;
	T_Msg_MMEAD *msg = (T_Msg_MMEAD *)buf;

	/* 注意填入CNU的MAC地址*/
	uint8_t oda[6] = {0x00, 0x1E, 0xE3, 0x00, 0x88, 0x38};
	
	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_BOOT_OUT_CNU;
	h.fragment = 0;
	memcpy(h.ODA, oda, sizeof(oda));
	h.LEN = 6;
	memcpy(buf, &h, sizeof(h));
	memcpy(msg->BUF, oda, 6);
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)+6) )
	{
		printf("TEST_MMEAD_RESET_DEV : OK\n");
		return 0;
	}
	return -1;
}

int TEST_MMEAD_LINK_DIAG_RX(T_UDP_SK_INFO *sk)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	int len = 0;

	T_Msg_MMEAD *req = (T_Msg_MMEAD *)buf;
	T_MMEAD_LINK_DIAG_INFO *req_data = (T_MMEAD_LINK_DIAG_INFO *)(req->BUF);
	
	T_REQ_Msg_MMEAD *ack = (T_REQ_Msg_MMEAD *)buf;	
	T_MMEAD_LINK_DIAG_RESULT *ack_data = (T_MMEAD_LINK_DIAG_RESULT *)(ack->BUF);	

	req->HEADER.M_TYPE = 0xCC08;
	req->HEADER.DEV_TYPE = WEC_3702I;
	req->HEADER.MM_TYPE = MMEAD_LINK_DIAG;
	req->HEADER.fragment = 0;	
	memcpy(req->HEADER.ODA, oda, sizeof(oda));
	req->HEADER.LEN = sizeof(T_MMEAD_LINK_DIAG_INFO);

	req_data->dir = 1;
	memcpy(req_data->peerNodeMac, cltMac, 6);
	memcpy(req_data->ccoMac, cltMac, 6);

	len = sizeof(req->HEADER) + req->HEADER.LEN;
	
	if( 0 == msg_communicate(sk, buf, len) )
	{
		printf("dir: %d\n", ack_data->dir);
		
		printf("mac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->mac[0], ack_data->mac[1], ack_data->mac[2], 
			ack_data->mac[3], ack_data->mac[4], ack_data->mac[5]
		);
		
		printf("tei: %d\n", ack_data->tei);
		
		printf("peerNodeMac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->peerNodeMac[0], ack_data->peerNodeMac[1], ack_data->peerNodeMac[2], 
			ack_data->peerNodeMac[3], ack_data->peerNodeMac[4], ack_data->peerNodeMac[5]
		);

		printf("ccoMac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->ccoMac[0], ack_data->ccoMac[1], ack_data->ccoMac[2], 
			ack_data->ccoMac[3], ack_data->ccoMac[4], ack_data->ccoMac[5]
		);
		
		printf("ccoNid: %02x%02x%02x%02x%02x%02x%02x\n", ack_data->ccoNid[0], 
			ack_data->ccoNid[1], ack_data->ccoNid[2], ack_data->ccoNid[3], 
			ack_data->ccoNid[4], ack_data->ccoNid[5], ack_data->ccoNid[6]
		);
		printf("ccoSnid: %d\n", ack_data->ccoSnid);
		printf("ccoTei: %d\n", ack_data->ccoTei);
		
		printf("tx: %d\n", ack_data->tx);
		printf("rx: %d\n", ack_data->rx);
		
		printf("bridgedMac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->bridgedMac[0], ack_data->bridgedMac[1], ack_data->bridgedMac[2], 
			ack_data->bridgedMac[3], ack_data->bridgedMac[4], ack_data->bridgedMac[5]
		);
		printf("bitRate: %.2f\n", ack_data->bitRate);
		printf("attenuation: %d\n", ack_data->attenuation);
		printf("MPDU_ACKD: %lld\n", ack_data->MPDU_ACKD);
		printf("MPDU_COLL: %lld\n", ack_data->MPDU_COLL);
		printf("MPDU_FAIL: %lld\n", ack_data->MPDU_FAIL);
		printf("PBS_PASS: %lld\n", ack_data->PBS_PASS);
		printf("PBS_FAIL: %lld\n", ack_data->PBS_FAIL);
		return 0;
	}

	return -1;
}

int TEST_MMEAD_LINK_DIAG_TX(T_UDP_SK_INFO *sk)
{
	uint8_t buf[MAX_UDP_SIZE] = {0};
	int len = 0;

	T_Msg_MMEAD *req = (T_Msg_MMEAD *)buf;
	T_MMEAD_LINK_DIAG_INFO *req_data = (T_MMEAD_LINK_DIAG_INFO *)(req->BUF);
	
	T_REQ_Msg_MMEAD *ack = (T_REQ_Msg_MMEAD *)buf;	
	T_MMEAD_LINK_DIAG_RESULT *ack_data = (T_MMEAD_LINK_DIAG_RESULT *)(ack->BUF);	

	req->HEADER.M_TYPE = 0xCC08;
	req->HEADER.DEV_TYPE = WEC_3702I;
	req->HEADER.MM_TYPE = MMEAD_LINK_DIAG;
	req->HEADER.fragment = 0;	
	memcpy(req->HEADER.ODA, cltMac, sizeof(cltMac));
	req->HEADER.LEN = sizeof(T_MMEAD_LINK_DIAG_INFO);

	req_data->dir = 0;
	memcpy(req_data->peerNodeMac, oda, 6);
	memcpy(req_data->ccoMac, cltMac, 6);

	len = sizeof(req->HEADER) + req->HEADER.LEN;
	
	if( 0 == msg_communicate(sk, buf, len) )
	{
		printf("dir: %d\n", ack_data->dir);
		
		printf("mac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->mac[0], ack_data->mac[1], ack_data->mac[2], 
			ack_data->mac[3], ack_data->mac[4], ack_data->mac[5]
		);
		
		printf("tei: %d\n", ack_data->tei);
		
		printf("peerNodeMac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->peerNodeMac[0], ack_data->peerNodeMac[1], ack_data->peerNodeMac[2], 
			ack_data->peerNodeMac[3], ack_data->peerNodeMac[4], ack_data->peerNodeMac[5]
		);

		printf("ccoMac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->ccoMac[0], ack_data->ccoMac[1], ack_data->ccoMac[2], 
			ack_data->ccoMac[3], ack_data->ccoMac[4], ack_data->ccoMac[5]
		);
		
		printf("ccoNid: %02x%02x%02x%02x%02x%02x%02x\n", ack_data->ccoNid[0], 
			ack_data->ccoNid[1], ack_data->ccoNid[2], ack_data->ccoNid[3], 
			ack_data->ccoNid[4], ack_data->ccoNid[5], ack_data->ccoNid[6]
		);
		printf("ccoSnid: %d\n", ack_data->ccoSnid);
		printf("ccoTei: %d\n", ack_data->ccoTei);
		
		printf("tx: %d\n", ack_data->tx);
		printf("rx: %d\n", ack_data->rx);
		
		printf("bridgedMac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ack_data->bridgedMac[0], ack_data->bridgedMac[1], ack_data->bridgedMac[2], 
			ack_data->bridgedMac[3], ack_data->bridgedMac[4], ack_data->bridgedMac[5]
		);
		printf("bitRate: %.2f\n", ack_data->bitRate);
		printf("attenuation: %d\n", ack_data->attenuation);
		printf("MPDU_ACKD: %lld\n", ack_data->MPDU_ACKD);
		printf("MPDU_COLL: %lld\n", ack_data->MPDU_COLL);
		printf("MPDU_FAIL: %lld\n", ack_data->MPDU_FAIL);
		printf("PBS_PASS: %lld\n", ack_data->PBS_PASS);
		printf("PBS_FAIL: %lld\n", ack_data->PBS_FAIL);
		return 0;
	}

	return -1;
}

int TEST_MMEAD_GET_RTL8306E_CONFIGS(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = NULL;
	uint8_t buf[MAX_UDP_SIZE];
	st_rtl8306eSettings *rtl8306e = NULL;
	struct timeval start, end;
	int i = 0;
	
	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3801I;
	h.MM_TYPE = MMEAD_GET_RTL8306E_CONFIG;
	h.fragment = 0;
	memcpy(h.ODA, oda, sizeof(oda));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));

	gettimeofday( &start, NULL );
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		gettimeofday( &end, NULL );
		r = (T_REQ_Msg_MMEAD *)buf;
		rtl8306e = (st_rtl8306eSettings *)(r->BUF);
		printf("\nRead rtl8306e configs [Time Used: %d Seconds %ul Microseconds]:\n", 
			(int)(end.tv_sec - start.tv_sec),
			(uint32_t)(end.tv_usec - start.tv_usec)
		);
		printf("-vlan config\n");
		printf("-vlan_enable: %d\n", rtl8306e->vlanConfig.vlan_enable);
		printf("-vlan_tag_aware: %d\n", rtl8306e->vlanConfig.vlan_tag_aware);
		printf("-ingress_filter: %d\n", rtl8306e->vlanConfig.ingress_filter);
		printf("-g_admit_control: %d\n", rtl8306e->vlanConfig.g_admit_control);
		for(i=0;i<=4;i++)
		{
			printf("-p%d pvid: %d\n", i, rtl8306e->vlanConfig.vlan_port[i].pvid);
			printf("-p%d egress_mode: %d\n", i, rtl8306e->vlanConfig.vlan_port[i].egress_mode);
			printf("-p%d admit_control: %d\n", i, rtl8306e->vlanConfig.vlan_port[i].admit_control);
		}
		
		printf("-bandwidth control config\n");
		printf("-g_rx_bandwidth_control_enable: %d\n", rtl8306e->bandwidthConfig.g_rx_bandwidth_control_enable);
		printf("-g_tx_bandwidth_control_enable: %d\n", rtl8306e->bandwidthConfig.g_tx_bandwidth_control_enable);
		for(i=0;i<=4;i++)
		{
			printf("-p%d rx_bandwidth_control_enable: %d\n", i, rtl8306e->bandwidthConfig.rxPort[i].bandwidth_control_enable);
			printf("-p%d rx_bandwidth_value: 0x%03x\n", i, rtl8306e->bandwidthConfig.rxPort[i].bandwidth_value);
			printf("-p%d tx_bandwidth_control_enable: %d\n", i, rtl8306e->bandwidthConfig.txPort[i].bandwidth_control_enable);
			printf("-p%d tx_bandwidth_value: 0x%03x\n", i, rtl8306e->bandwidthConfig.txPort[i].bandwidth_value);
		}
		printf("-storm filter settings:\n");
		printf("-rule: %d\n", rtl8306e->stormFilter.rule);
		printf("-disable_broadcast: %d\n", rtl8306e->stormFilter.disable_broadcast);
		printf("-disable_multicast: %d\n", rtl8306e->stormFilter.disable_multicast);
		printf("-disable_unknown: %d\n", rtl8306e->stormFilter.disable_unknown);
		printf("-iteration: %d\n", rtl8306e->stormFilter.iteration);
		printf("-thresholt_counter: %d\n", rtl8306e->stormFilter.thresholt);
		printf("-reset_source: %d\n", rtl8306e->stormFilter.reset_source);
		
		printf("-port loop detect config and status:\n");
		printf("-sid: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			rtl8306e->loopDetect.sid[0], rtl8306e->loopDetect.sid[1], 
			rtl8306e->loopDetect.sid[2], rtl8306e->loopDetect.sid[3],
			rtl8306e->loopDetect.sid[4], rtl8306e->loopDetect.sid[5]
		);
		printf("-loop detect enable: %d\n", rtl8306e->loopDetect.status);
		printf("-ldmethod: %d\n", rtl8306e->loopDetect.ldmethod);
		printf("-ldtime: %d\n", rtl8306e->loopDetect.ldtime);
		printf("-ldbckfrq: %d\n", rtl8306e->loopDetect.ldbckfrq);
		printf("-ldsclr: %d\n", rtl8306e->loopDetect.ldsclr);
		printf("-pabuzzer: %d\n", rtl8306e->loopDetect.pabuzzer);
		printf("-entaglf: %d\n", rtl8306e->loopDetect.entaglf);
		printf("-lpttlinit: %d\n", rtl8306e->loopDetect.lpttlinit);
		printf("-lpfpri: %d\n", rtl8306e->loopDetect.lpfpri);
		printf("-enlpfpri: %d\n", rtl8306e->loopDetect.enlpfpri);
		printf("-disfltlf: %d\n", rtl8306e->loopDetect.disfltlf);
		printf("-enlpttl: %d\n", rtl8306e->loopDetect.enlpttl);
		for(i=0;i<=4;i++)
		{
			printf("-p%d loop status: 0x%03x\n", i, rtl8306e->loopDetect.port_loop_status[i]);
		}
		printf("-mac limit config:\n");
		printf("-action: %d\n", rtl8306e->macLimit.action);
		printf("-system enable: %d\n", rtl8306e->macLimit.system.enable);
		printf("-system mport: %d\n", rtl8306e->macLimit.system.mport);
		printf("-system thresholt: %d\n", rtl8306e->macLimit.system.thresholt);
		printf("-system counter: %d\n", rtl8306e->macLimit.system.counter);
		for(i=0;i<4;i++)
		{
			printf("-p%d mac limit enable: %d\n", i, rtl8306e->macLimit.port[i].enable);
			printf("-p%d mac limit thresholt: %d\n", i, rtl8306e->macLimit.port[i].thresholt);
			printf("-p%d mac limit counter: %d\n", i, rtl8306e->macLimit.port[i].counter);
		}

		printf("-port control config:\n");
		for(i=0;i<5;i++)
		{
			printf("-port[%d] enable: %d\n", i, rtl8306e->portControl.port[i].enable);
		}
		
		printf("\n\n");
		return 0;
	}
	return -1;
}

int TEST_MMEAD_GET_CNU_HFID(T_UDP_SK_INFO *sk)
{
	T_Msg_Header_MMEAD h;	
	T_REQ_Msg_MMEAD *r = NULL;	
	uint8_t buf[MAX_UDP_SIZE];	
	h.M_TYPE = 0xCC08;	
	h.DEV_TYPE = WEC_3801I;	
	h.MM_TYPE = MMEAD_GET_USER_HFID;	
	h.fragment = 0;	

	memcpy(h.ODA, oda, sizeof(oda));	
	h.LEN = 0;	
	memcpy(buf, &h, sizeof(h));		
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )	
	{		
		r = (T_REQ_Msg_MMEAD *)buf;		
		printf("    User HFID = [%s]\n", r->BUF);		
		return 0;	
	}	
	return -1;
}
/*
int TEST_MMEAD_SET_CNU_HFID(T_UDP_SK_INFO *sk, uint8_t user_hfid[])
{
	uint8_t buf[MAX_UDP_SIZE];
	int len = 0;
	T_MMETS_REQ_MSG *request = (T_MMETS_REQ_MSG *)buf;
	uint8_t *p = (uint8_t *)(request->body);
	
	T_MMETS_ACK_MSG *reply = (T_MMETS_ACK_MSG *)buf;	
	
	request->header.M_TYPE = 0xCC08;
	request->header.DEV_TYPE = WEC701_M0;
	request->header.MM_TYPE = MMEAD_SET_USER_HFID;
	request->header.fragment = 0;
	memcpy(request->header.ODA, oda, sizeof(oda));
	request->header.LEN = 64;

	memcpy(p,user_hfid,64);

	len = sizeof(request->header) + request->header.LEN;
	
	return msg_communicate(sk, buf, len);
}
*/
int MMEAD_MSG_DEBUG_ENABLE(T_UDP_SK_INFO *sk, uint32_t enable)
{
	T_Msg_Header_MMEAD h;
	T_REQ_Msg_MMEAD *r = NULL;
	uint8_t buf[MAX_UDP_SIZE];

	h.M_TYPE = 0xCC08;
	h.DEV_TYPE = WEC_3702I;
	h.MM_TYPE = (enable?MMEAD_MODULE_MSG_DEBUG_ENABLE:MMEAD_MODULE_MSG_DEBUG_DISABLE);
	h.fragment = 0;
	memcpy(h.ODA, oda, sizeof(oda));
	h.LEN = 0;

	memcpy(buf, &h, sizeof(h));
	
	if( 0 == msg_communicate(sk, buf, sizeof(h)) )
	{
		r = (T_REQ_Msg_MMEAD *)buf;
		printf("MMEAD_MSG_DEBUG_ENABLE : OK\n");
		return 0;
	}
	return -1;
}

int main(int argc, char *argv[])
{
	int cmd = 0;
	uint8_t bMac[6] = {0};
	T_UDP_SK_INFO sk;
	uint8_t user_hfid[64] = {0};
	
	if( (argc != 3) && (argc != 4) && (argc != 5))
	{
		print_usage();
		return 0;
	}

	if( 0 != init_socket(&sk) )
	{
		return 0;
	}
	
	if( strcmp(argv[1], "debug") == 0)
	{
		if( strcmp(argv[2], "on") == 0 )
		{
			MMEAD_MSG_DEBUG_ENABLE(&sk, 1);
		}
		else if( strcmp(argv[2], "off") == 0 )
		{
			MMEAD_MSG_DEBUG_ENABLE(&sk, 0);
		}
		else
		{
			print_usage();
		}
	}	
	else if( strcmp(argv[1], "test") == 0)
	{
		//get test cmd
		cmd = atoi(argv[2]);
		//get des mac address
		if( argc == 4)
		{
			if( CMM_SUCCESS == boardapi_macs2b(argv[3], bMac) )
			{
				memcpy(oda, bMac, sizeof(oda));
			}
			else
			{
				printf("\n  MAC address invalid\n");	
			}
		}
		else if ( argc == 5)
		{
			if( CMM_SUCCESS == boardapi_macs2b(argv[3], bMac) )
			{
				memcpy(oda, bMac, sizeof(oda));
				memcpy(user_hfid,argv[4],strlen(argv[4]));
			}
			else
			{
				printf("\n  MAC address invalid\n");	
			}
		}
			
		switch(cmd)
		{
			case 1:
			{
				TEST_MODULE_OPERATION_READ(&sk);
				break;
			}
			case 2:
			{
				TEST_MODULE_OPERATION_WRITE(&sk);
				break;
			}
			case 3:
			{
				TEST_MMEAD_GET_CLT_MAC(&sk);
				break;
			}
			case 4:
			{
				TEST_MMEAD_GET_TOPOLOGY(&sk);
				break;
			}
			case 5:
			{
				TEST_MMEAD_GET_SOFTWARE_VERSION(&sk);
				break;
			}
			case 6:
			{
				TEST_MMEAD_GET_MANUFACTURER_INFO(&sk);
				break;
			}
			case 7:
			{
				TEST_MMEAD_GET_MOD_CRC(&sk);
				break;
			}
			case 8:
			{
				TEST_MMEAD_RESET_DEV(&sk);
				break;
			}
			case 9:
			{
				TEST_MMEAD_GET_PIB_CRC(&sk);
				break;
			}
			case 10:
			{
				TEST_MMEAD_WRITE_PIB(&sk);
				break;
			}
			case 11:
			{
				TEST_MMEAD_BOOTOUT_DEV(&sk);
				break;
			}
			case 12:
			{
				TEST_MMEAD_GET_TOPOLOGY_LOOP(&sk);
				break;
			}
			case 13:
			{
				TEST_MMEAD_LINK_DIAG_RX(&sk);
				break;
			}
			case 14:
			{
				TEST_MMEAD_LINK_DIAG_TX(&sk);
				break;
			}
			case 15:
			{
				TEST_MMEAD_READ_PIB(&sk);
				break;
			}
			case 16:
			{
				TEST_MMEAD_GET_TOPOLOGY_STATS(&sk);
				break;
			}
			case 17:
			{
				TEST_MMEAD_GET_FBS(&sk);
				break;
			}
			case 18:
			{
				TEST_MMEAD_SET_FBS_SUBBAND(&sk);
				break;
			}
			case 19:
			{
				TEST_MMEAD_SET_FBS_FULLBAND(&sk);
				break;
			}
			case 20:
			{
				TEST_MMEAD_SET_FBS_DISABLE(&sk);
				break;
			}
			case 21:
			{
				TEST_MMEAD_GET_TX_GAIN_VALUE(&sk);
				break;
			}
			case 22:
			{
				TEST_MMEAD_INCREASE_TX_GAIN_VALUE(&sk);
				break;
			}
			case 23:
			{
				TEST_MMEAD_DECREASE_TX_GAIN_VALUE(&sk);
				break;
			}
			case 24:
			{
				TEST_MMEAD_GET_RTL8306E_CONFIGS(&sk);
				break;
			}
			case 25:
			{
				TEST_MMEAD_GET_CNU_HFID(&sk);
				break;
			}
			default:
			{
				print_usage();
				break;
			}
		}		
	}
	else
	{
		print_usage();
	}
	close_socket(&sk);
	return 0;	
}


