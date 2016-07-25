/********************************************************************************
* msApiInit.c
*
* DESCRIPTION:
*       MS API initialization routine
*
* DEPENDENCIES:   Platform
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
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
#include <public.h>
#include <dbsapi.h>
#include <wecplatform.h>

#include "msSample.h"

#ifdef MULTI_ADDR_MODE
#undef MULTI_ADDR_MODE
#endif

#ifdef MANUAL_MODE
#undef MANUAL_MODE
#endif


GT_SYS_CONFIG   cfg;
GT_QD_DEV       diagDev;
GT_QD_DEV       *dev = &diagDev;
static T_DBS_DEV_INFO *dbsdev = NULL;
int hfcPort = 4;

void dsdTester_usage(void)
{
	printf("\nParameter error:\n");
	printf("\nUsage:\n");
	printf("dsdTester <case>\n");
	printf("  --case/0/1: init mgmt-vlan from dbs\n");
	printf("  --case/1/1: port based vlan case/1/1\n");
	printf("  --case/1/2: port based vlan case/1/2\n");
	printf("  --case/1/3: port based vlan case/1/3\n");
	printf("  --case/2/1: 802.1q fallback mode case/2/1\n");
	printf("  --case/2/2: 802.1q fallback mode case/2/2\n");
	printf("  --case/2/3: 802.1q fallback mode case/2/3\n");
	printf("  --case/3/1: secure + port based vlan case/3/1\n");
	printf("  --case/3/2: secure + port based vlan case/3/2\n");
	printf("  --case/3/3: secure + port based vlan case/3/3\n");
	printf("  --case/3/4: secure + port based vlan case/3/4\n");
	printf("  --case/4/1: secure + fallback vlan case/4/1\n");
	printf("  --case/5/1: display switch rmon conter1\n");
	printf("  --case/5/2: display switch rmon conter2\n");
	printf("  --case/5/3: display switch rmon conter3\n");
	printf("  --case/6/1: show all the vid entry in the VTU\n");
	printf("  --case/6/2: show all the Mac entry in the ATU\n");
	printf("  --case/6/3: sample add intellon multicast address 00:b0:52:00:00:01 in the ATU, P6 only\n");
	printf("  --case/6/4: sample del intellon multicast address 00:b0:52:00:00:01 from the ATU\n");
	printf("  --case/6/5: sample add intellon multicast address 00:b0:52:00:00:01 in the ATU, and all cable port\n");
	printf("  --case/7/1 1 0x200000 0x3D: 1 is Enable ,cbsLimit 0x200000 < 0xFFFFFF , cbsIncreament < 0xFF sample:: --case/7/1 0 is disable Storm prevent test for broadcast unknow unicast and multicast\n");
	printf("  --case/8/1: reset 88e6171r by software\n");
	printf("  --case/9/1: show Port vlan status\n");
	printf("\n\n");
}

void dsdTester_signalProcessHandle(int n)
{
	printf("\n\n==================================================================\n");
	fprintf(stderr, "INFO: dsdTester_signalProcessHandle close progress !\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "dsdTester_signalProcessHandle : module dsdTester exit");
	dbsClose(dbsdev);		
	exit(0);
}


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
	printf("Base Reg Addr : 0x%x\n",dev->baseRegAddr);
	printf("No of Ports   : %d\n",dev->numOfPorts);
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

	printf("QuarterDeck has been started.\n");

	return GT_OK;
}

/*
 *  Initialize the QuarterDeck. This should be done in BSP driver init routine.
 *    Since BSP is not combined with QuarterDeck driver, we are doing here.
*/
GT_STATUS dsdtInit(int cpuPort)
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
	printf("Base Reg Addr : 0x%x\n",dev->baseRegAddr);
	printf("No of Ports   : %d\n",dev->numOfPorts);
	printf("CPU Ports     : %d\n",dev->cpuPortNum);

	/*
	*  start the QuarterDeck
	*/
	if((status=sysEnable(dev)) != GT_OK)
	{
		printf("sysConfig return Failed\n");
		return status;
	}

	printf("QuarterDeck has been started.\n");

	return GT_OK;
}

GT_STATUS __undo_mgmt_vlan(void)
{
	int i;
	GT_STATUS status;
	GT_EGRESS_MODE mode;
	
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
	
	/* 3) Set port 0-3,6 's Egress Mode = 0 (egress unmodified) */	
	/* 4) Set port 4,5's Egress mode = 1 (egress untagged) */
	for( i=0; i<dev->numOfPorts; i++)
	{
		if( i == hfcPort )
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
	
	/* 1) Set port 0-3,6 to 802.1q Fallback Mode */
	/* 2) Set P4,5 to 802.1q secure mode */
	for( i=0; i<dev->numOfPorts; i++)
	{
		if( i == hfcPort )
		{
			vlanMode = GT_SECURE;
		}
		else if( i == dev->cpuPortNum )
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
	/* 3) Set port 0-3,6's DefaultVID (PVID) = 1 */
	/* Note: 3 is already done in MV88E6171R_INIT() */
	/* 4) Set P4,5's DefaultVID (PVID) = vid */
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
	/* 5) Set port 0-3,6 's Egress Mode = 0 (egress unmodified) */	
	/* 6) Set port 4,5's Egress mode = 1 (egress untagged) */
	for( i=0; i<dev->numOfPorts; i++)
	{
		if( i == hfcPort)
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
	/* 7) Add VLAN vid VTU entry (MemberTagP4,5 = 1, egress untagged, and MemberTagP0-3,6 = 2, egress tagged) */
	gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
	vtuEntry.DBNum = 0;
	vtuEntry.vid = vid;
	for( i=0; i<dev->numOfPorts; i++ )
	{
		if( i == hfcPort )                           /* MemberTagP4 = 1 (egress untagged) */
		{
			vtuEntry.vtuData.memberTagP[i] = MEMBER_EGRESS_UNTAGGED;
		}
		else if( i == dev->cpuPortNum )	/* MemberTagP5 = 1 (egress untagged) */
		{
			vtuEntry.vtuData.memberTagP[i] = MEMBER_EGRESS_UNTAGGED;
		}
		else						/* MemberTagP0-3,6 = 2 (egress tagged) */
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

void __displayCounter(GT_STATS_COUNTER_SET *statsCounter)
{
    printf("InUnicasts    %08i    ", statsCounter->InUnicasts);
    printf("InBroadcasts  %08i   \n", statsCounter->InBroadcasts);
    printf("InPause       %08i    ", statsCounter->InPause);
    printf("InMulticasts  %08i   \n", statsCounter->InMulticasts);
    printf("InFCSErr      %08i    ", statsCounter->InFCSErr);
    printf("AlignErr      %08i   \n", statsCounter->AlignErr);
    printf("InGoodOctets  %08i    ", statsCounter->InGoodOctets);
    printf("InBadOctets   %08i   \n", statsCounter->InBadOctets);
    printf("Undersize     %08i    ", statsCounter->Undersize);
    printf("Fragments     %08i   \n", statsCounter->Fragments);
    printf("In64Octets    %08i    ", statsCounter->In64Octets);
    printf("In127Octets   %08i   \n", statsCounter->In127Octets);
    printf("In255Octets   %08i    ", statsCounter->In255Octets);
    printf("In511Octets   %08i   \n", statsCounter->In511Octets);
    printf("In1023Octets  %08i    ", statsCounter->In1023Octets);
    printf("InMaxOctets   %08i   \n", statsCounter->InMaxOctets);
    printf("Jabber        %08i    ", statsCounter->Jabber);
    printf("Oversize      %08i   \n", statsCounter->Oversize);
    printf("InDiscards    %08i    ", statsCounter->InDiscards);
    printf("Filtered      %08i   \n", statsCounter->Filtered);
    printf("OutUnicasts   %08i    ", statsCounter->OutUnicasts);
    printf("OutBroadcasts %08i   \n", statsCounter->OutBroadcasts);
    printf("OutPause      %08i    ", statsCounter->OutPause);
    printf("OutMulticasts %08i   \n", statsCounter->OutMulticasts);
    printf("OutFCSErr     %08i    ", statsCounter->OutFCSErr);
    printf("OutGoodOctets %08i   \n", statsCounter->OutGoodOctets);
    printf("Out64Octets   %08i    ", statsCounter->Out64Octets);
    printf("Out127Octets  %08i   \n", statsCounter->Out127Octets);
    printf("Out255Octets  %08i    ", statsCounter->Out255Octets);
    printf("Out511Octets  %08i   \n", statsCounter->Out511Octets);
    printf("Out1023Octets %08i    ", statsCounter->Out1023Octets);
    printf("OutMaxOctets  %08i   \n", statsCounter->OutMaxOctets);
    printf("Collisions    %08i    ", statsCounter->Collisions);
    printf("Late          %08i   \n", statsCounter->Late);
    printf("Excessive     %08i    ", statsCounter->Excessive);
    printf("Multiple      %08i   \n", statsCounter->Multiple);
    printf("Single        %08i    ", statsCounter->Single);
    printf("Deferred      %08i   \n", statsCounter->Deferred);
    printf("OutDiscards   %08i   \n", statsCounter->OutDiscards);
}

void __displayCounter2(GT_STATS_COUNTER_SET2 *statsCounter)
{
    printf("InGoodOctetsHi  %08i    ", statsCounter->InGoodOctetsHi);
    printf("InGoodOctetsLo  %08i   \n", statsCounter->InGoodOctetsLo);
    printf("InBadOctets     %08i    ", statsCounter->InBadOctets);
    printf("OutDiscards     %08i   \n", statsCounter->OutDiscards);
    printf("InGoodFrames    %08i    ", statsCounter->InGoodFrames);
    printf("InBadFrames     %08i   \n", statsCounter->InBadFrames);
    printf("InBroadcasts    %08i    ", statsCounter->InBroadcasts);
    printf("InMulticasts    %08i   \n", statsCounter->InMulticasts);
    printf("64Octets        %08i    ", statsCounter->Octets64);
    printf("127Octets       %08i   \n", statsCounter->Octets127);
    printf("255Octets       %08i    ", statsCounter->Octets255);
    printf("511Octets       %08i   \n", statsCounter->Octets511);
    printf("1023Octets      %08i    ", statsCounter->Octets1023);
    printf("MaxOctets       %08i   \n", statsCounter->OctetsMax);
    printf("OutOctetsHi     %08i    ", statsCounter->OutOctetsHi);
    printf("OutOctetsLo     %08i   \n", statsCounter->OutOctetsLo);
    printf("OutFrames       %08i    ", statsCounter->OutFrames);
    printf("Excessive       %08i   \n", statsCounter->Excessive);
    printf("OutMulticasts   %08i    ", statsCounter->OutMulticasts);
    printf("OutBroadcasts   %08i    ", statsCounter->OutBroadcasts);
    printf("InBadMACCtrl    %08i    ", statsCounter->InBadMACCtrl);
    printf("OutPause        %08i   \n", statsCounter->OutPause);
    printf("InPause         %08i    ", statsCounter->InPause);
    printf("InDiscards      %08i   \n", statsCounter->InDiscards);
    printf("Undersize       %08i    ", statsCounter->Undersize);
    printf("Fragments       %08i   \n", statsCounter->Fragments);
    printf("Oversize        %08i    ", statsCounter->Oversize);
    printf("Jabber          %08i   \n", statsCounter->Jabber);
    printf("MACRcvErr       %08i    ", statsCounter->MACRcvErr);
    printf("InFCSErr        %08i   \n", statsCounter->InFCSErr);
    printf("Collisions      %08i    ", statsCounter->Collisions);
    printf("Late            %08i   \n", statsCounter->Late);
}

void __displayCounter3(GT_STATS_COUNTER_SET3 *statsCounter)
{
    printf("InGoodOctetsLo  %08i    ", statsCounter->InGoodOctetsLo);
    printf("InGoodOctetsHi  %08i   \n", statsCounter->InGoodOctetsHi);
    printf("InBadOctets     %08i    ", statsCounter->InBadOctets);
    printf("OutFCSErr       %08i   \n", statsCounter->OutFCSErr);
    printf("InUnicasts      %08i    ", statsCounter->InUnicasts);
    printf("Deferred        %08i   \n", statsCounter->Deferred);
    printf("InBroadcasts    %08i    ", statsCounter->InBroadcasts);
    printf("InMulticasts    %08i   \n", statsCounter->InMulticasts);
    printf("64Octets        %08i    ", statsCounter->Octets64);
    printf("127Octets       %08i   \n", statsCounter->Octets127);
    printf("255Octets       %08i    ", statsCounter->Octets255);
    printf("511Octets       %08i   \n", statsCounter->Octets511);
    printf("1023Octets      %08i    ", statsCounter->Octets1023);
    printf("MaxOctets       %08i   \n", statsCounter->OctetsMax);
    printf("OutOctetsLo     %08i    ", statsCounter->OutOctetsLo);
    printf("OutOctetsHi     %08i   \n", statsCounter->OutOctetsHi);
    printf("OutUnicasts     %08i    ", statsCounter->OutUnicasts);
    printf("Excessive       %08i   \n", statsCounter->Excessive);
    printf("OutMulticasts   %08i    ", statsCounter->OutMulticasts);
    printf("OutBroadcasts   %08i   \n", statsCounter->OutBroadcasts);
    printf("Single          %08i    ", statsCounter->Single);
    printf("OutPause        %08i   \n", statsCounter->OutPause);
    printf("InPause         %08i    ", statsCounter->InPause);
    printf("Multiple        %08i   \n", statsCounter->Multiple);
    printf("Undersize       %08i    ", statsCounter->Undersize);
    printf("Fragments       %08i   \n", statsCounter->Fragments);
    printf("Oversize        %08i    ", statsCounter->Oversize);
    printf("Jabber          %08i   \n", statsCounter->Jabber);
    printf("InMACRcvErr     %08i    ", statsCounter->InMACRcvErr);
    printf("InFCSErr        %08i   \n", statsCounter->InFCSErr);
    printf("Collisions      %08i    ", statsCounter->Collisions);
    printf("Late            %08i   \n", statsCounter->Late);
}

GT_STATUS dsdTester_initMgmtVlan(void)
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

GT_STATUS dsdTester_displayMacEntry(GT_U32 dbNum)
{
	GT_STATUS status;
	GT_ATU_ENTRY tmpMacEntry;
	int iCount = 0;

	memset(&tmpMacEntry,0,sizeof(GT_ATU_ENTRY));
	tmpMacEntry.DBNum = dbNum;

	while(1)
	{
		/* Get the sorted list of MAC Table. */
		if((status = gfdbGetAtuEntryNext(dev,&tmpMacEntry)) != GT_OK)
		{
			return status;
		}
		
		if( 0 == iCount )
			printf("ATU %d List:\n", dbNum);

		printf("(%02x-%02x-%02x-%02x-%02x-%02x) PortVec %#x\n",
			tmpMacEntry.macAddr.arEther[0],
			tmpMacEntry.macAddr.arEther[1],
			tmpMacEntry.macAddr.arEther[2],
			tmpMacEntry.macAddr.arEther[3],
			tmpMacEntry.macAddr.arEther[4],
			tmpMacEntry.macAddr.arEther[5],
			tmpMacEntry.portVec
		);

		iCount++;
	}
	return GT_OK;
}

GT_STATUS dsdTester_displayMacEntryAll(void)
{
	GT_U32          dbNum, maxDbNum;
	GT_ATU_ENTRY    entry;
	//GT_ATU_STAT        atuStat;
	GT_U32 iNum;
	GT_U32         *count = &iNum;

	printf("dsdTester_displayMacEntryAll Called.\n");

	if (IS_IN_DEV_GROUP(dev,DEV_DBNUM_FULL))
		maxDbNum = 16;
	else if(IS_IN_DEV_GROUP(dev,DEV_DBNUM_64))
		maxDbNum = 64;
	else if(IS_IN_DEV_GROUP(dev,DEV_DBNUM_256))
		maxDbNum = 256;
	else if(IS_IN_DEV_GROUP(dev,DEV_DBNUM_4096))
		maxDbNum = 4096;
	else
		maxDbNum = 1;

	for(dbNum=0; dbNum<maxDbNum; dbNum++)
	{
		dsdTester_displayMacEntry(dbNum);
	}

	printf("OK.\n");
	return GT_OK;
}

/*
 *    Add the intellon muticast MAC address (00:b0:52:00:00:01) into the ATU.
 *    Input - None
*/
GT_STATUS sampleAddAtherosMac(void)
{
    GT_STATUS status;
    GT_ATU_ENTRY macEntry;

    macEntry.macAddr.arEther[0] = 0x00;
    macEntry.macAddr.arEther[1] = 0xb0;
    macEntry.macAddr.arEther[2] = 0x52;
    macEntry.macAddr.arEther[3] = 0x00;
    macEntry.macAddr.arEther[4] = 0x00;
    macEntry.macAddr.arEther[5] = 0x01;

    macEntry.portVec = 1 << 6;     /* clt Port number. 7bits are used for portVector. */

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
    macEntry.DBNum = 0;
								
    /* 
     *    Add the MAC Address.
     */
    if((status = gfdbAddMacEntry(dev,&macEntry)) != GT_OK)
    {
        printf("gfdbAddMacEntry returned fail.\n");
        return status;
    }

    return GT_OK;
}

GT_STATUS dsdTester_addAtherosMulticastAddress(void)
{
	//st_dbsNetwork networkinfo;

	return sampleAddAtherosMac();
}


int find_dbnum_by_vid(uint32_t vid)
{
	GT_STATUS status;
	GT_VTU_ENTRY vtuEntry;

	gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
	vtuEntry.vid = 0xfff;
	
	if((status = gvtuGetEntryFirst(dev,&vtuEntry)) != GT_OK)
	{
		/* if can not find vid in the VTU, return DBNum = 0 */
		printf("can not find vid in the vtu\n");
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
			printf("can not find vid in the vtu\n");
			return 0;
		}		
		if( vtuEntry.vid == vid )
		{
			return vtuEntry.DBNum;
		}
	}
}

/* only need to call one time at app starting up */
GT_STATUS dsdTester_addAtherosMulticastAddressToAllCablePort(void)
{
	st_dbsNetwork networkinfo;
	GT_STATUS status;
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

#ifdef CFG_USE_PLATFORM_WR1004JLD
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
		macEntry.DBNum = find_dbnum_by_vid(networkinfo.col_mvlan_id);	
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

GT_STATUS dsdTester_delAtherosMulticastAddress(void)
{
	GT_ETHERADDR mac;

	mac.arEther[0] = 0x00;
	mac.arEther[1] = 0xb0;
	mac.arEther[2] = 0x52;
	mac.arEther[3] = 0x00;
	mac.arEther[4] = 0x00;
	mac.arEther[5] = 0x01;
	
	return gfdbDelMacEntry(dev, &mac);
}

GT_STATUS dsdTester_case_1_1_settings(void)
{
	return GT_OK;
}

GT_STATUS dsdTester_case_1_2_settings(void)
{
	GT_STATUS status;

	/* 1) Set P1 EgressMode = 1 (untag mode) */
	if((status = gprtSetEgressMode(dev,1,GT_UNTAGGED_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	
	/* 2) Set P2 EgressMode = 1 (untag mode) */
	if((status = gprtSetEgressMode(dev,2,GT_UNTAGGED_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	return status;
}

GT_STATUS dsdTester_case_1_3_settings(void)
{
	GT_STATUS status;	

	/* 1) Set P1 EgressMode = 2 (tag mode) */
	if((status = gprtSetEgressMode(dev,1,GT_TAGGED_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	
	/* 2) Set P2 EgressMode = 2 (tag mode) */
	if((status = gprtSetEgressMode(dev,2,GT_TAGGED_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	return status;
}

GT_STATUS dsdTester_case_2_1_settings(void)
{
	GT_STATUS status;
	
	/* 1) port 1 and port 2 both are 802.1q fallback mode */
	if((status = gvlnSetPortVlanDot1qMode(dev, 1, GT_FALLBACK)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}
	if((status = gvlnSetPortVlanDot1qMode(dev, 2, GT_FALLBACK)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}

	/* 2) Set P1 and P2's EgressMode = 0 (unmodify mode) */
	if((status = gprtSetEgressMode(dev,1,GT_UNMODIFY_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	if((status = gprtSetEgressMode(dev,2,GT_UNMODIFY_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	
	return status;
}

GT_STATUS dsdTester_case_2_2_settings(void)
{
	GT_STATUS status;
	
	/* 1) port 1 and port 2 both are 802.1q fallback mode */
	if((status = gvlnSetPortVlanDot1qMode(dev, 1, GT_FALLBACK)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}
	if((status = gvlnSetPortVlanDot1qMode(dev, 2, GT_FALLBACK)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}

	/* 2) Set P1 and P2's EgressMode = 1 (untag mode) */
	if((status = gprtSetEgressMode(dev,1,GT_UNTAGGED_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	if((status = gprtSetEgressMode(dev,2,GT_UNTAGGED_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	
	return status;
}

GT_STATUS dsdTester_case_2_3_settings(void)
{
	GT_STATUS status;
	
	/* 1) port 1 and port 2 both are 802.1q fallback mode */
	if((status = gvlnSetPortVlanDot1qMode(dev, 1, GT_FALLBACK)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}
	if((status = gvlnSetPortVlanDot1qMode(dev, 2, GT_FALLBACK)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}

	/* 2) Set P1 and P2's EgressMode = 2 (tag mode) */
	if((status = gprtSetEgressMode(dev,1,GT_TAGGED_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	if((status = gprtSetEgressMode(dev,2,GT_TAGGED_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	
	return status;
}

GT_STATUS dsdTester_case_3_1_settings(void)
{
	int i = 0;
	GT_STATUS status;
	GT_VTU_ENTRY vtuEntry;
	
	/* port 1 is 802.1q disable mode */
	if((status = gvlnSetPortVlanDot1qMode(dev, 1, GT_DISABLE)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}
	/* port 2 is 802.1q secure mode */
	if((status = gvlnSetPortVlanDot1qMode(dev, 2, GT_SECURE)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}
	/* set port 1's PVID(DefaultVID)=1 */
	if((status = gvlnSetPortVid(dev, 1, 1)) != GT_OK)
	{
		printf("gvlnSetPortVid return Failed\n");
		return status;
	}
	/* set port 2's PVID(DefaultVID)=1000 */
	if((status = gvlnSetPortVid(dev, 2, 1000)) != GT_OK)
	{
		printf("gvlnSetPortVid return Failed\n");
		return status;
	}
	/* set port 1 and port 2 are in the same vlan 1 by port based VLANTable bits */
	/* skip */
	/* set port 1's EgressMode=0(egress Unmodified) */
	if((status = gprtSetEgressMode(dev,1,GT_UNMODIFY_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	/* set port 2's EgressMode=1(egress Untagged) */
	if((status = gprtSetEgressMode(dev,2,GT_UNTAGGED_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	/* flush all VTU entries(vlan 1 is not in the VTU) */
	if((status = gvtuFlush(dev)) != GT_OK)
	{
		printf("gvtuFlush returned fail.\n");
		return status;
	}
	/* add VTU vlan 1000, MemberTagP1=0(egress unmodified), MemberTagP2=1(egress untagged) */
	gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
	vtuEntry.DBNum = 0;
	vtuEntry.vid = 1000;
	for( i=0; i<dev->numOfPorts; i++ )
	{
		if( 2 == i )
		{
			vtuEntry.vtuData.memberTagP[i] = MEMBER_EGRESS_UNTAGGED;
		}
		else
		{
			vtuEntry.vtuData.memberTagP[i] = MEMBER_EGRESS_UNMODIFIED;
		}
	}
	if((status = gvtuAddEntry(dev,&vtuEntry)) != GT_OK)
	{
		printf("gvtuAddEntry returned fail.\n");
		return status;
	}
	return status;
}

GT_STATUS dsdTester_case_3_2_settings(void)
{
	int i = 0;
	GT_STATUS status;
	GT_VTU_ENTRY vtuEntry;
	
	/* port 1 is 802.1q disable mode */
	if((status = gvlnSetPortVlanDot1qMode(dev, 1, GT_DISABLE)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}
	/* port 2 is 802.1q secure mode */
	if((status = gvlnSetPortVlanDot1qMode(dev, 2, GT_SECURE)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}
	/* set port 1's PVID(DefaultVID)=1 */
	if((status = gvlnSetPortVid(dev, 1, 1)) != GT_OK)
	{
		printf("gvlnSetPortVid return Failed\n");
		return status;
	}
	/* set port 2's PVID(DefaultVID)=1000 */
	if((status = gvlnSetPortVid(dev, 2, 1000)) != GT_OK)
	{
		printf("gvlnSetPortVid return Failed\n");
		return status;
	}
	/* set port 1 and port 2 are in the same vlan 1 by port based VLANTable bits */
	/* skip */
	/* set port 1's EgressMode=0(egress Unmodified) */
	if((status = gprtSetEgressMode(dev,1,GT_UNMODIFY_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	/* set port 2's EgressMode=1(egress Untagged) */
	if((status = gprtSetEgressMode(dev,2,GT_UNTAGGED_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	/* flush all VTU entries(vlan 1 is not in the VTU) */
	if((status = gvtuFlush(dev)) != GT_OK)
	{
		printf("gvtuFlush returned fail.\n");
		return status;
	}
	/* add VTU vlan 1000, MemberTagP1=3(egress tagged), MemberTagP2=1(egress untagged) */
	gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
	vtuEntry.DBNum = 0;
	vtuEntry.vid = 1000;
	for( i=0; i<dev->numOfPorts; i++ )
	{
		if( 2 == i )
		{
			vtuEntry.vtuData.memberTagP[i] = MEMBER_EGRESS_UNTAGGED;
		}
		else
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

GT_STATUS dsdTester_case_3_3_settings(void)
{
	int i = 0;
	GT_STATUS status;
	GT_VTU_ENTRY vtuEntry;
	
	/* port 1 is 802.1q disable mode */
	if((status = gvlnSetPortVlanDot1qMode(dev, 1, GT_DISABLE)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}
	/* port 2 is 802.1q secure mode */
	if((status = gvlnSetPortVlanDot1qMode(dev, 2, GT_SECURE)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}
	/* set port 1's PVID(DefaultVID)=1000 */
	if((status = gvlnSetPortVid(dev, 1, 1000)) != GT_OK)
	{
		printf("gvlnSetPortVid return Failed\n");
		return status;
	}
	/* set port 2's PVID(DefaultVID)=1000 */
	if((status = gvlnSetPortVid(dev, 2, 1000)) != GT_OK)
	{
		printf("gvlnSetPortVid return Failed\n");
		return status;
	}
	/* set port 1 and port 2 are in the same vlan 1 by port based VLANTable bits */
	/* skip */
	/* set port 1's EgressMode=0(egress Unmodified) */
	if((status = gprtSetEgressMode(dev,1,GT_UNMODIFY_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	/* set port 2's EgressMode=1(egress Untagged) */
	if((status = gprtSetEgressMode(dev,2,GT_UNTAGGED_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	/* flush all VTU entries(vlan 1 is not in the VTU) */
	if((status = gvtuFlush(dev)) != GT_OK)
	{
		printf("gvtuFlush returned fail.\n");
		return status;
	}
	/* add VTU vlan 1000, MemberTagP1=0(egress unmodified), MemberTagP2=1(egress untagged) */
	gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
	vtuEntry.DBNum = 0;
	vtuEntry.vid = 1000;
	for( i=0; i<dev->numOfPorts; i++ )
	{
		if( 2 == i )
		{
			vtuEntry.vtuData.memberTagP[i] = MEMBER_EGRESS_UNTAGGED;
		}
		else
		{
			vtuEntry.vtuData.memberTagP[i] = MEMBER_EGRESS_UNMODIFIED;
		}
	}
	if((status = gvtuAddEntry(dev,&vtuEntry)) != GT_OK)
	{
		printf("gvtuAddEntry returned fail.\n");
		return status;
	}
	return status;
}

GT_STATUS dsdTester_case_3_4_settings(void)
{
	int i = 0;
	GT_STATUS status;
	GT_VTU_ENTRY vtuEntry;
	
	/* port 1 is 802.1q disable mode */
	if((status = gvlnSetPortVlanDot1qMode(dev, 1, GT_DISABLE)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}
	/* port 2 is 802.1q secure mode */
	if((status = gvlnSetPortVlanDot1qMode(dev, 2, GT_SECURE)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}
	/* set port 1's PVID(DefaultVID)=1000 */
	if((status = gvlnSetPortVid(dev, 1, 1000)) != GT_OK)
	{
		printf("gvlnSetPortVid return Failed\n");
		return status;
	}
	/* set port 2's PVID(DefaultVID)=1000 */
	if((status = gvlnSetPortVid(dev, 2, 1000)) != GT_OK)
	{
		printf("gvlnSetPortVid return Failed\n");
		return status;
	}
	/* set port 1 and port 2 are in the same vlan 1 by port based VLANTable bits */
	/* skip */
	/* set port 1's EgressMode=0(egress Unmodified) */
	if((status = gprtSetEgressMode(dev,1,GT_UNMODIFY_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	/* set port 2's EgressMode=1(egress Untagged) */
	if((status = gprtSetEgressMode(dev,2,GT_UNTAGGED_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	/* flush all VTU entries(vlan 1 is not in the VTU) */
	if((status = gvtuFlush(dev)) != GT_OK)
	{
		printf("gvtuFlush returned fail.\n");
		return status;
	}
	/* add VTU vlan 1000, MemberTagP1=3(egress tagged), MemberTagP2=1(egress untagged) */
	gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
	vtuEntry.DBNum = 0;
	vtuEntry.vid = 1000;
	for( i=0; i<dev->numOfPorts; i++ )
	{
		if( 2 == i )
		{
			vtuEntry.vtuData.memberTagP[i] = MEMBER_EGRESS_UNTAGGED;
		}
		else
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

GT_STATUS dsdTester_case_4_1_settings(void)
{
	int i = 0;
	GT_STATUS status;
	GT_VTU_ENTRY vtuEntry;
	
	/* 1) Set P1 to 802.1q fallback mode; */
	if((status = gvlnSetPortVlanDot1qMode(dev, 1, GT_FALLBACK)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}
	/* 2) Set P1's DefaultVID (PVID) = 1 */
	if((status = gvlnSetPortVid(dev, 1, 1)) != GT_OK)
	{
		printf("gvlnSetPortVid return Failed\n");
		return status;
	}
	/* 3) Set P1's EgressMode = 0 (egress Unmodified) */
	if((status = gprtSetEgressMode(dev,1,GT_UNMODIFY_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	/* 4) Set P2 to 802.1q secure mode */
	if((status = gvlnSetPortVlanDot1qMode(dev, 2, GT_SECURE)) != GT_OK)
	{
		printf("gvlnSetPortVlanDot1qMode return Failed\n");
		return status;
	}
	/* 5) Set P2's DefaultVID (PVID) = 1000 */
	if((status = gvlnSetPortVid(dev, 2, 1000)) != GT_OK)
	{
		printf("gvlnSetPortVid return Failed\n");
		return status;
	}
	/* 6) Set P2's EgressMode = 1 (egress Untagged) */
	if((status = gprtSetEgressMode(dev,2,GT_UNTAGGED_EGRESS)) != GT_OK)
	{
		printf("gprtSetEgressMode returned fail.\n");		
	}
	/* 7) Set P1 and P2 are in the same port based VLANTable*/	
	/* 8) flush all VTU entries(vlan 1 is not in the VTU) */
	/* Note: 7 and 8 already done in MV88E6171R_INIT() */
	/* 9) Add VTU vlan 1000, MemberTagP1=2(egress tagged), MemberTagP2=1(egress untagged) */
	gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
	vtuEntry.DBNum = 0;
	vtuEntry.vid = 1000;
	for( i=0; i<dev->numOfPorts; i++ )
	{
		if( 2 == i )	/* MemberTagP2=1(egress untagged) */
		{
			vtuEntry.vtuData.memberTagP[i] = MEMBER_EGRESS_UNTAGGED;
		}
		else			/* MemberTagP1=2(egress tagged) */
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

GT_STATUS dsdTester_case_5_1_settings()
{
	GT_STATUS status;
	GT_LPORT port;
	GT_STATS_COUNTER_SET    statsCounterSet;

	for(port=0; port<dev->numOfPorts; port++)
	{
		printf("Port %i :\n",port);
		if((status = gstatsGetPortAllCounters(dev,port,&statsCounterSet)) != GT_OK)
		{
			printf("gstatsGetPortAllCounters returned faild.\n");
			return status;
		}
		__displayCounter(&statsCounterSet);
	}
	return GT_OK;
}

GT_STATUS dsdTester_case_5_2_settings()
{
	GT_STATUS status;
	GT_LPORT port;
	GT_STATS_COUNTER_SET2 statsCounterSet;
	
	for(port=0; port<dev->numOfPorts; port++)
	{
		printf("Port %i :\n",port);
		if((status = gstatsGetPortAllCounters2(dev,port,&statsCounterSet)) != GT_OK)
		{
			printf("gstatsGetPortAllCounters2 returned failed.\n");
			return status;
		}
		__displayCounter2(&statsCounterSet);
	}
	return GT_OK;
}

GT_STATUS dsdTester_case_5_3_settings()
{
	GT_STATUS status;
	GT_LPORT port;
	GT_STATS_COUNTER_SET3 statsCounterSet;
	
	for(port=0; port<dev->numOfPorts; port++)
	{
		printf("Port %i :\n",port);
		if((status = gstatsGetPortAllCounters3(dev,port,&statsCounterSet)) != GT_OK)
		{
			printf("gstatsGetPortAllCounters3 returned failed.\n");
			return status;
		}
		__displayCounter3(&statsCounterSet);
	}
	return GT_OK;
}

GT_STATUS dsdTester_showVIDTable()
{
	GT_STATUS status;
	GT_VTU_ENTRY vtuEntry;
	GT_LPORT port;    
	int portIndex;

	gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
	vtuEntry.vid = 0xfff;
	if((status = gvtuGetEntryFirst(dev,&vtuEntry)) != GT_OK)
	{
		printf("gvtuGetEntryCount returned fail.\n");
		return status;
	}

	printf("DBNum:%i, VID:%i \n",vtuEntry.DBNum,vtuEntry.vid);

	for(portIndex=0; portIndex<dev->numOfPorts; portIndex++)
	{
		port = portIndex;
		printf("Tag%i:%#x  ",port,vtuEntry.vtuData.memberTagP[port]);
	}

	printf("\n");

	while(1)
	{
		if((status = gvtuGetEntryNext(dev,&vtuEntry)) != GT_OK)
		{
			break;
		}

		printf("DBNum:%i, VID:%i \n",vtuEntry.DBNum,vtuEntry.vid);

		for(portIndex=0; portIndex<dev->numOfPorts; portIndex++)
		{
			port = portIndex;
			printf("Tag%i:%#x  ",port,vtuEntry.vtuData.memberTagP[port]);
		}

		printf("\n");
	}
	return GT_OK;
}


void dsdTester_showVtu(void)
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

GT_STATUS dsdTester_showVlan(void)
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


/*
 *    This setup function shows how to configure Ingress Rate of 128Kbps with the
 *    custom data information.
 *  it configures the resource 0 of Port 0 of Marvell SOHO Switch Device with 
 *    capability of PIRL to be :
 *    
 *    1) Custom setup for Ingress Rate : Enabled
 *    2) Custom EBS Limit : 0xFFFFFF
 *    3) Custom CBS Limit : 0x200000
 *    4) Custom Bucket Increament  : 0x3D
 *    5) Custom Bucket Rate Factor : 2
 *    6) Discarded frame : Do not account discarded frame due to queue congestion
 *    7) Filtered frame  : Account filtered frame
 *    8) Mgmt frame      : Exclude management frame from rate limiting calculation
 *    9) SA found in ATU : Exclude from ingress rate limiting calculation if the SA of the
 *                         frame is in ATU with EntryState that indicates Non Rate Limited.
 *    10) DA found in ATU : Include to ingress rate limiting calculation even though the DA of the
 *                         frame is in ATU with EntryState that indicates Non Rate Limited.
 *    11) Sampling Mode   : Disable the mode
 *    12) Action Mode     : Follow Limit action when there are not enough tokens to accept the
 *                         entire imcoming frame.
 *    13) Limit action   : Drop packets when the incoming rate exceeds the limit
 *    14) Rate type      : Rate is based on Traffic type
 *    15) Traffic type   : ARP, MGMT, Multicast, Broadcast, and Unicast frames are 
 *                           tracked as part of the rate resource calculation.
 *    16) Byte counted   : Account only Layer 3 bytes (IP header and payload)
 *
*/

GT_STATUS dsdTester_PIRL2CustomSetup(int cbsEnable, long cbsLimit, int cbsIncreament)
{
    GT_STATUS status;
    GT_PIRL2_DATA pirlData;
    GT_U32        irlRes;
    GT_LPORT     port;

    port = 0;
    irlRes = 0;

    //  demo  default cbsLimit 0x200000 
   //  demo  default cbsIncreament 0x3D 

    //add by stan 
    if( 1 == cbsEnable)
      pirlData.customSetup.isValid = GT_TRUE;
    else if( 0 == cbsEnable)
      pirlData.customSetup.isValid = GT_FALSE;
    else
	return GT_BAD_PARAM; 

    pirlData.customSetup.ebsLimit = 0xFFFFFF;
//    pirlData.customSetup.cbsLimit = 0x200000;
 //   pirlData.customSetup.bktIncrement = 0x3D;
    pirlData.customSetup.cbsLimit = cbsLimit;
    pirlData.customSetup.bktIncrement = cbsIncreament;
    pirlData.customSetup.bktRateFactor = 2;

    pirlData.accountQConf         = GT_FALSE;
    pirlData.accountFiltered    = GT_TRUE;

    pirlData.mgmtNrlEn = GT_TRUE;
    pirlData.saNrlEn   = GT_TRUE;
    pirlData.daNrlEn   = GT_FALSE;
    pirlData.samplingMode = GT_FALSE;
    pirlData.actionMode = PIRL_ACTION_USE_LIMIT_ACTION;

    pirlData.ebsLimitAction        = ESB_LIMIT_ACTION_DROP;
    pirlData.bktRateType        = BUCKET_TYPE_TRAFFIC_BASED;
    pirlData.bktTypeMask        = BUCKET_TRAFFIC_BROADCAST |
                                  BUCKET_TRAFFIC_MULTICAST |
                                  BUCKET_TRAFFIC_UNICAST   |
                                  BUCKET_TRAFFIC_MGMT_FRAME|
                                  BUCKET_TRAFFIC_ARP;

    pirlData.priORpt = GT_TRUE;
    pirlData.priMask = 0;

    pirlData.byteTobeCounted    = GT_PIRL2_COUNT_ALL_LAYER3;

    status = gpirl2WriteResource(dev,port,irlRes,&pirlData);

    switch (status)
    {
        case GT_OK:
            //MSG_PRINT(("PIRL2 writing completed.\n"));
            printf("PIRL2 writing completed.\n");
            break;
        case GT_BAD_PARAM:
            //MSG_PRINT(("Invalid parameters are given.\n"));
            printf("Invalid parameters are given.\n");
            break;
        case GT_NOT_SUPPORTED:
            //MSG_PRINT(("Device is not supporting PIRL2.\n"));
            printf("Device is not supporting PIRL2.\n");
            break;
        default:
            //MSG_PRINT(("Failure to configure device.\n"));
            printf("Failure to configure device.\n");
            break;
    }

    return status;
}

int main(int argc, char *argv[])
{
	int cpuPort = 5;
	long cbsLimit =0x000000;
	int cbsIncreament = 0x00;	
	int cbsEnable= 0;
	uint32_t vid;
	st_dbsSysinfo dsdtSysinfo;

	if( argc > 2 )
	{
		if( strcmp(argv[1], "case/7/1") != 0)
		{
			dsdTester_usage();
			return 0;	
		}
	}
	else if( argc != 2)
	{
		dsdTester_usage();
		return 0;	
	}

	dbsdev = dbsNoWaitOpen(MID_DSDT_TESTER);
	if( NULL == dbsdev )
	{
		return 0;
	}

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

	/* ×¢²áÒì³£ÍË³ö¾ä±úº¯Êý*/
	signal(SIGTERM, dsdTester_signalProcessHandle);
	signal(SIGINT, dsdTester_signalProcessHandle);

	dbs_sys_log(dbsdev, DBS_LOG_INFO, "starting module dsdTester success");
	printf("Starting module dsdTester	......		[OK]\n\n");
	printf("\n==================================================================\n\n");

	if( strcmp(argv[1], "case/0/1") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_initMgmtVlan() )
		{
			goto DSDT_END;
		}
		
		printf("\n");
		dsdTester_showVtu();

		printf("\n");
		dsdTester_showVlan();

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/1/1") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_case_1_1_settings() )
		{
			goto DSDT_END;
		}
		
		printf("\n");
		dsdTester_showVtu();

		printf("\n");
		dsdTester_showVlan();

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/1/2") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_case_1_2_settings() )
		{
			goto DSDT_END;
		}
		
		printf("\n");
		dsdTester_showVtu();

		printf("\n");
		dsdTester_showVlan();

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/1/3") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_case_1_3_settings() )
		{
			goto DSDT_END;
		}
		
		printf("\n");
		dsdTester_showVtu();

		printf("\n");
		dsdTester_showVlan();

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/2/1") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_case_2_1_settings() )
		{
			goto DSDT_END;
		}
		
		printf("\n");
		dsdTester_showVtu();

		printf("\n");
		dsdTester_showVlan();

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/2/2") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_case_2_2_settings() )
		{
			goto DSDT_END;
		}
		
		printf("\n");
		dsdTester_showVtu();

		printf("\n");
		dsdTester_showVlan();

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/2/3") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_case_2_3_settings() )
		{
			goto DSDT_END;
		}
		
		printf("\n");
		dsdTester_showVtu();

		printf("\n");
		dsdTester_showVlan();

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/3/1") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_case_3_1_settings() )
		{
			goto DSDT_END;
		}
		
		printf("\n");
		dsdTester_showVtu();

		printf("\n");
		dsdTester_showVlan();

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/3/2") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_case_3_2_settings() )
		{
			goto DSDT_END;
		}
		
		printf("\n");
		dsdTester_showVtu();

		printf("\n");
		dsdTester_showVlan();

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/3/3") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_case_3_3_settings() )
		{
			goto DSDT_END;
		}
		
		printf("\n");
		dsdTester_showVtu();

		printf("\n");
		dsdTester_showVlan();

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/3/4") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_case_3_4_settings() )
		{
			goto DSDT_END;
		}
		
		printf("\n");
		dsdTester_showVtu();

		printf("\n");
		dsdTester_showVlan();

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/4/1") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_case_4_1_settings() )
		{
			goto DSDT_END;
		}
		
		printf("\n");
		dsdTester_showVtu();

		printf("\n");
		dsdTester_showVlan();

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/5/1") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_case_5_1_settings() )
		{
			goto DSDT_END;
		}		

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/5/2") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_case_5_2_settings() )
		{
			goto DSDT_END;
		}		

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/5/3") == 0)
	{
		printf("\n");
		dsdtStart(cpuPort);

		if( GT_OK != dsdTester_case_5_3_settings() )
		{
			goto DSDT_END;
		}		

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/6/1") == 0)
	{
		printf("\n");
		dsdtInit(cpuPort);

		if( GT_OK != dsdTester_showVIDTable() )
		{
			goto DSDT_END;
		}		

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/6/2") == 0)
	{
		printf("\n");
		dsdtInit(cpuPort);

		if( GT_OK != dsdTester_displayMacEntryAll() )
		{
			goto DSDT_END;
		}		

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/6/3") == 0)
	{
		printf("\n");
		dsdtInit(cpuPort);

		if( GT_OK != dsdTester_addAtherosMulticastAddress() )
		{
			goto DSDT_END;
		}		

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/6/4") == 0)
	{
		printf("\n");
		dsdtInit(cpuPort);

		if( GT_OK != dsdTester_delAtherosMulticastAddress() )
		{
			goto DSDT_END;
		}		

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/6/5") == 0)
	{
		printf("\n");
		dsdtInit(cpuPort);

		if( GT_OK != dsdTester_addAtherosMulticastAddressToAllCablePort() )
		{
			goto DSDT_END;
		}		

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/7/1") == 0)
	{
		printf("\n");
		dsdtInit(cpuPort);
                //add by stan for param cbslimit argv[3] and increament argv[4]
                if(argc >= 3 ) {
                  cbsEnable = atoi(argv[2]);
                  // storm prevent enable
	 	  if( 1 == cbsEnable && 5 == argc) {
		    sscanf(argv[3], "%x", &cbsLimit );
		    sscanf(argv[4], "%x", &cbsIncreament);
                    cbsEnable = 1;
		  }else if( 0 == cbsEnable){
                    // storm prevent disable
		    cbsLimit = 0x200000;
                    cbsIncreament = 0x3D;
		  }else {
                    
		        printf(" storm prevent param error not 1 or 0 , pls reinput and try bye!\n");
			goto DSDT_END;
		  }
                }

		if( GT_OK != dsdTester_PIRL2CustomSetup(cbsEnable, cbsLimit, cbsIncreament) )
		{
			goto DSDT_END;
		}		

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/8/1") == 0)
	{
		printf("\n");
		dsdtInit(cpuPort);

		if( GT_OK != gsysSwReset(dev) )
		{
			printf("Failed.\n");
			goto DSDT_END;
		}
		else
		{
			printf("OK.\n");
		}

		printf("\n");
		goto DSDT_END;
	}
	else if( strcmp(argv[1], "case/9/1") == 0)
	{
		printf("\n");
		dsdtInit(cpuPort);
		
		printf("\n");
		dsdTester_showVlan();

		printf("\n");
		goto DSDT_END;
	}
	else
	{
		dsdTester_usage();
		goto DSDT_END;
	}
	
DSDT_END:
	
	printf("\n\n==================================================================\n\n");
	dbs_sys_log(dbsdev, DBS_LOG_INFO, "module dsdTester exit");
	dbsClose(dbsdev);
	
	return 0;
	
}

