/*====================================================================*
 *
 *   size_t ihpapi_TxFrame (size_t bufferLen, uint8_t buffer []);
 *
 *   ihpapi.h
 *
 *   Encode buffer with remaining data to be transmitted and return
 *   the number of bytes encoded or 0 on an encoding error;
 *
 *   Byte ordering has been taken care by the encoders;
 *
 *   internal state information is stored in the StateInfo structure
 *   defined in ihp.h;
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

#ifndef TXFRAME_SOURCE
#define TXFRAME_SOURCE

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "ihpapi.h"
#include "ihp.h"

struct SeqCB scb;
 
size_t ihpapi_TxFrame (size_t bufferLen, uint8_t buffer []) 

{

	TxInfo * tcb = &scb.tcb;
	size_t offset = 0;

#if INTELLON_SAFEMODE
 
	if (tcb->OSA == (uint8_t *)(0)) 
	{
		errno = EFAULT;
		return (0);
	}
	if (tcb->ODA == (uint8_t *)(0)) 
	{
		errno = EFAULT;
		return (0);
	}
	if (buffer == (uint8_t *)(0)) 
	{
		errno = EFAULT;
		return (0);
	}

#endif
 
	if (!tcb->txok) 
	{
		return (0);
	}
	tcb->txok = false;
	offset += EncodeEthernetHeader (buffer + offset, bufferLen - offset, tcb->ODA, tcb->OSA);
	offset += EncodeIntellonHeader (buffer + offset, bufferLen - offset, tcb->MMTYPE);
	if (tcb->hdrlen) 
	{
		memcpy (buffer + offset, &tcb->hdr, tcb->hdrlen);
		offset += tcb->hdrlen;
	}
	if (tcb->datalen) 
	{
		memcpy (buffer + offset, tcb->data, tcb->datalen);
		offset += tcb->datalen;
	}
	if (tcb->tlrlen) 
	{
		memcpy (buffer + offset, &tcb->trailer, tcb->tlrlen);
		offset += tcb->tlrlen;
	}
	if (offset < IHPAPI_ETHER_MIN_LEN) 
	{
		memset (buffer + offset, 0, IHPAPI_ETHER_MIN_LEN - offset);
		offset = IHPAPI_ETHER_MIN_LEN;
	}
	return (offset);
}

#endif
 

