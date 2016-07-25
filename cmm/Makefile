ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE=arm-linux-
STRIP	= $(CROSS_COMPILE)strip
CC = $(CROSS_COMPILE)gcc
CFLAGS +=-g -O2

KERNELDIR = $(ROOTDIR)/linux-3.4.6
DSDTROOT = $(ROOTDIR)/apps/DSDT_3.0
DSDTSWDIR = $(DSDTROOT)/switch

ATCFLAGS = -D_AT30TK175_ -D_AT30TS75_ -c -std=gnu99

IFLAGS = -I../include -I../dbs/include -I../boardapi -I$(KERNELDIR)/include/mtd

DSDT_INC = -I$(DSDTSWDIR)/Include \
	-I$(DSDTSWDIR)/Include/h/msApi

LDFLAGS += -L../dbs/client/lib
DBS_LIB = -ldbs
BOARDAPI_LD_FLAGS = -L../boardapi -lboardapi

QD_LIB = $(DSDTSWDIR)/Library/qdDrv.o

OBJ = cmm.o cmm_tm.o cmm_mmead.o cmm_reg.o cmm_alarm.o cmm_dsdt.o 
OBJ += cmm_sysMonitor.o upgrade.o at30ts75.o at30ts.o  i2c_recieve.o i2c_transmit.o
OBJ += cmm_rtl8306e.o

ROOTFSDIR = ${ROOTDIR}/apps/rootfs
#BIN_PATH = $(ROOTFSDIR)/usr/mnt/app

FLASHFSDIR = ${ROOTDIR}/apps/flashfs
BIN_PATH = $(FLASHFSDIR)/app

#PTHREAD_LIB=-lpthread

.PHONY: all clean distclean

all:build install

build:
	$(CC) $(CFLAGS) $(ATCFLAGS) $(IFLAGS) -c at30tk175stk/at30ts.c
	$(CC) $(CFLAGS) $(ATCFLAGS) $(IFLAGS) -c at30tk175stk/i2c_recieve.c
	$(CC) $(CFLAGS) $(ATCFLAGS) $(IFLAGS) -c at30tk175stk/i2c_transmit.c
	$(CC) $(CFLAGS) $(ATCFLAGS) $(IFLAGS) -c at30tk175stk/at30ts75.c
	$(CC) $(IFLAGS) -c cmm_sysMonitor.c $(CFLAGS)
	$(CC) $(IFLAGS) -c upgrade.c $(CFLAGS)
	$(CC) $(IFLAGS) -c cmm_tm.c $(CFLAGS)
	$(CC) $(IFLAGS) -c cmm_mmead.c $(CFLAGS)
	$(CC) $(IFLAGS) -c cmm_reg.c $(CFLAGS)
	$(CC) $(IFLAGS) -c cmm_alarm.c $(CFLAGS)
	$(CC) $(IFLAGS) -c cmm_rtl8306e.c $(CFLAGS)
	$(CC) $(IFLAGS) $(DSDT_INC) -c cmm_dsdt.c $(CFLAGS)
	$(CC) $(IFLAGS) -c cmm.c $(CFLAGS)
	$(CC) $(OBJ) $(QD_LIB) -o cmm $(LDFLAGS) $(DBS_LIB) $(BOARDAPI_LD_FLAGS)
	$(STRIP) cmm
	
install:
	mkdir -p $(BIN_PATH)
	cp cmm $(BIN_PATH)/

clean:
	rm -rf *.o cmm

distclean:
	rm -f $(BIN_PATH)/cmm