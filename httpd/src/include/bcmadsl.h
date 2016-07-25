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

//**************************************************************************
// File Name  : Adsl.h
//
// Description: This file contains the definitions, structures and function
//              prototypes for ADSL PHY interface
//
//**************************************************************************
#ifndef _BCMADSL_H_
#define _BCMADSL_H_

#include <bcmtypes.h>

//**************************************************************************
// Type Definitions
//**************************************************************************

// Return status values
typedef enum BcmAdslStatus
{
    BCMADSL_STATUS_SUCCESS = 0,
    BCMADSL_STATUS_ERROR
} BCMADSL_STATUS;

// Return status values
typedef enum AdslLinkState
{
    BCM_ADSL_LINK_UP = 0,
    BCM_ADSL_LINK_DOWN,
	BCM_ADSL_TRAINING_G992_EXCHANGE,
	BCM_ADSL_TRAINING_G992_CHANNEL_ANALYSIS,
	BCM_ADSL_TRAINING_G992_STARTED,
	BCM_ADSL_TRAINING_G994,
	BCM_ADSL_G994_NONSTDINFO_RECEIVED,
    BCM_ADSL_BERT_COMPLETE,
	BCM_ADSL_ATM_IDLE,
    BCM_ADSL_EVENT,
    BCM_ADSL_G997_FRAME_RECEIVED,
    BCM_ADSL_G997_FRAME_SENT
} ADSL_LINK_STATE;

#ifndef DISABLE_ADSL_OLD_DEF
#define	ADSL_LINK_UP		BCM_ADSL_LINK_UP
#define	ADSL_LINK_DOWN		BCM_ADSL_LINK_DOWN
#endif

/* ADSL test modes */
typedef enum AdslTestMode
{
    ADSL_TEST_NORMAL = 0,
    ADSL_TEST_REVERB,
    ADSL_TEST_MEDLEY,
    ADSL_TEST_SELECT_TONES,
    ADSL_TEST_NO_AUTO_RETRAIN,
	ADSL_TEST_MARGIN_TWEAK,
	ADSL_TEST_ESTIMATE_PLL_PHASE,
	ADSL_TEST_REPORT_PLL_PHASE_STATUS,
	ADSL_TEST_AFELOOPBACK,
    ADSL_TEST_L3,
    ADSL_TEST_DIAGMODE,
    ADSL_TEST_L0
} ADSL_TEST_MODE;

// ADSL_CHANNEL_ADDR Contains ADSL Utopia PHY addresses
typedef struct AdslChannelAddr
{
    UINT16 usFastChannelAddr;
    UINT16 usInterleavedChannelAddr;
} ADSL_CHANNEL_ADDR, *PADSL_CHANNEL_ADDR;

// ADSL_CONNECTION_INFO Contains ADSL Connection Info
typedef struct AdslConnectionInfo
{
	ADSL_LINK_STATE LinkState; 
//LGD_FOR_TR098
    unsigned long ShowtimeStart;
    UINT32 ulFastUpStreamRate;
    UINT32 ulFastDnStreamRate;
    UINT32 ulInterleavedUpStreamRate;
    UINT32 ulInterleavedDnStreamRate;
} ADSL_CONNECTION_INFO, *PADSL_CONNECTION_INFO;

/* OEM parameter definition */
#define ADSL_OEM_G994_VENDOR_ID       1    /* Vendor ID used during G.994 handshake */
#define ADSL_OEM_G994_XMT_NS_INFO     2    /* G.994 non-standard info field to send */
#define ADSL_OEM_G994_RCV_NS_INFO     3    /* G.994 received non-standard */
#define ADSL_OEM_EOC_VENDOR_ID        4    /* EOC reg. 0 */
#define ADSL_OEM_EOC_VERSION          5    /* EOC reg. 1 */
#define ADSL_OEM_EOC_SERIAL_NUMBER    6    /* EOC reg. 2 */
#define ADSL_OEM_T1413_VENDOR_ID      7    /* Vendor ID used during T1.413 handshake */
#define ADSL_OEM_T1413_EOC_VENDOR_ID  8    /* EOC reg. 0 (vendor ID) in T1.413 mode */

/* XMT gain definitions */
#define ADSL_XMT_GAIN_AUTO			  0x80000000

typedef struct
{
    int				diagCmd;
    int				diagMap;
    int				logTime;
	int				srvIpAddr;
	int				gwIpAddr;
} ADSL_DIAG, *PADSL_DIAG;

typedef struct
{
    short			x;
    short			y;
} ADSL_CONSTELLATION_POINT, *PADSL_CONSTELLATION_POINT;

#define ADSL_CONSTEL_DATA_ID		0
#define ADSL_CONSTEL_PILOT_ID		1

//#define ADSL_MIB_INFO	adslMibInfo
//typedef	ADSL_MIB_INFO   *PADSL_MIB_INFO;

typedef void (*ADSL_FN_NOTIFY_CB) (ADSL_LINK_STATE AdslLinkState, UINT32 ulParm); 

//**************************************************************************
// Function Prototypes
//**************************************************************************





#endif // _BCMADSL_H_

