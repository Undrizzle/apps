#ifndef SETHGSSIDSTATUS_SOURCE
#define SETHGSSIDSTATUS_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <public.h>

#include "ihpapi.h"
#include "ihp.h"
 
size_t ihpapi_SetHgSsidStatus(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], uint8_t ssid[4]) 
{	
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
		uint8_t ssid1_index;
		uint8_t ssid1_status;
		uint8_t ssid2_index;
		uint8_t ssid2_status;
		uint8_t ssid3_index;
		uint8_t ssid3_status;
		uint8_t ssid4_index;
		uint8_t ssid4_status;
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
	request->variable_length = 8;
	//printf("ssid1=%d\n",ssid[0]);
	//printf("ssid2=%d\n",ssid[1]);
	//printf("ssid3=%d\n",ssid[2]);
	//printf("ssid4=%d\n",ssid[3]);
	request->ssid1_index = 1;
	request->ssid1_status = ssid[0];
	request->ssid2_index = 2;
	request->ssid2_status = ssid[1];
	request->ssid3_index = 3;
	request->ssid3_status = ssid[2];
	request->ssid4_index = 4;
	request->ssid4_status = ssid[3];
	
	return IHPAPI_ETHER_MIN_LEN;
}

#endif

