#ifndef DECODEGETHGWANSTATUS_SOURCE
#define DECODEGETHGWANSTATUS_SOURCE

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "../ihpapi/ihp.h"
#include "../tools/memory.h"
 
int ihp_DecodeGetHgWanStatus(const uint8_t buffer [], size_t length, ihpapi_result_t * result) 
{
	struct __packed vs_get_hg_wan_status_cnf 
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
		uint8_t data[];	
	}
	* confirm = (struct vs_get_hg_wan_status_cnf *)(buffer);

	uint8_t wan_name1_length;
	uint8_t wan_name2_length;
	uint8_t wan_name3_length;
	uint8_t wan_name4_length;

	result->validData = false;
	result->opStatus.status = confirm->MSTATUS;
	if (0 == result->opStatus.status) 
	{
		wan_name1_length = confirm->data[0];
		memcpy(result->data.hgWanInfo.wan_name1, &confirm->data[1], wan_name1_length);
		result->data.hgWanInfo.status1 = confirm->data[1+wan_name1_length];
		wan_name2_length = confirm->data[1+wan_name1_length+1];
		memcpy(result->data.hgWanInfo.wan_name2, &confirm->data[1+wan_name1_length+1+1], wan_name2_length);
		result->data.hgWanInfo.status2 = confirm->data[1+wan_name1_length+1+1+wan_name2_length];
		wan_name3_length = confirm->data[1+wan_name1_length+1+1+wan_name2_length+1];
		memcpy(result->data.hgWanInfo.wan_name3, &confirm->data[1+wan_name1_length+1+1+wan_name2_length+1+1], wan_name3_length);
		result->data.hgWanInfo.status3 = confirm->data[1+wan_name1_length+1+1+wan_name2_length+1+1+wan_name3_length];
		wan_name4_length = confirm->data[1+wan_name1_length+1+1+wan_name2_length+1+1+wan_name3_length+1];
		memcpy(result->data.hgWanInfo.wan_name4, &confirm->data[1+wan_name1_length+1+1+wan_name2_length+1+1+wan_name3_length+1+1], wan_name4_length);
		result->data.hgWanInfo.status4 = confirm->data[1+wan_name1_length+1+1+wan_name2_length+1+1+wan_name3_length+1+1+wan_name4_length];
		result->validData = true;
	}
	return (result->opStatus.status);
	
}

#endif



