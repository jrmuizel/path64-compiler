/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


/* ====================================================================
 * ====================================================================
 *
 * Module: ipc_weak.h
 *
 * Revision history:
 *  14-Jun-95 - Original Version
 *  12-Apr-96 - Suppress C++ warnings
 *
 * Description:
 *
 *  Declare symbols defined in ld and referenced in ipa.so weak so that
 *  ipa.so will link cleanly.
 *
 * TODO:  C++ doesn't yet recognize #pragma weak, so we suppress this
 * all for C++ compilations.  Reinstate when C++ supports them.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef ipc_weak_INCLUDED
#define ipc_weak_INCLUDED

#if defined(TARG_ST) //&& defined(_NO_WEAK_SUPPORT_)

#ifndef LD_IPA_INTERFACE
#define EXTERN extern
#else
#define EXTERN
#endif

#include <cmplrs/host.h> // For typedef string
#include "defs.h"        // For INT, UINT64....
#include "linker.h"      // For MAX_LD_IPA.....

/* [CL] pointers to external symbols used by ipa.so */
typedef LD_IPA_OPTION ipa_option_array[MAX_LD_IPA];

#define IPA_EXTERNAL_SYMBOLS_VERSION 1

struct external_symbols_list {
  /* Size and version of this struct, for sanity checks */
    int size;
    int version;

    /* pass1.h */
    unsigned int *p_used_gp_area;

    /* ld_ipa_option.h */
    ipa_option_array *p_ld_ipa_opt;
/*     ipacom_flags */
    string *p_WB_flags;
    string *p_Y_flags;

    /* error.h */
/*     msg */

    /* ext_tbl.h */
    void (*p_merge_ext)(void*, char *, int, void*);
/*     enter_mext */
/*     slookup_mext */
/*     slookup_mext_idx */
/*     get_mext */
/*     ext_tbl */
    void* (*p_ld_slookup_mext)(char *, boolean);

    /* obj_file.h */
/*     num_ir */
/*     get_next_ir */
/*     is_archive_member */

    /* process.h */
    int (*p_create_tmpdir)(int);
    string (*p_create_unique_file)(string, char);
    string (*p_create_unique_file_in_curdir)(string, char);
    void (*p_add_to_tmp_file_list)(string);
    string *p_tmpdir;
/*     get_command_line */
/*     make_link */

    /* ld_util.h */
    string (*p_concat_names)(string, string);

    /* ld_main.h */
    int *p_arg_count;
    char** *p_arg_vector;
    char** *p_environ_vars;
    int *p_max_gpa_size;

    /* read.h */
    void (*p_read_one_section)(int, void*);
/*     read_headers */
/*     unread_sections */
/*     unread_obj */
/*     objs_mapped_total_size */
/*     copy_section */

    /* dem.h */
    char* (*p_always_demangle)(char *, char );

    /* elfhash.h */
/*     elfhash */

    /* ld_ipa_interface.h */
  UINT64 (*p_ld_get_section_size) (an_object_file_ptr pobj, int index);

  char *(*p_ld_get_section_name) (an_object_file_ptr pobj, int index);

  char *(*p_ld_get_section_base) (an_object_file_ptr pobj, int index);

  void *(*p_ld_get_mmap_addr) (an_object_file_ptr pobj);

/* extern unsigned int */
/* ld_get_sym_attr (void* pext); */

/* extern int */
/* ld_is_weak_symbol (void* pext); */

/* extern ST_EXPORT */
/* ld_get_export (void* pext); */

  void (*p_ld_set_st_idx) (void* pext, int st_idx);

  int (*p_ld_get_st_idx) (void* pext);

  BOOL (*p_ld_resolved_to_obj) (void* pext, void* pobj);

/* /*extern void */
/*   cleanup_symtab_for_ipa (void); */

  INT (*p_Count_elf_external_gots) (void);

  string *p_outfilename;
};

#define IPA_REF(name) (*ipa_external_symbols.name)

    /* pass1.h */
#define used_gp_area IPA_REF(p_used_gp_area)

    /* ld_ipa_option.h */
#define ld_ipa_opt IPA_REF(p_ld_ipa_opt)

/*     ipacom_flags */
#define WB_flags IPA_REF(p_WB_flags)
#define Y_flags IPA_REF(p_Y_flags)

    /* error.h */
/*     msg */

    /* ext_tbl.h */
#define merge_ext IPA_REF(p_merge_ext)
/*     enter_mext */
/*     slookup_mext */
/*     slookup_mext_idx */
/*     get_mext */
/*     ext_tbl */
#define ld_slookup_mext IPA_REF(p_ld_slookup_mext)

    /* obj_file.h */
/*     num_ir */
/*     get_next_ir */
/*     is_archive_member */

    /* process.h */
#define create_tmpdir IPA_REF(p_create_tmpdir)
#define create_unique_file IPA_REF(p_create_unique_file)
#define create_unique_file_in_curdir IPA_REF(p_create_unique_file_in_curdir)
#define add_to_tmp_file_list IPA_REF(p_add_to_tmp_file_list)
#define tmpdir IPA_REF(p_tmpdir)
/*     get_command_line */
/*     make_link */

    /* ld_util.h */
#define concat_names IPA_REF(p_concat_names)

    /* ld_main.h */
#define arg_count IPA_REF(p_arg_count)
#define arg_vector IPA_REF(p_arg_vector)
#define environ_vars IPA_REF(p_environ_vars)
#define max_gpa_size IPA_REF(p_max_gpa_size)

    /* read.h */
#define read_one_section IPA_REF(p_read_one_section)
/*     read_headers */
/*     unread_sections */
/*     unread_obj */
/*     objs_mapped_total_size */
/*     copy_section */

    /* dem.h */
#define always_demangle IPA_REF(p_always_demangle)

    /* elfhash.h */
/*     elfhash */

    /* ld_ipa_interface.h */
#define ld_get_section_size IPA_REF(p_ld_get_section_size)
#define ld_get_section_name IPA_REF(p_ld_get_section_name)
#define ld_get_section_base IPA_REF(p_ld_get_section_base)
#define ld_get_mmap_addr IPA_REF(p_ld_get_mmap_addr)

/* extern unsigned int */
/* ld_get_sym_attr (void* pext); */

/* extern int */
/* ld_is_weak_symbol (void* pext); */

/* extern ST_EXPORT */
/* ld_get_export (void* pext); */

#define ld_set_st_idx IPA_REF(p_ld_set_st_idx)
#define ld_get_st_idx IPA_REF(p_ld_get_st_idx)
#define ld_resolved_to_obj IPA_REF(p_ld_resolved_to_obj)

/* /*extern void */
/*   cleanup_symtab_for_ipa (void); */

#define Count_elf_external_gots IPA_REF(p_Count_elf_external_gots)
#define outfilename IPA_REF(p_outfilename)


EXTERN struct external_symbols_list ipa_external_symbols;

#else
#ifdef __PASS1_H__
#pragma weak used_gp_area
#endif /* __PASS1_H__ */

#ifdef __IPA_OPTION_H__
#pragma weak ld_ipa_opt
#pragma weak ipacom_flags
#pragma weak WB_flags
#pragma weak Y_flags
#endif /* __OPTION_H__ */

#ifdef __LD_ERROR_H__
#pragma weak msg
#endif /* __LD_ERROR_H__ */

#ifdef __EXT_TBL_H__
#pragma weak merge_ext
#pragma weak enter_mext
#pragma weak slookup_mext
#pragma weak slookup_mext_idx
#pragma weak get_mext
#pragma weak ext_tbl

#pragma weak ld_slookup_mext

#endif /* __EXT_TBL_H__ */

#ifdef __OBJ_FILE_H__
#pragma weak num_ir
#pragma weak get_next_ir
#pragma weak is_archive_member
#endif /* __OBJ_FILE_H__ */

#ifdef __PROCESS_H__
#pragma weak create_tmpdir
#pragma weak create_unique_file
#ifdef TARG_ST // [CL] create file in current dir rather that tmpdir
#pragma weak create_unique_file_in_curdir
#endif
#pragma weak add_to_tmp_file_list
#pragma weak tmpdir
#pragma weak get_command_line
#pragma weak make_link
#endif /* __PROCESS_H__ */

#ifdef __LD_UTIL_H__
#pragma weak concat_names
#endif /* __LD_UTIL_H__ */

#ifdef __LD_MAIN_H__
#pragma weak arg_count
#pragma weak arg_vector
#pragma weak environ_vars
#pragma weak max_gpa_size
#endif /* __LD_MAIN_H__ */

#ifdef __READ_H__
#pragma weak read_one_section
#pragma weak read_headers
#pragma weak unread_sections
#pragma weak unread_obj
#pragma weak objs_mapped_total_size
#pragma weak copy_section
#endif /* __READ_H__ */

#ifdef __DEM_H__
#pragma weak always_demangle
#endif /* __DEM_H__ */

#ifdef __ELFHASH_H
#pragma weak elfhash
#endif /* __ELFHASH_H */


#endif /* TARG_ST */

#endif /* ipc_weak_INCLUDED */
