#include <madCopyright.h>

/********************************************************************************
* madApiInternal.h
*
* DESCRIPTION:
*       Definitions for Driver's Internal Use
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __MADApiInternal_h
#define __MADApiInternal_h

#ifdef __cplusplus
extern "C" {
#endif

/* The following macro converts a binary    */
/* value (of 1 bit) to a boolean one.       */
/* 0 --> MAD_FALSE                           */
/* 1 --> MAD_TRUE                            */
#define MAD_BIT_2_BOOL(binVal,boolVal)                                  \
            (boolVal) = (((binVal) == 0) ? MAD_FALSE : MAD_TRUE)

/* The following macro converts a boolean   */
/* value to a binary one (of 1 bit).        */
/* MAD_FALSE --> 0                           */
/* MAD_TRUE --> 1                            */
#define MAD_BOOL_2_BIT(boolVal,binVal)                                  \
            (binVal) = (((boolVal) == MAD_TRUE) ? 1 : 0)

/* macro to convert VCT register value to actual distance */
#define MAD_VCT_CALC(_data)        \
        (((long)(_data)*8018 - 287510)/10000 + (((((long)(_data)*8018 - 287510)%10000) >= 5000)?1:0))

#define MAD_VCT_3016_CALC(_data)        \
        (((long)(_data)*7861 - 188620)/10000 + (((((long)(_data)*7861 - 188620)%10000) >= 5000)?1:0))

#define MAD_ADV_VCT_CALC(_data)        \
        (((long)(_data)*8333 - 191667)/10000 + (((((long)(_data)*8333 - 191667)%10000) >= 5000)?1:0))
/*        (((long)(_data)*8049 - 230000)/10000 + (((((long)(_data)*8049 - 230000)%10000) >= 5000)?1:0)) */

#define MAD_ADV_VCT_CALC_SHORT(_data)        \
        (((long)(_data)*7143 - 71429)/10000 + (((((long)(_data)*7143 - 71429)%10000) >= 5000)?1:0)) 
/*        (((long)(_data)*6923 - 121538)/10000 + (((((long)(_data)*6923 - 121538)%10000) >= 5000)?1:0))  */

/* macro to check VCT Failure */
#define IS_VCT_FAILED(_reg)        \
        (((_reg) & 0xFF) == 0xFF)

/* macro to find out if Amplitude is zero */
#define IS_ZERO_AMPLITUDE(_reg)    \
        (((_reg) & 0x7F00) == 0)

/* macro to retrieve Amplitude */
#define GET_AMPLITUDE(_reg)    \
        (((_reg) & 0x7F00) >> 8)
        
/* macro to find out if Amplitude is positive */
#define IS_POSITIVE_AMPLITUDE(_reg)    \
        (((_reg) & 0x8000) == 0x8000)

typedef struct _VCT_REGISTER
{
    MAD_U8    page;
    MAD_U8    regOffset;
} VCT_REGISTER;


#define MAD_DEV_CAPABILITY(dev, _cap)   (dev->phyInfo.featureSet & (_cap))
#define MAD_DEV_CAPABILITY1(dev, _cap)   (dev->phyInfo.featureSet1 & (_cap))
#define MAD_DEV_HWMODE(dev, _port, _mode) (dev->phyInfo.hwMode[_port] & (_mode))
#define MAD_DEV_HWREALMODE(dev, _port, _mode) (dev->phyInfo.hwRealMode[_port] & (_mode))

#define MAD_DEV_PORT_CHECK(dev, _port)   (dev->numOfPorts > _port)

/*******************************************************************************
* MADDbgPrint
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*
* COMMENTS:
*       None
*
*******************************************************************************/
void MADDbgPrint(char* format, ...);


/*******************************************************************************
* madSemRegister
*
* DESCRIPTION:
*       Assign Semaphore functions to the given semaphore set.
*        Driver maintains its own memory for the structure.
*
* INPUTS:
*        semFunctions - pointer to the MAD_SEM_ROUTINES
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madSemRegister
(
    IN  MAD_DEV*       dev,
    IN  MAD_SEM_ROUTINES* semRoutines
);


MAD_STATUS madGetHwMode
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    OUT MAD_U16     *hwMode
);

#define MAD_PTP_BUILD_TIME(_time1, _time2)       (((_time1) << 16) | (_time2))
#define MAD_PTP_L16_TIME(_time1) ((_time1) & 0xFFFF)
#define MAD_PTP_H16_TIME(_time1) (((_time1) >> 16) & 0xFFFF)

void madVCTDataInit(MAD_DEV*        dev);

#define MAD_LPORTVEC_2_PORTVEC(_lvec)      (MAD_U8)((_lvec) & 0xffff)
#define MAD_PORTVEC_2_LPORTVEC(_pvec)       (MAD_32)((_pvec) & 0xffff)

#ifdef __cplusplus
}
#endif

#endif /* __madApiInternal_h */
