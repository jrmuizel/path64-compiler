/* 

  Copyright (C) 2006 ST Microelectronics, Inc.  All Rights Reserved. 

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

  Contact information:  ST Microelectronics, Inc., 
  , or: 

  http://www.st.com 

  For further information regarding this notice, see: 

  http: 
*/
/*
 * API used by both Front-end and Back-end to load extension dlls
 * and initialize common structures (mtypes, intrinsics)
 */
#ifndef LOADER_H
#define LOADER_H

#include "dyn_isa_api_access.h"
#include "dyn_dll_api_access.h"
#include "dll_loader.h"
// #include "wn.h"
typedef struct WN;

#include "isa_loader.h"
#include "isa_loader_api.h"

/* ===============================
 * Description of an extension dll
 * =============================== */
struct Extension_dll {
  Extension_dll_handler    *handler;
  EXTENSION_HighLevel_Info *hooks;

  // Unique identifier of an extension (based on its name)
  INT32 extension_id;

  // Rank associated to a loaded extension,
  // (value between 0 and nb_loaded_extensions-1)
  INT32 rank;

  // Base indexes for dynamic components of current extension
  int  base_mtypes;
  int  base_intrinsics;
};

typedef struct Extension_dll Extension_dll_t;


/* ============================================================
 * Numbering conversion macros
 * ---------------------------
 *  They are used to convert identifiers allocated to extension 
 *  specific components from  the  extension  numbering (static
 *  resources + 1 extension) to  the  global  numbering (static
 *  resources + all extensions)
 *
 * For example, considers that:
 *  - MTYPE_STATIC_LAST = 29
 *     --> id 0 to 29 are reserved for static mtypes
 *  - Extension dll 'a' contains 2 additional mtypes,
 *      a_mm_0 and a_mm_1 with id 30 and 31 in the dll
 *  - Extension dll 'b' contains 1 additional mtype,
 *      b_mm_0 with id 30 in the dll
 * 
 * The global numbering will be:
 *  - [0..29] : static mtypes
 *  - 30,31   : a_mm_0 and a_mm_1
 *  - 32      : b_mm_0
 * ============================================================ */
/* TYPE_ID DLL_TO_GLOBAL_MTYPE (Extension_dll_t*, TYPE_ID) */
#define DLL_TO_GLOBAL_MTYPE(ext_dll, mtype)				\
  ((mtype >= (int)ext_dll->hooks->get_mtypes_base_count())		\
   ?(TYPE_ID)((int)mtype						\
	      - (int)ext_dll->hooks->get_mtypes_base_count()		\
	      + ext_dll->base_mtypes					\
	      )								\
   :mtype)
/* INTRINSIC DLL_TO_GLOBAL_INTRINSIC (Extension_dll_t*, INTRINSIC) */
#define DLL_TO_GLOBAL_INTRINSIC(ext_dll, intrinsic)			\
  ((intrinsic >= (int)ext_dll->hooks->get_intrinsics_base_count())	\
   ?(INTRINSIC)((int)intrinsic						\
		- (int)ext_dll->hooks->get_intrinsics_base_count()	\
		+ ext_dll->base_intrinsics				\
		)							\
   :intrinsic)


/* ==================================
 * Initialization of extension loader
 * ================================== */

/* Load extension dlls and return TRUE if successful */
extern bool Load_Extension_dlls(bool verbose);

/* Return a pointer to the extension table */
extern Extension_dll_t * Get_Extension_dll_tab(void);

/* Return Count of loaded extensions */
extern int Get_Extension_dll_count(void);

/* Get dll extension table. */
BE_EXPORTED extern void Get_Loader_Extension_Table( Extension_dll_t **ext_tab, 
                                                    mUINT32 *ext_count);

/* Initialize all needed stuff for machine mode */
BE_EXPORTED extern void Initialize_Extension_Loader(void);

/* Initialize Register */
BE_EXPORTED extern void Initialize_Extension_Loader_Register(void);

/* Initialize PU specific options */
BE_EXPORTED extern void Initialize_Extension_Loader_PU(WN *pu);

/* Specific initiliazation for extension support */
void Initialize_Extension_Support();

/* Cleanup code for the loader */
extern void Cleanup_Extension_Loader(void);


/* ============================================
 * MType and Intrinsic initialization functions
 * ============================================ */

/* OPEN64 Intrinsics init */
extern void Init_Intrinsics(int nb_builtins_to_add);

/* Add new intrinsic from an extension */
extern void Add_Intrinsics(const Extension_dll_t *dll_instance,
                           BOOL verbose);

/* Dynamic OPEN64 MTYPES initialization */
extern void Init_Mtypes(int nb_mtype_to_add);

/* Define the dynamic count: to be place in the loader section */
extern TYPE_ID Add_MTypes(const Extension_dll_t *dll_instance,
                          int **local_mtype_to_rclass,
                          int *mtype_count,
                          BOOL verbose);

/* Run thru extension intrinsics to create composed MTYPE for multiple result support */
extern void Add_Composed_Mtypes(void);

#endif /* LOADER_H */
