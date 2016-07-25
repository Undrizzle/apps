/*====================================================================*
 *
 *   size_t bgetnexts(uint8_t *bseg[], buffer_t *b);
 *
 *   buffer.h 
 *
 *   This function provides next buffer segment.
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
 *:  Released 2007 by Intellon Corporation, Ocala FL USA;
 *;  For demontration and evaluation only; Not for production use;
 *
 *   Contributor(s): 
 *	A. Vasquez, alex.vasquez@intellon.com
 *
 *--------------------------------------------------------------------*/

#ifndef BGETNEXTS_SOURCE
#define BGETNEXTS_SOURCE

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "../tools/buffer.h"
 
size_t bgetnexts (uint8_t *bseg [], buffer_t *b) 

{
	return (bgets (bseg, b->bcurrsegnum + 1, b));
}

#endif
 

