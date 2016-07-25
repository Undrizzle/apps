#ifndef PREVAIL_CBAT_ACTION_H
#define PREVAIL_CBAT_ACTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <net-snmp/library/asn1.h>

config_add_mib(prevail-mib)
config_require(util_funcs)

/*
* function definitions 
*/
FindVarMethod var_cbatAction;
WriteMethod     write_mpReset;
WriteMethod     write_restoreDefault;
WriteMethod     write_saveConfig;
WriteMethod     write_sendHeartbeat;

#ifdef __cplusplus
}
#endif

#endif
