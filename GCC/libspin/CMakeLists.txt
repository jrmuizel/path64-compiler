

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
install(TARGETS gspin DESTINATION ${PATH64_LIB_PATH} CONFIGURATIONS Debug)
