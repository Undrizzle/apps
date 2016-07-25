#ifndef DECODEGETUSERHFID_SOURCE
#define DECODEGETUSERHFID_SOURCE

#include <stdint.h>
#include <string.h>

#include "../ihpapi/ihp.h"
 
int ihp_DecodeGetUserHFID(const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	struct __packed vs_get_user_hfid_cnf 
	{
		vs_get_property_cnf_header_t get_property_cnf_header;
		uint8_t USER_HFID[64];
	}
	* confirm = (struct vs_get_user_hfid_cnf *)(buffer);

#if INTELLON_SAFEMODE
 
	if (length < sizeof (struct vs_get_user_hfid_cnf)) 
	{
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		return (-1);
	}

#endif

	if(0x02 != intohl(confirm->get_property_cnf_header.COOKIE))
	{
		result->validData = false;
		result->opStatus.status = 0x01;
		return (-1);
	}
	else
	{
		result->validData = true;
		result->opStatus.status = intohl(confirm->get_property_cnf_header.MSTATUS);	
		memcpy(result->data.UserHFIDInfo.USER_HFID , confirm->USER_HFID,64);
		return (0);
	}
}

#endif