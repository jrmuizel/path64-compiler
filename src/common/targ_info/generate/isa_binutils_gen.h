/*

  Copyright (C) 2008 STMicroelectronics, Inc.  All Rights Reserved.

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


// isa_binutils_gen.h
/////////////////////////////////////
//
// Interface for connecting GNU Binary Utilities to targinfo.
//
// This interface is specific for GNU Binary Utilities that are
// using targinfo through AIR.
//
//  TYPE BINUTILS_TYPE
//      A type that represents binary utility information for a subset.
//
//  TYPE EXTENSION_RELOCATE_ENCODING
//      A type that represents extension relocated instructions for encoding.
//
//  void ISA_Binutils_Begin(int portsize)
//      Initialize to generate Binary Utility information for an 
//      architecture. The information will be written to the files
//      targ_isa_binutils.[ch].
//      <portsize> is the maximum number of extension + 1 allowed for the
//      target.
//
//  void ISA_Binutils_Create(ISA_SUBSET subset,
//		             const char *abstract,
//			     const char *short_abstract,
//			     mUINT32 magic_key,
//			       INT   last_scratch,
//                           mUINT32 emul_byte_nr,
//                           mUINT32 DWARF_reg_nr,
//                           mUINT32 xversion,
//                           ... );
//      Create a binary utility type for <subset> by setting the 
//      following attributes:
//        - abstract:       string summarizing the subset features
//                          (long version).
//        - short_abstract: string summarizing the subset features 
//                          (short version).
//        - magic_key:      unique identifier of subset (0 means generic).
//        - last_scratch:   last register assumed as scratch by ABI. -1 for
//                          all register scratch (default).
//        - emul_byte_nr:   number of bytes reserved for emulation purposes
//                          at stack section end.
//        - DWARF_reg_nr:   number of DWARF registers defined by extension.
//        - xversion:       CORXpert version used for extension building (0
//                          when CORXpert not used).
//        - ...             list of couples of [port number, extension name]
//                          (port numbers are 0 to 7 for extensions, and 8 
//                           for core). [-1, NULL] is the ending couple.
//
//  void ISA_Binutils_Extension_Relocate_Encoding(ISA_SUBSET subset,
//		                 ...);
//      Attach an instruction encoding definition to a <subset> by setting the
//      following attributes:
//        - ...             list of quartet <relocation symbol>, <TOP>, <mask>.
//                          Last value is <NULL, TOP_UNDEFINED, -1ULL>.
//                          Relocation symbol is typically the mnemonic (uppercase)
//                          enclosed with $$. For example, for mp it can be "$$MP$$".
//                          This symbol will be used by the assembler and the linker.
//      The goal of an entry is to determine a <TOP> that makes the sequence to be 
//      relocatable. 
//
//  void ISA_Binutils_End(void)
//      Complete processing of binary utilities interface.
//
/////////////////////////////////////


#ifndef isa_binutils_gen_INCLUDED
#define isa_binutils_gen_INCLUDED

#include "gen_util_gen.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "targ_isa_subset.h"

extern void ISA_Binutils_Begin(int portsize);

extern void ISA_Binutils_Create(ISA_SUBSET subset,
	   		        const char *abstract,
			        const char *short_abstract,
			        mUINT32 magic_key,
			          INT   last_scratch,
			        mUINT32 emul_byte_nr,
			        mUINT32 DWARF_reg_nr,
			        mUINT32 xversion,
			        ... );

extern void ISA_Binutils_Extension_Relocate_Encoding(ISA_SUBSET subset,...);

extern void ISA_Binutils_End(void);

#ifdef __cplusplus
}
#endif
#endif /* isa_binutils_gen_INCLUDED */
