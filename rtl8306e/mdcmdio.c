/*
* Copyright (C) 2010 Realtek Semiconductor Corp.
* All Rights Reserved.
*
* This program is the proprietary software of Realtek Semiconductor
* Corporation and/or its licensors, and only be used, duplicated,
* modified or distributed under the authorized license from Realtek.
*
* ANY USE OF THE SOFTWARE OTEHR THAN AS AUTHORIZED UNDER 
* THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
* 
* $Revision: 10083 $ 
* $Date: 2010-06-07 11:18:41 +0800 (星期一, 07 六月 2010) $
*
* Purpose : MDC/MDIO example code
*
* Feature :  This file consists of following modules:
*                
*
*/

#include <stdio.h>
#include <stdlib.h>
//#include "rtl_types.h"
//#include "gpio.h"
#include "mdcmdio.h"      /*RTL8651B file*/
//#include "asicRegs.h"
#include "rtl8306e_types.h"

int32 smiRead(uint32 phyad, uint32 regad, uint32 * data) 
{
	*data = 0;
	printf("#smiRead(0x%x, 0x%x)\n", phyad, regad);   
	return SUCCESS ;
}

int32 smiWrite(uint32 phyad, uint32 regad, uint32 data) 
{
	printf("#smiWrite(0x%x, 0x%x)\n", phyad, regad); 
	return SUCCESS;
}

/* Function Name:
 *      smiReadBit
 * Description:
 *      Read one bit of PHY register
 * Input:
 *      phyad   - PHY address (0~31)
 *      regad   -  Register address (0 ~31) 
 *      bit       -  Register bit (0~15)   
 * Output:
 *      pdata    - the pointer of  Register bit value 
 * Return:
 *      SUCCESS         -  Success
 *      FAILED            -  Failure
 * Note:
 */

int32 smiReadBit(uint32 phyad, uint32 regad, uint32 bit, uint32 * pdata) 
{
    uint32 regData;

    if ((phyad > 31) || (regad > 31) || (bit > 15) || (pdata == NULL) ) 
        return  FAILED;
    
    if(bit>=16)
        * pdata = 0;
    else 
    {
        smiRead(phyad, regad, &regData);
        if(regData & (1<<bit)) 
            * pdata = 1;
        else
            * pdata = 0;
    }
    return SUCCESS;
}

/* Function Name:
 *      smiWriteBit
 * Description:
 *      Write one bit of PHY register
 * Input:
 *      phyad   - PHY address (0~31)
 *      regad   -  Register address (0 ~31) 
 *      bit       -  Register bit (0~15)   
 *      data     -  Bit value to be written
 * Output:
 *      none
 * Return:
 *      SUCCESS         -  Success
 *      FAILED            -  Failure
 * Note:
 */

int32 smiWriteBit(uint32 phyad, uint32 regad, uint32 bit, uint32 data) 
{
    uint32 regData;
    
    if ((phyad > 31) || (regad > 31) || (bit > 15) || (data > 1) ) 
        return  FAILED;
    smiRead(phyad, regad, &regData);
    if(data) 
        regData = regData | (1<<bit);
    else
        regData = regData & ~(1<<bit);
    smiWrite(phyad, regad, regData);
    return SUCCESS;
}



