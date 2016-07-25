#include <madCopyright.h>

/********************************************************************************
* madSysConfig.c
*
* DESCRIPTION:
*       API definitions for system configuration, and enabling.
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
#include <madDevConfig.h>
#include <madUtils.h>
#include <madHwCntl.h>
#include <madSem.h>
#include <madPlatformDeps.h>
#ifdef MAD_DSP_VCT
#include <madDSPVctApi.h>
#include <madDSPVct.h>
#endif

static MAD_BOOL fnRegister(MAD_DEV *dev, MAD_BSP_FUNCTIONS* pBSPFunctions);

/*******************************************************************************
* mdLoadDriver
*
* DESCRIPTION:
*       Marvell Alaska Driver Initialization Routine. 
*       This is the first routine that needs be called by system software. 
*       It takes *cfg from system software, and retures a pointer (*dev) 
*       to a data structure which includes infomation related to this Marvell Phy
*       device. This pointer (*dev) is then used for all the API functions. 
*       The following is the job performed by this routine:
*           1. store SMI read/write function into the given MAD_DEV structure
*           2. look for the Marvell Device
*           3. run Device specific initialization routine
*           4. disable Marvell Device interrupt
*           5. create semaphore if required
*
* INPUTS:
*       cfg  - Holds device configuration parameters provided by system software.
*
* OUTPUTS:
*       dev  - pointer to Holds device information to be used for each API call.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*       mdUnloadDriver is also provided to do driver cleanup.
*
*******************************************************************************/
MAD_STATUS mdLoadDriver
(
    IN  MAD_SYS_CONFIG  *cfg,
    OUT MAD_DEV         *dev
)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_INT_TYPE intType;

    MAD_DBG_INFO("madLoadDriver Called.\n");

    /* Check for parameters validity        */
    if(dev == NULL)
    {
        MAD_DBG_ERROR("driver struture is NULL.\n");
        return MAD_API_ERR_DEV;
    }

    /* Check for parameters validity        */
    if(cfg == NULL)
    {
        MAD_DBG_ERROR("sys config is NULL.\n");
        return MAD_API_ERR_SYS_CFG;
    }
    /* The initialization was already done. */
    if(dev->devEnabled)
    {
        MAD_DBG_ERROR("Device Driver already loaded.\n");
        return MAD_API_ERR_DEV_ALREADY_EXIST;
    }

    if(fnRegister(dev,&(cfg->BSPFunctions)) != MAD_TRUE)
    {
       MAD_DBG_ERROR("madRegister Failed.\n");
       return MAD_API_ERR_REGISTERD_FUNC;
    }

    dev->baseRegAddr = (MAD_U8)cfg->smiBaseAddr;

    /* Initialize the driver    */
    retVal = madDriverConfig(dev);
    if(retVal != MAD_OK)
    {
        MAD_DBG_ERROR("Initialize driverConfig Failed.\n");
        return (retVal | MAD_API_FAIL_INIT_DRV_CFG);
    }

    /* Initialize dev fields.         */
    dev->devGroup = 0;
    dev->flags = 0;

    /* Get the number of ports */
    switch (dev->deviceId)
    {
        case MAD_88E10X0:   
        case MAD_88E10X0S:   
        case MAD_88E1011:   
                dev->numOfPorts = 1;
                break;

        case MAD_88E104X:   
                dev->numOfPorts = 4;
                break;
        case MAD_88E1116:
                dev->numOfPorts = 1;
                dev->flags = MAD_FLAG_SHORT_CABLE_FIX;  
                break;

        case MAD_88E3016:
/*        case MAD_88E3019: */
                dev->numOfPorts = 1;
                break;

        case MAD_88E3082:
                dev->numOfPorts = 8;
                break;

        case MAD_88E1121:
                dev->numOfPorts = 2;
                if (dev->revision > 1)
                  dev->flags = MAD_FLAG_SHORT_CABLE_FIX; 
              break;

        case MAD_88E1149:
        case MAD_88E1149R:
        case MAD_SWG65G : 
        case MAD_88E1240:
        case MAD_88E1340S:
        case MAD_88E1340:
        case MAD_88E1340M:
        case MAD_88E1540:
                dev->numOfPorts = 4;
                dev->flags = MAD_FLAG_SHORT_CABLE_FIX; 
              break;

        case MAD_88E1181:
                dev->numOfPorts = 8;
                if (dev->revision >= 4)
                {
                    dev->flags = MAD_FLAG_SHORT_CABLE_FIX;
                }
                break;

        case MAD_88E1111:
                dev->numOfPorts = 1;
            break;
        case MAD_88E1112:
                dev->numOfPorts = 1;
            break;
        case MAD_88E114X:
                dev->numOfPorts = 4;
            break;
        case MAD_88E1119R:
        case MAD_88E1310:
                dev->numOfPorts = 1;
                break;

        default:
                MAD_DBG_ERROR("Unknown Device(%#x). Initialization failed\n",dev->deviceId);
                dev = NULL;
                return MAD_API_UNKNOWN_DEV;
    }

    if(madFindPhyInformation(dev,&dev->phyInfo) != MAD_OK)
    {
        MAD_DBG_ERROR("Unknown Device Information(%#x). Initialization failed\n",dev->deviceId);
        return MAD_API_UNKNOWN_DEV;
    }

    retVal = madDevPreInit(dev);
    if(retVal != MAD_OK)
    {
        MAD_DBG_ERROR("device Pre Init Failed.\n");
        return (retVal | MAD_API_FAIL_PRE_INIT_DEV);
    }

    /* disable interrupt */
    intType.intGroup0 = 0;
    intType.intGroup1 = 0;

    for(port=0; port<dev->numOfPorts; port++)
    {
        if((retVal = mdIntSetEnable(dev,port,&intType)) != MAD_OK)
        {
            MAD_DBG_ERROR("mdIntSetEnable returned fail.\n");
            return (retVal | MAD_API_FAIL_SET_INT_ST);
        }
    }

    /* Initialize the MultiAddress Register Access semaphore.    */
    if((dev->multiAddrSem = madSemCreate(dev,MAD_SEM_FULL)) == 0)
    {
        MAD_DBG_ERROR("semCreate Failed.\n");
        return MAD_API_FAIL_SEM_CREATE;
    }

    dev->devEnabled = 1;

#ifdef MAD_SM_VCT
    madVCTDataInit(dev); 
#endif

#ifdef MAD_DSP_VCT
    madImplDSPVctInitData ();
#endif

    MAD_DBG_INFO("OK.\n");
    return MAD_OK;
}

/*******************************************************************************
* mdUnloadDriver
*
* DESCRIPTION:
*       This function frees semaphore created by Marvell Alaska Driver,
*       disables Device interrupt, and clears MAD_DEV structure.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK           - on success
*       MAD_FAIL         - on error
*
* COMMENTS:
*       1.  This function should be called only after successful execution of
*           madLoadDriver().
*
*******************************************************************************/
MAD_STATUS mdUnloadDriver
(
    IN MAD_DEV* dev
)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_INT_TYPE intType;

    MAD_DBG_INFO("madUnloadDriver Called.\n");

    /* disable interrupt */
    intType.intGroup0 = 0;
    intType.intGroup1 = 0;

    for(port=0; port<dev->numOfPorts; port++)
    {
        if((retVal = mdIntSetEnable(dev,port,&intType)) != MAD_OK)
        {
            MAD_DBG_ERROR("mdIntSetEnable returned fail.\n");
            return (retVal | MAD_API_FAIL_SET_INT_ST);
        }
    }

    /* Delete the MultiAddress mode reagister access semaphore.    */
    if(madSemDelete(dev,dev->multiAddrSem) != MAD_OK)
    {
        MAD_DBG_ERROR("SemDelete Failed.\n");
        return MAD_API_FAIL_SEM_DELETE;
    }
 
    madMemSet(dev,0,sizeof(MAD_DEV));
    return MAD_OK;
}


/*******************************************************************************
* fnRegister
*
* DESCRIPTION:
*       BSP should register the following functions:
*        1) MII Read - (Input, must provide)
*            allows driver to read device registers.
*        2) MII Write - (Input, must provice)
*            allows driver to write to device registers.
*        3) Semaphore Create - (Input, optional)
*            OS specific Semaphore Creat function.
*        4) Semaphore Delete - (Input, optional)
*            OS specific Semaphore Delete function.
*        5) Semaphore Take - (Input, optional)
*            OS specific Semaphore Take function.
*        6) Semaphore Give - (Input, optional)
*            OS specific Semaphore Give function.
*        Notes: 3) ~ 6) should be provided all or should not be provided at all.
*
* INPUTS:
*        pBSPFunctions - pointer to the structure for above functions.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MAD_TRUE, if input is valid. MAD_FALSE, otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
static MAD_BOOL fnRegister(MAD_DEV *dev, MAD_BSP_FUNCTIONS* pBSPFunctions)
{
    dev->fmadReadMii =  pBSPFunctions->readMii;
    dev->fmadWriteMii = pBSPFunctions->writeMii;
    
    dev->semCreate = pBSPFunctions->semCreate;
    dev->semDelete = pBSPFunctions->semDelete;
    dev->semTake   = pBSPFunctions->semTake;
    dev->semGive   = pBSPFunctions->semGive;
    
    return MAD_TRUE;
}


