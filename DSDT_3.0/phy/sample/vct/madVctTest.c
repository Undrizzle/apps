#include <madCopyright.h>
/********************************************************************************
* madVctApi.c
*
* DESCRIPTION:
*       VCT API test functions (madVctTest,madGetExtendedStatus)
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

static
void displayVCTResult
(
    MAD_TEST_STATUS *cableStatus, 
    MAD_CABLE_LEN *cableLen
)
{
    switch(*cableStatus)
    {
        case MAD_TEST_FAIL:
            MSG_PRINT("Unable to test this Cable pair\n");
            break;
        case MAD_NORMAL_CABLE:
            MSG_PRINT("TDR test completed in this Cable pair. No problem found.\n");
            MSG_PRINT("Cable Length of this pair is unknown.\n");
            break;
        case MAD_IMPEDANCE_MISMATCH:
            MSG_PRINT("TDR test completed in this Cable pair with Impedance Mismatch.\n");
            MSG_PRINT("Approximatly %i meters from the tested port.\n",cableLen->errCableLen);
            break;
        case MAD_OPEN_CABLE:
            MSG_PRINT("TDR test completed in this Cable pair. The Cable pairis open.\n");
            MSG_PRINT("Approximatly %i meters from the tested port.\n",cableLen->errCableLen);
            break;
        case MAD_SHORT_CABLE:
            MSG_PRINT("TDR test completed in this Cable pair. Cable is short.\n");
            MSG_PRINT("Approximatly %i meters from the tested port.\n",cableLen->errCableLen);
            break;
        default:
            MSG_PRINT("Unknown Test Result.\n");
            break;
    }
}
static MAD_MDI_MODE orgMdiMode = MAD_AUTO_MDI_MDIX;
static MAD_BOOL     orgSGMIIState = MAD_TRUE;
static MAD_SPEED_MODE   orgSGMIISpeed = MAD_SPEED_1000M;

/* VCT (TDR) */
static    MAD_CABLE_STATUS cableStatus;

MAD_STATUS madVctTest(MAD_DEV *dev,MAD_LPORT port)
{
    MAD_STATUS status;
    int i;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_FAIL;
    }
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
		&&(dev->deviceId!=MAD_88E3082)&&(dev->deviceId!=MAD_88E1121)) 
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
		if((status = mdCopperGetAutoNeg(dev,port,&orgSGMIIState, (MAD_U32 *)&orgSGMIISpeed)) != MAD_OK)
		{
			MSG_PRINT("mdCopperGetAutoNeg return Failed\n");
		    testMADDisplayStatus(status);
			return status;
		}
	}

	/* Disable DTE, if device is MAD_88E1121 */
	if (dev->deviceId==MAD_88E1121)
	{
        status = mdCopperSetDTEDetectEnable(dev,port,MAD_FALSE, 7);  /* dropHys=7 */
        if (status != MAD_OK)
        {
            MSG_PRINT("mdCopperSetDTEDetectEnable return Failed %x ",(int)status);
            testMADDisplayStatus(status);
            return status;             
        }
	}
	
    /*
     *	Start and get Cable Test Result
    */
	do
	{
    if(((status = mdDiagGetCableStatus(dev,port, &cableStatus)) != MAD_OK)
		   && ((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
    {
        testMADDisplayStatus(status);
		if(status == MAD_API_TDRVCT_NOT_SUPPORT )
          MSG_PRINT(("gvctGetCableDiag return Failed. The device does not support VCT!\n"));
		else
          MSG_PRINT(("gvctGetCableDiag return Failed. Check Link down!\n"));

        return status;
    }
	} while ((status&MAD_RUN_ST_MASK)==MAD_PENDING);

    MSG_PRINT("Get status %x\n",(int)status);
    MSG_PRINT("Cable Test Result for Port %i\n",(int)port);

    for(i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        MSG_PRINT("MDI PAIR %i:\n",i);
        displayVCTResult(&cableStatus.cableStatus[i],
                            &cableStatus.cableLen[i]);
    }

	/* Enable DTE, if device is MAD_88E1121 */
	if (dev->deviceId==MAD_88E1121)
	{
        status = mdCopperSetDTEDetectEnable(dev,port,MAD_TRUE, 7);  /* dropHys=7 */
        if (status != MAD_OK)
        {
            MSG_PRINT("mdCopperSetDTEDetectEnable return Failed %x ",(int)status);
            testMADDisplayStatus(status);
            return status;             
        }
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
		&&(dev->deviceId!=MAD_88E3082)&&(dev->deviceId!=MAD_88E1121)) 
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

    status=mdSysSoftReset ( dev, port);
    if (status==MAD_OK)
    {
      MSG_PRINT("Soft resetfor  port %d.\n", (int)port);
    }

    return MAD_OK;
}

/* DSP VCT */
MAD_STATUS madGetExtendedStatus(MAD_DEV *dev,MAD_LPORT port)
{
    MAD_STATUS status;
    MAD_1000BT_EXTENDED_STATUS extendedStatus;
    int i;
    char ch;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_FAIL;
    }

    /*
     * 	Start getting Extended Information.
     */
    if((status = mdDiagGet1000BTExtendedStatus(dev,port, &extendedStatus)) != MAD_OK)
    {
        MSG_PRINT("gvctGetCableDiag return Failed\n");
		testMADDisplayStatus(status);
        return status;
    }

    if (!extendedStatus.isValid)
    {
        MSG_PRINT("Not able to get Extended Status.\n");
        MSG_PRINT("Please check if 1000B-T Link is established on Port %i.\n",(int)port);
        return status;
    }

    /* Pair Polarity */
    MSG_PRINT("Pair Polarity:\n");
    for(i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        MSG_PRINT("MDI PAIR %i: %s\n",i,
                (extendedStatus.pairPolarity[i] == MAD_POSITIVE)?"Positive":"Negative");
    }

    /* Pair Swap */
    MSG_PRINT("Pair Swap:\n");
    for(i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        switch(extendedStatus.pairSwap[i])
        {
            case MAD_CHANNEL_A:
                ch = 'A';
                break;
            case MAD_CHANNEL_B:
                ch = 'B';
                break;
            case MAD_CHANNEL_C:
                ch = 'C';
                break;
            case MAD_CHANNEL_D:
                ch = 'D';
                break;
            default:
                MSG_PRINT("Error: reported unknown Pair Swap %i\n",extendedStatus.pairSwap[i]);
                ch = 'U';
                break;
        }
    
        MSG_PRINT("MDI PAIR %i: Channel %c\n",i,ch);
    }

    /* Pair Polarity */
    MSG_PRINT("Pair Skew:\n");
    for(i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        MSG_PRINT("MDI PAIR %i: %ins\n",i,(int)extendedStatus.pairSkew[i]);
    }

    /* Pair Polarity */
    MSG_PRINT("Cable Len:\n");
    for(i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        MSG_PRINT("MDI PAIR %i: approximately %im\n",i,(int)extendedStatus.cableLen[i]);
    }

    status=mdSysSoftReset ( dev, port);
    if (status==MAD_OK)
    {
      MSG_PRINT("Soft resetfor  port %d.\n", (int)port);
    }
    return MAD_OK;
}


static
void displayAdvVCTResult
(
    MAD_ADV_CABLE_STATUS *cableStatus, 
    int	channel,
    MAD_ADV_VCT_MODE mode
)
{
	int i, j=channel;

	if(mode.mode==MAD_ADV_VCT_OFFSET)
	{
        if(cableStatus->u[j].offset==0)
          MSG_PRINT("Advanced VCT ofsset pair %x test failed\n", j);
        else if(cableStatus->u[j].offset==0x80)
          MSG_PRINT("Advanced VCT ofsset pair %x is 0 \n", j);
		else
          MSG_PRINT("Advanced VCT ofsset pair %x is %d \n", j, cableStatus->u[j].offset);

      return;
	}
	if(mode.mode==MAD_ADV_VCT_SAMPLE_POINT)
	{
        if(cableStatus->u[j].sampleAmp&0x80)
          MSG_PRINT("Advanced VCT sample point return pair %x is -%d \n", j, cableStatus->u[j].sampleAmp&0x7f);
		else
          MSG_PRINT("Advanced VCT sample point return pair %x is +%d \n", j, cableStatus->u[j].sampleAmp&0x7f);

      return;
	}


    switch(cableStatus->cableStatus[channel])
    {
        case MAD_ADV_VCT_FAIL:
            MSG_PRINT("Advanced Cable Test Failed\n");
            break;
        case MAD_ADV_VCT_NORMAL:
            MSG_PRINT("Cable Test Passed. No problem found.\n");
            break;
        case MAD_ADV_VCT_IMP_GREATER_THAN_115:
            MSG_PRINT("Cable Test Passed. Impedance is greater than 115 Ohms.\n");
            MSG_PRINT("Approximatly %i meters from the tested port.\n",cableStatus->u[channel].dist2fault);
            break;
        case MAD_ADV_VCT_IMP_LESS_THAN_85:
            MSG_PRINT("Cable Test Passed. Impedance is less than 85 Ohms.\n");
            MSG_PRINT("Approximatly %i meters from the tested port.\n",cableStatus->u[channel].dist2fault);
            break;
        case MAD_ADV_VCT_OPEN:
            MSG_PRINT("Cable Test Passed. Open Cable.\n");
            MSG_PRINT("Approximatly %i meters from the tested port.\n",cableStatus->u[channel].dist2fault);
            break;
        case MAD_ADV_VCT_SHORT:
            MSG_PRINT("Cable Test Passed. Shorted Cable.\n");
            MSG_PRINT("Approximatly %i meters from the tested port.\n",cableStatus->u[channel].dist2fault);
            break;
        case MAD_ADV_VCT_CROSS_PAIR_SHORT:
            MSG_PRINT("Cable Test Passed.\n");
    		for(i=0; i<MAD_MDI_PAIR_NUM; i++)
			{
				if(cableStatus->u[channel].crossShort.channel[i] == MAD_TRUE)
				{
		            MSG_PRINT("\tCross pair short with channel %i.\n",i);
	            	MSG_PRINT("\tApproximatly %i meters from the tested port.\n",
									cableStatus->u[channel].crossShort.dist2fault[i]);
				}
			}
            break;
        default:
            MSG_PRINT("Unknown Test Result.\n");
            break;
    }
}

/* Advanced VCT (TDR) */
static    MAD_ADV_CABLE_STATUS advCableStatus;

MAD_STATUS madAdvVctTest(MAD_DEV *dev,MAD_LPORT port)
{
  MAD_STATUS status;
  int i, j;
  MAD_ADV_VCT_MODE mode;

  MAD_ADV_VCT_MOD mod[4] = {
    MAD_ADV_VCT_FIRST_PEAK,
    MAD_ADV_VCT_MAX_PEAK,
	/* Currently, do not suppot */
    MAD_ADV_VCT_OFFSET,
    MAD_ADV_VCT_SAMPLE_POINT
	};
  char modeStr[4][32] = {
    "*** ADV_VCT_FIRST_PEAK >>>",
    "*** ADV_VCT_MAX_PEAK >>>",
    "*** ADV_VCT_OFFSET >>>",
    "*** ADV_VCT_SAMPLE_POINT >>>"
  };

  if (dev == 0)
  {
    MSG_PRINT("MAD driver is not initialized\n");
    return MAD_FAIL;
  }

  for (j=0; j<4; j++)
  {
	 mode.mode=mod[j];
 	 mode.transChanSel=MAD_ADV_VCT_TCS_NO_CROSSPAIR;   
/*	 mode.transChanSel=MAD_ADV_VCT_TCS_CROSSPAIR_2;   */
	 mode.sampleAvg = 0;
	 mode.peakDetHyst =0;
	 mode.samplePointDist =0; 
	if(mode.mode==MAD_ADV_VCT_SAMPLE_POINT)
	{
	   mode.samplePointDist =0x50; 
	}

    /*
     *	Start and get Cable Test Result
    */
/*printf("!!!!!!!!! start madAdvVctTest \n");*/
	status = MAD_OK;
	do
	{
    if(((status = mdDiagGetAdvCableStatus(dev,port,
		mode,&advCableStatus)) != MAD_OK)
		   && ((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
    {
        MSG_PRINT(("gvctGetCableDiag return Failed\n"));
		testMADDisplayStatus(status);
        return status;
    }
	} while ((status&MAD_RUN_ST_MASK)==MAD_PENDING);


    MSG_PRINT("^^^ Cable Test Result %s for Port %i\n", modeStr[j], (int)port);

    for(i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        MSG_PRINT("MDI PAIR %i:\n",i);
        displayAdvVCTResult(&advCableStatus, i, mode); 
    }
  }
  status=mdSysSoftReset ( dev, port);
  if (status==MAD_OK)
  {
    MSG_PRINT("Soft resetfor  port %d.\n", (int)port);
  }

  return MAD_OK;
}

static
void _displayAdvVCTResult
(
    MAD_ADV_CABLE_STATUS *cableStatus, 
    int	channel, 
    MAD_ADV_VCT_MODE mode
)
{
	int i, j=channel;

	if(mode.mode==MAD_ADV_VCT_OFFSET)
	{
        if(cableStatus->u[j].offset==0)
          MSG_PRINT("Advanced VCT ofsset pair %x test failed\n", j);
        else if(cableStatus->u[j].offset==0x80)
          MSG_PRINT("Advanced VCT ofsset pair %x is 0 \n", j);
		else
          MSG_PRINT("Advanced VCT ofsset pair %x is %d \n", j, cableStatus->u[j].offset);

      return;
	}

	if(mode.mode==MAD_ADV_VCT_SAMPLE_POINT)
	{
        if(cableStatus->u[j].sampleAmp&0x80)
          MSG_PRINT("Advanced VCT sample point return pair %x is -%d \n", j, cableStatus->u[j].sampleAmp&0x7f);
		else
          MSG_PRINT("Advanced VCT sample point return pair %x is +%d \n", j, cableStatus->u[j].sampleAmp&0x7f);

      return;
	}


   switch(cableStatus->cableStatus[channel])
    {
        case MAD_ADV_VCT_FAIL:
            MSG_PRINT("Advanced Cable Test Failed\n");
            break;
        case MAD_ADV_VCT_NORMAL:
            MSG_PRINT("Cable Test Passed. No problem found.\n");
            break;
        case MAD_ADV_VCT_IMP_GREATER_THAN_115:
            MSG_PRINT("Cable Test Passed. Impedance is greater than 115 Ohms.\n");
            MSG_PRINT("Approximatly %i meters from the tested port.\n",cableStatus->u[channel].dist2fault);
            break;
        case MAD_ADV_VCT_IMP_LESS_THAN_85:
            MSG_PRINT("Cable Test Passed. Impedance is less than 85 Ohms.\n");
            MSG_PRINT("Approximatly %i meters from the tested port.\n",cableStatus->u[channel].dist2fault);
            break;
        case MAD_ADV_VCT_OPEN:
            MSG_PRINT("Cable Test Passed. Open Cable.\n");
            MSG_PRINT("Approximatly %i meters from the tested port.\n",cableStatus->u[channel].dist2fault);
            break;
        case MAD_ADV_VCT_SHORT:
            MSG_PRINT("Cable Test Passed. Shorted Cable.\n");
            MSG_PRINT("Approximatly %i meters from the tested port.\n",cableStatus->u[channel].dist2fault);
            break;
        case MAD_ADV_VCT_CROSS_PAIR_SHORT:
            MSG_PRINT("Cable Test Passed.\n");
    		for(i=0; i<MAD_MDI_PAIR_NUM; i++)
			{
				if(cableStatus->u[channel].crossShort.channel[i] == MAD_TRUE)
				{
		            MSG_PRINT("\tCross pair short with channel %i.\n",i);
	            	MSG_PRINT("\tApproximatly %i meters from the tested port.\n",
									cableStatus->u[channel].crossShort.dist2fault[i]);
				}
			}
            break;
        default:
            MSG_PRINT("Unknown Test Result.\n");
            break;
    }
}



MAD_STATUS testUncompletedVCT (MAD_DEV *dev,MAD_LPORT port)
{
  MAD_STATUS retVal;
  MAD_BOOL onOff;
  MAD_ADV_CABLE_STATUS  advCableStatus;

  MSG_PRINT("Start testUncompletedVCT\n");
 
  retVal = mdCopperSetAutoNeg(dev,port,MAD_TRUE,0);
  if (retVal != MAD_OK)
  {
     MSG_PRINT("Setting for (port %i, test case %i) returned ",(int)port,0);
     testMADDisplayStatus(retVal);
     return retVal;             
  }
  MSG_PRINT("After set mode 0\n");
   retVal = mdCopperGetLinkStatus(dev,port,&onOff);
  if (retVal != MAD_OK)
  {
     MSG_PRINT("Get Link status error %#02x\t",(int)retVal); 
     testMADDisplayStatus(retVal);
  }

  if (onOff==MAD_TRUE)
  {
        MSG_PRINT("Link Up return\n");
        return MAD_OK;
  }
  else
  {
	  int i;
     MAD_ADV_VCT_MODE mode;

  	 mode.mode=MAD_ADV_VCT_MAX_PEAK;
	 mode.transChanSel=MAD_ADV_VCT_TCS_CROSSPAIR_0;  
	 /* mode.transChanSel=MAD_ADV_VCT_TCS_NO_CROSSPAIR; */
	 mode.sampleAvg = 0;
	 mode.peakDetHyst =0;
	 mode.samplePointDist =0; 


     MSG_PRINT("Link down, start advanced TDR\n");
      MSG_PRINT("Link Down, Start Advanced VCT(TDR)\n");
      if((retVal = mdDiagGetAdvCableStatus(dev,port,mode,&advCableStatus)) != MAD_OK)
	  {
        MSG_PRINT("gvctGetAdvCableStatus return Failed\n");
        testMADDisplayStatus(retVal);
		for (i=0; i<4; i++)
		  _displayAdvVCTResult( &advCableStatus, i, mode);
        return MAD_FAIL;
	  }
 		for (i=0; i<4; i++)
		  _displayAdvVCTResult( &advCableStatus, i, mode);
 }
 retVal=mdSysSoftReset ( dev, port);
 if (retVal==MAD_OK)
 {
   MSG_PRINT("Soft resetfor  port %d.\n", (int)port);
 }
 return MAD_OK;
}


/* Alternate VCT */
static    MAD_ALT_CABLE_STATUS altCableStatus;
static
void displayAltVCTResult
(
    MAD_ALT_CABLE_STATUS *cableStatus, 
    int	channel,
	int meter
);

MAD_STATUS madAltVctTest(MAD_DEV *dev,MAD_LPORT port, int meter)
{
  MAD_STATUS status;
  int i, j;
  MAD_U32 diagType;

  MAD_U32 mod[4] = {
	MAD_ALT_VCT_RUN_AUTONEGO_CYCLE_BIT | MAD_ALT_VCT_DISABLE_CROSS_PAIR_BIT | MAD_ALT_VCT_RUN_AFTER_BREAK_LNK_BIT,
	MAD_ALT_VCT_RUN_AUTONEGO_CYCLE_BIT,
	MAD_ALT_VCT_DISABLE_CROSS_PAIR_BIT,
	MAD_ALT_VCT_RUN_AFTER_BREAK_LNK_BIT
	};
  char modeStr[4][64] = {
/*    "VCT_RUN_AUTONEGO_CYCLE | DISABLE_CROSS_PAIR | RUN_AFTER_BREAK_LNK", */
    "VCT AUTONEGO_CYCLE | DISABLE_CROSS_PAIR | AFTER_BREAK_LNK", 
	"*** ALT_VCT_RUN_AUTONEGO_CYCLE >>>",
	"*** ALT_VCT_DISABLE_CROSS_PAIR >>>",
	"*** ALT_VCT_RUN_AFTER_BREAK_LNK >>>"
  };

  if (meter)
    altCableStatus.cableLenUnit = MAD_ALT_VCT_CABLE_LENGTH_UNIT_BIT; /* lenth unit is meter */
  else
	altCableStatus.cableLenUnit = 0; /* lenth unit is centimeter */

  if (dev == 0)
  {
    MSG_PRINT("MAD driver is not initialized\n");
    return MAD_FAIL;
  }

  for (j=0; j<4; j++)
  {
	 diagType=mod[j];

    /*
     *	Start and get Cable Test Result
    */
	status = MAD_OK;
	do
	{
    if(((status = mdDiagGetAltCableStatus(dev,port,
		diagType,&altCableStatus)) != MAD_OK)
		   && ((status&MAD_RUN_ST_MASK)!=MAD_PENDING))
    {
        MSG_PRINT(("mdDiagGetAltCableStatus return Failed\n"));
		testMADDisplayStatus(status);
        return status;
    }
	} while ((status&MAD_RUN_ST_MASK)==MAD_PENDING);


    MSG_PRINT("^^^ Cable Test Result %s for Port %i\n", modeStr[j], (int)port);

    for(i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        MSG_PRINT("MDI PAIR %i:\n",i);
        displayAltVCTResult(&altCableStatus, i, meter); 
    }
  }
  status=mdSysSoftReset ( dev, port);
  if (status==MAD_OK)
  {
    MSG_PRINT("Soft resetfor  port %d.\n", (int)port);
  }

  return MAD_OK;
}

static
void displayAltVCTResult
(
    MAD_ALT_CABLE_STATUS *cableStatus, 
    int	channel,
	int meter
)
{

    switch(cableStatus->cableFaultCode[channel])
    {
        case MAD_ALT_VCT_FAULT_INVALID:
            MSG_PRINT("Alternate VCT Cable Test passed, Fault invalid\n");
            break;
        case MAD_ALT_VCT_FAULT_PAIR_OK:
            MSG_PRINT("Cable Test Passed. Pair Ok.\n");
            break;
        case MAD_ALT_VCT_FAULT_PAIR_OPEN:
            MSG_PRINT("Cable Test Passed. Cable open.\n");
            break;
        case MAD_ALT_VCT_FAULT_SAME_PAIR_SHORT:
            MSG_PRINT("Cable Test Passed. Same pair short\n");
            break;
        case MAD_ALT_VCT_FAULT_CROSS_PAIR_SHORT:
            MSG_PRINT("Cable Test Passed. Cross pair short.\n");
            break;
        case MAD_ALT_VCT_FAULT_PAIR_BUSY:
            MSG_PRINT("Cable Test Passed. Fault pair busy.\n");
            break;
        default:
            MSG_PRINT("Unknown Test Result. %x\n", cableStatus->cableFaultCode[channel]);
            break;
    }

    if(meter)
   	  MSG_PRINT("\tApproximatly cable length %i m .\n", cableStatus->cableLen[channel]);
	else
   	  MSG_PRINT("\tApproximatly cable length %i cm .\n", cableStatus->cableLen[channel]);
}



