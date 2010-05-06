PROJECT(libdwarf)

INCLUDE(CheckTypeSize)
CHECK_TYPE_SIZE(uint64_t __UINT64_T)

INCLUDE(CheckIncludeFile)
CHECK_INCLUDE_FILE("elf.h" HAVE_ELF_H)

CONFIGURE_FILE(
  ${libdwarf_SOURCE_DIR}/libdwarf/config.h.cmake.in
  ${libdwarf_BINARY_DIR}/config.h @ONLY
  )

INCLUDE_DIRECTORIES(BEFORE
  ${PATHSCALE_INCLUDE_DIR}/libelf
  ${libdwarf_SOURCE_DIR}/dwarfdump
  ${libdwarf_SOURCE_DIR}/libdwarf
  ${libdwarf_BINARY_DIR}
  )

ADD_EXECUTABLE(tag_tree_build
               dwarfdump/tag_tree.c)

ADD_EXECUTABLE(tag_attr_build
               dwarfdump/tag_attr.c)


GET_TARGET_PROPERTY(TAG_TREE_BUILD_EXE tag_tree_build LOCATION)
GET_TARGET_PROPERTY(TAG_ATTR_BUILD_EXE tag_attr_build LOCATION)


ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dwarf_names.c
        COMMAND awk -f ${CMAKE_CURRENT_SOURCE_DIR}/dwarfdump/dwarf_names.awk ${CMAKE_CURRENT_SOURCE_DIR}/libdwarf/dwarf.h > ${CMAKE_CURRENT_BINARY_DIR}/dwarf_names.c
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dwarfdump/dwarf_names.awk ${CMAKE_CURRENT_SOURCE_DIR}/libdwarf/dwarf.h
        )

ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/_tag_attr_table.c
        COMMAND ${CMAKE_C_COMPILER} -xc -E -I${CMAKE_CURRENT_SOURCE_DIR}/libdwarf ${CMAKE_CURRENT_SOURCE_DIR}/dwarfdump/tag_attr.list | awk '!/^\#/ && !/^[ \\t]*$$/' | ${TAG_ATTR_BUILD_EXE}  > _tag_attr_table.c
        DEPENDS tag_attr_build ${CMAKE_CURRENT_SOURCE_DIR}/dwarfdump/tag_attr.list
        )


ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/_tag_tree_table.c
        COMMAND ${CMAKE_C_COMPILER} -xc -E -I${CMAKE_CURRENT_SOURCE_DIR}/libdwarf ${CMAKE_CURRENT_SOURCE_DIR}/dwarfdump/tag_tree.list | awk '!/^\#/ && !/^[ \\t]*$$/' | ${TAG_TREE_BUILD_EXE} > _tag_tree_table.c
        DEPENDS tag_tree_build ${CMAKE_CURRENT_SOURCE_DIR}/dwarfdump/tag_tree.list
        )


SET(DWARF_DEPS ${CMAKE_CURRENT_BINARY_DIR}/_tag_attr_table.c     
               ${CMAKE_CURRENT_BINARY_DIR}/_tag_tree_table.c)

SET_SOURCE_FILES_PROPERTIES(dwarfdump/print_die.c
                            PROPERTIES OBJECT_DEPENDS "${DWARF_DEPS}")


ADD_LIBRARY(dwarf
            dwarfdump/dwarfdump.c
            dwarfdump/makename.c
            dwarfdump/tag_attr.c
            dwarfdump/print_die.c
            dwarfdump/print_reloc.c
            dwarfdump/print_sections.c
            libdwarf/dwarf_abbrev.c
            libdwarf/dwarf_alloc.c
            libdwarf/dwarf_arange.c
            libdwarf/dwarf_die_deliv.c
            libdwarf/dwarf_error.c
            libdwarf/dwarf_form.c
            libdwarf/dwarf_frame.c
            libdwarf/dwarf_funcs.c
            libdwarf/dwarf_global.c
            libdwarf/dwarf_init_finish.c
            libdwarf/dwarf_line.c
            libdwarf/dwarf_macro.c
            libdwarf/dwarf_loc.c
            libdwarf/dwarf_query.c
            libdwarf/dwarf_string.c
            libdwarf/dwarf_stubs.c
            libdwarf/dwarf_types.c
            libdwarf/dwarf_util.c
            libdwarf/dwarf_leb.c
            libdwarf/dwarf_vars.c
            libdwarf/dwarf_weaks.c
            libdwarf/dwarf_addr_finder.c
            libdwarf/dwarf_print_lines.c
            libdwarf/dwarf_sort_line.c
            libdwarf/pro_alloc.c
            libdwarf/pro_arange.c
            libdwarf/pro_die.c
            libdwarf/pro_encode_nm.c
            libdwarf/pro_error.c
            libdwarf/pro_expr.c
            libdwarf/pro_finish.c
            libdwarf/pro_forms.c
            libdwarf/pro_funcs.c
            libdwarf/pro_frame.c
            libdwarf/pro_init.c
            libdwarf/pro_line.c
            libdwarf/pro_reloc.c
            libdwarf/pro_reloc_stream.c
            libdwarf/pro_reloc_symbolic.c
            libdwarf/pro_macinfo.c
            libdwarf/pro_pubnames.c
            libdwarf/pro_section.c
            libdwarf/pro_types.c
            libdwarf/pro_vars.c
            libdwarf/pro_weaks.c
            dwf_forms.c
            dwf_frame.c
            dwf_section.c
            ${CMAKE_CURRENT_BINARY_DIR}/dwarf_names.c
            )


