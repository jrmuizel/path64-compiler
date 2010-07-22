# TODO : This should build libwopt.a
set(PATHSCALE_WOPT_COMMON_SRCS
				"../com/cxx_template.cxx"
				"opt_alias_class.cxx"
				"opt_base.cxx"
				"opt_bb.cxx"
				"opt_bdce.cxx"
				"opt_bool.cxx"
				"opt_canon.cxx"
				"opt_cfg.cxx"
				"opt_cfg_trans.cxx"
				"opt_combine.cxx"
				"opt_cond_const.cxx"
				"opt_count.cxx"
				"opt_cse.cxx"
				"opt_daVinci.cxx"
				"opt_dbg.cxx"
				"opt_dce.cxx"
				"opt_dom.cxx"
				"opt_dse.cxx"
				"opt_du.cxx"
				"opt_eant.cxx"
				"opt_eavail.cxx"
				"opt_efinalize.cxx"
				"opt_ehoist.cxx"
				"opt_emit.cxx"
				"opt_eocc.cxx"
				"opt_etable.cxx"
				"opt_essa.cxx"
				"opt_estr.cxx"
				"opt_exc.cxx"
				"opt_expr.cxx"
				"opt_fb.cxx"
				"opt_find.cxx"
				"opt_fold.cxx"
				"opt_htable.cxx"
				"opt_htable_emit.cxx"
				"opt_ivr.cxx"
				"opt_leaf_iter.cxx"
				"opt_lftr2.cxx"
				"opt_loop.cxx"
				"opt_loop_trans.cxx"
				"opt_ltable.cxx"
				"opt_main.cxx"
				"opt_min_essa.cxx"
				"opt_mu_chi.cxx"
				"opt_spre_finalize.cxx"
				"opt_project.cxx"
				"opt_prop.cxx"
				"opt_region_emit.cxx"
				"opt_rename.cxx"
				"opt_rvi.cxx"
				"opt_rvilr.cxx"
				"opt_rvitab.cxx"
				"opt_rviwn.cxx"
				"opt_rvi_emit.cxx"
				"opt_ssa.cxx"
				"opt_ssu.cxx"
				"opt_speculate.cxx"
				"opt_stable.cxx"
				"opt_sym.cxx"
				"opt_tail.cxx"
				"opt_util.cxx"
				"opt_vn.cxx"
				"opt_vn_expr.cxx"
				"opt_vnfre.cxx"
				"opt_verify.cxx"
				"opt_whirlview.cxx"
				"opt_wn.cxx"
				"opt_alias_analysis.cxx"
				"opt_u64_lower.cxx"
				"opt_mtype_b.cxx"
				"opt_revise_ssa.cxx"
				"wodriver.cxx")

set(PATHSCALE_WOPT_ARM_SRCS "opt_tailmerge.cxx")
path64_is_arch_enabled(arm_enabled arm)
if(${arm_enabled})
set(PATHSCALE_WOPT_SRCS  ${PATHSCALE_WOPT_COMMON_SRCS} ${PATHSCALE_WOPT_ARM_SRCS})
else()
set(PATHSCALE_WOPT_SRCS  ${PATHSCALE_WOPT_COMMON_SRCS} )
endif()

ADD_DEFINITIONS(
	-DMONGOOSE_BE)

path64_set_multiarch_sources(wopt_sources COMMON ${PATHSCALE_WOPT_SRCS})
path64_add_multiarch_library(wopt STATIC wopt_sources)
path64_add_multiarch_dependencies(wopt generate_preg_list generate_topcode)
path64_set_multiarch_property(wopt COMPILE_FLAGS
                              "-I${CMAKE_CURRENT_SOURCE_DIR}/../com/@ARCH@ -I${PATHSCALE_BINARY_DIR}/common/targ_info/@ARCH@ -I${CMAKE_CURRENT_SOURCE_DIR}/../../common/com/@ARCH@")

path64_set_multiarch_sources(wopt_shared_sources COMMON
                             init.cxx
                             ../../common/util/bitset.c
                             ${PATHSCALE_WOPT_SRCS})
path64_add_multiarch_library(wopt-shared SHARED wopt_shared_sources)
path64_set_multiarch_property(wopt-shared PREFIX "")
path64_set_multiarch_property(wopt-shared OUTPUT_NAME "wopt")
path64_set_multiarch_property(wopt-shared COMPILE_DEFINITIONS "SHARED_BUILD")
path64_add_multiarch_dependencies(wopt-shared generate_preg_list generate_topcode)
path64_set_multiarch_property(wopt-shared COMPILE_FLAGS
                              "-I${CMAKE_CURRENT_SOURCE_DIR}/../com/@ARCH@ -I${PATHSCALE_BINARY_DIR}/common/targ_info/@ARCH@ -I${CMAKE_CURRENT_SOURCE_DIR}/../../common/com/@ARCH@")

# TODO : double check this is correct. wopt.so just includes be.os at the end of the list of objects
path64_multiarch_link_multiarch_libraries(wopt-shared be)


