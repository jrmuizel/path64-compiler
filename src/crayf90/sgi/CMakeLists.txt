# This should build mfef95
# If mfef95 is not installed then build with gfortran
# If you build with gfortran, build mfef95-fake that links against
# libgfortran first and then use mfef95-fake to build mfef95 that
# links against libpathfortran
# Right now config.cxx and many other files are built twice or even four times because of strange -D defines
# So we must build confix.cxx twice.. Once without -DBACK_END for mfef95 and
# once for be with -DBACK_END

include_directories(
.
../../crayf90/fe90
../../arith
../../common/util
../../common/com
../../common/com/x8664
../../linux/mfef90_includes
../../clibinc/cray
../include
../../include/cmplrs
${PATHSCALE_BINARY_DIR}/common/targ_info/x8664

)


set(PATHSCALE_MFEF95_SRCS       "../../common/com/config.cxx"
				"../../common/com/const.cxx"
				"../../common/com/controls.cxx"
				"../../common/com/dwarf_DST.cxx"
				"../../common/com/dwarf_DST_dump.cxx"
				"../../common/com/dwarf_DST_mem.cxx"
				"../../common/com/dwarf_DST_producer.cxx"
				"../../common/com/err_host.cxx"
				"../../common/com/f90_utils.cxx"
				"../../common/com/glob.cxx"
				"../../common/com/ir_bcom.cxx"
				"../../common/com/ir_bwrite.cxx"
				"../../common/com/ir_reader.cxx"
				"../../common/com/irbdata.cxx"
				"../../common/com/mtypes.cxx"
				"../../common/com/opcode.cxx"
				"../../common/com/opcode_core.cxx"
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
				"../../common/com/wutil.cxx"
				"../../common/com/@ARCH@/config_targ.cxx"
				"../../common/com/@ARCH@/config_elf_targ.cxx"
				"../../common/com/@ARCH@/targ_const.cxx"
				"../../common/com/@ARCH@/targ_sim.cxx"
				"../../crayf90/sgi/cwh_unimp.cxx"
				"../../crayf90/sgi/cwh_addr.cxx"
				"../../crayf90/sgi/cwh_auxst.cxx"
				"../../crayf90/sgi/cwh_block.cxx"
				"../../crayf90/sgi/cwh_data.cxx"
				"../../crayf90/sgi/cwh_directive.cxx"
				"../../crayf90/sgi/cwh_dope.cxx"
				"../../crayf90/sgi/cwh_dst.cxx"
				"../../crayf90/sgi/cwh_expr.cxx"
				"../../crayf90/sgi/cwh_intrin.cxx"
				"../../crayf90/sgi/cwh_io.cxx"
				"../../crayf90/sgi/cwh_mkdepend.cxx"
				"../../crayf90/sgi/cwh_pdgcs.cxx"
				"../../crayf90/sgi/cwh_preg.cxx"
				"../../crayf90/sgi/cwh_types.cxx"
				"../../crayf90/sgi/cwh_stab.cxx"
				"../../crayf90/sgi/cwh_stmt.cxx"
				"../../crayf90/sgi/cwh_stk.cxx"
				"../../crayf90/sgi/decorate_utils.cxx"
				"../../crayf90/sgi/f2c_abi_utils.cxx"
				"../../crayf90/sgi/sgi_cmd_line.cxx"
				"../../crayf90/sgi/path_intrinsic_list.cxx"
				"../../crayf90/sgi/make_depend.cxx"
				"../../common/com/@ARCH@/config_host.c"
				"../../common/com/@ARCH@/config_platform.c")

ADD_DEFINITIONS(
-DFRONT_END
-DFRONT_F90
-D_TARGET_MONGOOSE
${PATH64_STDC_FORMAT_MACROS})
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	ADD_DEFINITIONS(-D_DEBUG)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

# Building mfef95_tmp for host architecture


set(PATHSCALE_MFEF95_TMP_SRCS       "../../libcsup/msgnew/catgetmsg.c"
				   "../../libcsup/msgnew/catmsgfmt.c"
				   "../../libF77/pow_ci.c"
				   "../../libF77/pow_cc.c"
				   "../../libF77/pow_zi.c"
				   "../../libF77/pow_zz.c"
				   "../../libF77/_powcl.c"
				   "../../libF77/_powzi.c"
				   "../../libF77/_powzl.c"
				   "../../libF77/c_div.c"
				   "../../libF77/z_div.c"
				   "../../libfi/mathlb/chton.c"
				   "../../libfi/char/f90_index.c"
				   "../../libfi/char/index.c"
				   "../../libfi/char/f90_scan.c"
				   "../../libfi/char/scan.c"
				   "../../libfi/char/f90_verify.c"
				   "../../libfi/char/verify.c"
				   "../../libcsup/gen/strnstrn.c"
				   "../../libcsup/gen/strnrstrn.c"
				   "../../libfi/array/reshape.c"
				   "../../libfi/array/transfer.c"
				   "../../libf/fio/fmtparse.c"
)


include_directories(
    "../../libcsup/include"
    "../../clibinc"
    "../../libfi/include"
    "../../libf/include"
)

path64_get_host_arch(host_arch)

path64_add_executable_for_arch(mfef95-tmp ${host_arch}
                               ${PATHSCALE_MFEF95_SRCS} ${PATHSCALE_MFEF95_TMP_SRCS})
set_source_files_properties(${PATHSCALE_MFEF95_TMP_SRCS} PROPERTIES
                            COMPILE_DEFINITIONS "_LIBU;_LITTLE_ENDIAN")

IF(PATH64_USE_FOLD_F)
    target_link_libraries(mfef95-tmp comutil-host fe90-host fe90-tmp-host arith cif ${MFEF95_TMP_FORTRAN_LINK_LIB} m)
ELSE()
    target_link_libraries(mfef95-tmp comutil-host fe90-host arith cif m)
ENDIF()

path64_set_property_for_arch(mfef95-tmp ${host_arch} COMPILE_FLAGS
                             "-I${CMAKE_CURRENT_SOURCE_DIR}/../../common/com/@ARCH@")


add_custom_command(
    OUTPUT ${Path64_BINARY_DIR}/lib/${host_arch}/mfef95
    COMMAND ${CMAKE_COMMAND} -E create_symlink mfef95-tmp ${Path64_BINARY_DIR}/lib/${host_arch}/mfef95
    DEPENDS ${Path64_BINARY_DIR}/lib/${host_arch}/mfef95-tmp
    )

add_custom_command(
    OUTPUT ${Path64_BINARY_DIR}/bin/pathf90
    COMMAND ${CMAKE_COMMAND} -E create_symlink pathcc ${Path64_BINARY_DIR}/bin/pathf90
    COMMAND ${CMAKE_COMMAND} -E create_symlink pathcc ${Path64_BINARY_DIR}/bin/pathf95
    DEPENDS ${Path64_BINARY_DIR}/bin/pathcc
)


#add_custom_command(
#    OUTPUT ${Path64_BINARY_DIR}/lib/${PSC_FULL_VERSION}
#           ${Path64_BINARY_DIR}/lib/${host_arch}/mfef95
#           ${Path64_BINARY_DIR}/lib/be ${Path64_BINARY_DIR}/bin/pathf90
#    COMMAND ln -s . ${Path64_BINARY_DIR}/lib/${PSC_FULL_VERSION}
#    COMMAND ln -s mfef95-tmp ${Path64_BINARY_DIR}/lib/${host_arch}/mfef95
#    COMMAND ln -s pathcc ${Path64_BINARY_DIR}/bin/pathf90
#)



path64_set_multiarch_sources(mfef95_sources COMMON ${PATHSCALE_MFEF95_SRCS})
path64_add_multiarch_executable(mfef95 mfef95_sources)
IF(PATH64_USE_FOLD_F)
    path64_multiarch_link_multiarch_libraries(mfef95 comutil fe90 fe90-pathf)
ELSE()
    path64_multiarch_link_multiarch_libraries(mfef95 comutil fe90)
ENDIF()
path64_multiarch_link_libraries(mfef95 arith cif pathfortran-host m)
path64_set_multiarch_property(mfef95 COMPILE_FLAGS
                              "-I${CMAKE_CURRENT_SOURCE_DIR}/../../common/com/@ARCH@")
path64_install_multiarch(mfef95)
path64_add_dependencies_from_multiarch(compiler-stage-Fortran mfef95)

#add_custom_command(
#    OUTPUT ${Path64_BINARY_DIR}/lib/mfef95-stage
#    COMMAND rm -f ${Path64_BINARY_DIR}/lib/mfef95
#    COMMAND ln -fs ../bin/mfef95 ${Path64_BINARY_DIR}/lib/mfef95
#    COMMAND touch ${Path64_BINARY_DIR}/lib/mfef95-stage
#)

#add_custom_target(
#    mfef95-stage ALL
#    DEPENDS ${Path64_BINARY_DIR}/bin/mfef95
#    DEPENDS ${Path64_BINARY_DIR}/lib/mfef95-stage
#)

#install(TARGETS mfef95
#        RUNTIME DESTINATION ${PATH64_LIB_PATH})

# TODO : Fix the intrinsic_module_path since I know this isn't correct
add_custom_target(
    switch_fc
    DEPENDS ${Path64_BINARY_DIR}/lib/${host_arch}/mfef95
            ${Path64_BINARY_DIR}/bin/pathf90
    VERBATIM
    COMMAND printf "#!/bin/sh\\n${Path64_BINARY_DIR}/bin/pathf90 -Wf,-eQ -Wf,-intrinsic_module_gen -Wf,-intrinsic_module_path=. \"$\@\"\\n" > ${Path64_BINARY_DIR}/fc-wrap.sh
)

add_dependencies(switch_fc generate_cf95_cat mfef95-tmp compiler-stage)
path64_add_dependencies_from_multiarch(switch_fc be-all inline crtendS)

