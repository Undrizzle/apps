/*====================================================================*
 *
 *   int16_t DecodeEthernetHeader (const uint8_t buffer [], size_t length,  struct header_eth * address);
 *
 *   decode buffer as an Ethernet header and the validate MTYPE; 
 *   return -1 on decode error or 0 for success; a non-HomePlug
 *   frame is treated as an error; 
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

#ifndef DECODEETHERNETHEADER_SOURCE
#define DECODEETHERNETHEADER_SOURCE

#include <stdint.h>
#include <string.h>

#include "../ihpapi/ihp.h"
 
int16_t DecodeEthernetHeader (const uint8_t buffer [], size_t length, struct header_eth * address) 

{
	struct header_eth * header = (struct header_eth *)(buffer);

#if INTELLON_SAFEMODE
 
	if (buffer == (uint8_t *)(0)) 
	{
		return (-1);
	}

#endif
 
	if (address == (struct header_eth *)(0)) 
	{
		return (-1);
	}
	if (ntohs (header->MTYPE) != HOMEPLUG_MTYPE) 
	{
		return (-1);
	}
	memcpy (address->ODA, header->ODA, sizeof (address->ODA));
	memcpy (address->OSA, header->ODA, sizeof (address->OSA));
	address->MTYPE = header->MTYPE;
	return (0);
}

#endif
 

