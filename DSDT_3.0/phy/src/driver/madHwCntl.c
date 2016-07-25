#include <madCopyright.h>
/********************************************************************************
* madHwCntl.c
*
* DESCRIPTION:
*       Function declarations for Hw accessing phy registers.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <madHwCntl.h>
#include <madApiInternal.h>
#include <madSmiIf.h>
#include <madSem.h>


/****************************************************************************/
/* Phy register access functions.                                         */
/****************************************************************************/

/*******************************************************************************
* madHwReadPhyReg
*
* DESCRIPTION:
*       This function reads a Marvell phy register.
*
* INPUTS:
*       portNum - Port number to read the register for.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madHwReadPhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U8     regAddr,
    OUT MAD_U16    *data
)
{
    MAD_U8       phyAddr;
    MAD_STATUS   retVal;

    if(portNum >= dev->numOfPorts)
    {
        MAD_DBG_ERROR("Invalid port Number(%i)\n",portNum);
        return MAD_DRV_INVALID_PORT;
    }

    phyAddr = MAD_CALC_SMI_DEV_ADDR(dev, portNum);
    if (phyAddr > 0x1F)
    {
        MAD_DBG_ERROR("Failed to get phy Address (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_GET_PHY_ADDR;
    }

    madSemTake(dev,dev->multiAddrSem,OS_WAIT_FOREVER);

    retVal = madSmiIfReadRegister(dev,phyAddr,regAddr,data);

    madSemGive(dev,dev->multiAddrSem);

    if(retVal != MAD_OK)
    {
        MAD_DBG_ERROR("Failed to read register (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_READ_REG;
    }

    MAD_DBG_INFO("Read from port(%d): smiAddr %d, regAddr %d\n",
                    portNum,phyAddr,regAddr);

    return MAD_OK;

}


/*******************************************************************************
* madHwWritePhyReg
*
* DESCRIPTION:
*       This function writes to a switch's port phy register.
*
* INPUTS:
*       portNum - Port number to write the register for.
*       regAddr - The register's address.
*       data    - The data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madHwWritePhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U8     regAddr,
    IN  MAD_U16    data
)
{
    MAD_U8       phyAddr;
    MAD_STATUS   retVal;

    if(portNum >= dev->numOfPorts)
    {
        MAD_DBG_ERROR("Invalid port Number(%i)\n",portNum);
        return MAD_DRV_INVALID_PORT;
    }

    phyAddr = MAD_CALC_SMI_DEV_ADDR(dev, portNum);
    if (phyAddr > 0x1F)
    {
        MAD_DBG_ERROR("Failed to get phy Address (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_GET_PHY_ADDR;
    }

    MAD_DBG_INFO("Write to port(%d): smiAddr %d, regAddr %d, ",
                portNum,phyAddr,regAddr);
    MAD_DBG_INFO("data 0x%x.\n",data);

    madSemTake(dev,dev->multiAddrSem,OS_WAIT_FOREVER);

    retVal = madSmiIfWriteRegister(dev,phyAddr,regAddr,data);

    madSemGive(dev,dev->multiAddrSem);

    if(retVal != MAD_OK)
    {
        MAD_DBG_ERROR("Failed to write register (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_WRITE_REG;
    }
    
    return retVal;
}


/*******************************************************************************
* madHwGetPhyRegField
*
* DESCRIPTION:
*       This function reads a specified field from a switch's port phy register.
*
* INPUTS:
*       portNum     - Port number to read the register for.
*       regAddr     - The register's address.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to read.
*
* OUTPUTS:
*       data        - The read register field.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       1.  The sum of fieldOffset & fieldLength parameters must be smaller-
*           equal to 16.
*
*******************************************************************************/
MAD_STATUS madHwGetPhyRegField
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U8     regAddr,
    IN  MAD_U8     fieldOffset,
    IN  MAD_U8     fieldLength,
    OUT MAD_U16    *data
)
{
    MAD_U16 mask;            /* Bits mask to be read */
    MAD_U16 tmpData;
    MAD_STATUS   retVal;
    MAD_U8       phyAddr;

    if(portNum >= dev->numOfPorts)
    {
        MAD_DBG_ERROR("Invalid port Number(%i)\n",portNum);
        return MAD_DRV_INVALID_PORT;
    }

    phyAddr = MAD_CALC_SMI_DEV_ADDR(dev, portNum);
    if (phyAddr > 0x1F)
    {
        MAD_DBG_ERROR("Failed to get phy Address (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_GET_PHY_ADDR;
    }

    madSemTake(dev,dev->multiAddrSem,OS_WAIT_FOREVER);

    retVal = madSmiIfReadRegister(dev,phyAddr,regAddr,&tmpData);

    madSemGive(dev,dev->multiAddrSem);

    if(retVal != MAD_OK)
    {
        MAD_DBG_ERROR("Failed to read register (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_READ_REG;
    }
    
    MAD_CALC_MASK(fieldOffset,fieldLength,mask);

    tmpData = (tmpData & mask) >> fieldOffset;
    *data = tmpData;

    MAD_DBG_INFO("fOff %d, fLen %d, data 0x%x.\n",fieldOffset,fieldLength,*data);

    return MAD_OK;
}


/*******************************************************************************
* madHwSetPhyRegField
*
* DESCRIPTION:
*       This function writes to specified field in a switch's port phy register.
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       regAddr     - The register's address.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*       data        - Data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       1.  The sum of fieldOffset & fieldLength parameters must be smaller-
*           equal to 16.
*
*******************************************************************************/
MAD_STATUS madHwSetPhyRegField
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U8     regAddr,
    IN  MAD_U8     fieldOffset,
    IN  MAD_U8     fieldLength,
    IN  MAD_U16    data
)
{
    MAD_U16 mask;
    MAD_U16 tmpData;
    MAD_STATUS   retVal;
    MAD_U8       phyAddr;

    if(portNum >= dev->numOfPorts)
    {
        MAD_DBG_ERROR("Invalid port Number(%i)\n",portNum);
        return MAD_DRV_INVALID_PORT;
    }

    phyAddr = MAD_CALC_SMI_DEV_ADDR(dev, portNum);
    if (phyAddr > 0x1F)
    {
        MAD_DBG_ERROR("Failed to get phy Address (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_GET_PHY_ADDR;
    }

    madSemTake(dev,dev->multiAddrSem,OS_WAIT_FOREVER);

    retVal = madSmiIfReadRegister(dev,phyAddr,regAddr,&tmpData);
    if(retVal != MAD_OK)
    {
        madSemGive(dev,dev->multiAddrSem);

        MAD_DBG_ERROR("Failed to read register (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_READ_REG;
    }

    MAD_CALC_MASK(fieldOffset,fieldLength,mask);

    /* Set the desired bits to 0.                       */
    tmpData &= ~mask;
    /* Set the given data into the above reset bits.    */
    tmpData |= ((data << fieldOffset) & mask);

    retVal = madSmiIfWriteRegister(dev,phyAddr,regAddr,tmpData);

    madSemGive(dev,dev->multiAddrSem);

    if(retVal != MAD_OK)
    {
        MAD_DBG_ERROR("Failed to write register (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_WRITE_REG;
    }

    MAD_DBG_INFO("fieldOff %d, fieldLen %d, data 0x%x.\n",fieldOffset,
                  fieldLength,data);

    return MAD_OK;
}


/****************************************************************************/
/* Page Mode access                                                            */
/****************************************************************************/

/*******************************************************************************
* pageAccessStart
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
static MAD_STATUS pageAccessStart
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      phyAddr,
    IN  MAD_U16     pageNum,
    IN  MAD_U8      regAddr,
    OUT MAD_BOOL    *autoOn,
    OUT MAD_U16     *pageReg
)
{
    MAD_U16     data;
    MAD_U8      pageAddr;
    MAD_STATUS  retVal;

    /* if (((dev->deviceId==MAD_88E3016)||(dev->deviceId==MAD_88E3082))&&(regAddr==22))  */
    if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE)&&(regAddr==22))
        return MAD_OK;

    pageAddr = MAD_GET_PAGE_ADDR(dev, regAddr);
    if (pageAddr > 0x1F)
    {
        MAD_DBG_ERROR("Failed to get page Address (regAddr %i)\n",regAddr);
        return MAD_DRV_FAIL_GET_PAGE_ADDR;
    }

    *autoOn = MAD_FALSE;

    /* if page address is 22, then check and disable auto selection */
    if (pageAddr == 22)
    {
        switch(dev->phyInfo.pageType)
        {
            case MAD_PHY_PAGE_WRITE_BACK:
                break;

            case MAD_PHY_PAGE_DIS_AUTO1:    /* 88E1111 Type */
                retVal = madSmiIfReadRegister(dev,phyAddr,27,&data);
                if(retVal != MAD_OK)
                {
                    MAD_DBG_ERROR("Reading phy register (27) failed\n");
                    return MAD_DRV_FAIL_READ_REG;
                }

                *autoOn = (data & MAD_BIT_9)?MAD_FALSE:MAD_TRUE;

                if (*autoOn) /* Auto On */
                {
                    data ^= MAD_BIT_9;    /* toggle bit 9 */
                    retVal = madSmiIfWriteRegister(dev,phyAddr,27,data);
                    if(retVal != MAD_OK)
                    {
                        MAD_DBG_ERROR("Writing Page Number(%i) failed\n",pageNum);
                        return MAD_DRV_FAIL_WRITE_REG;
                    }
                }

                break;    

            case MAD_PHY_PAGE_DIS_AUTO2:    /* 88E1112 Type */
                retVal = madSmiIfReadRegister(dev,phyAddr,22,&data);
                if(retVal != MAD_OK)
                {
                    MAD_DBG_ERROR("Reading phy register (22) failed\n");
                    return MAD_DRV_FAIL_READ_REG;
                }

 /*               *autoOn = (data & MAD_BIT_9)?MAD_TRUE:MAD_FALSE; */ 
                *autoOn = (data & MAD_BIT_15)?MAD_TRUE:MAD_FALSE;

                if (*autoOn) /* Auto On */
                {
                    data ^= MAD_BIT_15;    /* toggle bit 15 */
                    retVal = madSmiIfWriteRegister(dev,phyAddr,22,data);
                    if(retVal != MAD_OK)
                    {
                        MAD_DBG_ERROR("Writing Page Number(%i) failed\n",pageNum);
                        return MAD_DRV_FAIL_WRITE_PAGE_NUM;
                    }
                }

                break;    

        }
    }
    retVal = madSmiIfReadRegister(dev,phyAddr,pageAddr,pageReg);
    if(retVal != MAD_OK)
    {
        MAD_DBG_ERROR("Reading phy register (%#x) failed\n",pageAddr);
        return MAD_DRV_FAIL_READ_REG;
    }


    /* if ((!(dev->deviceId==MAD_88E3016))&&(!(dev->deviceId==MAD_88E3082))) */
    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
      pageNum = MAD_UPDATE_PAGE_NUM(pageAddr, *pageReg) | pageNum;

    retVal = madSmiIfWriteRegister(dev,phyAddr,pageAddr,pageNum);
    if(retVal != MAD_OK)
    {
        MAD_DBG_ERROR("Writing Page Number(%i) failed\n",pageNum);
        return MAD_DRV_FAIL_WRITE_PAGE_NUM;
    }

    return MAD_OK;
}



/*******************************************************************************
* pageAccessStop
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
static MAD_STATUS pageAccessStop
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      phyAddr,
    IN  MAD_U16     pageNum,
    IN  MAD_U8      regAddr,
    IN  MAD_BOOL    autoOn,
    IN  MAD_U16     pageReg
)
{
    MAD_U8      pageAddr;
    MAD_U16     data;
    MAD_STATUS  retVal;

    /* if ((dev->deviceId==MAD_88E3016)||(dev->deviceId==MAD_88E3082)) */
    if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
        return MAD_OK;


    pageAddr = MAD_GET_PAGE_ADDR(dev, regAddr);
    if (pageAddr > 0x1F)
    {
        MAD_DBG_ERROR("Failed to get page Address (regAddr %i)\n",regAddr);
        return MAD_DRV_FAIL_GET_PAGE_ADDR;
    }

    retVal = madSmiIfWriteRegister(dev,phyAddr,pageAddr,pageReg);
    if(retVal != MAD_OK)
    {
        MAD_DBG_ERROR("Writing Page Number(%i) failed\n",pageNum);
        return MAD_DRV_FAIL_WRITE_PAGE_NUM;
    }

    if (autoOn == MAD_FALSE)
    {
        return MAD_OK;
    }

    switch(dev->phyInfo.pageType)
    {
        case MAD_PHY_PAGE_DIS_AUTO1:    /* 88E1111 Type */
            retVal = madSmiIfReadRegister(dev,phyAddr,27,&data);
            if(retVal != MAD_OK)
            {
                MAD_DBG_ERROR("Reading phy register (27) failed\n");
                return MAD_DRV_FAIL_READ_REG;
            }

            data &= ~MAD_BIT_9;    /* clear bit 9 */

            retVal = madSmiIfWriteRegister(dev,phyAddr,27,data);
            if(retVal != MAD_OK)
            {
                MAD_DBG_ERROR("Writing Page Number(%i) failed\n",pageNum);
                return MAD_DRV_FAIL_WRITE_PAGE_NUM;
            }
            break;    

        case MAD_PHY_PAGE_DIS_AUTO2:    /* 88E1112 Type */
            retVal = madSmiIfReadRegister(dev,phyAddr,22,&data);
            if(retVal != MAD_OK)
            {
                MAD_DBG_ERROR("Reading phy register (22) failed\n");
                return MAD_DRV_FAIL_READ_REG;
            }

            data |= MAD_BIT_15;    /* set bit 15 */
            retVal = madSmiIfWriteRegister(dev,phyAddr,22,data);
            if(retVal != MAD_OK)
            {
                MAD_DBG_ERROR("Writing Page Number(%i) failed\n",pageNum);
                return MAD_DRV_FAIL_WRITE_PAGE_NUM;
            }
            break;    

        default:
            break;
    }

    return MAD_OK;
}


/*******************************************************************************
* madHwReadPagedPhyReg
*
* DESCRIPTION:
*       This function reads a phy register in paged mode.
*
* INPUTS:
*       portNum - Port number to read the register for.
*       pageNum - Page number to be accessed.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madHwReadPagedPhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U16    pageNum,
    IN  MAD_U8     regAddr,
    OUT MAD_U16    *data
)
{
    MAD_U8       phyAddr;
    MAD_STATUS   retVal;
    MAD_BOOL     anyPage;
    MAD_BOOL     autoOn;
    MAD_U16      pageReg;

    if(portNum >= dev->numOfPorts)
    {
        MAD_DBG_ERROR("Invalid port Number(%i)\n",portNum);
        return MAD_DRV_INVALID_PORT;
    }

    if(MAD_IS_ANY_PAGE(dev,regAddr))
        anyPage = MAD_TRUE;
    else
        anyPage = MAD_FALSE;


    phyAddr = MAD_CALC_SMI_DEV_ADDR(dev, portNum);
    if (phyAddr > 0x1F)
    {
        MAD_DBG_ERROR("Failed to get phy Address (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_GET_PHY_ADDR;
    }


    madSemTake(dev,dev->multiAddrSem,OS_WAIT_FOREVER);

    if (!anyPage)
    {
        retVal = pageAccessStart(dev,phyAddr,pageNum,regAddr,&autoOn,&pageReg);
        if(retVal != MAD_OK)
        {
            madSemGive(dev,dev->multiAddrSem);

            MAD_DBG_ERROR("Accessing Page Register failed\n");
            return (retVal | MAD_DRV_FAIL_ACCESS_PAGE_REG);
        }
    }

    retVal = madSmiIfReadRegister(dev,phyAddr,regAddr,data);
    if(retVal != MAD_OK)
    {
        madSemGive(dev,dev->multiAddrSem);

        MAD_DBG_ERROR("Failed to read register (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_READ_REG;
    }

    if (!anyPage)
    {
        retVal = pageAccessStop(dev,phyAddr,pageNum,regAddr,autoOn,pageReg);
        if(retVal != MAD_OK)
        {
            madSemGive(dev,dev->multiAddrSem);

            MAD_DBG_ERROR("Accessing Page Register failed\n");
            return (retVal | MAD_DRV_FAIL_ACCESS_PAGE_REG);
        }
    }

    madSemGive(dev,dev->multiAddrSem);

    MAD_DBG_INFO("Read from port(%d): pageNum %i, smiAddr %i, regAddr %i\n",
                portNum,pageNum,phyAddr,regAddr);
    MAD_DBG_INFO("Data read:0x%x\n",*data);

    return MAD_OK;
}

/*******************************************************************************
* madHwWritePagedPhyReg
*
* DESCRIPTION:
*       This function writes to a phy register in paged mode.
*
* INPUTS:
*       portNum - Port number to write the register for.
*       pageNum - Page number to be accessed.
*       regAddr - The register's address.
*       data    - The data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madHwWritePagedPhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U16    pageNum,
    IN  MAD_U8     regAddr,
    IN  MAD_U16    data
)
{
    MAD_U8       phyAddr;
    MAD_STATUS   retVal;
    MAD_BOOL     anyPage;
    MAD_BOOL     autoOn;
    MAD_U16      pageReg;

    if(portNum >= dev->numOfPorts)
    {
        MAD_DBG_ERROR("Invalid port Number(%i)\n",portNum);
        return MAD_DRV_INVALID_PORT;
    }

    if(MAD_IS_ANY_PAGE(dev,regAddr))
        anyPage = MAD_TRUE;
    else
        anyPage = MAD_FALSE;

    phyAddr = MAD_CALC_SMI_DEV_ADDR(dev, portNum);
    if (phyAddr > 0x1F)
    {
        MAD_DBG_ERROR("Failed to get phy Address (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_GET_PHY_ADDR;
    }

    madSemTake(dev,dev->multiAddrSem,OS_WAIT_FOREVER);

    if (!anyPage)
    {
        retVal = pageAccessStart(dev,phyAddr,pageNum,regAddr,&autoOn,&pageReg);
        if(retVal != MAD_OK)
        {
            madSemGive(dev,dev->multiAddrSem);

            MAD_DBG_ERROR("Accessing Page Register failed\n");
            return (retVal | MAD_DRV_FAIL_ACCESS_PAGE_REG);
        }
    }
        
    retVal = madSmiIfWriteRegister(dev,phyAddr,regAddr,data);
    if(retVal != MAD_OK)
    {
        madSemGive(dev,dev->multiAddrSem);

        MAD_DBG_ERROR("Failed to write register (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_WRITE_REG;
    }

    if (!anyPage)
    {
        retVal = pageAccessStop(dev,phyAddr,pageNum,regAddr,autoOn,pageReg);
        if(retVal != MAD_OK)
        {
            madSemGive(dev,dev->multiAddrSem);

            MAD_DBG_ERROR("Accessing Page Register failed\n");
            return (retVal | MAD_DRV_FAIL_ACCESS_PAGE_REG);
        }
    }

    madSemGive(dev,dev->multiAddrSem);

    MAD_DBG_INFO("Write to port(%d): pageNum %i, smiAddr %i, regAddr %i\n",
            portNum,pageNum,phyAddr,regAddr);
    MAD_DBG_INFO("Data wrote:0x%x\n",data);

    return retVal;

}


/*******************************************************************************
* madHwGetPagedPhyRegField
*
* DESCRIPTION:
*       This function reads a specified field from a phy register in paged mode.
*
* INPUTS:
*       portNum     - Port number to read the register for.
*       pageNum - Page number to be accessed.
*       regAddr     - The register's address.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to read.
*
* OUTPUTS:
*       data        - The read register field.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       1.  The sum of fieldOffset & fieldLength parameters must be smaller-
*           equal to 16.
*
*******************************************************************************/
MAD_STATUS madHwGetPagedPhyRegField
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U16    pageNum,
    IN  MAD_U8     regAddr,
    IN  MAD_U8     fieldOffset,
    IN  MAD_U8     fieldLength,
    OUT MAD_U16    *data
)
{
    MAD_U16 mask;            /* Bits mask to be read */
    MAD_U16 tmpData;
    MAD_STATUS   retVal;
    MAD_U8       phyAddr;
    MAD_BOOL     anyPage;
    MAD_BOOL     autoOn;
    MAD_U16      pageReg;

    if(portNum >= dev->numOfPorts)
    {
        MAD_DBG_ERROR("Invalid port Number(%i)\n",portNum);
        return MAD_DRV_INVALID_PORT;
    }

    if(MAD_IS_ANY_PAGE(dev,regAddr))
        anyPage = MAD_TRUE;
    else
        anyPage = MAD_FALSE;

    phyAddr = MAD_CALC_SMI_DEV_ADDR(dev, portNum);
    if (phyAddr > 0x1F)
    {
        MAD_DBG_ERROR("Failed to get phy Address (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_GET_PHY_ADDR;
    }

    madSemTake(dev,dev->multiAddrSem,OS_WAIT_FOREVER);

    if (!anyPage)
    {
        retVal = pageAccessStart(dev,phyAddr,pageNum,regAddr,&autoOn,&pageReg);
        if(retVal != MAD_OK)
        {
            madSemGive(dev,dev->multiAddrSem);

            MAD_DBG_ERROR("Accessing Page Register failed\n");
            return (retVal | MAD_DRV_FAIL_ACCESS_PAGE_REG);
        }
    }

    retVal = madSmiIfReadRegister(dev,phyAddr,regAddr,&tmpData);
    if(retVal != MAD_OK)
    {
        madSemGive(dev,dev->multiAddrSem);

        MAD_DBG_ERROR("Failed to read register (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_READ_REG;
    }

    if (!anyPage)
    {
        retVal = pageAccessStop(dev,phyAddr,pageNum,regAddr,autoOn,pageReg);
        if(retVal != MAD_OK)
        {
            madSemGive(dev,dev->multiAddrSem);

            MAD_DBG_ERROR("Accessing Page Register failed\n");
            return (retVal | MAD_DRV_FAIL_ACCESS_PAGE_REG);
        }
    }

    madSemGive(dev,dev->multiAddrSem);

    MAD_CALC_MASK(fieldOffset,fieldLength,mask);

    tmpData = (tmpData & mask) >> fieldOffset;
    *data = tmpData;

    MAD_DBG_INFO("fOff %d, fLen %d, data 0x%x.\n",
                fieldOffset,fieldLength,*data);

    return MAD_OK;

}


/*******************************************************************************
* madHwSetPagedPhyRegField
*
* DESCRIPTION:
*       This function writes to specified field in a phy register in paged mode.
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       pageNum     - Page number to be accessed.
*       regAddr     - The register's address.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*       data        - Data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*       1.  The sum of fieldOffset & fieldLength parameters must be smaller-
*           equal to 16.
*
*******************************************************************************/
MAD_STATUS madHwSetPagedPhyRegField
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U16    pageNum,
    IN  MAD_U8     regAddr,
    IN  MAD_U8     fieldOffset,
    IN  MAD_U8     fieldLength,
    IN  MAD_U16    data
)
{
    MAD_U16 mask;
    MAD_U16 tmpData;
    MAD_STATUS   retVal;
    MAD_U8       phyAddr;
    MAD_BOOL     anyPage;
    MAD_BOOL     autoOn;
    MAD_U16      pageReg;


    if(portNum >= dev->numOfPorts)
    {
        MAD_DBG_ERROR("Invalid port Number(%i)\n",portNum);
        return MAD_DRV_INVALID_PORT;
    }

    if(MAD_IS_ANY_PAGE(dev,regAddr))
        anyPage = MAD_TRUE;
    else
        anyPage = MAD_FALSE;


    phyAddr = MAD_CALC_SMI_DEV_ADDR(dev, portNum);
    if (phyAddr > 0x1F)
    {
        MAD_DBG_ERROR("Failed to get phy Address (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_GET_PHY_ADDR;
    }

    madSemTake(dev,dev->multiAddrSem,OS_WAIT_FOREVER);

    if (!anyPage)
    {
        retVal = pageAccessStart(dev,phyAddr,pageNum,regAddr,&autoOn,&pageReg);
        if(retVal != MAD_OK)
        {
            madSemGive(dev,dev->multiAddrSem);

            MAD_DBG_ERROR("Accessing Page Register failed\n");
            return (retVal | MAD_DRV_FAIL_ACCESS_PAGE_REG);
        }
    }

    retVal = madSmiIfReadRegister(dev,phyAddr,regAddr,&tmpData);
    if(retVal != MAD_OK)
    {
        madSemGive(dev,dev->multiAddrSem);

        MAD_DBG_ERROR("Failed to read register (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_READ_REG;
    }

    MAD_CALC_MASK(fieldOffset,fieldLength,mask);

    /* Set the desired bits to 0. */
    tmpData &= ~mask;
    /* Set the given data into the above reset bits. */
    tmpData |= ((data << fieldOffset) & mask);

    retVal = madSmiIfWriteRegister(dev,phyAddr,regAddr,tmpData);

    if(retVal != MAD_OK)
    {
        madSemGive(dev,dev->multiAddrSem);
        MAD_DBG_ERROR("Failed to write register (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_WRITE_REG;
    }

    if (!anyPage)
    {
        retVal = pageAccessStop(dev,phyAddr,pageNum,regAddr,autoOn,pageReg);
        if(retVal != MAD_OK)
        {
            madSemGive(dev,dev->multiAddrSem);

            MAD_DBG_ERROR("Accessing Page Register failed\n");
            return (retVal | MAD_DRV_FAIL_ACCESS_PAGE_REG);
        }
    }

    madSemGive(dev,dev->multiAddrSem);

    MAD_DBG_INFO("fieldOff %d, fieldLen %d, data 0x%x.\n",fieldOffset,
                  fieldLength,data);

    return MAD_OK;
}


/*******************************************************************************
* madHwPagedSetCtrlPara
*
* DESCRIPTION:
*       This function set control parameter through soft reset.
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       pageNum     - Page number to be accessed.
*        ctrlData    - Control parameter.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS madHwPagedSetCtrlPara
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U16    pageNum,
    IN    MAD_U16       ctrlData
)
{
    MAD_U16 tmpData;
    MAD_STATUS   retVal;
    MAD_U8       phyAddr,regAddr;
    MAD_BOOL     anyPage;
    MAD_U32 retryCount;
    MAD_BOOL     autoOn;
    MAD_U16      pageReg;

    MAD_DBG_INFO("Software Reset...\n");

    if(portNum >= dev->numOfPorts)
    {
        MAD_DBG_ERROR("Invalid port Number(%i)\n",portNum);
        return MAD_DRV_INVALID_PORT;
    }

    regAddr = 0;    /* phy control register */

    if(MAD_IS_ANY_PAGE(dev,regAddr))
        anyPage = MAD_TRUE;
    else
        anyPage = MAD_FALSE;


    phyAddr = MAD_CALC_SMI_DEV_ADDR(dev, portNum);
    if (phyAddr > 0x1F)
    {
        MAD_DBG_ERROR("Failed to get phy Address (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_GET_PHY_ADDR;
    }

    madSemTake(dev,dev->multiAddrSem,OS_WAIT_FOREVER);

    if (!anyPage)
    {
        retVal = pageAccessStart(dev,phyAddr,pageNum,regAddr,&autoOn,&pageReg);
        if(retVal != MAD_OK)
        {
            madSemGive(dev,dev->multiAddrSem);

            MAD_DBG_ERROR("Accessing Page Register failed\n");
            return (retVal | MAD_DRV_FAIL_ACCESS_PAGE_REG);
        }
    }

    if (ctrlData != 0xFFFF )  /* suppose ctrlData should never be 0xFFFF, if it used to set control para */
        tmpData = ctrlData;
    else    /* It is a normal soft reset */
    {
        retVal = madSmiIfReadRegister(dev,phyAddr,regAddr,&tmpData);
        if(retVal != MAD_OK)
        {
            madSemGive(dev,dev->multiAddrSem);

            MAD_DBG_ERROR("Failed to read register (portNum %i)\n",portNum);
            return MAD_DRV_FAIL_READ_REG;
        }
    }

    /* Set the reset bit to 1. */
    tmpData |= MAD_BIT_15;

    retVal = madSmiIfWriteRegister(dev,phyAddr,regAddr,tmpData);
    if(retVal != MAD_OK)
    {
        madSemGive(dev,dev->multiAddrSem);

        MAD_DBG_ERROR("Failed to write register (portNum %i)\n",portNum);
        return MAD_DRV_FAIL_WRITE_REG;
    }

    for (retryCount = 0x1000; retryCount > 0; retryCount--)
    {
        retVal = madSmiIfReadRegister(dev,phyAddr,regAddr,&tmpData);
        if(retVal != MAD_OK)
        {
            madSemGive(dev,dev->multiAddrSem);

            MAD_DBG_ERROR("Failed to read register (portNum %i)\n",portNum);
            return MAD_DRV_FAIL_READ_REG;
        }
        if ((tmpData & MAD_BIT_15) == 0)
            break;
    }

    if (!anyPage)
    {
        retVal = pageAccessStop(dev,phyAddr,pageNum,regAddr,autoOn,pageReg);
        if(retVal != MAD_OK)
        {
            madSemGive(dev,dev->multiAddrSem);

            MAD_DBG_ERROR("Accessing Page Register failed\n");
            return (retVal | MAD_DRV_FAIL_ACCESS_PAGE_REG);
        }
    }

    madSemGive(dev,dev->multiAddrSem);

    if (retryCount == 0)
    {
        MAD_DBG_ERROR("Reset bit is not cleared\n");
        return MAD_DRV_FAIL_RESET;
    }

    return MAD_OK;
}

/*******************************************************************************
* madHwPagedReset
*
* DESCRIPTION:
*       This function performs softreset and waits until reset completion.
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       pageNum     - Page number to be accessed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK on success, or
*       MAD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS madHwPagedReset
(
    IN  MAD_DEV    *dev,
    IN  MAD_U8     portNum,
    IN  MAD_U16    pageNum
)
{
    MAD_STATUS   retVal;

    MAD_DBG_INFO("Software Reset...\n");
    retVal = madHwPagedSetCtrlPara (dev, portNum, pageNum, 0xFFFF);

    return retVal;
}


