#include <madCopyright.h>
/********************************************************************************
* testMAD.c
*
* DESCRIPTION:
*       API test functions
*
* DEPENDENCIES:   Platform.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "madSample.h"

/*
#define TEST_DEBUG
*/

extern void testMADDisplayStatus(MAD_STATUS status);


MAD_U32 testAutoNeg(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_BOOL    en;
    int         testCase, nCases;
    MAD_U32     mode, testMode[11];

	MSG_PRINT("Testing Forced Mode with various mode...\n");

    nCases = 4;
    testMode[0] = MAD_PHY_100FDX;
    testMode[1] = MAD_PHY_100HDX;
    testMode[2] = MAD_PHY_10FDX;
    testMode[3] = MAD_PHY_10HDX;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdCopperSetAutoNeg(dev,port,MAD_FALSE,testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdCopperGetAutoNeg(dev,port,&en,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (en != MAD_FALSE)
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, en %i)\n",(int)port,testCase,en);
                return MAD_FAIL;
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,testCase,(int)mode,(int)testMode[testCase]);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");

    MSG_PRINT("Testing SetAutoNeg with various mode...\n");


  if(dev->phyInfo.featureSet & MAD_PHY_30XX_MODE)
  { /* E3016/E3082 has no 1000M */
    nCases = 9;
    testMode[0] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX;

    testMode[1] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX;

    testMode[2] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_10FDX;

    testMode[3] = 
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX;

    testMode[4] = 
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10HDX;

    testMode[5] = MAD_AUTO_AD_100FDX;
    testMode[6] = MAD_AUTO_AD_100HDX;
    testMode[7] = MAD_AUTO_AD_10FDX;

    testMode[8] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX;
  }
  else	  
  {
    nCases = 11;
    testMode[0] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX |
               MAD_AUTO_AD_1000FDX |
               MAD_AUTO_AD_1000HDX |
               MAD_AUTO_AD_MANUAL_CONF_MS |
               MAD_AUTO_AD_FORCE_MASTER |
               MAD_AUTO_AD_PREFER_MULTI_PORT;

    testMode[1] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX |
               MAD_AUTO_AD_1000FDX |
               MAD_AUTO_AD_1000HDX;

    testMode[2] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_1000FDX;

    testMode[3] = 
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX |
               MAD_AUTO_AD_1000FDX |
               MAD_AUTO_AD_1000HDX;

    testMode[4] = 
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10HDX |
               MAD_AUTO_AD_1000HDX;

    testMode[5] = MAD_AUTO_AD_100FDX;
    testMode[6] = MAD_AUTO_AD_1000FDX;
    testMode[7] = MAD_AUTO_AD_100HDX;
    testMode[8] = MAD_AUTO_AD_1000HDX;
    testMode[9] = MAD_AUTO_AD_10FDX;

    testMode[10] = MAD_AUTO_AD_ASYM_PAUSE |
               MAD_AUTO_AD_PAUSE |
               MAD_AUTO_AD_100FDX |
               MAD_AUTO_AD_100HDX |
               MAD_AUTO_AD_10FDX |
               MAD_AUTO_AD_10HDX |
               MAD_AUTO_AD_1000FDX |
               MAD_AUTO_AD_1000HDX |
               MAD_AUTO_AD_MANUAL_CONF_MS |
               MAD_AUTO_AD_FORCE_MASTER |
               MAD_AUTO_AD_PREFER_MULTI_PORT;
  }

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdCopperSetAutoNeg(dev,port,MAD_TRUE,testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdCopperGetAutoNeg(dev,port,&en,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (en != MAD_TRUE)
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, en %i)\n",(int)port,testCase,en);
               return MAD_FAIL;
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %x:%x)\n",
                            (int)port,testCase,(unsigned int)mode,(unsigned int)testMode[testCase]);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n");
    return MAD_OK;
}   


MAD_U32 testMDIMode(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    int         testCase, nCases;
    MAD_MDI_MODE     mode, testMode[10];

    MSG_PRINT("Testing SetMDIMode with various mode...\n");

    nCases = 3;
    testMode[0] = MAD_AUTO_MDI_MDIX;
    testMode[1] = MAD_FORCE_MDI;
    testMode[2] = MAD_FORCE_MDIX;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdCopperSetMDIMode(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdCopperGetMDIMode(dev,port,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,testCase,(int)mode,(int)testMode[testCase]);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}



MAD_U32 testDownshift(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[10];
	MAD_16		count;
#define TestCount			3

    MSG_PRINT("Testing Downshift Enable...\n");

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;
	count = TestCount;;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdCopperSetDownshiftEnable(dev,port,testMode[testCase], count);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdCopperGetDownshiftEnable(dev,port,&mode, &count);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ((mode != testMode[testCase]) || (count != TestCount))
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i, count %i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase], count);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}


MAD_U32 testDTEDetect(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    int         testCase, nCases;
    MAD_BOOL    mode,testMode[10];
	MAD_U16		dropHys;
#define TestDropHys		7

    MSG_PRINT("Testing DTE Detect Enable...\n");

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;
	dropHys = TestDropHys;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdCopperSetDTEDetectEnable(dev,port,testMode[testCase], dropHys);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdCopperGetDTEDetectEnable(dev,port,&mode, &dropHys);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ((mode != testMode[testCase]) || (dropHys != TestDropHys))
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i dropHys %i)\n",
                            (int)port,testCase,(int)mode,(int)testMode[testCase], dropHys);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}


MAD_STATUS testMADCtrl(MAD_DEV *dev)
{
    testAutoNeg(dev);  
    testMDIMode(dev); 
    testDownshift(dev);
    testDTEDetect(dev);

    return MAD_OK;
}

#define MAX_SAVED_ERROR	32
static int savedResult [MAX_SAVED_ERROR];
static char savedResultHead [MAX_SAVED_ERROR] [64];

MAD_STATUS testMADStatus(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_BOOL    onOff;
    MAD_U32     data;
	MAD_32		i, count=0;

    MSG_PRINT("\tLink\tSpeed\tDpx\tShift\tDTE\n");

	for ( i=0; i<MAX_SAVED_ERROR; i++)
		savedResult [i]=0;


    for(port=0; port<dev->numOfPorts; port++)
    {
        MSG_PRINT("port%1i\t",(int)port);
        retVal = mdGetLinkStatus(dev,port,&onOff);
        if (retVal != MAD_OK)
        {
            MSG_PRINT("%x\t",(int)retVal);  
            sprintf(savedResultHead[count], "GetLinkStatus Failed %x: ", retVal);
            savedResult[count++]=retVal;
        }
        else
        {
            MSG_PRINT("%i\t",(int)onOff);
        }

        retVal = mdGetSpeedStatus(dev,port,(MAD_SPEED_MODE *)&data);
        if (retVal != MAD_OK)
        {
            MSG_PRINT("%x\t",(int)retVal); 
            sprintf(savedResultHead[count], "GetSpeedStatus Failed %x: ", retVal);
            savedResult[count++]=retVal;
        }
        else
        {
            MSG_PRINT("%i\t",(int)data);
        }
    
        retVal = mdGetDuplexStatus(dev,port,(MAD_DUPLEX_MODE *)&data);
        if (retVal != MAD_OK)
        {
            MSG_PRINT("%x\t",(int)retVal); 
            sprintf(savedResultHead[count], "GetDuplexStatus Failed %x: ", retVal);
            savedResult[count++]=retVal;
        }
        else
        {
            MSG_PRINT("%i\t",(int)data);
        }
    
        retVal = mdCopperGetDownshiftStatus(dev,port,&onOff); 
        if (retVal != MAD_OK)
        {
            MSG_PRINT("%x\t",(int)retVal); 
            sprintf(savedResultHead[count], "GetDownShiftStatus Failed %x: ", retVal);
            savedResult[count++]=retVal;
        }
        else
        {
            MSG_PRINT("%i\t",(int)onOff);
        }
    
        retVal = mdCopperGetDTEDetectStatus(dev,port,&onOff);  
        if (retVal != MAD_OK)
        {
            MSG_PRINT("%x\t",(int)retVal); 
            sprintf(savedResultHead[count], "GetDTEDetectStatus Failed %x: ", retVal);
            savedResult[count++]=retVal;
        }
        else
        {
            MSG_PRINT("%i\t",(int)onOff);
        }
        MSG_PRINT("\n");
   
    }

	if (count)
#ifdef SHOW_DEBUG_INFO
	  MSG_PRINT_DBG ("\n\nReturned error: \n");
#else
	  MSG_PRINT ("\n\n$$$ Show all returned error:\n");
#endif

	for (i=0; i<count; i++)
	{
      MSG_PRINT ("%s", savedResultHead[i]);
#ifdef SHOW_DEBUG_INFO
	  testMADDisplayStatus(savedResult[i]);
#endif
	}

    return MAD_OK;
}

MAD_STATUS startPktCRC(MAD_DEV *dev, MAD_LPORT port)
{
    MAD_STATUS retVal;
    MAD_PG     pktInfo;
    MAD_U32    counter;

    MSG_PRINT("Enabling CRC Counter and start generating Pkts\n");

    pktInfo.payload = 0;
    pktInfo.length = 0;
    pktInfo.tx = 0;
    pktInfo.en_type = 0;

    retVal = mdStatsSetCRCCheckerEnable(dev,port,0, MAD_TRUE);
    if (retVal != MAD_OK)
    {
        MSG_PRINT("enableCRCChecker return ");
        testMADDisplayStatus(retVal);
        return retVal;             
    }
    
    retVal = mdDiagSetPktGenEnable(dev,port,1,&pktInfo);
    if (retVal != MAD_OK)
    {
        MSG_PRINT("mdDiagSetPktGenEnable return ");
        testMADDisplayStatus(retVal);
        return retVal;             
    }
    
    MSG_PRINT("Press any key to stop Pkt Generator and read CRC Counter\n");

    /* wait for seconds... */
/*    gets(str);  get an entire string from the keyboard. */
    
    /*
     *	Read CRC Error Counter
    */
    retVal = mdStatsGetCRCErrorCounter(dev,port,&counter);
    if (retVal != MAD_OK)
    {
        MSG_PRINT("mdStatsGetCRCErrorCounter return ");
        testMADDisplayStatus(retVal);
        return retVal;             
    }

    MSG_PRINT("CRC Error Counter: %i\n",(int)counter);

    /*
     *	Read Frame Counter
    */
    retVal = mdStatsGetFrameCounter(dev,port,&counter);
    if (retVal != MAD_OK)
    {
        MSG_PRINT("mdStatsGetFrameCounter return ");
        testMADDisplayStatus(retVal);
        return retVal;             
    }

    MSG_PRINT("Frame Counter:     %i\n\n",(int)counter);

    /*
     *  Stop Pkt Generator
    */
    retVal = mdDiagSetPktGenEnable(dev,port,0,NULL);
    if (retVal != MAD_OK)
    {
        MSG_PRINT("mdDiagSetPktGenEnable return ");
        testMADDisplayStatus(retVal);
        return retVal;             
    }
    
    /*
     *  Disable CRC Counter
    */
    retVal = mdStatsSetCRCCheckerEnable(dev,port,0,MAD_FALSE);
    if (retVal != MAD_OK)
    {
        MSG_PRINT("enableCRCChecker return ");
        testMADDisplayStatus(retVal);
        return retVal;             
    }

    MSG_PRINT("Stopped generating Pkts and Disabled CRC Counter\n");
    return MAD_OK;
}


MAD_U32 testCheckCounterEnable(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    int         testCase, nCases;
    MAD_U8	    mode,testMode[6];
    MAD_BOOL    en, enSet;

    MSG_PRINT("Testing CRC Counter Enable Enable...\n");

  if((dev->deviceId==MAD_88E1340)||(dev->deviceId==MAD_88E1340M)
	||(dev->deviceId==MAD_88E1340S)||(dev->deviceId==MAD_88E1540))
  {
    nCases = 5;
    testMode[0] = MAD_CHECKER_CTRL_COPPER;
    testMode[1] = MAD_CHECKER_CTRL_SGMII;
    testMode[2]= MAD_CHECKER_CTRL_MACSEC_TR;
    testMode[3]= MAD_CHECKER_CTRL_QSGMII;
    testMode[4] = MAD_CHECKER_CTRL_MACSEC_REC;
    testMode[5] = MAD_CHECKER_CTRL_DISABLE;	/* not test */
  }
  else
  {
    nCases = 2;
    testMode[0] = 0;
    testMode[1] = 1;
  }

  enSet = MAD_TRUE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdStatsSetCRCCheckerEnable(dev,port,testMode[testCase], enSet);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdStatsGetCRCCheckerEnable(dev,port,&mode, &en);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ((mode != testMode[testCase]) || (en != enSet))
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %x:%x ebable %i:%i)\n",
                            (int)port,testCase,(int)mode,(int)testMode[testCase], enSet, en);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testTempInt(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[4];

    MSG_PRINT("Testing Temperature sensor interrupt Enable...\n");

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdTempIntSetEnable(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdTempIntGetEnable(dev,port,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testQSGMIIPwD(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[4];

    MSG_PRINT("Testing System QSGMII Power down Enable...\n");

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetQSGMIIPwDown(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetQSGMIIPwDown(dev,port,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 tesSnoop(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U16    mode,testMode[4];

    MSG_PRINT("Testing System Snooping control...\n");

    nCases = 3;
    testMode[0] = MAD_SNOOPING_OFF;
    testMode[1] = MAD_SNOOPING_FROM_NET;
    testMode[2] = MAD_SNOOPING_FROM_MAC;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetSnoopCtrl(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetSnoopCtrl(dev,port,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testLinkDownDelay(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U16    mode,testMode[4];

    MSG_PRINT("Testing Copper link down delay...\n");

    nCases = 4;
    testMode[0] = MAD_1G_LINKDOWN_DELAY_0;
    testMode[1] = MAD_1G_LINKDOWN_DELAY_10;
    testMode[2] = MAD_1G_LINKDOWN_DELAY_20;
    testMode[3] = MAD_1G_LINKDOWN_DELAY_40;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdCopperSetLinkDownDelay(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdCopperGetLinkDownDelay(dev,port,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testLinkDownDelayEn(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[4];

    MSG_PRINT("Testing Copper link down delay enable...\n");

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdCopperSetLinkDownDelayEnable(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdCopperGetLinkDownDelayEnable(dev,port,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testMadLED(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port=0;
    MAD_LED_CTRL_DATA   ledData, testLedData, kpLedData;

    MSG_PRINT("Testing LED Control data...\n");
      retVal = mdSysGetLEDCtrl(dev,port,&kpLedData);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Getting for (port %i) returned ",(int)port);
        testMADDisplayStatus(retVal);
        return retVal;             
      }

	testLedData.ledCtrl[0] = 1;
	testLedData.ledCtrl[1] = 2;
	testLedData.ledCtrl[2] = 3;
	testLedData.ledCtrl[3] = 4;
	testLedData.ledCtrl[4] = 5;
	testLedData.ledCtrl[5] = 6;
	testLedData.ledPolarity[0] = 1;
	testLedData.ledPolarity[1] = 2;
	testLedData.ledPolarity[2] = 3;
	testLedData.ledPolarity[3] = 3;
	testLedData.ledPolarity[4] = 2;
	testLedData.ledPolarity[5] = 1;
	testLedData.ledMixPercen0 = 0x5;
	testLedData.ledMixPercen1 = 0xa;
	testLedData.ledForceInt = 1;
	testLedData.ledPulseDuration = 0x5;
	testLedData.ledBlinkRate = 0x6;
	testLedData.ledSppedOffPulse_perio = 0x1;
	testLedData.ledSppedOnPulse_perio = 0x1;
	testLedData.ledFuncMap3 = 0x1;
	testLedData.ledFuncMap2 = 0x1;

    for(port=0; port<dev->numOfPorts; port++)
    {
      retVal = mdSysSetLEDCtrl(dev,port,&testLedData);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Setting for (port %i) returned ",(int)port);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
      retVal = mdSysGetLEDCtrl(dev,port,&ledData);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Getting for (port %i) returned ",(int)port);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
	  if ((testLedData.ledCtrl[0] != ledData.ledCtrl[0]) ||
		(testLedData.ledCtrl[1] != ledData.ledCtrl[1]) ||
		(testLedData.ledCtrl[2] != ledData.ledCtrl[2]) ||
		(testLedData.ledCtrl[3] != ledData.ledCtrl[3]) ||
		(testLedData.ledCtrl[4] != ledData.ledCtrl[4]) ||
		(testLedData.ledCtrl[5] != ledData.ledCtrl[5]) ||
		(testLedData.ledPolarity[0] != ledData.ledPolarity[0]) ||
		(testLedData.ledPolarity[1] != ledData.ledPolarity[1]) ||
		(testLedData.ledPolarity[2] != ledData.ledPolarity[2]) ||
		(testLedData.ledPolarity[3] != ledData.ledPolarity[3]) ||
		(testLedData.ledPolarity[4] != ledData.ledPolarity[4]) ||
		(testLedData.ledPolarity[5] != ledData.ledPolarity[5]) ||
		(testLedData.ledMixPercen0 != ledData.ledMixPercen0) ||
		(testLedData.ledMixPercen1 != ledData.ledMixPercen1) ||
		(testLedData.ledForceInt != ledData.ledForceInt) ||
		(testLedData.ledPulseDuration != ledData.ledPulseDuration) ||
		(testLedData.ledBlinkRate != ledData.ledBlinkRate) ||
		(testLedData.ledSppedOffPulse_perio != ledData.ledSppedOffPulse_perio) ||
		(testLedData.ledSppedOnPulse_perio != ledData.ledSppedOnPulse_perio) ||
		(testLedData.ledFuncMap3 != ledData.ledFuncMap3) ||
		(testLedData.ledFuncMap2 != ledData.ledFuncMap2))
      {
        MSG_PRINT("Unexpected return (port %i)\n", (int)port);
		MSG_PRINT(" ledCtrl %x:%x  %x:%x  %x:%x  %x:%x  %x:%x,  %x:%x \n", 
	       testLedData.ledCtrl[0], ledData.ledCtrl[0],
		   testLedData.ledCtrl[1], ledData.ledCtrl[1],
	       testLedData.ledCtrl[2], ledData.ledCtrl[2],
		   testLedData.ledCtrl[3], ledData.ledCtrl[3],
		   testLedData.ledCtrl[4], ledData.ledCtrl[4],
		   testLedData.ledCtrl[5], ledData.ledCtrl[5]);
		MSG_PRINT(" ledPolarity %x:%x  %x:%x  %x:%x  %x:%x  %x:%x,  %x:%x \n", 
	       testLedData.ledPolarity[0], ledData.ledPolarity[0],
		   testLedData.ledPolarity[1], ledData.ledPolarity[1],
	       testLedData.ledPolarity[2], ledData.ledPolarity[2],
		   testLedData.ledPolarity[3], ledData.ledPolarity[3],
		   testLedData.ledPolarity[4], ledData.ledPolarity[4],
		   testLedData.ledPolarity[5], ledData.ledPolarity[5]);
		MSG_PRINT(" Mix percentige 0_2_4: %x:%x  1_3_5: %x:%x \n", 
	       testLedData.ledMixPercen0, ledData.ledMixPercen0,
		   testLedData.ledMixPercen1, ledData.ledMixPercen1);
		MSG_PRINT(" Force INT: %x:%x  \n", 
		   testLedData.ledForceInt, ledData.ledForceInt);
		MSG_PRINT(" led Pulse Duration: %x:%x  \n", 
		   testLedData.ledPulseDuration, ledData.ledPulseDuration);
		MSG_PRINT(" Blink rate: %x:%x  \n", 
		   testLedData.ledBlinkRate, ledData.ledBlinkRate);
		MSG_PRINT(" Speed off Pulse Period %x:%x  \n", 
		   testLedData.ledSppedOffPulse_perio, ledData.ledSppedOffPulse_perio);
		MSG_PRINT(" Speed on Pulse Period %x:%x  \n", 
		   testLedData.ledSppedOnPulse_perio, ledData.ledSppedOnPulse_perio);
		MSG_PRINT(" Function pin mapping LED3 : %x:%x  LED2: %x:%x \n", 
	       testLedData.ledFuncMap3, ledData.ledFuncMap3,
		   testLedData.ledFuncMap2, ledData.ledFuncMap2);
        return MAD_FAIL;
      }

    }

      retVal = mdSysSetLEDCtrl(dev,port, &kpLedData);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Getting for (port %i) returned ",(int)port);
        testMADDisplayStatus(retVal);
        return retVal;             
      }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testCollAdj(MAD_DEV *dev)
{
    MAD_STATUS  retVal, retVal1;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U8    mode,testMode[8];
    MAD_U32     savedMode;
    MAD_BOOL    savedEn;

    MSG_PRINT("Testing Late Collision window adjust..\n");

    nCases = 8;
    testMode[0] = 1;
    testMode[1] = 2;
    testMode[2] = 4;
    testMode[3] = 8;
    testMode[4] = 0x10;
    testMode[5] = 0x15;
    testMode[6] = 0xa;
    testMode[7] = 0x1f;

    for(port=0; port<dev->numOfPorts; port++)
    {
	  retVal = mdCopperGetAutoNeg(dev,port,&savedEn,&savedMode);
	  if (retVal != MAD_OK)
	  {
	    MSG_PRINT("Getting Link mode or (port %i) returned error",(int)port);
	    testMADDisplayStatus(retVal);
	    return retVal;             
	  }
   
	  /* Foced to set Half dulpex mode */
	  retVal = mdCopperSetAutoNeg(dev,port,MAD_FALSE,MAD_PHY_100HDX);
	  if (retVal != MAD_OK)
	  {
	    MSG_PRINT("Setting Link mode MAD_PHY_100HDX for (port %i) returned error",(int)port);
	    testMADDisplayStatus(retVal);
	    return retVal;             
	  }
        for(testCase=0; testCase<nCases; testCase++)
        {   
    
            retVal1 = mdSysSetLateCollision_winAdj(dev,port,testMode[testCase]);
            if (retVal1 != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal1);
                break;;             
            }
    
            retVal1 = mdSysGetLateCollision_winAdj(dev,port,&mode);
            if (retVal1 != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal1);
                break;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]);
                retVal1 = MAD_FAIL;
				break;
            }


        }
		/* Set back link mode */
		retVal = mdCopperSetAutoNeg(dev,port,savedEn,savedMode);
		if (retVal != MAD_OK)
		{
		  MSG_PRINT("Setting Link mode  for (port %i) returned error",(int)port);
		  testMADDisplayStatus(retVal);
		  return retVal;             
		}
		if(retVal1!=MAD_OK)
			break;
		
    }

	if(retVal1!=MAD_OK)
	{
      /* MSG_PRINT("Failed.\n\n"); */
	  return retVal1;
	}

    MSG_PRINT("Passed.\n\n");

    return MAD_OK;
}

MAD_U32 testSyncFifo(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U8    mode,mode1,testMode[4];

    MSG_PRINT("Testing Synchronize FIFO depth..\n");

    nCases = 4;
    testMode[0] = MAD_SYNC_TRANS_FIFO_00;
    testMode[1] = MAD_SYNC_TRANS_FIFO_01;
    testMode[2] = MAD_SYNC_TRANS_FIFO_02;
    testMode[3] = MAD_SYNC_TRANS_FIFO_03;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        { 
            retVal = mdMacSetFifoDepth(dev,port,testMode[testCase],testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdMacGetFifoDepth(dev,port,&mode, &mode1);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ( (mode != testMode[testCase]) || (mode1 != testMode[testCase]))
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i+++%i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)mode1,(int)testMode[testCase]);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testPowerDownEn(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[2];

    MSG_PRINT("Testing Power Down enable...\n");

    nCases = 2;
    testMode[0] = MAD_FALSE;
    testMode[1] = MAD_TRUE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetPhyEnable(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetPhyEnable(dev,port,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testQSGMIIPowerDownEn(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[2];

    MSG_PRINT("Testing QSGMII Power Down enable...\n");

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetQSGMIIPwDown(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetQSGMIIPwDown(dev,port,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testMACPowerDownEn(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[2];

    MSG_PRINT("Testing MAC Power Down enable...\n");

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetMacPowerDown(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetMacPowerDown(dev,port,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testDetectPowerDownMode(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_U16    mode,testMode[3];

    MSG_PRINT("Testing Detect Power Down mode...\n");

    if(dev->phyInfo.featureSet & MAD_PHY_30XX_MODE)
	{
      nCases = 2;
      testMode[0] = MAD_ENERGE_DETECT_MODE_YES;
      testMode[1] = MAD_ENERGE_DETECT_MODE_NO;
	}
	else
	{
      nCases = 3;
      testMode[0] = MAD_ENERGE_DETECT_MODE_1;
      testMode[1] = MAD_ENERGE_DETECT_MODE_2;
      testMode[2] = MAD_ENERGE_DETECT_MODE_NO;
	}

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetDetectPowerDownMode(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetDetectPowerDownMode(dev,port,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testUniDirTransEn(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, nCases;
    MAD_BOOL    mode,testMode[2];

    MSG_PRINT("Testing Unidirectional transmit enable...\n");

    nCases = 2;
    testMode[0] = MAD_TRUE;
    testMode[1] = MAD_FALSE;

    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSysSetUniDirTrans(dev,port,testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSysGetUniDirTrans(dev,port,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if (mode != testMode[testCase])
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                            (int)port,(int)testCase,(int)mode,(int)testMode[testCase]);
                return MAD_FAIL;
            }

        }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_STATUS testPatPRBS(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_LPORT   port;
    MAD_U32   testCase, nCases;
    MAD_PRBS_GEN_CONFIG    mode,testMode[2];

    MSG_PRINT("Test setting Pattern generator PRBS \n");

    nCases = 2;
	testMode[0].genEn = 1;	
	testMode[0].countEn = 1;	
	testMode[0].polarCheckInv = 1;
	testMode[0].polarGenInv = 1;
	testMode[0].counterLock = 1;
	testMode[0].patternSel = 3;
	testMode[0].errCounter = 0;

	testMode[1].genEn = 0;	
	testMode[1].countEn = 0;	
	testMode[1].polarCheckInv = 0;
	testMode[1].polarGenInv = 0;
	testMode[1].counterLock = 0;
	testMode[1].patternSel = 0;
	testMode[1].errCounter = 0;


    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = madPatCtrlSetPRBS(dev,port,&testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = madPatCtrlGetPRBS(dev,port,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
 			if((mode.genEn != testMode[testCase].genEn) ||	
				(mode.countEn != testMode[testCase].countEn) ||	
				(mode.polarCheckInv != testMode[testCase].polarCheckInv) ||
				(mode.polarGenInv != testMode[testCase].polarGenInv) ||
				(mode.counterLock != testMode[testCase].counterLock) ||
				(mode.patternSel != testMode[testCase].patternSel) )
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, genEn %i:%i)\n",
                            (int)port,(int)testCase,(int)mode.genEn,(int)testMode[testCase].genEn);
                MSG_PRINT("		countEn %i:%i)\n", (int)mode.countEn,(int)testMode[testCase].countEn);
                MSG_PRINT("		polarCheckInv %i:%i)\n", (int)mode.polarCheckInv,(int)testMode[testCase].polarCheckInv);
                MSG_PRINT("		polarGenInv %i:%i)\n", (int)mode.polarGenInv,(int)testMode[testCase].polarGenInv);
                MSG_PRINT("		counterLock %i:%i)\n", (int)mode.counterLock,(int)testMode[testCase].counterLock);
                MSG_PRINT("		patternSel %i:%i)\n", (int)mode.patternSel,(int)testMode[testCase].patternSel);
                return MAD_FAIL;
            }
        }
    }

    MSG_PRINT("Passed.\n\n");
    
    return MAD_OK;
}

MAD_STATUS testPat125PRBS(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_LPORT   port;
    MAD_U32   testCase, nCases;
    MAD_PRBS_GEN_CONFIG    mode,testMode[2];

    MSG_PRINT("Test setting Pattern generator 1.25G PRBS \n");

    nCases = 2;
	testMode[0].genEn = 1;	
	testMode[0].countEn = 1;	
	testMode[0].polarCheckInv = 1;
	testMode[0].polarGenInv = 1;
	testMode[0].counterLock = 1;
	testMode[0].errCounter = 0;

	testMode[1].genEn = 0;	
	testMode[1].countEn = 0;	
	testMode[1].polarCheckInv = 0;
	testMode[1].polarGenInv = 0;
	testMode[1].counterLock = 0;
	testMode[1].errCounter = 0;


    for(port=0; port<dev->numOfPorts; port++)
    {
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = madPatCtrlSet125GPRBS(dev,port,&testMode[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned %x\n",(int)port,(int)testCase, retVal);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = madPatCtrlGet125GPRBS(dev,port,&mode);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned %x",(int)port,(int)testCase, retVal);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
 			if((mode.genEn != testMode[testCase].genEn) ||	
				(mode.countEn != testMode[testCase].countEn) ||	
				(mode.polarCheckInv != testMode[testCase].polarCheckInv) ||
				(mode.polarGenInv != testMode[testCase].polarGenInv) ||
				(mode.counterLock != testMode[testCase].counterLock) )
            {
                MSG_PRINT("Unexpected return (port %i, test case %i, genEn %i:%i)\n",
                            (int)port,(int)testCase,(int)mode.genEn,(int)testMode[testCase].genEn);
                MSG_PRINT("		countEn %i:%i)\n", (int)mode.countEn,(int)testMode[testCase].countEn);
                MSG_PRINT("		polarCheckInv %i:%i)\n", (int)mode.polarCheckInv,(int)testMode[testCase].polarCheckInv);
                MSG_PRINT("		polarGenInv %i:%i)\n", (int)mode.polarGenInv,(int)testMode[testCase].polarGenInv);
                MSG_PRINT("		counterLock %i:%i)\n", (int)mode.counterLock,(int)testMode[testCase].counterLock);
                MSG_PRINT("		patternSel %i:%i)\n", (int)mode.patternSel,(int)testMode[testCase].patternSel);
                return MAD_FAIL;
            }
        }
    }

    MSG_PRINT("Passed.\n\n");
    
    return MAD_OK;
}

MAD_STATUS testPtpEnable(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_BOOL    mode,testMode[2];

    MSG_PRINT("Test setting PTP Enable \n");

    nCases = 2;
	testMode[0] = MAD_TRUE;	

	testMode[1] = MAD_FALSE;	


    for(testCase=0; testCase<nCases; testCase++)
    {   
        retVal = mdPtpSetEnable(dev,testMode[testCase]);
        if (retVal != MAD_OK)
        {
            MSG_PRINT("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal);
            testMADDisplayStatus(retVal);
            return retVal;             
        }
    
        retVal = mdPtpGetEnable(dev,&mode);
        if (retVal != MAD_OK)
        {
            MSG_PRINT("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal);
            testMADDisplayStatus(retVal);
            return retVal;             
        }
          
		if (mode != testMode[testCase]) 
        {
            MSG_PRINT("Unexpected return (port %i, test case %i, en %i:%i)\n",
                        (int)0,(int)testCase,(int)mode,(int)testMode[testCase]);
            return MAD_FAIL;
        }
    }

    MSG_PRINT("Passed.\n\n");
    
    return MAD_OK;
}

MAD_STATUS testPtpIntEnable(MAD_DEV *dev)
{
  MAD_STATUS retVal;
  MAD_U32   testCase, nCases;
  MAD_BOOL    mode,testMode[2];
  MAD_BOOL en, kpEn;

  MSG_PRINT("Test setting PTP Interrupt Enable \n");

  nCases = 2;
  testMode[0] = MAD_TRUE;	
  testMode[1] = MAD_FALSE;	

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

	if (en!=MAD_TRUE)
	{
      MSG_PRINT("Can not enable PTP \n");
	  return MAD_FAIL;
	}
	else
      MSG_PRINT("!!!!!!!! Enable PTP !!!!!!\n");
#endif
    

    for(testCase=0; testCase<nCases; testCase++)
    {   
        retVal = mdPtpSetIntEnable(dev,testMode[testCase]);
        if (retVal != MAD_OK)
        {
            MSG_PRINT("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal);
            testMADDisplayStatus(retVal);
            return retVal;             
        }
    
        retVal = mdPtpGetIntEnable(dev,&mode);
        if (retVal != MAD_OK)
        {
            MSG_PRINT("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal);
            testMADDisplayStatus(retVal);
            return retVal;             
        }
          
		if (mode != testMode[testCase]) 
        {
            MSG_PRINT("Unexpected return (port %i, test case %i, en %i:%i)\n",
                        (int)0,(int)testCase,(int)mode,(int)testMode[testCase]);
            return MAD_FAIL;
        }
    }

    MSG_PRINT("Passed.\n\n");
    
    return MAD_OK;
}

MAD_STATUS testPtpConfig(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_PTP_CONFIG    mode,testMode[2];
	MAD_BOOL en, kpEn;
	int port;

    MSG_PRINT("Test setting PTP Config \n");

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

	if (en!=MAD_TRUE)
	{
      MSG_PRINT("Can not enable PTP \n");
	  return MAD_FAIL;
	}
	else
      MSG_PRINT("!!!!!!!! Enable PTP !!!!!!\n");
#endif
    
    nCases = 2;
	testMode[0].ptpEType = 1;	
	testMode[0].msgIdTSEn = 1;	
	testMode[0].tsArrPtr = 1;
	testMode[0].ptpArrIntEn = 1;
	testMode[0].ptpDepIntEn = 1;
	testMode[0].transSpec = 1;
/*	testMode[0].msgIdStartBit = 1; */
	testMode[0].disTSOverwrite = 1;

	testMode[1].ptpEType = 0;	
	testMode[1].msgIdTSEn = 0;	
	testMode[1].tsArrPtr = 0;
	testMode[1].ptpArrIntEn = 0;
	testMode[1].ptpDepIntEn = 0;
	testMode[1].transSpec = 0;
/*	testMode[1].msgIdStartBit = 0; */
	testMode[1].disTSOverwrite = 0;

  for(port=0; port<4; port++)
  {
			testMode[0].ptpArrIntEn = (1 << port);
			testMode[0].ptpDepIntEn = (1 << port);
    for(testCase=0; testCase<nCases; testCase++)
	{   
      retVal = mdPtpSetConfig_port(dev,port,&testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpGetConfig_port(dev,port,&mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
	  if((mode.ptpEType != testMode[testCase].ptpEType) ||	
		(mode.msgIdTSEn != testMode[testCase].msgIdTSEn) ||	
		(mode.tsArrPtr != testMode[testCase].tsArrPtr) ||
		(mode.ptpArrIntEn != testMode[testCase].ptpArrIntEn) ||
		(mode.ptpDepIntEn != testMode[testCase].ptpDepIntEn) ||
		(mode.transSpec != testMode[testCase].transSpec) ||
/*		(mode.msgIdStartBit != testMode[testCase].msgIdStartBit) || */
		(mode.disTSOverwrite != testMode[testCase].disTSOverwrite) )
      {
        MSG_PRINT("Unexpected return (port %i, test case %i, ptpEType %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.ptpEType,(int)testMode[testCase].ptpEType);
        MSG_PRINT("		msgIdTSEn %i:%i)\n", (int)mode.msgIdTSEn,(int)testMode[testCase].msgIdTSEn);
        MSG_PRINT("		tsArrPtr %i:%i)\n", (int)mode.tsArrPtr,(int)testMode[testCase].tsArrPtr);
        MSG_PRINT("		ptpArrIntEn %i:%i)\n", (int)mode.ptpArrIntEn,(int)testMode[testCase].ptpArrIntEn);
        MSG_PRINT("		ptpDepIntEn %i:%i)\n", (int)mode.ptpDepIntEn,(int)testMode[testCase].ptpDepIntEn);
        MSG_PRINT("		transSpec %i:%i)\n", (int)mode.transSpec,(int)testMode[testCase].transSpec);
        MSG_PRINT("		msgIdStartBit %i:%i)\n", (int)mode.msgIdStartBit,(int)testMode[testCase].msgIdStartBit);
        MSG_PRINT("		disTSOverwrite %i:%i)\n", (int)mode.disTSOverwrite,(int)testMode[testCase].disTSOverwrite);
        return MAD_FAIL;
      }
#if 0
      MSG_PRINT(" (port %i, test case %i, ptpEType %i:%i)\n",
                      (int)0,(int)testCase,(int)mode.ptpEType,(int)testMode[testCase].ptpEType);
      MSG_PRINT("		msgIdTSEn %i:%i)\n", (int)mode.msgIdTSEn,(int)testMode[testCase].msgIdTSEn);
      MSG_PRINT("		tsArrPtr %i:%i)\n", (int)mode.tsArrPtr,(int)testMode[testCase].tsArrPtr);
      MSG_PRINT("		ptpArrIntEn %i:%i)\n", (int)mode.ptpArrIntEn,(int)testMode[testCase].ptpArrIntEn);
      MSG_PRINT("		ptpDepIntEn %i:%i)\n", (int)mode.ptpDepIntEn,(int)testMode[testCase].ptpDepIntEn);
      MSG_PRINT("		transSpec %i:%i)\n", (int)mode.transSpec,(int)testMode[testCase].transSpec);
      MSG_PRINT("		msgIdStartBit %i:%i)\n", (int)mode.msgIdStartBit,(int)testMode[testCase].msgIdStartBit);
      MSG_PRINT("		disTSOverwrite %i:%i)\n", (int)mode.disTSOverwrite,(int)testMode[testCase].disTSOverwrite);
#endif
    }
  }

    MSG_PRINT("Passed.\n\n");
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}


MAD_STATUS testPtpGlobalConfig(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_PTP_GLOBAL_CONFIG    mode,testMode[2];
	MAD_BOOL en, kpEn;

    MSG_PRINT("Test setting PTP Global Config \n");

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

	if (en!=MAD_TRUE)
	{
      MSG_PRINT("Can not enable PTP \n");
	  return MAD_FAIL;
	}
	else
      MSG_PRINT("!!!!!!!! Enable PTP !!!!!!\n");
#endif
    
    nCases = 2;
	testMode[0].ptpEType = 1;	
	testMode[0].msgIdTSEn = 1;	
	testMode[0].tsArrPtr = 1;

	testMode[1].ptpEType = 0;	
	testMode[1].msgIdTSEn = 0;	
	testMode[1].tsArrPtr = 0;


    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpSetGlobalConfig(dev,&testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpGetGlobalConfig(dev,&mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
	  if((mode.ptpEType != testMode[testCase].ptpEType) ||	
		(mode.msgIdTSEn != testMode[testCase].msgIdTSEn) ||	
		(mode.tsArrPtr != testMode[testCase].tsArrPtr) )
      {
        MSG_PRINT("Unexpected return (port %i, test case %i, ptpEType %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.ptpEType,(int)testMode[testCase].ptpEType);
        MSG_PRINT("		msgIdTSEn %i:%i)\n", (int)mode.msgIdTSEn,(int)testMode[testCase].msgIdTSEn);
        MSG_PRINT("		tsArrPtr %i:%i)\n", (int)mode.tsArrPtr,(int)testMode[testCase].tsArrPtr);
        return MAD_FAIL;
      }
#if 1
      MSG_PRINT(" (port %i, test case %i, ptpEType %i:%i)\n",
                      (int)0,(int)testCase,(int)mode.ptpEType,(int)testMode[testCase].ptpEType);
      MSG_PRINT("		msgIdTSEn %i:%i)\n", (int)mode.msgIdTSEn,(int)testMode[testCase].msgIdTSEn);
      MSG_PRINT("		tsArrPtr %i:%i)\n", (int)mode.tsArrPtr,(int)testMode[testCase].tsArrPtr);
#endif
    }

    MSG_PRINT("Passed.\n\n");
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}


MAD_STATUS testPtpPortConfig(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_LPORT   port;
    MAD_U32   testCase, nCases;
    MAD_PTP_PORT_CONFIG    mode,testMode[2];
	MAD_BOOL en, kpEn;

    MSG_PRINT("Test setting PTP Port Config \n");

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

	if (en!=MAD_TRUE)
	{
      MSG_PRINT("Can not enable PTP \n");
	  return MAD_FAIL;
	}
	else
      MSG_PRINT("!!!!!!!! Enable PTP !!!!!!\n");
#endif
    
    nCases = 2;
	testMode[0].transSpec = 1;	
	testMode[0].disTSpec = MAD_TRUE;	
	testMode[0].etJump = 1;
	testMode[0].ipJump = 1;	
	testMode[0].ptpArrIntEn = MAD_TRUE;	
	testMode[0].ptpDepIntEn = MAD_TRUE;	
	testMode[0].disTSOverwrite = MAD_TRUE;

	testMode[1].transSpec = 0;	
	testMode[1].disTSpec = MAD_FALSE;	
	testMode[1].etJump = 0;
	testMode[1].ipJump = 0;	
	testMode[1].ptpArrIntEn = MAD_FALSE;	
	testMode[1].ptpDepIntEn = MAD_FALSE;	
	testMode[1].disTSOverwrite = MAD_FALSE;

  for(port=0; port<dev->numOfPorts; port++)
  {
    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = mdPtpSetPortConfig(dev, port, &testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Setting for (port %i, test case %i) returned %x\n",(int)port,(int)testCase, retVal);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = mdPtpGetPortConfig(dev, port, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Getting for (port %i, test case %i) returned %x",(int)port,(int)testCase, retVal);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
	  if((mode.transSpec != testMode[testCase].transSpec) ||	
		(mode.disTSpec != testMode[testCase].disTSpec) ||	
		(mode.etJump != testMode[testCase].etJump) ||	
		(mode.ipJump != testMode[testCase].ipJump) ||	
		(mode.ptpArrIntEn != testMode[testCase].ptpArrIntEn) ||	
		(mode.ptpDepIntEn != testMode[testCase].ptpDepIntEn) ||	
		(mode.disTSOverwrite != testMode[testCase].disTSOverwrite) )
      {
        MSG_PRINT("Unexpected return (port %i, test case %i, ptpEType %i:%i)\n",
                    (int)port,(int)testCase,(int)mode.transSpec,(int)testMode[testCase].transSpec);
        MSG_PRINT("		disTSpec %i:%i)\n", (int)mode.disTSpec,(int)testMode[testCase].disTSpec);
        MSG_PRINT("		etJump %i:%i)\n", (int)mode.etJump,(int)testMode[testCase].etJump);
        MSG_PRINT("		ipJump %i:%i)\n", (int)mode.ipJump,(int)testMode[testCase].ipJump);
        MSG_PRINT("		ptpArrIntEn %i:%i)\n", (int)mode.ptpArrIntEn,(int)testMode[testCase].ptpArrIntEn);
        MSG_PRINT("		ptpDepIntEn %i:%i)\n", (int)mode.ptpDepIntEn,(int)testMode[testCase].ptpDepIntEn);
        MSG_PRINT("		disTSOverwrite %i:%i)\n", (int)mode.disTSOverwrite,(int)testMode[testCase].disTSOverwrite);
        return MAD_FAIL;
      }
#if 0
    MSG_PRINT("  (port %i, test case %i, ptpEType %i:%i)\n",
                (int)port,(int)testCase,(int)mode.transSpec,(int)testMode[testCase].transSpec);
    MSG_PRINT("		disTSpec %i:%i)\n", (int)mode.disTSpec,(int)testMode[testCase].disTSpec);
    MSG_PRINT("		etJump %i:%i)\n", (int)mode.etJump,(int)testMode[testCase].etJump);
    MSG_PRINT("		ipJump %i:%i)\n", (int)mode.ipJump,(int)testMode[testCase].ipJump);
    MSG_PRINT("		ptpArrIntEn %i:%i)\n", (int)mode.ptpArrIntEn,(int)testMode[testCase].ptpArrIntEn);
    MSG_PRINT("		ptpDepIntEn %i:%i)\n", (int)mode.ptpDepIntEn,(int)testMode[testCase].ptpDepIntEn);
    MSG_PRINT("		disTSOverwrite %i:%i)\n", (int)mode.disTSOverwrite,(int)testMode[testCase].disTSOverwrite);
#endif
    }
  }

    MSG_PRINT("Passed.\n\n");
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}


MAD_STATUS testPtpPTPEn(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_BOOL    mode,testMode[2];
	MAD_BOOL en, kpEn;

    MSG_PRINT("Test PTP setting PTP enable \n");

    nCases = 2;
	testMode[0] = MAD_TRUE;	

	testMode[1] = MAD_FALSE;	

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

	if (en!=MAD_TRUE)
	{
      MSG_PRINT("Can not enable PTP \n");
	  return MAD_FAIL;
	}
	else
      MSG_PRINT("!!!!!!!! Enable PTP !!!!!!\n");
#endif


    for(testCase=0; testCase<nCases; testCase++)
    {   
        retVal = mdPtpSetPTPEn(dev,testMode[testCase]);
        if (retVal != MAD_OK)
        {
            MSG_PRINT("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal);
            testMADDisplayStatus(retVal);
            return retVal;             
        }
    
        retVal = mdPtpGetPTPEn(dev,&mode);
        if (retVal != MAD_OK)
        {
            MSG_PRINT("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal);
            testMADDisplayStatus(retVal);
            return retVal;             
        }
          
		if (mode != testMode[testCase]) 
        {
            MSG_PRINT("Unexpected return (port %i, test case %i, en %i:%i)\n",
                        (int)0,(int)testCase,(int)mode,(int)testMode[testCase]);
            return MAD_FAIL;
        }
    }

    MSG_PRINT("Passed.\n\n");
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    
    return MAD_OK;
}

MAD_STATUS testTaiEventConfig(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_TAI_EVENT_CONFIG    mode,testMode[2];
	MAD_BOOL en, kpEn;

    MSG_PRINT("Test TAI Event Config \n");

    nCases = 2;
	testMode[0].eventOverwrite = MAD_TRUE;	
	testMode[0].eventCtrStart = MAD_TRUE;	
	testMode[0].intEn = MAD_TRUE;	

	testMode[1].eventOverwrite = MAD_FALSE;	
	testMode[1].eventCtrStart = MAD_FALSE;	
	testMode[1].intEn = MAD_FALSE;	


#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

	if (en!=MAD_TRUE)
	{
      MSG_PRINT("Can not enable PTP \n");
	  return MAD_FAIL;
	}
	else
      MSG_PRINT("!!!!!!!! Enable PTP !!!!!!\n");
#endif


    for(testCase=0; testCase<nCases; testCase++)
    {   
        retVal = madTaiSetEventConfig(dev,&testMode[testCase]);
        if (retVal != MAD_OK)
        {
            MSG_PRINT("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal);
            testMADDisplayStatus(retVal);
            return retVal;             
        }
    
        retVal = madTaiGetEventConfig(dev,&mode);
        if (retVal != MAD_OK)
        {
            MSG_PRINT("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal);
            testMADDisplayStatus(retVal);
            return retVal;             
        }
          
	  if((mode.eventOverwrite != testMode[testCase].eventOverwrite) ||	
		(mode.eventCtrStart != testMode[testCase].eventCtrStart) ||	
		(mode.intEn != testMode[testCase].intEn) )
      {
        MSG_PRINT("Unexpected return (port %i, test case %i, eventOverwrite %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.eventOverwrite,(int)testMode[testCase].eventOverwrite);
        MSG_PRINT("		eventCtrStart %i:%i)\n", (int)mode.eventCtrStart,(int)testMode[testCase].eventCtrStart);
        MSG_PRINT("		intEn %i:%i)\n", (int)mode.intEn,(int)testMode[testCase].intEn);
        return MAD_FAIL;
      }
#if 0
      MSG_PRINT(" (port %i, test case %i, eventOverwrite %i:%i)\n",
                  (int)0,(int)testCase,(int)mode.eventOverwrite,(int)testMode[testCase].eventOverwrite);
      MSG_PRINT("		eventCtrStart %i:%i)\n", (int)mode.eventCtrStart,(int)testMode[testCase].eventCtrStart);
      MSG_PRINT("		intEn %i:%i)\n", (int)mode.intEn,(int)testMode[testCase].intEn);
#endif
    }

    MSG_PRINT("Passed.\n\n");
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    
    return MAD_OK;
}


MAD_STATUS testTaiTrigConfig(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_TAI_TRIGGER_CONFIG    mode,testMode[4];
	MAD_BOOL trigEn, testTrigEn[4];
	MAD_BOOL en, kpEn;

    MSG_PRINT("Test setting TAI Trig Config \n");

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

	if (en!=MAD_TRUE)
	{
      MSG_PRINT("Can not enable PTP \n");
	  return MAD_FAIL;
	}
	else
      MSG_PRINT("!!!!!!!! Enable PTP !!!!!!\n");
#endif
    
    nCases = 4;
	testTrigEn[0] = MAD_TRUE;
	testMode[0].mode = MAD_TAI_TRIG_ON_GIVEN_TIME;	
	testMode[0].trigGenAmt = 11;	
	testMode[0].pulseWidth = 13;
	testMode[0].trigClkComp = 0;

	testTrigEn[1] = MAD_TRUE;
	testMode[1].mode = MAD_TAI_TRIG_ON_GIVEN_TIME;	
	testMode[1].trigGenAmt = 0;	
	testMode[1].pulseWidth = 0;
	testMode[1].trigClkComp =0;

	testTrigEn[2] = MAD_TRUE;
	testMode[2].mode = MAD_TAI_TRIG_PERIODIC_PURSE;	
	testMode[2].trigGenAmt = 11;	
	testMode[2].pulseWidth = 0;
	testMode[2].trigClkComp = 0;

	testTrigEn[3] = MAD_TRUE;
	testMode[3].mode = MAD_TAI_TRIG_PERIODIC_PURSE;	
	testMode[3].trigGenAmt = 0;	
	testMode[3].pulseWidth = 0;
	testMode[3].trigClkComp =55;

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = madTaiSetTrigConfig(dev,testTrigEn[testCase], &testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = madTaiGetTrigConfig(dev, &trigEn, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
		if (trigEn != testTrigEn[testCase]) 
        {
            MSG_PRINT("Unexpected return (port %i, test case %i, en %i:%i)\n",
                        (int)0,(int)testCase,(int)trigEn,(int)testTrigEn[testCase]);
            return MAD_FAIL;
        }

		if((mode.mode != testMode[testCase].mode) ||	
		(mode.trigGenAmt != testMode[testCase].trigGenAmt) ||	
		(mode.pulseWidth != testMode[testCase].pulseWidth) ||
		(mode.trigClkComp != testMode[testCase].trigClkComp) )
      {
        MSG_PRINT("Unexpected return (port %i, test case %i, mode %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.mode,(int)testMode[testCase].mode);
        MSG_PRINT("		trigGenAmt %i:%i)\n", (int)mode.trigGenAmt,(int)testMode[testCase].trigGenAmt);
        MSG_PRINT("		pulseWidth %i:%i)\n", (int)mode.pulseWidth,(int)testMode[testCase].pulseWidth);
        MSG_PRINT("		trigClkComp %i:%i)\n", (int)mode.trigClkComp,(int)testMode[testCase].trigClkComp);
        return MAD_FAIL;
      }
#if 0
        MSG_PRINT(" (port %i, test case %i, mode %i:%i)\n",
                    (int)0,(int)testCase,(int)mode.mode,(int)testMode[testCase].mode);
        MSG_PRINT("		trigGenAmt %i:%i)\n", (int)mode.trigGenAmt,(int)testMode[testCase].trigGenAmt);
        MSG_PRINT("		pulseWidth %i:%i)\n", (int)mode.pulseWidth,(int)testMode[testCase].pulseWidth);
        MSG_PRINT("		trigClkComp %i:%i)\n", (int)mode.trigClkComp,(int)testMode[testCase].trigClkComp);
#endif
    }

    MSG_PRINT("Passed.\n\n");
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}


MAD_STATUS testTaiTSClkPer(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_U32    mode,testMode[2];
	MAD_BOOL en, kpEn;

    MSG_PRINT("Test setting TAI TS Clock Per \n");

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

	if (en!=MAD_TRUE)
	{
      MSG_PRINT("Can not enable PTP \n");
	  return MAD_FAIL;
	}
	else
      MSG_PRINT("!!!!!!!! Enable PTP !!!!!!\n");
#endif
    
    nCases = 2;
	testMode[0] = 0x5555;	

	testMode[1] = 0;	

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = madTaiSetTSClkPer(dev, testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = madTaiGetTSClkPer(dev, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
		if (mode != testMode[testCase]) 
        {
            MSG_PRINT("Unexpected return (port %i, test case %i, en %i:%i)\n",
                        (int)0,(int)testCase,(int)mode,(int)testMode[testCase]);
            return MAD_FAIL;
        }

#if 1
        MSG_PRINT(" (port %i, test case %i, mode %i:%i)\n",
                    (int)0,(int)testCase,(int)mode,(int)testMode[testCase]);
#endif
    }

    MSG_PRINT("Passed.\n\n");
#if 0
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}


MAD_STATUS testTaiMultiPTPSync(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_32    mode,testMode[3];
	MAD_BOOL multiEn, testMultiEn[3];
	MAD_BOOL en, kpEn;

    MSG_PRINT("Test setting TAI Multi PTP Sync \n");

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

	if (en!=MAD_TRUE)
	{
      MSG_PRINT("Can not enable PTP \n");
	  return MAD_FAIL;
	}
	else
      MSG_PRINT("!!!!!!!! Enable PTP !!!!!!\n");
#endif
    
    nCases = 3;
	testMultiEn[0] = MAD_TRUE;
	testMode[0] = 0x12345;	

	testMultiEn[1] = MAD_TRUE;
	testMode[1] = 0;	

	testMultiEn[2] = MAD_FALSE;
	testMode[2] = 0;	

    for(testCase=0; testCase<nCases; testCase++)
    {   
      retVal = madTaiSetMultiPTPSync(dev,testMultiEn[testCase], testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = madTaiGetMultiPTPSync(dev, &multiEn, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
		if (multiEn != testMultiEn[testCase]) 
        {
            MSG_PRINT("Unexpected return (port %i, test case %i, en %x:%x)\n",
                        (int)0,(int)testCase,(int)multiEn,(int)testMultiEn[testCase]);
            return MAD_FAIL;
        }

		if(mode != testMode[testCase])
      {
        MSG_PRINT("Unexpected return (port %i, test case %i, syncTime %x:%x)\n",
                    (int)0,(int)testCase,(int)mode,(int)testMode[testCase]);
        return MAD_FAIL;
      }
#if 0
        MSG_PRINT(" (port %i, test case %i, syncTime %x:%x)\n",
                    (int)0,(int)testCase,(int)mode,(int)testMode[testCase]);
#endif
    }

    MSG_PRINT("Passed.\n\n");
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}


MAD_STATUS testTaiTimeIncDec(MAD_DEV *dev)
{
    MAD_STATUS retVal;
    MAD_U32   testCase, nCases;
    MAD_32    mode,testMode[4];
	MAD_BOOL isInc, testIsInc[4], expired;
	MAD_BOOL en, kpEn;

    MSG_PRINT("Test setting TAI Multi PTP Sync \n");

#if 1
    retVal = mdPtpGetEnable(dev,&kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpSetEnable(dev, MAD_TRUE);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

    retVal = mdPtpGetEnable(dev,&en);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }

	if (en!=MAD_TRUE)
	{
      MSG_PRINT("Can not enable PTP \n");
	  return MAD_FAIL;
	}
	else
      MSG_PRINT("!!!!!!!! Enable PTP !!!!!!\n");
#endif
    
    nCases = 4;
	testIsInc[0] = MAD_TRUE;
	testMode[0] = 0x33;	

	testIsInc[1] = MAD_TRUE;
	testMode[1] = 0;	

	testIsInc[2] = MAD_FALSE;
	testMode[2] = 0x5a;	

	testIsInc[3] = MAD_FALSE;
	testMode[3] = 0;	

    for(testCase=0; testCase<nCases; testCase++)
    {   
	  if (testIsInc[testCase]==MAD_TRUE) 
        retVal = madTaiSetTimeInc(dev, testMode[testCase]);
	  else
        retVal = madTaiSetTimeDec(dev, testMode[testCase]);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Setting for (port %i, test case %i) returned %x\n",(int)0,(int)testCase, retVal);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
    
      retVal = madTaiGetTimeIncDec(dev, &expired, &isInc, &mode);
      if (retVal != MAD_OK)
      {
        MSG_PRINT("Getting for (port %i, test case %i) returned %x",(int)0,(int)testCase, retVal);
        testMADDisplayStatus(retVal);
        return retVal;             
      }
            
		if (isInc != testIsInc[testCase]) 
        {
            MSG_PRINT("Unexpected return (port %i, test case %i, en %x:%x)\n",
                        (int)0,(int)testCase,(int)isInc,(int)testIsInc[testCase]);
            return MAD_FAIL;
        }

		if(mode != testMode[testCase])
      {
        MSG_PRINT("Unexpected return (port %i, test case %i, expired %x amount %x:%x)\n",
                    (int)0,(int)testCase,(int)expired, (int)mode,(int)testMode[testCase]);
        return MAD_FAIL;
      }
#if 0
      MSG_PRINT(" (port %i, test case %i, isInc %x:%x)\n",
                        (int)0,(int)testCase,(int)isInc,(int)testIsInc[testCase]);
      MSG_PRINT(" (port %i, test case %i, expired %x amount %x:%x)\n",
                    (int)0,(int)testCase,(int)expired,(int)mode,(int)testMode[testCase]);
#endif
    }

    MSG_PRINT("Passed.\n\n");
#if 1
    retVal = mdPtpSetEnable(dev,kpEn);
    if (retVal != MAD_OK)
    {
      testMADDisplayStatus(retVal);
      return retVal;             
    }
#endif
        
    return MAD_OK;
}

MAD_U32 testSyncClkSetClkSelect(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, testCase1, nCases, nCases1;
    MAD_U8    refClk,testRefClk[2];
    MAD_U8    testInterf[3];

    MSG_PRINT("Testing Sync clocking set reference clock...\n");

      nCases = 2;
      testRefClk[0] = MAD_SYNC_CLK_REF_CLK_XTAL;
      testRefClk[1] = MAD_SYNC_CLK_REF_CLK_SCLK;
      nCases1 = 3;
      testInterf[0] = MAD_SYNC_CLK_MODE_PORT_COPPER;
      testInterf[1] = MAD_SYNC_CLK_MODE_PORT_125SERDES;
      testInterf[2] = MAD_SYNC_CLK_MODE_PORT_QSGMII;

    for(port=0; port<dev->numOfPorts; port++)
    {
      for(testCase1=0; testCase1<nCases1; testCase1++)
      {   
        for(testCase=0; testCase<nCases; testCase++)
        {   
            retVal = mdSyncClkSetClkSelect(dev,port,testInterf[testCase1],testRefClk[testCase]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSyncClkGetClkSelect(dev,port,testInterf[testCase1],&refClk);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i:%i) returned ",(int)port,(int)testCase1,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
           
            if (refClk != testRefClk[testCase]) 
            {
                MSG_PRINT("Unexpected return (port %i, test case %i:%i, interface %i, refClk %i:%i)\n",
                            (int)port,(int)testCase1,(int)testCase,(int)testInterf[testCase1],
							(int)refClk,(int)testRefClk[testCase]);
                return MAD_FAIL;
            }
		}
      }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_U32 testSyncClkSetRClk(MAD_DEV *dev)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_U32     testCase, testCase1, nCases, nCases1;
    MAD_U8    rclk,testRclk[2];
    MAD_U8    freq,testFreq[2];

    MSG_PRINT("Testing Sync Clocking Set Recoverd clock...\n");

      nCases = 4;
      testRclk[0] = MAD_SYNC_CLK_RCLK1;
      testRclk[1] = MAD_SYNC_CLK_RCLK2;
      testRclk[2] = MAD_SYNC_CLK_RCLK1_2;
      testRclk[3] = MAD_SYNC_CLK_RCLK_NO;
     nCases1 = 2;
      testFreq[0] = MAD_SYNC_CLK_FREQ_25M;
      testFreq[1] = MAD_SYNC_CLK_FREQ_125M;

    for(port=0; port<dev->numOfPorts; port++)
    {
      for(testCase=0; testCase<nCases; testCase++)
      {   
        for(testCase1=0; testCase1<nCases1; testCase1++)
        {   
            retVal = mdSyncClkSetRClk(dev,port,testRclk[testCase],testFreq[testCase1]);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
    
            retVal = mdSyncClkGetRClk(dev,port,&rclk,&freq);
            if (retVal != MAD_OK)
            {
                MSG_PRINT("Getting for (port %i, test case %i) returned ",(int)port,(int)testCase);
                testMADDisplayStatus(retVal);
                return retVal;             
            }
            
            if ((rclk != testRclk[testCase]) || (freq != testFreq[testCase1]))
            {
                MSG_PRINT("Unexpected return (port %i, test case %i:%i, rclk %i:%i, freq %i:%i)\n",
                            (int)port,(int)testCase,(int)testCase1,(int)rclk,(int)testRclk[testCase],
							(int)freq,(int)testFreq[testCase1]);
                return MAD_FAIL;
            }
		}
      }
    }

    MSG_PRINT("Passed.\n\n");
    return MAD_OK;
}

MAD_STATUS testMAD(MAD_DEV *dev)
{
    testMADCtrl(dev);
    testTempInt(dev);
    testQSGMIIPwD(dev);
	testLinkDownDelay(dev);
	testLinkDownDelayEn(dev);
	testMadLED(dev);
	testCollAdj(dev);
	testSyncFifo(dev);
	testPowerDownEn(dev);
	testQSGMIIPowerDownEn(dev);
	testMACPowerDownEn(dev);
	testDetectPowerDownMode(dev);
	testUniDirTransEn(dev);
	testPatPRBS(dev);
	testPat125PRBS(dev);
	testPtpEnable(dev);
	testPtpIntEnable(dev);
	testPtpConfig(dev);
	testPtpGlobalConfig(dev);
	testPtpPortConfig(dev);
	testPtpPTPEn(dev);
	testTaiEventConfig(dev);
	testTaiTrigConfig(dev);
	testTaiTSClkPer(dev);
	testTaiMultiPTPSync(dev);
	testTaiTimeIncDec(dev);
	testSyncClkSetRClk(dev);
	testSyncClkSetClkSelect(dev);
    return MAD_OK;
}
