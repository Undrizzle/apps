ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE = arm-linux-

CC		= $(CROSS_COMPILE)gcc
STRIP	= $(CROSS_COMPILE)strip

CFLAGS += -g -O2

NFS_DIR = /home/nfs
TFTPROOT = /tftpboot

SAFEFSDIR = ${ROOTDIR}/apps/safefs
USERFSDIR = ${ROOTDIR}/apps/flashfs
ROOTFSDIR = ${ROOTDIR}/apps/rootfs

#ROOTFSNAME = rootfs.ubifs
#ROOTFSIMG = rootfs.img

#USERFSNAME = userfs.ubifs
#USERFSIMG = userfs.img

UBIFSIMG = ubifs.bin

apps = sqlite
apps += DSDT_3.0
apps += busybox
apps += mtd-utils
apps += rtl8306e
apps += boardapi
apps += nvm
apps += dbs
apps += snmp
apps += alarm
apps += cli
apps += cmm
apps += httpd
apps += mmead
apps += register
#apps += regi
apps += template
apps += sysMonitor
apps += autoconfig
apps += tests
apps += mkimges

.PHONY: all clean distclean nfs

all:app

app:
	@for i in $(apps) ; do \
	echo -e "\n======== Making $$i ========";\
	make -C $$i || exit $?; \
	echo -e "======== $$i done ========\n"; \
	done

tftp:
	rm -f $(TFTPROOT)/$(UBIFSIMG)
	cp mkimges/$(UBIFSIMG) $(TFTPROOT)/

clean:
	-for i in $(apps) ; do [ ! -d $$i ] || make -C $$i clean; \
	done

nfs:
	sudo rm -rf $(NFS_DIR)/*
	sudo cp -a $(ROOTFSDIR)/* $(NFS_DIR)/
	sudo cp -a $(USERFSDIR)/* $(NFS_DIR)/usr/mnt/

distclean:	
	rm -f $(TFTPROOT)/$(UBIFSIMG)
	-for i in $(apps) ; do [ ! -d $$i ] || make -C $$i distclean; \
	done

