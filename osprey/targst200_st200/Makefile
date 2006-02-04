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
#  Makefile for targst200_st200
#

#
#  These directories must be "built" first because they generate header files
#  which other directories depend on.
#  Please Specify dependencies for each new <directory> in the variable
#  DEPS_FOR_<directory>.
#
INCLUDE_SUBDIRS = 

#
#  Next, all of the libraries are built because the tools depend on them.
#  Please Specify dependencies for each new <directory> in the variable
#  DEPS_FOR_<directory>.
#
LIBRARY_SUBDIRS =  


#
#  Finally, the compiler tools (components) are built.
#
#  Please Specify dependencies for each new <directory> in the variable
#  DEPS_FOR_<directory>.
#
#
TOOL_SUBDIRS = \
	instrumentationC \
	libunwind \
	libgcov

DEPS_FOR_instrumentationC =
DEPS_FOR_libgcov =

SUBDIRS := $(INCLUDE_SUBDIRS) $(LIBRARY_SUBDIRS) $(TOOL_SUBDIRS)

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
	@echo $(LOCAL_DEP) | /usr/bin/perl -p -e 's/-sep- */\n/g' > $@
	@echo $(LAST_DEP) | /usr/bin/perl -p -e 's/-sep- */\n/g' >> $@
	@echo $(INSTALL_DEP) | /usr/bin/perl -p -e 's/-sep- */\n/g' >> $@

endif
