ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

IMAGEDIR = $(ROOTDIR)/images
SAFEFSDIR = ${ROOTDIR}/apps/safefs
ROOTFSDIR = ${ROOTDIR}/apps/rootfs
USERFSDIR = ${ROOTDIR}/apps/flashfs

WEBTARNAME = wec9720ek.tar.bz2
WEBIMGNAME = wec9720ek-s.web

ENVIMGNAME = env.bin

#UBIIMGNAME = rootfs.ubifs
#ROOTFSIMGNAME = rootfs.bin
#USRFSIMGNAME = userfs.bin
#USERFSIMG = userfs.bin

UBISAFEFS = safefs.ubifs
UBIROOTFS = rootfs.ubifs
UBIUSERFS = userfs.ubifs

UBIFSIMG = ubifs.bin


.PHONY: all clean distclean

all:build mkenvimg mkfsimg mkwebimg install

build:	
	make -C mkweb
	make -C mkenv

mkenvimg:
	rm -f $(ENVIMGNAME)
	mkenv/mkenv

mkfsimg:
	rm -f $(UBIIMAGE)
	rm -rf $(SAFEFSDIR)
	mkdir -p $(ROOTFSDIR)/dev
	rm -f $(ROOTFSDIR)/dev/*
	cp -a $(ROOTFSDIR) $(SAFEFSDIR)
	rm -f $(SAFEFSDIR)/etc/rcS.d/S56mountubifs
	rm -f $(SAFEFSDIR)/etc/rcS.d/S98configure
	$(ROOTDIR)/tools/mkfs.ubifs -r $(SAFEFSDIR) -m 2KiB -e 126KiB -c 96 -D device_table -o $(UBISAFEFS)
	$(ROOTDIR)/tools/mkfs.ubifs -r $(ROOTFSDIR) -m 2KiB -e 126KiB -c 80 -D device_table -o $(UBIROOTFS)
	$(ROOTDIR)/tools/mkfs.ubifs -r $(USERFSDIR) -m 2KiB -e 126KiB -c 48  -o $(UBIUSERFS)
	$(ROOTDIR)/tools/ubinize -o $(UBIFSIMG) -m 2KiB -p 128KiB -s 512 ./ubinize.cfg
	rm -f $(UBISAFEFS) $(UBIROOTFS) $(UBIUSERFS)

mkwebimg:
	rm -f $(WEBIMGNAME)
	cp -a $(USERFSDIR)/app ./
	cp -a $(USERFSDIR)/html ./
	tar jcvf $(WEBTARNAME) app html
	rm -rf app html
	mkweb/mkweb
	rm -f $(WEBTARNAME)

install:
	mkdir -p $(IMAGEDIR)
	rm -f $(IMAGEDIR)/$(ENVIMGNAME)
	rm -f $(IMAGEDIR)/$(UBIFSIMG)
	rm -f $(IMAGEDIR)/$(WEBIMGNAME)		
	cp $(ENVIMGNAME) $(IMAGEDIR)/
	cp $(UBIFSIMG) $(IMAGEDIR)/
	cp $(WEBIMGNAME) $(IMAGEDIR)/
	
clean:	
	make clean -C mkweb
	make clean -C mkenv
	rm -rf $(SAFEFSDIR)

distclean:
	rm -f $(ENVIMGNAME) $(UBIFSIMG) $(WEBIMGNAME)
	rm -f $(IMAGEDIR)/$(ENVIMGNAME)
	rm -f $(IMAGEDIR)/$(UBIFSIMG)
	rm -f $(IMAGEDIR)/$(WEBIMGNAME)
	rm -rf $(SAFEFSDIR)
	rm -rf $(USERFSDIR)
	rm -rf $(ROOTFSDIR)/dev
