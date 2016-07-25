#ifndef __HTTPD_SYSMONITOR_MSG_H__
#define __HTTPD_SYSMONITOR_MSG_H__

#include <public.h>

int httpd2sysmonitor_sysledCtrol(uint8_t event);
int httpd2sysmonitor_init(void);
int httpd2sysmonitor_destroy(void);

#endif

