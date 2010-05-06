
#=============================================================================
# Copyright 2009 Kitware, Inc.
# Copyright (C) 2010 PathScale Inc. All rights reserved.
#
# Distributed under the OSI-approved BSD License (the "License").
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the names of Kitware, Inc., the Insight Software Consortium,
#   nor the names of their contributors may be used to endorse or promote
#   products derived from this software without specific prior written
#   permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================
# This must be changed since cmake doesn't automatically detect pathf90.
#if(NOT DEFINED REAL_Fortran_COMPILER)
  set(_desc "Looking for a Fortran compiler")
  message(STATUS ${_desc})
  file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/CheckFortran)
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/CheckFortran/CMakeLists.txt"
    "cmake_minimum_required(VERSION 2.8.1)
project(CheckFortran Fortran)
file(WRITE \"\${CMAKE_CURRENT_BINARY_DIR}/result.cmake\"
  \"set(REAL_Fortran_COMPILER \\\"\${CMAKE_Fortran_COMPILER}\\\")\\n\"
  \"set(CMAKE_Fortran_FLAGS \\\"\${CMAKE_Fortran_FLAGS}\\\")\\n\"
  )
")
  execute_process(
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/CheckFortran
    COMMAND ${CMAKE_COMMAND} . -G ${CMAKE_GENERATOR}
    OUTPUT_VARIABLE output
    ERROR_VARIABLE output
    RESULT_VARIABLE result
    )
  include(${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/CheckFortran/result.cmake OPTIONAL)
  if(REAL_Fortran_COMPILER AND "${result}" STREQUAL "0")
    GET_FILENAME_COMPONENT(BASE_COMPILER ${CMAKE_Fortran_COMPILER} NAME)
    IF("${CMAKE_Fortran_COMPILER_ID}" MATCHES "GNU")
	SET(MFEF95_TMP_FORTRAN_LINK_LIB "gfortran")
        SET(REAL_Fortran_COMPILER_FLAGS "-DBUILD_COMPILER_GNU -fcray-pointer -fsecond-underscore -m96bit-long-double")
    ELSEIF("${BASE_COMPILER}" MATCHES "pathf90")
        SET(REAL_Fortran_COMPILER_FLAGS " -Wf,-eQ ")
	SET(MFEF95_TMP_FORTRAN_LINK_LIB "-lpathfortran")
    ELSE()
        MESSAGE(FATAL_ERROR "Unsupported Fortran compiler found.  We support PathScale and GNU Fortran for bootstrap at this time.")
    ENDIF()

    file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
      "${_desc} passed with the following output:\n"
      "${output}\n")
    set(CMAKE_Fortran_COMPILER "${Path64_BINARY_DIR}/fc-wrap.sh" CACHE FILEPATH "Overriding Fortran compiler" FORCE)
    file(WRITE "${Path64_BINARY_DIR}/fc-wrap.sh"
    "#!/bin/sh
${REAL_Fortran_COMPILER} ${REAL_Fortran_COMPILER_FLAGS} \"\$@\"
")
    execute_process(
	COMMAND chmod 700 "${Path64_BINARY_DIR}/fc-wrap.sh"
    )
  else()
    set(CMAKE_Fortran_COMPILER NOTFOUND)
    file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
      "${_desc} failed with the following output:\n"
      "${output}\n")
  endif()
  message(STATUS "${_desc} - ${CMAKE_Fortran_COMPILER}")
  mark_as_advanced(CMAKE_Fortran_COMPILER)
  set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}" CACHE STRING "Fortran flags")
  mark_as_advanced(CMAKE_Fortran_FLAGS)
#endif()
