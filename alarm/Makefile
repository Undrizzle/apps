ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE=arm-linux-
STRIP	= $(CROSS_COMPILE)strip
CC	= $(CROSS_COMPILE)gcc
TARGET=arm-linux

IFLAGS = -I../include -I../dbs/include -I../boardapi -I../snmp/net-snmp-5.4.4/include
CFLAGS +=-g -O2
LDFLAGS += -L../dbs/client/lib -L../snmp/snmp-build/lib
DBS_LIB = -ldbs -lnetsnmp
BOARDAPI_LD_FLAGS = -L../boardapi -lboardapi

ROOTFSDIR = ${ROOTDIR}/apps/rootfs
#BIN_PATH = $(ROOTFSDIR)/usr/mnt/app

FLASHFSDIR = ${ROOTDIR}/apps/flashfs
BIN_PATH = $(FLASHFSDIR)/app

OBJ = alarm.o

.PHONY: all clean distclean

all:build install

build:
	$(CC) $(CFLAGS) $(IFLAGS) -c alarm.c
	$(CC) $(OBJ) -o alarm $(LDFLAGS) $(DBS_LIB) $(BOARDAPI_LD_FLAGS)
	$(STRIP) alarm

install:
	mkdir -p $(BIN_PATH)
	cp alarm $(BIN_PATH)/

clean:
	rm -f *.o alarm

distclean:
	rm -f $(BIN_PATH)/alarm

