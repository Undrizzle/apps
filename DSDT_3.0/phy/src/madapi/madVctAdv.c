#include <madCopyright.h>

/********************************************************************************
* madVctAdv.c
* 
* DESCRIPTION:
*       APIs for VCT (Advaced TDR).
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

#define MAD_ADV_VCT_ACCEPTABLE_SHORT_CABLE  11


/*******************************************************************************
* getDetailedAdvVCTResult
*
* DESCRIPTION:
*       This routine differenciate Open/Short from Impedance mismatch.
*
* INPUTS:
*       amp - amplitude
*       len - distance to fault
*       vctResult - test result 
*                   (Impedance mismatch, either > 115 ohms, or < 85 ohms)
*
* OUTPUTS:
*
* RETURNS:
*       MAD_ADV_VCT_STATUS
*
* COMMENTS:
*       This routine assumes test result is not normal nor cross pair short.
*
*******************************************************************************/
static
MAD_ADV_VCT_STATUS getDetailedAdvVCTResult
(
    IN  MAD_DEV  *dev,
    IN  MAD_U32  amp,
    IN  MAD_U32  len,
    IN  MAD_ADV_VCT_STATUS result
)
{
    MAD_ADV_VCT_STATUS vctResult;
    MAD_BOOL    update = MAD_FALSE;

    MAD_DBG_INFO("getDetailedAdvVCTResult Called.\n");
  if (dev->deviceId==MAD_88E1116)
  {
    if(len < 10)
    {
        if(amp > 54)  /* 90 x 0.6 */
            update = MAD_TRUE;
    }
    else if(len < 50)
    {
        if(amp > 42) /* 70 x 0.6 */
            update = MAD_TRUE;
    }
    else if(len < 110)
    {
        if(amp > 30)  /* 50 x 0.6 */
            update = MAD_TRUE;
    }
    else if(len < 140)
    {
        if(amp > 24)  /* 40 x 0.6 */
            update = MAD_TRUE;
    }
    else
    {
        if(amp > 18) /* 30 x 0.6 */
            update = MAD_TRUE;
    }
  }
  else
  {
    if(len < 10)
    {
        if(amp > 90)  
            update = MAD_TRUE;
    }
    else if(len < 50)
    {
        if(amp > 70) 
            update = MAD_TRUE;
    }
    else if(len < 110)
    {
        if(amp > 50)  
            update = MAD_TRUE;
    }
    else if(len < 140)
    {
        if(amp > 40)  
            update = MAD_TRUE;
    }
    else
    {
        if(amp > 30) 
            update = MAD_TRUE;
    }
  }



    switch (result)
    {
        case MAD_ADV_VCT_IMP_GREATER_THAN_115:
                if(update)
                    vctResult = MAD_ADV_VCT_OPEN;
                else
                    vctResult = result;
                break;
        case MAD_ADV_VCT_IMP_LESS_THAN_85:
                if(update)
                    vctResult = MAD_ADV_VCT_SHORT;
                else
                    vctResult = result;
                break;
        default:
                vctResult = result;
                break;
    }

    return vctResult;
}

/*******************************************************************************
* analizeAdvVCTResult
*
* DESCRIPTION:
*       This routine analize the Advanced VCT result.
*
* INPUTS:
*       channel - channel number where test was run
*       crossChannelReg - register values after the test is completed
*       mode    - use formula for normal cable case
*
* OUTPUTS:
*       cableStatus - analized test result.
*
* RETURNS:
*       -1, or distance to fault
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static
MAD_16 analizeAdvVCTNoCrosspairResult
(
    IN  MAD_DEV *dev,
    IN  int     channel, 
    IN  MAD_U16 *crossChannelReg, 
    IN  MAD_BOOL isShort,
    OUT MAD_ADV_CABLE_STATUS *cableStatus
)
{
    int len;
    MAD_16 dist2fault;
    MAD_ADV_VCT_STATUS vctResult = MAD_ADV_VCT_NORMAL;

    MAD_DBG_INFO("analizeAdvVCTNoCrosspairResult Called.\n");
    MAD_DBG_INFO("analizeAdvVCTNoCrosspairResult chan %d reg data %x\n", channel, crossChannelReg[channel]);

    dist2fault = -1;

    /* check if test is failed */
    if(IS_VCT_FAILED(crossChannelReg[channel]))
    {
       cableStatus->cableStatus[channel] = MAD_ADV_VCT_FAIL;
       return dist2fault;
    }

    /* Check if fault detected */
    if(IS_ZERO_AMPLITUDE(crossChannelReg[channel]))
    {
       cableStatus->cableStatus[channel] = MAD_ADV_VCT_NORMAL;
       return dist2fault;
    }

    /* find out test result by reading Amplitude */
    if(IS_POSITIVE_AMPLITUDE(crossChannelReg[channel]))
    {
       vctResult = MAD_ADV_VCT_IMP_GREATER_THAN_115;
    }
    else
    {
       vctResult = MAD_ADV_VCT_IMP_LESS_THAN_85;
    }

    /* 
     * now, calculate the distance for MAD_ADV_VCT_IMP_GREATER_THAN_115 and
     * MAD_ADV_VCT_IMP_LESS_THAN_85
     */
    switch (vctResult)
    {
        case MAD_ADV_VCT_IMP_GREATER_THAN_115:
        case MAD_ADV_VCT_IMP_LESS_THAN_85:
            if(!isShort)
            {
                len = (int)MAD_ADV_VCT_CALC(crossChannelReg[channel] & 0xFF);
            }
            else
            {
                len = (int)MAD_ADV_VCT_CALC_SHORT(crossChannelReg[channel] & 0xFF);
            }
            MAD_DBG_INFO("@@@@ no cross len %d\n", len);
            if (len < 0)
                len = 0;
            cableStatus->u[channel].dist2fault = (MAD_16)len;
            vctResult = getDetailedAdvVCTResult(
                            dev,
                            GET_AMPLITUDE(crossChannelReg[channel]),
                            len,
                            vctResult);
            dist2fault = (MAD_16)len;
            break;
        default:
            break;
    }

    cableStatus->cableStatus[channel] = vctResult;

    return dist2fault;
}



static
MAD_16 analizeAdvVCTResult
(
    IN  MAD_DEV *dev,
    IN  int     channel, 
    IN  MAD_U16 *crossChannelReg, 
    IN  MAD_BOOL isShort,
    OUT MAD_ADV_CABLE_STATUS *cableStatus
)
{
    int i, len;
    MAD_16 dist2fault;
    MAD_ADV_VCT_STATUS vctResult = MAD_ADV_VCT_NORMAL;

    MAD_DBG_INFO("analizeAdvVCTResult(Crosspair) chan %d reg data %x\n", channel, crossChannelReg[channel]);
    MAD_DBG_INFO("analizeAdvVCTResult Called.\n");

    dist2fault = -1;

    /* check if test is failed */
    for (i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        if(IS_VCT_FAILED(crossChannelReg[i]))
        {
            cableStatus->cableStatus[channel] = MAD_ADV_VCT_FAIL;
            return dist2fault;
        }
    }

    /* find out test result by reading Amplitude */
    for (i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        if (i == channel)
        {
            if(!IS_ZERO_AMPLITUDE(crossChannelReg[i]))
            {
                if(IS_POSITIVE_AMPLITUDE(crossChannelReg[i]))
                {
                    vctResult = MAD_ADV_VCT_IMP_GREATER_THAN_115;
                }
                else
                {
                    vctResult = MAD_ADV_VCT_IMP_LESS_THAN_85;
                }
            }
            continue;
        }

        if(IS_ZERO_AMPLITUDE(crossChannelReg[i]))
            continue;

        vctResult = MAD_ADV_VCT_CROSS_PAIR_SHORT;
        break;
    }

    /* if it is cross pair short, check the distance for each channel */
    if(vctResult == MAD_ADV_VCT_CROSS_PAIR_SHORT)
    {
        cableStatus->cableStatus[channel] = MAD_ADV_VCT_CROSS_PAIR_SHORT;
        for (i=0; i<MAD_MDI_PAIR_NUM; i++)
        {
            if(IS_ZERO_AMPLITUDE(crossChannelReg[i]))
            {
                cableStatus->u[channel].crossShort.channel[i] = MAD_FALSE;
                cableStatus->u[channel].crossShort.dist2fault[i] = 0;
                continue;
            }
                        
            cableStatus->u[channel].crossShort.channel[i] = MAD_TRUE;
            if(!isShort)
                len = (int)MAD_ADV_VCT_CALC(crossChannelReg[i] & 0xFF);
            else
                len = (int)MAD_ADV_VCT_CALC_SHORT(crossChannelReg[i] & 0xFF);
            MAD_DBG_INFO("@@@@ len %d\n", len);
            if (len < 0)
                len = 0;
            cableStatus->u[channel].crossShort.dist2fault[i] = (MAD_16)len;
            dist2fault = (MAD_16)len;
        }

        return dist2fault;
    }

    /* 
     * now, calculate the distance for MAD_ADV_VCT_IMP_GREATER_THAN_115 and
     * MAD_ADV_VCT_IMP_LESS_THAN_85
     */
    switch (vctResult)
    {
        case MAD_ADV_VCT_IMP_GREATER_THAN_115:
        case MAD_ADV_VCT_IMP_LESS_THAN_85:
            if(isShort)
                len = (int)MAD_ADV_VCT_CALC(crossChannelReg[channel] & 0xFF);
            else
                len = (int)MAD_ADV_VCT_CALC_SHORT(crossChannelReg[channel] & 0xFF);
            if (len < 0)
                len = 0;
            cableStatus->u[channel].dist2fault = (MAD_16)len;
            vctResult = getDetailedAdvVCTResult(
                            dev,
                            GET_AMPLITUDE(crossChannelReg[channel]),
                            len,
                            vctResult);
            dist2fault = (MAD_16)len;
            break;
        default:
            break;
    }

    cableStatus->cableStatus[channel] = vctResult;

    return dist2fault;
}


/*******************************************************************************
* runAdvCableTest_1181
*
* DESCRIPTION:
*       This routine performs the advanced virtual cable test for the PHY with
*       multiple page mode and returns the the status per MDIP/N.
*
* INPUTS:
*       port - logical port number.
*       mode - MAD_TRUE, if short cable detect is required
*              MAD_FALSE, otherwise
*
* OUTPUTS:
*       cableStatus - the port copper cable status.
*       tooShort    - if known distance to fault is too short
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
MAD_STATUS runAdvCableTest_1181
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_BOOL         mode,
    OUT MAD_ADV_CABLE_STATUS *cableStatus,
    OUT MAD_BOOL         *tooShort
)
{
    MAD_STATUS retVal;
    MAD_U16 u16Data;
    MAD_U16 crossChannelReg[MAD_MDI_PAIR_NUM];
    int i,j;
    MAD_16  dist2fault;

    VCT_REGISTER regList[MAD_MDI_PAIR_NUM][MAD_MDI_PAIR_NUM] = {
                            {{8,16},{8,17},{8,18},{8,19}},  /* channel 0 */
                            {{8,24},{8,25},{8,26},{8,27}},  /* channel 1 */
                            {{9,16},{9,17},{9,18},{9,19}},  /* channel 2 */
                            {{9,24},{9,25},{9,26},{9,27}}   /* channel 3 */
                            };

    MAD_DBG_INFO("runAdvCableTest_1181 Called.\n");

    if (mode)
        *tooShort = MAD_FALSE;

    /* 
     * start Advanced Virtual Cable Tester
     */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,8,MAD_REG_ADV_VCT_CONTROL_8,15,1,1)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    /* 
     * loop until test completion and result is valid
     */
    do
    {
        if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,8,MAD_REG_ADV_VCT_CONTROL_8,&u16Data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
        }
    } while(u16Data & 0x8000);

    MAD_DBG_INFO("Page 8 of Reg20 after test : %0#x.\n", u16Data);

    for (i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        /*
         * read the test result for the cross pair against selected MDI Pair
         */
        for (j=0; j<MAD_MDI_PAIR_NUM; j++)
        {
            if((retVal = madHwReadPagedPhyReg(
                            dev,hwPort,
                            regList[i][j].page,
                            regList[i][j].regOffset,
                            &crossChannelReg[j])) != MAD_OK)
            {
                MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_READ_PAGED_REG);
            }
        }

        /*
         * analyze the test result for RX Pair
         */
        dist2fault = analizeAdvVCTResult(dev, i, crossChannelReg, mode, cableStatus);

        if(mode)
        {
            if ((dist2fault>=0) && (dist2fault<MAD_ADV_VCT_ACCEPTABLE_SHORT_CABLE))
            {
                MAD_DBG_INFO("Distance to Fault is too Short. So, rerun after changing pulse width\n");
                *tooShort = MAD_TRUE;
                break;
            }
        }
    }

    return MAD_OK;
}

/************************************************************************************

*************************************************************************************/


static 
MAD_STATUS runAdvCableTest_1116_set
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_32           channel,
    IN  MAD_ADV_VCT_TRANS_CHAN_SEL        crosspair
)
{
    MAD_STATUS retVal;


    MAD_DBG_INFO("runAdvCableTest_1116_set Called.\n");
#if 0
    /* Set transmit channel */
   if (crosspair!=MAD_ADV_VCT_TCS_NO_CROSSPAIR)
   {
    if((retVal = madHwSetPagedPhyRegField(
        dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,11,3,0x4+channel)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
    /* 
     * start Advanced Virtual Cable Tester
     */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,15,1,1)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
   }
   else
   {
    if (channel==0)
    {
      if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,11,3,0x000)) != MAD_OK)
      {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
      /* 
      * start Advanced Virtual Cable Tester
      */
      if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,15,1,1)) != MAD_OK)
      {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
    }
   }
#else
      /* 
      * start Advanced Virtual Cable Tester
      */
      if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,15,1,1)) != MAD_OK)
      {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }

#endif

    return MAD_OK;
}

static 
MAD_STATUS runAdvCableTest_1116_check
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
    if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,&u16Data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }
    if (u16Data & 0x8000)
        return MAD_PENDING;

    MAD_DBG_INFO("Page 5 of Reg23 after test : %0#x.\n", u16Data);

    return MAD_OK;
}

static 
MAD_STATUS runAdvCableTest_1116_get
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_ADV_VCT_TRANS_CHAN_SEL    crosspair,
     IN  MAD_32            channel,
    OUT MAD_ADV_CABLE_STATUS *cableStatus,
    OUT MAD_BOOL         *tooShort
)
{
    MAD_STATUS retVal;
    MAD_U16 u16Data;
    MAD_U16 crossChannelReg[MAD_MDI_PAIR_NUM];
    int j;
    MAD_16  dist2fault;
    MAD_BOOL         mode;
    MAD_BOOL         localTooShort[MAD_MDI_PAIR_NUM];

    VCT_REGISTER regList[MAD_MDI_PAIR_NUM] = {
                    {5,16},{5,17},{5,18},{5,19} };
    mode = (dev->flags & MAD_FLAG_SHORT_CABLE_FIX)?MAD_TRUE:MAD_FALSE;

/*    if (mode)
        *tooShort = MAD_FALSE;
*/
    MAD_DBG_INFO("runAdvCableTest_1116_get Called.\n");


    if ((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,&u16Data)) != MAD_OK)
    {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    MAD_DBG_INFO("Page 5 of Reg23 after test : %0#x.\n", u16Data);

    /*
     * read the test result for the cross pair against selected MDI Pair
     */
    for (j=0; j<MAD_MDI_PAIR_NUM; j++)
    {
      if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,
                        regList[j].page,
                        regList[j].regOffset,
                        &crossChannelReg[j])) != MAD_OK)
      {
         MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
         return (retVal | MAD_API_FAIL_READ_PAGED_REG);
      }
      MAD_DBG_INFO("@@@@@ reg channel %d is %x \n", j, crossChannelReg[j]);
    }

    /*
     * analyze the test result for RX Pair
     */
    for (j=0; j<MAD_MDI_PAIR_NUM; j++)
    {
      if (crosspair!=MAD_ADV_VCT_TCS_NO_CROSSPAIR)
        dist2fault = analizeAdvVCTResult(dev, j, crossChannelReg, mode&(*tooShort), cableStatus);
      else
        dist2fault = analizeAdvVCTNoCrosspairResult(dev, j, crossChannelReg, mode&(*tooShort), cableStatus);

      localTooShort[j]=MAD_FALSE;
      if((mode)&&(*tooShort==MAD_FALSE))
      {
        if ((dist2fault>=0) && (dist2fault<MAD_ADV_VCT_ACCEPTABLE_SHORT_CABLE))
        {
           MAD_DBG_INFO("@@@#@@@@ it is too short dist2fault %d\n", dist2fault);
           MAD_DBG_INFO("Distance to Fault is too Short. So, rerun after changing pulse width\n");
           localTooShort[j]=MAD_TRUE;
/*           *tooShort = MAD_TRUE;
           return MAD_OK;   Should return to stop for loop */
        }
      }
    }
    /* check and decide if length is too short */
    for (j=0; j<MAD_MDI_PAIR_NUM; j++)
    {
         if (localTooShort[j]==MAD_FALSE) break;
    }

    if (j==MAD_MDI_PAIR_NUM)
          *tooShort = MAD_TRUE;
  
    return MAD_OK;
}

/************************************************************************************

*************************************************************************************/


static 
MAD_STATUS runAdvCableTest_1240_set
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_32           channel,
    IN  MAD_ADV_VCT_TRANS_CHAN_SEL        crosspair,
    IN  MAD_ADV_VCT_MOD     advMod
)
{
    MAD_STATUS retVal;


    MAD_DBG_INFO("runAdvCableTest_1240_set Called.\n");
#if 0
    /* Set transmit channel */
   if (crosspair!=MAD_ADV_VCT_TCS_NO_CROSSPAIR)
   {
    if((retVal = madHwSetPagedPhyRegField(
        dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,11,3,0x4+channel)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
    /* 
     * start Advanced Virtual Cable Tester
     */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,15,1,1)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
   }
   else
   {
    if (channel==0)
    {
      if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,11,3,0x000)) != MAD_OK)
      {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
      /* 
      * start Advanced Virtual Cable Tester
      */
      if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,15,1,1)) != MAD_OK)
      {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
    }
   }
#else
      /* 
      * start Advanced Virtual Cable Tester
      */
      if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,15,1,1)) != MAD_OK)
      {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }

#endif

    return MAD_OK;
}

static 
MAD_STATUS runAdvCableTest_1240_check
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
    if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,&u16Data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }
    if (u16Data & 0x8000)
        return MAD_PENDING;

    MAD_DBG_INFO("Page 5 of Reg23 after test : %0#x.\n", u16Data);

    return MAD_OK;
}

static 
MAD_STATUS runAdvCableTest_1240_get
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_ADV_VCT_TRANS_CHAN_SEL    crosspair,
     IN  MAD_32            channel,
    IN  MAD_ADV_VCT_MOD     advMod,
    OUT MAD_ADV_CABLE_STATUS *cableStatus,
    OUT MAD_BOOL         *tooShort
)
{
    MAD_STATUS retVal;
    MAD_U16 u16Data;
    MAD_U16 crossChannelReg[MAD_MDI_PAIR_NUM];
    int j;
    MAD_16  dist2fault;
    MAD_BOOL         mode;
    MAD_BOOL         localTooShort[MAD_MDI_PAIR_NUM];

    VCT_REGISTER regList[MAD_MDI_PAIR_NUM] = {
                    {5,16},{5,17},{5,18},{5,19} };
    mode = (dev->flags & MAD_FLAG_SHORT_CABLE_FIX)?MAD_TRUE:MAD_FALSE;

/*    if (mode)
        *tooShort = MAD_FALSE;
*/
    MAD_DBG_INFO("runAdvCableTest_1240_get Called.\n");


    if ((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,&u16Data)) != MAD_OK)
    {
            MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    MAD_DBG_INFO("Page 5 of Reg23 after test : %0#x.\n", u16Data);

    /*
     * read the test result for the cross pair against selected MDI Pair
     */
    for (j=0; j<MAD_MDI_PAIR_NUM; j++)
    {
      if((retVal = madHwReadPagedPhyReg(
                        dev,hwPort,
                        regList[j].page,
                        regList[j].regOffset,
                        &crossChannelReg[j])) != MAD_OK)
      {
         MAD_DBG_ERROR("Reading from paged phy reg failed.\n");
         return (retVal | MAD_API_FAIL_READ_PAGED_REG);
      }
      MAD_DBG_INFO("@@@@@ reg channel %d is %x \n", j, crossChannelReg[j]);
    }

    /*
     * analyze the test result for RX Pair
     */
    if (advMod==MAD_ADV_VCT_OFFSET)
    {
      for (j=0; j<MAD_MDI_PAIR_NUM; j++)
      {
        cableStatus->u[j].offset = (crossChannelReg[j]>>8)&0xff;
      }

      return MAD_OK;
    }

    if (advMod==MAD_ADV_VCT_SAMPLE_POINT)
    {
      for (j=0; j<MAD_MDI_PAIR_NUM; j++)
      {
        cableStatus->u[j].sampleAmp = (crossChannelReg[j]>>8)&0xff;
      }

      return MAD_OK;
    }

    for (j=0; j<MAD_MDI_PAIR_NUM; j++)
    {
      if (crosspair!=MAD_ADV_VCT_TCS_NO_CROSSPAIR)
        dist2fault = analizeAdvVCTResult(dev, j, crossChannelReg, mode&(*tooShort), cableStatus);
      else
        dist2fault = analizeAdvVCTNoCrosspairResult(dev, j, crossChannelReg, mode&(*tooShort), cableStatus);

      localTooShort[j]=MAD_FALSE;
      if((mode)&&(*tooShort==MAD_FALSE))
      {
        if ((dist2fault>=0) && (dist2fault<MAD_ADV_VCT_ACCEPTABLE_SHORT_CABLE))
        {
           MAD_DBG_INFO("@@@#@@@@ it is too short dist2fault %d\n", dist2fault);
           MAD_DBG_INFO("Distance to Fault is too Short. So, rerun after changing pulse width\n");
           localTooShort[j]=MAD_TRUE;
/*           *tooShort = MAD_TRUE;
           return MAD_OK;   Should return to stop for loop */
        }
      }
    }
    /* check and decide if length is too short */
    for (j=0; j<MAD_MDI_PAIR_NUM; j++)
    {
         if (localTooShort[j]==MAD_FALSE) break;
    }

    if (j==MAD_MDI_PAIR_NUM)
          *tooShort = MAD_TRUE;
  
    return MAD_OK;
}

/*******************************************************************************
* getAdvCableStatus_1181
*
* DESCRIPTION:
*       This routine performs the virtual cable test for the PHY with
*       multiple page mode and returns the the status per MDIP/N.
*
* INPUTS:
*       port - logical port number.
*       mode - advance VCT mode (either First Peak or Maximum Peak)
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
MAD_STATUS getAdvCableStatus_1181
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_ADV_VCT_MODE mode,
    OUT MAD_ADV_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS retVal;
    MAD_U16 orgPulse, u16Data;
    MAD_BOOL flag, tooShort;

    flag = (dev->flags & MAD_FLAG_SHORT_CABLE_FIX)?MAD_TRUE:MAD_FALSE;

    /*
     * set Adv VCT Mode
     */
    switch (mode.mode)
    {
        case MAD_ADV_VCT_FIRST_PEAK:
                break;
        case MAD_ADV_VCT_MAX_PEAK:
                break;
        case MAD_ADV_VCT_OFFSET:
        case MAD_ADV_VCT_SAMPLE_POINT:
      default:
                MAD_DBG_ERROR("Unknown Advanced VCT Mode.\n");
                return MAD_API_UNKNOWN_ADVVCT_MODE;
    }

    u16Data = (mode.mode<<6) | (mode.peakDetHyst) | (mode.sampleAvg<<8);
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,8,MAD_REG_ADV_VCT_CONTROL_8,0,11,u16Data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    if (flag)
    {
        /* save original Pulse Width */
        if((retVal = madHwGetPagedPhyRegField(
                        dev,hwPort,9,23,10,2,&orgPulse)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
        }

        /* set the Pulse Width with default value */
        if (orgPulse != 0)
        {
            if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,9,23,10,2,0)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
        }
    }

    if((retVal=runAdvCableTest_1181(dev,hwPort,flag,cableStatus,&tooShort)) != MAD_OK)
    {
        MAD_DBG_ERROR("Running advanced VCT failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_RUN);
    }

    if (flag)
    {
        if(tooShort)
        {
            /* set the Pulse Width with minimum width */
            if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,9,23,10,2,3)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }

            /* run the Adv VCT again */
            if((retVal=runAdvCableTest_1181(dev,hwPort,MAD_FALSE,cableStatus,&tooShort)) != MAD_OK)
            {
                MAD_DBG_ERROR("Running advanced VCT failed.\n");
                return (retVal | MAD_API_FAIL_ADVVCT_RUN);
            }

        }

        /* set the Pulse Width back to the original value */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,9,23,10,2,orgPulse)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

    }

    return MAD_OK;
}

#ifdef MAD_SM_VCT
/***********************************************************************************
*  AdvVct_1116 SM block 
************************************************************************************/

#define madAdv_1116_orgPulse        vctSmCfg->savedVeriable[0]
#define madAdv_1116_flag            vctSmCfg->savedVeriable[1]
#define madAdv_1116_tooShort        vctSmCfg->savedVeriable[2]
#define madAdv_1116_mode            vctSmCfg->savedVeriable[3]
#define madAdv_1116_transChanSel    vctSmCfg->savedVeriable[4]
#define madAdv_1116_sampleAvg        vctSmCfg->savedVeriable[5]
#define madAdv_1116_peakDetHyst     vctSmCfg->savedVeriable[6]


static 
MAD_STATUS advVct_1116_init
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;
    MAD_U16 u16Data;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);

  madAdv_1116_flag = (MAD_32)((dev->flags & MAD_FLAG_SHORT_CABLE_FIX)?MAD_TRUE:MAD_FALSE);

    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,0,13, &u16Data)) != MAD_OK)
    {
       MAD_DBG_ERROR("Reading paged phy reg failed.\n");
       return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }
    u16Data |= ((madAdv_1116_mode<<6) | (madAdv_1116_transChanSel<<11));
    if (madAdv_1116_peakDetHyst) u16Data |= (madAdv_1116_peakDetHyst);
    if (madAdv_1116_sampleAvg) u16Data |= (madAdv_1116_sampleAvg<<8) ;

    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,0,13,u16Data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    if (madAdv_1116_flag)
    {
        /* save original Pulse Width */
        if((retVal = madHwGetPagedPhyRegField(
                        dev,hwPort,5,28,10,2,(MAD_U16 *)&madAdv_1116_orgPulse)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
        }

        /* set the Pulse Width with default value */
        if (madAdv_1116_orgPulse != 0)
        {
            if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,5,28,10,2,0)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
        }
        madAdv_1116_tooShort = MAD_FALSE;
    }

    return MAD_OK;
}

static 
MAD_STATUS advVct_1116_set
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;
    MAD_ADV_VCT_TRANS_CHAN_SEL crosspair;
    MAD_32  channel;

  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
  crosspair = madAdv_1116_transChanSel;
  if (vctSmCfg->vctSmSt==MAD_VCT_SM_ST_PENDING_SUB)
    channel= vctSmCfg->subCheckCount - vctSmCfg->currCheckCount;
  else
    channel= vctSmCfg->checkCount - vctSmCfg->currCheckCount;

    if((retVal=runAdvCableTest_1116_set(dev,hwPort, channel, crosspair)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Running advanced VCT failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_SET);
    }


    return MAD_OK;
}

static 
MAD_STATUS advVct_1116_check
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;

    retVal=runAdvCableTest_1116_check(dev,hwPort);

    return retVal;
}

static 
MAD_STATUS advVct_1116_get
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
  MAD_STATUS retVal;
  MAD_ADV_VCT_TRANS_CHAN_SEL crosspair;
  MAD_32 channel;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
  MAD_ADV_CABLE_STATUS *cableStatus= (MAD_ADV_CABLE_STATUS *)(vctSmCfg->vctResultPtr);

  crosspair = madAdv_1116_transChanSel;

  if (vctSmCfg->vctSmSt==MAD_VCT_SM_ST_PENDING_SUB)
    channel= vctSmCfg->subCheckCount - vctSmCfg->currCheckCount;
  else
    channel= vctSmCfg->checkCount - vctSmCfg->currCheckCount;

  if((retVal=runAdvCableTest_1116_get(dev,hwPort, crosspair,
      channel,cableStatus,(MAD_BOOL *)&madAdv_1116_tooShort)) != MAD_OK)
    {
        MAD_DBG_ERROR("Running advanced VCT SM get failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_GET);
    }
   if (0x1&madAdv_1116_flag)
    {
        if(0x1&madAdv_1116_tooShort)
        {
            /* set the Pulse Width with minimum width */
            if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,5,28,10,2,3)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            madAdv_1116_flag=MAD_FALSE;
/*            MAD_CRITIC_INFO("Cable too short, try again!\n"); */
            return MAD_VCT_AGAIN;

        }
   }
   return MAD_OK;
}

static 
MAD_STATUS advVct_1116_close
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);

    if (madAdv_1116_flag)
    {         /* set the Pulse Width back to the original value */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,5,28,10,2,madAdv_1116_orgPulse)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

    }
    madVCTCloseSM (dev);
    return MAD_OK;
}

static    MAD_VCT_SM_ACTIONS  advVct1116SmActions = {
                                        advVct_1116_init,
                                        advVct_1116_set,
                                        advVct_1116_check,
                                        advVct_1116_get,
                                        advVct_1116_set,
                                        advVct_1116_get,
                                        advVct_1116_close
                                        };

static 
MAD_STATUS getAdvCableStatus_1116
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_ADV_VCT_MODE mode,
    OUT MAD_ADV_CABLE_STATUS *cableStatus
)
{
   MAD_STATUS retVal;
   MAD_32 checkCount, subCheckCount;

   MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
   vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);

   madAdv_1116_mode = mode.mode;
   madAdv_1116_transChanSel = mode.transChanSel;
   madAdv_1116_sampleAvg = mode.sampleAvg;
   madAdv_1116_peakDetHyst = mode.peakDetHyst;

    /*
     * Check Adv VCT Mode
     */
    switch (mode.mode)
    {
        case MAD_ADV_VCT_FIRST_PEAK:
        case MAD_ADV_VCT_MAX_PEAK:
                break;
        case MAD_ADV_VCT_OFFSET:
        case MAD_ADV_VCT_SAMPLE_POINT:
        default:
                MAD_DBG_ERROR("Unknown ADV VCT Mode, do not support.\n");
                return MAD_API_UNKNOWN_ADVVCT_MODE;
    }

   if ((mode.transChanSel)!=MAD_ADV_VCT_TCS_NO_CROSSPAIR)
   {
       checkCount=1; /* 4; */
       subCheckCount=1;  /* 4; */
   }
   else
   {
       checkCount=1;
       subCheckCount=1;
   }

    if((retVal = madVCTFindSM(dev, hwPort, MAD_VCT_SM_VCT_TYPE_ADV_TDR,
                  &advVct1116SmActions, MAD_TRUE, 
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
MAD_STATUS runAdvCableTest_1116
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_BOOL         mode,
    IN  MAD_ADV_VCT_TRANS_CHAN_SEL   crosspair,
    OUT MAD_ADV_CABLE_STATUS *cableStatus,
    OUT MAD_BOOL         *tooShort
)
{
  MAD_STATUS retVal;
  MAD_32  channel;

  MAD_DBG_INFO("runAdvCableTest_1116 Called.\n");

/*  if (mode)
        *tooShort = MAD_FALSE;  */

  if (crosspair!=MAD_ADV_VCT_TCS_NO_CROSSPAIR)
  {
     channel = crosspair - MAD_ADV_VCT_TCS_CROSSPAIR_0;
  }
  else
  {
     channel = 0;
  }
/*  for (channel=0; channel<MAD_MDI_PAIR_NUM; channel++) */
  {
    /* Set transmit channel */
    if((retVal=runAdvCableTest_1116_set(dev,hwPort, channel, crosspair)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Running advanced VCT failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_SET);
    }


    /* 
     * loop until test completion and result is valid
     */
    do 
    {
      if (((retVal=runAdvCableTest_1116_check(dev,hwPort)) != MAD_OK)&&
            ((retVal&MAD_RUN_ST_MASK) != MAD_PENDING))
      {
        MAD_DBG_ERROR("Running advanced VCT failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_CHECK);
      }
    } while((retVal&MAD_RUN_ST_MASK) == MAD_PENDING);

        /*
         * read the test result for the cross pair against selected MDI Pair
         */
      if((retVal=runAdvCableTest_1116_get(dev,hwPort, crosspair,
                   channel,cableStatus,(MAD_BOOL *)tooShort)) != MAD_OK)
      {
        MAD_DBG_ERROR("Running advanced VCT SM get failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_GET);

      }
  }
  return MAD_OK;
}

static 
MAD_STATUS getAdvCableStatus_1116
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_ADV_VCT_MODE mode,
    OUT MAD_ADV_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS retVal;
    MAD_U16 orgPulse, u16Data;
    MAD_BOOL flag, tooShort;
    MAD_ADV_VCT_TRANS_CHAN_SEL crosspair;

    MAD_CRITIC_INFO("Enter ADV VCT Non-Pendding Mode.\n");

    flag = (dev->flags & MAD_FLAG_SHORT_CABLE_FIX)?MAD_TRUE:MAD_FALSE;
    crosspair = mode.transChanSel;

    /*
     * Check Adv VCT Mode
     */
    switch (mode.mode)
    {
        case MAD_ADV_VCT_FIRST_PEAK:
        case MAD_ADV_VCT_MAX_PEAK:
                break;
        case MAD_ADV_VCT_OFFSET:
        case MAD_ADV_VCT_SAMPLE_POINT:
        default:
                MAD_DBG_ERROR("Unknown ADV VCT Mode.\n");
                return MAD_API_UNKNOWN_ADVVCT_MODE;
    }

    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,0,13, &u16Data)) != MAD_OK)
    {
       MAD_DBG_ERROR("Reading paged phy reg failed.\n");
       return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }
    u16Data |= ((mode.mode<<6) | (mode.transChanSel<<11));
    if (mode.peakDetHyst) u16Data |= (mode.peakDetHyst);
    if (mode.sampleAvg) u16Data |= (mode.sampleAvg<<8) ;
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,0,13,u16Data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    if (flag)
    {
        /* save original Pulse Width */
        if((retVal = madHwGetPagedPhyRegField(
                        dev,hwPort,5,28,10,2,&orgPulse)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
        }

        /* set the Pulse Width with default value */
        if (orgPulse != 0)
        {
            if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,5,28,10,2,0)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
        }
        tooShort=MAD_FALSE;
    }
    if((retVal=runAdvCableTest_1116(dev,hwPort,flag,crosspair, 
                cableStatus,&tooShort)) != MAD_OK)
    {
        MAD_DBG_ERROR("Running advanced VCT failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_RUN);
    }

    if (flag)
    {
        if(tooShort)
        {
            /* set the Pulse Width with minimum width */
            if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,5,28,10,2,3)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }

            /* run the Adv VCT again */
            MAD_CRITIC_INFO("Cable is too short, try again!\n");
            if((retVal=runAdvCableTest_1116(dev,hwPort,MAD_FALSE,crosspair,
                        cableStatus,&tooShort)) != MAD_OK)
            {
                MAD_DBG_ERROR("Running advanced VCT failed.\n");
                return (retVal | MAD_API_FAIL_ADVVCT_RUN);
            }

        }

        /* set the Pulse Width back to the original value */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,5,28,10,2,orgPulse)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

    }

    return MAD_OK;
}



#endif

/* 1240 like Phys */

#ifdef MAD_SM_VCT
/***********************************************************************************
*  AdvVct_1240 SM block 
************************************************************************************/

#define madAdv_1240_orgPulse        vctSmCfg->savedVeriable[0]
#define madAdv_1240_flag            vctSmCfg->savedVeriable[1]
#define madAdv_1240_tooShort        vctSmCfg->savedVeriable[2]
#define madAdv_1240_mode            vctSmCfg->savedVeriable[3]
#define madAdv_1240_transChanSel    vctSmCfg->savedVeriable[4]
#define madAdv_1240_sampleAvg        vctSmCfg->savedVeriable[5]
#define madAdv_1240_peakDetHyst     vctSmCfg->savedVeriable[6]
#define madAdv_1240_samplePointDist    vctSmCfg->savedVeriable[7]


static 
MAD_STATUS advVct_1240_init
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;
    MAD_U16 u16Data;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);

  madAdv_1240_flag = (MAD_32)((dev->flags & MAD_FLAG_SHORT_CABLE_FIX)?MAD_TRUE:MAD_FALSE);

    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,0,13, &u16Data)) != MAD_OK)
    {
       MAD_DBG_ERROR("Reading paged phy reg failed.\n");
       return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }
    u16Data |= ((madAdv_1240_mode<<6) | (madAdv_1240_transChanSel<<11));
    if (madAdv_1240_peakDetHyst) u16Data |= (madAdv_1240_peakDetHyst);
    if (madAdv_1240_sampleAvg) u16Data |= (madAdv_1240_sampleAvg<<8) ;

    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,0,13,u16Data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
    if(madAdv_1240_mode==MAD_ADV_VCT_SAMPLE_POINT)
    {
      if (madAdv_1240_samplePointDist) u16Data = (madAdv_1240_samplePointDist&0x1ff) ;
      if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_SMPL_DIST,0,8,u16Data)) != MAD_OK)
      {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
    }

    if (madAdv_1240_flag)
    {
        /* save original Pulse Width */
        if((retVal = madHwGetPagedPhyRegField(
                        dev,hwPort,5,28,10,2,(MAD_U16 *)&madAdv_1240_orgPulse)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
        }

        /* set the Pulse Width with default value */
        if (madAdv_1240_orgPulse != 0)
        {
            if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,5,28,10,2,0)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
        }
        madAdv_1240_tooShort = MAD_FALSE;
    }

    return MAD_OK;
}

static 
MAD_STATUS advVct_1240_set
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;
    MAD_ADV_VCT_TRANS_CHAN_SEL crosspair;
    MAD_32  channel;

  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
  crosspair = madAdv_1240_transChanSel;
  if (vctSmCfg->vctSmSt==MAD_VCT_SM_ST_PENDING_SUB)
    channel= vctSmCfg->subCheckCount - vctSmCfg->currCheckCount;
  else
    channel= vctSmCfg->checkCount - vctSmCfg->currCheckCount;

    if((retVal=runAdvCableTest_1240_set(dev,hwPort, channel, crosspair,madAdv_1240_mode)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Running advanced VCT failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_SET);
    }


    return MAD_OK;
}

static 
MAD_STATUS advVct_1240_check
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;

    retVal=runAdvCableTest_1240_check(dev,hwPort);

    return retVal;
}

static 
MAD_STATUS advVct_1240_get
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
  MAD_STATUS retVal;
  MAD_ADV_VCT_TRANS_CHAN_SEL crosspair;
  MAD_32 channel;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
  MAD_ADV_CABLE_STATUS *cableStatus= (MAD_ADV_CABLE_STATUS *)(vctSmCfg->vctResultPtr);

  crosspair = madAdv_1240_transChanSel;

  if (vctSmCfg->vctSmSt==MAD_VCT_SM_ST_PENDING_SUB)
    channel= vctSmCfg->subCheckCount - vctSmCfg->currCheckCount;
  else
    channel= vctSmCfg->checkCount - vctSmCfg->currCheckCount;

  if((retVal=runAdvCableTest_1240_get(dev,hwPort, crosspair,
      channel,madAdv_1240_mode, cableStatus,(MAD_BOOL *)&madAdv_1240_tooShort)) != MAD_OK)
    {
        MAD_DBG_ERROR("Running advanced VCT SM get failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_GET);
    }
   if (0x1&madAdv_1240_flag)
    {
        if(0x1&madAdv_1240_tooShort)
        {
            /* set the Pulse Width with minimum width */
            if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,5,28,10,2,3)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
            madAdv_1240_flag=MAD_FALSE;
/*            MAD_CRITIC_INFO("Cable too short, try again!\n"); */
            return MAD_VCT_AGAIN;

        }
   }
   return MAD_OK;
}

static 
MAD_STATUS advVct_1240_close
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort
)
{
    MAD_STATUS retVal;
  MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);

    if (madAdv_1240_flag)
    {         /* set the Pulse Width back to the original value */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,5,28,10,2,madAdv_1240_orgPulse)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

    }
    madVCTCloseSM (dev);
    return MAD_OK;
}

static    MAD_VCT_SM_ACTIONS  advVct1240SmActions = {
                                        advVct_1240_init,
                                        advVct_1240_set,
                                        advVct_1240_check,
                                        advVct_1240_get,
                                        advVct_1240_set,
                                        advVct_1240_get,
                                        advVct_1240_close
                                        };

static 
MAD_STATUS getAdvCableStatus_1240
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_ADV_VCT_MODE mode,
    OUT MAD_ADV_CABLE_STATUS *cableStatus
)
{
   MAD_STATUS retVal;
   MAD_32 checkCount, subCheckCount;

   MAD_VCT_SM_CFG    *vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
   vctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);

   madAdv_1240_mode = mode.mode;
   madAdv_1240_transChanSel = mode.transChanSel;
   madAdv_1240_sampleAvg = mode.sampleAvg;
   madAdv_1240_peakDetHyst = mode.peakDetHyst;
   madAdv_1240_samplePointDist = mode.samplePointDist; 


    /*
     * Check Adv VCT Mode
     */
    switch (mode.mode)
    {
        case MAD_ADV_VCT_FIRST_PEAK:
        case MAD_ADV_VCT_MAX_PEAK:
        case MAD_ADV_VCT_OFFSET:
        case MAD_ADV_VCT_SAMPLE_POINT:
                break;
        default:
                MAD_DBG_ERROR("Unknown ADV VCT Mode.\n");
                return MAD_API_UNKNOWN_ADVVCT_MODE;
    }

   if ((mode.transChanSel)!=MAD_ADV_VCT_TCS_NO_CROSSPAIR)
   {
       checkCount=1; /* 4; */
       subCheckCount=1;  /* 4; */
   }
   else
   {
       checkCount=1;
       subCheckCount=1;
   }

    if((retVal = madVCTFindSM(dev, hwPort, MAD_VCT_SM_VCT_TYPE_ADV_TDR,
                  &advVct1240SmActions, MAD_TRUE, 
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
MAD_STATUS runAdvCableTest_1240
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_BOOL         mode,
    IN  MAD_ADV_VCT_TRANS_CHAN_SEL   crosspair,
    IN  MAD_ADV_VCT_MOD     advMod,
    OUT MAD_ADV_CABLE_STATUS *cableStatus,
    OUT MAD_BOOL         *tooShort
)
{
  MAD_STATUS retVal;
  MAD_32  channel;

  MAD_DBG_INFO("runAdvCableTest_1240 Called.\n");

/*  if (mode)
        *tooShort = MAD_FALSE;  */

  if (crosspair!=MAD_ADV_VCT_TCS_NO_CROSSPAIR)
  {
     channel = crosspair - MAD_ADV_VCT_TCS_CROSSPAIR_0;
  }
  else
  {
     channel = 0;
  }
/*  for (channel=0; channel<MAD_MDI_PAIR_NUM; channel++) */
  {
    /* Set transmit channel */
    if((retVal=runAdvCableTest_1240_set(dev,hwPort, channel, crosspair, advMod)) != MAD_OK) 
    {
        MAD_DBG_ERROR("Running advanced VCT failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_SET);
    }


    /* 
     * loop until test completion and result is valid
     */
    do 
    {
      if (((retVal=runAdvCableTest_1240_check(dev,hwPort)) != MAD_OK)&&
            ((retVal&MAD_RUN_ST_MASK) != MAD_PENDING))
      {
        MAD_DBG_ERROR("Running advanced VCT failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_CHECK);
      }
    } while((retVal&MAD_RUN_ST_MASK) == MAD_PENDING);

        /*
         * read the test result for the cross pair against selected MDI Pair
         */
      if((retVal=runAdvCableTest_1240_get(dev,hwPort, crosspair, channel,
                     advMod, cableStatus,(MAD_BOOL *)tooShort)) != MAD_OK)
      {
        MAD_DBG_ERROR("Running advanced VCT SM get failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_SM_GET);

      }
  }
  return MAD_OK;
}

static 
MAD_STATUS getAdvCableStatus_1240
(    
    IN  MAD_DEV          *dev,
    IN  MAD_U8           hwPort,
    IN  MAD_ADV_VCT_MODE mode,
    OUT MAD_ADV_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS retVal;
    MAD_U16 orgPulse, u16Data;
    MAD_BOOL flag, tooShort;
    MAD_ADV_VCT_TRANS_CHAN_SEL crosspair;


    MAD_CRITIC_INFO("Enter ADV VCT Non-Pendding Mode.\n");

    flag = (dev->flags & MAD_FLAG_SHORT_CABLE_FIX)?MAD_TRUE:MAD_FALSE;
    crosspair = mode.transChanSel;

    /*
     * Check Adv VCT Mode
     */
    switch (mode.mode)
    {
        case MAD_ADV_VCT_FIRST_PEAK:
        case MAD_ADV_VCT_MAX_PEAK:
        case MAD_ADV_VCT_OFFSET:
        case MAD_ADV_VCT_SAMPLE_POINT:
                break;
        default:
                MAD_DBG_ERROR("Unknown ADV VCT Mode.\n");
                return MAD_API_UNKNOWN_ADVVCT_MODE;
    }

    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,0,13, &u16Data)) != MAD_OK)
    {
       MAD_DBG_ERROR("Reading paged phy reg failed.\n");
       return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
    }
    u16Data |= ((mode.mode<<6) | (mode.transChanSel<<11));
    if (mode.peakDetHyst) u16Data |= (mode.peakDetHyst);
    if (mode.sampleAvg) u16Data |= (mode.sampleAvg<<8) ;
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_CONTROL_5,0,13,u16Data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }
    if(mode.mode==MAD_ADV_VCT_SAMPLE_POINT)
    {
      if (mode.samplePointDist) u16Data = (mode.samplePointDist&0x1ff) ;
      if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,5,MAD_REG_ADV_VCT_SMPL_DIST,0,8,u16Data)) != MAD_OK)
      {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
      }
    }

    if (flag)
    {
        /* save original Pulse Width */
        if((retVal = madHwGetPagedPhyRegField(
                        dev,hwPort,5,28,10,2,&orgPulse)) != MAD_OK)
        {
            MAD_DBG_ERROR("Reading paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
        }

        /* set the Pulse Width with default value */
        if (orgPulse != 0)
        {
            if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,5,28,10,2,0)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }
        }
        tooShort=MAD_FALSE;
    }
    if((retVal=runAdvCableTest_1240(dev,hwPort,flag,crosspair, mode.mode,
                cableStatus,&tooShort)) != MAD_OK)
    {
        MAD_DBG_ERROR("Running advanced VCT failed.\n");
        return (retVal | MAD_API_FAIL_ADVVCT_RUN);
    }

    if (flag)
    {
        if(tooShort)
        {
            /* set the Pulse Width with minimum width */
            if((retVal = madHwSetPagedPhyRegField(
                            dev,hwPort,5,28,10,2,3)) != MAD_OK)
            {
                MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
                return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
            }

            /* run the Adv VCT again */
            MAD_CRITIC_INFO("Cable is too short, try again!\n");
            if((retVal=runAdvCableTest_1240(dev,hwPort,MAD_FALSE,crosspair, mode.mode,
                        cableStatus,&tooShort)) != MAD_OK)
            {
                MAD_DBG_ERROR("Running advanced VCT failed.\n");
                return (retVal | MAD_API_FAIL_ADVVCT_RUN);
            }

        }

        /* set the Pulse Width back to the original value */
        if((retVal = madHwSetPagedPhyRegField(
                        dev,hwPort,5,28,10,2,orgPulse)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
        }

    }

    return MAD_OK;
}



#endif























/*******************************************************************************
* mdDiagGetAdvCableStatus
*
* DESCRIPTION:
*       This routine performs the advanced VCT for the requested port,
*       and returns the status per MDI pair.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       mode - advance VCT mode (either First Peak or Maximum Peak)
*
* OUTPUTS:
*       cableStatus - test status and cable length
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15    : Soft reset
*       Page 5, Reg 21, Bit 6    : DSP Valid
*       Page 8, Reg 20, Bit 15:6 : Adv VCT Configuration Register
*       Page 8, Reg 16~19, Bit 15:0 : MDI[0] VCT Register
*       Page 8, Reg 24~27, Bit 15:0 : MDI[1] VCT Register
*       Page 9, Reg 16~19, Bit 15:0 : MDI[2] VCT Register
*       Page 9, Reg 24~27, Bit 15:0 : MDI[3] VCT Register
*
*******************************************************************************/
MAD_STATUS mdDiagGetAdvCableStatus
(
    IN  MAD_DEV*        dev,
    IN  MAD_LPORT       port,
    IN  MAD_ADV_VCT_MODE mode,
    OUT MAD_ADV_CABLE_STATUS *cableStatus
)
{
    MAD_STATUS retVal;
    MAD_U8 hwPort;

    MAD_DBG_INFO("madDiagGetAdvCableStatus Called.\n");

    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY(dev, MAD_PHY_ADV_VCT_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("VCT not supported.\n");
        return MAD_API_ADVTDRVCT_NOT_SUPPORT;
    }

    switch (dev->deviceId)
    {
        case MAD_88E1181:   
          if((retVal=getAdvCableStatus_1181(dev,hwPort,mode,cableStatus)) != MAD_OK)
         {
            MAD_DBG_ERROR("Running advanced VCT failed.\n");
            return (retVal | MAD_API_FAIL_ADVVCT_RUN);
         }
         break;
        case MAD_88E1116:   
        case MAD_88E1121:   
        case MAD_88E1149:   
        if(((retVal=getAdvCableStatus_1116(dev,hwPort,mode,cableStatus)) != MAD_OK) &&
             ((retVal&MAD_RUN_ST_MASK)!=MAD_PENDING))
         {
            MAD_DBG_ERROR("Running advanced VCT failed.\n");
            return (retVal | MAD_API_FAIL_ADVVCT_RUN);
         }
         if ((retVal&MAD_RUN_ST_MASK)==MAD_PENDING)
         {
             return retVal;
         }
         break;
        case MAD_88E1149R:   
        case MAD_SWG65G : 
        case MAD_88E1240:   
        case MAD_88E1340S:   
        case MAD_88E1340:   
        case MAD_88E1340M:   
        case MAD_88E1119R:   
        case MAD_88E1310:   
        case MAD_88E1540:   
         if(((retVal=getAdvCableStatus_1240(dev,hwPort,mode,cableStatus)) != MAD_OK) && 
             ((retVal&MAD_RUN_ST_MASK)!=MAD_PENDING))
         {
            MAD_DBG_ERROR("Running advanced VCT failed.\n");
            return (retVal | MAD_API_FAIL_ADVVCT_RUN);
         }
         if ((retVal&MAD_RUN_ST_MASK)==MAD_PENDING)
         {
             return retVal;
         }
         break;
        default:
            return MAD_API_ADVTDRVCT_NOT_SUPPORT;
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

