/*
 * systemInfo.c 
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
#include "systemInfo.h"

static long systemInfo_cltAmount = 0;
static long systemInfo_cnuAmount = 0;
static char systemInfo_hwVersion[64] = {0};
static char systemInfo_swVersion[64] = {0};
static long systemInfo_falshSize = 0;
static long systemInfo_sdramSize = 0;
static char systemInfo_Manufacturer[128] = {0};
static long systemInfo_devModel = 0;

oid systemInfo_variables_oid[] = { EOC_PREVAIL_OID, 8, 4 };

static struct variable1 systemInfo_variables[] = 
{
	{OID_PREVAIL_SYSINFO_CLT_AMOUNT, 		ASN_INTEGER, 	RONLY,	var_systemInfo, 1, {1}},
	{OID_PREVAIL_SYSINFO_CNU_AMOUNT, 		ASN_INTEGER, 	RONLY,	var_systemInfo, 1, {2}},
	{OID_PREVAIL_SYSINFO_HW_VERSION, 		ASN_OCTET_STR, 	RONLY,	var_systemInfo, 1, {3}},
	{OID_PREVAIL_SYSINFO_SW_VERSION, 		ASN_OCTET_STR, 	RONLY,	var_systemInfo, 1, {4}},
	{OID_PREVAIL_SYSINFO_FLASH_SIZE, 		ASN_INTEGER, 	RONLY,	var_systemInfo, 1, {5}},
	{OID_PREVAIL_SYSINFO_SDRAM_SIZE, 		ASN_INTEGER, 	RONLY,	var_systemInfo, 1, {6}},
	{OID_PREVAIL_SYSINFO_MANUFACTURER, 	ASN_OCTET_STR, 	RONLY, 	var_systemInfo, 1, {7}},
	{OID_PREVAIL_SYSINFO_DEVICE_MODEL, 		ASN_INTEGER, 	RONLY, 	var_systemInfo, 1, {8}}
};

void load_systemInfo(void)
{
	st_dbsSysinfo row;
	
	if( CMM_SUCCESS == dbsGetSysinfo(dbsdev, 1, &row) )
	{
		systemInfo_falshSize = row.col_flashsize;
		systemInfo_sdramSize = row.col_ramsize;
		strcpy(systemInfo_Manufacturer, row.col_mfinfo);
		strcpy(systemInfo_hwVersion, row.col_hwver);
		strcpy(systemInfo_swVersion, row.col_appver);
		strcpy(systemInfo_Manufacturer, row.col_mfinfo);
		systemInfo_falshSize = row.col_flashsize;
		systemInfo_sdramSize = row.col_ramsize;
		systemInfo_cltAmount = MAX_CLT_AMOUNT_LIMIT;
		systemInfo_cnuAmount = MAX_CNU_AMOUNT_LIMIT;
		systemInfo_devModel = snmp2cmm_mapDevModel(row.col_model);
	}
	else
	{
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "snmp load_systemInfo failed");
	}
	
	
}

unsigned char *var_systemInfo(struct variable *vp,
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
	
	*var_len = sizeof (uint8_t); 
       *write_method = NULL;
	
	switch(vp->magic)
	{
		case OID_PREVAIL_SYSINFO_CLT_AMOUNT:
			*var_len = sizeof(systemInfo_cltAmount);
			return (unsigned char *) &systemInfo_cltAmount; 
			break;
		case OID_PREVAIL_SYSINFO_CNU_AMOUNT:
			*var_len = sizeof(systemInfo_cnuAmount);
			return (unsigned char *) &systemInfo_cnuAmount; 
			break;
		case OID_PREVAIL_SYSINFO_HW_VERSION:
			*var_len = strlen(systemInfo_hwVersion);
			return systemInfo_hwVersion; 
			break;
		case OID_PREVAIL_SYSINFO_SW_VERSION:
			*var_len = strlen(systemInfo_swVersion);
			return systemInfo_swVersion; 
			break;
		case OID_PREVAIL_SYSINFO_FLASH_SIZE:
			*var_len = sizeof(systemInfo_falshSize);
			return (unsigned char *) &systemInfo_falshSize; 
			break;
		case OID_PREVAIL_SYSINFO_SDRAM_SIZE:
			*var_len = sizeof(systemInfo_sdramSize);
			return (unsigned char *) &systemInfo_sdramSize; 
			break;
		case OID_PREVAIL_SYSINFO_MANUFACTURER:
			*var_len = strlen(systemInfo_Manufacturer);
			return systemInfo_Manufacturer; 
			break;
		case OID_PREVAIL_SYSINFO_DEVICE_MODEL:
			*var_len = sizeof(systemInfo_devModel);
			return (unsigned char *) &systemInfo_devModel; 
			break;
		default:
			break;
	}
	return 0;
	
}

void init_systemInfo(void)
{
	REGISTER_MIB("systemInfo/systemInfo", systemInfo_variables,  variable1, systemInfo_variables_oid);
	load_systemInfo();
}

