#ifndef GETHGSSIDSTATUS_SOURCE
#define GETHGSSIDSTATUS_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"
 
size_t ihpapi_GetHgSsidStatus(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer []) 

{
	struct __packed vs_get_hg_ssid_status_req 
	{
		struct header_vs header;
		uint8_t action;
		uint8_t case_MainType;
		uint16_t case_SubType;
		uint8_t case_length;
		uint16_t RSVD;
		uint8_t case_val1;
		uint8_t case_val2;
		uint8_t extend_MainType;
		uint16_t extend_SubType;
		uint8_t extend_MainType1;
		uint16_t extend_SubType1;
		uint8_t extend_MainType2;
		uint16_t extend_SubType2;
	}
	* request = (struct vs_get_hg_ssid_status_req *)(buffer);

	size_t offset = 0;
	offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, da, sa);             //ODA,OSA,MTYPE
	offset += EncodeIntellonHeader (buffer + offset, bufferLen - offset, (VS_HOME_GATEWAY_OPERATION | MMTYPE_REQ));   //MMV,MMTYPE,OUI
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

	request->action = 0x02;
	request->case_MainType = 0xf7;
	request->case_SubType = ihtons(0x0003);
	request->case_length = 0x04;
	request->RSVD = ihtons(0);
	request->case_val1 = 0;
	request->case_val2 = 0;
	request->extend_MainType = 0xf8;
	request->extend_SubType = ihtons(0x3013);
	request->extend_MainType1 = 0xf8;
	request->extend_SubType1 = ihtons(0x3003);
	request->extend_MainType2 = 0xf8;
	request->extend_SubType2 = ihtons(0x3803);
	
	return (IHPAPI_ETHER_MIN_LEN);
}

#endif

