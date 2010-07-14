INCLUDE_DIRECTORIES(
  ${PATHSCALE_BINARY_DIR}/common/targ_info
  ${PATHSCALE_BINARY_DIR}/be
  be
  be/${BUILD_TARGET_DIR}
  cg
  com
  com/${BUILD_TARGET_DIR}
  lno
  opt
  prompf_anl
  region
  vho
  whirl2c
  whirl2f
  purple2
  .

  cg/${BUILD_TARGET_DIR}
  cg/gra_mon
  cg/swp

  ../dfgforise
  ../ipa/common
  ../ipa/local
  ../ipa/main/analyze
  ../ipa/main/optimize

  ../ipa_link/include

  ../libair
  ../common/com
  ../common/com/${BUILD_TARGET_DIR}
  ../common/instrument
  ../common/targ_info/access
  ../common/util
  ../common/stl  
  ../common/targ_info/parser/inc/


  ${PATHSCALE_BINARY_DIR}/../GCC/gcc
  ../include
  ../libdwarf/libdwarf
  )


ADD_DEFINITIONS(
  -funsigned-char
)

IF(PATH64_ENABLE_VALGRIND)
ADD_DEFINITIONS(
  -DMONGOOSE_BE
  -DBACK_END
  -D__MIPS_AND_IA64_ELF_H
  )
ELSE(PATH64_ENABLE_VALGRIND)
ADD_DEFINITIONS(
  -DNO_VALGRIND
  -DMONGOOSE_BE
  -DBACK_END
  -D__MIPS_AND_IA64_ELF_H
  )
ENDIF(PATH64_ENABLE_VALGRIND)

ADD_SUBDIRECTORY(be)
ADD_SUBDIRECTORY(cg)
ADD_SUBDIRECTORY(lno)
ADD_SUBDIRECTORY(opt)
ADD_SUBDIRECTORY(whirl2c)
ADD_SUBDIRECTORY(whirl2f)
ADD_SUBDIRECTORY(prompf_anl)
