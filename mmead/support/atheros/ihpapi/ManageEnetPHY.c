/*====================================================================*
 *
 *   size_t ihpapi_ManageEnetPHY (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_enetPHYCtl_t * inputEnetPHYInfo);
 *
 *   ihpapi.h
 *
 *   Encode buffer as an Intellon VS_ENET_SETTINGS Management Message and
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

#ifndef MANAGEENETPHY_SOURCE
#define MANAGEENETPHY_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"
 
size_t ihpapi_ManageEnetPHY (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_enetPHYCtl_t * inputEnetPHYInfo) 

{
	struct __packed vs_enet_settings_req 
	{
		struct header_vs header;
		uint8_t MCONTROL;
		uint8_t AUTONEGOTIATE;
		uint8_t ADVCAPS;
		uint8_t ESPEED;
		uint8_t EDUPLEX;
		uint8_t EFLOW_CONTROL;
	}
	* request = (struct vs_enet_settings_req *)(buffer);
	size_t offset = 0;
	offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, da, sa);
	offset += EncodeIntellonHeader (buffer + offset, bufferLen - offset, (VS_ENET_SETTINGS | MMTYPE_REQ));
	if (offset < sizeof (struct header_vs)) 
	{
		return (0);
	}
	if (bufferLen < IHPAPI_ETHER_MIN_LEN) 
	{
		errno = ERANGE;
		return (0);
	}
	if (inputEnetPHYInfo == (ihpapi_enetPHYCtl_t *)(0)) 
	{
		errno = EFAULT;
		return (0);
	}

/*
 * reject illegal input values;
 */
 
	if ( inputEnetPHYInfo->mcontrol > 0x01 ||
	     inputEnetPHYInfo->autonegotiate > 0x01 ||
             _anyset(inputEnetPHYInfo->advcaps,0xF0) ||
	     inputEnetPHYInfo->espeed > 0x01 ||
	     inputEnetPHYInfo->eduplex > 0x01 ||
	     inputEnetPHYInfo->eflowcontrol > 0x03 ) 
	{
		errno = EINVAL;
		return (0);
	}
	request->MCONTROL = inputEnetPHYInfo->mcontrol;
	request->AUTONEGOTIATE = inputEnetPHYInfo->autonegotiate;
	request->ADVCAPS = inputEnetPHYInfo->advcaps;
	request->ESPEED = inputEnetPHYInfo->espeed;
	request->EDUPLEX = inputEnetPHYInfo->eduplex;
	request->EFLOW_CONTROL =inputEnetPHYInfo->eflowcontrol;
	offset += 6;
	if (offset < IHPAPI_ETHER_MIN_LEN) 
	{
		memset (buffer + offset, 0, IHPAPI_ETHER_MIN_LEN - offset);
		offset = IHPAPI_ETHER_MIN_LEN;
	}

	return (offset);
}

#endif
 

