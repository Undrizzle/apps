/*====================================================================*
 *
 *   int ihp_DecodeEthernetPHYSettings (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
 *
 *   ihp.h
 *
 *   decode buffer as a VS_ENET_SETTINGS Confirm MME; update the structure 
 *   named by the caller; 
 *
 *   return 0 on decode success and -1 on decode error;
 *
 *   See the INT6000 Firmware Technical Reference Manual
 *   for more information;
 *
 *   This software and documentation is the property of Intellon 
 *   Corporation, Ocala, Florida. It is provided 'as is' without 
 *   expressed or implied warranty of any kind to anyone for any 
 *   reason. Intellon assumes no responsibility or liability for 
 *   errors or omissions in the software or documentation and 
 *   reserves the right to make changes without notification. 
 *   
 *   Intellon customers may modify and distribute the software 
 *   without obligation to Intellon. Since use of this software 
 *   is optional, users shall bear sole responsibility and 
 *   liability for any consequences of it's use. 
 *
 *.  Intellon HomePlug AV Application Programming Interface;
 *:  Published 2007 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use;
 *
 *   Contributor(s): 
 *	Alex Vasquez, alex.vasquez@intellon.com
 *
 *--------------------------------------------------------------------*/

#ifndef DECODEETHERNETPHYSETTINGS_SOURCE
#define DECODEETHERNETPHYSETTINGS_SOURCE

#include <stdint.h>
#include <string.h>

#include "../ihpapi/ihp.h"
 
int ihp_DecodeEthernetPHYSettings (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	struct vs_enet_settings_cnf 
	{
		struct header_vs header;
		uint8_t MSTATUS;
		uint8_t ESPEED;
		uint8_t EDUPLEX;
		uint8_t ELINK_STATUS;
		uint8_t EFLOW_CONTROL;
	}
	*confirm = (struct vs_enet_settings_cnf *)(buffer);
	ihpapi_manageEnetPHYData_t * message = &result->data.enetPHY;
	if (result == (ihpapi_result_t *)(0)) 
	{
		return (-1);
	}
	message->espeed = confirm->ESPEED;
	message->eduplex = confirm->EDUPLEX;
	message->elinkstatus = confirm->ELINK_STATUS;
	message->eflowcontrol = confirm->EFLOW_CONTROL;
	result->validData = true;
	result->opStatus.status = confirm->MSTATUS;
	return (0);
}

#endif
 

