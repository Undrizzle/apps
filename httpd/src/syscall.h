/*
<:copyright-gpl
 Copyright 2004 Broadcom Corp. All Rights Reserved.

 This program is free software; you can distribute it and/or modify it
 under the terms of the GNU General Public License (Version 2) as
 published by the Free Software Foundation.

 This program is distributed in the hope it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.

 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
:>
*/

/*****************************************************************************
//
******************************************************************************
//
//  Filename:       syscall.h
//  Author:         Peter T. Tran
//  Creation Date:  12/26/01
//
******************************************************************************
//  Description:
//      Define the global constants, and functions for sysCall utility.
//
*****************************************************************************/

#ifndef __SYS_CALL_H__
#define __SYS_CALL_H__
#include <netinet/in.h>

/********************** Global Functions ************************************/

#define FILE_OPEN_OK			0
#define FILE_OPEN_ERR			-1

#define MNTR_STS_OK			0
#define MNTR_STS_ADSL_DOWN		1
#define MNTR_STS_PPP_AUTH_ERR		2
#define MNTR_STS_PPP_DOWN		3
#define MNTR_STS_OPEN_ERR		4
#define MNTR_STS_DHCPC_PROGRESS  	5
#define MNTR_STS_DHCPC_FAIL   		6
#define MNTR_STS_ADSL_TRAINING		7
#define MNTR_STS_BRIDGE       		8
#define MNTR_STS_PPP_RETRY_AUTH		9

#define DHCPC_STS_FAIL           	0
#define DHCPC_STS_SUCCESS        	1
#define DHCPC_FAIL_MAX           	15

// Values for network protocol
#define PROTO_PPPOE			0
#define PROTO_PPPOA			1
#define PROTO_MER			2
#define PROTO_BRIDGE			3
#define PROTO_PPPOE_RELAY	        4
#define PROTO_IPOA			5
#define PROTO_IPOWAN                    6
#define PROTO_NONE 			10

#define SYSTEM_APPID       "SystemInfo"
#define SYS_USER_NAME_ID   1
#define SYS_PASSWORD_ID    2
#define SPT_USER_NAME_ID   3
#define SPT_PASSWORD_ID    4
#define USR_USER_NAME_ID   5
#define USR_PASSWORD_ID    6
#define SYS_DFT_GTWY_ID    7
#define SYS_DNS_ID         8
#define SYS_LOG_ID         9
#define SYS_SNMP_ID        10
#define SYS_FLAG_ID        11
#define SYS_TR69C_ID       12
#define SYS_MGMT_LOCK_ID   13

// Values that are used in syscall.c
#define SYS_CMD_LEN        256

#define IP_TBL_COL_NUM     0
#define IP_TBL_COL_PKTS    1
#define IP_TBL_COL_BYTES   2
#define IP_TBL_COL_TARGET  3
#define IP_TBL_COL_PROT    4
#define IP_TBL_COL_OPT     5
#define IP_TBL_COL_IN      6
#define IP_TBL_COL_OUT     7
#define IP_TBL_COL_SRC     8
#define IP_TBL_COL_DST     9
#define IP_TBL_COL_MAX     10

#define UPNP_IP_ADDRESS    "239.255.255.250"

#define BCM_PPPOE_CLIENT_STATE_PADO          0   /* waiting for PADO */
#define BCM_PPPOE_CLIENT_STATE_PADS          1   /* got PADO, waiting for PADS */
#define BCM_PPPOE_CLIENT_STATE_CONFIRMED     2   /* got PADS, session ID confirmed */
#define BCM_PPPOE_CLIENT_STATE_DOWN          3   /* totally down */
#define BCM_PPPOE_CLIENT_STATE_UP            4   /* totally up */
#define BCM_PPPOE_SERVICE_AVAILABLE          5   /* ppp service is available on the remote */

#define BCM_PPPOE_AUTH_FAILED                7
#define BCM_PPPOE_RETRY_AUTH                 8

#define BCM_PRIMARY_DNS                      1
#define BCM_SECONDARY_DNS                    0

#define PPP_ERROR_NONE 						0
#define PPP_ERROR_ISP_TIME_OUT					1
#define PPP_ERROR_COMMAND_ABORTED				2
#define PPP_ERROR_NOT_ENABLED_FOR_INTERNET		        3
#define PPP_ERROR_BAD_PHONE_NUMBER				4
#define PPP_ERROR_USER_DISCONNECT				5
#define PPP_ERROR_ISP_DISCONNECT				6
#define PPP_ERROR_IDLE_DISCONNECT				7
#define PPP_ERROR_FORCED_DISCONNECT				8
#define PPP_ERROR_SERVER_OUT_OF_RESOURCES		        9
#define PPP_ERROR_RESTRICTED_LOGON_HOURS		        10
#define PPP_ERROR_ACCOUNT_DISABLED				11
#define PPP_ERROR_ACCOUNT_EXPIRED				12
#define PPP_ERROR_PASSWORD_EXPIRED				13
#define PPP_ERROR_AUTHENTICATION_FAILURE		        14
#define PPP_ERROR_NO_DIALTONE					15
#define PPP_ERROR_NO_CARRIER					16
#define PPP_ERROR_NO_ANSWER					17	
#define PPP_ERROR_LINE_BUSY					18
#define PPP_ERROR_UNSUPPORTED_BITSPERSECOND		        19
#define PPP_ERROR_TOO_MANY_LINE_ERRORS			        20
#define PPP_ERROR_IP_CONFIGURATION				21
#define PPP_ERROR_UNKend					22

#define MAX_SEND_SIZE 256

typedef enum 
{
    PERSISTENT,
    NVRAM,
    BCM_IMAGE_CFE,
    BCM_IMAGE_FS,
    BCM_IMAGE_KERNEL,
    BCM_IMAGE_WHOLE,
    SCRATCH_PAD,
    FLASH_SIZE,
    SET_CS_PARAM,
} BOARD_IOCTL_ACTION;



#if 0
int __macs2b(const char *str, unsigned char *bin);
char * __getSDevTypeByModel(uint32_t model);
char * __getSModByMid(uint16_t mid);
char * __getAlarmType(uint16_t mid);
int __getAlarmLevel(uint32_t alarmCode);
int bcmGetIntfNameSocket(int socketfd, char *intfname);
#endif

int sysFlashSizeGet(void);
void bcmKillAllApps(void);
void bcmSystemReboot(void);

#endif
