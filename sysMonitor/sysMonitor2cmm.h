#ifndef __SYSMONITOR_CMM_H__
#define __SYSMONITOR_CMM_H__

#include <public.h>

int sysMonitor2cmm_sysReboot(T_UDP_SK_INFO *sk);
int sysMonitor2cmm_restoreDefault(T_UDP_SK_INFO *sk);
int sysMonitor2cmm_getCbatTemperature(T_UDP_SK_INFO *sk, st_temperature *temp_data);
int sysMonitor2cmm_destroy(T_UDP_SK_INFO *sk);
int sysMonitor2cmm_init(T_UDP_SK_INFO *sk);

#endif 

