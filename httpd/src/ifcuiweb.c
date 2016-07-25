#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "syscall.h"
#include "ifcuiweb.h"
#include "http2dbs.h"

void BcmWeb_getAllInfo(PWEB_NTWK_VAR pWebVar)
{
	char varValue[16] = {0};
	
	memset(pWebVar, 0, sizeof(PWEB_NTWK_VAR));
	
	strcpy(pWebVar->sysUserName, "admin");
	//strcpy(pWebVar->sysPassword, "admin");
	http2dbs_getWebAdminPwd(pWebVar->sysPassword);
	strcpy(pWebVar->sptUserName, "support");
	strcpy(pWebVar->sptPassword, "support");
	strcpy(pWebVar->usrUserName, "user");
	strcpy(pWebVar->usrPassword, "user");

	strcpy(pWebVar->curUserName, "admin");

	http2dbs_getDevSerials(pWebVar->wecDevSerial);
	http2dbs_getEocType(pWebVar->wecEoCType);
	http2dbs_getCltNumber(varValue);
	pWebVar->wecCltNumber = atoi(varValue);
	http2dbs_getCnuStations(varValue);
	pWebVar->wecCnuStation = atoi(varValue);
	http2dbs_getWlistStatus(varValue);
	pWebVar->wecWlistStatus = atoi(varValue);
	http2dbs_getWdtStatus(varValue);
	pWebVar->wecWDTStatus = atoi(varValue);
	http2dbs_getFlashSize(varValue);
	pWebVar->wecFlashSize = atoi(varValue);
	http2dbs_getSdramSize(varValue);
	pWebVar->wecSdramSize = atoi(varValue);

	http2dbs_getDevModel(pWebVar->wecDevModel);
	http2dbs_getHwVersion(pWebVar->wecHwVersion);
	http2dbs_getBootVersion(pWebVar->wecBootVersion);
	http2dbs_getKernelVersion(pWebVar->wecKernelVersion);
	http2dbs_getAppVersion(pWebVar->wecAppVersion);
	http2dbs_getAppHash(pWebVar->wecAppHash);
	http2dbs_getManufactory(pWebVar->wecManufactory);

}

void BcmWeb_initRtl8306eSettings(PWEB_NTWK_VAR pWebVar)
{
	st_dbsCnu cnu;

	dbsGetCnu(dbsdev, pWebVar->cnuid, &cnu);

	pWebVar->cnuPermition = cnu.col_auth?1:0;
	pWebVar->col_eth1sts = cnu.col_auth?1:0;
	pWebVar->col_eth2sts = cnu.col_auth?1:0;
	pWebVar->col_eth3sts = cnu.col_auth?1:0;
	pWebVar->col_eth4sts = cnu.col_auth?1:0;
	
	pWebVar->swVlanEnable = 0;
	pWebVar->swUplinkPortVMode = 0;
	pWebVar->swEth1PortVMode = 0;
	pWebVar->swEth2PortVMode = 0;
	pWebVar->swEth3PortVMode = 0;
	pWebVar->swEth4PortVMode = 0;

	pWebVar->swUplinkPortVid = 1;
	pWebVar->swEth1PortVid = 1;
	pWebVar->swEth2PortVid = 1;
	pWebVar->swEth3PortVid = 1;
	pWebVar->swEth4PortVid = 1;

	pWebVar->swRxRateLimitEnable = 0;
	pWebVar->swTxRateLimitEnable = 0;

	pWebVar->swUplinkRxRate = 0x7ff;
	pWebVar->swEth1RxRate = 0x7ff;
	pWebVar->swEth2RxRate = 0x7ff;
	pWebVar->swEth3RxRate = 0x7ff;
	pWebVar->swEth4RxRate = 0x7ff;
	pWebVar->swUplinkTxRate = 0x7ff;
	pWebVar->swEth1TxRate = 0x7ff;
	pWebVar->swEth2TxRate = 0x7ff;
	pWebVar->swEth3TxRate = 0x7ff;
	pWebVar->swEth4TxRate = 0x7ff;

	pWebVar->swLoopDetect = 0;
	pWebVar->swldmethod = 0;
	pWebVar->swldtime = 0;
	pWebVar->swldbckfrq = 0;
	pWebVar->swldsclr = 0;
	pWebVar->swpabuzzer = 0;
	pWebVar->swentaglf = 0;
	pWebVar->swlpttlinit = 0;
	pWebVar->swlpfpri = 0;
	pWebVar->swenlpfpri = 0;
	pWebVar->swdisfltlf = 0;
	pWebVar->swenlpttl = 0;
	pWebVar->swEth1LoopStatus = 0;
	pWebVar->swEth2LoopStatus = 0;
	pWebVar->swEth3LoopStatus = 0;
	pWebVar->swEth4LoopStatus = 0;
	strcpy(pWebVar->swSwitchSid, "52:54:4C:83:05:C0");

	pWebVar->swSfDisBroadcast = 1;
	pWebVar->swSfDisMulticast = 1;
	pWebVar->swSfDisUnknown = 1;
	pWebVar->swSfRule = 0;
	pWebVar->swSfResetSrc = 0;
	pWebVar->swSfIteration = 0;
	pWebVar->swSfThresholt = 0;

	//mac limit
	pWebVar->swMlSysEnable = 0;
	pWebVar->swMlSysThresholt = 0;
	pWebVar->swMlEth1Enable = 0;
	pWebVar->swMlEth1Thresholt = 0;
	pWebVar->swMlEth2Enable = 0;
	pWebVar->swMlEth2Thresholt = 0;
	pWebVar->swMlEth3Enable = 0;
	pWebVar->swMlEth3Thresholt = 0;
	pWebVar->swMlEth4Enable = 0;
	pWebVar->swMlEth4Thresholt = 0;
}

