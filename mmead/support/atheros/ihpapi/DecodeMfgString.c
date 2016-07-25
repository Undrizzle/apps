/*====================================================================*
 *
 *   int ihp_DecodeMfgString (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
 *
 *   ihp.h
 *
 *   decode buffer as a VS_MFG_STRING Confirm MME; update the structure 
 *   named by the caller; 
 *
 *   return 0 on decode success or -1 on decode error;
 * 
 *   EFAULT if buffer/result address or size are invalid or illegal;
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

#ifndef DECODEMFGSTRING_SOURCE
#define DECODEMFGSTRING_SOURCE

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "../ihpapi/ihp.h"
 
int ihp_DecodeMfgString (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	struct __packed vs_mfg_string_cnf 
	{
		struct header_vs header;
		uint8_t MSTATUS;
		uint8_t MSTRLENGTH;
		uint8_t MSTRING [64];
	}
	* confirm = (struct vs_mfg_string_cnf *)(buffer);
	ihpapi_getManufacturerInfoData_t * message = &result->data.mftrInfo;
	if (length < sizeof (struct vs_mfg_string_cnf)) 
	{
		return (-1);
		errno = EFAULT;
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		return (-1);
		errno = EFAULT;
	}
	message->mstrlength = confirm->MSTRLENGTH;
	memcpy (message->mstring, confirm->MSTRING, sizeof (message->mstring));
	result->dataLen = sizeof (ihpapi_getManufacturerInfoData_t);
	result->validData = true;
	result->opStatus.status = confirm->MSTATUS;
	return (0);
}

#endif
 

