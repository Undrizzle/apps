#ifndef __MME_AD_H__
#define __MME_AD_H__

#include <public.h>

#include <netpacket/packet.h>
#include <net/if_arp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <time.h>
#include <linux/if.h>
#include <fcntl.h>
#include <pthread.h>


#define MMEAD_MAX_CHILD_NUM 3

#ifdef MMEAD_SUPPORT_MUL_THREADS
#undef MMEAD_SUPPORT_MUL_THREADS
#endif

extern int MMEAD_MODULE_DEBUG_ENABLE;

void debug_dump_msg(const unsigned char memory [], size_t length, FILE *fp);

#define mmead_debug_printf(...);	\
{	\
	if( MMEAD_MODULE_DEBUG_ENABLE )	\
		fprintf(stderr, __VA_ARGS__);	\
}

/*==============================================================*/
/*                                    MMEAD模块内部消息接口定义                                      */

typedef struct
{
	int skfd;
	struct sockaddr_ll sockaddr;
	uint8_t peer[6];
}T_MME_SK_HANDLE;

typedef struct
{
	int skfd;
	struct sockaddr_in from;
}T_MMEAD_SK_HANDLE;

typedef struct
{
	T_MMEAD_SK_HANDLE SK_HANDLE;
	T_Msg_Header_MMEAD MMEAD_HEADER;
}T_T_Msg_header_MMEAD;

typedef struct
{
	T_MMEAD_SK_HANDLE sk;
	uint8_t b[MAX_UDP_SIZE];
#ifdef MMEAD_SUPPORT_MUL_THREADS
	pthread_t tid[MMEAD_MAX_CHILD_NUM];
	pthread_mutex_t	mutex;
	pthread_cond_t	condreq[MMEAD_MAX_CHILD_NUM];
	pthread_cond_t	condack[MMEAD_MAX_CHILD_NUM];
#endif
}MMEAD_BBLOCK_QUEUE;

/*==============================================================*/

#endif

