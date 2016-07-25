/*====================================================================*
 *
 *   uint32_t checksum32 (const int32_t memory [], size_t length, uint32_t checksum);
 *
 *   memory.h
 *
 *   compute the 32 bit checksum of a memory segment; the checksum
 *   is the one's complement of the XOR of all 32 bit words;
 *
 *.  Motley Tools from Charlie Maier at cmaier@cmassoc.net;
 *:  Published 2006 by Charles Maier Associates Limited;
 *;  Released under the GNU General Public Licence v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef CHECKSUM32_SOURCE
#define CHECKSUM32_SOURCE

#include <stdint.h>
#include <unistd.h>

#include "../tools/memory.h"
 
uint32_t checksum32 (const uint32_t memory [], size_t length, uint32_t checksum) 

{
	while (length-- > 0) 
	{
		checksum ^= tntohl(*memory);
		memory++;
	}
	return (~checksum);
}

#endif
 

