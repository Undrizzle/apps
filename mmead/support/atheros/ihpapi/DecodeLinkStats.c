/*====================================================================*
 *
 *   int ihp_DecodeLinkStats (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
 *
 *   ihp.h
 *
 *   decode buffer as a VS_LNK_STATS Confirm MME; update the structure 
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
 *	Charles Maier, charles.maier@intellon.com
 *	Alex Vasquez, alex.vasquez@intellon.com
 *
 *--------------------------------------------------------------------*/

#ifndef DECODELINKSTATS_SOURCE
#define DECODELINKSTATS_SOURCE

#include <stdint.h>
#include <string.h>

#include "../ihpapi/ihp.h"
 
int ihp_DecodeLinkStats (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	struct vs_lnk_stats_cnf 
	{
		struct header_vs header;
		int8_t MSTATUS;
		int8_t DIRECTION;
		int8_t LID;
		int8_t TEI;
		int8_t LSTATS [287];
	}
	*confirm = (struct vs_lnk_stats_cnf *)(buffer);
	ihpapi_getConnectInfoData_t * message = &result->data.connInfo;
	if (result == (ihpapi_result_t *)(0)) 
	{
		return (-1);
	}
	message->direction = confirm->DIRECTION;
	message->lid = confirm->LID;
	message->tei = confirm->TEI;
	if (confirm->DIRECTION == 0) 
	{
		memcpy (message->lstats, confirm->LSTATS, 40);
		result->dataLen = 43;
	}
	if (confirm->DIRECTION == 1) 
	{
		memcpy (message->lstats, confirm->LSTATS, 247);
		result->dataLen = 250;
	}
	if (confirm->DIRECTION == 2) 
	{
		memcpy (message->lstats, confirm->LSTATS, 287);
		result->dataLen = sizeof (ihpapi_getConnectInfoData_t);
	}
	result->validData = true;
	result->opStatus.status = confirm->MSTATUS;
	return (0);
}

#endif
 

