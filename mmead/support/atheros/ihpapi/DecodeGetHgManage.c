#ifndef DECODEGETHGMANAGE_SOURCE
#define DECODEGETHGMANAGE_SOURCE

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "../ihpapi/ihp.h"
#include "../tools/memory.h"
 
int ihp_DecodeGetHgManage (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	struct __packed vs_hg_manage_cnf 
	{
		struct header_vs header;  //10
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
		uint8_t variable_width;   //24
		/*uint8_t wanname_length;
		uint8_t wan_name[64];
		uint16_t vlan;
		uint8_t wlan_priority;
		uint8_t acs_user_name_length;
		uint8_t acs_user_name[64];
		uint8_t acs_password_length;
		uint8_t acs_password[64];
		uint8_t acs_url_length;
		uint8_t acs_url[64];
		uint8_t request_user_name_length;
		uint8_t request_user_name[64];
		uint8_t request_password_length;
		uint8_t request_password[64];
		uint8_t request_acs_url_length;
		uint8_t requset_acs_url[64];
		uint8_t ip_assigned_mode;
		uint8_t connection_protocol;
		uint8_t pppoe_user_name_length;
		uint8_t pppoe_user_name[64];
		uint8_t pppoe_password_length;
		uint8_t pppoe_password[64];
		uint8_t mng_ipv4_addr[4];
		uint8_t mng_ipv4_mask[4];
		uint8_t default_ipv4_gw[4];
		uint8_t mng_ipv6_addr[16];
		uint8_t mng_ipv6_mask;
		uint8_t default_ipv6_gw[16];
		uint8_t dns_ipv4[4];
		uint8_t dns_ipv4_copy[4];
		uint8_t dns_ipv6[16];
		uint8_t dns_ipv6_copy[16]; */
		uint8_t data[];
	}
	* confirm = (struct vs_hg_manage_cnf *)(buffer);
/*
#if INTELLON_SAFEMODE
 
	if (length < sizeof (struct vs_hg_manage_cnf)) 
	{
		errno = EFAULT;
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		errno = EFAULT;
		return (-1);
	}

#endif */	

	struct vs_hg_manage_data 
	{
		uint8_t wanname_length;
		uint8_t wan_name[255];
		uint8_t vlan[2];
		uint8_t vlan_priority;
		uint8_t acs_user_name_length;
		uint8_t acs_user_name[255];
		uint8_t acs_password_length;
		uint8_t acs_password[255];
		uint8_t acs_url_length;
		uint8_t acs_url[255];
		uint8_t request_user_name_length;
		uint8_t request_user_name[255];
		uint8_t request_password_length;
		uint8_t request_password[255];
		uint8_t request_acs_url_length;
		uint8_t requset_acs_url[255];
		uint8_t ip_assigned_mode;
		uint8_t connection_protocol;
		uint8_t pppoe_user_name_length;
		uint8_t pppoe_user_name[255];
		uint8_t pppoe_password_length;
		uint8_t pppoe_password[255];
		uint8_t mng_ipv4_addr[4];
		uint8_t mng_ipv4_mask[4];
		uint8_t default_ipv4_gw[4];
		uint8_t mng_ipv6_addr[16];
		uint8_t mng_ipv6_mask;
		uint8_t default_ipv6_gw[16];
		uint8_t dns_ipv4[4];
		uint8_t dns_ipv4_copy[4];
		uint8_t dns_ipv6[16];
		uint8_t dns_ipv6_copy[16]; 
	}manage_data;
	uint8_t index=0;
	//uint8_t i;
	
	manage_data.wanname_length = confirm->data[index];
	index = index + 1;
	memcpy(manage_data.wan_name, &confirm->data[index], manage_data.wanname_length);
	//printf("wan_name=%s\n",manage_data.wan_name);
	index = index + manage_data.wanname_length;
	memcpy(manage_data.vlan, &confirm->data[index],2);
	/*for(i=0;i<2;i++)
	{
		printf("vlan[%d]=%d\n",i,manage_data.vlan[i]);
	}*/
	index = index + 2;
	manage_data.vlan_priority = confirm->data[index];
	//printf("vlan_priority=%x\n",manage_data.vlan_priority);
	index = index + 1;
	manage_data.acs_user_name_length = confirm->data[index];
	index = index + 1;
	memcpy(manage_data.acs_user_name, &confirm->data[index],manage_data.acs_user_name_length);
	//printf("acs_user_name=%s\n",manage_data.acs_user_name);
	index = index + manage_data.acs_user_name_length;
	manage_data.acs_password_length = confirm->data[index];
	index = index + 1;
	memcpy(manage_data.acs_password, &confirm->data[index], manage_data.acs_password_length);
	//printf("acs_password=%s\n",manage_data.acs_password);
	index = index + manage_data.acs_password_length;
	manage_data.acs_url_length = confirm->data[index];
	index = index + 1;
	memcpy(manage_data.acs_url, &confirm->data[index], manage_data.acs_url_length);
	//printf("acs_url=%s\n",manage_data.acs_url);
	index = index + manage_data.acs_url_length;
	manage_data.request_user_name_length = confirm->data[index];
	index = index + 1;
	memcpy(manage_data.request_user_name, &confirm->data[index], manage_data.request_user_name_length);
	//printf("request_user_name=%s\n",manage_data.request_user_name);
	index = index + manage_data.request_user_name_length;
	manage_data.request_password_length = confirm->data[index];
	index = index + 1;
	memcpy(manage_data.request_password, &confirm->data[index], manage_data.request_password_length);
	//printf("request_password=%s\n",manage_data.request_password);
	index = index + manage_data.request_password_length;
	manage_data.request_acs_url_length = confirm->data[index]+4;
	index = index + 1;
	memcpy(manage_data.requset_acs_url, &confirm->data[index], manage_data.request_acs_url_length);
	//printf("request_acs_url=%s\n",manage_data.requset_acs_url);
	index = index + manage_data.request_acs_url_length;
	manage_data.ip_assigned_mode = confirm->data[index];
	//printf("ip_assigned_mode=%x\n",manage_data.ip_assigned_mode);
	index = index + 1;
	manage_data.connection_protocol = confirm->data[index];
	//printf("connection_protocol=%x\n",manage_data.connection_protocol);
	index = index + 1;
	manage_data.pppoe_user_name_length = confirm->data[index];
	index = index + 1;
	memcpy(manage_data.pppoe_user_name, &confirm->data[index], manage_data.pppoe_user_name_length);
	//printf("pppoe_user_name=%s\n",manage_data.pppoe_user_name);
	index = index + manage_data.pppoe_user_name_length;
	manage_data.pppoe_password_length = confirm->data[index];
	index = index + 1;
	memcpy(manage_data.pppoe_password, &confirm->data[index], manage_data.pppoe_password_length);
	//printf("pppoe_password=%s\n",manage_data.pppoe_password);
	index = index + manage_data.pppoe_password_length;
	memcpy(manage_data.mng_ipv4_addr, &confirm->data[index], 4);
	/*for(i=0;i<4;i++)
	{
		printf("mng_ipv4_addr[%d]=%d\n",i,manage_data.mng_ipv4_addr[i]);
	}*/
	index = index + 4;
	memcpy(manage_data.mng_ipv4_mask, &confirm->data[index], 4);
	/*for(i=0;i<4;i++)
	{
		printf("mng_ipv4_mask[%d]=%d\n",i,manage_data.mng_ipv4_mask[i]);
	}*/
	index = index + 4;
	memcpy(manage_data.default_ipv4_gw, &confirm->data[index], 4);
	/*for(i=0;i<4;i++)
	{
		printf("dafault_ipv4_gw[%d]=%d\n",i,manage_data.default_ipv4_gw[i]);
	}*/
	index = index + 4;
	memcpy(manage_data.mng_ipv6_addr, &confirm->data[index], 16);
	/*for(i=0;i<16;i++)
	{
		printf("mng_ipv6_addr[%d]=%x\n",i,manage_data.mng_ipv6_addr[i]);
	}*/
	index = index + 16;
	manage_data.mng_ipv6_mask = confirm->data[index];
	//printf("mng_ipv6_mask=%x\n",manage_data.mng_ipv6_mask);
	index = index + 1;
	memcpy(manage_data.default_ipv6_gw, &confirm->data[index], 16);
	/*for(i=0;i<16;i++)
	{
		printf("default_ipv6_gw[%d]=%x\n",i,manage_data.default_ipv6_gw[i]);
	}*/
	index = index + 16;
	memcpy(manage_data.dns_ipv4, &confirm->data[index], 4);
	/*for(i=0;i<4;i++)
	{
		printf("dns_ipv4[%d]=%d\n",i,manage_data.dns_ipv4[i]);
	}*/
	index = index + 4;
	memcpy(manage_data.dns_ipv4_copy, &confirm->data[index], 4);
	/*for(i=0;i<4;i++)
	{
		printf("dns_ipv4_copy[%d]=%d\n",i,manage_data.dns_ipv4_copy[i]);
	} */
	index = index + 4;
	memcpy(manage_data.dns_ipv6, &confirm->data[index], 16);
	/*for(i=0;i<16;i++)
	{
		printf("dns_ipv6[%d]=%x\n",i,manage_data.dns_ipv6[i]);
	} */
	index = index + 16;
	memcpy(manage_data.dns_ipv6_copy, &confirm->data[index], 16);
	/*for(i=0;i<16;i++)
	{
		printf("dns_ipv6_copy[%d]=%x\n",i,manage_data.dns_ipv6_copy[i]);
	} */
	
	
	result->validData = false;
	result->opStatus.status = confirm->MSTATUS;
	if (0 == result->opStatus.status) 
	{
		memcpy(result->data.hgmanageInfo.wan_name, manage_data.wan_name, 255);
		memcpy(result->data.hgmanageInfo.vlan, manage_data.vlan, 2);
		result->data.hgmanageInfo.priority = manage_data.vlan_priority;
		result->data.hgmanageInfo.ip_assigned_mode = manage_data.ip_assigned_mode;
		result->data.hgmanageInfo.connection_protocol = manage_data.connection_protocol;
		memcpy(result->data.hgmanageInfo.user_name, manage_data.pppoe_user_name, 255);
		memcpy(result->data.hgmanageInfo.password, manage_data.pppoe_password, 255);
		memcpy(result->data.hgmanageInfo.ipv4_addr, manage_data.mng_ipv4_addr, 4);
		memcpy(result->data.hgmanageInfo.ipv4_mask, manage_data.mng_ipv4_mask, 4);
		memcpy(result->data.hgmanageInfo.ipv4_gw, manage_data.default_ipv4_gw, 4);
		memcpy(result->data.hgmanageInfo.dns_ipv4, manage_data.dns_ipv4, 4);
		memcpy(result->data.hgmanageInfo.dns_ipv4_copy, manage_data.dns_ipv4_copy, 4);
			
		result->validData = true;
	}
	return (result->opStatus.status);
}

#endif

