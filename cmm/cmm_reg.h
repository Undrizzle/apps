#ifndef __CMM_REGISTER_H__
#define __CMM_REGISTER_H__

#include <public.h>

void send_notification_to_reg(stRegEvent *regEvent);
void send_notification2_to_reg(uint8_t mac[]);
int destroy_cmm_reg(void);
int init_cmm_reg(void);

#endif

