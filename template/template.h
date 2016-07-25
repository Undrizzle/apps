#ifndef __TEMPLATE_CORE_H__
#define __TEMPLATE_CORE_H__

#include <public.h>

extern int TEMPLATE_DEBUG_ENABLE;

#define __tm_debug(...);	\
{	\
	if( TEMPLATE_DEBUG_ENABLE )	\
		fprintf(stderr, __VA_ARGS__);	\
}

void __tm_msg(const unsigned char memory [], size_t length, FILE *fp);

#endif

