/*====================================================================*
 *
 *   void hexout (const uint8_t memory [], size_t length, char c, FILE *fp);
 *
 *   convert.h
 *
 *   print memory as a series of hex octets seperated by character c; 
 *   normally, c will be HEX_EXTENDER as defined in convert.h;
 *
 *.  Motley Tools from Charlie Maier at cmaier@cmassoc.net;
 *:  Published 2006 by Charles Maier Associates Limited;
 *;  Released under the GNU General Public Licence v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef HEXOUT_SOURCE
#define HEXOUT_SOURCE
 
/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdint.h>
 
/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/convert.h"
#include "../tools/memory.h"
 
/*====================================================================*
 *   functions; 
 *--------------------------------------------------------------------*/
 
void hexout (const uint8_t memory [], size_t length, char c, FILE *fp) 

{
	while (length-- > 0) 
	{
		putc (DIGITS_HEX [(*memory >> 4) & NIBBLE_MASK], fp);
		putc (DIGITS_HEX [(*memory >> 0) & NIBBLE_MASK], fp);
		if (length > 0) 
		{
			putc (c, fp);
			memory++;
		}
	}
	return;
}

/*====================================================================*
 *   
 *--------------------------------------------------------------------*/

#endif
 

