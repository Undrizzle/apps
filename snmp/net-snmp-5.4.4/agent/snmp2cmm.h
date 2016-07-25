#ifndef __MSG_AGENT_CMM_H__
#define __MSG_AGENT_CMM_H__

#include <stdio.h>
#include <string.h>
#include <public.h>

extern T_DBS_DEV_INFO *dbsdev;

int snmp2cmm_reloadCnuProfile(uint16_t id);
int snmp2cmm_resetCnu(uint16_t id);
int snmp2cmm_permitCnu(uint16_t id);
int snmp2cmm_UndoPermitCnu(uint16_t id);
int snmp2cmm_DeleteCnu(uint16_t id);
int snmp2cmm_restoreDefault(void);
int snmp2cmm_resetMp(void);
int snmp2cmm_uploadSoftware(void);
int snmp2cmm_isIpv4Valid(uint8_t * sip, uint32_t len);
int snmp2cmm_isIpv4NetmaskValid(uint8_t * sip, uint32_t len);
int snmp2cmm_macs2b(const char *str, uint8_t *bin);
int snmp2cmm_mapDevModel(int model);
int snmp2cmm_init(void);
int snmp2cmm_destroy(void);
int snmp2cmm_sendHeartbeat(void);
//int snmp2cmm_saveConfig(void);
//int msg_agc_do_network_config(T_szNetwork *p_networkInfo);
//int msg_agc_get_snmpConfig(T_szSnmpConf *snmpConfig);
//int msg_agc_set_snmpConfig(T_szSnmpConf *snmpConfig);
//int msg_agc_get_mib_tbl_row(szTblVariables_t *tbl, void *b, uint32_t len);
//int msg_agc_reset_all(void);

#endif
