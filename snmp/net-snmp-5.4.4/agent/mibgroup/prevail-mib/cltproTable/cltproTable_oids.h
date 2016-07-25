/*
 * Note: this file originally auto-generated by mib2c using
 *  : generic-table-oids.m2c 17548 2009-04-23 16:35:18Z hardaker $
 *
 * $Id:$
 */
#ifndef CLTPROTABLE_OIDS_H
#define CLTPROTABLE_OIDS_H

#ifdef __cplusplus
extern "C" {
#endif


/* column number definitions for table cltproTable */
#define CLTPROTABLE_OID              1,3,6,1,4,1,36186,8,10


#define COLUMN_CLTPROTID         1
    
#define COLUMN_CLTPROINDEX         2
    
#define COLUMN_CLTPROBASE         3
#define COLUMN_CLTPROBASE_FLAG    (0x1 << 0)
    
#define COLUMN_CLTPROMACLIMIT         4
#define COLUMN_CLTPROMACLIMIT_FLAG    (0x1 << 1)
    
#define COLUMN_CLTPROCURATE         5
#define COLUMN_CLTPROCURATE_FLAG    (0x1 << 2)
    
#define COLUMN_CLTPROCDRATE         6
#define COLUMN_CLTPROCDRATE_FLAG    (0x1 << 3)
    
#define COLUMN_CLTPROLOAGTIME         7
#define COLUMN_CLTPROLOAGTIME_FLAG    (0x1 << 4)
    
#define COLUMN_CLTPROREAGTIME         8
#define COLUMN_CLTPROREAGTIME_FLAG    (0x1 << 5)
    
#define COLUMN_CLTPROIGMPPRI         9
#define COLUMN_CLTPROIGMPPRI_FLAG    (0x1 << 6)
    
#define COLUMN_CLTPROUNICASTPRI         10
#define COLUMN_CLTPROUNICASTPRI_FLAG    (0x1 << 7)
    
#define COLUMN_CLTPROAVSPRI         11
#define COLUMN_CLTPROAVSPRI_FLAG    (0x1 << 8)
    
#define COLUMN_CLTPROMCASTPRI         12
#define COLUMN_CLTPROMCASTPRI_FLAG    (0x1 << 9)
    
#define COLUMN_CLTPROTBAPRISTS         13
#define COLUMN_CLTPROTBAPRISTS_FLAG    (0x1 << 10)
    
#define COLUMN_CLTPROCOSPRISTS         14
#define COLUMN_CLTPROCOSPRISTS_FLAG    (0x1 << 11)
    
#define COLUMN_CLTPROCOS0PRI         15
#define COLUMN_CLTPROCOS0PRI_FLAG    (0x1 << 12)
    
#define COLUMN_CLTPROCOS1PRI         16
#define COLUMN_CLTPROCOS1PRI_FLAG    (0x1 << 13)
    
#define COLUMN_CLTPROCOS2PRI         17
#define COLUMN_CLTPROCOS2PRI_FLAG    (0x1 << 14)
    
#define COLUMN_CLTPROCOS3PRI         18
#define COLUMN_CLTPROCOS3PRI_FLAG    (0x1 << 15)
    
#define COLUMN_CLTPROCOS4PRI         19
#define COLUMN_CLTPROCOS4PRI_FLAG    (0x1 << 16)
    
#define COLUMN_CLTPROCOS5PRI         20
#define COLUMN_CLTPROCOS5PRI_FLAG    (0x1 << 17)
    
#define COLUMN_CLTPROCOS6PRI         21
#define COLUMN_CLTPROCOS6PRI_FLAG    (0x1 << 18)
    
#define COLUMN_CLTPROCOS7PRI         22
#define COLUMN_CLTPROCOS7PRI_FLAG    (0x1 << 19)
    
#define COLUMN_CLTPROTOSPRISTS         23
#define COLUMN_CLTPROTOSPRISTS_FLAG    (0x1 << 20)
    
#define COLUMN_CLTPROTOS0PRI         24
#define COLUMN_CLTPROTOS0PRI_FLAG    (0x1 << 21)
    
#define COLUMN_CLTPROTOS1PRI         25
#define COLUMN_CLTPROTOS1PRI_FLAG    (0x1 << 22)
    
#define COLUMN_CLTPROTOS2PRI         26
#define COLUMN_CLTPROTOS2PRI_FLAG    (0x1 << 23)
    
#define COLUMN_CLTPROTOS3PRI         27
#define COLUMN_CLTPROTOS3PRI_FLAG    (0x1 << 24)
    
#define COLUMN_CLTPROTOS4PRI         28
#define COLUMN_CLTPROTOS4PRI_FLAG    (0x1 << 25)
    
#define COLUMN_CLTPROTOS5PRI         29
#define COLUMN_CLTPROTOS5PRI_FLAG    (0x1 << 26)
    
#define COLUMN_CLTPROTOS6PRI         30
#define COLUMN_CLTPROTOS6PRI_FLAG    (0x1 << 27)
    
#define COLUMN_CLTPROTOS7PRI         31
#define COLUMN_CLTPROTOS7PRI_FLAG    (0x1 << 28)
    
#define COLUMN_CLTPROCOMMIT         32
#define COLUMN_CLTPROCOMMIT_FLAG    (0x1 << 29)
    
#define COLUMN_CLTPROROWSTATUS         33
    

#define CLTPROTABLE_MIN_COL   COLUMN_CLTPROTID
#define CLTPROTABLE_MAX_COL   COLUMN_CLTPROROWSTATUS
    

    /*
     * TODO:405:r: Review CLTPROTABLE_SETTABLE_COLS macro.
     * OR together all the writable cols.
     */
#define CLTPROTABLE_SETTABLE_COLS (COLUMN_CLTPROBASE_FLAG | COLUMN_CLTPROMACLIMIT_FLAG | COLUMN_CLTPROCURATE_FLAG | COLUMN_CLTPROCDRATE_FLAG | COLUMN_CLTPROLOAGTIME_FLAG | COLUMN_CLTPROREAGTIME_FLAG | COLUMN_CLTPROIGMPPRI_FLAG | COLUMN_CLTPROUNICASTPRI_FLAG | COLUMN_CLTPROAVSPRI_FLAG | COLUMN_CLTPROMCASTPRI_FLAG | COLUMN_CLTPROTBAPRISTS_FLAG | COLUMN_CLTPROCOSPRISTS_FLAG | COLUMN_CLTPROCOS0PRI_FLAG | COLUMN_CLTPROCOS1PRI_FLAG | COLUMN_CLTPROCOS2PRI_FLAG | COLUMN_CLTPROCOS3PRI_FLAG | COLUMN_CLTPROCOS4PRI_FLAG | COLUMN_CLTPROCOS5PRI_FLAG | COLUMN_CLTPROCOS6PRI_FLAG | COLUMN_CLTPROCOS7PRI_FLAG | COLUMN_CLTPROTOSPRISTS_FLAG | COLUMN_CLTPROTOS0PRI_FLAG | COLUMN_CLTPROTOS1PRI_FLAG | COLUMN_CLTPROTOS2PRI_FLAG | COLUMN_CLTPROTOS3PRI_FLAG | COLUMN_CLTPROTOS4PRI_FLAG | COLUMN_CLTPROTOS5PRI_FLAG | COLUMN_CLTPROTOS6PRI_FLAG | COLUMN_CLTPROTOS7PRI_FLAG | COLUMN_CLTPROCOMMIT_FLAG)

#ifdef __cplusplus
}
#endif

#endif /* CLTPROTABLE_OIDS_H */
