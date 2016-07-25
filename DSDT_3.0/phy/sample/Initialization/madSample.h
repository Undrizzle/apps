#include <madCopyright.h>
/********************************************************************************
* madSample.h
*
* DESCRIPTION:
*       Types definitions for Sample program
*
* DEPENDENCIES:   Platform.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __pfTesth
#define __pfTesth

#include "madApi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* #define SHOW_DEBUG_INFO	1  */
#undef SHOW_DEBUG_INFO  
#ifndef MAD_APP
  #define MSG_PRINT(format,args...) printk(format, ## args)
  #ifdef SHOW_DEBUG_INFO
    #define MSG_PRINT_DBG(format,args...) printk(format, ## args)
  #else
    #define MSG_PRINT_DBG(format,args...);
  #endif
#else
  #define MSG_PRINT printf
  #ifdef SHOW_DEBUG_INFO
    #define MSG_PRINT_DBG printf
  #else
    #define MSG_PRINT_DBG
  #endif
#endif

#undef USE_SEMAPHORE

#ifdef USE_SEMAPHORE
MAD_SEM madOsSemCreate(MAD_SEM_BEGIN_STATE state);
MAD_STATUS madOsSemDelete(MAD_SEM smid);
MAD_STATUS madOsSemWait(MAD_SEM smid, MAD_U32 timeOut);
MAD_STATUS madOsSemSignal(MAD_SEM smid);
#endif

MAD_BOOL bspSpecificReadMii ( MAD_DEV* dev, unsigned int portNumber , unsigned int MIIReg,
                      unsigned int* value);
MAD_BOOL bspSpecificWriteMii ( MAD_DEV* dev, unsigned int portNumber , unsigned int MIIReg,
                       unsigned int value);


extern void testMADDisplayStatus(MAD_STATUS status);

#ifdef __cplusplus
}
#endif

#endif   /* __pfTesth */

