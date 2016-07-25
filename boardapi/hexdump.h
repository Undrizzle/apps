/*====================================================================*
 *
 *   void hexdump (const byte memory [], size_t length, FILE *fp);
 *
 *   convert.h
 *
 *   print memmory as a hexadecimal dump with address offsets and 
 *   ASCII display;
 *
 *.  Motley Tools from Charlie Maier at cmaier@cmassoc.net;
 *:  Published 2006 by Charles Maier Associates Limited;
 *;  Released under the GNU General Public Licence v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef __CMM_HEXDUMP_H__
#define __CMM_HEXDUMP_H__

extern void hexdump (const unsigned char memory [], size_t length, FILE *fp) ;

#endif

