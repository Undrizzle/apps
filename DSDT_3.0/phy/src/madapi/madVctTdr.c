#include <madCopyright.h>

/********************************************************************************
* madVctTdr.c
* 
* DESCRIPTION:
*       APIs for VCT (TDR).
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
#ifdef MAD_SM_VCT
static MAD_SM_DATA_STRUCT vctSmData;
#endif

#define MAD_LOOKUP_TABLE_ENTRY  128  /* 73 */


/*******************************************************************************
* analizeVCTResult
*
* DESCRIPTION:
*       This routine analize the virtual cable test result for the PHY device
*
* INPUTS:
*       regValue - test result
*
* OUTPUTS:
*       cableStatus - analized test result.
*       cableLen    - cable length or the distance where problem occurs.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static
MAD_STATUS analizeVCTResult
(
    IN  MAD_DEV *dev,
    IN  MAD_U16 regValue, 
    OUT MAD_TEST_STATUS *cableStatus,
    OUT MAD_CABLE_LEN *cableLen
)
{
    int len;

    MAD_DBG_INFO("analizeVCTResult Called.\n");

    switch((regValue & 0x6000) >> 13)
    {
        case 0:

            /* Check Impedance Mismatch */
            if ((regValue & 0xFF) < 0xFF)
            {
                /*  if the reflected amplitude is low it is good cable too.
                    for this registers values it is a good cable:
                    0xE23, 0xE24, 0xE25, 0xE26, 0xE27 */
                if ((regValue < 0xE23) || (regValue > 0xE27))
                {
                    *cableStatus = MAD_IMPEDANCE_MISMATCH;
                    len = (int)MAD_VCT_CALC(regValue & 0xFF);
                    if(len <= 0)
                        cableLen->errCableLen = 0;
                    else
                        cableLen->errCableLen = (MAD_U8)len;
                    break;
                }
            }

            /* test passed. No problem found. */
            *cableStatus = MAD_NORMAL_CABLE;
            cableLen->normCableLen = MAD_UNKNOWN_LEN;

            break;
        case 1:
            /* test passed. Cable is short. */
            *cableStatus = MAD_SHORT_CABLE;
            len = (int)MAD_VCT_CALC(regValue & 0xFF);
            if(len <= 0)
                cableLen->errCableLen = 0;
            else
                cableLen->errCableLen = (MAD_U8)len;
            break;

        case 2:
            /* test passed. Cable is open. */
            *cableStatus = MAD_OPEN_CABLE;
            len = (int)MAD_VCT_CALC(regValue & 0xFF);
            if(len <= 0)
                cableLen->errCableLen = 0;
            else
                cableLen->errCableLen = (MAD_U8)len;
            break;

        case 3:
        default:
            /* test failed. No result is valid. */
            *cableStatus = MAD_TEST_FAIL;
            break;
    }

    return MAD_OK;
}


#if MAD_SM_VCT
/*************************************************************************
*
**************************************************************************/
/*
static    MAD_U16 reg30_12, reg30_15;
static MAD_U16 reg0, reg16;
*/

#define madVct_3016_reg30_12        vctSmCfg->savedVeriable[0]
#define madVct_3016_reg30_15        vctSmCfg->savedVeriable[1]
#define madVct_3016_reg0            vctSmCfg->savedVeriable[2]
#define madVct_3016_reg16           vctSmCfg->savedVeriable[3]
#else
MAD_U16 madVct_3016_reg30_12;
MAD_U16 madVct_3016_reg30_15;
MAD_U16 madVct_3016_reg0;
MAD_U16 madVct_3016_reg16;
#endif

#ifdef MAD_SM_VCT

static MAD_STATUS runVctAgain_3016_set
(
    IN  MAD_DEV *dev,
    IN  MAD_U8  hwPort
)
{
    MAD_STATUS status;
#if MAD_SM_VCT
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
#endif

    if((status= madHwReadPagedPhyReg(dev,hwPort,12,30,(MAD_U16 *)&madVct_3016_reg30_12)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
        return (status | MAD_API_FAIL_READ_PAGED_REG);
    }
    if((status= madHwReadPagedPhyReg(dev,hwPort,15,30,(MAD_U16 *)&madVct_3016_reg30_15)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
        return (status | MAD_API_FAIL_READ_PAGED_REG);
    }

    if((status= madHwWritePagedPhyReg(dev,hwPort,12,30,0x4100)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (status | MAD_API_FAIL_WRITE_PAGED_REG);
    }
    if((status = madHwSetPagedPhyRegField(dev,hwPort,15,30,0,4,0x03)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (status | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }



    status=mdSysSetPhyReg (dev, hwPort, 26, 0x8000);
    if (status != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return status;
    }

    return MAD_OK;
} 

static 
MAD_STATUS getVctSt_26_check
(    
    IN  MAD_DEV         *dev,
    IN  MAD_U8          hwPort
);


static MAD_STATUS runVctAgain_3016_get
(
    IN  MAD_DEV *dev,
    IN  MAD_U8  hwPort,
    OUT MAD_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS status;
    MAD_U16 reg26;
    int len;
    int i;
#if MAD_SM_VCT
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
#endif

    for (i=0; i<(MAD_MDI_PAIR_NUM/2); i++)
    {
        /*
        * read the test result for the selected MDI Pair
        */
        status=mdSysGetPhyReg (dev, hwPort, 26+i, (MAD_U32 *)&reg26);
        if (status != MAD_OK)
        {
            return status;
        }
    /* MAD_DBG_INFO ("@@@ 3016 get reg_%i value: %x\n", 26+pairNo, reg26);  */
        
       switch((reg26 & 0x6000) >> 13)
       {
         case 0:
         /* Check Impedance Mismatch */
         if ((reg26 & 0xFF) < 0xFF)
         {
            /*  if the reflected amplitude is low it is good cable too.
                for this registers values it is a good cable:
                0xE23, 0xE24, 0xE25, 0xE26, 0xE27 */
            if ((reg26 < 0xE23) || (reg26 > 0xE27))
            {
                cableStatus->cableStatus[i] = MAD_IMPEDANCE_MISMATCH;
                len = (int)MAD_VCT_3016_CALC(reg26 & 0xFF);
                if(len <= 0)
                    cableStatus->cableLen[i].errCableLen = 0;
                else
                    cableStatus->cableLen[i].errCableLen = (MAD_U8)len;
                break;
            }
         }

         /* test passed. No problem found. */
          cableStatus->cableStatus[i] = MAD_NORMAL_CABLE;
          cableStatus->cableLen[i].normCableLen = MAD_UNKNOWN_LEN;

          break;

          case 2:
            /* test passed. Cable is open. */
           cableStatus->cableStatus[i] = MAD_OPEN_CABLE;
           len = (int)MAD_VCT_3016_CALC(reg26 & 0xFF);
           if(len <= 0)
                cableStatus->cableLen[i].errCableLen = 0;
            else
                cableStatus->cableLen[i].errCableLen = (MAD_U8)len;
            break;
         case 1:
         case 3:
         default:
            /* test failed. No result is valid. */
            cableStatus->cableStatus[i] = MAD_TEST_FAIL;
            break;
        } /* switch */
      }
     if((status= madHwWritePagedPhyReg(dev,hwPort,12,30,madVct_3016_reg30_12)) != MAD_OK)
     {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (status | MAD_API_FAIL_WRITE_PAGED_REG);
     }
     if((status= madHwWritePagedPhyReg(dev,hwPort,15,30,madVct_3016_reg30_15)) != MAD_OK)
     {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (status | MAD_API_FAIL_WRITE_PAGED_REG);
     }
    return MAD_OK;
} 


static
MAD_STATUS analizeVCTResult_3016
(
    IN  MAD_DEV *dev,
    IN  MAD_U8  hwPort,
    IN  MAD_U16 regValue, 
    IN    MAD_32    pairNo,
    OUT MAD_TEST_STATUS *cableStatus,
    OUT MAD_CABLE_LEN *cableLen
)
{
    int len;

    MAD_DBG_INFO("analizeVCTResult_3016 Called.\n");


    switch((regValue & 0x6000) >> 13)
    {
        case 0:

            /* Check Impedance Mismatch */
            if ((regValue & 0xFF) < 0xFF)
            {
                /*  if the reflected amplitude is low it is good cable too.
                    for this registers values it is a good cable:
                    0xE23, 0xE24, 0xE25, 0xE26, 0xE27 */
                if ((regValue < 0xE23) || (regValue > 0xE27))
                {
                    *cableStatus = MAD_IMPEDANCE_MISMATCH;
                    len = (int)MAD_VCT_3016_CALC(regValue & 0xFF);
                    if(len <= 0)
                        cableLen->errCableLen = 0;
                    else
                        cableLen->errCableLen = (MAD_U8)len;
                    break;
                }
            }

            /* test passed. No problem found. */
            *cableStatus = MAD_NORMAL_CABLE;
            cableLen->normCableLen = MAD_UNKNOWN_LEN;

            break;
        case 1:
            /* test passed. Cable is short. */
            *cableStatus = MAD_SHORT_CABLE;
            cableLen->errCableLen = 0;
            break;

        case 2:
            /* test passed. Cable is open. */
            *cableStatus = MAD_OPEN_CABLE;
            len = (int)MAD_VCT_3016_CALC(regValue & 0xFF);
/* MAD_DBG_INFO("!!!!!!!!! analizeVCTResult_3016  len %x \n", len);  */
            if(len >= 0x20)
            {
              cableLen->errCableLen = (MAD_U8)len;
              break;
            }
            else
              return MAD_API_FAIL_VCT_CABLELEN;
            break;

        case 3:
        default:
            /* test failed. No result is valid. */
            *cableStatus = MAD_TEST_FAIL;
/*            break; */
            return MAD_API_FAIL_VCT_TEST;
    }

    return MAD_OK;
}

#else
static
MAD_STATUS analizeVCTResult_3016_0
(
    IN  MAD_DEV *dev,
    IN  MAD_U8  hwPort,
    IN  MAD_U16 regValue, 
    IN    MAD_32    pairNo,
    OUT MAD_TEST_STATUS *cableStatus,
    OUT MAD_CABLE_LEN *cableLen
)
{
    MAD_STATUS status;
    MAD_U16 reg26;
    int count=0x1000000;
    int len;

    MAD_DBG_INFO("analizeVCTResult_3016 Called.\n");

/* MAD_DBG_INFO ("!!! 3016 get reg_%i value: %x\n", 26+pairNo, regValue);  */

    switch((regValue & 0x6000) >> 13)
    {
        case 0:

            /* Check Impedance Mismatch */
            if ((regValue & 0xFF) < 0xFF)
            {
                /*  if the reflected amplitude is low it is good cable too.
                    for this registers values it is a good cable:
                    0xE23, 0xE24, 0xE25, 0xE26, 0xE27 */
                if ((regValue < 0xE23) || (regValue > 0xE27))
                {
                    *cableStatus = MAD_IMPEDANCE_MISMATCH;
                    len = (int)MAD_VCT_3016_CALC(regValue & 0xFF);
                    if(len <= 0)
                        cableLen->errCableLen = 0;
                    else
                        cableLen->errCableLen = (MAD_U8)len;
                    break;
                }
            }

            /* test passed. No problem found. */
            *cableStatus = MAD_NORMAL_CABLE;
            cableLen->normCableLen = MAD_UNKNOWN_LEN;

            break;
        case 1:
            /* test passed. Cable is short. */
            *cableStatus = MAD_SHORT_CABLE;
            cableLen->errCableLen = 0;
            break;

        case 2:
            /* test passed. Cable is open. */
            *cableStatus = MAD_OPEN_CABLE;
            len = (int)MAD_VCT_3016_CALC(regValue & 0xFF);
/* MAD_DBG_INFO("!!!!!!!!! analizeVCTResult_3016  len %x \n", len);  */
            if(len >= 0x20)
            {
              cableLen->errCableLen = (MAD_U8)len;
              break;
            }
            else
            {
                MAD_U16 reg30_12, reg30_15;
/*                MAD_U16 reg30_12_tmp, reg30_15_tmp; */

                if((status= madHwReadPagedPhyReg(dev,hwPort,12,30,&reg30_12)) != MAD_OK)
                {
                    MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
                    return (status | MAD_API_FAIL_READ_PAGED_REG);
                }
                if((status= madHwReadPagedPhyReg(dev,hwPort,15,30,&reg30_15)) != MAD_OK)
                {
                    MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
                    return (status | MAD_API_FAIL_READ_PAGED_REG);
                }

                if((status= madHwWritePagedPhyReg(dev,hwPort,12,30,0x4100)) != MAD_OK)
                {
                    MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                    return (status | MAD_API_FAIL_WRITE_PAGED_REG);
                }
                if((status = madHwSetPagedPhyRegField(
                    dev,hwPort,15,30,0,4,0x03)) != MAD_OK)
                {
                  MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                  return (status | MAD_API_FAIL_SET_PAGED_REG_FIELD);
                }



/*
                if((status= madHwReadPagedPhyReg(dev,hwPort,15,30,&reg30_15_tmp)) != MAD_OK)
                {
                    MAD_DBG_ERROR("Reading to paged phy reg failed.\n");
                    return (status | MAD_API_FAIL_READ_PAGED_REG);
                }
                MAD_DBG_INFO ("!@! after reg_30_15 %x\n", reg30_15_tmp); */



                status=mdSysSetPhyReg (dev, hwPort, 26, 0x8000);
                if (status != MAD_OK)
                {
                    MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                    return status;
                }

                /* 
                * loop until test completion
                */
                do
                {
                    status=mdSysGetPhyReg (dev, hwPort, 26, (MAD_U32 *)&reg26);
                    if (status != MAD_OK)
                    {
                        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
                        return status;
                    }
                } while((reg26 & 0x8000)&&(count--));

                    /*
                    * read the test result for the selected MDI Pair
                    */
                    status=mdSysGetPhyReg (dev, hwPort, 26+pairNo, (MAD_U32 *)&reg26);
                    if (status != MAD_OK)
                    {
                        return status;
                    }
                    /* MAD_DBG_INFO ("@@@ 3016 get reg_%i value: %x\n", 26+pairNo, reg26);  */
        
                   switch((reg26 & 0x6000) >> 13)
                   {
                     case 0:
                     /* Check Impedance Mismatch */
                     if ((reg26 & 0xFF) < 0xFF)
                     {
                        /*  if the reflected amplitude is low it is good cable too.
                            for this registers values it is a good cable:
                            0xE23, 0xE24, 0xE25, 0xE26, 0xE27 */
                        if ((reg26 < 0xE23) || (reg26 > 0xE27))
                        {
                            *cableStatus = MAD_IMPEDANCE_MISMATCH;
                            len = (int)MAD_VCT_3016_CALC(reg26 & 0xFF);
                            if(len <= 0)
                                cableLen->errCableLen = 0;
                            else
                                cableLen->errCableLen = (MAD_U8)len;
                            break;
                        }
                     }

                      /* test passed. No problem found. */
                      *cableStatus = MAD_NORMAL_CABLE;
                      cableLen->normCableLen = MAD_UNKNOWN_LEN;

                      break;

                     case 2:
                        /* test passed. Cable is open. */
                        *cableStatus = MAD_OPEN_CABLE;
                        len = (int)MAD_VCT_3016_CALC(reg26 & 0xFF);
                        if(len <= 0)
                            cableLen->errCableLen = 0;
                        else
                            cableLen->errCableLen = (MAD_U8)len;
                        break;
                     case 1:
                     case 3:
                     default:
                        /* test failed. No result is valid. */
                        *cableStatus = MAD_TEST_FAIL;
                        break;
                   } /* switch */
                 if((status= madHwWritePagedPhyReg(dev,hwPort,12,30,reg30_12)) != MAD_OK)
                 {
                    MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                    return (status | MAD_API_FAIL_WRITE_PAGED_REG);
                }
                if((status= madHwWritePagedPhyReg(dev,hwPort,15,30,reg30_15)) != MAD_OK)
                {
                            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                            return (status | MAD_API_FAIL_WRITE_PAGED_REG);
                 }

                break;   
            } /* else */
            break;

        case 3:
        default:
            /* test failed. No result is valid. */
            *cableStatus = MAD_TEST_FAIL;
            break;
    }

    return MAD_OK;
}

#endif

/*******************************************************************************
* getCableStatus_26
*
* DESCRIPTION:
*       This routine perform the virtual cable test for the 10/100Mbps phy,
*       and returns the the status per Rx/Tx pair.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*       cableStatus - the port copper cable status.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*

*******************************************************************************/
#ifdef MAD_SM_VCT
static 
MAD_STATUS getVctSt_26_check
(    
    IN  MAD_DEV         *dev,
    IN  MAD_U8          hwPort
)
{
    MAD_STATUS status;
    MAD_U16 reg26;
    int count=0x10;

    MAD_DBG_INFO(("getVctSt_26_check Called.\n"));

    /* 
     * loop until test completion
     */
    do
    {
        status=mdSysGetPhyReg (dev, hwPort, 26, (MAD_32 *)&reg26);
        if (status != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return status;
        }
    } while((reg26 & 0x8000)&&(count--));
    if (reg26 & 0x8000)
        return MAD_PENDING;
    return MAD_OK;
}


static 
MAD_STATUS getVctSt_26_set
(    
    IN  MAD_DEV         *dev,
    IN  MAD_U8          hwPort
)
{
    MAD_STATUS status;
/*  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg); */
    MAD_DBG_INFO(("getVctSt_26_set Called.\n"));
    /* 
     * start Virtual Cable Tester
     */
    status=mdSysSetPhyReg (dev, hwPort, 26, 0x8000);
    if (status != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return status;
    }


    return MAD_OK;
}

static 
MAD_STATUS getVctSt_26_get
(    
    IN  MAD_DEV         *dev,
    IN  MAD_U8          hwPort,
    OUT MAD_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS status;
    MAD_U16 reg26;
    int i;
    MAD_BOOL    run3016VctAgain=MAD_FALSE;
/*  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg); */
    MAD_DBG_INFO(("getVctSt_26_get Called.\n"));

    cableStatus->phyType = MAD_PHY_100M;

    for (i=0; i<(MAD_MDI_PAIR_NUM/2); i++)
    {
      /*
       * read the test result for the selected MDI Pair
       */
      status=mdSysGetPhyReg (dev, hwPort, 26+i, (MAD_U32 *)&reg26);
      if (status != MAD_OK)
      {
        return status;
      }
        
      /*
       * analyze the test result for RX Pair
       */
    /* if ((dev->deviceId==MAD_88E3016) || (dev->deviceId==MAD_88E3082))  */
    if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
      {
        int len;
        len = (int)MAD_VCT_3016_CALC(reg26 & 0xFF);

        if(((reg26 & 0x6000) == 0x4000) && (len < 0x20))
        {
            run3016VctAgain=MAD_TRUE;
            break;
        }
        else
        {
          if((status = analizeVCTResult_3016(dev, hwPort, reg26, i,
                                &cableStatus->cableStatus[i], 
                                &cableStatus->cableLen[i])) != MAD_OK)
          {
            MAD_DBG_ERROR("analizeVCTResult failed.\n");
            return status;
          }
        }
      }
      else
      {
          if((status = analizeVCTResult(dev, reg26, 
                                &cableStatus->cableStatus[i], 
                                &cableStatus->cableLen[i])) != MAD_OK)
          {
            MAD_DBG_ERROR("analizeVCTResult failed.\n");
            return status;
          }
      }
    }

    if (run3016VctAgain==MAD_TRUE)
    {
        return MAD_VCT_AGAIN;
    }

    for (i=2; i<(MAD_MDI_PAIR_NUM); i++)
    {
        cableStatus->cableStatus[i]=MAD_TEST_FAIL;
        cableStatus->cableLen[i].normCableLen = MAD_UNKNOWN_LEN;
        cableStatus->cableLen[i].errCableLen = 0;
    }

    return MAD_OK;
}


/***********************************************************************************
*  vct_3016 SM block 
************************************************************************************/


static 
MAD_STATUS vct_3016_init
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
  MAD_STATUS retVal;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
 
  /* if ((dev->deviceId==MAD_88E3016) || (dev->deviceId==MAD_88E3082))  */
  if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
  {
    retVal=mdSysGetPhyReg (dev, hwPort, 16, (MAD_U32 *)&madVct_3016_reg16);
    if (retVal != MAD_OK)
    {
        MAD_DBG_ERROR("Getting for (port %i) returned %x", hwPort, (int)retVal);
        return retVal;             
    }
    /* disable auto mdi */
    retVal=mdSysSetPhyReg (dev, hwPort, 16, 0xffdf&madVct_3016_reg16);
    if (retVal != MAD_OK)
    {
        MAD_DBG_ERROR("Setting for (port %i, register 16) returned ",(int)hwPort);
        return retVal;             
    }

    /* Forced 100M */
    retVal=mdSysGetPhyReg (dev, hwPort, 0, (MAD_U32 *)&madVct_3016_reg0);
    if (retVal != MAD_OK)
    {
       MAD_DBG_ERROR("Getting for (port %i) returned %x", hwPort, (int)retVal);
       return retVal;             
    }

    retVal=mdSysSetPhyReg (dev, hwPort, 0, 0xa100);
    if (retVal != MAD_OK)
    {
       MAD_DBG_ERROR("Setting for (port %i, MAD_PHY_100FDX) returned ",(int)hwPort);
        return retVal;             
    }

  }

  if((retVal=getVctSt_26_set(dev,hwPort)) != MAD_OK) 
  {
    MAD_DBG_ERROR("Running  VCT sub set failed.\n");
    return (retVal | MAD_API_FAIL_VCT_SM_SET);
  }


  return MAD_OK;
}

static 
MAD_STATUS vct_3016_set
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;
    if((retVal=getVctSt_26_set(dev,hwPort)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Running  VCT set failed.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_SET);
    }

    return MAD_OK;

}

static 
MAD_STATUS vct_3016_check
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;

  if(((retVal=getVctSt_26_check(dev,hwPort)) != MAD_OK) 
    &&((retVal &MAD_RUN_ST_MASK) != MAD_PENDING))
    {
        MAD_DBG_ERROR("Running  VCT check failed.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_CHECK);
    }

    return retVal;

}

static 
MAD_STATUS vct_3016_get
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
  MAD_CABLE_STATUS *cableStatus= (MAD_CABLE_STATUS *)(vctSmCfg->vctResultPtr);

  if(((retVal=getVctSt_26_get(dev,hwPort, cableStatus)) != MAD_OK) && 
      (retVal != MAD_VCT_AGAIN))
    {
        MAD_DBG_ERROR("Running advanced VCT SM get failed.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_GET);
    }
  return retVal;
}

static 
MAD_STATUS vct_3016_sub_set
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
  MAD_STATUS retVal;

  if((retVal=runVctAgain_3016_set(dev,hwPort)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Running  VCT sub set failed.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_SET);
    }


    return MAD_OK;

}

static 
MAD_STATUS vct_3016_sub_get
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
  MAD_CABLE_STATUS *cableStatus= (MAD_CABLE_STATUS *)(vctSmCfg->vctResultPtr);


  if((retVal=runVctAgain_3016_get(dev,hwPort, cableStatus)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Running  VCT sub get failed.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_GET);
    }


    return MAD_OK;
}

static 
MAD_STATUS vct_3016_close
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
  MAD_STATUS status;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);

    /* if ((dev->deviceId==MAD_88E3016) || (dev->deviceId==MAD_88E3082))  */
    if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
    {
        status=mdSysSetPhyReg (dev, hwPort, 16, madVct_3016_reg16);
        if (status != MAD_OK)
        {
            MAD_DBG_ERROR("Setting for (port %i, ) returned %x",(int)hwPort, status);
            return status;             
        }
        status=mdSysSetPhyReg (dev, hwPort, 0, madVct_3016_reg0 | 0x8000);
        if (status != MAD_OK)
        {
            MAD_DBG_ERROR("Setting for (port %i, ) returned %x",(int)hwPort, status);
            return status;             
        }

    }
    madVCTCloseSM (dev);
    return MAD_OK;
 
}

static    MAD_VCT_SM_ACTIONS  vct_3016_SmActions = {
                                        vct_3016_init,
                                        vct_3016_set,
                                        vct_3016_check,
                                        vct_3016_get,
                                        vct_3016_sub_set,
                                        vct_3016_sub_get,
                                        vct_3016_close
                                        };

MAD_STATUS getCableStatus_26
(    
    IN  MAD_DEV         *dev,
    IN  MAD_U8          hwPort,
    OUT MAD_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS retVal;

  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
    vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);

    if((retVal = madVCTFindSM(dev, hwPort, MAD_VCT_SM_VCT_TYPE_TDR,
                  &vct_3016_SmActions, MAD_TRUE, 
                  (MAD_VCT_DATA_STRUCT *)cableStatus, 1, 2)) != MAD_OK)
    {
        MAD_DBG_ERROR("Can not find VCT State machine instance .\n");
        return (retVal | MAD_API_FAIL_VCT_SM_INSTANCE);
    }

    if(((retVal = madVCTImplSM(dev, hwPort, MAD_VCT_SM_EVT_CMD)) != MAD_OK)&&
            ((retVal &MAD_RUN_ST_MASK) != MAD_PENDING))
    {
        MAD_DBG_ERROR("Can not implement VCT State machine instance .\n");
        return (retVal | MAD_API_FAIL_VCT_SM_IMPLEM);
    }
    return retVal;
}

#else

static 
MAD_STATUS getCableStatus_26
(    
    IN  MAD_DEV         *dev,
    IN  MAD_U8          hwPort,
    OUT MAD_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS status;
    MAD_U16 reg26, reg0, reg16;
    int i;
    int count=0x1000000;

    MAD_DBG_INFO(("getCableStatus_26 Called.\n"));


    /* if ((dev->deviceId==MAD_88E3016) || (dev->deviceId==MAD_88E3082)) For 30XX only*/
    if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
    {
        status=mdSysGetPhyReg (dev, hwPort, 16, (MAD_U32 *)&reg16);
        if (status != MAD_OK)
        {
            MAD_DBG_ERROR("Getting for (port %i) returned %x", hwPort, (int)status);
            return status;             
        }
        /* disable auto mdi */
        status=mdSysSetPhyReg (dev, hwPort, 16, 0xffdf&reg16);
        if (status != MAD_OK)
        {
            MAD_DBG_ERROR("Setting for (port %i, register 16) returned ",(int)hwPort);
            return status;             
        }

        /* Forced 100M */
        status=mdSysGetPhyReg (dev, hwPort, 0, (MAD_U32 *)&reg0);
        if (status != MAD_OK)
        {
            MAD_DBG_ERROR("Getting for (port %i) returned %x", hwPort, (int)status);
            return status;             
        }

        status=mdSysSetPhyReg (dev, hwPort, 0, 0xa100);
        if (status != MAD_OK)
        {
            MAD_DBG_ERROR("Setting for (port %i, MAD_PHY_100FDX) returned ",(int)hwPort);
            return status;             
        }

    }

    /* 
     * start Virtual Cable Tester
     */
    status=mdSysSetPhyReg (dev, hwPort, 26, 0x8000);
    if (status != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return status;
    }

    /* 
     * loop until test completion
     */
    do
    {
        status=mdSysGetPhyReg (dev, hwPort, 26, (MAD_32 *)&reg26);
        if (status != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return status;
        }
    } while((reg26 & 0x8000)&&(count--));

    cableStatus->phyType = MAD_PHY_100M;

    MAD_DBG_INFO("Reg26 after test : %0#x.\n", reg26);


    for (i=0; i<(MAD_MDI_PAIR_NUM/2); i++)
    {
        /*
         * read the test result for the selected MDI Pair
         */
        status=mdSysGetPhyReg (dev, hwPort, 26+i, (MAD_U32 *)&reg26);
        if (status != MAD_OK)
        {
            return status;
        }
        
        /*
         * analyze the test result for RX Pair
         */
        /* if ((dev->deviceId==MAD_88E3016) || (dev->deviceId==MAD_88E3082))  */
        if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
        {
          if((status = analizeVCTResult_3016_0(dev, hwPort, reg26, i,
                                &cableStatus->cableStatus[i], 
                                &cableStatus->cableLen[i])) != MAD_OK)
          {
            MAD_DBG_ERROR("analizeVCTResult failed.\n");
            return status;
          }
        }
        else
        {
          if((status = analizeVCTResult(dev, reg26, 
                                &cableStatus->cableStatus[i], 
                                &cableStatus->cableLen[i])) != MAD_OK)
          {
            MAD_DBG_ERROR("analizeVCTResult failed.\n");
            return status;
          }
        }
    }

    /* for 30xx there is only pair 0 and pair 1 */
    for (i=2; i<(MAD_MDI_PAIR_NUM); i++)
    {
        cableStatus->cableStatus[i]=MAD_TEST_FAIL;
        cableStatus->cableLen[i].normCableLen = MAD_UNKNOWN_LEN;
        cableStatus->cableLen[i].errCableLen = 0;
    }

    /* if ((dev->deviceId==MAD_88E3016) || (dev->deviceId==MAD_88E3082))  */
    if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
    {
        status=mdSysSetPhyReg (dev, hwPort, 16, reg16);
        if (status != MAD_OK)
        {
            MAD_DBG_ERROR("Setting for (port %i, ) returned %x",(int)hwPort, status);
            return status;             
        }
        status=mdSysSetPhyReg (dev, hwPort, 0, reg0);
        if (status != MAD_OK)
        {
            MAD_DBG_ERROR("Setting for (port %i, ) returned %x",(int)hwPort, status);
            return status;             
        }

    }

    return MAD_OK;
}


#endif

/**************************************************************************
*
***************************************************************************/


static
MAD_STATUS  enable1stWorkAround_28_0
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort
)
{
    MAD_STATUS   status;

    /* enable 1st work-around */
    if ((status = madHwWritePagedPhyReg(dev, hwPort, 0x18, 30, 0x00c2)) != MAD_OK)
      return (status | MAD_API_FAIL_WRITE_PAGED_REG);

    if ((status = madHwWritePagedPhyReg(dev, hwPort, 0x18, 30, 0x00ca)) != MAD_OK)
      return (status | MAD_API_FAIL_WRITE_PAGED_REG);

    if ((status = madHwWritePagedPhyReg(dev, hwPort, 0x18, 30, 0x00c2)) != MAD_OK)
      return (status | MAD_API_FAIL_WRITE_PAGED_REG);

    return MAD_OK;
}

static
MAD_STATUS  disable1stWorkAround_28_0
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort
)
{
    MAD_STATUS status;

    /* disable 1st work-around */
    if ((status = madHwWritePagedPhyReg(dev, hwPort, 0x18, 30, 0x0042)) != MAD_OK)
      return (status | MAD_API_FAIL_WRITE_PAGED_REG);

    return MAD_OK;
}



/*******************************************************************************
* getCableStatus_28_0
*
* DESCRIPTION:
*       This routine perform the virtual cable test for the 10/100Mbps phy,
*       and returns the the status per Rx/Tx pair.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*       cableStatus - the port copper cable status.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
static 
MAD_STATUS getCableStatus_28_0_set
(    
    IN  MAD_DEV         *dev,
    IN  MAD_U8          hwPort
)
{
    MAD_STATUS status;

    MAD_DBG_INFO(("getCableStatus_28_0_set Called.\n"));

    /* 
     * start Virtual Cable Tester
     */
    if((status= madHwWritePagedPhyReg(dev,hwPort,0,28,0x8000)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (status | MAD_API_FAIL_WRITE_PAGED_REG);
    }


    return MAD_OK;
}

static 
MAD_STATUS getCableStatus_28_0_check
(    
    IN  MAD_DEV         *dev,
    IN  MAD_U8          hwPort
)
{
    MAD_STATUS status;
    MAD_U16 reg28;
    int count=0x10;

    MAD_DBG_INFO(("getCableStatus_28_0_check Called.\n"));

    /* 
     * loop until test completion
     */
    do
    {
        if((status= madHwReadPagedPhyReg(dev,hwPort,0,28,&reg28)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (status | MAD_API_FAIL_READ_PAGED_REG);
        }
        
    } while((reg28 & 0x8000)&&(count--));

    if (reg28 & 0x8000)
        return MAD_PENDING;


    return MAD_OK;
}
static 
MAD_STATUS getCableStatus_28_0_get
(    
    IN  MAD_DEV         *dev,
    IN  MAD_U8          hwPort,
    OUT MAD_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS status;
    MAD_U16 reg28;
    int i;

    MAD_DBG_INFO(("getCableStatus_28_0_get Called.\n"));


    cableStatus->phyType = MAD_PHY_1000M;


    for (i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        /*
         * read the test result for the selected MDI Pair
         */
        if((status= madHwReadPagedPhyReg(dev,hwPort,(MAD_U16)i,28,&reg28)) != MAD_OK)
        {
            return (status | MAD_API_FAIL_READ_PAGED_REG);
        }
        
        /*
         * analyze the test result for RX Pair
         */
        if((status = analizeVCTResult(dev, reg28, 
                                &cableStatus->cableStatus[i], 
                                &cableStatus->cableLen[i])) != MAD_OK)
        {
            MAD_DBG_ERROR("analizeVCTResult failed.\n");
            return status;
        }
    }

    MAD_DBG_INFO("Reg28 after test : %0#x.\n", reg28);

    return MAD_OK;
}
static 
MAD_STATUS getCableStatus_28_0
(    
    IN  MAD_DEV         *dev,
    IN  MAD_U8          hwPort,
    OUT MAD_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS retVal;
    if((retVal=getCableStatus_28_0_set(dev,hwPort)) != MAD_OK)
    {
       MAD_DBG_ERROR("Running VCT SM set failed.\n");
       return (retVal | MAD_API_FAIL_VCT_SM_SET);
    }
    if((retVal=getCableStatus_28_0_check(dev,hwPort)) != MAD_OK)
    {
       MAD_DBG_ERROR("Running VCT SM check failed.\n");
       return (retVal | MAD_API_FAIL_VCT_SM_CHECK);
    }
    if((retVal=getCableStatus_28_0_get(dev,hwPort, cableStatus)) != MAD_OK)
    {
       MAD_DBG_ERROR("Running VCT SM get failed.\n");
       return (retVal | MAD_API_FAIL_VCT_SM_GET);
    }



    return MAD_OK;
}

/*******************************************************************************
* getCableStatus_28_0_E
*
* DESCRIPTION:
*       This routine perform the virtual cable test for the 10/100Mbps phy,
*       and returns the the status per Rx/Tx pair.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*       cableStatus - the port copper cable status.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
#ifdef MAD_SM_VCT

/***********************************************************************************
*  vct_1111 SM block 
************************************************************************************/


static 
MAD_STATUS vct_1111_init
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
  MAD_STATUS retVal;
    if((retVal=enable1stWorkAround_28_0(dev,hwPort)) != MAD_OK)
    {
        MAD_DBG_ERROR("Enabling work around failed.\n");
        return (retVal| MAD_API_FAIL_VCT_INIT);
    }
  if((retVal=getCableStatus_28_0_set(dev,hwPort)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Running  VCT set failed.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_SET);
    }


    return MAD_OK;


}

static 
MAD_STATUS vct_1111_set
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;

  if((retVal=getCableStatus_28_0_set(dev,hwPort)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Running  VCT set failed.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_SET);
    }


    return MAD_OK;

}

static 
MAD_STATUS vct_1111_check
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;

  if(((retVal=getCableStatus_28_0_check(dev,hwPort)) != MAD_OK) 
    &&((retVal &MAD_RUN_ST_MASK) != MAD_PENDING))
    {
        MAD_DBG_ERROR("Running  VCT check failed.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_CHECK);
    }


    return retVal;

}

static 
MAD_STATUS vct_1111_get
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
  MAD_CABLE_STATUS *cableStatus= (MAD_CABLE_STATUS *)(vctSmCfg->vctResultPtr);

  if(((retVal=getCableStatus_28_0_get(dev,hwPort, cableStatus)) != MAD_OK) && 
      (retVal != MAD_VCT_AGAIN))
    {
        MAD_DBG_ERROR("Running VCT SM get failed.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_GET);
    }
  return retVal;
}


static 
MAD_STATUS vct_1111_close
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;
    if((retVal=disable1stWorkAround_28_0(dev,hwPort)) != MAD_OK)
    {
        MAD_DBG_ERROR("Disabling work around failed.\n");
        return (retVal | MAD_API_FAIL_VCT_CLOSE);
    }
    madVCTCloseSM (dev);
    return MAD_OK;
 
}

static    MAD_VCT_SM_ACTIONS  vct_1111_SmActions = {
                                        vct_1111_init,
                                        vct_1111_set,
                                        vct_1111_check,
                                        vct_1111_get,
                                        vct_1111_set,
                                        vct_1111_get,
                                        vct_1111_close
                                        };

static 
MAD_STATUS getCableStatus_28_0_E
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    OUT MAD_CABLE_STATUS *cableStatus
)
{
     MAD_STATUS retVal;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
    vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);


    if((retVal = madVCTFindSM(dev, hwPort, MAD_VCT_SM_VCT_TYPE_TDR,
                  &vct_1111_SmActions, MAD_TRUE, 
                  (MAD_VCT_DATA_STRUCT *)cableStatus, 1, 1)) != MAD_OK)
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


#endif

/*******************************************************************************
* getCableStatus_16_5
*
* DESCRIPTION:
*       This routine performs the virtual cable test for the PHY using
*       register 16_5 and returns the the status per MDIP/N.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*       cableStatus - the port copper cable status.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
#ifdef MAD_SM_VCT

static 
MAD_STATUS getCableStatus_16_5_set
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;

    MAD_DBG_INFO("getCableStatus_16_5_set Called.\n");

    /* 
     * start Virtual Cable Tester
     */
    if((retVal = madHwWritePagedPhyReg(
                    dev,hwPort,5,MAD_REG_MDI0_VCT_STATUS,0x8000)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    return MAD_OK;
}

static 
MAD_STATUS getCableStatus_16_5_check
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;
    MAD_U16 u16Data;
    int count=0x10;

    MAD_DBG_INFO("getCableStatus_16_5_check Called.\n");

    /* 
     * loop until test completion
     */
    do
    {
        if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,5,MAD_REG_MDI0_VCT_STATUS,&u16Data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }
    } while((u16Data & 0x8000)&&(count--));

    if (u16Data & 0x8000)
        return MAD_PENDING;

    return MAD_OK;
}

static 
MAD_STATUS getCableStatus_16_5_get
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    OUT MAD_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS retVal;
    MAD_U16 u16Data;
    int i;

    MAD_DBG_INFO("getCableStatus_16_5_get Called.\n");

   cableStatus->phyType = MAD_PHY_1000M;

    MAD_DBG_INFO("Page 5 of Reg16 after test : %0#x.\n", u16Data);

    for (i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        /*
         * read the test result for the selected MDI Pair
         */
        if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,5,MAD_REG_MDI0_VCT_STATUS+i,&u16Data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        /*
         * analyze the test result for RX Pair
         */
        if((retVal = analizeVCTResult(dev, u16Data, 
                                &cableStatus->cableStatus[i], 
                                &cableStatus->cableLen[i])) != MAD_OK)
        {
            MAD_DBG_ERROR("analizeVCTResult failed.\n");
            return retVal;
        }

    }

    return MAD_OK;
}




/***********************************************************************************
*  vct_1112 SM block 
************************************************************************************/


static 
MAD_STATUS vct_1112_init
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
  MAD_STATUS retVal;
  if((retVal=getCableStatus_16_5_set(dev,hwPort)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Running  VCT SM set failed.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_SET);
    }


    return MAD_OK;


}

static 
MAD_STATUS vct_1112_set
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;

  if((retVal=getCableStatus_16_5_set(dev,hwPort)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Running  VCT SM set failed.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_SET);
    }


    return MAD_OK;

}

static 
MAD_STATUS vct_1112_check
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;

  if(((retVal=getCableStatus_16_5_check(dev,hwPort)) != MAD_OK)&&
            ((retVal&MAD_RUN_ST_MASK) != MAD_PENDING)) 
    {
        MAD_DBG_ERROR("Running  VCT SM check failed.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_CHECK);
    }


    return retVal;

}

static 
MAD_STATUS vct_1112_get
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
  MAD_CABLE_STATUS *cableStatus= (MAD_CABLE_STATUS *)(vctSmCfg->vctResultPtr);

  if(((retVal=getCableStatus_16_5_get(dev,hwPort, cableStatus)) != MAD_OK) && 
      (retVal != MAD_VCT_AGAIN))
    {
        MAD_DBG_ERROR("Running VCT SM get failed.\n");
        return (retVal | MAD_API_FAIL_VCT_SM_GET);
    }
  return retVal;
}


static 
MAD_STATUS vct_1112_close
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    madVCTCloseSM (dev);
    return MAD_OK;
 
}

static    MAD_VCT_SM_ACTIONS  vct_1112_SmActions = {
                                        vct_1112_init,
                                        vct_1112_set,
                                        vct_1112_check,
                                        vct_1112_get,
                                        vct_1112_set,
                                        vct_1112_get,
                                        vct_1112_close
                                        };

static 
MAD_STATUS getCableStatus_16_5
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    OUT MAD_CABLE_STATUS *cableStatus
)
{
     MAD_STATUS retVal;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
    vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);


    if((retVal = madVCTFindSM(dev, hwPort, MAD_VCT_SM_VCT_TYPE_TDR,
                  &vct_1112_SmActions, MAD_TRUE, 
                  (MAD_VCT_DATA_STRUCT *)cableStatus, 1, 1)) != MAD_OK)
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
MAD_STATUS getCableStatus_16_5
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    OUT MAD_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS retVal;
    MAD_U16 u16Data;
    int i;

    MAD_DBG_INFO("getCableStatus Called.\n");

    /* 
     * start Virtual Cable Tester
     */
    if((retVal = madHwWritePagedPhyReg(
                    dev,hwPort,5,MAD_REG_MDI0_VCT_STATUS,0x8000)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
    /* 
     * loop until test completion
     */
    do
    {
        if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,5,MAD_REG_MDI0_VCT_STATUS,&u16Data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }
    } while(u16Data & 0x8000);

    cableStatus->phyType = MAD_PHY_1000M;

    MAD_DBG_INFO("Page 5 of Reg16 after test : %0#x.\n", u16Data);

    for (i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        /*
         * read the test result for the selected MDI Pair
         */
        if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,5,(MAD_U8)(MAD_REG_MDI0_VCT_STATUS+i),&u16Data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }

        /*
         * analyze the test result for RX Pair
         */
        if((retVal = analizeVCTResult(dev, u16Data, 
                                &cableStatus->cableStatus[i], 
                                &cableStatus->cableLen[i])) != MAD_OK)
        {
            MAD_DBG_ERROR("analizeVCTResult failed.\n");
            return retVal;
        }

    }

    return MAD_OK;
}



#endif
/*******************************************************************************
* mdDiagGetCableStatus
*
* DESCRIPTION:
*       This routine performs the virtual cable test for the requested port,
*       and returns the status per MDI pair.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*
* OUTPUTS:
*       cableStatus - test status and cable length
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15    : Soft reset
*       Page 5, Reg 21, Bit 6    : DSP Valid
*       Page 5, Reg 16, Bit 15:0 : MDI[0] VCT Register
*       Page 5, Reg 17, Bit 15:0 : MDI[1] VCT Register
*       Page 5, Reg 18, Bit 15:0 : MDI[2] VCT Register
*       Page 5, Reg 19, Bit 15:0 : MDI[3] VCT Register
*
*******************************************************************************/
MAD_STATUS mdDiagGetCableStatus
(
    IN  MAD_DEV*        dev,
    IN  MAD_LPORT       port,
    OUT MAD_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS  retVal;
    MAD_U8      hwPort;
/*    MAD_BOOL    onOff; */

    MAD_DBG_INFO("madDiagGetCableStatus Called.\n");
    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY(dev, MAD_PHY_VCT_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("TDR VCT not supported.\n");
        return MAD_API_TDRVCT_NOT_SUPPORT;
    }

    switch(dev->phyInfo.vctType)
    {
        case MAD_PHY_VCT_TYPE2:
#ifdef MAD_SM_VCT
            if(((retVal=getCableStatus_28_0_E(dev,hwPort,cableStatus)) != MAD_OK)
                &&((retVal &MAD_RUN_ST_MASK) != MAD_PENDING))
            {
                MAD_DBG_ERROR("Running VCT failed.\n");
                break;
            }
#else
            if((retVal=enable1stWorkAround_28_0(dev,hwPort)) != MAD_OK)
            {
                MAD_DBG_ERROR("Enabling work around failed.\n");
                break;
            }
            if((retVal=getCableStatus_28_0(dev,hwPort,cableStatus)) != MAD_OK)
            {
                MAD_DBG_ERROR("Running VCT failed.\n");
                break;
            }
            if((retVal=disable1stWorkAround_28_0(dev,hwPort)) != MAD_OK)
            {
                MAD_DBG_ERROR("Disabling work around failed.\n");
                break;
            }

#endif
            break;

        case MAD_PHY_VCT_TYPE4:
            if(((retVal=getCableStatus_16_5(dev,hwPort,cableStatus)) != MAD_OK)
                &&((retVal &MAD_RUN_ST_MASK) != MAD_PENDING))
            {
                MAD_DBG_ERROR("Running VCT failed.\n");
                break;
            }
            break;

        case MAD_PHY_VCT_TYPE3:
            if(((retVal=getCableStatus_28_0(dev,hwPort,cableStatus)) != MAD_OK)
                &&((retVal &MAD_RUN_ST_MASK) != MAD_PENDING))
            {
                MAD_DBG_ERROR("Running VCT failed.\n");
                break;
            }
             break;

       case MAD_PHY_VCT_TYPE1:
             /* if ((dev->deviceId==MAD_88E3016)) */
             if(MAD_DEV_CAPABILITY(dev, MAD_PHY_30XX_MODE))
             {
               MAD_U16 id2;
               retVal = madHwReadPagedPhyReg(dev, hwPort,0,MAD_REG_PHY_ID_2,&id2);
               if (retVal != MAD_OK)
               {
                    MAD_DBG_ERROR("Mad device Read Failed\n");
                    return (retVal | MAD_API_FAIL_READ_PAGED_REG);
               }
               if (id2==0x0e21)
                {
                    MAD_DBG_ERROR("Do not support VCT function\n");
                    return MAD_API_TDRVCT_NOT_SUPPORT;
               }
             }
            if(((retVal=getCableStatus_26(dev,hwPort,cableStatus)) != MAD_OK)
                &&((retVal &MAD_RUN_ST_MASK) != MAD_PENDING))
            {
                MAD_DBG_ERROR("Running VCT failed.\n");
                break;
            }
             break;

        default:
            return MAD_API_TDRVCT_NOT_SUPPORT;
    }
    if ((retVal&MAD_RUN_ST_MASK)==MAD_PENDING)
    {
         return retVal;
    }

    /*
     * reset the phy
     */
    if((madHwPagedReset(dev,hwPort,0)) != MAD_OK)
    {
        MAD_DBG_ERROR("Soft Reset failed.\n");
        return (MAD_API_FAIL_SW_RESET);
    }

    return retVal;
}



#ifdef MAD_SM_VCT

/*******************************************************************************
* madVCTDataInit*
* DESCRIPTION:
*       Initialize VCT data and VCT state machine.
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*        
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
void madVCTDataInit(MAD_DEV*        dev)
{
        vctSmData.smSt=MAD_SM_FREE;
        dev->SMDataPtr = &vctSmData;
        madVctSmInit(&(vctSmData.smCfg));
}

#endif
