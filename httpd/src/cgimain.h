#ifndef __CGI_MAIN_H__
#define __CGI_MAIN_H__

#include <stdio.h>
#include <fcntl.h>

#include <ifcdefs.h>

/********************** Global Types ****************************************/

#define WEB_BUF_SIZE_MAX      1024
#define WEB_MD_BUF_SIZE_MAX   264
#define WEB_BIG_BUF_SIZE_MAX  10000

#define WEB_DIAG_TYPE        0
#define WEB_DIAG_PREV        1
#define WEB_DIAG_CURR        2
#define WEB_DIAG_NEXT        3
#define WEB_DIAG_MAX         4

#define OPT_BUFF_MAX_LEN     16 

typedef struct {   
   char sysUserName[IFC_TINY_LEN];
   char sysPassword[IFC_PASSWORD_LEN];
   char sptUserName[IFC_TINY_LEN];
   char sptPassword[IFC_PASSWORD_LEN];
   char usrUserName[IFC_TINY_LEN];
   char usrPassword[IFC_PASSWORD_LEN];
   char curUserName[IFC_TINY_LEN];
   
   //add by frank
   char	wecFtpIpaddr[IFC_TINY_LEN];
   int		wecFtpPort;
   char	wecFtpUser[IFC_HOST_LEN];
   char	wecFtpPasswd[IFC_HOST_LEN];
   char	wecFtpFilePath[256];

   char	wecIpaddr[IFC_TINY_LEN];
   char	wecNetmask[IFC_TINY_LEN];
   char	wecDefaultGw[IFC_TINY_LEN];
   int		wecMgmtVlanSts;
   int		wecMgmtVlanId;

   char	snmpRoCommunity[IFC_HOST_LEN];
   char	snmpRwCommunity[IFC_HOST_LEN];
   char	snmpTrapIpaddr[IFC_TINY_LEN];
   int		snmpTrapDport;

   char wecDevSerial[IFC_HOST_LEN];
   char wecDevModel[IFC_HOST_LEN];
   char wecEoCType[IFC_TINY_LEN];
   int wecCltNumber;
   int wecCnuStation;
   int wecWlistStatus;
   int wecWDTStatus;
   char wecHwVersion[IFC_HOST_LEN];
   char wecBootVersion[IFC_HOST_LEN];
   char wecKernelVersion[IFC_HOST_LEN];
   char wecAppVersion[IFC_HOST_LEN];
   char wecAppHash[IFC_HOST_LEN];
   int wecFlashSize;
   int wecSdramSize;
   char wecManufactory[128];

   char cliAdminName[IFC_TINY_LEN];
   char cliAdminPwd[IFC_TINY_LEN];
   char cliSupportName[IFC_TINY_LEN];
   char cliOptPwd[IFC_TINY_LEN];
   char cliUserName[IFC_TINY_LEN];
   char cliUserPwd[IFC_TINY_LEN];

   char newCnuMac[IFC_SMALL_LEN];
   int newCnuModel;
   int newCnuPro;
   int cltid;
   int cnuid;
   int cnuPermition;
   int col_macLimit;
   int col_loagTime;
   int col_reagTime;
   int col_sfbSts;
   int col_sfuSts;
   int col_sfmSts;

   int col_vlanSts;
   int col_eth1vid;
   int col_eth2vid;
   int col_eth3vid;
   int col_eth4vid;

   int col_psctlSts;
   int col_eth1sts;
   int col_eth2sts;
   int col_eth3sts;
   int col_eth4sts;
   
   int col_rxLimitSts;
   int col_txLimitSts;
   int col_cpuPortRxRate;
   int col_cpuPortTxRate;
   int col_eth1rx;
   int col_eth1tx;
   int col_eth2rx;
   int col_eth2tx;
   int col_eth3rx;
   int col_eth3tx;
   int col_eth4rx;
   int col_eth4tx;

   int col_igmpPri;
   int col_unicastPri;
   int col_avsPri;
   int col_mcastPri;

   int col_tbaPriSts;
   int col_cosPriSts;
   int col_tosPriSts;

   int col_cos0pri;
   int col_cos1pri;
   int col_cos2pri;
   int col_cos3pri;
   int col_cos4pri;
   int col_cos5pri;
   int col_cos6pri;
   int col_cos7pri;

   int col_tos0pri;
   int col_tos1pri;
   int col_tos2pri;
   int col_tos3pri;
   int col_tos4pri;
   int col_tos5pri;
   int col_tos6pri;
   int col_tos7pri;

   int diagDir;
   char diagCnuMac[IFC_SMALL_LEN];
   int diagCnuModel;
   int diagCnuTei;

   char ccoMac[IFC_SMALL_LEN];
   char ccoNid[IFC_TINY_LEN];
   int ccoSnid;
   int ccoTei;

   int diagCnuRxRate;
   int diagCnuTxRate;
   char bitCarrier[IFC_TINY_LEN];
   int diagCnuAtten;

   char bridgedMac[IFC_SMALL_LEN];
   
   char MPDU_ACKD[IFC_SMALL_LEN];
   char MPDU_COLL[IFC_SMALL_LEN];
   char MPDU_FAIL[IFC_SMALL_LEN];
   char PBS_PASS[IFC_SMALL_LEN];
   char PBS_FAIL[IFC_SMALL_LEN];
   
   int diagResult;   

   //for upgrade
   int upgStep;
   int upgErrCode;

   int portid;

   //add for 8306e
   int swVlanEnable;
   int swUplinkPortVMode;
   int swEth1PortVMode;
   int swEth2PortVMode;
   int swEth3PortVMode;
   int swEth4PortVMode;
   int swUplinkPortVid;
   int swEth1PortVid;
   int swEth2PortVid;
   int swEth3PortVid;
   int swEth4PortVid;
   
   int swRxRateLimitEnable;
   int swTxRateLimitEnable;
   int swUplinkRxRate;
   int swEth1RxRate;
   int swEth2RxRate;
   int swEth3RxRate;
   int swEth4RxRate;
   int swUplinkTxRate;
   int swEth1TxRate;
   int swEth2TxRate;
   int swEth3TxRate;
   int swEth4TxRate;

   int swLoopDetect;
   int swldmethod;
   int swldtime;
   int swldbckfrq;
   int swldsclr;
   int swpabuzzer;
   int swentaglf;
   int swlpttlinit;
   int swlpfpri;
   int swenlpfpri;
   int swdisfltlf;
   int swenlpttl;
   int swEth1LoopStatus;
   int swEth2LoopStatus;
   int swEth3LoopStatus;
   int swEth4LoopStatus;
   char swSwitchSid[IFC_SMALL_LEN];

   int swSfDisBroadcast;
   int swSfDisMulticast;
   int swSfDisUnknown;
   int swSfRule;
   int swSfResetSrc;
   int swSfIteration;
   int swSfThresholt;

   int swMlSysEnable;
   int swMlSysThresholt;
   int swMlEth1Enable;
   int swMlEth1Thresholt;
   int swMlEth2Enable;
   int swMlEth2Thresholt;
   int swMlEth3Enable;
   int swMlEth3Thresholt;
   int swMlEth4Enable;
   int swMlEth4Thresholt;

   int wecSysupHours;
   int wecSysupMins;
   int wecSysupSecs;
   int wecSys0loads;
   int wecSys1loads;
   int wecSys2loads;
   int wecTotalram;
   int wecFreeram;

   //add by stan for template 
   int col_tempAutoSts;
   int col_curTemp;
   int col_eth1VlanAddSts;
   int col_eth1VlanStart;
   int col_eth2VlanAddSts;
   int col_eth2VlanStart;
   int col_eth3VlanAddSts;
   int col_eth3VlanStart;
   int col_eth4VlanAddSts;
   int col_eth4VlanStart;

   //add by burning for frequency set
   int freqsts;
   int stopfreq;

   //add by burning for home gateway
   char b_wan_name_1[100];
   int b_vlan_1;
   int b_priority_1;
   int b_ip_assigned_mode_1;
   int b_connection_protocol_1;
   int b_connection_mode_1;
   int b_service_type_1;
   char b_bind_lan_1[4];
   int b_bind_ssid_1;
   char b_username_1[100];
   char b_password_1[100];
   char b_ipv4_addr_1[16];
   char b_ipv4_mask_1[16];
   char b_ipv4_gw_1[16];
   char b_dns_ipv4_1[16];
   char b_dns_ipv4_copy_1[16];
   char b_wan_name_2[100];
   int b_vlan_2;
   int b_priority_2;
   int b_ip_assigned_mode_2;
   int b_connection_protocol_2;
   int b_connection_mode_2;
   int b_service_type_2;
   char b_bind_lan_2[4];
   int b_bind_ssid_2;
   char b_username_2[100];
   char b_password_2[100];
   char b_ipv4_addr_2[16];
   char b_ipv4_mask_2[16];
   char b_ipv4_gw_2[16];
   char b_dns_ipv4_2[16];
   char b_dns_ipv4_copy_2[16];
   int ssid1_status;
   int ssid2_status;
   int ssid3_status;
   int ssid4_status;
   char ssid_name1[100];
   char ssid_name2[100];
   char ssid_name3[100];
   char ssid_name4[100];
   
   char wecTemprature[IFC_SMALL_LEN];	/* host envirument temperature */

   char frmloadUrl[IFC_MEDIUM_LEN];
   char returnUrl[IFC_MEDIUM_LEN];
   int wecOptCode;
} WEB_NTWK_VAR, *PWEB_NTWK_VAR;

#define ADSL_BERT_STATE_STOP 0
#define ADSL_BERT_STATE_RUN 1
typedef struct {
   int berState;
   unsigned long berTime;
} WEB_TEST_VAR, *PWEB_TEST_VAR;

typedef void (*CGI_GET_HDLR) (int argc, char **argv, char *varValue);

typedef struct {
   char *cgiGetName;
   CGI_GET_HDLR cgiGetHdlr;
} CGI_GET_VAR, *PCGI_GET_VAR;

typedef void (*CGI_FNC_HDLR) (void);

typedef struct {
   char *cgiFncName;
   CGI_FNC_HDLR cgiFncHdlr;
} CGI_FNC_CMD, *PCGI_FNC_CMD;

typedef enum {
   CGI_STS_OK = 0,
   CGI_STS_ERR_GENERAL,
   CGI_STS_ERR_MEMORY,
   CGI_STS_ERR_FIND
} CGI_STATUS;

typedef enum {
   CGI_IFC_ETH = 0,
   CGI_IFC_USB
} CGI_IFC;

typedef enum {
   CGI_TYPE_NONE = 0,
   CGI_TYPE_STR,
   CGI_TYPE_MARK_STR,
   CGI_TYPE_NUM,
   CGI_TYPE_SYS_VERSION,
   CGI_TYPE_CFE_VERSION,
   CGI_TYPE_BOARD_ID,
   CGI_TYPE_ADSL_FLAG,
   CGI_TYPE_EXIST_PROTOCOL,
   CGI_TYPE_NUM_ENET,
   CGI_TYPE_NUM_PVC,
   CGI_TYPE_DHCP_LEASES,
   CGI_TYPE_PORT,
   CGI_TYPE_VPI,
   CGI_TYPE_VCI,
   CGI_TYPE_IPSEC_TABLE,
   CGI_TYPE_IPSEC_SETTINGS,
   CGI_TYPE_CERT_LIST,
   CGI_TYPE_ENET_DIAG,
   CGI_TYPE_VDSL_VERSION
} CGI_TYPE;

typedef struct {
   char *variable;
   void *value;
   CGI_TYPE type;
} CGI_ITEM, *PCGI_ITEM;


void do_cgi(char *path, FILE *fs);
void cgiFncCmd(int argc, char **argv);
void cgiGetVarOther(int argc, char **argv, char *varValue);
void cgiGetVar(char *varName, char *varValue);
void cgiSetVar(char *varName, char *varValue);

void cgiGetAllInfo(void);
void cgiInitRtl8306eSettings(void);

int cgiReboot(void);
void cgiUrlDecode(char *s);
void cgiParseSet(char *path);
CGI_STATUS cgiGetValueByName(char *query, char *id, char *val);
void cgiWriteMessagePage(FILE *fs, char *msg, char *title, char *location);

void do_test_cgi(char *path, FILE *fs);
void cgiGetTestVar(char *varName, char *varValue);
void cgiSetTestVar(char *varName, char *varValue);

void cgiTestParseSet(char *path);

int cgiUpgradeFirmware(PWEB_NTWK_VAR pWebVar);
int cgiUpgradeEnv(PWEB_NTWK_VAR pWebVar);
void cgiGetSysInfo(int argc, char **argv, char *varValue);
void cgiWriteUpgPage(int ret, FILE *fs, PWEB_NTWK_VAR pWebVar);


#endif
