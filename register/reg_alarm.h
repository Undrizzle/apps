#ifndef __REGISTER_ALARM_MSG_H__
#define __REGISTER_ALARM_MSG_H__

#include <public.h>

void clt_sts_transition_notification(uint32_t id, BOOLEAN sts);
void cnu_sts_transition_notification(uint32_t clt, uint32_t cnu, BOOLEAN sts);
void lllegal_cnu_register_notification(uint32_t clt, uint8_t mac[]);
void cnu_exceed_notification(uint32_t clt);
void cnu_block_notification(uint32_t clt, uint32_t cnu, uint8_t mac[], BOOLEAN sts);
void cnu_auto_config_notification(uint32_t clt, uint32_t cnu, uint32_t tid, uint32_t cKey, BOOLEAN sts);
void cnu_abort_auto_config_notification(uint32_t clt, uint32_t cnu);
void lllegal_cnu_kick_off_notification(uint32_t clt, uint8_t mac[], BOOLEAN sts);
void cnu_re_register_notification(uint32_t clt, uint32_t cnu);
void clt_heartbeat_loss_notification(uint32_t clt, uint32_t count);
void clt_cannot_finded_notification(uint32_t clt);
void cbat_system_sts_notification(uint8_t status);
void reg2alarm_send_heartbeat_notification(void);
int msg_alarm_destroy(void);
int msg_alarm_init(void);

#endif

