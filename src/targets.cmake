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


# Architecture flags
set(_PATH64_ARCH_FLAGS_x8664 "-DTARG_X8664")
set(_PATH64_ARCH_FLAGS_mips "-DTARG_MIPS")     # TODO: fix it
set(_PATH64_ARCH_FLAGS_rsk6 "-DTARG_RSK6")     # TODO: fix it


# Building list of enabled architectures
set(PATH64_ENABLE_ARCHES)
foreach(targ ${PATH64_ENABLE_TARGETS})
    set(targ_arch ${_PATH64_TARGET_ARCH_${targ}})
    list(FIND PATH64_ENABLE_ARCHES ${targ_arch} res)
    if(${res} EQUAL -1)
        list(APPEND PATH64_ENABLE_ARCHES ${targ_arch})
    endif()
endforeach()

message(STATUS "PATH64_ENABLE_ARCHES: ${PATH64_ENABLE_ARCHES}")


# Replaces substring in all strings in list
function(list_string_replace lst pattern replace)
    set(res)
    foreach(str ${${lst}})
        string(REPLACE ${pattern} ${replace} new_str ${str})
        list(APPEND res ${new_str}) 
    endforeach()

    set(${lst} ${res} PARENT_SCOPE)
endfunction()


# Replaces in list of strings:
# 1) All @PATTERN@ patterns with ${pattern_val}
# 2) All @PATTERN{name} patterns with name-${pattern_val}
function(list_string_replace_patterns lst pattern pattern_val)
    set(alist ${${lst}})
    list_string_replace(alist "@${pattern}@" ${pattern_val})

    set(new_lst)
    foreach(x ${alist})
        string(REGEX REPLACE "@${pattern}{(.+)}" "\\1-${pattern_val}" new_x ${x})
        list(APPEND new_lst ${new_x})
    endforeach()

    set(${lst} ${new_lst} PARENT_SCOPE)
endfunction()


function(list_string_replace_arch lst arch)
    set(blist ${${lst}})
    list_string_replace_patterns(blist "ARCH" ${arch})
    set(${lst} ${blist} PARENT_SCOPE)
endfunction()


# Returns multitarget cmake target name for specified target
function(path64_get_multitarget_cmake_target res_var name target)
    set(${res_var} ${name}-${target} PARENT_SCOPE)
endfunction()


# Returns multiarch cmake target name for specified architecture
function(path64_get_multiarch_cmake_target res_var name arch)
    set(${res_var} ${name}-${arch} PARENT_SCOPE)
endfunction()


# Returns list of cmake targets for specified multiarch target
function(path64_get_multiarch_targets res_var name)
    set(res)
    foreach(arch ${PATH64_ENABLE_ARCHES})
        path64_get_multiarch_cmake_target(targ ${name} ${arch})
        list(APPEND res ${targ})
    endforeach()

    set(${res_var} ${res} PARENT_SCOPE)
endfunction()


# Checks that target with specified name exists
function(path64_check_target_exists tname)
    if(NOT _PATH64_TARGET_ARCH_${tname})
        message(FATAL_ERROR "Target with name '${tname}' does not exist")
    endif()
endfunction()


# Checks that arch with specified name exists
function(path64_check_arch_exists aname)
    if(NOT _PATH64_ARCH_FLAGS_${aname})
        message(FATAL_ERROR "Architecture with name '${aname}' does not exist")
    endif()
endfunction()


# Returns true if specified architecture is enabled
function(path64_is_arch_enabled res_var arch)
    list(FIND PATH64_ENABLE_ARCHES ${arch} res)
    if(res EQUAL -1)
        set(${res_var} false PARENT_SCOPE)
    else()
        set(${res_var} true PARENT_SCOPE)
    endif()
endfunction()


# Sets sources for specified target in multitarget source list
function(path64_set_multitarget_sources src_list_name target)
    string(COMPARE EQUAL "${target}" "COMMON" res)
    if(NOT res)
        path64_check_target_exists("${target}")
    endif()

    set(${src_list_name}_${target} ${ARGN} PARENT_SCOPE)
endfunction()


# Sets sources for specified architecture in multiarch source list
function(path64_set_multiarch_sources src_list_name arch)
    if(NOT "X${arch}" STREQUAL "XCOMMON")
        path64_check_arch_exists("${arch}")
    endif()

    set(${src_list_name}_${arch} ${ARGN} PARENT_SCOPE)
endfunction()


# Adds library for specified target
function(path64_add_library_for_target name target type)
    # Compiler ABI.
    set(arch ${_PATH64_TARGET_ARCH_${target}})
    set(bits ${_PATH64_TARGET_BITS_${target}})
    set(arch_flag "${_PATH64_TARGET_FLAG_${target}} ${_PATH64_ARCH_FLAGS_${arch}}")
    set(build_lib_dir ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${arch}/${bits})
    set(install_lib_dir ${PATH64_LIB_PATH}/${arch}/${bits})
    set(install_inc_dir ${install_lib_dir}/include)

    path64_get_multitarget_cmake_target(lib_name ${name} ${target})

    # Replacing @TARGET@ with target name in source names
    set(sources ${ARGN})
    list_string_replace(sources "@TARGET@" ${target})

    add_library (${lib_name} ${type} ${sources})
    set_property(TARGET ${lib_name} PROPERTY OUTPUT_NAME ${name})
    set_property(TARGET ${lib_name} PROPERTY COMPILE_FLAGS ${arch_flag})
    set_property(TARGET ${lib_name} PROPERTY LINK_FLAGS ${arch_flag})

    set_property(TARGET ${lib_name} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${build_lib_dir})
    set_property(TARGET ${lib_name} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${build_lib_dir})

    install(TARGETS ${lib_name}
      LIBRARY DESTINATION ${install_lib_dir}
      ARCHIVE DESTINATION ${install_lib_dir})

endfunction()


# Adds library for specified architecture
function(path64_add_library_for_arch name arch type)
    # Compiler ABI.
    set(arch_flags ${_PATH64_ARCH_FLAGS_${arch}})
    set(build_lib_dir ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${arch})
    set(install_lib_dir ${PATH64_LIB_PATH}/${arch})
    set(install_inc_dir ${install_lib_dir}/include)

    path64_get_multiarch_cmake_target(lib_name ${name} ${arch})

    # Replacing @ARCH@ with architecture  name in source names
    set(sources ${ARGN})
    list_string_replace_arch(sources ${arch})

    add_library (${lib_name} ${type} ${sources})
    set_property(TARGET ${lib_name} PROPERTY OUTPUT_NAME ${name})
    set_property(TARGET ${lib_name} PROPERTY COMPILE_FLAGS ${arch_flags})
    set_property(TARGET ${lib_name} PROPERTY LINK_FLAGS ${arch_flags})

    set_property(TARGET ${lib_name} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${build_lib_dir})
    set_property(TARGET ${lib_name} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${build_lib_dir})

    install(TARGETS ${lib_name}
      LIBRARY DESTINATION ${install_lib_dir}
      ARCHIVE DESTINATION ${install_lib_dir})

endfunction()


# Adds executable for specified architecture
function(path64_add_executable_for_arch name arch)
    # Compiler ABI.
    set(arch_flags ${_PATH64_ARCH_FLAGS_${arch}})
    set(build_lib_dir ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${arch})
    set(install_lib_dir ${PATH64_LIB_PATH}/${arch})
    set(install_inc_dir ${install_lib_dir}/include)

    path64_get_multiarch_cmake_target(exe_name ${name} ${arch})

    # Replacing @ARCH@ with architecture  name in source names
    set(sources ${ARGN})
    list_string_replace_arch(sources ${arch})

    add_executable(${exe_name} ${sources})
    set_property(TARGET ${exe_name} PROPERTY OUTPUT_NAME ${name})
    set_property(TARGET ${exe_name} PROPERTY COMPILE_FLAGS ${arch_flags})
    set_property(TARGET ${exe_name} PROPERTY LINK_FLAGS ${arch_flags})

    set_property(TARGET ${exe_name} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${build_lib_dir})

    install(TARGETS ${exe_name}
      RUNTIME DESTINATION ${install_lib_dir})

endfunction()


# Adds library for all enabled targets
function(path64_add_multitarget_library name type src_list_name)
    foreach(targ ${PATH64_ENABLE_TARGETS})
        if(${src_list_name}_${targ})
            path64_add_library_for_target(${name} ${targ} ${type} ${${src_list_name}_${targ}})
        else()
            path64_add_library_for_target(${name} ${targ} ${type} ${${src_list_name}_COMMON})
        endif()
    endforeach()
endfunction()


# Adds library for all enabled architectures
function(path64_add_multiarch_library name type src_list_name)
    foreach(arch ${PATH64_ENABLE_ARCHES})
        if(${src_list_name}_${arch})
            path64_add_library_for_arch(${name} ${arch} ${type} ${${src_list_name}_${arch}})
        else()
            path64_add_library_for_arch(${name} ${arch} ${type} ${${src_list_name}_COMMON})
        endif()
    endforeach()
endfunction()


# Adds executable for all enabled architectures
function(path64_add_multiarch_executable name src_list_name)
    foreach(arch ${PATH64_ENABLE_ARCHES})
        if(${src_list_name}_${arch})
            path64_add_executable_for_arch(${name} ${arch} ${${src_list_name}_${arch}})
        else()
            path64_add_executable_for_arch(${name} ${arch} ${${src_list_name}_COMMON})
        endif()
    endforeach()
endfunction()


# Sets property in multitarget for specified target
function(path64_set_property_for_target name targ prop)
    set(prop_vals ${ARGN})

    if(prop MATCHES "COMPILE_FLAGS" OR prop MATCHES "LINK_FLAGS")
        set(arch ${_PATH64_TARGET_ARCH_${targ}})
        set(arch_flag "${_PATH64_TARGET_FLAG_${targ}} ${_PATH64_ARCH_FLAGS_${arch}}")
        set(prop_vals "${prop_vals} ${arch_flag}")
    endif()

    path64_get_multitarget_cmake_target(tg ${name} ${targ})
    set_property(TARGET ${tg} PROPERTY ${prop} ${prop_vals})
endfunction()


# Sets property in multiarch target for specified archiecture
function(path64_set_property_for_arch name arch prop)
    set(prop_vals ${ARGN})
    list_string_replace_arch(prop_vals ${arch})

    if(prop MATCHES "COMPILE_FLAGS" OR prop MATCHES "LINK_FLAGS")
        set(prop_vals "${prop_vals} ${_PATH64_ARCH_FLAGS_${arch}}")
    endif()

    path64_get_multiarch_cmake_target(tg ${name} ${arch})
    set_property(TARGET ${tg} PROPERTY ${prop} ${prop_vals})
endfunction()


# Sets property for multitarget
function(path64_set_multitarget_property name prop)
    foreach(targ ${PATH64_ENABLE_TARGETS})
        path64_set_property_for_target(${name} ${targ} ${prop} ${ARGN})
    endforeach()
endfunction()


# Sets property for multiarch target
function(path64_set_multiarch_property name prop)
    foreach(arch ${PATH64_ENABLE_ARCHES})
        path64_set_property_for_arch(${name} ${arch} ${prop} ${ARGN})
    endforeach()
endfunction()


# Adds link libraries to multitarget
function(path64_multitarget_link_libraries name)
    foreach(targ ${PATH64_ENABLE_TARGETS})
        path64_get_multitarget_cmake_target(tg ${name} ${targ})
        target_link_libraries(${tg} ${ARGN})
    endforeach()
endfunction()


# Adds link libraries to multiarch target
function(path64_multiarch_link_libraries name)
    foreach(arch ${PATH64_ENABLE_ARCHES})
        path64_get_multiarch_cmake_target(tg ${name} ${arch})
        target_link_libraries(${tg} ${ARGN})
    endforeach()
endfunction()


# Adds multiarch link libraries to multiarch target
function(path64_multiarch_link_multiarch_libraries name)
    foreach(arch ${PATH64_ENABLE_ARCHES})
        path64_get_multiarch_cmake_target(tg ${name} ${arch})
        foreach(lib ${ARGN})
            path64_get_multiarch_cmake_target(lib_tg ${lib} ${arch})
            target_link_libraries(${tg} ${lib_tg})
        endforeach()
    endforeach()
endfunction()


# Adds custom multiarch command
function(path64_add_multiarch_custom_command)
    # reading parameters
    set(outputs)
    set(cmd_with_args)
    set(depends)
    set(work_dir "")
    set(read_stage "UNKNOWN")

    foreach(arg ${ARGN})
        set(new_read_stage "UNKNOWN")
        
        if("X${arg}" STREQUAL "XOUTPUT")
            set(new_read_stage "OUTPUT")
        elseif("X${arg}" STREQUAL "XCOMMAND")
            set(new_read_stage "COMMAND")
        elseif("X${arg}" STREQUAL "XDEPENDS")
            set(new_read_stage "DEPENDS")
        elseif("X${arg}" STREQUAL "XWORKING_DIRECTORY")
            set(new_read_stage "WORKING_DIRECTORY")
        endif()

        if("X${new_read_stage}" STREQUAL "XUNKNOWN")
            if("X${read_stage}" STREQUAL "XOUTPUT")
                list(APPEND outputs ${arg})
            elseif("X${read_stage}" STREQUAL "XCOMMAND")
                list(APPEND cmd_with_args ${arg})
            elseif("X${read_stage}" STREQUAL "XDEPENDS")
                list(APPEND depends ${arg})
            elseif("X${read_stage}" STREQUAL "XWORKING_DIRECTORY")
                set(work_dir ${arg})
            endif()
        else()
            set(read_stage ${new_read_stage})
        endif()
    endforeach()

    # adding custom commands for each architecture
    foreach(arch ${PATH64_ENABLE_ARCHES})
        # replacing @ARCH@ with arch name in all parameters
        set(arch_outputs ${outputs})
        set(arch_depends ${depends})
        set(arch_cmd_with_args ${cmd_with_args})
        list_string_replace_arch(arch_outputs ${arch})
        list_string_replace_arch(arch_depends ${arch})
        list_string_replace_arch(arch_cmd_with_args ${arch})
        list_string_replace_arch(work_dir ${arch})

        # adding command
        add_custom_command(OUTPUT ${arch_outputs}
                           COMMAND ${arch_cmd_with_args}
                           DEPENDS ${arch_depends}
                           WORKING_DIRECTORY ${work_dir})
    endforeach()
endfunction()


# Adds custom multiarch target
function(path64_add_multiarch_custom_target name)
    # reading parameters
    set(cmd_with_args)
    set(depends)
    set(read_stage "UNKNOWN")

    foreach(arg ${ARGN})
        set(new_read_stage "UNKNOWN")
        if("X${arg}" STREQUAL "XCOMMAND")
            set(new_read_stage "COMMAND")
        elseif("X${arg}" STREQUAL "XDEPENDS")
            set(new_read_stage "DEPENDS")
        endif()

        if("X${new_read_stage}" STREQUAL "XUNKNOWN")
            if("X${read_stage}" STREQUAL "XCOMMAND")
                list(APPEND cmd_with_args ${arg})
            elseif("X${read_stage}" STREQUAL "XDEPENDS")
                list(APPEND depends ${arg})
            endif()
        else()
            set(read_stage ${new_read_stage})
        endif()
    endforeach()

    # adding custom commands for each architecture
    foreach(arch ${PATH64_ENABLE_ARCHES})
        # replacing @ARCH@ with arch name in all parameters
        set(arch_outputs ${outputs})
        set(arch_depends ${depends})
        set(arch_cmd_with_args ${cmd_with_args})
        list_string_replace_arch(arch_outputs ${arch})
        list_string_replace_arch(arch_depends ${arch})
        list_string_replace_arch(arch_cmd_with_arg ${arch})

        # adding target
        path64_get_multiarch_cmake_target(tg ${name} ${arch})
        if(arch_cmd_with_args)
            add_custom_target(${tg}
                              COMMAND ${arch_cmd_with_args}
                              DEPENDS ${arch_depends})
        else()
            add_custom_target(${tg}
                              DEPENDS ${arch_depends})
        endif()
    endforeach()
endfunction()


# Adds dependency to multiarch target
function(path64_add_multiarch_dependencies name)
    foreach(arch ${PATH64_ENABLE_ARCHES})
        path64_get_multiarch_cmake_target(tg ${name} ${arch})
        add_dependencies(${tg} ${ARGN})
    endforeach()
endfunction()


# Adds dependency to multiarch target from multiarch targets
function(path64_add_multiarch_multiarch_dependencies name)
    foreach(arch ${PATH64_ENABLE_ARCHES})
        path64_get_multiarch_cmake_target(tg ${name} ${arch})
        foreach(dep ${ARGN})
            path64_get_multiarch_cmake_target(dep_tg ${dep} ${arch})
            add_dependencies(${tg} ${dep_tg})
        endforeach()
    endforeach()
endfunction()

