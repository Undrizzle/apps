#include <madCopyright.h>

/*******************************************************************************
* madPortPTP.c
*
* DESCRIPTION:
*       API definitions for Precise Time Protocol logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <madApi.h>
#include <madApiInternal.h>
#include <madSem.h>
#include <madHwCntl.h>
#include <madDrvPhyRegs.h>
#include <madDrvConfig.h>


extern MAD_STATUS msec_en_PTP 
(
    IN  MAD_DEV    *dev,
    IN  int        en
);

/****************************************************************************/
/* PTP operation function declaration.                                    */
/****************************************************************************/
static MAD_STATUS ptpOperationPerform
(
    IN   MAD_DEV             *dev,
    IN   MAD_PTP_OPERATION    ptpOp,
    INOUT MAD_PTP_OP_DATA     *opData
);

/*******************************************************************************
* mdPtpSetEnable_port
*
* DESCRIPTION:
*       This routine set port PTP Enable/Disable.
*
* INPUTS:
*       port - The logical port number
*       en    - 1: Power on PTP. 0: Power off.
*       port - The logical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetEnable_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_BOOL    en
)
{
    MAD_STATUS           retVal;
    MAD_U16     data;
    MAD_U16    page;
    MAD_U8     reg;
    MAD_U32          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetEnable Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }
    if (en==MAD_TRUE)
    {
        data = 0;
    }
    else
    {
        data = 1;
    }
    page = MAD_PAGE6;
    reg = MAD_REG_GENERAL_CONTROL;
    if((retVal = madHwSetPagedPhyRegField(dev,hwPort,page,reg,9,1,data))!= MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",page,reg);
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
    if (dev->deviceId!=MAD_88E1340)
    {
      if (en==MAD_TRUE)
      {
         data = 1;
      }
      else
      {
        data = 0;
      }
      if((retVal = msec_en_PTP(dev, data))!= MAD_OK)
      {
         MAD_DBG_ERROR("Set macsec bypass PTP failed.\n");
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
      }
    }

    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetEnable_port
*
* DESCRIPTION:
*       This routine Get port PTP Enable/Disable.
*
* INPUTS:
*       port - The logical port number
*       en    - 1: Power on PTP. 0: Power off.
*
* OUTPUTS:
*       en    - 1: Power on PTP. 0: Power off.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetEnable_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT  MAD_BOOL    *en
)
{
    MAD_STATUS           retVal;
    MAD_U16     data;
    MAD_U16    page;
    MAD_U8     reg;
    MAD_U32          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetEnable Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }
    page = MAD_PAGE6;
    reg = MAD_REG_GENERAL_CONTROL;
    if((retVal = madHwGetPagedPhyRegField(dev,hwPort,page,reg,9,1,&data))!=MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",page,reg);
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    if (data)
    {
        *en=MAD_FALSE;
    }
    else
    {
        *en=MAD_TRUE;
    }
    return MAD_OK;
}

/*******************************************************************************
* mdPtpSetIntEnable_port
*
* DESCRIPTION:
*       This routine set port PTP interrupt Enable/Disable.
*
* INPUTS:
*       port - The logical port number
*       en    - 1: Power on PTP. 0: Power off.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetIntEnable_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_BOOL    en
)
{
    MAD_STATUS           retVal;
    MAD_U16     data;
    MAD_U16    page;
    MAD_U8     reg;
    MAD_U32          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetIntEnable Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }
    if (en==MAD_TRUE)
    {
        data = 3;
    }
    else
    {
        data = 0;
    }
    page = MAD_PAGE8;
    reg = MAD_REG_PTP_PORT_CONFIG_2;
    if((retVal = madHwSetPagedPhyRegField(dev,hwPort,page,reg,0,2,data))!= MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",page,reg);
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetIntEnable_port
*
* DESCRIPTION:
*       This routine get port PTP Interrupt Enable/Disable.
*
* INPUTS:
*       port - The logical port number
*       en    - 1: Power on PTP. 0: Power off.
*
* OUTPUTS:
*       en    - 1: Power on PTP. 0: Power off.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetIntEnable_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT  MAD_BOOL    *en
)
{
    MAD_STATUS           retVal;
    MAD_U16     data;
    MAD_U16    page;
    MAD_U8     reg;
    MAD_U32          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpGetIntEnable Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }
    page = MAD_PAGE8;
    reg = MAD_REG_PTP_PORT_CONFIG_2;
    if((retVal = madHwGetPagedPhyRegField(dev,hwPort,page,reg,0,2,&data))!=MAD_OK)
    {
        MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",page,reg);
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    if (data)
    {
        *en=MAD_TRUE;
    }
    else
    {
        *en=MAD_FALSE;
    }
    return MAD_OK;
}

/*******************************************************************************
* mdPtpSetConfig_port
*
* DESCRIPTION:
*       This routine writes port PTP configuration parameters.
*
* INPUTS:
*       port - The logical port number
*        ptpData  - PTP configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_PTP_CONFIG    *ptpData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_PTP_PORT_CONFIG    ptpPortData;
    MAD_U32          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetConfig Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xF;    /* Global register */
    op = MAD_PTP_WRITE_DATA;

    /* setting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    opData.ptpData = ptpData->ptpEType;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing PTPEType.\n"));
        return MAD_FAIL;
    }

    /* setting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    opData.ptpData = ptpData->msgIdTSEn;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing MsgIDTSEn.\n"));
        return MAD_FAIL;
    }

    /* setting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    opData.ptpData = ptpData->tsArrPtr;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TSArrPtr.\n"));
        return MAD_FAIL;
    }

    {
        ptpPortData.transSpec = ptpData->transSpec;
        ptpPortData.disTSpec = 1;    /* default value */
        ptpPortData.disTSOverwrite = ptpData->disTSOverwrite;
        ptpPortData.ipJump = 2;        /* default value */
        ptpPortData.etJump = 12;    /* default value */

        /* per port configuration */
/*        for(port=0; port<dev->numOfPorts; port++)   */
        {
            ptpPortData.ptpArrIntEn = (ptpData->ptpArrIntEn & (1 << port))? MAD_TRUE : MAD_FALSE;
            ptpPortData.ptpDepIntEn = (ptpData->ptpDepIntEn & (1 << port))? MAD_TRUE : MAD_FALSE;


            if((retVal = mdPtpSetPortConfig(dev, port, &ptpPortData)) != MAD_OK)
            {
                MAD_DBG_INFO(("Failed mdPtpSetPortConfig.\n"));
                return MAD_FAIL;
            }
        }

        return MAD_OK; 
    }
    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* mdPtpGetConfig_port
*
* DESCRIPTION:
*       This routine reads port PTP configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        ptpData  - PTP configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_PTP_CONFIG    *ptpData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_PTP_PORT_CONFIG    ptpPortData;
    MAD_U32          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpGetConfig Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }


    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xF;    /* Global register */
    op = MAD_PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }

    ptpData->ptpEType = opData.ptpData;

    /* getting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading MsgIDTSEn.\n"));
        return MAD_FAIL;
    }

    ptpData->msgIdTSEn = opData.ptpData;

    /* getting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TSArrPtr.\n"));
        return MAD_FAIL;
    }

    ptpData->tsArrPtr = opData.ptpData;

    {
        ptpData->ptpArrIntEn = 0;
        ptpData->ptpDepIntEn = 0;

        /* per port configuration */
/*        for(port=0; port<dev->numOfPorts; port++)  */
        {
            if((retVal = mdPtpGetPortConfig(dev, port, &ptpPortData)) != MAD_OK)
            {
                MAD_DBG_INFO(("Failed mdPtpGetPortConfig.\n"));
                return MAD_FAIL;
            }

            ptpData->ptpArrIntEn |= (ptpPortData.ptpArrIntEn ? (1 << port) : 0);
            ptpData->ptpDepIntEn |= (ptpPortData.ptpDepIntEn ? (1 << port) : 0);
        }

        ptpData->transSpec = ptpPortData.transSpec;
        ptpData->disTSOverwrite = ptpPortData.disTSOverwrite;

        ptpData->msgIdStartBit = 4;

        return MAD_OK;
    }


    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}

/*******************************************************************************
* mdPtpSetGlobalConfig_port
*
* DESCRIPTION:
*       This routine writes port PTP global configuration parameters.
*
* INPUTS:
*       port - The logical port number
*        ptpData  - PTP global configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetGlobalConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_PTP_GLOBAL_CONFIG    *ptpData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetGlobalConfig Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }


    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xF;    /* Global register */
    op = MAD_PTP_WRITE_DATA;

    /* setting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    opData.ptpData = ptpData->ptpEType;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing PTPEType.\n"));
        return MAD_FAIL;
    }

    /* setting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    opData.ptpData = ptpData->msgIdTSEn;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing MsgIDTSEn.\n"));
        return MAD_FAIL;
    }

    /* setting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    opData.ptpData = ptpData->tsArrPtr;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TSArrPtr.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* mdPtpGlobalGetConfig_port
*
* DESCRIPTION:
*       This routine reads port PTP global configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        ptpData  - PTP global configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetGlobalConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_PTP_GLOBAL_CONFIG    *ptpData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */

    MAD_DBG_INFO(("mdPtpGetGlobalConfig Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }


    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xF;    /* Global register */
    op = MAD_PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }

    ptpData->ptpEType = opData.ptpData;

    /* getting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading MsgIDTSEn.\n"));
        return MAD_FAIL;
    }

    ptpData->msgIdTSEn = opData.ptpData;

    /* getting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TSArrPtr.\n"));
        return MAD_FAIL;
    }

    ptpData->tsArrPtr = opData.ptpData;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* mdPtpSetPortConfig
*
* DESCRIPTION:
*       This routine writes PTP port configuration parameters.
*
* INPUTS:
*        ptpData  - PTP port configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetPortConfig
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_PTP_PORT_CONFIG    *ptpData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetPortConfig Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    if (ptpData->transSpec > 0xF)    /* 4 bits */
        return MAD_API_PTP_BAD_PARAM;

    if (ptpData->etJump > 0x1F)    /* 5 bits */
        return MAD_API_PTP_BAD_PARAM;

    if (ptpData->ipJump > 0x3F)    /* 6 bits */
        return MAD_API_PTP_BAD_PARAM;

    
    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = hwPort;

    /* TransSpec, DisTSpecCheck, DisTSOverwrite bit, offset 0 */
    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    op = MAD_PTP_WRITE_DATA;
    opData.ptpData = (ptpData->transSpec << 12) | (opData.ptpData & 0x1) |
                    ((ptpData->disTSpec?1:0) << 11) |
                    ((ptpData->disTSOverwrite?1:0) << 1);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TransSpec,DisTSpecCheck,DisTSOverwrite.\n"));
        return MAD_FAIL;
    }

    /* setting etJump and ipJump, offset 1 */
    opData.ptpAddr = 1;
    opData.ptpData = (ptpData->ipJump << 8) | ptpData->etJump;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing MsgIDTSEn.\n"));
        return MAD_FAIL;
    }

    /* setting Int, offset 2 */
    opData.ptpAddr = 2;
    opData.ptpData = (ptpData->ptpArrIntEn?1:0) |
                    ((ptpData->ptpDepIntEn?1:0) << 1);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TSArrPtr.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* mdPtpGetPortConfig
*
* DESCRIPTION:
*       This routine reads PTP configuration parameters for a port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        ptpData  - PTP port configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPortConfig
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    OUT MAD_PTP_PORT_CONFIG    *ptpData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpGetPortConfig Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }


    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = hwPort;
    op = MAD_PTP_READ_DATA;

    /* TransSpec, DisTSpecCheck, DisTSOverwrite bit, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }

    ptpData->transSpec = opData.ptpData >> 12;
    ptpData->disTSpec = ((opData.ptpData >> 11) & 0x1) ? MAD_TRUE : MAD_FALSE;
    ptpData->disTSOverwrite = ((opData.ptpData >> 1) & 0x1) ? MAD_TRUE : MAD_FALSE;

    /* getting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading MsgIDTSEn.\n"));
        return MAD_FAIL;
    }

    ptpData->ipJump = (opData.ptpData >> 8) & 0x3F;
    ptpData->etJump = opData.ptpData & 0x1F;

    /* getting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TSArrPtr.\n"));
        return MAD_FAIL;
    }

    ptpData->ptpDepIntEn = ((opData.ptpData >> 1) & 0x1) ? MAD_TRUE : MAD_FALSE;
    ptpData->ptpArrIntEn = (opData.ptpData & 0x1) ? MAD_TRUE : MAD_FALSE;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* mdPtpSetPTPEn_port
*
* DESCRIPTION:
*       This routine enables or disables port PTP.
*
* INPUTS:
*        en - MAD_TRUE to enable PTP, MAD_FALSE to disable PTP
*
* OUTPUTS:
*       port - The logical port number
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetPTPEn_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_BOOL        en
)
{
    MAD_STATUS           retVal;
    MAD_U32          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetPTPEn Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }


    if((retVal = mdPtpSetPortPTPEn(dev, hwPort, en)) != MAD_OK)
    {
           MAD_DBG_INFO(("Failed mdPtpSetPortPTPEn.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* mdPtpGetPTPEn_port
*
* DESCRIPTION:
*       This routine checks if port PTP is enabled.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        en - MAD_TRUE if enabled, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPTPEn_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL        *en
)
{
    MAD_STATUS           retVal;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("mdPtpGetPTPEn Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }


    if((retVal = mdPtpGetPortPTPEn(dev, hwPort, en)) != MAD_OK)
    {
           MAD_DBG_INFO(("Failed mdPtpGetPortPTPEn.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}

/*******************************************************************************
* mdPtpSetPortPTPEn
*
* DESCRIPTION:
*       This routine enables or disables PTP on a port.
*
* INPUTS:
*        en - MAD_TRUE to enable PTP, MAD_FALSE to disable PTP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetPortPTPEn
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_BOOL        en
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32            hwPort;

    MAD_DBG_INFO(("mdPtpSetPortPTPEn Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }


    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */
    opData.ptpAddr = 0;

    opData.ptpPort = hwPort;

    op = MAD_PTP_READ_DATA;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    op = MAD_PTP_WRITE_DATA;

    if (en)
        opData.ptpData &= ~0x1;
    else
        opData.ptpData |= 0x1;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TransSpec,DisTSpecCheck,DisTSOverwrite.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetPortPTPEn
*
* DESCRIPTION:
*       This routine checks if PTP is enabled on a port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        en - MAD_TRUE if enabled, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPortPTPEn
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    OUT MAD_BOOL        *en
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32            hwPort;

    MAD_DBG_INFO(("mdPtpGetPortPTPEn Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpAddr = 0;
    opData.ptpPort = hwPort;

    op = MAD_PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    *en = (opData.ptpData & 0x1) ? MAD_FALSE : MAD_TRUE;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* mdPtpGetPTPInt_port
*
* DESCRIPTION:
*       This routine gets port PTP interrupt status.
*        The PTP Interrupt bit gets set for a given port when an incoming PTP 
*        frame is time stamped and PTPArrIntEn for that port is set to 0x1.
*        Similary PTP Interrupt bit gets set for a given port when an outgoing
*        PTP frame is time stamped and PTPDepIntEn for that port is set to 0x1.
*        This bit gets cleared upon software reading and clearing the corresponding
*        time counter valid bits that are valid for that port.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        ptpInt     - interrupt status for each port (bit 0 for port 0, bit 1 for port 1, etc.)
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPTPInt_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_U32        *ptpInt
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */

    MAD_DBG_INFO(("mdPtpGetPTPInt Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }


    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */
    opData.ptpPort = 0xF;    /* Global register */
    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 8;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

/*    opData.ptpData &= (1 << dev->maxPorts) - 1; */

    *ptpInt = MAD_PORTVEC_2_LPORTVEC(opData.ptpData);

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* mdPtpGetPTPGlobalTime_port
*
* DESCRIPTION:
*       This routine gets the port global timer value that is running off of the free
*        running switch core clock.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        ptpTime    - PTP global time
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPTPGlobalTime_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_U32        *ptpTime
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("mdPtpGetPTPGlobalTime Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }


    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_MULTIPLE_DATA;
#if 0
    opData.ptpPort = IS_IN_DEV_GROUP(dev,DEV_TAI)?0xE:0xF;    /* Global register */
    opData.ptpAddr = IS_IN_DEV_GROUP(dev,DEV_TAI)?0xE:9;
#else
    opData.ptpPort = 0xF;    /* Global register */
    opData.ptpAddr = 9;

#endif
    opData.nData = 2;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    *ptpTime = MAD_PTP_BUILD_TIME(opData.ptpMultiData[1],opData.ptpMultiData[0]);

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* mdPtpGetTimeStamped
*
* DESCRIPTION:
*        This routine retrieves the PTP port status that includes time stamp value 
*        and sequce Id that are captured by PTP logic for a PTP frame that needs 
*        to be time stamped.
*
* INPUTS:
*       port         - logical port number.
*       timeToRead    - Arr0, Arr1, or Dep time (MAD_PTP_TIME enum type)
*
* OUTPUTS:
*        ptpStatus    - PTP port status
*
* RETURNS:
*       MAD_OK         - on success
*       MAD_FAIL     - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetTimeStamped
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_PTP_TIME    timeToRead,
    OUT MAD_PTP_TS_STATUS    *ptpStatus
)
{
    MAD_STATUS           retVal;
    MAD_U32                hwPort;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32                baseReg;

    MAD_DBG_INFO(("mdPtpGetTimeStamped Called.\n"));

    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

        baseReg = 8;

    switch (timeToRead)
    {
        case MAD_PTP_ARR0_TIME:
            opData.ptpAddr = baseReg + 0;
            break;
        case MAD_PTP_ARR1_TIME:
            opData.ptpAddr = baseReg + 4;
            break;
        case MAD_PTP_DEP_TIME:
            opData.ptpAddr = baseReg + 8;
            break;
        default:
            MAD_DBG_INFO(("Invalid time to be read\n"));
            return MAD_API_PTP_BAD_PARAM;
    }

    opData.ptpPort = hwPort;
    opData.ptpPhyPort = hwPort;    /* port number */
    opData.ptpBlock = 0;

/* #ifndef USE_SINGLE_READ */
#if 1 
    op = MAD_PTP_READ_TIMESTAMP_DATA;
    opData.nData = 4;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    ptpStatus->isValid = (opData.ptpMultiData[0] & 0x1) ? MAD_TRUE : MAD_FALSE;
    ptpStatus->status = (MAD_PTP_INT_STATUS)((opData.ptpMultiData[0] >> 1) & 0x3);
    ptpStatus->timeStamped = MAD_PTP_BUILD_TIME(opData.ptpMultiData[2],opData.ptpMultiData[1]);
    ptpStatus->ptpSeqId = opData.ptpMultiData[3];
#else
    {
    MAD_U32 data[4], i;

    op = MAD_PTP_READ_DATA;

    for (i=0; i<4; i++)
    {
        if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
        {
            MAD_DBG_INFO(("Failed reading DisPTP.\n"));
            return MAD_FAIL;
        }
    
        data[i] = opData.ptpData;
        opData.ptpAddr++;
    }

    ptpStatus->isValid = (data[0] & 0x1) ? MAD_TRUE : MAD_FALSE;
    ptpStatus->status = (MAD_PTP_INT_STATUS)((data[0] >> 1) & 0x3);
    ptpStatus->timeStamped = MAD_PTP_BUILD_TIME(data[2],data[1]);
    ptpStatus->ptpSeqId = data[3];

    }
#endif

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}

/*******************************************************************************
* mdPtpGetTSValidSt
*
* DESCRIPTION:
*        This routine gets PTP Time valid bit.
*
* INPUTS:
*       port         - logical port number.
*       timeToReset    - Arr0, Arr1, or Dep time (MAD_PTP_TIME enum type)
*
* OUTPUTS:
*    isValid        - MAD_TRUE: valid TS, MAD_FALSE: invalid TS.
*
* RETURNS:
*       MAD_OK         - on success
*       MAD_FAIL     - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetTSValidSt
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_PTP_TIME    timeToReset,
    OUT MAD_BOOL    *isValid
)
{
    MAD_STATUS           retVal;
    MAD_U32                hwPort;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32                baseReg;

    MAD_DBG_INFO(("mdPtpGetTSValidSt Called.\n"));

    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

        baseReg = 8;

    switch (timeToReset)
    {
        case MAD_PTP_ARR0_TIME:
            opData.ptpAddr = baseReg + 0;
            break;
        case MAD_PTP_ARR1_TIME:
            opData.ptpAddr = baseReg + 4;
            break;
        case MAD_PTP_DEP_TIME:
            opData.ptpAddr = baseReg + 8;
            break;
        default:
            MAD_DBG_INFO(("Invalid time to get valid\n"));
            return MAD_API_PTP_BAD_PARAM;
    }

    opData.ptpPort = hwPort;
    opData.ptpData = 0;
    opData.ptpBlock = 0;
    opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing Port Status.\n"));
        return MAD_FAIL;
    }

    *isValid = (opData.ptpData & 0x1) ? MAD_TRUE : MAD_FALSE;
    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}

/*******************************************************************************
* mdPtpResetTimeStamp
*
* DESCRIPTION:
*        This routine resets PTP Time valid bit so that PTP logic can time stamp
*        a next PTP frame that needs to be time stamped.
*
* INPUTS:
*       port         - logical port number.
*       timeToReset    - Arr0, Arr1, or Dep time (MAD_PTP_TIME enum type)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MAD_OK         - on success
*       MAD_FAIL     - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpResetTimeStamp
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_PTP_TIME    timeToReset
)
{
    MAD_STATUS           retVal;
    MAD_U32                hwPort;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32                baseReg;

    MAD_DBG_INFO(("mdPtpResetTimeStamp Called.\n"));

    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

        baseReg = 8;

    switch (timeToReset)
    {
        case MAD_PTP_ARR0_TIME:
            opData.ptpAddr = baseReg + 0;
            break;
        case MAD_PTP_ARR1_TIME:
            opData.ptpAddr = baseReg + 4;
            break;
        case MAD_PTP_DEP_TIME:
            opData.ptpAddr = baseReg + 8;
            break;
        default:
            MAD_DBG_INFO(("Invalid time to reset\n"));
            return MAD_API_PTP_BAD_PARAM;
    }

    opData.ptpPort = hwPort;
    opData.ptpData = 0;
    opData.ptpBlock = 0;
    opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing Port Status.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* mdPtpGetReg
*
* DESCRIPTION:
*       This routine reads PTP register.
*
* INPUTS:
*       port         - logical port number.
*       ptpPort     - original PTP port number. (global port is 0xf)
*       regOffset    - register to read
*
* OUTPUTS:
*        data        - register data
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetReg
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_U8    ptpPort,
    IN  MAD_U32        regOffset,
    OUT MAD_U32        *data
)
{
    MAD_STATUS           retVal;
    MAD_U32                hwPort;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;

    MAD_DBG_INFO(("mdPtpGetReg Called.\n"));

    hwPort = (MAD_U32)port;

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }


    if (regOffset > 0x1F)
    {
        MAD_DBG_INFO(("Invalid reg offset\n"));
        return MAD_API_PTP_BAD_PARAM;
    }

    op = MAD_PTP_READ_DATA;
    opData.ptpPort = ptpPort;
    opData.ptpAddr = regOffset;
    opData.ptpBlock = 0;
    opData.ptpPhyPort = hwPort;    /* port number */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    *data = opData.ptpData;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}

/*******************************************************************************
* mdPtpSetReg
*
* DESCRIPTION:
*       This routine writes data to PTP register.
*
* INPUTS:
*       port         - logical port number
*       ptpPort     - original PTP port number. (global port is 0xf)
*       regOffset    - register to be written
*        data        - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetReg
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_U8    ptpPort,
    IN  MAD_U32        regOffset,
    IN  MAD_U32        data
)
{
    MAD_STATUS           retVal;
    MAD_U32                hwPort;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;

    MAD_DBG_INFO(("mdPtpSetReg Called.\n"));

    hwPort = (MAD_U32)port;
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }


    if ((regOffset > 0x1F) || (data > 0xFFFF))
    {
        MAD_DBG_INFO(("Invalid reg offset/data\n"));
        return MAD_API_PTP_BAD_PARAM;
    }

    op = MAD_PTP_WRITE_DATA;
    opData.ptpPort = ptpPort;
    opData.ptpAddr = regOffset;
    opData.ptpData = data;
    opData.ptpBlock = 0;
    opData.ptpPhyPort = hwPort;    /* port number */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }


    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* madTaiSetEventConfig_port
*
* DESCRIPTION:
*       This routine sets port TAI Event Capture configuration parameters.
*
* INPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* OUTPUTS:
*       port - The logical port number
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiSetEventConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_TAI_EVENT_CONFIG    *eventData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiSetEventConfig Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }

    opData.ptpData &= ~((3<<14)|(1<<8));
    if (eventData->intEn)
        opData.ptpData |= (1 << 8);
    if (eventData->eventOverwrite)
        opData.ptpData |= (1 << 15);
    if (eventData->eventCtrStart)
        opData.ptpData |= (1 << 14);

    op = MAD_PTP_WRITE_DATA;

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing PTPEType.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* madTaiGetEventConfig_port
*
* DESCRIPTION:
*       This routine gets port TAI Event Capture configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetEventConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_TAI_EVENT_CONFIG    *eventData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiGetEventConfig Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }

    eventData->intEn = (opData.ptpData & (1<<8))?MAD_TRUE:MAD_FALSE;
    eventData->eventOverwrite = (opData.ptpData & (1<<15))?MAD_TRUE:MAD_FALSE;
    eventData->eventCtrStart = (opData.ptpData & (1<<14))?MAD_TRUE:MAD_FALSE;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* madTaiGetEventStatus_port
*
* DESCRIPTION:
*       This routine gets port TAI Event Capture status.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetEventStatus_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_TAI_EVENT_STATUS    *status
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32                 data[2];
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiGetEventStatus Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    opData.ptpAddr = 9;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }

    status->isValid = (opData.ptpData & (1<<8))?MAD_TRUE:MAD_FALSE;
    status->eventCtr = opData.ptpData & 0xFF;
    status->eventErr = (opData.ptpData & (1<<9))?MAD_TRUE:MAD_FALSE;

    if (status->isValid == MAD_FALSE)
    {
        return MAD_OK;
    }

    opData.ptpAddr = 10;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }
    data[0] = opData.ptpData;

    opData.ptpAddr = 11;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }
    data[1] = opData.ptpData;

    status->eventTime = MAD_PTP_BUILD_TIME(data[1],data[0]);

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* madTaiGetEventInt_port
*
* DESCRIPTION:
*       This routine gets port TAI Event Capture Interrupt status.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        intStatus     - interrupt status for TAI Event capture
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetEventInt_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL        *intStatus
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiGetEventInt Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpPort = 0xE;    /* Global register */
    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 9;
    opData.ptpBlock = 0;
    opData.ptpPhyPort = hwPort;    /* port number */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    *intStatus = (opData.ptpData & 0x8000)?MAD_TRUE:MAD_FALSE;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* madTaiGetTrigInt_port
*
* DESCRIPTION:
*       This routine gets port TAI Trigger Interrupt status.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        intStatus     - interrupt status for TAI Trigger
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetTrigInt_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL        *intStatus
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiGetTrigInt Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpPort = 0xE;    /* Global register */
    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 8;
    opData.ptpBlock = 0;
    opData.ptpPhyPort = hwPort;    /* port number */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    *intStatus = (opData.ptpData & 0x8000)?MAD_TRUE:MAD_FALSE;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* madTaiSetTrigConfig_port
*
* DESCRIPTION:
*       This routine sets port TAI Trigger configuration parameters.
*
* INPUTS:
*        trigEn    - enable/disable TAI Trigger.
*        trigData  - TAI Trigger configuration parameters (valid only if trigEn is MAD_TRUE).
*
* OUTPUTS:
*       port - The logical port number
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiSetTrigConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_BOOL     trigEn,
  IN  MAD_TAI_TRIGGER_CONFIG    *trigData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiSetTrigConfig Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }

    opData.ptpData &= ~(3|(1<<9));

    if (trigEn == MAD_FALSE)
    {
        op = MAD_PTP_WRITE_DATA;

        if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
        {
            MAD_DBG_INFO(("Failed writing PTPEType.\n"));
            return MAD_FAIL;
        }

        return MAD_OK;
    }

    opData.ptpData |= 1;

    if (trigData->intEn)
        opData.ptpData |= (1 << 9);

    if (trigData->mode == MAD_TAI_TRIG_ON_GIVEN_TIME)
        opData.ptpData |= (1 << 1);

    op = MAD_PTP_WRITE_DATA;

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing PTPEType.\n"));
        return MAD_FAIL;
    }

    opData.ptpAddr = 2;
    opData.ptpData = MAD_PTP_L16_TIME(trigData->trigGenAmt);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing PTPEType.\n"));
        return MAD_FAIL;
    }

    opData.ptpAddr = 3;
    opData.ptpData = MAD_PTP_H16_TIME(trigData->trigGenAmt);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing PTPEType.\n"));
        return MAD_FAIL;
    }

    if (trigData->mode == MAD_TAI_TRIG_ON_GIVEN_TIME)
    {
        if ((trigData->pulseWidth >= 0) && (trigData->pulseWidth <= 0xF))
        {            
            op = MAD_PTP_READ_DATA;
            opData.ptpAddr = 5;        /* PulseWidth */

            if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
            {
                MAD_DBG_INFO(("Failed writing PTPEType.\n"));
                return MAD_FAIL;
            }

            op = MAD_PTP_WRITE_DATA;
            opData.ptpAddr = 5;        /* PulseWidth */
            opData.ptpData &= (~0xF000);
            opData.ptpData |= (MAD_U16)(trigData->pulseWidth << 12);

            if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
            {
                MAD_DBG_INFO(("Failed writing PTPEType.\n"));
                return MAD_FAIL;
            }
        }
    }
    else
    {
        op = MAD_PTP_WRITE_DATA;
        opData.ptpAddr = 4;        /* TrigClkComp */
/*        opData.ptpData = (MAD_U16)trigData->trigClkComp; */
        opData.ptpData = trigData->trigClkComp; 

        if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
        {
            MAD_DBG_INFO(("Failed writing PTPEType.\n"));
            return MAD_FAIL;
        }
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}

/*******************************************************************************
* madTaiGetTrigConfig_port
*
* DESCRIPTION:
*       This routine gets port TAI Trigger configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        trigEn    - enable/disable TAI Trigger.
*        trigData  - TAI Trigger configuration parameters (valid only if trigEn is MAD_TRUE).
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetTrigConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL     *trigEn,
  OUT MAD_TAI_TRIGGER_CONFIG    *trigData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32                 data[2];
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiGetTrigConfig Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }

    if (!(opData.ptpData & 1))
    {
        *trigEn = MAD_FALSE;
        return MAD_OK;
    }

    if (trigData == NULL)
    {
        return MAD_API_PTP_BAD_PARAM;
    }

    *trigEn = MAD_TRUE;
    trigData->mode = (opData.ptpData >> 1) & 1;
    trigData->intEn = (opData.ptpData >> 9) & 1;

    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }
    data[0] = opData.ptpData;

    opData.ptpAddr = 3;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }
    data[1] = opData.ptpData;

    trigData->trigGenAmt = MAD_PTP_BUILD_TIME(data[1],data[0]);

    opData.ptpAddr = 5;        /* PulseWidth */
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing PTPEType.\n"));
        return MAD_FAIL;
    }

    trigData->pulseWidth = (MAD_U32)((opData.ptpData >> 12) & 0xF);

    /* getting TrigClkComp, offset 4 */
    opData.ptpAddr = 4;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }

    trigData->trigClkComp = (MAD_U32)opData.ptpData;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* madTaiGetTSClkPer_port
*
* DESCRIPTION:
*         Time Stamping Clock Period in pico seconds.
*        This routine specifies the clock period for the time stamping clock supplied 
*        to the port PTP hardware logic.
*        This is the clock that is used by the hardware logic to update the PTP 
*        Global Time Counter.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        clk        - time stamping clock period
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiGetTSClkPer_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_U32        *clk
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiGetTSClkPer Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    opData.ptpAddr = 1;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    *clk = (MAD_U32)opData.ptpData;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* madTaiSetTSClkPer_port
*
* DESCRIPTION:
*         Time Stamping Clock Period in pico seconds.
*        This routine specifies the clock period for the time stamping clock supplied 
*        to the PTP hardware logic.
*        This is the clock that is used by the hardware logic to update the port PTP 
*        Global Time Counter.
*
* INPUTS:
*        clk        - time stamping clock period
*
* OUTPUTS:
*       port - The logical port number
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiSetTSClkPer_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_U32        clk
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */

    MAD_DBG_INFO(("madTaiSetTSClkPer Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_WRITE_DATA;

    opData.ptpAddr = 1;

    opData.ptpData = (MAD_U16)clk; 

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* madTaiSetMultiPTPSync_port
*
* DESCRIPTION:
*         This routine sets port Multiple PTP device sync mode and sync time (TrigGenAmt).
*        When enabled, the hardware logic detects a low to high transition on the 
*        EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*        register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*       port - The logical port number
*        multiEn        - enable/disable Multiple PTP device sync mode
*        syncTime    - sync time (valid only if multiEn is MAD_TRUE)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_FAIL    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         When enabled, madTaiSetTrigConfig, madTaiSetEventConfig, madTaiSetTimeInc,
*        and madTaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
MAD_STATUS madTaiSetMultiPTPSync_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_BOOL     multiEn,
  IN  MAD_32        syncTime
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */

    MAD_DBG_INFO(("madTaiSetMultiPTPSync Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI register.\n"));
        return MAD_FAIL;
    }

    opData.ptpData &= ~(1 << 2);

    if (multiEn == MAD_FALSE)
    {
        op = MAD_PTP_WRITE_DATA;

        if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
        {
            MAD_DBG_INFO(("Failed writing TAI register.\n"));
            return MAD_FAIL;
        }

        return MAD_OK;
    }

    opData.ptpData |= (1 << 2);


    op = MAD_PTP_WRITE_DATA;

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI register.\n"));
        return MAD_FAIL;
    }

    opData.ptpAddr = 2;
    opData.ptpData = MAD_PTP_L16_TIME(syncTime);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI register.\n"));
        return MAD_FAIL;
    }

    opData.ptpAddr = 3;
    opData.ptpData = MAD_PTP_H16_TIME(syncTime);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI register.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* madTaiGetMultiPTPSync_port
*
* DESCRIPTION:
*         This routine gets port Multiple PTP device sync mode and sync time (TrigGenAmt).
*        When enabled, the hardware logic detects a low to high transition on the 
*        EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*        register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        multiEn        - enable/disable Multiple PTP device sync mode
*        syncTime    - sync time (valid only if multiEn is MAD_TRUE)
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_FAIL    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         When enabled, madTaiSetTrigConfig, madTaiSetEventConfig, madTaiSetTimeInc,
*        and madTaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
MAD_STATUS madTaiGetMultiPTPSync_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL     *multiEn,
  OUT MAD_32        *syncTime
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32                 data[2];
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiGetMultiPTPSync Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI register.\n"));
        return MAD_FAIL;
    }

    if(!(opData.ptpData & (1 << 2)))
    {
        *multiEn = MAD_FALSE;
        *syncTime = 0;
        return MAD_OK;
    }
    *multiEn = MAD_TRUE;

    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI register.\n"));
        return MAD_FAIL;
    }
    data[0] = opData.ptpData;

    opData.ptpAddr = 3;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI register.\n"));
        return MAD_FAIL;
    }
    data[1] = opData.ptpData;

    *syncTime = MAD_PTP_BUILD_TIME(data[1],data[0]);

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* madTaiGetTimeIncDec_port
*
* DESCRIPTION:
*         This routine retrieves port Time increment/decrement setup.
*        This amount specifies the number of units of PTP Global Time that need to be 
*        incremented or decremented. This is used for adjusting the PTP Global Time 
*        counter value by a certain amount.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        expired    - MAD_TRUE if inc/dec occurred, MAD_FALSE otherwise
*        inc        - MAD_TRUE if increment, MAD_FALSE if decrement
*        amount    - increment/decrement amount
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         Time increment or decrement will be excuted once.
*
*******************************************************************************/
MAD_STATUS madTaiGetTimeIncDec_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL        *expired,
  OUT MAD_BOOL        *inc,
  OUT MAD_U32        *amount
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiGetTimeIncDec Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    opData.ptpAddr = 5;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    *inc = (opData.ptpData & 0x800)?MAD_FALSE:MAD_TRUE;
    *amount = (MAD_U32)(opData.ptpData & 0x7FF);

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    *expired = (opData.ptpData & 0x8)?MAD_FALSE:MAD_TRUE;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* madTaiSetTimeInc_port
*
* DESCRIPTION:
*         This routine enables port time increment by the specifed time increment amount.
*        The amount specifies the number of units of PTP Global Time that need to be 
*        incremented. This is used for adjusting the PTP Global Time counter value by
*        a certain amount.
*        Increment occurs just once.
*
* INPUTS:
*        amount    - time increment amount (0 ~ 0x7FF)
*
* OUTPUTS:
*       port - The logical port number
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiSetTimeInc_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_U32        amount
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiSetTimeInc Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    /* set TimeIncAmt */
    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    opData.ptpAddr = 5;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    opData.ptpData &= 0xF000;
    opData.ptpData |= amount;

    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    /* set TimeIncEn */
    opData.ptpBlock = 0x0;    /* PTP register space */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    opData.ptpData |= 0x8;

    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* madTaiSetTimeDec_port
*
* DESCRIPTION:
*         This routine enables port time decrement by the specifed time decrement amount.
*        The amount specifies the number of units of PTP Global Time that need to be 
*        decremented. This is used for adjusting the PTP Global Time counter value by
*        a certain amount.
*        Decrement occurs just once.
*
* INPUTS:
*       port - The logical port number
*        amount    - time decrement amount (0 ~ 0x7FF)
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_FAIL    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiSetTimeDec_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_U32        amount
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiSetTimeInc Called.\n"));
    hwPort = (MAD_U32)MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_ERROR("The port %d is over range.\n", (int)port);
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR("PTP not supported.\n");
        return MAD_API_PTP_NOT_SUPPORT;
    }

    /* set TimeIncAmt */
    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    opData.ptpAddr = 5;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI register.\n"));
        return MAD_FAIL;
    }

    opData.ptpData &= 0xF000;
    opData.ptpData |= amount;
    opData.ptpData |= 0x800;    /* decrement */

    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI register.\n"));
        return MAD_FAIL;
    }

    /* set TimeIncEn */
    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI register.\n"));
        return MAD_FAIL;
    }

    opData.ptpData |= 0x8;

    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI register.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}




/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
/*******************************************************************************
* mapRegs
*
* DESCRIPTION:
*       This function maps parameters in PTP_OP_DATA to page and register number of PHY.
*
* INPUTS:
*       opData    - points to the data storage that the operation requires.
*
* OUTPUTS:
*       port    - port number.
*       page    - page number.
*       reg     - register number.
*
* RETURNS:
*       MAD_OK on success,
*       MAD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static MAD_STATUS mapRegs(MAD_PTP_OP_DATA    *opData, MAD_U8 *port, MAD_U16 *page, MAD_U8 *reg)
{

  *port = opData->ptpPhyPort;   
  switch (opData->ptpPort)
  {
    case 0: /* PTP Port configuration/Status */
    case 1: /* PTP Port configuration/Status */
    case 2: /* PTP Port configuration/Status */
    case 3: /* PTP Port configuration/Status */
    /* Port 0, 1, 2, 3 for 1340*/
      *page = MAD_PAGE8;
      *reg = opData->ptpAddr;
      if (opData->ptpAddr>15)
      {
        *page = MAD_PAGE9;
        *reg = opData->ptpAddr-16;
      }

      break;
    case 0xe: /* TAI configuration/Status */
      *page = MAD_PAGE12;
      *reg = opData->ptpAddr;
      break;
    case 0xf:/* PTP Global configuration/Status */
      switch (opData->ptpAddr)
      {
        case 0:
        case 1:
        case 2:
          *page = MAD_PAGE14;
          *reg = opData->ptpAddr;
            break;
        case 8:
      *page = MAD_PAGE14;
      *reg = opData->ptpAddr;
          break;
        case 9:
        case 10:
          *page = MAD_PAGE12;
          *reg = opData->ptpAddr+5;
          break;
        default:
          return MAD_FAIL;
      }
      break;
    default:
      return MAD_FAIL;
  }
  return MAD_OK;
}


/*******************************************************************************
* ptpOperationPerform
*
* DESCRIPTION:
*       This function accesses PTP Command Register and Data Register.
*
* INPUTS:
*       ptpOp      - The stats operation bits to be written into the stats
*                    operation register.
*
* OUTPUTS:
*       ptpData    - points to the data storage that the operation requires.
*
* RETURNS:
*       MAD_OK on success,
*       MAD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static MAD_STATUS ptpOperationPerform
(
  IN    MAD_DEV             *dev,
  IN    MAD_PTP_OPERATION        ptpOp,
  INOUT MAD_PTP_OP_DATA        *opData
)
{
  MAD_STATUS       retVal;    /* Functions return value */
  MAD_U16          data;     /* temporary Data storage */
  MAD_U32     i;
  MAD_U8     port;
  MAD_U16    page;
  MAD_U8     reg, kpReg;


  madSemTake(dev,dev->ptpRegsSem,OS_WAIT_FOREVER);

  /* Set the PTP Operation register */
  switch (ptpOp)
  {
    case MAD_PTP_WRITE_DATA:
      data = (MAD_U16)opData->ptpData;

      retVal = mapRegs(opData, &port, &page, &reg);
      if(retVal != MAD_OK)
      {
        madSemGive(dev,dev->ptpRegsSem);
        return retVal;
      }

      if((retVal = madHwWritePagedPhyReg(dev,port,page,reg,data))!= MAD_OK)
      {
        MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",page,reg);
        madSemGive(dev,dev->ptpRegsSem);
    return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
      }
 
      madSemGive(dev,dev->ptpRegsSem);
      break;

    case MAD_PTP_READ_DATA:
      retVal = mapRegs(opData, &port, &page, &reg);
      if(retVal != MAD_OK)
      {
        madSemGive(dev,dev->ptpRegsSem);
    return retVal;
      }
      retVal = madHwReadPagedPhyReg(dev,port,page,reg,&data);
      opData->ptpData = data;
      if(retVal != MAD_OK)
      {
        MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",page,reg);
        madSemGive(dev,dev->ptpRegsSem);
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
      }
  
      madSemGive(dev,dev->ptpRegsSem);
      break;

    case MAD_PTP_READ_MULTIPLE_DATA:
      for(i=0; i<opData->nData; i++)
      {
        retVal = mapRegs(opData, &port, &page, &reg);
        if(retVal != MAD_OK)
        {
          madSemGive(dev,dev->ptpRegsSem);
          return retVal;
        }
        retVal = madHwReadPagedPhyReg(dev,port,page,reg,&data);
        opData->ptpMultiData[i] = (MAD_U32)data;
        if(retVal != MAD_OK)
        {
          madSemGive(dev,dev->ptpRegsSem);
      return retVal;
    }
     opData->ptpAddr++;
      }

      madSemGive(dev,dev->ptpRegsSem);
      break;

    case MAD_PTP_READ_TIMESTAMP_DATA:
      kpReg = opData->ptpAddr;
      retVal = mapRegs(opData, &port, &page, &reg);
      if(retVal != MAD_OK)
      {
        madSemGive(dev,dev->ptpRegsSem);
    return retVal;
      }

      retVal = madHwReadPagedPhyReg(dev,port,page,reg,&data);
      opData->ptpMultiData[0] = (MAD_U32)data;
      if (!(data & 0x1))
      {
        /* valid bit is not set */
        madSemGive(dev,dev->ptpRegsSem);
        return retVal;
      }

      for(i=1; i<opData->nData; i++)
      {
        opData->ptpAddr=kpReg+i;
        retVal = mapRegs(opData, &port, &page, &reg);
        if(retVal != MAD_OK)
        {
          madSemGive(dev,dev->ptpRegsSem);
      return retVal;
        }
    retVal = madHwReadPagedPhyReg(dev,port,page,reg,&data);
        opData->ptpMultiData[i] = (MAD_U32)data;
        if(retVal != MAD_OK)
        {
          madSemGive(dev,dev->ptpRegsSem);
          return retVal;
    }
        opData->ptpAddr++;
      }

      data =0;
      opData->ptpAddr = kpReg;
      retVal = mapRegs(opData, &port, &page, &reg);
      if(retVal != MAD_OK)
      {
        madSemGive(dev,dev->ptpRegsSem);
    return retVal;
      }


      if((retVal = madHwWritePagedPhyReg(dev,port,page,reg,data))!= MAD_OK)
      {
        MAD_DBG_ERROR("Writing to paged phy reg (%i,%i) failed.\n",page,reg);
        madSemGive(dev,dev->ptpRegsSem);
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
      }

      madSemGive(dev,dev->ptpRegsSem);
      break;

    default:
      madSemGive(dev,dev->ptpRegsSem);
      return MAD_FAIL;
  }

  madSemGive(dev,dev->ptpRegsSem);
  return MAD_OK;
}





