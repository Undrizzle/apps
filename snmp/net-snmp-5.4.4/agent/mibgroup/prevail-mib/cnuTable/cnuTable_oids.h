/*
 * Note: this file originally auto-generated by mib2c using
 *  : generic-table-oids.m2c 17548 2009-04-23 16:35:18Z hardaker $
 *
 * $Id:$
 */
#ifndef CNUTABLE_OIDS_H
#define CNUTABLE_OIDS_H

#ifdef __cplusplus
extern "C" {
#endif


/* column number definitions for table cnuTable */
#define CNUTABLE_OID              1,3,6,1,4,1,36186,8,1


#define COLUMN_CNUTID         1
    
#define COLUMN_CNUINDEX         2
    
#define COLUMN_CNUMODEL         3
    
#define COLUMN_CNUMACADDRESS         4
    
#define COLUMN_CNUONLINESTATUS         5
    
#define COLUMN_CNUAUTHORIZED         6
    
#define COLUMN_CNUSOFTWAREVERSION         7
    
#define COLUMN_CNURXRATE         8
    
#define COLUMN_CNUTXRATE         9
    
#define COLUMN_CNUSNR         10
    
#define COLUMN_CNUBPC         11
    
#define COLUMN_CNUATTENUATION         12
    
#define COLUMN_CNUACTION         13
#define COLUMN_CNUACTION_FLAG    (0x1 << 0)
    
#define COLUMN_CNUSYNCHRONIZED         14
    
#define COLUMN_CNUROWSTATUS         15
    

#define CNUTABLE_MIN_COL   COLUMN_CNUTID
#define CNUTABLE_MAX_COL   COLUMN_CNUROWSTATUS
    

    /*
     * TODO:405:r: Review CNUTABLE_SETTABLE_COLS macro.
     * OR together all the writable cols.
     */
#define CNUTABLE_SETTABLE_COLS (COLUMN_CNUACTION_FLAG)

#ifdef __cplusplus
}
#endif

#endif /* CNUTABLE_OIDS_H */