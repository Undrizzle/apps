/*====================================================================*
 *
 *   int ihp_DecodeWriteMemory (const uint8_t buffer [], size_t length, ihpapi_result_t * result, WrtMemBlock * wrmemblk);
 *
 *   ihp.h
 *
 *   decode buffer as a VS_WR_MEM Confirm MME; update the structure
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
 *	Alex Vasquez, alex.vasquez@intellon.com 
 *
 *--------------------------------------------------------------------*/

#ifndef DECODEWRITEMEMORY_SOURCE
#define DECODEWRITEMEMORY_SOURCE

#include <stdint.h>
#include <errno.h>

#include "../ihpapi/ihp.h"
 
int ihp_DecodeWriteMemory (const uint8_t buffer [], size_t length, ihpapi_result_t * result, WrtMemBlock * wrmemblk) 

{
	struct __packed vs_wr_mem_cnf 
	{
		struct header_vs header;
		uint8_t MSTATUS;
		uint32_t WADDRESS;
		uint32_t WLENGTH;
	}
	* confirm = (struct vs_wr_mem_cnf *)(buffer);

#if INTELLON_SAFEMODE
 
	if (length < sizeof (struct vs_wr_mem_cnf)) 
	{
		errno = ERANGE;
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		errno = EFAULT;
		return (-1);
	}
	if (wrmemblk == (WrtMemBlock *)(0)) 
	{
		errno = EFAULT;
		return (-1);
	}

#endif
 
	result->opStatus.status = confirm->MSTATUS;
	wrmemblk->cnf.WADDRESS  = intohl(confirm->WADDRESS);
	wrmemblk->cnf.WLENGTH   = intohl(confirm->WLENGTH);
	return (0);
}

#endif
 

