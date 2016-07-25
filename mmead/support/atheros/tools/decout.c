/*====================================================================*
 *
 *   void decout (const uint8_t memory [], size_t length, char c, FILE *fp);
 *
 *   convert.h
 *
 *   print memory as a series of decimal octets seperated by chracter
 *   c; normally, c will be DEC_EXTENDER as defined in convert.h; 
 *
 *.  Motley Tools from Charlie Maier at cmaier@cmassoc.net;
 *:  Published 2006 by Charles Maier Associates Limited;
 *;  Released under the GNU General Public Licence v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef DECOUT_SOURCE
#define DECOUT_SOURCE
 
/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdint.h>
 
/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/convert.h"
 
/*====================================================================*
 *   functions;
 *--------------------------------------------------------------------*/
 
void decout (const uint8_t memory [], size_t length, char c, FILE *fp) 

{
	while (length-- > 0) 
	{
		unsigned order = 1000;
		while ((order /= 10) > 0) 
		{
			putc (DIGITS_DEC [(* memory / order) % 10], fp);
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
 

