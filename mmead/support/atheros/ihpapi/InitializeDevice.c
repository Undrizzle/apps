/*====================================================================*
 *
 *   int ihpapi_InitializeDevice (uint8_t sa [], uint8_t da [],size_t FW_len, uint8_t FW_pbuffer [], size_t PIB_len, uint8_t PIB_pbuffer [], unsigned options);
 *
 *   ihpapi.h
 *
 *   This function provides the ability to download the MAC firmware,
 *   PIB, or the SDRAM parameters (INT6300) as a result of a Host Action
 *   Request VS_HOST_ACTION, starting a Writing Sequence.
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
 *	Alex Vasquez, alex.vasquez@intellon.com
 *	Charles Maier, charles.maier@intellon.com
 *	Nathan Houghton, nathan.houghton@intellon.com
 *
 *--------------------------------------------------------------------*/

#ifndef INITIALIZEDEVICE_SOURCE
#define INITIALIZEDEVICE_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"

#include "../tools/memory.h"
 
int ihpapi_InitializeDevice (uint8_t sa [], uint8_t da [], size_t FW_len, uint8_t FW_pbuffer [], size_t PIB_len, uint8_t PIB_pbuffer [], unsigned options) 

{
	extern struct SeqCB scb;
	NVMBlockHeader *hdr;
	TxInfo *tcb = &scb.tcb;
	tcb->txok = false;

#if INTELLON_SAFEMODE
 
	if (PIB_pbuffer == (uint8_t *)(0)) 
	{
		errno = EFAULT;
		return (-1);
	}

#endif
 
	if (_anyset (scb.flags, scbFlag_bsy)) 
	{
		errno = EBUSY;
		return (-1);
	}
	memset (&scb, 0, sizeof (scb));
	scb.flags = options;
	_clrbits (scb.flags, ~scbFlag_options);
	scb.opcode = IHPAPI_OPCODE_INITIALIZE_DEVICE;
	tcb->txok = true;
	memcpy (tcb->ODA, da, IHPAPI_ETHER_ADDR_LEN);
	memcpy (tcb->OSA, sa, IHPAPI_ETHER_ADDR_LEN);
	_setbits (scb.flags, scbFlag_PB);
	scb.bcb [1] = bopen (PIB_pbuffer, PIB_len, MAX_MODULE_TX_LENGTH);
	if (scb.bcb [1] == (buffer_t *)(0)) 
	{
		return (-1);
	}
	scb.modid = PIB_MODID;
	if (_anybits (scb.flags, scbFlag_is6k) || _allbits (scb.flags, scbFlag_noINT6Kfwflash)) 
	{
/*! At this point:
 *  Could be a Host Action Required)
 *  Firmware and PIB only -> VS_WR_MOD
 *  No trailer in the tcb required.
 */
		if (FW_pbuffer)
		{
			scb.bcb [0] = bopen (FW_pbuffer, FW_len, MAX_MODULE_TX_LENGTH);
			if (scb.bcb [0] == (buffer_t *)(0)) 
			{
				bclose (scb.bcb [1]);
				return (-1);
			}
			_setbits (scb.flags, scbFlag_FW);
			scb.txbcb = scb.bcb [0];
			scb.modid |= MACSW_MODID;
			tcb->WRMDBLKR.MODULEID = MACSW_MODID;
			tcb->datalen = bgets (&tcb->data, 0, scb.bcb [0]);
		}
		else
		{
			scb.txbcb = scb.bcb [1];
			tcb->WRMDBLKR.MODULEID = PIB_MODID;
			tcb->datalen = bgets (&tcb->data, 0, scb.bcb [1]);
		}
		tcb->MMTYPE = VS_WR_MOD | MMTYPE_REQ;
		tcb->hdrlen = sizeof (tcb->WRMDBLKR);
		tcb->WRMDBLKR.LENGTH = ihtons(tcb->datalen);
		tcb->WRMDBLKR.OFFSET = 0;
		tcb->WRMDBLKR.CHKSUM = ihtonl(checksum32 ((const uint32_t *)tcb->data, (tcb->datalen >> 2), 0));
	}
	else 
	{
/*! At this point:
 * Host Action Required 
 * Device Type -> Not a INT6000
 * Blank Flash or No Flash 
 * The Firmware and the PIB -> VS_WR_MEM
 * No trailer in the tcb required
 */
 
/*! Initialize the bcb for the firmware:
 * need to search to find the firmware header from the nvm file. The firmware
 * is the last entry on the nvm file, i.e. NEXTHEADER is equal to 0.;
 */

		if (FW_pbuffer == (uint8_t *)(0)) 
		{
			bclose (scb.bcb [1]);
			errno = EFAULT;
			return (-1);
		} 
		hdr = ihp_FindFWHeader (FW_pbuffer, FW_len);
		if (hdr == 0) 
		{
			bclose (scb.bcb [1]);
			return (-1);
		}
		if (htons(*(uint16_t *)PIB_pbuffer) < 0x0305)
		{
			scb.pibmemoffset = PIB_LEGACY_ADDRESS;
		}
		else
		{
			scb.pibmemoffset = PIB_MEMORY_ADDRESS;
		}
		scb.bcb [0] = bopen ((uint8_t *)hdr + sizeof (NVMBlockHeader), ihtonl(hdr->IMAGELENGTH), MAX_MODULE_TX_LENGTH);
		if (scb.bcb [0] == (buffer_t *)(0)) 
		{
			bclose (scb.bcb [1]);
			return (-1);
		}
		_setbits (scb.flags, scbFlag_FW);
		scb.modid |= MACSW_MODID;
		scb.txbcb = scb.bcb [0];
		tcb->MMTYPE = VS_WR_MEM | MMTYPE_REQ;
		tcb->hdrlen = sizeof (tcb->WRMMBLKR);
		tcb->datalen = bgets (&tcb->data, 0, scb.bcb [0]);
		tcb->WRMMBLKR.WADDRESS = ihtonl(hdr->IMAGEADDR);
		tcb->WRMMBLKR.WLENGTH = ihtonl(tcb->datalen);
	}
	return (0);
}

#endif
 

