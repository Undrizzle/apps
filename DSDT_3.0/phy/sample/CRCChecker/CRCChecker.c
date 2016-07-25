#include <madCopyright.h>
/********************************************************************************
* CRCChecker.c
*
* DESCRIPTION:
*       CRCChecker setup sample (enableCRCChecker, disableCRCChecker, 
*       readCRCCounter, and clearCRCCounter).
*       CRC Error Counter and Frame Counter is valid after CRC Checker is enabled, and
*       in order to clear those counters, CRC Checker needs to be disabled and then 
*       enabled again.
*       Both counters will peg to 0xFF.
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
 * Enable CRC Checker.
 */
MAD_STATUS enableCRCChecker(MAD_DEV *dev,MAD_LPORT port, MAD_U32 mode)
{
    MAD_STATUS status;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_API_ERR_DEV;
    }

    MSG_PRINT("Enable CRC Checker for port %i\n",(int)port);

    /*
     *	Enable CRC Checker
    */
    if((status = mdStatsSetCRCCheckerEnable(dev,port,mode,MAD_TRUE)) != MAD_OK)
    {
        MSG_PRINT("mdStatsSetCRCCheckerEnable return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    return MAD_OK;
}


/*
 * Disable CRC Checker.
 */
MAD_STATUS disableCRCChecker(MAD_DEV *dev,MAD_LPORT port)
{
    MAD_STATUS status;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_API_ERR_DEV;
    }

    MSG_PRINT("Disable CRC Checker for port %i\n",(int)port);

    /*
     *	Disable CRC Checker
    */
    if((status = mdStatsSetCRCCheckerEnable(dev,port,0, MAD_FALSE)) != MAD_OK)
    {
        MSG_PRINT("mdStatsSetCRCCheckerEnable return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    return MAD_OK;
}


/*
 * Clear CRC Counter (CRC Error Counter and Frame Counter).
 */
MAD_STATUS clearCRCCounter(MAD_DEV *dev,MAD_LPORT port)
{
    MAD_STATUS status;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_API_ERR_DEV;
    }

    MSG_PRINT("Clear CRC Counter for port %i\n",(int)port);

    /*
     *	Disable CRC Checker
    */
    if((status = mdStatsSetCRCCheckerEnable(dev,port,0, MAD_FALSE)) != MAD_OK)
    {
        MSG_PRINT("mdStatsSetCRCCheckerEnable return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    /*
     *	Enable CRC Checker
    */
    if((status = mdStatsSetCRCCheckerEnable(dev,port,0,MAD_TRUE)) != MAD_OK)
    {
        MSG_PRINT("mdStatsSetCRCCheckerEnable return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    return MAD_OK;
}


/*
 * Read CRC Counter (CRC Error Counter and Frame Counter).
 */
MAD_STATUS readCRCCounter(MAD_DEV *dev,MAD_LPORT port)
{
    MAD_STATUS status;
    MAD_U32    counter;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_API_ERR_DEV;
    }

    MSG_PRINT("Reading CRC Counter for port %i\n",(int)port);

    /*
     *	Read CRC Error Counter
    */
    if((status = mdStatsGetCRCErrorCounter(dev,port,&counter)) != MAD_OK)
    {
        MSG_PRINT("mdStatsGetCRCErrorCounter return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    MSG_PRINT("CRC Error Counter: %i\n",(int)counter);

    /*
     *	Read Frame Counter
    */
    if((status = mdStatsGetFrameCounter(dev,port,&counter)) != MAD_OK)
    {
        MSG_PRINT("mdStatsGetFrameCounter return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    MSG_PRINT("Frame Counter:     %i\n",(int)counter);

    return MAD_OK;
}



