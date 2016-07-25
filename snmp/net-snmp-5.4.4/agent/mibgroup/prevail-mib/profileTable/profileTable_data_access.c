/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 14170 $ of $ 
 *
 * $Id:$
 */
/* standard Net-SNMP includes */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
 /* modified by frank */
#include <dbsapi.h>

/* include our parent header */
#include "profileTable.h"
#include <snmp2cmm.h>

#include "profileTable_data_access.h"

/** @ingroup interface
 * @addtogroup data_access data_access: Routines to access data
 *
 * These routines are used to locate the data used to satisfy
 * requests.
 * 
 * @{
 */
/**********************************************************************
 **********************************************************************
 ***
 *** Table profileTable
 ***
 **********************************************************************
 **********************************************************************/
/*
 * prevail-mib::profileTable is subid 8 of modEoCMib.
 * Its status is Current.
 * OID: .1.3.6.1.4.1.36186.8.8, length: 9
*/

/**
 * initialization for profileTable data access
 *
 * This function is called during startup to allow you to
 * allocate any resources you need for the data table.
 *
 * @param profileTable_reg
 *        Pointer to profileTable_registration
 *
 * @retval MFD_SUCCESS : success.
 * @retval MFD_ERROR   : unrecoverable error.
 */
int
profileTable_init_data(profileTable_registration * profileTable_reg)
{
    DEBUGMSGTL(("verbose:profileTable:profileTable_init_data","called\n"));
    //printf("-->profileTable_init_data()\n");

    /*
     * TODO:303:o: Initialize profileTable data.
     */
    /*
    ***************************************************
    ***             START EXAMPLE CODE              ***
    ***---------------------------------------------***/
    /*
     * if you are the sole writer for the file, you could
     * open it here. However, as stated earlier, we are assuming
     * the worst case, which in this case means that the file is
     * written to by someone else, and might not even exist when
     * we start up. So we can't do anything here.
     */
    /*
    ***---------------------------------------------***
    ***              END  EXAMPLE CODE              ***
    ***************************************************/

    return MFD_SUCCESS;
} /* profileTable_init_data */

/**
 * container overview
 *
 */

/**
 * container initialization
 *
 * @param container_ptr_ptr A pointer to a container pointer. If you
 *        create a custom container, use this parameter to return it
 *        to the MFD helper. If set to NULL, the MFD helper will
 *        allocate a container for you.
 * @param  cache A pointer to a cache structure. You can set the timeout
 *         and other cache flags using this pointer.
 *
 *  This function is called at startup to allow you to customize certain
 *  aspects of the access method. For the most part, it is for advanced
 *  users. The default code should suffice for most cases. If no custom
 *  container is allocated, the MFD code will create one for your.
 *
 *  This is also the place to set up cache behavior. The default, to
 *  simply set the cache timeout, will work well with the default
 *  container. If you are using a custom container, you may want to
 *  look at the cache helper documentation to see if there are any
 *  flags you want to set.
 *
 * @remark
 *  This would also be a good place to do any initialization needed
 *  for you data source. For example, opening a connection to another
 *  process that will supply the data, opening a database, etc.
 */
void
profileTable_container_init(netsnmp_container **container_ptr_ptr,
                             netsnmp_cache *cache)
{
    DEBUGMSGTL(("verbose:profileTable:profileTable_container_init","called\n"));
    //printf("-->profileTable_container_init()\n");
    if (NULL == container_ptr_ptr) {
        snmp_log(LOG_ERR,"bad container param to profileTable_container_init\n");
        return;
    }

    /*
     * For advanced users, you can use a custom container. If you
     * do not create one, one will be created for you.
     */
    *container_ptr_ptr = NULL;

    if (NULL == cache) {
        snmp_log(LOG_ERR,"bad cache param to profileTable_container_init\n");
        return;
    }

    /*
     * TODO:345:A: Set up profileTable cache properties.
     *
     * Also for advanced users, you can set parameters for the
     * cache. Do not change the magic pointer, as it is used
     * by the MFD helper. To completely disable caching, set
     * cache->enabled to 0.
     */
    cache->timeout = PROFILETABLE_CACHE_TIMEOUT; /* seconds */
} /* profileTable_container_init */

/**
 * container shutdown
 *
 * @param container_ptr A pointer to the container.
 *
 *  This function is called at shutdown to allow you to customize certain
 *  aspects of the access method. For the most part, it is for advanced
 *  users. The default code should suffice for most cases.
 *
 *  This function is called before profileTable_container_free().
 *
 * @remark
 *  This would also be a good place to do any cleanup needed
 *  for you data source. For example, closing a connection to another
 *  process that supplied the data, closing a database, etc.
 */
void
profileTable_container_shutdown(netsnmp_container *container_ptr)
{
    DEBUGMSGTL(("verbose:profileTable:profileTable_container_shutdown","called\n"));
    //printf("-->profileTable_container_shutdown()\n");
    if (NULL == container_ptr) {
        snmp_log(LOG_ERR,"bad params to profileTable_container_shutdown\n");
        return;
    }
} /* profileTable_container_shutdown */

/**
 * load initial data
 *
 * TODO:350:M: Implement profileTable data load
 * This function will also be called by the cache helper to load
 * the container again (after the container free function has been
 * called to free the previous contents).
 *
 * @param container container to which items should be inserted
 *
 * @retval MFD_SUCCESS              : success.
 * @retval MFD_RESOURCE_UNAVAILABLE : Can't access data source
 * @retval MFD_ERROR                : other error.
 *
 *  This function is called to load the index(es) (and data, optionally)
 *  for the every row in the data set.
 *
 * @remark
 *  While loading the data, the only important thing is the indexes.
 *  If access to your data is cheap/fast (e.g. you have a pointer to a
 *  structure in memory), it would make sense to update the data here.
 *  If, however, the accessing the data invovles more work (e.g. parsing
 *  some other existing data, or peforming calculations to derive the data),
 *  then you can limit yourself to setting the indexes and saving any
 *  information you will need later. Then use the saved information in
 *  profileTable_row_prep() for populating data.
 *
 * @note
 *  If you need consistency between rows (like you want statistics
 *  for each row to be from the same time frame), you should set all
 *  data here.
 *
 */
int
profileTable_container_load(netsnmp_container *container)
{
	profileTable_rowreq_ctx *rowreq_ctx;
	long   tblid;
	st_dbsProfile row;

	//printf("-->profileTable_container_load()\n");
	for( tblid=1; tblid<=MAX_CNU_AMOUNT_LIMIT; tblid++ )
	{
		/* 从数据库获取数据*/
		if( CMM_SUCCESS != dbsGetProfile(dbsdev, tblid,  &row) )
		{
			dbs_sys_log(dbsdev, DBS_LOG_ERR, "profileTable_container_load dbsGetProfile failed");
			return MFD_RESOURCE_UNAVAILABLE;
		}

		rowreq_ctx = profileTable_allocate_rowreq_ctx(NULL);
		
		if (NULL == rowreq_ctx)
		{
			snmp_log(LOG_ERR, "memory allocation failed\n");
			return MFD_RESOURCE_UNAVAILABLE;
		}
		
		if( MFD_SUCCESS != profileTable_indexes_set(rowreq_ctx, tblid)) 
		{
			snmp_log(LOG_ERR,"error setting index while loading " "profileTable data.\n");
			profileTable_release_rowreq_ctx(rowreq_ctx);
			continue;
		}

		rowreq_ctx->data.proIndex = row.id;
		rowreq_ctx->data.proBase = row.col_base;
		rowreq_ctx->data.proMacLimit = row.col_macLimit;
		rowreq_ctx->data.proCableUplinkRate = row.col_curate;
		rowreq_ctx->data.proCableDownlinkRate = row.col_cdrate;
		rowreq_ctx->data.proLocalAgingTime = row.col_loagTime;
		rowreq_ctx->data.proRemoteAgingTime = row.col_reagTime;
		rowreq_ctx->data.proIgmpPri = row.col_igmpPri;
		rowreq_ctx->data.proUnicastPri = row.col_unicastPri;
		rowreq_ctx->data.proAvsPri = row.col_avsPri;
		rowreq_ctx->data.proMcastPri = row.col_mcastPri;
		rowreq_ctx->data.proTbaPriSts = row.col_tbaPriSts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proCosPriSts = row.col_cosPriSts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proCos0pri = row.col_cos0pri;
		rowreq_ctx->data.proCos1pri = row.col_cos1pri;
		rowreq_ctx->data.proCos2pri = row.col_cos2pri;
		rowreq_ctx->data.proCos3pri = row.col_cos3pri;
		rowreq_ctx->data.proCos4pri = row.col_cos4pri;
		rowreq_ctx->data.proCos5pri = row.col_cos5pri;
		rowreq_ctx->data.proCos6pri = row.col_cos6pri;
		rowreq_ctx->data.proCos7pri = row.col_cos7pri;
		rowreq_ctx->data.proTosPriSts = row.col_tosPriSts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proTos0pri = row.col_tos0pri;
		rowreq_ctx->data.proTos1pri = row.col_tos1pri;
		rowreq_ctx->data.proTos2pri = row.col_tos2pri;
		rowreq_ctx->data.proTos3pri = row.col_tos3pri;
		rowreq_ctx->data.proTos4pri = row.col_tos4pri;
		rowreq_ctx->data.proTos5pri = row.col_tos5pri;
		rowreq_ctx->data.proTos6pri = row.col_tos6pri;
		rowreq_ctx->data.proTos7pri = row.col_tos7pri;
		rowreq_ctx->data.proSfbSts = row.col_sfbSts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proSfuSts = row.col_sfuSts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proSfmSts = row.col_sfmSts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proSfRate = row.col_sfRate;
		rowreq_ctx->data.proVlanStatus = row.col_vlanSts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proEth1vid = row.col_eth1vid;
		rowreq_ctx->data.proEth2vid = row.col_eth2vid;
		rowreq_ctx->data.proEth3vid = row.col_eth3vid;
		rowreq_ctx->data.proEth4vid = row.col_eth4vid;
		rowreq_ctx->data.proPortPriSts = row.col_portPriSts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proEth1pri = row.col_eth1pri;
		rowreq_ctx->data.proEth2pri = row.col_eth2pri;
		rowreq_ctx->data.proEth3pri = row.col_eth3pri;
		rowreq_ctx->data.proEth4pri = row.col_eth4pri;
		rowreq_ctx->data.proRxLimitSts = row.col_rxLimitSts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proCpuPortRxRate = row.col_cpuPortRxRate;
		rowreq_ctx->data.proEth1rx = row.col_eth1rx;
		rowreq_ctx->data.proEth2rx = row.col_eth2rx;
		rowreq_ctx->data.proEth3rx = row.col_eth3rx;
		rowreq_ctx->data.proEth4rx = row.col_eth4rx;
		rowreq_ctx->data.proTxLimitSts = row.col_txLimitSts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proCpuPortTxRate = row.col_cpuPortTxRate;
		rowreq_ctx->data.proEth1tx = row.col_eth1tx;
		rowreq_ctx->data.proEth2tx = row.col_eth2tx;
		rowreq_ctx->data.proEth3tx = row.col_eth3tx;
		rowreq_ctx->data.proEth4tx = row.col_eth4tx;
		rowreq_ctx->data.proPsctlSts = row.col_psctlSts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proCpuPortSts = row.col_cpuPortSts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proEth1sts = row.col_eth1sts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proEth2sts = row.col_eth2sts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proEth3sts = row.col_eth3sts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proEth4sts = row.col_eth4sts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;
		rowreq_ctx->data.proCommit = TRUTHVALUE_FALSE;
		rowreq_ctx->data.proRowStatus = row.col_row_sts?TRUTHVALUE_TRUE:TRUTHVALUE_FALSE;

		CONTAINER_INSERT(container, rowreq_ctx);
		//usleep(5000);
	}

	return MFD_SUCCESS;
} /* profileTable_container_load */


/**
 * container clean up
 *
 * @param container container with all current items
 *
 *  This optional callback is called prior to all
 *  item's being removed from the container. If you
 *  need to do any processing before that, do it here.
 *
 * @note
 *  The MFD helper will take care of releasing all the row contexts.
 *
 */
void
profileTable_container_free(netsnmp_container *container)
{
    DEBUGMSGTL(("verbose:profileTable:profileTable_container_free","called\n"));
    //printf("-->profileTable_container_free()\n");
    /*
     * TODO:380:M: Free profileTable container data.
     */
} /* profileTable_container_free */

/**
 * prepare row for processing.
 *
 *  When the agent has located the row for a request, this function is
 *  called to prepare the row for processing. If you fully populated
 *  the data context during the index setup phase, you may not need to
 *  do anything.
 *
 * @param rowreq_ctx pointer to a context.
 *
 * @retval MFD_SUCCESS     : success.
 * @retval MFD_ERROR       : other error.
 */
int
profileTable_row_prep( profileTable_rowreq_ctx *rowreq_ctx)
{
    DEBUGMSGTL(("verbose:profileTable:profileTable_row_prep","called\n"));
    //printf("-->profileTable_row_prep()\n");
    netsnmp_assert(NULL != rowreq_ctx);

    /*
     * TODO:390:o: Prepare row for request.
     * If populating row data was delayed, this is the place to
     * fill in the row for this request.
     */

    return MFD_SUCCESS;
} /* profileTable_row_prep */

/** @} */
