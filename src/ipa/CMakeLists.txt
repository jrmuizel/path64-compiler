include_directories(
   common 
   local 
   main/optimize
   main/analyze
   ../be/lno 
   ../be/com 
   ../common/com 
   ../be/region 
   ../be/opt 
   ../common/com/@ARCH@
   ../common/util 
   ../fake_ld/common 
   ../fake_ld 
   ../be 
   ..
   ../include 
   ../include/libelf
   ../ipa/inline
   ${PATHSCALE_BINARY_DIR}/be
  )

path64_set_multiarch_sources(libipl_sources COMMON
	"local/loop_info.cxx"
	"local/ipl_main.cxx" 
	"local/ipl_bread_write.cxx"
	"local/ipl_summary_print.cxx"
	"local/ipl_summarize_util.cxx"
	"local/ipl_linex.cxx"
	"local/ipl_array_bread_write.cxx"
	"local/ipl_tlog.cxx"
	"local/ipl_outline.cxx"
	"local/init.cxx"
	"local/ipl_elfsym.cxx"
	"local/ipl_reorder.cxx" 
	"../be/lno/lwn_util.cxx" 
	"../be/lno/access_vector.cxx" 
	"../be/lno/if_info.cxx" 
	"../be/lno/lno_bv.cxx" 
	"../be/com/ipa_section.cxx" 
	"../be/com/ipa_section_main.cxx" 
	"../be/com/ipa_section_print.cxx" 
	"../be/com/ipl_lno_util.cxx"
	"../be/com/reshape.cxx" 
	"../be/com/wb_summary.cxx" 
	"../be/com/wb_ipl_summary.cxx" 
	"../be/com/ipa_cost_util.cxx" 
)

path64_set_multiarch_sources(libipa_sources COMMON
	"common/ipc_partition.c"
	"common/ipc_partition.c"
	"../common/com/dwarf_DST.cxx"
	"../common/com/dwarf_DST_mem.cxx"
	"../common/com/dwarf_DST_producer.cxx"
	"common/ip_graph.cxx"
	"common/ipc_dst_merge.cxx"
	"common/ipc_dst_utils.cxx"
	"common/ipc_link.cxx"
	"common/ipc_main.cxx"
	"common/ipc_bread.cxx"
	"common/ipc_bwrite.cxx"
	"common/ipc_compile_direct.cxx"
	"common/ipc_compile_run.cxx"
	"common/ipc_compile_helper.cxx"
	"common/ipc_option.cxx"
	"common/ipc_pic.cxx"
	"common/ipc_file.cxx"
	"common/ipc_symtab.cxx"
	"common/ipc_symtab_merge.cxx"
	"common/ipc_type_merge.cxx"
	"common/ipc_ty_hash.cxx"
	"common/init.cxx"
	"local/ipl_summary_print.cxx"
	"local/ipl_tlog.cxx"
	"main/analyze/ipa_option.cxx"
	"main/analyze/ip_graph_trav.cxx"
	"main/analyze/ipa_cg.cxx"
	"main/analyze/ipa_chg.cxx"
	"main/analyze/ipa_main.cxx"
	"main/analyze/ipa_cprop.cxx"
	"main/analyze/ipa_cprop_annot.cxx"
	"main/analyze/ipa_df.cxx"
	"main/analyze/ipa_pad.cxx"
	"main/analyze/ipa_feedback.cxx"
	"main/analyze/ipa_solver.cxx"
	"main/analyze/sparse_bv.cxx"
	"main/analyze/ipa_lno_write.cxx"
	"main/analyze/ipa_reshape.cxx"
	"main/analyze/cg_browser.cxx"
	"main/analyze/cg_summary.cxx"
	"main/analyze/cgb.cxx"
	"main/analyze/cgb_carray.cxx"
	"main/analyze/cgb_ipa.cxx"
	"main/analyze/wb_ipa.cxx"
	"main/analyze/ipa_section_prop.cxx"
	"main/analyze/ipa_section_annot.cxx"
	"main/analyze/ipa_inline.cxx"
	"main/analyze/cg_print.cxx"
	"main/analyze/ipaa.cxx"
	"main/analyze/ipa_preopt.cxx"
	"main/analyze/ipa_summary.cxx"
	"main/analyze/ipa_nested_pu.cxx"
	"main/analyze/ipa_cost.cxx"
	"main/analyze/ipa_reorder.cxx"
	"main/analyze/ipa_builtins.cxx"
	"main/analyze/ipa_struct_opt.cxx"
	"main/optimize/ipo_main.cxx"
	"main/optimize/ipo_const.cxx"
	"main/optimize/ipo_inline.cxx"
	"main/optimize/ipo_inline_util.cxx"
	"main/optimize/ipo_clone.cxx"
	"main/optimize/ipo_dce.cxx"
	"main/optimize/ipo_alias.cxx"
	"main/optimize/ipo_autopar.cxx"
	"main/optimize/ipo_parent.cxx"
	"main/optimize/ipo_split.cxx"
	"main/optimize/ipo_pad.cxx"
	"main/optimize/ipo_alias_class.cxx"
	"main/optimize/ipo_icall.cxx"
	"main/optimize/ipo_struct_opt.cxx"
	"../be/com/ipa_section_main.cxx"
	"../be/com/ipa_section_print.cxx"
	"../be/com/clone.cxx"
	"../be/com/clone_DST_utils.cxx"
	"../be/com/reshape.cxx"
	"../be/com/wb_summary.cxx"
	"../be/com/ipa_cost_util.cxx"
	"../be/com/be_util.cxx"
	"../be/lno/lwn_util.cxx"
)

path64_set_multiarch_sources(PATHSCALE_INLINE_SRCS COMMON
	"main/optimize/ipo_parent.cxx"
	"main/optimize/ipo_tlog_utils.cxx"
	"main/optimize/ipo_inline_util.cxx"
	"main/optimize/ipo_inline.cxx"
	"main/optimize/ipo_clone.cxx"
	"main/analyze/ip_graph_trav.cxx"
	"main/analyze/ipa_cg.cxx"
	"main/analyze/ipa_inline.cxx"
	"main/analyze/ipa_nested_pu.cxx"
	"main/analyze/ipa_summary.cxx"
	"common/ipc_bread.cxx"
	"common/ipc_file.cxx"
	"common/ip_graph.cxx"
	"common/ipc_option.cxx"
	"common/ipc_utils.cxx"
	"common/ipc_dst_utils.cxx"
	"inline/inline_driver.cxx"
	"inline/inline_stub.cxx"
	"inline/inline.cxx"
	"inline/inline_utils.cxx"
	"inline/timelib.cxx"
	"local/ipl_summarize_util.cxx"
	"../common/com/dwarf_DST.cxx"
	"../common/com/dwarf_DST_mem.cxx"
	"../common/com/dwarf_DST_producer.cxx"
	"../be/com/clone.cxx"
	"../be/com/ipl_lno_util.cxx"
	"../be/com/cxx_template.cxx"
	"../be/lno/lwn_util.cxx"
)

add_definitions(${PATH64_STDC_FORMAT_MACROS})

if(PATH64_ENABLE_IPA)

path64_add_multiarch_library(ipa SHARED libipa_sources)
path64_set_multiarch_property(ipa PREFIX "")
path64_set_multiarch_property(ipa COMPILE_DEFINITIONS MONGOOSE_BE;_SUPPORT_IPA;__MIPS_AND_IA64_ELF_H;BACK_END)
path64_set_multiarch_property(ipa COMPILE_FLAGS
                              "-I${PATHSCALE_BINARY_DIR}/common/targ_info/@ARCH@ -I${CMAKE_CURRENT_SOURCE_DIR}/../common/com/@ARCH@")
path64_multiarch_link_multiarch_libraries(ipa be ipl)
path64_add_multiarch_multiarch_dependencies(ipa generate_topcode)
path64_install_multiarch(ipa)
path64_add_dependencies_from_multiarch(compiler-stage ipa)

endif()

path64_add_multiarch_library(ipl SHARED libipl_sources)
path64_set_multiarch_property(ipl PREFIX "")
path64_set_multiarch_property(ipl COMPILE_DEFINITIONS MONGOOSE_BE;IPA_SUMMARY;__MIPS_AND_IA64_ELF_H)
path64_set_multiarch_property(ipl COMPILE_FLAGS
                              "-I${PATHSCALE_BINARY_DIR}/common/targ_info/@ARCH@ -I${CMAKE_CURRENT_SOURCE_DIR}/../common/com/@ARCH@")
path64_add_multiarch_dependencies(ipl generate_preg_list generate_topcode)
path64_install_multiarch(ipl)
path64_add_dependencies_from_multiarch(compiler-stage ipl)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    path64_multiarch_link_multiarch_libraries(ipl comutil wopt-shared be)
endif()

path64_add_multiarch_executable(inline PATHSCALE_INLINE_SRCS)
path64_set_multiarch_property(inline COMPILE_DEFINITIONS MONGOOSE_BE;_LIGHTWEIGHT_INLINER;_STDC_FORMAT_MACROS)
path64_multiarch_link_libraries(inline)
path64_multiarch_link_multiarch_libraries(inline comutil be)

# Link in system libdwarf
# XXX: this is hardcoded for /usr/local/lib/libdwarf.a now, fix this
add_library(dwarf STATIC IMPORTED)
set_property(TARGET dwarf PROPERTY IMPORTED_LOCATION /usr/local/lib/libdwarf.a)

path64_multiarch_link_libraries(inline dwarf)

path64_multiarch_link_libraries(inline elfutil elf)
path64_set_multiarch_property(inline COMPILE_FLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}/../common/com/@ARCH@")
path64_install_multiarch(inline)
path64_add_dependencies_from_multiarch(compiler-stage inline)

#path64_add_multiarch_dependencies(inline inline_hack)
#
#add_custom_command(
#    OUTPUT   ${Path64_BINARY_DIR}/lib/inline
#    COMMAND ln -sf  ../bin/inline ${Path64_BINARY_DIR}/lib/inline
#)
#
#add_custom_target(
#    inline_hack
#    DEPENDS ${Path64_BINARY_DIR}/lib/inline
#)


