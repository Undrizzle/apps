#ifndef PREVAIL_SOFTWARE_MGMT_H
#define PREVAIL_SOFTWARE_MGMT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <net-snmp/library/asn1.h>

config_add_mib(prevail-mib)
config_require(util_funcs)

/*
* function definitions 
*/
FindVarMethod var_softwareMgmt;
WriteMethod     write_softwareUploadTFTPServerIP;
WriteMethod     write_softwareUploadTFTPServerPort;
WriteMethod     write_softwareUploadLogin;
WriteMethod     write_softwareUploadPassword;
WriteMethod     write_softwareUploadFileName;
WriteMethod     write_softwareUploadProceed;

#ifdef __cplusplus
}
#endif

#endif
