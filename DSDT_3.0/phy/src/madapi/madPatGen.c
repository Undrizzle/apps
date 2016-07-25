#include <madCopyright.h>

/********************************************************************************
* madPatGen.c
*
* DESCRIPTION:
*       APIs to implemet test of PRBS Generator and Counter 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include <madApi.h>
#include <madApiInternal.h>
#include <madHwCntl.h>
#include <madDrvPhyRegs.h>
#include <madDrvConfig.h>

/* QSGNII PRBS */
/*******************************************************************************
* madPatCtrlSetPRBS
*
* DESCRIPTION:
*       This function select PRBS test -  PRBS 7, PRBS23, PRBS31.
*
* INPUTS:
*       port - The logical port number
*        genCfg - PRBS generator config structure: MAD_PRBS_GEN_CONFIG
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madPatCtrlSetPRBS
(
    IN  MAD_DEV                *dev,
    IN  MAD_LPORT            port,
    IN  MAD_PRBS_GEN_CONFIG *genCfg
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_U16        data;

  MAD_DBG_INFO("madPatCtrlSetPRBS called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY1(dev, MAD_PHY_QSGMII_PRBS_GEN)))
  {
    MAD_DBG_ERROR("Pattern generator not supported.\n");
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }

  data = (genCfg->polarCheckInv&1)<<7 | 
           (genCfg->polarGenInv&1)<<6 | 
           (genCfg->counterLock&1)<<5 | 
           (genCfg->patternSel&0x3)<<2;
  if (genCfg->genEn)
    data |= 1; /* enable PRBS generator */
  if (genCfg->countEn)
    data |= (1<<1); /* enable PRBS generator */

  if((retVal = madHwWritePagedPhyReg(dev,hwPort,MAD_PAGE4,MAD_REG_QSGMII_PRBS_CTRL,data))
      != MAD_OK)
  {
    MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",MAD_PAGE4,MAD_REG_QSGMII_PRBS_CTRL);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }

  return MAD_OK;

}

/*******************************************************************************
* madPatCtrlGetPRBS
*
* DESCRIPTION:
*       This function get PRBS test setting -  PRBS 7, PRBS23, PRBS31.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        genCfg - PRBS generator config structure: MAD_PRBS_GEN_CONFIG
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madPatCtrlGetPRBS
(
    IN  MAD_DEV                *dev,
    IN  MAD_LPORT            port,
    OUT  MAD_PRBS_GEN_CONFIG *genCfg
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_U16        data;

  MAD_DBG_INFO("madPatCtrlGetPRBS called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY1(dev, MAD_PHY_QSGMII_PRBS_GEN)))
  {
    MAD_DBG_ERROR("Pattern generator not supported.\n");
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }

  if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE4,MAD_REG_QSGMII_PRBS_CTRL,&data))
      != MAD_OK)
  {
    MAD_DBG_ERROR("Reading to paged phy reg (%i,%i) failed.\n",MAD_PAGE4,MAD_REG_QSGMII_PRBS_CTRL);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
  genCfg->polarCheckInv = (data>>7)&0x1;
  genCfg->polarGenInv = (data>>6)&0x1;
  genCfg->counterLock = (data>>5)&0x1;
  genCfg->patternSel = (data>>2)&0x3;

  genCfg->genEn = data&1;
  genCfg->countEn = (data>>1)&1;

  if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE4,MAD_REG_QSGMII_PRBS_ERR_COUNT_LSB,&data))
      != MAD_OK)
  {
    MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",MAD_PAGE4,MAD_REG_QSGMII_PRBS_CTRL);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
  genCfg->errCounter = data;
  if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE4,MAD_REG_QSGMII_PRBS_ERR_COUNT_MSB,&data))
      != MAD_OK)
  {
    MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",MAD_PAGE4,MAD_REG_QSGMII_PRBS_CTRL);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
  genCfg->errCounter &= 0x0000ffff;
  genCfg->errCounter |= (data<<16);

  return MAD_OK;

}

/*******************************************************************************
* madResetCounters
*
* DESCRIPTION:
*       This function reset Counter. 
*
* INPUTS:
*        None
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madResetPRBSCounters
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("madPatCtrlGetPRBS31 called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY1(dev, MAD_PHY_QSGMII_PRBS_GEN)))
  {
    MAD_DBG_ERROR("Pattern generator not supported.\n");
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }

  /* Reset counter */
  if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE4,MAD_REG_QSGMII_PRBS_CTRL,4,1,1))
     != MAD_OK)
  {
    MAD_DBG_ERROR("Writing phy reg failed.\n");
    return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }
  if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE4,MAD_REG_QSGMII_PRBS_CTRL,4,1,0))
     != MAD_OK)
  {
    MAD_DBG_ERROR("Writing phy reg failed.\n");
    return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }
  return MAD_OK;
}


/* 1.25G1.25G  PRBS */
/*******************************************************************************
* madPatCtrlSet125GPRBS
*
* DESCRIPTION:
*       This function select 1.25G PRBS test.
*
* INPUTS:
*       port - The logical port number
*        genCfg - PRBS generator config structure: MAD_PRBS_GEN_CONFIG
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madPatCtrlSet125GPRBS
(
    IN  MAD_DEV                *dev,
    IN  MAD_LPORT            port,
    IN  MAD_PRBS_GEN_CONFIG *genCfg
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_U16        data;

  MAD_DBG_INFO("madPatCtrlSet125GPRBS called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY1(dev, MAD_PHY_125G_PRBS_GEN)))
  {
    MAD_DBG_ERROR("Pattern generator not supported.\n");
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }

  data = (genCfg->polarCheckInv&1)<<7 | 
           (genCfg->polarGenInv&1)<<6 | 
           (genCfg->counterLock&1)<<5;
  if (genCfg->genEn)
    data |= 1; /* enable PRBS generator */
  if (genCfg->countEn)
    data |= (1<<1); /* enable PRBS generator */

  if((retVal = madHwWritePagedPhyReg(dev,hwPort,MAD_PAGE1,MAD_REG_FIBER_PRBS_CTRL,data))
      != MAD_OK)
  {
    MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",MAD_PAGE1,MAD_REG_FIBER_PRBS_CTRL);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }

  return MAD_OK;

}

/*******************************************************************************
* madPatCtrlGet125GPRBS
*
* DESCRIPTION:
*       This function get 1.25G PRBS test setting
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        genCfg - PRBS generator config structure: MAD_PRBS_GEN_CONFIG
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madPatCtrlGet125GPRBS
(
    IN  MAD_DEV                *dev,
    IN  MAD_LPORT            port,
    OUT  MAD_PRBS_GEN_CONFIG *genCfg
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_U16        data;

  MAD_DBG_INFO("madPatCtrlGet125GPRBS called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY1(dev, MAD_PHY_125G_PRBS_GEN)))
  {
    MAD_DBG_ERROR("Pattern generator not supported.\n");
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }

  if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE1,MAD_REG_FIBER_PRBS_CTRL,&data))
      != MAD_OK)
  {
    MAD_DBG_ERROR("Reading to paged phy reg (%i,%i) failed.\n",MAD_PAGE1,MAD_REG_FIBER_PRBS_CTRL);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
  genCfg->polarCheckInv = (data>>7)&0x1;
  genCfg->polarGenInv = (data>>6)&0x1;
  genCfg->counterLock = (data>>5)&0x1;

  genCfg->genEn = data&1;
  genCfg->countEn = (data>>1)&1;

  if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE1,MAD_REG_FIBER_PRBS_ERR_COUNT_LSB,&data))
      != MAD_OK)
  {
    MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",MAD_PAGE1,MAD_REG_FIBER_PRBS_CTRL);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
  genCfg->errCounter = data;
  if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE1,MAD_REG_FIBER_PRBS_ERR_COUNT_MSB,&data))
      != MAD_OK)
  {
    MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",MAD_PAGE1,MAD_REG_FIBER_PRBS_CTRL);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }
  genCfg->errCounter &= 0x0000ffff;
  genCfg->errCounter |= (data<<16);

  return MAD_OK;

}

/*******************************************************************************
* madReset125GPRBSCounters
*
* DESCRIPTION:
*       This function reset Counter. 
*
* INPUTS:
*        None
*
* OUTPUTS:
*        None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madResetCounters
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO("madPatCtrlGetPRBS31 called.\n");

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!(MAD_DEV_CAPABILITY1(dev, MAD_PHY_125G_PRBS_GEN)))
  {
    MAD_DBG_ERROR("Pattern generator not supported.\n");
    return MAD_API_NEW_FEATURE_NOT_SUPPORT;
  }

  /* Reset counter */
  if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE1,MAD_REG_FIBER_PRBS_CTRL,4,1,1))
     != MAD_OK)
  {
    MAD_DBG_ERROR("Writing phy reg failed.\n");
    return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }
  if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE1,MAD_REG_FIBER_PRBS_CTRL,4,1,0))
     != MAD_OK)
  {
    MAD_DBG_ERROR("Writing phy reg failed.\n");
    return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }
  return MAD_OK;
}

