#include <stdio.h>
#include <string.h>
#include <dbsapi.h>
#include "http2dbs.h"
#include <boardapi.h>

/* 与DBS  通讯的设备文件*/
T_DBS_DEV_INFO *dbsdev = NULL;

int http2dbs_getCnuIndexByMacaddress(char *mac, stCnuNode *index)
{
	return dbsSelectCnuIndexByMacAddress(dbsdev, mac, index);
}

int http2dbs_getProfile(uint16_t id, st_dbsProfile * profile)
{
	return dbsGetProfile(dbsdev, id, profile);
}

int http2dbs_setProfile(uint16_t id, st_dbsProfile * profile)
{
	return dbsUpdateProfile(dbsdev, id, profile);
}


int http2dbs_getCnu(uint16_t id, st_dbsCnu * cnu)
{
	return dbsGetCnu(dbsdev, id, cnu);
}

int http2dbs_setCnu(uint16_t id, st_dbsCnu * cnu)
{
	return dbsUpdateCnu(dbsdev, id, cnu);
}

int http2dbs_doCltAgTimeSettings(PWEB_NTWK_VAR pWebVar)
{
	int flag = 0;
	st_dbsCltConf row;

	if( CMM_SUCCESS != dbsGetCltconf(dbsdev, pWebVar->cltid, &row) )
	{
		return CMM_FAILED;
	}

	if( row.col_loagTime != pWebVar->col_loagTime )
	{
		row.col_loagTime = pWebVar->col_loagTime;
		flag++;
	}
	if( row.col_reagTime != pWebVar->col_reagTime )
	{
		row.col_reagTime = pWebVar->col_reagTime;
		flag++;
	}
	
	if( flag )
	{
		return dbsUpdateCltconf(dbsdev, pWebVar->cltid, &row);
	}
	else
	{
		return CMM_SUCCESS;
	}
}

int http2dbs_saveTemplate(PWEB_NTWK_VAR pWebVar)
{
	int flag = 0;
	st_dbsTemplate row;

	if( CMM_SUCCESS != dbsGetTemplate(dbsdev, pWebVar->col_curTemp, &row) )
	{
	        printf("Error pWebVar->col_curTemp  =%d \n", pWebVar->col_curTemp);
		return CMM_FAILED;
	}

	row.col_tempAutoSts = pWebVar->col_tempAutoSts;
	row.col_curTemp = pWebVar->col_curTemp;
	row.col_eth1VlanAddSts = pWebVar->col_eth1VlanAddSts;
	row.col_eth1VlanStart = pWebVar->col_eth1VlanStart;
	row.col_eth1VlanStop = 0;
	row.col_eth2VlanAddSts = pWebVar->col_eth2VlanAddSts;
	row.col_eth2VlanStart = pWebVar->col_eth2VlanStart;
	row.col_eth2VlanStop = 0;
	row.col_eth3VlanAddSts = pWebVar->col_eth3VlanAddSts;
	row.col_eth3VlanStart = pWebVar->col_eth3VlanStart;
	row.col_eth3VlanStop = 0;
	row.col_eth4VlanAddSts = pWebVar->col_eth4VlanAddSts;
	row.col_eth4VlanStart = pWebVar->col_eth4VlanStart;
	row.col_eth4VlanStop = 0;
	

	return dbsUpdateTemplate(dbsdev, pWebVar->col_curTemp, &row);
	
}


int http2dbs_doCltDecapSettings(PWEB_NTWK_VAR pWebVar)
{
	int flag = 0;
	st_dbsCltConf row;

	if( CMM_SUCCESS != dbsGetCltconf(dbsdev, pWebVar->cltid, &row) )
	{
		return CMM_FAILED;
	}

	if( row.col_igmpPri != pWebVar->col_igmpPri )
	{
		row.col_igmpPri = pWebVar->col_igmpPri;
		flag++;
	}
	if( row.col_unicastPri != pWebVar->col_unicastPri )
	{
		row.col_unicastPri = pWebVar->col_unicastPri;
		flag++;
	}
	if( row.col_avsPri != pWebVar->col_avsPri )
	{
		row.col_avsPri = pWebVar->col_avsPri;
		flag++;
	}
	if( row.col_mcastPri != pWebVar->col_mcastPri )
	{
		row.col_mcastPri = pWebVar->col_mcastPri;
		flag++;
	}
	
	if( flag )
	{
		return dbsUpdateCltconf(dbsdev, pWebVar->cltid, &row);
	}
	else
	{
		return CMM_SUCCESS;
	}
}

int http2dbs_doCltQosEnable(PWEB_NTWK_VAR pWebVar)
{
	st_dbsCltConf row;

	if( CMM_SUCCESS != dbsGetCltconf(dbsdev, pWebVar->cltid, &row) )
	{
		return CMM_FAILED;
	}

	if( row.col_tbaPriSts != pWebVar->col_tbaPriSts )
	{
		row.col_tbaPriSts = pWebVar->col_tbaPriSts;
		return dbsUpdateCltconf(dbsdev, pWebVar->cltid, &row);
	}

	return CMM_SUCCESS;
}




int http2dbs_doCltQosSettings(PWEB_NTWK_VAR pWebVar)
{
	//int flag = 0;
	st_dbsCltConf row;

	if( (1==pWebVar->col_cosPriSts)&&(1==pWebVar->col_tosPriSts) )
	{
		return CMM_FAILED;
	}

	if( CMM_SUCCESS != dbsGetCltconf(dbsdev, pWebVar->cltid, &row) )
	{
		return CMM_FAILED;
	}

	if( 1 == pWebVar->col_cosPriSts )
	{
		//row.col_tbaPriSts = 1;		
		row.col_cosPriSts = 1;
		row.col_cos0pri = pWebVar->col_cos0pri;
		row.col_cos1pri = pWebVar->col_cos1pri;
		row.col_cos2pri = pWebVar->col_cos2pri;
		row.col_cos3pri = pWebVar->col_cos3pri;
		row.col_cos4pri = pWebVar->col_cos4pri;
		row.col_cos5pri = pWebVar->col_cos5pri;
		row.col_cos6pri = pWebVar->col_cos6pri;
		row.col_cos7pri = pWebVar->col_cos7pri;
		row.col_tosPriSts = 0;
		row.col_tos0pri = 1;
		row.col_tos1pri = 0;
		row.col_tos2pri = 0;
		row.col_tos3pri = 1;
		row.col_tos4pri = 2;
		row.col_tos5pri = 2;
		row.col_tos6pri = 3;
		row.col_tos7pri = 3;
	}
	else if( 1 == pWebVar->col_tosPriSts )
	{
		//row.col_tbaPriSts = 1;
		row.col_cosPriSts = 0;
		row.col_cos0pri = 1;
		row.col_cos1pri = 0;
		row.col_cos2pri = 0;
		row.col_cos3pri = 1;
		row.col_cos4pri = 2;
		row.col_cos5pri = 2;
		row.col_cos6pri = 3;
		row.col_cos7pri = 3;		
		row.col_tosPriSts = 1;
		row.col_tos0pri = pWebVar->col_tos0pri;
		row.col_tos1pri = pWebVar->col_tos1pri;
		row.col_tos2pri = pWebVar->col_tos2pri;
		row.col_tos3pri = pWebVar->col_tos3pri;
		row.col_tos4pri = pWebVar->col_tos4pri;
		row.col_tos5pri = pWebVar->col_tos5pri;
		row.col_tos6pri = pWebVar->col_tos6pri;
		row.col_tos7pri = pWebVar->col_tos7pri;			
	}
	else
	{
		//row.col_tbaPriSts = 0;		
		row.col_cosPriSts = 1;
		row.col_cos0pri = 1;
		row.col_cos1pri = 0;
		row.col_cos2pri = 0;
		row.col_cos3pri = 1;
		row.col_cos4pri = 2;
		row.col_cos5pri = 2;
		row.col_cos6pri = 3;
		row.col_cos7pri = 3;

		row.col_tosPriSts = 0;
		row.col_tos0pri = 1;
		row.col_tos1pri = 0;
		row.col_tos2pri = 0;
		row.col_tos3pri = 1;
		row.col_tos4pri = 2;
		row.col_tos5pri = 2;
		row.col_tos6pri = 3;
		row.col_tos7pri = 3;
	}

	return dbsUpdateCltconf(dbsdev, pWebVar->cltid, &row);
}

int http2dbs_getWebAdminPwd(char *varValue)
{
	st_dbsCliRole row;

	if( CMM_SUCCESS != dbsGetCliRole(dbsdev, 4, &row) )
	{
		sprintf(varValue, "%s", "admin");
	}
	else
	{
		sprintf(varValue, "%s", row.col_pwd);		
	}
	return CMM_SUCCESS;
}

int http2dbs_setWebAdminPwd(PWEB_NTWK_VAR pWebVar)
{
	st_dbsCliRole row;

	row.id = 4;
	strcpy((char *)row.col_user, "admin");
	strcpy((char *)row.col_pwd, (const char *)(pWebVar->sysPassword));
	return dbsUpdateCliRole(dbsdev, 4, &row);
}

int http2dbs_getDevSerials(char *varValue)
{
	strcpy(varValue, "WECxx EoC CBAT");
	return CMM_SUCCESS;
}

int http2dbs_getDevModel(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}

	strcpy(varValue, boardapi_getDeviceModelStr(row.col_model));
	
	return CMM_SUCCESS;
}

int http2dbs_getEocType(char *varValue)
{
	strcpy(varValue, boardapi_getCltStandardStr());
	return CMM_SUCCESS;
}

int http2dbs_getCltNumber(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_maxclt);
		return CMM_SUCCESS;
	}
}

int http2dbs_getCnuStations(char *varValue)
{
	sprintf(varValue, "%d", MAX_CNUS_PER_CLT);
	return 0;
}

int http2dbs_getWlistStatus(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_wlctl);
		return CMM_SUCCESS;
	}
}

int http2dbs_getWdtStatus(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_wdt);
		return CMM_SUCCESS;
	}
}

int http2dbs_getFlashSize(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_flashsize);
		return CMM_SUCCESS;
	}
}

int http2dbs_getSdramSize(char *varValue)
{	
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_ramsize);
		return CMM_SUCCESS;
	}
}

int http2dbs_getHwVersion(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_hwver);
		return CMM_SUCCESS;
	}
}

int http2dbs_getBootVersion(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_bver);
		return CMM_SUCCESS;
	}
}

int http2dbs_getKernelVersion(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_kver);
		return CMM_SUCCESS;
	}
}

int http2dbs_getAppVersion(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_appver);
		return CMM_SUCCESS;
	}
}

int http2dbs_getAppHash(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_apphash);
		return CMM_SUCCESS;
	}
}

int http2dbs_getManufactory(char *varValue)
{
	st_dbsSysinfo row;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_mfinfo);
		return CMM_SUCCESS;
	}
}

int http2dbs_getWecIpaddr(char *varValue)
{
	st_dbsNetwork row;

	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_ip);
		return CMM_SUCCESS;
	}
}

int http2dbs_getWecNetmask(char *varValue)
{
	st_dbsNetwork row;

	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_netmask);
		return CMM_SUCCESS;
	}
}

int http2dbs_getWecDefaultGw(char *varValue)
{	
	st_dbsNetwork row;

	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_gw);
		return CMM_SUCCESS;
	}
}

int http2dbs_getWecMgmtVlanSts(char *varValue)
{
	st_dbsNetwork row;

	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_mvlan_sts);
		return CMM_SUCCESS;
	}
}

int http2dbs_getWecMgmtVlanId(char *varValue)
{
	st_dbsNetwork row;

	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_mvlan_id);
		return CMM_SUCCESS;
	}
}

int http2dbs_getSnmpRoCommunity(char *varValue)
{
	st_dbsSnmp row;

	if( CMM_SUCCESS != dbsGetSnmp(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_rdcom);
		return CMM_SUCCESS;
	}
}

int http2dbs_getSnmpRwCommunity(char *varValue)
{
	st_dbsSnmp row;

	if( CMM_SUCCESS != dbsGetSnmp(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_wrcom);
		return CMM_SUCCESS;
	}
}

int http2dbs_getSnmpTrapIpaddr(char *varValue)
{
	st_dbsSnmp row;

	if( CMM_SUCCESS != dbsGetSnmp(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		strcpy(varValue, (const char *)row.col_sina);
		return CMM_SUCCESS;
	}
}

int http2dbs_getSnmpTrapDport(char *varValue)
{
	st_dbsSnmp row;

	if( CMM_SUCCESS != dbsGetSnmp(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	else
	{
		sprintf(varValue, "%d", row.col_tpa);
		return CMM_SUCCESS;
	}
}

int http2dbs_setWecNetworkConfig(PWEB_NTWK_VAR pWebVar)
{
	int flag = 0;
	st_dbsNetwork row;

	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}

	if( strcmp((const char *)row.col_ip, (const char *)pWebVar->wecIpaddr) != 0 )
	{
		strcpy((char *)row.col_ip, (const char *)pWebVar->wecIpaddr);
		flag++;
	}
	if( strcmp((const char *)row.col_netmask, (const char *)pWebVar->wecNetmask) != 0 )
	{
		strcpy((char *)row.col_netmask, (const char *)pWebVar->wecNetmask);
		flag++;
	}
	if( strcmp((const char *)row.col_gw, (const char *)pWebVar->wecDefaultGw) != 0 )
	{
		strcpy((char *)row.col_gw, (const char *)pWebVar->wecDefaultGw);
		flag++;
	}
	if( row.col_mvlan_sts != pWebVar->wecMgmtVlanSts )
	{
		row.col_mvlan_sts = pWebVar->wecMgmtVlanSts;
		flag++;
	}
	if( row.col_mvlan_id != pWebVar->wecMgmtVlanId )
	{
		row.col_mvlan_id = pWebVar->wecMgmtVlanId;
		flag++;
	}
	if( flag )
	{
		return dbsUpdateNetwork(dbsdev, 1, &row);
	}
	else
	{
		return CMM_SUCCESS;
	}
}

int http2dbs_setSnmpConfig(PWEB_NTWK_VAR pWebVar)
{
	int flag = 0;
	st_dbsSnmp row;

	if( CMM_SUCCESS != dbsGetSnmp(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}
	
	if( strcmp((const char *)row.col_rdcom, (const char *)pWebVar->snmpRoCommunity) != 0 )
	{
		strcpy((char *)row.col_rdcom, (const char *)pWebVar->snmpRoCommunity);
		flag++;
	}
	if( strcmp((const char *)row.col_wrcom, (const char *)pWebVar->snmpRwCommunity) != 0 )
	{
		strcpy((char *)row.col_wrcom, (const char *)pWebVar->snmpRwCommunity);
		flag++;
	}
	if( strcmp((const char *)row.col_sina, (const char *)pWebVar->snmpTrapIpaddr) != 0 )
	{
		strcpy((char *)row.col_sina, (const char *)pWebVar->snmpTrapIpaddr);
		flag++;
	}
	if( row.col_tpa != pWebVar->snmpTrapDport )
	{
		row.col_tpa = pWebVar->snmpTrapDport;
		flag++;
	}
	if( flag )
	{
		return dbsUpdateSnmp(dbsdev, 1, &row);
	}
	else
	{
		return CMM_SUCCESS;
	}
}

int http2dbs_setWebAdminPasswd(char *varValue)
{
	return CMM_FAILED;
}

int http2dbs_setCliAdminPasswd(char *varValue)
{
	st_dbsCliRole row;

	row.id = 1;
	strcpy((char *)row.col_user, "admin");
	strcpy((char *)row.col_pwd, (const char *)varValue);
	return dbsUpdateCliRole(dbsdev, 1, &row);
}

int http2dbs_setCliOptPasswd(char *varValue)
{
	st_dbsCliRole row;

	row.id = 2;
	strcpy((char *)row.col_user, "operator");
	strcpy((char *)row.col_pwd, (const char *)varValue);
	return dbsUpdateCliRole(dbsdev, 2, &row);
}

int http2dbs_setCliUserPasswd(char *varValue)
{
	st_dbsCliRole row;

	row.id = 3;
	strcpy((char *)row.col_user, "user");
	strcpy((char *)row.col_pwd, (const char *)varValue);
	return dbsUpdateCliRole(dbsdev, 3, &row);
}

int http2dbs_getFtpIpaddr(char *varValue)
{
	st_dbsSwmgmt row;

	if( 0 != dbsGetSwmgmt(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}	
	strcpy(varValue, (const char *)row.col_ip);
	return CMM_SUCCESS;
}

int http2dbs_getFtpPort(char *varValue)
{
	st_dbsSwmgmt row;

	if( 0 != dbsGetSwmgmt(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}	
	sprintf(varValue, "%d", row.col_port);
	return CMM_SUCCESS;
}

int http2dbs_getFtpUser(char *varValue)
{
	st_dbsSwmgmt row;

	if( 0 != dbsGetSwmgmt(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}	
	strcpy(varValue, (const char *)row.col_user);
	return CMM_SUCCESS;
}

int http2dbs_getFtpPasswd(char *varValue)
{
	st_dbsSwmgmt row;

	if( 0 != dbsGetSwmgmt(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}	
	strcpy(varValue, (const char *)row.col_pwd);
	return CMM_SUCCESS;
}

int http2dbs_getFtpFilePath(char *varValue)
{
	st_dbsSwmgmt row;

	if( 0 != dbsGetSwmgmt(dbsdev, 1, &row) )
	{
		return CMM_FAILED;
	}	
	strcpy(varValue, (const char *)row.col_path);
	return CMM_SUCCESS;
}

int http2dbs_getOptlog(int id, st_dbsOptlog *plog)
{
	return dbsGetOptlog(dbsdev, id, plog);
}

int http2dbs_writeOptlog(int status, char *msg)
{
	//int i = 0;
	time_t b_time;
	time(&b_time);
	st_dbsOptlog log;

	log.time = b_time;
	log.who = MID_HTTP;		/* MID_HTTP */
	strcpy((char *)log.cmd, (const char *)msg);
	log.level = DBS_LOG_INFO;	/* DBS_LOG_INFO */
	log.result = status?CMM_FAILED:CMM_SUCCESS;

	return dbs_opt_log(dbsdev, &log);
}

int http2dbs_getSyslog(uint32_t row, st_dbsSyslog *log)
{
	return dbsGetSyslog(dbsdev, row, log);
}

int http2dbs_writeSyslog(uint32_t priority, const char *log)
{
	return dbs_sys_log(dbsdev, priority, log);
}

int http2dbs_getAlarmlog(uint32_t row, st_dbsAlarmlog *log)
{
	return dbsGetAlarmlog(dbsdev, row, log);
}

int http2dbs_writeAlarmlog(st_dbsAlarmlog *log)
{
	return dbs_alarm_log(dbsdev, log);
}

int http2dbs_getSwmgmt(int id, st_dbsSwmgmt *pRow)
{
	return dbsGetSwmgmt(dbsdev, id, pRow);
}

int http2dbs_saveConfig(void)
{
	return dbsFflush(dbsdev);
}

int http2dbs_destroy(void)
{
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "module httpd exit");
	dbsClose(dbsdev);
	return CMM_SUCCESS;
}

int http2dbs_init(void)
{
	dbsdev = dbsOpen(MID_HTTP);
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: httpd->dbsOpen error, exited !\n");
		return CMM_FAILED;
	}
	else
	{
		dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting module httpd success");
		printf("Starting module httpd		......		[OK]\n");
	}
	return CMM_SUCCESS;
} 

