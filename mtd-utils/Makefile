ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE=arm-linux-
STRIP	= $(CROSS_COMPILE)strip

CFLAGS += CROSS=arm-linux- WITHOUT_XATTR=1

SRCDIR = ${ROOTDIR}/apps/mtd-utils/mtd-utils-1.5.0
ROOTFSDIR = ${ROOTDIR}/apps/rootfs
BIN_PATH = $(ROOTFSDIR)/sbin

OBJS =	$(SRCDIR)/arm-linux/flashcp
OBJS +=	$(SRCDIR)/arm-linux/flash_erase
OBJS +=	$(SRCDIR)/arm-linux/flash_lock
OBJS +=	$(SRCDIR)/arm-linux/flash_unlock
OBJS +=	$(SRCDIR)/arm-linux/mtd_debug
OBJS +=	$(SRCDIR)/arm-linux/nanddump
OBJS +=	$(SRCDIR)/arm-linux/nandtest
OBJS +=	$(SRCDIR)/arm-linux/nandwrite
OBJS +=	$(SRCDIR)/arm-linux/ubi-utils/mtdinfo
OBJS +=	$(SRCDIR)/arm-linux/ubi-utils/ubiattach
#OBJS +=	$(SRCDIR)/arm-linux/ubi-utils/ubicrc32
OBJS +=	$(SRCDIR)/arm-linux/ubi-utils/ubidetach
OBJS +=	$(SRCDIR)/arm-linux/ubi-utils/ubiformat
OBJS +=	$(SRCDIR)/arm-linux/ubi-utils/ubimkvol
OBJS +=	$(SRCDIR)/arm-linux/ubi-utils/ubinfo
#OBJS +=	$(SRCDIR)/arm-linux/ubi-utils/ubinize
#OBJS +=	$(SRCDIR)/arm-linux/ubi-utils/ubirename
OBJS +=	$(SRCDIR)/arm-linux/ubi-utils/ubirmvol
OBJS +=	$(SRCDIR)/arm-linux/ubi-utils/ubirsvol
OBJS +=	$(SRCDIR)/arm-linux/ubi-utils/ubiupdatevol


.PHONY: all clean distclean

all:build install

build:
	make -C $(SRCDIR) $(CFLAGS) 

install:
	mkdir -p $(BIN_PATH)
	$(STRIP) $(OBJS)
	cp $(SRCDIR)/arm-linux/flashcp $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/flash_erase $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/flash_lock $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/flash_unlock $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/mtd_debug $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/nanddump $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/nandtest $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/nandwrite $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/ubi-utils/mtdinfo $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/ubi-utils/ubiattach $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/ubi-utils/ubidetach $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/ubi-utils/ubiformat $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/ubi-utils/ubinfo $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/ubi-utils/ubimkvol $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/ubi-utils/ubirmvol $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/ubi-utils/ubirsvol $(BIN_PATH)/
	cp $(SRCDIR)/arm-linux/ubi-utils/ubiupdatevol $(BIN_PATH)/

clean:
	make -C $(SRCDIR) clean
	rm -rf $(SRCDIR)/arm-linux

distclean:
	rm -rf $(SRCDIR)/arm-linux
	rm -f $(BIN_PATH)/flashcp
	rm -f $(BIN_PATH)/flash_erase
	rm -f $(BIN_PATH)/flash_lock
	rm -f $(BIN_PATH)/flash_unlock
	rm -f $(BIN_PATH)/mtd_debug
	rm -f $(BIN_PATH)/nanddump
	rm -f $(BIN_PATH)/nandtest
	rm -f $(BIN_PATH)/nandwrite
	rm -f $(BIN_PATH)/mtdinfo
	rm -f $(BIN_PATH)/ubiattach
	rm -f $(BIN_PATH)/ubidetach
	rm -f $(BIN_PATH)/ubiformat
	rm -f $(BIN_PATH)/ubinfo
	rm -f $(BIN_PATH)/ubimkvol
	rm -f $(BIN_PATH)/ubirmvol
	rm -f $(BIN_PATH)/ubirsvol
	rm -f $(BIN_PATH)/ubiupdatevol

