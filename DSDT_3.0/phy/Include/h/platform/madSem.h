#include <madCopyright.h>

/********************************************************************************
* madOs.h
*
* DESCRIPTION:
*       Operating System wrapper
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#ifndef __madSemh
#define __madSemh

#include <madApi.h>

#ifdef __cplusplus
extern "C" {
#endif

/***** Defines  ********************************************************/

#define OS_WAIT_FOREVER             0

#define OS_MAX_TASKS                30
#define OS_MAX_TASK_NAME_LENGTH     10

#define OS_MAX_QUEUES               30
#define OS_MAX_QUEUE_NAME_LENGTH    10

#define OS_MAX_EVENTS               10

#define OS_MAX_SEMAPHORES           50

#define OS_EOF                      (-1)


/*******************************************************************************
* madSemCreate
*
* DESCRIPTION:
*       Create semaphore.
*
* INPUTS:
*        state - beginning state of the semaphore, either SEM_EMPTY or SEM_FULL
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MAD_SEM if success. Otherwise, NULL
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_SEM madSemCreate
(
    IN MAD_DEV    *dev,
    IN MAD_SEM_BEGIN_STATE state
);

/*******************************************************************************
* madSemDelete
*
* DESCRIPTION:
*       Delete semaphore.
*
* INPUTS:
*       smid - semaphore Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madSemDelete
(
    IN MAD_DEV    *dev,
    IN MAD_SEM       smid
);

/*******************************************************************************
* madSemTake
*
* DESCRIPTION:
*       Wait for semaphore.
*
* INPUTS:
*       smid    - semaphore Id
*       timeOut - time out in miliseconds or 0 to wait forever
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*       OS_TIMEOUT - on time out
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madSemTake
(
    IN MAD_DEV    *dev,
    IN MAD_SEM       smid,
    IN MAD_U32       timeOut
);

/*******************************************************************************
* madSemGive
*
* DESCRIPTION:
*       release the semaphore which was taken previously.
*
* INPUTS:
*       smid    - semaphore Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madSemGive
(
    IN MAD_DEV    *dev,
    IN MAD_SEM       smid
);

#ifdef __cplusplus
}
#endif

#endif  /* __madSemh */
