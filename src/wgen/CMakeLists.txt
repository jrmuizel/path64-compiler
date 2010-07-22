include_directories(
  .
  ../include/gnu
  ../common/com
  ../common/util
  ${PATHSCALE_BINARY_DIR}/common/targ_info

  ../ipa_link/include
  ../ipa/common
  ../ipa/local
  ../ipa/main/analyze
  ../ipa/main/optimize
  ../../GCC/libspin
  ../be/com
  ${PATHSCALE_BINARY_DIR}/../GCC/gcc
  ../../GCC/gcc
  ../libair
  ../libfereconf
 
  ../common/targ_info/parser/inc/
  ../common/targ_info/access

  )

add_definitions(
# TODO : Try to minimize these
  -DFRONT_END
  -DUSE_DECL_SRCPOS
   ${PATH64_STDC_FORMAT_MACROS}
  -DNEW_SYMTAB
  -DFRONT_END_C
  -DCFE
  -DCIL
  -DDO_IL_LOWERING=0
  -DNO_USR_INCLUDE=TRUE
  -DAUTOMATIC_TEMPLATE_INSTANTIATION=0
  -DINSTANTIATION_BY_IMPLICIT_INCLUSION=0
  -DBACK_END_IS_C_GEN_BE=0
  -DMONGOOSE_CIF
  -DSGI_RAG_BACKEND
  -DSGI_MONGOOSE
  -DHANDLE_PRAGMA_WEAK
  -D__STDC_FORMAT_MACROS
  -DGPLUSPLUS_FE
  )

path64_set_multiarch_sources(wgen42_sources COMMON
  ../common/com/config.cxx
  ../common/com/const.cxx
  ../common/com/controls.cxx
  ../common/com/dwarf_DST.cxx
  ../common/com/dwarf_DST_dump.cxx
  ../common/com/dwarf_DST_mem.cxx
  ../common/com/dwarf_DST_producer.cxx
  ../common/com/err_host.cxx
  ../common/com/glob.cxx
  ../common/com/intrn_info.cxx
  ../common/com/ir_bcom.cxx
  ../common/com/ir_bwrite.cxx
  ../common/com/ir_reader.cxx
  ../common/com/irbdata.cxx
  ../common/com/mtypes.cxx
  ../common/com/opcode.cxx
  ../common/com/opcode_core.cxx
  ../common/com/pu_info.cxx
  ../common/com/strtab.cxx
  ../common/com/symtab.cxx
  ../common/com/symtab_verify.cxx
  ../common/com/ttype.cxx
  ../common/com/wn.cxx
  ../common/com/wn_map.cxx
  ../common/com/wn_pragmas.cxx
  ../common/com/wn_simp.cxx
  ../common/com/wn_util.cxx
  ../common/com/wutil.cxx
  ../common/com/xstats.cxx
  ../common/com/@ARCH@/config_targ.cxx
  ../common/com/@ARCH@/config_elf_targ.cxx
  ../common/com/@ARCH@/targ_const.cxx
  ../common/com/@ARCH@/targ_sim.cxx
  ../common/com/@ARCH@/config_host.c
  ../common/com/@ARCH@/config_platform.c
  wgen_misc.cxx
  wgen_expr.cxx
  wgen_stmt.cxx
  wgen_decl.cxx
  wgen_dst.cxx
  wgen_spin_symbol.cxx
  omp_directive.cxx
  wgen_omp_check_stack.cxx
  wgen_omp_directives.cxx
  main.c
  varasm.c
  c_int_model.c
  )
path64_set_multiarch_sources(wgen42_sources arm 
../common/com/config.cxx
  ../common/com/const.cxx
  ../common/com/controls.cxx
  ../common/com/dwarf_DST.cxx
  ../common/com/dwarf_DST_dump.cxx
  ../common/com/dwarf_DST_mem.cxx
  ../common/com/dwarf_DST_producer.cxx
  ../common/com/err_host.cxx
  ../common/com/glob.cxx
  ../common/com/intrn_info.cxx
  ../common/com/ir_bcom.cxx
  ../common/com/ir_bwrite.cxx
  ../common/com/ir_reader.cxx
  ../common/com/irbdata.cxx
  ../common/com/mtypes.cxx
  ../common/com/opcode.cxx
  ../common/com/opcode_core.cxx
  ../common/com/pu_info.cxx
  ../common/com/strtab.cxx
  ../common/com/symtab.cxx
  ../common/com/symtab_verify.cxx
  ../common/com/ttype.cxx
  ../common/com/wn.cxx
  ../common/com/wn_map.cxx
  ../common/com/wn_pragmas.cxx
  ../common/com/wn_simp.cxx
  ../common/com/wn_util.cxx
  ../common/com/wutil.cxx
  ../common/com/xstats.cxx
  ../common/com/@ARCH@/config_targ.cxx
  ../common/com/@ARCH@/config_elf_targ.cxx
  ../common/com/@ARCH@/targ_const.cxx
  ../common/com/@ARCH@/targ_sim.cxx
  ../common/com/@ARCH@/config_host.c
  ../common/com/@ARCH@/config_platform.c
  wgen_misc.cxx
  wgen_expr.cxx
  wgen_stmt.cxx
  wgen_decl.cxx
  wgen_dst.cxx
  wgen_spin_symbol.cxx
  omp_directive.cxx
  wgen_omp_check_stack.cxx
  wgen_omp_directives.cxx
  main.c
  varasm.c
  c_int_model.c 
                         wgen_tracing.cxx 
                         wgen_pragmas.cxx
                         ../common/com/gccfe_targinfo_interface.cxx #st
                         ../common/util/vspec_parse.c
                         ../common/util/vspec_token.c
                         ../common/util/vspec.c
                         ../common/com/@ARCH@/targ_register_common.cxx )

set_property(SOURCE ../common/com/wn_util.cxx PROPERTY COMPILE_DEFINITIONS MONGOOSE_BE)

path64_add_multiarch_executable(wgen42 wgen42_sources)
path64_multiarch_link_multiarch_libraries(wgen42 comutil targinfo)
path64_is_arch_enabled(arm_enabled arm)
if(${arm_enabled})
path64_multiarch_link_libraries(wgen42 fereconf iberty cmplrs m)
else()
path64_multiarch_link_libraries(wgen42 iberty cmplrs m)
endif()
path64_multiarch_link_multiarch_libraries(wgen42 spin)

path64_set_multiarch_property(wgen42 COMPILE_FLAGS
                              "-I${CMAKE_CURRENT_SOURCE_DIR}/../common/com/@ARCH@ -I${PATHSCALE_BINARY_DIR}/common/targ_info/@ARCH@")

#add_custom_command(
#    OUTPUT ${CMAKE_BINARY_DIR}/lib/wgen42
#    DEPENDS wgen42
#    COMMAND ln -s ../bin/wgen42 ${CMAKE_BINARY_DIR}/lib
#)
#
#add_custom_target(
#    stageit-wgen42 ALL
#    DEPENDS ${CMAKE_BINARY_DIR}/lib/wgen42
#)

#install(TARGETS wgen42
#        RUNTIME DESTINATION ${PATH64_LIB_PATH})
#install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"${CMAKE_INSTALL_PREFIX}/${PATH64_LIB_PATH}/wgen42${CMAKE_EXECUTABLE_SUFFIX}\" \"${CMAKE_INSTALL_PREFIX}/${PATH64_LIB_PATH}/wgen${CMAKE_EXECUTABLE_SUFFIX}\")")
