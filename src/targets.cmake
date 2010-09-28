# message() string listing allowed target architectures.
set(_PATH64_SUPPORTED_TARGETS_STRING "Supported architectures are:
  x86_32
  x86_64
  mips_32
  mips_64
  rsk6_32
  rsk6_64
  arm")

# Target information table, keyed by entries of PATH64_ENABLE_TARGETS.
# Reference table entries with ${_PATH64_TARGET_ARCH_${arch}}.

set(_PATH64_TARGET_ARCH_x86_32 x8664)
set(_PATH64_TARGET_FLAG_x86_32 -m32)
set(_PATH64_TARGET_BITS_x86_32 32)
set(_PATH64_TARGET_ABI_x86_32 ABI_M32)

set(_PATH64_TARGET_ARCH_x86_64 x8664)
set(_PATH64_TARGET_FLAG_x86_64 -m64)
set(_PATH64_TARGET_BITS_x86_64 64)
set(_PATH64_TARGET_ABI_x86_64 ABI_M64)

set(_PATH64_TARGET_ARCH_mips_32 mips)
set(_PATH64_TARGET_FLAG_mips_32 -n32)
set(_PATH64_TARGET_BITS_mips_32 32)
set(_PATH64_TARGET_ABI_mips_32 ABI_N32)

set(_PATH64_TARGET_ARCH_mips_64 mips)
set(_PATH64_TARGET_FLAG_mips_64 -n64)
set(_PATH64_TARGET_BITS_mips_64 64)
set(_PATH64_TARGET_ABI_mips_64 ABI_N64)

set(_PATH64_TARGET_ARCH_rsk6_32 rsk6)
set(_PATH64_TARGET_FLAG_rsk6_32 -q32)
set(_PATH64_TARGET_BITS_rsk6_32 32)
set(_PATH64_TARGET_ABI_rsk6_32 ABI_Q32)

set(_PATH64_TARGET_ARCH_rsk6_64 rsk6)
set(_PATH64_TARGET_FLAG_rsk6_64 -q64)
set(_PATH64_TARGET_BITS_rsk6_64 64)
set(_PATH64_TARGET_ABI_rsk6_64 ABI_Q64)

set(_PATH64_TARGET_ARCH_arm arm)
set(_PATH64_TARGET_FLAG_arm -n64)
set(_PATH64_TARGET_BITS_arm 64)
set(_PATH64_TARGET_ABI_arm ABI_ARM_ver1)

# Architecture flags
set(_PATH64_ARCH_FLAGS_x8664 "-DTARG_X8664")
set(_PATH64_ARCH_FLAGS_mips "-DTARG_MIPS")     # TODO: fix it
set(_PATH64_ARCH_FLAGS_rsk6 "-DTARG_RSK6")     # TODO: fix it
set(_PATH64_ARCH_FLAGS_arm "-DTARG_ST -DTARG_ARM -DBE_EXPORTED= -DTARGINFO_EXPORTED= -DSUPPORTS_SELECT -DMUMBLE_ARM_BSP")     # TODO: fix it


# Checking that target architectures are specified
if(NOT PATH64_ENABLE_TARGETS)
    message( FATAL_ERROR "Must specify at least one target architecture.
${_PATH64_SUPPORTED_TARGETS_STRING}
Please add -DPATH64_ENABLE_TARGETS=x86_64 to your cmake ../Path64 line
Multiple targets can be specified with
  \"-DPATH64_ENABLE_TARGETS=x86_32;mips_64\"
on the command line.")
endif()


# Building list of enabled architectures
set(PATH64_ENABLE_ARCHES)
set(_targets "")
set(_sep "")
    
foreach(targ ${PATH64_ENABLE_TARGETS})
    set(targ_arch ${_PATH64_TARGET_ARCH_${targ}})

    if(NOT targ_arch)
        message(FATAL_ERROR "'${targ}' is not among supported architectures.
${_PATH64_SUPPORTED_TARGETS_STRING}
Please edit PATH64_ENABLE_TARGETS to list only valid architectures.
")
    endif()

    list(FIND PATH64_ENABLE_ARCHES ${targ_arch} res)
    if(${res} EQUAL -1)
        list(APPEND PATH64_ENABLE_ARCHES ${targ_arch})
    endif()

    set(_targets "${_targets}${_sep}${targ}")
    set(_sep ", ")
endforeach()


foreach(targ ${PATH64_ENABLE_TARGETS})
    set(targ_arch ${_PATH64_TARGET_ARCH_${targ}})
    set(targ_bits ${_PATH64_TARGET_BITS_${targ}})
    file(MAKE_DIRECTORY ${Path64_BINARY_DIR}/lib/${targ_arch}/${targ_bits})
endforeach()


# First list element is the default.
# TODO: test for the the native build environment and make that the default target
list(GET PATH64_ENABLE_TARGETS 0 PATH64_DEFAULT_TARGET)
message(STATUS
  "Building support for targets ${_targets}.  Default is ${PATH64_DEFAULT_TARGET}.")
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


# Returns file name extension
function(path64_get_file_ext res fname)
    set(ext ${fname})

    while(1)
        get_filename_component(ex ${ext} EXT)

        string(LENGTH "${ex}" exlength)
        if(${exlength} EQUAL 0)
            set(${res} ${ext} PARENT_SCOPE)
            return()
        endif()

        math(EXPR exlength "${exlength} - 1")
        string(SUBSTRING ${ex} 1 ${exlength} ext)
    endwhile()

    set(${res} ${ext} PARENT_SCOPE)
endfunction()


# Returns file name base
function(path64_get_file_base res fn)
    get_filename_component(fname ${fn} NAME)
    path64_get_file_ext(ext ${fname})

    string(LENGTH ${fname} fname_length)
    string(LENGTH ${ext} ext_length)
    math(EXPR base_length "${fname_length} - ${ext_length} - 1")
    string(SUBSTRING "${fname}" 0 ${base_length} base)
    set(${res} ${base} PARENT_SCOPE)
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


# Returns target bits
function(path64_get_target_bits res_var targ)
   set(${res_var} ${_PATH64_TARGET_BITS_${targ}} PARENT_SCOPE)
endfunction()


# Returns target for host system
function(path64_get_host_target res_var)
    # TODO: find way how to get target for host system
    set(${res_var} "x86_64" PARENT_SCOPE)
endfunction()


# Returns architecture for host system
function(path64_get_host_arch res_var)
    path64_get_host_target(targ)
    set(${res_var} ${_PATH64_TARGET_ARCH_${targ}} PARENT_SCOPE)
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


# Returns true if specified target is enabled
function(path64_is_target_enabled res_var targ)
    list(FIND PATH64_ENABLE_TARGETS ${targ} res)
    if(res EQUAL -1)
        set(${res_var} false PARENT_SCOPE)
    else()
        set(${res_var} true PARENT_SCOPE)
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
function(path64_set_multitarget_sources name target)
    string(COMPARE EQUAL "${target}" "COMMON" res)
    if(NOT res)
        path64_check_target_exists("${target}")
    endif()

    set(path64_multitarget_sources_${name}_${target} ${ARGN} PARENT_SCOPE)
endfunction()


# Sets sources for specified architecture in multiarch source list
function(path64_set_multiarch_sources src_list_name arch)
    if(NOT "X${arch}" STREQUAL "XCOMMON")
        path64_check_arch_exists("${arch}")
    endif()

    set(${src_list_name}_${arch} ${ARGN} PARENT_SCOPE)
endfunction()


# path64 compilers for languages
set(path64_compiler_C "${Path64_BINARY_DIR}/bin/pathcc")
set(path64_compiler_CXX "${Path64_BINARY_DIR}/bin/pathCC")
#set(path64_compiler_Fortran "${Path64_BINARY_DIR}/bin/pathf90")
# TODO: enable pathf90
set(path64_compiler_Fortran "${CMAKE_Fortran_COMPILER}")

# Adds library for specified target
function(path64_add_library_for_target name target type)
    path64_check_target_exists(${target})

    get_property(compile_defs DIRECTORY PROPERTY COMPILE_DEFINITIONS)

    # Compiler ABI.
    set(arch ${_PATH64_TARGET_ARCH_${target}})
    set(bits ${_PATH64_TARGET_BITS_${target}})
    set(arch_flag ${_PATH64_TARGET_FLAG_${target}} ${_PATH64_ARCH_FLAGS_${arch}})
    set(build_lib_dir ${Path64_BINARY_DIR}/lib/${arch}/${bits})
    set(install_lib_dir lib/${PSC_FULL_VERSION}/${arch}/${bits})

    # Replacing @TARGET@ with target name in source names
    set(sources ${ARGN})
    list_string_replace(sources "@TARGET@" ${target})

#    add_library (${name} ${type} ${sources})
#    set_property(TARGET ${name} PROPERTY COMPILE_FLAGS ${arch_flag})
#    set_property(TARGET ${name} PROPERTY LINK_FLAGS ${arch_flag})
#
#    set_property(TARGET ${name} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${build_lib_dir})
#    set_property(TARGET ${name} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${build_lib_dir})
     
    set(compile_defs_flags)
    foreach(def ${compile_defs})
        list(APPEND compile_defs_flags "-D${def}")
    endforeach()

    # Searching header dependencies
    set(header_deps)
    foreach(src ${sources})
        path64_get_file_ext(src_ext ${src})
        if("${src_ext}" STREQUAL "h")
            list(APPEND header_deps ${src})
        endif()
    endforeach()

    set(compiler-deps)

    # Adding rules for compiling sources
    set(objects)
    foreach(src ${sources})
        # Getting source language
        get_property(src_lang SOURCE ${src} PROPERTY LANGUAGE)
        if(NOT src_lang)
            # Trying get language from extension
            path64_get_file_ext(src_ext ${src})
            foreach(lang C CXX Fortran)
                foreach(lang_ext ${CMAKE_${lang}_SOURCE_FILE_EXTENSIONS})
                    if("${lang_ext}" STREQUAL "${src_ext}")
                        set(src_lang ${lang})
                        break()
                    endif()
                endforeach()
                if(src_lang)
                    break()
                endif()
            endforeach()

            # Special case for assembler
            if("${src_ext}" STREQUAL "S" OR "${src_ext}" STREQUAL "s")
                set(src_lang C)
            endif()
        endif()

        # special case for headers
        if(NOT "${src_ext}" STREQUAL "h")
            set(last_src_lang ${src_lang})

            if(NOT src_lang)
                message(FATAL_ERROR "Can not determine language for ${src}")
            endif()
    
            # Getting source compile definitions
            get_property(src_compile_defs_list SOURCE ${src} PROPERTY COMPILE_DEFINITIONS)
            set(src_compile_defs_flags)
            foreach(def ${src_compile_defs_list})
                list(APPEND src_compile_defs_flags "-D${def}")
            endforeach()
    
            # Getting source compile flags
            get_property(src_flags_list SOURCE ${src} PROPERTY COMPILE_FLAGS)
            string(REPLACE " " ";" src_flags "${src_flags_list}")
    
            # Getting target compile flags
            string(REPLACE " " ";" target_flags
                    "${path64_multitarget_property_${name}_COMPILE_FLAGS} ${path64_multitarget_property_${name}_${target}_COMPILE_FLAGS}")

            # Gettings language flags
            string(REPLACE " " ";" lang_flags "${CMAKE_${src_lang}_FLAGS}")
    
            # Getting target compile definitions
            set(target_compile_defs)
            foreach(def ${path64_multitarget_property_${name}_COMPILE_DEFINITIONS})
                list(APPEND target_compile_defs "-D${def}")
            endforeach()
    
            # Getting include directories
            get_property(incl_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
            set(incl_dirs_flags)
            foreach(dir ${incl_dirs})
                list(APPEND incl_dirs_flags "-I${dir}")
            endforeach()

            # Getting full path to source
            set(oname ${src})
            if(NOT EXISTS ${src})
                # Trying path relative to current source dir
                if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
                    set(src "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
                else()
                    message(FATAL_ERROR "Can not find ${src} source")
                endif()
            endif()
    
            # Getting object output name and making path to it
            string(REPLACE "." "_" oname_mangled ${oname})
            set(object_name "${CMAKE_CURRENT_BINARY_DIR}/${name}-${targ}/${oname_mangled}.o")
            get_filename_component(object_path ${object_name} PATH)
            file(MAKE_DIRECTORY ${object_path})

            # Getting build type flags
            if("X${CMAKE_BUILD_TYPE}" STREQUAL "XDebug")
                set(build_type_flags_str "${CMAKE_${src_lang}_FLAGS_DEBUG}")
            elseif("X${CMAKE_BUILD_TYPE}" STREQUAL "XRelease")
                set(build_type_flags_str "${CMAKE_${src_lang}_FLAGS_RELEASE}")
            endif()
            string(REPLACE " " ";" build_type_flags "${build_type_flags_str}")

            # Removing conflicting options frm build_type_flags
            set(oflags -O0 -O1 -O2 -O3)
            foreach(oflag ${oflags})
                list(FIND target_flags ${oflag} res)
                if(NOT ${res} EQUAL -1)
                    list(REMOVE_ITEM build_type_flags ${oflags})
                    break()
                endif()
            endforeach()          

            add_custom_command(OUTPUT ${object_name}
                               COMMAND ${path64_compiler_${src_lang}} -c -o ${object_name}
                                       ${arch_flag}
                                       ${src_flags}
                                       ${incl_dirs_flags}
                                       ${target_flags}
                                       ${compile_defs_flags}
                                       ${src_compile_defs_flags}
                                       ${target_compile_defs}
                                       ${lang_flags}
                                       ${src}
                                       ${build_type_flags}
                               DEPENDS ${src} ${header_deps})
            list(APPEND objects ${object_name})
            list(FIND compiler-deps "compiler-stage-${src_lang}" res)
            if(res EQUAL -1)
                list(APPEND compiler-deps "compiler-stage-${src_lang}")
            endif()
        endif()
    endforeach()

    if(path64_multitarget_property_${name}_OUTPUT_NAME)
        set(oname ${path64_multitarget_property_${name}_OUTPUT_NAME})
    else()
        set(oname ${name})
    endif()

    # Adding rule for linking
    if("X${type}" STREQUAL "XSTATIC")
        set(library_file
            "${build_lib_dir}/${CMAKE_STATIC_LIBRARY_PREFIX}${oname}${CMAKE_STATIC_LIBRARY_SUFFIX}")
        add_custom_command(OUTPUT ${library_file}
                           COMMAND ${CMAKE_AR} -cr ${library_file} ${objects}
                           DEPENDS ${objects})
    elseif("X${type}" STREQUAL "XSHARED")
        string(REPLACE " " ";" target_link_flags "${path64_multitarget_property_${name}_LINK_FLAGS}")
        set(library_file
            "${build_lib_dir}/${CMAKE_SHARED_LIBRARY_PREFIX}${oname}${CMAKE_SHARED_LIBRARY_SUFFIX}")

        set(link_libs_flags)
        foreach(lib ${path64_multitarget_link_libraries_${name}})
            list(APPEND link_libs_flags "-l${lib}")
        endforeach()

        if(path64_multitarget_property_${name}_LINKER_LANGUAGE)
            set(link_lang ${path64_multitarget_property_${name}_LINKER_LANGUAGE})
        else()
            set(link_lang ${last_src_lang})
        endif()

        list(FIND compiler-deps "compiler-stage-${link_lang}" res)
        if(res EQUAL -1)
            list(APPEND compiler-deps "compiler-stage-${link_lang}")
        endif()

        add_custom_command(OUTPUT ${library_file}
                           COMMAND ${path64_compiler_${link_lang}} -shared -o ${library_file}
                                   ${arch_flag}
                                   ${target_flags}
                                   ${target_link_flags}
                                   ${objects}
                                   ${link_libs_flags}
                           DEPENDS ${objects})
    else()
        message(FATAL_ERROR "Unknown library type: ${type}")
    endif()

    add_custom_target(${name}-${targ} ALL
                      DEPENDS ${library_file})
    add_dependencies(${name}-${targ} ${compiler-deps})

    install(FILES ${library_file}
            DESTINATION ${install_lib_dir})

#    install(TARGETS ${name}
#      LIBRARY DESTINATION ${install_lib_dir}
#      ARCHIVE DESTINATION ${install_lib_dir})

endfunction()


# Adds library for specified architecture
function(path64_add_library_for_arch name arch type)
    path64_check_arch_exists(${arch})

    # Compiler ABI.
    set(arch_flags ${_PATH64_ARCH_FLAGS_${arch}})
    set(build_lib_dir ${Path64_BINARY_DIR}/lib/${arch})

    # Replacing @ARCH@ with architecture  name in source names
    set(sources ${ARGN})
    list_string_replace_arch(sources ${arch})

    add_library (${name} ${type} ${sources})
    set_property(TARGET ${name} PROPERTY COMPILE_FLAGS ${arch_flags})
    set_property(TARGET ${name} PROPERTY LINK_FLAGS ${arch_flags})

    set_property(TARGET ${name} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${build_lib_dir})
    set_property(TARGET ${name} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${build_lib_dir})

endfunction()


# Adds executable for specified architecture
function(path64_add_executable_for_arch name arch)
    path64_check_arch_exists(${arch})

    # Compiler ABI.
    set(arch_flags ${_PATH64_ARCH_FLAGS_${arch}})
    set(build_lib_dir ${Path64_BINARY_DIR}/lib/${arch})

    # Replacing @ARCH@ with architecture  name in source names
    set(sources ${ARGN})
    list_string_replace_arch(sources ${arch})

    add_executable(${name} ${sources})
    set_property(TARGET ${name} PROPERTY COMPILE_FLAGS ${arch_flags})
    set_property(TARGET ${name} PROPERTY LINK_FLAGS ${arch_flags})

    set_property(TARGET ${name} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${build_lib_dir})

endfunction()


# Adds library for all enabled targets
function(path64_add_multitarget_library name type)
    set(src_list_name path64_multitarget_sources_${name})
    foreach(targ ${PATH64_ENABLE_TARGETS})
        if(${src_list_name}_${targ})
            path64_add_library_for_target(${name} ${targ} ${type} ${${src_list_name}_${targ}})
        else()
            path64_add_library_for_target(${name} ${targ} ${type} ${${src_list_name}_COMMON})
        endif()
        #set_property(TARGET ${tg_name} PROPERTY OUTPUT_NAME ${name})
    endforeach()
endfunction()


# Adds library for all enabled architectures
function(path64_add_multiarch_library name type src_list_name)
    foreach(arch ${PATH64_ENABLE_ARCHES})
        path64_get_multiarch_cmake_target(tg_name ${name} ${arch})
        if(${src_list_name}_${arch})
            path64_add_library_for_arch(${tg_name} ${arch} ${type} ${${src_list_name}_${arch}})
        else()
            path64_add_library_for_arch(${tg_name} ${arch} ${type} ${${src_list_name}_COMMON})
        endif()
        set_property(TARGET ${tg_name} PROPERTY OUTPUT_NAME ${name})
    endforeach()
endfunction()


# Adds executable for all enabled architectures
function(path64_add_multiarch_executable name src_list_name)
    foreach(arch ${PATH64_ENABLE_ARCHES})
        path64_get_multiarch_cmake_target(tg_name ${name} ${arch})
        if(${src_list_name}_${arch})
            path64_add_executable_for_arch(${tg_name} ${arch} ${${src_list_name}_${arch}})
        else()
            path64_add_executable_for_arch(${tg_name} ${arch} ${${src_list_name}_COMMON})
        endif()
        set_property(TARGET ${tg_name} PROPERTY OUTPUT_NAME ${name})
    endforeach()
endfunction()


# Adds rules for installing multiarch target
function(path64_install_multiarch name)
    foreach(arch ${PATH64_ENABLE_ARCHES})
        path64_get_multiarch_cmake_target(tg_name ${name} ${arch})
        set(install_lib_dir ${PATH64_LIB_PATH}/${arch})
        install(TARGETS ${tg_name}
                LIBRARY DESTINATION ${install_lib_dir}
                ARCHIVE DESTINATION ${install_lib_dir}
                RUNTIME DESTINATION ${install_lib_dir})
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

    if(prop_vals)
        set_property(TARGET ${name} PROPERTY ${prop} ${prop_vals})
    else()
        set_property(TARGET ${name} PROPERTY ${prop} "")
    endif()
endfunction()


# Sets property for multitarget
function(path64_set_multitarget_property_ name prop)
    foreach(targ ${PATH64_ENABLE_TARGETS})
        set(path64_multitarget_property_${name}_${prop} ${ARGN} PARENT_SCOPE)
    endforeach()
endfunction()


# Sets target specific property for multitarget
function(path64_set_multitarget_property_for_target name targ prop)
    set(path64_multitarget_property_${name}_${targ}_${prop} ${ARGN} PARENT_SCOPE)
endfunction()


# Sets property for multiarch target
function(path64_set_multiarch_property name prop)
    foreach(arch ${PATH64_ENABLE_ARCHES})
        path64_get_multiarch_cmake_target(tg ${name} ${arch})
        path64_set_property_for_arch(${tg} ${arch} ${prop} ${ARGN})
    endforeach()
endfunction()


# Adds link libraries to multitarget
function(path64_multitarget_link_libraries name)
    foreach(targ ${PATH64_ENABLE_TARGETS})
        set(path64_multitarget_link_libraries_${name} ${ARGN} PARENT_SCOPE)
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


# Adds dependencies from multiarch targets
function(path64_add_dependencies_from_multiarch name)
    foreach(dep ${ARGN})
        foreach(arch ${PATH64_ENABLE_ARCHES})
            path64_get_multiarch_cmake_target(dep_tg ${dep} ${arch})
            add_dependencies(${name} ${dep_tg})
        endforeach()
    endforeach()
endfunction()


# Adds dependencies for multitarget from multitarget
function(path64_add_multitarget_dependencies name)
    foreach(targ ${PATH64_ENABLE_TARGETS})
        foreach(dep ${ARGN})
            add_dependencies(${name}-${targ} ${dep}-${targ})
        endforeach()
    endforeach()
endfunction()


# Adds dependencies from multitarget
function(path64_add_dependencies_from_multitarget name)
    foreach(targ ${PATH64_ENABLE_TARGETS})
        foreach(dep ${ARGN})
       add_dependencies(${name} ${dep}-${targ})
        endforeach()
    endforeach()
endfunction()

