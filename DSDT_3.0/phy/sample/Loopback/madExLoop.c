#include <madCopyright.h>
/********************************************************************************
* madExLoop.c
*
* DESCRIPTION:
*       External Loopback setup sample (startExLoopback, stopExLoopback).
*       Since External Loopback setup(mdDiagSetExternalLoopback API) overwrites
*       Copper Auto-Neg mode, mdDiagSetExternalLoopback API should be called after 
*       loopback test is stopped.
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
    Assumes that the following is the original setup data for Copper Auto-Neg
    mode.
    This value is used when Loopback setup is disabled.
    Please notes that this value can be retrieved from PHY with the API,
    mdCopperGetAutoNeg, as shown in startExLoopback function.
*/
static MAD_BOOL orgAutoNegState = MAD_TRUE;
static MAD_U32 orgMode = MAD_AUTO_AD_ASYM_PAUSE  |
                         MAD_AUTO_AD_PAUSE       |
                         MAD_AUTO_AD_100FDX      |
                         MAD_AUTO_AD_100HDX      |
                         MAD_AUTO_AD_10FDX       |
                         MAD_AUTO_AD_10HDX       |
                         MAD_AUTO_AD_1000FDX     |
                         MAD_AUTO_AD_1000HDX     |
                         MAD_AUTO_AD_MANUAL_CONF_MS  |
                         MAD_AUTO_AD_FORCE_MASTER    |
                         MAD_AUTO_AD_PREFER_MULTI_PORT;


MAD_STATUS startExLoopback(MAD_DEV *dev,MAD_LPORT port,MAD_SPEED_MODE mode)
{
    MAD_STATUS status;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_FAIL;
    }

    MSG_PRINT("External Loopback for port %i with mode %i\n",(int)port,mode);

    /*
     *	Retrieve Copper Auto-Neg mode
    */
    if((status = mdCopperGetAutoNeg(dev,port,&orgAutoNegState,&orgMode)) != MAD_OK)
    {
        MSG_PRINT("mdCopperGetAutoNeg return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    /*
     *	Start External Loopback
    */
    if((status = mdDiagSetExternalLoopback(dev,port,MAD_TRUE,mode)) != MAD_OK)
    {
        MSG_PRINT("mdDiagSetExternalLoopback return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    return MAD_OK;
}


MAD_STATUS stopExLoopback(MAD_DEV *dev,MAD_LPORT port, MAD_SPEED_MODE mode)
{
    MAD_STATUS status;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_FAIL;
    }

    MSG_PRINT("Stopping External Loopback for port %i\n",(int)port);

    /*
     *	Stop External Loopback
    */
    if((status = mdDiagSetExternalLoopback(dev,port,MAD_FALSE,mode)) != MAD_OK)
    {
        MSG_PRINT("mdDiagSetExternalLoopback return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    /*
     *	Reconfigure Copper Auto-Neg mode
    */
    if((status = mdCopperSetAutoNeg(dev,port,orgAutoNegState,orgMode)) != MAD_OK)
    {
        MSG_PRINT("mdCopperSetAutoNeg return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }


    return MAD_OK;
}

