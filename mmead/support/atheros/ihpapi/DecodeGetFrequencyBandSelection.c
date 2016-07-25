#ifndef DECODEGETFREQUENCYBANDSELECTION_SOURCE
#define DECODEGETFREQUENCYBANDSELECTION_SOURCE

#include <stdint.h>
#include <string.h>

#include "../ihpapi/ihp.h"
 
int ihp_DecodeGetFrequencyBandSelection (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	struct __packed vs_get_frequency_band_selection_cnf 
	{
		vs_get_property_cnf_header_t get_property_cnf_header;
		uint8_t FBSTATUS;
		uint16_t START_BAND;
		uint16_t STOP_BAND;		
	}
	* confirm = (struct vs_get_frequency_band_selection_cnf *)(buffer);

#if INTELLON_SAFEMODE
 
	if (length < sizeof (struct vs_get_frequency_band_selection_cnf)) 
	{
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		return (-1);
	}

#endif

	if(0x00 != intohl(confirm->get_property_cnf_header.COOKIE))
	{
		result->validData = false;
		result->opStatus.status = 0x01;
		return (-1);
	}
	else
	{
		result->validData = true;
		result->opStatus.status = intohl(confirm->get_property_cnf_header.MSTATUS);			
		result->data.FrequencyBandSelectionInfo.FBSTATUS = confirm->FBSTATUS;
		result->data.FrequencyBandSelectionInfo.START_BAND = intohs(confirm->START_BAND);
		result->data.FrequencyBandSelectionInfo.STOP_BAND = intohs(confirm->STOP_BAND);		
		return (0);
	}
}

#endif
 

