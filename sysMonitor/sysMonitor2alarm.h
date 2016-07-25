#ifndef __SYSMONITOR_ALARM_H__
#define __SYSMONITOR_ALARM_H__

#include <public.h>

void sysMonitor2alarm_get_temperature_error_notification(T_UDP_SK_INFO *sk);
void sysMonitor2alarm_send_temperature_notification(T_UDP_SK_INFO *sk, st_temperature *curTemp);

int sysMonitor2alarm_destroy(T_UDP_SK_INFO *sk);
int sysMonitor2alarm_init(T_UDP_SK_INFO *sk);

#endif

