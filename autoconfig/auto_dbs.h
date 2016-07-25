#ifndef __AUTOCONFIG_DB_MSG_H__
#define __AUTOCONFIG_DB_MSG_H__

#include <public.h>

extern T_DBS_DEV_INFO *dbsdev;

T_DBS_DEV_INFO * auto_dbsOpen(void);
int auto_dbsClose(void);

#endif