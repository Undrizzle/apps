#ifndef __CMM_MMEAD_H__
#define __CMM_MMEAD_H__

#include <public.h>

int mmead_get_ar8236_phy(uint8_t ODA[], T_szMdioPhy *v);
int mmead_set_ar8236_phy(uint8_t ODA[], T_szMdioPhy *v);
int mmead_get_ar8236_reg(uint8_t ODA[], T_szMdioSw *v);
int mmead_set_ar8236_reg(uint8_t ODA[], T_szMdioSw *v);
int mmead_get_rtl8306e_register(uint8_t ODA[], T_szSwRtl8306eConfig *pRegInfo);
int mmead_set_rtl8306e_register(uint8_t ODA[], T_szSwRtl8306eConfig *pRegInfo);
int mmead_get_rtl8306e_configs(uint8_t ODA[], st_rtl8306eSettings *rtl8306e);
int mmead_get_rtl8306e_port_status(uint8_t ODA[], st_rtl8306e_port_status *linkstatus);
int mmead_write_rtl8306e_mod(uint8_t ODA[], uint8_t *mod, uint32_t mod_len);
int mmead_erase_mod(uint8_t ODA[], T_MMEAD_ERASE_MOD_REQ_INFO *erase);
int mmead_get_user_hfid(T_szCnuUserHFID *cnuuserhfid );
int mmead_get_Hg_Manage(uint8_t ODA[], T_szHgManage *manageInfo);
int mmead_set_Hg_Manage(uint8_t ODA[], T_szHgManageConfig *manageInfo);
int mmead_get_Hg_Business(uint8_t ODA[], T_szHgBusiness *businessInfo);
int mmead_set_Hg_Business(uint8_t ODA[], T_szSetHgBusinessConfig *businessInfo);


int mmead_do_link_diag
(
	uint8_t ODA[], 
	T_MMEAD_LINK_DIAG_INFO *inputInfo, 
	T_MMEAD_LINK_DIAG_RESULT *outputInfo
);

int destroy_cmm_mmead(void);
int init_cmm_mmead(void);

#endif


