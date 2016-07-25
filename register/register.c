/*****************************************************************************************
  文件名称 : register.c
  文件描述 : register模块主入口函数
  				该模块为后台进程，负责定期获取拓扑信息并更新至
  				数据库；发现拓扑变迁时通知告警管理模块处理告警
  				并触发自动配置和自动升级流程。
  				不涉及网络字节序与主机字节序的转换
  修订记录 :
           1 创建 : frank
             日期 : 2010-07-29
             描述 : 创建文件

 *****************************************************************************************/
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <dbsapi.h>
#include <boardapi.h>
#include "register.h"
#include "reg_dbs.h"
#include "reg_alarm.h"
#include "reg_tm.h"
#include "reg_cmm.h"
#include "reg_mmead.h"

T_UDP_SK_INFO SK_REGI;
T_UDP_SK_INFO SK_REG2CMM;
static BBLOCK_QUEUE bblock;
T_TOPOLOGY_INFO topEntry;
uint8_t cltFlags[MAX_CLT_AMOUNT_LIMIT] = {0};
uint8_t cnuFlags[MAX_CNU_AMOUNT_LIMIT] = {0};
int REGISTER_DEBUG_ENABLE = 0;

/********************************************************************************************
*	函数名称:debug_dump_msg
*	函数功能:调试用API，以十六进制的方式将缓冲区的内容输出到
*				   文件指针fp指向的设备文件
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void debug_dump_msg(const unsigned char memory [], size_t length, FILE *fp)
{
	if(REGISTER_DEBUG_ENABLE)
	{
		debug_printf("----------------------------------------------------------------------\n");
		hexdump(memory, length, fp);
		debug_printf("\n----------------------------------------------------------------------\n");
	}
}

/********************************************************************************************
*	函数名称:debug_print_top
*	函数功能:调试用API，打印从MMEAD模块获取的实时拓扑信息
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void debug_print_top(T_MMEAD_TOPOLOGY *plist)
{	
	if( REGISTER_DEBUG_ENABLE )
	{
		int i = 0;	
		if( NULL == plist ) return;
		debug_printf("=============================================================================\n");
		debug_printf( "clt.Mac = [%02X:%02X:%02X:%02X:%02X:%02X], clt.NumStas = [%d], clt.DevType = [%d]\n", 
			plist->clt.Mac[0], plist->clt.Mac[1], plist->clt.Mac[2], plist->clt.Mac[3], plist->clt.Mac[4], plist->clt.Mac[5], 
			plist->clt.NumStas, plist->clt.DevType );
		if( plist->clt.NumStas > 0 )
		{
			for( i=0; i<plist->clt.NumStas; i++ )
			{
				debug_printf( "	-- cnu[%d].Mac = [%02X:%02X:%02X:%02X:%02X:%02X], TX/RX = [%d/%d], DevType = [%d]\n", 
					i, 
					plist->cnu[i].Mac[0], plist->cnu[i].Mac[1], plist->cnu[i].Mac[2], 
					plist->cnu[i].Mac[3], plist->cnu[i].Mac[4], plist->cnu[i].Mac[5], 
					plist->cnu[i].AvgPhyTx, plist->cnu[i].AvgPhyRx, plist->cnu[i].DevType );
			}
		}
		debug_printf("=============================================================================\n");
	}
}

/********************************************************************************************
*	函数名称:SignalProcessHandle
*	函数功能:注册模块异常处理句柄函数
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void RegSignalProcessHandle(int n)
{
	/* 发送系统关闭的告警*/
	cbat_system_sts_notification(0);
	/* 关闭socket接口 */
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "SignalProcessHandle : module register exit");
	reg2cmm_destroy(&SK_REG2CMM);
	msg_mmead_destroy();
	msg_alarm_destroy();
	msg_tm_destroy();
	reg_dbsClose();
	//printf("\nRegister exited !\n");
	exit(0);
}

BOOLEAN isCnuIndexOnUsed(uint32_t clt_index, uint32_t cnu_index)
{
	uint32_t onUsed;
	int inode = (clt_index-1)*MAX_CNUS_PER_CLT+(cnu_index-1);
	
	if( BOOL_FALSE == topEntry.tb_cnu[inode].OnUsed )
	{
		return BOOL_FALSE;
	}
	else if( CMM_SUCCESS == db_get_user_onused(clt_index, cnu_index, &onUsed))
	{
		if( 0 == onUsed )
		{
			return BOOL_FALSE;
		}
		else
		{
			return BOOL_TRUE;
		}
	}
	else
	{
		fprintf(stderr, "ERROR: isCnuIndexOnUsed->db_get_user_onused(%d, %d) !\n", clt_index, cnu_index);
		return BOOL_TRUE;
	}
}

int set_cnu_pro_sync(uint32_t clt_index, uint32_t cnu_index, BOOLEAN status)
{
	DB_INTEGER_V st_iValue;
	BOOLEAN flag = status?BOOL_TRUE:BOOL_FALSE;
	
	st_iValue.ci.tbl = DBS_SYS_TBL_ID_CNU;
	st_iValue.ci.row = (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index;
	st_iValue.ci.col = DBS_SYS_TBL_CNU_COL_ID_SYNCH;
	st_iValue.ci.colType = DBS_INTEGER;
	st_iValue.len = sizeof(uint32_t);
	st_iValue.integer = flag;
	
	if( CMM_SUCCESS != dbsUpdateInteger(dbsdev, &st_iValue))
	{
		perror("ERROR: set_cnu_pro_sync->dbsUpdateInteger !\n");
		return CMM_FAILED;
	}
	else
	{
		db_fflush();
		return CMM_SUCCESS;
	}
}

int delete_cnu_entry(uint8_t mac[])
{
	int i = 0;
	int j = 0;
	int ret = CMM_SUCCESS;
	uint8_t cnu_macb[6] = {0};
	st_dbsCnu cnu;
	T_TOPOLOGY_INFO *this = &topEntry;
	
	for( i=0; i<MAX_CLT_AMOUNT_LIMIT; i++)
	{
		for( j=0; j<MAX_CNUS_PER_CLT; j++ )
		{
			if( CMM_SUCCESS == db_get_cnu( i+1, j+1, &cnu ) )
			{
				boardapi_macs2b(cnu.col_mac, cnu_macb);
				if( 0 == memcmp((const char *)mac, (const char *)(cnu_macb), 6) )
				{
					debug_printf("db_delete_cnu(%d, %d)\n", i+1, j+1);
					ret += db_delete_cnu(i+1, j+1);
				}
				else
				{
					continue;
				}
			}
			else
			{
				ret++;
			}
		}
	}
	for( i=0; i<MAX_CNU_AMOUNT_LIMIT; i++ )
	{		
		if( 0 == memcmp((const char *)mac, (const char *)(this->tb_cnu[i].Mac), 6) )
		{
			this->tb_cnu[i].DevType = 0;
			this->tb_cnu[i].online = DEV_STS_OFFLINE;
			this->tb_cnu[i].RxRate = 0;
			this->tb_cnu[i].TxRate = 0;
			this->tb_cnu[i].OnUsed = 0;
			bzero(this->tb_cnu[i].Mac, 6);
		}
	}

	return ret;
}

/********************************************************************************************
*	函数名称:find_idle
*	函数功能:从指定CLT的CNU列表中查找第一个空闲的index
*	返回值:成功:返回该CLT拓扑中一个可用的index, 失败:返回0
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int find_idle(int cltid)
{
	int i = 0;
	int cnuid = 0;
	
	for( i=0; i<MAX_CNUS_PER_CLT; i++ )
	{
		cnuid = i+1;
		//printf("\n##find_idle->isCnuIndexOnUsed()\n");
		if( BOOL_FALSE == isCnuIndexOnUsed(cltid, cnuid))
		{
			break;
		}
		//usleep(2000);
	}
	return cnuid;
}

void refresh_signon_cnu(uint32_t clt_index, uint32_t cnu_index, T_MMEAD_CNU_INFO *activeCnu)
{
	//int invalidCnuAccessEn = BOOL_FALSE;
	st_dbsCnu cnu;
	int cnuid = (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index;
	int inode = cnuid-1;
	T_TOPOLOGY_INFO *this = &topEntry;

	if( CMM_SUCCESS != db_get_cnu(clt_index, cnu_index, &cnu) )
	{
		perror("ERROR: refresh_signon_cnu->db_get_cnu !\n");
		return;
	}

	/*****************************************************************************************/
	/* 解决这里潜在的漏洞*/
	/* 如果MAC地址相同的非法用户在这里也能混进去*/
	/* 这里不应该更改设备类型，如果发现冲突则告警即可*/
	/*****************************************************************************************/
	if( cnu.col_model != activeCnu->DevType )
	{
		cnu.col_sts = DEV_STS_OFFLINE;
		cnu.col_rx = 0;
		cnu.col_tx = 0;
		this->tb_cnu[inode].online = DEV_STS_OFFLINE;
		this->tb_cnu[inode].RxRate = 0;
		this->tb_cnu[inode].TxRate = 0;
		db_update_cnu(clt_index, cnu_index, &cnu);
		/* 写系统日志*/
		dbs_sys_log(dbsdev, DBS_LOG_WARNING, "refresh_signon_cnu encountered cnu with conflict mac address");
		/* 发送告警*/
		fprintf(stderr, "refresh_signon_cnu mac address conflicting !\n");
	}
	/*****************************************************************************************/
	/*****************************************************************************************/
	else
	{
		/* 更新网元数据库*/
		cnu.id = cnuid;
		cnu.col_sts = DEV_STS_ONLINE;
		//cnu.col_model = activeCnu->DevType;
		cnu.col_rx = activeCnu->AvgPhyRx;
		cnu.col_tx = activeCnu->AvgPhyTx;
		if( CMM_SUCCESS == db_update_cnu(clt_index, cnu_index, &cnu))
		{
			/* 同步数据至内存数据表*/
			//this->tb_cnu[inode].DevType = activeCnu->DevType;
			this->tb_cnu[inode].online = DEV_STS_ONLINE;
			this->tb_cnu[inode].RxRate = activeCnu->AvgPhyRx;
			this->tb_cnu[inode].TxRate = activeCnu->AvgPhyTx;
			/* 通知告警管理模块*/
			cnu_sts_transition_notification(clt_index, cnu_index, DEV_STS_ONLINE);
		}
		else
		{
			perror("ERROR: refresh_signon_cnu->db_update_cnu !\n");
		}
		return;
	}
}

void do_cnu_template_auto_config(uint32_t clt_index, uint32_t cnu_index, T_MMEAD_CNU_INFO *activeCnu)
{
	st_dbsTemplate template;
	st_dbsProfile profile;
	DB_INTEGER_V iValue;
	st_dbsCnu cnu;
	uint8_t bMac[6] = {0};
	uint8_t mod[1024] = {0};
	int opt_sts=CMM_SUCCESS;
	int len = 0;
	int i=0;

	stCnuNode iNode;


	st_rtl8306eSettings ack_data;

	memset(&template, 0, sizeof(st_dbsTemplate));
		
	//get 1 row is templage management row
	if(CMM_SUCCESS != dbsGetTemplate(dbsdev, 1, &template))
	{
		perror("ERROR: do_cnu_template_auto_config->dbsGetTemplate !\n");
		return;
	}else{
	       //eth1 enable auto
	       if(1 == template.col_eth1VlanAddSts)
		{
		    if(template.col_eth1VlanStart > 1 && template.col_eth1VlanStart <= 4030 )
		    {
				//ok now auto template config
				if(template.col_eth1VlanStop < template.col_eth1VlanStart|| template.col_eth1VlanStop >= 4030 )	
					template.col_eth1VlanStop = template.col_eth1VlanStart  ;					
				else
				      template.col_eth1VlanStop++;
		    }
	       	}	
		else
		{
	       	   //eth1 disable auto 
	       	       if(template.col_eth1VlanStart >1 && template.col_eth1VlanStart <= 4094 )
				template.col_eth1VlanStop = template.col_eth1VlanStart  ;	
		}
		 //eth2 enable auto
	       if(1 == template.col_eth2VlanAddSts)
		{
		    if(template.col_eth2VlanStart > 1 && template.col_eth2VlanStart <= 4030 )
		    {
				//ok now auto template config
				if(template.col_eth2VlanStop <= template.col_eth2VlanStart|| template.col_eth2VlanStop >= 4030 )	
					template.col_eth2VlanStop = template.col_eth2VlanStart  ;					
				else
				      template.col_eth2VlanStop++;
		    }
	       	}	
		 else
		  {
	       	   //eth2 disable auto 
	       	       if(template.col_eth2VlanStart >=1 && template.col_eth2VlanStart <= 4094 )
				template.col_eth2VlanStop = template.col_eth2VlanStart  ;	
		  }
		 //eth3 enable auto
	       if(1 == template.col_eth3VlanAddSts)
		{
		    if(template.col_eth3VlanStart > 1 && template.col_eth3VlanStart <= 4030 )
		    {
				//ok now auto template config
				if(template.col_eth3VlanStop <= template.col_eth3VlanStart|| template.col_eth3VlanStop >= 4030 )	
					template.col_eth3VlanStop = template.col_eth3VlanStart  ;					
				else
				      template.col_eth3VlanStop++;
		    }
	       	}	
		else
		{
	       	   //eth3 disable auto 
	       	       if(template.col_eth3VlanStart >=1 && template.col_eth3VlanStart <= 4094 )
				template.col_eth3VlanStop = template.col_eth3VlanStart  ;	
		}
		 //eth4 enable auto
	       if(1 == template.col_eth4VlanAddSts)
		{
		    if(template.col_eth4VlanStart > 1 && template.col_eth4VlanStart <= 4030 )
		    {
				//ok now auto template config
				if(template.col_eth4VlanStop <= template.col_eth4VlanStart|| template.col_eth4VlanStop >= 4030 )	
					template.col_eth4VlanStop = template.col_eth4VlanStart  ;					
				else
				      template.col_eth4VlanStop++;
		    }
	       	}	
		 else
		   {
	       	       //eth4 disable auto 
	       	       if(template.col_eth4VlanStart >=1 && template.col_eth4VlanStart <= 4094 )
				template.col_eth4VlanStop = template.col_eth4VlanStart  ;	
		   }
								
		//config send to cnu now and save profile db vlan
		//st_rtl8306eSettings ack_data;

		//update template dbs
		if(CMM_SUCCESS != dbsUpdateTemplate(dbsdev, 1, &template))
		{
			perror("ERROR: do_cnu_template_auto_config->dbsUpdateTemplate !\n");
			return;
		}
	
		if( (cnu_index<1)||(cnu_index > MAX_CNU_AMOUNT_LIMIT))
		{
			printf("\n#ERROR[01]\n");
			opt_sts = CMM_FAILED;
		}
		else if( CMM_SUCCESS != dbsGetCnu(dbsdev,  (clt_index-1) * 64 + cnu_index, &cnu) )
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

		for(i=0; i< 6; i++){
			printf("bMac[%d]=[%d]\n", i, bMac[i]);
		}

		if(CMM_SUCCESS != reg_mmead_get_rtl8306e_configs(bMac,  &ack_data))
		{
			opt_sts = CMM_FAILED;
			printf("\n#ERROR[05]\n");
			printf("reg_mmead_get_rtl8306e_configs error\n" );
			return opt_sts;
		}

		//update switch and send config
		if(template.col_eth1VlanStart  == 1)
			ack_data.vlanConfig.vlan_port[0].egress_mode= 3;
		else{
			ack_data.vlanConfig.vlan_port[0].egress_mode = 1;
			ack_data.vlanConfig.vlan_port[0].pvid = template.col_eth1VlanStop;
			}
		if(template.col_eth2VlanStart  == 1)
			ack_data.vlanConfig.vlan_port[1].egress_mode = 3;
		else{
			ack_data.vlanConfig.vlan_port[1].egress_mode = 1;
			ack_data.vlanConfig.vlan_port[1].pvid = template.col_eth2VlanStop;
			}
		if(template.col_eth3VlanStart  == 1)
			ack_data.vlanConfig.vlan_port[2].egress_mode = 3;
		else{
			ack_data.vlanConfig.vlan_port[2].egress_mode = 1;
			ack_data.vlanConfig.vlan_port[2].pvid = template.col_eth3VlanStop;
			}
		if(template.col_eth4VlanStart  == 1)
			ack_data.vlanConfig.vlan_port[3].egress_mode = 3;
		else{
			ack_data.vlanConfig.vlan_port[3].egress_mode = 1;
			ack_data.vlanConfig.vlan_port[3].pvid = template.col_eth4VlanStop;
			}
			
		ack_data.vlanConfig.vlan_enable = 1;
		ack_data.vlanConfig.vlan_tag_aware = 1;
		ack_data.vlanConfig.vlan_port[4].egress_mode = 2;
		ack_data.vlanConfig.vlan_port[4].pvid = 1;
		
		
		

		//send to cmm to update db and switch reg
		iNode.clt = clt_index;
		iNode.cnu = cnu_index;
		if(CMM_SUCCESS  != reg2cmm_writeSwitchSettings(&SK_REG2CMM, &iNode,  &ack_data))
		{
			opt_sts = CMM_FAILED;
			printf("\n#ERROR[06]\n");
			printf("reg2cmm_writeSwitchSettings error\n" );
			return opt_sts;
		}

		printf("reg2cmm_writeSwitchSettings now successfull!!!\n" );
		
		
	}
	
}

void do_cnu_auto_config(uint32_t clt_index, uint32_t cnu_index, T_MMEAD_CNU_INFO *activeCnu)
{
	uint32_t PIB_CRC = 0;
	int cnuid = (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index;
	int inode = cnuid-1;
	//uint32_t MOD_CRC = 0;
	//uint32_t tid = 0;
	st_dbsCnu cnu;
	DB_INTEGER_V iValue;
	T_TOPOLOGY_INFO *this = &topEntry;
	
	/* 解决MAC地址相同但是设备类型不同的CNU 自动配置的BUG */
	/* 如:在该CBAT下曾经有MAC为3071b2000010的WEC-3702I接入，该设备
	下线之后，又有同样MAC地址，但是设备类型为WEC-604的设备
	注册，由于此时该设备的类型还没有写入数据库，TMCore模块
	仍然认为该设备为WEC-3702I，故生成CRC以及PIB时，仍然认为
	该设备为WEC-3702I，如果此时将生成的PIB写入设备，会产生将
	设备原始设备类型覆盖为WEC-3702I的BUG。*/
	/* 解决该问题的办法:下发配置之前检查数据库中的设备类型
	字段是否与activeCnu结构中的一致，若一致则下发配置，若不
	一致则发送告警并禁止下发配置*/
	if( CMM_SUCCESS != db_get_cnu(clt_index, cnu_index, &cnu) )
	{
		perror("ERROR: do_cnu_auto_config->db_get_cnu !\n");
		/* 发送CNU放弃自动配置的告警*/
		cnu_abort_auto_config_notification(clt_index, cnu_index);
		return;
	}
	else
	{
		if( cnu.col_model != activeCnu->DevType )
		{
			/*****************************************************************************************/
			/* 解决这里潜在的漏洞*/
			/* 如果MAC地址相同的非法用户在这里也能混进去*/
			/*****************************************************************************************/
			#if 0
			/* 这里还有潜在的漏洞*/
			/* 如果MAC地址相同的非法用户在这里也能混进去*/
			refresh_signon_cnu(clt_index, cnu_index, activeCnu);
			/* 发送CNU放弃自动配置的告警*/
			cnu_abort_auto_config_notification(clt_index, cnu_index);
			return;
			#endif
			
			/* 这里正确的做法应当是: */
			
			/* 1. 将数据库和内存中该设备的状态均置为离线状态；*/
			this->tb_cnu[inode].online = DEV_STS_OFFLINE;
			this->tb_cnu[inode].RxRate = 0;
			this->tb_cnu[inode].TxRate = 0;

			cnu.col_sts = DEV_STS_OFFLINE;
			cnu.col_rx = 0;
			cnu.col_tx = 0;
			
			/* 2. 如果发现的设备类型合法，则更新设备的类型；*/
			if( boardapi_isCnuSupported(activeCnu->DevType) )
			{
				this->tb_cnu[inode].DevType = activeCnu->DevType;
				cnu.col_model = activeCnu->DevType;
				/* 这里假定设备类型变更则一定用重新发配置*/
				cnu.col_synch = 0;
				if( CMM_SUCCESS != dbsUpdateCnu(dbsdev, cnuid, &cnu) )
				{
					perror("ERROR: do_cnu_auto_config->dbsUpdateCnu !\n");
					/* 发送CNU放弃自动配置的告警*/
					cnu_abort_auto_config_notification(clt_index, cnu_index);
					return;
				}
				/* 如果是WEC701系列，则需要更新profile->base_pib */
				switch(activeCnu->DevType)
				{
					case WEC701_C2:
					case WEC701_C4:
					{
						iValue.ci.tbl = DBS_SYS_TBL_ID_CNUPRO;
						iValue.ci.row = cnuid;
						iValue.ci.col = DBS_SYS_TBL_PROFILE_COL_ID_BASE;
						iValue.ci.colType = DBS_INTEGER;
						iValue.integer = 12;
						iValue.len = sizeof(int32_t);
						if( CMM_SUCCESS != dbsUpdateInteger(dbsdev, &iValue) )
						{
							perror("ERROR: do_cnu_auto_config->dbsUpdateInteger !\n");
							/* 发送CNU放弃自动配置的告警*/
							cnu_abort_auto_config_notification(clt_index, cnu_index);
							return;
						}
						break;
					}
					default:
						break;
				}
				printf("Warnning: fixed non-matched cnu type %d at index %d/%d\n", 
					activeCnu->DevType, clt_index, cnu_index);
				/* 写系统日志*/
				dbs_sys_log(dbsdev, DBS_LOG_WARNING, "do_cnu_auto_config encountered cnu with conflict mac address");
				/* 继续进入下面的自动配置流程*/
			}			
			/* 3. 如果发现的设备类型不合法则踢下线；*/
			else
			{
				/* 发送非法设备接入告警*/
				/* 通知告警管理模块*/
				lllegal_cnu_register_notification(clt_index, activeCnu->Mac);
				#if 0
				/* 禁止添加该设备*/				
				if( CMM_SUCCESS != msg_reg_mmead_bootout_dev(this->tb_clt.Mac, activeCnu->Mac) )
				{					
					lllegal_cnu_kick_off_notification(1, activeCnu->Mac, BOOL_FALSE);
				}
				else
				{
					lllegal_cnu_kick_off_notification(1, activeCnu->Mac, BOOL_TRUE);
				}
				#endif
				/* 写系统日志*/
				dbs_sys_log(dbsdev, DBS_LOG_WARNING, "do_cnu_auto_config encountered invalid cnu with conflict mac address");
				/* 退出自动配置流程*/
				return;
			}
			/*****************************************************************************************/
			/*****************************************************************************************/
		}
	}

	if( !boardapi_isCnuSupported(activeCnu->DevType) )
	{
		/* 其他类型的CNU都不进行任何自动配置*/
		refresh_signon_cnu(clt_index, cnu_index, activeCnu);
		/* 发送CNU放弃自动配置的告警*/
		cnu_abort_auto_config_notification(clt_index, cnu_index);
		return;
	}
	
	/* 从模板管理模块获取该用户的PIB CRC */
	if( CMM_SUCCESS != tm_get_user_pib_crc(clt_index, cnu_index, &PIB_CRC) )
	{
		perror("ERROR: do_cnu_auto_config->tm_get_user_pib_crc !\n");
		/* 发送CNU放弃自动配置的告警*/
		cnu_abort_auto_config_notification(clt_index, cnu_index);
		return;
	}
/* 测试的时候一般不希望下发PIB，故有此修改*/
/****************************************************************************************/
#if 1
	if( BOOL_TRUE == cnu.col_synch )
	{
		refresh_signon_cnu(clt_index, cnu_index, activeCnu);
	}
	else
	{
		/* 通知模板管理模块自动生成该用户的MOD */
		if( CMM_SUCCESS != tm_gen_user_mod(clt_index, cnu_index) )
		{
			perror("ERROR: do_cnu_auto_config->tm_gen_user_mod !\n");
			/* 发送CNU放弃自动配置的告警*/
			cnu_abort_auto_config_notification(clt_index, cnu_index);
			return;
		}
		/* 自动下发MOD */
		if( CMM_SUCCESS == msg_reg_mmead_wr_user_mod(activeCnu->DevType, activeCnu->Mac) )
		{
			/* 发送告警*/
			cnu_auto_config_notification(clt_index, cnu_index, cnu_index, 1, BOOL_TRUE);
			/* 将标志位csyncStatus 置1 */
			set_cnu_pro_sync(clt_index, cnu_index, BOOL_TRUE);
		}
		else
		{
			/* 发送告警*/
			cnu_auto_config_notification(clt_index, cnu_index, cnu_index, 1, BOOL_FALSE);
		}
		/* 通知模板管理模块销毁该用户的配置*/
		if( CMM_SUCCESS != tm_distroy_user_mod(clt_index, cnu_index) )
		{
			perror("ERROR: do_cnu_auto_config->tm_distroy_user_mod !\n");
			return;
		}
		
	}
#else
	/* for debug */
	//printf("\ndo_cnu_auto_config(): PIB_CRC = 0x%X, activeCnu->CRC = 0x%X\n", PIB_CRC, activeCnu->CRC[0]);
	
	/* 比较PIB CRC 校验码*/
	if( PIB_CRC == activeCnu->CRC[0] )
	//if(1)	/* PIB CRC 不能正确读取，先屏蔽该段代码*/
	{
		#if 0
		/* 从模板管理模块获取该用户的MOD CRC */
		if( CMM_SUCCESS != tm_get_user_mod_crc(clt_index, cnu_index, &MOD_CRC) )
		{
			perror("ERROR: do_cnu_auto_config->tm_get_user_mod_crc !\n");
			return;
		}
		
		/* 比较MOD CRC 校验码*/
		if( MOD_CRC == activeCnu->CRC[1] )
		#endif
		
		if( BOOL_TRUE == cnu.col_synch )
		{
			refresh_signon_cnu(clt_index, cnu_index, activeCnu);
		}
		else
		{
			/* 通知模板管理模块自动生成该用户的MOD */
			if( CMM_SUCCESS != tm_gen_user_mod(clt_index, cnu_index) )
			{
				perror("ERROR: do_cnu_auto_config->tm_gen_user_mod !\n");
				/* 发送CNU放弃自动配置的告警*/
				cnu_abort_auto_config_notification(clt_index, cnu_index);
				return;
			}
			if( CMM_SUCCESS == msg_reg_mmead_wr_user_mod(activeCnu->DevType, activeCnu->Mac) )
			{
				/* 发送告警*/
				cnu_auto_config_notification(clt_index, cnu_index, cnu_index, 1, BOOL_TRUE);
				/* 将标志位csyncStatus 置1 */
				set_cnu_pro_sync(clt_index, cnu_index, BOOL_TRUE);
			}
			else
			{
				/* 发送告警*/
				cnu_auto_config_notification(clt_index, cnu_index, cnu_index, 1, BOOL_FALSE);
			}
			/* 通知模板管理模块销毁该用户的配置*/
			if( CMM_SUCCESS != tm_distroy_user_mod(clt_index, cnu_index) )
			{
				perror("ERROR: do_cnu_auto_config->tm_distroy_user_mod !\n");
				return;
			}			
		}
	}
	else
	{
		/* 通知模板管理模块自动生成该用户的PIB */
		if( CMM_SUCCESS != tm_gen_user_pib(clt_index, cnu_index) )
		{
			perror("ERROR: do_cnu_auto_config->tm_gen_user_pib !\n");
			return;
		}
		/* 自动下发PIB */
		//if( CMM_SUCCESS != tm_get_cnu_tid(clt_index, cnu_index, &tid) )
		//{
		//	perror("ERROR: do_cnu_auto_config->tm_get_cnu_tid !\n");
		//	return;
		//}
		//printf("auto send pib(pro %d) configuration to clt %d cnu %d\n", tid, clt_index, cnu_index);
		//printf("\r\n  auto send pib configuration to clt %d cnu %d\n", clt_index, cnu_index);
		//FlashDevice will return failed if HFID is not the same
		if( CMM_SUCCESS == msg_reg_mmead_wr_user_pib(activeCnu->DevType, activeCnu->Mac) )
		{
			/* 发送告警*/
			cnu_auto_config_notification(clt_index, cnu_index, cnu_index, 0, BOOL_TRUE);
		}
		else
		{
			/* 发送告警*/
			cnu_auto_config_notification(clt_index, cnu_index, cnu_index, 0, BOOL_FALSE);
		}
		/* 通知模板管理模块销毁该用户的配置*/
		if( CMM_SUCCESS != tm_distroy_user_pib(clt_index, cnu_index) )
		{
			perror("ERROR: do_cnu_auto_config->tm_distroy_user_pib !\n");
			return;
		}		
	}
#endif
/****************************************************************************************/
}

void refresh_active_cnu(uint32_t clt_index, uint32_t cnu_index, T_MMEAD_CNU_INFO activeCnu)
{
	st_dbsCnu cnu;
	int cnuid = (clt_index-1)*MAX_CNUS_PER_CLT+cnu_index;
	int inode = cnuid-1;
	T_TOPOLOGY_INFO *this = &topEntry;
	
	if( (this->tb_cnu[inode].RxRate != activeCnu.AvgPhyRx) || 
	     (this->tb_cnu[inode].TxRate != activeCnu.AvgPhyTx))
	{
		db_get_cnu(clt_index, cnu_index, &cnu);
		
		cnu.id = cnuid;
		cnu.col_rx = activeCnu.AvgPhyRx;
		cnu.col_tx = activeCnu.AvgPhyTx;
		if( CMM_SUCCESS == db_update_cnu(clt_index, cnu_index, &cnu))
		{
			/* 同步数据*/
			this->tb_cnu[inode].RxRate = activeCnu.AvgPhyRx;
			this->tb_cnu[inode].TxRate = activeCnu.AvgPhyTx;
		}		
	}

	/* 解决CNU快速按电源导致的BUG */
	//if( !isCnuAuthorized(clt_index, cnu_index) )
	//{
	//	msg_reg_mmead_block_user(activeCnu.DevType, activeCnu.Mac);
	//}	
	
}

/********************************************************************************************
*	函数名称:do_clt_register
*	函数功能:CLT注册函数，该函数完成CLT上线时的注册过程
*					1. 完成CLT信息在数据库中的注册;
*					2. 将注册获取的实时信息同步至内存;
*					3. 完成告警通知；
*					4. 完成自动配置；
*	注意:调用该函数进行注册的设备一定是数据库网元表中
*			已经存在的设备，若是新发现的设备在调用该函数
*			进行注册时，需要先将设备的标识信息写入数据库
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void do_clt_register(uint32_t clt_index, T_MMEAD_CLT_INFO activeClt)
{
	st_dbsClt clt;
	T_TOPOLOGY_INFO *this = &topEntry;

	db_get_clt(clt_index, &clt);

	if( DEV_STS_OFFLINE == this->tb_clt[clt_index-1].online )
	{
		clt.id = clt_index;
		clt.col_model = activeClt.DevType;
		clt.col_sts = DEV_STS_ONLINE;
		clt.col_numStas = activeClt.NumStas;		
		
		if( CMM_SUCCESS == db_update_clt(clt_index, &clt))
		{
			/* 同步数据*/
			this->tb_clt[clt_index-1].online = DEV_STS_ONLINE;
			this->tb_clt[clt_index-1].DevType = activeClt.DevType;
			this->tb_clt[clt_index-1].NumStas = activeClt.NumStas;
			/* 通知告警管理模块*/
			clt_sts_transition_notification(clt_index, DEV_STS_ONLINE);			
			/* 通知自动升级模块*/
			/* 通知自动配置模块*/
			/* 代码缺失*//* 暂不支持CLT自动配置*/
		}
		else
		{
			perror("ERROR: do_clt_register->db_update_clt !\n");
		}
	}
	else
	{
		/* 更新网元数据表中的NumStas 字段*/
		if( this->tb_clt[clt_index-1].NumStas != activeClt.NumStas )
		{
			clt.id = clt_index;
			clt.col_sts = DEV_STS_ONLINE;
			clt.col_numStas = activeClt.NumStas;
			if( CMM_SUCCESS == db_update_clt(clt_index, &clt))
			{
				/* 同步数据*/
				this->tb_clt[clt_index-1].NumStas = activeClt.NumStas;
			}
		}
	}
}

/********************************************************************************************
*	函数名称:do_clt_unregister
*	函数功能:CLT反注册函数，该函数完成CLT下线时的反注册过程
*					1. 完成CLT信息在数据库中的反注册;
*					2. 将反注册信息同步至内存;
*					3. 完成告警通知；
*	注意:调用该函数进行反注册的设备一定是数据库网元表中
*			已经存在的设备
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void do_clt_unregister(uint32_t clt_index)
{
	int inode = clt_index-1;
	T_TOPOLOGY_INFO *this = &topEntry;
	
	if( DEV_STS_OFFLINE != this->tb_clt[inode].online )
	{
		/* 写数据库*/
		if( CMM_SUCCESS == db_unregister_clt(clt_index) )
		{
			/* 同步数据*/
			this->tb_clt[inode].online = DEV_STS_OFFLINE;
			this->tb_clt[inode].NumStas = 0;
			/* 通知告警管理模块*/
			clt_sts_transition_notification(clt_index, DEV_STS_OFFLINE);
		}
		else
		{
			perror("ERROR: do_clt_unregister->db_unregister_clt !\n");
		}
	}
}

/********************************************************************************************
*	函数名称:do_cnu_register
*	函数功能:CNU注册函数，该函数完成CNU上线时的注册过程
*					1. 完成CNU信息在数据库中的注册;
*					2. 将注册获取的实时信息同步至内存;
*					3. 完成告警通知；
*					4. 完成自动配置；
*	注意:调用该函数进行注册的设备一定是数据库网元表中
*			已经存在的设备，若是新发现的设备在调用该函数
*			进行注册时，需要先将设备的标识信息写入数据库
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void do_cnu_register(uint32_t clt_index, uint32_t cnu_index, T_MMEAD_CNU_INFO activeCnu)
{
	int autoCfgSts = 0;
	//uint32_t userType = 0;
	//st_dbsCnu cnu;

	int inode = (clt_index-1)*MAX_CNUS_PER_CLT+(cnu_index-1);
	T_TOPOLOGY_INFO *this = &topEntry;
	
	/* 调用该函数进行注册的CNU一定是合法的设备，因为非法
	** CNU在注册之前就禁止添加至用户表，根本不会触发注册过程*/

	/* 如果有外部重新注册请求*/
	if( REG_CNURESET == cnuFlags[inode] )
	{
		//printf("\r\n  register event call : reset clt %d cnu %d\n", clt_index, cnu_index);
		
		/* 发送MME重启CNU*/
		msg_reg_mmead_reset_eoc(activeCnu.DevType, activeCnu.Mac);

		/* 将该CNU强制下线*/
		do_cnu_unregister(clt_index, cnu_index);
		
		//还原标志位
		cnuFlags[inode] = 0;

		return;
	}
	else if( REG_CNU_FORCE_REGISTRATION == cnuFlags[inode] )
	{
		//printf("\r\n  register event call : force clt %d cnu %d re-registration\n", clt_index, cnu_index);
		
		/* 发送CNU强制重新注册的告警*/
		cnu_re_register_notification(clt_index, cnu_index);
		
		/* 获取该CNU的授权状态*/
		//if( isCnuAuthorized(clt_index, cnu_index) )
		//{
			//开启CNU用户端口
		//	msg_reg_mmead_enable_user(activeCnu.DevType, activeCnu.Mac);
		//}
		//else
		//{
			//关闭CNU用户端口
		//	msg_reg_mmead_block_user(activeCnu.DevType, activeCnu.Mac);
		//}
		/* 将该CNU强制下线*/
		do_cnu_unregister(clt_index, cnu_index);
		/* 将标志位csyncStatus 置0 */
		//set_cnu_pro_sync(clt_index, cnu_index, BOOL_FALSE);

		//还原标志位
		cnuFlags[inode] = 0;
		
		return;
	}
	
	
	/* 如果该设备有状态变迁则进行处理*/
	if( DEV_STS_OFFLINE == this->tb_cnu[inode].online )
	{
		#if 0
		/* 获取该CNU的用户类型*/
		if( CMM_SUCCESS == db_get_user_type(clt_index, cnu_index, &userType))
		{
			if( 0 == userType )
			{
				/* 匿名用户注册*/
				printf("\nclt %d cnu %d do anonymous registration\n", clt_index, cnu_index);
				do_anonymous_register(clt_index, cnu_index, activeCnu);
			}
			else
			{
				/* 知名用户注册*/
				printf("\nclt %d cnu %d do registration\n", clt_index, cnu_index);
				do_user_register(clt_index, cnu_index, activeCnu);
			}
		}
		else
		{
			perror("ERROR: do_cnu_register->db_get_user_type !\n");
			return;
		}
		#endif
		
		///////////////////////////////////////////////////////////////////
		/* 获取全局自动配置使能状态*/
		
		if( CMM_SUCCESS != db_get_auto_config_sts(&autoCfgSts))
		{
			perror("ERROR: do_cnu_register->db_get_auto_config_sts !\n");
			return ;
		}
		else
		{
			if( !boardapi_isCnuSupported(activeCnu.DevType) )
			{
				cnu_abort_auto_config_notification(clt_index, cnu_index);
				refresh_signon_cnu(clt_index, cnu_index, &activeCnu);
			}			
			else if( 0 == autoCfgSts )
			{
				//这里还有潜在的漏洞
				// 如果MAC地址相同的非法用户在这里也能混进去
				refresh_signon_cnu(clt_index, cnu_index, &activeCnu);
			}
			else
			{
				// 自动配置
				do_cnu_auto_config(clt_index, cnu_index, &activeCnu);
			}
		 }
		    
	}
	else
	{
		/* 更新网元数据库中的实时数据*/
		/* 这里还有潜在的漏洞*//* 不过要求动作很快才能触发*/
		/* 如果MAC地址相同的非法用户在这里也能混进去*/
		refresh_active_cnu(clt_index, cnu_index, activeCnu);
	}
	
}

void do_cnu_delete(uint32_t clt_index, uint32_t cnu_index)
{
	int inode = (clt_index-1)*MAX_CNUS_PER_CLT+(cnu_index-1);
	T_TOPOLOGY_INFO *this = &topEntry;

	/* 同步数据*/
	this->tb_cnu[inode].online = DEV_STS_OFFLINE;
	this->tb_cnu[inode].RxRate = 0;
	this->tb_cnu[inode].TxRate = 0;
	this->tb_cnu[inode].OnUsed = 0;
	bzero(this->tb_cnu[inode].Mac, 6);

	//db_unregister_cnu(cnu_index);	
	/* 数据表项在TM中已经被删除*/
	//db_delete_cnu(cnu_index);
	db_fflush();
}


/********************************************************************************************
*	函数名称:do_cnu_unregister
*	函数功能:CNU反注册函数，该函数完成CNU下线时的反注册过程
*					1. 完成CNU信息在数据库中的反注册;
*					2. 将反注册信息同步至内存;
*					3. 完成告警通知；
*	注意:调用该函数进行反注册的设备一定是数据库网元表中
*			已经存在的设备
*	作者:frank
*	时间:2010-08-13
*********************************************************************************************/
void do_cnu_unregister(uint32_t clt_index, uint32_t cnu_index)
{
	int inode = (clt_index-1)*MAX_CNUS_PER_CLT+(cnu_index-1);
	int cnuid = inode + 1;
	T_TOPOLOGY_INFO *this = &topEntry;
	
	if( DEV_STS_OFFLINE != this->tb_cnu[inode].online )
	{
		/* 写数据库*/
		if( CMM_SUCCESS == db_unregister_cnu(clt_index, cnu_index) )
		{
			/* 同步数据*/
			this->tb_cnu[inode].online = DEV_STS_OFFLINE;
			this->tb_cnu[inode].RxRate = 0;
			this->tb_cnu[inode].TxRate = 0;
			/* 通知告警管理模块*/
			cnu_sts_transition_notification(clt_index, cnu_index, DEV_STS_OFFLINE);
		}
		else
		{
			perror("ERROR: do_cnu_unregister->db_unregister_cnu !\n");
		}
	}
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
		//usleep(5000);
	}
	return 1;
}

void do_create_cnu(uint8_t macaddr[])
{
	int cltid = 1;
	int idle = 0;
	int inode = 0;
	st_dbsCnu cnu;
	T_TOPOLOGY_INFO *this = &topEntry;

	/* 假定默认添加设备类型*/
	//cnu.col_model = WEC_604;
	cnu.col_model = WEC701_C4;
	sprintf(cnu.col_mac, "%02X:%02X:%02X:%02X:%02X:%02X", 
		macaddr[0], macaddr[1], macaddr[2], 
		macaddr[3], macaddr[4], macaddr[5]
	);
	cnu.col_sts = 0;
	cnu.col_auth = 1;
	strcpy(cnu.col_ver, "Unknown");
	cnu.col_rx = 0;
	cnu.col_tx = 0;
	strcpy(cnu.col_snr, "0%");
	strcpy(cnu.col_bpc, "0%");
	strcpy(cnu.col_att, "0dB");
	cnu.col_synch = BOOL_TRUE;
	cnu.col_row_sts = BOOL_TRUE;

	/* 先判断MAC 地址是否冲突*/
	if( !__isNewCnuMacaddr(macaddr) )
	{
		printf("\r\n\r\n  create entry for cnu %s failed !\n", cnu.col_mac);
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "register create cnu error: mac conflict !");
		return;
	}

	/* 判断是否达到用户上限*/
	idle = find_idle(cltid);
	inode = (cltid-1)*MAX_CNUS_PER_CLT+(idle-1);
	
	if( 0 == idle )
	{		
		/* CNU 表已满，禁止添加*/
		printf("\r\n\r\n  create entry for cnu %s failed !\n", cnu.col_mac);
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "register create cnu error: no cnu entry !");
		return;
	}
	else
	{
		/* 需要添加至数据库以及内存中*/
		cnu.id = inode+1;		
		if( CMM_SUCCESS == db_new_cnu(cltid, idle, &cnu))
		{
			/* 同步数据*/
			this->tb_cnu[inode].DevType = WEC701_C4;
			memcpy((char *)(this->tb_cnu[inode].Mac), (const char *)(macaddr), 6);			
			this->tb_cnu[inode].online = 0;
			this->tb_cnu[inode].RxRate = 0;
			this->tb_cnu[inode].TxRate = 0;
			this->tb_cnu[inode].OnUsed = BOOL_TRUE;
			db_fflush();
			dbs_sys_log(dbsdev, DBS_LOG_INFO, "module register create entry for cnu success !");
			return;
		}
		else
		{
			printf("\r\n\r\n  create entry for cnu %s error !\n", cnu.col_mac);
			dbs_sys_log(dbsdev, DBS_LOG_ERR, "register create cnu error: system error !");
			return;
		}
	}	
}

int try_to_add_cnu(int cltid, T_MMEAD_CNU_INFO activeCnu)
{
	int idle = 0;
	int inode = 0;
	int invalidCnuAccessEn = BOOL_TRUE;
	uint8_t supCnuMac0[6] = {0x30, 0x71, 0xB2, 0x00, 0x00, 0x10};
	uint8_t supCnuMac1[6] = {0x00, 0x1E, 0xE3, 0x20, 0x11, 0x01};
	//uint8_t MA[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
	//uint8_t MB[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
	st_dbsCnu cnu;
	T_TOPOLOGY_INFO *this = &topEntry;

	//printf("\n@@try_to_add_cnu\n");
	
	if( !boardapi_isValidUnicastMacb(activeCnu.Mac) )
	{
		return 0;
	}
	
	/* 判断是否为非法用户*/
	if( ! boardapi_isCnuSupported(activeCnu.DevType) )
	{		
		/* 发送非法设备接入告警*/
		/* 通知告警管理模块*/
		if( !boardapi_isCnuTrusted(activeCnu.DevType))
		{
			lllegal_cnu_register_notification(cltid, activeCnu.Mac);	
		}
	}	

	/* 返回该CLT下一个可用的CNU索引*/
	idle = find_idle(cltid);
	inode = (cltid-1)*MAX_CNUS_PER_CLT+(idle-1);
	
	if( 0 == idle )
	{
		/* 发送CNU用户数量超限的告警*/
		cnu_exceed_notification(cltid);
		/* 禁止添加该设备*/
		if( CMM_SUCCESS != msg_reg_mmead_bootout_dev(this->tb_clt[cltid-1].Mac, activeCnu.Mac) )
		{
			
			lllegal_cnu_kick_off_notification(cltid, activeCnu.Mac, BOOL_FALSE);
		}
		else
		{
			lllegal_cnu_kick_off_notification(cltid, activeCnu.Mac, BOOL_TRUE);
		}
	}
	else
	{
		/* 先将该CNU的信息添加至网元数据库*/
		cnu.id = inode+1;
		cnu.col_model = activeCnu.DevType;
		sprintf(cnu.col_mac, "%02X:%02X:%02X:%02X:%02X:%02X", 
			activeCnu.Mac[0], activeCnu.Mac[1], activeCnu.Mac[2], 
			activeCnu.Mac[3], activeCnu.Mac[4], activeCnu.Mac[5]
		);
		//memcpy((char *)(cnu.mac), (const char *)(activeCnu.Mac), 6);
		debug_printf("try_to_add_cnu(%d, %d), mac[%s]\n", cltid, idle, cnu.col_mac);
		cnu.col_sts = 0;		
		/* 如果发现是超级终端则自动添加为具名用户*/
		if( (memcmp(supCnuMac0, activeCnu.Mac, 6) == 0) || (memcmp(supCnuMac1, activeCnu.Mac, 6) == 0))
		{
			cnu.col_auth = 1;	
			cnu.col_synch = BOOL_FALSE;
		}
		else
		{
			/* Modified by frank */
			//cnu.col_auth = 0;
			cnu.col_auth = 1;
			/* 解决移机后配置会被局端作为匿名用户而覆盖的问题*/
			/* 用户希望在A局点下开通之后直接拿到B局点下就可以使用*/
			/* 如下2种情况可以正常移机使用*/
			/* 1. 终端移至局点B时如果是一个新用户，此时配置不会覆盖*/
			/* 2. 终端移至局点B是如果是一个未经离线配置的离线用户时*/
			/* 如下2种情况仍然存在移机后配置被覆盖的情况:*/
			/* 1. 终端在局点B下已经是一个离线用户，并且进行过离线配置*/
			/* 2. 终端在局点B下已经是一个预开户的用户*/
			cnu.col_synch = BOOL_TRUE;
		}

		if(boardapi_isAr7400Device(activeCnu.DevType))
		{
			strcpy(cnu.col_ver, "AR7400-v7.1.1-1-X-FINAL");
		}
		else if(boardapi_isAr6400Device(activeCnu.DevType))
		{
			strcpy(cnu.col_ver, "INT6000-v4.1.0-0-2-X-FINAL");
		}
		else
		{
			strcpy(cnu.col_ver, "unknown");
		}
				
		cnu.col_rx = 0;
		cnu.col_tx = 0;
		strcpy(cnu.col_snr, "0%");
		strcpy(cnu.col_bpc, "0%");
		strcpy(cnu.col_att, "0dB");
		//cnu.col_synch = BOOL_FALSE;
		cnu.col_row_sts = BOOL_TRUE;

		if( 1 == cnu.col_auth )
		{
			if( CMM_SUCCESS == db_new_su(cltid, idle, &cnu))
			{
				/* 同步数据*/
				this->tb_cnu[inode].online = DEV_STS_OFFLINE;
				this->tb_cnu[inode].DevType = activeCnu.DevType;
				memcpy((char *)(this->tb_cnu[inode].Mac), (const char *)(activeCnu.Mac), 6);
				this->tb_cnu[inode].OnUsed = BOOL_TRUE;
				db_fflush();
			}
			else
			{
				idle = 0;
			}			
		}
		else
		{
			if( CMM_SUCCESS == db_new_cnu(cltid, idle, &cnu))
			{
				/* 同步数据*/
				this->tb_cnu[inode].online = DEV_STS_OFFLINE;
				this->tb_cnu[inode].DevType = activeCnu.DevType;
				memcpy((char *)(this->tb_cnu[inode].Mac), (const char *)(activeCnu.Mac), 6);
				this->tb_cnu[inode].OnUsed = BOOL_TRUE;
				db_fflush();
			}
			else
			{
				idle = 0;
			}
		}
	}
	return idle;
} 

/********************************************************************************************
*	函数名称:try_to_register_new_cun
*	函数功能:完成新发现设备的注册
*					1. 将新发现的设备基本信息添加至网元数据库
*					2. 调用do_cnu_register函数完成设备注册
*	返回值:
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
void try_to_register_new_cun(int cltid, T_MMEAD_CNU_INFO activeCnu)
{
	int idle = 0;
	int cnuid = 0;
	
	idle = try_to_add_cnu(cltid, activeCnu);
	if( idle )
	{
		cnuid = idle;
		do_cnu_register(cltid, cnuid, activeCnu);
	}
}

/********************************************************************************************
*	函数名称:do_clt_discorver
*	函数功能:
*	返回值:
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
void do_clt_discorver(int cltid, T_MMEAD_TOPOLOGY *plist)
{
	do_clt_register(cltid, plist->clt);
}

/********************************************************************************************
*	函数名称:do_cnu_dropped
*	函数功能:
*	返回值:
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
void do_cnu_dropped(int cltid, T_MMEAD_TOPOLOGY *plist)
{
	int i = 0;
	int j = 0;
	int cnuid = 0;
	int inode = 0;
	//int inode = (clt-1)*MAX_CNUS_PER_CLT;
	//int cnuid = inode + 1;
	BOOLEAN find_cnu = BOOL_FALSE;
	BOOLEAN isCnuOnused;
	T_TOPOLOGY_INFO *this = &topEntry;
	uint8_t null_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	/* 如果之前在线的CNU设备在plist中找不到，则说明该设备下线了*/
	
	for( i=0; i<MMEAD_MAX_CNU_NUM; i++ )
	{
		inode = (cltid-1)*MAX_CNUS_PER_CLT+i;
		cnuid = i+1;
		//printf("\n##do_cnu_dropped->isCnuIndexOnUsed()\n");
		isCnuOnused = isCnuIndexOnUsed(cltid, cnuid);
		/* 寻找前一次拓扑中在线的CNU设备*/
		if( boardapi_isValidUnicastMacb(this->tb_cnu[inode].Mac)
			&& (BOOL_TRUE == isCnuOnused) 
			&& (DEV_STS_ONLINE == this->tb_cnu[inode].online) )
		{
			find_cnu = BOOL_FALSE;
			for( j=0; j<plist->clt.NumStas; j++ )
			{
				if( memcmp((const char *)(plist->cnu[j].Mac), (const char *)(this->tb_cnu[inode].Mac), 6) == 0 )
				{
					find_cnu = BOOL_TRUE;
					break;
				}
			}
			if( !find_cnu )
			{
				/* 则说明该设备下线了*/				
				do_cnu_unregister(cltid, cnuid);
			}
		}
		//usleep(2000);
	}
}

/********************************************************************************************
*	函数名称:do_cnu_discorver
*	函数功能:
*	返回值:
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
void do_cnu_discorver(int cltid, T_MMEAD_TOPOLOGY *plist)
{
	int i = 0;
	int j = 0;	
	int cnuid = 0;
	int inode = 0;
	BOOLEAN isCnuOnused;
	BOOLEAN discover_new = BOOL_TRUE;		
	//uint8_t null_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	T_TOPOLOGY_INFO *this = &topEntry;

	/* 被发现的设备分为2种类型: */
	/* 1, 新发现的设备
	** 2, 以前为off-line但现在变为online的设备*/
	for( i=0; i<plist->clt.NumStas; i++ )
	{	
		discover_new = BOOL_TRUE;
		for( j=0; j<MAX_CNUS_PER_CLT; j++ )
		{
			inode = (cltid-1)*MAX_CNUS_PER_CLT+j;
			cnuid = j+1;
			if( 0 != memcmp((const char *)(plist->cnu[i].Mac), (const char *)(this->tb_cnu[inode].Mac), 6) )
			{
				continue;
			}
			else
			{
				//printf("\n##do_cnu_discorver->isCnuIndexOnUsed()\n");
				isCnuOnused = isCnuIndexOnUsed(cltid, cnuid);
				if( BOOL_TRUE == isCnuOnused )
				{
					/* 表示这不是一个新发现的设备*/
					discover_new = BOOL_FALSE;
					do_cnu_register(cltid, cnuid, plist->cnu[i]);
					//usleep(2000);
					break;
				}
			}
			#if 0
			/* 这段代码效率低，可以采用select 数据库一次就完成*/			
			usleep(2000);
			if( (memcmp((const char *)(plist->cnu[i].Mac), (const char *)(this->tb_cnu[j].Mac), 6) == 0) 
				&& (BOOL_TRUE == isCnuOnused) )
			{
				/* 表示这不是一个新发现的设备*/
				discover_new = BOOL_FALSE;
				do_cnu_register(1, j+1, plist->cnu[i]);
				break;
			}
			#endif
		}

		/* 发现了一个新CNU设备*/
		if( discover_new )
		{
			/* delete duplicate cnu in topEntry */
			if( CMM_SUCCESS != delete_cnu_entry(plist->cnu[i].Mac) )
			{
				dbs_sys_log(dbsdev, DBS_LOG_ERR, "register_new_cun: delete_cnu_entry failed");
			}
			else
			{
				try_to_register_new_cun(cltid, plist->cnu[i]);
			}
		}
	}
}

/********************************************************************************************
*	函数名称:pro_clt_dropped
*	函数功能:
*	返回值:
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
void pro_clt_dropped(uint32_t clt_index)
{
	int i = 0;
	
	for( i=0; i<MAX_CNUS_PER_CLT; i++ )
	{
		do_cnu_unregister(clt_index, i+1);
		//usleep(5000);
	}

	do_clt_unregister(clt_index);
}

/********************************************************************************************
*	函数名称:pro_top_sts_transition
*	函数功能:
*	返回值:
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
void pro_top_sts_transition(int clt, T_MMEAD_TOPOLOGY *plist)
{
	int i = 0;
	int cnuid; 
	/* 这个函数里面,CLT一定在线*/
	/* 如果之前CLT不在线，则需要写上线*/
	do_clt_discorver(clt, plist);


	/* 如果CNU较之前有状态变迁，需要做相应逻辑处理*/
	/* 先处理下线的设备*/	
	do_cnu_dropped(clt, plist);	

	/* 再处理上线的设备，分为2种:
	新发现的设备和以前为off-line但现在变为online的设备*/
	do_cnu_discorver(clt, plist);
}

/********************************************************************************************
*	函数名称:ProcessTopologyChange
*	函数功能:
*	返回值:
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
void ProcessTopologyChange(int clt, T_MMEAD_TOPOLOGY *plist)
{
	if( NULL == plist )
	{
		/* 此分支可能意味着CLT都下线了，但是这种情况很少发生的*/
		/* 只需要将所有在线的设备写下线并发告警*/
		pro_clt_dropped(clt);
		return;
	}
	else
	{
		/* 进入此分支意味着CLT一定在线，但CNU可能有其他状态变更*/
		/* plist 仅仅包含在线的设备*/	
		debug_print_top(plist);
		pro_top_sts_transition(clt, plist);
		return;		
	}
}

/********************************************************************************************
*	函数名称:ProcessRegist
*	函数功能:
*	返回值:
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
void ProcessRegist(void)
{
	/*  在进行发包测试时候，线卡会出现丢失的现象，在此添加线卡
	**  丢失次数计数，当连续十次线卡丢失我们才认为是真正的丢失
	**  故障，此时再进行线卡下线逻辑的处理*/
	int i = 0;
	int cltid = 0;
	int iFlag = 0;
	int cltLossTimes[MAX_CLT_AMOUNT_LIMIT];
	T_MMEAD_TOPOLOGY nelist;

	for( i=0; i<MAX_CLT_AMOUNT_LIMIT; i++ )
	{
		cltLossTimes[i] = 0;
	}
	
	while(1)
	{		
		if(iFlag)
		{			
			/* 发送心跳*/
			reg2alarm_send_heartbeat_notification();
			sleep(5);
		}
		else
		{
			/* 注册模块初始化时不发送心跳*/
			iFlag = 1;
		}
		
		sleep(REGISTER_POLL_INT);
		
		/* 读取外部模块请求事件*/
		ProcessExtReq();

		/* 如果REG_CLT_RESET 被置位，此处只处理了1个CLT复位*/
		for( i=0; i<MAX_CLT_AMOUNT_LIMIT; i++ )
		{
			cltid = i+1;
			if( REG_CLT_RESET == cltFlags[i] )
			{
				printf("\n-->register event call : reset clt\n");
				//还原标志位
				cltFlags[i] = 0;				
				/* 发送MME重启CLT */
				msg_reg_mmead_reset_eoc(topEntry.tb_clt[i].DevType, topEntry.tb_clt[i].Mac);
				/* 设备下线 */
				ProcessTopologyChange(cltid, NULL);				
			}
			else
			{
				/* 如果是有效的clt */
				if( !boardapi_isValidUnicastMacb(topEntry.tb_clt[i].Mac) )
				{
					continue;
				}
				/* 从MMEAD获取在线设备列表*/
				else if( msg_reg_mmead_get_nelist(topEntry.tb_clt[i].Mac, &nelist) != CMM_SUCCESS )
				{
					/* 可能意味着CLT都下线了，但是这种情况很少发生的*/
					cltLossTimes[i]++;
					if( cltLossTimes[i] > 5 )
					{
						cltLossTimes[i] = 0;
						printf("\nProcessRegist: loss clt/%d\n", cltid);
						ProcessTopologyChange(cltid, NULL);
					}
					continue;
				}
				else
				{
					/*  还原计数器*/
					if( 0 != cltLossTimes[i] )
					{
						/* 防止故障时告警发送过于频繁*/
						if( cltLossTimes[i] > 5 )
						{
							/*  产生一条线卡暂时丢失的告警*/
							clt_heartbeat_loss_notification(cltid, cltLossTimes[i]);
						}
						cltLossTimes[i] = 0;
					}
					/* 与上一次的拓扑信息相比较，处理状态变迁的节点*/
					debug_printf("\n\nmsg_reg_mmead_get_nelist(%02X:%02X:%02X:%02X:%02X:%02X)\n", 
						topEntry.tb_clt[i].Mac[0], topEntry.tb_clt[i].Mac[1], topEntry.tb_clt[i].Mac[2], 
						topEntry.tb_clt[i].Mac[3], topEntry.tb_clt[i].Mac[4], topEntry.tb_clt[i].Mac[5]
					);
					ProcessTopologyChange(cltid, &nelist);					
				}
			}
		}
		db_real_fflush();
	}
}

int get_req_ext(void)
{
	BBLOCK_QUEUE *this = &bblock;
	T_Msg_CMM *req = NULL;
	stTmUserInfo *pIndex = NULL;
	int FromAddrSize = 0;	
	fd_set fdsr;
	int maxsock;
	struct timeval tv;

	memcpy((char *)&(this->sk), (const char *)&SK_REGI, sizeof(T_UDP_SK_INFO));
	bzero(this->b, MAX_UDP_SIZE);

	// initialize file descriptor set
	FD_ZERO(&fdsr);
	FD_SET(this->sk.sk, &fdsr);

	// timeout setting
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	maxsock = this->sk.sk;
	if( select(maxsock + 1, &fdsr, NULL, NULL, &tv) > 0 )
	{
		if (FD_ISSET(this->sk.sk, &fdsr))
		{
			FromAddrSize = sizeof(this->sk.skaddr);
			this->blen = recvfrom(this->sk.sk, this->b, MAX_UDP_SIZE, 0, 
				(struct sockaddr *)&(this->sk.skaddr), &FromAddrSize);
			if(this->blen>0)
			{
				req = (T_Msg_CMM *)(this->b);
				pIndex = (stTmUserInfo *)(req->BUF);
				if(req->HEADER.usMsgType == REG_CNURESET)
				{
					//设置CNU强制重启标志位
					cnuFlags[(pIndex->clt-1)*MAX_CNUS_PER_CLT+(pIndex->cnu-1)] = REG_CNURESET;
					return CMM_SUCCESS;
				}
				else if( REG_CNU_FORCE_REGISTRATION == req->HEADER.usMsgType )
				{
					//设置CNU强制重新注册标志位
					cnuFlags[(pIndex->clt-1)*MAX_CNUS_PER_CLT+(pIndex->cnu-1)] = REG_CNU_FORCE_REGISTRATION;
					/* 将标志位csyncStatus 置0 */
					set_cnu_pro_sync(pIndex->clt, pIndex->cnu, BOOL_FALSE);
			//		printf("   pIndex->clt=%d\n",pIndex->clt);
			//		printf("   pIndex->cnu=%d\n",pIndex->cnu);
					return CMM_SUCCESS;
				}
				else if( REG_CNU_DELETE == req->HEADER.usMsgType )
				{
					//printf("\r\n  register event call : delete clt %d cnu %d\n", pIndex->clt, pIndex->cnu);
					/* 在内存中删除该设备的信息*/
					do_cnu_delete(pIndex->clt, pIndex->cnu);
					cnuFlags[(pIndex->clt-1)*MAX_CNUS_PER_CLT+(pIndex->cnu-1)] = 0;
					return CMM_SUCCESS;					
				}
				else if( REG_CLT_RESET == req->HEADER.usMsgType )
				{
					/* 设置CLT 强制重启标志位*/
					cltFlags[pIndex->clt-1] = REG_CLT_RESET;
					return CMM_SUCCESS;
				}
				else if( REG_CNU_CREATE == req->HEADER.usMsgType )
				{
					uint8_t new_cnu_mac[6] = {0};
					memcpy(new_cnu_mac, req->BUF, 6);
					/* 将CNU 添加至网元数据库*/
					do_create_cnu(new_cnu_mac);
					return CMM_SUCCESS;
				}
			}
		}
	}
	
	return CMM_FAILED;
}

void ProcessExtReq(void)
{
	int ret = CMM_SUCCESS;

	//清空标志位
	bzero(cltFlags, sizeof(cltFlags));
	bzero(cnuFlags, sizeof(cnuFlags));
	//printf("\n======== begin ProcessExtReq ========\n");
	do
	{
		ret = get_req_ext();
	}while( CMM_SUCCESS == ret );
	//printf("\n======== end ProcessExtReq ========\n");	
}

int msg_regi_init(void)
{
	T_UDP_SK_INFO *sk = &SK_REGI;
	struct sockaddr_in server_addr;
	//int iMode = 1;

	/*创建UDP SOCKET接口*/
	if( ( sk->sk = socket(PF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		return CMM_CREATE_SOCKET_ERROR;
	}	
	
	bzero((char *)&(sk->skaddr), sizeof(sk->skaddr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(REG_CORE_LISTEN_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sk->sk, (struct sockaddr*)&server_addr, sizeof(server_addr))<0)
	{
		return CMM_CREATE_SOCKET_ERROR;
	}

	/* 设置SOCKET为非阻塞模式*/
	//fcntl(sk->sk, F_SETFL, O_NONBLOCK);
	
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:init_nelib
*	函数功能:
*	返回值:
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int init_nelib(void)
{
	int i = 0;
	int j = 0;
	int inode = 0;
	int cltdetected = 0;
	uint8_t null_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	st_dbsClt clt;
	T_TOPOLOGY_INFO *topology = &topEntry;

	bzero( topology, sizeof(T_TOPOLOGY_INFO));
	
	/* 从数据库中将原始网元信息初始化到全局数组*/
	if( db_init_nelib(topology) != CMM_SUCCESS )
	{
		printf("init_nelib : CMM_FAILED");
		return CMM_FAILED;
	}
	else
	{
		/* 网元数据库中记录的设备初始状态要全部初始化为off-line*/
		/* 网元库中其他实时的动态数据也要初始化为0 */
		if( CMM_SUCCESS != db_init_all() )
		{
			printf("init_nelib : CMM_FAILED");
			return CMM_FAILED;
		}
		else
		{
			for( i=0; i<MAX_CLT_AMOUNT_LIMIT; i++ )
			{
				topology->tb_clt[i].online = 0;
				topology->tb_clt[i].NumStas = 0;
				for( j=0; j<MAX_CNUS_PER_CLT; j++ )
				{
					inode = i*MAX_CNUS_PER_CLT+j;
					topology->tb_cnu[inode].online = DEV_STS_OFFLINE;
					topology->tb_cnu[inode].RxRate = 0;
					topology->tb_cnu[inode].TxRate = 0;
				}
			}			
		}
	}	
	
	/* get clt info and save to dbs */
	for( i=0; i<MAX_CLT_AMOUNT_LIMIT; i++ )
	{
		clt.id = i+1;
		strcpy(clt.col_mac, "00:00:00:00:00:00");
		clt.col_sts = DEV_STS_OFFLINE;
		clt.col_maxStas = MAX_CNUS_PER_CLT;
		clt.col_numStas = 0;
		strcpy(clt.col_swVersion, "AR7400-v7.1.1-1-X-FINAL");
		clt.col_synch = 0;		
		
		/* 每次启动时都重新搜索每个端口的clt */		
		/* check clt port link status first */
		if( 0 == reg2cmm_getCltPortLinkStatus(&SK_REG2CMM, clt.id) )
		{
				printf("probe clt%d: no clt present\r\n", i+1);
				/* delete this clt in dbs */
				clt.col_row_sts = 0;
				db_update_clt(i+1, &clt);
				/* 发送不能发现线卡的异常告警*/
				clt_cannot_finded_notification(i+1);
				continue;
		}
		else 
		{
			/* bingding atheros address to clt port i */
			if( CMM_SUCCESS != reg2cmm_bindingAtheroesAddr2CablePort(&SK_REG2CMM, clt.id) )
			{
				printf("register binding clt%d error\n", i+1);
				/* delete this clt in dbs */
				clt.col_row_sts = 0;
				db_update_clt(i+1, &clt);
				/* 发送不能发现线卡的异常告警*/
				clt_cannot_finded_notification(i+1);
				continue;
			}
			/* 获取CLT的MAC地址并写入数据库*/
			else if( msg_reg_mmead_get_clt(&(topology->tb_clt[i])) != CMM_SUCCESS )
			{
				printf("probe clt%d: no clt present\r\n", i+1);
				/* delete this clt in dbs */
				clt.col_row_sts = 0;
				db_update_clt(i+1, &clt);
				/* 发送不能发现线卡的异常告警*/
				clt_cannot_finded_notification(i+1);				
			}
			else
			{				
				cltdetected++;				
				/* 在这里并不做设备上线的处理*/
				/* 仅仅只是将CLT的MAC地址写入数据库*/				
				clt.col_model = topology->tb_clt[i].DevType;
				sprintf(clt.col_mac, "%02X:%02X:%02X:%02X:%02X:%02X", 
					topology->tb_clt[i].Mac[0], topology->tb_clt[i].Mac[1], topology->tb_clt[i].Mac[2], 
					topology->tb_clt[i].Mac[3], topology->tb_clt[i].Mac[4], topology->tb_clt[i].Mac[5]
				);				
				clt.col_row_sts = 1;		
				db_update_clt(i+1, &clt);
				printf("probe clt/%d[%s]: success\n", i+1, clt.col_mac);
			}
		}
			
	}
		
	db_fflush();
	//return cltdetected?CMM_SUCCESS:CMM_FAILED;
	return CMM_SUCCESS;
}

/********************************************************************************************
*	函数名称:main
*	函数功能:
*	返回值:
*	作者:frank
*	时间:2010-07-23
*********************************************************************************************/
int main(void)
{	
	/*创建与数据库模块通讯的外部SOCKET接口*/
	dbsdev = reg_dbsOpen();
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: register->dbsOpen error, exited !\n");
		return CMM_CREATE_SOCKET_ERROR;
	}
	
	/* Waiting for mmead init */
	dbsWaitModule(dbsdev, MF_CMM|MF_MMEAD|MF_ALARM|MF_TM);
	
	/*创建与MMEAD模块通讯的外部SOCKET接口*/
	if( CMM_SUCCESS != msg_mmead_init() )
	{
		perror("Register->msg_mmead_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module register msg_mmead_init error, exited !");
		reg_dbsClose();
		return CMM_CREATE_SOCKET_ERROR;
	}

	/*创建与告警模块通讯的外部SOCKET接口*/
	if( CMM_SUCCESS != msg_alarm_init() )
	{
		perror("Register->msg_alarm_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module register msg_alarm_init error, exited !");
		reg_dbsClose();
		return CMM_CREATE_SOCKET_ERROR;
	}	

	/*创建与模板管理模块通讯的外部SOCKET接口*/
	if( CMM_SUCCESS != msg_tm_init() )
	{
		perror("Register->msg_tm_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module register msg_tm_init error, exited !");
		reg_dbsClose();
		return CMM_CREATE_SOCKET_ERROR;
	}

	/*创建与模板管理模块通讯的外部SOCKET接口*/
	if( CMM_SUCCESS != reg2cmm_init(&SK_REG2CMM) )
	{
		perror("Register->reg2cmm_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module register reg2cmm_init error, exited !");
		reg_dbsClose();
		return CMM_CREATE_SOCKET_ERROR;
	}

	/* 创建处理外部请求的SOCKET接口*/
	if( CMM_SUCCESS != msg_regi_init() )
	{
		perror("Register->msg_regi_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module register msg_regi_init error, exited !");
		reg_dbsClose();
		return CMM_CREATE_SOCKET_ERROR;
	}

	/* 注册异常退出句柄函数*/
	signal(SIGTERM, RegSignalProcessHandle);

	/* 发送系统启动的告警*/
	cbat_system_sts_notification(1);

	/* step 1:模块启动时初始化，获取DB中原始拓扑信息*/
	if( init_nelib() != CMM_SUCCESS )
	{
		perror("module register done: no clt detected\n");
		dbs_sys_log(dbsdev, DBS_LOG_WARNING, "module register done: no clt detected");
		reg2cmm_destroy(&SK_REG2CMM);
		msg_tm_destroy();
		msg_alarm_destroy();
		msg_mmead_destroy();
		reg_dbsClose();
		return CMM_FAILED;
	}

	fprintf(stderr, "Starting module Register	......		[OK]\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting module register success");
	
	/* 循环处理注册事件*/
	ProcessRegist();

	/* 不要在这个后面添加代码，执行不到滴*/
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "SignalProcessHandle : module register exit");
	msg_mmead_destroy();
	msg_alarm_destroy();
	msg_tm_destroy();
	reg_dbsClose();
	
	return 0;
}

