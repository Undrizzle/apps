ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE=arm-linux-
STRIP	= $(CROSS_COMPILE)strip
CC	= $(CROSS_COMPILE)gcc
TARGET=arm-linux

IFLAGS = -I../include -I../dbs/include
CFLAGS += -D_AT30TK175_ -D_AT30TS75_ -g2 -Wall -c -std=gnu99
LDFLAGS += -L../dbs/client/lib
DBS_LIB = -ldbs

ROOTFSDIR = ${ROOTDIR}/apps/rootfs

FLASHFSDIR = ${ROOTDIR}/apps/flashfs
BIN_PATH = $(FLASHFSDIR)/app

OBJ = at30ts.o i2c_recieve.o i2c_transmit.o at30tk175stk_example.o

.PHONY: all clean distclean

all:build install

build:
	$(CC) $(CFLAGS) $(IFLAGS) -c at30ts.c
	$(CC) $(CFLAGS) $(IFLAGS) -c i2c_recieve.c
	$(CC) $(CFLAGS) $(IFLAGS) -c i2c_transmit.c
	$(CC) $(CFLAGS) $(IFLAGS) -c at30tk175stk_example.c
	$(CC) $(OBJ) -o at30tk175stk_example $(LDFLAGS) $(DBS_LIB)
	$(STRIP) at30tk175stk_example

install:
	mkdir -p $(BIN_PATH)
	cp at30tk175stk_example $(BIN_PATH)/

clean:
	rm -f *.o at30tk175stk_example

distclean:
	rm -f $(BIN_PATH)/at30tk175stk_example


