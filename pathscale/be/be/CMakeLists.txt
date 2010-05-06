PROJECT(BE)

SET(PATHSCALE_BE_BE_SRCS "../../common/targ_info/access/ti_init.c"
                         "../../common/targ_info/access/ti_res.c"
                         "../../common/targ_info/access/ti_res_count.c"
                         "../../common/targ_info/access/ti_res_res.c"
                         "../../common/util/c_a_to_q.c"
                         "../../common/util/c_q_rel.c"
                         "../../common/util/c_q_to_a.c"
                         "../../common/util/c_qtenscale.c"
                         "../../common/util/x8664/c_qwmultu.c"
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
                         "../../common/com/x8664/config_host.c"
                         "../../common/com/x8664/config_platform.c"
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
                         "x8664/fill_align_targ.cxx"
                         "x8664/driver_targ.cxx"
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
                         "../com/wn_unroll.cxx"
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
                         "../com/x8664/betarget.cxx"
                         "../com/x8664/sections.cxx"
                         "../../common/com/x8664/config_cache_targ.cxx"
                         "../../common/com/x8664/config_targ.cxx"
                         "../../common/com/x8664/config_elf_targ.cxx"
                         "../../common/com/x8664/targ_const.cxx"
                         "../../common/com/x8664/targ_sim.cxx"
                         "../../common/instrument/instr_reader.cxx"
                         "../../ipa/main/optimize/ipo_tlog_utils.cxx")

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


add_library(be-static STATIC ${PATHSCALE_BE_BE_SRCS})
set_property(TARGET be-static PROPERTY OUTPUT_NAME be)
target_link_libraries(be-static targinfo m ${libdl})

add_library(be SHARED ${PATHSCALE_BE_BE_SRCS})
set_property(TARGET be PROPERTY PREFIX "")
set_target_properties(be PROPERTIES COMPILE_DEFINITIONS "SHARED_BUILD")
target_link_libraries(be targinfo m ${libdl})
add_dependencies(be be-static) # serialize to generate preg_list.cxx only once


add_executable (be-exec driver.cxx driver_util.cxx iter.cxx  ../com/phase.cxx)
# TODO : Must be the elf, elfutil and dwarf built from the source and not a system installed one
# TODO : Make sure link targets are static
target_link_libraries (be-exec -Wl,-whole-archive be-static cg wopt -Wl,-no-whole-archive iberty dwarf elfutil elf targinfo)
SET_TARGET_PROPERTIES(be-exec PROPERTIES OUTPUT_NAME "be")

install(TARGETS be be-exec DESTINATION ${PATH64_LIB_PATH})

install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"be${CMAKE_EXECUTABLE_SUFFIX}\" \"${CMAKE_INSTALL_PREFIX}/lib/${PSC_FULL_VERSION}/ipl${CMAKE_EXECUTABLE_SUFFIX}\")")

