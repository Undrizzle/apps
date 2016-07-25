#ifndef __NVM_UTILS_H__
#define __NVM_UTILS_H__

#include <stdio.h>
#include <public.h>

void nvm_dump(void);
int nvm_set_mt_parameters(stMTmsgInfo *para);
int nvm_get_parameter(const char *key, char *value);
int nvm_set_parameter(const char *key, const char *value);

#endif 
