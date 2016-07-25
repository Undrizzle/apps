# makefile - build sample files
#
# modification history
# --------------------
# 06-06-05,mj	created
# 06-22-05,mj	added more sample sources
#
#######################################################################

DEMONAME = sample
WORK_TO_DO = $(DEMONAME).o

exe : $(WORK_TO_DO)

# Include common variable definitions
include $(MAD_TOOL_DIR)\make.defs

.PHONY : exe

MADLIB = $(LIB_DIR)/$(MAD_PROJ_NAME).o

OBJDIRS	= $(subst /,\,$(dir $(MADLIB)))

CSOURCES = Initialization/madOsSem.c Initialization/bspSpecificMii.c    \
           Initialization/madApiInit.c Interrupt/madIntSample.c         \
           Loopback/madExLoop.c Loopback/madMacIfLoop.c PktGen/madPktGenSample.c    \
           vct/madVctTest.c CRCChecker/CRCChecker.c test/testMAD.c

EXTRA_INCLUDE = -I./Initialization
COBJECTS  	= $(CSOURCES:.c=.o)
AOBJECTS  	= $(ASOURCES:.s=.o)
ifeq ($(OBJECTS),)
OBJECTS  	= $(COBJECTS) $(AOBJECTS)
endif

OBJECTS_MS= $(subst /,\,$(OBJECTS))

$(DEMONAME).o : $(OBJECTS) $(MADLIB)
	$(LD) $(LDFLAGS) -Map $(DEMONAME).map -o $(DEMONAME).o $(OBJECTS) $(MADLIB)

$(OBJECTS) : %.o : %.c
.c.o :
	$(CC) $(CFLAGS) $(EXTRA_DEFINE) $(EXTRA_INCLUDE) $(ADDED_CFLAGS) -c $< -o $@

.PHONY : clean
clean :
	@for %x in ($(OBJECTS_MS)) do \
		$(RM) %x
	$(RM) $(DEMONAME).*

#end of file
