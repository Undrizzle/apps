
/*****************************************************************************************
  文件名称 : manager.c
  文件描述 : 添加MIB 节点，实现各标准节点
  修订记录 :
           1 创建 : may2250
             日期 : 2010-07-28
             描述 : 创建文件

 *****************************************************************************************/

#ifndef _SNMP_MANAGER_C_
#define _SNMP_MANAGER_C_

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "manager.h"
#include "../../../../../include/public.h"

static onlinecnu_table_data onlinecnu_tbl;
static onlinecnu_table_data *ponlinecnu_tbl = &onlinecnu_tbl;
static eoc_list_t onlinecnu_list;

static cnuport_table_data cnuport_tbl;
static cnuport_table_data *pcnuport_tbl = &cnuport_tbl;
static eoc_list_t cnuport_list;

static cnu_table_data cnu_tbl;
static cnu_table_data *pcnu_tbl = &cnu_tbl;
static eoc_list_t cnu_list;

//static template_table_data template_tbl;
//static template_table_data *ptemplate_tbl = &template_tbl;
//static eoc_list_t cnu_template_list;

struct eoc_link_node *eoc_lstNode, *eoc_nwlNode, *eoc_hdlNode;

static T_szTemplate szTemplate;
static uint16_t template_sts = 0;
static char array_value[1024];

/*===============SOCKET FUNCTION =================== */
int sockfd;
struct sockaddr_in servaddr;
void init_snmp_sock(void)
{
	int len;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(CMM_LISTEN_PORT);
	/* CMM 模块IP 地址*/
	servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	len=sizeof(servaddr);
     	if( connect(sockfd, (struct sockaddr *)&servaddr, len) == -1 )
	{
		perror("connect error!");
		exit(1);
	}
}

int msg_communicate(int sk, uint8_t *buf, uint32_t len)
{
	int ret = 0;
	int sendn = 0;
	int rev_len = 0;
	int FromAddrSize = 0;
	int maxsock;
	fd_set fdsr;
	struct timeval tv;
	struct sockaddr_in from;
	T_CMM_MSG_HEADER_ACK *r = NULL;
	T_Msg_Header_CMM *req = (T_Msg_Header_CMM *)buf;
#if 0
	printf("\n=====================================================\n");
	printf("sk = %d, ", sk);
	printf("sin_addr = %d, ", servaddr.sin_addr);
	printf("sin_port = %d\n", servaddr.sin_port);
	printf("=====================================================\n");
	printf("usSrcMID = %d, ", req->usSrcMID);
	printf("usDstMID = %d, ", req->usDstMID);
	printf("usMsgType = 0x%X, ", req->usMsgType);
	printf("ulBodyLength = %d, ", req->ulBodyLength);
	printf("fragment = %d\n", req->fragment);
	printf("=====================================================\n");
#endif
	sendn = sendto(sk, buf, len, 0, (struct sockaddr *)&(servaddr), sizeof(struct sockaddr));
	if ( -1 == sendn )
	{
		perror("SNMP call sendto error, continue !\n");
		return CMM_FAILED;
	}
	
	// initialize file descriptor set
	FD_ZERO(&fdsr);
	FD_SET(sk, &fdsr);

	// timeout setting
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	
	maxsock = sk;

	ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
	if( ret <= 0 )
	{
		printf("snmp wait cmm select time out\n");
		return CMM_FAILED;
	}
	
	// check whether a new connection comes
	if (FD_ISSET(sk, &fdsr))
	{
		FromAddrSize = sizeof(from);
		rev_len = recvfrom(sk, buf, MAX_UDP_SIZE, 0, (struct sockaddr *)&from, &FromAddrSize);

		if ( -1 == rev_len )
		{
			perror("SNMP recvfrom error, continue !\n");
			return CMM_FAILED;
		}
		else
		{
			r = (T_CMM_MSG_HEADER_ACK *)buf;
			return r->Result;
			
		}			
	}
	else
	{
		printf("fd is not setted, continue !\n");
		return CMM_FAILED;
	}
}

void close_sock(void)
{
	close(sockfd);
}

int getSnmpConfig(T_szSnmpConf *snmpConfig)
{
	uint32_t len = 0;
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_CMM header;
	T_CMM_MSG_ACK *ack = (T_CMM_MSG_ACK *)buf;
	T_szSnmpConf *pValue = (T_szSnmpConf *)(ack->BUF);

	header.usSrcMID = MID_SNMP;
	header.usDstMID = MID_CMM;
	header.usMsgType = CMM_READ_SNMP_CONF;
	header.fragment = 0;
	header.ulBodyLength = sizeof(T_szSnmpConf);

	bzero(buf, MAX_UDP_SIZE);
	memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
	memcpy(buf+sizeof(T_Msg_Header_CMM), snmpConfig, sizeof(T_szSnmpConf));
	len = sizeof(T_Msg_Header_CMM)+sizeof(T_szSnmpConf);
	
	if(msg_communicate(sockfd, (uint8_t *) buf, len) != CMM_SUCCESS)
	{
		return SNMP_ERR_BADVALUE;
	}

	memcpy(snmpConfig, pValue, sizeof(T_szSnmpConf));
	//printf("\n##getSnmpConfig: trap server = %s\n", snmpConfig->f_trap2sink);
	
	return CMM_SUCCESS;
}


/*==========================================================*/

/*===================函数声明=============================*/

int WriteMethod_cnuresetall(int action,
                                             unsigned char *var_val,
                                             unsigned char var_val_type,
                                             size_t var_val_len, 
                                             unsigned char *statP, 
                                             oid *name, size_t length);

int WriteMethod_cnureset(int action,
                 u_char * var_val,
                 u_char var_val_type,
                 size_t var_val_len,
                 u_char * statP, oid * name, size_t name_len);

int WriteMethod_cnutable(int action,
                 u_char * var_val,
                 u_char var_val_type,
                 size_t var_val_len,
                 u_char * statP, oid * name, size_t name_len);

int WriteMethod_templatetable(int action,
                 u_char * var_val,
                 u_char var_val_type,
                 size_t var_val_len,
                 u_char * statP, oid * name, size_t name_len);

int WriteMethod_optentry(int action,
                 u_char * var_val,
                 u_char var_val_type,
                 size_t var_val_len,
                 u_char * statP, oid * name, size_t name_len);

int WriteMethod_cbatstatus(int action,
                 u_char * var_val,
                 u_char var_val_type,
                 size_t var_val_len,
                 u_char * statP, oid * name, size_t name_len);

unsigned char *prevail_info(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method);
        					                  
unsigned char *var_online_cnu(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method);

unsigned char *var_online_cnu_table(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method);

unsigned char *var_cnu_port(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method);

unsigned char *var_cnu_port_table(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method);

unsigned char *var_cnu_prevail(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method);

unsigned char *var_cnu_table(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method);

unsigned char *var_template_table(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method);

unsigned char *var_optentry(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method);

unsigned char *var_cbatstatus(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method);
/*==========================================================*/


/*======  杭州万隆公司节点(新增)==================*/
static struct variable1 prevail_info_variabies[]=
{
	{PREVAIL_INFO_NAME , ASN_OCTET_STR, RONLY, prevail_info, 1, {1}},
	{PREVAIL_INFO_CONTACT, ASN_OCTET_STR, RONLY, prevail_info, 1, {2}},
	{PREVAIL_INFO_EMAIL, ASN_OCTET_STR, RONLY, prevail_info, 1, {3}},
	{PREVAIL_INFO_DEVICETYPE, ASN_OCTET_STR, RONLY, prevail_info, 1, {4}}
};
oid prevail_info_variabies_oid[]={OID_PREVAIL};

/*======= ==========================================*/

static struct variable1 online_cnu_variables[] = 
{
	{OID_ONLINECNU_ALLRESET, ASN_INTEGER, RWRITE, var_online_cnu, 1, {1}},
	{OID_ONLINECNU_AMOUNT, ASN_INTEGER, RONLY, var_online_cnu, 1, {2}},
	{OID_ONLINECNU_LIMIT, ASN_INTEGER, RWRITE, var_online_cnu, 1, {3}}
};
oid online_cnu_variables_oid[] = {EOC_OID_CNU_ONLINECNU};

static struct variable1 online_cnu_table_variables[] = {	
	{OID_ONLINECNU_TABLE_MASTERINDEX, ASN_INTEGER, RONLY, var_online_cnu_table, 1, {1}},
	{OID_ONLINECNU_TABLE_CNUINDEX, ASN_INTEGER, RONLY, var_online_cnu_table, 1, {2}},
	{OID_ONLINECNU_TABLE_MACADDRESS, ASN_OCTET_STR, RONLY, var_online_cnu_table, 1, {3}},
	{modEoCCNUOnlineStatus, ASN_INTEGER, RONLY, var_online_cnu_table, 1, {4}},	
	{OID_ONLINECNU_TABLE_MODELNO, ASN_OCTET_STR, RONLY, var_online_cnu_table, 1, {5}},	
	{OID_ONLINECNU_TABLE_PORTAMOUNT, ASN_INTEGER, RONLY, var_online_cnu_table, 1, {6}},
	{OID_ONLINECNU_TABLE_AUTHORSTATE, ASN_INTEGER, RONLY, var_online_cnu_table, 1, {7}},
	{OID_ONLINECNU_TABLE_SOFTWAREVER, ASN_OCTET_STR, RONLY, var_online_cnu_table, 1, {8}},
	{OID_ONLINECNU_TABLE_CNURESET, ASN_INTEGER, RWRITE, var_online_cnu_table, 1, {9}},	
	{OID_ONLINECNU_TABLE_PHYDOWNRATE, ASN_INTEGER, RONLY, var_online_cnu_table, 1, {13}},
	{OID_ONLINECNU_TABLE_PHYUPRATE, ASN_INTEGER, RONLY, var_online_cnu_table, 1, {14}}
};
oid online_cnu_table_variables_oid[] = {EOC_OID_CNU_ONLINECNU, 4, 1};
/*======= ==========================================*/
static struct variable1 cnu_port_variables[] = 
{
	{OID_CNUPORT_PORTLIMIT, ASN_INTEGER, RONLY, var_cnu_port, 1, {1}},
	{OID_CNUPORT_AMOUNTLIMIT, ASN_INTEGER, RWRITE, var_cnu_port, 1, {2}},
};
oid cnu_port_variables_oid[] = {EOC_OID_CNU_PORT};

static struct variable1 cnu_port_table_variables[] = {
	{OID_CNUPORTTABLE_MASTERINDEX, ASN_INTEGER, RONLY, var_cnu_port_table, 1, {1}},
	{OID_CNUPORTTABLE_CNUINDEX, ASN_INTEGER, RONLY, var_cnu_port_table, 1, {2}},
	{OID_CNUPORTTABLE_PORTINDEX, ASN_INTEGER, RONLY, var_cnu_port_table, 1, {3}},
	{OID_CNUPORTTABLE_PORTEN, ASN_INTEGER, RWRITE, var_cnu_port_table, 1, {4}},
	{OID_CNUPORTTABLE_PORTSERVICE, ASN_INTEGER, RWRITE, var_cnu_port_table, 1, {5}}
};
oid cnu_port_table_variables_oid[] = {EOC_OID_CNU_PORT, 3, 1};

/*======= ==========================================*/
static struct variable1 cnu_variables[] = 
{
	{OID_PREVAIL_NAME, ASN_OCTET_STR, RONLY, var_cnu_prevail, 1, {1}},
	{OID_PREVAIL_CONTACT, ASN_OCTET_STR, RONLY, var_cnu_prevail, 1, {2}},
	{OID_PREVAIL_EAMIL, ASN_OCTET_STR, RONLY, var_cnu_prevail, 1, {3}},
	{OID_PREVAIL_DEVICETYPE, ASN_OCTET_STR, RONLY, var_cnu_prevail, 1, {4}},
};
oid cnu_variables_oid[] = {EOC_PREVAIL_OID};

static struct variable1 cnu_table_variables[] = {
	{OID_CNU_TABLE_MASTERINDEX, ASN_INTEGER, RONLY, var_cnu_table, 1, {1}},
	{OID_CNU_TABLE_CNUINDEX, ASN_INTEGER, RONLY, var_cnu_table, 1, {2}},
	{OID_CNU_TABLE_MACADDRESS, ASN_OCTET_STR, RONLY, var_cnu_table, 1, {3}},
	{OID_CNU_TABLE_STATUS, ASN_INTEGER, RONLY, var_cnu_table, 1, {4}},	
	{OID_CNU_TABLE_DEVTYPE, ASN_INTEGER, RONLY, var_cnu_table, 1, {5}},	
	{OID_CNU_TABLE_SOFTWAREVER, ASN_OCTET_STR, RONLY, var_cnu_table, 1, {6}},
	{OID_CNU_TABLE_CNURESET, ASN_INTEGER, RWRITE, var_cnu_table, 1, {7}},
	{OID_CNU_TABLE_RF, ASN_INTEGER, RONLY, var_cnu_table, 1, {8}},
	{OID_CNU_TABLE_RFDOWNQ, ASN_INTEGER, RONLY, var_cnu_table, 1, {9}},	
	{OID_CNU_TABLE_RFUPQ, ASN_INTEGER, RONLY, var_cnu_table, 1, {10}},
	{OID_CNU_TABLE_USERTYPE, ASN_INTEGER, RWRITE, var_cnu_table, 1, {11}},
	{OID_CNU_TABLE_ACCESSEN, ASN_INTEGER, RWRITE, var_cnu_table, 1, {12}},
	{OID_CNU_TABLE_TEMPID, ASN_INTEGER, RWRITE, var_cnu_table, 1, {13}},
	{OID_CNU_TABLE_AUTOUPEN, ASN_INTEGER, RONLY, var_cnu_table, 1, {14}},
	{OID_CNU_TABLE_PHYDOWNRATE, ASN_INTEGER, RONLY, var_cnu_table, 1, {15}},	
	{OID_CNU_TABLE_PHYUPRATE, ASN_INTEGER, RONLY, var_cnu_table, 1, {16}},
	{OID_CNU_TABLE_ONUSED, ASN_INTEGER, RONLY, var_cnu_table, 1, {17}}
};
oid cnu_table_variables_oid[] = {EOC_PREVAIL_OID, 5, 1};

static struct variable1 cnu_templatetable_variables[] = {
 	{OID_TEMPLATE_INDEX, ASN_INTEGER, RWRITE, var_template_table, 1, {1}},
	{OID_TEMPLATE_IDLE, ASN_INTEGER, RWRITE, var_template_table, 1, {2}},
	{OID_TEMPLATE_EN, ASN_INTEGER, RWRITE, var_template_table, 1, {3}},
	{OID_TEMPLATE_DEVTYPE, ASN_INTEGER, RWRITE, var_template_table, 1, {4}},	
	{OID_TEMPLATE_BASE, ASN_INTEGER, RWRITE, var_template_table, 1, {5}},	
	{OID_TEMPLATE_NAME, ASN_OCTET_STR, RWRITE, var_template_table, 1, {6}},
	{OID_TEMPLATE_ISMACLIMITVALID, ASN_INTEGER, RWRITE, var_template_table, 1, {7}},
	{OID_TEMPLATE_MACLIMITEN, ASN_INTEGER, RWRITE, var_template_table, 1, {8}},
	{OID_TEMPLATE_MACLIMITNUM, ASN_INTEGER, RWRITE, var_template_table, 1, {9}},	
	{OID_TEMPLATE_ISSTORMFILTERVALID, ASN_INTEGER, RWRITE, var_template_table, 1, {10}},
	{OID_TEMPLATE_BROADSTORMEN, ASN_INTEGER, RWRITE, var_template_table, 1, {11}},
	{OID_TEMPLATE_UNISTORMEN, ASN_INTEGER, RWRITE, var_template_table, 1, {12}},
	{OID_TEMPLATE_MULTISTORMEN, ASN_INTEGER, RWRITE, var_template_table, 1, {13}},
	{OID_TEMPLATE_STORMRATELEVEL, ASN_INTEGER, RWRITE, var_template_table, 1, {14}},
	{OID_TEMPLATE_ISVLANVALID, ASN_INTEGER, RWRITE, var_template_table, 1, {15}},	
	{OID_TEMPLATE_VLANEN, ASN_INTEGER, RWRITE, var_template_table, 1, {16}},
	{OID_TEMPLATE_PORT0VID, ASN_INTEGER, RWRITE, var_template_table, 1, {17}},
	{OID_TEMPLATE_PORT1VID, ASN_INTEGER, RWRITE, var_template_table, 1, {18}},	
	{OID_TEMPLATE_PORT2VID, ASN_INTEGER, RWRITE, var_template_table, 1, {19}},	
	{OID_TEMPLATE_PORT3VID, ASN_INTEGER, RWRITE, var_template_table, 1, {20}},
	{OID_TEMPLATE_ISPORTPRIVALID, ASN_INTEGER, RWRITE, var_template_table, 1, {21}},
	{OID_TEMPLATE_PORT0PRI, ASN_INTEGER, RWRITE, var_template_table, 1, {22}},
	{OID_TEMPLATE_PORT1PRI, ASN_INTEGER, RWRITE, var_template_table, 1, {23}},	
	{OID_TEMPLATE_PORT2PRI, ASN_INTEGER, RWRITE, var_template_table, 1, {24}},
	{OID_TEMPLATE_PORT3PRI, ASN_INTEGER, RWRITE, var_template_table, 1, {25}},
	{OID_TEMPLATE_ISRATELIMITVALID, ASN_INTEGER, RWRITE, var_template_table, 1, {26}},
	{OID_TEMPLATE_PORT0RXRATE, ASN_INTEGER, RWRITE, var_template_table, 1, {27}},
	{OID_TEMPLATE_PORT1RXRATE, ASN_INTEGER, RWRITE, var_template_table, 1, {28}},
	{OID_TEMPLATE_PORT2RXRATE, ASN_INTEGER, RWRITE, var_template_table, 1, {29}},	
	{OID_TEMPLATE_PORT3RXRATE, ASN_INTEGER, RWRITE, var_template_table, 1, {30}},
	{OID_TEMPLATE_PORT0TXRATE, ASN_INTEGER, RWRITE, var_template_table, 1, {31}},
	{OID_TEMPLATE_PORT1TXRATE, ASN_INTEGER, RWRITE, var_template_table, 1, {32}},
	{OID_TEMPLATE_PORT2TXRATE, ASN_INTEGER, RWRITE, var_template_table, 1, {33}},
	{OID_TEMPLATE_PORT3TXRATE, ASN_INTEGER, RWRITE, var_template_table, 1, {34}},	
	{OID_TEMPLATE_ISPORTENVALID, ASN_INTEGER, RWRITE, var_template_table, 1, {35}},
	{OID_TEMPLATE_PORT0EN, ASN_INTEGER, RWRITE, var_template_table, 1, {36}},
	{OID_TEMPLATE_PORT1EN, ASN_INTEGER, RWRITE, var_template_table, 1, {37}},
	{OID_TEMPLATE_PORT2EN, ASN_INTEGER, RWRITE, var_template_table, 1, {38}},
	{OID_TEMPLATE_PORT3EN, ASN_INTEGER, RWRITE, var_template_table, 1, {39}},
	{OID_TEMPLATE_PROGET, ASN_INTEGER, RWRITE, var_template_table, 1, {40}},
	{OID_TEMPLATE_SAVE, ASN_INTEGER, RWRITE, var_template_table, 1, {41}},
	{OID_TEMPLATE_RESULT, ASN_INTEGER, RONLY, var_template_table, 1, {42}}
};
oid cnu_templatetable_variables_oid[] = {EOC_PREVAIL_OID, 6};

static struct variable1 optentry_variables[] = {
 	{OID_SAVE_CONFIG, ASN_INTEGER, RWRITE, var_optentry, 1, {1}},
	{OID_REBOOT_CBAT, ASN_INTEGER, RWRITE, var_optentry, 1, {2}},
	{OID_RESTORE_CBAT, ASN_INTEGER, RWRITE, var_optentry, 1, {3}}
	
};

oid optentry_variables_oid[] = {EOC_PREVAIL_OID, 7};

static struct variable1 cbatstatus_variables[] = {
 	{OID_CBAT_SAVECONFIG, ASN_INTEGER, RWRITE, var_cbatstatus, 1, {8}}
	
};

oid cbatstatus_variables_oid[] = {OID_CBATSTATUSGROUP};
/**************************************************************************/
/*======  杭州万隆公司节点实现==================*/

unsigned char *prevail_info(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method)
{
	if(header_generic(vp, name, length, exact, var_len, write_method) ==  MATCH_FAILED)
		{return NULL;}	
	
	switch(vp->magic)
	{
		case PREVAIL_INFO_NAME:
			strcpy(array_value,"Hangzhou Prevail Optoelectronic Equipment Co.,LTD");
			*var_len = sizeof("Hangzhou Prevail Optoelectronic Equipment Co.,LTD");
			return array_value; 
			break;
		case PREVAIL_INFO_CONTACT:			
			strcpy(array_value,"XI YU XI");
			*var_len = sizeof("XI YU XI");
			return array_value;
			break;
		case PREVAIL_INFO_EMAIL:
			strcpy(array_value,"stanhangzhou&gmail.com");
			*var_len = sizeof("stanhangzhou&gmail.com");
			return array_value;
			break;
		case PREVAIL_INFO_DEVICETYPE:
			strcpy(array_value,"EOC-WEC-3502I");
			*var_len = sizeof("EOC-WEC-3502I");
			return array_value;
			break;
		default:
			break;
	}
	return 0;
	
}

unsigned char *var_online_cnu(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method)
{
	T_Msg_Header_CMM header;
	szOidVariables_t oidvar;
	T_CMM_MSG_ACK *ack;
	uint8_t buf[MAX_UDP_SIZE];
	if(header_generic(vp, name, length, exact, var_len, write_method) ==  MATCH_FAILED)
		{return NULL;}
	
	header.usSrcMID = MID_SNMP;
	header.usDstMID = MID_CMM;
	header.usMsgType = CMM_GET_SNMP_LEAF;
	header.fragment = 0;
	header.ulBodyLength = sizeof(szOidVariables_t);	
	oidvar.variables_oid_len = sizeof(oidvar.variables_oid);
	memcpy(oidvar.variables_oid, online_cnu_variables_oid, sizeof(online_cnu_variables_oid));
	
	*write_method =NULL;
	*var_len = sizeof(unsigned long);
	switch(vp->magic)
	{
		case OID_ONLINECNU_ALLRESET:
			*write_method = WriteMethod_cnuresetall; 	
			return array_value; 
			break;
		case OID_ONLINECNU_AMOUNT:			
			oidvar.magic = OID_ONLINECNU_AMOUNT;
			memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
			memcpy(buf + sizeof(T_Msg_Header_CMM), &oidvar, sizeof(szOidVariables_t));
			msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szOidVariables_t));
			ack = (T_CMM_MSG_ACK *)buf;
			*(unsigned long *)array_value = *(uint32_t *)(ack->BUF);
			return array_value;
			break;
		case OID_ONLINECNU_LIMIT:
			oidvar.magic = OID_ONLINECNU_LIMIT;
			memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
			memcpy(buf + sizeof(T_Msg_Header_CMM), &oidvar, sizeof(szOidVariables_t));
			msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szOidVariables_t));
			ack = (T_CMM_MSG_ACK *)buf;
			*(unsigned long *)array_value = *(uint32_t *)(ack->BUF);			
			return array_value;
			break;
		default:
			break;
	}	
	return 0;
}

void eoc_flushList ( char *l )
{
    eoc_list_t*	list;
    list = (eoc_list_t*)l;

    while ( !LIST_EMPTY ( list ))
    {
        eoc_hdlNode = LIST_FIRST ( list );
        if ( eoc_hdlNode->data )
        {
            free ( eoc_hdlNode->data );
        }
        LIST_REMOVE ( eoc_hdlNode, nodes );
        free ( eoc_hdlNode );
    }
}

void eoc_addList (char *l, char *data, int len, int refBytes)
{	
  eoc_list_t* list = (eoc_list_t*)l;
  // create a new node and the data that goes in it
  eoc_nwlNode = malloc(sizeof(struct eoc_link_node));
  eoc_nwlNode->data = malloc(len);
  memcpy (eoc_nwlNode->data, data, len);
	
  // this deals with an empty list
  if( LIST_EMPTY ( list )) 
  {
    LIST_INSERT_HEAD (list, eoc_nwlNode, nodes);
    return;
  }

  // this deals with UIDs that match
  for(eoc_hdlNode=LIST_FIRST(list); eoc_hdlNode!=NULL; eoc_hdlNode=LIST_NEXT(eoc_hdlNode, nodes)) 
  {
    if(memcmp(data, eoc_hdlNode->data, refBytes) == 0 ) // found matching UID
    {
      LIST_INSERT_AFTER (eoc_hdlNode, eoc_nwlNode, nodes);
      if (eoc_hdlNode->data) free(eoc_hdlNode->data);
      LIST_REMOVE (eoc_hdlNode, nodes);
      free (eoc_hdlNode);
      return;
    }
  }

  // this deals with inserting a new UID in the list
  for(eoc_hdlNode=LIST_FIRST(list); eoc_hdlNode!=NULL; eoc_hdlNode=LIST_NEXT(eoc_hdlNode, nodes))
  {
    eoc_lstNode = eoc_hdlNode;
    if (memcmp(eoc_hdlNode->data, data, refBytes) > 0 ) // old ID > new ID AND
    {
      LIST_INSERT_BEFORE (eoc_hdlNode, eoc_nwlNode, nodes);
      return;
    }
  }

  // this deals with a UID that needs to go on the end of the list
  LIST_INSERT_AFTER (eoc_lstNode, eoc_nwlNode, nodes);      
   return;
}


int load_onlinecnu_table(void)
{	
	int  i = 0;
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_CMM header;
	szTblVariables_t tblvar;
	table_data_t *tbl_data;
	T_CMM_MSG_ACK *ack;
	snmp_cnu_index_t *cnu_index;
	IS_CACH_TIMEOUT(onlinecnu_tbl.cachTime);
	eoc_flushList((char *)&onlinecnu_list);
	//在这里获取CNU数量
	snmp_cnu_index_t snmp_cnu_index;
	header.usSrcMID = MID_SNMP;
	header.usDstMID = MID_CMM;
	header.usMsgType = CMM_GET_ONLINECNU_INDEX;
	header.fragment = 0;
	header.ulBodyLength = 0;
	bzero(buf, MAX_UDP_SIZE);
	memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
	if(msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM)) != CMM_SUCCESS)
	{
		return SNMP_ERR_BADVALUE;
	}
	cnu_index = (snmp_cnu_index_t *)(buf + sizeof(T_CMM_MSG_HEADER_ACK));
	memcpy(&snmp_cnu_index, cnu_index, sizeof(snmp_cnu_index_t));
	
	if(snmp_cnu_index.index[0] != 0)
	{
		do
		{
			//在这里获取CNU 列表			
			header.usSrcMID = MID_SNMP;
			header.usDstMID = MID_CMM;
			header.usMsgType = CMM_GET_SNMP_TABLE_ROW;
			header.fragment = 0;
			header.ulBodyLength = sizeof(szTblVariables_t);
			memcpy(tblvar.variables_tbl, online_cnu_table_variables_oid, sizeof(online_cnu_table_variables_oid));
			tblvar.variables_tbl_len = sizeof(tblvar.variables_tbl);
			tblvar.row = snmp_cnu_index.index[i];
			bzero(buf, MAX_UDP_SIZE);
			memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
			memcpy(buf + sizeof(T_Msg_Header_CMM), &tblvar, sizeof(szTblVariables_t));
			msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szTblVariables_t));
			ack = (T_CMM_MSG_ACK *)buf;
			if(ack->header.Result != CMM_SUCCESS)
			{
				return SNMP_ERR_BADVALUE;
			}			
			tbl_data = (table_data_t *)ack->BUF;
			onlinecnu_tbl.master_index = tbl_data->CltIndex;
			onlinecnu_tbl.cnu_index = tbl_data->CnuIndex;
			memcpy(onlinecnu_tbl.mac, tbl_data->mac, 6);	
			onlinecnu_tbl.online = tbl_data->online;
			strcpy(onlinecnu_tbl.sw_ver, tbl_data->SwVer);
			onlinecnu_tbl.phydownrate = tbl_data->RxRate;
			onlinecnu_tbl.phyuprate = tbl_data->TxRate;
			if(tbl_data->DevType == WEC_3702I)
			{
				onlinecnu_tbl.ports = 2;
			}else if(tbl_data->DevType == WEC_3703I)
			{
				onlinecnu_tbl.ports = 3;
			}else if(tbl_data->DevType == WEC_3704I)
			{
				onlinecnu_tbl.ports = 4;
			}else
			{
				onlinecnu_tbl.ports = 0;
			}
			eoc_addList((char *)&onlinecnu_list, (char *)&onlinecnu_tbl, sizeof(onlinecnu_tbl),sizeof(long)*2);
			i++;
		}while((snmp_cnu_index.index[i] != 0) && (i<64));		
	}

	return SNMP_ERR_NOERROR;
}


/* table */
unsigned char *var_online_cnu_table(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method)
{
	oid rName [ MAX_OID_LEN ] = {0};	
	if(load_onlinecnu_table() != SNMP_ERR_NOERROR)
	{ 
		return NULL;
	}
	
	memcpy((char *)rName, (char *)vp->name, (int)vp->namelen * sizeof (oid));
	for(eoc_hdlNode=LIST_FIRST(&onlinecnu_list); eoc_hdlNode!=NULL; eoc_hdlNode=LIST_NEXT(eoc_hdlNode, nodes))
	{
		ponlinecnu_tbl = (onlinecnu_table_data *)eoc_hdlNode->data; 
		rName[vp->namelen] = ponlinecnu_tbl->master_index;
		rName[vp->namelen+1] = ponlinecnu_tbl->cnu_index;
		if((exact && (snmp_oid_compare(rName, vp->namelen+2, name, *length) == 0)) || //request
            (!exact && (snmp_oid_compare(rName, vp->namelen+2, name, *length) >  0)))  //exact == 0 means request next
        {
        	memcpy((char *)name, (char *)rName, (vp->namelen + 2) * sizeof(oid));
	     	*length = vp->namelen + 2;
	     	*var_len = sizeof (long); 
              *write_method = NULL;		
			switch(vp->magic)
			{
				case OID_ONLINECNU_TABLE_MASTERINDEX:
					return (unsigned char*)&ponlinecnu_tbl->master_index;
					break;
				case OID_ONLINECNU_TABLE_CNUINDEX:
					return (unsigned char*)&ponlinecnu_tbl->cnu_index;
					break;
				case OID_ONLINECNU_TABLE_MACADDRESS:	
					*var_len = 6;					
					return (unsigned char*)&ponlinecnu_tbl->mac;
					break;
				case modEoCCNUOnlineStatus:
					return (unsigned char*)&ponlinecnu_tbl->online;
					break;		
				case OID_ONLINECNU_TABLE_MODELNO:
					*var_len = strlen(ponlinecnu_tbl->model);
					return (unsigned char*)&ponlinecnu_tbl->model;
					break;
				case OID_ONLINECNU_TABLE_AUTHORSTATE:
					return (unsigned char*)&ponlinecnu_tbl->author_state;
					break;
				case OID_ONLINECNU_TABLE_SOFTWAREVER:
					*var_len = strlen(ponlinecnu_tbl->sw_ver);
					return (unsigned char*)&ponlinecnu_tbl->sw_ver;
					break;				
				case OID_ONLINECNU_TABLE_PORTAMOUNT:
					return (unsigned char*)&ponlinecnu_tbl->ports;
					break;			
				case OID_ONLINECNU_TABLE_CNURESET:		
					*write_method = WriteMethod_cnureset;
					return (unsigned char*)&ponlinecnu_tbl->reset;
					break;			
				case OID_ONLINECNU_TABLE_PHYDOWNRATE:
					return (unsigned char*)&ponlinecnu_tbl->phydownrate;
					break;
				case OID_ONLINECNU_TABLE_PHYUPRATE:	
					return (unsigned char*)&ponlinecnu_tbl->phyuprate;
					break;		
				default:
					break;
			}
		}
	}
	return 0;
}

unsigned char *var_cnu_port(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method)
{	
	T_Msg_Header_CMM header;
	szOidVariables_t oidvar;
	T_CMM_MSG_ACK *ack;
	uint8_t buf[MAX_UDP_SIZE];
	if(header_generic(vp, name, length, exact, var_len, write_method) ==  MATCH_FAILED)
        return NULL;

	header.usSrcMID = MID_SNMP;
	header.usDstMID = MID_CMM;
	header.usMsgType = CMM_GET_SNMP_LEAF;
	header.fragment = 0;
	header.ulBodyLength = sizeof(szOidVariables_t);	
	oidvar.variables_oid_len = sizeof(oidvar.variables_oid);
	memcpy(oidvar.variables_oid, cnu_port_variables_oid, sizeof(cnu_port_variables_oid));
	*write_method = 0;
	*var_len = sizeof(unsigned long);
	switch(vp->magic)
	{
		case OID_CNUPORT_PORTLIMIT:
			oidvar.magic = OID_CNUPORT_PORTLIMIT;
			memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
			memcpy(buf + sizeof(T_Msg_Header_CMM), &oidvar, sizeof(szOidVariables_t));
			msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szOidVariables_t));
			ack = (T_CMM_MSG_ACK *)buf;
			*(unsigned long *)array_value = *(uint32_t *)(ack->BUF);
			return array_value;
			break;
		case OID_CNUPORT_AMOUNTLIMIT:
			oidvar.magic = OID_CNUPORT_AMOUNTLIMIT;
			memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
			memcpy(buf + sizeof(T_Msg_Header_CMM), &oidvar, sizeof(szOidVariables_t));
			msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szOidVariables_t));
			ack = (T_CMM_MSG_ACK *)buf;
			*(unsigned long *)array_value = *(uint32_t *)(ack->BUF);
			return array_value;
			break;
		default:
			break;
	}

	return 0;
}

int load_cnu_port_table(void)
{

	IS_CACH_TIMEOUT(onlinecnu_tbl.cachTime);
	

	return SNMP_ERR_NOERROR;
	
}

unsigned char *var_cnu_port_table(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method)
{
	oid rName [ MAX_OID_LEN ] = {0};	
	if(load_cnu_port_table() != SNMP_ERR_NOERROR)
	{
		return NULL;
	}

	memcpy((char *)rName, (char *)vp->name, (int)vp->namelen * sizeof (oid));
	for(eoc_hdlNode=LIST_FIRST(&cnuport_list); eoc_hdlNode!=NULL; eoc_hdlNode=LIST_NEXT(eoc_hdlNode, nodes))
	{
		pcnuport_tbl = (cnuport_table_data *)eoc_hdlNode->data;	
		
        	rName[vp->namelen] = pcnuport_tbl->master_index;
	 	rName[vp->namelen+1] = pcnuport_tbl->cnu_index;
        	rName[vp->namelen+2] = pcnuport_tbl->port_index;

		if((exact && (snmp_oid_compare(rName, vp->namelen+3, name, *length) == 0)) || //request
            (!exact && (snmp_oid_compare(rName, vp->namelen+3, name, *length) >  0)))  //exact == 0 means request next
        {
        	memcpy((char *)name, (char *)rName, (vp->namelen + 3) * sizeof(oid));
	     	*length = vp->namelen + 3;
	     	*var_len = sizeof (long); 
            *write_method = NULL;
			switch(vp->magic)
			{
				case OID_CNUPORTTABLE_MASTERINDEX:
					return (unsigned char*)&pcnuport_tbl->master_index;
					break;
				case OID_CNUPORTTABLE_CNUINDEX:
					return (unsigned char*)&pcnuport_tbl->cnu_index;
					break;
				case OID_CNUPORTTABLE_PORTINDEX:
					return (unsigned char*)&pcnuport_tbl->port_index;
					break;
				case OID_CNUPORTTABLE_PORTEN:                    
                   
					break;
				case OID_CNUPORTTABLE_PORTSERVICE:
                  
					break;
				default:
					break;
			}
		}
	}
	
	return 0;
}

unsigned char *var_cnu_prevail(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method)
{
	if(header_generic(vp, name, length, exact, var_len, write_method) ==  MATCH_FAILED)
		{return NULL;}	
	
	switch(vp->magic)
	{
		case OID_PREVAIL_NAME:
			strcpy(array_value,"Hangzhou Prevail Optoelectronic Equipment Co.,LTD");
			*var_len = sizeof("Hangzhou Prevail Optoelectronic Equipment Co.,LTD");
			return array_value; 
			break;
		case OID_PREVAIL_CONTACT:			
			strcpy(array_value,"XI YU XI");
			*var_len = sizeof("XI YU XI");
			return array_value;
			break;
		case OID_PREVAIL_EAMIL:
			strcpy(array_value,"stanhangzhou&gmail.com");
			*var_len = sizeof("stanhangzhou&gmail.com");
			return array_value;
			break;
		case OID_PREVAIL_DEVICETYPE:
			strcpy(array_value,"EOC-WEC-3502I");
			*var_len = sizeof("EOC-WEC-3502I");
			return array_value;
			break;
		default:
			break;
	}
	return 0;
	
}

int load_cnu_table(void)
{	
	int  i = 0;
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_CMM header;
	szTblVariables_t tblvar;
	cnu_table_data_t *tbl_data;
	T_CMM_MSG_ACK *ack;
	IS_CACH_TIMEOUT(cnu_tbl.cachTime);
	eoc_flushList((char *)&cnu_list);

	bzero(buf, MAX_UDP_SIZE);
	header.usSrcMID = MID_SNMP;
	header.usDstMID = MID_CMM;
	header.usMsgType = CMM_GET_SNMP_TABLE_ROW;
	header.fragment = 0;
	header.ulBodyLength = sizeof(szTblVariables_t);
	memcpy(tblvar.variables_tbl, cnu_table_variables_oid, sizeof(cnu_table_variables_oid));
	tblvar.variables_tbl_len = sizeof(tblvar.variables_tbl);
	for(i = 0; i< MAX_CNU_AMOUNT_LIMIT; i++)
	{		
		//在这里获取CNU 列表			
		
		tblvar.row = i + 1;
		bzero(buf, MAX_UDP_SIZE);
		memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
		memcpy(buf + sizeof(T_Msg_Header_CMM), &tblvar, sizeof(szTblVariables_t));
		msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szTblVariables_t));
		ack = (T_CMM_MSG_ACK *)buf;
		
		if(ack->header.Result != CMM_SUCCESS)
		{
			return SNMP_ERR_BADVALUE;
		}			
		tbl_data = (cnu_table_data_t *)ack->BUF;
		if(tbl_data->onused == 0)
		{
			continue;
		}
		cnu_tbl.master_index = tbl_data->master_index;
		cnu_tbl.cnu_index = tbl_data->cnu_index;
		memcpy(cnu_tbl.mac, tbl_data->mac, 6);	
		cnu_tbl.online = tbl_data->online;
		cnu_tbl.accessenable = tbl_data->accessenable;
		cnu_tbl.autoupenable = tbl_data->autoupenable;
		cnu_tbl.onused = tbl_data->onused;
		cnu_tbl.tempid = tbl_data->tempid;
		strcpy(cnu_tbl.sw_ver, tbl_data->sw_ver);
		cnu_tbl.phydownrate = tbl_data->phydownrate;
		cnu_tbl.phyuprate = tbl_data->phyuprate;
		cnu_tbl.devtype = tbl_data->devtype;
		cnu_tbl.usertype = tbl_data->usertype;
		cnu_tbl.rfatt = tbl_data->rfatt;
		cnu_tbl.rfdownq = tbl_data->rfdownq;
		cnu_tbl.rfupq = tbl_data->rfupq;
		eoc_addList((char *)&cnu_list, (char *)&cnu_tbl, sizeof(cnu_tbl),sizeof(long)*2);
			
	}

	return SNMP_ERR_NOERROR;
}

/* table */
unsigned char *var_cnu_table(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method)
{
	oid rName [ MAX_OID_LEN ] = {0};	
	if(load_cnu_table() != SNMP_ERR_NOERROR)
	{ 
		return NULL;
	}
	memcpy((char *)rName, (char *)vp->name, (int)vp->namelen * sizeof (oid));
	for(eoc_hdlNode=LIST_FIRST(&cnu_list); eoc_hdlNode!=NULL; eoc_hdlNode=LIST_NEXT(eoc_hdlNode, nodes))
	{
		pcnu_tbl = (cnu_table_data *)eoc_hdlNode->data; 
		rName[vp->namelen] = pcnu_tbl->master_index;
		rName[vp->namelen+1] = pcnu_tbl->cnu_index;
		if((exact && (snmp_oid_compare(rName, vp->namelen+2, name, *length) == 0)) || //request
            (!exact && (snmp_oid_compare(rName, vp->namelen+2, name, *length) >  0)))  //exact == 0 means request next
        {
        	memcpy((char *)name, (char *)rName, (vp->namelen + 2) * sizeof(oid));
	     	*length = vp->namelen + 2;
	     	*var_len = sizeof (long); 
              *write_method = NULL;		
			switch(vp->magic)
			{
				case OID_CNU_TABLE_MASTERINDEX:
					return (unsigned char*)&pcnu_tbl->master_index;
					break;
				case OID_CNU_TABLE_CNUINDEX:
					return (unsigned char*)&pcnu_tbl->cnu_index;
					break;
				case OID_CNU_TABLE_MACADDRESS:					
					*var_len = 6;			
					return (unsigned char*)&pcnu_tbl->mac;
					break;
				case OID_CNU_TABLE_STATUS:
					return (unsigned char*)&pcnu_tbl->online;
					break;		
				case OID_CNU_TABLE_DEVTYPE:
					return (unsigned char*)&pcnu_tbl->devtype;
					break;
				case OID_CNU_TABLE_RF:
					return (unsigned char*)&pcnu_tbl->rfatt;
					break;
				case OID_CNU_TABLE_SOFTWAREVER:
					*var_len = strlen(pcnu_tbl->sw_ver);
					return (unsigned char*)&pcnu_tbl->sw_ver;
					break;				
				case OID_CNU_TABLE_USERTYPE:
					*write_method = WriteMethod_cnutable;
					return (unsigned char*)&pcnu_tbl->usertype;
					break;			
				case OID_CNU_TABLE_CNURESET:
					*write_method = WriteMethod_cnureset;
					return (unsigned char*)&pcnu_tbl->reset;
					break;			
				case OID_CNU_TABLE_PHYDOWNRATE:
					return (unsigned char*)&pcnu_tbl->phydownrate;
					break;
				case OID_CNU_TABLE_PHYUPRATE:	
					return (unsigned char*)&pcnu_tbl->phyuprate;
					break;	
				case OID_CNU_TABLE_ACCESSEN:
					*write_method = WriteMethod_cnutable;
					return (unsigned char*)&pcnu_tbl->accessenable;
					break;
				case OID_CNU_TABLE_TEMPID:	
					*write_method = WriteMethod_cnutable;
					return (unsigned char*)&pcnu_tbl->tempid;
					break;
				case OID_CNU_TABLE_AUTOUPEN:
					*write_method = WriteMethod_cnutable;
					return (unsigned char*)&pcnu_tbl->autoupenable;
					break;
				case OID_CNU_TABLE_ONUSED:	
					return (unsigned char*)&pcnu_tbl->onused;
					break;
				case OID_CNU_TABLE_RFDOWNQ:	
					return (unsigned char*)&pcnu_tbl->rfdownq;
					break;
				case OID_CNU_TABLE_RFUPQ:	
					return (unsigned char*)&pcnu_tbl->rfupq;
					break;
				default:
					break;
			}
		}
	}
	return 0;
}

int load_template_table(T_szTemplate *tbl_data)
{	
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_CMM header;
	szTblVariables_t tblvar;
	T_CMM_MSG_ACK *ack;
	header.usSrcMID = MID_SNMP;
	header.usDstMID = MID_CMM;
	header.usMsgType = CMM_GET_SNMP_TABLE_ROW;
	header.fragment = 0;
	header.ulBodyLength = sizeof(szTblVariables_t);
	memcpy(tblvar.variables_tbl, cnu_templatetable_variables_oid, sizeof(cnu_templatetable_variables_oid));
	tblvar.variables_tbl_len = sizeof(tblvar.variables_tbl);
	tblvar.row = szTemplate.tm_info.id;	
	bzero(buf, MAX_UDP_SIZE);
	memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
	memcpy(buf + sizeof(T_Msg_Header_CMM), &tblvar, sizeof(szTblVariables_t));
	msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szTblVariables_t));
	ack = (T_CMM_MSG_ACK *)buf;
	if(ack->header.Result != CMM_SUCCESS)
	{	 
		return SNMP_ERR_BADVALUE;
	}	
	memcpy(tbl_data, ack->BUF, sizeof(T_szTemplate));
	//tbl_data = (T_szTemplate *)ack->BUF;
	
#if 0
	int  i = 1;
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_CMM header;
	szTblVariables_t tblvar;
	T_szTemplate *tbl_data;
	T_CMM_MSG_ACK *ack;
	IS_CACH_TIMEOUT(template_tbl.cachTime);
	eoc_flushList((char *)&cnu_template_list);
	header.usSrcMID = MID_SNMP;
	header.usDstMID = MID_CMM;
	header.usMsgType = CMM_GET_SNMP_TABLE_ROW;
	header.fragment = 0;
	header.ulBodyLength = sizeof(szTblVariables_t);
	memcpy(tblvar.variables_tbl, cnu_templatetable_variables_oid, sizeof(cnu_templatetable_variables_oid));
	tblvar.variables_tbl_len = sizeof(tblvar.variables_tbl);
	while(1)
	{		
		//在这里获取CNU 列表			
		
		tblvar.row = i;
		bzero(buf, MAX_UDP_SIZE);
		memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
		memcpy(buf + sizeof(T_Msg_Header_CMM), &tblvar, sizeof(szTblVariables_t));
		msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szTblVariables_t));
		ack = (T_CMM_MSG_ACK *)buf;

		if(ack->header.Result != CMM_SUCCESS)
		{	
			return SNMP_ERR_BADVALUE;
		}			
		tbl_data = (T_szTemplate *)ack->BUF;
		if(tbl_data->tm_info.idle == 1)
		{	
			break;
		}
		template_tbl.tm_info.id = tbl_data->tm_info.id;		
		template_tbl.tm_info.idle = tbl_data->tm_info.idle;
		template_tbl.tm_info.enable = tbl_data->tm_info.enable;
		template_tbl.tm_info.DevType = tbl_data->tm_info.DevType;
		template_tbl.tm_info.base = tbl_data->tm_info.base;
		strcpy(template_tbl.tm_info.tname, tbl_data->tm_info.tname);
		template_tbl.pib.pib_spec.isValid = tbl_data->pib.pib_spec.isValid;
		template_tbl.pib.pib_spec.MacLimitEn = tbl_data->pib.pib_spec.MacLimitEn;
		template_tbl.pib.pib_spec.MacLimitNum = tbl_data->pib.pib_spec.MacLimitNum;
		template_tbl.mod.stormFilter.isValid = tbl_data->mod.stormFilter.isValid;
		template_tbl.mod.stormFilter.BroadStormEn = tbl_data->mod.stormFilter.BroadStormEn;
		template_tbl.mod.stormFilter.UniStormEn= tbl_data->mod.stormFilter.UniStormEn;
		template_tbl.mod.stormFilter.MultiStormEn= tbl_data->mod.stormFilter.MultiStormEn;
		template_tbl.mod.stormFilter.StormRateLevel= tbl_data->mod.stormFilter.StormRateLevel;
		template_tbl.mod.vlan.isValid = tbl_data->mod.vlan.isValid;
		template_tbl.mod.vlan.VlanEnable = tbl_data->mod.vlan.VlanEnable;
		template_tbl.mod.vlan.vid[0] = tbl_data->mod.vlan.vid[0];
		template_tbl.mod.vlan.vid[1] = tbl_data->mod.vlan.vid[1];
		template_tbl.mod.vlan.vid[2] = tbl_data->mod.vlan.vid[2];
		template_tbl.mod.vlan.vid[3] = tbl_data->mod.vlan.vid[3];
		
		template_tbl.mod.portPri.isValid = tbl_data->mod.portPri.isValid;
		template_tbl.mod.portPri.pri[0] = tbl_data->mod.portPri.pri[0];
		template_tbl.mod.portPri.pri[1] = tbl_data->mod.portPri.pri[1];
		template_tbl.mod.portPri.pri[2] = tbl_data->mod.portPri.pri[2];
		template_tbl.mod.portPri.pri[3] = tbl_data->mod.portPri.pri[3];
		template_tbl.mod.rateLimit.isValid = tbl_data->mod.rateLimit.isValid;
		template_tbl.mod.rateLimit.rxRate[0] = tbl_data->mod.rateLimit.rxRate[0];
		template_tbl.mod.rateLimit.rxRate[1] = tbl_data->mod.rateLimit.rxRate[1];
		template_tbl.mod.rateLimit.rxRate[2] = tbl_data->mod.rateLimit.rxRate[2];
		template_tbl.mod.rateLimit.rxRate[3] = tbl_data->mod.rateLimit.rxRate[3];
		template_tbl.mod.rateLimit.txRate[0] = tbl_data->mod.rateLimit.txRate[0];
		template_tbl.mod.rateLimit.txRate[1] = tbl_data->mod.rateLimit.txRate[1];
		template_tbl.mod.rateLimit.txRate[2] = tbl_data->mod.rateLimit.txRate[2];
		template_tbl.mod.rateLimit.txRate[3] = tbl_data->mod.rateLimit.txRate[3];
		template_tbl.mod.portAuth.isValid = tbl_data->mod.portAuth.isValid;
		template_tbl.mod.portAuth.portStatus[0] = tbl_data->mod.portAuth.portStatus[0];
		template_tbl.mod.portAuth.portStatus[1] = tbl_data->mod.portAuth.portStatus[1];
		template_tbl.mod.portAuth.portStatus[2] = tbl_data->mod.portAuth.portStatus[2];
		template_tbl.mod.portAuth.portStatus[3] = tbl_data->mod.portAuth.portStatus[3];

		eoc_addList((char *)&cnu_template_list, (char *)&template_tbl, sizeof(template_tbl),sizeof(long));
		i = tbl_data->tm_info.id + 1;
	}
#endif
	return SNMP_ERR_NOERROR;

}

/* table */
unsigned char *var_template_table(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method)
{	
	if(header_generic(vp, name, length, exact, var_len, write_method) ==  MATCH_FAILED)
		{return NULL;}
	*var_len = sizeof (uint8_t); 
       *write_method = NULL;		
	switch(vp->magic)
	{
		case OID_TEMPLATE_INDEX:		
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (long); 
			return (unsigned char*)&szTemplate.tm_info.id;			
			break;
		case OID_TEMPLATE_IDLE:
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.tm_info.idle;
			break;
		case OID_TEMPLATE_EN:						
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.tm_info.enable;
			break;
		case OID_TEMPLATE_DEVTYPE:
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.tm_info.DevType;
			break;		
		case OID_TEMPLATE_BASE:
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.tm_info.base;
			break;
		case OID_TEMPLATE_NAME:
			*write_method = WriteMethod_templatetable;
			*var_len = strlen(szTemplate.tm_info.tname);
			return (unsigned char*)&szTemplate.tm_info.tname;
			break;
		case OID_TEMPLATE_ISMACLIMITVALID:		
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.pib.pib_spec.isValid;
			break;				
		case OID_TEMPLATE_MACLIMITEN:
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.pib.pib_spec.MacLimitEn;
			break;			
		case OID_TEMPLATE_MACLIMITNUM:		
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (uint16_t); 
			return (unsigned char*)&szTemplate.pib.pib_spec.MacLimitNum;
			break;			
		case OID_TEMPLATE_ISSTORMFILTERVALID:
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.stormFilter.isValid;
			break;
		case OID_TEMPLATE_BROADSTORMEN:	
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.stormFilter.BroadStormEn;
			break;	
		case OID_TEMPLATE_UNISTORMEN:
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.stormFilter.UniStormEn;
			break;
		case OID_TEMPLATE_MULTISTORMEN:
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.stormFilter.MultiStormEn;
			break;
		case OID_TEMPLATE_STORMRATELEVEL:
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.stormFilter.StormRateLevel;
			break;
		case OID_TEMPLATE_ISVLANVALID:	
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.vlan.isValid;
			break;
		case OID_TEMPLATE_VLANEN:	
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.vlan.VlanEnable;
			break;
		case OID_TEMPLATE_PORT0VID:	
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (uint16_t); 
			return (unsigned char*)&szTemplate.mod.vlan.vid[0];
			break;
		case OID_TEMPLATE_PORT1VID:
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (uint16_t); 
			return (unsigned char*)&szTemplate.mod.vlan.vid[1];
			break;
		case OID_TEMPLATE_PORT2VID:
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (uint16_t); 
			return (unsigned char*)&szTemplate.mod.vlan.vid[2];
			break;
		case OID_TEMPLATE_PORT3VID:	
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (uint16_t); 
			return (unsigned char*)&szTemplate.mod.vlan.vid[3];
			break;
		case OID_TEMPLATE_ISPORTPRIVALID:
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.portPri.isValid;
			break;		
		case OID_TEMPLATE_PORT0PRI:
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.portPri.pri[0];
			break;
		case OID_TEMPLATE_PORT1PRI:
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.portPri.pri[1];
			break;
		case OID_TEMPLATE_PORT2PRI:
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.portPri.pri[2];
			break;				
		case OID_TEMPLATE_PORT3PRI:
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.portPri.pri[3];
			break;			
		case OID_TEMPLATE_ISRATELIMITVALID:	
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.rateLimit.isValid;
			break;			
		case OID_TEMPLATE_PORT0RXRATE:
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (uint32_t); 
			return (unsigned char*)&szTemplate.mod.rateLimit.rxRate[0];
			break;
		case OID_TEMPLATE_PORT1RXRATE:	
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (uint32_t); 
			return (unsigned char*)&szTemplate.mod.rateLimit.rxRate[1];
			break;	
		case OID_TEMPLATE_PORT2RXRATE:
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (uint32_t); 
			return (unsigned char*)&szTemplate.mod.rateLimit.rxRate[2];
			break;
		case OID_TEMPLATE_PORT3RXRATE:	
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (uint32_t); 
			return (unsigned char*)&szTemplate.mod.rateLimit.rxRate[3];
			break;
		case OID_TEMPLATE_PORT0TXRATE:
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (uint32_t); 
			return (unsigned char*)&szTemplate.mod.rateLimit.txRate[0];
			break;
		case OID_TEMPLATE_PORT1TXRATE:	
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (uint32_t); 
			return (unsigned char*)&szTemplate.mod.rateLimit.txRate[1];
			break;
		case OID_TEMPLATE_PORT2TXRATE:	
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (uint32_t); 
			return (unsigned char*)&szTemplate.mod.rateLimit.txRate[2];
			break;
		case OID_TEMPLATE_PORT3TXRATE:	
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (uint32_t); 
			return (unsigned char*)&szTemplate.mod.rateLimit.txRate[3];
			break;
		case OID_TEMPLATE_ISPORTENVALID:	
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.portAuth.isValid;
			break;
		case OID_TEMPLATE_PORT0EN:
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.portAuth.portStatus[0];
			break;
		case OID_TEMPLATE_PORT1EN:	
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.portAuth.portStatus[1];
			break;
		case OID_TEMPLATE_PORT2EN:	
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.portAuth.portStatus[2];
			break;
		case OID_TEMPLATE_PORT3EN:	
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&szTemplate.mod.portAuth.portStatus[3];
			break;
		case OID_TEMPLATE_PROGET:	
			*write_method = WriteMethod_templatetable;
			*var_len = sizeof (uint32_t); 
			return (unsigned char*)&szTemplate.tm_info.id;                         
			break;
		case OID_TEMPLATE_SAVE:	
			*write_method = WriteMethod_templatetable;
			return (unsigned char*)&template_sts;                         
			break;
		case OID_TEMPLATE_RESULT:	
			return (unsigned char*)&template_sts;                         
			break;
		default:
			break;
	}

	return 0;
#if 0
	oid rName [ MAX_OID_LEN ] = {0};	

	if(load_template_table() != SNMP_ERR_NOERROR)
	{ 
		return NULL;
	}
	memcpy((char *)rName, (char *)vp->name, (int)vp->namelen * sizeof (oid));
	for(eoc_hdlNode=LIST_FIRST(&cnu_template_list); eoc_hdlNode!=NULL; eoc_hdlNode=LIST_NEXT(eoc_hdlNode, nodes))
	{
		ptemplate_tbl = (cnu_table_data *)eoc_hdlNode->data; 
		rName[vp->namelen] = ptemplate_tbl->tm_info.id;
		//rName[vp->namelen+1] = ptemplate_tbl->tm_info.id;
		if((exact && (snmp_oid_compare(rName, vp->namelen+1, name, *length) == 0)) || //request
            (!exact && (snmp_oid_compare(rName, vp->namelen+1, name, *length) >  0)))  //exact == 0 means request next
        {
        	memcpy((char *)name, (char *)rName, (vp->namelen + 1) * sizeof(oid));
	     	*length = vp->namelen + 1;
	     	*var_len = sizeof (long); 
              *write_method = NULL;		
			switch(vp->magic)
			{
				case OID_TEMPLATE_INDEX:					
					return (unsigned char*)&ptemplate_tbl->tm_info.id;
					break;
				case OID_TEMPLATE_IDLE:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->tm_info.idle;
					break;
				case OID_TEMPLATE_EN:						
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->tm_info.enable;
					break;
				case OID_TEMPLATE_DEVTYPE:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->tm_info.DevType;
					break;		
				case OID_TEMPLATE_BASE:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->tm_info.base;
					break;
				case OID_TEMPLATE_NAME:
					*var_len = strlen(ptemplate_tbl->tm_info.tname);
					return (unsigned char*)&ptemplate_tbl->tm_info.tname;
					break;
				case OID_TEMPLATE_ISMACLIMITVALID:		
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->pib.pib_spec.isValid;
					break;				
				case OID_TEMPLATE_MACLIMITEN:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->pib.pib_spec.MacLimitEn;
					break;			
				case OID_TEMPLATE_MACLIMITNUM:			
					return (unsigned char*)&ptemplate_tbl->pib.pib_spec.MacLimitNum;
					break;			
				case OID_TEMPLATE_ISSTORMFILTERVALID:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.stormFilter.isValid;
					break;
				case OID_TEMPLATE_BROADSTORMEN:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.stormFilter.BroadStormEn;
					break;	
				case OID_TEMPLATE_UNISTORMEN:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.stormFilter.UniStormEn;
					break;
				case OID_TEMPLATE_MULTISTORMEN:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.stormFilter.MultiStormEn;
					break;
				case OID_TEMPLATE_STORMRATELEVEL:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.stormFilter.StormRateLevel;
					break;
				case OID_TEMPLATE_ISVLANVALID:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.vlan.isValid;
					break;
				case OID_TEMPLATE_VLANEN:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.vlan.VlanEnable;
					break;
				case OID_TEMPLATE_PORT0VID:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.vlan.vid[0];
					break;
				case OID_TEMPLATE_PORT1VID:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.vlan.vid[1];
					break;
				case OID_TEMPLATE_PORT2VID:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.vlan.vid[2];
					break;
				case OID_TEMPLATE_PORT3VID:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.vlan.vid[3];
					break;
				case OID_TEMPLATE_ISPORTPRIVALID:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.portPri.isValid;
					break;		
				case OID_TEMPLATE_PORT0PRI:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.portPri.pri[0];
					break;
				case OID_TEMPLATE_PORT1PRI:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.portPri.pri[1];
					break;
				case OID_TEMPLATE_PORT2PRI:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.portPri.pri[2];
					break;				
				case OID_TEMPLATE_PORT3PRI:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.portPri.pri[3];
					break;			
				case OID_TEMPLATE_ISRATELIMITVALID:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.rateLimit.isValid;
					break;			
				case OID_TEMPLATE_PORT0RXRATE:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.rateLimit.rxRate[0];
					break;
				case OID_TEMPLATE_PORT1RXRATE:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.rateLimit.rxRate[1];
					break;	
				case OID_TEMPLATE_PORT2RXRATE:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.rateLimit.rxRate[2];
					break;
				case OID_TEMPLATE_PORT3RXRATE:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.rateLimit.rxRate[3];
					break;
				case OID_TEMPLATE_PORT0TXRATE:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.rateLimit.txRate[0];
					break;
				case OID_TEMPLATE_PORT1TXRATE:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.rateLimit.txRate[1];
					break;
				case OID_TEMPLATE_PORT2TXRATE:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.rateLimit.txRate[2];
					break;
				case OID_TEMPLATE_PORT3TXRATE:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.rateLimit.txRate[3];
					break;
				case OID_TEMPLATE_ISPORTENVALID:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.portAuth.isValid;
					break;
				case OID_TEMPLATE_PORT0EN:
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.portAuth.portStatus[0];
					break;
				case OID_TEMPLATE_PORT1EN:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.portAuth.portStatus[1];
					break;
				case OID_TEMPLATE_PORT2EN:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.portAuth.portStatus[2];
					break;
				case OID_TEMPLATE_PORT3EN:	
					*write_method = WriteMethod_templatetable;
					return (unsigned char*)&ptemplate_tbl->mod.portAuth.portStatus[3];
					break;
				default:
					break;
			}
		}
	}
	return 0;
#endif
}

unsigned char *var_optentry(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method)
{	
	if(header_generic(vp, name, length, exact, var_len, write_method) ==  MATCH_FAILED)
		{return NULL;}
	*var_len = sizeof (long); 
       *write_method = NULL;		
	switch(vp->magic)
	{
		case OID_SAVE_CONFIG:		
			*write_method = WriteMethod_optentry;
			return (unsigned char*)&array_value;			
			break;
		case OID_REBOOT_CBAT:		
			*write_method = WriteMethod_optentry;
			return (unsigned char*)&array_value;			
			break;
		case OID_RESTORE_CBAT:		
			*write_method = WriteMethod_optentry;
			return (unsigned char*)&array_value;			
			break;
		default:
			break;

		}
	return 0;
}

unsigned char *var_cbatstatus(struct variable *vp,
        					                  oid     *name,
        					                  size_t  *length,
        					                  int     exact,
        					                  size_t  *var_len,
        					                  WriteMethod **write_method)
{	
	if(header_generic(vp, name, length, exact, var_len, write_method) ==  MATCH_FAILED)
		{return NULL;}
	*var_len = sizeof (long); 
       *write_method = NULL;		
	switch(vp->magic)
	{
		case OID_CBAT_SAVECONFIG:		
			*write_method = WriteMethod_cbatstatus;
			return (unsigned char*)&array_value;			
			break;
		default:
			break;

		}
	return 0;
}

void init_manager(void)
{
	init_snmp_sock();
	REGISTER_MIB("prevail",prevail_info_variabies,variable1,prevail_info_variabies_oid);
	REGISTER_MIB("online_cnu",online_cnu_variables,variable1,online_cnu_variables_oid);
	REGISTER_MIB("online_cnu_table",online_cnu_table_variables,variable1,online_cnu_table_variables_oid);
	REGISTER_MIB("cnu_port",cnu_port_variables,variable1,cnu_port_variables_oid);	
	REGISTER_MIB("cnu_port_table",cnu_port_table_variables,variable1,cnu_port_table_variables_oid);
	REGISTER_MIB("prevail_cnu",cnu_variables,variable1,cnu_variables_oid);
	REGISTER_MIB("prevail_cnutable",cnu_table_variables,variable1,cnu_table_variables_oid);
	REGISTER_MIB("prevail_templatetable",cnu_templatetable_variables,variable1,cnu_templatetable_variables_oid);
	REGISTER_MIB("optentry",optentry_variables,variable1,optentry_variables_oid);
	REGISTER_MIB("cbatdevstatusgroup",cbatstatus_variables,variable1,cbatstatus_variables_oid);
	onlinecnu_tbl.cachTime = 0;
	cnuport_tbl.cachTime = 0;
	cnu_tbl.cachTime = 0;
	//template_tbl.cachTime = 0;
	return ;
}


/*========写函数实现======================================*/
int WriteMethod_cnuresetall(int action,
                                             unsigned char *var_val,
                                             unsigned char var_val_type,
                                             size_t var_val_len, 
                                             unsigned char *statP, 
                                             oid *name, size_t length)
{
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_CMM header;
	szOidVariables_t tblvar;
	T_CMM_MSG_ACK *ack;
	switch(action)
	{
		case RESERVE1:	
			if (var_val_type != ASN_INTEGER) {
            			DEBUGMSGTL(("example", "%x not integer type", var_val_type));
           			 return SNMP_ERR_WRONGTYPE;
        		}
        		if (var_val_len > sizeof(int)) {
           			 DEBUGMSGTL(("example", "wrong length %x", var_val_len));
           			 return SNMP_ERR_WRONGLENGTH;
      			  }
			break;
		case RESERVE2:
			break;
		case ACTION:			
			header.usSrcMID=MID_SNMP;
			header.usDstMID=MID_CMM;
			header.usMsgType=CMM_SET_SNMP_LEAF;
			header.fragment=0;
			header.ulBodyLength=sizeof(szOidVariables_t);
			memcpy(tblvar.variables_oid,online_cnu_variables_oid,sizeof(online_cnu_variables_oid));
			tblvar.magic=OID_ONLINECNU_ALLRESET;
			tblvar.variables_oid_len = sizeof(tblvar.variables_oid);
			tblvar.value = 1;
			bzero(buf, MAX_UDP_SIZE);
			memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
			memcpy(buf + sizeof(T_Msg_Header_CMM), &tblvar, sizeof(szOidVariables_t));
			msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szOidVariables_t));
			ack = (T_CMM_MSG_ACK *)buf;
			if(ack->header.Result != CMM_SUCCESS)
			{
				return SNMP_ERR_BADVALUE;
			}
			*(unsigned long *)array_value = *(int *)ack->BUF;
			break;
		case UNDO:
			break;
		case COMMIT:
			break;
		case FREE:
			break;
	}

	return SNMP_ERR_NOERROR;
}


int WriteMethod_cnureset(int action,
                 u_char * var_val,
                 u_char var_val_type,
                 size_t var_val_len,
                 u_char * statP, oid * name, size_t name_len)
{
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_CMM header;
	szTblOidVariables_t tblvar;
	T_CMM_MSG_ACK *ack;
	switch (action) {
    case RESERVE1:
        /*
         *  Check that the value being set is acceptable
         */
        if (var_val_type != ASN_INTEGER) {
            DEBUGMSGTL(("example", "%x not integer type", var_val_type));
            return SNMP_ERR_WRONGTYPE;
        }
        if (var_val_len > sizeof(int)) {
            DEBUGMSGTL(("example", "wrong length %x", var_val_len));
            return SNMP_ERR_WRONGLENGTH;
        }
        break;

    case RESERVE2:   
        break;

    case FREE:       
        break;

    case ACTION:      	 
	 header.usSrcMID = MID_SNMP;
	 header.usDstMID = MID_CMM;
	 header.usMsgType = CMM_SET_SNMP_TABLE_NODE;
	 header.fragment = 0;
	 header.ulBodyLength = sizeof(szTblOidVariables_t);
	 memcpy(tblvar.variables_tbl, cnu_table_variables_oid, sizeof(cnu_table_variables_oid));
	 tblvar.variables_tbl_len = sizeof(tblvar.variables_tbl);
	 tblvar.row = pcnu_tbl->cnu_index;
	 tblvar.magic = OID_ONLINECNU_TABLE_CNURESET;
	 bzero(buf, MAX_UDP_SIZE);
	memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
	memcpy(buf + sizeof(T_Msg_Header_CMM), &tblvar, sizeof(szOidVariables_t));
	msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szOidVariables_t));
	ack = (T_CMM_MSG_ACK *)buf;
	*(unsigned long*)&array_value = ack->header.Result;
        break;

    case UNDO:        
        break;

    case COMMIT:      
        break;

    }
    return SNMP_ERR_NOERROR;
}

int WriteMethod_cnutable(int action,
                 u_char * var_val,
                 u_char var_val_type,
                 size_t var_val_len,
                 u_char * statP, oid * name, size_t name_len)
{
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_CMM header;
	szTblOidVariables_t tblvar;
	T_CMM_MSG_ACK *ack;
	int static val = 0;
	int leaf_id;
	switch (action) {
    case RESERVE1:
        /*
         *  Check that the value being set is acceptable
         */
        if (var_val_type != ASN_INTEGER) {
            DEBUGMSGTL(("example", "%x not integer type", var_val_type));
            return SNMP_ERR_WRONGTYPE;
        }
        if (var_val_len > sizeof(int)) {
            DEBUGMSGTL(("example", "wrong length %x", var_val_len));
            return SNMP_ERR_WRONGLENGTH;
        }	 
        break;

    case RESERVE2:   
        break;

    case FREE:       
        break;

    case ACTION:      	 
	 header.usSrcMID = MID_SNMP;
	 header.usDstMID = MID_CMM;
	 header.usMsgType = CMM_SET_SNMP_TABLE_NODE;
	 header.fragment = 0;
	 header.ulBodyLength = sizeof(szTblOidVariables_t);
	 memcpy(tblvar.variables_tbl, cnu_table_variables_oid, sizeof(cnu_table_variables_oid));
	 tblvar.variables_tbl_len = sizeof(tblvar.variables_tbl);
	 tblvar.row = pcnu_tbl->cnu_index;
	 leaf_id = (int) name[10 - 1];
	 switch(leaf_id)
	 {
		case OID_CNU_TABLE_TEMPID:
			 tblvar.magic = OID_CNU_TABLE_TEMPID;
			 break;
		case OID_CNU_TABLE_USERTYPE:
			 tblvar.magic = OID_CNU_TABLE_USERTYPE;
			 break;
		case OID_CNU_TABLE_ACCESSEN:
			 tblvar.magic = OID_CNU_TABLE_ACCESSEN;
			 break;
		case OID_CNU_TABLE_AUTOUPEN:
			 tblvar.magic = OID_CNU_TABLE_AUTOUPEN;
			 break;
	 }
	 //val = (uint32_t ) *var_val;
	 memcpy(&val, var_val, sizeof(uint32_t));
	 bzero(buf, MAX_UDP_SIZE);
	 memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
	 memcpy(buf + sizeof(T_Msg_Header_CMM), &tblvar, sizeof(szTblOidVariables_t));
	 memcpy(buf + sizeof(T_Msg_Header_CMM) + sizeof(szTblOidVariables_t), &val, sizeof(uint32_t));
	 msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szTblOidVariables_t) + sizeof(uint32_t));
	 ack = (T_CMM_MSG_ACK *)buf;
	 *(unsigned long*)&array_value = ack->header.Result;
        break;

    case UNDO:        
        break;

    case COMMIT:      
        break;

    }
    return SNMP_ERR_NOERROR;
}

int WriteMethod_templatetable(int action,
                 u_char * var_val,
                 u_char var_val_type,
                 size_t var_val_len,
                 u_char * statP, oid * name, size_t name_len)
{
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_CMM header;
	szTblOidVariables_t tblvar;
	T_CMM_MSG_ACK *ack;
	int static val = 0;
	uint32_t *proid;
	uint8_t tname[128] = {0};
	int leaf_id;
	switch (action) {
    case RESERVE1:
        /*
         *  Check that the value being set is acceptable
         */
         /*
        if (var_val_type != ASN_INTEGER) {
            DEBUGMSGTL(("example", "%x not integer type", var_val_type));
            return SNMP_ERR_WRONGTYPE;
        }
        if (var_val_len > sizeof(int)) {
            DEBUGMSGTL(("example", "wrong length %x", var_val_len));
            return SNMP_ERR_WRONGLENGTH;
        }	 
        */
        break;

    case RESERVE2:   
        break;

    case FREE:       
        break;

    case ACTION:      	 
	 header.usSrcMID = MID_SNMP;
	 header.usDstMID = MID_CMM;
	 header.usMsgType = CMM_SET_SNMP_TABLE_NODE;
	 header.fragment = 0;
	 header.ulBodyLength = sizeof(szTblOidVariables_t);
	 memcpy(tblvar.variables_tbl, cnu_templatetable_variables_oid, sizeof(cnu_templatetable_variables_oid));
	 tblvar.variables_tbl_len = sizeof(tblvar.variables_tbl);
	 tblvar.row = szTemplate.tm_info.id;
	 leaf_id = (int) name[9 - 1];
	 switch(leaf_id)
	 {
	 	case OID_TEMPLATE_INDEX:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.tm_info.id = val;
			 *(unsigned long*)&array_value = val;
			 break;
	 	case OID_TEMPLATE_IDLE:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.tm_info.idle = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_EN:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.tm_info.enable = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_DEVTYPE:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.tm_info.DevType = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_BASE:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.tm_info.base = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_NAME:
			 memcpy(tname, var_val, var_val_len);
			 strcpy(szTemplate.tm_info.tname, tname);
			 //*(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_ISMACLIMITVALID:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.pib.pib_spec.isValid = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_MACLIMITEN:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.pib.pib_spec.MacLimitEn = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_MACLIMITNUM:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.pib.pib_spec.MacLimitNum = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_ISSTORMFILTERVALID:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.stormFilter.isValid = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_BROADSTORMEN:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.stormFilter.BroadStormEn = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_UNISTORMEN:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.stormFilter.UniStormEn = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_MULTISTORMEN:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.stormFilter.MultiStormEn = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_STORMRATELEVEL:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.stormFilter.StormRateLevel = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_ISVLANVALID:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.vlan.isValid = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_VLANEN:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.vlan.VlanEnable = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT0VID:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.vlan.vid[0] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT1VID:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.vlan.vid[1] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT2VID:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.vlan.vid[2] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT3VID:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.vlan.vid[3] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_ISPORTPRIVALID:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.portPri.isValid = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT0PRI:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.portPri.pri[0] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT1PRI:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.portPri.pri[1] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT2PRI:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.portPri.pri[2] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT3PRI:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.portPri.pri[3] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_ISRATELIMITVALID:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.rateLimit.isValid = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT0RXRATE:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.rateLimit.rxRate[0] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT1RXRATE:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.rateLimit.rxRate[1] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT2RXRATE:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.rateLimit.rxRate[2] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT3RXRATE:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.rateLimit.rxRate[3] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT0TXRATE:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.rateLimit.txRate[0] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT1TXRATE:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.rateLimit.txRate[1] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT2TXRATE:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.rateLimit.txRate[2] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT3TXRATE:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.rateLimit.txRate[3] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_ISPORTENVALID:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.portAuth.isValid = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT0EN:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.portAuth.portStatus[0] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT1EN:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.portAuth.portStatus[1] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT2EN:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.portAuth.portStatus[2] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PORT3EN:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.mod.portAuth.portStatus[3] = val;
			 *(unsigned long*)&array_value = val;
			 break;
		case OID_TEMPLATE_PROGET:				
			 memcpy(&val, var_val, sizeof(uint32_t));
			 szTemplate.tm_info.id = val;
			// *(unsigned long*)&array_value = val;
			
			 if(load_template_table(&szTemplate) != SNMP_ERR_NOERROR)
			{ 
				template_sts = 1;
				return NULL;
			}			 
			 template_sts = 0;
			 
			 return SNMP_ERR_NOERROR;
			 break;
		case OID_TEMPLATE_SAVE:
			 memcpy(&val, var_val, sizeof(uint32_t));
			 tblvar.val = val;
			 tblvar.magic = OID_TEMPLATE_SAVE;
			 bzero(buf, MAX_UDP_SIZE);
			 memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
			 memcpy(buf + sizeof(T_Msg_Header_CMM), &tblvar, sizeof(szTblOidVariables_t));
			 memcpy(buf + sizeof(T_Msg_Header_CMM) + sizeof(szTblOidVariables_t), &szTemplate, sizeof(T_szTemplate));
			 msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szTblOidVariables_t) + sizeof(T_szTemplate));
			 ack = (T_CMM_MSG_ACK *)buf;
			 proid = (uint32_t *)(ack->BUF);
			 memcpy(&szTemplate.tm_info.id, proid, sizeof(uint32_t));
			 template_sts = ack->header.Result;
			 //*(unsigned long*)&array_value = ack->header.Result;
			 break;
		default:
			return SNMP_ERR_NOERROR;
			break;
	 }

	
        break;

    case UNDO:        
        break;

    case COMMIT:      
        break;

    }
    return SNMP_ERR_NOERROR;
}

int WriteMethod_optentry(int action,
                 u_char * var_val,
                 u_char var_val_type,
                 size_t var_val_len,
                 u_char * statP, oid * name, size_t name_len)
{
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_CMM header;
	szOidVariables_t tblvar;
	T_CMM_MSG_ACK *ack;
	int leaf_id;
	switch (action) {
    case RESERVE1:
        /*
         *  Check that the value being set is acceptable
         */
         /*
        if (var_val_type != ASN_INTEGER) {
            DEBUGMSGTL(("example", "%x not integer type", var_val_type));
            return SNMP_ERR_WRONGTYPE;
        }
        if (var_val_len > sizeof(int)) {
            DEBUGMSGTL(("example", "wrong length %x", var_val_len));
            return SNMP_ERR_WRONGLENGTH;
        }	 
        */
        break;

    case RESERVE2:   
        break;

    case FREE:       
        break;

    case ACTION:      	 
	 header.usSrcMID = MID_SNMP;
	 header.usDstMID = MID_CMM;
	 header.usMsgType = CMM_SET_SNMP_LEAF;
	 header.fragment = 0;
	 header.ulBodyLength = sizeof(szOidVariables_t);
	 memcpy(tblvar.variables_oid, optentry_variables_oid, sizeof(optentry_variables_oid));
	 tblvar.variables_oid_len = sizeof(tblvar.variables_oid);
	 leaf_id = (int) name[9 - 1];
	 switch(leaf_id)
	 {
	 	case OID_SAVE_CONFIG:
			 tblvar.magic = OID_SAVE_CONFIG;
			 bzero(buf, MAX_UDP_SIZE);
			 memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
			 memcpy(buf + sizeof(T_Msg_Header_CMM), &tblvar, sizeof(szOidVariables_t));			 
			 msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szOidVariables_t));
			 ack = (T_CMM_MSG_ACK *)buf;
			 *(unsigned long*)&array_value = ack->header.Result;
			 break;	 	
		case OID_REBOOT_CBAT:
			 tblvar.magic = OID_REBOOT_CBAT;
			 bzero(buf, MAX_UDP_SIZE);
			 memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
			 memcpy(buf + sizeof(T_Msg_Header_CMM), &tblvar, sizeof(szOidVariables_t));			 
			 msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szOidVariables_t));
			 ack = (T_CMM_MSG_ACK *)buf;
			 *(unsigned long*)&array_value = ack->header.Result;
			 break;
		case OID_RESTORE_CBAT:
			 tblvar.magic = OID_RESTORE_CBAT;
			 bzero(buf, MAX_UDP_SIZE);
			 memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
			 memcpy(buf + sizeof(T_Msg_Header_CMM), &tblvar, sizeof(szOidVariables_t));			 
			 msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szOidVariables_t));
			 ack = (T_CMM_MSG_ACK *)buf;
			 *(unsigned long*)&array_value = ack->header.Result;
			 break;
		default:
			return SNMP_ERR_NOERROR;
			break;
	 }

	
        break;

    case UNDO:        
        break;

    case COMMIT:      
        break;

    }
    return SNMP_ERR_NOERROR;
}


int WriteMethod_cbatstatus(int action,
                 u_char * var_val,
                 u_char var_val_type,
                 size_t var_val_len,
                 u_char * statP, oid * name, size_t name_len)
{
	uint8_t buf[MAX_UDP_SIZE];
	T_Msg_Header_CMM header;
	szOidVariables_t tblvar;
	T_CMM_MSG_ACK *ack;
	int leaf_id;
	switch (action) {
    case RESERVE1:
        /*
         *  Check that the value being set is acceptable
         */
         /*
        if (var_val_type != ASN_INTEGER) {
            DEBUGMSGTL(("example", "%x not integer type", var_val_type));
            return SNMP_ERR_WRONGTYPE;
        }
        if (var_val_len > sizeof(int)) {
            DEBUGMSGTL(("example", "wrong length %x", var_val_len));
            return SNMP_ERR_WRONGLENGTH;
        }	 
        */
        break;

    case RESERVE2:   
        break;

    case FREE:       
        break;

    case ACTION:      	 
	 header.usSrcMID = MID_SNMP;
	 header.usDstMID = MID_CMM;
	 header.usMsgType = CMM_SET_SNMP_LEAF;
	 header.fragment = 0;
	 header.ulBodyLength = sizeof(szOidVariables_t);
	 memcpy(tblvar.variables_oid, cbatstatus_variables_oid, sizeof(cbatstatus_variables_oid));
	 tblvar.variables_oid_len = sizeof(tblvar.variables_oid);
	 leaf_id = (int) name[12 - 1];
	 switch(leaf_id)
	 {
	 	case OID_CBAT_SAVECONFIG:
			 tblvar.magic = OID_CBAT_SAVECONFIG;
			 bzero(buf, MAX_UDP_SIZE);
			 memcpy(buf, &header, sizeof(T_Msg_Header_CMM));
			 memcpy(buf + sizeof(T_Msg_Header_CMM), &tblvar, sizeof(szOidVariables_t));			 
			 msg_communicate(sockfd, (uint8_t *) buf, sizeof(T_Msg_Header_CMM) + sizeof(szOidVariables_t));
			 ack = (T_CMM_MSG_ACK *)buf;
			 *(unsigned long*)&array_value = ack->header.Result;
			 break;	 	
		default:
			return SNMP_ERR_NOERROR;
			break;
	 }

	
        break;

    case UNDO:        
        break;

    case COMMIT:      
        break;

    }
    return SNMP_ERR_NOERROR;
}
/*============================================================================*/

#endif

