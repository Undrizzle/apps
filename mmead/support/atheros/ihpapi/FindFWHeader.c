/*====================================================================*
 *
 *   NVMBlockHeader * ihp_FindFWHeader (const uint8_t FW_pbuffer [], size_t length);
 *
 *   ihp.h
 *
 *   Search an NVM header chain for the firmware header and return
 *   the header address or 0 on failure; failure can occur because
 *   an invalid NVM header is found or the buffer is too short; 
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

#ifndef FINDFWHEADER_SOURCE
#define FINDFWHEADER_SOURCE

#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#include "ihp.h"
#include "ihpapi.h"

#include "../tools/memory.h"
 
NVMBlockHeader *ihp_FindFWHeader (const uint8_t FW_pbuffer [], size_t length) 

{
	NVMBlockHeader * header;
	int isLegacy;

	isLegacy = ihp_isLegacyNVM(FW_pbuffer, length);

	if (-1 == isLegacy)
	{
		return (0);
	}
	if (isLegacy)
	{
		for (header = (NVMBlockHeader *)(FW_pbuffer); (uint8_t *)(header) < (FW_pbuffer + length); header = (NVMBlockHeader *)(FW_pbuffer + intohl(header->NEXTHEADER))) 
		{
			if (!header->NEXTHEADER) 
			{
				return (header);
			}
		}
	}
	else
	{
		header = ihp_FindNVMObject (FW_pbuffer, length, nvmBlockHeaderEntryType_Firmware);
		if (header)
		{
			return ( header );
		}
	}
	errno = EFAULT;
	return (0);
}

#endif
 

