/*====================================================================*
 *
 *   size_t ihpapi_GetToneMapInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_toneMapCtl_t * inputToneMapInfo);
 *
 *   ihpapi.h
 *
 *   Encode buffer as an Intellon VS_TONE_MAP_CHAR Management Message and
 *   return the number of bytes encoded or 0 on encoding error; the
 *   error code is stored in errno and returned in status;
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
 *	Alex Vasquez, alex.vasquez@intellon.com
 *
 *--------------------------------------------------------------------*/

#ifndef GETTONEMAP_SOURCE
#define GETTONEMAP_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"
 
size_t ihpapi_GetToneMapInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_toneMapCtl_t * inputToneMapInfo) 

{
	struct __packed vs_tone_map_char_req 
	{
		struct header_vs header;
		uint8_t MACADDRESS [IHPAPI_ETHER_ADDR_LEN];
		uint8_t TMSLOT;
	}
	* request = (struct vs_tone_map_char_req *)(buffer);
	size_t offset = 0;
	offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, da, sa);
	offset += EncodeIntellonHeader (buffer + offset, bufferLen - offset, (VS_TONE_MAP_CHAR | MMTYPE_REQ));
	if (offset < sizeof (struct header_vs)) 
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
	}
	if (inputToneMapInfo == (ihpapi_toneMapCtl_t *)(0)) 
	{
		errno = EFAULT;
		return (0);
	}
	request->TMSLOT = inputToneMapInfo->tmslot;
	memcpy (request->MACADDRESS, inputToneMapInfo->macaddress, IHPAPI_ETHER_ADDR_LEN);
	return (IHPAPI_ETHER_MIN_LEN);
}

size_t ihpapi_Get74ToneMapInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_toneMapCtl_t * inputToneMapInfo) 

{
	struct __packed vs_tone_map_char_req 
	{
		struct header_v1_vs header;
		uint8_t SUBVER;
		uint8_t RESERVED[3];
		uint8_t MACADDRESS [IHPAPI_ETHER_ADDR_LEN];
		uint8_t TMSLOT;
		uint8_t COUPLINGSEL;
	}
	* request = (struct vs_tone_map_char_req *)(buffer);
	size_t offset = 0;
	offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, da, sa);
	offset += EncodeV1IntellonHeader (buffer + offset, bufferLen - offset, (VS_TONE_MAP_CHAR | MMTYPE_REQ));
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
	}
	if (inputToneMapInfo == (ihpapi_toneMapCtl_t *)(0)) 
	{
		errno = EFAULT;
		return (0);
	}
	request->SUBVER = 0;
	request->RESERVED[0] = 0;
	request->RESERVED[1] = 0;
	request->RESERVED[2] = 0;
	request->COUPLINGSEL = 0;
	request->TMSLOT = inputToneMapInfo->tmslot;
	memcpy (request->MACADDRESS, inputToneMapInfo->macaddress, IHPAPI_ETHER_ADDR_LEN);
	return (IHPAPI_ETHER_MIN_LEN);
}

#endif
 

