#include <madCopyright.h>

/********************************************************************************
* madSysCtrl.c
*
* DESCRIPTION:
*       API definitions for the system control configuration
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
#include <madDrvConfig.h>
#include <madHwCntl.h>
#include <madSem.h>
#include <madPlatformDeps.h>
#include <madDrvConfig.h>

/*******************************************************************************
* mdGetLinkStatus
*
* DESCRIPTION:
*       This routine retrieves the Link status of the current active media.
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
*       Reg 17 Page0 (Copper) or Page1(Fiber), Bit 10 : Link
*
*******************************************************************************/
static MAD_STATUS mdGetLinkStatus_
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *linkOn
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdGetLinkStatus called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
     MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
     return MAD_API_ERROR_OVER_PORT_RANGE;

  }

  *linkOn = MAD_FALSE;
  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    /* Get the required bit in Copper Status Register 1 */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE0, MAD_REG_COPPER_STATUS_1,
                    10,1,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_REG_FIELD);
    }

    /* Enabled or disabled */
    MAD_BIT_2_BOOL(data,*linkOn);

  }
  else if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_FIBER))
  {
    /* Get the required bit in Copper Status Register 1 */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE1, MAD_REG_FIBER_STATUS_1,
                    10,1,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_REG_FIELD);
    }

    /* Enabled or disabled */
    MAD_BIT_2_BOOL(data,*linkOn);
  }
  else
  {
     MAD_DBG_ERROR("Device is neither Copper nor Fiber.\n");
     return MAD_API_ERROR_SET_COPPER_FIBER;
  }
 
    return MAD_OK;
}

MAD_STATUS mdGetLinkStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *linkOn
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if((retVal=mdGetLinkStatus_(dev,port,linkOn))!= MAD_OK)
  {
    MAD_DBG_ERROR("mdGetLinkStatus for Copper or Fiber failed.\n");
    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR))
      return (retVal);
  }

  if((MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR))&&(*linkOn==MAD_TRUE))
    return MAD_OK;
  
  /* After 88E1340 */
    if ((MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
        ||(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_FIBER)))
        return MAD_OK;

  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_SGMII_MEDIA))
  {
    /* Get the required bit in Copper Status Register 1 */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE1, MAD_REG_FIBER_STATUS_1,
                    10,1,&data)) != MAD_OK)
    {
      MAD_DBG_ERROR("Reading from phy reg failed.\n");
      return (retVal | MAD_API_FAIL_GET_REG_FIELD);
    }

     /* Enabled or disabled */
     MAD_BIT_2_BOOL(data,*linkOn);
  }
  else if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_QSGMII_MEDIA))
  {
    /* Get the required bit in Copper Status Register 1 */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE4, MAD_REG_QSGMII_STATUS_1,
                    10,1,&data)) != MAD_OK)
    {
      MAD_DBG_ERROR("Reading from phy reg failed.\n");
      return (retVal | MAD_API_FAIL_GET_REG_FIELD);
     }

     /* Enabled or disabled */
     MAD_BIT_2_BOOL(data,*linkOn);
  }
  else if((MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_AUTO_DETECT_SC))
         ||(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_AUTO_DETECT_XC)))
  {
    /* Get the required bit in Fiber Status Register 1 */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE1, MAD_REG_FIBER_STATUS_1,
                    10,1,&data)) != MAD_OK)
    {
      MAD_DBG_ERROR("Reading from phy reg failed.\n");
      return (retVal | MAD_API_FAIL_GET_REG_FIELD);
     }
     MAD_BIT_2_BOOL(data,*linkOn);

    if (*linkOn != MAD_TRUE)
    {
      /* Get the required bit in Copper Status Register 1 */
      if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE0, MAD_REG_COPPER_STATUS_1,
                    10,1,&data)) != MAD_OK)
      {
        MAD_DBG_ERROR("Reading from phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_REG_FIELD);
      }
      /* Enabled or disabled */
      MAD_BIT_2_BOOL(data,*linkOn);
    }
    else
    {
    }

  }
  else
  {
    MAD_DBG_ERROR("Device is neither Copper nor Fiber nor SGMII nor QSFMII.\n");
    return MAD_API_ERROR_SET_COPPER_FIBER;
  }
 
    return MAD_OK;
}


/*******************************************************************************
* mdGetSpeedStatus
*
* DESCRIPTION:
*       This routine retrieves the current Speed  of the current active media.
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
*       Reg 17 Page0 (Copper) or Page1(Fiber), Bit 15:14,11 : Speed, Speed resolved
*
*******************************************************************************/
MAD_STATUS mdGetSpeedStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_SPEED_MODE *mode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdGetSpeedStatus called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
     MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
     return MAD_API_ERROR_OVER_PORT_RANGE;

  }

  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    /* Get Status Register 1 */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE0, MAD_REG_COPPER_STATUS_1,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_REG);
    }
  }
  else if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_FIBER))
  {
    /* Get Status Register 1 */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE1, MAD_REG_FIBER_STATUS_1,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_REG);
    }
  }
  else
  {
     MAD_DBG_ERROR("Device is neither Copper nor Fiber.\n");
     return MAD_API_ERROR_SET_COPPER_FIBER;
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
* mdGetMacSpeedStatus
*
* DESCRIPTION:
*       This routine retrieves the current SGMII Mac Speed.
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
*       Reg 21 Page2, Bit 2,1,0 : Speed, Speed resolved
*
*******************************************************************************/
MAD_STATUS mdGetMacSpeedStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_SPEED_MODE *mode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;
    MAD_BOOL    linkOn;

    MAD_DBG_INFO("mdGetSpeedStatus called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
     MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
     return MAD_API_ERROR_OVER_PORT_RANGE;

  }

  if((retVal=mdGetLinkStatus_(dev,port, &linkOn))!= MAD_OK)
  {
    MAD_DBG_ERROR("mdGetLinkStatus for Copper or Fiber failed.\n");
    return (retVal);
  }

  if (linkOn == MAD_TRUE)
  {
    retVal = mdGetSpeedStatus(dev,port,mode);
    if (retVal != MAD_OK)
    {
      MAD_DBG_ERROR("mdGetSpeedStatus for Copper or Fiber failed.\n");
    }
    return (retVal);
  }

  /* Get Mac Status Register */
  if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE2, MAD_REG_MAC_CONTROL,&data))
      != MAD_OK)
  {
    MAD_DBG_ERROR("Reading from phy reg failed.\n");
    return (retVal | MAD_API_FAIL_READ_REG);
  }

    data &= 0x3;

    switch (data)
    {
        case 4:
            *mode = MAD_SPEED_10M;
            break;
        case 5:
            *mode = MAD_SPEED_100M;
            break;
        case 6:
            *mode = MAD_SPEED_1000M;
            break;
        default:
            MAD_DBG_ERROR("Invalid speed is set %i.\n",data);
            return MAD_API_UNKNOWN_SPEED_MODE;
    }

    return MAD_OK;
}


/*******************************************************************************
* mdGetDuplexStatus
*
* DESCRIPTION:
*       This routine retrieves the current Duplex of the current active media. 
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
*       Reg 17 Page0 (Copper) or Page1(Fiber), Bit 13,11 : Duplex, Duplex resolved
*
*******************************************************************************/
MAD_STATUS mdGetDuplexStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_DUPLEX_MODE *mode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdGetDuplexStatus called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
     MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
     return MAD_API_ERROR_OVER_PORT_RANGE;

  }

  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    /* Get Status Register 1 */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE0, MAD_REG_COPPER_STATUS_1,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_REG);
    }
  }
  else if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_FIBER))
  {
    /* Get Status Register 1 */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE1, MAD_REG_FIBER_STATUS_1,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_REG);
    }
  }
  else
  {
     MAD_DBG_ERROR("Device is neither Copper nor Fiber.\n");
     return MAD_API_ERROR_SET_COPPER_FIBER;
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
* mdSysGetPhyReg
*
* DESCRIPTION:
*       This function reads a phy register of the current page.
*       This API is provided for debugging or general use.
*
* INPUTS:
*       portNum - Port number to read the register for.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetPhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U32    portNum,
    IN  MAD_U32    regAddr,
    OUT MAD_U32    *data
)
{
    return madHwReadPhyReg(dev,(MAD_U8)portNum,(MAD_U8)regAddr,(MAD_U16*)data);
}


/*******************************************************************************
* mdSysSetPhyReg
*
* DESCRIPTION:
*       This function writes to a phy register of the current page..
*       This API is provided for debugging or general use.
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
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetPhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U32    portNum,
    IN  MAD_U32    regAddr,
    IN  MAD_U32    data
)
{
    return madHwWritePhyReg(dev,(MAD_U8)portNum,(MAD_U8)regAddr,(MAD_U16)data);
}


/*******************************************************************************
* mdSysGetPagedPhyReg
*
* DESCRIPTION:
*       This function reads a phy register in paged mode.
*       This API is provided for debugging or general use.
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
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetPagedPhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U32    portNum,
    IN  MAD_U32    pageNum,
    IN  MAD_U32    regAddr,
    OUT MAD_U32    *data
)
{
    MAD_STATUS  retVal;

    if((retVal=madHwReadPagedPhyReg(dev,(MAD_U8)portNum,(MAD_U16)pageNum,
                                (MAD_U8)regAddr,(MAD_U16*)data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Paged Phy Register read failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    return MAD_OK;
}


/*******************************************************************************
* mdSysSetPagedPhyReg
*
* DESCRIPTION:
*       This function writes to a phy register in paged mode.
*       This API is provided for debugging or general use.
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
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetPagedPhyReg
(
    IN  MAD_DEV    *dev,
    IN  MAD_U32    portNum,
    IN  MAD_U32    pageNum,
    IN  MAD_U32    regAddr,
    IN  MAD_U32    data
)
{
    MAD_STATUS  retVal;

    if((retVal=madHwWritePagedPhyReg(dev,(MAD_U8)portNum,(MAD_U16)pageNum,
                                (MAD_U8)regAddr,(MAD_U16)data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Paged Phy Register write failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    return MAD_OK;
}


/*******************************************************************************
* mdSysSetPhyEnable
*
* DESCRIPTION:
*       This function enables a copper medium of the phy by powering up.
*
* INPUTS:
*       portNum - Port number to be configured.
*       en      - MAD_TRUE to enable or MAD_FALSE to disable
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
MAD_STATUS mdSysSetPhyEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_BOOL    en
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("madSysSetPhyEnable called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  /* Enable or disable PHY */
  if (en==MAD_TRUE)
    data = 0;
  else
    data = 1;


  if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_EX_POWER_DOWN))
  {
      MAD_DBG_ERROR("Set Power down does not support.\n");
      return MAD_API_PW_DW_NOT_SUPPORT;
  }

  if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
  {
    /* Set or reset Power Down bit */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_CONTROL_1,
                    2,1,data)) != MAD_OK)
    {
      MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
      return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
  }

  if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR))
  {
    /* Set or reset Power Down bit */
    if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,MAD_PAGE0,MAD_REG_CONTROL,
                            11,1,data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
  }
  else
  {
    if(MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
    {
      /* Set or reset Power Down bit */
      if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,MAD_PAGE0,MAD_REG_CONTROL,
                            11,1,data)) != MAD_OK)
      {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
    }
    else if((MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_FIBER)) ||
           (MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_SGMII_MEDIA)))
    {
      /* Set or reset Power Down bit */
      if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,MAD_PAGE1,MAD_REG_CONTROL,
                            11,1,data)) != MAD_OK)
      {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
    }
    else if(MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_QSGMII_MEDIA))
    {
      /* Set or reset Power Down bit */
      if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,MAD_PAGE4,MAD_REG_CONTROL,
                            11,1,data)) != MAD_OK)
      {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
    }
  }
    return MAD_OK;
}


/*******************************************************************************
* mdSysGetPhyEnable
*
* DESCRIPTION:
*       This function can be use to check if copper of the phy is powered up.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       en      - MAD_TRUE if enabled, or MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetPhyEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("madSysGetPhyEnable called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_EX_POWER_DOWN))
  {
      MAD_DBG_ERROR("Set Power down does not support.\n");
      return MAD_API_PW_DW_NOT_SUPPORT;
  }

  if(!(MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR)))
  {
    /* Get the Power down bit */
    if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE0,MAD_REG_CONTROL,11,1,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }
  }
  else
  {

    if(MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
    {
       /* Get the Power down bit */
       if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE0,MAD_REG_CONTROL,11,1,&data))
        != MAD_OK)
       {
          MAD_DBG_ERROR("Reading phy reg failed.\n");
          return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
       }
    }
    else if((MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_FIBER)) ||
           (MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_SGMII_MEDIA)))
    {
       /* Get the Power down bit */
       if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE1,MAD_REG_CONTROL,11,1,&data))
        != MAD_OK)
       {
          MAD_DBG_ERROR("Reading phy reg failed.\n");
          return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
       }
    }
    else if(MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_QSGMII_MEDIA))
    {
       /* Get the Power down bit */
       if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE4,MAD_REG_CONTROL,11,1,&data))
        != MAD_OK)
       {
          MAD_DBG_ERROR("Reading phy reg failed.\n");
          return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
       }
    }
  }

    if(data)
        *en = MAD_FALSE;
    else
        *en = MAD_TRUE;

    return MAD_OK;
}

/*******************************************************************************
* mdSysSetQSGMIIPwDown
*
* DESCRIPTION:
*       This function set Global QSGMII power down.
*
* INPUTS:
*       port - Port number to be configured.
*       en      - MAD_TRUE to power down or MAD_FALSE to power up
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
MAD_STATUS mdSysSetQSGMIIPwDown
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_BOOL    en
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("madSysSetPhyEnable called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* Enable or disable PHY */
    if (en)
        data = 0;
    else
        data = 1;

  if(MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR))
  {
    /* Set or reset Power Down bit */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE4,MAD_REG_QSGMII_GLOBAL_CTRL_1,
                    11,1,data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
  }
  else
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;

  return MAD_OK;
}


/*******************************************************************************
* mdSysGetQSGMIIPwDown
*
* DESCRIPTION:
*       This function get Global QSGMII power down statue
*
* INPUTS:
*       port - Port number to be configured.
*
* OUTPUTS:
*       en      - MAD_TRUE to power down or MAD_FALSE to power up
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetQSGMIIPwDown
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("madSysGetPhyEnable called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR))
  {
    /* Set or reset Power Down bit */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE4,MAD_REG_QSGMII_GLOBAL_CTRL_1,
                    11,1,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
  
    if(data)
      *en = MAD_FALSE;
    else
      *en = MAD_TRUE;
  }
  else
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;

  return MAD_OK;
}

/*******************************************************************************
* mdSysSoftReset
*
* DESCRIPTION:
*       This function set different type soft reset.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSoftReset
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port
)
{
    return madHwPagedReset(dev,MAD_LPORT_2_PORT(port),0);
}

/*******************************************************************************
* mdSysModSoftReset
*
* DESCRIPTION:
*       This function set soft reset.
*
* INPUTS:
*       portNum - Port number to be configured.
*       mod - Reset mode to be configured.(MAD_RESET_MODE_PORT_COPPER: 0_0.15,
*                                     MAD_RESET_MODE_PORT_FIBER: 0_1.15,
*                                     MAD_RESET_MODE_PORT_QSGMII: 0_4.15,
*                                     MAD_RESET_MODE_ALL_QSGNII: 20_4.15,
*                                     MAD_RESET_MODE_PORT_COUNTER: 20_6.15,
*                                     MAD_RESET_MODE_ALL_CHIP: 27_4.15)
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysModSoftReset
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_U32        mode
)
{
  switch (mode)
  {
    case MAD_RESET_MODE_PORT_COPPER:
        return madHwPagedReset(dev,MAD_LPORT_2_PORT(port),MAD_PAGE0);
    case MAD_RESET_MODE_PORT_FIBER:
        return madHwPagedReset(dev,MAD_LPORT_2_PORT(port),MAD_PAGE1);
    case MAD_RESET_MODE_PORT_QSGMII:
        return madHwPagedReset(dev,MAD_LPORT_2_PORT(port),MAD_PAGE4);

    case MAD_RESET_MODE_ALL_QSGNII:
        return madHwSetPagedPhyRegField(dev,MAD_LPORT_2_PORT(port),
                        MAD_PAGE4,(MAD_U8)26, 15,1,1);
    case MAD_RESET_MODE_PORT_COUNTER:
        return madHwSetPagedPhyRegField(dev,MAD_LPORT_2_PORT(port),
                        MAD_PAGE6,(MAD_U8)20, 15,1,1);
    case MAD_RESET_MODE_ALL_CHIP:
        return madHwSetPagedPhyRegField(dev,MAD_LPORT_2_PORT(port),
                        MAD_PAGE4,(MAD_U8)27, 15,1,1);
    default:
        MAD_DBG_ERROR("Invalid reset mode is set %x.\n", (unsigned int)mode);
        return MAD_API_UNKNOWN_RESET_MODE;
  }
}

/*******************************************************************************
* mdSysSetMacPowerDown
*
* DESCRIPTION:
*       This function enables Mac Interface of the phy by powering up.
*
* INPUTS:
*       portNum - Port number to be configured.
*       en      - MAD_TRUE to enable or MAD_FALSE to disable
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
MAD_STATUS mdSysSetMacPowerDown
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_BOOL    en
)
{
    MAD_U16     mode, data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("madSysSetPhyEnable called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* Enable or disable PHY */
    if (en)
        data = 0;
    else
        data = 1;


  if(MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR))
  {
    if((retVal = madGetHwMode(dev,hwPort, &mode)) != MAD_OK)
    {
      MAD_DBG_ERROR("Reading to phy reg failed.\n");
      return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }

    switch (mode)
    {
      case 0:
       case 1:
      /* Set or reset Power Down bit */
        if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,MAD_PAGE2,MAD_REG_MAC_CONTROL_1,
                            3,1,data)) != MAD_OK)
        {
          MAD_DBG_ERROR("Writing to phy reg failed.\n");
          return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        break;
      case 2:
      case 3:
       case 4:
       /* Set or reset Power Down bit */
        if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,MAD_PAGE1,MAD_REG_FIBER_CONTROL_1,
                            3,1,data)) != MAD_OK)
        {
          MAD_DBG_ERROR("Writing to phy reg failed.\n");
          return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        break;
      case 6:
       case 7:
        if(MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
        {
          /* Set or reset Power Down bit */
          if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,MAD_PAGE2,MAD_REG_FIBER_CONTROL_1,
                            3,1,data)) != MAD_OK)
          {
            MAD_DBG_ERROR("Writing to phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
          }
        }
        else if(MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_FIBER))
        {
          /* Set or reset Power Down bit */
          if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,MAD_PAGE1,MAD_REG_FIBER_CONTROL_1,
                            3,1,data)) != MAD_OK)
          {
            MAD_DBG_ERROR("Writing to phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
          }
        }
        break;

      default:
        MAD_DBG_ERROR("Invalid HW mode is set %x.\n", (unsigned int)data);
        return MAD_API_UNKNOWN_HW_MODE;
    }
  }
  else
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;

  /* Soft Reset */
  if((retVal = mdSysSoftReset(dev, hwPort)) != MAD_OK)
  {
    MAD_DBG_ERROR("Soft Reset failed.\n");
    return (retVal | MAD_API_FAIL_SW_RESET);
  }

  return MAD_OK;
}


/*******************************************************************************
* mdSysGetMacPowerDown
*
* DESCRIPTION:
*       This function can be use to check if Mac interface is powered up.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       en      - MAD_TRUE if enabled, or MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetMacPowerDown
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en
)
{
    MAD_U16     mode, data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("madSysGetPhyEnable called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

  if(MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR))
  {
    if((retVal = madGetHwMode(dev,hwPort, &mode)) != MAD_OK)
    {
      MAD_DBG_ERROR("Reading to phy reg failed.\n");
      return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }

    switch (mode)
    {
      case 0:
       case 1:
       /* Get Power Down bit */
        if((retVal = madHwGetPagedPhyRegField(
                            dev,hwPort,MAD_PAGE2,MAD_REG_MAC_CONTROL_1,
                            3,1,&data)) != MAD_OK)
        {
          MAD_DBG_ERROR("Reading to phy reg failed.\n");
          return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        break;
      case 2:
      case 3:
       case 4:
       /* Get Power Down bit */
        if((retVal = madHwGetPagedPhyRegField(
                            dev,hwPort,MAD_PAGE1,MAD_REG_FIBER_CONTROL_1,
                            3,1,&data)) != MAD_OK)
        {
          MAD_DBG_ERROR("Reading to phy reg failed.\n");
          return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        break;
      case 6:
       case 7:
        if(MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
        {
       /* Get Power Down bit */
          if((retVal = madHwGetPagedPhyRegField(
                            dev,hwPort,MAD_PAGE2,MAD_REG_FIBER_CONTROL_1,
                            3,1,&data)) != MAD_OK)
          {
            MAD_DBG_ERROR("Reading to phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
          }
        }
        else if(MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_FIBER))
        {
          /* Set or reset Power Down bit */
          if((retVal = madHwGetPagedPhyRegField(
                            dev,hwPort,MAD_PAGE1,MAD_REG_FIBER_CONTROL_1,
                            3,1,&data)) != MAD_OK)
          {
            MAD_DBG_ERROR("Reading to phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
          }
        }
        break;

      default:
        MAD_DBG_ERROR("Invalid HW mode is set %x.\n", (unsigned int)mode);
        return MAD_API_UNKNOWN_HW_MODE;
    }
  }
  else
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;


    if(data)
        *en = MAD_FALSE;
    else
        *en = MAD_TRUE;

    return MAD_OK;
}


/*******************************************************************************
* mdSysGetDetectPowerDownModeSt
*
* DESCRIPTION:
*       This function get status of Energe Detect Power down Modes.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       status - 1 if active, 0 if sleep
*       change - 1 if enege detect state changed, 0 if no changed
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetDetectPowerDownModeSt
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U16      *status,
    OUT MAD_U16      *change
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdSysGetDetectPowerDownModeSt called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_DETECT_POWER_DOWN))
  {
      MAD_DBG_ERROR("Detect Power down does not support.\n");
      return MAD_API_DETECT_PW_DW_NOT_SUPPORT;
  }

  if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
  {
      MAD_DBG_ERROR("Detect Power down does not support.\n");
      return MAD_API_DETECT_PW_DW_NOT_SUPPORT;
  }

  if(MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    /* Get the Energe Detect Power down status bit */
    if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_STATUS_1,4,1,status))
       != MAD_OK)
    {
      MAD_DBG_ERROR("Reading phy reg failed.\n");
      return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }
    /* Get the Energe Detect Power down state change bit */
    if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_STATUS_2,4,1,change))
       != MAD_OK)
    {
      MAD_DBG_ERROR("Reading phy reg failed.\n");
      return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }
  }
  else
    return MAD_API_COPPER_NOT_SUPPORT;

  return MAD_OK;
}

/*******************************************************************************
* mdSysSetDetectPowerDownMode
*
* DESCRIPTION:
*       This function set modes of Energe Detect Power down.
*
* INPUTS:
*       portNum - Port number to be configured.
*       mode - MAD_ENERGE_DETECT_MODE_NO if normal, 
*               For chip group 30xx
*              MAD_ENERGE_DETECT_MODE_YES if Setting mode, 
*               For chip group 1xxx.
*              MAD_ENERGE_DETECT_MODE_1 if mode1, 
*              MAD_ENERGE_DETECT_MODE_2 if mode0
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetDetectPowerDownMode
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN MAD_U16      mode
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdSysGetDetectPowerDownMode called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_DETECT_POWER_DOWN))
  {
      MAD_DBG_ERROR("Detect Power down does not support.\n");
      return MAD_API_DETECT_PW_DW_NOT_SUPPORT;
  }

  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
    {
      /* Set the Energe Detect Power down mode */
      if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_CONTROL_1,14,1,mode))
       != MAD_OK)
      {
        MAD_DBG_ERROR("Writing phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
    }
    else
    {
      /* Set the Energe Detect Power down mode */
      if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_CONTROL_1,8,2,mode))
       != MAD_OK)
      {
        MAD_DBG_ERROR("Writing phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
    }
  }
  else
    return MAD_API_COPPER_NOT_SUPPORT;

  return MAD_OK;
}

/*******************************************************************************
* mdSysGetDetectPowerDownMode
*
* DESCRIPTION:
*       This function get modes of Energe Detect Power down.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       mode - MAD_ENERGE_DETECT_MODE_NO if normal, 
*               For chip group 30xx
*              MAD_ENERGE_DETECT_MODE_YES if Setting mode, 
*               For chip group 1xxx.
*              MAD_ENERGE_DETECT_MODE_1 if mode1, 
*              MAD_ENERGE_DETECT_MODE_2 if mode0
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetDetectPowerDownMode
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U16     *mode
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdSysGetDetectPowerDownMode called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_DETECT_POWER_DOWN))
  {
      MAD_DBG_ERROR("Detect Power down does not support.\n");
      return MAD_API_DETECT_PW_DW_NOT_SUPPORT;
  }

  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
    {
      /* Get the Energe Detect Power down mode */
      if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_CONTROL_1,14,1,mode))
       != MAD_OK)
      {
        MAD_DBG_ERROR("Writing phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
    }
    else
    {
      /* Get the Energe Detect Power down mode */
      if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_CONTROL_1,8,2,mode))
          != MAD_OK)
      {
        MAD_DBG_ERROR("Reading phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
      }
    }
  }
  else
    return MAD_API_COPPER_NOT_SUPPORT;

  return MAD_OK;
}

/*******************************************************************************
* mdSysSetSnoopCtrl
*
* DESCRIPTION:
*       This function set modes of Snooping Control.
*
* INPUTS:
*       portNum - Port number to be configured.
*       mode -    MAD_SNOOPING_OFF        
*                MAD_SNOOPING_FROM_NET
*                MAD_SNOOPING_FROM_MAC
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetSnoopCtrl
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN    MAD_U16     mode
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdSysSetSnoopCtrl called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR))
  {
    /* Set the Snooping controln mode */
    if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE6,MAD_REG_GENERAL_CONTROL,10,3,mode))
       != MAD_OK)
    {
      MAD_DBG_ERROR("Writing phy reg failed.\n");
      return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
  }
  else
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;

  return MAD_OK;
}

/*******************************************************************************
* mdSysGetSnoopCtrl
*
* DESCRIPTION:
*       This function get modes of Snooping Control.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       mode -    MAD_SNOOPING_OFF        
*                MAD_SNOOPING_FROM_NET
*                MAD_SNOOPING_FROM_MAC
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetSnoopCtrl
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U16     *mode
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdSysGetDetectPowerDownMode called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR))
  {
    /* Get the Snooping controln mode */
    if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE6,MAD_REG_GENERAL_CONTROL,10,3,mode))
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
* mdSysSetLEDCtrl
*
* DESCRIPTION:
*       This function set LED Control Data.
*
* INPUTS:
*       portNum - Port number to be configured.
*       ledData - ledCtrl[6];
*                  ledPolarity[6];
*                  ledMixPercen0;
*                  ledMixPercen1;
*                  ledForceInt;
*                  ledPulseDuration;
*                  ledBlinkRate;
*                  ledSppedOffPulse_perio;
*                  ledSppedOnPulse_perio;
*                  ledFuncMap3;
*                  ledFuncMap2;
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysSetLEDCtrl
(
    IN  MAD_DEV                *dev,
    IN  MAD_LPORT            port,
    IN    MAD_LED_CTRL_DATA   *ledData
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_U8      page, reg;
  MAD_U16      data;

  MAD_DBG_INFO("mdSysSetLEDCtrl called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY(dev, MAD_PHY_LED_CONTROL)))
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;

  page = MAD_PAGE3;
  reg = MAD_REG_LED_FUNC_CTRL;
  data = ((ledData->ledCtrl[0]&0xf)) | ((ledData->ledCtrl[1]<<4)&0xf0)
        | ((ledData->ledCtrl[2]<<8)&0xf00) | ((ledData->ledCtrl[3]<<12)&0xf000);
  if((retVal=madHwWritePagedPhyReg(dev,(MAD_U8)hwPort,(MAD_U16)page,
                                reg,(MAD_U16)data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Paged Phy Register write failed.\n");
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
  reg = MAD_REG_LED_POLAR_CTRL;
  data = (ledData->ledPolarity[0]&0x3) | ((ledData->ledPolarity[1]<<2)&0xc)
        | ((ledData->ledPolarity[2]<<4)&0x30) | ((ledData->ledPolarity[3]<<6)&0xc0)
        | ((ledData->ledMixPercen0<<8)&0xf00) | ((ledData->ledMixPercen1<<12)&0xf000);
  if((retVal=madHwWritePagedPhyReg(dev,(MAD_U8)hwPort,(MAD_U16)page,
                                reg,(MAD_U16)data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Paged Phy Register write failed.\n");
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
  reg = MAD_REG_LED_TIMER_CTRL;
  data = ((ledData->ledForceInt<<15)&0x8000) | ((ledData->ledPulseDuration<<12)&0x7000)
        | ((ledData->ledBlinkRate<<8)&0x700) | ((ledData->ledSppedOffPulse_perio<<2)&0xc)
        | ((ledData->ledSppedOnPulse_perio)&0x3);
  if((retVal=madHwWritePagedPhyReg(dev,(MAD_U8)hwPort,(MAD_U16)page,
                                reg,(MAD_U16)data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Paged Phy Register write failed.\n");
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
  reg = MAD_REG_LED_FUNC_POLAR_CTRL;
  data = (ledData->ledCtrl[4]&0xf) | ((ledData->ledCtrl[5]<<4)&0xf0)
        | ((ledData->ledPolarity[4]<<8)&0x300) | ((ledData->ledPolarity[5]<<10)&0xc00)
        | ((ledData->ledFuncMap3<<15)&0x8000) | ((ledData->ledFuncMap2<<14)&0x4000);
  if((retVal=madHwWritePagedPhyReg(dev,(MAD_U8)hwPort,(MAD_U16)page,
                                reg,(MAD_U16)data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Paged Phy Register write failed.\n");
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }

    return MAD_OK;
}

/*******************************************************************************
* mdSysGetLEDCtrl
*
* DESCRIPTION:
*       This function Get LED Control Data.
*
* INPUTS:
*       hwPort - Port number to be configured.
*
* OUTPUTS:
*       ledData - ledCtrl[6];
*                  ledPolarity[6];
*                  ledMixPercen0;
*                  ledMixPercen1;
*                  ledForceInt;
*                  ledPulseDuration;
*                  ledBlinkRate;
*                  ledSppedOffPulse_perio;
*                  ledSppedOnPulse_perio;
*                  ledFuncMap3;
*                  ledFuncMap2;
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetLEDCtrl
(
    IN  MAD_DEV                *dev,
    IN  MAD_LPORT            port,
    IN    MAD_LED_CTRL_DATA   *ledData
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_U8      page, reg;
  MAD_U16      data;

  MAD_DBG_INFO("mdSysGetLEDCtrl called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY(dev, MAD_PHY_LED_CONTROL)))
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;

  page = MAD_PAGE3;
  reg = MAD_REG_LED_FUNC_CTRL;
  if((retVal=madHwReadPagedPhyReg(dev,(MAD_U8)hwPort,(MAD_U16)page,
                                reg,&data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Paged Phy Register read failed.\n");
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
  ledData->ledCtrl[0] = data&0xf; 
  ledData->ledCtrl[1] = (data>>4)&0xf;
  ledData->ledCtrl[2] = (data>>8)&0xf;
  ledData->ledCtrl[3] = (data>>12)&0xf;

  reg = MAD_REG_LED_POLAR_CTRL;
  if((retVal=madHwReadPagedPhyReg(dev,(MAD_U8)hwPort,(MAD_U16)page,
                                reg,&data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Paged Phy Register read failed.\n");
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
  ledData->ledPolarity[0] = data&0x3;
  ledData->ledPolarity[1] = (data>>2)&0x3;
  ledData->ledPolarity[2] = (data>>4)&0x3;
  ledData->ledPolarity[3] = (data>>6)&0x3;
  ledData->ledMixPercen0 = (data>>8)&0xf;
  ledData->ledMixPercen1 = (data>>12)&0xf;

  reg = MAD_REG_LED_TIMER_CTRL;
  if((retVal=madHwReadPagedPhyReg(dev,(MAD_U8)hwPort,(MAD_U16)page,
                                reg,&data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Paged Phy Register read failed.\n");
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
  ledData->ledForceInt = (data>>15)&0x1;
  ledData->ledPulseDuration = (data>>12)&0x7;
  ledData->ledBlinkRate = (data>>8)&0x7;
  ledData->ledSppedOffPulse_perio = (data>>2)&0x3;
  ledData->ledSppedOnPulse_perio = data&0x3;

  reg = MAD_REG_LED_FUNC_POLAR_CTRL;
  if((retVal=madHwReadPagedPhyReg(dev,(MAD_U8)hwPort,(MAD_U16)page,
                                reg,&data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Paged Phy Register read failed.\n");
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
  ledData->ledCtrl[4] = data &0xf;
  ledData->ledCtrl[5] = (data>>4)&0xf;
  ledData->ledPolarity[4] = (data>>8)&0x3;
  ledData->ledPolarity[5] = (data>>10)&0x3;
  ledData->ledFuncMap3 = (data>>15)&0x1;
  ledData->ledFuncMap2 = (data>>14)&0x1;

    return MAD_OK;
}

/*******************************************************************************
* mdSysSetUniDirTrans
*
* DESCRIPTION:
*       This function set 802.3ah unidirectioanal transmit.
*
* INPUTS:
*       port    - Port number to be configured.
*       en      - MAD_TRUE to be enabled or MAD_FALSE to be disabled
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
MAD_STATUS mdSysSetUniDirTrans
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_BOOL    en
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdSysSetUniDirTrans called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_UNIDIRECT_TRABSMIT))
  {
    MAD_DBG_ERROR("Does not support Unidirectional transmit \n");
    return MAD_API_UNIDIR_TRANS_NOT_SUPPORT;
  }

  if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
  {
    /* Set Unidirctional transmit bit */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE0, MAD_REG_CONTROL,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_REG);
    }
    if (en==MAD_TRUE)
    {
        data &= ~0x1120;  /* change bit 5,8,12 */
        data |= 0x120;
    }
    else
    {
        data &= ~0x1020;
        data |= 0x1000;
    }
    /* Set Unidirctional transmit bit */
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,MAD_PAGE0, MAD_REG_CONTROL,data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_REG);
    }
    
    
  }
  else
  {
      MAD_U16  page;
      MAD_U8   reg;

    /* Get Unidirctional transmit bit */
    if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
    {
        reg = MAD_REG_COPPER_CONTROL_1;
        page = MAD_PAGE0;
    }
    else if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_FIBER))
    {
        reg = MAD_REG_FIBER_CONTROL_1;
        page = MAD_PAGE1;
    }
    else
        return MAD_API_UNIDIR_TRANS_NOT_SUPPORT;
    if (en==MAD_TRUE)
    {
        data = 1;
    }
    else
    {
        data = 0;
    }
    
    if((retVal = madHwSetPagedPhyRegField(dev,hwPort,page,reg,10,1, data))
       != MAD_OK)
    {
      MAD_DBG_ERROR("Reading phy reg failed.\n");
      return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
  }
  return MAD_OK;
}


/*******************************************************************************
* mdSysGetUniDirTrans
*
* DESCRIPTION:
*       This function get 802.3ah unidirectioanal transmit.
*
* INPUTS:
*       port - Port number to be configured.
*
* OUTPUTS:
*       en   - MAD_TRUE to be enabled or MAD_FALSE to be disabled
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdSysGetUniDirTrans
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdSysGetUniDirTrans called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_UNIDIRECT_TRABSMIT))
  {
    MAD_DBG_ERROR("Does not support Unidirectional transmit \n");
    return MAD_API_UNIDIR_TRANS_NOT_SUPPORT;
  }

  if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
  {
    /* Get Unidirctional transmit bit */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE0, MAD_REG_CONTROL,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_REG);
    }
    if (data & 0x20)
    {
        *en = MAD_TRUE;
    }
    else
    {
        *en = MAD_FALSE;
    }
  }
  else
  {
      MAD_U16  page;
      MAD_U8   reg;

    /* Get Unidirctional transmit bit */
    if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
    {
        reg = MAD_REG_COPPER_CONTROL_1;
        page = MAD_PAGE0;
    }
    else if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_FIBER))
    {
        reg = MAD_REG_FIBER_CONTROL_1;
        page = MAD_PAGE1;
    }
    else
        return MAD_API_UNIDIR_TRANS_NOT_SUPPORT;

    if((retVal = madHwGetPagedPhyRegField(dev,hwPort,page,reg,10,1, &data))
       != MAD_OK)
    {
      MAD_DBG_ERROR("Reading phy reg failed.\n");
      return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }


    if (data)
    {
        *en = MAD_TRUE;
    }
    else
    {
        *en = MAD_FALSE;
    }
    

  }
  return MAD_OK;
}

