# Source files in this directory
TARGET = 
CSOURCES	= madDrvConfig.c madDevConfig.c madHwCntl.c madUtils.c
ASOURCES	=

# Include common variable definitions
ifeq ($(OS_RUN),VXWORKS)
include $(MAD_TOOL_DIR)\make.defs
endif
ifeq ($(OS_RUN),LINUX)
include $(MAD_TOOL_DIR)/makelnx.defs
endif

# Add in extra stuffs
EXTRA_INCLUDE	+=
EXTRA_DEFINE	+=
ADDED_CFLAGS	+=
#ADDED_CFLAGS	+= -DDEBUG_MAD

ifeq ($(DSP_VCT),TRUE)
ADDED_CFLAGS	+= -DMAD_DSP_VCT
endif

# Include common build rules
ifeq ($(OS_RUN),VXWORKS)
include $(MAD_TOOL_DIR)\make.rules
endif
ifeq ($(OS_RUN),LINUX)
include $(MAD_TOOL_DIR)/makelnx.rules
endif

# end of file
