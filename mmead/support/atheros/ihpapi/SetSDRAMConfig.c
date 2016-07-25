/*====================================================================*
 *
 *   size_t ihpapi_SetSDRAMConfig (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], size_t SDRAM_len, uint8_t SDRAM_pbuffer []);
 *
 *   ihpapi.h
 *
 *   Encode buffer as an Intellon VS_SET_SDRAM Management Message and
 *   return the number of bytes encoded or 0 on encoding error; the
 *   error code is stored in errno and returned in status;
 *
 *   The particular message occupies exactly IHPAPI_ETHER_MIN_LEN bytes and
 *   so there is no need to pad the buffer; 
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

#ifndef SETSDRAMCONFIG_SOURCE
#define SETSDRAMCONFIG_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"

#include "../tools/memory.h"

size_t ihpapi_SetSDRAMConfig (uint8_t sa [], uint8_t da [], size_t bufferLen, uint8_t buffer [], size_t SDRAM_len, uint8_t SDRAM_buffer []) 

{
	struct __packed vs_set_sdram_req 
	{
		struct header_vs header;
		uint8_t SDRAMCONFIG [SDRAM_MAX_LEN];
		uint32_t CHECKSUM;
	}
	* request = (struct vs_set_sdram_req *)(buffer);
	int isLegacy;
	size_t offset = 0;
	extern struct SeqCB scb;
	NVMBlockHeader *hdr;
	TxInfo *tcb = &scb.tcb;
	tcb->txok = false;

#if INTELLON_SAFEMODE
 
	if (bufferLen < IHPAPI_ETHER_MIN_LEN) 
	{
		errno = ERANGE;
		return (0);
	}
	if (SDRAM_buffer == 0) 
	{
		errno = EINVAL;
		return (0);
	}

#endif

/* Determine Config file type. */
	if (SDRAM_len <= SDRAM_MAX_LEN)
	{
		if (SDRAM_len != SDRAM_MAX_LEN)
		{
			errno = ERANGE;
			return (0);
		}

		/* SDRAM Config file */
 
		offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, da, sa);
		offset += EncodeIntellonHeader (buffer + offset, bufferLen - offset, (VS_SET_SDRAM | MMTYPE_REQ));
		if (offset < sizeof (struct header_vs)) 
		{
			return (0);
		}
		memcpy (request->SDRAMCONFIG, SDRAM_buffer, SDRAM_len);
		request->CHECKSUM = ihtonl(checksum32 ((const uint32_t *)SDRAM_buffer, (SDRAM_len >> 2), 0));
		offset += SDRAM_len + sizeof (request->CHECKSUM);
		if (offset < IHPAPI_ETHER_MIN_LEN) 
		{
			memset (buffer + offset, 0, IHPAPI_ETHER_MIN_LEN - offset);
			offset = IHPAPI_ETHER_MIN_LEN;
		}
	}
	else
	{
		/* SDRAM Configuration may be part of the NVM file - verify it!  */
		/* Precondition: Blank or no-flash only operation  */
		if (_anyset (scb.flags, scbFlag_bsy)) 
		{
			errno = EBUSY;
			return (0);
		}
		if ( SDRAM_len < sizeof (NVMBlockHeader))
		{
			errno = ERANGE;
			return (0);
		}
		isLegacy = ihp_isLegacyNVM(SDRAM_buffer, SDRAM_len);
		if ( -1 == isLegacy )
		{
			return (0);
		}
		if ( 1 == isLegacy )
		{
			errno = EINVAL;
			return (0);
		}
		/* At this point a valid NVM file. Find the memory control object! */
		hdr = ihp_FindNVMObject ( SDRAM_buffer, SDRAM_len, nvmBlockHeaderEntryType_Memctl );
		if (hdr == 0) 
		{
			return (0);
		}
		scb.flags = NOINT6K_NOFW;
		_clrbits (scb.flags, ~scbFlag_options);
		scb.opcode = IHPAPI_OPCODE_SET_SDRAM_CONFIG;
		tcb->txok = true;
		memcpy (tcb->ODA, da, IHPAPI_ETHER_ADDR_LEN);
		memcpy (tcb->OSA, sa, IHPAPI_ETHER_ADDR_LEN);
		scb.pibmemoffset = 0;
		scb.bcb [0] = bopen ((uint8_t *)hdr + sizeof (NVMBlockHeader), ihtonl(hdr->IMAGELENGTH), MAX_MODULE_TX_LENGTH);
		if (scb.bcb [0] == (buffer_t *)(0)) 
		{
			return (0);
		}
		_setbits (scb.flags, scbFlag_MCTL);
		scb.modid = MACSW_MODID;
		scb.txbcb = scb.bcb [0];
		tcb->MMTYPE = VS_WR_MEM | MMTYPE_REQ;
		tcb->hdrlen = sizeof (tcb->WRMMBLKR);
		tcb->datalen = bgets (&tcb->data, 0, scb.bcb [0]);
		tcb->WRMMBLKR.WADDRESS = ihtonl(hdr->IMAGEADDR);
		tcb->WRMMBLKR.WLENGTH = ihtonl(tcb->datalen);

		/* Ready to start sequence: Prepare first packet! */
		offset = ihpapi_TxFrame (bufferLen, buffer);
		if (0 == offset)
		{
			bclose(scb.bcb [0]);
		}
	}

	return (offset);
}


int ihp_isLegacyNVM (const uint8_t FW_pbuffer [], size_t length) 

{
	NVMBlockHeader * header;
	int isLegacyNVM = 1;

	for (header = (NVMBlockHeader *)(FW_pbuffer); (uint8_t *)(header) < (FW_pbuffer + length); header = (NVMBlockHeader *)(FW_pbuffer + intohl(header->NEXTHEADER))) 
	{
		if (checksum32 ((const uint32_t *)(header), sizeof (NVMBlockHeader) >> 2, 0)) 
		{
			errno = EINVAL;
			return (-1);
		}
		if (header->NVMHEADERMINORVERSION != nvmBlockHeaderMinorVersion_Legacy) 
		{
			isLegacyNVM = 0;
		}
		if (!header->NEXTHEADER)
		{
			break;
		}
	}

	return (isLegacyNVM);
}

NVMBlockHeader *ihp_FindNVMObject (const uint8_t FW_pbuffer [], size_t length, uint8_t oType) 

{
	NVMBlockHeader * header;
	for (header = (NVMBlockHeader *)(FW_pbuffer); (uint8_t *)(header) < (FW_pbuffer + length); header = (NVMBlockHeader *)(FW_pbuffer + intohl(header->NEXTHEADER))) 
	{
		if (checksum32 ((const uint32_t *)(header), sizeof (NVMBlockHeader) >> 2, 0)) 
		{
			errno = EINVAL;
			return (0);
		}
		if (nvmBlockHeaderMinorVersion_Legacy != header->NVMHEADERMINORVERSION && header->ENTRYTYPE == oType) 
		{
			return (header);
		}
		if (!header->NEXTHEADER)
		{
			break;
		}
	}
	errno = EFAULT;
	return (0);
}

#endif
 

