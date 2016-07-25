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

OBJDIR = ./obj
OBJ = mme_rtl8306e.o mmeapi.o mmead.o support/atheros/tools/*.o support/atheros/ihpapi/ihpapi.o

ROOTFSDIR = ${ROOTDIR}/apps/rootfs
#BIN_PATH = $(ROOTFSDIR)/usr/mnt/app

FLASHFSDIR = ${ROOTDIR}/apps/flashfs
BIN_PATH = $(FLASHFSDIR)/app

.PHONY: all clean distclean

all:build install

build:
	make -C support/atheros/tools
	make -C support/atheros/ihpapi
	$(CC) $(CFLAGS) $(IFLAGS) -c mme_rtl8306e.c
	$(CC) $(CFLAGS) $(IFLAGS) -c mmeapi.c
	$(CC) $(CFLAGS) $(IFLAGS) -c mmead.c
	$(CC) $(OBJ) -o mmead $(LDFLAGS) $(DBS_LIB) $(BOARDAPI_LD_FLAGS)
	$(STRIP) mmead
	
install:
	mkdir -p $(BIN_PATH)
	cp mmead $(BIN_PATH)/

clean:
	make -C support/atheros/tools clean
	make -C support/atheros/ihpapi clean
	rm -f *.o mmead

distclean:
	rm -f $(BIN_PATH)/mmead
