#---------------------------------------------------------------------------------
# PS3 Homebrew Application Makefile
#---------------------------------------------------------------------------------

# Include PSL1GHT toolchain
ifeq ($(strip $(PSL1GHT)),)
$(error "Please set PSL1GHT in your environment. export PSL1GHT=<path to PSL1GHT>")
endif

include $(PSL1GHT)/ppu_rules

# Override make's built-in LD variable
LD := $(PREFIX)gcc

#---------------------------------------------------------------------------------
# Application Info
#---------------------------------------------------------------------------------
TARGET		:=	clumsytools
BUILD		:=	build
SOURCES		:=	source
DATA		:=	data
INCLUDES	:=	include
PKGFILES	:=	pkgfiles

TITLE		:=	ClumsyTools App
APPID		:=	NP00CLMSY
CONTENTID	:=	UP0001-$(APPID)_00-0000000000000000
VERSION		:=	1.0.0

#---------------------------------------------------------------------------------
# Options for code generation
#---------------------------------------------------------------------------------
CFLAGS		=	-O2 -Wall -mcpu=cell $(MACHDEP) $(INCLUDE)
CXXFLAGS	=	$(CFLAGS)

#---------------------------------------------------------------------------------
# Any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=	-lrt -llv2 -lsysutil -lsysmodule -lgcm_sys -lrsx -lnet -lnetctl -lio -lm

#---------------------------------------------------------------------------------
# List of directories containing libraries
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS)

#---------------------------------------------------------------------------------
# No real need to edit anything past this point
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

export BUILDDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# Automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# Build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES), -I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					$(LIBPSL1GHT_INC) \
					-I$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# Build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					$(LIBPSL1GHT_LIB)

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo "[CLEAN]  Removing build files..."
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).self $(OUTPUT)*.pkg EBOOT.BIN PARAM.SFO

#---------------------------------------------------------------------------------
run:
	ps3load $(OUTPUT).self

#---------------------------------------------------------------------------------
pkg: $(BUILD) $(OUTPUT).pkg

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# Main targets
#---------------------------------------------------------------------------------
$(OUTPUT).self: $(OUTPUT).elf
$(OUTPUT).elf:	$(OFILES)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .bin extension
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
# PKG Build Rules
#---------------------------------------------------------------------------------
# Use custom PKG rule to override ppu_rules default
%.pkg: %.self
	@echo "[PKG]    Building package..."
	@mkdir -p $(BUILDDIR)/pkg/USRDIR
	@$(SELF_NPDRM) $(BUILDDIR)/$(basename $(notdir $<)).elf $(BUILDDIR)/pkg/USRDIR/EBOOT.BIN $(CONTENTID) > /dev/null
	@$(SFO) --title "$(TITLE)" --appid "$(APPID)" -f $(CURDIR)/sfo.xml $(BUILDDIR)/pkg/PARAM.SFO
	@if [ -n "$(PKGFILES)" -a -d "$(PKGFILES)" ]; then cp -rf $(PKGFILES)/* $(BUILDDIR)/pkg/; fi
	@$(PKG) --contentid $(CONTENTID) $(BUILDDIR)/pkg/ $@ > /dev/null
	@cp $@ $(basename $@).gnpdrm.pkg
	@$(PACKAGE_FINALIZE) $(basename $@).gnpdrm.pkg
	@echo "[DONE]   Package created: $(basename $@).gnpdrm.pkg"

