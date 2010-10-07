# this should build coco
SET(PATHSCALE_COCO_SRCS "coco.F90" )

option(PATH64_ENABLE_COCO "Enable f95 coco preprocessor" ON)

set_source_files_properties("coco.F90" PROPERTIES LANGUAGE Fortran)
if(PATH64_ENABLE_FORTRAN)
    if(PATH64_ENABLE_COCO)
        path64_set_multiarch_sources(coco_sources COMMON ${PATHSCALE_COCO_SRCS})
        path64_add_multiarch_executable(coco coco_sources)
        path64_add_multiarch_dependencies(coco switch_fc compiler-stage-libs-Fortran)
        path64_install_multiarch(coco)
    endif()
endif()
