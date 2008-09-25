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

#include "extension_include.h"
#include "dyn_dll_api_access.h"
#include "wn.h"

#ifdef TARG_ST
/* ===============================
 * Description of an extension dll
 * =============================== */
struct Extension_dll {
  char                     *extname;
  char                     *dllname;
  void                     *handler;
  EXTENSION_HighLevel_Info *hooks;

  // Base indexes for dynamic components of current extension
  int  base_mtypes;
  int  base_mmodes;
  int  base_intrinsics;
  int  base_builtins;
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
 *  - MACHINE_MODE_STATIC_LAST = 29
 *     --> id 0 to 29 are reserved for static mmodes
 *  - Extension dll 'a' contains 2 additional machine modes,
 *      a_mm_0 and a_mm_1 with id 30 and 31 in the dll
 *  - Extension dll 'b' contains 1 additional machine mode,
 *      b_mm_0 with id 30 in the dll
 * 
 * The global numbering will be:
 *  - [0..29] : static mmodes
 *  - 30,31   : a_mm_0 and a_mm_1
 *  - 32      : b_mm_0
 * ============================================================ */
/* machine_mode_t DLL_TO_GLOBAL_MMODE (Extension_dll_t*, machine_mode_t) */
#define DLL_TO_GLOBAL_MMODE(ext_dll, mmode)				\
  ((mmode >= (int)ext_dll->hooks->get_modes_base_count())		\
   ?(machine_mode_t)((int)mmode						\
		     - (int)ext_dll->hooks->get_modes_base_count()	\
		     + ext_dll->base_mmodes				\
		     )							\
   :mmode)
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
/* machine_mode_t DLL_TO_GLOBAL_BUILTIN (Extension_dll_t*, machine_mode_t) */
#define DLL_TO_GLOBAL_BUILTIN(ext_dll, builtin)				\
  ((builtin >= (int)ext_dll->hooks->get_builtins_base_count())		\
   ?(enum built_in_function)((int)builtin				\
		- (int)ext_dll->hooks->get_builtins_base_count()	\
		+ ext_dll->base_builtins				\
		)							\
   :builtin)


/* ============================================
 * MType and Intrinsic initialization functions
 * ============================================ */
/* Load extension dlls and return a pointer to the extension table
   and the count of loaded extensions */
extern void Load_Extension_dlls(Extension_dll_t **ext_tab,
				int *ext_count,
				BOOL verbose);

/* Define the dynamic count: to be place in the loader section*/
extern TYPE_ID Add_MTypes(const Extension_dll_t *dll_instance, int **local_mtype_to_rclass, int *mtype_count, BOOL verbose);

/* Initialize all needed stuff for machine mode */
BE_EXPORTED extern void Initialize_Extension_Loader(void);

/* Initialize Register */
BE_EXPORTED extern void Initialize_Extension_Loader_Register(void);

/* Initialize PU specific options */
BE_EXPORTED extern void Initialize_Extension_Loader_PU(WN *pu);

/* OPEN64 Intrinsics init */
extern void Init_Intrinsics(int nb_builtins_to_add);

/* Add new intrinsic from an extension */
extern void Add_Intrinsics(const Extension_dll_t *dll_instance, BOOL verbose);

/* Dynamic OPEN64 MTYPES initialization */
extern void Init_Mtypes(int nb_mtype_to_add);

/* Cleanup code for the loader */
extern void Cleanup_Extension_Loader(void);

/* Common things: these functions are shared between wfe_loader and lai_loader */
#include "loader_internal.h"
/* Return a pointer to the newly allocated API used to access to the 
   ISA description of the specified extension. */
extern EXTENSION_ISA_Info *Generate_EXTENSION_ISA_Info(const Extension_dll_t *dll_instance, BOOL verbose);

/* Isa register init */
extern void Initialize_ISA_RegisterClasses(Lai_Loader_Info_t &ext_info);

/* ABI init */
extern void Initialize_ABI_Properties(Lai_Loader_Info_t &ext_info);

/* Map between preg and register in the extension */
BE_EXPORTED extern ISA_REGISTER_CLASS EXTENSION_PREG_to_REGISTER_CLASS(PREG_NUM preg);

/* Map between gcc machine mode preg and open64 MTYPE */
BE_EXPORTED extern TYPE_ID MachineMode_To_Mtype(machine_mode_t mode);

/* Map between gcc machine mode preg and open64 MTYPE */
BE_EXPORTED extern machine_mode_t Mtype_To_MachineMode(TYPE_ID mode);

/* Run thru extension intrinsic to create composed mtype for multiple result support */
extern void Add_Composed_Mtype(void);

/* Get dll extension table. */
BE_EXPORTED extern void Get_Loader_Extension_Table( Extension_dll_t **ext_tab, 
                                                    mUINT32 *ext_count);


#endif /* TARG_ST */

#endif /* LOADER_H */
