ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE=arm-linux-
STRIP	= $(CROSS_COMPILE)strip
CC = $(CROSS_COMPILE)gcc
CFLAGS +=-g -O2

IFLAGS = -I../include -I../dbs/include -I../boardapi
LDFLAGS += -L../dbs/client/lib
DBS_LIB = -ldbs
BOARDAPI_LD_FLAGS = -L../boardapi -lboardapi

OBJ = autoconfig.o auto_dbs.o auto_mmead.o auto_cmm.o

ROOTFSDIR = ${ROOTDIR}/apps/rootfs
#BIN_PATH = $(ROOTFSDIR)/usr/mnt/app

FLASHFSDIR = ${ROOTDIR}/apps/flashfs
BIN_PATH = $(FLASHFSDIR)/app

.PHONY: all clean distclean

all:build install

build:
	$(CC) $(IFLAGS) -c autoconfig.c $(CFLAGS)
	$(CC) $(IFLAGS) -c auto_dbs.c $(CFLAGS)
	$(CC) $(IFLAGS) -c auto_mmead.c $(CFLAGS)
	#$(CC) $(IFLAGS) -c reg_alarm.c $(CFLAGS)
	#$(CC) $(IFLAGS) -c reg_tm.c $(CFLAGS)
	$(CC) $(IFLAGS) -c auto_cmm.c $(CFLAGS)
	$(CC) $(OBJ) -o autoconfig $(LDFLAGS) $(DBS_LIB) $(BOARDAPI_LD_FLAGS)
	$(STRIP) autoconfig
	
install:
	mkdir -p $(BIN_PATH)
	cp autoconfig $(BIN_PATH)/

clean:
	rm -f *.o autoconfig

distclean:
	rm -f $(BIN_PATH)/autoconfig