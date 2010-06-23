# Looking for gcc runtime search path
SET(SOURCE_FILE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/rt.cc")

FILE(WRITE "${SOURCE_FILE}" "#include <iostream>
int main(void)
{
        std::cout << \"\";
        return 0;
}
")

MESSAGE(STATUS "Checking for gcc runtime search path")
TRY_COMPILE(CXX_RUNTIME_TESTED
    ${CMAKE_BINARY_DIR} ${SOURCE_FILE}
    CMAKE_FLAGS -DLINK_LIBRARIES:STRING=-v
    OUTPUT_VARIABLE OUTPUT)
FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
    "Checking for gcc runtime search path:\n"
    "${OUTPUT}\n")

STRING(REGEX MATCHALL "collect2.*$"
                COLLECT2_COMMAND ${OUTPUT})
FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
    "collect2 command:\n"
    "${COLLECT2_COMMAND}\n")

STRING(REGEX MATCHALL "-L\([^ ]*\)" LIBS ${COLLECT2_COMMAND})

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

