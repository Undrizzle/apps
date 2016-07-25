/*====================================================================*
 *
 *   size_t ihpapi_GetNetworkInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
 *
 *   ihpapi.h
 *
 *   Encode buffer as an Intellon VS_NE_INFO Management Message and
 *   return the number of bytes encoded or 0 on encoding error;
 *
 *   The buffer is automatically padded to IHPAPI_ETHER_MIN_LEN bytes and
 *   is ready for Ethernet transmission;
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

#ifndef GETNETWORKINFOSTATS_SOURCE
#define GETNETWORKINFOSTATS_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"
 
size_t ihpapi_GetNetworkInfoStats (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []) 

{
	size_t offset = 0;
	
	offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, da, sa);
	offset += EncodeV1IntellonHeader (buffer + offset, bufferLen - offset, (VS_NW_INFO_STATS | MMTYPE_REQ));
	if (offset < sizeof (struct header_v1_vs)) 
	{
		return (0);
	}
	if (bufferLen < IHPAPI_ETHER_MIN_LEN) 
	{
		errno = ERANGE;
		return (0);
	}
	if (offset < IHPAPI_ETHER_MIN_LEN) 
	{
		memset (buffer + offset, 0, IHPAPI_ETHER_MIN_LEN - offset);
		//add by stan for 74 0xa074
		offset+=4;
		buffer[offset] = 0x00;
		offset++;
		buffer[offset] = 0x3c;
	}
	
	return (IHPAPI_ETHER_MIN_LEN);
}

#endif
 

