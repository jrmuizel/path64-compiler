set(PATHSCALE_LIBHUGETLBFS_SRC	"elflink.c"
				"sys-elf_x86_64.S"
				"hugeutils.c"
				"version.c"
				"init.c"
				"morecore.c"
				"debug.c")

ADD_DEFINITIONS(
	-D__LIBHUGETLBFS__
	-DOPEN64_MOD
	-DM_PAGE
	-fPIC
)
IF(PATH64_ENABLE_HUGEPAGES)
ADD_LIBRARY(hugetlbfs-psc SHARED ${PATHSCALE_LIBHUGETLBFS_SRC})
ADD_LIBRARY(hugetlbfs-psc-static STATIC ${PATHSCALE_LIBHUGETLBFS_SRC})
SET_TARGET_PROPERTIES(hugetlbfs-psc-static PROPERTIES OUTPUT_NAME "hugetlbfs-psc")
set_property(TARGET hugetlbfs-psc PROPERTY SOVERSION 1)
install(TARGETS hugetlbfs-psc hugetlbfs-psc-static
        LIBRARY DESTINATION ${PATH64_LIB_PATH}
        ARCHIVE DESTINATION ${PATH64_LIB_PATH})


IF(PATH64_ENABLE_PROFILING)
add_library(hugetlbfs-psc_p STATIC ${PATHSCALE_LIBHUGETLBFS_SRC})
set_property(TARGET hugetlbfs-psc_p PROPERTY COMPILE_FLAGS -pg)
install(TARGETS hugetlbfs-psc_p
        LIBRARY DESTINATION ${PATH64_LIB_PATH}
        ARCHIVE DESTINATION ${PATH64_LIB_PATH})

ENDIF(PATH64_ENABLE_PROFILING)


# TODO : Install these linker scripts
# ldscripts/elf_i386.xBDT
# ldscripts/elf_i386.xB
# ldscripts/elf_i386_1G.xBDT
install(FILES ldscripts/elf_x86_64.xB ldscripts/elf_x86_64_1G.xBDT ldscripts/elf_x86_64.xBDT DESTINATION ${PATH64_LIB_PATH}/lib)
ENDIF(PATH64_ENABLE_HUGEPAGES)
