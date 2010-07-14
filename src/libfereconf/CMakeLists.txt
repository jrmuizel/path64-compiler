# build libfereconf.a and a couple custom commands

SET(PATHSCALE_FERECONF_COMMON_SRCS     "gccfe_loader.cxx"
                                "fe_loader.cxx"
                                "gccfe_errors.c"
  "../common/com/config.cxx"
  "../common/com/const.cxx"
  "../common/com/controls.cxx"
  "../common/com/dwarf_DST.cxx"		
  "../common/com/dwarf_DST_dump.cxx"	
  "../common/com/dwarf_DST_mem.cxx"	
  "../common/com/dwarf_DST_producer.cxx" 
  "../common/com/err_host.cxx"
  "../common/com/glob.cxx"		
  "../common/com/intrn_info.cxx"	
  "../common/com/ir_bcom.cxx"	
  "../common/com/ir_bwrite.cxx"		
  "../common/com/ir_reader.cxx"		
  "../common/com/irbdata.cxx"		
  "../common/com/mtypes.cxx"		
  "../common/com/opcode.cxx"		
  "../common/com/opcode_core.cxx"
  "../common/com/pu_info.cxx"		
  "../common/com/strtab.cxx"		
  "../common/com/symtab.cxx"		
  "../common/com/symtab_verify.cxx"	
  "../common/com/ttype.cxx"		
  "../common/com/wn.cxx"	
  "../common/com/wn_map.cxx"	
  "../common/com/wn_pragmas.cxx"	
  "../common/com/wn_simp.cxx"	
  "../common/com/wn_util.cxx"
  "../common/com/wutil.cxx"		
  "../common/com/xstats.cxx"
  "../common/com/gccfe_targinfo_interface.cxx"
  "../common/com/${BUILD_TARGET_DIR}/config_host.c"
  "../common/com/${BUILD_TARGET_DIR}/config_platform.c"
  "../common/com/${BUILD_TARGET_DIR}/config_targ.cxx"
  "../common/com/${BUILD_TARGET_DIR}/config_elf_targ.cxx"
  "../common/com/${BUILD_TARGET_DIR}/targ_const.cxx"
  "../common/com/${BUILD_TARGET_DIR}/targ_sim.cxx"
)
SET(PATHSCALE_FERECONF_ARM_SRCS "../common/com/${BUILD_TARGET_DIR}/targ_register_common.cxx")

IF (${BUILD_TARGET_DIR} MATCHES "arm")
SET(PATHSCALE_FERECONF_SRCS ${PATHSCALE_FERECONF_COMMON_SRCS} ${PATHSCALE_FERECONF_ARM_SRCS})
ELSE()
SET(PATHSCALE_FERECONF_SRCS ${PATHSCALE_FERECONF_COMMON_SRCS})
ENDIF()

ADD_DEFINITIONS(
 -DIN_GCC -DHAVE_CONFIG_H
 -DLONGLONG
 -DFRONT_END
 -DFE_GNU_4_2_0
 -DUSE_DECL_SRCPOS
 -D_NEW_SYMTAB
 -DFRONT_END_C
 -DCFE -DCIL
 -DDO_IL_LOWERING=0
 -DNO_USR_INCLUDE=TRUE
 -DAUTOMATIC_TEMPLATE_INSTANTIATION=0
 -DINSTANTIATION_BY_IMPLICIT_INCLUSION=0
 -DBACK_END_IS_C_GEN_BE=0
-DHANDLE_WFE_PRAGMAS #ST
-DSGI_MONGOOSE
-DDEBUG
)
INCLUDE_DIRECTORIES(
	.
	../common/com
        ../common/util
        ../common/com/${BUILD_TARGET_DIR}
        ${PATHSCALE_BINARY_DIR}/common/targ_info
)


add_library(fereconf STATIC ${PATHSCALE_FERECONF_SRCS})

