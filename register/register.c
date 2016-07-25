/*****************************************************************************************
  �ļ����� : register.c
  �ļ����� : registerģ������ں���
  				��ģ��Ϊ��̨���̣������ڻ�ȡ������Ϣ��������
  				���ݿ⣻�������˱�Ǩʱ֪ͨ�澯����ģ�鴦��澯
  				�������Զ����ú��Զ��������̡�
  				���漰�����ֽ����������ֽ����ת��
  �޶���¼ :
           1 ���� : frank
             ���� : 2010-07-29
             ���� : �����ļ�

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
*	��������:debug_dump_msg
*	��������:������API����ʮ�����Ƶķ�ʽ�������������������
*				   �ļ�ָ��fpָ����豸�ļ�
*	����:frank
*	ʱ��:2010-08-13
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
*	��������:debug_print_top
*	��������:������API����ӡ��MMEADģ���ȡ��ʵʱ������Ϣ
*	����:frank
*	ʱ��:2010-08-13
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
*	��������:SignalProcessHandle
*	��������:ע��ģ���쳣����������
*	����:frank
*	ʱ��:2010-08-13
*********************************************************************************************/
void RegSignalProcessHandle(int n)
{
	/* ����ϵͳ�رյĸ澯*/
	cbat_system_sts_notification(0);
	/* �ر�socket�ӿ� */
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
*	��������:find_idle
*	��������:��ָ��CLT��CNU�б��в��ҵ�һ�����е�index
*	����ֵ:�ɹ�:���ظ�CLT������һ�����õ�index, ʧ��:����0
*	����:frank
*	ʱ��:2010-07-23
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
	/* �������Ǳ�ڵ�©��*/
	/* ���MAC��ַ��ͬ�ķǷ��û�������Ҳ�ܻ��ȥ*/
	/* ���ﲻӦ�ø����豸���ͣ�������ֳ�ͻ��澯����*/
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
		/* дϵͳ��־*/
		dbs_sys_log(dbsdev, DBS_LOG_WARNING, "refresh_signon_cnu encountered cnu with conflict mac address");
		/* ���͸澯*/
		fprintf(stderr, "refresh_signon_cnu mac address conflicting !\n");
	}
	/*****************************************************************************************/
	/*****************************************************************************************/
	else
	{
		/* ������Ԫ���ݿ�*/
		cnu.id = cnuid;
		cnu.col_sts = DEV_STS_ONLINE;
		//cnu.col_model = activeCnu->DevType;
		cnu.col_rx = activeCnu->AvgPhyRx;
		cnu.col_tx = activeCnu->AvgPhyTx;
		if( CMM_SUCCESS == db_update_cnu(clt_index, cnu_index, &cnu))
		{
			/* ͬ���������ڴ����ݱ�*/
			//this->tb_cnu[inode].DevType = activeCnu->DevType;
			this->tb_cnu[inode].online = DEV_STS_ONLINE;
			this->tb_cnu[inode].RxRate = activeCnu->AvgPhyRx;
			this->tb_cnu[inode].TxRate = activeCnu->AvgPhyTx;
			/* ֪ͨ�澯����ģ��*/
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
	
	/* ���MAC��ַ��ͬ�����豸���Ͳ�ͬ��CNU �Զ����õ�BUG */
	/* ��:�ڸ�CBAT��������MACΪ3071b2000010��WEC-3702I���룬���豸
	����֮������ͬ��MAC��ַ�������豸����ΪWEC-604���豸
	ע�ᣬ���ڴ�ʱ���豸�����ͻ�û��д�����ݿ⣬TMCoreģ��
	��Ȼ��Ϊ���豸ΪWEC-3702I��������CRC�Լ�PIBʱ����Ȼ��Ϊ
	���豸ΪWEC-3702I�������ʱ�����ɵ�PIBд���豸���������
	�豸ԭʼ�豸���͸���ΪWEC-3702I��BUG��*/
	/* ���������İ취:�·�����֮ǰ������ݿ��е��豸����
	�ֶ��Ƿ���activeCnu�ṹ�е�һ�£���һ�����·����ã�����
	һ�����͸澯����ֹ�·�����*/
	if( CMM_SUCCESS != db_get_cnu(clt_index, cnu_index, &cnu) )
	{
		perror("ERROR: do_cnu_auto_config->db_get_cnu !\n");
		/* ����CNU�����Զ����õĸ澯*/
		cnu_abort_auto_config_notification(clt_index, cnu_index);
		return;
	}
	else
	{
		if( cnu.col_model != activeCnu->DevType )
		{
			/*****************************************************************************************/
			/* �������Ǳ�ڵ�©��*/
			/* ���MAC��ַ��ͬ�ķǷ��û�������Ҳ�ܻ��ȥ*/
			/*****************************************************************************************/
			#if 0
			/* ���ﻹ��Ǳ�ڵ�©��*/
			/* ���MAC��ַ��ͬ�ķǷ��û�������Ҳ�ܻ��ȥ*/
			refresh_signon_cnu(clt_index, cnu_index, activeCnu);
			/* ����CNU�����Զ����õĸ澯*/
			cnu_abort_auto_config_notification(clt_index, cnu_index);
			return;
			#endif
			
			/* ������ȷ������Ӧ����: */
			
			/* 1. �����ݿ���ڴ��и��豸��״̬����Ϊ����״̬��*/
			this->tb_cnu[inode].online = DEV_STS_OFFLINE;
			this->tb_cnu[inode].RxRate = 0;
			this->tb_cnu[inode].TxRate = 0;

			cnu.col_sts = DEV_STS_OFFLINE;
			cnu.col_rx = 0;
			cnu.col_tx = 0;
			
			/* 2. ������ֵ��豸���ͺϷ���������豸�����ͣ�*/
			if( boardapi_isCnuSupported(activeCnu->DevType) )
			{
				this->tb_cnu[inode].DevType = activeCnu->DevType;
				cnu.col_model = activeCnu->DevType;
				/* ����ٶ��豸���ͱ����һ�������·�����*/
				cnu.col_synch = 0;
				if( CMM_SUCCESS != dbsUpdateCnu(dbsdev, cnuid, &cnu) )
				{
					perror("ERROR: do_cnu_auto_config->dbsUpdateCnu !\n");
					/* ����CNU�����Զ����õĸ澯*/
					cnu_abort_auto_config_notification(clt_index, cnu_index);
					return;
				}
				/* �����WEC701ϵ�У�����Ҫ����profile->base_pib */
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
							/* ����CNU�����Զ����õĸ澯*/
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
				/* дϵͳ��־*/
				dbs_sys_log(dbsdev, DBS_LOG_WARNING, "do_cnu_auto_config encountered cnu with conflict mac address");
				/* ��������������Զ���������*/
			}			
			/* 3. ������ֵ��豸���Ͳ��Ϸ��������ߣ�*/
			else
			{
				/* ���ͷǷ��豸����澯*/
				/* ֪ͨ�澯����ģ��*/
				lllegal_cnu_register_notification(clt_index, activeCnu->Mac);
				#if 0
				/* ��ֹ��Ӹ��豸*/				
				if( CMM_SUCCESS != msg_reg_mmead_bootout_dev(this->tb_clt.Mac, activeCnu->Mac) )
				{					
					lllegal_cnu_kick_off_notification(1, activeCnu->Mac, BOOL_FALSE);
				}
				else
				{
					lllegal_cnu_kick_off_notification(1, activeCnu->Mac, BOOL_TRUE);
				}
				#endif
				/* дϵͳ��־*/
				dbs_sys_log(dbsdev, DBS_LOG_WARNING, "do_cnu_auto_config encountered invalid cnu with conflict mac address");
				/* �˳��Զ���������*/
				return;
			}
			/*****************************************************************************************/
			/*****************************************************************************************/
		}
	}

	if( !boardapi_isCnuSupported(activeCnu->DevType) )
	{
		/* �������͵�CNU���������κ��Զ�����*/
		refresh_signon_cnu(clt_index, cnu_index, activeCnu);
		/* ����CNU�����Զ����õĸ澯*/
		cnu_abort_auto_config_notification(clt_index, cnu_index);
		return;
	}
	
	/* ��ģ�����ģ���ȡ���û���PIB CRC */
	if( CMM_SUCCESS != tm_get_user_pib_crc(clt_index, cnu_index, &PIB_CRC) )
	{
		perror("ERROR: do_cnu_auto_config->tm_get_user_pib_crc !\n");
		/* ����CNU�����Զ����õĸ澯*/
		cnu_abort_auto_config_notification(clt_index, cnu_index);
		return;
	}
/* ���Ե�ʱ��һ�㲻ϣ���·�PIB�����д��޸�*/
/****************************************************************************************/
#if 1
	if( BOOL_TRUE == cnu.col_synch )
	{
		refresh_signon_cnu(clt_index, cnu_index, activeCnu);
	}
	else
	{
		/* ֪ͨģ�����ģ���Զ����ɸ��û���MOD */
		if( CMM_SUCCESS != tm_gen_user_mod(clt_index, cnu_index) )
		{
			perror("ERROR: do_cnu_auto_config->tm_gen_user_mod !\n");
			/* ����CNU�����Զ����õĸ澯*/
			cnu_abort_auto_config_notification(clt_index, cnu_index);
			return;
		}
		/* �Զ��·�MOD */
		if( CMM_SUCCESS == msg_reg_mmead_wr_user_mod(activeCnu->DevType, activeCnu->Mac) )
		{
			/* ���͸澯*/
			cnu_auto_config_notification(clt_index, cnu_index, cnu_index, 1, BOOL_TRUE);
			/* ����־λcsyncStatus ��1 */
			set_cnu_pro_sync(clt_index, cnu_index, BOOL_TRUE);
		}
		else
		{
			/* ���͸澯*/
			cnu_auto_config_notification(clt_index, cnu_index, cnu_index, 1, BOOL_FALSE);
		}
		/* ֪ͨģ�����ģ�����ٸ��û�������*/
		if( CMM_SUCCESS != tm_distroy_user_mod(clt_index, cnu_index) )
		{
			perror("ERROR: do_cnu_auto_config->tm_distroy_user_mod !\n");
			return;
		}
		
	}
#else
	/* for debug */
	//printf("\ndo_cnu_auto_config(): PIB_CRC = 0x%X, activeCnu->CRC = 0x%X\n", PIB_CRC, activeCnu->CRC[0]);
	
	/* �Ƚ�PIB CRC У����*/
	if( PIB_CRC == activeCnu->CRC[0] )
	//if(1)	/* PIB CRC ������ȷ��ȡ�������θöδ���*/
	{
		#if 0
		/* ��ģ�����ģ���ȡ���û���MOD CRC */
		if( CMM_SUCCESS != tm_get_user_mod_crc(clt_index, cnu_index, &MOD_CRC) )
		{
			perror("ERROR: do_cnu_auto_config->tm_get_user_mod_crc !\n");
			return;
		}
		
		/* �Ƚ�MOD CRC У����*/
		if( MOD_CRC == activeCnu->CRC[1] )
		#endif
		
		if( BOOL_TRUE == cnu.col_synch )
		{
			refresh_signon_cnu(clt_index, cnu_index, activeCnu);
		}
		else
		{
			/* ֪ͨģ�����ģ���Զ����ɸ��û���MOD */
			if( CMM_SUCCESS != tm_gen_user_mod(clt_index, cnu_index) )
			{
				perror("ERROR: do_cnu_auto_config->tm_gen_user_mod !\n");
				/* ����CNU�����Զ����õĸ澯*/
				cnu_abort_auto_config_notification(clt_index, cnu_index);
				return;
			}
			if( CMM_SUCCESS == msg_reg_mmead_wr_user_mod(activeCnu->DevType, activeCnu->Mac) )
			{
				/* ���͸澯*/
				cnu_auto_config_notification(clt_index, cnu_index, cnu_index, 1, BOOL_TRUE);
				/* ����־λcsyncStatus ��1 */
				set_cnu_pro_sync(clt_index, cnu_index, BOOL_TRUE);
			}
			else
			{
				/* ���͸澯*/
				cnu_auto_config_notification(clt_index, cnu_index, cnu_index, 1, BOOL_FALSE);
			}
			/* ֪ͨģ�����ģ�����ٸ��û�������*/
			if( CMM_SUCCESS != tm_distroy_user_mod(clt_index, cnu_index) )
			{
				perror("ERROR: do_cnu_auto_config->tm_distroy_user_mod !\n");
				return;
			}			
		}
	}
	else
	{
		/* ֪ͨģ�����ģ���Զ����ɸ��û���PIB */
		if( CMM_SUCCESS != tm_gen_user_pib(clt_index, cnu_index) )
		{
			perror("ERROR: do_cnu_auto_config->tm_gen_user_pib !\n");
			return;
		}
		/* �Զ��·�PIB */
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
			/* ���͸澯*/
			cnu_auto_config_notification(clt_index, cnu_index, cnu_index, 0, BOOL_TRUE);
		}
		else
		{
			/* ���͸澯*/
			cnu_auto_config_notification(clt_index, cnu_index, cnu_index, 0, BOOL_FALSE);
		}
		/* ֪ͨģ�����ģ�����ٸ��û�������*/
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
			/* ͬ������*/
			this->tb_cnu[inode].RxRate = activeCnu.AvgPhyRx;
			this->tb_cnu[inode].TxRate = activeCnu.AvgPhyTx;
		}		
	}

	/* ���CNU���ٰ���Դ���µ�BUG */
	//if( !isCnuAuthorized(clt_index, cnu_index) )
	//{
	//	msg_reg_mmead_block_user(activeCnu.DevType, activeCnu.Mac);
	//}	
	
}

/********************************************************************************************
*	��������:do_clt_register
*	��������:CLTע�ắ�����ú������CLT����ʱ��ע�����
*					1. ���CLT��Ϣ�����ݿ��е�ע��;
*					2. ��ע���ȡ��ʵʱ��Ϣͬ�����ڴ�;
*					3. ��ɸ澯֪ͨ��
*					4. ����Զ����ã�
*	ע��:���øú�������ע����豸һ�������ݿ���Ԫ����
*			�Ѿ����ڵ��豸�������·��ֵ��豸�ڵ��øú���
*			����ע��ʱ����Ҫ�Ƚ��豸�ı�ʶ��Ϣд�����ݿ�
*	����:frank
*	ʱ��:2010-08-13
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
			/* ͬ������*/
			this->tb_clt[clt_index-1].online = DEV_STS_ONLINE;
			this->tb_clt[clt_index-1].DevType = activeClt.DevType;
			this->tb_clt[clt_index-1].NumStas = activeClt.NumStas;
			/* ֪ͨ�澯����ģ��*/
			clt_sts_transition_notification(clt_index, DEV_STS_ONLINE);			
			/* ֪ͨ�Զ�����ģ��*/
			/* ֪ͨ�Զ�����ģ��*/
			/* ����ȱʧ*//* �ݲ�֧��CLT�Զ�����*/
		}
		else
		{
			perror("ERROR: do_clt_register->db_update_clt !\n");
		}
	}
	else
	{
		/* ������Ԫ���ݱ��е�NumStas �ֶ�*/
		if( this->tb_clt[clt_index-1].NumStas != activeClt.NumStas )
		{
			clt.id = clt_index;
			clt.col_sts = DEV_STS_ONLINE;
			clt.col_numStas = activeClt.NumStas;
			if( CMM_SUCCESS == db_update_clt(clt_index, &clt))
			{
				/* ͬ������*/
				this->tb_clt[clt_index-1].NumStas = activeClt.NumStas;
			}
		}
	}
}

/********************************************************************************************
*	��������:do_clt_unregister
*	��������:CLT��ע�ắ�����ú������CLT����ʱ�ķ�ע�����
*					1. ���CLT��Ϣ�����ݿ��еķ�ע��;
*					2. ����ע����Ϣͬ�����ڴ�;
*					3. ��ɸ澯֪ͨ��
*	ע��:���øú������з�ע����豸һ�������ݿ���Ԫ����
*			�Ѿ����ڵ��豸
*	����:frank
*	ʱ��:2010-08-13
*********************************************************************************************/
void do_clt_unregister(uint32_t clt_index)
{
	int inode = clt_index-1;
	T_TOPOLOGY_INFO *this = &topEntry;
	
	if( DEV_STS_OFFLINE != this->tb_clt[inode].online )
	{
		/* д���ݿ�*/
		if( CMM_SUCCESS == db_unregister_clt(clt_index) )
		{
			/* ͬ������*/
			this->tb_clt[inode].online = DEV_STS_OFFLINE;
			this->tb_clt[inode].NumStas = 0;
			/* ֪ͨ�澯����ģ��*/
			clt_sts_transition_notification(clt_index, DEV_STS_OFFLINE);
		}
		else
		{
			perror("ERROR: do_clt_unregister->db_unregister_clt !\n");
		}
	}
}

/********************************************************************************************
*	��������:do_cnu_register
*	��������:CNUע�ắ�����ú������CNU����ʱ��ע�����
*					1. ���CNU��Ϣ�����ݿ��е�ע��;
*					2. ��ע���ȡ��ʵʱ��Ϣͬ�����ڴ�;
*					3. ��ɸ澯֪ͨ��
*					4. ����Զ����ã�
*	ע��:���øú�������ע����豸һ�������ݿ���Ԫ����
*			�Ѿ����ڵ��豸�������·��ֵ��豸�ڵ��øú���
*			����ע��ʱ����Ҫ�Ƚ��豸�ı�ʶ��Ϣд�����ݿ�
*	����:frank
*	ʱ��:2010-08-13
*********************************************************************************************/
void do_cnu_register(uint32_t clt_index, uint32_t cnu_index, T_MMEAD_CNU_INFO activeCnu)
{
	int autoCfgSts = 0;
	//uint32_t userType = 0;
	//st_dbsCnu cnu;

	int inode = (clt_index-1)*MAX_CNUS_PER_CLT+(cnu_index-1);
	T_TOPOLOGY_INFO *this = &topEntry;
	
	/* ���øú�������ע���CNUһ���ǺϷ����豸����Ϊ�Ƿ�
	** CNU��ע��֮ǰ�ͽ�ֹ������û����������ᴥ��ע�����*/

	/* ������ⲿ����ע������*/
	if( REG_CNURESET == cnuFlags[inode] )
	{
		//printf("\r\n  register event call : reset clt %d cnu %d\n", clt_index, cnu_index);
		
		/* ����MME����CNU*/
		msg_reg_mmead_reset_eoc(activeCnu.DevType, activeCnu.Mac);

		/* ����CNUǿ������*/
		do_cnu_unregister(clt_index, cnu_index);
		
		//��ԭ��־λ
		cnuFlags[inode] = 0;

		return;
	}
	else if( REG_CNU_FORCE_REGISTRATION == cnuFlags[inode] )
	{
		//printf("\r\n  register event call : force clt %d cnu %d re-registration\n", clt_index, cnu_index);
		
		/* ����CNUǿ������ע��ĸ澯*/
		cnu_re_register_notification(clt_index, cnu_index);
		
		/* ��ȡ��CNU����Ȩ״̬*/
		//if( isCnuAuthorized(clt_index, cnu_index) )
		//{
			//����CNU�û��˿�
		//	msg_reg_mmead_enable_user(activeCnu.DevType, activeCnu.Mac);
		//}
		//else
		//{
			//�ر�CNU�û��˿�
		//	msg_reg_mmead_block_user(activeCnu.DevType, activeCnu.Mac);
		//}
		/* ����CNUǿ������*/
		do_cnu_unregister(clt_index, cnu_index);
		/* ����־λcsyncStatus ��0 */
		//set_cnu_pro_sync(clt_index, cnu_index, BOOL_FALSE);

		//��ԭ��־λ
		cnuFlags[inode] = 0;
		
		return;
	}
	
	
	/* ������豸��״̬��Ǩ����д���*/
	if( DEV_STS_OFFLINE == this->tb_cnu[inode].online )
	{
		#if 0
		/* ��ȡ��CNU���û�����*/
		if( CMM_SUCCESS == db_get_user_type(clt_index, cnu_index, &userType))
		{
			if( 0 == userType )
			{
				/* �����û�ע��*/
				printf("\nclt %d cnu %d do anonymous registration\n", clt_index, cnu_index);
				do_anonymous_register(clt_index, cnu_index, activeCnu);
			}
			else
			{
				/* ֪���û�ע��*/
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
		/* ��ȡȫ���Զ�����ʹ��״̬*/
		
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
				//���ﻹ��Ǳ�ڵ�©��
				// ���MAC��ַ��ͬ�ķǷ��û�������Ҳ�ܻ��ȥ
				refresh_signon_cnu(clt_index, cnu_index, &activeCnu);
			}
			else
			{
				// �Զ�����
				do_cnu_auto_config(clt_index, cnu_index, &activeCnu);
			}
		 }
		    
	}
	else
	{
		/* ������Ԫ���ݿ��е�ʵʱ����*/
		/* ���ﻹ��Ǳ�ڵ�©��*//* ����Ҫ�����ܿ���ܴ���*/
		/* ���MAC��ַ��ͬ�ķǷ��û�������Ҳ�ܻ��ȥ*/
		refresh_active_cnu(clt_index, cnu_index, activeCnu);
	}
	
}

void do_cnu_delete(uint32_t clt_index, uint32_t cnu_index)
{
	int inode = (clt_index-1)*MAX_CNUS_PER_CLT+(cnu_index-1);
	T_TOPOLOGY_INFO *this = &topEntry;

	/* ͬ������*/
	this->tb_cnu[inode].online = DEV_STS_OFFLINE;
	this->tb_cnu[inode].RxRate = 0;
	this->tb_cnu[inode].TxRate = 0;
	this->tb_cnu[inode].OnUsed = 0;
	bzero(this->tb_cnu[inode].Mac, 6);

	//db_unregister_cnu(cnu_index);	
	/* ���ݱ�����TM���Ѿ���ɾ��*/
	//db_delete_cnu(cnu_index);
	db_fflush();
}


/********************************************************************************************
*	��������:do_cnu_unregister
*	��������:CNU��ע�ắ�����ú������CNU����ʱ�ķ�ע�����
*					1. ���CNU��Ϣ�����ݿ��еķ�ע��;
*					2. ����ע����Ϣͬ�����ڴ�;
*					3. ��ɸ澯֪ͨ��
*	ע��:���øú������з�ע����豸һ�������ݿ���Ԫ����
*			�Ѿ����ڵ��豸
*	����:frank
*	ʱ��:2010-08-13
*********************************************************************************************/
void do_cnu_unregister(uint32_t clt_index, uint32_t cnu_index)
{
	int inode = (clt_index-1)*MAX_CNUS_PER_CLT+(cnu_index-1);
	int cnuid = inode + 1;
	T_TOPOLOGY_INFO *this = &topEntry;
	
	if( DEV_STS_OFFLINE != this->tb_cnu[inode].online )
	{
		/* д���ݿ�*/
		if( CMM_SUCCESS == db_unregister_cnu(clt_index, cnu_index) )
		{
			/* ͬ������*/
			this->tb_cnu[inode].online = DEV_STS_OFFLINE;
			this->tb_cnu[inode].RxRate = 0;
			this->tb_cnu[inode].TxRate = 0;
			/* ֪ͨ�澯����ģ��*/
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
	
	/* ������00:00:00:00:00:00 */
	if( memcmp(mac, MA, 6) == 0 )
	{
		return 0;
	}
	/* ������FF:FF:FF:FF:FF:FF */
	if( memcmp(mac, MB, 6) == 0 )
	{
		return 0;
	}
	/* Ѱ��CNU ����ֹ�ظ�*/
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

	/* �ٶ�Ĭ������豸����*/
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

	/* ���ж�MAC ��ַ�Ƿ��ͻ*/
	if( !__isNewCnuMacaddr(macaddr) )
	{
		printf("\r\n\r\n  create entry for cnu %s failed !\n", cnu.col_mac);
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "register create cnu error: mac conflict !");
		return;
	}

	/* �ж��Ƿ�ﵽ�û�����*/
	idle = find_idle(cltid);
	inode = (cltid-1)*MAX_CNUS_PER_CLT+(idle-1);
	
	if( 0 == idle )
	{		
		/* CNU ����������ֹ���*/
		printf("\r\n\r\n  create entry for cnu %s failed !\n", cnu.col_mac);
		dbs_sys_log(dbsdev, DBS_LOG_ERR, "register create cnu error: no cnu entry !");
		return;
	}
	else
	{
		/* ��Ҫ��������ݿ��Լ��ڴ���*/
		cnu.id = inode+1;		
		if( CMM_SUCCESS == db_new_cnu(cltid, idle, &cnu))
		{
			/* ͬ������*/
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
	
	/* �ж��Ƿ�Ϊ�Ƿ��û�*/
	if( ! boardapi_isCnuSupported(activeCnu.DevType) )
	{		
		/* ���ͷǷ��豸����澯*/
		/* ֪ͨ�澯����ģ��*/
		if( !boardapi_isCnuTrusted(activeCnu.DevType))
		{
			lllegal_cnu_register_notification(cltid, activeCnu.Mac);	
		}
	}	

	/* ���ظ�CLT��һ�����õ�CNU����*/
	idle = find_idle(cltid);
	inode = (cltid-1)*MAX_CNUS_PER_CLT+(idle-1);
	
	if( 0 == idle )
	{
		/* ����CNU�û��������޵ĸ澯*/
		cnu_exceed_notification(cltid);
		/* ��ֹ��Ӹ��豸*/
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
		/* �Ƚ���CNU����Ϣ�������Ԫ���ݿ�*/
		cnu.id = inode+1;
		cnu.col_model = activeCnu.DevType;
		sprintf(cnu.col_mac, "%02X:%02X:%02X:%02X:%02X:%02X", 
			activeCnu.Mac[0], activeCnu.Mac[1], activeCnu.Mac[2], 
			activeCnu.Mac[3], activeCnu.Mac[4], activeCnu.Mac[5]
		);
		//memcpy((char *)(cnu.mac), (const char *)(activeCnu.Mac), 6);
		debug_printf("try_to_add_cnu(%d, %d), mac[%s]\n", cltid, idle, cnu.col_mac);
		cnu.col_sts = 0;		
		/* ��������ǳ����ն����Զ����Ϊ�����û�*/
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
			/* ����ƻ������ûᱻ�ֶ���Ϊ�����û������ǵ�����*/
			/* �û�ϣ����A�ֵ��¿�֮ͨ��ֱ���õ�B�ֵ��¾Ϳ���ʹ��*/
			/* ����2��������������ƻ�ʹ��*/
			/* 1. �ն������ֵ�Bʱ�����һ�����û�����ʱ���ò��Ḳ��*/
			/* 2. �ն������ֵ�B�������һ��δ���������õ������û�ʱ*/
			/* ����2�������Ȼ�����ƻ������ñ����ǵ����:*/
			/* 1. �ն��ھֵ�B���Ѿ���һ�������û������ҽ��й���������*/
			/* 2. �ն��ھֵ�B���Ѿ���һ��Ԥ�������û�*/
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
				/* ͬ������*/
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
				/* ͬ������*/
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
*	��������:try_to_register_new_cun
*	��������:����·����豸��ע��
*					1. ���·��ֵ��豸������Ϣ�������Ԫ���ݿ�
*					2. ����do_cnu_register��������豸ע��
*	����ֵ:
*	����:frank
*	ʱ��:2010-07-23
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
*	��������:do_clt_discorver
*	��������:
*	����ֵ:
*	����:frank
*	ʱ��:2010-07-23
*********************************************************************************************/
void do_clt_discorver(int cltid, T_MMEAD_TOPOLOGY *plist)
{
	do_clt_register(cltid, plist->clt);
}

/********************************************************************************************
*	��������:do_cnu_dropped
*	��������:
*	����ֵ:
*	����:frank
*	ʱ��:2010-07-23
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

	/* ���֮ǰ���ߵ�CNU�豸��plist���Ҳ�������˵�����豸������*/
	
	for( i=0; i<MMEAD_MAX_CNU_NUM; i++ )
	{
		inode = (cltid-1)*MAX_CNUS_PER_CLT+i;
		cnuid = i+1;
		//printf("\n##do_cnu_dropped->isCnuIndexOnUsed()\n");
		isCnuOnused = isCnuIndexOnUsed(cltid, cnuid);
		/* Ѱ��ǰһ�����������ߵ�CNU�豸*/
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
				/* ��˵�����豸������*/				
				do_cnu_unregister(cltid, cnuid);
			}
		}
		//usleep(2000);
	}
}

/********************************************************************************************
*	��������:do_cnu_discorver
*	��������:
*	����ֵ:
*	����:frank
*	ʱ��:2010-07-23
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

	/* �����ֵ��豸��Ϊ2������: */
	/* 1, �·��ֵ��豸
	** 2, ��ǰΪoff-line�����ڱ�Ϊonline���豸*/
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
					/* ��ʾ�ⲻ��һ���·��ֵ��豸*/
					discover_new = BOOL_FALSE;
					do_cnu_register(cltid, cnuid, plist->cnu[i]);
					//usleep(2000);
					break;
				}
			}
			#if 0
			/* ��δ���Ч�ʵͣ����Բ���select ���ݿ�һ�ξ����*/			
			usleep(2000);
			if( (memcmp((const char *)(plist->cnu[i].Mac), (const char *)(this->tb_cnu[j].Mac), 6) == 0) 
				&& (BOOL_TRUE == isCnuOnused) )
			{
				/* ��ʾ�ⲻ��һ���·��ֵ��豸*/
				discover_new = BOOL_FALSE;
				do_cnu_register(1, j+1, plist->cnu[i]);
				break;
			}
			#endif
		}

		/* ������һ����CNU�豸*/
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
*	��������:pro_clt_dropped
*	��������:
*	����ֵ:
*	����:frank
*	ʱ��:2010-07-23
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
*	��������:pro_top_sts_transition
*	��������:
*	����ֵ:
*	����:frank
*	ʱ��:2010-07-23
*********************************************************************************************/
void pro_top_sts_transition(int clt, T_MMEAD_TOPOLOGY *plist)
{
	int i = 0;
	int cnuid; 
	/* �����������,CLTһ������*/
	/* ���֮ǰCLT�����ߣ�����Ҫд����*/
	do_clt_discorver(clt, plist);


	/* ���CNU��֮ǰ��״̬��Ǩ����Ҫ����Ӧ�߼�����*/
	/* �ȴ������ߵ��豸*/	
	do_cnu_dropped(clt, plist);	

	/* �ٴ������ߵ��豸����Ϊ2��:
	�·��ֵ��豸����ǰΪoff-line�����ڱ�Ϊonline���豸*/
	do_cnu_discorver(clt, plist);
}

/********************************************************************************************
*	��������:ProcessTopologyChange
*	��������:
*	����ֵ:
*	����:frank
*	ʱ��:2010-07-23
*********************************************************************************************/
void ProcessTopologyChange(int clt, T_MMEAD_TOPOLOGY *plist)
{
	if( NULL == plist )
	{
		/* �˷�֧������ζ��CLT�������ˣ���������������ٷ�����*/
		/* ֻ��Ҫ���������ߵ��豸д���߲����澯*/
		pro_clt_dropped(clt);
		return;
	}
	else
	{
		/* ����˷�֧��ζ��CLTһ�����ߣ���CNU����������״̬���*/
		/* plist �����������ߵ��豸*/	
		debug_print_top(plist);
		pro_top_sts_transition(clt, plist);
		return;		
	}
}

/********************************************************************************************
*	��������:ProcessRegist
*	��������:
*	����ֵ:
*	����:frank
*	ʱ��:2010-07-23
*********************************************************************************************/
void ProcessRegist(void)
{
	/*  �ڽ��з�������ʱ���߿�����ֶ�ʧ�������ڴ�����߿�
	**  ��ʧ����������������ʮ���߿���ʧ���ǲ���Ϊ�������Ķ�ʧ
	**  ���ϣ���ʱ�ٽ����߿������߼��Ĵ���*/
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
			/* ��������*/
			reg2alarm_send_heartbeat_notification();
			sleep(5);
		}
		else
		{
			/* ע��ģ���ʼ��ʱ����������*/
			iFlag = 1;
		}
		
		sleep(REGISTER_POLL_INT);
		
		/* ��ȡ�ⲿģ�������¼�*/
		ProcessExtReq();

		/* ���REG_CLT_RESET ����λ���˴�ֻ������1��CLT��λ*/
		for( i=0; i<MAX_CLT_AMOUNT_LIMIT; i++ )
		{
			cltid = i+1;
			if( REG_CLT_RESET == cltFlags[i] )
			{
				printf("\n-->register event call : reset clt\n");
				//��ԭ��־λ
				cltFlags[i] = 0;				
				/* ����MME����CLT */
				msg_reg_mmead_reset_eoc(topEntry.tb_clt[i].DevType, topEntry.tb_clt[i].Mac);
				/* �豸���� */
				ProcessTopologyChange(cltid, NULL);				
			}
			else
			{
				/* �������Ч��clt */
				if( !boardapi_isValidUnicastMacb(topEntry.tb_clt[i].Mac) )
				{
					continue;
				}
				/* ��MMEAD��ȡ�����豸�б�*/
				else if( msg_reg_mmead_get_nelist(topEntry.tb_clt[i].Mac, &nelist) != CMM_SUCCESS )
				{
					/* ������ζ��CLT�������ˣ���������������ٷ�����*/
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
					/*  ��ԭ������*/
					if( 0 != cltLossTimes[i] )
					{
						/* ��ֹ����ʱ�澯���͹���Ƶ��*/
						if( cltLossTimes[i] > 5 )
						{
							/*  ����һ���߿���ʱ��ʧ�ĸ澯*/
							clt_heartbeat_loss_notification(cltid, cltLossTimes[i]);
						}
						cltLossTimes[i] = 0;
					}
					/* ����һ�ε�������Ϣ��Ƚϣ�����״̬��Ǩ�Ľڵ�*/
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
					//����CNUǿ��������־λ
					cnuFlags[(pIndex->clt-1)*MAX_CNUS_PER_CLT+(pIndex->cnu-1)] = REG_CNURESET;
					return CMM_SUCCESS;
				}
				else if( REG_CNU_FORCE_REGISTRATION == req->HEADER.usMsgType )
				{
					//����CNUǿ������ע���־λ
					cnuFlags[(pIndex->clt-1)*MAX_CNUS_PER_CLT+(pIndex->cnu-1)] = REG_CNU_FORCE_REGISTRATION;
					/* ����־λcsyncStatus ��0 */
					set_cnu_pro_sync(pIndex->clt, pIndex->cnu, BOOL_FALSE);
			//		printf("   pIndex->clt=%d\n",pIndex->clt);
			//		printf("   pIndex->cnu=%d\n",pIndex->cnu);
					return CMM_SUCCESS;
				}
				else if( REG_CNU_DELETE == req->HEADER.usMsgType )
				{
					//printf("\r\n  register event call : delete clt %d cnu %d\n", pIndex->clt, pIndex->cnu);
					/* ���ڴ���ɾ�����豸����Ϣ*/
					do_cnu_delete(pIndex->clt, pIndex->cnu);
					cnuFlags[(pIndex->clt-1)*MAX_CNUS_PER_CLT+(pIndex->cnu-1)] = 0;
					return CMM_SUCCESS;					
				}
				else if( REG_CLT_RESET == req->HEADER.usMsgType )
				{
					/* ����CLT ǿ��������־λ*/
					cltFlags[pIndex->clt-1] = REG_CLT_RESET;
					return CMM_SUCCESS;
				}
				else if( REG_CNU_CREATE == req->HEADER.usMsgType )
				{
					uint8_t new_cnu_mac[6] = {0};
					memcpy(new_cnu_mac, req->BUF, 6);
					/* ��CNU �������Ԫ���ݿ�*/
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

	//��ձ�־λ
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

	/*����UDP SOCKET�ӿ�*/
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

	/* ����SOCKETΪ������ģʽ*/
	//fcntl(sk->sk, F_SETFL, O_NONBLOCK);
	
	return CMM_SUCCESS;
}

/********************************************************************************************
*	��������:init_nelib
*	��������:
*	����ֵ:
*	����:frank
*	ʱ��:2010-07-23
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
	
	/* �����ݿ��н�ԭʼ��Ԫ��Ϣ��ʼ����ȫ������*/
	if( db_init_nelib(topology) != CMM_SUCCESS )
	{
		printf("init_nelib : CMM_FAILED");
		return CMM_FAILED;
	}
	else
	{
		/* ��Ԫ���ݿ��м�¼���豸��ʼ״̬Ҫȫ����ʼ��Ϊoff-line*/
		/* ��Ԫ��������ʵʱ�Ķ�̬����ҲҪ��ʼ��Ϊ0 */
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
		
		/* ÿ������ʱ����������ÿ���˿ڵ�clt */		
		/* check clt port link status first */
		if( 0 == reg2cmm_getCltPortLinkStatus(&SK_REG2CMM, clt.id) )
		{
				printf("probe clt%d: no clt present\r\n", i+1);
				/* delete this clt in dbs */
				clt.col_row_sts = 0;
				db_update_clt(i+1, &clt);
				/* ���Ͳ��ܷ����߿����쳣�澯*/
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
				/* ���Ͳ��ܷ����߿����쳣�澯*/
				clt_cannot_finded_notification(i+1);
				continue;
			}
			/* ��ȡCLT��MAC��ַ��д�����ݿ�*/
			else if( msg_reg_mmead_get_clt(&(topology->tb_clt[i])) != CMM_SUCCESS )
			{
				printf("probe clt%d: no clt present\r\n", i+1);
				/* delete this clt in dbs */
				clt.col_row_sts = 0;
				db_update_clt(i+1, &clt);
				/* ���Ͳ��ܷ����߿����쳣�澯*/
				clt_cannot_finded_notification(i+1);				
			}
			else
			{				
				cltdetected++;				
				/* �����ﲢ�����豸���ߵĴ���*/
				/* ����ֻ�ǽ�CLT��MAC��ַд�����ݿ�*/				
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
*	��������:main
*	��������:
*	����ֵ:
*	����:frank
*	ʱ��:2010-07-23
*********************************************************************************************/
int main(void)
{	
	/*���������ݿ�ģ��ͨѶ���ⲿSOCKET�ӿ�*/
	dbsdev = reg_dbsOpen();
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: register->dbsOpen error, exited !\n");
		return CMM_CREATE_SOCKET_ERROR;
	}
	
	/* Waiting for mmead init */
	dbsWaitModule(dbsdev, MF_CMM|MF_MMEAD|MF_ALARM|MF_TM);
	
	/*������MMEADģ��ͨѶ���ⲿSOCKET�ӿ�*/
	if( CMM_SUCCESS != msg_mmead_init() )
	{
		perror("Register->msg_mmead_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module register msg_mmead_init error, exited !");
		reg_dbsClose();
		return CMM_CREATE_SOCKET_ERROR;
	}

	/*������澯ģ��ͨѶ���ⲿSOCKET�ӿ�*/
	if( CMM_SUCCESS != msg_alarm_init() )
	{
		perror("Register->msg_alarm_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module register msg_alarm_init error, exited !");
		reg_dbsClose();
		return CMM_CREATE_SOCKET_ERROR;
	}	

	/*������ģ�����ģ��ͨѶ���ⲿSOCKET�ӿ�*/
	if( CMM_SUCCESS != msg_tm_init() )
	{
		perror("Register->msg_tm_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module register msg_tm_init error, exited !");
		reg_dbsClose();
		return CMM_CREATE_SOCKET_ERROR;
	}

	/*������ģ�����ģ��ͨѶ���ⲿSOCKET�ӿ�*/
	if( CMM_SUCCESS != reg2cmm_init(&SK_REG2CMM) )
	{
		perror("Register->reg2cmm_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module register reg2cmm_init error, exited !");
		reg_dbsClose();
		return CMM_CREATE_SOCKET_ERROR;
	}

	/* ���������ⲿ�����SOCKET�ӿ�*/
	if( CMM_SUCCESS != msg_regi_init() )
	{
		perror("Register->msg_regi_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module register msg_regi_init error, exited !");
		reg_dbsClose();
		return CMM_CREATE_SOCKET_ERROR;
	}

	/* ע���쳣�˳��������*/
	signal(SIGTERM, RegSignalProcessHandle);

	/* ����ϵͳ�����ĸ澯*/
	cbat_system_sts_notification(1);

	/* step 1:ģ������ʱ��ʼ������ȡDB��ԭʼ������Ϣ*/
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
	
	/* ѭ������ע���¼�*/
	ProcessRegist();

	/* ��Ҫ�����������Ӵ��룬ִ�в�����*/
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "SignalProcessHandle : module register exit");
	msg_mmead_destroy();
	msg_alarm_destroy();
	msg_tm_destroy();
	reg_dbsClose();
	
	return 0;
}

