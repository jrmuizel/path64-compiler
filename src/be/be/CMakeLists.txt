PROJECT(BE)

SET(PATHSCALE_BE_BE_COMMON_SRCS "../../common/targ_info/access/ti_init.c"
                         "../../common/targ_info/access/ti_res.c"
                         "../../common/targ_info/access/ti_res_count.c"
                         "../../common/targ_info/access/ti_res_res.c"
                         "../../common/util/c_a_to_q.c"
                         "../../common/util/c_q_rel.c"
                         "../../common/util/c_q_to_a.c"
                         "../../common/util/c_qtenscale.c"
                         "../../common/util/@ARCH@/c_qwmultu.c"
                         "../../common/util/quadsim.c"
                         "../../common/util/bitset.c"
                         "../../common/util/dso.c"
                         "../../common/util/file_util.c"
                         "../../common/util/flags.c"
                         "../../common/util/memory.c"
                         "../../common/util/priority_queue.c"
                         "../../common/util/resource.c"
                         "../../common/util/tlog.c"
                         "../../common/util/tracing.c"
                         "../../common/util/util.c"
                         "../../common/util/vstring.c"
                         "../../common/com/@ARCH@/config_host.c"
                         "../../common/com/@ARCH@/config_platform.c"
                         "cleanup.cxx"
                         "fill_align.cxx"
                         "goto_conv.cxx"
                         "mem_ctr.cxx"
                         "dra_ec.cxx"
                         "dra_file_util.cxx"
                         "dra_clone.cxx"
                         "dra_mangle.cxx"
                         "omp_lower.cxx"
                         "../com/privatize_common.cxx"
                         "rewrite_pragmas.cxx"
                         "wb_omp.cxx"
                         "wb_lwr.cxx"
                         "wb_anl.cxx"
                         "@ARCH@/fill_align_targ.cxx"
                         "@ARCH@/driver_targ.cxx"
                         "../com/be_util.cxx"
                         "../com/be_version.cxx"
                         "../com/cxx_base.cxx"
                         "../com/cxx_graph.cxx"
                         "../com/cxx_hash_util.cxx"
                         "../com/mfmc_misc.cxx"
                         "../com/mfmc_query.cxx"
                         "../com/mfmc_setup.cxx"
                         "../com/mfmc_solve.cxx"
                         "../com/opt_addr_flags.cxx"
                         "../com/opt_cvtl_rule.cxx"
                         "../com/printsrc.cxx"
                         "../com/timing.cxx"
                         "../com/weak.cxx"
                         "../com/output_func_start_profiler.cxx"
                         "../com/wn_cyg_instrument.cxx"
                         "../com/wn_instrument.cxx"
                         "../com/ipa_lno_file.cxx"
                         "../com/ipa_lno_summary.cxx"
                         "../com/ipa_lno_util.cxx"
                         "../com/be_symtab.cxx"
                         "../com/data_layout.cxx"
                         "../com/dep_graph.cxx"
                         "../com/dvector.cxx"
                         "../com/emulate.cxx"
                         "../com/fb_cfg.cxx"
                         "../com/fb_whirl.cxx"
                         "../com/opt_alias_mgr.cxx"
                         "../com/opt_alias_rule.cxx"
                         "../com/opt_goto.cxx"
                         "../com/opt_ipaa_io.cxx"
                         "../com/opt_points_to_non_template.cxx"
                         "../com/prompf.cxx"
                         "../com/standardize.cxx"
                         "../com/stblock.cxx"
                         "../com/w2op.cxx"
                         "../com/whirl2src.cxx"
                         "../com/wn_fio.cxx"
                         "../com/wn_lower.cxx"
                         "../com/u64_lower_wn.cxx"
                         "../com/wn_mp.cxx"
                         "../com/wn_verifier.cxx"
                         "../com/wn_retype_expr.cxx"
                         "../com/wn_reassoc.cxx"
                         "../com/wb.cxx"
                         "../com/wb_browser.cxx"
                         "../com/wb_buffer.cxx"
                         "../com/wb_carray.cxx"
                         "../com/wb_ipl.cxx"
                         "../com/wb_util.cxx"
                         "${PATHSCALE_BINARY_DIR}/be/preg_list.cxx"
                         "../com/clone.cxx"
                         "../com/clone_DST_utils.cxx"
                         "../com/com_whirlview.cxx"
                         "../../common/com/config.cxx"
                         "../../common/com/config_cache.cxx"
                         "../../common/com/dra_demangle.cxx"
                         "../../common/com/dwarf_DST.cxx"
                         "../../common/com/dwarf_DST_dump.cxx"
                         "../../common/com/dwarf_DST_producer.cxx"
                         "../../common/com/fb_info.cxx"
                         "../../common/com/intrn_info.cxx"
                         "../../common/com/ll.cxx"
                         "../../common/com/mtypes.cxx"
                         "../../common/com/opcode.cxx"
                         "../../common/com/opcode_core.cxx"
                         "../../common/com/wutil.cxx"
                         "../com/DaVinci.cxx"
                         "../../common/com/const.cxx"
                         "../../common/com/dwarf_DST_mem.cxx"
                         "../../common/com/err_host.cxx"
                         "../../common/com/f90_utils.cxx"
                         "../../common/com/glob.cxx"
                         "../../common/com/ir_bcom.cxx"
                         "../../common/com/ir_bread.cxx"
                         "../../common/com/ir_bwrite.cxx"
                         "../../common/com/ir_reader.cxx"
                         "../../common/com/irbdata.cxx"
                         "../../common/com/pu_info.cxx"
                         "../../common/com/strtab.cxx"
                         "../../common/com/symtab.cxx"
                         "../../common/com/symtab_verify.cxx"
                         "../../common/com/ttype.cxx"
                         "../../common/com/wn.cxx"
                         "../../common/com/wn_map.cxx"
                         "../../common/com/wn_pragmas.cxx"
                         "../../common/com/wn_simp.cxx"
                         "../../common/com/wn_util.cxx"
                         "../../common/com/xstats.cxx"
                         "../../common/util/cxx_memory.cxx"
                         "../../common/util/errors.cxx"
                         "../../common/util/options_stack.cxx"
                         "../region/ori.cxx"
                         "../region/rbi.cxx"
                         "../region/region_init.cxx"
                         "../region/region_bounds.cxx"
                         "../region/region_util.cxx"
                         "../lno/soe.cxx"
                         "../lno/mat.cxx"
                         "../vho/f90_lower.cxx"
                         "../vho/f90_lower_dep.cxx"
                         "../vho/vho_lower.cxx"
                         "../vho/wb_f90_lower.cxx"
                         "../com/@ARCH@/betarget.cxx"
                         "../com/@ARCH@/sections.cxx"
                         "../../common/com/@ARCH@/config_cache_targ.cxx"
                         "../../common/com/@ARCH@/config_targ.cxx"
                         "../../common/com/@ARCH@/config_elf_targ.cxx"
                         "../../common/com/@ARCH@/targ_const.cxx"
                         "../../common/com/@ARCH@/targ_sim.cxx"
                         "../../common/instrument/instr_reader.cxx"
                         "../../ipa/main/optimize/ipo_tlog_utils.cxx")

SET(PATHSCALE_BE_BE_X8664_SRCS  "../com/wn_unroll.cxx")

SET(PATHSCALE_BE_BE_ARM_SRCS
                           "../com/wn_lower_mul.cxx"
                         "../com/rt_lower_wn.cxx"
                         "@ARCH@/mul_cost_model.cxx" #st
                         "../../common/util/zint.cxx" #st
                         "../../common/util/lbitmask.cxx" #st
                         "../../common/util/lbrange.cxx" #st
                         "../../common/util/larange.cxx" #st
                         "../../common/util/lvrange.cxx" #st
                         "../../common/util/lbvalrange.cxx" #st
                         "../../common/util/lbitvalue.cxx" #st
                         "../../common/util/lalign.cxx" #st
                         "../../common/util/paired_lattice.cxx" #st
                         "../../common/util/range.cxx" #st
                         "../../common/com/loader.cxx"
                         "../../common/com/isa_loader.cxx" #st
                         "../../common/com/dll_loader.cxx" #st
                         "../../common/com/lai_loader.cxx" #st
                         "../../common/com/extension_intrinsic.cxx" #st
                         "../../common/com/dyn_isa_api_access.cxx" #st
                         "../../common/com/dyn_dll_api_access.cxx" #st
                           )

path64_is_arch_enabled(arm_enabled arm)
if(${arm_enabled})
SET( PATHSCALE_BE_BE_SRCS ${PATHSCALE_BE_BE_COMMON_SRCS} ${PATHSCALE_BE_BE_ARM_SRCS})
else()
SET( PATHSCALE_BE_BE_SRCS ${PATHSCALE_BE_BE_COMMON_SRCS} ${PATHSCALE_BE_BE_X8664_SRCS})
endif()
ADD_DEFINITIONS(
# Apparently someone likes legos
	-D_LEGO_CLONER)

add_custom_command(
  OUTPUT ${PATHSCALE_BINARY_DIR}/be/preg_list.cxx ${PATHSCALE_BINARY_DIR}/be/preg_list.h
  COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/com/gen_preg_list.sh ${PATHSCALE_SOURCE_DIR}/common/util
  DEPENDS ${PATHSCALE_SOURCE_DIR}/be/com/gen_preg_list.sh
  WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
  )
SET_SOURCE_FILES_PROPERTIES(${PATHSCALE_BINARY_DIR}/be/preg_list.h PROPERTIES GENERATED 1)
SET_SOURCE_FILES_PROPERTIES(${PATHSCALE_BINARY_DIR}/be/preg_list.cxx PROPERTIES GENERATED 1)
ADD_CUSTOM_TARGET(generate_preg_list DEPENDS ${PATHSCALE_BINARY_DIR}/be/preg_list.h ${PATHSCALE_BINARY_DIR}/be/preg_list.cxx)

add_custom_command(
    OUTPUT   extension_include.h
    COMMAND  ${PATHSCALE_SOURCE_DIR}/scripts/common/preprocess.pl -I${PATHSCALE_SOURCE_DIR}/common/com -I${PATHSCALE_SOURCE_DIR}/common/com/${BUILD_TARGET_DIR} -I${PATHSCALE_BINARY_DIR}/../GCC/gcc -I${PATHSCALE_SOURCE_DIR}/../GCC/gcc -MD  ${PATHSCALE_SOURCE_DIR}/include/extension_include.hin -o ${PATHSCALE_BINARY_DIR}/include/extension_include.h
)
SET_SOURCE_FILES_PROPERTIES(extension_include.h PROPERTIES GENERATED 1)
ADD_CUSTOM_TARGET(generate_extension DEPENDS extension_include.h)

#add_library(be-static STATIC ${PATHSCALE_BE_BE_SRCS})
path64_set_multiarch_sources(be_sources COMMON ${PATHSCALE_BE_BE_SRCS})
path64_add_multiarch_library(be-static STATIC be_sources)
path64_set_multiarch_property(be-static OUTPUT_NAME be)
path64_multiarch_link_multiarch_libraries(be-static targinfo)
path64_multiarch_link_libraries(be-static m ${libdl})
path64_set_multiarch_property(be-static COMPILE_FLAGS
                              "-I${CMAKE_CURRENT_SOURCE_DIR}/@ARCH@ -I${CMAKE_CURRENT_SOURCE_DIR}/../com/@ARCH@  -I${CMAKE_CURRENT_SOURCE_DIR}/../cg/@ARCH@  -I${PATHSCALE_BINARY_DIR}/common/targ_info/@ARCH@ -I${CMAKE_CURRENT_SOURCE_DIR}/../../common/com/@ARCH@")
path64_add_multiarch_multiarch_dependencies(be-static generate_topcode)

path64_add_multiarch_library(be SHARED be_sources)
path64_set_multiarch_property(be PREFIX "")
path64_set_multiarch_property(be COMPILE_FLAGS
                              "-DSHARED_BUILD -I${CMAKE_CURRENT_SOURCE_DIR}/@ARCH@  -I${CMAKE_CURRENT_SOURCE_DIR}/../com/@ARCH@  -I${CMAKE_CURRENT_SOURCE_DIR}/../cg/@ARCH@ -I${PATHSCALE_BINARY_DIR}/common/targ_info/@ARCH@ -I${CMAKE_CURRENT_SOURCE_DIR}/../../common/com/@ARCH@")
path64_add_multiarch_multiarch_dependencies(be generate_topcode)

IF(NOT "${CMAKE_SYSTEM_NAME}" MATCHES "SunOS")
    # --export-dynamic is not needed on osol
    path64_set_multiarch_property(be LINK_FLAGS "-Wl,--export-dynamic")
ENDIF()

path64_multiarch_link_multiarch_libraries(be targinfo)
path64_multiarch_link_libraries(be SYS m ${libdl})
# serialize to generate preg_list.cxx only once
path64_add_multiarch_multiarch_dependencies(be be-static) 


path64_set_multiarch_sources(be_exec_sources COMMON
                              driver.cxx driver_util.cxx iter.cxx  ../com/phase.cxx)
path64_add_multiarch_executable(be-exec be_exec_sources)
# TODO : Must be the elf, elfutil and dwarf built from the source and not a system installed one
# TODO : Make sure link targets are static
path64_multiarch_link_libraries(be-exec -Wl,--whole-archive)
path64_multiarch_link_multiarch_libraries(be-exec be-static cg wopt)
path64_multiarch_link_libraries(be-exec  SYS rtkutils -Wl,--no-whole-archive iberty)
path64_multiarch_link_multiarch_libraries(be-exec dwarf)
path64_multiarch_link_libraries(be-exec elfutil elf)
path64_multiarch_link_multiarch_libraries(be-exec targinfo)
path64_set_multiarch_property(be-exec OUTPUT_NAME "be")
path64_set_multiarch_property(be-exec COMPILE_FLAGS
                              "-DSHARED_BUILD  -I${CMAKE_CURRENT_SOURCE_DIR}/@ARCH@  -I${CMAKE_CURRENT_SOURCE_DIR}/../com/@ARCH@ -I${CMAKE_CURRENT_SOURCE_DIR}/../cg/@ARCH@  -I${PATHSCALE_BINARY_DIR}/common/targ_info/@ARCH@ -I${CMAKE_CURRENT_SOURCE_DIR}/../../common/com/@ARCH@")
path64_add_multiarch_multiarch_dependencies(be-exec generate_topcode)

if(NOT "${CMAKE_SYSTEM_NAME}" MATCHES "SunOS")
    # --export-dynamic is not needed on osol
    path64_set_multiarch_property(be-exec LINK_FLAGS "-Wl,--export-dynamic")
endif()

foreach(arch ${PATH64_ENABLE_ARCHES})
    install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"be${CMAKE_EXECUTABLE_SUFFIX}\" \"${CMAKE_INSTALL_PREFIX}/lib/${PSC_FULL_VERSION}/${arch}/ipl${CMAKE_EXECUTABLE_SUFFIX}\")")
endforeach()


path64_add_dependencies_from_multiarch(be-all be-exec be)

