/*
 * systemDesc.c 
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
#include "systemDesc.h"

static char systemDesc_enterpriseName[64];
static char systemDesc_enterpriseContact[32];
static char systemDesc_enterpriseEmail[64];
static char systemDesc_enterpriseDevName[64];

oid systemDesc_variables_oid[] = { EOC_PREVAIL_OID, 8, 3 };

static struct variable1 systemDesc_variables[] = 
{
	{OID_PREVAIL_ENTERPRISE_NAME, 		ASN_OCTET_STR, 	RONLY,	var_systemDesc, 1, {1}},
	{OID_PREVAIL_ENTERPRISE_CONTACT, 	ASN_OCTET_STR, 	RONLY,	var_systemDesc, 1, {2}},
	{OID_PREVAIL_ENTERPRISE_EMAIL, 		ASN_OCTET_STR, 	RONLY,	var_systemDesc, 1, {3}},
	{OID_PREVAIL_ENTERPRISE_DEV_NAME, 	ASN_OCTET_STR, 	RONLY, 	var_systemDesc, 1, {4}}
};

void load_systemDesc(void)
{
	strcpy(systemDesc_enterpriseName, "Hangzhou Prevail Optoelectronic Equipment Co.,LTD");
	strcpy(systemDesc_enterpriseContact, "XI YU XI");
	strcpy(systemDesc_enterpriseEmail, "stanhangzhou@gmail.com");
	strcpy(systemDesc_enterpriseDevName, "WEC9720EK");
}

unsigned char *var_systemDesc(struct variable *vp,
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
		case OID_PREVAIL_ENTERPRISE_NAME:			
			*var_len = strlen(systemDesc_enterpriseName);
			return systemDesc_enterpriseName; 
			break;
		case OID_PREVAIL_ENTERPRISE_CONTACT:
			*var_len = strlen(systemDesc_enterpriseContact);
			return systemDesc_enterpriseContact; 
			break;
		case OID_PREVAIL_ENTERPRISE_EMAIL:
			*var_len = strlen(systemDesc_enterpriseEmail);
			return systemDesc_enterpriseEmail; 
			break;
		case OID_PREVAIL_ENTERPRISE_DEV_NAME:
			*var_len = strlen(systemDesc_enterpriseDevName);
			return systemDesc_enterpriseDevName; 
			break;
		default:
			break;
	}
	return 0;
	
}

void init_systemDesc(void)
{
	REGISTER_MIB("systemDesc/systemDesc", systemDesc_variables,  variable1, systemDesc_variables_oid);
	load_systemDesc();
}

