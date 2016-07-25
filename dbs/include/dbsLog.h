#ifndef __DBS_LOG_H__
#define __DBS_LOG_H__

#include <dbs.h>

int dbs_count_alarmlog(uint32_t *n);
int dbs_count_optlog(uint32_t *n);
int dbs_count_syslog(uint32_t *n);

int dbs_get_alarmlog(uint32_t row, st_dbsAlarmlog *v);
int dbs_get_optlog(uint32_t row, st_dbsOptlog *v);
int dbs_get_syslog(uint32_t row, st_dbsSyslog *v);
int dbs_get_mslog(st_dbsMslog *ms);

int dbs_write_alarmlog(st_dbsAlarmlog *v);
int dbs_write_optlog(st_dbsOptlog *v);
int dbs_write_syslog(st_dbsSyslog *v);
int dbs_write_mslog(st_dbsMslog *ms);

int dbs_log_debug_enable(int enable);
int dbs_log_open(void);
int dbs_log_close(void);

#endif

