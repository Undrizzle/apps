ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE = arm-linux-
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar -r
STRIP	= $(CROSS_COMPILE)strip

KERNELDIR = $(ROOTDIR)/linux-3.4.6

IFLAGS = -I../include -I$(KERNELDIR)/include/mtd
CFLAGS += -g -O2

#ROOTFSDIR = ${ROOTDIR}/apps/rootfs
#BIN_PATH = $(ROOTFSDIR)/sbin

OBJS = nvm-utils.o
OBJS += hexdump.o
OBJS += md5.o
OBJS += boardapi.o

.PHONY: all clean distclean

all:build

build:
	$(CC) $(IFLAGS) -c nvm-utils.c $(CFLAGS)
	$(CC) $(IFLAGS) -c hexdump.c $(CFLAGS)
	$(CC) $(IFLAGS) -c md5.c $(CFLAGS)
	$(CC) $(IFLAGS) -c boardapi.c $(CFLAGS)
	$(AR) libboardapi.a $(OBJS)

clean:
	rm -f $(OBJS) *.a

distclean:	
	rm -f $(OBJS) *.a