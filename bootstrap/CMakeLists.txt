
if(NOT PATH64_BOOTSTRAP_STAGES)
	SET(PATH64_BOOTSTRAP_STAGES 2)
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/bootstrap-test.sh.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/bootstrap-test.sh")

path64_get_host_target(host_targ)
path64_is_target_enabled(host_targ_enabled ${host_targ})
if(host_targ_enabled)
    set(HOST_TARG_FLAGS ${_PATH64_TARGET_FLAG_${host_targ}})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/run_test.sh.cmake.in
                   ${CMAKE_CURRENT_BINARY_DIR}/run_test.sh)

    # adding hello test for C
    add_test(hello_c ${CMAKE_CURRENT_BINARY_DIR}/run_test.sh hello_c pathcc hello_c.c)

    # adding hello test for CPP
    add_test(hello_cpp ${CMAKE_CURRENT_BINARY_DIR}/run_test.sh hello_cpp pathCC hello_cpp.cpp)
endif()

add_test(path64_bootstrap_test "${CMAKE_CURRENT_BINARY_DIR}/bootstrap-test.sh")

