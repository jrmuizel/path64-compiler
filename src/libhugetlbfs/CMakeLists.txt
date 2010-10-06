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
    path64_set_multitarget_sources(hugetlbfs-psc COMMON ${PATHSCALE_LIBHUGETLBFS_SRC})
    path64_set_multitarget_property_(hugetlbfs-psc COMPILE_FLAGS -fPIC)
    path64_add_multitarget_library(hugetlbfs-psc SHARED)
    path64_add_multitarget_dependencies(hugetlbfs-psc pscrt-static)

    path64_set_multitarget_sources(hugetlbfs-psc-static COMMON ${PATHSCALE_LIBHUGETLBFS_SRC})
    path64_set_multitarget_property_(hugetlbfs-psc-static OUTPUT_NAME "hugetlbfs-psc")
    path64_add_multitarget_library(hugetlbfs-psc-static STATIC)

    IF(PATH64_ENABLE_PROFILING)
        path64_set_multitarget_sources(hugetlbfs-psc_p COMMON ${PATHSCALE_LIBHUGETLBFS_SRC})
        path64_set_multitarget_property_(hugetlbfs-psc_p COMPILE_FLAGS -pg)
        path64_add_multitarget_library(hugetlbfs-psc_p STATIC)
    ENDIF(PATH64_ENABLE_PROFILING)

    # linker scripts
    set(linker_scripts)
    foreach(targ ${PATH64_ENABLE_TARGETS})
        set(arch ${_PATH64_TARGET_ARCH_${targ}})
        set(bits ${_PATH64_TARGET_BITS_${targ}})

        add_custom_command(OUTPUT ${Path64_BINARY_DIR}/lib/${arch}/${bits}/elf.xB
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different
                                   ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/elf_${targ}.xB
                                   ${Path64_BINARY_DIR}/lib/${arch}/${bits}/elf.xB
                           DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/elf_${targ}.xB)

        add_custom_command(OUTPUT ${Path64_BINARY_DIR}/lib/${arch}/${bits}/elf_1G.xBDT
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different
                                   ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/elf_${targ}_1G.xBDT
                                   ${Path64_BINARY_DIR}/lib/${arch}/${bits}/elf_1G.xBDT
                           DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/elf_${targ}_1G.xBDT)

        add_custom_command(OUTPUT ${Path64_BINARY_DIR}/lib/${arch}/${bits}/elf.xBDT
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different
                                   ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/elf_${targ}.xBDT
                                   ${Path64_BINARY_DIR}/lib/${arch}/${bits}/elf.xBDT
                           DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/elf_${targ}.xBDT)

        set(l_scripts
                    ${Path64_BINARY_DIR}/lib/${arch}/${bits}/elf.xB
                    ${Path64_BINARY_DIR}/lib/${arch}/${bits}/elf_1G.xBDT
                    ${Path64_BINARY_DIR}/lib/${arch}/${bits}/elf.xBDT)

        install(FILES ${l_scripts}
                DESTINATION ${PATH64_LIB_PATH}/${arch}/${bits})

        list(APPEND linker_scripts ${l_scripts})
    endforeach()

    add_custom_target(hugetlbfs-scripts ALL DEPENDS ${linker_scripts})
    add_dependencies(compiler-stage-libs-C hugetlbfs-scripts)

ENDIF(PATH64_ENABLE_HUGEPAGES)
