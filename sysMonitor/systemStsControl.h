#ifndef __SYSTEM_STATUS_CONTROL_H__
#define __SYSTEM_STATUS_CONTROL_H__

#include <stdio.h>

int get_systemStatus(void);
void set_systemStatus(int status);

/* ע��g_systemStatus ������ʵĻ�����*/
void destroy_systemStatusLock(void);

/* ��ʼ��g_systemStatus ������ʵĻ�����*/
void init_systemStatusLock(void);

#endif 
