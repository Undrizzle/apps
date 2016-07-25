/*====================================================================*
 *
 *   int ihp_DecodeHostAction (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
 *
 *   ihp.h
 *
 *   decode buffer as a VS_HOST_ACTION Indication MME; update the 
 *   structure named by the caller; 
 *
 *   return 0 on decode success or -1 on decode failure;
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
 *	Charles Maier, charles.maier@intellon.com
 *
 *--------------------------------------------------------------------*/

#ifndef DECODEHOSTACTION_SOURCE
#define DECODEHOSTACTION_SOURCE

#include <stdint.h>

#include "../ihpapi/ihp.h"
 
int ihp_DecodeHostAction (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	struct vs_host_action_ind 
	{
		struct header_vs header;
		uint8_t HOST_ACTION_REQ;
	}
	* indication = (struct vs_host_action_ind *)(buffer);
	if (length < sizeof (struct vs_host_action_ind)) 
	{
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		return (-1);
	}
	result->data.hostAction.request = indication->HOST_ACTION_REQ;
	result->dataLen = 1;
	result->validData = true;
	result->opStatus.status = 0;
	return (0);
}

#endif
 

