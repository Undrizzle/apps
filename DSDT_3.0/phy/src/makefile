# makefile - build main object file
#
# modification history
# --------------------
# 06-03-05,mj	created
#
#######################################################################

TARGET = $(MAD_PROJ_NAME)

ifeq ($(OS_RUN),VXWORKS)
WORK_TO_DO = $(MAD_PROJ_NAME).o

# Include common variable definitions
include $(MAD_TOOL_DIR)\make.defs
endif

ifeq ($(OS_RUN),LINUX)
WORK_TO_DO = $(MAD_PROJ_NAME).o

# Include common variable definitions
include $(MAD_TOOL_DIR)/makelnx.defs
endif

exe : $(WORK_TO_DO)

.PHONY : exe

vpath %.o    $(OBJDIR)
vpath %.lib  $(OBJDIR)

MAINDRV_PATH = $(SRC_BASE_PATH)

SRC_OBJ_DIR = $(MAINDRV_PATH)/driver \
               $(MAINDRV_PATH)/madapi \
               $(MAINDRV_PATH)/macsec \
               $(MAINDRV_PATH)/platform

OBJECTS_LIST = $(MAINDRV_PATH)/driver/$(OBJDIR)/driver.o \
               $(MAINDRV_PATH)/madapi/$(OBJDIR)/madapi.o \
               $(MAINDRV_PATH)/macsec/$(OBJDIR)/macsec.o \
               $(MAINDRV_PATH)/platform/$(OBJDIR)/platform.o
ifeq ($(DSP_VCT),TRUE)
OBJECTS_LIST += $(MAINDRV_PATH)/madapi/dspVct/$(OBJDIR)/dspVct.o 
ifeq ($(MVL_MATH),TRUE)
OBJECTS_LIST += $(MAINDRV_PATH)/madapi/dspVct/math/$(OBJDIR)/math.o 
endif
endif

OBJECTS = $(OBJECTS_LIST)

OBJDIRS	= $(subst /,\,$(dir $(OBJECTS)))
LIBDIRS	= $(subst /,\,$(LIB_DIR))

OBJDIRS_/ = $(subst \,/,$(OBJDIRS))
LIBDIRS_/ = $(subst \,/,$(LIBDIRS))

ifeq ($(OS_RUN),VXWORKS)
$(MAD_PROJ_NAME).o : $(OBJECTS)
	@ $(ECHO) '------------'
	@ $(ECHO) 'Building $@'
	$(LD) $(LDFLAGS) -Map $(LIB_DIR)/$(MAD_PROJ_NAME).map -o $(LIB_DIR)/$(MAD_PROJ_NAME).o $(OBJECTS)

$(OBJECTS) : FORCE
	@($(CD) $(subst /,\,$(dir $(@D)))) && $(MAKE)

endif

ifeq ($(OS_RUN),LINUX)
$(MAD_PROJ_NAME).o : $(OBJECTS)
	@ $(ECHO) '------------'
	@ $(ECHO) 'Building $@'
	$(LD) $(LDFLAGS) -Map $(LIB_DIR)/$(MAD_PROJ_NAME).map -o $(LIB_DIR)/$(MAD_PROJ_NAME).o $(OBJECTS)

$(OBJECTS) : FORCE
	@cd $(dir $(@D)); $(MAKE)

endif

FORCE :

.PHONY : clean
ifeq ($(OS_RUN),VXWORKS)
clean :
	@for %x in ($(OBJDIRS)) do \
	    @($(CD) %x..) && $(MAKE) clean
	- ($(CD) $(LIBDIRS)) && $(RM) $(MAD_PROJ_NAME).o
	- ($(CD) $(LIBDIRS)) && $(RM) $(MAD_PROJ_NAME).map
endif

ifeq ($(OS_RUN),LINUX)
clean :
	@for i in $(SRC_OBJ_DIR); do	\
		cd $$i; $(RM) -r -f *.o *.map *_obj; 	\
	done
#		cd $$i; $(RM) -f *.o *.map; 	
	@for i in $(OBJDIRS_/); do	\
		$(RM) -r -f $$i; 	\
	done
#		cd $$i..; $(MAKE) clean;	
	@cd $(LIB_DIR); $(RM) *.o
	@cd $(LIB_DIR); $(RM) *.map
endif

#end of file
