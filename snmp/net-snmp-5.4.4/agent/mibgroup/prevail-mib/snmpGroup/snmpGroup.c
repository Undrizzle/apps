/*
 * snmpGroup.c 
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
#include "snmpGroup.h"

static char snmpGroup_snmpVersion[16];
static char snmpGroup_readCommunity[64];
static char snmpGroup_writeCommunity[64];
static char snmpGroup_trapVersion[16];
static char snmpGroup_trapCommunity[64];
static char snmpGroup_trapServerAddress[32];
static long snmpGroup_trapServerPort;

oid snmpGroup_variables_oid[] = { EOC_PREVAIL_OID, 8, 2 };

static struct variable1 snmpGroup_variables[] = 
{
	{OID_SNMP_VERSION, 					ASN_OCTET_STR, 	RONLY,	var_snmpGroup, 1, {1}},
	{OID_SNMP_READ_COMMUNITY, 			ASN_OCTET_STR, 	RWRITE,	var_snmpGroup, 1, {2}},
	{OID_SNMP_WRITE_COMMUNITY, 		ASN_OCTET_STR, 	RWRITE,	var_snmpGroup, 1, {3}},
	{OID_SNMP_TRAP_VERSION, 			ASN_OCTET_STR, 	RONLY, 	var_snmpGroup, 1, {4}},
	{OID_SNMP_TRAP_COMMUNITY, 			ASN_OCTET_STR, 	RWRITE,	var_snmpGroup, 1, {5}},
	{OID_SNMP_TRAP_SERVER_IP_ADDRESS,	ASN_OCTET_STR, 	RWRITE,	var_snmpGroup, 1, {6}},
	{OID_SNMP_TRAP_SERVER_PORT, 		ASN_INTEGER, 	RWRITE,	var_snmpGroup, 1, {7}}
};

int write_readCommunity(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_TEXT_V strValue;
	
	if (var_val_type != ASN_OCTET_STR)
	{
		DEBUGMSGTL(("write_readCommunity", "not ASN_OCTET_STR\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (var_val_len >= 64)
	{
		DEBUGMSGTL(("write_readCommunity", "bad ASN_OCTET_STR length\n"));
		return SNMP_ERR_WRONGLENGTH;
	}
	
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( (var_val_len == strlen(snmpGroup_readCommunity)) &&
			(memcmp(snmpGroup_readCommunity, var_val, var_val_len) == 0))
		{
			return SNMP_ERR_NOERROR;
		}
		if (var_val_len != 0)
		{
			strcpy(snmpGroup_readCommunity, var_val);
			snmpGroup_readCommunity[var_val_len] = '\0';
		}
		else
		{
			snmpGroup_readCommunity[0] = '\0';
		}
		/* 写入数据库*/
		strValue.ci.tbl = DBS_SYS_TBL_ID_SNMPINFO;
		strValue.ci.row = 1;
		strValue.ci.col = DBS_SYS_TBL_SNMP_COL_ID_RC;
		strValue.ci.colType = DBS_TEXT;
		strValue.len = var_val_len;
		strcpy(strValue.text, snmpGroup_readCommunity);
		
		ret = dbsUpdateText(dbsdev, &strValue);
		
		if( SNMP_ERR_NOERROR != ret )
		{
			DEBUGMSGTL(("write_readCommunity", "SNMP_CMM_COM_ERROR\n"));
		}
	}
	
	return ret;
}


int write_writeCommunity(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_TEXT_V strValue;
	
	if (var_val_type != ASN_OCTET_STR)
	{
		DEBUGMSGTL(("write_writeCommunity", "not ASN_OCTET_STR\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (var_val_len >= 64)
	{
		DEBUGMSGTL(("write_writeCommunity", "bad ASN_OCTET_STR length\n"));
		return SNMP_ERR_WRONGLENGTH;
	}
	
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( (var_val_len == strlen(snmpGroup_writeCommunity)) &&
			(memcmp(snmpGroup_writeCommunity, var_val, var_val_len) == 0))
		{
			return SNMP_ERR_NOERROR;
		}
		if (var_val_len != 0)
		{
			strcpy(snmpGroup_writeCommunity, var_val);
			snmpGroup_writeCommunity[var_val_len] = '\0';
		}
		else
		{
			snmpGroup_writeCommunity[0] = '\0';
		}
		/* 写入数据库*/
		strValue.ci.tbl = DBS_SYS_TBL_ID_SNMPINFO;
		strValue.ci.row = 1;
		strValue.ci.col = DBS_SYS_TBL_SNMP_COL_ID_WC;
		strValue.ci.colType = DBS_TEXT;
		strValue.len = var_val_len;
		strcpy(strValue.text, snmpGroup_writeCommunity);
		
		ret = dbsUpdateText(dbsdev, &strValue);
		
		if( SNMP_ERR_NOERROR != ret )
		{
			DEBUGMSGTL(("write_writeCommunity", "SNMP_CMM_COM_ERROR\n"));
		}
	}
	
	return ret;
}

int write_trapCommunity(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_TEXT_V strValue;
	
	if (var_val_type != ASN_OCTET_STR)
	{
		DEBUGMSGTL(("write_trapCommunity", "not ASN_OCTET_STR\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (var_val_len >= 64)
	{
		DEBUGMSGTL(("write_trapCommunity", "bad ASN_OCTET_STR length\n"));
		return SNMP_ERR_WRONGLENGTH;
	}
	
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( (var_val_len == strlen(snmpGroup_trapCommunity)) &&
			(memcmp(snmpGroup_trapCommunity, var_val, var_val_len) == 0))
		{
			return SNMP_ERR_NOERROR;
		}
		if (var_val_len != 0)
		{
			strcpy(snmpGroup_trapCommunity, var_val);
			snmpGroup_trapCommunity[var_val_len] = '\0';
		}
		else
		{
			snmpGroup_trapCommunity[0] = '\0';
		}
		/* 写入数据库*/
		strValue.ci.tbl = DBS_SYS_TBL_ID_SNMPINFO;
		strValue.ci.row = 1;
		strValue.ci.col = DBS_SYS_TBL_SNMP_COL_ID_TC;
		strValue.ci.colType = DBS_TEXT;
		strValue.len = var_val_len;
		strcpy(strValue.text, snmpGroup_trapCommunity);
		
		ret = dbsUpdateText(dbsdev, &strValue);
		
		if( SNMP_ERR_NOERROR != ret )
		{
			DEBUGMSGTL(("write_trapCommunity", "SNMP_CMM_COM_ERROR\n"));
		}
	}
	
	return ret;
}

int write_trapServerIpAddress(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_TEXT_V strValue;
	
	if (var_val_type != ASN_OCTET_STR)
	{
		DEBUGMSGTL(("write_trapServerIpAddress", "not ASN_OCTET_STR\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (var_val_len >= 32)
	{
		DEBUGMSGTL(("write_trapServerIpAddress", "bad ASN_OCTET_STR length\n"));
		return SNMP_ERR_WRONGLENGTH;
	}
	if( !snmp2cmm_isIpv4Valid(var_val, var_val_len) )
	{
		DEBUGMSGTL(("write_trapServerIpAddress", "invalid ipv4 address\n"));
		return SNMP_ERR_WRONGVALUE;
	}
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( (var_val_len == strlen(snmpGroup_trapServerAddress)) &&
			(memcmp(snmpGroup_trapServerAddress, var_val, var_val_len) == 0))
		{
			return SNMP_ERR_NOERROR;
		}
		if (var_val_len != 0)
		{
			strcpy(snmpGroup_trapServerAddress, var_val);
			snmpGroup_trapServerAddress[var_val_len] = '\0';
		}
		else
		{
			snmpGroup_trapServerAddress[0] = '\0';
		}
		/* 写入数据库*/
		strValue.ci.tbl = DBS_SYS_TBL_ID_SNMPINFO;
		strValue.ci.row = 1;
		strValue.ci.col = DBS_SYS_TBL_SNMP_COL_ID_SA;
		strValue.ci.colType = DBS_TEXT;
		strValue.len = var_val_len;
		strcpy(strValue.text, snmpGroup_trapServerAddress);
		
		ret = dbsUpdateText(dbsdev, &strValue);
		
		if( SNMP_ERR_NOERROR != ret )
		{
			DEBUGMSGTL(("write_trapServerIpAddress", "SNMP_CMM_COM_ERROR\n"));
		}
		else
		{
			SnmpdReconfig(1);
		}
	}
	
	return ret;
}

int write_trapServerport(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_INTEGER_V iValue;
	
	if (var_val_type != ASN_INTEGER)
	{
		DEBUGMSGTL(("write_trapServerport", "not ASN_INTEGER\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if( (*(long *)var_val < 1) || (*(long *)var_val > 65535) )
	{
		DEBUGMSGTL(("write_trapServerport", "invalid port\n"));
		return SNMP_ERR_WRONGVALUE;
	}
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( snmpGroup_trapServerPort == (*((long *) var_val) ))
		{
			return SNMP_ERR_NOERROR;
		}
		else
		{
			snmpGroup_trapServerPort = *((long *) var_val);
			iValue.ci.tbl = DBS_SYS_TBL_ID_SNMPINFO;
			iValue.ci.row = 1;
			iValue.ci.col = DBS_SYS_TBL_SNMP_COL_ID_PA;
			iValue.ci.colType = DBS_INTEGER;
			iValue.len = sizeof(uint32_t);
			iValue.integer = snmpGroup_trapServerPort;

			ret = dbsUpdateInteger(dbsdev, &iValue);
			
			if( SNMP_ERR_NOERROR != ret )
			{
				DEBUGMSGTL(("write_trapServerport", "SNMP_CMM_COM_ERROR\n"));
			}
			else
			{
				SnmpdReconfig(1);
			}
		}
	}
	
	return ret;
}

int load_snmpGroup(void)
{
	st_dbsSnmp snmpConfig;	

	if( 0 == dbsGetSnmp(dbsdev, 1, &snmpConfig) )
	{
		strcpy(snmpGroup_snmpVersion, "snmpV2");
		strcpy(snmpGroup_readCommunity, snmpConfig.col_rdcom);
		strcpy(snmpGroup_writeCommunity, snmpConfig.col_wrcom);
		strcpy(snmpGroup_trapVersion, "trapV2");
		strcpy(snmpGroup_trapCommunity, snmpConfig.col_trapcom);
		strcpy(snmpGroup_trapServerAddress, snmpConfig.col_sina);
		snmpGroup_trapServerPort = snmpConfig.col_tpa;
	}
	else
	{
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "snmp load_snmpGroup failed");
		strcpy(snmpGroup_snmpVersion, "snmpV2");
		strcpy(snmpGroup_readCommunity, "public");
		strcpy(snmpGroup_writeCommunity, "private");
		strcpy(snmpGroup_trapVersion, "trapV2");
		strcpy(snmpGroup_trapCommunity, "public");
		strcpy(snmpGroup_trapServerAddress, "192.168.223.254");
		snmpGroup_trapServerPort = 162;
	}
	return 0;
}

unsigned char *var_snmpGroup(struct variable *vp,
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
	else if( load_snmpGroup() != 0)
	{
		return NULL;
	}
	
	*var_len = sizeof (uint8_t); 
       *write_method = NULL;
	
	switch(vp->magic)
	{
		case OID_SNMP_VERSION:			
			*var_len = strlen(snmpGroup_snmpVersion);
			return snmpGroup_snmpVersion; 
			break;
		case OID_SNMP_READ_COMMUNITY:
			*write_method = write_readCommunity;
			*var_len = strlen(snmpGroup_readCommunity);
			return snmpGroup_readCommunity; 
			break;
		case OID_SNMP_WRITE_COMMUNITY:
			*write_method = write_writeCommunity;
			*var_len = strlen(snmpGroup_writeCommunity);
			return snmpGroup_writeCommunity; 
			break;
		case OID_SNMP_TRAP_VERSION:
			*var_len = strlen(snmpGroup_trapVersion);
			return snmpGroup_trapVersion; 
			break;
		case OID_SNMP_TRAP_COMMUNITY:
			*write_method = write_trapCommunity;
			*var_len = strlen(snmpGroup_trapCommunity);
			return snmpGroup_trapCommunity; 
			break;
		case OID_SNMP_TRAP_SERVER_IP_ADDRESS:
			*write_method = write_trapServerIpAddress;
			*var_len = strlen(snmpGroup_trapServerAddress);
			return snmpGroup_trapServerAddress; 
			break;
		case OID_SNMP_TRAP_SERVER_PORT:
			*write_method = write_trapServerport;
			*var_len = sizeof(snmpGroup_trapServerPort);
			return (unsigned char *) &snmpGroup_trapServerPort; 
			break;
		default:
			break;
	}
	return 0;
	
}

void init_snmpGroup(void)
{
	REGISTER_MIB("snmpGroup/snmpGroup", snmpGroup_variables,  variable1, snmpGroup_variables_oid);
}

