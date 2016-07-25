ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE=arm-linux-
STRIP	= $(CROSS_COMPILE)strip
CC = $(CROSS_COMPILE)gcc
CFLAGS +=-g -O2

LDFLAGS += -L../dbs/client/lib
DBS_LIB = -ldbs
INCLUDE = -I../include -I../dbs/include -I../boardapi
BOARDAPI_LD_FLAGS = -L../boardapi -lboardapi
BUILDDIR = obj
BINDIR = bin

ROOTFSDIR = ${ROOTDIR}/apps/rootfs
#BIN_PATH = $(ROOTFSDIR)/usr/mnt/app

FLASHFSDIR = ${ROOTDIR}/apps/flashfs
BIN_PATH = $(FLASHFSDIR)/app

BIN_NAME=cli

#CFLAGS  += -msoft-float

.PHONY: all clean distclean

all:build install

build:
	mkdir -p $(BUILDDIR)
	mkdir -p $(BINDIR)
	$(CC) $(INCLUDE) -c util/*.c $(CFLAGS)
	$(CC) $(INCLUDE) -c app/*.c $(CFLAGS)
	$(CC) $(INCLUDE) -c *.c $(CFLAGS)
	mv *.o $(BUILDDIR)
	$(CC) $(BUILDDIR)/*.o -o $(BINDIR)/$(BIN_NAME) $(LDFLAGS) $(DBS_LIB) $(BOARDAPI_LD_FLAGS)
	$(STRIP) $(BINDIR)/$(BIN_NAME)
	
install:
	mkdir -p $(BIN_PATH)
	cp $(BINDIR)/$(BIN_NAME) $(BIN_PATH)/

clean:
	rm -f *.o
	rm -rf $(BUILDDIR) $(BINDIR)

distclean:
	rm -f $(BIN_PATH)/$(BIN_NAME)
