/***********************************************************************************************/
/* �ڶ��̳߳�������У����ڶ���߳���DBSͨѶʱ�� ������ͬһ��      */
/* SOCKET�ӿڣ�����Ҫ���̻߳��������� ֹ��ͬ���߳�ͬʱ��DBSͨѶ     */
/* �����µ���Ϣ���ҵ����⣬ ����������ÿ���̴߳���������SOCKET      */
/* �ӿ���DBSͨѶ�� �����Ͳ��ᵼ���̼߳���Ϣ���ң���������֮ǰ      */
/* dbsapi  �����Լ��������ͬһ�����ڵĲ�ͬ�̲߳��ܶ�����Ե�SOCKET */
/* ��Ϣ�ӿڣ���������˷���DBS���̻߳���������ܸ��������             */
/***********************************************************************************************/

#ifndef __SM2DBS_MUTEX_H__
#define __SM2DBS_MUTEX_H__

#include <stdio.h>
#include <dbsapi.h>

void dbsMutexWaitModule(unsigned int MF);
int dbs_mutex_sys_log(unsigned int priority, const char *message);
int dbsMutexGetSysinfo(uint16_t id, st_dbsSysinfo *row);

/* ע��dbs ������ʵĻ����������ر�dbs ���ʾ��*/
int destroy_sm2dbs(void);

/* ��ʼ��dbs ������ʵĻ�����������dbs ���ʾ��*/
int init_sm2dbs(unsigned short srcMod);

#endif 


