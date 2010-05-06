

SET(LINUX_HFILES
	bstring.h
	elf_stuff.h
	sgidefs.h
	standards.h
   )

SET(LINUX_SYS_HFILES
	syssgi.h)

SET(INCLUDE_HFILES
	compact_reloc.h
	dwarf.h
	elf_abi.h
	elf_mips.h
	elfaccess.h
	fp_class.h
	isam.h
	libdwarf.h
	libXlate.h
	objlist.h
	obj_list.h
	obj_type.h
	rld_interface.h
	stamp.h
	svr4_math.h
   )


IF(${BUILD_OS} MATCHES "DARWIN")
SET(INCLUDE_HFILES
    ${INCLUDE_HFILES}
	darwin_elf.h
   )
ENDIF(${BUILD_OS} MATCHES "DARWIN")

SET(CMPLR_HFILES
	cif_c.h
	dwarf_addr_finder.h
	elf_interfaces.h
	f_errno.h
	events.h
	fb.h
	host.h
	leb128.h
	make_depend.h
	newinst.h
	rcodes.h
	xlate.h
   )

SET(CMPLR_DIRS cmplrs)

SET(SYS_HFILES
	elf_whirl.h
	inst.h
	unwind.h
	unwindP.h
   )

IF(${BUILD_TARGET} MATCHES "IA64")
    SET(SYS_DIRS ia64)
ENDIF(${BUILD_TARGET} MATCHES "IA64")

IF(${BUILD_TARGET} MATCHES "MIPS")
    set(SYS_DIRS  ia64)
ENDIF(${BUILD_TARGET} MATCHES "MIPS")

IF(${BUILD_TARGET} MATCHES "X8664")
    set(SYS_DIRS  ia64)
ENDIF(${BUILD_TARGET} MATCHES "X8664")

SET(LIBELF_HFILES
	elf_repl.h
	libelf.h
	nlist.h
	sys_elf.h
   )

SET(LDIRT dwarf.h)


## To prevent repetitious submakes to this subdirectory, Makefile.gsetup
## files in other subdirectories invoke submake here only when the file
## pathscale_defs.h is absent or out of date.  The dependencies for
## pathscale_defs.h are provided in linux/make/gcommonrules for
## Makefile.gbase files in other build subdirectories.
#
#default: pathscale_defs.h
#	@: Make directories in case they do not exist.
#	@for d in sys cmplrs libelf; do \
#	    if [ ! -d $$d ]; then \
#		mkdir $$d && echo Making $$d; \
#	    fi; \
#	done	A


SET(PATHSCALE_INCLUDE_DIR ${PATHSCALE_BINARY_DIR}/include)

FILE(MAKE_DIRECTORY ${PATHSCALE_INCLUDE_DIR}/sys)
FILE(MAKE_DIRECTORY ${PATHSCALE_INCLUDE_DIR}/cmplrs)
FILE(MAKE_DIRECTORY ${PATHSCALE_INCLUDE_DIR}/libelf)

#	@for h in $(LINUX_HFILES); do \
#	    if ! test -e $$h; then \
#	      ln -sf $(BUILD_TOT)/linux/include/$$h $$h; \
#	    fi; \
#	done
FOREACH(H ${LINUX_HFILES})
    CONFIGURE_FILE(linux/include/${H} ${PATHSCALE_INCLUDE_DIR}/${H})
ENDFOREACH(H)


#	@for h in $(LINUX_SYS_HFILES); do \
#	    if ! test -e sys/$$h; then \
#	      ln -sf $(BUILD_TOT)/../linux/include/sys/$$h sys/$$h; \
#	    fi; \
#	done
FOREACH(H ${LINUX_SYS_HFILES})
    CONFIGURE_FILE(linux/include/sys/${H} ${PATHSCALE_INCLUDE_DIR}/sys/${H})
ENDFOREACH(H)


#	@for h in $(INCLUDE_HFILES); do \
#	    if ! test -e $$h; then \
#	      ln -sf $(BUILD_TOT)/include/$$h $$h; \
#	    fi; \
#	done
FOREACH(H ${INCLUDE_HFILES})
    CONFIGURE_FILE(include/${H} ${PATHSCALE_INCLUDE_DIR}/${H})
ENDFOREACH(H)


#	@for h in $(CMPLR_DIRS); do \
#	    if ! test -e $$h; then \
#	      mkdir $$h; \
#	    fi; \
#	done
FILE(MAKE_DIRECTORY ${PATHSCALE_INCLUDE_DIR}/cmplrs)


#	@for h in $(CMPLR_HFILES); do \
#	    if ! test -e cmplrs/$$h; then \
#	      ln -sf $(BUILD_TOT)/../include/cmplrs/$$h cmplrs/$$h; \
#	    fi; \
#	done
FOREACH(H ${CMPLR_HFILES})
    CONFIGURE_FILE(include/cmplrs/${H} ${PATHSCALE_INCLUDE_DIR}/cmplrs/${H})
ENDFOREACH(H)


#	@for h in $(SYS_HFILES); do \
#	    if ! test -e sys/$$h; then \
#	      ln -sf $(BUILD_TOT)/../include/sys/$$h sys/$$h; \
#	    fi; \
#	done
FOREACH(H ${SYS_HFILES})
    CONFIGURE_FILE(include/sys/${H} ${PATHSCALE_INCLUDE_DIR}/sys/${H})
ENDFOREACH(H)


#	@for h in $(SYS_DIRS); do \
#	    if ! test -e sys/$$h; then \
#	      ln -sf $(BUILD_TOT)/../include/sys/$$h sys/$$h; \
#	    fi; \
#	done
FOREACH(H ${SYS_DIRS})
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E create_symlink include/sys/${H}
                    ${PATHSCALE_INCLUDE_DIR}/sys/${H}
                    )
ENDFOREACH(H)


#
## If you change these prerequisites, also update pathscale_defs.h in
## linux/make/gcommonrules.
#pathscale_defs.h: $(BUILD_TOT)/include/pathscale_defs.h
#	cp $(BUILD_TOT)/include/pathscale_defs.h $@
#
CONFIGURE_FILE(include/pathscale_defs.h.in
               ${PATHSCALE_INCLUDE_DIR}/pathscale_defs.h)



FOREACH(H ${LIBELF_HFILES})
    CONFIGURE_FILE(libelf/lib/${H} ${PATHSCALE_INCLUDE_DIR}/libelf/${H})
ENDFOREACH(H)


INCLUDE_DIRECTORIES(BEFORE SYSTEM include ${PATHSCALE_INCLUDE_DIR})


install(FILES include/omp/omp_lib.h
              include/omp/omp_lib.f
        DESTINATION include)

#clean:
#
#clobber:
#	@rm -rf *.h *.sg $(CMPLR_DIRS) ia64 stl sys

