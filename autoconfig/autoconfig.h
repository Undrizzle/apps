#ifndef __AUTOCONFIG_H__
#define __AUTOCONFIG_H__

#include <public.h>

extern int AUTOCONFIG_DEBUG_ENABLE;

#define AUTOCONFIG_POLL_INT	20

#define debug_printf(...);	\
{	\
	if( AUTOCONFIG_DEBUG_ENABLE )	\
		fprintf(stderr, __VA_ARGS__);	\
}

void debug_dump_msg(const unsigned char memory [], size_t length, FILE *fp);
void ProcessExtReq(void);
void do_cnu_autoconfig(uint32_t clt_index, uint32_t cnu_index);

#endif
