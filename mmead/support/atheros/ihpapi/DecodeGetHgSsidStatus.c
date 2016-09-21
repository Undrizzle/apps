#ifndef DECODEGETHGSSIDSTATUS_SOURCE
#define DECODEGETHGSSIDSTATUS_SOURCE

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "../ihpapi/ihp.h"
#include "../tools/memory.h"
 
int ihp_DecodeGetHgSsidStatus (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	struct __packed vs_get_hg_ssid_status_cnf 
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
		uint8_t ssid_num;
		uint8_t ssid_status;
		uint8_t extend_MainType1;
		uint16_t extend_SubType1;
		uint8_t variable_width1;
		uint8_t ssid_name_data[];
	}
	* confirm = (struct vs_get_hg_ssid_status_cnf *)(buffer);

	uint8_t ssid_name1_length;
	uint8_t ssid_name2_length;
	uint8_t ssid_name3_length;
	uint8_t ssid_name4_length;
	uint8_t ssid_pwd1_length;
	uint8_t ssid_pwd2_length;
	uint8_t ssid_pwd3_length;
	uint8_t ssid_pwd4_length;

	result->validData = false;
	result->opStatus.status = confirm->MSTATUS;
	if (0 == result->opStatus.status) 
	{
		result->data.hgSsidInfo.ssid_status = confirm->ssid_status;
		ssid_name1_length = confirm->ssid_name_data[1];
		memcpy(result->data.hgSsidInfo.ssid_name1, &confirm->ssid_name_data[1+1],ssid_name1_length);
		//printf("ssid1=%s\n",result->data.hgSsidInfo.ssid_name1);
		ssid_name2_length = confirm->ssid_name_data[1+ssid_name1_length+1+1];
		memcpy(result->data.hgSsidInfo.ssid_name2, &confirm->ssid_name_data[1+ssid_name1_length+1+1+1], ssid_name2_length);
		//printf("ssid2=%s\n",result->data.hgSsidInfo.ssid_name2);
		ssid_name3_length = confirm->ssid_name_data[1+ssid_name1_length+1+1+ssid_name2_length+1+1];
		memcpy(result->data.hgSsidInfo.ssid_name3, &confirm->ssid_name_data[1+ssid_name1_length+1+1+ssid_name2_length+1+1+1], ssid_name3_length);
		//printf("ssid3=%s\n",result->data.hgSsidInfo.ssid_name3);
		ssid_name4_length = confirm->ssid_name_data[1+ssid_name1_length+1+ssid_name2_length+1+ssid_name3_length+1+1+1+1];
		memcpy(result->data.hgSsidInfo.ssid_name4, &confirm->ssid_name_data[1+1+ssid_name1_length+1+1+ssid_name2_length+1+1+ssid_name3_length+1+1], ssid_name4_length);
		//printf("ssid4=%s\n",result->data.hgSsidInfo.ssid_name4);
		ssid_pwd1_length = confirm->ssid_name_data[1+ssid_name1_length+1+ssid_name2_length+1+ssid_name3_length+1+1+1+1+ssid_name4_length+6];
		memcpy(result->data.hgSsidInfo.ssid_pwd1, &confirm->ssid_name_data[1+ssid_name1_length+1+ssid_name2_length+1+ssid_name3_length+1+1+1+1+ssid_name4_length+6+1], ssid_pwd1_length);
		//printf("ssid1 pwd=%s\n",result->data.hgSsidInfo.ssid_pwd1);
		ssid_pwd2_length = confirm->ssid_name_data[1+ssid_name1_length+1+ssid_name2_length+1+ssid_name3_length+1+1+1+1+ssid_name4_length+6+ssid_pwd1_length+2];
		memcpy(result->data.hgSsidInfo.ssid_pwd2, &confirm->ssid_name_data[1+ssid_name1_length+1+ssid_name2_length+1+ssid_name3_length+1+1+1+1+ssid_name4_length+6+ssid_pwd1_length+2+1], ssid_pwd2_length);
		//printf("ssid2 pwd=%s\n",result->data.hgSsidInfo.ssid_pwd2);
		ssid_pwd3_length = confirm->ssid_name_data[1+ssid_name1_length+1+ssid_name2_length+1+ssid_name3_length+1+1+1+1+ssid_name4_length+6+ssid_pwd1_length+2+ssid_pwd2_length+2];
		memcpy(result->data.hgSsidInfo.ssid_pwd3, &confirm->ssid_name_data[1+ssid_name1_length+1+ssid_name2_length+1+ssid_name3_length+1+1+1+1+ssid_name4_length+6+ssid_pwd1_length+2+ssid_pwd2_length+2+1], ssid_pwd3_length); 
		//printf("ssid3 pwd=%s\n",result->data.hgSsidInfo.ssid_pwd3);
		ssid_pwd4_length = confirm->ssid_name_data[1+ssid_name1_length+1+ssid_name2_length+1+ssid_name3_length+1+1+1+1+ssid_name4_length+6+ssid_pwd1_length+2+ssid_pwd2_length+2+ssid_pwd3_length+2];
		memcpy(result->data.hgSsidInfo.ssid_pwd4, &confirm->ssid_name_data[1+ssid_name1_length+1+ssid_name2_length+1+ssid_name3_length+1+1+1+1+ssid_name4_length+6+ssid_pwd1_length+2+ssid_pwd2_length+2+ssid_pwd3_length+2+1], ssid_pwd4_length);
		//printf("ssid4 pwd=%s\n",result->data.hgSsidInfo.ssid_pwd4);
		
		result->validData = true;
	}
	return (result->opStatus.status);
	
}

#endif


