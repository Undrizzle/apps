/*====================================================================*
 *
 *   size_t ihp_EncodeSetSDRAMCfg (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], size_t sdlen, uint8_t sdlen []);
 *
 *   ihp.h
 *
 *   Encode buffer as an Intellon VS_SET_CFG Management Message and
 *   return the number of bytes encoded or 0 on encoding error; the
 *   error code is stored in errno and returned in status;
 *
 *   The buffer is automatically padded to ETHER_MIN_LEN bytes and
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
 *	Charles Maier, charles.maier@intellon.com
 *
 *--------------------------------------------------------------------*/

#ifndef ENCODESETSDRAMCFG_SOURCE
#define ENCODESETSDRAMCFG_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"
#include "../tools/memory.h"
 
size_t ihp_EncodeSetSDRAMCfg (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], size_t sdlen, uint8_t sd []) 

{
	struct __packed vs_set_cfg_req 
	{
		struct header_vs header;
		uint8_t SDRAMCONFIG [32];
		uint32_t CHECKSUM;
	}
	* request = (struct vs_set_cfg_req *)(buffer);

	size_t offset = 0;

#if INTELLON_SAFEMODE
 
	if (bufferLen < IHPAPI_ETHER_MIN_LEN || sdlen != sizeof (request->SDRAMCONFIG) || bufferLen < sizeof (struct vs_set_cfg_req)) 
	{
		errno = ERANGE;
		return (0);
	}

#endif
 
	offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, da, sa);
	offset += EncodeIntellonHeader (buffer + offset, bufferLen - offset, (VS_SET_SDRAM | MMTYPE_REQ));
	if (offset < sizeof (struct header_vs)) 
	{
		return (0);
	}
	memcpy (request->SDRAMCONFIG, sd, sizeof (request->SDRAMCONFIG));
	request->CHECKSUM = ihtonl(checksum32 ((const uint32_t *)(request->SDRAMCONFIG), sizeof (request->SDRAMCONFIG) >> 2, 0));
	offset += sizeof (request->SDRAMCONFIG) + sizeof (request->CHECKSUM);
	if (offset < IHPAPI_ETHER_MIN_LEN) 
	{
		memset (buffer + offset, 0, IHPAPI_ETHER_MIN_LEN - offset);
		offset = IHPAPI_ETHER_MIN_LEN;
	}
	return (offset);
}

#endif
 

