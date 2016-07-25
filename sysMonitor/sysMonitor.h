#ifndef __SYS_MONITOR_H__
#define __SYS_MONITOR_H__

#include <stdio.h>
#include <sys/ioctl.h>

/*define ioctl command*/
#define KEY_MAGIC			'k'
#define LED_ON_CMD      		_IO(KEY_MAGIC, 1)
#define LED_OFF_CMD     		_IO(KEY_MAGIC, 2)
#define GET_KEY_STS_CMD	_IO(KEY_MAGIC, 3)

/*define led number*/
#define LEFT_LED  			(unsigned long)1
#define RIGHT_LED 			(unsigned long)2

#endif 

