/*****************************************************************************
//
//  Copyright (c) 2002  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92619
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
******************************************************************************
//
//  Filename:       systemlog.h
//  Author:         Yen Tran
//  Creation Date:  5/2/02
//
******************************************************************************
//  Description:
//      Define the global constants, and functions for sysDiag utility.
//
*****************************************************************************/

#ifndef __SYSTEM_LOG_H__
#define __SYSTEM_LOG_H__

/********************** Global Functions ************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/syslog.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* option is masked bits of BCM_SYSLOG_LOCAL, BCM_SYSLOG_REMOTE,
   BCM_SYSLOG_FILE and BCM_SYSLOG_CIRCULAR */
/* localDisplayLevel, localLogLevel and remoteLogLevel are essentially
   the priority of message which ranges from LOG_EMERG to LOG_DEBUG as
   defined in /sys/syslog.h. */
typedef struct bcmSyslogCb {
  int status;              /* enable, disable */
  int option;              /* log local, log remote, log local+remote */
  int serverPort;          /* if remote logging, syslog server's UDP port number */
  int localDisplayLevel;   /* messages get displayed if their priority is higher than this level */
  int localLogLevel;       /* messages get logged if their priority is higher than this level */
  int remoteLogLevel;      /* messages get logged if their priority is higher than this level */
  struct in_addr serverIP; /* if remote logging, syslog server's IP address */
} BCM_SYSLOG_CB, *pBCM_SYSLOG_CB;

#define BCM_SYSLOG_TASK             1
#define BCM_KLOG_TASK               2

/* bit fields for syslogd options */
#define BCM_SYSLOG_LOCAL                1
#define BCM_SYSLOG_REMOTE               2
#define BCM_SYSLOG_FILE                 4
#define BCM_SYSLOG_CIRCULAR             8
#define BCM_SYSLOG_LOCAL_REMOTE         3

#define BCM_SYSLOG_LOCAL_MASK           1
#define BCM_SYSLOG_REMOTE_MASK          2
#define BCM_SYSLOG_LOCAL_REMOTE_MASK    3
#define BCM_SYSLOG_CIRCULAR_MASK        8

/* default values */
#define BCM_SYSLOG_DEF_DISPLAY_LEVEL     LOG_ERR
#define BCM_SYSLOG_DEF_LOCAL_LOG_LEVEL   LOG_DEBUG
#define BCM_SYSLOG_DEF_REMOTE_LOG_LEVEL  LOG_ERR

#define BCM_SYSLOG_FIRST_READ           -2
#define BCM_SYSLOG_READ_BUFFER_ERROR    -1
#define BCM_SYSLOG_READ_BUFFER_END      -3
#define BCM_SYSLOG_MAX_LINE_SIZE        255
#define BCM_SYSLOG_MESSAGE_LEN_BYTES    4
#define BCM_SYSLOG_KILL_SIGKILL         9
#define BCM_SYSLOG_KILL_SIGTERM         15

#define BCM_SYSLOG_STATUS_ENABLE        1
#define BCM_SYSLOG_STATUS_DISABLE       0

/***************************************************
 This function is called to start syslogd & klogd
****************************************************/

/***********************************************************************
 This function is called to read circular buffer
 ptr is the ptr of line to be read; buffer is the ptr of buffer where
 message line is to be stored and return; this function returns new
 ptr value, this ptr value is essentially the next line to be read. 
***********************************************************************/

#if defined(__cplusplus)
}
#endif

#endif /* __SYSTEM_LOG_H__ */
