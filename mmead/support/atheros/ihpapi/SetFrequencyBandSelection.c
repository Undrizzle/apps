#ifndef SETFREQUENCYBANDSELECTION_SOURCE
#define SETFREQUENCYBANDSELECTION_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"
 
size_t ihpapi_SetFrequencyBandSelection
(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], ihpapi_getFrequencyBandSelection_t *FrequencyBandSelectionInfo) 
{
	struct __packed vs_set_frequency_band_selection_req 
	{
		vs_set_property_req_header_t set_property_cnf_header;
		uint8_t FBSTATUS;
		uint16_t START_BAND;
		uint16_t STOP_BAND;	
	}
	* request = (struct vs_set_frequency_band_selection_req *)(buffer);
	size_t offset = 0;
	offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, da, sa);
	offset += EncodeIntellonHeader (buffer + offset, bufferLen - offset, (VS_SET_PROPERTY | MMTYPE_REQ));
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
	request->set_property_cnf_header.COOKIE = ihtonl(0);
	/* 0x02: DoNotApply_Persist_DoNotReset; 0x06: DoNotApply_Persist_Reset */
	request->set_property_cnf_header.OPTION = 0x06;
	/* Reserved field */
	request->set_property_cnf_header.RESERVED[0] = 0x00;
	request->set_property_cnf_header.RESERVED[1] = 0x00;
	request->set_property_cnf_header.RESERVED[2] = 0x00;
	/* Property version */
	request->set_property_cnf_header.PROP_VERSION = ihtonl(0);
	/* Property ID */
	request->set_property_cnf_header.PROP_ID = ihtonl(109);
	/* Property Data Length */
	request->set_property_cnf_header.PROP_DATA_LENGTH = ihtonl(5);
	/* Property data */
	request->FBSTATUS = FrequencyBandSelectionInfo->FBSTATUS;
	request->START_BAND = ihtons(FrequencyBandSelectionInfo->START_BAND);
	request->STOP_BAND = ihtons(FrequencyBandSelectionInfo->STOP_BAND);	
	
	return (IHPAPI_ETHER_MIN_LEN);
}

#endif
 

