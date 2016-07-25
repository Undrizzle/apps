/*====================================================================*
 *
 *   void binout (const uint8_t memory [], size_t length, char c, FILE *fp);
 *
 *   convert.h
 *
 *   print memory as a series of binary octets separated by characte
 *   c; normally, c will be BIN_EXTENDER as defined in convert.h;
 *
 *.  Motley Tools from Charlie Maier at cmaier@cmassoc.net;
 *:  Published 2006 by Charles Maier Associates Limited;
 *;  Released under the GNU General Public Licence v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef BINOUT_SOURCE
#define BINOUT_SOURCE
 
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
 
void binout (const uint8_t memory [], size_t length, char c, FILE *fp) 

{
	while (length-- > 0) 
	{
		unsigned bits = 8;
		while (bits-- > 0) 
		{
			putc (DIGITS_BIN [(*memory >> bits) & BIT_MASK], fp);
		}
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
 

