#ifndef PREVAIL_SNMP_GROUP_H
#define PREVAIL_SNMP_GROUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <net-snmp/library/asn1.h>

config_add_mib(prevail-mib)
config_require(util_funcs)

/*
* function definitions 
*/
//void     init_ucdDemoPublic(void);
FindVarMethod var_snmpGroup;
WriteMethod     write_readCommunity;
WriteMethod     write_writeCommunity;
WriteMethod     write_trapCommunity;
WriteMethod     write_trapServerIpAddress;
WriteMethod     write_trapServerport;

#ifdef __cplusplus
}
#endif

#endif
