/**************************************************************************************
  文件名称 : mmeapi.c
  文件描述 : 与Athreos的设备进行MME交互的API
  				
  修订记录 :
           1 创建 : frank
             日期 : 2010-07-27
             描述 : 创建文件

 **************************************************************************************/

#include <assert.h>
#include <errno.h>
#include <sys/poll.h>
#include "mmead.h"
#include "mmeapi.h"
#include "support/atheros/ihpapi/ihpapi.h"
#include "support/atheros/ihpapi/ihp.h"

#ifdef __GNUC__
#ifndef __packed
#define __packed __attribute__ ((packed))
#endif
#endif

extern uint8_t OSA[6];

uint8_t QCA_MADDR[6] = 
{
	0x00,
	0xb0,
	0x52,
	0x00,
	0x00,
	0x01 
};

int MME_Atheros_MsgGetDeviceInfo(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_DEV_INFO *pDevInfo);

/********************************************************************************************
*	函数名称:readpacket
*	函数功能:MME 二层OAM RAW SOCKET报文接收函数
*	返回值:成功返回接收的字节数，失败返回其他数值
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
static int readpacket
(T_MME_SK_HANDLE *MME_SK, uint16_t MMtype, uint8_t *buffer, int buffersize, int *msg_len)
{
	int status;
	int wait_time = 1500;
	socklen_t socklen = sizeof(struct sockaddr_ll);

	struct pollfd pollfd = 
	{
		MME_SK->skfd,
		POLLIN,
		0
	};
	
	*msg_len = 0;
	
	
	//while(retry--)
	//{
		/* 获取拓扑时该消息并发较多，防止处理时间过长导致
		    register 获取不到响应*/
		if( VS_RD_MOD == MMtype)
		{
			wait_time = 500;
		}
		status = poll(&pollfd, 1, wait_time);
	//	if(status>0) break;

	//	printf("\r\n====>Mmead retry!\n");
	//}
	
	if (status <= 0) 
	{
		return -1;
	}	
	else
	{
		memset((uint8_t *)buffer, 0, buffersize);
		*msg_len = recvfrom(MME_SK->skfd, buffer, buffersize, 0, 
			(struct sockaddr *)&(MME_SK->sockaddr), &socklen);		
		if (*msg_len == -1) 
		{
			return -1;
		}
		/* 打印接收的数据报文*/
		mmead_debug_printf("\n====> MMEAD RECIEVED MME:\n");
		debug_dump_msg(buffer, *msg_len, stderr);
		return (*msg_len);
	}
	return (0);
}

/********************************************************************************************
*	函数名称:mme_tx
*	函数功能:MME 二层OAM RAW SOCKET报文发送函数
*	返回值:返回发送的字节数
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
static int mme_tx(T_MME_SK_HANDLE *MME_SK, uint8_t *buffer, int buffersize)
{
	int sendn = 0;
	struct header_eth *header = NULL;
	
	if( buffersize > 0 )
	{
		assert( NULL != buffer );
		header = (struct header_eth *)(buffer);
		
		memcpy(MME_SK->peer, header->ODA, 6);
		
		/* 打印发送的数据报文*/
		mmead_debug_printf("\n<==== MMEAD SEND MME:\n");
		debug_dump_msg(buffer, buffersize, stderr);
		
		sendn = sendto(MME_SK->skfd, buffer, buffersize, 0, 
			(struct sockaddr *)&(MME_SK->sockaddr), sizeof(struct sockaddr_ll));
		/* 等待设备处理MME完成 */
		//usleep(5000);
	}	
	return sendn;
}

/********************************************************************************************
*	函数名称:mme_rx
*	函数功能:MME 二层OAM RAW SOCKET报文接收函数，并负责完成
*				   
*	返回值:成功返回0，失败返回其他数值
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
static int mme_rx
( T_MME_SK_HANDLE *MME_SK, uint16_t MMtype, uint8_t *buffer, int buffersize, int *msg_len, ihpapi_result_t *xresult )	
{
	int ret=0;
	long packetsize;
	header_cnf *p = NULL;	

	while ( (packetsize = readpacket(MME_SK, MMtype, buffer, buffersize, msg_len)) > 0)	
	{
		p = (header_cnf *)buffer;		

		if( memcmp(MME_SK->peer, QCA_MADDR, 6) != 0 )
		{		
			if( memcmp(p->ethernet.OSA, MME_SK->peer, 6) != 0 )
			{
				mmead_debug_printf("\nPeer not match, continue !\n");
				mmead_debug_printf("PEER[%02x:%02x:%02x:%02x:%02x:%02x], OSA[%02x:%02x:%02x:%02x:%02x:%02x]\n", 
					MME_SK->peer[0], MME_SK->peer[1], MME_SK->peer[2], 
					MME_SK->peer[3], MME_SK->peer[4], MME_SK->peer[5], 
					p->ethernet.OSA[0], p->ethernet.OSA[1], p->ethernet.OSA[2], 
					p->ethernet.OSA[3], p->ethernet.OSA[4], p->ethernet.OSA[5]
				);
				continue;
			}		
		}
		
		if( intohs(p->intellon.MMTYPE) != ( MMtype|MMTYPE_CNF ) )
		{
			mmead_debug_printf("MMtype not match, continue !\n");
			continue;
		}
		if (ihpapi_RxFrame(packetsize, buffer, xresult) == -1)
		{			
			switch(errno)			
			{
				case EFAULT:				
					mmead_debug_printf( "invalid packet or result\n");		
					break;			
				case EBADMSG:				
					mmead_debug_printf("wrong message error or EBADMSG\n");
					break;			
				case ENOSYS:
					mmead_debug_printf("wrong message error or ENOSYS\n");						
					break;			
				case EPROTONOSUPPORT:		
					mmead_debug_printf("wrong message error or EPROTONOSUPPORT\n");							
					break;
				case EAGAIN:
					/* Atheros 6400增加了设备上线主动通知的MME */
					/* 我们需要将该消息过滤掉*/
					break;
				default:				
					mmead_debug_printf("unknown RxFrame error\n"); 		
					break;			
			}			
			continue;		
		}	
		else
		{
			break;	
		}
	}

	if ( (*msg_len) > 0 )
	{
		ret = CMM_SUCCESS;
	}
	else
	{
		ret = -1;
	}

	return (ret);

}

/********************************************************************************************
*	函数名称:mme_v1_rx
*	函数功能:MME 二层OAM RAW SOCKET报文接收函数，并负责完成
*				   
*	返回值:成功返回0，失败返回其他数值
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
static int mme_v1_rx
( T_MME_SK_HANDLE *MME_SK, uint16_t MMtype, uint8_t *buffer, int buffersize, int *msg_len, ihpapi_result_t *xresult )	
{
	int ret=0;
	long packetsize;
	header_V1_cnf *p = NULL;

	while ( (packetsize = readpacket(MME_SK, MMtype, buffer, buffersize, msg_len)) > 0)	
	{
		p = (header_V1_cnf *)buffer;
		if( intohs(p->intellon.MMTYPE) != ( MMtype|MMTYPE_CNF ) )
		{
			printf("  MMTYPE ERROR\n");
			mmead_debug_printf("MMtype not match, continue !\n");
			continue;
		}
		if (ihpapi_v1_RxFrame(packetsize, buffer, xresult) == -1)
		{	
			printf(" ihpapi_v1_RxFrame error\n");
			switch(errno)			
			{
				case EFAULT:				
					mmead_debug_printf( "invalid packet or result\n");		
					break;			
				case EBADMSG:				
					mmead_debug_printf("wrong message error or EBADMSG\n");
					break;			
				case ENOSYS:
					mmead_debug_printf("wrong message error or ENOSYS\n");						
					break;			
				case EPROTONOSUPPORT:		
					mmead_debug_printf("wrong message error or EPROTONOSUPPORT\n");							
					break;
				case EAGAIN:
					/* Atheros 6400增加了设备上线主动通知的MME */
					/* 我们需要将该消息过滤掉*/
					break;
				default:				
					mmead_debug_printf("unknown RxFrame error\n"); 		
					break;			
			}			
			continue;		
		}	
		else
		{
			break;	
		}
	}

	if ( (*msg_len) > 0 )
	{
		ret = CMM_SUCCESS;
	}
	else
	{
		ret = -1;
	}

	return (ret);

}

int smiRead(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szMdioPhy *v)
{
	int packetsize;
	int recv_msg_len = 0;
	MdioCmdInfo stMdioCmdInfo;
	uint8_t buffer[IHPAPI_ETHER_MIN_LEN];
	ihpapi_result_t xresult;

	assert( NULL != v );
	mmead_debug_printf("  -->smiRead\n");
	memset(buffer, 0, sizeof(buffer));

	stMdioCmdInfo.OPERATION = 0x00;
	stMdioCmdInfo.PHY_ADDR = v->phy;
	stMdioCmdInfo.REGISTER_ADDR = v->reg;
	stMdioCmdInfo.DATA = 0x0000;
	
	packetsize = ihpapi_MdioCommand ( OSA, ODA, &stMdioCmdInfo, IHPAPI_ETHER_MIN_LEN, buffer );
	mmead_debug_printf("\n  -->smiRead phy 0x%02X register 0x%02X", stMdioCmdInfo.PHY_ADDR, stMdioCmdInfo.REGISTER_ADDR);
#if 1
	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_MDIO_COMMAND, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	else if(xresult.validData)
	{
		//v->value = ((uint32_t)(((xresult.data.mdioInfo.value<<8)|(xresult.data.mdioInfo.value>>8)))&0x0000ffff);
		v->value = (uint32_t)(intohs(xresult.data.mdioInfo.value));
		return CMM_SUCCESS;
	}
	else
	{
		return CMM_MME_ERROR;
	}
#else
	/* 打印发送的数据报文*/
	printf("<==== MMEAD->smiRead SEND MME:\n");
	hexdump(buffer, packetsize, stderr);
	return CMM_SUCCESS; 
#endif
}

int smiWrite(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szMdioPhy *v)
{
	int packetsize;
	int recv_msg_len = 0;
	MdioCmdInfo stMdioCmdInfo;
	uint8_t buffer[IHPAPI_ETHER_MIN_LEN];
	ihpapi_result_t xresult;

	assert( NULL != v );
	mmead_debug_printf("  -->smiWrite\n");
	memset(buffer, 0, sizeof(buffer));

	stMdioCmdInfo.OPERATION = 0x01;
	stMdioCmdInfo.PHY_ADDR = v->phy;
	stMdioCmdInfo.REGISTER_ADDR = v->reg;
	//stMdioCmdInfo.DATA = ((uint16_t)((v->value<<8)|(v->value>>8))&0xffff);
	stMdioCmdInfo.DATA = ihtons((uint16_t)(v->value));
	
	packetsize = ihpapi_MdioCommand ( OSA, ODA, &stMdioCmdInfo, IHPAPI_ETHER_MIN_LEN, buffer );
	mmead_debug_printf("\n  -->smiWrite phy 0x%02X register 0x%02X value 0x%04X\n", stMdioCmdInfo.PHY_ADDR, stMdioCmdInfo.REGISTER_ADDR, stMdioCmdInfo.DATA);
#if 1
	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_MDIO_COMMAND, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	else if(xresult.validData)
	{
		return CMM_SUCCESS;
	}
	else
	{
		return CMM_MME_ERROR;
	}
#else
	/* 打印发送的数据报文*/
	printf("<==== MMEAD->smiWrite SEND MME:\n");
	hexdump(buffer, packetsize, stderr);
	return CMM_SUCCESS; 
#endif
}

int rtl8306e_read(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szMdioRtl8306e *v)
{
	int ret = CMM_FAILED;
	T_szMdioPhy mdioInfo;
	v->value = 0x0000;

	if ((v->phy >= 7) || (v->page >= 5))
	{
		printf("\n#ERROR[10]\n");
		return CMM_FAILED;
	}

	/* Select PHY Register Page through configuring PHY 0 Register 16 [bit1 bit15] */
	mdioInfo.phy = 0;
	mdioInfo.reg = 16;
	if( CMM_SUCCESS != smiRead(MME_SK, ODA, &mdioInfo) )
	{
		printf("\n#ERROR[11]\n");
		return CMM_FAILED;
	}
	
	switch (v->page) 
	{
		case 0:
		{
			mdioInfo.phy = 0;
			mdioInfo.reg = 16;
			mdioInfo.value = (mdioInfo.value & 0x7FFF) | 0x0002;
			ret = smiWrite(MME_SK, ODA, &mdioInfo);
			if(ret)
			{
				printf("\n#ERROR[12]\n");
			}
			break;
		}
		case 1:
		{
			mdioInfo.phy = 0;
			mdioInfo.reg = 16;
			mdioInfo.value = mdioInfo.value | 0x8002;
			ret = smiWrite(MME_SK, ODA, &mdioInfo);
			if(ret)
			{
				printf("\n#ERROR[13]\n");
			}
			break;
		}
		case 2:
		{
			mdioInfo.phy = 0;
			mdioInfo.reg = 16;
			mdioInfo.value = mdioInfo.value & 0x7FFD;
			ret = smiWrite(MME_SK, ODA, &mdioInfo);
			if(ret)
			{
				printf("\n#ERROR[14]\n");
			}
			break;
		}
		case 3:
		{
			mdioInfo.phy = 0;
			mdioInfo.reg = 16;
			mdioInfo.value = (mdioInfo.value & 0xFFFD) | 0x8000;
			ret = smiWrite(MME_SK, ODA, &mdioInfo);
			if(ret)
			{
				printf("\n#ERROR[15]\n");
			}
			break;
		}
		case 4:
		{
			mdioInfo.phy = 5;
			mdioInfo.reg = 16;
			if( CMM_SUCCESS == smiRead(MME_SK, ODA, &mdioInfo) )
			{
				mdioInfo.value |= 0x2;
				ret = smiWrite(MME_SK, ODA, &mdioInfo);
				if(ret)
				{
					printf("\n#ERROR[16]\n");
				}
			}
			else
			{
				printf("\n#ERROR[17]\n");
				ret = CMM_FAILED;
			}
			break;
		}
		default:
		{
			printf("\n#ERROR[18]\n");
			return CMM_FAILED;
		}
	}

	if( CMM_SUCCESS != ret )
	{
		return CMM_FAILED;
	}
	
	//read register in selected page
	mdioInfo.phy = v->phy;
	mdioInfo.reg = v->reg;
	if( CMM_SUCCESS == smiRead(MME_SK, ODA, &mdioInfo) )
	{
		v->value = mdioInfo.value & 0xFFFF;
	}
	else
	{
		printf("\n#ERROR[19]\n");
	}
	if(4 == v->page)
	{
		/*exit page 4*/
		mdioInfo.phy = 5;
		mdioInfo.reg = 16;
		if( CMM_SUCCESS == smiRead(MME_SK, ODA, &mdioInfo) )
		{
			mdioInfo.value &= ~0x2;
			ret = smiWrite(MME_SK, ODA, &mdioInfo);
			if(ret)
			{
				printf("\n#ERROR[20]\n");
			}
		}
	} 
	if(ret)
	{
		printf("\n#ERROR[21]\n");
	}
	return ret;	
}

int rtl8306_getAsicVlan(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint32_t vlanIndex, uint32_t *vid)
{
	T_szMdioRtl8306e rtl8306e_mdio_info;
	
	if( vid == NULL )
	{
		return CMM_FAILED;
	}
		
	switch(vlanIndex)
	{
		case 0: /*VLAN[A]*/
		{
			rtl8306e_mdio_info.phy = 0;
			rtl8306e_mdio_info.reg = 25;
			rtl8306e_mdio_info.page = 0;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}			
		case 1: /*VLAN[B]*/
		{
			rtl8306e_mdio_info.phy = 1;
			rtl8306e_mdio_info.reg = 25;
			rtl8306e_mdio_info.page = 0;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 2: /*VLAN[C]*/
		{
			rtl8306e_mdio_info.phy = 2;
			rtl8306e_mdio_info.reg = 25;
			rtl8306e_mdio_info.page = 0;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 3: /*VLAN[D]*/
		{
			rtl8306e_mdio_info.phy = 3;
			rtl8306e_mdio_info.reg = 25;
			rtl8306e_mdio_info.page = 0;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 4: /*VLAN[E]*/
		{
			rtl8306e_mdio_info.phy = 4;
			rtl8306e_mdio_info.reg = 25;
			rtl8306e_mdio_info.page = 0;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 5: /*VLAN[F]*/
		{
			rtl8306e_mdio_info.phy = 0;
			rtl8306e_mdio_info.reg = 27;
			rtl8306e_mdio_info.page = 1;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 6: /*VLAN[G]*/
		{
			rtl8306e_mdio_info.phy = 1;
			rtl8306e_mdio_info.reg = 27;
			rtl8306e_mdio_info.page = 1;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 7: /*VLAN[H]*/
		{
			rtl8306e_mdio_info.phy = 2;
			rtl8306e_mdio_info.reg = 27;
			rtl8306e_mdio_info.page = 1;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 8: /*VLAN[I]*/
		{
			rtl8306e_mdio_info.phy = 3;
			rtl8306e_mdio_info.reg = 27;
			rtl8306e_mdio_info.page = 1;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 9: /*VLAN[J]*/
		{
			rtl8306e_mdio_info.phy = 4;
			rtl8306e_mdio_info.reg = 27;
			rtl8306e_mdio_info.page = 1;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 10: /*VLAN[K]*/
		{
			rtl8306e_mdio_info.phy = 0;
			rtl8306e_mdio_info.reg = 29;
			rtl8306e_mdio_info.page = 1;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 11: /*VLAN[L]*/
		{
			rtl8306e_mdio_info.phy = 1;
			rtl8306e_mdio_info.reg = 29;
			rtl8306e_mdio_info.page = 1;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 12: /*VLAN[M]*/
		{
			rtl8306e_mdio_info.phy = 2;
			rtl8306e_mdio_info.reg = 29;
			rtl8306e_mdio_info.page = 1;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
 		case 13: /*VLAN[N]*/
		{
			rtl8306e_mdio_info.phy = 3;
			rtl8306e_mdio_info.reg = 29;
			rtl8306e_mdio_info.page = 1;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
 		case 14: /*VLAN[O]*/
		{
			rtl8306e_mdio_info.phy = 4;
			rtl8306e_mdio_info.reg = 29;
			rtl8306e_mdio_info.page = 1;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 15: /*VLAN[P]*/
		{
			rtl8306e_mdio_info.phy = 0;
			rtl8306e_mdio_info.reg = 31;
			rtl8306e_mdio_info.page = 1;
			if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
			{
				return CMM_FAILED;
			}
			break;
		}
		default:
			return CMM_FAILED;
	}	
	*vid = (rtl8306e_mdio_info.value) & 0xFFF;
	return	CMM_SUCCESS;
		
}	

int rtl8306_getAsicPortVlanIndex(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint32_t port, uint32_t *vlanIndex)
{
	uint32_t tmp;
	T_szMdioRtl8306e rtl8306e_mdio_info;	
	
	if((port > 5) || vlanIndex == NULL)
	{
		return CMM_FAILED;
	}
		
	if (port < 5)
	{
		rtl8306e_mdio_info.phy = port;
		rtl8306e_mdio_info.reg = 24;
		rtl8306e_mdio_info.page = 0;
		if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
		{
			return CMM_FAILED;
		}
	}		
	else
	{
		rtl8306e_mdio_info.phy = 0;
		rtl8306e_mdio_info.reg = 26;
		rtl8306e_mdio_info.page = 1;
		if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
		{
			return CMM_FAILED;
		}
	}
	
	*vlanIndex = (rtl8306e_mdio_info.value>>12) & 0xF;
	
	return CMM_SUCCESS;
}

int rtl8306_getPvid(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint32_t port, uint32_t *vid)
{
	uint32_t index;

	/*check port number*/
	if (port > 5)
		return CMM_FAILED;

	/*check vid*/
	if (vid == NULL)
		return CMM_FAILED;
		
	/*get the pvid*/
	if( CMM_SUCCESS != rtl8306_getAsicPortVlanIndex(MME_SK, ODA, port, &index) )
	{
		return CMM_FAILED;
	}
	if( CMM_SUCCESS != rtl8306_getAsicVlan(MME_SK, ODA, index, vid) )
	{
		return CMM_FAILED;
	}

	return CMM_SUCCESS;
}

int rtl8306_getPortVlanMode(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint32_t port, uint32_t *mode)
{
	T_szMdioRtl8306e rtl8306e_mdio_info;	
	uint32_t regValue;
	
	if (port > 5 || (mode == NULL))
	{
		return CMM_FAILED;
	}
		
	/*Port 5 corresponding PHY6*/	
	if (port == 5 )
	{
		port ++ ;
	}

	rtl8306e_mdio_info.phy = port;
	rtl8306e_mdio_info.reg = 22;
	rtl8306e_mdio_info.page = 0;

	if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
	{
		return CMM_FAILED;
	}
	*mode = rtl8306e_mdio_info.value & 0x3;
	
       /*
	//When enable inserting CPU tag, VLAN tag could not be added 
	rtl8306_getAsicPhyReg(2, 21, 3, &regValue);
	if ((*option == RTL8306_VLAN_ITAG) && ((regValue & 0x9000) == 0x1000))
		*option = RTL8306_VLAN_UNDOTAG; 	
	*/
	return CMM_SUCCESS;
}

int rtl8306e_vlan_portAcceptFrameType_get(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint32_t port, uint8_t *pAccept_frame_type)
{
	uint32_t regval;
	T_szMdioRtl8306e rtl8306e_mdio_info;	
    
	if((port > 5) || (pAccept_frame_type == NULL))
	{
		return CMM_FAILED;
	}
        

	if(0 == port)
	{
		rtl8306e_mdio_info.phy = 0;
		rtl8306e_mdio_info.reg = 31;
		rtl8306e_mdio_info.page = 0;
		if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
		{
			return CMM_FAILED;
		}		
		*pAccept_frame_type = (rtl8306e_mdio_info.value >> 7) & 0x3;
	}
	else if (5 == port)
	{
		rtl8306e_mdio_info.phy = 6;
		rtl8306e_mdio_info.reg = 30;
		rtl8306e_mdio_info.page = 1;
		if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
		{
			return CMM_FAILED;
		}		
		*pAccept_frame_type = (rtl8306e_mdio_info.value >> 6) & 0x3;
	}
    	else
	{
		rtl8306e_mdio_info.phy = port;
		rtl8306e_mdio_info.reg = 21;
		rtl8306e_mdio_info.page = 2;
		if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
		{
			return CMM_FAILED;
		}		
		*pAccept_frame_type = (rtl8306e_mdio_info.value >> 11) & 0x3;
	}

	return CMM_SUCCESS;
}


int CalcXorChecksum(void *apBuffer, uint32_t aBufferSize, uint32_t *apCheckSum)
{
	uint32_t vCheckSum = 0;
	uint32_t * vpCurr = (uint32_t *) apBuffer;
	
	if ( aBufferSize & 3 )
	{
		return BOOL_FALSE;
	}

	//need to read in four bytes at a time!
	while ( aBufferSize ) 
	{
		vCheckSum ^= *vpCurr;
		vpCurr++;
		aBufferSize -= 4;
	}
	
	*apCheckSum = (~vCheckSum);
	return BOOL_TRUE;
}

uint32_t checksum_32_pib (register const void * memory, register size_t extent, register uint32_t checksum) 

{

#ifdef __GNUC__

	while (extent >= sizeof (checksum)) 
	{
		checksum ^= *(typeof (checksum) *)(memory);
		memory += sizeof (checksum);
		extent -= sizeof (checksum);
	}

#else

	uint32_t * offset = (uint32_t *)(memory);
	while (extent >= sizeof (uint32_t)) 
	{
		extent -= sizeof (uint32_t);
		checksum ^= *offset++;
	}

#endif

	return (~checksum);
}


uint32_t checksum_32_mod (const uint8_t * ptr, register size_t len) 

{

	uint32_t crc32_tab[256]={0x0,0x4c11db7,0x9823b6e,0xd4326d9,   
                            0x130476dc,0x17c56b6b,0x1a864db2,0x1e475005,   
                            0x2608edb8,0x22c9f00f,0x2f8ad6d6,0x2b4bcb61,   
                            0x350c9b64,0x31cd86d3,0x3c8ea00a,0x384fbdbd,   
                            0x4c11db70,0x48d0c6c7,0x4593e01e,0x4152fda9,   
                            0x5f15adac,0x5bd4b01b,0x569796c2,0x52568b75,   
                            0x6a1936c8,0x6ed82b7f,0x639b0da6,0x675a1011,   
                            0x791d4014,0x7ddc5da3,0x709f7b7a,0x745e66cd,   
                            0x9823b6e0,0x9ce2ab57,0x91a18d8e,0x95609039,   
                            0x8b27c03c,0x8fe6dd8b,0x82a5fb52,0x8664e6e5,   
                            0xbe2b5b58,0xbaea46ef,0xb7a96036,0xb3687d81,   
                            0xad2f2d84,0xa9ee3033,0xa4ad16ea,0xa06c0b5d,   
                            0xd4326d90,0xd0f37027,0xddb056fe,0xd9714b49,   
                            0xc7361b4c,0xc3f706fb,0xceb42022,0xca753d95,   
                            0xf23a8028,0xf6fb9d9f,0xfbb8bb46,0xff79a6f1,   
                            0xe13ef6f4,0xe5ffeb43,0xe8bccd9a,0xec7dd02d,   
                            0x34867077,0x30476dc0,0x3d044b19,0x39c556ae,   
                            0x278206ab,0x23431b1c,0x2e003dc5,0x2ac12072,   
                            0x128e9dcf,0x164f8078,0x1b0ca6a1,0x1fcdbb16,   
                            0x18aeb13,0x54bf6a4,0x808d07d,0xcc9cdca,   
                            0x7897ab07,0x7c56b6b0,0x71159069,0x75d48dde,   
                            0x6b93dddb,0x6f52c06c,0x6211e6b5,0x66d0fb02,   
                            0x5e9f46bf,0x5a5e5b08,0x571d7dd1,0x53dc6066,   
                            0x4d9b3063,0x495a2dd4,0x44190b0d,0x40d816ba,   
                            0xaca5c697,0xa864db20,0xa527fdf9,0xa1e6e04e,   
                            0xbfa1b04b,0xbb60adfc,0xb6238b25,0xb2e29692,   
                            0x8aad2b2f,0x8e6c3698,0x832f1041,0x87ee0df6,   
                            0x99a95df3,0x9d684044,0x902b669d,0x94ea7b2a,   
                            0xe0b41de7,0xe4750050,0xe9362689,0xedf73b3e,   
                            0xf3b06b3b,0xf771768c,0xfa325055,0xfef34de2,   
                            0xc6bcf05f,0xc27dede8,0xcf3ecb31,0xcbffd686,   
                            0xd5b88683,0xd1799b34,0xdc3abded,0xd8fba05a,   
                            0x690ce0ee,0x6dcdfd59,0x608edb80,0x644fc637,   
                            0x7a089632,0x7ec98b85,0x738aad5c,0x774bb0eb,   
                            0x4f040d56,0x4bc510e1,0x46863638,0x42472b8f,   
                            0x5c007b8a,0x58c1663d,0x558240e4,0x51435d53,   
                            0x251d3b9e,0x21dc2629,0x2c9f00f0,0x285e1d47,   
                            0x36194d42,0x32d850f5,0x3f9b762c,0x3b5a6b9b,   
                            0x315d626,0x7d4cb91,0xa97ed48,0xe56f0ff,   
                            0x1011a0fa,0x14d0bd4d,0x19939b94,0x1d528623,   
                            0xf12f560e,0xf5ee4bb9,0xf8ad6d60,0xfc6c70d7,   
                            0xe22b20d2,0xe6ea3d65,0xeba91bbc,0xef68060b,   
                            0xd727bbb6,0xd3e6a601,0xdea580d8,0xda649d6f,   
                            0xc423cd6a,0xc0e2d0dd,0xcda1f604,0xc960ebb3,   
                            0xbd3e8d7e,0xb9ff90c9,0xb4bcb610,0xb07daba7,   
                            0xae3afba2,0xaafbe615,0xa7b8c0cc,0xa379dd7b,   
                            0x9b3660c6,0x9ff77d71,0x92b45ba8,0x9675461f,   
                            0x8832161a,0x8cf30bad,0x81b02d74,0x857130c3,   
                            0x5d8a9099,0x594b8d2e,0x5408abf7,0x50c9b640,   
                            0x4e8ee645,0x4a4ffbf2,0x470cdd2b,0x43cdc09c,   
                            0x7b827d21,0x7f436096,0x7200464f,0x76c15bf8,   
                            0x68860bfd,0x6c47164a,0x61043093,0x65c52d24,   
                            0x119b4be9,0x155a565e,0x18197087,0x1cd86d30,   
                            0x29f3d35,0x65e2082,0xb1d065b,0xfdc1bec,   
                            0x3793a651,0x3352bbe6,0x3e119d3f,0x3ad08088,   
                            0x2497d08d,0x2056cd3a,0x2d15ebe3,0x29d4f654,   
                            0xc5a92679,0xc1683bce,0xcc2b1d17,0xc8ea00a0,   
                            0xd6ad50a5,0xd26c4d12,0xdf2f6bcb,0xdbee767c,   
                            0xe3a1cbc1,0xe760d676,0xea23f0af,0xeee2ed18,   
                            0xf0a5bd1d,0xf464a0aa,0xf9278673,0xfde69bc4,   
                            0x89b8fd09,0x8d79e0be,0x803ac667,0x84fbdbd0,   
                            0x9abc8bd5,0x9e7d9662,0x933eb0bb,0x97ffad0c,   
                            0xafb010b1,0xab710d06,0xa6322bdf,0xa2f33668,   
                            0xbcb4666d,0xb8757bda,0xb5365d03,0xb1f740b4};
		uint32_t crc;
		uint32_t oldcrc32;   
     uint8_t  temp;       
     crc=0;   
     while (len--!=0)   
     {   temp=(crc>>24)&0xff;   
         oldcrc32=crc32_tab[*ptr^temp];   
         crc=(crc<<8)^oldcrc32;   
         ptr++;   
   
    }   
    return(crc); 
}

int gen_pib_file_path(uint8_t *path, const uint8_t ODA[])
{
	sprintf(path, "/var/tmp/%02X_%02X_%02X_%02X_%02X_%02X.PIB", 
		ODA[0], ODA[1], ODA[2], ODA[3], ODA[4], ODA[5]);
	mmead_debug_printf("gen_pib_file_path = [%s]\n", path);
	return 0;
}

int gen_mod_file_path(uint8_t *path, const uint8_t ODA[])
{
	sprintf(path, "/var/tmp/%02X_%02X_%02X_%02X_%02X_%02X.BIN", 
		ODA[0], ODA[1], ODA[2], ODA[3], ODA[4], ODA[5]);
	mmead_debug_printf("gen_mod_file_path = [%s]\n", path);
	return 0;
}

/********************************************************************************************
*	函数名称:ModuleSession
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int ModuleSession
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint32_t SessionID, T_MMEAD_WR_MOD_REQ_INFO *mmead_request)
{
	//size_t offset = 0;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];

#pragma pack (push,1)
	typedef struct __packed vs_module_operation_start_request 
	{
		header_eth ethernet;
		header_mme intellon;
		uint32_t RESERVED1;
		uint8_t NUM_OP_DATA;
		uint16_t MOD_OP;
		uint16_t MOD_OP_DATA_LEN;
		uint32_t MOD_OP_RSVD;
		uint32_t MOD_OP_SESSION_ID;
		uint8_t NUM_MODULES;
		vs_module_operation_spec MOD_OP_SPEC [1];
	}ModOperationWriteSessionReqInfo;

	typedef struct __packed vs_module_operation_start_confirm 
	{
		header_eth ethernet;
		header_mme intellon;
		uint16_t MSTATUS;
		uint16_t ERR_REC_CODE;
		uint32_t RESERVED;
		uint8_t NUM_OP_DATA;
		uint16_t MOD_OP;
		uint16_t MOD_OP_DATA_LEN;
		uint32_t MOD_OP_RSVD;
		uint32_t MOD_OP_SESSION_ID;
		uint8_t NUM_MODULES;
		vs_module_operation_data MOD_OP_DATA [1];
	}ModOperationWriteSessionAckInfo;
#pragma pack (pop)

	ModOperationWriteSessionReqInfo *request = (ModOperationWriteSessionReqInfo *)(buffer);
	ModOperationWriteSessionAckInfo *comfirm = (ModOperationWriteSessionAckInfo *)(buffer);
	vs_module_operation_spec * spec = (vs_module_operation_spec *)(&request->MOD_OP_SPEC);
	uint32_t module;
	ihpapi_result_t xresult;

	memset(buffer, 0, sizeof(buffer));
	
	EncodeEthernetHeader ((uint8_t *)&(request->ethernet), sizeof(struct header_eth), ODA, OSA);	
	EncodeIntellonHeader ((uint8_t *)&(request->intellon), sizeof(struct header_mme), (VS_MODULE_OPERATION | MMTYPE_REQ));

	request->RESERVED1 = ihtonl(0);			/* Reserved for future use (must be set to 0) */
	request->NUM_OP_DATA = 1;		/* Reserved for future use (must be set to 1) */
	request->MOD_OP = ihtons(0x10);	/* 10: Start Write Session */
	request->MOD_OP_DATA_LEN = ihtons(13 + sizeof (struct vs_module_operation_spec));
	request->MOD_OP_RSVD = ihtonl(0);		/* Reserved for future use (must be set to 0) */
	request->MOD_OP_SESSION_ID = ihtonl(SessionID);
	request->NUM_MODULES = 1;		/* Number of Modules in the Session (Maximum value is 2) */
	
	for (module = 0; module < request->NUM_MODULES; module++) 
	{
		spec->MODULE_ID = ihtons (mmead_request->MODULE_ID);
		spec->MODULE_SUB_ID = ihtons (mmead_request->MODULE_SUB_ID);
		spec->MODULE_LENGTH = ihtonl (mmead_request->MODULE_LENGTH);
		if( !CalcXorChecksum(	(void *)(mmead_request->MODULE_DATA), 
								mmead_request->MODULE_LENGTH, 
								&(spec->MODULE_CHKSUM)
							) 
		)
		{
			return CMM_MME_ERROR;
		}
	}

	if( mme_tx(MME_SK, buffer, sizeof(ModOperationWriteSessionReqInfo)) <= 0 )
	{
		return CMM_MME_ERROR;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_MODULE_OPERATION, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		if( intohs(comfirm->MSTATUS) )
		{
			return CMM_MME_ERROR;
		}
		else
		{
			return CMM_SUCCESS;
		}
	}
	else
	{
		return CMM_MME_ERROR;
	}
}

/********************************************************************************************
*	函数名称:ModuleWrite
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int ModuleWrite
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint32_t SessionID, T_MMEAD_WR_MOD_REQ_INFO *mmead_request)
{
	//size_t offset = 0;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];

#pragma pack (push,1)
	typedef struct __packed vs_module_operation_write_request 
	{
		header_eth ethernet;
		header_mme intellon;
		uint32_t RESERVED;
		uint8_t NUM_OP_DATA;
		uint16_t MOD_OP;
		uint16_t MOD_OP_DATA_LEN;
		uint32_t MOD_OP_RSVD;
		uint32_t MOD_OP_SESSION_ID;
		uint8_t MODULE_IDX;
		uint16_t MODULE_ID;
		uint16_t MODULE_SUB_ID;
		uint16_t MODULE_LENGTH;
		uint32_t MODULE_OFFSET;
		uint8_t MODULE_DATA [1400];
	}ModOperationWriteReqInfo;

	typedef struct __packed vs_module_operation_write_confirm 
	{
		header_eth ethernet;
		header_mme intellon;
		uint16_t MSTATUS;
		uint16_t ERR_REC_CODE;
		uint32_t RESERVED;
		uint8_t NUM_OP_DATA;
		uint16_t MOD_OP;
		uint16_t MOD_OP_DATA_LEN;
		uint32_t MOD_OP_RSVD;
		uint32_t MOD_OP_SESSION_ID;
		uint8_t MODULE_IDX;
		uint16_t MODULE_ID;
		uint16_t MODULE_SUB_ID;
		uint16_t MODULE_LENGTH;  /* ?? uint32_t or uint16_t ??*/
		uint32_t MODULE_OFFSET;
	}ModOperationWriteAckInfo;
#pragma pack (pop)
	
	ModOperationWriteReqInfo *request = (ModOperationWriteReqInfo *)(buffer);
	ModOperationWriteAckInfo *comfirm = (ModOperationWriteAckInfo *)(buffer);
	ihpapi_result_t xresult;

	memset(buffer, 0, sizeof(buffer));
	
	EncodeEthernetHeader ((uint8_t *)&(request->ethernet), sizeof(struct header_eth), ODA, OSA);	
	EncodeIntellonHeader ((uint8_t *)&(request->intellon), sizeof(struct header_mme), (VS_MODULE_OPERATION | MMTYPE_REQ));

	request->RESERVED = ihtonl(0);			/* Reserved for future use (must be set to 0) */
	request->NUM_OP_DATA = 1;		/* Reserved for future use (must be set to 1) */
	request->MOD_OP = ihtons (0x11);	/* 10: Write Module to Memory */
	request->MOD_OP_DATA_LEN = ihtons (23 + mmead_request->MODULE_LENGTH);
	request->MOD_OP_RSVD = ihtonl(0);
	request->MOD_OP_SESSION_ID = ihtonl (SessionID);
	request->MODULE_IDX = 0;			/* (must be set to 0) */
	request->MODULE_ID = ihtons (mmead_request->MODULE_ID);
	request->MODULE_SUB_ID = ihtons (mmead_request->MODULE_SUB_ID);
	request->MODULE_LENGTH = ihtons (mmead_request->MODULE_LENGTH);
	request->MODULE_OFFSET = ihtonl (0);
	memcpy(request->MODULE_DATA, mmead_request->MODULE_DATA, mmead_request->MODULE_LENGTH);

	if( mme_tx(MME_SK, buffer, sizeof(ModOperationWriteReqInfo)) <= 0 )
	{
		return CMM_MME_ERROR;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_MODULE_OPERATION, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		if( intohs(comfirm->MSTATUS) )
		{
			return CMM_MME_ERROR;
		}
		else
		{
			return CMM_SUCCESS;
		}
	}
	else
	{
		return CMM_MME_ERROR;
	}	
}

/********************************************************************************************
*	函数名称:ModuleCommit
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int ModuleCommit
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint32_t SessionID, T_MMEAD_WR_MOD_REQ_INFO *mmead_request)
{
	//size_t offset = 0;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];

#pragma pack (push,1)
	typedef struct __packed vs_module_operation_commit_request 
	{
		header_eth ethernet;
		header_mme intellon;
		uint32_t RESERVED;
		uint8_t NUM_OP_DATA;
		uint16_t MOD_OP;
		uint16_t MOD_OP_DATA_LEN;
		uint32_t MOD_OP_RSVD;
		uint32_t MOD_OP_SESSION_ID;
		uint32_t COMMIT_CODE;
		uint8_t RSVD [20];
	}ModOperationWriteCommitReqInfo;

	typedef struct __packed vs_module_operation_commit_confirm 
	{
		header_eth ethernet;
		header_mme intellon;
		uint16_t MSTATUS;
		uint16_t ERR_REC_CODE;
		uint32_t RESERVED1;
		uint8_t NUM_OP_DATA;
		uint16_t MOD_OP;
		uint16_t MOD_OP_DATA_LEN;
		uint32_t MOD_OP_RSVD;
		uint32_t MOD_OP_SESSION_ID;
		uint32_t COMMIT_CODE;
		uint8_t NUM_MODULES;
		vs_module_operation_data MOD_OP_DATA [1];
	}ModOperationWriteCommitAckInfo;
#pragma pack (pop)

	ModOperationWriteCommitReqInfo *request = (ModOperationWriteCommitReqInfo *)(buffer);
	ModOperationWriteCommitAckInfo *comfirm = (ModOperationWriteCommitAckInfo *)(buffer);
	ihpapi_result_t xresult;

	memset(buffer, 0, sizeof(buffer));
	
	EncodeEthernetHeader ((uint8_t *)&(request->ethernet), sizeof(struct header_eth), ODA, OSA);	
	EncodeIntellonHeader ((uint8_t *)&(request->intellon), sizeof(struct header_mme), (VS_MODULE_OPERATION | MMTYPE_REQ));

	request->RESERVED = ihtonl(0);			/* Reserved for future use (must be set to 0) */
	request->NUM_OP_DATA = 1;		/* Reserved for future use (must be set to 1) */
	request->MOD_OP = ihtons (0x12);	/* 12: Commit Module from Memory to NVM */
	request->MOD_OP_DATA_LEN = ihtons (36);
	request->MOD_OP_RSVD = ihtonl(0);
	request->MOD_OP_SESSION_ID = ihtonl (SessionID);
	request->COMMIT_CODE = ihtonl (0);

	if( mme_tx(MME_SK, buffer, sizeof(ModOperationWriteCommitReqInfo)) <= 0 )
	{
		return CMM_MME_ERROR;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_MODULE_OPERATION, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		if( intohs(comfirm->MSTATUS) )
		{
			return CMM_MME_ERROR;
		}
		else
		{
			return CMM_SUCCESS;
		}
	}
	else
	{
		return CMM_MME_ERROR;
	}
}

/********************************************************************************************
*	函数名称:ModuleCommit
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int WritePIB(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[])
{
	int ret = CMM_SUCCESS;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	FILE *fp;
	uint32_t length = INT6K_BLOCKSIZE;
	uint32_t offset = 0;
	uint32_t extent = 0;
	uint8_t PIB_PATH[64] = {0};
	
#pragma pack (push,1)
	typedef struct __packed vs_wr_mod_request 
	{
		header_eth ethernet;
		header_mme intellon;
		uint8_t MODULEID;
		uint8_t RESERVED;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
		uint32_t MCHKSUM;
		uint8_t MBUFFER [INT6K_BLOCKSIZE];
	}ModWriteReqInfo;

	typedef struct __packed vs_wr_mod_confirm 
	{
		header_eth ethernet;
		header_mme intellon;
		uint8_t MSTATUS;
		uint8_t MODULEID;
		uint8_t RESERVED;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
	}ModWriteAckInfo;
#pragma pack (pop)

	ModWriteReqInfo *request = (ModWriteReqInfo *)(buffer);
	ModWriteAckInfo *comfirm = (ModWriteAckInfo *)(buffer);
	ihpapi_result_t xresult;

	mmead_debug_printf("-------->WritePIB\n");
	gen_pib_file_path(PIB_PATH, ODA);
	
	/* 判断文件是否存在*/
	if( access(PIB_PATH, 0) )
	{
		printf("WritePIB error : file %s does not existed\n", PIB_PATH);
		return CMM_MME_ERROR;
	}
	else if( (fp = fopen(PIB_PATH, "rb")) < 0 )
	{
		return CMM_MME_ERROR;
	}
	else
	{
		fseek(fp, 0L, SEEK_END);
		extent = ftell(fp);	/* 文件的实际长度*/
		fseek(fp, 0L, SEEK_SET);
		//printf("MME_Atheros_MsgWritePib extent = %d\n", extent);
	}

	while (extent)
	{
		memset(buffer, 0, sizeof(buffer));
		
		EncodeEthernetHeader ((uint8_t *)&(request->ethernet), sizeof(struct header_eth), ODA, OSA);	
		EncodeIntellonHeader ((uint8_t *)&(request->intellon), sizeof(struct header_mme), (VS_WR_MOD | MMTYPE_REQ));
		if (length > extent) 
		{
			length = extent;
		}
		if (fread (request->MBUFFER, 1, length, fp) < length)
		{
			ret = CMM_MME_ERROR;
			break;
		}
		
		request->MODULEID = VS_MODULE_PIB;
		request->RESERVED = 0;
		request->MLENGTH = ihtons(length);
		request->MOFFSET = ihtonl(offset);
		request->MCHKSUM = checksum_32_pib(request->MBUFFER, length, 0);
		
		if( mme_tx(MME_SK, buffer, sizeof(ModWriteReqInfo)) <= 0 )
		{
			ret = CMM_MME_ERROR;
			break;
		}
		memset(buffer,0,sizeof(buffer));
		if ( mme_rx(MME_SK, VS_WR_MOD, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
		{
			ret = CMM_MME_ERROR;
			break;
		}
		if(xresult.validData)
		{
			//ret = intohs(comfirm->MSTATUS);
			ret = comfirm->MSTATUS;
			if( ret )
			{
				ret = CMM_MME_ERROR;
				break;
			}
			else
			{
				if (intohs (comfirm->MLENGTH) != length) 
				{
					ret = CMM_MME_ERROR;
					break;
				}
				if (intohl (comfirm->MOFFSET) != offset) 
				{
					ret = CMM_MME_ERROR;
					break;
				}
				extent -= length;
				offset += length;
				continue;
			}
		}	
		else
		{
			ret = CMM_MME_ERROR;
			break;
		}
	}
	/* 关闭文件*/
	fclose(fp);
	//remove(PIB_PATH);
	return ret;
}

/********************************************************************************************
*	函数名称:FlashDevice
*	函数功能:将PIB写入NVM并重启目标设备
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int FlashDevice(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[])
{
	//int ret = CMM_SUCCESS;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	
#pragma pack (push,1)
	typedef struct __packed vs_mod_nvm_request 
	{
		header_eth ethernet;
		header_mme intellon;
		uint8_t MODULEID;
		uint8_t RESERVED;
	}ModNvmReqInfo;

	typedef struct __packed vs_mod_nvm_confirm 
	{
		header_eth ethernet;
		header_mme intellon;
		uint8_t MSTATUS;
		uint8_t MODULEID;
	}ModNvmAckInfo;
#pragma pack (pop)

	ModNvmReqInfo *request = (ModNvmReqInfo *)(buffer);
	ModNvmAckInfo *comfirm = (ModNvmAckInfo *)(buffer);
	ihpapi_result_t xresult;
	
	mmead_debug_printf("-------->FlashDevice\n");
	
	memset(buffer, 0, sizeof(buffer));
	
	EncodeEthernetHeader ((uint8_t *)&(request->ethernet), sizeof(struct header_eth), ODA, OSA);	
	EncodeIntellonHeader ((uint8_t *)&(request->intellon), sizeof(struct header_mme), (VS_MOD_NVM | MMTYPE_REQ));

	request->MODULEID = VS_MODULE_PIB;	
	request->RESERVED = 0;	/* Reserved for future use (must be set to 0) */

	if( mme_tx(MME_SK, buffer, sizeof(ModNvmReqInfo)) <= 0 )
	{
		printf("FlashDevice error : mme_tx\n");
		return CMM_MME_ERROR;
	}
	memset(buffer,0,sizeof(buffer));
	if ( mme_rx(MME_SK, VS_MOD_NVM, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		printf("FlashDevice error : mme_rx\n");
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		//return intohs(comfirm->MSTATUS);
		if(comfirm->MSTATUS)
		{
			printf("\nFlashDevice: comfirm->MSTATUS = %d\n", comfirm->MSTATUS);
		}
		return comfirm->MSTATUS;
	}	
	else
	{
		printf("FlashDevice error : xresult.validData\n");
		return CMM_MME_ERROR;
	}	
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgNeRefresh
*	函数功能:更新在线网元节点信息，排除已经下线的设备
*				   
*	返回值:无
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
void MME_Atheros_MsgNeRefresh
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_TOPOLOGY *pNEList)
{
	int i = 0;	
	int j = 0;
	//uint8_t mstring[64] = {0};
	uint8_t MB[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
	T_MMEAD_DEV_INFO stDevInfo;
	T_MMEAD_TOPOLOGY NEList;
	assert( NULL != pNEList );
	uint32_t RealStations = pNEList->clt.NumStas;
	

	mmead_debug_printf("-------->MME_Atheros_MsgNeRefresh\n");
	//printf("sizeof(T_MMEAD_TOPOLOGY) = %d\n", sizeof(T_MMEAD_TOPOLOGY));
	
	bzero((char *)&NEList, sizeof(NEList));
	memcpy(&NEList, pNEList, sizeof(T_MMEAD_TOPOLOGY));
	bzero((char *)pNEList, sizeof(T_MMEAD_TOPOLOGY));

#if 1
	/* 获取CLT的设备类型，同时可以确定是否在线*/	
	if( MME_Atheros_MsgGetDeviceInfo(MME_SK, NEList.clt.Mac, &stDevInfo) == CMM_SUCCESS )
	{
		NEList.clt.DevType = stDevInfo.DevType;
		NEList.clt.CRC[0] = stDevInfo.CRC[0];
		NEList.clt.CRC[1] = stDevInfo.CRC[1];
	}
	else
	{
		/* 如果CLT通讯都有问题就不再考虑CNU了*/
		return;
	}
#else
	NEList.clt.DevType = WEC_3801I;
	NEList.clt.CRC[0] = 0;
	NEList.clt.CRC[1] = 0;
#endif
	/* 获取CNU的设备类型，同时可以确定是否在线*/
	if( NEList.clt.NumStas > 0 )
	{
		for( i=0; i<NEList.clt.NumStas; i++ )
		{
			if( memcmp(NEList.cnu[i].Mac, MB, 6) == 0 )
			{
				/* 有时候会出现广播地址，在这里排除掉*/
				NEList.cnu[i].DevType = WEC_XXXXX;
				NEList.cnu[i].CRC[0] = 0;
				NEList.cnu[i].CRC[1] = 0;
				RealStations--;
				continue;
			}
			//usleep(5000);
			if( MME_Atheros_MsgGetDeviceInfo(MME_SK, NEList.cnu[i].Mac, &stDevInfo) == CMM_SUCCESS )
			{
				NEList.cnu[i].DevType = stDevInfo.DevType;
				NEList.cnu[i].CRC[0] = stDevInfo.CRC[0];
				NEList.cnu[i].CRC[1] = stDevInfo.CRC[1];
			}
			else
			{
				/* 此分支标示CNU不在线*/
				NEList.cnu[i].DevType = WEC_XXXXX;
				NEList.cnu[i].CRC[0] = 0;
				NEList.cnu[i].CRC[1] = 0;
				RealStations--;
			}
		}
	}	
	
	/* 将实际在线的设备写到结构体中*/
	pNEList->clt.DevType = NEList.clt.DevType;
	pNEList->clt.NumStas = RealStations;
	memcpy(pNEList->clt.Mac, NEList.clt.Mac, IHPAPI_ETHER_ADDR_LEN);
	pNEList->clt.CRC[0] = NEList.clt.CRC[0];
	pNEList->clt.CRC[1] = NEList.clt.CRC[1];
	
	if( RealStations > 0 )
	{
		for( i=0, j=0; i<NEList.clt.NumStas; i++ )
		{
			if( NEList.cnu[i].DevType == WEC_XXXXX )
			{
				continue;
			}
			else
			{
				pNEList->cnu[j].DevType = NEList.cnu[i].DevType;
				pNEList->cnu[j].AvgPhyRx = NEList.cnu[i].AvgPhyRx;
				pNEList->cnu[j].AvgPhyTx = NEList.cnu[i].AvgPhyTx;
				pNEList->cnu[j].Tei = NEList.cnu[i].Tei;
				memcpy(pNEList->cnu[j].Mac, NEList.cnu[i].Mac, IHPAPI_ETHER_ADDR_LEN);
				pNEList->cnu[j].CRC[0] = NEList.cnu[i].CRC[0];
				pNEList->cnu[j].CRC[1] = NEList.cnu[i].CRC[1];
				j++;
			}
		}
	}
	return;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetNetWorkInfo
*	函数功能:ihpapi_GetNetworkInfo
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetNetWorkInfo
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_TOPOLOGY *pNEList)
{
	int packetsize;
	int i = 0;	
	int recv_msg_len = 0;
	ihpapi_result_t xresult;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	
	assert( NULL != pNEList );

	mmead_debug_printf("-------->MME_Atheros_MsgGetNetWorkInfo\n");
	
	memset(pNEList, 0, sizeof(T_MMEAD_TOPOLOGY));
	memset(buffer, 0, IHPAPI_ETHER_MAX_LEN);
	
	packetsize = ihpapi_GetNetworkInfo(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);
	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_NW_INFO, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS )
	{
		return CMM_MME_ERROR;
	}
	else if( xresult.validData )
	{
		pNEList->clt.DevType = WEC_XXXXX;
		pNEList->clt.NumStas = xresult.data.netInfo.nwinfo[17];
		memcpy(pNEList->clt.Mac, &(xresult.data.netInfo.nwinfo[10]), IHPAPI_ETHER_ADDR_LEN);

		if( pNEList->clt.NumStas > 0 )
		{
			for( i=0; i<pNEList->clt.NumStas; i++ )
			{
				pNEList->cnu[i].DevType = WEC_XXXXX;
				memcpy(pNEList->cnu[i].Mac, &(xresult.data.netInfo.nwinfo[18+i*15]), IHPAPI_ETHER_ADDR_LEN);
				pNEList->cnu[i].Tei = xresult.data.netInfo.nwinfo[24+i*15];
				pNEList->cnu[i].AvgPhyTx = xresult.data.netInfo.nwinfo[31+i*15];
				pNEList->cnu[i].AvgPhyRx = xresult.data.netInfo.nwinfo[32+i*15];
			}
		}
		return CMM_SUCCESS;
	}
	else
	{
		return CMM_MME_ERROR;
	}
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetNetWorkInfoStats
*	函数功能:ihpapi_GetNetworkInfoStats
*				   only 支持 v1版本 在 oui 之前增加FMI
*	返回值:操作是否成功的状态码
*	作者:Stan
*	时间:2013-03-12
*********************************************************************************************/
int MME_Atheros_MsgGetNetWorkInfoStats
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_TOPOLOGY *pNEList)
{
	int packetsize;
	int i = 0;	
	int recv_msg_len = 0;
	ihpapi_result_t xresult;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	
	assert( NULL != pNEList );

	mmead_debug_printf("-------->MME_Atheros_MsgGetNetWorkInfoStats\n");
	
	memset(pNEList, 0, sizeof(T_MMEAD_TOPOLOGY));
	memset(buffer, 0, IHPAPI_ETHER_MAX_LEN);
	
	packetsize = ihpapi_GetNetworkInfoStats(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);
		
	if( 0 != packetsize )
	{	
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{		
			return CMM_MME_ERROR;
		}
	}
	else
	{		
		return CMM_FAILED;
	}
  
  
	memset(buffer,0,sizeof(buffer));

	if ( mme_v1_rx(MME_SK, VS_NW_INFO_STATS, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS )
	{
		return CMM_MME_ERROR;
	}
	else if( xresult.validData )
	{
		pNEList->clt.DevType = WEC_3801I;
		pNEList->clt.NumStas = xresult.data.netInfoStats.NUM_STAS;
		memcpy(pNEList->clt.Mac, xresult.data.netInfoStats.CCO_MACADDR, IHPAPI_ETHER_ADDR_LEN);
		
		if( pNEList->clt.NumStas > 0 )
		{
			v1sta_t *p = (struct stav1_s *)&xresult.data.netInfoStats.nwinfostats;
			for( i=0; i<pNEList->clt.NumStas; i++ )
			{				
				pNEList->cnu[i].DevType = WEC_XXXXX;
				memcpy(pNEList->cnu[i].Mac, p->DA, IHPAPI_ETHER_ADDR_LEN);
				pNEList->cnu[i].Tei = p->TEI;
				pNEList->cnu[i].AvgPhyTx = p->AVGPHYDR_TX[0];
				pNEList->cnu[i].AvgPhyRx = p->AVGPHYDR_RX[0];
				p++;
			}
		}
		return CMM_SUCCESS;
	}
	else
	{
		return CMM_MME_ERROR;
	}
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetManufacturerInfo
*	函数功能:ihpapi_GetManufacturerInfo
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetManufacturerInfo
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint8_t *pManufacturerInfo)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	assert( NULL != pManufacturerInfo );
	mmead_debug_printf("-------->MME_Atheros_MsgGetManufacturerInfo\n");
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_GetManufacturerInfo(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);

	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_MFG_STRING, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	else if(xresult.validData)
	{
		memcpy(pManufacturerInfo, xresult.data.mftrInfo.mstring, 
			((xresult.data.mftrInfo.mstrlength > STRNG_MAX_LEN)?STRNG_MAX_LEN:xresult.data.mftrInfo.mstrlength));
		return CMM_SUCCESS;
	}
	else
	{
		return CMM_MME_ERROR;
	}	
}


/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetSwVer
*	函数功能:获取设备的软件版本
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetSwVer
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint8_t *pStr)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("-------->MME_Atheros_MsgGetSwVer\n");
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_GetVersionInfo(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);

	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_SW_VER, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		memcpy(pStr, xresult.data.version.mversion, 
			((xresult.data.version.mverlength > (2*STRNG_MAX_LEN))?(2*STRNG_MAX_LEN):xresult.data.version.mverlength));
		return CMM_SUCCESS;
	}	
	return CMM_MME_ERROR;
}

int MME_Atheros_MsgGetFrequencyBandSelection
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_FBS *pdata)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("-------->MME_Atheros_MsgGetFrequencyBandSelection\n");
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_GetFrequencyBandSelection(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);

	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_GET_PROPERTY, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		pdata->FBSTATUS = xresult.data.FrequencyBandSelectionInfo.FBSTATUS;
		pdata->START_BAND = xresult.data.FrequencyBandSelectionInfo.START_BAND;
		pdata->STOP_BAND = xresult.data.FrequencyBandSelectionInfo.STOP_BAND;
		return CMM_SUCCESS;
	}	
	return CMM_MME_ERROR;
}

int MME_Atheros_MsgSetFrequencyBandSelection
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_FBS *pdata)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;
	ihpapi_getFrequencyBandSelection_t FrequencyBandSelectionInfo;

	mmead_debug_printf("-------->MME_Atheros_MsgSetFrequencyBandSelection\n");
	memset(buffer, 0, sizeof(buffer));
	FrequencyBandSelectionInfo.FBSTATUS = pdata->FBSTATUS;
	FrequencyBandSelectionInfo.START_BAND = pdata->START_BAND;
	FrequencyBandSelectionInfo.STOP_BAND = pdata->STOP_BAND;
	
	packetsize = ihpapi_SetFrequencyBandSelection(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer, &FrequencyBandSelectionInfo);

	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_SET_PROPERTY, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		return xresult.opStatus.status;
	}	
	return CMM_MME_ERROR;
}

int MME_Atheros_MsgGetTxGain(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint8_t *pdata)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("-------->MME_Atheros_MsgGetTxGain\n");
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_GetTxGain(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);

	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_GET_PROPERTY, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		*pdata = xresult.data.txGainInfo.TX_GAIN;
		return CMM_SUCCESS;
	}	
	return CMM_MME_ERROR;
}

int MME_Atheros_MsgSetTxGain(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint8_t tx_gain)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("-------->MME_Atheros_MsgSetTxGain\n");
	memset(buffer, 0, sizeof(buffer));
	
	packetsize = ihpapi_SetTxGain(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer, tx_gain);

	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_SET_PROPERTY, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		return xresult.opStatus.status;
	}	
	return CMM_MME_ERROR;
}

int MME_Atheros_MsgGetUserHFID(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint8_t *pdata)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("-------->MME_Atheros_MsgGetUserHFID\n");
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_GetUserHFID(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);

	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_GET_PROPERTY, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		memcpy(pdata, xresult.data.UserHFIDInfo.USER_HFID, sizeof( xresult.data.UserHFIDInfo.USER_HFID));
		return CMM_SUCCESS;
	}	
	return CMM_MME_ERROR;
}
/*
int MME_Atheros_MsgSetUserHFID(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint8_t user_hfid[])
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("-------->MME_Atheros_MsgSetUserHFID\n");
	memset(buffer, 0, sizeof(buffer));
	
	packetsize = ihpapi_SetUserHFID(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer, user_hfid);

	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_SET_PROPERTY, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		return xresult.opStatus.status;
	}	
	return CMM_MME_ERROR;
}*/

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetCltMac
*	函数功能:获取CLT的MAC地址
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetCltMac(T_MME_SK_HANDLE *MME_SK, uint8_t clt_mac[])
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	uint8_t ODA [6] = 
	{
		0x00,
		0xb0,
		0x52,
		0x00,
		0x00,
		0x01 
	};

	mmead_debug_printf("-------->MME_Atheros_MsgGetCltMac\n");
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_GetVersionInfo(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);

	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			//perror("ERROR: mme_tx failed\n");
			return CMM_MME_ERROR;
		}
	}
	else
	{
		//perror("ihpapi_GetVersionInfo error\n");
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_SW_VER, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	else if(xresult.validData)
	{
		memcpy(clt_mac, xresult.OSA, IHPAPI_ETHER_ADDR_LEN);
		return CMM_SUCCESS;
	}	
	else
	{
		return CMM_MME_ERROR;
	}
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgResetDevice
*	函数功能:ihpapi_ResetDevice
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgResetDevice(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[])
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("-------->MME_Atheros_MsgResetDevice\n");
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_ResetDevice(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);

	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_RS_DEV, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	else if(xresult.validData)
	{
		//printf("MME_Atheros_MsgResetDevice status = %d\n", xresult.opStatus.status);
		return xresult.opStatus.status;
	}
	else
	{
		return CMM_MME_ERROR;
	}	
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetTopology
*	函数功能:获取网元节点信息
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetTopology
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_TOPOLOGY *pNEList)
{
	mmead_debug_printf("-------->MME_Atheros_MsgGetTopology\n");
	
	/* 获取Athreos设备的网络节点信息，注意是5分钟之前的*/
	if( MME_Atheros_MsgGetNetWorkInfo(MME_SK, ODA, pNEList) != CMM_SUCCESS )
	{
		return CMM_MME_ERROR;
	}

	/* 排除已经下线的CNU设备*/
	MME_Atheros_MsgNeRefresh(MME_SK, ODA, pNEList);

	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetTopologyStats
*	函数功能:74系列分片获取网元节点信息
*				   
*	返回值:操作是否成功的状态码
*	作者:stan
*	时间:2013-03-12
*********************************************************************************************/
int MME_Atheros_MsgGetTopologyStats
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_TOPOLOGY *pNEList)
{
	mmead_debug_printf("-------->MME_Atheros_MsgGetTopologyStats\n");
	
	/* 获取Athreos设备的网络节点信息，注意是5分钟之前的*/
	if( MME_Atheros_MsgGetNetWorkInfoStats(MME_SK, ODA, pNEList) != CMM_SUCCESS )
	{
		return CMM_MME_ERROR;
	}

	/* 排除已经下线的CNU设备*/
	MME_Atheros_MsgNeRefresh(MME_SK, ODA, pNEList);

	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetRtl8306eBandwidthConfig
*	函数功能:
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2013-10-18
*********************************************************************************************/
int MME_Atheros_MsgGetRtl8306eBandwidthConfig
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	st_cnuSwitchBandwidthConfig *bandwidthInfo
)
{
	int i = 0;
	uint32_t tmp;
	uint32_t tmp2 = 0;
	T_szMdioRtl8306e rtl8306e_mdio_info;

	rtl8306e_mdio_info.phy = 0;
	rtl8306e_mdio_info.reg = 21;
	rtl8306e_mdio_info.page = 3;
	if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
	{
		return CMM_FAILED;
	}
	else
	{
		tmp = (rtl8306e_mdio_info.value >> 15) & 0x1;
		bandwidthInfo->g_rx_bandwidth_control_enable = (tmp == 0)?1:0;
	}

	for(i=0;i<=4;i++)
	{
		rtl8306e_mdio_info.phy = i;
		rtl8306e_mdio_info.reg = 21;
		rtl8306e_mdio_info.page = 2;
		if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
		{
			return CMM_FAILED;
		}
		else
		{
			bandwidthInfo->rxPort[i].bandwidth_value = (rtl8306e_mdio_info.value) & 0x7ff;
			bandwidthInfo->rxPort[i].bandwidth_control_enable = bandwidthInfo->g_rx_bandwidth_control_enable;
		}		
	}

	for(i=0;i<=4;i++)
	{
		rtl8306e_mdio_info.phy = i;
		rtl8306e_mdio_info.reg = 18;
		rtl8306e_mdio_info.page = 2;
		if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
		{
			return CMM_FAILED;
		}
		else
		{
			tmp = (rtl8306e_mdio_info.value >> 15) & 0x1;
			bandwidthInfo->txPort[i].bandwidth_control_enable = (tmp == 0)?0:1;			
			bandwidthInfo->txPort[i].bandwidth_value = (rtl8306e_mdio_info.value) & 0x7ff;
			tmp2 |= bandwidthInfo->txPort[i].bandwidth_control_enable;
		}		
	}

	bandwidthInfo->g_tx_bandwidth_control_enable = (tmp2 == 0)?0:1;
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetRtl8306eVlanConfig
*	函数功能:
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2013-10-18
*********************************************************************************************/
int MME_Atheros_MsgGetRtl8306eVlanConfig
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	st_cnuSwitchVlanConfig *vlanInfo
)
{
	int i = 0;
	uint32_t tmp;
	T_szMdioRtl8306e rtl8306e_mdio_info;
	
	//get vlan_enable [phy 0 reg 18 page 0][bit:8]
	rtl8306e_mdio_info.phy = 0;
	rtl8306e_mdio_info.reg = 18;
	rtl8306e_mdio_info.page = 0;
	if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
	{
		return CMM_FAILED;
	}
	else
	{
		/* 1 = Disable VLAN; 0 = Enable VLAN */
		tmp = (rtl8306e_mdio_info.value >> 8) & 0x1;
		vlanInfo->vlan_enable = (tmp == 0)?1:0;
	}

	//get 802.1q vlan status [phy 0 reg 16 page 0][bit:10]
	rtl8306e_mdio_info.phy = 0;
	rtl8306e_mdio_info.reg = 16;
	rtl8306e_mdio_info.page = 0;
	if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
	{
		return CMM_FAILED;
	}
	else
	{
		/* 1 = Disable 802.1Q tagged-VID Aware function */
		tmp = (rtl8306e_mdio_info.value >> 10) & 0x1;
		vlanInfo->vlan_tag_aware = (tmp == 0)?1:0;

		tmp = (rtl8306e_mdio_info.value >> 9) & 0x1;
		vlanInfo->ingress_filter = (tmp == 0)?1:0;
		/* 1 = The switch accepts all frames it receives whether tagged or untagged */
		/* 0 = The switch will only accept tagged frames and will drop untagged frames */
		tmp = (rtl8306e_mdio_info.value >> 8) & 0x1;
		vlanInfo->g_admit_control = (tmp == 0)?1:0;
	}

	/* get vlan info for each port */
	for( i=0; i<=5; i++ )
	{
		/* pvid */
		if( CMM_SUCCESS != rtl8306_getPvid(MME_SK, ODA, i, &tmp) )
		{
			return CMM_FAILED;
		}
		vlanInfo->vlan_port[i].pvid = tmp;
		/* port egress tag mode */
		/* 11 = Do not insert or remove VLAN tags to/from packet which is output on this port*/
		/* 10 = The switch will add VLAN tags to packets, if they are not tagged*/
		/* 01 = The switch will remove VLAN tags from packets, if they are tagged*/
		/* 00 = The switch will remove VLAN tags from packets then add new tags to them*/
		if( CMM_SUCCESS != rtl8306_getPortVlanMode(MME_SK, ODA, i, &tmp) )
		{
			return CMM_FAILED;
		}
		vlanInfo->vlan_port[i].egress_mode = tmp;
		/* port admit frame type */
		if( CMM_SUCCESS != rtl8306e_vlan_portAcceptFrameType_get(MME_SK, ODA, i, &(vlanInfo->vlan_port[i].admit_control)) )
		{
			return CMM_FAILED;
		}
	}
	
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetRtl8306eLoopDetect
*	函数功能:
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2013-10-18
*********************************************************************************************/
int MME_Atheros_MsgGetRtl8306eLoopDetect
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	st_cnuSwitchLoopDetect *loopDetect
)
{
	int i = 0;
	uint32_t tmp;
	T_szMdioRtl8306e rtl8306e_mdio_info;
	
	//get loop detect status [phy 0 reg 16 page 0][bit:2]
	rtl8306e_mdio_info.phy = 0;
	rtl8306e_mdio_info.reg = 16;
	rtl8306e_mdio_info.page = 0;
	if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
	{
		return CMM_FAILED;
	}
	else
	{
		/* 0 = Disable VLAN; 1 = Enable loop detect */
		tmp = (rtl8306e_mdio_info.value >> 2) & 0x1;
		loopDetect->status = (tmp == 0)?0:1;
	}
	
	/* get port loop status */
	for( i=0; i<=4; i++ )
	{
		//get port x loop status [phy 0~4 reg 24 page 0][bit:8]
		rtl8306e_mdio_info.phy = i;
		rtl8306e_mdio_info.reg = 24;
		rtl8306e_mdio_info.page = 0;
		if( CMM_SUCCESS != rtl8306e_read(MME_SK, ODA, &rtl8306e_mdio_info) )
		{
			return CMM_FAILED;
		}
		else
		{
			/* 0 = No loop exists on port x; 1 = A loop has been detected on port x */
			tmp = (rtl8306e_mdio_info.value >> 8) & 0x1;
			loopDetect->port_loop_status[i] = (tmp == 0)?0:1;
		}
	}
	
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetPhyReg
*	函数功能:
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetPhyReg(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szMdioPhy *v)
{
	//printf("\n[MME_Atheros_MsgGetPhyReg :phy %x register %x]\n", v->phy, v->reg);
	return smiRead(MME_SK, ODA, v);
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetPhyReg
*	函数功能:
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgSetPhyReg(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szMdioPhy *v)
{
	//printf("\n[MME_Atheros_MsgSetPhyReg :phy %x register %x value %x]\n", v->phy, v->reg, v->value);
	return smiWrite(MME_SK, ODA, v);
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetSwReg
*	函数功能:
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetSwReg(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szMdioSw *v)
{
	uint32_t reg_word_addr;
	T_szMdioPhy smi;

	//printf("\n[MME_Atheros_MsgGetSwReg : register %x]\n", v->reg);
	/* change reg_addr to 16-bit word address, 32-bit aligned */
	reg_word_addr = ((v->reg) & 0xfffffffc) >> 1;

	/* configure register high address */	 
	smi.phy = 0x18;
	smi.reg = 0x0;
	smi.value = ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */
	if( CMM_SUCCESS != smiWrite(MME_SK, ODA, &smi) )
	{
		return CMM_FAILED;
	}

	/* For some registers such as MIBs, since it is read/clear, we should */
	/* read the lower 16-bit register then the higher one */

	/* read register in lower address */
	smi.phy = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
	smi.reg = (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
	if( CMM_SUCCESS != smiRead(MME_SK, ODA, &smi) )
	{
		return CMM_FAILED;
	}
	else
	{
		v->value = smi.value;
	}

	/* read register in higher address */
	reg_word_addr++;
	smi.phy = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
	smi.reg = (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
	if( CMM_SUCCESS != smiRead(MME_SK, ODA, &smi) )
	{
		return CMM_FAILED;
	}
	else
	{
		 v->value |= (smi.value << 16);
	}

	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgSetSwReg
*	函数功能:
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgSetSwReg(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szMdioSw *v)
{
	uint32_t reg_word_addr;
	T_szMdioPhy smi;

	//printf("\n[MME_Atheros_MsgSetSwReg : register %x value %x]\n", v->reg, v->value);
	/* change reg_addr to 16-bit word address, 32-bit aligned */
	reg_word_addr = ((v->reg) & 0xfffffffc) >> 1;

	/* configure register high address */
	smi.phy = 0x18;
	smi.reg = 0x0;
	smi.value = ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */
	if( CMM_SUCCESS != smiWrite(MME_SK, ODA, &smi) )
	{
		return CMM_FAILED;
	}

	/* For some registers such as ARL and VLAN, since they include BUSY bit */
	/* in lower address, we should write the higher 16-bit register then the */
	/* lower one */

	/* write register in higher address */
	reg_word_addr++;
	smi.phy = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
	smi.reg = (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
	smi.value = (((v->value) >> 16) & 0xffff);
	if( CMM_SUCCESS != smiWrite(MME_SK, ODA, &smi) )
	{
		return CMM_FAILED;
	}
	
	/* write register in lower address */
	reg_word_addr--;
	smi.phy = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
	smi.reg = (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
	smi.value = ((v->value)& 0xffff);

	return smiWrite(MME_SK, ODA, &smi);
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgReadModule
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgReadModule
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], 
	T_MMEAD_RD_MOD_REQ_INFO *mmead_request, T_MMEAD_RD_MOD_ACK_INFO *mmead_comfirm)
{
	int ret = 0;
	//size_t offset = 0;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	
#pragma pack (push,1)
	typedef struct __packed vs_module_operation_read_request 
	{
		header_eth ethernet;
		header_mme intellon;
		uint32_t RESERVED;
		uint8_t NUM_OP_DATA;
		uint16_t MOD_OP;
		uint16_t MOD_OP_DATA_LEN;
		uint32_t MOD_OP_RSVD;
		uint16_t MODULE_ID;
		uint16_t MODULE_SUB_ID;
		uint16_t MODULE_LENGTH;
		uint32_t MODULE_OFFSET;
	}ModOperationReadReqInfo;

	typedef struct __packed vs_module_operation_read_confirm 
	{
		header_eth ethernet;
		header_mme intellon;
		uint16_t MSTATUS;
		uint16_t ERR_REC_CODE;
		uint32_t RESERVED;
		uint8_t NUM_OP_DATA;
		uint16_t MOD_OP;
		uint16_t MOD_OP_DATA_LEN;
		uint32_t MOD_OP_RSVD;
		uint16_t MODULE_ID;
		uint16_t MODULE_SUB_ID;
		uint16_t MODULE_LENGTH;
		uint32_t MODULE_OFFSET;
		uint8_t DATA [INT6K_BLOCKSIZE];
	}ModOperationReadAckInfo;
#pragma pack (pop)

	ModOperationReadReqInfo *request = (ModOperationReadReqInfo *)(buffer);
	ModOperationReadAckInfo *comfirm = (ModOperationReadAckInfo *)(buffer);
	ihpapi_result_t xresult;

	memset(buffer, 0, sizeof(buffer));
	
	EncodeEthernetHeader ((uint8_t *)&(request->ethernet), sizeof(struct header_eth), ODA, OSA);	
	EncodeIntellonHeader ((uint8_t *)&(request->intellon), sizeof(struct header_mme), (VS_MODULE_OPERATION | MMTYPE_REQ));

	request->RESERVED = ihtonl(0);			/* Reserved for future use (must be set to 0) */
	request->NUM_OP_DATA = 1;		/* Reserved for future use (must be set to 1) */
	request->MOD_OP = ihtons(0x01);	/* 01: Read Module from NVM */ 
	request->MOD_OP_DATA_LEN = ihtons(18); 
	request->MOD_OP_RSVD = ihtonl(0);
	request->MODULE_ID = ihtons(mmead_request->MODULE_ID); 
	request->MODULE_SUB_ID = ihtons(mmead_request->MODULE_SUB_ID);
	request->MODULE_LENGTH = ihtons(mmead_request->LENGTH); 
	request->MODULE_OFFSET = ihtonl(mmead_request->OFFSET);

	if( mme_tx(MME_SK, buffer, sizeof(ModOperationReadReqInfo)) <= 0 )
	{
		return CMM_MME_ERROR;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_MODULE_OPERATION, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		ret = intohs(comfirm->MSTATUS);
		if( ret )
		{
			return ret;
		}
		else
		{
			mmead_comfirm->MODULE_ID = intohs(comfirm->MODULE_ID);
			mmead_comfirm->MODULE_SUB_ID = intohs(comfirm->MODULE_SUB_ID);
			mmead_comfirm->LENGTH = (uint32_t)intohs(comfirm->MODULE_LENGTH);
			mmead_comfirm->OFFSET = intohl(comfirm->MODULE_OFFSET);
			memcpy(mmead_comfirm->MODULE_DATA, comfirm->DATA, mmead_comfirm->LENGTH);
			return CMM_SUCCESS;
		}
	}	
	else
	{
		return CMM_MME_ERROR;
	}
}

int MME_Atheros_MsgGetHgManage(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szHgManage *pdata)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("-------->MME_Atheros_MsgGetHgManage\n");
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_GetHgManage(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);
	
	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_HOME_GATEWAY_OPERATION, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		memcpy(pdata, &(xresult.data.hgmanageInfo),sizeof(xresult.data.hgmanageInfo));
		//printf("pdata->wan_name=%s\n",pdata->wan_name);
		return CMM_SUCCESS;
	}	
	return CMM_MME_ERROR;

}

int MME_Atheros_MsgSetHgManage(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szHgManage manage)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("--------->MME_Atheros_MsgSetHgManage\n");
	memset(buffer, 0, sizeof(buffer));

	packetsize = ihpapi_SetHgManage(OSA, ODA, IHPAPI_ETHER_MAX_LEN/2, buffer, manage);

	if(0 != packetsize)
	{
		if(mme_tx(MME_SK, buffer, packetsize) <= 0)
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer, 0, sizeof(buffer));

	if( mme_rx(MME_SK, VS_HOME_GATEWAY_OPERATION, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		return xresult.opStatus.status;
	}

	return CMM_MME_ERROR;
}

int MME_Atheros_MsgGetHgBusiness(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szHgBusiness *pdata)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;
	
	mmead_debug_printf("-------->MME_Atheros_MsgGetHgBusiness\n");
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_GetHgBusiness(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);
		
	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}
	
	memset(buffer,0,sizeof(buffer));
	
	if ( mme_rx(MME_SK, VS_HOME_GATEWAY_OPERATION, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		memcpy(pdata, &(xresult.data.hgbusinessInfo),sizeof(xresult.data.hgbusinessInfo));
		return CMM_SUCCESS;
	}	
	return CMM_MME_ERROR;
}

int MME_Atheros_MsgSetHgBusiness(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szSetHgBusiness manage)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("--------->MME_Atheros_MsgSetHgBusiness\n");
	memset(buffer, 0, sizeof(buffer));

	packetsize = ihpapi_SetHgBusiness(OSA, ODA, IHPAPI_ETHER_MAX_LEN/2, buffer, manage);

	if(0 != packetsize)
	{
		if(mme_tx(MME_SK, buffer, packetsize) <= 0)
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer, 0, sizeof(buffer));

	if( mme_rx(MME_SK, VS_HOME_GATEWAY_OPERATION, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		return xresult.opStatus.status;
	}

	return CMM_MME_ERROR;
}

int MME_Atheros_MsgRebootHg(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[])
{
	int packetsize;
	uint8_t buffer[IHPAPI_ETHER_MIN_LEN];

	mmead_debug_printf("--------->MME_Atheros_MsgRebootHg\n");
	memset(buffer, 0, sizeof(buffer));

	packetsize = ihpapi_RebootHg(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);

	if(0 != packetsize)
	{
		if(mme_tx(MME_SK, buffer, packetsize) <= 0)
		{
			return CMM_MME_ERROR;
		}
		else
		{
			return CMM_SUCCESS;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	return CMM_MME_ERROR;
}

int MME_Atheros_MsgResetHg(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[])
{
	int packetsize;
	uint8_t buffer[IHPAPI_ETHER_MIN_LEN];

	mmead_debug_printf("--------->MME_Atheros_MsgRebootHg\n");
	memset(buffer, 0, sizeof(buffer));

	packetsize = ihpapi_ResetHg(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);

	if(0 != packetsize)
	{
		if(mme_tx(MME_SK, buffer, packetsize) <= 0)
		{
			return CMM_MME_ERROR;
		}
		else
		{
			return CMM_SUCCESS;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	return CMM_MME_ERROR;
}

int MME_Atheros_MsgGetHgSsidStatus(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szHgSsid *pdata)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;
	
	mmead_debug_printf("-------->MME_Atheros_MsgGetHgSsidStatus\n");
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_GetHgSsidStatus(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);
		
	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}
	
	memset(buffer,0,sizeof(buffer));
	
	if ( mme_rx(MME_SK, VS_HOME_GATEWAY_OPERATION, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		memcpy(pdata, &(xresult.data.hgSsidInfo),sizeof(xresult.data.hgSsidInfo));
		return CMM_SUCCESS;
	}	
	return CMM_MME_ERROR;
}

int MME_Atheros_MsgSetHgSsidStatus(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szSetHgSsid ssid)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("--------->MME_Atheros_MsgSetHgSsidStatus\n");
	memset(buffer, 0, sizeof(buffer));

	packetsize = ihpapi_SetHgSsidStatus(OSA, ODA, IHPAPI_ETHER_MAX_LEN/2, buffer, ssid);

	if(0 != packetsize)
	{
		if(mme_tx(MME_SK, buffer, packetsize) <= 0)
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer, 0, sizeof(buffer));

	if( mme_rx(MME_SK, VS_HOME_GATEWAY_OPERATION, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		return xresult.opStatus.status;
	}

	return CMM_MME_ERROR; 
}

int MME_Atheros_MsgGetHgWanStatus(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szHgWanStatus *pdata)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;
	
	mmead_debug_printf("-------->MME_Atheros_MsgGetHgWanStatus\n");
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_GetHgWanStatus(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);
		
	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}
	
	memset(buffer,0,sizeof(buffer));
	
	if ( mme_rx(MME_SK, VS_HOME_GATEWAY_OPERATION, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		memcpy(pdata, &(xresult.data.hgWanInfo),sizeof(xresult.data.hgWanInfo));
		return CMM_SUCCESS;
	}	
	return CMM_MME_ERROR;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetModuleCrc
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetModuleCrc(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint32_t *crc)
{
	int ret = 0;
	uint32_t ModLen = 0;
	//size_t offset = 0;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];

	mmead_debug_printf("-------->MME_Atheros_MsgGetModuleCrc\n");
	
#pragma pack (push,1)
	typedef struct __packed vs_module_operation_read_request 
	{
		header_eth ethernet;
		header_mme intellon;
		uint32_t RESERVED;
		uint8_t NUM_OP_DATA;
		uint16_t MOD_OP;
		uint16_t MOD_OP_DATA_LEN;
		uint32_t MOD_OP_RSVD;
		uint16_t MODULE_ID;
		uint16_t MODULE_SUB_ID;
		uint16_t MODULE_LENGTH;
		uint32_t MODULE_OFFSET;
	}ModOperationReadReqInfo;

	typedef struct __packed vs_module_operation_read_confirm 
	{
		header_eth ethernet;
		header_mme intellon;
		uint16_t MSTATUS;
		uint16_t ERR_REC_CODE;
		uint32_t RESERVED;
		uint8_t NUM_OP_DATA;
		uint16_t MOD_OP;
		uint16_t MOD_OP_DATA_LEN;
		uint32_t MOD_OP_RSVD;
		uint16_t MODULE_ID;
		uint16_t MODULE_SUB_ID;
		uint16_t MODULE_LENGTH;
		uint32_t MODULE_OFFSET;
		uint8_t DATA [INT6K_BLOCKSIZE];
	}ModOperationReadAckInfo;
#pragma pack (pop)

	ModOperationReadReqInfo *request = (ModOperationReadReqInfo *)(buffer);
	ModOperationReadAckInfo *comfirm = (ModOperationReadAckInfo *)(buffer);
	ihpapi_result_t xresult;

	memset(buffer, 0, sizeof(buffer));
	
	EncodeEthernetHeader ((uint8_t *)&(request->ethernet), sizeof(struct header_eth), ODA, OSA);	
	EncodeIntellonHeader ((uint8_t *)&(request->intellon), sizeof(struct header_mme), (VS_MODULE_OPERATION | MMTYPE_REQ));

	request->RESERVED = ihtonl(0);	/* Reserved for future use (must be set to 0) */
	request->NUM_OP_DATA = 1;		/* Reserved for future use (must be set to 1) */
	request->MOD_OP = ihtons(0x01);	/* 01: Read Module from NVM */ 
	request->MOD_OP_DATA_LEN = ihtons(18); 
	request->MOD_OP_RSVD = ihtonl(0);
	request->MODULE_ID = ihtons(0x1000); 
	request->MODULE_SUB_ID = ihtons(0);
	request->MODULE_LENGTH = ihtons(INT6K_BLOCKSIZE); 
	request->MODULE_OFFSET = ihtonl(0);

	if( mme_tx(MME_SK, buffer, sizeof(ModOperationReadReqInfo)) <= 0 )
	{
		return CMM_MME_ERROR;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_MODULE_OPERATION, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	//printf("GetModuleCrc:xresult.validData = %d\n", xresult.validData);
	if(xresult.validData)
	{
		ret = intohs(comfirm->MSTATUS);
		//printf("GetModuleCrc:comfirm->MSTATUS = %d\n", ret);
		if( ret )
		{
			return ret;
		}
		else
		{
			ModLen = (uint32_t)intohs(comfirm->MODULE_LENGTH);			
			*crc = checksum_32_mod((void *)(comfirm->DATA), ModLen);
		
			//printf("MOD CRC = 0x%X\n", crc);
			return CMM_SUCCESS;
		}
	}	
	else
	{
		return CMM_MME_ERROR;
	}
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgWriteModule
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgWriteModule(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[])
{
	FILE *fp;
	uint32_t SessionID = 0x78563412;
	uint8_t MOD_PATH[64] = {0};
	T_MMEAD_WR_MOD_REQ_INFO wr_mod_req = {0};

	gen_mod_file_path(MOD_PATH, ODA);
	
	/* 判断文件是否存在*/
	if( access(MOD_PATH, 0) )
	{
		printf("WriteModule error : file %s does not existed\n", MOD_PATH);
		return CMM_MME_ERROR;
	}
	else if( (fp = fopen(MOD_PATH, "rb")) < 0 )
	{
		return CMM_MME_ERROR;
	}
	else
	{
		fseek(fp, 0L, SEEK_END);
		wr_mod_req.MODULE_LENGTH = ftell(fp);	/* 文件的实际长度*/
		fseek(fp, 0L, SEEK_SET);
		if (wr_mod_req.MODULE_LENGTH > 1400) 
		{
			fclose(fp);
			printf("WriteModule error : MODULE_LENGTH 1\n");
			return CMM_MME_ERROR;
		}
		else if (wr_mod_req.MODULE_LENGTH % sizeof (uint32_t)) 
		{
			fclose(fp);
			printf("WriteModule error : MODULE_LENGTH 2\n");
			return CMM_MME_ERROR;
		}
		else if (fread (wr_mod_req.MODULE_DATA, 1, wr_mod_req.MODULE_LENGTH, fp) < wr_mod_req.MODULE_LENGTH)
		{
			fclose(fp);
			printf("WriteModule error : MODULE_LENGTH 3\n");
			return CMM_MME_ERROR;
		}
		else
		{
			fclose(fp);
		}
	}	

	wr_mod_req.MODULE_ID = 0x1000;
	wr_mod_req.MODULE_SUB_ID = 0;	
	
	if( ModuleSession(MME_SK, ODA, SessionID, &wr_mod_req) )
	{
		/* 失败*/
		return CMM_MME_ERROR;
	}
	if( ModuleWrite(MME_SK, ODA, SessionID, &wr_mod_req) )
	{
		/* 失败*/
		return CMM_MME_ERROR;
	}
	if( ModuleCommit(MME_SK, ODA, SessionID, &wr_mod_req) )
	{
		/* 失败*/
		return CMM_MME_ERROR;
	}
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgDirectWriteModule
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgDirectWriteModule(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint8_t *mod, uint32_t len)
{
	uint32_t SessionID = 0x78563412;
	T_MMEAD_WR_MOD_REQ_INFO wr_mod_req = {0};

	if (len > 1400) 
	{
		printf("WriteModule error : buffer is not enough\n");
		return CMM_MME_ERROR;
	}
	else if (len % sizeof (uint32_t)) 
	{
		printf("WriteModule error : mod length invalid\n");
		return CMM_MME_ERROR;
	}

	wr_mod_req.MODULE_ID = 0x1000;
	wr_mod_req.MODULE_SUB_ID = 0;
	wr_mod_req.MODULE_LENGTH = len;	
	memcpy(wr_mod_req.MODULE_DATA, mod, len);			
	
	if( ModuleSession(MME_SK, ODA, SessionID, &wr_mod_req) )
	{
		/* 失败*/
		return CMM_MME_ERROR;
	}
	if( ModuleWrite(MME_SK, ODA, SessionID, &wr_mod_req) )
	{
		/* 失败*/
		return CMM_MME_ERROR;
	}
	if( ModuleCommit(MME_SK, ODA, SessionID, &wr_mod_req) )
	{
		/* 失败*/
		return CMM_MME_ERROR;
	}
	
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgEraseModule
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgEraseModule(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_ERASE_MOD_REQ_INFO *erase)
{
	uint32_t SessionID = 0x78563412;
	T_MMEAD_WR_MOD_REQ_INFO wr_mod_req = {0};

	wr_mod_req.MODULE_ID = erase->MODULE_ID;
	wr_mod_req.MODULE_SUB_ID = erase->MODULE_SUB_ID;
	wr_mod_req.MODULE_LENGTH = 4;
	bzero(wr_mod_req.MODULE_DATA, 1400);		
	
	if( ModuleSession(MME_SK, ODA, SessionID, &wr_mod_req) )
	{
		/* 失败*/
		return CMM_MME_ERROR;
	}
	if( ModuleWrite(MME_SK, ODA, SessionID, &wr_mod_req) )
	{
		/* 失败*/
		return CMM_MME_ERROR;
	}
	if( ModuleCommit(MME_SK, ODA, SessionID, &wr_mod_req) )
	{
		/* 失败*/
		return CMM_MME_ERROR;
	}
	
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetPibCrc
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetPibCrc(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint32_t *crc)
{
	int ret = 0;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	
#pragma pack (push,1)
	typedef struct __packed vs_rd_mod_request 
	{
		header_eth ethernet;
		header_mme intellon;
		uint8_t MODULEID;
		uint8_t RESERVED;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
	}ModReadReqInfo;

	typedef struct __packed vs_rd_mod_confirm 
	{
		header_eth ethernet;
		header_mme intellon;
		uint8_t MSTATUS;
		uint8_t RESERVED1 [3];
		uint8_t MODULEID;
		uint8_t RESERVED;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
		uint32_t CHKSUM;
		uint8_t BUFFER [INT6K_BLOCKSIZE];
	}ModReadAckInfo;
#pragma pack (pop)

	ModReadReqInfo *request = (ModReadReqInfo *)(buffer);
	ModReadAckInfo *comfirm = (ModReadAckInfo *)(buffer);
	ihpapi_result_t xresult;

	memset(buffer, 0, sizeof(buffer));
	
	EncodeEthernetHeader ((uint8_t *)&(request->ethernet), sizeof(struct header_eth), ODA, OSA);	
	EncodeIntellonHeader ((uint8_t *)&(request->intellon), sizeof(struct header_mme), (VS_RD_MOD | MMTYPE_REQ));

	request->MODULEID = VS_MODULE_PIB;
	request->RESERVED = 0;
	request->MLENGTH = ihtons(12);	/* PIB Version and Header len */
	request->MOFFSET = ihtonl(0);

	if( mme_tx(MME_SK, buffer, sizeof(ModReadReqInfo)) <= 0 )
	{
		return CMM_MME_ERROR;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_RD_MOD, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		ret = comfirm->MSTATUS;
		if( ret )
		{
			return ret;
		}
		else
		{
			PIBVersionHeader *pibhdr = (PIBVersionHeader *)(comfirm->BUFFER);
			*crc = pibhdr->Checksum32;
			return CMM_SUCCESS;
		}
	}	
	else
	{
		return CMM_MME_ERROR;
	}
}

int MME_Atheros_MsgGetPibLen(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint32_t *len)
{
	int ret = 0;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];

	mmead_debug_printf("-------->MME_Atheros_MsgGetPibLen\n");
	
#pragma pack (push,1)
	typedef struct __packed vs_rd_mod_request 
	{
		header_eth ethernet;
		header_mme intellon;
		uint8_t MODULEID;
		uint8_t RESERVED;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
	}ModReadReqInfo;

	typedef struct __packed vs_rd_mod_confirm 
	{
		header_eth ethernet;
		header_mme intellon;
		uint8_t MSTATUS;
		uint8_t RESERVED1 [3];
		uint8_t MODULEID;
		uint8_t RESERVED;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
		uint32_t CHKSUM;
		uint8_t BUFFER [INT6K_BLOCKSIZE];
	}ModReadAckInfo;
#pragma pack (pop)

	ModReadReqInfo *request = (ModReadReqInfo *)(buffer);
	ModReadAckInfo *comfirm = (ModReadAckInfo *)(buffer);
	ihpapi_result_t xresult;

	memset(buffer, 0, sizeof(buffer));
	
	EncodeEthernetHeader ((uint8_t *)&(request->ethernet), sizeof(struct header_eth), ODA, OSA);	
	EncodeIntellonHeader ((uint8_t *)&(request->intellon), sizeof(struct header_mme), (VS_RD_MOD | MMTYPE_REQ));

	request->MODULEID = VS_MODULE_PIB;
	request->RESERVED = 0;
	request->MLENGTH = ihtons(12);	/* PIB Version and Header len */
	request->MOFFSET = ihtonl(0);

	if( mme_tx(MME_SK, buffer, sizeof(ModReadReqInfo)) <= 0 )
	{
		return CMM_MME_ERROR;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_RD_MOD, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		ret = comfirm->MSTATUS;
		if( ret )
		{
			return ret;
		}
		else
		{
			PIBVersionHeader *pibhdr = (PIBVersionHeader *)(comfirm->BUFFER);
			*len = intohs(pibhdr->PIBLength);
			return CMM_SUCCESS;
		}
	}	
	else
	{
		return CMM_MME_ERROR;
	}
}

int MME_Atheros_MsgGetPibByOffset(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], char *pdata, uint32_t offset, uint32_t len)
{
	int ret = 0;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];

	mmead_debug_printf("-------->MME_Atheros_MsgGetPibByOffset\n");
	
#pragma pack (push,1)
	typedef struct __packed vs_rd_mod_request 
	{
		header_eth ethernet;
		header_mme intellon;
		uint8_t MODULEID;
		uint8_t RESERVED;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
	}ModReadReqInfo;

	typedef struct __packed vs_rd_mod_confirm 
	{
		header_eth ethernet;
		header_mme intellon;
		uint8_t MSTATUS;
		uint8_t RESERVED1 [3];
		uint8_t MODULEID;
		uint8_t RESERVED;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
		uint32_t CHKSUM;
		uint8_t BUFFER [INT6K_BLOCKSIZE];
	}ModReadAckInfo;
#pragma pack (pop)

	ModReadReqInfo *request = (ModReadReqInfo *)(buffer);
	ModReadAckInfo *comfirm = (ModReadAckInfo *)(buffer);
	ihpapi_result_t xresult;

	memset(buffer, 0, sizeof(buffer));
	
	EncodeEthernetHeader ((uint8_t *)&(request->ethernet), sizeof(struct header_eth), ODA, OSA);	
	EncodeIntellonHeader ((uint8_t *)&(request->intellon), sizeof(struct header_mme), (VS_RD_MOD | MMTYPE_REQ));

	request->MODULEID = VS_MODULE_PIB;
	request->RESERVED = 0;
	request->MLENGTH = ihtons(len);
	request->MOFFSET = ihtonl(offset);

	if( mme_tx(MME_SK, buffer, sizeof(ModReadReqInfo)) <= 0 )
	{
		perror("\n1\n");
		return CMM_MME_ERROR;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_RD_MOD, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		perror("\n2\n");
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		ret = comfirm->MSTATUS;
		if( ret )
		{
			perror("\n3\n");
			return ret;
		}
		else if (intohs(comfirm->MLENGTH) != len) 
		{
			perror("\n4\n");
			return CMM_MME_ERROR;
		}
		else if (intohl(comfirm->MOFFSET) != offset) 
		{
			perror("\n5\n");
			return CMM_MME_ERROR;
		}
		else if (checksum_32_pib (comfirm->BUFFER, len, (comfirm->CHKSUM))) 
		{
			perror("\n6\n");
			return CMM_MME_ERROR;
		}
		else
		{
			memcpy(pdata, comfirm->BUFFER, len);
			return CMM_SUCCESS;
		}
	}	
	else
	{
		perror("\n7\n");
		return CMM_MME_ERROR;
	}
}

int MME_AR7411_MsgGetPibCrc(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint32_t *crc)
{
	int ret = 0;
	uint32_t pib_len = 0;

	uint32_t offset = 0;
	uint32_t length = INT6K_BLOCKSIZE;
	
	char *pib_data = NULL;

	mmead_debug_printf("-------->MME_AR7411_MsgGetPibCrc\n");
	
	if( CMM_SUCCESS != MME_Atheros_MsgGetPibLen(MME_SK, ODA, &pib_len) )
	{
		return CMM_MME_ERROR;
	}

	pib_data = (char *)malloc(pib_len);
	if( NULL == pib_data )
	{
		return CMM_MME_ERROR;
	}

	do
	{
		if ((offset + length) > pib_len) 
		{
			length = pib_len - offset;
		}
		if( CMM_SUCCESS != MME_Atheros_MsgGetPibByOffset(MME_SK, ODA, pib_data+offset, offset, length))
		{
			free(pib_data);
			return CMM_MME_ERROR;
		}
		offset += length;
	}
	while (offset < pib_len);

	/* jisuan */
	memset(pib_data+0x08, 0x00, 4);
	*crc = checksum_32_pib(pib_data, pib_len, 0);
	free(pib_data);
	return CMM_SUCCESS;
}

int MME_Atheros_MsgReadPib(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[])
{
	int ret = 0;
	uint32_t pib_len = 0;

	uint32_t offset = 0;
	uint32_t length = INT6K_BLOCKSIZE;
	
	char *pib_data = NULL;
	uint8_t PIB_PATH[64] = {0};
	FILE * fp = NULL;

	mmead_debug_printf("-------->MME_Atheros_MsgReadPib\n");
	
	if( CMM_SUCCESS != MME_Atheros_MsgGetPibLen(MME_SK, ODA, &pib_len) )
	{
		return CMM_MME_ERROR;
	}

	pib_data = (char *)malloc(pib_len);
	if( NULL == pib_data )
	{
		return CMM_MME_ERROR;
	}

	do
	{
		if ((offset + length) > pib_len) 
		{
			length = pib_len - offset;
		}
		if( CMM_SUCCESS != MME_Atheros_MsgGetPibByOffset(MME_SK, ODA, pib_data+offset, offset, length))
		{
			free(pib_data);
			return CMM_MME_ERROR;
		}
		offset += length;
	}
	while (offset < pib_len);

	/* jisuan */
	gen_pib_file_path(PIB_PATH, ODA);
	
	if( (fp = fopen(PIB_PATH, "wb")) < 0 )
	{
		free(pib_data);
		return CMM_MME_ERROR;
	}
	else
	{
		if(fwrite(pib_data, pib_len, 1, fp) != 1)
		{
			free(pib_data);
			fclose(fp);
			return CMM_MME_ERROR;
		}
		fclose(fp);
	}
	free(pib_data);
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgWritePib
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgWritePib(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[])
{
	mmead_debug_printf("-------->MME_Atheros_MsgWritePib\n");
	if (WritePIB (MME_SK, ODA)) 
	{
		printf("@@@@ WritePIB error @@@@\n");
		return CMM_MME_ERROR;
	}
	if (FlashDevice (MME_SK, ODA)) 
	{
		printf("@@@@ FlashDevice error @@@@\n");
		return CMM_MME_ERROR;
	}
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetPibSpec
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetPibSpec
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_DEV_INFO *pDevInfo)
{
	int ret = 0;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];

	mmead_debug_printf("-------->MME_Atheros_MsgGetPibSpec\n");	
#pragma pack (push,1)
	typedef struct __packed vs_rd_mod_request 
	{
		header_eth ethernet;
		header_mme intellon;
		uint8_t MODULEID;
		uint8_t RESERVED;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
	}ModReadReqInfo;

	typedef struct __packed vs_rd_mod_confirm 
	{
		header_eth ethernet;
		header_mme intellon;
		uint8_t MSTATUS;
		uint8_t RESERVED1 [3];
		uint8_t MODULEID;
		uint8_t RESERVED;
		uint16_t MLENGTH;
		uint32_t MOFFSET;
		uint32_t CHKSUM;
		uint8_t BUFFER [INT6K_BLOCKSIZE];
	}ModReadAckInfo;
#pragma pack (pop)

	ModReadReqInfo *request = (ModReadReqInfo *)(buffer);
	ModReadAckInfo *comfirm = (ModReadAckInfo *)(buffer);
	ihpapi_result_t xresult;

	memset(buffer, 0, sizeof(buffer));
	EncodeEthernetHeader ((uint8_t *)&(request->ethernet), sizeof(struct header_eth), ODA, OSA);	
	EncodeIntellonHeader ((uint8_t *)&(request->intellon), sizeof(struct header_mme), (VS_RD_MOD | MMTYPE_REQ));

	request->MODULEID = VS_MODULE_PIB;
	request->RESERVED = 0;
	request->MLENGTH = ihtons(1024);	/* PIB Version and Header  + Local Device Configuration len */
	request->MOFFSET = ihtonl(0);

	if( mme_tx(MME_SK, buffer, sizeof(ModReadReqInfo)) <= 0 )
	{
		return CMM_MME_ERROR;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_RD_MOD, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	//printf("xresult.validData = %d\n", xresult.validData);
	if(xresult.validData)
	{
		ret = comfirm->MSTATUS;
		//printf("comfirm->MSTATUS = %d\n", ret);
		if( ret )
		{
			return ret;
		}
		else
		{
			PIBVersionHeader *pibhdr = (PIBVersionHeader *)(comfirm->BUFFER);
			uint8_t *HFID_USER = (comfirm->BUFFER) + 0x0074;			
			
			pDevInfo->CRC[0] = pibhdr->Checksum32;
			if( strcmp(HFID_USER, "WEC-3801I") == 0 )
			{
				pDevInfo->DevType = WEC_3801I;
			}
			else if( strcmp(HFID_USER, "WEC701-M0") == 0 )
			{
				pDevInfo->DevType = WEC701_M0;
			}
#if 0
			else if( strcmp(HFID_USER, "WEC-3601I") == 0 )
			{
				pDevInfo->DevType = WEC_3601I;
			}
			else if( strcmp(HFID_USER, "WEC-3602I") == 0 )
			{
				pDevInfo->DevType = WEC_3602I;
			}
			else if( strcmp(HFID_USER, "WEC-3604I") == 0 )
			{
				pDevInfo->DevType = WEC_3604I;
			}
			else if( strcmp(HFID_USER, "WEC-3703I") == 0 )
			{
				pDevInfo->DevType = WEC_3703I;
			}
#endif
			else if( strcmp(HFID_USER, "WEC-3702I") == 0 )
			{
				pDevInfo->DevType = WEC_3702I;
			}			
			else if( strcmp(HFID_USER, "WEC-602") == 0 )
			{
				pDevInfo->DevType = WEC_602;
			}
			else if( strcmp(HFID_USER, "WEC-604") == 0 )
			{
				pDevInfo->DevType = WEC_604;
			}			
			else if( strcmp(HFID_USER, "WEC701-C2") == 0 )
			{
				pDevInfo->DevType = WEC701_C2;
			}
			else if( strcmp(HFID_USER, "WEC701-C4") == 0 )
			{
				pDevInfo->DevType = WEC701_C4;
			}
			else if( strcmp(HFID_USER, "WEC701-E4") == 0 )
			{
				pDevInfo->DevType = WEC701_E4;
			}
			else if( strcmp(HFID_USER, "WEC-3702I-E4") == 0 )
			{
				pDevInfo->DevType = WEC_3702I_E4;
			}
			else if( strcmp(HFID_USER, "WEC701-L4") == 0)
			{
				pDevInfo->DevType = WEC701_L4;
			}
			else if( strcmp(HFID_USER, "WEC701-W4") == 0)
			{
				pDevInfo->DevType = WEC701_W4;
			}
			else if( strcmp(HFID_USER, "S744_8306RA") == 0 || strcmp(HFID_USER, "WEC701W C4") == 0)
			{
				pDevInfo->DevType = WEC701W_C4;
			}
			else
			{
				/* 非法设备*/
				pDevInfo->DevType = WEC_INVALID;
			}

			//printf("HFID_USER = %s, PIB CRC = 0x%X\n", HFID_USER, pibhdr->Checksum32);
			
			return CMM_SUCCESS;
		}
	}	
	else
	{
		return CMM_MME_ERROR;
	}
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetDeviceInfo
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetDeviceInfo
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_DEV_INFO *pDevInfo)
{
	uint32_t modCrc = 0;
	mmead_debug_printf("-------->MME_Atheros_MsgGetDeviceInfo\n");

	/* 获取设备类型和PIB CRC */	
	if( MME_Atheros_MsgGetPibSpec(MME_SK, ODA, pDevInfo) )
	{
		return CMM_MME_ERROR;	
	}
	#if 0
	if( (WEC701_M0 == pDevInfo->DevType)||(WEC701_C2 == pDevInfo->DevType)||(WEC701_C4 == pDevInfo->DevType) )
	{
		 if( CMM_SUCCESS != MME_AR7411_MsgGetPibCrc(MME_SK, ODA, &pDevInfo->CRC[0]) )
		 {
		 	perror("\nERROR: MME_Atheros_MsgGetDeviceInfo->MME_AR7411_MsgGetPibCrc()\n");
			return CMM_MME_ERROR;	
		 }
	}
	#endif
	#if 0
	switch(pDevInfo->DevType)
	{
		case WEC_3702I:
		case WEC_3703I:
		case WEC_602:
		case WEC_604:
		{
			if( MME_Atheros_MsgGetModuleCrc(MME_SK, ODA, &modCrc) )
			{
				modCrc = 0;
			}
			break;
		}
		default:
		{
			modCrc = 0;
			break;
		}
	}
	#endif
	pDevInfo->CRC[1] = modCrc;
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgEnableDev
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgEnableDev(T_MME_SK_HANDLE *MME_SK, uint8_t devType, uint8_t ODA[])
{
	mmead_debug_printf("-------->MME_Atheros_MsgEnableDev\n");
	T_szMdioSw smi;
	smi.reg = 0x104;
	smi.value = 0x4004;
	switch(devType)
	{
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_C2:
		case WEC701_C4:
		{
		
			if( CMM_SUCCESS != MME_Atheros_MsgSetSwReg(MME_SK, ODA, &smi) )
			{
				return CMM_FAILED;
			}
			return CMM_SUCCESS;
		}
		case WEC_3602I:
		{			
			return CMM_SUCCESS;
		}
		default:
		{
			return CMM_UNKNOWN_DEVTYPE;	
		}
	}
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgDisableDev
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgDisableDev(T_MME_SK_HANDLE *MME_SK, uint8_t devType, uint8_t ODA[])
{
	mmead_debug_printf("-------->MME_Atheros_MsgDisableDev\n");
	T_szMdioSw smi;
	smi.reg = 0x104;
	smi.value = 0x4000;
	switch(devType)
	{
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_C2:
		case WEC701_C4:
		{
			//printf("MME_Atheros_MsgDisableDev [devType = %d, mac = %02X:%02X:%02X:%02X:%02X:%02X]\n", 
			//	devType, ODA[0], ODA[1], ODA[2], ODA[3], ODA[4], ODA[5]);
			
			if( CMM_SUCCESS != MME_Atheros_MsgSetSwReg(MME_SK, ODA, &smi) )
			{
				return CMM_FAILED;
			}
			return CMM_SUCCESS;
		}
		case WEC_3602I:
		{
			//printf("MME_Atheros_MsgDisableDev [devType = %d, mac = %02X:%02X:%02X:%02X:%02X:%02X]\n", 
			//	devType, ODA[0], ODA[1], ODA[2], ODA[3], ODA[4], ODA[5]);
			
			return CMM_SUCCESS;
		}
		default:
		{
			return CMM_UNKNOWN_DEVTYPE;	
		}
	}
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgDisableDev
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgBootOutDev(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint8_t DES[])
{
#if 1
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;	
	
	mmead_debug_printf("-------->MME_Atheros_MsgBootOutDev\n");

#pragma pack (push,1)
	typedef struct __packed vs_slave_mem_req
	{
		header_eth ethernet;
		header_mme intellon;
		uint8_t SLAVE_ADDR[6];
		uint8_t AUTHORIZATION_MODE;
		uint8_t ASSOC_RETRY_TIME;
	}slaveMemReqInfo;

	typedef struct __packed vs_slave_mem_ack 
	{
		header_eth ethernet;
		header_mme intellon;
		uint8_t MSTATUS;
	}slaveMemAckInfo;
#pragma pack (pop)

	slaveMemReqInfo *request = (slaveMemReqInfo *)(buffer);
	slaveMemAckInfo *confirm = (slaveMemAckInfo *)(buffer);

	memset(buffer, 0, sizeof(buffer));
	
	EncodeEthernetHeader ((uint8_t *)&(request->ethernet), sizeof(struct header_eth), ODA, OSA);	
	EncodeIntellonHeader ((uint8_t *)&(request->intellon), sizeof(struct header_mme), (VS_SLAVE_MEM | MMTYPE_REQ));

	memcpy(request->SLAVE_ADDR, DES, 6);	/* Slave MAC Address */
	request->AUTHORIZATION_MODE = 0x01;	/* Eject slave from AVLN */
	request->ASSOC_RETRY_TIME = 0x0f;	/* Never retry */

	if( mme_tx(MME_SK, buffer, sizeof(slaveMemReqInfo)) <= 0 )
	{
		return CMM_MME_ERROR;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_SLAVE_MEM, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		return confirm->MSTATUS;
	}
	else
	{
		return CMM_MME_ERROR;
	}
#else
	return CMM_SUCCESS;
#endif
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetRxToneMapInfo
*	函数功能:
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetRxToneMapInfo
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	ihpapi_toneMapCtl_t * inputToneMapInfo, 
	ihpapi_getRxToneMapData_t *outputToneMapInfo
)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("-------->MME_Atheros_MsgGetRxToneMapInfo\n");
	
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_GetRxToneMapInfo(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer, inputToneMapInfo);

	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer, 0, sizeof(buffer));

	if ( mme_rx(MME_SK, VS_RX_TONE_MAP_CHAR, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		memcpy(outputToneMapInfo, &xresult.data.rxToneMap, sizeof(ihpapi_getRxToneMapData_t));
		return xresult.opStatus.status;
	}	
	return CMM_MME_ERROR;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGet74RxToneMapInfo
*	函数功能:
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGet74RxToneMapInfo
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	ihpapi_toneMapCtl_t * inputToneMapInfo, 
	ihpapi_getRxToneMapData_t *outputToneMapInfo
)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("-------->MME_Atheros_MsgGetRxToneMapInfo\n");
	
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_Get74RxToneMapInfo(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer, inputToneMapInfo);

	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer, 0, sizeof(buffer));

	if ( mme_v1_rx(MME_SK, VS_RX_TONE_MAP_CHAR, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		memcpy(outputToneMapInfo, &xresult.data.rxToneMap, sizeof(ihpapi_getRxToneMapData_t));
		return xresult.opStatus.status;
	}	
	return CMM_MME_ERROR;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetTxToneMapInfo
*	函数功能:
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetTxToneMapInfo
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	ihpapi_toneMapCtl_t * inputToneMapInfo, 
	ihpapi_getToneMapData_t *outputToneMapInfo
)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("-------->MME_Atheros_MsgGetTxToneMapInfo\n");
	
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_GetToneMapInfo(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer, inputToneMapInfo);

	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer, 0, sizeof(buffer));

	if ( mme_rx(MME_SK, VS_TONE_MAP_CHAR, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		memcpy(outputToneMapInfo, &xresult.data.toneMap, sizeof(ihpapi_getToneMapData_t));
		return xresult.opStatus.status;
	}	
	return CMM_MME_ERROR;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGet74TxToneMapInfo
*	函数功能:
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGet74TxToneMapInfo
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	ihpapi_toneMapCtl_t * inputToneMapInfo, 
	ihpapi_getToneMapData_t *outputToneMapInfo
)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("-------->MME_Atheros_MsgGet74TxToneMapInfo\n");
	
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_Get74ToneMapInfo(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer, inputToneMapInfo);
	
	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer, 0, sizeof(buffer));

	if ( mme_v1_rx(MME_SK, VS_TONE_MAP_CHAR, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		memcpy(outputToneMapInfo, &xresult.data.toneMap, sizeof(ihpapi_getToneMapData_t));
		return xresult.opStatus.status;
	}
	return CMM_MME_ERROR;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetConnectionInfo
*	函数功能:
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetConnectionInfo
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	ihpapi_connectCtl_t * inputConnectInfo, 
	ihpapi_getConnectInfoData_t *outputConnectInfo
)
{
	int packetsize;
	int recv_msg_len = 0;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	ihpapi_result_t xresult;

	mmead_debug_printf("-------->MME_Atheros_MsgGetConnectionInfo\n");
	
	memset(buffer, 0, sizeof(buffer));
	packetsize = ihpapi_GetConnectionInfo(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer, inputConnectInfo);

	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer, 0, sizeof(buffer));

	if ( mme_rx(MME_SK, VS_LNK_STATS, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS)
	{
		return CMM_MME_ERROR;
	}
	if(xresult.validData)
	{
		memcpy(outputConnectInfo, &xresult.data.connInfo, sizeof(ihpapi_getConnectInfoData_t));
		return xresult.opStatus.status;
	}	
	return CMM_MME_ERROR;
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetNetInfo
*	函数功能:ihpapi_GetNetworkInfo
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetNetInfo
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	ihpapi_getNetworkInfoData_t *outputNetInfo
)
{
	int packetsize;
	int recv_msg_len = 0;
	ihpapi_result_t xresult;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	
	assert( NULL != outputNetInfo );

	mmead_debug_printf("-------->MME_Atheros_MsgGetNetInfo\n");
	
	memset(outputNetInfo, 0, sizeof(ihpapi_getNetworkInfoData_t));
	memset(buffer, 0, IHPAPI_ETHER_MAX_LEN);
	
	packetsize = ihpapi_GetNetworkInfo(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);
	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_rx(MME_SK, VS_NW_INFO, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS )
	{
		return CMM_MME_ERROR;
	}
	else if( xresult.validData )
	{
		memcpy(outputNetInfo, &xresult.data.netInfo, sizeof(ihpapi_getNetworkInfoData_t));
		return xresult.opStatus.status;		
	}
	else
	{
		return CMM_MME_ERROR;
	}
}

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGet74NetInfo
*	函数功能:ihpapi_GetNetworkInfo
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGet74NetInfo
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	ihpapi_getNetworkInfoData_t *outputNetInfo
)
{
	int packetsize;
	int recv_msg_len = 0;
	ihpapi_result_t xresult;
	uint8_t buffer[IHPAPI_ETHER_MAX_LEN];
	
	assert( NULL != outputNetInfo );

	mmead_debug_printf("-------->MME_Atheros_MsgGetNetInfo\n");
	
	memset(outputNetInfo, 0, sizeof(ihpapi_getNetworkInfoData_t));
	memset(buffer, 0, IHPAPI_ETHER_MAX_LEN);
	
	packetsize = ihpapi_Get74NetworkInfo(OSA, ODA, IHPAPI_ETHER_MIN_LEN, buffer);
	if( 0 != packetsize )
	{
		if( mme_tx(MME_SK, buffer, packetsize) <= 0 )
		{
			return CMM_MME_ERROR;
		}
	}
	else
	{
		return CMM_FAILED;
	}

	memset(buffer,0,sizeof(buffer));

	if ( mme_v1_rx(MME_SK, VS_NW_INFO, buffer, sizeof(buffer), &recv_msg_len, &xresult) != CMM_SUCCESS )
	{
		printf("mme_v1_rx error\n");
		return CMM_MME_ERROR;
	}
	else if( xresult.validData )
	{
		memcpy(outputNetInfo, &xresult.data.netInfo, sizeof(ihpapi_getNetworkInfoData_t));
		return xresult.opStatus.status;		
	}
	else
	{
		return CMM_MME_ERROR;
	}
}


