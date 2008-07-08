#GNU makefile for generation of genpr.

include ../../binutils.tools.def

SHELL=/bin/sh

V=@

#Folders

DIR_RTKUTILS    =../../../../common/rtkutils
DIR_UTIL_INC=$(DIR_UTIL)
DIR_UTIL_LIB=$(DIR_UTIL)/lib

DIR_GENSYN    =..
DIR_GENSYN_SRC=$(DIR_GENSYN)/src
DIR_GENSYN_INC=$(DIR_GENSYN)/include
DIR_GENSYN_BIN=$(DIR_GENSYN)/bin

#Tools
#CC = $(BUILD_TOOLS_PATH)gcc
CFLAGS_OPTIM = -O4
CFLAGS_DEBUG = -g
include $(DIR_UTIL_INC)/makefile.version
CFLAGS  += $(CPP_FLAGS) $(VFLAGS) $(MORE_DEFINE) -I$(DIR_GENSYN_INC) -I$(DIR_UTIL_INC)/$(HOST_OS) -I$(DIR_UTIL_INC) -D_FOR_GAS
GENSYNDEPLIBS = $(DIR_UTIL_LIB)/scutil$(HOST_OS).a $(DIR_UTIL_LIB)/libutils$(HOST_OS).a

ifeq "$(STATUS)" "RELEASE"
STRIP=$(BUILD_TOOLS_PATH)$(HOST_OS_PREFIX)strip
else
STRIP=ls -l
endif

EXE_SXGENSYN = $(DIR_GENSYN_BIN)/sxgensyn$(EXE_SUFFIX)
EXE_TOOLKIT  = $(DIR_GENSYN_BIN)/sxgensyn_toolkit$(EXE_SUFFIX)

#List of objects
OBJ_SXGENSYN =                 \
	$(HOST_OS)/action0.o        \
	$(HOST_OS)/analex0.o        \
	$(HOST_OS)/avl.o            \
	$(HOST_OS)/gensyn_error.o   \
	$(HOST_OS)/gensyn_option.o  \
	$(HOST_OS)/gensyn_version.o \
	$(HOST_OS)/format.o         \
	$(HOST_OS)/groups.o         \
	$(HOST_OS)/lname.o          \
	$(HOST_OS)/table.o          \
	$(HOST_OS)/arbsem.o         \
	$(HOST_OS)/anasynt0.o       \
	$(HOST_OS)/encinfo.o

OBJ_TOOLKIT = $(OBJ_SXGENSYN:.o=_toolkit.o)

all: info dirs $(EXE_SXGENSYN) $(EXE_TOOLKIT)

$(EXE_SXGENSYN): $(OBJ_SXGENSYN) $(GENSYNDEPLIBS)
	@echo linking $@
	$(V) $(CC) -o $@ $^ $(LDFLAGS) $(GENSYNDEPLIBS)
	$(V) $(STRIP) $@

$(EXE_TOOLKIT): $(OBJ_TOOLKIT)
	@echo linking $@
	$(V) $(CC) -o $@ $^ $(LDFLAGS)
	$(V) $(STRIP) $@

$(HOST_OS)/%.o: $(DIR_GENSYN_SRC)/%.c
	@echo compiling $@
	$(V) $(CC) -c -o $@ $(CFLAGS) $<

$(HOST_OS)/%_toolkit.o: $(DIR_GENSYN_SRC)/%.c
	@echo compiling $@
	$(V) $(CC) -c -o $@ $(CFLAGS) -D__TOOLKIT__ $<

help:
	@echo -------------------------------------------------------
	@echo Makefile - generation of $(EXE_SXGENSYN) $(EXE_TOOLKIT)
	@echo
	@echo [all]    : build all
	@echo
	@echo [clean]  : delete objects and executable
	@echo
	@echo [install]: install toolkit binary in $(INSTALL_DIR)/bin as sxgensyn
	@echo
	@echo [help]   : display help information
	@echo
	@echo -------------------------------------------------------

debug:
	@echo DEBUG : --------------------------------------------------------------------------------
	@echo DEBUG : Compile with:
	@echo DEBUG : $(CC) -c -o {object} {source} $(CFLAGS)
	@echo DEBUG : $(CC) -c -o {object} {source} -D__TOOLKIT__ $(CFLAGS)
	@echo DEBUG : --------------------------------------------------------------------------------
	@echo DEBUG : Link with:
	@echo DEBUG : $(CC) -o {exec} {objects} $(LDFLAGS)
	@echo DEBUG : --------------------------------------------------------------------------------
	@echo DEBUG : VERSION=$(VERSION)
	@echo DEBUG : EXE_SXGENSYN=$(EXE_SXGENSYN)
	@echo DEBUG : EXE_TOOLKIT=$(EXE_TOOLKIT)
	@echo DEBUG : OBJ_SXGENSYN=$(OBJ_SXGENSYN)
	@echo DEBUG : OBJ_TOOLKIT=$(OBJ_TOOLKIT)
	@echo DEBUG : --------------------------------------------------------------------------------

info:
	@echo INFO : --------------------------------------------------------------------------------
	@echo INFO : Making $(STATUS) $(EXE) version V$(VERSION) for $(HOST_OS)
	@echo INFO : --------------------------------------------------------------------------------

clean:
	@echo cleaning
	$(V) rm -rf $(OBJ_SXGENSYN) $(OBJ_TOOLKIT) core

distclean: clean
	@echo distcleaning
	$(V) rm -rf $(EXE_SXGENSYN) $(EXE_TOOLKIT) $(HOST_OS)

install: $(EXE_TOOLKIT)
	$(V) cp $(EXE_TOOLKIT) $(INSTALL_DIR)/bin/$(EXE_SXGENSYN)
	$(V) chmod 755 $(INSTALL_DIR)/bin/$(EXE_SXGENSYN)

dirs:
	@if [ ! -d $(HOST_OS) ] ; then echo Making dir $(HOST_OS) ; mkdir $(HOST_OS) ; fi

