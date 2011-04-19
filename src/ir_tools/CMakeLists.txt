
set(IR_SIZE_SRCS
"ir_size.cxx"
)

set(IR_B2A_SRCS 
"ir_a2b.cxx"
)

set (IR_COMMON_SRCS
"../common/com/config.cxx"
"../common/com/const.cxx"
"../common/com/dwarf_DST.cxx"
"../common/com/dwarf_DST_dump.cxx"
"../common/com/dwarf_DST_mem.cxx"
"../common/com/glob.cxx"
"../common/com/ir_bcom.cxx"
"../common/com/ir_bread.cxx"
"../common/com/ir_bwrite.cxx"
"../common/com/ir_reader.cxx"
"../common/com/irbdata.cxx"
"../common/com/mtypes.cxx"
"../common/com/opcode.cxx"
"../common/com/opcode_core.cxx"
"../common/com/pu_info.cxx"
"../common/com/strtab.cxx"
"../common/com/symtab.cxx"
"../common/com/symtab_verify.cxx"
"../common/com/wn.cxx"
"../common/com/wn_map.cxx"
"../common/com/wn_pragmas.cxx"
"../common/com/wn_simp.cxx"
"../common/com/wn_util.cxx"
"../common/com/wutil.cxx"
"../common/com/xstats.cxx"
"../common/com/@ARCH@/config_elf_targ.cxx"
"../common/com/@ARCH@/config_targ.cxx"
"../common/com/@ARCH@/targ_const.cxx"
"../common/com/@ARCH@/targ_sim.cxx"
"../common/com/@ARCH@/config_host.c"
"../common/com/@ARCH@/config_platform.c"
)

INCLUDE_DIRECTORIES(
                    .
                                       ${PATHSCALE_BINARY_DIR}/common/targ_info				
                                 	../common/com
					../common/com/@ARCH@
					../common/util
					../ipa/common
					../ipa/local
					../ipa/main/analyze
					../ipa/main/optimize
					../be/com
					../be/opt
					../be/region)

ADD_DEFINITIONS(-DIR_TOOLS
                ${PATH64_STDC_FORMAT_MACROS})

set_property(SOURCE ${IR_COMMON_SRCS} PROPERTY COMPILE_DEFINITIONS __MIPS_AND_IA64_ELF_H;MONGOOSE_BE)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")

path64_set_multiarch_sources(ir_b2a_sources COMMON ${IR_B2A_SRCS} ${IR_COMMON_SRCS})
path64_add_multiarch_executable(ir_b2a ir_b2a_sources)
path64_multiarch_link_multiarch_libraries(ir_b2a comutil targinfo)
path64_multiarch_link_libraries(ir_b2a m)
path64_set_multiarch_property(ir_b2a COMPILE_FLAGS
                              "-I${CMAKE_CURRENT_SOURCE_DIR}/../common/com/@ARCH@ -I${PATHSCALE_BINARY_DIR}/common/targ_info/@ARCH@")
path64_install_multiarch(ir_b2a)

path64_set_multiarch_sources(ir_size_sources COMMON ${IR_SIZE_SRCS} ${IR_COMMON_SRCS})
path64_add_multiarch_executable(ir_size ir_size_sources)
path64_multiarch_link_multiarch_libraries(ir_size comutil targinfo)
path64_multiarch_link_libraries(ir_size m)
path64_set_multiarch_property(ir_size COMPILE_FLAGS
                              "-I${CMAKE_CURRENT_SOURCE_DIR}/../common/com/@ARCH@ -I${PATHSCALE_BINARY_DIR}/common/targ_info/@ARCH@")
path64_install_multiarch(ir_size)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows") 
    path64_multiarch_link_libraries(ir_b2a winlib iberty)
    path64_multiarch_link_libraries(ir_size winlib iberty)
endif() 


foreach(arch ${PATH64_ENABLE_ARCHES})
    install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${PATH64_LIB_PATH}/${arch}/ir_b2a\" \"${CMAKE_INSTALL_PREFIX}/bin/ir_b2a-${arch}\")")
    install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${PATH64_LIB_PATH}/${arch}/ir_size\" \"${CMAKE_INSTALL_PREFIX}/bin/ir_size-${arch}\")")
    install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/ir_b2a-${arch}\" \"${CMAKE_INSTALL_PREFIX}/bin/ir_sel-${arch}\")")
    install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/ir_b2a-${arch}\" \"${CMAKE_INSTALL_PREFIX}/bin/ir_all-${arch}\")")
endforeach()


ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
