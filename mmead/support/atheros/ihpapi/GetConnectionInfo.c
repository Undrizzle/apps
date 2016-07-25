/*====================================================================*
 *
 *   size_t ihpapi_GetConnectionInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_connectCtl_t * inputConnectInfo);
 *
 *   ihpapi.h
 *
 *   Encode buffer as an Intellon VS_LNK_STATS Management Message and
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
 *	Charles Maier, charles.maier@intellon.com
 *
 *--------------------------------------------------------------------*/

#ifndef GETCONNECTIONINFO_SOURCE
#define GETCONNECTIONINFO_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"
 
size_t ihpapi_GetConnectionInfo (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_connectCtl_t * inputConnectInfo) 

{
	struct __packed vs_lnk_stats_req 
	{
		struct header_vs header;
		uint8_t MCONTROL;
		uint8_t DIRECTION;
		uint8_t LID;
		uint8_t MACADDRESS [IHPAPI_ETHER_ADDR_LEN];
	}
	* request = (struct vs_lnk_stats_req *)(buffer);
	size_t offset = 0;
	offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, da, sa);
	offset += EncodeIntellonHeader (buffer + offset, bufferLen - offset, (VS_LNK_STATS | MMTYPE_REQ));
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
	if (inputConnectInfo == (ihpapi_connectCtl_t *)(0)) 
	{
		errno = EFAULT;
		return (0);
	}

/*
 * reject illegal MCONTROL values;
 */
 
	if (inputConnectInfo->mcontrol > 0x01) 
	{
		errno = EINVAL;
		return (0);
	}
	request->MCONTROL = inputConnectInfo->mcontrol;

/*
 * reject illegal DIRECTION values;
 */
 
	if (inputConnectInfo->direction > 0x02) 
	{
		errno = EINVAL;
		return (0);
	}
	request->DIRECTION = inputConnectInfo->direction;

/*
 * reject illegal LID values;
 */
 
	switch (inputConnectInfo->lid) 
	{
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0xF8:
	case 0xFC:
		break;
	default:
		errno = EINVAL;
		return (0);
	}
	request->LID = inputConnectInfo->lid;
	memcpy (request->MACADDRESS, inputConnectInfo->macaddress, sizeof (request->MACADDRESS));
	return (IHPAPI_ETHER_MIN_LEN);
}

#endif
 

