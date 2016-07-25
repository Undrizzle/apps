#ifndef __TEMPLATE_INTERFACE__
#define __TEMPLATE_INTERFACE__

#include <public.h>

uint32_t tm_add_cnu_into_wlist(stTmUserInfo *szTmNewUser);
uint32_t tm_del_cnu_from_wlist(stTmUserInfo *szTmUser);
uint16_t tm_del_cnu(stTmUserInfo *szTmUser);
uint32_t tm_get_profile(st_dbsProfile *profile);
uint32_t tm_write_profile(st_dbsProfile *profile);
uint32_t tm_get_wlist_status(uint32_t *status);
uint32_t tm_enable_wlist(void);
uint32_t tm_disable_wlist(void);
uint32_t tm_destroy_conf(stAutoGenConfig *pConf);
uint32_t tm_gen_conf(stAutoGenConfig *pConf);
uint32_t tm_gen_crc(stAutoGenCrc *pCrc);
uint32_t tm_dump_register(uint16_t clt_index, uint16_t cnu_index);
uint32_t tm_dump_mod(uint16_t clt_index, uint16_t cnu_index);
uint32_t tm_dump_cnu_pib(uint16_t clt_index, uint16_t cnu_index);
uint32_t tm_dump_clt_pib(uint16_t clt_index);

//uint32_t tm_get_template(st_dbsProfile *profile);
//uint32_t tm_write_template(st_dbsProfile *profile);
//uint32_t tm_gen_conf(stAutoGenConfig *pConf);
//uint32_t tm_destroy_conf(stAutoGenConfig *pConf);
//uint32_t tm_get_anonymous_auth(uint32_t *status);
//uint32_t tm_set_anonymous_auth(uint32_t status);
//uint32_t tm_get_auto_config_sts(uint32_t *status);
//uint32_t tm_set_auto_config_sts(uint32_t status);
//uint32_t tm_get_user_auto_config_sts(stUserAutoConfigInfo *userAutoConfinfo);
//uint32_t tm_set_user_auto_config_sts(stUserAutoConfigInfo *userAutoConfinfo);
//uint32_t tm_get_template(uint32_t tid, T_szTemplate *szTm);
//uint32_t tm_write_template(stWrTemplateInfo *TemplateInfo);
//uint32_t tm_del_template(uint32_t tid);
//uint32_t tm_new_template(T_szTemplate *szTm);
//uint32_t tm_get_user_config(stTmUserConfigInfo *szTmUserConfig);
//uint32_t tm_write_user_config(stTmUserConfigInfo *szTmUserConfig);
//uint32_t tm_new_user(stTmNewUserInfo *szTmNewUser);
//uint32_t tm_del_user(stTmUserInfo *szTmUser);

#endif

