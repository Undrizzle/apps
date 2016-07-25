#ifndef __REGISTER_H__
#define __REGISTER_H__

#include <public.h>

extern int REGISTER_DEBUG_ENABLE;

#define REGISTER_POLL_INT	20

#define debug_printf(...);	\
{	\
	if( REGISTER_DEBUG_ENABLE )	\
		fprintf(stderr, __VA_ARGS__);	\
}

void debug_dump_msg(const unsigned char memory [], size_t length, FILE *fp);
void ProcessExtReq(void);
void do_cnu_unregister(uint32_t clt_index, uint32_t cnu_index);

#endif

