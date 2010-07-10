# TODO : These should not have a hard coded ${BUILD_TARGET_DIR} and should be per target
SET(PATHSCALE_CG_SRCS
			"${PATHSCALE_BINARY_DIR}/be/bb_list.cxx"
			"${PATHSCALE_BINARY_DIR}/be/gra_bb_list.cxx"
			"${PATHSCALE_BINARY_DIR}/be/lrange_list.cxx"
			"${PATHSCALE_BINARY_DIR}/be/op_list.cxx"
			"${PATHSCALE_BINARY_DIR}/be/st_list.cxx"
			"${PATHSCALE_BINARY_DIR}/be/tn_list.cxx"
			"${PATHSCALE_BINARY_DIR}/be/void_list.cxx"
			"${PATHSCALE_BINARY_DIR}/be/cg_loop_scc_prop.cxx"
			"${PATHSCALE_BINARY_DIR}/be/tn_prop.cxx"
			"${PATHSCALE_BINARY_DIR}/be/bb_set.cxx"
			"${PATHSCALE_BINARY_DIR}/be/lrange_set.cxx"
			"${PATHSCALE_BINARY_DIR}/be/tn_set.cxx"
			"${PATHSCALE_BINARY_DIR}/be/gtn_set.cxx"
			"../../be/cg/swp/cg_swp_allocator.cxx"
			"../../be/cg/swp/cg_swp_code_gen.cxx"
			"../../be/cg/swp/cg_swp_driver.cxx"
			"../../be/cg/swp/cg_swp_heuristic.cxx"
			"../../be/cg/swp/cg_swp_lifeness.cxx"
			"../../be/cg/swp/cg_swp_lifetime.cxx"
			"../../be/cg/swp/cg_swp_loop.cxx"
			"../../be/cg/swp/cg_swp_mindist.cxx"
			"../../be/cg/swp/cg_swp_minlt.cxx"
			"../../be/cg/swp/cg_swp_mrt.cxx"
			"../../be/cg/swp/cg_swp_mve.cxx"
			"../../be/cg/swp/cg_swp_note.cxx"
			"../../be/cg/swp/cg_swp_options.cxx"
			"../../be/cg/swp/cg_swp_op_vector.cxx"
			"../../be/cg/swp/cg_swp_scheduler.cxx"
			"../../be/cg/swp/cg_swp_slack.cxx"
			"../../be/cg/${BUILD_TARGET_DIR}/cg_flags_arch.cxx"
			"../../be/cg/${BUILD_TARGET_DIR}/cg_swp_target.cxx"
			"../../be/cg/${BUILD_TARGET_DIR}/cgdwarf_targ.cxx"
			"../../be/cg/${BUILD_TARGET_DIR}/cgemit_targ.cxx"
		#	"../../be/cg/${BUILD_TARGET_DIR}/cgtarget.cxx"
			"../../be/cg/${BUILD_TARGET_DIR}/cg_loop_recur.cxx"
		#	"../../be/cg/${BUILD_TARGET_DIR}/ebo_special.cxx"
		#	"../../be/cg/${BUILD_TARGET_DIR}/entry_exit_targ.cxx"
		#	"../../be/cg/${BUILD_TARGET_DIR}/exp_branch.cxx"
		#	"../../be/cg/${BUILD_TARGET_DIR}/exp_divrem.cxx"
		#	"../../be/cg/${BUILD_TARGET_DIR}/exp_loadstore.cxx"
		#	"../../be/cg/${BUILD_TARGET_DIR}/exp_predicate.cxx"
		#	"../../be/cg/${BUILD_TARGET_DIR}/expand.cxx"
			"../../be/cg/${BUILD_TARGET_DIR}/pqs_cg.cxx"
			"../../be/cg/${BUILD_TARGET_DIR}/pqs.cxx"
			"../../be/cg/${BUILD_TARGET_DIR}/register_targ.cxx"
		#	"../../be/cg/${BUILD_TARGET_DIR}/cg_sched.cxx"
	#		"../../be/cg/${BUILD_TARGET_DIR}/cg_convert_x87.cxx"
		#	"../../be/cg/${BUILD_TARGET_DIR}/cg_misc.cxx"
			"../../common/com/em_dwarf.cxx"
			"../../common/com/em_elf.cxx"
			"../../common/com/${BUILD_TARGET_DIR}/targ_em_const.cxx"
			"../../common/com/${BUILD_TARGET_DIR}/targ_em_elf.cxx"
			"../../common/com/${BUILD_TARGET_DIR}/targ_em_dwarf.cxx"
			"../../be/cg/annotations.cxx"
			"../../be/cg/bblist.cxx"
			"../../be/cg/bb_map.cxx"
			"../../be/cg/bbutil.cxx"
			"../../be/cg/calls.cxx"
			"../../be/cg/cflow.cxx"
			"../../be/cg/cg.cxx"
			"../../be/cg/cg_db_op.cxx"
			"../../be/cg/cgdriver.cxx"
			"../../be/cg/cgdwarf.cxx"
			"../../be/cg/cgemit.cxx"
			"../../be/cg/cgexp.cxx"
			"../../be/cg/cg_flags.cxx"
		#	"../../be/cg/cg_gcov.cxx"
			"../../be/cg/cg_loop.cxx"
			"../../be/cg/cg_loop_mii.cxx"
			"../../be/cg/cg_loop_scc.cxx"
			"../../be/cg/cgprep.cxx"
			"../../be/cg/cg_region.cxx"
			"../../be/cg/cg_sched_est.cxx"
			"../../be/cg/cg_spill.cxx"
			"../../be/cg/cg_thr.cxx"
			"../../be/cg/cg_vector.cxx"
			"../../be/cg/cio_rwtran.cxx"
			"../../be/cg/comment.cxx"
			"../../be/cg/dominate.cxx"
			"../../be/cg/ebo.cxx"
			"../../be/cg/findloops.cxx"
			"../../be/cg/freq.cxx"
			"../../be/cg/gcm.cxx"
			"../../be/cg/hb_block_select.cxx"
			"../../be/cg/hb_cflow.cxx"
			"../../be/cg/hb.cxx"
			"../../be/cg/hb_hazards.cxx"
			"../../be/cg/hb_id_candidates.cxx"
			"../../be/cg/hb_if_convert.cxx"
			"../../be/cg/hb_sched.cxx"
			"../../be/cg/hb_tail_duplication.cxx"
			"../../be/cg/hb_trace.cxx"
		#	"../../be/cg/iface_scn.cxx"
			"../../be/cg/igls.cxx"
			"../../be/cg/init.cxx"
			"../../be/cg/label_util.cxx"
			"../../be/cg/localize.cxx"
			"../../be/cg/note.cxx"
			"../../be/cg/register.cxx"
			"../../be/cg/reg_live.cxx"
			"../../be/cg/tag.cxx"
			"../../be/cg/variants.cxx"
			"../../be/cg/whirl2ops.cxx"
			"../../be/cg/cg_cflow.cxx"
			"../../be/cg/cg_dep_graph.cxx"
			"../../be/cg/eh_region.cxx"
			"../../be/cg/gra_live.cxx"
			"../../be/cg/gtn_tn_set.cxx"
			"../../be/cg/gtn_universe.cxx"
			"../../be/cg/lra.cxx"
			"../../be/cg/op_map.cxx"
			"../../be/cg/oputil.cxx"
			"../../be/cg/tn_map.cxx"
			"../../be/cg/tnutil.cxx"
			"../../be/cg/gra_mon/gra.cxx"
			"../../be/cg/gra_mon/gra_bb.cxx"
			"../../be/cg/gra_mon/gra_cflow.cxx"
			"../../be/cg/gra_mon/gra_color.cxx"
			"../../be/cg/gra_mon/gra_create.cxx"
			"../../be/cg/gra_mon/gra_grant.cxx"
			"../../be/cg/gra_mon/gra_interfere.cxx"
			"../../be/cg/gra_mon/gra_loop.cxx"
			"../../be/cg/gra_mon/gra_lrange.cxx"
			"../../be/cg/gra_mon/gra_lrange_subuniverse.cxx"
			"../../be/cg/gra_mon/gra_lrange_vset.cxx"
			"../../be/cg/gra_mon/gra_lunit.cxx"
			"../../be/cg/gra_mon/gra_pref.cxx"
			"../../be/cg/gra_mon/gra_region.cxx"
			"../../be/cg/gra_mon/gra_spill.cxx"
			"../../be/cg/gra_mon/gra_split.cxx"
			"../../be/cg/gra_mon/gra_trace.cxx")


add_custom_command(
	OUTPUT ${PATHSCALE_BINARY_DIR}/be/bb_list.cxx ${PATHSCALE_BINARY_DIR}/be/bb_list.h
	COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/cg/gen_bb_list.sh ${PATHSCALE_SOURCE_DIR}/common/util
	DEPENDS ${PATHSCALE_SOURCE_DIR}/be/cg/gen_bb_list.sh
	WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
)
add_custom_command(
	OUTPUT ${PATHSCALE_BINARY_DIR}/be/gra_bb_list.cxx ${PATHSCALE_BINARY_DIR}/be/gra_bb_list.h
	COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/cg/gra_mon/gen_gra_bb_list.sh ${PATHSCALE_SOURCE_DIR}/common/util
	DEPENDS ${PATHSCALE_SOURCE_DIR}/be/cg/gra_mon/gen_gra_bb_list.sh
	WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
)
add_custom_command(
	OUTPUT ${PATHSCALE_BINARY_DIR}/be/lrange_list.cxx ${PATHSCALE_BINARY_DIR}/be/lrange_list.h
	COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/cg/gra_mon/gen_lrange_list.sh ${PATHSCALE_SOURCE_DIR}/common/util
	DEPENDS ${PATHSCALE_SOURCE_DIR}/be/cg/gra_mon/gen_lrange_list.sh
	WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
)
add_custom_command(
	OUTPUT ${PATHSCALE_BINARY_DIR}/be/op_list.cxx ${PATHSCALE_BINARY_DIR}/be/op_list.h
	COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/cg/gen_op_list.sh ${PATHSCALE_SOURCE_DIR}/common/util
	DEPENDS ${PATHSCALE_SOURCE_DIR}/be/cg/gen_op_list.sh
	WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
)
add_custom_command(
	OUTPUT ${PATHSCALE_BINARY_DIR}/be/st_list.cxx ${PATHSCALE_BINARY_DIR}/be/st_list.h
	COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/cg/gen_st_list.sh ${PATHSCALE_SOURCE_DIR}/common/util
	DEPENDS ${PATHSCALE_SOURCE_DIR}/be/cg/gen_st_list.sh
	WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
)
add_custom_command(
	OUTPUT ${PATHSCALE_BINARY_DIR}/be/tn_list.cxx ${PATHSCALE_BINARY_DIR}/be/tn_list.h
	COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/cg/gen_tn_list.sh ${PATHSCALE_SOURCE_DIR}/common/util
	DEPENDS ${PATHSCALE_SOURCE_DIR}/be/cg/gen_tn_list.sh
	WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
)
add_custom_command(
	OUTPUT ${PATHSCALE_BINARY_DIR}/be/void_list.cxx ${PATHSCALE_BINARY_DIR}/be/void_list.h
	COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/cg/gen_void_list.sh ${PATHSCALE_SOURCE_DIR}/common/util
	DEPENDS ${PATHSCALE_SOURCE_DIR}/be/cg/gen_void_list.sh
	WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
)
add_custom_command(
	OUTPUT ${PATHSCALE_BINARY_DIR}/be/cg_loop_scc_prop.cxx ${PATHSCALE_BINARY_DIR}/be/cg_loop_scc_prop.h
	COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/cg/gen_cg_loop_scc_prop.sh ${PATHSCALE_SOURCE_DIR}/common/util
	DEPENDS ${PATHSCALE_SOURCE_DIR}/be/cg/gen_cg_loop_scc_prop.sh
	WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
)
add_custom_command(
	OUTPUT ${PATHSCALE_BINARY_DIR}/be/tn_prop.cxx ${PATHSCALE_BINARY_DIR}/be/tn_prop.h
	COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/cg/gen_tn_prop.sh ${PATHSCALE_SOURCE_DIR}/common/util
	DEPENDS ${PATHSCALE_SOURCE_DIR}/be/cg/gen_tn_prop.sh
	WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
)
add_custom_command(
	OUTPUT ${PATHSCALE_BINARY_DIR}/be/bb_set.cxx ${PATHSCALE_BINARY_DIR}/be/bb_set.h
	COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/cg/gen_bb_set ${PATHSCALE_SOURCE_DIR}/common/util
	DEPENDS ${PATHSCALE_SOURCE_DIR}/be/cg/gen_bb_set
	WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
)
add_custom_command(
	OUTPUT ${PATHSCALE_BINARY_DIR}/be/opsch_set.h
	COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/cg/gen_opsch_set ${PATHSCALE_SOURCE_DIR}/common/util
	WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
	DEPENDS ${PATHSCALE_SOURCE_DIR}/be/cg/gen_opsch_set
)
add_custom_command(
	OUTPUT ${PATHSCALE_BINARY_DIR}/be/lrange_set.cxx ${PATHSCALE_BINARY_DIR}/be/lrange_set.h
	COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/cg/gra_mon/gen_lrange_set ${PATHSCALE_SOURCE_DIR}/common/util
	DEPENDS ${PATHSCALE_SOURCE_DIR}/be/cg/gra_mon/gen_lrange_set
	WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
)
add_custom_command(
	OUTPUT ${PATHSCALE_BINARY_DIR}/be/tn_set.cxx ${PATHSCALE_BINARY_DIR}/be/tn_set.h
	COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/cg/gen_tn_set ${PATHSCALE_SOURCE_DIR}/common/util
	DEPENDS ${PATHSCALE_SOURCE_DIR}/be/cg/gen_tn_set
	WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
)
add_custom_command(
	OUTPUT ${PATHSCALE_BINARY_DIR}/be/gtn_set.cxx ${PATHSCALE_BINARY_DIR}/be/gtn_set.h
	COMMAND /bin/sh ${PATHSCALE_SOURCE_DIR}/be/cg/gen_gtn_set ${PATHSCALE_SOURCE_DIR}/common/util
	DEPENDS ${PATHSCALE_SOURCE_DIR}/be/cg/gen_gtn_set
	WORKING_DIRECTORY ${PATHSCALE_BINARY_DIR}/be
)

add_library(cg STATIC ${PATHSCALE_CG_SRCS} ${PATHSCALE_BINARY_DIR}/be/opsch_set.h)
add_dependencies(cg targinfo)

ADD_DEPENDENCIES(cg generate_preg_list)
ADD_DEPENDENCIES(cg generate_topcode)
