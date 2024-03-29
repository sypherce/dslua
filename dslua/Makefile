PROGNAME = DSLua
OFILES   +=
ADD_LIBS +=

PATH 		:= $(DEVKITARM)/bin:$(PATH)

ARM7BIN		:= -7 $(PAPATH)/lib/arm7/arm7.bin
TEXT1 		:= DSLua
TEXT2 		:= Preview
TEXT3 		:= www.dslua.com
ICON 		:= -b $(CURDIR)/../logo.bmp
#LOGO		:= -o $(CURDIR)/../logo_wifi.bmp
LOGO		:= -o $(CURDIR)/../logo.bmp

#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif
#---------------------------------------------------------------------------------
# TARGET is the name of the output, if this ends with _mb generates a multiboot image
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET	:=	$(shell basename $(CURDIR))
BUILD		:=	build
SOURCES	:=	gfx source data \
lib/lua-5.0.2
INCLUDES	:=	./ include build data  \
lib/lua-5.0.2
RELEASE		:=	release
TEXT		:=	text
EXPORT_DIR := /c/ndsexamples/
#---------------------------------------------------------------------------------
# ARM7BIN is the path to an arm7 binary other than the default
#	usage: ARM7BIN := -7 binaryName.bin
#
# ICON is the path to an icon to be used int the header plus text
#	usage: ICON := -t iconName.bmp "text line one; text line 2; text line 3"
#
#---------------------------------------------------------------------------------



#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-mthumb-interwork

# note: arm9tdmi isn't the correct CPU arch, but anything newer and LD
# *insists* it has a FPU or VFP, and it won't take no for an answer!

NUMBER= -DLUA_USER_H='"./lib/lua-5.0.2/etc/luser_number.h"' -DUSE_INT

CFLAGS	:=	-g -Wall -O2\
 		-mcpu=arm9tdmi -mtune=arm9tdmi -fomit-frame-pointer\
		-ffast-math \
		$(ARCH) \
		-DUSE_DEGREES \
		-DNODEBUG

ifeq (${CFSUPPORT},1)
CFLAGS	+=	-DUSE_CF
endif

CFLAGS	+=	$(INCLUDE) -DARM9 -I$(DEVKITPRO)/PAlib/include/nds

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	:=	-g $(ARCH) -mno-fpu -L$(DEVKITPRO)/PAlib/lib

#---------------------------------------------------------------------------------
# path to tools - this can be deleted if you set the path in windows
#---------------------------------------------------------------------------------
# export PATH		:=	/d/dev/ds/devkitARM_r11/bin:/bin

#---------------------------------------------------------------------------------
# PATH to ndslib - just make a system variable called NDSLIBPATH and be done with it
#---------------------------------------------------------------------------------
# NDSLIBPATH	:=	/d/dev/ds/ndslib/

#---------------------------------------------------------------------------------
# the prefix on the compiler executables
#---------------------------------------------------------------------------------
PREFIX			:=	arm-eabi-
#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	  := -lfat -lnds9
LIBSPA	  := -lpa9
LIBDSWIFI := -ldswifi9


#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS		:=	$(DEVKITPRO)/libnds
LIBDIRPA	:=	$(PAPATH)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

export CC		:=	$(PREFIX)gcc
export CXX		:=	$(PREFIX)g++
export AR		:=	$(PREFIX)ar
export OBJCOPY	:=	$(PREFIX)objcopy
#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
export LD		:=	$(CXX)
#export LD		:=	$(CC)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
PCXFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.pcx)))
BINFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.bin)))
PALFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.pal)))
RAWFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.raw)))
MAPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.map)))
JPEGFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.jpg)))
MODFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.mod)))
GIFFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.gif)))
BMPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.bmp)))

export OFILES	:=	$(MAPFILES:.map=.o) $(RAWFILES:.raw=.o) $(PALFILES:.pal=.o) $(BINFILES:.bin=.o) $(PCXFILES:.pcx=.o) $(JPEGFILES:.jpg=.o) $(MODFILES:.mod=.o) $(GIFFILES:.gif=.o) $(BMPFILES:.bmp=.o)\
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include/nds) \
					-I$(PAPATH)/include/nds\
					-I$(CURDIR)/$(BUILD)

export LIBPATHS		:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)
export LIBPATHPA	:=	$(foreach dir,$(LIBDIRPA),-L$(dir)/lib)

.PHONY: $(BUILD) clean export release

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
	@gbfs $(BUILD)/data.gbfs scripts/*.*
	
	@echo ========== Create packed .nds
	@cp $(OUTPUT).nds $(BUILD)/$(TARGET).bin
	@padbin 256 $(BUILD)/$(TARGET).bin
	@cat $(BUILD)/$(TARGET).bin $(BUILD)/data.gbfs>$(OUTPUT)-PACK.nds
	
	@echo ========== Create packed .ds.gba
	@cp $(OUTPUT).ds.gba $(BUILD)/$(TARGET).gba.bin
	@padbin 256 $(BUILD)/$(TARGET).gba.bin
	@cat $(BUILD)/$(TARGET).gba.bin $(BUILD)/data.gbfs>$(OUTPUT)-PACK.ds.gba
	
	@echo ========== Create packed .sc.nds
	@cp $(OUTPUT).sc.nds $(BUILD)/$(TARGET).sc.bin
	@padbin 256 $(BUILD)/$(TARGET).sc.bin
	@cat $(BUILD)/$(TARGET).sc.bin $(BUILD)/data.gbfs>$(OUTPUT)-PACK.sc.nds

#---------------------------------------------------------------------------------
release:
	@echo Release ...$(TARGET)
	@rm -fr $(RELEASE)
	@mkdir $(RELEASE)
	@mkdir $(RELEASE)/scripts
	@cp $(TARGET).nds $(RELEASE)/$(TARGET).nds
	@cp text/* $(RELEASE)/
	@cp scripts/*.* $(RELEASE)/scripts
	@cp pack.bat $(RELEASE)/
	@cp -r bin $(RELEASE)/

clean:
	@echo clean ...$(TARGET)
	@rm -fr $(BUILD) $(RELEASE) bundle *.elf *.*ds*

export:
	@echo exporting ...$(TARGET)
	@cp *.nds $(EXPORT_DIR)/$(TARGET).nds

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).ds.gba	: 	$(OUTPUT).nds

$(OUTPUT).nds	: 	$(OUTPUT).bin

$(OUTPUT).bin	:	$(OUTPUT).elf

$(OUTPUT).elf	:	$(OFILES)

#---------------------------------------------------------------------------------
%.ds.gba: %.nds
	@echo built ... $(notdir $@)
	@dsbuild $<
	@cp $(CURDIR)/../$(notdir $@) ../$(notdir $(OUTPUT)).sc.nds

#---------------------------------------------------------------------------------
%.nds: %.bin
	
	@ndstool -c $@ -9 $(TARGET).bin $(ARM7BIN) $(LOGO) $(ICON) "$(TEXT1);$(TEXT2);$(TEXT3)"


#---------------------------------------------------------------------------------
%.bin: %.elf
	
	@$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

#---------------------------------------------------------------------------------
%.elf:
	@echo $(LD)  $(LDFLAGS) -specs=ds_arm9.specs $(OFILES) $(LIBPATHZ) $(LIBZ) $(LIBDSWIFI) $(LIBPATHPA) $(LIBSPA) $(LIBPATHS) $(LIBS) -o $(TARGET).elf
	@$(LD)  $(LDFLAGS) -specs=ds_arm9.specs $(OFILES) $(LIBPATHZ) $(LIBZ) $(LIBDSWIFI) $(LIBPATHPA) $(LIBSPA) $(LIBPATHS) $(LIBS) -o $(TARGET).elf



#---------------------------------------------------------------------------------
# Compile Targets for C/C++
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
%.o : %.cpp
	@echo $(notdir $<)
	@$(CXX) -MM $(CFLAGS) -o $*.d $<
	@$(CXX) $(CFLAGS) -c $< -o$@

#---------------------------------------------------------------------------------
%.o : %.c
	@echo $(notdir $<)
	@$(CC) -MM $(CFLAGS) -o $*.d $<
	@$(CC)  $(CFLAGS) -c $< -o$@

#---------------------------------------------------------------------------------
%.o : %.s
	@echo $(notdir $<)
	@$(CC) -MM $(CFLAGS) -o $*.d $<
	@$(CC)  $(ASFLAGS) -c $< -o$@

define bin2o
	cp $(<) $(*).tmp
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm \
	--rename-section .data=.rodata \
	--redefine-sym _binary_$*_tmp_start=$*\
	--redefine-sym _binary_$*_tmp_end=$*_end\
	--redefine-sym _binary_$*_tmp_size=$*_size\
	$(*).tmp $(@)
	echo "extern const u8" $(*)"[];" > $(*).h
	echo "extern const u32" $(*)_size[]";" >> $(*).h
	rm $(*).tmp
endef

#---------------------------------------------------------------------------------
%.o	:	%.pcx
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.o	:	%.raw
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.o	:	%.pal
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.o	:	%.map
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.o	:	%.mdl
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.o	:	%.jpg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.o	:	%.mod
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.o	:	%.gif
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.o	:	%.bmp
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)


-include $(DEPENDS)
#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
