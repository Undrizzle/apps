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

#include "platformTools.h"
#include "mvGpio.h"
#include "madSmi.h"

unsigned short gpioMuxIn(unsigned int smiPair)
{
    unsigned short s;    /* GPIO Selection */
    unsigned short smi, smi_mask;

    switch (smiPair)
    {
        case 0:
            smi = GPIO_SMI0;
            smi_mask = GPIO_SMI0_MASK;
            break;

        case 1:
            smi = GPIO_SMI1;
            smi_mask = GPIO_SMI1_MASK;
            break;

        case 2:
            smi = GPIO_SMI2;
            smi_mask = GPIO_SMI2_MASK;
            break;

        case 3:
            smi = GPIO_SMI3;
            smi_mask = GPIO_SMI3_MASK;
            break;

        case 4:
            smi = GPIO_SMI4;
            smi_mask = GPIO_SMI4_MASK;
            break;

        case 5:
            smi = GPIO_SMI5;
            smi_mask = GPIO_SMI5_MASK;
            break;

        case 6:
            smi = GPIO_SMI6;
            smi_mask = GPIO_SMI6_MASK;
            break;

        default:
            return 0xFFFF;
    }

    s = GPIO_GET_SELECTION(GPIO_BASE_REG_ADDR, smi_mask);
    if (s != smi)
        GPIO_SET_SELECTION(GPIO_BASE_REG_ADDR, smi, smi_mask);

    return s;
}

void gpioMuxOut(unsigned int smiPair, unsigned short sel)
{
    unsigned short smi, smi_mask;

    if (sel == 0xFFFF)
        return;

    switch (smiPair)
    {
        case 0:
            smi = GPIO_SMI0;
            smi_mask = GPIO_SMI0_MASK;
            break;

        case 1:
            smi = GPIO_SMI1;
            smi_mask = GPIO_SMI1_MASK;
            break;

        case 2:
            smi = GPIO_SMI2;
            smi_mask = GPIO_SMI2_MASK;
            break;

        case 3:
            smi = GPIO_SMI3;
            smi_mask = GPIO_SMI3_MASK;
            break;

        case 4:
            smi = GPIO_SMI4;
            smi_mask = GPIO_SMI4_MASK;
            break;

        case 5:
            smi = GPIO_SMI5;
            smi_mask = GPIO_SMI5_MASK;
            break;

        case 6:
            smi = GPIO_SMI6;
            smi_mask = GPIO_SMI6_MASK;
            break;

        default:
            return;
    }

    if (sel != smi)
        GPIO_SET_SELECTION(GPIO_BASE_REG_ADDR, sel, smi_mask);

}

