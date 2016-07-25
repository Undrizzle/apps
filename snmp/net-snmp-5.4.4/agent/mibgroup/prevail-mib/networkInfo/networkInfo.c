/*
 * networkInfo.c 
 */

#include <net-snmp/net-snmp-config.h>
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#if TIME_WITH_SYS_TIME
# ifdef WIN32
#  include <sys/timeb.h>
# else
#  include <sys/time.h>
# endif
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#if HAVE_WINSOCK_H
#include <winsock.h>
#endif

#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include <dbsapi.h>
#include <snmp2cmm.h>
#include "util_funcs.h"
#include "networkInfo.h"

static char networkInfo_ipAddress[32];
static char networkInfo_netmask[32];
static char networkInfo_gateway[32];
/* SYNTAX TruthValue */
static long networkInfo_mgmtVlanStatus = 2;
static long networkInfo_mgmtVlanId = 1;
static char networkInfo_macAddress[6];

oid networkInfo_variables_oid[] = { EOC_PREVAIL_OID, 8, 5 };

static struct variable1 networkInfo_variables[] = 
{
	{OID_PREVAIL_NETWORK_INFO_IP_ADDRESS, 		ASN_OCTET_STR, 	RWRITE,	var_networkInfo, 1, {1}},
	{OID_PREVAIL_NETWORK_INFO_NET_MASK, 		ASN_OCTET_STR, 	RWRITE,	var_networkInfo, 1, {2}},
	{OID_PREVAIL_NETWORK_INFO_GATEWAY, 		ASN_OCTET_STR, 	RWRITE,	var_networkInfo, 1, {3}},
	{OID_PREVAIL_NETWORK_INFO_MGMT_VLAN_STS, 	ASN_INTEGER, 	RWRITE, 	var_networkInfo, 1, {4}},
	{OID_PREVAIL_NETWORK_INFO_MGMT_VLAN_ID, 	ASN_INTEGER, 	RWRITE,	var_networkInfo, 1, {5}},
	{OID_PREVAIL_NETWORK_INFO_MAC_ADDRESS, 	ASN_OCTET_STR, 	RONLY,	var_networkInfo, 1, {6}}
};

int write_ipAddress(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_TEXT_V strValue;
	
	if (var_val_type != ASN_OCTET_STR)
	{
		DEBUGMSGTL(("write_ipAddress", "not ASN_OCTET_STR\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (var_val_len >= 32)
	{
		DEBUGMSGTL(("write_ipAddress", "bad ASN_OCTET_STR length\n"));
		return SNMP_ERR_WRONGLENGTH;
	}
	if( !snmp2cmm_isIpv4Valid(var_val, var_val_len) )
	{
		DEBUGMSGTL(("write_ipAddress", "invalid ipv4 address\n"));
		return SNMP_ERR_WRONGVALUE;
	}
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( (var_val_len == strlen(networkInfo_ipAddress)) &&
			(memcmp(networkInfo_ipAddress, var_val, var_val_len) == 0))
		{
			return SNMP_ERR_NOERROR;
		}
		if (var_val_len != 0)
		{
			strcpy(networkInfo_ipAddress, var_val);
			networkInfo_ipAddress[var_val_len] = '\0';
		}
		else
		{
			networkInfo_ipAddress[0] = '\0';
		}
		/* 写入数据库*/
		strValue.ci.tbl = DBS_SYS_TBL_ID_NETWORK;
		strValue.ci.row = 1;
		strValue.ci.col = DBS_SYS_TBL_NETWORK_COL_ID_IP;
		strValue.ci.colType = DBS_TEXT;
		strValue.len = var_val_len;
		strcpy(strValue.text, networkInfo_ipAddress);
		
		ret = dbsUpdateText(dbsdev, &strValue);
		
		if( SNMP_ERR_NOERROR != ret )
		{
			DEBUGMSGTL(("write_ipAddress", "SNMP_CMM_COM_ERROR\n"));
		}
	}
	
	return ret;
}


int write_netmask(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_TEXT_V strValue;
	
	if (var_val_type != ASN_OCTET_STR)
	{
		DEBUGMSGTL(("write_netmask", "not ASN_OCTET_STR\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (var_val_len >= 32)
	{
		DEBUGMSGTL(("write_netmask", "bad ASN_OCTET_STR length\n"));
		return SNMP_ERR_WRONGLENGTH;
	}
	if( !snmp2cmm_isIpv4NetmaskValid(var_val, var_val_len) )
	{
		DEBUGMSGTL(("write_netmask", "invalid ipv4 address\n"));
		return SNMP_ERR_WRONGVALUE;
	}
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( (var_val_len == strlen(networkInfo_netmask)) &&
			(memcmp(networkInfo_netmask, var_val, var_val_len) == 0))
		{
			return SNMP_ERR_NOERROR;
		}
		if (var_val_len != 0)
		{
			strcpy(networkInfo_netmask, var_val);
			networkInfo_netmask[var_val_len] = '\0';
		}
		else
		{
			networkInfo_netmask[0] = '\0';
		}
		/* 写入数据库*/
		strValue.ci.tbl = DBS_SYS_TBL_ID_NETWORK;
		strValue.ci.row = 1;
		strValue.ci.col = DBS_SYS_TBL_NETWORK_COL_ID_MASK;
		strValue.ci.colType = DBS_TEXT;
		strValue.len = var_val_len;
		strcpy(strValue.text, networkInfo_netmask);
		
		ret = dbsUpdateText(dbsdev, &strValue);
		
		if( SNMP_ERR_NOERROR != ret )
		{
			DEBUGMSGTL(("write_netmask", "SNMP_CMM_COM_ERROR\n"));
		}
	}
	
	return ret;
}


int write_gateway(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_TEXT_V strValue;
	
	if (var_val_type != ASN_OCTET_STR)
	{
		DEBUGMSGTL(("write_gateway", "not ASN_OCTET_STR\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (var_val_len >= 32)
	{
		DEBUGMSGTL(("write_gateway", "bad ASN_OCTET_STR length\n"));
		return SNMP_ERR_WRONGLENGTH;
	}
	if( !snmp2cmm_isIpv4Valid(var_val, var_val_len) )
	{
		DEBUGMSGTL(("write_gateway", "invalid ipv4 address\n"));
		return SNMP_ERR_WRONGVALUE;
	}
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( (var_val_len == strlen(networkInfo_gateway)) &&
			(memcmp(networkInfo_gateway, var_val, var_val_len) == 0))
		{
			return SNMP_ERR_NOERROR;
		}
		if (var_val_len != 0)
		{
			strcpy(networkInfo_gateway, var_val);
			networkInfo_gateway[var_val_len] = '\0';
		}
		else
		{
			networkInfo_gateway[0] = '\0';
		}
		/* 写入数据库*/
		strValue.ci.tbl = DBS_SYS_TBL_ID_NETWORK;
		strValue.ci.row = 1;
		strValue.ci.col = DBS_SYS_TBL_NETWORK_COL_ID_GW;
		strValue.ci.colType = DBS_TEXT;
		strValue.len = var_val_len;
		strcpy(strValue.text, networkInfo_gateway);
		
		ret = dbsUpdateText(dbsdev, &strValue);
		
		if( SNMP_ERR_NOERROR != ret )
		{
			DEBUGMSGTL(("write_gateway", "SNMP_CMM_COM_ERROR\n"));
		}
	}
	
	return ret;
}

int write_mgmtVlanStatus(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_INTEGER_V iValue;
	
	if (var_val_type != ASN_INTEGER)
	{
		DEBUGMSGTL(("write_mgmtVlanStatus", "not ASN_INTEGER\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if( (1 != *(long *)var_val) && (2 != *(long *)var_val) )
	{
		DEBUGMSGTL(("write_mgmtVlanStatus", "not ASN_BOOLEAN\n"));
		return SNMP_ERR_WRONGVALUE;
	}
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( networkInfo_mgmtVlanStatus == (*((long *) var_val) ))
		{
			return SNMP_ERR_NOERROR;
		}
		else
		{
			networkInfo_mgmtVlanStatus = *((long *) var_val);
			iValue.ci.tbl = DBS_SYS_TBL_ID_NETWORK;
			iValue.ci.row = 1;
			iValue.ci.col = DBS_SYS_TBL_NETWORK_COL_ID_MVSTS;
			iValue.ci.colType = DBS_INTEGER;
			iValue.len = sizeof(uint32_t);
			iValue.integer = (1==networkInfo_mgmtVlanStatus)?1:0;

			ret = dbsUpdateInteger(dbsdev, &iValue);
			
			if( SNMP_ERR_NOERROR != ret )
			{
				DEBUGMSGTL(("write_mgmtVlanStatus", "SNMP_CMM_COM_ERROR\n"));
			}			
		}
	}
	
	return ret;
}

int write_mgmtVlanId(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_INTEGER_V iValue;
	
	if (var_val_type != ASN_INTEGER)
	{
		DEBUGMSGTL(("write_mgmtVlanId", "not ASN_INTEGER\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if( (*(long *)var_val < 1) || (*(long *)var_val > 4094) )
	{
		DEBUGMSGTL(("write_mgmtVlanId", "invalid vid\n"));
		return SNMP_ERR_WRONGVALUE;
	}
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( networkInfo_mgmtVlanId == (*((long *) var_val) ))
		{
			return SNMP_ERR_NOERROR;
		}
		else
		{
			networkInfo_mgmtVlanId = *((long *) var_val);
			iValue.ci.tbl = DBS_SYS_TBL_ID_NETWORK;
			iValue.ci.row = 1;
			iValue.ci.col = DBS_SYS_TBL_NETWORK_COL_ID_MVID;
			iValue.ci.colType = DBS_INTEGER;
			iValue.len = sizeof(uint32_t);
			iValue.integer = networkInfo_mgmtVlanId;

			ret = dbsUpdateInteger(dbsdev, &iValue);
			
			if( SNMP_ERR_NOERROR != ret )
			{
				DEBUGMSGTL(("write_mgmtVlanId", "SNMP_CMM_COM_ERROR\n"));
			}			
		}
	}
	
	return ret;
}


int load_networkInfo(void)
{
	st_dbsNetwork row;

	if( CMM_SUCCESS == dbsGetNetwork(dbsdev, 1, &row) )
	{
		strcpy(networkInfo_ipAddress, row.col_ip);
		strcpy(networkInfo_netmask, row.col_netmask);
		strcpy(networkInfo_gateway, row.col_gw);
		networkInfo_mgmtVlanStatus = (row.col_mvlan_sts?1:2);
		networkInfo_mgmtVlanId = row.col_mvlan_id;
		snmp2cmm_macs2b(row.col_mac, networkInfo_macAddress);
	}
	else
	{
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "snmp load_networkInfo failed");
		strcpy(networkInfo_ipAddress, "192.168.223.1");
		strcpy(networkInfo_netmask, "255.255.255.0");
		strcpy(networkInfo_gateway, "0.0.0.0");
		networkInfo_mgmtVlanStatus = 2;
		networkInfo_mgmtVlanId = 1;
		snmp2cmm_macs2b("00:00:00:00:00:00", networkInfo_macAddress);		
	}
	
	return SNMP_ERR_NOERROR;
}

unsigned char *var_networkInfo(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method)
{
	if(header_generic(vp, name, length, exact, var_len, write_method) ==  MATCH_FAILED)
	{
		return NULL;
	}
	else if( SNMP_ERR_NOERROR != load_networkInfo() )
	{
		return NULL;
	}
	
	*var_len = sizeof (uint8_t); 
       *write_method = NULL;
	
	switch(vp->magic)
	{
		case OID_PREVAIL_NETWORK_INFO_IP_ADDRESS:
			*write_method = write_ipAddress;
			*var_len = strlen(networkInfo_ipAddress);
			return networkInfo_ipAddress; 
			break;
		case OID_PREVAIL_NETWORK_INFO_NET_MASK:
			*write_method = write_netmask;
			*var_len = strlen(networkInfo_netmask);
			return networkInfo_netmask; 
			break;
		case OID_PREVAIL_NETWORK_INFO_GATEWAY:
			*write_method = write_gateway;
			*var_len = strlen(networkInfo_gateway);
			return networkInfo_gateway; 
			break;		
		case OID_PREVAIL_NETWORK_INFO_MGMT_VLAN_STS:
			*write_method = write_mgmtVlanStatus;
			*var_len = sizeof(networkInfo_mgmtVlanStatus);
			return (unsigned char *) &networkInfo_mgmtVlanStatus; 
			break;
		case OID_PREVAIL_NETWORK_INFO_MGMT_VLAN_ID:
			*write_method = write_mgmtVlanId;
			*var_len = sizeof(networkInfo_mgmtVlanId);
			return (unsigned char *) &networkInfo_mgmtVlanId; 
			break;
		case OID_PREVAIL_NETWORK_INFO_MAC_ADDRESS:			
			*var_len = sizeof(networkInfo_macAddress);
			return networkInfo_macAddress; 
			break;
		default:
			break;
	}
	return 0;
	
}

void init_networkInfo(void)
{
	REGISTER_MIB("networkInfo/networkInfo", networkInfo_variables,  variable1, networkInfo_variables_oid);
}

