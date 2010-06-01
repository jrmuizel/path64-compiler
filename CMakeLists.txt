CMAKE_MINIMUM_REQUIRED(VERSION 2.8.1 FATAL_ERROR)
PROJECT(Path64)

# Disallow in-source build
IF("${Path64_SOURCE_DIR}" STREQUAL "${Path64_BINARY_DIR}")
  MESSAGE(FATAL_ERROR "Path64 must be built out-of-source.  Remove files from the source tree that were created by this attempt:
  rm -rf CMakeFiles CMakeCache.txt
Then create a separate build directory and run CMake there.
  mkdir ../Path64-build
  cd ../Path64-build
  cmake ../Path64
This approach keeps the source tree pristine.")
ENDIF()

# Search for various include files and functions
include(${Path64_SOURCE_DIR}/cmake/Modules/ConfigureChecks.cmake)

# Force the build_type=debug flags to sane values overriding users choices
SET(CMAKE_C_FLAGS_DEBUG  "-O0 -g -D_DEBUG" CACHE STRING "Flags used by the compiler during debug builds." FORCE)
SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g -D_DEBUG" CACHE STRING "Flags used by the compiler during debug builds." FORCE)
SET(CMAKE_Fortran_FLAGS_DEBUG  "-O0 -g3 -D_DEBUG" CACHE STRING "Flags used by the compiler during debug builds." FORCE)

# Define user option whether to build the Fortran compiler.
OPTION(PATH64_ENABLE_FORTRAN "Build Fortran compiler" ON)
IF(PATH64_ENABLE_FORTRAN)
  ENABLE_LANGUAGE(Fortran)
  IF(CMAKE_Fortran_COMPILER)
    INCLUDE(${Path64_SOURCE_DIR}/cmake/Modules/CheckFortran.cmake)
  ELSE()
    MESSAGE(STATUS "No working Fortran compiler is available")
    SET(PATH64_ENABLE_FORTRAN 0)
  ENDIF()
ENDIF()

# Define user option whether to build the profiling libraries
OPTION(PATH64_ENABLE_PROFILING "Build profiling libraries" ON)
IF(PATH64_ENABLE_PROFILING)
	MESSAGE(STATUS "Profiling build enabled")
ELSE()
	MESSAGE(STATUS "Profiling build disabled")
ENDIF()

# Define user option whether to build OpenMP
OPTION(PATH64_ENABLE_OPENMP "Build OpenMP" ON)
IF(PATH64_ENABLE_OPENMP)
	MESSAGE(STATUS "OpenMP build enabled")
ELSE()
	MESSAGE(STATUS "OpenMP build disabled")
ENDIF()

# Define user option whether to build both 32/64bit libraries
OPTION(PATH64_ENABLE_MULTILIB "Multilib build" OFF)
IF(PATH64_ENABLE_MULTILIB)
	MESSAGE(STATUS "Multilib not supported yet, but enabled")
#ELSE()
#	MESSAGE(STATUS "Multilib not supported yet disabled")
ENDIF()

# Define user option whether to build the profiling libraries
# Which targets are we going to build.  Currently supported is x8664,mips
# We may want to dodge multilib and just treat x86 as it's own target.
# Then we could test is for the the native build environment is and make that the default
SET(PATH64_ENABLE_TARGETS "" CACHE STRING "Target archs (x8664;mips)")
IF(PATH64_ENABLE_TARGETS)

	foreach(arg ${PATH64_ENABLE_TARGETS})
		set(targetsX "${targetsX} ${arg}")
	endforeach(arg ${PATH64_ENABLE_TARGETS})
	MESSAGE(STATUS "We are going to build the following targets${targetsX}")
ELSE()
	MESSAGE( FATAL_ERROR "Must specify at least one target.
Supported targets are : x8664 and mips
Please add -DPATH64_ENABLE_TARGETS=x8664 to your cmake ../Path64 line
Multiple targets can be specified like \"-DPATH64_ENABLE_TARGETS=x8664;mips\"" )
ENDIF()
# Multi target support is indefinitely delayed
set(BUILD_TARGET_DIR "x8664")

# Define if we are doing a supported build
OPTION(PATH64_ENABLE_SUPPORTED "Enabling supported build" OFF)
IF(PATH64_ENABLE_SUPPORTED)
	MESSAGE(FATAL_ERROR "This source tree is still not officially supported yet")
ELSE()
	MESSAGE(STATUS "Community supported version.  For officially supported version please visit http://www.pathscale.com")
ENDIF()

# Valgrind?
OPTION(PATH64_ENABLE_VALGRIND "Enabling valgrind" OFF)
IF(PATH64_ENABLE_VALGRIND)
	MESSAGE(STATUS "Valgrind enabled")
ELSE()
	MESSAGE(STATUS "Valgrind disabled")
ENDIF()

# Whatever arbitrary number you'd like in 3 string format eg 3.9.99
SET(PSC_FULL_VERSION "3.9.99" CACHE STRING "Defaulting to version 3.9.99")
IF(PSC_FULL_VERSION)
	MESSAGE(STATUS "Version is set to "${PSC_FULL_VERSION})
	string(REGEX REPLACE "^([0-9.]+).*$" "\\1" TMPVERSION ${PSC_FULL_VERSION})
	# If it's not in three string format it sucks to be you
	string(REGEX REPLACE "^([0-9]+).+$" "\\1" PSC_MAJOR_VERSION ${TMPVERSION})
	string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*$" "\\1" PSC_MINOR_VERSION ${TMPVERSION})
ELSE()
	MESSAGE(FATAL_ERROR "Must set PSC_FULL_VERSION")
ENDIF()

# TODO : This should use FIND_FILE and be more logical when it comes to cross compiles and multilib
IF(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
SET(PSC_CRT_PATH "/usr/lib/amd64" CACHE STRING "Location of crt1.o crti.o and crtn.o")
ELSE()
SET(PSC_CRT_PATH "/usr/lib" CACHE STRING "Location of crt1.o crti.o and crtn.o")
ENDIF()

IF(PSC_CRT_PATH)
	MESSAGE(STATUS "PSC_CRT_PATH is set to "${PSC_CRT_PATH})
ELSE()
	MESSAGE(FATAL_ERROR "Must set PSC_CRT_PATH")
ENDIF()

# Do we build with huge pages support
OPTION(PATH64_ENABLE_HUGEPAGES "Enabling supported huge pages" OFF)
IF(PATH64_ENABLE_HUGEPAGES)
	IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		MESSAGE(STATUS "Building with huge pages support.")
	ELSE()
		MESSAGE(FATAL_ERROR "Huge pages not supported on this platform yet.  Please disable this option or contact PathScale for adding support")
	ENDIF()
ELSE()
	MESSAGE(STATUS "Not building with huge pages support")
ENDIF()

# Do we build the optimized math libraries
OPTION(PATH64_ENABLE_MATHLIBS "Building optimized math libraries" OFF)
IF(PATH64_ENABLE_MATHLIBS)
	MESSAGE(STATUS "Optimized math libraries enabled")
ELSE()
	MESSAGE(STATUS "Optimized math libraries disabled")
ENDIF()

# Are we using the PathScale runtime
OPTION(PATH64_ENABLE_PSCRUNTIME "Using the PathScale c and c++ runtime" OFF)
IF(PATH64_ENABLE_PSCRUNTIME)
	MESSAGE(STATUS "PathScale runtime enabled")
ELSE()
	MESSAGE(STATUS "GNU runtime enabled")
ENDIF()

SET(NAMEPREFIX "path" CACHE STRING "Default name excutable prefix [path]")
IF(NAMEPREFIX)
	MESSAGE(STATUS "Executables will be prefixed with ${NAMEPREFIX}")
ELSE()
	MESSAGE( FATAL_ERROR "The executable name prefix must not be empty." )
ENDIF()


# Set default install prefix (instead of CMake's /usr/local default).
# TODO : How do we fix this so cmake -i shows the correct default prefix?
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX "/opt/path64" CACHE PATH "Installation root prefix" FORCE)
ENDIF()

# On Linux some things needs to link with -ldl
# but on FBSD they should not.
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(libdl "dl")
ELSE()
    set(libdl "")
ENDIF()


# Selecting macro for enabling int types formats for C++
SET(PATH64_STDC_FORMAT_MACROS -D__STDC_FORMAT_MACROS)

# Forching build for 64-bit on osol
IF(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
SET(CMAKE_CXX_FLAGS "-m64")
SET(CMAKE_C_FLAGS "-m64")
SET(CMAKE_Fortran_FLAGS "-m64")
SET(CMAKE_EXE_LINKER_FLAGS "-m64")
SET(CMAKE_MODULE_LINKER_FLAGS "-m64")
SET(CMAKE_SHARED_LINKER_FLAGS "-m64")
ENDIF()


# Tell GCC_XML that we are providing gccxml_cc1plus for it.
SET(GCCXML_NATIVE_CC1PLUS 1)

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/Modules/GCCVersion.cmake)


# Place all targets near the top of the build tree by default.
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${Path64_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${Path64_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${Path64_BINARY_DIR}/lib)

SET(PATH64_LIB_PATH lib/${PSC_FULL_VERSION})

# Tell GCC where to install relative to the CMAKE_INSTALL_PREFIX.
SET(PATH64_GCC_INSTALL_PATH ${PATH64_LIB_PATH})


ADD_DEFINITIONS(-D_SGI_SOURCE
                -D__GNU_BUG_WORKAROUND
                -DTARG_X8664
                -DKEY
                -DFE_GNU_4_2_0
                -DLONGLONG
                -D_MIPSEL
                -DTARG_LINUX
                -D_MIPS_SZINT=32
                -D_MIPS_SZPTR=32
                -D_MIPS_SZLONG=32
               )

# Build the actual ld line to be used in driver

execute_process(COMMAND "ld --help 2>&1 | grep hash-style > /dev/null"
 RESULT_VARIABLE LD_NOT_SUPPORTS_HASH_STYLE)

if(${LD_NOT_SUPPORTS_HASH_STYLE})
SET(LD_HASH_STYLE_FLAG "")
else()
SET(LD_HASH_STYLE_FLAG "--hash-style=gnu")
endif()


# Build GCC and pathscale.
ADD_SUBDIRECTORY(GCC)
ADD_SUBDIRECTORY(pathscale)
