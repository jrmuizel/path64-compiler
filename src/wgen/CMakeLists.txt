include_directories(
  .
  ../include/gnu
  ../common/com
  ../common/com/${BUILD_TARGET_DIR}
  ../common/util

  ../ipa/common
  ../ipa/local
  ../ipa/main/analyze
  ../ipa/main/optimize

  ../be/com
  )

add_definitions(
# TODO : Try to minimize these
  -DFRONT_END
  -DUSE_DECL_SRCPOS
  -D__STDC_FORMAT_MACROS
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
  -DGPLUSPLUS_FE
  )

set(wgen42_sources
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
  ../common/com/${BUILD_TARGET_DIR}/config_targ.cxx
  ../common/com/${BUILD_TARGET_DIR}/config_elf_targ.cxx
  ../common/com/${BUILD_TARGET_DIR}/targ_const.cxx
  ../common/com/${BUILD_TARGET_DIR}/targ_sim.cxx
  ../common/com/${BUILD_TARGET_DIR}/config_host.c
  ../common/com/${BUILD_TARGET_DIR}/config_platform.c
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

set_property(SOURCE ../common/com/wn_util.cxx PROPERTY COMPILE_DEFINITIONS MONGOOSE_BE)

add_executable(wgen42 ${wgen42_sources})
target_link_libraries(wgen42 comutil iberty spin cmplrs m)

install(TARGETS wgen42
        RUNTIME DESTINATION ${PATH64_LIB_PATH})
#install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"${CMAKE_INSTALL_PREFIX}/${PATH64_LIB_PATH}/wgen42${CMAKE_EXECUTABLE_SUFFIX}\" \"${CMAKE_INSTALL_PREFIX}/${PATH64_LIB_PATH}/wgen${CMAKE_EXECUTABLE_SUFFIX}\")")
