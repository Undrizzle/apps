/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 14170 $ of $
 *
 * $Id:$
 */
#ifndef CLTPROTABLE_DATA_ACCESS_H
#define CLTPROTABLE_DATA_ACCESS_H

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
 *** Table cltproTable
 ***
 **********************************************************************
 **********************************************************************/
/*
 * prevail-mib::cltproTable is subid 10 of modEoCMib.
 * Its status is Current.
 * OID: .1.3.6.1.4.1.36186.8.10, length: 9
*/


    int cltproTable_init_data(cltproTable_registration * cltproTable_reg);


    /*
     * TODO:180:o: Review cltproTable cache timeout.
     * The number of seconds before the cache times out
     */
#define CLTPROTABLE_CACHE_TIMEOUT   60

void cltproTable_container_init(netsnmp_container **container_ptr_ptr,
                             netsnmp_cache *cache);
void cltproTable_container_shutdown(netsnmp_container *container_ptr);

int cltproTable_container_load(netsnmp_container *container);
void cltproTable_container_free(netsnmp_container *container);

int cltproTable_cache_load(netsnmp_container *container);
void cltproTable_cache_free(netsnmp_container *container);

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
    int cltproTable_row_prep( cltproTable_rowreq_ctx *rowreq_ctx);



#ifdef __cplusplus
}
#endif

#endif /* CLTPROTABLE_DATA_ACCESS_H */
