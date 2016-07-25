#include <madCopyright.h>

/********************************************************************************
* madDrvConfig.h
*
* DESCRIPTION:
*       Includes driver level configuration and initialization function.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/

#include <madDrvPhyRegs.h>
#include <madDrvConfig.h>
#include <madSmiIf.h>
#include <madHwCntl.h>
#include <madApiDefs.h>
#include <madApiInternal.h>

/*******************************************************************************
* madDriverConfig
*
* DESCRIPTION:
*       This function finds Marvell Phy Device and saves the phy information to
*       the driver structure.
*
* INPUTS:
*        None.
* OUTPUTS:
*        None.
*
* RETURNS:
*       MAD_OK               - on success, or
*       MAD_FAIL             - otherwise.
*
* COMMENTS:
*       1.  This function should perform the following:
*           -   Find Marvell Phy Device.
*           -   Find Marvell Phy Device.
*
*******************************************************************************/
MAD_STATUS madDriverConfig
(
    IN MAD_DEV    *dev
)
{
    MAD_U16     deviceId;
    MAD_U16     id1, id2;
    MAD_STATUS  retVal;
    
    /* Set page as 0 */
    retVal = madSmiIfWriteRegister(dev,dev->baseRegAddr,MAD_REG_PAGE_ADDR,0);
    if (retVal != MAD_OK)
    {
        MAD_DBG_ERROR("SMI Write Failed\n");
        return MAD_DRV_FAIL_WRITE_REG;
    }

    retVal = madSmiIfReadRegister(dev,dev->baseRegAddr,MAD_REG_PHY_ID_1,&id1);
    if (retVal != MAD_OK)
    {
        MAD_DBG_ERROR("SMI Read Failed\n");
        return MAD_DRV_FAIL_READ_REG;
    }

    retVal = madSmiIfReadRegister(dev,dev->baseRegAddr,MAD_REG_PHY_ID_2,&id2);
    if (retVal != MAD_OK)
    {
        MAD_DBG_ERROR("SMI Read Failed\n");
        return MAD_DRV_FAIL_READ_REG;
    }
    if(!IS_MARVELL_OUI(id1,id2))
    {
        MAD_DBG_ERROR("Cannot find Marvell Device id1 %x id2 %x\n", id1, id2);
        return MAD_DRV_NO_DEVICE_FOUND;
    }

    deviceId = id2 & 0x3FF;
    
    /* Init the device's config struct. */
    dev->deviceId = deviceId >> 4;
    dev->revision = (MAD_U8)deviceId & 0xF; 


    return MAD_OK;
}



/*******************************************************************************
* madPagedAccessStart
*
* DESCRIPTION:
*       This function stores page register and Auto Reg Selection mode if needed.
*
* INPUTS:
*       hwPort   - port number where the Phy is connected
*       pageType - type of the page registers
*
* OUTPUTS:
*       autoOn  - MAD_TRUE if Auto Reg Selection enabled, MAD_FALSE otherwise.
*       pageReg - Page Register Data
*
* RETURNS:
*       MAD_OK   - if success
*       MAD_FAIL - othrwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madPagedAccessStart
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    IN  MAD_U8      pageType,
    OUT MAD_BOOL    *autoOn,
    OUT MAD_U16     *pageReg
)
{
    MAD_U16 data;
    MAD_STATUS status;

    switch(pageType)
    {
        case MAD_PHY_PAGE_WRITE_BACK:
            break;
        case MAD_PHY_PAGE_DIS_AUTO1:    /* 88E1111 Type */
            if((status= madHwGetPhyRegField(dev,hwPort,27,9,1,&data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Not able to read Phy Register.\n");
                return (status | MAD_DRV_NO_ABLE_READ_REG);
            }

            data ^= 0x1;    /* toggle bit 0 */
            MAD_BIT_2_BOOL(data, *autoOn);

            if (*autoOn) /* Auto On */
            {
                if((status= madHwSetPhyRegField(dev,hwPort,27,9,1,data)) != MAD_OK)
                {
                    MAD_DBG_ERROR("Not able to write Phy Register.\n");
                    return (status | MAD_DRV_NO_ABLE_WRITE_REG);
                }
            }

            break;    

        case MAD_PHY_PAGE_DIS_AUTO2:    /* 88E1112 Type */
            if((status= madHwGetPhyRegField(dev,hwPort,22,15,1,&data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Not able to read Phy Register.\n");
                return (status | MAD_DRV_NO_ABLE_READ_REG);
            }

            MAD_BIT_2_BOOL(data, *autoOn);
            data ^= 0x1;    /* toggle bit 0 */

            if (*autoOn) /* Auto On */
            {
                if((status= madHwSetPhyRegField(dev,hwPort,22,15,1,data)) != MAD_OK)
                {
                    MAD_DBG_ERROR("Not able to write Phy Register.\n");
                    return (status | MAD_DRV_NO_ABLE_WRITE_REG);
                }
            }

            break;    

        case MAD_PHY_NO_PAGE:
        default:
            /* Nothing to do */
            return MAD_OK;
    }

    if((status= madHwGetPhyRegField(dev,hwPort,22,0,8,pageReg)) != MAD_OK)
    {
        MAD_DBG_ERROR("Not able to read Phy Register.\n");
        return (status | MAD_DRV_NO_ABLE_READ_REG);
    }

    return MAD_OK;
}


/*******************************************************************************
* madPagedAccessStop
*
* DESCRIPTION:
*       This function restores page register and Auto Reg Selection mode if needed.
*
* INPUTS:
*       hwPort   - port number where the Phy is connected
*       pageType - type of the page registers
*       autoOn   - MAD_TRUE if Auto Reg Selection enabled, MAD_FALSE otherwise.
*       pageReg  - Page Register Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK     - if success
*       MAD_FAIL - othrwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madPagedAccessStop
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    IN  MAD_U8      pageType,
    IN  MAD_BOOL    autoOn,
    IN  MAD_U16     pageReg
)
{
    MAD_U16 data;
    MAD_STATUS status;

    switch(pageType)
    {
        case MAD_PHY_PAGE_WRITE_BACK:
            break;

        case MAD_PHY_PAGE_DIS_AUTO1:    /* 88E1111 Type */
            if (autoOn) /* Auto On */
            {
                data = 0;
                if((status= madHwSetPhyRegField(dev,hwPort,27,9,1,data)) != MAD_OK)
                {
                    MAD_DBG_ERROR("Not able to write Phy Register.\n");
                    return (status | MAD_DRV_NO_ABLE_WRITE_REG);
                }
            }
            break;    

        case MAD_PHY_PAGE_DIS_AUTO2:    /* 88E1112 Type */
            if (autoOn) /* Auto On */
            {
                data = 1;
                if((status= madHwSetPhyRegField(dev,hwPort,22,15,1,data)) != MAD_OK)
                {
                    MAD_DBG_ERROR(("Not able to write Phy Register.\n"));
                    return (status | MAD_DRV_NO_ABLE_WRITE_REG);
                }
            }
            break;    

        case MAD_PHY_NO_PAGE:
        default:
            /* Nothing to do */
            return MAD_OK;
    }


    if((status= madHwSetPhyRegField(dev,hwPort,22,0,8,pageReg)) != MAD_OK)
    {
        MAD_DBG_ERROR(("Not able to write Phy Register.\n"));
        return (status | MAD_DRV_NO_ABLE_WRITE_REG);
    }

    return MAD_OK;
}


/*
    HW Mode selection (Media and MAC Interface) with register 27_0.
    This function should be called after dev->phyInfo's pageType and anyPage
    are properly assigned.
*/
static
MAD_STATUS madHwMode_27_0
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    OUT MAD_U16     *hwMode
)
{
    MAD_U16     data,disSel;
    MAD_STATUS  retVal;

    /* Get the HW Mode bits */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,27,
                    0,4,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_DRV_NO_ABLE_READ_REG);
    }

    /* Get the auto selection bits */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,27,
                    9,1,&disSel)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_DRV_NO_ABLE_READ_REG);
    }

    /* analize HW Mode */
    switch (data)
    {
        case 0:
        case 4:
            *hwMode = MAD_PHY_MODE_COPPER|MAD_PHY_MODE_SGMII;
            break;        
        case 3:
            *hwMode = MAD_PHY_MODE_FIBER|MAD_PHY_MODE_RGMII;
            break;        
        case 6:
            *hwMode = MAD_PHY_MODE_RGMII|MAD_PHY_MODE_SGMII;
            break;        
        case 7:
            *hwMode = MAD_PHY_MODE_FIBER;
            break;        
        case 8:
        case 12:
            *hwMode = MAD_PHY_MODE_COPPER|MAD_PHY_MODE_FIBER;
            break;        
        case 9:
            *hwMode = MAD_PHY_MODE_COPPER;
            if (!disSel)
                *hwMode |= MAD_PHY_MODE_FIBER;
            break;        
        case 11:
            *hwMode = MAD_PHY_MODE_COPPER|MAD_PHY_MODE_RGMII;
            if (!disSel)
                *hwMode |= MAD_PHY_MODE_FIBER;
            break;        
        case 13:
            *hwMode = MAD_PHY_MODE_COPPER;
            if (!disSel)
                *hwMode |= MAD_PHY_MODE_FIBER;
            break;        
        case 14:
            *hwMode = MAD_PHY_MODE_SGMII;
            break;        
        case 15:
            *hwMode = MAD_PHY_MODE_COPPER;
            if (!disSel)
                *hwMode |= MAD_PHY_MODE_FIBER;
            break;        
        case 1:
        case 2:
        case 5:
        case 10:
        default:
            MAD_DBG_ERROR("Unknown HW Mode %#x.\n",data);
            return MAD_DRV_UNKNOWN_HW_MOD;
                
    }
    
    return MAD_OK;
}

/*
    HW Mode selection (Media and MAC Interface) with register 16_2
    This function should be called after dev->phyInfo's pageType and anyPage
    are properly assigned.
*/
static
MAD_STATUS madHwMode_16_2
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    OUT MAD_U16     *hwMode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;

    /* Get the HW Mode bits */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,2,16,
                    7,3,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_DRV_NO_ABLE_READ_REG);
    }

    /* analize HW Mode */
    switch (data)
    {
        case 0:
     /*       *hwMode = MAD_PHY_MODE_100FX; */
            *hwMode = MAD_PHY_MODE_100FX|MAD_PHY_MODE_COPPER; 
            break;        
        case 1:
/*            *hwMode = MAD_PHY_MODE_COPPER;  */
            *hwMode = MAD_PHY_MODE_COPPER|MAD_PHY_MODE_GBIC; 
            break;        
        case 2:
            *hwMode = MAD_PHY_MODE_COPPER|MAD_PHY_MODE_SGMII;
            break;        
        case 3:
/*            *hwMode = MAD_PHY_MODE_COPPER|MAD_PHY_MODE_FIBER;  */
            *hwMode = MAD_PHY_MODE_COPPER|MAD_PHY_MODE_FIBER|MAD_PHY_MODE_SGMII; 
            break;        
        case 5:
            *hwMode = MAD_PHY_MODE_COPPER;
            break;        
        case 6:
            *hwMode = MAD_PHY_MODE_SGMII;
            break;        
        case 7:
            *hwMode = MAD_PHY_MODE_FIBER;
            break;        
        case 4:
        default:
            MAD_DBG_ERROR("Unknown HW Mode %#x.\n",data);
            return MAD_DRV_UNKNOWN_HW_MOD;
                
    }
    
    return MAD_OK;
}
/*
    HW Mode selection (Media and MAC Interface) with register 20_6 (E1340).
    This function should be called after dev->phyInfo's pageType and anyPage
    are properly assigned.
*/

MAD_STATUS madGetHwMode
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    OUT MAD_U16     *hwMode
)
{
    MAD_STATUS  retVal;

    /* Get the HW Mode bits */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,6,20,
                    0,3,hwMode)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_DRV_NO_ABLE_READ_REG);
    }
    return MAD_OK;

}

static
MAD_STATUS madHwMode_20_6
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    OUT MAD_U16     *hwMode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;

    /* Get the HW Mode bits */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,6,20,
                    0,3,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_DRV_NO_ABLE_READ_REG);
    }

    /* analize HW Mode */
    switch (data)
    {
        case 0:
            *hwMode = MAD_PHY_MODE_COPPER|MAD_PHY_MODE_QSGMII;
            break;        
        case 1:
            *hwMode = MAD_PHY_MODE_COPPER|MAD_PHY_MODE_SGMII;
            break;        
        case 2:
        case 3:
            *hwMode = MAD_PHY_MODE_FIBER|MAD_PHY_MODE_QSGMII;
            break;        
        case 4:
            *hwMode = MAD_PHY_MODE_SGMII_MEDIA|MAD_PHY_MODE_QSGMII;
            break;        
        case 5:
            *hwMode = MAD_PHY_MODE_QSGMII_MEDIA|MAD_PHY_MODE_SGMII;
            break;        
        case 6:
            *hwMode = MAD_PHY_MODE_AUTO_DETECT_SC|MAD_PHY_MODE_QSGMII;
            break;        
        case 7:
            *hwMode = MAD_PHY_MODE_AUTO_DETECT_XC|MAD_PHY_MODE_QSGMII;
            break;        
        default:
            MAD_DBG_ERROR("Unknown HW Mode %#x.\n",data);
            return MAD_DRV_UNKNOWN_HW_MOD;
                
    }

    /* MAD_DBG_ERROR("!!!!!!!!!!!!!!!!!!!!! set config hw %x\n", *hwMode); */

    
    return MAD_OK;
}

/*******************************************************************************
* madFindPhyInformation
*
* DESCRIPTION:
*       This function gets feature set of the given PHY, such as VCT capability
*       adn register type, DTE capability and register type, Packet Generator
*       capability and register type, etc.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       phyInfo - Phy Feature Set
*
* RETURNS:
*       MAD_OK   - if the given is the known Marvell Phy,
*       MAD_FAIL - othrwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madFindPhyInformation
(
    IN  MAD_DEV     *dev,
    OUT MAD_PHY_INFO    *phyInfo
)
{
    MAD_U32 phyId,rev;
    MAD_U8  port;
    MAD_STATUS retVal;

    phyId = dev->deviceId;
    rev = dev->revision;
    phyInfo->featureSet1 = 0;  /* added extended features */

    switch (phyId)
    {
        case MAD_88E1011:   
            phyInfo->anyPage = 0xFFFFFFFF;
            phyInfo->featureSet = MAD_PHY_MAC_IF_LOOP | MAD_PHY_CRC_CHECKER|
                            MAD_PHY_EX_POWER_DOWN|MAD_PHY_DETECT_POWER_DOWN;
            
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE2;
            phyInfo->pktGenType = 5; /* Not support */
            phyInfo->hwMode[0] = MAD_PHY_MODE_COPPER | MAD_PHY_MODE_SGMII;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_0;

            break;
        case MAD_88E10X0:   
        case MAD_88E10X0S:   
            phyInfo->anyPage = 0xFFFFFFFF;
            phyInfo->featureSet = MAD_PHY_MAC_IF_LOOP | MAD_PHY_CRC_CHECKER;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_0;
            
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE2;
            phyInfo->pktGenType = 5; /* Not support */
            phyInfo->hwMode[0] = MAD_PHY_MODE_COPPER;

            break;
            
        case MAD_88E104X:   
            phyInfo->anyPage = 0xFFFFFFFF;
            phyInfo->featureSet = MAD_PHY_VCT_CAPABLE|MAD_PHY_DTE_CAPABLE|MAD_PHY_DW_SHIFT_CAPABLE|
                            MAD_PHY_EX_CABLE_STATUS|MAD_PHY_IEEE_TEST|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|MAD_PHY_EXTERNAL_LOOP|
                            MAD_PHY_RESTRICTED_PAGE|MAD_PHY_CRC_CHECKER|MAD_PHY_EX_POWER_DOWN;
            
            phyInfo->vctType = MAD_PHY_VCT_TYPE2; 
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE1;
            if (rev < 4)
                phyInfo->dteType = MAD_PHY_DTE_TYPE3;    /* Need workaround */
            else
                phyInfo->dteType = MAD_PHY_DTE_TYPE2;
            phyInfo->dwshiftType = MAD_PHY_DOWNSHIFT_TYPE1;      

            phyInfo->pktGenType = 5; /* Not support */
            phyInfo->crcCheckType = MAD_PHY_CRCCHECK_TYPE2;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE2;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE3;
            phyInfo->exLoopType = MAD_PHY_EX_LB_TYPE1;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE0;
            phyInfo->pageType = MAD_PHY_PAGE_DIS_AUTO1;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_2;

            for(port=0; port<dev->numOfPorts; port++)
            {
                if((retVal=madHwMode_27_0(dev,port,&phyInfo->hwMode[port])) != MAD_OK)
                  return retVal;
            }
            break;
            
        case MAD_88E1111:
            phyInfo->anyPage = 0xFFF1FE0C;
            phyInfo->featureSet = MAD_PHY_VCT_CAPABLE|MAD_PHY_DTE_CAPABLE|MAD_PHY_DW_SHIFT_CAPABLE|
                            MAD_PHY_EX_CABLE_STATUS|MAD_PHY_IEEE_TEST|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|MAD_PHY_EXTERNAL_LOOP|
                            MAD_PHY_RESTRICTED_PAGE|
                            MAD_PHY_PKT_GENERATOR|MAD_PHY_CRC_CHECKER|
                            MAD_PHY_EX_POWER_DOWN|MAD_PHY_DETECT_POWER_DOWN;

            phyInfo->vctType = MAD_PHY_VCT_TYPE2;
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE1;
            if (rev < 2)
                phyInfo->dteType = MAD_PHY_DTE_TYPE3;    /* Need workaround */
            else
                phyInfo->dteType = MAD_PHY_DTE_TYPE2;
            phyInfo->dwshiftType = MAD_PHY_DOWNSHIFT_TYPE1;      
            
            phyInfo->pktGenType = MAD_PHY_PKTGEN_TYPE1;
            phyInfo->crcCheckType = MAD_PHY_CRCCHECK_TYPE2;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE4;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE3;
            phyInfo->exLoopType = MAD_PHY_EX_LB_TYPE1;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE0;
            phyInfo->pageType = MAD_PHY_PAGE_DIS_AUTO1;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_2;

            for(port=0; port<dev->numOfPorts; port++)
            {
                if((retVal=madHwMode_27_0(dev,port,&phyInfo->hwMode[port])) != MAD_OK)
                    return retVal;
            }

            break;

        case MAD_88E1112:
            phyInfo->anyPage = 0x0400FE0C;
            phyInfo->featureSet = MAD_PHY_VCT_CAPABLE|MAD_PHY_DTE_CAPABLE|MAD_PHY_DW_SHIFT_CAPABLE|
                            MAD_PHY_EX_CABLE_STATUS|MAD_PHY_IEEE_TEST|
                            MAD_PHY_RESTRICTED_PAGE|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|MAD_PHY_EXTERNAL_LOOP|
                            MAD_PHY_PKT_GENERATOR|MAD_PHY_CRC_CHECKER|
                            MAD_PHY_EX_POWER_DOWN|MAD_PHY_DETECT_POWER_DOWN;
            
            phyInfo->vctType = MAD_PHY_VCT_TYPE4;
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE2;
            phyInfo->dwshiftType = MAD_PHY_DOWNSHIFT_TYPE2;      
            phyInfo->dteType = MAD_PHY_DTE_TYPE4;
            
            phyInfo->pktGenType = MAD_PHY_PKTGEN_TYPE2;
            phyInfo->crcCheckType = MAD_PHY_CRCCHECK_TYPE1;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE1;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE1;
            phyInfo->exLoopType = MAD_PHY_EX_LB_TYPE1;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE0;
            phyInfo->pageType = MAD_PHY_PAGE_DIS_AUTO2;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_0;

            for(port=0; port<dev->numOfPorts; port++)
            {
                if(madHwMode_16_2(dev,port,&phyInfo->hwMode[port]) != MAD_OK)
                    return MAD_FAIL;
            }
            break;
            
        case MAD_88E1116:
            phyInfo->anyPage = 0x2040FFFF;
            phyInfo->featureSet = MAD_PHY_DTE_CAPABLE|MAD_PHY_DW_SHIFT_CAPABLE|
                            MAD_PHY_EX_CABLE_STATUS|MAD_PHY_IEEE_TEST|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|MAD_PHY_EXTERNAL_LOOP|
                            MAD_PHY_PKT_GENERATOR|MAD_PHY_CRC_CHECKER|MAD_PHY_EX_POWER_DOWN |
                            MAD_PHY_DETECT_POWER_DOWN|MAD_PHY_ADV_VCT_CAPABLE;
            phyInfo->vctType = MAD_PHY_VCT_TYPE0;
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE2;
            phyInfo->dteType = MAD_PHY_DTE_TYPE4;
            phyInfo->dwshiftType = MAD_PHY_DOWNSHIFT_TYPE2;      
            phyInfo->pktGenType = MAD_PHY_PKTGEN_TYPE2;
            phyInfo->crcCheckType = MAD_PHY_CRCCHECK_TYPE1;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE2;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE2;
            phyInfo->exLoopType = MAD_PHY_EX_LB_TYPE2;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE0;
            phyInfo->pageType = MAD_PHY_PAGE_WRITE_BACK;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_2;

            for(port=0; port<dev->numOfPorts; port++)
            {
                phyInfo->hwMode[port] = MAD_PHY_MODE_COPPER | MAD_PHY_MODE_RGMII;
            }
            break;
            
        case MAD_88E3016:
/*
            phyInfo->anyPage = 0xFFFFFFFF;
            phyInfo->featureSet = MAD_PHY_30XX_MODE|MAD_PHY_VCT_CAPABLE|MAD_PHY_IEEE_TEST|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|
                            MAD_PHY_CRC_CHECKER|MAD_PHY_EX_POWER_DOWN|MAD_PHY_DETECT_POWER_DOWN;
            phyInfo->vctType = MAD_PHY_VCT_TYPE1;
            phyInfo->crcCheckType = MAD_PHY_CRCCHECK_TYPE3;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE7;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE5;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE0;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_0;

            for(port=0; port<dev->numOfPorts; port++)
            {
                phyInfo->hwMode[port] = MAD_PHY_MODE_COPPER | MAD_PHY_MODE_RGMII;
            }
            break;

        case MAD_88E3019:
*/
            phyInfo->anyPage = 0xFFFFFFFF;
            phyInfo->featureSet = MAD_PHY_30XX_MODE|MAD_PHY_VCT_CAPABLE|MAD_PHY_IEEE_TEST|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|
                            MAD_PHY_CRC_CHECKER|MAD_PHY_EX_POWER_DOWN|MAD_PHY_DETECT_POWER_DOWN;
            phyInfo->vctType = MAD_PHY_VCT_TYPE1;
            phyInfo->crcCheckType = MAD_PHY_CRCCHECK_TYPE3;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE7;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE5;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE0;
/*            phyInfo->pageType = MAD_PHY_PAGE_WRITE_BACK;  */
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_1;
            phyInfo->featureSet1 =  MAD_PHY_UNIDIRECT_TRABSMIT;

            for(port=0; port<dev->numOfPorts; port++)
            {
                phyInfo->hwMode[port] = MAD_PHY_MODE_COPPER | MAD_PHY_MODE_RGMII;
            }
            break;
            
        case MAD_88E3082:
            phyInfo->anyPage = 0xFFFFFFFF;
            phyInfo->featureSet = MAD_PHY_30XX_MODE|MAD_PHY_VCT_CAPABLE|MAD_PHY_IEEE_TEST|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|
                            MAD_PHY_EX_POWER_DOWN|MAD_PHY_DETECT_POWER_DOWN;
            phyInfo->vctType = MAD_PHY_VCT_TYPE1;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE7;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE5;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE0;
            if ((rev) < 9)
              phyInfo->dteType = MAD_PHY_DTE_TYPE1;    /* need workaround */
            else
              phyInfo->dteType = MAD_PHY_DTE_TYPE5;
            phyInfo->dwshiftType = MAD_PHY_DOWNSHIFT_TYPE2;      
/*            phyInfo->pageType = MAD_PHY_PAGE_WRITE_BACK;  */
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_1;

            for(port=0; port<dev->numOfPorts; port++)
            {
                phyInfo->hwMode[port] = MAD_PHY_MODE_COPPER | MAD_PHY_MODE_RGMII;
            }
            break;
            
        case MAD_88E114X:
            phyInfo->anyPage = 0x2FF1FE0C;
            phyInfo->featureSet = MAD_PHY_VCT_CAPABLE|MAD_PHY_DTE_CAPABLE|MAD_PHY_DW_SHIFT_CAPABLE|
                            MAD_PHY_EX_CABLE_STATUS|MAD_PHY_IEEE_TEST|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|MAD_PHY_EXTERNAL_LOOP|
                            MAD_PHY_PKT_GENERATOR|MAD_PHY_RESTRICTED_PAGE|
                            MAD_PHY_CRC_CHECKER;
            
            phyInfo->vctType = MAD_PHY_VCT_TYPE2;
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE1;
            if (rev < 4)
                phyInfo->dteType = MAD_PHY_DTE_TYPE3;    /* Need workaround */
            else
                phyInfo->dteType = MAD_PHY_DTE_TYPE2;
            phyInfo->dwshiftType = MAD_PHY_DOWNSHIFT_TYPE1;      
            
            phyInfo->pktGenType = MAD_PHY_PKTGEN_TYPE1;
            phyInfo->crcCheckType = MAD_PHY_CRCCHECK_TYPE2;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE4;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE3;
            phyInfo->exLoopType = MAD_PHY_EX_LB_TYPE1;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE3;
            phyInfo->pageType = MAD_PHY_PAGE_DIS_AUTO1;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_2;

            for(port=0; port<dev->numOfPorts; port++)
            {
                if((retVal=madHwMode_27_0(dev,port,&phyInfo->hwMode[port])) != MAD_OK)
                    return retVal;
            }
            break;
            
        case MAD_88E1121:
            phyInfo->anyPage = 0x2040FFFF;
            phyInfo->featureSet = MAD_PHY_DTE_CAPABLE|MAD_PHY_DW_SHIFT_CAPABLE|
                            MAD_PHY_EX_CABLE_STATUS|MAD_PHY_IEEE_TEST|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|MAD_PHY_EXTERNAL_LOOP|
                            MAD_PHY_PKT_GENERATOR|MAD_PHY_CRC_CHECKER|MAD_PHY_EX_POWER_DOWN|
                            MAD_PHY_DETECT_POWER_DOWN;
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE2; 
            phyInfo->dteType = MAD_PHY_DTE_TYPE4;
            phyInfo->dwshiftType = MAD_PHY_DOWNSHIFT_TYPE2;      
            phyInfo->pktGenType = MAD_PHY_PKTGEN_TYPE2;
            phyInfo->crcCheckType = MAD_PHY_CRCCHECK_TYPE1;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE2;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE2;
            phyInfo->exLoopType = MAD_PHY_EX_LB_TYPE2;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE0;
            phyInfo->pageType = MAD_PHY_PAGE_WRITE_BACK;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_2;

            if (dev->revision > 1)
            {
              phyInfo->vctType = MAD_PHY_VCT_TYPE0;
              phyInfo->featureSet |= MAD_PHY_ADV_VCT_CAPABLE;
            }
            else
            {
              phyInfo->featureSet |= MAD_PHY_VCT_CAPABLE;
              phyInfo->vctType = MAD_PHY_VCT_TYPE4;
            }

            for(port=0; port<dev->numOfPorts; port++)
            {
                phyInfo->hwMode[port] = MAD_PHY_MODE_COPPER | MAD_PHY_MODE_RGMII;
            }
            break;
                    
        case MAD_88E1149:
            phyInfo->anyPage = 0x2040FFFF;
            phyInfo->featureSet = MAD_PHY_DTE_CAPABLE|MAD_PHY_DW_SHIFT_CAPABLE|MAD_PHY_IEEE_TEST|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|MAD_PHY_EXTERNAL_LOOP|
                            MAD_PHY_PKT_GENERATOR|MAD_PHY_CRC_CHECKER|MAD_PHY_EX_POWER_DOWN|
                            MAD_PHY_EX_CABLE_STATUS|MAD_PHY_DETECT_POWER_DOWN;
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE2; 
            phyInfo->dteType = MAD_PHY_DTE_TYPE4;
            phyInfo->dwshiftType = MAD_PHY_DOWNSHIFT_TYPE2;      
            phyInfo->pktGenType = MAD_PHY_PKTGEN_TYPE2;
            phyInfo->crcCheckType = MAD_PHY_CRCCHECK_TYPE1;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE1;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE2;
            phyInfo->exLoopType = MAD_PHY_EX_LB_TYPE2;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE2;
            phyInfo->pageType = MAD_PHY_PAGE_WRITE_BACK;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_2;


            for(port=0; port<dev->numOfPorts; port++)
            {
                phyInfo->hwMode[port] = MAD_PHY_MODE_COPPER | MAD_PHY_MODE_SGMII;
            }
#ifdef MAD_DSP_VCT
            phyInfo->featureSet |= MAD_PHY_DSP_VCT;
            phyInfo->dspVctType = MAD_PHY_DSP_VCT_TYPE1;
#endif
            break;
            
        case MAD_88E1149R:
        case MAD_SWG65G : 
        case MAD_88E1240:
            phyInfo->anyPage = 0x2040FFFF;
            phyInfo->featureSet = MAD_PHY_DTE_CAPABLE|MAD_PHY_DW_SHIFT_CAPABLE|MAD_PHY_IEEE_TEST|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|MAD_PHY_EXTERNAL_LOOP|
                            MAD_PHY_PKT_GENERATOR|MAD_PHY_CRC_CHECKER|MAD_PHY_EX_POWER_DOWN|
                            MAD_PHY_EX_CABLE_STATUS|MAD_PHY_DETECT_POWER_DOWN;
            phyInfo->featureSet |= MAD_PHY_ADV_VCT_CAPABLE;
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE3; 
            phyInfo->dteType = MAD_PHY_DTE_TYPE4;
            phyInfo->dwshiftType = MAD_PHY_DOWNSHIFT_TYPE2;      
            phyInfo->pktGenType = MAD_PHY_PKTGEN_TYPE2;
            phyInfo->crcCheckType = MAD_PHY_CRCCHECK_TYPE1;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE2;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE2;
            phyInfo->exLoopType = MAD_PHY_EX_LB_TYPE2;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE0;
            phyInfo->pageType = MAD_PHY_PAGE_WRITE_BACK;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_2;

            if ((dev->deviceId == MAD_88E1149R)||(dev->deviceId == MAD_SWG65G))
            {
              phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE4;
            }

            for(port=0; port<dev->numOfPorts; port++)
            {
                phyInfo->hwMode[port] = MAD_PHY_MODE_COPPER | MAD_PHY_MODE_SGMII;
            }
#ifdef MAD_DSP_VCT
            phyInfo->featureSet |= MAD_PHY_DSP_VCT;
            phyInfo->dspVctType = MAD_PHY_DSP_VCT_TYPE1;
#endif
            break;
            
        case MAD_88E1340S:
        case MAD_88E1340:
        case MAD_88E1340M:
        case MAD_88E1540:
            phyInfo->anyPage = 0;
            phyInfo->featureSet = MAD_PHY_DTE_CAPABLE|MAD_PHY_DW_SHIFT_CAPABLE|MAD_PHY_IEEE_TEST|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|MAD_PHY_EXTERNAL_LOOP|
                            MAD_PHY_PKT_GENERATOR|MAD_PHY_CRC_CHECKER|MAD_PHY_SYNCRO_FIFO|
                            MAD_PHY_EX_POWER_DOWN|
                            MAD_PHY_EX_CABLE_STATUS|MAD_PHY_DETECT_POWER_DOWN;
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE4; 
            phyInfo->dteType = MAD_PHY_DTE_TYPE4;
            phyInfo->dwshiftType = MAD_PHY_DOWNSHIFT_TYPE2;      
            phyInfo->pktGenType = MAD_PHY_PKTGEN_TYPE4;
            phyInfo->crcCheckType = MAD_PHY_CRCCHECK_TYPE4;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE8;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE6;
            phyInfo->exLoopType = MAD_PHY_EX_LB_TYPE2;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE4;
            phyInfo->pageType = MAD_PHY_PAGE_WRITE_BACK;

            phyInfo->vctType = MAD_PHY_VCT_TYPE0;
            phyInfo->featureSet |= MAD_PHY_ADV_VCT_CAPABLE;
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE3;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_2;
            phyInfo->featureSet |= MAD_PHY_LED_CONTROL;
            phyInfo->featureSet1 = MAD_PHY_SNOOPING_CONTROL | MAD_PHY_SYNC_FIFO                
                                 | MAD_PHY_1G_LINK_DOWN_INDICAT    | MAD_PHY_RX_ER_BYTE_CAPTURE        
                                 | MAD_PHY_QSGMII_PRBS_GEN | MAD_PHY_125G_PRBS_GEN            
                                 | MAD_PHY_UNIDIRECT_TRABSMIT | MAD_PHY_FLP_EXCH_COMP_NO_LNK    
                                 | MAD_PHY_DUPLEX_MISMATCH_INDC    | MAD_PHY_LINK_DISCONNECT_INDC    
                                 | MAD_PHY_ALT_VCT_CAPABLE;
            phyInfo->featureSet1 |= MAD_PHY_MACSEC_CAPABLE;    
/*            if (phyId==MAD_88E1340S) */
              phyInfo->featureSet1 |= MAD_PHY_PTP_TAI_CAPABLE;                
              phyInfo->featureSet1 |= MAD_PHY_SYNC_ETHNET;

            for(port=0; port<dev->numOfPorts; port++)
            {
                if((retVal=madHwMode_20_6(dev,port,&phyInfo->hwMode[port])) != MAD_OK)
                  return retVal;
            }
            phyInfo->featureSet |= MAD_PHY_FULL_PAGE_ADDR;
           break;

        case MAD_88E1119R:
            phyInfo->anyPage = 0x2040FFFF;
            phyInfo->featureSet = MAD_PHY_DTE_CAPABLE|MAD_PHY_DW_SHIFT_CAPABLE|MAD_PHY_IEEE_TEST|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|MAD_PHY_EXTERNAL_LOOP|
                            MAD_PHY_PKT_GENERATOR|MAD_PHY_CRC_CHECKER|MAD_PHY_SYNCRO_FIFO|
                            MAD_PHY_EX_POWER_DOWN|
                            MAD_PHY_EX_CABLE_STATUS|MAD_PHY_DETECT_POWER_DOWN;
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE4; 
            phyInfo->dteType = MAD_PHY_DTE_TYPE4;
            phyInfo->dwshiftType = MAD_PHY_DOWNSHIFT_TYPE2;      
            phyInfo->pktGenType = MAD_PHY_PKTGEN_TYPE4;
            phyInfo->crcCheckType = MAD_PHY_CRCCHECK_TYPE1;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE2;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE2;
            phyInfo->exLoopType = MAD_PHY_EX_LB_TYPE2;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE0;
             phyInfo->pageType = MAD_PHY_PAGE_WRITE_BACK;

            phyInfo->vctType = MAD_PHY_VCT_TYPE0;
            phyInfo->featureSet |= MAD_PHY_ADV_VCT_CAPABLE;
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE0;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_2;
            phyInfo->featureSet |= MAD_PHY_LED_CONTROL;
            phyInfo->featureSet1 =  MAD_PHY_UNIDIRECT_TRABSMIT | MAD_PHY_FLP_EXCH_COMP_NO_LNK    
                                     | MAD_PHY_LINK_DISCONNECT_INDC;

            for(port=0; port<dev->numOfPorts; port++)
            {
                phyInfo->hwMode[port] = MAD_PHY_MODE_COPPER | MAD_PHY_MODE_GMII;
            }
           break;

        case MAD_88E1310:
            phyInfo->anyPage = 0x2040FFFF;
            phyInfo->featureSet = MAD_PHY_DTE_CAPABLE|MAD_PHY_DW_SHIFT_CAPABLE|MAD_PHY_IEEE_TEST|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|MAD_PHY_EXTERNAL_LOOP|
                            MAD_PHY_PKT_GENERATOR|MAD_PHY_CRC_CHECKER|MAD_PHY_SYNCRO_FIFO|
                            MAD_PHY_EX_POWER_DOWN|
                            MAD_PHY_EX_CABLE_STATUS|MAD_PHY_DETECT_POWER_DOWN;
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE4; 
            phyInfo->dteType = MAD_PHY_DTE_TYPE4;
            phyInfo->dwshiftType = MAD_PHY_DOWNSHIFT_TYPE2;      
            phyInfo->pktGenType = MAD_PHY_PKTGEN_TYPE4;
            phyInfo->crcCheckType = MAD_PHY_CRCCHECK_TYPE1;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE2;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE2;
            phyInfo->exLoopType = MAD_PHY_EX_LB_TYPE2;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE0;
             phyInfo->pageType = MAD_PHY_PAGE_WRITE_BACK;

            phyInfo->vctType = MAD_PHY_VCT_TYPE0;
            phyInfo->featureSet |= MAD_PHY_ADV_VCT_CAPABLE;
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE0;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_2;
            phyInfo->featureSet |= MAD_PHY_LED_CONTROL;
            phyInfo->featureSet1 =  MAD_PHY_UNIDIRECT_TRABSMIT | MAD_PHY_FLP_EXCH_COMP_NO_LNK    
                                     | MAD_PHY_LINK_DISCONNECT_INDC;

            for(port=0; port<dev->numOfPorts; port++)
            {
                phyInfo->hwMode[port] = MAD_PHY_MODE_COPPER | MAD_PHY_MODE_RGMII;
            }
           break;

        case MAD_88E1181:
            phyInfo->anyPage = 0x2040FFFF;
            phyInfo->featureSet = MAD_PHY_VCT_CAPABLE|MAD_PHY_DTE_CAPABLE|MAD_PHY_DW_SHIFT_CAPABLE|
                            MAD_PHY_EX_CABLE_STATUS|MAD_PHY_IEEE_TEST|
                            MAD_PHY_MAC_IF_LOOP|MAD_PHY_LINE_LOOP|MAD_PHY_EXTERNAL_LOOP|
                            MAD_PHY_PKT_GENERATOR|MAD_PHY_CRC_CHECKER|MAD_PHY_EX_POWER_DOWN|
                            MAD_PHY_DETECT_POWER_DOWN|MAD_PHY_VOD_ADJUST;
            phyInfo->vctType = MAD_PHY_VCT_TYPE4;
            phyInfo->exStatusType = MAD_PHY_EX_STATUS_TYPE2;
            phyInfo->dteType = MAD_PHY_DTE_TYPE4;
            phyInfo->dwshiftType = MAD_PHY_DOWNSHIFT_TYPE2;      
            phyInfo->pktGenType = MAD_PHY_PKTGEN_TYPE2;
            phyInfo->crcCheckType = MAD_PHY_CRCCHECK_TYPE1;
            phyInfo->macIfLoopType = MAD_PHY_LOOPBACK_TYPE5;
            phyInfo->lineLoopType = MAD_PHY_LINE_LB_TYPE2;
            phyInfo->exLoopType = MAD_PHY_EX_LB_TYPE2;
            phyInfo->ieeeTestType = MAD_PHY_IEEE_TEST_TYPE1;
            phyInfo->pageType = MAD_PHY_PAGE_WRITE_BACK;
            phyInfo->interruptType = MAD_PHY_INTERRUPT_TYPE_2;

            if (dev->revision > 3)
            {
                phyInfo->featureSet |= MAD_PHY_ADV_VCT_CAPABLE;
            }

            for(port=0; port<dev->numOfPorts; port++)
            {
                phyInfo->hwMode[port] = MAD_PHY_MODE_COPPER | MAD_PHY_MODE_SGMII;
            }
            break;
            
        default:
            return MAD_DRV_ERROR_DEV_ID;
    }

    for(port=0; port<dev->numOfPorts; port++)
    {
      phyInfo->hwRealMode[port] = phyInfo->hwMode[port];
    }

                
    return MAD_OK;
}


