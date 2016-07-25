#include <madCopyright.h>

/********************************************************************************
* madLoopback.c
* 
* DESCRIPTION:
*       APIs to run Loopback test.
*
* DEPENDENCIES:
*       madCpCtrl.c and madSysCtrl.c are required.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
* Check : Reset doesn't clear the loopback bit when read and write back the
*         register.
*******************************************************************************/

#include <madApi.h>
#include <madApiInternal.h>
#include <madHwCntl.h>
#include <madDrvPhyRegs.h>
#include <madDrvConfig.h>

/*
    MAC Interface Loopback for 88E1181 like devices (MAD_PHY_LOOPBACK_TYPE5)
*/
static 
MAD_STATUS madMACIfLB_1181
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_BOOL  en,
    IN  MAD_MAC_LOOPBACK_MODE   mode,
    IN  MAD_SPEED_MODE            speed

)
{
    MAD_U16     data, modeData;
    MAD_STATUS  retVal;
    MAD_SPEED_MODE oldSpeed;
    MAD_BOOL    linkOn;

    if (en)
    {
        /* enable MAC Interface Loopback */
        
        switch(mode)
        {
            case MAD_MAC_LOOPBACK_MODE_0:
                modeData = 0;
                break;
            case MAD_MAC_LOOPBACK_MODE_1:
                modeData = 1 << 4;
                break;
            case MAD_MAC_LOOPBACK_MODE_2:
                modeData = 2 << 4;
                break;
            case MAD_MAC_LOOPBACK_MODE_3:
                modeData = 3 << 4;
                break;
            default:
                MAD_DBG_ERROR("Invalid mode.\n");
                return MAD_API_UNKNOWN_MACIF_LB_MODE;
        }

        if (modeData != 0)
        {
            /* check if link is on */
            if((retVal = mdGetLinkStatus(dev,MAD_PORT_2_LPORT(hwPort),&linkOn)) != MAD_OK)
            {
                MAD_DBG_ERROR("mdGetLinkStatus failed.\n");
                return (retVal | MAD_API_FAIL_GET_LINK_ST);
            }

            if(!linkOn)
            {
                MAD_DBG_INFO("Link should be established for mode %i\n",mode);
                return MAD_API_LINK_DOWN;
            }
        }

        /* Disable Sleep mode and Auto MDI Crossover mode */
        if((retVal = madHwReadPagedPhyReg(dev,hwPort,0,MAD_REG_COPPER_CONTROL_1,&data)) 
            != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        data &= ~0x0360;

        if((retVal = madHwWritePagedPhyReg(dev,hwPort,0,MAD_REG_COPPER_CONTROL_1,data)) 
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }

        /* Soft Reset */
        if((retVal = madHwPagedReset(dev,hwPort,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Soft Reset failed.\n");
            return (retVal | MAD_API_FAIL_SW_RESET);
        }

        /* Disable loopback bit */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        14,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        /* Get the current speed */
        if((retVal = mdCopperGetSpeedStatus(dev,MAD_PORT_2_LPORT(hwPort),&oldSpeed)) != MAD_OK)
        {
            MAD_DBG_ERROR("mdCopperGetSpeedStatus failed.\n");
            return (retVal | MAD_API_FAIL_GET_SPEED_ST);
        }

        if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,2,MAD_REG_MAC_CONTROL,&data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        /* disable MAC If Auto-Nego */
        data &= ~MAD_BIT_12;

        /* Select speed and Loopback Mode */
        switch (speed)
        {
            case MAD_SPEED_10M:
                data &= ~(MAD_BIT_13|MAD_BIT_6|MAD_BIT_5|MAD_BIT_4);
                data |= modeData;
                break;
            case MAD_SPEED_100M:
                data &= ~(MAD_BIT_13|MAD_BIT_6|MAD_BIT_5|MAD_BIT_4);
                data |= modeData;
                data |= MAD_BIT_13;
                break;
            case MAD_SPEED_1000M:
                data &= ~(MAD_BIT_13|MAD_BIT_6|MAD_BIT_5|MAD_BIT_4);
                data |= modeData;
                data |= MAD_BIT_6;
                break;
            default:
                data &= ~(MAD_BIT_5|MAD_BIT_4);
                data |= modeData;
                break;
        }

        /* Set the test mode and MAC If speed */
        if((retVal = madHwWritePagedPhyReg(
                        dev,hwPort,2,MAD_REG_MAC_CONTROL,data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
        /* Set Copper interface and speed of operation.  Xu */
        if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,0,MAD_REG_CONTROL,&data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        /* disable Auto-Nego, set 1000M, duplex */
        data &= ~(MAD_BIT_12|MAD_BIT_13|MAD_BIT_6|MAD_BIT_8);
        data |= MAD_BIT_6;
        data |= MAD_BIT_8;
        if((retVal = madHwWritePagedPhyReg(
                        dev,hwPort,0,MAD_REG_CONTROL,data)) != MAD_OK)
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

        /* Enable loopback mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        14,1,1)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        
    }
    else
    {
        /* Clear the test mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_MAC_CONTROL,
                        4,2,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        /* Disable loopback mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        14,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        /* enable Auto-Nego. */
        if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,0,MAD_REG_CONTROL,&data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        data |= MAD_BIT_12;
        if((retVal = madHwWritePagedPhyReg(
                        dev,hwPort,0,MAD_REG_CONTROL,data)) != MAD_OK)
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
    }

    return MAD_OK;
}

/*
    MAC Interface Loopback for 88E3016, E3082 like devices (MAD_PHY_LOOPBACK_TYPE7)
*/
static MAD_U32 saved_3016_reg0=0x3100;

static 
MAD_STATUS madMACIfLB_3016
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_BOOL  en,
    IN  MAD_MAC_LOOPBACK_MODE   mode
)
{
  MAD_STATUS  retVal;

  if (en)
  {
    /* E3016 MAC loopback enable */

    /* Save */
    if((retVal = mdSysGetPhyReg(
                    dev,hwPort, 0,
                    &saved_3016_reg0)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal);
    }
    
    /* Enable Force 100M full-duplex */
    if((retVal = mdSysSetPhyReg(
                    dev,hwPort, 0,
                    0xa100)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal);
    }
    /* Enable Loopback */
    if((retVal = mdSysSetPhyReg(
                    dev,hwPort, 0,
                    0x6100)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal);
    }
    /* Erite page */
    if((retVal = mdSysSetPhyReg(
                    dev,hwPort, 29,
                    0x0002)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal);
    }
    /* Block 100M link and disable transmitter */
    if((retVal = mdSysSetPhyReg(
                    dev,hwPort, 30,
                    0x876f)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal);
    }


  }
  else
  /* E3016 MAC loopback disable */
  {
    /* Disable Loopback */
    if((retVal = mdSysSetPhyReg(
                    dev,hwPort, 0,
                    0x2100)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal);
    }
    /* Erite page */
    if((retVal = mdSysSetPhyReg(
                    dev,hwPort, 29,
                    0x0002)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal);
    }
    /* Un-Block 100M link */
    if((retVal = mdSysSetPhyReg(
                    dev,hwPort, 30,
                    0x0000)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal);
    }
    /* Set back reg0*/
    if((retVal = mdSysSetPhyReg(
                    dev,hwPort, 0,
                    saved_3016_reg0)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to phy reg failed.\n");
        return (retVal);
    }
    

  }
  return MAD_OK;
}

/*
    MAC Interface Loopback for 88E1116, E1121 like devices (MAD_PHY_LOOPBACK_TYPE2)
*/
static 
MAD_STATUS madMACIfLB_1116
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_BOOL  en,
    IN  MAD_MAC_LOOPBACK_MODE   mode,
    IN  MAD_SPEED_MODE            speed
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_SPEED_MODE oldSpeed;

    if (en)
    {
        /* enable MAC Interface Loopback */
        
        if (mode != MAD_MAC_LOOPBACK_MODE_0)
        {
            MAD_DBG_INFO("The device does not support for mode %i\n",mode);
            return MAD_API_MACIF_LB_MOD_NOT_SUPPORT;
        }

        /* Disable Sleep mode and Auto MDI Crossover mode */
        if((retVal = madHwReadPagedPhyReg(dev,hwPort,0,MAD_REG_COPPER_CONTROL_1,&data)) 
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        data &= ~0x0360;

        if((retVal = madHwWritePagedPhyReg(dev,hwPort,0,MAD_REG_COPPER_CONTROL_1,data)) 
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }

        /* Soft Reset */
        if((retVal = madHwPagedReset(dev,hwPort,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Soft Reset failed.\n");
            return (retVal | MAD_API_FAIL_SW_RESET);
        }

        /* Disable loopback bit */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        14,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        /* Get the current speed */
        if((retVal = mdCopperGetSpeedStatus(dev,MAD_PORT_2_LPORT(hwPort),&oldSpeed)) != MAD_OK)
        {
            MAD_DBG_ERROR("mdCopperGetSpeedStatus failed.\n");
            return (retVal | MAD_API_FAIL_GET_SPEED_ST);
        }

        if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,2,MAD_REG_MAC_CONTROL,&data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }


        /* Select speed and Loopback Mode */
        switch (speed)
        {
            case MAD_SPEED_10M:
                data &= ~(MAD_BIT_13|MAD_BIT_6);
                 break;
            case MAD_SPEED_100M:
                data &= ~(MAD_BIT_13|MAD_BIT_6);
                data |= MAD_BIT_13;
                break;
            case MAD_SPEED_1000M:
                data &= ~(MAD_BIT_13|MAD_BIT_6);
                 data |= MAD_BIT_6;
                break;
            default:
                 break;
        }

        /* Set the test mode and MAC If speed */
        if((retVal = madHwWritePagedPhyReg(
                        dev,hwPort,2,MAD_REG_MAC_CONTROL,data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
        /* Set Copper interface and speed of operation.  Xu */
        if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,0,MAD_REG_CONTROL,&data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        /* disable Auto-Nego, set 1000M, duplex */
        data &= ~(MAD_BIT_12|MAD_BIT_13|MAD_BIT_6|MAD_BIT_8);
        data |= MAD_BIT_6;
        data |= MAD_BIT_8;
        if((retVal = madHwWritePagedPhyReg(
                        dev,hwPort,0,MAD_REG_CONTROL,data)) != MAD_OK)
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

        /* Enable loopback mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        14,1,1)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        
    }
    else
    {
        /* Clear the test mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_MAC_CONTROL,
                        4,2,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        /* Disable loopback mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        14,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        /* enable Auto-Nego. */
        if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,0,MAD_REG_CONTROL,&data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        data |= MAD_BIT_12;
        if((retVal = madHwWritePagedPhyReg(
                        dev,hwPort,0,MAD_REG_CONTROL,data)) != MAD_OK)
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
    }

    return MAD_OK;
}


/*
    MAC Interface Loopback for 88E1149, E1240, E1112 like devices (MAD_PHY_LOOPBACK_TYPE1)
*/
static 
MAD_STATUS madMACIfLB_1149
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_BOOL  en,
    IN  MAD_MAC_LOOPBACK_MODE   mode,
    IN  MAD_SPEED_MODE            speed
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_SPEED_MODE oldSpeed;

    if (en)
    {
        /* enable MAC Interface Loopback */
        
        if (mode != MAD_MAC_LOOPBACK_MODE_0)
        {
            MAD_DBG_INFO("The device does not support for mode %i\n",mode);
            return MAD_API_MACIF_LB_MOD_NOT_SUPPORT;
        }

        /* Disable Sleep mode and Auto MDI Crossover mode */
        if((retVal = madHwReadPagedPhyReg(dev,hwPort,0,MAD_REG_COPPER_CONTROL_1,&data)) 
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        data &= ~0x0360;

        if((retVal = madHwWritePagedPhyReg(dev,hwPort,0,MAD_REG_COPPER_CONTROL_1,data)) 
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }

        /* Soft Reset */
        if((retVal = madHwPagedReset(dev,hwPort,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Soft Reset failed.\n");
            return (retVal | MAD_API_FAIL_SW_RESET);
        }

        /* Disable loopback bit */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        14,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        /* Get the current speed */
        if((retVal = mdCopperGetSpeedStatus(dev,MAD_PORT_2_LPORT(hwPort),&oldSpeed)) != MAD_OK)
        {
            MAD_DBG_ERROR("mdCopperGetSpeedStatus failed.\n");
            return (retVal | MAD_API_FAIL_GET_SPEED_ST);
        }

        if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,2,MAD_REG_MAC_CONTROL,&data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        /* disable MAC If Auto-Nego */
        data &= ~MAD_BIT_12;

        /* Select speed and Loopback Mode */
        switch (speed)
        {
            case MAD_SPEED_10M:
                data &= ~(MAD_BIT_13|MAD_BIT_6);
                 break;
            case MAD_SPEED_100M:
                data &= ~(MAD_BIT_13|MAD_BIT_6);
                data |= MAD_BIT_13;
                break;
            case MAD_SPEED_1000M:
                data &= ~(MAD_BIT_13|MAD_BIT_6);
                 data |= MAD_BIT_6;
                break;
            default:
                 break;
        }

        /* Set the test mode and MAC If speed */
        if((retVal = madHwWritePagedPhyReg(
                        dev,hwPort,2,MAD_REG_MAC_CONTROL,data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
        /* Set Copper interface and speed of operation.  Xu */
        if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,0,MAD_REG_CONTROL,&data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        /* disable Auto-Nego, set 1000M, duplex */
        data &= ~(MAD_BIT_12|MAD_BIT_13|MAD_BIT_6|MAD_BIT_8);
        data |= MAD_BIT_6;
        data |= MAD_BIT_8;
        if((retVal = madHwWritePagedPhyReg(
                        dev,hwPort,0,MAD_REG_CONTROL,data)) != MAD_OK)
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

        /* Enable loopback mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        14,1,1)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        
    }
    else
    {
        /* Clear the test mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_MAC_CONTROL,
                        4,2,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        /* Disable loopback mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        14,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        /* enable Auto-Nego. */
        if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,0,MAD_REG_CONTROL,&data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        data |= MAD_BIT_12;
        if((retVal = madHwWritePagedPhyReg(
                        dev,hwPort,0,MAD_REG_CONTROL,data)) != MAD_OK)
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
    }

    return MAD_OK;
}



/*
    MAC Interface Loopback for 88E1111 like devices (MAD_PHY_LOOPBACK_TYPE4)
*/
static 
MAD_STATUS madMACIfLB_1111_SGMII
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_BOOL  en,
    IN  MAD_MAC_LOOPBACK_MODE   mode
)
{
    MAD_STATUS  retVal;

    if (en)
    {
        /* enable MAC Interface Loopback */
        
        /* Enable loopback mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,22,
                        0,7,0x01)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        12,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        if((retVal = madHwPagedReset(dev,hwPort,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Soft Reset failed.\n");
            return (retVal | MAD_API_FAIL_SW_RESET);
        }
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        14,1,1)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
    }
    else
    {
        /* disable MAC Interface Loopback */
        
        /* Disable loopback mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        14,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        12,1,1)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
                
        /* Soft Reset */
        if((retVal = madHwPagedReset(dev,hwPort,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Soft Reset failed.\n");
            return (retVal | MAD_API_FAIL_SW_RESET);
        }
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,22,
                        0,7,0x0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
    }

    return MAD_OK;
}
 
static 
MAD_STATUS madMACIfLB_1111
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_BOOL  en,
    IN  MAD_MAC_LOOPBACK_MODE   mode
)
{
  MAD_STATUS  retVal, status;
  MAD_BOOL    linkOn, autoNegEn;
  MAD_U32     autoMode;
  static MAD_U16    keepData, keepData_1;
  static MAD_BOOL dataKept=MAD_FALSE, data_1Kept=MAD_FALSE;

  if (MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_SGMII)) 
  {
    if((status = mdSGMIIGetAutoNeg(dev,hwPort,&autoNegEn,(MAD_SPEED_MODE *)&autoMode)) != MAD_OK)
    {
      MAD_DBG_ERROR("mdSGMIIGetAutoNeg return Failed\n");
      return (status | MAD_API_FAIL_GET_AUTONEG);
    }
    if (autoNegEn==MAD_TRUE)
    {
      MAD_DBG_ERROR("SGMII Auto negotiation is enabled, and Mac Loopback cann't performed.\n");
      return MAD_OK;
    }
    return madMACIfLB_1111_SGMII(dev,hwPort,en,mode);
      
  }
  else if (MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_GBIC)) 
  {
    if((status = mdCopperGetAutoNeg(dev,hwPort,&autoNegEn,&autoMode)) != MAD_OK)
    {
      MAD_DBG_ERROR("mdCopperGetAutoNeg return Failed\n");
      return (status | MAD_API_FAIL_GET_AUTONEG);
    }
    if (autoNegEn==MAD_TRUE)
    {
      MAD_DBG_ERROR("GBIC Auto negotiation is enabled, and Mac Loopback cann't performed.\n");
      return MAD_OK;
    }
    return madMACIfLB_1111_SGMII(dev,hwPort,en,mode);
      
  }
  else
  {
    if (en)
    {
      if((status = mdCopperGetAutoNeg(dev,hwPort,&autoNegEn,&autoMode)) != MAD_OK)
      {
        MAD_DBG_ERROR("mdCopperGetAutoNeg return Failed\n");
        return (status | MAD_API_FAIL_GET_AUTONEG);
      }
       if (autoNegEn==MAD_TRUE)
      {
        /* check if link is on */
        if((retVal = mdGetLinkStatus(dev,MAD_PORT_2_LPORT(hwPort),&linkOn)) != MAD_OK)
        {
           MAD_DBG_ERROR("mdGetLinkStatus failed.\n");
           return (retVal | MAD_API_FAIL_GET_LINK_ST);
        }

        if(linkOn)
        {
           MAD_DBG_INFO("Auto neg is enabled and Link is on, Mac Loopback can not perform.\n");
           return MAD_OK;
        }
        else
        {
            /* To perform Mac loopback  */
            /* enable MAC Interface Loopback */
        
            /* Enable loopback mode */
            if((retVal = madHwGetPagedPhyRegField(
                        dev,hwPort,0, 20,
                        4,3,&keepData)) != MAD_OK)
            {
              MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
              return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
            } 
            dataKept=MAD_TRUE;
            if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0, 20,
                        4,3,0x5)) != MAD_OK)
            {
              MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
              return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            } 
            if((retVal = madHwPagedReset(dev,hwPort,0)) != MAD_OK)
            {
              MAD_DBG_ERROR("Soft Reset failed.\n");
              return (retVal | MAD_API_FAIL_SW_RESET);
            }
            if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        14,1,1)) != MAD_OK)
            {
              MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
              return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
        } /* link on */
      }  /*auto neg en */
      else /* Auto neg disabled */
      {
            /* Mac interface speed is 100M */
            if((retVal = madHwGetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        13,1,&keepData)) != MAD_OK)
            {
              MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
              return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
            }
            dataKept=MAD_TRUE;
            if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        13,1,1)) != MAD_OK)
            {
              MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
              return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_CONTROL,
                    6,1, &keepData_1)) != MAD_OK)
            {
              MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
              return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
            }
            data_1Kept=MAD_TRUE;
             if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_CONTROL,
                    6,1,0)) != MAD_OK)
            {
              MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
              return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            /* software reset */
            if((retVal = madHwPagedReset(dev,hwPort,0)) != MAD_OK)
            {
              MAD_DBG_ERROR("Soft Reset failed.\n");
              return (retVal | MAD_API_FAIL_SW_RESET);
            }

            if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        14,1,1)) != MAD_OK)
            {
              MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
              return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
      } /* auto neg dis */
    } /* en*/
    else
    {
      /* disable MAC Interface Loopback */
      if((status = mdCopperGetAutoNeg(dev,hwPort,&autoNegEn,&autoMode)) != MAD_OK)
      {
        MAD_DBG_ERROR("mdCopperGetAutoNeg return Failed\n");
        return (status | MAD_API_FAIL_GET_AUTONEG);
      }
       if (autoNegEn==MAD_TRUE)
      {
         if (dataKept==MAD_TRUE)
         {
            if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0, 20,
                        4,3,keepData)) != MAD_OK)
            {
              MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
              return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            } 
            dataKept=MAD_FALSE;
         }
         if((retVal = madHwPagedReset(dev,hwPort,0)) != MAD_OK)
         {
              MAD_DBG_ERROR("Soft Reset failed.\n");
              return (retVal | MAD_API_FAIL_SW_RESET);
         }
       
      }
      else
      {
        if (dataKept==MAD_TRUE)
        {
            if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        13,1, keepData)) != MAD_OK)
            {
              MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
              return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            dataKept=MAD_FALSE;
        }
        if (data_1Kept==MAD_TRUE)
        {
            if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        6,1,keepData_1)) != MAD_OK)
            {
              MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
              return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            data_1Kept=MAD_FALSE;
         }
         /* software reset */
         if((retVal = madHwPagedReset(dev,hwPort,0)) != MAD_OK)
         {
              MAD_DBG_ERROR("Soft Reset failed.\n");
              return (retVal | MAD_API_FAIL_SW_RESET);
         }
      }

      /* Disable loopback mode */
      if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_CONTROL,
                        14,1,0)) != MAD_OK)
      {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
        
      /* Soft Reset */
      if((retVal = madHwPagedReset(dev,hwPort,0)) != MAD_OK)
      {
         MAD_DBG_ERROR("Soft Reset failed.\n");
         return (retVal | MAD_API_FAIL_SW_RESET);
      }
    }
  }
  return MAD_OK;
}
/*
    MAC Interface Loopback for 88E340 like devices (MAD_PHY_LOOPBACK_TYPE8)
*/
static 
MAD_STATUS madMACIfLB_1340
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_BOOL  en,
    IN  MAD_MAC_LOOPBACK_MODE   mode,
    IN  MAD_SPEED_MODE            speed
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U16    page;
    MAD_U8     addr;

    if (en)
    {
      /* enable MAC Interface Loopback */
        
      if ((mode != MAD_MAC_LOOPBACK_MODE_SYS)
            &&(mode != MAD_MAC_LOOPBACK_MODE_SYNC_SERDES_125)
            &&(mode != MAD_MAC_LOOPBACK_MODE_SYNC_SERDES_5))
      {
        MAD_DBG_INFO("The device does not support for mode %i\n",mode);
        return MAD_API_MACIF_LB_MOD_NOT_SUPPORT;
      }

      if (mode == MAD_MAC_LOOPBACK_MODE_SYS)
      {
        /* Get speed */
        if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE2,
                     MAD_REG_MAC_CONTROL_2_1340,&data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        /* Select speed */
        switch (speed)
        {
            case MAD_SPEED_10M:
                data &= ~(0x3);
                data |= 0x4;
                 break;
            case MAD_SPEED_100M:
                data &= ~(0x3);
                data |= 0x5;
                break;
            case MAD_SPEED_1000M:
                data &= ~(0x3);
                data |= 0x6;
                break;
            default:
                 break;
        }

        /* Set speed */
        if((retVal = madHwWritePagedPhyReg(dev,hwPort,MAD_PAGE2,
                     MAD_REG_MAC_CONTROL_2_1340,data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }

        /* Eable loopback bit */
        if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE0,
                     MAD_REG_CONTROL, 14,1,1)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE1,
                     MAD_REG_CONTROL, 14,1,1)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE4,
                     MAD_REG_CONTROL, 14,1,1)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
      }/* MAD_MAC_LOOPBACK_MODE_SYNC_SYS done*/
      else  /* MAD_MAC_LOOPBACK_MODE_SYNC_SERDES */
      {
        if (mode == MAD_MAC_LOOPBACK_MODE_SYNC_SERDES_125)
        {
          page = MAD_PAGE1;
          addr = MAD_REG_FIBER_CONTROL_1;
        }
        else
         {
          page = MAD_PAGE4;
          addr = MAD_REG_QSGMII_CONTROL_1;
        }
        /* Get data */
        if((retVal = madHwReadPagedPhyReg(dev,hwPort,page, addr,&data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }
        data |= 0x1100;
        /* Set enable Synchronous SERDES loopback */
        if((retVal = madHwWritePagedPhyReg(dev,hwPort,page, addr,data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }
      }
      
    }
    else
    {
      /* disable MAC Interface Loopback */
        
      if ((mode != MAD_MAC_LOOPBACK_MODE_SYS)
            &&(mode != MAD_MAC_LOOPBACK_MODE_SYNC_SERDES_125)
            &&(mode != MAD_MAC_LOOPBACK_MODE_SYNC_SERDES_5))
      {
        MAD_DBG_INFO("The device does not support for mode %i\n",mode);
        return MAD_API_MACIF_LB_MOD_NOT_SUPPORT;
      }

      if (mode == MAD_MAC_LOOPBACK_MODE_SYS)
      {
        /* Disable loopback bit */
        if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE0,
                     MAD_REG_CONTROL, 14,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE1,
                     MAD_REG_CONTROL, 14,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE4,
                     MAD_REG_CONTROL, 14,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
      }/* MAD_MAC_LOOPBACK_MODE_SYNC_SYS done*/
      else  /* MAD_MAC_LOOPBACK_MODE_SYNC_SERDES */
      {
        if (mode == MAD_MAC_LOOPBACK_MODE_SYNC_SERDES_125)
        {
          page = MAD_PAGE1;
          addr = MAD_REG_FIBER_CONTROL_1;
        }
        else
         {
          page = MAD_PAGE4;
          addr = MAD_REG_QSGMII_CONTROL_1;
        }
        /* Get data */
        if((retVal = madHwReadPagedPhyReg(dev,hwPort,page, addr,&data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }
        data &= 0xEEFF;
        /* Set enable Synchronous SERDES loopback */
        if((retVal = madHwWritePagedPhyReg(dev,hwPort,page, addr,data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }
      }
      
    }

    return MAD_OK;
}



/*******************************************************************************
* mdDiagSetMACIfLoopback
*
* DESCRIPTION:
*       This routine enables or disables MAC Interface Loopback.
*        When enables loopback test, one of the following modes is required:
*           Loopback mode 0,
*           Loopback mode 1 (link should be established),
*           Loopback mode 2 (link should be established), and
*           Loopback mode 3 (link should be established)
*       If link is not established for the mode 1,2,and 3, it will return MAD_FAIL.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE to disable
*       mode - MAD_MAC_LOOPBACK_MODE, if en is MAD_TRUE
*              ignored, if en is MAD_FALSE
*              supported Loopback Modes are:
*                   MAD_MAC_LOOPBACK_MODE_0,
*                   MAD_MAC_LOOPBACK_MODE_1,
*                   MAD_MAC_LOOPBACK_MODE_2, 
*                   MAD_MAC_LOOPBACK_MODE_3,
*                    MAD_MAC_LOOPBACK_MODE_SYS,
*                    MAD_MAC_LOOPBACK_MODE_SYNC_SERDES_125,
*                    MAD_MAC_LOOPBACK_MODE_SYNC_SERDES_5
*       speed - MAD_SPEED_MODE.
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       After MAC Interface Loopback, the followings need to be reconfigured:
*           MDI Crossover mode (mdCopperSetMDIMode API), and
*           SGMII Auto-Neg mode (mdSGMIISetAutoNeg API)
*
*       Accessed Registers:
*
*******************************************************************************/
MAD_STATUS mdDiagSetMACIfLoopback
(
    IN  MAD_DEV   *dev,
    IN  MAD_LPORT port,
    IN  MAD_BOOL  en,
    IN  MAD_MAC_LOOPBACK_MODE   mode,
    IN  MAD_SPEED_MODE            speed
)
{
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdDiagSetMACIfLoopback called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_MAC_IF_LOOP))
    {
        MAD_DBG_ERROR("MAC Interface Loopback not supported.\n");
        return MAD_API_MACIF_LB_NOT_SUPPORT;
    }
    switch(dev->phyInfo.macIfLoopType)
    {
        case MAD_PHY_LOOPBACK_TYPE1:
            return madMACIfLB_1149(dev,hwPort,en,mode, speed);

        case MAD_PHY_LOOPBACK_TYPE2:    /* E1116 */
            return madMACIfLB_1116(dev,hwPort,en,mode, speed);

        case MAD_PHY_LOOPBACK_TYPE3:
            return madMACIfLB_1111(dev,hwPort,en,mode);

        case MAD_PHY_LOOPBACK_TYPE4:
            return madMACIfLB_1111(dev,hwPort,en,mode);

        case MAD_PHY_LOOPBACK_TYPE5:
            return madMACIfLB_1181(dev,hwPort,en,mode, speed);

        case MAD_PHY_LOOPBACK_TYPE7:    /* E30** */
            return madMACIfLB_3016(dev,hwPort,en,mode);

        case MAD_PHY_LOOPBACK_TYPE8:   /* for 1340 */
            return madMACIfLB_1340(dev,hwPort,en,mode, speed);

        case MAD_PHY_LOOPBACK_TYPE0:
        case MAD_PHY_LOOPBACK_TYPE6:
        default:
            return MAD_API_MACIF_LB_NOT_SUPPORT;
    }

    return MAD_OK;
}


/*
    Line Loopback for the device using register 0, page 2
*/
static
MAD_STATUS madLineLB_0_2
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

    /* Enable line loopback */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,2,0,
                    14,1,data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    return MAD_OK;
}

/*
    Line Loopback for the device using register 21, page 2 (1181, 1149)
*/
static
MAD_STATUS madLineLB_21_2
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

    /* Enable line loopback */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,2,MAD_REG_MAC_CONTROL,
                    14,1,data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    return MAD_OK;
}

/*
    Line Loopback for the device using register 20 (114x, 104x)
*/
static
MAD_STATUS madLineLB_20
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

    /* Enable line loopback */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,MAD_REG_EXT_PHY_SPEC_CONTROL,
                    14,1,data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    return MAD_OK;
}


/*
    Line Loopback for the device using register 28 (3016)
*/

static
MAD_STATUS madLineLB_28
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

    /* Enable line loopback */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,0,28,
                    4,1,data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    return MAD_OK;
}

/*
    Line Loopback for the device using register 21, page 2 and 16_1 and 16_4(1340)
*/
static
MAD_STATUS madLineLB_1340
(
    IN  MAD_DEV   *dev,
    IN  MAD_LPORT port,
    IN  MAD_BOOL  en
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U16    page;
  MAD_U8     addr;
  MAD_U8    hwPort;

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);

  /* Enable or disable */
  MAD_BOOL_2_BIT(en,data);

  if(MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    /* Enable line loopback */
    if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE2,MAD_REG_MAC_CONTROL,
                    14,1,data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
  }
  else
  {
    if(MAD_DEV_HWREALMODE(dev, hwPort, MAD_PHY_MODE_QSGMII_MEDIA))
    {
      page = MAD_PAGE4;
      addr = MAD_REG_QSGMII_CONTROL_1;
    }
    else
    {
      page = MAD_PAGE1;
      addr = MAD_REG_FIBER_CONTROL_1;
    }
    /* Get data */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,page, addr,&data)) != MAD_OK)
    {
      MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
      return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    if (en == MAD_TRUE)
      data |= 0x1100;
    else
      data &= 0xEEFF;

    /* Set enable Synchronous SERDES loopback */
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,page, addr,data)) != MAD_OK)
    {
      MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
      return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }
  }
  return MAD_OK;
}

/*******************************************************************************
* mdDiagSetLineLoopback
*
* DESCRIPTION:
*       This routine enables or disables Line Loopback.
*       Link should be established with full duplex mode before enabling Line 
*       loopback. Otherwise, this routine returns fail.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE to disable
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 17, Bit 10 : Copper Link
*       Page 2, Reg 21, Bit 14 : Line loopback enable
*
*******************************************************************************/
MAD_STATUS mdDiagSetLineLoopback
(
    IN  MAD_DEV   *dev,
    IN  MAD_LPORT port,
    IN  MAD_BOOL  en
)
{
    MAD_STATUS  retVal;
    MAD_U8      hwPort;
    MAD_BOOL    mode;

    MAD_DBG_INFO("mdDiagSetLineLoopback called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_MAC_IF_LOOP))
    {
        MAD_DBG_ERROR("Line Loopback not supported.\n");
        return MAD_API_LINE_LB_NOT_SUPPORT;
    }

    if (en)
    {
        /* Get the current link */
        if((retVal = mdGetLinkStatus(dev,port,&mode)) != MAD_OK)
        {
            MAD_DBG_ERROR("mdGetLinkStatus failed.\n");
            return (retVal | MAD_API_FAIL_GET_LINK_ST);
        }
        if(!mode)
        {
            MAD_DBG_INFO("Link should be established before this call\n");
            return MAD_API_LINK_DOWN;
        }

        /* Get the current duplex mode */
        if((retVal = mdGetDuplexStatus(dev,port,&mode)) != MAD_OK)
        {
            MAD_DBG_ERROR("mdGetDuplexStatus failed.\n");
            return (retVal | MAD_API_FAIL_GET_DUPLEX_ST);
        }

        if(!mode)
        {
            MAD_DBG_INFO("Only Full Duplex mode is supported.\n");
            return MAD_API_NOT_FULL_DUPLEX;
        }

    }

    switch(dev->phyInfo.lineLoopType)
    {
        case MAD_PHY_LINE_LB_TYPE1:
            return madLineLB_0_2(dev,hwPort,en);

        case MAD_PHY_LINE_LB_TYPE2:
            return madLineLB_21_2(dev,hwPort,en);

        case MAD_PHY_LINE_LB_TYPE3:
           return madLineLB_20(dev,hwPort,en);

        case MAD_PHY_LINE_LB_TYPE5: /* E30** */
           return madLineLB_28(dev,hwPort,en);

        case MAD_PHY_LINE_LB_TYPE6:  /* for 1340 */
            return madLineLB_1340(dev,port,en);

        case MAD_PHY_LINE_LB_TYPE4:
        default:
            return MAD_API_LINE_LB_NOT_SUPPORT;
    }

    return MAD_OK;
}




/*
    External Loopback for the 88E1149 like device (1181 1240 1340)
*/
static
MAD_STATUS madExtLB_1149
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_BOOL  en,
    IN  MAD_SPEED_MODE  mode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;

    if (en)
    {
        switch (mode)
        {
            case MAD_SPEED_10M:
                /* Set the speed/duplex */
                if((retVal = mdCopperSetAutoNeg(dev,MAD_PORT_2_LPORT(hwPort),MAD_FALSE,MAD_PHY_10FDX)) != MAD_OK)
                {
                    MAD_DBG_ERROR("mdCopperSetAutoNeg failed.\n");
                    return (retVal | MAD_API_FAIL_SET_AUTONEG);
                }
                data = 0;
                break;

            case MAD_SPEED_100M:
                /* Set the speed/duplex */
                if((retVal = mdCopperSetAutoNeg(dev,MAD_PORT_2_LPORT(hwPort),MAD_FALSE,MAD_PHY_100FDX)) != MAD_OK)
                {
                    MAD_DBG_ERROR("mdCopperSetAutoNeg failed.\n");
                    return (retVal | MAD_API_FAIL_SET_AUTONEG);
                }
                data = 0;
                break;

            case MAD_SPEED_1000M:
                /* Set the speed/duplex */
                if((retVal = mdCopperSetAutoNeg(dev,MAD_PORT_2_LPORT(hwPort),MAD_TRUE,MAD_AUTO_AD_1000FDX)) != MAD_OK)
                {
                    MAD_DBG_ERROR("mdCopperSetAutoNeg failed.\n");
                    return (retVal | MAD_API_FAIL_SET_AUTONEG);
                }
                data = 1;
                break;
            default:
                MAD_DBG_ERROR("Invalid mode %i.\n",mode);
                return MAD_API_UNKNOWN_EXT_LB_MODE;
        }
                
        /* Set Stub Test */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,6,MAD_REG_PACKET_GENERATION,
                        5,1,data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

    }
    else
    {
        /* Disable Stub Test */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,6,MAD_REG_PACKET_GENERATION,
                        5,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
      
    }

    return MAD_OK;
}



static MAD_STATUS madIntGetEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_INT_TYPE    *intType
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("madIntGetEnable called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {

    /* Copper Interrupt */

    /* Set Interrupt Enable Register for Copper */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,0,MAD_REG_COPPER_INT_ENABLE, &data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }
    intType->intGroup0 = data & 0xFFFF;

    /* SGMII Interrupt */

    /* Set Interrupt Enable Register for SGMII */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,2,MAD_REG_MAC_INT_ENABLE, &data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }
    intType->intGroup0 |= (data & 0xFFFF) << 16;
    
  }
  else if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_FIBER))
  {

    /* Copper Interrupt */

    /* Set Interrupt Enable Register for Copper */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,0,MAD_REG_FIBER_INT_ENABLE, &data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }
    intType->intGroup1 = data & 0xFFFF;

  }
  else
  {
     MAD_DBG_ERROR("Device shoud be ether Copper or Fiber.\n");
     return MAD_API_ERROR_SET_COPPER_FIBER;
  }

    return MAD_OK;
}

/*
    External Loopback for the 88E1111 like device (104x, 114x)
*/

static
MAD_STATUS madExtLB_1111
(
    IN  MAD_DEV   *dev,
    IN  MAD_U8    hwPort,
    IN  MAD_BOOL  en,
    IN  MAD_SPEED_MODE  mode
)
{
    static MAD_INT_TYPE intTypeData, intTypeDataTmp, *intTypePtr=NULL;
    MAD_STATUS  retVal;
    if (en)
    {
        switch (mode)
        {
            case MAD_SPEED_10M:
                /* Set the speed/duplex */
                if((retVal = mdCopperSetAutoNeg(dev,MAD_PORT_2_LPORT(hwPort),MAD_FALSE,MAD_PHY_10FDX)) != MAD_OK)
                {
                    MAD_DBG_ERROR("mdCopperSetAutoNeg failed.\n");
                    return (retVal | MAD_API_FAIL_SET_AUTONEG);
                }
          
                break;

            case MAD_SPEED_100M:
                /* Set the speed/duplex */
                if((retVal = mdCopperSetAutoNeg(dev,MAD_PORT_2_LPORT(hwPort),MAD_FALSE,MAD_PHY_100FDX)) != MAD_OK)
                {
                    MAD_DBG_ERROR("mdCopperSetAutoNeg failed.\n");
                    return (retVal | MAD_API_FAIL_SET_AUTONEG);
                }
               
                break;

            case MAD_SPEED_1000M:
                /* Set the speed/duplex */
                if((retVal = mdCopperSetAutoNeg(dev,MAD_PORT_2_LPORT(hwPort),MAD_TRUE,MAD_AUTO_AD_1000FDX)) != MAD_OK)
                {
                    MAD_DBG_ERROR("mdCopperSetAutoNeg failed.\n");
                    return (retVal | MAD_API_FAIL_SET_AUTONEG);
                }
               
                break;
            default:
                MAD_DBG_ERROR("Invalid mode %i.\n",mode);
                return MAD_API_UNKNOWN_EXT_LB_MODE;
        }
        

        /* Set Stub Test */
      if (mode == MAD_SPEED_1000M)
      {        
        /* To save interupt status */
        intTypePtr=&intTypeData; /* It enters in loopback testing. */
        if((retVal = madIntGetEnable(
                        dev,hwPort,    intTypePtr)) != MAD_OK)    
        {
            MAD_DBG_ERROR("mdIntGetStatus failed.\n");
            return (retVal | MAD_API_FAIL_GET_INT_ST);
        }

        intTypeDataTmp.intGroup0 = 0;
        intTypeDataTmp.intGroup1 = 0;
        if((retVal = mdIntSetEnable(
                        dev,hwPort,    &(intTypeDataTmp))) != MAD_OK)
        {
            MAD_DBG_ERROR("mdIntSetEnable failed.\n");
            return (retVal | MAD_API_FAIL_SET_INT_ST);
        }
        
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0, 9,
                        11,2,0x3)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        /* To perform soft reset */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0, 0,
                        15,1,0x1)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort, 7, MAD_REG_PAGE_ACCESS,
                        3,1,0x1)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort, 0x10, MAD_REG_PAGE_ACCESS,
                        1,1,0x1)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }


        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort, 0x12, MAD_REG_PAGE_ACCESS,
                        0,1,0x1)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
      }  

    }
    else
    {
      /* Disable Stub Test */
      if (mode == MAD_SPEED_1000M)
      {        

        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0, 9,
                        11,2,0x0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        /* To perform soft reset) */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0, 0,
                        15,1,0x1)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort, 7, MAD_REG_PAGE_ACCESS,
                        3,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort, 0x10, MAD_REG_PAGE_ACCESS,
                        1,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort, 0x12, MAD_REG_PAGE_ACCESS,
                        0,1,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        /* To recover interupt status */
        if (intTypePtr!=NULL) /* It is in loop testing */
        {
          /* Register 18 = data before testing (enable interrupts)*/
          if((retVal = mdIntSetEnable(
                        dev,hwPort, intTypePtr)) != MAD_OK)
          {
            MAD_DBG_ERROR("mdIntSetEnable failed.\n");
            return (retVal | MAD_API_FAIL_SET_INT_ST);
          }
          intTypePtr=NULL; /* return from loopback testing */
        }
      }  

      
    }

    return MAD_OK;
}

/*******************************************************************************
* mdDiagSetExternalLoopback
*
* DESCRIPTION:
*       This routine forces speed to the specified mode and if 1000Base-T mode
*       is used, it enables stub test as well.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to enable, MAD_FALSE to disable
*       mode - MAD_SPEED_MODE, if en is MAD_TRUE
*              ignored, if en is MAD_FALSE
*              supported Speed modes are:
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
*       After External Loopback, the followings need to be reconfigured:
*           Copper Speed/Duplex and Auto_Nego mode (mdCopperSetAutoNeg)
*
*       Accessed Registers:
*       Page 0, Reg 0, Bit 15,13,12,8,6 : 
*                                   Soft reset,speed,Auto-Neg enable,duplex
*       Page 0, Reg 4, Bit 11,10,8:5  : Advertisement Register
*       Page 0, Reg 9, Bit 12:8       : 1000 Base-T Control Register
*       Page 6, Reg 16, Bit 5 : Enable stub test
*        Various hidden registers are accessed.
*
*******************************************************************************/
MAD_STATUS mdDiagSetExternalLoopback
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    IN  MAD_BOOL        en,
    IN  MAD_SPEED_MODE  mode
)
{
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdDiagSetExternalLoopback called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    if(!MAD_DEV_CAPABILITY(dev, MAD_PHY_EXTERNAL_LOOP))
    {
        MAD_DBG_ERROR("MAC Interface Loopback not supported.\n");
        return MAD_API_EXT_LB_NO_SET_FUNC;
    }

    switch(dev->phyInfo.exLoopType)
    {
        case MAD_PHY_EX_LB_TYPE0:
            return MAD_API_EXT_LB_NO_SET_FUNC;

        case MAD_PHY_EX_LB_TYPE2:
            return madExtLB_1149(dev,hwPort,en,mode);

        case MAD_PHY_EX_LB_TYPE1:
            return madExtLB_1111(dev,hwPort,en,mode);  

        default:
            return MAD_API_EXT_LB_NO_SET_FUNC;
    }

    return MAD_OK;
}


