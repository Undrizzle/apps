/* 
 * Copyright 2010, Marvell International Ltd.
 * 
 * THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.
 * NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT
 * OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE
 * DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.
 * THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED
 * OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.
 */


#ifndef MAD_ERROR_DEFS_H
#define MAD_ERROR_DEFS_H

#define MAD_RUN_ST_MASK            0x0000000F

#define MAD_ERR_ST_DRV_0_BASE    0x00000000
#define MAD_ERR_ST_DRV_1_BASE    0x00000100
#define MAD_ERR_ST_API_0_BASE    0x00010000
#define MAD_ERR_ST_API_1_BASE    0x01000000


#define MAD_ERR_ST_DRV_0_MASK    0x000000FF
#define MAD_ERR_ST_DRV_1_MASK    0x0000FF00
#define MAD_ERR_ST_API_0_MASK    0x00FF0000
#define MAD_ERR_ST_API_1_MASK    0xFF000000

/* Definitions of MAD driver status*/
/* MAD status are constructed from following parts:
     MAD normal status :            0x00000000-0x0000000f
     MAD driver level status 0:        0x00000010-0x000000ff    Bit 00 - Bit 07
     MAD driver level status 1:        0x00000100-0x0000ff00   Bit 08 - Bit 15
     MAD API level status 0:        0x00000010-0x000000ff    Bit 16 - Bit 23
     MAD API level status 1:        0x00000100-0x0000ff00   Bit 24 - Bit 31
*/
/* MAD run status */
#define MAD_OK        0   /* Operation succeeded */
#define MAD_FAIL    1    /* Operation failed    */

#define MAD_PENDING        2    /* MAD Pending  */
#define MAD_VCT_AGAIN    3    /* MAD VCT need do again */

#define MAD_API_CABLE_LEN_NOT_READY        4 /*Ready bit of Cable length resiter is not set. */
#define MAD_API_LINK_DOWN                5 /*Link should be established */
#define MAD_API_NOT_FULL_DUPLEX            6 /*Only Full Duplex mode is supported. */
#define MAD_TIMEOUT                        7 /* Time Out*/
#define MAD_API_NOT_HALF_DUPLEX            8 /*Only Half Duplex mode is supported. */

#define MAD_API_ST_END            0xf

#define MAD_DRV_ERR_ST_START    0x10

/* MAD error status driver level 0 */
#define MAD_DRV_INVALID_PORT            0x11    /*Invalid port Number*/
#define MAD_DRV_FAIL_GET_PHY_ADDR        0x12    /*Failed to get phy Address*/
#define MAD_DRV_FAIL_READ_REG            0x13    /*Failed to read register*/
#define MAD_DRV_FAIL_WRITE_REG            0x14    /*Failed to write register*/
#define MAD_DRV_FAIL_GET_PAGE_ADDR        0x15    /*Failed to get page Address  */
#define MAD_DRV_FAIL_WRITE_PAGE_NUM        0x16    /*Writing Page Number failed */
#define MAD_DRV_FAIL_RESET                0x17    /*Reset bit is not cleared */
#define MAD_DRV_NO_DEVICE_FOUND            0x18    /*Cannot find Marvell Device */
#define MAD_DRV_UNKNOWN_HW_MOD            0x19    /*Unknown HW Mode */
#define MAD_DRV_ERROR_DEV_ID            0x1a    /*Error device ID */
#define MAD_DRV_ERROR_VERSION            0x1b    /*Error Version */



/* MAD error status driver level 1 */

#define MAD_DRV_FAIL_ACCESS_PAGE_REG    0x1<<8    /*Accessing Page Register failed */
#define MAD_DRV_FAIL_WRITE_PAGED_REG    0x2<<8    /*SMI Paged register Write failed */
#define MAD_DRV_NO_ABLE_READ_REG        0x3<<8    /*Not able to read Phy Register. */
#define MAD_DRV_NO_ABLE_WRITE_REG        0x4<<8    /*Not able to write Phy Register. */


/* MAD error status API level 0 */

#define MAD_API_COPPER_NOT_SUPPORT        0x1<<16    /* Copper not supported. */
#define MAD_API_SGMII_NOT_SUPPORT        0x2<<16    /*SGMII not supported. */
#define MAD_API_RGMII_NOT_SUPPORT        0x3<<16    /*RGMII not supported. */
 
#define MAD_API_FAIL_READ_PAGED_REG        0x4<<16    /*Reading from paged phy reg failed. */
#define MAD_API_FAIL_WRITE_PAGED_REG    0x5<<16    /*Writing to paged phy reg failed. */
#define MAD_API_FAIL_READ_REG            0x6<<16    /*Reading from phy reg failed. */
#define MAD_API_FAIL_WRITE_REG            0x7<<16    /*Writing to phy reg failed. */
#define MAD_API_FAIL_GET_PAGED_REG_FIELD    0x8<<16     /*Get from paged phy reg field failed. */
#define MAD_API_FAIL_SET_PAGED_REG_FIELD    0x9<<16     /*Set to paged phy reg field failed. */
#define MAD_API_FAIL_GET_REG_FIELD        0xa<<16    /*Get from phy reg field failed. */
#define MAD_API_FAIL_SET_REG_FIELD        0xb<<16    /*Set to phy reg field failed. */
#define MAD_API_FAIL_SET_CTRL_REG        0xc<<16    /* Set control register failed. */
#define MAD_API_FAIL_SW_RESET            0xd<<16    /*Soft Reset failed. */

#define MAD_API_UNKNOWN_DEV                0xe<<16    /*Unknown Device */
#define MAD_API_UNKNOWN_AUTONEG_MODE    0xf<<16 /*Unknown AutoNeg mode is set */

#define MAD_API_UNKNOWN_SPEED_MODE        0x10<<16/*Unknown speed mode */
#define MAD_API_UNKNOWN_MDI_X_MODE        0x11<<16/*Unknown MDI Crossover mode is set. */
#define MAD_API_UNKNOWN_DUPLEX_MODE        0x12<<16/*Unknown Duplex mode is set. */
#define MAD_API_UNKNOWN_MACIF_LB_MODE    0x13<<16/*Unknown MAC Interface Loopback mode is set. */
#define MAD_API_UNKNOWN_EXT_LB_MODE        0x14<<16/*Unknown External Loopback mode is set. */
#define MAD_API_UNKNOWN_IEEE_TEST_MODE    0x15<<16/*Unknown IEEE test mode is set. */
#define MAD_API_UNKNOWN_PKG_GEN_TYPE    0x16<<16/*Unknown package generator type is set. */
#define MAD_API_UNKNOWN_CRC_CHECK_TYPE    0x17<<16/*Unknown CRC Check type is set. */
#define MAD_API_UNKNOWN_CRC_ERR_COUNT_TYPE    0x18<<16 /*Unknown CRC Error Counter type is set. */
#define MAD_API_UNKNOWN_CRC_FRM_COUNT_TYPE    0x19<<16 /*Unknown CRC Frame Counter type is set. */
#define MAD_API_UNKNOWN_ADVVCT_MODE        0x1a<<16/*Unknown Advanced VCT Mode. */
#define MAD_API_UNKNOWN_VOD_PARA        0x1b<<16/*Unknown VOD parameter. */
 
#define MAD_API_CRC_FRM_COUNT_NOT_SUPPORT    0x1c<<16 /*CRC Frame Counter not supported. */
#define MAD_API_CRC_ERR_COUNT_NOT_SUPPORT    0x1d<<16 /*CRC Error Counter not supported. */
#define MAD_API_CRC_CHECK_NOT_SUPPORT    0x1e<<16/*CRC Check not supported. */
#define MAD_API_PKG_GEN_NOT_SUPPORT        0x1f<<16/*Package generator not supported. */
#define MAD_API_IEEE_TEST_NOT_SUPPORT    0x20<<16/*IEEE Test not supported. */
#define MAD_API_DOWNSHIFT_NOT_SUPPORT    0x21<<16/*DownShift not supported. */
#define MAD_API_DTEDETECT_NOT_SUPPORT    0x22<<16/*DTEDetect not supported. */
#define MAD_API_VOD_NOT_SUPPORT            0x23<<16/*VOD not supported. */
#define MAD_API_DSPVCT_NOT_SUPPORT        0x24<<16/*DSP (Ext) VCT not supported. */
#define MAD_API_ADVDSPVCT_NOT_SUPPORT    0x25<<16/*Advanced DSP VCT not supported. */
#define MAD_API_TDRVCT_NOT_SUPPORT        0x26<<16/*TDR VCT not supported. */
#define MAD_API_ADVTDRVCT_NOT_SUPPORT    0x27<<16 /*Advanced TDR VCT not supported. */
#define MAD_API_PW_DW_NOT_SUPPORT        0x28<<16 /*Set power down not supported. */
#define MAD_API_DETECT_PW_DW_NOT_SUPPORT    0x29<<16 /*Set power down not supported. */
#define MAD_API_UNIDIR_TRANS_NOT_SUPPORT    0x2a<<16 /*Unidirectional transmit not supported. */
#define MAD_API_MACSEC_NOT_SUPPORT        0x2b<<16 /*MACSEC not supported. */

#define MAD_API_MACIF_LB_NOT_SUPPORT    0x30<<16/*MAC Interface Loopback not supported. */
#define MAD_API_LINE_LB_NOT_SUPPORT        0x31<<16/*Line Loopback not supported. */
#define MAD_API_EXT_LB_NO_SET_FUNC        0x32<<16/*External Loopback no setting function. */

#define MAD_API_ERROR_SET_COPPER_FIBER    0x33<<16/*Device shoud be ether Copper or Fiber. */
#define MAD_API_ERR_DEV                    0x34<<16/*driver struture is NULL. */
#define MAD_API_ERR_SYS_CFG                0x35<<16/*sys config is NULL. */
#define MAD_API_ERR_DEV_ALREADY_EXIST    0x36<<16/*Device Driver already loaded. */
#define MAD_API_ERR_REGISTERD_FUNC        0x37<<16/*Register function Failed. */
#define MAD_API_ERROR_OVER_PORT_RANGE    0x38<<16/*The port is over range. */
#define MAD_API_ERROR_IS_NOT_GIGA_PORT    0x39<<16/*The port is not GIGA port. */

#define MAD_API_MACIF_LB_MOD_NOT_SUPPORT    0x40<<16/*MAC Interface Loopback mode not supported. */
#define MAD_API_UNKNOWN_RESET_MODE         0x41<<16/*Unknown Reset mode  */
#define MAD_API_UNKNOWN_ALTVCT_MODE        0x42<<16/*Unknown Alternate VCT Mode. */
#define MAD_API_UNKNOWN_HW_MODE            0x43<<16/*Unknown Alternate VCT Mode. */
#define MAD_API_ALTTDRVCT_NOT_SUPPORT    0x44<<16 /*Altnate TDR VCT not supported. */
#define MAD_API_INTSUM_NOT_SUPPORT        0x45<<16 /*Interrupt summary not supported. */
#define MAD_API_SYNCRO_FIFO_NOT_SUPPORT        0x46<<16 /*Synchronize FIFO not supported. */
#define MAD_API_NEW_FEATURE_NOT_SUPPORT        0x47<<16 /*1340 new features not supported. */
#define MAD_API_PTP_NOT_SUPPORT            0x48<<16 /*1340S PTP features not supported. */
#define MAD_API_PTP_BAD_PARAM            0x49<<16 /*Set PTP bad parameters. */

#define MAD_API_ADVDSPVCT_ATTEN_NOT_SUPPORT        0x50<<16 /*DSP_VCT Attenu vs Freq not supported. */
#define MAD_API_ADVDSPVCT_CABLELEN_NOT_SUPPORT  0x51<<16 /*DSP_VCT Cable length not supported.*/
#define MAD_API_ADVDSPVCT_XTALK_NOT_SUPPORT        0x52<<16 /*DSP_VCT Xtalk vs Freq not supported.*/
#define MAD_API_ADVDSPVCT_CABLETYPE_NOT_SUPPORT    0x53<<16 /*DSP_VCT Etherbet cable type not supported.*/
#define MAD_API_ADVDSPVCT_SPLITPAIR_NOT_SUPPORT    0x54<<16 /*DSP_VCT Split pair not supported.*/
#define MAD_API_ADVDSPVCT_RLOSS_NOT_SUPPORT        0x55<<16 /*DSP_VCT Return loss not supported.*/

#define MAD_MSEC_FAIL_PORT_SET_MAC_CTRL            0x60<<16/*MacSec Set Port Mac Ctrl error.*/
#define MAD_MSEC_FAIL_PORT_GET_MAC_CTRL            0x61<<16/*MacSec Get Port Mac Ctrl error.*/
#define MAD_MSEC_FAIL_PORT_SET_MAC_SPEED        0x62<<16/*MacSec Set Port Speed error.*/
#define MAD_MSEC_FAIL_PORT_GET_MAC_SPEED        0x63<<16/*MacSec Get Port Speed error.*/
#define MAD_MSEC_FAIL_PORT_DIS_ELU_ENTRY        0x64<<16/*MacSec disable Elu entry error.*/
#define MAD_MSEC_FAIL_PORT_SET_ELU_ENTRY        0x65<<16/*MacSec set Elu entry error.*/
#define MAD_MSEC_FAIL_PORT_SET_ECT_ENTRY        0x66<<16/*MacSec set ECT entry error.*/
#define MAD_MSEC_FAIL_PORT_SET_EKEY_ENTRY        0x67<<16/*MacSec set EKey entry error.*/
#define MAD_MSEC_FAIL_PORT_SET_EHKEY_ENTRY        0x68<<16/*MacSec set EHKey entry error.*/
#define MAD_MSEC_FAIL_PORT_GET_ELU_ENTRY        0x69<<16/*MacSec get Elu entry error.*/
#define MAD_MSEC_FAIL_PORT_GET_ECT_ENTRY        0x6a<<16/*MacSec get ECT entry error.*/
#define MAD_MSEC_FAIL_PORT_GET_EKEY_ENTRY        0x6b<<16/*MacSec get EKey entry error.*/
#define MAD_MSEC_FAIL_PORT_GET_EHKEY_ENTRY        0x6c<<16/*MacSec get EHKey entry error.*/

#define MAD_MSEC_FAIL_PORT_DIS_ILU_ENTRY        0x70<<16/*MacSec disable Ilu entry error.*/
#define MAD_MSEC_FAIL_PORT_SET_ILU_ENTRY        0x71<<16/*MacSec set Ilu entry error.*/
#define MAD_MSEC_FAIL_PORT_SET_ICT_ENTRY        0x72<<16/*MacSec set ICT entry error.*/
#define MAD_MSEC_FAIL_PORT_SET_IKEY_ENTRY        0x73<<16/*MacSec set IKey entry error.*/
#define MAD_MSEC_FAIL_PORT_SET_IHKEY_ENTRY        0x74<<16/*MacSec set IHKey entry error.*/
#define MAD_MSEC_FAIL_PORT_GET_ILU_ENTRY        0x75<<16/*MacSec get Ilu entry error.*/
#define MAD_MSEC_FAIL_PORT_GET_ICT_ENTRY        0x76<<16/*MacSec get ICT entry error.*/
#define MAD_MSEC_FAIL_PORT_GET_IKEY_ENTRY        0x77<<16/*MacSec get IKey entry error.*/
#define MAD_MSEC_FAIL_PORT_GET_IHKEY_ENTRY        0x78<<16/*MacSec get IHKey entry error.*/
#define MAD_MSEC_FAIL_PORT_SET_NEXPN_ENTRY        0x79<<16/*MacSec set NexPn entry error.*/
#define MAD_MSEC_FAIL_PORT_GET_IGR_ENTRY        0x7a<<16/*MacSec get IGR entry error.*/

/* MAD error status API level 1 */

#define MAD_API_FAIL_GET_SPEED_ST        0x1<<24    /*mdCopper Get Speed Status failed. */
#define MAD_API_FAIL_GET_DUPLEX_ST        0x2<<24    /*md Duplex Status failed. */
#define MAD_API_FAIL_GET_LINK_ST        0x3<<24    /*mdGetLinkStatus failed. */
#define MAD_API_FAIL_GET_AUTONEG        0x4<<24    /*mdSGMIIGetAutoNeg return Failed */
#define MAD_API_FAIL_SET_AUTONEG        0x5<<24    /*mdSGMIISetAutoNeg return Failed */
#define MAD_API_FAIL_GET_INT_ST            0x6<<24    /*mdIntGetStatus failed. */
#define MAD_API_FAIL_SET_INT_ST            0x7<<24    /*mdIntSetEnable failed. */
#define MAD_API_FAIL_VCT_SM_SET            0x8<<24    /*run VCT State machine set again failed. */
#define MAD_API_FAIL_VCT_SM_CHECK        0x9<<24    /*run VCT State machine check again failed. */
#define MAD_API_FAIL_VCT_SM_GET            0xa<<24    /*run VCT State machine get again failed. */
#define MAD_API_FAIL_VCT_CABLELEN        0xb<<24    /*run VCT Cable length test failed. */
#define MAD_API_FAIL_VCT_SM_INSTANCE    0xc<<24    /*Can not find VCT State machine instance . */
#define MAD_API_FAIL_VCT_SM_IMPLEM        0xd<<24    /*Can not implement VCT State machine instance . */
#define MAD_API_FAIL_VCT_INIT            0xe<<24    /*Enabling work around failed. */
#define MAD_API_FAIL_VCT_CLOSE            0xf<<24    /*Disabling work around failed. */
#define MAD_API_FAIL_VCT_TEST            0x10<<24/*VCT test failed. No result is valid. */

#define MAD_API_FAIL_EXTVCT_DIST        0x11<<24/*getDSPDistance failed. */
#define MAD_API_FAIL_ADVVCT_RUN            0x12<<24/*Running advanced VCT failed. */
#define MAD_API_FAIL_ADVVCT_SM_SET        0x13<<24/*run ADV VCT State machine set again failed. */
#define MAD_API_FAIL_ADVVCT_SM_CHECK    0x14<<24/*run ADV VCT State machine check again failed. */
#define MAD_API_FAIL_ADVVCT_SM_GET        0x15<<24/*run ADV VCT State machine get again failed. */
#define MAD_API_FAIL_ALTVCT_RUN            0x16<<24/*Running alternate VCT failed. */

#define MAD_API_FAIL_INIT_DRV_CFG        0x1a<<24/*Initialize driverConfig Failed. */
#define MAD_API_FAIL_PRE_INIT_DEV        0x1b<<24/*Pre-Initialize device Failed. */

#define MAD_API_FAIL_SEM_CREATE            0x1c<<24/*semCreate Failed. */
#define MAD_API_FAIL_SEM_DELETE            0x1d<<24/*semDelete Failed. */

#define MAD_API_FAIL_STATE_MACHINE        0x1e<<24/*state machine Failed. */

#define MAD_API_FAIL_ADVVCTDSP_ATTEN    0x20<<24/*DSP_VCT Attenuation Vs Freq error.*/
#define MAD_API_FAIL_ADVVCTDSP_CABLEN    0x21<<24/*DSP_VCT Cable length error.*/
#define MAD_API_FAIL_ADVVCTDSP_XTALK     0x22<<24/*DSP_VCT Xtalk vs Freq error.*/
#define MAD_API_FAIL_ADVVCTDSP_CABLETP   0x23<<24/*DSP_VCT Ethernet cable type error.*/
#define MAD_API_FAIL_ADVVCTDSP_SPLITPA   0x24<<24/*DSP_VCT Split pair error.*/
#define MAD_API_FAIL_ADVVCTDSP_RLOSS     0x25<<24/*DSP_VCT Return loss error.*/

#define MAD_MSEC_FAIL_INIT               0x30<<24/*MacSec Initial error.*/
#define MAD_MSEC_FAIL_CMN_CFG            0x31<<21/*MacSec Initial common config error.*/
#define MAD_MSEC_FAIL_EGR_CFG            0x32<<21/*MacSec Initial Ergress config error.*/
#define MAD_MSEC_FAIL_OTHER_BLK_INIT     0x33<<21/*MacSec Initial other block error.*/
#define MAD_MSEC_FAIL_PORT_MAC_INIT      0x34<<21/*MacSec Initial Port Mac error.*/
#define MAD_MSEC_FAIL_STATUS_INIT        0x35<<21/*MacSec Initial Status error.*/
#define MAD_MSEC_FAIL_GET_EG_ENTRY       0x36<<21/*MacSec Get Egress Entry error.*/
#define MAD_MSEC_FAIL_IGR_DEF_ACT        0x37<<21/*MacSec Set IGR DEF ACT error.*/

/* status / error codes */
typedef unsigned int MAD_STATUS;


#endif /* MAD_ERROR_DEFS_H */

