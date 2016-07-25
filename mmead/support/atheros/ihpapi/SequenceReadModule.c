/*====================================================================*
 *
 *   int ihp_sequenceReadModule (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
 *
 *   ihp.h
 *
 *   Contributor(s): 
 *	Alex Vasquez, alex.vasquez@intellon.com
 *	Charles Maier, charles.maier@intellon.com
 *
 *   sequence buffer as a VS_RD_MOD Confirm MME; update structures
 *   named by the caller; 
 *
 *   return 0 on sequence success or -1 on sequence failure;
 *
 *   See the INT6000 Firmware Technical Reference Manual
 *   for more information;
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

#ifndef SEQUENCEREADMODULE_SOURCE
#define SEQUENCEREADMODULE_SOURCE

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "../ihpapi/ihpapi.h" 
#include "../ihpapi/ihp.h"
#include "../tools/memory.h"
 
int ihp_SequenceReadModule (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	extern struct SeqCB scb;
	int rc = -1;
	TxInfo * tcb = &scb.tcb;
	result->opCode =scb.opcode;
	result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
	if (0 == (rc = ihp_DecodeReadModule (buffer, length, result))) 
	{
		if (0 == result->opStatus.status) 
		{
			result->opCompltCode = IHPAPI_OPCMPLTCODE_TRANSMIT;
			scb.retries = MAX_TX_RETRY_COUNT;
			tcb->data = 0;
			tcb->datalen = 0;
			rc = 0;
			if (result->data.updateDev.eof) 
			{
				memset (tcb, 0, sizeof (TxInfo));
				memcpy (tcb->ODA, result->OSA, IHPAPI_ETHER_ADDR_LEN);
				memcpy (tcb->OSA, result->ODA, IHPAPI_ETHER_ADDR_LEN);
				switch (scb.action) 
				{
				case IHPAPI_HAR_UPLOAD_FWnPB:
					if (_anyset (scb.flags, scbFlag_FW)) 
					{
						_clrbits (scb.flags, scbFlag_FW);
						tcb->txok = true;
						tcb->MMTYPE = VS_RD_MOD | MMTYPE_REQ;
						tcb->hdrlen = sizeof (tcb->RDMDBLKR);
						tcb->RDMDBLKR.MODULEID = PIB_MODID;
						tcb->RDMDBLKR.LENGTH = ihtons(MAX_MODULE_TX_LENGTH);
						tcb->RDMDBLKR.OFFSET = 0;
					}
					else 
					{
						result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
					}
					break;
				case IHPAPI_HAR_UPLOAD_PB:
				case IHPAPI_HAR_UPLOAD_FW:
					_clrbits (scb.flags, scbFlag_FW | scbFlag_PB);
					result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
					break;
				default:
					result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
					_clrbits (scb.flags, scbFlag_FW | scbFlag_PB);
					result->opStatus.status = errno = EINVAL;
					rc = -1;
				}
			}
			else 
			{
				tcb->txok = true;
				tcb->RDMDBLKR.OFFSET = ihtonl(intohl(tcb->RDMDBLKR.OFFSET) + intohs(tcb->RDMDBLKR.LENGTH));
				if (_anyset (scb.flags, scbFlag_FW) && intohl(tcb->RDMDBLKR.OFFSET) == scb.offset) 
				{
					tcb->RDMDBLKR.LENGTH = ihtons(sizeof (NVMBlockHeader));
				}
				else 
				{
					if ((intohl(tcb->RDMDBLKR.OFFSET) + MAX_MODULE_TX_LENGTH) > scb.moduleLen) 
					{
						tcb->RDMDBLKR.LENGTH = ihtons(scb.moduleLen - intohl(tcb->RDMDBLKR.OFFSET));
					}
					else 
					{
						tcb->RDMDBLKR.LENGTH = ihtons(MAX_MODULE_TX_LENGTH);
					}
				}
			}
		}
		else 
		{
			if (scb.retries--) 
			{
				tcb->txok = true;
				result->opCompltCode = IHPAPI_OPCMPLTCODE_TRANSMIT;
				rc = 0;
			}
			else 
			{
				result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
				_clrbits (scb.flags, scbFlag_FW | scbFlag_PB);
			}
		}
	}
	return (rc);
}

#endif
 

