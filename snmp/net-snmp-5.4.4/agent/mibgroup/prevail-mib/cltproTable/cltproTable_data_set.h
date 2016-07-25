/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 12077 $ of $ 
 *
 * $Id:$
 */
#ifndef CLTPROTABLE_DATA_SET_H
#define CLTPROTABLE_DATA_SET_H

#ifdef __cplusplus
extern "C" {
#endif

/* *********************************************************************
 * SET function declarations
 */

/* *********************************************************************
 * SET Table declarations
 */
/**********************************************************************
 **********************************************************************
 ***
 *** Table cltproTable
 ***
 **********************************************************************
 **********************************************************************/
/*
 * prevail-mib::cltproTable is subid 10 of modEoCMib.
 * Its status is Current.
 * OID: .1.3.6.1.4.1.36186.8.10, length: 9
*/


int cltproTable_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx);
int cltproTable_undo_cleanup( cltproTable_rowreq_ctx *rowreq_ctx);
int cltproTable_undo( cltproTable_rowreq_ctx *rowreq_ctx);
int cltproTable_commit( cltproTable_rowreq_ctx *rowreq_ctx);
int cltproTable_undo_commit( cltproTable_rowreq_ctx *rowreq_ctx);


int cltproIndex_check_value( cltproTable_rowreq_ctx *rowreq_ctx, long cltproIndex_val);
int cltproIndex_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproIndex_set( cltproTable_rowreq_ctx *rowreq_ctx, long cltproIndex_val );
int cltproIndex_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproBase_check_value( cltproTable_rowreq_ctx *rowreq_ctx, long cltproBase_val);
int cltproBase_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproBase_set( cltproTable_rowreq_ctx *rowreq_ctx, long cltproBase_val );
int cltproBase_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproMacLimit_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproMacLimit_val);
int cltproMacLimit_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproMacLimit_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproMacLimit_val );
int cltproMacLimit_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproCuRate_check_value( cltproTable_rowreq_ctx *rowreq_ctx, long cltproCuRate_val);
int cltproCuRate_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproCuRate_set( cltproTable_rowreq_ctx *rowreq_ctx, long cltproCuRate_val );
int cltproCuRate_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproCdRate_check_value( cltproTable_rowreq_ctx *rowreq_ctx, long cltproCdRate_val);
int cltproCdRate_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproCdRate_set( cltproTable_rowreq_ctx *rowreq_ctx, long cltproCdRate_val );
int cltproCdRate_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproLoagTime_check_value( cltproTable_rowreq_ctx *rowreq_ctx, long cltproLoagTime_val);
int cltproLoagTime_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproLoagTime_set( cltproTable_rowreq_ctx *rowreq_ctx, long cltproLoagTime_val );
int cltproLoagTime_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproReagTime_check_value( cltproTable_rowreq_ctx *rowreq_ctx, long cltproReagTime_val);
int cltproReagTime_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproReagTime_set( cltproTable_rowreq_ctx *rowreq_ctx, long cltproReagTime_val );
int cltproReagTime_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproIgmpPri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproIgmpPri_val);
int cltproIgmpPri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproIgmpPri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproIgmpPri_val );
int cltproIgmpPri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproUnicastPri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproUnicastPri_val);
int cltproUnicastPri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproUnicastPri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproUnicastPri_val );
int cltproUnicastPri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproAvsPri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproAvsPri_val);
int cltproAvsPri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproAvsPri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproAvsPri_val );
int cltproAvsPri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproMcastPri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproMcastPri_val);
int cltproMcastPri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproMcastPri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproMcastPri_val );
int cltproMcastPri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproTbaPriSts_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTbaPriSts_val);
int cltproTbaPriSts_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproTbaPriSts_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTbaPriSts_val );
int cltproTbaPriSts_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproCosPriSts_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCosPriSts_val);
int cltproCosPriSts_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproCosPriSts_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCosPriSts_val );
int cltproCosPriSts_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproCos0pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos0pri_val);
int cltproCos0pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproCos0pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos0pri_val );
int cltproCos0pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproCos1pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos1pri_val);
int cltproCos1pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproCos1pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos1pri_val );
int cltproCos1pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproCos2pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos2pri_val);
int cltproCos2pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproCos2pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos2pri_val );
int cltproCos2pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproCos3pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos3pri_val);
int cltproCos3pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproCos3pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos3pri_val );
int cltproCos3pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproCos4pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos4pri_val);
int cltproCos4pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproCos4pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos4pri_val );
int cltproCos4pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproCos5pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos5pri_val);
int cltproCos5pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproCos5pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos5pri_val );
int cltproCos5pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproCos6pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos6pri_val);
int cltproCos6pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproCos6pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos6pri_val );
int cltproCos6pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproCos7pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos7pri_val);
int cltproCos7pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproCos7pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCos7pri_val );
int cltproCos7pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproTosPriSts_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTosPriSts_val);
int cltproTosPriSts_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproTosPriSts_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTosPriSts_val );
int cltproTosPriSts_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproTos0pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos0pri_val);
int cltproTos0pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproTos0pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos0pri_val );
int cltproTos0pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproTos1pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos1pri_val);
int cltproTos1pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproTos1pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos1pri_val );
int cltproTos1pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproTos2pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos2pri_val);
int cltproTos2pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproTos2pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos2pri_val );
int cltproTos2pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproTos3pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos3pri_val);
int cltproTos3pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproTos3pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos3pri_val );
int cltproTos3pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproTos4pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos4pri_val);
int cltproTos4pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproTos4pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos4pri_val );
int cltproTos4pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproTos5pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos5pri_val);
int cltproTos5pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproTos5pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos5pri_val );
int cltproTos5pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproTos6pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos6pri_val);
int cltproTos6pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproTos6pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos6pri_val );
int cltproTos6pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproTos7pri_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos7pri_val);
int cltproTos7pri_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproTos7pri_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproTos7pri_val );
int cltproTos7pri_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproCommit_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCommit_val);
int cltproCommit_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproCommit_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproCommit_val );
int cltproCommit_undo( cltproTable_rowreq_ctx *rowreq_ctx );

int cltproRowStatus_check_value( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproRowStatus_val);
int cltproRowStatus_undo_setup( cltproTable_rowreq_ctx *rowreq_ctx );
int cltproRowStatus_set( cltproTable_rowreq_ctx *rowreq_ctx, u_long cltproRowStatus_val );
int cltproRowStatus_undo( cltproTable_rowreq_ctx *rowreq_ctx );


int cltproTable_check_dependencies(cltproTable_rowreq_ctx *ctx);


#ifdef __cplusplus
}
#endif

#endif /* CLTPROTABLE_DATA_SET_H */