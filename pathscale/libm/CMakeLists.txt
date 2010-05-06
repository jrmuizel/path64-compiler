project(libmv)

SET(PATHSCALE_MV_SRCS   	"../libmpath/amd/vrdacos.S"
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

ADD_DEFINITIONS(
# Should always build with a higher level of optimization
	-O3
	-LNO:vintr=0
	-std=gnu99)

IF(PATH64_ENABLE_MATHLIBS)
set_source_files_properties(${PATHSCALE_MV_SRCS} PROPERTIES LANGUAGE C)
add_library(mv-static STATIC ${PATHSCALE_MV_SRCS})
SET_TARGET_PROPERTIES(mv-static PROPERTIES OUTPUT_NAME "mv")

add_library(mv SHARED ${PATHSCALE_MV_SRCS})
set_property(TARGET mv PROPERTY COMPILE_DEFINITIONS PIC)
set_property(TARGET mv PROPERTY SOVERSION 1)
set_property(TARGET mv PROPERTY LINK_FLAGS "-Wl,-x -Wl,--version-script=${libmv_SOURCE_DIR}/libmv.vs")
target_link_libraries(mv m)

IF(PATH64_ENABLE_PROFILING)
add_library(mv_p STATIC ${PATHSCALE_MV_SRCS})
set_property(TARGET mv_p PROPERTY COMPILE_FLAGS -pg)
install(TARGETS mv_p ARCHIVE DESTINATION ${PATH64_LIB_PATH})
ENDIF(PATH64_ENABLE_PROFILING)

install(TARGETS mv-static mv
        LIBRARY DESTINATION ${PATH64_LIB_PATH}
        ARCHIVE DESTINATION ${PATH64_LIB_PATH})

ENDIF(PATH64_ENABLE_MATHLIBS)
