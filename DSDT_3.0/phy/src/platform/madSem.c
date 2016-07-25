#include <madCopyright.h>
/********************************************************************************
* madOs.c
*
* DESCRIPTION:
*       Semaphore related routines
*
* DEPENDENCIES:
*       OS Dependent.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#include <madApi.h>
#include <madSem.h>


/*******************************************************************************
* madSemCreate
*
* DESCRIPTION:
*       Create semaphore.
*
* INPUTS:
*        state - beginning state of the semaphore, either MAD_SEM_EMPTY or MAD_SEM_FULL
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
    IN MAD_SEM_BEGIN_STATE  state
)
{
    if(dev->semCreate)
        return dev->semCreate(state);

    return 1; /* should return any value other than 0 to let it keep going */
}

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
    IN MAD_DEV  *dev,
    IN MAD_SEM  smid
)
{
    if((dev->semDelete) && (smid))
        return dev->semDelete(smid);

    return MAD_OK;
}


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
    IN MAD_DEV  *dev,
    IN MAD_SEM  smid, 
    IN MAD_U32  timeOut
)
{
    if(dev->semTake)
        return dev->semTake(smid, timeOut);

    return MAD_OK;

}

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
    IN MAD_SEM    smid
)
{
    if(dev->semGive)
        return dev->semGive(smid);

    return MAD_OK;
}

