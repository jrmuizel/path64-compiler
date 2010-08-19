# build gensyn and a couple custom commands

SET(PATHSCALE_GENSYSN_SRC   
   "src/action0.c"
    "src/anasynt0.c"
    "src/avl.c"
    "src/format.c"
    "src/gensyn_option.c"
    "src/groups.c"
    "src/table.c"
    "src/analex0.c"        
    "src/arbsem.c"       
    "src/encinfo.c"        
    "src/gensyn_error.c"   
    "src/gensyn_version.c" 
    "src/lname.c")

INCLUDE_DIRECTORIES(
	include
	../../../rtkutils/)


add_executable(gensyn ${PATHSCALE_GENSYSN_SRC})
target_link_libraries(gensyn rtkutils)

