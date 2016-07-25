#include <madCopyright.h>

/********************************************************************************
* madVersion.h
*
* DESCRIPTION:
*       Includes software version information for the Marvell Alaska Driver.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <madApi.h>
#include <madUtils.h>

char madApiCopyright[] = MADAPI_COPYRIGHT;
char madApiVersion[] = MADAPI_VERSION;

/*******************************************************************************
* mdVersion
*
* DESCRIPTION:
*       This function returns the version of the Marvell Alaska Driver.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       version     - driver version.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_FAIL | [reason]  - on error
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS mdVersion
(
    OUT MAD_VERSION   *version
)
{
    if(version == NULL)
        return MAD_DRV_ERROR_VERSION;

    if(madStrlen(madApiVersion) > VERSION_MAX_LEN)
    {
        return MAD_DRV_ERROR_VERSION;
    }

    madMemCpy(version->version,madApiVersion,madStrlen(madApiVersion));
    version->version[madStrlen(madApiVersion)] = '\0';
    return MAD_OK;
}

