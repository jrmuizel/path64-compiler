# We want relocatable objects even though libtarginfo.a is static.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Hide generator binaries away from the rest of the compiler.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

set(targinfo_deps "")

macro(gen_info type name)
  add_executable(${name}_gen generate/${name}_gen.cxx ${type}/${BUILD_TARGET_DIR}/${name}.cxx)
  target_link_libraries(${name}_gen ${ARGN})

  # Generate each source as its own library so it can be linked into
  # other generator executables with high granularity.
  add_custom_command(
    OUTPUT targ_${name}.c targ_${name}.h
    COMMAND ${name}_gen
    DEPENDS ${name}_gen
    )
  add_library(targ_${name} targ_${name}.c targ_${name}.h)

  # The final targinfo library depends on all individual sources.
  list(APPEND targinfo_deps targ_${name})
endmacro()

#-----------------------------------------------------------------------------
include_directories(
  access
  generate
  ${PATHSCALE_SOURCE_DIR}/common/com
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PATHSCALE_SOURCE_DIR}/common/targ_info/isa/${BUILD_TARGET_DIR}
  ${PATHSCALE_SOURCE_DIR}/libair/
  ${PATHSCALE_SOURCE_DIR}/common/targ_info/parser/inc
  )

add_library(gen_util STATIC generate/gen_util.cxx)

add_executable(isa_gen generate/isa_gen.cxx isa/${BUILD_TARGET_DIR}/isa.cxx)
target_link_libraries(isa_gen gen_util)
add_custom_command(
  OUTPUT topcode.c topcode.h
  COMMAND isa_gen
  DEPENDS isa_gen
  )
add_library(isa_gen_topcode STATIC topcode.c topcode.h)
target_link_libraries(isa_gen_topcode gen_util)

SET_SOURCE_FILES_PROPERTIES(${PATHSCALE_BINARY_DIR}/common/targ_info/topcode.h PROPERTIES GENERATED 1)
SET_SOURCE_FILES_PROPERTIES(${PATHSCALE_BINARY_DIR}/common/targ_info/topcode.c PROPERTIES GENERATED 1)

ADD_CUSTOM_TARGET(generate_topcode DEPENDS ${PATHSCALE_BINARY_DIR}/common/targ_info/topcode.h ${PATHSCALE_BINARY_DIR}/common/targ_info/topcode.c)
IF (${BUILD_TARGET_DIR} MATCHES "arm")
#debater
add_executable(isa_syntax_gen generate/isa_syntax_gen.cxx isa/arm/isa_syntax.cxx)
  target_link_libraries(isa_syntax_gen targ_isa_relocs targ_isa_lits targ_isa_registers targ_isa_enums targ_isa_operands isa_gen_topcode)

  # Generate each source as its own library so it can be linked into
  # other generator executables with high granularity.
  add_custom_command(
    OUTPUT targ_isa_print.c targ_isa_print.h targ_isa_parse.c targ_isa_parse.h
    COMMAND isa_syntax_gen
    DEPENDS isa_syntax_gen
    )
  add_library(targ_isa_print targ_isa_print.c targ_isa_print.h)

  # The final targinfo library depends on all individual sources.
  list(APPEND targinfo_deps targ_isa_print targ_isa_parse)
#end debater
ELSE()
gen_info(isa isa_print        targ_isa_operands targ_isa_properties isa_gen_topcode)
ENDIF()
gen_info(isa isa_enums        gen_util)
gen_info(isa isa_lits         isa_gen_topcode)

gen_info(isa isa_properties   isa_gen_topcode)
gen_info(isa isa_pack         targ_isa_properties isa_gen_topcode)
gen_info(isa isa_bundle       targ_isa_subset targ_isa_pack targ_proc targ_isa_properties isa_gen_topcode)
gen_info(isa isa_decode       targ_isa_subset targ_isa_bundle targ_isa_pack isa_gen_topcode)

gen_info(isa isa_subset       isa_gen_topcode)
gen_info(isa isa_hazards      targ_isa_subset isa_gen_topcode)
gen_info(isa isa_registers    targ_isa_subset isa_gen_topcode)
target_link_libraries(targ_isa_registers targ_isa_subset)
IF (${BUILD_TARGET_DIR} MATCHES "arm")
gen_info(isa isa_variants      isa_gen_topcode)
gen_info(isa isa_binutils      targ_isa_subset targ_isa_pack isa_gen_topcode)
gen_info(isa isa_relocs       targ_isa_subset targ_isa_lits isa_gen_topcode)
gen_info(isa isa_operands     targ_isa_relocs targ_isa_properties targ_isa_registers targ_isa_enums targ_isa_lits isa_gen_topcode)
ELSE()
gen_info(isa isa_operands      targ_isa_properties targ_isa_registers targ_isa_enums targ_isa_lits isa_gen_topcode)

ENDIF()
gen_info(isa isa_pseudo       targ_isa_operands targ_isa_registers targ_isa_enums isa_gen_topcode)

gen_info(proc proc            gen_util)
gen_info(proc proc_properties targ_proc gen_util)
gen_info(abi  abi_properties  targ_isa_subset targ_isa_registers gen_util)
#debater
add_custom_command(
  OUTPUT armv5e_subset_targ_isa_parse.c
  COMMAND gensyn armv5e_subset_targ_isa_parse.pr -t extension -ac armv5e_subset_targ_isa_parse.c
  DEPENDS gensyn
  )
add_custom_command(
  OUTPUT armv6_subset_targ_isa_parse.c
  COMMAND gensyn armv6_subset_targ_isa_parse.pr -t extension -ac armv6_subset_targ_isa_parse.c
  DEPENDS gensyn
  )

#debater
set(TARGINFO_COMMON_SRC access/ti_asm.c
	access/ti_bundle.c
	access/ti_latency.c
	access/ti_errors.c
	topcode.c
	targ_isa_subset.c
	targ_isa_properties.c
	targ_isa_enums.c
	targ_isa_hazards.c
	targ_isa_lits.c
	targ_isa_print.c
	targ_isa_pack.c
	targ_isa_operands.c
	targ_isa_registers.c
	targ_isa_decode.c
	targ_isa_pseudo.c
	targ_abi_properties.c
	targ_isa_bundle.c
	targ_proc.c
	targ_proc_properties.c )

set(TARGINFO_ARM_SRC targ_isa_relocs.c
        targ_isa_variants.c
        targ_isa_binutils.c
        targ_isa_parse.c
        armv5e_subset_targ_isa_parse.c 
        armv6_subset_targ_isa_parse.c )

IF (${BUILD_TARGET_DIR} MATCHES "arm")
set(TARGINFO_SRC ${TARGINFO_COMMON_SRC} ${TARGINFO_ARM_SRC})
ELSE()
set(TARGINFO_SRC ${TARGINFO_COMMON_SRC})
ENDIF()
# Collect the rest of the sources in a 'targinfo' library.
add_library(targinfo STATIC ${TARGINFO_SRC})
add_dependencies(targinfo ${targinfo_deps})


#-----------------------------------------------------------------------------
add_library(si_gen generate/si_gen.cxx)
target_link_libraries(si_gen
  targ_isa_subset
  targ_isa_properties
  targ_isa_operands
  targ_isa_registers
  targ_isa_enums
  targ_isa_lits
  )

# TODO : We should not assume things are in x8664
# The generators be per target so it could be arm/mips/nvisa/x8664.. etc
IF (${BUILD_TARGET_DIR} MATCHES "arm")

add_executable(armv5e_si_gen proc/arm/armv5e_si.cxx)
target_link_libraries(armv5e_si_gen si_gen isa_gen_topcode)

add_custom_command(
  OUTPUT armv5e.c
  COMMAND armv5e_si_gen
  DEPENDS armv5e_si_gen
  )
add_library(armv5e SHARED armv5e.c generate/init.cxx)
set_property(TARGET armv5e PROPERTY PREFIX "")

add_executable(armv6_si_gen proc/arm/armv6_si.cxx)
target_link_libraries(armv6_si_gen si_gen isa_gen_topcode)

add_custom_command(
  OUTPUT armv6.c
  COMMAND armv6_si_gen
  DEPENDS armv6_si_gen
  )
add_library(armv6 SHARED armv6.c generate/init.cxx)
set_property(TARGET armv6 PROPERTY PREFIX "")
ELSE()
add_executable(opteron_si_gen proc/x8664/opteron_si.cxx)
target_link_libraries(opteron_si_gen si_gen isa_gen_topcode)

add_custom_command(
  OUTPUT opteron.c
  COMMAND opteron_si_gen
  DEPENDS opteron_si_gen
  )
add_library(opteron SHARED opteron.c generate/init.cxx)
set_property(TARGET opteron PROPERTY PREFIX "")

add_executable(barcelona_si_gen proc/x8664/barcelona_si.cxx)
target_link_libraries(barcelona_si_gen si_gen isa_gen_topcode)

add_custom_command(
  OUTPUT barcelona.c
  COMMAND barcelona_si_gen
  DEPENDS barcelona_si_gen
  )
add_library(barcelona SHARED barcelona.c generate/init.cxx)
set_property(TARGET barcelona PROPERTY PREFIX "")


add_executable(wolfdale_si_gen proc/x8664/wolfdale_si.cxx)
target_link_libraries(wolfdale_si_gen si_gen isa_gen_topcode)

add_custom_command(
  OUTPUT wolfdale.c
  COMMAND wolfdale_si_gen
  DEPENDS wolfdale_si_gen
  )
add_library(wolfdale SHARED wolfdale.c generate/init.cxx)
set_property(TARGET wolfdale PROPERTY PREFIX "")


add_executable(core_si_gen proc/x8664/core_si.cxx)
target_link_libraries(core_si_gen si_gen isa_gen_topcode)

add_custom_command(
  OUTPUT core.c
  COMMAND core_si_gen
  DEPENDS core_si_gen
  )
add_library(core SHARED core.c generate/init.cxx)
set_property(TARGET core PROPERTY PREFIX "")


add_executable(em64t_si_gen proc/x8664/em64t_si.cxx)
target_link_libraries(em64t_si_gen si_gen isa_gen_topcode)

add_custom_command(
  OUTPUT em64t.c
  COMMAND em64t_si_gen
  DEPENDS em64t_si_gen
  )
add_library(em64t SHARED em64t.c generate/init.cxx)
set_property(TARGET em64t PROPERTY PREFIX "")
ENDIF()
IF (${BUILD_TARGET_DIR} MATCHES "arm")
install(TARGETS armv5e armv6
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${PATH64_LIB_PATH})
ELSE()
install(TARGETS barcelona em64t core wolfdale opteron 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${PATH64_LIB_PATH})
ENDIF()
