#include <stdio.h>
#include <pthread.h>
#include <public.h>
#include "systemStsControl.h"

/* 描述系统状态的全局变量*/
int g_systemStatus = SYSLED_STS_NORMAL;

pthread_mutex_t systemStatusLock;

int get_systemStatus(void)
{
	int iStatus = 0;
	pthread_mutex_lock(&systemStatusLock);
	iStatus = g_systemStatus;
	pthread_mutex_unlock(&systemStatusLock);
	return iStatus;
}

void set_systemStatus(int status)
{
	pthread_mutex_lock(&systemStatusLock);
	g_systemStatus = status;
	pthread_mutex_unlock(&systemStatusLock);
}

void destroy_systemStatusLock(void)
{
	pthread_mutex_destroy(&systemStatusLock);
}

/* 初始化g_systemStatus 互斥访问的互斥锁*/
void init_systemStatusLock(void)
{
	/* 用默认属性初始化互斥锁*/
	pthread_mutex_init(&systemStatusLock, NULL);
}

