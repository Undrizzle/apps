#include <madCopyright.h>

/********************************************************************************
* madStats.c
* 
* DESCRIPTION:
*       APIs for Statistics.
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
* mdStatsGetIdleErrorCounter
*
* DESCRIPTION:
*       This routine gets idle error counter. Idle error counter is cleared 
*       after this function call. This counter pegs at 0xFF and is not roll over.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       count - Idle error counter
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 10, Bit 7:0 : Idle Error Count
*
*******************************************************************************/
MAD_STATUS mdStatsGetIdleErrorCounter
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U32     *count
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdStatsGetIdleErrorCounter called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    if ((!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER)) ||
         (!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_FIBER)) )
    {
        MAD_DBG_ERROR("Copper not supported.\n");
        return MAD_API_COPPER_NOT_SUPPORT;
    }

    /* Get the required bits in MAD_REG_1000_STATUS */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_1000_STATUS,
                    0,8,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }

    *count = (MAD_U32)data;

    return MAD_OK;
}


/*
    CRC Checker for the device using register 16_6 and 17_6
*/
static
MAD_STATUS madCRCCheck_16_6
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_U8    mode,
    IN  MAD_BOOL  en
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
 
    /* Enable or disable */
    MAD_BOOL_2_BIT(en,data);

#if 1
    /* Set the CRC Checker path */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,6,MAD_REG_PACKET_GENERATION,
                    7,1,mode)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
#endif


    /* Set the CRC Checker Enable bit */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,6,MAD_REG_PACKET_GENERATION,
                    4,1,data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    return MAD_OK;
}

/*
    CRC Checker for the device using register 18_6 and 17_6 for 1340
*/
static
MAD_STATUS madCRCCheck_1340
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_U8    mode,
    IN  MAD_BOOL  en
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
 

    /* Enable or disable */
    MAD_BOOL_2_BIT(en,data);

    if (en != MAD_TRUE)
      mode = 0;

    /* Set the CRC Checker path */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,6,MAD_REG_CHECK_CONTROL,
                    0,3,mode)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    return MAD_OK;
}

/*
    CRC Checker for the device using register 30_16 and 30_12
*/
static
MAD_STATUS madCRCCheck_30_16
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_BOOL  en
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
 
    /* Enable or disable */
    MAD_BOOL_2_BIT(en,data);

        if((retVal = madHwWritePagedPhyReg (
                        dev,hwPort,0x10, MAD_REG_PAGE_ACCESS,
                        data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }


   return MAD_OK;
}

/*
    CRC Checker for the device using register 29 and 30
*/
static
MAD_STATUS madCRCCheck_30
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_BOOL  en
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
 
    /* Enable or disable */
    MAD_BOOL_2_BIT(en,data);

    if((retVal = madHwWritePagedPhyReg (
                    dev,hwPort, 0x9, MAD_REG_PAGE_ACCESS,
                    data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }


   return MAD_OK;
}


/*******************************************************************************
* mdStatsSetCRCCheckerEnable
*
* DESCRIPTION:
*       This routine enables or disables CRC Checker. When CRC Checker is 
*       enabled, both CRC Error Counter Frame Counter are enabled. The counters
*       do not clear on a read. To clear counters, disable and enable the CRC
*       Checker.
*       Both counters will peg to 0xFF.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*        mode - Counter path (Copper or SGMII...)
*                For all exept for 1340
*                    0: Copper receive payh
*                    1: SGMII input path
*                For 1340
*                    MAD_CHECKER_CTRL_DISABLE    0
*                    MAD_CHECKER_CTRL_COPPER        2
*                    MAD_CHECKER_CTRL_SGMII        4
*                    MAD_CHECKER_CTRL_MACSEC_TR    5
*                    MAD_CHECKER_CTRL_QSGMII        6
*                    MAD_CHECKER_CTRL_MACSEC_REC    7
*       en   - MAD_TRUE to enable, MAD_FALSE otherwise
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 6, Reg 16, Bit 4 : CRC Counter Enable
*
*******************************************************************************/
MAD_STATUS mdStatsSetCRCCheckerEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_U8      mode,
    IN  MAD_BOOL    en
)
{
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdStatsSetCRCCheckerEnable called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_CRC_CHECKER))
    {
        MAD_DBG_ERROR("feature not supported.\n");
        return MAD_API_CRC_CHECK_NOT_SUPPORT;
    }

    switch(dev->phyInfo.crcCheckType)
    {
        case MAD_PHY_CRCCHECK_TYPE1:
            return madCRCCheck_16_6(dev,hwPort,mode, en);

        case MAD_PHY_CRCCHECK_TYPE2:
            return madCRCCheck_30_16(dev,hwPort,en);

        case MAD_PHY_CRCCHECK_TYPE3:
            return madCRCCheck_30(dev,hwPort,en);

        case MAD_PHY_CRCCHECK_TYPE4:  /* for 1340 */
            return madCRCCheck_1340(dev,hwPort,mode, en);

         default:
            return MAD_API_UNKNOWN_CRC_CHECK_TYPE;
    }

    return MAD_OK;
}

/*
    CRC Checker status in register 16_7 and 16_4
*/
static
MAD_STATUS madCRCGetStatus_16_6
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    OUT  MAD_U8    *mode,
    OUT  MAD_BOOL  *en
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
 
    /* Set the CRC Checker path */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE6,MAD_REG_PACKET_GENERATION,
                    7,1,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    *mode = data&0xff;

    /* Set the CRC Checker Enable bit */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,6,MAD_REG_PACKET_GENERATION,
                    4,1,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    /* Enable or disable */
    MAD_BIT_2_BOOL(data, *en);

    return MAD_OK;
}
/*
    CRC Checker status in register 18_6
*/
static
MAD_STATUS madCRCGetStatus_1340
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    OUT  MAD_U8    *mode,
    OUT  MAD_BOOL  *en
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
 
    /* Set the CRC Checker path */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,6,MAD_REG_CHECK_CONTROL,
                    0,3,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
    
    *mode = data&0xff;

    if (*mode==0)
        *en = MAD_FALSE;
    else
        *en = MAD_TRUE;


    return MAD_OK;
}


/*******************************************************************************
* mdStatsGetCRCCheckerEnable
*
* DESCRIPTION:
*       This routine gets enable path qnd status of CRC Checker. 
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*        mode - Counter path (Copper or SGMII...)
*                For all exept for 1340
*                    0: Copper receive payh
*                    1: SGMII input path
*                For 1340
*                    MAD_CHECKER_CTRL_DISABLE    0
*                    MAD_CHECKER_CTRL_COPPER        2
*                    MAD_CHECKER_CTRL_SGMII        4
*                    MAD_CHECKER_CTRL_MACSEC_TR    5
*                    MAD_CHECKER_CTRL_QSGMII        6
*                    MAD_CHECKER_CTRL_MACSEC_REC    7
*       en   - MAD_TRUE to enable, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 6, Reg 16, Bit 4 : CRC Counter Enable
*
*******************************************************************************/
MAD_STATUS mdStatsGetCRCCheckerEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT  MAD_U8     *mode,
    OUT  MAD_BOOL   *en
)
{
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdStatsGetCRCCheckerEnable called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_CRC_CHECKER))
    {
        MAD_DBG_ERROR("feature not supported.\n");
        return MAD_API_CRC_CHECK_NOT_SUPPORT;
    }

    switch(dev->phyInfo.crcCheckType)
    {
        case MAD_PHY_CRCCHECK_TYPE1:
            return madCRCGetStatus_16_6(dev,hwPort,mode, en);

        case MAD_PHY_CRCCHECK_TYPE4:  /* for 1340 */
            return madCRCGetStatus_1340(dev,hwPort,mode, en);

         default:
            return MAD_API_UNKNOWN_CRC_CHECK_TYPE;
    }

    return MAD_OK;
}

/*
    CRC Counter for the device using register 17_6
*/
static
MAD_STATUS madCRCCounter_17_6
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_BOOL  low16,
    OUT MAD_U32   *count
)
{
    MAD_U16     data, offset;
    MAD_STATUS  retVal;

    offset = (low16)?0:8;

    /* Get the required bits in CRC Counter Register */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,6,MAD_REG_CRC_COUNTERS,
                    (MAD_U8)(offset&0xff),8,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }

    *count = (MAD_U32)data;

    return MAD_OK;
}

/*
    CRC Counter for the device using register 30_12
*/
static
MAD_STATUS madCRCCounter_30_12
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_BOOL  low16,
    OUT MAD_U32   *count
)
{
    MAD_U16     data, offset;
    MAD_STATUS  retVal;

    offset = (low16)?0:8;

    /* Get the required bits in CRC Counter Register */


    if((retVal = madHwReadPagedPhyReg(
                    dev,hwPort,0x0c, MAD_REG_PAGE_ACCESS,
                    &data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    *count = (MAD_U32) (0xff & ( data >> (offset)));

    return MAD_OK;
}

/*
    CRC Counter for the device using register 30
*/
static
MAD_STATUS madCRCCounter_30
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_BOOL  low16,
    OUT MAD_U32   *count
)
{
    MAD_U16     data, offset;
    MAD_STATUS  retVal;

    offset = (low16)?0:8;

    /* Get the required bits in CRC Counter Register */


    if((retVal = madHwReadPagedPhyReg(
                    dev,hwPort, 0x9, MAD_REG_PAGE_ACCESS,
                    &data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }
    *count = (MAD_U32) (0xff & ( data >> (offset)));

    return MAD_OK;
}

/*******************************************************************************
* mdStatsGetCRCErrorCounter
*
* DESCRIPTION:
*       This routine gets CRC error counter.
*       In order to activate CRC error counter, CRC Checker should be enabled
*       with mdStatsSetCRCCounterEnable function.
*       In order to clear the counter, CRC counter should be disabled and then
*       enabled again.
*       This counter will peg to 0xFF.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       count - CRC error counter
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 6, Reg 17, Bit 7:0 : CRC error counter
*
*******************************************************************************/
MAD_STATUS mdStatsGetCRCErrorCounter
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U32     *count
)
{
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdStatsGetCRCErrorCounter called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_CRC_CHECKER))
    {
        MAD_DBG_ERROR("feature not supported.\n");
        return MAD_API_CRC_ERR_COUNT_NOT_SUPPORT;
    }

    switch(dev->phyInfo.crcCheckType)
    {
        case MAD_PHY_CRCCHECK_TYPE1:
        case MAD_PHY_CRCCHECK_TYPE4:  /* for 1340 */
            return madCRCCounter_17_6(dev,hwPort,MAD_TRUE,count);

        case MAD_PHY_CRCCHECK_TYPE2:
            return madCRCCounter_30_12(dev,hwPort,MAD_TRUE,count);

        case MAD_PHY_CRCCHECK_TYPE3:
            return madCRCCounter_30(dev,hwPort,MAD_TRUE,count);

        default:
            return MAD_API_UNKNOWN_CRC_ERR_COUNT_TYPE;
    }

    return MAD_OK;
}


/*******************************************************************************
* mdStatsGetFrameCounter
*
* DESCRIPTION:
*       This routine gets Frame counter. 
*       In order to activate CRC error counter, CRC Checker should be enabled
*       with mdStatsSetCRCCounterEnable function.
*       In order to clear the counter, CRC counter should be disabled and then
*       enabled again.
*       This counter will peg to 0xFF.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       count - CRC error counter
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 6, Reg 17, Bit 15:8 : Frame counter
*
*******************************************************************************/
MAD_STATUS mdStatsGetFrameCounter
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U32     *count
)
{
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdStatsGetCRCErrorCounter called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_CRC_CHECKER))
    {
        MAD_DBG_ERROR("feature not supported.\n");
        return MAD_API_CRC_FRM_COUNT_NOT_SUPPORT;
    }

    switch(dev->phyInfo.crcCheckType)
    {
        case MAD_PHY_CRCCHECK_TYPE1:
        case MAD_PHY_CRCCHECK_TYPE4:  /* for 1340 */
            return madCRCCounter_17_6(dev,hwPort,MAD_FALSE,count);

        case MAD_PHY_CRCCHECK_TYPE2:
            return madCRCCounter_30_12(dev,hwPort,MAD_FALSE,count);

        case MAD_PHY_CRCCHECK_TYPE3:
            /* TYPE 3 for 3016, there is no frame count */
            return MAD_API_CRC_FRM_COUNT_NOT_SUPPORT;

        default:
            return MAD_API_UNKNOWN_CRC_FRM_COUNT_TYPE;
    }

    return MAD_OK;
}


/*******************************************************************************
* mdStatsGetRxErCapture
*
* DESCRIPTION:
*       This routine gets RX_ER Capture Valid bit. 
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       path -  1: receive path of QSGMII, 0: receive path of Copper/Fiber.
*   
* OUTPUTS:
*       capture - MAD_TRUE: Capture data valid. MAD_FALSE: otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, 4, Reg 20, Bit 15 : RX_ER Capture
*
*******************************************************************************/
MAD_STATUS mdStatsGetRxErCapture
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_U8        path,
    OUT MAD_BOOL     *capture
)
{
    MAD_U8      hwPort, page;
    MAD_STATUS  retVal;
    MAD_U16     data;

    MAD_DBG_INFO("mdStatsGetRxErCapture called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_RX_ER_BYTE_CAPTURE))
    {
        MAD_DBG_ERROR("feature not supported.\n");
        return MAD_API_CRC_FRM_COUNT_NOT_SUPPORT;
    }

    if (path)
    {
        page = MAD_PAGE4;
    }
    else
    {
        page = MAD_PAGE2;
    }
    /* Get the required bits in CRC Counter Register */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,page,MAD_REG_NAC_RX_ER_BYTE_CAPT,
                    15, 1,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }

    /* Enable or disable */
    MAD_BIT_2_BOOL(data, *capture);

    return MAD_OK;
}

/*******************************************************************************
* mdStatsGetRxErByteNun
*
* DESCRIPTION:
*       This routine gets RX_ER Byte number increments after every read.. 
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       path -  1: receive path of QSGMII, 0: receive path of Copper/Fiber.
*   
* OUTPUTS:
*       byteNum - RX_ER Byte number increments
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, 4, Reg 20, Bit 13:12 
*
*******************************************************************************/
MAD_STATUS mdStatsGetRxErByteNun
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_U8        path,
    OUT MAD_BOOL     *byteNum
)
{
    MAD_U8      hwPort, page;
    MAD_STATUS  retVal;
    MAD_U16     data;

    MAD_DBG_INFO("mdStatsGetRxErByteNun called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_RX_ER_BYTE_CAPTURE))
    {
        MAD_DBG_ERROR("feature not supported.\n");
        return MAD_API_CRC_FRM_COUNT_NOT_SUPPORT;
    }

    if (path)
    {
        page = MAD_PAGE4;
    }
    else
    {
        page = MAD_PAGE2;
    }
    /* Get the required bits in CRC Counter Register */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,page,MAD_REG_NAC_RX_ER_BYTE_CAPT,
                    12, 2,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }

    *byteNum = data;
    return MAD_OK;
}

/*******************************************************************************
* mdStatsGetRxEr
*
* DESCRIPTION:
*       This routine gets RX error.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       path -  1: receive path of QSGMII, 0: receive path of Copper/Fiber.
*   
* OUTPUTS:
*       rxEr - RX_ER Error. Normaly is 0. 
*                            When it is long sequence of RX_EX, it ia 1
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, 4, Reg 20, Bit 9 
*
*******************************************************************************/
MAD_STATUS mdStatsGetRxEr
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_U8        path,
    OUT MAD_U8     *rxEr
)
{
    MAD_U8      hwPort, page;
    MAD_STATUS  retVal;
    MAD_U16     data;

    MAD_DBG_INFO("mdStatsGetRxEr called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_RX_ER_BYTE_CAPTURE))
    {
        MAD_DBG_ERROR("feature not supported.\n");
        return MAD_API_CRC_FRM_COUNT_NOT_SUPPORT;
    }

    if (path)
    {
        page = MAD_PAGE4;
    }
    else
    {
        page = MAD_PAGE2;
    }
    /* Get the required bits in CRC Counter Register */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,page,MAD_REG_NAC_RX_ER_BYTE_CAPT,
                    9, 1,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }

    *rxEr = data&0xff;
    return MAD_OK;
}

/*******************************************************************************
* mdStatsGetRxDv
*
* DESCRIPTION:
*       This routine gets RX data valid.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       path -  1: receive path of QSGMII, 0: receive path of Copper/Fiber.
*   
* OUTPUTS:
*       rxDv - RX data valid. MAD_TRUE: RX data valid. MAD_FALSE: otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, 4, Reg 20, Bit 8 
*
*******************************************************************************/
MAD_STATUS mdStatsGetRxDv
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_U8        path,
    OUT MAD_BOOL    *rxDv
)
{
    MAD_U8      hwPort, page;
    MAD_STATUS  retVal;
    MAD_U16     data;

    MAD_DBG_INFO("mdStatsGetRxDv called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_RX_ER_BYTE_CAPTURE))
    {
        MAD_DBG_ERROR("feature not supported.\n");
        return MAD_API_CRC_FRM_COUNT_NOT_SUPPORT;
    }

    if (path)
    {
        page = MAD_PAGE4;
    }
    else
    {
        page = MAD_PAGE2;
    }
    /* Get the required bits in CRC Counter Register */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,page,MAD_REG_NAC_RX_ER_BYTE_CAPT,
                    8, 1,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }

    /* Enable or disable */
    MAD_BIT_2_BOOL(data, *rxDv);
    return MAD_OK;
}

/*******************************************************************************
* mdStatsGetRxData
*
* DESCRIPTION:
*       This routine gets RX_ER Data. 
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       path -  1: receive path of QSGMII, 0: receive path of Copper/Fiber.
*   
* OUTPUTS:
*       rxData - RX_ER Data
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 2, 4, Reg 20, Bit 7:0 
*
*******************************************************************************/
MAD_STATUS mdStatsGetRxData
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_U8        path,
    OUT MAD_U8     *rxData
)
{
    MAD_U8      hwPort, page;
    MAD_STATUS  retVal;
    MAD_U16     data;

    MAD_DBG_INFO("mdStatsGetRxData called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_RX_ER_BYTE_CAPTURE))
    {
        MAD_DBG_ERROR("feature not supported.\n");
        return MAD_API_CRC_FRM_COUNT_NOT_SUPPORT;
    }

    if (path)
    {
        page = MAD_PAGE4;
    }
    else
    {
        page = MAD_PAGE2;
    }
    /* Get the required bits in CRC Counter Register */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,page,MAD_REG_NAC_RX_ER_BYTE_CAPT,
                    0, 8,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }

    *rxData = data&0xff;
    return MAD_OK;
}


/*******************************************************************************
* mdSysGetLinkDisconnectCounter
*
* DESCRIPTION:
*       This function get Link Disconnect counter.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       count -    
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetLinkDisconnectCounter
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U16     *count
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdSysGetLinkDisconnectCounter called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(MAD_DEV_CAPABILITY1(dev, MAD_PHY_LINK_DISCONNECT_INDC))
  {
    /* Get the Snooping controln mode */
    if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE6,MAD_REG_LATE_COLIS_WIN_ADJ,0,8,count))
       != MAD_OK)
    {
      MAD_DBG_ERROR("Reading phy reg failed.\n");
      return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
  }
  else
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;


  return MAD_OK;
}

/*******************************************************************************
* mdSysGetLateCollision
*
* DESCRIPTION:
*       This function get Duplex Mismatch indicator: Late collision 97-128 bytes.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       count -    MAD_LATE_COLLISION_COUNTER:
*                count_65;
*                count_97;
*                count_129;
*                count_192;
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetLateCollision
(
    IN  MAD_DEV                        *dev,
    IN  MAD_LPORT                    port,
    OUT MAD_LATE_COLLISION_COUNTER  *count
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_U16      data;
  MAD_BOOL    mode;

  MAD_DBG_INFO("mdSysGetLateCollision called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR("Copper not supported.\n");
    return MAD_API_COPPER_NOT_SUPPORT;
  }

  /* Get the current duplex mode */
  if((retVal = mdGetDuplexStatus(dev,port,&mode)) != MAD_OK)
  {
    MAD_DBG_ERROR("mdGetDuplexStatus failed.\n");
    return (retVal | MAD_API_FAIL_GET_DUPLEX_ST);
  }

  if(mode)
  {
    MAD_DBG_INFO("Only Half Duplex mode is supported.\n");
    return MAD_API_NOT_HALF_DUPLEX;
  }

  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_DUPLEX_MISMATCH_INDC))
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
 

  if((retVal = madHwReadPagedPhyReg(
                    dev,hwPort, MAD_PAGE6, MAD_REG_LATE_COLIS_CNT_12,
                    &data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
    return (retVal | MAD_API_FAIL_READ_PAGED_REG);
  }
  count->count_65 = (0xff & data);
  count->count_97 = (0xff & ( data >> 8));

  if((retVal = madHwReadPagedPhyReg(
                    dev,hwPort, MAD_PAGE6, MAD_REG_LATE_COLIS_CNT_34,
                    &data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
    return (retVal | MAD_API_FAIL_READ_PAGED_REG);
  }
  count->count_129 = (0xff & data);
  count->count_192 = (0xff & ( data >> 8));

  return MAD_OK;
}


/*******************************************************************************
* mdSysSetLateCollision_winAdj
*
* DESCRIPTION:
*       This function set Late collision Window adjust.
*
* INPUTS:
*       portNum - Port number to be configured.
*       adj -    Number of bytes ro advance in late collision window.
*                0 start at 64 byte, 1 = start at 63rd byte... etc.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetLateCollision_winAdj
(
    IN  MAD_DEV        *dev,
    IN  MAD_LPORT    port,
    IN MAD_U8        adj
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_U16      data;
  MAD_BOOL    mode;

  MAD_DBG_INFO("mdSysSetLateCollision_winAdj called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR("Copper not supported.\n");
    return MAD_API_COPPER_NOT_SUPPORT;
  }

  /* Get the current duplex mode */
  if((retVal = mdGetDuplexStatus(dev,port,&mode)) != MAD_OK)
  {
    MAD_DBG_ERROR("mdGetDuplexStatus failed.\n");
    return (retVal | MAD_API_FAIL_GET_DUPLEX_ST);
  }

  if(mode)
  {
    MAD_DBG_INFO("Only Half Duplex mode is supported.\n");
    return MAD_API_NOT_HALF_DUPLEX;
  }

  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_DUPLEX_MISMATCH_INDC))
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
 
  data = adj;

  /* Set the Late collision window adjust */
  if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE6,MAD_REG_LATE_COLIS_WIN_ADJ,8,5,data))
     != MAD_OK)
  {
    MAD_DBG_ERROR("Writing phy reg failed.\n");
    return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }

  return MAD_OK;
}

/*******************************************************************************
* mdSysGetLateCollision_winAdj
*
* DESCRIPTION:
*       This function get Late collision Window adjust.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       adj -    Number of bytes ro advance in late collision window.
*                0 start at 64 byte, 1 = start at 63rd byte... etc.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetLateCollision_winAdj
(
    IN  MAD_DEV        *dev,
    IN  MAD_LPORT    port,
    OUT MAD_U8        *adj
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_U16      data;
  MAD_BOOL    mode;

  MAD_DBG_INFO("mdSysGetLateCollision_winAdj called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR("Copper not supported.\n");
    return MAD_API_COPPER_NOT_SUPPORT;
  }

  /* Get the current duplex mode */
  if((retVal = mdGetDuplexStatus(dev,port,&mode)) != MAD_OK)
  {
    MAD_DBG_ERROR("mdGetDuplexStatus failed.\n");
    return (retVal | MAD_API_FAIL_GET_DUPLEX_ST);
  }

  if(mode)
  {
    MAD_DBG_INFO("Only Half Duplex mode is supported.\n");
    return MAD_API_NOT_HALF_DUPLEX;
  }

  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_DUPLEX_MISMATCH_INDC))
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
 

  /* Get the Late collision window adjust */
  if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE6,MAD_REG_LATE_COLIS_WIN_ADJ,8,5, &data))
     != MAD_OK)
  {
    MAD_DBG_ERROR("Reading phy reg failed.\n");
    return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
  }
  *adj = (0x1f & data);

  return MAD_OK;
}

