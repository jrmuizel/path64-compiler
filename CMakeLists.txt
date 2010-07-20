CMAKE_MINIMUM_REQUIRED(VERSION 2.8.1 FATAL_ERROR)
PROJECT(Path64)

ENABLE_TESTING()

# Disallow in-source build
IF("${Path64_SOURCE_DIR}" STREQUAL "${Path64_BINARY_DIR}")
  MESSAGE(FATAL_ERROR "Project must be built out-of-source.  Remove files from the source tree that were created by this attempt:
  rm -rf CMakeFiles CMakeCache.txt
Then create a separate build directory and run CMake there.
  mkdir ../build
  cd ../build
  cmake ../
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

#-----------------------------------------------------------------------------
# Target architecture table.
include(${Path64_SOURCE_DIR}/src/targets.cmake)

# Define user option whether to build the profiling libraries
# Which targets are we going to build.  Currently supported is x8664,mips
# We may want to dodge multilib and just treat x86 as it's own target.
# Then we could test is for the the native build environment is and make that the default
set(PATH64_ENABLE_TARGETS "" CACHE STRING "Target archs (x8664_32;mips_64)")
if(PATH64_ENABLE_TARGETS)
  set(_targets "")
  set(_sep "")
  foreach(arch ${PATH64_ENABLE_TARGETS})
    # Look for a target table entry for this architectures.
    if(NOT _PATH64_TARGET_ARCH_${arch})
      message(FATAL_ERROR "'${arch}' is not among supported architectures.
${_PATH64_SUPPORTED_TARGETS_STRING}
Please edit PATH64_ENABLE_TARGETS to list only valid architectures.
")
    endif()
    set(_targets "${_targets}${_sep}${arch}")
    set(_sep ", ")
  endforeach(arch)
  # First list element is the default.
  list(GET PATH64_ENABLE_TARGETS 0 PATH64_DEFAULT_TARGET)
  message(STATUS
    "Building support for targets ${_targets}.  Default is ${PATH64_DEFAULT_TARGET}.")
else()
  message( FATAL_ERROR "Must specify at least one target architecture.
${_PATH64_SUPPORTED_TARGETS_STRING}
Please add -DPATH64_ENABLE_TARGETS=x8664_64 to your cmake ../Path64 line
Multiple targets can be specified with
  \"-DPATH64_ENABLE_TARGETS=x8664_32;mips_64\"
on the command line.")
endif()

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

IF(NOT PSC_CRT_PATH)
	SET(PSC_CRT_PATH "/usr/lib")
	MESSAGE(FATAL_ERROR "Must set PSC_CRT_PATH")
ENDIF()
MESSAGE(STATUS "PSC_CRT_PATH is set to "${PSC_CRT_PATH})

# Do we build with huge pages support
OPTION(PATH64_ENABLE_HUGEPAGES "Enabling supported huge pages" OFF)
IF(PATH64_ENABLE_HUGEPAGES)
	IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		MESSAGE(STATUS "Building with huge pages support.")
	ELSE()
		MESSAGE(FATAL_ERROR "Huge pages not supported on this platform yet.  Please disable this option or contact PathScale for support")
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

# Use X86 elf file format for whirl objects
OPTION(X86_WHIRL_OBJECTS "Use x86 elf file format for whirl object files" OFF)
IF(X86_WHIRL_OBJECTS)
        MESSAGE(STATUS "Using x86 elf file format for whirl object files")
        ADD_DEFINITIONS(-DX86_WHIRL_OBJECTS)
ELSE()
        MESSAGE(STATUS "Using MIPS elf file format for whirl object files")
ENDIF()

# Fuse WHIRL and native code in IPA objects
OPTION(FAT_WHIRL_OBJECTS " Fuse WHIRL and native code in IPA objects" OFF)
IF(FAT_WHIRL_OBJECTS)
        MESSAGE(STATUS "Configuring for fat IPA objects")
        ADD_DEFINITIONS(-DFAT_WHIRL_OBJECTS)
ELSE()
        MESSAGE(STATUS "Configuring for simple WHIRL IPA objects")
ENDIF()


# Are we using the PathScale runtime
OPTION(PATH64_ENABLE_PSCRUNTIME "Using the PathScale c and c++ runtime" OFF)
IF(PATH64_ENABLE_PSCRUNTIME)
	ADD_DEFINITIONS(-DPATH64_ENABLE_PSCRUNTIME)
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

OPTION(PATH64_BUILD_32 "Build 32-bit version" OFF)

IF(PATH64_BUILD_32)
    SET(CMAKE_CXX_FLAGS "-m32")
    SET(CMAKE_C_FLAGS "-m32")
    SET(CMAKE_Fortran_FLAGS "-m32")
    SET(CMAKE_EXE_LINKER_FLAGS "-m32")
    SET(CMAKE_MODULE_LINKER_FLAGS "-m32")
    SET(CMAKE_SHARED_LINKER_FLAGS "-m32")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
# Forching build for 64-bit on osol
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
#                -DTARG_X8664
                -DKEY
                -DFE_GNU_4_2_0
                -D_LONGLONG
                -D_MIPSEL
                -DTARG_LINUX
                -D_MIPS_SZINT=32
                -D_MIPS_SZPTR=32
                -D_MIPS_SZLONG=32
               )

# GNU-style hashing needs support of the dynamic linker, so white-list
# potential platforms.
IF (NOT DEFINED SUPPORT_HASH_STYLE_GNU)
    SET(SOURCE_FILE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/hash-style.c")

    FILE(WRITE "${SOURCE_FILE}" "#include <stdio.h>
int main(void)
{
	return printf(\"\");
}
")

    MESSAGE(STATUS "Checking for --hash-style=gnu support")
    TRY_RUN(SUPPORT_HASH_STYLE_GNU SUPPORT_HASH_STYLE_GNU_BUILD
	${CMAKE_BINARY_DIR} ${SOURCE_FILE}
        CMAKE_FLAGS -DLINK_LIBRARIES:STRING=-Wl,--hash-style=gnu
	OUTPUT_VARIABLE OUTPUT)
    IF (SUPPORT_HASH_STYLE_GNU_BUILD AND SUPPORT_HASH_STYLE_GNU EQUAL 0)
	MESSAGE(STATUS "Checking for --hash-style=gnu support -- found ")
    ELSE()
	MESSAGE(STATUS "Checking for --hash-style=gnu support -- not found ")
    	FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
    	    "Checking support for --hash-style=gnu support failed with the following output:\n"
    	    "${OUTPUT}\n")
    ENDIF()
ENDIF()

IF (SUPPORT_HASH_STYLE_GNU_BUILD AND SUPPORT_HASH_STYLE_GNU EQUAL 0)
    SET(LD_HASH_STYLE_FLAG "--hash-style=gnu")
ELSE()
    SET(LD_HASH_STYLE_FLAG "")
ENDIF()

# build runtime search paths
include(${Path64_SOURCE_DIR}/cmake/Modules/GCCRuntimeDirs.cmake)

IF (PATH64_DEFAULT_ABI)
    ADD_DEFINITIONS(-DPATH64_DEFAULT_ABI=${PATH64_DEFAULT_ABI})
ENDIF()

# Build GCC and pathscale.
ADD_SUBDIRECTORY(GCC)
ADD_SUBDIRECTORY(src)

ADD_SUBDIRECTORY(bootstrap)

