#ifndef __HTTP_CMM_H__
#define __HTTP_CMM_H__

#include <stdio.h>
#include <string.h>
#include <public.h>
#include "cgimain.h"


int http2cmm_rebootClt(int id);
int http2cmm_reloadClt(int id);
int http2cmm_rebootCnu(int id);
int http2cmm_reloadCnu(int id);
int http2cmm_deleteCnu(int id);
int http2cmm_permitCnu(int id);
int http2cmm_undoPermitCnu(int id);

int http2cmm_doLinkDiag( PWEB_NTWK_VAR pWebVar );
int http2cmm_doWListCtrlSettings( PWEB_NTWK_VAR pWebVar );
int http2cmm_doSpeedLimitSettings( PWEB_NTWK_VAR pWebVar );
int http2cmm_doFrequencySettings(PWEB_NTWK_VAR pWebVar);
int http2cmm_readFrequencySettings( PWEB_NTWK_VAR pWebVar );
int http2cmm_doShutdownSettings( PWEB_NTWK_VAR pWebVar );
int http2cmm_doCnuVlanSettings( PWEB_NTWK_VAR pWebVar );
int http2cmm_doSFilterSettings( PWEB_NTWK_VAR pWebVar );
int http2cmm_doAgTimeSettings( PWEB_NTWK_VAR pWebVar );
int http2cmm_doMacLimiting( PWEB_NTWK_VAR pWebVar );
int http2cmm_createCnu( PWEB_NTWK_VAR pWebVar );

int http2cmm_clearPortStats(void);
int http2cmm_getPortStats(int port, T_CMM_PORT_STATS_INFO *stats);
int http2cmm_getPortStatsAll(T_CMM_PORT_STATS_INFO *stats);
int http2cmm_getPortPropety(int port, T_CMM_PORT_PROPETY_INFO *propety);
int http2cmm_getPortPropetyAll(T_CMM_PORT_PROPETY_INFO *propety);
int http2cmm_getCbatTemperature(st_temperature *temp_data);
int http2cmm_sysReboot(void);
int http2cmm_restoreDefault(void);
int http2cmm_getCnuUserHFID(T_szCnuUserHFID *cnuuserhfid );
int http2cmm_getRtl8306ePortStatusInfo(st_rtl8306e_port_status *linkstatus);
int http2cmm_upgrade(void);
int http2cmm_readSwitchSettings(PWEB_NTWK_VAR pWebVar);
int http2cmm_writeSwitchSettings(PWEB_NTWK_VAR pWebVar);
int http2cmm_getSwitchSettings(stCnuNode *node, st_rtl8306eSettings * rtl8306e);
int http2cmm_setSwitchSettings(stCnuNode *node, st_rtl8306eSettings * rtl8306e);
int http2cmm_eraseSwitchSettings(PWEB_NTWK_VAR pWebVar);
int http2cmm_getHgBusiness(PWEB_NTWK_VAR pWebVar);
int http2cmm_setHgBusiness1(PWEB_NTWK_VAR pWebVar);
int http2cmm_setHgBusiness2(PWEB_NTWK_VAR pWebVar);
int http2cmm_getNmsBusiness(stCnuNode *node,T_szNmsBusiness *business);
int http2cmm_setNmsBusiness(stCnuNode *node, T_szSetNmsBusiness *business);


int http2cmm_destroy(void);
int http2cmm_init(void);

#endif 


