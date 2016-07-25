#include <madCopyright.h>
/********************************************************************************
* madMiiSmiIf.c
*
* DESCRIPTION:
*       Includes functions prototypes for initializing and accessing the
*       MII / SMI interface.
*       This is the only file to be included from upper layers.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <madDrvPhyRegs.h>
#include <madHwCntl.h>
#include <madSmiIf.h>
#include <madPlatformDeps.h>
#include <madSem.h>


/*******************************************************************************
* madSmiIfReadRegister
*
* DESCRIPTION:
*       This function reads a register throw the SMI / MII interface, to be used
*       by upper layers.
*
* INPUTS:
*       phyAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*
* OUTPUTS:
*       data        - The register's data.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madSmiIfReadRegister
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      phyAddr,
    IN  MAD_U8      regAddr,
    OUT MAD_U16     *data
)
{
    unsigned int tmpData;

    if(dev->fmadReadMii(dev,(MAD_U32)phyAddr,(MAD_U32)regAddr,&tmpData) != MAD_TRUE)
    {
        return MAD_FAIL;
    }
    *data = (MAD_U16)tmpData;
    return MAD_OK;
}


/*******************************************************************************
* madSmiIfWriteRegister
*
* DESCRIPTION:
*       This function writes to a register throw the SMI / MII interface, to be
*       used by upper layers.
*
* INPUTS:
*       phyAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       data        - The data to be written to the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madSmiIfWriteRegister
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      phyAddr,
    IN  MAD_U8      regAddr,
    IN  MAD_U16     data
)
{
    if(dev->fmadWriteMii(dev,(MAD_U32)phyAddr,(MAD_U32)regAddr,(MAD_U32)data) != MAD_TRUE)
    {
        return MAD_DRV_FAIL_WRITE_REG;
    }
    return MAD_OK;
}


