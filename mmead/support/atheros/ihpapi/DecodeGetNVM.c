/*====================================================================*
 *
 *   int ihp_DecodeGetNVM (const uint8_t buffer [], size_t length, ihpapi_result_t * result, struct MemoryInfo * memory);
 *
 *   ihp.h
 *
 *   decode buffer as a VS_GET_NVM Confirm MME; update the structure
 *   named by the caller; 
 *
 *   return 0 on decode success or -1 on decode error;
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

#ifndef DECODEGETNVM_SOURCE
#define DECODEGETNVM_SOURCE

#include <stdint.h>

#include "../ihpapi/ihp.h"
 
int ihp_DecodeGetNVM (const uint8_t buffer [], size_t length, ihpapi_result_t * result, struct MemoryInfo * memory) 

{
	struct __packed vs_get_nvm_cnf 
	{
		struct header_vs header;
		uint8_t MSTATUS;
		uint32_t NVMTYPE;
		uint32_t NVMPAGE;
		uint32_t NVMBLOCK;
		uint32_t NVMSIZE;
	}
	* confirm = (struct vs_get_nvm_cnf *)(buffer);
	if (length < sizeof (struct vs_get_nvm_cnf)) 
	{
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		return (-1);
	}
	if (memory == (struct MemoryInfo *)(0)) 
	{
		return (-1);
	}
	result->opStatus.status = confirm->MSTATUS;
	memory->NVMTYPE  = intohl(confirm->NVMTYPE);
	memory->NVMPAGE  = intohl(confirm->NVMPAGE);
	memory->NVMBLOCK = intohl(confirm->NVMBLOCK);
	memory->NVMSIZE  = intohl(confirm->NVMSIZE);
	return (0);
}

#endif
 

