/*====================================================================*
 *
 *   int ihp_DecodeFlashNVM (const uint8_t buffer [], size_t length, ihpapi_result_t * result, BlockInfo * block);
 *
 *   ihp.h
 *
 *   decode buffer as a VS_MOD_NVM Confirm MME; update structures 
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

#ifndef DECODEFLASHNVM_SOURCE
#define DECODEFLASHNVM_SOURCE

#include <stdint.h>
#include <string.h>

#include "../ihpapi/ihp.h"
#include "../ihpapi/ihpapi.h"
 
int ihp_DecodeFlashNVM (const uint8_t buffer [], size_t length, ihpapi_result_t * result, struct BlockInfo * block) 

{
	struct __packed vs_mod_nvm_cnf 
	{
		struct header_vs header;
		uint8_t MSTATUS;
		uint8_t MODULEID;
	}
	* confirm = (struct vs_mod_nvm_cnf *)(buffer);
	if (length < sizeof (struct vs_mod_nvm_cnf)) 
	{
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		return (-1);
	}
	if (block == (BlockInfo *)(0)) 
	{
		return (-1);
	}
	result->validData = true;
	result->opStatus.status = confirm->MSTATUS;
	memset (block, 0, sizeof (BlockInfo));
	block->block.flshnvmblk.MODULEID = confirm->MODULEID;
	return (0);
}

#endif
 

