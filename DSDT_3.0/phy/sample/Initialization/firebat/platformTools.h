/*******************************************************************************
*                   Copyright 2002, GALILEO TECHNOLOGY, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), GALILEO TECHNOLOGY LTD. (GTL) AND GALILEO TECHNOLOGY, INC. (GTI).    *
********************************************************************************
* platformTools.h - Header File for Basic gtCore logic functions and definitions
*
* DESCRIPTION:
*       This header file contains simple read/write macros for addressing
*       the SDRAM, devices, MV`s internal registers and PCI (using the PCI`s
*       address space). The macros take care of Big/Little endian conversions. 
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#ifndef __INCplatformToolsh
#define __INCplatformToolsh

/* includes */

/* globals */

/* typedefs */
/*typedef enum _bool {false,true} bool; */

/* defines  */

/****************************************/
/*          GENERAL Definitions			*/
/****************************************/
#define NO_BIT          0x00000000
#define BIT0            0x00000001
#define BIT1            0x00000002
#define BIT2            0x00000004
#define BIT3            0x00000008
#define BIT4            0x00000010
#define BIT5            0x00000020
#define BIT6            0x00000040
#define BIT7            0x00000080
#define BIT8            0x00000100
#define BIT9            0x00000200
#define BIT10           0x00000400
#define BIT11           0x00000800
#define BIT12           0x00001000
#define BIT13           0x00002000
#define BIT14           0x00004000
#define BIT15           0x00008000
#define BIT16           0x00010000
#define BIT17           0x00020000
#define BIT18           0x00040000
#define BIT19           0x00080000
#define BIT20           0x00100000
#define BIT21           0x00200000
#define BIT22           0x00400000
#define BIT23           0x00800000
#define BIT24           0x01000000
#define BIT25           0x02000000
#define BIT26           0x04000000
#define BIT27           0x08000000
#define BIT28           0x10000000
#define BIT29           0x20000000
#define BIT30           0x40000000
#define BIT31           0x80000000
    /* Size defintions */
#define _1K             0x00000400
#define _2K             0x00000800
#define _4K             0x00001000
#define _8K             0x00002000
#define _16K            0x00004000
#define _32K            0x00008000
#define _64K            0x00010000
#define _128K           0x00020000
#define _256K           0x00040000
#define _512K           0x00080000
#define _1M             0x00100000
#define _2M             0x00200000
#define _3M             0x00300000
#define _4M             0x00400000
#define _5M             0x00500000
#define _6M             0x00600000
#define _7M             0x00700000
#define _8M             0x00800000
#define _9M             0x00900000
#define _10M            0x00a00000
#define _11M            0x00b00000
#define _12M            0x00c00000
#define _13M            0x00d00000
#define _14M            0x00e00000
#define _15M            0x00f00000
#define _16M            0x01000000
#define _32M            0x02000000
#define _64M            0x04000000
#define _128M           0x08000000
#define _256M           0x10000000
#define _512M           0x20000000
#define _1G             0x40000000

/* D-Cache is not enabled */
#define D_CACHE_FLUSH_LINE(addr, offset)	
#define CPU_PIPE_FLUSH 	

#ifdef LE /* Little Endian */          	
#define SHORT_SWAP(X) (X)
#define WORD_SWAP(X) (X)
#define LONG_SWAP(X) ((l64)(X))

#else    /* Big Endian */
#define SHORT_SWAP(X) ((X <<8 ) | (X >> 8))

#define WORD_SWAP(X) (((X)&0xff)<<24)+      \
                    (((X)&0xff00)<<8)+      \
                    (((X)&0xff0000)>>8)+    \
                    (((X)&0xff000000)>>24)

#define LONG_SWAP(X) ( (l64) (((X)&0xffULL)<<56)+               \
                            (((X)&0xff00ULL)<<40)+              \
                            (((X)&0xff0000ULL)<<24)+            \
                            (((X)&0xff000000ULL)<<8)+           \
                            (((X)&0xff00000000ULL)>>8)+         \
                            (((X)&0xff0000000000ULL)>>24)+      \
                            (((X)&0xff000000000000ULL)>>40)+    \
                            (((X)&0xff00000000000000ULL)>>56))   

#endif

#define NONE_CACHEABLE(X)		(0x00000000 | (X))

/* Write 32/16/8 bit NonCacheable */

#define MV_WRITE_CHAR(address, data)                                           \
        ((*((volatile unsigned char *)NONE_CACHEABLE((address))))=             \
        ((unsigned char)(data)))
         
#define MV_WRITE_SHORT(address, data)                                          \
        ((*((volatile unsigned short *)NONE_CACHEABLE((address)))) =           \
        ((unsigned short)(data)))

#define MV_WRITE_WORD(address, data)                                           \
        ((*((volatile unsigned int *)NONE_CACHEABLE((address)))) =             \
        ((unsigned int)(data)))

/* Read 32/16/8 bits NonCacheable - returns data in variable. */
#define MV_READ_CHAR(address, pData)                                           \
        (*(pData) = (*((volatile unsigned char *)NONE_CACHEABLE(address))))

#define MV_READ_SHORT(address, pData)                                          \
        (*(pData) = (*((volatile unsigned short *)NONE_CACHEABLE(address))))

#define MV_READ_WORD(address, pData)                                           \
        (*(pData) = (*((volatile unsigned int *)NONE_CACHEABLE(address))))

/* Read 32/16/8 bit NonCacheable - returns data direct. */
#define MV_READCHAR(address)                                                   \
        ((*((volatile unsigned char *)NONE_CACHEABLE(address))))

#define MV_READSHORT(address)                                                  \
        ((*((volatile unsigned short *)NONE_CACHEABLE(address))))

#define MV_READWORD(address)                                                   \
        ((*((volatile unsigned int *)NONE_CACHEABLE(address))))


/* Write 32/16/8 bit NonCacheable after SWAP if required */

#define MV_WRITE_SWCHAR(address, data)                                           \
        ((*((volatile unsigned char *)NONE_CACHEABLE((address))))=             \
        ((unsigned char)(data)))
         
#define MV_WRITE_SWSHORT(address, data)                                          \
        ((*((volatile unsigned short *)NONE_CACHEABLE((address)))) =           \
        SHORT_SWAP((unsigned short)(data)))

#define MV_WRITE_SWWORD(address, data)                                           \
        ((*((volatile unsigned int *)NONE_CACHEABLE((address)))) =             \
        WORD_SWAP((unsigned int)(data)))

/* Read 32/16/8 bits NonCacheable after SWAP if required - returns data in variable. */
#define MV_READ_SWCHAR(address, pData)                                           \
        (*(pData) = (*((volatile unsigned char *)NONE_CACHEABLE(address))))

#define MV_READ_SWSHORT(address, pData)                                          \
        (*(pData) = SHORT_SWAP(*((volatile unsigned short *)NONE_CACHEABLE(address))))

#define MV_READ_SWWORD(address, pData)                                           \
        (*(pData) = WORD_SWAP(*((volatile unsigned int *)NONE_CACHEABLE(address))))

#define USER_MAC_MODE(_option)          (_option) & 0x1
#define USER_SPEED_MODE(_option)        ((_option) >> 1) & 0x3
                                                                                
#define USER_SPEED_100M         0
#define USER_SPEED_200M         1
#define USER_SPEED_400M         2
#define USER_SPEED_10M          3
                                                                                
#endif /* __INCplatformToolsh*/

