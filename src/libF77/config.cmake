
include(CheckFunctionExists)

set(CMAKE_REQUIRED_INCLUDES "math.h")
set(CMAKE_REQUIRED_LIBRARIES "m")

check_function_exists(sincos HAVE_SINCOS)
check_function_exists(sincosf HAVE_SINCOSF)

set(CMAKE_REQUIRED_INCLUDES)
set(CMAKE_REQUIRED_LIBRARIES)

configure_file(${Path64_SOURCE_DIR}/src/libF77/f77_config.h.cmake.in
               ${Path64_BINARY_DIR}/src/include/f77_config.h)

