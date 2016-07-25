#include <madCopyright.h>

/********************************************************************************
* madSyncClkCtrl.c
* 
* DESCRIPTION:
*       APIs to control Syncronous Clocking.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <madApi.h>
#include <madApiInternal.h>
#include <madHwCntl.h>
#include <madDrvPhyRegs.h>
#include <madDrvConfig.h>



/*******************************************************************************
* mdSyncClkSetRClk
*
* DESCRIPTION:
*       This routine sets the RCLK1 and RCLK2 pins of the chip outputs 
*		either a 125MHz or 25 MHz clock
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       rclk - MAD_SYNC_CLK_RCLK1/MAD_SYNC_CLK_RCLK2/MAD_SYNC_CLK_RCLK2.
*		frq	 - MAD_SYNC_CLK_FREQ_25M/MAD_SYNC_CLK_FREQ_125M
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 2, Reg 16, Bit 8, 9, 12    
*
*******************************************************************************/
MAD_STATUS mdSyncClkSetRClk
(
    IN MAD_DEV          *dev,
    IN MAD_LPORT        port,
    IN MAD_8            rclk,
    IN MAD_8            freq
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdSyncClkSetRClk called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  { 
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY1(dev, MAD_PHY_SYNC_ETHNET)))
  {
    MAD_DBG_ERROR("Sync Ethenet is not supported.\n");
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }


	if(rclk==MAD_SYNC_CLK_RCLK1)
      data = 1;
	else if(rclk==MAD_SYNC_CLK_RCLK2)
      data = 2;
	else if(rclk==MAD_SYNC_CLK_RCLK1_2)
      data = 3;
	else
      data = 0;


    /* Set the MAC Interface Control Register */
	if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE2,MAD_REG_MAC_CONTROL_1, 8,2, data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

	if(freq==MAD_SYNC_CLK_FREQ_25M)
      data = 0;
	else if(freq==MAD_SYNC_CLK_FREQ_125M)
      data = 1;
	else
      return MAD_API_PTP_BAD_PARAM;


    /* Set the MAC Interface Control Register */
	if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE2,MAD_REG_MAC_CONTROL_1, 12,1, data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }


    return MAD_OK;
}


/*******************************************************************************
* mdSyncClkGetRClk
*
* DESCRIPTION:
*       This routine gets the RCLK1 and RCLK2 pins of the chip outputs 
*		either a 125MHz or 25 MHz clock
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       rclk - MAD_SYNC_CLK_RCLK1/MAD_SYNC_CLK_RCLK2/MAD_SYNC_CLK_RCLK2.
*		frq	 - MAD_SYNC_CLK_FREQ_25M/MAD_SYNC_CLK_FREQ_125M
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 2, Reg 16, Bit 8, 9, 12    
*
*******************************************************************************/
MAD_STATUS mdSyncClkGetRClk
(
    IN MAD_DEV          *dev,
    IN MAD_LPORT        port,
    OUT MAD_8            *rclk,
    OUT MAD_8            *freq
)
{
  MAD_U16     data, bits;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdSyncClkGetRClk called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY1(dev, MAD_PHY_SYNC_ETHNET)))
  {
    MAD_DBG_ERROR("Sync Ethenet is not supported.\n");
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }

    /* Get the MAC Interface Control Register */
	if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE2,MAD_REG_MAC_CONTROL_1, 8,2, &data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }
	if(data==1)
      *rclk = MAD_SYNC_CLK_RCLK1;
	else if(data==2)
      *rclk = MAD_SYNC_CLK_RCLK2;
	else if(data==3)
      *rclk = MAD_SYNC_CLK_RCLK1_2;
	else
      *rclk = MAD_SYNC_CLK_RCLK_NO;

    /* Get the MAC Interface Control Register */
	if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE2,MAD_REG_MAC_CONTROL_1, 12,1, &data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

	*freq=data;
	if (data==0)
	  *freq=MAD_SYNC_CLK_FREQ_25M;
	else if(data==1)
	  *freq=MAD_SYNC_CLK_FREQ_125M;
	else
	  *freq=MAD_SYNC_CLK_FREQ_NO;


    return MAD_OK;
}


/*******************************************************************************
* mdSyncClkSetClkSelect
*
* DESCRIPTION:
*       This routine sets to select between the local reference clock and a 
*		cleaned up recovered clock.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       interf - MAD_SYNC_CLK_MODE_PORT_COPPER
*					MAD_SYNC_CLK_MODE_PORT_125SERDES
*					MAD_SYNC_CLK_MODE_PORT_QSGMII
*		refClk	 - MAD_SYNC_CLK_REF_CLK_XTAL/MAD_SYNC_CLK_REF_CLK_SCLK
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 2, Reg 16, Bit 7, 6    
*       Page 4, Reg 26, Bit 14   
*
*******************************************************************************/
MAD_STATUS mdSyncClkSetClkSelect
(
    IN MAD_DEV          *dev,
    IN MAD_LPORT        port,
    IN MAD_8            interf,
    IN MAD_8            refClk
)
{
  MAD_U16     data,page,reg, bits;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdSyncClkSetClkSelect called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }
  if(!(MAD_DEV_CAPABILITY1(dev, MAD_PHY_SYNC_ETHNET)))
  {
    MAD_DBG_ERROR("Sync Ethenet is not supported.\n");
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }


	if(interf==MAD_SYNC_CLK_MODE_PORT_COPPER)
	{
      page=MAD_PAGE2;
	  reg=MAD_REG_MAC_CONTROL_1;
      bits = 7;
	}
	else if(interf==MAD_SYNC_CLK_MODE_PORT_125SERDES)
	{
      page=MAD_PAGE2;
	  reg=MAD_REG_MAC_CONTROL_1;
      bits = 6;
	}
	else if(interf==MAD_SYNC_CLK_MODE_PORT_QSGMII)
	{
      page=MAD_PAGE4;
	  reg=MAD_REG_QSGMII_GLOBAL_CTRL_1;
      bits = 14;
	}
	else
      return MAD_API_PTP_BAD_PARAM;


	if(refClk==MAD_SYNC_CLK_REF_CLK_XTAL)
		data=0;
	else if(refClk==MAD_SYNC_CLK_REF_CLK_SCLK)
		data=1;
	else
      return MAD_API_PTP_BAD_PARAM;

    /* Set the MAC Interface Control Register */
	if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,page, reg, bits,1, data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    return MAD_OK;
}


/*******************************************************************************
* mdSyncClkGetClkSelect
*
* DESCRIPTION:
*       This routine gets to select between the local reference clock and a 
*		cleaned up recovered clock.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       interf - MAD_SYNC_CLK_MODE_PORT_COPPER
*					MAD_SYNC_CLK_MODE_PORT_125SERDES
*					MAD_SYNC_CLK_MODE_PORT_QSGMII
*   
* OUTPUTS:
*		refClk	 - MAD_SYNC_CLK_REF_CLK_XTAL/MAD_SYNC_CLK_REF_CLK_SCLK
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 2, Reg 16, Bit 7, 6    
*       Page 4, Reg 26, Bit 14   
*
*******************************************************************************/
MAD_STATUS mdSyncClkGetClkSelect
(
    IN MAD_DEV          *dev,
    IN MAD_LPORT        port,
    IN MAD_8            interf,
    OUT MAD_8           *refClk
)
{
  MAD_U16     data, page,reg, bits;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdSyncClkGetClkSelect called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY1(dev, MAD_PHY_SYNC_ETHNET)))
  {
    MAD_DBG_ERROR("Sync Ethenet is not supported.\n");
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }

	if(interf==MAD_SYNC_CLK_MODE_PORT_COPPER)
	{
      page=MAD_PAGE2;
	  reg=MAD_REG_MAC_CONTROL_1;
      bits = 7;
	}
	else if(interf==MAD_SYNC_CLK_MODE_PORT_125SERDES)
	{
      page=MAD_PAGE2;
	  reg=MAD_REG_MAC_CONTROL_1;
      bits = 6;
	}
	else if(interf==MAD_SYNC_CLK_MODE_PORT_QSGMII)
	{
      page=MAD_PAGE4;
	  reg=MAD_REG_QSGMII_GLOBAL_CTRL_1;
      bits = 14;
	}
	else
      return MAD_API_PTP_BAD_PARAM;


    /* Get the MAC Interface Control Register */
	if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,page, reg, bits,1, &data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }
	if(data==0)
	  *refClk=MAD_SYNC_CLK_REF_CLK_XTAL;
	else if(data==1)
	  *refClk=MAD_SYNC_CLK_REF_CLK_SCLK;
	else
	  *refClk=MAD_SYNC_CLK_REF_CLK_NO;

    return MAD_OK;
}




