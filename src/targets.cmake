# message() string listing allowed target architectures.
set(_PATH64_SUPPORTED_TARGETS_STRING "Supported architectures are:
  x8664_32
  x8664_64
  mips_32
  mips_64
  rsk6_32
  rsk6_64")

# Target information table, keyed by entries of PATH64_ENABLE_TARGETS.
# Reference table entries with ${_PATH64_TARGET_ARCH_${arch}}.

set(_PATH64_TARGET_ARCH_x8664_32 x8664)
set(_PATH64_TARGET_FLAG_x8664_32 -m32)
set(_PATH64_TARGET_BITS_x8664_32 32)

set(_PATH64_TARGET_ARCH_x8664_64 x8664)
set(_PATH64_TARGET_FLAG_x8664_64 -m64)
set(_PATH64_TARGET_BITS_x8664_64 64)

set(_PATH64_TARGET_ARCH_mips_32 mips)
set(_PATH64_TARGET_FLAG_mips_32 -n32)
set(_PATH64_TARGET_BITS_mips_32 32)

set(_PATH64_TARGET_ARCH_mips_64 mips)
set(_PATH64_TARGET_FLAG_mips_64 -n64)
set(_PATH64_TARGET_BITS_mips_64 64)

set(_PATH64_TARGET_ARCH_rsk6_32 rsk6)
set(_PATH64_TARGET_FLAG_rsk6_32 -q32)
set(_PATH64_TARGET_BITS_rsk6_32 32)

set(_PATH64_TARGET_ARCH_rsk6_64 rsk6)
set(_PATH64_TARGET_FLAG_rsk6_64 -q64)
set(_PATH64_TARGET_BITS_rsk6_64 64)


# Returns library cmake target name for specified architecture
function(path64_get_target_library_cmake_target res_var name target)
    set(${res_var} ${name}-${target} PARENT_SCOPE)
endfunction()


# Function to add libraries for given target architecture.
# The logical CMake library names are mangled by the architecture, but
# their OUTPUT_NAME on disk is not.  The build output and install
# locations include the architecture.
function(path64_add_library_for_target name target)
    # Getting list of sources from args
    set(sources ${ARGV})
    list(REMOVE_AT sources 0 1)

    # Compiler ABI.
    set(arch ${_PATH64_TARGET_ARCH_${target}})
    set(bits ${_PATH64_TARGET_BITS_${target}})
    set(arch_flag ${_PATH64_TARGET_FLAG_${target}})
    set(build_lib_dir ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${arch}/${bits})
    set(install_lib_dir ${PATH64_LIB_PATH}/${arch}/${bits})
    set(install_inc_dir ${install_lib_dir}/include)

    path64_get_target_library_cmake_target(lib_name ${name} ${target})

    add_library (${lib_name} ${sources})
    set_property(TARGET ${lib_name} PROPERTY OUTPUT_NAME ${name})
    set_property(TARGET ${lib_name} PROPERTY COMPILE_FLAGS ${arch_flag})

    set_property(TARGET ${lib_name} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${build_lib_dir})
    set_property(TARGET ${lib_name} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${build_lib_dir})

    install(TARGETS ${lib_name}
      LIBRARY DESTINATION ${install_lib_dir}
      ARCHIVE DESTINATION ${install_lib_dir})

endfunction()


# Adds library for all enabled targets
function(path64_add_target_library name)
    set(sources ${ARGV})
    list(REMOVE_AT sources 0)

    foreach(targ ${PATH64_ENABLE_TARGETS})
        path64_add_library_for_target(${name} ${targ} ${sources})
    endforeach()
endfunction()


# Sets property for library for specified target
function(path64_set_library_property_for_target name targ prop)
    set(prop_vals ${ARGV})
    list(REMOVE_AT prop_vals 0 1 2)

    if(prop MATCHES "COMPILE_FLAGS")
        set(prop_vals "${prop_vals} ${_PATH64_TARGET_FLAG_${targ}}")
    endif()

    path64_get_target_library_cmake_target(tg ${name} ${targ})
    set_property(TARGET ${tg} PROPERTY ${prop} ${prop_vals})
endfunction()


# Sets property for target library
function(path64_set_target_library_property name prop)
    set(prop_vals ${ARGV})
    list(REMOVE_AT prop_vals 0 1)

    foreach(targ ${PATH64_ENABLE_TARGETS})
        path64_set_library_property_for_target(${name} ${targ} ${prop} ${prop_vals})
    endforeach()
endfunction()

