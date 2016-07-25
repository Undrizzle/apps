/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 17337 $ of $ 
 *
 * $Id:$
 */
/** \page MFD helper for profileTable
 *
 * \section intro Introduction
 * Introductory text.
 *
 */
/* standard Net-SNMP includes */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

/* include our parent header */
#include "profileTable.h"

#include <net-snmp/agent/mib_modules.h>

#include "profileTable_interface.h"

const oid profileTable_oid[] = { PROFILETABLE_OID };
const int profileTable_oid_size = OID_LENGTH(profileTable_oid);

    profileTable_registration  profileTable_user_context;

void initialize_table_profileTable(void);
void shutdown_table_profileTable(void);


/**
 * Initializes the profileTable module
 */
void
init_profileTable(void)
{
    DEBUGMSGTL(("verbose:profileTable:init_profileTable","called\n"));

    /*
     * TODO:300:o: Perform profileTable one-time module initialization.
     */
     
    /*
     * here we initialize all the tables we're planning on supporting
     */
    if (should_init("profileTable"))
        initialize_table_profileTable();

} /* init_profileTable */

/**
 * Shut-down the profileTable module (agent is exiting)
 */
void
shutdown_profileTable(void)
{
    if (should_init("profileTable"))
        shutdown_table_profileTable();

}

/**
 * Initialize the table profileTable 
 *    (Define its contents and how it's structured)
 */
void
initialize_table_profileTable(void)
{
    profileTable_registration * user_context;
    u_long flags;

    DEBUGMSGTL(("verbose:profileTable:initialize_table_profileTable","called\n"));

    /*
     * TODO:301:o: Perform profileTable one-time table initialization.
     */

    /*
     * TODO:302:o: |->Initialize profileTable user context
     * if you'd like to pass in a pointer to some data for this
     * table, allocate or set it up here.
     */
    /*
     * a netsnmp_data_list is a simple way to store void pointers. A simple
     * string token is used to add, find or remove pointers.
     */
    user_context = netsnmp_create_data_list("profileTable", NULL, NULL);
    
    /*
     * No support for any flags yet, but in the future you would
     * set any flags here.
     */
    flags = 0;
    
    /*
     * call interface initialization code
     */
    _profileTable_initialize_interface(user_context, flags);
} /* initialize_table_profileTable */

/**
 * Shutdown the table profileTable 
 */
void
shutdown_table_profileTable(void)
{
    /*
     * call interface shutdown code
     */
    _profileTable_shutdown_interface(&profileTable_user_context);
}

/**
 * extra context initialization (eg default values)
 *
 * @param rowreq_ctx    : row request context
 * @param user_init_ctx : void pointer for user (parameter to rowreq_ctx_allocate)
 *
 * @retval MFD_SUCCESS  : no errors
 * @retval MFD_ERROR    : error (context allocate will fail)
 */
int
profileTable_rowreq_ctx_init(profileTable_rowreq_ctx *rowreq_ctx,
                           void *user_init_ctx)
{
    DEBUGMSGTL(("verbose:profileTable:profileTable_rowreq_ctx_init","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);
    
    /*
     * TODO:210:o: |-> Perform extra profileTable rowreq initialization. (eg DEFVALS)
     */

    return MFD_SUCCESS;
} /* profileTable_rowreq_ctx_init */

/**
 * extra context cleanup
 *
 */
void profileTable_rowreq_ctx_cleanup(profileTable_rowreq_ctx *rowreq_ctx)
{
    DEBUGMSGTL(("verbose:profileTable:profileTable_rowreq_ctx_cleanup","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);
    
    /*
     * TODO:211:o: |-> Perform extra profileTable rowreq cleanup.
     */
} /* profileTable_rowreq_ctx_cleanup */

/**
 * pre-request callback
 *
 *
 * @retval MFD_SUCCESS              : success.
 * @retval MFD_ERROR                : other error
 */
int
profileTable_pre_request(profileTable_registration * user_context)
{
    DEBUGMSGTL(("verbose:profileTable:profileTable_pre_request","called\n"));

    /*
     * TODO:510:o: Perform profileTable pre-request actions.
     */

    return MFD_SUCCESS;
} /* profileTable_pre_request */

/**
 * post-request callback
 *
 * Note:
 *   New rows have been inserted into the container, and
 *   deleted rows have been removed from the container and
 *   released.
 *
 * @param user_context
 * @param rc : MFD_SUCCESS if all requests succeeded
 *
 * @retval MFD_SUCCESS : success.
 * @retval MFD_ERROR   : other error (ignored)
 */
int
profileTable_post_request(profileTable_registration * user_context, int rc)
{
    DEBUGMSGTL(("verbose:profileTable:profileTable_post_request","called\n"));

    /*
     * TODO:511:o: Perform profileTable post-request actions.
     */

    /*
     * check to set if any rows were changed.
     */
    if (profileTable_dirty_get()) {
        /*
         * check if request was successful. If so, this would be
         * a good place to save data to its persistent store.
         */
        if (MFD_SUCCESS == rc) {
            /*
             * save changed rows, if you haven't already
             */
        }
        
        profileTable_dirty_set(0); /* clear table dirty flag */
    }

    return MFD_SUCCESS;
} /* profileTable_post_request */


/** @{ */