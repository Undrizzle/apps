/***********************************************************************************************/
/* 在多线程程序设计中，由于多个线程与DBS通讯时候 共享了同一个      */
/* SOCKET接口，所以要加线程互斥锁来禁 止不同的线程同时与DBS通讯     */
/* 而导致的消息错乱的问题， 常规做法是每个线程创建单独的SOCKET      */
/* 接口与DBS通讯， 这样就不会导致线程间消息错乱，但是由于之前      */
/* dbsapi  的设计约束导致了同一进程内的不同线程不能独享各自的SOCKET */
/* 消息接口，所以添加了访问DBS的线程互斥锁来规避该设计问题             */
/***********************************************************************************************/

#ifndef __SM2DBS_MUTEX_H__
#define __SM2DBS_MUTEX_H__

#include <stdio.h>
#include <dbsapi.h>

void dbsMutexWaitModule(unsigned int MF);
int dbs_mutex_sys_log(unsigned int priority, const char *message);
int dbsMutexGetSysinfo(uint16_t id, st_dbsSysinfo *row);

/* 注销dbs 互斥访问的互斥锁，并关闭dbs 访问句柄*/
int destroy_sm2dbs(void);

/* 初始化dbs 互斥访问的互斥锁，并打开dbs 访问句柄*/
int init_sm2dbs(unsigned short srcMod);

#endif 


