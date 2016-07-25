/*******************************************************************************
*                Copyright 2005, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
*********************************************************************************/
/* 
 * FILENAME:    $Workfile: bspSpecificMii.c $ 
 * REVISION:    $Revision: 1 $ 
 * 
 * DESCRIPTION: SMI access routines for 88E6218 board
 *     
 */
#include "madSample.h"

/*
 * For each platform, all we need is 
 * Assigning SMI Read/Write functions 
 *         fgtReadMii : to read MII registers, and
 *         fgtWriteMii : to write MII registers.
*/


/* 
 *  Firefox(88E6218) Specific Definition
 */
#define SMI_OP_CODE_BIT_READ                    1
#define SMI_OP_CODE_BIT_WRITE                   0
#define SMI_BUSY                                1<<28
#define READ_VALID                              1<<27

#define SMI_TIMEOUT_COUNTER                1000


#define PLATFORM_MADREG_BASE     0x80000000    
#define FAST_ETH_MAC_OFFSET          0x8000
#define MAD_REG_ETHER_SMI_REG        (FAST_ETH_MAC_OFFSET+0x10) 

/* Access to 88E6218 registers (Read/Write) */
static __inline void madOsGtRegWrite(MAD_U32 madreg, MAD_U32 data)
{
    *(volatile MAD_U32*)(PLATFORM_MADREG_BASE|madreg) = data;
}

static __inline MAD_U32 madOsGtRegRead(MAD_U32 madreg)
{
    return *(volatile MAD_U32*)(PLATFORM_MADREG_BASE|madreg);
}



/*****************************************************************************
*
* bspSpecificReadMii
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
MAD_BOOL bspSpecificReadMii(MAD_DEV* dev, unsigned int portNumber , unsigned int MIIReg,
                        unsigned int* value)
{
     MAD_U32            smiReg;
    unsigned int    phyAddr;
    unsigned int    timeOut = SMI_TIMEOUT_COUNTER; /* in 100MS units */
    int                i;

    /* first check that it is not busy */
    smiReg = madOsGtRegRead(MAD_REG_ETHER_SMI_REG);    
    if (smiReg & SMI_BUSY) 
    {
        for(i=0; i<SMI_TIMEOUT_COUNTER; i++);
        do 
        {
            smiReg = madOsGtRegRead(MAD_REG_ETHER_SMI_REG);
            if (timeOut-- < 1) 
            {
                return MAD_FALSE;
            }            
        } while (smiReg & SMI_BUSY);
    }    
    /* not busy */
    phyAddr = portNumber;
    smiReg =  (phyAddr << 16) | (SMI_OP_CODE_BIT_READ << 26) | (MIIReg << 21) 
            | SMI_OP_CODE_BIT_READ << 26;

    madOsGtRegWrite(MAD_REG_ETHER_SMI_REG, smiReg);
    timeOut = SMI_TIMEOUT_COUNTER; /* initialize the time out var again */
    smiReg = madOsGtRegRead(MAD_REG_ETHER_SMI_REG);
    if (!(smiReg & READ_VALID)) 
    {
        for(i = 0 ; i < SMI_TIMEOUT_COUNTER; i++);
        do 
        {
            smiReg = madOsGtRegRead(MAD_REG_ETHER_SMI_REG);
            if (timeOut-- < 1 ) 
            {
                return MAD_FALSE;
            }
        } while (!(smiReg & READ_VALID));
    }
    *value = (unsigned int)(smiReg & 0xffff);    

    return MAD_TRUE;
}

/*****************************************************************************
* 
* bspSpecificWriteMii
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
MAD_BOOL bspSpecificWriteMii(MAD_DEV* dev, unsigned int portNumber , unsigned int MIIReg,
                       unsigned int value)
{
    MAD_U32            smiReg;
    unsigned int    phyAddr;
    unsigned int    timeOut = SMI_TIMEOUT_COUNTER; /* in 100MS units */
    int                i;

    /* first check that it is not busy */    
    smiReg = madOsGtRegRead(MAD_REG_ETHER_SMI_REG);
    if (smiReg & SMI_BUSY) 
    {
        for(i=0; i<SMI_TIMEOUT_COUNTER; i++);
        do 
        {
            smiReg = madOsGtRegRead(MAD_REG_ETHER_SMI_REG);
            if (timeOut-- < 1) 
            {
                return MAD_FALSE;
            }            
        } while (smiReg & SMI_BUSY);
    }
    /* not busy */
    phyAddr = portNumber;

    smiReg = 0; /* make sure no garbage value in reserved bits */
    smiReg = smiReg | (phyAddr << 16) | (SMI_OP_CODE_BIT_WRITE << 26) |
             (MIIReg << 21) | (value & 0xffff);    
    madOsGtRegWrite(MAD_REG_ETHER_SMI_REG, smiReg);    

    return MAD_TRUE;
}
