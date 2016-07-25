#ifndef GETFREQUENCYBANDSELECTION_SOURCE
#define GETFREQUENCYBANDSELECTION_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"
 
size_t ihpapi_GetFrequencyBandSelection (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []) 

{
	vs_get_property_req_t* request = (vs_get_property_req_t *)(buffer);
	size_t offset = 0;
	offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, da, sa);
	offset += EncodeIntellonHeader (buffer + offset, bufferLen - offset, (VS_GET_PROPERTY | MMTYPE_REQ));
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

	/* Unique ID for the request */
	request->COOKIE = ihtonl(0);
	/* Output format(0x00=binary) */
	request->OUTPUT_FORMAT = 0x00;
	/* Property format(0x00=string, 0x01=4-byte ID) */
	request->PROP_FORMAT = 0x01;
	/* Reserved field */
	request->RESERVED[0] = 0x00;
	request->RESERVED[1] = 0x00;
	/* Property version */
	request->PROP_VERSION = ihtonl(0);
	/* Property string length, it should by 4 bytes for "ID" property format */
	request->PROP_STR_LENGTH = ihtonl(4);
	/* Proterty string, for "ID" property format, the next 4 bytes are the ID in little endian format */
	request->PRO_STR = ihtonl(109);
	
	return (IHPAPI_ETHER_MIN_LEN);
}

#endif
 

