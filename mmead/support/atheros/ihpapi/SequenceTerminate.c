/*====================================================================*
 *
 *   void ihpapi_SequenceTerminate(void);
 *
 *   ihpapi.h
 *
 *   The API provides this function call to reset the sequencer to
 *   initial state.
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
 *:  Published 2007 by Intellon Corp. ALL RIGHTS RESERVED;
 *;  For demonstration and evaluation only; Not for production use;
 *
 *   Contributor(s): 
 *	Alex Vasquez, alex.vasquez@intellon.com
 *
 *--------------------------------------------------------------------*/

#ifndef SEQUENCETERMINATE_SOURCE
#define SEQUENCETERMINATE_SOURCE

#include <string.h>

#include "ihpapi.h"
#include "ihp.h"
 
void ihpapi_SequenceTerminate () 

{
	extern struct SeqCB scb;
	bclose (scb.bcb [0]);
	bclose (scb.bcb [1]);
	memset (&scb, 0, sizeof (scb));
	return;
}

#endif


