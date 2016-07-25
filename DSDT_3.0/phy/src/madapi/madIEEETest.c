#include <madCopyright.h>

/********************************************************************************
* madIEEETest.c
* 
* DESCRIPTION:
*       APIs for IEEE Test.
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

static void localDelay (MAD_32 delay)
{
    volatile MAD_32 count=delay;
    while (count--);
}


static MAD_STATUS setIEEETest_114X
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    IN  MAD_BOOL        en,
    IN  MAD_IEEE_TEST_MODE  mode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;
    MAD_DBG_INFO("mdDiagSetIEEETest called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    if (en)
    {
        /* enable IEEE Test */
        switch (mode)
        {
            case MAD_IEEE_TEST_WAVEFORM:
                data = 1;
                break;
            case MAD_IEEE_TEST_MASTER_JITTER:
                data = 2;
                break;
            case MAD_IEEE_TEST_SLAVE_JITTER:
                data = 3;
                break;
            case MAD_IEEE_TEST_DISTORTION:
                data = 4;
                break;
            default:
                MAD_DBG_ERROR("Unknown mode %i.\n",mode);
                return MAD_API_UNKNOWN_IEEE_TEST_MODE;
        }
        if (data!=3)  /* except for mode 3 */
        {
            /* Force master mode  and begin with all test mode disabled*/
            if((retVal = madHwWritePagedPhyReg(
                        dev,hwPort,0,MAD_REG_1000_CONTROL,
                        0x1f00)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
            }

        }  
        else /* mode 3 */
        {
            /* Force slave mode  and begin with all test mode disabled*/
            if((retVal = madHwWritePagedPhyReg(
                        dev,hwPort,0,MAD_REG_1000_CONTROL,
                        0x1700)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
            }
        }    
        if((retVal = madHwPagedReset(dev,hwPort,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Soft Reset failed.\n");
            return (retVal | MAD_API_FAIL_SW_RESET);
        }

        if((retVal = madHwWritePagedPhyReg(dev,hwPort,0x1e,30,0xcc00))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }

        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_1000_CONTROL,
                        13,3,data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        if((retVal = madHwWritePagedPhyReg(dev,hwPort,0x18,30,0x00ca))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
       
        localDelay(100);
        if((retVal = madHwWritePagedPhyReg(dev,hwPort,0x18,30,0x0000))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
        localDelay(100);
        if((retVal = madHwWritePagedPhyReg(dev,hwPort,0x1d,30,0x00c0))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
        if((retVal = madHwWritePagedPhyReg(dev,hwPort,0x1e,30,0xc800))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
        localDelay(100);
        if((retVal = madHwWritePagedPhyReg(dev,hwPort,0x1e,30,0xc400))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
        localDelay(100);
        if((retVal = madHwWritePagedPhyReg(dev,hwPort,0x1e,30,0xc000))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
        if((retVal = madHwWritePagedPhyReg(dev,hwPort,0x1e,30,0xc100))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
    }
    else
    {
        /* Clear the test mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_1000_CONTROL,
                        13,3,0)) != MAD_OK)
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


    return MAD_OK;
}

static MAD_STATUS setIEEETest_1149
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    IN  MAD_BOOL        en,
    IN  MAD_IEEE_TEST_MODE  mode
)
{
    MAD_U16     data, mMode;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdDiagSetIEEETest called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    if (en)
    {
        /* enable IEEE Test */
        switch (mode)
        {
            case MAD_IEEE_TEST_WAVEFORM:
                data = 1;
                mMode = 0x1F;
                break;
            case MAD_IEEE_TEST_MASTER_JITTER:
                data = 2;
                mMode = 0x1F;
                break;
            case MAD_IEEE_TEST_SLAVE_JITTER:
                data = 3;
                mMode = 0x17;
                break;
            case MAD_IEEE_TEST_DISTORTION:
                data = 4;
                mMode = 0x1F;
                break;
            default:
                MAD_DBG_ERROR("Unknown mode %i.\n",mode);
                return MAD_API_UNKNOWN_IEEE_TEST_MODE;
        }

        /* Enable TX_TCLK */
        if((retVal = madHwSetPagedPhyRegField(dev,hwPort,0,30,3,4,0x4))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        if((retVal = madHwSetPagedPhyRegField(dev,hwPort,1,30,13,1,0x1))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        /* Set the test mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_1000_CONTROL,
                        8,5,mMode)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        if((retVal = madHwWritePagedPhyReg(
                        dev,hwPort,0,0,0x1140)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
        }
        if((retVal = madHwPagedReset(dev,hwPort,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("Soft Reset failed.\n");
            return (retVal | MAD_API_FAIL_SW_RESET);
        }

        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_1000_CONTROL,
                        13,3,data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
    }
    else
    {
        /* Disable TX_TCLK */
        if((retVal = madHwSetPagedPhyRegField(dev,hwPort,4,30,0,5,0))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        /* Clear the test mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_1000_CONTROL,
                        13,3,0)) != MAD_OK)
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


    return MAD_OK;
}


static MAD_STATUS setIEEETest_1181
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    IN  MAD_BOOL        en,
    IN  MAD_IEEE_TEST_MODE  mode
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdDiagSetIEEETest called.\n");
    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    if (en)
    {
        /* enable IEEE Test */
        switch (mode)
        {
            case MAD_IEEE_TEST_WAVEFORM:
                data = 1;
                break;
            case MAD_IEEE_TEST_MASTER_JITTER:
                data = 2;
                break;
            case MAD_IEEE_TEST_SLAVE_JITTER:
                data = 3;
                break;
            case MAD_IEEE_TEST_DISTORTION:
                data = 4;
                break;
            default:
                MAD_DBG_ERROR("Unknown mode %i.\n",mode);
                return MAD_API_UNKNOWN_IEEE_TEST_MODE;
        }

        /* Enable TX_TCLK */
        if((retVal = madHwSetPagedPhyRegField(dev,hwPort,4,30,0,5,0x18))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
        /* Set the test mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_1000_CONTROL,
                        13,3,data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }
    }
    else
    {
        /* Disable TX_TCLK */
        if((retVal = madHwSetPagedPhyRegField(dev,hwPort,4,30,0,5,0))
            != MAD_OK)
        {
            MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

        /* Clear the test mode */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,0,MAD_REG_1000_CONTROL,
                        13,3,0)) != MAD_OK)
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


    return MAD_OK;
}


/*******************************************************************************
* mdDiagSetIEEETest
*
* DESCRIPTION:
*       This routine starts or stop IEEE test mode.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE to start, MAD_FALSE to stop
*       mode - MAD_IEEE_TEST_MODE, if en is MAD_TRUE
*              ignored, if en is MAD_FALSE
*              supported test modes are:
*                   MAD_IEEE_TEST_WAVEFORM,
*                   MAD_IEEE_TEST_MASTER_JITTER,
*                   MAD_IEEE_TEST_SLAVE_JITTER, or
*                   MAD_IEEE_TEST_DISTORTION
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       After IEEE Test, the followings need to be reconfigured:
*           Copper Speed/Duplex and Auto_Nego mode (mdCopperSetAutoNeg)
*
*       Page 0, Reg 9, Bit 15:13 : Test modes
*       Various hidden registers are accessed.
*
*******************************************************************************/
MAD_STATUS mdDiagSetIEEETest
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    IN  MAD_BOOL        en,
    IN  MAD_IEEE_TEST_MODE  mode
)
{
    MAD_U8      hwPort;

    MAD_DBG_INFO("mdDiagSetIEEETest called.\n");

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    switch(dev->phyInfo.ieeeTestType)
    {
        case MAD_PHY_IEEE_TEST_TYPE1:
            return setIEEETest_1181(dev,hwPort,en,mode);
        
        case MAD_PHY_IEEE_TEST_TYPE2:
        case MAD_PHY_IEEE_TEST_TYPE4: /* for 1340 */
            return setIEEETest_1149(dev,hwPort,en,mode);
 
        case MAD_PHY_IEEE_TEST_TYPE3:
            return setIEEETest_114X(dev,hwPort,en,mode);
 
        case MAD_PHY_IEEE_TEST_TYPE0:
        default:
            return MAD_API_IEEE_TEST_NOT_SUPPORT;
    }

    return MAD_OK;
}

