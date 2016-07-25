/*====================================================================*
 *
 *   size_t ihpapi_SetKey (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_key_t * pkeyParm);
 *
 *   ihpapi.h
 *
 *   Encode buffer as an Intellon VS_SET_KEY Management Message and
 *   return the number of bytes encoded or 0 on encoding error; the
 *   error code is stored in errno and returned in status;
 *
 *   The particular message occupies exactly IHPAPI_ETHER_MIN_LEN bytes and
 *   so there is no need to pad the buffer; 
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

#ifndef SETKEY_SOURCE
#define SETKEY_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"
 
size_t ihpapi_SetKey (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_key_t * pKeyParm) 

{
	struct __packed vs_set_key_req 
	{
		struct header_vs header;
		uint8_t EKS;
		uint8_t NMK [KEY_MAX_LEN];
		uint8_t PEKS;
		uint8_t RDA [IHPAPI_ETHER_ADDR_LEN];
		uint8_t DAK [KEY_MAX_LEN];
	}
	* request = (struct vs_set_key_req *)(buffer);
	size_t offset = 0;
	offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, da, sa);
	offset += EncodeIntellonHeader (buffer + offset, bufferLen - offset, (VS_SET_KEY | MMTYPE_REQ));
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
	request->EKS = 0x01;
	switch (pKeyParm->peks) 
	{
	case 0x00:
	case 0x0F:
		break;
	default:
		errno = EINVAL;
		return (0);
	}
	request->PEKS = pKeyParm->peks;
	memcpy (request->NMK, pKeyParm->nmk, KEY_MAX_LEN);
	memcpy (request->RDA, pKeyParm->rda, IHPAPI_ETHER_ADDR_LEN);
	memcpy (request->DAK, pKeyParm->dak, KEY_MAX_LEN);
	return (IHPAPI_ETHER_MIN_LEN);
}

#endif
 

