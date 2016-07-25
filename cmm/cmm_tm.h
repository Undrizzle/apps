#ifndef __CMM_TM_H__
#define __CMM_TM_H__

#include <public.h>

int cmmTmDumpCnuReg(uint16_t cltId, uint16_t cnuId);
int cmmTmDumpCnuMod(uint16_t cltId, uint16_t cnuId);
int cmmTmDumpCnuPib(uint16_t cltId, uint16_t cnuId);
int cmmTmDoWlistControl(uint16_t status);
int cmmTmDeleteCnu(uint16_t cltId, uint16_t cnuId);
int cmmTmDoCnuPermit(uint16_t cltId, uint16_t cnuId);
int cmmTmUndoCnuPermit(uint16_t cltId, uint16_t cnuId);
int cmmTmGetCnuProfile(uint16_t cltId, uint16_t cnuId, st_dbsProfile* profile);
int cmmTmWriteCnuProfile(uint16_t cltId, uint16_t cnuId, st_dbsProfile* profile);
int destroy_cmm_tm(void);
int init_cmm_tm(void);

#endif

