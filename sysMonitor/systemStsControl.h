#ifndef __SYSTEM_STATUS_CONTROL_H__
#define __SYSTEM_STATUS_CONTROL_H__

#include <stdio.h>

int get_systemStatus(void);
void set_systemStatus(int status);

/* 注销g_systemStatus 互斥访问的互斥锁*/
void destroy_systemStatusLock(void);

/* 初始化g_systemStatus 互斥访问的互斥锁*/
void init_systemStatusLock(void);

#endif 
