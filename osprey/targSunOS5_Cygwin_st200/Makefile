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
#  Makefile for targSunOS5_MingW_st200
#

#
#  These directories must be "built" first because they generate header files
#  which other directories depend on.
#
INCLUDE_SUBDIRS = \
	include \
	targ_info

#
#  Next, all of the libraries are built because the tools depend on them.
#
#### Only these for now #####
LIBRARY_SUBDIRS =  \
        libiberty  \
	libelf     \
	libcmplrs  \
	libcomutil \
	libdwarf   \
	libelfutil \
        libSYS 


#
#  Finally, the compiler tools (components) are built.
#
#  Note that be_driver must come last since it depends on other components
#  being built first.
#
TOOL_SUBDIRS = \
	gccfe \
	g++fe \
	be \
	lai \
        ipl \
	ipa \
	wopt \
	lno \
	lw_inline \
        whirl2c \
	driver 

#        ir_tools 
#	dwarfdump \
#	elfdump \
#	whirl2f \

SUBDIRS=$(INCLUDE_SUBDIRS) $(LIBRARY_SUBDIRS) $(TOOL_SUBDIRS)

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


# Dependencies
targ_info=include
libiberty=include
libelf=include
libcmplrs=include
libcomutil=include
libdwarf=include
libelfutil=include
libSYS=include
be=include
driver=include libSYS
gccfe=libcmplrs libcomutil
g++fe=libcmplrs libcomutil
be=targ_info
lai=libelf libelfutil libdwarf be targ_info
ipl=be wopt
ipa=be ipl
wopt=be
lno=be wopt ipl
lw_inline=be
whirl2c=be


# Now transform dependencies written above into Make dependencies

# Utility "functions"
subst_local=$(strip local_$(targ)): $(addprefix local_,$($(targ)))-sep-
subst_install=$(strip install_$(targ)): $(addprefix install_,$($(targ)))-sep-

# Generate "local" rules
# Transform each dependencies of the form
# "targ1=dep1 dep2"
# "targ2=dep3 dep4"
# into local_targ1: local_dep1 local_dep2-sep-local_targ2: local_dep3 local_dep4-sep-
# -sep- are transformed into newlines later

LOCAL_DEP := $(foreach targ,$(SUBDIRS),$(subst_local))


# Same thing for "install" rules.
INSTALL_DEP := $(foreach targ,$(SUBDIRS),$(subst_install))

# No need for dependencies for "clean" and "clobber" rules

# Generate the newly generated dependencies into an auxiliary Makefile fragment

-include deps.mk

deps.mk: Makefile
	@echo $(LOCAL_DEP) | perl -p -e 's/-sep- */\n/g' > $@
	@echo $(INSTALL_DEP) | perl -p -e 's/-sep- */\n/g' >> $@

