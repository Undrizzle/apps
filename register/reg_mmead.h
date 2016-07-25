#ifndef __REGISTER_MMEAD_MSG_H__
#define __REGISTER_MMEAD_MSG_H__

#include <public.h>

int msg_reg_mmead_enable_user(uint32_t devType, uint8_t ODA[]);
int msg_reg_mmead_block_user(uint32_t devType, uint8_t ODA[]);
int msg_reg_mmead_wr_user_mod(uint32_t devType, uint8_t ODA[]);
int msg_reg_mmead_wr_user_pib(uint32_t devType, uint8_t ODA[]);
int msg_reg_mmead_bootout_dev(uint8_t ODA[], uint8_t CNU_MAC[]);
int msg_reg_mmead_get_clt(T_CLT_INFO *clt);
int reg_mmead_get_rtl8306e_configs(uint8_t ODA[], st_rtl8306eSettings *rtl8306e);
int reg_mmead_write_rtl8306e_mod(uint8_t ODA[], uint8_t *mod, uint32_t mod_len);
int msg_reg_mmead_get_nelist(uint8_t ODA[], T_MMEAD_TOPOLOGY *plist);
int msg_reg_mmead_get_user_hfid(uint8_t ODA[], uint8_t *pdata);
int msg_reg_mmead_reset_eoc(uint32_t devType, uint8_t ODA[]);
int msg_mmead_destroy(void);
int msg_mmead_init(void);

#endif


