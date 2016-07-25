/*****************************************************************************************
  文件名称 : cmm.c
  文件描述 : cmm模块主入口函数
  				主要处理来自UI子系统内部子模块的请求。为了简化设
  				计，CMM被设计为单进程单线程，即处理完前一个请求
  				，后面的请求才被执行；对于那些需要耗时较长的操
  				作（如升级请求），CMM需要立即返回，然后将请求交
  				给后台进程去处理，外部模块通过额外的查询请求来
  				获取操作的处理状态；操作日志的记录在CMM模块中进行。
  修订记录 :
           1 创建 : frank
             日期 : 2010-08-13
             描述 : 创建文件

 *****************************************************************************************/
#include <assert.h>
#include <signal.h>
#include <net/route.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/mii.h>
#include <linux/sem.h>
#include <dirent.h>
#include <fcntl.h>
#include <dbsapi.h>
#include <boardapi.h>
//#include <mtd-abi.h>
#include "cmm.h"
#include "cmm_tm.h"
#include "cmm_mmead.h"
#include "cmm_reg.h"
#include "cmm_alarm.h"
#include "cmm_dsdt.h"
#include "cmm_rtl8306e.h"
#include "upgrade.h"
#include "at30tk175stk/at30ts75.h"

int CMM_MODULE_DEBUG_ENABLE = 0;

T_UDP_SK_INFO SK_CMM;	/* 负责处理外部请求的套接字*/

static BBLOCK_QUEUE bblock;

/* 与DBS  通讯的设备文件*/
T_DBS_DEV_INFO *dbsdev = NULL;

int __findCnuEntry()
{
	int i = 0;
	DB_INTEGER_V iValue;

	for( i=1; i<=MAX_CNU_AMOUNT_LIMIT; i++ )
	{
		iValue.ci.tbl = DBS_SYS_TBL_ID_CNU;
		iValue.ci.row = i;
		iValue.ci.col = DBS_SYS_TBL_CNU_COL_ID_ROWSTS;
		iValue.ci.colType = DBS_INTEGER;
		if( CMM_SUCCESS != dbsGetInteger(dbsdev, &iValue))
		{
			return 0;
		}
		else if( 0 == iValue.integer )
		{
			return 1;
		}
	}
	return 0;
}

int __isNewCnuMacaddr(uint8_t mac[])
{
	int i = 0;
	DB_TEXT_V strValue;
	uint8_t MA[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
	uint8_t MB[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
	uint8_t MR[6] = {0};
	
	/* 不允许00:00:00:00:00:00 */
	if( memcmp(mac, MA, 6) == 0 )
	{
		return 0;
	}
	/* 不允许FF:FF:FF:FF:FF:FF */
	if( memcmp(mac, MB, 6) == 0 )
	{
		return 0;
	}
	/* 寻找CNU 表，防止重复*/
	for( i=1; i<MAX_CNU_AMOUNT_LIMIT; i++ )
	{
		strValue.ci.tbl = DBS_SYS_TBL_ID_CNU;
		strValue.ci.row = i;
		strValue.ci.col = DBS_SYS_TBL_CNU_COL_ID_MAC;
		strValue.ci.colType = DBS_TEXT;
		if( CMM_SUCCESS != dbsGetText(dbsdev, &strValue) )
		{
			return 0;
		}
		else if( CMM_SUCCESS != boardapi_macs2b(strValue.text, MR))
		{
			return 0;
		}
		else if( memcmp(mac, MR, 6) == 0 )
		{
			return 0;
		}
	}
	return 1;
}

/********************************************************************************************
*	函数名称:do_network_config
*	函数功能:设置本机ip ,网管,子网掩码
*	作者:may2250
*	时间:2010-09-13
*********************************************************************************************/
int do_network_config(st_dbsNetwork networkinfo)
{
	int fd;
	int I_ip ,I_mask, I_gw;
	struct ifreq ifr; 
	struct sockaddr_in *sin;
	struct rtentry  rt;
	char *ifname="eth0";
    
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
	{
		perror("do_network_config socket error !\n");     
		return CMM_FAILED;
	}
    	memset(&ifr,0,sizeof(ifr)); 
    	strcpy(ifr.ifr_name, "eth0"); 
	sin = (struct sockaddr_in*)&ifr.ifr_addr;     
	sin->sin_family = AF_INET;     
 
    	if(inet_aton(networkinfo.col_ip, &(sin->sin_addr)) < 0)   
	{     
		perror("do_network_config->inet_aton error !\n");
		close(fd);
		return CMM_FAILED;     
	}    
    
	if(ioctl(fd,SIOCSIFADDR,&ifr) < 0)   
	{     
		perror("do_network_config ioctl error !\n");
		close(fd);
		return CMM_FAILED;     
	}
   	 //netmask
	if(inet_aton(networkinfo.col_netmask, &(sin->sin_addr)) < 0)   
	{     
		perror("inet_pton   error");
		close(fd);
		return CMM_FAILED;     
	}    
	if(ioctl(fd, SIOCSIFNETMASK, &ifr) < 0)
	{
		perror("do_network_config ioctl error !\n");
		close(fd);
		return CMM_FAILED; 
	}
/*****************************************************/
/* 如果IP和网关不在同一个网段内，此段
** 程序会导致设备网络异常，暂时屏蔽*/
/*****************************************************/
#if 1
   	 //gateway
    	memset(&rt, 0, sizeof(struct rtentry));
    	memset(sin, 0, sizeof(struct sockaddr_in));
    	sin->sin_family = AF_INET;
    	sin->sin_port = 0;
		
	if(strlen(networkinfo.col_gw) == 0)
	{
		close(fd);
		return CMM_SUCCESS;
	}		

	I_ip = inet_addr(networkinfo.col_ip);
	I_mask = inet_addr(networkinfo.col_netmask);
	I_gw = inet_addr(networkinfo.col_gw);
	
	if((I_ip & I_mask) == (I_gw & I_mask))
	{
		if(inet_aton(networkinfo.col_gw, &sin->sin_addr)<0)
		{
			printf ( "inet_aton error\n" );
			close(fd);
			return CMM_FAILED; 
		}
		memcpy ( &rt.rt_gateway, sin, sizeof(struct sockaddr_in));
		((struct sockaddr_in *)&rt.rt_dst)->sin_family=AF_INET;
		((struct sockaddr_in *)&rt.rt_genmask)->sin_family=AF_INET;
		rt.rt_dev = ifname;
		rt.rt_flags = RTF_UP|RTF_GATEWAY;
		if (ioctl(fd, SIOCADDRT, &rt)<0)
		{
			perror( "ioctl(SIOCADDRT) error in set_default_route\n");
			close(fd);
			return CMM_FAILED;
		}			
	}
	
	
#endif

	close(fd);
	return CMM_SUCCESS;
}

int do_dsdt_rgmii_delay_config(st_dsdtRgmiiTimingDelay* pDelay)
{
	int opt_sts=CMM_SUCCESS;

	opt_sts = cmm2dsdt_setRgmiiTimingDelay(pDelay);
	
	if( CMM_SUCCESS ==  opt_sts )
	{
		if( pDelay->rxdelay )
		{
			dbs_sys_log(dbsdev, DBS_LOG_DEBUG, "dsdt port6 rgmii rx delay enabled");
		}
		if( pDelay->txdelay )
		{
			dbs_sys_log(dbsdev, DBS_LOG_DEBUG, "dsdt port6 rgmii tx delay enabled");
		}
	}

	return opt_sts;
}

/********************************************************************************************
*	函数名称:init_network
*	函数功能:从数据库读取网络配置信息,初始化配置
*	作者:may2250
*	时间:2010-09-13
*********************************************************************************************/
int init_network(void)
{
	//int opt_sts=CMM_SUCCESS;	
	st_dbsNetwork	szNetwork;
	st_dbsSysinfo		szSysinfo;
	st_dsdtRgmiiTimingDelay dsdtRgmiiDelay;

	/*从数据库中读取配置信息*/
	if(CMM_SUCCESS == dbsGetSysinfo(dbsdev, 1, &szSysinfo))
	{
		/* dsdt rgmii delay config */
		dsdtRgmiiDelay.port = 6;
		dsdtRgmiiDelay.rxdelay = szSysinfo.col_p6rxdelay;
		dsdtRgmiiDelay.txdelay = szSysinfo.col_p6txdelay;
		if( CMM_SUCCESS != do_dsdt_rgmii_delay_config(&dsdtRgmiiDelay) )
		{
			perror("cmm init_network->do_dsdt_rgmii_delay_config failed !\n");
			return CMM_FAILED;
		}		
	}
	else
	{
		perror("CMM init_network->dbsGetSysinfo error !\n");
		return CMM_FAILED;
	}
	
	if(CMM_SUCCESS == dbsGetNetwork(dbsdev, 1, &szNetwork))
	{
		/* 设置ip,子网掩码,网关*/
		if( do_network_config(szNetwork) != CMM_SUCCESS )
		{
			perror("CMM init_network->do_network_config error !\n");
			return CMM_FAILED;
		}
		/* 配置管理VLAN */
		if( cmm2dsdt_mgmtVlanInit() != CMM_SUCCESS )
		{
			perror("CMM init_network->cmm2dsdt_mgmtVlanInit error !\n");
			return CMM_FAILED;
		}
		/*其它配置*/
		return CMM_SUCCESS;
	}
	else
	{
		perror("CMM init_network->db_get_network_info error !\n");
		return CMM_FAILED;
	}
}

/********************************************************************************************
*	函数名称:cmm_dump_msg
*	函数功能:调试用API，以十六进制的方式将缓冲区的内容输出到
*				   文件指针fp指向的设备文件
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void cmm_dump_msg(const unsigned char memory [], size_t length, FILE *fp)
{
	if(CMM_MODULE_DEBUG_ENABLE)
	{
		printf("----------------------------------------------------------------------\n");
		hexdump(memory, length, fp);
		printf("\n----------------------------------------------------------------------\n");
	}
}

/********************************************************************************************
*	函数名称:CMM_ProcessAck
*	函数功能:该函数为CMM外部应答函数，CMM请求处理
*				  完成之后通过该函数将处理结果返回给请求者
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void CMM_ProcessAck(uint16_t status, BBLOCK_QUEUE *this, uint8_t *b, uint32_t len)
{
	assert(NULL != this);
	
	T_Msg_Header_CMM rh;
	T_Msg_Header_CMM *h = (T_Msg_Header_CMM *)(this->b);
	uint8_t buf[MAX_UDP_SIZE];
	uint16_t result = status;
	int sendn = 0;
	int total_len = len + sizeof(uint16_t) + sizeof(rh);

	bzero((char *)buf, MAX_UDP_SIZE);

	rh.usSrcMID = MID_CMM;
	rh.usDstMID = h->usSrcMID;
	rh.usMsgType = h->usMsgType;
	rh.fragment = 0;
	rh.ulBodyLength = (len + sizeof(uint16_t));		

	if( total_len > MAX_UDP_SIZE )
	{
		result = CMM_BUFFER_OVERFLOW;
		rh.ulBodyLength = sizeof(uint16_t);
		memcpy(buf, &rh, sizeof(rh));
		memcpy(buf+sizeof(rh), &result, sizeof(uint16_t));
		dbs_sys_log(dbsdev, DBS_LOG_ALERT, "cmm->nCMM_ProcessAck msg length exceeded");	
		
		/* 将处理信息发送给请求者 */
		sendn = sendto(this->sk.sk, buf, sizeof(rh)+sizeof(uint16_t), 0, 
				(struct sockaddr *)&(this->sk.skaddr), sizeof(this->sk.skaddr));
		if( sendn > 0 )
		{
			/* 打印接收的数据报文*/
			cmm_debug_printf("\n<==== CMM SEND MASSAGE:\n");
			cmm_dump_msg(buf, sizeof(rh)+sizeof(uint16_t)+len, stdout);
		}
		return;
	}
	
	memcpy(buf, &rh, sizeof(rh));
	memcpy(buf+sizeof(rh), &result, sizeof(uint16_t));

	if( 0 != len )
	{
		assert(NULL != b);
		memcpy(buf+sizeof(rh)+sizeof(uint16_t), b, len);
	}
	
	/* 打印接收的数据报文*/
	cmm_debug_printf("\n<==== CMM SEND MASSAGE:\n");
	cmm_dump_msg(buf, sizeof(rh)+sizeof(uint16_t)+len, stderr);
		
	/* 将处理信息发送给请求者 */
	sendn = sendto(this->sk.sk, buf, total_len, 0, 
				(struct sockaddr *)&(this->sk.skaddr), sizeof(this->sk.skaddr));
	if( sendn != total_len )
	{
		dbs_sys_log(dbsdev, DBS_LOG_ALERT, "cmm->nCMM_ProcessAck call sendto error");
	}
}

/********************************************************************************************
*	函数名称:CMM_ProcessMTAck
*	函数功能:向生产测试工具发送应答的消息接口
*	作者:may2250
*	时间:2011-4-26
*********************************************************************************************/
void CMM_ProcessMTAck(BBLOCK_QUEUE *this, uint8_t *b, uint32_t len)
{
	if(sendto(this->sk.sk, b, len, 0,(struct sockaddr *)&(this->sk.skaddr), sizeof(this->sk.skaddr) ) == -1)
	{
		dbs_sys_log(dbsdev, DBS_LOG_ALERT, "ERROR: CMM_ProcessMTAck sendto failed");
		perror("\nERROR: CMM_ProcessMTAck sendto failed\n");
	}
}

#if 0
/********************************************************************************************
*	函数名称:CMM_ProcessSequenceAck
*	函数功能:该函数为CMM外部应答函数，CMM请求处理
*				  完成之后通过该函数将处理结果返回给请求者
*				  该函数完成超长报文的应答
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void CMM_ProcessSequenceAck(uint16_t status, BBLOCK_QUEUE *this, uint8_t *b, uint32_t len)
{
	assert(NULL != this);

	uint8_t *p = b;
	T_Msg_Header_CMM rh;
	T_Msg_Header_CMM *h = (T_Msg_Header_CMM *)(this->b);
	uint8_t buf[MAX_UDP_SIZE];
	uint16_t result = status;
	int sendn = 0;
	int i = 1;
	int lenth = 0;
	int remain_len = len;
	int step = (MAX_UDP_SIZE-sizeof(rh)-sizeof(uint16_t));

	rh.usSrcMID = MID_CMM;
	rh.usDstMID = h->usSrcMID;
	rh.usMsgType = h->usMsgType;	

	for( remain_len=len; remain_len>0; remain_len-=step,p+=step )
	{
		bzero((char *)buf, MAX_UDP_SIZE);
		if( remain_len > step )
		{
			/* 如果不是最后一帧*/
			rh.fragment = 1;
			rh.ulBodyLength = (step + sizeof(uint16_t));
			memcpy(buf, &rh, sizeof(rh));
			memcpy(buf+sizeof(rh), &result, sizeof(uint16_t));
			memcpy(buf+sizeof(rh)+sizeof(uint16_t), p, step);
			lenth = MAX_UDP_SIZE;
		}
		else
		{
			/* 最后一帧*/
			rh.fragment = 0;
			rh.ulBodyLength = (remain_len + sizeof(uint16_t));
			memcpy(buf, &rh, sizeof(rh));
			memcpy(buf+sizeof(rh), &result, sizeof(uint16_t));
			memcpy(buf+sizeof(rh)+sizeof(uint16_t), p, remain_len);
			lenth = (sizeof(rh)+sizeof(uint16_t)+remain_len);
		}

		/* 打印接收的数据报文*/
		cmm_debug_printf("\n<==== CMM SEND MASSAGE [ SYN = %d ]:\n", i++);
		cmm_dump_msg(buf, lenth, stderr);
		
		/* 将处理信息发送给请求者 */
		sendn = sendto(this->sk.sk, buf, lenth, 0, (struct sockaddr *)&(this->sk.skaddr), sizeof(this->sk.skaddr));
		if( sendn != lenth )
		{
			dbs_sys_log(DBS_LOG_ALERT, "cmm->nCMM_ProcessAck call sendto error");
		}
	}	
}
#endif

int CMM_WriteOptLog(BBLOCK_QUEUE *this, int result)
{
	time_t b_time;
	st_dbsOptlog log;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);

	/* HTTP 的操作日志在HTTPD中已经记录*/
	if( req->HEADER.usSrcMID == MID_HTTP )
	{
		return CMM_SUCCESS;
	}
	if( req->HEADER.usSrcMID == MID_SYSMONITOR )
	{
		return CMM_SUCCESS;
	}
	if( req->HEADER.usSrcMID == MID_REGISTER )
	{
		return CMM_SUCCESS;
	}
	if( req->HEADER.usMsgType == CMM_GET_CBAT_TEMPERATURE )
	{
		return CMM_SUCCESS;
	}
	if( req->HEADER.usMsgType == CMM_DEBUG_PRINT_PORT_STAT )
	{
		return CMM_SUCCESS;
	}

	/* 获取系统当前时间*/
	time(&b_time);

	log.who = req->HEADER.usSrcMID;
	log.time = b_time;
	log.level = DBS_LOG_INFO;
	log.result = result?CMM_FAILED:CMM_SUCCESS;

	switch(req->HEADER.usMsgType)
	{
		case CMM_MODULE_DEBUG_CONTROL:
		{
			strcpy(log.cmd, "CMM_MODULE_DEBUG_CONTROL");
			break;
		}
		case CMM_CLT_RESET:
		{
			strcpy(log.cmd, "CMM_RESET_CLT");
			break;
		}
		case CMM_CNU_RESET:
		{
			strcpy(log.cmd, "CMM_RESET_CNU");
			break;
		}
		case CMM_SEND_TOPOLOGY_HB_TRAP:
		{
			strcpy(log.cmd, "SNMP_CMM_SEND_HEARTBEAT");
			break;
		}
		case CMM_DO_HB_TRAP_CTRL:
		{
			strcpy(log.cmd, "CMM_DO_HB_TRAP_CTRL");
			break;
		}
		case CMM_UNDO_HB_TRAP_CTRL:
		{
			strcpy(log.cmd, "CMM_UNDO_HB_TRAP_CTRL");
			break;
		}
		case CMM_CBAT_RESET:
		case CMM_RESTORE_DEFAULT:
		{
			/* 该命令导致系统重启，保存日志信息将没有意义*/
			break;
		}
		case CMM_CNU_VLAN_CONFIG:
		{
			strcpy(log.cmd, "CMM_DO_CNU_VLAN_CONFIG");
			break;
		}
		case CMM_CLI_FLOWCONTROL:
		{
			strcpy(log.cmd, "CMM_DO_CNU_RATE_LIMITING");
			break;
		}
		case CMM_CLI_STROMCONTROL:
		{
			strcpy(log.cmd, "CMM_DO_CNU_STORM_FILTER");
			break;
		}
		case CMM_CLI_SHUTDOWN:
		{
			strcpy(log.cmd, "CMM_DO_PROT_STATUS_CONFIG");
			break;
		}
		case CMM_AR8236_PHY_REG_READ:
		{
			strcpy(log.cmd, "CMM_DEBUG_PHY_REG_READ");
			break;
		}
		case CMM_AR8236_PHY_REG_WRITE:
		{
			strcpy(log.cmd, "CMM_DEBUG_PHY_REG_WRITE");
			break;
		}
		case CMM_AR8236_SW_REG_READ:
		{
			strcpy(log.cmd, "CMM_DEBUG_SW_REG_READ");
			break;
		}
		case CMM_USER_HFID_READ:
		{
			strcpy(log.cmd, "CMM_DEBUG_USER_HFID_READ");
			break;
		}
		case CMM_AR8236_SW_REG_WRITE:
		{
			strcpy(log.cmd, "CMM_DEBUG_SW_REG_WRITE");
			break;
		}
		case CMM_MME_MDIO_READ:
		{
			strcpy(log.cmd, "CMM_MME_MDIO_READ");
			break;
		}
		case CMM_MME_MDIO_WRITE:
		{
			strcpy(log.cmd, "CMM_MME_MDIO_WRITE");
			break;
		}
		case CMM_CLI_UPGRADE:
		{
			strcpy(log.cmd, "CMM_UPGRADE_SOFTWARE");
			break;
		}
		case CMM_CLI_SEND_CONFIG:
		{
			strcpy(log.cmd, "CMM_RELOAD_CNU_PROFILE");
			break;
		}
		case CMM_CLI_USER_NEW:
		{
			strcpy(log.cmd, "CMM_PERMIT_CNU");
			break;
		}
		case CMM_CLI_USER_DEL:
		{
			strcpy(log.cmd, "CMM_UNDO_PERMIT_CNU");
			break;
		}
		case CMM_CLI_DELETE_USER:
		{
			strcpy(log.cmd, "CMM_DELETE_CNU");
			break;
		}
		case CMM_DO_WLIST_CONTROL:
		{
			strcpy(log.cmd, "CMM_ENABLE_WLIST");
			break;
		}
		case CMM_UNDO_WLIST_CONTROL:
		{
			strcpy(log.cmd, "CMM_DISABLE_WLIST");
			break;
		}
		case CMM_CREATE_CNU:
		{
			strcpy(log.cmd, "CMM_CREATE_CNU");
			break;
		}
		case CMM_DUMP_CNU_REG:
		{
			strcpy(log.cmd, "CMM_DEBUG_DUMP_CNU_REG");
			break;
		}
		case CMM_DUMP_CNU_MOD:
		{
			strcpy(log.cmd, "CMM_DEBUG_DUMP_CNU_MOD");
			break;
		}
		case CMM_DUMP_CNU_PIB:
		{
			strcpy(log.cmd, "CMM_DEBUG_DUMP_CNU_PIB");
			break;
		}
		case CMM_DO_WDT_CONTROL:
		{
			strcpy(log.cmd, "CMM_DO_WDT_CONTROL");
			break;
		}
		case CMM_UNDO_WDT_CONTROL:
		{
			strcpy(log.cmd, "CMM_UNDO_WDT_CONTROL");
			break;
		}
		case CMM_GET_CBAT_TEMPERATURE:
		{
			strcpy(log.cmd, "CMM_GET_CBAT_TEMPERATURE");
			break;
		}
		case CMM_GET_DSDT_RGMII_DELAY:
		{
			strcpy(log.cmd, "CMM_GET_DSDT_RGMII_DELAY");
			break;
		}
		case CMM_SET_DSDT_RGMII_DELAY:
		{
			strcpy(log.cmd, "CMM_SET_DSDT_RGMII_DELAY");
			break;
		}
		case CMM_CNU_SWITCH_READ:
		{
			strcpy(log.cmd, "read cnu switch register");
			break;
		}
		case CMM_CNU_SWITCH_WRITE:
		{
			strcpy(log.cmd, "write cnu switch register");
			break;
		}
		case CMM_DO_CNU_ACL_DROP_MME:
		{
			strcpy(log.cmd, "CMM_DO_CNU_ACL_DROP_MME");
			break;
		}
		case CMM_UNDO_CNU_ACL_DROP_MME:
		{
			strcpy(log.cmd, "CMM_UNDO_CNU_ACL_DROP_MME");
			break;
		}
		case CMM_ERASE_MOD_A:
		{
			strcpy(log.cmd, "CMM_ERASE_MOD_1");
			break;
		}
		case CMM_ERASE_MOD_B:
		{
			strcpy(log.cmd, "CMM_ERASE_MOD_2");
			break;
		}
		default:
		{
			strcpy(log.cmd, "CMM_UNKONEN_CMD");
			break;
		}
	}

	/* 将日志写入数据库*/
	return dbs_opt_log(dbsdev, &log);

}

int CMM_ProcessModuleDebugCtl(BBLOCK_QUEUE *this)
{
	assert( NULL != this );
	
	int opt_sts = CMM_SUCCESS;
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	st_ModuleDebugCtl *pDebugClt = (st_ModuleDebugCtl *)(msg->BUF);

	switch(pDebugClt->usMID)
	{
		case MID_CMM:
		{
			CMM_MODULE_DEBUG_ENABLE = pDebugClt->enable;
			break;
		}
		case MID_DBS:
		{
			opt_sts = dbsMsgDebug(dbsdev, pDebugClt->enable);
			break;
		}
		case MID_SQL:
		{
			opt_sts = dbsSQLDebug(dbsdev, pDebugClt->enable);
			break;
		}
		default:
		{
			opt_sts = CMM_FAILED;
			break;
		}
	}
	
	/* 将处理信息发送给请求者*/
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return CMM_SUCCESS;
}

int CMM_ProcessRestoreDefault(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	/* 设置系统指示灯忙状态*/
	cmm2sysmonitor_sysledCtrol(SYSLED_STS_BUSY);
	/* 需要发送告警*/
	cmm2alarm_sendCbatResetNotification();
	/* 删除文件系统中的配置*/
	system("rm -f /usr/mnt/config/databases/*");	
	sleep(3);
	/* 设置系统指示灯系统停止运行状态*/
	cmm2sysmonitor_sysledCtrol(SYSLED_STS_RESET);
	system("reboot");
	return opt_sts;
}

int CMM_ProcessCbatReset(BBLOCK_QUEUE *this)
{
	CMM_ProcessAck(CMM_SUCCESS, this, NULL, 0);
	cmm2alarm_sendCbatResetNotification();
	/* 设置系统指示灯系统停止运行状态*/
	cmm2sysmonitor_sysledCtrol(SYSLED_STS_RESET);
	/* 需要发送告警*/
	sleep(1);
    	system("reboot");	
	return CMM_SUCCESS;
}

int CMM_ProcessSendHeartbeat(BBLOCK_QUEUE *this)
{
	CMM_ProcessAck(CMM_SUCCESS, this, NULL, 0);
	cmm2alarm_sendCbatTopHbTrap();
	return CMM_SUCCESS;
}

int CMM_ProcessHeartbeatCtrl(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);

	if( CMM_DO_HB_TRAP_CTRL == req->HEADER.usMsgType )
	{
		cmm2alarm_heartbeatEnable(1);
	}
	else if( CMM_UNDO_HB_TRAP_CTRL == req->HEADER.usMsgType )
	{
		cmm2alarm_heartbeatEnable(0);
	}
	else
	{
		opt_sts = CMM_FAILED;
	}	
	
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessCltReset(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);	
	uint16_t *index = (uint16_t *)(req->BUF);
	st_dbsClt clt;
	stRegEvent registerEvent;

	registerEvent.clt = *index;
	registerEvent.cnu = 0;
	registerEvent.event = REG_CLT_RESET;

	//printf("\r\n\r\n  CMM_ProcessCtlReset(%d)\n", registerEvent.clt);
	
	if( (registerEvent.clt < 1) ||(registerEvent.clt > MAX_CLT_AMOUNT_LIMIT))
	{
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != dbsGetClt(dbsdev, registerEvent.clt, &clt))
	{
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != clt.col_sts)||(BOOL_TRUE != clt.col_row_sts))
	{
		opt_sts = CMM_SUCCESS;
	}
	else
	{
		/* 发送MME 将CLT 重启*/
		send_notification_to_reg(&registerEvent);
		opt_sts = CMM_SUCCESS;
	}

	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessCnuReset(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);	
	uint16_t *index = (uint16_t *)(req->BUF);
	st_dbsCnu cnu;
	stRegEvent registerEvent;

	registerEvent.clt = (*index-1)/MAX_CNUS_PER_CLT + 1 ;
	registerEvent.cnu = *index;
	registerEvent.event = REG_CNURESET;

	//printf("\r\n\r\n  CMM_ProcessResetCnu(%d)\n", registerEvent.cnu);

	if( (registerEvent.cnu < 1)||(registerEvent.cnu > MAX_CNU_AMOUNT_LIMIT))
	{
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != dbsGetCnu(dbsdev, registerEvent.cnu, &cnu) )
	{
		opt_sts = CMM_FAILED;
	}	
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		opt_sts = CMM_SUCCESS;
	}
	else 
	{
		opt_sts = CMM_SUCCESS;
		send_notification_to_reg(&registerEvent);		
	}

	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessCnuVlanConfig(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	int cltid;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	st_dbsProfile *req_data = (st_dbsProfile *)(req->BUF);

	//printf("\r\n\r\n  CMM_ProcessCnuVlanConfig(%d)\n", req_data->id);
	if (req_data->id < 65)
	{
		cltid = 1;
	}
	else if (req_data->id > 64 && req_data->id < 129)
	{
		cltid = 2;
	}
	else if (req_data->id > 128 && req_data->id <193)
	{
		cltid = 3;
	}
	else
	{
		cltid = 4;
	}
	
	opt_sts = cmmTmWriteCnuProfile(cltid, req_data->id, req_data);
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessCnuRateLimit(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	int cltid;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	st_dbsProfile *req_data = (st_dbsProfile *)(req->BUF);

	//printf("\r\n\r\n  CMM_ProcessCnuRateLimit(%d)\n", req_data->id);
	if (req_data->id < 65)
	{
		cltid = 1;
	}
	else if (req_data->id > 64 && req_data->id < 129)
	{
		cltid = 2;
	}
	else if (req_data->id > 128 && req_data->id <193)
	{
		cltid = 3;
	}
	else
	{
		cltid = 4;
	}
	
	opt_sts = cmmTmWriteCnuProfile(cltid, req_data->id, req_data);
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessCnuStormFilter(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	int cltid;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	st_dbsProfile *req_data = (st_dbsProfile *)(req->BUF);

	//printf("\r\n\r\n  CMM_ProcessCnuStormFilter(%d)\n", req_data->id);
	if (req_data->id < 65)
	{
		cltid = 1;
	}
	else if (req_data->id > 64 && req_data->id < 129)
	{
		cltid = 2;
	}
	else if (req_data->id > 128 && req_data->id <193)
	{
		cltid = 3;
	}
	else
	{
		cltid = 4;
	}
	
	opt_sts = cmmTmWriteCnuProfile(cltid, req_data->id, req_data);
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessCnuPortStatusConfig(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	int cltid;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	st_dbsProfile *req_data = (st_dbsProfile *)(req->BUF);

	//printf("\r\n\r\n  CMM_ProcessCnuPortStatusConfig(%d)\n", req_data->id);
	if (req_data->id < 65)
	{
		cltid = 1;
	}
	else if (req_data->id > 64 && req_data->id < 129)
	{
		cltid = 2;
	}
	else if (req_data->id > 128 && req_data->id <193)
	{
		cltid = 3;
	}
	else
	{
		cltid = 4;
	}
	
	opt_sts = cmmTmWriteCnuProfile(cltid, req_data->id, req_data);
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessCnuMacLimitConfig(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	int cltid;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	st_dbsProfile *req_data = (st_dbsProfile *)(req->BUF);

	if (req_data->id < 65)
	{
		cltid = 1;
	}
	else if (req_data->id > 64 && req_data->id < 129)
	{
		cltid = 2;
	}
	else if (req_data->id > 128 && req_data->id <193)
	{
		cltid = 3;
	}
	else
	{
		cltid = 4;
	}

	opt_sts = cmmTmWriteCnuProfile(cltid, req_data->id, req_data);
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessMmeMdioPhyRead(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	st_dbsClt clt;
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	T_szAr8236Phy *smiPhy = (T_szAr8236Phy *)(msg->BUF);
	T_szAr8236Phy cli_ar8236_phy;
	T_szMdioPhy ar8236_phy;
	uint8_t bMac[6] = {0};

	cli_ar8236_phy.clt = smiPhy->clt;
	cli_ar8236_phy.cnu = smiPhy->cnu;
	cli_ar8236_phy.mdioPhy.phy = smiPhy->mdioPhy.phy;
	cli_ar8236_phy.mdioPhy.reg = smiPhy->mdioPhy.reg;
	cli_ar8236_phy.mdioPhy.value = 0x0000;

	if( 0 != cli_ar8236_phy.cnu )
	{
		opt_sts = CMM_FAILED;
	}
	else if( (smiPhy->clt<1) ||(smiPhy->clt>MAX_CLT_AMOUNT_LIMIT))
	{
		opt_sts = CMM_FAILED;
	}	
	else if( CMM_SUCCESS != dbsGetClt(dbsdev, smiPhy->clt, &clt) )
	{
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != clt.col_sts)||BOOL_TRUE != clt.col_row_sts )
	{
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(clt.col_mac, bMac) )
	{
		opt_sts = CMM_FAILED;
	}	
	else
	{
		memcpy(&ar8236_phy, &(smiPhy->mdioPhy), sizeof(T_szMdioPhy));
		
		/* 从MMEAD获取信息*/	
		if( CMM_SUCCESS == mmead_get_ar8236_phy(bMac, &ar8236_phy))
		{
			cli_ar8236_phy.mdioPhy.value = ar8236_phy.value;
			opt_sts = CMM_SUCCESS;
		}
		else
		{
			opt_sts = CMM_FAILED;
		}
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&cli_ar8236_phy, sizeof(T_szAr8236Phy));

	return opt_sts;
}

int CMM_ProcessMmeMdioPhyWrite(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	st_dbsClt clt;
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	T_szAr8236Phy *smiPhy = (T_szAr8236Phy *)(msg->BUF);
	T_szAr8236Phy cli_ar8236_phy;
	T_szMdioPhy ar8236_phy;
	uint8_t bMac[6] = {0};

	cli_ar8236_phy.clt = smiPhy->clt;
	cli_ar8236_phy.cnu = smiPhy->cnu;
	cli_ar8236_phy.mdioPhy.phy = smiPhy->mdioPhy.phy;
	cli_ar8236_phy.mdioPhy.reg = smiPhy->mdioPhy.reg;
	cli_ar8236_phy.mdioPhy.value = smiPhy->mdioPhy.value;

	if( 0 != cli_ar8236_phy.cnu )
	{
		opt_sts = CMM_FAILED;
	}
	else if( (smiPhy->clt<1) ||(smiPhy->clt>MAX_CLT_AMOUNT_LIMIT))
	{
		opt_sts = CMM_FAILED;
	}	
	else if( CMM_SUCCESS != dbsGetClt(dbsdev, smiPhy->clt, &clt) )
	{
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != clt.col_sts)||BOOL_TRUE != clt.col_row_sts )
	{
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(clt.col_mac, bMac) )
	{
		opt_sts = CMM_FAILED;
	}	
	else
	{
		memcpy(&ar8236_phy, &(smiPhy->mdioPhy), sizeof(T_szMdioPhy));
		/* 从MMEAD获取信息*/
		opt_sts = mmead_set_ar8236_phy(bMac, &ar8236_phy);		
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&cli_ar8236_phy, sizeof(T_szAr8236Phy));
	return opt_sts;
}


int CMM_ProcessReadAr8236Phy(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	T_szAr8236Phy *smiPhy = (T_szAr8236Phy *)(msg->BUF);
	T_szAr8236Phy cli_ar8236_phy;
	T_szMdioPhy ar8236_phy;
	uint8_t bMac[6] = {0};

	cli_ar8236_phy.clt = smiPhy->clt;
	cli_ar8236_phy.cnu = smiPhy->cnu;
	cli_ar8236_phy.mdioPhy.phy = smiPhy->mdioPhy.phy;
	cli_ar8236_phy.mdioPhy.reg = smiPhy->mdioPhy.reg;
	cli_ar8236_phy.mdioPhy.value = 0x0000;

	if( (smiPhy->cnu<1)||(smiPhy->cnu > MAX_CNU_AMOUNT_LIMIT))
	{
		opt_sts = CMM_FAILED;
	}
	else if( (smiPhy->clt<1) ||(smiPhy->clt>MAX_CLT_AMOUNT_LIMIT))
	{
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != dbsGetCnu(dbsdev, smiPhy->cnu, &cnu) )
	{
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		opt_sts = CMM_FAILED;
	}
	else if( !boardapi_isCnuSupported(cnu.col_model) )
	{
		opt_sts = CMM_FAILED;
	}
	else
	{
		memcpy(&ar8236_phy, &(smiPhy->mdioPhy), sizeof(T_szMdioPhy));
		
		/* 从MMEAD获取信息*/	
		if( CMM_SUCCESS == mmead_get_ar8236_phy(bMac, &ar8236_phy))
		{
			cli_ar8236_phy.mdioPhy.value = ar8236_phy.value;
			opt_sts = CMM_SUCCESS;
		}
		else
		{
			opt_sts = CMM_FAILED;
		}
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&cli_ar8236_phy, sizeof(T_szAr8236Phy));

	return opt_sts;
}

int CMM_ProcessWriteAr8236Phy(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	T_szAr8236Phy *smiPhy = (T_szAr8236Phy *)(msg->BUF);
	T_szAr8236Phy cli_ar8236_phy;
	T_szMdioPhy ar8236_phy;
	uint8_t bMac[6] = {0};

	cli_ar8236_phy.clt = smiPhy->clt;
	cli_ar8236_phy.cnu = smiPhy->cnu;
	cli_ar8236_phy.mdioPhy.phy = smiPhy->mdioPhy.phy;
	cli_ar8236_phy.mdioPhy.reg = smiPhy->mdioPhy.reg;
	cli_ar8236_phy.mdioPhy.value = smiPhy->mdioPhy.value;

	if( CMM_SUCCESS != dbsGetCnu(dbsdev, smiPhy->cnu, &cnu) )
	{
		opt_sts = CMM_DB_GETCNU_ERROR;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		opt_sts = CMM_FAILED;
	}
	else if( !boardapi_isCnuSupported(cnu.col_model) )
	{
		opt_sts = CMM_FAILED;
	}
	else
	{
		memcpy(&ar8236_phy, &(smiPhy->mdioPhy), sizeof(T_szMdioPhy));
		/* 从MMEAD获取信息*/
		opt_sts = mmead_set_ar8236_phy(bMac, &ar8236_phy);		
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&cli_ar8236_phy, sizeof(T_szAr8236Phy));
	return opt_sts;
}

int CMM_ProcessCnuSwitchConfigRead(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	uint8_t bMac[6] = {0};
	
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);
	
	st_rtl8306eSettings ack_data;

	if( (req_data->cnu<1)||(req_data->cnu > MAX_CNU_AMOUNT_LIMIT))
	{
		printf("\n#ERROR[01]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != dbsGetCnu(dbsdev, req_data->cnu, &cnu) )
	{
		printf("\n#ERROR[02]\n");
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		printf("\n#ERROR[03]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		printf("\n#ERROR[04]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != mmead_get_rtl8306e_configs(bMac, &ack_data) )
	{
		printf("\n#ERROR[05]\n");
		opt_sts = CMM_FAILED;
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&ack_data, sizeof(st_rtl8306eSettings));

	return opt_sts;
	
}

int CMM_ProcessCnuSwitchPortStatusRead(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	uint8_t bMac[6] = {0};
	st_dbsCnu cnu;
	
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);
	
	st_rtl8306e_port_status ack_data;

	 if( CMM_SUCCESS != dbsGetCnu(dbsdev, req_data->cnu, &cnu) )
	{
		printf("\n#ERROR[02]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		printf("\n#ERROR[04]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != mmead_get_rtl8306e_port_status(bMac, &ack_data) )
	{
		printf("\n#ERROR[05]\n");
		opt_sts = CMM_FAILED;
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&ack_data, sizeof(st_rtl8306e_port_status));

	return opt_sts;
}

int CMM_ProcessCltFrequencyBandRead(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	uint8_t bMac[6] = {0};
	st_dbsClt clt;
	
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);
	
	st_cltFreq ack_data;

	 if( CMM_SUCCESS != dbsGetClt(dbsdev, req_data->clt, &clt) )
	{
		printf("\n#ERROR[02]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(clt.col_mac, bMac) )
	{
		printf("\n#ERROR[04]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != mmead_get_clt_frequency_band(bMac, &ack_data) )
	{
		printf("\n#ERROR[05]\n");
		opt_sts = CMM_FAILED;
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&ack_data, sizeof(st_cltFreq));

	return opt_sts;

}

int CMM_ProcessCltFrequencyBandWrite(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	uint8_t bMac[6] = {0};
	st_dbsClt clt;

	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	st_cltFreqReq *req_data = (st_cltFreqReq *)(req->BUF);
	
	if( CMM_SUCCESS != dbsGetClt(dbsdev, req_data->cltid, &clt) )
	{
		printf("\n#ERROR[02]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(clt.col_mac, bMac) )
	{
		printf("\n#ERROR[04]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != mmead_set_clt_frequency_band(bMac, req_data) )
	{
		printf("\n#ERROR[05]\n");
		opt_sts = CMM_FAILED;
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
	
}

int CMM_ProcessCnuSwitchConfigWrite(BBLOCK_QUEUE *this)
{
	uint32_t len = 0;
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	uint8_t bMac[6] = {0};
	uint8_t mod[1024] = {0};
	int cnuId;
	
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	rtl8306eWriteInfo *req_data = (rtl8306eWriteInfo *)(req->BUF);

	//add bys tan for savedb
	st_dbsProfile *req_data_db = (st_dbsProfile *)(req->BUF + sizeof(rtl8306eWriteInfo));

	cnuId = (req_data_db->id - 1) %MAX_CNUS_PER_CLT + 1;
	opt_sts = cmmTmWriteCnuProfile(req_data->node.clt, cnuId, req_data_db);
	//CMM_ProcessAck(opt_sts, this, NULL, 0);
	//return opt_sts;
	//add by stan for savedb
	if(CMM_SUCCESS != opt_sts){
		opt_sts = CMM_FAILED;
		printf("\n----------tmp write cnu profile to dbs error\n");
		return opt_sts;
	}
	
	//st_rtl8306eSettings ack_data;
	
	if( (req_data->node.cnu<1)||(req_data->node.cnu > MAX_CNU_AMOUNT_LIMIT))
	{
		printf("\n#ERROR[01]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != dbsGetCnu(dbsdev, req_data->node.cnu, &cnu) )
	{
		printf("\n#ERROR[02]\n");
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		printf("\n#ERROR[03]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		printf("\n#ERROR[04]\n");
		opt_sts = CMM_FAILED;
	}
	
	//gen mod
	len = rtl8306e_gen_mod(&req_data->rtl8306eConfig, mod);
	if( len == 0 )
	{
		printf("\n#ERROR[05]\n");
		opt_sts = CMM_FAILED;
	}
	
	//**************send to mmead*************//
	if( CMM_SUCCESS != mmead_write_rtl8306e_mod(bMac, mod, len) )
	{
		opt_sts = CMM_FAILED;
	}	

	
	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, NULL, 0);

	return opt_sts;
	
}

int CMM_ProcessCnuEraseModa(BBLOCK_QUEUE *this)
{
	uint32_t cnuid = 0;	
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	uint8_t bMac[6] = {0};
	T_MMEAD_ERASE_MOD_REQ_INFO erase;
	
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	erase.MODULE_ID = 0x1000;
	erase.MODULE_SUB_ID = 0;

	cnuid = (req_data->clt - 1)*MAX_CNUS_PER_CLT + req_data->cnu;
	
	//st_rtl8306eSettings ack_data;
	if( (req_data->clt<1)||(req_data->clt > MAX_CLT_AMOUNT_LIMIT))
	{
		printf("\n#ERROR[00]\n");
		opt_sts = CMM_FAILED;
	}
	if( (req_data->cnu<1)||(req_data->cnu > MAX_CNUS_PER_CLT))
	{
		printf("\n#ERROR[01]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != dbsGetCnu(dbsdev, cnuid, &cnu) )
	{
		printf("\n#ERROR[02]\n");
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		printf("\n#ERROR[03]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		printf("\n#ERROR[04]\n");
		opt_sts = CMM_FAILED;
	}

	//**************send to mmead*************//
	if( CMM_SUCCESS != mmead_erase_mod(bMac, &erase) )
	{
		opt_sts = CMM_FAILED;
	}	

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, NULL, 0);

	return opt_sts;
	
}

int CMM_ProcessCnuEraseModb(BBLOCK_QUEUE *this)
{
	uint32_t cnuid = 0;	
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	uint8_t bMac[6] = {0};
	T_MMEAD_ERASE_MOD_REQ_INFO erase;
	
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	erase.MODULE_ID = 0x1000;
	erase.MODULE_SUB_ID = 1;

	cnuid = (req_data->clt - 1)*MAX_CNUS_PER_CLT + req_data->cnu;
	
	//st_rtl8306eSettings ack_data;
	if( (req_data->clt<1)||(req_data->clt > MAX_CLT_AMOUNT_LIMIT))
	{
		printf("\n#ERROR[00]\n");
		opt_sts = CMM_FAILED;
	}
	if( (req_data->cnu<1)||(req_data->cnu > MAX_CNUS_PER_CLT))
	{
		printf("\n#ERROR[01]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != dbsGetCnu(dbsdev, cnuid, &cnu) )
	{
		printf("\n#ERROR[02]\n");
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		printf("\n#ERROR[03]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		printf("\n#ERROR[04]\n");
		opt_sts = CMM_FAILED;
	}

	//**************send to mmead*************//
	if( CMM_SUCCESS != mmead_erase_mod(bMac, &erase) )
	{
		opt_sts = CMM_FAILED;
	}	

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, NULL, 0);

	return opt_sts;
	
}

int CMM_ProcessCnuSwitchRead(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	uint8_t bMac[6] = {0};
	
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	T_szSwRtl8306eConfig *req_data = (T_szSwRtl8306eConfig *)(req->BUF);
	
	T_szSwRtl8306eConfig ack_data;
	//T_szMdioPhy ar8236_phy;
	

	ack_data.clt = req_data->clt;
	ack_data.cnu = req_data->cnu;
	ack_data.mdioInfo.phy = req_data->mdioInfo.phy;
	ack_data.mdioInfo.reg = req_data->mdioInfo.reg;
	ack_data.mdioInfo.page = req_data->mdioInfo.page;
	ack_data.mdioInfo.value = 0x0000;
	//printf("\nRead phy %d register %d page %d\n", ack_data.mdioInfo.phy, ack_data.mdioInfo.reg, ack_data.mdioInfo.page);

	if( (ack_data.cnu<1)||(ack_data.cnu > MAX_CNU_AMOUNT_LIMIT))
	{
		printf("\n#ERROR[01]\n");
		opt_sts = CMM_FAILED;
	}	
	else if( CMM_SUCCESS != dbsGetCnu(dbsdev, ack_data.cnu, &cnu) )
	{
		printf("\n#ERROR[02]\n");
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		printf("\n#ERROR[03]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		printf("\n#ERROR[04]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != mmead_get_rtl8306e_register(bMac, &ack_data) )
	{
		printf("\n#ERROR[05]\n");
		opt_sts = CMM_FAILED;
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&ack_data, sizeof(T_szSwRtl8306eConfig));

	return opt_sts;
}

int CMM_ProcessCnuSwitchWrite(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	uint8_t bMac[6] = {0};
	
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	T_szSwRtl8306eConfig *req_data = (T_szSwRtl8306eConfig *)(req->BUF);
	
	if( (req_data->cnu < 1)||(req_data->cnu > MAX_CNU_AMOUNT_LIMIT))
	{
		opt_sts = CMM_FAILED;
	}	
	else if( CMM_SUCCESS != dbsGetCnu(dbsdev, req_data->cnu, &cnu) )
	{
		opt_sts = CMM_DB_GETCNU_ERROR;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		opt_sts = CMM_FAILED;
	}
	else
	{
		/* 从MMEAD获取信息*/
		opt_sts = mmead_set_rtl8306e_register(bMac, req_data);		
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessReadAr8236Reg(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	T_szAr8236Reg *smiReg = (T_szAr8236Reg *)(msg->BUF);	
	T_szAr8236Reg cli_ar8236_reg;
	T_szMdioSw ar8236_reg;
	uint8_t bMac[6] = {0};

	cli_ar8236_reg.clt = smiReg->clt;
	cli_ar8236_reg.cnu = smiReg->cnu;
	cli_ar8236_reg.mdioReg.reg = smiReg->mdioReg.reg;
	cli_ar8236_reg.mdioReg.value = 0x0000;

	if( CMM_SUCCESS != dbsGetCnu(dbsdev, smiReg->cnu, &cnu) )
	{
		opt_sts = CMM_DB_GETCNU_ERROR;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		opt_sts = CMM_FAILED;
	}
	else if( !boardapi_isCnuSupported(cnu.col_model) )
	{
		opt_sts = CMM_FAILED;
	}
	else
	{
		memcpy(&ar8236_reg, &(smiReg->mdioReg), sizeof(T_szMdioSw));
		/* 从MMEAD获取信息*/	
		if( CMM_SUCCESS == mmead_get_ar8236_reg(bMac, &ar8236_reg))
		{
			cli_ar8236_reg.mdioReg.value = ar8236_reg.value;
			opt_sts = CMM_SUCCESS;
		}
		else
		{
			opt_sts = CMM_FAILED;
		}
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&cli_ar8236_reg, sizeof(T_szAr8236Reg));
	return opt_sts;
}

int CMM_ProcessReadUserHFID(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	T_szCnuUserHFID *req_data = (T_szCnuUserHFID *)(req->BUF);	

	T_szCnuUserHFID ack_data;

	memcpy(ack_data.ODA,  req_data->ODA, 6);
	memset(ack_data.pdata,0,sizeof(uint8_t)*64);
	
	if ( CMM_SUCCESS != mmead_get_user_hfid(&ack_data))
	{
		printf("cmm_mmead get user_hfid error\n");
		opt_sts = CMM_FAILED;
	}
	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&ack_data, sizeof(T_szCnuUserHFID));

	return opt_sts;
}

int CMM_ProcessReadHgManage(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	uint8_t bMac[6] = {0};
	
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	T_szHgManageConfig  *req_data = (T_szHgManageConfig *)(req->BUF);
	
	T_szHgManage ack_data;

	if( (req_data->cnu<1)||(req_data->cnu > MAX_CNU_AMOUNT_LIMIT))
	{
		printf("\n#ERROR[01]\n");
		opt_sts = CMM_FAILED;
	}	
	else if( CMM_SUCCESS != dbsGetCnu(dbsdev, req_data->cnu, &cnu) )
	{
		printf("\n#ERROR[02]\n");
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		printf("\n#ERROR[03]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		printf("\n#ERROR[04]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != mmead_get_Hg_Manage(bMac, &ack_data) )
	{
		printf("\n#ERROR[05]\n");
		opt_sts = CMM_FAILED;
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&ack_data, sizeof(T_szHgManage));

	return opt_sts;

}

int CMM_ProcessReadHgBusiness(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	uint8_t bMac[6] = {0};
	
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	T_szHgBusinessConfig  *req_data = (T_szHgBusinessConfig *)(req->BUF);
	
	T_szHgBusiness ack_data;

	if( (req_data->cnu<1)||(req_data->cnu > MAX_CNU_AMOUNT_LIMIT))
	{
		printf("\n#ERROR[01]\n");
		opt_sts = CMM_FAILED;
	}	
	else if( CMM_SUCCESS != dbsGetCnu(dbsdev, req_data->cnu, &cnu) )
	{
		printf("\n#ERROR[02]\n");
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		printf("\n#ERROR[03]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		printf("\n#ERROR[04]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != mmead_get_Hg_Business(bMac, &ack_data) )
	{
		printf("\n#ERROR[05]\n");
		opt_sts = CMM_FAILED;
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&ack_data, sizeof(T_szHgBusiness));

	return opt_sts;
}

int CMM_ProcessWriteHgManage(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	uint8_t bMac[6] = {0};
	
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	T_szHgManageConfig  *req_data = (T_szHgManageConfig *)(req->BUF);
	
	if( (req_data->cnu<1)||(req_data->cnu > MAX_CNU_AMOUNT_LIMIT))
	{
		printf("\n#ERROR[01]\n");
		opt_sts = CMM_FAILED;
	}	
	else if( CMM_SUCCESS != dbsGetCnu(dbsdev, req_data->cnu, &cnu) )
	{
		printf("\n#ERROR[02]\n");
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		printf("\n#ERROR[03]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		printf("\n#ERROR[04]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != mmead_set_Hg_Manage(bMac, req_data) )
	{
		printf("\n#ERROR[05]\n");
		opt_sts = CMM_FAILED;
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, NULL, 0);

	return opt_sts;
}

int CMM_ProcessWriteHgBusiness(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	uint8_t bMac[6] = {0};
	
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	T_szSetHgBusinessConfig  *req_data = (T_szSetHgBusinessConfig *)(req->BUF);
	
	if( (req_data->cnu<1)||(req_data->cnu > MAX_CNU_AMOUNT_LIMIT))
	{
		printf("\n#ERROR[01]\n");
		opt_sts = CMM_FAILED;
	}	
	else if( CMM_SUCCESS != dbsGetCnu(dbsdev, req_data->cnu, &cnu) )
	{
		printf("\n#ERROR[02]\n");
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		printf("\n#ERROR[03]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		printf("\n#ERROR[04]\n");
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != mmead_set_Hg_Business(bMac, req_data) )
	{
		printf("\n#ERROR[05]\n");
		opt_sts = CMM_FAILED;
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, NULL, 0);

	return opt_sts;
}


int CMM_ProcessWriteAr8236Reg(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	T_szAr8236Reg *smiReg = (T_szAr8236Reg *)(msg->BUF);	
	T_szAr8236Reg cli_ar8236_reg;
	T_szMdioSw ar8236_reg;
	uint8_t bMac[6] = {0};

	cli_ar8236_reg.clt = smiReg->clt;
	cli_ar8236_reg.cnu = smiReg->cnu;
	cli_ar8236_reg.mdioReg.reg = smiReg->mdioReg.reg;
	cli_ar8236_reg.mdioReg.value = smiReg->mdioReg.value;

	if( CMM_SUCCESS != dbsGetCnu(dbsdev, smiReg->cnu, &cnu) )
	{
		opt_sts = CMM_DB_GETCNU_ERROR;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		opt_sts = CMM_FAILED;
	}
	else if( !boardapi_isCnuSupported(cnu.col_model) )
	{
		opt_sts = CMM_FAILED;
	}
	else
	{
		memcpy(&ar8236_reg, &(smiReg->mdioReg), sizeof(T_szMdioSw));
		/* 从MMEAD获取信息*/
		opt_sts = mmead_set_ar8236_reg(bMac, &ar8236_reg);
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&cli_ar8236_reg, sizeof(T_szAr8236Reg));
	return opt_sts;
}

int CMM_ProcessLinkDiag(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	int tblCnuRid = 0;
	st_dbsCnu cnu;
	st_dbsClt clt;
	uint8_t oda[6] = {0};
	
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	T_CMM_LINK_DIAG_INFO *msg_data = (T_CMM_LINK_DIAG_INFO *)(msg->BUF);

	T_MMEAD_LINK_DIAG_INFO inputInfo;
	T_MMEAD_LINK_DIAG_RESULT outputInfo;	

	tblCnuRid = (msg_data->clt - 1)*MAX_CNUS_PER_CLT + msg_data->cnu;
		
	/* RX/TX */
	if( msg_data->dir > 1 )
	{
		opt_sts = CMM_FAILED;
		/* 将处理信息发送给请求者 */
		CMM_ProcessAck(opt_sts, this, NULL, 0);
		return opt_sts;
	}

	/* Get CCo MAC address */
	if( CMM_SUCCESS == dbsGetClt(dbsdev, msg_data->clt, &clt) )
	{
		inputInfo.dir = msg_data->dir;
		if( CMM_SUCCESS != boardapi_macs2b(clt.col_mac, inputInfo.ccoMac) )
		{
			opt_sts = CMM_FAILED;
			/* 将处理信息发送给请求者 */
			CMM_ProcessAck(opt_sts, this, NULL, 0);
			return opt_sts;
		}		
		memcpy(inputInfo.peerNodeMac, inputInfo.ccoMac, 6);
	}
	else
	{
		opt_sts = CMM_FAILED;
		/* 将处理信息发送给请求者 */
		CMM_ProcessAck(opt_sts, this, NULL, 0);
		return opt_sts;
	}

	/* CNU 不在线禁止诊断*/
	if( CMM_SUCCESS == dbsGetCnu(dbsdev, tblCnuRid, &cnu) )
	{
		if( DEV_STS_ONLINE != cnu.col_sts )
		{
			opt_sts = CMM_FAILED;
			/* 将处理信息发送给请求者 */
			CMM_ProcessAck(opt_sts, this, NULL, 0);
			return opt_sts;
		}
		printf("cnu.col_model=%d\n",cnu.col_model);
		switch( cnu.col_model )
		{
			case WEC_3702I:
				inputInfo.chipser = 0;
				break;
			case WEC_604:
				inputInfo.chipser = 0;
				break;
			case WEC_3702I_E4:
				inputInfo.chipser = 0;
				break;
			case WEC701_C4:
				inputInfo.chipser = 1;
				break;
			case WEC701_E4:
				inputInfo.chipser = 1;
				break;
			case WEC701_L4:
				inputInfo.chipser = 1;
				break;
			case WEC701_W4: 
				inputInfo.chipser = 1;
				break;
			case WEC701W_C4:
				inputInfo.chipser = 1;
				break;	
			default:
				inputInfo.chipser = 1;
				break;
		}
	}
	else
	{
		opt_sts = CMM_FAILED;
		/* 将处理信息发送给请求者 */
		CMM_ProcessAck(opt_sts, this, NULL, 0);
		return opt_sts;
	}

	if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, oda) )
	{
		opt_sts = CMM_FAILED;
		/* 将处理信息发送给请求者 */
		CMM_ProcessAck(opt_sts, this, NULL, 0);
		return opt_sts;
	}
	
	if( CMM_SUCCESS != mmead_do_link_diag(oda, &inputInfo, &outputInfo) )
	{
		opt_sts = CMM_FAILED;
		/* 将处理信息发送给请求者 */
		CMM_ProcessAck(opt_sts, this, NULL, 0);
		return opt_sts;
	}
	else
	{
		opt_sts = CMM_SUCCESS;
		outputInfo.model = cnu.col_model;
		/* 将处理信息发送给请求者 */
		CMM_ProcessAck(opt_sts, this, (uint8_t *)&outputInfo, sizeof(T_MMEAD_LINK_DIAG_RESULT));
		return opt_sts;
	}
}

int CMM_ProcessGetPortPropety(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	uint32_t *msg_data = (uint32_t *)(msg->BUF);

	T_CMM_PORT_PROPETY_INFO ack_data;
	int portid = *msg_data;

	if( ( portid < 0 )||(portid > 6) )
	{
		opt_sts = CMM_FAILED;
		/* 将处理信息发送给请求者 */
		CMM_ProcessAck(opt_sts, this, NULL, 0);
		return opt_sts;
	}

	ack_data.linkStatus = cmm2dsdt_getPortLinkStatus(portid);
	ack_data.speed = cmm2dsdt_getPortLinkSpeed(portid);
	ack_data.duplex = cmm2dsdt_getPortLinkDuplex(portid);
	ack_data.pri = cmm2dsdt_getPortPri(portid);
	ack_data.flowControl = cmm2dsdt_getPortFlowControl(portid);
	ack_data.portStatus = cmm2dsdt_getPortState(portid);	

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&ack_data, sizeof(T_CMM_PORT_PROPETY_INFO));
	return opt_sts;
}

int CMM_ProcessGetPortPropetyAll(BBLOCK_QUEUE *this)
{
	int i = 0;

	T_CMM_PORT_PROPETY_INFO ack_data[CBAT_SW_PORT_NUM];

	for( i=0; i<CBAT_SW_PORT_NUM; i++ )
	{
		ack_data[i].linkStatus = cmm2dsdt_getPortLinkStatus(i);
		ack_data[i].speed = cmm2dsdt_getPortLinkSpeed(i);
		ack_data[i].duplex = cmm2dsdt_getPortLinkDuplex(i);
		ack_data[i].pri = cmm2dsdt_getPortPri(i);
		ack_data[i].flowControl = cmm2dsdt_getPortFlowControl(i);
		ack_data[i].portStatus = cmm2dsdt_getPortState(i);
	}		

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(CMM_SUCCESS, this, (uint8_t *)&ack_data, sizeof(T_CMM_PORT_PROPETY_INFO)*CBAT_SW_PORT_NUM);
	return CMM_SUCCESS;
}

int CMM_ProcessGetCltPortLinkSts(BBLOCK_QUEUE *this)
{
	//int opt_sts = CMM_SUCCESS;
	uint32_t cltid = 0;
	uint32_t portid = 0;
	uint32_t linkStatus = 0;

	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	uint32_t *req_data = (uint32_t *)(req->BUF);
	cltid = *req_data;

	portid = boardapi_getCltDsdtPortid(cltid);
	linkStatus = cmm2dsdt_getPortLinkStatus(portid);

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(CMM_SUCCESS, this, (uint8_t *)&linkStatus, sizeof(linkStatus));
	return CMM_SUCCESS;
}

int CMM_ProcessGetDsdtRgmiiDelay(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;	

	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	st_dsdtRgmiiTimingDelay *msg_data = (st_dsdtRgmiiTimingDelay *)(msg->BUF);
	
	st_dsdtRgmiiTimingDelay ack_data;
	ack_data.port = msg_data->port;

	opt_sts = cmm2dsdt_getRgmiiTimingDelay(&ack_data);	

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&ack_data, sizeof(st_dsdtRgmiiTimingDelay));
	return opt_sts;
}

int CMM_ProcessSetDsdtRgmiiDelay(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	st_dsdtRgmiiTimingDelay *msg_data = (st_dsdtRgmiiTimingDelay *)(msg->BUF);

	/* save dsdt rgmii delay config in dbs */
	if( CMM_SUCCESS != dbsSetDsdtRgmiiDelay(dbsdev, msg_data) )
	{
		fprintf(stderr, "\r\n  WARNING: Cannot save dsdt rgmii delay config into databases.");
		dbs_sys_log(dbsdev, DBS_LOG_WARNING, "cannot save dsdt rgmii delay config into databases");
	}

	opt_sts = cmm2dsdt_setRgmiiTimingDelay(msg_data);

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessSetDsdtPortMirroring(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	st_dsdtPortMirroring *msg_data = (st_dsdtPortMirroring *)(msg->BUF);


	opt_sts = cmm2dsdt_setPortMirroring(msg_data);	

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessAddAtherosMulticastAddr2CablePort(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	uint32_t portid = 0;
	
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	uint32_t cid = *(uint32_t *)(msg->BUF);

	portid = boardapi_getCltDsdtPortid(cid);	

	/* del atu mac entry before add */
	if( CMM_SUCCESS == cmm2dsdt_delAtherosMulticastAddressFromAtu() )
	{
		if( PORT_CABLE_PORT_ALL == portid )
		{
			opt_sts = cmm2dsdt_addAtherosMulticastAddressToAllCablePort();
		}
		else if( PORT_CABLE_PORT_NULL == portid )
		{
			opt_sts = CMM_SUCCESS;
		}
		else
		{
			opt_sts = cmm2dsdt_addAtherosMulticastAddress2Port(portid);
		}
	}
	else
	{
		opt_sts = CMM_FAILED;
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessDelAtherosMulticastAddr(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;

	opt_sts = cmm2dsdt_delAtherosMulticastAddressFromAtu();	

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessDsdtMacAddressBinding(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	stDsdtMacBinding *macBindingInfo = (stDsdtMacBinding *)(msg->BUF);
	
	/* del atu mac entry before add */
	opt_sts = cmm2dsdt_bindingMacAddress(macBindingInfo);

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessGetCbatTemperature(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	st_temperature ack_data;

	opt_sts = get_at30ts75_init_status();
	if( CMM_SUCCESS == opt_sts )
	{
		opt_sts = at30ts75_read_temperature(&ack_data);
	}
	
	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&ack_data, sizeof(st_temperature));
	return opt_sts;
}

int CMM_ProcessGetPortStatsAll(BBLOCK_QUEUE *this)
{
	int i = 0;

	T_CMM_PORT_STATS_INFO ack_data[CBAT_SW_PORT_NUM];
	T_CMM_PORT_STATS_INFO *p = ack_data;

	for( i=0; i<CBAT_SW_PORT_NUM; i++ )
	{
		cmm2dsdt_getPortAllCounters(i, p++);
	}		

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(CMM_SUCCESS, this, (uint8_t *)&ack_data, sizeof(T_CMM_PORT_STATS_INFO)*CBAT_SW_PORT_NUM);
	return CMM_SUCCESS;
}

int CMM_ProcessGetPortStats(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	uint32_t *msg_data = (uint32_t *)(msg->BUF);

	T_CMM_PORT_STATS_INFO ack_data;
	int portid = *msg_data;

	if( ( portid < 0 )||(portid > 6) )
	{
		opt_sts = CMM_FAILED;
		/* 将处理信息发送给请求者 */
		CMM_ProcessAck(opt_sts, this, NULL, 0);
		return opt_sts;
	}

	//cmm2dsdt_getPortCtr(portid, &ack_data);
	cmm2dsdt_getPortAllCounters(portid, &ack_data);

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, (uint8_t *)&ack_data, sizeof(T_CMM_PORT_STATS_INFO));
	return opt_sts;
}

int CMM_ProcessDebugPrintPortStats(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	uint32_t *msg_data = (uint32_t *)(msg->BUF);

	int portid = *msg_data;

	if( ( portid < 0 )||(portid > 6) )
	{
		opt_sts = CMM_FAILED;
		/* 将处理信息发送给请求者 */
		CMM_ProcessAck(opt_sts, this, NULL, 0);
		return opt_sts;
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(cmm2dsdt_debugPrintPortAllCounters(portid), this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessClearPortStats(BBLOCK_QUEUE *this)
{
	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(cmm2dsdt_clearPortCounters(), this, NULL, 0);
	return CMM_SUCCESS;
}

int CMM_ProcessUpgradeFtp(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	
	T_Msg_CMM *request = (T_Msg_CMM *)(this->b);

	if( request->HEADER.usSrcMID == MID_CLI )
	{
		CMM_ProcessAck(processCliUpgrade(), this, NULL, 0);
		return CMM_SUCCESS;
	}
	else
	{
		CMM_ProcessAck(CMM_SUCCESS, this, NULL, 0);
		opt_sts = processSnmpUpgrade();
	}
	return opt_sts;
}

int CMM_ProcessReloadCnuProfile(BBLOCK_QUEUE *this)
{
	stRegEvent registerEvent;
	T_Msg_CMM *msg = (T_Msg_CMM *)(this->b);
	stTmUserInfo *inode = (stTmUserInfo *)(msg->BUF);

	registerEvent.clt = inode->clt;
	registerEvent.cnu = inode->cnu;
	registerEvent.event = REG_CNU_FORCE_REGISTRATION;

	send_notification_to_reg(&registerEvent);

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(CMM_SUCCESS, this, NULL, 0);
	
	return CMM_SUCCESS;	
}

int CMM_ProcessUserPermit(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	//  printf("\r\n\r\n  CMM_ProcessUserPermit(%d, %d)\n", req_data->clt, req_data->cnu);
	
	opt_sts = cmmTmDoCnuPermit(req_data->clt, req_data->cnu);
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessUndoUserPermit(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	//printf("\r\n\r\n  CMM_ProcessUndoUserPermit(%d, %d)\n", req_data->clt, req_data->cnu);
	
	opt_sts = cmmTmUndoCnuPermit(req_data->clt, req_data->cnu);
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessDeleteCnu(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	//printf("\r\n\r\n  CMM_ProcessDeleteCnu(%d, %d)\n", req_data->clt, req_data->cnu);
	
	opt_sts = cmmTmDeleteCnu(req_data->clt, req_data->cnu);
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessCreateCnu(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);

	//printf("\r\n\r\n  CMM_ProcessCreateCnu()\n");

	/* 判断CNU 数量是否达到极限*/
	if( !__findCnuEntry() )
	{
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "cmm create cnu error: cnu entry is full !");
		opt_sts = TM_DB_MAX_ROW;
	}	
	/* 判断新添加的MAC 地址必须有效且没有重复*/
	else if( !__isNewCnuMacaddr(req->BUF) )
	{
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "cmm create cnu error: cnu mac conflict !");
		opt_sts = CMM_CONFIG_REPEAT;
	}
	else
	{
		/* 发送请求至注册模块*/
		send_notification2_to_reg(req->BUF);
		opt_sts = CMM_SUCCESS;
	}
	
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessDoWlistControl(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);

	if( CMM_DO_WLIST_CONTROL == req->HEADER.usMsgType )
	{
		//printf("\r\n\r\n  CMM_ProcessDoWlistControl(1)\n");
		opt_sts = cmmTmDoWlistControl(1);
	}
	else if( CMM_UNDO_WLIST_CONTROL == req->HEADER.usMsgType )
	{
		//printf("\r\n\r\n  CMM_ProcessDoWlistControl(0)\n");
		opt_sts = cmmTmDoWlistControl(0);
	}
	else
	{
		opt_sts = CMM_FAILED;
	}	
	
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessDoWdtControl(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	DB_INTEGER_V iValue;

	iValue.ci.tbl = DBS_SYS_TBL_ID_SYSINFO;
	iValue.ci.row = 1;
	iValue.ci.col = DBS_SYS_TBL_SYSINFO_COL_ID_WDT;
	iValue.ci.colType = DBS_INTEGER;
	iValue.len = sizeof(uint32_t);

	if( CMM_DO_WDT_CONTROL == req->HEADER.usMsgType )
	{
		if( CMM_SUCCESS == nvm_set_parameter("watchdog", "on") )
		{
			iValue.integer = 1;
			if( CMM_SUCCESS == dbsUpdateInteger(dbsdev, &iValue) )
			{
				opt_sts = CMM_SUCCESS;
			}
			else
			{
				opt_sts = CMM_FAILED;
			}
		}
		else
		{
			opt_sts = CMM_FAILED;
		}		
	}
	else if( CMM_UNDO_WDT_CONTROL == req->HEADER.usMsgType )
	{
		if( CMM_SUCCESS == nvm_set_parameter("watchdog", "off") )
		{
			iValue.integer = 0;
			if( CMM_SUCCESS == dbsUpdateInteger(dbsdev, &iValue) )
			{
				opt_sts = CMM_SUCCESS;
			}
			else
			{
				opt_sts = CMM_FAILED;
			}
		}
		else
		{
			opt_sts = CMM_FAILED;
		}
	}
	else
	{
		opt_sts = CMM_FAILED;
	}	
	
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessDumpCnuReg(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	//printf("\r\n\r\n  CMM_ProcessDumpCnuReg(%d, %d)\n", req_data->clt, req_data->cnu);
	
	opt_sts = cmmTmDumpCnuReg(req_data->clt, req_data->cnu);
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessDumpCnuMod(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	//printf("\r\n\r\n  CMM_ProcessDumpCnuMod(%d, %d)\n", req_data->clt, req_data->cnu);
	
	opt_sts = cmmTmDumpCnuMod(req_data->clt, req_data->cnu);
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

int CMM_ProcessDumpCnuPib(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	//printf("\r\n\r\n  CMM_ProcessDumpCnuPib(%d, %d)\n", req_data->clt, req_data->cnu);
	
	opt_sts = cmmTmDumpCnuPib(req_data->clt, req_data->cnu);
	CMM_ProcessAck(opt_sts, this, NULL, 0);
	return opt_sts;
}

uint32_t rtl8306e_cpu_port_set(uint8_t ODA[], uint8_t disable, uint8_t enTagInsert, uint8_t enTagRemove)
{
	uint8_t a = 0;
	uint8_t b = 0;
	uint8_t c = 0;
	T_szSwRtl8306eConfig regInfo;

	a = disable?1:0;
	b = enTagInsert?1:0;
	c = enTagRemove?1:0;

	/*Enable CPU port function, Enable inserting CPU TAG, Enable removing CPU TAG */
	regInfo.mdioInfo.phy = 2;
	regInfo.mdioInfo.reg = 21;
	regInfo.mdioInfo.page = 3;
	if( CMM_SUCCESS != mmead_get_rtl8306e_register(ODA, &regInfo) )
	{
		return CMM_FAILED;
	}
	
	regInfo.mdioInfo.value = (regInfo.mdioInfo.value & 0x7FFF)  | (a << 15) | (b << 12) | (c << 11);
	
	return mmead_set_rtl8306e_register(ODA, &regInfo);
	
}

uint32_t rtl8306e_igrAcl_get(uint8_t ODA[], stAclEntryInfo *entry)
{
	uint32_t pollcnt  ;
	uint32_t bitValue;
	T_szSwRtl8306eConfig regInfo;

	assert( NULL != entry );

	if ( entry->entryadd > 15 )
	{
		return CMM_FAILED;
	}
        
	//printf("\nrtl8306e_igrAcl_get:\n");
	/*trigger a command to read ACL entry*/
	regInfo.mdioInfo.phy = 3;
	regInfo.mdioInfo.reg = 22;
	regInfo.mdioInfo.page = 3;
	//rtl8306e_reg_get(3, 22, 3, &regValue);
	if( CMM_SUCCESS != mmead_get_rtl8306e_register(ODA, &regInfo) )
	{
		return CMM_FAILED;
	}
	regInfo.mdioInfo.value = (regInfo.mdioInfo.value & 0x81FF) | (0x3 << 13) | (entry->entryadd << 9);
	//rtl8306e_reg_set(3, 22, 3, regValue);
	if( CMM_SUCCESS != mmead_set_rtl8306e_register(ODA, &regInfo) )
	{
		return CMM_FAILED;
	}

  	//printf("\n++++++Polling whether the command is done++++++\n");
	/*Polling whether the command is done*/
	for (pollcnt = 0; pollcnt < 100 ; pollcnt++) 
	{
		regInfo.mdioInfo.phy = 3;
		regInfo.mdioInfo.reg = 22;
		regInfo.mdioInfo.page = 3;
		//rtl8306e_regbit_read(3, 22, 14, 3, &bitValue);
		if( CMM_SUCCESS != mmead_get_rtl8306e_register(ODA, &regInfo) )
		{
			return CMM_FAILED;
		}
		else
		{
			bitValue = ((regInfo.mdioInfo.value >> 14) & 0x1);
			if (!bitValue)
				break;
		}
	}
	//printf("\n++++++++++++ End Polling ++++++++++++\n");
	if (pollcnt == 100)
		return CMM_FAILED;

	regInfo.mdioInfo.phy = 3;
	regInfo.mdioInfo.reg = 21;
	regInfo.mdioInfo.page = 3;
	//rtl8306e_reg_get(3, 21, 3, &regValue);
	if( CMM_SUCCESS != mmead_get_rtl8306e_register(ODA, &regInfo) )
	{
		return CMM_FAILED;
	}
	entry->data = regInfo.mdioInfo.value;
	
	regInfo.mdioInfo.phy = 3;
	regInfo.mdioInfo.reg = 22;
	regInfo.mdioInfo.page = 3;
	//rtl8306e_reg_get(3, 22, 3, &regValue);
	if( CMM_SUCCESS != mmead_get_rtl8306e_register(ODA, &regInfo) )
	{
		return CMM_FAILED;
	}
	entry->priority = (regInfo.mdioInfo.value  >> 7) & 0x3;
	entry->action= (regInfo.mdioInfo.value  >> 5) & 0x3;
	entry->phyport = (regInfo.mdioInfo.value >> 2) & 0x7;
	entry->protocol = regInfo.mdioInfo.value & 0x3;

	return CMM_SUCCESS;
}

uint32_t rtl8306e_igrAcl_set(uint8_t ODA[], stAclEntryInfo *entry)
{
	//uint32_t regValue;
	uint32_t value;
	uint32_t pollcnt  ;
	uint32_t bitValue;
	T_szSwRtl8306eConfig regInfo;

	assert( NULL != entry );

	if ((entry->entryadd > 15) || (entry->phyport > 0x7) || (entry->action > 3) ||(entry->protocol > 3) ||(entry->priority > 3))
    	{
		return CMM_FAILED;
	}
	//printf("\nrtl8306e_igrAcl_set:\n");
  
	/*set EtherType or TCP/UDP Ports, ACL entry access register 0*/
	regInfo.mdioInfo.phy = 3;
	regInfo.mdioInfo.reg = 21;
	regInfo.mdioInfo.page = 3;
	regInfo.mdioInfo.value = entry->data;
	//rtl8306e_reg_write(3, 21, 3, entry->data);
	if( CMM_SUCCESS != mmead_set_rtl8306e_register(ODA, &regInfo) )
	{
		return CMM_FAILED;
	}

	/*set ACL entry access register 1*/
	regInfo.mdioInfo.phy = 3;
	regInfo.mdioInfo.reg = 22;
	regInfo.mdioInfo.page = 3;
	//rtl8306e_reg_read(3, 22, 3, &regValue);
	if( CMM_SUCCESS != mmead_get_rtl8306e_register(ODA, &regInfo) )
	{
		return CMM_FAILED;
	}
	value = (1 << 14) | (entry->entryadd << 9)  | (entry->priority << 7) | (entry->action << 5) | (entry->phyport << 2) | entry->protocol ;
	regInfo.mdioInfo.value = (regInfo.mdioInfo.value & 0x8000) | value  ;
	//rtl8306e_reg_write(3, 22, 3, regInfo.mdioInfo.value);
	if( CMM_SUCCESS != mmead_set_rtl8306e_register(ODA, &regInfo) )
	{
		return CMM_FAILED;
	}
	//printf("\n++++++Polling whether the command is done++++++\n");
	/*Polling whether the command is done*/
	for (pollcnt = 0; pollcnt < 100; pollcnt++) 
	{
		regInfo.mdioInfo.phy = 3;
		regInfo.mdioInfo.reg = 22;
		regInfo.mdioInfo.page = 3;
		//rtl8306e_regbit_read(3, 22, 14, 3, &bitValue);
		if( CMM_SUCCESS != mmead_get_rtl8306e_register(ODA, &regInfo) )
		{
			return CMM_FAILED;
		}
		else
		{
			bitValue = ((regInfo.mdioInfo.value >> 14) & 0x1);
			if (!bitValue)
				break;
		}		
	}
	//printf("\n++++++++++++ End Polling ++++++++++++\n");
	if (pollcnt == 100)
		return CMM_FAILED;

	return CMM_SUCCESS;
}

uint32_t rtl8306e_igrAcl_add(uint8_t ODA[], stAclEntryInfo *entry)
{
	uint32_t entaddr;
	uint32_t isFull ;
	stAclEntryInfo ientry;

	//printf("\nrtl8306e_igrAcl_add:\n");
	if( NULL == entry )
		return CMM_FAILED;
    
	isFull = BOOL_TRUE;
    
	/*if  exist an  acl entry of the same rule according by phyport, protocol,data,
	 *only update priority and action
	 */
	for (entaddr = 0; entaddr < 16; entaddr ++ ) 
	{
		ientry.entryadd = entaddr;
		if (rtl8306e_igrAcl_get(ODA, &ientry) != CMM_SUCCESS)
			return CMM_FAILED;

		if ((ientry.phyport == entry->phyport) && (ientry.protocol == entry->protocol) && (ientry.data == entry->data)) 
		{
			entry->entryadd = entaddr;
			if (rtl8306e_igrAcl_set(ODA, entry) != CMM_SUCCESS )
				return CMM_FAILED;
			else
				return CMM_SUCCESS;
		}            
	}    

	/*if not exist the rule, find an invalid entry to write it , else return table full */
	for (entaddr = 0; entaddr < 16; entaddr ++ )
	{
		ientry.entryadd = entaddr;
		if (rtl8306e_igrAcl_get(ODA, &ientry) != CMM_SUCCESS)
			return CMM_FAILED;
        
		if (ientry.phyport == 6) 
		{
			entry->entryadd = entaddr;
			if (rtl8306e_igrAcl_set(ODA, entry) != CMM_SUCCESS)
				return CMM_FAILED;
			else 
			{
				isFull = BOOL_FALSE;
				break;
			}                        
		}            
	}    

	if (isFull)
		return CMM_FAILED;
	else
		return CMM_SUCCESS;
}

uint32_t rtl8306e_igrAcl_del(uint8_t ODA[], stAclEntryInfo *entry)
{
	uint32_t entaddr;
	uint32_t isHit;
	stAclEntryInfo ientry;

	if( NULL == entry )
		return CMM_FAILED;

	//printf("\nrtl8306e_igrAcl_del:\n");
	isHit = BOOL_FALSE;
    
	if (entry->phyport == 6)
		return CMM_FAILED;
    
	for (entaddr = 0; entaddr < 16; entaddr++ ) 
	{
		ientry.entryadd = entaddr;
		if (rtl8306e_igrAcl_get(ODA, &ientry) != CMM_SUCCESS)
			return CMM_FAILED;
        
        	if ((ientry.phyport == entry->phyport) && (ientry.protocol == entry->protocol) && (ientry.data == entry->data))
		{
			ientry.entryadd = entaddr;
			ientry.action = 0;
			ientry.phyport = 6;
			ientry.protocol = 0;
			ientry.priority = 0;
			ientry.data = 0;
			if (rtl8306e_igrAcl_set(ODA, &ientry) != CMM_SUCCESS)
				return CMM_FAILED;
			isHit = BOOL_TRUE;
			break;
		}                
	}

	if (isHit)
		return CMM_SUCCESS;
	else
		return CMM_SUCCESS;
}

uint32_t rtl8306e_igrAcl_init(uint8_t ODA[])
{
	uint32_t entaddr;
	stAclEntryInfo entry;

	//printf("\nrtl8306e_igrAcl_init:\n");
	/*empty the acl table*/
	for (entaddr = 0; entaddr < 16; entaddr++ ) 
	{
		entry.entryadd = entaddr;
		entry.priority = 0;
		entry.phyport = 0x6;	/*RTL8306_ACL_INVALID*/
		entry.protocol = 0;		/*RTL8306_ACL_ETHER*/
		entry.action = 0;		/*RTL8306_ACT_DROP*/    		
		entry.data = 0;		
		
        	if ( CMM_SUCCESS != rtl8306e_igrAcl_set(ODA, &entry))
		{
			return CMM_FAILED;
		}
	}
    
	return CMM_SUCCESS;
}

int CMM_ProcessDoCnuAclDropMme(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	uint32_t cnuid = 0;
	uint8_t bMac[6] = {0};
	st_dbsCnu cnu;
	stAclEntryInfo entry;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	cnuid = (req_data->clt - 1)*MAX_CNUS_PER_CLT + req_data->cnu;

	if( (cnuid < 1) || (cnuid > MAX_CNU_AMOUNT_LIMIT))
	{
		opt_sts = CMM_FAILED;
	}	
	else if( CMM_SUCCESS != dbsGetCnu(dbsdev, cnuid, &cnu) )
	{
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		opt_sts = CMM_FAILED;
	}
	/*Enable CPU port function, Enable inserting CPU TAG, Enable removing CPU TAG */
	else if( CMM_SUCCESS != rtl8306e_cpu_port_set(bMac, 0, 0, 1))
	{
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != rtl8306e_igrAcl_init(bMac) )
	{
		opt_sts = CMM_FAILED;
	}
	else
	{
		entry.data = 0x88e1;	/*HomePlug av*/
		entry.entryadd = 0;
		entry.phyport = 7;		/*any port*/
		entry.priority = 0;
		entry.protocol = 0;		/*ethertype*/
		entry.action = 0;		/*drop*/
		opt_sts = rtl8306e_igrAcl_add(bMac, &entry);
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, NULL, 0);

	return opt_sts;
}

int CMM_ProcessUndoCnuAclDropMme(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	uint32_t cnuid = 0;
	uint8_t bMac[6] = {0};
	st_dbsCnu cnu;
	stAclEntryInfo entry;
	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	stTmUserInfo *req_data = (stTmUserInfo *)(req->BUF);

	cnuid = (req_data->clt - 1)*MAX_CNUS_PER_CLT + req_data->cnu;

	if( (cnuid < 1) || (cnuid > MAX_CNU_AMOUNT_LIMIT))
	{
		opt_sts = CMM_FAILED;
	}	
	else if( CMM_SUCCESS != dbsGetCnu(dbsdev, cnuid, &cnu) )
	{
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		opt_sts = CMM_FAILED;
	}
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		opt_sts = CMM_FAILED;
	}
	/*Disable CPU port function, disable inserting CPU TAG, disable removing CPU TAG */
	else if( CMM_SUCCESS != rtl8306e_cpu_port_set(bMac, 1, 0, 0))
	{
		opt_sts = CMM_FAILED;
	}
	else
	{
		entry.data = 0x88e1;	/*HomePlug av*/
		//entry.entryadd = 0;
		entry.phyport = 7;		/*any port*/
		//entry.priority = 0;
		entry.protocol = 0;		/*ethertype*/
		//entry.action = 0;		/*drop*/
		opt_sts = rtl8306e_igrAcl_del(bMac, &entry);
	}

	/* 将处理信息发送给请求者 */
	CMM_ProcessAck(opt_sts, this, NULL, 0);

	return opt_sts;
}

/********************************************************************************************
*	函数名称:CMM_ProcessConnect
*	函数功能:接收MACTOOL工具连接验证
*	作者:may2250
*	时间:2011-4-26
*********************************************************************************************/
static int CMM_ProcessMTConnect(BBLOCK_QUEUE *this)
{
	char *ack_data = "OK";
	CMM_ProcessMTAck(this, ack_data, strlen(ack_data));
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:CMM_ProcessToolMac
*	函数功能:接收MACTOOL工具的MAC信息并烧入CBAT
*	作者:may2250
*	时间:2011-4-26
*********************************************************************************************/
static int CMM_ProcessMTProgram(BBLOCK_QUEUE *this)
{
	int opt_sts = CMM_SUCCESS;
	stMTmsgInfo recvpack;
	//char strDevModel[32] = {0};

	T_Msg_CMM *req = (T_Msg_CMM *)(this->b);
	stMTmsgInfo *req_data = (stMTmsgInfo *)(req->BUF);

	bzero(&recvpack, sizeof(recvpack));
	
	memcpy(recvpack.mac, req_data->mac, 17);
	
	/* change big-endian to little-endian */
	recvpack.model = boardapi_umapDevModel(req_data->model);
	
	/*烧写主控板MAC*/
	opt_sts = boardapi_setMTParameters(&recvpack);
	
	if( CMM_SUCCESS ==  opt_sts )
	{
		CMM_ProcessMTAck(this, "1", 1);
		return CMM_SUCCESS;
	}
	else
	{
		CMM_ProcessMTAck(this, "0", 1);
		perror("\nERROR: CMM_ProcessToolMac write mac address failed\n");
		dbs_sys_log(dbsdev, DBS_LOG_ALERT, "ERROR: CMM_ProcessToolMac write mac address failed");
		return CMM_FAILED;
	}	
}

void cmmProcessManager(void)
{
	int opt_sts = CMM_SUCCESS;
	T_Msg_Header_CMM *h = NULL;
	BBLOCK_QUEUE *this = &bblock;
	int FromAddrSize = 0;	
	memcpy((char *)&(this->sk), (const char *)&SK_CMM, sizeof(T_UDP_SK_INFO));
	FromAddrSize = sizeof(SK_CMM.skaddr);

	assert( NULL != this );

	while(1)
	{
		bzero(this->b, MAX_UDP_SIZE);
		
		/*这里都假设fragment字段为0，不考虑有消息分片的情况*/
		this->blen = recvfrom(this->sk.sk, this->b, MAX_UDP_SIZE, 0, 
			(struct sockaddr *)&(this->sk.skaddr), &FromAddrSize);

		/* 打印接收到的报文*/
		cmm_debug_printf("\n====> CMM RECIEVED MASSAGE:\n");
		cmm_dump_msg(this->b, this->blen, stderr);
		
		h = (T_Msg_Header_CMM *)(this->b);

		/* WEC9720EK cpu is little-endian but WEC-3501I cpu is big-endian, Modifications to production test software compatible interface */
		if( h->usDstMID != MID_CMM )
		{
			fprintf(stderr, "cmm received error msg: usDstMID = 0x%02X, continue", h->usDstMID);
			continue;
		}
		
		/* 该分支语句中的任意一个case的处理函数都不应该阻塞 */
		switch(h->usMsgType)
		{
			case CMM_MODULE_DEBUG_CONTROL:
			{
				opt_sts = CMM_ProcessModuleDebugCtl(this);
				break;
			}
			case CMM_CLT_RESET:
			{
				opt_sts = CMM_ProcessCltReset(this);
				break;
			}
			case CMM_CNU_RESET:
			{
				opt_sts = CMM_ProcessCnuReset(this);
				break;
			}
			case CMM_SEND_TOPOLOGY_HB_TRAP:
			{
				opt_sts = CMM_ProcessSendHeartbeat(this);
				break;
			}
			case CMM_DO_HB_TRAP_CTRL:
			case CMM_UNDO_HB_TRAP_CTRL:
			{
				opt_sts = CMM_ProcessHeartbeatCtrl(this);
				break;
			}
			case CMM_CBAT_RESET:
			{
				opt_sts = CMM_ProcessCbatReset(this);
				break;
			}
			case CMM_RESTORE_DEFAULT:
			{
				opt_sts = CMM_ProcessRestoreDefault(this);
				break;
			}
			case CMM_CNU_VLAN_CONFIG:
			{
				opt_sts = CMM_ProcessCnuVlanConfig(this);
				break;
			}
			case CMM_CLI_FLOWCONTROL:
			{
				opt_sts = CMM_ProcessCnuRateLimit(this);
				break;
			}
			case CMM_CLI_STROMCONTROL:
			{
				opt_sts = CMM_ProcessCnuStormFilter(this);
				break;
			}
			case CMM_CLI_SHUTDOWN:
			{
				opt_sts = CMM_ProcessCnuPortStatusConfig(this);
				break;
			}
			case CMM_CLI_DO_AGING_TIME_CONFIG:
			case CMM_DO_MAC_LIMIT_CONFIG:
			{
				opt_sts = CMM_ProcessCnuMacLimitConfig(this);
				break;
			}
			case CMM_AR8236_PHY_REG_READ:
			{
				opt_sts = CMM_ProcessReadAr8236Phy(this);
				break;
			}
			case CMM_AR8236_PHY_REG_WRITE:
			{
				opt_sts = CMM_ProcessWriteAr8236Phy(this);
				break;
			}
			case CMM_AR8236_SW_REG_READ:
			{
				opt_sts = CMM_ProcessReadAr8236Reg(this);
				break;
			}
			case CMM_USER_HFID_READ:
			{
				opt_sts = CMM_ProcessReadUserHFID(this);
				break;
			}
			case CMM_GET_HG_MANAGE:
			{
				opt_sts = CMM_ProcessReadHgManage(this);
				break;
			}
			case CMM_SET_HG_MANAGE:
			{
				opt_sts = CMM_ProcessWriteHgManage(this);
			}
			case CMM_GET_HG_BUSINESS:
			{
				opt_sts = CMM_ProcessReadHgBusiness(this);
				break;
			} 
			case CMM_SET_HG_BUSINESS:
			{
				opt_sts = CMM_ProcessWriteHgBusiness(this);
				break;
			}
			case CMM_AR8236_SW_REG_WRITE:
			{
				opt_sts = CMM_ProcessWriteAr8236Reg(this);
				break;
			}
			case CMM_CNU_SWITCH_READ:
			{
				opt_sts = CMM_ProcessCnuSwitchRead(this);
				break;
			}
			case CMM_CNU_SWITCH_CONFIG_READ:
			{
				opt_sts = CMM_ProcessCnuSwitchConfigRead(this);
				break;
			}
			case CMM_CNU_SWITCH_CONFIG_WRITE:
			{
				opt_sts = CMM_ProcessCnuSwitchConfigWrite(this);
				break;
			}
			case CMM_ERASE_MOD_A:
			{
				opt_sts = CMM_ProcessCnuEraseModa(this);
				break;
			}
			case CMM_ERASE_MOD_B:
			{
				opt_sts = CMM_ProcessCnuEraseModb(this);
				break;
			}
			case CMM_CNU_SWITCH_WRITE:
			{
				opt_sts = CMM_ProcessCnuSwitchWrite(this);
				break;
			}
			case CMM_MME_MDIO_READ:
			{
				opt_sts = CMM_ProcessMmeMdioPhyRead(this);
				break;
			}
			case CMM_MME_MDIO_WRITE:
			{
				opt_sts = CMM_ProcessMmeMdioPhyWrite(this);
				break;
			}
			case CMM_CLI_UPGRADE:
			{
				opt_sts=CMM_ProcessUpgradeFtp(this);
				break;
			}
			case CMM_CLI_SEND_CONFIG:
			{
				opt_sts=CMM_ProcessReloadCnuProfile(this);
				break;
			}
			case CMM_CLI_USER_NEW:
			{
				opt_sts=CMM_ProcessUserPermit(this);
				break;
			}
			case CMM_CLI_USER_DEL:
			{
				opt_sts=CMM_ProcessUndoUserPermit(this);
				break;
			}
			case CMM_CLI_DELETE_USER:
			{
				opt_sts=CMM_ProcessDeleteCnu(this);
				break;
			}
			case CMM_DO_WLIST_CONTROL:
			case CMM_UNDO_WLIST_CONTROL:
			{
				opt_sts=CMM_ProcessDoWlistControl(this);
				break;
			}
			case CMM_DO_WDT_CONTROL:
			case CMM_UNDO_WDT_CONTROL:
			{
				opt_sts=CMM_ProcessDoWdtControl(this);
				break;
			}
			case CMM_CREATE_CNU:
			{
				opt_sts=CMM_ProcessCreateCnu(this);
				break;
			}
			case CMM_DUMP_CNU_REG:
			{
				opt_sts=CMM_ProcessDumpCnuReg(this);
				break;
			}
			case CMM_DUMP_CNU_MOD:
			{
				opt_sts=CMM_ProcessDumpCnuMod(this);
				break;
			}
			case CMM_DUMP_CNU_PIB:
			{
				opt_sts=CMM_ProcessDumpCnuPib(this);
				break;
			}
			case CMM_DO_LINK_DIAG:
			{
				opt_sts=CMM_ProcessLinkDiag(this);
				break;
			}
			case CMM_GET_IP175D_PORT_PROPETY:
			{
				opt_sts=CMM_ProcessGetPortPropety(this);
				break;
			}
			case CMM_GET_PORT_STAT:
			{
				opt_sts=CMM_ProcessGetPortStats(this);
				break;
			}
			case CMM_CLEAR_PORT_STAT:
			{
				opt_sts = CMM_ProcessClearPortStats(this);
				break;
			}
			case CMM_DEBUG_PRINT_PORT_STAT:
			{
				opt_sts=CMM_ProcessDebugPrintPortStats(this);
				break;
			}
			case CMM_GET_DSDT_RGMII_DELAY:
			{
				opt_sts=CMM_ProcessGetDsdtRgmiiDelay(this);
				break;
			}
			case CMM_SET_DSDT_RGMII_DELAY:
			{
				opt_sts=CMM_ProcessSetDsdtRgmiiDelay(this);
				break;
			}
#ifdef __AT30TK175STK__
			case CMM_GET_CBAT_TEMPERATURE:
			{
				opt_sts=CMM_ProcessGetCbatTemperature(this);
				break;
			}
#endif
			case CMM_SET_DSDT_PORT_MIRRORING:
			{
				opt_sts=CMM_ProcessSetDsdtPortMirroring(this);
				break;
			}
			case CMM_ADD_ATHEROS_ADDR:
			{
				opt_sts=CMM_ProcessAddAtherosMulticastAddr2CablePort(this);
				break;
			}
			case CMM_DEL_ATHEROS_ADDR:
			{
				opt_sts=CMM_ProcessDelAtherosMulticastAddr(this);
				break;
			}
			case CMM_DSDT_MAC_BINDING:
			{
				opt_sts=CMM_ProcessDsdtMacAddressBinding(this);
				break;
			}
			case CMM_GET_CLT_PORT_LINK_STS:
			{
				opt_sts = CMM_ProcessGetCltPortLinkSts(this);
				break;
			}
			case CMM_DO_CNU_ACL_DROP_MME:
			{
				opt_sts = CMM_ProcessDoCnuAclDropMme(this);
				break;
			}
			case CMM_UNDO_CNU_ACL_DROP_MME:
			{
				opt_sts = CMM_ProcessUndoCnuAclDropMme(this);
				break;
			}
			case CMM_GET_88E6171R_PORT_PROPETY_ALL:
			{
				opt_sts=CMM_ProcessGetPortPropetyAll(this);
				break;
			}
			case CMM_GET_88E6171R_PORT_STATS_ALL:
			{
				opt_sts=CMM_ProcessGetPortStatsAll(this);
				break;
			}
			case CMM_CONNET:
			{
				opt_sts = CMM_ProcessMTConnect(this);
				break;
			}
			case CMM_TOOL_MAC:
			{
				opt_sts = CMM_ProcessMTProgram(this);
				break;
			}	
			case CMM_GET_RTL8306E_PORT_STATUS_INFO:
			{
				opt_sts = CMM_ProcessCnuSwitchPortStatusRead(this);
				break;
			}
			case CMM_GET_CLT_FREQUENCY_BAND:
			{
				opt_sts = CMM_ProcessCltFrequencyBandRead(this);
				break;
			}
			case CMM_SET_CLT_FREQUENCY_BAND:
			{
				opt_sts = CMM_ProcessCltFrequencyBandWrite(this);
				break;
			}
			default:
			{
				/* 对于不支持的消息类型应该给予应答以便让请求者知道 */
				CMM_ProcessAck(CMM_UNKNOWN_MMTYPE, this, NULL, 0);
				opt_sts = CMM_UNKNOWN_MMTYPE;
				break;
			}
		}
		CMM_WriteOptLog(this, opt_sts);
	}
}

int init_socket_cmm(void)
{
	struct sockaddr_in server_addr;
	T_UDP_SK_INFO *sk = &SK_CMM;

	/*创建UDP SOCKET接口*/
	if( ( sk->sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}

	/* CMM模块守护进程套接字*/
	bzero((char *)&(sk->skaddr), sizeof(sk->skaddr));
   	server_addr.sin_family = PF_INET;
   	server_addr.sin_port = htons(CMM_LISTEN_PORT);		/* 目的端口号*/
   	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	/* 目的地址*/
	if (bind(sk->sk, (struct sockaddr*)&server_addr, sizeof(server_addr))<0)
	{
		return CMM_CREATE_SOCKET_ERROR;
	}

	return CMM_SUCCESS;
}

int close_socket_cmm(void)
{
	T_UDP_SK_INFO *sk = &SK_CMM;
	if( sk->sk != 0)
	{
		close(sk->sk);
		sk->sk = 0;
	}
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:cmmSignalProcessHandle
*	函数功能:异常处理句柄函数，当进程退出时该函数被调度
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void cmmSignalProcessHandle(int n)
{
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "cmmSignalProcessHandle : module cmm exit");
	
	/* 关闭socket接口 */
	cmm2alarm_destroy();
	destroy_cmm_reg();
	destroy_cmm_tm();
	destroy_cmm_mmead();
	close_socket_cmm();
	dbsClose(dbsdev);
#ifdef __AT30TK175STK__
	uninit_at30ts75();
#endif
	exit(0);
}

/********************************************************************************************
*	函数名称:main
*	函数功能:CMM主入口函数，接收来自外部模块的请求，
*				   完成业务逻辑的处理
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
int main(void)
{
	/*创建与数据库模块通讯的外部SOCKET接口*/
	dbsdev = dbsOpen(MID_CMM);
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: cmm->dbsOpen error, exited !\n");
		return CMM_CREATE_SOCKET_ERROR;
	}
	
	/* 创建外部通讯UDP SOCKET */
	if( init_socket_cmm() != CMM_SUCCESS )
	{
		fprintf(stderr, "cmm->init_socket_cmm error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module cmm init_socket_cmm error, exited !");
		dbsClose(dbsdev);
		return CMM_CREATE_SOCKET_ERROR;
	}
	
	/*创建与MMEAD模块通讯的外部SOCKET接口*/
	if( init_cmm_mmead() != CMM_SUCCESS )
	{
		fprintf(stderr, "cmm->init_cmm_mmead error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module cmm init_cmm_mmead error, exited !");
		close_socket_cmm();
		dbsClose(dbsdev);
		return CMM_CREATE_SOCKET_ERROR;
	}
	
	/*创建与TEMPLATE模块通讯的外部SOCKET接口*/
	if( init_cmm_tm() != CMM_SUCCESS )
	{
		fprintf(stderr, "cmm->init_cmm_tm error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module cmm init_cmm_tm error, exited !");
		destroy_cmm_mmead();
		close_socket_cmm();
		dbsClose(dbsdev);
		return CMM_CREATE_SOCKET_ERROR;
	}
	
	/*创建与register模块通讯的外部SOCKET接口*/
	if( init_cmm_reg() != CMM_SUCCESS )
	{
		fprintf(stderr, "cmm->init_cmm_reg error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module cmm init_cmm_reg error, exited !");
		destroy_cmm_tm();
		destroy_cmm_mmead();
		close_socket_cmm();
		dbsClose(dbsdev);
		return CMM_CREATE_SOCKET_ERROR;
	}

	/*创建与alarm模块通讯的外部SOCKET接口*/
	if( cmm2alarm_init() != CMM_SUCCESS )
	{
		fprintf(stderr, "cmm->cmm2alarm_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module cmm cmm2alarm_init error, exited !");
		destroy_cmm_reg();
		destroy_cmm_tm();
		destroy_cmm_mmead();
		close_socket_cmm();
		dbsClose(dbsdev);
		return CMM_CREATE_SOCKET_ERROR;
	}

	/*创建与sysMOnitor模块通讯的外部SOCKET接口*/
	if( cmm2sysmonitor_init() != CMM_SUCCESS )
	{
		fprintf(stderr, "cmm->cmm2sysmonitor_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module cmm cmm2sysmonitor_init error, exited !");
		cmm2alarm_destroy();
		destroy_cmm_reg();
		destroy_cmm_tm();
		destroy_cmm_mmead();
		close_socket_cmm();
		dbsClose(dbsdev);
		return CMM_CREATE_SOCKET_ERROR;
	}

	/*初始化DSDT*/
	if( cmm2dsdt_init() != CMM_SUCCESS )
	{
		fprintf(stderr, "cmm->cmm2dsdt_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module cmm cmm2dsdt_init error, exited !");
		cmm2sysmonitor_destroy();
		cmm2alarm_destroy();
		destroy_cmm_reg();
		destroy_cmm_tm();
		destroy_cmm_mmead();
		close_socket_cmm();
		dbsClose(dbsdev);
		return CMM_FAILED;
	}	
	
	/*初始化网络信息配置*/
	if( init_network() != CMM_SUCCESS )
	{
		fprintf(stderr, "cmm->init_network error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module cmm init_network error, exited !");
		cmm2sysmonitor_destroy();
		cmm2alarm_destroy();
		destroy_cmm_reg();
		destroy_cmm_tm();
		destroy_cmm_mmead();
		close_socket_cmm();
		dbsClose(dbsdev);
		return CMM_FAILED;
	}

	/* please binding after init_network */
	if( cmm2dsdt_addAtherosMulticastAddressToAllCablePort() != CMM_SUCCESS )
	{
		fprintf(stderr, "cmm->init_network error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module cmm init_network error, exited !");
		cmm2sysmonitor_destroy();
		cmm2alarm_destroy();
		destroy_cmm_reg();
		destroy_cmm_tm();
		destroy_cmm_mmead();
		close_socket_cmm();
		dbsClose(dbsdev);
		return CMM_FAILED;
	}
	else
	{
		//fprintf(stderr, "cmm binding atheros multicast address\n");
		dbs_sys_log(dbsdev, DBS_LOG_INFO, "cmm add atheros multicast address to all cable port");
	}
	
#ifdef __AT30TK175STK__
	/* 初始化温感芯片*/
	if( init_at30ts75() != CMM_SUCCESS )
	{
		fprintf(stderr, "cmm->init_at30ts75() failed\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module cmm init_at30ts75 failed");
	}
#endif

	/* 注册异常退出句柄函数*/
	signal(SIGTERM, cmmSignalProcessHandle);	

	fprintf(stderr, "Starting module CMM		......		[OK]\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting module cmm success");
	
	/* 循环处理外部请求*/
	cmmProcessManager();

	/* 不要在这个后面添加代码，执行不到滴*/
	/* 关闭socket接口 */
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "module cmm exit");	
	cmm2sysmonitor_destroy();
	cmm2alarm_destroy();
	destroy_cmm_reg();
	destroy_cmm_tm();
	destroy_cmm_mmead();
	close_socket_cmm();
	dbsClose(dbsdev);
#ifdef __AT30TK175STK__
	uninit_at30ts75();
#endif
	return 0;
}

