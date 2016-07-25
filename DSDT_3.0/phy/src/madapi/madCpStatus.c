#include <madCopyright.h>

/********************************************************************************
* madCpStatus.c
* 
* DESCRIPTION:
*       APIs to get PHY status.
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
* mdCopperGetDownshiftStatus
*
* DESCRIPTION:
*       This routine retrieves the status of Downshift.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       st   - MAD_TRUE if downshifted, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 17, Bit 5 : Copper Status 1 (Downshift status)
*
*******************************************************************************/
MAD_STATUS mdCopperGetDownshiftStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *st
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperGetDownshiftStatus called.\n");

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

    /* if ((dev->deviceId==MAD_88E3082) ||(dev->deviceId==MAD_88E3016) ) */
    if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
    {
        MAD_DBG_ERROR("Down Shift not supported.\n");
        return MAD_API_DOWNSHIFT_NOT_SUPPORT;
    }

    /* Get the required bit in Copper Status Register 1 */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_COPPER_STATUS_1,
                    5,1,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }

    /* Enabled or disabled */
    MAD_BIT_2_BOOL(data,*st);

    return MAD_OK;
}

/*******************************************************************************
* mdCopperGetLinkStatus
*
* DESCRIPTION:
*       This routine retrieves the Link status.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       linkOn - MAD_TRUE if link is established, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 17, Bit 10 : Copper Link
*
*******************************************************************************/
MAD_STATUS mdCopperGetLinkStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *linkOn
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperGetLinkStatus called.\n");

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

    /* Get the required bit in Copper Status Register 1 */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_COPPER_STATUS_1,
                    10,1,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }

    /* Enabled or disabled */
    MAD_BIT_2_BOOL(data,*linkOn);

    return MAD_OK;
}


/*******************************************************************************
* mdCopperGetSpeedStatus
*
* DESCRIPTION:
*       This routine retrieves the current Speed.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       mode - current speed (MAD_SPEED_MODE).
*               MAD_SPEED_10M, for 10 Mbps
*               MAD_SPEED_100M, for 100 Mbps
*               MAD_SPEED_1000M, for 1000 Mbps
*               MAD_SPEED_UNKNOWN, when speed is not resoved.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 17, Bit 15:14,11 : Speed, Speed resolved
*
*******************************************************************************/
MAD_STATUS mdCopperGetSpeedStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_SPEED_MODE *mode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperGetSpeedStatus called.\n");

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

    /* Get Copper Status Register 1 */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,0,MAD_REG_COPPER_STATUS_1,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* check if Speed and Duplex is resolved */
    if (!(data & MAD_BIT_11))
    {
        MAD_DBG_INFO("Speed and Duplex is not resolved.\n");
        *mode = MAD_SPEED_UNKNOWN;
        return MAD_OK;

    }

    data = (data & 0xC000) >> 14;

    switch (data)
    {
        case 0:
            *mode = MAD_SPEED_10M;
            break;
        case 1:
            *mode = MAD_SPEED_100M;
            break;
        case 2:
            *mode = MAD_SPEED_1000M;
            break;
        default:
            MAD_DBG_ERROR("Invalid speed is set %i.\n",data);
            return MAD_API_UNKNOWN_SPEED_MODE;
    }

    return MAD_OK;
}


/*******************************************************************************
* mdCopperGetDuplexStatus
*
* DESCRIPTION:
*       This routine retrieves the current Duplex.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       mode - current duplex mode (MAD_DUPLEX_MODE).
*               MAD_HALF_DUPLEX, for half duplex mode
*               MAD_FULL_DUPLEX, for half duplex mode
*               MAD_DUPLEX_UNKNOWN, when duplex is not resoved.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 17, Bit 13,11 : Duplex, Duplex resolved
*
*******************************************************************************/
MAD_STATUS mdCopperGetDuplexStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_DUPLEX_MODE *mode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperGetDuplexStatus called.\n");

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

    /* Get Copper Status Register 1 */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,0,MAD_REG_COPPER_STATUS_1,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* check if Speed and Duplex is resolved */
    if (!(data & MAD_BIT_11))
    {
        MAD_DBG_INFO("Speed and Duplex is not resolved.\n");
        *mode = MAD_DUPLEX_UNKNOWN;
        return MAD_OK;

    }

    data = (data >> 13) & 0x1;

    switch (data)
    {
        case 0:
            *mode = MAD_HALF_DUPLEX;
            break;
        case 1:
            *mode = MAD_FULL_DUPLEX;
            break;
        default:
            MAD_DBG_ERROR("Invalid duplex is set %i.\n",data);
            return MAD_API_UNKNOWN_DUPLEX_MODE;
    }

    return MAD_OK;
}


/*******************************************************************************
* mdCopperGetDTEDetectStatus
*
* DESCRIPTION:
*       This routine retrieves the status of DTE (such as IP Phone) Detect.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       st   - MAD_TRUE if DTE detected, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 17, Bit 2 : Copper Status (DTE Detect)
*       or
*       Page 0, Reg 27, Bit 4 : Copper Status (DTE Detect) 
*
*******************************************************************************/
MAD_STATUS mdCopperGetDTEDetectStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *st
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperGetDTEDetectStatus called.\n");

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

    switch (dev->deviceId)
    {
        case MAD_88E104X:   
        case MAD_88E1111:   
        case MAD_88E114X:  
           /* Get the required bit in Copper Status Register 1 */
            if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,27,
                    4,1,&data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
            }
            break;

        case MAD_88E3082:  
           /* Get the required bit in Copper Status Register 1 */
            if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,17,
                    16,1,&data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
            }
            break;

        case MAD_88E1181:   
        case MAD_88E1149:   
        case MAD_88E1149R:   
        case MAD_SWG65G : 
        case MAD_88E1240:   
        case MAD_88E1340S:   
        case MAD_88E1340:   
        case MAD_88E1340M:   
        case MAD_88E1540:   
        case MAD_88E1116:   
        case MAD_88E1112:   
        case MAD_88E1121:   
        case MAD_88E1119R:   
        case MAD_88E1310:   
           /* Get the required bit in Copper Status Register 1 */
            if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_COPPER_STATUS_1,
                    2,1,&data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
            }
            break;

           default:
            return MAD_API_DTEDETECT_NOT_SUPPORT;

    }



    /* Enabled or disabled */
    MAD_BIT_2_BOOL(data,*st);

    return MAD_OK;
}



