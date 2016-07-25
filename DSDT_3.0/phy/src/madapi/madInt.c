#include <madCopyright.h>

/********************************************************************************
* madInt.c
* 
* DESCRIPTION:
*       APIs for Interrupt event
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
* mdIntSetEnable
*
* DESCRIPTION:
*       Enable/Disable PHY Interrupts.
*       This register determines whether the INT# pin is asserted when an 
*       interrupt event occurs. When an interrupt occurs, the corresponding bit 
*       is set and remains set until register 19 is read via the SMI. 
*       When interrupt enable bits are not set in register 18, interrupt status 
*       bits in register 19 are still set when the corresponding interrupt events
*       occur. However, the INT# is not asserted.
*       There are two groups of interrups. Group 0 is for the interrupts 
*       related Copper and SGMII. Group 1 is for the interrupts related to the
*       Fiber. Since each PHY device defines the different set of interrupts,
*       please refer to the specific device's datasheet for details.
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*       port    - The logical port number
*       intType - the type of interrupt to enable/disable. 
*                 Group 0 is any combination of 
*                   MAD_COPPER_AUTONEGO_ERROR,
*                    MAD_COPPER_SPEED_CHANGED,
*                    MAD_COPPER_DUPLEX_CHANGED,
*                    MAD_COPPER_PAGE_RECEIVED,
*                    MAD_COPPER_AUTO_NEG_COMPLETED,
*                    MAD_COPPER_LINK_STATUS_CHANGED,
*                    MAD_COPPER_SYMBOL_ERROR,
*                    MAD_COPPER_FALSE_CARRIER,
*                    MAD_COPPER_CROSSOVER_CHANGED,
*                    MAD_COPPER_DOWNSHIFT_DETECT,
*                   MAD_COPPER_ENERGY_DETECT,
*                   MAD_COPPER_DTE_DETECT_CHANGED,
*                   MAD_COPPER_POLARITY_CHANGED,
*                   MAD_COPPER_JABBER, and
*                   High 16 bits:
*                    MAD_SGMII_TRANS_FIFO_FLOW.
*                    MAD_SGMII_RECV_FIFO_FLOW.
*                    MAD_SGMII_TRANS_FIFO_IDLE_INSERT
*                    MAD_SGMII_TRANS_FIFO_IDLE_DELET
*                    MAD_SGMII_RECV_FIFO_IDLE_INSERT
*                    MAD_SGMII_RECV_FIFO_IDLE_DELET
*
*                 Group 1 is any combination of
*                   MAD_FIBER_SPEED_CHANGED,
*                   MAD_FIBER_DUPLEX_CHANGED,
*                   MAD_FIBER_PAGE_RECEIVED,
*                   MAD_FIBER_AUTO_NEG_COMPLETED,
*                   MAD_FIBER_LINK_STATUS_CHANGED,
*                   MAD_FIBER_SYMBOL_ERROR,
*                   MAD_FIBER_FALSE_CARRIER,
*                   MAD_FIBER_FIFO_FLOW, and
*                   MAD_FIBER_ENERGY_DETECT
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 18, Bit 15:0 : Copper Interrupt Enable Register
*       Page 1, Reg 18, Bit 15:0 : Fiber Interrupt Enable Register (88E114x)
*       Page 2, Reg 18, Bit 7    : MAC Interface Interrupt (Overflow/Underflow)
*                                   (88E1181)
*
*******************************************************************************/
static MAD_STATUS mdIntSetEnable_f(MAD_DEV *dev, MAD_LPORT port, MAD_INT_TYPE *intType);
MAD_STATUS mdIntSetEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_INT_TYPE    *intType
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;
    MAD_BOOL isCopper=MAD_TRUE, isFiber=MAD_TRUE; 

  if(MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR))
  {
      return (mdIntSetEnable_f(dev,port, intType));
  }

  MAD_DBG_INFO("mdIntSetEnable called.\n");

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
    data = (MAD_U16)(intType->intGroup0 & 0xFFFF);

    /* Set Interrupt Enable Register for Copper */
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_INT_ENABLE,data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    /* SGMII Interrupt */
    data = (MAD_U16)((intType->intGroup0 >> 16) & 0xFFFF);

    /* Set Interrupt Enable Register for SGMII */
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,MAD_PAGE2,MAD_REG_MAC_INT_ENABLE,data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
    
  }
  else
  {
     isCopper=MAD_FALSE;
  }

  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_FIBER))
  {

    /* Fiber Interrupt */
    data = (MAD_U16)(intType->intGroup1 & 0xFFFF);

    /* Set Interrupt Enable Register for Fiber */
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,MAD_PAGE1,MAD_REG_FIBER_INT_ENABLE,data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

  }
  else
  {
     isFiber=MAD_FALSE;
  }

  
  if ((isFiber==MAD_FALSE)&&(isCopper==MAD_FALSE))
  {
     MAD_DBG_ERROR("Device shoud be ether Copper or Fiber.\n");
     return MAD_API_ERROR_SET_COPPER_FIBER;
  }

    return MAD_OK;
}

static MAD_STATUS mdIntSetEnable_f
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_INT_TYPE    *intType
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_BOOL isCopper=MAD_TRUE, isFiber=MAD_TRUE; 


  MAD_DBG_INFO("mdIntSetEnable_f called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  /* Set Media interface interrupt */
  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {

    /* Copper Interrupt */
    data = (MAD_U16)(intType->intGroup0 & 0xFFFF);

    /* Set Interrupt Enable Register for Copper */
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_INT_ENABLE,data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
  }

  if((MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_FIBER)) ||
     (MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_SGMII_MEDIA)))
  {

    /* Fiber Interrupt */
    data = (MAD_U16)(intType->intGroup1 & 0xFFFF);

    /* Set Interrupt Enable Register for Fiber */
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,MAD_PAGE1,MAD_REG_FIBER_INT_ENABLE,data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

  }

  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_QSGMII_MEDIA))
  {

    /* Fiber Interrupt */
    data = (MAD_U16)(intType->intGroup1 & 0xFFFF);

    /* Set Interrupt Enable Register for Fiber */
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,MAD_PAGE4,MAD_REG_QSGMII_INT_ENABLE,data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

  }

  /* Set Mac interface interrupt */

  if((MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_SGMII)) ||
     (MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_QSGMII)))
  {
    /* SGMII Interrupt */
    data = (MAD_U16)((intType->intGroup0 >> 16) & 0xFFFF);


    /* Set Interrupt Enable Register for SGMII */
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,MAD_PAGE2,MAD_REG_MAC_INT_ENABLE,data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
    
  }
  
  if ((isFiber==MAD_FALSE)&&(isCopper==MAD_FALSE))
  {
     MAD_DBG_ERROR("Device shoud be ether Copper or Fiber.\n");
     return MAD_API_ERROR_SET_COPPER_FIBER;
  }

    return MAD_OK;
}

/*******************************************************************************
* mdIntGetEnable
*
* DESCRIPTION:
*       Get interrupt setting
*       There are two groups of interrups. Group 0 is for the interrupts 
*       related Copper and SGMII. Group 1 is for the interrupts related to the
*       Fiber. Since each PHY device defines the different set of interrupts,
*       please refer to the specific device's datasheet for details.
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*       port    - The logical port number
*       intType - the type of interrupt which causes an interrupt.
*                  Group 0 is any combination of 
*                   MAD_COPPER_AUTONEGO_ERROR,
*                   MAD_COPPER_SPEED_CHANGED,
*                   MAD_COPPER_DUPLEX_CHANGED,
*                   MAD_COPPER_PAGE_RECEIVED,
*                   MAD_COPPER_AUTO_NEG_COMPLETED,
*                   MAD_COPPER_LINK_STATUS_CHANGED,
*                   MAD_COPPER_SYMBOL_ERROR,
*                   MAD_COPPER_FALSE_CARRIER,
*                   MAD_COPPER_CROSSOVER_CHANGED,
*                   MAD_COPPER_DOWNSHIFT_DETECT,
*                   MAD_COPPER_ENERGY_DETECT,
*                   MAD_COPPER_DTE_DETECT_CHANGED,
*                   MAD_COPPER_POLARITY_CHANGED,
*                   MAD_COPPER_JABBER, and
*                  High 16 bits:
*                   MAD_SGMII_TRANS_FIFO_FLOW.
*                   MAD_SGMII_RECV_FIFO_FLOW.
*                   MAD_SGMII_TRANS_FIFO_IDLE_INSERT
*                   MAD_SGMII_TRANS_FIFO_IDLE_DELET
*                   MAD_SGMII_RECV_FIFO_IDLE_INSERT
*                   MAD_SGMII_RECV_FIFO_IDLE_DELET
*                  Group 1 is any combination of
*                   MAD_FIBER_SPEED_CHANGED,
*                   MAD_FIBER_DUPLEX_CHANGED,
*                   MAD_FIBER_PAGE_RECEIVED,
*                   MAD_FIBER_AUTO_NEG_COMPLETED,
*                   MAD_FIBER_LINK_STATUS_CHANGED,
*                   MAD_FIBER_SYMBOL_ERROR,
*                   MAD_FIBER_FALSE_CARRIER,
*                   MAD_FIBER_FIFO_FLOW, and
*                   MAD_FIBER_ENERGY_DETECT
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 18, Bit 15:0 : Copper Interrupt Status Register
*       Page 1, Reg 18, Bit 15:0 : Fiber Interrupt Status Register (88E114x)
*       Page 2, Reg 18, Bit 7    : MAC Interface Interrupt (Overflow/Underflow)
*                                   (88E1181)
*
*******************************************************************************/
static MAD_STATUS mdIntGetEnable_f(MAD_DEV *dev, MAD_LPORT port, MAD_INT_TYPE *intType);
MAD_STATUS mdIntGetEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_INT_TYPE    *intType
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_BOOL isCopper=MAD_TRUE, isFiber=MAD_TRUE; 

  if(MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR))
  {
      return (mdIntGetEnable_f(dev,port, intType));
  }

  MAD_DBG_INFO("mdIntGetEnable called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
  }


  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {

    /* Get Interrupt Enable Register for Copper */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,0,MAD_REG_COPPER_INT_ENABLE,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* Copper Interrupt */
    intType->intGroup0 = data;

    /* Get Interrupt Enable Register for SGMII */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,2,MAD_REG_MAC_INT_ENABLE,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* SGMII Interrupt */
    intType->intGroup0 |= (data << 16);
  }
  else
  {
     isCopper=MAD_FALSE;
  }

  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_FIBER))
  {
    /* Get Interrupt Enable Register for Fiber */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,1,MAD_REG_FIBER_INT_ENABLE,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* Fiber Interrupt */
    intType->intGroup1 = data;
  }
  else
  {
     isFiber=MAD_FALSE;
  }
  
  if ((isFiber==MAD_FALSE)&&(isCopper==MAD_FALSE))
  {
     MAD_DBG_ERROR("Device shoud be ether Copper or Fiber.\n");
     return MAD_API_ERROR_SET_COPPER_FIBER;
  }



    return MAD_OK;
}

static MAD_STATUS mdIntGetEnable_f
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_INT_TYPE    *intType
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdIntGetEnable_f called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
  }


  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {

    /* Get Interrupt Enable Register for Copper */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_INT_ENABLE,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* Copper Interrupt */
    intType->intGroup0 = data;

  }

  if((MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_FIBER)) ||
     (MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_SGMII_MEDIA)))
  {
    /* Get Interrupt Enable Register for Fiber */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE1,MAD_REG_FIBER_INT_ENABLE,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* Fiber Interrupt */
    intType->intGroup1 = data;
  }

  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_QSGMII_MEDIA))
  {

    /* Get Interrupt Enable Register for Copper */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE4,MAD_REG_QSGMII_INT_ENABLE,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* Copper Interrupt */
    intType->intGroup0 = data;

  }
  if((MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_SGMII)) ||
     (MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_QSGMII)))
  {

    /* Get Interrupt Enable Register for SGMII */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE2,MAD_REG_MAC_INT_ENABLE,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* SGMII Interrupt */
    intType->intGroup0 |= (data << 16);
  }


    return MAD_OK;
}

/*******************************************************************************
* mdIntGetStatus
*
* DESCRIPTION:
*       Check to see if a specific type of interrupt occured
*       There are two groups of interrups. Group 0 is for the interrupts 
*       related Copper and SGMII. Group 1 is for the interrupts related to the
*       Fiber. Since each PHY device defines the different set of interrupts,
*       please refer to the specific device's datasheet for details.
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*       port    - The logical port number
*       intType - the type of interrupt which causes an interrupt.
*                  Group 0 is any combination of 
*                   MAD_COPPER_AUTONEGO_ERROR,
*                   MAD_COPPER_SPEED_CHANGED,
*                   MAD_COPPER_DUPLEX_CHANGED,
*                   MAD_COPPER_PAGE_RECEIVED,
*                   MAD_COPPER_AUTO_NEG_COMPLETED,
*                   MAD_COPPER_LINK_STATUS_CHANGED,
*                   MAD_COPPER_SYMBOL_ERROR,
*                   MAD_COPPER_FALSE_CARRIER,
*                   MAD_COPPER_CROSSOVER_CHANGED,
*                   MAD_COPPER_DOWNSHIFT_DETECT,
*                   MAD_COPPER_ENERGY_DETECT,
*                   MAD_COPPER_DTE_DETECT_CHANGED,
*                   MAD_COPPER_POLARITY_CHANGED,
*                   MAD_COPPER_JABBER, and
*                  High 16 bits:
*                   MAD_SGMII_TRANS_FIFO_FLOW.
*                   MAD_SGMII_RECV_FIFO_FLOW.
*                   MAD_SGMII_TRANS_FIFO_IDLE_INSERT
*                   MAD_SGMII_TRANS_FIFO_IDLE_DELET
*                   MAD_SGMII_RECV_FIFO_IDLE_INSERT
*                   MAD_SGMII_RECV_FIFO_IDLE_DELET
*                  Group 1 is any combination of
*                   MAD_FIBER_SPEED_CHANGED,
*                   MAD_FIBER_DUPLEX_CHANGED,
*                   MAD_FIBER_PAGE_RECEIVED,
*                   MAD_FIBER_AUTO_NEG_COMPLETED,
*                   MAD_FIBER_LINK_STATUS_CHANGED,
*                   MAD_FIBER_SYMBOL_ERROR,
*                   MAD_FIBER_FALSE_CARRIER,
*                   MAD_FIBER_FIFO_FLOW, and
*                   MAD_FIBER_ENERGY_DETECT
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 19, Bit 15:0 : Copper Interrupt Status Register
*       Page 1, Reg 19, Bit 15:0 : Fiber Interrupt Status Register (88E114x)
*       Page 2, Reg 19, Bit 7    : MAC Interface Interrupt (Overflow/Underflow)
*                                   (88E1181)
*
*******************************************************************************/
static MAD_STATUS mdIntGetStatus_f(MAD_DEV *dev, MAD_LPORT port, MAD_INT_TYPE *intType);
MAD_STATUS mdIntGetStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_INT_TYPE    *intType
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_BOOL isCopper=MAD_TRUE, isFiber=MAD_TRUE; 

  if(MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR))
  {
      return (mdIntGetStatus_f(dev,port, intType));
  }

  MAD_DBG_INFO("mdIntGetStatus called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
  }


  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {

    /* Get Interrupt Enable Register for Copper */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,0,MAD_REG_COPPER_STATUS_2,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* Copper Interrupt */
    intType->intGroup0 = data;

    /* Get Interrupt Enable Register for SGMII */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,2,MAD_REG_MAC_STATUS_2,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* SGMII Interrupt */
    intType->intGroup0 |= (data << 16);
  }
  else
  {
     isCopper=MAD_FALSE;
  }

  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_FIBER))
  {
    /* Get Interrupt Enable Register for Fiber */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,1,MAD_REG_FIBER_STATUS_2,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* Fiber Interrupt */
    intType->intGroup1 = data;
  }
  else
  {
     isFiber=MAD_FALSE;
  }
  
  if ((isFiber==MAD_FALSE)&&(isCopper==MAD_FALSE))
  {
     MAD_DBG_ERROR("Device shoud be ether Copper or Fiber.\n");
     return MAD_API_ERROR_SET_COPPER_FIBER;
  }



    return MAD_OK;
}

static MAD_STATUS mdIntGetStatus_f
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_INT_TYPE    *intType
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdIntGetStatus_f called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
  }


  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {

    /* Get Interrupt Enable Register for Copper */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE0,MAD_REG_COPPER_STATUS_2,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* Copper Interrupt */
    intType->intGroup0 = data;

  }

  if((MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_FIBER)) ||
     (MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_SGMII_MEDIA)))
  {
    /* Get Interrupt Enable Register for Fiber */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE1,MAD_REG_FIBER_STATUS_2,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* Fiber Interrupt */
    intType->intGroup1 = data;
  }

  if(MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_QSGMII_MEDIA))
  {

    /* Get Interrupt Enable Register for Copper */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE4,MAD_REG_QSGMII_STATUS_2,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* Copper Interrupt */
    intType->intGroup0 = data;

  }
  if((MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_SGMII)) ||
     (MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_QSGMII)))
  {

    /* Get Interrupt Enable Register for SGMII */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE2,MAD_REG_MAC_STATUS_2,&data))
        != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    /* SGMII Interrupt */
    intType->intGroup0 |= (data << 16);
  }


    return MAD_OK;
}

/*******************************************************************************
* mdIntGetPortSummary
*
* DESCRIPTION:
*       Lists the ports that have active interrupts. It provides a quick way to 
*       isolate the interrupt so that the MAC or switch does not have to poll the
*       interrupt status register (19) for all ports. Reading this register does 
*       not de-assert the INT# pin
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*
* OUTPUTS:
*       intPortVec - port vector with the bits set for the corresponding 
*                   ports with active interrupt. E.g., the bit number 0 and 2 are
*                   set when port number 0 and 2 have active interrupt
*                   For 1340 bit 15 indicates MacSec interrupt.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 23, Bit 7:0 : Copper Global Interrupt Status
*
*******************************************************************************/
MAD_STATUS mdIntGetPortSummary
(
    IN  MAD_DEV     *dev,
    OUT MAD_U32     *intPortVec
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_U8 page, reg;
  MAD_BOOL  fullVec = MAD_FALSE;


  MAD_DBG_INFO("mdIntGetPortSummary called.\n");

  /* translate LPORT to hardware port */
  hwPort = 0;

  switch(dev->phyInfo.interruptType)
  {
    case MAD_PHY_INTERRUPT_TYPE_1: /* For 30xx using reg 20 */
        page = MAD_PAGE0;
        reg = MAD_REG_COPPER_CONTROL_2;
        break;
    case MAD_PHY_INTERRUPT_TYPE_2:  /* using reg 23 */
        page = MAD_PAGE0;
        reg = MAD_REG_GLOBAL_INT_STATUS;
        break;
    case MAD_PHY_INTERRUPT_TYPE_0:
    default:
        return MAD_API_INTSUM_NOT_SUPPORT;
        break;
  }

  /* Get Interrupt Summary Register for Copper */
  if((retVal = madHwGetPagedPhyRegField(
              dev,hwPort,page,reg,0,dev->numOfPorts,&data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
    return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
  }

  *intPortVec = data;

    return MAD_OK;
}

/*******************************************************************************
* mdTempIntSetEnable
*
* DESCRIPTION:
*       Enable/Disable Temperature sensor Interrupt.
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*       port    - The logical port number
*
*        en        - MAD_TRUE: enable, MAD_FALSE: disable.
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 6, Reg 26, Bit 7    : (88E1340)
*
*******************************************************************************/
MAD_STATUS mdTempIntSetEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_BOOL    en
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;


  MAD_DBG_INFO("mdTempIntSetEnable called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR)))
  {
    MAD_DBG_ERROR("The temperature sensor interrup is not supported.\n");
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }

  /* Enable or disable */
  MAD_BOOL_2_BIT(en,data);

  /* Set Interrupt Enable Register for Temperature sensor */
  if((retVal = madHwSetPagedPhyRegField(
                 dev,hwPort,MAD_PAGE6,MAD_REG_MISC_TEST,7,1,data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Writing from paged phy reg failed.\n");
    return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
  }

  return MAD_OK;
}

/*******************************************************************************
* mdTempIntGetEnable
*
* DESCRIPTION:
*       Get Enable/Disable Temperature sensor Interrupt status.
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*       port    - The logical port number
*
* OUTPUTS:
*        en        - MAD_TRUE: enable, MAD_FALSE: disable.
*
* RETURNS:
*       MAD_OK - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 6, Reg 26, Bit 7    : (88E1340)
*
*******************************************************************************/
MAD_STATUS mdTempIntGetEnable
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    OUT  MAD_BOOL   *en
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdIntGetEnable_f called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR)))
  {
    MAD_DBG_ERROR("The temperature sensor interrup is not supported.\n");
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }


  /* Get Interrupt Enable Register for Temperature sensor */
  if((retVal = madHwGetPagedPhyRegField(
                 dev,hwPort,MAD_PAGE6,MAD_REG_MISC_TEST,7,1,&data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
    return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
  }

  MAD_BIT_2_BOOL(data,*en);
  return MAD_OK;
}

/*******************************************************************************
* mdTempIntGetStatus
*
* DESCRIPTION:
*       Get Temperature sensor Interrupt status.
*
* INPUTS:
*       dev     - MAD driver structure returned from mdLoadDriver
*       port    - The logical port number
*
* OUTPUTS:
*        status    - MAD_TRUE: Temperature reached threshold, MAD_FALSE: not reched.
*
* RETURNS:
*       MAD_OK - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 6, Reg 26, Bit 6    : (88E1340)
*
*******************************************************************************/
MAD_STATUS mdTempIntGetStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    OUT  MAD_BOOL   *status
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("mdIntGetStatus_f called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY(dev, MAD_PHY_FULL_PAGE_ADDR)))
  {
    MAD_DBG_ERROR("The temperature sensor interrup is not supported.\n");
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }


  /* Get Interrupt Status for Temperature sensor */
  if((retVal = madHwGetPagedPhyRegField(
                 dev,hwPort,MAD_PAGE6,MAD_REG_MISC_TEST,6,1,&data)) != MAD_OK)
  {
    MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
    return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
  }

  MAD_BIT_2_BOOL(data,*status);

  return MAD_OK;
}

