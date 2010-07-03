SET(PATHSCALE_FE90_C_SRCS "cifcmnrecs.c"
			"cifftnrecs.c"
			"cmd_line.c"
			"cond_comp.c"
			"debug.c"
			"fecif.c"
			"fold_drive.c"
			"fortout.c"
			"i_cvrt.c"
			"inline.c"
			"ir.c"
			"lex.c"
			"main.c"
			"messages.c"
			"module.c"
			"nameres.c"
			"p_asg_expr.c"
			"p_ctl_flow.c"
			"p_dcl_attr.c"
			"p_dcl_pu.c"
			"p_dcl_util.c"
			"p_dcls.c"
			"p_directiv.c"
			"p_driver.c"
			"p_end.c"
			"p_io.c"
			"p_utils.c"
			"release.c"
			"s_asg_expr.c"
			"s_call.c"
			"s_cnstrct.c"
			"s_ctl_flow.c"
			"s_data.c"
			"s_dcl_pu.c"
			"s_dcls.c"
			"s_directiv.c"
			"s_driver.c"
			"s_end.c"
			"s_interoperable.c"
			"s_intrin.c"
			"s_io.c"
			"s_rcnstrct.c"
			"s_typ_init.c"
			"s_utils.c"
			"src_input.c"
			"sytb.c"
			"utils.c")

set(PATHSCALE_FE90_F_SRCS "fold.f")

INCLUDE_DIRECTORIES(
	"../../crayf90/fe90"
	"../../arith"
	"../../common/com"
	"../../common/com/x8664"
	"../../linux/mfef90_includes"
	"../../clibinc/cray"
)

add_custom_command(
    OUTPUT cf95.cat
    COMMAND ${PATHSCALE_SOURCE_DIR}/linux/tools/caterr -c cf95.cat ${CMAKE_CURRENT_SOURCE_DIR}/cf95.msgs
    COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/cf95.cat ${Path64_BINARY_DIR}/lib
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cf95.msgs
)
set_source_files_properties(cf95.cat PROPERTIES GENERATED 1)
file(COPY cf95.msgs DESTINATION ${Path64_BINARY_DIR}/lib)

add_custom_target(
    generate_cf95_cat
    DEPENDS cf95.cat
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cf95.cat DESTINATION ${PATH64_LIB_PATH})
install(FILES cf95.msgs DESTINATION ${PATH64_LIB_PATH})
install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"cf95.msgs\" \"${CMAKE_INSTALL_PREFIX}/lib/${PSC_FULL_VERSION}/pathf95.msgs\")")

add_library(fe90-pathf STATIC ${PATHSCALE_FE90_F_SRCS})
add_dependencies(fe90-pathf switch_fc)

add_library(fe90-tmp STATIC ${PATHSCALE_FE90_F_SRCS})

add_library(fe90 STATIC ${PATHSCALE_FE90_C_SRCS})
set_target_properties(fe90 PROPERTIES COMPILE_FLAGS "-std=gnu99")

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	ADD_DEFINITIONS(-D_DEBUG)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
