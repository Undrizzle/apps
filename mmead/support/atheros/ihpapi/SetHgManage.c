#ifndef SETHGMANAGE_SOURCE
#define SETHGMANAGE_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <public.h>

#include "ihpapi.h"
#include "ihp.h"
 
size_t ihpapi_SetHgManage(uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], T_szHgManage manage) 
{
	uint8_t wan_name_length_v = strlen(manage.wan_name) + 1;
	uint8_t pppoe_user_name_length_v = strlen(manage.user_name) + 1;
	uint8_t pppoe_password_length_v = strlen(manage.password) + 1;

	struct __packed vs_set_hg_manage_req
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
		uint8_t acs_user_name_length;
		uint8_t acs_user_name[8];
		uint8_t acs_password_length;
		uint8_t acs_password[13];
		uint8_t acs_url_length;
		uint8_t acs_url[31];
		uint8_t request_user_name_length;
		uint8_t request_user_name[12];
		uint8_t request_password_length;
		uint8_t request_password[14];
		uint8_t request_acs_url_length;
		uint8_t request_acs_url[26];
		uint8_t ip_assigned_mode;
		uint8_t connection_protocol;
		uint8_t pppoe_user_name_length;
		uint8_t pppoe_user_name[pppoe_user_name_length_v];
		uint8_t pppoe_password_length;
		uint8_t pppoe_password[pppoe_password_length_v];
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
	}
	* request = (struct vs_set_hg_manage_req *)(buffer);

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
	request->extend_SubType = ihtons(0x1001);
	request->variable_length = wan_name_length_v + 0x08 + 0x0d + 0x1f + 0x0c + 0x0e + 0x1a + pppoe_user_name_length_v + pppoe_password_length_v;
	request->wan_name_length = wan_name_length_v;
	memcpy(request->wan_name, &manage.wan_name, wan_name_length_v);
	memcpy(request->vlan, &manage.vlan, 2);
	request->vlan_priority = manage.priority;
	request->acs_user_name_length = 0x08;
	request->acs_user_name[0] = 0x4f;
	request->acs_user_name[1] = 0x70;
	request->acs_user_name[2] = 0x65;
	request->acs_user_name[3] = 0x6e;
	request->acs_user_name[4] = 0x57;
	request->acs_user_name[5] = 0x52;
	request->acs_user_name[6] = 0x54;
	request->acs_user_name[7] = 0;
	request->acs_password_length = 0x0d;
	request->acs_password[0] = 0x6f;
	request->acs_password[1] = 0x70;
	request->acs_password[2] = 0x65;
	request->acs_password[3] = 0x6e;
	request->acs_password[4] = 0x77;
	request->acs_password[5] = 0x72;
	request->acs_password[6] = 0x74;
	request->acs_password[7] = 0x31;
	request->acs_password[8] = 0x32;
	request->acs_password[9] = 0x33;
	request->acs_password[10] = 0x34;
	request->acs_password[11] = 0x35;
	request->acs_password[12] = 0;
	request->acs_url_length = 0x1f;
	request->acs_url[0] = 0x68;
	request->acs_url[1] = 0x74;
	request->acs_url[2] = 0x74;
	request->acs_url[3] = 0x70;
	request->acs_url[4] = 0x3a;
	request->acs_url[5] = 0x2f;
	request->acs_url[6] = 0x2f;
	request->acs_url[7] = 0x31;
	request->acs_url[8] = 0x30;
	request->acs_url[9] = 0x2e;
	request->acs_url[10] = 0x33;
	request->acs_url[11] = 0x32;
	request->acs_url[12] = 0x2e;
	request->acs_url[13] = 0x31;
	request->acs_url[14] = 0x30;
	request->acs_url[15] = 0x30;
	request->acs_url[16] = 0x2e;
	request->acs_url[17] = 0x38;
	request->acs_url[18] = 0x2f;
	request->acs_url[19] = 0x73;
	request->acs_url[20] = 0x65;
	request->acs_url[21] = 0x72;
	request->acs_url[22] = 0x76;
	request->acs_url[23] = 0x65;
	request->acs_url[24] = 0x72;
	request->acs_url[25] = 0x3a;
	request->acs_url[26] = 0x38;
	request->acs_url[27] = 0x30;
	request->acs_url[28] = 0x38;
	request->acs_url[29] = 0x30;
	request->acs_url[30] = 0;
	request->request_user_name_length = 0x0c;
	request->request_user_name[0] = 0x74;
	request->request_user_name[1] = 0x65;
	request->request_user_name[2] = 0x73;
	request->request_user_name[3] = 0x74;
	request->request_user_name[4] = 0x5f;
	request->request_user_name[5] = 0x72;
	request->request_user_name[6] = 0x5f;
	request->request_user_name[7] = 0x75;
	request->request_user_name[8] = 0x73;
	request->request_user_name[9] = 0x65;
	request->request_user_name[10] = 0x72;
	request->request_user_name[11] = 0;
	request->request_password_length = 0x0e;
	request->request_password[0] = 0x74;
	request->request_password[1] = 0x65;
	request->request_password[2] = 0x73;
	request->request_password[3] = 0x74;
	request->request_password[4] = 0x5f;
	request->request_password[5] = 0x72;
	request->request_password[6] = 0x5f;
	request->request_password[7] = 0x70;
	request->request_password[8] = 0x61;
	request->request_password[9] = 0x73;
	request->request_password[10] = 0x73;
	request->request_password[11] = 0x77;
	request->request_password[12] = 0x64;
	request->request_password[13] = 0;
	request->request_acs_url_length = 0x1a;
	request->request_acs_url[0] = 0x68;
	request->request_acs_url[1] = 0x74;
	request->request_acs_url[2] = 0x74;
	request->request_acs_url[3] = 0x70;
	request->request_acs_url[4] = 0x3a;
	request->request_acs_url[5] = 0x2f;
	request->request_acs_url[6] = 0x2f;
	request->request_acs_url[7] = 0x31;
	request->request_acs_url[8] = 0x32;
	request->request_acs_url[9] = 0x37;
	request->request_acs_url[10] = 0x2e;
	request->request_acs_url[11] = 0x30;
	request->request_acs_url[12] = 0x2e;
	request->request_acs_url[13] = 0x30;
	request->request_acs_url[14] = 0x2e;
	request->request_acs_url[15] = 0x31;
	request->request_acs_url[16] = 0x3a;
	request->request_acs_url[17] = 0x39;
	request->request_acs_url[18] = 0xf8;
	request->request_acs_url[19] = 0x01;
	request->request_acs_url[20] = 0x10;
	request->request_acs_url[21] = 0x5f;
	request->request_acs_url[22] = 0x30;
	request->request_acs_url[23] = 0x39;
	request->request_acs_url[24] = 0x30;
	request->request_acs_url[25] = 0;
	request->ip_assigned_mode = manage.ip_assigned_mode;
	request->connection_protocol = manage.connection_protocol;

	request->pppoe_user_name_length = pppoe_user_name_length_v;
	memcpy(request->pppoe_user_name, manage.user_name, pppoe_user_name_length_v);
	request->pppoe_password_length = pppoe_password_length_v;
	memcpy(request->pppoe_password, manage.password, pppoe_password_length_v);

	memcpy(request->mng_ipv4_addr, &manage.ipv4_addr, 4);
	memcpy(request->mng_ipv4_mask, &manage.ipv4_mask, 4);
	memcpy(request->default_ipv4_gw, &manage.ipv4_gw, 4);
	memset(request->mng_ipv6_addr, 0, 16);
	request->mng_ipv6_mask = 0;
	memset(request->default_ipv6_gw, 0, 16);
	memcpy(request->dns_ipv4, &manage.dns_ipv4, 4);
	memcpy(request->dns_ipv4_copy, &manage.dns_ipv4_copy, 4);
	memset(request->dns_ipv6, 0, 16);
	memset(request->dns_ipv6_copy, 0, 16);
	
	return (IHPAPI_ETHER_MAX_LEN/2);
}

#endif

