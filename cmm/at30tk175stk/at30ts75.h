#ifndef __AT30TS75_H__
#define __AT30TS75_H__

#include <public.h>

uint8_t at30ts75_read_temperature(st_temperature *temp_data);
int get_at30ts75_init_status(void);
void uninit_at30ts75(void);
int init_at30ts75(void);

#endif
