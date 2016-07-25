ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE=arm-linux-
STRIP	= $(CROSS_COMPILE)strip
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar -r

IFLAGS = -I../include -I../boardapi -I../sqlite/sqlite-3.7.10 -I./include
#LDFLAGS += -L../sqlite/__install/lib
SQLITE_LD_FLAGS = -L../sqlite/__install/lib -lsqlite3
BOARDAPI_LD_FLAGS = -L../boardapi -lboardapi
CFLAGS +=-g -O2

OBJS = dbsUtils.o dbs.o dbsUnderlayer.o dbsLog.o

ROOTFSDIR = ${ROOTDIR}/apps/rootfs
#BIN_PATH = $(ROOTFSDIR)/usr/mnt/app

FLASHFSDIR = ${ROOTDIR}/apps/flashfs
BIN_PATH = $(FLASHFSDIR)/app


DB_PATH = $(ROOTFSDIR)/etc/config/databases

.PHONY: all clean distclean

all:build install

build:
	$(CC) $(IFLAGS) -c dbsUtils.c $(CFLAGS)
	$(CC) $(IFLAGS) -c sever/dbsUnderlayer.c $(CFLAGS)
	$(CC) $(IFLAGS) -c sever/dbsLog.c $(CFLAGS)
	$(CC) $(IFLAGS) -c sever/dbs.c $(CFLAGS)
	$(CC) $(OBJS) -o sever/dbs $(SQLITE_LD_FLAGS) $(BOARDAPI_LD_FLAGS)
	$(CC) $(IFLAGS) -c client/dbsapi.c $(CFLAGS)
	mkdir -p client/lib
	$(AR) client/lib/libdbs.a dbsapi.o
	$(STRIP) sever/dbs
	
install:	
	mkdir -p $(BIN_PATH)
	cp sever/dbs $(BIN_PATH)/
	mkdir -p $(DB_PATH)
	cp databases/system.db $(DB_PATH)/system.db
	cp databases/log.db $(DB_PATH)/log.db

clean:
	rm -f $(OBJS) dbsapi.o sever/dbs
	rm -rf client/lib

distclean:
	rm -rf $(DB_PATH)
	rm -rf $(BIN_PATH)/dbs

