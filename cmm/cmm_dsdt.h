#ifndef __CMM_DSDT_H__
#define __CMM_DSDT_H__

#include <stdio.h>
#include <string.h>
#include <public.h>

int cmm2dsdt_setPortMirroring(st_dsdtPortMirroring *pMirror);
int cmm2dsdt_getRgmiiTimingDelay(st_dsdtRgmiiTimingDelay *pdelay);
int cmm2dsdt_setRgmiiTimingDelay(st_dsdtRgmiiTimingDelay *pdelay);
int cmm2dsdt_debugPrintPortAllCounters(int port);
int cmm2dsdt_clearPortCounters(void);
int cmm2dsdt_getPortCtr(int port, T_CMM_PORT_STATS_INFO *stats);
int cmm2dsdt_getPortAllCounters(int port, T_CMM_PORT_STATS_INFO *stats);
int cmm2dsdt_getPortLinkStatus(int port);
int cmm2dsdt_getPortLinkSpeed(int port);
int cmm2dsdt_getPortLinkDuplex(int port);
int cmm2dsdt_getPortPri(int port);
int cmm2dsdt_getPortFlowControl(int port);
int cmm2dsdt_getPortState(int port);
int cmm2dsdt_mgmtVlanInit(void);
int cmm2dsdt_addAtherosMulticastAddress2Port(int portid);
int cmm2dsdt_addAtherosMulticastAddressToAllCablePort(void);
int cmm2dsdt_delAtherosMulticastAddressFromAtu(void);
int cmm2dsdt_bindingMacAddress(stDsdtMacBinding *macBindingInfo);
int cmm2dsdt_init(void);

#endif 
