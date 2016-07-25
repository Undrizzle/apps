#ifndef SETHGBUSINESS_SOURCE
#define SETHGBUSINESS_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <public.h>

#include "ihpapi.h"
#include "ihp.h"
 
size_t ihpapi_SetHgBusiness(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], T_szSetHgBusiness manage) 
{
	uint8_t wan_name_length_v = strlen(manage.wan_name) + 1;
	uint8_t pppoe_user_name_length_v = strlen(manage.user_name) + 1;
	uint8_t pppoe_password_length_v = strlen(manage.password) + 1;
	uint16_t length =  1+ wan_name_length_v + 11 + 1 + pppoe_user_name_length_v + 1+ pppoe_password_length_v + 85;
	
	struct __packed vs_set_hg_business_req
	{
		struct header_vs header;
		uint8_t action;
		uint8_t case_MainType;
		uint16_t case_SubType;
		uint8_t case_length;
		uint16_t RSVD;
		uint8_t case_val1;
		uint8_t case_val2;
		uint8_t extend_MainType;
		uint16_t extend_SubType;
		uint8_t variable_length;
		uint8_t wan_name_length;
		uint8_t wan_name[wan_name_length_v];
		uint8_t vlan[2];
		uint8_t vlan_priority;
		uint8_t connection_mode;
		uint8_t ip_assigned_mode;
		uint8_t service_type;
		uint8_t connection_protocol;
		uint8_t bind_interface[4];
		uint8_t pppoe_user_name_length;
		uint8_t pppoe_user_name[pppoe_user_name_length_v];
		uint8_t pppoe_password_length;
		uint8_t pppoe_password[pppoe_password_length_v];
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
	}
	* request = (struct vs_set_hg_business_req *)(buffer);

	size_t offset = 0;
	offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, da, sa);             //ODA,OSA,MTYPE
	offset += EncodeIntellonHeader (buffer + offset, bufferLen - offset, (VS_HOME_GATEWAY_OPERATION | MMTYPE_REQ));   //MMV,MMTYPE,OUI
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

	request->action = 0x01;
	request->case_MainType = 0xf7;
	request->case_SubType = ihtons(0x0003);
	request->case_length = 0x04;
	request->RSVD = ihtons(0);
	request->case_val1 = 0;
	request->case_val2 = 0;
	request->extend_MainType = 0xf8;
	request->extend_SubType = ihtons(0x1002);
	if(length > 127) {
		request->variable_length = 127;
	} else {
		request->variable_length = 1+ wan_name_length_v + 11 + 1 + pppoe_user_name_length_v + 1+ pppoe_password_length_v + 85;
	}
	request->wan_name_length = wan_name_length_v;
	memcpy(request->wan_name, &manage.wan_name, wan_name_length_v);
	memcpy(request->vlan, &manage.vlan, 2);
	request->vlan_priority = manage.priority;
	request->connection_mode = manage.connection_mode;
	request->ip_assigned_mode = manage.ip_assigned_mode;
	request->service_type = manage.service_type;
	request->connection_protocol = manage.connection_protocol;
	memcpy(request->bind_interface, &manage.bind_interface, 4);
	request->pppoe_user_name_length = pppoe_user_name_length_v;
	memcpy(request->pppoe_user_name, manage.user_name, pppoe_user_name_length_v);
	request->pppoe_password_length = pppoe_password_length_v;
	memcpy(request->pppoe_password, manage.password, pppoe_password_length_v);

	memcpy(request->serve_ipv4_addr, &manage.ipv4_addr, 4);
	memcpy(request->serve_ipv4_mask, &manage.ipv4_mask, 4);
	memcpy(request->default_ipv4_gw, &manage.ipv4_gw, 4);
	memset(request->serve_ipv6_addr, 0, 16);
	request->serve_ipv6_mask = 0;
	memset(request->default_ipv6_gw, 0, 16);
	memcpy(request->dns_ipv4, &manage.dns_ipv4, 4);
	memcpy(request->dns_ipv4_copy, &manage.dns_ipv4_copy, 4);
	memset(request->dns_ipv6, 0, 16);
	memset(request->dns_ipv6_copy, 0, 16);

	if(length > 127) {
		uint8_t temp[127];
		memcpy(temp, &buffer[160], 127);
		buffer[160] = 0xf8;
		buffer[161] = 0x02;
		buffer[162] = 0x10;
		buffer[163] = length - 127;
		memcpy(&buffer[164], temp, 127);
	}
	
	return (IHPAPI_ETHER_MAX_LEN/2);
}

#endif

