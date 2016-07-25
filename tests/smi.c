#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/mii.h>

#define PHY_MAX_ADDR 				32
#define PHYID_88E6171R_GLOBAL2	0x1c
#define REG_PHY_CR					0x18
#define REG_PHY_DR					0x19

#define PHY_CR_BUSY					(1<<15)
#define PHY_CR_MODE_C22			(1<<12)
#define PHY_CR_MODE_C45			(0<<12)
#define PHY_CR_OP_READ				(2<<10)
#define PHY_CR_OP_WRITE			(1<<10)
#define PHY_CR_ADDR_OFFS			5

void smi_usage(void)
{
	printf("\nParameter error:\n");
	printf("\nUsage:\n");
	printf("smi {read|write}\n");
	printf("	smi read phy <addr> reg <register>\n");
	printf("	-- test read switch register var smi interface\n");
	printf("	smi write phy <addr> reg <register> value <value> \n");
	printf("	-- test write switch register var smi interface\n");
	
	printf("\n\n");
}

unsigned short smi_read(unsigned short phyaddr, unsigned short regnum)
{
	int fd;
	struct ifreq ifr; 
	struct mii_ioctl_data *smi_data = NULL;	

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
	{
		perror("  smi_read socket error !\n");     
		return 0;
	}

	memset(&ifr,0,sizeof(ifr)); 
	strcpy(ifr.ifr_name, "eth0"); 
	smi_data = (struct mii_ioctl_data *)&(ifr.ifr_ifru);
	
	smi_data->phy_id = phyaddr;
	smi_data->reg_num = regnum;
	
	if(ioctl( fd, SIOCGMIIREG, &ifr ) < 0 )   
	{     
		perror("  smi_read ioctl error !\n");
		close(fd);
		return 0;     
	}
	else
	{
		return smi_data->val_out;
	}
	
}

int smi_write(unsigned short phyaddr, unsigned short regnum, unsigned short regvalue)
{
	int fd;
	struct ifreq ifr; 
	struct mii_ioctl_data *smi_data = NULL;	

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
	{
		perror("  smi_write socket error !\n");     
		return -1;
	}

	memset(&ifr,0,sizeof(ifr)); 
	strcpy(ifr.ifr_name, "eth0"); 
	smi_data = (struct mii_ioctl_data *)&(ifr.ifr_ifru);
	
	smi_data->phy_id = phyaddr;
	smi_data->reg_num = regnum;
	smi_data->val_in = regvalue;
	
	if( ioctl( fd, SIOCSMIIREG, &ifr ) < 0 )   
	{     
		perror("  smi_write ioctl error !\n");
		close(fd);
		return -1;     
	}
	else
	{
		return 0;
	}
	
}

#define CHECK_PHY_CR_BUSY(phy, reg)	\
	while(1)	\
	{ \
		value = smi_read(phy, reg); \
		if( value & PHY_CR_BUSY ) \
		{ \
			usleep(10); \
		} \
		else \
		{ \
			break; \
		} \
	}

unsigned short smi_phy_read(unsigned short phyaddr, unsigned short regnum)
{
	unsigned short value = 0xffff;
	
	/* check if phy command register is busy */
	CHECK_PHY_CR_BUSY(PHYID_88E6171R_GLOBAL2, REG_PHY_CR);

	value = 0;
	value = PHY_CR_BUSY|PHY_CR_MODE_C22|PHY_CR_OP_READ|(phyaddr<<PHY_CR_ADDR_OFFS)|regnum;
	printf("\n  1: write phy 0x%x reg 0x%x value 0x%x\n", PHYID_88E6171R_GLOBAL2, REG_PHY_CR, value);
	smi_write(PHYID_88E6171R_GLOBAL2, REG_PHY_CR, value);

	/* wait when phy command register is busy */
	CHECK_PHY_CR_BUSY(PHYID_88E6171R_GLOBAL2, REG_PHY_CR);

	printf("  2: read phy 0x%x reg 0x%x\n\n", PHYID_88E6171R_GLOBAL2, REG_PHY_DR);
	return smi_read(PHYID_88E6171R_GLOBAL2, REG_PHY_DR);

}

int smi_phy_write(unsigned short phyaddr, unsigned short regnum, unsigned short regvalue)
{
	unsigned short value = 0xffff;
	
	/* check if phy command register is busy */
	CHECK_PHY_CR_BUSY(PHYID_88E6171R_GLOBAL2, REG_PHY_CR);

	printf("\n  1: write phy 0x%x reg 0x%x value 0x%x\n", PHYID_88E6171R_GLOBAL2, REG_PHY_DR, regvalue);
	/* write regvalue to phy data register */
	smi_write(PHYID_88E6171R_GLOBAL2, REG_PHY_DR, regvalue);
	
	value = 0;
	value = PHY_CR_BUSY|PHY_CR_MODE_C22|PHY_CR_OP_WRITE|(phyaddr<<PHY_CR_ADDR_OFFS)|regnum;
	printf("  2: write phy 0x%x reg 0x%x value 0x%x\n\n", PHYID_88E6171R_GLOBAL2, REG_PHY_CR, value);
	return smi_write(PHYID_88E6171R_GLOBAL2, REG_PHY_CR, value);
}

unsigned short smi_port_read(unsigned short phyaddr, unsigned short regnum)
{
	return smi_read(phyaddr, regnum);
}

int smi_port_write(unsigned short phyaddr, unsigned short regnum, unsigned short regvalue)
{
	return smi_write(phyaddr, regnum, regvalue);	
}

unsigned short do_smi_read(unsigned short phyaddr, unsigned short regnum)
{
	if( phyaddr <= 6 )
	{
		return smi_phy_read(phyaddr, regnum);
	}
	else if( phyaddr < PHY_MAX_ADDR )
	{
		return smi_port_read(phyaddr, regnum);
	}
	else
	{
		perror("  ERROR: invalid phyaddr\n"); 
		return 0xffff;
	}
}

int do_smi_write(unsigned short phyaddr, unsigned short regnum, unsigned short regvalue)
{
	if( phyaddr <= 6 )
	{
		return smi_phy_write(phyaddr, regnum, regvalue);
	}
	else if( phyaddr < PHY_MAX_ADDR )
	{
		return smi_port_write(phyaddr, regnum, regvalue);
	}
	else
	{
		perror("  ERROR: invalid phyaddr\n"); 
		return -1;
	}	
}

int main(int argc, char *argv[])
{
	unsigned int phyaddr = 0;
	unsigned int regnum = 0;
	unsigned int regvalue = 0;
	
	if( (argc != 6) && (argc != 8) )
	{
		smi_usage();
		return 0;
	}
	if( strcmp(argv[1], "read") == 0 )
	{
		if( argc != 6 )
		{
			smi_usage();
			return 0;
		}
		if( strcmp(argv[2], "phy") != 0 )
		{
			smi_usage();
			return 0;
		}
		if( strcmp(argv[4], "reg") != 0 )
		{
			smi_usage();
			return 0;
		}
		sscanf(argv[3], "%x", &phyaddr);
		sscanf(argv[5], "%x", &regnum);
		printf("\n  smi read phy 0x%x reg 0x%x:\n", phyaddr, regnum);
		regvalue = do_smi_read(phyaddr, regnum);
		printf("  value: 0x%x\n\n", regvalue);
		return 0;
	}
	else if( strcmp(argv[1], "write") == 0 )
	{
		if( argc != 8 )
		{
			smi_usage();
			return 0;
		}
		if( strcmp(argv[2], "phy") != 0 )
		{
			smi_usage();
			return 0;
		}
		if( strcmp(argv[4], "reg") != 0 )
		{
			smi_usage();
			return 0;
		}
		if( strcmp(argv[6], "value") != 0 )
		{
			smi_usage();
			return 0;
		}
		sscanf(argv[3], "%x", &phyaddr);
		sscanf(argv[5], "%x", &regnum);
		sscanf(argv[7], "%x", &regvalue);
		printf("\n  smi write phy 0x%x reg 0x%x value 0x%x\n", phyaddr, regnum, regvalue);
		if( 0 == do_smi_write(phyaddr, regnum, regvalue) )
		{
			printf("  success\n\n");
		}
		else
		{
			printf("  failed\n\n");
		}
		
		return 0;
	}
	else
	{
		smi_usage();
		return 0;
	}
}

