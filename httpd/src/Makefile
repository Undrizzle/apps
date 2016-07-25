TARGET=arm-linux
CROSS_COMPILE=arm-linux-
STRIP	= $(CROSS_COMPILE)strip
AR = $(CROSS_COMPILE)ar -r
CC = $(CROSS_COMPILE)gcc

CFLAGS +=-g -O2

IFLAGS = -I. -I./include -I./include/json -I../../include -I../../dbs/include -I../../boardapi
DBS_LIB = -L../../dbs/client/lib -ldbs
JSON_LIB = -L./lib/ -ljson
BOARDAPI_LD_FLAGS = -L../../boardapi -lboardapi

obj-y = basic.o cgimain.o cgicmd.o jsonmain.o ej.o cgintwk.o httpd.o ifcuiweb.o syscall.o upload.o upgrade.o
obj-y += http2dbs.o http2cmm.o httpd2sysMonitor.o

all:build
build:
	$(CC) $(IFLAGS) -c httpd2sysMonitor.c $(CFLAGS)
	$(CC) $(IFLAGS) -c upload.c $(CFLAGS)
	$(CC) $(IFLAGS) -c upgrade.c $(CFLAGS)
	$(CC) $(IFLAGS) -c ifcuiweb.c $(CFLAGS)
	$(CC) $(IFLAGS) -c syscall.c $(CFLAGS)
	$(CC) $(IFLAGS) -c $(HTML_SRC_DIR)/src/cgintwk.c $(CFLAGS)
	$(CC) $(IFLAGS) -c basic.c $(CFLAGS)
	$(CC) $(IFLAGS) -c $(HTML_SRC_DIR)/src/cgimain.c $(CFLAGS)
	$(CC) $(IFLAGS) -c cgicmd.c $(CFLAGS)
	$(CC) $(IFLAGS) -c jsonmain.c $(CFLAGS) -std=c99 
	$(CC) $(IFLAGS) -c ej.c $(CFLAGS)
	$(CC) $(IFLAGS) -c http2dbs.c $(CFLAGS)
	$(CC) $(IFLAGS) -c http2cmm.c $(CFLAGS)
	$(CC) $(IFLAGS) -c httpd.c $(CFLAGS)	
	$(CC) $(obj-y) -o httpd $(JSON_LIB) $(DBS_LIB) $(BOARDAPI_LD_FLAGS)

clean:
	rm -f *.o httpd

