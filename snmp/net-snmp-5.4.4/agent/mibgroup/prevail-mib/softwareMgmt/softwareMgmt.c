/*
 * softwareMgmt.c 
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
#include "softwareMgmt.h"

static char softwareMgmt_currentSoftwareVersion[64];
static char softwareMgmt_softwareUploadTFTPServerIP[32];
static long softwareMgmt_softwareUploadTFTPServerPort;
static char softwareMgmt_softwareUploadLogin[32];
static char softwareMgmt_softwareUploadPassword[16];
static char softwareMgmt_softwareUploadFileName[256];
/* SYNTAX TruthValue */
static long softwareMgmt_softwareUploadProceed;
static long softwareMgmt_softwareUploadResult;

oid softwareMgmt_variables_oid[] = { EOC_PREVAIL_OID, 8, 7 };

static struct variable1 softwareMgmt_variables[] = 
{
	{OID_SNMP_SW_MGMT_CUR_VER, 		ASN_OCTET_STR, 	RONLY,	var_softwareMgmt, 1, {1}},
	{OID_SNMP_SW_MGMT_SERV_IP, 		ASN_OCTET_STR, 	RWRITE,	var_softwareMgmt, 1, {2}},
	{OID_SNMP_SW_MGMT_SERV_PORT, 		ASN_INTEGER, 	RWRITE,	var_softwareMgmt, 1, {3}},
	{OID_SNMP_SW_MGMT_SERV_LOGIN, 	ASN_OCTET_STR, 	RWRITE, 	var_softwareMgmt, 1, {4}},
	{OID_SNMP_SW_MGMT_SERV_PWD, 		ASN_OCTET_STR, 	RWRITE,	var_softwareMgmt, 1, {5}},
	{OID_SNMP_SW_MGMT_SERV_FILE,		ASN_OCTET_STR, 	RWRITE,	var_softwareMgmt, 1, {6}},
	{OID_SNMP_SW_MGMT_UPLOAD_ACTION, 	ASN_INTEGER, 	RWRITE,	var_softwareMgmt, 1, {7}},
	{OID_SNMP_SW_MGMT_UPLOAD_RESULT, 	ASN_INTEGER, 	RONLY,	var_softwareMgmt, 1, {8}}	
};

int write_softwareUploadTFTPServerIP(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_TEXT_V strValue;
	
	if (var_val_type != ASN_OCTET_STR)
	{
		DEBUGMSGTL(("write_softwareUploadTFTPServerIP", "not ASN_OCTET_STR\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (var_val_len >= 32)
	{
		DEBUGMSGTL(("write_softwareUploadTFTPServerIP", "bad ASN_OCTET_STR length\n"));
		return SNMP_ERR_WRONGLENGTH;
	}
	if( !snmp2cmm_isIpv4Valid(var_val, var_val_len) )
	{
		DEBUGMSGTL(("write_softwareUploadTFTPServerIP", "invalid ipv4 address\n"));
		return SNMP_ERR_WRONGVALUE;
	}
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( (var_val_len == strlen(softwareMgmt_softwareUploadTFTPServerIP)) &&
			(memcmp(softwareMgmt_softwareUploadTFTPServerIP, var_val, var_val_len) == 0))
		{
			return SNMP_ERR_NOERROR;
		}
		if (var_val_len != 0)
		{
			strcpy(softwareMgmt_softwareUploadTFTPServerIP, var_val);
			softwareMgmt_softwareUploadTFTPServerIP[var_val_len] = '\0';
		}
		else
		{
			softwareMgmt_softwareUploadTFTPServerIP[0] = '\0';
		}
		/* 写入数据库*/
		strValue.ci.tbl = DBS_SYS_TBL_ID_SWMGMT;
		strValue.ci.row = 1;
		strValue.ci.col = DBS_SYS_TBL_SWMGMT_COL_ID_IP;
		strValue.ci.colType = DBS_TEXT;
		strValue.len = var_val_len;
		strcpy(strValue.text, softwareMgmt_softwareUploadTFTPServerIP);
		
		ret = dbsUpdateText(dbsdev, &strValue);
		
		if( SNMP_ERR_NOERROR != ret )
		{
			DEBUGMSGTL(("write_softwareUploadTFTPServerIP", "SNMP_CMM_COM_ERROR\n"));
		}
	}
	
	return ret;
}

int write_softwareUploadTFTPServerPort(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_INTEGER_V iValue;
	
	if (var_val_type != ASN_INTEGER)
	{
		DEBUGMSGTL(("write_softwareUploadTFTPServerPort", "not ASN_INTEGER\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if( (*(long *)var_val < 1) || (*(long *)var_val > 65535) )
	{
		DEBUGMSGTL(("write_softwareUploadTFTPServerPort", "invalid port\n"));
		return SNMP_ERR_WRONGVALUE;
	}
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( softwareMgmt_softwareUploadTFTPServerPort == (*((long *) var_val) ))
		{
			return SNMP_ERR_NOERROR;
		}
		else
		{
			softwareMgmt_softwareUploadTFTPServerPort = *((long *) var_val);
			iValue.ci.tbl = DBS_SYS_TBL_ID_SWMGMT;
			iValue.ci.row = 1;
			iValue.ci.col = DBS_SYS_TBL_SWMGMT_COL_ID_PORT;
			iValue.ci.colType = DBS_INTEGER;
			iValue.len = sizeof(uint32_t);
			iValue.integer = softwareMgmt_softwareUploadTFTPServerPort;

			ret = dbsUpdateInteger(dbsdev, &iValue);
			
			if( SNMP_ERR_NOERROR != ret )
			{
				DEBUGMSGTL(("write_softwareUploadTFTPServerPort", "SNMP_CMM_COM_ERROR\n"));
			}			
		}
	}
	
	return ret;
}

int write_softwareUploadLogin(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_TEXT_V strValue;
	
	if (var_val_type != ASN_OCTET_STR)
	{
		DEBUGMSGTL(("write_softwareUploadLogin", "not ASN_OCTET_STR\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (var_val_len >= 32)
	{
		DEBUGMSGTL(("write_softwareUploadLogin", "bad ASN_OCTET_STR length\n"));
		return SNMP_ERR_WRONGLENGTH;
	}
	
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( (var_val_len == strlen(softwareMgmt_softwareUploadLogin)) &&
			(memcmp(softwareMgmt_softwareUploadLogin, var_val, var_val_len) == 0))
		{
			return SNMP_ERR_NOERROR;
		}
		if (var_val_len != 0)
		{
			strcpy(softwareMgmt_softwareUploadLogin, var_val);
			softwareMgmt_softwareUploadLogin[var_val_len] = '\0';
		}
		else
		{
			softwareMgmt_softwareUploadLogin[0] = '\0';
		}
		/* 写入数据库*/
		strValue.ci.tbl = DBS_SYS_TBL_ID_SWMGMT;
		strValue.ci.row = 1;
		strValue.ci.col = DBS_SYS_TBL_SWMGMT_COL_ID_USR;
		strValue.ci.colType = DBS_TEXT;
		strValue.len = var_val_len;
		strcpy(strValue.text, softwareMgmt_softwareUploadLogin);
		
		ret = dbsUpdateText(dbsdev, &strValue);
		
		if( SNMP_ERR_NOERROR != ret )
		{
			DEBUGMSGTL(("write_softwareUploadLogin", "SNMP_CMM_COM_ERROR\n"));
		}
	}
	
	return ret;
}

int write_softwareUploadPassword(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_TEXT_V strValue;
	
	if (var_val_type != ASN_OCTET_STR)
	{
		DEBUGMSGTL(("write_softwareUploadPassword", "not ASN_OCTET_STR\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (var_val_len >= 16)
	{
		DEBUGMSGTL(("write_softwareUploadPassword", "bad ASN_OCTET_STR length\n"));
		return SNMP_ERR_WRONGLENGTH;
	}
	
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( (var_val_len == strlen(softwareMgmt_softwareUploadPassword)) &&
			(memcmp(softwareMgmt_softwareUploadPassword, var_val, var_val_len) == 0))
		{
			return SNMP_ERR_NOERROR;
		}
		if (var_val_len != 0)
		{
			strcpy(softwareMgmt_softwareUploadPassword, var_val);
			softwareMgmt_softwareUploadPassword[var_val_len] = '\0';
		}
		else
		{
			softwareMgmt_softwareUploadPassword[0] = '\0';
		}
		/* 写入数据库*/
		strValue.ci.tbl = DBS_SYS_TBL_ID_SWMGMT;
		strValue.ci.row = 1;
		strValue.ci.col = DBS_SYS_TBL_SWMGMT_COL_ID_PWD;
		strValue.ci.colType = DBS_TEXT;
		strValue.len = var_val_len;
		strcpy(strValue.text, softwareMgmt_softwareUploadPassword);
		
		ret = dbsUpdateText(dbsdev, &strValue);
		
		if( SNMP_ERR_NOERROR != ret )
		{
			DEBUGMSGTL(("write_softwareUploadPassword", "SNMP_CMM_COM_ERROR\n"));
		}
	}
	
	return ret;
}

int write_softwareUploadFileName(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	int ret = SNMP_ERR_NOERROR;
	DB_TEXT_V strValue;
	
	if (var_val_type != ASN_OCTET_STR)
	{
		DEBUGMSGTL(("write_softwareUploadFileName", "not ASN_OCTET_STR\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if (var_val_len >= 256)
	{
		DEBUGMSGTL(("write_softwareUploadFileName", "bad ASN_OCTET_STR length\n"));
		return SNMP_ERR_WRONGLENGTH;
	}
	
	if (action == COMMIT)
	{
		/* 如果该值没有改变，则不需要写入数据库*/
		if( (var_val_len == strlen(softwareMgmt_softwareUploadFileName)) &&
			(memcmp(softwareMgmt_softwareUploadFileName, var_val, var_val_len) == 0))
		{
			return SNMP_ERR_NOERROR;
		}
		if (var_val_len != 0)
		{
			strcpy(softwareMgmt_softwareUploadFileName, var_val);
			softwareMgmt_softwareUploadFileName[var_val_len] = '\0';
		}
		else
		{
			softwareMgmt_softwareUploadFileName[0] = '\0';
		}
		/* 写入数据库*/
		strValue.ci.tbl = DBS_SYS_TBL_ID_SWMGMT;
		strValue.ci.row = 1;
		strValue.ci.col = DBS_SYS_TBL_SWMGMT_COL_ID_PATH;
		strValue.ci.colType = DBS_TEXT;
		strValue.len = var_val_len;
		strcpy(strValue.text, softwareMgmt_softwareUploadFileName);
		
		ret = dbsUpdateText(dbsdev, &strValue);
		
		if( SNMP_ERR_NOERROR != ret )
		{
			DEBUGMSGTL(("write_softwareUploadFileName", "SNMP_CMM_COM_ERROR\n"));
		}
	}
	
	return ret;
}

int write_softwareUploadProceed(int action,
                          u_char * var_val,
                          u_char var_val_type,
                          size_t var_val_len,
                          u_char * statP, oid * name, size_t name_len)
{
	if (var_val_type != ASN_INTEGER)
	{
		DEBUGMSGTL(("write_softwareUploadProceed", "not ASN_INTEGER\n"));
		return SNMP_ERR_WRONGTYPE;
	}
	if( (1 != *(long *)var_val) && (2 != *(long *)var_val) )
	{
		DEBUGMSGTL(("write_softwareUploadProceed", "not ASN_BOOLEAN\n"));
		return SNMP_ERR_WRONGVALUE;
	}
	if (action == COMMIT)
	{
		if( 1 == (*((long *) var_val) ))
		{
			/* 置开始升级标志位*/
			/* 升级的逻辑有待完善*/
			if( 0 != snmp2cmm_uploadSoftware() )
			{
				return SNMP_ERR_GENERR;
			}
			/*
			else if( 0 != snmp2cmm_resetMp() )
			{
				return SNMP_ERR_GENERR;
			}
			*/
		}
	}	
	return SNMP_ERR_NOERROR;
}

int load_softwareMgmt(void)
{
	st_dbsSwmgmt row;
	st_dbsSysinfo sysInfo;
	
	/* load data from cmm *//* ??? */
	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &sysInfo) )
	{
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "snmp load_softwareMgmt failed");
	}
	
	//printf("-->load_softwareMgmt\n");
	if( CMM_SUCCESS != dbsGetSwmgmt(dbsdev, 1, &row) )
	{
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "snmp load_softwareMgmt failed");
	}
	else
	{
		strcpy(softwareMgmt_currentSoftwareVersion, sysInfo.col_appver);
		strcpy(softwareMgmt_softwareUploadTFTPServerIP, row.col_ip);
		softwareMgmt_softwareUploadTFTPServerPort = row.col_port;
		strcpy(softwareMgmt_softwareUploadLogin, row.col_user);
		strcpy(softwareMgmt_softwareUploadPassword, row.col_pwd);
		strcpy(softwareMgmt_softwareUploadFileName, row.col_path);
	}

	/* SYNTAX TruthValue */
	softwareMgmt_softwareUploadProceed = 2;
	softwareMgmt_softwareUploadResult = 0;

	return 0;
}

unsigned char *var_softwareMgmt(struct variable *vp,
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
	else if( load_softwareMgmt() != 0)
	{
		return NULL;
	}
	
	*var_len = sizeof (uint8_t); 
       *write_method = NULL;
	
	switch(vp->magic)
	{
		case OID_SNMP_SW_MGMT_CUR_VER:			
			*var_len = strlen(softwareMgmt_currentSoftwareVersion);
			return softwareMgmt_currentSoftwareVersion; 
			break;
		case OID_SNMP_SW_MGMT_SERV_IP:
			*write_method = write_softwareUploadTFTPServerIP;
			*var_len = strlen(softwareMgmt_softwareUploadTFTPServerIP);
			return softwareMgmt_softwareUploadTFTPServerIP; 
			break;
		case OID_SNMP_SW_MGMT_SERV_PORT:
			*write_method = write_softwareUploadTFTPServerPort;
			*var_len = sizeof(softwareMgmt_softwareUploadTFTPServerPort);
			return (unsigned char *) &softwareMgmt_softwareUploadTFTPServerPort; 
			break;
		case OID_SNMP_SW_MGMT_SERV_LOGIN:
			*write_method = write_softwareUploadLogin;
			*var_len = strlen(softwareMgmt_softwareUploadLogin);
			return softwareMgmt_softwareUploadLogin; 
			break;
		case OID_SNMP_SW_MGMT_SERV_PWD:
			*write_method = write_softwareUploadPassword;
			*var_len = strlen(softwareMgmt_softwareUploadPassword);
			return softwareMgmt_softwareUploadPassword; 
			break;
		case OID_SNMP_SW_MGMT_SERV_FILE:
			*write_method = write_softwareUploadFileName;
			*var_len = strlen(softwareMgmt_softwareUploadFileName);
			return softwareMgmt_softwareUploadFileName; 
			break;
		case OID_SNMP_SW_MGMT_UPLOAD_ACTION:
			*write_method = write_softwareUploadProceed;
			*var_len = sizeof(softwareMgmt_softwareUploadProceed);
			return (unsigned char *) &softwareMgmt_softwareUploadProceed; 
			break;
		case OID_SNMP_SW_MGMT_UPLOAD_RESULT:
			*var_len = sizeof(softwareMgmt_softwareUploadResult);
			return (unsigned char *) &softwareMgmt_softwareUploadResult; 
			break;
		default:
			break;
	}
	return 0;
	
}

void init_softwareMgmt(void)
{
	REGISTER_MIB("softwareMgmt/softwareMgmt", softwareMgmt_variables,  variable1, softwareMgmt_variables_oid);
}

