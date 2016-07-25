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
MENU_WEC_WEB_EXIT	 				=26
MENU_WEC_TEMPLATE_MGMT                         =27
MENU_WEC_FLASH				      =28
//MENU_WEC_WEB_TEST_A	 			= 27

var menuTitle = new Array();

menuTitle[MENU_WEC_SYS_SETTINGS]              = '系统设置';
menuTitle[MENU_WEC_NTWK_SETTINGS]             = '网络设置';
menuTitle[MENU_WEC_SNMP_SETTINGS]             = 'SNMP设置';
menuTitle[MENU_WEC_NTP_SETTINGS]              = 'NTP设置';
menuTitle[MENU_WEC_PORT_SETTINGS]             = '端口设置';
menuTitle[MENU_WEC_PORT_PROPERTY]             = '端口属性';
menuTitle[MENU_WEC_PORT_STATISTICS]           = '端口统计';
menuTitle[MENU_WEC_NET_MGMT]                  = '网络管理';
menuTitle[MENU_WEC_TOPOLOGY]                  = '拓扑表';
menuTitle[MENU_WEC_CLT_MGMT]                  = '线卡管理';
menuTitle[MENU_WEC_CNU_MGMT]                  = '终端用户管理';
menuTitle[MENU_WEC_WLIST_CTL]                 = '授权管理';
menuTitle[MENU_WEC_LINK_DIAG]                 = '诊断';
menuTitle[MENU_WEC_SYS_INFO]                  = '系统信息';
menuTitle[MENU_WEC_BASIC_INFO]                = '基本信息';
menuTitle[MENU_WEC_SYS_LOG]                   = '系统日志';
menuTitle[MENU_WEC_OPT_LOG]                   = '操作日志';
menuTitle[MENU_WEC_ALARM_LOG]                 = '告警日志';
menuTitle[MENU_WEC_SAVE_CONFIG]               = '保存配置信息';
menuTitle[MENU_WEC_SYSTEM_REBOOT]             = '重启系统';
menuTitle[MENU_WEC_RESTORE_DEFAULT]           = '恢复出厂设置';
menuTitle[MENU_WEC_DB_MGMT]                   = '配置备份与恢复';
menuTitle[MENU_WEC_UPGRADE]                   = '软件升级';
menuTitle[MENU_WEC_ACCOUNT_MGMT]              = '帐户管理';
menuTitle[MENU_WEC_ACCOUNT_WEB]               = 'WEB账户';
menuTitle[MENU_WEC_ACCOUNT_CLI]               = 'CLI账户';
menuTitle[MENU_WEC_WEB_EXIT]                  = '退出';
menuTitle[MENU_WEC_TEMPLATE_MGMT]             = '模板管理与VLAN池';
menuTitle[MENU_WEC_FLASH]		      = '固件升级';
//menuTitle[MENU_WEC_WEB_TEST_A]                  = 'Test001';


function getMenuTitle(index) {
   return menuTitle[index];
}
