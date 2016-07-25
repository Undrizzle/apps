#include <madCopyright.h>
/********************************************************************************
* madApiInit.c
*
* DESCRIPTION:
*       MAD API initialization routine
*
* DEPENDENCIES:   Platform
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "madSample.h"


MAD_DEV  madDevStruc;
MAD_DEV  *madDev=&madDevStruc;

static char * getDeviceName ( MAD_DEVICE_ID deviceId)
{

    switch (deviceId)
    {
        case MAD_88E10X0: return ("MAD_88E10X0 ");   
        case MAD_88E10X0S: return ("MAD_88E10X0S ");   
        case MAD_88E1011: return ("MAD_88E1011 ");   
        case MAD_88E104X: return ("MAD_88E104X ");
        case MAD_88E1111: return ("MAD_88E1111/MAD_88E1115 ");
        case MAD_88E1112: return ("MAD_88E1112 ");
        case MAD_88E1116: return ("MAD_88E1116/MAD_88E1116R ");
        case MAD_88E114X: return ("MAD_88E114X ");
        case MAD_88E1149: return ("MAD_88E1149 ");
        case MAD_88E1149R: return ("MAD_88E1149R ");
        case MAD_SWG65G : return ("MAD_SWG65G ");
        case MAD_88E1181: return ("MAD_88E1181 ");
        case MAD_88E3016: return ("MAD_88E3015/MAD_88E3016/MAD_88E3018/MAD_88E3019");
/*        case MAD_88E3019: return ("MAD_88E3019 "); */
        case MAD_88E1121: return ("MAD_88E1121/MAD_88E1121R ");
        case MAD_88E3082: return ("MAD_88E3082/MAD_88E3083 ");
        case MAD_88E1240: return ("MAD_88E1240 ");
        case MAD_88E1340S: return ("MAD_88E1340S ");
        case MAD_88E1340: return ("MAD_88E1340 ");
        case MAD_88E1340M: return ("MAD_88E1340M ");
        case MAD_88E1119R: return ("MAD_88E1119R ");
        case MAD_88E1310: return ("MAD_88E1310 ");
        case MAD_88E1540: return ("MAD_88E1540 ");
        default : return (" No-name ");
    }
} ;


/*
 *  Initialize the MAD. This should be done in BSP driver init routine.
 *    Since BSP is not combined with MAD, we are doing here.
*/

MAD_STATUS madSampleLoadDriver(MAD_DEV *dev, int smiAddr)
{
    MAD_SYS_CONFIG   cfg;
    MAD_STATUS status;

    /* clear structures */
    memset((char*)&cfg,0,sizeof(MAD_SYS_CONFIG));
    memset((char*)dev,0,sizeof(MAD_DEV));

    /*
     *  Register all the required functions to MAD.
    */

    cfg.BSPFunctions.readMii   = bspSpecificReadMii;
    cfg.BSPFunctions.writeMii  = bspSpecificWriteMii;
#ifdef USE_SEMAPHORE
    cfg.BSPFunctions.semCreate = madOsSemCreate;
    cfg.BSPFunctions.semDelete = madOsSemDelete;
    cfg.BSPFunctions.semTake   = madOsSemWait;
    cfg.BSPFunctions.semGive   = madOsSemSignal;
#else
    cfg.BSPFunctions.semCreate = NULL;
    cfg.BSPFunctions.semDelete = NULL;
    cfg.BSPFunctions.semTake   = NULL;
    cfg.BSPFunctions.semGive   = NULL;
#endif

    cfg.smiBaseAddr = smiAddr;  /* Set SMI Address */


    if((status=mdLoadDriver(&cfg, dev)) != MAD_OK)
    {
        MSG_PRINT(("madLoadDriver return Failed\n"));
        return status;
    }

    MSG_PRINT("Device Name   : %s\n", getDeviceName(dev->deviceId));
    MSG_PRINT("Device ID     : 0x%x\n",dev->deviceId);
    MSG_PRINT("Revision      : 0x%x\n",dev->revision);
    MSG_PRINT("Base Reg Addr : 0x%x\n",dev->baseRegAddr);
    MSG_PRINT("No of Ports   : %d\n",dev->numOfPorts);

    MSG_PRINT("MAD has been started.\n");

    return MAD_OK;
}


/*
 * This is the sample start routine.
 * This function will do the followings:
 *  1. Load MAD driver for the Marvell Phy mapped to the given SMI address.
 *  2. Enable all the ports in the Marvell Phy device.
 */

MAD_STATUS madStart(int smiAddr)
{
    MAD_U8 port;
    MAD_DEV *dev = madDev;

    /* load MAD Driver */
    if (madSampleLoadDriver(dev,smiAddr) != MAD_OK)
    {
        MSG_PRINT("Failed to load MAD Driver\n");
        return MAD_FAIL;
    }

    /* enable all 8 PHYs */
    for(port=0; port<dev->numOfPorts; port++)
    {
        if(mdSysSetPhyEnable(dev,port,MAD_TRUE) != MAD_OK)
        {
            MSG_PRINT("Failed to Enable Phy (port %i)\n",port);
            return MAD_FAIL;
        }
    }

    return MAD_OK;
}

MAD_STATUS mdMsecLoadDriver
(
    IN  MAD_SYS_CONFIG  *cfg,
    OUT MAD_DEV         *dev
);
