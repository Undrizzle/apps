#include <madCopyright.h>

/********************************************************************************
* madVctSM.c
* 
* DESCRIPTION:
*       State Machine Functions for VCT (TDR and DSP).
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


#define MAD_VCT_SM_NUM 1
static MAD_VCT_SM_CFG    madVctSmCfg[MAD_VCT_SM_NUM];

void    madVctSmInit(void ** smCfgPtr)
{
  int i;

  for (i=0; i<MAD_VCT_SM_NUM; i++)
  {
    madVctSmCfg[i].vctSmSt=MAD_VCT_SM_ST_FREE;
    madVctSmCfg[i].dev=NULL;
    madVctSmCfg[i].port=0;
    madVctSmCfg[i].currentVctType=MAD_VCT_SM_VCT_TYPE_MAX;
    madVctSmCfg[i].subPending=MAD_FALSE;
    madVctSmCfg[i].checkCount=0;
    madVctSmCfg[i].subCheckCount=0;
    madVctSmCfg[i].currCheckCount=0;
    madVctSmCfg[i].vctResultPtr=NULL;
    madVctSmCfg[i].vctSmAct.vctSmActInit=NULL;
    madVctSmCfg[i].vctSmAct.vctSmActSet=NULL;
    madVctSmCfg[i].vctSmAct.vctSmActCheck=NULL;
    madVctSmCfg[i].vctSmAct.vctSmActGet=NULL;
    madVctSmCfg[i].vctSmAct.vctSmActSubSet=NULL;
    madVctSmCfg[i].vctSmAct.vctSmActSubGet=NULL;
    madVctSmCfg[i].vctSmAct.vctSmActClose=NULL;
  }
  *smCfgPtr=(void *)&madVctSmCfg[0];
   
}

static void setVctSmSt (MAD_DEV *dev, MAD_VCT_SM_STATE *vctSmSt, MAD_VCT_SM_STATE currSt)
{
    *vctSmSt=currSt; 
    if (currSt==MAD_VCT_SM_ST_FREE)
        (dev->SMDataPtr)->smSt=MAD_SM_FREE;
    else
        (dev->SMDataPtr)->smSt=MAD_SM_PENDING;
}

void madVCTInitSM(MAD_DEV*  dev, MAD_U8 port, MAD_VCT_SM_VCT_TYPE vctType, 
                  MAD_VCT_SM_ACTIONS *actions, MAD_BOOL subPending, 
                  MAD_VCT_DATA_STRUCT  *vctData, MAD_32 checkCount,
                  MAD_32 subCheckCount)
{
  MAD_VCT_SM_CFG    *currentVctSmCfg;

  currentVctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);

  setVctSmSt (dev, &(currentVctSmCfg->vctSmSt), MAD_VCT_SM_ST_INIT);
    currentVctSmCfg->dev=dev;
    currentVctSmCfg->port=port;
    currentVctSmCfg->currentVctType=vctType;
    currentVctSmCfg->subPending=subPending;
    currentVctSmCfg->checkCount=checkCount;
    currentVctSmCfg->currCheckCount=checkCount;
    currentVctSmCfg->subCheckCount=subCheckCount;
    currentVctSmCfg->vctResultPtr=vctData;
    
    currentVctSmCfg->vctSmAct.vctSmActInit=actions->vctSmActInit;
    currentVctSmCfg->vctSmAct.vctSmActSet=actions->vctSmActSet;
    currentVctSmCfg->vctSmAct.vctSmActCheck=actions->vctSmActCheck;
    currentVctSmCfg->vctSmAct.vctSmActGet=actions->vctSmActGet;
    currentVctSmCfg->vctSmAct.vctSmActSubSet=actions->vctSmActSubSet;
    currentVctSmCfg->vctSmAct.vctSmActSubGet=actions->vctSmActSubGet;
    currentVctSmCfg->vctSmAct.vctSmActClose=actions->vctSmActClose;

}

void madVCTCloseSM(MAD_DEV*        dev)
{
  MAD_VCT_SM_CFG    *currentVctSmCfg;

  currentVctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);

    setVctSmSt (dev, &(currentVctSmCfg->vctSmSt), MAD_VCT_SM_ST_FREE);
    currentVctSmCfg->dev=NULL;
    currentVctSmCfg->port=0;
    currentVctSmCfg->currentVctType=MAD_VCT_SM_VCT_TYPE_MAX;
    currentVctSmCfg->subPending=MAD_FALSE;
    currentVctSmCfg->checkCount=0;
    currentVctSmCfg->subCheckCount=0;
    currentVctSmCfg->currCheckCount=0;
    currentVctSmCfg->vctResultPtr=NULL;
    currentVctSmCfg->vctSmAct.vctSmActInit=NULL;
    currentVctSmCfg->vctSmAct.vctSmActSet=NULL;
    currentVctSmCfg->vctSmAct.vctSmActCheck=NULL;
    currentVctSmCfg->vctSmAct.vctSmActGet=NULL;
    currentVctSmCfg->vctSmAct.vctSmActSubSet=NULL;
    currentVctSmCfg->vctSmAct.vctSmActSubGet=NULL;
    currentVctSmCfg->vctSmAct.vctSmActClose=NULL;

}


MAD_STATUS madVCTFindSM(MAD_DEV*  dev, MAD_U8 port, MAD_VCT_SM_VCT_TYPE vctType,
                  MAD_VCT_SM_ACTIONS *actions, MAD_BOOL subPending,
                  MAD_VCT_DATA_STRUCT  *vctData, MAD_32 checkCount, 
                  MAD_32 subCheckCount)
{
  MAD_VCT_SM_CFG    *currentVctSmCfg;

  currentVctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);
    if (currentVctSmCfg->vctSmSt!=MAD_VCT_SM_ST_FREE)
    {
        /* check if it is a matched SM */
        if ((dev->deviceId==(currentVctSmCfg->dev)->deviceId) && 
             (vctType==currentVctSmCfg->currentVctType) &&
             (port == currentVctSmCfg->port))

        {
            currentVctSmCfg=&madVctSmCfg[0];
            return MAD_OK;
        }
        else
            return MAD_API_FAIL_STATE_MACHINE;
    }
    else
    {
        madVCTInitSM (dev, port, vctType, actions, subPending, vctData, 
                      checkCount, subCheckCount);
    }
    return MAD_OK;
}


MAD_STATUS madVCTImplSM(MAD_DEV*  dev, MAD_U8 port,    MAD_VCT_SM_EVT event)
{
  MAD_VCT_SM_CFG    *currentVctSmCfg;
  MAD_STATUS status;

  currentVctSmCfg=(MAD_VCT_SM_CFG    *)((dev->SMDataPtr)->smCfg);

    switch (currentVctSmCfg->vctSmSt)
    {
    case MAD_VCT_SM_ST_INIT:
        {
            switch (event)
            {
            case MAD_VCT_SM_EVT_CMD:
                if (((status=currentVctSmCfg->vctSmAct.vctSmActInit(dev, port))!=MAD_OK)&&((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
                    return MAD_API_FAIL_STATE_MACHINE|status;
                if (((status=currentVctSmCfg->vctSmAct.vctSmActSet(dev, port))!=MAD_OK)&&((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
                    return MAD_API_FAIL_STATE_MACHINE|status;
                setVctSmSt (dev, &(currentVctSmCfg->vctSmSt), MAD_VCT_SM_ST_PENDING);
                return MAD_PENDING;
                break;
            case MAD_VCT_SM_EVT_POLL:
                return MAD_OK;
                break;
            default:
                break;
                
            }

        }
        break;
    case MAD_VCT_SM_ST_PENDING:
        {
            switch (event)
            {
            case MAD_VCT_SM_EVT_CMD:
            case MAD_VCT_SM_EVT_POLL:
                if (((status=currentVctSmCfg->vctSmAct.vctSmActCheck(dev, port))==MAD_OK)&&((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
                {
                    if (((status=currentVctSmCfg->vctSmAct.vctSmActGet(dev, port))&MAD_RUN_ST_MASK) ==MAD_VCT_AGAIN)
                    {
                        if (currentVctSmCfg->subPending==MAD_TRUE)
                        {
                            setVctSmSt (dev, &(currentVctSmCfg->vctSmSt), MAD_VCT_SM_ST_PENDING_SUB);
                            currentVctSmCfg->currCheckCount=currentVctSmCfg->subCheckCount;
                            status=currentVctSmCfg->vctSmAct.vctSmActSubSet(dev, port);
                            if ((status!=MAD_OK)&&((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
                              return MAD_API_FAIL_STATE_MACHINE|status;
                            return MAD_PENDING;
                        }
                        else
                            return MAD_API_FAIL_STATE_MACHINE;
                    }
                    else if ((status!=MAD_OK)&&((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
                        return MAD_API_FAIL_STATE_MACHINE|status;
                    

/*                    if ((--(currentVctSmCfg->currCheckCount)) >= 0)  */
                    if ((--(currentVctSmCfg->currCheckCount)) > 0) 
                    {
                        if (((status=currentVctSmCfg->vctSmAct.vctSmActSet(dev, port))!=MAD_OK)&&((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
                            return MAD_API_FAIL_STATE_MACHINE|status;
                        return MAD_PENDING;
                    }
                    else
                    {
                        currentVctSmCfg->currCheckCount=0;

                        if (event==MAD_VCT_SM_EVT_CMD)
                        {
                            setVctSmSt (dev, &(currentVctSmCfg->vctSmSt), MAD_VCT_SM_ST_INIT);
                            status=currentVctSmCfg->vctSmAct.vctSmActClose(dev, port);
                            return status;
                        }
                        return MAD_OK;
                    }
                }
                else if ((status != MAD_OK)&&((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
                       return MAD_API_FAIL_STATE_MACHINE|status;

                return MAD_PENDING;
                break;
            default:
                return MAD_API_FAIL_STATE_MACHINE;
                break;
                
            }

        }
        break;
    case MAD_VCT_SM_ST_PENDING_SUB:
        {
            switch (event)
            {
            case MAD_VCT_SM_EVT_POLL:
            case MAD_VCT_SM_EVT_CMD:
                if (((status=currentVctSmCfg->vctSmAct.vctSmActCheck(dev, port))==MAD_OK)&&((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
                {
                    if ((currentVctSmCfg->currCheckCount)--)
                    {
                        status=currentVctSmCfg->vctSmAct.vctSmActSubSet(dev, port);
                        if ((status!=MAD_OK)&&((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
                            return MAD_API_FAIL_STATE_MACHINE|status;
                        return MAD_PENDING;
                    }
                    else
                    {
                      currentVctSmCfg->currCheckCount=0;
                      if (event==MAD_VCT_SM_EVT_CMD)
                      {
                        status=currentVctSmCfg->vctSmAct.vctSmActSubGet(dev, port);
                        if ((status!=MAD_OK)&&((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
                            return MAD_API_FAIL_STATE_MACHINE|status;
                        setVctSmSt (dev, &(currentVctSmCfg->vctSmSt), MAD_VCT_SM_ST_INIT);
                        status=currentVctSmCfg->vctSmAct.vctSmActClose(dev, port);
                        if ((status!=MAD_OK)&&((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
                            return MAD_API_FAIL_STATE_MACHINE|status;
                      }
                      return MAD_OK;
                    }
                }
                else if ((status!=MAD_OK)&&((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
                    return MAD_API_FAIL_STATE_MACHINE|status;
                return MAD_PENDING;
                break;
            default:
                return MAD_API_FAIL_STATE_MACHINE;
                break;
                
            }

        }
        break;
    default:
        return MAD_API_FAIL_STATE_MACHINE;
        break;
    }
    return MAD_API_FAIL_STATE_MACHINE;

}



/*******************************************************************************
* mdGetSMState
*
* DESCRIPTION:
*       This routine retrieves the state of VCT State Machine.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_API_FAIL_STATE_MACHINE - on error
*        MAD_PENDDING - on state machine pending
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS mdGetSMState
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port
)
{

  if ((dev->SMDataPtr)->smSt == MAD_SM_PENDING)
      return  madVCTImplSM(dev, MAD_LPORT_2_PORT(port), MAD_VCT_SM_EVT_POLL);
  else
      return MAD_OK;
 
}


/*******************************************************************************
* mdStopSM
*
* DESCRIPTION:
*       This routine stop to run VCT State Machine, and set SM free.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_API_FAIL_STATE_MACHINE - on error
*        MAD_PENDDING - on state machine pending
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS mdStopSM
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port
)
{
    madVCTCloseSM (dev);
    mdSysSoftReset (dev, port);
    return MAD_OK;
}

