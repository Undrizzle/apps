/*====================================================================*
 *   
 *   int ihpapi_RxFrame (size_t length, uint8_t buffer [], ihpapi_result_t result);
 *
 *   ihpapi.h
 *
 *   This function decodes recived frames and provides the results to the application.
 *   The information provided in the results structure allows the application to
 *   futher process data or allows the API to continue sequencing an operation in
 *   progress.
 *
 *   EFAULT if buffer/result address or size are invalid or illegal;
 *   message version or vendor identifier is wrong and so decoding cannot 
 *   be trusted;
 *
 *   EPERM means the frame is HomePlug protocol and is not
 *   supported;
 *
 *   ENOSYS means unrecognized MME function or type;
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

#ifndef FRAMERECEIVED_SOURCE
#define FRAMERECEIVED_SOURCE

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "../tools/memory.h"
#include "ihpapi.h"
#include "ihp.h"
 
char ihpapi_version [] = IHPAPI_VERSION;

int ihpapi_RxFrame (size_t length, uint8_t buffer [], ihpapi_result_t * result) 

{
	extern struct SeqCB scb;
	int rc = -1;
	uint8_t OUI [3] = 
	{
		0x00,
		0xB0,
		0x52 
	};
	TxInfo *tcb = &scb.tcb;
	BlockInfo block;
	struct header_cnf * header = (struct header_cnf *)(buffer);
	vs_get_property_cnf_header_t *pget_property_cnf_header = (vs_get_property_cnf_header_t *)buffer;
	vs_set_property_cnf_t *pset_property_cnf = (vs_set_property_cnf_t *)buffer;
	vs_hg_cnf_header_t *hg_cnf = (vs_hg_cnf_header_t *)buffer;
	memset (result, 0, sizeof (* result));
	result->opCode = IHPAPI_OPCODE_NOOP;
	result->opCompltCode = IHPAPI_OPCMPLTCODE_NOOP;
	result->opStatus.type = XX_MMTYPE_BAD;
	result->opStatus.status = 0;
	result->validData = false;
	result->dataLen = 0;

#if INTELLON_SAFEMODE
 
	if (buffer == (uint8_t *)(0)) 
	{
		result->opStatus.status = errno = EFAULT;
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		result->opStatus.status = errno = EFAULT;
		return (-1);
	}

#endif
 
	if (ntohs (header->ethernet.MTYPE) != HOMEPLUG_MTYPE) 
	{
		result->opStatus.status = errno = EPERM;
		return (-1);
	}
	if (header->intellon.MMV != INTELLON_MMV) 
	{
		result->opStatus.status = errno = EFAULT;
		return (-1);
	}
	if (memcmp (header->intellon.OUI, OUI, sizeof (OUI))) 
	{
		result->opStatus.status = errno = EFAULT;
		return (-1);
	}
	result->opStatus.type = intohs(header->intellon.MMTYPE);
	memcpy (result->ODA, header->ethernet.ODA, IHPAPI_ETHER_ADDR_LEN);
	memcpy (result->OSA, header->ethernet.OSA, IHPAPI_ETHER_ADDR_LEN);
	switch (result->opStatus.type) 
	{
	case VS_HST_ACTION | MMTYPE_IND:
		rc = ihp_DecodeHostAction (buffer, length, result);
		memset (tcb, 0, sizeof (TxInfo));
		tcb->txok = true;
		memcpy (tcb->ODA, result->OSA, IHPAPI_ETHER_ADDR_LEN);
		memcpy (tcb->OSA, result->ODA, IHPAPI_ETHER_ADDR_LEN);
		if (!rc) 
		{
			tcb->HARRSPBLK.STATUS = 0x00;
		}
		else 
		{
			tcb->HARRSPBLK.STATUS = 0x01;
		}
		tcb->MMTYPE = VS_HST_ACTION | MMTYPE_RSP;
		tcb->hdrlen = sizeof (HARRspBlock);
		result->opCode = IHPAPI_OPCODE_HOST_ACTION_REQUEST;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_TRANSMIT;
		break;
	case VS_SW_VER | MMTYPE_CNF:
		rc = ihp_DecodeGetVersion (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_GET_VERSION_INFO;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	case VS_RS_DEV | MMTYPE_CNF:
		rc = ihp_DecodeResetDevice (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_RESET_DEVICE;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		memset (&scb, 0, sizeof (scb));
		break;
	case VS_FAC_DEFAULT | MMTYPE_CNF:
		rc = ihp_DecodeResetToFactoryDefaults (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_RESET_TO_FACTORY_DEFAULTS;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		memset (&scb, 0, sizeof (scb));
		break;
	case VS_SET_KEY | MMTYPE_CNF:
		rc = ihp_DecodeSetKey (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_SET_KEY;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	case VS_MFG_STRING | MMTYPE_CNF:
		rc = ihp_DecodeMfgString (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_GET_MANUFACTURER_INFO;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	case VS_LNK_STATS | MMTYPE_CNF:
		rc = ihp_DecodeLinkStats (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_GET_CONNECTION_INFO;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	case VS_NW_INFO | MMTYPE_CNF:
		rc = ihp_DecodeNetworkInfo (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_GET_NETWORK_INFO;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	case VS_TONE_MAP_CHAR | MMTYPE_CNF:
		rc = ihp_DecodeGetToneMapInfo (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_GET_TONE_MAP_INFO;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	case VS_RX_TONE_MAP_CHAR | MMTYPE_CNF:
		rc = ihp_DecodeGetRxToneMapInfo (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_GET_RX_TONE_MAP_INFO;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	case VS_ENET_SETTINGS | MMTYPE_CNF:
		rc = ihp_DecodeEthernetPHYSettings (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_MANAGE_ENET_PHY;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	case VS_SET_SDRAM | MMTYPE_CNF:
		rc = ihp_DecodeSetSDRAMCfg (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_SET_SDRAM_CONFIG;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	case VS_WR_MEM | MMTYPE_CNF:
		rc = ihp_SequenceWriteMemory (buffer, length, result);
		break;
	case VS_WR_MOD | MMTYPE_CNF:
		//rc = ihp_SequenceWriteModule (buffer, length, result);
		rc = ihp_DecodeWriteModule (buffer, length, result);
		break;
	case VS_RD_MOD | MMTYPE_CNF:
		//rc = ihp_SequenceReadModule (buffer, length, result);
		rc = ihp_DecodeReadModule (buffer, length, result);
		break;
	case VS_MOD_NVM | MMTYPE_CNF:
		rc = ihp_DecodeFlashNVM (buffer, length, result, &block);
		result->opCode = scb.opcode;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		memset (&scb, 0, sizeof (scb));
		break;
	case VS_ST_MAC | MMTYPE_CNF:
		rc = ihp_DecodeStartMAC (buffer, length, result, &block);
		result->opCode = scb.opcode;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		memset (&scb, 0, sizeof (scb));
		break;
	case VS_CLASSIFICATION | MMTYPE_CNF:
		rc = ihp_DecodeSetClassification (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_SET_CLASSIFICATION_INFO;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	case VS_MDIO_COMMAND | MMTYPE_CNF:
		rc = ihp_DecodeMdioInfo (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_MDIO_COMMAND_INFO;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	case VS_MODULE_OPERATION | MMTYPE_CNF:
		rc = 0;
		result->validData = true;
		break;
	case VS_HOME_GATEWAY_OPERATION | MMTYPE_CNF:
		if(hg_cnf->action == 0x02)
		{
			if(0x1001 == intohs(hg_cnf->extend_SubType))
			{
				rc = ihp_DecodeGetHgManage(buffer, length, result);
			}

			if(0x1002 == intohs(hg_cnf->extend_SubType))
			{
				rc = ihp_DecodeGetHgBusiness(buffer, length, result);
			}
			if(0x3013 == intohs(hg_cnf->extend_SubType))
			{
				rc = ihp_DecodeGetHgSsidStatus(buffer, length, result);
			}
			if(0x1004 == intohs(hg_cnf->extend_SubType))
			{
				rc = ihp_DecodeGetHgWanStatus(buffer, length, result);
			}
			if(0x2004 == intohs(hg_cnf->extend_SubType))
			{
				rc = ihp_DecodeGetHgWifiMode(buffer, length, result);
			}
		}
		if(hg_cnf->action == 0x01)
		{
			if(0x1001 == intohs(hg_cnf->extend_SubType))
			{
				rc = ihp_DecodeSetHgManage(buffer, length, result);
			}

			if(0x1002 == intohs(hg_cnf->extend_SubType))
			{
				rc = ihp_DecodeSetHgBusiness(buffer, length, result);
			}
			if(0x3013 == intohs(hg_cnf->extend_SubType))
			{
				rc = ihp_DecodeSetHgSsidStatus(buffer, length, result);
			}
			if(0x2004 == intohs(hg_cnf->extend_SubType))
			{
				rc = ihp_DecodeSetHgWifiMode(buffer, length, result);
			}
		}
		break;
	case VS_SLAVE_MEM | MMTYPE_CNF:
		rc = 0;
		result->validData = true;
		break;
	/* Atheros 6400增加了设备上线主动通知的MME */
	case VS_DEVICEUP_TRAP:
		result->opStatus.status = EAGAIN;
		result->opStatus.type = XX_MMTYPE_BAD;
		break;
	case VS_GET_PROPERTY | MMTYPE_CNF:
		
		if(0x00 == intohl(pget_property_cnf_header->COOKIE))
		{
			rc = ihp_DecodeGetFrequencyBandSelection (buffer, length, result);
		}
		else if(0x01 == intohl(pget_property_cnf_header->COOKIE))
		{
			rc = ihp_DecodeGetTxGain (buffer, length, result);
		}
		else if(0x02 == intohl(pget_property_cnf_header->COOKIE))
		{
			rc = ihp_DecodeGetUserHFID (buffer, length, result);
		}
		else
		{
			printf("ihpapi_RxFrame->case VS_GET_PROPERTY: set errno = ENOSYS\n");
			result->opStatus.status = errno = ENOSYS;
			result->opStatus.type = XX_MMTYPE_BAD;
		}
		break;
	case VS_SET_PROPERTY | MMTYPE_CNF:
		
		if(0x00 == intohl(pset_property_cnf->COOKIE))
		{
			rc = ihp_DecodeSetFrequencyBandSelection (buffer, length, result);
		}
		else if(0x01 == intohl(pset_property_cnf->COOKIE))
		{
			rc = ihp_DecodeSetTxGain (buffer, length, result);
		}
		else
		{
			printf("ihpapi_RxFrame->case VS_GET_PROPERTY: set errno = ENOSYS\n");
			result->opStatus.status = errno = ENOSYS;
			result->opStatus.type = XX_MMTYPE_BAD;
		}
		break;
	default:
		printf("ihpapi_RxFrame->case default: set errno = ENOSYS\n");
		result->opStatus.status = errno = ENOSYS;
		result->opStatus.type = XX_MMTYPE_BAD;
	}
	return (rc);
}


//add by stan for v1
int ihpapi_v1_RxFrame (size_t length, uint8_t buffer [], ihpapi_result_t * result) 

{
	extern struct SeqCB scb;
	int rc = -1;
	//int i=0;
	uint8_t OUI [3] = 
	{
		0x00,
		0xB0,
		0x52 
	};
	//TxInfo *tcb = &scb.tcb;
	//BlockInfo block;
	struct header_v1_cnf * header = (struct header_v1_cnf *)(buffer);
	memset (result, 0, sizeof (* result));
	result->opCode = IHPAPI_OPCODE_NOOP;
	result->opCompltCode = IHPAPI_OPCMPLTCODE_NOOP;
	result->opStatus.type = XX_MMTYPE_BAD;
	result->opStatus.status = 0;
	result->validData = false;
	result->dataLen = 0;
	
	

#if INTELLON_SAFEMODE
 
	if (buffer == (uint8_t *)(0)) 
	{
		result->opStatus.status = errno = EFAULT;
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		result->opStatus.status = errno = EFAULT;
		return (-1);
	}

#endif
 
	if (ntohs (header->ethernet.MTYPE) != HOMEPLUG_MTYPE) 
	{
		result->opStatus.status = errno = EPERM;
		return (-1);
	}
	//fix by stan use 0x01 version 
	
	
	if (header->intellon.MMV != 0x01) 
	{
		result->opStatus.status = errno = EFAULT;
		return (-1);
	}
	if (memcmp (header->intellon.OUI, OUI, sizeof (OUI))) 
	{
		result->opStatus.status = errno = EFAULT;
		return (-1);
	}
	result->opStatus.type = intohs(header->intellon.MMTYPE);
	memcpy (result->ODA, header->ethernet.ODA, IHPAPI_ETHER_ADDR_LEN);
	memcpy (result->OSA, header->ethernet.OSA, IHPAPI_ETHER_ADDR_LEN);
	
	switch (result->opStatus.type) 
	{
	case VS_NW_INFO_STATS | MMTYPE_CNF:
		rc = ihp_DecodeNetworkInfoStats (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_GET_NETWORK_INFO_STATS;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	case VS_TONE_MAP_CHAR | MMTYPE_CNF:
		rc = ihp_DecodeGet74ToneMapInfo (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_GET_TONE_MAP_INFO;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	case VS_RX_TONE_MAP_CHAR | MMTYPE_CNF:
		rc = ihp_DecodeGet74RxToneMapInfo (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_GET_RX_TONE_MAP_INFO;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	case VS_NW_INFO | MMTYPE_CNF:
		rc = ihp_Decode74NetworkInfo (buffer, length, result);
		result->opCode = IHPAPI_OPCODE_GET_NETWORK_INFO;
		result->opCompltCode = IHPAPI_OPCMPLTCODE_COMPLETE;
		break;
	default:
		printf("ihpapi_RxFrame->case default: set errno = ENOSYS\n");
		result->opStatus.status = errno = ENOSYS;
		result->opStatus.type = XX_MMTYPE_BAD;
	}
	return (rc);
}
#endif
 

