#include <assert.h>
#include "mme_rtl8306e.h"
#include "mmead.h"

static T_MME_SK_HANDLE RTL8306E_MME_SK;
static uint8_t RTL8306E_ODA[6];

RTL_REGISTER_DESIGN rtl8306e_register_table[] = 
{
	/*flag*/		/*phy*/		/*register*/		/*page*/	/*value*/
	{1,		    	 0,			16,				0,			0x07FA },
	{1,		    	 0,			18,				0,			0x7FFF },
	{1,		    	 0,			19,				0,			0xFFFF },	
	{1,		    	 0,			24,				0,			0x0EDF },
	{1,		    	 1,			24,				0,			0x0EC0 },
	{1,		    	 2,			24,				0,			0x0EC0 },
	{1,		    	 3,			24,				0,			0x0EC0 },
	{1,		    	 4,			24,				0,			0x0EC0 },
	{1,		    	 0,			25,				0,			0xE001 },
	{1,		    	 1,			25,				0,			0xE000 },
	{1,		    	 2,			25,				0,			0xE000 },
	{1,		    	 3,			25,				0,			0xE000 },
	{1,		    	 4,			25,				0,			0xE000 },
	{1,		    	 6,			25,				0,			0xD000 },
	{1,		    	 0,			22,				0,			0x877F },
	{1,		    	 1,			22,				0,			0x877F },
	{1,		    	 2,			22,				0,			0x877F },
	{1,		    	 3,			22,				0,			0x877F },
	{1,		    	 4,			22,				0,			0x877F },
	{1,		    	 0,			31,				0,			0x0000 },
	{1,		    	 3,			16,				0,			0x5452 },
	{1,		    	 3,			17,				0,			0x834C },
	{1,		    	 3,			18,				0,			0xC005 },	
	{1,		    	 4,			23,				0,			0x0000 },
	{1,		    	 0,			0,				0,			0x3100 },
	{1,		    	 1,			0,				0,			0x3100 },
	{1,		    	 2,			0,				0,			0x3100 },
	{1,		    	 3,			0,				0,			0x3100 },
	{1,		    	 4,			0,				0,			0x3100 },
	
	{1,		    	 6,			17,				1,			0x0000 },
	{1,		    	 1,			30,				1,			0x0000 },
	{1,		    	 1,			31,				1,			0x0000 },
	{1,		    	 2,			30,				1,			0x0000 },
	{1,		    	 2,			31,				1,			0x0000 },
	{1,		    	 3,			30,				1,			0x0000 },
	{1,		    	 3,			31,				1,			0x0000 },
	
	{1,		    	 0,			21,				2,			0x07FF },
	{1,		    	 1,			21,				2,			0x07FF },
	{1,		    	 2,			21,				2,			0x07FF },
	{1,		    	 3,			21,				2,			0x07FF },
	{1,		    	 4,			21,				2,			0x07FF },
	{1,		    	 0,			18,				2,			0x07FF },
	{1,		    	 1,			18,				2,			0x07FF },
	{1,		    	 2,			18,				2,			0x07FF },
	{1,		    	 3,			18,				2,			0x07FF },
	{1,		    	 4,			18,				2,			0x07FF },

	{1,		    	 0,			21,				3,			0x94CA },
	{1,		    	 2,			23,				3,			0x83B1 },
	{1,		    	 0,			26,				3,			0x0000 },
	{1,		    	 0,			27,				3,			0x0000 },
	
	/*********************** The End****************************/
	{0,		    	 0,			0,				0,			0x0000 }	
};

void rtl8306e_set_mme_sk(T_MME_SK_HANDLE *MME_SK)
{
	memcpy((void *)&RTL8306E_MME_SK, (void *)MME_SK, sizeof(T_MME_SK_HANDLE));
}

T_MME_SK_HANDLE * rtl8306e_get_mme_sk(void)
{
	return &RTL8306E_MME_SK;
}

void rtl8306e_set_mme_oda(uint8_t ODA[])
{
	memcpy((void *)&RTL8306E_ODA, (void *)ODA, 6);
}

uint8_t *rtl8306e_get_mme_oda(void)
{
	return RTL8306E_ODA;
}

uint32_t rtl8306e_getPhyReg(const uint16_t phyad, const uint16_t regad, const uint16_t npage,  uint16_t *value)
{
	T_szMdioRtl8306e rtl8306e_mdio_info;
	RTL_REGISTER_DESIGN *register_table = rtl8306e_register_table;

	if ((phyad >= 7) || (regad >= 32) || (npage >= 4))
	{
		printf("\nERROR: rtl8306e get phy %d reg %d page %d\n", phyad, regad, npage);
		return CMM_FAILED;
	}

	while(register_table->flag)
	{
		if((phyad == register_table->phy) && (regad == register_table->reg) && (npage == register_table->page))
		{
			*value = register_table->value;			
			return CMM_SUCCESS;
		}
		else
		{
			register_table++;
			continue;
		}		
	}
	
	//read from cnu if can not find
	//printf("\nINFO: rtl8306e_getPhyReg smiRead(%d, %d, %d)\n", phyad, regad, npage);
	rtl8306e_mdio_info.phy = phyad;
	rtl8306e_mdio_info.reg = regad;
	rtl8306e_mdio_info.page = npage;
	if( CMM_SUCCESS != rtl8306e_read(rtl8306e_get_mme_sk(), rtl8306e_get_mme_oda(), &rtl8306e_mdio_info) )
	{
		printf("\nERROR: rtl8306e get phy %d reg %d page %d\n", phyad, regad, npage);
		return CMM_FAILED;
	}
	else
	{
		*value = rtl8306e_mdio_info.value;	
		return CMM_SUCCESS;
	}
}


uint32_t rtl8306e_setPhyReg(const uint16_t phyad, const uint16_t regad, const uint16_t npage,  const uint16_t value)
{
	RTL_REGISTER_DESIGN *register_table = rtl8306e_register_table;

	if ((phyad >= 7) || (regad >= 32) || (npage >= 4))
	{
		printf("\nERROR: rtl8306e set phy %d reg %d page %d\n", phyad, regad, npage);
		return CMM_FAILED;
	}

	while(register_table->flag)
	{
		if((phyad == register_table->phy) && (regad == register_table->reg) && (npage == register_table->page))
		{
			register_table->value = value;			
			return CMM_SUCCESS;
		}
		else
		{
			register_table++;
			continue;
		}		
	}
	printf("\nERROR: rtl8306e set phy %d reg %d page %d\n", phyad, regad, npage);
	return CMM_FAILED;
}

uint32_t rtl8306e_getPhyRegBit(const uint16_t phyad, const uint16_t regad, const uint16_t bit, const uint16_t npage,  uint16_t *pvalue)
{
	uint16_t rdata;

	if ((phyad >= 7) || (regad >= 32) ||(npage >= 4) || (bit > 15) || (pvalue == NULL))
	{
		printf("\nERROR: rtl8306e get phy %d reg %d page %d bit %d\n", phyad, regad, npage, bit);
		return CMM_FAILED;
	}
	if( CMM_SUCCESS != rtl8306e_getPhyReg(phyad, regad, npage, &rdata))
	{
		return CMM_FAILED;
	}
	if (rdata & (1 << bit))
	{
		*pvalue = 1;
	}		
	else 
	{
		*pvalue = 0;
	}
	return CMM_SUCCESS;
}

uint32_t rtl8306e_setPhyRegBit(const uint16_t phyad, const uint16_t regad, const uint16_t bit, const uint16_t npage,  const uint16_t value)
{
	uint16_t rdata;
	
	if ((phyad >= 7) || (regad >= 32) || (npage >= 4) || (bit > 15) || (value >1))
	{
		printf("\nERROR: rtl8306e set phy %d reg %d page %d bit %d\n", phyad, regad, npage, bit);
		return CMM_FAILED;
	}
	if( CMM_SUCCESS != rtl8306e_getPhyReg(phyad, regad, npage, &rdata))
	{
		return CMM_FAILED;
	}	
	
	if (value)
	{
		if( CMM_SUCCESS != rtl8306e_setPhyReg(phyad, regad, npage, rdata | (1 << bit)))
		{
			return CMM_FAILED;
		}
	}
	else
	{
		if( CMM_SUCCESS != rtl8306e_setPhyReg(phyad, regad, npage, rdata & (~(1 << bit))))
		{
			return CMM_FAILED;
		}
	}
	return CMM_SUCCESS;
}

uint32_t rtl8306e_getAsicVlan(uint16_t vlanIndex, uint16_t *vid)
{
	uint16_t tmp;
	
	if( vid == NULL )
	{
		return CMM_FAILED;
	}
		
	switch(vlanIndex)
	{
		case 0: /*VLAN[A]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(0, 25, 0, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}			
		case 1: /*VLAN[B]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(1, 25, 0, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 2: /*VLAN[C]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(2, 25, 0, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 3: /*VLAN[D]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(3, 25, 0, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 4: /*VLAN[E]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(4, 25, 0, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 5: /*VLAN[F]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(0, 27, 1, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 6: /*VLAN[G]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(1, 27, 1, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 7: /*VLAN[H]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(2, 27, 1, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 8: /*VLAN[I]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(3, 27, 1, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 9: /*VLAN[J]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(4, 27, 1, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 10: /*VLAN[K]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(0, 29, 1, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 11: /*VLAN[L]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(1, 29, 1, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 12: /*VLAN[M]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(2, 29, 1, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
 		case 13: /*VLAN[N]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(3, 29, 1, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
 		case 14: /*VLAN[O]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(4, 29, 1, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
		case 15: /*VLAN[P]*/
		{
			if( CMM_SUCCESS != rtl8306e_getPhyReg(0, 31, 1, &tmp) )
			{
				return CMM_FAILED;
			}
			break;
		}
		default:
			return CMM_FAILED;
	}	
	*vid = (tmp) & 0xFFF;
	return	CMM_SUCCESS;
		
}	

uint32_t rtl8306e_getPortVlanMode(uint16_t port, uint16_t *mode)
{
	uint16_t regValue;
	
	if (port > 5 || (mode == NULL))
	{
		return CMM_FAILED;
	}
		
	/*Port 5 corresponding PHY6*/	
	if (port == 5 )
	{
		port ++ ;
	}

	if( CMM_SUCCESS != rtl8306e_getPhyReg(port, 22, 0, &regValue) )
	{
		return CMM_FAILED;
	}
	*mode = regValue & 0x3;
	
       /*
	//When enable inserting CPU tag, VLAN tag could not be added 
	rtl8306_getAsicPhyReg(2, 21, 3, &regValue);
	if ((*option == RTL8306_VLAN_ITAG) && ((regValue & 0x9000) == 0x1000))
		*option = RTL8306_VLAN_UNDOTAG; 	
	*/
	return CMM_SUCCESS;
}

uint32_t rtl8306e_getVlanPortAcceptFrameType(uint16_t port, uint16_t *pAccept_frame_type)
{
	uint16_t regval;
    
	if((port > 5) || (pAccept_frame_type == NULL))
	{
		return CMM_FAILED;
	}
        

	if(0 == port)
	{
		if( CMM_SUCCESS != rtl8306e_getPhyReg(0, 31, 0, &regval) )
		{
			return CMM_FAILED;
		}		
		*pAccept_frame_type = (regval >> 7) & 0x3;
	}
	else if (5 == port)
	{
		if( CMM_SUCCESS != rtl8306e_getPhyReg(6, 30, 1, &regval) )
		{
			return CMM_FAILED;
		}		
		*pAccept_frame_type = (regval >> 6) & 0x3;
	}
    	else
	{
		if( CMM_SUCCESS != rtl8306e_getPhyReg(port, 21, 2, &regval) )
		{
			return CMM_FAILED;
		}		
		*pAccept_frame_type = (regval >> 11) & 0x3;
	}

	return CMM_SUCCESS;
}

uint32_t rtl8306e_getAsicPortVlanIndex(uint16_t port, uint16_t *vlanIndex)
{
	uint16_t tmp;
	
	if((port > 5) || vlanIndex == NULL)
	{
		return CMM_FAILED;
	}
		
	if (port < 5)
	{
		if( CMM_SUCCESS != rtl8306e_getPhyReg(port, 24, 0, &tmp) )
		{
			return CMM_FAILED;
		}
	}		
	else
	{
		if( CMM_SUCCESS != rtl8306e_getPhyReg(0, 26, 1, &tmp) )
		{
			return CMM_FAILED;
		}
	}
	
	*vlanIndex = (tmp>>12) & 0xF;
	
	return CMM_SUCCESS;
}

uint32_t rtl8306e_getPvid(uint16_t port, uint16_t *vid)
{
	uint16_t index;

	/*check port number*/
	if (port > 5)
		return CMM_FAILED;

	/*check vid*/
	if (vid == NULL)
		return CMM_FAILED;
		
	/*get the pvid*/
	if( CMM_SUCCESS != rtl8306e_getAsicPortVlanIndex(port, &index) )
	{
		return CMM_FAILED;
	}
	if( CMM_SUCCESS != rtl8306e_getAsicVlan(index, vid) )
	{
		return CMM_FAILED;
	}

	return CMM_SUCCESS;
}

uint32_t rtl8306e_get_vlan_config(st_cnuSwitchVlanConfig *vlanInfo)
{
	int i = 0;
	uint16_t rdata;
	uint32_t ret = CMM_SUCCESS;
	
	//get vlan_enable [phy 0 reg 18 page 0][bit:8], /* 1 = Disable VLAN; 0 = Enable VLAN */
	if( CMM_SUCCESS != rtl8306e_getPhyRegBit(0, 18, 8, 0, &rdata) )
	{
		return CMM_FAILED;
	}
	else
	{
		vlanInfo->vlan_enable = (rdata == 0)?1:0;
	}
	//get 802.1q vlan status [phy 0 reg 16 page 0][bit:10]
	if( CMM_SUCCESS != rtl8306e_getPhyRegBit(0, 16, 10, 0, &rdata) )
	{
		return CMM_FAILED;
	}
	else
	{
		vlanInfo->vlan_tag_aware = (rdata == 0)?1:0;
	}
	//get ingress_filter [phy 0 reg 16 page 0][bit:9]
	if( CMM_SUCCESS != rtl8306e_getPhyRegBit(0, 16, 9, 0, &rdata) )
	{
		return CMM_FAILED;
	}
	else
	{
		vlanInfo->ingress_filter = (rdata == 0)?1:0;
	}
	//get g_admit_control [phy 0 reg 16 page 0][bit:8]
	if( CMM_SUCCESS != rtl8306e_getPhyRegBit(0, 16, 8, 0, &rdata) )
	{
		return CMM_FAILED;
	}
	else
	{
		vlanInfo->g_admit_control = (rdata == 0)?1:0;
	}

	/* get vlan info for each port */
	for( i=0; i<5; i++ )
	{
		/* pvid */
		if( CMM_SUCCESS != rtl8306e_getPvid(i, &rdata) )
		{
			return CMM_FAILED;
		}
		vlanInfo->vlan_port[i].pvid = rdata;
		/* port egress tag mode */
		/* 11 = Do not insert or remove VLAN tags to/from packet which is output on this port*/
		/* 10 = The switch will add VLAN tags to packets, if they are not tagged*/
		/* 01 = The switch will remove VLAN tags from packets, if they are tagged*/
		/* 00 = The switch will remove VLAN tags from packets then add new tags to them*/
		if( CMM_SUCCESS != rtl8306e_getPortVlanMode(i, &rdata) )
		{
			return CMM_FAILED;
		}
		vlanInfo->vlan_port[i].egress_mode = rdata;
		/* port admit frame type */
		if( CMM_SUCCESS != rtl8306e_getVlanPortAcceptFrameType(i, &rdata) )
		{
			return CMM_FAILED;
		}
		vlanInfo->vlan_port[i].admit_control = rdata;
	}
	return CMM_SUCCESS;
}

uint32_t rtl8306e_get_bandwidth_control_config(st_cnuSwitchBandwidthConfig *bandwidthInfo)
{
	int i = 0;
	uint16_t tmp;
	uint16_t tmp2 = 0;

	if( CMM_SUCCESS != rtl8306e_getPhyRegBit(0, 21, 15, 3, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		bandwidthInfo->g_rx_bandwidth_control_enable = (tmp == 0)?1:0;
	}

	for(i=0;i<=4;i++)
	{
		if( CMM_SUCCESS != rtl8306e_getPhyReg(i, 21, 2, &tmp) )
		{
			return CMM_FAILED;
		}
		else
		{
			bandwidthInfo->rxPort[i].bandwidth_value = (tmp & 0x7ff);
			bandwidthInfo->rxPort[i].bandwidth_control_enable = bandwidthInfo->g_rx_bandwidth_control_enable;
		}		
	}

	for(i=0;i<=4;i++)
	{
		if( CMM_SUCCESS != rtl8306e_getPhyReg(i, 18, 2, &tmp) )
		{
			return CMM_FAILED;
		}
		else
		{
			tmp2 = (tmp >> 15) & 0x1;
			bandwidthInfo->txPort[i].bandwidth_control_enable = (tmp2 == 0)?0:1;			
			bandwidthInfo->txPort[i].bandwidth_value = (tmp) & 0x7ff;
			bandwidthInfo->g_tx_bandwidth_control_enable |= bandwidthInfo->txPort[i].bandwidth_control_enable;
		}		
	}

	return CMM_SUCCESS;
}

uint32_t rtl8306e_get_loop_detect_config(st_cnuSwitchLoopDetect *loopDetect)
{
	int i = 0;
	uint16_t tmp;
	
	//get loop detect status [phy 0 reg 16 page 0][bit:2]
	if( CMM_SUCCESS != rtl8306e_getPhyRegBit(0, 16, 2, 0, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		/* 0 = Disable loop detect; 1 = Enable loop detect */
		loopDetect->status = (tmp == 0)?0:1;
	}
	
	/* get port loop status */
	for( i=0; i<=4; i++ )
	{
		//get port x loop status [phy 0~4 reg 24 page 0][bit:8]
		if( CMM_SUCCESS != rtl8306e_getPhyRegBit(i, 24, 8, 0, &tmp) )
		{
			return CMM_FAILED;
		}
		else
		{
			/* 0 = No loop exists on port x; 1 = A loop has been detected on port x */
			loopDetect->port_loop_status[i] = (tmp == 0)?0:1;
		}
	}
	
	if( CMM_SUCCESS != rtl8306e_getPhyReg(4, 23, 0, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		/*LDMETHOD*/
		loopDetect->ldmethod = (tmp>>15)&0x1;
		/*LDTIMER*/
		loopDetect->ldtime = (tmp>>13)&0x3;
		/*LDBCK_FRQ*/
		loopDetect->ldbckfrq = (tmp>>12)&0x1;
		/*LDSCLR*/
		loopDetect->ldsclr = (tmp>>11)&0x1;
		/*PABUZZER*/
		loopDetect->pabuzzer = (tmp>>10)&0x1;
		/*EN_TAGLF*/
		loopDetect->entaglf = (tmp>>9)&0x1;
	}
	if( CMM_SUCCESS != rtl8306e_getPhyReg(6, 17, 1, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		/*LPTTL_INIT[3:0]*/
		loopDetect->lpttlinit = (tmp>>8)&0xf;
		/*LPF_PRI[1:0]*/
		loopDetect->lpfpri = (tmp>>6)&0x3;
		/*EN_LPF_PRI*/
		loopDetect->enlpfpri = (tmp>>5)&0x1;
		/*DISFLTLF*/
		loopDetect->disfltlf = (tmp>>4)&0x1;
		/*EN_LPTTL*/
		loopDetect->enlpttl = (tmp>>3)&0x1;
	}
	if( CMM_SUCCESS != rtl8306e_getPhyReg(3, 16, 0, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		loopDetect->sid[1] = (tmp>>8)&0xff;
		loopDetect->sid[0] = tmp&0xff;
	}
	if( CMM_SUCCESS != rtl8306e_getPhyReg(3, 17, 0, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		loopDetect->sid[3] = (tmp>>8)&0xff;
		loopDetect->sid[2] = tmp&0xff;
	}
	if( CMM_SUCCESS != rtl8306e_getPhyReg(3, 18, 0, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		loopDetect->sid[5] = (tmp>>8)&0xff;
		loopDetect->sid[4] = tmp&0xff;
	}
	
	return CMM_SUCCESS;
}

uint32_t rtl8306e_get_storm_filter_config(st_cnuSwitchStormFilter *stormFilter)
{
	int i = 0;
	uint16_t tmp;

	if( CMM_SUCCESS != rtl8306e_getPhyRegBit(0, 18, 2, 0, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		stormFilter->disable_broadcast = (tmp == 0)?0:1;
	}

	if( CMM_SUCCESS != rtl8306e_getPhyRegBit(2, 23, 9, 3, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		stormFilter->disable_multicast = (tmp == 0)?0:1;
	}

	if( CMM_SUCCESS != rtl8306e_getPhyRegBit(2, 23, 8, 3, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		stormFilter->disable_unknown = (tmp == 0)?0:1;
	}

	if( CMM_SUCCESS != rtl8306e_getPhyRegBit(6, 17, 1, 1, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		stormFilter->rule = (tmp == 0)?0:1;
	}

	if( CMM_SUCCESS != rtl8306e_getPhyRegBit(6, 25, 7, 0, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		stormFilter->reset_source = (tmp == 0)?0:1;
	}

	if( CMM_SUCCESS != rtl8306e_getPhyReg(6, 25, 0, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		stormFilter->iteration = (tmp>>5)&0x3;
	}

	//3-bit thresholt
	if( CMM_SUCCESS != rtl8306e_getPhyReg(6, 25, 0, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		stormFilter->thresholt = (tmp>>8)&0x3;
	}
	if( CMM_SUCCESS != rtl8306e_getPhyRegBit(6, 17, 0, 1, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		stormFilter->thresholt |= (((tmp == 0)?0:1)<<2);
	}

	return CMM_SUCCESS;
}

uint32_t rtl8306e_get_port_link_status(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], st_rtl8306e_port_status *linkstatus)
{
	uint16_t tmp;
	uint8_t i;
	uint8_t link[4];

	assert( NULL != MME_SK);
	assert( NULL != linkstatus);

	rtl8306e_set_mme_sk(MME_SK);
	rtl8306e_set_mme_oda(ODA);

	for( i = 0; i < 4; i++ )
	{
		if( CMM_SUCCESS != rtl8306e_getPhyRegBit(i, 1, 2, 0, &tmp) )
		{
			return CMM_FAILED;
		}
		else 
		{
			link[i] = (tmp?1:0);
		}
	}
	linkstatus ->port1linkstatus = link[0];
	linkstatus ->port2linkstatus = link[1];
	linkstatus ->port3linkstatus = link[2];
	linkstatus ->port4linkstatus = link[3];
	return CMM_SUCCESS;
}

uint32_t rtl8306e_get_mac_limit_config(st_cnuSwitchMacLimit *macLimit)
{
	int i = 0;
	uint16_t tmp;

	if( CMM_SUCCESS != rtl8306e_getPhyRegBit(0, 31, 9, 0, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		macLimit->action = (tmp?1:0);
	}

	if( CMM_SUCCESS != rtl8306e_getPhyRegBit(0, 26, 11, 3, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		macLimit->system.enable = (tmp?1:0);
	}

	if( CMM_SUCCESS != rtl8306e_getPhyReg(0, 26, 3, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		macLimit->system.counter = (tmp&0x7ff);
	}

	if( CMM_SUCCESS != rtl8306e_getPhyReg(0, 31, 0, &tmp) )
	{
		return CMM_FAILED;
	}
	else
	{
		macLimit->system.thresholt = (tmp&0x7f);
		macLimit->system.mport = (tmp>>10)&0x3f;
	}

	for(i=0;i<4;i++)
	{
		if (0 == i)
		{
			if( CMM_SUCCESS != rtl8306e_getPhyRegBit(0, 26, 12, 3, &tmp) )
			{
				return CMM_FAILED;
			}
			else
			{
				macLimit->port[i].enable = (tmp?1:0);
			}
			if( CMM_SUCCESS != rtl8306e_getPhyReg(0, 27, 3, &tmp) )
			{
				return CMM_FAILED;
			}
			else
			{
				macLimit->port[i].counter = (tmp&0x7ff);
				macLimit->port[i].thresholt = (tmp>>11)&0x1f;
			}
		}
		else
		{
			if( CMM_SUCCESS != rtl8306e_getPhyRegBit(i, 30, 15, 1, &tmp) )
			{
				return CMM_FAILED;
			}
			else
			{
				macLimit->port[i].enable = (tmp?1:0);
			}
			if( CMM_SUCCESS != rtl8306e_getPhyReg(i, 31, 1, &tmp) )
			{
				return CMM_FAILED;
			}
			else
			{
				macLimit->port[i].counter = (tmp&0x7ff);
				macLimit->port[i].thresholt = (tmp>>11)&0x1f;
			}
		}
	}

	return CMM_SUCCESS;
}

uint32_t rtl8306e_get_port_control_config(st_cnuSwitchPortControl *portControl)
{
	int i = 0;
	uint16_t tmp;

	for(i=0;i<5;i++)
	{
		if( CMM_SUCCESS != rtl8306e_getPhyRegBit(i, 0, 11, 0, &tmp) )
		{
			return CMM_FAILED;
		}
		else
		{
			portControl->port[i].enable = (tmp?0:1);
		}
	}

	return CMM_SUCCESS;
}

uint32_t mme_rtl8306e_change_page(uint8_t page)
{
	uint32_t ret = CMM_SUCCESS;
	T_szMdioPhy v;

	v.phy = 0;
	v.reg = 16;
	v.value = 0;

	ret += smiRead(rtl8306e_get_mme_sk(), rtl8306e_get_mme_oda(), &v);
	if(ret)
	{
		return CMM_FAILED;
	}
	
	switch (page)
	{
		case 0:
		{
			v.value = ((v.value & 0x7FFF) | 0x0002);			
			break;
		}
		case 1:
		{
			v.value = (v.value | 0x8002);
			break;
		}
		case 2:
		{
			v.value = (v.value & 0x7FFD);
			break;
		}
		case 3:
		{
			v.value = ((v.value & 0xFFFD) | 0x8000);
			break;
		}
		default:
		{
			return CMM_FAILED;
		}
	}
	ret += smiWrite(rtl8306e_get_mme_sk(), rtl8306e_get_mme_oda(), &v);
	return ret;
}

uint32_t mme_rtl8306e_register_table_init(void)
{
	uint32_t ret = CMM_SUCCESS;
	T_szMdioPhy v;
	uint8_t cur_page = 0xff;
	RTL_REGISTER_DESIGN *register_table = rtl8306e_register_table;	
	
	while(register_table->flag)
	{
		if( cur_page != register_table->page )
		{
			ret += mme_rtl8306e_change_page(register_table->page);
			if(ret)
			{
				return CMM_FAILED;
			}
			cur_page = register_table->page;			
		}
		v.phy = register_table->phy;
		v.reg = register_table->reg;
		v.value = 0;
		ret += smiRead(rtl8306e_get_mme_sk(), rtl8306e_get_mme_oda(), &v);
		if(ret)
		{
			return CMM_FAILED;
		}
		register_table->value = v.value;
		register_table++;
	}
	return ret;
} 

uint32_t mme_rtl8306e_get_config_all(T_MME_SK_HANDLE *MME_SK, uint8_t ODA[], st_rtl8306eSettings *rtl8306e)
{
	assert( NULL != MME_SK);
	assert( NULL != rtl8306e);

	rtl8306e_set_mme_sk(MME_SK);
	rtl8306e_set_mme_oda(ODA);
	
	/**************** read all register value to rtl8306e_register_table first *****************/
	if( CMM_SUCCESS != mme_rtl8306e_register_table_init())
	{
		return CMM_FAILED;
	}
	
	/**************** get all configs from rtl8306e_register_table *************************/
	//get vlan configs from rtl8306e_register_table
	if( CMM_SUCCESS != rtl8306e_get_vlan_config(&(rtl8306e->vlanConfig)))
	{
		return CMM_FAILED;
	}
	//get bandwidth control configs from rtl8306e_register_table
	if( CMM_SUCCESS != rtl8306e_get_bandwidth_control_config(&(rtl8306e->bandwidthConfig)))
	{
		return CMM_FAILED;
	}
	//get port loop detect configs and status from rtl8306e_register_table
	if( CMM_SUCCESS != rtl8306e_get_loop_detect_config(&(rtl8306e->loopDetect)))
	{
		return CMM_FAILED;
	}
	//get storm filter config from rtl8306e_register_table
	if( CMM_SUCCESS != rtl8306e_get_storm_filter_config(&(rtl8306e->stormFilter)))
	{
		return CMM_FAILED;
	}
	//get mac limit config from rtl8306e_register_table
	if( CMM_SUCCESS != rtl8306e_get_mac_limit_config(&(rtl8306e->macLimit)))
	{
		return CMM_FAILED;
	}
	//get port control config from rtl8306e_register_table
	if( CMM_SUCCESS != rtl8306e_get_port_control_config(&(rtl8306e->portControl)))
	{
		return CMM_FAILED;
	}

	return CMM_SUCCESS;
}

