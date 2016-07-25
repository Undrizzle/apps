#ifndef __TM_REGISTER_H__
#define __TM_REGISTER_H__

#include <public.h>

void tm2reg_send_notification(stRegEvent *regEvent);
int tm2reg_destroy(void);
int tm2reg_init(void);

#endif

