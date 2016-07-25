/*====================================================================*
 *
 *   int ihpapi_UpdateDevice (uint8_t sa [], uint8_t da [], uint8_t type);
 *
 *   ihpapi.h
 *
 *   This sequencing function reads the MAC firmware, PIB, or both.
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
 *
 *--------------------------------------------------------------------*/

#ifndef UPDATEDEVICE_SOURCE
#define UPDATEDEVICE_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"
 
int ihpapi_UpdateDevice (uint8_t sa [], uint8_t da [], ihpapi_hostActionRequest_t type) 

{
	extern struct SeqCB scb;
	TxInfo *tcb = &scb.tcb;
	tcb->txok = false;
	if (_anyset (scb.flags, scbFlag_bsy)) 
	{
		errno = EBUSY;
		return (-1);
	}
	memset (&scb, 0, sizeof (scb));
	switch (type) 
	{
	case IHPAPI_HAR_UPLOAD_FW:
		_setbits (scb.flags, scbFlag_FW);
		tcb->RDMDBLKR.MODULEID = MACSW_MODID;
		tcb->RDMDBLKR.LENGTH = ihtons(sizeof (NVMBlockHeader));
		break;
	case IHPAPI_HAR_UPLOAD_FWnPB:
		_setbits (scb.flags, scbFlag_FW | scbFlag_PB);
		tcb->RDMDBLKR.MODULEID = MACSW_MODID;
		tcb->RDMDBLKR.LENGTH = ihtons(sizeof (NVMBlockHeader));
		break;
	case IHPAPI_HAR_UPLOAD_PB:
		_setbits (scb.flags, scbFlag_PB);
		tcb->RDMDBLKR.MODULEID = PIB_MODID;
		tcb->RDMDBLKR.LENGTH = ihtons(MAX_MODULE_TX_LENGTH);
		break;
	default:
		errno = EINVAL;
		return (-1);
	}
	_setbits (scb.flags, scbFlag_flash);
	scb.opcode = IHPAPI_OPCODE_UPDATE_DEVICE;
	scb.action = type;
	tcb->txok = true;
	memcpy (tcb->ODA, da, IHPAPI_ETHER_ADDR_LEN);
	memcpy (tcb->OSA, sa, IHPAPI_ETHER_ADDR_LEN);
	tcb->MMTYPE = VS_RD_MOD | MMTYPE_REQ;
	tcb->hdrlen = sizeof (tcb->RDMDBLKR);
	tcb->RDMDBLKR.OFFSET = 0;
	return (0);
}

#endif
 

