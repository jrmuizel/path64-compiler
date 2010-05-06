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

gen_info(isa isa_enums        gen_util)
gen_info(isa isa_lits         isa_gen_topcode)

gen_info(isa isa_properties   isa_gen_topcode)
gen_info(isa isa_pack         targ_isa_properties isa_gen_topcode)
gen_info(isa isa_print        targ_isa_properties isa_gen_topcode)
gen_info(isa isa_bundle       targ_isa_properties isa_gen_topcode)
gen_info(isa isa_decode       targ_isa_bundle targ_isa_pack isa_gen_topcode)

gen_info(isa isa_subset       isa_gen_topcode)
gen_info(isa isa_hazards      targ_isa_subset isa_gen_topcode)
gen_info(isa isa_registers    targ_isa_subset isa_gen_topcode)
target_link_libraries(targ_isa_registers targ_isa_subset)

gen_info(isa isa_operands     targ_isa_properties targ_isa_registers targ_isa_enums targ_isa_lits isa_gen_topcode)
gen_info(isa isa_pseudo       targ_isa_operands targ_isa_registers targ_isa_enums isa_gen_topcode)

gen_info(proc proc            gen_util)
gen_info(proc proc_properties targ_proc gen_util)
gen_info(abi  abi_properties  targ_isa_subset targ_isa_registers gen_util)

# Collect the rest of the sources in a 'targinfo' library.
add_library(targinfo STATIC
	access/ti_asm.c
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
	targ_proc_properties.c
)
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

install(TARGETS barcelona em64t core wolfdale opteron
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${PATH64_LIB_PATH})
