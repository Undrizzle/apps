#ifndef __REGISTER_DB_MSG_H__
#define __REGISTER_DB_MSG_H__

#include <public.h>

extern T_DBS_DEV_INFO *dbsdev;

T_DBS_DEV_INFO * reg_dbsOpen(void);
int reg_dbsClose(void);
int db_init_clt(int cltid);
int db_init_cnu(int cltid, int cnuid);
int db_init_all(void);
int db_get_clt(int cltid, st_dbsClt *clt);
int db_get_cnu(int cltid, int cnuid, st_dbsCnu *cnu);
int db_update_clt(int cltid, st_dbsClt *clt);
int db_update_cnu(int cltid, int cnuid, st_dbsCnu *cnu);
int db_unregister_clt(int cltid);
int db_unregister_cnu(int cltid, int cnuid);
int db_delete_cnu(int cltid, int cnuid);
int db_new_cnu(int cltid, int cnuid, st_dbsCnu *cnu);
int db_new_su(int cltid, int cnuid, st_dbsCnu *cnu);
int db_get_user_type(uint32_t cltid, uint32_t cnuid, uint32_t *userType);
int db_get_user_onused(uint32_t cltid, uint32_t cnuid, uint32_t *onUsed);
int db_get_anonymous_access_sts(uint32_t *anonyAccSts);
int db_get_user_access_sts(uint32_t cltid, uint32_t cnuid, uint32_t *userAccSts);
int db_get_template_auto_config_sts(uint32_t *autoCfgSts);
int db_get_auto_config_sts(uint32_t *autoCfgSts);
int db_get_user_auto_config_sts(uint32_t cltid, uint32_t cnuid, uint32_t *userAutoCfgSts);
int db_init_nelib(T_TOPOLOGY_INFO *this);
int db_fflush(void);
int db_real_fflush(void);

#endif

