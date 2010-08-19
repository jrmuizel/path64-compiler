
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
#install(TARGETS gspin DESTINATION ${PATH64_LIB_PATH} CONFIGURATIONS Debug)
