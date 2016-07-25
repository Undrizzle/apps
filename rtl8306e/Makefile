ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE = arm-linux-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
STRIP	= $(CROSS_COMPILE)strip

CFLAGS += -g -O2
LDFLAGS = -nostdlib -r -X -N


OBJS = mdcmdio.o
OBJS += rtl8306e_asicdrv.o
OBJS += rtk_api.o

.PHONY: all clean distclean

all:build

build:
	$(CC) $(IFLAGS) -c mdcmdio.c $(CFLAGS)
	$(CC) $(IFLAGS) -c rtl8306e_asicdrv.c $(CFLAGS)
	$(CC) $(IFLAGS) -c rtk_api.c $(CFLAGS)
	$(LD) $(LDFLAGS) -o rtl8306e.o $(OBJS)

clean:
	rm -f $(OBJS) rtl8306e.o

distclean:	
	rm -f $(OBJS) rtl8306e.o