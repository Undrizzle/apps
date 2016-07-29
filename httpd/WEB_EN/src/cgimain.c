#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <net/route.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>

#include <httpd.h>
#include <cgimain.h>
#include <cgintwk.h>
#include <syscall.h>
#include <secdefs.h>
#include <bcmadsl.h>

#include <sysdiag.h>
#include <bcmatmapi.h>
#include <objectdefs.h>
#include <bcmcfmsys.h>
#include <bcmcfm.h>
#include <systemlog.h>
#include <ifcuiweb.h>
#include <http2dbs.h>
#include <http2cmm.h>
#include <upload.h>


#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))


CGI_GET_VAR WebVarTable[] = {
	{ "sysInfo", cgiGetSysInfo },   
};

CGI_FNC_CMD WebFncTable[] = {
   //{ "stsifcupdate",  cgiUpdateStsIfc },   
};

WEB_NTWK_VAR glbWebVar; // this global var is accessed from cgintwk.c, and cgiautoscan.c
int glbUploadMode;     // used for web page upload image or updating settings.
int glbUpgradeMode;
extern int glbstyle;
extern int gradestatus;
extern int gradejump;
extern void do_ej(char *path, FILE *stream);
extern void do_file(char *path, FILE *stream);

void do_cgi(char *path, FILE *fs) {
   extern void destroy(void);
   char filename[WEB_BUF_SIZE_MAX];
   char logmsg[256]={0};
   char* query = NULL;
   char* ext = NULL;
   int ret = 0;
   int cltid = 0;
   int cnuid = 0;

   query = strchr(path, '?');
   if ( query != NULL )
      cgiParseSet(path);

   filename[0] = '\0';
   ext = strchr(path, '.');
   if ( ext != NULL ) {
      *ext = '\0';
      strcpy(filename, path);
      strcat(filename, ".html");
      if ( strstr(filename, "wecUpgradeInfo.html") != NULL )
	{
		strcpy(filename, "upgrading.upg");
		strcpy(logmsg, "upgrading firmware image");
		ret = cgiUpgradeFirmware(&glbWebVar);
		if( ret != 0 )
		{
			do_upload_finnal();
		}		
		/* 升级结束才写日志*/
		if( glbWebVar.upgStep == 0 )
		{
			/*opt-log*/			
			http2dbs_writeOptlog(ret, logmsg);
		}
	}
	else if ( strstr(filename, "ntwkcfg.html") != NULL )
	{		
		ret = http2dbs_setWecNetworkConfig(&glbWebVar);
		/*opt-log*/
		strcpy(logmsg, "do network settings");
		http2dbs_writeOptlog(ret, logmsg);		
		strcpy(glbWebVar.returnUrl, "showNetworkInfo.cmd");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "previewSnmp.html") != NULL )
	{
		//strcpy(glbWebVar.frmloadUrl, "wecSnmpCfg.html");
		//strcpy(filename, "/webs/wecPreView.html");	
		strcpy(filename, "/webs/wecSnmpCfg.html");
	}
	else if ( strstr(filename, "wecSnmpCfgInfo.html") != NULL )
	{
		ret = http2dbs_setSnmpConfig(&glbWebVar);
		/*opt-log*/
		strcpy(logmsg, "do snmp settings");
		http2dbs_writeOptlog(ret, logmsg);			
		strcpy(glbWebVar.returnUrl, "wecSnmpCfg.html");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "wecSaveDbInfo.html") != NULL )
	{
		strcpy(logmsg, "save configuration to databases");
		ret = http2dbs_saveConfig();
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "wecSaveDb.html");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "wecRebootInfo.html") != NULL )
	{
		strcpy(logmsg, "cbat system reboot");
		ret = http2cmm_sysReboot();
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			strcpy(glbWebVar.returnUrl, "wecReboot.html");
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
	}
	else if ( strstr(filename, "wecRestoreInfo.html") != NULL )
	{
		strcpy(logmsg, "restore default settings");
		ret = http2cmm_restoreDefault();
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			strcpy(glbWebVar.returnUrl, "wecRestoreDefault.html");
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
	}
	else if ( strstr(filename, "wecWebUsersInfo.html") != NULL )
	{
		ret = http2dbs_setWebAdminPwd(&glbWebVar);		
		/*opt-log*/			
		http2dbs_writeOptlog(ret, "set web admin password");

		strcpy(glbWebVar.returnUrl, "wecWebUsers.html");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "wecCliAdmin.html") != NULL )
	{
		strcpy(logmsg, "modify cli admin password");
		ret = http2dbs_setCliAdminPasswd(glbWebVar.cliAdminPwd);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "wecCliUsers.html");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "wecCliOpt.html") != NULL )
	{
		strcpy(logmsg, "modify cli operator password");
		ret = http2dbs_setCliOptPasswd(glbWebVar.cliOptPwd);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "wecCliUsers.html");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "wecCliUser.html") != NULL )
	{
		strcpy(logmsg, "modify cli user password");
		ret = http2dbs_setCliUserPasswd(glbWebVar.cliUserPwd);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "wecCliUsers.html");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cltReboot.html") != NULL )
	{
		sprintf(logmsg, "reboot clt/%d", glbWebVar.cltid);
		ret = http2cmm_rebootClt(glbWebVar.cltid);
		/*opt-log*/		
		http2dbs_writeOptlog(ret, logmsg);
		sprintf(glbWebVar.returnUrl, "cltManagement.cmd?cltid=%d", glbWebVar.cltid);
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cltReload.html") != NULL )
	{
		sprintf(logmsg, "reload profile for clt/%d", glbWebVar.cltid);
		ret = http2cmm_reloadClt(glbWebVar.cltid);
		/*opt-log*/		
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "cltManagement.cmd");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cnuCreate.html") != NULL )
	{		
		ret = http2cmm_createCnu(&glbWebVar);
		/*opt-log*/
		sprintf(logmsg, "create new cnu <%s>", glbWebVar.newCnuMac);
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "previewTopology.cgi");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cnuReboot.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		sprintf(logmsg, "reboot cnu/%d/%d", cltid, cnuid);
		ret = http2cmm_rebootCnu(glbWebVar.cnuid);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		sprintf(glbWebVar.returnUrl, "cnuManagement.cmd?cnuid=%d", glbWebVar.cnuid);
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cnuReload.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		sprintf(logmsg, "reload profile for cnu/%d/%d", cltid, cnuid);
		ret = http2cmm_reloadCnu(glbWebVar.cnuid);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		sprintf(glbWebVar.returnUrl, "cnuManagement.cmd?cnuid=%d", glbWebVar.cnuid);
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cnuDelete.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		sprintf(logmsg, "delete cnu/%d/%d", cltid, cnuid);
		ret = http2cmm_deleteCnu(glbWebVar.cnuid);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "previewTopology.cgi");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cnuPermit.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		sprintf(logmsg, "permit cnu/%d/%d", cltid, cnuid);
		ret = http2cmm_permitCnu(glbWebVar.cnuid);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		sprintf(glbWebVar.returnUrl, "cnuManagement.cmd?cnuid=%d", glbWebVar.cnuid);
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "cnuUndoPermit.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		sprintf(logmsg, "undo permit cnu/%d/%d", cltid, cnuid);
		ret = http2cmm_undoPermitCnu(glbWebVar.cnuid);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		sprintf(glbWebVar.returnUrl, "cnuManagement.cmd?cnuid=%d", glbWebVar.cnuid);
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");
	}
	else if ( strstr(filename, "macLimit.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		if( glbWebVar.col_macLimit )
		{
			sprintf(logmsg, "do mac address limiting for cnu/%d/%d",  cltid, cnuid);
		}
		else
		{
			sprintf(logmsg, "uodo mac address limiting for cnu/%d/%d",  cltid, cnuid);
		}		
		ret = http2cmm_doMacLimiting(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
	}
	else if ( strstr(filename, "setAgTime.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		sprintf(logmsg, "do aging time settings for cnu/%d/%d", cltid, cnuid);	
		ret = http2cmm_doAgTimeSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
	}
	else if ( strstr(filename, "setSFilter.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		if(glbWebVar.col_sfbSts|glbWebVar.col_sfuSts|glbWebVar.col_sfmSts)
		{
			sprintf(logmsg, "do storm filter settings for cnu/%d/%d",  cltid, cnuid);	
		}
		else
		{
			sprintf(logmsg, "undo storm filter settings for cnu/%d/%d",  cltid, cnuid);	
		}
		ret = http2cmm_doSFilterSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
	}
	else if ( strstr(filename, "setCnuVlan.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		if(glbWebVar.col_vlanSts)
		{
			sprintf(logmsg, "do vlan settings for cnu/%d/%d",  cltid, cnuid);	
		}
		else
		{
			sprintf(logmsg, "undo vlan settings for cnu/%d/%d",  cltid, cnuid);	
		}
		ret = http2cmm_doCnuVlanSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
	}
	else if ( strstr(filename, "setPLinkSts.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		if(glbWebVar.col_eth1sts && glbWebVar.col_eth2sts && glbWebVar.col_eth3sts && glbWebVar.col_eth4sts)
		{
			sprintf(logmsg, "undo port shut down settings for cnu/%d/%d", cltid, cnuid);	
		}
		else
		{
			sprintf(logmsg, "do port shut down settings for cnu/%d/%d", cltid, cnuid);	
		}
		ret = http2cmm_doShutdownSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
	}
	else if ( strstr(filename, "doRateLimit.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		if(glbWebVar.col_rxLimitSts|glbWebVar.col_txLimitSts)
		{
			sprintf(logmsg, "do port speed limit settings for cnu/%d/%d", cltid, cnuid);	
		}
		else
		{
			sprintf(logmsg, "undo port speed limit settings for cnu/%d/%d", cltid, cnuid);	
		}
		ret = http2cmm_doSpeedLimitSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
	}
	else if ( strstr(filename, "saveProfile.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		sprintf(logmsg, "save profile for cnu/%d/%d", cltid, cnuid);
		ret = http2dbs_saveConfig();
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCnuPro.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else
		{
			sprintf(filename, "cnuManagement.cmd?cnuid=%d", glbWebVar.cnuid);
		}
	}
	else if ( strstr(filename, "setCltAgTime.html") != NULL )
	{
		ret = http2dbs_doCltAgTimeSettings(&glbWebVar);
		/*opt-log*/
		sprintf(logmsg, "do aging time settings for clt/%d", glbWebVar.cltid);
		http2dbs_writeOptlog(ret, logmsg);
		
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
	}
	else if ( strstr(filename, "setCltFreq.html") != NULL )
	{
		ret = http2cmm_doFrequencySettings(&glbWebVar);
		/*opt-log*/
		sprintf(logmsg, "do frequency band settings for clt/%d", glbWebVar.cltid);
		http2dbs_writeOptlog(ret, logmsg);
		
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
	}
	else if ( strstr(filename, "readCltFreq.html") != NULL )
	{
		ret = http2cmm_readFrequencySettings(&glbWebVar);
		/*opt-log*/
		sprintf(logmsg, "Get frequency band settings for clt/%d", glbWebVar.cltid);
		http2dbs_writeOptlog(ret, logmsg);
		
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
	}
	else if ( strstr(filename, "setCltDeCap.html") != NULL )
	{
		ret = http2dbs_doCltDecapSettings(&glbWebVar);
		/*opt-log*/
		sprintf(logmsg, "do default cap settings for clt/%d", glbWebVar.cltid);	
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
	}
	else if ( strstr(filename, "enableCltQoS.html") != NULL )
	{
		ret = http2dbs_doCltQosEnable(&glbWebVar);
		/*opt-log*/	
		sprintf(logmsg, "%s qos for clt/%d", glbWebVar.col_tbaPriSts?"enable":"disable", glbWebVar.cltid);
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
	}
	else if ( strstr(filename, "setCltQoS.html") != NULL )
	{
		ret = http2dbs_doCltQosSettings(&glbWebVar);
		/*opt-log*/	
		sprintf(logmsg, "do qos settings for clt/%d", glbWebVar.cltid);
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
	}
	else if ( strstr(filename, "saveCltProfile.html") != NULL )
	{		
		ret = http2dbs_saveConfig();
		/*opt-log*/
		sprintf(logmsg, "save profile for clt/%d", glbWebVar.cltid);
		http2dbs_writeOptlog(ret, logmsg);
		if( 0 != ret )
		{
			sprintf(glbWebVar.returnUrl, "editCltPro.cmd?cltid=%d", glbWebVar.cltid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "cltManagement.cmd?cltid=%d", glbWebVar.cltid);
	}
	else if ( strstr(filename, "doWlistCtl.html") != NULL )
	{
		sprintf(logmsg, "do white-list control settings");
		ret = http2cmm_doWListCtrlSettings(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		strcpy(glbWebVar.returnUrl, "wecWlistCtrl.html");
		glbWebVar.wecOptCode = (ret?1:0);
		strcpy(filename, "/webs/wecOptResult2.html");	
	}
	else if ( strstr(filename, "wecLinkDiag.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		sprintf(logmsg, "do physical link status diagnosis for cnu/%d/%d", cltid, cnuid);
		ret = http2cmm_doLinkDiag(&glbWebVar);
		/*opt-log*/			
		http2dbs_writeOptlog(ret, logmsg);
		sprintf(filename, "linkDiagResult.cmd");
	}
	else if ( strstr(filename, "clearPortStas.html") != NULL )
	{
		ret = http2cmm_clearPortStats();
		strcpy(filename, "wecPortStats.cmd");
		/*opt-log*/			
		http2dbs_writeOptlog(ret, "clear port statistics counter");
	}
	else if ( strstr(filename, "previewTopology.html") != NULL )
	{
		strcpy(glbWebVar.frmloadUrl, "wecTopology.cmd");
		strcpy(filename, "/webs/wecPreView.html");	
	}
	else if ( strstr(filename, "mgmtTemplate.html") != NULL )
	{
	
		strcpy(filename, "wecTemplateMgmt.cmd");
		/*opt-log*/			
		http2dbs_writeOptlog(ret, "management  template");
	}
	else if ( strstr(filename, "saveTemplate.html") != NULL )
	{  
		ret = http2dbs_saveTemplate(&glbWebVar);

		if( 0 != ret )
		{
			glbWebVar.wecOptCode = CMM_FAILED;
		}
		else
			glbWebVar.wecOptCode = CMM_SUCCESS;

		sprintf(glbWebVar.returnUrl, "mgmtTemplate.cgi");
		strcpy(filename, "/webs/wecOptResult2.html");
		
		/*opt-log*/					
		http2dbs_writeOptlog(ret, "save  template");
		
	}
	else if ( strstr(filename, "previewLinkDiag.html") != NULL )
	{
		strcpy(glbWebVar.frmloadUrl, "wecLinkDiag.cmd");
		strcpy(filename, "/webs/wecPreView.html");	
	}	
	else if ( strstr(filename, "rtl8306eConfig.html") != NULL )
	{
		cgiInitRtl8306eSettings();
		//sprintf(glbWebVar.frmloadUrl, "rtl8306eConfig.cgi?cnuid=%d", glbWebVar.cnuid);
		strcpy(filename, "rtl8306eConfigView.cmd");
	}	
	else if ( strstr(filename, "rtl8306eConfigRead.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		ret = http2cmm_readSwitchSettings(&glbWebVar);
		//strcpy(filename, "/webs/rtl8306eConfig.html");
		strcpy(filename, "rtl8306eConfigView.cmd");
		/*opt-log*/
		sprintf(logmsg, "read cnu/%d/%d switch settings", cltid, cnuid);
		http2dbs_writeOptlog(ret, logmsg);
	}
	else if ( strstr(filename, "BusinessWanRead") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		sprintf(logmsg, "read cnu/%d/%d home gateway business wan settings", cltid, cnuid);
		ret = http2cmm_getHgBusiness(&glbWebVar);
		http2dbs_writeOptlog(ret, logmsg);
		if(ret != 0) 
		{
			sprintf(glbWebVar.returnUrl, "editCnuWifi.cmd?cnuid=%d",glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else {
			sprintf(filename, "editCnuWifi.cmd?cnuid=%d",glbWebVar.cnuid);
		}
	}
	else if ( strstr(filename, "BusinessWan1Write") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		sprintf(logmsg, "write cnu/%d/%d home gateway bussiness wan1 settings", cltid, cnuid);
		ret = http2cmm_setHgBusiness1(&glbWebVar);
		http2dbs_writeOptlog(ret, logmsg);
		if(ret != 0)
		{
			sprintf(glbWebVar.returnUrl, "editCnuWifi.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuWifi.cmd?cnuid=%d",glbWebVar.cnuid);
	}
	else if ( strstr(filename, "BusinessWan2Write") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		sprintf(logmsg, "write cnu/%d/%d home gateway bussiness wan2 settings", cltid, cnuid);
		ret = http2cmm_setHgBusiness2(&glbWebVar);
		http2dbs_writeOptlog(ret, logmsg);
		if(ret != 0)
		{
			sprintf(glbWebVar.returnUrl, "editCnuWifi.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuWifi.cmd?cnuid=%d",glbWebVar.cnuid);
	}
	else if ( strstr(filename, "WifiReboot") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		sprintf(logmsg, "reboot cnu/%d/%d home gateway", cltid, cnuid);
		ret = http2cmm_rebootHg(&glbWebVar);
		http2dbs_writeOptlog(ret, logmsg);
		if(ret != 0)
		{
			sprintf(glbWebVar.returnUrl, "editCnuWifi.cmd?cnuid=%d", glbWebVar.cnuid);
			glbWebVar.wecOptCode = CMM_FAILED;
			strcpy(filename, "/webs/wecOptResult2.html");
		}
		else sprintf(filename, "editCnuWifi.cmd?cnuid=%d", glbWebVar.cnuid);
	}
	else if ( strstr(filename, "rtl8306eConfigWrite.html") != NULL )
	{
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		ret = http2cmm_writeSwitchSettings(&glbWebVar);
		//strcpy(filename, "/webs/rtl8306eConfig.html");
		strcpy(filename, "rtl8306eConfigView.cmd");		
		/*opt-log*/
		sprintf(logmsg, "write cnu/%d/%d switch settings", cltid, cnuid);
		http2dbs_writeOptlog(ret, logmsg);
	}
	else if ( strstr(filename, "rtl8306eConfigErase.html") != NULL )
	{	
		cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT + 1;
		cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT + 1;
		ret = http2cmm_eraseSwitchSettings(&glbWebVar);
		//strcpy(filename, "/webs/rtl8306eConfig.html");
		strcpy(filename, "rtl8306eConfigView.cmd");
		/*opt-log*/
		sprintf(logmsg, "erase cnu/%d/%d switch settings", cltid, cnuid);
		http2dbs_writeOptlog(ret, logmsg);
	}
	
	if( strstr(filename, ".cmd") != NULL )
	{
		do_cmd_cgi(filename, fs);
	}
	else if( strstr(filename, ".upg") != NULL )
	{
		cgiWriteUpgPage(ret, fs, &glbWebVar);
	}
	else
	{
		do_ej(filename, fs);
	}
   } else
      cgiWriteMessagePage(fs, "Message", "The selected web page is not implemented yet.", 0);
}

void do_test_cgi(char *path, FILE *fs) {
   char filename[WEB_BUF_SIZE_MAX];
   char* query = NULL;
   char* ext = NULL;

   query = strchr(path, '?');
   if ( query != NULL )
      cgiTestParseSet(path);

   filename[0] = '\0';
   ext = strchr(path, '.');
   if ( ext != NULL ) {
      *ext = '\0';
      strcpy(filename, path);
      strcat(filename, ".html");
      do_ej(filename, fs);
   }
}

void cgiGetVarOther(int argc, char **argv, char *varValue) {
   int i = 0;

   if ( argc < 2) return;

   for ( ; i < ARRAY_SIZE(WebVarTable); i++ )
   
      if ( strcmp(argv[1], WebVarTable[i].cgiGetName) == 0 )
         break;
   if ( i >= ARRAY_SIZE(WebVarTable) )
      return;

   (*(WebVarTable[i].cgiGetHdlr))(argc, argv, varValue);
}

void cgiFncCmd(int argc, char **argv) {
   int i = 0;

   for ( ; i < ARRAY_SIZE(WebFncTable); i++ )
      if ( strcmp(argv[1], WebFncTable[i].cgiFncName) == 0 )
         break;
   if ( i >= ARRAY_SIZE(WebFncTable) )
      return;

   (*(WebFncTable[i].cgiFncHdlr))();
}

int cgiPasswordNeedRefresh(char *attrname)
{
    int i = 0;
    const char* cgiTblPasswordNeedRefreshName[]= 
    {
        "sysPassword", 
        "sptPassword", 
        "usrPassword",
        NULL
    }; 
	
    if (attrname == NULL)
        return 0;

    for (i = 0; NULL != cgiTblPasswordNeedRefreshName[i];i++)
        if (!strcmp(attrname,cgiTblPasswordNeedRefreshName[i]))
            return 1;
    return 0;
}

CGI_ITEM CgiGetTable[] = {
   { "sysUserName", (void *)glbWebVar.sysUserName, CGI_TYPE_STR },
   { "sysPassword", (void *)glbWebVar.sysPassword, CGI_TYPE_MARK_STR },
   { "sptUserName", (void *)glbWebVar.sptUserName, CGI_TYPE_STR },
   { "sptPassword", (void *)glbWebVar.sptPassword, CGI_TYPE_MARK_STR },
   { "usrUserName", (void *)glbWebVar.usrUserName, CGI_TYPE_STR },
   { "usrPassword", (void *)glbWebVar.usrPassword, CGI_TYPE_MARK_STR },
   { "curUserName", (void *)glbWebVar.curUserName, CGI_TYPE_STR },

   { "wecDevSerial", (void *)&glbWebVar.wecDevSerial, CGI_TYPE_STR },
   { "wecDevModel", (void *)glbWebVar.wecDevModel, CGI_TYPE_STR },
   { "wecEoCType", (void *)&glbWebVar.wecEoCType, CGI_TYPE_STR },
   { "wecCltNumber", (void *)&glbWebVar.wecCltNumber, CGI_TYPE_NUM },
   { "wecCnuStation", (void *)&glbWebVar.wecCnuStation, CGI_TYPE_NUM },
   { "wecWlistStatus", (void *)&glbWebVar.wecWlistStatus, CGI_TYPE_NUM },
   { "wecWDTStatus", (void *)&glbWebVar.wecWDTStatus, CGI_TYPE_NUM },
   { "wecHwVersion", (void *)glbWebVar.wecHwVersion, CGI_TYPE_STR },
   { "wecBootVersion", (void *)glbWebVar.wecBootVersion, CGI_TYPE_STR },
   { "wecKernelVersion", (void *)glbWebVar.wecKernelVersion, CGI_TYPE_STR },
   { "wecAppVersion", (void *)glbWebVar.wecAppVersion, CGI_TYPE_STR },
   { "wecAppHash", (void *)glbWebVar.wecAppHash, CGI_TYPE_STR },
   { "wecFlashSize", (void *)&glbWebVar.wecFlashSize, CGI_TYPE_NUM },
   { "wecSdramSize", (void *)&glbWebVar.wecSdramSize, CGI_TYPE_NUM },
   { "wecManufactory", (void *)glbWebVar.wecManufactory, CGI_TYPE_STR },

   { "cltid", (void *)&glbWebVar.cltid, CGI_TYPE_NUM },
   { "cnuid", (void *)&glbWebVar.cnuid, CGI_TYPE_NUM },
   { "diagDir", (void *)&glbWebVar.diagDir, CGI_TYPE_NUM },
   { "diagResult", (void *)&glbWebVar.diagResult, CGI_TYPE_NUM },
   
   { "upgStep", (void *)&glbWebVar.upgStep, CGI_TYPE_NUM },
   { "upgErrCode", (void *)&glbWebVar.upgErrCode, CGI_TYPE_NUM },

   { "portid", (void *)&glbWebVar.portid, CGI_TYPE_NUM }, 

   { "wecSysupHours", (void *)&glbWebVar.wecSysupHours, CGI_TYPE_NUM }, 
   { "wecSysupMins", (void *)&glbWebVar.wecSysupMins, CGI_TYPE_NUM }, 
   { "wecSysupSecs", (void *)&glbWebVar.wecSysupSecs, CGI_TYPE_NUM }, 

   { "wecSys0loads", (void *)&glbWebVar.wecSys0loads, CGI_TYPE_NUM }, 
   { "wecSys1loads", (void *)&glbWebVar.wecSys1loads, CGI_TYPE_NUM }, 
   { "wecSys2loads", (void *)&glbWebVar.wecSys2loads, CGI_TYPE_NUM }, 
   { "wecTotalram", (void *)&glbWebVar.wecTotalram, CGI_TYPE_NUM }, 
   { "wecFreeram", (void *)&glbWebVar.wecFreeram, CGI_TYPE_NUM }, 

   { "wecTemprature", (void *)glbWebVar.wecTemprature, CGI_TYPE_STR },

   { "frmloadUrl", (void *)glbWebVar.frmloadUrl, CGI_TYPE_STR },
   { "returnUrl", (void *)glbWebVar.returnUrl, CGI_TYPE_STR },
   { "wecOptCode", (void *)&glbWebVar.wecOptCode, CGI_TYPE_NUM }, 

   { "swVlanEnable", (void *)&glbWebVar.swVlanEnable, CGI_TYPE_NUM }, 
   { "swUplinkPortVMode", (void *)&glbWebVar.swUplinkPortVMode, CGI_TYPE_NUM }, 
   { "swEth1PortVMode", (void *)&glbWebVar.swEth1PortVMode, CGI_TYPE_NUM }, 
   { "swEth2PortVMode", (void *)&glbWebVar.swEth2PortVMode, CGI_TYPE_NUM }, 
   { "swEth3PortVMode", (void *)&glbWebVar.swEth3PortVMode, CGI_TYPE_NUM }, 
   { "swEth4PortVMode", (void *)&glbWebVar.swEth4PortVMode, CGI_TYPE_NUM }, 

   { "swUplinkPortVid", (void *)&glbWebVar.swUplinkPortVid, CGI_TYPE_NUM }, 
   { "swEth1PortVid", (void *)&glbWebVar.swEth1PortVid, CGI_TYPE_NUM }, 
   { "swEth2PortVid", (void *)&glbWebVar.swEth2PortVid, CGI_TYPE_NUM }, 
   { "swEth3PortVid", (void *)&glbWebVar.swEth3PortVid, CGI_TYPE_NUM }, 
   { "swEth4PortVid", (void *)&glbWebVar.swEth4PortVid, CGI_TYPE_NUM }, 

   { "swRxRateLimitEnable", (void *)&glbWebVar.swRxRateLimitEnable, CGI_TYPE_NUM }, 
   { "swTxRateLimitEnable", (void *)&glbWebVar.swTxRateLimitEnable, CGI_TYPE_NUM }, 
   { "swUplinkRxRate", (void *)&glbWebVar.swUplinkRxRate, CGI_TYPE_NUM }, 
   { "swEth1RxRate", (void *)&glbWebVar.swEth1RxRate, CGI_TYPE_NUM }, 
   { "swEth2RxRate", (void *)&glbWebVar.swEth2RxRate, CGI_TYPE_NUM },
   { "swEth3RxRate", (void *)&glbWebVar.swEth3RxRate, CGI_TYPE_NUM },
   { "swEth4RxRate", (void *)&glbWebVar.swEth4RxRate, CGI_TYPE_NUM },
   { "swUplinkTxRate", (void *)&glbWebVar.swUplinkTxRate, CGI_TYPE_NUM },
   { "swEth1TxRate", (void *)&glbWebVar.swEth1TxRate, CGI_TYPE_NUM },
   { "swEth2TxRate", (void *)&glbWebVar.swEth2TxRate, CGI_TYPE_NUM },
   { "swEth3TxRate", (void *)&glbWebVar.swEth3TxRate, CGI_TYPE_NUM },
   { "swEth4TxRate", (void *)&glbWebVar.swEth4TxRate, CGI_TYPE_NUM },

   { "col_tempAutoSts", (void *)&glbWebVar.col_tempAutoSts, CGI_TYPE_NUM }, 
   { "col_curTemp", (void *)&glbWebVar.col_curTemp, CGI_TYPE_NUM }, 
   { "col_eth1VlanAddSts", (void *)&glbWebVar.col_eth1VlanAddSts, CGI_TYPE_NUM }, 
   { "col_eth1VlanStart", (void *)&glbWebVar.col_eth1VlanStart, CGI_TYPE_NUM }, 
   { "col_eth2VlanAddSts", (void *)&glbWebVar.col_eth2VlanAddSts, CGI_TYPE_NUM }, 
   { "col_eth2VlanStart", (void *)&glbWebVar.col_eth2VlanStart, CGI_TYPE_NUM }, 
   { "col_eth3VlanAddSts", (void *)&glbWebVar.col_eth3VlanAddSts, CGI_TYPE_NUM }, 
   { "col_eth3VlanStart", (void *)&glbWebVar.col_eth3VlanStart, CGI_TYPE_NUM }, 
   { "col_eth4VlanAddSts", (void *)&glbWebVar.col_eth4VlanAddSts, CGI_TYPE_NUM }, 
   { "col_eth4VlanStart", (void *)&glbWebVar.col_eth4VlanStart, CGI_TYPE_NUM }, 

   //{ "swLoopDetect", (void *)&glbWebVar.swLoopDetect, CGI_TYPE_NUM },
   //{ "swSwitchSid", (void *)glbWebVar.swSwitchSid, CGI_TYPE_STR },
   //{ "swEth1LoopStatus", (void *)&glbWebVar.swEth1LoopStatus, CGI_TYPE_NUM },
   //{ "swEth2LoopStatus", (void *)&glbWebVar.swEth2LoopStatus, CGI_TYPE_NUM },
   //{ "swEth3LoopStatus", (void *)&glbWebVar.swEth3LoopStatus, CGI_TYPE_NUM },
   //{ "swEth4LoopStatus", (void *)&glbWebVar.swEth4LoopStatus, CGI_TYPE_NUM },
   
   { NULL, NULL, CGI_TYPE_NONE }
};

void cgiGetVar(char *varName, char *varValue) {
   int i = 0;
   //char ver[IFC_TINY_LEN];
   //char id[IFC_TINY_LEN];
   struct sysinfo wecSysinfo;
#ifdef __AT30TK175STK__
	st_temperature temp_data;
#endif

   varValue[0] = '\0';
   /*You can add any source code here to synchronize the date you instresting between webgui and dbs*/
   if ( strcmp(varName, "wecIpaddr") == 0 )
   {
   	/* 从数据库获取*/
	http2dbs_getWecIpaddr(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecNetmask") == 0 )
   {
	http2dbs_getWecNetmask(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecDefaultGw") == 0 )
   {
	http2dbs_getWecDefaultGw(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecMgmtVlanSts") == 0 )
   {
	http2dbs_getWecMgmtVlanSts(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecMgmtVlanId") == 0 )
   {
	http2dbs_getWecMgmtVlanId(varValue);		
	return;
   }
   else if ( strcmp(varName, "snmpRoCommunity") == 0 )
   {
	http2dbs_getSnmpRoCommunity(varValue);		
	return;
   } 
   else if ( strcmp(varName, "snmpRwCommunity") == 0 )
   {
	http2dbs_getSnmpRwCommunity(varValue);		
	return;
   } 
   else if ( strcmp(varName, "snmpTrapIpaddr") == 0 )
   {
	http2dbs_getSnmpTrapIpaddr(varValue);		
	return;
   } 
   else if ( strcmp(varName, "snmpTrapDport") == 0 )
   {
	http2dbs_getSnmpTrapDport(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecFtpIpaddr") == 0 )
   {
	http2dbs_getFtpIpaddr(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecFtpPort") == 0 )
   {
	http2dbs_getFtpPort(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecFtpUser") == 0 )
   {
	http2dbs_getFtpUser(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecFtpPasswd") == 0 )
   {
	http2dbs_getFtpPasswd(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecFtpFilePath") == 0 )
   {
	http2dbs_getFtpFilePath(varValue);		
	return;
   }
   else if ( strcmp(varName, "wecWlistStatus") == 0 )
   {
	http2dbs_getWlistStatus(varValue);
	return;
   }   
   else if ( strcmp(varName, "wecTemprature") == 0 )
   {
#ifdef __AT30TK175STK__
	if( CMM_SUCCESS == http2cmm_getCbatTemperature(&temp_data) )
	{
		if( 0 == temp_data.sign )
		{
			sprintf(varValue, "+%d.%d Centigrade", temp_data.itemp, temp_data.ftemp);
		}
		else
		{
			sprintf(varValue, "-%d.%d Centigrade", temp_data.itemp, temp_data.ftemp);
		}
	}
	else
	{
		strcpy(varValue, "Unknown");
	}
#else
	strcpy(varValue, "Not supported");
#endif
	return;
   }
   else if ( strcmp(varName, "wecSysupHours") == 0 )
   {
   	sysinfo(&wecSysinfo);	
	glbWebVar.wecSysupHours = wecSysinfo.uptime/3600;
	glbWebVar.wecSysupMins = (wecSysinfo.uptime%3600)/60;
	glbWebVar.wecSysupSecs = (wecSysinfo.uptime%3600)%60;
	glbWebVar.wecSys0loads = wecSysinfo.loads[0];
	glbWebVar.wecSys1loads = wecSysinfo.loads[1];
	glbWebVar.wecSys2loads = wecSysinfo.loads[2];
	glbWebVar.wecTotalram = wecSysinfo.totalram/1024;
	glbWebVar.wecFreeram = wecSysinfo.freeram/1024;
	sprintf(varValue, "%02d", glbWebVar.wecSysupHours);
	return;
   }
   else if ( strcmp(varName, "wecSysupMins") == 0 )
   {   	
	sprintf(varValue, "%02d", glbWebVar.wecSysupMins);
	return;
   }
   else if ( strcmp(varName, "wecSysupSecs") == 0 )
   {   	
	sprintf(varValue, "%02d", glbWebVar.wecSysupSecs);
	return;
   }
   
   for ( ; CgiGetTable[i].variable != NULL; i++ )
      if ( strcmp(varName, CgiGetTable[i].variable) == 0 )
         break;
         
   if ( CgiGetTable[i].variable != NULL ) {
      switch ( CgiGetTable[i].type ) {
      case CGI_TYPE_STR:
         strcpy(varValue, (char *)CgiGetTable[i].value);
         break;
      case CGI_TYPE_MARK_STR:
         strcpy(varValue, (char *)CgiGetTable[i].value);
         //bcmProcessMarkStrChars(varValue);
         break;
      case CGI_TYPE_NUM:
         sprintf(varValue, "%d", *((int *)CgiGetTable[i].value));
         break;
      case CGI_TYPE_SYS_VERSION:         
         break;
      case CGI_TYPE_CFE_VERSION:
         break;
      case CGI_TYPE_BOARD_ID:
         break;
      case CGI_TYPE_ADSL_FLAG:
         break;
      case CGI_TYPE_EXIST_PROTOCOL:
         break;
      case CGI_TYPE_DHCP_LEASES:
         break;
      case CGI_TYPE_ENET_DIAG:
         break;
      default:
         varValue[0] = '\0';
         break;
      }
   }
}

CGI_ITEM CgiSetTable[] = {
   { "sysUserName", (void *)glbWebVar.sysUserName, CGI_TYPE_STR },
   { "sysPassword", (void *)glbWebVar.sysPassword, CGI_TYPE_STR },
   { "sptUserName", (void *)glbWebVar.sptUserName, CGI_TYPE_STR },
   { "sptPassword", (void *)glbWebVar.sptPassword, CGI_TYPE_STR },
   { "usrUserName", (void *)glbWebVar.usrUserName, CGI_TYPE_STR },
   { "usrPassword", (void *)glbWebVar.usrPassword, CGI_TYPE_STR },
   { "curUserName", (void *)glbWebVar.curUserName, CGI_TYPE_STR },
   
   { "wecFtpIpaddr", (void *)&glbWebVar.wecFtpIpaddr, CGI_TYPE_STR },
   { "wecFtpPort", (void *)&glbWebVar.wecFtpPort, CGI_TYPE_NUM },
   { "wecFtpUser", (void *)&glbWebVar.wecFtpUser, CGI_TYPE_STR },
   { "wecFtpPasswd", (void *)&glbWebVar.wecFtpPasswd, CGI_TYPE_STR },
   { "wecFtpFilePath", (void *)&glbWebVar.wecFtpFilePath, CGI_TYPE_STR },

   { "wecIpaddr", (void *)&glbWebVar.wecIpaddr, CGI_TYPE_STR },
   { "wecNetmask", (void *)&glbWebVar.wecNetmask, CGI_TYPE_STR },
   { "wecDefaultGw", (void *)&glbWebVar.wecDefaultGw, CGI_TYPE_STR },
   { "wecMgmtVlanSts", (void *)&glbWebVar.wecMgmtVlanSts, CGI_TYPE_NUM },
   { "wecMgmtVlanId", (void *)&glbWebVar.wecMgmtVlanId, CGI_TYPE_NUM },

   { "snmpRoCommunity", (void *)&glbWebVar.snmpRoCommunity, CGI_TYPE_STR },
   { "snmpRwCommunity", (void *)&glbWebVar.snmpRwCommunity, CGI_TYPE_STR },
   { "snmpTrapIpaddr", (void *)&glbWebVar.snmpTrapIpaddr, CGI_TYPE_STR },
   { "snmpTrapDport", (void *)&glbWebVar.snmpTrapDport, CGI_TYPE_NUM },

   //for create new cnu
   { "newCnuMac", (void *)&glbWebVar.newCnuMac, CGI_TYPE_STR },
   { "newCnuModel", (void *)&glbWebVar.newCnuModel, CGI_TYPE_NUM },
   { "newCnuPro", (void *)&glbWebVar.newCnuPro, CGI_TYPE_NUM },

   { "cltid", (void *)&glbWebVar.cltid, CGI_TYPE_NUM },
   { "cnuid", (void *)&glbWebVar.cnuid, CGI_TYPE_NUM },
   { "col_macLimit", (void *)&glbWebVar.col_macLimit, CGI_TYPE_NUM },
   { "col_loagTime", (void *)&glbWebVar.col_loagTime, CGI_TYPE_NUM },
   { "col_reagTime", (void *)&glbWebVar.col_reagTime, CGI_TYPE_NUM },
   { "col_sfbSts", (void *)&glbWebVar.col_sfbSts, CGI_TYPE_NUM },
   { "col_sfuSts", (void *)&glbWebVar.col_sfuSts, CGI_TYPE_NUM },
   { "col_sfmSts", (void *)&glbWebVar.col_sfmSts, CGI_TYPE_NUM },

   { "col_vlanSts", (void *)&glbWebVar.col_vlanSts, CGI_TYPE_NUM },
   { "col_eth1vid", (void *)&glbWebVar.col_eth1vid, CGI_TYPE_NUM },
   { "col_eth2vid", (void *)&glbWebVar.col_eth2vid, CGI_TYPE_NUM },
   { "col_eth3vid", (void *)&glbWebVar.col_eth3vid, CGI_TYPE_NUM },
   { "col_eth4vid", (void *)&glbWebVar.col_eth4vid, CGI_TYPE_NUM },

   { "col_psctlSts", (void *)&glbWebVar.col_psctlSts, CGI_TYPE_NUM },
   { "col_eth1sts", (void *)&glbWebVar.col_eth1sts, CGI_TYPE_NUM },
   { "col_eth2sts", (void *)&glbWebVar.col_eth2sts, CGI_TYPE_NUM },
   { "col_eth3sts", (void *)&glbWebVar.col_eth3sts, CGI_TYPE_NUM },
   { "col_eth4sts", (void *)&glbWebVar.col_eth4sts, CGI_TYPE_NUM },

   { "col_rxLimitSts", (void *)&glbWebVar.col_rxLimitSts, CGI_TYPE_NUM },
   { "col_txLimitSts", (void *)&glbWebVar.col_txLimitSts, CGI_TYPE_NUM },
   { "col_cpuPortRxRate", (void *)&glbWebVar.col_cpuPortRxRate, CGI_TYPE_NUM },
   { "col_cpuPortTxRate", (void *)&glbWebVar.col_cpuPortTxRate, CGI_TYPE_NUM },
   { "col_eth1rx", (void *)&glbWebVar.col_eth1rx, CGI_TYPE_NUM },
   { "col_eth1tx", (void *)&glbWebVar.col_eth1tx, CGI_TYPE_NUM },
   { "col_eth2rx", (void *)&glbWebVar.col_eth2rx, CGI_TYPE_NUM },
   { "col_eth2tx", (void *)&glbWebVar.col_eth2tx, CGI_TYPE_NUM },
   { "col_eth3rx", (void *)&glbWebVar.col_eth3rx, CGI_TYPE_NUM },
   { "col_eth3tx", (void *)&glbWebVar.col_eth3tx, CGI_TYPE_NUM },
   { "col_eth4rx", (void *)&glbWebVar.col_eth4rx, CGI_TYPE_NUM },
   { "col_eth4tx", (void *)&glbWebVar.col_eth4tx, CGI_TYPE_NUM },

   { "col_igmpPri", (void *)&glbWebVar.col_igmpPri, CGI_TYPE_NUM },
   { "col_unicastPri", (void *)&glbWebVar.col_unicastPri, CGI_TYPE_NUM },
   { "col_avsPri", (void *)&glbWebVar.col_avsPri, CGI_TYPE_NUM },
   { "col_mcastPri", (void *)&glbWebVar.col_mcastPri, CGI_TYPE_NUM },
   { "col_tbaPriSts", (void *)&glbWebVar.col_tbaPriSts, CGI_TYPE_NUM },
   { "col_cosPriSts", (void *)&glbWebVar.col_cosPriSts, CGI_TYPE_NUM },
   { "col_tosPriSts", (void *)&glbWebVar.col_tosPriSts, CGI_TYPE_NUM },
   { "col_cos0pri", (void *)&glbWebVar.col_cos0pri, CGI_TYPE_NUM },
   { "col_cos1pri", (void *)&glbWebVar.col_cos1pri, CGI_TYPE_NUM },
   { "col_cos2pri", (void *)&glbWebVar.col_cos2pri, CGI_TYPE_NUM },
   { "col_cos3pri", (void *)&glbWebVar.col_cos3pri, CGI_TYPE_NUM },
   { "col_cos4pri", (void *)&glbWebVar.col_cos4pri, CGI_TYPE_NUM },
   { "col_cos5pri", (void *)&glbWebVar.col_cos5pri, CGI_TYPE_NUM },
   { "col_cos6pri", (void *)&glbWebVar.col_cos6pri, CGI_TYPE_NUM },
   { "col_cos7pri", (void *)&glbWebVar.col_cos7pri, CGI_TYPE_NUM },
   { "col_tos0pri", (void *)&glbWebVar.col_tos0pri, CGI_TYPE_NUM },
   { "col_tos1pri", (void *)&glbWebVar.col_tos1pri, CGI_TYPE_NUM },
   { "col_tos2pri", (void *)&glbWebVar.col_tos2pri, CGI_TYPE_NUM },
   { "col_tos3pri", (void *)&glbWebVar.col_tos3pri, CGI_TYPE_NUM },
   { "col_tos4pri", (void *)&glbWebVar.col_tos4pri, CGI_TYPE_NUM },
   { "col_tos5pri", (void *)&glbWebVar.col_tos5pri, CGI_TYPE_NUM },
   { "col_tos6pri", (void *)&glbWebVar.col_tos6pri, CGI_TYPE_NUM },
   { "col_tos7pri", (void *)&glbWebVar.col_tos7pri, CGI_TYPE_NUM },
   { "freqsts", (void *)&glbWebVar.freqsts, CGI_TYPE_NUM },
   { "stopfreq", (void *)&glbWebVar.stopfreq, CGI_TYPE_NUM },

   { "diagDir", (void *)&glbWebVar.diagDir, CGI_TYPE_NUM },

   { "upgStep", (void *)&glbWebVar.upgStep, CGI_TYPE_NUM },
   { "upgErrCode", (void *)&glbWebVar.upgErrCode, CGI_TYPE_NUM },

   { "portid", (void *)&glbWebVar.portid, CGI_TYPE_NUM }, 

   { "wecWlistStatus", (void *)&glbWebVar.wecWlistStatus, CGI_TYPE_NUM }, 

   { "cliAdminPwd", (void *)&glbWebVar.cliAdminPwd, CGI_TYPE_STR },
   { "cliOptPwd", (void *)&glbWebVar.cliOptPwd, CGI_TYPE_STR },
   { "cliUserPwd", (void *)&glbWebVar.cliUserPwd, CGI_TYPE_STR },

   { "swVlanEnable", (void *)&glbWebVar.swVlanEnable, CGI_TYPE_NUM }, 
   { "swUplinkPortVMode", (void *)&glbWebVar.swUplinkPortVMode, CGI_TYPE_NUM }, 
   { "swEth1PortVMode", (void *)&glbWebVar.swEth1PortVMode, CGI_TYPE_NUM }, 
   { "swEth2PortVMode", (void *)&glbWebVar.swEth2PortVMode, CGI_TYPE_NUM }, 
   { "swEth3PortVMode", (void *)&glbWebVar.swEth3PortVMode, CGI_TYPE_NUM }, 
   { "swEth4PortVMode", (void *)&glbWebVar.swEth4PortVMode, CGI_TYPE_NUM }, 

   { "swUplinkPortVid", (void *)&glbWebVar.swUplinkPortVid, CGI_TYPE_NUM }, 
   { "swEth1PortVid", (void *)&glbWebVar.swEth1PortVid, CGI_TYPE_NUM }, 
   { "swEth2PortVid", (void *)&glbWebVar.swEth2PortVid, CGI_TYPE_NUM }, 
   { "swEth3PortVid", (void *)&glbWebVar.swEth3PortVid, CGI_TYPE_NUM }, 
   { "swEth4PortVid", (void *)&glbWebVar.swEth4PortVid, CGI_TYPE_NUM }, 

   { "swRxRateLimitEnable", (void *)&glbWebVar.swRxRateLimitEnable, CGI_TYPE_NUM }, 
   { "swTxRateLimitEnable", (void *)&glbWebVar.swTxRateLimitEnable, CGI_TYPE_NUM }, 
   { "swUplinkRxRate", (void *)&glbWebVar.swUplinkRxRate, CGI_TYPE_NUM }, 
   { "swEth1RxRate", (void *)&glbWebVar.swEth1RxRate, CGI_TYPE_NUM }, 
   { "swEth2RxRate", (void *)&glbWebVar.swEth2RxRate, CGI_TYPE_NUM },
   { "swEth3RxRate", (void *)&glbWebVar.swEth3RxRate, CGI_TYPE_NUM },
   { "swEth4RxRate", (void *)&glbWebVar.swEth4RxRate, CGI_TYPE_NUM },
   { "swUplinkTxRate", (void *)&glbWebVar.swUplinkTxRate, CGI_TYPE_NUM },
   { "swEth1TxRate", (void *)&glbWebVar.swEth1TxRate, CGI_TYPE_NUM },
   { "swEth2TxRate", (void *)&glbWebVar.swEth2TxRate, CGI_TYPE_NUM },
   { "swEth3TxRate", (void *)&glbWebVar.swEth3TxRate, CGI_TYPE_NUM },
   { "swEth4TxRate", (void *)&glbWebVar.swEth4TxRate, CGI_TYPE_NUM },

   { "swLoopDetect", (void *)&glbWebVar.swLoopDetect, CGI_TYPE_NUM },
   { "swSwitchSid", (void *)glbWebVar.swSwitchSid, CGI_TYPE_STR },

   { "swSfDisBroadcast", (void *)&glbWebVar.swSfDisBroadcast, CGI_TYPE_NUM }, 
   { "swSfDisMulticast", (void *)&glbWebVar.swSfDisMulticast, CGI_TYPE_NUM }, 
   { "swSfDisUnknown", (void *)&glbWebVar.swSfDisUnknown, CGI_TYPE_NUM }, 
   { "swSfRule", (void *)&glbWebVar.swSfRule, CGI_TYPE_NUM }, 
   { "swSfResetSrc", (void *)&glbWebVar.swSfResetSrc, CGI_TYPE_NUM }, 
   { "swSfIteration", (void *)&glbWebVar.swSfIteration, CGI_TYPE_NUM }, 
   { "swSfThresholt", (void *)&glbWebVar.swSfThresholt, CGI_TYPE_NUM }, 

   { "swMlSysEnable", (void *)&glbWebVar.swMlSysEnable, CGI_TYPE_NUM }, 
   { "swMlSysThresholt", (void *)&glbWebVar.swMlSysThresholt, CGI_TYPE_NUM }, 
   { "swMlEth1Enable", (void *)&glbWebVar.swMlEth1Enable, CGI_TYPE_NUM }, 
   { "swMlEth1Thresholt", (void *)&glbWebVar.swMlEth1Thresholt, CGI_TYPE_NUM }, 
   { "swMlEth2Enable", (void *)&glbWebVar.swMlEth2Enable, CGI_TYPE_NUM }, 
   { "swMlEth2Thresholt", (void *)&glbWebVar.swMlEth2Thresholt, CGI_TYPE_NUM }, 
   { "swMlEth3Enable", (void *)&glbWebVar.swMlEth3Enable, CGI_TYPE_NUM }, 
   { "swMlEth3Thresholt", (void *)&glbWebVar.swMlEth3Thresholt, CGI_TYPE_NUM }, 
   { "swMlEth4Enable", (void *)&glbWebVar.swMlEth4Enable, CGI_TYPE_NUM }, 
   { "swMlEth4Thresholt", (void *)&glbWebVar.swMlEth4Thresholt, CGI_TYPE_NUM }, 

   { "col_tempAutoSts", (void *)&glbWebVar.col_tempAutoSts, CGI_TYPE_NUM }, 
   { "col_curTemp", (void *)&glbWebVar.col_curTemp, CGI_TYPE_NUM }, 
   { "col_eth1VlanAddSts", (void *)&glbWebVar.col_eth1VlanAddSts, CGI_TYPE_NUM }, 
   { "col_eth1VlanStart", (void *)&glbWebVar.col_eth1VlanStart, CGI_TYPE_NUM }, 
   { "col_eth2VlanAddSts", (void *)&glbWebVar.col_eth2VlanAddSts, CGI_TYPE_NUM }, 
   { "col_eth2VlanStart", (void *)&glbWebVar.col_eth2VlanStart, CGI_TYPE_NUM }, 
   { "col_eth3VlanAddSts", (void *)&glbWebVar.col_eth3VlanAddSts, CGI_TYPE_NUM }, 
   { "col_eth3VlanStart", (void *)&glbWebVar.col_eth3VlanStart, CGI_TYPE_NUM }, 
   { "col_eth4VlanAddSts", (void *)&glbWebVar.col_eth4VlanAddSts, CGI_TYPE_NUM }, 
   { "col_eth4VlanStart", (void *)&glbWebVar.col_eth4VlanStart, CGI_TYPE_NUM }, 
   
   { "cnuPermition", (void *)&glbWebVar.cnuPermition, CGI_TYPE_NUM }, 

   { "bWanName1", (void *)&glbWebVar.b_wan_name_1, CGI_TYPE_STR },
   { "bWanName2", (void *)&glbWebVar.b_wan_name_2, CGI_TYPE_STR },
   { "bVlan1", (void *)&glbWebVar.b_vlan_1, CGI_TYPE_NUM },
   { "bVlan2", (void *)&glbWebVar.b_vlan_2, CGI_TYPE_NUM },
   { "bPriority1", (void *)&glbWebVar.b_priority_1, CGI_TYPE_NUM },
   { "bPriority2", (void *)&glbWebVar.b_priority_2, CGI_TYPE_NUM },
   { "bConnection1", (void *)&glbWebVar.b_connection_mode_1, CGI_TYPE_NUM },
   { "bConnection2", (void *)&glbWebVar.b_connection_mode_2, CGI_TYPE_NUM },
   { "bIpMode1", (void *)&glbWebVar.b_ip_assigned_mode_1, CGI_TYPE_NUM },
   { "bIpMode2", (void *)&glbWebVar.b_ip_assigned_mode_2, CGI_TYPE_NUM },
   { "bService1", (void *)&glbWebVar.b_service_type_1, CGI_TYPE_NUM },
   { "bService2", (void *)&glbWebVar.b_service_type_2, CGI_TYPE_NUM },
   { "bProtocol1", (void *)&glbWebVar.b_connection_protocol_1, CGI_TYPE_NUM },
   { "bProtocol2", (void *)&glbWebVar.b_connection_protocol_2, CGI_TYPE_NUM },
   { "bBindLan1", (void *)&glbWebVar.b_bind_lan_1, CGI_TYPE_STR },
   { "bBindLan2", (void *)&glbWebVar.b_bind_lan_2, CGI_TYPE_STR },
   { "bBindSsid1", (void *)&glbWebVar.b_bind_ssid_1, CGI_TYPE_NUM },
   { "bBindSsid2", (void *)&glbWebVar.b_bind_ssid_2, CGI_TYPE_NUM },
   { "bUsername1", (void *)&glbWebVar.b_username_1, CGI_TYPE_STR },
   { "bUsername2", (void *)&glbWebVar.b_username_2, CGI_TYPE_STR },
   { "bPassword1", (void *)&glbWebVar.b_password_1, CGI_TYPE_STR },
   { "bPassword2", (void *)&glbWebVar.b_password_2, CGI_TYPE_STR },
   { "bIpv4Addr1", (void *)&glbWebVar.b_ipv4_addr_1, CGI_TYPE_STR },
   { "bIpv4Addr2", (void *)&glbWebVar.b_ipv4_addr_2, CGI_TYPE_STR },
   { "bIpv4Mask1", (void *)&glbWebVar.b_ipv4_mask_1, CGI_TYPE_STR },
   { "bIpv4Mask2", (void *)&glbWebVar.b_ipv4_mask_2, CGI_TYPE_STR },
   { "bIpv4Gw1", (void *)&glbWebVar.b_ipv4_gw_1, CGI_TYPE_STR },
   { "bIpv4Gw2", (void *)&glbWebVar.b_ipv4_gw_2, CGI_TYPE_STR },
   { "bDnsIpv41", (void *)&glbWebVar.b_dns_ipv4_1, CGI_TYPE_STR },
   { "bDnsIpv42", (void *)&glbWebVar.b_dns_ipv4_2, CGI_TYPE_STR },
   { "bDnsTpv4Copy1", (void *)&glbWebVar.b_dns_ipv4_copy_1, CGI_TYPE_STR },
   { "bDnsTpv4Copy2", (void *)&glbWebVar.b_dns_ipv4_copy_1, CGI_TYPE_STR },
   
   { NULL, NULL, CGI_TYPE_NONE }
};

void cgiSetVar(char *varName, char *varValue) {
   int i = 0;

   for ( ; CgiSetTable[i].variable != NULL; i++ )
      if ( strcmp(varName, CgiSetTable[i].variable) == 0 )
         break;
         
   if ( CgiSetTable[i].variable != NULL ) {
      switch ( CgiSetTable[i].type ) {
      case CGI_TYPE_STR:
         strcpy((char *)CgiSetTable[i].value, varValue);
         break;
      case CGI_TYPE_NUM:
         *((int *)CgiSetTable[i].value) = atoi(varValue);
         break;
      case CGI_TYPE_PORT:
         break;
      case CGI_TYPE_VPI:
         break;
      case CGI_TYPE_VCI:
         break;
      default:
         break;
      }
   }
}

void cgiGetTestVar(char *varName, char *varValue) {

   
}

void cgiSetTestVar(char *varName, char *varValue) {
  
}


void cgiGetAllInfo(void) 
{
	BcmWeb_getAllInfo(&glbWebVar);
}

void cgiInitRtl8306eSettings(void)
{
	BcmWeb_initRtl8306eSettings(&glbWebVar);
}

int UpgradeBegin(PWEB_NTWK_VAR pWebVar)
{
	pWebVar->upgErrCode = RC_UPG_ON_GOING;
	return 0;
}

int UpgradePrepare(PWEB_NTWK_VAR pWebVar)
{
	return 0;
}

int UpgradeDownload(PWEB_NTWK_VAR pWebVar)
{
	return 0;
}

int UpgradeCheckfile(PWEB_NTWK_VAR pWebVar)
{
	//check file MD5
	if ( glbstyle == 1 )
	{
		if( checkUpgradeFile() == BOOL_FALSE )
		{
			pWebVar->upgErrCode = RC_UPG_MD_CHECK_ERROR;
			return -1;
		}
		else 
		{
			return 0;
		}
	}
	else if ( glbstyle == 0 )
	{
		if( checkFile() == BOOL_FALSE )
		{
			pWebVar->upgErrCode = RC_UPG_MD_CHECK_ERROR;
			return -1;
		}
		else
		{
			return 0;
		}
	}
}

int UpgradeErase(PWEB_NTWK_VAR pWebVar)
{
	//erase flash
	if (glbstyle == 1)
	{
		if( eraseUpgradeFlash() == BOOL_FALSE )
		{
			pWebVar->upgErrCode = RC_UPG_FLASH_ERASE_ERROR;
			return -1;
		}
		else
		{
			return 0;
		}
		
	}
	else if ( glbstyle == 0 )
	{
		if( eraseFlash() == BOOL_FALSE )
		{
			pWebVar->upgErrCode = RC_UPG_FLASH_ERASE_ERROR;
			return -1;
		}
		else
		{
			return 0;
		}
	}
}

int UpgradeWrite(PWEB_NTWK_VAR pWebVar)
{
	//write flash
	if (glbstyle == 1 )
	{
		if (gradestatus == 1)
		{
			if( do_upgrade_post() == RC_UPG_OK )
			{
				pWebVar->upgErrCode = RC_UPG_OK;
				pWebVar->upgStep = 8;
				glbstyle = 0;
				return 0;
			}
			else
			{
				pWebVar->upgErrCode = RC_UPG_FLASH_WRITE_ERROR;
				pWebVar->upgStep = 7;
				glbstyle = 0;
				return -1;
			}
		}
		else
		{
			if( do_upgrade_post() == RC_UPG_OK )
			{
				pWebVar->upgErrCode = RC_UPG_OK;
				if( gradejump == 1 )
					pWebVar->upgStep = 0;
				else
					pWebVar->upgStep = 7;
				glbstyle = 0;
				return  0;
			}
			else
			{
				pWebVar->upgErrCode = RC_UPG_FLASH_WRITE_ERROR;
				pWebVar->upgStep = 7;
				glbstyle = 0;
				return  -1;
			}
		}
	}
	else if ( glbstyle == 0 )
	{
		if( do_upload_post() == RC_UPG_OK )
		{
			pWebVar->upgErrCode = RC_UPG_OK;
			pWebVar->upgStep = 0;
			return 0;
		}
		else
		{
			pWebVar->upgErrCode = RC_UPG_FLASH_WRITE_ERROR;
			pWebVar->upgStep = 0;
			return -1;
		}
	}
}

int cgiUpgradeFirmware(PWEB_NTWK_VAR pWebVar)
{
	switch(pWebVar->upgStep)
	{
		case 1:
		{
			return UpgradeBegin(pWebVar);
		}
		case 2:
		{
			return UpgradePrepare(pWebVar);
		}
		case 3:
		{
			return UpgradeDownload(pWebVar);
		}
		case 4:
		{
			return UpgradeCheckfile(pWebVar);
		}
		case 5:
		{
			return UpgradeErase(pWebVar);
		}
		case 6:
		{
			return UpgradeWrite(pWebVar);
		}
		default:
		{
			return RC_UPG_GENERIC_ERROR;
		}
	}
}

void cgiGetSysInfo(int argc, char **argv, char *varValue) {
   extern int glbUploadStatus;  // it is defined in upload.c

   if ( strcmp(argv[2], "upldSts") == 0 )
      sprintf(varValue, "%d", glbUploadStatus); 
   else
      strcpy(varValue, "&nbsp");
}

int cgiReboot(void)
{
	printf("cgiReboot()\n");
	return -1;
}

void cgiWriteUpgStep1Page(FILE *fs, int ret)
{	
	if( ret == 0 )
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function upgAction(){\n");
		fprintf(fs, "	var loc = 'wecUpgradeInfo.cgi?upgStep=2';\n");
		fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('upgAction()', 1000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");		
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");		
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}
	else
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='red'>Failed</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}	
}

void cgiWriteUpgStep2Page(FILE *fs, int ret)
{	
	if( ret == 0 )
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function upgAction(){\n");
		fprintf(fs, "	var loc = 'wecUpgradeInfo.cgi?upgStep=3';\n");
		fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('upgAction()', 1000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}
	else
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='red'>Failed</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}	
}

void cgiWriteUpgStep3Page(FILE *fs, int ret)
{	
	if( ret == 0 )
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function upgAction(){\n");
		fprintf(fs, "	var loc = 'wecUpgradeInfo.cgi?upgStep=4';\n");
		fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('upgAction()', 1000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}
	else
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='red'>Failed</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}	
}

void cgiWriteUpgStep4Page(FILE *fs, int ret)
{	
	if( ret == 0 )
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function upgAction(){\n");
		fprintf(fs, "	var loc = 'wecUpgradeInfo.cgi?upgStep=5';\n");
		fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('upgAction()', 1000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Erase flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}
	else
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function reboot(){\n");
		fprintf(fs, "	var loc = 'main.html';\n");
		fprintf(fs, "	var code = 'window.top.location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('reboot()', 40000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='red'>Failed</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td><font color='red'>Firmware uploading failed</font></td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>System is restarting now, please wait</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
		bcmSystemReboot();
	}	
}

void cgiWriteUpgStep5Page(FILE *fs, int ret)
{	
	if( ret == 0 )
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function upgAction(){\n");
		fprintf(fs, "	var loc = 'wecUpgradeInfo.cgi?upgStep=6';\n");
		fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('upgAction()', 2000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Erase flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Write image to flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}
	else
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function reboot(){\n");
		fprintf(fs, "	var loc = 'main.html';\n");
		fprintf(fs, "	var code = 'window.top.location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('reboot()', 40000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Erase flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='red'>Failed</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td><font color='red'>Firmware uploading failed</font></td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>System is restarting now, please wait</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
		bcmSystemReboot();
	}	
}

void cgiWriteUpgStep0Page(FILE *fs, int ret)
{	
	if( ret == 0 )
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function reboot(){\n");
		fprintf(fs, "	var loc = 'main.html';\n");
		fprintf(fs, "	var code = 'window.top.location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('reboot()', 40000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Erase flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Write image to flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td><font color='green'>Firmware uploading success</font></td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>System is restarting now, please wait</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}
	else
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function reboot(){\n");
		fprintf(fs, "	var loc = 'main.html';\n");
		fprintf(fs, "	var code = 'window.top.location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('reboot()', 40000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Erase flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");			
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Write image to flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='red'>Failed</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td><font color='red'>Firmware uploading failed</font></td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>System is restarting now, please wait</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}	
	bcmSystemReboot();
}

void cgiWriteUpgStep7Page(FILE *fs, int ret)
{	
	if( ret == 0 )
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function reboot(){\n");
		fprintf(fs, "	var loc = 'upgrade2.html';\n");
		fprintf(fs, "	var code = 'window.location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('reboot()', 40000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Erase flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Write image to flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td><font color='green'>Firmware uploading success</font></td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>System is restarting now, please wait</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}
	else
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function reboot(){\n");
		fprintf(fs, "	var loc = 'upgrade2.html';\n");
		fprintf(fs, "	var code = 'window.location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('reboot()', 40000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Erase flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");			
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Write image to flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='red'>Failed</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td><font color='red'>Firmware uploading failed</font></td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>System is restarting now, please wait</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}	
	bcmSystemReboot();
}

void cgiWriteUpgStep8Page(FILE *fs, int ret)
{	
	if( ret == 0 )
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function reboot(){\n");
		fprintf(fs, "	var loc = 'upgrade3.html';\n");
		fprintf(fs, "	var code = 'window.location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('reboot()', 40000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Erase flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Write image to flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td><font color='green'>Firmware uploading success</font></td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>System is restarting now, please wait</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}
	else
	{
		fprintf(fs, "<html><head>\n");
		fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
		fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
		fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
		fprintf(fs, "<title>EoC</title>\n<base target='_self'>\n");
		fprintf(fs, "<SCRIPT language=JavaScript>\n");
		fprintf(fs, "function reboot(){\n");
		fprintf(fs, "	var loc = 'upgrade3.html';\n");
		fprintf(fs, "	var code = 'window.location=\"' + loc + '\"';\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function frmLoad() {\n");
		fprintf(fs, "	setTimeout('reboot()', 40000);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "</SCRIPT>\n");
		fprintf(fs, "</head>\n");
		fprintf(fs, "<body onLoad='frmLoad()'>\n<blockquote>\n<form>\n");
		fprintf(fs, "<br>\n<table border=0 cellpadding=5 cellspacing=0>\n");
		fprintf(fs, "<tr><td class='maintitle'><b>Firmware Upgrading</b></td>\n</tr>\n</table>\n");
		fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
		fprintf(fs, "<td class='mainline' width=800></td>\n</tr>\n</table>\n");
		fprintf(fs, "<br>The system is being upgraded, this will take several minutes. \n");
		fprintf(fs, "<br><font color='red'>Warning: Do not perform other operations, or cut off the power supply before the end of the operation !</font></br>\n");
		fprintf(fs, "<br><br>\n");
		fprintf(fs, "<table border=0 cellpadding=3 cellspacing=0>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td width=300>Prepare system resources</td>\n");
		fprintf(fs, "		<td width=80>......</td>\n");
		fprintf(fs, "		<td width=100>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Download firmware image</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Check firmware image MD5</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Erase flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='green'>OK</font>]</td>\n");
		fprintf(fs, "	<tr>\n");			
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>Write image to flash disk</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<font color='red'>Failed</font>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td><font color='red'>Firmware uploading failed</font></td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "		<td>&nbsp;</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td>System is restarting now, please wait</td>\n");
		fprintf(fs, "		<td>......</td>\n");
		fprintf(fs, "		<td>[<IMG src='wait.gif'>]</td>\n");
		fprintf(fs, "	<tr>\n");	
		fprintf(fs, "</table>\n");
		fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
		fflush(fs);
	}	
}



void cgiWriteUpgPage(int ret, FILE *fs, PWEB_NTWK_VAR pWebVar)
{
	switch(pWebVar->upgStep)
	{
		case 0:
		{
			return cgiWriteUpgStep0Page(fs, ret);
		}
		case 1:
		{
			return cgiWriteUpgStep1Page(fs, ret);
		}
		case 2:
		{
			return cgiWriteUpgStep2Page(fs, ret);
		}
		case 3:
		{
			return cgiWriteUpgStep3Page(fs, ret);
		}
		case 4:
		{
			return cgiWriteUpgStep4Page(fs, ret);
		}
		case 5:
		{
			return cgiWriteUpgStep5Page(fs, ret);
		}
		case 7:
		{
			return cgiWriteUpgStep7Page(fs, ret);
		}
		case 8:
		{
			return cgiWriteUpgStep8Page(fs, ret);
		}
		default:
		{
			//return cgiWriteUpgStep6Page(fs, ret);
			break;
		}
	}
	
}

void cgiWriteMessagePage(FILE *fs, char *title,
                         char *msg, char *location) {
   fprintf(fs, "<html><head>\n");
   fprintf(fs, "<link rel=stylesheet href='stylemain.css' type='text/css'>\n");
   fprintf(fs, "<link rel=stylesheet href='colors.css' type='text/css'>\n");
   fprintf(fs, "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
   fprintf(fs, "<title>EoC</title>\n");

   fprintf(fs, "<script language='javascript'>\n");
   fprintf(fs, "<!-- hide\n");
   fprintf(fs, "function btnBack() {\n");
   fprintf(fs, "   var code = 'location=\"%s\"';\n", location);
   fprintf(fs, "   eval(code);\n");
   fprintf(fs, "}\n");
   fprintf(fs, "// done hiding -->\n");
   fprintf(fs, "</script>\n");

   fprintf(fs, "</head>\n");
   fprintf(fs, "<body>\n<blockquote>\n<form>\n");
   fprintf(fs, "<b>%s</b><br><br>\n", title);
   fprintf(fs, "%s<br><br>\n", msg);

   if ( location != NULL ) {
      fprintf(fs, "<center>\n");
      fprintf(fs, "<input type='button' " \
                    "value='&nbsp;Back&nbsp;' " \
                    "onClick='btnBack()'>\n");
      fprintf(fs, "</center>\n");
   }

   fprintf(fs, "</form>\n</blockquote>\n</body>\n</html>\n");
   fflush(fs);
}

void do_auth(char *userid, char *passwd, char *realm,
             char *sptPasswd, char *usrPasswd)
{
   /*
     http://192.168.1.1 after sysPassword and usrPassword changed by TR069.
     we should refresh password information to make it pass auth.
   */
   //cgiRefreshPassword("sysPassword");
   //cgiRefreshPassword("sptPassword");
   //cgiRefreshPassword("usrPassword");
   strcpy(userid, glbWebVar.sysUserName);
   strcpy(passwd, glbWebVar.sysPassword);
   strcpy(sptPasswd, glbWebVar.sptPassword);
   strcpy(usrPasswd, glbWebVar.usrPassword);
   strcpy(realm, "EoC CBAT");
}

/* Converts hexadecimal to decimal (character): */

char hexToDec(char *what) {
   char digit;

   digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
   digit *= 16;
   digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));

   return (digit);
}

/* Unescapes "%"-escaped characters in a query: */

void unescapeUrl(char *url) {
   int x,y,len;

   len = strlen(url);

   for ( x = 0, y = 0; url[y]; x++, y++) {
      if ( (url[x] = url[y]) == '%' &&
           y < (len - 2) ) {
         url[x] = hexToDec(&url[y+1]);
         y += 2;
      }
   }
   url[x] = '\0';
}

void cgiUrlDecode(char *s) {
   char *pstr = s;

   /* convert plus (+) to space (' ') */
   for ( pstr = s;
          pstr != NULL && *pstr != '\0';
          pstr++ )
      if ( *pstr == '+' ) *pstr = ' ';

   unescapeUrl(s);
}

void cgiParseSet(char *path) {
   char *query = strchr(path, '?');
   char *name, *value, *next;

   /* Parse name=value&name=value& ... &name=value */
   if (query) {
      for (value = ++query; value; value = next) {
         name = strsep(&value, "=");
         if (name) {
            next = value;
            value = strsep(&next, "&");
            if (!value) {
               value = next;
               next = NULL;
            }
            cgiUrlDecode(value);
            cgiSetVar(name, value);
         } else
            next = NULL;
      }
   }
}

void cgiTestParseSet(char *path) {
   char *query = strchr(path, '?');
   char *name, *value, *next;

   /* Parse name=value&name=value& ... &name=value */
   if (query) {
      for (value = ++query; value; value = next) {
         name = strsep(&value, "=");
         if (name) {
            next = value;
            value = strsep(&next, "&");
            if (!value) {
               value = next;
               next = NULL;
            }
            cgiUrlDecode(value);
            cgiSetTestVar(name, value);
         } else
            next = NULL;
      }
   }
}

CGI_STATUS cgiGetValueByName(char *query, char *id, char *val) {
   int ret = CGI_STS_ERR_GENERAL;
   int idlen;
   char *name = NULL, *value = NULL, *pc = NULL;

   /* validate & initialize return value */
   if ( val == NULL ) return ret;
   *val = '\0';

   if ( query == NULL || id == NULL) return ret;

   if ( *query =='\0' || *id == '\0') return ret;

   /* search for the given id */
   /* Parse name=value&name=value& ... &name=value */
   name = strstr(query, id);
   idlen = strlen(id);
   while ( name != NULL ) {
	 if (name[idlen] != '=')  
             name = strstr(name+idlen, id);
         else
	     break;
   }	 
   
   if ( name == NULL ) return CGI_STS_ERR_FIND;
   
   value = name+idlen;
   for ( pc = val, value++;
             value != NULL && *value != '&' && *value != '\0';
             pc++, value++ )
         *pc = *value;
   *pc = '\0';
   cgiUrlDecode(val);
   
   ret = CGI_STS_OK;
   return ret;
}


