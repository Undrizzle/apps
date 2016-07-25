#ifndef DECODESETHGMANAGE_SOURCE
#define DECODESETHGMANAGE_SOURCE

#include <stdint.h>
#include <string.h>

#include "../ihpapi/ihp.h"
 
int ihp_DecodeSetHgManage(const uint8_t buffer [ ], size_t length, ihpapi_result_t * result)

{
	vs_hg_cnf_header_t * confirm = (vs_hg_cnf_header_t *)(buffer);

	if( 0x01 != intohs(confirm->action) )
	{
		result->validData = false;
		result->opStatus.status = 0x01;
		return (-1);
	}
	else
	{
		result->validData = true;
		result->opStatus.status = intohs(confirm->MSTATUS);		
		return (0);
	}
}

#endif
 

