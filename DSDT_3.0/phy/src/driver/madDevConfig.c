#include <madCopyright.h>

/********************************************************************************
* madDevConfig.h
*
* DESCRIPTION:
*       Includes device specific configurations.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <madDrvPhyRegs.h>
#include <madDevConfig.h>
#include <madSmiIf.h>
#include <madHwCntl.h>
#include <madApiDefs.h>


/*******************************************************************************
* runOnce88E1181
*
* DESCRIPTION:
*       This function configures 88E1181 Device to function properly.
*       As specified in App Notes, internal AVDD is adjusted.
*
* INPUTS:
*        None.
* OUTPUTS:
*        None.
*
* RETURNS:
*       MAD_OK               - on success, or
*       MAD_FAIL             - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static MAD_STATUS runOnce88E1181
(
    IN MAD_DEV    *dev
)
{
    MAD_STATUS  retVal;
    MAD_U8      port;

    MAD_DBG_INFO("runOnce88E1181 called\n");

    for(port=0; port<dev->numOfPorts; port++)
    {
        /* REG22 = 0xFF; REG24 = 0xCAA0 */
        if((retVal=madHwWritePagedPhyReg(dev,port,0xFF,24,0xCAA0)) != MAD_OK)
        {
            MAD_DBG_ERROR("SMI Paged Write failed (port%i, reg24)\n",port);
            return (retVal | MAD_DRV_FAIL_WRITE_PAGED_REG);
        }

        /* REG22 = 0xFF; REG23 = 0x2003 */
        if((retVal=madHwWritePagedPhyReg(dev,port,0xFF,23,0x2003)) != MAD_OK)
        {
            MAD_DBG_ERROR("SMI Paged Write failed (port%i, reg23)\n",port);
            return (retVal | MAD_DRV_FAIL_WRITE_PAGED_REG);
        }

        /* if the device is 88E1181 Rev E.0, run the followings. */
        if (dev->revision > 3)
        {
            /* Amplitude Theshold Setup. */

            /* Page 9, REG20 = 0x0F12 */
            if((retVal=madHwWritePagedPhyReg(dev,port,9,20,0x0F12)) != MAD_OK)
            {
                MAD_DBG_ERROR("SMI Paged Write failed (port%i, reg23)\n",port);
                return (retVal | MAD_DRV_FAIL_WRITE_PAGED_REG);
            }

            /* Page 9, REG21 = 0x0708 */
            if((retVal=madHwWritePagedPhyReg(dev,port,9,21,0x0708)) != MAD_OK)
            {
                MAD_DBG_ERROR("SMI Paged Write failed (port%i, reg23)\n",port);
                return (retVal | MAD_DRV_FAIL_WRITE_PAGED_REG);
            }

            /* Page 9, REG23 = 0x0005 */
            if((retVal=madHwWritePagedPhyReg(dev,port,9,23,0x0005)) != MAD_OK)
            {
                MAD_DBG_ERROR("SMI Paged Write failed (port%i, reg23)\n",port);
                return (retVal | MAD_DRV_FAIL_WRITE_PAGED_REG);
            }
        }

        /* reset page register */
        if((retVal=madHwWritePhyReg(dev,port,22,0)) != MAD_OK)
        {
            MAD_DBG_ERROR("SMI Paged Write failed (port%i, reg24)\n",port);
            return (retVal | MAD_DRV_FAIL_WRITE_PAGED_REG);
        }

    }

    return MAD_OK;
}

static MAD_STATUS runOnce88E104X
(
    IN MAD_DEV    *dev
)
{
    MAD_DBG_ERROR("Pre-Initialization is done for the device %#x.\n",dev->deviceId);
    return MAD_OK;
}
static MAD_STATUS runOnce88E1111
(
    IN MAD_DEV    *dev
)
{
    MAD_DBG_ERROR("Pre-Initialization is done for the device %#x.\n",dev->deviceId);
    return MAD_OK;
}
static MAD_STATUS runOnce88E1112
(
    IN MAD_DEV    *dev
)
{
    MAD_DBG_ERROR("Pre-Initialization is done for the device %#x.\n",dev->deviceId);
    return MAD_OK;
}
static MAD_STATUS runOnce88E1116
(
    IN MAD_DEV    *dev
)
{
    MAD_DBG_ERROR("Pre-Initialization is done for the device %#x.\n",dev->deviceId);
    return MAD_OK;
}
static MAD_STATUS runOnce88E114X
(
    IN MAD_DEV    *dev
)
{
    MAD_DBG_ERROR("Pre-Initialization is done for the device %#x.\n",dev->deviceId);
    return MAD_OK;
}
static MAD_STATUS runOnce88E3016
(
    IN MAD_DEV    *dev
)
{
    MAD_DBG_ERROR("Pre-Initialization is done for the device %#x.\n",dev->deviceId);
    return MAD_OK;
}
static MAD_STATUS runOnce88E3082
(
    IN MAD_DEV    *dev
)
{
    MAD_DBG_ERROR("Pre-Initialization is done for the device %#x.\n",dev->deviceId);
    return MAD_OK;
}
static MAD_STATUS runOnce88E1121
(
    IN MAD_DEV    *dev
)
{
    MAD_DBG_ERROR("Pre-Initialization is done for the device %#x.\n",dev->deviceId);
    return MAD_OK;
}
static MAD_STATUS runOnce88E1149
(
    IN MAD_DEV    *dev
)
{
    MAD_STATUS  retVal;
    MAD_U8      port;

    MAD_DBG_INFO("runOnce88E1149 called\n");

    for(port=0; port<dev->numOfPorts; port++)
    {
        /* REG22 = 0xFF; REG24 = 0x1111 */
        if((retVal=madHwWritePagedPhyReg(dev,port,0xFF,24,0x1111)) != MAD_OK)
        {
            MAD_DBG_ERROR("SMI Paged Write failed (port%i, reg24)\n",port);
            return retVal;
        }

        /* REG22 = 0xFF; REG23 = 0x2012 */
        if((retVal=madHwWritePagedPhyReg(dev,port,0xFF,23,0x2012)) != MAD_OK)
        {
            MAD_DBG_ERROR("SMI Paged Write failed (port%i, reg23)\n",port);
            return retVal;
        }

    }

    return MAD_OK;
}
static MAD_STATUS runOnce88E1240
(
    IN MAD_DEV    *dev
)
{
    MAD_DBG_ERROR("Pre-Initialization is done for the device %#x.\n",dev->deviceId);
    return MAD_OK;
}

/*******************************************************************************
* madDevPreInit
*
* DESCRIPTION:
*       This function configures Marvell Phy Device to function properly.
*       This function applied only once when driver is loaded. 
*
* INPUTS:
*        None.
* OUTPUTS:
*        None.
*
* RETURNS:
*       MAD_OK               - on success, or
*       MAD_FAIL             - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS madDevPreInit
(
    IN MAD_DEV    *dev
)
{
    MAD_DBG_INFO("madDriverPreInit called\n");
    switch (dev->deviceId)
    {
        case MAD_88E10X0:   
        case MAD_88E10X0S:   
        case MAD_88E1011:   
        case MAD_88E104X:
                return runOnce88E104X(dev);
        case MAD_88E1111:
                return runOnce88E1111(dev);
        case MAD_88E1112:
                return runOnce88E1112(dev);
        case MAD_88E1116:
                return runOnce88E1116(dev);
        case MAD_88E114X:
                return runOnce88E114X(dev);
        case MAD_88E3016:
                return runOnce88E3016(dev);
        case MAD_88E3082:
                return runOnce88E3082(dev);
        case MAD_88E1121:
                return runOnce88E1121(dev);
        case MAD_88E1149:
        case MAD_88E1149R:
        case MAD_SWG65G : 
                return runOnce88E1149(dev);
        case MAD_88E1181:
                return runOnce88E1181(dev);
        case MAD_88E1240:
        case MAD_88E1340S:
        case MAD_88E1340:
        case MAD_88E1340M:
        case MAD_88E1310:
        case MAD_88E1119R:
        case MAD_88E1540:
                return runOnce88E1240(dev);   /* Do nothing */
        default:
                MAD_DBG_ERROR("No Pre-Init routine is defined for the device %#x.\n",dev->deviceId);
                break;
    }
    
    return MAD_OK;
}
