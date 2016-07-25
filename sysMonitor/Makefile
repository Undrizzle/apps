ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE=arm-linux-
STRIP	= $(CROSS_COMPILE)strip
CC	= $(CROSS_COMPILE)gcc
TARGET=arm-linux

IFLAGS = -I../include -I../dbs/include -I../boardapi
CFLAGS +=-g -O2
LDFLAGS += -L../dbs/client/lib
DBS_LIB = -ldbs -lpthread
BOARDAPI_LD_FLAGS = -L../boardapi -lboardapi

ROOTFSDIR = ${ROOTDIR}/apps/rootfs
#BIN_PATH = $(ROOTFSDIR)/usr/mnt/app

FLASHFSDIR = ${ROOTDIR}/apps/flashfs
BIN_PATH = $(FLASHFSDIR)/app

OBJ = systemStsControl.o
#OBJ += sm2dbsMutex.o
OBJ += sysMonitor2cmm.o
OBJ += sysMonitor2alarm.o
OBJ += at91ButtonProcessor.o
OBJ += sysindiProcessor.o
OBJ += sysledProcessor.o
OBJ += wdtProcessor.o
OBJ += atmProcessor.o
OBJ += sysMonitor.o

.PHONY: all clean distclean

all:build install

build:
	$(CC) $(CFLAGS) $(IFLAGS) -c systemStsControl.c
#	$(CC) $(CFLAGS) $(IFLAGS) -c sm2dbsMutex.c
	$(CC) $(CFLAGS) $(IFLAGS) -c sysMonitor2cmm.c
	$(CC) $(CFLAGS) $(IFLAGS) -c sysMonitor2alarm.c
	$(CC) $(CFLAGS) $(IFLAGS) -c at91ButtonProcessor.c
	$(CC) $(CFLAGS) $(IFLAGS) -c sysindiProcessor.c
	$(CC) $(CFLAGS) $(IFLAGS) -c sysledProcessor.c
	$(CC) $(CFLAGS) $(IFLAGS) -c wdtProcessor.c
	$(CC) $(CFLAGS) $(IFLAGS) -c atmProcessor.c
	$(CC) $(CFLAGS) $(IFLAGS) -c sysMonitor.c
	$(CC) $(OBJ) -o sysMonitor $(LDFLAGS) $(DBS_LIB) $(BOARDAPI_LD_FLAGS)
	$(STRIP) sysMonitor

install:
	mkdir -p $(BIN_PATH)
	cp sysMonitor $(BIN_PATH)/

clean:
	rm -f *.o sysMonitor

distclean:
	rm -f $(BIN_PATH)/sysMonitor

