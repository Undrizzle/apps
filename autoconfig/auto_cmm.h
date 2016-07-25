#ifndef __AUTOCONFIG_CMM_MSG_H__
#define __AUTOCONFIG_CMM_MSG_H__

#include <public.h>

int auto2cmm_writeSwitchSettings(T_UDP_SK_INFO *sk, stCnuNode *node, st_rtl8306eSettings * rtl8306e);

int auto2cmm_init(T_UDP_SK_INFO *sk);
int auto2cmm_destroy(T_UDP_SK_INFO *sk);

#endif
