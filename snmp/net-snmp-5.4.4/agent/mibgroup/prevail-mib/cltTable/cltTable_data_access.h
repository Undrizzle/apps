/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 14170 $ of $
 *
 * $Id:$
 */
#ifndef CLTTABLE_DATA_ACCESS_H
#define CLTTABLE_DATA_ACCESS_H

#ifdef __cplusplus
extern "C" {
#endif


/* *********************************************************************
 * function declarations
 */

/* *********************************************************************
 * Table declarations
 */
/**********************************************************************
 **********************************************************************
 ***
 *** Table cltTable
 ***
 **********************************************************************
 **********************************************************************/
/*
 * prevail-mib::cltTable is subid 9 of modEoCMib.
 * Its status is Current.
 * OID: .1.3.6.1.4.1.36186.8.9, length: 9
*/


    int cltTable_init_data(cltTable_registration * cltTable_reg);


    /*
     * TODO:180:o: Review cltTable cache timeout.
     * The number of seconds before the cache times out
     */
#define CLTTABLE_CACHE_TIMEOUT   60

void cltTable_container_init(netsnmp_container **container_ptr_ptr,
                             netsnmp_cache *cache);
void cltTable_container_shutdown(netsnmp_container *container_ptr);

int cltTable_container_load(netsnmp_container *container);
void cltTable_container_free(netsnmp_container *container);

int cltTable_cache_load(netsnmp_container *container);
void cltTable_cache_free(netsnmp_container *container);

    /*
    ***************************************************
    ***             START EXAMPLE CODE              ***
    ***---------------------------------------------***/
/* *********************************************************************
 * Since we have no idea how you really access your data, we'll go with
 * a worst case example: a flat text file.
 */
#define MAX_LINE_SIZE 256
    /*
    ***---------------------------------------------***
    ***              END  EXAMPLE CODE              ***
    ***************************************************/
    int cltTable_row_prep( cltTable_rowreq_ctx *rowreq_ctx);



#ifdef __cplusplus
}
#endif

#endif /* CLTTABLE_DATA_ACCESS_H */
