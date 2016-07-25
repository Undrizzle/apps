#include <madCopyright.h>
/********************************************************************************
* displayMADStatus.c
*
* DESCRIPTION:
*       API test functions
*
* DEPENDENCIES:   Platform.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "madSample.h"


void displayStatus_drv_0(MAD_STATUS status)
{
	switch(status&MAD_ERR_ST_DRV_0_MASK)
	{
		case MAD_OK	:
			if (status) 
				break;
			MSG_PRINT(" Operation succeeded \n");    
			break;
		case MAD_FAIL	:			
			MSG_PRINT(" Operation failed    \n");    
			break;

		case MAD_PENDING	:			
			MSG_PRINT(" MAD Pending  \n");    
			break;
		case MAD_VCT_AGAIN	:			
			MSG_PRINT(" MAD VCT need do again \n");    
			break;

		case MAD_API_CABLE_LEN_NOT_READY :			
			MSG_PRINT("Ready bit of Cable length resiter is not set. \n");    
			break;
		case MAD_API_LINK_DOWN			:			
			MSG_PRINT("Link should be established \n");    
			break;
		case MAD_API_NOT_FULL_DUPLEX	:			
			MSG_PRINT("Only Full Duplex mode is supported. \n");    
			break;
		case MAD_API_NOT_HALF_DUPLEX	:			
			MSG_PRINT("Only Half Duplex mode is supported. \n");    
			break;
		case    MAD_TIMEOUT         :   
			MSG_PRINT("Time Out.\n");
			break;

/* MAD error status driver level 0 */
		case MAD_DRV_INVALID_PORT		:			
			MSG_PRINT("Invalid port Number\n");    
			break;
		case MAD_DRV_FAIL_GET_PHY_ADDR		:			
			MSG_PRINT("Failed to get phy Address\n");    
			break;
		case MAD_DRV_FAIL_READ_REG		:			
			MSG_PRINT("Failed to read register\n");    
			break;
		case MAD_DRV_FAIL_WRITE_REG		:			
			MSG_PRINT("Failed to write register\n");    
			break;
		case MAD_DRV_FAIL_GET_PAGE_ADDR		:			
			MSG_PRINT("Failed to get page Address  \n");    
			break;
		case MAD_DRV_FAIL_WRITE_PAGE_NUM	:			
			MSG_PRINT("Writing Page Number failed \n");    
			break;
		case MAD_DRV_FAIL_RESET			:			
			MSG_PRINT("Reset bit is not cleared \n");    
			break;
		case MAD_DRV_NO_DEVICE_FOUND		:			
			MSG_PRINT("Cannot find Marvell Device \n");    
			break;
		case MAD_DRV_UNKNOWN_HW_MOD		:			
			MSG_PRINT("Unknown HW Mode \n");    
			break;
		case MAD_DRV_ERROR_DEV_ID		:			
			MSG_PRINT("Error device ID \n");    
			break;
		case MAD_DRV_ERROR_VERSION		:			
			MSG_PRINT("Error Version \n");    
			break;
		default:			
			MSG_PRINT("Unknown Error Driver layer 0L\n");    
			break;
	}

}

void displayStatus_drv_1(MAD_STATUS status)
{

/* MAD error status driver level 1 */
	switch(status&MAD_ERR_ST_DRV_1_MASK)
	{
		case 0	:
			break;
		case MAD_DRV_FAIL_ACCESS_PAGE_REG	:			
			MSG_PRINT("Accessing Page Register failed \n");    
			break;
		case MAD_DRV_FAIL_WRITE_PAGED_REG	:			
			MSG_PRINT("SMI Paged register Write failed \n");    
			break;
		case MAD_DRV_NO_ABLE_READ_REG		:			
			MSG_PRINT("Not able to read Phy Register. \n");    
			break;
		case MAD_DRV_NO_ABLE_WRITE_REG		:			
			MSG_PRINT("Not able to write Phy Register. \n");    
			break;
		default:			
			MSG_PRINT("Unknown Error Driver layer 1L\n");    
			break;
	}


}

void displayStatus_api_0(MAD_STATUS status)
{
/* MAD error status API level 0 */
	switch(status&MAD_ERR_ST_API_0_MASK)
	{
		case 0	:
			break;
		case MAD_API_COPPER_NOT_SUPPORT		:			
			MSG_PRINT(" Copper not supported. \n");    
			break;
		case MAD_API_SGMII_NOT_SUPPORT		:			
			MSG_PRINT("SGMII not supported. \n");    
			break;
		case MAD_API_RGMII_NOT_SUPPORT		:			
			MSG_PRINT("RGMII not supported. \n");    
			break;
 
		case MAD_API_FAIL_READ_PAGED_REG	:			
			MSG_PRINT("Reading from paged phy reg failed. \n");    
			break;
		case MAD_API_FAIL_WRITE_PAGED_REG	:			
			MSG_PRINT("Writing to paged phy reg failed. \n");    
			break;
		case MAD_API_FAIL_READ_REG			:			
			MSG_PRINT("Reading from phy reg failed. \n");    
			break;
		case MAD_API_FAIL_WRITE_REG			:			
			MSG_PRINT("Writing to phy reg failed. \n");    
			break;
		case MAD_API_FAIL_GET_PAGED_REG_FIELD	:			
			MSG_PRINT("Get from paged phy reg field failed. \n");    
			break;
		case MAD_API_FAIL_SET_PAGED_REG_FIELD	:			
			MSG_PRINT("Set to paged phy reg field failed. \n");    
			break;
		case MAD_API_FAIL_GET_REG_FIELD		:			
			MSG_PRINT("Get from phy reg field failed. \n");    
			break;
		case MAD_API_FAIL_SET_REG_FIELD		:			
			MSG_PRINT("Set to phy reg field failed. \n");    
			break;
		case MAD_API_FAIL_SET_CTRL_REG		:			
			MSG_PRINT(" Set control register failed. \n");    
			break;
		case MAD_API_FAIL_SW_RESET			:			
			MSG_PRINT("Soft Reset failed. \n");    
			break;

		case MAD_API_UNKNOWN_DEV			:			
			MSG_PRINT("Unknown Device \n");    
			break;
		case MAD_API_UNKNOWN_AUTONEG_MODE	:			
			MSG_PRINT("Unknown AutoNeg mode is set \n");    
			break;

		case MAD_API_UNKNOWN_SPEED_MODE		:			
			MSG_PRINT("Unknown speed mode \n");    
			break;
		case MAD_API_UNKNOWN_MDI_X_MODE		:			
			MSG_PRINT("Unknown MDI_X mode is set. \n");    
			break;
		case MAD_API_UNKNOWN_DUPLEX_MODE		:			
			MSG_PRINT("Unknown Duplex mode is set. \n");    
			break;
		case MAD_API_UNKNOWN_RESET_MODE		:			
			MSG_PRINT("Unknown Reset mode is set. \n");    
			break;
		case MAD_API_UNKNOWN_HW_MODE		:			
			MSG_PRINT("Unknown Hardware mode is set. \n");    
			break;
		case MAD_API_UNKNOWN_MACIF_LB_MODE	:			
			MSG_PRINT("Unknown MAC Interface Loopback mode is set. \n");    
			break;
		case MAD_API_UNKNOWN_EXT_LB_MODE	:			
			MSG_PRINT("Unknown External Loopback mode is set. \n");    
			break;
		case MAD_API_UNKNOWN_IEEE_TEST_MODE	:			
			MSG_PRINT("Unknown IEEE test mode is set. \n");    
			break;
		case MAD_API_UNKNOWN_PKG_GEN_TYPE	:			
			MSG_PRINT("Unknown package generator type is set. \n");    
			break;
		case MAD_API_UNKNOWN_CRC_CHECK_TYPE	:			
			MSG_PRINT("Unknown CRC Check type is set. \n");    
			break;
		case MAD_API_UNKNOWN_CRC_ERR_COUNT_TYPE	 :			
			MSG_PRINT("Unknown CRC Error Counter type is set. \n");    
			break;
		case MAD_API_UNKNOWN_CRC_FRM_COUNT_TYPE	 :			
			MSG_PRINT("Unknown CRC Frame Counter type is set. \n");    
			break;
		case MAD_API_UNKNOWN_ADVVCT_MODE		:			
			MSG_PRINT("Unknown Advanced VCT Mode, do not support. \n");    
			break;
		case MAD_API_UNKNOWN_ALTVCT_MODE		:			
			MSG_PRINT("Unknown Alternate VCT Mode. \n");    
			break;
		case MAD_API_UNKNOWN_VOD_PARA		:			
			MSG_PRINT("Unknown VOD parameter. \n");    
			break;
 
		case MAD_API_CRC_FRM_COUNT_NOT_SUPPORT	:			
			MSG_PRINT("CRC Frame Counter not supported. \n");    
			break;
		case MAD_API_CRC_ERR_COUNT_NOT_SUPPORT	:			
			MSG_PRINT("CRC Error Counter not supported. \n");    
			break;
		case MAD_API_CRC_CHECK_NOT_SUPPORT	:			
			MSG_PRINT("CRC Check not supported. \n");    
			break;
		case MAD_API_PKG_GEN_NOT_SUPPORT	:			
			MSG_PRINT("Package generator not supported. \n");    
			break;
		case MAD_API_IEEE_TEST_NOT_SUPPORT	:			
			MSG_PRINT("IEEE Test not supported. \n");    
			break;
		case MAD_API_DOWNSHIFT_NOT_SUPPORT	:			
			MSG_PRINT("DownShift not supported. \n");    
			break;
		case MAD_API_DTEDETECT_NOT_SUPPORT	:			
			MSG_PRINT("DTEDetect not supported. \n");    
			break;
		case MAD_API_VOD_NOT_SUPPORT		:			
			MSG_PRINT("VOD not supported. \n");    
			break;
		case MAD_API_DSPVCT_NOT_SUPPORT		:			
			MSG_PRINT("DSP (Ext) VCT not supported. \n");    
			break;
		case MAD_API_ADVDSPVCT_NOT_SUPPORT	:			
			MSG_PRINT("Advanced DSP VCT not supported. \n");    
			break;
		case MAD_API_TDRVCT_NOT_SUPPORT		:			
			MSG_PRINT("TDR VCT not supported. \n");    
			break;
		case MAD_API_ADVTDRVCT_NOT_SUPPORT	:			
			MSG_PRINT("Advanced TDR VCT not supported. \n");    
			break;
		case MAD_API_PW_DW_NOT_SUPPORT	:			
			MSG_PRINT("Set Power Down not supported. \n");    
			break;
		case MAD_API_DETECT_PW_DW_NOT_SUPPORT	:			
			MSG_PRINT("Detect Power Down not supported. \n");    
			break;
		case MAD_API_UNIDIR_TRANS_NOT_SUPPORT	:			
			MSG_PRINT("Unidirectional transmit not supported. \n");    
			break;
		case MAD_API_MACSEC_NOT_SUPPORT	:			
			MSG_PRINT("Macsec not supported. \n");    
			break;
		case MAD_API_ALTTDRVCT_NOT_SUPPORT	:			
			MSG_PRINT("Alternate TDR VCT not supported. \n");    
			break;
		case MAD_API_INTSUM_NOT_SUPPORT	:			
			MSG_PRINT("Interrupt summary not supported. \n");    
			break;
		case MAD_API_SYNCRO_FIFO_NOT_SUPPORT	:			
			MSG_PRINT("Synchronize FIFO not supported. \n");    
			break;

		case MAD_API_NEW_FEATURE_NOT_SUPPORT	:			
			MSG_PRINT("88E1340 new features not supported. \n");    
			break;

		case MAD_API_PTP_NOT_SUPPORT	:			
			MSG_PRINT("PTP features not supported. \n");    
			break;

		case MAD_API_PTP_BAD_PARAM	:			
			MSG_PRINT("Set PTP bad parameters. \n");    
			break;

		case MAD_API_MACIF_LB_NOT_SUPPORT	:			
			MSG_PRINT("MAC Interface Loopback not supported. \n");    
			break;
		case MAD_API_LINE_LB_NOT_SUPPORT	:			
			MSG_PRINT("Line Loopback not supported. \n");    
			break;
		case MAD_API_EXT_LB_NO_SET_FUNC		:			
			MSG_PRINT("External Loopback no setting function. \n");    
			break;
		case MAD_API_MACIF_LB_MOD_NOT_SUPPORT	:			
			MSG_PRINT("MAC Interface Loopback mode not supported. \n");    
			break;

		case MAD_API_ERROR_SET_COPPER_FIBER	:			
			MSG_PRINT("Device shoud be ether Copper or Fiber. \n");    
			break;
		case MAD_API_ERR_DEV				:			
			MSG_PRINT("driver struture is NULL. \n");    
			break;
		case MAD_API_ERR_SYS_CFG			:			
			MSG_PRINT("sys config is NULL. \n");    
			break;
		case MAD_API_ERR_DEV_ALREADY_EXIST	:			
			MSG_PRINT("Device Driver already loaded. \n");    
			break;
		case MAD_API_ERR_REGISTERD_FUNC		:			
			MSG_PRINT("Register function Failed. \n");    
			break;
		case MAD_API_ERROR_OVER_PORT_RANGE		:			
			MSG_PRINT("The port is over range.\n");    
			break;
		case MAD_API_ERROR_IS_NOT_GIGA_PORT		:			
			MSG_PRINT("The port is not a Giga port.\n");    
			break;

		case MAD_API_ADVDSPVCT_ATTEN_NOT_SUPPORT :		
			MSG_PRINT("DSP_VCT Attenu vs Freq not supported.\n");    
			break;

		case MAD_API_ADVDSPVCT_CABLELEN_NOT_SUPPORT  :
			MSG_PRINT("DSP_VCT Cable length not supported.\n");    
			break;

		case MAD_API_ADVDSPVCT_XTALK_NOT_SUPPORT		:
			MSG_PRINT("DSP_VCT Xtalk vs Freq not supported.\n");    
			break;

		case MAD_API_ADVDSPVCT_CABLETYPE_NOT_SUPPORT	:
			MSG_PRINT("DSP_VCT Etherbet cable type not supported.\n");    
			break;

		case MAD_API_ADVDSPVCT_SPLITPAIR_NOT_SUPPORT	:
			MSG_PRINT("DSP_VCT Split pair not supported.\n");    
			break;

		case MAD_API_ADVDSPVCT_RLOSS_NOT_SUPPORT		:
			MSG_PRINT("DSP_VCT Return loss not supported.\n");    
			break;

		case MAD_MSEC_FAIL_PORT_SET_MAC_CTRL	:
			MSG_PRINT("MacSec Set Port Mac Ctrl error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_GET_MAC_CTRL	:		
			MSG_PRINT("MacSec Get Port Mac Ctrl error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_SET_MAC_SPEED	:	
			MSG_PRINT("MacSec Set Port Speed error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_GET_MAC_SPEED	:	
			MSG_PRINT("MacSec Get Port Speed error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_DIS_ELU_ENTRY	:	
			MSG_PRINT("MacSec disable Elu entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_SET_ELU_ENTRY	:	
			MSG_PRINT("MacSec set Elu entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_SET_ECT_ENTRY	:	
			MSG_PRINT("MacSec set ECT entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_SET_EKEY_ENTRY	:	
			MSG_PRINT("MacSec set EKey entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_SET_EHKEY_ENTRY	:	
			MSG_PRINT("MacSec set EHKey entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_GET_ELU_ENTRY	:	
			MSG_PRINT("MacSec get Elu entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_GET_ECT_ENTRY	:	
			MSG_PRINT("MacSec get ECT entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_GET_EKEY_ENTRY	:	
			MSG_PRINT("MacSec get EKey entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_GET_EHKEY_ENTRY	:	
			MSG_PRINT("MacSec get EHKey entry error.\n");    
			break;

		case MAD_MSEC_FAIL_PORT_DIS_ILU_ENTRY	:	
			MSG_PRINT("MacSec disable Ilu entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_SET_ILU_ENTRY	:	
			MSG_PRINT("MacSec set Ilu entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_SET_ICT_ENTRY	:	
			MSG_PRINT("MacSec set ICT entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_SET_IKEY_ENTRY	:	
			MSG_PRINT("MacSec set IKey entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_SET_IHKEY_ENTRY	:	
			MSG_PRINT("MacSec set IHKey entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_GET_ILU_ENTRY	:	
			MSG_PRINT("MacSec get Ilu entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_GET_ICT_ENTRY	:	
			MSG_PRINT("MacSec get ICT entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_GET_IKEY_ENTRY	:	
			MSG_PRINT("MacSec get IKey entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_GET_IHKEY_ENTRY	:	
			MSG_PRINT("MacSec get IHKey entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_SET_NEXPN_ENTRY	:	
			MSG_PRINT("MacSec set NexPn entry error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_GET_IGR_ENTRY	:	
			MSG_PRINT("MacSec get IGR entry error.\n");    
			break;

		default:			
			MSG_PRINT("Unknown Error App layer 0L\n");    
			break;
	}

}

void displayStatus_api_1(MAD_STATUS status)
{

/* MAD error status API level 1 */
	switch(status&MAD_ERR_ST_API_1_MASK)
	{
		case 0	:
			break;
		case MAD_API_FAIL_GET_SPEED_ST		:			
			MSG_PRINT("mdCopper Get Speed Status failed. \n");    
			break;
		case MAD_API_FAIL_GET_DUPLEX_ST		:			
			MSG_PRINT("md Duplex Status failed. \n");    
			break;
		case MAD_API_FAIL_GET_LINK_ST		:			
			MSG_PRINT("mdGetLinkStatus failed. \n");    
			break;
		case MAD_API_FAIL_GET_AUTONEG		:			
			MSG_PRINT("mdSGMIIGetAutoNeg return Failed \n");    
			break;
		case MAD_API_FAIL_SET_AUTONEG		:			
			MSG_PRINT("mdSGMIISetAutoNeg return Failed \n");    
			break;
		case MAD_API_FAIL_GET_INT_ST		:			
			MSG_PRINT("mdIntGetStatus failed. \n");    
			break;
		case MAD_API_FAIL_SET_INT_ST		:			
			MSG_PRINT("mdIntSetEnable failed. \n");    
			break;
		case MAD_API_FAIL_VCT_SM_SET		:			
			MSG_PRINT("run VCT State machine set again failed. \n");    
			break;
		case MAD_API_FAIL_VCT_SM_CHECK		:			
			MSG_PRINT("run VCT State machine check again failed. \n");    
			break;
		case MAD_API_FAIL_VCT_SM_GET		:			
			MSG_PRINT("run VCT State machine get again failed. \n");    
			break;
		case MAD_API_FAIL_VCT_CABLELEN		:			
			MSG_PRINT("run VCT Cable length test failed. \n");    
			break;
		case MAD_API_FAIL_VCT_SM_INSTANCE	:			
			MSG_PRINT("Can not find VCT State machine instance . \n");    
			break;
		case MAD_API_FAIL_VCT_SM_IMPLEM		:			
			MSG_PRINT("Can not implement VCT State machine instance . \n");    
			break;
		case MAD_API_FAIL_VCT_INIT			:			
			MSG_PRINT("Enabling work around failed. \n");    
			break;
		case MAD_API_FAIL_VCT_CLOSE			:			
			MSG_PRINT("Disabling work around failed. \n");    
			break;
		case MAD_API_FAIL_VCT_TEST			:
			MSG_PRINT("VCT test failed. No result is valid. \n");
			break;

		case MAD_API_FAIL_EXTVCT_DIST		:			
			MSG_PRINT("getDSPDistance failed. \n");    
			break;
		case MAD_API_FAIL_ADVVCT_RUN		:			
			MSG_PRINT("Running advanced VCT failed. \n");    
			break;
		case MAD_API_FAIL_ALTVCT_RUN		:			
			MSG_PRINT("Running alternate VCT failed. \n");    
			break;
		case MAD_API_FAIL_ADVVCT_SM_SET		:			
			MSG_PRINT("run ADV VCT State machine set again failed. \n");    
			break;
		case MAD_API_FAIL_ADVVCT_SM_CHECK	:			
			MSG_PRINT("run ADV VCT State machine check again failed. \n");    
			break;
		case MAD_API_FAIL_ADVVCT_SM_GET		:			
			MSG_PRINT("run ADV VCT State machine get again failed. \n");    
			break;

		case MAD_API_FAIL_INIT_DRV_CFG		:			
			MSG_PRINT("Initialize driverConfig Failed. \n");    
			break;
		case MAD_API_FAIL_PRE_INIT_DEV		:			
			MSG_PRINT("Pre-Initialize device Failed. \n");    
			break;

		case MAD_API_FAIL_SEM_CREATE		:			
			MSG_PRINT("semCreate Failed. \n");    
			break;
		case MAD_API_FAIL_SEM_DELETE		:			
			MSG_PRINT("semDelete Failed. \n");    
			break;
		case MAD_API_FAIL_STATE_MACHINE		:
			MSG_PRINT("state machine Failed. \n");
			break;
		case MAD_API_FAIL_ADVVCTDSP_ATTEN	:
			MSG_PRINT("DSP_VCT Attenuation Vs Freq error.\n");    
			break;

		case MAD_API_FAIL_ADVVCTDSP_CABLEN	:
			MSG_PRINT("DSP_VCT Cable length error.\n");    
			break;

		case MAD_API_FAIL_ADVVCTDSP_XTALK	:
			MSG_PRINT("DSP_VCT Xtalk vs Freq error.\n");    
			break;

		case MAD_API_FAIL_ADVVCTDSP_CABLETP	 :
			MSG_PRINT("DSP_VCT Ethernet cable type error.\n");    
			break;

		case MAD_API_FAIL_ADVVCTDSP_SPLITPA	 :
			MSG_PRINT("DSP_VCT Split pair error.\n");    
			break;

		case MAD_API_FAIL_ADVVCTDSP_RLOSS	:
			MSG_PRINT("DSP_VCT Return loss error.\n");    
			break;

		case MAD_MSEC_FAIL_INIT				:
			MSG_PRINT("MacSec Initial error.\n");    
			break;
		case MAD_MSEC_FAIL_CMN_CFG			:
			MSG_PRINT("MacSec Initial common config error.\n");    
			break;
		case MAD_MSEC_FAIL_EGR_CFG			:
			MSG_PRINT("MacSec Initial Ergress config error.\n");    
			break;
		case MAD_MSEC_FAIL_OTHER_BLK_INIT			:
			MSG_PRINT("MacSec Initial Other Block error.\n");    
			break;
		case MAD_MSEC_FAIL_PORT_MAC_INIT	:	
			MSG_PRINT("MacSec Initial Port Mac error.\n");    
			break;
		case MAD_MSEC_FAIL_STATUS_INIT		:
			MSG_PRINT("MacSec Initial Status error.\n");    
			break;
		case MAD_MSEC_FAIL_GET_EG_ENTRY		:
			MSG_PRINT("MacSec Get Egress Entry error.\n");    
			break;
		case MAD_MSEC_FAIL_IGR_DEF_ACT		:
			MSG_PRINT("MacSec Set IGR Def Act error.\n");    
			break;

		default:			
			MSG_PRINT("Unknown Error App layer 3L\n");    
			break;
	}

}

void testMADDisplayStatus(MAD_STATUS status)
{
	MSG_PRINT("\n");    
	displayStatus_drv_0(status);
	displayStatus_drv_1(status);
	displayStatus_api_0(status);
	displayStatus_api_1(status);
}

