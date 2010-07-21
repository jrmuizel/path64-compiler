

path64_is_target_enabled(res x8664_64)
if(res)
    ADD_SUBDIRECTORY(elf-x86_64)
endif()

path64_is_target_enabled(res x8664_32)
if(res)
    ADD_SUBDIRECTORY(elf-i386)
endif()

