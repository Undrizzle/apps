#include <stdio.h>
#include <pthread.h>

#include "sm2dbsMutex.h"

pthread_mutex_t sm2dbsLock;


void dbsMutexWaitModule(unsigned int MF)
{
	pthread_mutex_lock(&sm2dbsLock);
	dbsWaitModule(MF);
	pthread_mutex_unlock(&sm2dbsLock);
}

int dbs_mutex_sys_log(unsigned int priority, const char *message)
{
	int ret = 0;

	pthread_mutex_lock(&sm2dbsLock);
	ret = dbs_sys_log(priority, message);
	pthread_mutex_unlock(&sm2dbsLock);
	
	return ret;
}

int dbsMutexGetSysinfo(uint16_t id, st_dbsSysinfo *row)
{
	int ret = 0;

	pthread_mutex_lock(&sm2dbsLock);
	ret = dbsGetSysinfo(id, row);
	pthread_mutex_unlock(&sm2dbsLock);
	
	return ret;
}

int destroy_sm2dbs(void)
{
	int ret = 0;
	
	pthread_mutex_lock(&sm2dbsLock);
	ret = dbsClose();
	pthread_mutex_unlock(&sm2dbsLock);
	
	pthread_mutex_destroy(&sm2dbsLock);
	return ret;
}

/* ��ʼ��dbs ������ʵĻ�����������dbs ���ʾ��*/
int init_sm2dbs(unsigned short srcMod)
{
	int ret = 0;
	/* ��Ĭ�����Գ�ʼ��������*/
	pthread_mutex_init(&sm2dbsLock, NULL);
	
	pthread_mutex_lock(&sm2dbsLock);
	ret = dbsOpen(srcMod);
	pthread_mutex_unlock(&sm2dbsLock);
	return ret;
}

