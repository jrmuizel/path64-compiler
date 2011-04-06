

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    INCLUDE_DIRECTORIES(BEFORE ${GCC_SOURCE_DIR}/include)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

path64_set_multiarch_sources(spin_sources COMMON
                              gspin-alloc.c
            gspin-assert.c
            gspin-io.c
            gspin-list.c
            gspin-mempool.c
            gspin-tel.c
            gspin-test-driver.c
            gspin-tree.c)

path64_add_multiarch_library(spin STATIC spin_sources)


path64_set_multiarch_sources(gspin_sources COMMON
                  gspin-test-driver.c)
path64_add_multiarch_executable(gspin gspin_sources)
path64_multiarch_link_multiarch_libraries(gspin spin)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    path64_multiarch_link_libraries(gspin winlib)
ENDIF()
if("X${CMAKE_BUILD_TYPE}" STREQUAL "XDebug")
    path64_install_multiarch(gspin)
endif()
