MENU_WEC_SYS_SETTINGS				 	 = 0
MENU_WEC_NTWK_SETTINGS				 = 1
MENU_WEC_SNMP_SETTINGS				 = 2
MENU_WEC_NTP_SETTINGS				 	 = 3
MENU_WEC_PORT_SETTINGS				 = 4
MENU_WEC_PORT_PROPERTY				 = 5
MENU_WEC_PORT_STATISTICS			 = 6
MENU_WEC_NET_MGMT			 				 = 7
MENU_WEC_TOPOLOGY			 		= 8
MENU_WEC_CLT_MGMT			 		= 9
MENU_WEC_CNU_MGMT			 		= 10
MENU_WEC_WLIST_CTL			 	= 11
MENU_WEC_LINK_DIAG			 	= 12
MENU_WEC_SYS_INFO			 		= 13
MENU_WEC_BASIC_INFO			 	= 14
MENU_WEC_SYS_LOG			 		= 15
MENU_WEC_OPT_LOG			 		= 16
MENU_WEC_ALARM_LOG			 	= 17
MENU_WEC_SAVE_CONFIG			= 18
MENU_WEC_SYSTEM_REBOOT		= 19
MENU_WEC_RESTORE_DEFAULT	= 20
MENU_WEC_DB_MGMT			 		= 21
MENU_WEC_UPGRADE			 		= 22
MENU_WEC_ACCOUNT_MGMT	 		= 23
MENU_WEC_ACCOUNT_WEB	 		= 24
MENU_WEC_ACCOUNT_CLI	 		= 25
MENU_WEC_WEB_EXIT	 				= 26
MENU_WEC_TEMPLATE_MGMT                         =27
MENU_WEC_FLASH					= 28
//MENU_WEC_WEB_TEST_A	 			= 27

var menuTitle = new Array();

menuTitle[MENU_WEC_SYS_SETTINGS]              = 'System Info';
menuTitle[MENU_WEC_NTWK_SETTINGS]             = 'Network';
menuTitle[MENU_WEC_SNMP_SETTINGS]             = 'SNMP';
menuTitle[MENU_WEC_NTP_SETTINGS]              = 'NTP';
menuTitle[MENU_WEC_PORT_SETTINGS]             = 'Port Settings';
menuTitle[MENU_WEC_PORT_PROPERTY]             = 'Properties';
menuTitle[MENU_WEC_PORT_STATISTICS]           = 'Statistics';
menuTitle[MENU_WEC_NET_MGMT]                  = 'Management';
menuTitle[MENU_WEC_TOPOLOGY]                  = 'Topology';
menuTitle[MENU_WEC_CLT_MGMT]                  = 'CLT';
menuTitle[MENU_WEC_CNU_MGMT]                  = 'CNU';
menuTitle[MENU_WEC_WLIST_CTL]                 = 'Authorization';
menuTitle[MENU_WEC_LINK_DIAG]                 = 'Diagnostics';
menuTitle[MENU_WEC_SYS_INFO]                  = 'Device Info';
menuTitle[MENU_WEC_BASIC_INFO]                = 'Basic';
menuTitle[MENU_WEC_SYS_LOG]                   = 'System Log';
menuTitle[MENU_WEC_OPT_LOG]                   = 'Operating Log';
menuTitle[MENU_WEC_ALARM_LOG]                 = 'Alarm Log';
menuTitle[MENU_WEC_SAVE_CONFIG]               = 'Save';
menuTitle[MENU_WEC_SYSTEM_REBOOT]             = 'Reboot';
menuTitle[MENU_WEC_RESTORE_DEFAULT]           = 'Restore Default';
menuTitle[MENU_WEC_DB_MGMT]                   = 'Settings';
menuTitle[MENU_WEC_UPGRADE]                   = 'Update Software';
menuTitle[MENU_WEC_ACCOUNT_MGMT]              = 'Passwords';
menuTitle[MENU_WEC_ACCOUNT_WEB]               = 'WEB';
menuTitle[MENU_WEC_ACCOUNT_CLI]               = 'CLI';
menuTitle[MENU_WEC_WEB_EXIT]                  = 'Exit';
menuTitle[MENU_WEC_TEMPLATE_MGMT]        ='VLAN Autoconfig';
menuTitle[MENU_WEC_FLASH]		      = 'Upgrade Firmware';
//menuTitle[MENU_WEC_WEB_TEST_A]                  = 'Test001';


function getMenuTitle(index) {
   return menuTitle[index];
}
