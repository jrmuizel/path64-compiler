# this should build coco
SET(PATHSCALE_COCO_SRCS "coco.F90" )

set_source_files_properties("coco.F90" PROPERTIES LANGUAGE Fortran)
if(PATH64_ENABLE_FORTRAN)
    path64_set_multiarch_sources(coco_sources COMMON ${PATHSCALE_COCO_SRCS})
    path64_add_multiarch_executable(coco coco_sources)
#    path64_add_multiarch_multiarch_dependencies(coco pathfortran pathfstart)
endif()
