project(libmv)

SET(PATHSCALE_MV_SRCS ../libmpath/dummy.c)

SET(PATHSCALE_MV_SRCS_X64
                "../libmpath/amd/vrdacos.S"
				"../libmpath/amd/vrdaexp.s"
				"../libmpath/amd/vrdalog.s"
				"../libmpath/amd/vrdalog10.s"
				"../libmpath/amd/vrdasin.S"
				"../libmpath/amd/vrdasincos.S"
				"../libmpath/amd/vrsaexpf.s"
				"../libmpath/amd/vrsalogf.s"
				"../libmpath/amd/vrd2cos.S"
				"../libmpath/amd/vrd2cosh.S"
				"../libmpath/amd/vrd2exp.s"
				"../libmpath/amd/vrd2log.s"
				"../libmpath/amd/vrd2log10.s"
				"../libmpath/amd/vrd2pow.S"
				"../libmpath/amd/vrd4pow.S"
				"../libmpath/amd/vrd2sin.S"
				"../libmpath/amd/vrd2sincos.S"
				"../libmpath/amd/vrd2sinh.S"
				"../libmpath/amd/vrd4cos.S"
				"../libmpath/amd/vrd4exp.s"
				"../libmpath/amd/vrd4log.s"
				"../libmpath/amd/vrd4log10.s"
				"../libmpath/amd/vrd4sin.S"
				"../libmpath/amd/vrs4cosf.S"
				"../libmpath/amd/vrs4expf.s"
				"../libmpath/amd/vrs4log10f.S"
				"../libmpath/amd/vrs4logf.s"
				"../libmpath/amd/vrs4powf.S"
				"../libmpath/amd/vrs4sinf.S"
				"../libmpath/amd/vrs8expf.s"
				"../libmpath/amd/vrs8logf.s"
				"../libmpath/amd/vrs8powf.S"
				"../libmpath/vacos.c"
				"../libmpath/vacosf.c"
				"../libmpath/vasin.c"
				"../libmpath/vasinf.c"
				"../libmpath/vatan.c"
				"../libmpath/vatanf.c"
				"../libmpath/vcis.c"
				"../libmpath/vcisf.c"
				"../libmpath/vcos.c"
				"../libmpath/vcosf.c"
				"../libmpath/vexp.c"
				"../libmpath/vexpf.c"
				"../libmpath/vlog.c"
				"../libmpath/vlog10.c"
				"../libmpath/vlog10f.c"
				"../libmpath/vlogf.c"
				"../libmpath/vsin.c"
				"../libmpath/vsinf.c"
				"../libmpath/vsqrt.c"
				"../libmpath/vsqrtf.c"
				"../libmpath/vtan.c"
				"../libmpath/vtanf.c"
				"../libmpath/amd/vrdapow.c"
				"../libmpath/amd/vrsapowf.c")


INCLUDE_DIRECTORIES(
	../libmpath/amd
	../libm
)

# Should always build with a higher level of optimization
set(compile_flags "-O3 -LNO:vintr=0 -std=gnu99")

IF(PATH64_ENABLE_MATHLIBS)

set_source_files_properties(${PATHSCALE_MV_SRCS} PROPERTIES LANGUAGE C)

path64_set_multitarget_sources(mv-static COMMON ${PATHSCALE_MV_SRCS})
path64_set_multitarget_sources(mv-static x86_64 ${PATHSCALE_MV_SRCS_X64})
path64_set_multitarget_property_(mv-static OUTPUT_NAME "mv")
path64_set_multitarget_property_(mv-static COMPILE_FLAGS "${compile_flags} -fPIC -DPIC")
path64_add_multitarget_library(mv-static STATIC)
path64_add_multitarget_multitarget_dependencies(mv-static pscrt-static)
path64_add_dependencies_from_multitarget(compiler-stage-libs-C mv-static)


path64_set_multitarget_sources(mv COMMON ${PATHSCALE_MV_SRCS})
path64_set_multitarget_sources(mv x86_64 ${PATHSCALE_MV_SRCS_X64})
path64_set_multitarget_property_(mv COMPILE_FLAGS "${compile_flags} -fPIC -DPIC")
path64_set_multitarget_property_(mv SOVERSION 1)

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
    path64_set_multitarget_property_(mv LINK_FLAGS
            "-Wl,-x -Wl,--version-script=${libmv_SOURCE_DIR}/libmv.vs -fbootstrap-hack -nodefaultlibs")
else()
    path64_set_multitarget_property_(mv LINK_FLAGS "-fbootstrap-hack -nodefaultlibs")
endif()

path64_multitarget_link_libraries(mv m c)

path64_add_multitarget_library(mv SHARED)
path64_add_multitarget_multitarget_dependencies(mv pscrt-static)
if(PATH64_ENABLE_PSCRUNTIME)
    path64_add_multitarget_multitarget_dependencies(mv compiler-rt)
endif()

path64_add_dependencies_from_multitarget(compiler-stage-libs-C mv)


if(PATH64_ENABLE_PROFILING)
    path64_set_multitarget_sources(mv_p COMMON ${PATHSCALE_MV_SRCS})
    path64_set_multitarget_sources(mv_p x86_64 ${PATHSCALE_MV_SRCS_X64})
    if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
        path64_set_multitarget_property_(mv_p COMPILE_FLAGS "-pg ${compile_flags}")
    else()
        path64_set_multitarget_property_(mv_p COMPILE_FLAGS "${compile_flags}")
    endif()
    path64_add_multitarget_library(mv_p STATIC)
    path64_add_dependencies_from_multitarget(compiler-stage-libs-C mv_p)
endif(PATH64_ENABLE_PROFILING)

ENDIF(PATH64_ENABLE_MATHLIBS)
