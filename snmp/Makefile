ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE=arm-linux-
STRIP	= $(CROSS_COMPILE)strip

TMPDISTDIR	= ${ROOTDIR}/apps/snmp/snmp-build
SRCDIR		= ${ROOTDIR}/apps/snmp/net-snmp-5.4.4

TARGET = arm-linux
ENDIANNESS = little

ROOTFSDIR = ${ROOTDIR}/apps/rootfs
#BIN_PATH = $(ROOTFSDIR)/usr/mnt/app

FLASHFSDIR = ${ROOTDIR}/apps/flashfs
BIN_PATH = $(FLASHFSDIR)/app

SNMPCONFIG_PATH = $(ROOTFSDIR)/etc/config

ACTIVE_NODES = ucd-snmp/dlmod if-mib host mibII/system_mib mibII/sysORTable \
				mibII/at mibII/ifTable mibII/ip mibII/snmp_mib mibII/tcp mibII/icmp mibII/udp \
				prevail-mib/cbatAction/cbatAction prevail-mib/softwareMgmt/softwareMgmt \
				prevail-mib/networkInfo/networkInfo prevail-mib/snmpGroup/snmpGroup \
				prevail-mib/systemDesc/systemDesc prevail-mib/systemInfo/systemInfo \
				prevail-mib/cnuTable/cnuTable prevail-mib/profileTable/profileTable \
				prevail-mib/cltTable/cltTable prevail-mib/cltproTable/cltproTable

.PHONY: all clean distclean

PRIVATE_PUBLIC_INCLUDES = ${ROOTDIR}/apps/include
PRIVATE_DBS_INCLUDES = ${ROOTDIR}/apps/dbs/include

SRCDIR=$(CURDIR)/net-snmp-5.4.4
LDFLAGS=-L$(TOOLCHAINS_SYSROOT)./lib -L$(TOOLCHAINS_SYSROOT)./usr/lib -L${ROOTDIR}/apps/dbs/client/lib
CPPFLAGS=-I$(PRIVATE_PUBLIC_INCLUDES) -I$(PRIVATE_DBS_INCLUDES)
export LDFLAGS CPPFLAGS

all:configure build install

# Notes:
#	ieee802dot11 supported by an open source project,
#	it's required lib "-lm".
configure:
	mkdir -p $(TMPDISTDIR)
	cd $(SRCDIR);\
	./configure \
	--prefix=$(TMPDISTDIR) \
	--host=$(TARGET) \
	--enable-as-needed \
	--enable-mini-agent \
	--with-endianness=$(ENDIANNESS) \
	--with-default-snmp-version="2" \
	--with-sys-contact="frank_cl@126.com" \
	--with-sys-location="China" \
	--with-logfile="/var/log/snmp.log" \
	--with-persistent-directory="/var/net-snmp" \
	--with-libs="-lm" \
	--disable-applications \
	--without-perl-modules \
	--without-perl \
	--with-out-transports="IPX AAL5PVC" \
	--with-out-mib-modules="snmpv3 snmpv3mibs" \
	--disable-embedded-perl \
	--disable-manuals \
	--disable-mibs \
	--disable-mib-loading \
	--disable-debugging \
	--disable-des \
	--disable-privacy \
	--disable-md5 \
	--enable-shared=no \
	--enable-static=yes \
	--with-mib-modules="${ACTIVE_NODES}" \
	CFLAGS="-Os -Ulinux -Dlinux=linux" \
	;
	touch configure;

build:
	make -C $(SRCDIR) all install
	$(STRIP) $(TMPDISTDIR)/sbin/snmpd

install:
	mkdir -p $(BIN_PATH)
	cp $(TMPDISTDIR)/sbin/snmpd $(BIN_PATH)/
	mkdir -p $(SNMPCONFIG_PATH)
	cp snmpd.conf $(SNMPCONFIG_PATH)/

clean:	
	make -C $(SRCDIR) clean
	rm -rf $(TMPDISTDIR)

distclean:
	make -C $(SRCDIR) distclean;
	rm -rf $(TMPDISTDIR)
	rm -f configure
	rm -f $(BIN_PATH)/snmpd
	rm -f $(SNMPCONFIG_PATH)/snmpd.conf

