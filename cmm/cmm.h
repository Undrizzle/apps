#ifndef __CMM_H__
#define __CMM_H__

#include <public.h>

extern int CMM_MODULE_DEBUG_ENABLE;

#define cmm_debug_printf(...);	\
{	\
	if( CMM_MODULE_DEBUG_ENABLE )	\
		fprintf(stderr, __VA_ARGS__);	\
}

void cmm_dump_msg(const unsigned char memory [], size_t length, FILE *fp);

#endif

