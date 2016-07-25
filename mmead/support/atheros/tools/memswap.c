/*====================================================================*
 *
 *   void memswap (uint8_t buffer1[], uint8_t buffer2[], size_t length);
 *   
 *   exchange the contents of one buffer with that of another; return
 *   no value; 
 *
 *   one application for this function is to exchange the source and
 *   destination addresses in an Ethernet frame to form a response
 *   message;
 *
 *.  Motley Tools from Charlie Maier at cmaier@cmassoc.net;
 *:  Published 2006 by Charles Maier Associates Limited;
 *;  Released under the GNU General Public Licence v2 and later;
 *   
 *--------------------------------------------------------------------*/

#ifndef MEMSWAP_SOURCE
#define MEMSWAP_SOURCE

#include <stdint.h>

#include "../tools/memory.h"
 
void memswap (uint8_t buffer1 [], uint8_t buffer2 [], size_t length) 

{
	if (buffer1 == buffer2) 
	{
		return;
	}

#if INTELLON_SAFEMODE
 
	if (buffer1 == (uint8_t *)(0)) 
	{
		return;
	}
	if (buffer2 == (uint8_t *)(0)) 
	{
		return;
	}

#endif
 
	while (length-- > 0) 
	{
		uint8_t byte = *buffer1;
		*buffer1++ = *buffer2;
		*buffer2++ = byte;
	}
	return;
}

#endif
 

