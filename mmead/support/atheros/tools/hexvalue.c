/*====================================================================*
 *
 *   unsigned hexvalue (unsigned c, unsigned value);
 *
 *   convert.h
 *   
 *   increment value by the hexadecimal equivalent of digit c and
 *   return the result; set errno to EINVAL and return 16 if c is
 *   not a hexadecimal digit;
 *
 *   for example, hexvalue ("A", 0) returns 0xA or 10; the following
 *   statements set variable "value" to 0xABCD:
 *
 *      unsigned value = 0;
 *      value = hexvalue ("A", value << 4);
 *      value = hexvalue ("b", value << 4);
 *      value = hexvalue ("C", value << 4);
 *      value = hexvalue ("d", value << 4);
 *	if (errno == EINVAL)
 *	{
 *	   error (1, errno, "Rats!");
 *	}
 * 
 *
 *.  Motley Tools by Charles Maier; cmaier@cmassoc.net;
 *:  Published 2005 by Charles Maier Associates for internal use;
 *;  Released under GNU General Public License v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef HEXVALUE_SOURCE
#define HEXVALUE_SOURCE

#include <errno.h>

#include "../tools/convert.h"
 
unsigned hexvalue (unsigned c, register unsigned value) 

{
	switch ((char)(c)) 
	{
	case 'F':
	case 'f':
		value++;
	case 'E':
	case 'e':
		value++;
	case 'D':
	case 'd':
		value++;
	case 'C':
	case 'c':
		value++;
	case 'B':
	case 'b':
		value++;
	case 'A':
	case 'a':
		value++;
	case '9':
		value++;
	case '8':
		value++;
	case '7':
		value++;
	case '6':
		value++;
	case '5':
		value++;
	case '4':
		value++;
	case '3':
		value++;
	case '2':
		value++;
	case '1':
		value++;
	case '0':
		errno = 0;
		break;
	default:
		errno = EINVAL;
		return (16);
	}
	return (value);
}

#endif
 

