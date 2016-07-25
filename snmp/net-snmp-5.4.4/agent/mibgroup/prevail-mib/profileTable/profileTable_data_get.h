/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 12088 $ of $
 *
 * $Id:$
 *
 * @file profileTable_data_get.h
 *
 * @addtogroup get
 *
 * Prototypes for get functions
 *
 * @{
 */
#ifndef PROFILETABLE_DATA_GET_H
#define PROFILETABLE_DATA_GET_H

#ifdef __cplusplus
extern "C" {
#endif

/* *********************************************************************
 * GET function declarations
 */

/* *********************************************************************
 * GET Table declarations
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
    /*
     * indexes
     */

    int proIndex_get( profileTable_rowreq_ctx *rowreq_ctx, long * proIndex_val_ptr );
    int proBase_get( profileTable_rowreq_ctx *rowreq_ctx, long * proBase_val_ptr );
    int proMacLimit_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proMacLimit_val_ptr );
    int proCableUplinkRate_get( profileTable_rowreq_ctx *rowreq_ctx, long * proCableUplinkRate_val_ptr );
    int proCableDownlinkRate_get( profileTable_rowreq_ctx *rowreq_ctx, long * proCableDownlinkRate_val_ptr );
    int proLocalAgingTime_get( profileTable_rowreq_ctx *rowreq_ctx, long * proLocalAgingTime_val_ptr );
    int proRemoteAgingTime_get( profileTable_rowreq_ctx *rowreq_ctx, long * proRemoteAgingTime_val_ptr );
    int proIgmpPri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proIgmpPri_val_ptr );
    int proUnicastPri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proUnicastPri_val_ptr );
    int proAvsPri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proAvsPri_val_ptr );
    int proMcastPri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proMcastPri_val_ptr );
    int proTbaPriSts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proTbaPriSts_val_ptr );
    int proCosPriSts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proCosPriSts_val_ptr );
    int proCos0pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proCos0pri_val_ptr );
    int proCos1pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proCos1pri_val_ptr );
    int proCos2pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proCos2pri_val_ptr );
    int proCos3pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proCos3pri_val_ptr );
    int proCos4pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proCos4pri_val_ptr );
    int proCos5pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proCos5pri_val_ptr );
    int proCos6pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proCos6pri_val_ptr );
    int proCos7pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proCos7pri_val_ptr );
    int proTosPriSts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proTosPriSts_val_ptr );
    int proTos0pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proTos0pri_val_ptr );
    int proTos1pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proTos1pri_val_ptr );
    int proTos2pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proTos2pri_val_ptr );
    int proTos3pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proTos3pri_val_ptr );
    int proTos4pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proTos4pri_val_ptr );
    int proTos5pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proTos5pri_val_ptr );
    int proTos6pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proTos6pri_val_ptr );
    int proTos7pri_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proTos7pri_val_ptr );
    int proSfbSts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proSfbSts_val_ptr );
    int proSfuSts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proSfuSts_val_ptr );
    int proSfmSts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proSfmSts_val_ptr );
    int proSfRate_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proSfRate_val_ptr );
    int proVlanStatus_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proVlanStatus_val_ptr );
    int proEth1vid_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth1vid_val_ptr );
    int proEth2vid_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth2vid_val_ptr );
    int proEth3vid_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth3vid_val_ptr );
    int proEth4vid_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth4vid_val_ptr );
    int proPortPriSts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proPortPriSts_val_ptr );
    int proEth1pri_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth1pri_val_ptr );
    int proEth2pri_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth2pri_val_ptr );
    int proEth3pri_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth3pri_val_ptr );
    int proEth4pri_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth4pri_val_ptr );
    int proRxLimitSts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proRxLimitSts_val_ptr );
    int proCpuPortRxRate_get( profileTable_rowreq_ctx *rowreq_ctx, long * proCpuPortRxRate_val_ptr );
    int proEth1rx_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth1rx_val_ptr );
    int proEth2rx_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth2rx_val_ptr );
    int proEth3rx_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth3rx_val_ptr );
    int proEth4rx_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth4rx_val_ptr );
    int proTxLimitSts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proTxLimitSts_val_ptr );
    int proCpuPortTxRate_get( profileTable_rowreq_ctx *rowreq_ctx, long * proCpuPortTxRate_val_ptr );
    int proEth1tx_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth1tx_val_ptr );
    int proEth2tx_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth2tx_val_ptr );
    int proEth3tx_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth3tx_val_ptr );
    int proEth4tx_get( profileTable_rowreq_ctx *rowreq_ctx, long * proEth4tx_val_ptr );
    int proPsctlSts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proPsctlSts_val_ptr );
    int proCpuPortSts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proCpuPortSts_val_ptr );
    int proEth1sts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proEth1sts_val_ptr );
    int proEth2sts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proEth2sts_val_ptr );
    int proEth3sts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proEth3sts_val_ptr );
    int proEth4sts_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proEth4sts_val_ptr );
    int proCommit_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proCommit_val_ptr );
    int proRowStatus_get( profileTable_rowreq_ctx *rowreq_ctx, u_long * proRowStatus_val_ptr );


int profileTable_indexes_set_tbl_idx(profileTable_mib_index *tbl_idx, long proTid_val);
int profileTable_indexes_set(profileTable_rowreq_ctx *rowreq_ctx, long proTid_val);




#ifdef __cplusplus
}
#endif

#endif /* PROFILETABLE_DATA_GET_H */
/** @} */
