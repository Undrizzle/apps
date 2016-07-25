/*
 * cbatAction.c 
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

#include "util_funcs.h"
#include "cbatAction.h"
#include <snmp2cmm.h>
#include <dbsapi.h>
//#include <trap_private.h>

/* SYNTAX TruthValue */
static long cbatAction_mpReset = 2;
/* SYNTAX TruthValue */
static long cbatAction_restoreDefault = 2;
/* SYNTAX TruthValue */
static long cbatAction_saveConfig = 2;
/* SYNTAX TruthValue */
static long cbatAction_sendHeartbeat = 2;

oid cbatAction_variables_oid[] = { EOC_PREVAIL_OID, 8, 6 };
const int cbatAction_variables_oid_size = OID_LENGTH(cbatAction_variables_oid);

static struct variable1 cbatAction_variables[] = 
{
	{OID_PREVAIL_CBAT_ACTION_MP_RESET, 		ASN_INTEGER, 	RWRITE,	var_cbatAction, 1, {1}},
	{OID_PREVAIL_CBAT_ACTION_SAVE_CONFIG, 		ASN_INTEGER, 	RWRITE, 	var_cbatAction, 1, {2}},
	{OID_PREVAIL_CBAT_ACTION_RESTORE_DEFAULT, 	ASN_INTEGER, 	RWRITE,	var_cbatAction, 1, {3}},
	{OID_PREVAIL_CBAT_ACTION_SEND_HEARTBEAT, 	ASN_INTEGER, 	RWRITE,	var_cbatAction, 1, {4}}	
};

int write_mpReset(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	if (var_val_type != ASN_INTEGER)
	{
		DEBUGMSGTL(("write_mpReset", "SNMP_ERR_WRONGTYPE\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (action == COMMIT)
	{
		if( 1 == (*((long *) var_val) ))
		{
			if( 0 != snmp2cmm_resetMp() )
			{
				return SNMP_ERR_GENERR;
			}
		}
	}	
	return SNMP_ERR_NOERROR;
}

int write_saveConfig(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	if (var_val_type != ASN_INTEGER)
	{
		DEBUGMSGTL(("write_saveConfig", "SNMP_ERR_WRONGTYPE\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (action == COMMIT)
	{
		if( 1 == (*((long *) var_val) ))
		{
			if( 0 != dbsFflush(dbsdev) )
			{
				return SNMP_ERR_GENERR;
			}
		}
	}	
	return SNMP_ERR_NOERROR;
}

int write_restoreDefault(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	if (var_val_type != ASN_INTEGER)
	{
		DEBUGMSGTL(("write_restoreDefault", "SNMP_ERR_WRONGTYPE\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (action == COMMIT)
	{
		if( 1 == (*((long *) var_val) ))
		{
			if( 0 != snmp2cmm_restoreDefault() )
			{
				return SNMP_ERR_GENERR;
			}
		}
	}	
	return SNMP_ERR_NOERROR;
}

int write_sendHeartbeat(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	if (var_val_type != ASN_INTEGER)
	{
		DEBUGMSGTL(("write_sendHeartbeat", "SNMP_ERR_WRONGTYPE\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (action == COMMIT)
	{
		if( 1 == (*((long *) var_val) ))
		{
			/* 支持定时自动发送心跳，无需点击时发送*/
			#if 0
			if( 0 != snmp2cmm_sendHeartbeat() )
			{
				return SNMP_ERR_GENERR;
			}
			#endif
		}
	}	
	return SNMP_ERR_NOERROR;
}


unsigned char *var_cbatAction(struct variable *vp,
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
		case OID_PREVAIL_CBAT_ACTION_MP_RESET:
		{
			*write_method = write_mpReset;
			*var_len = sizeof(cbatAction_mpReset);
			return (unsigned char *) &cbatAction_mpReset; 
			break;
		}
		case OID_PREVAIL_CBAT_ACTION_SAVE_CONFIG:
		{
			*write_method = write_saveConfig;
			*var_len = sizeof(cbatAction_saveConfig);
			return (unsigned char *) &cbatAction_saveConfig; 
			break;
		}
		case OID_PREVAIL_CBAT_ACTION_RESTORE_DEFAULT:
		{
			*write_method = write_restoreDefault;
			*var_len = sizeof(cbatAction_restoreDefault);
			return (unsigned char *) &cbatAction_restoreDefault; 
			break;
		}
		case OID_PREVAIL_CBAT_ACTION_SEND_HEARTBEAT:
		{
			*write_method = write_sendHeartbeat;
			*var_len = sizeof(cbatAction_sendHeartbeat);
			return (unsigned char *) &cbatAction_sendHeartbeat; 
			break;
		}
		default:
		{
			break;
		}
	}
	return 0;
	
}

void init_cbatAction(void)
{
	REGISTER_MIB("cbatAction/cbatAction", cbatAction_variables,  variable1, cbatAction_variables_oid);	
}

