#include <madCopyright.h>

/********************************************************************************
* madApiDefs.h
*
* DESCRIPTION:
*       API definitions for Marvell Phy Devices
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __MADApiDefs_h
#define __MADApiDefs_h

#ifdef __cplusplus
extern "C" {
#endif
#include <madApiTypes.h>

#ifdef _VXWORKS
#include "vxWorks.h"
#include "logLib.h"
#include "stdio.h"
#include "stdarg.h"
#elif defined(LINUX)
 #ifdef __KERNEL__
  #include "linux/kernel.h" 
//  #include "stdarg.h" 
 #endif
#else
#include "stdio.h"
#endif

/* Debug function   */

/* DEBUG_MAD should be defined in the makefile */
/*    logMsg(dbgStr,0,1,2,3,4,5); */
extern void madDbgPrint(char* format, ...);

#define MAD_DBG_INFO madDbgPrint

#ifdef _VXWORKS
#define MAD_CRITIC_INFO printf
#elif defined(LINUX)
 #ifdef __KERNEL__
  #define MAD_CRITIC_INFO printk
 #else
  #define MAD_CRITIC_INFO madDbgPrint 
/*  #define MAD_CRITIC_INFO printf */
 #endif
#else
/* #define MAD_CRITIC_INFO madDbgPrint */
#define MAD_CRITIC_INFO printf
#endif

#define MAD_DBG_ERROR MAD_CRITIC_INFO

typedef MAD_U32 MAD_SEM;

/*
 *   Logical Port value based on a Port
 *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
 *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |0|  reserved                                   |    port       |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *
 *   The following macros should be used to extract specific info
 *   from a Logical Port index
 */
typedef MAD_U32 MAD_LPORT;

#define MAD_LPORT_2_PORT(_lport)      (MAD_U8)((_lport) & 0xff)
#define MAD_PORT_2_LPORT(_port)       (MAD_32)((_port) & 0xff)

#define MAD_MAX_PORT    8

#define IS_MARVELL_OUI(_reg2,_reg3)        \
        (((_reg2)==0x0141) && (((_reg3)&0xFC00)==0x0C00))

/* Define the different device type that may exist in system */
typedef enum
{
    MAD_88E3061  = 0x1,      /* E3061, E3081 */
    MAD_88E104X  = 0x2,      /* E1040, E1041, E1042 */
    MAD_88E10X0  = 0x4,      /* E1000, E1010, E1020 */
    MAD_88E10X0S = 0x5,      /* E1000S, E1010S, E1020S */
    MAD_88E1011  = 0x6,      /* E1011, E1011S */
    MAD_88E3082  = 0x8,
    MAD_88E1112  = 0x9,
    MAD_88E1149  = 0xA,
    MAD_88E1121  = 0xB,
    MAD_88E1111  = 0xC,        /* E1111, E1115 */
    MAD_88E114X  = 0xD,
    MAD_88E1181  = 0xE,
    MAD_88E1340S = 0x1C,    /* 88E1340S */
    MAD_88E1340  = 0x1e,    /* 88E1340/x0a */
    MAD_88E1340M = 0x1f,    /* 88E1340M/x0a */
    MAD_88E1116  = 0x21,     /* E1116, E1116R */
    MAD_88E3016  = 0x22,     /* E3015, E3016, E3018 */
    MAD_88E3019  = 0x22,    /* 88E3019 */
    MAD_88E1240  = 0x23,
    MAD_88E1149R = 0x25,
    MAD_SWG65G   = 0x27,    /* switch Amber Phy */
    MAD_88E1119R = 0x28,    /* 88E1119R */
    MAD_88E1310  = 0x29,    /* 88E1310 */
    MAD_88E1540  = 0x2B    /* 88E1540L */
} MAD_DEVICE_ID;

/*
 * Definitions for Copper Auto-Neg mode
 *
 *  31 30                     16 15                       0
 * ---------------------------------------------------------
 * |         Register 9         |        Register 4        |
 * ---------------------------------------------------------
 * 
 * Each bit is identical to its related register bit.
*/
#define MAD_AUTO_AD_ASYM_PAUSE          MAD_BIT_11  /* Asymmetric Pause */
#define MAD_AUTO_AD_PAUSE               MAD_BIT_10  /* MAC Pause */
#define MAD_AUTO_AD_100FDX              MAD_BIT_8   /* 100 Full Duplex */
#define MAD_AUTO_AD_100HDX              MAD_BIT_7   /* 100 Half Duplex */
#define MAD_AUTO_AD_10FDX               MAD_BIT_6   /* 10 Full Duplex */
#define MAD_AUTO_AD_10HDX               MAD_BIT_5   /* 10 Half Duplex */

#define MAD_AUTO_AD_1000FDX             MAD_BIT_9  << 16 /* 1000 Full Duplex */
#define MAD_AUTO_AD_1000HDX             MAD_BIT_8  << 16 /* 1000 Half Duplex */
#define MAD_AUTO_AD_MANUAL_CONF_MS      MAD_BIT_12 << 16 /* Master/Slave Manual Configuration */
#define MAD_AUTO_AD_FORCE_MASTER        MAD_BIT_11 << 16 /* Manual Configure as Master */
#define MAD_AUTO_AD_PREFER_MULTI_PORT   MAD_BIT_10 << 16 /* Prefer multi-port device (MASTER) */

#define MAD_AN_MOD_AUTO_SPEED_AUTO_DUPLEX   (MAD_AUTO_AD_1000FDX         |   \
                                         MAD_AUTO_AD_100FDX     |    \
                                         MAD_AUTO_AD_100HDX     |    \
                                         MAD_AUTO_AD_10FDX      |    \
                                         MAD_AUTO_AD_10HDX)

#define MAD_AN_MOD_1000_AUTO_DUPLEX     (MAD_AUTO_AD_1000FDX         |   \
                                         MAD_AUTO_AD_1000HDX )
#define MAD_AN_MOD_100_AUTO_DUPLEX      (MAD_AUTO_AD_100FDX     |    \
                                         MAD_AUTO_AD_100HDX  )

#define MAD_AN_MOD_10_AUTO_DUPLEX       (MAD_AUTO_AD_10FDX      |    \
                                         MAD_AUTO_AD_10HDX)
#define MAD_AN_MOD_AUTO_SPEED_DUPLEX_FULL   (MAD_AUTO_AD_1000FDX         |   \
                                         MAD_AUTO_AD_100FDX     |    \
                                         MAD_AUTO_AD_10FDX  )
#define MAD_AN_MOD_AUTO_SPEED_DUPLEX_HALF   ( MAD_AUTO_AD_100HDX     |    \
                                         MAD_AUTO_AD_10HDX)
#define MAD_AN_MOD_1000_DUPLEX_FULL      MAD_AUTO_AD_1000FDX
#define MAD_AN_MOD_1000_DUPLEX_HALF      MAD_AUTO_AD_1000HDX          
#define MAD_AN_MOD_100_DUPLEX_FULL       MAD_AUTO_AD_100FDX
#define MAD_AN_MOD_100_DUPLEX_HALF       MAD_AUTO_AD_100HDX
#define MAD_AN_MOD_10_DUPLEX_FULL        MAD_AUTO_AD_10FDX 
#define MAD_AN_MOD_10_DUPLEX_HALF        MAD_AUTO_AD_10HDX     

#define MAD_AUTO_AD_MASK_REG4  (MAD_AUTO_AD_ASYM_PAUSE |    \
                                MAD_AUTO_AD_PAUSE      |    \
                                MAD_AUTO_AD_100FDX     |    \
                                MAD_AUTO_AD_100HDX     |    \
                                MAD_AUTO_AD_10FDX      |    \
                                MAD_AUTO_AD_10HDX)

#define MAD_AUTO_AD_MASK_REG9  (MAD_AUTO_AD_1000FDX         |   \
                                MAD_AUTO_AD_1000HDX         |   \
                                MAD_AUTO_AD_MANUAL_CONF_MS  |   \
                                MAD_AUTO_AD_FORCE_MASTER    |   \
                                MAD_AUTO_AD_PREFER_MULTI_PORT)

#define MAD_AUTO_AD_MASK    (MAD_AUTO_AD_MASK_REG4 | MAD_AUTO_AD_MASK_REG9)

#define MAD_FIBER_AUTO_AD_ASYM_PAUSE          MAD_BIT_8  /* Asymmetric Pause */
#define MAD_FIBER_AUTO_AD_PAUSE               MAD_BIT_7  /* MAC Pause */
#define MAD_FIBER_AUTO_AD_1000FDX             MAD_BIT_5  /* 1000X Full Duplex */
#define MAD_FIBER_AUTO_AD_1000HDX             MAD_BIT_6  /* 1000X Half Duplex */
#define MAD_FIBER_AUTO_AD_MASK_REG4  (MAD_FIBER_AUTO_AD_ASYM_PAUSE |    \
                                      MAD_FIBER_AUTO_AD_PAUSE      |    \
                                      MAD_FIBER_AUTO_AD_1000FDX         |   \
                                      MAD_FIBER_AUTO_AD_1000HDX         )

/* Definition for forced phy speed/duplex */
#define MAD_PHY_100FDX      0x1
#define MAD_PHY_100HDX      0x2
#define MAD_PHY_10FDX       0x3
#define MAD_PHY_10HDX       0x4
#define MAD_PHY_1000FDX     0x5
#define MAD_PHY_1000HDX     0x6


/* Definition for phy speed */
typedef enum
{
    MAD_SPEED_10M = 0,
    MAD_SPEED_100M,
    MAD_SPEED_1000M,
    MAD_SPEED_AUTO,    /* for autoNego speed */
    MAD_SPEED_UNKNOWN
} MAD_SPEED_MODE;


/* Definition for phy dumplex */
typedef enum
{
    MAD_HALF_DUPLEX = 0,
    MAD_FULL_DUPLEX,
    MAD_AUTO_DUPLEX,  /* for autonego duplex */
    MAD_DUPLEX_UNKNOWN
} MAD_DUPLEX_MODE;


/*
* typedef: enum MAD_PHY_AUTO_MODE
*/

typedef enum
{
    PHY_MOD_AUTO_SPEED_AUTO_DUPLEX, /* Auto for both speed and duplex. */
    PHY_MOD_1000_AUTO_DUPLEX,       /* Auto for duplex only and speed 1000Mbps.*/
    PHY_MOD_100_AUTO_DUPLEX,        /* Auto for duplex only and speed 100Mbps. */
    PHY_MOD_10_AUTO_DUPLEX,         /* Auto for duplex only and speed 10Mbps.  */
    PHY_MOD_AUTO_SPEED_DUPLEX_FULL, /* Auto for speed only and Full duplex. */
    PHY_MOD_AUTO_SPEED_DUPLEX_HALF, /* Auto for speed only and Half duplex. (1000Mbps is not supported) */
    PHY_MOD_1000_DUPLEX_FULL,       /* 1000Mbps Full duplex. */
    PHY_MOD_100_DUPLEX_FULL,        /* 100Mbps Full duplex. */
    PHY_MOD_100_DUPLEX_HALF,        /* 100Mbps Half duplex. */
    PHY_MOD_10_DUPLEX_FULL,         /* 10Mbps Full duplex. */
    PHY_MOD_10_DUPLEX_HALF          /* 10Mbps Half duplex. */
}MAD_PHY_AUTO_MODE;



/* Definition for MDI/MDIX mode */
typedef enum
{
    MAD_AUTO_MDI_MDIX = 0,
    MAD_FORCE_MDI,
    MAD_FORCE_MDIX
} MAD_MDI_MODE;

/*
 * typedef: struct MAD_1000T_MASTER_SLAVE
 *
 * Description: 1000Base-T Master/Slave Configuration
 *
 * Fields:
 *      autoConfig   - MAD_TRUE for auto-config, MAD_FALSE for manual setup.
 *      masterPrefer - MAD_TRUE if Master configuration is preferred.
 *
 */
typedef struct _MAD_1000T_MASTER_SLAVE
{
    MAD_BOOL    autoConfig;
    MAD_BOOL masterPrefer;
} MAD_1000T_MASTER_SLAVE;


/* Definition for MAC Interface Loopback Mode */
typedef enum
{
    MAD_MAC_LOOPBACK_MODE_0 = 0,
    MAD_MAC_LOOPBACK_MODE_1,
    MAD_MAC_LOOPBACK_MODE_2,
    MAD_MAC_LOOPBACK_MODE_3,
    MAD_MAC_LOOPBACK_MODE_SYS,
    MAD_MAC_LOOPBACK_MODE_SYNC_SERDES_125,
    MAD_MAC_LOOPBACK_MODE_SYNC_SERDES_5
} MAD_MAC_LOOPBACK_MODE;


/* Definition for CRC Error counter and Frame counter */ 
/* For 1340 */
#define MAD_CHECKER_CTRL_DISABLE    0
#define MAD_CHECKER_CTRL_COPPER        2
#define MAD_CHECKER_CTRL_SGMII        4
#define MAD_CHECKER_CTRL_MACSEC_TR    5
#define MAD_CHECKER_CTRL_QSGMII        6
#define MAD_CHECKER_CTRL_MACSEC_REC    7


/* Definition for packet generator */

/* Payload */
typedef enum
{
    MAD_PG_PAYLOAD_RANDOM = 0,    /* Pseudo-random */
    MAD_PG_PAYLOAD_5AA5        /* 5A,A5,5A,A5,... */
} MAD_PG_PAYLOAD;

/* Length */
typedef enum
{
    MAD_PG_LENGTH_64 = 0,        /* 64 bytes */
    MAD_PG_LENGTH_1514
} MAD_PG_LENGTH;

/* Error */
typedef enum
{
    MAD_PG_TX_NORMAL = 0,        /* No Error */
    MAD_PG_TX_ERROR            /* Tx packets with CRC error and Symbol error */
} MAD_PG_TX;

/* Enable type for 88E1340 */
typedef enum
{
    MAD_PG_DISABLE   = 0,        /* Normal operation */
    MAD_PG_EN_COPPER = 2,        /* Generate packets on Copper interface */
    MAD_PG_EN_SGMII  = 4,        /* Generate packets on SGMII interface */
    MAD_PG_EN_QSGMII = 6,        /* Generate packets on QSGMII interface */
    MAD_PG_EN_MACSEC_TX = 5,    /* Generate packets on MacSec Transmit path */
    MAD_PG_EN_MACSEC_RX = 7        /* Generate packets on MacSec Receive path */
} MAD_PG_EN_TYPE;

/* Structure for packet generator */
typedef struct
{
    MAD_PG_PAYLOAD  payload;
    MAD_PG_LENGTH   length;
    MAD_PG_TX       tx;
    MAD_PG_EN_TYPE  en_type;
} MAD_PG;


/* Definition for SGMII VOD */
typedef enum
{
    MAD_SGMII_VOD_14mV = 0,
    MAD_SGMII_VOD_112mV,
    MAD_SGMII_VOD_210mV,
    MAD_SGMII_VOD_308mV,
    MAD_SGMII_VOD_406mV
} MAD_SGMII_VOD;


/* Definition for Copper VOD */
typedef enum
{
    MAD_VOD_0_PERCENT = 0,
    MAD_VOD_2_PERCENT,
    MAD_VOD_4_PERCENT,
    MAD_VOD_6_PERCENT,
    MAD_VOD_8_PERCENT,
    MAD_VOD_10_PERCENT,
    MAD_VOD_12_PERCENT,
    MAD_VOD_14_PERCENT,
    MAD_VOD_Minus_2_PERCENT,
    MAD_VOD_Minus_4_PERCENT,
    MAD_VOD_Minus_6_PERCENT,
    MAD_VOD_Minus_8_PERCENT,
    MAD_VOD_Minus_10_PERCENT,
    MAD_VOD_Minus_12_PERCENT,
    MAD_VOD_Minus_14_PERCENT
} MAD_COPPER_VOD;

/* Definition for IEEE Test Mode */
typedef enum
{
    MAD_IEEE_TEST_WAVEFORM = 1,
    MAD_IEEE_TEST_MASTER_JITTER,
    MAD_IEEE_TEST_SLAVE_JITTER,
    MAD_IEEE_TEST_DISTORTION
} MAD_IEEE_TEST_MODE;
    
/* Definition for Synchronizing FIFO depth */
#define MAD_SYNC_TRANS_FIFO_00        0 
#define MAD_SYNC_TRANS_FIFO_01        1
#define MAD_SYNC_TRANS_FIFO_02        2 
#define MAD_SYNC_TRANS_FIFO_03        3 

#define MAD_SYNC_RECV_FIFO_00        0 
#define MAD_SYNC_RECV_FIFO_01        1 
#define MAD_SYNC_RECV_FIFO_02        2 
#define MAD_SYNC_RECV_FIFO_03        3 

/* Maximal number of phys a device may have. */
/* #define MAX_SWITCH_PORTS    8 */

#define VERSION_MAX_LEN     30

/*
 * Typedef: struct MAD_VERSION
 *
 * Description: This struct holds the package version.
 *
 * Fields:
 *      version - string array holding the version.
 *
 */
typedef struct
{
    MAD_U8   version[VERSION_MAX_LEN];
}MAD_VERSION;

/* Definition for LED Control */
/* Definition for LED Control Mode */
typedef struct
{
    MAD_U8   ledCtrl[6];
    MAD_U8   ledPolarity[6];
    MAD_U8   ledMixPercen0;
    MAD_U8   ledMixPercen1;
    MAD_U8   ledForceInt;
    MAD_U8   ledPulseDuration;
    MAD_U8   ledBlinkRate;
    MAD_U8   ledSppedOffPulse_perio;
    MAD_U8   ledSppedOnPulse_perio;
    MAD_U8   ledFuncMap3;
    MAD_U8   ledFuncMap2;
}MAD_LED_CTRL_DATA;
    
/* Definition for Duplex Mismatch indicator */
typedef struct
{
    MAD_U8   count_65;
    MAD_U8   count_97;
    MAD_U8   count_129;
    MAD_U8   count_192;
}MAD_LATE_COLLISION_COUNTER;
    

/*
 * Typedef: struct MAD_INT_TYPE
 *
 * Description: This struct holds interrupt cause for each different group.
 *
 * Fields:
 *      intGroup0 - interrupt cause for Int Group 0
 *      intGroup1 - interrupt cause for Int Group 1
 *
 */
typedef struct
{
    MAD_U32   intGroup0;
    MAD_U32   intGroup1;
} MAD_INT_TYPE;

/*
 * Definition of Interrupt Cause Group 0
 *
 *  31 30                     16 15                       0
 * ---------------------------------------------------------
 * |   SGMII Interface Int      |   Copper Iterface Int    |
 * ---------------------------------------------------------
 * 
 * Each bit is identical to its related register bit.
*/

#define MAD_COPPER_AUTONEGO_ERROR       MAD_BIT_15
#define MAD_COPPER_SPEED_CHANGED        MAD_BIT_14
#define MAD_COPPER_DUPLEX_CHANGED        MAD_BIT_13
#define MAD_COPPER_PAGE_RECEIVED        MAD_BIT_12
#define MAD_COPPER_AUTO_NEG_COMPLETED    MAD_BIT_11
#define MAD_COPPER_LINK_STATUS_CHANGED    MAD_BIT_10
#define MAD_COPPER_SYMBOL_ERROR            MAD_BIT_9
#define MAD_COPPER_FALSE_CARRIER        MAD_BIT_8
#define MAD_COPPER_CROSSOVER_CHANGED    MAD_BIT_6
#define MAD_COPPER_DOWNSHIFT_DETECT        MAD_BIT_5
#define MAD_COPPER_ENERGY_DETECT        MAD_BIT_4
#define MAD_COPPER_DTE_DETECT_CHANGED    MAD_BIT_2
#define MAD_COPPER_POLARITY_CHANGED        MAD_BIT_1
#define MAD_COPPER_JABBER                MAD_BIT_0

/*  1181 1116*/
#define MAD_SGMII_FIFO_FLOW                MAD_BIT_7 << 16
#define MAD_SGMII_FIFO_IDLE_INSERT        MAD_BIT_3 << 16
#define MAD_SGMII_FIFO_IDLE_DELET        MAD_BIT_2 << 16

/* 1149R. 1240. 1340. 1119R, 1310 */
#define MAD_SGMII_TRANS_FIFO_FLOW            MAD_BIT_7 << 16
#define MAD_SGMII_RECV_FIFO_FLOW            MAD_BIT_6 << 16
#define MAD_SGMII_TRANS_FIFO_IDLE_INSERT    MAD_BIT_3 << 16
#define MAD_SGMII_TRANS_FIFO_IDLE_DELET        MAD_BIT_2 << 16
#define MAD_SGMII_RECV_FIFO_IDLE_INSERT        MAD_BIT_1 << 16
#define MAD_SGMII_RECV_FIFO_IDLE_DELET        MAD_BIT_0 << 16
/*
 * Definition of Interrupt Cause Group 1.
 * 
 *  31 30                     16 15                       0
 * ---------------------------------------------------------
 * |      Not defined           |    Fiber Iterface Int    |
 * ---------------------------------------------------------
 * 
 * Each bit is identical to its related register bit.
*/

#define MAD_FIBER_SPEED_CHANGED            MAD_BIT_14
#define MAD_FIBER_DUPLEX_CHANGED        MAD_BIT_13
#define MAD_FIBER_PAGE_RECEIVED            MAD_BIT_12
#define MAD_FIBER_AUTO_NEG_COMPLETED    MAD_BIT_11
#define MAD_FIBER_LINK_STATUS_CHANGED    MAD_BIT_10
#define MAD_FIBER_SYMBOL_ERROR            MAD_BIT_9
#define MAD_FIBER_FALSE_CARRIER            MAD_BIT_8
#define MAD_FIBER_FIFO_FLOW             MAD_BIT_7
#define MAD_FIBER_ENERGY_DETECT            MAD_BIT_4


typedef struct _MAD_DEV MAD_DEV;

/*
 * semaphore related definitions.
 * User Applications may register Semaphore functions using following definitions
 */
typedef enum
{
    MAD_SEM_EMPTY,
    MAD_SEM_FULL
} MAD_SEM_BEGIN_STATE;

typedef MAD_SEM (*FMAD_SEM_CREATE)(MAD_SEM_BEGIN_STATE state);
typedef MAD_STATUS (*FMAD_SEM_DELETE)(MAD_SEM semId);
typedef MAD_STATUS (*FMAD_SEM_TAKE)(MAD_SEM semId, MAD_U32 timOut);
typedef MAD_STATUS (*FMAD_SEM_GIVE)(MAD_SEM semId);

typedef struct
{
    FMAD_SEM_CREATE    semCreate;     /* create semapore */
    FMAD_SEM_DELETE    semDelete;     /* delete the semapore */
    FMAD_SEM_TAKE    semTake;    /* try to get a semapore */
    FMAD_SEM_GIVE    semGive;    /* return semaphore */
} MAD_SEM_ROUTINES;

/*
 * definitions for registering MII access functions.
 *
*/
typedef MAD_BOOL (*FMAD_READ_MII)(
                        MAD_DEV*   dev,
                        unsigned int phyAddr, 
                        unsigned int miiReg, 
                        unsigned int* value);
typedef MAD_BOOL (*FMAD_WRITE_MII)(
                        MAD_DEV*   dev,
                        unsigned int phyAddr, 
                        unsigned int miiReg, 
                        unsigned int value);
typedef MAD_BOOL (*FMAD_INT_HANDLER)(
                        MAD_DEV*   dev,
                        MAD_U16*);

typedef struct _MAD_BSP_FUNCTIONS
{
    FMAD_READ_MII     readMii;    /* read MII Registers */
    FMAD_WRITE_MII     writeMii;    /* write MII Registers */
    FMAD_SEM_CREATE    semCreate;     /* create semapore */
    FMAD_SEM_DELETE    semDelete;     /* delete the semapore */
    FMAD_SEM_TAKE    semTake;    /* try to get a semapore */
    FMAD_SEM_GIVE    semGive;    /* return semaphore */
} MAD_BSP_FUNCTIONS;



#define MAD_MDI_PAIR_NUM         4    /* (1,2),(3,6),(4,5),(7,8) */
#define MAD_CHANNEL_PAIR_NUM     2    /* (channel A,B),(channel C,D) */

/*
 * typedef: enum MAD_TEST_STATUS
 *
 * Description: Enumeration of VCT test status
 *
 * Enumerations:
 *      MAD_TEST_FAIL    - virtual cable test failed.
 *      MAD_NORMAL_CABLE - normal cable.
 *      MAD_IMPEDANCE_MISMATCH - impedance mismatch.
 *      MAD_OPEN_CABLE   - open in cable.
 *      MAD_SHORT_CABLE  - short in cable.
 *
 */
typedef enum
{
    MAD_TEST_FAIL,
    MAD_NORMAL_CABLE,
    MAD_IMPEDANCE_MISMATCH,
    MAD_OPEN_CABLE,
    MAD_SHORT_CABLE,
} MAD_TEST_STATUS;


/*
 * Definition for MAD_NORMAL_CABLE_LEN
 *
 * Description: Enumeration for normal cable length
 *
 * Enumerations:
 *      MAD_LESS_THAN_50M - cable length less than 50 meter.
 *      MAD_50M_80M       - cable length between 50 - 80 meter.
 *      MAD_80M_110M      - cable length between 80 - 110 meter.
 *      MAD_110M_140M     - cable length between 110 - 140 meter.
 *      MAD_MORE_THAN_140 - cable length more than 140 meter.
 *      MAD_UNKNOWN_LEN   - unknown length.
 *
 */

#define MAD_SHORTER_THAN_50M    -1
#define MAD_BETWEEN_50M_80M     -2
#define MAD_BETWEEN_80M_110M    -3
#define MAD_BETWEEN_110M_140M   -4
#define MAD_LONGER_THAN_140M    -5
#define MAD_UNKNOWN_LEN         -6


/*
 * typedef: enum MAD_CABLE_LEN
 *
 * Description: Enumeration cable length
 *
 * Enumerations:
 *      normCableLen - cable length.
 *                     if zero or positive, it's the cable length in meters.
 *                     if negative, it's one of the MAD_NORMAL_CABLE_LEN definition
 *                     defined above.
 *      errCableLen  - for cable failure the estimate fault distance in meters.
 *
 */
typedef union
{
    MAD_16 normCableLen;
    MAD_16 errCableLen;

} MAD_CABLE_LEN;

/*
 * typedef: struct MAD_CABLE_STATUS
 *
 * Description: virtual cable diagnostic status per MDI pair.
 *
 * Fields:
 *      cableStatus - VCT cable status.
 *      cableLen    - VCT cable length.
 *    phyType        - type of phy (100M phy or Gigabit phy)
 */
typedef struct
{
    MAD_TEST_STATUS  cableStatus[MAD_MDI_PAIR_NUM];
    MAD_CABLE_LEN    cableLen[MAD_MDI_PAIR_NUM];
    MAD_U16        phyType;

} MAD_CABLE_STATUS;


/*
 * typedef: enum MAD_CABLE_TYPE
 *
 * Description: Enumeration of Receiver Channel Assignment
 *              This enum type is not used. Please refer to the MAD_RX_CHANNEL
 *              defined below.
 *
 * Enumerations:
 *        MAD_STRAIGHT_CABLE    - straight cable
 *                            channel A on MDI[1], channel B on MDI[0]  
 *                            channel C on MDI[3], channel D on MDI[2]  
 *      MAD_CROSSOVER_CABLE - crossover cable
 *                            channel A on MDI[0], channel B on MDI[1]  
 *                            channel C on MDI[2], channel D on MDI[3]  
 */
typedef enum
{
    MAD_STRAIGHT_CABLE,
    MAD_CROSSOVER_CABLE

} MAD_CABLE_TYPE;

/*
 * typedef: enum MAD_RX_CHANNEL
 *
 * Description: Enumeration of Receiver Channel Assignment
 *
 * Enumerations:
 *        MAD_CHANNEL_A   - Channel A
 *        MAD_CHANNEL_B   - Channel B
 *        MAD_CHANNEL_C   - Channel C
 *        MAD_CHANNEL_D   - Channel D
 */
typedef enum
{
    MAD_CHANNEL_A,
    MAD_CHANNEL_B,
    MAD_CHANNEL_C,
    MAD_CHANNEL_D
} MAD_RX_CHANNEL;


/*
 * typedef: enum MAD_POLARITY_STATUS
 *
 * Description: Enumeration of polarity status
 *
 * Enumerations:
 *        MAD_POSITIVE    - positive polarity
 *      MAD_NEGATIVE    - negative polarity
 */
typedef enum
{
    MAD_POSITIVE,
    MAD_NEGATIVE

} MAD_POLARITY_STATUS;


/*
 * typedef: struct MAD_1000BT_EXTENDED_STATUS
 *
 * Description: Currently the 1000Base-T PCS can determine the cable polarity
 *         on pairs A,B,C,D; crossover on pairs A,B and C,D; and skew among 
 *        the pares. These status enhance the capability of the virtual cable tester
 *
 * Fields:
 *      isValid        - MAD_TRUE if this structure have valid information, 
 *                       MAD_FALSE otherwise.
 *                      It is valid only if 1000BASE-T Link is up.
 *      pairSwap    - Receive channel assignement
 *        pairPolarity- MAD_POSITIVE, if polarity is positive,
 *                      MAD_NEGATIVE, otherwise
 *        pairSkew    - pair skew in units of ns
 *        cableLen    - cable length based on DSP
 */
typedef struct
{
    MAD_BOOL            isValid;
    MAD_RX_CHANNEL      pairSwap[MAD_MDI_PAIR_NUM];
    MAD_POLARITY_STATUS pairPolarity[MAD_MDI_PAIR_NUM];
    MAD_U32             pairSkew[MAD_MDI_PAIR_NUM];
    MAD_U32                cableLen[MAD_MDI_PAIR_NUM];
} MAD_1000BT_EXTENDED_STATUS;


/*
 * if isGigPhy in MAD_CABLE_STATUS is not MAD_TRUE, cableStatus and cableLen 
 * will have only 2 pairs available.
 * One is RX Pair and the other is TX Pair.
 */
#define MAD_MDI_RX_PAIR        0    /* cableStatus[0] or cableLen[0] */
#define MAD_MDI_TX_PAIR        1    /* cableStatus[1] or cableLen[1] */

/* definition for Phy Type */
#define MAD_PHY_100M        0 /* 10/100M phy, E3082 or E3083 */
#define MAD_PHY_1000M        1 /* Gigabit phy, the rest phys */
#define MAD_PHY_10000M        2 /* 10 Gigabit phy, unused */


/* Definition for Advance Virtual Cable Test */

/*
 * typedef: enum MAD_ADV_VCT_MODE
 *
 * Description: Enumeration of Advanced VCT Mode and Transmitter channel select
 *
 * Enumerations:
 *      MAD_ADV_VCT_FIRST_PEAK   - first peak above a certain threshold is reported.
 *      MAD_ADV_VCT_MAX_PEAK     - maximum peak above a certain threshold is reported.
 *        MAD_ADV_VCT_OFFSE         - offset
 *        MAD_ADV_VCT_SAMPLE_POINT - sample point
 *
 *        MAD_ADV_VCT_NO_CROSSPAIR - Transmitter channel select is 000
 *        MAD_ADV_VCT_CROSSPAIR    - Transmitter channelselect is 100/101/110/111
 *   Example: mode = MAD_ADV_VCT_FIRST_PEAK | MAD_ADV_VCT_CROSSPAIR.
 */
typedef enum
{
    /* Advanced VCT Mode */
    MAD_ADV_VCT_TCS_NO_CROSSPAIR        = 0,
    MAD_ADV_VCT_TCS_CROSSPAIR_0            = 0x4,
    MAD_ADV_VCT_TCS_CROSSPAIR_1            = 0x5,
    MAD_ADV_VCT_TCS_CROSSPAIR_2            = 0x6,
    MAD_ADV_VCT_TCS_CROSSPAIR_3            = 0x7
} MAD_ADV_VCT_TRANS_CHAN_SEL;

typedef enum
{
    /* Advanced VCT Mode */
    MAD_ADV_VCT_SAVG_2        = 0,
    MAD_ADV_VCT_SAVG_4        = 1,
    MAD_ADV_VCT_SAVG_8        = 2,
    MAD_ADV_VCT_SAVG_16        = 3,
    MAD_ADV_VCT_SAVG_32        = 4,
    MAD_ADV_VCT_SAVG_64        = 5,
    MAD_ADV_VCT_SAVG_128    = 6,
    MAD_ADV_VCT_SAVG_256    = 7
} MAD_ADV_VCT_SAMPLE_AVG;

typedef enum
{
    /* Advanced VCT Mode */
    MAD_ADV_VCT_MAX_PEAK        =0x00,
    MAD_ADV_VCT_FIRST_PEAK        =0x01,
    MAD_ADV_VCT_OFFSET            =0x02,
    MAD_ADV_VCT_SAMPLE_POINT    =0x03
#if 0
    /* Advanced VCT Transmiter Channel Select */
    MAD_ADV_VCT_NO_CROSSPAIR    =0x04,  /* 23_5 bit 13:11 = 000 */
    MAD_ADV_VCT_CROSSPAIR        =0x08   /* 23_5 bit 13:11 = 100, 101, 110, 111 */
#endif
} MAD_ADV_VCT_MOD;
/*
#define MAD_ADV_VCT_TEST_MODE_MASK        0x3
#define MAD_ADV_VCT_CROSSPAIR_MASK        0xc
*/

typedef unsigned int MAD_ADV_VCT_PEAKDET_HYST;

typedef struct
{
    MAD_ADV_VCT_MOD                    mode;
    MAD_ADV_VCT_TRANS_CHAN_SEL      transChanSel;
    MAD_ADV_VCT_SAMPLE_AVG            sampleAvg;
    MAD_ADV_VCT_PEAKDET_HYST        peakDetHyst;
    MAD_16                            samplePointDist; 
} MAD_ADV_VCT_MODE;

/*
 * typedef: enum MAD_ADV_VCT_STATUS
 *
 * Description: Enumeration of Advanced VCT status
 *
 * Enumerations:
 *      MAD_ADV_VCT_FAIL     - advanced virtual cable test failed.
 *                             cable lengh cannot be determined.
 *      MAD_ADV_VCT_NORMAL   - normal cable.
 *                             cable lengh may not be determined.
 *      MAD_ADV_VCT_IMP_GREATER_THAN_115 - impedance mismatch > 115 ohms
 *                             cable lengh is valid.
 *      MAD_ADV_VCT_IMP_LESS_THAN_85 - impedance mismatch < 85 ohms
 *                             cable lengh is valid.
 *      MAD_ADV_VCT_OPEN      - cable open
 *                             cable lengh is valid.
 *      MAD_ADV_VCT_SHORT      - cable shorted
 *                             cable lengh is valid.
 *      MAD_ADV_VCT_CROSS_PAIR_SHORT - cross pair short.
 *                             cable lengh for each channel is valid.
 */
typedef enum
{
    MAD_ADV_VCT_FAIL,
    MAD_ADV_VCT_NORMAL,
    MAD_ADV_VCT_IMP_GREATER_THAN_115,
    MAD_ADV_VCT_IMP_LESS_THAN_85,
    MAD_ADV_VCT_OPEN,
    MAD_ADV_VCT_SHORT,
    MAD_ADV_VCT_CROSS_PAIR_SHORT
} MAD_ADV_VCT_STATUS;


/*
 * typedef: struct MAD_CROSS_PAIR_LIST
 *
 * Description: strucuture for cross pair short channels.
 *
 * Fields:
 *      channel - cross pair short channel list
 *                channel[i] is MAD_TRUE if the channel[i] is cross pair short
 *                with the current channel under test.
 *      dist2fault - estimated distance to the shorted location.
 *                   valid only if related channel (above) is MAD_TRUE.
 */
typedef struct _MAD_CROSS_SHORT_LIST
{
    MAD_BOOL    channel[MAD_MDI_PAIR_NUM];
    MAD_16     dist2fault[MAD_MDI_PAIR_NUM];
} MAD_CROSS_SHORT_LIST;


/*
 * typedef: struct MAD_ADV_CABLE_STATUS
 *
 * Description: strucuture for advanced cable status.
 *
 * Fields:
 *      cableStatus - VCT cable status for each channel.
 *      crossShort  - cross pair short list for each channel. 
 *                    Valid only if relative cableStatus is MAD_ADV_VCT_CROSS_PAIR_SHORT.
 *      dist2fault  - estimated distance to fault for each channel.
 *                    Valid if relative cableStatus is one of the followings:
 *                      MAD_ADV_VCT_NORMAL
 *                      MAD_ADV_VCT_IMP_GREATER_THAN_115
 *                      MAD_ADV_VCT_IMP_LESS_THAN_85,
 *                      MAD_ADV_VCT_OPEN, or
 *                        MAD_ADV_VCT_SHORT
 *      offset  - The offset seen at the receiver.
 *      sampleAmp  - returned amplitude for sample point.
 */
typedef struct
{
    MAD_ADV_VCT_STATUS   cableStatus[MAD_MDI_PAIR_NUM];
    union {
        MAD_CROSS_SHORT_LIST crossShort;
        MAD_16     dist2fault;
        MAD_16     offset;
        MAD_16     sampleAmp;
    }u[MAD_MDI_PAIR_NUM];
} MAD_ADV_CABLE_STATUS;

/*
    typedef struct VCT DATA 
*/
typedef enum
{
    MAD_SM_FREE=0,
    MAD_SM_PENDING,
} MAD_SM_STATE;

typedef struct 
{
    union
    {
        MAD_ADV_CABLE_STATUS    advCableStatus;
        MAD_CABLE_STATUS        cableStatus;
        MAD_1000BT_EXTENDED_STATUS    ExtCableStatus;
    }    vctResult;
}    MAD_VCT_DATA_STRUCT;

typedef struct
{
  MAD_SM_STATE    smSt;
  MAD_VCT_DATA_STRUCT vctData;
  void *    smCfg;
}    MAD_SM_DATA_STRUCT;

/***************************************************************************************/
/* Definition for Alternate Virtual Cable Test */

/* Alternate VCT Phy cable diagnostics control bits */

#define  MAD_ALT_VCT_RUN_ALT_VCT_BIT            (1<<15)
#define  MAD_ALT_VCT_RUN_AUTONEGO_CYCLE_BIT        (1<<14)
#define  MAD_ALT_VCT_DISABLE_CROSS_PAIR_BIT        (1<<13)
#define  MAD_ALT_VCT_RUN_AFTER_BREAK_LNK_BIT    (1<<12)
#define  MAD_ALT_VCT_CABLE_DIAG_STATUS_BIT        (1<<11)
#define  MAD_ALT_VCT_CABLE_LENGTH_UNIT_BIT        (1<<10)

#define MAD_ALT_VCT_DIAG_TYPE_MASK  (MAD_ALT_VCT_RUN_AUTONEGO_CYCLE_BIT | \
                                    MAD_ALT_VCT_DISABLE_CROSS_PAIR_BIT | \
                                    MAD_ALT_VCT_RUN_AFTER_BREAK_LNK_BIT)

/*
 * typedef: enum MAD_ALT_VCT_FAULT_CODE
 *
 * Description: Enumeration of Alternate VCT Fault Code
 *
 * Enumerations:
 *      MAD_ALT_VCT_FAULT_INVALID           - Alternate VCT invalid
 *      MAD_ALT_VCT_FAULT_PAIR_OK           - Alternate VCT Pair Ok
 *      MAD_ALT_VCT_FAULT_PAIR_OPEN           - Alternate VCT Pair Open
 *      MAD_ALT_VCT_FAULT_SAME_PAIR_SHORT  - Alternate VCT same Pair short
 *      MAD_ALT_VCT_FAULT_CROSS_PAIR_SHORT - Alternate VCT cross Pair short
 *      MAD_ALT_VCT_FAULT_PAIR_BUSY           - Alternate VCT Pair busy
 */
typedef enum
{
    MAD_ALT_VCT_FAULT_INVALID = 0,
    MAD_ALT_VCT_FAULT_PAIR_OK,
    MAD_ALT_VCT_FAULT_PAIR_OPEN,
    MAD_ALT_VCT_FAULT_SAME_PAIR_SHORT,
    MAD_ALT_VCT_FAULT_CROSS_PAIR_SHORT,
    MAD_ALT_VCT_FAULT_PAIR_BUSY = 9
} MAD_ALT_VCT_FAULT_CODE;

/*
 * typedef: struct MAD_ALT_CABLE_STATUS
 *
 * Description: strucuture for alternate cable status.
 *
 * Fields:
 *      cableFaultCode - Alternate VCT cable fault code for each channel.
 *      cableLen  - length of fault in meter or centermeter based on MAD_ALT_VCT_CABLE_LENGTH_UNIT_BIT
 */
typedef struct
{
    MAD_ALT_VCT_FAULT_CODE   cableFaultCode[MAD_MDI_PAIR_NUM];
    MAD_U16                     cableLenUnit;
    MAD_16                     cableLen[MAD_MDI_PAIR_NUM];
} MAD_ALT_CABLE_STATUS;


#define MAD_PAT_SEL_PRBS_7        0
#define MAD_PAT_SEL_PRBS_23        1
#define MAD_PAT_SEL_PRBS_31        2
#define MAD_PAT_SEL_101010        3

/*
 * typedef: struct MAD_PRBS_GEN_CONFIG
 *
 * Description: strucuture for PRBS Generator and Checker
 *
 * Fields:
 *        genEn;        -    1: enter the test mode. 0: stop the test mode.
 *        countEn;    -    1: enable PRBS Checker Counter. 0: disable
 *      polarCheckInv - Invert Checker Polarity
 *      polarGenInv - Invert Generator Polarity
 *      counterLock  - PRBS lock, 0: counter free run, 
 *                        1: Do not start counting until PRBS locks first
 *      patternSel - 
 *                MAD_PAT_SEL_PRBS_7        0
*                MAD_PAT_SEL_PRBS_23        1
*                MAD_PAT_SEL_PRBS_31        2
*                MAD_PAT_SEL_101010        3
*        errCounter  - 32 bits counter;
*/
typedef struct
{
    MAD_U8     genEn;    
    MAD_U8     countEn;    
    MAD_U8   polarCheckInv;
    MAD_U8   polarGenInv;
    MAD_U8   counterLock;
    MAD_U8   patternSel;
    MAD_U32     errCounter;
} MAD_PRBS_GEN_CONFIG;

/**************************************************************************************/
/*
 * typedef: struct MAD_PHY_INFO
 *
 * Description: strucuture for PHY's feature information
 *
 * Fields:
 *    anyPage     - each bit represents if the corresponding register is any page
 *    featureSet  - list of features supported
 *    featureSet1 - list of extended features supported
 *    hwMode      - PHY Mode for each port
 *    hwRealMode  - Current PHY Mode for each port
 *    vctType     - VCT Register Type
 *    dteType     - DTE Register Type
 *    dwshiftType     - Down shift Register Type
 *    pktGenType  - Pkt Generator Register Type
 *    crcCheckType    - CRC Check/Counter Register Type
 *    macIfLoopType   - MAC IF Loopback Register Type
 *    lineLoopType    - Line Loopback Register Type
 *    exLoopType      - External Loopback Register Type
 *    pageType        - Page Restriction Type
 */
typedef struct _MAD_PHY_INFO
{
    MAD_U32    anyPage;
    MAD_U32    featureSet;
    MAD_U32    featureSet1;
    MAD_U16    hwMode[MAD_MAX_PORT];
    MAD_U16    hwRealMode[MAD_MAX_PORT];
    MAD_U8    pageType;
    MAD_U8    vctType;
    MAD_U8    exStatusType;
    MAD_U8    dteType;
    MAD_U8    dwshiftType;
    MAD_U8    pktGenType;
    MAD_U8    crcCheckType;
    MAD_U8    macIfLoopType;
    MAD_U8    lineLoopType;
    MAD_U8    exLoopType;
    MAD_U8    ieeeTestType;
#ifdef MAD_DSP_VCT
    MAD_U8  dspVctType;
#endif
    MAD_U8    interruptType;

    MAD_U8    reserve2;
} MAD_PHY_INFO;


/* MAD_PHY_INFO hwMode definition */
#define MAD_PHY_MODE_NONE            0x0000
#define MAD_PHY_MODE_COPPER            0x0001
#define MAD_PHY_MODE_FIBER            0x0002
#define MAD_PHY_MODE_GMII           0x0004
#define MAD_PHY_MODE_SGMII           0x0008
#define MAD_PHY_MODE_RGMII           0x0010
#define MAD_PHY_MODE_SERDES_CORE    0x0020
#define MAD_PHY_MODE_100FX           0x0040
#define MAD_PHY_MODE_GBIC           0x0080  /* Added */
#define MAD_PHY_MODE_QSGMII           0x0100    /* For 1340 */
#define MAD_PHY_MODE_AUTO_DETECT_SC    0x0200    /* For 1340 */
#define MAD_PHY_MODE_AUTO_DETECT_XC    0x0400    /* For 1340 */
#define MAD_PHY_MODE_SGMII_MEDIA    0x0800    /* For 1340 */
#define MAD_PHY_MODE_QSGMII_MEDIA    0x1000    /* For 1340 */

/* MAD_PHY_INFO featureSet definition */
#define MAD_PHY_VCT_CAPABLE                0x0001
#define MAD_PHY_DTE_CAPABLE                0x0002
#define MAD_PHY_EX_CABLE_STATUS            0x0004
#define MAD_PHY_DW_SHIFT_CAPABLE        0x0008
#define MAD_PHY_PKT_GENERATOR            0x0010
#define MAD_PHY_CRC_CHECKER                0x0020
#define MAD_PHY_SYNCRO_FIFO                0x0040
#define MAD_PHY_MAC_IF_LOOP                0x0100
#define MAD_PHY_LINE_LOOP                0x0200
#define MAD_PHY_EXTERNAL_LOOP            0x0400
#define MAD_PHY_RESTRICTED_PAGE            0x0800
#define MAD_PHY_IEEE_TEST                0x1000
#define MAD_PHY_ADV_VCT_CAPABLE            0x2000
#define MAD_PHY_LED_CONTROL                0x4000
#define MAD_PHY_EX_POWER_DOWN            0x8000
#define MAD_PHY_VOD_ADJUST                0x10000
#define MAD_PHY_FULL_PAGE_ADDR            0x20000  /* start from 1340 */
#define MAD_PHY_30XX_MODE                0x40000  /* 3015/3016/3082/3019, which do not support 1G */
#define MAD_PHY_DETECT_POWER_DOWN        0x80000

#ifdef MAD_DSP_VCT
#define MAD_PHY_DSP_VCT                0x10000000
#endif

/* MAD_PHY_INFO featureSet1 definition */
#define MAD_PHY_SNOOPING_CONTROL        0x0001
#define MAD_PHY_SYNC_FIFO                0x0002
#define MAD_PHY_1G_LINK_DOWN_INDICAT    0x0004
#define MAD_PHY_RX_ER_BYTE_CAPTURE        0x0008
#define MAD_PHY_QSGMII_PRBS_GEN            0x0010
#define MAD_PHY_125G_PRBS_GEN            0x0020
#define MAD_PHY_UNIDIRECT_TRABSMIT        0x0040
#define MAD_PHY_FLP_EXCH_COMP_NO_LNK    0x0080
#define MAD_PHY_DUPLEX_MISMATCH_INDC    0x0100
#define MAD_PHY_LINK_DISCONNECT_INDC    0x0400
#define MAD_PHY_ALT_VCT_CAPABLE            0x4000
#define MAD_PHY_PTP_TAI_CAPABLE            0x8000
#define MAD_PHY_SYNC_ETHNET               0x10000

#define MAD_PHY_MACSEC_CAPABLE            0x10000000


#ifdef MAD_DSP_VCT

/* VCT Register Type */
#define MAD_PHY_DSP_VCT_TYPE0    0    /* Do not support */
#define MAD_PHY_DSP_VCT_TYPE1    1    /* E1149 */
#endif

/* VCT Register Type */
#define MAD_PHY_VCT_TYPE0    0    /* Do not support */
#define MAD_PHY_VCT_TYPE1    1    /* 10/100 Fast Ethernet */
#define MAD_PHY_VCT_TYPE2    2    /* Uses Register 28_0 */
#define MAD_PHY_VCT_TYPE3    3    /* Uses Register 28_0 with work around */
#define MAD_PHY_VCT_TYPE4    4    /* Uses Register 16_5 */

/* Extended Status Register Type */
#define MAD_PHY_EX_STATUS_TYPE0    0    /* Do not support 88E10XX 88E30XX*/
#define MAD_PHY_EX_STATUS_TYPE1    1    /* 88E1111, 88E1115, 88E1141, 88E1145 */
#define MAD_PHY_EX_STATUS_TYPE2    2    /* 88E1112, 88E1116, 88E1119, 88E1125,88E1149, 88E1181 */
#define MAD_PHY_EX_STATUS_TYPE3    3    /* 88E1240 */
#define MAD_PHY_EX_STATUS_TYPE4    4    /* 88E1119R, 88E1149R, 88E1340 88E1310*/

/* DTE Register Type */
#define MAD_PHY_DTE_TYPE1    1    /* E3082, 10M/100M without page support */
#define MAD_PHY_DTE_TYPE2    2    /* E1111, E104x, E114x, 1000M without page support */
#define MAD_PHY_DTE_TYPE3    3    /* E1111, E104x, E114x, 1000M without page but with work around */
#define MAD_PHY_DTE_TYPE4    4    /* All new, 1000M with page support */
#define MAD_PHY_DTE_TYPE5    5    /* E3082, 10M/100M with page support Rev>9*/

/* Pkt Generator Register Type */
#define MAD_PHY_PKTGEN_TYPE1    1    /* Uses Register 30 (114x,1111) */
#define MAD_PHY_PKTGEN_TYPE2    2    /* Uses Register 16 (1181,1149,1240,1116,1112)*/
#define MAD_PHY_PKTGEN_TYPE3    3    /* Uses Register 25 */
#define MAD_PHY_PKTGEN_TYPE4    4    /* Uses Register 16 + (1340)*/

/* CRC Checker Register Type */
#define MAD_PHY_CRCCHECK_TYPE1    1    /* Uses Register 16_6, 17_6 (1181,1149,1240,1116,1112)*/ 
#define MAD_PHY_CRCCHECK_TYPE2    2    /* Uses Register 30_16, 30_12 (114x)*/
#define MAD_PHY_CRCCHECK_TYPE3    3    /* Uses Register 29, 30 (104x,1111) */
#define MAD_PHY_CRCCHECK_TYPE4    4    /* Uses Register 16_6, 17_6 (1340)*/ 
                  /* Uses Register 25, 24 */

/* MAC Interface Loopback Register Type */
#define MAD_PHY_LOOPBACK_TYPE0    0    /* Don't do anything */
#define MAD_PHY_LOOPBACK_TYPE1    1    /* For E1149 E1240(SGMII) */
#define MAD_PHY_LOOPBACK_TYPE2    2    /* For E1116, E1121 (RGMII)*/
#define MAD_PHY_LOOPBACK_TYPE3    3    /* For E1112 (SGMII) */
#define MAD_PHY_LOOPBACK_TYPE4    4    /* For DEV_E1111, E114x like device */
#define MAD_PHY_LOOPBACK_TYPE5    5    /* For DEV_E1181 like device */
#define MAD_PHY_LOOPBACK_TYPE6    6    /* For DEV_S15LV and DEV_G15LV like device */
#define MAD_PHY_LOOPBACK_TYPE7    7    /* For E3016 and E3082 */
#define MAD_PHY_LOOPBACK_TYPE8    8    /* For E1340 */

/* Line Loopback Register Type */
#define MAD_PHY_LINE_LB_TYPE1    1    /* 0_2.14 */
#define MAD_PHY_LINE_LB_TYPE2    2    /* 21_2.14 */
#define MAD_PHY_LINE_LB_TYPE3    3    /* 20.14 */
#define MAD_PHY_LINE_LB_TYPE4    4    /* 16.12 */
#define MAD_PHY_LINE_LB_TYPE5    5    /* 28.4 for E3015/3016 */
#define MAD_PHY_LINE_LB_TYPE6    6    /* 21_2.14 , 16_1.12 16_1.18, 14_4.12, 16_4.18 for 1340 */

/* External Loopback Register Type */
#define MAD_PHY_EX_LB_TYPE0    0    /* Don't do anything */
#define MAD_PHY_EX_LB_TYPE1    1    /* For DEV_E1111 like dev */
#define MAD_PHY_EX_LB_TYPE2    2    /* For DEV_E1149 like dev 1240, 1340*/

/* IEEE Test Register Type */
#define MAD_PHY_IEEE_TEST_TYPE0    0    /* No special setup required */
#define MAD_PHY_IEEE_TEST_TYPE1    1    /* For DEV_E1181 like dev */
#define MAD_PHY_IEEE_TEST_TYPE2    2    /* For DEV_E1149 like dev */
#define MAD_PHY_IEEE_TEST_TYPE3    3    /* For DEV_E114X like dev */
#define MAD_PHY_IEEE_TEST_TYPE4    4    /* For DEV_E1340 like dev */

/* Down Shift Type */
#define MAD_PHY_DOWNSHIFT_TYPE0    0    /* No special setup required */
#define MAD_PHY_DOWNSHIFT_TYPE1    1    /* For 104x, 1111, 114x */
#define MAD_PHY_DOWNSHIFT_TYPE2    2    /* For 1181, 1149, 1116, 1112 1240 1340*/

/* Restricted Page Access Type */
#define MAD_PHY_PAGE_WRITE_BACK    0    /* For every device */
#define MAD_PHY_PAGE_DIS_AUTO1    1    /* For 88E1111 type */
#define MAD_PHY_PAGE_DIS_AUTO2    2    /* For 88E1121 type */
#define MAD_PHY_NO_PAGE            3    /* No Pages */

/* MAD_PHY_INFO Interrupt type definition */
#define MAD_PHY_INTERRUPT_TYPE_0    0    /* For 1000 1010 1011 1020 1112 1113  */
#define MAD_PHY_INTERRUPT_TYPE_1    1    /* For 3015 3016 3019 3061 3082 */
#define MAD_PHY_INTERRUPT_TYPE_2    2    /* For 1121, 1181, 1116, 1118, 1149, 1240 1340 1310 119R*/



/* reset mode for 1340 */
#define MAD_RESET_MODE_PORT_COPPER            0x0001
#define MAD_RESET_MODE_PORT_FIBER            0x0002
#define MAD_RESET_MODE_PORT_QSGMII            0x0003
#define MAD_RESET_MODE_ALL_QSGNII            0x0004
#define MAD_RESET_MODE_PORT_COUNTER            0x0005
#define MAD_RESET_MODE_ALL_CHIP                0x0006

/* Copper Energe Detect Modes */
#define MAD_ENERGE_DETECT_MODE_NO            0x0000
#define MAD_ENERGE_DETECT_MODE_YES            0x0001
#define MAD_ENERGE_DETECT_MODE_1            0x0002
#define MAD_ENERGE_DETECT_MODE_2            0x0003

/* Snooping control Modes */
#define MAD_SNOOPING_OFF                0x0000
#define MAD_SNOOPING_FROM_NET            0x0002
#define MAD_SNOOPING_FROM_MAC            0x0003

/* 1000T Link down delays */
#define MAD_1G_LINKDOWN_DELAY_0            0x0000
#define MAD_1G_LINKDOWN_DELAY_10        0x0001
#define MAD_1G_LINKDOWN_DELAY_20        0x0002
#define MAD_1G_LINKDOWN_DELAY_40        0x0003

/* Sync Clocking mode for 1340 */
#define MAD_SYNC_CLK_MODE_PORT_COPPER            0x0001
#define MAD_SYNC_CLK_MODE_PORT_125SERDES        0x0002
#define MAD_SYNC_CLK_MODE_PORT_QSGMII            0x0003

/* Sync Clocking Recovered clock for 1340 */
#define MAD_SYNC_CLK_RCLK1            0x0000
#define MAD_SYNC_CLK_RCLK2            0x0001
#define MAD_SYNC_CLK_RCLK1_2        0x0002
#define MAD_SYNC_CLK_RCLK_NO        0x0003

/* Sync Clocking Recovered clock frequency for 1340 */
#define MAD_SYNC_CLK_FREQ_25M            0x0000
#define MAD_SYNC_CLK_FREQ_125M            0x0001
#define MAD_SYNC_CLK_FREQ_NO            0x0002

/* Sync Clocking Reference clock  for 1340 */
#define MAD_SYNC_CLK_REF_CLK_XTAL        0x0000
#define MAD_SYNC_CLK_REF_CLK_SCLK        0x0001
#define MAD_SYNC_CLK_REF_CLK_NO            0x0002
/*
    flags for MAD_DEV structure
*/
#define MAD_FLAG_SHORT_CABLE_FIX    0x0001    /* requires short cable fix on adv vct */



#define MAX_PTP_CONSECUTIVE_READ    4

/*
 * Typedef: enum MAD_PTP_OPERATION
 *
 * Description: Defines the PTP (Precise Time Protocol) Operation type
 *
 * Fields:
 *      MAD_PTP_WRITE_DATA             - Write data to the PTP register
 *      MAD_PTP_READ_DATA            - Read data from PTP register
 *      MAD_PTP_READ_MULTIPLE_DATA    - Read multiple data from PTP register
 *      MAD_PTP_READ_TIMESTAMP_DATA    - Read timestamp data from PTP register
 *                    valid bit will be reset after read
 */
typedef enum
{
    MAD_PTP_WRITE_DATA             = 0x3,
    MAD_PTP_READ_DATA              = 0x4,
    MAD_PTP_READ_MULTIPLE_DATA    = 0x6,
    MAD_PTP_READ_TIMESTAMP_DATA    = 0x8,
} MAD_PTP_OPERATION;


/*
 * Typedef: enum MAD_PTP_SPEC
 *
 * Description: Defines the PTP (Precise Time Protocol) SPEC type
 *
 * Fields:
 *      MAD_PTP_IEEE_1588         - IEEE 1588
 *      MAD_PTP_IEEE_802_1AS    - IEEE 802.1as
 */
typedef enum
{
    MAD_PTP_IEEE_1588        = 0x0,
    MAD_PTP_IEEE_802_1AS    = 0x1
} MAD_PTP_SPEC;


/*
 *  typedef: struct MAD_PTP_OP_DATA
 *
 *  Description: data required by PTP Operation
 *
 *  Fields:
 *      ptpPort        - physical port of the device, Except 0xE for TAI 0xF for global
 *      ptpBlock     - block of addresses
 *      ptpPhyPort     - MAD Phy port number
 *      ptpAddr     - register address
 *      ptpData     - data for ptp register.
 *      ptpMultiData- used for multiple read operation.
 *      nData         - number of data to be read on multiple read operation.
 */
typedef struct
{
    MAD_U32    ptpPort;
    MAD_U32    ptpBlock;
    MAD_U32    ptpPhyPort;
    MAD_U32    ptpAddr;
    MAD_U32    ptpData;
    MAD_U32    ptpMultiData[MAX_PTP_CONSECUTIVE_READ];
    MAD_U32    nData;
} MAD_PTP_OP_DATA;


/*
 *  typedef: struct MAD_PTP_CONFIG
 *
 *  Description: PTP configuration parameters
 *
 *  Fields:
 *      ptpEType    - PTP Ether Type
 *      msgIdTSEn     - Message IDs that needs time stamp
 *      tsArrPtr     - Time stamp arrival time counter pointer (either Arr0Time or Arr1Time)
 *      ptpArrIntEn    - PTP port arrival interrupt enable
 *      ptpDepIntEn    - PTP port departure interrupt enable
 *      transSpec    - This is to differentiate between various timing protocols.
 *      msgIdStartBit     - Message ID starting bit in the PTP common header
 *      disTSOverwrite     - disable time stamp counter overwriting until the corresponding
 *                          timer valid bit is cleared.
 */
typedef struct
{
    MAD_U32    ptpEType;
    MAD_U32    msgIdTSEn;
    MAD_U32    tsArrPtr;
    MAD_U32    ptpArrIntEn;
    MAD_U32    ptpDepIntEn;
    MAD_PTP_SPEC    transSpec;
    MAD_U32    msgIdStartBit;
    MAD_BOOL    disTSOverwrite;
} MAD_PTP_CONFIG;


/*
 *  typedef: struct MAD_PTP_GLOBAL_CONFIG
 *
 *  Description: PTP global configuration parameters
 *
 *  Fields:
 *      ptpEType    - PTP Ether Type
 *      msgIdTSEn     - Message IDs that needs time stamp
 *      tsArrPtr     - Time stamp arrival time counter pointer (either Arr0Time or Arr1Time)
 */
typedef struct
{
    MAD_U32    ptpEType;
    MAD_U32    msgIdTSEn;
    MAD_U32    tsArrPtr;
} MAD_PTP_GLOBAL_CONFIG;


/*
 *  typedef: struct MAD_PTP_PORT_CONFIG
 *
 *  Description: PTP configuration parameters for each port
 *
 *  Fields:
 *      transSpec    - This is to differentiate between various timing protocols.
 *      disTSpec     - Disable Transport specific check
 *      etJump         - offset to Ether type start address in bytes
 *      ipJump         - offset to IP header start address counting from Ether type offset
 *      ptpArrIntEn    - PTP port arrival interrupt enable
 *      ptpDepIntEn    - PTP port departure interrupt enable
 *      disTSOverwrite     - disable time stamp counter overwriting until the corresponding
 *                          timer valid bit is cleared.
 */
typedef struct
{
    MAD_PTP_SPEC    transSpec;
    MAD_BOOL    disTSpec;
    MAD_U32    etJump;
    MAD_U32    ipJump;
    MAD_BOOL    ptpArrIntEn;
    MAD_BOOL    ptpDepIntEn;
    MAD_BOOL    disTSOverwrite;
} MAD_PTP_PORT_CONFIG;


/*
 * Typedef: enum MAD_PTP_TIME
 *
 * Description: Defines the PTP Time to be read
 *
 * Fields:
 *      MAD_PTP_WRITE_DATA             - Write data to the PTP register
 *      MAD_PTP_READ_DATA            - Read data from PTP register
 *      MAD_PTP_READ_MULTIPLE_DATA    - Read multiple data from PTP register
 */
typedef enum
{
    MAD_PTP_ARR0_TIME = 0x0,
    MAD_PTP_ARR1_TIME = 0x1,
    MAD_PTP_DEP_TIME = 0x2
} MAD_PTP_TIME;


/*
 * Typedef: enum MAD_PTP_INT_STATUS
 *
 * Description: Defines the PTP Port interrupt status for time stamp
 *
 * Fields:
 *      MAD_PTP_INT_NORMAL        - No error condition occurred
 *      MAD_PTP_INT_OVERWRITE     - PTP logic has to process more than one PTP frame
 *                                  that needs time stamping before the current read.
 *                                Only the latest one is saved.
 *      MAD_PTP_INT_DROP          - PTP logic has to process more than one PTP frame
 *                                  that needs time stamping before the current read.
 *                                Only the oldest one is saved.
 *
 */
typedef enum
{
    MAD_PTP_INT_NORMAL         = 0x0,
    MAD_PTP_INT_OVERWRITE     = 0x1,
    MAD_PTP_INT_DROP         = 0x2
} MAD_PTP_INT_STATUS;


/*
 *  typedef: struct MAD_PTP_TS_STATUS
 *
 *  Description: PTP port status of time stamp
 *
 *  Fields:
 *      isValid        - time stamp is valid
 *      status        - time stamp error status
 *      timeStamped    - time stamp value of a PTP frame that needs to be time stamped
 *      ptpSeqId    - sequence ID of the frame whose time stamp information has been captured
 */
typedef struct
{
    MAD_BOOL    isValid;
    MAD_U32    timeStamped;
    MAD_U32    ptpSeqId;
    MAD_PTP_INT_STATUS    status;
} MAD_PTP_TS_STATUS;


/*
 *  typedef: struct MAD_PTP_PORT_DISCARD_STATS
 *
 *  Description: PTP port discard statistics. The counter (4 bit wide) wraps around after 15. 
 *
 *  Fields:
 *      tsDepDisCtr    - PTP departure frame discard counter for PTP frames that need time stamping.
 *      ntsDepDisCtr    - PTP departure frame discard counter for PTP frames that do not need time stamping.
 *      tsArrDisCtr    - PTP arrival frame discard counter for PTP frames that need time stamping.
 *      ntsArrDisCtr    - PTP arrival frame discard counter for PTP frames that do not need time stamping.
 */
typedef struct
{
    MAD_U32    tsDepDisCtr;
    MAD_U32    ntsDepDisCtr;
    MAD_U32    tsArrDisCtr;
    MAD_U32    ntsArrDisCtr;
} MAD_PTP_PORT_DISCARD_STATS;



/*
 *  typedef: struct MAD_TAI_EVENT_CONFIG
 *
 *  Description: TAI event capture configuration parameters
 *
 *  Fields:
 *      eventOverwrite    - event capture overwrite
 *      eventCtrStart         - event counter start
 *      intEn             - event capture interrupt enable
 */
typedef struct
{
    MAD_BOOL    eventOverwrite;
    MAD_BOOL    eventCtrStart;
    MAD_BOOL    intEn;
} MAD_TAI_EVENT_CONFIG;


/*
 *  typedef: struct MAD_TAI_EVENT_STATUS
 *
 *  Description: TAI event capture status
 *
 *  Fields:
 *      isValid        - eventTime is valid
 *      eventTime     - PTP global time when event is registered.
 *      eventCtr    - event capture counter. increamented only if eventCtrStart is set.
 *      eventErr    - isValid is already set when a new event is observed.
 */
typedef struct
{
    MAD_BOOL    isValid;
    MAD_U32    eventTime;
    MAD_U32    eventCtr;
    MAD_BOOL    eventErr;
} MAD_TAI_EVENT_STATUS;


typedef enum
{
    MAD_TAI_TRIG_PERIODIC_PURSE = 0,    /* generate periodic purse */
    MAD_TAI_TRIG_ON_GIVEN_TIME        /* generate purse when 
                                    PTP global time matches with given time */
} MAD_TAI_TRIG_MODE;


/*
 *  typedef: struct MAD_TAI_TRIGGER_CONFIG
 *
 *  Description: TAI trigger generator configuration parameters
 *
 *  Fields:
 *      intEn         - trigger generator interrupt enable
 *      mode        - trigger mode, either MAD_TAI_TRIG_PERIODIC_PURSE or 
 *                      MAD_TAI_TRIG_ON_GIVEN_TIME
 *      trigGenAmt     - if mode is MAD_TAI_TRIG_PERIODIC_PURSE,
 *                      this value is used as a clock period in TSClkPer increments
 *                      If mode is MAD_TAI_TRIG_ON_GIVEN_TIME,
 *                      this value is used to compare with PTP global time.
 *      pulseWidth        - pulse width in units of TSClkPer.
 *                      this value should be 1 ~ 0xF. If it's 0, no changes made.
 *                      this value is valid only in MAD_TAI_TRIG_ON_GIVEN_TIME mode.
 *      trigClkComp    - trigger mode clock compensation amount in pico sec.
 *                      this value is valid only in MAD_TAI_TRIG_PERIODIC_PURSE mode.
 */
typedef struct
{
    MAD_BOOL    intEn;
    MAD_TAI_TRIG_MODE     mode;
    MAD_U32    trigGenAmt;
    MAD_U32    pulseWidth;
    MAD_U32    trigClkComp;
} MAD_TAI_TRIGGER_CONFIG;




/*
 * Typedef: struct MAD_SYS_CONFIG
 *
 * Description: System configuration Parameters struct.
 *
 * Fields:
 *    smiBaseAddr        - SMI Address of the phy device
 *    BSPFunctions    - Group of BSP specific functions.
 *                SMI Read/Write and Semaphore Related functions.
 */
typedef struct
{
    MAD_U32    smiBaseAddr;    /* SMI Address of the device */
    MAD_BSP_FUNCTIONS BSPFunctions;
}MAD_SYS_CONFIG;

typedef enum
{
    MAD_MSEC_REV_Z0A=0,
    MAD_MSEC_REV_Y0A=1,
    MAD_MSEC_REV_A0B=2,
	MAD_MSEC_REV_A1A=3,
	MAD_MSEC_REV_A2A=4,
    MAD_MSEC_REV_UNKNOWN  = -1
} MAD_MSEC_REV;

/* comatible for USB test */
typedef struct  _MAD_MSEC_CTRL {
  int      dev_num;    /* indicates the device number (0 if only one) when multiple devices are present on SVB.*/
  int      port_num;   /* Indicates which port (0 to 4) is requesting CPU */
  MAD_U16 prev_addr;  /* < Prev write address */
  MAD_U16 prev_dataL; /* < Prev dataL value */
  MAD_MSEC_REV msec_rev;  /* 1340 revision */
} MAD_MSEC_CTRL;

/*
 * Typedef: struct MAD_DEV
 *
 * Description: Includes phy configuration data.
 *
 * Fields:
 *   deviceId        - The device type identifier.
 *   revision        - The device revision number.
 *   baseRegAddr    - SMI Base address of the Phy device.
 *   numOfPorts        - Number of ports in the device.
 *   flags            - device flags (defined above)
 *   devGroup        - the device group
 *   devName        - name of the device
 *     multiAddrSem    - Semaphore for RMON counter access
 *   phyInfo        - device information
 *     fgtReadMii        - platform specific SMI register Read function
 *     fgtWriteMii    - platform specific SMI register Write function
 *     semCreate        - function to create semapore
 *     semDelete        - function to delete the semapore
 *     semTake        - function to get a semapore
 *     semGive        - function to return semaphore
 *     appData        - application data that user may use. driver won't use this field at all.
 */
struct _MAD_DEV
{
    MAD_DEVICE_ID   deviceId;
    MAD_U8     revision;
    MAD_U8     baseRegAddr;
    MAD_U8     numOfPorts;
    MAD_U8     reserved;
    MAD_U16     flags;
    MAD_U16     devEnabled;
    MAD_U32        devGroup;
    MAD_U32     devName;
    MAD_SEM     multiAddrSem;
    MAD_SEM        ptpRegsSem;

    MAD_PHY_INFO    phyInfo;

    FMAD_READ_MII    fmadReadMii;
    FMAD_WRITE_MII     fmadWriteMii;

    FMAD_SEM_CREATE    semCreate;     /* create semapore */
    FMAD_SEM_DELETE    semDelete;     /* delete the semapore */
    FMAD_SEM_TAKE   semTake;    /* try to get a semapore */
    FMAD_SEM_GIVE   semGive;    /* return semaphore */
    MAD_SM_DATA_STRUCT * SMDataPtr;         /* Internal state machine data */
    MAD_MSEC_CTRL  msec_ctrl;

    void*       appData;

};


#ifdef __cplusplus
}
#endif

#endif /* __madApi_h */
