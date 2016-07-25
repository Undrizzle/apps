#ifndef __MME_API_H__
#define __MME_API_H__

#include "support/atheros/ihpapi/ihpapi.h"
#include "support/atheros/ihpapi/ihp.h"

/********************************************************************************************
*	函数名称:MME_Atheros_MsgNeRefresh
*	函数功能:更新在线网元节点信息，排除已经下线的设备
*				   
*	返回值:无
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
void MME_Atheros_MsgNeRefresh
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_TOPOLOGY *pNEList);

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetNetWorkInfo
*	函数功能:ihpapi_GetNetworkInfo
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetNetWorkInfo
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_TOPOLOGY *pNEList);


/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetNetWorkInfoStats
*	函数功能:ihpapi_GetNetworkInfoStats
*				   
*	返回值:操作是否成功的状态码
*	作者:Stan
*	时间:2013-03-12
*********************************************************************************************/
int MME_Atheros_MsgGetNetWorkInfoStats
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_TOPOLOGY *pNEList);


/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetManufacturerInfo
*	函数功能:ihpapi_GetManufacturerInfo
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetManufacturerInfo
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint8_t *pManufacturerInfo);

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetSwVer
*	函数功能:获取设备的软件版本
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetSwVer
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], uint8_t *pStr);

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetCltMac
*	函数功能:获取CLT的MAC地址
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetCltMac(T_MME_SK_HANDLE *MME_SK, uint8_t clt_mac[]);

/********************************************************************************************
*	函数名称:MME_Atheros_MsgResetDevice
*	函数功能:ihpapi_ResetDevice
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgResetDevice(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[]);

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetTopology
*	函数功能:获取网元节点信息
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetTopology
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_TOPOLOGY *pNEList);

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetTopology
*	函数功能:获取网元节点信息
*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgGetTopologyStats
(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], T_MMEAD_TOPOLOGY *pNEList);


/********************************************************************************************
*	函数名称:MME_Atheros_MsgGetRxToneMapInfo
*	函数功能:获取网元节点信息
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
);

/********************************************************************************************
*	函数名称:MME_Atheros_MsgGet74RxToneMapInfo
*	函数功能:获取网元节点信息
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
);
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
);

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
);

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
);

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
);

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
);

int MME_Atheros_MsgGetFrequencyBandSelection
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	T_MMEAD_FBS *pdata
);

int MME_Atheros_MsgSetFrequencyBandSelection
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	T_MMEAD_FBS *pdata
);

int MME_Atheros_MsgGetRtl8306eVlanConfig
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	st_cnuSwitchVlanConfig *vlanInfo
);

int MME_Atheros_MsgGetRtl8306eBandwidthConfig
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	st_cnuSwitchBandwidthConfig *bandwidthInfo
);

int MME_Atheros_MsgGetRtl8306eLoopDetect
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	st_cnuSwitchLoopDetect *loopDetect
);

/********************************************************************************************
*	函数名称:MME_Atheros_MsgDirectWriteModule
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgDirectWriteModule
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	uint8_t *mod, 
	uint32_t len
);

/********************************************************************************************
*	函数名称:MME_Atheros_MsgEraseModule
*	函数功能:*				   
*	返回值:操作是否成功的状态码
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int MME_Atheros_MsgEraseModule
(
	T_MME_SK_HANDLE *MME_SK, 
	uint8_t ODA[], 
	T_MMEAD_ERASE_MOD_REQ_INFO *erase
);

#endif

