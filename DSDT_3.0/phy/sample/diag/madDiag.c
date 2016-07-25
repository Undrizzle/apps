#include <madCopyright.h>
/********************************************************************************
* madDiag.c
*
* DESCRIPTION:
*       Diag test procedures
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
#include "madHwCntl.h"


MAD_STATUS madRdReg(MAD_DEV *dev,MAD_LPORT hwPort, MAD_U16 pageNum, MAD_U16 regNum, MAD_U32 *data)
{
    MAD_STATUS status;
    
    status=mdSysGetPagedPhyReg ( dev, hwPort, pageNum, regNum, data);
    if (status==MAD_OK)
    {
      *data &= 0xffff; 
      MSG_PRINT("Reading page %d port %d register %d : 0x%04x.\n", (int)pageNum, (int)hwPort, (int)regNum, (int)*data);
    }
   else
        testMADDisplayStatus(status);

    return status;
}

MAD_STATUS madWrReg(MAD_DEV *dev,MAD_LPORT hwPort, MAD_U16 pageNum, MAD_U16 regNum, MAD_U16 data)
{
    MAD_STATUS status;

    MSG_PRINT("Writinging page %d port %d register %d for 0x%04x.\n", (int)pageNum, (int)hwPort, (int)regNum, (int)data);
    status=mdSysSetPagedPhyReg ( dev, hwPort, pageNum, regNum, data);
    if (status==MAD_OK)
    {
      MSG_PRINT("Writing page %d port %d register %d : 0x%04x.\n", (int)pageNum, (int)hwPort, (int)regNum, (int)data);
    }
   else
    testMADDisplayStatus(status);
    return status;
}

MAD_STATUS madDispRegs(MAD_DEV *dev,MAD_LPORT hwPort, 
                          MAD_U16 pageNum)
{
    MAD_STATUS status;
    
    int i;

    if (dev == 0)
    {
        MSG_PRINT("MAD driver is not initialized\n");
        return MAD_API_ERR_DEV;
    }

  MSG_PRINT("Read page %d port %d: \n", (int)pageNum, (int)hwPort);

  for (i=0; i<32; i++)
  {
    MAD_U16 data;
    if((status = madHwReadPagedPhyReg(dev,hwPort,pageNum, i, &data)) != MAD_OK)
    {
        MSG_PRINT("Reading page %d  port %d register %d failed.\n", (int)pageNum, (int)hwPort, i);
        testMADDisplayStatus(status);
        return MAD_API_ERR_DEV;
    }
    if ((i+1)%4)
      MSG_PRINT("reg %02d: 0x%04x    ", i, (int)data);
    else
      MSG_PRINT("reg %02d: 0x%04x\n", i, (int)data);
  }
  MSG_PRINT("\n");
      return MAD_OK;
}


MAD_STATUS madSoftReset(MAD_DEV *dev,MAD_LPORT hwPort) 
{
    MAD_STATUS status;
    
    status=mdSysSoftReset ( dev, hwPort);
    if (status==MAD_OK)
    {
      MSG_PRINT("Soft resetfor  port %d.\n", (int)hwPort);
    }
   else
        testMADDisplayStatus(status);

    return status;
}

