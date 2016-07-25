/**********************************************************************
 Copyright (c), 1991-2007, Prevail ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称 : cli_strres.h
 文件描述 : 用来定义字符串资源的文件


 函数列表 :


 修订记录 :
          1 创建 : 
            日期 : 2007-10-29
            描述 :

**********************************************************************/


#ifndef __CLI_STRRES_H__
#define __CLI_STRRES_H__


/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/



#define CLI_LOGED_INFO           "\r\n\r\n  Welcome to command line system! " \
                                     "\r\n  Soft Version :1.0 \r\n"
#define CLI_HOSTNAME_NULL        "\r\n  Hostname get failed!"
#define CLI_SCROLL_ENABLE        "\r\n\r\n  Set screen output scroll manual"
#define CLI_SCROLL_DISABLE       "\r\n\r\n  Set screen output scroll automatic"
#define CLI_LOGOUT_CONFIRM       "\r\n\r\n  Are you sure to logout?"
#define CLI_INPUT_CONFIRM_ERROR   "\r\n  Error input!"
#define CLI_INPUT_CONFIRM_TIMEOUT "\r\n  Input timeout!"


#define CLI_LOG_HEAD_DISP        "\r\n Operation                 Username  Log Mode  Client IP     Date      Time"
#define CLI_LOG_TABLE_ITEM       "\r\n %-25s %-9s %-7s %-15s %-18s"

/*----------------------------------*/
/*    命令管理部分语种资源定义      */
/*----------------------------------*/
#define CLI_CMD_NO_HELPINFO      "No Help Information !!!!!!!"
#define CLI_HELPINFO_HEAD        "\r\n\r\n  Command Word"
#define CLI_HELPINFO_NEXT        "\r\n\r\n  Next Command Word"
#define CLI_INTP_CMD_INCOMPLETE  "\r\n  Incompleted command: "
#define CLI_INTP_NEED_COMWORD    "\r\n  need command key: "
#define CLI_INTP_NEED_PARAM      "\r\n  need parameter: "
#define CLI_INTP_NEED_VALUE      "\r\n  need value: "
#define CLI_INTP_CMD_ERR         "\r\n  Incorrect command: "
#define CLI_INTP_CMD_FAILED      "\r\n  Execute command failed."
#define CLI_INTP_CMD_NOEXEC      "\r\n  Action is not available."
#define CLI_INTP_INVALID_PARAM   "\r\n  Invalid parameter:"
#define CLI_INTP_INVALID_VALUE   "\r\n  Invalid parameter value:"
#define CLI_INTP_INNEED_PARAM    "\r\n  Too many parameters:superfluous-parameter-"
#define CLI_PARAM_CANNOT_SAVE    "\r\n  Saving parameter failed. [Value = %s] [Index = %d]"
#define CLI_VALUE_HAS_BLANK      "\r\n  Parameter or value exclude blank!"
#define CLI_INTEG_OVERFLOW       "\r\n  Integer value out of range."
#define CLI_HEX_OVERFLOW         "\r\n  Hex value out of range."
#define CLI_STRING_TOO_LONG      "\r\n  String too long!"
#define CLI_INVALID_KEY          "\r\n  Invalid keyword value."
#define CLI_INVALID_IP           "\r\n  Invalid IP address."
#define CLI_INVALID_MAC          "\r\n  Invalid MAC address."
#define CLI_INVALID_MASK         "\r\n  Invalid MASK address."
#define CLI_INVALID_DATE         "\r\n  Invalid Date."
#define CLI_INVALID_TIME         "\r\n  Invalid Time."
#define CLI_NO_SUCH_MODE         "\r\n  Wanted a non-existed mode."
#define CLI_PUSH_CMD_FAILED      "\r\n  Push command to stack failed."
#define CLI_MODE_DEL_FAILED      "\r\n  Connect count is 0, mode delete fail."
#define CLI_POP_CMD_FAILED       "\r\n  Pop command to stack failed."
#define CLI_MODE_CONV_FAILED     "\r\n  Mode conversion failed."
#define CLI_MODE_DEL_BANNED      "\r\n Some other users are configuring in the mode, so you don't delete it!"
#define CLI_MODE_INIT_ERR        "\r\n  Initializing Command Mode Structure Failed!"
#define CLI_ROOTOBJ_INIT_ERR     "\r\n  Initializing Root Object Failed!"
#define CLI_MODECMD_REG_ERR      "\r\n  Register Mode Command Failed!"
#define CLI_CMD_BAD_STATUS       "\r\n  Bad status Pointer! (Program Error)"
#define CLI_HELP_INFO            "\r\n  Input \"?\" to get the help information,"\
                                    "\r\n  Input \"cm xxx\" to change you command mode to xxx,"\
                                    "\r\n  Input CTRL+C to interrupt the command input"\
                                    "\r\n  Now list the command word of all mode:"
#define CLI_CMD_TOO_BIG          "\r\n  Too big command "
/*----------------------------------*/
/*    命令行其它部分语种资源定义    */
/*----------------------------------*/
#define PUB_TABLE_EMBODY  "\r\n ----------------------------------------------------------------------------"
#define PUB_STRING_OUT    "%s"







#define CMDHELP_GLB_SHOW  "Commands of display system information"
#define CMDHELP_GLB_SHOW_LOG            HELP_FUN "Display log information of user "  \
                                        HELP_FMT "show log "
/* help 命令的帮助信息 */
#define CMDHELP_GLB_HELP                HELP_FUN "Help information of command line operate"

/* hostname 的帮助信息 */
#define CMDHELP_GLB_SET        "Operation of system attribute set"

/* map 的帮助信息 */
//#define CMDHELP_GLB_QUEUE_MAP        "Map cos or tos to pri queue"

/* cls 的帮助信息 */
#define CMDHELP_GLB_CLS                 HELP_FUN "Clear screen "
/* exit 的帮助信息 */
#define CMDHELP_GLB_EXIT                HELP_FUN "Logout from the system "
/* set timeout 的帮助信息 */
#define CMDHELP_GLB_SET_TIMEOUT         HELP_FUN "Set the timeout value: long or short"  \
                                        HELP_FMT "set timeout {long|short}"  \
                                        HELP_OPT "long  : long time (120minutes)"\
                                        HELP_SPC "short : short time (5 minutes)"
/* set scroll 的帮助信息 */
#define CMDHELP_GLB_SET_SCROLL          HELP_FUN "Set screen scroll mode: manual or automatic " \
                                        HELP_FMT "scroll {automatic|manual}"
/* cm 的帮助信息 */
#define CMDHELP_GLB_CM        "Command of mode convert"
/* 命令 cm ..的帮助信息*/
#define CMDHELP_GLB_CM_PARENT           HELP_FUN "Exit to parent mode "
/* 命令 cm root的帮助信息*/
#define CMDHELP_GLB_CM_ROOT             HELP_FUN "Exit to root mode "


#define CMDHELP_GENL_CM_CONFIG          HELP_FUN "Enter configure mode" \
                                        HELP_FMT "cm configure"

#define CMDHELP_GENL_CM_INTERFACE         HELP_FUN "Enter interface mode" \
                                        HELP_FMT "cm interface"

#define CMDHELP_GENL_CM_IF_CLT         HELP_FUN "Enter interface clt" \
                                        HELP_FMT "cm clt/x"                                  

#define CMDHELP_GENL_CM_IF_CNU         HELP_FUN "Enter interface cnu" \
                                        HELP_FMT "cm cnu/x/x"  

#define CMDHELP_GLB_IF_CONFIG           HELP_FUN "Show or configure the WAN interface"  \
                                        HELP_SPC "Show all interface when the word 'set' was ignored."\
                                        HELP_FMT "ifconfig [set <interface> <ip> <mask> <gateway>]"  \
                                        HELP_OPT "interface : Number of WAN interface. range of 0~1"\
                                        HELP_SPC "ip        : IP addreass"\
                                        HELP_SPC "mask      : Subnet mask"\
                                        HELP_SPC "gateway   : Gateway if interface"\
                                        HELP_EXP "ifconfig"\
                                        HELP_EXP "ifconfig set 1 10.0.0.9 255.0.0.0 10.0.0.1"

#define CMDHELP_GLB_IPT                 HELP_FUN "Show or configure the IPT module"  \
                                        HELP_SPC "Show all entry indexes recorded when the specific command was ignored."\
                                        HELP_FMT "ipt"  \
                                        HELP_FMT "ipt [add <path>]"  \
                                        HELP_FMT "ipt [del <path>]"  \
                                        HELP_FMT "ipt [get <path>]"  \
                                        HELP_FMT "ipt [set <path> <value>]"  \
                                        HELP_FMT "ipt [commit]"  \
                                        HELP_FMT "ipt [cancel]"  \
                                        HELP_FMT "ipt [wanconnect]"  \
                                        HELP_FMT "ipt [wandisconnect]"  \
                                        HELP_OPT "path      : the node path"\
                                        HELP_SPC "value     : the node value"\
                                        HELP_EXP "ipt /* Not available */"\
                                        HELP_EXP "ipt add InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping."\
                                        HELP_EXP "ipt del InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.1."\
                                        HELP_EXP "ipt get InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.1.PortMappingEnabled"\
                                        HELP_EXP "ipt set InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.1.PortMappingEnabled 1"\
                                        HELP_EXP "ipt commit"\
                                        HELP_EXP "ipt cancel"\
                                        HELP_EXP "ipt wanconnect"\
                                        HELP_EXP "ipt wandisconnect"

/* 命令topology 的帮助信息 */
#define CMDHELP_GLB_TOPOLOGY            HELP_FUN "Show topology information of the system."  \
                                        HELP_FMT "topology display" 

/* 命令reboot 的帮助信息 */
#define CMDHELP_GLB_REBOOT                HELP_FUN "Restart the cbat, clt or cnu system."  \
                                        HELP_FMT "reboot" 

/* 命令info-center 的帮助信息 */
#define CMDHELP_GLB_SHOWOPTLOG                HELP_FUN "Display the system Log."  \
                                        HELP_FMT "info-center display" 

/* 命令authentication的帮助信息 */
#define CMDHELP_GLB_SETPASSWORD1                "Set the Username's Password. authentication password set "    
								

/* 命令authentication password 的帮助信息 */
#define CMDHELP_GLB_SETPASSWORD                HELP_FUN "Config the CLI role's password."  \
                                       	HELP_FMT "password {admin|operator|user} <password>" 

/* 命令ip-address 的帮助信息 */
#define CMDHELP_GLB_SETIP                  HELP_FUN  "Config the network settings of the system. " \
						HELP_FMT "ip-address <ip> <netmask>"       

/* 命令ip-gateway 的帮助信息 */
#define CMDHELP_GLB_SETIPG                  HELP_FUN  "Config the gateway of the system. " \
						HELP_FMT "ip-gateway <gateway>"   

/* 命令anonymous 的帮助信息 */
#define CMDHELP_GLB_WLIST_CTL          HELP_FUN  "Enable or disable white list control. " \
						HELP_SPC "Only cnus in the white list can be authorized if white list is enabled."\
						HELP_FMT "white-list {enable|disable}"
/* 命令wdt 的帮助信息 */
#define CMDHELP_GLB_WDT_CTL          HELP_FUN  "Enable or disable wdt timer. " \
						HELP_FMT "wdt {enable|disable}"

/* 命令heartbeat 的帮助信息 */
#define CMDHELP_GLB_HB_CTL          HELP_FUN  "Enable or disable cbat heartbeat traps. " \
						HELP_FMT "heartbeat {enable|disable}"
						
/* 命令management-vlan 的帮助信息 */
#define CMDHELP_GLB_MANAGEMENT_VLAN               HELP_FUN "Do management vlan configuration of the system."  \
                                        HELP_FMT "mgmt-vlan <vlan>" 	
/* 命令display 的帮助信息 */
#define CMDHELP_GLB_DISPLAY                "Show system information or settings."  
                                  

/* 命令system 的帮助信息 */
#define CMDHELP_GLB_UNDOVALN              HELP_FUN "Disable management vlan settings."  \
                                        HELP_FMT "undo mgmt-vlan " 

/* 命令system 的帮助信息 */
#define CMDHELP_GLB_UNDOADDRESS              HELP_FUN "Restore default the network settings."  \
                                        HELP_FMT "undo ip-address " 

/* 命令system 的帮助信息 */
#define CMDHELP_GLB_UNDOGATEWAY             HELP_FUN "Restore default the ip-gateway settings."  \
                                        HELP_FMT "undo ip-gateway " 

/* set ftp_server 的帮助信息 */
#define CMDHELP_GLB_SET_FTP_SERVER          HELP_FUN "Set ftp-server to upgrade the system. " \
                                        HELP_FMT "ftp-server set ip-address <ip>"\
                                        HELP_FMT "ftp-server set port <port>"\
                                        HELP_FMT "ftp-server set user-name <username>"\
                                        HELP_FMT "ftp-server set passwd <password>"\
                                        HELP_FMT "ftp-server set file-path <path>"

/* set snmp 的帮助信息 */
#define CMDHELP_GLB_SET_SNMP_CONF          HELP_FUN "Do snmp settings of the system. "\
                                        HELP_FMT "snmp set read-community <community>"\
                                        HELP_FMT "snmp set write-community <community>"\
                                        HELP_FMT "snmp set trap-community <community>"\
                                        HELP_FMT "snmp set trap-server <server>"\
                                        HELP_FMT "snmp set trap-desport <port>"

/* set mac-limit 的帮助信息 */
#define CMDHELP_GLB_SET_MAC_LIMIT          HELP_FUN "Do cnu bridged mac address number limiting settings. "\
					     HELP_SPC "hosts	: number of hosts allowed to transmit through cnu device. range of 0~8."\
                                        HELP_FMT "mac-limit set bridged-host-number <hosts>"
                                        

/* set mac-limit 的帮助信息 */
#define CMDHELP_GLB_SET_AGING_TIME          HELP_FUN "Do cnu address table aging time settings. "\
					     HELP_SPC "time	: the aging time in minutes. range of 1~2000."\
                                        HELP_FMT "aging-time set {local|remote} <time> minutes"

/* qos 的帮助信息 */
#define CMDHELP_GLB_QOS_USING          HELP_FUN "Select pri queue maping type for QoS. "\
                                        HELP_FMT "qos using {cos|tos}"

/* qos map 的帮助信息 */
#define CMDHELP_GLB_QOS_MAP          HELP_FUN "Map cos or tos bits to pri queue. "\
					     HELP_SPC "bits	: cos or tos bit value. range of 0~7."\
					     HELP_SPC "pri	: qos pri queue. range of 0~3."\
                                        HELP_FMT "qos-mapping cos <bit> cap <pri>"\
                                        HELP_FMT "qos-mapping tos <bit> cap <pri>"
                                        
/* upgrade 的帮助信息 */
#define CMDHELP_GLB_UPGRADE                 HELP_FUN "Upgrade the firmware."

/* debug 的帮助信息 */
#define CMDHELP_GLB_DEBUG                 HELP_FUN "Turn ON/OFF module debug information."\
								HELP_FMT "debug: {cmm|dbs|sql} {enable|disable} " 

#define CMDHELP_GLB_AR8236_SMI_REG            HELP_FUN "Read or write AR8236 switch internal register value from MDIO interface." \
						HELP_SPC "regad	: the register address. range of 0x0000~0xFFFF."\
						HELP_SPC "regvalue	: the register value to be written. range of 0x00~0xFFFFFFFF."\
						HELP_FMT "ar8236-reg read register <regad>"  \
						HELP_FMT "ar8236-reg write <regvalue> register <regad>"

#define CMDHELP_GLB_AR8236_SMI_PHY            HELP_FUN "Read or write AR8236 switch internal phy register value from MDIO interface." \
						HELP_SPC "phyad	: the internal phy address. range of 0x0~0x4."\
						HELP_SPC "regad	: the internal phy register address. range of 0x0~0xFF."\
						HELP_SPC "regvalue	: the register value to be written. range of 0x00~0xFFFF."\
						HELP_FMT "ar8236-phy read phy <phyad> register <regad>"  \
						HELP_FMT "ar8236-phy write <regvalue> phy <phyad> register <regad>"

#define CMDHELP_GLB_CNU_SWITCH            HELP_FUN "Read or write cnu switch register value from MDIO interface." \
						HELP_SPC "phyad	: the internal phy address. range of 0~6."\
						HELP_SPC "regad	: the internal register address. range of 0~32."\
						HELP_SPC "pageid	: the internal page address. range of 0~3."\
						HELP_SPC "regvalue	: the register value to be written. range of 0x0000~0xFFFF."\
						HELP_FMT "cnu-switch read phy <phyad> register <regad> page <pageid>"  \
						HELP_FMT "cnu-switch write <regvalue> phy <phyad> register <regad> page <pageid>"

/* acl 的帮助信息 */
#define CMDHELP_GLB_CNU_ACL                 HELP_FUN "Drop mme by acl."

#define CMDHELP_GLB_MME_MDIO            HELP_FUN "Read or write phy register value from MDIO interface." \
						HELP_SPC "phyad	: the internal phy address. range of 0x0~0xf."\
						HELP_SPC "regad	: the internal phy register address. range of 0x0~0xFF."\
						HELP_SPC "regvalue	: the register value to be written. range of 0x00~0xFFFF."\
						HELP_FMT "mme-mdio read phy <phyad> register <regad>"  \
						HELP_FMT "mme-mdio write <regvalue> phy <phyad> register <regad>"

/* clt 的帮助信息 */
#define CMDHELP_GLB_CLT                 HELP_FUN "SET CLT INDEX AND CNU INDEX."\
								HELP_FMT "clt: <index(u1~4)> [cnu <cnuindex(u1~512)>]" 

/* where 的帮助信息 */
#define CMDHELP_GLB_WHERE                 HELP_FUN "Show which interface you are locate, CLT INDEX id and CNU INDEX id."\
								HELP_FMT "where" 

/* vlan 的帮助信息 */
#define CMDHELP_GLB_VLAN_CONFIG            HELP_FUN "Do cnu vlan settings." \
						HELP_SPC "You can add a vlan to vlan table after vlan enabled. There are 16 vlan index in the vlan table."\
						HELP_OPT "vid	: Vlan id(1~4094)"\
						HELP_FMT "vlan set {eth1|eth2|eth3|eth4} vlan-id <vid> "
/* vlan 的帮助信息 */
#define CMDHELP_GLB_UNDO_CNU_VLAN            HELP_FUN "Disable cnu vlan settings." \
						HELP_FMT "undo vlan"

/* reset 的帮助信息 */
#define CMDHELP_GLB_RESET                 HELP_FUN "Reset the cnu."\
								HELP_FMT "reset" 

/* save-config 的帮助信息 */
#define CMDHELP_GLB_SAVE                 HELP_FUN "Save the configurations you have operated."\
								HELP_FMT "save-config" 

#define CMDHELP_GLB_TEMPLATE            HELP_FUN "Config the information of the Template Or import/export/delete the Template ." \
						HELP_SPC "ip		: Ip address of the Template server."\
						HELP_SPC "port	: Port Num to connect to the template server. range of 0~65535."\
						HELP_SPC "username	: Login name to connect to the template server."\
						HELP_SPC "password	: Login password to connect to the template server.."\
						HELP_SPC "filename	: The file to be operated."\
						HELP_FMT "template import"\
						HELP_FMT "template export"\
						HELP_FMT "template delete"\
						HELP_FMT "template server-addr ip <ip address> [p <port>]"\
						HELP_FMT "template server-user user <username> [p <password>]"\
						HELP_FMT "template server-file file <filename> "

/* flow-control 的帮助信息 */
#define CMDHELP_GLB_FLOWCONTROL            HELP_FUN "Bandwidth limiting for the cnu device." \
						HELP_SPC "rate	:must be times of 32Kb(0Kb~100Mb). No rate limiting if rate is 0."\
						HELP_FMT "rate-limit set {eth1|eth2|eth3|eth4|cpu-port} uplink <rate> {Kb|Mb} downlink <rate> {Kb|Mb}"

/* flow-control 的帮助信息 */
#define CMDHELP_GLB_UNDO_RATE_LIMIT            HELP_FUN "Disable bandwidth limiting for the cnu device." \
						HELP_FMT "undo rate-limit"

/* shutdown 的帮助信息 */
#define CMDHELP_GLB_SHUTDOWN            HELP_FUN "Shutdown cnu port." \
						HELP_FMT "shutdown {eth1|eth2|eth3|eth4}"

/* undo shutdown 的帮助信息 */
#define CMDHELP_GLB_UNDO_SHUTDOWN            HELP_FUN "Undo shutdown for all cnu port." \
						HELP_FMT "undo shutdown"

/* undo mac-limit 的帮助信息 */
#define CMDHELP_GLB_UNDO_MAC_LIMIT            HELP_FUN "Undo cnu bridged mac address number limiting." \
						HELP_FMT "undo mac-limit"

/* undo aging-time 的帮助信息 */
#define CMDHELP_GLB_UNDO_AGING_TIME            HELP_FUN "Undo cnu address table aging time config." \
						HELP_FMT "undo aging-time"

/* undo qos 的帮助信息 */
#define CMDHELP_GLB_UNDO_QOS            HELP_FUN "Undo qos settings." \
						HELP_FMT "undo qos"
						
/*undo的帮助信息 */
#define CMDHELP_GLB_UNDO             "To be default config" 

/*undo shutdown 的帮助信息 */
#define CMDHELP_GLB_UNDO_INTERFACE            HELP_FUN "Disable the cnu port." \
						HELP_SPC "portid	:Port Num to be config."\
						HELP_FMT "shutdown  <portid>"

/*storm-control 的帮助信息 */
#define CMDHELP_GLB_STROM_FILTER            HELP_FUN "Enable or disable frame calculate to storm control." \
						HELP_SPC "The frame rate will be set to 1kpps if storm filter enable."\
						HELP_FMT "storm-filter {broadcast|unknown-unicast|unknown-multicast} {enable|disable}"

/* undo storm-control 的帮助信息 */
#define CMDHELP_GLB_UNDO_STORM_FILTER            HELP_FUN "Disable all frame calculate to storm control." \
						HELP_FMT "undo storm-filter"

/*display storm-filter的帮助信息 */
#define CMDHELP_GLB_DISPLAY_STORM            HELP_FUN "show  storm-filter information." \
						HELP_FMT "display storm-filter"
						
/*display vlan的帮助信息 */
#define CMDHELP_GLB_DISPLAY_VLAN          HELP_FUN "Show the cnu vlan ." \
						HELP_FMT "display vlan"

/*display ratelimit的帮助信息 */
#define CMDHELP_GLB_DISPLAY_RATE          HELP_FUN "Show the cnu port rate limit ." \
						HELP_FMT "display rate-limit"

/*display cur-profile的帮助信息 */
#define CMDHELP_GLB_DISPLAY_CUR_PROFILE          HELP_FUN "Show current configurations of the cnu" \
						HELP_FMT "display cur-profile"
						
/*display vlan的帮助信息 */
#define CMDHELP_GLB_DIS_VLAN          HELP_FUN "Show the management vlan configuration of the system." \
						HELP_FMT "display mgmt-vlan"

/*display 的帮助信息 */
#define CMDHELP_GLB_DIS_TOP           HELP_FUN "Show topology information of the system." \
						HELP_FMT "display topology"

/*display 的帮助信息 */
#define CMDHELP_GLB_DIS_OPT           HELP_FUN "Show operation log of the system." \
						HELP_FMT "display opt-log"

/*display 的帮助信息 */
#define CMDHELP_GLB_DIS_ALARM           HELP_FUN "Show system alarm log." \
						HELP_FMT "display alarm-log"

/*display 的帮助信息 */
#define CMDHELP_GLB_DIS_SYSLOG	HELP_FUN "Show system log." \
						HELP_FMT "display sys-log"

/*display 的帮助信息 */
#define CMDHELP_GLB_DIS_SYSINFO           HELP_FUN "Show the basic information of the system." \
						HELP_FMT "display sysinfo"

/*display 的帮助信息 */
#define CMDHELP_GLB_DIS_FTP           HELP_FUN "Show the ftp server configuration of the system." \
						HELP_FMT "display ftp-server"

/*display 的帮助信息 */
#define CMDHELP_GLB_DIS_NETWORK           HELP_FUN "Show the network settings of the system." \
						HELP_FMT "display network-info"

/*display 的帮助信息 */
#define CMDHELP_GLB_DIS_MGMT          HELP_FUN "Show the management vlan configuration of the system." \
						HELP_FMT "display mgmt-vlan"

/*display 的帮助信息 */
#define CMDHELP_GLB_DIS_USERS           HELP_FUN "Show white list users." \
						HELP_FMT "display user-white-list"

/*display 的帮助信息 */
#define CMDHELP_GLB_DIS_ANONYMOUS           HELP_FUN "Show anonymous' information." \
						HELP_FMT "display anonymous"

/*display 的帮助信息 */
#define CMDHELP_GLB_DIS_PROFLIE           HELP_FUN "Show profile information." \
						HELP_FMT "display profile <pro(u1~256)>"

/*display 的帮助信息 */
#define CMDHELP_GLB_DIS_SNMP           HELP_FUN "Show snmp information." \
						HELP_FMT "display snmp"

/*delete 的帮助信息 */
#define CMDHELP_GLB_DELETE          HELP_FUN "Delete cnu or profile from system."

/*delete cnu 的帮助信息 */
#define CMDHELP_GLB_DELETE_CNU          HELP_FUN "Remove unused cnu from topology." \
						HELP_SPC "index	:cnu index in the topology."\
						HELP_FMT "delete cnu <index>" \
						HELP_FMT "delete cnu 1/1"

/*create cnu 的帮助信息 */
#define CMDHELP_GLB_CREATE_CNU          HELP_FUN "Create a cnu into topology." \
						HELP_SPC "mac	:new cnu mac address, for example 30-71-B2-20-11-01."\
						HELP_FMT "create cnu <mac>"

/*create cnu 的帮助信息 */
#define CMDHELP_GLB_DEBUG_DUMP          HELP_FUN "Debug dump informations." \
						HELP_FMT "dump {register|mod|pib}"
						
/*access 的帮助信息 */
#define CMDHELP_GLB_ACCESS          HELP_FUN "Enable/Disable user access." \
						HELP_FMT "access {enable}|{disable}"

/*send-config 的帮助信息 */
#define CMDHELP_GLB_RELOAD_PROFILE          HELP_FUN "Reload profile for current clt or cnu." \
						HELP_FMT "reload-profile"

/*user 的帮助信息 */
#define CMDHELP_GLB_USER         "Config User."

/*permit 的帮助信息 */
#define CMDHELP_GLB_PERMIT          HELP_FUN "User white list permit." 

/*permit 的帮助信息 */
#define CMDHELP_GLB_PERMIT_USER          HELP_FUN "User white list permit." \
						HELP_SPC "Add a cnu into user white list."\
						HELP_SPC "index	:CNU index in the topology."\
						HELP_FMT "permit cnu <index>" \
						HELP_FMT "permit cnu 1/1"

/*undo permit 的帮助信息 */
#define CMDHELP_GLB_UNDO_PERMIT          HELP_FUN "Undo user white list permit." \
						HELP_SPC "remove a cnu from user white list."\
						HELP_SPC "index	:CNU index in the topology."\
						HELP_FMT "undo permit cnu <index>" \
						HELP_FMT "undo permit cnu 1/1"

/*undo cnu acl drop mme 的帮助信息 */
#define CMDHELP_GLB_UNDO_CNU_ACL          HELP_FUN "Undo acl-drop-mme."
						
/*user del 的帮助信息 */
#define CMDHELP_GLB_USER_DEL          HELP_FUN "del user." \
						HELP_FMT "user del clt <cltid> cnu <cnuid>"

/*user edit 的帮助信息 */
#define CMDHELP_GLB_USER_EDIT          HELP_FUN "edit user." \
						HELP_FMT "user edit clt <cltid> cnu <cnuid> profile <pro>"

/*restore default 的帮助信息 */
#define CMDHELP_GLB_RESTORE          HELP_FUN "Restore the device to factory default." \
						HELP_FMT "restore-default"

/* 命令dsdt-stats 的帮助信息 */
#define CMDHELP_GLB_DSDT_DBG          HELP_FUN  "Show or clear MV88E6171R port statistics. " \
						HELP_FMT "dsdt-stats {print|clear}"

/*dsdt-rgmii-delay 的帮助信息 */
#define CMDHELP_GLB_DSDT_TIMING_DELAY            HELP_FUN "Show or Enable or disable MV88E6171R RGMII timing delay." \
						HELP_FMT "dsdt-rgmii-delay {get|enable|disable} {port5|port6} {rx|tx|all}"

/* dsdt-port-mirror 的帮助信息 */
#define CMDHELP_GLB_DSDT_PORT_MIRROR            HELP_FUN "Enable port mirroring on MV88E6171R." \
						HELP_FMT "dsdt-port-mirror from p6 to p0"

/* dsdt-mac-binding 的帮助信息 */
#define CMDHELP_GLB_DSDT_MAC_BIND            HELP_FUN "Binding a mac address to switch port static." \
						HELP_FMT "dsdt-binding mac-address 01-02-03-04-05-06 to p0~p6"
/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/


#endif

