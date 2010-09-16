# build arith.a and a couple custom commands

SET(PATHSCALE_ARITH_SRCS        "bits.c"
				"compare.c"
				"convert.c"
				"cray_fadd.c"
				"cray_fdiv.c"
				"cray_fmul.c"
				"cvt.c"
				"edition.c"
				"ieee_fadd.c"
				"ieee_fdiv.c"
				"ieee_fmul.c"
				"ieee_norm.c"
				"ieee_sqrt.c"
				"intrin.c"
				"math.c"
				"miscmath.c"
				"strcvt.c"
				"native.c"
				"test_ar_intrin.c"
				"cray1_sim.c"
				"fintrin_dc_irix_tmp.f"
				"fintrin_irix_tmp.f")

INCLUDE_DIRECTORIES(
	.
	../clibinc)

# -DEDITION used to come from src/arith/edition
ADD_DEFINITIONS(
	-D_LITTLE_ENDIAN
	-DEDITION=\"23\")

add_library(arith STATIC ${PATHSCALE_ARITH_SRCS})

add_custom_command(
    OUTPUT arith.msgs
    COMMAND ${CMAKE_C_COMPILER} -xc -E -DKEY -traditional -I. ${CMAKE_CURRENT_SOURCE_DIR}/arith.msg > arith.msgs
)

add_custom_command(
    OUTPUT arith.cat
    COMMAND ${PATHSCALE_SOURCE_DIR}/linux/tools/caterr -c arith.cat arith.msgs
    DEPENDS arith.msgs
)
add_custom_target(
    gen_arith_cat
    DEPENDS arith.cat
)
add_dependencies(arith gen_arith_cat)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/arith.cat DESTINATION ${PATH64_LIB_PATH})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/arith.msgs DESTINATION ${PATH64_LIB_PATH})
