function createBcmMenu()
{   
	foldersTree = gFld('', 'info.html');
	//ϵͳ���ò˵�
	wecSysSettings = insFld(foldersTree, gFld(getMenuTitle(MENU_WEC_SYS_SETTINGS), 'showNetworkInfo.cmd'));
	insFld(wecSysSettings, gFld(getMenuTitle(MENU_WEC_NTWK_SETTINGS), 'showNetworkInfo.cmd'));
	insFld(wecSysSettings, gFld(getMenuTitle(MENU_WEC_SNMP_SETTINGS), 'previewSnmp.cgi'));
	////insFld(wecSysSettings, gFld(getMenuTitle(MENU_WEC_NTP_SETTINGS), 'wecNtpcfg.html'));
	//�˿����ò˵�
	wecPortSettings = insFld(foldersTree, gFld(getMenuTitle(MENU_WEC_PORT_SETTINGS), 'wecPortPropety.cmd'));
	insFld(wecPortSettings, gFld(getMenuTitle(MENU_WEC_PORT_PROPERTY), 'wecPortPropety.cmd'));
	insFld(wecPortSettings, gFld(getMenuTitle(MENU_WEC_PORT_STATISTICS), 'wecPortStats.cmd'));
	//�������˵�
	wecNtwkMgmt = insFld(foldersTree, gFld(getMenuTitle(MENU_WEC_NET_MGMT), 'previewTopology.cgi'));
	insFld(wecNtwkMgmt, gFld(getMenuTitle(MENU_WEC_TOPOLOGY), 'previewTopology.cgi'));
	//insFld(wecNtwkMgmt, gFld(getMenuTitle(MENU_WEC_CLT_MGMT), 'cltManagement.cmd'));
	//insFld(wecNtwkMgmt, gFld(getMenuTitle(MENU_WEC_CNU_MGMT), 'previewCnus.cgi'));
	insFld(wecNtwkMgmt, gFld(getMenuTitle(MENU_WEC_WLIST_CTL), 'wecWlistCtrl.html'));
	insFld(wecNtwkMgmt, gFld(getMenuTitle(MENU_WEC_LINK_DIAG), 'previewLinkDiag.cgi'));
	//ģ�����˵�
	wecTemplateMgmt = insFld(foldersTree, gFld(getMenuTitle(MENU_WEC_TEMPLATE_MGMT), 'mgmtTemplate.cgi'));	
	//insFld(wecTemplateMgmt, gFld(getMenuTitle(MENU_WEC_TEMPLATE_EDIT), 'mgmtTemplate.cgi'));
	
	
	//ϵͳ��Ϣ�˵�
	wecSystemInfo = insFld(foldersTree, gFld(getMenuTitle(MENU_WEC_SYS_INFO), 'info.html'));
	insFld(wecSystemInfo, gFld(getMenuTitle(MENU_WEC_BASIC_INFO), 'info.html'));
	insFld(wecSystemInfo, gFld(getMenuTitle(MENU_WEC_SYS_LOG), 'wecSyslog.html'));
	insFld(wecSystemInfo, gFld(getMenuTitle(MENU_WEC_OPT_LOG), 'wecOptlog.html'));
	insFld(wecSystemInfo, gFld(getMenuTitle(MENU_WEC_ALARM_LOG), 'wecAlarmlog.html'));
	//��������
	insFld(foldersTree, gFld(getMenuTitle(MENU_WEC_SAVE_CONFIG), 'wecSaveDb.html'));
	//insFld(wecDevMgmt, gFld(getMenuTitle(MENU_WEC_SAVE_CONFIG), 'wecSaveDb.html'));
	insFld(foldersTree, gFld(getMenuTitle(MENU_WEC_SYSTEM_REBOOT), 'wecReboot.html'));
	insFld(foldersTree, gFld(getMenuTitle(MENU_WEC_RESTORE_DEFAULT), 'wecRestoreDefault.html'));
	////insFld(wecDevMgmt, gFld(getMenuTitle(MENU_WEC_DB_MGMT), 'info.html'));
	insFld(foldersTree, gFld(getMenuTitle(MENU_WEC_UPGRADE), 'upload.html'));
	insFld(foldersTree, gFld(getMenuTitle(MENU_WEC_FLASH), 'upgrade1.html'));
	wecAccountMgmt = insFld(foldersTree, gFld(getMenuTitle(MENU_WEC_ACCOUNT_MGMT), 'wecWebUsers.html'));
	//insDoc(wecAccountMgmt, gLnk('R', getMenuTitle(MENU_WEC_ACCOUNT_WEB), 'wecWebUsers.html'));
	//insDoc(wecAccountMgmt, gLnk('R', getMenuTitle(MENU_WEC_ACCOUNT_CLI), 'wecCliUsers.html'));
	insFld(wecAccountMgmt, gFld(getMenuTitle(MENU_WEC_ACCOUNT_WEB), 'wecWebUsers.html'));
	insFld(wecAccountMgmt, gFld(getMenuTitle(MENU_WEC_ACCOUNT_CLI), 'wecCliUsers.html'));
	//�˳��˵�
	insFld(foldersTree, gFld(getMenuTitle(MENU_WEC_WEB_EXIT), 'wecLogout.html'));

}