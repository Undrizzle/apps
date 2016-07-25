/*******************************************************************************
*                 Copyright 2006, MARVELL TECHNOLOGY GROUP LTD                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), GALILEO TECHNOLOGY LTD. (GTL) AND GALILEO TECHNOLOGY, INC. (GTI).    *
********************************************************************************
*
* madInit.c
*
* DESCRIPTION:
*       MAD initialization module
*
* DEPENDENCIES:   Platform
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "madSample.h"

#include "madSmi.h"

#define MAX_MAD_DEVICES        7

extern int madSmiRead (unsigned int smiPair, unsigned int smiAddr, 
                    unsigned int reg, unsigned int* value);
extern int madSmiWrite (unsigned int smiPair, unsigned int smiAddr, 
                    unsigned int reg, unsigned int value);

static MAD_DEV    maddev[MAX_MAD_DEVICES];
static MAD_U32        nMADDevices = 0;


static MAD_BOOL bspSpecificReadMii(MAD_DEV* dev, unsigned int smiAddr, 
               unsigned int reg, unsigned int* value)
{
int ret;
/* printf ("^^^^^^^^ bspSpecificReadMii smiPair %x smiAddr %x reg %x value %x\n", (unsigned int)dev->appData, smiAddr, reg, *value); */
         ret=madSmiRead((unsigned int)dev->appData, smiAddr, reg, value);
     if (ret)
        return MAD_TRUE;
     else
        return MAD_FALSE;
}
                                                                                
static MAD_BOOL bspSpecificWriteMii(MAD_DEV* dev, unsigned int smiAddr,
                      unsigned int reg, unsigned int value)
{
int ret;
        ret = madSmiWrite((unsigned int)dev->appData, smiAddr, reg, value);
     if (ret)
        return MAD_TRUE;
     else
        return MAD_FALSE;
}                                                                                



static char * madGetDeviceName ( MAD_DEVICE_ID deviceId)
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
        case MAD_88E3016: return ("MAD_88E3015/MAD_88E3016/MAD_88E3018/MAD_88E3019 ");
/*        case MAD_88E3019: return ("MAD_88E3019 "); */
        case MAD_88E1121: return ("MAD_88E1121/MAD_88E1121R ");
        case MAD_88E3082: return ("MAD_88E3082/MAD_88E3083 ");
        case MAD_88E1240: return ("MAD_88E1240 ");
        case MAD_88E1340S: return ("MAD_88E1340S ");
        case MAD_88E1340: return ("MAD_88E1340 ");
        case MAD_88E1340M: return ("MAD_88E1340M ");
        case MAD_88E1119R: return ("MAD_88E1119R ");
        case MAD_88E1310:  return ("MAD_88E1310 ");
        case MAD_88E1540: return ("MAD_88E1540 ");
        default : return (" No-name ");
    }
} ;

/*
 * The MAD device pointer.
 * A constant pointer to the one and only MAD device.
 */
MAD_DEV           *mad_dev = &maddev[0];
MAD_DEV           *mad_ext = &maddev[1];

static MAD_BOOL madSMIRead(MAD_DEV* dev, unsigned int smiAddr, 
              unsigned int reg, unsigned int* value)
{
    return (MAD_BOOL)madSmiRead((unsigned int)dev->appData, smiAddr, reg, value);
}

static MAD_BOOL madSMIWrite(MAD_DEV* dev, unsigned int smiAddr, 
              unsigned int reg, unsigned int value)
{
    return (MAD_BOOL)madSmiWrite((unsigned int)dev->appData, smiAddr, reg, value);
}

static MAD_STATUS madStart(MAD_DEV* dev,  int smiPair)
{
    MAD_STATUS status = MAD_FAIL;
    MAD_SYS_CONFIG   cfg;
#ifdef MAD_HEADER_MODE
    int    i;
#endif

    memset((char*)&cfg,0,sizeof(MAD_SYS_CONFIG));
    memset((char*)dev,0,sizeof(MAD_DEV));

    cfg.BSPFunctions.readMii   = bspSpecificReadMii;
    cfg.BSPFunctions.writeMii  = bspSpecificWriteMii;
#ifdef USE_SEMAPHORE
    cfg.BSPFunctions.semCreate = osSemCreate;
    cfg.BSPFunctions.semDelete = osSemDelete;
    cfg.BSPFunctions.semTake   = osSemWait;
    cfg.BSPFunctions.semGive   = osSemSignal;
#else
    cfg.BSPFunctions.semCreate = NULL;
    cfg.BSPFunctions.semDelete = NULL;
    cfg.BSPFunctions.semTake   = NULL;
    cfg.BSPFunctions.semGive   = NULL;
#endif

    dev->appData = smiPair;
    cfg.smiBaseAddr = 0;  /* Set SMI Address */

    if((status=mdLoadDriver(&cfg, dev)) != MAD_OK)
    {
        return status;
    }

    MSG_PRINT("Device Name   : %s\n", madGetDeviceName(dev->deviceId));
    MSG_PRINT("Device ID     : 0x%x\n",dev->deviceId);
    MSG_PRINT("Revision      : 0x%x\n",dev->revision);
    MSG_PRINT("Base Reg Addr : 0x%x\n",dev->baseRegAddr);
    MSG_PRINT("No of Ports   : %d\n",dev->numOfPorts);
    MSG_PRINT("Smi Pair         : %d\n",dev->appData);

    MSG_PRINT("MAD has been started.\n");

    return MAD_OK;
}

static void madClose(void) 
{
    if (mad_dev->devEnabled)
        mdUnloadDriver(mad_dev);
}


MAD_STATUS madInit(int smi)
{
  MAD_U32        smiPair, nDevices;
  MAD_STATUS    status;

  madSmiInit();

  nDevices = 0;

  if (smi==-1)
  {
    for(smiPair=0; smiPair<MAX_MDC_MDIO_PAIR; smiPair++)
    {
      status = madStart(&maddev[nDevices], smiPair);
      if (MAD_OK != status)
      {
         status = madStart(&maddev[nDevices],smiPair);
        if (MAD_OK != status)
        {
            continue;
        }
      }

      nDevices++;
      if (nDevices >= MAX_MAD_DEVICES)
        break;
    }
  }
  else
  {
    status = madStart(&maddev[nDevices], smi);
    if (MAD_OK != status)
    {
        nDevices=0;
           MSG_PRINT("No Device %d found\n", smi);
    }
    else
    {
        nDevices=1;
           MSG_PRINT("!!! Device %d found\n", smi);
    }
  }

  nMADDevices = nDevices;

  if (nDevices == 0)
  {
    return MAD_FAIL;
  }
  

  return MAD_OK;
}


int madExist(void)
{
    return nMADDevices;
}
