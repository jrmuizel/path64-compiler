
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
"../common/com/x8664/config_elf_targ.cxx"
"../common/com/x8664/config_targ.cxx"
"../common/com/x8664/targ_const.cxx"
"../common/com/x8664/targ_sim.cxx"
"../common/com/x8664/config_host.c"
"../common/com/x8664/config_platform.c"
)

INCLUDE_DIRECTORIES(
                    .
					../common/com
					../common/com/x8664
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
ADD_EXECUTABLE(ir_b2a ${IR_B2A_SRCS} ${IR_COMMON_SRCS})
ADD_EXECUTABLE(ir_size ${IR_SIZE_SRCS} ${IR_COMMON_SRCS})

target_link_libraries(ir_b2a comutil m )
target_link_libraries(ir_size comutil m )

install(TARGETS ir_b2a ir_size
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${PATH64_LIB_PATH})



install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"${CMAKE_INSTALL_PREFIX}/bin/ir_b2a\" \"${CMAKE_INSTALL_PREFIX}/bin/ir_sel\")")
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"${CMAKE_INSTALL_PREFIX}/bin/ir_b2a\" \"${CMAKE_INSTALL_PREFIX}/bin/ir_all\")")
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
