#ifndef __DBS_UNDERLAYER_H__
#define __DBS_UNDERLAYER_H__

#include <dbs.h>


int dbs_underlayer_select_cnu_index_by_mac(char *mac, stCnuNode *index);
int dbs_underlayer_get_column(DB_COL_VAR *v);
int dbs_underlayer_update_column(DB_COL_VAR *v);
int dbs_underlayer_get_row_clirole(st_dbsCliRole *row);
int dbs_underlayer_get_row_clt(st_dbsClt *row);
int dbs_underlayer_get_row_cltconfig(st_dbsCltConf *row);
int dbs_underlayer_get_row_cnu(st_dbsCnu *row);
int dbs_underlayer_get_row_depro(st_dbsCnuDefaultProfile *row);
int dbs_underlayer_get_row_network(st_dbsNetwork *row);
int dbs_underlayer_get_row_profile(st_dbsProfile *row);
int dbs_underlayer_get_row_snmp(st_dbsSnmp *row);
int dbs_underlayer_get_row_swmgmt(st_dbsSwmgmt *row);
int dbs_underlayer_get_row_sysinfo(st_dbsSysinfo *row);
int dbs_underlayer_get_row_template(st_dbsTemplate *row);
int dbs_underlayer_update_row_clirole(st_dbsCliRole *row);
int dbs_underlayer_update_row_clt(st_dbsClt *row);
int dbs_underlayer_update_row_cltconfig(st_dbsCltConf *row);
int dbs_underlayer_update_row_cnu(st_dbsCnu *row);
int dbs_underlayer_update_row_depro(st_dbsCnuDefaultProfile *row);
int dbs_underlayer_update_row_network(st_dbsNetwork *row);
int dbs_underlayer_update_row_profile(st_dbsProfile *row);
int dbs_underlayer_update_row_snmp(st_dbsSnmp *row);
int dbs_underlayer_update_row_swmgmt(st_dbsSwmgmt *row);
int dbs_underlayer_update_row_sysinfo(st_dbsSysinfo *row);
int dbs_underlayer_update_row_template(st_dbsTemplate *row);
int dbs_underlayer_destory_row_clt(uint16_t id);
int dbs_underlayer_destory_row_cltconf(uint16_t id);
int dbs_underlayer_destory_row_cnu(uint16_t id);
int dbs_underlayer_destory_row_profile(uint16_t id);
int dbs_underlayer_create_su_profile(uint16_t id);
int dbs_underlayer_create_dewl_profile(uint16_t id);
int dbs_underlayer_create_debl_profile(uint16_t id);
int dbs_underlayer_create_su2_profile(uint16_t id);
int dbs_underlayer_create_dewl2_profile(uint16_t id);
int dbs_underlayer_create_debl2_profile(uint16_t id);
int dbs_underlayer_debug_enable(int enable);
int dbs_underlayer_fflush(void);
int dbs_underlayer_open(void);
int dbs_underlayer_close(void);


#endif

