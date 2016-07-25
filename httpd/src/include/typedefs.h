/*
 * $Copyright Open Broadcom Corporation$   
 * $Id: typedefs.h,v 1.49.8.1 2003/10/16 23:10:58 mthawani Exp $
 */

#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

/*----------------------- define TRUE, FALSE, NULL, bool ----------------*/

#ifndef FALSE
#define FALSE	0
#endif
#ifndef TRUE
#define TRUE	1
#endif
#ifndef NULL
#define	NULL 0
#endif
#ifndef ON
#define	ON	1
#endif

typedef unsigned char uchar;

typedef signed char	int8;
typedef unsigned char	uint8;
typedef signed short	int16;
typedef unsigned short	uint16;
typedef signed int	int32;
typedef unsigned int	uint32;
typedef signed long long int64;
typedef unsigned long long uint64;
typedef float		float32;
typedef double		float64;

typedef unsigned long	ulong;

#endif /* _TYPEDEFS_H_ */ 

