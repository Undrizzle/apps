/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 17337 $ of $ 
 *
 * $Id:$
 */
/** \page MFD helper for cltTable
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
#include "cltTable.h"

#include <net-snmp/agent/mib_modules.h>

#include "cltTable_interface.h"

const oid cltTable_oid[] = { CLTTABLE_OID };
const int cltTable_oid_size = OID_LENGTH(cltTable_oid);

    cltTable_registration  cltTable_user_context;

void initialize_table_cltTable(void);
void shutdown_table_cltTable(void);


/**
 * Initializes the cltTable module
 */
void
init_cltTable(void)
{
    DEBUGMSGTL(("verbose:cltTable:init_cltTable","called\n"));

    /*
     * TODO:300:o: Perform cltTable one-time module initialization.
     */
     
    /*
     * here we initialize all the tables we're planning on supporting
     */
    if (should_init("cltTable"))
        initialize_table_cltTable();

} /* init_cltTable */

/**
 * Shut-down the cltTable module (agent is exiting)
 */
void
shutdown_cltTable(void)
{
    if (should_init("cltTable"))
        shutdown_table_cltTable();

}

/**
 * Initialize the table cltTable 
 *    (Define its contents and how it's structured)
 */
void
initialize_table_cltTable(void)
{
    cltTable_registration * user_context;
    u_long flags;

    DEBUGMSGTL(("verbose:cltTable:initialize_table_cltTable","called\n"));

    /*
     * TODO:301:o: Perform cltTable one-time table initialization.
     */

    /*
     * TODO:302:o: |->Initialize cltTable user context
     * if you'd like to pass in a pointer to some data for this
     * table, allocate or set it up here.
     */
    /*
     * a netsnmp_data_list is a simple way to store void pointers. A simple
     * string token is used to add, find or remove pointers.
     */
    user_context = netsnmp_create_data_list("cltTable", NULL, NULL);
    
    /*
     * No support for any flags yet, but in the future you would
     * set any flags here.
     */
    flags = 0;
    
    /*
     * call interface initialization code
     */
    _cltTable_initialize_interface(user_context, flags);
} /* initialize_table_cltTable */

/**
 * Shutdown the table cltTable 
 */
void
shutdown_table_cltTable(void)
{
    /*
     * call interface shutdown code
     */
    _cltTable_shutdown_interface(&cltTable_user_context);
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
cltTable_rowreq_ctx_init(cltTable_rowreq_ctx *rowreq_ctx,
                           void *user_init_ctx)
{
    DEBUGMSGTL(("verbose:cltTable:cltTable_rowreq_ctx_init","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);
    
    /*
     * TODO:210:o: |-> Perform extra cltTable rowreq initialization. (eg DEFVALS)
     */

    return MFD_SUCCESS;
} /* cltTable_rowreq_ctx_init */

/**
 * extra context cleanup
 *
 */
void cltTable_rowreq_ctx_cleanup(cltTable_rowreq_ctx *rowreq_ctx)
{
    DEBUGMSGTL(("verbose:cltTable:cltTable_rowreq_ctx_cleanup","called\n"));

    netsnmp_assert(NULL != rowreq_ctx);
    
    /*
     * TODO:211:o: |-> Perform extra cltTable rowreq cleanup.
     */
} /* cltTable_rowreq_ctx_cleanup */

/**
 * pre-request callback
 *
 *
 * @retval MFD_SUCCESS              : success.
 * @retval MFD_ERROR                : other error
 */
int
cltTable_pre_request(cltTable_registration * user_context)
{
    DEBUGMSGTL(("verbose:cltTable:cltTable_pre_request","called\n"));

    /*
     * TODO:510:o: Perform cltTable pre-request actions.
     */

    return MFD_SUCCESS;
} /* cltTable_pre_request */

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
cltTable_post_request(cltTable_registration * user_context, int rc)
{
    DEBUGMSGTL(("verbose:cltTable:cltTable_post_request","called\n"));

    /*
     * TODO:511:o: Perform cltTable post-request actions.
     */

    /*
     * check to set if any rows were changed.
     */
    if (cltTable_dirty_get()) {
        /*
         * check if request was successful. If so, this would be
         * a good place to save data to its persistent store.
         */
        if (MFD_SUCCESS == rc) {
            /*
             * save changed rows, if you haven't already
             */
        }
        
        cltTable_dirty_set(0); /* clear table dirty flag */
    }

    return MFD_SUCCESS;
} /* cltTable_post_request */


/** @{ */