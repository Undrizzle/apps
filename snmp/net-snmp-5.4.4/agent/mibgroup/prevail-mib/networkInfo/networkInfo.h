#ifndef PREVAIL_NETWORK_INFO_H
#define PREVAIL_NETWORK_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <net-snmp/library/asn1.h>

config_add_mib(prevail-mib)
config_require(util_funcs)

/*
* function definitions 
*/
FindVarMethod var_networkInfo;
WriteMethod     write_ipAddress;
WriteMethod     write_netmask;
WriteMethod     write_gateway;
WriteMethod     write_mgmtVlanStatus;
WriteMethod     write_mgmtVlanId;

#ifdef __cplusplus
}
#endif

#endif
