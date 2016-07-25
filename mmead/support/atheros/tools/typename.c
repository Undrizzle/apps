/*====================================================================*
 *
 *   const char *typename ( const struct _type_ types[], size_t count, type_t type, const char * name);
 *
 *   tools.h  
 *
 *   return the name associated with a given type by searching a name
 *   list arranged in ascending order by type; return the typename if
 *   the type is found or the name argument if not;
 *
 *   typedef type_t and struct _type_ are defined in types.h; 
 *
 *.  Motley Tools from Charlie Maier at cmaier@cmassoc.net;
 *:  Published 2006 by Charles Maier Associates Limited;
 *;  Released under the GNU General Public Licence v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef TYPENAME_SOURCE
#define TYPENAME_SOURCE

#include "../tools/types.h"
#include "../tools/tools.h"
 
const char *typename ( const struct _type_ types [], size_t count, type_t type, const char * name) 

{
	size_t lower = 0;
	size_t upper = count;
	while (lower < upper) 
	{
		size_t index = (lower + upper) >> 1;
		signed order = type - types [index].type;
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
		return (types [index].name);
	}
	return (name);
}

#endif
 

