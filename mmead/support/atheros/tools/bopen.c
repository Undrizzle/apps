/*====================================================================*
 *
 *   void bclose(buffer_t * b);
 *
 *   buffer.h 
 *
 *   This function closes buffer structure and frees resources.
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

#ifndef BOPEN_SOURCE
#define BOPEN_SOURCE
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "../tools/buffer.h"
 
buffer_t * bopen (uint8_t b [], size_t blen, size_t bseglen) 

{
	buffer_t * buffer;
	if (b == 0) 
	{
		errno = EINVAL;
		return (0);
	}
	if (blen == 0 || bseglen == 0) 
	{
		errno = EINVAL;
		return (0);
	}
	if ((buffer = (buffer_t *)malloc (sizeof (buffer_t))) == 0) 
	{
		return (0);
	}
	buffer->bstart = buffer->bcurr = b;
	buffer->bend = b + blen;
	buffer->blen = blen;
	buffer->bseglen = bseglen;
	buffer->bcurrsegnum = 0;
	return buffer;
}

#endif
 

