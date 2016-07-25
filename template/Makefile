ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE=arm-linux-
STRIP	= $(CROSS_COMPILE)strip
CC		= $(CROSS_COMPILE)gcc

CFLAGS +=-g -O2
IFLAGS = -I../include -I../dbs/include -I../boardapi
LDFLAGS += -L../dbs/client/lib
DBS_LIB = -ldbs
BOARDAPI_LD_FLAGS = -L../boardapi -lboardapi

OBJ = template.o tm_interface.o tm_reg.o

ROOTFSDIR = ${ROOTDIR}/apps/rootfs
#BIN_PATH = $(ROOTFSDIR)/usr/mnt/app

FLASHFSDIR = ${ROOTDIR}/apps/flashfs
BIN_PATH = $(FLASHFSDIR)/app

PIB_PATH = $(ROOTFSDIR)/etc/config/template

.PHONY: all clean distclean

all:build install

build:
	$(CC) $(IFLAGS) -c template.c $(CFLAGS)
	$(CC) $(IFLAGS) -c tm_interface.c $(CFLAGS)
	$(CC) $(IFLAGS) -c tm_reg.c $(CFLAGS)
	$(CC) $(OBJ) -o tmcore $(LDFLAGS) $(DBS_LIB) $(BOARDAPI_LD_FLAGS)
	$(STRIP) tmcore
	
install:
	mkdir -p $(BIN_PATH)
	cp tmcore $(BIN_PATH)/
	mkdir -p $(PIB_PATH)
	cp -Pf pibs/* $(PIB_PATH)/

clean:
	rm -f *.o tmcore

distclean:
	rm -rf $(PIB_PATH)
	rm -f $(BIN_PATH)/tmcore