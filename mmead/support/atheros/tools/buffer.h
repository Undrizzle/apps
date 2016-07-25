/*====================================================================*
 *
 *   buffer.h - Buffer segmentation functions
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

#ifndef BUFFER_H_
#define BUFFER_H_

#include "../tools/types.h"
 
typedef struct buffer_s 

{
	uint8_t *bstart;
	uint8_t *bcurr;
	uint8_t *bend;
	size_t bcurrsegnum;
	size_t blen;
	size_t bseglen;
}

buffer_t;
buffer_t *bopen (uint8_t b [], size_t blen, size_t bseglen);
size_t bgeto (buffer_t *b);
size_t bgets (uint8_t *bseg [], size_t bsegnum, buffer_t *b);
size_t bgetnexts (uint8_t *bseg [], buffer_t *b);
bool beof (size_t length, buffer_t *b);
void bclose (buffer_t *b);

/*BUFFER_H_*/

#endif 
 

