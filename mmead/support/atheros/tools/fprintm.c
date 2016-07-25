/*====================================================================*
 *
 *   void fprintm (FILE *fp, const char * format, const uint8_t memory [], size_t length);
 *   void printm (const char * format, const uint8_t memory [], size_t length);
 *
 *   convert.h
 *
 *   print memory as a stream of octets embedded within 
 *   a character string; the string is passed as a format string as
 *   with fprintf the string can contain one of the following:
 *
 *   %d or %D	replace this with the memory region formatted
 *		as decimal octets; separate octets with ' ' 
 *		for %d and DEC_EXTENDER for %D;
 *
 *   %b or %B	replace this with the memory region formatted
 *		as binary octets; separate octets with ' ' 
 *		for %b and BIN_EXTENDER for %B;
 *
 *   %X or %X	replace this with the memory region formatted
 *		as hexadecimal octets; separate octets with ' ' 
 *		for %x and HEX_EXTENDER for %X;
 *
 *.  Motley Tools from Charlie Maier at cmaier@cmassoc.net;
 *:  Published 2006 by Charles Maier Associates Limited;
 *;  Released under the GNU General Public Licence v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef FPRINTM_SOURCE
#define FPRINTM_SOURCE
 
/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
 
/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/memory.h"
#include "../tools/convert.h"
 
/*====================================================================*
 *   functions;
 *--------------------------------------------------------------------*/
 
void fprintm (FILE *fp, const char * format, const uint8_t memory [], size_t length) 

{
	while (*format != (char)(0)) 
	{
		switch (*format) 
		{
		case '%':
			switch (*++format) 
			{
			case 'd':
				decout (memory, length, ' ', fp);
				break;
			case 'D':
				decout (memory, length, DEC_EXTENDER, fp);
				break;
			case 'b':
				binout (memory, length, ' ', fp);
				break;
			case 'B':
				binout (memory, length, BIN_EXTENDER, fp);
				break;
			case 'x':
				hexout (memory, length, ' ', fp);
				break;
			case 'X':
				hexout (memory, length, HEX_EXTENDER, fp);
				break;
			default:
				putc (*--format, fp);
				putc (*++format, fp);
				break;
			}
			break;
		case '\\':
			switch (*++format) 
			{
			case 'f':
				putc ('\f', fp);
				break;
			case 'r':
				putc ('\r', fp);
				break;
			case 'n':
				putc ('\n', fp);
				break;
			case 't':
				putc ('\t', fp);
				break;
			default:
				putc (*format, fp);
				break;
			}
			break;
		default:
			putc (*format, fp);
			break;
		}
		format++;
	}
	return;
}

void printm (const char * format, const uint8_t memory [], size_t length) 

{
	fprintm (stdout, format, memory, length);
	return;
}

/*====================================================================*
 *
 *   int main (int argc, const char *argv []);
 *
 *   print memory[] using format strings from command line arguments;
 *
 *.  Motley Tools from Charlie Maier at cmaier@cmassoc.net;
 *:  Published 2006 by Charles Maier Associates Limited;
 *;  Released under the GNU General Public Licence v2 and later;
 *
 *--------------------------------------------------------------------*/

#if 0

#include "../tools/binout.c"
#include "../tools/hexout.c"
#include "../tools/decout.c"
 
int main (int argc, const char *argv []) 

{
	char memory [] = 
	{
		0x00,
		0x01,
		0x02,
		0x03,
		0x04,
		0x05,
		0x06,
		0x07,
		0x08,
		0x09,
		0x0A,
		0x0B,
		0x0C,
		0x0D,
		0x0E,
		0x0F 
	};
	while ((--argc > 0) && (*++argv != (const char *)(0))) 
	{
		printm (*argv, memory, sizeof (memory));
	}
	return (0);
}

#endif
 
/*====================================================================*
 *
 *--------------------------------------------------------------------*/

#endif
 

