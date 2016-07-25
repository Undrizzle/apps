#include <madCopyright.h>

/********************************************************************************
* madVctExt.c
* 
* DESCRIPTION:
*       APIs for VCT (DSP).
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <madApi.h>
#include <madApiInternal.h>
#include <madHwCntl.h>
#include <madDrvPhyRegs.h>
#include <madDrvConfig.h>
#include "madSMVct.h"

#define MAD_LOOKUP_TABLE_ENTRY  128  /* 73 */

#define MAD_ADV_VCT_ACCEPTABLE_SHORT_CABLE  11

static  MAD_U8 tbl_1181[MAD_LOOKUP_TABLE_ENTRY] = 
                    {  2,  4,  8, 14, 18, 20, 25, 30, 33, 36,
                      39, 42, 46, 48, 51, 54, 57, 59, 62, 64,
                      66, 69, 71, 73, 75, 77, 80, 81, 83, 85,
                      87, 88, 90, 93, 95, 97, 98,100,101,103,
                     104,106,106,107,109,110,111,113,114,115,
                     116,118,119,120,121,122,124,125,126,127,
                     128,129,130,131,132,133,134,135,136,137,
                     138,139,140};

static  MAD_U8 tbl_1111[MAD_LOOKUP_TABLE_ENTRY] = 
                    {  0,  2,  4, 5, 6, 9, 13, 17, 20, 23,
                      27, 30, 33, 35, 38, 41, 43, 46, 48, 51,
                      53, 55, 58, 60, 62, 64, 66, 68, 70, 72,
                      73, 75, 77, 79, 80, 82, 84, 85, 87, 88,
                      90, 91, 93, 94, 96, 97, 98,100,101,102,
                     104,105,106,107,109,110,111,112,113,114,
                     116,117,118,119,120,121,122,123,124,125,
                     126,127,128,129,130,131,132,133,134,134};

static  MAD_U8 tbl_1112[MAD_LOOKUP_TABLE_ENTRY] =   /* from 17*/
                    {  0,  4,  8, 11, 14, 18, 21, 24, 28, 31, 
                      34, 37, 39, 42, 44, 47, 49, 52, 54, 56, 
                      58, 60, 62, 64, 66, 68, 70, 72, 74, 75, 
                      77, 79, 80, 82, 83, 85, 87, 88, 89, 91, 
                      92, 94, 95, 96, 98, 99,100,101,103,104,
                      105,106,107,108,109,111,112,113,114,115,
                      116,117,118,119,120,121,122,123,124,124,
                      125,126,127,128,129,130,131,131,132,133,
                      134,135,135,136,137,138,139,139,140,141,
                      142,142,143,144,144,145,146,147,147,148};

static  MAD_U8 tbl_1116[MAD_LOOKUP_TABLE_ENTRY] =   /* from 16*/
                    {  2,  4,  8, 14, 18, 20, 25, 30, 33, 36, 
                      39, 42, 46, 48, 51, 54, 57, 59, 62, 64, 
                      66, 69, 71, 73, 75, 77, 80, 81, 83, 85, 
                      87, 88, 90, 93, 95, 97, 98, 100, 101, 103, 
                      104,106,106,107,109,110,111,113,114,115,
                      116,118,119,120,121,122,124,125,126,127,
                      128,129,130,131,132,133,134,135,136,137,
                      138,139,140};

static  MAD_U8 tbl_1240[MAD_LOOKUP_TABLE_ENTRY] = 
                    {  1,  2,  5, 10, 13, 15, 18, 22, 26, 30, 
                      33, 35, 38, 40, 43, 45, 48, 51, 53, 55, 
                      58, 60, 63, 65, 68, 69, 70, 71, 73, 75, 
                      77, 79, 80, 81, 82, 83, 85, 87, 88, 90, 
                      91, 92, 93, 95, 97, 98,100,101,102,103,
                     105,106,107,108,109,110,111,112,113,114,
                     115,116,117,118,119,120,121,122,123,124,
                     125,126,127,128,129,130};


/*******************************************************************************
* dspLookup
*
* DESCRIPTION:
*       This routine returns cable length (meters) by reading DSP Lookup table.
*
* INPUTS:
*       regValue - register 21
*
* OUTPUTS:
*       cableLen - cable length (unit of meters).
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static
MAD_STATUS dspLookup
(
    IN  MAD_DEV *dev,
    IN  MAD_U16 regValue, 
    OUT MAD_32  *cableLen
)
{
    MAD_U16 startEntry,tableEntry;
    MAD_U8* tbl;
    switch(dev->phyInfo.exStatusType)
    {
        case MAD_PHY_EX_STATUS_TYPE1:    /* 88E1111/88E1141/E1145 */
            startEntry = 18-1;
            tableEntry = 80;
            tbl = tbl_1111;
            break;

        case MAD_PHY_EX_STATUS_TYPE2:    /* 88E1112 */
         if(dev->deviceId==MAD_88E1181)
		 {	 /* 88E1181 */
            startEntry = 16;
            tableEntry = 73;
            tbl = tbl_1181;
		 }
		 else if(dev->deviceId==MAD_88E1112)
		 {	 /* 88E1112 */
            startEntry = 17;
            tableEntry = 100;
            tbl = tbl_1112;
		 }
         else
         {
            startEntry = 16;
            tableEntry = 73;
            tbl = tbl_1116;
		 }
            break;

        case MAD_PHY_EX_STATUS_TYPE3:   /* 88E6165 Internal Phy */
/*              startEntry = 18;  G65G */
                startEntry = 21;
            tableEntry = 76;
            tbl = tbl_1240;
            break;

        default:
            return MAD_API_DSPVCT_NOT_SUPPORT;
    }

    if (tbl ==NULL)
    {
        *cableLen = -1;
        return MAD_OK;
    }

    if (regValue < startEntry)
    {
        *cableLen = 0;
        return MAD_OK;
    }

    if (regValue >= (tableEntry+startEntry-1))
    {
        regValue = tableEntry-1;
    }
    else
    {
        regValue -= startEntry;
    }

    *cableLen = (MAD_32)tbl[regValue];
    return MAD_OK;
}

/*******************************************************************************
* getDSPDistance_1111
*
* DESCRIPTION:
*       This routine returns cable length (meters) from DSP method.
*       This routine is for the 88E1111 like devices.
*
* INPUTS:
*       mdi - pair of each MDI (0..3).
*
* OUTPUTS:
*       cableLen - cable length (unit of meters).
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static
MAD_STATUS getDSPDistance_1111
(
    IN  MAD_DEV *dev,
    IN  MAD_U8  hwPort,
    IN  MAD_U32 mdi,
    OUT MAD_32 *cableLen
)
{
    MAD_U16     data, pageNum;
    MAD_STATUS  retVal;

    MAD_DBG_INFO("getDSPDistance Called.\n");

    pageNum = 0x8754 + (MAD_U16)((mdi << 12)&0xf000);

    if((retVal = madHwReadPagedPhyReg(dev,hwPort,pageNum,31,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading length of MDI pair failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    return dspLookup(dev,data,cableLen);
}


/*******************************************************************************
* getDSPDistance_1181
*
* DESCRIPTION:
*       This routine returns cable length (meters) from DSP method.
*       This routine is for the 88E1181 like devices.
*
* INPUTS:
*       mdi - pair of each MDI (0..3).
*
* OUTPUTS:
*       cableLen - cable length (unit of meters).
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static
MAD_STATUS getDSPDistance_1181
(
    IN  MAD_DEV *dev,
    IN  MAD_U8  hwPort,
    IN  MAD_U32 mdi,
    OUT MAD_32 *cableLen
)
{
    MAD_U16     data, retryCount;
    MAD_STATUS  retVal;

    MAD_DBG_INFO("getDSPDistance Called.\n");

    /* Set the required bits for Cable length register */
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,0xff,19,(MAD_U16)(0x1018+(0xff&mdi)))) != MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    retryCount = 1000;

    do
    {
        if(retryCount == 0)
        {
            MAD_DBG_ERROR("Ready bit of Cable length resiter is not set.\n");
            return MAD_API_CABLE_LEN_NOT_READY;
        }

        /* Check the ready bit of Cable length register */
        if((retVal = madHwGetPagedPhyRegField(dev,hwPort,0xff,19,15,1,&data)) != MAD_OK)
        {
            MAD_DBG_ERROR("Writing to paged phy reg failed.\n");
            return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
        }

        retryCount--;

    } while(!data);

    /* read length of MDI pair */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,0xff,21,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR("Reading length of MDI pair failed.\n");
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
    }

    retVal= dspLookup(dev,data,cableLen);
    return retVal;
}

/*******************************************************************************
* getDSPDistance_1240
*
* DESCRIPTION:
*       This routine returns cable length (meters) from DSP method.
*       This routine is for the 88E1181 like devices.
*
* INPUTS:
*       mdi - pair of each MDI (0..3).
*
* OUTPUTS:
*       cableLen - cable length (unit of meters).
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static
MAD_STATUS getDSPDistance_1240
(
    IN  MAD_DEV *dev,
    IN  MAD_U8  hwPort,
    IN  MAD_U32 mdi,
    OUT MAD_32 *cableLen
)
{
    MAD_U16     data, retryCount;
    MAD_STATUS  retVal;

    MAD_DBG_INFO(("getDSPDistance Called.\n"));

    /* Set the required bits for Cable length register */
    if((retVal = madHwWritePagedPhyReg(dev,hwPort,0xff,16,(MAD_U16)(0x1118+(0xff&mdi)))) != MAD_OK)
    {
        MAD_DBG_INFO(("Writing to paged phy reg failed.\n"));
        return retVal;
    }

    retryCount = 1000;

    do
    {
        if(retryCount == 0)
        {
            MAD_DBG_INFO(("Ready bit of Cable length resiter is not set.\n"));
            return MAD_FAIL;
        }

        /* Check the ready bit of Cable length register */
        if((retVal = madHwGetPagedPhyRegField(dev,hwPort,0xff,16,15,1,&data)) != MAD_OK)
        {
            MAD_DBG_INFO(("Writing to paged phy reg failed.\n"));
            return retVal;
        }

        retryCount--;

    } while(!data);

    /* read length of MDI pair */
    if((retVal = madHwReadPagedPhyReg(dev,hwPort,0xff,18,&data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Reading length of MDI pair failed.\n"));
        return retVal;
    }

    return dspLookup(dev,data,cableLen);
}




/*******************************************************************************
* getExtDistance
*
* DESCRIPTION:
*       This routine retrieves Pair Skew, Pair Swap, and Pair Polarity
*        for 1000M phy with multiple page mode
*
* INPUTS:
*       dev - device context.
*       port - logical port number.
*
* OUTPUTS:
*       extendedStatus - extended cable status.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static MAD_STATUS getExtDistance
(
    IN  MAD_DEV         *dev,
    IN  MAD_U8            hwPort,
    OUT MAD_1000BT_EXTENDED_STATUS *extendedStatus
)
{
    MAD_STATUS retVal;
    MAD_U16 i;

   extendedStatus->isValid = MAD_FALSE;
    /* DSP based cable length */
    switch(dev->phyInfo.exStatusType)
    {
        case MAD_PHY_EX_STATUS_TYPE1:
            for (i=0; i<MAD_MDI_PAIR_NUM; i++)
            {
                if((retVal = getDSPDistance_1111(dev,hwPort,i,&extendedStatus->cableLen[i])) != MAD_OK)
                {
                    MAD_DBG_ERROR("getDSPDistance failed.\n");
                    return (retVal | MAD_API_FAIL_EXTVCT_DIST);
                }
            }
            break;
        case MAD_PHY_EX_STATUS_TYPE2:
            for (i=0; i<MAD_MDI_PAIR_NUM; i++)
            {
                if((retVal = getDSPDistance_1181(dev,hwPort,i,&extendedStatus->cableLen[i])) != MAD_OK)
                {
                    MAD_DBG_ERROR("getDSPDistance failed.\n");
                    return (retVal | MAD_API_FAIL_EXTVCT_DIST);
                }
            }
            break;

        case MAD_PHY_EX_STATUS_TYPE3:
            for (i=0; i<MAD_MDI_PAIR_NUM; i++)
            {
                if((retVal = getDSPDistance_1240(dev,hwPort,i,&extendedStatus->cableLen[i])) != MAD_OK)
                {
                    MAD_DBG_INFO(("getDSPDistance failed.\n"));
                    return retVal;
                }
            }
            break;

       default:
            return MAD_API_DSPVCT_NOT_SUPPORT;
    }

    return MAD_OK;
}



/*******************************************************************************
* getExtStatus_reg28
*
* DESCRIPTION:
*       This routine retrieves Pair Skew, Pair Swap, and Pair Polarity
*        for 1000M phy through register 28 in page 5 and page 4 .
*
* INPUTS:
*       dev - device context.
*       port - logical port number.
*
* OUTPUTS:
*       extendedStatus - extended cable status.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 4, Reg 28, Bit 15:0 : 1000B-T Pair Skew Register
*       Page 5, Reg 28, Bit 6:0  : 1000B-T Pair Swap and Polarity
*
*******************************************************************************/
static MAD_STATUS getExtStatus_reg28
(
    IN  MAD_DEV         *dev,
    IN  MAD_U8           hwPort,
    OUT MAD_1000BT_EXTENDED_STATUS *extendedStatus
)
{
    MAD_STATUS status;
    MAD_U16 u16Data, i;

    /*
     * get data from 28_5 register
     */
    if((status= madHwReadPagedPhyReg(dev,hwPort,5,28,&u16Data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Not able to read a Phy register.\n"));
        return status;
    }

    /* if bit 6 is not set, it's not valid. */
    if (!(u16Data & 0x0040))
    {
        MAD_DBG_INFO("Valid Bit is not set (%x).\n", u16Data);
        extendedStatus->isValid = MAD_FALSE;
        return MAD_OK;
    }

    extendedStatus->isValid = MAD_TRUE;
    
    /* get Pair Polarity */
    for(i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        switch((u16Data >> i) & 0x1)
        {
            case 0:
                extendedStatus->pairPolarity[i] = MAD_POSITIVE;
                break;
            default:
                extendedStatus->pairPolarity[i] = MAD_NEGATIVE;
                break;
        }
    }

    /* get Pair Swap */
    for(i=0; i<MAD_CHANNEL_PAIR_NUM; i++)
    {
        switch((u16Data >> (i+4)) & 0x1)
        {
            case 0:
                extendedStatus->pairSwap[i] = MAD_STRAIGHT_CABLE;
                break;
            default:
                extendedStatus->pairSwap[i] = MAD_CROSSOVER_CABLE;
                break;
        }
    }

    /*
     * get data from 28_4 register
     */
    if((status= madHwReadPagedPhyReg(dev,hwPort,4,28,&u16Data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Not able to read a Phy register.\n"));
        return status;
    }

    /* get Pair Skew */
    for(i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        extendedStatus->pairSkew[i] = ((u16Data >> i*4) & 0xF) * 8;
    }

    return MAD_OK;
}


/*******************************************************************************
* getExtStatusPage5
*
* DESCRIPTION:
*       This routine retrieves Pair Skew, Pair Swap, and Pair Polarity
*        for 1000M phy with page 5 regigster 21, and 20
*
* INPUTS:
*       dev - device context.
*       port - logical port number.
*
* OUTPUTS:
*       extendedStatus - extended cable status.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 5, Reg 20, Bit 15:0 : 1000B-T Pair Skew Register
*       Page 5, Reg 21, Bit 6:0  : 1000B-T Pair Swap and Polarity
*
*******************************************************************************/
static MAD_STATUS getExtStatusPage5
(
    IN  MAD_DEV         *dev,
    IN  MAD_U8            hwPort,
    OUT MAD_1000BT_EXTENDED_STATUS *extendedStatus
)
{
    MAD_STATUS status;
    MAD_U16 u16Data, i;

    /*
     * get data from 21_5 register for pair swap
     */
    if((status= madHwReadPagedPhyReg(dev,hwPort,5,21,&u16Data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Not able to read a paged Phy register.\n"));
        return status;
    }

    /* if bit 6 is not set, it's not valid. */
    if (!(u16Data & 0x0040))
    {
        MAD_DBG_INFO("Valid Bit is not set (%0#x).\n", u16Data);
        extendedStatus->isValid = MAD_FALSE;
        return MAD_OK;
    }

    extendedStatus->isValid = MAD_TRUE;
    
    /* get Pair Polarity */
    for(i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        switch((u16Data >> i) & 0x1)
        {
            case 0:
                extendedStatus->pairPolarity[i] = MAD_POSITIVE;
                break;
            default:
                extendedStatus->pairPolarity[i] = MAD_NEGATIVE;
                break;
        }
    }

    /* get Pair Swap */
    for(i=0; i<MAD_CHANNEL_PAIR_NUM; i++)
    {
        switch((u16Data >> (i+4)) & 0x1)
        {
            case 0:
                extendedStatus->pairSwap[i] = MAD_STRAIGHT_CABLE;
                break;
            default:
                extendedStatus->pairSwap[i] = MAD_CROSSOVER_CABLE;
                break;
        }
    }

    /*
     * get data from 20_5 register for pair skew
     */
    if((status= madHwReadPagedPhyReg(dev,hwPort,5,20,&u16Data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Not able to read a paged Phy register.\n"));
        return status;
    }

    /* get Pair Skew */
    for(i=0; i<MAD_MDI_PAIR_NUM; i++)
    {
        extendedStatus->pairSkew[i] = ((u16Data >> i*4) & 0xF) * 8;
    }

    return MAD_OK;
}


/*******************************************************************************
* mdDiagGet1000BTExtStatus
*
* DESCRIPTION:
*       This routine retrieves Pair Skew, Pair Swap, and Pair Polarity
*
* INPUTS:
*       dev - device context.
*       port - logical port number.
*
* OUTPUTS:
*       extendedStatus - extended cable status.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
static MAD_STATUS mdDiagGet1000BTExtStatus
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT        port,
    OUT MAD_1000BT_EXTENDED_STATUS *extendedStatus
)
{
	MAD_STATUS status;
    MAD_U8 hwPort;

    MAD_DBG_INFO("madDiagGet1000BTExtStatus_o Called.\n");

    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY(dev, MAD_PHY_EX_CABLE_STATUS) ||
        !MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("VCT not supported.\n");
        return MAD_API_DSPVCT_NOT_SUPPORT;
    }

    
    switch(dev->phyInfo.exStatusType)
    {
        case MAD_PHY_EX_STATUS_TYPE1:
            status = getExtStatus_reg28(dev,hwPort,extendedStatus);
            break;
        case MAD_PHY_EX_STATUS_TYPE2:
        case MAD_PHY_EX_STATUS_TYPE3:
        case MAD_PHY_EX_STATUS_TYPE4:
            status = getExtStatusPage5(dev,hwPort,extendedStatus);
            break;
        default:
               MAD_DBG_INFO(("Device is not supporting Extended Cable Status.\n"));
            status = MAD_API_DSPVCT_NOT_SUPPORT;
    }

    return status;
}




/*******************************************************************************
* mdDiagGet1000BTExtendedStatus
*
* DESCRIPTION:
*       This routine retrieves extended cable status, such as Pair Poloarity,
*        Pair Swap, and Pair Skew. Note that this routine will be success only
*        if 1000Base-T Link is up.
*        Note: Since DSP based cable length in extended status is based on 
*             constants from test results. At present, only E1181, E1111, E1240, and
*             E1112 are available.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - logical port number.
*
* OUTPUTS:
*       extendedStatus - the extended cable status.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15    : Soft reset 
*
*******************************************************************************/
MAD_STATUS mdDiagGet1000BTExtendedStatus
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT   port,
    OUT MAD_1000BT_EXTENDED_STATUS *extendedStatus
)
{
    MAD_STATUS retVal;
    MAD_U8 hwPort;

    MAD_DBG_INFO("madDiagGet1000BTExtendedStatus Called.\n");

    hwPort = MAD_LPORT_2_PORT(port);

    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY(dev, MAD_PHY_EX_CABLE_STATUS) ||
        !MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("VCT not supported.\n");
        return MAD_API_DSPVCT_NOT_SUPPORT;
    }

    switch(dev->phyInfo.exStatusType)
    {
        case MAD_PHY_EX_STATUS_TYPE1:
        case MAD_PHY_EX_STATUS_TYPE2:
        case MAD_PHY_EX_STATUS_TYPE3:
           if((retVal = getExtDistance(dev,hwPort,extendedStatus)) != MAD_OK)
            {
                MAD_DBG_ERROR("Getting Extanded Cable Status failed.\n");
                break;
            }

            break;
        case MAD_PHY_EX_STATUS_TYPE4:
            break;

        default:
            return MAD_API_DSPVCT_NOT_SUPPORT;
    }
    return mdDiagGet1000BTExtStatus(dev, port, extendedStatus);
}

