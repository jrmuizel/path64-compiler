
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    INCLUDE_DIRECTORIES(BEFORE ${GCC_SOURCE_DIR}/include)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

ADD_LIBRARY(spin
            gspin-alloc.c
            gspin-assert.c
            gspin-io.c
            gspin-list.c
            gspin-mempool.c
            gspin-tel.c
            gspin-test-driver.c
            gspin-tree.c)

ADD_EXECUTABLE(gspin gspin-test-driver.c)
target_link_libraries(gspin spin)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(gspin winlib)
ENDIF()

install(TARGETS gspin DESTINATION ${PATH64_LIB_PATH} CONFIGURATIONS Debug)
