/*====================================================================*
 *   
 *   ihpapi.h - Intellon API External Definitions and Declarations;
 * 
 *   This file is one component of the Intellon Application Program
 *   Interface. It defines all constants and declares variables and
 *   functions accessible to application programs;
 *
 *   Read the Intellon HomePlug AV Application Programming Interface
 *   User's Guide for more information;
 *
 *   This software and documentation is the property of Intellon 
 *   Corporation, Ocala, Florida. It is provided 'as is' without 
 *   expressed or implied warranty of any kind to anyone for any 
 *   reason. Intellon assumes no responsibility or liability for 
 *   errors or omissions in the software or documentation and 
 *   reserves the right to make changes without notification. 
 *   
 *   Intellon customers may modify and distribute the software 
 *   without obligation to Intellon. Since use of this software 
 *   is optional, users shall bear sole responsibility and 
 *   liability for any consequences of it's use. 
 *
 *.  Intellon HomePlug AV Application Programming Interface;
 *:  Published 2007 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use;
 *
 *   Contributor(s): 
 *	Alex Vasquez, alex.vasquez@intellon.com
 *	Charles Maier, charles.maier@intellon.com
 *  
 *--------------------------------------------------------------------*/

#ifndef IHPAPI_HEADER
#define IHPAPI_HEADER
 
/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/
 
/*! Normally, we would include unistd.h to obtain the definition of
 *  size_t but not all system have that so stdio.h does the trick;
 */

#include <stdio.h>
#include <stdint.h>
#include <public.h>
 
/*====================================================================*
 *   
 *--------------------------------------------------------------------*/

#include "../tools/types.h"
 
/*====================================================================*
 *  IHPAPI version;
 *--------------------------------------------------------------------*/

#define IHPAPI_VERSION "Intellon HomePlug AV API Version 2.2.0.1"
 
/*====================================================================*
 *   Definitions used by IHPAPI;
 *--------------------------------------------------------------------*/

#define SAMPLE_DEBUG 			0	

#define DBG_HEX_DUMP_BUFF(s,l)		{if(SAMPLE_DEBUG) hexdump (s, l, stdout);}
#define DBG_PRINTF(s ...)				{if(SAMPLE_DEBUG) printf(s);}
#define ERROR_PRINTF(s ...)			{printf(s);}

/*! The maximum number of bytes in the NMK and DAK values;
 */

#define KEY_MAX_LEN                 16
 
/*! The maximum number of bytes in the Link Statistics result;
 */

#define LSTATS_MAX_LEN              287
 
/*! The maximum number of bytes in a string;
 */

#define STRNG_MAX_LEN               64
 
/*! The maximum number of modulation carriers;
 */

#define MOD_CARRIER_MAX_NUM         1156
 
/*! The maximum numbers of bytes containing two modulation carriers in each nibble of a byte;
 */

#define MOD_CARRIER_MAX_TUPLE_NUM   (MOD_CARRIER_MAX_NUM/2)


/*! Initialize Device Options */

#define INT6K_FLASH           35
#define INT6K_NOFLASH         34
#define NOINT6K_FW_FLASH      19
#define NOINT6K_NOFW          0
 
/*====================================================================*
 *   The following definitions allow IPv4 portability between different
 *   OS and development environment. These were extracted from
 *   net/ethernet and linux/if_ether;
 *--------------------------------------------------------------------*/
 
/*! Octets in one ethernet addr;
 */

#define IHPAPI_ETH_ALEN	6
 
/*! Total octets in header;	 
 */

#define IHPAPI_ETH_HLEN	14
 
/*! Min. octets in frame sans FCS; 
 */

#define IHPAPI_ETH_ZLEN	60
 
/* Max. octets in payload	 
 */

#define IHPAPI_ETH_DATA_LEN	1500
 
/*! Max. octets in frame sans FCS; 
 */

#define IHPAPI_ETH_FRAME_LEN	1514
 
/*! size of ethernet addr; 
 */

#define	IHPAPI_ETHER_ADDR_LEN	IHPAPI_ETH_ALEN
 
/*! bytes in type field; 
 */

#define	IHPAPI_ETHER_TYPE_LEN	2
 
/*! bytes in CRC field; 
 */

#define	IHPAPI_ETHER_CRC_LEN	4
 
/*! total octets in header; 
 */

#define	IHPAPI_ETHER_HDR_LEN	IHPAPI_ETH_HLEN
 
/*! min packet length; 
 */

#define	IHPAPI_ETHER_MIN_LEN	(IHPAPI_ETH_ZLEN + IHPAPI_ETHER_CRC_LEN)
 
/*! max packet length; 
 */

#define	IHPAPI_ETHER_MAX_LEN	(IHPAPI_ETH_FRAME_LEN + IHPAPI_ETHER_CRC_LEN)

#define INT6K_BLOCKSIZE 1024
#define VS_MODULE_PIB   (1 << 1)
 
/*====================================================================*
 *   structure passed to EncodeLinkStats encoding function;
 *--------------------------------------------------------------------*/
 
/*! This structure contains information needed to encode a vs_LNK_STATS
 *  request message by function ihpapi_getConnectionInfo;
 *  see the INT6000 Firmware Technical Reference Manual
 *  for more information;
 */
 
typedef struct __packed ihpapi_connectCtl_s 

{
	uint8_t mcontrol;
	uint8_t direction;
	uint8_t lid;
	uint8_t macaddress [IHPAPI_ETHER_ADDR_LEN];
}

ihpapi_connectCtl_t;

/*====================================================================*
 *   structure passed to EncodeEnetPHYSettings encoding function;
 *--------------------------------------------------------------------*/
 
/*! This structure contains information needed to encode a 
 *  VS_ENET_SETTINGS request message by function ihpapi_ManageEnetPhy;
 *  see the INT6000 Firmware Technical Reference Manual
 *  for more information;
 */
 
typedef struct __packed ihpapi_enetPHYCtl_s 

{
	uint8_t mcontrol;
	uint8_t autonegotiate;
	uint8_t advcaps;
	uint8_t espeed;
	uint8_t eduplex;
	uint8_t eflowcontrol;
}

ihpapi_enetPHYCtl_t;

/*====================================================================*
 *   structure passed to EncodeToneMapCharacteristics encoding functions;
 *--------------------------------------------------------------------*/
 
/*! This structure contains information needed to encode a 
 *  VS_TONE_MAP_CHAR and VS_RX_TONE_MAP_CHAR request message by function 
 *  ihpapi_GetToneMapInfo;
 *  see the INT6000 Firmware Technical Reference Manual
 *  for more information;
 */
 
typedef struct __packed ihpapi_toneMapCtl_s 

{
	uint8_t macaddress [IHPAPI_ETHER_ADDR_LEN];
	uint8_t tmslot;
}

ihpapi_toneMapCtl_t;



/*====================================================================*
 *   structure passed to EncodeSetKey encoding function;
 *--------------------------------------------------------------------*/
 
/*! This structure contains information needed to encode a VS_SET_KEY
 *  request message by function ihpapi_SetKey;
 *  see the INT6000 Firmware Technical Reference Manual
 *  for more information;
 */
 
typedef struct __packed ihpapi_key_s 

{
	uint8_t nmk [KEY_MAX_LEN];
	uint8_t peks;
	uint8_t rda [IHPAPI_ETHER_ADDR_LEN];
	uint8_t dak [KEY_MAX_LEN];
}

ihpapi_key_t;

/*====================================================================*
 *   Operation Codes returned by RxFrame;
 *--------------------------------------------------------------------*/
 
/*! This enumeration defines operation codes used by the API to 
 *  communicate operational state information;
 */
 
typedef enum ihpapi_opCode_e 

{
	IHPAPI_OPCODE_NOOP = 0,
	IHPAPI_OPCODE_GET_CONNECTION_INFO = 1,
	IHPAPI_OPCODE_GET_MANUFACTURER_INFO = 2,
	IHPAPI_OPCODE_GET_NETWORK_INFO = 3,
	IHPAPI_OPCODE_GET_VERSION_INFO = 4,
	IHPAPI_OPCODE_RESET_DEVICE = 5,
	IHPAPI_OPCODE_HOST_ACTION_REQUEST = 6,
	IHPAPI_OPCODE_INITIALIZE_DEVICE = 7,
	IHPAPI_OPCODE_SET_KEY = 8,
	IHPAPI_OPCODE_UPDATE_DEVICE = 9,
	IHPAPI_OPCODE_GET_TONE_MAP_INFO = 10,
	IHPAPI_OPCODE_MANAGE_ENET_PHY = 11,
	IHPAPI_OPCODE_SET_SDRAM_CONFIG = 12,
	IHPAPI_OPCODE_RESET_TO_FACTORY_DEFAULTS = 13,
	IHPAPI_OPCODE_GET_RX_TONE_MAP_INFO = 14,
	IHPAPI_OPCODE_SET_CLASSIFICATION_INFO = 15,
	IHPAPI_OPCODE_MDIO_COMMAND_INFO = 16,
	IHPAPI_OPCODE_GET_NETWORK_INFO_STATS = 17,
	IHPAPI_OPCODE_MAX 
}

ihpapi_opCode_t;

/*====================================================================*
 *   Operation completion codes returned by RxFrame;
 *--------------------------------------------------------------------*/
 
/*! This enumeration defines completion codes used by the API to 
 *  communicate operational state;
 */
 
typedef enum ihpapi_opCmpltCode_e 

{
	IHPAPI_OPCMPLTCODE_NOOP = 0,
	IHPAPI_OPCMPLTCODE_TRANSMIT = 1,
	IHPAPI_OPCMPLTCODE_COMPLETE = 2,
	IHPAPI_OPCMPLTCODE_MAX 
}

ihpapi_opCmpltCode_t;

/*====================================================================*
 *   Operation Status Information;
 *--------------------------------------------------------------------*/
 
/*! This structure contains the MMTYPE and MMSTATUS fields of Intellon
 *  vendor specific messages decoded by function RxFrame or an illegal
 *  MTYPE value and POSIX error code on decoding error; error codes are 
 *  also available in global variable errno and functions perror and
 *  strerror can be used to display a message;
 *  see IEEE Std 1003.1 2004 for more information;
 */
 
typedef struct ihpapi_opStatus_s 

{
	uint16_t type;
	uint8_t status;
}

ihpapi_opStatus_t;

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/
 
/*! This structure contains information decoded from a VS_LNK_STATS
 *  message by function RxFrame;
 *  see the INT6000 Firmware Technical Reference Manual for more 
 *  information;
 */
 
typedef struct __packed ihpapi_getConnectInfoData_s 

{
	uint8_t direction;
	uint8_t lid;
	uint8_t tei;
	uint8_t lstats [LSTATS_MAX_LEN];
}

ihpapi_getConnectInfoData_t;

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/
 
/*! This structure contains information decoded from a VS_MFG_STRING
 *  message by function RxFrame; 
 *  see the INT6000 Firmware Technical Reference Manual 
 *  for more information;
 */
 
typedef struct __packed ihpapi_getManufacturerInfoData_s 

{
	uint8_t mstrlength;
	uint8_t mstring [STRNG_MAX_LEN];
}

ihpapi_getManufacturerInfoData_t;

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/
 
/*! This structure contain information decoded from a VS_NW_INFO
 *  message by function RxFrame; 
 *  see the INT6000 Firmware Technical Reference Manual 
 *  for more information;
 */


 
typedef struct __packed ihpapi_getNetworkInfoData_s 

{
	uint8_t numavlns;
	uint8_t nwinfo [IHPAPI_ETHER_MAX_LEN - IHPAPI_ETHER_HDR_LEN];
}

ihpapi_getNetworkInfoData_t;

typedef struct __packed ihpapi_getFrequencyBandSelection_s 
{
	uint8_t FBSTATUS;
	uint16_t START_BAND;
	uint16_t STOP_BAND;	
}
ihpapi_getFrequencyBandSelection_t;

typedef struct __packed ihpapi_getTxGain_s 
{
	uint8_t TX_GAIN;
}
ihpapi_getTxGain_t;

typedef struct __packed ihpapi_getUserHFID_s 
{
	uint8_t USER_HFID[64];
}
ihpapi_getUserHFID_t;

typedef struct __packed ihpapi_getHgManage_s
{
	uint8_t wan_name[255];
	uint8_t vlan[2];
	uint8_t priority;
	uint8_t ip_assigned_mode;
	uint8_t connection_protocol;
	uint8_t user_name[255];
	uint8_t password[255];
	uint8_t ipv4_addr[4];
	uint8_t ipv4_mask[4];
	uint8_t ipv4_gw[4];
	uint8_t dns_ipv4[4];
	uint8_t dns_ipv4_copy[4];
}
ihpapi_getHgManage_t;

typedef struct __packed ihpapi_getHgBusiness_s
{
	uint8_t wan_name_1[100];
	uint8_t vlan_1[2];
	uint8_t priority_1;
	uint8_t connection_mode_1;
	uint8_t ip_assigned_mode_1;
	uint8_t service_type_1;
	uint8_t connection_protocol_1;
	uint8_t bind_interface_1[4];
	uint8_t user_name_1[100];
	uint8_t password_1[100];
	uint8_t ipv4_addr_1[4];
	uint8_t ipv4_mask_1[4];
	uint8_t ipv4_gw_1[4];
	uint8_t dns_ipv4_1[4];
	uint8_t dns_ipv4_copy_1[4];
	uint8_t wan_name_2[100];
	uint8_t vlan_2[2];
	uint8_t priority_2;
	uint8_t connection_mode_2;
	uint8_t ip_assigned_mode_2;
	uint8_t service_type_2;
	uint8_t connection_protocol_2;
	uint8_t bind_interface_2[4];
	uint8_t user_name_2[100];
	uint8_t password_2[100];
	uint8_t ipv4_addr_2[4];
	uint8_t ipv4_mask_2[4];
	uint8_t ipv4_gw_2[4];
	uint8_t dns_ipv4_2[4];
	uint8_t dns_ipv4_copy_2[4];
}
ihpapi_getHgBusiness_t;

typedef struct __packed ihpapi_getHgSsid_s
{
	uint8_t ssid_status;
	uint8_t ssid_name1[100];
	uint8_t ssid_name2[100];
	uint8_t ssid_name3[100];
	uint8_t ssid_name4[100];
}
ihpapi_getHgSsid_t;

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/
 
/*! This structure contain information decoded from a VS_NW_INFO
 *  message by function RxFrame; 
 *  see the INT74107.1.2 v1 Firmware Technical Reference Manual 
 *  for more information;
 *  add by stan
 */
 
 	typedef struct __packed stav1_s  {
		uint8_t DA[6];
		uint8_t TEI;
		uint8_t RESERVED[3];
		uint8_t F1STBDA[6];
		uint8_t AVGPHYDR_TX[2];
		uint8_t TR_COUPLING;
		uint8_t RESERVED1;
		uint8_t AVGPHYDR_RX[2];
		int8_t REVERSEVED2[2];
	}
	 v1sta_t;
	
 
typedef struct __packed ihpapi_getNetworkInfoStatsData_s 

{	
	  uint8_t MME_SUBVER;
		uint8_t RESERVED;
		uint8_t MME_DATA_LEN[2];
		uint8_t FIRST_TEI;
		uint8_t NUM_STAS;
		uint8_t RESERVED1[2];
		uint8_t IN_AVLIN;
		uint8_t NID[7];
		uint8_t RESERVED2;
		uint8_t SNID;
		uint8_t TEI;
		uint8_t RESERVED3[2];
		uint8_t STATIONROLE;
		uint8_t CCO_MACADDR[6];
		uint8_t ACCESS;
		uint8_t NUMCORDNWS;
		uint8_t CCO_TEI;
		uint8_t REVERSEVED4[7];
		uint8_t nwinfostats[IHPAPI_ETHER_MAX_LEN - IHPAPI_ETHER_HDR_LEN - 38]  ; 
}

ihpapi_getNetworkInfoStatsData_t;
/*====================================================================*
 *   
 *--------------------------------------------------------------------*/
 
/*! This structure contains information decoded from a VS_SW_VER
 *  message by function RxFrame; 
 *  see the INT6000 Firmware Technical Reference Manual 
 *  for more information;
 */
 
typedef struct __packed ihpapi_getVersionData_s 

{
	uint8_t mdeviceid;
	uint8_t mverlength;
	uint8_t mversion [2*STRNG_MAX_LEN];
	uint8_t upgradable;
}

ihpapi_getVersionData_t;

/*====================================================================*
 *   Host Action Request returned by RxFrame;
 *--------------------------------------------------------------------*/
 
/*! This enumeration defines host action requests used by the API to 
 *  communicate host action information;
 */
 
typedef enum ihpapi_hostActionRequest_e 

{
	IHPAPI_HAR_INITIALIZE_DEVICE = 0x00,
	IHPAPI_HAR_UPLOAD_FW = 0x01,
	IHPAPI_HAR_UPLOAD_PB = 0x02,
	IHPAPI_HAR_UPLOAD_FWnPB = 0x03,
	IHPAPI_HAR_INITIALIZE_DEVICE_SDRAMCFG = 0x04, 
	IHPAPI_HAR_RESET_TO_FACTORY_DEFAULT = 0x05,
	IHPAPI_HAR_MAX
}

ihpapi_hostActionRequest_t;

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/
 
/*! This structure contains information decoded from a VS_HOST_ACTION
 *  message by function RxFrame; See the INT6000 Firmware 
 *  Technical Reference Manual for more 
 *  information;
 */
 
typedef struct ihpapi_hostActionRequestData_s 

{
	ihpapi_hostActionRequest_t request;
}

ihpapi_hostActionRequestData_t;

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/
 
/*! This structure contains information decoded from a VS_RD_MOD
 *  message by function RxFrame; See the INT6000
 *  Firmware Technical Reference Manual for more information;
 */
 
typedef struct __packed ihpapi_updateDeviceData_s 

{
	ihpapi_hostActionRequest_t type;
	bool eof;
	size_t bufferLen;
	uint8_t buffer [IHPAPI_ETHER_MAX_LEN - IHPAPI_ETHER_HDR_LEN];
}

ihpapi_updateDeviceData_t;

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/
 
/*! This structure contains information decoded from a VS_ENET_SETTINGS
 *  message by function RxFrame; See the INT6000 
 *  Firmware Technical Reference Manual for more information;
 */
 
typedef struct __packed ihpapi_manageEnetPHYData_s 

{
	uint8_t espeed;
	uint8_t eduplex;
	uint8_t elinkstatus;
	uint8_t eflowcontrol;
}

ihpapi_manageEnetPHYData_t;

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/
 
/*! This structure contains information decoded from a VS_TONE_MAP_CHAR
 *  message by function RxFrame; See the INT6000 
 *  Firmware Technical Reference Manual for more information;
 */
 
typedef struct __packed ihpapi_getToneMapData_s 

{
	uint8_t tmslot;
	uint8_t numtms;
	uint16_t tmnumactcarriers;
	uint8_t mod_carrier [MOD_CARRIER_MAX_TUPLE_NUM];
}

ihpapi_getToneMapData_t;

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/
 
/*! This structure contains information decoded from a VS_RX_TONE_MAP_CHAR
 *  message by function RxFrame; See the INT6000 
 *  Firmware Technical Reference Manual for more information;
 */
 
typedef struct __packed ihpapi_getRxToneMapData_s 

{
	uint8_t tmslot;
	uint8_t numtms;
	uint16_t tmnumactcarriers;
	uint8_t mod_carrier [MOD_CARRIER_MAX_TUPLE_NUM];
	uint8_t gil;
	uint8_t avg_agc_gain;
}

ihpapi_getRxToneMapData_t;

typedef struct __packed ihpapi_mdioCommandData_s 

{
	uint16_t value;
}

ihpapi_mdioCommandData_t;

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/
 
/*! Function RxFrame returns information in this structure by decoding
 *  incoming Etherenet frames as Intellon vendor specific messages; if
 *  a message contains more information than status, applications can 
 *  access that information using an appropriate union;
 */
 
typedef struct __packed ihpapi_result_s 

{
	ihpapi_opCode_t opCode;
	ihpapi_opCmpltCode_t opCompltCode;
	ihpapi_opStatus_t opStatus;
	uint8_t ODA [IHPAPI_ETHER_ADDR_LEN];
	uint8_t OSA [IHPAPI_ETHER_ADDR_LEN];
	bool validData;
	size_t dataLen;
	union data 
	{
		ihpapi_getConnectInfoData_t connInfo;
		ihpapi_getManufacturerInfoData_t mftrInfo;
		ihpapi_getNetworkInfoData_t netInfo;
		ihpapi_getNetworkInfoStatsData_t netInfoStats;
		ihpapi_getVersionData_t version;
		ihpapi_hostActionRequestData_t hostAction;
		ihpapi_updateDeviceData_t updateDev;
		ihpapi_manageEnetPHYData_t enetPHY;
		ihpapi_getToneMapData_t toneMap;
		ihpapi_getRxToneMapData_t rxToneMap;
		ihpapi_mdioCommandData_t mdioInfo;
		ihpapi_getFrequencyBandSelection_t FrequencyBandSelectionInfo;
		ihpapi_getTxGain_t txGainInfo;
		ihpapi_getUserHFID_t UserHFIDInfo;
		ihpapi_getHgManage_t hgmanageInfo;
		ihpapi_getHgBusiness_t hgbusinessInfo;
		ihpapi_getHgSsid_t hgSsidInfo;
	}
	data;
}

ihpapi_result_t;

#pragma pack (push, 1)
typedef struct __packed ClassifierInfo
{
	uint8_t MCONTROL;
	uint8_t VOLATILITY;
	uint8_t ACTION;
	uint8_t OPERAND;
	uint8_t NUM_CLASSIFIERS;
	uint8_t CR_ID_1;
	uint8_t CR_OPERAND_1;
	uint8_t CR_VALUE_1[16];
	uint8_t CR_ID_2;
	uint8_t CR_OPERAND_2;
	uint8_t CR_VALUE_2[16];
	uint8_t CR_ID_3;
	uint8_t CR_OPERAND_3;
	uint8_t CR_VALUE_3[16];
}

ClassifierInfo;

typedef struct __packed MdioCmdInfo
{
	uint8_t OPERATION;
	uint8_t PHY_ADDR;
	uint8_t REGISTER_ADDR;
	uint16_t DATA;
}

MdioCmdInfo;





typedef struct __packed vs_module_operation_spec 
{
	uint16_t MODULE_ID;
	uint16_t MODULE_SUB_ID;
	uint32_t MODULE_LENGTH;
	uint32_t MODULE_CHKSUM;
}vs_module_operation_spec;

typedef struct vs_module_operation_data 
{
	uint16_t MOD_STATUS;
	uint16_t ERR_REC_CODE;
}vs_module_operation_data;

#pragma pack (pop)

/*====================================================================*
 *   encoder functions; see the Intellon HomePlug AV
 *   Application Programming Interface Manual for more information;
 *--------------------------------------------------------------------*/
size_t ihpapi_Bandwidthlimiting (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [],uint8_t direction,uint8_t speed); 
size_t ihpapi_GetConnectionInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_connectCtl_t * inputConnectInfo);
size_t ihpapi_GetManufacturerInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
size_t ihpapi_GetNetworkInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
size_t ihpapi_Get74NetworkInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
size_t ihpapi_GetNetworkInfoStats (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
size_t ihpapi_GetVersionInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
size_t ihpapi_GetToneMapInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_toneMapCtl_t * inputToneMapInfo);
size_t ihpapi_Get74ToneMapInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_toneMapCtl_t * inputToneMapInfo);
size_t ihpapi_GetRxToneMapInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_toneMapCtl_t * inputToneMapInfo);
size_t ihpapi_Get74RxToneMapInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_toneMapCtl_t * inputToneMapInfo);
size_t ihpapi_ManageEnetPHY (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_enetPHYCtl_t * inputENETPHYInfo);
size_t ihpapi_ResetDevice (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
size_t ihpapi_ResetToFactoryDefaults (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
size_t ihpapi_SetKey (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_key_t * pkeyParm);
size_t ihpapi_SetSDRAMConfig (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], size_t SDRAM_len, uint8_t SDRAM_buffer []);
size_t ihpapi_SetClassification (uint8_t sa [], uint8_t da [], ClassifierInfo *pClassifierInfo, size_t bufferLen, uint8_t buffer []) ;
size_t ihpapi_MdioCommand ( uint8_t sa [], uint8_t da [], MdioCmdInfo*pMdioCmdInfo, size_t bufferLen, uint8_t buffer [] ) ;
size_t ihpapi_GetFrequencyBandSelection (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []) ;
size_t ihpapi_SetFrequencyBandSelection(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_getFrequencyBandSelection_t *FrequencyBandSelectionInfo);
size_t ihpapi_GetTxGain(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
size_t ihpapi_SetTxGain(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], uint8_t tx_gain);
size_t ihpapi_GetUserHFID(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
//size_t ihpapi_SetUserHFID(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], uint8_t user_hfid[]);
size_t ihpapi_GetHgManage(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
size_t ihpapi_GetHgBusiness(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
size_t ihpapi_SetHgManage(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], T_szHgManage manage);
size_t ihpapi_SetHgBusiness(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], T_szSetHgBusiness manage);
size_t ihpapi_RebootHg(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
size_t ihpapi_ResetHg(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
size_t ihpapi_GetHgSsidStatus(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
size_t ihpapi_SetHgSsidStatus(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], uint8_t ssid[4]);
/*====================================================================*
 *   decoder functions; see the Intellon HomePlug AV
 *   Application Programming Interface Manual for more information;
 *--------------------------------------------------------------------*/
 
int ihpapi_RxFrame (size_t length, uint8_t buffer [], ihpapi_result_t * result);
int ihpapi_v1_RxFrame (size_t length, uint8_t buffer [], ihpapi_result_t * result); 

/*====================================================================*
 *   sequencer functions; see the Intellon HomePlug AV
 *   Application Programming Interface Manual for more information;
 *--------------------------------------------------------------------*/
 
size_t ihpapi_TxFrame (size_t bufferLen, uint8_t buffer []);
int ihpapi_InitializeDevice (uint8_t sa [], uint8_t da [], size_t FW_len, uint8_t FW_pbuffer [], size_t PIB_len, uint8_t PIB_pbuffer [], unsigned options);
bool ihpapi_SequenceInProgress (void);
void ihpapi_SequenceTerminate (void);
int ihpapi_UpdateDevice (uint8_t sa [], uint8_t da [], ihpapi_hostActionRequest_t type);

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif
 

