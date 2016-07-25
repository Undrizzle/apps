#include <madCopyright.h>

/********************************************************************************
* madCpCtrl.c
* 
* DESCRIPTION:
*       APIs to control Copper.
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
* mdCopperSetAutoNeg
*
* DESCRIPTION:
*       This routine enables or disables Copper Auto-Neg with the specified mode.
*       When enables Auto-Neg, advertisement register value is updated with the  
*       combination of the followings:
*           Asym Pause, Pause, 100FDX, 100HDX, 10FDX, 10HDX,
*           Master/Slave Manual, Force Master, Port Type, 1000FDX, and 1000HDX
*       When disables Auto-Neg, only one of the following modes is supported:
*           100FDX, 100HDX, 10FDX, or 10HDX.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE to disable
*       mode - if en is MAD_TRUE, combination of the followings:
*                   MAD_AUTO_AD_ASYM_PAUSE,
*                   MAD_AUTO_AD_PAUSE,
*                   MAD_AUTO_AD_100FDX,
*                   MAD_AUTO_AD_100HDX,
*                   MAD_AUTO_AD_10FDX,
*                   MAD_AUTO_AD_10HDX,
*                   MAD_AUTO_AD_1000FDX,
*                   MAD_AUTO_AD_1000HDX,
*                   MAD_AUTO_AD_MANUAL_CONF_MS,
*                   MAD_AUTO_AD_FORCE_MASTER, and
*                   MAD_AUTO_AD_PREFER_MULTI_PORT
*              if en is MAD_FALSE,
*                   MAD_PHY_1000FDX,
*                   MAD_PHY_1000HDX,
*                   MAD_PHY_100FDX,
*                   MAD_PHY_100HDX,
*                   MAD_PHY_10FDX, or
*                   MAD_PHY_10HDX 
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15,13,12,8 : Soft reset,Speed,Auto-neg enable,Duplex
*       Page 0, Reg 4, Bit 11,10,8:5  : Advertisement Register
*       Page 0, Reg 9, Bit 12:8       : 1000 Base-T Control Register
*
*******************************************************************************/
MAD_STATUS mdCopperSetAutoNeg
(
    IN  MAD_DEV   *dev,
    IN  MAD_LPORT port,
    IN  MAD_BOOL  en,
    IN  MAD_U32   mode
)
{
    MAD_U16     data, macData=0;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperSetAutoNeg called.\n");

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

    /* Enable Auto-Nego */
    if (en)
    {

        /* Check if the given mode is valid */
        if (mode & ~MAD_AUTO_AD_MASK)
        {
            /* unknown mode is set */
            MAD_DBG_ERROR("Unknown AutoNeg mode(%#x) is set.\n", (unsigned int)(mode & ~MAD_AUTO_AD_MASK));
            return MAD_API_UNKNOWN_AUTONEG_MODE;
        }

        /* modify Auto-Neg Ad Register */
        if((retVal = madHwReadPagedPhyReg(dev,hwPort,0,MAD_REG_AUTO_AD,&data))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg (%i,%i) failed.\n",0,MAD_REG_AUTO_AD);
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }
        
        data = (data & ~MAD_AUTO_AD_MASK_REG4) | ((MAD_U16)(mode & 0xFFFF));

        if((retVal = madHwWritePagedPhyReg(dev,hwPort,0,MAD_REG_AUTO_AD,data))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",0,MAD_REG_AUTO_AD);
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
      /* if ((dev->deviceId!=MAD_88E3016)&&(dev->deviceId!=MAD_88E3082))  */
      if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
          /* E3016/E3082 has no 1000M */
      {

        /* modify 1000B-T Control Reg */
        if((retVal = madHwReadPagedPhyReg(dev,hwPort,0,MAD_REG_1000_CONTROL,&data))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg (%i,%i) failed.\n",0,MAD_REG_1000_CONTROL);
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        data = ((MAD_U16)((data & ~(MAD_AUTO_AD_MASK_REG9>>16)) | ((mode >> 16) & 0xFFFF)));

        if((retVal = madHwWritePagedPhyReg(dev,hwPort,0,MAD_REG_1000_CONTROL,data))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",0,MAD_REG_1000_CONTROL);
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
        /* bits to set to Control Register */
        /* data = MAD_REG_CTRL_RESET|MAD_REG_CTRL_AUTONEGO; */
        data = MAD_REG_CTRL_AUTONEGO|MAD_REG_CTRL_DUPLEX|MAD_REG_CTRL_SPEED_MSB;
        macData=1;
      }
      else
      {
        /* bits to set to Control Register */
        /* data = MAD_REG_CTRL_RESET|MAD_REG_CTRL_AUTONEGO; */
        data = MAD_REG_CTRL_AUTONEGO|MAD_REG_CTRL_DUPLEX|MAD_REG_CTRL_SPEED;
      }
    }
    else
    {
        /* Force Speed and Duplex */
        switch (mode)
        {
            case MAD_PHY_1000FDX:
                    data = MAD_REG_CTRL_SPEED_MSB|MAD_REG_CTRL_DUPLEX;
                    break;
            case MAD_PHY_1000HDX:
                    data = MAD_REG_CTRL_SPEED_MSB;
                    break;
            case MAD_PHY_100FDX:
                    data = MAD_REG_CTRL_SPEED|MAD_REG_CTRL_DUPLEX;
                    break;
            case MAD_PHY_100HDX:
                    data = MAD_REG_CTRL_SPEED;
                    break;
            case MAD_PHY_10FDX:
                    data = MAD_REG_CTRL_DUPLEX;
                    break;
            case MAD_PHY_10HDX:
                    data = 0;
                    break;
            default:
                    /* unknown mode is set */
                    MAD_DBG_ERROR("Unknown mode %i.\n",(int)mode);
                    return MAD_API_UNKNOWN_SPEED_MODE;
        }
        macData = 0;

    }

  /* if ((dev->deviceId==MAD_88E3016)||(dev->deviceId==MAD_88E3082))  */
  if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
  /* E3016/E3082 has no 1000M */
  {
    /* Set control parameter and soft reset */
    if((retVal = madHwPagedSetCtrlPara(dev,hwPort,0, data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Set control register failed.\n");
        return (retVal | MAD_API_FAIL_SET_CTRL_REG);
    }
  }
  else
  {
    /* Set Control Register */
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,0,MAD_REG_CONTROL,data))
        != MAD_OK)
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
    if(MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR))
    {
      /* Set MAC SGMII or QSGMII auto nego en/disable for 1340 */
      if (dev->phyInfo.hwMode[port] & MAD_PHY_MODE_SGMII)
      {
        if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE1,0, 12,1, macData)) != MAD_OK)
        {
          MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
          return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        /* Soft Reset */
        if((retVal = mdSysModSoftReset(dev, hwPort, MAD_RESET_MODE_PORT_FIBER)) != MAD_OK)
        {
          MAD_DBG_ERROR("Soft Reset failed.\n");
          return (retVal | MAD_API_FAIL_SW_RESET);
        }
      }
      else if (dev->phyInfo.hwMode[port] & MAD_PHY_MODE_QSGMII)
      {
        if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,MAD_PAGE4,0, 12,1, macData)) != MAD_OK)
        {
          MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
          return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        /* Soft Reset */
        if((retVal = mdSysModSoftReset(dev, hwPort, MAD_RESET_MODE_PORT_QSGMII)) != MAD_OK)
        {
          MAD_DBG_ERROR("Soft Reset failed.\n");
          return (retVal | MAD_API_FAIL_SW_RESET);
        }
      }
    }
  }



    return MAD_OK;
}


/*******************************************************************************
* mdCopperGetAutoNeg
*
* DESCRIPTION:
*       This routine retrieves Copper Auto-Neg mode.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*       mode - if en is MAD_TRUE, combination of the followings:
*                   MAD_AUTO_AD_ASYM_PAUSE,
*                   MAD_AUTO_AD_PAUSE,
*                   MAD_AUTO_AD_100FDX,
*                   MAD_AUTO_AD_100HDX,
*                   MAD_AUTO_AD_10FDX,
*                   MAD_AUTO_AD_10HDX,
*                   MAD_AUTO_AD_1000FDX,
*                   MAD_AUTO_AD_1000HDX,
*                   MAD_AUTO_AD_MANUAL_CONF_MS,
*                   MAD_AUTO_AD_FORCE_MASTER, and
*                   MAD_AUTO_AD_PREFER_MULTI_PORT
*              if en is MAD_FALSE,
*                   MAD_PHY_1000FDX,
*                   MAD_PHY_1000HDX,
*                   MAD_PHY_100FDX,
*                   MAD_PHY_100HDX,
*                   MAD_PHY_10FDX, or
*                   MAD_PHY_10HDX 
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 13,12,8   : Speed, Auto-neg enable, and Duplex
*       Page 0, Reg 4, Bit 11,10,8:5 : Advertisement Register
*       Page 0, Reg 9, Bit 12:8      : 1000 Base-T Control Register
*
*******************************************************************************/
MAD_STATUS mdCopperGetAutoNeg
(
    IN  MAD_DEV   *dev,
    IN  MAD_LPORT port,
    OUT MAD_BOOL  *en,
    OUT MAD_U32   *mode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperGetAutoNeg called.\n");

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

    /* read Control Register */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,0,MAD_REG_CONTROL,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg (%i,%i) failed.\n",0,MAD_REG_AUTO_AD);
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* check if Auto-Nego is enabled */
    if (data & MAD_REG_CTRL_AUTONEGO)
    {
        /* read Auto-Neg Ad Register */
        if((retVal = madHwReadPagedPhyReg(dev,hwPort,0,MAD_REG_AUTO_AD,&data))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg (%i,%i) failed.\n",0,MAD_REG_AUTO_AD);
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }



        *mode = (MAD_U32)data & MAD_AUTO_AD_MASK_REG4;

      /* if ((dev->deviceId!=MAD_88E3016)&&(dev->deviceId!=MAD_88E3082))  */
      if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
          /* E3016/E3082 has no 1000M */
      {

        /* read 1000B-T Config Register */
        if((retVal = madHwReadPagedPhyReg(dev,hwPort,0,MAD_REG_1000_CONTROL,&data))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Reading to paged phy reg (%i,%i) failed.\n",0,MAD_REG_AUTO_AD);
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        *mode |= ((MAD_U32)data << 16) & MAD_AUTO_AD_MASK_REG9;
      }

        *en = MAD_TRUE;
    }
    else
    {
        /* Auto-Nego is disabled. */
        switch (data & (MAD_REG_CTRL_SPEED|MAD_REG_CTRL_DUPLEX|MAD_REG_CTRL_SPEED_MSB))
        {
            case 0:
                *mode = MAD_PHY_10HDX;
                break;
            case MAD_REG_CTRL_SPEED:
                *mode = MAD_PHY_100HDX;
                break;
            case MAD_REG_CTRL_DUPLEX:
                *mode = MAD_PHY_10FDX;
                break;
			case (MAD_REG_CTRL_SPEED|MAD_REG_CTRL_DUPLEX):
                *mode = MAD_PHY_100FDX;
                break;
			case (MAD_REG_CTRL_SPEED_MSB|MAD_REG_CTRL_DUPLEX):
                *mode = MAD_PHY_1000FDX;
                 break;
			case (MAD_REG_CTRL_SPEED_MSB):
                *mode = MAD_PHY_1000HDX;
                 break;
            default:
                *mode = MAD_PHY_1000FDX;  /* temprary */
                 break;
		}

        *en = MAD_FALSE;
    }

    return MAD_OK;
}

/*******************************************************************************
* mdCopperSetAutoRestart
*
* DESCRIPTION:
*       This routine restarts Copper Auto-Neg.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*       MAD_BAD_PARAM   - on bad parameters
*
* COMMENTS:
*       Page 0, Reg 0, Bit 9 : Copper Control 0 (MDI Crossover)
*
*******************************************************************************/
MAD_STATUS mdCopperSetAutoRestart
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port
)
{
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperSetAutoRestart called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %i is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
    {
        MAD_DBG_ERROR("Copper not supported.\n");
        return MAD_API_COPPER_NOT_SUPPORT;
    }

    /* Set Copper Control Register 0 Bit 9*/
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_CONTROL,
                    (MAD_U8)9,1,1)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_CONTROL,
                    (MAD_U8)9,1,0)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    return MAD_OK;
}

/*******************************************************************************
* mdGetAutoNegoMode
*
* DESCRIPTION:
*       This routine transltes auto nego status, speed, and dyplex mode into auto nego mode.
*
* INPUTS:
*        en            - Auto nego enble/disable.
*        speedMode    - MAD_SPEED_MODE.
*        duplexMode    - MAD_DUPLEX_MODE.
* OUTPUTS:
*       mode        - Auto nego mode.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*       MAD_BAD_PARAM   - on bad parameters
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS mdGetAutoNegoMode
(
    IN    MAD_BOOL        autoNegoEn, 
    IN    MAD_SPEED_MODE    speedMode, 
    IN    MAD_DUPLEX_MODE duplexMode, 
    OUT    MAD_U32            *autoMode
)
{
  if(autoNegoEn==MAD_TRUE)  /* Auto nego enabled */
  {
    if (duplexMode==MAD_AUTO_DUPLEX) /* it is auto Duplex */
    {
        switch (speedMode)
        {
            case MAD_SPEED_10M:
                *autoMode = MAD_AN_MOD_10_AUTO_DUPLEX;
                break;
            case MAD_SPEED_100M:
                *autoMode = MAD_AN_MOD_100_AUTO_DUPLEX;
                break;
            case MAD_SPEED_1000M:
                *autoMode = MAD_AN_MOD_1000_AUTO_DUPLEX;
                break;
            case MAD_SPEED_AUTO:
                *autoMode = MAD_AN_MOD_AUTO_SPEED_AUTO_DUPLEX;
                break;
            default:
            /* MSG_PRINT("Get wrong speed status error\t");  */
                return MAD_FAIL;
        }
    }
    else if (speedMode==MAD_SPEED_AUTO) /* it is suto speed */
    {
         switch (duplexMode)
        {
            case MAD_FULL_DUPLEX:
                *autoMode = MAD_AN_MOD_AUTO_SPEED_DUPLEX_FULL;
                break;
            case MAD_HALF_DUPLEX:
                *autoMode = MAD_AN_MOD_AUTO_SPEED_DUPLEX_HALF;
                break;
            case MAD_AUTO_DUPLEX:
                *autoMode = MAD_AN_MOD_AUTO_SPEED_AUTO_DUPLEX;
                break;
            default:
            /* MSG_PRINT("Get wrong speed status error\t");  */
                return MAD_FAIL;
        }
    }
	else  /* it is auto nego speed and duplex mode */
	{
		*autoMode = MAD_AN_MOD_AUTO_SPEED_AUTO_DUPLEX;
	}
  }
  else /* Auto nego disabled */
  {
    if (duplexMode==MAD_FULL_DUPLEX) /* it is Full Duplex */
    {
            switch (speedMode)
          {
            case MAD_SPEED_10M:
                *autoMode = MAD_PHY_10FDX;
                break;
            case MAD_SPEED_100M:
                *autoMode = MAD_PHY_100FDX;
                break;
            case MAD_SPEED_1000M:
                *autoMode = MAD_PHY_1000FDX;
                break;
            default:
                /* MSG_PRINT("Get wrong speed status error\t");  */
                return MAD_FAIL;
          }
    }
    else /* it is Half Duplex */
    {
          switch (speedMode)
          {
             case MAD_SPEED_10M:
                *autoMode = MAD_PHY_10HDX;
                break;
            case MAD_SPEED_100M:
                *autoMode = MAD_PHY_100HDX;
                break;
            case MAD_SPEED_1000M:
                *autoMode = MAD_PHY_1000HDX;
                break;
            default:
                /* MSG_PRINT("Get wrong speed status error\t");  */
                 return MAD_FAIL;
          }
    }
  }
  return MAD_OK;
}


/*******************************************************************************
* mdCopperSetMDIMode
*
* DESCRIPTION:
*       This routine sets Copper MDI Crossover modes.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       mode - MAD_MDI_MODE:
*                   MAD_AUTO_MDI_MDIX,
*                   MAD_FORCE_MDI, or
*                   MAD_FORCE_MDIX
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15   : Soft reset
*       Page 0, Reg 16, Bit 6:5 : Copper Control 1 (MDI Crossover)
*
*******************************************************************************/
MAD_STATUS mdCopperSetMDIMode
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    IN  MAD_MDI_MODE    mode
)
{
    MAD_U16     data, regLoc;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperSetMDIMode called.\n");

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

    switch (mode)
    {
        case MAD_AUTO_MDI_MDIX:
            data = 3;
            break;
        case MAD_FORCE_MDI:
            data = 0;
            break;
        case MAD_FORCE_MDIX:
            data = 1;
            break;
        default:
            MAD_DBG_ERROR("Unknown MDI Crossover mode is set.\n");
            return MAD_API_UNKNOWN_MDI_X_MODE;
    }

    regLoc=5;
    /* if ((dev->deviceId==MAD_88E3016)||(dev->deviceId==MAD_88E3082))  */
    if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
        regLoc=4;

    /* Set Copper Control Register 1 */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_COPPER_CONTROL_1,
                    (MAD_U8)(regLoc&0xff),2,data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
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
* mdCopperGetMDIMode
*
* DESCRIPTION:
*       This routine gets Copper MDI Crossover modes.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       mode - MAD_MDI_MODE:
*                   MAD_AUTO_MDI_MDIX,
*                   MAD_FORCE_MDI, or
*                   MAD_FORCE_MDIX
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 16, Bit 6:5 : Copper Control 1 (MDI Crossover)
*
*******************************************************************************/
MAD_STATUS mdCopperGetMDIMode
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    OUT MAD_MDI_MODE    *mode
)
{
    MAD_U16     data, regLoc;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperGetMDIMode called.\n");

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

    regLoc=5;
    /* if ((dev->deviceId==MAD_88E3016)||(dev->deviceId==MAD_88E3082))  */
    if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
        regLoc=4;


    /* Get the required bits in Copper Control Register 1 */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_COPPER_CONTROL_1,
                    (MAD_U8)(regLoc&0xff),2,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }

    switch (data)
    {
        case 0 : 
            *mode = MAD_FORCE_MDI;
            break;
        case 1 : 
            *mode = MAD_FORCE_MDIX;
            break;
        case 3 : 
            *mode = MAD_AUTO_MDI_MDIX;
            break;
        default:
            MAD_DBG_ERROR("Unknown MDI Crossover mode is set.\n");
            return MAD_API_UNKNOWN_MDI_X_MODE;
    }

    return MAD_OK;
}


/*******************************************************************************
* mdCopperSetDownshiftEnable
*
* DESCRIPTION:
*       This routine enables or disables Downshift.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE otherwise
*       count  - Number of Downshift counter
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15  : Soft reset
*       Page 0, Reg 16, Bit 11 : Copper Control 1 (Downshift enable)
*       or
*       Page 0, Reg 20, Bit 8 :  Extended PHY Specific Control Register(Downshift enable)
*
*******************************************************************************/
MAD_STATUS mdCopperSetDownshiftEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_BOOL    en,
    IN  MAD_U16      count
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperSetDownshiftEnable called.\n");

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
  
    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_DW_SHIFT_CAPABLE))
    {
        MAD_DBG_ERROR("Down shift not supported.\n");
        return MAD_API_DOWNSHIFT_NOT_SUPPORT;
    }

    /* Enable or disable */
    MAD_BOOL_2_BIT(en,data);

    switch (dev->phyInfo.dwshiftType)
    {
        case MAD_PHY_DOWNSHIFT_TYPE1:   
           /* Set Extended PHY Specific Control Register */
            if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,20,
                    8,1,data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,20,
                    9,3,count)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            break;
            
        case MAD_PHY_DOWNSHIFT_TYPE2:   
           /* Set Copper Control Register 1 */
            if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_COPPER_CONTROL_1,
                    11,1,data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_COPPER_CONTROL_1,
                    12,3,count)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            break;

           default:
            return MAD_API_DOWNSHIFT_NOT_SUPPORT;

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
* mdCopperGetDownshiftEnable
*
* DESCRIPTION:
*       This routine gets if Downshift is enabled.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*       count  - Number of Downshift counter
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 16, Bit 11 : Copper Control 1 (Downshift enable)
*       or
*       Page 0, Reg 20, Bit 8 :  Extended PHY Specific Control Register(Downshift enable)
*
*******************************************************************************/
MAD_STATUS mdCopperGetDownshiftEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en,
    OUT MAD_U16     *count
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperGetDownshiftEnable called.\n");

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

    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_DW_SHIFT_CAPABLE))
    {
        MAD_DBG_ERROR("Down shift not supported.\n");
        return MAD_API_DOWNSHIFT_NOT_SUPPORT;
    }

    switch (dev->phyInfo.dwshiftType)
    {
        case MAD_PHY_DOWNSHIFT_TYPE1:   
           /* Get the required bits in Extended PHY Specific Control Register */
            if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,20,
                    8,1,&data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
            }
            if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,20,
                    9,3,count)) != MAD_OK)
            {
                MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
            }
            break;

        case MAD_PHY_DOWNSHIFT_TYPE2:   
           /* Get the required bits in Copper Control Register 1 */
            if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_COPPER_CONTROL_1,
                    11,1,&data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
            }
            if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_COPPER_CONTROL_1,
                    12,3,count)) != MAD_OK)
            {
                MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
            }
            break;

           default:
            return MAD_API_DOWNSHIFT_NOT_SUPPORT;

    }

    /* Enabled or disabled */
    MAD_BIT_2_BOOL(data,*en);

    return MAD_OK;
}

static
MAD_STATUS dteWorkAround_Phy100M
(
    IN  MAD_DEV *dev,
    IN  MAD_U8            hwPort
)
{
    MAD_STATUS status = MAD_OK;
    MAD_U32 threshold[] = {0x000B,0x0000,0x8780,0x0000,0x8F80,0x0000,
                          0x9780,0x0000,0x9F80,0x0000,0xA780,0x0000,
                          0xAF80,0x0000,0xB780,0x0000,0xBF80,0x0000,
                          0xC780,0x0000,0xCF80,0x0000,0xD780,0x0000,
                          0xDF80,0x0000,0xE780,0x0000,0xEF80,0x0000,
                          0xF780,0x0000,0xFF80,0x0000};
    int i, thresholdSize;

    /* force r125 clock */
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1D,0x0003)) != MAD_OK)
    {
        return status;
    }
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1E,0x807f)) != MAD_OK)
    {
        return status;
    }

    /* write thresholds */
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1D,0x000B)) != MAD_OK)
    {
        return status;
    }

    thresholdSize = sizeof(threshold)/sizeof(MAD_U32);

    for(i=0; i<thresholdSize; i++)
    {
        if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1E,(MAD_U16)threshold[i])) != MAD_OK)
        {
            return status;
        }
    }

    /* setting adc Masking */
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1D,0x0001)) != MAD_OK)
    {
        return status;
    }
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1E,0x4000)) != MAD_OK)
    {
        return status;
    }

    /* setting noise level */
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1D,0x0005)) != MAD_OK)
    {
        return status;
    }
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1E,0xA000)) != MAD_OK)
    {
        return status;
    }

    /* 
        offseting cable length measurement by 6.72m(2*4*0.84m)
        set 30_10.14:11 to 0x1001 for cable length measure.
    */ 
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1D,0x000a)) != MAD_OK)
    {
        return status;
    }
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1E,0x4840)) != MAD_OK)
    {
        return status;
    }

    /* release force r125 clock */
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1D,0x0003)) != MAD_OK)
    {
        return status;
    }
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1E,0x0000)) != MAD_OK)
    {
        return status;
    }


    return status;
}

static
MAD_STATUS dteWorkAround_Phy1000M
(
    IN  MAD_DEV *dev,
    IN  MAD_U8            hwPort
)
{
    MAD_STATUS status = MAD_OK;
    MAD_U32 threshold[] = {0x0000,0x8780,0x0000,0x8F80,0x0000,0x9780,
                          0x0000,0x9F80,0x0000,0xA780,0x0000,0xAF80,
                          0x0000,0xB780,0x0000,0xBF80,0x0000,0xC780,
                          0x0000,0xCF80,0x0000,0xD780,0x0000,0xDF80,
                          0x0000,0xE780,0x0000,0xEF80,0x0000,0xF780,
                          0x0000,0xFF80,0x0000};
    int i, thresholdSize;

    /*  */
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1D,0x001B)) != MAD_OK)
    {
        return status;
    }
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1E,0x43FF)) != MAD_OK)
    {
        return status;
    }

    /*  */
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1D,0x001C)) != MAD_OK)
    {
        return status;
    }
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1E,0x9999)) != MAD_OK)
    {
        return status;
    }

    /*  */
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1D,0x001F)) != MAD_OK)
    {
        return status;
    }
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1E,0xE00C)) != MAD_OK)
    {
        return status;
    }

    /*  */
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1D,0x0018)) != MAD_OK)
    {
        return status;
    }
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1E,0xFFA1)) != MAD_OK)
    {
        return status;
    }

    /* write thresholds */
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1D,0x0010)) != MAD_OK)
    {
        return status;
    }

    thresholdSize = sizeof(threshold)/sizeof(MAD_U32);

    for(i=0; i<thresholdSize; i++)
    {
        if((status= madHwWritePagedPhyReg(dev,hwPort,0,0x1E,(MAD_U16)threshold[i])) != MAD_OK)
        {
            return status;
        }
    }

    return status;
}

/*******************************************************************************
* mdCopperSetDTEDetectEnable
*
* DESCRIPTION:
*       This routine enables or disables DTE (such as IP Phone) detect.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE otherwise
*       dropHys   - DTE detect status drop hysteresis
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15 : Soft reset
*       Page 0, Reg 26, Bit 8 : Copper Control 2 (DTE Detect)
*       or
*       Page 0, Reg 20, Bit 2 :  Extended PHY Specific Control Register(Downshift enable)
*
*******************************************************************************/
MAD_STATUS mdCopperSetDTEDetectEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_BOOL    en,
    IN  MAD_U16     dropHys
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperSetDTEDetectEnable called.\n");

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

    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_DTE_CAPABLE))
    {
        MAD_DBG_ERROR("DTE not supported.\n");
        return MAD_API_DTEDETECT_NOT_SUPPORT;
    }

    /* Enable or disable */
    MAD_BOOL_2_BIT(en,data);

    switch (dev->phyInfo.dteType)
    {
        case MAD_PHY_DTE_TYPE1:  
            /* Set Extended PHY Specific Control Register */
            if((retVal = madHwSetPhyRegField(
                    dev,hwPort,16,
                    15,1,data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_REG_FIELD);
            }
            if((retVal = madHwSetPhyRegField(
                    dev,hwPort,22,
                    12,4,dropHys)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_REG_FIELD);
            }
            if((retVal = dteWorkAround_Phy100M(dev,hwPort)) != MAD_OK)
            {
                MAD_DBG_ERROR("dreWorkAround failed.\n");
                return retVal;
            }
            break;

        case MAD_PHY_DTE_TYPE2:   
            if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,20,
                    2,1,data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,27,
                    5,4,dropHys)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            break;

        case MAD_PHY_DTE_TYPE3:   
            /* Set Extended PHY Specific Control Register */
            if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,20,
                    2,1,data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,27,
                    5,4,dropHys)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            if((retVal = dteWorkAround_Phy1000M(dev,hwPort)) != MAD_OK)
            {
                MAD_DBG_ERROR("dreWorkAround failed.\n");
                return retVal;
            }
            break;

        case MAD_PHY_DTE_TYPE4:   /* after 1112 */
            /* Set Copper Control Register 3 */
            if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_COPPER_CONTROL_3,
                    8,1,data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_COPPER_CONTROL_3,
                    4,4,dropHys)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            break;

        case MAD_PHY_DTE_TYPE5:  
            /* Set Extended PHY Specific Control Register */
            if((retVal = madHwSetPhyRegField(
                    dev,hwPort,16,
                    15,1,data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_REG_FIELD);
            }
            if((retVal = madHwSetPhyRegField(
                    dev,hwPort,22,
                    12,4,dropHys)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_REG_FIELD);
            }
            break;

           default:
            return MAD_API_DTEDETECT_NOT_SUPPORT;

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
* mdCopperGetDTEDetectEnable
*
* DESCRIPTION:
*       This routine gets if DTE Detect is enabled.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*       dropHys   - DTE detect status drop hysteresis
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 26, Bit 8 : Copper Control 2 (DTE Detect)
*       or
*       Page 0, Reg 20, Bit 2 :  Extended PHY Specific Control Register(Downshift enable)
*
*******************************************************************************/
MAD_STATUS mdCopperGetDTEDetectEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en,
    OUT MAD_U16     *dropHys
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperGetDTEDetectEnable called.\n");

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

    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_DTE_CAPABLE))
    {
        MAD_DBG_ERROR("DTE not supported.\n");
        return MAD_API_DTEDETECT_NOT_SUPPORT;
    }

    switch (dev->phyInfo.dteType)
    {
        case MAD_PHY_DTE_TYPE2:   
        case MAD_PHY_DTE_TYPE3:   
         /* Get the required bits in Extended PHY Specific Control Register */
            if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,20,
                    2,1,&data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
            }
            if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,27,
                    5,4,dropHys)) != MAD_OK)
            {
                MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
            }
            break;

        case MAD_PHY_DTE_TYPE1:   
         /* Get the required bits in Extended PHY Specific Control Register */
            if((retVal = madHwGetPhyRegField(
                    dev,hwPort,16,
                    15,1,&data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_GET_REG_FIELD);
            }
            if((retVal = madHwSetPhyRegField(
                    dev,hwPort,22,
                    12,4,*dropHys)) != MAD_OK)
            {
                MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_REG_FIELD);
            }
            break;

        case MAD_PHY_DTE_TYPE4:   
         /* Get the required bits in Copper Control Register 3(0.26,before 1340: control reg 2 */
            if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_COPPER_CONTROL_3,
                    8,1,&data)) != MAD_OK)
            {
                MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
            }
            if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_COPPER_CONTROL_3,
                    4,4,dropHys)) != MAD_OK)
            {
                MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
            }
            break;

           default:
            return MAD_API_DTEDETECT_NOT_SUPPORT;

    }


    /* Enabled or disabled */
    MAD_BIT_2_BOOL(data,*en);

    return MAD_OK;
}

/*******************************************************************************
* mdCopperSetLinkDownDelay
*
* DESCRIPTION:
*       This function set Gigabit Link Down Delay.
*
* INPUTS:
*       portNum - Port number to be configured.
*       delay -    MAD_1G_LINKDOWN_DELAY_0  (0ms)
*                MAD_1G_LINKDOWN_DELAY_10 (10 +/- 2ms)
*                MAD_1G_LINKDOWN_DELAY_20 (20 +/- 2ms)
*                MAD_1G_LINKDOWN_DELAY_40 (40 +/- 2ms)
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
MAD_STATUS mdCopperSetLinkDownDelay
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN    MAD_U16     delay
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdCopperSetLinkDownDelay called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_1G_LINK_DOWN_INDICAT))
  {
     MAD_DBG_ERROR("Link down delay is not supported.\n");
     return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR("Copper not supported.\n");
    return MAD_API_COPPER_NOT_SUPPORT;
  }

  /* Set Link down delay */
  if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_CONTROL_3,10,2,delay))
       != MAD_OK)
  {
    MAD_DBG_ERROR("Writing phy reg failed.\n");
    return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }

  return MAD_OK;
}

/*******************************************************************************
* mdCopperGetLinkDownDelay
*
* DESCRIPTION:
*       This function get Gigabit Link Down Delay.
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       delay -    MAD_1G_LINKDOWN_DELAY_0  (0ms)
*                MAD_1G_LINKDOWN_DELAY_10 (10 +/- 2ms)
*                MAD_1G_LINKDOWN_DELAY_20 (20 +/- 2ms)
*                MAD_1G_LINKDOWN_DELAY_40 (40 +/- 2ms)
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdCopperGetLinkDownDelay
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_U16     *delay
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdCopperGetLinkDownDelay called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY1(dev, MAD_PHY_1G_LINK_DOWN_INDICAT)))
  {
     MAD_DBG_ERROR("Link down delay is not supported.\n");
     return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR("Copper not supported.\n");
    return MAD_API_COPPER_NOT_SUPPORT;
  }

  /* Get Link down delay */
  if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_CONTROL_3,10,2,delay))
       != MAD_OK)
  {
    MAD_DBG_ERROR("Reading phy reg failed.\n");
    return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }

  return MAD_OK;
}

/*******************************************************************************
* mdCopperSetLinkDownDelayEnable
*
* DESCRIPTION:
*       This function set enable of Gigabit Link Down Delay.
*
* INPUTS:
*       portNum - Port number to be configured.
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
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
MAD_STATUS mdCopperSetLinkDownDelayEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    IN  MAD_BOOL    en
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_U16     data;

  MAD_DBG_INFO("mdCopperSetLinkDownDelayEnable called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY1(dev, MAD_PHY_1G_LINK_DOWN_INDICAT)))
  {
     MAD_DBG_ERROR("Link down delay is not supported.\n");
     return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR("Copper not supported.\n");
    return MAD_API_COPPER_NOT_SUPPORT;
  }
    MAD_BOOL_2_BIT(en,data);

  /* Set Link down delay enable*/
  if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_CONTROL_3,9,1,data))
       != MAD_OK)
  {
    MAD_DBG_ERROR("Writing phy reg failed.\n");
    return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }

  return MAD_OK;
}

/*******************************************************************************
* mdCopperGetLinkDownDelayEnable
*
* DESCRIPTION:
*       This function get Gigabit Link Down Delay enable status
*
* INPUTS:
*       portNum - Port number to be configured.
*
* OUTPUTS:
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS mdCopperGetLinkDownDelayEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_BOOL    *en
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_U16     data;

  MAD_DBG_INFO("mdCopperGetLinkDownDelayEnable called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {  
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY1(dev, MAD_PHY_1G_LINK_DOWN_INDICAT)))
  {
     MAD_DBG_ERROR("Link down delay is not supported.\n");
     return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR("Copper not supported.\n");
    return MAD_API_COPPER_NOT_SUPPORT;
  }

  /* Get Link down delay enable*/
  if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_CONTROL_3,9,1,&data))
       != MAD_OK)
  {
    MAD_DBG_ERROR("Reading phy reg failed.\n");
    return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }
    
  MAD_BIT_2_BOOL(data,*en);

  return MAD_OK;
}

/*******************************************************************************
* mdCopperSetSpeedDuplex
*
* DESCRIPTION:
*       Set Phy speed and duplex.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - port number.
*       speed   -  speed 10, 100, 1000.  
*       duplex   - Duplex MAD_TRUE, Half MAD_FALSE.  
*   
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/

MAD_STATUS mdCopperSetSpeedDuplex 
(
    IN  MAD_DEV    *dev,
    IN  int         port,
    IN  MAD_U32    speed,
    IN  MAD_BOOL   duplex
)
{
  MAD_STATUS   retVal;
  MAD_U32      mode;

  switch (speed)
  {
    case 10:
        if (duplex==MAD_TRUE)
            mode = MAD_AUTO_AD_10FDX;
        else
            mode = MAD_AUTO_AD_10HDX;
        break;
    case 100:
        if (duplex==MAD_TRUE)
            mode = MAD_AUTO_AD_100FDX;
        else
            mode = MAD_AUTO_AD_100HDX;
        break;
    case 1000:
    default:
        if (duplex==MAD_TRUE)
            mode = MAD_AUTO_AD_1000FDX;
        else
            mode = MAD_AUTO_AD_1000HDX;
        break;
  }

  retVal = mdCopperSetAutoNeg(dev,port,MAD_TRUE, mode);
  if (retVal != MAD_OK)
  {
    MAD_DBG_ERROR("Setting for (port %i, mode %i) returned ",(int)port,(int)mode);
    return retVal;             
  }
#ifdef DEBUG
  {
      MAD_BOOL en;
      MAD_U32  chkMode;

    retVal = mdCopperGetAutoNeg(dev,port, &en, &chkMode);
    if (retVal != MAD_OK)
    {
      MAD_DBG_ERROR("Failed Setting for (port %i, mode %i) \n",(int)port,(int)chkMode);
      return retVal;             
    }
    MAD_CRITIC_INFO("Set AutoNeg enable %x get %x\n", en, MAD_TRUE);
    MAD_CRITIC_INFO("Set AutoNeg Mode %x get %x\n", (unsigned int)chkMode, (unsigned int)mode);
  }

#endif

  return MAD_OK;
}

/*******************************************************************************
* mdCopperSet1000TMasterMode
*
* DESCRIPTION:
*       This routine sets the ports 1000Base-T Master mode and restart the Auto
*        negotiation.
* INPUTS:
*       port - the logical port number.
*       mode - MAD_1000T_MASTER_SLAVE structure
*                autoConfig   - MAD_TRUE for auto, MAD_FALSE for manual setup.
*                masterPrefer - MAD_TRUE if Master configuration is preferred.
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 0, Reg 9, Bit 10-12   : Master/Slave configuration 
*
*******************************************************************************/
MAD_STATUS mdCopperSet1000TMasterMode
(
    IN  MAD_DEV                   *dev,
    IN  MAD_LPORT                 port,
    IN  MAD_1000T_MASTER_SLAVE    *mode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperSet1000TMasterMode called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
          /* E3016/E3082 has no 1000M */
    {
      MAD_DBG_ERROR("The port %d is not 1000T.\n", (int)port);
      return MAD_API_ERROR_IS_NOT_GIGA_PORT;
    }

    if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
    {
        MAD_DBG_ERROR("Copper not supported.\n");
        return MAD_API_COPPER_NOT_SUPPORT;
    }

    if(mode->autoConfig == MAD_TRUE)
    {
        if(mode->masterPrefer == MAD_TRUE)
        {
            data = 0x1;
        }
        else
        {
            data = 0x0;
        }
    }
    else
    {
        if(mode->masterPrefer == MAD_TRUE)
        {
            data = 0x6;
        }
        else
        {
            data = 0x4;
        }
    }

    /* Set Copper Control Register 1 */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_1000_CONTROL,
                    10,3,data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
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
* mdCopperGet1000TMasterMode
*
* DESCRIPTION:
*       This routine gets the ports 1000Base-T Master mode and restart the Auto
*        negotiation.
*
* INPUTS:
*       port - the logical port number.
*   
* OUTPUTS:
*       mode - MAD_1000T_MASTER_SLAVE structure
*                autoConfig   - MAD_TRUE for auto, MAD_FALSE for manual setup.
*                masterPrefer - MAD_TRUE if Master configuration is preferred.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 16, Bit 6:5 : Copper Control 1 (MDI Crossover)
*
*******************************************************************************/
MAD_STATUS mdCopperGet1000TMasterMode
(
    IN  MAD_DEV                   *dev,
    IN  MAD_LPORT                 port,
    OUT MAD_1000T_MASTER_SLAVE    *mode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdCopperGetMDIMode called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
          /* E3016/E3082 has no 1000M */
    {
      MAD_DBG_ERROR("The port %d is not 1000T.\n", (int)port);
      return MAD_API_ERROR_IS_NOT_GIGA_PORT;
    }


    if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
    {
        MAD_DBG_ERROR("Copper not supported.\n");
        return MAD_API_COPPER_NOT_SUPPORT;
    }


    /* Get the required bits in Copper Control Register 1 */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_1000_CONTROL,
                    10,3,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }

    if(data & 0x4)    /* Manual Mode */
    {
        mode->autoConfig = MAD_FALSE;

        if(data & 0x2)
        {
            mode->masterPrefer = MAD_TRUE;
        }
        else
        {
            mode->masterPrefer = MAD_FALSE;
        }
    }
    else    /* Auto Mode */
    {
        mode->autoConfig = MAD_TRUE;

        if(data & 0x1)
        {
            mode->masterPrefer = MAD_TRUE;
        }
        else
        {
            mode->masterPrefer = MAD_FALSE;
        }
    }

    return MAD_OK;
}


