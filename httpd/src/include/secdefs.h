/*****************************************************************************
//
//  Copyright (c) 2000-2002  Broadcom Corporation
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
//  Filename:       secdefs.h
//  Author:         Peter T. Tran
//  Creation Date:  06/10/02
//
//  Description:
//      Define the Global structs, enumerations for security configuations.
//
*****************************************************************************/

#ifndef __SEC_DEFS_H
#define __SEC_DEFS_H

#include "ifcdefs.h"

/********************** Include Files ***************************************/

/********************** Global Types ****************************************/

typedef enum {
   SEC_STS_OK = 0,
   SEC_STS_ERR_GENERAL,
   SEC_STS_ERR_MEMORY,
   SEC_STS_ERR_OBJECT_NOT_FOUND
} SEC_STATUS;

typedef enum {
   SEC_COMMAND_TYPE_ADD = 0,
   SEC_COMMAND_TYPE_INSERT,
   SEC_COMMAND_TYPE_REMOVE
} SEC_COMMAND_TYPE;

/********************** Global Constants ************************************/

#define SECURITY_APPID           "SecCfg"
#define SCM_VRT_SRV_NUM_ID       1
#define SCM_VRT_SRV_TBL_ID       2
#define SCM_FLT_OUT_NUM_ID       3
#define SCM_FLT_OUT_TBL_ID       4
#define SCM_DMZ_HOST_ID          7
#define SCM_PRT_TRG_NUM_ID       8
#define SCM_PRT_TRG_TBL_ID       9
#define SCM_MAC_FLT_NUM_ID       10
#define SCM_MAC_FLT_TBL_ID       11
#define SCM_ACC_CNTR_NUM_ID      12
#define SCM_ACC_CNTR_TBL_ID      13
#define SCM_ACC_CNTR_MODE_ID     14
#define QOS_CLS_NUM_ID           15
#define QOS_CLS_TBL_ID           16
#define SCM_FLT_IN_NUM_ID        17
#define SCM_FLT_IN_TBL_ID        18
#define SCM_SRV_CNTR_ID          19
#define QOS_MGMT_CFG_ID          20
#define QOS_QUEUE_NUM_ID         21
#define QOS_QUEUE_TBL_ID         22


#define SEC_BUFF_SRVNAME_LEN     32 
#define SEC_BUFF_HOSTNAME_LEN    64
#define SEC_BUFF_MAX_LEN         16 
#define SEC_CMD_MAX_LEN          2048
#define SEC_PROTO_BOTH           0
#define SEC_PROTO_TCP            1
#define SEC_PROTO_UDP            2
#define SEC_PROTO_ICMP           3
#define SEC_PROTO_MAX            4
#ifdef CT_SNMP_MIB_VERSION_2
#define SEC_PROTO_IGMP_IPTABLE          5
#endif
#define FTP_SERVER_PORT_21       "21"
#define FTP_SERVER_PORT_2121     "2121"
#define WEB_SERVER_PORT_80       "80"
#define WEB_SERVER_PORT_8080     "8080"
#define SNMP_AGENT_PORT_161      "161"
#define SNMP_AGENT_PORT_16116    "16116"
#define SSH_SERVER_PORT_22       "22"
#define SSH_SERVER_PORT_2222     "2222"
#define TELNET_SERVER_PORT_23    "23"
#define TELNET_SERVER_PORT_2323  "2323"
#define TFTP_SERVER_PORT_69      "69"
#define TFTP_SERVER_PORT_6969    "6969"

#define SEC_MAC_MAX_LEN          6

#define SEC_PROTO_NONE           0
#define SEC_PROTO_PPPOE          1        // require PPP_DISC
#define SEC_PROTO_IPV4           2        // reuire ARP
#define SEC_PROTO_IPV6           3	      // require ARP
#define SEC_PROTO_ATALK	         4
#define SEC_PROTO_IPX	         5
#define SEC_PROTO_NEtBEUI        6
#define SEC_PROTO_IGMP           7

#ifdef CT_SNMP_MIB_VERSION_2
#define SEC_PROTO_IGMP_NMS  8
#endif

#define SEC_PTR_TRG_MAX          32       // max port triggering entries.
#define SEC_VIR_SRV_MAX          32       // max virtual server entries.
#define SEC_ADD_REMOVE_ROW       6        // display add remove button if row > 6

#define SEC_MAX_PORT_MIRRORS     2       // max virtual server entries.
#define QOS_TYPE_GENERAL         1
#define QOS_TYPE_WL              2        // wireless
#define QOS_TYPE_DIFFSERV        3        // DiffServ

/********************** Global Structs ************************************/

typedef struct {
   uint32 id;
   char srvName[SEC_BUFF_SRVNAME_LEN];
   struct in_addr addr;
   struct in_addr remoteAddr;
   unsigned short protocol;
   unsigned short eStart;
   unsigned short eEnd;
   unsigned short iStart;
   unsigned short iEnd;
   unsigned short status;
   unsigned long  leaseDuration;
} SEC_VRT_SRV_ENTRY, *PSEC_VRT_SRV_ENTRY;

typedef struct {
   int protocol;
   int fromPort;
   int toPort;
} SEC_FLT_PRT_ENTRY, *PSEC_FLT_PRT_ENTRY;

typedef struct {
   char fromAddress[SEC_BUFF_MAX_LEN];
   char toAddress[SEC_BUFF_MAX_LEN];
} SEC_FLT_ADDR_ENTRY, *PSEC_FLT_ADDR_ENTRY;

typedef struct {
   int protocol;
   char fltName[SEC_BUFF_MAX_LEN];
   char srcAddr[SEC_BUFF_MAX_LEN];
   char srcMask[SEC_BUFF_MAX_LEN];
   char srcPort[SEC_BUFF_MAX_LEN];
   char dstAddr[SEC_BUFF_MAX_LEN];
   char dstMask[SEC_BUFF_MAX_LEN];
   char dstPort[SEC_BUFF_MAX_LEN];
   char wanIf[IFC_TINY_LEN];   
} SEC_FLT_ENTRY, *PSEC_FLT_ENTRY;


// PROTOCOL flag
// 16 bit: bit 0-7:  trigger protocol type -- defined above such as SEC_PROTO_TCP...
//         bit 8-15: open protocol type -- defined above such as SEC_PROTO_TCP...
typedef struct {
   unsigned short
   tProto:8,
   oProto:8;
} PROTO_FLAG;

typedef struct {
   PROTO_FLAG flag;
   unsigned short tStart;
   unsigned short tEnd;
   unsigned short oStart;
   unsigned short oEnd;
   char appName[SEC_BUFF_MAX_LEN];
} SEC_PRT_TRG_ENTRY, *PSEC_PRT_TRG_ENTRY;

#define ACL_FILE_NAME     "/var/acl.conf"

typedef struct {
   char address[SEC_BUFF_MAX_LEN];
} SEC_ACC_CNTR_ENTRY, *PSEC_ACC_CNTR_ENTRY;

#define QOS_PROTO_NONE    -1

#define QOS_PRIORITY_LOW        1
#define QOS_PRIORITY_MED        2
#define QOS_PRIORITY_HIGH       3
#define QOS_PRIORITY_HIGHEST    4

#define QOS_PHY_PORT_NONE -1
#define QOS_PHY_PORT_MIN  1
#define QOS_PHY_PORT_MAX  4
#define QOS_PHY_NAME_NONE	""

#define TOS_NONE                  -1
#define TOS_NORMAL_SERVICE        0
#define TOS_MINIMIZE_COST         2
#define TOS_MAXIMIZE_RELIABILITY  4
#define TOS_MAXIMIZE_THROUGHPUT   8
#define TOS_MINIMIZE_DELAY        16

#define DSCP_NONE                 -1
#define DSCP_AUTO                 -2
#define DSCP_MASK                 0xFC
#define DSCP_DEFAULT              0x00
#define DSCP_AF13                 0x38
#define DSCP_AF12                 0x30
#define DSCP_AF11                 0x28
#define DSCP_CS1                  0x20
#define DSCP_AF23                 0x58
#define DSCP_AF22                 0x50
#define DSCP_AF21                 0x48
#define DSCP_CS2                  0x40
#define DSCP_AF33                 0x78
#define DSCP_AF32                 0x70
#define DSCP_AF31                 0x68
#define DSCP_CS3                  0x60
#define DSCP_AF43                 0x98
#define DSCP_AF42                 0x90
#define DSCP_AF41                 0x88
#define DSCP_CS4                  0x80
#define DSCP_EF                   0xB8
#define DSCP_CS5                  0xA0
#define DSCP_CS6                  0xC0
#define DSCP_CS7                  0xE0
/*
int8 dscpMarkValues[] = {DSCP_DEFAULT, DSCP_AF13, DSCP_AF12, DSCP_AF11, DSCP_CS1,
                         DSCP_AF23, DSCP_AF22, DSCP_AF21, DSCP_CS2,
                         DSCP_AF33, DSCP_AF32, DSCP_AF31, DSCP_CS3,
                         DSCP_AF43, DSCP_AF42, DSCP_AF41, DSCP_CS4,
                         DSCP_EF, DSCP_CS5, DSCP_CS6, DSCP_CS7};

char* dscpMarkDesc[] = {"default", "AF13", "AF12", "AF11", "CS1",
                         "AF23", "AF22", "AF21", "CS2",
                         "AF33", "AF32", "AF31", "CS3",
                         "AF43", "AF42", "AF41", "CS4",
                         "EF", "CS5", "CS6", "CS7", NULL};
*/

#define PRECEDENCE_NONE   -1
#define PRECEDENCE_MIN    0
#define PRECEDENCE_MAX    7

//QoS STATUS
#define QOS_STATUS_ENABLE  1
#define QOS_STATUS_DISABLE 0

//QoS Default Value
#define QOS_DEFAULT_QUEUE      -1   /* Default queue */
#define QOS_DEFAULT_DSCPMARK   -1   /* Default DSCP mark */
#define QOS_DEFAULT_PRECEDENCE  1   /* Default precedence */
#define QOS_DEFAULT_PORT       -1   /* Default port */

typedef struct {
   uint8  enableQos;
   int    defaultDSCPMark;
   int    defaultQueue;
}QOS_MGMT_CFG, *PQOS_MGMT_CFG;

#define QOS_CLS_MAX_ENTRY 32

typedef struct {
   uint32 clsKey;                      //Classfication Key
   char   clsName[SEC_BUFF_MAX_LEN];   //WEB UI
   int8 ipoptionlist; //DanielXue for HGI
   uint32 clsOrder;                    //Classfication Order            
   int8   clsEnable;                   //Classfication Enable
   int8   protocol;                    //protocol
   int8   wanVlan8021p;                //etherPriorityMark
   int8   vlan8021p;                   //etherPriorityCheck
   
   char   srcAddr[SEC_BUFF_MAX_LEN];   //srcIP
   char   srcMask[SEC_BUFF_MAX_LEN];   //srcMask
   char   srcPort[SEC_BUFF_MAX_LEN];   //srcPort, srcPortRangeMax
   char   dstAddr[SEC_BUFF_MAX_LEN];   //destIP
   char   dstMask[SEC_BUFF_MAX_LEN];   //destMask
   char   dstPort[SEC_BUFF_MAX_LEN];   //destPort, destPortRangeMax
   int8   dscpMark;                    //DSCP Mark
   int8   dscpCheck;                   //DSCP Check
   char srcMacAddr[SEC_MAC_MAX_LEN];
   char dstMacAddr[SEC_MAC_MAX_LEN];
   char srcMacMask[SEC_MAC_MAX_LEN];
   char dstMacMask[SEC_MAC_MAX_LEN];
   uint32 clsQueueKey;                 //Queue Key

   int8   protocolExclude;             //protocolExlcude
   int8   srcPortExclude;              //srcPortExclude
   int8   srcIPExclude;                //srcIPExcludey
   int8   dstIPExclude;                //destIpExclude
   int8   dstPortExclude;              //destPortExlcude
   int8   dscpChkExclude;              //DSCP Check Exclude
   int8   etherPrioExclude;            //etherPriorityExclude

   char   lanIfcName[IFC_TINY_LEN];    //LAN interface
   int8   type;                        //just use GENERAL and WL type
   int8   wlpriority;                  //wlpriority
} QOS_CLASS_ENTRY, *PQOS_CLASS_ENTRY;

#ifdef WIRELESS
#define MAX_QOS_QUEUE_ENTRY   24    // max QoS Queue entries inclue 4 wireless queues.
#else
#define MAX_QOS_QUEUE_ENTRY   16    // max QoS Queue entries.
#endif

#ifdef WIRELESS
#define WIRELESS_PREDEFINE_QUEUE 8
#else
#define WIRELESS_PREDEFINE_QUEUE 0
#endif
#define WIRELESS_QUEUE_INTF      "wireless"
#define ATM_QUEUE_INTFPREFIX     "PVC"

#define QOS_QUEUE_MAX            (ATM_VCC_MAX + WIRELESS_PREDEFINE_QUEUE) 

typedef struct{
   uint32 queueKey;
   uint8  queueEnable;
   char   queueIntf[IFC_LARGE_LEN];
   uint32 queuePrecedence;
} QOS_QUEUE_ENTRY, *PQOS_QUEUE_ENTRY;


#define SSC_MODE_DISABLE          0
#define SSC_MODE_ENABLE           1
#define SSC_MODE_ENABLE_LAN       2
#define SSC_MODE_ENABLE_WAN       3

typedef struct {
   int modeFtp;
   int modeHttp;
   int modeIcmp;
   int modeSnmp;
   int modeSsh;
   int modeTelnet;
   int modeTftp;
} SEC_SRV_CNTR_INFO, *PSEC_SRV_CNTR_INFO;

#define MAC_FORWARD 0
#define MAC_BLOCKED 1
#define LAN_TO_WAN  0
#define WAN_TO_LAN  1
#define BOTH        2
#define MAC_DIR_MAX 3
// mac entry flag define
// 32 bit
// bit 0-15 bit: protocal type -- defined above such as SEC_PROTO_PPPOE...
// bit 16-17   : direction 0=lan->wan, 1=wan->lan, 2=lan<->wan
// bit 18-31   : reserved
typedef struct {
   unsigned int
   protocol:16,
   direction:2,
   reserved:14;
} MAC_FIL_FLAG;

typedef struct {
   MAC_FIL_FLAG flag;         
   char destMac[SEC_MAC_MAX_LEN];
   char srcMac[SEC_MAC_MAX_LEN];
   char wanIf[IFC_TINY_LEN];
} SEC_MAC_FLT_ENTRY, *PSEC_MAC_FLT_ENTRY;

#endif

