#include <Copyright.h>

/********************************************************************************
* gtSysConfig.c
*
* DESCRIPTION:
*       API definitions for system configuration, and enabling.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

#include <msApi.h>
#include <msApiPrototype.h>
#include <gtDrvConfig.h>
#include <gtSem.h>
#include <platformDeps.h>
#ifdef GT_USE_MAD
#include <madApi.h>
#include <madApiDefs.h>
#endif
#include <gtHwCntl.h>



static GT_BOOL gtRegister(GT_QD_DEV *qd_dev, BSP_FUNCTIONS* pBSPFunctions);

#ifdef GT_USE_MAD
static MAD_BOOL madSMIRead(MAD_DEV* dev, unsigned int smiAddr, 
              unsigned int reg, unsigned int* value)
{
  GT_STATUS  status;
  GT_U16 data;

  status = 	hwReadPhyReg((GT_QD_DEV *)(dev->appData), smiAddr, reg, &data);

  if(status == GT_OK)
  {
	*value = data;
	return MAD_TRUE;
  }
  else
	return MAD_FALSE;
}

static MAD_BOOL madSMIWrite(MAD_DEV* dev, unsigned int smiAddr, 
              unsigned int reg, unsigned int value)
{
  GT_STATUS  status;
  GT_U16 data;

  data = value;
  status = 	hwWritePhyReg((GT_QD_DEV *)(dev->appData), smiAddr, reg, data);

  if(status == GT_OK)
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
        case MAD_88E1310: 
            return ("MAD_88E1310 ");
        default : return (" No-name ");
    }
} ;


static MAD_STATUS madStart(GT_QD_DEV* qd_dev,  int smiPair)
{
    MAD_STATUS status = MAD_FAIL;
    MAD_DEV* dev = (MAD_DEV*)&(qd_dev->mad_dev);
    MAD_SYS_CONFIG   cfg;


    cfg.BSPFunctions.readMii   = (FMAD_READ_MII )madSMIRead;
    cfg.BSPFunctions.writeMii  = (FMAD_WRITE_MII )madSMIWrite;
    cfg.BSPFunctions.semCreate = NULL;
    cfg.BSPFunctions.semDelete = NULL;
    cfg.BSPFunctions.semTake   = NULL;
    cfg.BSPFunctions.semGive   = NULL;

    dev->appData = (void *)qd_dev;
    cfg.smiBaseAddr = 0;  /* Set SMI Address */

    if((status=mdLoadDriver(&cfg, dev)) != MAD_OK)
    {
        return status;
    }

    DBG_INFO(("Device Name   : %s\n", madGetDeviceName(dev->deviceId)));
    DBG_INFO(("Device ID     : 0x%x\n",dev->deviceId));
    DBG_INFO(("Revision      : 0x%x\n",dev->revision));
    DBG_INFO(("Base Reg Addr : 0x%x\n",dev->baseRegAddr));
    DBG_INFO(("No of Ports   : %d\n",dev->numOfPorts));
    DBG_INFO(("QD dev        : %x\n",dev->appData));

    DBG_INFO(("MAD has been started.\n"));

#ifdef KERNEL_DBG
#else
    DBG_INFO(("Device Name   : %s\n", madGetDeviceName(dev->deviceId)));
    DBG_INFO(("Device ID     : 0x%x\n",dev->deviceId));
    DBG_INFO(("Revision      : 0x%x\n",dev->revision));
    DBG_INFO(("Base Reg Addr : 0x%x\n",dev->baseRegAddr));
    DBG_INFO(("No of Ports   : %d\n",dev->numOfPorts));
    DBG_INFO(("QD dev        : %x\n",dev->appData));

    DBG_INFO(("MAD has been started.\n"));
#endif
	qd_dev->use_mad = GT_TRUE;
    return MAD_OK;
}

/*
static void madClose(MAD_DEV* dev) 
{
    if (dev->devEnabled)
        mdUnloadDriver(dev);
}
*/

 GT_STATUS qd_madInit(GT_QD_DEV    *dev)
{
  MAD_STATUS    status;

  status = madStart(dev, 0);
  if (MAD_OK != status)
  {
        DBG_INFO(("sMAD Initialization Failed.\n"));
        qdUnloadDriver(dev);
        return GT_FAIL;
  }

  return GT_OK;
}

#endif /* GT_USE_MAD */

/*******************************************************************************
* qdLoadDriver
*
* DESCRIPTION:
*       QuarterDeck Driver Initialization Routine. 
*       This is the first routine that needs be called by system software. 
*       It takes *cfg from system software, and retures a pointer (*dev) 
*       to a data structure which includes infomation related to this QuarterDeck
*       device. This pointer (*dev) is then used for all the API functions. 
*
* INPUTS:
*       cfg  - Holds device configuration parameters provided by system software.
*
* OUTPUTS:
*       dev  - Holds device information to be used for each API call.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_ALREADY_EXIST    - if device already started
*       GT_BAD_PARAM        - on bad parameters
*
* COMMENTS:
*     qdUnloadDriver is also provided to do driver cleanup.
*
*******************************************************************************/
GT_STATUS qdLoadDriver
(
    IN  GT_SYS_CONFIG   *cfg,
    OUT GT_QD_DEV    *dev
)
{
    GT_STATUS   retVal;
    GT_LPORT    port;

    DBG_INFO(("qdLoadDriver Called.\n"));
    printf(("qdLoadDriver Called.\n"));

    /* Check for parameters validity        */
    if(dev == NULL)
    {
        DBG_INFO(("Failed.\n"));
	 printf(("Failed(211).\n"));
        return GT_BAD_PARAM;
    }

    /* Check for parameters validity        */
    if(cfg == NULL)
    {
        DBG_INFO(("Failed.\n"));
	 printf(("Failed(219).\n"));
        return GT_BAD_PARAM;
    }

    /* The initialization was already done. */
    if(dev->devEnabled)
    {
        DBG_INFO(("QuarterDeck already started.\n"));
	 printf(("Failed(227).\n"));
        return GT_ALREADY_EXIST;
    }

    if(gtRegister(dev,&(cfg->BSPFunctions)) != GT_TRUE)
    {
       DBG_INFO(("gtRegister Failed.\n"));
	printf(("Failed(234).\n"));
       return GT_FAIL;
    }
    dev->accessMode = (GT_U8)cfg->mode.scanMode;
    if (dev->accessMode == SMI_MULTI_ADDR_MODE)
    {
        dev->baseRegAddr = 0;
        dev->phyAddr = (GT_U8)cfg->mode.baseAddr;
    }
    else
    {
        dev->baseRegAddr = (GT_U8)cfg->mode.baseAddr;
        dev->phyAddr = 0;
    }


    /* Initialize the driver    */
    retVal = driverConfig(dev);
    if(retVal != GT_OK)
    {
        DBG_INFO(("driverConfig Failed.\n"));
	 printf(("Failed(255).\n"));
        return retVal;
    }

    /* Initialize dev fields.         */
    dev->cpuPortNum = cfg->cpuPortNum;
    dev->maxPhyNum = 5;
    dev->devGroup = 0;
    dev->devStorage = 0;
    /* Assign Device Name */
    dev->devName = 0;
    dev->devName1 = 0;

    if((dev->deviceId&0xfff8)==GT_88EC000) /* device id 0xc00 - 0xc07 are GT_88EC0XX */
      dev->deviceId=GT_88EC000;

    switch(dev->deviceId)
    {
        case GT_88E6021:
                dev->numOfPorts = 3;
                dev->maxPorts = 3;
                dev->maxPhyNum = 2;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6021;
                break;

        case GT_88E6051:
                dev->numOfPorts = 5;
                dev->maxPorts = 5;
                dev->maxPhyNum = 5;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6051;
                break;

        case GT_88E6052:
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 5;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6052;
                break;

        case GT_88E6060:
                if((dev->cpuPortNum != 4)&&(dev->cpuPortNum != 5))
                {
                    return GT_FAIL;
                }
                dev->numOfPorts = 6;
                dev->maxPorts = 6;
                dev->maxPhyNum = 5;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6060;
                break;

        case GT_88E6031:
                dev->numOfPorts = 3;
                dev->maxPorts = 6;
                dev->maxPhyNum = 3;
                dev->validPortVec = 0x31;    /* port 0, 4, and 5 */
                dev->validPhyVec = 0x31;    /* port 0, 4, and 5 */
                dev->devName = DEV_88E6061;
                break;

        case GT_88E6061:
                dev->numOfPorts = 6;
                dev->maxPorts = 6;
                dev->maxPhyNum = 6;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6061;
                break;

        case GT_88E6035:
                dev->numOfPorts = 3;
                dev->maxPorts = 6;
                dev->maxPhyNum = 3;
                dev->validPortVec = 0x31;    /* port 0, 4, and 5 */
                dev->validPhyVec = 0x31;    /* port 0, 4, and 5 */
                dev->devName = DEV_88E6065;
                break;

        case GT_88E6055:
                dev->numOfPorts = 5;
                dev->maxPorts = 6;
                dev->maxPhyNum = 5;
                dev->validPortVec = 0x2F;    /* port 0,1,2,3, and 5 */
                dev->validPhyVec = 0x2F;    /* port 0,1,2,3, and 5 */
                dev->devName = DEV_88E6065;
                break;

        case GT_88E6065:
                dev->numOfPorts = 6;
                dev->maxPorts = 6;
                dev->maxPhyNum = 6;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6065;
                break;

        case GT_88E6063:
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 5;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6063;
                break;

        case GT_FH_VPN:
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 5;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_FH_VPN;
                break;

        case GT_FF_EG:
                if(dev->cpuPortNum != 5)
                {
                    return GT_FAIL;
                }
                dev->numOfPorts = 6;
                dev->maxPorts = 6;
                dev->maxPhyNum = 5;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_FF_EG;
                break;

        case GT_FF_HG:
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 5;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_FF_HG;
                break;

        case GT_88E6083:
                dev->numOfPorts = 10;
                dev->maxPorts = 10;
                dev->maxPhyNum = 8;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6083;
                break;

        case GT_88E6153:
                dev->numOfPorts = 6;
                dev->maxPorts = 6;
                dev->maxPhyNum = 6;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6183;
                break;

        case GT_88E6181:
                dev->numOfPorts = 8;
                dev->maxPorts = 8;
                dev->maxPhyNum = 8;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6181;
                break;

        case GT_88E6183:
                dev->numOfPorts = 10;
                dev->maxPorts = 10;
                dev->maxPhyNum = 10;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6183;
                break;

        case GT_88E6093:
                dev->numOfPorts = 11;
                dev->maxPorts = 11;
                dev->maxPhyNum = 11;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6093;
                break;

        case GT_88E6092:
                dev->numOfPorts = 11;
                dev->maxPorts = 11;
                dev->maxPhyNum = 11;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6092;
                break;

        case GT_88E6095:
                dev->numOfPorts = 11;
                dev->maxPorts = 11;
                dev->maxPhyNum = 11;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6095;
                break;

        case GT_88E6045:
                dev->numOfPorts = 6;
                dev->maxPorts = 11;
                dev->maxPhyNum = 11;
                dev->validPortVec = 0x60F;
                dev->validPhyVec = 0x60F;
                dev->devName = DEV_88E6095;
                break;

        case GT_88E6097:
                dev->numOfPorts = 11;
                dev->maxPorts = 11;
                dev->maxPhyNum = 11;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6097;
                break;

        case GT_88E6096:
                dev->numOfPorts = 11;
                dev->maxPorts = 11;
                dev->maxPhyNum = 11;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6096;
                break;

        case GT_88E6047:
                dev->numOfPorts = 6;
                dev->maxPorts = 11;
                dev->maxPhyNum = 11;
                dev->validPortVec = 0x60F;
                dev->validPhyVec = 0x60F;
                dev->devName = DEV_88E6097;
                break;

        case GT_88E6046:
                dev->numOfPorts = 6;
                dev->maxPorts = 11;
                dev->maxPhyNum = 11;
                dev->validPortVec = 0x60F;
                dev->validPhyVec = 0x60F;
                dev->devName = DEV_88E6096;
                break;

        case GT_88E6085:
                dev->numOfPorts = 10;
                dev->maxPorts = 11;
                dev->maxPhyNum = 11;
                dev->validPortVec = 0x6FF;
                dev->validPhyVec = 0x6FF;
                dev->devName = DEV_88E6096;
                break;

        case GT_88E6152:
                dev->numOfPorts = 6;
                dev->maxPorts = 6;
                dev->maxPhyNum = 6;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6182;
                break;

        case GT_88E6155:
                dev->numOfPorts = 6;
                dev->maxPorts = 6;
                dev->maxPhyNum = 6;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6185;
                break;

        case GT_88E6182:
                dev->numOfPorts = 10;
                dev->maxPorts = 10;
                dev->maxPhyNum = 10;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6182;
                break;

        case GT_88E6185:
                dev->numOfPorts = 10;
                dev->maxPorts = 10;
                dev->maxPhyNum = 10;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->devName = DEV_88E6185;
                break;

        case GT_88E6121:
                dev->numOfPorts = 3;
                dev->maxPorts = 8;
                dev->maxPhyNum = 3;
                dev->validPortVec = 0xE;    /* port 1, 2, and 3 */
                dev->validPhyVec = 0xE;        /* port 1, 2, and 3 */
                dev->devName = DEV_88E6108;
                break;

        case GT_88E6122:
                dev->numOfPorts = 6;
                dev->maxPorts = 8;
                dev->maxPhyNum = 16;
                dev->validPortVec = 0x7E;    /* port 1 ~ 6 */
                dev->validPhyVec = 0xF07E;    /* port 1 ~ 6, 12 ~ 15 (serdes) */
                dev->validSerdesVec = 0xF000;
                dev->devName = DEV_88E6108;
                break;

        case GT_88E6131:
        case GT_88E6108:
                dev->numOfPorts = 8;
                dev->maxPorts = 8;
                dev->maxPhyNum = 16;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = (1 << dev->maxPhyNum) - 1;
                dev->validSerdesVec = 0xF000;
                dev->devName = DEV_88E6108;
                break;

        case GT_88E6123:
                dev->numOfPorts = 3;
                dev->maxPorts = 6;
                dev->maxPhyNum = 14;
                dev->validPortVec = 0x23;
                dev->validPhyVec = 0x303F;
                dev->validSerdesVec = 0x3000;
                dev->devName = DEV_88E6161;
                break;

        case GT_88E6140:
                dev->numOfPorts = 6;
                dev->maxPorts = 6;
                dev->maxPhyNum = 14;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = 0x303F;
                dev->validSerdesVec = 0x3000;
                dev->devName = DEV_88E6165;
                break;

        case GT_88E6161:
                dev->numOfPorts = 6;
                dev->maxPorts = 6;
                dev->maxPhyNum = 14;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = 0x303F;
                dev->validSerdesVec = 0x3000;
                dev->devName = DEV_88E6161;
                break;

        case GT_88E6165:
                dev->numOfPorts = 6;
                dev->maxPorts = 6;
                dev->maxPhyNum = 14;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = 0x303F;
                dev->validSerdesVec = 0x3000;
                dev->devName = DEV_88E6165;
                break;

        case GT_88E6351:
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 7;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = 0x7F;
                dev->devName = DEV_88E6351;
                break;

        case GT_88E6175:
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 7;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = 0x7F;
                dev->devName1 = DEV_88E6175; /* test device group 1 */
                break;

        case GT_88E6125 :
                dev->numOfPorts = 4;
                dev->maxPorts = 7;
                dev->maxPhyNum = 7;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPortVec &= ~(0x7);
                dev->validPhyVec = 0x78;
                dev->devName = DEV_88E6171;
                break;

        case GT_88E6171 :
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 7;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = 0x7F;
                dev->devName = DEV_88E6171;
                break;

        case GT_88E6321 :
                dev->numOfPorts = 4;
                dev->maxPorts = 7;
                dev->maxPhyNum = 7;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPortVec &= ~(0x7);
                dev->validPhyVec = 0x78;
                dev->devName = DEV_88E6371;
                break;

        case GT_88E6350 :
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 7;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = 0x7F;
                dev->devName = DEV_88E6371;
                break;

        case GT_88EC000 :
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 7;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = 0x7F;
                dev->devName1 = DEV_88EC000;
                break;
        case GT_88E3020:
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 7;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = 0x7F;
                dev->devName1 = DEV_88E3020;
                break;
        case GT_88E6020:
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 7;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = 0x7F;
                dev->devName1 = DEV_88E6020;
                break;
        case GT_88E6070:
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 7;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = 0x7F;
                dev->devName1 = DEV_88E6070;
                break;
        case GT_88E6071:
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 7;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = 0x7F;
                dev->devName1 = DEV_88E6071;
                break;
        case GT_88E6220:
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 7;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = 0x7F;
                dev->devName1 = DEV_88E6220;
                break;
        case GT_88E6250:
                dev->numOfPorts = 7;
                dev->maxPorts = 7;
                dev->maxPhyNum = 7;
                dev->validPortVec = (1 << dev->numOfPorts) - 1;
                dev->validPhyVec = 0x7F;
                dev->devName1 = DEV_88E6250;
                break;

        default:
                DBG_INFO(("Unknown Device. Initialization failed\n"));
                return GT_FAIL;
    }

    dev->cpuPortNum = GT_PORT_2_LPORT(cfg->cpuPortNum);

    if(dev->cpuPortNum == GT_INVALID_PORT)
    {
        if(GT_LPORT_2_PORT((GT_LPORT)cfg->cpuPortNum) != GT_INVALID_PORT)
        {
            dev->cpuPortNum = cfg->cpuPortNum;
        }
        else
        {
            return GT_BAD_CPU_PORT;
        }
    }

    /* Initialize the MultiAddress Register Access semaphore.    */
    if((dev->multiAddrSem = gtSemCreate(dev,GT_SEM_FULL)) == 0)
    {
        DBG_INFO(("semCreate Failed.\n"));
        qdUnloadDriver(dev);
        return GT_FAIL;
    }

    /* Initialize the ATU semaphore.    */
    if((dev->atuRegsSem = gtSemCreate(dev,GT_SEM_FULL)) == 0)
    {
        DBG_INFO(("semCreate Failed.\n"));
        qdUnloadDriver(dev);
        return GT_FAIL;
    }

    /* Initialize the VTU semaphore.    */
    if((dev->vtuRegsSem = gtSemCreate(dev,GT_SEM_FULL)) == 0)
    {
        DBG_INFO(("semCreate Failed.\n"));
        qdUnloadDriver(dev);
        return GT_FAIL;
    }

    /* Initialize the STATS semaphore.    */
    if((dev->statsRegsSem = gtSemCreate(dev,GT_SEM_FULL)) == 0)
    {
        DBG_INFO(("semCreate Failed.\n"));
        qdUnloadDriver(dev);
        return GT_FAIL;
    }

    /* Initialize the PIRL semaphore.    */
    if((dev->pirlRegsSem = gtSemCreate(dev,GT_SEM_FULL)) == 0)
    {
        DBG_INFO(("semCreate Failed.\n"));
        qdUnloadDriver(dev);
        return GT_FAIL;
    }

    /* Initialize the PTP semaphore.    */
    if((dev->ptpRegsSem = gtSemCreate(dev,GT_SEM_FULL)) == 0)
    {
        DBG_INFO(("semCreate Failed.\n"));
        qdUnloadDriver(dev);
        return GT_FAIL;
    }

    /* Initialize the Table semaphore.    */
    if((dev->tblRegsSem = gtSemCreate(dev,GT_SEM_FULL)) == 0)
    {
        DBG_INFO(("semCreate Failed.\n"));
        qdUnloadDriver(dev);
        return GT_FAIL;
    }

    /* Initialize the EEPROM Configuration semaphore.    */
    if((dev->eepromRegsSem = gtSemCreate(dev,GT_SEM_FULL)) == 0)
    {
        DBG_INFO(("semCreate Failed.\n"));
        qdUnloadDriver(dev);
        return GT_FAIL;
    }

    /* Initialize the PHY Device Register Access semaphore.    */
    if((dev->phyRegsSem = gtSemCreate(dev,GT_SEM_FULL)) == 0)
    {
        DBG_INFO(("semCreate Failed.\n"));
        qdUnloadDriver(dev);
        return GT_FAIL;
    }

    /* Initialize the ports states to forwarding mode. */
    if(cfg->initPorts == GT_TRUE)
    {
        for (port=0; port<dev->numOfPorts; port++)
        {
            if((retVal = gstpSetPortState(dev,port,GT_PORT_FORWARDING)) != GT_OK)
               {
                DBG_INFO(("Failed.\n"));
                qdUnloadDriver(dev);
                   return retVal;
            }
        }
    }

    dev->use_mad = GT_FALSE;
#ifdef GT_USE_MAD
    qd_madInit(dev);
#endif

    if(cfg->skipInitSetup == GT_SKIP_INIT_SETUP)
    {
        dev->devEnabled = 1;
        dev->devNum = cfg->devNum;

        DBG_INFO(("OK.\n"));
        return GT_OK;
    }

    if(IS_IN_DEV_GROUP(dev,DEV_ENHANCED_CPU_PORT))
    {
        if((retVal = gsysSetRsvd2CpuEnables(dev,0)) != GT_OK)
        {
            DBG_INFO(("gsysGetRsvd2CpuEnables failed.\n"));
            qdUnloadDriver(dev);
            return retVal;
        }

        if((retVal = gsysSetRsvd2Cpu(dev,GT_FALSE)) != GT_OK)
        {
            DBG_INFO(("gsysSetRsvd2Cpu failed.\n"));
            qdUnloadDriver(dev);
            return retVal;
        }
    }

    if (IS_IN_DEV_GROUP(dev,DEV_CPU_DEST_PER_PORT))
    {
        for (port=0; port<dev->numOfPorts; port++)
        {
            retVal = gprtSetCPUPort(dev,port,dev->cpuPortNum);
            if(retVal != GT_OK)
            {
                DBG_INFO(("Failed.\n"));
                qdUnloadDriver(dev);
                   return retVal;
            }
        }
    }

    if(IS_IN_DEV_GROUP(dev,DEV_CPU_PORT))
    {
        retVal = gsysSetCPUPort(dev,dev->cpuPortNum);
        if(retVal != GT_OK)
           {
            DBG_INFO(("Failed.\n"));
            qdUnloadDriver(dev);
               return retVal;
        }
    }

    if(IS_IN_DEV_GROUP(dev,DEV_CPU_DEST))
    {
        retVal = gsysSetCPUDest(dev,dev->cpuPortNum);
        if(retVal != GT_OK)
           {
            DBG_INFO(("Failed.\n"));
            qdUnloadDriver(dev);
               return retVal;
        }
    }

    if(IS_IN_DEV_GROUP(dev,DEV_MULTICAST))
    {
        if((retVal = gsysSetRsvd2Cpu(dev,GT_FALSE)) != GT_OK)
        {
            DBG_INFO(("gsysSetRsvd2Cpu failed.\n"));
            qdUnloadDriver(dev);
            return retVal;
        }
    }

    if (IS_IN_DEV_GROUP(dev,DEV_PIRL_RESOURCE))
    {
        retVal = gpirlInitialize(dev);
        if(retVal != GT_OK)
           {
            DBG_INFO(("Failed.\n"));
            qdUnloadDriver(dev);
               return retVal;
        }
    }

    if (IS_IN_DEV_GROUP(dev,DEV_PIRL2_RESOURCE))
    {
        retVal = gpirl2Initialize(dev);
        if(retVal != GT_OK)
           {
            DBG_INFO(("Failed.\n"));
            qdUnloadDriver(dev);
               return retVal;
        }
    }

    dev->devEnabled = 1;
    dev->devNum = cfg->devNum;

    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* sysEnable
*
* DESCRIPTION:
*       This function enables the system for full operation.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       1.  This function should be called only after successful execution of
*           qdLoadDriver().
*
*******************************************************************************/
GT_STATUS sysEnable( GT_QD_DEV *dev)
{
    DBG_INFO(("sysEnable Called.\n"));
    DBG_INFO(("OK.\n"));
    return driverEnable(dev);
}


/*******************************************************************************
* qdUnloadDriver
*
* DESCRIPTION:
*       This function unloads the QuaterDeck Driver.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       1.  This function should be called only after successful execution of
*           qdLoadDriver().
*
*******************************************************************************/
GT_STATUS qdUnloadDriver
(
    IN GT_QD_DEV* dev
)
{
    DBG_INFO(("qdUnloadDriver Called.\n"));

    /* Delete the MultiAddress mode reagister access semaphore.    */
    if(gtSemDelete(dev,dev->multiAddrSem) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }
 
    /* Delete the ATU semaphore.    */
    if(gtSemDelete(dev,dev->atuRegsSem) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    /* Delete the VTU semaphore.    */
    if(gtSemDelete(dev,dev->vtuRegsSem) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    /* Delete the STATS semaphore.    */
    if(gtSemDelete(dev,dev->statsRegsSem) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    /* Delete the PIRL semaphore.    */
    if(gtSemDelete(dev,dev->pirlRegsSem) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    /* Delete the PTP semaphore.    */
    if(gtSemDelete(dev,dev->ptpRegsSem) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    /* Delete the Table semaphore.    */
    if(gtSemDelete(dev,dev->tblRegsSem) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    /* Delete the EEPROM Configuration semaphore.    */
    if(gtSemDelete(dev,dev->eepromRegsSem) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    /* Delete the PHY Device semaphore.    */
    if(gtSemDelete(dev,dev->phyRegsSem) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    gtMemSet(dev,0,sizeof(GT_QD_DEV));
    return GT_OK;
}


/*******************************************************************************
* gtRegister
*
* DESCRIPTION:
*       BSP should register the following functions:
*        1) MII Read - (Input, must provide)
*            allows QuarterDeck driver to read QuarterDeck device registers.
*        2) MII Write - (Input, must provice)
*            allows QuarterDeck driver to write QuarterDeck device registers.
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
*       GT_TRUE, if input is valid. GT_FALSE, otherwise.
*
* COMMENTS:
*       This function should be called only once.
*
*******************************************************************************/
static GT_BOOL gtRegister(GT_QD_DEV *dev, BSP_FUNCTIONS* pBSPFunctions)
{
    dev->fgtReadMii =  pBSPFunctions->readMii;
    dev->fgtWriteMii = pBSPFunctions->writeMii;
#ifdef GT_RMGMT_ACCESS
    dev->fgtHwAccessMod =  pBSPFunctions->hwAccessMod;
    dev->fgtHwAccess = pBSPFunctions->hwAccess;
#endif
	
    dev->semCreate = pBSPFunctions->semCreate;
    dev->semDelete = pBSPFunctions->semDelete;
    dev->semTake   = pBSPFunctions->semTake  ;
    dev->semGive   = pBSPFunctions->semGive  ;
    
    return GT_TRUE;
}


