#ifndef __CLI_CMM_H__
#define __CLI_CMM_H__

#include <public.h>

int cli2cmm_DoDsdtMacBinding(stDsdtMacBinding *macBindingInfo);
int cli2cmm_DoPortMirroring(st_dsdtPortMirroring *pMirrorInfo);
int cli2cmm_getCbatTemperature(st_temperature *temp_data);
int cli2cmm_setRgmiiTimingDelay(st_dsdtRgmiiTimingDelay *pdelay);
int cli2cmm_getRgmiiTimingDelay(st_dsdtRgmiiTimingDelay *pdelay);
int cli2cmm_DebugPrintAllDsdtPortStats(int port);
int cli2cmm_ClearDsdtPortStats(void);
int cli2cmm_reloadCltProfile(uint16_t cltid);
int cli2cmm_reloadCnuProfile(uint16_t cltid, uint16_t cnuid);
int cli2cmm_permitCnu(uint16_t cltid, uint16_t cnuid);
int cli2cmm_UndoPermitCnu(uint16_t cltid, uint16_t cnuid);
int cli2cmm_DeleteCnu(uint16_t cltid, uint16_t cnuid);
int cli2cmm_CreateCnu(uint8_t mac[]);
int cli2cmm_DoWlistControl(uint16_t status);
int cli2cmm_DoWdtControl(uint16_t status);
int cli2cmm_DoHBControl(uint16_t status);
int cli2cmm_readAr8236Reg(T_szAr8236Reg *szAr8236Reg);
int cli2cmm_writeAr8236Reg(T_szAr8236Reg *szAr8236Reg);
int cli2cmm_readAr8236Phy(T_szAr8236Phy *szAr8236Phy);
int cli2cmm_writeAr8236Phy(T_szAr8236Phy *szAr8236Phy);
int cli2cmm_mdioReadPhy(T_szAr8236Phy *szAr8236Phy);
int cli2cmm_mdioWritePhy(T_szAr8236Phy *szAr8236Phy);
int cli2cmm_readCnuSwitchRegister(T_szSwRtl8306eConfig *rtl8306eSettings);
int cli2cmm_writeCnuSwitchRegister(T_szSwRtl8306eConfig *rtl8306eSettings);
int cli2cmm_shutdownConfig(st_dbsProfile *profile);
int cli2cmm_macLimitConfig1(st_dbsProfile *profile);
int cli2cmm_macLimitConfig2(rtl8306eWriteInfo *req_data, st_dbsProfile *profile);
int cli2cmm_stormFilterConfig1(st_dbsProfile *profile);
int cli2cmm_stormFilterConfig2(rtl8306eWriteInfo *req_data, st_dbsProfile *profile);
int cli2cmm_rateLimitConfig1(st_dbsProfile *profile);
int cli2cmm_rateLimitConfig2(rtl8306eWriteInfo *req_data, st_dbsProfile *profile);
int cli2cmm_vlanConfig1(st_dbsProfile *profile);
int cli2cmm_vlanConfig2(rtl8306eWriteInfo *req_data, st_dbsProfile *profile);
int cli2cmm_resetClt(uint16_t id);
int cli2cmm_resetCnu(uint16_t id);
int cli2cmm_resetMp(void);
int cli2cmm_restoreDefault(void);
int cli2cmm_upgradeApp(void);
int cli2cmm_debug(st_ModuleDebugCtl *debug);
int cli2cmm_Dump(uint16_t cltid, uint16_t cunid, uint16_t flag);
int cli2cmm_do_aclDropMme(uint16_t cltid, uint16_t cnuid);
int destroy_cli_cmm(void);
int init_cli_cmm(void);
int cli2cmm_readUserHFID(T_szCnuUserHFID *cnuuserhfid);
int cli2cmm_readSwitchSettings(rtl8306eWriteInfo *rtl8306eSettings );

#endif

