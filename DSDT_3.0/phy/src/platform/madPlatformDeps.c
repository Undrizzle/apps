#include <madCopyright.h>
/********************************************************************************
* madPlatformDeps.c
*
* DESCRIPTION:
*       platform dependent functions
*
* DEPENDENCIES:   Platform.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#include <madApi.h>
#include <madHwCntl.h>
#include <madPlatformDeps.h>


MAD_BOOL madDefaultMiiRead (unsigned int portNumber , unsigned int miiReg, unsigned int* value)
{
    *value = 0;

    return MAD_TRUE;
}

MAD_BOOL madDefaultMiiWrite (unsigned int portNumber , unsigned int miiReg, unsigned int value)
{
    return MAD_TRUE;
}
