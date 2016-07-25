#ifndef __IF_CUI_WEB_H__
#define __IF_CUI_WEB_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "cgimain.h"
#include <secdefs.h>

void BcmWeb_getAllInfo(PWEB_NTWK_VAR pWebVar);
void BcmWeb_initRtl8306eSettings(PWEB_NTWK_VAR pWebVar);

#endif

