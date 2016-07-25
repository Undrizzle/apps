/*====================================================================*
 *
 *   convert.h
 *
 *.  Motley Tools by Charles Maier; cmaier@cmassoc.net;
 *:  Published 2005 by Charles Maier Associates for internal use;
 *;  Released under GNU General Public License v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef CONVERT_HEADER
#define CONVERT_HEADER
 
/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdint.h>
 
/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/types.h"
#include "../tools/tools.h"
 
/*====================================================================*
 *   character constants;
 *--------------------------------------------------------------------*/

#define BIN_EXTENDER ' '
#define DEC_EXTENDER '.'
#define HEX_EXTENDER ':'
 
/*====================================================================*
 *   string constants;
 *--------------------------------------------------------------------*/

#define DIGITS_BIN "01" 
#define DIGITS_DEC "0123456789" 
#define DIGITS_HEX "0123456789ABCDEF" 
 
/*====================================================================*
 *   functions;
 *--------------------------------------------------------------------*/
 
unsigned hexvalue (unsigned c, unsigned value);

/*====================================================================*
 *   end definitions;
 *--------------------------------------------------------------------*/

#endif
 

