/*====================================================================*
 *
 *   signed hexencode (uint8_t memory [], size_t length, const char *string);
 *
 *   convert.h
 *
 *   convert a hexadecimal string to a byte array; permit an optional
 *   HEX_EXTENDER character between successive octets;
 *
 *   constant character HEX_EXTENDER is defined in convert.h;
 *
 *.  Motley Tools by Charles Maier; cmaier@cmassoc.net;
 *:  Published 2005 by Charles Maier Associates for internal use;
 *;  Released under GNU General Public License v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef HEXENCODE_SOURCE
#define HEXENCODE_SOURCE

#include <ctype.h>
#include <errno.h>

#include "../tools/memory.h"
#include "../tools/convert.h"
 
signed hexencode (uint8_t memory [], size_t length, const char *string) 

{
	while (length-- > 0) 
	{
		*memory = hexvalue (*string++, *memory << 4);
		if (errno) 
		{
			return (-1);
		}
		*memory = hexvalue (*string++, *memory << 4);
		if (errno) 
		{
			return (-1);
		}
		memory++;
		if (*string == HEX_EXTENDER) 
		{
			string++;
		}
	}
	return (0);
}

#endif
 

