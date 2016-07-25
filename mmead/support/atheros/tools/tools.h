/*====================================================================*
 *
 *   tools.h - 
 *   
 *.  Motley Tools by Charles Maier; cmaier@cmassoc.net;
 *:  Published 2005 by Charles Maier Associates for internal use;
 *;  Released under GNU General Public License v2 and later;
 *
 *--------------------------------------------------------------------*/

#ifndef TOOLS_HEADER
#define TOOLS_HEADER
 
/*====================================================================*
 *   system header files;
 *--------------------------------------------------------------------*/

#include <unistd.h>
#include <stdint.h>
 
/*====================================================================*
 *   custom header files;
 *--------------------------------------------------------------------*/

#include "../tools/types.h"
 
/*====================================================================*
 *   macro expansions;
 *--------------------------------------------------------------------*/

#define NEW(object) (object *)(malloc(sizeof(object)))
 
/*====================================================================*
 *   
 *--------------------------------------------------------------------*/
 
unsigned uintspec (const char *value, unsigned minimum, unsigned maximum);
const char * typename (const struct _type_ list [], size_t size, type_t type, const char * name);
const char * codename (const struct _code_ list [], size_t size, code_t code, const char * name);
size_t typeload (struct _type_ list [], size_t size);
size_t codeload (struct _code_ list [], size_t size);
void typesave (struct _type_ list [], size_t size, const char * name);
void codesave (struct _code_ list [], size_t size, const char * name);

/*====================================================================*
 *   end definitions;
 *--------------------------------------------------------------------*/

#endif
 

