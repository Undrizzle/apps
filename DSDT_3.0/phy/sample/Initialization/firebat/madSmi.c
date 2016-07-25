/*
 * (C) Copyright 2005
 * Marvell Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
             
#include "madSample.h"

#include "platformTools.h"
#include "madSoc.h"
#include "mvGpio.h"
#include "madSmi.h"
/*#include "qd.h" */

void madSmiInit(void);

void madSmiInit()
{
    MV_WRITE_SWWORD(SMI7_MDIO_TIMING_REG, 0x1555);
    MV_WRITE_SWWORD(SMI7_MDC_FREQ_REG, 0x3fff);

#if 0
//#ifndef CONFIG_MINI_UBOOT
    /* look for switch devices which might be connected to the FireBat */
    madInit(-1);
                                                                        
    if(!madExist())
    {
        /*
         * Enable UniMAC SMI
        */
        GPIO_SET_SELECTION(GPIO_BASE_REG_ADDR, GPIO_UNIMAC, GPIO_UNIMAC_MASK);
    }
#endif
}


/*****************************************************************************
*
* madSmiRead
*
* DESCRIPTION
*     This function read SMI register of the given SMI Pair and SMI Address.
*     Valid SMI Pair is 0 ~ 6.
* 
* Inputs
*    smiPair    - mdc/mdio pair number
*     smiAddr - smi address 
*     reg     - the MII register offset.
*
* Outputs
*     value - pointer to unsigned int which will receive the value.
*
* Returns Value
*     1, if success.
*     0, if fail.
*
*/

int madSmiRead (unsigned int smiPair, unsigned int smiAddr, 
            unsigned int reg, unsigned int* value)
{
    SMI7_REG smiReg;
    int timeOut;
    volatile int i;
    unsigned short gpio;

    if (smiPair >= MAX_MDC_MDIO_PAIR)
        return 0;

    /* first check that it is not busy */
    timeOut = SMI7_TIMEOUT_COUNTER;
    SMI7_COMMAND_READ (smiPair,&smiReg);    
    if (smiReg & SMI7_BUSY) {
        /* smiPair 0 for switch */
        for(i = 1 ; i < SMI7_TIMEOUT_COUNTER; i++);
        do {
            SMI7_COMMAND_READ (smiPair,&smiReg);    
            if (timeOut-- < 1) {
                return 0;
            }            
        } while (smiReg & SMI7_BUSY);
    }    

    /* if not enabled, enable the given smi pair from GPIO muliplxer */
    gpio = gpioMuxIn(smiPair);

    /* set smi command register */
    smiReg =  (smiAddr << 16) | (SMI7_OP_CODE_BIT_READ << 26) | (reg << 21) 
        | (SMI7_MODE << 28) | (SMI7_BUSY);

    SMI7_COMMAND_WRITE (smiPair,smiReg);    

    /* initialize the time out var again */
    timeOut = SMI7_TIMEOUT_COUNTER;

    SMI7_COMMAND_READ (smiPair,&smiReg);    

    if (!(smiReg & SMI7_READ_VALID)) 
    {
        /* smiPair 0 for switch */
        for(i = 1 ; i < SMI7_TIMEOUT_COUNTER; i++);
        do 
        {
            SMI7_COMMAND_READ (smiPair,&smiReg);    
            if (timeOut-- < 1 ) 
            {
                break;
            }
         } while (!(smiReg & SMI7_READ_VALID));
    }

    *value = smiReg & 0xFFFF;

    /* write back GPIO muliplxer selection */
/*    gpioMuxOut(smiPair,gpio); */

    return 1;
}


/*****************************************************************************
* 
* madSmiWrite
* 
* DESCRIPTION
*     This function writes data to a SMI register of the given SMI Pair and 
*    SMI Address. Valid SMI Pair is 0 ~ 6.
* 
* Inputs
*    smiPair    - mdc/mdio pair number
*     smiAddr - smi address 
*     reg     - the MII register offset.
*     value - pointer to unsigned int which will receive the value.
*
* Outputs
*    none
*
* Returns Value
*     1, if success.
*     0, if fail.
*
*/

int madSmiWrite (unsigned int smiPair, unsigned int smiAddr, 
            unsigned int reg, unsigned int value)
{
    SMI7_REG smiReg;
    int timeOut;
    volatile int i;
    unsigned short gpio;
    
    if (smiPair >= MAX_MDC_MDIO_PAIR)
        return 0;

    /* first check that it is not busy */    
    timeOut = SMI7_TIMEOUT_COUNTER;
    SMI7_COMMAND_READ (smiPair,&smiReg);    
    if (smiReg & SMI7_BUSY) 
    {
        /* smiPair 0 for switch */
        for(i = 1 ; i < SMI7_TIMEOUT_COUNTER; i++);
        do {
            SMI7_COMMAND_READ (smiPair,&smiReg);
            if (timeOut-- < 1) 
            {
                return 0;
            }            
        } while (smiReg & SMI7_BUSY);
    }    

    /* if not enabled, enable the given smi pair from GPIO muliplxer */
    gpio = gpioMuxIn(smiPair);
    
    /* set smi command register */
    smiReg = (smiAddr << 16) | (SMI7_OP_CODE_BIT_WRITE << 26) |
        (reg << 21) | (SMI7_MODE << 28) | (SMI7_BUSY) | (value & 0xffff);    

    SMI7_COMMAND_WRITE (smiPair,smiReg);    
    
    /* write back GPIO muliplxer selection */
/*    gpioMuxOut(smiPair,gpio); */

    return 1;
}

