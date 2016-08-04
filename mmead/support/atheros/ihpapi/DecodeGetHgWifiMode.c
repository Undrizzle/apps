#ifndef DECODEGETHGWIFIMODE_SOURCE
#define DECODEGETHGWIFIMODE_SOURCE

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "../ihpapi/ihp.h"
#include "../tools/memory.h"
 
int ihp_DecodeGetHgWifiMode(const uint8_t buffer [], size_t length, ihpapi_result_t * result) 
{
	struct __packed vs_get_hg_wifi_mode_cnf 
	{
		struct header_vs header;  
		uint8_t MSTATUS;
		uint8_t action;
		uint8_t case_MainType;
		uint16_t case_SubType;
		uint8_t case_length;
		uint16_t RSVD;
		uint8_t case_val1;
		uint8_t case_val2;
		uint8_t extend_MainType;
		uint16_t extend_SubType;
		uint8_t variable_width;  
		uint8_t mode;	
	}
	* confirm = (struct vs_get_hg_wifi_mode_cnf *)(buffer);

	result->validData = false;
	result->opStatus.status = confirm->MSTATUS;
	if (0 == result->opStatus.status) 
	{
		result->data.hgWifiMode = confirm->mode;
		result->validData = true;
	}
	return (result->opStatus.status);
	
}

#endif

