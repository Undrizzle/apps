#include <madCopyright.h>
/********************************************************************************
* madLineLoop.c
*
* DESCRIPTION:
*       Line Loopback setup sample (startLineLoopback, stopLineLoopback).
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
*/

MAD_STATUS startLineLoopback(MAD_DEV *dev,MAD_LPORT port)
{
    MAD_STATUS status;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_FAIL;
    }

    MSG_PRINT("Line Loopback for port %i \n",(int)port);

    /*
     *	Start Line Loopback
    */
    if((status = mdDiagSetLineLoopback(dev,port,MAD_TRUE)) != MAD_OK)
    {
        MSG_PRINT("mdDiagSetLineLoopback return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    return MAD_OK;
}


MAD_STATUS stopLineLoopback(MAD_DEV *dev,MAD_LPORT port)
{
    MAD_STATUS status;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_FAIL;
    }

    MSG_PRINT("Stopping Line Loopback for port %i\n",(int)port);

    /*
     *	Stop Line Loopback
    */
    if((status = mdDiagSetLineLoopback(dev,port,MAD_FALSE)) != MAD_OK)
    {
        MSG_PRINT("mdDiagSetLineLoopback return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    return MAD_OK;
}

