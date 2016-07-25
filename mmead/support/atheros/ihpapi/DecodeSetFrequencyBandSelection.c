#ifndef DECODESETFREQUENCYBANDSELECTION_SOURCE
#define DECODESETFREQUENCYBANDSELECTION_SOURCE

#include <stdint.h>
#include <string.h>

#include "../ihpapi/ihp.h"
 
int ihp_DecodeSetFrequencyBandSelection(const uint8_t buffer [ ], size_t length, ihpapi_result_t * result)

{
	vs_set_property_cnf_t* confirm = ( vs_set_property_cnf_t *)(buffer);

#if INTELLON_SAFEMODE
 
	if (length < sizeof (vs_set_property_cnf_t)) 
	{
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		return (-1);
	}

#endif

	if( 0x00 != intohl(confirm->COOKIE) )
	{
		result->validData = false;
		result->opStatus.status = 0x01;
		return (-1);
	}
	else
	{
		result->validData = true;
		result->opStatus.status = intohl(confirm->MSTATUS);		
		return (0);
	}
}

#endif
 

