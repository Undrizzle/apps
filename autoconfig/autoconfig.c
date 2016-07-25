/* autoconfig.c 
   using check cnu list and auto config tbl_template's vlan 
   vlan auto add 1 one by one ore all eth port the same vlan
   disable and enable vlan autoconfig
   add bys stan 2014.9.26 stanhangzhou@qq.com
 */

#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <dbsapi.h>
#include <boardapi.h>
#include "autoconfig.h"
#include "auto_dbs.h"
#include "auto_mmead.h"
#include "auto_cmm.h"
//#include "reg_alarm.h"
//#include "reg_tm.h"
//#include "reg_cmm.h"
//#include "reg_mmead.h"

T_UDP_SK_INFO SK_AUTOCONFIG;
T_UDP_SK_INFO SK_AUTOCONFIG2CMM;
static BBLOCK_QUEUE bblock;
T_TOPOLOGY_INFO topEntry;
uint8_t cltFlags[MAX_CLT_AMOUNT_LIMIT] = {0};
uint8_t cnuFlags[MAX_CNU_AMOUNT_LIMIT] = {0};
uint8_t cnuconfigedFlags[MAX_CNU_AMOUNT_LIMIT] = {0};
int REGISTER_DEBUG_ENABLE = 0;


void do_one_cnu_templat_autoconfig(int clt_index, int cnu_index, st_dbsTemplate * ptemplate)
{
	int opt_sts = CMM_SUCCESS;
	st_dbsCnu cnu;
	stCnuNode iNode;
	int i=0;

	uint8_t bMac[6] = {0};
	uint8_t mod[1024] = {0};

	st_rtl8306eSettings ack_data;

	
    if( CMM_SUCCESS != dbsGetCnu(dbsdev, (clt_index -1 ) *64 + cnu_index, &cnu) )
	{
		//printf("\n#ERROR[02]\n");
		opt_sts = CMM_FAILED;
	}
	else if(cnu.col_auto_sts){
		opt_sts = CMM_FAILED;
	}
	else if( (DEV_STS_ONLINE != cnu.col_sts)||BOOL_TRUE != cnu.col_row_sts )
	{
		//printf("\n#ERROR[03]\n");
		opt_sts = CMM_FAILED;
		
	}	
	else if( CMM_SUCCESS != boardapi_macs2b(cnu.col_mac, bMac) )
	{
		//printf("\n#ERROR[04]\n");
		opt_sts = CMM_FAILED;
	}

	if( !(34 == cnu.col_model||35 == cnu.col_model ||37 == cnu.col_model ||38 == cnu.col_model))
	{
		//3702i e4 701e4 l4 w4 supported
		//printf("\n#ERROR[05] DEV model not supported\n");
		opt_sts = CMM_FAILED;
		
	}
	if(0==bMac[0] && 0==bMac[1] && 0==bMac[2] && 0==bMac[3] && 0==bMac[4] && 0==bMac[5] )
	{
		//printf("\n#ERROR[06] DEV model not supported\n");
		opt_sts = CMM_FAILED;
	}

	

	if(CMM_SUCCESS != opt_sts) return opt_sts;

	if(CMM_SUCCESS != auto_mmead_get_rtl8306e_configs(bMac,  &ack_data))
	{
		opt_sts = CMM_FAILED;
		//printf("\n#ERROR[07]\n");
		//printf("auto_mmead_get_rtl8306e_configs error\n" );
		return opt_sts;
	}

	//printf("now call auto_mmead_get_rtl8306e_configs get  vlan1 =[%d]\n", ack_data.vlanConfig.vlan_port[0].pvid);
	//printf("now call auto_mmead_get_rtl8306e_configs get  vlan2 =[%d]\n", ack_data.vlanConfig.vlan_port[1].pvid);
	//printf("now call auto_mmead_get_rtl8306e_configs get  vlan3 =[%d]\n", ack_data.vlanConfig.vlan_port[2].pvid);
	//printf("now call auto_mmead_get_rtl8306e_configs get  vlan4 =[%d]\n", ack_data.vlanConfig.vlan_port[3].pvid);
	//printf("now call auto_mmead_get_rtl8306e_configs get  vlan cpu =[%d]\n", ack_data.vlanConfig.vlan_port[4].pvid);


	//update switch and send config
	//eth1
	if(ptemplate->col_eth1VlanAddSts){
		if(ptemplate->col_eth1VlanStart > 1 && ptemplate->col_eth1VlanStart < 4030){
			//ok now auto template config
			if(ptemplate->col_eth1VlanStop < ptemplate->col_eth1VlanStart|| ptemplate->col_eth1VlanStop >= 4030 )	
				ptemplate->col_eth1VlanStop = ptemplate->col_eth1VlanStart;					
			else
			      ptemplate->col_eth1VlanStop++;
	    	
			ack_data.vlanConfig.vlan_port[0].egress_mode = 1;
			ack_data.vlanConfig.vlan_port[0].pvid = ptemplate->col_eth1VlanStop;	
			//printf("ETH1 now set ack_data vlanid=%d, vlanstart=%d \n", ack_data.vlanConfig.vlan_port[0].pvid, ptemplate->col_eth1VlanStop);		
		}		
		else{
			//printf("col_eth1VlanStart id error pls reset it");
			return CMM_FAILED;
		}
	}
	else{		
		if(ptemplate->col_eth1VlanStart > 1 && ptemplate->col_eth1VlanStart < 4030){
			ptemplate->col_eth1VlanStop = ptemplate->col_eth1VlanStart;	
			ack_data.vlanConfig.vlan_port[0].egress_mode = 1;
			ack_data.vlanConfig.vlan_port[0].pvid = ptemplate->col_eth1VlanStop;			
		}
		else if(1 == ptemplate->col_eth1VlanStart){
			ptemplate->col_eth1VlanStop = 0;
			ack_data.vlanConfig.vlan_port[0].egress_mode= 3;	
			ack_data.vlanConfig.vlan_port[0].pvid = 1;
		}
		else{
			//printf("col_eth1VlanStart id error pls reset it");
			return CMM_FAILED;
		}
	}


	//eth2
	if(ptemplate->col_eth2VlanAddSts){
		if(ptemplate->col_eth2VlanStart > 1 && ptemplate->col_eth2VlanStart < 4030){
			//ok now auto template config
			if(ptemplate->col_eth2VlanStop < ptemplate->col_eth2VlanStart|| ptemplate->col_eth2VlanStop >= 4030 )	
				ptemplate->col_eth2VlanStop = ptemplate->col_eth2VlanStart;					
			else
			      ptemplate->col_eth2VlanStop++;
	    	
			ack_data.vlanConfig.vlan_port[1].egress_mode = 1;
			ack_data.vlanConfig.vlan_port[1].pvid = ptemplate->col_eth2VlanStop;	
			//printf("ETH2 now set ack_data swutcg vlanid=%d,  template vlan=%d \n", ack_data.vlanConfig.vlan_port[1].pvid, ptemplate->col_eth2VlanStop);		
		}		
		else{
			//printf("col_eth2VlanStart id error pls reset it");
			return CMM_FAILED;
		}
	}
	else{		
		if(ptemplate->col_eth2VlanStart > 1 && ptemplate->col_eth2VlanStart < 4030){
			ptemplate->col_eth2VlanStop = ptemplate->col_eth2VlanStart;	
			ack_data.vlanConfig.vlan_port[1].egress_mode = 1;
			ack_data.vlanConfig.vlan_port[1].pvid = ptemplate->col_eth2VlanStop;			
		}
		else if(1 == ptemplate->col_eth2VlanStart){
			ptemplate->col_eth2VlanStop = 0;
			ack_data.vlanConfig.vlan_port[1].egress_mode= 3;	
			ack_data.vlanConfig.vlan_port[1].pvid = 1;
		}
		else{
			//printf("col_eth2VlanStart id error pls reset it");
			return CMM_FAILED;
		}
	}
	//eth3
	if(ptemplate->col_eth3VlanAddSts){
		if(ptemplate->col_eth3VlanStart > 1 && ptemplate->col_eth3VlanStart < 4030){
			//ok now auto template config
			if(ptemplate->col_eth3VlanStop < ptemplate->col_eth3VlanStart|| ptemplate->col_eth3VlanStop >= 4030 )	
				ptemplate->col_eth3VlanStop = ptemplate->col_eth3VlanStart;					
			else
			      ptemplate->col_eth3VlanStop++;
	    	
			ack_data.vlanConfig.vlan_port[2].egress_mode = 1;
			ack_data.vlanConfig.vlan_port[2].pvid = ptemplate->col_eth3VlanStop;	
			//printf("ETH3 now set ack_data sw vlan=%d, template vlan=%d\n", ack_data.vlanConfig.vlan_port[2].pvid, ptemplate->col_eth3VlanStop);		
		}		
		else{
			//printf("col_eth3VlanStart id error pls reset it");
			return CMM_FAILED;
		}
	}
	else{		
		if(ptemplate->col_eth3VlanStart > 1 && ptemplate->col_eth3VlanStart < 4030){
			ptemplate->col_eth3VlanStop = ptemplate->col_eth3VlanStart;	
			ack_data.vlanConfig.vlan_port[2].egress_mode = 1;
			ack_data.vlanConfig.vlan_port[2].pvid = ptemplate->col_eth3VlanStop;			
		}
		else if(1 == ptemplate->col_eth3VlanStart){
			ptemplate->col_eth3VlanStop = 0;
			ack_data.vlanConfig.vlan_port[2].egress_mode= 3;	
			ack_data.vlanConfig.vlan_port[2].pvid = 1;
		}
		else{
			//printf("col_eth3VlanStart id error pls reset it");
			return CMM_FAILED;
		}
	}

	//eth4
	if(ptemplate->col_eth4VlanAddSts){
		if(ptemplate->col_eth4VlanStart > 1 && ptemplate->col_eth4VlanStart < 4030){
			//ok now auto template config
			if(ptemplate->col_eth4VlanStop < ptemplate->col_eth4VlanStart|| ptemplate->col_eth4VlanStop >= 4030 )	
				ptemplate->col_eth4VlanStop = ptemplate->col_eth4VlanStart;					
			else
			      ptemplate->col_eth4VlanStop++;
	    	
			ack_data.vlanConfig.vlan_port[3].egress_mode = 1;
			ack_data.vlanConfig.vlan_port[3].pvid = ptemplate->col_eth4VlanStop;	
			//printf("ETH4 now set ack_data sw vlan=%d, template vlan=%d\n", ack_data.vlanConfig.vlan_port[3].pvid, ptemplate->col_eth4VlanStop);		
		}		
		else{
			//printf("col_eth4VlanStart id error pls reset it");
			return CMM_FAILED;
		}
	}
	else{		
		if(ptemplate->col_eth4VlanStart > 1 && ptemplate->col_eth4VlanStart < 4030){
			ptemplate->col_eth4VlanStop = ptemplate->col_eth4VlanStart;	
			ack_data.vlanConfig.vlan_port[3].egress_mode = 1;
			ack_data.vlanConfig.vlan_port[3].pvid = ptemplate->col_eth4VlanStop;			
		}
		else if(1 == ptemplate->col_eth4VlanStart){
			ptemplate->col_eth4VlanStop = 0;
			ack_data.vlanConfig.vlan_port[3].egress_mode= 3;	
			ack_data.vlanConfig.vlan_port[3].pvid = 1;
		}
		else{
			//printf("col_eth4VlanStart id error pls reset it");
			return CMM_FAILED;
		}
	}
	
	//cpu port 4 eth5	
	ack_data.vlanConfig.vlan_enable = 1;
	ack_data.vlanConfig.vlan_tag_aware = 1;
	ack_data.vlanConfig.vlan_port[4].egress_mode = 2;
	ack_data.vlanConfig.vlan_port[4].pvid = 1;

	//send to cmm to update db and switch reg
	iNode.clt = clt_index;
	iNode.cnu = cnu_index;


	// printf("eth1 vlanstart=[%d], stop =[%d]\n", ptemplate->col_eth1VlanStop);
	// printf("eth2 vlanstart=[%d], stop =[%d]\n", ptemplate->col_eth2VlanStop);
	// printf("eth3 vlanstart=[%d], stop =[%d]\n", ptemplate->col_eth3VlanStop);
	// printf("eth4 vlanstart=[%d], stop =[%d]\n", ptemplate->col_eth4VlanStop);

	
	//update template dbs
	if(CMM_SUCCESS != dbsUpdateTemplate(dbsdev, 1, ptemplate))
	{
		//perror("ERROR: do_cnu_template_auto_config->dbsUpdateTemplate !\n");
		return;
	}

	dbsFflush(dbsdev);

    //printf("now willl calll   auto2cmm_writeSwitchSettings befoooooore\n");
	if(CMM_SUCCESS  != auto2cmm_writeSwitchSettings(&SK_AUTOCONFIG2CMM, &iNode,  &ack_data))
	{
		opt_sts = CMM_FAILED;
		//printf("\n#ERROR[06]\n");
		//printf("auto2cmm_writeSwitchSettings error\n" );
		return opt_sts;
	}

	//set cnu table col_auto_sts = 1 save to db
	cnu.col_auto_sts = 1;
	if( CMM_SUCCESS != dbsUpdateCnu(dbsdev, (clt_index -1 ) *64 + cnu_index, &cnu) )
	{
		//printf("\n#ERROR[02]\n");
		opt_sts = CMM_FAILED;
	}

	printf("auto2cmm_writeSwitchSettings now successfull!!!\n" );

}

void do_cnu_template_auto_config(void )
{
	st_dbsTemplate template;
	st_dbsProfile profile;
	DB_INTEGER_V iValue;
	
	
	int opt_sts=CMM_SUCCESS;
	int len = 0;
	int i=0, j=0;




	//printf("do_cnu_template_auto_config\n");

	/* 获取全局自动模板下发VLAN自增使能状态*/
	memset(&template, 0, sizeof(st_dbsTemplate));
		
	//get 1 row is templage management row
	if(CMM_SUCCESS != dbsGetTemplate(dbsdev, 1, &template))
	{
		//perror("ERROR: do_cnu_template_auto_config->dbsGetTemplate !\n");
		return;
	}else{
	    

		if(1 != template.col_tempAutoSts){
			 return;
		}
		    
		//printf("now template.col_tempAutoSts ===1\n");
	
	    
	
	//	printf("now save template dbs ok\n");
	    for( i=1; i <= MAX_CLT_AMOUNT_LIMIT; i++)
			for(j=1; j<=  MAX_CNUS_PER_CLT; j++)
			{				
					do_one_cnu_templat_autoconfig(i,j, &template);
					sleep(4);
				
			}

		
	}
	
}




void ProcessRegist(void)
{
	int i = 0;
	int cltid = 0;
	int iFlag = 0;
	int cltLossTimes[MAX_CLT_AMOUNT_LIMIT];
	T_MMEAD_TOPOLOGY nelist;

	while(1)
	{		
				
	    
		
		
		//sleep(AUTOCONFIG_POLL_INT);
		
		sleep(6);

		/* ¶ÁÈ¡Íâ²¿Ä£¿éÇëÇóÊÂ¼þ*/


		do_cnu_template_auto_config();
	}

}


int main(void)
{	
	/*´´½¨ÓëÊý¾Ý¿âÄ£¿éÍ¨Ñ¶µÄÍâ²¿SOCKET½Ó¿Ú*/
	
	dbsdev = auto_dbsOpen();
	if( NULL == dbsdev )
	{
		fprintf(stderr,"ERROR: autoconfig->dbsOpen error, exited !\n");
		return CMM_CREATE_SOCKET_ERROR;
	}
	
	
	/* Waiting for mmead init */
	dbsWaitModule(dbsdev, MF_REGI|MF_CMM|MF_MMEAD|MF_ALARM|MF_TM);
	

	if( CMM_SUCCESS != msg_auto_mmead_init() )
	{
		perror("Register->msg_auto_mmead_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module register msg_auto_mmead_init error, exited !");
		auto_dbsClose();
		return CMM_CREATE_SOCKET_ERROR;
	}

	if( CMM_SUCCESS != auto2cmm_init(&SK_AUTOCONFIG2CMM) )
	{
		perror("Register->auto2cmm_init error, exited !\n");
		dbs_sys_log(dbsdev, DBS_LOG_EMERG, "module register auto2cmm_init error, exited !");			
		auto_dbsClose();
		return CMM_CREATE_SOCKET_ERROR;
	}
	
	fprintf(stderr, "Starting module autoconfig	......		[OK]\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting module autoconfig success");
	
	/* Ñ­»·´¦Àí×¢²áÊÂ¼þ*/
	ProcessRegist();

	/* ²»ÒªÔÚÕâ¸öºóÃæÌí¼Ó´úÂë£¬Ö´ÐÐ²»µ½µÎ*/
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "SignalProcessHandle : module autoconfig exit");
	auto_dbsClose();
	
	return 0;
}

