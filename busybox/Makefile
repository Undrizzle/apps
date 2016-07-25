ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE=arm-linux-
STRIP	= $(CROSS_COMPILE)strip
TARGET = arm-linux

SRCDIR = ${ROOTDIR}/apps/busybox/busybox-1.20.0
ROOTFSDIR = ${ROOTDIR}/apps/rootfs
BUILDDIR = $(SRCDIR)/_install

.PHONY: all clean distclean

all:build install
	
build:
	cp $(SRCDIR)/wec9720ek_defconfig $(SRCDIR)/.config
	make -C $(SRCDIR)

install:
	make install -C $(SRCDIR)
	rm -f $(ROOTFSDIR)/bin/busybox
	cp -f $(BUILDDIR)/bin/busybox $(ROOTFSDIR)/bin/ 
	
clean:
	make -C $(SRCDIR) clean;
	
distclean:
	make -C $(SRCDIR) distclean;
	rm -f $(ROOTFSDIR)/bin/busybox
