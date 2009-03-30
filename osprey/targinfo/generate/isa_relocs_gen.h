/*

  Copyright (C) 2007-2008 STMicroelectronics, Inc.  All Rights Reserved.

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

*/

/*
 * This file has been added by STMicroelectronics.
 *
 */


/////////////////////////////////////
// isa_relocs_gen.h
/////////////////////////////////////
//
// Interface to describe relocation information.
//
// Although relocation is a generic concept that is not related to
// one given type of file format, work done hereafter is closely
// related to ELF standard and ELF file format.
//
//  void Isa_Relocs_Begin(void)
//      Initialize to generate relocation information for an architecture.
//      The information will be written to the files targ_isa_relocs.[ch].  
//
//  void ISA_Reloc_Duplicate_Relocation_for_Relaxation( ISA_SUBSET subset, ...)
//      Setup relocation generator so that it creates two entries for each
//      relocation required in order to deal with relaxation purposes on
//      cores that have a variable-length instruction set. When activated
//      this features uses virtual id numbers for relocation as 2n for the
//      relaxing version of the relocation and 2n+1 for associated 
//      non-relaxing one. Therefore the core contains twice the number of
//      relocations declared in isa_relocs.cxx file.
//      Takes a list of subset concerned by relaxation (may be all core subsets
//      are not concerned with this feature). List closing value is 
//      ISA_SUBSET_UNDEFINED.
//
//  TYPE RELOC_TYPE
//      An abstract type that represents a relocation function.
//
//  TYPE ISA_VIRTUAL_RELOC_TYPE
//      An abstract type that represents a logical relocation. A
//      logical relocation consists of one or many physical
//      relocations. Physical relocations are primarily described
//      by their (unique and non-splitted) bitfields and their ELF
//      identifiers.
//
//      Logical relocations are necessary (at at least very useful)
//      on architectures that extensively use splitted bitfields.
//
//  RELOC_TYPE ISA_Create_Reloc_Type(const char *name,
//				     RELOC_MAIN_SYMBOL main_symbol,
//				     RELOC_SECOND_SYMBOL second_symbol,
//				     RELOC_ADDEND addend,
//				     RELOC_PCREL pcrel,
//				     RELOC_GPREL gprel,
//				     RELOC_GOTREL gotrel );
//      Create a relocation type by setting the following attributes:
//        - main_symbol: applied on a symbol.
//        - second_symbol: applied on two symbols. Second symbol is coded 
//          in immediate bit field.
//        - addend: Elf addend.
//        - pcrel: PC (Program Counter) relative.
//        - gprel: GP (Global Pointer) relative.
//        - gotrel: GOT (Global Offset Table) relative.
//
//  TYPE BITFIELD
//       An abstract type that represents a bitfield.
//
//  TYPE ISA_RELOC_OVERFLOW_TYPE
//      Enumeration of overflow checking types:
//         - ISA_RELOC_NO_OVERFLOW: no check.
//         - ISA_RELOC_OVERFLOW_BITFIELD: binary relocated value exceed bitfield.
//         - ISA_RELOC_OVERFLOW_SIGNED  : unsigned relocated value exceed 
//                                        unsigned representable value.
//         - ISA_RELOC_OVERFLOW_UNSIGNED: signed relocated value exceed signed
//                                        representable value.
//
//  TYPE ISA_RELOC_UNDERFLOW_TYPE
//      Enumeration of underflow checking types:
//         - ISA_RELOC_NO_UNDERFLOW: no check.
//         - ISA_RELOC_UNDERFLOW: check if lowest bits are 0 in case of scaling.
//
//  ISA_RELOC_TYPE ISA_Create_Reloc (
//                         int logic_id,
//                         const char *name,
//			   const char *syntax,
//			   ISA_RELOC_OVERFLOW_TYPE overflow,
//			   ISA_RELOC_UNDERFLOW_TYPE underflow,
//			   int right_shift,
//			   RELOC_TYPE rel_type,
//			   ...)
//      Create a relocation entry by setting the following parameters:
//         - logic_id: logical identifier
//         - name: relocation's name.
//         - syntax: for syntaxically explicit relocations.
//         - overflow: overflow type checking.
//         - underflow: underflow type checking. This is to check if
//           lowest bit are equal to 0 in case of scaling.
//         - right_shift: scaling applied on the relocation.
//         - rel_type: relocation function.
//         - variadic argument: bit fields list ending by BITFIELD_END.
//
//  void ISA_Relocs_Subset( ISA_SUBSET subset, ...)
//      List relocations associated to a given subset. This function
//      accepts the following parameters:
//         - subset: isa subset identifier
//         - reloc: variadic argument of type ISA_VIRTUAL_RELOC_TYPE.
//           variadic list is to be finished by NULL pointer.
//
//  void ISA_Relocs_End(void)
//      Complete processing of relocations.
//
/////////////////////////////////////


#ifndef isa_relocs_gen_INCLUDED
#define isa_relocs_gen_INCLUDED

#ifdef _KEEP_RCS_ID
#endif /* _KEEP_RCS_ID */

#ifdef __cplusplus
extern "C" {
#endif

#include "targ_isa_lits.h"
#include "targ_isa_subset.h"

typedef enum {
  R_NO_MAIN_SYMBOL,
  R_MAIN_SYMBOL,
} RELOC_MAIN_SYMBOL;

typedef enum {
  R_NO_SECOND_SYMBOL,
  R_SECOND_SYMBOL,
} RELOC_SECOND_SYMBOL;

typedef enum {
  R_NO_ADDEND,
  R_ADDEND,
} RELOC_ADDEND;

typedef enum {
  R_NO_PCREL,
  R_PCREL,
} RELOC_PCREL;

typedef enum {
  R_NO_GPREL,
  R_GPREL,
} RELOC_GPREL;

typedef enum {
  R_NO_GOTREL,
  R_GOTREL,
} RELOC_GOTREL;

typedef enum {
  ISA_RELOC_NO_OVERFLOW,
  ISA_RELOC_OVERFLOW_BITFIELD,
  ISA_RELOC_OVERFLOW_SIGNED,
  ISA_RELOC_OVERFLOW_UNSIGNED,
} ISA_RELOC_OVERFLOW_TYPE;

typedef enum {
  ISA_RELOC_NO_UNDERFLOW,
  ISA_RELOC_UNDERFLOW,
} ISA_RELOC_UNDERFLOW_TYPE;

typedef struct reloc_type *RELOC_TYPE;

typedef struct bitfield *BITFIELD;

#define BITFIELD_END ((int)-1)

#define MAX_BITFIELDS (6)

typedef struct isa_virtual_reloc_type *ISA_VIRTUAL_RELOC_TYPE;

extern void ISA_Relocs_Begin (void);

extern void ISA_Reloc_Duplicate_Relocation_for_Relaxation(ISA_SUBSET subset,
                                                          ...);

extern RELOC_TYPE ISA_Create_Reloc_Type(const char *name,
					RELOC_MAIN_SYMBOL main_symbol,
					RELOC_SECOND_SYMBOL second_symbol,
					RELOC_ADDEND addend,
					RELOC_PCREL pcrel,
					RELOC_GPREL gprel,
					RELOC_GOTREL gotrel );

extern BITFIELD ISA_Create_BitField   (const char *name,
                                       int start_bit, 
                                       int bit_size);

extern ISA_VIRTUAL_RELOC_TYPE ISA_Create_Reloc
                                      (int logic_id,
                                       const char *name,
                                       const char *syntax,
                                       ISA_RELOC_OVERFLOW_TYPE overflow,
                                       ISA_RELOC_UNDERFLOW_TYPE underflow,
                                       int right_shift,
                                       RELOC_TYPE rel_type,
                                       ...);

extern void ISA_Reloc_Subset          (ISA_SUBSET subset,
                                       ...);

extern void ISA_Reloc_Set_Core_Subset(const char * subset_name);

extern void ISA_Relocs_End (void);

#ifdef __cplusplus
}
#endif
#endif /* isa_relocs_gen_INCLUDED */
