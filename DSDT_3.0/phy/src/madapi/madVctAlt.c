#include <madCopyright.h>

/********************************************************************************
* madVctAlt.c
* 
* DESCRIPTION:
*       APIs for VCT (Altaced Alternate VCT).
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
#include "madSMVct.h"

/* #include <math.h> */

#define MAD_ALT_VCT_ACCEPTABLE_SHORT_CABLE  11



/************************************************************************************

*************************************************************************************/


static 
MAD_STATUS runAltCableTest_1340_set
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_U32             diagType,
    IN  MAD_U16             cableLenUnit
)
{
    MAD_STATUS retVal;
    MAD_U16 u16Data;


    MAD_DBG_INFO("runAltCableTest_1340_set Called.\n");

    /*
     * Check Alt VCT Mode
     */
    if (!(diagType&MAD_ALT_VCT_DIAG_TYPE_MASK))
    {
      MAD_DBG_ERROR("Unknown ALT VCT type.\n");
      return MAD_API_UNKNOWN_ALTVCT_MODE;
    }

    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE7,
                     MAD_REG_PHY_CBL_DIAG_CTRL, &u16Data)) != MAD_OK)
    {
       MAD_DBG_ERROR("Reading paged phy reg failed.\n");
       return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }
    u16Data &= ~(MAD_ALT_VCT_DIAG_TYPE_MASK|MAD_ALT_VCT_CABLE_LENGTH_UNIT_BIT);
    u16Data |= (diagType|cableLenUnit);
    if((retVal = madHwWritePagedPhyReg(
                    dev,hwPort,MAD_PAGE7,MAD_REG_PHY_CBL_DIAG_CTRL,u16Data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

      /* 
      * start Alterbate Virtual Cable Tester
      */
      if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE7,
                        MAD_REG_PHY_CBL_DIAG_CTRL,15,1,1)) != MAD_OK)
      {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }


    return MAD_OK;
}

static 
MAD_STATUS runAltCableTest_1340_check
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;
    MAD_U16 u16Data;
    /* 
     * loop until test completion and result is valid
     */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE7,
                       MAD_REG_PHY_CBL_DIAG_CTRL,&u16Data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }
    if (u16Data & 0x0800)
        return MAD_PENDING;

    MAD_DBG_INFO("Page 7 of Reg21 after test : %0#x.\n", u16Data);

    return MAD_OK;
}

static 
MAD_STATUS runAltCableTest_1340_get
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    OUT MAD_ALT_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS retVal;
    int j;
    MAD_U16  diagCtrl, diagResult;

    VCT_REGISTER regList[MAD_MDI_PAIR_NUM] = {
                    {7,16},{7,17},{7,18},{7,19} };

    MAD_DBG_INFO("runAltCableTest_1340_get Called.\n");


    if ((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE7,
                    MAD_REG_PHY_CBL_DIAG_CTRL,&diagCtrl)) != MAD_OK)
    {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    MAD_DBG_INFO("Page 7 of Reg21 after test : %0#x.\n", diagCtrl);

    if ((retVal = madHwReadPagedPhyReg(dev,hwPort,MAD_PAGE7,
                    MAD_REG_PHY_CBL_DIAG_RESULT,&diagResult)) != MAD_OK)
    {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    MAD_DBG_INFO("Page 7 of Reg20 after test : %0#x.\n", diagResult);
    for (j=0; j<MAD_MDI_PAIR_NUM; j++)
    {
       cableStatus->cableFaultCode[j] = (diagResult>> (j*4)) & 0xf;
    }
    /*
     * read the test result for the cross pair against selected MDI Pair
     */
    for (j=0; j<MAD_MDI_PAIR_NUM; j++)
    {
      if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,
                        regList[j].page,
                        regList[j].regOffset,
                        (MAD_U16*)&(cableStatus->cableLen[j]))) != MAD_OK)
      {
         MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
         return (retVal | MAD_API_FAIL_READ_PAGED_REG);
      }
      MAD_DBG_INFO("@@@@@ reg channel %d is %x \n", j, cableStatus->cableLen[j]);
    }

    return MAD_OK;
}


#ifdef MAD_SM_VCT
/***********************************************************************************
*  AltVct_1340 SM block 
************************************************************************************/

#define madAlt_1340_diagType        vctSmCfg->savedVeriable[0]
#define madAlt_1340_cableLenUnit    vctSmCfg->savedVeriable[1]

static 
MAD_STATUS advVct_1340_init
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{

    return MAD_OK;
}

static 
MAD_STATUS advVct_1340_set
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;

  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);

    if((retVal=runAltCableTest_1340_set(dev,hwPort, madAlt_1340_diagType, madAlt_1340_cableLenUnit)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Running alternate VCT failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_SET);
    }


    return MAD_OK;
}

static 
MAD_STATUS advVct_1340_check
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;

    retVal=runAltCableTest_1340_check(dev,hwPort);

    return retVal;
}

static 
MAD_STATUS advVct_1340_get
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
  MAD_STATUS retVal;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
  MAD_ALT_CABLE_STATUS *cableStatus= (MAD_ALT_CABLE_STATUS *)(vctSmCfg->vctResultPtr);



  if((retVal=runAltCableTest_1340_get(dev,hwPort, cableStatus)) != MAD_OK)
    {
        MAD_DBG_ERROR("Running alternate VCT SM get failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_GET);
    }
   return MAD_OK;
}

static 
MAD_STATUS advVct_1340_close
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{

    madVCTCloseSM (dev);
    return MAD_OK;
}

static    MAD_VCT_SM_ACTIONS  advVctSmActions = {
                                        advVct_1340_init,
                                        advVct_1340_set,
                                        advVct_1340_check,
                                        advVct_1340_get,
                                        advVct_1340_set,
                                        advVct_1340_get,
                                        advVct_1340_close
                                        };

static 
MAD_STATUS getAltCableStatus_1340
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_U32             diagType,
    OUT MAD_ALT_CABLE_STATUS *cableStatus
)
{
   MAD_STATUS retVal;
   MAD_32 checkCount, subCheckCount;

   MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
   vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);

   madAlt_1340_diagType = diagType;
   madAlt_1340_cableLenUnit = cableStatus->cableLenUnit;

       checkCount=1; /* 4; */
       subCheckCount=1;  /* 4; */
    if((retVal = madVCTFindSM(dev, hwPort, MAD_VCT_SM_VCT_TYPE_ADV_TDR,
                  &advVctSmActions, MAD_TRUE, 
                  (MAD_VCT_DATA_STRUCT *)cableStatus, checkCount, subCheckCount)) != MAD_OK)
    {
        MAD_DBG_ERROR("Can not find VCT State machine instatnce.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_INSTANCE);
    }

    if(((retVal = madVCTImplSM(dev, hwPort, MAD_VCT_SM_EVT_CMD)) != MAD_OK)&&
            ((retVal&MAD_RUN_ST_MASK) != MAD_PENDING))
    {
        MAD_DBG_ERROR("Can not find VCT State machine implement.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_IMPLEM);
    }
    return retVal;
}

#else

static 
MAD_STATUS runAltCableTest_1340
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_U32             diagType,
    OUT MAD_ALT_CABLE_STATUS *cableStatus
)
{
  MAD_STATUS retVal;

  MAD_DBG_INFO("runAltCableTest_1340 Called.\n");


    /* Set transmit channel */
    if((retVal=runAltCableTest_1340_set(dev,hwPort, diagType, cableStatus->cableLenUnit)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Running alternate VCT failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_SET);
    }


    /* 
     * loop until test completion and result is valid
     */
    do 
    {
      if (((retVal=runAltCableTest_1340_check(dev,hwPort)) != MAD_OK)&&
            ((retVal&MAD_RUN_ST_MASK) != MAD_PENDING))
      {
        MAD_DBG_ERROR("Running alternate VCT failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_CHECK);
      }
    } while((retVal&MAD_RUN_ST_MASK) == MAD_PENDING);

        /*
         * read the test result for the cross pair against selected MDI Pair
         */
      if((retVal=runAltCableTest_1340_get(dev,hwPort, cableStatus)) != MAD_OK)
      {
        MAD_DBG_ERROR("Running alternate VCT SM get failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_GET);

      }
  return MAD_OK;
}

static 
MAD_STATUS getAltCableStatus_1340
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_U32             diagType,
    OUT MAD_ALT_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS retVal;

    MAD_CRITIC_INFO("Enter ALT VCT Non-Pendding Mode.\n");


    if((retVal=runAltCableTest_1340(dev,hwPort,diagType, cableStatus)) != MAD_OK)
    {
        MAD_DBG_ERROR("Running alternate VCT failed.\n");
        return (retVal | MAD_API_FAIL_ALTVCT_RUN);
    }

    return MAD_OK;
}



#endif

/*******************************************************************************
* mdDiagGetAltCableStatus
*
* DESCRIPTION:
*       This routine performs the alternate VCT for the requested port,
*       and returns the status per MDI pairand cable length.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       diagType - advance VCT type:
*                    MAD_ALT_VCT_RUN_AUTONEGO_CYCLE_BIT |
*                    MAD_ALT_VCT_DISABLE_CROSS_PAIR_BIT  |
*                    MAD_ALT_VCT_RUN_AFTER_BREAK_LNK_BIT
*
* OUTPUTS:
*       cableStatus - test status and cable length
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:/
*       Page 0, Reg 0, Bit 15    : Soft reset
*       Page 7, Reg 16-21, 
*
*******************************************************************************/
MAD_STATUS mdDiagGetAltCableStatus
(
    IN  MAD_DEV*        dev,
    IN  MAD_LPORT       port,
    IN  MAD_U32            diagType,
    OUT MAD_ALT_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS retVal;
    MAD_U8 hwPort;

    MAD_DBG_INFO("madDiagGetAltCableStatus Called.\n");

    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_ALT_VCT_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("Alterbate VCT not supported.\n");
        return MAD_API_ALTTDRVCT_NOT_SUPPORT;
    }

    switch (dev->deviceId)
    {
        case MAD_88E1340S:   
        case MAD_88E1340:   
        case MAD_88E1340M:   
        case MAD_88E1540:   
         if(((retVal=getAltCableStatus_1340(dev,hwPort,diagType,cableStatus)) != MAD_OK) &&
             ((retVal&MAD_RUN_ST_MASK)!=MAD_PENDING))
         {
            MAD_DBG_ERROR("Running alternate VCT failed.\n");
            return (retVal | MAD_API_FAIL_ALTVCT_RUN);
         }
         if ((retVal&MAD_RUN_ST_MASK)==MAD_PENDING)
         {
             return retVal;
         }
         break;
        default:
            return MAD_API_ALTTDRVCT_NOT_SUPPORT;
    }


    /*
     * reset the phy
     */
   if((madHwPagedReset(dev,hwPort,0)) != MAD_OK)
    {
        MAD_DBG_ERROR("Soft Reset failed.\n");
        return (MAD_API_FAIL_SW_RESET);
    }

    return MAD_OK;
}

