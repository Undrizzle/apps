ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE = arm-linux-
CC = $(CROSS_COMPILE)gcc
STRIP	= $(CROSS_COMPILE)strip

#KERNELDIR = $(ROOTDIR)/linux-3.4.6

IFLAGS = -I../include -I../boardapi
CFLAGS += -g -O2
LDFLAGS += -L../boardapi
LIBS = -lboardapi


ROOTFSDIR = ${ROOTDIR}/apps/rootfs
BIN_PATH = $(ROOTFSDIR)/sbin

.PHONY: all clean distclean

all:build install

build:
	$(CC) $(IFLAGS) -c nvm.c $(CFLAGS)
	$(CC) nvm.o -o nvm $(LDFLAGS) $(LIBS)

install:
	$(STRIP) nvm
	cp -f nvm $(BIN_PATH)/
	if [ -f $(BIN_PATH)/printenv ]; then \
		( unlink $(BIN_PATH)/printenv )\
	fi
	if [ -f $(BIN_PATH)/setenv ]; then \
		( unlink $(BIN_PATH)/setenv )\
	fi
	if [ -f $(BIN_PATH)/getenv ]; then \
		( unlink $(BIN_PATH)/getenv )\
	fi
	ln -s nvm $(BIN_PATH)/printenv
	ln -s nvm $(BIN_PATH)/setenv
	ln -s nvm $(BIN_PATH)/getenv

clean:
	rm -f *.o nvm

distclean:	
	if [ -f $(BIN_PATH)/printenv ]; then \
		( unlink $(BIN_PATH)/printenv )\
	fi
	if [ -f $(BIN_PATH)/setenv ]; then \
		( unlink $(BIN_PATH)/setenv )\
	fi
	if [ -f $(BIN_PATH)/getenv ]; then \
		( unlink $(BIN_PATH)/getenv )\
	fi
	rm -f *.o nvm
	rm -f $(BIN_PATH)/nvm
