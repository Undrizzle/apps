#ifndef __DBS_API_H__
#define __DBS_API_H__

#include <public.h>

int dbsSelectCnuIndexByMacAddress(T_DBS_DEV_INFO *dev, char *mac, stCnuNode *index);
int dbsGetInteger(T_DBS_DEV_INFO *dev, DB_INTEGER_V *v);
int dbsGetText(T_DBS_DEV_INFO *dev, DB_TEXT_V *v);
int dbsGetCliRole(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCliRole *row);
int dbsGetClt(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsClt *row);
int dbsGetCltconf(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCltConf *row);
int dbsGetCnu(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCnu *row);
int dbsGetDepro(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCnuDefaultProfile *row);
int dbsGetTemplate(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsTemplate  *row);
int dbsGetNetwork(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsNetwork *row);
int dbsGetProfile(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsProfile *row);
int dbsGetSnmp(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsSnmp *row);
int dbsGetSwmgmt(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsSwmgmt *row);
int dbsGetSysinfo(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsSysinfo *row);
int dbsUpdateInteger(T_DBS_DEV_INFO *dev, DB_INTEGER_V *v);
int dbsUpdateText(T_DBS_DEV_INFO *dev, DB_TEXT_V *v);
int dbsUpdateCliRole(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCliRole *row);
int dbsUpdateClt(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsClt *row);
int dbsUpdateCltconf(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCltConf *row);
int dbsUpdateCnu(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCnu *row);
int dbsUpdateDepro(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsCnuDefaultProfile *row);
int dbsUpdateNetwork(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsNetwork *row);
int dbsUpdateProfile(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsProfile *row);
int dbsUpdateSnmp(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsSnmp *row);
int dbsUpdateSwmgmt(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsSwmgmt *row);
int dbsUpdateSysinfo(T_DBS_DEV_INFO *dev, uint16_t id, st_dbsSysinfo *row);
int dbsDestroyRowClt(T_DBS_DEV_INFO *dev, uint16_t id);
int dbsDestroyRowCltconf(T_DBS_DEV_INFO *dev, uint16_t id);
int dbsDestroyRowCnu(T_DBS_DEV_INFO *dev, uint16_t id);
int dbsDestroyRowProfile(T_DBS_DEV_INFO *dev, uint16_t id);
int dbsCreateSuProfileForCnu(T_DBS_DEV_INFO *dev, uint16_t id);
int dbsCreateDewlProfileForCnu(T_DBS_DEV_INFO *dev, uint16_t id);
int dbsCreateDeblProfileForCnu(T_DBS_DEV_INFO *dev, uint16_t id);
int dbsCreateSuProfileForWec701Cnu(T_DBS_DEV_INFO *dev, uint16_t id);
int dbsCreateDewlProfileForWec701Cnu(T_DBS_DEV_INFO *dev, uint16_t id);
int dbsCreateDeblProfileForWec701Cnu(T_DBS_DEV_INFO *dev, uint16_t id);
int dbsSetDsdtRgmiiDelay(T_DBS_DEV_INFO *dev, st_dsdtRgmiiTimingDelay *dsdtRgmiiTimingDelay);
int dbsLogCount(T_DBS_DEV_INFO *dev, uint16_t tbl, uint32_t *n);
int dbsGetSyslog(T_DBS_DEV_INFO *dev, uint32_t row, st_dbsSyslog *log);
int dbsGetOptlog(T_DBS_DEV_INFO *dev, uint32_t row, st_dbsOptlog *log);
int dbsGetAlarmlog(T_DBS_DEV_INFO *dev, uint32_t row, st_dbsAlarmlog *log);
int dbs_sys_log(T_DBS_DEV_INFO *dev, uint32_t priority, const char *message);
int dbs_opt_log(T_DBS_DEV_INFO *dev, st_dbsOptlog *log);
int dbs_alarm_log(T_DBS_DEV_INFO *dev, st_dbsAlarmlog *log);
int dbsRegisterModuleById(T_DBS_DEV_INFO *dev, uint16_t mid);
int dbsDestroyModuleById(T_DBS_DEV_INFO *dev, uint16_t mid);
void dbsWaitModule(T_DBS_DEV_INFO *dev, uint32_t MF);
BOOLEAN dbsGetModuleStatus(T_DBS_DEV_INFO *dev, uint16_t mid);
int dbsMsgDebug(T_DBS_DEV_INFO *dev, uint32_t status);
int dbsSQLDebug(T_DBS_DEV_INFO *dev, uint32_t status);
int dbsFflush(T_DBS_DEV_INFO *dev);
T_DBS_DEV_INFO * dbsNoWaitOpen(uint16_t mid);
T_DBS_DEV_INFO * dbsOpen(uint16_t mid);
int dbsClose(T_DBS_DEV_INFO *dev);

#endif

