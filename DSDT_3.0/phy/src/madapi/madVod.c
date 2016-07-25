#include <madCopyright.h>

/********************************************************************************
* madVod.c
* 
* DESCRIPTION:
*       APIs to adjust VOD.
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
* mdDiagSetSGMIIVod
*
* DESCRIPTION:
*       This routine sets differential output voltage of SGMII.
*        The following VODs are supported to 1181, and 1149:
*           MAD_SGMII_VOD_14mV,
*           MAD_SGMII_VOD_112mV,
*           MAD_SGMII_VOD_210mV,
*           MAD_SGMII_VOD_308mV, and
*           MAD_SGMII_VOD_406mV
*        For 88E1112:
*           MAD_SGMII_VOD_14mV is 0.50v
*           MAD_SGMII_VOD_112mV is 0.60v,
*           MAD_SGMII_VOD_210mV is 0.70v,
*           MAD_SGMII_VOD_308mV is 0.80v, and
*           MAD_SGMII_VOD_406mV is 0.90v.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       vod  - MAD_SGMII_VOD value
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 2, Reg 26, Bit 2:0 : SGMII Output Amplitude
*
*******************************************************************************/
MAD_STATUS mdDiagSetSGMIIVod
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    IN  MAD_SGMII_VOD   vod
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdDiagSetSGMIIVod called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_VOD_ADJUST))
    {
        MAD_DBG_ERROR("SGMII VOD not supported.\n");
        return MAD_API_VOD_NOT_SUPPORT;
    }

    switch (vod)
    {
        case MAD_SGMII_VOD_14mV:
            data = 0;
            break;
        case MAD_SGMII_VOD_112mV:
            data = 1;
            break;
        case MAD_SGMII_VOD_210mV:
            data = 2;
            break;
        case MAD_SGMII_VOD_308mV:
            data = 3;
            break;
        case MAD_SGMII_VOD_406mV:
            data = 4;
            break;
        default:
            MAD_DBG_ERROR("Invalid vod\n");
            return MAD_API_UNKNOWN_VOD_PARA;
    }

    /* Set the required bits for SGMII Output Amplitude */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,2,MAD_REG_MAC_CONTROL_2,
                    0,3,data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    return MAD_OK;
}


/*******************************************************************************
* mdDiagSetCopperVod
*
* DESCRIPTION:
*       This routine sets differential output voltage of Copper.
*       VOD amplitude on the MDI pins can be adjusted for 10/100/1000Mbps speeds
*       of operation with this function.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       mode - Speed mode (MAD_SPEED_MODE: 10M,100M,or 1000M)
*       vod  - Copper VOD value (MAD_COPPER_VOD)
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page ff, Reg 24, Bit 15:0        : VOD adjust for each MDIs
*       Page ff, Reg 23, Bit 15,13,5,1,0 : apply VOD adjustment
*
*******************************************************************************/
MAD_STATUS mdDiagSetCopperVod
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    IN  MAD_SPEED_MODE  mode,
    IN  MAD_COPPER_VOD  vod
)
{
    MAD_U16     data, addr;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdDiagSetCopperVod called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_VOD_ADJUST))
    {
        MAD_DBG_ERROR("SGMII VOD not supported.\n");
        return MAD_API_VOD_NOT_SUPPORT;
    }

    switch (vod)
    {
        case MAD_VOD_0_PERCENT:
            data = 0;
            break;
        case MAD_VOD_2_PERCENT:
            data = 9;
            break;
        case MAD_VOD_4_PERCENT:
            data = 10;
            break;
        case MAD_VOD_6_PERCENT:
            data = 11;
            break;
        case MAD_VOD_8_PERCENT:
            data = 12;
            break;
        case MAD_VOD_10_PERCENT:
            data = 13;
            break;
        case MAD_VOD_12_PERCENT:
            data = 14;
            break;
        case MAD_VOD_14_PERCENT:
            data = 15;
            break;
        case MAD_VOD_Minus_2_PERCENT:
            data = 1;
            break;
        case MAD_VOD_Minus_4_PERCENT:
            data = 2;
            break;
        case MAD_VOD_Minus_6_PERCENT:
            data = 3;
            break;
        case MAD_VOD_Minus_8_PERCENT:
            data = 4;
            break;
        case MAD_VOD_Minus_10_PERCENT:
            data = 5;
            break;
        case MAD_VOD_Minus_12_PERCENT:
            data = 6;
            break;
        case MAD_VOD_Minus_14_PERCENT:
            data = 7;
            break;
        default:
            MAD_DBG_ERROR("Invalid vod\n");
            return MAD_API_UNKNOWN_VOD_PARA;
    }

    /* Select speed and Loopback Mode */
    switch (mode)
    {
        case MAD_SPEED_10M:
            addr = 0x2011;
            data |= (data << 4);
            break;
        case MAD_SPEED_100M:
            addr = 0x2011;
            data |= (data << 4);
            data <<= 8;
            break;
        case MAD_SPEED_1000M:
            addr = 0x2012;
            data |= (data << 4);
            data |= (data << 8);
            break;
        default:
            MAD_DBG_ERROR("Invalid speed\n");
            return MAD_API_UNKNOWN_SPEED_MODE;
    }

    /* Set the required bits for MDI Amplitude */
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,0xff,24,data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    /* Set the required bits for AFE Register */
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,0xff,23,addr)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    return MAD_OK;
}


