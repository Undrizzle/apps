#ifndef __CMM_ALARM_MSG_H__
#define __CMM_ALARM_MSG_H__

#include <public.h>

void cmm2alarm_sendCbatResetNotification(void);
void cmm2alarm_sendCbatTopHbTrap(void);
void cmm2alarm_heartbeatEnable(uint32_t status);
void cmm2alarm_upgradeNotification(uint32_t status);
int cmm2alarm_init(void);
int cmm2alarm_destroy(void);

#endif

