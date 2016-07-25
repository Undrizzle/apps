#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/mii.h>

#include <msApi.h>
#include <dbsapi.h>

#include "cmm_dsdt.h"

#ifdef MULTI_ADDR_MODE
#undef MULTI_ADDR_MODE
#endif

#ifdef MANUAL_MODE
#undef MANUAL_MODE
#endif


GT_SYS_CONFIG   cfg;
GT_QD_DEV       diagDev;
GT_QD_DEV       *dev = &diagDev;
extern T_DBS_DEV_INFO *dbsdev;

GT_BOOL MV88E6171R_SMI_READ (GT_QD_DEV* dev, unsigned int portNumber , unsigned int MIIReg,
                        unsigned int* value)
{

    	int fd;
	struct ifreq ifr; 
	struct mii_ioctl_data *smi_data = NULL;	

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
	{
		perror("  MV88E6171R_SMI_READ socket error !\n");     
		return GT_FALSE;
	}

	memset(&ifr,0,sizeof(ifr)); 
	strcpy(ifr.ifr_name, "eth0"); 
	smi_data = (struct mii_ioctl_data *)&(ifr.ifr_ifru);
	
	smi_data->phy_id = portNumber;
	smi_data->reg_num = MIIReg;
	
	if(ioctl( fd, SIOCGMIIREG, &ifr ) < 0 )   
	{     
		perror("  MV88E6171R_SMI_READ ioctl error !\n");
		close(fd);
		return GT_FALSE;     
	}
	else
	{
		close(fd);
		*value = (smi_data->val_out)&0x0000ffff;
		return GT_TRUE;
	}
}


GT_BOOL MV88E6171R_SMI_WRITE (GT_QD_DEV* dev, unsigned int portNumber , unsigned int MIIReg,
                       unsigned int value)
{
	int fd;
	struct ifreq ifr; 
	struct mii_ioctl_data *smi_data = NULL;	

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
	{
		perror("  MV88E6171R_SMI_WRITE socket error !\n");     
		return GT_FALSE;
	}

	memset(&ifr,0,sizeof(ifr)); 
	strcpy(ifr.ifr_name, "eth0"); 
	smi_data = (struct mii_ioctl_data *)&(ifr.ifr_ifru);
	
	smi_data->phy_id = portNumber;
	smi_data->reg_num = MIIReg;
	smi_data->val_in = value;
	
	if( ioctl( fd, SIOCSMIIREG, &ifr ) < 0 )   
	{     
		perror("  MV88E6171R_SMI_WRITE ioctl error !\n");
		close(fd);
		return GT_FALSE;     
	}
	else
	{
		close(fd);
		return(GT_TRUE);
	}
}

GT_STATUS MV88E6171R_INIT(void)
{
	GT_STATUS status;
	GT_DOT1Q_MODE mode;
	int i;
	GT_U16 vid;
	GT_LPORT index;
	GT_LPORT portToSet;
	GT_VTU_ENTRY vtuEntry;
	GT_LPORT portList[MAX_SWITCH_PORTS];
	
	/* 1) Clear VLAN ID Table */
	if((status = gvtuFlush(dev)) != GT_OK)
	{
		printf("gvtuFlush returned fail.\n");
		return status;
	}

	/* 2) Set all ports to 802.1q disable Mode */
	for( i=0; i<dev->numOfPorts; i++)
	{
		if((status = gvlnSetPortVlanDot1qMode(dev, i, GT_DISABLE)) != GT_OK)
		{
			printf("gvlnSetPortVlanDot1qMode return Failed\n");
			return status;
		}
	}

	/* 3) Set Port VLAN Mapping for all ports */
	for( portToSet=0; portToSet<dev->numOfPorts; portToSet++ )
	{
		index = 0;
		for (i=0; i<dev->numOfPorts; i++)
		{
			if (i == portToSet)
			{
				continue;
			}
			else
			{
				portList[index++] = i;
			}
		}
		if((status = gvlnSetPortVlanPorts(dev,portToSet,portList,index)) != GT_OK)
		{
			printf(("gvlnSetPortVlanPorts returned fail.\n"));
			return status;
		}
	}

	/* 4) Set all ports DefaultVID (PVID) = 1*/
	vid = 1;
	for(i=0; i<dev->numOfPorts; i++)
	{
		if((status = gvlnSetPortVid(dev,i,vid)) != GT_OK)
		{
			printf("gvlnSetPortVid returned fail.\n");
			return status;
		}
	}

	/* 5) Set all ports Egress mode = 0 (egress unmodified) */
	for(i=0; i<dev->numOfPorts; i++)
	{
		if((status = gprtSetEgressMode(dev,i,GT_UNMODIFY_EGRESS)) != GT_OK)
		{
			printf("gprtSetEgressMode returned fail.\n");
			return status;
		}
	}
	
	/* init rgmii delay config in cmm init_network() */
#if 0
	/* 6) Enable port6 RGMII RX delay */
	if((status = gpcsSetRGMIITimingDelay(dev, 6, GT_TRUE, GT_FALSE)) != GT_OK)
	{
		printf("gpcsSetRGMIITimingDelay returned fail.\n");
		return status;
	}
#endif	
	return GT_OK;
}

/*
 *  Initialize the QuarterDeck. This should be done in BSP driver init routine.
 *    Since BSP is not combined with QuarterDeck driver, we are doing here.
*/
GT_STATUS dsdtStart(int cpuPort)
{
	GT_STATUS status;

	memset((char*)&cfg,0,sizeof(GT_SYS_CONFIG));
	memset((char*)&diagDev,0,sizeof(GT_QD_DEV));

	cfg.BSPFunctions.readMii   = MV88E6171R_SMI_READ;
	cfg.BSPFunctions.writeMii  = MV88E6171R_SMI_WRITE;
#ifdef GT_RMGMT_ACCESS
	cfg.BSPFunctions.hwAccess  = NULL; 
#endif
	cfg.BSPFunctions.semCreate = NULL;
	cfg.BSPFunctions.semDelete = NULL;
	cfg.BSPFunctions.semTake   = NULL;
	cfg.BSPFunctions.semGive   = NULL;	

	cfg.initPorts = GT_FALSE;    /* Set switch ports to Forwarding mode. If GT_FALSE, use Default Setting. */
	cfg.cpuPortNum = cpuPort;
	cfg.mode.scanMode = SMI_AUTO_SCAN_MODE;    /* Scan 0 or 0x10 base address to find the QD */
	cfg.mode.baseAddr = 0;
	
	if( (status=qdLoadDriver(&cfg, dev)) != GT_OK )
	{
		printf("qdLoadDriver return Failed\n");
		return status;
	}

	printf("Device ID     : 0x%x\n",dev->deviceId);
	//printf("Base Reg Addr : 0x%x\n",dev->baseRegAddr);
	//printf("No of Ports   : %d\n",dev->numOfPorts);
	printf("CPU Ports     : %d\n",dev->cpuPortNum);

	/*
	*  start the QuarterDeck
	*/
	if((status=sysEnable(dev)) != GT_OK)
	{
		printf("sysConfig return Failed\n");
		return status;
	}

	if( GT_OK != MV88E6171R_INIT() )
	{
		printf("MV88E6171R_INIT return Failed\n");
		return GT_FAIL;
	}

	//printf("QuarterDeck has been started\n");

	return GT_OK;
}

int __find_dbnum_by_vid(uint32_t vid)
{
	GT_STATUS status;
	GT_VTU_ENTRY vtuEntry;

	gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
	vtuEntry.vid = 0xfff;
	
	if((status = gvtuGetEntryFirst(dev,&vtuEntry)) != GT_OK)
	{
		/* if can not find vid in the VTU, return DBNum = 0 */
		printf("#can not find vid in the vtu\n");
		return 0;
	}
	else
	{
		if( vtuEntry.vid == vid )
		{
			return vtuEntry.DBNum;
		}		
	}

	while(1)
	{
		if((status = gvtuGetEntryNext(dev,&vtuEntry)) != GT_OK)
		{
			/* if can not find vid in the VTU, return DBNum = 0 */
			printf("$can not find vid in the vtu\n");
			return 0;
		}		
		if( vtuEntry.vid == vid )
		{
			return vtuEntry.DBNum;
		}
	}
}

GT_STATUS __undo_mgmt_vlan(void)
{
	int i;
	int Model;
	GT_STATUS status;
	GT_EGRESS_MODE mode;
	st_dbsSysinfo dsdtSysinfo;
	int hfcPort = 4;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &dsdtSysinfo))
	{
		return CMM_FAILED;
	} 

	if( dsdtSysinfo.col_model == 33 )
	{
		hfcPort = 6;
	}
	else if (dsdtSysinfo.col_model == 32 )
	{
		hfcPort = 0;
	}
	else if (  dsdtSysinfo.col_model == 36 )
	{
		hfcPort = 4;
	}
	/* 1) Set port 0-6 to 802.1q Fallback Mode */
	for( i=0; i<dev->numOfPorts; i++)
	{
		if((status = gvlnSetPortVlanDot1qMode(dev, i, GT_FALLBACK)) != GT_OK)
		{
			printf("gvlnSetPortVlanDot1qMode return Failed\n");
			return status;
		}
	}
	/* 2) Set port 0-6's DefaultVID (PVID) = 1 */
	/* Note: 2 is already done in MV88E6171R_INIT() */
	
	/* 3) Set port 0-3's Egress Mode = 0 (egress unmodified) */	
	/* 4) Set port hfc,cpu's Egress mode = 1 (egress untagged) */
	
	
	for( i=0; i<dev->numOfPorts; i++)
	{
		if( i == hfcPort)
		{
			mode = GT_UNTAGGED_EGRESS;
		}
		else if( i == dev->cpuPortNum )
		{
			mode = GT_UNTAGGED_EGRESS;
		}
		else
		{
			mode = GT_UNMODIFY_EGRESS;
		}
		if((status = gprtSetEgressMode(dev,i,mode)) != GT_OK)
		{
			printf("gprtSetEgressMode returned fail.\n");		
		}
	}
	
	return status;
}

GT_STATUS __do_mgmt_vlan(uint32_t vid)
{
	int i;
	GT_STATUS status;
	GT_DOT1Q_MODE vlanMode;
	GT_EGRESS_MODE egressMode;
	GT_VTU_ENTRY vtuEntry;
	st_dbsSysinfo dsdtSysinfo;
	int hfcPort = 4;

	if( CMM_SUCCESS != dbsGetSysinfo(dbsdev, 1, &dsdtSysinfo))
	{
		return CMM_FAILED;
	} 

	if( dsdtSysinfo.col_model == 33 )
	{
		hfcPort = 6;
	}
	else if ( dsdtSysinfo.col_model == 32 )
	{
		hfcPort = 0;
	}
	else if( dsdtSysinfo.col_model == 36 )
	{
		hfcPort = 4;
	}

#if 0	
	/* 1) Set port 0-4,6 to 802.1q Fallback Mode */
	/* 2) Set P5 to 802.1q secure mode */
	for( i=0; i<dev->numOfPorts; i++)
	{
		if( i == dev->cpuPortNum )
		{
			vlanMode = GT_SECURE;
		}
		else
		{
			vlanMode = GT_FALLBACK;
		}
		if((status = gvlnSetPortVlanDot1qMode(dev, i, vlanMode)) != GT_OK)
		{
			printf("gvlnSetPortVlanDot1qMode return Failed\n");
			return status;
		}
	}
#else
	/* 当启用管理VLAN时，AR7410的MME消息没有将管理VLAN携带回来
	    导致管理CPU和CLT不通，暂时添加此代码进行测试*/
	/* 1) Set port 0-6 to 802.1q Fallback Mode */
	for( i=0; i<dev->numOfPorts; i++)
	{
		if((status = gvlnSetPortVlanDot1qMode(dev, i, GT_FALLBACK)) != GT_OK)
		{
			printf("gvlnSetPortVlanDot1qMode return Failed\n");
			return status;
		}
	}
#endif
	/* 3) Set port 0-3,6's DefaultVID (PVID) = 1 */
	/* Note: 3 is already done in MV88E6171R_INIT() */
#if 0
	/* 当启用管理VLAN时，AR7410的MME消息没有将管理VLAN携带回来
	    导致管理CPU和CLT不通，暂时添加此代码进行测试*/
	if((status = gvlnSetPortVid(dev, 6, vid)) != GT_OK)
	{
		printf("gvlnSetPortVid return Failed\n");
		return status;
	}
#endif
	/* 4) Set hfc,cpu's DefaultVID (PVID) = vid */
	if((status = gvlnSetPortVid(dev, hfcPort, vid)) != GT_OK)
	{
		printf("gvlnSetPortVid return Failed\n");
		return status;
	}	
	if((status = gvlnSetPortVid(dev, dev->cpuPortNum, vid)) != GT_OK)
	{
		printf("gvlnSetPortVid return Failed\n");
		return status;
	}	
	/* 5) Set port 0-3's Egress Mode = 0 (egress unmodified) */	
	/* 6) Set port hfc,cpu's Egress mode = 1 (egress untagged) */
	for( i=0; i<dev->numOfPorts; i++)
	{
		if( i == hfcPort )
		{
			egressMode = GT_UNTAGGED_EGRESS;
		}
		else if( i == dev->cpuPortNum )
		{
			egressMode = GT_UNTAGGED_EGRESS;
		}
		else
		{
			egressMode = GT_UNMODIFY_EGRESS;
		}
		if((status = gprtSetEgressMode(dev,i,egressMode)) != GT_OK)
		{
			printf("gprtSetEgressMode returned fail.\n");		
		}
	}
	/* 7) Add VLAN vid VTU entry (MemberTag hfc,cpu = 1, egress untagged, and MemberTagP0-3 = 2, egress tagged) */
	gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
	vtuEntry.DBNum = 0;
	vtuEntry.vid = vid;
	for( i=0; i<dev->numOfPorts; i++ )
	{
		if( i == hfcPort )                     /* MemberTag hfc = 1 (egress untagged) */
		{
			vtuEntry.vtuData.memberTagP[i] = MEMBER_EGRESS_UNTAGGED;
		}
		else if( i == dev->cpuPortNum )	/* MemberTagP5 = 1 (egress untagged) */
		{
			vtuEntry.vtuData.memberTagP[i] = MEMBER_EGRESS_UNTAGGED;
		}
		else						/* MemberTagP0-3 = 2 (egress tagged) */
		{
			vtuEntry.vtuData.memberTagP[i] = MEMBER_EGRESS_TAGGED;
		}
	}
	if((status = gvtuAddEntry(dev,&vtuEntry)) != GT_OK)
	{
		printf("gvtuAddEntry returned fail.\n");
		return status;
	}
	
	return status;
}

GT_STATUS dsdtInitMgmtVlan(void)
{
	st_dbsNetwork networkinfo;

	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &networkinfo) )
	{
		printf("dbsGetNetwork return Failed\n");
		return GT_FAIL;
	}

	if( networkinfo.col_mvlan_sts )
	{
		if( (networkinfo.col_mvlan_id < 1) ||(networkinfo.col_mvlan_id > 4094) )
		{
			printf("Warnning: undo mgmt-vlan because mgmt-vlanid is invalid\n");
			return __undo_mgmt_vlan();
		}
		else
		{
			return __do_mgmt_vlan(networkinfo.col_mvlan_id);
		}
	}
	else
	{
		return __undo_mgmt_vlan();
	}	
}

void dsdtShowVtu(void)
{
	GT_STATUS ret = GT_OK;
	GT_U32 numEntries;

	printf("dsdTester_showVtu:\n");
	
	ret = gvtuGetEntryCount(dev, &numEntries);
	if( GT_OK == ret )
	{
		printf("  VTU entry numbers: %d\n", numEntries);
	}
	else
	{
		printf(" gvtuGetEntryCount failed\n");
	}
}

GT_STATUS dsdtShowVlan(void)
{
	int i;
	int j = 0;
	GT_STATUS status;
	GT_DOT1Q_MODE vlanMode;
	GT_EGRESS_MODE egressMode;
	GT_LPORT portList[MAX_SWITCH_PORTS];
	GT_U8 memPortsLen = 0;
	GT_U16 vid;
	GT_BOOL noEgrPolicy;

	printf("dsdTester_showVlan:\n");

	/* 0) show gloable egress policy status */
	if((status = gvlnGetNoEgrPolicy(dev,&noEgrPolicy)) != GT_OK)
	{
		printf("  gvlnGetNoEgrPolicy returned fail.\n");
		return status;
	}
	else
	{
		printf("  no gloable egress policy: %s\n", noEgrPolicy?"True":"False");
	}
	
	/* 1) show port vlan mode */
	for( i=0; i<dev->numOfPorts; i++ )
	{
		if((status = gvlnGetPortVlanDot1qMode(dev,i, &vlanMode)) != GT_OK)
		{
			printf("  gvlnGetPortVlanDot1qMode returned fail.\n");
			return status;
		}
		else
		{
			printf("  port %d 802.1q vlan mode: %d\n", i, vlanMode);
		}
	}

	/* 2) show port based VLANTable */
	for( i=0; i<dev->numOfPorts; i++ )
	{
		if((status = gvlnGetPortVlanPorts(dev,i, portList, &memPortsLen)) != GT_OK)
		{
			printf("  gvlnGetPortVlanPorts returned fail.\n");
			return status;
		}
		else
		{
			printf("  port %d port based vlan member ports: ", i);
			for( j=0; j<memPortsLen; j++ )
			{
				printf("%d ", portList[j]);
			}
			printf("\n");
		}
	}

	/* 3) show port pvid */
	for( i=0; i<dev->numOfPorts; i++ )
	{
		if((status = gvlnGetPortVid(dev,i, &vid)) != GT_OK)
		{
			printf("  gvlnGetPortVid returned fail.\n");
			return status;
		}
		else
		{
			printf("  port %d 802.1q DefaultVID: %d\n", i, vid);
		}
	}

	/* 4) show port Egress Mode */
	for( i=0; i<dev->numOfPorts; i++ )
	{
		if((status = gprtGetEgressMode(dev,i, &egressMode)) != GT_OK)
		{
			printf("  gprtGetEgressMode returned fail.\n");
			return status;
		}
		else
		{
			printf("  port %d EgressMode: %d\n", i, egressMode);
		}
	}

	return GT_OK;
}

int cmm2dsdt_clearPortCounters(void)
{
	GT_STATUS status;

	if((status = gstatsFlushAll(dev)) != GT_OK)
	{
		printf("gstatsFlushAll returned failed.\n");
		return CMM_FAILED;
	}
	if((status = gprtClearAllCtr(dev)) != GT_OK)
	{
		//Why call gprtClearAllCtr return failed???
		//printf("gprtClearAllCtr returned failed.\n");
		//return CMM_FAILED;
		return GT_OK;
	}
	return GT_OK;
}

int cmm2dsdt_getPortCtr(int port, T_CMM_PORT_STATS_INFO *stats)
{
	GT_PORT_STAT ctr;
	
	if( gprtGetPortCtr(dev, port, &ctr) != GT_OK )
	{
		return CMM_FAILED;
	}
	else
	{
		stats->rxCtr = ctr.rxCtr;
		stats->txCtr = ctr.txCtr;
		return CMM_SUCCESS;
	}
}

int cmm2dsdt_getPortAllCounters(int port, T_CMM_PORT_STATS_INFO *stats)
{
	GT_STATS_COUNTER_SET3 statsCounterSet;
	
	if( gstatsGetPortAllCounters3(dev, port, &statsCounterSet) != GT_OK )
	{
		printf("gstatsGetPortAllCounters3 returned failed.\n");
		return CMM_FAILED;
	}
	else
	{
		stats->InBroadcasts = statsCounterSet.InBroadcasts;
		stats->InMulticasts = statsCounterSet.InMulticasts;
		stats->InUnicasts = statsCounterSet.InUnicasts;
		stats->InGoodOctets = statsCounterSet.InGoodOctetsLo;
		stats->rxCtr = stats->InBroadcasts + stats->InMulticasts + stats->InUnicasts;

		stats->OutBroadcasts = statsCounterSet.OutBroadcasts;
		stats->OutMulticasts = statsCounterSet.OutMulticasts;
		stats->OutUnicasts = statsCounterSet.OutUnicasts;
		stats->OutGoodOctets = statsCounterSet.OutOctetsLo;
		stats->txCtr = stats->OutBroadcasts + stats->OutMulticasts + stats->OutUnicasts;
		
		return CMM_SUCCESS;
	}
}

int cmm2dsdt_debugPrintPortAllCounters(int port)
{
	GT_STATS_COUNTER_SET3 statsCounterSet;
	GT_PORT_STAT2 ctr2;
	
	if( gstatsGetPortAllCounters3(dev, port, &statsCounterSet) != GT_OK )
	{
		printf("gstatsGetPortAllCounters3 returned failed.\n");
		return CMM_FAILED;
	}
	else
	{
		printf("\r\n=======================================================");
		printf("\r\nprot %d:", port);
		printf("\r\n=======================================================");
		printf("\r\n  InGoodOctetsLo:	%u", statsCounterSet.InGoodOctetsLo);
		printf("\r\n  InGoodOctetsHi:	%u", statsCounterSet.InGoodOctetsHi);
		printf("\r\n  InBadOctets:	%u", statsCounterSet.InBadOctets);
		printf("\r\n  OutFCSErr:	%u", statsCounterSet.OutFCSErr);
		printf("\r\n  InUnicasts:	%u", statsCounterSet.InUnicasts);
		printf("\r\n  Deferred:	%u", statsCounterSet.Deferred);
		printf("\r\n  InBroadcasts:	%u", statsCounterSet.InBroadcasts);
		printf("\r\n  InMulticasts:	%u", statsCounterSet.InMulticasts);
		printf("\r\n  Octets64:	%u", statsCounterSet.Octets64);
		printf("\r\n  Octets127:	%u", statsCounterSet.Octets127);
		printf("\r\n  Octets255:	%u", statsCounterSet.Octets255);
		printf("\r\n  Octets511:	%u", statsCounterSet.Octets511);
		printf("\r\n  Octets1023:	%u", statsCounterSet.Octets1023);
		printf("\r\n  OctetsMax:	%u", statsCounterSet.OctetsMax);
		printf("\r\n  OutOctetsLo:	%u", statsCounterSet.OutOctetsLo);
		printf("\r\n  OutOctetsHi:	%u", statsCounterSet.OutOctetsHi);
		printf("\r\n  OutUnicasts:	%u", statsCounterSet.OutUnicasts);
		printf("\r\n  Excessive:	%u", statsCounterSet.Excessive);
		printf("\r\n  OutMulticasts:	%u", statsCounterSet.OutMulticasts);
		printf("\r\n  OutBroadcasts:	%u", statsCounterSet.OutBroadcasts);
		printf("\r\n  Single:	%u", statsCounterSet.Single);
		printf("\r\n  OutPause:	%u", statsCounterSet.OutPause);
		printf("\r\n  InPause:	%u", statsCounterSet.InPause);
		printf("\r\n  Multiple:	%u", statsCounterSet.Multiple);
		printf("\r\n  Undersize:	%u", statsCounterSet.Undersize);
		printf("\r\n  Fragments:	%u", statsCounterSet.Fragments);
		printf("\r\n  Oversize:	%u", statsCounterSet.Oversize);
		printf("\r\n  Jabber:	%u", statsCounterSet.Jabber);
		printf("\r\n  InMACRcvErr:	%u", statsCounterSet.InMACRcvErr);
		printf("\r\n  InFCSErr:	%u", statsCounterSet.InFCSErr);
		printf("\r\n  Collisions:	%u", statsCounterSet.Collisions);
		printf("\r\n  Late:	%u", statsCounterSet.Late);
		
		//return CMM_SUCCESS;
	}
	if( gprtGetPortCtr2(dev, port, &ctr2) != GT_OK )
	{
		printf("gprtGetPortCtr2 returned failed.\n");
		return CMM_FAILED;
	}
	else
	{
		printf("\r\n  inDiscardLo:	%u", ctr2.inDiscardLo);
		printf("\r\n  inDiscardHi:		%u", ctr2.inDiscardHi);
		printf("\r\n  inFiltered:	%u", ctr2.inFiltered);
		printf("\r\n  outFiltered:	%u\n", ctr2.outFiltered);		
		
		return CMM_SUCCESS;
	}
}


int cmm2dsdt_getPortLinkStatus(int port)
{
	GT_BOOL status;

	if( PORT_CABLE_PORT_NULL == port)
	{
		return 0;
	}
	else if( gprtGetLinkState(dev, port, &status) != GT_OK )
	{
		return 0;
	}
	else
	{
		return status?1:0;
	}
}

int cmm2dsdt_getPortLinkSpeed(int port)
{
	GT_PORT_SPEED_MODE speed;
	
	if( gprtGetSpeedMode(dev, port, &speed) != GT_OK )
	{
		return SPEED_UNKNOWN;
	}
	else
	{
		switch(speed)
		{
			case PORT_SPEED_10_MBPS:
			{
				return SPEED_10M;
			}
			case PORT_SPEED_100_MBPS:
			{
				return SPEED_100M;
			}
			case PORT_SPEED_1000_MBPS:
			{
				return SPEED_1000M;
			}
			default:
			{
				return SPEED_UNKNOWN;
			}
		}
	}
}

int cmm2dsdt_getPortLinkDuplex(int port)
{
	GT_BOOL mode;
	
	if( gprtGetDuplex(dev, port, &mode) != GT_OK )
	{
		return DUPLEX_HALF;
	}
	else
	{
		return mode?DUPLEX_FULL:DUPLEX_HALF;
	}
}

int cmm2dsdt_getPortPri(int port)
{
	GT_U8 pri;
	
	if( gqosGetDefFPri(dev, port, &pri) != GT_OK )
	{
		return 0;
	}
	else
	{
		return pri;
	}
}

int cmm2dsdt_getPortFlowControl(int port)
{
	GT_BOOL state;
	
	if( gprtGetFlowCtrl(dev, port, &state) != GT_OK )
	{
		return BOOL_FALSE;
	}
	else
	{
		return state;
	}
}

int cmm2dsdt_getPortState(int port)
{
	GT_PORT_STP_STATE state;
	
	if( gstpGetPortState(dev, port, &state) != GT_OK )
	{
		return BOOL_FALSE;
	}
	else
	{
		switch(state)
		{
			case GT_PORT_LEARNING:
			case GT_PORT_FORWARDING:
			{
				return BOOL_TRUE;
			}
			default:
			{
				return BOOL_FALSE;
			}
		}
	}
}

int cmm2dsdt_getRgmiiTimingDelay(st_dsdtRgmiiTimingDelay *pdelay)
{
	GT_BOOL status;
	GT_BOOL rxmode;
	GT_BOOL txmode;

	status = gpcsGetRGMIITimingDelay(dev, pdelay->port, &rxmode, &txmode);
	if( GT_OK != status )
	{
		return CMM_FAILED;
	}
	else
	{
		pdelay->rxdelay = rxmode;
		pdelay->txdelay = txmode;
		return CMM_SUCCESS;
	}
}

int cmm2dsdt_setRgmiiTimingDelay(st_dsdtRgmiiTimingDelay *pdelay)
{
	GT_BOOL status;

	status = gpcsSetRGMIITimingDelay(dev, pdelay->port, pdelay->rxdelay, pdelay->txdelay);
	if( GT_OK != status )
	{
		return CMM_FAILED;
	}
	else
	{
		return CMM_SUCCESS;
	}
}

int cmm2dsdt_setPortMirroring(st_dsdtPortMirroring *pMirror)
{
	GT_BOOL status;

	status = gprtSetEgressMonitorSource(dev, pMirror->source, GT_TRUE);
	if( GT_OK != status )
	{
		return CMM_FAILED;
	}

	status = gprtSetIngressMonitorSource(dev, pMirror->source, GT_TRUE);
	if( GT_OK != status )
	{
		return CMM_FAILED;
	}

	status = gsysSetIngressMonitorDest(dev, pMirror->dest);
	if( GT_OK != status )
	{
		return CMM_FAILED;
	}

	status = gsysSetEgressMonitorDest(dev, pMirror->dest);
	if( GT_OK != status )
	{
		return CMM_FAILED;
	}

	return CMM_SUCCESS;
}

int cmm2dsdt_mgmtVlanInit(void)
{
	if( dsdtInitMgmtVlan() != GT_OK )
	{
		return CMM_FAILED;	
	}
	else
	{
		return CMM_SUCCESS;
	}
}

int cmm2dsdt_addAtherosMulticastAddress2Port(int portid)
{
	st_dbsNetwork networkinfo;
	int status;
	GT_ATU_ENTRY macEntry;	

	macEntry.macAddr.arEther[0] = 0x00;
	macEntry.macAddr.arEther[1] = 0xb0;
	macEntry.macAddr.arEther[2] = 0x52;
	macEntry.macAddr.arEther[3] = 0x00;
	macEntry.macAddr.arEther[4] = 0x00;
	macEntry.macAddr.arEther[5] = 0x01;

	macEntry.portVec = (1 << portid);
	macEntry.prio = 0;            /* Priority (2bits). When these bits are used they override
                                any other priority determined by the frame's data. This value is
                                meaningful only if the device does not support extended priority
                                information such as MAC Queue Priority and MAC Frame Priority */
	macEntry.exPrio.macQPri = 0;    /* If device doesnot support MAC Queue Priority override, 
                                    this field is ignored. */
	macEntry.exPrio.macFPri = 0;    /* If device doesnot support MAC Frame Priority override, 
                                    this field is ignored. */
	macEntry.exPrio.useMacFPri = 0;    /* If device doesnot support MAC Frame Priority override, 
                                    this field is ignored. */
	macEntry.entryState.ucEntryState = GT_UC_STATIC;
                                /* This address is locked and will not be aged out.
                                Refer to GT_ATU_UC_STATE in msApiDefs.h for other option. */
	macEntry.trunkMember = GT_FALSE;
								
	/* get mgmt-vlan status */
	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &networkinfo) )
	{
		printf("dbsGetNetwork return Failed\n");
		return GT_FAIL;
	}

	if( networkinfo.col_mvlan_sts )
	{
		/*if mgmt-vlan is enabled: find vid in the VTU */
		macEntry.DBNum = __find_dbnum_by_vid(networkinfo.col_mvlan_id);	
	}
	else
	{
		/*if mgmt-vlan is disabled: add 00:b0:52:00:00:01 to DBNum 0*/
		macEntry.DBNum = 0;
	}

	/* Add the MAC Address */
	return gfdbAddMacEntry(dev,&macEntry);
}

int cmm2dsdt_addAtherosMulticastAddressToAllCablePort(void)
{
	st_dbsNetwork networkinfo;
	int status;
	GT_ATU_ENTRY macEntry;	

	macEntry.macAddr.arEther[0] = 0x00;
	macEntry.macAddr.arEther[1] = 0xb0;
	macEntry.macAddr.arEther[2] = 0x52;
	macEntry.macAddr.arEther[3] = 0x00;
	macEntry.macAddr.arEther[4] = 0x00;
	macEntry.macAddr.arEther[5] = 0x01;

#ifdef CFG_USE_PLATFORM_WEC9720EK
	macEntry.portVec = (1 << PORT_CABLE1_PORT_ID);     /* clt Port number. 7bits are used for portVector. */
#endif

#ifdef CFG_USE_PLATFORM_WR1004JL
	macEntry.portVec = (1 << PORT_CABLE1_PORT_ID);     /* clt Port number. 7bits are used for portVector. */
#endif

#ifdef CFG_USE_PLATFORM_WR1004SJL
	macEntry.portVec = (1 << PORT_CABLE1_PORT_ID)
						|(1 << PORT_CABLE2_PORT_ID)
						|(1 << PORT_CABLE3_PORT_ID)
						|(1 << PORT_CABLE4_PORT_ID);     /* clt Port number. 7bits are used for portVector. */
#endif

#ifdef CFG_USE_PLATFORM_WR1004SJLD
	macEntry.portVec = (1 << PORT_CABLE1_PORT_ID)
						|(1 << PORT_CABLE2_PORT_ID);     /* clt Port number. 7bits are used for portVector. */
#endif

	macEntry.prio = 0;            /* Priority (2bits). When these bits are used they override
                                any other priority determined by the frame's data. This value is
                                meaningful only if the device does not support extended priority
                                information such as MAC Queue Priority and MAC Frame Priority */

	macEntry.exPrio.macQPri = 0;    /* If device doesnot support MAC Queue Priority override, 
                                    this field is ignored. */
	macEntry.exPrio.macFPri = 0;    /* If device doesnot support MAC Frame Priority override, 
                                    this field is ignored. */
	macEntry.exPrio.useMacFPri = 0;    /* If device doesnot support MAC Frame Priority override, 
                                    this field is ignored. */

	macEntry.entryState.ucEntryState = GT_UC_STATIC;
                                /* This address is locked and will not be aged out.
                                Refer to GT_ATU_UC_STATE in msApiDefs.h for other option. */

	macEntry.trunkMember = GT_FALSE;
								
	/* get mgmt-vlan status */
	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &networkinfo) )
	{
		printf("dbsGetNetwork return Failed\n");
		return GT_FAIL;
	}

	if( networkinfo.col_mvlan_sts )
	{
		/*if mgmt-vlan is enabled: find vid in the VTU */
		macEntry.DBNum = __find_dbnum_by_vid(networkinfo.col_mvlan_id);	
	}
	else
	{
		/*if mgmt-vlan is disabled: add 00:b0:52:00:00:01 to DBNum 0*/
		macEntry.DBNum = 0;
	}

	/* Add the MAC Address */
	if((status = gfdbAddMacEntry(dev,&macEntry)) != GT_OK)
	{
		printf("gfdbAddMacEntry returned fail.\n");
		return status;
	}

	return GT_OK;
}

int cmm2dsdt_delAtherosMulticastAddressFromAtu(void)
{
	int status;
	st_dbsNetwork networkinfo;	
	GT_ATU_ENTRY macEntry;	

	macEntry.macAddr.arEther[0] = 0x00;
	macEntry.macAddr.arEther[1] = 0xb0;
	macEntry.macAddr.arEther[2] = 0x52;
	macEntry.macAddr.arEther[3] = 0x00;
	macEntry.macAddr.arEther[4] = 0x00;
	macEntry.macAddr.arEther[5] = 0x01;
								
	/* get mgmt-vlan status */
	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &networkinfo) )
	{
		printf("dbsGetNetwork return Failed\n");
		return GT_FAIL;
	}

	if( networkinfo.col_mvlan_sts )
	{
		/*if mgmt-vlan is enabled: find vid in the VTU */
		macEntry.DBNum = __find_dbnum_by_vid(networkinfo.col_mvlan_id);	
	}
	else
	{
		/*if mgmt-vlan is disabled: add 00:b0:52:00:00:01 to DBNum 0*/
		macEntry.DBNum = 0;
	}

	/* Delete the MAC Address from ATU */
	return gfdbDelAtuEntry(dev, &macEntry);
}

int cmm2dsdt_bindingMacAddress(stDsdtMacBinding *macBindingInfo)
{
	st_dbsNetwork networkinfo;
	int status;
	GT_ATU_ENTRY macEntry;

	memcpy(macEntry.macAddr.arEther, macBindingInfo->mac, 6);

	macEntry.portVec = (1 << macBindingInfo->portid);
	macEntry.prio = 0;            /* Priority (2bits). When these bits are used they override
                                any other priority determined by the frame's data. This value is
                                meaningful only if the device does not support extended priority
                                information such as MAC Queue Priority and MAC Frame Priority */
	macEntry.exPrio.macQPri = 0;    /* If device doesnot support MAC Queue Priority override, 
                                    this field is ignored. */
	macEntry.exPrio.macFPri = 0;    /* If device doesnot support MAC Frame Priority override, 
                                    this field is ignored. */
	macEntry.exPrio.useMacFPri = 0;    /* If device doesnot support MAC Frame Priority override, 
                                    this field is ignored. */
	macEntry.entryState.ucEntryState = GT_UC_STATIC;
                                /* This address is locked and will not be aged out.
                                Refer to GT_ATU_UC_STATE in msApiDefs.h for other option. */
	macEntry.trunkMember = GT_FALSE;
								
	/* get mgmt-vlan status */
	if( CMM_SUCCESS != dbsGetNetwork(dbsdev, 1, &networkinfo) )
	{
		printf("dbsGetNetwork return Failed\n");
		return GT_FAIL;
	}

	if( networkinfo.col_mvlan_sts )
	{
		/*if mgmt-vlan is enabled: find vid in the VTU */
		macEntry.DBNum = __find_dbnum_by_vid(networkinfo.col_mvlan_id);	
	}
	else
	{
		/*if mgmt-vlan is disabled: add 00:b0:52:00:00:01 to DBNum 0*/
		macEntry.DBNum = 0;
	}

	/* Add the MAC Address */
	return gfdbAddMacEntry(dev,&macEntry);
}

int cmm2dsdt_init(void)
{
	int cpuPort = 5;
	
	if( dsdtStart(cpuPort) != GT_OK )
	{
		return CMM_FAILED;	
	}
	else
	{
		return CMM_SUCCESS;
	}
}

