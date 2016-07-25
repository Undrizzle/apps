#include <madCopyright.h>

/********************************************************************************
 * madDrvPhyRegs.h
 *
 * DESCRIPTION:
 *       definitions of the register map of Marvell Phy Device
 *
 * DEPENDENCIES:
 *
 * FILE REVISION NUMBER:
 *
 *******************************************************************************/
#ifndef __madDrvPhyRegsh
#define __madDrvPhyRegsh

/* Marvell Phy Registers */
#define MAD_PAGE0    0
#define MAD_PAGE1    1
#define MAD_PAGE2    2
#define MAD_PAGE3    3
#define MAD_PAGE4    4
#define MAD_PAGE5    5
#define MAD_PAGE6    6
#define MAD_PAGE7    7
#define MAD_PAGE8    8
#define MAD_PAGE9    9
#define MAD_PAGE12    12
#define MAD_PAGE14    14
#define MAD_PAGE16    16

/* Any Page or Page 0 for 1340*/
#define MAD_REG_CONTROL         0
#define MAD_REG_STATUS          1
#define MAD_REG_PHY_ID_1        2
#define MAD_REG_PHY_ID_2        3
#define MAD_REG_AUTO_AD         4
#define MAD_REG_PARTNER_AD      5
#define MAD_REG_AUTO_EXP        6
#define MAD_REG_NEXT_PAGE       7
#define MAD_REG_PARTNER_NEXT_PAGE   8
#define MAD_REG_1000_CONTROL    9
#define MAD_REG_1000_STATUS     10
#define MAD_REG_EXT_STATUS      15
#define MAD_REG_EXT_PHY_SPEC_CONTROL      20
#define MAD_REG_PAGE_ADDR       22
#define MAD_REG_PAGE_ADDR_2     29
#define MAD_REG_PAGE_ACCESS     30  /* Mult-functions access register, which page is on Reg 29 */



/* Page 0 */
#define MAD_REG_COPPER_CONTROL_1            16
#define MAD_REG_COPPER_STATUS_1                17
#define MAD_REG_COPPER_INT_ENABLE            18
#define MAD_REG_COPPER_STATUS_2                19
#define MAD_REG_COPPER_CONTROL_2            20 /* befor 1340 call control reg 3 */
#define MAD_REG_COPPER_RX_ERROR_COUNTER        21
#define MAD_REG_GLOBAL_INT_STATUS            23
#define MAD_REG_COPPER_CONTROL_3            26  /* befor 1340 call control reg 2 */

/* Page 1 */
#define MAD_REG_FIBER_CONTROL                0
#define MAD_REG_FIBER_STATUS                1
#define MAD_REG_FIBER_AUTO_AD                4
#define MAD_REG_FIBER_PARTNER_AD            5
#define MAD_REG_FIBER_AUTO_EXP                6
#define MAD_REG_FIBER_NEXT_PAGE                7
#define MAD_REG_FIBER_PARTNER_NEXT_PAGE        8
#define MAD_REG_FIBER_EXT_STATUS            15

#define MAD_REG_FIBER_CONTROL_1                16
#define MAD_REG_FIBER_STATUS_1                17
#define MAD_REG_FIBER_INT_ENABLE            18
#define MAD_REG_FIBER_STATUS_2                19
#define MAD_REG_FIBER_RX_ERROR_COUNTER        21

#define MAD_REG_FIBER_PRBS_CTRL                23
#define MAD_REG_FIBER_PRBS_ERR_COUNT_LSB    24
#define MAD_REG_FIBER_PRBS_ERR_COUNT_MSB    25
#define MAD_REG_FIBER_SPEC_CTRL_2            26


/* Page 2 */
#define MAD_REG_MAC_CONTROL_1                16
#define MAD_REG_MAC_STATUS_1                17
#define MAD_REG_MAC_INT_ENABLE                18
#define MAD_REG_MAC_STATUS_2                19
#define MAD_REG_NAC_RX_ER_BYTE_CAPT            20
#define MAD_REG_MAC_CONTROL                    21
#define MAD_REG_MAC_CONTROL_2                26
#define MAD_REG_MAC_CO_SERDES_RX_ER_BYTE_CTRL_2       20
#define MAD_REG_MAC_CONTROL_2_1340            21

/* Page 3 */
#define MAD_REG_LED_FUNC_CTRL         16
#define MAD_REG_LED_POLAR_CTRL        17
#define MAD_REG_LED_TIMER_CTRL        18
#define MAD_REG_LED_FUNC_POLAR_CTRL   19


/* Page 4 */
#define MAD_REG_QSGMII_CONTROL         0
#define MAD_REG_QSGMII_STATUS          1
#define MAD_REG_QSGMII_AUTO_AD         4
#define MAD_REG_QSGMII_PARTNER_AD      5
#define MAD_REG_QSGMII_AUTO_EXP        6
#define MAD_REG_QSGMII_CONTROL_1            16
#define MAD_REG_QSGMII_STATUS_1                17
#define MAD_REG_QSGMII_INT_ENABLE            18
#define MAD_REG_QSGMII_STATUS_2                19
#define MAD_REG_QSGMII_RX_ER_BYTE_CAPT        20
#define MAD_REG_QSGMII_RX_ER_COUNTER        21
#define MAD_REG_QSGMII_PRBS_CTRL            23
#define MAD_REG_QSGMII_PRBS_ERR_COUNT_LSB   24
#define MAD_REG_QSGMII_PRBS_ERR_COUNT_MSB   25
#define MAD_REG_QSGMII_GLOBAL_CTRL_1        26
#define MAD_REG_QSGMII_GLOBAL_CTRL_2        27

/* Page 5 */
#define MAD_REG_MDI0_VCT_STATUS     16
#define MAD_REG_MDI1_VCT_STATUS     17
#define MAD_REG_MDI2_VCT_STATUS     18
#define MAD_REG_MDI3_VCT_STATUS     19
#define MAD_REG_PAIR_SKEW_STATUS    20
#define MAD_REG_PAIR_SWAP_STATUS    21
#define MAD_REG_ADV_VCT_CONTROL_5    23
#define MAD_REG_ADV_VCT_SMPL_DIST    24
#define MAD_REG_ADV_VCT_X_PAIR_PTHR    25
#define MAD_REG_ADV_VCT_SMPAIR_PTHR_01    26
#define MAD_REG_ADV_VCT_SMPAIR_PTHR_23    27
#define MAD_REG_ADV_VCT_SMPAIR_PTHR_4_P    28

/* Page 6 */
#define MAD_REG_PACKET_GENERATION   16
#define MAD_REG_CRC_COUNTERS        17
#define MAD_REG_CHECK_CONTROL       18
#define MAD_REG_GENERAL_CONTROL        20
#define MAD_REG_LATE_COLIS_CNT_12   23
#define MAD_REG_LATE_COLIS_CNT_34   24
#define MAD_REG_LATE_COLIS_WIN_ADJ  25
#define MAD_REG_MISC_TEST           26

/* Page 7 */
#define MAD_REG_PHY_CBL_DIAG_0_LEN   16
#define MAD_REG_PHY_CBL_DIAG_1_LEN   17
#define MAD_REG_PHY_CBL_DIAG_2_LEN   18
#define MAD_REG_PHY_CBL_DIAG_3_LEN   19
#define MAD_REG_PHY_CBL_DIAG_RESULT  20
#define MAD_REG_PHY_CBL_DIAG_CTRL    21
#define MAD_REG_ADV_VCT_XPAIR_NTHR        25
#define MAD_REG_ADV_VCT_SMPAIR_NTHR_01    26
#define MAD_REG_ADV_VCT_SMPAIR_NTHR_23    27
#define MAD_REG_ADV_VCT_SMPAIR_NTHR_4_P    28

/* Page 8, PTP */
#define MAD_REG_PTP_PORT_CONFIG_0        0
#define MAD_REG_PTP_PORT_CONFIG_1        1
#define MAD_REG_PTP_PORT_CONFIG_2        2
#define MAD_REG_PTP_ARR0_PORT_STATUS    8
#define MAD_REG_PTP_TIME_ARR0_01        9
#define MAD_REG_PTP_TIME_ARR0_23        10
#define MAD_REG_PTP_TIME_ARR0_SEQID        11
#define MAD_REG_PTP_ARR1_PORT_STATUS    12
#define MAD_REG_PTP_TIME_ARR1_01        13
#define MAD_REG_PTP_TIME_ARR1_23        14
#define MAD_REG_PTP_TIME_ARR1_SEQID        15

/* Page 8, Advanced VCT */
#define MAD_REG_ADV_VCT_CONTROL_8        20

/* Page 9, PTP */
#define MAD_REG_PTP_DEP_PORT_STATUS        0
#define MAD_REG_PTP_TIME_DEP_01            1
#define MAD_REG_PTP_TIME_DEP_23            2
#define MAD_REG_PTP_TIME_DEP_SEQID        3
#define MAD_REG_PTP_PORT_STATUS            5


/* Page 11, LinkCript  */
#define MAD_REG_LINKCRYPT_READ_ADDR        0
#define MAD_REG_LINKCRYPT_WRITE_ADDR    1
#define MAD_REG_LINKCRYPT_DATA_LO        2
#define MAD_REG_LINKCRYPT_DATA_HI        3

/* Page 12, TAI Global Config  */
#define MAD_REG_TAI_CONFIG_0            0
#define MAD_REG_TAI_CONFIG_1            1
#define MAD_REG_TAI_CONFIG_2            2
#define MAD_REG_TAI_CONFIG_3            3
#define MAD_REG_TAI_CONFIG_4            4
#define MAD_REG_TAI_CONFIG_5            5
#define MAD_REG_TAI_CONFIG_8            8
#define MAD_REG_TAI_CONFIG_9            9
#define MAD_REG_TAI_CONFIG_10            10
#define MAD_REG_TAI_CONFIG_11            11
#define MAD_REG_TAI_CONFIG_12            12
#define MAD_REG_TAI_CONFIG_13            13
#define MAD_REG_TAI_CONFIG_14            14
#define MAD_REG_TAI_CONFIG_15            15

/* Bit Definition for MAD_REG_CONTROL */
#define MAD_REG_CTRL_RESET          0x8000
#define MAD_REG_CTRL_LOOPBACK       0x4000
#define MAD_REG_CTRL_SPEED          0x2000
#define MAD_REG_CTRL_AUTONEGO       0x1000
#define MAD_REG_CTRL_POWER          0x800
#define MAD_REG_CTRL_ISOLATE        0x400
#define MAD_REG_CTRL_RESTART_AUTONEGO   0x200
#define MAD_REG_CTRL_DUPLEX         0x100
#define MAD_REG_CTRL_SPEED_MSB      0x40

#define MAD_REG_CTRL_POWER_BIT              11
#define MAD_REG_CTRL_RESTART_AUTONEGO_BIT   9


/* Bit Definition for MAD_REG_AUTO_AD */
#define MAD_REG_AUTO_AD_NEXTPAGE        0x8000
#define MAD_REG_AUTO_AD_REMOTEFAULT     0x4000
#define MAD_REG_AUTO_AD_ASYM_PAUSE      0x800
#define MAD_REG_AUTO_AD_PAUSE           0x400
#define MAD_REG_AUTO_AD_100_FULL        0x100
#define MAD_REG_AUTO_AD_100_HALF        0x80
#define MAD_REG_AUTO_AD_10_FULL         0x40
#define MAD_REG_AUTO_AD_10_HALF         0x20


/* Bit Definition for MAD_REG_1000_CONTROL */
#define MAD_REG_MS_MANUAL_CONFIG    0x1000
#define MAD_REG_MS_CONFIG_VALUE     0x800
#define MAD_REG_MS_PORT_TYPE        0x400
#define MAD_REG_1000T_FULL          0x200
#define MAD_REG_1000T_HALF          0x100


/* Bit definition for MAD_REG_COPPER_INT_ENABLE */
#define MAD_REG_INT_AUTO_NEGO_ERROR     0x8000
#define MAD_REG_INT_SPEED_CHANGED       0x4000
#define MAD_REG_INT_DUPLEX_CHANGED      0x2000
#define MAD_REG_INT_PAGE_RECEIVED       0x1000
#define MAD_REG_INT_AUTO_NEG_COMPLETED  0x800
#define MAD_REG_INT_LINK_STATUS_CHANGED 0x400
#define MAD_REG_INT_SYMBOL_ERROR        0x200
#define MAD_REG_INT_FALSE_CARRIER       0x100
#define MAD_REG_INT_CROSSOVER_CHANGED   0x40
#define MAD_REG_INT_DOWNSHIFT           0x20
#define MAD_REG_INT_ENERGY_DETECT       0x10
#define MAD_REG_INT_DTE_DETECT_CHANGED  0x4
#define MAD_REG_INT_POLARITY_CHANGED    0x2
#define MAD_REG_INT_JABBER              0x1


/* Bit definition for MAD_REG_MAC_INT_ENABLE */
#define MAD_REG_MAC_INT_FIFO_ERROR      0x80


#endif /* __madDrvPhyRegsh */
