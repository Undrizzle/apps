/*====================================================================*
 *
 *   int ihp_SequenceWriteModule (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
 *
 *   ihp.h
 *
 *   This function provides the ability to download the MAC firmware, 
 *   PIB, and SDRAM parameters. This function also decodes input buffer a
 *   and prepare VS_WR_MOD MMEs for transmittion. update the structure
 *   named by the caller; 
 *
 *   return 0 on decode success or -1 on decode error;
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
 *
 *--------------------------------------------------------------------*/

#ifndef SEQUENCEWRITEMODULE_SOURCE
#define SEQUENCEWRITEMODULE_SOURCE

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "../ihpapi/ihpapi.h" 
#include "../ihpapi/ihp.h"
#include "../tools/memory.h"
 
int ihp_SequenceWriteModule (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	extern struct SeqCB scb;
	int rc;
	TxInfo *tcb = (TxInfo *)&scb.tcb;
	NVMBlockHeader *fwhdr;
	WrtModBlock wrmodblk;
	bool exit = false;
	result->opCode = scb.opcode;
	result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
	if ( scb.bcb [0] )
	{
		fwhdr = (NVMBlockHeader *)scb.bcb [0]->bstart;
	}
	//rc = ihp_DecodeWriteModule (buffer, length, result, &wrmodblk);
	rc = ihp_DecodeWriteModule (buffer, length, result);
	if (0 == rc) 
	{
		if (0 == result->opStatus.status && 0 != wrmodblk.cnf.LENGTH) 
		{
			result->opCompltCode = IHPAPI_OPCMPLTCODE_TRANSMIT;
			result->validData = false;
			scb.retries = MAX_TX_RETRY_COUNT;
			rc = 0;
			if (beof (wrmodblk.cnf.LENGTH, scb.txbcb)) 
			{
				memset (tcb, 0, sizeof (TxInfo));
				tcb->txok = true;
				memcpy (tcb->ODA, result->OSA, IHPAPI_ETHER_ADDR_LEN);
				memcpy (tcb->OSA, result->ODA, IHPAPI_ETHER_ADDR_LEN);
				if (_anyset (scb.flags, scbFlag_FW)) 
				{
					_clrbits (scb.flags, scbFlag_FW);
					scb.txbcb = scb.bcb [1];
					tcb->MMTYPE = VS_WR_MOD | MMTYPE_REQ;
					tcb->hdrlen = sizeof (tcb->WRMDBLKR);
					tcb->datalen = bgets (&tcb->data, 0, scb.bcb [1]);
					tcb->WRMDBLKR.MODULEID = PIB_MODID;
					tcb->WRMDBLKR.LENGTH = ihtons(tcb->datalen);
					tcb->WRMDBLKR.OFFSET = 0;
					tcb->WRMDBLKR.CHKSUM = ihtonl(checksum32 ((const uint32_t *)tcb->data, (tcb->datalen >> 2), 0));
					exit = true;
				}
				else 
				{
					_clrbits (scb.flags, scbFlag_PB);
				}
				if (!exit) 
				{
					if (_anybits (scb.flags, scbFlag_flash)) 
					{
						tcb->MMTYPE = VS_MOD_NVM | MMTYPE_REQ;
						tcb->hdrlen = sizeof (tcb->FLSHNVMBLK.MODULEID);
						tcb->FLSHNVMBLK.MODULEID = scb.modid | (scb.flags & scbFlag_force);
					}
					else 
					{
						tcb->MMTYPE = VS_ST_MAC | MMTYPE_REQ;
						tcb->hdrlen = sizeof (tcb->STMCBLKR);
						tcb->STMCBLKR.MODULEID = MACSW_MODID;
						tcb->STMCBLKR.IMAGELOAD = fwhdr->IMAGEADDR;
						tcb->STMCBLKR.IMAGELENGTH = fwhdr->IMAGELENGTH;
						tcb->STMCBLKR.IMAGECHKSUM = fwhdr->IMAGECHECKSUM;
						tcb->STMCBLKR.IMAGESTART = fwhdr->ENTRYPOINT;
					}
					bclose (scb.bcb [1]);
					bclose (scb.bcb [0]);
				}
			}
			else 
			{
				tcb->txok = true;
				tcb->MMTYPE = VS_WR_MOD | MMTYPE_REQ;
				tcb->hdrlen = sizeof (tcb->WRMDBLKR);
				tcb->datalen = bgetnexts (&tcb->data, scb.txbcb);
				tcb->WRMDBLKR.MODULEID = wrmodblk.cnf.MODULEID;
				tcb->WRMDBLKR.LENGTH = ihtons(tcb->datalen);
				tcb->WRMDBLKR.OFFSET = bgeto (scb.txbcb);
				tcb->WRMDBLKR.CHKSUM = ihtonl(checksum32 ((const uint32_t *)tcb->data, (tcb->datalen >> 2), 0));
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
				errno = EINVAL;
				rc = -1;
			}
		}
	}
	return (rc);
}

#endif
 

