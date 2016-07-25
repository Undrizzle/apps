/*====================================================================*
 *
 *   size_t EncodeIntellonHeader (uint8_t buffer [], signed length, uint16_t MMTYPE);
 *
 *   ihp.h
 *
 *   Encode buffer with an Intellon vendor specific message header 
 *   having HomePlug message protocol version (INTELLON_MMV) and 
 *   Intellon message type (MMTYPE);
 *
 *   return the number of bytes actually encoded or 0 on encode error; 
 *   the error code is stored in errno;
 *
 *   this function is called by all Intellon vendor specific message
 *   encoding functions; message content MUST BE SENT LITTLE ENDIAN;
 *
 *   see the INT6000 Firmware Technical Reference 
 *   Manual for information about MME headers and message types; the
 *   Intellon OUI is implicit in this function;
 *
 *   this function only encodes the header; see EncodeIntellonPacket
 *   to encode the entire message packet;
 *
 *   MMV is the version number of the MME command set; currently, 
 *   there is only one command set for Intellon MMEs; 
 *
 *   MMTYPE indicates the desired Intellon device operation taken
 *   from the TRM; some operations are undocumented and should not
 *   be used;
 *
 *   OUI is the Organizationally Unique Identifier resgistered with
 *   the IEEE by the vendor and is a constant for Intellon Devices; 
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

#ifndef ENCODEINTELLONHEADER_SOURCE
#define ENCODEINTELLONHEADER_SOURCE

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "../ihpapi/ihp.h"
#include "../tools/types.h"
 
size_t EncodeIntellonHeader (uint8_t buffer [], signed length, uint16_t MMTYPE) 

{
	struct header_mme * header = (struct header_mme *)(buffer);

#if INTELLON_SAFEMODE
 
	if (buffer == (uint8_t *)(0)) 
	{
		errno = EFAULT;
		return (0);
	}

#endif
 
	if (length < sizeof (struct header_mme)) 
	{
#if INTELLON_SAFEMODE
 
		memset (buffer, 0, length);

#endif
 
		errno = ERANGE;
		return (0);
	}
	header->MMV = INTELLON_MMV;
	header->MMTYPE = ihtons(MMTYPE);
	header->OUI [0] = 0x00;
	header->OUI [1] = 0xB0;
	header->OUI [2] = 0x52;
	return (sizeof (struct header_mme));
}

//add by stan for 74 mmv=1 encoder intellon header
size_t EncodeV1IntellonHeader (uint8_t buffer [], signed length, uint16_t MMTYPE) 

{
	struct header_v1_mme * header = (struct header_v1_mme *)(buffer);

#if INTELLON_SAFEMODE
 
	if (buffer == (uint8_t *)(0)) 
	{
		errno = EFAULT;
		return (0);
	}

#endif
 
	if (length < sizeof (struct header_v1_mme)) 
	{
#if INTELLON_SAFEMODE
 
		memset (buffer, 0, length);

#endif
 
		errno = ERANGE;
		return (0);
	}
	header->MMV = 0x01;
	header->MMTYPE = ihtons(MMTYPE);
	header->FMI [0] = 0x00;
	header->FMI [1] = 0x00;
	header->OUI [0] = 0x00;
	header->OUI [1] = 0xB0;
	header->OUI [2] = 0x52;
	return (sizeof (struct header_v1_mme));
}

#endif
 

