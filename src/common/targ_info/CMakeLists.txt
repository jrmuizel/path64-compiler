# We want relocatable objects even though libtarginfo.a is static.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Hide generator binaries away from the rest of the compiler.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

set(targinfo_deps "")

macro(gen_info type name)
  path64_set_multiarch_sources(${name}_gen_sources
                               COMMON
                               generate/${name}_gen.cxx
                               ${type}/@ARCH@/${name}.cxx)
  path64_add_multiarch_executable(${name}_gen ${name}_gen_sources)
  path64_set_multiarch_property(${name}_gen COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/@ARCH@")
  path64_multiarch_link_multiarch_libraries(${name}_gen ${ARGN})

  # Generate each source as its own library so it can be linked into
  # other generator executables with high granularity.
  path64_add_multiarch_custom_command(
    OUTPUT @ARCH@/targ_${name}.c @ARCH@/targ_${name}.h
    COMMAND @ARCH{${name}_gen}
    DEPENDS @ARCH{${name}_gen}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/@ARCH@
    )
  path64_set_multiarch_sources(targ_${name}_sources COMMON @ARCH@/targ_${name}.c @ARCH@/targ_${name}.h)
  path64_add_multiarch_library(targ_${name} STATIC targ_${name}_sources)

  # The final targinfo library depends on all individual sources.
  list(APPEND targinfo_deps targ_${names})
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

path64_set_multiarch_sources(gen_util_sources COMMON generate/gen_util.cxx)
path64_add_multiarch_library(gen_util STATIC gen_util_sources)

path64_set_multiarch_sources(isa_gen_sources COMMON
                             generate/isa_gen.cxx
                             isa/@ARCH@/isa.cxx)
path64_add_multiarch_executable(isa_gen isa_gen_sources)
path64_multiarch_link_multiarch_libraries(isa_gen gen_util)

foreach(arch ${PATH64_ENABLE_ARCHES})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${arch})
endforeach()

path64_add_multiarch_custom_command(
  OUTPUT @ARCH@/topcode.c @ARCH@/topcode.h
  COMMAND @ARCH{isa_gen}
  DEPENDS @ARCH{isa_gen}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/@ARCH@
  )

path64_set_multiarch_sources(isa_gen_topcode_sources COMMON @ARCH@/topcode.c @ARCH@/topcode.h)
path64_add_multiarch_library(isa_gen_topcode STATIC isa_gen_topcode_sources)
path64_multiarch_link_multiarch_libraries(isa_gen_topcode gen_util)

foreach(arch ${PATH64_ENABLE_ARCHES})
    SET_SOURCE_FILES_PROPERTIES(${PATHSCALE_BINARY_DIR}/common/targ_info/${arch}/topcode.h PROPERTIES GENERATED 1)
    SET_SOURCE_FILES_PROPERTIES(${PATHSCALE_BINARY_DIR}/common/targ_info/${arch}/topcode.c PROPERTIES GENERATED 1)
endforeach()

path64_add_multiarch_custom_target(generate_topcode DEPENDS 
                                   ${CMAKE_CURRENT_BINARY_DIR}/@ARCH@/topcode.h
                                   ${CMAKE_CURRENT_BINARY_DIR}/@ARCH@/topcode.c)
path64_is_arch_enabled(arm_enabled arm)
if(${arm_enabled})
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
else()
gen_info(isa isa_print        targ_isa_operands targ_isa_properties isa_gen_topcode)
endif()
gen_info(isa isa_enums        gen_util)
gen_info(isa isa_lits         isa_gen_topcode)

gen_info(isa isa_properties   isa_gen_topcode)
gen_info(isa isa_bundle       targ_isa_subset targ_isa_pack targ_proc targ_isa_properties isa_gen_topcode)
gen_info(isa isa_decode       targ_isa_subset targ_isa_bundle targ_isa_pack isa_gen_topcode)

gen_info(isa isa_subset       isa_gen_topcode)
gen_info(isa isa_hazards      targ_isa_subset isa_gen_topcode)
gen_info(isa isa_registers    targ_isa_subset isa_gen_topcode)
path64_multiarch_link_multiarch_libraries(targ_isa_registers targ_isa_subset)
if(${arm_enabled})
gen_info(isa isa_variants      isa_gen_topcode)
gen_info(isa isa_binutils      targ_isa_subset targ_isa_pack isa_gen_topcode)
gen_info(isa isa_relocs        targ_isa_subset targ_isa_lits isa_gen_topcode)
gen_info(isa isa_operands      targ_isa_relocs targ_isa_properties targ_isa_registers targ_isa_enums targ_isa_lits isa_gen_topcode)
gen_info(isa isa_pack          targ_isa_properties targ_isa_operands isa_gen_topcode)
else()
gen_info(isa isa_operands      targ_isa_properties targ_isa_registers targ_isa_enums targ_isa_lits isa_gen_topcode)
gen_info(isa isa_pack         targ_isa_properties isa_gen_topcode)
endif()
gen_info(isa isa_pseudo       targ_isa_operands targ_isa_registers targ_isa_enums isa_gen_topcode)

gen_info(proc proc            gen_util)
gen_info(proc proc_properties targ_proc gen_util)
gen_info(abi  abi_properties  targ_isa_subset targ_isa_registers gen_util)
#debater
if(${arm_enabled})
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
endif()
# Collect the rest of the sources in a 'targinfo' library.
path64_set_multiarch_sources(targinfo_common_sources COMMON
	access/ti_asm.c
	access/ti_bundle.c
	access/ti_latency.c
	access/ti_errors.c
	@ARCH@/topcode.c
	@ARCH@/targ_isa_subset.c
	@ARCH@/targ_isa_properties.c
	@ARCH@/targ_isa_enums.c
	@ARCH@/targ_isa_hazards.c
	@ARCH@/targ_isa_lits.c
	@ARCH@/targ_isa_print.c
	@ARCH@/targ_isa_pack.c
	@ARCH@/targ_isa_operands.c
	@ARCH@/targ_isa_registers.c
	@ARCH@/targ_isa_decode.c
	@ARCH@/targ_isa_pseudo.c
	@ARCH@/targ_abi_properties.c
	@ARCH@/targ_isa_bundle.c
	@ARCH@/targ_proc.c
	@ARCH@/targ_proc_properties.c
)

path64_set_multiarch_sources(targinfo_arm_sources targ_isa_relocs.c
        targ_isa_variants.c
        targ_isa_binutils.c
        targ_isa_parse.c
        armv5e_subset_targ_isa_parse.c 
        armv6_subset_targ_isa_parse.c )

if(${arm_enabled})
set(targinfo_sources ${targinfo_common_sources} ${targinfo_arm_sources})
else()
set(targinfo_sources ${targinfo_common_sources})
endif()
path64_add_multiarch_library(targinfo STATIC targinfo_sources)
path64_set_multiarch_property(targinfo COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/@ARCH@")
path64_add_multiarch_multiarch_dependencies(targinfo ${targinfo_deps})


#-----------------------------------------------------------------------------
path64_set_multiarch_sources(si_gen_sources COMMON generate/si_gen.cxx)
path64_add_multiarch_library(si_gen STATIC si_gen_sources)
path64_set_multiarch_property(si_gen COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/@ARCH@")
path64_multiarch_link_multiarch_libraries(si_gen
  targ_isa_subset
  targ_isa_properties
  targ_isa_operands
  targ_isa_registers
  targ_isa_enums
  targ_isa_lits
  )

# X8664 architecture specific
path64_is_arch_enabled(x8664_enabled x8664)
if(${x8664_enabled})
    path64_get_multiarch_cmake_target(si_gen_x8664_name si_gen x8664)
    path64_get_multiarch_cmake_target(isa_gen_topcode_x8664_name isa_gen_topcode x8664)

    add_executable(opteron_si_gen proc/x8664/opteron_si.cxx)
    target_link_libraries(opteron_si_gen ${si_gen_x8664_name} ${isa_gen_topcode_x8664_name})
    set_property(TARGET opteron_si_gen APPEND PROPERTY
                 COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/x8664")

    add_custom_command(
      OUTPUT opteron.c
      COMMAND opteron_si_gen
      DEPENDS opteron_si_gen
      )
    add_library(opteron SHARED opteron.c generate/init.cxx)
    set_property(TARGET opteron PROPERTY PREFIX "")
    set_property(TARGET opteron APPEND PROPERTY
                 COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/x8664")

    add_executable(barcelona_si_gen proc/x8664/barcelona_si.cxx)
    target_link_libraries(barcelona_si_gen ${si_gen_x8664_name} ${isa_gen_topcode_x8664_name})
    set_property(TARGET barcelona_si_gen APPEND PROPERTY
                 COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/x8664")

    add_custom_command(
      OUTPUT barcelona.c
      COMMAND barcelona_si_gen
      DEPENDS barcelona_si_gen
      )
    add_library(barcelona SHARED barcelona.c generate/init.cxx)
    set_property(TARGET barcelona PROPERTY PREFIX "")
    set_property(TARGET barcelona APPEND PROPERTY
                 COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/x8664")


    add_executable(wolfdale_si_gen proc/x8664/wolfdale_si.cxx)
    target_link_libraries(wolfdale_si_gen ${si_gen_x8664_name} ${isa_gen_topcode_x8664_name})
    set_property(TARGET wolfdale_si_gen APPEND PROPERTY
                 COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/x8664")

    add_custom_command(
      OUTPUT wolfdale.c
      COMMAND wolfdale_si_gen
      DEPENDS wolfdale_si_gen
      )
    add_library(wolfdale SHARED wolfdale.c generate/init.cxx)
    set_property(TARGET wolfdale PROPERTY PREFIX "")
    set_property(TARGET wolfdale APPEND PROPERTY
                 COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/x8664")


    add_executable(core_si_gen proc/x8664/core_si.cxx)
    target_link_libraries(core_si_gen ${si_gen_x8664_name} ${isa_gen_topcode_x8664_name})
    set_property(TARGET core_si_gen APPEND PROPERTY
                 COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/x8664")

    add_custom_command(
      OUTPUT core.c
      COMMAND core_si_gen
      DEPENDS core_si_gen
      )
    add_library(core SHARED core.c generate/init.cxx)
    set_property(TARGET core PROPERTY PREFIX "")
    set_property(TARGET core APPEND PROPERTY
                 COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/x8664")


    add_executable(em64t_si_gen proc/x8664/em64t_si.cxx)
    target_link_libraries(em64t_si_gen ${si_gen_x8664_name} ${isa_gen_topcode_x8664_name})
    set_property(TARGET em64t_si_gen APPEND PROPERTY
                 COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/x8664")

    add_custom_command(
      OUTPUT em64t.c
      COMMAND em64t_si_gen
      DEPENDS em64t_si_gen
      )
    add_library(em64t SHARED em64t.c generate/init.cxx)
    set_property(TARGET em64t PROPERTY PREFIX "")
    set_property(TARGET em64t APPEND PROPERTY
                 COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/x8664")

    install(TARGETS barcelona em64t core wolfdale opteron
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION ${PATH64_LIB_PATH}/x8664)
endif()

# ARM architecture specific 
if(${arm_enabled})
    path64_get_multiarch_cmake_target(si_gen_arm_name si_gen arm)
    path64_get_multiarch_cmake_target(isa_gen_topcode_arm_name isa_gen_topcode arm)

    add_executable(oarmv5e_si_gen proc/arm/armv5e_si.cxx)
    target_link_libraries(armv5e_si_gen ${si_gen_arm_name} ${isa_gen_topcode_arm_name})
    set_property(TARGET armv5e_si_gen APPEND PROPERTY
                 COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/arm")

   add_custom_command(
       OUTPUT armv5e.c
       COMMAND armv5e_si_gen
       DEPENDS armv5e_si_gen
   )
   add_library(armv5e SHARED armv5e.c generate/init.cxx)

    set_property(TARGET armv5e PROPERTY PREFIX "")
    set_property(TARGET armv5e APPEND PROPERTY
                 COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/arm")

 add_executable(oarmv6_si_gen proc/arm/armv6_si.cxx)
    target_link_libraries(armv6_si_gen ${si_gen_arm_name} ${isa_gen_topcode_arm_name})
    set_property(TARGET armv6_si_gen APPEND PROPERTY
                 COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/arm")

   add_custom_command(
       OUTPUT armv6.c
       COMMAND armv6_si_gen
       DEPENDS armv6_si_gen
   )
   add_library(armv6 SHARED armv6.c generate/init.cxx)

    set_property(TARGET armv6 PROPERTY PREFIX "")
    set_property(TARGET armv6 APPEND PROPERTY
                 COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/arm")
  install(TARGETS armv5e armv6
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${PATH64_LIB_PATH}/arm)


endif()
>>>>>>> 8ba551f58c7d9540b69625a291588d344b63af01
