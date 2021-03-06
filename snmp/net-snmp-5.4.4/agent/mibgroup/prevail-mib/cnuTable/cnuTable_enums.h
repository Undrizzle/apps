/*
 * Note: this file originally auto-generated by mib2c using
 *  : generic-table-enums.m2c 12526 2005-07-15 22:41:16Z rstory $
 *
 * $Id:$
 */
#ifndef CNUTABLE_ENUMS_H
#define CNUTABLE_ENUMS_H

#ifdef __cplusplus
extern "C" {
#endif

 /*
 * NOTES on enums
 * ==============
 *
 * Value Mapping
 * -------------
 * If the values for your data type don't exactly match the
 * possible values defined by the mib, you should map them
 * below. For example, a boolean flag (1/0) is usually represented
 * as a TruthValue in a MIB, which maps to the values (1/2).
 *
 */
/*************************************************************************
 *************************************************************************
 *
 * enum definitions for table cnuTable
 *
 *************************************************************************
 *************************************************************************/

/*************************************************************
 * constants for enums for the MIB node
 * cnuModel (DevModelValue / ASN_INTEGER)
 *
 * since a Textual Convention may be referenced more than once in a
 * MIB, protect againt redefinitions of the enum values.
 */
#ifndef DEVMODELVALUE_ENUMS
#define DEVMODELVALUE_ENUMS

#define DEVMODELVALUE_X7  1 
#define DEVMODELVALUE_E31  2 
#define DEVMODELVALUE_Q31  3 
#define DEVMODELVALUE_C22  4 
#define DEVMODELVALUE_S220  5 
#define DEVMODELVALUE_S60  6 
#define DEVMODELVALUE_L2  7 
#define DEVMODELVALUE_L3  8 
#define DEVMODELVALUE_C2  9 
#define DEVMODELVALUE_C4  10
#define DEVMODELVALUE_CLT1  11
#define DEVMODELVALUE_UNKNOWN  256 

#endif /* DEVMODELVALUE_ENUMS */


/*************************************************************
 * constants for enums for the MIB node
 * cnuOnlineStatus (TruthValue / ASN_INTEGER)
 *
 * since a Textual Convention may be referenced more than once in a
 * MIB, protect againt redefinitions of the enum values.
 */
#ifndef TRUTHVALUE_ENUMS
#define TRUTHVALUE_ENUMS

#define TRUTHVALUE_TRUE  1 
#define TRUTHVALUE_FALSE  2 

#endif /* TRUTHVALUE_ENUMS */


/*************************************************************
 * constants for enums for the MIB node
 * cnuAuthorized (TruthValue / ASN_INTEGER)
 *
 * since a Textual Convention may be referenced more than once in a
 * MIB, protect againt redefinitions of the enum values.
 */
#ifndef TRUTHVALUE_ENUMS
#define TRUTHVALUE_ENUMS

#define TRUTHVALUE_TRUE  1 
#define TRUTHVALUE_FALSE  2 

#endif /* TRUTHVALUE_ENUMS */


/*************************************************************
 * constants for enums for the MIB node
 * cnuAction (CnuRstActValue / ASN_INTEGER)
 *
 * since a Textual Convention may be referenced more than once in a
 * MIB, protect againt redefinitions of the enum values.
 */
#ifndef CNURSTACTVALUE_ENUMS
#define CNURSTACTVALUE_ENUMS

#define CNURSTACTVALUE_CNU_NO_ACTION  0 
#define CNURSTACTVALUE_CNU_RESET  1 
#define CNURSTACTVALUE_CNU_RELOAD_PROFILE  2 
#define CNURSTACTVALUE_CNU_PERMIT  3 
#define CNURSTACTVALUE_CNU_UNDO_PERMIT  4 
#define CNURSTACTVALUE_CNU_DIAGNOSE  5 
#define CNURSTACTVALUE_CNU_DELETE  6 

#endif /* CNURSTACTVALUE_ENUMS */


/*************************************************************
 * constants for enums for the MIB node
 * cnuSynchronized (TruthValue / ASN_INTEGER)
 *
 * since a Textual Convention may be referenced more than once in a
 * MIB, protect againt redefinitions of the enum values.
 */
#ifndef TRUTHVALUE_ENUMS
#define TRUTHVALUE_ENUMS

#define TRUTHVALUE_TRUE  1 
#define TRUTHVALUE_FALSE  2 

#endif /* TRUTHVALUE_ENUMS */


/*************************************************************
 * constants for enums for the MIB node
 * cnuRowStatus (TruthValue / ASN_INTEGER)
 *
 * since a Textual Convention may be referenced more than once in a
 * MIB, protect againt redefinitions of the enum values.
 */
#ifndef TRUTHVALUE_ENUMS
#define TRUTHVALUE_ENUMS

#define TRUTHVALUE_TRUE  1 
#define TRUTHVALUE_FALSE  2 

#endif /* TRUTHVALUE_ENUMS */




#ifdef __cplusplus
}
#endif

#endif /* CNUTABLE_ENUMS_H */
