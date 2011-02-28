# TODO : This should build libwopt.a
set(PATHSCALE_WOPT_SRCS
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

ADD_DEFINITIONS(
	-DMONGOOSE_BE)

add_library(wopt STATIC ${PATHSCALE_WOPT_SRCS})

ADD_DEPENDENCIES(wopt generate_preg_list)
ADD_DEPENDENCIES(wopt generate_topcode)

add_library(wopt-shared SHARED init.cxx ../../common/util/bitset.c  ${PATHSCALE_WOPT_SRCS})
set_property(TARGET wopt-shared PROPERTY PREFIX "")
SET_TARGET_PROPERTIES(wopt-shared PROPERTIES OUTPUT_NAME "wopt")
set_target_properties(wopt-shared PROPERTIES COMPILE_DEFINITIONS "SHARED_BUILD")
ADD_DEPENDENCIES(wopt-shared generate_preg_list)
ADD_DEPENDENCIES(wopt-shared generate_topcode)

# TODO : double check this is correct. wopt.so just includes be.os at the end of the list of objects
target_link_libraries(wopt-shared be iberty)

install(TARGETS wopt-shared
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${PATH64_LIB_PATH})
