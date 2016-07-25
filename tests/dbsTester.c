/****************************************************************************************
*	ÎÄ¼þÃû:dbsTester.c
*	¹¦ÄÜ:dbs Ä£¿éµÄ²âÊÔ³ÌÐò
*	×÷Õß:frank
*	Ê±¼ä:2010-07-21
*
*****************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <linux/if.h>

#include <public.h>
#include <dbsapi.h>
#include <boardapi.h>

static T_DBS_DEV_INFO *dev = NULL;

void dbsTester_usage(void)
{
	printf("\nParameter error:\n");
	printf("\nUsage:\n");
	printf("dbsTester {msg-debug|sql-debug} {enable|disable}\n");
	printf("dbsTester test {0|1|...}\n");
	printf("	--0: test save system.db from ramfs to flash\n");
	printf("	--1: test get an integer value from dbs system.db\n");
	printf("	--2: test get all integer value from dbs system.db\n");
	printf("	--3: test update an integer value to dbs system.db\n");
	printf("	--4: test update all integer value to dbs system.db\n");
	printf("	--5: test get a text value from dbs system.db\n");
	printf("	--6: test get all text value from dbs system.db\n");
	printf("	--7: test update a text value to dbs system.db\n");
	printf("	--8: test update all text value to dbs system.db\n");
	printf("	--9: test write syslog to dbs [loop usleep 100]\n");
	printf("	--10: test show syslog from dbs\n");
	printf("	--11: test write optlog to dbs [loop usleep 100]\n");
	printf("	--12: test show optlog from dbs\n");
	printf("	--13: test write alarmlog to dbs [loop usleep 100]\n");
	printf("	--14: test show alarmlog from dbs\n");
	printf("	--15: test register a module\n");
	printf("	--16: test destroy a module\n");
	printf("	--17: test register all modules\n");
	printf("	--18: test destroy all modules\n");
	printf("	--19: test wait all modules\n");
	printf("	--20: test update and get one-row interfaces\n");
	printf("	--21: test update and get all-row interfaces\n");
	printf("	--22: test create su profile for all cnus\n");
	printf("	--23: test create white-list user default profile for all cnus\n");
	printf("	--24: test create black-list user default profile for all cnus\n");
	printf("	--25: test destroy all (clt, cltconf, cnu, profile)\n");
	printf("	--26: test select cnu index by mac\n");
	
	printf("\n\n");
}

void dbsTester_signalProcessHandle(int n)
{
	printf("\n\n==================================================================\n");
	fprintf(stderr, "INFO: dbsTester_signalProcessHandle close dbsTester !\n");
	dbs_sys_log(dev, DBS_LOG_INFO, "dbsTester_signalProcessHandle : module dbsTester exit");
	dbsClose(dev);		
	exit(0);
}

int __test_dbs_row_tbl_clirole_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	int i = 0;
	st_dbsCliRole row_w;
	st_dbsCliRole row_r;

	sprintf(row_w.col_user, "str_%d_%d", id, ++i);
	sprintf(row_w.col_pwd, "str_%d_%d", id, ++i);	
		
	if( 0 != dbsUpdateCliRole(dev, id, &row_w) )
	{
		printf("ERROR: __test_dbs_row_tbl_clirole_interfaces->dbsUpdateCliRole(%d)\n", id);
		return -1;
	}
	
	if( 0 == dbsGetCliRole(dev, id, &row_r) )
	{
		printf("\n%d | %s | %s\n", row_r.id, row_r.col_user, row_r.col_pwd);
		return 0;
	}
	else
	{
		printf("ERROR: __test_dbs_row_tbl_clirole_interfaces->dbsGetCliRole(%d)\n", id);
		return -1;
	}
}

int __test_dbs_row_tbl_clt_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	int i = 0;
	st_dbsClt row_w;
	st_dbsClt row_r;

	/* ¸³Öµ*/
	row_w.col_model = ++i;
	sprintf(row_w.col_mac, "str_%d_%d", id, ++i);
	row_w.col_sts = ++i;
	row_w.col_maxStas = ++i;
	row_w.col_numStas = ++i;
	sprintf(row_w.col_swVersion, "str_%d_%d", id, ++i);
	row_w.col_synch = ++i;
	row_w.col_row_sts = ++i;

	/* ²âÊÔÐ´Èë*/
	if( 0 != dbsUpdateClt(dev, id, &row_w) )
	{
		printf("ERROR: __test_dbs_row_tbl_clt_interfaces->dbsUpdateClt(%d)\n", id);
		return -1;
	}

	/* ²âÊÔ¶ÁÈ¡*/
	if( 0 == dbsGetClt(dev, id, &row_r) )
	{
		printf("\n%d | %d | %s | %d | %d | %d | %s | %d | %d \n", 
			row_r.id, 
			row_r.col_model, 
			row_r.col_mac,
			row_r.col_sts,
			row_r.col_maxStas,
			row_r.col_numStas,
			row_r.col_swVersion,
			row_r.col_synch,
			row_r.col_row_sts
		);
		return 0;
	}
	else
	{
		printf("ERROR: __test_dbs_row_tbl_clt_interfaces->dbsGetClt(%d)\n", id);
		return -1;
	}
}

int __test_dbs_row_tbl_cltconf_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	int i = 0;
	st_dbsCltConf row_w;
	st_dbsCltConf row_r;

	/* ¸³Öµ*//* row_w.col_model = ++i; */ /* sprintf(row_w.col_mac, "str_%d_%d", id, ++i); */
	row_w.col_row_sts = ++i;
	row_w.col_base = ++i;
	row_w.col_macLimit = ++i;
	row_w.col_curate = ++i;
	row_w.col_cdrate = ++i;
	row_w.col_loagTime = ++i;
	row_w.col_reagTime = ++i;
	row_w.col_igmpPri = ++i;
	row_w.col_unicastPri = ++i;
	row_w.col_avsPri = ++i;
	row_w.col_mcastPri = ++i;
	row_w.col_tbaPriSts = ++i;
	row_w.col_cosPriSts = ++i;
	row_w.col_cos0pri = ++i;
	row_w.col_cos1pri = ++i;
	row_w.col_cos2pri = ++i;
	row_w.col_cos3pri = ++i;
	row_w.col_cos4pri = ++i;
	row_w.col_cos5pri = ++i;
	row_w.col_cos6pri = ++i;
	row_w.col_cos7pri = ++i;
	row_w.col_tosPriSts = ++i;
	row_w.col_tos0pri = ++i;
	row_w.col_tos1pri = ++i;
	row_w.col_tos2pri = ++i;
	row_w.col_tos3pri = ++i;
	row_w.col_tos4pri = ++i;
	row_w.col_tos5pri = ++i;
	row_w.col_tos6pri = ++i;
	row_w.col_tos7pri = ++i;	

	/* ²âÊÔÐ´Èë*/
	if( 0 != dbsUpdateCltconf(dev, id, &row_w) )
	{
		printf("ERROR: __test_dbs_row_tbl_cltconf_interfaces->dbsUpdateCltconf(%d)\n", id);
		return -1;
	}

	/* ²âÊÔ¶ÁÈ¡*/
	if( 0 == dbsGetCltconf(dev, id, &row_r) )
	{
		printf("\n%d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d\n", 
			row_r.id, row_r.col_row_sts, row_r.col_base, row_r.col_macLimit, 
			row_r.col_curate, row_r.col_cdrate, row_r.col_loagTime, row_r.col_reagTime, 
			row_r.col_igmpPri, row_r.col_unicastPri, row_r.col_avsPri, row_r.col_mcastPri, 
			row_r.col_tbaPriSts, row_r.col_cosPriSts, row_r.col_cos0pri, row_r.col_cos1pri, 
			row_r.col_cos2pri, row_r.col_cos3pri, row_r.col_cos4pri, row_r.col_cos5pri, 
			row_r.col_cos6pri, row_r.col_cos7pri, row_r.col_tosPriSts, row_r.col_tos0pri, 
			row_r.col_tos1pri, row_r.col_tos2pri, row_r.col_tos3pri, row_r.col_tos4pri, 
			row_r.col_tos5pri, row_r.col_tos6pri, row_r.col_tos7pri
		);
		return 0;
	}
	else
	{
		printf("ERROR: __test_dbs_row_tbl_cltconf_interfaces->dbsGetCltconf(%d)\n", id);
		return -1;
	}
}

int __test_dbs_row_tbl_cnu_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	int i = 0;
	st_dbsCnu row_w;
	st_dbsCnu row_r;

	/* ¸³Öµ*//* row_w.col_model = ++i; */ /* sprintf(row_w.col_mac, "str_%d_%d", id, ++i); */
	row_w.col_model = ++i;
	sprintf(row_w.col_mac, "str_%d_%d", id, ++i);
	row_w.col_sts = ++i;
	row_w.col_auth = ++i;
	sprintf(row_w.col_ver, "str_%d_%d", id, ++i);
	row_w.col_rx = ++i;
	row_w.col_tx = ++i;
	sprintf(row_w.col_snr, "str_%d_%d", id, ++i);
	sprintf(row_w.col_bpc, "str_%d_%d", id, ++i);
	sprintf(row_w.col_att, "str_%d_%d", id, ++i);
	row_w.col_synch = ++i;
	row_w.col_row_sts = ++i;

	/* ²âÊÔÐ´Èë*/
	if( 0 != dbsUpdateCnu(dev, id, &row_w) )
	{
		printf("ERROR: __test_dbs_row_tbl_cnu_interfaces->dbsUpdateCnu(%d)\n", id);
		return -1;
	}

	/* ²âÊÔ¶ÁÈ¡*/
	if( 0 == dbsGetCnu(dev, id, &row_r) )
	{
		printf("\n%d | %d | %s | %d | %d | %s | %d | %d | %s | %s | %s | %d | %d\n", 
			row_r.id, row_r.col_model, row_r.col_mac, row_r.col_sts, 
			row_r.col_auth, row_r.col_ver, row_r.col_rx, row_r.col_tx, 
			row_r.col_snr, row_r.col_bpc, row_r.col_att, row_r.col_synch, 
			row_r.col_row_sts
		);
		return 0;
	}
	else
	{
		printf("ERROR: __test_dbs_row_tbl_cnu_interfaces->dbsGetCnu(%d)\n", id);
		return -1;
	}
}

int __test_dbs_row_tbl_depro_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	int i = 0;
	st_dbsCnuDefaultProfile row_w;
	st_dbsCnuDefaultProfile row_r;

	/* ¸³Öµ*//* row_w.col_model = ++i; */ /* sprintf(row_w.col_mac, "str_%d_%d", id, ++i); */
	row_w.col_base = ++i;
	row_w.col_macLimit = ++i;
	row_w.col_curate = ++i;
	row_w.col_cdrate = ++i;
	row_w.col_loagTime = ++i;
	row_w.col_reagTime = ++i;
	row_w.col_igmpPri = ++i;
	row_w.col_unicastPri = ++i;
	row_w.col_avsPri = ++i;
	row_w.col_mcastPri = ++i;
	row_w.col_tbaPriSts = ++i;
	row_w.col_cosPriSts = ++i;
	row_w.col_cos0pri = ++i;
	row_w.col_cos1pri = ++i;
	row_w.col_cos2pri = ++i;
	row_w.col_cos3pri = ++i;
	row_w.col_cos4pri = ++i;
	row_w.col_cos5pri = ++i;
	row_w.col_cos6pri = ++i;
	row_w.col_cos7pri = ++i;
	row_w.col_tosPriSts = ++i;
	row_w.col_tos0pri = ++i;
	row_w.col_tos1pri = ++i;
	row_w.col_tos2pri = ++i;
	row_w.col_tos3pri = ++i;
	row_w.col_tos4pri = ++i;
	row_w.col_tos5pri = ++i;
	row_w.col_tos6pri = ++i;
	row_w.col_tos7pri = ++i;
	row_w.col_sfbSts = ++i;
	row_w.col_sfuSts = ++i;
	row_w.col_sfmSts = ++i;
	row_w.col_sfRate = ++i;
	row_w.col_vlanSts = ++i;
	row_w.col_eth1vid = ++i;
	row_w.col_eth2vid = ++i;
	row_w.col_eth3vid = ++i;
	row_w.col_eth4vid = ++i;
	row_w.col_portPriSts = ++i;
	row_w.col_eth1pri = ++i;
	row_w.col_eth2pri = ++i;
	row_w.col_eth3pri = ++i;
	row_w.col_eth4pri = ++i;
	row_w.col_rxLimitSts = ++i;
	row_w.col_cpuPortRxRate = ++i;
	row_w.col_eth1rx = ++i;
	row_w.col_eth2rx = ++i;
	row_w.col_eth3rx = ++i;
	row_w.col_eth4rx = ++i;
	row_w.col_txLimitSts = ++i;
	row_w.col_cpuPortTxRate = ++i;
	row_w.col_eth1tx = ++i;
	row_w.col_eth2tx = ++i;
	row_w.col_eth3tx = ++i;
	row_w.col_eth4tx = ++i;
	row_w.col_psctlSts = ++i;
	row_w.col_cpuPortSts = ++i;
	row_w.col_eth1sts = ++i;
	row_w.col_eth2sts = ++i;
	row_w.col_eth3sts = ++i;
	row_w.col_eth4sts = ++i;

	/* ²âÊÔÐ´Èë*/
	if( 0 != dbsUpdateDepro(dev, id, &row_w) )
	{
		printf("ERROR: __test_dbs_row_tbl_depro_interfaces->dbsUpdateDepro(%d)\n", id);
		return -1;
	}

	/* ²âÊÔ¶ÁÈ¡*/
	if( 0 == dbsGetDepro(dev, id, &row_r) )
	{
		printf("\n%d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d\n", 
			row_r.id, 	row_w.col_base, 
			row_w.col_macLimit, row_w.col_curate, row_w.col_cdrate, row_w.col_loagTime, 
			row_w.col_reagTime, row_w.col_igmpPri, row_w.col_unicastPri, row_w.col_avsPri, 
			row_w.col_mcastPri, row_w.col_tbaPriSts, row_w.col_cosPriSts, row_w.col_cos0pri, 
			row_w.col_cos1pri, row_w.col_cos2pri, row_w.col_cos3pri, row_w.col_cos4pri, 
			row_w.col_cos5pri, row_w.col_cos6pri, row_w.col_cos7pri, row_w.col_tosPriSts, 
			row_w.col_tos0pri, row_w.col_tos1pri, row_w.col_tos2pri, row_w.col_tos3pri, 
			row_w.col_tos4pri, row_w.col_tos5pri, row_w.col_tos6pri, row_w.col_tos7pri, 
			row_w.col_sfbSts, row_w.col_sfuSts, row_w.col_sfmSts, row_w.col_sfRate, 
			row_w.col_vlanSts, row_w.col_eth1vid, row_w.col_eth2vid, row_w.col_eth3vid, 
			row_w.col_eth4vid, row_w.col_portPriSts, row_w.col_eth1pri, row_w.col_eth2pri, 
			row_w.col_eth3pri, row_w.col_eth4pri, row_w.col_rxLimitSts, row_w.col_cpuPortRxRate, 
			row_w.col_eth1rx, row_w.col_eth2rx, row_w.col_eth3rx, row_w.col_eth4rx, 
			row_w.col_txLimitSts, row_w.col_cpuPortTxRate, row_w.col_eth1tx, row_w.col_eth2tx, 
			row_w.col_eth3tx, row_w.col_eth4tx, row_w.col_psctlSts, row_w.col_cpuPortSts, 
			row_w.col_eth1sts, row_w.col_eth2sts, row_w.col_eth3sts, row_w.col_eth4sts
		);
		return 0;
	}
	else
	{
		printf("ERROR: __test_dbs_row_tbl_depro_interfaces->dbsGetDepro(%d)\n", id);
		return -1;
	}
}


int __test_dbs_row_tbl_network_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	int i = 0;
	st_dbsNetwork row_w;
	st_dbsNetwork row_r;

	/* ¸³Öµ*//* row_w.col_model = ++i; */ /* sprintf(row_w.col_mac, "str_%d_%d", id, ++i); */
	sprintf(row_w.col_ip, "str_%d_%d", id, ++i);
	sprintf(row_w.col_netmask, "str_%d_%d", id, ++i);
	sprintf(row_w.col_gw, "str_%d_%d", id, ++i);
	row_w.col_dhcp = ++i;
	sprintf(row_w.col_dns, "str_%d_%d", id, ++i);
	sprintf(row_w.col_mac, "str_%d_%d", id, ++i);
	row_w.col_mvlan_sts = ++i;
	row_w.col_mvlan_id = ++i;	

	/* ²âÊÔÐ´Èë*/
	if( 0 != dbsUpdateNetwork(dev, id, &row_w) )
	{
		printf("ERROR: __test_dbs_row_tbl_network_interfaces->dbsUpdateNetwork(%d)\n", id);
		return -1;
	}

	/* ²âÊÔ¶ÁÈ¡*/
	if( 0 == dbsGetNetwork(dev, id, &row_r) )
	{
		printf("\n%d | %s | %s | %s | %d | %s | %s | %d | %d\n", 
			row_r.id, 
			row_r.col_ip, row_r.col_netmask, 	row_r.col_gw, row_r.col_dhcp, 
			row_r.col_dns, row_r.col_mac, row_r.col_mvlan_sts, row_r.col_mvlan_id
		);
		return 0;
	}
	else
	{
		printf("ERROR: __test_dbs_row_tbl_network_interfaces->dbsGetNetwork(%d)\n", id);
		return -1;
	}
}

int __test_dbs_row_tbl_profile_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	int i = 0;
	st_dbsProfile row_w;
	st_dbsProfile row_r;

	/* ¸³Öµ*//* row_w.col_model = ++i; */ /* sprintf(row_w.col_mac, "str_%d_%d", id, ++i); */
	row_w.col_row_sts = ++i;
	row_w.col_base = ++i;
	row_w.col_macLimit = ++i;
	row_w.col_curate = ++i;
	row_w.col_cdrate = ++i;
	row_w.col_loagTime = ++i;
	row_w.col_reagTime = ++i;
	row_w.col_igmpPri = ++i;
	row_w.col_unicastPri = ++i;
	row_w.col_avsPri = ++i;
	row_w.col_mcastPri = ++i;
	row_w.col_tbaPriSts = ++i;
	row_w.col_cosPriSts = ++i;
	row_w.col_cos0pri = ++i;
	row_w.col_cos1pri = ++i;
	row_w.col_cos2pri = ++i;
	row_w.col_cos3pri = ++i;
	row_w.col_cos4pri = ++i;
	row_w.col_cos5pri = ++i;
	row_w.col_cos6pri = ++i;
	row_w.col_cos7pri = ++i;
	row_w.col_tosPriSts = ++i;
	row_w.col_tos0pri = ++i;
	row_w.col_tos1pri = ++i;
	row_w.col_tos2pri = ++i;
	row_w.col_tos3pri = ++i;
	row_w.col_tos4pri = ++i;
	row_w.col_tos5pri = ++i;
	row_w.col_tos6pri = ++i;
	row_w.col_tos7pri = ++i;
	row_w.col_sfbSts = ++i;
	row_w.col_sfuSts = ++i;
	row_w.col_sfmSts = ++i;
	row_w.col_sfRate = ++i;
	row_w.col_vlanSts = ++i;
	row_w.col_eth1vid = ++i;
	row_w.col_eth2vid = ++i;
	row_w.col_eth3vid = ++i;
	row_w.col_eth4vid = ++i;
	row_w.col_portPriSts = ++i;
	row_w.col_eth1pri = ++i;
	row_w.col_eth2pri = ++i;
	row_w.col_eth3pri = ++i;
	row_w.col_eth4pri = ++i;
	row_w.col_rxLimitSts = ++i;
	row_w.col_cpuPortRxRate = ++i;
	row_w.col_eth1rx = ++i;
	row_w.col_eth2rx = ++i;
	row_w.col_eth3rx = ++i;
	row_w.col_eth4rx = ++i;
	row_w.col_txLimitSts = ++i;
	row_w.col_cpuPortTxRate = ++i;
	row_w.col_eth1tx = ++i;
	row_w.col_eth2tx = ++i;
	row_w.col_eth3tx = ++i;
	row_w.col_eth4tx = ++i;
	row_w.col_psctlSts = ++i;
	row_w.col_cpuPortSts = ++i;
	row_w.col_eth1sts = ++i;
	row_w.col_eth2sts = ++i;
	row_w.col_eth3sts = ++i;
	row_w.col_eth4sts = ++i;

	/* ²âÊÔÐ´Èë*/
	if( 0 != dbsUpdateProfile(dev, id, &row_w) )
	{
		printf("ERROR: __test_dbs_row_tbl_profile_interfaces->dbsUpdateProfile(%d)\n", id);
		return -1;
	}

	/* ²âÊÔ¶ÁÈ¡*/
	if( 0 == dbsGetProfile(dev, id, &row_r) )
	{
		printf("\n%d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d | %d\n", 
			row_r.id, 	row_w.col_row_sts, row_w.col_base, 
			row_w.col_macLimit, row_w.col_curate, row_w.col_cdrate, row_w.col_loagTime, 
			row_w.col_reagTime, row_w.col_igmpPri, row_w.col_unicastPri, row_w.col_avsPri, 
			row_w.col_mcastPri, row_w.col_tbaPriSts, row_w.col_cosPriSts, row_w.col_cos0pri, 
			row_w.col_cos1pri, row_w.col_cos2pri, row_w.col_cos3pri, row_w.col_cos4pri, 
			row_w.col_cos5pri, row_w.col_cos6pri, row_w.col_cos7pri, row_w.col_tosPriSts, 
			row_w.col_tos0pri, row_w.col_tos1pri, row_w.col_tos2pri, row_w.col_tos3pri, 
			row_w.col_tos4pri, row_w.col_tos5pri, row_w.col_tos6pri, row_w.col_tos7pri, 
			row_w.col_sfbSts, row_w.col_sfuSts, row_w.col_sfmSts, row_w.col_sfRate, 
			row_w.col_vlanSts, row_w.col_eth1vid, row_w.col_eth2vid, row_w.col_eth3vid, 
			row_w.col_eth4vid, row_w.col_portPriSts, row_w.col_eth1pri, row_w.col_eth2pri, 
			row_w.col_eth3pri, row_w.col_eth4pri, row_w.col_rxLimitSts, row_w.col_cpuPortRxRate, 
			row_w.col_eth1rx, row_w.col_eth2rx, row_w.col_eth3rx, row_w.col_eth4rx, 
			row_w.col_txLimitSts, row_w.col_cpuPortTxRate, row_w.col_eth1tx, row_w.col_eth2tx, 
			row_w.col_eth3tx, row_w.col_eth4tx, row_w.col_psctlSts, row_w.col_cpuPortSts, 
			row_w.col_eth1sts, row_w.col_eth2sts, row_w.col_eth3sts, row_w.col_eth4sts
		);
		return 0;
	}
	else
	{
		printf("ERROR: __test_dbs_row_tbl_profile_interfaces->dbsGetProfile(%d)\n", id);
		return -1;
	}
}

int __test_dbs_row_tbl_snmp_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	int i = 0;
	st_dbsSnmp row_w;
	st_dbsSnmp row_r;

	/* ¸³Öµ*//* row_w.col_model = ++i; */ /* sprintf(row_w.col_mac, "str_%d_%d", id, ++i); */
	sprintf(row_w.col_rdcom, "str_%d_%d", id, ++i);
	sprintf(row_w.col_wrcom, "str_%d_%d", id, ++i);
	sprintf(row_w.col_trapcom, "str_%d_%d", id, ++i);
	sprintf(row_w.col_sina, "str_%d_%d", id, ++i);
	sprintf(row_w.col_sinb, "str_%d_%d", id, ++i);
	row_w.col_tpa = ++i;
	row_w.col_tpb = ++i;

	/* ²âÊÔÐ´Èë*/
	if( 0 != dbsUpdateSnmp(dev, id, &row_w) )
	{
		printf("ERROR: __test_dbs_row_tbl_snmp_interfaces->dbsUpdateSnmp(%d)\n", id);
		return -1;
	}

	/* ²âÊÔ¶ÁÈ¡*/
	if( 0 == dbsGetSnmp(dev, id, &row_r) )
	{
		printf("\n%d | %s | %s | %s | %s | %s | %d | %d\n", 
			row_r.id, 	row_r.col_rdcom, row_r.col_wrcom, row_r.col_trapcom, 
			row_r.col_sina, row_r.col_sinb, row_r.col_tpa, row_r.col_tpb
		);
		return 0;
	}
	else
	{
		printf("ERROR: __test_dbs_row_tbl_snmp_interfaces->dbsGetSnmp(%d)\n", id);
		return -1;
	}
}

int __test_dbs_row_tbl_swmgmt_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	int i = 0;
	st_dbsSwmgmt row_w;
	st_dbsSwmgmt row_r;

	/* ¸³Öµ*//* row_w.col_model = ++i; */ /* sprintf(row_w.col_mac, "str_%d_%d", id, ++i); */
	sprintf(row_w.col_ip, "str_%d_%d", id, ++i);
	row_w.col_port = ++i;
	sprintf(row_w.col_user, "str_%d_%d", id, ++i);
	sprintf(row_w.col_pwd, "str_%d_%d", id, ++i);
	sprintf(row_w.col_path, "str_%d_%d", id, ++i);

	/* ²âÊÔÐ´Èë*/
	if( 0 != dbsUpdateSwmgmt(dev, id, &row_w) )
	{
		printf("ERROR: __test_dbs_row_tbl_swmgmt_interfaces->dbsUpdateSwmgmt(%d)\n", id);
		return -1;
	}

	/* ²âÊÔ¶ÁÈ¡*/
	if( 0 == dbsGetSwmgmt(dev, id, &row_r) )
	{
		printf("\n%d | %s | %d | %s | %s | %s\n", 
			row_r.id, row_r.col_ip, row_r.col_port, row_r.col_user, 
			row_r.col_pwd, row_r.col_path
		);
		return 0;
	}
	else
	{
		printf("ERROR: __test_dbs_row_tbl_swmgmt_interfaces->dbsGetSwmgmt(%d)\n", id);
		return -1;
	}
}

int __test_dbs_row_tbl_sysinfo_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	int i = 0;
	st_dbsSysinfo row_w;
	st_dbsSysinfo row_r;

	/* ¸³Öµ*//* row_w.col_model = ++i; */ /* sprintf(row_w.col_mac, "str_%d_%d", id, ++i); */
	row_w.col_model = ++i;
	row_w.col_maxclt = ++i;
	sprintf(row_w.col_hwver, "str_%d_%d", id, ++i);
	sprintf(row_w.col_bver, "str_%d_%d", id, ++i);
	sprintf(row_w.col_kver, "str_%d_%d", id, ++i);
	sprintf(row_w.col_appver, "str_%d_%d", id, ++i);
	row_w.col_flashsize = ++i;
	row_w.col_ramsize = ++i;
	row_w.col_wlctl = ++i;
	row_w.col_autoud = ++i;
	row_w.col_autoconf = ++i;
	row_w.col_wdt = ++i;
	sprintf(row_w.col_mfinfo, "str_%d_%d", id, ++i);

	/* ²âÊÔÐ´Èë*/
	if( 0 != dbsUpdateSysinfo(dev, id, &row_w) )
	{
		printf("ERROR: __test_dbs_row_tbl_sysinfo_interfaces->dbsUpdateSysinfo(%d)\n", id);
		return -1;
	}

	/* ²âÊÔ¶ÁÈ¡*/
	if( 0 == dbsGetSysinfo(dev, id, &row_r) )
	{
		printf("\n%d | %d | %d | %s | %s | %s | %s | %d | %d | %d | %d | %d | %d | %s\n", 
			row_r.id, row_r.col_model, row_r.col_maxclt, row_r.col_hwver, 
			row_r.col_bver, row_r.col_kver, row_r.col_appver, row_r.col_flashsize, 
			row_r.col_ramsize, row_r.col_wlctl, row_r.col_autoud, row_r.col_autoconf, 
			row_r.col_wdt, row_r.col_mfinfo
		);
		return 0;
	}
	else
	{
		printf("ERROR: __test_dbs_row_tbl_sysinfo_interfaces->dbsGetSysinfo(%d)\n", id);
		return -1;
	}
}

int __test_dbs_create_su_profile_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	return dbsCreateSuProfileForCnu(dev, id);
}

int __test_dbs_create_dewl_profile_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	return dbsCreateDewlProfileForCnu(dev, id);
}

int __test_dbs_create_debl_profile_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	return dbsCreateDeblProfileForCnu(dev, id);
}

int __test_dbs_destroy_clt_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	return dbsDestroyRowClt(dev, id);
}

int __test_dbs_destroy_cltconf_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	return dbsDestroyRowCltconf(dev, id);
}

int __test_dbs_destroy_cnu_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	return dbsDestroyRowCnu(dev, id);
}

int __test_dbs_destroy_profile_interfaces(T_DBS_DEV_INFO *dev, uint16_t id)
{
	return dbsDestroyRowProfile(dev, id);
}

int TEST_DBS_FFLUSH(T_DBS_DEV_INFO *dev)
{
	struct timeval start, end;

	gettimeofday( &start, NULL );
	if( 0 == dbsFflush(dev) )
	{
		gettimeofday( &end, NULL );
		printf("dbs backup success !\n");
	}
	else
	{
		gettimeofday( &end, NULL );
		printf("dbs backup failed !\n");
	}
	printf("INFO: TEST_DBS_FFLUSH->dbsFflush: [Time Used: %d Seconds %ul Microseconds]\n", 
		(int)(end.tv_sec - start.tv_sec),
		(uint32_t)(end.tv_usec - start.tv_usec)
	);
	
	return 0;
}

int TEST_DBS_GET_INTEGER(T_DBS_DEV_INFO *dev)
{
	int n = 0;

	DB_INTEGER_V v;

	printf("\n Please select the table id:\n");
	printf("	[0] tbl_cliroles\n");
	printf("	[1] tbl_clt\n");
	printf("	[2] tbl_cltconf\n");
	printf("	[3] tbl_cnu\n");
	printf("	[4] tbl_depro\n");
	printf("	[5] tbl_network\n");
	printf("	[6] tbl_profile\n");
	printf("	[7] tbl_snmp\n");
	printf("	[8] tbl_swmgmt\n");
	printf("	[9] tbl_sysinfo\n");
	printf("input: ");
	
	scanf("%d", &n);
	v.ci.tbl = n;
	printf("\n Please input row id(1 is for the first row id): ");
	scanf("%d", &n);
	v.ci.row = n;
	printf("\n Please input col id(0 is for the first col id): ");
	scanf("%d", &n);
	v.ci.col = n;
	printf(" Your input is : tbl = %d, row = %d, col = %d\n", v.ci.tbl, v.ci.row, v.ci.col);	

	if( 0 == dbsGetInteger(dev, &v) )
	{
		if( DBS_NULL == v.ci.colType )
		{
			printf("-->ack : tbl = %d, row = %d, col = %d, value = (null)\n", 
				v.ci.tbl, v.ci.row, v.ci.col);
		}
		else
		{
			printf("-->ack : tbl = %d, row = %d, col = %d, value = %d\n", 
				v.ci.tbl, v.ci.row, v.ci.col, v.integer);
		}
	}
	else
	{
		printf("-->ack : failed !\n");
	}

	return 0;
}

int TEST_DBS_GET_ALL_INTEGER(T_DBS_DEV_INFO *dev)
{
	int i = 0;
	int j = 0;
	int n = 0;
	DB_INTEGER_V v;

	printf("\n Please select the table id:\n");
	printf("	[0] tbl_cliroles\n");
	printf("	[1] tbl_clt\n");
	printf("	[2] tbl_cltconf\n");
	printf("	[3] tbl_cnu\n");
	printf("	[4] tbl_depro\n");
	printf("	[5] tbl_network\n");
	printf("	[6] tbl_profile\n");
	printf("	[7] tbl_snmp\n");
	printf("	[8] tbl_swmgmt\n");
	printf("	[9] tbl_sysinfo\n");
	printf("input: ");
	
	scanf("%d", &n);
	v.ci.tbl = n;

	switch(v.ci.tbl)
	{
		case DBS_SYS_TBL_ID_CLIROLE:
		{
			for( i=1;i<=3;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CLIROLE;j++ )
				{
					if( DBS_SYS_TBL_CLIROLES_COL_ID_USER == j ) continue;
					if( DBS_SYS_TBL_CLIROLES_COL_ID_PWD == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetInteger(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = %d\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, row = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CLT:
		{
			for( i=1;i<=4;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CLT;j++ )
				{
					if( DBS_SYS_TBL_CLT_COL_ID_MAC == j ) continue;
					if( DBS_SYS_TBL_CLT_COL_ID_SWVER == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetInteger(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = %d\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, row = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CLTPRO:
		{
			for( i=1;i<=4;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CLTPRO;j++ )
				{
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetInteger(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = %d\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, row = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CNU:
		{
			for( i=1;i<=256;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CNU;j++ )
				{
					if( DBS_SYS_TBL_CNU_COL_ID_MAC == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_VER == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_SNR == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_BPC == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_ATT == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetInteger(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = %d\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, row = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CNUDEPRO:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CNUDEPRO;j++ )
				{
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetInteger(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = %d\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, row = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_NETWORK:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_NETWORK;j++ )
				{
					if( DBS_SYS_TBL_NETWORK_COL_ID_IP == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_MASK == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_GW == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_DNS == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_MAC == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetInteger(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = %d\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, row = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CNUPRO:
		{
			for( i=1;i<=256;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CNUPRO;j++ )
				{
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetInteger(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = %d\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, row = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_SNMPINFO:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_SNMPINFO;j++ )
				{
					if( DBS_SYS_TBL_SNMP_COL_ID_RC == j ) continue;
					if( DBS_SYS_TBL_SNMP_COL_ID_WC == j ) continue;
					if( DBS_SYS_TBL_SNMP_COL_ID_TC == j ) continue;
					if( DBS_SYS_TBL_SNMP_COL_ID_SA == j ) continue;
					if( DBS_SYS_TBL_SNMP_COL_ID_SB == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetInteger(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = %d\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, row = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_SWMGMT:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_SWMGMT;j++ )
				{
					if( DBS_SYS_TBL_SWMGMT_COL_ID_IP == j ) continue;
					if( DBS_SYS_TBL_SWMGMT_COL_ID_USR == j ) continue;
					if( DBS_SYS_TBL_SWMGMT_COL_ID_PWD == j ) continue;
					if( DBS_SYS_TBL_SWMGMT_COL_ID_PATH == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetInteger(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = %d\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, row = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_SYSINFO:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_SYSINFO;j++ )
				{
					if( DBS_SYS_TBL_SYSINFO_COL_ID_HWVER == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_BVER == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_KVER == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_APPVER == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_MF == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetInteger(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = %d\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, row = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_TEMPLATE:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_TEMPLATE;j++ )
				{					
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetInteger(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = %d\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, row = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		default:
		{
			printf("input error !\n\n");
		}
	}
	
	return 0;
}

int TEST_DBS_UPDATE_INTEGER(T_DBS_DEV_INFO *dev)
{
	int n = 0;

	DB_INTEGER_V v1;
	DB_INTEGER_V v2;

	printf("\n Please select the table id:\n");
	printf("	[0] tbl_cliroles\n");
	printf("	[1] tbl_clt\n");
	printf("	[2] tbl_cltconf\n");
	printf("	[3] tbl_cnu\n");
	printf("	[4] tbl_depro\n");
	printf("	[5] tbl_network\n");
	printf("	[6] tbl_profile\n");
	printf("	[7] tbl_snmp\n");
	printf("	[8] tbl_swmgmt\n");
	printf("	[9] tbl_sysinfo\n");
	printf("input: ");
	
	scanf("%d", &n);
	v1.ci.tbl = n;
	printf("\n Please input row id(1 is for the first row id): ");
	scanf("%d", &n);
	v1.ci.row = n;
	printf("\n Please input col id(0 is for the first col id): ");
	scanf("%d", &n);
	v1.ci.col = n;
	printf("\n Please input col value: ");
	scanf("%d", &n);
	v1.integer = n;
	v1.len = sizeof(uint32_t);
	printf(" Your input is : tbl = %d, row = %d, col = %d, value = %d\n", v1.ci.tbl, v1.ci.row, v1.ci.col, v1.integer);

	if( 0 == dbsUpdateInteger(dev, &v1) )
	{
		printf("-->ack : update success !\n");
		/* ¶ÁÈ¡Ð£Ñé*/
		v2.ci.tbl = v1.ci.tbl;
		v2.ci.row = v1.ci.row;
		v2.ci.col = v1.ci.col;
		v2.len = 0;
		v2.integer = 0;
		
		if( 0 == dbsGetInteger(dev, &v2) )
		{
			if( v1.len == 0 )
			{
				if( v2.len == 0 )
				{
					printf("-->ack : check success !\n");
				}
				else
				{
					printf("-->ack : check error !\n");
				}
			}
			else
			{
				if( v1.len != v2.len )
				{
					printf("-->ack : check error !\n");
				}
				else if( v1.integer != v2.integer )
				{
					printf("-->ack : check error !\n");
				}
				else
				{
					printf("-->ack : check success !\n");
				}
			}
		}
		else
		{
			printf("-->ack : check failed !\n");
		}
	}
	else
	{
		printf("-->ack : update failed !\n");
	}

	return 0;
}

int TEST_DBS_UPDATE_ALL_INTEGER(T_DBS_DEV_INFO *dev)
{
	int i = 0;
	int j = 0;
	int n = 0;
	DB_INTEGER_V v;

	printf("\n Please select the table id:\n");
	printf("	[0] tbl_cliroles\n");
	printf("	[1] tbl_clt\n");
	printf("	[2] tbl_cltconf\n");
	printf("	[3] tbl_cnu\n");
	printf("	[4] tbl_depro\n");
	printf("	[5] tbl_network\n");
	printf("	[6] tbl_profile\n");
	printf("	[7] tbl_snmp\n");
	printf("	[8] tbl_swmgmt\n");
	printf("	[9] tbl_sysinfo\n");
	printf("input: ");
	
	scanf("%d", &n);
	v.ci.tbl = n;
	printf("\n Please input an integer value: ");
	scanf("%d", &n);
	v.integer = n;
	v.len = sizeof(uint32_t);

	switch(v.ci.tbl)
	{
		case DBS_SYS_TBL_ID_CLIROLE:
		{
			for( i=1;i<=3;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CLIROLE;j++ )
				{
					if( DBS_SYS_TBL_CLIROLES_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_CLIROLES_COL_ID_USER == j ) continue;
					if( DBS_SYS_TBL_CLIROLES_COL_ID_PWD == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateInteger(dev, &v) )
					{
						if( v.len == 0 )
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=null) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=%d) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : dbsTester_updateInteger(tbl=%d, key=%d, col=%d, value=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col, v.integer);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CLT:
		{
			for( i=1;i<=4;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CLT;j++ )
				{
					if( DBS_SYS_TBL_CLT_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_CLT_COL_ID_MAC == j ) continue;
					if( DBS_SYS_TBL_CLT_COL_ID_SWVER == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateInteger(dev, &v) )
					{
						if( v.len == 0 )
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=null) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=%d) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : dbsTester_updateInteger(tbl=%d, key=%d, col=%d, value=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col, v.integer);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CLTPRO:
		{
			for( i=1;i<=4;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CLTPRO;j++ )
				{
					if( DBS_SYS_TBL_CLTPRO_COL_ID_ID == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateInteger(dev, &v) )
					{
						if( v.len == 0 )
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=null) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=%d) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : dbsTester_updateInteger(tbl=%d, key=%d, col=%d, value=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col, v.integer);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CNU:
		{
			for( i=1;i<=256;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CNU;j++ )
				{
					if( DBS_SYS_TBL_CNU_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_MAC == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_VER == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_SNR == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_BPC == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_ATT == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateInteger(dev, &v) )
					{
						if( v.len == 0 )
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=null) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=%d) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : dbsTester_updateInteger(tbl=%d, key=%d, col=%d, value=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col, v.integer);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CNUDEPRO:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CNUDEPRO;j++ )
				{
					if( DBS_SYS_TBL_DEPRO_COL_ID_ID == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateInteger(dev, &v) )
					{
						if( v.len == 0 )
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=null) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=%d) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : dbsTester_updateInteger(tbl=%d, key=%d, col=%d, value=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col, v.integer);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_NETWORK:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_NETWORK;j++ )
				{
					if( DBS_SYS_TBL_NETWORK_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_IP == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_MASK == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_GW == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_DNS == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_MAC == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateInteger(dev, &v) )
					{
						if( v.len == 0 )
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=null) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=%d) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : dbsTester_updateInteger(tbl=%d, key=%d, col=%d, value=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col, v.integer);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CNUPRO:
		{
			for( i=1;i<=256;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CNUPRO;j++ )
				{
					if( DBS_SYS_TBL_PROFILE_COL_ID_ID == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateInteger(dev, &v) )
					{
						if( v.len == 0 )
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=null) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=%d) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : dbsTester_updateInteger(tbl=%d, key=%d, col=%d, value=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col, v.integer);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_SNMPINFO:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_SNMPINFO;j++ )
				{
					if( DBS_SYS_TBL_SNMP_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_SNMP_COL_ID_RC == j ) continue;
					if( DBS_SYS_TBL_SNMP_COL_ID_WC == j ) continue;
					if( DBS_SYS_TBL_SNMP_COL_ID_TC == j ) continue;
					if( DBS_SYS_TBL_SNMP_COL_ID_SA == j ) continue;
					if( DBS_SYS_TBL_SNMP_COL_ID_SB == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateInteger(dev, &v) )
					{
						if( v.len == 0 )
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=null) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=%d) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : dbsTester_updateInteger(tbl=%d, key=%d, col=%d, value=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col, v.integer);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_SWMGMT:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_SWMGMT;j++ )
				{
					if( DBS_SYS_TBL_SWMGMT_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_SWMGMT_COL_ID_IP == j ) continue;
					if( DBS_SYS_TBL_SWMGMT_COL_ID_USR == j ) continue;
					if( DBS_SYS_TBL_SWMGMT_COL_ID_PWD == j ) continue;
					if( DBS_SYS_TBL_SWMGMT_COL_ID_PATH == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateInteger(dev, &v) )
					{
						if( v.len == 0 )
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=null) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=%d) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : dbsTester_updateInteger(tbl=%d, key=%d, col=%d, value=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col, v.integer);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_SYSINFO:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_SYSINFO;j++ )
				{
					if( DBS_SYS_TBL_SYSINFO_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_HWVER == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_BVER == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_KVER == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_APPVER == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_MF == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateInteger(dev, &v) )
					{
						if( v.len == 0 )
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=null) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : dbsTester_updateInteger(tbl=%d, row=%d, col=%d, value=%d) success !\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : dbsTester_updateInteger(tbl=%d, key=%d, col=%d, value=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col, v.integer);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_TEMPLATE:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_TEMPLATE;j++ )
				{					
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetInteger(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col);
						}
						else
						{
							printf("-->ack : tbl = %d, row = %d, col = %d, value = %d\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.integer);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, row = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		
		default:
		{
			printf("input error !\n\n");
		}
	}
	
	return 0;
}

int TEST_DBS_GET_TEXT(T_DBS_DEV_INFO *dev)
{
	int n = 0;
	DB_TEXT_V v;

	printf("\n Please select the table id:\n");
	printf("	[0] tbl_cliroles\n");
	printf("	[1] tbl_clt\n");
	printf("	[2] tbl_cltconf\n");
	printf("	[3] tbl_cnu\n");
	printf("	[4] tbl_depro\n");
	printf("	[5] tbl_network\n");
	printf("	[6] tbl_profile\n");
	printf("	[7] tbl_snmp\n");
	printf("	[8] tbl_swmgmt\n");
	printf("	[9] tbl_sysinfo\n");
	printf("input: ");
	
	scanf("%d", &n);
	v.ci.tbl = n;
	printf("\n Please input row id(1 is for the first row id): ");
	scanf("%d", &n);
	v.ci.row = n;
	printf("\n Please input col id(0 is for the first col id): ");
	scanf("%d", &n);
	v.ci.col = n;
	printf(" Your input is : tbl = %d, key = %d, col = %d\n", v.ci.tbl, v.ci.row, v.ci.col);

	if( 0 == dbsGetText(dev, &v) )
	{
		if( DBS_NULL == v.ci.colType )
		{
			printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = (null)\n", 
				v.ci.tbl, v.ci.row, v.ci.col, v.len);
		}
		else
		{
			printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = \"%s\"\n", 
				v.ci.tbl, v.ci.row, v.ci.col, v.len, v.text);
		}
	}
	else
	{
		printf("-->ack : failed !\n");
	}

	return 0;
}

int TEST_DBS_GET_ALL_TEXT(T_DBS_DEV_INFO *dev)
{
	int i = 0;
	int j = 0;
	int n = 0;
	DB_TEXT_V v;

	printf("\n Please select the table id:\n");
	printf("	[0] tbl_cliroles\n");
	printf("	[1] tbl_clt\n");
	printf("	[2] tbl_cltconf\n");
	printf("	[3] tbl_cnu\n");
	printf("	[4] tbl_depro\n");
	printf("	[5] tbl_network\n");
	printf("	[6] tbl_profile\n");
	printf("	[7] tbl_snmp\n");
	printf("	[8] tbl_swmgmt\n");
	printf("	[9] tbl_sysinfo\n");
	printf("input: ");
	
	scanf("%d", &n);
	v.ci.tbl = n;

	switch(v.ci.tbl)
	{
		case DBS_SYS_TBL_ID_CLIROLE:
		{
			for( i=1;i<=3;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CLIROLE;j++ )
				{
					if( DBS_SYS_TBL_CLIROLES_COL_ID_ID == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetText(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.len);
						}
						else
						{
							printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = \"%s\"\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.len, v.text);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, key = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CLT:
		{
			for( i=1;i<=4;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CLT;j++ )
				{
					if( DBS_SYS_TBL_CLT_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_CLT_COL_ID_MODEL == j ) continue;
					if( DBS_SYS_TBL_CLT_COL_ID_STS == j ) continue;
					if( DBS_SYS_TBL_CLT_COL_ID_MAXSTATS == j ) continue;
					if( DBS_SYS_TBL_CLT_COL_ID_NUMSTATS == j ) continue;
					if( DBS_SYS_TBL_CLT_COL_ID_SYNCH == j ) continue;
					if( DBS_SYS_TBL_CLT_COL_ID_ROWSTS == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetText(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.len);
						}
						else
						{
							printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = \"%s\"\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.len, v.text);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, key = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CLTPRO:
		{
			break;
		}
		case DBS_SYS_TBL_ID_CNU:
		{
			for( i=1;i<=256;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CNU;j++ )
				{
					if( DBS_SYS_TBL_CNU_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_MODEL == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_STS == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_AUTH == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_RX == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_TX == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_SYNCH == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_ROWSTS == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetText(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.len);
						}
						else
						{
							printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = \"%s\"\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.len, v.text);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, key = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CNUDEPRO:
		{
			break;
		}
		case DBS_SYS_TBL_ID_NETWORK:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_NETWORK;j++ )
				{
					if( DBS_SYS_TBL_NETWORK_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_DHCP == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_MVSTS == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_MVID == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetText(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.len);
						}
						else
						{
							printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = \"%s\"\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.len, v.text);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, key = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CNUPRO:
		{
			break;
		}
		case DBS_SYS_TBL_ID_SNMPINFO:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_SNMPINFO;j++ )
				{
					if( DBS_SYS_TBL_SNMP_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_SNMP_COL_ID_PA == j ) continue;
					if( DBS_SYS_TBL_SNMP_COL_ID_PB == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetText(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.len);
						}
						else
						{
							printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = \"%s\"\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.len, v.text);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, key = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_SWMGMT:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_SWMGMT;j++ )
				{
					if( DBS_SYS_TBL_SWMGMT_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_SWMGMT_COL_ID_PORT == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetText(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.len);
						}
						else
						{
							printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = \"%s\"\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.len, v.text);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, key = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_SYSINFO:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_SYSINFO;j++ )
				{
					if( DBS_SYS_TBL_SYSINFO_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_MODEL == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_MAXCLT == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_FS == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_RS == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_WLCTL == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_AU == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_AC == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_WDT == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsGetText(dev, &v) )
					{
						if( DBS_NULL == v.ci.colType )
						{
							printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = (null)\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.len);
						}
						else
						{
							printf("-->ack : tbl = %d, key = %d, col = %d, len = %d, value = \"%s\"\n", 
								v.ci.tbl, v.ci.row, v.ci.col, v.len, v.text);
						}
					}
					else
					{
						printf("-->ack : tbl = %d, key = %d, col = %d failed !\n", v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}		
		default:
		{
			printf("input error !\n\n");
		}
	}
	
	return 0;
}

int TEST_DBS_UPDATE_TEXT(T_DBS_DEV_INFO *dev)
{
	int n = 0;
	DB_TEXT_V v1;
	DB_TEXT_V v2;
	uint8_t testString[256] = {0};

	printf("\n Please select the table id:\n");
	printf("	[0] tbl_cliroles\n");
	printf("	[1] tbl_clt\n");
	printf("	[2] tbl_cltconf\n");
	printf("	[3] tbl_cnu\n");
	printf("	[4] tbl_depro\n");
	printf("	[5] tbl_network\n");
	printf("	[6] tbl_profile\n");
	printf("	[7] tbl_snmp\n");
	printf("	[8] tbl_swmgmt\n");
	printf("	[9] tbl_sysinfo\n");
	printf("input: ");
	
	scanf("%d", &n);
	v1.ci.tbl = n;
	printf("\n Please input row id(1 is for the first row id): ");
	scanf("%d", &n);
	v1.ci.row = n;
	printf("\n Please input col id(0 is for the first col id): ");
	scanf("%d", &n);
	v1.ci.col = n;
	printf("\n Please input a string: ");
	scanf("%s", testString);
	v1.len = strlen(testString);
	strncpy(v1.text, testString, v1.len);
	v1.text[v1.len] = '\0';
	
	printf(" Your input is : tbl = %d, row = %d, col = %d, value = \"%s\"\n", v1.ci.tbl, v1.ci.row, v1.ci.col, v1.text);

	if( 0 == dbsUpdateText(dev, &v1) )
	{
		printf("-->ack : update success !\n");
		/* ¶ÁÈ¡Ð£Ñé*/
		v2.ci.tbl = v1.ci.tbl;
		v2.ci.row = v1.ci.row;
		v2.ci.col = v1.ci.col;
		v2.len = 0;
		v2.text[0] = '\0';
		
		if( 0 == dbsGetText(dev, &v2) )
		{
			if( 0 == v1.len )
			{
				if( 0 == v2.len )
				{
					printf("-->ack : check success !\n");
				}
				else
				{
					printf("-->ack : check error !\n");
				}
			}
			else if( v2.len != v1.len )
			{
				printf("-->ack : check error !\n");
			}
			else
			{
				if( memcmp(v2.text, v1.text, v2.len) == 0 )
				{
					printf("-->ack : check success !\n");
				}
				else
				{
					printf("-->ack : check error !\n");
				}
			}
		}
		else
		{
			printf("-->ack : check failed !\n");
		}
	}
	else
	{
		printf("-->ack : update failed !\n");
	}

	return 0;
}

int TEST_DBS_UPDATE_ALL_TEXT(T_DBS_DEV_INFO *dev)
{
	int i = 0;
	int j = 0;
	int n = 0;
	uint8_t testString[256] = {0};
	DB_TEXT_V v;

	printf("\n Please select the table id:\n");
	printf("	[0] tbl_cliroles\n");
	printf("	[1] tbl_clt\n");
	printf("	[2] tbl_cltconf\n");
	printf("	[3] tbl_cnu\n");
	printf("	[4] tbl_depro\n");
	printf("	[5] tbl_network\n");
	printf("	[6] tbl_profile\n");
	printf("	[7] tbl_snmp\n");
	printf("	[8] tbl_swmgmt\n");
	printf("	[9] tbl_sysinfo\n");
	printf("input: ");
	
	scanf("%d", &n);
	v.ci.tbl = n;
	printf("\n Please input a text value: ");
	scanf("%s", testString);
	v.len = strlen(testString);
	strncpy(v.text, testString, v.len);
	v.text[v.len] = '\0';

	switch(v.ci.tbl)
	{
		case DBS_SYS_TBL_ID_CLIROLE:
		{
			for( i=1;i<=3;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CLIROLE;j++ )
				{
					if( DBS_SYS_TBL_CLIROLES_COL_ID_ID == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateText(dev, &v) )
					{
						printf("-->ack : dbsTester_updateText(tbl=%d, key=%d, col=%d) success !\n", 
							v.ci.tbl, v.ci.row, v.ci.col);
					}
					else
					{
						printf("-->ack : dbsTester_updateText(tbl=%d, key=%d, col=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CLT:
		{
			for( i=1;i<=4;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CLT;j++ )
				{
					if( DBS_SYS_TBL_CLT_COL_ID_MAC != j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateText(dev, &v) )
					{
						printf("-->ack : dbsTester_updateText(tbl=%d, key=%d, col=%d) success !\n", 
							v.ci.tbl, v.ci.row, v.ci.col);
					}
					else
					{
						printf("-->ack : dbsTester_updateText(tbl=%d, key=%d, col=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CLTPRO:
		{
			break;
		}
		case DBS_SYS_TBL_ID_CNU:
		{
			for( i=1;i<=256;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_CNU;j++ )
				{
					if( DBS_SYS_TBL_CNU_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_MODEL == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_STS == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_AUTH == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_RX == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_TX == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_SYNCH == j ) continue;
					if( DBS_SYS_TBL_CNU_COL_ID_ROWSTS == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateText(dev, &v) )
					{
						printf("-->ack : dbsTester_updateText(tbl=%d, key=%d, col=%d) success !\n", 
							v.ci.tbl, v.ci.row, v.ci.col);
					}
					else
					{
						printf("-->ack : dbsTester_updateText(tbl=%d, key=%d, col=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CNUDEPRO:
		{
			break;
		}
		case DBS_SYS_TBL_ID_NETWORK:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_NETWORK;j++ )
				{
					if( DBS_SYS_TBL_NETWORK_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_DHCP == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_MVSTS == j ) continue;
					if( DBS_SYS_TBL_NETWORK_COL_ID_MVID == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateText(dev, &v) )
					{
						printf("-->ack : dbsTester_updateText(tbl=%d, key=%d, col=%d) success !\n", 
							v.ci.tbl, v.ci.row, v.ci.col);
					}
					else
					{
						printf("-->ack : dbsTester_updateText(tbl=%d, key=%d, col=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_CNUPRO:
		{
			break;
		}
		case DBS_SYS_TBL_ID_SNMPINFO:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_SNMPINFO;j++ )
				{
					if( DBS_SYS_TBL_SNMP_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_SNMP_COL_ID_PA == j ) continue;
					if( DBS_SYS_TBL_SNMP_COL_ID_PB == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateText(dev, &v) )
					{
						printf("-->ack : dbsTester_updateText(tbl=%d, key=%d, col=%d) success !\n", 
							v.ci.tbl, v.ci.row, v.ci.col);
					}
					else
					{
						printf("-->ack : dbsTester_updateText(tbl=%d, key=%d, col=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_SWMGMT:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_SWMGMT;j++ )
				{
					if( DBS_SYS_TBL_SWMGMT_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_SWMGMT_COL_ID_PORT == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateText(dev, &v) )
					{
						printf("-->ack : dbsTester_updateText(tbl=%d, key=%d, col=%d) success !\n", 
							v.ci.tbl, v.ci.row, v.ci.col);
					}
					else
					{
						printf("-->ack : dbsTester_updateText(tbl=%d, key=%d, col=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}
		case DBS_SYS_TBL_ID_SYSINFO:
		{
			for( i=1;i<=1;i++)
			{
				for( j=0;j<DBS_SYS_TBL_COLS_SYSINFO;j++ )
				{
					if( DBS_SYS_TBL_SYSINFO_COL_ID_ID == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_MODEL == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_MAXCLT == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_FS == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_RS == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_WLCTL == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_AU == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_AC == j ) continue;
					if( DBS_SYS_TBL_SYSINFO_COL_ID_WDT == j ) continue;
					v.ci.row = i;
					v.ci.col = j;
					if( 0 == dbsUpdateText(dev, &v) )
					{
						printf("-->ack : dbsTester_updateText(tbl=%d, key=%d, col=%d) success !\n", 
							v.ci.tbl, v.ci.row, v.ci.col);
					}
					else
					{
						printf("-->ack : dbsTester_updateText(tbl=%d, key=%d, col=%d) ERROR !\n", 
							v.ci.tbl, v.ci.row, v.ci.col);
					}
				}
			}
			break;
		}		
		default:
		{
			printf("input error !\n\n");
		}
	}
	
	return 0;
}

int TEST_DBS_MSG_DEBUG_ENABLE(T_DBS_DEV_INFO *dev)
{
	return dbsMsgDebug(dev, 1);
}

int TEST_DBS_MSG_DEBUG_DISABLE(T_DBS_DEV_INFO *dev)
{
	return dbsMsgDebug(dev, 0);
}

int TEST_DBS_SQL_DEBUG_ENABLE(T_DBS_DEV_INFO *dev)
{
	return dbsSQLDebug(dev, 1);
}

int TEST_DBS_SQL_DEBUG_DISABLE(T_DBS_DEV_INFO *dev)
{
	return dbsSQLDebug(dev, 0);
}

int TEST_DBS_WRITE_SYSLOG(T_DBS_DEV_INFO *dev)
{
	int i = 0;
	uint8_t syslog[256] = {0};
	
	while(1)
	{
		bzero(syslog, 256);
		usleep(100);
		sprintf(syslog, "TEST_DBS_WRITE_SYSLOG[%d]", ++i);
		dbs_sys_log(dev, DBS_LOG_DEBUG, syslog);
	}

	return 0;
}

int TEST_DBS_SHOW_SYSLOG(T_DBS_DEV_INFO *dev)
{
	int i = 0;
	uint32_t n = 0;
	st_dbsSyslog log;

	if( 0 != dbsLogCount(dev, DBS_LOG_TBL_ID_SYS, &n))
	{
		printf("-->ack : TEST_DBS_SHOW_SYSLOG->dbsLogCount() ERROR !\n");
		return -1;
	}
	else
	{
		printf("TEST_DBS_SHOW_SYSLOG : [%d]\n", n);
		for( i=n;i>0;i--)
		{			
			if( 0 != dbsGetSyslog(dev, i, &log) )
			{
				printf("-->ack : TEST_DBS_SHOW_SYSLOG->dbsGetSyslog(%d) ERROR !\n", i);
				break;
			}
			else
			{
				printf("%d | %d | %d | %s\n", log.time, log.who, log.level, log.log);
			}
		}
	}
	return 0;
}

int TEST_DBS_WRITE_OPTLOG(T_DBS_DEV_INFO *dev)
{
	int i = 0;
	time_t b_time;
	st_dbsOptlog log;	
	
	while(1)
	{
		bzero(&log, sizeof(log));
		
		time(&b_time);		
		log.time = b_time;
		log.who = MID_DBS_TESTER;
		sprintf(log.cmd, "TEST_DBS_WRITE_OPTLOG[%d]", ++i);
		log.level = DBS_LOG_DEBUG;
		log.result = CMM_SUCCESS;
		
		dbs_opt_log(dev, &log);	
		
		usleep(100);		
	}

	return 0;
}

int TEST_DBS_SHOW_OPTLOG(T_DBS_DEV_INFO *dev)
{
	int i = 0;
	uint32_t n = 0;
	st_dbsOptlog log;

	if( 0 != dbsLogCount(dev, DBS_LOG_TBL_ID_OPT, &n))
	{
		printf("-->ack : TEST_DBS_SHOW_OPTLOG->dbsLogCount() ERROR !\n");
		return -1;
	}
	else
	{
		printf("TEST_DBS_SHOW_OPTLOG : [%d]\n", n);
		for( i=n;i>0;i--)
		{			
			if( 0 != dbsGetOptlog(dev, i, &log) )
			{
				printf("-->ack : TEST_DBS_SHOW_OPTLOG->dbsGetOptlog(%d) ERROR !\n", i);
				break;
			}
			else
			{
				printf("%d | %d | %s | %d | %d\n", log.time, log.who, log.cmd, log.level, log.result);
			}
		}
	}
	return 0;
}

int TEST_DBS_WRITE_ALARMLOG(T_DBS_DEV_INFO *dev)
{
	int i = 0;
	time_t b_time;
	st_dbsAlarmlog log;	
	
	while(1)
	{
		bzero(&log, sizeof(log));

		
		log.alarmCode = 888888;
		log.alarmType = 1;
		log.alarmValue = 666666;
		strcpy(log.cbatMac, "30:71:b2:00:00:00");
		log.cltId = 0;
		log.cnuId = 0;
		log.itemNum = 0;
		strcpy(log.oid, "1.3.6.86186.5.9.1.1.0");
		time(&b_time);
		log.realTime = b_time;
		log.serialFlow = ++i;
		sprintf(log.trap_info, "TEST_DBS_WRITE_ALARMLOG[%d]", i);
		
		dbs_alarm_log(dev, &log);	
		
		usleep(100);		
	}

	return 0;
}

int TEST_DBS_SHOW_ALARMLOG(T_DBS_DEV_INFO *dev)
{
	int i = 0;
	uint32_t n = 0;
	st_dbsAlarmlog log;

	if( 0 != dbsLogCount(dev, DBS_LOG_TBL_ID_ALARM, &n))
	{
		printf("-->ack : TEST_DBS_SHOW_ALARMLOG->dbsLogCount() ERROR !\n");
		return -1;
	}
	else
	{
		printf("TEST_DBS_SHOW_ALARMLOG : [%d]\n", n);
		for( i=n;i>0;i--)
		{			
			if( 0 != dbsGetAlarmlog(dev, i, &log) )
			{
				printf("-->ack : TEST_DBS_SHOW_ALARMLOG->dbsGetSyslog(%d) ERROR !\n", i);
				break;
			}
			else
			{
				printf("%d |%d |%d |%s |%d |%d |%d |%s |%d |%d |%s\n", 
					log.alarmCode, log.alarmType, log.alarmValue, log.cbatMac, 
					log.cltId, log.cnuId, log.itemNum, log.oid, 
					log.realTime, log.serialFlow, log.trap_info);
			}
		}
	}
	return 0;
}

int TEST_DBS_REGIST_MODULE(T_DBS_DEV_INFO *dev)
{
	int n = 0;
	uint16_t mid = 0;

	printf("\n Please input the module id [1~32]:\n");
	printf("input: ");
	
	scanf("%d", &n);
	mid = n;
	
	if( 0 == dbsRegisterModuleById(dev, mid) )
	{
		printf("TEST_DBS_REGIST_MODULE : SUCCESS !\n");
	}
	else
	{
		printf("TEST_DBS_REGIST_MODULE : FAILED !\n");
	}
	
	return 0;
}

int TEST_DBS_DESTROY_MODULE(T_DBS_DEV_INFO *dev)
{
	int n = 0;
	uint16_t mid = 0;

	printf("\n Please input the module id [1~32]:\n");
	printf("input: ");
	
	scanf("%d", &n);
	mid = n;
	
	if( 0 == dbsDestroyModuleById(dev, mid) )
	{
		printf("TEST_DBS_DESTROY_MODULE : SUCCESS !\n");
	}
	else
	{
		printf("TEST_DBS_DESTROY_MODULE : FAILED !\n");
	}
	
	return 0;
}

int TEST_DBS_REGIST_ALL_MODULES(T_DBS_DEV_INFO *dev)
{
	uint16_t mid = 0;

	for( mid=1;mid<=MAX_MODULE_NUMS;mid++)
	{
		if( 0 == dbsRegisterModuleById(dev, mid) )
		{
			printf("TEST_DBS_REGIST_ALL_MODULES : MID[%d] SUCCESS !\n", mid);
		}
		else
		{
			printf("TEST_DBS_REGIST_ALL_MODULES : MID[%d] FAILED !\n", mid);
		}
	}	
	return 0;
}

int TEST_DBS_DESTROY_ALL_MODULES(T_DBS_DEV_INFO *dev)
{
	uint16_t mid = 0;

	for( mid=1;mid<=MAX_MODULE_NUMS;mid++)
	{
		if( 0 == dbsDestroyModuleById(dev, mid) )
		{
			printf("TEST_DBS_DESTROY_ALL_MODULES : MID[%d] SUCCESS !\n", mid);
		}
		else
		{
			printf("TEST_DBS_DESTROY_ALL_MODULES : MID[%d] FAILED !\n", mid);
		}
	}	
	return 0;
}

int TEST_DBS_WAIT_ALL_MODULES(T_DBS_DEV_INFO *dev)
{
	uint32_t MF = 0xFFFFFFFF;

	dbsWaitModule(dev, MF);	
	return 0;
}

int TEST_DBS_TBL_ROW_INTERFACES(T_DBS_DEV_INFO *dev)
{
	int tbl_id = 0;
	int tbl_row = 0;	
	int tbl = 0;
	int id = 0;

	printf("\n Please select the table id:\n");
	printf("	[0] tbl_cliroles\n");
	printf("	[1] tbl_clt\n");
	printf("	[2] tbl_cltconf\n");
	printf("	[3] tbl_cnu\n");
	printf("	[4] tbl_depro\n");
	printf("	[5] tbl_network\n");
	printf("	[6] tbl_profile\n");
	printf("	[7] tbl_snmp\n");
	printf("	[8] tbl_swmgmt\n");
	printf("	[9] tbl_sysinfo\n");
	printf("input: ");
	
	scanf("%d", &tbl_id);
	tbl = tbl_id;
	printf("\n Please input row id(1 is for the first row id): ");
	scanf("%d", &tbl_row);
	id = tbl_row;

	printf("\n==================================================================\n\n");

	switch(tbl)
	{
		case DBS_SYS_TBL_ID_CLIROLE:
		{
			__test_dbs_row_tbl_clirole_interfaces(dev, id);
			break;
		}
		case DBS_SYS_TBL_ID_CLT:
		{
			__test_dbs_row_tbl_clt_interfaces(dev, id);
			break;
		}
		case DBS_SYS_TBL_ID_CLTPRO:
		{
			__test_dbs_row_tbl_cltconf_interfaces(dev, id);
			break;
		}
		case DBS_SYS_TBL_ID_CNU:
		{
			__test_dbs_row_tbl_cnu_interfaces(dev, id);
			break;
		}
		case DBS_SYS_TBL_ID_CNUDEPRO:
		{
			__test_dbs_row_tbl_depro_interfaces(dev, id);
			break;
		}
		case DBS_SYS_TBL_ID_NETWORK:
		{
			__test_dbs_row_tbl_network_interfaces(dev, id);
			break;
		}
		case DBS_SYS_TBL_ID_CNUPRO:
		{
			__test_dbs_row_tbl_profile_interfaces(dev, id);
			break;
		}
		case DBS_SYS_TBL_ID_SNMPINFO:
		{
			__test_dbs_row_tbl_snmp_interfaces(dev, id);
			break;
		}
		case DBS_SYS_TBL_ID_SWMGMT:
		{
			__test_dbs_row_tbl_swmgmt_interfaces(dev, id);
			break;
		}
		case DBS_SYS_TBL_ID_SYSINFO:
		{
			__test_dbs_row_tbl_sysinfo_interfaces(dev, id);
			break;
		}
		default:
		{
			printf("input error !\n\n");
			break;
		}
	}

	printf("\n==================================================================\n");
	return 0;
}

int TEST_DBS_TBL_ALL_ROW_INTERFACES(T_DBS_DEV_INFO *dev)
{
	int id = 1;
	
	/* step 1 : test row interfaces tbl_cliroles */
	for( id=1;id<=3;id++ )
	{
		__test_dbs_row_tbl_clirole_interfaces(dev, id);
	}
	/* step 2 : test row interfaces tbl_clt */
	for( id=1;id<=MAX_CLT_AMOUNT_LIMIT;id++ )
	{
		__test_dbs_row_tbl_clt_interfaces(dev, id);
	}
	/* step 3 : test row interfaces tbl_cltconf */
	for( id=1;id<=MAX_CLT_AMOUNT_LIMIT;id++ )
	{
		__test_dbs_row_tbl_cltconf_interfaces(dev, id);
	}
	/* step 4 : test row interfaces tbl_cnu */
	for( id=1;id<=(MAX_CNU_AMOUNT_LIMIT);id++ )
	{
		__test_dbs_row_tbl_cnu_interfaces(dev, id);
	}
	/* step 5 : test row interfaces tbl_depro */
	for( id=1;id<=1;id++ )
	{
		__test_dbs_row_tbl_depro_interfaces(dev, id);
	}
	/* step 6 : test row interfaces tbl_network */
	for( id=1;id<=1;id++ )
	{
		__test_dbs_row_tbl_network_interfaces(dev, id);
	}
	/* step 7 : test row interfaces tbl_profile */
	for( id=1;id<=(MAX_CNU_AMOUNT_LIMIT);id++ )
	{
		__test_dbs_row_tbl_profile_interfaces(dev, id);
	}
	/* step 8 : test row interfaces tbl_snmp */
	for( id=1;id<=1;id++ )
	{
		__test_dbs_row_tbl_snmp_interfaces(dev, id);
	}
	/* step 9 : test row interfaces tbl_swmgmt */
	for( id=1;id<=1;id++ )
	{
		__test_dbs_row_tbl_swmgmt_interfaces(dev, id);
	}
	/* step 10 : test row interfaces tbl_sysinfo */
	for( id=1;id<=1;id++ )
	{
		__test_dbs_row_tbl_sysinfo_interfaces(dev, id);
	}
	
	return 0;
}

int TEST_DBS_CREATE_SU_PROFILE_INTERFACES(T_DBS_DEV_INFO *dev)
{
	int id = 1;
	for( id=1;id<=(MAX_CNU_AMOUNT_LIMIT);id++ )
	{
		if( 0 != __test_dbs_create_su_profile_interfaces(dev, id) )
		{
			printf("-->TEST_DBS_CREATE_SU_PROFILE_INTERFACES [%d] ERROR !\n", id);
		}
		else
		{
			printf("-->TEST_DBS_CREATE_SU_PROFILE_INTERFACES [%d] SUCCESS !\n", id);
		}
	}
	return 0;
}

int TEST_DBS_CREATE_DEWL_PROFILE_INTERFACES(T_DBS_DEV_INFO *dev)
{
	int id = 1;
	for( id=1;id<=(MAX_CNU_AMOUNT_LIMIT);id++ )
	{
		if( 0 != __test_dbs_create_dewl_profile_interfaces(dev, id) )
		{
			printf("-->TEST_DBS_CREATE_DEWL_PROFILE_INTERFACES [%d] ERROR !\n", id);
		}
		else
		{
			printf("-->TEST_DBS_CREATE_DEWL_PROFILE_INTERFACES [%d] SUCCESS !\n", id);
		}
	}
	return 0;
}

int TEST_DBS_CREATE_DEBL_PROFILE_INTERFACES(T_DBS_DEV_INFO *dev)
{
	int id = 1;
	for( id=1;id<=(MAX_CNU_AMOUNT_LIMIT);id++ )
	{
		if( 0 != __test_dbs_create_debl_profile_interfaces(dev, id) )
		{
			printf("-->TEST_DBS_CREATE_DEBL_PROFILE_INTERFACES [%d] ERROR !\n", id);
		}
		else
		{
			printf("-->TEST_DBS_CREATE_DEBL_PROFILE_INTERFACES [%d] SUCCESS !\n", id);
		}
	}
	return 0;
}

int TEST_DBS_DESTROY_CLT_CNU_INTERFACES(T_DBS_DEV_INFO *dev)
{
	int id = 1;
	printf("\n==================================================================\n");
	for( id=1;id<=MAX_CLT_AMOUNT_LIMIT;id++ )
	{
		if( 0 != __test_dbs_destroy_clt_interfaces(dev, id) )
		{
			printf("-->__test_dbs_destroy_clt_interfaces [%d] ERROR !\n", id);
		}
		else
		{
			printf("-->__test_dbs_destroy_clt_interfaces [%d] SUCCESS !\n", id);
		}
	}
	printf("\n==================================================================\n");
	for( id=1;id<=MAX_CLT_AMOUNT_LIMIT;id++ )
	{
		if( 0 != __test_dbs_destroy_cltconf_interfaces(dev, id) )
		{
			printf("-->__test_dbs_destroy_cltconf_interfaces [%d] ERROR !\n", id);
		}
		else
		{
			printf("-->__test_dbs_destroy_cltconf_interfaces [%d] SUCCESS !\n", id);
		}
	}
	printf("\n==================================================================\n");
	for( id=1;id<=(MAX_CNU_AMOUNT_LIMIT);id++ )
	{
		if( 0 != __test_dbs_destroy_cnu_interfaces(dev, id) )
		{
			printf("-->__test_dbs_destroy_cnu_interfaces [%d] ERROR !\n", id);
		}
		else
		{
			printf("-->__test_dbs_destroy_cnu_interfaces [%d] SUCCESS !\n", id);
		}
	}
	printf("\n==================================================================\n");
	for( id=1;id<=(MAX_CNU_AMOUNT_LIMIT);id++ )
	{
		if( 0 != __test_dbs_destroy_profile_interfaces(dev, id) )
		{
			printf("-->__test_dbs_destroy_profile_interfaces [%d] ERROR !\n", id);
		}
		else
		{
			printf("-->__test_dbs_destroy_profile_interfaces [%d] SUCCESS !\n", id);
		}
	}
	printf("\n==================================================================\n");
	return 0;
}

int TEST_DBS_SELECT_CNU_BY_MAC(T_DBS_DEV_INFO *dev)
{
	char strmac[32] = {0};
	stCnuNode iNode;
	struct timeval start, end;

	printf("Please input a mac address: ");

	scanf("%s", strmac);
	if( CMM_SUCCESS != boardapi_mac2Uppercase(strmac) )
	{
		printf("\nMAC Address is invalid\n");
		return CMM_FAILED;
	}	

	gettimeofday( &start, NULL );
	if( CMM_SUCCESS == dbsSelectCnuIndexByMacAddress(dev, strmac, &iNode) )
	{
		gettimeofday( &end, NULL );
		printf("\nCNU is selected: [CNU/%d/%d]\n", iNode.clt, iNode.cnu);
		printf("\nTime Used: %d Seconds %ul Microseconds\n\n", 
			(int)(end.tv_sec - start.tv_sec),
			(uint32_t)(end.tv_usec - start.tv_usec)
		);
	}
	else
	{
		printf("\nCnu cannot be selected\n\n");
	}
	
	return CMM_SUCCESS;
}


int main(int argc, char *argv[])
{	
	if( argc != 3 )
	{
		dbsTester_usage();
		return 0;
	}

	dev = dbsNoWaitOpen(MID_DBS_TESTER);
	if( NULL == dev )
	{
		return 0;
	}

	/* ×¢²áÒì³£ÍË³ö¾ä±úº¯Êý*/
	signal(SIGTERM, dbsTester_signalProcessHandle);
	signal(SIGINT, dbsTester_signalProcessHandle);

	dbs_sys_log(dev, DBS_LOG_INFO, "starting module dbsTester success");
	printf("Starting module dbsTester	......		[OK]\n\n");

	printf("\n==================================================================\n\n");
	if( strcmp(argv[1], "msg-debug") == 0)
	{
		if( strcmp(argv[2], "enable") == 0)
		{
			TEST_DBS_MSG_DEBUG_ENABLE(dev);
		}
		else if( strcmp(argv[2], "disable") == 0)
		{
			TEST_DBS_MSG_DEBUG_DISABLE(dev);
		}
		else
		{
			dbsTester_usage();
		}
	}
	else if( strcmp(argv[1], "sql-debug") == 0)
	{
		if( strcmp(argv[2], "enable") == 0)
		{
			TEST_DBS_SQL_DEBUG_ENABLE(dev);
		}
		else if( strcmp(argv[2], "disable") == 0)
		{
			TEST_DBS_SQL_DEBUG_DISABLE(dev);
		}
		else
		{
			dbsTester_usage();
		}
	}
	else if( strcmp(argv[1], "test") == 0)
	{
		int cmd = atoi(argv[2]);
		switch(cmd)
		{
			case 0:
				TEST_DBS_FFLUSH(dev);
				break;
			case 1:
				TEST_DBS_GET_INTEGER(dev);
				break;
			case 2:
				TEST_DBS_GET_ALL_INTEGER(dev);
				break;
			case 3:
				TEST_DBS_UPDATE_INTEGER(dev);
				break;
			case 4:
				TEST_DBS_UPDATE_ALL_INTEGER(dev);
				break;
			case 5:
				TEST_DBS_GET_TEXT(dev);
				break;
			case 6:
				TEST_DBS_GET_ALL_TEXT(dev);
				break;
			case 7:
				TEST_DBS_UPDATE_TEXT(dev);
				break;
			case 8:
				TEST_DBS_UPDATE_ALL_TEXT(dev);
				break;
			case 9:
				TEST_DBS_WRITE_SYSLOG(dev);
				break;
			case 10:
				TEST_DBS_SHOW_SYSLOG(dev);
				break;
			case 11:
				TEST_DBS_WRITE_OPTLOG(dev);
				break;
			case 12:
				TEST_DBS_SHOW_OPTLOG(dev);
				break;
			case 13:
				TEST_DBS_WRITE_ALARMLOG(dev);
				break;
			case 14:
				TEST_DBS_SHOW_ALARMLOG(dev);
				break;
			case 15:
				TEST_DBS_REGIST_MODULE(dev);
				break;
			case 16:
				TEST_DBS_DESTROY_MODULE(dev);
				break;
			case 17:
				TEST_DBS_REGIST_ALL_MODULES(dev);
				break;
			case 18:
				TEST_DBS_DESTROY_ALL_MODULES(dev);
				break;
			case 19:
				TEST_DBS_WAIT_ALL_MODULES(dev);
				break;
			case 20:
				TEST_DBS_TBL_ROW_INTERFACES(dev);
				break;
			case 21:
				TEST_DBS_TBL_ALL_ROW_INTERFACES(dev);
				break;
			case 22:
				TEST_DBS_CREATE_SU_PROFILE_INTERFACES(dev);
				break;
			case 23:
				TEST_DBS_CREATE_DEWL_PROFILE_INTERFACES(dev);
				break;
			case 24:
				TEST_DBS_CREATE_DEBL_PROFILE_INTERFACES(dev);
				break;
			case 25:
				TEST_DBS_DESTROY_CLT_CNU_INTERFACES(dev);
				break;
			case 26:
				TEST_DBS_SELECT_CNU_BY_MAC(dev);
				break;
			default:
				dbsTester_usage();
				break;
		}
	}
	else
	{
		dbsTester_usage();
	}
	printf("\n\n==================================================================\n");
	dbs_sys_log(dev, DBS_LOG_INFO, "module dbsTester exit");
	dbsClose(dev);
	return 0;	
}

