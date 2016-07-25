ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

export DSDT_USER_BASE=$(ROOTDIR)/apps/DSDT_3.0/switch
export DSDT_PROJ_NAME=qdDrv
export DSDT_TOOL_DIR=$(DSDT_USER_BASE)/tools
export RELEASE=YES
export OS_RUN=LINUX
export MAD_USER_BASE=$(DSDT_USER_BASE)/../phy
export MAD_PROJ_NAME=madDrv
export MAD_TOOL_DIR=$(MAD_USER_BASE)/tools
export USER_API_NAME=
export USER_NAME=
export DSP_VCT=FALSE
export VCT_PENDING=FALSE
export DSDT_USE_MAD=FALSE

#export DSDT_USER_BASE DSDT_PROJ_NAME DSDT_TOOL_DIR RELEASE OS_RUN MAD_USER_BASE MAD_PROJ_NAME MAD_TOOL_DIR DSP_VCT VCT_PENDING DSDT_USE_MAD

#include $(ROOTDIR)/apps/DSDT_3.0/switch/tools/setenv

MVL_QD_DIR := $(DSDT_USER_BASE)
MVL_LIB_DIR := $(DSDT_USER_BASE)/Library


all:	SW


SW:
	mkdir -p $(MVL_LIB_DIR)
	make -C $(MVL_QD_DIR)/src;

clean :
	mkdir -p $(MVL_LIB_DIR)
	make -C $(MVL_QD_DIR)/src clean
distclean:
	rm -rf $(MVL_LIB_DIR)
