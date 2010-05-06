# this should build coco
SET(PATHSCALE_COCO_SRCS "coco.F90" )

set_source_files_properties("coco.F90" PROPERTIES LANGUAGE Fortran)
if(PATH64_ENABLE_FORTRAN)
add_executable (coco ${PATHSCALE_COCO_SRCS})
add_dependencies(coco pathfortran pathfstart mv mpath)
install(TARGETS coco
    RUNTIME DESTINATION ${PATH64_LIB_PATH})

endif()
