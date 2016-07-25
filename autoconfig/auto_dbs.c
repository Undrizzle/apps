#include <dbsapi.h>
#include <boardapi.h>
#include "auto_dbs.h"
#include "autoconfig.h"

static uint8_t save_flag = 0;

/* ÓëDBS  Í¨Ñ¶µÄÉè±¸ÎÄ¼þ*/
T_DBS_DEV_INFO *dbsdev = NULL;

T_DBS_DEV_INFO * auto_dbsOpen(void)
{
	return dbsOpen(MID_AUTOCONFIG);
}

int auto_dbsClose(void)
{
	return dbsClose(dbsdev);
}
