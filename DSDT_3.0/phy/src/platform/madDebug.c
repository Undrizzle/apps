#include <madCopyright.h>
/********************************************************************************
* madDebug.c
*
* DESCRIPTION:
*       Debug message display routine
*
* DEPENDENCIES:
*       OS Dependent
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifdef DEBUG_MAD 
#ifdef _VXWORKS
#include "vxWorks.h"
#include "logLib.h"
#include "stdarg.h"
#elif defined(LINUX)
#include "stdarg.h"
#endif

/*******************************************************************************
* madDbgPrint
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*
* COMMENTS:
*       None
*
*******************************************************************************/
#if defined(_VXWORKS) || defined(LINUX)
void madDbgPrint(char* format, ...)
{
    va_list argP;
    char dbgStr[1000] = "";

    va_start(argP, format);

    vsprintf(dbgStr, format, argP);

#ifdef _VXWORKS
    printf(dbgStr);
/*    logMsg(dbgStr,0,1,2,3,4,5); */
#elif defined(LINUX)
  #ifdef __KERNEL__
    printk(dbgStr);
  #else
    printf(dbgStr);
  #endif
#endif
    return;
}
#else
void madDbgPrint(char* format, ...)
{
}
#endif
#else /* DEBUG_MAD not defined */
void madDbgPrint(char* format, ...)
{
}
#endif /* DEBUG_MAD */

