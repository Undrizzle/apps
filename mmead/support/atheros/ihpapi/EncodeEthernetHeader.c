/*====================================================================*
 *
 *   size_t EncodeEthernetHeader (uint8_t buffer[], signed length, uint8_t ODA [], uint8_t OSA []);
 *
 *   ihp.h
 *
 *   encode buffer with a standard Ethernet header having a specified
 *   source address (OSA) and destination address (ODA) address and a
 *   HomePlug AV ethertype (HOMEPLUG_MTYPE);
 *
 *   return the number of bytes encoded or 0 on encode error; set
 *   errno to the appropriate POSIX error number;
 *
 *   OSA is the hardware (MAC) address of the source device;
 *
 *   ODA is the hardware (MAC) address of the desintation device;
 *
 *   MTYPE is the HomePlug AV Ethertype;
 *
 *   There is no need to flush the header since this function writes
 *   to all locations unless there is an error; the caller may elect
 *   to flush the buffer before calling this function;
 *
 *   This software and documentation is the property of Intellon 
 *   Corporation, Ocala, Florida. It is provided 'as is' without 
 *   expressed or implied warranty of any kind to anyone for any 
 *   reason. Intellon assumes no responsibility or liability for 
 *   errors or omissions in the software or documentation and 
 *   reserves the right to make changes without notification. 
 *   
 *   Intellon customers may modify and distribute the software 
 *   without obligation to Intellon. Since use of this software 
 *   is optional, users shall bear sole responsibility and 
 *   liability for any consequences of it's use. 
 *
 *.  Intellon HomePlug AV Application Programming Interface;
 *:  Published 2007 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use;
 *
 *   Contributor(s): 
 *	Charles Maier, charles.maier@intellon.com
 *	Alex Vasquez, alex.vasquez@intellon.com
 *
 *--------------------------------------------------------------------*/

#ifndef ENCODEETHERNETHEADER_SOURCE
#define ENCODEETHERNETHEADER_SOURCE

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "../ihpapi/ihp.h"
#include "../tools/types.h"
 
size_t EncodeEthernetHeader (uint8_t buffer [], signed length, uint8_t ODA [], uint8_t OSA []) 

{
	struct header_eth *header = (struct header_eth *)(buffer);

#if INTELLON_SAFEMODE
 
	if (buffer == (uint8_t *)(0)) 
	{
		errno = EFAULT;
		return (0);
	}
	if (ODA == (uint8_t *)(0)) 
	{
		errno = EFAULT;
		return (0);
	}
	if (OSA == (uint8_t *)(0)) 
	{
		errno = EFAULT;
		return (0);
	}

#endif
 
	if (length < sizeof (struct header_eth)) 
	{
#if INTELLON_SAFEMODE
 
		memset (buffer, 0, length);

#endif
 
		errno = ERANGE;
		return (0);
	}
	memcpy (header->ODA, ODA, sizeof (header->ODA));
	memcpy (header->OSA, OSA, sizeof (header->OSA));
	header->MTYPE = htons (HOMEPLUG_MTYPE);
	return (sizeof (struct header_eth));
}

#endif
 

