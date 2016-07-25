#ifndef DECODEGETTXGAIN_SOURCE
#define DECODEGETTXGAIN_SOURCE

#include <stdint.h>
#include <string.h>

#include "../ihpapi/ihp.h"
 
int ihp_DecodeGetTxGain(const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	struct __packed vs_get_tx_gain_cnf 
	{
		vs_get_property_cnf_header_t get_property_cnf_header;
		uint8_t TX_GAIN;
	}
	* confirm = (struct vs_get_tx_gain_cnf *)(buffer);

#if INTELLON_SAFEMODE
 
	if (length < sizeof (struct vs_get_tx_gain_cnf)) 
	{
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		return (-1);
	}

#endif

	if(0x01 != intohl(confirm->get_property_cnf_header.COOKIE))
	{
		result->validData = false;
		result->opStatus.status = 0x01;
		return (-1);
	}
	else
	{
		result->validData = true;
		result->opStatus.status = intohl(confirm->get_property_cnf_header.MSTATUS);	
		result->data.txGainInfo.TX_GAIN = confirm->TX_GAIN;
		return (0);
	}
}

#endif
 

