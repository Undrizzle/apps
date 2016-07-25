#include <madCopyright.h>
/********************************************************************************
* madIntSample.c
*
* DESCRIPTION:
*		This sample shows how to enable Marvell Alaska Phy interrupt
*       and how to handle each Interrupt Cause.
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

#define MAD_SAMPLE_TEST     1

/*
 *	To enable Marvell Alaska Phy interrupt, mdIntSetEnable API can be used.
 *  In this sample, the following Int Causes are enabled:
 *  MAD_COPPER_SPEED_CHANGED,
 *  MAD_COPPER_DUPLEX_CHANGED,
 *  MAD_COPPER_LINK_STATUS_CHANGED,
 *  MAD_COPPER_DOWNSHIFT_DETECT,
 *  MAD_COPPER_DTE_DETECT_CHANGED
 *  MAD_SGMII_FIFO_FLOW.
 *	MAD_SGMII_FIFO_IDLE_INSERT
 *  Please note that Int Causes specified above belongs to Int Group0.
*/
static 
MAD_U32 sampleGroup0Int = MAD_COPPER_SPEED_CHANGED    |
                          MAD_COPPER_DUPLEX_CHANGED   |
                          MAD_COPPER_LINK_STATUS_CHANGED  |
                          MAD_COPPER_DOWNSHIFT_DETECT |
                          MAD_COPPER_DTE_DETECT_CHANGED |
						MAD_SGMII_FIFO_FLOW|
						MAD_SGMII_FIFO_IDLE_INSERT;

MAD_STATUS sampleMADIntEnable(MAD_DEV *dev)
{
	MAD_STATUS status;
	MAD_LPORT port;
	MAD_INT_TYPE intType, getIntType;

    memset(&intType, 0, sizeof(MAD_INT_TYPE));

    intType.intGroup0 = sampleGroup0Int;

	for(port=0; port<dev->numOfPorts; port++)
	{
		if((status = mdIntSetEnable(dev,port,&intType)) != MAD_OK)
		{
			MSG_PRINT("mdIntSetEnable returned fail.\n");
            testMADDisplayStatus(status);
			return status;
		}
		if((status = mdIntGetEnable(dev,port,&getIntType)) != MAD_OK)
		{
			MSG_PRINT("mdIntSetEnable returned fail.\n");
            testMADDisplayStatus(status);
			return status;
		}
		MSG_PRINT("Set interrupt vec: %x, get: %x\n", 
			(unsigned int)getIntType.intGroup0,	(unsigned int)intType.intGroup0);
	}

	return MAD_OK;
}

/*
 *	Disable Interrupt.
*/
MAD_STATUS sampleMADIntDisable(MAD_DEV *dev)
{
	MAD_STATUS status;
	MAD_LPORT port;
	MAD_INT_TYPE intType;

    /* clear out all int causes */
    memset(&intType, 0, sizeof(MAD_INT_TYPE));

	for(port=0; port<dev->numOfPorts; port++)
	{
		if((status = mdIntSetEnable(dev,port,&intType)) != MAD_OK)
		{
			MSG_PRINT("mdIntSetEnable returned fail.\n");
            testMADDisplayStatus(status);
			return status;
		}
	}

	return MAD_OK;
}


/*
 *	This sample function shows how to deal with Marvell Phy Interrupt.
 *  It is assumed that Marvell Phy Interrupt was disabled before the call and
 *  will be enabled after the call.
*/
MAD_STATUS sampleMADIntHandler(MAD_DEV *dev)
{
    MAD_U32 portVec, intCause;
    MAD_LPORT port;
    MAD_INT_TYPE intType;
    MAD_STATUS status;

    /*
     *	Check if any of the Phy ports generated the interrupt.
    */
    if(mdIntGetPortSummary(dev,&portVec) != MAD_OK)
    {
        /* MAD experiences hardware failure (not able to read SMI). */
        return MAD_FAIL;
    }   

    if (portVec == 0)
    {
        /* None of the ports generated interrupt. */
#ifdef MAD_SAMPLE_TEST
        MSG_PRINT("None of the ports generated Interrupt.\n");
#endif

        return MAD_OK;
    }

    for(port=0; port<dev->numOfPorts; port++)
    {
        if(!(portVec & (0x1 << port)))
            continue;

        /* Get the Int Cause */
        if((status = mdIntGetStatus(dev,port,&intType)) != MAD_OK)
        {
#ifdef MAD_SAMPLE_TEST
            MSG_PRINT("mdIntGetStatus returned fail.\n");
#endif
            testMADDisplayStatus(status);
            return status;
        }

#ifdef MAD_SAMPLE_TEST
        MSG_PRINT("Port %i: Int Cause %#x.\n",(int)port,(int)intType.intGroup0);
#endif

        /* clear out uninterested interrupts that are not enabled. */
        intCause = intType.intGroup0 & sampleGroup0Int;

        if(intCause & MAD_COPPER_SPEED_CHANGED)
        {
            /* 
             *  Speed has been changed.
             *  ToDo...
            */
#ifdef MAD_SAMPLE_TEST
            MSG_PRINT("MAD_COPPER_SPEED_CHANGED Int\n");
#endif
        }

        if(intCause & MAD_COPPER_DUPLEX_CHANGED)
        {
            /* 
             *  Duplex mode has been changed.
             *  ToDo...
            */
#ifdef MAD_SAMPLE_TEST
            MSG_PRINT("MAD_COPPER_DUPLEX_CHANGED Int\n");
#endif
        }

        if(intCause & MAD_COPPER_LINK_STATUS_CHANGED)
        {
            /* 
             *  Link Status changed.
             *  ToDo...
            */
#ifdef MAD_SAMPLE_TEST
            MSG_PRINT("MAD_COPPER_LINK_STATUS_CHANGED Int\n");
#endif

        }

        if(intCause & MAD_COPPER_DOWNSHIFT_DETECT)
        {
            /* 
             *  Downshift detected.
             *  ToDo...
            */
#ifdef MAD_SAMPLE_TEST
            MSG_PRINT("MAD_COPPER_DOWNSHIFT_DETECT Int\n");
#endif
    
        }

        if(intCause & MAD_COPPER_DTE_DETECT_CHANGED)
        {
            /* 
             *  DTE power detection status changed.
             *  ToDo...
            */
#ifdef MAD_SAMPLE_TEST
            MSG_PRINT("MAD_COPPER_DTE_DETECT_CHANGED Int\n");
#endif
    
        }

    }

	return MAD_OK;
}

