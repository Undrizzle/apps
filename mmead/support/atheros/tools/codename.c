/*====================================================================*
 *
 *   const char *codename ( const struct _code_ codes[], size_t count, code_t code, const char * name);
 *
 *   tools.h  
 *
 *   return the name associated with a given code by searching a name
 *   list arranged in ascending order by code; return the codename if
 *   the code is found or the name argument if not;
 *
 *   typedef code_t and struct _code_ are defined in types.h; 
 *
 *.  Motley Tools from Charlie Maier at cmaier@cmassoc.net;
 *:  Published 2006 by Charles Maier Associates Limited;
 *;  Released under the GNU General Public Licence v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef CODENAME_SOURCE
#define CODENAME_SOURCE

#include "../tools/types.h"
#include "../tools/tools.h"
 
const char *codename ( const struct _code_ codes [], size_t count, code_t code, const char * name) 

{
	size_t lower = 0;
	size_t upper = count;
	while (lower < upper) 
	{
		size_t index = (lower + upper) >> 1;
		signed order = code - codes [index].code;
		if (order < 0) 
		{
			upper = index - 0;
			continue;
		}
		if (order > 0) 
		{
			lower = index + 1;
			continue;
		}
		return (codes [index].name);
	}
	return (name);
}

#endif
 

