#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

include $(DEVKITARM)/ds_rules

export TARGET		:=	$(shell basename $(CURDIR))
export TOPDIR		:=	$(CURDIR)


#---------------------------------------------------------------------------------
# path to tools - this can be deleted if you set the path in windows
#---------------------------------------------------------------------------------
export PATH		:=	$(DEVKITARM)/bin:$(PATH)

.PHONY: $(TARGET).arm7 $(TARGET).arm9

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).ds.gba

$(TARGET).ds.gba	: $(TARGET).nds

#---------------------------------------------------------------------------------
$(TARGET).nds	:	lib/libmikmod7.a $(TARGET).arm7 lib/libmikmod9.a lib/liblua-5.1.2.a lib/libdslua.a $(TARGET).arm9
	ndstool	-c $(TARGET).nds -7 $(TARGET).arm7 -9 $(TARGET).arm9

#---------------------------------------------------------------------------------
$(TARGET).arm7	: arm7/$(TARGET).elf
$(TARGET).arm9	: arm9/$(TARGET).elf

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7
	
#---------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C arm9
	
#---------------------------------------------------------------------------------
lib/liblua-5.1.2.a:
	$(MAKE) -C lib/lua-5.1.2
	
#---------------------------------------------------------------------------------
lib/libmikmod7.a:
	$(MAKE) -C lib/mikmod
	
#---------------------------------------------------------------------------------
lib/libmikmod9.a:
	$(MAKE) -C lib/mikmod
	
#---------------------------------------------------------------------------------
lib/libdslua.a:
	$(MAKE) -C lib/dslua

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C lib/dslua clean
	$(MAKE) -C lib/lua-5.1.2 clean
	$(MAKE) -C lib/mikmod clean
	$(MAKE) -C arm7 clean
	rm -f $(TARGET).ds.gba $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9
