#include <madCopyright.h>

/*******************************************************************************
* madPTP.c
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

#define madPtpDefaultPort 2


/*******************************************************************************
* mdPtpSetEnable
*
* DESCRIPTION:
*       This routine set all ports PTP Enable/Disable.
*
* INPUTS:
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
MAD_STATUS mdPtpSetEnable
(
  IN  MAD_DEV 	*dev,
  IN  MAD_BOOL	en
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = mdPtpSetEnable_port(dev, port, en);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpGetEnable
*
* DESCRIPTION:
*       This routine Get default port PTP Enable/Disable.
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
MAD_STATUS mdPtpGetEnable
(
  IN  MAD_DEV 	*dev,
  OUT  MAD_BOOL	*en
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
   
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetEnable_port(dev, port, en);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpSetIntEnable
*
* DESCRIPTION:
*       This routine set all ports PTP interrupt Enable/Disable.
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
MAD_STATUS mdPtpSetIntEnable
(
  IN  MAD_DEV 	*dev,
  IN  MAD_BOOL	en
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = mdPtpSetIntEnable_port(dev, port, en);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpGetIntEnable
*
* DESCRIPTION:
*       This routine get default port PTP Interrupt Enable/Disable.
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
MAD_STATUS mdPtpGetIntEnable
(
  IN  MAD_DEV 	*dev,
  OUT  MAD_BOOL	*en
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetIntEnable_port(dev, port, en);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpSetConfig
*
* DESCRIPTION:
*       This routine writes all ports PTP configuration parameters.
*
* INPUTS:
*       port - The logical port number
*		ptpData  - PTP configuration parameters.
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
MAD_STATUS mdPtpSetConfig
(
  IN  MAD_DEV 	*dev,
  IN  MAD_PTP_CONFIG	*ptpData
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = mdPtpSetConfig_port(dev, port, ptpData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}


/*******************************************************************************
* mdPtpGetConfig
*
* DESCRIPTION:
*       This routine reads default port PTP configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*		ptpData  - PTP configuration parameters.
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
MAD_STATUS mdPtpGetConfig
(
  IN  MAD_DEV 	*dev,
  OUT MAD_PTP_CONFIG	*ptpData
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetConfig_port(dev, port, ptpData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}

/*******************************************************************************
* mdPtpSetGlobalConfig
*
* DESCRIPTION:
*       This routine writes all ports PTP global configuration parameters.
*
* INPUTS:
*       port - The logical port number
*		ptpData  - PTP global configuration parameters.
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
MAD_STATUS mdPtpSetGlobalConfig
(
  IN  MAD_DEV 	*dev,
  IN  MAD_PTP_GLOBAL_CONFIG	*ptpData
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = mdPtpSetGlobalConfig_port(dev, port, ptpData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* mdPtpGlobalGetConfig
*
* DESCRIPTION:
*       This routine reads default port PTP global configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*		ptpData  - PTP global configuration parameters.
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
MAD_STATUS mdPtpGetGlobalConfig
(
  IN  MAD_DEV 	*dev,
  OUT MAD_PTP_GLOBAL_CONFIG	*ptpData
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetGlobalConfig_port(dev, port, ptpData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}

/*******************************************************************************
* mdPtpSetPTPEn
*
* DESCRIPTION:
*       This routine enables or disables all ports PTP.
*
* INPUTS:
*       en - MAD_TRUE to enable PTP, MAD_FALSE to disable PTP
*
* OUTPUTS:
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
MAD_STATUS mdPtpSetPTPEn
(
  IN  MAD_DEV   *dev,
  IN  MAD_BOOL          en
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = mdPtpSetPTPEn_port(dev, port, en);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}
/*******************************************************************************
* mdPtpGetPTPEn
*
* DESCRIPTION:
*       This routine checks if default port PTP is enabled.
*
* INPUTS:
*       None
* OUTPUTS:
*       en - MAD_TRUE if enabled, MAD_FALSE otherwise
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
MAD_STATUS mdPtpGetPTPEn
(
  IN  MAD_DEV   *dev,
  OUT MAD_BOOL          *en
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetPTPEn_port(dev, port, en);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}

/*******************************************************************************
* mdPtpGetPTPInt
*
* DESCRIPTION:
*       This routine gets default port PTP interrupt status for each port.
*	The PTP Interrupt bit gets set for a given port when an incoming PTP 
*	frame is time stamped and PTPArrIntEn for that port is set to 0x1.
*	Similary PTP Interrupt bit gets set for a given port when an outgoing
*	PTP frame is time stamped and PTPDepIntEn for that port is set to 0x1.
*	This bit gets cleared upon software reading and clearing the corresponding
*	time counter valid bits that are valid for that port.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*	ptpInt 	- interrupt status for each port (bit 0 for port 0, bit 1 for port 1, etc.)
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
MAD_STATUS mdPtpGetPTPInt
(
  IN  MAD_DEV 	*dev,
  OUT MAD_U32		*ptpInt
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetPTPInt_port(dev, port, ptpInt);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}


/*******************************************************************************
* mdPtpGetPTPGlobalTime
*
* DESCRIPTION:
*       This routine gets the default port global timer value that is running off of the free
*		running switch core clock.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*		ptpTime	- PTP global time
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
MAD_STATUS mdPtpGetPTPGlobalTime
(
  IN  MAD_DEV 	*dev,
  OUT MAD_U32		*ptpTime
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetPTPGlobalTime_port(dev, port, ptpTime);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* madTaiSetEventConfig
*
* DESCRIPTION:
*       This routine sets all ports TAI Event Capture configuration parameters.
*
* INPUTS:
*		eventData  - TAI Event Capture configuration parameters.
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
MAD_STATUS madTaiSetEventConfig
(
  IN  MAD_DEV 	*dev,
  IN  MAD_TAI_EVENT_CONFIG	*eventData
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetEventConfig_port(dev, port, eventData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}


/*******************************************************************************
* madTaiGetEventConfig
*
* DESCRIPTION:
*       This routine gets default port TAI Event Capture configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*		eventData  - TAI Event Capture configuration parameters.
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
MAD_STATUS madTaiGetEventConfig
(
  IN  MAD_DEV 	*dev,
  OUT MAD_TAI_EVENT_CONFIG	*eventData
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetEventConfig_port(dev, port, eventData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}


/*******************************************************************************
* madTaiGetEventStatus
*
* DESCRIPTION:
*       This routine gets default port TAI Event Capture status.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*		eventData  - TAI Event Capture configuration parameters.
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
MAD_STATUS madTaiGetEventStatus
(
  IN  MAD_DEV 	*dev,
  OUT MAD_TAI_EVENT_STATUS	*status
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetEventStatus_port(dev, port, status);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}


/*******************************************************************************
* madTaiGetEventInt
*
* DESCRIPTION:
*       This routine gets default port TAI Event Capture Interrupt status.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*		intStatus 	- interrupt status for TAI Event capture
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
MAD_STATUS madTaiGetEventInt
(
  IN  MAD_DEV 	*dev,
  OUT MAD_BOOL		*intStatus
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetEventInt_port(dev, port, intStatus);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* madTaiGetTrigInt
*
* DESCRIPTION:
*       This routine gets default port TAI Trigger Interrupt status.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*		intStatus 	- interrupt status for TAI Trigger
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
MAD_STATUS madTaiGetTrigInt
(
  IN  MAD_DEV 	*dev,
  OUT MAD_BOOL		*intStatus
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetTrigInt_port(dev, port, intStatus);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* madTaiSetTrigConfig
*
* DESCRIPTION:
*       This routine sets all ports TAI Trigger configuration parameters.
*
* INPUTS:
*		trigEn    - enable/disable TAI Trigger.
*		trigData  - TAI Trigger configuration parameters (valid only if trigEn is MAD_TRUE).
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
MAD_STATUS madTaiSetTrigConfig
(
  IN  MAD_DEV 	*dev,
  IN  MAD_BOOL 	trigEn,
  IN  MAD_TAI_TRIGGER_CONFIG	*trigData
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetTrigConfig_port(dev, port, trigEn, trigData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}

/*******************************************************************************
* madTaiGetTrigConfig
*
* DESCRIPTION:
*       This routine gets default port TAI Trigger configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*		trigEn    - enable/disable TAI Trigger.
*		trigData  - TAI Trigger configuration parameters (valid only if trigEn is MAD_TRUE).
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
MAD_STATUS madTaiGetTrigConfig
(
  IN  MAD_DEV 	*dev,
  OUT MAD_BOOL 	*trigEn,
  OUT MAD_TAI_TRIGGER_CONFIG	*trigData
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetTrigConfig_port(dev, port, trigEn, trigData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* madTaiGetTSClkPer
*
* DESCRIPTION:
* 	 default port Time Stamping Clock Period in pico seconds.
*	This routine specifies the clock period for the time stamping clock supplied 
*	to the PTP hardware logic.
*	This is the clock that is used by the hardware logic to update the PTP 
*	Global Time Counter.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*		clk		- time stamping clock period
*
* RETURNS:
* 		MAD_OK      - on success
* 		MAD_FAIL    - on error
* 		MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
MAD_STATUS madTaiGetTSClkPer
(
  IN  MAD_DEV 	*dev,
  OUT MAD_U32		*clk
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetTSClkPer_port(dev, port, clk);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* madTaiSetTSClkPer
*
* DESCRIPTION:
* 	all ports Time Stamping Clock Period in pico seconds.
*	This routine specifies the clock period for the time stamping clock supplied 
*	to the PTP hardware logic.
*	This is the clock that is used by the hardware logic to update the PTP 
*	Global Time Counter.
*
* INPUTS:
*	clk		- time stamping clock period
*
* OUTPUTS:
*       port - The logical port number
*
* RETURNS:
* 	MAD_OK      - on success
* 	MAD_FAIL    - on error
* 	MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
* 	None
*
*******************************************************************************/
MAD_STATUS madTaiSetTSClkPer
(
  IN  MAD_DEV 	*dev,
  IN  MAD_U32		clk
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetTSClkPer_port(dev, port, clk);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* madTaiSetMultiPTPSync
*
* DESCRIPTION:
* 	This routine sets all ports Multiple PTP device sync mode and sync time (TrigGenAmt).
*	When enabled, the hardware logic detects a low to high transition on the 
*	EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*	register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*       port - The logical port number
*	multiEn		- enable/disable Multiple PTP device sync mode
*	syncTime	- sync time (valid only if multiEn is MAD_TRUE)
*
* OUTPUTS:
*	None.
*
* RETURNS:
*	MAD_OK      - on success
*	MAD_FAIL    - on error
*	MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
* 	When enabled, madTaiSetTrigConfig, madTaiSetEventConfig, madTaiSetTimeInc,
*	and madTaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
MAD_STATUS madTaiSetMultiPTPSync
(
  IN  MAD_DEV 	*dev,
  IN  MAD_BOOL 	multiEn,
  IN  MAD_32		syncTime
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetMultiPTPSync_port(dev, port, multiEn, syncTime);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}


/*******************************************************************************
* madTaiGetMultiPTPSync
*
* DESCRIPTION:
*	This routine sets default port Multiple PTP device sync mode and sync time (TrigGenAmt).
*	When enabled, the hardware logic detects a low to high transition on the 
*	EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*	register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*	multiEn		- enable/disable Multiple PTP device sync mode
*	syncTime	- sync time (valid only if multiEn is MAD_TRUE)
*
* RETURNS:
*	MAD_OK      - on success
*	MAD_FAIL    - on error
*	MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
* 	When enabled, madTaiSetTrigConfig, madTaiSetEventConfig, madTaiSetTimeInc,
*	and madTaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
MAD_STATUS madTaiGetMultiPTPSync
(
  IN  MAD_DEV 	*dev,
  OUT MAD_BOOL 	*multiEn,
  OUT MAD_32		*syncTime
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetMultiPTPSync_port(dev, port, multiEn, syncTime);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}


/*******************************************************************************
* madTaiGetTimeIncDec
*
* DESCRIPTION:
*	This routine retrieves default port Time increment/decrement setup.
*	This amount specifies the number of units of PTP Global Time that need to be 
*	incremented or decremented. This is used for adjusting the PTP Global Time 
*	counter value by a certain amount.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*		expired	- MAD_TRUE if inc/dec occurred, MAD_FALSE otherwise
*		inc		- MAD_TRUE if increment, MAD_FALSE if decrement
*		amount	- increment/decrement amount
*
* RETURNS:
* 		MAD_OK      - on success
* 		MAD_FAIL    - on error
* 		MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
* 		Time increment or decrement will be excuted once.
*
*******************************************************************************/
MAD_STATUS madTaiGetTimeIncDec
(
  IN  MAD_DEV 	*dev,
  OUT MAD_BOOL		*expired,
  OUT MAD_BOOL		*inc,
  OUT MAD_U32		*amount
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetTimeIncDec_port(dev, port, expired, inc, amount);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* madTaiSetTimeInc
*
* DESCRIPTION:
* 	This routine enables all ports time increment by the specifed time increment amount.
*	The amount specifies the number of units of PTP Global Time that need to be 
*	incremented. This is used for adjusting the PTP Global Time counter value by
*	a certain amount.
*	Increment occurs just once.
*
* INPUTS:
*	amount	- time increment amount (0 ~ 0x7FF)
*
* OUTPUTS:
*       port - The logical port number
*
* RETURNS:
* 	MAD_OK      - on success
* 	MAD_FAIL    - on error
* 	MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
* 	None
*
*******************************************************************************/
MAD_STATUS madTaiSetTimeInc
(
  IN  MAD_DEV 	*dev,
  IN  MAD_U32		amount
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetTimeInc_port(dev, port, amount);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* madTaiSetTimeDec
*
* DESCRIPTION:
* 	This routine enables all ports time decrement by the specifed time decrement amount.
*	The amount specifies the number of units of PTP Global Time that need to be 
*	decremented. This is used for adjusting the PTP Global Time counter value by
*	a certain amount.
*	Decrement occurs just once.
*
* INPUTS:
*       port - The logical port number
*	amount	- time decrement amount (0 ~ 0x7FF)
*
* OUTPUTS:
* 	None.
*
* RETURNS:
* 	MAD_OK      - on success
* 	MAD_FAIL    - on error
* 	MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
* 	None
*
*******************************************************************************/
MAD_STATUS madTaiSetTimeDec
(
  IN  MAD_DEV 	*dev,
  IN  MAD_U32		amount
)
{
  MAD_STATUS       	retVal;
  MAD_LPORT	port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetTimeDec_port(dev, port, amount);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;


}
