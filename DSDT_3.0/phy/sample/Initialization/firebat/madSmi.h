/*
 * (C) Copyright 2005, Marvell Semiconductor, Inc.
 *
 * Configuation settings for the Firebat development board.
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

#ifndef __SMI_H
#define __SMI_H

#define MAX_MDC_MDIO_PAIR    7

/* SMI Registers */
#define GPIO_BASE_REG_ADDR        0x8000d000
#define SMI7_BASE_REG            0x90005000

#define SMI7_CMD_REG            (SMI7_BASE_REG | 0x00)
#define SMI7_MDC_FREQ_REG        (SMI7_BASE_REG | 0x1C)
#define SMI7_MDIO_TIMING_REG        (SMI7_BASE_REG | 0x20)

#define SMI7_DATA_SIZE        4    /* 4 bytes */

#define SMI7_OP_CODE_BIT_READ    2
#define SMI7_OP_CODE_BIT_WRITE   1
#define SMI7_MODE        1    /* IEEE 802.3 Clause 22 */
#define SMI7_BUSY                (1<<30)
#define SMI7_READ_VALID              (1<<29)

#define SMI7_TIMEOUT_COUNTER        10000

#define SMI7_COMMAND_READ(smi,pData)                                        \
    *pData = WORD_SWAP(*(volatile unsigned int *) (SMI7_CMD_REG+(4*smi)))

#define SMI7_COMMAND_WRITE(smi,data)                                     \
    *(volatile unsigned int *)(SMI7_CMD_REG+(4*smi)) = WORD_SWAP((data))

typedef unsigned int              SMI7_REG;

#endif /* __SMI_H_ */
