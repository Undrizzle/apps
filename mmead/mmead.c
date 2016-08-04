/*****************************************************************************************
  文件名称 : mmead.c
  文件描述 : MME适配模块主入口函数
  				该模块处理来自系统内部其他模块的配置
  				请求，不涉及网络字节序与主机字节序的
  				转换
  修订记录 :
           1 创建 : frank
             日期 : 2010-07-02
             描述 : 创建文件

 *****************************************************************************************/

#include <assert.h>
#include <signal.h>
#include "mmead.h"
#include "mmeapi.h"
#include "mme_rtl8306e.h"
#include "support/atheros/ihpapi/ihpapi.h"
#include "support/atheros/ihpapi/ihp.h"
#include "../dbs/include/dbsapi.h"
#include <boardapi.h>

uint8_t OSA [6] = 
{
	0x00,
	0xb0,
	0x52,
	0x00,
	0x00,
	0x01 
};

static MMEAD_BBLOCK_QUEUE bblock;
int MMEAD_MODULE_DEBUG_ENABLE = 1;

/* 与DBS  通讯的设备文件*/
T_DBS_DEV_INFO *dbsdev = NULL;

void hexdump (const unsigned char memory [], size_t length, FILE *fp) ;
void MME_Atheros_ProcessGetTopology(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK);
void MME_Atheros_ProcessGetTopologyStats(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK);
int MME_Atheros_MsgGetPhyReg(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szMdioPhy *v);
int MME_Atheros_MsgSetPhyReg(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szMdioPhy *v);
int MME_Atheros_MsgGetSwReg(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szMdioSw *v);
int MME_Atheros_MsgSetSwReg(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_szMdioSw *v);
int MME_Atheros_MsgReadModule
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], 
	T_MMEAD_RD_MOD_REQ_INFO *mmead_request, T_MMEAD_RD_MOD_ACK_INFO *mmead_comfirm);
int MME_Atheros_MsgWriteModule(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[]);
int MME_Atheros_MsgGetModuleCrc(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint32_t *crc);
int MME_Atheros_MsgGetPibCrc(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint32_t *crc);
int MME_Atheros_MsgWritePib(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[]);
int MME_Atheros_MsgEnableDev(T_MME_SK_HANDLE *MME_SK, uint8_t devType, uint8_t ODA[]);
int MME_Atheros_MsgDisableDev(T_MME_SK_HANDLE *MME_SK, uint8_t devType, uint8_t ODA[]);
int MME_Atheros_MsgBootOutDev(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint8_t DES[]);

int __mapModCarrier(char value)
{
	switch(value)
	{
		case 1:
		{
			return 1;
		}
		case 2:
		{
			return 2;
		}
		case 3:
		{
			return 3;
		}
		case 4:
		{
			return 4;
		}
		case 5:
		{
			return 6;
		}
		case 6:
		{
			return 8;
		}
		case 7:
		{
			return 10;
		}
		case 8:
		{
			return 12;
		}
		default:
		{
			return 0;
		}
	}
}

#if 0
/********************************************************************************************
*	函数名称:get_mac_addr
*	函数功能:该函数通过系统调用获取本机的MAC地址
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
static int get_mac_addr(char * interface, uint8_t *pMacAddr)
{
	int fd = 0;
	struct ifreq req;
	struct  sockaddr *phwaddr;

	if( interface == NULL )
	{
		return -1;
	}
	if( pMacAddr == NULL )
	{
		return -1;
	}
	
	strcpy( req.ifr_name, interface );
	fd = socket( PF_INET, SOCK_RAW, IPPROTO_ICMP );
	if( fd < 0 )
	{
		perror("FUNC __get_mac_addr() : socket error !\n");
		return -1;
	}
	phwaddr = &req.ifr_hwaddr;
	if( ioctl( fd, SIOCGIFHWADDR, &req ) == 0 )
	{
		memcpy((char *)pMacAddr, (char *)phwaddr->sa_data, 6);
	}
	else
	{
		perror("FUNC __get_mac_addr() : ioctl error !\n");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}
#endif
void debug_dump_msg(const unsigned char memory [], size_t length, FILE *fp)
{
	if(MMEAD_MODULE_DEBUG_ENABLE)
	{
		mmead_debug_printf("-----------------------------------------------------------------------\n");
		hexdump(memory, length, fp);
		mmead_debug_printf("\n-----------------------------------------------------------------------\n");
	}
}

/********************************************************************************************
*	函数名称:MMEAD_ProcessAck
*	函数功能:该函数为MMEAD外部应答函数，MMEAD请求处理
*				  完成之后通过该函数将处理结果返回给请求者
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
void MMEAD_ProcessAck(uint16_t status, MMEAD_BBLOCK_QUEUE *this, uint8_t *b, uint32_t len)
{
	assert(NULL != this);
	
	T_REQ_Msg_Header_MMEAD rh;
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	uint8_t buf[MAX_UDP_SIZE];
	uint16_t result = status;
	int sendn = 0;

	bzero(buf, sizeof(buf));
	
	rh.M_TYPE = h->M_TYPE;
	rh.DEV_TYPE = h->DEV_TYPE;
	rh.MM_TYPE = h->MM_TYPE;
	rh.fragment = 0;
	rh.LEN = (len + sizeof(uint16_t));

	memcpy(buf, &rh, sizeof(rh));
	memcpy(buf+sizeof(rh), &result, sizeof(uint16_t));

	if( 0 != len )
	{
		assert(NULL != b);
		memcpy(buf+sizeof(rh)+sizeof(uint16_t), b, len);
	}
	
	/* 打印接收的数据报文*/
	mmead_debug_printf("\n<==== MMEAD SEND MASSAGE:\n");
	debug_dump_msg(buf, sizeof(rh)+sizeof(uint16_t)+len, stderr);
	
	/* 将处理信息发送给请求者 */
	sendn = sendto(this->sk.skfd, buf, sizeof(rh)+sizeof(uint16_t)+len, 0, 
				(struct sockaddr *)&(this->sk.from), sizeof(this->sk.from));
}

/********************************************************************************************
*	函数名称:MME_ProcessModuleDebugEnable
*	函数功能:通过调用该函数打开模块的调试消息，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessModuleDebugEnable(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	MMEAD_MODULE_DEBUG_ENABLE = 1;
	/* 将错误信息返回给请求者 */
	MMEAD_ProcessAck(CMM_SUCCESS, this, NULL, 0);
}

/********************************************************************************************
*	函数名称:MME_ProcessModuleDebugDisable
*	函数功能:通过调用该函数打开模块的调试消息，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessModuleDebugDisable(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	MMEAD_MODULE_DEBUG_ENABLE = 0;
	/* 将错误信息返回给请求者 */
	MMEAD_ProcessAck(CMM_SUCCESS, this, NULL, 0);
}

/********************************************************************************************
*	函数名称:MME_Atheros_ProcessGetCltMac
*	函数功能:该函数为MME_ProcessGetCltMac函数完成硬件屏蔽之后
*				  调用的子函数，
*				  通过发送MME获取CLT的MAC地址，并将获取
*				  到的信息通过消息接口发送给请求模块
*	限制:这里假设只有一个CLT线卡，否则该函数的逻辑需要
*			修改
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_Atheros_ProcessGetCltMac(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	uint8_t clt_mac[IHPAPI_ETHER_ADDR_LEN];
	
	/* 向设备发送MME *//* 等待设备回应*/
	/* 将处理信息发送给请求者 */
	MMEAD_ProcessAck(MME_Atheros_MsgGetCltMac(MME_SK, clt_mac), 
						this, clt_mac, IHPAPI_ETHER_ADDR_LEN);
}

/********************************************************************************************
*	函数名称:MME_Spc_ProcessGetCltMac
*	函数功能:该函数为MME_ProcessGetCltMac函数完成硬件屏蔽之后
*				  调用的子函数，
*				  通过发送MME获取CLT的MAC地址，并将获取
*				  到的信息通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_Spc_ProcessGetCltMac(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	
	printf("MME_Spc_ProcessGetCltMac\n");
	/* 代码缺失 */

	/* 将错误信息返回给请求者 */
	MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
}

/********************************************************************************************
*	函数名称:MME_Atheros_ProcessResetDevice
*	函数功能:该函数为MME_ProcessResetDevice函数完成硬件屏蔽之后
*				  调用的子函数，
*				  该函数通过发送MME完成CLT/CNU的复位操作，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_Atheros_ProcessResetDevice(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	/* 向设备发送MME *//* 等待设备回应*/
	/* 将处理信息发送给请求者 */
	MMEAD_ProcessAck(MME_Atheros_MsgResetDevice(MME_SK, h->ODA), this, NULL, 0);
}

/********************************************************************************************
*	函数名称:MME_Atheros_ProcessGetSwVer
*	函数功能:该函数为MME_ProcessGetSwVer函数完成硬件屏蔽之后
*				  调用的子函数，
*				  该函数通过发送MME获取CLT/CNU的软件版本，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_Atheros_ProcessGetSwVer(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	uint8_t VerStr[2*STRNG_MAX_LEN] = {0};
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	/* 向设备发送MME *//* 等待设备回应*/
	/* 将处理信息发送给请求者 */
	MMEAD_ProcessAck(MME_Atheros_MsgGetSwVer(MME_SK, h->ODA, VerStr), this, VerStr, 2*STRNG_MAX_LEN);
}

void MME_Atheros_ProcessGetFrequencyBandSelection(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_MMEAD_FBS fbs;
	/* 向设备发送MME *//* 等待设备回应*/
	/* 将处理信息发送给请求者 */
	MMEAD_ProcessAck(MME_Atheros_MsgGetFrequencyBandSelection(MME_SK, h->ODA, &fbs), this, (uint8_t *)&fbs, sizeof(T_MMEAD_FBS));
}

void MME_Atheros_ProcessSetFrequencyBandSelection(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_MMETS_REQ_MSG *req= (T_MMETS_REQ_MSG *)(this->b);	
	T_MMEAD_FBS fbs;

	memcpy(&fbs, req->body, sizeof(T_MMEAD_FBS));

	/* 向设备发送MME *//* 等待设备回应*/
	/* 将处理信息发送给请求者 */
	MMEAD_ProcessAck(MME_Atheros_MsgSetFrequencyBandSelection(MME_SK, req->header.ODA, &fbs), this, NULL, 0);
}

void MME_Atheros_ProcessGetTxGain(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	uint8_t tx_gain;
	/* 向设备发送MME *//* 等待设备回应*/
	/* 将处理信息发送给请求者 */
	MMEAD_ProcessAck(MME_Atheros_MsgGetTxGain(MME_SK, h->ODA, &tx_gain), this, (uint8_t *)&tx_gain, sizeof(uint8_t));
}

void MME_Atheros_ProcessGetUserHFID(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	uint8_t user_hfid[64] = {0};
	/* 向设备发送MME *//* 等待设备回应*/
	/* 将处理信息发送给请求者 */
	MMEAD_ProcessAck(MME_Atheros_MsgGetUserHFID(MME_SK, h->ODA, user_hfid),this,user_hfid,64);
}

void MME_Atheros_ProcessGetHgManage(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_szHgManage manage_data; 
	
	MMEAD_ProcessAck(MME_Atheros_MsgGetHgManage(MME_SK, h->ODA, &manage_data), this, &manage_data, sizeof(T_szHgManage));
	
}

void MME_Atheros_ProcessSetHgManage(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_MMETS_REQ_MSG *req = (T_MMETS_REQ_MSG *)(this->b);
	T_szHgManage manage;

	memcpy(&manage, req->body, sizeof(manage));

	MMEAD_ProcessAck(MME_Atheros_MsgSetHgManage(MME_SK, req->header.ODA, manage), this, NULL, 0);
}

void MME_Atheros_ProcessGetHgBusiness(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_szHgBusiness business_data;
	
	MMEAD_ProcessAck(MME_Atheros_MsgGetHgBusiness(MME_SK, h->ODA, &business_data), this, &business_data, sizeof(T_szHgBusiness));
} 

void MME_Atheros_ProcessSetHgBusiness(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_MMETS_REQ_MSG *req = (T_MMETS_REQ_MSG *)(this->b);
	T_szSetHgBusiness manage;

	memcpy(&manage, req->body, sizeof(manage));

	MMEAD_ProcessAck(MME_Atheros_MsgSetHgBusiness(MME_SK, req->header.ODA, manage), this, NULL, 0);
}

void MME_Atheros_ProcessRebootHg(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MMEAD_ProcessAck(MME_Atheros_MsgRebootHg(MME_SK, h->ODA), this, NULL, 0);
}

void MME_Atheros_ProcessResetHg(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MMEAD_ProcessAck(MME_Atheros_MsgResetHg(MME_SK, h->ODA), this, NULL, 0);
}

void MME_Atheros_ProcessGetHgSsidStatus(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_szHgSsid ssid_info;
	
	MMEAD_ProcessAck(MME_Atheros_MsgGetHgSsidStatus(MME_SK, h->ODA, &ssid_info), this, &ssid_info, sizeof(T_szHgSsid));
} 

void MME_Atheros_ProcessSetHgSsidStatus(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_MMETS_REQ_MSG *req = (T_MMETS_REQ_MSG *)(this->b);
	T_szSetHgSsid ssid;

	memcpy(&ssid, req->body, sizeof(ssid));

	MMEAD_ProcessAck(MME_Atheros_MsgSetHgSsidStatus(MME_SK, req->header.ODA, ssid), this, NULL, 0);
}

void MME_Atheros_ProcessGetHgWanStatus(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_szHgWanStatus wan_info;
	
	MMEAD_ProcessAck(MME_Atheros_MsgGetHgWanStatus(MME_SK, h->ODA, &wan_info), this, &wan_info, sizeof(T_szHgWanStatus));
} 

void MME_Atheros_ProcessGetHgWifiMode(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	uint8_t mode;
	
	MMEAD_ProcessAck(MME_Atheros_MsgGetHgWifiMode(MME_SK, h->ODA, &mode), this, &mode, sizeof(uint8_t));
} 

void MME_Atheros_ProcessSetHgWifiMode(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_MMETS_REQ_MSG *req = (T_MMETS_REQ_MSG *)(this->b);
	uint8_t mode;

	memcpy(&mode, req->body, sizeof(mode));

	MMEAD_ProcessAck(MME_Atheros_MsgSetHgWifiMode(MME_SK, req->header.ODA, mode), this, NULL, 0);
}

/*
void MME_Atheros_ProcessSetUserHFID(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_MMETS_REQ_MSG *req = (T_MMETS_REQ_MSG *)(this->b);
	uint8_t user_hfid[64] = {0};
	
	memcpy(user_hfid, req->body, 64);
	

	MMEAD_ProcessAck(MME_Atheros_MsgSetUserHFID(MME_SK, req->header.ODA, user_hfid), this, NULL, 0);
}*/

void MME_Atheros_ProcessSetTxGain(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_MMETS_REQ_MSG *req= (T_MMETS_REQ_MSG *)(this->b);	
	uint8_t tx_gain;

	memcpy(&tx_gain, req->body, sizeof(uint8_t));
	/* 向设备发送MME *//* 等待设备回应*/
	/* 将处理信息发送给请求者 */
	MMEAD_ProcessAck(MME_Atheros_MsgSetTxGain(MME_SK, req->header.ODA, tx_gain), this, NULL, 0);
}

/********************************************************************************************
*	函数名称:MME_Atheros_ProcessGetManufacturerInfo
*	函数功能:该函数为MME_ProcessGetManufacturerInfo函数完成硬件屏蔽之后
*				  调用的子函数，
*				  该函数通过发送MME获取CLT/CNU的厂商信息，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_Atheros_ProcessGetManufacturerInfo(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	uint8_t VerStr[STRNG_MAX_LEN] = {0};
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	/* 向设备发送MME *//* 等待设备回应*/
	/* 将处理信息发送给请求者 */
	MMEAD_ProcessAck(MME_Atheros_MsgGetManufacturerInfo(MME_SK, h->ODA, VerStr), this, VerStr, STRNG_MAX_LEN);
}

/********************************************************************************************
*	函数名称:MME_Atheros_ProcessGetTopology
*	函数功能:该函数为MME_ProcessGetTopology函数完成硬件屏蔽之后
*				  调用的子函数，
*				  该函数通过发送MME获取在线的网元，并将获取
*				  到的信息通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_Atheros_ProcessGetTopology(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_MMEAD_TOPOLOGY NEList;

	/* 向设备发送MME *//* 等待设备回应*/
	/* 将处理信息发送给请求者 */
	MMEAD_ProcessAck(MME_Atheros_MsgGetTopology(MME_SK, h->ODA, &NEList), 
						this, (uint8_t *)&NEList, sizeof(NEList));
}

/********************************************************************************************
*	函数名称:MME_Atheros_ProcessGetTopologyStats
*	函数功能:  本函数是针对74系列分片包0xa0 74 获取拓扑的函数
*         该函数为MME_ProcessGetTopologyStats函数完成硬件屏蔽之后
*				  调用的子函数，
*				  该函数通过发送MME获取在线的网元，并将获取
*				  到的信息通过消息接口发送给请求模块
*	作者:stan
*	时间:2013-03-12
*********************************************************************************************/
void MME_Atheros_ProcessGetTopologyStats(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_MMEAD_TOPOLOGY NEList;

	/* 向设备发送MME *//* 等待设备回应*/
	/* 将处理信息发送给请求者 */
	MMEAD_ProcessAck(MME_Atheros_MsgGetTopologyStats(MME_SK, h->ODA, &NEList), 
						this, (uint8_t *)&NEList, sizeof(NEList));
}

/********************************************************************************************
*	函数名称:MME_ProcessGetCltMac
*	函数功能:该函数通过发送MME获取CLT的MAC地址，并将获取
*				  到的信息通过消息接口发送给请求模块
*	限制:这里假设只有一个CLT线卡，否则该函数的逻辑需要
*			修改
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessGetCltMac(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	switch(h->DEV_TYPE)
	{
		case WEC_3801I:
		{
			MME_Atheros_ProcessGetCltMac(this, MME_SK);
			break;
		}
		case WEC_3501S:
		case WEC_3502S:
		case WEC_3504S:
		{
			MME_Spc_ProcessGetCltMac(this, MME_SK);
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessResetDevice
*	函数功能:该函数通过发送MME完成CLT/CNU的复位操作，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessResetDevice(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	switch(h->DEV_TYPE)
	{
		case WEC_3601I:
		case WEC_3602I:
		case WEC_3604I:
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_3801I:
		case WEC_602:
		case WEC_604:
		case WEC701_M0:
		case WEC701_C2:
		case WEC701_C4:
		case WEC701_E4:
		case WEC701_L4:
		case WEC701_W4:
		default:
		{
			MME_Atheros_ProcessResetDevice(this, MME_SK);
			break;
		}		
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessGetSwVer
*	函数功能:，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessGetSwVer(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	switch(h->DEV_TYPE)
	{
		case WEC_3601I:
		case WEC_3602I:
		case WEC_3604I:
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_3801I:
		case WEC_602:
		case WEC_604:
		case WEC701_M0:
		case WEC701_C2:
		case WEC701_C4:
		case WEC701_E4:
		case WEC701_L4:
		case WEC701_W4:
		{
			MME_Atheros_ProcessGetSwVer(this, MME_SK);
			break;
		}
		case WEC_3501S:
		case WEC_3502S:
		case WEC_3504S:
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

void MME_ProcessGetFrequecyBandSelection(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	if( boardapi_isAr7400Device(h->DEV_TYPE) )
	{
		MME_Atheros_ProcessGetFrequencyBandSelection(this, MME_SK);
	}
	else
	{
		/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
		MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
	}	
}

void MME_ProcessSetFrequecyBandSelection(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	if( boardapi_isAr7400Device(h->DEV_TYPE) )
	{
		MME_Atheros_ProcessSetFrequencyBandSelection(this, MME_SK);
	}
	else
	{
		/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
		MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
	}	
}

void MME_ProcessGetTxGain(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	if( boardapi_isAr7400Device(h->DEV_TYPE) )
	{
		MME_Atheros_ProcessGetTxGain(this, MME_SK);
	}
	else
	{
		/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
		MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
	}	
}

void MME_ProcessGetUserHFID(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MME_Atheros_ProcessGetUserHFID(this, MME_SK);
}

void MME_ProcessGetHgManage(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MME_Atheros_ProcessGetHgManage(this, MME_SK);
}

void MME_ProcessSetHgManage(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MME_Atheros_ProcessSetHgManage(this, MME_SK);
}

void MME_ProcessGetHgBusiness(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MME_Atheros_ProcessGetHgBusiness(this, MME_SK);
} 

void MME_ProcessSetHgBusiness(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MME_Atheros_ProcessSetHgBusiness(this, MME_SK);
}

void MME_ProcessRebootHg(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MME_Atheros_ProcessRebootHg(this, MME_SK);
}

void MME_ProcessResetHg(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MME_Atheros_ProcessResetHg(this, MME_SK);
}

void MME_ProcessGetHgSsidStatus(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MME_Atheros_ProcessGetHgSsidStatus(this, MME_SK);
} 

void MME_ProcessSetHgSsidStatus(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MME_Atheros_ProcessSetHgSsidStatus(this, MME_SK);
} 

void MME_ProcessGetHgWanStatus(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MME_Atheros_ProcessGetHgWanStatus(this, MME_SK);
} 

void MME_ProcessGetHgWifiMode(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MME_Atheros_ProcessGetHgWifiMode(this, MME_SK);
} 

void MME_ProcessSetHgWifiMode(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MME_Atheros_ProcessSetHgWifiMode(this, MME_SK);
} 


/*
void MME_ProcessSetUserHFID(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	MME_Atheros_ProcessSetUserHFID(this, MME_SK);
}*/

void MME_ProcessSetTxGain(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	if( boardapi_isAr7400Device(h->DEV_TYPE) )
	{
		MME_Atheros_ProcessSetTxGain(this, MME_SK);
	}
	else
	{
		/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
		MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
	}	
}

/********************************************************************************************
*	函数名称:MME_ProcessGetManufacturerInfo
*	函数功能:，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessGetManufacturerInfo(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	switch(h->DEV_TYPE)
	{
		case WEC_3601I:
		case WEC_3602I:
		case WEC_3604I:
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_3801I:
		case WEC_602:
		case WEC_604:
		case WEC701_M0:
		case WEC701_C2:
		case WEC701_C4:
		case WEC701_E4:
		case WEC701_L4:
		case WEC701_W4:
			
		{
			MME_Atheros_ProcessGetManufacturerInfo(this, MME_SK);
			break;
		}
		case WEC_3501S:
		case WEC_3502S:
		case WEC_3504S:
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}


/********************************************************************************************
*	函数名称:MME_ProcessGetTopology
*	函数功能:该函数通过发送MME获取在线的网元，并将获取
*				  到的信息通过消息接口发送给请求模块
*	限制:这里一次调用只能获取一块CLT线卡的拓扑，对于有
*			多块线卡的CBAT，注册模块需要针对每块CLT都调用
*			一次该函数才能拿到所有的拓扑信息
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessGetTopology(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	switch(h->DEV_TYPE)
	{
		/* 这里仅仅支持Atheros的CLT线卡WEC_3801I */
		case WEC_3801I:
		{
			MME_Atheros_ProcessGetTopology(this, MME_SK);
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessGetTopologyStats
*	函数功能: 针对74系列 分片机制获取 0xa0 74 获取拓扑
*  该函数通过发送MME获取在线的网元，并将获取
*				  到的信息通过消息接口发送给请求模块
*	限制:这里一次调用只能获取一块CLT线卡的拓扑，对于有
*			多块线卡的CBAT，注册模块需要针对每块CLT都调用
*			一次该函数才能拿到所有的拓扑信息
*	作者:stan
*	时间:2013-01-12
*********************************************************************************************/
void MME_ProcessGetTopologyStats(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	switch(h->DEV_TYPE)
	{
		/* 这里仅仅支持Atheros的CLT线卡WEC_3801I */
		case WEC_3801I:
		{
			MME_Atheros_ProcessGetTopologyStats(this, MME_SK);
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessMdioRead
*	函数功能:通过发送MDIO MME来读取CNU上的switch寄存器，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessMdioRead(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_Msg_MMEAD *msg = (T_Msg_MMEAD *)(this->b);
	T_szMdioPhy v;

	memcpy(&v, msg->BUF, sizeof(T_szMdioPhy));

	switch(h->DEV_TYPE)
	{
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_C2:
		case WEC701_C4:
		case WEC701_E4:
		case WEC701_L4:
		case WEC701_W4:	
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(MME_Atheros_MsgGetPhyReg(MME_SK, h->ODA, &v), 
				this, (uint8_t *)&v, sizeof(T_szMdioPhy));
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessMdioWrite
*	函数功能:通过发送MDIO MME来设置CNU上的switch寄存器，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessMdioWrite(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_Msg_MMEAD *msg = (T_Msg_MMEAD *)(this->b);
	T_szMdioPhy v;

	memcpy(&v, msg->BUF, sizeof(T_szMdioPhy));
	
	switch(h->DEV_TYPE)
	{
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_C2:
		case WEC701_C4:
		case WEC701_E4:
		case WEC701_L4:
		case WEC701_W4:
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(MME_Atheros_MsgSetPhyReg(MME_SK, h->ODA, &v), 
				this, (uint8_t *)&v, sizeof(T_szMdioPhy));
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessAr8236PhyRegRead
*	函数功能:通过发送MDIO MME来读取CNU上的AR8236的内部PHY寄存器，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessAr8236PhyRegRead(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_Msg_MMEAD *msg = (T_Msg_MMEAD *)(this->b);
	T_szMdioPhy v;

	memcpy(&v, msg->BUF, sizeof(T_szMdioPhy));

	switch(h->DEV_TYPE)
	{
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_C2:
		case WEC701_C4:
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(MME_Atheros_MsgGetPhyReg(MME_SK, h->ODA, &v), 
				this, (uint8_t *)&v, sizeof(T_szMdioPhy));
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessAr8236PhyRegWrite
*	函数功能:通过发送MDIO MME来设置CNU上的AR8236的内部PHY寄存器，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessAr8236PhyRegWrite(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_Msg_MMEAD *msg = (T_Msg_MMEAD *)(this->b);
	T_szMdioPhy v;

	memcpy(&v, msg->BUF, sizeof(T_szMdioPhy));
	
	switch(h->DEV_TYPE)
	{
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_C2:
		case WEC701_C4:
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(MME_Atheros_MsgSetPhyReg(MME_SK, h->ODA, &v), 
				this, (uint8_t *)&v, sizeof(T_szMdioPhy));
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessAr8236SwRegRead
*	函数功能:通过发送MDIO MME来读取CNU上的AR8236的内部MAC寄存器，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessAr8236SwRegRead(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_Msg_MMEAD *msg = (T_Msg_MMEAD *)(this->b);
	T_szMdioSw v;

	memcpy(&v, msg->BUF, sizeof(T_szMdioSw));	
	
	switch(h->DEV_TYPE)
	{
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_C2:
		case WEC701_C4:
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(MME_Atheros_MsgGetSwReg(MME_SK, h->ODA, &v), 
				this, (uint8_t *)&v, sizeof(T_szMdioSw));
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessAr8236SwRegWrite
*	函数功能:通过发送MDIO MME来设置CNU上的AR8236的内部MAC寄存器，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessAr8236SwRegWrite(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_Msg_MMEAD *msg = (T_Msg_MMEAD *)(this->b);
	T_szMdioSw v;

	memcpy(&v, msg->BUF, sizeof(T_szMdioSw));	
	
	switch(h->DEV_TYPE)
	{
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_C2:
		case WEC701_C4:
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(MME_Atheros_MsgSetSwReg(MME_SK, h->ODA, &v), 
				this, (uint8_t *)&v, sizeof(T_szMdioSw));
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessReadModuleOperation
*	函数功能:获取CNU的自定义参数快，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessReadModuleOperation(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_Msg_MMEAD *msg = (T_Msg_MMEAD *)(this->b);
	T_MMEAD_RD_MOD_REQ_INFO *request = (T_MMEAD_RD_MOD_REQ_INFO *)(msg->BUF);
	T_MMEAD_RD_MOD_ACK_INFO comfirm;	
	
	switch(h->DEV_TYPE)
	{
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_C2:
		case WEC701_C4:
		case WEC701_E4:
		case WEC701_L4:
		case WEC701_W4:
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(MME_Atheros_MsgReadModule(MME_SK, h->ODA, request, &comfirm), 
				this, (uint8_t *)&comfirm, sizeof(comfirm));
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessWriteModuleOperation
*	函数功能:获取CNU的自定义参数快，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessWriteModuleOperation(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	//T_Msg_MMEAD *msg = (T_Msg_MMEAD *)(this->b);	
	
	switch(h->DEV_TYPE)
	{
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_C2:
		case WEC701_C4:
		case WEC701_E4:
		case WEC701_L4:
		case WEC701_W4:
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(MME_Atheros_MsgWriteModule(MME_SK, h->ODA), this, NULL, 0);
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessDirectWriteMod
*	函数功能:写参数块
*	作者:frank
*	时间:2013-10-27
*********************************************************************************************/
void MME_ProcessDirectWriteMod(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_MMETS_REQ_MSG *MMETS_REQ = (T_MMETS_REQ_MSG *)(this->b);
	uint8_t *mod = MMETS_REQ->body;
	uint32_t len = MMETS_REQ->header.LEN;
	
	MMEAD_ProcessAck
	(
		MME_Atheros_MsgDirectWriteModule(MME_SK, MMETS_REQ->header.ODA, mod, len), 
		this,
		NULL, 
		0
	);
}

/********************************************************************************************
*	函数名称:MME_ProcessDirectWriteMod
*	函数功能:写参数块
*	作者:frank
*	时间:2013-10-27
*********************************************************************************************/
void MME_ProcessEraseMod(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_MMETS_REQ_MSG *MMETS_REQ = (T_MMETS_REQ_MSG *)(this->b);
	T_MMEAD_ERASE_MOD_REQ_INFO *erase = (T_MMEAD_ERASE_MOD_REQ_INFO *)(MMETS_REQ->body);
	
	MMEAD_ProcessAck
	(
		MME_Atheros_MsgEraseModule(MME_SK, MMETS_REQ->header.ODA, erase), 
		this,
		NULL, 
		0
	);
}

/********************************************************************************************
*	函数名称:MME_ProcessReadModuleOperationCrc
*	函数功能:获取CNU的自定义参数快的CRC，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MME_ProcessReadModuleOperationCrc(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	uint32_t ModCrc = 0;	
	
	switch(h->DEV_TYPE)
	{
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_C2:
		case WEC701_C4:
		case WEC701_E4:
		case WEC701_L4:
		case WEC701_W4:	
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(MME_Atheros_MsgGetModuleCrc(MME_SK, h->ODA, &ModCrc), 
				this, (uint8_t *)&ModCrc, sizeof(uint32_t));
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessReadPibCrc
*	函数功能:获取6400设备PIB的CRC，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-12-01
*********************************************************************************************/
void MME_ProcessReadPibCrc(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	uint32_t ModCrc = 0;	
	
	switch(h->DEV_TYPE)
	{
		case WEC_3801I:
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_M0:
		case WEC701_C2:
		case WEC701_C4:
		case WEC701_E4:
		case WEC701_L4:
		case WEC701_W4:
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(MME_Atheros_MsgGetPibCrc(MME_SK, h->ODA, &ModCrc), 
				this, (uint8_t *)&ModCrc, sizeof(uint32_t));
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessReadPib
*	函数功能:获取6400设备PIB，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-12-01
*********************************************************************************************/
void MME_ProcessReadPib(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	uint32_t ModCrc = 0;	
	
	switch(h->DEV_TYPE)
	{
		case WEC_3801I:
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_M0:
		case WEC701_C2:
		case WEC701_C4:
		case WEC701_E4:
		case WEC701_L4:
		case WEC701_W4:
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(MME_Atheros_MsgReadPib(MME_SK, h->ODA), 
				this, (uint8_t *)&ModCrc, sizeof(uint32_t));
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessWritePib
*	函数功能:将预先定义的PIB 写入6400设备，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2010-12-01
*********************************************************************************************/
void MME_ProcessWritePib(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);	
	
	switch(h->DEV_TYPE)
	{
		case WEC_3801I:
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_M0:
		case WEC701_C2:
		case WEC701_C4:
		case WEC701_E4:
		case WEC701_L4:
		case WEC701_W4:
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(MME_Atheros_MsgWritePib(MME_SK, h->ODA), this, NULL, 0);
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessUserEnable
*	函数功能:发送MME允许某个设备访问网络
*	作者:frank
*	时间:2010-12-01
*********************************************************************************************/
void MME_ProcessUserEnable(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);	
	
	switch(h->DEV_TYPE)
	{
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_C2:
		case WEC701_C4:
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(MME_Atheros_MsgEnableDev(MME_SK, h->DEV_TYPE, h->ODA), this, NULL, 0);
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessUserBlock
*	函数功能:发送MME禁止某个设备访问网络
*	作者:frank
*	时间:2010-12-01
*********************************************************************************************/
void MME_ProcessUserBlock(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	
	
	switch(h->DEV_TYPE)
	{
		case WEC_3702I:
		case WEC_3703I:
		case WEC_3704I:
		case WEC_602:
		case WEC_604:
		case WEC701_C2:
		case WEC701_C4:
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(MME_Atheros_MsgDisableDev(MME_SK, h->DEV_TYPE, h->ODA), this, NULL, 0);
			break;
		}
		default:
		{
			/* 对于不支持的DEV_TYPE应该给予应答以便让请求者知道 */
			MMEAD_ProcessAck(CMM_UNKNOWN_DEVTYPE, this, NULL, 0);
			break;
		}
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessDevBootOut
*	函数功能:将一个CNU从AVLAN中踢出
*	作者:frank
*	时间:2010-12-01
*********************************************************************************************/
void MME_ProcessDevBootOut(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_Msg_MMEAD *msg = (T_Msg_MMEAD *)(this->b);
	uint8_t cnuMac[6];

	memcpy(cnuMac, msg->BUF, 6);
	
	/* 将错误信息返回给请求者 */
	MMEAD_ProcessAck(MME_Atheros_MsgBootOutDev(MME_SK, h->ODA, cnuMac), this, NULL, 0);
}

/********************************************************************************************
*	函数名称:MME_ProcessLinkDiag
*	函数功能:
*	作者:frank
*	时间:2010-12-01
*********************************************************************************************/
void MME_ProcessLinkDiag(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	T_Msg_MMEAD *msg = (T_Msg_MMEAD *)(this->b);
	T_MMEAD_LINK_DIAG_INFO *idata = (T_MMEAD_LINK_DIAG_INFO *)(msg->BUF);
	
	ihpapi_toneMapCtl_t inputToneMapInfo;
	ihpapi_connectCtl_t inputConnectInfo;
	ihpapi_getConnectInfoData_t outputConnectInfo;
	ihpapi_getNetworkInfoData_t outputNetInfo;
	T_MMEAD_LINK_DIAG_RESULT diagInfo;
	uint8_t *p = NULL;
	uint32_t *int32p = NULL;
	int i = 0;
	int temp = 0;
	uint64_t dh = 0;
	uint64_t dl = 0;
	uint8_t cfound = 0;
	
	bzero(&diagInfo, sizeof(T_MMEAD_LINK_DIAG_RESULT));
	diagInfo.dir = idata->dir;
	memcpy(diagInfo.mac, msg->HEADER.ODA, 6);
	memcpy(diagInfo.peerNodeMac, idata->peerNodeMac, 6);
	memcpy(diagInfo.ccoMac, idata->ccoMac, 6);
	
	inputToneMapInfo.tmslot = 0;
	memcpy(inputToneMapInfo.macaddress, idata->peerNodeMac, 6);	

	inputConnectInfo.mcontrol = 0;
	inputConnectInfo.direction = idata->dir;
	inputConnectInfo.lid = 0xF8;
	memcpy(inputConnectInfo.macaddress, idata->peerNodeMac, 6);	

	bzero(&outputConnectInfo, sizeof(ihpapi_getConnectInfoData_t));

	/* (Tx = 0, Rx = 1, Tx & Rx = 2) */
	if( 0 == idata->dir )
	{
		ihpapi_getToneMapData_t outputToneMapInfo;		
		bzero(&outputToneMapInfo, sizeof(ihpapi_getToneMapData_t));
		if( idata->chipser == 0 )
		{
			if( CMM_SUCCESS == MME_Atheros_MsgGetTxToneMapInfo(MME_SK, msg->HEADER.ODA, &inputToneMapInfo, &outputToneMapInfo) )
			{
				p = outputToneMapInfo.mod_carrier;
				for( i=0; i<MOD_CARRIER_MAX_TUPLE_NUM; i++ )
				{
					temp += __mapModCarrier(p[i]&0x0f);
					temp += __mapModCarrier((p[i]>>4)&0x0f);
				}
				diagInfo.bitRate = (float)temp/outputToneMapInfo.tmnumactcarriers;
				diagInfo.attenuation = 0;			
			}
			else
			{
				/* 将错误信息返回给请求者 */
				MMEAD_ProcessAck(CMM_FAILED, this, NULL, 0);
				return;
			}
		}
		else if( idata->chipser == 1)
		{
			if( CMM_SUCCESS == MME_Atheros_MsgGet74TxToneMapInfo(MME_SK, msg->HEADER.ODA, &inputToneMapInfo, &outputToneMapInfo) )
			{
				p = outputToneMapInfo.mod_carrier;
				for( i=0; i<MOD_CARRIER_MAX_TUPLE_NUM; i++ )
				{
					temp += __mapModCarrier(p[i]&0x0f);
					temp += __mapModCarrier((p[i]>>4)&0x0f);
				}
				diagInfo.bitRate = (float)temp/outputToneMapInfo.tmnumactcarriers;
				diagInfo.attenuation = 0;
			}
			else
			{
				/* 将错误信息返回给请求者 */
				MMEAD_ProcessAck(CMM_FAILED, this, NULL, 0);
				return;
			}
		}			
	}
	else if( 1 == idata->dir )
	{
		ihpapi_getRxToneMapData_t outputToneMapInfo;
		bzero(&outputToneMapInfo, sizeof(ihpapi_getRxToneMapData_t));
		if( idata->chipser == 0 )
		{
			if( CMM_SUCCESS == MME_Atheros_MsgGetRxToneMapInfo(MME_SK, msg->HEADER.ODA, &inputToneMapInfo, &outputToneMapInfo) )
			{
				p = outputToneMapInfo.mod_carrier;
				for( i=0; i<MOD_CARRIER_MAX_TUPLE_NUM; i++ )
				{
					temp += __mapModCarrier(p[i]&0x0f);
					temp += __mapModCarrier((p[i]>>4)&0x0f);
				}			
				diagInfo.bitRate = (float)temp/outputToneMapInfo.tmnumactcarriers;
				diagInfo.attenuation = outputToneMapInfo.avg_agc_gain;			
			}
			else
			{
				/* 将错误信息返回给请求者 */
				MMEAD_ProcessAck(CMM_FAILED, this, NULL, 0);
				return;
			}
		}
		else if( idata->chipser == 1 )
		{
			if( CMM_SUCCESS == MME_Atheros_MsgGet74RxToneMapInfo(MME_SK, msg->HEADER.ODA, &inputToneMapInfo, &outputToneMapInfo) )
			{
				p = outputToneMapInfo.mod_carrier;
				for( i=0; i<MOD_CARRIER_MAX_TUPLE_NUM; i++ )
				{
					temp += __mapModCarrier(p[i]&0x0f);
					temp += __mapModCarrier((p[i]>>4)&0x0f);
				}			
				diagInfo.bitRate = (float)temp/outputToneMapInfo.tmnumactcarriers;
				diagInfo.attenuation = outputToneMapInfo.avg_agc_gain;			
			}
			else
			{
				/* 将错误信息返回给请求者 */
				MMEAD_ProcessAck(CMM_FAILED, this, NULL, 0);
				return;
			}
		}
	}
	else
	{
		/* 将错误信息返回给请求者 */
		MMEAD_ProcessAck(CMM_FAILED, this, NULL, 0);
		return;
	}

	if( CMM_SUCCESS == MME_Atheros_MsgGetConnectionInfo(MME_SK, msg->HEADER.ODA, &inputConnectInfo, &outputConnectInfo) )
	{
		int32p = (uint32_t *)outputConnectInfo.lstats;
		
		dl = intohl(*int32p++);
		dh = intohl(*int32p++);		
		diagInfo.MPDU_ACKD = (dh<<32)|dl;

		dl = intohl(*int32p++);
		dh = intohl(*int32p++);
		diagInfo.MPDU_COLL = (dh<<32)|dl;

		dl = intohl(*int32p++);
		dh = intohl(*int32p++);
		diagInfo.MPDU_FAIL = (dh<<32)|dl;

		dl = intohl(*int32p++);
		dh = intohl(*int32p++);
		diagInfo.PBS_PASS = (dh<<32)|dl;

		dl = intohl(*int32p++);
		dh = intohl(*int32p++);
		diagInfo.PBS_FAIL = (dh<<32)|dl;
	}
	else
	{
		/* 将错误信息返回给请求者 */
		MMEAD_ProcessAck(CMM_FAILED, this, NULL, 0);
		return;
	}

	if( idata->chipser == 0 )
	{
		if( CMM_SUCCESS == MME_Atheros_MsgGetNetInfo(MME_SK, diagInfo.ccoMac, &outputNetInfo) )
		{
			cfound = 0;
			p = outputNetInfo.nwinfo;
			memcpy(diagInfo.ccoNid, p, 7);
			p += 7;
			diagInfo.ccoSnid = *p++;
			diagInfo.ccoTei = *p;
			p += 9;
			temp = *p++;
			for( i=0; i<temp; i++, p+=15 )
			{
				if( 0 == memcmp(msg->HEADER.ODA, p, 6) )
				{
					cfound = 1;
					diagInfo.tei = *(p+6);
					memcpy(diagInfo.bridgedMac, p+7, 6);
					diagInfo.tx = *(p+13);
					diagInfo.rx = *(p+14);
					break;
				}
			}
			if(!cfound)
			{
				/* 将错误信息返回给请求者 */
				MMEAD_ProcessAck(CMM_FAILED, this, NULL, 0);
				return;
			}
		}
		else
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(CMM_FAILED, this, NULL, 0);
			return;
		}
	}
	else if( idata->chipser == 1 )
	{
		if( CMM_SUCCESS == MME_Atheros_MsgGet74NetInfo(MME_SK, diagInfo.ccoMac, &outputNetInfo) )
		{
			cfound = 0;
			p = outputNetInfo.nwinfo;
			memcpy(diagInfo.ccoNid, p, 7);
			p += 9;
			diagInfo.ccoSnid = *p++;
			diagInfo.ccoTei = *p;
			p += 16;
			temp = *p++;
			p += 5;
			for( i=0; i<temp; i++, p+=24 )
			{
				if( 0 == memcmp(msg->HEADER.ODA, p, 6) )
				{
					cfound = 1;
					diagInfo.tei = *(p+6);
					memcpy(diagInfo.bridgedMac, p+10, 6);
					diagInfo.tx = *(p+16) | (*(p+17)<<8);
					diagInfo.rx = *(p+20) | (*(p+21)<<8);
					break;
				}
			}
			if(!cfound)
			{
				/* 将错误信息返回给请求者 */
				MMEAD_ProcessAck(CMM_FAILED, this, NULL, 0);
				return;
			}
		}
		else
		{
			/* 将错误信息返回给请求者 */
			MMEAD_ProcessAck(CMM_FAILED, this, NULL, 0);
			return;
		}
	}
	MMEAD_ProcessAck(CMM_SUCCESS, this, (uint8_t *)&diagInfo, sizeof(T_MMEAD_LINK_DIAG_RESULT));
}

/********************************************************************************************
*	函数名称:MME_ProcessGetRtl8306eConfig
*	函数功能:通过发送MDIO MME来读取CNU上的RTL8306E的内部寄存器配置，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:frank
*	时间:2013-10-18
*********************************************************************************************/
void MME_ProcessGetRtl8306eConfig(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	uint32_t len = 0;
	uint8_t buf[MAX_BODY_SIZE];		
	st_rtl8306eSettings *rtl8306e = (st_rtl8306eSettings *)buf;
	T_MMETS_REQ_MSG *req = (T_MMETS_REQ_MSG *)(this->b);

	len = sizeof(st_rtl8306eSettings);
	if( CMM_SUCCESS != mme_rtl8306e_get_config_all(MME_SK, req->header.ODA, rtl8306e))
	{
		MMEAD_ProcessAck(CMM_MME_ERROR, this, NULL, 0);
	}
	else
	{
		MMEAD_ProcessAck(CMM_SUCCESS, this, buf, len);	
	}
}

/********************************************************************************************
*	函数名称:MME_ProcessGetRtl8306ePortStatus
*	函数功能:通过发送MDIO MME来读取CNU上的RTL8306E的内部寄存器配置，
*				  并将操作结果通过消息接口发送给请求模块
*	作者:Burning
*	时间:2015-2-12
*********************************************************************************************/
void MME_ProcessGetRtl8306ePortStatus(MMEAD_BBLOCK_QUEUE *this, T_MME_SK_HANDLE *MME_SK)
{
	uint32_t len = 0;
	uint8_t buf[MAX_BODY_SIZE];		
	st_rtl8306e_port_status *linkstatus = (st_rtl8306e_port_status *)buf;
	T_MMETS_REQ_MSG *req = (T_MMETS_REQ_MSG *)(this->b);

	len = sizeof(st_rtl8306e_port_status);
	if( CMM_SUCCESS != rtl8306e_get_port_link_status(MME_SK, req->header.ODA, linkstatus))
	{
		MMEAD_ProcessAck(CMM_MME_ERROR, this, NULL, 0);
	}
	else
	{
		MMEAD_ProcessAck(CMM_SUCCESS, this, buf, len);	
	}
}


/********************************************************************************************
*	函数名称:MME_InitSocket
*	函数功能:创建跟CLT/CNU通讯的2层SOCKET
*	限制:假设同一个CBAT下的CLT模块不能混插，这里只创建
*			了与Atheros通讯的MME SOCKET，其它类型的待扩展
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
int MME_InitSocket(T_MME_SK_HANDLE *sk, uint8_t prog)
{
	int ret = 0;
	struct ifreq interface;
	struct ifreq ifr;
	
	switch(prog)
	{
		case EOC_PROG_ATHEROS:
		{
			/* Create socket */
			sk->skfd = socket(PF_PACKET, SOCK_RAW, htons(HOMEPLUG_MTYPE));
			if (sk->skfd < 0)
			{
				ret = CMM_CREATE_SOCKET_ERROR;
				break;
			}
			/* Bind socket to eth0 interface */
			strncpy(interface.ifr_ifrn.ifrn_name, LOCAL_INTERFACE0, sizeof(LOCAL_INTERFACE0));
			if (setsockopt(sk->skfd, SOL_SOCKET, SO_BINDTODEVICE, 
							(char *)&interface, sizeof(interface)) < 0)
			{
				perror("MME_InitSocket setsockopt error\n");
				close(sk->skfd);
				ret = CMM_CREATE_SOCKET_ERROR;
				break;
			}
			/* Fill socekt address. */
			memset(&(sk->sockaddr), 0, sizeof(sk->sockaddr));
			
			strncpy(ifr.ifr_name, LOCAL_INTERFACE0, sizeof(ifr.ifr_name));
			if (ioctl(sk->skfd, SIOCGIFINDEX, &ifr) < 0)
			{
				printf("MME_InitSocket ioctl SIOCGIFINDEX error\n");
				close(sk->skfd);
				ret = CMM_CREATE_SOCKET_ERROR;
				break;
			}

			sk->sockaddr.sll_family = PF_PACKET;
			sk->sockaddr.sll_protocol = htons(HOMEPLUG_MTYPE);
			sk->sockaddr.sll_ifindex = ifr.ifr_ifindex;
			sk->sockaddr.sll_hatype = ARPHRD_IEEE802;
			sk->sockaddr.sll_pkttype = PACKET_HOST;
			sk->sockaddr.sll_halen = IHPAPI_ETHER_ADDR_LEN;

			ret = CMM_SUCCESS;
			
			break;
		}
		default:
		{
			ret = CMM_UNKNOWN_PROG;
			break;
		}
	}
	return ret;
}

//#ifdef MMEAD_SUPPORT_MUL_THREADS
#if 0
/********************************************************************************************
*	函数名称:ComReqManager
*	函数功能:MMEAD子线程句柄函数，完成设备的通用信息交
*				  互功能， 并将信息通过消息发送给请求模块；
*				  此线程处理的都是响应时间短的实时请求
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void ComReqManager(void)
{
	MMEAD_BBLOCK_QUEUE my_block;
	MMEAD_BBLOCK_QUEUE *this = &my_block;
	MMEAD_BBLOCK_QUEUE *that = &bblock;
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_MME_SK_HANDLE MME_SK;
	
	printf("MmeManager Created thread ComReqManager successfully\n");

	/* 创建MME通信的二层socket */
	if( MME_InitSocket(&MME_SK, EOC_PROG_ATHEROS) != 0 )
	{
		perror("ERROR: ComReqManager MME_InitSocket failed, exit\n");
		pthread_exit(NULL);
	}
	printf("ComReqManager MME_InitSocket %d successfully\n", MME_SK.skfd);

	while(1)
	{
		pthread_mutex_lock(&(that->mutex));
	
		//printf("ComReqManager is waiting signal condreq[0]\n");

		/* 阻塞等待父进程的信号通知 */
		pthread_cond_wait(&(that->condreq[0]), &(that->mutex));

		//printf("ComReqManager signal condreq[0] arrived. Doing something\n");
		/* 在这里完成数据拷贝 */
		memcpy(this, that, sizeof(MMEAD_BBLOCK_QUEUE));
		//printf("ComReqManager done and send signal condack[0]\n");

		/* 通知父进程数据已经拷贝完毕，可以进入下一轮接收过程 */
		pthread_cond_signal(&(that->condack[0]));
		
		pthread_mutex_unlock(&(that->mutex));

		/* 处理逻辑，代码完善中 */
		/* 该分支语句中的任意一个case的处理函数都不应该阻塞 */
		switch(h->MM_TYPE)
		{
			case MMEAD_GET_TOPOLOGY:
				MME_ProcessGetTopology(this, &MME_SK);
				break;
		  case MMEAD_GET_TOPOLOGY_STATS:
		  	MME_ProcessGetTopologyStats(this, &MME_SK);
				break;
			case MMEAD_GET_CLT_MAC:
				MME_ProcessGetCltMac(this, &MME_SK);
				break;
			case MMEAD_RESET_DEV:
				MME_ProcessResetDevice(this, &MME_SK);
				break;
			case MMEAD_GET_SOFTWARE_VERSION:
				MME_ProcessGetSwVer(this, &MME_SK);
				break;
			case MMEAD_GET_MANUFACTURER_INFO:
				MME_ProcessGetManufacturerInfo(this, &MME_SK);
				break;				
			default:
				/* 对于不支持的消息类型应该给予应答以便让请求者知道 */
				MMEAD_ProcessAck(CMM_UNKNOWN_MMTYPE, this, NULL, 0);
				break;
		}
	}

	close(MME_SK.skfd);
	pthread_exit(NULL);
	
}
#else
void ComReqManager(T_MME_SK_HANDLE *MME_SK)
{
	MMEAD_BBLOCK_QUEUE *this = &bblock;
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	
	/* 处理逻辑，代码完善中 */
	/* 该分支语句中的任意一个case的处理函数都不应该阻塞 */
	switch(h->MM_TYPE)
	{
		case MMEAD_GET_TOPOLOGY:
			MME_ProcessGetTopology(this, MME_SK);
			break;
		case MMEAD_GET_TOPOLOGY_STATS:
		  MME_ProcessGetTopologyStats(this, MME_SK);
			break;			
		case MMEAD_GET_CLT_MAC:
			MME_ProcessGetCltMac(this, MME_SK);
			break;
		case MMEAD_RESET_DEV:
			MME_ProcessResetDevice(this, MME_SK);
			break;
		case MMEAD_GET_SOFTWARE_VERSION:
			MME_ProcessGetSwVer(this, MME_SK);
			break;
		case MMEAD_GET_MANUFACTURER_INFO:
			MME_ProcessGetManufacturerInfo(this, MME_SK);
			break;
		case MMEAD_MODULE_MSG_DEBUG_ENABLE:
			MME_ProcessModuleDebugEnable(this, MME_SK);
			break;
		case MMEAD_MODULE_MSG_DEBUG_DISABLE:
			MME_ProcessModuleDebugDisable(this, MME_SK);
			break;
		case MMEAD_AR8236_PHY_REG_READ:
			MME_ProcessAr8236PhyRegRead(this, MME_SK);
			break;
		case MMEAD_AR8236_PHY_REG_WRITE:
			MME_ProcessAr8236PhyRegWrite(this, MME_SK);
			break;
		case MMEAD_AR8236_SW_REG_READ:
			MME_ProcessAr8236SwRegRead(this, MME_SK);
			break;
		case MMEAD_AR8236_SW_REG_WRITE:
			MME_ProcessAr8236SwRegWrite(this, MME_SK);
			break;
		case MMEAD_READ_MODULE_OPERATION:
			MME_ProcessReadModuleOperation(this, MME_SK);
			break;
		case MMEAD_WRITE_MODULE_OPERATION:
			MME_ProcessWriteModuleOperation(this, MME_SK);
			break;
		case MMEAD_READ_MODULE_OPERATION_CRC:
			MME_ProcessReadModuleOperationCrc(this, MME_SK);
			break;
		case MMEAD_READ_PIB_CRC:
			MME_ProcessReadPibCrc(this, MME_SK);
			break;
		case MMEAD_READ_PIB:
			MME_ProcessReadPib(this, MME_SK);
			break;
		case MMEAD_WRITE_PIB:
			MME_ProcessWritePib(this, MME_SK);
			break;
		case MMEAD_AUTHORIZATION_USER:
			MME_ProcessUserEnable(this, MME_SK);
			break;
		case MMEAD_BLOCK_USER:
			MME_ProcessUserBlock(this, MME_SK);
			break;
		case MMEAD_BOOT_OUT_CNU:
			MME_ProcessDevBootOut(this, MME_SK);
			break;
		case MMEAD_LINK_DIAG:
			MME_ProcessLinkDiag(this, MME_SK);
			break;
		case MMEAD_GET_FREQUENCY_BAND_SELECTION:
			MME_ProcessGetFrequecyBandSelection(this, MME_SK);
			break;
		case MMEAD_SET_FREQUENCY_BAND_SELECTION:
			MME_ProcessSetFrequecyBandSelection(this, MME_SK);
			break;
		case MMEAD_GET_TX_GAIN:
			MME_ProcessGetTxGain(this, MME_SK);
			break;
		case MMEAD_GET_USER_HFID:
			MME_ProcessGetUserHFID(this, MME_SK);
			break;
		case MMEAD_GET_HG_MANAGE:
			MME_ProcessGetHgManage(this, MME_SK);
			break;
		case MMEAD_SET_HG_MANAGE:
			MME_ProcessSetHgManage(this, MME_SK);
			break;
		case MMEAD_GET_HG_BUSINESS:
			MME_ProcessGetHgBusiness(this, MME_SK);
			break;
		case MMEAD_SET_HG_BUSINESS:
			MME_ProcessSetHgBusiness(this, MME_SK);
			break;
		case MMEAD_REBOOT_HG:
			MME_ProcessRebootHg(this, MME_SK);
			break;
		case MMEAD_RESET_HG:
			MME_ProcessResetHg(this, MME_SK);
			break;
		case MMEAD_GET_HG_SSID_STATUS:
			MME_ProcessGetHgSsidStatus(this, MME_SK);
			break;
		case MMEAD_SET_HG_SSID_STATUS:
			MME_ProcessSetHgSsidStatus(this, MME_SK);
			break;
		case MMEAD_GET_HG_WAN_STATUS:
			MME_ProcessGetHgWanStatus(this, MME_SK);
			break;
		case MMEAD_GET_HG_WIFI_MODE:
			MME_ProcessGetHgWifiMode(this, MME_SK);
			break;
		case MMEAD_SET_HG_WIFI_MODE:
			MME_ProcessSetHgWifiMode(this, MME_SK);
			break;
		case MMEAD_SET_TX_GAIN:
			MME_ProcessSetTxGain(this, MME_SK);
			break;
		case MMEAD_MDIO_READ:
			MME_ProcessMdioRead(this, MME_SK);
			break;
		case MMEAD_MDIO_WRITE:
			MME_ProcessMdioWrite(this, MME_SK);
			break;
		case MMEAD_GET_RTL8306E_CONFIG:
			MME_ProcessGetRtl8306eConfig(this, MME_SK);
			break;
		case MMEAD_WRITE_MOD:
			MME_ProcessDirectWriteMod(this, MME_SK);
			break;
		case MMEAD_ERASE_MOD:
			MME_ProcessEraseMod(this, MME_SK);
			break;
		case MMEAD_GET_RTL8306E_PORT_STATUS:
			MME_ProcessGetRtl8306ePortStatus(this, MME_SK);
			break;
		default:
			/* 对于不支持的消息类型应该给予应答以便让请求者知道 */
			dbs_sys_log(dbsdev, DBS_LOG_ERR, "mmead ComReqManager->MMEAD_ProcessAck[CMM_UNKNOWN_MMTYPE]");
			MMEAD_ProcessAck(CMM_UNKNOWN_MMTYPE, this, NULL, 0);
			break;
	}
}
#endif

//#ifdef MMEAD_SUPPORT_MUL_THREADS
#if 0
/********************************************************************************************
*	函数名称:DiscoverManager
*	函数功能:MMEAD子线程句柄函数，完成设备的发现功能，
*				  并将拓扑信息通过消息发送给注册模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void DiscoverManager(void)
{
	MMEAD_BBLOCK_QUEUE my_block;
	MMEAD_BBLOCK_QUEUE *this = &my_block;
	MMEAD_BBLOCK_QUEUE *that = &bblock;
	//T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);
	T_MME_SK_HANDLE MME_SK;
	
	printf("MmeManager Created thread DiscoverManager successfully\n");
	
	/* 创建MME通信的二层socket */
	if( MME_InitSocket(&MME_SK, EOC_PROG_ATHEROS) != 0 )
	{
		perror("ERROR: DiscoverManager MME_InitSocket failed, exit\n");
		pthread_exit(NULL);
	}
	printf("DiscoverManager MME_InitSocket %d successfully\n", MME_SK.skfd);

	while(1)
	{
		pthread_mutex_lock(&(that->mutex));
	
		//printf("DiscoverManager is waiting signal condreq[1]\n");

		/* 阻塞,等待主进程的条件信号*/
		pthread_cond_wait(&(that->condreq[1]), &(that->mutex));

		//printf("DiscoverManager signal condreq[1] arrived. Doing something\n");
		/* 在这里完成数据拷贝 */
		memcpy(this, that, sizeof(MMEAD_BBLOCK_QUEUE));
		//printf("DiscoverManager done and send signal condack[1]\n");

		/* 向主进程发送拷贝结束的条件信号*/
		pthread_cond_signal(&(that->condack[1]));
		
		pthread_mutex_unlock(&(that->mutex));

		/* 处理TOP逻辑*/
		MME_ProcessGetTopology(this, &MME_SK);
		
	}

	close(MME_SK.skfd);
	pthread_exit(NULL);
	
}

/********************************************************************************************
*	函数名称:do_notification
*	函数功能:当MMEAD主进程完成消息接收之后调用该函数,
*				  通过发送条件信号通知相应子线程来处理,然后
*				  等待子线程处理完成的条件信号到达之后返回,
*				  此时父进程可以进入下一轮接收状态
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void do_notification(void)
{
	MMEAD_BBLOCK_QUEUE *this = &bblock;
	T_Msg_Header_MMEAD *h = (T_Msg_Header_MMEAD *)(this->b);

	pthread_mutex_lock(&(this->mutex));
	switch(h->MM_TYPE)
	{
		/* 多线程中不同的线程是共享SOCKET接收缓冲区的，
		为了避免出现残余消息，需要注意清空*/
		/* 这里我们实际上只用了一个线程来处理与设备的通信，
		避免线程中出现残余消息*/
		case MMEAD_MAX_REQ:
			//printf("case MMEAD_GET_TOPOLOGY: do_notification->pthread_cond_signal\n");
			pthread_cond_signal(&(this->condreq[1]));
			pthread_cond_wait(&(this->condack[1]), &(this->mutex));
			//printf("case MMEAD_GET_TOPOLOGY: do_notification->pthread_cond_wait->finished\n");
			break;
		default:
			//printf("default: do_notification->pthread_cond_signal\n");
			pthread_cond_signal(&(this->condreq[0]));
			pthread_cond_wait(&(this->condack[0]), &(this->mutex));
			//printf("default: do_notification->pthread_cond_wait->finished\n");
			break;
	}
	pthread_mutex_unlock(&(this->mutex));
}
#endif

/********************************************************************************************
*	函数名称:MMEAD_ProcForExit
*	函数功能:MMEAD进程的异常处理句柄函数，完成异常终止
*				   之前的资源回收工作
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
void MMEAD_ProcForExit(int n)
{	
	MMEAD_BBLOCK_QUEUE *this = &bblock;

//#ifdef MMEAD_SUPPORT_MUL_THREADS
#if 0
	int i = 0;
	// wait till the thread dies
	pthread_join(this->tid[0], NULL);
	pthread_join(this->tid[1], NULL);

	
	
	/* 注销线程锁和同步信号量 */
	for( i=0; i<MMEAD_MAX_CHILD_NUM; i++ )
	{
		pthread_cond_destroy(&(this->condack[i]));
		pthread_cond_destroy(&(this->condreq[i]));
	}
	pthread_mutex_destroy(&(this->mutex));

	//printf("Threads exited !\n");
	
#endif
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "MMEAD_ProcForExit : module mmead exit");
	dbsClose(dbsdev);
	/* 关闭socket接口 */
	close(this->sk.skfd);
	
	exit(0);
}

/********************************************************************************************
*	函数名称:main
*	函数功能:MMEAD主入口函数，接收来自外部模块的请求，
*				   完成与CLT/CNU的MME交互之后把结果响应给外部
*				   模块
*	作者:frank
*	时间:2010-07-22
*********************************************************************************************/
int main(void)
{
	//int i = 0;
	int rev_len = 0;
	int FromAddrSize;
	struct sockaddr_in MmeadAddr;
	T_Msg_Header_MMEAD *h = NULL;
	MMEAD_BBLOCK_QUEUE *this = &bblock;

	/*创建与数据库模块互斥通讯的外部SOCKET接口*/
	dbsdev = dbsOpen(MID_MMEAD);
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: mmead->dbsOpen error, exited !\n");
		return CMM_CREATE_SOCKET_ERROR;
	}

	signal(SIGTERM, MMEAD_ProcForExit);
	if( CMM_SUCCESS != boardapi_macs2b(boardapi_getMacAddress(), OSA) )
	//if( get_mac_addr(LOCAL_INTERFACE0, OSA) != 0 )
	{
		fprintf(stderr, "ERROR: mmead->get_mac_addr, exit !\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "mmead get_mac_addr error, exited");
		dbsClose(dbsdev);
		return -1;
	}	

//#ifdef MMEAD_SUPPORT_MUL_THREADS
#if 0
	/* 初始化线程锁和同步信号量 */
	for( i=0; i<MMEAD_MAX_CHILD_NUM; i++ )
	{
		pthread_cond_init(&(this->condack[i]), NULL);
		pthread_cond_init(&(this->condreq[i]), NULL);
	}
	pthread_mutex_init(&(this->mutex), NULL);
	
#endif

	/*创建外部SOCKET接口*/
	if( ( this->sk.skfd = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		fprintf(stderr, "ERROR: mmead->socket, exit !\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "mmead create socket error, exited");
		dbsClose(dbsdev);
		return -1;
	}

	bzero((char *)&MmeadAddr, sizeof(MmeadAddr));
       MmeadAddr.sin_family = PF_INET;
       MmeadAddr.sin_port = htons(MMEAD_LISTEN_PORT);
       MmeadAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(this->sk.skfd, (struct sockaddr*)&MmeadAddr, sizeof(MmeadAddr))<0)
	{
		fprintf(stderr, "ERROR: mmead->bind, exit !\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "mmead binding socket error, exited");
		dbsClose(dbsdev);
		return -1;
	}

//#ifdef MMEAD_SUPPORT_MUL_THREADS
#if 0
	/* 初始化通用MME请求消息处理线程 */
	if( pthread_create( &(this->tid[0]), NULL, (void *)ComReqManager, NULL ) == -1 )
	{
		perror("ERROR: MmeManager Create thread ComReqManager failed, exit\n");
		return -1;
	}
	
	/* 初始化设备发现MME请求消息处理线程 */
	if( pthread_create( &(this->tid[1]), NULL, (void *)DiscoverManager, NULL ) == -1 )
	{
		perror("ERROR: MmeManager Create thread DiscoverManager failed, exit\n");
		return -1;
	}
#else
	T_MME_SK_HANDLE MME_SK;
	/* 创建MME通信的二层socket */
	if( MME_InitSocket(&MME_SK, EOC_PROG_ATHEROS) != 0 )
	{
		fprintf(stderr, "ERROR: mmead->MME_InitSocket, exit !\n");
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "mmead init mme socket error, exited");
		dbsClose(dbsdev);
		return -1;
	}

#endif
	
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting module mmead success");
	printf("Starting module MMEAD		......		[OK]\n");

	FromAddrSize = sizeof(this->sk.from);
	/* 父进程循环接收外部消息并通知相应子线程处理 */
	while(1)
	{
//#ifdef MMEAD_SUPPORT_MUL_THREADS
#if 0
		/* 在接收数据到缓冲时先加锁 */
		pthread_mutex_lock(&(this->mutex));

		bzero(this->b, MAX_UDP_SIZE);
		
		/*这里都假设fragment字段为0，不考虑有消息分片的情况*/
		rev_len = recvfrom(this->sk.skfd, this->b, MAX_UDP_SIZE, 0, 
							(struct sockaddr *)&(this->sk.from), &FromAddrSize);

		/* 数据接收完成之后解锁 */
		pthread_mutex_unlock(&(this->mutex));
#else
		bzero(this->b, MAX_UDP_SIZE);
		/*这里都假设fragment字段为0，不考虑有消息分片的情况*/
		rev_len = recvfrom(this->sk.skfd, this->b, MAX_UDP_SIZE, 0, 
							(struct sockaddr *)&(this->sk.from), &FromAddrSize);
#endif
		
		if ( -1 == rev_len )
		{
			fprintf(stderr, "ERROR: mmead->recvfrom, continue !\n");
			dbs_sys_log(dbsdev, DBS_LOG_ERR, "mmead call recvfrom error, continue");
		}		
		else
		{
			h = (T_Msg_Header_MMEAD *)(this->b);
			if( MMEAD_MSG_ID != h->M_TYPE )
			{
				/* 对于MMEAD_MSG_ID不正确的请求不作应答，请求方
				等待接收时需要进行select，以免进程阻塞*/
				fprintf(stderr, "ERROR: mmead->recvfrom[Non-matched msg], skip !\n");
				dbs_sys_log(dbsdev, DBS_LOG_WARNING, "Non-matched msg revieved by MMEAD, skip");
			}
			else
			{
				/* 打印接收到的报文*/
				mmead_debug_printf("\n====> MMEAD RECIEVED MASSAGE:\n");
				debug_dump_msg(this->b, rev_len, stderr);
//#ifdef MMEAD_SUPPORT_MUL_THREADS
#if 0
				/* 通过发送条件信号通知相应子线程来处理请求 */
				do_notification();
#else
				ComReqManager(&MME_SK);
#endif
			}
		}
		
	}

	/* 不要在这个后面添加代码，执行不到滴*/
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "module mmead exit");
	dbsClose(dbsdev);
	return 0;
}

