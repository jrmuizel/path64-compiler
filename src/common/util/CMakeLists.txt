include_directories(
  ${PATHSCALE_SOURCE_DIR}/common/util
  ${PATHSCALE_SOURCE_DIR}/common/com
  ${PATHSCALE_SOURCE_DIR}/common/com/${BUILD_TARGET_DIR}
  )

IF(NOT PATH64_ENABLE_VALGRIND)
add_definitions(
  -DNO_VALGRIND
 )
ENDIF(NOT PATH64_ENABLE_VALGRIND)

add_library(comutil STATIC
  cxx_memory.cxx
  errors.cxx
  bitset.c
  c_q_rel.c
  file_util.c
  flags.c
  linklist.c
  memory.c
  priority_queue.c
  quadsim.c
  resource.c
  mstack.c
  tracing.c
  util.c
  vstring.c
  c_a_to_q.c
  c_q_to_a.c
  c_qtenscale.c
  ${BUILD_TARGET_DIR}/c_qwmultu.c
  )

add_executable(gen_x_set gen_x_set.c)

add_executable(gen_x_list gen_x_list.c)

add_executable(gen_x_prop gen_x_prop.c)

add_executable(symlink symlink.c)
target_link_libraries(symlink winlib)
