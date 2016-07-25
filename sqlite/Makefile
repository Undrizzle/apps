ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE=arm-linux-
STRIP	= $(CROSS_COMPILE)strip
TARGET = arm-linux

SRCDIR = ${ROOTDIR}/apps/sqlite/sqlite-3.7.10
BUILDDIR = ${ROOTDIR}/apps/sqlite/__install

ROOTFSDIR = ${ROOTDIR}/apps/rootfs
BIN_PATH = $(ROOTFSDIR)/sbin
LIB_PATH = $(ROOTFSDIR)/lib

.PHONY: all clean distclean

all:configure build install

configure:
	cd $(SRCDIR);\
	./configure \
	--prefix=$(BUILDDIR) \
	--host=$(TARGET) \
	;
	
build:
	mkdir -p $(BUILDDIR)
	make -C $(SRCDIR)

install:
	make install -C $(SRCDIR)
	$(STRIP) $(BUILDDIR)/lib/libsqlite3.so*
	$(STRIP) $(BUILDDIR)/bin/sqlite3
	mkdir -p $(LIB_PATH)
	cp -Pf $(BUILDDIR)/lib/libsqlite3.so* $(LIB_PATH)/
	cp $(BUILDDIR)/bin/sqlite3 $(BIN_PATH)/
	
clean:
	make -C $(SRCDIR) clean;
	rm -rf $(BUILDDIR)
	
distclean:
	make -C $(SRCDIR) distclean;
	rm -rf $(BUILDDIR)
	rm -rf $(LIB_PATH)/libsqlite3.so*
	rm -rf $(BIN_PATH)/sqlite3

