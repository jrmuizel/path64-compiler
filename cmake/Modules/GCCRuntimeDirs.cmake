# Looking for gcc runtime search path
SET(SOURCE_FILE "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/GetGccRuntimeDir/rt.cc")

MESSAGE(STATUS "Checking for gcc runtime search path")
file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/GetGccRuntimeDir)
FILE(WRITE "${SOURCE_FILE}" "#include <iostream>
int main(void)
{
        std::cout << \"\";
        return 0;
}
")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/GetGccRuntimeDir/CMakeLists.txt"
    "cmake_minimum_required(VERSION 2.8.1)
project(GetGccRuntimeDir CXX)

TRY_RUN(RUN_RES COMP_RES
    \${CMAKE_BINARY_DIR} \"${SOURCE_FILE}\"
    CMAKE_FLAGS
	\"-DLINK_LIBRARIES:STRING=-v\"
    COMPILE_OUTPUT_VARIABLE COMP_OUT
    RUN_OUTPUT_VARIABLE RUN_OUT)
FILE(WRITE \"\${CMAKE_CURRENT_BINARY_DIR}/result.cmake\"
    \"set(GCCRTDATA \\\"\${COMP_OUT}\\\")\\n\")
")

execute_process(
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/GetGccRuntimeDir
    COMMAND env CXX=g++ ${CMAKE_COMMAND} . -G ${CMAKE_GENERATOR}
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
    RESULT_VARIABLE result
)

message(STATUS "execute output")
message(STATUS "${output}")

include(${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/GetGccRuntimeDir/result.cmake OPTIONAL)

if ("${result}" STREQUAL "0")
    STRING(REGEX MATCHALL "collect2.*$"
		    COLLECT2_COMMAND "${GCCRTDATA}")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
	"collect2 command:\n"
	"${COLLECT2_COMMAND}\n")

    STRING(REGEX MATCHALL "-L\([^ ]*\)" LIBS "${COLLECT2_COMMAND}")

    FOREACH(libdir ${LIBS})
	STRING(REPLACE "-L" "" stripped_dir ${libdir})
	SET(stripped_dirs_list ${stripped_dirs_list} ${stripped_dir})
    ENDFOREACH()

    FOREACH(stripped_dir ${stripped_dirs_list})
	IF(EXISTS "${stripped_dir}/libsupc++.a")
	    SET(CONFIGURED_LIBSUPCXX_DIR ${stripped_dir})
	ENDIF()
	IF(EXISTS "${stripped_dir}/libstdc++.a")
	    SET(CONFIGURED_LIBSTDCXX_DIR ${stripped_dir})
	ENDIF()
	IF(EXISTS "${stripped_dir}/libgcc.a")
	    SET(CONFIGURED_LIBGCC_DIR ${stripped_dir})
	ENDIF()
	IF(EXISTS "${stripped_dir}/libgcc_eh.a")
	    SET(CONFIGURED_LIBGCC_EH_DIR ${stripped_dir})
	ENDIF()
	IF(EXISTS "${stripped_dir}/libgcc_s.so")
	    SET(CONFIGURED_LIBGCC_S_DIR ${stripped_dir})
	ENDIF()
    ENDFOREACH()
endif()
