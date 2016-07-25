/*====================================================================*
 *
 *   memory.h
 *
 *.  Motley Tools from Charlie Maier at cmaier@cmassoc.net;
 *:  Published 2006 by Charles Maier Associates Limited;
 *;  Released under the GNU General Public Licence v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef MEMORY_HEADER
#define MEMORY_HEADER
 
/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
 
/*====================================================================*
 *
 *--------------------------------------------------------------------*/
/* Byte order functionalities are provided by the following header
 * files. For other OS, add the appropriate header file.
 */

#if defined (_WIN32)
# include <winsock2.h>
#else
# include <netinet/in.h>
#endif

/* The byte ordering of intellon Vendor Specific MMEs content 
 * are little endian, i.e. that it is assume that the host 
 * is always little endian. The following has been added to 
 * handle the byte ordering of the Intellon VS MMEs.
 *
 * Note: The byte ordering of the Ethernet header, within a 
 * intellon VS MME, is big endian (network).
 */
#if __BYTE_ORDER == __LITTLE_ENDIAN
/* The intellon VS MME host byte order is the same as 
 * intellon VS MME network byte order, so these functions 
 * are all just identity.  
 */
# define tntohl(x)	(x)
# define tntohs(x)	(x)
# define thtonl(x)	(x)
# define thtons(x)	(x)
#else
# if __BYTE_ORDER == __BIG_ENDIAN
#  define tntohl(x)	__bswap_32 (x)
#  define tntohs(x)	__bswap_16 (x)
#  define thtonl(x)	__bswap_32 (x)
#  define thtons(x)	__bswap_16 (x)
# endif
#endif

#define BYTE_MASK 0xFF
#define NIBBLE_MASK 0x0F
#define BIT_MASK 0x01
 
/*====================================================================*
 *   memory increment/decrement functions;
 *--------------------------------------------------------------------*/
 
signed memincr (uint8_t memory [], size_t length);
signed memdecr (uint8_t memory [], size_t length);
void memswap (uint8_t memory1 [], uint8_t memory2 [], size_t length);

/*====================================================================*
 *   memory validation functions;
 *--------------------------------------------------------------------*/
 
uint32_t checksum32 (const uint32_t memory [], size_t length, uint32_t checksum);
uint16_t checksum16 (const uint16_t memory [], size_t length, uint16_t chesksum);
uint32_t fdchecksum32 (int fd, size_t length, uint32_t checksum);

/*====================================================================*
 *   memory encode functions;
 *--------------------------------------------------------------------*/
 
signed decencode (uint8_t memory [], size_t length, const char *string);
signed hexencode (uint8_t memory [], size_t length, const char *string);
signed binencode (uint8_t memory [], size_t length, const char *string);

/*====================================================================*
 *   memory decode functions;
 *--------------------------------------------------------------------*/
 
size_t decdecode (const uint8_t memory [], size_t length, char buffer [], size_t chars);
size_t hexdecode (const uint8_t memory [], size_t length, char buffer [], size_t chars);
size_t bindecode (const uint8_t memory [], size_t length, char buffer [], size_t chars);

/*====================================================================*
 *   memory input functions;
 *--------------------------------------------------------------------*/
 
size_t hexin (uint8_t memory [], size_t length, FILE *fp);
size_t binin (uint8_t memory [], size_t length, FILE *fp);

/*====================================================================*
 *   memory print functions;
 *--------------------------------------------------------------------*/
 
void hexout (const uint8_t memory [], size_t length, char c, FILE *fp);
void binout (const uint8_t memory [], size_t length, char c, FILE *fp);
void decout (const uint8_t memory [], size_t length, char c, FILE *fp);
void hexdump (const uint8_t memory [], size_t length, FILE *fp);
void fprintm (FILE *fp, const char *format, const uint8_t memory [], size_t length);
void printm (const char *format, const uint8_t memory [], size_t length);

/*====================================================================*
 *   end definitions;
 *--------------------------------------------------------------------*/

#endif
 

