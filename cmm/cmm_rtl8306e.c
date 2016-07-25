#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "cmm_rtl8306e.h"

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
	{1,		    	 2,			21,				3,			0x8000 },
	{1,		    	 3,			21,				3,			0x0000 },
	{1,		    	 3,			22,				3,			0xA000 },
	{1,		    	 0,			26,				3,			0x0000 },
	{1,		    	 0,			27,				3,			0x0000 },
	
	/*********************** The End****************************/
	{0,		    	 0,			0,				0,			0x0000 }	
};

#ifndef __BYTE_ORDER
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
# define intohl(x)	(x)
# define intohs(x)	(x)
# define ihtonl(x)	(x)
# define ihtons(x)	(x)
#else
# if __BYTE_ORDER == __BIG_ENDIAN
#  define intohl(x)	__bswap_32 (x)
#  define intohs(x)	__bswap_16 (x)
#  define ihtonl(x)	__bswap_32 (x)
#  define ihtons(x)	__bswap_16 (x)
# endif
#endif

uint32_t __smiWriteCmd(uint8_t phy, uint8_t reg, uint16_t value, uint16_t mask, uint8_t *buf)
{
	uint16_t command[3] = {0x0000, 0x0000, 0x0000};
	command[0] = ihtons(((0x8005 | (phy<<4)) | (reg<<9)));	
	command[1] = ihtons(value);
	command[2] = ihtons(mask);	
	memcpy(buf, command, 6);
	return 6;
}

uint32_t __smiWriteMod(uint16_t phy, uint16_t reg, uint16_t value, uint32_t offset, uint8_t *buf)
{
	//uint16_t reg_word_addr;	
	uint16_t last_cmd_num = 0;
	uint16_t new_cmd_num = 0;
	uint16_t svalue = 0;
	uint32_t len = 0;

	if( 0 == offset )
	{
		new_cmd_num = 1;
		svalue = ihtons((uint16_t)(1|(new_cmd_num<<6)));
		memcpy((buf+offset), &svalue, sizeof(uint16_t));
		offset += sizeof(uint16_t);
		len += sizeof(uint16_t);
	}
	else
	{
		last_cmd_num = ihtons(*(uint16_t *)buf)>>6;
		new_cmd_num = 1;
		svalue = ihtons((uint16_t)(1|((last_cmd_num+new_cmd_num)<<6)));
		memcpy(buf, &svalue, sizeof(uint16_t));
	}

	offset += __smiWriteCmd(phy, reg, value, 0xffff, (buf+offset));
	len += 3*sizeof(uint16_t);	

	//printf("\n__smiWriteMod len = %d\n", len);	
	return len;
}

#if 0
uint16_t __uint16t_set_bit(const uint16_t src, uint16_t bit, uint16_t bitValue)
{
	uint16_t ret = src;
	
	if( bit > 15 )
	{
		return ret;
	}
	
	if( 0 == bitValue )
	{
		ret &= (~(1<<bit));
		return ret;
	}
	else if( 1 == bitValue )
	{
		ret |= (1<<bit);
		return ret;
	}
	else
	{
		return ret;
	}
}
#endif

uint32_t rtl8306e_getPhyReg(const uint16_t phyad, const uint16_t regad, const uint16_t npage,  uint16_t *value)
{
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
	printf("\nERROR: rtl8306e get phy %d reg %d page %d\n", phyad, regad, npage);
	return CMM_FAILED;
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

uint32_t rtl8306e_set_sid(uint8_t sid[])
{	
	uint32_t ret = 0;
	uint16_t *p = (uint16_t *)sid;

	ret += rtl8306e_setPhyReg(3, 16, 0, p[0]);
	ret += rtl8306e_setPhyReg(3, 17, 0, p[1]);
	ret += rtl8306e_setPhyReg(3, 18, 0, p[2]);
	
	return ret;
}

uint32_t rtl8306e_loop_detet_enable(uint16_t enabled)
{	
	uint32_t ret = 0;
	ret += rtl8306e_setPhyRegBit(0, 16, 2, 0, (enabled==0)?0:1);	
	return ret;
}

uint32_t rtl8306e_set_loop_detet_ldmethod(uint16_t v)
{	
	uint32_t ret = 0;
	ret += rtl8306e_setPhyRegBit(4, 23, 15, 0, (v==0)?0:1);	
	return ret;
}

uint32_t rtl8306e_set_loop_detet_ldtime(uint16_t v)
{	
	uint32_t ret = 0;
	uint16_t regValue = 0;
	
	ret += rtl8306e_getPhyReg(4, 23, 0, &regValue);
	regValue = (regValue & 0x9FFF) | ((v&0x3)<<13);
	ret += rtl8306e_setPhyReg(4, 23, 0, regValue);
	return ret;
}

uint32_t rtl8306e_filter_loop_detet_frame(uint16_t enabled)
{	
	uint32_t ret = 0;
	ret += rtl8306e_setPhyRegBit(6, 17, 4, 1, (enabled==0)?0:1);	
	return ret;
}

uint32_t rtl8306e_loop_detet_ttl_enable(uint16_t enabled)
{	
	uint32_t ret = 0;
	ret += rtl8306e_setPhyRegBit(6, 17, 3, 1, (enabled==0)?0:1);	
	return ret;
}

uint32_t rtl8306e_vlan_enable(uint16_t enabled)
{	
	uint32_t ret = 0;
	
	ret += rtl8306e_setPhyRegBit(0, 18, 8, 0, enabled==0?1:0);
	/*disable trunk*/
	ret += rtl8306e_setPhyRegBit(0, 19, 11, 0, 1);
	return ret;
}

uint32_t rtl8306e_tag_aware(uint16_t enabled)
{
	return rtl8306e_setPhyRegBit(0, 16, 10, 0, enabled==0?1:0 );
}

uint32_t rtl8306e_ingress_filter(uint16_t enabled)
{
	return rtl8306e_setPhyRegBit(0, 16, 9, 0, enabled==0?1:0);
}

uint32_t rtl8306e_admit_control(uint16_t enabled)
{
	return rtl8306e_setPhyRegBit(0, 16, 8, 0, enabled==0?1:0 );
}

uint32_t rtl8306e_clearVtu(void)
{
	int i = 0;
	uint16_t rdata;
	uint32_t ret = CMM_SUCCESS;

	for(i=0;i<=4;i++)
	{
		//clear vid
		ret += rtl8306e_getPhyReg(i, 25, 0, &rdata);
		ret += rtl8306e_setPhyReg(i, 25, 0, (rdata & 0xF000));
		//clear vlan port member
		ret += rtl8306e_getPhyReg(i, 24, 0, &rdata);
		ret += rtl8306e_setPhyReg(i, 24, 0, (rdata & 0xFFC0));		
	}	
	return ret;
}

uint32_t rtl8306e_getVlan(uint16_t vlanIndex, uint16_t *vid, uint16_t *memberPortMask)
{
	uint16_t rdata;
	uint32_t ret = CMM_SUCCESS;

	if( vlanIndex > 4 )
	{
		return CMM_FAILED;
	}
	if(vid == NULL || memberPortMask == NULL)
	{
		return CMM_FAILED;
	}		

	ret += rtl8306e_getPhyReg(vlanIndex, 25, 0, &rdata);
	*vid = rdata & 0xFFF;

	ret += rtl8306e_getPhyReg(vlanIndex, 24, 0, &rdata);
	*memberPortMask = rdata & 0x3F;
	
	return ret;
}	

uint32_t rtl8306e_setVlan(uint16_t vlanIndex, uint16_t vid, uint16_t memberPortMask)
{
	uint16_t rdata;
	uint32_t ret = CMM_SUCCESS;

	if( vlanIndex > 4 )
	{
		return CMM_FAILED;
	}

	ret += rtl8306e_getPhyReg(vlanIndex, 25, 0, &rdata);
	ret += rtl8306e_setPhyReg(vlanIndex, 25, 0, (rdata & 0xF000)|(vid & 0xFFF));

	ret += rtl8306e_getPhyReg(vlanIndex, 24, 0, &rdata);
	ret += rtl8306e_setPhyReg(vlanIndex, 24, 0, (rdata & 0xFFC0)|(memberPortMask & 0x3F));

	return ret;
}	

uint32_t rtl8306e_addVlan(uint16_t vid)
{
	uint32_t ret = 0;
	uint16_t vidData;
	uint16_t memData;
	uint16_t index;
	uint16_t fullflag;
	int i;
	//RTL_REGISTER_DESIGN *register_table = rtl8306e_register_table;
	
	/*check vid*/
	if ( vid < 1 || vid > 4094 )
		return CMM_FAILED;

	/*check if vid exists and check if vlan is full*/
	fullflag = 1;
	index = 4;
	for( i=4; i>=0; i--)
	{
		ret += rtl8306e_getVlan(i, &vidData, &memData);
		if (vidData == 0)	/*has empty entry*/
		{
			index = i;	/*set available Vlan Entry index*/
			fullflag = 0;
			continue;
		}
		if (vidData == vid)
		{
			return CMM_SUCCESS;
		}			
	}
	if (fullflag == 1)
		return CMM_FAILED;

	/*check over and add Vlan*/
	ret += rtl8306e_setVlan(index, vid, 0);
	return ret;
	
}

uint32_t rtl8306e_setPortVlanIndex(uint16_t port, uint16_t vlanIndex)
{
	uint32_t ret = 0;
	uint16_t regValue;

	if(port > 4)
		return CMM_FAILED;
	
	ret += rtl8306e_getPhyReg(port, 24, 0, &regValue);
	regValue = (regValue & 0xFFF) | (vlanIndex<<12);
	ret += rtl8306e_setPhyReg(port, 24, 0, regValue);	
       
	return ret;
}

uint32_t rtl8306e_setPvid(uint16_t port, uint16_t vid)
{
	uint32_t ret = 0;
	uint16_t vidData,memData;
	uint16_t index;
	int i;
	
	/*check vid*/
	if ( vid > 4094 || vid < 1)
		return CMM_FAILED;
	
	/*check port number*/
	if (port > 4)
		return CMM_FAILED;

	/*For init only*/
	if ( vid == 0)
		ret += rtl8306e_setPortVlanIndex(port, 0);

	/*search the vid*/
	index = 5;	/*init a invalid value*/
	for(i=0; i<=4; i++)
	{
		ret += rtl8306e_getVlan(i, &vidData, &memData);
		if ( vidData == vid)
		{
			index = i;
			break;
		}			
	}
	
	if (index == 5)	/*vid not exists*/
		return CMM_FAILED;

	/*set pvid*/
	ret += rtl8306e_setPortVlanIndex(port,index);    
	
	return ret;
}

uint32_t rtl8306e_addVlanPortMember(uint16_t vid, uint16_t port)
{
	uint16_t vidData,memData;
	uint16_t index;
	int i;
	uint32_t ret = 0;

	/*check vid*/
	if ( vid < 1 || vid > 4094 )
		return CMM_FAILED;
	/*check port number*/
	if (port > 4)
		return CMM_FAILED;

	/*search the vid*/
	index = 5;	/*init a invalid value*/
	for(i = 0; i  <= 4; i++)
	{
		ret += rtl8306e_getVlan(i, &vidData, &memData);
		if ( vidData == vid)
		{
			index = i;
			break;
		}			
	}
	
	if (index == 5)	/*vid not exists*/
		return CMM_FAILED;

	/*add the port number to the Vlan*/
	memData |= 1 << port;
	ret += rtl8306e_setVlan(index, vid, memData);
    
	return ret;
}

uint32_t rtl8306e_setPortEgressMode(uint16_t port, uint16_t option)
{
	uint32_t ret = 0;
	uint16_t regValue;
	uint16_t speed, duplex, nway;
     
	if ( (port > 4) || (option > 0x3) ) 
	{
		return CMM_FAILED;
	}		

#if 0      
	/*save mac 4 or port status when operate reg.22*/
	if (port == 4)
	{
		rtl8306e_getPhyRegBit(5, 0, 13, 0, &speed);
		rtl8306e_getPhyRegBit(5, 0, 12, 0, &nway);
		rtl8306e_getPhyRegBit(5, 0, 8, 0, &duplex);            
	}
#endif	
	ret += rtl8306e_getPhyReg(port, 22, 0, &regValue);
	ret += rtl8306e_setPhyReg(port, 22, 0, (regValue & 0xFFFC) | option);
#if 0 	
       /*restore mac 4 or port status when operate reg.22*/
	if (port == 4)
	{
		rtl8306e_setPhyRegBit(5, 0, 13, 0, speed);
		rtl8306e_setPhyRegBit(5, 0, 12, 0, nway);
		rtl8306e_setPhyRegBit(5, 0, 8, 0, duplex);
	}
#endif		
	return ret;
}

uint32_t rtl8306e_setPortAcceptFrameType(uint16_t port, uint16_t accept_frame_type)
{
	uint32_t ret = 0;
	uint16_t regval;

	if(port > 4)
	{
		return CMM_FAILED;
	}

	if(0 == port)
	{
		ret += rtl8306e_getPhyReg(0, 31, 0, &regval);
		regval &= ~(0x3 << 7);
		regval |= (accept_frame_type << 7);
		ret += rtl8306e_setPhyReg(0, 31, 0, regval);
	}
	else
	{
		ret += rtl8306e_getPhyReg(port, 21, 2, &regval);
		regval &= ~(0x3 << 11);
		regval |= (accept_frame_type << 11);
		ret += rtl8306e_setPhyReg(port, 21, 2, regval);        
	}

	return ret;
}

uint32_t rtl8306e_setPortBandWidth(uint16_t port, uint16_t n64Kbps, uint16_t direction, uint16_t enabled)
{
	uint32_t ret = 0;
	uint16_t regval;
    
	if ((port > 4) || (n64Kbps > 0x7FF) || (direction > 1))
		return CMM_FAILED;    

	//RTL8306_PORT_RX
	if (0 == direction) 
	{  
		/*configure port Rx rate*/
		if (!enabled ) 
		{
			ret += rtl8306e_setPhyRegBit(0, 21, 15, 3, 1);
		} 
		else 
		{
			ret += rtl8306e_setPhyRegBit(0, 21, 15, 3, 0);
			ret += rtl8306e_getPhyReg(port, 21, 2, &regval);
			regval = (regval & 0xF800) | (n64Kbps & 0x7FF);
			ret += rtl8306e_setPhyReg(port, 21, 2, regval);
	        }
	} 
	else 
	{  
		/*configure port Tx rate*/
		if (!enabled) 
		{
			ret += rtl8306e_setPhyRegBit(port, 18, 15, 2, 0);
		} 
		else 
		{
			ret += rtl8306e_setPhyRegBit(port, 18, 15, 2, 1);
			ret += rtl8306e_getPhyReg(port, 18, 2, &regval);
			regval = (regval & 0xF800) | (n64Kbps & 0x7FF);
			ret += rtl8306e_setPhyReg(port, 18, 2, regval);
		}
	}
	return CMM_SUCCESS;
}

uint32_t rtl8306e_storm_filter_disable(uint8_t frameType, uint16_t disable)
{
	switch(frameType)
	{
		/* disable broadcast storm filter */
		case 0:
		{
			return rtl8306e_setPhyRegBit(0, 18, 2, 0, (disable==1)?1:0 );
		}
		/* disable multicast storm filter */
		case 1:
		{
			return rtl8306e_setPhyRegBit(2, 23, 9, 3, (disable==1)?1:0 );
		}
		/* disable unknown da unicast storm filter */
		case 2:
		{
			return rtl8306e_setPhyRegBit(2, 23, 8, 3, (disable==1)?1:0 );
		}
		default:
		{
			return CMM_FAILED;
		}
	}
	
}

uint32_t rtl8306e_set_storm_filter_type(uint16_t type)
{
	return rtl8306e_setPhyRegBit(6, 17, 1, 1, (type==0)?0:1 );
}

uint32_t rtl8306e_set_storm_filter_reset_source(uint16_t resetSource)
{
	return rtl8306e_setPhyRegBit(6, 25, 7, 0, (resetSource==0)?0:1 );
}

uint32_t rtl8306e_set_storm_filter_iteration(uint16_t iteration)
{
	uint16_t rdata = 0;
	uint32_t ret = CMM_SUCCESS;

	rdata = ((iteration&0x3)<<5);

	ret += rtl8306e_setPhyReg(6, 25, 0, rdata);

	return ret;
}

uint32_t rtl8306e_set_storm_filter_thresholt(uint16_t thresholt)
{
	uint32_t ret = CMM_SUCCESS;

	ret += rtl8306e_setPhyReg(6, 25, 0, ((thresholt&0x3)<<8));
	ret += rtl8306e_setPhyRegBit(6, 17, 0, 1, (((thresholt>>2)&0x1)==0)?0:1 );

	return ret;
}

uint32_t rtl8306e_set_mac_limit_action(uint8_t action)
{
	return rtl8306e_setPhyRegBit(0, 31, 9, 0, action?1:0 );	
}

uint32_t rtl8306e_set_mac_limit_system(st_systemMacLimit *system)
{
	uint32_t ret = CMM_SUCCESS;
	uint16_t rdata = 0;
	
	assert( NULL != system );

	ret += rtl8306e_setPhyRegBit(0, 26, 11, 3, (system->enable)?1:0 );
	
	ret += rtl8306e_getPhyReg(0, 31, 0, &rdata);
	ret += rtl8306e_setPhyReg(0, 31, 0, (rdata & 0x380)|((system->thresholt)&0x7f)|(((system->mport)&0x3f)<<10));
	
	return ret;	
}

uint32_t rtl8306e_set_mac_limit_port(uint8_t port, st_portMacLimit *portMacLimit)
{
	uint32_t ret = CMM_SUCCESS;
	uint16_t rdata = 0;
	
	assert( NULL != portMacLimit );

	if (port > 3)
	{
		return CMM_FAILED;
	}
	if (0 == port)
	{
		ret += rtl8306e_setPhyRegBit(0, 26, 12, 3, (portMacLimit->enable)?1:0);
		ret += rtl8306e_getPhyReg(0, 27, 3, &rdata);
		ret += rtl8306e_setPhyReg(0, 27, 3, (rdata & 0x7ff)|(((portMacLimit->thresholt)&0x1f)<<11));
	}
	else
	{
		ret += rtl8306e_setPhyRegBit(port, 30, 15, 1, (portMacLimit->enable)?1:0);
		ret += rtl8306e_getPhyReg(port, 31, 1, &rdata);
		ret += rtl8306e_setPhyReg(port, 31, 1, (rdata & 0x7ff)|(((portMacLimit->thresholt)&0x1f)<<11));
	}
	
	return ret;	
}

uint32_t rtl8306e_vlan_register_prepare(st_cnuSwitchVlanConfig *vlan)
{
	int i = 0;
	uint32_t ret = CMM_SUCCESS;
	
	assert( NULL != vlan );

	ret += rtl8306e_vlan_enable(vlan->vlan_enable);
	ret += rtl8306e_tag_aware(vlan->vlan_tag_aware);
	ret += rtl8306e_ingress_filter(vlan->ingress_filter);
	ret += rtl8306e_admit_control(vlan->g_admit_control);

	/*clear vlan table*/
	ret += rtl8306e_clearVtu();
	for(i=0;i<=4;i++)
	{
		if( i < 4 )	/* LAN port */
		{
			ret += rtl8306e_addVlan(vlan->vlan_port[i].pvid);
			ret += rtl8306e_setPvid(i, vlan->vlan_port[i].pvid);
			ret += rtl8306e_addVlanPortMember(vlan->vlan_port[i].pvid, i);
			//add cpu port to vlan member port
			ret += rtl8306e_addVlanPortMember(vlan->vlan_port[i].pvid, 4);
			//port egress mode
			ret += rtl8306e_setPortEgressMode(i, vlan->vlan_port[i].egress_mode);
			//port ingress accept frame type
			ret += rtl8306e_setPortAcceptFrameType(i, 0);
		}
		else		/* CPU port */
		{
			ret += rtl8306e_addVlan(vlan->vlan_port[i].pvid);
			ret += rtl8306e_setPvid(i, vlan->vlan_port[i].pvid);
			ret += rtl8306e_addVlanPortMember(vlan->vlan_port[i].pvid, i);
			ret += rtl8306e_setPortEgressMode(i, vlan->vlan_port[i].egress_mode);
			ret += rtl8306e_setPortAcceptFrameType(i, 0);

		}		
	}

	return ret;
}

uint32_t rtl8306e_loop_detection_register_prepare(st_cnuSwitchLoopDetect *loopDetection)
{
	uint32_t ret = CMM_SUCCESS;
	
	assert( NULL != loopDetection );

	ret += rtl8306e_loop_detet_enable(loopDetection->status);
	ret += rtl8306e_set_loop_detet_ldmethod(loopDetection->ldmethod);
	ret += rtl8306e_set_loop_detet_ldtime(loopDetection->ldtime);
	ret += rtl8306e_filter_loop_detet_frame(loopDetection->disfltlf);
	ret += rtl8306e_loop_detet_ttl_enable(loopDetection->enlpttl);
	ret += rtl8306e_set_sid(loopDetection->sid);
	return ret;
}

uint32_t rtl8306e_bandwidth_control_register_prepare(st_cnuSwitchBandwidthConfig *bandwidth)
{
	int i = 0;
	uint32_t ret = CMM_SUCCESS;
	
	assert( NULL != bandwidth );

	for(i=0;i<=4;i++)
	{
		//rx bandwidth control enable is global
		ret += rtl8306e_setPortBandWidth(i, bandwidth->rxPort[i].bandwidth_value, 0, bandwidth->g_rx_bandwidth_control_enable);
		//tx bandwidth control enable is configed by each port
		ret += rtl8306e_setPortBandWidth(i, bandwidth->txPort[i].bandwidth_value, 1, bandwidth->txPort[i].bandwidth_control_enable);
	}

	return ret;
}

uint32_t rtl8306e_storm_filter_register_prepare(st_cnuSwitchStormFilter *stormFilter)
{
	uint32_t ret = CMM_SUCCESS;
	
	assert( NULL != stormFilter );

	ret += rtl8306e_storm_filter_disable(0, stormFilter->disable_broadcast);
	ret += rtl8306e_storm_filter_disable(1, stormFilter->disable_multicast);
	ret += rtl8306e_storm_filter_disable(2, stormFilter->disable_unknown);
	ret += rtl8306e_set_storm_filter_type(stormFilter->rule);
	ret += rtl8306e_set_storm_filter_iteration(stormFilter->iteration);
	ret += rtl8306e_set_storm_filter_thresholt(stormFilter->thresholt);
	ret += rtl8306e_set_storm_filter_reset_source(stormFilter->reset_source);

	return ret;
}

uint32_t rtl8306e_mac_limit_register_prepare(st_cnuSwitchMacLimit *macLimit)
{
	int i = 0;
	uint32_t ret = CMM_SUCCESS;
	
	assert( NULL != macLimit );

	ret += rtl8306e_set_mac_limit_action(macLimit->action);
	ret += rtl8306e_set_mac_limit_system(&(macLimit->system));
	for(i=0;i<4;i++)
	{
		ret += rtl8306e_set_mac_limit_port(i, &(macLimit->port[i]));
	}

	return ret;
}

uint32_t rtl8306e_cpu_port_disable(uint8_t disable)
{
	return rtl8306e_setPhyRegBit(2, 21, 15, 3, disable?1:0 );	
}

uint32_t rtl8306e_cpu_tag_insert_enable(uint8_t enable)
{
	return rtl8306e_setPhyRegBit(2, 21, 12, 3, enable?1:0 );	
}

uint32_t rtl8306e_cpu_tag_remove_enable(uint8_t enable)
{
	return rtl8306e_setPhyRegBit(2, 21, 11, 3, enable?1:0 );	
}

uint32_t rtl8306e_acl_set(stAclEntryInfo *entry)
{
	uint32_t ret = CMM_SUCCESS;
	uint16_t rdata = 0;
	uint16_t value;
	
	assert( NULL != entry );

	if ((entry->entryadd > 15) || (entry->phyport > 0x7) || (entry->action > 3) ||(entry->protocol > 3) ||(entry->priority > 3))
    	{
		return CMM_FAILED;
	}
	
	ret += rtl8306e_setPhyReg(3, 21, 3, entry->data);

	ret += rtl8306e_getPhyReg(3, 22, 3, &rdata);
	value = (1<<14)|(entry->entryadd<<9)|(entry->priority<<7)| (entry->action<<5)|(entry->phyport<<2)|entry->protocol;
	rdata = (rdata & 0x8000) |value;
	ret += rtl8306e_setPhyReg(3, 22, 3, rdata);

	return ret;
}

uint32_t rtl8306e_acl_register_prepare(void)
{
	int i = 0;
	uint32_t ret = CMM_SUCCESS;
	stAclEntryInfo entry;
	
	//acl drop mme by default	
	entry.entryadd = 0;
	entry.phyport = 7;		/*any port*/
	entry.priority = 0;
	entry.protocol = 0;		/*ethertype*/
	entry.action = 0;		/*drop*/
	entry.data = 0x88e1;	/*HomePlug av*/

	ret += rtl8306e_cpu_port_disable(0);
	ret += rtl8306e_cpu_tag_insert_enable(0);
	ret += rtl8306e_cpu_tag_remove_enable(1);
	ret += rtl8306e_acl_set(&entry);

	return ret;
}

uint32_t rtl8306e_set_port_powerdown(uint16_t port, uint16_t powerdown)
{	
	uint32_t ret = 0;

	if( port > 4 )
	{
		return CMM_FAILED;
	}
	
	ret += rtl8306e_setPhyRegBit(port, 0, 11, 0, powerdown?1:0);	
	return ret;
}

uint32_t rtl8306e_port_control_register_prepare(st_cnuSwitchPortControl *portControl)
{
	int i = 0;
	uint32_t ret = CMM_SUCCESS;
	
	assert( NULL != portControl );

	for(i=0;i<5;i++)
	{
		ret += rtl8306e_set_port_powerdown(i, (portControl->port[i].enable)?0:1);
	}

	return ret;
}

uint32_t rtl8306e_register_prepare(st_rtl8306eSettings *rtl8306e)
{
	uint32_t ret = CMM_SUCCESS;
	st_cnuSwitchVlanConfig *vlan = NULL;
	st_cnuSwitchBandwidthConfig *bandwidth = NULL;
	st_cnuSwitchLoopDetect *loopDetection = NULL;
	st_cnuSwitchStormFilter *stormFilter = NULL;
	st_cnuSwitchMacLimit *macLimit = NULL;
	st_cnuSwitchPortControl *portControl = NULL;
	
	assert( NULL != rtl8306e );

	vlan = &(rtl8306e->vlanConfig);
	bandwidth = &(rtl8306e->bandwidthConfig);
	loopDetection = &(rtl8306e->loopDetect);
	stormFilter = &(rtl8306e->stormFilter);
	macLimit = &(rtl8306e->macLimit);
	portControl = &(rtl8306e->portControl);

	ret += rtl8306e_vlan_register_prepare(vlan);
	ret += rtl8306e_bandwidth_control_register_prepare(bandwidth);
	ret += rtl8306e_loop_detection_register_prepare(loopDetection);
	ret += rtl8306e_storm_filter_register_prepare(stormFilter);
	ret += rtl8306e_mac_limit_register_prepare(macLimit);
	ret += rtl8306e_acl_register_prepare();
	ret += rtl8306e_port_control_register_prepare(portControl);
	return ret;
}

void rtl8306e_register_dump(void)
{
	RTL_REGISTER_DESIGN *register_table = rtl8306e_register_table;

	printf("\n\n+++++++++++++++++++++++++++++++++++++++++++++++++\n");
	while(register_table->flag)
	{
		printf("cnu-switch write 0x%04x phy %d register %d page %d\n", register_table->value, 
			register_table->phy, register_table->reg, register_table->page);
		register_table++;
	}
	printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

uint32_t rtl8306e_mod_prepare(uint8_t *mod)
{
	assert( NULL != mod );
	
	uint32_t offset = 0;
	uint8_t cur_page = 0xff;
	uint16_t rdata = 0;
	RTL_REGISTER_DESIGN *register_table = rtl8306e_register_table;

	//*mod = block;
	bzero(mod, 1024);

	while(register_table->flag)
	{
		//change page if current page is not equal to the register page to be writen
		/* note: do not support to write page 4 */
		if( cur_page != register_table->page )
		{
			rtl8306e_getPhyReg(0, 16, 0, &rdata);
			switch (register_table->page)
			{
				case 0:
				{
					offset += __smiWriteMod(0, 16, ((rdata & 0x7FFF) | 0x0002), offset, mod);
					break;
				}
				case 1:
				{
					offset += __smiWriteMod(0, 16, (rdata | 0x8002), offset, mod);
					break;
				}
				case 2:
				{
					offset += __smiWriteMod(0, 16, (rdata & 0x7FFD), offset, mod);
					break;
				}
				case 3:
				{
					offset += __smiWriteMod(0, 16, ((rdata & 0xFFFD) | 0x8000), offset, mod);
					break;
				}
				default:
				{
					return 0;
				}
			}
		}
		offset += __smiWriteMod(register_table->phy, register_table->reg, register_table->value, offset, mod);
		
		if( offset > 1018 )
		{			
			fprintf(stderr, "ERROR: rtl8306e_mod_prepare offset !");
			return 0;
		}
		cur_page = register_table->page;
		register_table++;
	}
	
	if( 0 != ( offset % 4 ) )
	{
		offset += 2;
	}
	return offset;	
}

void rtl8306e_mod_dump(const uint8_t*b, uint32_t len)
{
	int i = 0;
	uint8_t temp = 0;
	
	assert( NULL != b );
	
	/* 打印参数块*/
	printf("\r\n\r\n  ==================================================\n");
	printf("  __dump_atheros_mod len = %d", len);
	printf("\n  ==================================================");
	printf("\r\n  ");
	for( i=0; i<len; i++ )
	{		
		temp = *(b+i);
		printf("%02X ", temp);
		if( (i+1)%16 == 0 )
		{
			printf("\r\n  ");
		}
	}
	printf("\n  ==================================================\n");
	/* 打印参数块*/
}

uint32_t rtl8306e_gen_mod(st_rtl8306eSettings *rtl8306e, uint8_t *mod)
{
	uint32_t len = 0;
	
	assert( NULL != rtl8306e );
	assert( NULL != mod );

	if( CMM_SUCCESS != rtl8306e_register_prepare(rtl8306e) )
	{
		printf("ERROR: call prepare_rtl8306e_register() failed\n");
		return 0;
	}

	//for debug
	//rtl8306e_register_dump();

	len = rtl8306e_mod_prepare(mod);
	
	if( 0 == len )
	{
		printf("ERROR: call rtl8306e_mod_prepare() failed\n");
	}
	else
	{
		//for debug
		//rtl8306e_mod_dump(mod, len);
	}

	return len;	
}

