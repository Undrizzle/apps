
/********************************************************************************
* ev96122mii.c
*
* DESCRIPTION:
*       SMI access routines for EV-96122 board
*
* DEPENDENCIES:   Platform.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/mii.h>
#include "msApiInit.h"

/*
 * For each platform, all we need is 
 * 1) Assigning functions into 
 *         fgtReadMii : to read MII registers, and
 *         fgtWriteMii : to write MII registers.
 *
 * 2) Register Interrupt (Not Defined Yet.)
*/

/* 
 *  EV-96122 Specific Definition
*/

typedef enum _bool{false,true} bool;

/*****************************************************************************
*
* bool etherReadMIIReg (unsigned int portNumber , unsigned int MIIReg,
* unsigned int* value)
*
* Description
* This function will access the MII registers and will read the value of
* the MII register , and will retrieve the value in the pointer.
* Inputs
* portNumber - one of the 2 possiable Ethernet ports (0-1).
* MIIReg - the MII register offset.
* Outputs
* value - pointer to unsigned int which will receive the value.
* Returns Value
* true if success.
* false if fail to make the assignment.
* Error types (and exceptions if exist)
*/

GT_BOOL gtBspReadMii (GT_QD_DEV* dev, unsigned int portNumber , unsigned int MIIReg,
                        unsigned int* value)
{

    	int fd;
	struct ifreq ifr; 
	struct mii_ioctl_data *smi_data = NULL;	

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
	{
		perror("  gtBspReadMii socket error !\n");     
		return false;
	}

	memset(&ifr,0,sizeof(ifr)); 
	strcpy(ifr.ifr_name, "eth0"); 
	smi_data = (struct mii_ioctl_data *)&(ifr.ifr_ifru);
	
	smi_data->phy_id = portNumber;
	smi_data->reg_num = MIIReg;
	
	if(ioctl( fd, SIOCGMIIREG, &ifr ) < 0 )   
	{     
		perror("  gtBspReadMii ioctl error !\n");
		close(fd);
		return false;     
	}
	else
	{
		*value = (smi_data->val_out)&0x0000ffff;
		return true;
	}
}

/*****************************************************************************
* 
* bool etherWriteMIIReg (unsigned int portNumber , unsigned int MIIReg,
* unsigned int value)
* 
* Description
* This function will access the MII registers and will write the value
* to the MII register.
* Inputs
* portNumber - one of the 2 possiable Ethernet ports (0-1).
* MIIReg - the MII register offset.
* value -the value that will be written.
* Outputs
* Returns Value
* true if success.
* false if fail to make the assignment.
* Error types (and exceptions if exist)
*/

GT_BOOL gtBspWriteMii (GT_QD_DEV* dev, unsigned int portNumber , unsigned int MIIReg,
                       unsigned int value)
{
	int fd;
	struct ifreq ifr; 
	struct mii_ioctl_data *smi_data = NULL;	

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
	{
		perror("  gtBspWriteMii socket error !\n");     
		return false;
	}

	memset(&ifr,0,sizeof(ifr)); 
	strcpy(ifr.ifr_name, "eth0"); 
	smi_data = (struct mii_ioctl_data *)&(ifr.ifr_ifru);
	
	smi_data->phy_id = portNumber;
	smi_data->reg_num = MIIReg;
	smi_data->val_in = value;
	
	if( ioctl( fd, SIOCSMIIREG, &ifr ) < 0 )   
	{     
		perror("  gtBspWriteMii ioctl error !\n");
		close(fd);
		return false;     
	}
	else
	{
		return(true);
	}
}


void gtBspMiiInit(GT_QD_DEV* dev)
{
    return;    
}
