#include <madCopyright.h>
/********************************************************************************
* madMacIfLoop.c
*
* DESCRIPTION:
*       MAC Interface Loopback setup sample (startMACIfLoopback, stopMACIfLoopback).
*       Since MAC Interface Loopback setup(mdDiagSetMACIfLoopback API) overwrites
*       MDI Crossover mode and SGMII Auto-Neg mode, the APIs, mdCopperSetMDIMode
*       and mdSGMIISetAutoNeg, are called after loopback setup is disabled.
*
* DEPENDENCIES:
*       MAD Driver should be loaded and Phys should be enabled
*       before running sample test functions as specified in
*       sample\Initialization\madApiInit.c.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "madSample.h"


/* 
    Assumes that the followings are the original setup data for MDI Crossover 
    and SGMII Auto-Neg mode.
    These values are used when Loopback setup is disabled.
    Please notes that these values can be retrieved from PHY with the APIs,
    mdCopperGetMDIMode and mdSGMIISetAutoNeg, as shown in startMACIfLoopback
    function.
*/
static MAD_MDI_MODE orgMdiMode = MAD_AUTO_MDI_MDIX;
static MAD_BOOL     orgSGMIIState = MAD_TRUE;
static MAD_SPEED_MODE   orgSGMIISpeed = MAD_SPEED_1000M;


MAD_STATUS startMACIfLoopback(MAD_DEV *dev,MAD_LPORT port,MAD_MAC_LOOPBACK_MODE mode)
{
    MAD_STATUS status;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_FAIL;
    }

    MSG_PRINT("MAC Interface Loopback for port %i with mode %i\n",(int)port,mode);

    /*
     *	Retrieve MDI Crossover mode
    */
    if((status = mdCopperGetMDIMode(dev,port,&orgMdiMode)) != MAD_OK)
    {
       MSG_PRINT("mdCopperGetMDIMode return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    /*
     *	Retrieve SGMII Auto-Neg mode
    */
	if ((dev->deviceId!=MAD_88E1116)&&(dev->deviceId!=MAD_88E3016)
		&&(dev->deviceId!=MAD_88E3082)&&(dev->deviceId!=MAD_88E1121) 
		&&(dev->deviceId!=MAD_88E1119R)&&(dev->deviceId!=MAD_88E1310)) 
		/* E1116/E3016/E3082/E1121 has no SGMII */
	{
		if((status = mdSGMIIGetAutoNeg(dev,port,&orgSGMIIState,&orgSGMIISpeed)) != MAD_OK)
		{
			MSG_PRINT("mdSGMIIGetAutoNeg return Failed\n");
		    testMADDisplayStatus(status);
			return status;
		}
	}
	else
	{
		if((status = mdCopperGetAutoNeg(dev,port,&orgSGMIIState,&orgSGMIISpeed)) != MAD_OK)
		{
			MSG_PRINT("mdCopperGetAutoNeg return Failed\n");
		    testMADDisplayStatus(status);
			return status;
		}
	}

    /*
     *	Start MAC Interface Loopback
    */
	if((status = mdDiagSetMACIfLoopback(dev,port,MAD_TRUE,mode, MAD_SPEED_100M)) != MAD_OK)
    {
        MSG_PRINT("mdDiagSetMACIfLoopback return Failed\n");
        if (mode != MAD_MAC_LOOPBACK_MODE_0)
            MSG_PRINT("Please check if link is established on port %i\n",(int)port);
        testMADDisplayStatus(status);

        return status;
    }

    return MAD_OK;
}


MAD_STATUS stopMACIfLoopback(MAD_DEV *dev,MAD_LPORT port,MAD_MAC_LOOPBACK_MODE mode)
{
    MAD_STATUS status;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_FAIL;
    }

    MSG_PRINT("Stopping MAC Interface Loopback for port %i\n",(int)port);

    /*
     *	Start MAC Interface Loopback
    */
    if((status = mdDiagSetMACIfLoopback(dev,port,MAD_FALSE,mode, MAD_SPEED_100M)) != MAD_OK)
    {
        MSG_PRINT("mdDiagSetMACIfLoopback return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    /*
     *	Reconfigure MDI Crossover mode
    */
    if((status = mdCopperSetMDIMode(dev,port,orgMdiMode)) != MAD_OK)
    {
        MSG_PRINT("mdCopperSetMDIMode return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    /*
     *	Reconfigure SGMII Auto-Neg mode
    */
	if ((dev->deviceId!=MAD_88E1116)&&(dev->deviceId!=MAD_88E3016)
		&&(dev->deviceId!=MAD_88E3082)&&(dev->deviceId!=MAD_88E1121) 
		&&(dev->deviceId!=MAD_88E1119R)&&(dev->deviceId!=MAD_88E1310)) 
		/* E1116/E3016/E1121 has RGMII only */
	{
		if((status = mdSGMIISetAutoNeg(dev,port,orgSGMIIState,orgSGMIISpeed)) != MAD_OK)
		{
			MSG_PRINT("mdSGMIISetAutoNeg return Failed\n");
		    testMADDisplayStatus(status);
			return status;
		}
	}
	else
	{
		if((status = mdCopperSetAutoNeg(dev,port,orgSGMIIState,orgSGMIISpeed)) != MAD_OK)
		{
			MSG_PRINT("mdCopperSetAutoNeg return Failed\n");
		    testMADDisplayStatus(status);
			return status;
		}
	}


    return MAD_OK;
}


