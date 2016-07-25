/*****************************************************************************
//
//  Copyright (c) 2005  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92619
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
******************************************************************************
//
//  Filename:       bcmcfm.h
//  Author:         Peter Tran
//  Creation Date:  04/28/05
//
******************************************************************************
//  Description:
//  Define the global types, enums. and constants for BcmCfm_ in object format
//
*****************************************************************************/

#ifndef __BCM_CFM_H__
#define __BCM_CFM_H__

#include <netinet/in.h>
#include "ifcdefs.h"

// System Group Objects
#define BCMCFM_OBJ_SYS_DEVICE_INFO              0x00100001 // BcmCfm_DevInfoCfg_t
#define BCMCFM_OBJ_SYS_MGMT_ADMIN_LOGIN         0x00100002 // BcmCfm_MgmtLoginCfg_t
#define BCMCFM_OBJ_SYS_MGMT_USER_LOGIN          0x00100003 // BcmCfm_MgmtLoginCfg_t
#define BCMCFM_OBJ_SYS_MGMT_REMOTE_LOGIN        0x00100004 // BcmCfm_MgmtLoginCfg_t
#define BCMCFM_OBJ_SYS_SYSLOG                   0x00100005 // BcmCfm_SysLogCfg_t
#define BCMCFM_OBJ_SYS_UPNP                     0x00100006 // BcmCfm_UpnpCfg_t
#define BCMCFM_OBJ_SYS_SNMP                     0x00100007 // BcmCfm_SnmpCfg_t
#define BCMCFM_OBJ_SYS_DDNS                     0x00100008 // BcmCfm_DDnsCfg_t
#define BCMCFM_OBJ_SYS_NTP                      0x00100009 // BcmCfm_NtpCfg_t
#define BCMCFM_OBJ_SYS_IPP                      0x0010000A // BcmCfm_IppCfg_t
#define BCMCFM_OBJ_SYS_TR069                    0x0010000B // BcmCfm_TR069Cfg_t
#define BCMCFM_OBJ_SEC_ACC_CTRL_BY_HOST         0x0010000C // BcmCfm_ACHostCfg_t            
#define BCMCFM_OBJ_SEC_ACC_CTRL_BY_SERVICE      0x0010000D // BcmCfm_ACServiceCfg_t          
#define BCMCFM_OBJ_SYS_MGMT_LOCK                0x0010000E // BcmCfm_MgmtLockCfg_t
#define BCMCFM_OBJ_SEC_ACC_CTRL_CFG_STS         0x0010000F // BcmCfm_ACHostCfg_t   
#define BCMCFM_OBJ_SYS_CERTIFICATE_LOCAL        0x00100010 // BcmCfm_CertificateCfg_t
#define BCMCFM_OBJ_SYS_IGMPSNP_FLAGS            0x00100011
#define BCMCFM_OBJ_SYS_IGMPSNP_MODE             0x00100012
#define BCMCFM_OBJ_SYS_IPSEC                    0x00100013
#define BCMCFM_OBJ_SYS_CERTIFICATE_CA           0x00100014       // BcmCfm_CertificateCfg_t
#define BCMCFM_OBJ_SYS_DEBUG_PORTMIRROR         0x00100015 // BcmCfm_CertificateCfg_t
#define BCMCFM_OBJ_SYS_MACFILTER_FLAGS          0x00100016

// Physical Interface Group Objects
#define BCMCFM_OBJ_IFC_ETHERNET                 0x00200001 // BcmCfm_EthIfcCfg_t
#define BCMCFM_OBJ_IFC_USB                      0x00200002 // BcmCfm_UsbIfcCfg_t
#define BCMCFM_OBJ_IFC_WLAN                     0x00200003 // BcmCfm_WlanIfcCfg_t
#define BCMCFM_OBJ_IFC_WLAN_VIRT                0x00200004 // BcmCfm_WlanIfcVirtCfg_t
#define BCMCFM_OBJ_IFC_ADSL                     0x00200005 // BcmCfm_AdslIfcCfg_t
#define BCMCFM_OBJ_IFC_ATM                      0x00200006 // BcmCfm_AtmIfcCfg_t
#define BCMCFM_OBJ_IFC_ATM_VCC                  0x00200007 // BcmCfm_AtmVccCfg_t

// Network Group Objects
#define BCMCFM_OBJ_NTWK_INTF                    0x00300001 // BcmCfm_NtwkIntfCfg_t
#define BCMCFM_OBJ_NTWK_BRIDGE                  0x00300002 // BcmCfm_NtwkBridgeCfg_t
#define BCMCFM_OBJ_NTWK_DEFAULT_GATEWAY         0x00300007 // BcmCfm_NtwkDefaultGatewayCfg_t
#define BCMCFM_OBJ_NTWK_ROUTE_ENTRY             0x00300008 // BcmCfm_NtwkRouteEntryCfg_t
#define BCMCFM_OBJ_NTWK_DNS                     0x00300009 // BcmCfm_NtwkDnsCfg_t
#define BCMCFM_OBJ_NTWK_RIP                     0x0030000A // BcmCfm_NtwkRipCfg_t
#define BCMCFM_OBJ_NTWK_DHCP_SERVER             0x0030000C // BcmCfm_NtwkDhcpSrvCfg_t
#define BCMCFM_OBJ_NTWK_LAN_HOST             0x0030000D // BcmCfm_NtwkLanHostSts_t
#define BCMCFM_OBJ_NTWK_BRIDGING                0x0030000E // BcmCfm_NtwkBridgingCfg_t
#define BCMCFM_OBJ_NTWK_BRIDGE_FILTER           0x0030000F // BcmCfm_NtwkBridgeFltCfg_t
#define BCMCFM_OBJ_NTWK_BRIDGE_AVLINTF          0x00300010 // BcmCfm_NtwkBridgeAvlIntfCfg_t
#define BCMCFM_OBJ_NTWK_RIP_INTERFACE    0x00300011 // BcmCfm_NtwkRipCfg_t

// Network Security Group Objects
#define BCMCFM_OBJ_SEC_DMZ_HOST                 0x00400001 // BcmCfm_SecDmzHostCfg_t    
#define BCMCFM_OBJ_SEC_VIRTUAL_SERVER           0x00400002 // BcmCfm_SecVirtualServerCfg_t    
#define BCMCFM_OBJ_SEC_FILTER_INCOMING                   0x00400003 // BcmCfm_SecFilterCfg_t    
#define BCMCFM_OBJ_SEC_PORT_TRIGGER              0x00400004 // BcmCfm_SecPortTriggerCfg_t   
#define BCMCFM_OBJ_SEC_MAC_FILTER               0x00400005 // BcmCfm_SecMacFilterCfg_t    
#define BCMCFM_OBJ_SEC_PAREN_CTRL_CFG         0x00400006
#define BCMCFM_OBJ_SEC_FILTER_OUTGOING        0x00400007 // BcmCfm_SecFilterCfg_t    

// Network Performance Group Objects
#define BCMCFM_OBJ_PERF_QOS_CLS                 0x00500001 // BcmCfm_PerfQosCfg_t
#define BCMCFM_OBJ_PERF_QOS_QMGMT               0x00500002 // BcmCfm_PerfQosQMgmtCfg_t
#define BCMCFM_OBJ_PERF_QOS_QCFG                0x00500003 // BcmCfm_PerfQosQueueCfg_t

// Network Voice Group Objects
#define BCMCFM_OBJ_VOICE                        0x00600001 // BcmCfm_VoiceCfg_t

// Network Bluetooth Group Objects
#define BCMCFM_OBJ_BLUETOOTH                    0x00700001 // BcmCfm_BluetoothCfg_t

// Wireless LAN Management Group Objects
#define BCMCFM_OBJ_WLAN_SEC                     0x00800001 // BcmCfm_WlanSecCfg_t  
#define BCMCFM_OBJ_WLAN_SEC_MAC_FILTER          0x00800002 // BcmCfm_WlanSecMacFilterCfg_t
#define BCMCFM_OBJ_WLAN_WDS_PEER_MAC            0x00800003 // BcmCfm_WlanWdsPeerCfg_t
#define BCMCFM_OBJ_WLAN_WDS_SCANNED_MAC         0x00800004 // BcmCfm_WlanWdsPeerCfg_t
#define BCMCFM_OBJ_WLAN_CLIENT_MAC              0x00800005 // BcmCfm_WlanWdsPeerCfg_t
#define BCMCFM_OBJ_WLAN_SUPPORTED_COUNTRY       0x00800006 // BcmCfm_WlanSupportedCountry_t
#define BCMCFM_OBJ_WLAN_SUPPORTED_CHANNEL       0x00800007 // BcmCfm_WlanSupportedChannel_t

// Diagnostic Group Objects
#define BCMCFM_OBJ_SYS_DIAG                     0x00900001 // BcmCfm_DiagData_t

//Device Association Group Objects
        
#define BCMCFM_OBJ_DEVICE_ASSOCIATION           0x00A00001 // BcmCfm_ManageableDevice_t

// Error definitions
#define INV_OBJ_INDEX 0xFFFFFFFF

#if SUPPORT_PORT_MAP
#define BCMCFM_BRIDGE_NOT_ASSOC -1
#endif
#ifdef SUPPORT_TR69C
#define TR69C_START     1
#define TR69C_STOP      2
#endif

// Configuration Status
typedef enum {
    BcmCfm_CfgDisabled,
    BcmCfm_CfgEnabled
} BcmCfm_CfgStatus;

// Link State
typedef enum {
    BcmCfm_LinkDown,
    BcmCfm_LinkUp
} BcmCfm_LinkState;

// ADSL Link State
typedef enum {
    BcmCfm_AdslLinkUp=0,
    BcmCfm_AdslLinkDown,
    BcmCfm_AdslTrainingExchange,
    BcmCfm_AdslTrainingChannelAnalysis,
    BcmCfm_AdslTrainingStarted,
    BcmCfm_AdslTrainingG994,
    BcmCfm_AdslTrainingG994NonstdInfoRx,
    BcmCfm_AdslBertComplete,
    BcmCfm_AdslAtmIdle
} BcmCfm_AdslLinkState;

// System log level
typedef enum {
#if 0
    BcmCfm_SyslogStore_LocalBuffer, 
    BcmCfm_SyslogStore_LocalFile, 
    BcmCfm_SyslogStore_Remote,
    BcmCfm_SyslogStore_LocalBufANDRemote, 
    BcmCfm_SyslogStore_LocalFileANDRemote
#endif
    BcmCfm_SyslogStore_Local = 1, 
    BcmCfm_SyslogStore_Remote, 
    BcmCfm_SyslogStore_Both,
} BcmCfm_SyslogStore;

typedef enum {
    BcmCfm_SyslogLevelEmerg,     /* system is unusable */
    BcmCfm_SyslogLevelAlert,     /* action must be taken immediately */
    BcmCfm_SyslogLevelCrit,      /* critical conditions */
    BcmCfm_SyslogLevelErr,       /* error conditions */
    BcmCfm_SyslogLevelWarning,   /* warning conditions */
    BcmCfm_SyslogLevelNotice,    /* normal but significant condition */
    BcmCfm_SyslogLevelInfo,      /* informational */
    BcmCfm_SyslogLevelDebug      /* debug-level messages */
} BcmCfm_SyslogLevel;

// Network interface protocols
typedef enum {
    BcmCfm_NtwkIntfProtoPPPoE,
    BcmCfm_NtwkIntfProtoPPPoA,
    BcmCfm_NtwkIntfProtoIPoA,
    BcmCfm_NtwkIntfProtoMAC,
    BcmCfm_NtwkIntfProtoIPoWAN
} BcmCfm_NtwkIntfProtocol;

// PPP authentication methods
typedef enum {
    BcmCfm_NtwkIntfPPPAuthAuto,
    BcmCfm_NtwkIntfPPPAuthPAP,
    BcmCfm_NtwkIntfPPPAuthCHAP,
    BcmCfm_NtwkIntfPPPAuthMSCHAP
} BcmCfm_NtwkIntfPPPAuth;

// ATM Service Categories
typedef enum {
    BcmCfm_ATMCatUBR,
    BcmCfm_ATMCatUBRwithPCR,
    BcmCfm_ATMCatCBR,
    BcmCfm_ATMCatVBR,
    BcmCfm_ATMCatVBRnrt
} BcmCfm_atmCategory;

// ATM AAL Type
typedef enum {
    BcmCfm_ATMAal2,
    BcmCfm_ATMAalTransparent,
    BcmCfm_ATMAal0Packet,
    BcmCfm_ATMAal0CellCrc,
    BcmCfm_ATMAal5 = 7
} BcmCfm_atmAalType;

// ATM Encapsulation Mode
typedef enum {
    BcmCfm_ATMEncapVcmuxRoute = 1,
    BcmCfm_ATMEncapVcmuxBridge8023,
    BcmCfm_ATMEncapVcmuxBridge8025,
    BcmCfm_ATMEncapVcmuxBridge8026,
    BcmCfm_ATMEncapVcmuxLanEmulation8023,
    BcmCfm_ATMEncapVcmuxLanEmulation8025,
    BcmCfm_ATMEncapLlc,
    BcmCfm_ATMEncapFrameRelaySscs,
    BcmCfm_ATMEncapOther,
    BcmCfm_ATMEncapUnknown
} BcmCfm_atmEncapMode;

// DDns Provider
typedef enum {
    BcmCfm_DDnsProviderTzo,
    BcmCfm_DDnsProviderDynDns
} BcmCfm_DDnsProvider;

// PPP State
typedef enum {
    BcmCfm_PPPStateUnknown,
    BcmCfm_PPPStateDown,
    BcmCfm_PPPStateConnecting,
    BcmCfm_PPPStateUp,
    BcmCfm_PPPStateAuthFail
} BcmCfm_PPPState;

// DHCP Client State
typedef enum {
    BcmCfm_DHCPStateUnknown,
    BcmCfm_DHCPStateDown,
    BcmCfm_DHCPStateUp,
    BcmCfm_DHCPStateInProgress
} BcmCfm_DHCPState;

// Access control service
typedef enum {
    BcmCfm_ACServiceFtp,
    BcmCfm_ACServiceHttp,
    BcmCfm_ACServiceIcmp,
    BcmCfm_ACServiceSnmp,
    BcmCfm_ACServiceSsh,
    BcmCfm_ACServiceTelnet,
    BcmCfm_ACServiceTftp
} BcmCfm_ACService;

// Security IP Filter Direction
typedef enum {
    BcmCfm_IpFilterIncoming,
    BcmCfm_IpFilterOutgoing
} BcmCfm_IpFilterDirection;

typedef enum {
    BcmCfm_MacFilterIncoming,
    BcmCfm_MacFilterOutgoing,
    BcmCfm_MacFilterBoth
} BcmCfm_MacFilterDirEnum;

// Security MAC Filter Protocol
typedef enum {
    BcmCfm_MacFilterPppoe = 1,
    BcmCfm_MacFilterIpv4,
    BcmCfm_MacFilterIpv6,
    BcmCfm_MacFilterAppleTalk,
    BcmCfm_MacFilterIpx,
    BcmCfm_MacFilterNetBeui,
    BcmCfm_MacFilterIgmp
} BcmCfm_MacFilterProtocol;

// Ethernet Auto State
typedef enum {
    BcmCfm_StateOff,
    BcmCfm_StateOn
} BcmCfm_State;

// Ethernet Speed
typedef enum {
    BcmCfm_EthSpeed100,
    BcmCfm_EthSpeed10
} BcmCfm_EthSpeed;

// Ethernet Duplex
typedef enum {
    BcmCfm_EthDuplexFull,
    BcmCfm_EthDuplexHalf
} BcmCfm_EthDuplex;

// Ethernet Duplex
typedef enum {
    BcmCfm_NtwkRipOperationActive,
    BcmCfm_NtwkRipOperationPassive
} BcmCfm_NtwkRipOperation;

// Network Ecapsulation Mode
typedef enum {
    BcmCfm_NtwkEncapLLC,
    BcmCfm_NtwkEncapVCMUX
} BcmCfm_NtwkEncapMode;

// DHCP Sever Type
typedef enum {
    BcmCfm_NtwkDhcpSrvNormal,
    BcmCfm_NtwkDhcpSrvRelay
} BcmCfm_DhcpSrvType;

// Wireless Authentication
typedef enum {
    BcmCfm_WlanAuth_Open,
    BcmCfm_WlanAuth_Shared
} BcmCfm_Wlan_Auth;

// Wireless Wep Key Length
typedef enum {
    BcmCfm_WlanWepLen_128Bits,
    BcmCfm_WlanWepLen_64Bits
} BcmCfm_WlanWepLen;

// Wireless 802.1x Authentication
typedef enum {
    BcmCfm_WlanAuthMode_None,
    BcmCfm_WlanAuthMode_Radius,
} BcmCfm_Wlan_AuthMode;

// Wireless Authentication Key Management
typedef struct _BcmCfm_Wlan_WpaAKmBit {
    uint8
    BcmCfm_Wlan_WpaAKm_None:1,
    BcmCfm_Wlan_WpaAKm_Unspecified:1, 
    BcmCfm_Wlan_WpaAKm_Psk:1, 
    reserved:3,
    BcmCfm_Wlan_Wpa2AKm_Unspecified:1,
    BcmCfm_Wlan_Wpa2AKm_Psk:1;
} BcmCfm_Wlan_WpaAKmBit;

typedef struct _BcmCfm_Wlan_WpaAKm {
    union {
        uint8 val;
        BcmCfm_Wlan_WpaAKmBit bitVal;
    } u;	
} BcmCfm_Wlan_WpaAKm;

// Wireless Encryption
typedef enum {
    BcmCfm_Wlan_Crypto_Tkip,
    BcmCfm_Wlan_Crypto_Aes,
    BcmCfm_Wlan_Crypto_Tkip_Aes
} BcmCfm_Wlan_Crypto;

// Wireless Preamble type
typedef enum {
    BcmCfm_Wlan_Preamble_Long,
    BcmCfm_Wlan_Preamble_Short,
} BcmCfm_Wlan_Preamble;

// Wireless 54g Mode
typedef enum {
    BcmCfm_Wlan_gMode_b_Only,        
    BcmCfm_Wlan_gMode_Auto,          
    BcmCfm_Wlan_gMode_Performance = 4,
    BcmCfm_Wlan_gMode_Lrs = 5
} BcmCfm_Wlan_gMode;

// Wireless Phy Type
typedef enum {
    BcmCfm_Wlan_PhyType_a,
    BcmCfm_Wlan_PhyType_b,
    BcmCfm_Wlan_PhyType_g
} BcmCfm_Wlan_PhyType;

// Wireless BasicRateSet
typedef enum {
    BcmCfm_Wlan_BasicRateSet_Default,
    BcmCfm_Wlan_BasicRateSet_1_2_Mbps,        
    BcmCfm_Wlan_BasicRateSet_All,        
    BcmCfm_Wlan_BasicRateSet_WiFi_Alt,        
} BcmCfm_Wlan_BasicRateSet;

// Wireless Mode
typedef enum {
    BcmCfm_Wlan_Mode_AP,
    BcmCfm_Wlan_Mode_Bridge
} BcmCfm_Wlan_Mode;

// Wireless Mode
typedef enum {
    BcmCfm_Wlan_Band_A = 1,
    BcmCfm_Wlan_Band_B = 2
} BcmCfm_Wlan_Band;

// Bridge Restrict Mode
typedef enum {
    BcmCfm_Wlan_WDS_RestrictMode_Enable,
    BcmCfm_Wlan_WDS_RestrictMode_Disable,
    BcmCfm_Wlan_WDS_RestrictMode_Enable_Scan
} BcmCfm_Wlan_WDS_RestrictMode;

// Mac Filter Mode
typedef enum {
    BcmCfm_Wlan_Mac_FilterMode_Disabled,
    BcmCfm_Wlan_Mac_FilterMode_Allow,
    BcmCfm_Wlan_Mac_FilterMode_Deny
} BcmCfm_Wlan_Mac_FilterMode;

// Regulatory Mode
typedef enum {
    BcmCfm_Wlan_RegulatoryMode_Disabled,
    BcmCfm_Wlan_RegulatoryMode_11h,
    BcmCfm_Wlan_RegulatoryMode_11d,
} BcmCfm_Wlan_RegulatoryMode;

// Ses WDS Mode
typedef enum {
    BcmCfm_Wlan_SesWdsMode_Disabled,
    BcmCfm_Wlan_SesWdsMode_Configurator_Regular,
    BcmCfm_Wlan_SesWdsMode_Configurator_WdsOnly,
    BcmCfm_Wlan_SesWdsMode_Configurator_Client
} BcmCfm_Wlan_SesWdsMode;

// Ses Input Event Status
typedef enum {
    BcmCfm_Wlan_SesStatus_Success,
    BcmCfm_Wlan_SesStatus_Unknown
} BcmCfm_Wlan_SesStatus;

//BeaconType
typedef enum {
    BcmCfm_Wlan_BeaconType_None,
    BcmCfm_Wlan_BeaconType_Basic,	
    BcmCfm_Wlan_BeaconType_WPA,
    BcmCfm_Wlan_BeaconType_11i,
    BcmCfm_Wlan_BeaconType_BasicandWPA,
    BcmCfm_Wlan_BeaconType_Basicand11i,
    BcmCfm_Wlan_BeaconType_WPAand11i,
    BcmCfm_Wlan_BeaconType_BasicandWPAand11i
} BcmCfm_Wlan_BeaconType;

//BcmCfm_Wlan_BasicEncryptionModes
typedef enum {
    BcmCfm_Wlan_BasicEncryptionModes_None,
    BcmCfm_Wlan_BasicEncryptionModes_WEPEncryption,    
} BcmCfm_Wlan_BasicEncryptionModes;

//BcmCfm_Wlan_BasicAuthenticationMode
typedef enum {
    BcmCfm_Wlan_BasicAuthenticationMode_None,
    BcmCfm_Wlan_BasicAuthenticationMode_EAPAuthentication
} BcmCfm_Wlan_BasicAuthenticationMode;

//BcmCfm_Wlan_WPAEncryptionModes
typedef enum {
    BcmCfm_Wlan_WPAEncryptionModes_WEPEncryption,
    BcmCfm_Wlan_WPAEncryptionModes_TKIPEncryption,
    BcmCfm_Wlan_WPAEncryptionModes_WEPandTKIPEncryption,
    BcmCfm_Wlan_WPAEncryptionModes_AESEncryption,
    BcmCfm_Wlan_WPAEncryptionModes_WEPandAESEncryption,
    BcmCfm_Wlan_WPAEncryptionModes_TKIPandAESEncryption,
    BcmCfm_Wlan_WPAEncryptionModes_WEPandTKIPandAESEncryption
} BcmCfm_Wlan_WPAEncryptionModes;

//BcmCfm_Wlan_WPAAuthenticationMode
typedef enum {
    BcmCfm_Wlan_WPAAuthenticationMode_PSKAuthentication,
    BcmCfm_Wlan_WPAAuthenticationMode_EAPAuthentication
} BcmCfm_Wlan_WPAAuthenticationMode;

//BcmCfm_Wlan_IEEE11iEncryptionModes
typedef enum {
    BcmCfm_Wlan_IEEE11iEncryptionModes_WEPEncryption,
    BcmCfm_Wlan_IEEE11iEncryptionModes_TKIPEncryption,
    BcmCfm_Wlan_IEEE11iEncryptionModes_WEPandTKIPEncryption,
    BcmCfm_Wlan_IEEE11iEncryptionModes_AESEncryption,
    BcmCfm_Wlan_IEEE11iEncryptionModes_WEPandAESEncryption,
    BcmCfm_Wlan_IEEE11iEncryptionModes_TKIPandAESEncryption,
    BcmCfm_Wlan_IEEE11iEncryptionModes_WEPandTKIPandAESEncryption
} BcmCfm_Wlan_IEEE11iEncryptionModes;

//BcmCfm_Wlan_IEEE11iAuthenticationMode
typedef enum {
    BcmCfm_Wlan_IEEE11iAuthenticationMode_PSKAuthentication,
    BcmCfm_Wlan_IEEE11iAuthenticationMode_EAPAuthentication,
    BcmCfm_Wlan_IEEE11iAuthenticationMode_EAPandPSKAuthentication
} BcmCfm_Wlan_IEEE11iAuthenticationMode;

// Tx Rate
typedef enum {
    BcmCfm_Wlan_TxRate_Auto     = 0,         // a/b/g
    BcmCfm_Wlan_TxRate_1Mbps    = 10000000,    // b/g
    BcmCfm_Wlan_TxRate_2Mbps    = 20000000,    // b/g
    BcmCfm_Wlan_TxRate_5_5Mbps  = 55000000,    // b/g
    BcmCfm_Wlan_TxRate_6Mbps    = 60000000,    // a/g
    BcmCfm_Wlan_TxRate_9Mbps    = 90000000,    // a/g
    BcmCfm_Wlan_TxRate_11Mbps   = 11000000,    // b/g
    BcmCfm_Wlan_TxRate_12Mbps   = 12000000,    // a/g
    BcmCfm_Wlan_TxRate_18Mbps   = 18000000,    // a/g
    BcmCfm_Wlan_TxRate_24Mbps   = 24000000,    // a/g
    BcmCfm_Wlan_TxRate_36Mbps   = 36000000,    // a/g
    BcmCfm_Wlan_TxRate_48Mbps   = 48000000,    // a/g
    BcmCfm_Wlan_TxRate_54Mbps   = 54000000     // a/g
} BcmCfm_Wlan_TxRate;

// Sta Status
typedef struct _BcmCfm_Wlan_StaStatus {
    uint8
    reserved1:4,
    Associated:1,
    Authorized:1,
    reserved2:2;
} BcmCfm_Wlan_StaStatus;

// Time Zone
typedef enum {
    BcmCfm_TzInternationalDateLineWest,
    BcmCfm_TzMidwayIsland,
    BcmCfm_TzHawaii,
    BcmCfm_TzAlaska,
    BcmCfm_TzPacificTime,
    BcmCfm_TzArizona,
    BcmCfm_TzChihuahua,
    BcmCfm_TzMountaiTime,
    BcmCfm_TzCentralAmerica,
    BcmCfm_TzCentralTime,
    BcmCfm_TzGuadalajara,
    BcmCfm_TzSaskatchewan,
    BcmCfm_TzBogota,
    BcmCfm_TzEasternTime,
    BcmCfm_TzIndiana,
    BcmCfm_TzAtlanticTime,
    BcmCfm_TzCaracas,
    BcmCfm_TzSantiago,
    BcmCfm_TzNewfoundland,
    BcmCfm_TzBrasilia,
    BcmCfm_TzBuenosAires,
    BcmCfm_TzGreenland,
    BcmCfm_TzMidAtlantic,
    BcmCfm_TzAzores,
    BcmCfm_TzCapeVerdeIsland,
    BcmCfm_TzCasablanca,
    BcmCfm_TzGreenwichMeanTime,
    BcmCfm_TzAmsterdam,
    BcmCfm_TzBelgrade,
    BcmCfm_TzBrussels,
    BcmCfm_TzSarajevo,
    BcmCfm_TzWestCentralAfrica,
    BcmCfm_TzAthens,
    BcmCfm_TzBucharest,
    BcmCfm_TzCairo,
    BcmCfm_TzHarare,
    BcmCfm_TzHelsinki,
    BcmCfm_TzJerusalem,
    BcmCfm_TzBaghdad,
    BcmCfm_TzKuwait,
    BcmCfm_TzMoscow,
    BcmCfm_TzNairobi,
    BcmCfm_TzTehran,
    BcmCfm_TzAbuDhabi,
    BcmCfm_TzBaku,
    BcmCfm_TzKabul,
    BcmCfm_TzEkaterinburg,
    BcmCfm_TzIslamabad,
    BcmCfm_TzChennai,
    BcmCfm_TzKathmandu,
    BcmCfm_TzAlmaty,
    BcmCfm_TzAstana,
    BcmCfm_TzSriJayawardenepura,
    BcmCfm_TzRangoon,
    BcmCfm_TzBangkok,
    BcmCfm_TzKrasnoyarsk,
    BcmCfm_TzBeijing,
    BcmCfm_TzIrkutsk,
    BcmCfm_TzKualaLumpur,
    BcmCfm_TzPerth,
    BcmCfm_TzTaipei,
    BcmCfm_TzOsaka,
    BcmCfm_TzSeoul,
    BcmCfm_TzYakutsk,
    BcmCfm_TzAdelaide,
    BcmCfm_TzDarwin,
    BcmCfm_TzBrisbane,
    BcmCfm_TzCanberra,
    BcmCfm_TzGuam,
    BcmCfm_TzHobart,
    BcmCfm_TzVladivostok,
    BcmCfm_TzMagadan,
    BcmCfm_TzSolomon,
    BcmCfm_TzAuckland,
    BcmCfm_TzFiji
} BcmCfm_TimeZone;

// API return status
typedef enum {
    BcmCfm_Ok = 0,
    BcmCfm_Fail
} BcmCfm_Status;

typedef enum {
    BcmCfm_NoImageFormat,
    BcmCfm_BroadcomImageFormat,
    BcmCfm_FlashImageFormat,
    BcmCfm_PsiTextFormat
} BcmCfm_DownloadFormat;

// Lock/Unlock Management Configuration
typedef enum {
    BcmCfm_MgmtUnlocked,
    BcmCfm_MgmtLocked
} BcmCfm_CfgMgmtLock;

// UI Management Type
typedef enum {
    BcmCfm_TypeMgmtUnknown,
    BcmCfm_TypeMgmtCLI,
    BcmCfm_TypeMgmtMenuCLI,
    BcmCfm_TypeMgmtWEB,
    BcmCfm_TypeMgmtSNMP,
    BcmCfm_TypeMgmtTR069,
    BcmCfm_TypeMgmtUPNP
} BcmCfm_TypeMgmt;

// Stats Type
typedef enum {
    BcmCfm_StatsTxBytes=0,
    BcmCfm_StatsRxBytes,
    BcmCfm_StatsTxPkts,
    BcmCfm_StatsRxPkts,
    BcmCfm_StatsIfName,
    BcmCfm_StatsTxErrors,  
    BcmCfm_StatsRxErrors
} BcmCfm_Stats;

typedef enum {
    BcmCfm_AccessService_Disable =0,
    BcmCfm_AccessService_Lan,
    BcmCfm_AccessService_Wan,
    BcmCfm_AccessService_Enable
} BcmCfm_AccessService_Stats;

// Route Type
typedef enum {
    BcmCfm_CfgRtTypeDefault=0,
    BcmCfm_CfgRtTypeNetwork,
    BcmCfm_CfgRtTypeHost
} BcmCfm_CfgRouteType;

#define MAC_ADDR_LEN          6
#define OPT_BUFF_MAX_LEN    16
#define INTF_IN_LIST_MAX      256
#define SSID_LEN              32
#define CNTRY_BUF_LEN         4
#define IPSEC_SMALL_LEN       32
// Data Structures for System GroupObjects

typedef struct _BcmCfm_DevInfo {
    char                 *hwVersion;
    char                 *swVersion;
    char                 *deviceLog;
    char                 *releaseVersion;
    char                 *manufacturerOui;
    char                 *serialNumber;
    char                 *productClass;
    char                 *manufacturer;
} BcmCfm_DevInfoCfg_t, *PBcmCfm_DevInfoCfg_t;

typedef struct _BcmCfm_MgmtLogin {
    BcmCfm_CfgStatus     status;
    char                 *name;
    char                 *password;
} BcmCfm_MgmtLoginCfg_t, *PBcmCfm_MgmtLoginCfg_t;

typedef struct _BcmCfm_LockMgmtCfg {
    BcmCfm_CfgMgmtLock action;
    BcmCfm_TypeMgmt    type;
} BcmCfm_LockMgmtCfg_t, *PBcmCfm_LockMgmtCfg_t;

typedef struct _BcmCfm_SyslogOption {
    uint8
    local:1,
    remote:1,
    file:1,
    circular:1,
    reserved:4;
} BcmCfm_SyslogOption;

typedef struct _BcmCfm_SyslogCfg {
    BcmCfm_CfgStatus    status;                /* enable, disable */
    //int option;   
    BcmCfm_SyslogStore  option;                /* log local, log remote, log local+remote */
    BcmCfm_SyslogLevel  localDisplayLevel;     /* messages get displayed if their priority is higher than this level */
    BcmCfm_SyslogLevel  localLogLevel;         /* messages get logged if their priority is higher than this level */
    BcmCfm_SyslogLevel  remoteLogLevel;        /* messages get logged if their priority is higher than this level */
    struct in_addr            serverIP;              /* if remote logging, syslog server's IP address */
    uint16              serverPort;            /* if remote logging, syslog server's UDP port number */
} BcmCfm_SyslogCfg_t, *PBcmCfm_SyslogCfg_t;

typedef struct _BcmCfm_UpnpCfg {
    BcmCfm_CfgStatus     status;
} BcmCfm_UpnpCfg_t, *PBcmCfm_UpnpCfg_t;

typedef struct _BcmCfm_SysFlagCfg {
    BcmCfm_CfgStatus     igmpSnpCfg;
    BcmCfm_CfgStatus     igmpSnpMode;
    BcmCfm_CfgStatus     macFilterMode;
} BcmCfm_SysFlagCfg_t, *PBcmCfm_SysFlagCfg_t;

typedef struct _BcmCfm_SnmpCfg {
    BcmCfm_CfgStatus     status;
    char                 *roCommunity;
    char                 *rwCommunity;
    char                 *sysName;
    char                 *sysLocation;
    char                 *sysContact;
    uint32               trapIp; 
} BcmCfm_SnmpCfg_t, *PBcmCfm_SnmpCfg_t;

typedef struct _BcmCfm_DDnsCfg {
    BcmCfm_CfgStatus      status;
    BcmCfm_DDnsProvider   provider;
    char                  *interface;
    char                  *username;
    char                  *password;
    char                  *hostname;
} BcmCfm_DDnsCfg_t, *PBcmCfm_DDnsCfg_t;

typedef struct _BcmCfm_NtpCfg {
    BcmCfm_CfgStatus     status;
    BcmCfm_TimeZone      timeZone;
    char                 *ntpServer1;
    char                 *ntpServer2;
} BcmCfm_NtpCfg_t, *PBcmCfm_NtpCfg_t;

typedef struct _BcmCfm_IppCfg {
    BcmCfm_CfgStatus     status;
    char                 *name;
    char                 *makeModel;
    char                 *device;
} BcmCfm_IppCfg_t, *PBcmCfm_IppCfg_t;

typedef struct _BcmCfm_TR069Cfg {
    BcmCfm_CfgStatus     status;
    uint8                         upgradesManaged;
    uint8                         upgradeAvailable;
    uint8                         informEnable;   
    uint32                        informTime;    
    uint32                        informInterval;
    uint8                         noneConnReqAuth;
    char                          *acsURL;
    char                          *acsUser;
    char                          *acsPwd;
    char                          *parameterKey; 
    char                          *connReqURL;
    char                          *connReqUser;
    char                          *connReqPwd;
    char                          *kickURL;
    char                          *provisioningCode;
} BcmCfm_TR069Cfg_t, *PBcmCfm_TR069Cfg_t;

typedef struct _BcmCfm_ACHostCfg {
    BcmCfm_CfgStatus     status;
    uint32               ipAddress;
} BcmCfm_ACHostCfg_t, *PBcmCfm_ACHostCfg_t;

typedef struct _BcmCfm_PareContCfg {
    BcmCfm_CfgStatus     status;
    char *Username;
    char *StartTime;
    char *EndTime;
    char *Days;
    char *MACAddress;
} BcmCfm_PareContCfg_t, *PBcmCfm_PareContCfg_t;

typedef struct _BcmCfm_PortTriggerCfg {
    BcmCfm_CfgStatus     status;
    char *Name;
    char *TriggerProtocol;
    char *OpenProtocol;
    uint16 TriggerPortStart;
    uint16 TriggerPortEnd;
    uint16 OpenPortStart;
    uint16 OpenPortEnd;
} BcmCfm_PortTriggerCfg_t, *PBcmCfm_PortTriggerCfg_t;

typedef struct _BcmCfm_CertificateCfg
{
   char *certName;
   char *certSubject;
   char *certType;
   char *certPassword;
   char *certContent;
   char *certPrivKey;
   char *certReqPub;
   int refCount;
}BcmCfm_CertificateCfg_t, *PBcmCfm_CertificateCfg_t;

typedef struct _BcmCfm_PortMirrorCfg {
    int      recNo;
    char   *monitorPort;
    char   *dir;
    char   *flowType;
    char   *mirrorPort;
    BcmCfm_CfgStatus   status;
} BcmCfm_PortMirrorCfg_t, *PBcmCfm_PortMirrorCfg_t;

typedef struct _BcmCfm_IPSecCfg
{
    int en;
    char *ipsConnName;
    char *ipsRemoteGWAddr;
    char *ipsLocalIPMode;
    char *ipsLocalIP;
    char *ipsLocalMask; 
    char *ipsLocalRangeLo; 
    char *ipsLocalRangeHi;
    char *ipsRemoteIPMode; 
    char *ipsRemoteIP;
    char *ipsRemoteMask;
    char *ipsRemoteRangeLo;
    char *ipsRemoteRangeHi; 
    char *ipsKeyExM;
    char *ipsAuthM; 
    char *ipsPSK;
    char *ipsCertificateName;
    char *ipsPerfectFSEn;
    char *ipsManualEncryptionAlgo;
    char *ipsManualEncryptionKey;
    char *ipsManualAuthAlgo;
    char *ipsManualAuthKey;
    char *ipsSPI;
    char *ipsPh1Mode;
    char *ipsPh1EncryptionAlgo;
    char *ipsPh1IntegrityAlgo;
    char *ipsPh1DHGroup;
    int ipsPh1KeyTime;
    char *ipsPh2EncryptionAlgo;
    char *ipsPh2IntegrityAlgo;
    char *ipsPh2DHGroup;
    int ipsPh2KeyTime;
} BcmCfm_IPSecCfg_t, *PBcmCfm_IPSecCfg_t;

typedef struct _BcmCfm_GlobalIPSecCfg
{
    char wanIP[IPSEC_SMALL_LEN];
    int BcmIPSec_delayedCmd;
}BcmCfm_GlobalIPSecCfg_t,*PBcmCfm_GlobalIPSecCfg_t;

typedef struct _BcmCfm_ACServiceCfg {
    BcmCfm_CfgStatus     statusLan;
    BcmCfm_CfgStatus     statusWan;
    BcmCfm_ACService     service;
} BcmCfm_ACServiceCfg_t, *PBcmCfm_ACServiceCfg_t;

// Data Structures for Physical Interface Object

// BcmCfm_OBJ_IFC_ETHERNET
typedef struct _BcmCfm_EthIfcCfg {
    BcmCfm_CfgStatus     status;
    uint8                mac[MAC_ADDR_LEN];
    BcmCfm_EthSpeed      speed;
    BcmCfm_EthDuplex     duplex;
    BcmCfm_CfgStatus     autoNeg;
} BcmCfm_EthIfcCfg_t, *PBcmCfm_EthIfcCfg_t;

typedef struct _BcmCfm_EthIfcSts {
    BcmCfm_LinkState     linkState;
    BcmCfm_State         autoNegState;
    BcmCfm_EthSpeed      speed;
    BcmCfm_EthDuplex     duplex;
//LGD_FOR_TR098
    uint32          rxBytes;
    uint32          rxPkts;
    uint32          rxErrors;
    uint32          rxDrops;
    uint32          txBytes;
    uint32          txPkts;
    uint32          txErrors;
    uint32          txDrops;

} BcmCfm_EthIfcSts_t, *PBcmCfm_EthIfcSts_t;

// BcmCfm_OBJ_IFC_USB
typedef struct _BcmCfm_UsbIfcCfg {
    BcmCfm_CfgStatus     status;
    uint8                mac[MAC_ADDR_LEN];
    uint8                macHost[MAC_ADDR_LEN];
} BcmCfm_UsbIfcCfg_t, *PBcmCfm_UsbIfcCfg_t;

typedef struct _BcmCfm_UsbIfcSts {
    BcmCfm_LinkState         linkState;
} BcmCfm_UsbIfcSts_t, *PBcmCfm_UsbIfcSts_t;

typedef struct _BcmCfm_WlanCurrentCountry{
    char      cntryAbbrev[CNTRY_BUF_LEN];
} BcmCfm_WlanCurrentCountry_t, *PBcmCfm_WlanCurrentCountry_t;

// BcmCfm_OBJ_IFC_WLAN
typedef struct _BcmCfm_WlanIfcCfg {
    BcmCfm_CfgStatus                    status;             // Enable/Disable Wireless Interface
    uint8                               apply;              // Apply to activate changes
    uint8                               mac[MAC_ADDR_LEN];  // MAC Address/BSSID 
    BcmCfm_Wlan_Mode                    mode;               // Wireless mode
    char                                *ssid;     // SSID
    BcmCfm_WlanCurrentCountry_t         country;            // Country Name
    BcmCfm_Wlan_Preamble                preambleType;       // Preamble type
    BcmCfm_CfgStatus                    hideSsid;           // Hide SSID
    uint32                              channel;            // Channel 
    uint32                              fragThreshold;      // Fragmentation Threshold
    uint32                              rtsThreshold;       // RTS threshold
    uint32                              dtmInterval;        // DTIM interval
    uint32                              bcnInterval;        // Beacon interval
    BcmCfm_Wlan_TxRate                  rate;               // Tx rate
    BcmCfm_Wlan_TxRate                  mcastRate;          // Multicast Tx rate
    BcmCfm_Wlan_BasicRateSet            basicRateSet;       // Basic rate set 
    BcmCfm_Wlan_gMode                   gMode;              // 54g mode selection
    BcmCfm_CfgStatus	                protection;         // Enable/Disable 54g protection
    BcmCfm_CfgStatus                    frameBurst;         // Enable/Disable XPress
    BcmCfm_CfgStatus                    apIsolation;        // Enable/Disable AP isolatation
    BcmCfm_Wlan_Band                    band;               // Band selection i.e. 802.11a (5GHz) or 802.11b/g (2.4Ghz)
    BcmCfm_CfgStatus                    afterBurner;        // Enable/Disable AfterBurner
    BcmCfm_CfgStatus                    wme;                // Enable/Disable WME
    BcmCfm_CfgStatus                    wmeNoAck;           // Enable/Disable WME No-Acknowledgement 
    uint16                              maxAssoc;           // Max Associated STA limit
    BcmCfm_Wlan_WDS_RestrictMode        lazyWds;            // WDS Restrict Mode
    uint32                              wdsTimeout;         // WDS link detection interval
    BcmCfm_Wlan_Mac_FilterMode          macFilterMode;      // MAC Filter Mode
    uint8                               antDiv;             // Antenna Selection
    BcmCfm_CfgStatus                    ure;                // Enable/Disable URE, Universal Range Extender
    BcmCfm_Wlan_RegulatoryMode          regulatory;         // Regulatory Mode (802.11h/802.11d/none)
    uint32                              dfsPreIsmInterval;  // 802.11h Channel scanning time before in service monitoring
    uint32                              dfsPostIsmInterval; // 802.11h In Service Monitoring Channel Availability Check period
    uint32                              tpcDb;              // 802.11h TPC Mitigation in dB
    uint32                              csScanTimer;        // auto channel selection periodical timer
#ifdef SUPPORT_SES        
    BcmCfm_CfgStatus                    sesEnable;          // Enable/Disable SES
    uint32                              sesEvent;           // Ses initial event    
    BcmCfm_Wlan_SesWdsMode              sesWdsMode;         // Ses WDS Mode
    BcmCfm_CfgStatus                    sesClient;          // Enable/Disable Ses client mode
    BcmCfm_Wlan_SesStatus               sesStatus;          // Ses input event status
#endif    
} BcmCfm_WlanIfcCfg_t, *PBcmCfm_WlanIfcCfg_t;

// BCMCFM_OBJ_IFC_WLAN_VIRT
typedef struct _BcmCfm_WlanIfcVirtCfg {
    BcmCfm_CfgStatus   status;        // Enable/Disable this virtual interface
    uint32             attachToIndex; // Index of physical interface
    char               ssid[SSID_LEN];// SSID
} BcmCfm_WlanIfcVirtCfg_t, *PBcmCfm_WlanIfcVirtCfg_t;

typedef struct _BcmCfm_WlanIfcSts {
    BcmCfm_LinkState    linkState;
    uint32              channel;
    BcmCfm_Wlan_PhyType phyType;
    uint32              attachToIndex; // Index of physical interface
    uint32              coreRev;
} BcmCfm_WlanIfcSts_t, *PBcmCfm_WlanIfcSts_t;

//BcmCfm_OBJ_IFC_ADSL
typedef struct _BcmCfm_AdslIfcCfg {
    BcmCfm_CfgStatus enabled;
    uint16   mode;
    uint16   bitmap;
    uint16   bitswap;
    uint16   sra;
    uint16   lpair;
} BcmCfm_AdslIfcCfg_t, *PBcmCfm_AdslIfcCfg_t;

typedef struct _BcmCfm_AdslLineEntry {
    uint8    adslLineCoding;
    uint8    adslLineType;
} adslLineEntry_t;

typedef struct _BcmCfm_AdslPhysEntry {
    char     *adslVendorID;
    uint32   adslCurrSnrMgn;
    uint32   adslCurrAtn;
    uint32   adslCurrStatus;
    uint32   adslCurrOutputPwr;
    uint32   adslCurrAttainableRate;
} adslPhysEntry_t;

typedef struct _BcmCfm_AdslFullPhysEntry {
    char     *adslSerialNumber;
    char     *adslVendorID;
    char     *adslVersionNumber;
    uint32   adslCurrSnrMgn;
    uint32   adslCurrAtn;
    uint32   adslCurrStatus;
    uint32   adslCurrOutputPwr;
    uint32   adslCurrAttainableRate;
} adslFullPhysEntry_t;

typedef struct _BcmCfm_AdslDataConnectionInfo {
   uint16   K;
   uint8    S, R, D;
} adslDataConnectionInfo_t;

typedef struct _BcmCfm_AdslConnectionInfo {
    uint8    chType; /* fast or interleaved */
    uint8    modType; /* modulation type: G.DMT or T1.413 */
    uint8    trellisCoding; /* off(0) or on(1) */
    adslDataConnectionInfo_t rcvInfo;
    adslDataConnectionInfo_t xmtInfo;
} adslConnectionInfo_t;

typedef struct _BcmCfm_Adsl2DataConnectionInfo {
    uint8    Nlp;
    uint8    Nbc;
    uint8    MSGlp;
    uint16   MSGc;
    uint32   L;
    uint16   M;
    uint16   T;
    uint16   D;
    uint16   R;
    uint16   B;
} adsl2DataConnectionInfo_t;

typedef struct _BcmCfm_Adsl2ConnectionInfo {
    uint32   adsl2Mode;
    uint32   rcvRate;
    uint32   xmtRate;
    uint8    pwrState; /* Lx state: x = 0..3 */
    adsl2DataConnectionInfo_t rcv2Info;
    adsl2DataConnectionInfo_t xmt2Info;
} adsl2ConnectionInfo_t;

typedef struct _BcmCfm_AdslConnectionDataStat {
   uint32   cntRS;	
   uint32   cntRSCor;	
   uint32   cntRSUncor;	
   uint32   cntSF;	
   uint32   cntSFErr;	
} adslConnectionDataStat_t;

typedef struct _BcmCfm_AdslConnectionStat {
    adslConnectionDataStat_t rcvStat;
    adslConnectionDataStat_t xmtStat;
} adslConnectionStat_t;

typedef struct _BcmCfm_AdslAtmConnectionDataStat {
    uint32   cntHEC;
    uint32   cntOCD;
    uint32   cntLCD;
    uint32   cntES;
    uint32   cntCellTotal;
    uint32   cntCellData;
    uint32   cntCellDrop;
    uint32   cntBitErrs;
} adslAtmConnectionDataStat_t;

typedef struct _BcmCfm_AdslAtmConnectionStat {
    adslAtmConnectionDataStat_t rcvStat;
    adslAtmConnectionDataStat_t xmtStat;
} adslAtmConnectionStat_t;

typedef struct _BcmCfm_AdslPerfCounters {
    uint32   adslLofs;
    uint32   adslLoss;
    uint32   adslLols;	/* Loss of Link failures (ATUC only) */
    uint32   adslLprs;
    uint32   adslESs;	/* Count of Errored Seconds */
    uint32   adslInits;	/* Count of Line initialization attempts (ATUC only) */
    uint32   adslUAS;	/* Count of Unavailable Seconds */
    uint32   adslSES;	/* Count of Severely Errored Seconds */
    uint32   adslLOSS;	/* Count of LOS seconds */
    uint32   adslFECs;	/* Count of FEC seconds  */
} adslPerfCounters_t;

typedef struct _BcmCfm_AdslFailureCounters {
  unsigned long           adslRetr;               /* Count of total retrains */
  unsigned long           adslRetrLof;    /* Count of retrains due to LOF */
  unsigned long           adslRetrLos;    /* Count of retrains due to LOS */
  unsigned long           adslRetrLpr;    /* Count of retrains due to LPR */
  unsigned long           adslRetrLom;    /* Count of retrains due to LOM */
  unsigned long           adslInitErr;    /* Count of training failures */
  unsigned long           adslInitTo;             /* Count of training timeouts */
  unsigned long           adslLineSearch; /* Count of line search inits */
} adslFailureCounters_t;

typedef struct _BcmCfm_AdslPerfDataEntry {
    adslPerfCounters_t       perfTotal;
    uint32                   adslPerfValidIntervals;
    uint32                   adslPerfInvalidIntervals;
    adslPerfCounters_t       perfCurr15Min;
    uint32                   adslPerfCurr15MinTimeElapsed;
    adslPerfCounters_t       perfCurr1Day;
    uint32                   adslPerfCurr1DayTimeElapsed;
    adslPerfCounters_t       perfPrev1Day;
    uint32                   adslAturPerfPrev1DayMoniSecs;
    adslFailureCounters_t    failTotal;
} adslPerfDataEntry_t;

typedef struct _BcmCfm_AdslChanEntry {
  unsigned long              adslChanIntlDelay;
  unsigned long              adslChanCurrTxRate;
  unsigned long              adslChanPrevTxRate;
  unsigned long              adslChanCrcBlockLength;
} adslChanEntry_t;

typedef struct _BcmCfm_AdslChanCounters {
  unsigned long              adslChanReceivedBlks;
  unsigned long              adslChanTransmittedBlks;
  unsigned long              adslChanCorrectedBlks;
  unsigned long              adslChanUncorrectBlks;
} adslChanCounters_t;

typedef struct _BcmCfm_AdslChanPerfDataEntry {
  adslChanCounters_t         perfTotal;
  unsigned long              adslChanPerfValidIntervals;
  unsigned long              adslChanPerfInvalidIntervals;
  adslChanCounters_t         perfCurr15Min;
  unsigned long              adslPerfCurr15MinTimeElapsed;
  adslChanCounters_t         perfCurr1Day;
  unsigned long              adslPerfCurr1DayTimeElapsed;
  adslChanCounters_t         perfPrev1Day;
  unsigned long              adslAturPerfPrev1DayMoniSecs;
} adslChanPerfDataEntry_t;

typedef struct _BcmCfm_AdslIfcSts {
    BcmCfm_AdslLinkState     linkState;     
//LGD_FOR_TR098
    unsigned long  	     showtimeStart;
    uint32                   upStreamRate;
    uint32                   downStreamRate;
    adslLineEntry_t          adslLine;
    adslPhysEntry_t          adslPhysRcv;
    adslFullPhysEntry_t      adslPhysXmt;
    adslConnectionInfo_t     adslConnInfo;
    adsl2ConnectionInfo_t    adsl2ConnInfo;
    adslConnectionStat_t     adslConnStat;
    adslPerfDataEntry_t      adslRxPerfData;
    adslPerfCounters_t       adslTxPerf;
    adslAtmConnectionStat_t  atmConnStat;
    adslChanEntry_t          adslChanIntl;
    adslChanPerfDataEntry_t  adslChanIntlPerfData;
    adslChanPerfDataEntry_t  adslChanFastPerfData;
    adslConnectionStat_t     adslStatSincePowerOn;
    adslAtmConnectionStat_t  atmStatSincePowerOn;
    adslPerfDataEntry_t      perfSinceShowTime;
} BcmCfm_AdslIfcSts_t, *PBcmCfm_AdslIfcSts_t;

// BcmCfm_OBJ_IFC_ATM
typedef struct _BcmCfm_AtmIfcCfg {
    BcmCfm_CfgStatus     status;
} BcmCfm_AtmIfcCfg_t, *PBcmCfm_AtmIfcCfg_t;

/* just following the frame work here; copy these atm statistics from bcmatmapi.h */
typedef struct _BcmCfm_AtmIfcAal2Stats
{
  uint32 ulIfOctets;
  uint32 ulIfOutOctets;
  uint32 ulIfInUcastPkts;
  uint32 ulIfOutUcastPkts;
  uint32 ulIfInErrors;
  uint32 ulIfOutErrors;
  uint32 ulIfInDiscards;
  uint32 ulIfOutDiscards;
} BcmCfm_AtmIfcAal2Stats_t, *PBcmCfm_AtmIfcAal2Stats_t;

typedef struct _BcmCfm_AtmIntfAal5Aal0Stats
{
  uint32 ulIfInOctets;
  uint32 ulIfOutOctets;
  uint32 ulIfInUcastPkts;
  uint32 ulIfOutUcastPkts;
  uint32 ulIfInErrors;
  uint32 ulIfOutErrors;
  uint32 ulIfInDiscards;
  uint32 ulIfOutDiscards;
  uint32 ulIfInPriPkts;
} BcmCfm_AtmIntfAal5Aal0Stats_t, *PBcmCfm_AtmIntfAal5Aal0Stats_t;

typedef struct _BcmCfm_AtmIntfAtmStats
{
  uint32 ulIfInOctets;
  uint32 ulIfOutOctets;
  uint32 ulIfInErrors;
  uint32 ulIfInUnknownProtos;
  uint32 ulIfOutErrors;
  // The following fields are added together to calculate ulIfInErrors.
  uint32 ulIfInHecErrors;
  // The following fields are added together to calculate ulIfInUnknownProtos.
  uint32 ulIfInInvalidVpiVciErrors;
  uint32 ulIfInPortNotEnabledErrors;
  uint32 ulIfInPtiErrors;
  uint32 ulIfInIdleCells;
  uint32 ulIfInCircuitTypeErrors;
  uint32 ulIfInOamRmCrcErrors;
  uint32 ulIfInGfcErrors;
} BcmCfm_AtmIntfAtmStats_t, *PBcmCfm_AtmIntfAtmStats_t;

typedef struct _BcmCfm_AtmIfcSts {
    BcmCfm_AtmIntfAtmStats_t AtmIntfStats;
    BcmCfm_AtmIntfAal5Aal0Stats_t Aal5IntfStats;
    BcmCfm_AtmIntfAal5Aal0Stats_t Aal0IntfStats;
} BcmCfm_AtmIfcSts_t, *PBcmCfm_AtmIfcSts_t;

// BcmCfm_OBJ_IFC_ATM_VCC
typedef struct _BcmCfm_AtmVccCfg {
    BcmCfm_CfgStatus     status;
    uint32               attachToIndex;
    uint32               portId;
    uint32               vpi;
    uint32               vci;
    BcmCfm_atmCategory   atmCategory;
    uint32               pcr;
    uint32               scr;
    uint32               mbs;
    uint32               mcr;
    BcmCfm_atmAalType    aalType;
    BcmCfm_atmEncapMode  encapMode;
    BcmCfm_CfgStatus     enbQos;
} BcmCfm_AtmVccCfg_t, *PBcmCfm_AtmVccCfg_t;

typedef struct _BcmCfm_AtmVccSts {
    uint32   vccCrcErrors;
    uint32   vccSarTimeOuts;
    uint32   vccOverSizedSdus;
    uint32   vccShortPacketErrors;
    uint32   vccLengthErrors;
} BcmCfm_AtmVccSts_t, *PBcmCfm_AtmVccSts_t;

// Network interface configuration object members
// To create this object a minimum of objId and index of the physical
// or other network interface must be provided. 

typedef struct _BcmCfm_PppCfg {
    char                   *uName;                  // Username
    char                   *passwd;                 // Password
    char                   *svcName;                // Service name
    BcmCfm_NtwkIntfPPPAuth authType;                // Auth type
    BcmCfm_CfgStatus       onDemandEnable;          // Dial-on-demand enabled
    BcmCfm_CfgStatus       staticIpEnable;          // Static IP address enabled
    uint32                 ipAddress;               // Statically assigned IP
    uint32                 idleTimeout;             // Dial-on-demand timeout
} BcmCfm_PppCfg_t, *PBcmCfm_PppCfg_t;

typedef struct _BcmCfm_IpCfg {
    BcmCfm_CfgStatus       dhcpClientEnable;  // Enable/Disable DHCP client
    uint32                 addr;              // IP address
    uint32                 mask;              // Subnet Mask
    uint32                 bcastAddr;         // broadcast address
} BcmCfm_IpCfg_t, *PBcmCfm_IpCfg_t;

typedef struct _BcmCfm_NtwkIntfCfg {
    BcmCfm_CfgStatus           status;        // Network interface status
    uint32                     attachToObjId; // Object ID of physical interface this network interface is attached to
    uint32                     attachToIndex; // Index of physical interface
    uint32                     fwEnable;
    uint32                     natEnable;
    uint32                     igmpEnable;
    uint32                     ipextEnable;
    uint16                     connIndex;
    char                       *uName;        // User-friendly network interface name
    BcmCfm_NtwkIntfProtocol    protocol;      // Interface Protocol
    uint32                     mtu;           // Maximum transfer unit
    union {
        BcmCfm_PppCfg_t        ppp;
        BcmCfm_IpCfg_t         ip;
    } proto;                                        
} BcmCfm_NtwkIntfCfg_t, *PBcmCfm_NtwkIntfCfg_t;


#define DHCP_VENDOR_ID_LEN     64
#define DHCP_MAX_VENDOR_IDS    5

typedef struct _BcmCfm_NtwkBridgeCfg {
    BcmCfm_CfgStatus       bridgeEnable;  // Configuration status
    BcmCfm_CfgStatus       status;        // Operational status
    char                   *uName;                        // Username
    //original member for vendor id, tr069c do not care it, and set '0' to each byte.
    char                   vendorid[DHCP_MAX_VENDOR_IDS][DHCP_VENDOR_ID_LEN];
} BcmCfm_NtwkBridgeCfg_t, *PBcmCfm_NtwkBridgeCfg_t;

typedef struct _BcmCfm_NtwkBridgingCfg {
    uint32 maxBridgeEntries;          // Max table size of bridge table
    uint32 maxFltEntries;             // Max table size of filter table
    uint32 maxMarkEntries;            // Max size of the marking table
    uint32 bridgeNumOfEntries;        // Num. of entries in the bridge table
    uint32 fltNumOfEntries;           // Num. of entries in the filter table
    uint32 markNumOfEntries;          // Num. of entries in the marking table
    uint32 availIntfNumOfEntries;     // Num. of entries in AvailableInterfaces table
} BcmCfm_NtwkBridgingCfg_t, *PBcmCfm_NtwkBridgingCfg_t;

typedef enum {
    FILTER_TYPE_STATIC = 0,
    FILTER_TYPE_DYNAMIC
}FILTER_INTF_TYPE;

#define BcmCfm_NtwkBridgeFltIntfRef_ALL "ALLInterfaces"
#define BcmCfm_NtwkBridgeFltIntfRef_LAN "LANInterfaces"
#define BcmCfm_NtwkBridgeFltIntfRef_WAN "WANInterfaces"

typedef struct _BcmCfm_NtwkBridgeFltCfg {
    BcmCfm_CfgStatus fltCfgEnable;    // Filter configured status
    char             *fltOperStatus;  // Filter operational status
    int32            fltBridgeRef;    // Bridge key value of the bridge object.
    char             *fltIntf;        // Equals to the key value of available interfaces table.

    FILTER_INTF_TYPE  fltType;        // Filter Type
} BcmCfm_NtwkBridgeFltCfg_t, *PBcmCfm_NtwkBridgeFltCfg_t;

typedef struct _BcmCfm_NtwkBridgeAvlIntfCfg {
    uint32 objIndex; // key associated with this interface.
    char   *intfType;     // Interface type
    char   *intfRef;      // fully qualified name of the interface.
} BcmCfm_NtwkBridgeAvlIntfCfg_t, *PBcmCfm_NtwkBridgeAvlIntfCfg_t;

typedef struct _BcmCfm_PppSts {
    BcmCfm_PPPState state;
    uint32          addr;
    uint32   	    linkupstarttime;
    uint32 	    disconnreason;
} BcmCfm_PppSts_t, *PBcmCfm_PppSts_t;

typedef struct _BcmCfm_IpSts {
    BcmCfm_DHCPState    state;
    uint32              addr;
    uint32              mask;
} BcmCfm_IpSts_t, *PBcmCfm_IpSts_t;

typedef struct _BcmCfmNtwkIntfSts {
    char            *cfmName;
    uint32          rxBytes;
    uint32          rxPkts;
    uint32          rxErrors;
    uint32          rxDrops;
    uint32          txBytes;
    uint32          txPkts;
    uint32          txErrors;
    uint32          txDrops;
    uint32          collisions;
    uint32          overruns;
    uint32          carrier;
    uint32          txqueuelen;
    uint32                     attachToObjId; // Object ID of physical interface this network interface is attached to
    BcmCfm_NtwkIntfProtocol    protocol;      // Interface Protocol
    union {
        BcmCfm_PppSts_t        ppp;
        BcmCfm_IpSts_t         ip;
    } proto;
} BcmCfm_NtwkIntfSts_t, *PBcmCfm_NtwkIntfSts_t;

typedef struct _BcmCfm_NtwkDefaultGatewayCfg {
    uint32           intfIndex;                      // Default gateway interface index
    uint32           defaultGateway;                 // Default gateway IP address
} BcmCfm_NtwkDefaultGatewayCfg_t, *PBcmCfm_NtwkDefaultGatewayCfg_t;

typedef struct _BcmCfm_NtwkDefaultGatewaySts {
    BcmCfm_State     autoState;
    uint32           intfIndex;                      // Default gateway interface index
    uint32           defaultGateway;                 // Default gateway IP address
} BcmCfm_NtwkDefaultGatewaySts_t, *PBcmCfm_NtwkDefaultGatewaySts_t;

typedef struct _BcmCfm_NtwkRouteEntryCfg {
    BcmCfm_CfgStatus status;                  // Static route status
    BcmCfm_CfgRouteType type;                 // Static route type
    uint32           intfIndex;               // Static route interface index
    uint32           ipAddress;               // Static route IP address
    uint32           subnetMask;              // Static route subnet mask
    uint32           gateway;                 // Static route gateway
    int                 metric;               // Static route metric
    BcmCfm_CfgStatus    persistance;          // Static route persistance
} BcmCfm_NtwkRouteEntryCfg_t, *PBcmCfm_NtwkRouteEntryCfg_t;

typedef struct _BcmCfm_NtwkDnsCfg {
    uint32           preferredDns;                 // Primary DNS IP address
    uint32           alternateDns;                 // Secondary DNS IP address
    char             *domainName;               // Domain name
} BcmCfm_NtwkDnsCfg_t, *PBcmCfm_NtwkDnsCfg_t;

typedef struct _BcmCfm_NtwkDnsSts {
    BcmCfm_State     autoState;
    uint32           preferredDns;                 // Primary DNS IP address
    uint32           alternateDns;                 // Secondary DNS IP address
} BcmCfm_NtwkDnsSts_t, *PBcmCfm_NtwkDnsSts_t;

typedef struct _BcmCfm_NtwkRipIntf {
    BcmCfm_CfgStatus        status;                    // Interface RIP status
    uint32                  index;                     // Interface index
    uint8                   version;                   // Interface RIP version: 1, 2 version and 3 = Both
    BcmCfm_NtwkRipOperation operation;                 // Interface RIP operation
} BcmCfm_NtwkRipIntf_t, *PBcmCfm_NtwkRipIntf_t;

typedef struct _BcmCfm_NtwkRipCfg {
    uint16               globalMode;                   // RIP mode
    uint16               debugFlag;                    // RIP debug flag
    uint16               distributeFlag;               // RIP distributed flag
    uint16               intfNum;
    BcmCfm_CfgStatus     enableLog;                    // Enable/Disable RIP log
    BcmCfm_NtwkRipIntf_t intfList[INTF_IN_LIST_MAX];   // List of interfaces that have RIP configurations
} BcmCfm_NtwkRipCfg_t, *PBcmCfm_NtwkRipCfg_t;

typedef struct BcmCfm_NtwkDhcpSrvCfg {
    BcmCfm_CfgStatus status;           // dhcp server status
//LGD_FOR_TR098
    BcmCfm_CfgStatus dhcpsrvconf;
    BcmCfm_DhcpSrvType type;           // dhcp server type
    uint32 attachToObjId;              // Network interface type attached to.
    uint32 attachToIndex;              // network interface index dhcp server is attached to
    uint32 leasedTime;                 // dhcp server leased time
    uint32 startAddress;               // dhcp server start IP address pool
    uint32 endAddress;                 // dhcp server end IP address pool
//LGD_FOR_TR098
    uint32 subnetMask;	               // dhcp server address pool subnetmask
} BcmCfm_NtwkDhcpSrvCfg_t, *PBcmCfm_NtwkDhcpSrvCfg_t;

typedef struct _BcmCfm_NtwkLanHostSts {
    BcmCfm_CfgStatus status;                  // Host status
    char   *name;                                     // Host name
    uint8   macAddress[MAC_ADDR_LEN]; // Host MAC address
    uint32 ipAddress;                               // Host IP address
    uint32 expires;                                   // Host remaining leased time
} BcmCfm_NtwkLanHostSts_t, *PBcmCfm_NtwkLanHostSts_t;

typedef struct _BcmCfm_SecDmzHostCfg {
    BcmCfm_CfgStatus     status;                       // DMZ host status
    uint32               ipAddress;                    // DMZ host IP address
} BcmCfm_SecDmzHostCfg_t, *PBcmCfm_SecDmzHostCfg_t;

typedef struct _BcmCfm_IpFilterProtocol {
   uint8
   tcp:1,
   udp:1,
   icmp:1,
   reserved:5;
} BcmCfm_IpFilterProtocol;

typedef struct _BcmCfm_MacFilterDirection {
    uint8
    incoming:1,
    outgoing:1,
    reserved:6;
} BcmCfm_MacFilterDirection;

typedef struct _BcmCfm_SecVirtualServerCfg {
    BcmCfm_CfgStatus            status;                       // Status
    BcmCfm_IpFilterProtocol     protocol;                     // protocol
    uint16                      externalPortStart;            // External port start
    uint16                      externalPortEnd;              // External port end
    uint16                      internalPortStart;            // Internal port start
    uint16                      internalPortEnd;              // Internal port end
    uint32                      ipAddress;                    // IP address
    uint32                      leaseDuration;                // lease Duration for CT
    uint32                      remoteHost;                   // remoteHost;
    char                        *uName;                       // User-friendly name
} BcmCfm_SecVirtualServerCfg_t, *PBcmCfm_SecVirtualServerCfg_t;

typedef struct _BcmCfm_SecFilterCfg {
    BcmCfm_CfgStatus            status;                       // Status
    //BcmCfm_IpFilterProtocol     protocol;                     // Protocol
    char                         *protocol;                     // Protocol
    BcmCfm_IpFilterDirection    direction;                    // Filter direction: in or out
    uint16                      srcPortStart;                 // Source port start
    uint16                      srcPortEnd;                   // Source port end
    uint16                      dstPortStart;                 // Destination port start
    uint16                      dstPortEnd;                   // Destination port end
    uint32                      intfIndex;                    // Interface index
    uint32                      srcAddr;                      // Source IP address
    uint32                      srcMask;                      // Source subnet mask
    uint32                      dstAddr;                      // Destination IP address
    uint32                      dstMask;                      // Destination subnet mask
    char                        *uName;                       // User-friendly name
} BcmCfm_SecFilterCfg_t, *PBcmCfm_SecFilterCfg_t;

typedef struct _BcmCfm_SecPortTriggerCfg {
    BcmCfm_CfgStatus            status;                       // Status
    BcmCfm_IpFilterProtocol     triggerProtocol;              // Trigger protocol
    BcmCfm_IpFilterProtocol     openProtocol;                 // Open protocol
    uint16                      triggerPortStart;             // Trigger port start
    uint16                      triggerPortEnd;               // Trigger port end
    uint16                      openPortStart;                // Open port start
    uint16                      openPortEnd;                  // Open port end
    char                        *uName;                       // User-friendly application name
} BcmCfm_SecPortTriggerCfg_t, *PBcmCfm_SecPortTriggerCfg_t;

typedef struct _BcmCfm_SecMacFilterCfg {
    BcmCfm_CfgStatus            status;                       // Status
    BcmCfm_MacFilterProtocol    protocol;                     // Protocol
    BcmCfm_MacFilterDirection   direction;                    // Direction
    uint8                       srcMac[MAC_ADDR_LEN];         // Source MAC address
    uint8                       dstMac[MAC_ADDR_LEN];         // Destination MAC address
    uint32                      intfIndex;                    // Interface index
} BcmCfm_SecMacFilterCfg_t, *PBcmCfm_SecMacFilterCfg_t;

typedef struct _BcmCfm_SecParentalControlCfg {
    BcmCfm_CfgStatus            status;                       // Status
    uint8                       startTime;                    // start time
    uint8                       endTime;                      // end time
    uint8                       mac[MAC_ADDR_LEN];            // MAC address
    uint8                       days;                         // day bit mask
    char                        *username;                    // username
} BcmCfm_SecParentalControlCfg_t, *PBcmCfm_SecParentalControlCfg_t;

#define QOS_NAME_LEN         16

typedef struct _BcmCfm_PerfQosCfg {
    uint32                      clsKey;                       // Classfication Key
    BcmCfm_CfgStatus            status;                       // Status
    uint32                      order;                        // order
    BcmCfm_IpFilterProtocol     protocol;                     // Protocol
    int8                        wanVlan8021p;                 // etherPriorityMark
    int8                        vlan8021p;                    // VLAN 8021p
    int                         srcPortStart;                 // Source port start
    int                         srcPortEnd;                   // Source port end
    int                         dstPortStart;                 // Destination port start
    int                         dstPortEnd;                   // Destination port end
    uint32                      srcAddr;                      // Source IP address
    uint32                      srcMask;                      // Source subnet mask
    uint32                      dstAddr;                      // Destination IP address
    uint32                      dstMask;                      // Destination subnet mask
    
    uint8                       destIpExclude;                // Exclude destination IP
    uint8                       srcIpExclude;                 // Source IP exclude
    uint8                       protExclude;                  // Exclude specified protocol
    uint8                       destportExclude;              // Exclude destination port(s)
    uint8                       srcPortExclude;               // Exclude source port(s)
    uint8                       etherPrioExclude;             // Exclude etherPrio exclude
    int8                        dscpCheck;                    // Use DSCP to check for classification
    uint8                       dscpExclude;                  // Exclude DSCP check
    int8                        dscpMark;                     // Mark egress packet with this DSCP
    uint32                      clsQueueKey;                  // Queue Key

    uint8                       ipoptionlist;                    //DanielXue add for HGI
    char                        optionval[OPT_BUFF_MAX_LEN]; //DanielXue add for HGI
    char                        srcMacAddr[MAC_ADDR_LEN];  // Source mac address
    char                        dstMacAddr[MAC_ADDR_LEN];  // Destination Mac address
    char                        srcMacMask[MAC_ADDR_LEN];  // Source mac mask
    char                        dstMacMask[MAC_ADDR_LEN];  // Destination mac mask
    
    char                        clsName[QOS_NAME_LEN];        // class name
    char                        lanIfcName[QOS_NAME_LEN];     // LAN interface
    
} BcmCfm_PerfQosCfg_t, *PBcmCfm_PerfQosCfg_t;

typedef struct _BcmCfm_PerfQosQMgmtCfg {
    BcmCfm_CfgStatus            status;          // Configuration status
    uint32                      maxQueues;       // Max. number of queues for each egress interface
    uint32                      maxClassEntries; // Max. size of classification table
    uint32                      numClassEntries; // Max. number of configured classification entries
    uint32                      maxQueueEntries; // Max size of queue table
    uint32                      numQueueEntries; // Number of configured queue table entries
    uint32                      defaultQueue;    // instance ID of the default queue table entry
    int                         defDSCPMark;     // Default DSCP mark not associated with any classifier
}BcmCfm_PerfQosQMgmtCfg_t,  *PBcmCfm_PerfQosQMgmtCfg_t;

typedef struct _BcmCfm_PerfQosQueueCfg {
    uint32                      queueKey;        // Instance ID
    BcmCfm_CfgStatus            status;          // Config status
    char                        *queueIntf;      // Queue interface
    int                         queuePrcd;       // Queue precedence;
}BcmCfm_PerfQosQueueCfg_t, *PBcmCfm_PerfQosQueueCfg_t;

// BCMCFM_OBJ_WLAN_SEC_MAC_FILTER
typedef struct _BcmCfm_WlanSecCfg {
    uint8                       apply;                        // Apply to activate changes         
    uint32                      attachToObjId;                // Object ID of physical interface this network interface is attached to
    uint32                      attachToIndex;                // Index of physical interface
    BcmCfm_Wlan_Auth            auth;                         // 802.11 authentication: open/shared
    BcmCfm_CfgStatus            wepEnabled;                   // Disable/Enable WEP
    uint32                      keyIndex;                     // WEP key index
    BcmCfm_WlanWepLen           keyLen;                       // WEP key length
    char*                       key[4];                       // WEP key 1-4
    BcmCfm_Wlan_AuthMode        authMode;                     // 802.1x Authentication mode: radius/none
    BcmCfm_Wlan_WpaAKm          WpaAkm;                       // WiFi user authentication key management wpa,psk,wpa2,psk2
    BcmCfm_CfgStatus            preAuth;                      // WPA2 Preauthentication
    BcmCfm_Wlan_Crypto          crypto;                       // Crypto tkip,aes,tkip+aes
    uint32                      radiusServerIP;               // RADIUS Server IP
    uint16                      radiusPort;                   // RADIUS Server port
    char                        *radiusKey;                   // RADIUS Server key
    char                        *wpaPsk;                      // WPA pre-shared key
    uint32                      gtkRekeyInterval;             // Network Rekey Interval
    uint32                      netReauthInterval;            // Network Reauthentication Interval			
#ifdef SUPPORT_TR69C    
    //TR069 specific
    BcmCfm_CfgStatus                        tr69cOverride;                   // Enable TR069 override
    BcmCfm_Wlan_BeaconType                  tr69cBeaconType;                 // BeaconType
    BcmCfm_Wlan_BasicEncryptionModes        tr69cBasicEncryptionModes;       // BasicEncryptionModes
    BcmCfm_Wlan_BasicAuthenticationMode     tr69cBasicAuthenticationMode;    // BasicAuthenticationMode   	
    BcmCfm_Wlan_WPAEncryptionModes          tr69cWPAEncryptionModes;         // WPAEncryptionModes
    BcmCfm_Wlan_WPAAuthenticationMode       tr69cWPAAuthenticationMode;      // WPAAuthenticationMode
    BcmCfm_Wlan_IEEE11iEncryptionModes      tr69cIEEE11iEncryptionModes;     // IEEE11iEncryptionModes
    BcmCfm_Wlan_IEEE11iAuthenticationMode   tr69cIEEE11iAuthenticationMode;  // IEEE11iAuthenticationMode
#endif    
} BcmCfm_WlanSecCfg_t, *PBcmCfm_WlanSecCfg_t;

// BCMCFM_OBJ_WLAN_SEC_MAC_FILTER
typedef struct _BcmCfm_WlanSecMacFilterCfg {
    uint32                      attachToIndex;  
    uint8                       mac[MAC_ADDR_LEN];              
    BcmCfm_Wlan_Mac_FilterMode  status;    
} BcmCfm_WlanSecMacFilterCfg_t, *PBcmCfm_WlanSecMacFilterCfg_t;

// BCMCFM_OBJ_WLAN_WDS_PEER
typedef struct _BcmCfm_WlanWdsPeerCfg {
    uint32              attachToIndex;                
    uint8               mac[MAC_ADDR_LEN];
    BcmCfm_CfgStatus    status;                       
} BcmCfm_WlanWdsPeerCfg_t, *PBcmCfm_WlanWdsPeerCfg_t;

// BCMCFM_OBJ_WLAN_WDS_SCANNED_MAC
typedef struct _BcmCfm_WlanWdsScannedMac {
    uint32  attachToIndex; 	
    uint8   mac[MAC_ADDR_LEN];
} BcmCfm_WlanWdsScannedMac_t, *PBcmCfm_WlanWdsScannedMac_t;

// BCMCFM_OBJ_WLAN_CLIENT_MAC
typedef struct _BcmCfm_WlanClientMac {
    uint32    attachToIndex;     
    char      mac[MAC_ADDR_LEN];
    BcmCfm_Wlan_StaStatus      status;    
} BcmCfm_WlanClientMac_t, *PBcmCfm_WlanClientMac_t;

// BCMCFM_OBJ_WLAN_SUPPORTED_COUNTRY
typedef struct _BcmCfm_WlanSupportedCountry {
    uint32    attachToIndex; 	
    char      cntryAbbrev[CNTRY_BUF_LEN];
    char      *cntryFullName;
} BcmCfm_WlanSupportedCountry_t, *PBcmCfm_WlanSupportedCountry_t;

// BCMCFM_OBJ_WLAN_SUPPORTED_CHANNEL
typedef struct _BcmCfm_WlanSupportedChannel {
    uint32    attachToIndex; 
    uint32    channel;   
} BcmCfm_WlanSupportedChannel_t, *PBcmCfm_WlanSupportedChannel_t;

typedef struct _BcmCfm_VoiceCfg_t {
    char      *proxAddr;
    char      *proxPort;
    char      *regAddr;
    char      *regPort;
    char      *domain;
    char      *codec;
    char      *ptime;
    char      *locale;
    char      *ifName;
    char      *logSrvAddr;
    char      *logSrvPort;
    char      *obProxyAddr;
    char      *obProxyPort;
    char      *ext1;
    char      *ext2;
    char      *ext3;
    char      *ext4;
    char      *passwd1;
    char      *passwd2;
    char      *passwd3;
    char      *passwd4;
    char      *dispName1;    
    char      *dispName2;    
    char      *dispName3;    
    char      *dispName4;    
    char      *authName1;       
    char      *authName2;      
    char      *authName3;       
    char      *authName4;       
    char      *lineEnable;       
    char      *regExpire;   
    char      *transport;        
    char      *digitMap;           
    char      *wmiEnable;       
    char      *codecList;         
    char      *dtmfRelay;         
    char      *t38Enable;         
    char      *vadEnable;         
    
//     //Todo: Add PSTN/STUN stuff
}BcmCfm_VoiceCfg_t, *PBcmCfm_VoiceCfg_t;

typedef struct _BcmCfm_DiagPingCfg {
  char *state;          /* None, Requested, Completed, Error_CannotResolveHostName */
  char *interface;      /* wan or lan ip-layer */
  char *host;           /* host name or ip address */
  uint32 repetition;    /* repetition to perform ping */
  uint32 timeout;    
  uint32 size;
  uint32 successCount;  
  uint32 failCount;
  uint32 avgResponseTime;   /* in ms */
  uint32 minResponseTime;
  uint32 maxResponseTime;
} BcmCfm_DiagPingCfg_t, *PBcmCfm_DiagPingCfg_t;

typedef struct _BcmCfm_DiagDslCfg {
  char *state;            /* None, Requested, Complete */
  int  ACTPSDds;          /* actual power spectual density downstream */
  int  ACTPSDus;          /* actual power spectual density upstream */
  int  ACTATPds;          /* actual aggregate transmitter power downstream */
  int  ACTATPus;          /* actual aggregate transmitter power upstream */
  int  HLINSCds;          /* linear representation scale downstream */
  char *HLINpsds;         /* downstream linear channel characteristics, list of integers */
  char *QLNpsds;          /* downstream quiet line noise, list of integers */
  char *SNRpsds;          /* downstream SNR, list of integers */
  char *BITSpsds;         /* downstream bit allocation, list of integers */
  char *GAINSpsds;        /* downstream gain allocation, list of integers */
} BcmCfm_DiagDslCfg_t, *PBcmCfm_DiagDslCfg_t;

typedef struct _BcmCfm_DiagAtmCfg {
  char *state;            /* None, Requested, Complete */
  uint32 repetition;     
  uint32 timeout;         /* in ms */
  uint32 successCount;    
  uint32 failCount;
  uint32 avgResponseTime;
  uint32 minResponseTime;
  uint32 maxResponseTime;
  uint32 vpi;
  uint32 vci;
  uint32 port;
} BcmCfm_DiagAtmCfg_t, *PBcmCfm_DiagAtmCfg_t;

#define BCM_CFM_DIAG_PING         1
#define BCM_CFM_DIAG_DSL          2
#define BCM_CFM_DIAG_ATM_LOOPACK  3
typedef struct _BcmCfm_DiagDataCfg {
    uint8 type;
    union {
        BcmCfm_DiagPingCfg_t     ping;
        BcmCfm_DiagDslCfg_t      dsl;
        BcmCfm_DiagAtmCfg_t      atm;
    } u;
} BcmCfm_DiagDataCfg_t, *PBcmCfm_DiagDataCfg_t;



#endif /* __BCM_CFM_H__ */
