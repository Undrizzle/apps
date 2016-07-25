#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/syslog.h>
#include <net/if_arp.h>
#include <net/route.h>

#include <cgicmd.h>
#include <cgintwk.h>
#include <cgimain.h>
#include <syscall.h>
#include <httpd.h>
#include <bcmtypes.h>
#include <ifcuiweb.h>
#include <bcmcfm.h>
#include <http2dbs.h>
#include <http2cmm.h>
#include <dbsapi.h>
#include <boardapi.h>
#include <public.h>

extern WEB_NTWK_VAR glbWebVar;

int mapRate2SelectIndex(uint32_t rate)
{
	if( (rate==0)||(rate==(32*0x1fff)) )
	{
		return 0;
	}
	else if( (rate>0)&&(rate<=128) )
	{
		return 1;
	}
	else if( (rate>128)&&(rate<=256) )
	{
		return 2;
	}
	else if( (rate>256)&&(rate<=512) )
	{
		return 3;
	}
	else if( (rate>512)&&(rate<=1024) )
	{
		return 4;
	}
	else if( (rate>1024)&&(rate<=1536) )
	{
		return 5;
	}
	else if( (rate>1536)&&(rate<=2048) )
	{
		return 6;
	}
	else if( (rate>2048)&&(rate<=3072) )
	{
		return 7;
	}
	else if( (rate>3072)&&(rate<=4096) )
	{
		return 8;
	}
	else if( (rate>4096)&&(rate<=6144) )
	{
		return 9;
	}
	else if( rate > 6144 )
	{
		return 10;
	}
	return 0;
}

void cgiNtwkView(char *query, FILE *fs)
{
	st_dbsNetwork row;
	
	dbsGetNetwork(dbsdev, 1, &row);
	
	fprintf(fs, "<html>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'/>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'/>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");
	
	fprintf(fs, "function frmLoad(){\n");
	fprintf(fs, "	var ivalue;\n");
	fprintf(fs, "	var ists;\n");
	fprintf(fs, "	$('#ethIpAddress').val('%s');\n", row.col_ip);
	fprintf(fs, "	$('#ethSubnetMask').val('%s');\n", row.col_netmask);
	fprintf(fs, "	$('#ethGateway').val('%s');\n", row.col_gw);
	fprintf(fs, "	$('#ethaddr').val('%s');\n", row.col_mac);
	fprintf(fs, "	$('#ethaddr').attr('disabled', 'disabled');\n");
	if( 0 == row.col_mvlan_sts )
	{
		fprintf(fs, "	hideMgmtVlanInfo(true);\n");
		fprintf(fs, "	$('#enblMgmtVlan').attr('checked', false);\n");
		fprintf(fs, "	$('#vlanid').val(1);\n");
	}
	else
	{
		fprintf(fs, "	hideMgmtVlanInfo(false);\n");
		fprintf(fs, "	$('#enblMgmtVlan').attr('checked', true);\n");
		fprintf(fs, "	$('#vlanid').val(%d);\n", row.col_mvlan_id);
	}
	fprintf(fs, "}\n");
	
	fprintf(fs, "function hideMgmtVlanInfo(hide){\n");
	fprintf(fs, "	if( hide ) $('#mgmtVlanInfo').hide();\n");
	fprintf(fs, "	else $('#mgmtVlanInfo').show();\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "function mgmtVlanCbClick(){\n");
	fprintf(fs, "	if ( $('#enblMgmtVlan').is(':checked') == true ) hideMgmtVlanInfo(false);\n");
	fprintf(fs, "	else hideMgmtVlanInfo(true);\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "function btnSave(){\n");
	fprintf(fs, "	var ivalue;\n");
	fprintf(fs, "	var loc = 'ntwkcfg.cgi?';\n");
	fprintf(fs, "	ivalue = $('#ethIpAddress').val();\n");
	fprintf(fs, "	if ( isValidIpAddress(ivalue) == false ){\n");
	fprintf(fs, "		alert('IP地址 \"' + ivalue + '\" 输入不正确！');\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, "	}else loc += 'wecIpaddr=' + ivalue;\n");
	fprintf(fs, "	ivalue = $('#ethSubnetMask').val();\n");
	fprintf(fs, "	if ( isValidSubnetMask(ivalue) == false ){\n");
	fprintf(fs, "		alert('子网掩码 \"' + ivalue + '\" 输入不正确！');\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, " 	}else loc += '&wecNetmask=' + ivalue;\n");
	fprintf(fs, "	ivalue = $('#ethGateway').val();\n");
	fprintf(fs, "	if ( isValidGwAddress(ivalue) == false ){\n");
	fprintf(fs, "		alert('网关地址 \"' + ivalue + '\" 输入不正确！');\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, "	}else loc += '&wecDefaultGw=' + ivalue;\n");
	fprintf(fs, "	ivalue = $('#vlanid').val();\n");
	fprintf(fs, "	if ( enblMgmtVlan.checked == true ){\n");
	fprintf(fs, "		loc += '&wecMgmtVlanSts=1';\n");
	fprintf(fs, "		if ( isValidVlanId(ivalue) == false ) return;\n");
	fprintf(fs, "		else loc += '&wecMgmtVlanId=' + ivalue;\n");
	fprintf(fs, "	}else{\n");
	fprintf(fs, "		loc += '&wecMgmtVlanSts=0';\n");
	fprintf(fs, "		loc += '&wecMgmtVlanId=1';\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "$(function(){\n");
	fprintf(fs, "	frmLoad();\n");
	fprintf(fs, "	$('#accordion').accordion({\n");
	fprintf(fs, "		collapsible: true,\n");
	fprintf(fs, "		heightStyle: 'content'\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#dialog').dialog({\n");
	fprintf(fs, "		autoOpen: false,\n");
	fprintf(fs, "		width:410,\n");
	fprintf(fs, "		show:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: 'blind',\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "		hide:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: 'explode',\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#opener').button(\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			icons:\n");
	fprintf(fs, "			{\n");
	fprintf(fs, "				primary: 'ui-icon-help'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "			text: false\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	).click(function(){\n");
	fprintf(fs, "		$('#dialog').dialog('open');\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#comBtn').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-arrowthickstop-1-s'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{
		fprintf(fs, "	$('#comBtn').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
	}
	else
	{
		fprintf(fs, "	$('#comBtn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		btnSave();\n");
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "});\n");
	
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "	<tr><td class='maintitle'><b>网络设置</b></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0 width='100%'>\n");
	fprintf(fs, "	<tr><td class='mainline' width='100%'></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<div id='accordion'>\n");
	fprintf(fs, "	<h3>网络连接设置</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata' width=260>IP地址</td>\n");
	fprintf(fs, "				<td class='diagdata' width=300><input type='text' id='ethIpAddress'></td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>子网掩码</td>\n");
	fprintf(fs, "				<td class='diagdata'><input type='text' id='ethSubnetMask'></td>\n");
	fprintf(fs, "			</tr>\n");	
	fprintf(fs, "				<td class='diagdata'>网关</td>\n");
	fprintf(fs, "				<td class='diagdata'><input type='text' id='ethGateway'></td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>MAC地址</td>\n");
	fprintf(fs, "				<td class='diagdata'><input type='text' id='ethaddr'></td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata' colspan=2>&nbsp;</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'><input type='checkbox' id='enblMgmtVlan' onClick='mgmtVlanCbClick()'></td>\n");
	fprintf(fs, "				<td class='diagdata'>启用管理VLAN</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "		</table>\n");

	fprintf(fs, "		<div id='mgmtVlanInfo'>\n");
	fprintf(fs, "			<table border=0 cellpadding=0 cellspacing=0>	\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata' width=260>管理VLAN-ID</td>\n");
	fprintf(fs, "					<td class='diagdata' width=300><input type='text' id='vlanid'></td>\n");
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "			</table>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "</div>\n");	
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<button id='comBtn'>确 定</button>\n");
	fprintf(fs, "	<button id='opener'>帮助</button>\n");
	fprintf(fs, "</p>\n");
	
	fprintf(fs, "<div id='dialog' title='帮助信息'>\n");
	fprintf(fs, "	通过本页面，您可以设置本机的管理IP地址以及管理VLAN。</br>\n");
	fprintf(fs, "	<br>注意：</br>\n");
	fprintf(fs, "	1. 请确保修改后的配置保存至非易失性存储器；</br>\n");
	fprintf(fs, "	2. 网络配置参数需要在系统重启之后才能生效；</br>\n");
	fprintf(fs, "	3. 设置生效之后请以新的IP地址以及指定的管理VLAN通道访问设备。</br>	\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "</blockquote>\n");
	fprintf(fs, "</body>\n");
	fprintf(fs, "</html>\n");
	
	fflush(fs);	
}

void cgiPortPropetyView(char *query, FILE *fs)
{
	int id = 0;
	char action[IFC_LARGE_LEN];
	T_CMM_PORT_PROPETY_INFO propety;

	cgiGetValueByName(query, "portid", action);
	id = atoi(action);

	bzero((void *)&propety, sizeof(T_CMM_PORT_PROPETY_INFO));
	http2cmm_getPortPropety(id, &propety);	

	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");
	fprintf(fs, "function btnReturn(){\n");
	fprintf(fs, "	var loc = 'wecPortPropety.cmd';\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function btnSave(portid){\n");
	fprintf(fs, "	var loc = 'portPropety.cmd?portid=%d';\n", id);	
	fprintf(fs, "	if( $(\"#portStatus\").val() == 0 ){\n");
	fprintf(fs, "		if(!confirm('警告：端口即将关闭！')) return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "$(function()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	$(\"#portSpeed\")[0].selectedIndex = %d\n", propety.speed);
	fprintf(fs, "	$(\"#portDuplex\")[0].selectedIndex = %d\n", propety.duplex);
	fprintf(fs, "	$(\"#portPri\")[0].selectedIndex = %d\n", propety.pri);
	fprintf(fs, "	$(\"#portFlowControl\")[0].selectedIndex = %d\n", propety.flowControl);
	fprintf(fs, "	$(\"#portStatus\")[0].selectedIndex = %d\n", propety.portStatus);
	fprintf(fs, "	$(\"#accordion\").accordion({\n");
	fprintf(fs, "		collapsible: true,\n");
	fprintf(fs, "		heightStyle: 'content'\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#retBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-carat-1-w'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$(\"#retBtn\" ).button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnReturn();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#saveBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-disk'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{	
		fprintf(fs, "	$('#saveBtn').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
	}
	else
	{
		fprintf(fs, "	$( \"#saveBtn\" ).button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		btnSave();\n");
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "});\n");
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "	<br>\n");
	fprintf(fs, "	<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "<tr><td class='maintitle'><b>%s端口属性</b></td></tr>\n", boardapi_getDsdtPortName(id));
	fprintf(fs, "	</table>\n");
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=0 width=100%>\n");
	fprintf(fs, "		<tr><td class='mainline' width=100%></td></tr>\n");
	fprintf(fs, "	</table>\n");
	fprintf(fs, "	<br><br>\n");
	fprintf(fs, "	<div id=\"accordion\">\n");
	fprintf(fs, "		<h3>端口属性参数</h3>\n");
	fprintf(fs, "		<div>\n");
	fprintf(fs, "			<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata' width=300>端口连接速度</td>\n");
	fprintf(fs, "					<td class='diagdata' width=260>\n");
	fprintf(fs, "						<select id=portSpeed style='WIDTH: 150px'>\n");
	fprintf(fs, "							<option value=0>自协商</option>\n");
	fprintf(fs, "							<option value=1>10Mbps</option>\n");
	fprintf(fs, "							<option value=2>100Mbps</option>\n");
	fprintf(fs, "							<option value=3>1000Mbps</option>\n");
	fprintf(fs, "						</select>\n");
	fprintf(fs, "					</td>\n");
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata'>端口双工状态</td>\n");
	fprintf(fs, "					<td class='diagdata'>\n");
	fprintf(fs, "						<select id=portDuplex style='WIDTH: 150px'>\n");
	fprintf(fs, "							<option value=0>自协商</option>\n");
	fprintf(fs, "							<option value=1>半双工</option>\n");
	fprintf(fs, "							<option value=2>全双工</option>\n");
	fprintf(fs, "						</select>\n");
	fprintf(fs, "					</td>\n");
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata'>端口优先级</td>\n");
	fprintf(fs, "					<td class='diagdata'>\n");
	fprintf(fs, "						<select id=portPri style='WIDTH: 150px'>\n");
	fprintf(fs, "							<option value=0>0</option>\n");
	fprintf(fs, "							<option value=1>1</option>\n");
	fprintf(fs, "							<option value=2>2</option>\n");
	fprintf(fs, "							<option value=3>3</option>\n");
	fprintf(fs, "							<option value=4>4</option>\n");
	fprintf(fs, "							<option value=5>5</option>\n");
	fprintf(fs, "							<option value=6>6</option>\n");
	fprintf(fs, "							<option value=7>7</option>\n");
	fprintf(fs, "						</select>\n");
	fprintf(fs, "					</td>\n");
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata'>流控</td>\n");
	fprintf(fs, "					<td class='diagdata'>\n");
	fprintf(fs, "						<select id=portFlowControl style='WIDTH: 150px'>\n");
	fprintf(fs, "							<option value=0>禁用</option>\n");
	fprintf(fs, "							<option value=1>启用</option>\n");
	fprintf(fs, "						</select>\n");
	fprintf(fs, "					</td>\n");
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata'>端口状态</td>\n");
	fprintf(fs, "					<td class='diagdata'>\n");
	fprintf(fs, "						<select id=portStatus style='WIDTH: 150px'>\n");
	fprintf(fs, "							<option value=0>禁用</option>\n");
	fprintf(fs, "							<option value=1>启用</option>\n");
	fprintf(fs, "						</select>\n");
	fprintf(fs, "					</td>\n");
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata' colspan=2>&nbsp;</td>\n");
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "			</table>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "	<p>\n");
	fprintf(fs, "		<button id='retBtn'>返 回</button>\n");
	fprintf(fs, "		<button id='saveBtn'>确 定</button>\n");
	fprintf(fs, "	</p>\n");
	fprintf(fs, "</blockquote>\n");
	fprintf(fs, "</body>\n");
	fprintf(fs, "</html>\n");
	
	fflush(fs);
}

void cgiPortPropetyViewAll(char *query, FILE *fs)
{
	int i = 0;
	int iStyle = 0;
	T_CMM_PORT_PROPETY_INFO propety[CBAT_SW_PORT_NUM];	

	bzero(propety, sizeof(T_CMM_PORT_PROPETY_INFO)*CBAT_SW_PORT_NUM);
	http2cmm_getPortPropetyAll(propety);

	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script src='util.js'></script>\n");

	fprintf(fs, "<script language='javascript'>\n");
	fprintf(fs, "function btnConfig(portid){\n");
	fprintf(fs, "	var loc = 'portPropety.cmd?';\n");
	fprintf(fs, "	loc += 'portid=' + portid;\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	//fprintf(fs, "	\n");
	
	fprintf(fs, "function btnRefresh(){\n");
	fprintf(fs, "	var code = 'wecPortPropety.cmd';\n");
	fprintf(fs, "	location.href = code;\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "$(function()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	$( '.configBtn' ).button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-home'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#refreshBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-refresh'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$( '#refreshBtn' ).click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnRefresh();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "});\n");
	
	fprintf(fs, "</script>\n");
	
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "	<br>\n");
	fprintf(fs, "	<table border='0' cellpadding='5' cellspacing='0'>\n");
	fprintf(fs, "		<tr><td class='maintitle'><b>端口属性</b></td></tr>\n");	
	fprintf(fs, "	</table>\n");	
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=0 width=100%>\n");
	fprintf(fs, "		<tr><td class='mainline' width=100%></td></tr>\n");
	fprintf(fs, "	</table>\n");	
	fprintf(fs, "	<br><br>\n");
	fprintf(fs, "	通过该页面，您可以查看及配置本机以太网以及同轴端口的属性，包括：速率、双工状态、端口流控以及端口状态等属性参数。</br>\n");
	fprintf(fs, "	<br>\n");
	fprintf(fs, "	<ul>\n");
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=1>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class=thead width=120>端口</td>\n");
	fprintf(fs, "			<td class=thead width=120>连接状态</td>\n");
	fprintf(fs, "			<td class=thead width=120>速度/双工</td>\n");
	fprintf(fs, "			<td class=thead width=50>优先级</td>\n");
	fprintf(fs, "			<td class=thead width=100>流控</td>\n");
	fprintf(fs, "			<td class=thead width=100>端口状态</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td colspan=6>&nbsp;</td>\n");
	fprintf(fs, "		</tr>\n");

	for( i=0; i<CBAT_SW_PORT_NUM; i++)
	{
		if( !boardapi_isDsdtPortValid(i) ) continue;
		iStyle++;		
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td class=%s>\n", (iStyle%2==0)?"cola":"colb");
		fprintf(fs, "			<button class='configBtn' onClick=btnConfig(%d) style='width:120;'>%s</button>\n", i, boardapi_getDsdtPortName(i));
		fprintf(fs, "		</td>\n");
		if( propety[i].linkStatus )
		{
			fprintf(fs, "	<td class='%s'><font color=green>已连接</font></td>\n", (iStyle%2==0)?"cola":"colb");
		}
		else
		{
			fprintf(fs, "	<td class='%s'><font color=red>未连接</font></td>\n", (iStyle%2==0)?"cola":"colb");
		}
		if( 0 == propety[i].speed )
		{
			if( 0 == propety[i].duplex )
			{
				fprintf(fs, "	<td class='%s'>自动/自动</td>\n", (iStyle%2==0)?"cola":"colb");
			}
			else if( 1 == propety[i].duplex )
			{
				fprintf(fs, "	<td class='%s'>自动/半双工</td>\n", (iStyle%2==0)?"cola":"colb");
			}
			else
			{
				fprintf(fs, "	<td class='%s'>自动/全双工</td>\n", (iStyle%2==0)?"cola":"colb");
			}
		}
		else if( 1 == propety[i].speed )
		{
			if( 0 == propety[i].duplex )
			{
				fprintf(fs, "	<td class='%s'>10M/自动</td>\n", (iStyle%2==0)?"cola":"colb");
			}
			else if( 1 == propety[i].duplex )
			{
				fprintf(fs, "	<td class='%s'>10M/半双工</td>\n", (iStyle%2==0)?"cola":"colb");
			}
			else
			{
				fprintf(fs, "	<td class='%s'>10M/全双工</td>\n", (iStyle%2==0)?"cola":"colb");
			}
		}
		else if( 2 == propety[i].speed )
		{
			if( 0 == propety[i].duplex )
			{
				fprintf(fs, "	<td class='%s'>100M/自动</td>\n", (iStyle%2==0)?"cola":"colb");
			}
			else if( 1 == propety[i].duplex )
			{
				fprintf(fs, "	<td class='%s'>100M/半双工</td>\n", (iStyle%2==0)?"cola":"colb");
			}
			else
			{
				fprintf(fs, "	<td class='%s'>100M/全双工</td>\n", (iStyle%2==0)?"cola":"colb");
			}
		}
		else
		{
			if( 0 == propety[i].duplex )
			{
				fprintf(fs, "	<td class='%s'>1000M/自动</td>\n", (iStyle%2==0)?"cola":"colb");
			}
			else if( 1 == propety[i].duplex )
			{
				fprintf(fs, "	<td class='%s'>1000M/半双工</td>\n", (iStyle%2==0)?"cola":"colb");
			}
			else
			{
				fprintf(fs, "	<td class='%s'>1000M/全双工</td>\n", (iStyle%2==0)?"cola":"colb");
			}
		}
		fprintf(fs, "		<td class='%s'>%d</td>\n", (iStyle%2==0)?"cola":"colb", propety[i].pri);
		fprintf(fs, "		<td class='%s'>%s</td>\n", (iStyle%2==0)?"cola":"colb", propety[i].flowControl?"启用":"禁用");
		fprintf(fs, "		<td class='%s'>%s</td>\n", (iStyle%2==0)?"cola":"colb", propety[i].portStatus?"开启":"关闭");		
		fprintf(fs, "	</tr>\n");
	}

	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td colspan=6>&nbsp;</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class=listend colspan=6></td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "	</table>\n");
	fprintf(fs, "	<br>\n");	
	fprintf(fs, "	</ul>\n");	
	fprintf(fs, "	<p>\n");
	fprintf(fs, "		<button id='refreshBtn'>刷 新</button>\n");
	fprintf(fs, "	</p>\n");
	fprintf(fs, "</blockquote>\n");
	fprintf(fs, "</body>\n");
	fprintf(fs, "</html>\n");
	
	fflush(fs);
}

void cgiPortStatsView(char *query, FILE *fs) 
{
	int id = 0;
	char action[IFC_LARGE_LEN];
	T_CMM_PORT_STATS_INFO stats;

	cgiGetValueByName(query, "portid", action);
	id = atoi(action);

	bzero((void *)&stats, sizeof(T_CMM_PORT_STATS_INFO));
	http2cmm_getPortStats(id, &stats);	
	
	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>	\n");
	fprintf(fs, "function btnReturn(){\n");
	fprintf(fs, "	var loc = 'wecPortStats.cmd';\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");

	fprintf(fs, "$(function()\n");
	fprintf(fs, "{\n");	
	fprintf(fs, "	$(\"#accordion\").accordion({\n");
	fprintf(fs, "		collapsible: true,\n");
	fprintf(fs, "		heightStyle: 'content'\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#retBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-carat-1-w'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$(\"#retBtn\" ).button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnReturn();\n");
	fprintf(fs, "	});\n");	
	fprintf(fs, "});\n");
	fprintf(fs, "</script>\n");
	
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "	<br>\n");
	fprintf(fs, "	<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "		<tr><td class='maintitle'><b>%s端口统计</b></td></tr>", boardapi_getDsdtPortName(id));	
	fprintf(fs, "	</table>\n");
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=0 width=100%>\n");
	fprintf(fs, "		<tr><td class='mainline' width=100%></td></tr> \n");
	fprintf(fs, "	</table>\n");
	fprintf(fs, "	<br><br>\n");	
	fprintf(fs, "	<div id=\"accordion\">\n");	
	fprintf(fs, "		<h3>接收报文统计信息</h3>\n");
	fprintf(fs, "		<div>\n");	
	fprintf(fs, "		<table border=0 cellpadding=0 cellspacing=0>\n");	
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata' width=300>Broadcast</td>\n");
	fprintf(fs, "				<td class='diagdata'>%u</td>\n", stats.InBroadcasts);	
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Unicast</td>\n");
	fprintf(fs, "				<td class='diagdata'>%u</td>\n", stats.InUnicasts);	
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Multicast</td>\n");
	fprintf(fs, "				<td class='diagdata'>%u</td>\n", stats.InMulticasts);
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Errors</td>\n");
	fprintf(fs, "				<td class='diagdata'>0</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Runts</td>\n");
	fprintf(fs, "				<td class='diagdata'>0</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Giants</td>\n");
	fprintf(fs, "				<td class='diagdata'>0</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>CRC Errors</td>\n");
	fprintf(fs, "				<td class='diagdata'>0</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Frame</td>\n");
	fprintf(fs, "				<td class='diagdata'>0</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Aborts</td>\n");
	fprintf(fs, "				<td class='diagdata'>0</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Ignored</td>\n");
	fprintf(fs, "				<td class='diagdata'>0</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "		</table>\n");
	fprintf(fs, "		</div>\n");	
	fprintf(fs, "		<h3>发送报文统计信息</h3>\n");
	fprintf(fs, "		<div>\n");	
	fprintf(fs, "		<table border=0 cellpadding=0 cellspacing=0>\n");	
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata' width=300>Broadcast</td>\n");
	fprintf(fs, "				<td class='diagdata'>%u</td>\n", stats.OutBroadcasts);
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Unicast</td>\n");
	fprintf(fs, "				<td class='diagdata'>%u</td>\n", stats.OutUnicasts);	
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Multicast</td>\n");
	fprintf(fs, "				<td class='diagdata'>%u</td>\n", stats.OutMulticasts);
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Errors</td>\n");
	fprintf(fs, "				<td class='diagdata'>0</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Runts</td>\n");
	fprintf(fs, "				<td class='diagdata'>0</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Giants</td>\n");
	fprintf(fs, "				<td class='diagdata'>0</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>CRC Errors</td>\n");
	fprintf(fs, "				<td class='diagdata'>0</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Frame</td>\n");
	fprintf(fs, "				<td class='diagdata'>0</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Aborts</td>\n");
	fprintf(fs, "				<td class='diagdata'>0</td>\n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata'>Ignored</td>\n");
	fprintf(fs, "				<td class='diagdata'>0</td>\n");
	fprintf(fs, "			</tr>\n");	
	fprintf(fs, "		</table>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");

	fprintf(fs, "	<p>\n");
	fprintf(fs, "		<button id='retBtn'>返 回</button>\n");
	fprintf(fs, "	</p>\n");
	fprintf(fs, "</blockquote>\n");
	fprintf(fs, "</body>\n");
	fprintf(fs, "</html>\n");
	
	fflush(fs);
}

void cgiPortStatsViewAll(char *query, FILE *fs)
{
	int i = 0;
	int iStyle = 0;
	T_CMM_PORT_STATS_INFO stats[CBAT_SW_PORT_NUM];	

	bzero(stats, sizeof(T_CMM_PORT_STATS_INFO)*CBAT_SW_PORT_NUM);
	http2cmm_getPortStatsAll(stats);

	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script src='util.js'></script>\n");

	fprintf(fs, "<script language='javascript'>\n");
	fprintf(fs, "function btnClear(){\n");
	fprintf(fs, "	var loc = 'clearPortStas.cgi';\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");	
	fprintf(fs, "function btnRefresh(){\n");
	fprintf(fs, "	var code = 'wecPortStats.cmd';\n");
	fprintf(fs, "	location.href = code;\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function showPortStats(portid){\n");
	fprintf(fs, "	var loc = 'portStatsDetail.cmd?';\n");
	fprintf(fs, "	loc += 'portid=' + portid;\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");	
	fprintf(fs, "$(function()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	$( '.showBtn' ).button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-home'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#refreshBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-refresh'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$( '#refreshBtn' ).click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnRefresh();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#clearBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-shuffle'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{
		fprintf(fs, "	$('#clearBtn').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
	}
	else
	{
		fprintf(fs, "	$( '#clearBtn' ).click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		btnClear();\n");
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "});\n");
	
	fprintf(fs, "</script>\n");
	
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "	<br>\n");
	fprintf(fs, "	<table border='0' cellpadding='5' cellspacing='0'>\n");
	fprintf(fs, "		<tr><td class='maintitle'><b>端口统计</b></td></tr>\n");	
	fprintf(fs, "	</table>\n");	
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=0 width=100%>\n");
	fprintf(fs, "		<tr><td class='mainline' width=100%></td></tr>\n");
	fprintf(fs, "	</table>\n");	
	fprintf(fs, "	<br><br>\n");
	fprintf(fs, "	通过本页面，您可以查询以太网接口以及同轴接口的数据包统计信息。</br>\n");
	fprintf(fs, "	<br>\n");
	fprintf(fs, "	<ul>\n");
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=2>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class=thead width=120>端口</td>\n");
	fprintf(fs, "			<td class=thead width=90>发送数据包</td>\n");
	fprintf(fs, "			<td class=thead width=140>发送字节数</td>\n");
	fprintf(fs, "			<td class=thead width=90>接收数据包</td>\n");
	fprintf(fs, "			<td class=thead width=140>接收字节数</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td colspan=5>&nbsp;</td>\n");
	fprintf(fs, "		</tr>\n");

	for( i=0; i<CBAT_SW_PORT_NUM; i++)
	{
		if( !boardapi_isDsdtPortValid(i) ) continue;
		iStyle++;		
		fprintf(fs, "	<tr>\n");
		fprintf(fs, "		<td class=%s>\n", (iStyle%2==0)?"cola":"colb");
		fprintf(fs, "			<button class='showBtn' onClick=showPortStats(%d) style='width:120;'>%s</button>\n", i, boardapi_getDsdtPortName(i));
		fprintf(fs, "		</td>\n");
		fprintf(fs, "		<td class='%s'>%d</td>\n", (iStyle%2==0)?"cola":"colb", stats[i].txCtr);
		fprintf(fs, "		<td class='%s'>%d</td>\n", (iStyle%2==0)?"cola":"colb", stats[i].OutGoodOctets);
		fprintf(fs, "		<td class='%s'>%d</td>\n", (iStyle%2==0)?"cola":"colb", stats[i].rxCtr);
		fprintf(fs, "		<td class='%s'>%d</td>\n", (iStyle%2==0)?"cola":"colb", stats[i].InGoodOctets);
		fprintf(fs, "	</tr>\n");
	}

	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td colspan=5>&nbsp;</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class=listend colspan=5></td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "	</table>\n");
	fprintf(fs, "	<br>\n");	
	fprintf(fs, "	</ul>\n");	
	fprintf(fs, "	<p>\n");
	fprintf(fs, "		<button id='refreshBtn'>刷 新</button>\n");
	fprintf(fs, "		<button id='clearBtn'>清 除</button>\n");
	fprintf(fs, "	</p>\n");
	fprintf(fs, "</blockquote>\n");
	fprintf(fs, "</body>\n");
	fprintf(fs, "</html>\n");
	
	fflush(fs);
}

void cgiCltProfile(char *query, FILE *fs) 
{
	int id = 0;
	st_dbsCltConf profile;
	char action[IFC_LARGE_LEN];

	cgiGetValueByName(query, "cltid", action);
	id = atoi(action);

	if( CMM_SUCCESS != dbsGetCltconf(dbsdev, id, &profile) )
	{
		/*Error*/
		sprintf(glbWebVar.returnUrl, "cltManagement.cmd?cltid=%d", id);
		glbWebVar.wecOptCode = CMM_FAILED;
		do_ej("/webs/wecOptResult2.html", fs);
		return;
	}

	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'/>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'/>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");
	fprintf(fs, "function btnReturn(){\n");
	fprintf(fs, "	location.href = 'cltManagement.cmd?cltid=%d';\n", id);
	fprintf(fs, "}\n");
	fprintf(fs, "function setAgTime()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	var msg;\n");
	fprintf(fs, "	var value;\n");
	fprintf(fs, "	var loc = 'setCltAgTime.cgi?';\n");
	fprintf(fs, "	loc += 'cltid=%d';\n", id);
	fprintf(fs, "	if ( isNaN(parseInt($('#loagTime').val())) == true )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		msg = '本地桥接地址老化时间 \"' + $('#loagTime').val() + '\" 输入不正确！';\n");
	fprintf(fs, "		alert(msg);\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	value = parseInt($('#loagTime').val());\n");
	fprintf(fs, "	if ( value < 1 || value > 2000 )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		msg = '本地桥接地址老化时间 \"' + value + '\" 超出范围[1-2000]！';\n");
	fprintf(fs, "		alert(msg);\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	if ( isNaN(parseInt($('#reagTime').val())) == true )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		msg = '远程桥接地址老化时间 \"' + $('#reagTime').val() + '\" 输入不正确！';\n");
	fprintf(fs, "		alert(msg);\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	value = parseInt($('#reagTime').val());\n");
	fprintf(fs, "	if ( value < 1 || value > 2000 )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		msg = '远程桥接地址老化时间 \"' + value + '\" 超出范围[1-2000]！';\n");
	fprintf(fs, "		alert(msg);\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	loc += '&col_loagTime=' + $('#loagTime').val();\n");
	fprintf(fs, "	loc += '&col_reagTime=' + $('#reagTime').val();\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");

	fprintf(fs, "function enableQoS()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	var loc = 'enableCltQoS.cgi?cltid=%d';\n", id);
	fprintf(fs, "	loc += '&col_tbaPriSts=' + $('#tbaPriSts').val();\n");		
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "function saveProfile(){\n");
	fprintf(fs, "	var loc = 'saveCltProfile.cgi?cltid=%d';\n", id);
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");

	fprintf(fs, "function setDefaultCap()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	var loc = 'setCltDeCap.cgi?cltid=%d';\n", id);	
	fprintf(fs, "	loc += '&col_igmpPri=' + $('#igmpPri').val();\n");
	fprintf(fs, "	loc += '&col_unicastPri=' + $('#unicastPri').val();\n");
	fprintf(fs, "	loc += '&col_avsPri=' + $('#avsPri').val();\n");
	fprintf(fs, "	loc += '&col_mcastPri=' + $('#mcastPri').val();\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");

	fprintf(fs, "function setQoS()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	var loc = 'setCltQoS.cgi?cltid=%d';\n", id);
	fprintf(fs, "	if( 0 == $('#asPriType').val() )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		loc += '&col_cosPriSts=1';\n");
	fprintf(fs, "		loc += '&col_tosPriSts=0';\n");
	fprintf(fs, "		loc += '&col_cos0pri=' + $('#pri0QueueMap').val();\n");
	fprintf(fs, "		loc += '&col_cos1pri=' + $('#pri1QueueMap').val();\n");
	fprintf(fs, "		loc += '&col_cos2pri=' + $('#pri2QueueMap').val();\n");
	fprintf(fs, "		loc += '&col_cos3pri=' + $('#pri3QueueMap').val();\n");
	fprintf(fs, "		loc += '&col_cos4pri=' + $('#pri4QueueMap').val();\n");
	fprintf(fs, "		loc += '&col_cos5pri=' + $('#pri5QueueMap').val();\n");
	fprintf(fs, "		loc += '&col_cos6pri=' + $('#pri6QueueMap').val();\n");
	fprintf(fs, "		loc += '&col_cos7pri=' + $('#pri7QueueMap').val();\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	else\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		loc += '&col_cosPriSts=0';\n");
	fprintf(fs, "		loc += '&col_tosPriSts=1';\n");
	fprintf(fs, "		loc += '&col_tos0pri=' + $('#pri0QueueMap').val();\n");
	fprintf(fs, "		loc += '&col_tos1pri=' + $('#pri1QueueMap').val();\n");
	fprintf(fs, "		loc += '&col_tos2pri=' + $('#pri2QueueMap').val();\n");
	fprintf(fs, "		loc += '&col_tos3pri=' + $('#pri3QueueMap').val();\n");
	fprintf(fs, "		loc += '&col_tos4pri=' + $('#pri4QueueMap').val();\n");
	fprintf(fs, "		loc += '&col_tos5pri=' + $('#pri5QueueMap').val();\n");
	fprintf(fs, "		loc += '&col_tos6pri=' + $('#pri6QueueMap').val();\n");
	fprintf(fs, "		loc += '&col_tos7pri=' + $('#pri7QueueMap').val();\n");
	fprintf(fs, "	}\n");	
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");	

	fprintf(fs, "function setFrequency()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	var msg;\n");
	fprintf(fs, "	var value;\n");
	fprintf(fs, "	var loc = 'setCltFreq.cgi?';\n");
	fprintf(fs, "	loc += 'cltid=%d';\n", id);
	fprintf(fs, "	if ( isNaN(parseInt($('#stopFreq').val())) == true )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		msg = '结束频段 \"' + $('#stopFreq').val() + '\" 输入不正确.';\n");
	fprintf(fs, "		alert(msg);\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	value = parseInt($('#stopFreq').val());\n");
	fprintf(fs, "	if($('#Freqsts').val() != 0)\n");
	fprintf(fs, "    {\n");
	fprintf(fs, "		if ( value < 30 || value > 65 )\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			msg = '结束频段 \"' + value + '\" 超出范围 [30-65].';\n");
	fprintf(fs, "			alert(msg);\n");
	fprintf(fs, "			return;\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	loc += '&freqsts=' + $('#Freqsts').val();\n");
	fprintf(fs, "	loc += '&stopfreq=' + $('#stopFreq').val();\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");	
	
	fprintf(fs, "function btnSave(eventid)\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	if(1==eventid)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		setAgTime();\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	else if(2==eventid)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		enableQoS();\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	else if(3==eventid)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		setDefaultCap();\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	else if(4==eventid)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		setQoS();\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "    else if(5==eventid)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		setFrequency();\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	else\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		alert('Error: btnSave(null)');\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "}\n");

	fprintf(fs, "function btnDone(){\n");
	fprintf(fs, "	var loc = 'saveCltProfile.cgi?cltid=%d';\n", id);
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "$(function()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	$('#loagTime').val(%d);\n", profile.col_loagTime);
	fprintf(fs, "	$('#reagTime').val(%d);\n", profile.col_reagTime);
	fprintf(fs, "	$('#tbaPriSts').val(%d);\n", profile.col_tbaPriSts?1:0);
	fprintf(fs, "	$('#igmpPri').val(%d);\n", profile.col_igmpPri);
	fprintf(fs, "	$('#unicastPri').val(%d);\n", profile.col_unicastPri);
	fprintf(fs, "	$('#avsPri').val(%d);\n", profile.col_avsPri);
	fprintf(fs, "	$('#mcastPri').val(%d);\n", profile.col_mcastPri);
	fprintf(fs, "	$('#asPriType').val(%d);\n", profile.col_cosPriSts?0:1);
	if( profile.col_cosPriSts )
	{
		fprintf(fs, "	$('#pri0QueueMap').val(%d);\n", profile.col_cos0pri);
		fprintf(fs, "	$('#pri1QueueMap').val(%d);\n", profile.col_cos1pri);
		fprintf(fs, "	$('#pri2QueueMap').val(%d);\n", profile.col_cos2pri);
		fprintf(fs, "	$('#pri3QueueMap').val(%d);\n", profile.col_cos3pri);
		fprintf(fs, "	$('#pri4QueueMap').val(%d);\n", profile.col_cos4pri);
		fprintf(fs, "	$('#pri5QueueMap').val(%d);\n", profile.col_cos5pri);
		fprintf(fs, "	$('#pri6QueueMap').val(%d);\n", profile.col_cos6pri);
		fprintf(fs, "	$('#pri7QueueMap').val(%d);\n", profile.col_cos7pri);
	}
	else
	{
		fprintf(fs, "	$('#pri0QueueMap').val(%d);\n", profile.col_tos0pri);
		fprintf(fs, "	$('#pri1QueueMap').val(%d);\n", profile.col_tos1pri);
		fprintf(fs, "	$('#pri2QueueMap').val(%d);\n", profile.col_tos2pri);
		fprintf(fs, "	$('#pri3QueueMap').val(%d);\n", profile.col_tos3pri);
		fprintf(fs, "	$('#pri4QueueMap').val(%d);\n", profile.col_tos4pri);
		fprintf(fs, "	$('#pri5QueueMap').val(%d);\n", profile.col_tos5pri);
		fprintf(fs, "	$('#pri6QueueMap').val(%d);\n", profile.col_tos6pri);
		fprintf(fs, "	$('#pri7QueueMap').val(%d);\n", profile.col_tos7pri);
	}	
	fprintf(fs, "	$('#Freqsts').val(%d);\n", glbWebVar.freqsts);
	fprintf(fs, "	$('#stopFreq').val(%d);\n", glbWebVar.stopfreq);
	fprintf(fs, "	$('#accordion').accordion({\n");
	fprintf(fs, "		collapsible: true,\n");
	fprintf(fs, "		heightStyle: 'content'\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#dialog').dialog({\n");
	fprintf(fs, "		autoOpen: false,\n");
	fprintf(fs, "		width:400,\n");
	fprintf(fs, "		show:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: 'blind',\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "		hide:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: 'explode',\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#opener').button(\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			icons:\n");
	fprintf(fs, "			{\n");
	fprintf(fs, "				primary: 'ui-icon-help'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "			text: false\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	).click(function(){\n");
	fprintf(fs, "		$('#dialog').dialog('open');\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#returnBtn').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-carat-1-w'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#returnBtn').button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnReturn();\n");
	fprintf(fs, "	});\n");
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{
		fprintf(fs, "	$('#save1Btn').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "   $('#save2Btn').button().click(function(event){\n");
                fprintf(fs, "                   alert('您没有足够的权限...');\n");
                fprintf(fs, "   });\n");
		fprintf(fs, "   $('#save3Btn').button().click(function(event){\n");
                fprintf(fs, "                   alert('您没有足够的权限...');\n");
                fprintf(fs, "   });\n");
		fprintf(fs, "   $('#save4Btn').button().click(function(event){\n");
                fprintf(fs, "                   alert('您没有足够的权限...');\n");
                fprintf(fs, "   });\n");
		fprintf(fs, "   $('#save5Btn').button().click(function(event){\n");
                fprintf(fs, "                   alert('您没有足够的权限...');\n");
                fprintf(fs, "   });\n");


	}
	else
	{
		fprintf(fs, "	$('#save1Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		btnSave(1);\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "	$('#save2Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		btnSave(2);\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "	$('#save3Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		btnSave(3);\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "	$('#save4Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		btnSave(4);\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "	$('#save5Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		btnSave(5);\n");
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "	$('#comBtn').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-disk'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{
		fprintf(fs, "	$('#comBtn').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
	}
	else
	{
		fprintf(fs, "	$('#comBtn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		btnDone();\n");
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "     if($('#Freqsts').val() == 0){\n");
	fprintf(fs, "        $('#stopFreq').attr('disabled', 'disable')\n");
	fprintf(fs, "	 }\n");
	fprintf(fs, "   $('#Freqsts').change(function(){\n");  
	fprintf(fs, "     if($('#Freqsts').val() != 0){\n");  	  
	fprintf(fs, "        $('#stopFreq').removeAttr('disabled');\n");  
	fprintf(fs, "		}\n");
	fprintf(fs, "     if($('#Freqsts').val() == 0){\n"); 
	fprintf(fs, "        $('#stopFreq').val(0)\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "});\n");
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "	<tr><td class='maintitle'><b>CLT参数配置</b></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0 width='100%'>\n");
	fprintf(fs, "	<tr><td class='mainline' width='100%'></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<div id='accordion'>\n");
	fprintf(fs, "	<h3>MAC地址表老化时间</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<div class='dwall'>\n");
	fprintf(fs, "			<div class='dwcontent'>\n");
	fprintf(fs, "				<table class='dwcontent'>\n");
	fprintf(fs, "	  				<tr>\n");
	fprintf(fs, "						<td class='diagdata' width=250>本地桥接MAC地址表老化时间</td>\n");
	fprintf(fs, "						<td class='diagdata' width=250><input type='text' id='loagTime' size='8'>分钟[范围：1~2000]</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>远程桥接MAC地址表老化时间</td>\n");
	fprintf(fs, "						<td class='diagdata'><input type='text' id='reagTime' size='8'>分钟[范围：1~2000]</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "				</table>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "			<div class='dwbtn'><button id='save1Btn'>确 定</button></div>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "	<h3>QoS->全局配置</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<div class='dwall'>\n");
	fprintf(fs, "			<div class='dwcontent'>\n");
	fprintf(fs, "				<table class='dwcontent'>\n");
	fprintf(fs, "	  			<tr>\n");
	fprintf(fs, "						<td class='diagdata' width=300>启用QoS功能</td>\n");
	fprintf(fs, "						<td class='diagdata' width=200>\n");
	fprintf(fs, "							<select id='tbaPriSts' size=1>\n");
	fprintf(fs, "								<option value='0'>禁用</option>\n");
	fprintf(fs, "								<option value='1'>启用</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "				</table>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "			<div class='dwbtn'><button id='save2Btn'>确 定</button></div>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "	<h3>QoS->业务报文优先级设置</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<div class='dwall'>\n");
	fprintf(fs, "			<div class='dwcontent'>\n");
	fprintf(fs, "				<table class='dwcontent'>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata' width=300>组播协议报文优先级</td>\n");
	fprintf(fs, "						<td class='diagdata' width=200>\n");
	fprintf(fs, "							<select id='igmpPri' size=1>\n");
	fprintf(fs, "								<option value='0'>CAP0</option>\n");
	fprintf(fs, "								<option value='1'>CAP1</option>\n");
	fprintf(fs, "								<option value='2'>CAP2</option>\n");
	fprintf(fs, "								<option value='3'>CAP3</option>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>单播报文优先级</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='unicastPri' size=1>\n");
	fprintf(fs, "								<option value='0'>CAP0</option>\n");
	fprintf(fs, "								<option value='1'>CAP1</option>\n");
	fprintf(fs, "								<option value='2'>CAP2</option>\n");
	fprintf(fs, "								<option value='3'>CAP3</option>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>多播流优先级</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='avsPri' size=1>\n");
	fprintf(fs, "								<option value='0'>CAP0</option>\n");
	fprintf(fs, "								<option value='1'>CAP1</option>\n");
	fprintf(fs, "								<option value='2'>CAP2</option>\n");
	fprintf(fs, "								<option value='3'>CAP3</option>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>广播报文优先级</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='mcastPri' size=1>\n");
	fprintf(fs, "								<option value='0'>CAP0</option>\n");
	fprintf(fs, "								<option value='1'>CAP1</option>\n");
	fprintf(fs, "								<option value='2'>CAP2</option>\n");
	fprintf(fs, "								<option value='3'>CAP3</option>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "				</table>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "			<div class='dwbtn'><button id='save3Btn'>确 定</button></div>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "	<h3>QoS->流优先级映射</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<div class='dwall'>\n");
	fprintf(fs, "			<div class='dwcontent'>\n");
	fprintf(fs, "				<table class='dwcontent'>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata' width=300>流优先级映射类型</td>\n");
	fprintf(fs, "						<td class='diagdata' width=200>\n");
	fprintf(fs, "							<select id='asPriType' size=1>\n");
	fprintf(fs, "								<option value='0'>COS</option>\n");
	fprintf(fs, "								<option value='1'>TOS</option>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata' colspan=2></td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>COS0/TOS0:</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='pri0QueueMap' size=1>\n");
	fprintf(fs, "								<option value='0'>CAP0</option>\n");
	fprintf(fs, "								<option value='1'>CAP1</option>\n");
	fprintf(fs, "								<option value='2'>CAP2</option>\n");
	fprintf(fs, "								<option value='3'>CAP3</option>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>COS1/TOS1:</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='pri1QueueMap' size=1>\n");
	fprintf(fs, "								<option value='0'>CAP0</option>\n");
	fprintf(fs, "								<option value='1'>CAP1</option>\n");
	fprintf(fs, "								<option value='2'>CAP2</option>\n");
	fprintf(fs, "								<option value='3'>CAP3</option>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>COS2/TOS2:</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='pri2QueueMap' size=1>\n");
	fprintf(fs, "								<option value='0'>CAP0</option>\n");
	fprintf(fs, "								<option value='1'>CAP1</option>\n");
	fprintf(fs, "								<option value='2'>CAP2</option>\n");
	fprintf(fs, "								<option value='3'>CAP3</option>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>COS3/TOS3:</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='pri3QueueMap' size=1>\n");
	fprintf(fs, "								<option value='0'>CAP0</option>\n");
	fprintf(fs, "								<option value='1'>CAP1</option>\n");
	fprintf(fs, "								<option value='2'>CAP2</option>\n");
	fprintf(fs, "								<option value='3'>CAP3</option>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>COS4/TOS4:</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='pri4QueueMap' size=1>\n");
	fprintf(fs, "								<option value='0'>CAP0</option>\n");
	fprintf(fs, "								<option value='1'>CAP1</option>\n");
	fprintf(fs, "								<option value='2'>CAP2</option>\n");
	fprintf(fs, "								<option value='3'>CAP3</option>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>COS5/TOS5:</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='pri5QueueMap' size=1>\n");
	fprintf(fs, "								<option value='0'>CAP0</option>\n");
	fprintf(fs, "								<option value='1'>CAP1</option>\n");
	fprintf(fs, "								<option value='2'>CAP2</option>\n");
	fprintf(fs, "								<option value='3'>CAP3</option>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>COS6/TOS6:</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='pri6QueueMap' size=1>\n");
	fprintf(fs, "								<option value='0'>CAP0</option>\n");
	fprintf(fs, "								<option value='1'>CAP1</option>\n");
	fprintf(fs, "								<option value='2'>CAP2</option>\n");
	fprintf(fs, "								<option value='3'>CAP3</option>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>COS7/TOS7:</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='pri7QueueMap' size=1>\n");
	fprintf(fs, "								<option value='0'>CAP0</option>\n");
	fprintf(fs, "								<option value='1'>CAP1</option>\n");
	fprintf(fs, "								<option value='2'>CAP2</option>\n");
	fprintf(fs, "								<option value='3'>CAP3</option>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "				</table>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "			<div class='dwbtn'><button id='save4Btn'>确 定</button></div>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "	<h3>CLT 工作频段</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<div class='dwall'>\n");
	fprintf(fs, "			<div class='dwcontent'>\n");
	fprintf(fs, "				<table class='dwcontent'>\n");
	fprintf(fs, "	  				<tr>\n");
	fprintf(fs, "						<td class='diagdata' width=300>RF频段选择:</td>\n");
	fprintf(fs, "						<td class='diagdata' width=200>\n");
	fprintf(fs, "							<select id='Freqsts' size=1>\n");
	fprintf(fs, "								<option value='0'>禁用</option>\n");
	fprintf(fs, "								<option value='1'>启用</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "	  				<tr>\n");
	fprintf(fs, "						<td class='diagdata' width=250>RF起始频段:</td>\n");
	fprintf(fs, "						<td class='diagdata' width=250> 5MHz</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>RF结束频段:</td>\n");
	fprintf(fs, "						<td class='diagdata' width=250><input type='text' id='stopFreq' size='8'>MHz [Range of 30~65]</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "				</table>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "			<div class='dwbtn'><button id='save5Btn'>确定</button></div>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<button id='returnBtn'>返 回</button>\n");
	fprintf(fs, "	<button id='comBtn'>保 存</button>\n");
	fprintf(fs, "	<button id='opener'>帮助</button>\n");
	fprintf(fs, "</p>\n");
	fprintf(fs, "<div id='dialog' title='Help Information'>\n");
	fprintf(fs, "	通过本页面，您可以完成对CLT调制模块的参数配置</br>\n");
	fprintf(fs, "	<br>1. 点击 '确定' 按钮修改相应的参数；\n");
	fprintf(fs, "	<br>2. 点击 '保存' 按钮将所有修改的参数存储至数据库。\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "</blockquote>\n");
	fprintf(fs, "</body>\n");
	fprintf(fs, "</html>\n");
	
	//fprintf(fs, "\n");
	fflush(fs);
}

void cgiCnuProfile(char *query, FILE *fs) 
{
	int id = 0;
	int cltid = 0;
   	int cnuid = 0;
	st_dbsProfile profile;
	char action[IFC_LARGE_LEN];

	cgiGetValueByName(query, "cnuid", action);
	id = atoi(action);
	if( CMM_SUCCESS != dbsGetProfile(dbsdev, id, &profile) )
	{
		/*Error*/
		sprintf(glbWebVar.returnUrl, "cnuManagement.cmd?cnuid=%d", id);
		glbWebVar.wecOptCode = CMM_FAILED;
		do_ej("/webs/wecOptResult2.html", fs);
		return;
	}
	cltid = (id-1)/MAX_CNUS_PER_CLT+1;
	cnuid = (id-1)%MAX_CNUS_PER_CLT+1;

	fprintf(fs, "<html>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'/>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'/>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");
	
	fprintf(fs, "function frmLoad() {\n");	
	fprintf(fs, "	$('#macLimitSts').val(%d);\n", profile.col_macLimit?1:0);
	fprintf(fs, "	$('#macLimitNum').val(%d);\n", ((profile.col_macLimit==65)||(profile.col_macLimit>8))?0:profile.col_macLimit);
	fprintf(fs, "	$('#loagTime').val(%d);\n", profile.col_loagTime);
	fprintf(fs, "	$('#reagTime').val(%d);\n", profile.col_reagTime);
	fprintf(fs, "	$('#sfbSts').val(%d);\n", profile.col_sfbSts?1:0);
	fprintf(fs, "	$('#sfuSts').val(%d);\n", profile.col_sfuSts?1:0);
	fprintf(fs, "	$('#sfmSts').val(%d);\n", profile.col_sfmSts?1:0);
	fprintf(fs, "	$('#vlanSts').val(%d);\n", profile.col_vlanSts?1:0);
	fprintf(fs, "	$('#eth1vid').val(%d);\n", profile.col_eth1vid);
	fprintf(fs, "	$('#eth2vid').val(%d);\n", profile.col_eth2vid);
	fprintf(fs, "	$('#eth3vid').val(%d);\n", profile.col_eth3vid);
	fprintf(fs, "	$('#eth4vid').val(%d);\n", profile.col_eth4vid);	
	if( 0 == profile.col_rxLimitSts )
	{
		fprintf(fs, "	$('#cpuPortRx').val(0);\n");
		fprintf(fs, "	$('#eth1rx').val(0);\n");
		fprintf(fs, "	$('#eth2rx').val(0);\n");
		fprintf(fs, "	$('#eth3rx').val(0);\n");
		fprintf(fs, "	$('#eth4rx').val(0);\n");
	}
	else
	{
		fprintf(fs, "	$('#cpuPortRx').val(%d);\n", mapRate2SelectIndex(profile.col_cpuPortRxRate));
		fprintf(fs, "	$('#eth1rx').val(%d);\n", mapRate2SelectIndex(profile.col_eth1rx));
		fprintf(fs, "	$('#eth2rx').val(%d);\n", mapRate2SelectIndex(profile.col_eth2rx));
		fprintf(fs, "	$('#eth3rx').val(%d);\n", mapRate2SelectIndex(profile.col_eth3rx));
		fprintf(fs, "	$('#eth4rx').val(%d);\n", mapRate2SelectIndex(profile.col_eth4rx));
	}	
	if( 0 == profile.col_txLimitSts )
	{
		fprintf(fs, "	$('#cpuPortTx').val(0);\n");
		fprintf(fs, "	$('#eth1tx').val(0);\n");
		fprintf(fs, "	$('#eth2tx').val(0);\n");
		fprintf(fs, "	$('#eth3tx').val(0);\n");
		fprintf(fs, "	$('#eth4tx').val(0);\n");
	}
	else
	{
		fprintf(fs, "	$('#cpuPortTx').val(%d);\n", mapRate2SelectIndex(profile.col_cpuPortTxRate));
		fprintf(fs, "	$('#eth1tx').val(%d);\n", mapRate2SelectIndex(profile.col_eth1tx));
		fprintf(fs, "	$('#eth2tx').val(%d);\n", mapRate2SelectIndex(profile.col_eth2tx));
		fprintf(fs, "	$('#eth3tx').val(%d);\n", mapRate2SelectIndex(profile.col_eth3tx));
		fprintf(fs, "	$('#eth4tx').val(%d);\n", mapRate2SelectIndex(profile.col_eth4tx));
	}	
	fprintf(fs, "	$('#eth1sts').val(%d);\n", profile.col_eth1sts?1:0);
	fprintf(fs, "	$('#eth2sts').val(%d);\n", profile.col_eth2sts?1:0);
	fprintf(fs, "	$('#eth3sts').val(%d);\n", profile.col_eth3sts?1:0);
	fprintf(fs, "	$('#eth4sts').val(%d);\n", profile.col_eth4sts?1:0);	
	fprintf(fs, "}\n");

	fprintf(fs, "function btnReturn(){\n");
	fprintf(fs, "	var loc = 'cnuManagement.cmd?cnuid=';\n");
	fprintf(fs, "	loc += %d;\n", id);
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "function setMacLimiting() {\n");
	fprintf(fs, "	var loc = 'macLimit.cgi?';\n");
	fprintf(fs, "	loc += 'cnuid=%d';\n", id);	
	fprintf(fs, "	if($('#macLimitSts').val() == 0){\n");
	fprintf(fs, "		loc += '&col_macLimit=0';\n");	
	fprintf(fs, "	}else{\n");
	fprintf(fs, "		if( $('#macLimitNum').val() == 0 ){\n");
	fprintf(fs, "			loc += '&col_macLimit=65';\n");
	fprintf(fs, "		}else{\n");
	fprintf(fs, "			loc += '&col_macLimit=' + $('#macLimitNum').val();\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	}\n");	
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "function setAgTime()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	var msg;\n");
	fprintf(fs, "	var value;\n");
	fprintf(fs, "	var loc = 'setAgTime.cgi?';\n");
	fprintf(fs, "	loc += 'cnuid=%d';\n", id);
	fprintf(fs, "	value = parseInt($('#loagTime').val());\n");
	fprintf(fs, "	if ( isNaN(value) == true )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		msg = '本地桥接地址老化时间 \"' + $('#loagTime').val() + '\" 输入不正确！';\n");
	fprintf(fs, "		alert(msg);\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, "	}\n");	
	fprintf(fs, "	if ( value < 1 || value > 2000 )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		msg = '本地桥接地址老化时间 \"' + value + '\" 超出范围[1-2000]！';\n");
	fprintf(fs, "		alert(msg);\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	value = parseInt($('#reagTime').val());\n");
	fprintf(fs, "	if ( isNaN(value) == true )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		msg = '远程桥接地址老化时间 \"' + value + '\" 输入不正确！';\n");
	fprintf(fs, "		alert(msg);\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, "	}\n");	
	fprintf(fs, "	if ( value < 1 || value > 2000 )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		msg = '远程桥接地址老化时间 \"' + value + '\" 超出范围[1-2000]！';\n");
	fprintf(fs, "		alert(msg);\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	loc += '&col_loagTime=' + $('#loagTime').val();\n");
	fprintf(fs, "	loc += '&col_reagTime=' + $('#reagTime').val();\n");	
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "function setSFilter(sts)\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	var loc = 'setSFilter.cgi?cnuid=%d';\n", id);	
	fprintf(fs, "	if(0 == sts)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		loc += '&col_sfbSts=0';\n");
	fprintf(fs, "		loc += '&col_sfuSts=0';\n");
	fprintf(fs, "		loc += '&col_sfmSts=0';\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	else\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		loc += '&col_sfbSts=' + $('#sfbSts').val();\n");
	fprintf(fs, "		loc += '&col_sfuSts=' + $('#sfuSts').val();\n");
	fprintf(fs, "		loc += '&col_sfmSts=' + $('#sfmSts').val();\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "function setVlan(sts){\n");
	fprintf(fs, "	var loc = 'setCnuVlan.cgi?cnuid=%d';\n", id);	
	fprintf(fs, "	if( (0==sts) || ($('#vlanSts').val()==0)){\n");
	fprintf(fs, "		$('#vlanSts').val(0);\n");
	fprintf(fs, "		$('#eth1vid').val(1);\n");
	fprintf(fs, "		$('#eth2vid').val(1);\n");
	fprintf(fs, "		$('#eth3vid').val(1);\n");
	fprintf(fs, "		$('#eth4vid').val(1);\n");
	fprintf(fs, "	}else{\n");
	fprintf(fs, "		if( isValidVlanId($('#eth1vid').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidVlanId($('#eth2vid').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidVlanId($('#eth3vid').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidVlanId($('#eth4vid').val()) == false ) return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	loc += '&col_vlanSts=' + $('#vlanSts').val();\n");
	fprintf(fs, "	loc += '&col_eth1vid=' + $('#eth1vid').val();\n");
	fprintf(fs, "	loc += '&col_eth2vid=' + $('#eth2vid').val();\n");
	fprintf(fs, "	loc += '&col_eth3vid=' + $('#eth3vid').val();\n");
	fprintf(fs, "	loc += '&col_eth4vid=' + $('#eth4vid').val();\n");	
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "function setPLinkSts(sts){\n");
	fprintf(fs, "	var loc = 'setPLinkSts.cgi?cnuid=%d';\n", id);	
	fprintf(fs, "	if( 0==sts){\n");	
	fprintf(fs, "		$('#eth1sts').val(1);\n");
	fprintf(fs, "		$('#eth2sts').val(1);\n");
	fprintf(fs, "		$('#eth3sts').val(1);\n");
	fprintf(fs, "		$('#eth4sts').val(1);\n");
	fprintf(fs, "	}\n");	
	fprintf(fs, "	loc += '&col_eth1sts=' + $('#eth1sts').val();\n");
	fprintf(fs, "	loc += '&col_eth2sts=' + $('#eth2sts').val();\n");
	fprintf(fs, "	loc += '&col_eth3sts=' + $('#eth3sts').val();\n");
	fprintf(fs, "	loc += '&col_eth4sts=' + $('#eth4sts').val();\n");	
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "function doRateLimit(sts){\n");
	fprintf(fs, "	var loc = 'doRateLimit.cgi?cnuid=%d';\n", id);	
	fprintf(fs, "	if(0==sts){\n");
	fprintf(fs, "		$('#cpuPortRx').val(0);\n");
	fprintf(fs, "		$('#cpuPortTx').val(0);\n");
	fprintf(fs, "		$('#eth1rx').val(0);\n");
	fprintf(fs, "		$('#eth2rx').val(0);\n");
	fprintf(fs, "		$('#eth3rx').val(0);\n");
	fprintf(fs, "		$('#eth4rx').val(0);\n");
	fprintf(fs, "		$('#eth1tx').val(0);\n");
	fprintf(fs, "		$('#eth2tx').val(0);\n");
	fprintf(fs, "		$('#eth3tx').val(0);\n");
	fprintf(fs, "		$('#eth4tx').val(0);\n");
	fprintf(fs, "		loc += '&col_rxLimitSts=0';\n");
	fprintf(fs, "		loc += '&col_txLimitSts=0';\n");
	fprintf(fs, "	}else{\n");
	fprintf(fs, "		loc += '&col_rxLimitSts=1';\n");
	fprintf(fs, "		loc += '&col_txLimitSts=1';\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	loc += '&col_cpuPortRxRate=' + $('#cpuPortRx').val();\n");
	fprintf(fs, "	loc += '&col_cpuPortTxRate=' + $('#cpuPortTx').val();\n");
	fprintf(fs, "	loc += '&col_eth1rx=' + $('#eth1rx').val();\n");
	fprintf(fs, "	loc += '&col_eth1tx=' + $('#eth1tx').val();\n");
	fprintf(fs, "	loc += '&col_eth2rx=' + $('#eth2rx').val();\n");
	fprintf(fs, "	loc += '&col_eth2tx=' + $('#eth2tx').val();\n");
	fprintf(fs, "	loc += '&col_eth3rx=' + $('#eth3rx').val();\n");
	fprintf(fs, "	loc += '&col_eth3tx=' + $('#eth3tx').val();\n");
	fprintf(fs, "	loc += '&col_eth4rx=' + $('#eth4rx').val();\n");
	fprintf(fs, "	loc += '&col_eth4tx=' + $('#eth4tx').val();\n");	
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "function saveProfile(){\n");
	fprintf(fs, "	var loc = 'saveProfile.cgi?cnuid=%d';\n", id);
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");

	fprintf(fs, "$(function()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	frmLoad();\n");
	fprintf(fs, "	$('#accordion').accordion({\n");
	fprintf(fs, "		collapsible: true,\n");
	fprintf(fs, "		heightStyle: 'content'\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#dialog').dialog({\n");
	fprintf(fs, "		autoOpen: false,\n");
	fprintf(fs, "		width:400,\n");
	fprintf(fs, "		show:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: 'blind',\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "		hide:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: 'explode',\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#opener').button(\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			icons:\n");
	fprintf(fs, "			{\n");
	fprintf(fs, "				primary: 'ui-icon-help'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "			text: false\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	).click(function(){\n");
	fprintf(fs, "		$('#dialog').dialog('open');\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#returnBtn').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-carat-1-w'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#returnBtn').button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnReturn();\n");
	fprintf(fs, "	});\n");
	
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{
		fprintf(fs, "	$('#save1Btn').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "   $('#save2Btn').button().click(function(event){\n");
                fprintf(fs, "                   alert('您没有足够的权限...');\n");
                fprintf(fs, "   });\n");
		fprintf(fs, "   $('#save3Btn').button().click(function(event){\n");
                fprintf(fs, "                   alert('您没有足够的权限...');\n");
                fprintf(fs, "   });\n");
		fprintf(fs, "   $('#save4Btn').button().click(function(event){\n");
                fprintf(fs, "                   alert('您没有足够的权限...');\n");
                fprintf(fs, "   });\n");
		fprintf(fs, "   $('#save5Btn').button().click(function(event){\n");
                fprintf(fs, "                   alert('您没有足够的权限...');\n");
                fprintf(fs, "   });\n");
		fprintf(fs, "   $('#save6Btn').button().click(function(event){\n");
                fprintf(fs, "                   alert('您没有足够的权限...');\n");
                fprintf(fs, "   });\n");
		fprintf(fs, "   $('#clear1Btn').button().click(function(event){\n");
                fprintf(fs, "                   alert('您没有足够的权限...');\n");
                fprintf(fs, "   });\n");
		fprintf(fs, "   $('#clear2Btn').button().click(function(event){\n");
                fprintf(fs, "                   alert('您没有足够的权限...');\n");
                fprintf(fs, "   });\n");
		fprintf(fs, "   $('#clear3Btn').button().click(function(event){\n");
                fprintf(fs, "                   alert('您没有足够的权限...');\n");
                fprintf(fs, "   });\n");
		fprintf(fs, "   $('#clear4Btn').button().click(function(event){\n");
                fprintf(fs, "                   alert('您没有足够的权限...');\n");
                fprintf(fs, "   });\n");
	}
	else
	{
		fprintf(fs, "	$('#save1Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		setMacLimiting();\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "	$('#save2Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		setAgTime();\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "	$('#save3Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		setSFilter(1);\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "	$('#save4Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		setVlan(1);\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "	$('#save5Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		setPLinkSts(1);\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "	$('#save6Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		doRateLimit(1);\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "	$('#clear1Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		setSFilter(0);\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "	$('#clear2Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		setVlan(0);\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "	$('#clear3Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		setPLinkSts(0);\n");
		fprintf(fs, "	});\n");
		fprintf(fs, "	$('#clear4Btn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		doRateLimit(0);\n");
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "	$('#comBtn').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-disk'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{	
		fprintf(fs, "	$('#comBtn').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
	}
	else
	{
		fprintf(fs, "	$('#comBtn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		saveProfile();\n");
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "});\n");
	//fprintf(fs, "\n");

	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "	<tr><td class='maintitle'><b>CNU参数配置</b></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0 width='100%'>\n");
	fprintf(fs, "	<tr><td class='mainline' width='100%'></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<div id='accordion'>\n");
	fprintf(fs, "	<h3>MAC地址限制</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<div class='dwall'>\n");
	fprintf(fs, "			<div class='dwcontent'>\n");
	fprintf(fs, "				<table class='dwcontent'>\n");
	fprintf(fs, "	  				<tr>\n");
	fprintf(fs, "						<td class='diagdata' width=240>启用MAC地址限制</td>\n");
	fprintf(fs, "						<td class='diagdata' width=260>\n");
	fprintf(fs, "							<select id='macLimitSts' size=1>\n");
	fprintf(fs, "								<option value='0'>禁用</option>\n");
	fprintf(fs, "								<option value='1'>启用</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>最大允许桥接的MAC地址数量</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='macLimitNum' size=1>\n");
	fprintf(fs, "								<option value='0'>0</option>\n");
	fprintf(fs, "								<option value='1'>1</option>\n");
	fprintf(fs, "								<option value='2'>2</option>\n");
	fprintf(fs, "								<option value='3'>3</option>\n");
	fprintf(fs, "								<option value='4'>4</option>\n");
	fprintf(fs, "								<option value='5'>5</option>\n");
	fprintf(fs, "								<option value='6'>6</option>\n");
	fprintf(fs, "								<option value='7'>7</option>\n");
	fprintf(fs, "								<option value='8'>8</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "				</table>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "			<div class='dwbtn'><button id='save1Btn'>确 定</button></div>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "	<h3>MAC地址表老化时间</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<div class='dwall'>\n");
	fprintf(fs, "			<div class='dwcontent'>\n");
	fprintf(fs, "				<table class='dwcontent'>\n");
	fprintf(fs, "	  				<tr>\n");
	fprintf(fs, "						<td class='diagdata' width=240>本地桥接MAC地址表老化时间</td>\n");
	fprintf(fs, "						<td class='diagdata' width=260>\n");
	fprintf(fs, "							<input type='text' id='loagTime' size='8'>分钟[范围：1~2000]\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "	  				<tr>\n");
	fprintf(fs, "						<td class='diagdata'>远程桥接MAC地址表老化时间</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<input type='text' id='reagTime' size='8'>分钟[范围：1~2000]\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "				</table>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "			<div class='dwbtn'><button id='save2Btn'>确 定</button></div>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "	<h3>报文抑制</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<div class='dwall'>\n");
	fprintf(fs, "			<div class='dwcontent'>\n");
	fprintf(fs, "				<table class='dwcontent'>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata' width=240>广播风暴抑制</td>\n");
	fprintf(fs, "						<td class='diagdata' width=260>\n");
	fprintf(fs, "							<select id='sfbSts' size=1>\n");
	fprintf(fs, "								<option value='0'>禁用</option>\n");
	fprintf(fs, "								<option value='1'>启用</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>未知单播风暴抑制</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='sfuSts' size=1>\n");
	fprintf(fs, "								<option value='0'>禁用</option>\n");
	fprintf(fs, "								<option value='1'>启用</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>未知组播风暴抑制</td>\n");	
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='sfmSts' size=1>\n");
	fprintf(fs, "								<option value='0'>禁用</option>\n");
	fprintf(fs, "								<option value='1'>启用</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>风暴抑制阀值</td>\n");
	if( 0 == profile.col_sfRate )
	{			
		fprintf(fs, "					<td class='diagdata'>禁用</td>\n");
	}
	else
	{
		fprintf(fs, "					<td class='diagdata'>&nbsp;%d Kpps</td>\n", (1<<(profile.col_sfRate-1)));
	}
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "				</table>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "			<div class='dwbtn'>\n");
	fprintf(fs, "				<button id='clear1Btn'>清 除</button><br>\n");
	fprintf(fs, "				<button id='save3Btn'>确 定</button>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "	<h3>802.1Q VLAN</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<div class='dwall'>\n");
	fprintf(fs, "			<div class='dwcontent'>\n");
	fprintf(fs, "				<table class='dwcontent'>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata' width=240>启用VLAN</td>\n");
	fprintf(fs, "						<td class='diagdata' width=260>\n");
	fprintf(fs, "							<select id='vlanSts' size=1>\n");
	fprintf(fs, "								<option value='0'>禁用</option>\n");
	fprintf(fs, "								<option value='1'>启用</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>ETH1 PVID:</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<input type='text' id='eth1vid' size='8'>[范围：1~4094]\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>ETH2 PVID:</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<input type='text' id='eth2vid' size='8'>[范围：1~4094]\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>ETH3 PVID:</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<input type='text' id='eth3vid' size='8'>[范围：1~4094]\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>ETH4 PVID:</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<input type='text' id='eth4vid' size='8'>[范围：1~4094]\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");	
	fprintf(fs, "				</table>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "			<div class='dwbtn'>\n");
	fprintf(fs, "				<button id='clear2Btn'>清 除</button>\n");
	fprintf(fs, "				<button id='save4Btn'>确 定</button>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "	<h3>端口设置</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<div class='dwall'>\n");
	fprintf(fs, "			<div class='dwcontent'>\n");
	fprintf(fs, "				<table class='dwcontent'>\n");
	fprintf(fs, "	  				<tr>\n");
	fprintf(fs, "						<td class='diagdata' width=240>ETH1端口状态</td>\n");
	fprintf(fs, "						<td class='diagdata' width=260>\n");
	fprintf(fs, "							<select id='eth1sts' size=1>\n");
	fprintf(fs, "								<option value='0'>禁用</option>\n");
	fprintf(fs, "								<option value='1'>启用</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>ETH2端口状态</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='eth2sts' size=1>\n");
	fprintf(fs, "								<option value='0'>禁用</option>\n");
	fprintf(fs, "								<option value='1'>启用</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>ETH3端口状态</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='eth3sts' size=1>\n");
	fprintf(fs, "								<option value='0'>禁用</option>\n");
	fprintf(fs, "								<option value='1'>启用</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>ETH4端口状态</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='eth4sts' size=1>\n");
	fprintf(fs, "								<option value='0'>禁用</option>\n");
	fprintf(fs, "								<option value='1'>启用</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "				</table>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "			<div class='dwbtn'>\n");
	fprintf(fs, "				<button id='clear3Btn'>清 除</button><br>\n");
	fprintf(fs, "				<button id='save5Btn'>确 定</button>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "	<h3>端口限速</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<div class='dwall'>\n");
	fprintf(fs, "			<div class='dwcontent'>\n");
	fprintf(fs, "				<table class='dwcontent'>\n");
	fprintf(fs, "	  				<tr>\n");
	fprintf(fs, "						<td class='diagdata' width=160>端口</td>\n");
	fprintf(fs, "						<td class='diagdata' width=170>&nbsp;上传速率</td>\n");
	fprintf(fs, "						<td class='diagdata' width=170>&nbsp;下载速率</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>CABLE</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='cpuPortTx' size=1>\n");
	fprintf(fs, "								<option value='0'>无限制</option>\n");
	fprintf(fs, "								<option value='1'>128 Kbps</option>\n");
	fprintf(fs, "								<option value='2'>256 Kbps</option>\n");
	fprintf(fs, "								<option value='3'>512 Kbps</option>\n");
	fprintf(fs, "								<option value='4'>1 Mbps</option>\n");
	fprintf(fs, "								<option value='5'>1.5 Mbps</option>\n");
	fprintf(fs, "								<option value='6'>2 Mbps</option>\n");
	fprintf(fs, "								<option value='7'>3 Mbps</option>\n");	
	fprintf(fs, "								<option value='8'>4 Mbps</option>\n");
	fprintf(fs, "								<option value='9'>6 Mbps</option>\n");
	fprintf(fs, "								<option value='10'>8 Mbps</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='cpuPortRx' size=1>\n");
	fprintf(fs, "								<option value='0'>无限制</option>\n");
	fprintf(fs, "								<option value='1'>128 Kbps</option>\n");
	fprintf(fs, "								<option value='2'>256 Kbps</option>\n");
	fprintf(fs, "								<option value='3'>512 Kbps</option>\n");
	fprintf(fs, "								<option value='4'>1 Mbps</option>\n");
	fprintf(fs, "								<option value='5'>1.5 Mbps</option>\n");
	fprintf(fs, "								<option value='6'>2 Mbps</option>\n");
	fprintf(fs, "								<option value='7'>3 Mbps</option>\n");	
	fprintf(fs, "								<option value='8'>4 Mbps</option>\n");
	fprintf(fs, "								<option value='9'>6 Mbps</option>\n");
	fprintf(fs, "								<option value='10'>8 Mbps</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>ETH1</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='eth1rx' size=1>\n");
	fprintf(fs, "								<option value='0'>无限制</option>\n");
	fprintf(fs, "								<option value='1'>128 Kbps</option>\n");
	fprintf(fs, "								<option value='2'>256 Kbps</option>\n");
	fprintf(fs, "								<option value='3'>512 Kbps</option>\n");
	fprintf(fs, "								<option value='4'>1 Mbps</option>\n");
	fprintf(fs, "								<option value='5'>1.5 Mbps</option>\n");
	fprintf(fs, "								<option value='6'>2 Mbps</option>\n");
	fprintf(fs, "								<option value='7'>3 Mbps</option>\n");	
	fprintf(fs, "								<option value='8'>4 Mbps</option>\n");
	fprintf(fs, "								<option value='9'>6 Mbps</option>\n");
	fprintf(fs, "								<option value='10'>8 Mbps</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='eth1tx' size=1>\n");
	fprintf(fs, "								<option value='0'>无限制</option>\n");
	fprintf(fs, "								<option value='1'>128 Kbps</option>\n");
	fprintf(fs, "								<option value='2'>256 Kbps</option>\n");
	fprintf(fs, "								<option value='3'>512 Kbps</option>\n");
	fprintf(fs, "								<option value='4'>1 Mbps</option>\n");
	fprintf(fs, "								<option value='5'>1.5 Mbps</option>\n");
	fprintf(fs, "								<option value='6'>2 Mbps</option>\n");
	fprintf(fs, "								<option value='7'>3 Mbps</option>\n");	
	fprintf(fs, "								<option value='8'>4 Mbps</option>\n");
	fprintf(fs, "								<option value='9'>6 Mbps</option>\n");
	fprintf(fs, "								<option value='10'>8 Mbps</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>ETH2</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='eth2rx' size=1>\n");
	fprintf(fs, "								<option value='0'>无限制</option>\n");
	fprintf(fs, "								<option value='1'>128 Kbps</option>\n");
	fprintf(fs, "								<option value='2'>256 Kbps</option>\n");
	fprintf(fs, "								<option value='3'>512 Kbps</option>\n");
	fprintf(fs, "								<option value='4'>1 Mbps</option>\n");
	fprintf(fs, "								<option value='5'>1.5 Mbps</option>\n");
	fprintf(fs, "								<option value='6'>2 Mbps</option>\n");
	fprintf(fs, "								<option value='7'>3 Mbps</option>\n");	
	fprintf(fs, "								<option value='8'>4 Mbps</option>\n");
	fprintf(fs, "								<option value='9'>6 Mbps</option>\n");
	fprintf(fs, "								<option value='10'>8 Mbps</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='eth2tx' size=1>\n");
	fprintf(fs, "								<option value='0'>无限制</option>\n");
	fprintf(fs, "								<option value='1'>128 Kbps</option>\n");
	fprintf(fs, "								<option value='2'>256 Kbps</option>\n");
	fprintf(fs, "								<option value='3'>512 Kbps</option>\n");
	fprintf(fs, "								<option value='4'>1 Mbps</option>\n");
	fprintf(fs, "								<option value='5'>1.5 Mbps</option>\n");
	fprintf(fs, "								<option value='6'>2 Mbps</option>\n");
	fprintf(fs, "								<option value='7'>3 Mbps</option>\n");	
	fprintf(fs, "								<option value='8'>4 Mbps</option>\n");
	fprintf(fs, "								<option value='9'>6 Mbps</option>\n");
	fprintf(fs, "								<option value='10'>8 Mbps</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>ETH3</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='eth3rx' size=1>\n");
	fprintf(fs, "								<option value='0'>无限制</option>\n");
	fprintf(fs, "								<option value='1'>128 Kbps</option>\n");
	fprintf(fs, "								<option value='2'>256 Kbps</option>\n");
	fprintf(fs, "								<option value='3'>512 Kbps</option>\n");
	fprintf(fs, "								<option value='4'>1 Mbps</option>\n");
	fprintf(fs, "								<option value='5'>1.5 Mbps</option>\n");
	fprintf(fs, "								<option value='6'>2 Mbps</option>\n");
	fprintf(fs, "								<option value='7'>3 Mbps</option>\n");	
	fprintf(fs, "								<option value='8'>4 Mbps</option>\n");
	fprintf(fs, "								<option value='9'>6 Mbps</option>\n");
	fprintf(fs, "								<option value='10'>8 Mbps</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='eth3tx' size=1>\n");
	fprintf(fs, "								<option value='0'>无限制</option>\n");
	fprintf(fs, "								<option value='1'>128 Kbps</option>\n");
	fprintf(fs, "								<option value='2'>256 Kbps</option>\n");
	fprintf(fs, "								<option value='3'>512 Kbps</option>\n");
	fprintf(fs, "								<option value='4'>1 Mbps</option>\n");
	fprintf(fs, "								<option value='5'>1.5 Mbps</option>\n");
	fprintf(fs, "								<option value='6'>2 Mbps</option>\n");
	fprintf(fs, "								<option value='7'>3 Mbps</option>\n");	
	fprintf(fs, "								<option value='8'>4 Mbps</option>\n");
	fprintf(fs, "								<option value='9'>6 Mbps</option>\n");
	fprintf(fs, "								<option value='10'>8 Mbps</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "					<tr>\n");
	fprintf(fs, "						<td class='diagdata'>ETH4</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='eth4rx' size=1>\n");
	fprintf(fs, "								<option value='0'>无限制</option>\n");
	fprintf(fs, "								<option value='1'>128 Kbps</option>\n");
	fprintf(fs, "								<option value='2'>256 Kbps</option>\n");
	fprintf(fs, "								<option value='3'>512 Kbps</option>\n");
	fprintf(fs, "								<option value='4'>1 Mbps</option>\n");
	fprintf(fs, "								<option value='5'>1.5 Mbps</option>\n");
	fprintf(fs, "								<option value='6'>2 Mbps</option>\n");
	fprintf(fs, "								<option value='7'>3 Mbps</option>\n");	
	fprintf(fs, "								<option value='8'>4 Mbps</option>\n");
	fprintf(fs, "								<option value='9'>6 Mbps</option>\n");
	fprintf(fs, "								<option value='10'>8 Mbps</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "						<td class='diagdata'>\n");
	fprintf(fs, "							<select id='eth4tx' size=1>\n");
	fprintf(fs, "								<option value='0'>无限制</option>\n");
	fprintf(fs, "								<option value='1'>128 Kbps</option>\n");
	fprintf(fs, "								<option value='2'>256 Kbps</option>\n");
	fprintf(fs, "								<option value='3'>512 Kbps</option>\n");
	fprintf(fs, "								<option value='4'>1 Mbps</option>\n");
	fprintf(fs, "								<option value='5'>1.5 Mbps</option>\n");
	fprintf(fs, "								<option value='6'>2 Mbps</option>\n");
	fprintf(fs, "								<option value='7'>3 Mbps</option>\n");	
	fprintf(fs, "								<option value='8'>4 Mbps</option>\n");
	fprintf(fs, "								<option value='9'>6 Mbps</option>\n");
	fprintf(fs, "								<option value='10'>8 Mbps</option>\n");
	fprintf(fs, "							</select>\n");
	fprintf(fs, "						</td>\n");
	fprintf(fs, "					</tr>\n");
	fprintf(fs, "				</table>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "			<div class='dwbtn'>\n");
	fprintf(fs, "				<button id='clear4Btn'>清 除</button><br>\n");
	fprintf(fs, "				<button id='save6Btn'>确 定</button>\n");
	fprintf(fs, "			</div>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<button id='returnBtn'>返 回</button>\n");
	fprintf(fs, "	<button id='comBtn'>保 存</button>\n");
	fprintf(fs, "	<button id='opener'>帮助</button>\n");
	fprintf(fs, "</p>\n");
	fprintf(fs, "<div id='dialog' title='帮助信息'>\n");
	fprintf(fs, "	通过本页面，您可以设置CNU/%d/%d的配置参数。</br>\n", cltid, cnuid);
	fprintf(fs, "	<br>-- 点击 '确定' 按钮修改相应的参数；\n");
	fprintf(fs, "	<br>-- 点击 '清除' 按钮将相应的配置项还原为默认值；\n");
	fprintf(fs, "	<br>-- 点击 '保存' 按钮将所有修改的参数存储至数据库。</br>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "</blockquote>\n");
	fprintf(fs, "</body>\n");
	fprintf(fs, "</html>\n");
	
	fflush(fs);
}

void cgiCnuProfileExt(char *query, FILE *fs)
{
	st_dbsCnu cnu;
	uint8_t *p = NULL;
	uint32_t iTemp = 0;
	uint8_t mymac[6] = {0};
	uint8_t mySid[32] = {0};

	dbsGetCnu(dbsdev, glbWebVar.cnuid, &cnu);
	if( cnu.col_auth != glbWebVar.cnuPermition )
	{
		cnu.col_auth = glbWebVar.cnuPermition;
		dbsUpdateCnu(dbsdev, glbWebVar.cnuid, &cnu);
	}
	boardapi_macs2b(cnu.col_mac, mymac);
	p = (uint8_t *)&iTemp;
	p[0] = mymac[5];
	p[1] = mymac[4];
	p[2] = mymac[3];
	p[3] = mymac[2];
	iTemp++;
	mymac[5] = p[0];
	mymac[4] = p[1];
	mymac[3] = p[2];
	mymac[2] = p[3];
	sprintf(mySid, "%02X:%02X:%02X:%02X:%02X:%02X", mymac[0], mymac[1], mymac[2], mymac[3], mymac[4], mymac[5]);
	
	fprintf(fs, "<html>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'/>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'/>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");	
	fprintf(fs, "function frmLoad(){\n");
	fprintf(fs, "	$('#cnuPermition').val(%d);\n", glbWebVar.cnuPermition);
	fprintf(fs, "	hidePortStatusInfo(%s)\n", glbWebVar.cnuPermition?"false":"true");
	fprintf(fs, "	$('#eth1portsts').val(%d);\n",  glbWebVar.col_eth1sts);
	fprintf(fs, "	$('#eth2portsts').val(%d);\n",  glbWebVar.col_eth2sts);
	fprintf(fs, "	$('#eth3portsts').val(%d);\n",  glbWebVar.col_eth3sts);
	fprintf(fs, "	$('#eth4portsts').val(%d);\n",  glbWebVar.col_eth4sts);	
	fprintf(fs, "	$('#vlanStatus').val(%d);\n", glbWebVar.swVlanEnable);
	fprintf(fs, "	$('#eth1vmode').val(%d);\n", glbWebVar.swEth1PortVMode);
	fprintf(fs, "	$('#eth1vid').val(%d);\n", glbWebVar.swEth1PortVid);
	fprintf(fs, "	$('#eth2vmode').val(%d);\n", glbWebVar.swEth2PortVMode);
	fprintf(fs, "	$('#eth2vid').val(%d);\n", glbWebVar.swEth2PortVid);
	fprintf(fs, "	$('#eth3vmode').val(%d);\n", glbWebVar.swEth3PortVMode);
	fprintf(fs, "	$('#eth3vid').val(%d);\n", glbWebVar.swEth3PortVid);
	fprintf(fs, "	$('#eth4vmode').val(%d);\n", glbWebVar.swEth4PortVMode);
	fprintf(fs, "	$('#eth4vid').val(%d);\n", glbWebVar.swEth4PortVid);
	fprintf(fs, "	$('#cpuvmode').val(%d);\n", glbWebVar.swUplinkPortVMode);
	fprintf(fs, "	$('#cpuvid').val(%d);\n", glbWebVar.swUplinkPortVid);
	fprintf(fs, "        $('#eth1vmode').attr('disabled', 'disabled');\n");
	fprintf(fs, "        $('#eth2vmode').attr('disabled', 'disabled');\n");  
	fprintf(fs, "        $('#eth3vmode').attr('disabled', 'disabled');\n");
	fprintf(fs, "        $('#eth4vmode').attr('disabled', 'disabled');\n");
	fprintf(fs, "        $('#cpuvmode').attr('disabled', 'disabled');\n");
	fprintf(fs, "        $('#cpuvid').attr('disabled', 'disabled');\n");
	fprintf(fs, "     if($('#vlanStatus').val() == 0){\n");
	fprintf(fs, "   $('#eth1vmode').val(0);\n");
    fprintf(fs, "        $('#eth1vid').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#eth1vid').val(1);\n");
	fprintf(fs, "   $('#eth2vmode').val(0);\n");
    fprintf(fs, "        $('#eth2vid').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#eth2vid').val(1);\n");	
	fprintf(fs, "   $('#eth3vmode').val(0);\n");
    fprintf(fs, "        $('#eth3vid').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#eth3vid').val(1);\n");
	fprintf(fs, "   $('#eth4vmode').val(0);\n");
    fprintf(fs, "        $('#eth4vid').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#eth4vid').val(1);\n");
	fprintf(fs, "   $('#cpuvmode').val(0);\n");
	fprintf(fs, "   $('#cpuvid').val(1);\n");
    fprintf(fs, "     }\n");
	fprintf(fs, "   $('#vlanStatus').change(function(){\n");  
	fprintf(fs, "     if($('#vlanStatus').val() != 0){\n");  	  
	fprintf(fs, "        $('#eth1vid').removeAttr('disabled');\n"); 
	fprintf(fs, "        $('#eth2vid').removeAttr('disabled');\n");  
    fprintf(fs, "        $('#eth3vid').removeAttr('disabled');\n");  
    fprintf(fs, "        $('#eth4vid').removeAttr('disabled');\n");  
	fprintf(fs, "            $('#eth1vid').blur(function() {\n");
	fprintf(fs, "		 if( $('#eth1vid').val() >1 & $('#eth1vid').val() <4095){\n");
	fprintf(fs, "			$('#eth1vmode').val(1);\n");
	fprintf(fs, "			$('#cpuvmode').val(2);\n");
	fprintf(fs, "               }\n");
	fprintf(fs, "               if( $('#eth1vid').val() == 1)\n"); 
	fprintf(fs, "                      $('#eth1vmode').val(0);\n");
	fprintf(fs, "               if( $('#eth1vmode').val() ==0 & $('#eth2vmode').val() ==0 & $('#eth3vmode').val() ==0 & $('#eth4vmode').val() ==0)\n");
	fprintf(fs, "                      $('#cpuvmode').val(0);\n"); 
	fprintf(fs, "            });\n");
	fprintf(fs, "            $('#eth2vid').blur(function() {\n");
	fprintf(fs, "		 if( $('#eth2vid').val() >1 & $('#eth2vid').val() <4095)\n");
	fprintf(fs, "               {\n");
	fprintf(fs, "			$('#eth2vmode').val(1);\n");
	fprintf(fs, "			$('#cpuvmode').val(2);\n");
	fprintf(fs, "               }\n");
	fprintf(fs, "               if( $('#eth2vid').val() == 1)\n"); 
	fprintf(fs, "                      $('#eth2vmode').val(0);\n");
	fprintf(fs, "               if( $('#eth1vmode').val() ==0 & $('#eth2vmode').val() ==0 & $('#eth3vmode').val() ==0 & $('#eth4vmode').val() ==0)\n");
	fprintf(fs, "                      $('#cpuvmode').val(0);\n"); 
	fprintf(fs, "            });\n");
	fprintf(fs, "            $('#eth3vid').blur(function() {\n");
	fprintf(fs, "		 if( $('#eth3vid').val() >1 & $('#eth3vid').val() <4095)\n");
	fprintf(fs, "               {\n");
	fprintf(fs, "			$('#eth3vmode').val(1);\n");
	fprintf(fs, "			$('#cpuvmode').val(2);\n");
	fprintf(fs, "               }\n");
	fprintf(fs, "               if( $('#eth3vid').val() == 1)\n"); 
	fprintf(fs, "                      $('#eth3vmode').val(0);\n");
	fprintf(fs, "               if( $('#eth1vmode').val() ==0 & $('#eth2vmode').val() ==0 & $('#eth3vmode').val() ==0 & $('#eth4vmode').val() ==0)\n");
	fprintf(fs, "                      $('#cpuvmode').val(0);\n"); 
	fprintf(fs, "            });\n");
	fprintf(fs, "            $('#eth4vid').blur(function() {\n");
	fprintf(fs, "		 if( $('#eth4vid').val() >1 & $('#eth4vid').val() <4095)\n");
	fprintf(fs, "               {\n");
	fprintf(fs, "			$('#eth4vmode').val(1);\n");
	fprintf(fs, "			$('#cpuvmode').val(2);\n");
	fprintf(fs, "               }\n");
	fprintf(fs, "               if( $('#eth4vid').val() == 1)\n"); 
	fprintf(fs, "                      $('#eth4vmode').val(0);\n");
	fprintf(fs, "               if( $('#eth1vmode').val() ==0 & $('#eth2vmode').val() ==0 & $('#eth3vmode').val() ==0 & $('#eth4vmode').val() ==0)\n");
	fprintf(fs, "                      $('#cpuvmode').val(0);\n"); 
	fprintf(fs, "            });\n");
	fprintf(fs, "	  }else{\n");  
    fprintf(fs, "        $('#eth1vid').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#eth1vid').val(1);\n");
    fprintf(fs, "        $('#eth1vmode').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#eth1vmode').val(0);\n");
    fprintf(fs, "        $('#eth2vid').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#eth2vid').val(1);\n");
    fprintf(fs, "        $('#eth2vmode').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#eth2vmode').val(0);\n");
	fprintf(fs, "        $('#eth3vid').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#eth3vid').val(1);\n");
    fprintf(fs, "        $('#eth3vmode').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#eth3vmode').val(0);\n");
	fprintf(fs, "        $('#eth4vid').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#eth4vid').val(1);\n");
    fprintf(fs, "        $('#eth4vmode').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#eth4vmode').val(0);\n");
	fprintf(fs, "        $('#cpuvid').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#couvid').val(1);\n");
    fprintf(fs, "        $('#cpuvmode').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#cpuvmode').val(0);\n");
    fprintf(fs, "     }\n");  
    fprintf(fs, "   });\n");
	fprintf(fs, "            $('#eth1vid').blur(function() {\n");
	fprintf(fs, "		 if( $('#eth1vid').val() >1 & $('#eth1vid').val() <4095){\n");
	fprintf(fs, "			$('#eth1vmode').val(1);\n");
	fprintf(fs, "			$('#cpuvmode').val(2);\n");
	fprintf(fs, "               }\n");
	fprintf(fs, "               if( $('#eth1vid').val() == 1)\n"); 
	fprintf(fs, "                      $('#eth1vmode').val(0);\n");
	fprintf(fs, "               if( $('#eth1vmode').val() ==0 & $('#eth2vmode').val() ==0 & $('#eth3vmode').val() ==0 & $('#eth4vmode').val() ==0)\n");
	fprintf(fs, "                      $('#cpuvmode').val(0);\n"); 
	fprintf(fs, "            });\n");
	fprintf(fs, "            $('#eth2vid').blur(function() {\n");
	fprintf(fs, "		 if( $('#eth2vid').val() >1 & $('#eth2vid').val() <4095){\n");
	fprintf(fs, "			$('#eth2vmode').val(1);\n");
	fprintf(fs, "			$('#cpuvmode').val(2);\n");
	fprintf(fs, "               }\n");
	fprintf(fs, "               if( $('#eth2vid').val() == 1)\n"); 
	fprintf(fs, "                      $('#eth2vmode').val(0);\n");
	fprintf(fs, "               if( $('#eth1vmode').val() ==0 & $('#eth2vmode').val() ==0 & $('#eth3vmode').val() ==0 & $('#eth4vmode').val() ==0)\n");
	fprintf(fs, "                      $('#cpuvmode').val(0);\n"); 
	fprintf(fs, "            });\n");
	fprintf(fs, "            $('#eth3vid').blur(function() {\n");
	fprintf(fs, "		 if( $('#eth3vid').val() >1 & $('#eth3vid').val() <4095){\n");
	fprintf(fs, "			$('#eth3vmode').val(1);\n");
	fprintf(fs, "			$('#cpuvmode').val(2);\n");
	fprintf(fs, "               }\n");
	fprintf(fs, "               if( $('#eth3vid').val() == 1)\n"); 
	fprintf(fs, "                      $('#eth3vmode').val(0);\n");
	fprintf(fs, "               if( $('#eth1vmode').val() ==0 & $('#eth2vmode').val() ==0 & $('#eth3vmode').val() ==0 & $('#eth4vmode').val() ==0)\n");
	fprintf(fs, "                      $('#cpuvmode').val(0);\n"); 
	fprintf(fs, "            });\n");
	fprintf(fs, "            $('#eth4vid').blur(function() {\n");
	fprintf(fs, "		 if( $('#eth4vid').val() >1 & $('#eth4vid').val() <4095){\n");
	fprintf(fs, "			$('#eth4vmode').val(1);\n");
	fprintf(fs, "			$('#cpuvmode').val(2);\n");
	fprintf(fs, "               }\n");
	fprintf(fs, "               if( $('#eth4vid').val() == 1)\n"); 
	fprintf(fs, "                      $('#eth4vmode').val(0);\n");
	fprintf(fs, "               if( $('#eth1vmode').val() ==0 & $('#eth2vmode').val() ==0 & $('#eth3vmode').val() ==0 & $('#eth4vmode').val() ==0)\n");
	fprintf(fs, "                      $('#cpuvmode').val(0);\n"); 
	fprintf(fs, "            });\n");
	fprintf(fs, "	$('#rxBctrlStatus').val(%d);\n", glbWebVar.swRxRateLimitEnable);
	fprintf(fs, "	$('#txBctrlStatus').val(%d);\n", glbWebVar.swTxRateLimitEnable);
	fprintf(fs, "	$('#eth1rxvalue').val(%d);\n", glbWebVar.swEth1RxRate);
	fprintf(fs, "	$('#eth2rxvalue').val(%d);\n", glbWebVar.swEth2RxRate);
	fprintf(fs, "	$('#eth3rxvalue').val(%d);\n", glbWebVar.swEth3RxRate);
	fprintf(fs, "	$('#eth4rxvalue').val(%d);\n", glbWebVar.swEth4RxRate);
	fprintf(fs, "	$('#cablerxvalue').val(%d);\n", glbWebVar.swUplinkRxRate);
	fprintf(fs, "	$('#eth1txvalue').val(%d);\n", glbWebVar.swEth1TxRate);
	fprintf(fs, "	$('#eth2txvalue').val(%d);\n", glbWebVar.swEth2TxRate);
	fprintf(fs, "	$('#eth3txvalue').val(%d);\n", glbWebVar.swEth3TxRate);
	fprintf(fs, "	$('#eth4txvalue').val(%d);\n", glbWebVar.swEth4TxRate);
	fprintf(fs, "	$('#cabletxvalue').val(%d);\n", glbWebVar.swUplinkTxRate);
	fprintf(fs, "     if($('#rxBctrlStatus').val() == 0){\n");
	fprintf(fs, "        $('#eth1rxvalue').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#eth1rxvalue').val(2047);\n");
    fprintf(fs, "        $('#eth2rxvalue').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#eth2rxvalue').val(2047);\n");
    fprintf(fs, "        $('#eth3rxvalue').attr('disabled', 'disabled');\n");  
	fprintf(fs, "   $('#eth3rxvalue').val(2047);\n");
    fprintf(fs, "        $('#eth4rxvalue').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#eth4rxvalue').val(2047);\n");	
    fprintf(fs, "        $('#cablerxvalue').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#cablerxvalue').val(2047);\n");
    fprintf(fs, "     }\n");
	fprintf(fs, "   $('#rxBctrlStatus').change(function(){\n");  
	fprintf(fs, "     if($('#rxBctrlStatus').val() != 0){\n");  	  
	fprintf(fs, "        $('#eth1rxvalue').removeAttr('disabled');\n"); 
	fprintf(fs, "        $('#eth2rxvalue').removeAttr('disabled');\n");  
    fprintf(fs, "        $('#eth3rxvalue').removeAttr('disabled');\n");  
    fprintf(fs, "        $('#eth4rxvalue').removeAttr('disabled');\n");  
    fprintf(fs, "        $('#cablerxvalue').removeAttr('disabled');\n");   
    fprintf(fs, "	  }else{\n");  
    fprintf(fs, "        $('#eth1rxvalue').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#eth1rxvalue').val(2047);\n");
    fprintf(fs, "        $('#eth2rxvalue').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#eth2rxvalue').val(2047);\n");
    fprintf(fs, "        $('#eth3rxvalue').attr('disabled', 'disabled');\n");  
	fprintf(fs, "   $('#eth3rxvalue').val(2047);\n");
    fprintf(fs, "        $('#eth4rxvalue').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#eth4rxvalue').val(2047);\n");	
    fprintf(fs, "        $('#cablerxvalue').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#cablerxvalue').val(2047);\n");
    fprintf(fs, "     }\n");  
    fprintf(fs, "   });\n");
	fprintf(fs, "     if($('#txBctrlStatus').val() == 0){\n");
	fprintf(fs, "        $('#eth1txvalue').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#eth1txvalue').val(2047);\n");
    fprintf(fs, "        $('#eth2txvalue').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#eth2txvalue').val(2047);\n");
    fprintf(fs, "        $('#eth3txvalue').attr('disabled', 'disabled');\n");  
	fprintf(fs, "   $('#eth3txvalue').val(2047);\n");
    fprintf(fs, "        $('#eth4txvalue').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#eth4txvalue').val(2047);\n");	
    fprintf(fs, "        $('#cabletxvalue').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#cabletxvalue').val(2047);\n");
    fprintf(fs, "     }\n");
	fprintf(fs, "   $('#txBctrlStatus').change(function(){\n");  
	fprintf(fs, "     if($('#txBctrlStatus').val() != 0){\n");  	  
	fprintf(fs, "        $('#eth1txvalue').removeAttr('disabled');\n"); 
	fprintf(fs, "        $('#eth2txvalue').removeAttr('disabled');\n");  
    fprintf(fs, "        $('#eth3txvalue').removeAttr('disabled');\n");  
    fprintf(fs, "        $('#eth4txvalue').removeAttr('disabled');\n");  
    fprintf(fs, "        $('#cabletxvalue').removeAttr('disabled');\n");   
    fprintf(fs, "	  }else{\n");  
    fprintf(fs, "        $('#eth1txvalue').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#eth1txvalue').val(2047);\n");
    fprintf(fs, "        $('#eth2txvalue').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#eth2txvalue').val(2047);\n");
    fprintf(fs, "        $('#eth3txvalue').attr('disabled', 'disabled');\n");  
	fprintf(fs, "   $('#eth3txvalue').val(2047);\n");
    fprintf(fs, "        $('#eth4txvalue').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#eth4txvalue').val(2047);\n");	
    fprintf(fs, "        $('#cabletxvalue').attr('disabled', 'disabled');\n"); 
	fprintf(fs, "   $('#cabletxvalue').val(2047);\n");
    fprintf(fs, "     }\n");  
    fprintf(fs, "   });\n");
	fprintf(fs, "	$('#loopDetectStatus').val(%d);\n", glbWebVar.swLoopDetect);
	fprintf(fs, "	$('#swsid').val('%s');\n", glbWebVar.swSwitchSid);
	fprintf(fs, "	$('#swsid').attr('disabled', 'disabled');\n");
	fprintf(fs, "	$('#sidQuickSetup').val(0);\n");
	fprintf(fs, "	$('#eth1LoopStatus').val(%d);\n", glbWebVar.swEth1LoopStatus);
	fprintf(fs, "	$('#eth2LoopStatus').val(%d);\n", glbWebVar.swEth2LoopStatus);
	fprintf(fs, "	$('#eth3LoopStatus').val(%d);\n", glbWebVar.swEth3LoopStatus);
	fprintf(fs, "	$('#eth4LoopStatus').val(%d);\n", glbWebVar.swEth4LoopStatus);
	fprintf(fs, "	$('#sfrules').val(%d);\n", glbWebVar.swSfRule);
	fprintf(fs, "	$('#sfbroadcast').val(%d);\n", glbWebVar.swSfDisBroadcast);
	fprintf(fs, "	$('#sfmulticast').val(%d);\n", glbWebVar.swSfDisMulticast);
	fprintf(fs, "	$('#sfunknown').val(%d);\n", glbWebVar.swSfDisUnknown);
	fprintf(fs, "	$('#sfiteration').val(%d);\n", glbWebVar.swSfIteration);
	fprintf(fs, "	$('#sfcounter').val(%d);\n", glbWebVar.swSfThresholt);
	fprintf(fs, "	$('#sfreset').val(%d);\n", glbWebVar.swSfResetSrc);
	fprintf(fs, "	$('#mlsysen').val(%d);\n", glbWebVar.swMlSysEnable);
	fprintf(fs, "	$('#mlsysmax').val(%d);\n", glbWebVar.swMlSysThresholt);
	fprintf(fs, "	$('#mleth1en').val(%d);\n", glbWebVar.swMlEth1Enable);
	fprintf(fs, "	$('#mleth1max').val(%d);\n", glbWebVar.swMlEth1Thresholt);
	fprintf(fs, "	$('#mleth2en').val(%d);\n", glbWebVar.swMlEth2Enable);
	fprintf(fs, "	$('#mleth2max').val(%d);\n", glbWebVar.swMlEth2Thresholt);
	fprintf(fs, "	$('#mleth3en').val(%d);\n", glbWebVar.swMlEth3Enable);
	fprintf(fs, "	$('#mleth3max').val(%d);\n", glbWebVar.swMlEth3Thresholt);
	fprintf(fs, "	$('#mleth4en').val(%d);\n", glbWebVar.swMlEth4Enable);
	fprintf(fs, "	$('#mleth4max').val(%d);\n", glbWebVar.swMlEth4Thresholt);
	fprintf(fs, "     if($('#mlsysen').val() == 0){\n");
	fprintf(fs, "        $('#mlsysmax').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#mlsysmax').val(0);\n");
	fprintf(fs, "     }\n");
	fprintf(fs, "     if($('#mleth1en').val() == 0){\n");
	fprintf(fs, "        $('#mleth1max').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#mleth1max').val(0);\n");
	fprintf(fs, "     }\n");
	fprintf(fs, "     if($('#mleth2en').val() == 0){\n");
	fprintf(fs, "        $('#mleth2max').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#mleth2max').val(0);\n");
	fprintf(fs, "     }\n");
	fprintf(fs, "     if($('#mleth3en').val() == 0){\n");
	fprintf(fs, "        $('#mleth3max').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#mleth3max').val(0);\n");
	fprintf(fs, "     }\n");
	fprintf(fs, "     if($('#mleth4en').val() == 0){\n");
	fprintf(fs, "        $('#mleth4max').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#mleth4max').val(0);\n");
	fprintf(fs, "     }\n");
	fprintf(fs, "   $('#mlsysen').change(function(){\n");  
	fprintf(fs, "     if($('#mlsysen').val() != 0){\n");  	  
	fprintf(fs, "        $('#mlsysmax').removeAttr('disabled');\n"); 
	fprintf(fs, "	  }else{\n");  
	fprintf(fs, "        $('#mlsysmax').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#mlsysmax').val(0);\n");
	fprintf(fs, "     }\n");  
    fprintf(fs, "   });\n");
	fprintf(fs, "   $('#mleth1en').change(function(){\n");  
	fprintf(fs, "     if($('#mleth1en').val() != 0){\n");  	  
	fprintf(fs, "        $('#mleth1max').removeAttr('disabled');\n"); 
	fprintf(fs, "	  }else{\n");  
	fprintf(fs, "        $('#mleth1max').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#mleth1max').val(0);\n");
	fprintf(fs, "     }\n");  
    fprintf(fs, "   });\n");
	fprintf(fs, "   $('#mleth2en').change(function(){\n");  
	fprintf(fs, "     if($('#mleth2en').val() != 0){\n");  	  
	fprintf(fs, "        $('#mleth2max').removeAttr('disabled');\n"); 
	fprintf(fs, "	  }else{\n");  
	fprintf(fs, "        $('#mleth2max').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#mleth2max').val(0);\n");
	fprintf(fs, "     }\n");  
    fprintf(fs, "   });\n");
	fprintf(fs, "   $('#mleth3en').change(function(){\n");  
	fprintf(fs, "     if($('#mleth3en').val() != 0){\n");  	  
	fprintf(fs, "        $('#mleth3max').removeAttr('disabled');\n"); 
	fprintf(fs, "	  }else{\n");  
	fprintf(fs, "        $('#mleth3max').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#mleth3max').val(0);\n");
	fprintf(fs, "     }\n");  
    fprintf(fs, "   });\n");
	fprintf(fs, "   $('#mleth4en').change(function(){\n");  
	fprintf(fs, "     if($('#mleth4en').val() != 0){\n");  	  
	fprintf(fs, "        $('#mleth4max').removeAttr('disabled');\n"); 
	fprintf(fs, "	  }else{\n");  
	fprintf(fs, "        $('#mleth4max').attr('disabled', 'disabled');\n");
	fprintf(fs, "   $('#mleth4max').val(0);\n");
	fprintf(fs, "     }\n");  
    fprintf(fs, "   });\n");
	fprintf(fs, "}\n");

	fprintf(fs, "function hidePortStatusInfo(hide){\n");
	fprintf(fs, "	if( hide ) $('#PortStatusInfo').hide();\n");
	fprintf(fs, "	else $('#PortStatusInfo').show();\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function cnuPermitClick(){\n");
	fprintf(fs, "	if ( $('#cnuPermition').val() == 1 )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		$('#eth1portsts').val(1);\n");
	fprintf(fs, "		$('#eth2portsts').val(1);\n");
	fprintf(fs, "		$('#eth3portsts').val(1);\n");
	fprintf(fs, "		$('#eth4portsts').val(1);\n");
	fprintf(fs, "		hidePortStatusInfo(false);\n");
	fprintf(fs, "	}else{\n");	
	fprintf(fs, "		hidePortStatusInfo(true);\n");
	fprintf(fs, "		$('#eth1portsts').val(0);\n");
	fprintf(fs, "		$('#eth2portsts').val(0);\n");
	fprintf(fs, "		$('#eth3portsts').val(0);\n");
	fprintf(fs, "		$('#eth4portsts').val(0);\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "}\n");

	fprintf(fs, "function sidSetup(){\n");
	fprintf(fs, "	var mysid = $('#sidQuickSetup').val();\n");
	fprintf(fs, "	var myselectid = $('#sidQuickSetup')[0].selectedIndex;\n");
	fprintf(fs, "	if(myselectid==1)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		$('#swsid').val(mysid);\n");
	fprintf(fs, "		$('#swsid').attr('disabled', 'disabled');\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	else if(myselectid==2)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		$('#swsid').val(mysid);\n");
	fprintf(fs, "		$('#swsid').attr('disabled', 'disabled');\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	else if(myselectid==3)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		$('#swsid').val(mysid);\n");
	fprintf(fs, "		$('#swsid').removeAttr('disabled');\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "function Return(){\n");
	fprintf(fs, "	var loc = 'cnuManagement.cmd?cnuid=';\n");
	fprintf(fs, "	loc += %d;\n", glbWebVar.cnuid);
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function Read(){\n");
	fprintf(fs, "	$('#btn_read').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_write').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_return').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_erase').attr('disabled', true);\n");
	fprintf(fs, "	var loc = 'rtl8306eConfigRead.cgi?';\n");
	fprintf(fs, "	loc += 'cnuid=' + %d;\n", glbWebVar.cnuid);
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function Write(){\n");
	fprintf(fs, "	var msg;\n");
	fprintf(fs, "	var loc = 'rtl8306eConfigWrite.cgi?';\n");
	fprintf(fs, "	if($('#vlanStatus').val() != 0){\n");
	fprintf(fs, "		if( isValidVlanId($('#cpuvid').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidVlanId($('#eth1vid').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidVlanId($('#eth2vid').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidVlanId($('#eth3vid').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidVlanId($('#eth4vid').val()) == false ) return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	if($('#rxBctrlStatus').val() != 0){\n");
	fprintf(fs, "		if( isValidBctrlValue($('#eth1rxvalue').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidBctrlValue($('#eth2rxvalue').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidBctrlValue($('#eth3rxvalue').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidBctrlValue($('#eth4rxvalue').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidBctrlValue($('#cablerxvalue').val()) == false ) return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	if($('#txBctrlStatus').val() != 0)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		if( isValidBctrlValue($('#eth1txvalue').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidBctrlValue($('#eth2txvalue').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidBctrlValue($('#eth3txvalue').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidBctrlValue($('#eth4txvalue').val()) == false ) return;\n");
	fprintf(fs, "		if( isValidBctrlValue($('#cabletxvalue').val()) == false ) return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	if($('#mlsysen').val() != 0)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		if( isValidMlsys($('#mlsysmax').val()) == false ) return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	if($('#mleth1en').val() != 0)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		if( isValidMlport($('#mleth1max').val()) == false ) return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	if($('#mleth2en').val() != 0)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		if( isValidMlport($('#mleth2max').val()) == false ) return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	if($('#mleth3en').val() != 0)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		if( isValidMlport($('#mleth3max').val()) == false ) return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	if($('#mleth4en').val() != 0)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		if( isValidMlport($('#mleth4max').val()) == false ) return;\n");
	fprintf(fs, "	}\n");	
	fprintf(fs, "	loc += 'cnuid=' + %d;\n", glbWebVar.cnuid);

	fprintf(fs, "	if ( $('#cnuPermition').val() == 1 )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		loc += '&cnuPermition=1';\n");
	fprintf(fs, "		loc += '&col_eth1sts=' + $('#eth1portsts').val();\n");
	fprintf(fs, "		loc += '&col_eth2sts=' + $('#eth2portsts').val();\n");
	fprintf(fs, "		loc += '&col_eth3sts=' + $('#eth3portsts').val();\n");
	fprintf(fs, "		loc += '&col_eth4sts=' + $('#eth4portsts').val();\n");
	fprintf(fs, "	}else{\n");	
	fprintf(fs, "		loc += '&cnuPermition=0';\n");
	fprintf(fs, "		loc += '&col_eth1sts=0';\n");
	fprintf(fs, "		loc += '&col_eth2sts=0';\n");
	fprintf(fs, "		loc += '&col_eth3sts=0';\n");
	fprintf(fs, "		loc += '&col_eth4sts=0';\n");
	fprintf(fs, "	}\n");
	
	fprintf(fs, "	loc += '&swVlanEnable=' + $('#vlanStatus').val();\n");
	fprintf(fs, "		loc += '&swUplinkPortVMode=' + $('#cpuvmode').val();\n");
	fprintf(fs, "		loc += '&swEth1PortVMode=' + $('#eth1vmode').val();\n");
	fprintf(fs, "		loc += '&swEth2PortVMode=' + $('#eth2vmode').val();\n");
	fprintf(fs, "		loc += '&swEth3PortVMode=' + $('#eth3vmode').val();\n");
	fprintf(fs, "		loc += '&swEth4PortVMode=' + $('#eth4vmode').val();\n");
	fprintf(fs, "		loc += '&swUplinkPortVid=' + $('#cpuvid').val();\n");
	fprintf(fs, "		loc += '&swEth1PortVid=' + $('#eth1vid').val();\n");
	fprintf(fs, "		loc += '&swEth2PortVid=' + $('#eth2vid').val();\n");
	fprintf(fs, "		loc += '&swEth3PortVid=' + $('#eth3vid').val();\n");
	fprintf(fs, "		loc += '&swEth4PortVid=' + $('#eth4vid').val();\n");
	fprintf(fs, "	loc += '&swRxRateLimitEnable=' + $('#rxBctrlStatus').val();\n");
	fprintf(fs, "	loc += '&swTxRateLimitEnable=' + $('#txBctrlStatus').val();\n");
	fprintf(fs, "		loc += '&swUplinkRxRate=' + $('#cablerxvalue').val();\n");
	fprintf(fs, "		loc += '&swEth1RxRate=' + $('#eth1rxvalue').val();\n");
	fprintf(fs, "		loc += '&swEth2RxRate=' + $('#eth2rxvalue').val();\n");
	fprintf(fs, "		loc += '&swEth3RxRate=' + $('#eth3rxvalue').val();\n");
	fprintf(fs, "		loc += '&swEth4RxRate=' + $('#eth4rxvalue').val();\n");
	fprintf(fs, "		loc += '&swUplinkTxRate=' + $('#cabletxvalue').val();\n");
	fprintf(fs, "		loc += '&swEth1TxRate=' + $('#eth1txvalue').val();\n");
	fprintf(fs, "		loc += '&swEth2TxRate=' + $('#eth2txvalue').val();\n");
	fprintf(fs, "		loc += '&swEth3TxRate=' + $('#eth3txvalue').val();\n");
	fprintf(fs, "		loc += '&swEth4TxRate=' + $('#eth4txvalue').val();\n");
	fprintf(fs, "	loc += '&swLoopDetect=' + $('#loopDetectStatus').val();\n");
	fprintf(fs, "	if( isValidMacAddress($('#swsid').val()) == false ){\n");
	fprintf(fs, "		msg = 'SID \"' + $('#swsid').val() + '\" 输入不正确！.';\n");
	fprintf(fs, "		alert(msg);\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	loc += '&swSwitchSid=' + $('#swsid').val();\n");	
	fprintf(fs, "	loc += '&swSfDisBroadcast=' + $('#sfbroadcast').val();\n");
	fprintf(fs, "	loc += '&swSfDisMulticast=' + $('#sfmulticast').val();\n");
	fprintf(fs, "	loc += '&swSfDisUnknown=' + $('#sfunknown').val();\n");
	fprintf(fs, "	loc += '&swSfRule=' + $('#sfrules').val();\n");
	fprintf(fs, "	loc += '&swSfResetSrc=' + $('#sfreset').val();\n");
	fprintf(fs, "	loc += '&swSfIteration=' + $('#sfiteration').val();\n");
	fprintf(fs, "	loc += '&swSfThresholt=' + $('#sfcounter').val();\n");
	fprintf(fs, "	loc += '&swMlSysEnable=' + $('#mlsysen').val();\n");	
	fprintf(fs, "	loc += '&swMlSysThresholt=' + $('#mlsysmax').val();\n");
	fprintf(fs, "	loc += '&swMlEth1Enable=' + $('#mleth1en').val();\n");	
	fprintf(fs, "	loc += '&swMlEth1Thresholt=' + $('#mleth1max').val();\n");
	fprintf(fs, "	loc += '&swMlEth2Enable=' + $('#mleth2en').val();\n");	
	fprintf(fs, "	loc += '&swMlEth2Thresholt=' + $('#mleth2max').val();\n");
	fprintf(fs, "	loc += '&swMlEth3Enable=' + $('#mleth3en').val();\n");	
	fprintf(fs, "	loc += '&swMlEth3Thresholt=' + $('#mleth3max').val();\n");
	fprintf(fs, "	loc += '&swMlEth4Enable=' + $('#mleth4en').val();\n");	
	fprintf(fs, "	loc += '&swMlEth4Thresholt=' + $('#mleth4max').val();\n");
	fprintf(fs, "	$('#btn_read').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_write').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_return').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_erase').attr('disabled', true);\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");	
	fprintf(fs, "function restoreDefault(){\n");
	fprintf(fs, "	$('#btn_read').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_write').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_return').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_erase').attr('disabled', true);\n");
	fprintf(fs, "	var loc = 'rtl8306eConfigErase.cgi?';\n");
	fprintf(fs, "	loc += 'cnuid=' + %d;\n", glbWebVar.cnuid);
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");	
	fprintf(fs, "$(function(){\n");
	fprintf(fs, "	frmLoad();\n");
	fprintf(fs, "	$('#accordion').accordion({\n");
	fprintf(fs, "		collapsible: true,\n");
	fprintf(fs, "		heightStyle: 'content'\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#dialog').dialog({\n");
	fprintf(fs, "		autoOpen: false,\n");
	fprintf(fs, "		width:370,\n");
	fprintf(fs, "		show:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: 'blind',\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "		hide:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: 'explode',\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#opener').button(\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			icons:\n");
	fprintf(fs, "			{\n");
	fprintf(fs, "				primary: 'ui-icon-help'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "			text: false\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	).click(function(){\n");
	fprintf(fs, "		$('#dialog').dialog('open');\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#btn_return').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-carat-1-w'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#btn_return').button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		Return();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#btn_read').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-arrow-4-diag'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#btn_read').button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		Read();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#btn_write').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-tag'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{
		fprintf(fs, "	$('#btn_write').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
	}
	else
	{
		fprintf(fs, "	$('#btn_write').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		Write();\n");
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "	$('#btn_erase').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-refresh'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{	
		fprintf(fs, "	$('#btn_erase').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
	}
	else
	{
		fprintf(fs, "	$('#btn_erase').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		restoreDefault();\n");
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "});\n");
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "	<tr><td class='maintitle'><b>CNU参数配置</b></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0 width='100%'>\n");
	fprintf(fs, "	<tr><td class='mainline' width='100%'></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<br><br>\n");
	fprintf(fs, "<div id='accordion'>  \n");
	fprintf(fs, "<h3>CNU及端口授权</h3>\n");
	fprintf(fs, "<div>\n");
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata' width=300>CNU授权状态</td>\n");
	fprintf(fs, "			<td class='diagdata' width=150>\n");
	fprintf(fs, "				<select id='cnuPermition' onClick='cnuPermitClick()' size=1 style='width:80px'>\n");
	fprintf(fs, "					<option value=0>禁止</option>\n");
	fprintf(fs, "					<option value=1>允许</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr><td class='diagdata' colspan=2>&nbsp;</td></tr>\n");	
	fprintf(fs, "	</table>\n");	
	fprintf(fs, "	<div id='PortStatusInfo'>\n");
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata' width=300>ETH1端口状态</td>\n");
	fprintf(fs, "			<td class='diagdata' width=150>\n");
	fprintf(fs, "				<select id='eth1portsts' size=1 style='width:80px'>\n");
	fprintf(fs, "					<option value=0>禁用</option>\n");
	fprintf(fs, "					<option value=1>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH2端口状态</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='eth2portsts' size=1 style='width:80px'>\n");
	fprintf(fs, "					<option value=0>禁用</option>\n");
	fprintf(fs, "					<option value=1>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH3端口状态</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='eth3portsts' size=1 style='width:80px'>\n");
	fprintf(fs, "					<option value=0>禁用</option>\n");
	fprintf(fs, "					<option value=1>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH4端口状态</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='eth4portsts' size=1 style='width:80px'>\n");
	fprintf(fs, "					<option value=0>禁用</option>\n");
	fprintf(fs, "					<option value=1>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "	</table>\n");
	fprintf(fs, "	</div>\n");	
	fprintf(fs, "</div>\n");
	fprintf(fs, "<h3>802.1Q VLAN</h3>\n");
	fprintf(fs, "<div>\n");
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>启用 802.1q Vlan</td>\n");
	fprintf(fs, "			<td class='diagdata' colspan=2>\n");
	fprintf(fs, "				<select id='vlanStatus' size=1>\n");
	fprintf(fs, "					<option value='0'>禁用</option>\n");
	fprintf(fs, "					<option value='1'>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr><td class='diagdata' colspan=3>&nbsp;</td></tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata' width=150>端口</td>\n");
	fprintf(fs, "			<td class='diagdata' width=300>&nbsp;VLAN ID</td>\n");
//	fprintf(fs, "			<td class='diagdata' width=150>&nbsp;VLAN模式</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH1</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='eth1vid' size='8'>[范围：1~4094]</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='eth1vmode' size=1 style='visibility:hidden'>\n");
	fprintf(fs, "					<option value='0'>透传</option>\n");
	fprintf(fs, "					<option value='1'>Untag</option>\n");
	fprintf(fs, "					<option value='2'>Tag</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH2</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='eth2vid' size='8'>[范围：1~4094]</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='eth2vmode' size=1 style='visibility:hidden'>\n");
	fprintf(fs, "					<option value='0'>透传</option>\n");
	fprintf(fs, "					<option value='1'>Untag</option>\n");
	fprintf(fs, "					<option value='2'>Tag</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH3</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='eth3vid' size='8'>[范围：1~4094]</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='eth3vmode' size=1 style='visibility:hidden'>\n");
	fprintf(fs, "					<option value='0'>透传</option>\n");
	fprintf(fs, "					<option value='1'>Untag</option>\n");
	fprintf(fs, "					<option value='2'>Tag</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH4</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='eth4vid' size='8'>[范围：1~4094]</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='eth4vmode' size=1 style='visibility:hidden'>\n");
	fprintf(fs, "					<option value='0'>透传</option>\n");
	fprintf(fs, "					<option value='1'>Untag</option>\n");
	fprintf(fs, "					<option value='2'>Tag</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr> \n");
	fprintf(fs, "		<tr>\n");
//	fprintf(fs, "			<td class='diagdata'>CABLE</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='cpuvid' size='8' style='visibility:hidden'></td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='cpuvmode' size=1 style='visibility:hidden'>\n");
	fprintf(fs, "					<option value='0'>透传</option>\n");
	fprintf(fs, "					<option value='1'>Untag</option>\n");
	fprintf(fs, "					<option value='2'>Tag</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "	</table>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "<h3>端口限速</h3>\n");
	fprintf(fs, "<div>\n");
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=0>  \n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata' width=300>端口接收限速</td>\n");
	fprintf(fs, "			<td class='diagdata' width=300>\n");
	fprintf(fs, "				<select id='rxBctrlStatus' size=1>\n");
	fprintf(fs, "					<option value='0'>禁用</option>\n");
	fprintf(fs, "					<option value='1'>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH1端口接收限速</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='eth1rxvalue' size='10'>*64Kbits/s [范围：0~2047]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH2端口接收限速</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='eth2rxvalue' size='10'>*64Kbits/s [范围：0~2047]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH3端口接收限速</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='eth3rxvalue' size='10'>*64Kbits/s [范围：0~2047]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH4端口接收限速</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='eth4rxvalue' size='10'>*64Kbits/s [范围：0~2047]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>CABLE端口接收限速</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='cablerxvalue' size='10'>*64Kbits/s [范围：0~2047]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>端口发送限速</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='txBctrlStatus' size=1>\n");
	fprintf(fs, "					<option value='0'>禁用</option>\n");
	fprintf(fs, "					<option value='1'>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH1端口发送限速</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='eth1txvalue' size='10'>*64Kbits/s [范围：0~2047]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH2端口发送限速</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='eth2txvalue' size='10'>*64Kbits/s [范围：0~2047]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH3端口发送限速</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='eth3txvalue' size='10'>*64Kbits/s [范围：0~2047]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH4端口发送限速</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='eth4txvalue' size='10'>*64Kbits/s [范围：0~2047]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>CABLE端口发送限速</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='cabletxvalue' size='10'>*64Kbits/s [范围：0~2047]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "	</table>\n");
	fprintf(fs, "</div>\n");	
	fprintf(fs, "<h3>报文抑制</h3>\n");
	fprintf(fs, "<div>\n");
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata' width=300>过滤器规则</td>\n");
	fprintf(fs, "			<td class='diagdata' width=150>\n");
	fprintf(fs, "				<select id='sfrules' size=1 style='width:80px'>\n");
	fprintf(fs, "					<option value=0>类型1</option>\n");
	fprintf(fs, "					<option value=1>类型2</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>广播风暴抑制</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='sfbroadcast' size=1 style='width:80px'>\n");
	fprintf(fs, "					<option value=0>启用</option>\n");
	fprintf(fs, "					<option value=1>禁用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>组播风暴抑制</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='sfmulticast' size=1 style='width:80px'>\n");
	fprintf(fs, "					<option value=0>启用</option>\n");
	fprintf(fs, "					<option value=1>禁用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>未知单播风暴抑制</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='sfunknown' size=1 style='width:80px'>\n");
	fprintf(fs, "					<option value=0>启用</option>\n");
	fprintf(fs, "					<option value=1>禁用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>过滤器迭代时间</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='sfiteration' size=1 style='width:80px'>\n");
	fprintf(fs, "					<option value=0>800ms</option>\n");
	fprintf(fs, "					<option value=1>400ms</option>\n");
	fprintf(fs, "					<option value=2>200ms</option>\n");
	fprintf(fs, "					<option value=3>100ms</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>过滤器阀值</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='sfcounter' size=1 style='width:80px'>\n");
	fprintf(fs, "					<option value=5>256</option>\n");
	fprintf(fs, "					<option value=4>128</option>\n");
	fprintf(fs, "					<option value=0>64</option>\n");
	fprintf(fs, "					<option value=1>32</option>\n");
	fprintf(fs, "					<option value=2>16</option>\n");
	fprintf(fs, "					<option value=3>8</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>过滤器重置条件</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='sfreset' size=1 style='width:120px'>\n");
	fprintf(fs, "					<option value=0>Timer Or Packet</option>\n");
	fprintf(fs, "					<option value=1>Only Timer</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "  	</table>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "<h3>MAC地址限制</h3>\n");
	fprintf(fs, "<div>\n");
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata' colspan=3><b>基于每CNU的MAC地址限制</b></td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata' colspan=2>状态</td>\n");
	fprintf(fs, "			<td class='diagdata' width=300>\n");
	fprintf(fs, "				<select id='mlsysen' size=1>\n");
	fprintf(fs, "					<option value=0>禁用</option>\n");
	fprintf(fs, "					<option value=1>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata' colspan=2>最大允许桥接的MAC地址数</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='mlsysmax' size='10'> [范围：0~127]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata' colspan=3></td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata' colspan=3><b>基于CNU端口的MAC地址限制</b></td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata' width=150>端口</td>\n");
	fprintf(fs, "			<td class='diagdata' width=150>&nbsp;状态</td>\n");
	fprintf(fs, "			<td class='diagdata' width=300>&nbsp;最大允许桥接的MAC地址数</td>\n");
	fprintf(fs, "		</tr>\n");	
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH1</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='mleth1en' size=1>\n");
	fprintf(fs, "					<option value=0>禁用</option>\n");
	fprintf(fs, "					<option value=1>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='mleth1max' size='10'> [范围：0~31]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH2</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='mleth2en' size=1>\n");
	fprintf(fs, "					<option value=0>禁用</option>\n");
	fprintf(fs, "					<option value=1>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='mleth2max' size='10'> [范围：0~31]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH3</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='mleth3en' size=1>\n");
	fprintf(fs, "					<option value=0>禁用</option>\n");
	fprintf(fs, "					<option value=1>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='mleth3max' size='10'> [范围：0~31]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH4</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='mleth4en' size=1>\n");
	fprintf(fs, "					<option value=0>禁用</option>\n");
	fprintf(fs, "					<option value=1>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='mleth4max' size='10'> [范围：0~31]</td>\n");
	fprintf(fs, "		</tr>\n");	
	fprintf(fs, "	</table>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "<h3>环路检测</h3>\n");
	fprintf(fs, "<div>\n");
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata' width=300>启用环路检测</td>\n");
	fprintf(fs, "			<td class='diagdata' colspan=2>\n");
	fprintf(fs, "				<select id='loopDetectStatus' size=1>\n");
	fprintf(fs, "					<option value='0'>禁用</option>\n");
	fprintf(fs, "					<option value='1'>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>环路检测SID</td>\n");
	fprintf(fs, "			<td class='diagdata'><input type='text' id='swsid'></td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='sidQuickSetup' size=1 onclick='sidSetup()'>\n");
	fprintf(fs, "					<option value='0'>快捷设置</option>\n");
	fprintf(fs, "					<option value='52:54:4C:83:05:C0'>默认值</option>\n");
	fprintf(fs, "					<option value='%s'>推荐值</option>\n", mySid);
	fprintf(fs, "					<option value=''>自定义</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH1环路状态</td>\n");
	fprintf(fs, "			<td class='diagdata' colspan=2>\n");
	fprintf(fs, "				<select id='eth1LoopStatus' disabled='disabled'>\n");
	fprintf(fs, "					<option value='0'>否</option>\n");
	fprintf(fs, "					<option value='1'>是</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH2环路状态</td>\n");
	fprintf(fs, "			<td class='diagdata' colspan=2>\n");
	fprintf(fs, "				<select id='eth2LoopStatus' disabled='disabled'>\n");
	fprintf(fs, "					<option value='0'>否</option>\n");
	fprintf(fs, "					<option value='1'>是</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH3环路状态</td>\n");
	fprintf(fs, "			<td class='diagdata' colspan=2>\n");
	fprintf(fs, "				<select id='eth3LoopStatus' disabled='disabled'>\n");
	fprintf(fs, "					<option value='0'>否</option>\n");
	fprintf(fs, "					<option value='1'>是</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH4环路状态</td>\n");
	fprintf(fs, "			<td class='diagdata' colspan=2>\n");
	fprintf(fs, "				<select id='eth4LoopStatus' disabled='disabled'>\n");
	fprintf(fs, "					<option value='0'>否</option>\n");
	fprintf(fs, "					<option value='1'>是</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "  	</table>\n");
	fprintf(fs, "  </div>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<button id='btn_return'>返 回</button>\n");
	fprintf(fs, "	<button id='btn_read'>读 取</button>\n");
	fprintf(fs, "	<button id='btn_write'>应 用</button>\n");
	fprintf(fs, "	<button id='btn_erase'>恢复出厂设置</button>\n");
	fprintf(fs, "	<button id='opener'>帮助</button>\n");
	fprintf(fs, "</p>\n");
	fprintf(fs, "<div id='dialog' title='帮助信息'>\n");
	fprintf(fs, "通过本页面，您可以查看和修改用户端口的配置。<br><br>\n");
	fprintf(fs, "1. 点击 '读取' 按钮可以获取该用户端口的当前配置信息；<br>\n");
	fprintf(fs, "2. 点击 '应用' 按钮可以将您修改的配置信息加载到设备上<br>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "</blockquote>\n");
	fprintf(fs, "</body>\n");
	fprintf(fs, "</html>\n");
	
	fflush(fs);	
}


void cgiTemplateMgmt(char *query, FILE *fs)
{
	int id = 0;
	st_dbsCnu cnu;
	uint8_t *p = NULL;
	uint32_t iTemp = 0;
	uint8_t mymac[6] = {0};
	uint8_t mySid[32] = {0};
	st_dbsTemplate template;

		
	

	memset(&template, 0, sizeof(st_dbsTemplate));
		
	//get 1 row is templage management row
	dbsGetTemplate(dbsdev, 1, &template);
	
	fprintf(fs, "<html>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'/>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'/>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript' src='util.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");
	fprintf(fs, "function frmLoad(){\n");
	fprintf(fs, "	$('#col_tempAutoSts').val(%d);\n", template.col_tempAutoSts);
	fprintf(fs, "	$('#col_curTemp').val(%d);\n", template.col_curTemp);
	fprintf(fs, "	$('#col_eth1VlanAddSts').val(%d);\n", template.col_eth1VlanAddSts);
	fprintf(fs, "	$('#col_eth1VlanStart').val(%d);\n", template.col_eth1VlanStart);
	fprintf(fs, "	$('#col_eth2VlanAddSts').val(%d);\n", template.col_eth2VlanAddSts);
	fprintf(fs, "	$('#col_eth2VlanStart').val(%d);\n", template.col_eth2VlanStart);
	fprintf(fs, "	$('#col_eth3VlanAddSts').val(%d);\n", template.col_eth3VlanAddSts);
	fprintf(fs, "	$('#col_eth3VlanStart').val(%d);\n", template.col_eth3VlanStart);
	fprintf(fs, "	$('#col_eth4VlanAddSts').val(%d);\n", template.col_eth4VlanAddSts);
	fprintf(fs, "	$('#col_eth4VlanStart').val(%d);\n", template.col_eth4VlanStart);
    fprintf(fs, "    if($('#col_tempAutoSts').val() ==0){\n");
    fprintf(fs, "        $('#col_eth1VlanAddSts').attr('disabled', 'disabled');\n");  
    fprintf(fs, "        $('#col_eth1VlanStart').attr('disabled', 'disabled');\n");  
    fprintf(fs, "        $('#col_eth2VlanAddSts').attr('disabled', 'disabled');\n");  
    fprintf(fs, "        $('#col_eth2VlanStart').attr('disabled', 'disabled');\n");  
    fprintf(fs, "        $('#col_eth3VlanAddSts').attr('disabled', 'disabled');\n");  
    fprintf(fs, "        $('#col_eth3VlanStart').attr('disabled', 'disabled');\n");  
    fprintf(fs, "        $('#col_eth4VlanAddSts').attr('disabled', 'disabled');\n");  
    fprintf(fs, "        $('#col_eth4VlanStart').attr('disabled', 'disabled');\n");      
    fprintf(fs, "	     $('#col_eth1VlanAddSts').val(%d);\n", 0);
    fprintf(fs, "	     $('#col_eth1VlanStart').val(%d);\n", 1);
    fprintf(fs, "	     $('#col_eth2VlanAddSts').val(%d);\n", 0);
    fprintf(fs, "	     $('#col_eth2VlanStart').val(%d);\n", 1);
    fprintf(fs, "	     $('#col_eth3VlanAddSts').val(%d);\n", 0);
    fprintf(fs, "	     $('#col_eth3VlanStart').val(%d);\n", 1);
    fprintf(fs, "	     $('#col_eth4VlanAddSts').val(%d);\n", 0);
    fprintf(fs, "	     $('#col_eth4VlanStart').val(%d);\n", 1);
    fprintf(fs, "     }\n");  
	fprintf(fs, "   $('#col_tempAutoSts').change(function(){\n");  
	fprintf(fs, "     if($('#col_tempAutoSts').val() != 0){\n");  	  
	fprintf(fs, "        $('#col_eth1VlanAddSts').removeAttr('disabled');\n");  
	fprintf(fs, "        $('#col_eth1VlanStart').removeAttr('disabled');\n");  
    fprintf(fs, "        $('#col_eth2VlanAddSts').removeAttr('disabled');\n");  
    fprintf(fs, "        $('#col_eth2VlanStart').removeAttr('disabled');\n");  
    fprintf(fs, "        $('#col_eth3VlanAddSts').removeAttr('disabled');\n");  
    fprintf(fs, "        $('#col_eth3VlanStart').removeAttr('disabled');\n");  
    fprintf(fs, "        $('#col_eth4VlanAddSts').removeAttr('disabled');\n");  
    fprintf(fs, "        $('#col_eth4VlanStart').removeAttr('disabled');\n");  
    fprintf(fs, "	  }else{\n");  
    fprintf(fs, "        $('#col_eth1VlanAddSts').attr('disabled', 'disabled');\n");  
    fprintf(fs, "        $('#col_eth1VlanStart').attr('disabled', 'disabled');\n");  
    fprintf(fs, "        $('#col_eth2VlanAddSts').attr('disabled', 'disabled');\n");  
    fprintf(fs, "        $('#col_eth2VlanStart').attr('disabled', 'disabled');\n");  
    fprintf(fs, "        $('#col_eth3VlanAddSts').attr('disabled', 'disabled');\n");  
    fprintf(fs, "        $('#col_eth3VlanStart').attr('disabled', 'disabled');\n");  
    fprintf(fs, "        $('#col_eth4VlanAddSts').attr('disabled', 'disabled');\n");  
    fprintf(fs, "        $('#col_eth4VlanStart').attr('disabled', 'disabled');\n");  
    fprintf(fs, "	     $('#col_eth1VlanAddSts').val(%d);\n", 0);
    fprintf(fs, "	     $('#col_eth1VlanStart').val(%d);\n", 1);
    fprintf(fs, "	     $('#col_eth2VlanAddSts').val(%d);\n", 0);
    fprintf(fs, "	     $('#col_eth2VlanStart').val(%d);\n", 1);
    fprintf(fs, "	     $('#col_eth3VlanAddSts').val(%d);\n", 0);
    fprintf(fs, "	     $('#col_eth3VlanStart').val(%d);\n", 1);
    fprintf(fs, "	     $('#col_eth4VlanAddSts').val(%d);\n", 0);
    fprintf(fs, "	     $('#col_eth4VlanStart').val(%d);\n", 1);
    fprintf(fs, "     }\n");  
    fprintf(fs, "   });\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function hidePortStatusInfo(hide){\n");
	fprintf(fs, "	if( hide ) $('#PortStatusInfo').hide();\n");
	fprintf(fs, "	else $('#PortStatusInfo').show();\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function cnuPermitClick(){\n");
	fprintf(fs, "	if ( $('#cnuPermition').val() == 1 )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		$('#eth1portsts').val(1);\n");
	fprintf(fs, "		$('#eth2portsts').val(1);\n");
	fprintf(fs, "		$('#eth3portsts').val(1);\n");
	fprintf(fs, "		$('#eth4portsts').val(1);\n");
	fprintf(fs, "		hidePortStatusInfo(false);\n");
	fprintf(fs, "	}else{\n");	
	fprintf(fs, "		hidePortStatusInfo(true);\n");
	fprintf(fs, "		$('#eth1portsts').val(0);\n");
	fprintf(fs, "		$('#eth2portsts').val(0);\n");
	fprintf(fs, "		$('#eth3portsts').val(0);\n");
	fprintf(fs, "		$('#eth4portsts').val(0);\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "}\n");

	fprintf(fs, "function sidSetup(){\n");
	fprintf(fs, "	var mysid = $('#sidQuickSetup').val();\n");
	fprintf(fs, "	var myselectid = $('#sidQuickSetup')[0].selectedIndex;\n");
	fprintf(fs, "	if(myselectid==1)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		$('#swsid').val(mysid);\n");
	fprintf(fs, "		$('#swsid').attr('disabled', 'disabled');\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	else if(myselectid==2)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		$('#swsid').val(mysid);\n");
	fprintf(fs, "		$('#swsid').attr('disabled', 'disabled');\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	else if(myselectid==3)\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		$('#swsid').val(mysid);\n");
	fprintf(fs, "		$('#swsid').removeAttr('disabled');\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "function Return(){\n");
	fprintf(fs, "	var loc = 'cnuManagement.cmd?cnuid=';\n");
	fprintf(fs, "	loc += %d;\n", glbWebVar.cnuid);
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function Read(){\n");
	fprintf(fs, "	$('#btn_read').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_write').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_return').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_erase').attr('disabled', true);\n");
	fprintf(fs, "	var loc = 'rtl8306eConfigRead.cgi?';\n");
	fprintf(fs, "	loc += 'cnuid=' + %d;\n", glbWebVar.cnuid);
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function Write(){\n");
	fprintf(fs, "	var msg;\n");
	//fprintf(fs, "	var loc = 'wecTemplateSave.cmd?';\n");
	fprintf(fs, "	var loc = 'saveTemplate.cgi?';\n");
	fprintf(fs, "	if($('#col_tempAutoSts').val() != 0){\n");
	fprintf(fs, "		loc += 'col_tempAutoSts=1';\n");
	fprintf(fs, "		loc += '&col_curTemp=1';\n");	
	fprintf(fs, "	}else{\n");	
	fprintf(fs, "		loc += 'col_tempAutoSts=0';\n");
	fprintf(fs, "		loc += '&col_curTemp=1';\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	if($('#col_eth1VlanAddSts').val() != 0){\n");
	fprintf(fs, "		loc += '&col_eth1VlanAddSts=1';\n");
	fprintf(fs, "	}else{\n");		
	fprintf(fs, "		loc += '&col_eth1VlanAddSts=0';\n");
	fprintf(fs, "	}\n");	
	fprintf(fs, "   loc += '&col_eth1VlanStart=' + $('#col_eth1VlanStart').val();\n");		
	fprintf(fs, "	if($('#col_eth2VlanAddSts').val() != 0){\n");
	fprintf(fs, "		loc += '&col_eth2VlanAddSts=1';\n");
	fprintf(fs, "		loc += '&col_eth2VlanStart=' + $('#col_eth2VlanStart').val();\n");			
	fprintf(fs, "	}else{\n");	
	fprintf(fs, "		loc += '&col_eth2VlanAddSts=0';\n");
	fprintf(fs, "		loc += '&col_eth2VlanStart=' + $('#col_eth2VlanStart').val();\n");			
	fprintf(fs, "	}\n");	
	fprintf(fs, "	if($('#col_eth3VlanAddSts').val() != 0){\n");
	fprintf(fs, "		loc += '&col_eth3VlanAddSts=1';\n");
	fprintf(fs, "		loc += '&col_eth3VlanStart=' + $('#col_eth3VlanStart').val();\n");		
	fprintf(fs, "	}else{\n");	
	fprintf(fs, "		loc += '&col_eth3VlanAddSts=0';\n");
	fprintf(fs, "		loc += '&col_eth3VlanStart=' + $('#col_eth3VlanStart').val();\n");		
	fprintf(fs, "	}\n");	
	fprintf(fs, "	if($('#col_eth4VlanAddSts').val() != 0){\n");
	fprintf(fs, "		loc += '&col_eth4VlanAddSts=1';\n");
	fprintf(fs, "		loc += '&col_eth4VlanStart=' + $('#col_eth4VlanStart').val();\n");		
	fprintf(fs, "	}else{\n");	
	fprintf(fs, "		loc += '&col_eth4VlanAddSts=0';\n");
	fprintf(fs, "		loc += '&col_eth4VlanStart=' + $('#col_eth4VlanStart').val();\n");		
	fprintf(fs, "	}\n");	
	fprintf(fs, "	$('#btn_read').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_write').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_return').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_erase').attr('disabled', true);\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");	
	fprintf(fs, "function restoreDefault(){\n");
	fprintf(fs, "	$('#btn_read').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_write').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_return').attr('disabled', true);\n");
	fprintf(fs, "	$('#btn_erase').attr('disabled', true);\n");
	fprintf(fs, "	var loc = 'rtl8306eConfigErase.cgi?';\n");
	fprintf(fs, "	loc += 'cnuid=' + %d;\n", glbWebVar.cnuid);
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");	
	fprintf(fs, "$(function(){\n");
	fprintf(fs, "	frmLoad();\n");
	fprintf(fs, "	$('#accordion').accordion({\n");
	fprintf(fs, "		collapsible: true,\n");
	fprintf(fs, "		heightStyle: 'content'\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#dialog').dialog({\n");
	fprintf(fs, "		autoOpen: false,\n");
	fprintf(fs, "		width:370,\n");
	fprintf(fs, "		show:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: 'blind',\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "		hide:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: 'explode',\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#opener').button(\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			icons:\n");
	fprintf(fs, "			{\n");
	fprintf(fs, "				primary: 'ui-icon-help'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "			text: false\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	).click(function(){\n");
	fprintf(fs, "		$('#dialog').dialog('open');\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#btn_return').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-carat-1-w'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#btn_return').button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		Return();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#btn_read').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-arrow-4-diag'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#btn_read').button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		Read();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#btn_write').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-tag'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{
		fprintf(fs, "	$('#btn_write').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
	}
	else
	{
		fprintf(fs, "	$('#btn_write').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		Write();\n");
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "	$('#btn_erase').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-refresh'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{	
		fprintf(fs, "	$('#btn_erase').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
	}
	else
	{
		fprintf(fs, "	$('#btn_erase').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		restoreDefault();\n");
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "});\n");
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "	<tr><td class='maintitle'><b>模板</b></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0 width='100%'>\n");
	fprintf(fs, "	<tr><td class='mainline' width='100%'></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<br><br>\n");
	fprintf(fs, "<div id='accordion'>  \n");
	fprintf(fs, "<h3>模板管理与VLAN自增</h3>\n");
	fprintf(fs, "<div>\n");
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>启用模板自动下发功能</td>\n");
	fprintf(fs, "			<td class='diagdata' >\n");
	fprintf(fs, "				<select id='col_tempAutoSts' size=1>\n");
	fprintf(fs, "					<option value='0'>禁用</option>\n");
	fprintf(fs, "					<option value='1'>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr><td class='diagdata' colspan=3>&nbsp;</td></tr>\n");
	fprintf(fs, "		<tr><td class='diagdata' colspan=3>&nbsp;</td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table  border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "		<tr><td class='diagdata' colspan=3>&nbsp;</td></tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata' width=150>端口</td>\n");
	fprintf(fs, "			<td class='diagdata' width=150>VLAN自增</td>\n");	
	fprintf(fs, "			<td class='diagdata' width=220 display>&nbsp;起始VLAN ID</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH1</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='col_eth1VlanAddSts' size=1>\n");
	fprintf(fs, "					<option value='0'>禁用</option>\n");
	fprintf(fs, "					<option value='1'>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");	
	fprintf(fs, "			<td class='diagdata' width=220 ><input id='col_eth1VlanStart' type='text' value='0'  size='8'>[范围：1~4030]</td>\n");	
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH2</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='col_eth2VlanAddSts' size=1>\n");
	fprintf(fs, "					<option value='0'>禁用</option>\n");
	fprintf(fs, "					<option value='1'>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");		
	fprintf(fs, "			<td class='diagdata' width=220><input type='text' value='0' id='col_eth2VlanStart' size='8'>[范围：1~4030]</td>\n");	
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH3</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='col_eth3VlanAddSts' size=1>\n");
	fprintf(fs, "					<option value='0'>禁用</option>\n");
	fprintf(fs, "					<option value='1'>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");		
	fprintf(fs, "			<td class='diagdata' width=220><input type='text' value='0' id='col_eth3VlanStart' size='8'>[范围：1~4030]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "		<tr>\n");
	fprintf(fs, "			<td class='diagdata'>ETH4</td>\n");
	fprintf(fs, "			<td class='diagdata'>\n");
	fprintf(fs, "				<select id='col_eth4VlanAddSts' size=1>\n");
	fprintf(fs, "					<option value='0'>禁用</option>\n");
	fprintf(fs, "					<option value='1'>启用</option>\n");
	fprintf(fs, "				</select>\n");
	fprintf(fs, "			</td>\n");		
	fprintf(fs, "			<td class='diagdata' width=220><input type='text' value='0' id='col_eth4VlanStart' size='8'>[范围：1~4030]</td>\n");
	fprintf(fs, "		</tr>\n");
	fprintf(fs, "	</table>\n");
	fprintf(fs, "  </div>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<button id='btn_write'>确 定</button>\n");
	fprintf(fs, "	<button id='opener'>帮助</button>\n");
	fprintf(fs, "</p>\n");
	fprintf(fs, "<div id='dialog' title='帮助信息'>\n");
	fprintf(fs, "通过本页面，您可以查看和修改模板自动下发及VLAN自增功能。<br><br>\n");
	fprintf(fs, "1. 如果启用自动下发，则发现新在线用户终端将自动下发VLAN配置；<br>\n");
	fprintf(fs, "2. 如果启用VLAN自增功能，将从起始VLAN开始每发现一个终端上线，则VLAN自动加一，并下发<br>\n");
	fprintf(fs, "3. 如果禁止VLAN自增功能，将从使用配置的端口VLAN开始每发现一个终端上线，并自动下发<br>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "</blockquote>\n");
	fprintf(fs, "</body>\n");
	fprintf(fs, "</html>\n");
	
	fflush(fs);	
}


void cgiCltMgmt(char *query, FILE *fs)
{	
	int id = 0;
	st_dbsClt clt;
	char action[IFC_LARGE_LEN];

	cgiGetValueByName(query, "cltid", action);
	id = atoi(action);

	dbsGetClt(dbsdev, id, &clt);

	fprintf(fs, "<html>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'/>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'/>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");
	fprintf(fs, "function btnReturn(){\n");
	fprintf(fs, "	location.href = 'previewTopology.cgi';\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function btnReboot(cltid){\n");
	fprintf(fs, "	var loc = 'cltReboot.cgi?cltid=';\n");
	fprintf(fs, "	loc += cltid;\n");
	fprintf(fs, "	if(!confirm('您确定要重启CLT模块吗？')) return;\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function btnConfig(cltid){\n");
	fprintf(fs, "	var loc = 'editCltPro.cmd?cltid=';\n");
	fprintf(fs, "	loc += cltid;\n");
	fprintf(fs, "	        loc = 'readCltFreq.cgi?cltid=' + cltid;\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "$(function(){\n");
	fprintf(fs, "	$('#accordion').accordion({\n");
	fprintf(fs, "		collapsible: true,\n");
	fprintf(fs, "		heightStyle: 'content'\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#dialog').dialog({\n");
	fprintf(fs, "		autoOpen: false,\n");
	fprintf(fs, "		width:360,\n");
	fprintf(fs, "		show:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: 'blind',\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "		hide:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: 'explode',\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#opener').button(\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			icons:\n");
	fprintf(fs, "			{\n");
	fprintf(fs, "				primary: 'ui-icon-help'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "			text: false\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	).click(function(){\n");
	fprintf(fs, "		$('#dialog').dialog('open');\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#returnBtn').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-carat-1-w'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#returnBtn').button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnReturn();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#rebootBtn').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-refresh'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{
		fprintf(fs, "	$('#rebootBtn').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
	}
	else
	{
		fprintf(fs, "	$('#rebootBtn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		btnReboot(%d);\n", id);
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "	$('#configBtn').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-tag'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#configBtn').button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnConfig(%d);\n", id);
	fprintf(fs, "	});\n");
	fprintf(fs, "});\n");
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "	<tr><td class='maintitle'><b>CLT信息</b></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0 width='100%'>\n");
	fprintf(fs, "	<tr><td class='mainline' width='100%'></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<div id='accordion'>\n");
	fprintf(fs, "	<h3>CLT调制模块基本信息</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<table border=0 cellpadding=3 cellspacing=0>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td width=280>索引号</td>\n");
	fprintf(fs, "				<td width=200>CLT/%d</td>\n", id);
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>调制模块MAC地址</td>\n");
	fprintf(fs, "				<td>%s</td>\n", clt.col_mac);
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>运行状态</td>\n");
	if( 0 == clt.col_row_sts )
	{
		fprintf(fs, "				<td><img src='suppress.png'></td>\n");
	}
	else
	{
		fprintf(fs, "				<td><img src='%s'></td>\n", clt.col_sts?"true.png":"net_down.gif");
	}
	
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>最大用户数</td>\n");
	fprintf(fs, "				<td>%d</td>\n", clt.col_maxStas);
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>当前在线用户数</td>\n");
	fprintf(fs, "				<td>%d</td>\n", clt.col_numStas);
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>调制芯片型号</td>\n");
	fprintf(fs, "				<td>AR741x</td> \n");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>调制模块软件版本</td>\n");
	fprintf(fs, "				<td>ar741x-v7.1.3-1-X-FINAL</td> \n");
	fprintf(fs, "			</tr>	\n");
	fprintf(fs, "		</table>\n");
	fprintf(fs, "		<br>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<button id='returnBtn'>返 回</button>\n");
	fprintf(fs, "	<button id='rebootBtn'>重 启</button>\n");
	fprintf(fs, "	<button id='configBtn'>配 置</button>\n");
	fprintf(fs, "	<button id='opener'>帮助</button>\n");
	fprintf(fs, "</p>\n");
	fprintf(fs, "<div id='dialog' title='帮助信息'>\n");
	fprintf(fs, "	<br>1. 点击 \"重启\" 按钮可以重新启动该调制模块；</br>\n");
	fprintf(fs, "	<br>2. 点击 \"配置\" 按钮可以配置该调制模块的相关参数。</br>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "</blockquote>\n");
	fprintf(fs, "</body>\n");
	fprintf(fs, "</html>\n");
	
	fflush(fs);	
}


void cgiCnuMgmt(char *query, FILE *fs)
{
	int i=0;
	int isCnuSupported = BOOL_TRUE;	
	int iCount = 0;

	
	int id = 0;
	int cltid = 0;
	int cnuid = 0;
	st_dbsCnu cnu;
	st_dbsProfile profile;
	char action[IFC_LARGE_LEN];
	T_szCnuUserHFID cnuuserhfid;
	st_rtl8306e_port_status linkstatus;

	cgiGetValueByName(query, "cnuid", action);
	id = atoi(action);

	cltid = (id-1)/MAX_CNUS_PER_CLT+1;
	cnuid = (id-1)%MAX_CNUS_PER_CLT+1;

	linkstatus.id = id;
	
	dbsGetCnu(dbsdev, id, &cnu);
	dbsGetProfile(dbsdev, id, &profile);
	if ( boardapi_isAr6400Device(cnu.col_model) != 1)
	{
		if (  cnu.col_sts == 1)
		{
			boardapi_macs2b(cnu.col_mac, cnuuserhfid.ODA);
			http2cmm_getCnuUserHFID(&cnuuserhfid);
		}
	}

	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'/>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'/>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>	\n");
	fprintf(fs, "function btnReturn(){\n");
	fprintf(fs, "	location.href = 'previewTopology.cgi';\n");
	fprintf(fs, "}\n");
	if(CNU_SWITCH_TYPE_AR8236 == boardapi_getCnuSwitchType(cnu.col_model))
	{
		fprintf(fs, "function btnPermit(cnuid, permition){\n");
		fprintf(fs, "	var loc;\n");
		fprintf(fs, "	if(0==permition)\n");
		fprintf(fs, "	{\n");
		fprintf(fs, "		loc = 'cnuUndoPermit.cgi?cnuid=';\n");
		fprintf(fs, "	}\n");
		fprintf(fs, "	else\n");
		fprintf(fs, "	{\n");
		fprintf(fs, "		loc = 'cnuPermit.cgi?cnuid=';\n");
		fprintf(fs, "	}\n");
		fprintf(fs, "	loc += cnuid;\n");
		fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "	//alert(code);\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		fprintf(fs, "function btnReload(cnuid){\n");
		fprintf(fs, "	var loc = 'cnuReload.cgi?cnuid=';\n");
		fprintf(fs, "	loc += cnuid;\n");
		fprintf(fs, "	if(!confirm('您确定需要重新加载该用户的配置吗？')) return;\n");
		fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "	//alert(code);\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
	}
	fprintf(fs, "function btnReboot(cnuid){\n");
	fprintf(fs, "	var loc = 'cnuReboot.cgi?cnuid=';\n");
	fprintf(fs, "	loc += cnuid;\n");
	fprintf(fs, "	if(!confirm('您确定要重新启动该终端设备吗？')) return;\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	if( 0 == cnu.col_sts )
	{
		//you can delete this cnu if cnu is off-line
		fprintf(fs, "function btnDelete(cnuid){\n");
		fprintf(fs, "	var loc = 'cnuDelete.cgi?cnuid=';\n");
		fprintf(fs, "	loc += cnuid;\n");
		fprintf(fs, "	if(!confirm('您确定要将该用户从拓扑表中删除吗？')) return;\n");
		fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "	//alert(code);\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
		//you cannot config off-line e4 cnu
		fprintf(fs, "function btnConfig(cnuid, modType){\n");
		fprintf(fs, "	if(0==modType){\n");
		fprintf(fs, "		var loc = 'editCnuPro.cmd?cnuid=' + cnuid;\n");
		fprintf(fs, "		var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "		//alert(code);\n");
		fprintf(fs, "		eval(code);\n");
		fprintf(fs, "	}\n");
		fprintf(fs, "	else{\n");
		fprintf(fs, "		var msg = '警告：禁止配置该离线设备！';\n");
		fprintf(fs, "		alert(msg);\n");
		fprintf(fs, "	}\n");
		fprintf(fs, "}\n");
	}
	else
	{
		//you can not delete this cnu if cnu is online
		fprintf(fs, "function btnDelete(cnuid){\n");
		fprintf(fs, "	var msg = '警告：禁止删除在线设备！';\n");
		fprintf(fs, "	alert(msg);\n");		
		fprintf(fs, "}\n");
		//you can config online cnu
		fprintf(fs, "function btnConfig(cnuid, modType){\n");
		fprintf(fs, "	var loc;\n");
		fprintf(fs, "	if(0==modType)\n");
		fprintf(fs, "		loc = 'editCnuPro.cmd?cnuid=' + cnuid;\n");
		fprintf(fs, "	else{\n");
		fprintf(fs, "		loc = 'rtl8306eConfig.cgi?cnuid=' + cnuid;	\n");
		fprintf(fs, "	        loc = 'rtl8306eConfigRead.cgi?cnuid=' + cnuid;\n");
		fprintf(fs, "             }\n");
		fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
		fprintf(fs, "	//alert(code);\n");
		fprintf(fs, "	eval(code);\n");
		fprintf(fs, "}\n");
	}	
	fprintf(fs, "$(function(){\n");
	fprintf(fs, "	$('#accordion').accordion({\n");
	fprintf(fs, "		collapsible: true,\n");
	fprintf(fs, "		heightStyle: 'content'\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#dialog').dialog({\n");
	fprintf(fs, "		autoOpen: false,\n");
	fprintf(fs, "		width:440,\n");
	fprintf(fs, "		show:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: 'blind',\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "		hide:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: 'explode',\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	});	\n");
	fprintf(fs, "	$('#opener').button(\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			icons:\n");
	fprintf(fs, "			{\n");
	fprintf(fs, "				primary: 'ui-icon-help'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "			text: false\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	).click(function(){\n");
	fprintf(fs, "		$('#dialog').dialog('open');\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#returnBtn').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-carat-1-w'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#returnBtn').button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnReturn();\n");
	fprintf(fs, "	});\n");
	if(CNU_SWITCH_TYPE_AR8236 == boardapi_getCnuSwitchType(cnu.col_model))
	{
		fprintf(fs, "	$('#permitBtn').button({\n");
		fprintf(fs, "			icons: {\n");
		fprintf(fs, "				primary: 'ui-icon-person'\n");
		fprintf(fs, "			},\n");
		fprintf(fs, "	});\n");
		if(strcmp(glbWebVar.curUserName,"user")  == 0)
		{
			fprintf(fs, "	$('#permitBtn').button().click(function(event){\n");
			fprintf(fs, "       		alert('您没有足够的权限...');\n");
			fprintf(fs, "	});\n");
		}
		else
		{
			fprintf(fs, "	$('#permitBtn').button().click(function(event){\n");
			fprintf(fs, "		event.preventDefault();\n");
			fprintf(fs, "		btnPermit(%d, %d);\n", id, cnu.col_auth?0:1);
			fprintf(fs, "	});\n");
		}
		fprintf(fs, "	$('#reloadBtn').button({\n");
		fprintf(fs, "			icons: {\n");
		fprintf(fs, "				primary: 'ui-icon-extlink'\n");
		fprintf(fs, "			},\n");
		fprintf(fs, "	});\n");
		if(strcmp(glbWebVar.curUserName,"user")  == 0)
		{
			fprintf(fs, "	$('#reloadBtn').button().click(function(event){\n");
			fprintf(fs, "       		alert('您没有足够的权限...');\n");
			fprintf(fs, "	});\n");
		}
		else
		{
			fprintf(fs, "	$('#reloadBtn').button().click(function(event){\n");
			fprintf(fs, "		event.preventDefault();\n");
			fprintf(fs, "		btnReload(%d);\n", id);
			fprintf(fs, "	});\n");
		}
	}
	fprintf(fs, "	$('#rebootBtn').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-refresh'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{
		fprintf(fs, "	$('#rebootBtn').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
	}
	else
	{
		fprintf(fs, "	$('#rebootBtn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		btnReboot(%d);\n", id);
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "	$('#configBtn').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-tag'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#configBtn').button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnConfig(%d, %d);\n", id, boardapi_getCnuSwitchType(cnu.col_model));
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#delBtn').button({\n");
	fprintf(fs, "			icons: {\n");
	fprintf(fs, "				primary: 'ui-icon-trash'\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "	});\n");
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{
		fprintf(fs, "	$('#delBtn').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
	}
	else
	{
		fprintf(fs, "	$('#delBtn').button().click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		btnDelete(%d);\n", id);
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "});\n");
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "	<tr><td class='maintitle'><b>CNU信息</b></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0 width='100%'>\n");
	fprintf(fs, "	<tr><td class='mainline' width='100%'></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<br><br>\n");
	fprintf(fs, "<div id='accordion'>\n");
	fprintf(fs, "	<h3>CNU基本信息</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<table border=0 cellpadding=3 cellspacing=0>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td width=280>索引号</td>\n");
	fprintf(fs, "				<td width=200>CNU/%d/%d</td>\n", cltid, cnuid);
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>设备型号</td>\n");
	fprintf(fs, "				<td>%s</td>\n", boardapi_getDeviceModelStr(cnu.col_model) == "UNKNOWN" ? cnu.col_user_hfid : boardapi_getDeviceModelStr(cnu.col_model));
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>MAC地址</td>\n");
	fprintf(fs, "				<td>%s</td>\n", cnu.col_mac);
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>模板索引</td>\n");
	fprintf(fs, "				<td>%d</td>\n", id);
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>PIB参数块索引:</td>\n");
	fprintf(fs, "				<td>%d</td>\n", profile.col_base);
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>模板启用状态</td>\n");
	fprintf(fs, "				<td>%s</td> \n", profile.col_row_sts?"启用":"禁用");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>模板运行状态</td>\n");
	fprintf(fs, "				<td>%s</td> \n", boardapi_getCnuSwitchType(cnu.col_model)?"否":"是");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>授权状态</td>\n");
	fprintf(fs, "				<td>%s</td>\n", cnu.col_auth?"允许":"禁止");
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>物理层RX/TX传输速率</td>\n");
	fprintf(fs, "				<td>%d/%d</td>\n", cnu.col_rx, cnu.col_tx);
	fprintf(fs, "			</tr>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td>在线状态</td>\n");
	fprintf(fs, "				<td>%s</td>\n", cnu.col_sts?"在线":"离线");
	fprintf(fs, "			</tr>	\n");	
	if(boardapi_isAr7400Device(cnu.col_model))
	{
		fprintf(fs, "			<tr>\n");
		fprintf(fs, "				<td>调制芯片型号</td>\n");
		fprintf(fs, "				<td>AR741x</td> \n");
		fprintf(fs, "			</tr>\n");
		fprintf(fs, "			<tr>\n");
		fprintf(fs, "				<td>软件版本</td>\n");
		fprintf(fs, "				<td>AR7400-v7.1.1-1-X-FINAL</td>\n");
		fprintf(fs, "			</tr>\n");
	}
	else if(boardapi_isAr6400Device(cnu.col_model))
	{
		fprintf(fs, "			<tr>\n");
		fprintf(fs, "				<td>调制芯片型号</td>\n");
		fprintf(fs, "				<td>AR6400</td> \n");
		fprintf(fs, "			</tr>\n");
		fprintf(fs, "			<tr>\n");
		fprintf(fs, "				<td>软件版本</td>\n");
		fprintf(fs, "				<td>INT6000-v4.1.0-0-2-X-FINAL</td>\n");
		fprintf(fs, "			</tr>\n");
	}
	else
	{
		fprintf(fs, "			<tr>\n");
		fprintf(fs, "				<td>调制芯片型号</td>\n");
		fprintf(fs, "				<td>未知</td> \n");
		fprintf(fs, "			</tr>\n");
		fprintf(fs, "			<tr>\n");
		fprintf(fs, "				<td>软件版本</td>\n");
		fprintf(fs, "				<td>未知</td>\n");
		fprintf(fs, "			</tr>\n");
	}	
	if(CNU_SWITCH_TYPE_RTL8306E == boardapi_getCnuSwitchType(cnu.col_model))
	{	
		if ( http2cmm_getRtl8306ePortStatusInfo(&linkstatus) == 0)
		{
			fprintf(fs, " 			<tr>\n");
			fprintf(fs, "				<td>Ethernet1连接状态</td>\n");
			fprintf(fs, "                 		<td><IMG src='%s'></td>\n", linkstatus.port1linkstatus?"net_up.gif":"net_down.gif");
			fprintf(fs, "			</tr>\n");
			fprintf(fs, " 			<tr>\n");
			fprintf(fs, "				<td>Ethernet2连接状态</td>\n");
			fprintf(fs, "                 		<td><IMG src='%s'></td>\n", linkstatus.port2linkstatus?"net_up.gif":"net_down.gif");
			fprintf(fs, "			</tr>\n");
			fprintf(fs, " 			<tr>\n");
			fprintf(fs, "				<td>Ethernet3连接状态</td>\n");
			fprintf(fs, "                 		<td><IMG src='%s'></td>\n", linkstatus.port3linkstatus?"net_up.gif":"net_down.gif");
			fprintf(fs, "			</tr>\n");
			fprintf(fs, " 			<tr>\n");
			fprintf(fs, "				<td>Ethernet4连接状态</td>\n");
			fprintf(fs, "                 		<td><IMG src='%s'></td>\n", linkstatus.port4linkstatus?"net_up.gif":"net_down.gif");
			fprintf(fs, "			</tr>\n");
		}
	}
	fprintf(fs, "		</table>\n");
	fprintf(fs, "		<br>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<button id=returnBtn>返 回</button>\n");
	if(CNU_SWITCH_TYPE_AR8236 == boardapi_getCnuSwitchType(cnu.col_model))
	{
		if(cnu.col_auth)
		{
			fprintf(fs, "	<button id=permitBtn>禁 止</button>\n");
		}
		else
		{
			fprintf(fs, "	<button id=permitBtn>允 许</button>\n");
		}
		fprintf(fs, "	<button id=reloadBtn>重 载</button>\n");
	}	
	fprintf(fs, "	<button id=rebootBtn>重 启</button>\n");
	fprintf(fs, "	<button id=delBtn>删 除</button>\n");
	fprintf(fs, "	<button id=configBtn>配 置</button>\n");
	fprintf(fs, "	<button id=opener>帮助</button>\n");
	fprintf(fs, "</p>\n");
	fprintf(fs, "<div id='dialog' title='帮助信息'>\n");
	fprintf(fs, "通过本页面，您可以完成对CNU设备的管理和维护，如：修改模板配置参数、\n");
	fprintf(fs, "更改授权状态等。<br><br>\n");
	fprintf(fs, "-- 点击 '配置' 按钮可以修改该用户的配置参数；<br>\n");
	fprintf(fs, "-- 点击 '允许'/'禁止' 按钮可以修改该用户的授权状态；<br>\n");
	fprintf(fs, "-- 点击 '重载' 按钮可以重新载入该用户的配置信息；<br>\n");
	fprintf(fs, "-- 点击 '删除' 按钮可以删除该终端用户<br>\n");
	fprintf(fs, "<br><b>注意: </b>禁止删除在线的终端用户。</br>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "</blockquote>\n");
	fprintf(fs, "</body>\n");
	fprintf(fs, "</html>\n");

	fflush(fs);	
}

void cgiLinkDiag(char *query, FILE *fs) 
{
	int i=0;
	//int n = 0;
	int cltid = 0;
	int cnuid = 0;
	//int iCount = 0;
	st_dbsCnu cnu;

	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");
	fprintf(fs, "function btnRefresh()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	location.href = \"previewLinkDiag.cgi\";\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function wecLinkDiag()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	var loc = 'wecLinkDiag.cgi?';\n");
	fprintf(fs, "	if( $(\"#cnuDev\").val() == 0 )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		alert('请选择一个CNU设备！');\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	if( $(\"#fangxiang\").val() == 0 )\n");
	fprintf(fs, "	{\n");
	fprintf(fs, "		alert('请选择链路诊断方向！');\n");
	fprintf(fs, "		return;\n");
	fprintf(fs, "	}\n");
	fprintf(fs, "	loc += 'cnuid=' + $(\"#cnuDev\").val();\n");
	fprintf(fs, "	loc += '&diagDir=' + $(\"#fangxiang\").val();\n");
	fprintf(fs, "	var code = 'location=\"' + loc + '\"';\n");
	fprintf(fs, "	//alert(code);\n");
	fprintf(fs, "	eval(code);\n");
	fprintf(fs, "}\n");
	fprintf(fs, "$(function()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	$(\"#accordion\").accordion({\n");
	fprintf(fs, "		collapsible: true,\n");
	fprintf(fs, "		heightStyle: \"content\" \n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$( \"#dialog\" ).dialog({\n");
	fprintf(fs, "		autoOpen: false,\n");
	fprintf(fs, "		width:450,\n");
	fprintf(fs, "		show:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: \"blind\",\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "		hide:\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			effect: \"explode\",\n");
	fprintf(fs, "			duration: 1000\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#refreshBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-refresh'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$( \"#refreshBtn\" ).button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnRefresh();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#comBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-lightbulb'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$( \"#comBtn\" ).button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		wecLinkDiag();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$( \"#opener\" ).button(\n");
	fprintf(fs, "		{\n");
	fprintf(fs, "			icons:\n");
	fprintf(fs, "			{\n");
	fprintf(fs, "				primary: \"ui-icon-help\"\n");
	fprintf(fs, "			},\n");
	fprintf(fs, "			text: false\n");
	fprintf(fs, "		}\n");
	fprintf(fs, "	).click(function(){\n");
	fprintf(fs, "		$( \"#dialog\" ).dialog( \"open\" );\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "});\n");
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "	<br>\n");
	fprintf(fs, "	<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "		<tr><td class='maintitle'><b>链路诊断</b></td></tr>\n");
	fprintf(fs, "	</table>\n");
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=0 width=100%>  \n");
	fprintf(fs, "		<tr><td class='mainline' width=100%></td></tr>\n");
	fprintf(fs, "	</table>\n");
	fprintf(fs, "	<br>\n");
	fprintf(fs, "	<br>\n");
	fprintf(fs, "	<div id=\"accordion\">\n");
	fprintf(fs, "		<h3>EoC物理链路诊断</h3>\n");
	fprintf(fs, "		<div>\n");
	fprintf(fs, "			<br>\n");
	fprintf(fs, "			<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata' width=300>CNU设备列表</td>\n");
	fprintf(fs, "					<td class='diagdata' width=260>\n");
	fprintf(fs, "						<select id='cnuDev'>\n");
	fprintf(fs, "							<option value=0 selected='selected'>请选择</option>\n");
	for( i=1; i<=MAX_CNU_AMOUNT_LIMIT; i++ )
	{
		cltid = (i-1)/MAX_CNUS_PER_CLT+1;
		cnuid = (i-1)%MAX_CNUS_PER_CLT+1;
		if( dbsGetCnu(dbsdev, i, &cnu) != CMM_SUCCESS )
		{
			/* Read CNU failed, exit */
			break;
		}
		else
		{
			if( cnu.col_row_sts == 0 )
			{
				/*CNU column empty, get next */
				continue;
			}
			else
			{
				//iCount++;
				if( DEV_STS_ONLINE == cnu.col_sts )
				{
					//n = i/MAX_CNUS_PER_CLT+1;
					fprintf(fs, "							<option value='%d'>CNU/%d/%d: [%s]</option>\n", i, cltid, cnuid, cnu.col_mac);
				}
			}
		}
	}
	fprintf(fs, "						</select>\n");
	fprintf(fs, "					</td>\n");
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata'>诊断测试方向</td>\n");
	fprintf(fs, "					<td class='diagdata'>\n");
	fprintf(fs, "						<select id='fangxiang'>\n");
	fprintf(fs, "							<option value=0 selected='selected'>请选择</option>\n");
	fprintf(fs, "							<option value=1>上行链路状况诊断</option>\n");
	fprintf(fs, "							<option value=2>下行链路状况诊断</option>\n");
	fprintf(fs, "						</select>\n");
	fprintf(fs, "					</td>\n");
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "			</table>\n");
	fprintf(fs, "			<br>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "	<p>\n");
	fprintf(fs, "		<button id=refreshBtn>刷 新</button>\n");
	fprintf(fs, "		<button id=comBtn>开始诊断</button>\n");
	fprintf(fs, "		<button id=opener>帮助</button>\n");
	fprintf(fs, "	</p>\n");
	fprintf(fs, "	<div id=dialog title='帮助信息'>\n");
	fprintf(fs, "		通过本页面，您可以完成对终端用户链路状况的诊断。<br>\n");
	fprintf(fs, "		您可以对上行或者下行的链路状况单独进行诊断。请从终端设备下拉列表中选择一个终端用户，\n");
	fprintf(fs, "		然后选择目标诊断方向，点击开始诊断测试按钮启动链路诊断测试。</br>\n");
	fprintf(fs, "		注意: 只有在线的设备才能做出正确的诊断。</br></br>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "</blockquote>\n");
	fprintf(fs, "</body>\n");
	fprintf(fs, "</html>\n");
	
	fflush(fs);
}

void cgiLinkDiagResult(char *query, FILE *fs)
{
	int cltid = 0;
   	int cnuid = 0;

	cltid = (glbWebVar.cnuid-1)/MAX_CNUS_PER_CLT+1;
	cnuid = (glbWebVar.cnuid-1)%MAX_CNUS_PER_CLT+1;
	
	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");
	fprintf(fs, "function btnReturn()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	location.href = 'previewLinkDiag.cgi';\n");
	fprintf(fs, "}\n");
	fprintf(fs, "$(function()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	$('#accordion').accordion({\n");
	fprintf(fs, "		collapsible: true,\n");
	fprintf(fs, "		heightStyle: 'content'\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#retBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-carat-1-w'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$( '#retBtn' ).button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnReturn();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "});\n");
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "	<br>\n");
	fprintf(fs, "	<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "		<tr><td class='maintitle'><b>链路诊断</b></td></tr>\n");
	fprintf(fs, "	</table>\n");
	fprintf(fs, "	<table border=0 cellpadding=0 cellspacing=0 width='100%'> \n");
	fprintf(fs, "		<tr><td class='mainline' width='100%'></td></tr> \n");
	fprintf(fs, "	</table>\n");
	fprintf(fs, "	<br>\n");
	fprintf(fs, "	<br>\n");
	fprintf(fs, "	<div id='accordion'>\n");
	fprintf(fs, "		<h3>EoC物理链路诊断测试信息</h3>\n");
	fprintf(fs, "		<div>\n");
	fprintf(fs, "			<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata' width=350>诊断测试结果</td>\n");
	if( CMM_SUCCESS == glbWebVar.diagResult )
	{
		fprintf(fs, "					<td class='diagdata' width=200><font color='green'>成功</font></td>\n");
	}
	else
	{
		fprintf(fs, "					<td class='diagdata' width=200><font color='red'>失败</font></td>\n");
	}
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>被诊断设备索引号</td>\n");
	fprintf(fs, "		<td class='diagdata'>CNU/%d/%d</td>\n", cltid, cnuid);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>MAC地址</td>\n");
	fprintf(fs, "		<td class='diagdata'>%s</td>\n", glbWebVar.diagCnuMac);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>TEI</td>\n");
	fprintf(fs, "		<td class='diagdata'>%d</td>\n", glbWebVar.diagCnuTei);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>设备型号</td>\n");
	fprintf(fs, "		<td class='diagdata'>%s</td>\n", boardapi_getDeviceModelStr(glbWebVar.diagCnuModel));
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>目标诊断方向</td>\n");
	if( 1 == glbWebVar.diagDir )
	{
		fprintf(fs, "		<td class='diagdata'>TX（上行）</td>\n");
	}
	else if( 2 == glbWebVar.diagDir )
	{
		fprintf(fs, "		<td class='diagdata'>RX（下行）</td>\n");
	}
	else
	{
		fprintf(fs, "		<td class='diagdata'>未定义</td>\n");
	}
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>CCo MAC 地址</td>\n");
	fprintf(fs, "		<td class='diagdata'>%s</td>\n", glbWebVar.ccoMac);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>CCo NID</td>\n");
	fprintf(fs, "		<td class='diagdata'>%s</td>\n", glbWebVar.ccoNid);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>CCo SNID</td>\n");
	fprintf(fs, "		<td class='diagdata'>%d</td>\n", glbWebVar.ccoSnid);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>CCo TEI</td>\n");
	fprintf(fs, "		<td class='diagdata'>%d</td>\n", glbWebVar.ccoTei);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>平均发送速率</td>\n");
	fprintf(fs, "		<td class='diagdata'>%d Mbps</td>\n", glbWebVar.diagCnuRxRate);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>平均接收速率</td>\n");
	fprintf(fs, "		<td class='diagdata'>%d Mbps</td>\n", glbWebVar.diagCnuTxRate);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>子载波平均比特速率</td>\n");
	fprintf(fs, "		<td class='diagdata'>%s</td>\n", glbWebVar.bitCarrier);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>链路衰减</td>\n");
	fprintf(fs, "		<td class='diagdata'>%d dB</td>\n", glbWebVar.diagCnuAtten);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>设备桥接主机MAC地址</td>\n");
	fprintf(fs, "		<td class='diagdata'>%s</td>\n", glbWebVar.bridgedMac);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>发送MPDUs成功数量</td>\n");
	fprintf(fs, "		<td class='diagdata'>%s</td>\n", glbWebVar.MPDU_ACKD);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>发送MPDUs冲突数量</td>\n");
	fprintf(fs, "		<td class='diagdata'>%s</td>\n", glbWebVar.MPDU_COLL);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>发送MPDUs失败数量</td>\n");
	fprintf(fs, "		<td class='diagdata'>%s</td>\n", glbWebVar.MPDU_FAIL);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>发送PBs成功数量</td>\n");
	fprintf(fs, "		<td class='diagdata'>%s</td>\n", glbWebVar.PBS_PASS);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "	<tr>\n");
	fprintf(fs, "		<td class='diagdata'>发送PBs失败数量</td>\n");
	fprintf(fs, "		<td class='diagdata'>%s</td>\n", glbWebVar.PBS_FAIL);
	fprintf(fs, "	</tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "	<p>\n");
	fprintf(fs, "		<button id=retBtn>返 回</button>\n");
	fprintf(fs, "	</p>\n");
	
	fprintf(fs, "</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiTopologyView(char *query, FILE *fs) 
{
	int i=0;
	int j = 0;
	int n = 0;
	int iCount = 0;
	st_dbsClt clt;
	st_dbsCnu cnu;
	char logmsg[256]={0};
	T_szCnuUserHFID cnuuserhfid;

	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");

	fprintf(fs, "function btnRefresh(){\n");
	fprintf(fs, "	location.href = 'previewTopology.cgi';\n");
	fprintf(fs, "}\n");
	fprintf(fs, "function btnNewCnu(){\n");
	fprintf(fs, "	window.location='wecNewCnu.html';\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "$(function(){\n");
	fprintf(fs, "	$('#refreshBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-refresh'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#refreshBtn').click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnRefresh();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#newCnuBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-person'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	if(strcmp(glbWebVar.curUserName,"user")  == 0)
	{
		fprintf(fs, "	$('#newCnuBtn').button().click(function(event){\n");
		fprintf(fs, "       		alert('您没有足够的权限...');\n");
		fprintf(fs, "	});\n");
	}
	else
	{
		fprintf(fs, "	$('#newCnuBtn').click(function(event){\n");
		fprintf(fs, "		event.preventDefault();\n");
		fprintf(fs, "		btnNewCnu();\n");
		fprintf(fs, "	});\n");
	}
	fprintf(fs, "});\n");

	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "	<tr><td class='maintitle'><b>网络拓扑</b></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0 width='100%'>\n");
	fprintf(fs, "	<tr><td class='mainline' width='100%'></td></tr>\n");
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<br><br>\n");	
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='diagret' align='center' width=80>索引</td>\n");
	fprintf(fs, "<td class='diagret' align='center' width=150>MAC地址</td>\n");
	fprintf(fs, "<td class='diagret' align='center' width=120>设备型号</td>\n");
	fprintf(fs, "<td class='diagret' align='center' width=80>授权</td>\n");
	fprintf(fs, "<td class='diagret' align='center' width=90>RX/TX</td>\n");
	fprintf(fs, "<td class='diagret' align='center' width=60>状态</td>\n");
	fprintf(fs, "<td class='diagret' align='center' width=60>查看</td>\n</tr>\n");
	fprintf(fs, "<tr>\n<td colspan=7>&nbsp;</td>\n</tr>\n");


	for( i=1; i<=MAX_CLT_AMOUNT_LIMIT; i++ )
	{	
		if( CMM_SUCCESS == dbsGetClt(dbsdev, i, &clt) )
		{
			/* no clt present in this port */
			if( 0 == clt.col_row_sts )
			{
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='clt' align='center'>CLT/%d</td>\n", i);
				fprintf(fs, "<td class='clt' align='center'><img src='help.gif'></td>\n");
				fprintf(fs, "<td class='clt' align='center'>CLT</td>\n");
				fprintf(fs, "<td class='clt' align='center'>--</td>\n");
				fprintf(fs, "<td class='clt' align='center'>--</td>\n");
				fprintf(fs, "<td class='clt' align='center'><IMG src='suppress.png'></td>\n");
				fprintf(fs, "<td class='clt' align='center'><img src='show.gif'></td>\n");
				fprintf(fs, "<tr>\n");
			}
			else
			{
				iCount = 0;
				
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='clt' align='center'>CLT/%d</td>\n", i);
				fprintf(fs, "<td class='clt' align='center'>[%s]</td>\n", clt.col_mac);
				fprintf(fs, "<td class='clt' align='center'>CLT</td>\n");
				fprintf(fs, "<td class='clt' align='center'>--</td>\n");
				fprintf(fs, "<td class='clt' align='center'>--</td>\n");
				fprintf(fs, "<td class='clt' align='center'><IMG src='%s'></td>\n", clt.col_sts?"true.png":"net_down.gif");
				fprintf(fs, "<td class='clt' align='center'>\n");
				fprintf(fs, "<img src='show.gif' style='cursor:pointer' onclick='window.location=\"cltManagement.cmd?cltid=%d\"'>\n", i);
				fprintf(fs, "</td>\n");
				fprintf(fs, "<tr>\n");
				for( j=1; j<=MAX_CNUS_PER_CLT; j++ )
				{
					n = (i-1)*MAX_CNUS_PER_CLT+j;
					if( dbsGetCnu(dbsdev, n, &cnu) != CMM_SUCCESS )
					{
						break;
					}
					else
					{
						if( cnu.col_row_sts == 0 )
						{
							/**/
							continue;
						}
						else
						{	
							if ( boardapi_isAr6400Device(cnu.col_model) ==1)
							{
								iCount++;
								if( (iCount % 2) == 0 )
								{
									/**/
									fprintf(fs, "<tr>\n");
									fprintf(fs, "<td class='cnub' align='center'>CNU/%d/%d</td>\n", i, j);
									fprintf(fs, "<td class='cnub' align='center'>%s</td>\n", cnu.col_mac);
									fprintf(fs, "<td class='cnub' align='center'>%s</td>\n", boardapi_getDeviceModelStr(cnu.col_model));
									fprintf(fs, "<td class='cnub' align='center'>%s</td>\n", cnu.col_auth?"允许":"禁止");
									fprintf(fs, "<td class='cnub' align='center'>%d/%d</td>\n", cnu.col_rx, cnu.col_tx);
									fprintf(fs, "<td class='cnub' align='center'><IMG src='%s'></td>\n", cnu.col_sts?"net_up.gif":"net_down.gif");
									//fprintf(fs, "<td class='cnub' align='center'><img src='show.gif' style='cursor:pointer' onclick='window.location=\"cnuProfile.cmd?viewid=%d\"'></td>\n", n);
									fprintf(fs, "<td class='cnub' align='center'><img src='show.gif' style='cursor:pointer' onclick='window.location=\"cnuManagement.cmd?cnuid=%d\"'></td>\n", n);
									fprintf(fs, "</tr>\n");
								}
								else
								{
									/**/
									fprintf(fs, "<tr>\n");
									fprintf(fs, "<td class='cnua' align='center'>CNU/%d/%d</td>\n", i, j);
									fprintf(fs, "<td class='cnua' align='center'>%s</td>\n", cnu.col_mac);
									fprintf(fs, "<td class='cnua' align='center'>%s</td>\n", boardapi_getDeviceModelStr(cnu.col_model));
									fprintf(fs, "<td class='cnua' align='center'>%s</td>\n", cnu.col_auth?"允许":"禁止");
									fprintf(fs, "<td class='cnua' align='center'>%d/%d</td>\n", cnu.col_rx, cnu.col_tx);
									fprintf(fs, "<td class='cnua' align='center'><IMG src='%s'></td>\n", cnu.col_sts?"net_up.gif":"net_down.gif");
									//fprintf(fs, "<td class='cnua' align='center'><img src='show.gif' style='cursor:pointer' onclick='window.location=\"cnuProfile.cmd?viewid=%d\"'></td>\n", n);
									fprintf(fs, "<td class='cnua' align='center'><img src='show.gif' style='cursor:pointer' onclick='window.location=\"cnuManagement.cmd?cnuid=%d\"'></td>\n", n);
									fprintf(fs, "</tr>\n");
								}
							}
							else
							{
								if ( cnu.col_sts == 1)
								{
									 boardapi_macs2b(cnu.col_mac, cnuuserhfid.ODA);
									 http2cmm_getCnuUserHFID(&cnuuserhfid);
									 memcpy(cnu.col_user_hfid,cnuuserhfid.pdata,64);
									 dbsUpdateCnu( dbsdev, n, &cnu);
									 dbsFflush(dbsdev);
								}
								iCount++;
								if( (iCount % 2) == 0 )
								{
									/**/
									fprintf(fs, "<tr>\n");
									fprintf(fs, "<td class='cnub' align='center'>CNU/%d/%d</td>\n", i, j);
									fprintf(fs, "<td class='cnub' align='center'>%s</td>\n", cnu.col_mac);
									fprintf(fs, "<td class='cnub' align='center'>%s</td>\n", boardapi_getDeviceModelStr(cnu.col_model) == "UNKNOWN" ? cnu.col_user_hfid : boardapi_getDeviceModelStr(cnu.col_model));
									fprintf(fs, "<td class='cnub' align='center'>%s</td>\n", cnu.col_auth?"允许":"禁止");
									fprintf(fs, "<td class='cnub' align='center'>%d/%d</td>\n", cnu.col_rx, cnu.col_tx);
									fprintf(fs, "<td class='cnub' align='center'><IMG src='%s'></td>\n", cnu.col_sts?"net_up.gif":"net_down.gif");
									//fprintf(fs, "<td class='cnub' align='center'><img src='show.gif' style='cursor:pointer' onclick='window.location=\"cnuProfile.cmd?viewid=%d\"'></td>\n", n);
									fprintf(fs, "<td class='cnub' align='center'><img src='show.gif' style='cursor:pointer' onclick='window.location=\"cnuManagement.cmd?cnuid=%d\"'></td>\n", n);
									fprintf(fs, "</tr>\n");
								}
								else
								{
									/**/
									fprintf(fs, "<tr>\n");
									fprintf(fs, "<td class='cnua' align='center'>CNU/%d/%d</td>\n", i, j);
									fprintf(fs, "<td class='cnua' align='center'>%s</td>\n", cnu.col_mac);
									fprintf(fs, "<td class='cnua' align='center'>%s</td>\n", boardapi_getDeviceModelStr(cnu.col_model) == "UNKNOWN" ? cnu.col_user_hfid : boardapi_getDeviceModelStr(cnu.col_model));
									fprintf(fs, "<td class='cnua' align='center'>%s</td>\n", cnu.col_auth?"允许":"禁止");
									fprintf(fs, "<td class='cnua' align='center'>%d/%d</td>\n", cnu.col_rx, cnu.col_tx);
									fprintf(fs, "<td class='cnua' align='center'><IMG src='%s'></td>\n", cnu.col_sts?"net_up.gif":"net_down.gif");
									//fprintf(fs, "<td class='cnua' align='center'><img src='show.gif' style='cursor:pointer' onclick='window.location=\"cnuProfile.cmd?viewid=%d\"'></td>\n", n);
									fprintf(fs, "<td class='cnua' align='center'><img src='show.gif' style='cursor:pointer' onclick='window.location=\"cnuManagement.cmd?cnuid=%d\"'></td>\n", n);
									fprintf(fs, "</tr>\n");
								}
							}
						}
					}
				}
			}			
		}
		else
		{
			/**/
			fprintf(fs, "<tr>\n<td class='clt' align='center' width=700>** 系统错误，请重试 **</td>\n<tr>\n");
			strcpy(logmsg, "show topology");
			http2dbs_writeOptlog(-1, logmsg);
		}
	}	
	fprintf(fs, "<tr>\n<td colspan=7>&nbsp;</td>\n</tr>\n");
	fprintf(fs, "<tr>\n<td class='listend' colspan=7></td>\n</tr>\n");
	fprintf(fs, "</table>\n");	
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<button id='refreshBtn'>刷 新</button>\n");
	fprintf(fs, "	<button id='newCnuBtn'>创建CNU</button>\n");
	fprintf(fs, "</p>\n");
	fprintf(fs, "</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}


void cgiOptlogViewByMid(char *query, FILE *fs, int mid)
{   
	int i=0;
	int logCount = 0;
	st_dbsOptlog st_log;
	struct tm *tim;
	char timenow[32] = {0};

	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");

	fprintf(fs, "function btnRefresh(){\n");
	fprintf(fs, "	window.location.reload();\n");
	fprintf(fs, "}\n");

	fprintf(fs, "function btnReturn()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	location.href = 'wecOptlog.html';\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "$(function(){\n");
	fprintf(fs, "	$('#refreshBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-refresh'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#refreshBtn').click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnRefresh();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#retBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-carat-1-w'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$( '#retBtn' ).button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnReturn();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "});\n");
	
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "	<tr><td class='maintitle'><b>操作日志</b></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0 width='100%'>\n");
	fprintf(fs, "	<tr><td class='mainline' width='100%'></td></tr>\n");
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<br>以下列表显示本机的操作日志 [筛选条件 = “%s”]:</br>\n<br><br>\n", boardapi_getModNameStr(mid));
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='diagret' width=160>时间</td>\n");
	fprintf(fs, "<td class='diagret' width=100>操作者</td>\n");
	fprintf(fs, "<td class='diagret' width=100>操作结果</td>\n");
	fprintf(fs, "<td class='diagret' width=400>日志信息</td>\n</tr>\n</table>\n<br>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=2>\n");
	//fprintf(fs, "\n");
   
	// write body
	for( i=1; i<=512; i++ )
	{
		if( http2dbs_getOptlog(i, &st_log) == CMM_SUCCESS )
		{
			if( mid != st_log.who ) continue;
			logCount++;
			tim = localtime((const time_t *)&st_log.time);
			sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
			tim->tm_mday, tim->tm_hour, tim->tm_min
			);
			if( st_log.result == 0 )
			{
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='diagdata' width=158>%s</td>\n", timenow);
				fprintf(fs, "<td class='diagdata' width=98>%s</td>\n", boardapi_getModNameStr(st_log.who));
				fprintf(fs, "<td class='diagdata' width=98>成功</td>\n");
				fprintf(fs, "<td class='diagdata' width=398>%s</td>\n</tr>\n", st_log.cmd);
			}
			else
			{
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='logfailed' width=158>%s</td>\n", timenow);
				fprintf(fs, "<td class='logfailed' width=98>%s</td>\n", boardapi_getModNameStr(st_log.who));
				fprintf(fs, "<td class='logfailed' width=98>失败</td>\n");
				fprintf(fs, "<td class='logfailed' width=398>%s</td>\n</tr>\n", st_log.cmd);
			}
		}
		else
		{
			break;
		}
	}
	if(logCount == 0 )
	{
		fprintf(fs, "<tr>\n<td class='diagdata' colspan=4>没有检索到操作日志信息</td>\n</tr>\n");
	}
	
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<br>\n<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n<td class='listend' width=760></td>\n</tr>\n</table>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<button id='retBtn'>返 回</button>\n");
	fprintf(fs, "	<button id='refreshBtn'>刷 新</button>\n");	
	fprintf(fs, "</p>\n");
	fprintf(fs, "</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiOptlogViewByStatus(char *query, FILE *fs, int status)
{   
	int i=0;
	int logCount = 0;
	st_dbsOptlog st_log;
	struct tm *tim;
	char timenow[32] = {0};

	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");

	fprintf(fs, "function btnRefresh(){\n");
	fprintf(fs, "	window.location.reload();\n");
	fprintf(fs, "}\n");

	fprintf(fs, "function btnReturn()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	location.href = 'wecOptlog.html';\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "$(function(){\n");
	fprintf(fs, "	$('#refreshBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-refresh'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#refreshBtn').click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnRefresh();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#retBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-carat-1-w'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$( '#retBtn' ).button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnReturn();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "});\n");
	
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "	<tr><td class='maintitle'><b>操作日志</b></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0 width='100%'>\n");
	fprintf(fs, "	<tr><td class='mainline' width='100%'></td></tr>\n");
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<br>以下列表显示本机的操作日志 [筛选条件 = “%s”]:</br>\n<br><br>\n", status?"Failed":"Success");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='diagret' width=160>时间</td>\n");
	fprintf(fs, "<td class='diagret' width=100>操作者</td>\n");
	fprintf(fs, "<td class='diagret' width=100>操作结果</td>\n");
	fprintf(fs, "<td class='diagret' width=400>日志信息</td>\n</tr>\n</table>\n<br>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=2>\n");
	//fprintf(fs, "\n");
   
	// write body
	for( i=1; i<=512; i++ )
	{
		if( http2dbs_getOptlog(i, &st_log) == CMM_SUCCESS )
		{
			if( status != st_log.result ) continue;
			logCount++;
			tim = localtime((const time_t *)&st_log.time);
			sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
			tim->tm_mday, tim->tm_hour, tim->tm_min
			);
			if( st_log.result == 0 )
			{
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='diagdata' width=158>%s</td>\n", timenow);
				fprintf(fs, "<td class='diagdata' width=98>%s</td>\n", boardapi_getModNameStr(st_log.who));
				fprintf(fs, "<td class='diagdata' width=98>成功</td>\n");
				fprintf(fs, "<td class='diagdata' width=398>%s</td>\n</tr>\n", st_log.cmd);
			}
			else
			{
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='logfailed' width=158>%s</td>\n", timenow);
				fprintf(fs, "<td class='logfailed' width=98>%s</td>\n", boardapi_getModNameStr(st_log.who));
				fprintf(fs, "<td class='logfailed' width=98>失败</td>\n");
				fprintf(fs, "<td class='logfailed' width=398>%s</td>\n</tr>\n", st_log.cmd);
			}
		}
		else
		{
			break;
		}
	}
	if(logCount == 0 )
	{
		fprintf(fs, "<tr>\n<td class='diagdata' colspan=4>没有检索到操作日志信息</td>\n</tr>\n");
	}
	
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<br>\n<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n<td class='listend' width=760></td>\n</tr>\n</table>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<button id='retBtn'>返 回</button>\n");
	fprintf(fs, "	<button id='refreshBtn'>刷 新</button>\n");	
	fprintf(fs, "</p>\n");
	fprintf(fs, "</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiOptlogViewAll(char *query, FILE *fs)
{   
	int i=0;
	int logCount = 0;
	st_dbsOptlog st_log;
	struct tm *tim;
	char timenow[32] = {0};

	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");

	fprintf(fs, "function btnRefresh(){\n");
	fprintf(fs, "	window.location.reload();\n");
	fprintf(fs, "}\n");

	fprintf(fs, "function btnReturn()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	location.href = 'wecOptlog.html';\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "$(function(){\n");
	fprintf(fs, "	$('#refreshBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-refresh'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#refreshBtn').click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnRefresh();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#retBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-carat-1-w'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$( '#retBtn' ).button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnReturn();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "});\n");
	
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "	<tr><td class='maintitle'><b>操作日志</b></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0 width='100%'>\n");
	fprintf(fs, "	<tr><td class='mainline' width='100%'></td></tr>\n");
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<br>以下列表显示本机的操作日志 [筛选条件 = “All”]:</br>\n<br><br>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	fprintf(fs, "<td class='diagret' width=160>时间</td>\n");
	fprintf(fs, "<td class='diagret' width=100>操作者</td>\n");
	fprintf(fs, "<td class='diagret' width=100>操作结果</td>\n");
	fprintf(fs, "<td class='diagret' width=400>日志信息</td>\n</tr>\n</table>\n<br>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=2>\n");
   
	// write body
	for( i=1; i<=512; i++ )
	{
		if( http2dbs_getOptlog(i, &st_log) == CMM_SUCCESS )
		{
			logCount++;
			tim = localtime((const time_t *)&st_log.time);
			sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
			tim->tm_mday, tim->tm_hour, tim->tm_min
			);
			if( st_log.result == 0 )
			{
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='diagdata' width=158>%s</td>\n", timenow);
				fprintf(fs, "<td class='diagdata' width=98>%s</td>\n", boardapi_getModNameStr(st_log.who));
				fprintf(fs, "<td class='diagdata' width=98>SUCCESS</td>\n");
				fprintf(fs, "<td class='diagdata' width=398>%s</td>\n</tr>\n", st_log.cmd);
			}
			else
			{
				fprintf(fs, "<tr>\n");
				fprintf(fs, "<td class='logfailed' width=158>%s</td>\n", timenow);
				fprintf(fs, "<td class='logfailed' width=98>%s</td>\n", boardapi_getModNameStr(st_log.who));
				fprintf(fs, "<td class='logfailed' width=98>FAILED</td>\n");
				fprintf(fs, "<td class='logfailed' width=398>%s</td>\n</tr>\n", st_log.cmd);
			}
		}
		else
		{
			break;
		}
	}
	if(logCount == 0 )
	{
		fprintf(fs, "<tr>\n<td class='diagdata' colspan=4>没有检索到操作日志信息</td>\n</tr>\n");
	}
	
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<br>\n<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n<td class='listend' width=760></td>\n</tr>\n</table>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<button id='retBtn'>返 回</button>\n");
	fprintf(fs, "	<button id='refreshBtn'>刷 新</button>\n");	
	fprintf(fs, "</p>\n");
	fprintf(fs, "</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiSyslogViewByLevel(char *query, FILE *fs, int level)
{
	int i=0;
	int logCount = 0;	
	st_dbsSyslog st_log;
	struct tm *tim;
	char timenow[32] = {0};

	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");

	fprintf(fs, "function btnRefresh(){\n");
	fprintf(fs, "	window.location.reload();\n");
	fprintf(fs, "}\n");

	fprintf(fs, "function btnReturn()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	location.href = 'wecSyslog.html';\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "$(function(){\n");
	fprintf(fs, "	$('#refreshBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-refresh'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#refreshBtn').click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnRefresh();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#retBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-carat-1-w'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$( '#retBtn' ).button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnReturn();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "});\n");
	
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "	<tr><td class='maintitle'><b>系统日志</b></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0 width='100%'>\n");
	fprintf(fs, "	<tr><td class='mainline' width='100%'></td></tr>\n");
	fprintf(fs, "</table>\n");
	
	switch(level)
	{
		case DBS_LOG_EMERG:
		{
			fprintf(fs, "<br>以下列表显示本机的系统日志 [筛选条件 = “Emergency”]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_ALERT:
		{
			fprintf(fs, "<br>以下列表显示本机的系统日志 [筛选条件 = “Alert”]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_CRIT:
		{
			fprintf(fs, "<br>以下列表显示本机的系统日志 [筛选条件 = “Cratical”]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_ERR:
		{
			fprintf(fs, "<br>以下列表显示本机的系统日志 [筛选条件 = “Error”]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_WARNING:
		{
			fprintf(fs, "<br>以下列表显示本机的系统日志 [筛选条件 = “Warnning”]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_NOTICE:
		{
			fprintf(fs, "<br>以下列表显示本机的系统日志 [筛选条件 = “Notice”]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_INFO:
		{
			fprintf(fs, "<br>以下列表显示本机的系统日志 [筛选条件 = “Informational”]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_DEBUG:
		{
			fprintf(fs, "<br>以下列表显示本机的系统日志 [筛选条件 = “Debugging”]:</br>\n<br><br>\n");
			break;
		}
		default:
		{
			fprintf(fs, "<br>以下列表显示本机的系统日志 [筛选条件 = “All”]:</br>\n<br><br>\n");
			break;
		}
	}	
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	
	fprintf(fs, "<td class='diagret' width=150>时间</td>\n");
	fprintf(fs, "<td class='diagret' width=110>模块</td>\n");
	fprintf(fs, "<td class='diagret' width=110>等级</td>\n");
	fprintf(fs, "<td class='diagret' width=400>日志信息</td>\n</tr>\n</table>\n<br>\n");

	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=2>\n");
	
	/*loop insert log here*/
	for( i=1; i<=1024; i++ )
	{
		if( http2dbs_getSyslog(i, &st_log) == CMM_SUCCESS )
		{
			if( st_log.level > level ) continue;
			logCount++;
			tim = localtime((const time_t *)&st_log.time);
			sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
				tim->tm_mday, tim->tm_hour, tim->tm_min
			);
			switch(st_log.level)
			{
				case DBS_LOG_EMERG:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logemg' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logemg' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='logemg' width=108>Emergency</td>\n");
					fprintf(fs, "<td class='logemg' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
				case DBS_LOG_ALERT:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logalert' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logalert' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='logalert' width=108>Alert</td>\n");
					fprintf(fs, "<td class='logalert' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
				case DBS_LOG_CRIT:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logcrt' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logcrt' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='logcrt' width=108>Cratical</td>\n");
					fprintf(fs, "<td class='logcrt' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
				case DBS_LOG_ERR:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logerr' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logerr' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='logerr' width=108>Error</td>\n");
					fprintf(fs, "<td class='logerr' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
				case DBS_LOG_WARNING:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logwarn' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logwarn' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='logwarn' width=108>Warnning</td>\n");
					fprintf(fs, "<td class='logwarn' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
				case DBS_LOG_NOTICE:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='lognotice' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='lognotice' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='lognotice' width=108>Notice</td>\n");
					fprintf(fs, "<td class='lognotice' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
				case DBS_LOG_INFO:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='loginfo' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='loginfo' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='loginfo' width=108>Informational</td>\n");
					fprintf(fs, "<td class='loginfo' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
				default:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logdbg' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logdbg' width=108>%s</td>\n", boardapi_getModNameStr(st_log.who));
					fprintf(fs, "<td class='logdbg' width=108>Debugging</td>\n");
					fprintf(fs, "<td class='logdbg' width=398>%s</td>\n</tr>\n", st_log.log);
					break;
				}
			}
		}
		else
		{
			break;
		}
	}
	if(logCount == 0 )
	{
		fprintf(fs, "<tr>\n<td class='diagdata' colspan=4>没有检索到系统日志信息</td>\n</tr>\n");
	}
	
	fprintf(fs, "</table>\n");
	
	fprintf(fs, "<br>\n<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n<td class='listend' width=770></td>\n</tr>\n</table>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<button id='retBtn'>返 回</button>\n");
	fprintf(fs, "	<button id='refreshBtn'>刷 新</button>\n");	
	fprintf(fs, "</p>\n");	
	fprintf(fs, "</blockquote>\n</body>\n</html>\n");
	
	fflush(fs);
}

void writePopErrorPage(FILE *fs)
{
	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");

	fprintf(fs, "function btnClose(){\n");
	fprintf(fs, "	window.close();\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "$(function(){\n");
	fprintf(fs, "	$('#accordion').accordion({\n");
	fprintf(fs, "		collapsible: true,\n");
	fprintf(fs, "		heightStyle: 'content'\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#closeBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-circle-close'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$( '#closeBtn' ).button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnClose();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "});\n");

	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");	
	fprintf(fs, "<br>\n");	
	fprintf(fs, "<div id='accordion'>\n");
	fprintf(fs, "	<h3>系统消息</h3>\n");
	fprintf(fs, "	<div>\n");
	fprintf(fs, "		<br>\n");
	fprintf(fs, "		<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "				<td class='diagdata' width=250>操作状态</td>\n");
	fprintf(fs, "				<td class='diagdata' width=150><b><font color=red>失败</font></b></td>\n");
	fprintf(fs, "			<tr>\n");
	fprintf(fs, "		</table>\n");
	fprintf(fs, "		<br>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "</div>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<button id=closeBtn>关 闭</button>\n");
	fprintf(fs, "</p>\n");
	fprintf(fs, "</body>\n</html>\n");
	fflush(fs);	
}

void cgiAlarmlogDetailView(char *query, FILE *fs) 
{
	char action[IFC_LARGE_LEN];
	int id = 0;
	int alarmLevel = 0;
	st_dbsAlarmlog st_log;
	struct tm *tim;
	char timenow[32] = {0};

	cgiGetValueByName(query, "viewid", action);
	id = atoi(action);

	if( http2dbs_getAlarmlog(id, &st_log) != CMM_SUCCESS )
	{
		writePopErrorPage(fs);
		return;
	}

	tim = localtime((const time_t *)&st_log.realTime);
	sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
		tim->tm_mday, tim->tm_hour, tim->tm_min
	);
	alarmLevel = boardapi_getAlarmLevel(&st_log);

	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");
	
	fprintf(fs, "function btnClose(){\n");
	fprintf(fs, "	window.close();\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "$(function(){\n");
	fprintf(fs, "	$('#accordion').accordion({\n");
	fprintf(fs, "		collapsible: true,\n");
	fprintf(fs, "		heightStyle: 'content'\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#closeBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-circle-close'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$( '#closeBtn' ).button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnClose();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "});\n");
	
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	//fprintf(fs, "<blockquote>\n");
	fprintf(fs, "	<br>\n");
	fprintf(fs, "	<div id='accordion'>\n");
	fprintf(fs, "		<h3>告警详细信息</h3>\n");
	fprintf(fs, "		<div>\n");
	fprintf(fs, "			<table border=0 cellpadding=0 cellspacing=0>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata' width=150>告警流水号</td>\n");
	fprintf(fs, "					<td class='diagdata' width=300>%d</td>\n", st_log.serialFlow);
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata'>告警时间</td>\n");
	fprintf(fs, "					<td class='diagdata'>%s</td>\n", timenow);
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata'>告警码</td>\n");
	fprintf(fs, "					<td class='diagdata'>%d</td>\n", st_log.alarmCode);
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata'>告警参数OID</td>\n");
	fprintf(fs, "					<td class='diagdata'>%s</td>\n", st_log.oid);
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata'>告警等级</td>\n");
	fprintf(fs, "					<td class='diagdata'>%s</td>\n", boardapi_getAlarmLevelStr(alarmLevel));
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata'>告警主机MAC地址</td>\n");
	fprintf(fs, "					<td class='diagdata'>%s</td>\n", st_log.cbatMac);
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata'>告警节点</td>\n");
	fprintf(fs, "					<td class='diagdata'>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata'>告警类型</td>\n");
	fprintf(fs, "					<td class='diagdata'>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata'>告警参数值</td>\n");
	fprintf(fs, "					<td class='diagdata'>%d</td>\n", st_log.alarmValue);	
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "				<tr>\n");
	fprintf(fs, "					<td class='diagdata'>告警信息</td>\n");
	fprintf(fs, "					<td class='diagdata'>%s</td>\n", st_log.trap_info);
	fprintf(fs, "				</tr>\n");
	fprintf(fs, "			</table>\n");
	fprintf(fs, "		</div>\n");
	fprintf(fs, "	</div>\n");
	fprintf(fs, "	<p>\n");
	fprintf(fs, "		<button id='closeBtn'>关 闭</button>\n");
	fprintf(fs, "	</p>\n");	
	//fprintf(fs, "</blockquote>\n");
	fprintf(fs, "</body>\n</html>\n");
	fflush(fs);	
}

void cgiAlarmlogViewByLevel(char *query, FILE *fs, int level)
{
	int i=0;
	int logCount = 0;
	int alarmLevel = 0;
	st_dbsAlarmlog st_log;
	struct tm *tim;
	char timenow[32] = {0};

	fprintf(fs, "<html>\n");
	fprintf(fs, "<head>\n");
	fprintf(fs, "<title>EoC</title>\n");
	fprintf(fs, "<base target='_self'>\n");
	fprintf(fs, "<meta http-equiv='Content-Type' content='text/html;charset=utf-8;no-cache'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/jquery-ui-1.10.3.custom.min.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='css/redmond/demos.css'/>\n");
	fprintf(fs, "<link rel='stylesheet' href='stylemain.css' type='text/css'>\n");
	fprintf(fs, "<link rel='stylesheet' href='colors.css' type='text/css'>\n");
	fprintf(fs, "<script src='js/jquery-1.9.1.js'></script>\n");
	fprintf(fs, "<script src='js/jquery-ui-1.10.3.custom.min.js'></script>\n");
	fprintf(fs, "<script language='javascript'>\n");

	fprintf(fs, "function btnRefresh(){\n");
	fprintf(fs, "	window.location.reload();\n");
	fprintf(fs, "}\n");

	fprintf(fs, "function btnReturn()\n");
	fprintf(fs, "{\n");
	fprintf(fs, "	location.href = 'wecAlarmlog.html';\n");
	fprintf(fs, "}\n");
	
	fprintf(fs, "$(function(){\n");
	fprintf(fs, "	$('#refreshBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-refresh'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#refreshBtn').click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnRefresh();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$('#retBtn').button({\n");
	fprintf(fs, "		icons: {\n");
	fprintf(fs, "			primary: 'ui-icon-carat-1-w'\n");
	fprintf(fs, "		},\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "	$( '#retBtn' ).button().click(function(event){\n");
	fprintf(fs, "		event.preventDefault();\n");
	fprintf(fs, "		btnReturn();\n");
	fprintf(fs, "	});\n");
	fprintf(fs, "});\n");
	
	fprintf(fs, "</script>\n");
	fprintf(fs, "</head>\n");
	fprintf(fs, "<body>\n");
	fprintf(fs, "<blockquote>\n");
	fprintf(fs, "<br>\n");
	fprintf(fs, "<table border=0 cellpadding=5 cellspacing=0>\n");
	fprintf(fs, "	<tr><td class='maintitle'><b>告警日志</b></td></tr>\n");
	fprintf(fs, "</table>\n");
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0 width='100%'>\n");
	fprintf(fs, "	<tr><td class='mainline' width='100%'></td></tr>\n");
	fprintf(fs, "</table>\n");
	
	switch(level)
	{
		case DBS_LOG_EMERG:
		{
			fprintf(fs, "<br>以下列表显示本机的告警日志 [筛选条件 = “Emergency”]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_ALERT:
		{
			fprintf(fs, "<br>以下列表显示本机的告警日志 [筛选条件 = “Alert”]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_CRIT:
		{
			fprintf(fs, "<br>以下列表显示本机的告警日志 [筛选条件 = “Cratical”]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_ERR:
		{
			fprintf(fs, "<br>以下列表显示本机的告警日志 [筛选条件 = “Error”]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_WARNING:
		{
			fprintf(fs, "<br>以下列表显示本机的告警日志 [筛选条件 = “Warnning”]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_NOTICE:
		{
			fprintf(fs, "<br>以下列表显示本机的告警日志 [筛选条件 = “Notice”]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_INFO:
		{
			fprintf(fs, "<br>以下列表显示本机的告警日志 [筛选条件 = “Informational”]:</br>\n<br><br>\n");
			break;
		}
		case DBS_LOG_DEBUG:
		{
			fprintf(fs, "<br>以下列表显示本机的告警日志 [筛选条件 = “Debugging”]:</br>\n<br><br>\n");
			break;
		}
		default:
		{
			fprintf(fs, "<br>以下列表显示本机的告警日志 [筛选条件 = “All”]:</br>\n<br><br>\n");
			break;
		}
	}	
	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n");
	
	fprintf(fs, "<td class='diagret' width=80>序号</td>\n");
	fprintf(fs, "<td class='diagret' width=60>告警码</td>\n");
	fprintf(fs, "<td class='diagret' width=150>告警时间</td>\n");
	fprintf(fs, "<td class='diagret' width=50>节点</td>\n");
	fprintf(fs, "<td class='diagret' width=100>告警类型</td>\n");
	fprintf(fs, "<td class='diagret' width=60>告警值</td>\n");
	fprintf(fs, "<td class='diagret' width=400>告警信息</td>\n</tr>\n</table>\n<br>\n");

	fprintf(fs, "<table border=0 cellpadding=0 cellspacing=2>\n");
	/*loop insert log here*/
	for( i=1; i<=512; i++ )
	{
		if( http2dbs_getAlarmlog(i, &st_log) == CMM_SUCCESS )
		{
			alarmLevel = boardapi_getAlarmLevel(&st_log);			
			if( alarmLevel > level ) continue;
			logCount++;
			tim = localtime((const time_t *)&st_log.realTime);
			sprintf(timenow,"%4d-%02d-%02d %02d:%02d",tim->tm_year+1900,tim->tm_mon+1,
				tim->tm_mday, tim->tm_hour, tim->tm_min
			);
			/*convert temperature value*/
			if( 200903 == st_log.alarmCode )
			{
				if( 0 == (st_log.alarmValue>>24) )
				{
					st_log.alarmValue = (st_log.alarmValue>>16)&0x000000ff;
				}
				else
				{
					st_log.alarmValue = (~(((st_log.alarmValue>>16)&0x000000ff)-1));
				}
			}
			switch(alarmLevel)
			{
				case DBS_LOG_EMERG:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logemg' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=350'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='logemg' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='logemg' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logemg' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='logemg' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='logemg' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='logemg' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
				case DBS_LOG_ALERT:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logalert' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=350'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='logalert' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='logalert' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logalert' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='logalert' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='logalert' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='logalert' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
				case DBS_LOG_CRIT:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logcrt' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=350'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='logcrt' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='logcrt' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logcrt' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='logcrt' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='logcrt' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='logcrt' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
				case DBS_LOG_ERR:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logerr' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=350'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='logerr' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='logerr' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logerr' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='logerr' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='logerr' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='logerr' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
				case DBS_LOG_WARNING:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logwarn' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=350'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='logwarn' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='logwarn' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logwarn' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='logwarn' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='logwarn' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='logwarn' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
				case DBS_LOG_NOTICE:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='lognotice' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=350'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='lognotice' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='lognotice' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='lognotice' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='lognotice' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='lognotice' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='lognotice' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
				case DBS_LOG_INFO:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='loginfo' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=350'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='loginfo' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='loginfo' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='loginfo' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='loginfo' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='loginfo' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='loginfo' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
				/*DBS_LOG_DEBUG*/
				default:
				{
					fprintf(fs, "<tr>\n");
					fprintf(fs, "<td class='logdbg' width=78>\n");
					fprintf(fs, "<a href='alarmlogDetail.cmd?viewid=%d' onclick= \"window.open(this.href, 'AlarmLog', 'menubar=yes,resizable=yes,scrollbars=yes,titlebar=yes,toolbar=no,width=520,height=350'); return false\">\n", st_log.serialFlow);
					fprintf(fs, "<font color=blue>%d</font></a></td>\n", st_log.serialFlow);
					fprintf(fs, "<td class='logdbg' width=58>%d</td>\n", st_log.alarmCode);
					fprintf(fs, "<td class='logdbg' width=148>%s</td>\n", timenow);
					fprintf(fs, "<td class='logdbg' width=48>%d.%d</td>\n", st_log.cltId, st_log.cnuId);
					fprintf(fs, "<td class='logdbg' width=98>%s</td>\n", boardapi_getAlarmTypeStr(st_log.alarmType));
					fprintf(fs, "<td class='logdbg' width=58>%d</td>\n", st_log.alarmValue);
					fprintf(fs, "<td class='logdbg' width=398>%s</td>\n</tr>\n", st_log.trap_info);
					break;
				}
			}
		}
		else
		{
			break;
		}
	}
	if(logCount == 0 )
	{
		fprintf(fs, "<tr>\n<td class='diagdata' colspan=7>没有检索到告警信息</td>\n</tr>\n");
	}
	fprintf(fs, "</table>\n");
	fprintf(fs, "<br>\n<table border=0 cellpadding=0 cellspacing=0>\n<tr>\n<td class='listend' width=900></td>\n</tr>\n</table>\n");
	fprintf(fs, "<p>\n");
	fprintf(fs, "	<button id='retBtn'>返 回</button>\n");
	fprintf(fs, "	<button id='refreshBtn'>刷 新</button>\n");	
	fprintf(fs, "</p>\n");
	fprintf(fs, "</blockquote>\n</body>\n</html>\n");
	fflush(fs);
}

void cgiOptlogView(char *query, FILE *fs) 
{
	char action[IFC_LARGE_LEN];
	int condition = 0;

	cgiGetValueByName(query, "action", action);
	condition = atoi(action);

	switch(condition)
	{
		case 0:
		{
			cgiOptlogViewByMid(query, fs, MID_HTTP);
			break;
		}
		case 1:
		{
			cgiOptlogViewByMid(query, fs, MID_CLI);
			break;
		}
		case 2:
		{
			cgiOptlogViewByMid(query, fs, MID_SNMP);
			break;
		}
		case 3:
		{
			cgiOptlogViewByStatus(query, fs, CMM_SUCCESS);
			break;
		}
		case 4:
		{
			cgiOptlogViewByStatus(query, fs, CMM_FAILED);
			break;
		}
		case 5:
		{
			cgiOptlogViewAll(query, fs);
			break;
		}
		default:
		{
			strcpy(glbWebVar.returnUrl, "wecOptlog.html");
			glbWebVar.wecOptCode = CMM_FAILED;
			do_ej("/webs/wecOptResult2.html", fs);
			break;
		}
	}
}

void cgiSyslogView(char *query, FILE *fs) 
{
	char action[IFC_LARGE_LEN];
	int condition = 0;

	cgiGetValueByName(query, "action", action);
	condition = atoi(action);

	switch(condition)
	{
		case DBS_LOG_EMERG:
		case DBS_LOG_ALERT:
		case DBS_LOG_CRIT:
		case DBS_LOG_ERR:
		case DBS_LOG_WARNING:
		case DBS_LOG_NOTICE:
		case DBS_LOG_INFO:
		case DBS_LOG_DEBUG:
		case DBS_LOG_DEBUG+1:
		{
			cgiSyslogViewByLevel(query, fs, condition);
			break;
		}
		default:
		{
			strcpy(glbWebVar.returnUrl, "wecSyslog.html");
			glbWebVar.wecOptCode = CMM_FAILED;
			do_ej("/webs/wecOptResult2.html", fs);
			break;
		}
	}
}

void cgiAlarmlogView(char *query, FILE *fs) 
{
	char action[IFC_LARGE_LEN];
	int condition = 0;

	cgiGetValueByName(query, "action", action);
	condition = atoi(action);

	switch(condition)
	{
		case DBS_LOG_EMERG:
		case DBS_LOG_ALERT:
		case DBS_LOG_CRIT:
		case DBS_LOG_ERR:
		case DBS_LOG_WARNING:
		case DBS_LOG_NOTICE:
		case DBS_LOG_INFO:
		case DBS_LOG_DEBUG:
		case DBS_LOG_DEBUG+1:
		{
			cgiAlarmlogViewByLevel(query, fs, condition);
			break;
		}
		default:
		{
			strcpy(glbWebVar.returnUrl, "wecAlarmlog.html");
			glbWebVar.wecOptCode = CMM_FAILED;
			do_ej("/webs/wecOptResult2.html", fs);
			break;
		}
	}
}





