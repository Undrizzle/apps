#ifndef __AUTOCONFIG_MMEAD_MSG_H__
#define __AUTOCONFIG_MMEAD_MSG_H__

#include <public.h>

int auto_mmead_get_rtl8306e_configs(uint8_t ODA[], st_rtl8306eSettings *rtl8306e);
//int auto_mmead_write_rtl8306e_mod(uint8_t ODA[], uint8_t *mod, uint32_t mod_len);
//int msg_reg_mmead_get_nelist(uint8_t ODA[], T_MMEAD_TOPOLOGY *plist);

int msg_auto_mmead_destroy(void);
int msg_auto_mmead_init(void);

#endif