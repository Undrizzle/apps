/*====================================================================*
 *
 *   int ihpapi_SequenceInProgress (void);
 *
 *   ihpapi.h
 *
 *   The Intellon HomePlug AV API works with one sequence at the time.
 *   The API provides this function call so the Caller can check
 *   the precondition of single sequence only.
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
 *	Charles Maier, charles.maier@intellon.com
 *
 *--------------------------------------------------------------------*/

#ifndef SEQUENCEINPROGRESS_SOURCE
#define SEQUENCEINPROGRESS_SOURCE

#include "ihpapi.h"
#include "ihp.h"
 
bool ihpapi_SequenceInProgress () 

{
	extern struct SeqCB scb;
	return (_anyset (scb.flags, scbFlag_bsy));
}

#endif
 

