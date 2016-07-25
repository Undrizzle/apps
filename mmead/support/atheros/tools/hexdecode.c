/*====================================================================*
 *
 *   size_t hexdecode (const uint8_t memory [], size_t bytes, char buffer [], size_t chars);
 *
 *   convert.h
 *
 *   decode a memory block of given length in bytes as a string of
 *   separated hexadecimal bytes; terminate when the string fills
 *   or the memory ends; terminate the string and return the actual
 *   string length;
 *
 *   allow three string characters for each memory byte; this means
 *   that the buffer must hold at least three characters or nothing
 *   will be decoded; the maximum number of bytes is the lesser of
 *   chars/3 and bytes; 
 *
 *.  Motley Tools by Charles Maier; cmaier@cmassoc.net;
 *:  Published 2005 by Charles Maier Associates for internal use;
 *;  Released under GNU General Public License v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef HEXDECODE_SOURCE
#define HEXDECODE_SOURCE

#include "../tools/convert.h"
#include "../tools/memory.h"
 
/*====================================================================*
 *   variables; 
 *--------------------------------------------------------------------*/
 
size_t hexdecode (const uint8_t memory [], size_t bytes, char buffer [], size_t chars) 

{
	char *string = buffer;

#ifdef INTELLON_SAFEMODE
 
	if (buffer == (char) (0)) 
	{
		return (0);
	}
	if (memory == (char) (0)) 
	{
		return (0);
	}

#endif
 
	if (chars > 0) 
	{
		for (chars /= 3; (chars-- > 0) && (bytes-- > 0); memory++) 
		{
			*string++ = DIGITS_HEX [(*memory >> 4) & 0x0F];
			*string++ = DIGITS_HEX [(*memory >> 0) & 0x0F];
			if ((chars > 0) && (bytes > 0)) 
			{
				*string++ = HEX_EXTENDER;
			}
		}
		*string = (char) (0);
	}
	return (string - buffer);
}

#endif
 

