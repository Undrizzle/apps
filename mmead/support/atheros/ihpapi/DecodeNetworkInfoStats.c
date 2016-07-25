/*====================================================================*
 *
 *   int ihp_DecodeNetworkInfo (const uint8_t buffer [], size_t length, ihpapi_result_t * result);
 *
 *   ihp.h
 *
 *   decode buffer as a VS_LNK_STATS Confirm MME; update the structure 
 *   named by the caller; 
 *
 *   return 0 on decode success and -1 on decode error;
 * 
 *   EFAULT if buffer/result address or size are invalid or illegal;
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

#ifndef DECODENETWORKINFOSTATS_SOURCE
#define DECODENETWORKINFOSTATS_SOURCE

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "../ihpapi/ihp.h"
 
/*====================================================================*
 *   single station structure;
 *--------------------------------------------------------------------*/
 


/*====================================================================*
 *   single network structure;
 *--------------------------------------------------------------------*/
 
typedef struct __packed network_v1_s 

{
	struct header_v1_vs header;
		uint8_t MME_SUBVER;
		uint8_t RESERVED;
		uint16_t MME_DATA_LEN;
		uint8_t FIRST_TEI;
		uint8_t NUM_STAS;
		uint16_t RESERVED1;
		uint8_t IN_AVLIN;
		uint8_t NID[7];
		uint8_t RESERVED2;
		uint8_t SNID;
		uint8_t TEI;
		uint16_t RESERVED3;
		uint8_t STATIONROLE;
		uint8_t CCO_MACADDR[6];
		uint8_t ACCESS;
		uint8_t NUMCORDNWS;
		uint8_t CCO_TEI;
		uint8_t REVERSEVED4[7];		
} network_v1_t;


int ihp_DecodeNetworkInfoStats (const uint8_t buffer [], size_t length, ihpapi_result_t * result) 

{
	
	//int i=0;
	
	
	struct vs_nw_info_stats_v1_cnf 
	{
		struct header_v1_vs header;
		uint8_t MME_SUBVER;
		uint8_t RESERVED;
		uint8_t MME_DATA_LEN[2];
		uint8_t FIRST_TEI;
		uint8_t NUM_STAS;
		uint8_t RESERVED1[2];
		uint8_t IN_AVLIN;
		uint8_t NID[7];
		uint8_t RESERVED2;
		uint8_t SNID;
		uint8_t TEI;
		uint8_t RESERVED3[2];
		uint8_t STATIONROLE;
		uint8_t CCO_MACADDR[6];
		uint8_t ACCESS;
		uint8_t NUMCORDNWS;
		uint8_t CCO_TEI;
		uint8_t REVERSEVED4[7];
		uint8_t nwinfostats[IHPAPI_ETHER_MAX_LEN - IHPAPI_ETHER_HDR_LEN - 38]  ; 
	}
	*confirm = (struct vs_nw_info_stats_v1_cnf *)(buffer);
	ihpapi_getNetworkInfoStatsData_t * message = & result->data.netInfoStats;
	//v1sta_t  * pv1sta = (v1sta_t  *)(message->nwinfostats);
	
	if (length < sizeof (struct network_v1_s)) 
	{
		errno = EFAULT;
		return (-1);
	}
	if (result == (ihpapi_result_t *)(0)) 
	{
		errno = EFAULT;
		return (-1);
	}
		
	message->MME_SUBVER = confirm->MME_SUBVER;
	message->RESERVED = confirm->RESERVED;
	memcpy (message->MME_DATA_LEN, confirm->MME_DATA_LEN, sizeof(confirm->MME_DATA_LEN));	
	message->FIRST_TEI = confirm->FIRST_TEI;
	message->NUM_STAS = confirm->NUM_STAS;
	memcpy (message->RESERVED1, confirm->RESERVED1, sizeof(confirm->RESERVED1));
	message->IN_AVLIN = confirm->IN_AVLIN;
	memcpy (message->NID, confirm->NID, sizeof(confirm->NID));
	message->RESERVED2 = confirm->RESERVED2;
	message->SNID = confirm->SNID;
	message->TEI = confirm->TEI;
	memcpy (message->RESERVED3, confirm->RESERVED3, sizeof(confirm->RESERVED3));
	message->STATIONROLE = confirm->STATIONROLE;
	memcpy (message->CCO_MACADDR, confirm->CCO_MACADDR, sizeof(confirm->CCO_MACADDR));
	message->ACCESS = confirm->ACCESS;
	message->NUMCORDNWS = confirm->NUMCORDNWS;
	message->CCO_TEI = confirm->CCO_TEI;
	memcpy (message->REVERSEVED4, confirm->REVERSEVED4, sizeof(confirm->REVERSEVED4));
	memcpy (message->nwinfostats, confirm->nwinfostats, sizeof (confirm->nwinfostats));
	result->dataLen = sizeof (ihpapi_getNetworkInfoStatsData_t);
	result->validData = true;
	result->opStatus.status = 0;
	return (0);
}

#endif
 

