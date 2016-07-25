/*====================================================================*
 *
 *   types.h - generic data type definitions and declarations; 
 *
 *   these are our favorite definitions and declarations; they are used
 *   throughout our software for clarity and consistency; learn to love
 *   them and make them your own;
 *
 *.  Motley Tools from Charlie Maier at cmaier@cmassoc.net;
 *:  Published 2006 by Charles Maier Associates Limited;
 *;  Released under the GNU General Public Licence v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef TYPES_HEADER
#define TYPES_HEADER
 
/*====================================================================*
 *   old POSIX types; 
 *--------------------------------------------------------------------*/

#include <stdint.h>
 
/*====================================================================*
 *   new POSIX types; 
 *--------------------------------------------------------------------*/
 
typedef signed errno_t;
typedef signed signo_t;
typedef unsigned char byte;

/*====================================================================*
 *   define C++ style true and false for use in standard C programs;
 *--------------------------------------------------------------------*/

#ifndef __cplusplus


typedef enum 

{
	false,
	true 
}

bool;

#endif

#ifdef __GNUC__
#ifndef __packed
#define __packed __attribute__ ((packed))
#endif
#endif
 
/*====================================================================*
 *   define flagword (bitmap) variable type for clarity;
 *--------------------------------------------------------------------*/
 
typedef signed file_t;
typedef signed sock_t;
typedef signed code_t;
typedef uint16_t type_t;
typedef unsigned char byte_t;
typedef unsigned flag_t;
typedef struct _file_ 

{
	file_t fd;
	const char *name;
}

file;
typedef struct _type_ 

{
	type_t type;
	const char *name;
}

TYPE;
typedef struct _code_ 

{
	code_t code;
	const char *name;
}

CODE;

/*====================================================================*
 *   end definitions and declarations;
 *--------------------------------------------------------------------*/

#endif
 

