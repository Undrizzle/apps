#ifndef __REGISTER_TM_MSG_H__
#define __REGISTER_TM_MSG_H__

#include <public.h>

int tm_get_user_pib_crc(uint32_t clt, uint32_t cnu, uint32_t *Crc);
//int tm_get_user_mod_crc(uint32_t clt, uint32_t cnu, uint32_t *Crc);
int tm_gen_user_pib(uint32_t clt, uint32_t cnu);
int tm_gen_user_mod(uint32_t clt, uint32_t cnu);
int tm_distroy_user_pib(uint32_t clt, uint32_t cnu);
int tm_distroy_user_mod(uint32_t clt, uint32_t cnu);
int msg_tm_init(void);
int msg_tm_destroy(void);

#endif


