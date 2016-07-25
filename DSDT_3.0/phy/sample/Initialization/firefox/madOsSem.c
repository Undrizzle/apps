#include <madCopyright.h>
/********************************************************************************
* osSem.c
*
* DESCRIPTION:
*       Semaphore related routines
*
* DEPENDENCIES:
*       OS Dependent.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/


#include <madApi.h>

MAD_SEM madOsSemCreate( MAD_SEM_BEGIN_STATE state);
MAD_STATUS madOsSemDelete(MAD_SEM smid);
MAD_STATUS madOsSemWait(  MAD_SEM smid, MAD_U32 timeOut);
MAD_STATUS madOsSemSignal(MAD_SEM smid);

/*******************************************************************************
* osSemCreate
*
* DESCRIPTION:
*       Create semaphore.
*
* INPUTS:
*       name   - semaphore Name
*       init   - init value of semaphore counter
*       count  - max counter (must be >= 1)
*
* OUTPUTS:
*       smid - semaphore Id
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_SEM madOsSemCreate(MAD_SEM_BEGIN_STATE state)
{
    return MAD_OK;
}

/*******************************************************************************
* osSemDelete
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
MAD_STATUS madOsSemDelete(MAD_SEM smid)
{
    return MAD_OK;
}

/*******************************************************************************
* osSemWait
*
* DESCRIPTION:
*       Wait on semaphore.
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
MAD_STATUS madOsSemWait(MAD_SEM smid, MAD_U32 timeOut)
{
    return MAD_OK;
}

/*******************************************************************************
* osSemSignal
*
* DESCRIPTION:
*       Signal a semaphore.
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
MAD_STATUS madOsSemSignal(MAD_SEM smid)
{
    return MAD_OK;
}
