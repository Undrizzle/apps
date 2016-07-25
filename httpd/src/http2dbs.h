#ifndef __HTTP_DBS_H__
#define __HTTP_DBS_H__

#include <stdio.h>
#include <string.h>
#include <public.h>
#include "cgimain.h"

extern T_DBS_DEV_INFO *dbsdev;

int http2dbs_getCnuIndexByMacaddress(char *mac, stCnuNode *index);
int http2dbs_getProfile(uint16_t id, st_dbsProfile * profile);
int http2dbs_setProfile(uint16_t id, st_dbsProfile * profile);
int http2dbs_getCnu(uint16_t id, st_dbsCnu * cnu);
int http2dbs_setCnu(uint16_t id, st_dbsCnu * cnu);

int http2dbs_saveTemplate(PWEB_NTWK_VAR pWebVar);

int http2dbs_doCltAgTimeSettings(PWEB_NTWK_VAR pWebVar);
int http2dbs_doCltDecapSettings(PWEB_NTWK_VAR pWebVar);
int http2dbs_doCltQosEnable(PWEB_NTWK_VAR pWebVar);
int http2dbs_doCltQosSettings(PWEB_NTWK_VAR pWebVar);

int http2dbs_getWebAdminPwd(char *varValue);
int http2dbs_setWebAdminPwd(PWEB_NTWK_VAR pWebVar);

int http2dbs_getWecIpaddr(char *varValue);
int http2dbs_getWecNetmask(char *varValue);
int http2dbs_getWecDefaultGw(char *varValue);
int http2dbs_getWecMgmtVlanSts(char *varValue);
int http2dbs_getWecMgmtVlanId(char *varValue);
int http2dbs_getSnmpRoCommunity(char *varValue);
int http2dbs_getSnmpRwCommunity(char *varValue);
int http2dbs_getSnmpTrapIpaddr(char *varValue);
int http2dbs_getSnmpTrapDport(char *varValue);

int http2dbs_getDevSerials(char *varValue);
int http2dbs_getEocType(char *varValue);
int http2dbs_getCltNumber(char *varValue);
int http2dbs_getCnuStations(char *varValue);
int http2dbs_getWlistStatus(char *varValue);
int http2dbs_getWdtStatus(char *varValue);
int http2dbs_getFlashSize(char *varValue);
int http2dbs_getSdramSize(char *varValue);
int http2dbs_getDevModel(char *varValue);
int http2dbs_getHwVersion(char *varValue);
int http2dbs_getBootVersion(char *varValue);
int http2dbs_getKernelVersion(char *varValue);
int http2dbs_getAppVersion(char *varValue);
int http2dbs_getAppHash(char *varValue);
int http2dbs_getManufactory(char *varValue);
int http2dbs_getFtpIpaddr(char *varValue);
int http2dbs_getFtpPort(char *varValue);
int http2dbs_getFtpUser(char *varValue);
int http2dbs_getFtpPasswd(char *varValue);
int http2dbs_getFtpFilePath(char *varValue);

#if 0
int http2dbs_setWecIpaddr(char *varValue);
int http2dbs_setWecNetmask(char *varValue);
int http2dbs_setWecDefaultGw(char *varValue);
int http2dbs_setWecMgmtVlanSts(char *varValue);
int http2dbs_setWecMgmtVlanId(char *varValue);



#endif
int http2dbs_setWebAdminPasswd(char *varValue);
int http2dbs_setCliAdminPasswd(char *varValue);
int http2dbs_setCliOptPasswd(char *varValue);
int http2dbs_setCliUserPasswd(char *varValue);

int http2dbs_setWecNetworkConfig(PWEB_NTWK_VAR pWebVar);
int http2dbs_setSnmpConfig(PWEB_NTWK_VAR pWebVar);
int http2dbs_getOptlog(int id, st_dbsOptlog *plog);
int http2dbs_writeOptlog(int status, char *msg);

int http2dbs_getSyslog(uint32_t row, st_dbsSyslog *log);
int http2dbs_writeSyslog(uint32_t priority, const char *log);
int http2dbs_getAlarmlog(uint32_t row, st_dbsAlarmlog *log);
int http2dbs_writeAlarmlog(st_dbsAlarmlog *log);
int http2dbs_getSwmgmt(int id, st_dbsSwmgmt *pRow);
int http2dbs_saveConfig(void);

int http2dbs_destroy(void);
int http2dbs_init(void);

#endif 

