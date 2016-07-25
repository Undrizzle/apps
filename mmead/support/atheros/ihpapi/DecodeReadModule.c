/*====================================================================*
 *
 *   int ihp_DecodeReadModule (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
 *
 *   ihp.h
 *
 *   decode buffer as a VS_RD_MOD Confirm MME; update structures
 *   named by the caller; 
 *
 *   return 0 on decode success or -1 on decode failure;
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
 *	Charles Maier, charles.maier@intellon.com
 *	Alex Vasquez, alex.vasquez@intellon.com
 *
 *--------------------------------------------------------------------*/

#ifndef DECODEREADMODULE_SOURCE
#define DECODEREADMODULE_SOURCE

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "../ihpapi/ihp.h"
#include "../tools/memory.h"
 
int ihp_DecodeReadModule (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	//extern struct SeqCB scb;
	//size_t size;
	struct __packed vs_rd_mod_cnf 
	{
		struct header_vs header;
		uint8_t MSTATUS;
		uint8_t RES1 [3];
		uint8_t MODULEID;
		uint8_t RESERVED;
		uint16_t LENGTH;
		uint32_t OFFSET;
		uint32_t CHKSUM;
		uint8_t DATA [1];
	}
	* confirm = (struct vs_rd_mod_cnf *)(buffer);

#if INTELLON_SAFEMODE
 
	if (length < sizeof (struct vs_rd_mod_cnf)) 
	{
		errno = EFAULT;
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		errno = EFAULT;
		return (-1);
	}

#endif
 
	result->validData = false;
	result->opStatus.status = confirm->MSTATUS;
	result->data.updateDev.type = confirm->MODULEID;
	if (0 == result->opStatus.status) 
	{
		if (checksum32 ((uint32_t *)confirm->DATA, (size_t)intohs((confirm->LENGTH) >> 2), intohl(confirm->CHKSUM))) 
		{
			result->opStatus.status = errno = EINVAL;
		}
		else if (MAX_MODULE_TX_LENGTH < intohs(confirm->LENGTH)) 
		{
			result->opStatus.status = errno = ERANGE;
		}
		else 
		{
			result->data.updateDev.bufferLen = intohs(confirm->LENGTH);
			memcpy (result->data.updateDev.buffer, confirm->DATA, intohs(confirm->LENGTH));
			result->validData = true;
			result->dataLen = sizeof (ihpapi_updateDeviceData_t) - sizeof (result->data.updateDev.buffer) + result->data.updateDev.bufferLen;

			/*
			 *	calculate end of module
			 */
 			#if 0
			if (confirm->MODULEID == PIB_MODID && intohl(confirm->OFFSET) == 0) 
			{
				PIBVersionHeader *pibhdr = (PIBVersionHeader *)&confirm->DATA [0];
				scb.moduleLen = intohs(pibhdr->PIBLength);
				scb.offset = 0;
			}
			if (confirm->MODULEID == MACSW_MODID && intohl(confirm->OFFSET) == scb.offset) 
			{
				NVMBlockHeader *nvmhdr = (NVMBlockHeader *)&confirm->DATA [0];
				if (checksum32 ((const uint32_t *)(nvmhdr), sizeof (NVMBlockHeader) >> 2, 0)) 
				{
					errno = EFAULT;
					return (-1);
				}
				scb.moduleLen += intohl(nvmhdr->IMAGELENGTH) + sizeof (NVMBlockHeader);
				scb.offset = intohl(nvmhdr->NEXTHEADER);
			}
			size = intohl(confirm->OFFSET) + intohs(confirm->LENGTH);
			if (!scb.offset && (size >= scb.moduleLen)) 
			{
				result->data.updateDev.eof = true;
			}
			else 
			{
				result->data.updateDev.eof = false;
			}
			#endif
		}
	}
	return (result->opStatus.status);
}

#endif
 

