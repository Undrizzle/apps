/*====================================================================*
 *
 *   ihp.h - Intellon API Internal Definitions and Declarations;
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
 *	Charles Maier, charles.maier@intellon.com
 *	Alex Vasquez, alex.vasquez@intellon.com
 *
 *--------------------------------------------------------------------*/

#ifndef IHP_HEADER
#define IHP_HEADER
 
/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdint.h>
 
/* Byte order functionalities. endian, are reqired by IHPAPI. The
 * following include takes care of Windows 32 and Linux. For other
 * OS, add the appropriate header file.
 */

#if defined (_WIN32)
# include <winsock2.h>
#else
# include <netinet/in.h>
#endif
 
/* The byte ordering of intellon Vendor Specific MMEs content 
 * are little endian, i.e. that it is assumes that the host 
 * is always little endian. The following has been added to 
 * handle the byte ordering of the Intellon VS MMEs.
 *
 * Note: The byte ordering of the Ethernet header, within a 
 * intellon VS MME, is big endian (network).
 */

#if __BYTE_ORDER == __LITTLE_ENDIAN
 
/* The intellon VS MME host byte order is the same as 
 * intellon VS MME network byte order, so these functions 
 * are all just identity.  
 */

# define intohl(x)	(x)
# define intohs(x)	(x)
# define ihtonl(x)	(x)
# define ihtons(x)	(x)
#else
# if __BYTE_ORDER == __BIG_ENDIAN
#  define intohl(x)	__bswap_32 (x)
#  define intohs(x)	__bswap_16 (x)
#  define ihtonl(x)	__bswap_32 (x)
#  define ihtons(x)	__bswap_16 (x)
# endif
#endif
 
/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../ihpapi/ihpapi.h"
#include "../tools/types.h"
#include "../tools/flags.h"
#include "../tools/buffer.h"
 
/*====================================================================*
 *   preprocessor definitions;
 *--------------------------------------------------------------------*/
 
/*! This preprocessor definition enables or disables precautionary code
 *  that some might consider uneccessary once an application is stable;
 *  For example, null address tests and buffer flushing;
 */

#define INTELLON_SAFEMODE 1
 
/*! This preprocessor definition enables or disables exploratory code
 *  used to test new designs or observe internal behaviour; For example,
 *  printf function calls and analytical computations;
 */

#define INTELLON_TESTMODE 1
 
/*! This preprocessor definitions enables or disables alternative code
 *  that may be more maintainable than other code; For example, using
 *  'switch' statements instead of more efficient 'if' statements;
 */

#define INTELLON_MAINTAIN 1
 
/*====================================================================*
 *   HomePlug AV Constants;
 *--------------------------------------------------------------------*/

#define HOMEPLUG_MTYPE 0x88E1
#define HOMEPLUG_MMV 0x01  
#define INTELLON_MMTYPE 0xA000
#define INTELLON_MMV 0x00  
 
/*====================================================================*
 * HomePlug Management Message Ranges for Information Only;
 * See the HomePlug AV Specification for MMTYPE ranges;
 *--------------------------------------------------------------------*/

#if 0
#define CC_MMTYPE_MIN 0x0000
#define CC_MMTYPE_MAX 0x1FFC
#define CP_MMTYPE_MIN 0x2000
#define CP_MMTYPE_MAX 0x3FFC
#define NN_MMTYPE_MIN 0x4000
#define NN_MMTYPE_MAX 0x5FFC
#define CM_MMTYPE_MIN 0x6000
#define CM_MMTYPE_MAX 0x7FFC
#define MS_MMTYPE_MIN 0x8000
#define MS_MMTYPE_MAX 0x9FFC
#define VS_MMTYPE_MIN 0xA000 
#define VS_MMTYPE_MAX 0xBFFC
#define XX_MMTYPE_MIN 0xC000
#define XX_MMTYPE_MAX 0xFFFC
#endif

#define XX_MMTYPE_BAD 0xFFFF
 
/*====================================================================*
 * HomePlug Generic Management Message Types;
 * See the HomePlug AV Specification 1.0.02 for
 * more information;
 * See function MMName() to translate MM codes to MM names;
 *--------------------------------------------------------------------*/

#define CC_CCO_APPOINT 0x0000
#define CC_BACKUP_APPOINT 0x0004
#define CC_LINK_INFO 0x0008
#define CC_HANDOVER 0x000C
#define CC_HANDOVER_INFO 0x0010
#define CC_DISCOVER_LIST 0x0014
#define CC_LINK_NEW 0x0018
#define CC_LINK_MOD 0x001C
#define CC_LINK_SQZ 0x0020
#define CC_LINK_REL 0x0024
#define CC_DETECT_REPORT 0x0028
#define CC_WHO_RU 0x002C
#define CC_ASSOC 0x0030
#define CC_LEAVE 0x0034
#define CC_SET_TEI_MAP 0x0038
#define CC_RELAY 0x003C
#define CC_BEACON_RELIABILITY 0x0040
#define CC_CC_ALLOC_MOVE 0x0044
#define CC_ACCESS_NEW 0x0048
#define CC_ACCESS_REL 0x004C
#define CC_DCPPC 0x0050
#define CC_HP1_DET 0x0054
#define CC_BLE_UPDATE 0x0058
#define CP_PROXY_APPOINT 0x2000
#define PH_PROXY_APPOINT 0x2004
#define CP_PROXY_WAKE 0x2008
#define NN_INL 0x4000
#define NN_NEW_NET 0x4004
#define NN_ADD_ALLOC 0x4008
#define NN_REL_ALLOC 0x400C
#define NN_REL_NET 0x4010
#define CM_ASSOCIATED_STA 0x6000
#define CM_ENCRYPTED_PAYLOAD 0x6004
#define CM_SET_KEY 0x6008
#define CM_GET_KEY 0x600C
#define CM_SC_JOIN 0x6010
#define CM_CHAN_EST 0x6014
#define CM_TM_UPDATE 0x6018
#define CM_AMP_MAP 0x601C
#define CM_BRG_INFO 0x6020
#define CM_CONN_NEW 0x6024
#define CM_CONN_REL 0x6028
#define CM_CONN_MOD 0x602C
#define CM_CONN_INFO 0x6030
#define CM_STA_CAP 0x6034
#define CM_NW_INFO 0x6038
#define CM_GET_BEACON 0x603C
#define CM_HFID 0x6040
#define CM_MME_ERROR 0x6044
#define CM_NW_STATS 0x6048
 
/*====================================================================*
 * Intellon Manufacturer Specufic Management Message Types;
 * See the Intellon Homeplug AV Firmware Technical Reference Manual
 * for more information on Intellon MMEs;
 * This software uses 0xFFFF to indicate an unknown or unwanted MME;
 *--------------------------------------------------------------------*/

#define MS_PB_ENC 0x8000
 
/*====================================================================*
 * Intellon Vendor Specific Management Message Types;
 * See the Intellon Homeplug AV Firmware Technical Reference Manual
 * for more information on Intellon MMEs;
 * This software uses 0xFFFF to indicate an unknown or unwanted MME;
 *--------------------------------------------------------------------*/

#define VS_SW_VER 0xA000
#define VS_WR_MEM 0xA004
#define VS_RD_MEM 0xA008
#define VS_ST_MAC 0xA00C
#define VS_GET_NVM 0xA010
#define VS_RSVD_1 0xA014
#define VS_RSVD_2 0xA018
#define VS_RS_DEV 0xA01C
#define VS_WR_MOD 0xA020
#define VS_RD_MOD 0xA024
#define VS_MOD_NVM 0xA028
#define VS_WD_RPT 0xA02C
#define VS_LNK_STATS 0xA030
#define VS_SNIFFER 0xA034
#define VS_NW_INFO 0xA038
#define VS_RSVD_3 0xA03C
#define VS_CP_RPT 0xA040
#define VS_RSVD_4 0xA044
#define VS_FR_LBK 0xA048
#define VS_LBK_STAT 0xA04C
#define VS_SET_KEY 0xA050
#define VS_MFG_STRING 0xA054
#define VS_RD_CBLOCK 0xA058
#define VS_SET_SDRAM 0xA05C
#define VS_HST_ACTION 0xA060
#define VS_RSVD_5 0xA064
#define VS_OP_ATTRIBUTES 0xA068
#define VS_ENET_SETTINGS 0xA06C
#define VS_TONE_MAP_CHAR 0xA070

/* ar7410 7.1.2 分片机制获取拓扑 */
#define VS_NW_INFO_STATS 0xA074
#define VS_SLAVE_MEM	0xA078
#define VS_FAC_DEFAULT 0xA07C
#define VS_CLASSIFICATION 0xA088
#define VS_RX_TONE_MAP_CHAR 0xA090
#define VS_SET_LED_BEHAVIOR 0xA094
#define VS_BANDWIDTH_LIMITING 0xA0A4
#define VS_MDIO_COMMAND 0xA09C
/* Atheros 6400增加了设备上线主动通知的MME */
#define VS_DEVICEUP_TRAP 0xA0A2
#define VS_MODULE_OPERATION 0xA0B0

#define VS_GET_PROPERTY 0xA0F8
#define VS_SET_PROPERTY 0xA100

#define VS_HOME_GATEWAY_OPERATION 0xAE0C
 
/*====================================================================*
 * HomePlug AV MMEs have 4 variants indicated by the 2 MMTYPE LSBs;
 * See the Intellon Homeplug AV Firmware Technical Reference Manual;
 *--------------------------------------------------------------------*/

#define MMTYPE_REQ 0x00
#define MMTYPE_CNF 0x01
#define MMTYPE_IND 0x02
#define MMTYPE_RSP 0x03

#define MMTYPE_MASK 0xFFFC
#define MMTYPE_MODE 0x0003
 
/*====================================================================*
 *   ethernet frame header only;
 *--------------------------------------------------------------------*/

#pragma pack (push, 1)
 
/*! A standard Ethernet header struct defined using HomePlug AV terminology;
 *  See the Intellon HomePlug AV Firmware Technical Reference
 *  Manual for more information;
 */
 
typedef struct __packed header_eth 

{
	uint8_t ODA [IHPAPI_ETHER_ADDR_LEN];
	uint8_t OSA [IHPAPI_ETHER_ADDR_LEN];
	uint16_t MTYPE;
}

header_eth;

/*! Vendor specific header structure;
 *  See the Intellon Homeplug AV Firmware Technical Reference Manual
 *  for more information;
 */
 
typedef struct __packed header_mme 

{
	uint8_t MMV;
	uint16_t MMTYPE;
	uint8_t OUI [3];
}

header_mme;

/*! Vendor specific header structure; for v1
 *  See the Intellon Homeplug AV Firmware Technical Reference Manual
 *  for more information;
 * add by stan
 */
 
typedef struct __packed header_v1_mme 

{
	uint8_t MMV;
	uint16_t MMTYPE;
	uint8_t FMI[2];
	uint8_t OUI [3];
}

header_v1_mme;

/*! Manufacturer specific header structure; 
 *  See the Intellon HomePlug AV Firmware Technical Reference Manual 
 *  for more information;
 */
 
typedef struct __packed header_mfg 

{
	uint8_t MMV;
	uint16_t MMTYPE;
}

header_mfg;

/*! A manufacturer specific HomePlug AV message frame header defined 
 *  using structures defined above to reduce program clutter;
 */
 
typedef struct __packed header_ms 

{
	struct header_eth ethernet;
	struct header_mfg customer;
}

header_ms;

/*! A vendor specific HomePlug AV message frame header defined 
 *  using structures defined above to reduce program clutter;
 */
 
typedef struct __packed header_vs 

{
	struct header_eth ethernet;
	struct header_mme intellon;
}

header_vs;


//add by stan
/*! A vendor specific HomePlug AV message frame header defined 
 *  using structures defined above to reduce program clutter;
 */
 
typedef struct __packed header_v1_vs 

{
	struct header_eth ethernet;
	struct header_v1_mme intellon;
}

header_v1_vs;

/*
 *
 */
 
typedef struct __packed header_cnf 

{
	struct header_eth ethernet;
	struct header_mme intellon;
	uint8_t MSTATUS;
}

header_cnf;

/*
 *
 */
 
typedef struct __packed header_v1_cnf 

{
	struct header_eth ethernet;
	struct header_v1_mme intellon;
	uint8_t MSTATUS;
}

header_V1_cnf;

typedef struct __packed vs_get_property_req_s 
{
	header_vs header;
	uint32_t COOKIE;
	uint8_t OUTPUT_FORMAT;
	uint8_t PROP_FORMAT;
	uint8_t RESERVED[2];
	uint32_t PROP_VERSION;
	uint32_t PROP_STR_LENGTH;
	uint32_t PRO_STR;
}
vs_get_property_req_t;

typedef struct __packed vs_get_property_cnf_header_s 
{
	header_vs header;
	uint32_t MSTATUS;
	uint32_t COOKIE;
	uint8_t OUTPUT_FORMAT;
	uint8_t RESERVED[3];
	uint32_t PROP_STR_LENGTH;
}
vs_get_property_cnf_header_t;

typedef struct __packed vs_hg_cnf_header_s
{
	header_vs header;
	uint8_t MSTATUS;
	uint8_t action;
	uint8_t case_MainType;
	uint16_t case_SubType;
	uint8_t case_length;
	uint16_t RSVD;
	uint8_t case_val1;
	uint8_t case_val2;
	uint8_t extend_MainType;
	uint16_t extend_SubType;	
}
vs_hg_cnf_header_t;

typedef struct __packed vs_set_property_req_header_s 
{
	header_vs header;
	uint32_t COOKIE;
	uint8_t OPTION;
	uint8_t RESERVED[3];
	uint32_t PROP_VERSION;
	uint32_t PROP_ID;
	uint32_t PROP_DATA_LENGTH;
}
vs_set_property_req_header_t;

typedef struct __packed vs_set_property_cnf_s 
{
	header_vs header;
	uint32_t MSTATUS;
	uint32_t COOKIE;
}
vs_set_property_cnf_t;


#pragma pack (pop)
 
/*====================================================================*
 *   vendor specific management message ethernet frame;
 *--------------------------------------------------------------------*/
 
/*! Module IDs used by several MMEs. See the Intellon HomePlug AV 
 *  Technical Reference Manual for more information;
 */

#define MACSL_MODID    0x00
#define MACSW_MODID    0x01
#define PIB_MODID      0x02
#define MACSWPIB_MODID 0x03
#define WRALT_MODID    0x10
 
/*! SDRAM Configuration Size. See the Intellon HomePlug AV 
 *  Technical Reference Manual for more information;
 */

#define  SDRAM_MAX_LEN  32
 
/*! Manufacturer specific packet structure;
 *  See the Intellon HomePlug AV Firmware Technical Reference Manual 
 *  for more information;
 */

#pragma pack (push, 1)
 
typedef struct __packed packet_ms 

{
	struct header_eth ethernet;
	struct header_mfg customer;
	uint8_t MBODY [IHPAPI_ETH_FRAME_LEN - sizeof (struct header_mfg)];
}

packet_ms;

/*! A vendor specific HomePlug AV packet structure defined using Homeplug
 *  AV terminology; See the Intellon HomePlug AV Firmware Technical
 *  Reference Manual for more information;
 */
 
typedef struct __packed packet_vs 

{
	struct header_eth ethernet;
	struct header_mme intellon;
	uint8_t MBODY [IHPAPI_ETH_FRAME_LEN - sizeof (struct header_mme)];
}

packet_vs;

#pragma pack (pop)
 
/*====================================================================*
 *   
 *--------------------------------------------------------------------*/
 
/*! This structure contains information decoded from a VS_GET_NVM 
 *  confirmation message;
 *  see the Intellon HomePlug AV Firmware Technical Reference Manual
 *  for more information;
 */

#pragma pack (push, 1)
 
typedef struct __packed MemoryInfo 

{
	uint32_t NVMTYPE;
	uint32_t NVMPAGE;
	uint32_t NVMBLOCK;
	uint32_t NVMSIZE;
}

MemoryInfo;
/*
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
*/
#pragma pack (pop)
 
/*====================================================================*
 *   
 *--------------------------------------------------------------------*/
 
/*! Ths structure contains information decoded from a VS_ST_MAC, 
 *  VS_WR_MEM, VS_RD_MEM, VS_RD_MOD, VS_RW_MOD or VS_MOD_NVM message;
 *  see the Intellon HomePlug AV Firmware Technical Reference Manual for more 
 *  information;
 */

/*! Runtime firmware prior to 3.2 store the PIB at a higher address 
 *  than current versions; Add compiler option -DLEGACY_PIBOFFSET if 
 *  you are use legacy firmware;
 */

#define PIB_LEGACY_ADDRESS 0x01F00000
#define PIB_MEMORY_ADDRESS 0x01000000

#pragma pack (push, 1)
 
typedef struct __packed PIBVersionHeader 

{
	uint8_t FWMajorVersion;
	uint8_t PIBMinorVersion;
	uint8_t RSVD1 [2];
	uint16_t PIBLength;
	uint8_t RSVD2 [2];
	uint32_t Checksum32;
}

PIBVersionHeader;
typedef struct __packed NVMBlockHeader 

{
	uint32_t HEADERVERSION; 
	uint32_t IMAGEROMADDR;
	uint32_t IMAGEADDR;
	uint32_t IMAGELENGTH;
	uint32_t IMAGECHECKSUM;
	uint32_t ENTRYPOINT;
	uint8_t  NVMHEADERMINORVERSION;
	uint8_t  ENTRYTYPE;
	uint16_t IGNOREMASK;
	uint8_t  RESERVED [8];
	uint32_t NEXTHEADER;
	uint32_t HEADERCHECKSUM;
}

NVMBlockHeader;

/*! Defines for NVM header minor version; */
#define nvmBlockHeaderMinorVersion_Legacy 0
#define nvmBlockHeaderMinorVersion_New    1

/*! Defines for NVM entry type (valid if nvmBlockHeaderMinorVersion_New); */
#define nvmBlockHeaderEntryType_Generic                0
#define nvmBlockHeaderEntryType_SdramConfigForSynopsis 1
#define nvmBlockHeaderEntryType_SdramConfigForDenali   2
#define nvmBlockHeaderEntryType_SdramAppletForDenali   3
#define nvmBlockHeaderEntryType_Firmware               4
#define nvmBlockHeaderEntryType_OasClient              5
#define nvmBlockHeaderEntryType_CustomBlock            6
#define nvmBlockHeaderEntryType_Memctl                 7
#define nvmBlockHeaderEntryType_Invalid                8

/*! Defines for NVM ignore 16-bit mask (valid if nvmBlockHeaderMinorVersion_New); */
#define nvmBlockHeaderIgnoreMask_INT6000 0x0001   
#define nvmBLockHeaderIgnoreMask_INT6300 0x0002
#define nvmBlockHeaderIgnoreMask_INT6400 0x0004

#define NVMBlockHeaderIgnoreMask_Valid   (nvmBlockHeaderIgnoreMask_INT6000 |nvmBlockHeaderIgnoreMask_INT6300 | nvmBlockHeaderIgnoreMask_INT6400)

#define nvmBlockHeaderIgnoreMask_INT6000_INT6300  (nvmBlockHeaderIgnoreMask_INT6000 | nvmBlockHeaderIgnoreMask_INT6300)

typedef union StMacBlock 

{
	struct __packed 
	{
		uint8_t MODULEID;
		uint8_t Reserved [3];
		uint32_t IMAGELOAD;
		uint32_t IMAGELENGTH;
		uint32_t IMAGECHKSUM;
		uint32_t IMAGESTART;
	}
	rqst;
	struct __packed 
	{
		uint8_t MODULEID;
	}
	cnf;
}

StMacBlock;
typedef union WrtMemBlock 

{
	struct __packed 
	{
		uint32_t WADDRESS;
		uint32_t WLENGTH;
	}
	rqst;
	struct __packed 
	{
		uint32_t WADDRESS;
		uint32_t WLENGTH;
	}
	cnf;
}

WrtMemBlock;
typedef union RdMemBlock 

{
	struct __packed rqst 
	{
		uint32_t RADDRESS;
		uint32_t RLENGTH;
	}
	rqst;
	struct __packed 
	{
		uint32_t RADDRESS;
		uint32_t RLENGTH;
	}
	cnf;
}

RdMemBlock;
typedef union WrtModBlock 

{
	struct __packed 
	{
		uint8_t MODULEID;
		uint8_t RES1;
		uint16_t LENGTH;
		uint32_t OFFSET;
		uint32_t CHKSUM;
	}
	rqst;
	struct __packed 
	{
		uint8_t MODULEID;
		uint8_t RES1;
		uint16_t LENGTH;
		uint32_t OFFSET;
	}
	cnf;
}

WrtModBlock;
typedef union RdModBlock 

{
	struct __packed 
	{
		uint8_t MODULEID;
		uint8_t RES1;
		uint16_t LENGTH;
		uint32_t OFFSET;
	}
	rqst;
	struct __packed 
	{
		uint8_t RES1 [3];
		uint8_t MODULEID;
		uint8_t RES2;
		uint16_t LENGTH;
		uint32_t OFFSET;
		uint32_t CHKSUM;
	}
	cnf;
}

RdModBlock;
typedef struct __packed HARRspBlock 

{
	uint8_t STATUS;
}

HARRspBlock;
typedef struct __packed FlshNVMBlock 

{
	uint8_t MODULEID;
}

FlshNVMBlock;
typedef struct __packed BlockInfo 

{
	union 
	{
		StMacBlock stmacblk;
		WrtMemBlock wrmemblk;
		RdMemBlock rdmemblk;
		WrtModBlock wrmodblk;
		RdModBlock rdmodblk;
		HARRspBlock harrspblk;
		FlshNVMBlock flshnvmblk;
	}
	block;
}

BlockInfo;

#pragma pack (pop)
 
/*! This structure contains state transition information passed 
 *  directly between RxFrame and TxFrame such that RxFrame sets 
 *  the transmit information and TxFrame reacts to it; it is  
 *  similar information stored in the struct opStatus member of 
 *  struct ihpapi_result_t;
 */

#define MAX_MODULE_TX_LENGTH 1024 

#pragma pack (push, 1)
 
typedef struct __packed TxInfo 

{
	bool txok;
	uint8_t ODA [IHPAPI_ETHER_ADDR_LEN];
	uint8_t OSA [IHPAPI_ETHER_ADDR_LEN];
	uint16_t MMTYPE;
	size_t hdrlen;
	BlockInfo hdr;
	size_t datalen;
	uint8_t *data;
	size_t tlrlen;
	uint32_t trailer;
}

TxInfo;

#pragma pack (pop)
 
/*! Defines to help coding; */

#define STMCBLKR  hdr.block.stmacblk.rqst
#define STMCBLKC  hdr.block.stmacblk.cnf
#define WRMDBLKR  hdr.block.wrmodblk.rqst
#define WRMDBLKC  hdr.block.wrmodblk.cnf
#define WRMMBLKR  hdr.block.wrmemblk.rqst
#define WRMMBLKC  hdr.block.wrmemblk.cnf
#define RDMDBLKR  hdr.block.rdmodblk.rqst
#define RDMDBLKC  hdr.block.rdmodblk.cnf
#define RDMMBLKR  hdr.block.rdmemblk.rqst
#define RDMMBLKC  hdr.block.rdmemblk.cnf
#define HARRSPBLK hdr.block.harrspblk
#define FLSHNVMBLK hdr.block.flshnvmblk

#define MAX_TX_RETRY_COUNT 512
 
/*! Sequence Control Block; Intellon API functions share this structure 
 *  to monitor progress and manage state transitions; everything anyone
 *  could possibly want to know is recorded right here;
 */
 
/* flag definition. 0=false, 1=true 
 *
 *  Bits 0-7 are used and reserved for the ihapapi options, i.e. external use. The rest
 *  of the bits, i.e. bits 8-31 are for internal use only.
 */

#define scbFlag_flash 1
#define scbFlag_hasfw 2
#define scbFlag_rsvd2 4
#define scbFlag_rsvd3 8
#define scbFlag_force 16
#define scbFlag_is6k  32
#define scbFlag_rsvd6 64
#define scbFlag_rsvd7 128
#define scbFlag_bsy   256
#define scbFlag_FW    512
#define scbFlag_PB    1024
#define scbFlag_MCTL  2048

#define scbFlag_options (scbFlag_flash | scbFlag_hasfw | scbFlag_is6k | scbFlag_force)

#define scbFlag_INT6Kflash (scbFlag_flash | scbFlag_hasfw | scbFlag_is6k)
#define scbFlag_INT6Knoflash (scbFlag_hasfw | scbFlag_is6k)
#define scbFlag_noINT6Kfwflash (scbFlag_flash | scbFlag_hasfw | scbFlag_force)
#define scbFlag_noINT6Knofwflash (scbFlag_flash)
#define scbFlag_noINT6Knoflash 0
#define scbFlag_noINT6Knofw 0

#pragma pack (push, 1)
 
typedef struct __packed SeqCB 

{
	flag_t flags;
	ihpapi_hostActionRequest_t action;
	ihpapi_opCode_t opcode;
	uint16_t retries;
	uint8_t SA [IHPAPI_ETHER_ADDR_LEN];
	uint32_t moduleLen;
	uint32_t offset;
	uint32_t pibmemoffset;
	TxInfo tcb;
	buffer_t *bcb [2];
/* 0-FW, 1-PB */
	buffer_t *txbcb;
	uint8_t modid;
}

SeqCB;

#pragma pack (pop)
 
/*====================================================================*
 *   internal encoder functions;
 *--------------------------------------------------------------------*/
 
size_t EncodeEthernetHeader (uint8_t buffer [], signed length, uint8_t ODA [], uint8_t OSA []);
size_t EncodeIntellonHeader (uint8_t buffer [], signed length, uint16_t MMTYPE);

size_t EncodeV1IntellonHeader (uint8_t buffer [], signed length, uint16_t MMTYPE);

/*====================================================================*
 *   internal decoder functions;
 *--------------------------------------------------------------------*/
 
int ihp_DecodeGetVersion (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeLinkStats (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeNetworkInfoStats (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeNetworkInfo (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeSetKey (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeMfgString (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeHostAction (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeResetDevice (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeResetToFactoryDefaults (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeSetSDRAMCfg (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeGetToneMapInfo (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeGet74ToneMapInfo (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeGetRxToneMapInfo (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeGet74RxToneMapInfo (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeEthernetPHYSettings (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeStartMAC (const uint8_t buffer [], size_t length, ihpapi_result_t * result, BlockInfo * block);
int ihp_DecodeReadModule (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeGetFrequencyBandSelection (const uint8_t buffer [], size_t length, ihpapi_result_t * result) ;
int ihp_DecodeSetFrequencyBandSelection (const uint8_t buffer [], size_t length, ihpapi_result_t * result) ;
int ihp_DecodeGetTxGain(const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeSetTxGain(const uint8_t buffer [ ], size_t length, ihpapi_result_t * result);
//int ihp_DecodeWriteModule (const uint8_t buffer [], size_t length, ihpapi_result_t * result, WrtModBlock * wrmodblk);
int ihp_DecodeWriteModule (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeWriteMemory (const uint8_t buffer [], size_t length, ihpapi_result_t * result, WrtMemBlock * wrmemblk);
int ihp_DecodeFlashNVM (const uint8_t buffer [], size_t length, ihpapi_result_t * result, BlockInfo * block);
int ihp_DecodeGetNVM (const uint8_t buffer [], size_t length, ihpapi_result_t * result, struct MemoryInfo * memory);
int ihp_DecodeSetClassification (const uint8_t buffer [], size_t length, ihpapi_result_t * result) ;
int ihp_DecodeMdioInfo (const uint8_t buffer [], size_t length, ihpapi_result_t * result) ;
int ihp_DecodeGetUserHFID (const uint8_t buffer[], size_t length, ihpapi_result_t * result);
//int ihp_DecodeSetUserHFID(const uint8_t buffer [ ], size_t length, ihpapi_result_t * result);
int ihp_DecodeGetHgManage (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeGetHgBusiness (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeSetHgManage(const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeSetHgBusiness(const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeGetHgSsidStatus(const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_DecodeGetHgSsidStatus(const uint8_t buffer [], size_t length, ihpapi_result_t * result);
/*====================================================================*
 *   internal sequencer functions;
 *--------------------------------------------------------------------*/
 
int ihp_SequenceReadModule (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_SequenceWriteModule (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
int ihp_SequenceWriteMemory (const uint8_t buffer [], size_t length, ihpapi_result_t * result);

/*====================================================================*
 *   internal helper functions;
 *--------------------------------------------------------------------*/
NVMBlockHeader *ihp_FindFWHeader (const uint8_t FW_pbuffer [], size_t length);
NVMBlockHeader *ihp_FindNVMObject ( const uint8_t NVM_pbuffer [], size_t length, uint8_t oType );
int ihp_isLegacyNVM (const uint8_t FW_pbuffer [], size_t length);

/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif
 

