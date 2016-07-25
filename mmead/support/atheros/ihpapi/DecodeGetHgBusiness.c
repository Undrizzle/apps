#ifndef DECODEGETHGBUSINESS_SOURCE
#define DECODEGETHGBUSINESS_SOURCE

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "../ihpapi/ihp.h"
#include "../tools/memory.h"
 
int ihp_DecodeGetHgBusiness (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	struct __packed vs_hg_business_cnf 
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
	* confirm = (struct vs_hg_business_cnf *)(buffer);
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

	struct vs_hg_business_data 
	{
		uint8_t wanname_length;
		uint8_t wan_name[255];
		uint8_t vlan[2];
		uint8_t vlan_priority;
		uint8_t connection_mode;
		uint8_t ip_assigned_mode;
		uint8_t service_type;
		uint8_t connection_protocol;
		uint8_t bind_interface[4];
		uint8_t pppoe_user_name_length;
		uint8_t pppoe_user_name[255];
		uint8_t pppoe_password_length;
		uint8_t pppoe_password[255];
		uint8_t serve_ipv4_addr[4];
		uint8_t serve_ipv4_mask[4];
		uint8_t default_ipv4_gw[4];
		uint8_t serve_ipv6_addr[16];
		uint8_t serve_ipv6_mask;
		uint8_t default_ipv6_gw[16];
		uint8_t dns_ipv4[4];
		uint8_t dns_ipv4_copy[4];
		uint8_t dns_ipv6[16];
		uint8_t dns_ipv6_copy[16];
	}business_data1, business_data2;
	uint8_t index=0;
	uint8_t data[512];
	//uint8_t i;

	memcpy(data, confirm->data, 127); 
	memcpy(data+127, &confirm->data[131], 127);
	memcpy(data+127+127, &confirm->data[262], 127);
	
	business_data1.wanname_length = data[index];
	index = index + 1;
	memcpy(business_data1.wan_name, &data[index], business_data1.wanname_length);
	//printf("wan_name=%s\n",business_data1.wan_name);
	index = index + business_data1.wanname_length;
	memcpy(business_data1.vlan, &data[index],2);
	/*for(i=0;i<2;i++)
	{
		printf("vlan[%d]=%d\n",i,business_data1.vlan[i]);
	}*/
	index = index + 2;
	business_data1.vlan_priority = data[index];
	//printf("vlan_priority=%d\n",business_data1.vlan_priority);
	index = index + 1;
	business_data1.connection_mode = data[index];
	//printf("connection_mode=%d\n",business_data1.connection_mode);
	index = index + 1;
	business_data1.ip_assigned_mode = data[index];
	//printf("ip_assigned_mode = %d\n",business_data1.ip_assigned_mode);
	index = index + 1;
	business_data1.service_type = data[index];
	//printf("service_type = %d\n", business_data1.service_type);
	index = index + 1;
	business_data1.connection_protocol = data[index];
	//printf("connection_protocol = %d\n",business_data1.connection_protocol);
	index = index + 1;
	memcpy(business_data1.bind_interface, &data[index], 4);
	/*for(i=0;i<4;i++) {
		printf("bind_interface[%d] = %d\n",i,business_data1.bind_interface[i]);
	}*/
	index = index + 4;
	business_data1.pppoe_user_name_length = data[index];
	index = index + 1;
	memcpy(business_data1.pppoe_user_name, &data[index], business_data1.pppoe_user_name_length);
	//printf("pppoe_user_name=%s\n",business_data1.pppoe_user_name);
	index = index + business_data1.pppoe_user_name_length;
	business_data1.pppoe_password_length = data[index];
	index = index + 1;
	memcpy(business_data1.pppoe_password, &data[index], business_data1.pppoe_password_length);
	//printf("pppoe_password=%s\n",business_data1.pppoe_password);
	index = index + business_data1.pppoe_password_length;
	memcpy(business_data1.serve_ipv4_addr, &data[index], 4);
	/*for(i=0;i<4;i++) {
		printf("serve_ipv4_addr[%d]=%d\n",i,business_data1.serve_ipv4_addr[i]);
	}*/
	index = index + 4;
	memcpy(business_data1.serve_ipv4_mask, &data[index], 4);
	/*for(i=0;i<4;i++) {
		printf("serve_ipv4_mask[%d]=%d\n",i,business_data1.serve_ipv4_mask[i]);
	}*/
	index = index + 4;
	memcpy(business_data1.default_ipv4_gw, &data[index], 4);
	/*for(i=0;i<4;i++) {
		printf("default_ipv4_gw[%d]=%d\n",i,business_data1.default_ipv4_gw[i]);
	}*/
	index = index + 4;
	memcpy(business_data1.serve_ipv6_addr, &data[index], 16);
	/*for(i=0;i<16;i++) {
		printf("serve_ipv6_addr[%d] = %d\n",i,business_data1.serve_ipv6_addr[i]);
	}*/
	index = index + 16;
	business_data1.serve_ipv6_mask = data[index];
	//printf("serve_ipv6_mask=%d\n",business_data1.serve_ipv6_mask);
	index = index + 1;
	memcpy(business_data1.default_ipv6_gw, &data[index], 16);
	/*for(i=0;i<16;i++){
		printf("default_ipv6_gw[%d]=%d\n",i,business_data1.default_ipv6_gw[i]);
	}*/
	index = index + 16;
	memcpy(business_data1.dns_ipv4, &data[index], 4);
	/*for(i=0;i<4;i++) {
		printf("dns_ipv4[%d]=%d\n",i,business_data1.dns_ipv4[i]);
	}*/
	index = index + 4;
	memcpy(business_data1.dns_ipv4_copy, &data[index], 4);
	/*for(i=0;i<4;i++) {
		printf("dns_ipv4_copy[%d]=%d\n",i,business_data1.dns_ipv4_copy[i]);
	}*/
	index = index + 4;
	memcpy(business_data1.dns_ipv6, &data[index], 16);
	/*for(i=0; i<16; i++) {
		printf("dns_ipv6[%d]=%d\n",i,business_data1.dns_ipv6[i]);
	}*/
	index = index + 16;
	memcpy(business_data1.dns_ipv6_copy, &data[index], 16);
	/*for(i=0; i<16; i++) {
		printf("dns_ipv6_copy[%d]=%d\n",i,business_data1.dns_ipv6_copy[i]);
	}*/
	index = index + 16;

	business_data2.wanname_length = data[index];
	index = index + 1;
	
	memcpy(business_data2.wan_name, &data[index], business_data2.wanname_length);
	//printf("wan_name=%s\n",business_data2.wan_name);
	index = index + business_data2.wanname_length;
	memcpy(business_data2.vlan, &data[index],2);
	/*for(i=0;i<2;i++)
	{
		printf("vlan[%d]=%d\n",i,business_data2.vlan[i]);
	}*/
	index = index + 2;
	business_data2.vlan_priority = data[index];
	//printf("vlan_priority=%d\n",business_data2.vlan_priority);
	index = index + 1;
	business_data2.connection_mode = data[index];
	//printf("connection_mode=%d\n",business_data2.connection_mode);
	index = index + 1;
	business_data2.ip_assigned_mode = data[index];
	//printf("ip_assigned_mode = %d\n",business_data2.ip_assigned_mode);
	index = index + 1;
	business_data2.service_type = data[index];
	//printf("service_type = %d\n", business_data2.service_type);
	index = index + 1;
	business_data2.connection_protocol = data[index];
	//printf("connection_protocol = %d\n",business_data2.connection_protocol);
	index = index + 1;
	memcpy(business_data2.bind_interface, &data[index], 4);
	/*for(i=0;i<4;i++) {
		printf("bind_interface[%d] = %d\n",i,business_data2.bind_interface[i]);
	}*/
	index = index + 4;
	business_data2.pppoe_user_name_length = data[index];
	index = index + 1;
	memcpy(business_data2.pppoe_user_name, &data[index], business_data2.pppoe_user_name_length);
	//printf("pppoe_user_name=%s\n",business_data2.pppoe_user_name);
	index = index + business_data2.pppoe_user_name_length;
	business_data2.pppoe_password_length = data[index];
	index = index + 1;
	memcpy(business_data2.pppoe_password, &data[index], business_data2.pppoe_password_length);
	//printf("pppoe_password=%s\n",business_data2.pppoe_password);
	index = index + business_data2.pppoe_password_length;
	memcpy(business_data2.serve_ipv4_addr, &data[index], 4);
	/*for(i=0;i<4;i++) {
		printf("serve_ipv4_addr[%d]=%d\n",i,business_data2.serve_ipv4_addr[i]);
	}*/
	index = index + 4;
	memcpy(business_data2.serve_ipv4_mask, &data[index], 4);
/*	for(i=0;i<4;i++) {
		printf("serve_ipv4_mask[%d]=%d\n",i,business_data2.serve_ipv4_mask[i]);
	}*/
	index = index + 4;
	memcpy(business_data2.default_ipv4_gw, &data[index], 4);
	/*for(i=0;i<4;i++) {
		printf("default_ipv4_gw[%d]=%d\n",i,business_data2.default_ipv4_gw[i]);
	}*/
	index = index + 4;
	memcpy(business_data2.serve_ipv6_addr, &data[index], 16);
/*	for(i=0;i<16;i++) {
		printf("serve_ipv6_addr[%d] = %d\n",i,business_data2.serve_ipv6_addr[i]);
	}*/
	index = index + 16;
	business_data2.serve_ipv6_mask = data[index];
	//printf("serve_ipv6_mask=%d\n",business_data2.serve_ipv6_mask);
	index = index + 1;
	memcpy(business_data2.default_ipv6_gw, &data[index], 16);
	/*for(i=0;i<16;i++){
		printf("default_ipv6_gw[%d]=%d\n",i,business_data2.default_ipv6_gw[i]);
	}*/
	index = index + 16;
	memcpy(business_data2.dns_ipv4, &data[index], 4);
	/*for(i=0;i<4;i++) {
		printf("dns_ipv4[%d]=%d\n",i,business_data2.dns_ipv4[i]);
	}*/
	index = index + 4;
	memcpy(business_data2.dns_ipv4_copy, &data[index], 4);
	/*for(i=0;i<4;i++) {
		printf("dns_ipv4_copy[%d]=%d\n",i,business_data2.dns_ipv4_copy[i]);
	}*/
	index = index + 4;
	memcpy(business_data2.dns_ipv6, &data[index], 16);
	/*for(i=0; i<16; i++) {
		printf("dns_ipv6[%d]=%d\n",i,business_data2.dns_ipv6[i]);
	}*/
	index = index + 16;
	memcpy(business_data2.dns_ipv6_copy, &data[index], 16);
/*	for(i=0; i<16; i++) {
		printf("dns_ipv6_copy[%d]=%d\n",i,business_data2.dns_ipv6_copy[i]);
	}*/
	
	result->validData = false;
	result->opStatus.status = confirm->MSTATUS;
	if (0 == result->opStatus.status) 
	{
		memcpy(result->data.hgbusinessInfo.wan_name_1, business_data1.wan_name, 100);
		memcpy(result->data.hgbusinessInfo.vlan_1, business_data1.vlan, 2);
		result->data.hgbusinessInfo.connection_mode_1 = business_data1.connection_mode;
		result->data.hgbusinessInfo.ip_assigned_mode_1 = business_data1.ip_assigned_mode;
		result->data.hgbusinessInfo.service_type_1 = business_data1.service_type;
		result->data.hgbusinessInfo.connection_protocol_1 = business_data1.connection_protocol;
		memcpy(result->data.hgbusinessInfo.bind_interface_1, business_data1.bind_interface, 4);
		memcpy(result->data.hgbusinessInfo.user_name_1, business_data1.pppoe_user_name, 100);
		memcpy(result->data.hgbusinessInfo.password_1, business_data1.pppoe_password, 100);
		memcpy(result->data.hgbusinessInfo.ipv4_addr_1, business_data1.serve_ipv4_addr, 4);
		memcpy(result->data.hgbusinessInfo.ipv4_mask_1, business_data1.serve_ipv4_mask, 4);
		memcpy(result->data.hgbusinessInfo.ipv4_gw_1, business_data1.default_ipv4_gw, 4);
		memcpy(result->data.hgbusinessInfo.dns_ipv4_1, business_data1.dns_ipv4, 4);
		memcpy(result->data.hgbusinessInfo.dns_ipv4_copy_1, business_data1.dns_ipv4_copy, 4);

		memcpy(result->data.hgbusinessInfo.wan_name_2, business_data2.wan_name, 100);
		memcpy(result->data.hgbusinessInfo.vlan_2, business_data2.vlan, 2);
		result->data.hgbusinessInfo.connection_mode_2 = business_data2.connection_mode;
		result->data.hgbusinessInfo.ip_assigned_mode_2 = business_data2.ip_assigned_mode;
		result->data.hgbusinessInfo.service_type_2 = business_data2.service_type;
		result->data.hgbusinessInfo.connection_protocol_2 = business_data2.connection_protocol;
		memcpy(result->data.hgbusinessInfo.bind_interface_2, business_data2.bind_interface, 4);
		memcpy(result->data.hgbusinessInfo.user_name_2, business_data2.pppoe_user_name, 100);
		memcpy(result->data.hgbusinessInfo.password_2, business_data2.pppoe_password, 100);
		memcpy(result->data.hgbusinessInfo.ipv4_addr_2, business_data2.serve_ipv4_addr, 4);
		memcpy(result->data.hgbusinessInfo.ipv4_mask_2, business_data2.serve_ipv4_mask, 4);
		memcpy(result->data.hgbusinessInfo.ipv4_gw_2, business_data2.default_ipv4_gw, 4);
		memcpy(result->data.hgbusinessInfo.dns_ipv4_2, business_data2.dns_ipv4, 4);
		memcpy(result->data.hgbusinessInfo.dns_ipv4_copy_2, business_data2.dns_ipv4_copy, 4);

		result->validData = true;
	}
	return (result->opStatus.status);
}

#endif

