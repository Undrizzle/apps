#include <madCopyright.h>

/********************************************************************************
* madPlatformDeps.h
*
* DESCRIPTION:
*       platform dependent definitions
*
* DEPENDENCIES:   Platform.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __madPlatformDepsh
#define __madPlatformDepsh

#include <madApi.h>

#ifdef __cplusplus
extern "C" {
#endif

MAD_BOOL madDefaultMiiRead (unsigned int portNumber , unsigned int miiReg, unsigned int* value);
MAD_BOOL madDefaultMiiWrite (unsigned int portNumber , unsigned int miiReg, unsigned int value);

#ifdef __cplusplus
}
#endif

#endif   /* madPlatformDepsh */
