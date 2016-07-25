/* 
 * Copyright 2005, Marvell International Ltd.
 * 
 * THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.
 * NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT
 * OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE
 * DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.
 * THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED
 * OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.
 */


#ifndef MAD_TYPES_H
#define MAD_TYPES_H


/* general */

#undef IN
#define IN
#undef OUT
#define OUT
#undef INOUT
#define INOUT


#ifndef NULL
#define NULL ((void*)0)
#endif

typedef void  MAD_VOID;
typedef char  MAD_8;
typedef short MAD_16; 
typedef long  MAD_32;

typedef unsigned char  MAD_U8;
typedef unsigned short MAD_U16;
typedef unsigned long  MAD_U32;
typedef unsigned int   MAD_UINT; 

typedef unsigned long long  MAD_U64;

/*
typedef union {
    MAD_U8        c[8];
    MAD_U16    s[4];
    MAD_U32    l[2];
} MAD_U64;
*/

typedef enum {
    MAD_FALSE = 0,
    MAD_TRUE  = 1
} MAD_BOOL;

typedef void          (*MAD_VOIDFUNCPTR) (void); /* ptr to function returning void */
typedef unsigned int  (*MAD_INTFUNCPTR)  (void); /* ptr to function returning int  */


/* module state */
typedef enum {
    MAD_STATE_NONE = 0,    /* Uninitialized */
    MAD_STATE_ACTIVE        /* Started */
} MAD_STATE;


#include "madErrorDefs.h"


/* bit definition */
#define MAD_BIT_0       0x0001
#define MAD_BIT_1       0x0002
#define MAD_BIT_2       0x0004
#define MAD_BIT_3       0x0008
#define MAD_BIT_4       0x0010
#define MAD_BIT_5       0x0020
#define MAD_BIT_6       0x0040
#define MAD_BIT_7       0x0080
#define MAD_BIT_8       0x0100
#define MAD_BIT_9       0x0200
#define MAD_BIT_10      0x0400
#define MAD_BIT_11      0x0800
#define MAD_BIT_12      0x1000
#define MAD_BIT_13      0x2000
#define MAD_BIT_14      0x4000
#define MAD_BIT_15      0x8000

#endif /* MAD_TYPES_H */
