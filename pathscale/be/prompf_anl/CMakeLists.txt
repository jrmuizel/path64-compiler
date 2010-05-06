ADD_LIBRARY(prompf_anl  SHARED  ../com/cxx_template.cxx
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

set_property(TARGET prompf_anl PROPERTY PREFIX "")
# TODO : double check this is correct. wopt.so just includes be.os at the end of the list of objects
target_link_libraries(prompf_anl be)

install(TARGETS prompf_anl
        LIBRARY DESTINATION ${PATH64_LIB_PATH}
        ARCHIVE DESTINATION ${PATH64_LIB_PATH})

