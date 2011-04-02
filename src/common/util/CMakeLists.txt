include_directories(
  ${PATHSCALE_SOURCE_DIR}/common/util
  ${PATHSCALE_SOURCE_DIR}/common/com
  )

IF(NOT PATH64_ENABLE_VALGRIND)
add_definitions(
  -DNO_VALGRIND
  ${PATH64_STDC_FORMAT_MACROS}
 )
ELSE(NOT PATH64_ENABLE_VALGRIND)
add_definitions(
  ${PATH64_STDC_FORMAT_MACROS}
 )
ENDIF(NOT PATH64_ENABLE_VALGRIND)

set(COMUTIL_SRCS
  cxx_memory.cxx
  errors.cxx
  bitset.c
  c_q_rel.c
  dso.c
  file_util.c
  flags.c
  libelftc_dem_gnu3.c
  libelftc_vstr.c
  memory.c
  priority_queue.c
  quadsim.c
  resource.c
  mstack.c
  tlog.c
  tracing.c
  util.c
  vstring.c
  vspec.c
  vspec_parse.c
  vspec_token.c
  c_a_to_q.c
  c_q_to_a.c
  c_qtenscale.c
  @ARCH@/c_qwmultu.c
  )

path64_set_multiarch_sources(comutil_sources COMMON ${COMUTIL_SRCS})
path64_add_multiarch_library(comutil STATIC comutil_sources)
path64_set_multiarch_property(comutil COMPILE_FLAGS
                              "-I${CMAKE_CURRENT_SOURCE_DIR}/../com/@ARCH@ -fPIC")

# Building comutil for host architecture (used when building fortran)
path64_get_host_arch(host_arch)
path64_add_library_for_arch(comutil-host ${host_arch} STATIC ${COMUTIL_SRCS})
path64_set_property_for_arch(comutil-host ${host_arch} COMPILE_FLAGS
                             "-I${CMAKE_CURRENT_SOURCE_DIR}/../com/@ARCH@ -fPIC")



add_executable(gen_x_set gen_x_set.c string_case.cxx)

add_executable(gen_x_list gen_x_list.c string_case.cxx)

add_executable(gen_x_prop gen_x_prop.c string_case.cxx)
