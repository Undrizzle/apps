#ifndef PREVAIL_SYSTEM_INFO_H
#define PREVAIL_SYSTEM_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <net-snmp/library/asn1.h>

config_add_mib(prevail-mib)
config_require(util_funcs)

/*
* function definitions 
*/
FindVarMethod var_systemInfo;

#ifdef __cplusplus
}
#endif

#endif
