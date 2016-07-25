#include <madCopyright.h>

/********************************************************************************
* madMACCtrl.c
* 
* DESCRIPTION:
*       APIs to control MAC Interface.
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
* mdSGMIISetAutoNeg
*
* DESCRIPTION:
*       This routine enables or disables SGMII Auto-Neg.
*       Supported modes are 1000M, 100M, or 10M.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE to disable
*       mode - MAD_SPEED_MODE to setup default SGMII speed:
*                   MAD_SPEED_10M,
*                   MAD_SPEED_100M, or
*                   MAD_SPEED_1000M
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15       : Soft reset
*       Page 2, Reg 21, Bit 13,12,6 : MAC If Control 1 (Speed,Auto-neg enable)
*
*******************************************************************************/
MAD_STATUS mdSGMIISetAutoNeg
(
    IN MAD_DEV          *dev,
    IN MAD_LPORT        port,
    IN MAD_BOOL         en,
    IN MAD_SPEED_MODE   mode
)
{
    MAD_U16     data,tmpData;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdSGMIISetAutoNeg called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_SGMII))
    {
        MAD_DBG_ERROR("SGMII not supported.\n");
        return MAD_API_SGMII_NOT_SUPPORT;
    }

    /* Enable or disable Auto mode */
    if (en)
    {
        data = MAD_BIT_12;
    }
    else
    {
        data = 0;
    }

    switch (mode)
    {
        case MAD_SPEED_10M:
            break;
        case MAD_SPEED_100M:
            data |= MAD_BIT_13;
            break;
        case MAD_SPEED_1000M:
            data |= MAD_BIT_6;
            break;
        default:
            MAD_DBG_ERROR("Invalid mode.\n");
            return MAD_API_UNKNOWN_AUTONEG_MODE;
    }

    /* Get the MAC Interface Control Register */
    if((retVal = madHwReadPagedPhyReg(
                    dev,hwPort,2,MAD_REG_MAC_CONTROL,&tmpData)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* Modify the MAC Interface Control data */
    tmpData &= ~(MAD_BIT_13|MAD_BIT_12|MAD_BIT_6);
    tmpData |= data;

    /* Set MAC Interface speed mode */
    if((retVal = madHwWritePagedPhyReg(
                    dev,hwPort,2,MAD_REG_MAC_CONTROL,tmpData)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    /* Soft Reset */
    if((retVal = madHwPagedReset(dev,hwPort,0)) != MAD_OK)
    {
        MAD_DBG_ERROR("Soft Reset failed.\n");
        return (retVal | MAD_API_FAIL_SW_RESET);
    }

    return MAD_OK;
}


/*******************************************************************************
* mdSGMIIGetAutoNeg
*
* DESCRIPTION:
*       This routine retrieves SGMII Auto-Neg mode.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*       mode - default SGMII speed (MAD_SPEED_MODE):
*                   MAD_SPEED_10M,
*                   MAD_SPEED_100M, or
*                   MAD_SPEED_1000M
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, Reg 21, Bit 13,12,6 : MAC If Control 1 (Speed,Auto-neg enable)
*
*******************************************************************************/
MAD_STATUS mdSGMIIGetAutoNeg
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    OUT MAD_BOOL        *en,
    OUT MAD_SPEED_MODE  *mode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdSGMIIGetAutoNeg called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_SGMII))
    {
        MAD_DBG_ERROR("SGMII not supported.\n");
        return MAD_API_SGMII_NOT_SUPPORT;
    }

    /* Get the MAC Interface Control Register */
    if((retVal = madHwReadPagedPhyReg(
                    dev,hwPort,2,MAD_REG_MAC_CONTROL,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* Auto-Nego enabled? */
    if (data & MAD_BIT_12)
        *en = MAD_TRUE;
    else
        *en = MAD_FALSE;

    /* Default speed mode */
    switch (data & (MAD_BIT_6|MAD_BIT_13))
    {
        case 0:
            *mode = MAD_SPEED_10M;
            break;
        case MAD_BIT_6:
            *mode = MAD_SPEED_1000M;
            break;
        case MAD_BIT_13:
            *mode = MAD_SPEED_100M;
            break;
        default:
            *mode = MAD_SPEED_UNKNOWN;
            break;
    }

    return MAD_OK;
}


/*******************************************************************************
* mdRGMIISetSpeed
*
* DESCRIPTION:
*       This routine sets RGMII speed.
*       Supported modes are 1000M, 100M, or 10M.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       mode - MAD_SPEED_MODE to setup RGMII speed:
*                   MAD_SPEED_10M,
*                   MAD_SPEED_100M, or
*                   MAD_SPEED_1000M
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15       : Soft reset
*       Page 2, Reg 21, Bit 13,6    : MAC If Control 1
*
*******************************************************************************/
MAD_STATUS mdRGMIISetSpeed
(
    IN MAD_DEV          *dev,
    IN MAD_LPORT        port,
    IN MAD_SPEED_MODE   mode
)
{
    MAD_U16     data,tmpData;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdRGMIISetSpeed called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_RGMII))
    {
        MAD_DBG_ERROR("RGMII not supported.\n");
        return MAD_API_RGMII_NOT_SUPPORT;
    }

    data = 0;

    switch (mode)
    {
        case MAD_SPEED_10M:
            break;
        case MAD_SPEED_100M:
            data |= MAD_BIT_13;
            break;
        case MAD_SPEED_1000M:
            data |= MAD_BIT_6;
            break;
        default:
            MAD_DBG_ERROR("Invalid mode.\n");
            return MAD_API_UNKNOWN_SPEED_MODE;
    }

    /* Get the MAC Interface Control Register */
    if((retVal = madHwReadPagedPhyReg(
                    dev,hwPort,2,MAD_REG_MAC_CONTROL,&tmpData)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* Modify the MAC Interface Control data */
    tmpData &= ~(MAD_BIT_13|MAD_BIT_6);
    tmpData |= data;

    /* Set MAC Interface speed mode */
    if((retVal = madHwWritePagedPhyReg(
                    dev,hwPort,2,MAD_REG_MAC_CONTROL,tmpData)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    /* Soft Reset */
    if((retVal = madHwPagedReset(dev,hwPort,0)) != MAD_OK)
    {
        MAD_DBG_ERROR("Soft Reset failed.\n");
        return (retVal | MAD_API_FAIL_SW_RESET);
    }

    return MAD_OK;
}


/*******************************************************************************
* mdRGMIIGetSpeed
*
* DESCRIPTION:
*       This routine retrieves RGMII speed.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       mode - RGMII speed (MAD_SPEED_MODE):
*                   MAD_SPEED_10M,
*                   MAD_SPEED_100M, or
*                   MAD_SPEED_1000M
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, Reg 21, Bit 13,6 : MAC If Control 1 (Speed)
*
*******************************************************************************/
MAD_STATUS mdRGMIIGetSpeed
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    OUT MAD_SPEED_MODE  *mode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdRGMIIGetSpeed called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_RGMII))
    {
        MAD_DBG_ERROR("RGMII not supported.\n");
        return MAD_API_RGMII_NOT_SUPPORT;
    }

    /* Get the MAC Interface Control Register */
    if((retVal = madHwReadPagedPhyReg(
                    dev,hwPort,2,MAD_REG_MAC_CONTROL,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* Default speed mode */
    switch (data & (MAD_BIT_6|MAD_BIT_13))
    {
        case 0:
            *mode = MAD_SPEED_10M;
            break;
        case MAD_BIT_6:
            *mode = MAD_SPEED_1000M;
            break;
        case MAD_BIT_13:
            *mode = MAD_SPEED_100M;
            break;
        default:
            *mode = MAD_SPEED_UNKNOWN;
            return MAD_API_UNKNOWN_SPEED_MODE;
            break;
    }

    return MAD_OK;
}


/*******************************************************************************
* mdMacSetFifoDepth
*
* DESCRIPTION:
*       This routine sets Mac transmit/Receive FIFO depth.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       transFifoDepth - MAD_SYNC_TRANS_FIFO_XX 
*       recvFifoDepth  - MAD_SYNC_RECV_FIFO_XX
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 2, Reg 16, Bit 12,13,14,15    : MAC If Control 1
*
*******************************************************************************/
MAD_STATUS mdMacSetFifoDepth
(
    IN MAD_DEV          *dev,
    IN MAD_LPORT        port,
    IN MAD_U8            transFifoDepth,
    IN MAD_U8            recvFifoDepth
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdMacSetFifoDepth called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_SYNCRO_FIFO))
  {
     MAD_DBG_ERROR("Syncronize FIFO not supported.\n");
     return MAD_API_SYNCRO_FIFO_NOT_SUPPORT;
  }


  data = 0;
  data |= (transFifoDepth << 2) | recvFifoDepth;

  if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,2,MAD_REG_MAC_CONTROL_1,
                    12,4,data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
    return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }

  return MAD_OK;
}


/*******************************************************************************
* mdMacGetFifoDepth
*
* DESCRIPTION:
*       This routine sets Mac transmit/Receive FIFO depth.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       transFifoDepth - MAD_SYNC_TRANS_FIFO_XX 
*       recvFifoDepth  - MAD_SYNC_RECV_FIFO_XX
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, Reg 16, Bit 12,13,14,15    : MAC If Control 1
*
*******************************************************************************/
MAD_STATUS mdMacGetFifoDepth
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    OUT MAD_U8            *transFifoDepth,
    OUT MAD_U8            *recvFifoDepth
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdMacGetFifoDepth called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_SYNCRO_FIFO))
  {
     MAD_DBG_ERROR("Syncronize FIFO not supported.\n");
     return MAD_API_SYNCRO_FIFO_NOT_SUPPORT;
  }


  if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,2,MAD_REG_MAC_CONTROL_1,
                    12,4, &data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
    return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }

  *transFifoDepth = (data >> 2) & 0x3;
  *recvFifoDepth  =  data & 0x3;

  return MAD_OK;
}


