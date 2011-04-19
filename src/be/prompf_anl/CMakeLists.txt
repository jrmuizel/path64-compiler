path64_set_multiarch_sources(prompf_anl_sources COMMON
                ../com/cxx_template.cxx
				../lno/lwn_util.cxx
				anl_diagnostics.cxx
				anl_cbuf.cxx	
				anl_file_mngr.cxx
				anl_srcpos.cxx
				w2cf_translator.cxx
				anl_varlist.cxx
				anl_pragma_attribute.cxx
				anl_loop_construct.cxx
				anl_region_construct.cxx
				anl_pragma_construct.cxx
				anl_func_entry.cxx
				anl_driver.cxx
				anl_lno_dump.cxx)

ADD_DEFINITIONS(
	-DMONGOOSE_BE
	-DBUILD_PROMPF_ANL_SO
	-D__STDC_LIMIT_MACROS
	-UBACK_END)

path64_add_multiarch_library(prompf_anl SHARED prompf_anl_sources)
path64_set_multiarch_property(prompf_anl PREFIX "")
path64_set_multiarch_property(prompf_anl COMPILE_FLAGS
                              "-I${CMAKE_CURRENT_SOURCE_DIR}/../../common/com/@ARCH@")
path64_install_multiarch(prompf_anl)
path64_multiarch_link_multiarch_libraries(prompf_anl comutil)

# TODO : double check this is correct. wopt.so just includes be.os at the end of the list of objects
path64_multiarch_link_multiarch_libraries(prompf_anl be)

