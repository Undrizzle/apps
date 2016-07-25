#include <madCopyright.h>
/********************************************************************************
* madPktGenSample.c
*
* DESCRIPTION:
*       Packet Generator setup sample (startPktGenerator and stopPktGenerator).
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
 * Start Packet Generator.
 * Input:
 *      pktload - enum MAD_PG_PAYLOAD (MAD_PG_PAYLOAD_RANDOM or MAD_PG_PAYLOAD_5AA5)
 *      length  - enum MAD_PG_LENGTH  (MAD_PG_LENGTH_64 or MAD_PG_LENGTH_1514)
 *      tx      - enum MAD_PG_TX      (MAD_PG_TX_NORMAL or MAD_PG_TX_ERROR)
 *      en_type - enum MAD_PG_EN_TYPE (MAD_PG_DISABLE or MAD_PG_EN_COPPER...)
*/
MAD_STATUS startPktGenerator
(
    MAD_DEV         *dev,
    MAD_LPORT       port,
    MAD_PG_PAYLOAD  payload,
    MAD_PG_LENGTH   length,
    MAD_PG_TX       tx,
    MAD_PG_EN_TYPE  en_type
)
{
    MAD_STATUS status;
    MAD_PG     pktInfo;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_FAIL;
    }

    MSG_PRINT("Start Packet Generator for port %i\n",(int)port);

    pktInfo.payload = payload; /* Pseudo-random, 5AA55AA5... */
    pktInfo.length = length;   /* 64 bytes, 1514 bytes */
    pktInfo.tx = tx;           /* normal packet, error packet */
    pktInfo.en_type = en_type; /* Enable_type */

    /*
     *	Start Packet Generator
    */
    if((status = mdDiagSetPktGenEnable(dev,port,1,&pktInfo)) != MAD_OK)
    {
        MSG_PRINT("mdDiagSetPktGenEnable return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    return MAD_OK;
}


/*
 * Stop Packet Generator.
 */
MAD_STATUS stopPktGenerator(MAD_DEV *dev,MAD_LPORT port)
{
    MAD_STATUS status;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_FAIL;
    }

    MSG_PRINT("Stopping Packet Generator for port %i\n",(int)port);

    /*
     *	Start Packet Generator
    */
    if((status = mdDiagSetPktGenEnable(dev,port,0,NULL)) != MAD_OK)
    {
        MSG_PRINT("mdDiagSetPktGenEnable return Failed\n");
        testMADDisplayStatus(status);
        return status;
    }

    return MAD_OK;
}
