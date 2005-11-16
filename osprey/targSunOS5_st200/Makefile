#
#
#  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of version 2 of the GNU General Public License as
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it would be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
#
#  Further, this software is distributed without any warranty that it is
#  free of the rightful claim of any third person regarding infringement 
#  or the like.  Any license provided herein, whether implied or 
#  otherwise, applies only to this software file.  Patent licenses, if 
#  any, provided herein do not apply to combinations of this program with 
#  other software, or any other product whatsoever.  
#
#  You should have received a copy of the GNU General Public License along
#  with this program; if not, write the Free Software Foundation, Inc., 59
#  Temple Place - Suite 330, Boston MA 02111-1307, USA.
#
#  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
#  Mountain View, CA 94043, or:
#
#  http://www.sgi.com
#
#  For further information regarding this notice, see:
#
#  http://oss.sgi.com/projects/GenInfo/NoticeExplan
#
#

#
#  Makefile for targSunOS5_st200
#

#
#  These directories must be "built" first because they generate header files
#  which other directories depend on.
#  Please Specify dependencies for each new <directory> in the variable
#  DEPS_FOR_<directory>.
#
INCLUDE_SUBDIRS = \
	include \
	targ_info

DEPS_FOR_include	=
DEPS_FOR_targ_info	= include

#
#  Next, all of the libraries are built because the tools depend on them.
#  Please Specify dependencies for each new <directory> in the variable
#  DEPS_FOR_<directory>.
#
#### Only these for now #####
LIBRARY_SUBDIRS =  \
        libiberty  \
	libelf     \
	libcmplrs  \
	libcomutil \
	libdwarf   \
	libelfutil \
	libSYS     \
	instrumentationC

DEPS_FOR_libiberty	= include
DEPS_FOR_libelf		= include
DEPS_FOR_libcmplrs	= include
DEPS_FOR_libcomutil	= include
DEPS_FOR_libdwarf	= include
DEPS_FOR_libelfutil	= include
DEPS_FOR_libSYS		= include
DEPS_FOR_instrumentationC = include

#
#  Finally, the compiler tools (components) are built.
#
#  Please Specify dependencies for each new <directory> in the variable
#  DEPS_FOR_<directory>.
#
#  Note that be_driver must come last since it depends on other components
#  being built first.
#
TOOL_SUBDIRS = \
	gccfe \
	g++fe \
	wopt \
	be \
	lai \
        ipl \
	ipa \
	lno \
	lw_inline \
        whirl2c \
        ir_tools \
	driver 

#	dwarfdump \
#	elfdump \
#	whirl2f \

DEPS_FOR_gccfe	:= $(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS)
DEPS_FOR_g++fe	:= $(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS)
DEPS_FOR_gccfe	:= $(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS)
DEPS_FOR_be	:= $(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS)
DEPS_FOR_lai	=  be
DEPS_FOR_wopt	= be
DEPS_FOR_ipl	= be
DEPS_FOR_ipa	= be wopt ipl
DEPS_FOR_lno	= be wopt ipl
DEPS_FOR_lw_inline :=  $(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS) be
DEPS_FOR_whirl2c :=  $(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS) be
DEPS_FOR_whirl2f :=  $(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS) be
DEPS_FOR_elfdump :=  $(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS)
DEPS_FOR_dwarfdump :=  $(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS)
DEPS_FOR_ir_tools :=  $(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS) be
DEPS_FOR_driver :=  $(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS)

SUBDIRS := $(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS) $(TOOL_SUBDIRS)

.PHONY: default local install clean clobber last

default: $(addprefix default_,$(SUBDIRS))
local: $(addprefix local_,$(SUBDIRS))
install: $(addprefix install_,$(SUBDIRS))
clean: $(addprefix clean_,$(SUBDIRS))
clobber: $(addprefix clobber_,$(SUBDIRS))
last: $(addprefix last_,$(SUBDIRS))

clean_%:
	$(MAKE) -C $* clean
clobber_%:
	$(MAKE) -C $* clobber
default_%:
	$(MAKE) -C $* default
local_%:
	$(MAKE) -C $* local
install_%:
	$(MAKE) -C $* install
last_%:
	$(MAKE) -C $* last


ifndef NO_DEPS
# Now transform dependencies written above into Make dependencies

# Utility "functions"
subst_local=$(strip local_$(targ)): $(addprefix local_,$(DEPS_FOR_$(targ)))-sep-
subst_last=$(strip last_$(targ)): $(addprefix last_,$(DEPS_FOR_$(targ)))-sep-
subst_install=$(strip install_$(targ)): $(addprefix install_,$(DEPS_FOR_$(targ)))-sep-

# Generate "local" rules
# Transform each dependencies of the form
# "DEPS_FOR_targ1=dep1 dep2"
# "DEPS_FOR_targ2=dep3 dep4"
# into local_targ1: local_dep1 local_dep2-sep-local_targ2: local_dep3 local_dep4-sep-
# -sep- are transformed into newlines later
# Don't use $(SUBDIRS) in the dependency rules as SUBDIRS may be redefined
# on the make command line. For instance: make SUBDIRS=lai install.
LOCAL_DEP := $(foreach targ,$(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS) $(TOOL_SUBDIRS),$(subst_local))


# Same thing for "last" rules.
LAST_DEP := $(foreach targ,$(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS) $(TOOL_SUBDIRS),$(subst_last))

# Same thing for "install" rules.
INSTALL_DEP := $(foreach targ,$(INCLUDE_SUBDIRS)  $(LIBRARY_SUBDIRS) $(TOOL_SUBDIRS),$(subst_install))

# No need for dependencies for "clean" and "clobber" rules

# Generate the newly generated dependencies into an auxiliary Makefile fragment

-include deps.mk

deps.mk: Makefile
	@echo $(LOCAL_DEP) | perl -p -e 's/-sep- */\n/g' > $@
	@echo $(LAST_DEP) | perl -p -e 's/-sep- */\n/g' >> $@
	@echo $(INSTALL_DEP) | perl -p -e 's/-sep- */\n/g' >> $@

endif