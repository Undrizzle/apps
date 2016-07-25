/*====================================================================*
 *
 *   size_t ihpapi_GetVersionInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []);
 *
 *   ihpapi.h
 *
 *   Encode buffer as an Intellon VS_SW_VER Management Message and
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

#ifndef BANDWIDTHLIMITING_SOURCE
#define BANDWIDTHLIMITING_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"
 
size_t ihpapi_Bandwidthlimiting (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [],uint8_t direction,uint8_t speed) 

{
	size_t offset = 0;
	offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, da, sa);
	offset += EncodeIntellonHeader (buffer + offset, bufferLen - offset, (VS_BANDWIDTH_LIMITING | MMTYPE_REQ));
	if (offset < sizeof (struct header_vs)) 
	{
		return (0);
	}
	if (bufferLen < IHPAPI_ETHER_MIN_LEN) 
	{
		errno = ERANGE;
		return (0);
	}
	if(direction == 1)
	{
		offset = 0x18;		//for uplink
		switch(speed)
		{	
			case 1:
				memset(buffer + offset,0x0000000,4);	//disable
			case 2:
				memset(buffer + offset,0x00010000,4);	//64K
			case 3:
				memset(buffer + offset,0x01000000,4);	//16M
		}
	}else if(direction == 2)
	{
		offset = 0x1C;		//for downlink
		switch(speed)
		{
			case 1:
				memset(buffer + offset,0x0000000,4);	//disable
			case 2:
				memset(buffer + offset,0x00010000,4);	//64K
			case 3:
				memset(buffer + offset,0x01000000,4);	//16M
		}
	}

	memset (buffer + offset, 0, IHPAPI_ETHER_MIN_LEN - offset);
	return (IHPAPI_ETHER_MIN_LEN);
}

#endif
 

