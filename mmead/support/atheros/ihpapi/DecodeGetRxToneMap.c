/*====================================================================*
 *
 *   int ihp_DecodeGetRxToneMapInfo (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
 *
 *   ihp.h
 *
 *   decode buffer as a VS_RX_TONE_MAP_CHAR Confirm MME; update the structure
 *   named by the caller; 
 *
 *   return 0 on decode success or -1 on decode error;
 *
 *   See the INT6000 Firmware Technical Reference Manual
 *   for more information;
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

#ifndef DECODEGETRXTONEMAPINFO_SOURCE
#define DECODEGETRXTONEMAPINFO_SOURCE

#include <stdint.h>
#include <string.h>

#include "../ihpapi/ihp.h"
 
int ihp_DecodeGetRxToneMapInfo (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	struct __packed vs_rx_tone_map_char_cnf 
	{
		struct header_vs header;
		uint8_t MSTATUS;
		uint8_t TMSLOT;
		uint8_t NUMTMS;
		uint16_t TMNUMACTCARRIERS;
		uint8_t MOD_CARRIER [MOD_CARRIER_MAX_TUPLE_NUM];
		uint8_t GIL;
		uint8_t AVG_AGC_GAIN;
	}
	* confirm = (struct vs_rx_tone_map_char_cnf *)(buffer);

#if INTELLON_SAFEMODE
 
	if (length < sizeof (struct vs_rx_tone_map_char_cnf)) 
	{
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		return (-1);
	}

#endif

	result->validData = true;
	result->opStatus.status = confirm->MSTATUS;
	result->data.rxToneMap.tmslot = confirm->TMSLOT;
	result->data.rxToneMap.numtms = confirm->NUMTMS;
	result->data.rxToneMap.tmnumactcarriers = intohs(confirm->TMNUMACTCARRIERS);
	result->data.rxToneMap.gil = confirm->GIL;
	result->data.rxToneMap.avg_agc_gain = confirm->AVG_AGC_GAIN;
	memcpy (result->data.rxToneMap.mod_carrier, confirm->MOD_CARRIER, MOD_CARRIER_MAX_TUPLE_NUM);
	return (0);
}

int ihp_DecodeGet74RxToneMapInfo (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	struct __packed vs_rx_tone_map_char_cnf 
	{
		struct header_v1_vs header;
		uint8_t MSTATUS;
		uint8_t RESERVED1;
		uint16_t LENGTH;
		uint8_t SUBVER;
		uint8_t RESERVED2;
		uint8_t MACADDRESS [IHPAPI_ETHER_ADDR_LEN];
		uint8_t TMSLOT;
		uint8_t RESERVED3;
		uint8_t NUMTMS;
		uint8_t RESERVED4;
		uint16_t TMNUMACTCARRIERS;
		uint32_t RESERVED;	
		uint8_t GIL;
		uint8_t RESERVED5;
		uint8_t AVG_AGC_GAIN;
		uint8_t RESERVED6;
		uint8_t MOD_CARRIER [MOD_CARRIER_MAX_TUPLE_NUM];
	}
	* confirm = (struct vs_rx_tone_map_char_cnf *)(buffer);

#if INTELLON_SAFEMODE
 
	if (length < sizeof (struct vs_rx_tone_map_char_cnf)) 
	{
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		return (-1);
	}

#endif

	result->validData = true;
	result->opStatus.status = confirm->MSTATUS;
	result->data.rxToneMap.tmslot = confirm->TMSLOT;
	result->data.rxToneMap.numtms = confirm->NUMTMS;
	result->data.rxToneMap.tmnumactcarriers = intohs(confirm->TMNUMACTCARRIERS);
	result->data.rxToneMap.gil = confirm->GIL;
	result->data.rxToneMap.avg_agc_gain = confirm->AVG_AGC_GAIN;
	memcpy (result->data.rxToneMap.mod_carrier, confirm->MOD_CARRIER, MOD_CARRIER_MAX_TUPLE_NUM);
	return (0);
}

#endif
 

