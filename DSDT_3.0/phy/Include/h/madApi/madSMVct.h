#include <madCopyright.h>

/********************************************************************************
* madSMVct.h
* 
* DESCRIPTION:
*       State Machine Definitions for VCT (TDR and DSP).
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __MADSMVCT_h
#define __MADSMVCT_h


typedef enum {
    MAD_VCT_SM_ST_FREE = 0,    /* VCT State Machine free state */
    MAD_VCT_SM_ST_INIT,    /* Initial state */
    MAD_VCT_SM_ST_PENDING,        /* Pending state*/
    MAD_VCT_SM_ST_PENDING_SUB,        /* Second Pending state */
    MAD_VCT_SM_ST_MAX
} MAD_VCT_SM_STATE;

typedef enum {
    MAD_VCT_SM_EVT_CMD = 0,    /* VCT Command event */
    MAD_VCT_SM_EVT_POLL,        /* Polling event */
    MAD_VCT_SM_EVT_MAX
} MAD_VCT_SM_EVT;

typedef enum {
    MAD_VCT_SM_VCT_TYPE_TDR = 0,    /* VCT TDR calling */
    MAD_VCT_SM_VCT_TYPE_ADV_TDR,        /* VCT Advaved TDR calling */
    MAD_VCT_SM_VCT_TYPE_DSP,        /* VCT DSP calling */
    MAD_VCT_SM_VCT_TYPE_ADV_DSP,        /* VCT Advaved DSP calling */
    MAD_VCT_SM_VCT_TYPE_MAX
} MAD_VCT_SM_VCT_TYPE;

typedef MAD_STATUS (*FMAD_VCT_SM_INIT)(MAD_DEV* dev, MAD_U8 port);
typedef MAD_STATUS (*FMAD_VCT_SM_SET)(MAD_DEV* dev, MAD_U8 port);
typedef MAD_STATUS (*FMAD_VCT_SM_CHECK)(MAD_DEV* dev, MAD_U8 port);
typedef MAD_STATUS (*FMAD_VCT_SM_GET)(MAD_DEV* dev, MAD_U8 port);
typedef MAD_STATUS (*FMAD_VCT_SM_CLOSE)(MAD_DEV* dev, MAD_U8 port);

typedef struct
{
    FMAD_VCT_SM_INIT    vctSmActInit;
    FMAD_VCT_SM_SET        vctSmActSet;
    FMAD_VCT_SM_CHECK   vctSmActCheck;
    FMAD_VCT_SM_GET        vctSmActGet;
    FMAD_VCT_SM_SET        vctSmActSubSet;
    FMAD_VCT_SM_GET        vctSmActSubGet;
    FMAD_VCT_SM_CLOSE    vctSmActClose;
}MAD_VCT_SM_ACTIONS;

typedef struct
{
    MAD_VCT_SM_STATE   vctSmSt;
    MAD_DEV*        dev;
    MAD_U8            port;
    MAD_VCT_SM_VCT_TYPE    currentVctType;
    MAD_BOOL        subPending;
    MAD_32            checkCount;
    MAD_32            subCheckCount;
    MAD_32            currCheckCount;
    MAD_VCT_SM_ACTIONS    vctSmAct;
    MAD_32            savedVeriable[8];
    MAD_VCT_DATA_STRUCT *vctResultPtr;
}MAD_VCT_SM_CFG;


void    madVctSmInit(void ** smCfgPtr);

MAD_STATUS madVCTFindSM(MAD_DEV*  dev, MAD_U8 port, MAD_VCT_SM_VCT_TYPE vctType,
                  MAD_VCT_SM_ACTIONS *actions, MAD_BOOL subPending,
                  MAD_VCT_DATA_STRUCT  *vctData, MAD_32 checkCount, 
                  MAD_32 subCheckCount);

MAD_STATUS madVCTImplSM(MAD_DEV*  dev, MAD_U8 port,    MAD_VCT_SM_EVT event);

void madVCTCloseSM(MAD_DEV*        dev);

#endif /* __MADSMVCT_H */
