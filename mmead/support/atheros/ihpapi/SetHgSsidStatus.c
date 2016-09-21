#ifndef SETHGSSIDSTATUS_SOURCE
#define SETHGSSIDSTATUS_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <public.h>

#include "ihpapi.h"
#include "ihp.h"
 
size_t ihpapi_SetHgSsidStatus(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], T_szSetHgSsid ssid) 
{	
	uint8_t ssid_name_length_v = strlen(ssid.ssid_name) + 1;
	uint8_t ssid_pwd_length_v = strlen(ssid.ssid_pwd) + 1;
	
	struct __packed vs_set_hg_ssid_status_req
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
		uint8_t variable_length;
		uint8_t ssid_index;
		uint8_t ssid_status;
		uint8_t extend1_MainType;
		uint16_t extend1_SubType;
		uint8_t variable1_length;
		uint8_t ssid1_index;
		uint8_t ssid_name_length;
		uint8_t ssid_name[ssid_name_length_v];
		uint8_t extend2_MainType;
		uint16_t extend2_SubType;
		uint8_t variable2_length;
		uint8_t ssid2_index;
		uint8_t ssid_pwd_length;
		uint8_t ssid_pwd[ssid_pwd_length_v];
	}
	* request = (struct vs_set_hg_ssid_status_req *)(buffer);

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

	request->action = 0x01;
	request->case_MainType = 0xf7;
	request->case_SubType = ihtons(0x0003);
	request->case_length = 0x04;
	request->RSVD = ihtons(0);
	request->case_val1 = 0;
	request->case_val2 = 0;
	request->extend_MainType = 0xf8;
	request->extend_SubType = ihtons(0x3013);
	request->variable_length = 2;
	request->ssid_index = ssid.ssid_index;
	request->ssid_status = ssid.ssid_status;
	request->extend1_MainType = 0xf8;
	request->extend1_SubType = ihtons(0x3003);
	request->variable1_length = 1 + 1 + ssid_name_length_v;
	request->ssid1_index = ssid.ssid_index;
	request->ssid_name_length = ssid_name_length_v;
	memcpy(request->ssid_name, &ssid.ssid_name, ssid_name_length_v);
	request->extend2_MainType = 0xf8;
	request->extend2_SubType = ihtons(0x3803);
	request->variable2_length = 1 + 1 + ssid_pwd_length_v;
	request->ssid2_index = ssid.ssid_index;
	request->ssid_pwd_length = ssid_pwd_length_v;
	memcpy(request->ssid_pwd, &ssid.ssid_pwd, ssid_pwd_length_v);

	return (IHPAPI_ETHER_MAX_LEN/2);
}

#endif

