/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

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
#ifndef __TARG_EM_DWARF_H__
#define __TARG_EM_DWARF_H__

#include "defs.h"              // Needed by mempool.h for MEM_PTR definition
#include "dwarf.h"             // For dwarf operand definition

// Obsolete CIE initial bytes definition
#define TARG_INIT_BYTES {DW_CFA_nop}

#define DWARF_TARGET_FLAG	DW_DLC_ISA_IA64

/**
 * Define the debug identifier of register R0
 */
#define	DBGREG_R0  0

/**
 * Define the debug identifier of register LK
 */
#define	DBGREG_LK  14

/**
 * Define the debug identifier of register SP
 */
#define	DBGREG_SP  15

/**
 * Define the debug identifier of register R16
 */
#define	DBGREG_R16  16

/**
 * Define the debug identifier of virtual CFA register
 */
#define DBGREG_CFA 48

/**
 * Define the debug identifier of virtual return address register for interrupt
 * function
 */
#define DBGREG_RETADDR_INTERRUPT DBGREG_CFA

/**
 * Define the debug identifier of virtual return address register for normal
 * function
 */
#define DBGREG_RETADDR DBGREG_LK

/**
 * Define the debug identifier of register LCR0
 */
#define DBGREG_LCR0 49

/**
 * Define the debug identifier of register GR
 */
#define DBGREG_GR 57

/**
 * Define the debug identifier of register FPX register F0
 */
#define DBGFPXREG_F0 73


typedef void (*Cg_Dwarf_Sym_To_Elfsym_Ofst)(Dwarf_Unsigned,
					    Dwarf_Unsigned *,
					    Dwarf_Unsigned *);

typedef Dwarf_Unsigned (*symbol_index_recorder)(Elf64_Word);

typedef int            (*advancer_to_next_stream)(Dwarf_Signed);
typedef Dwarf_Ptr      (*next_buffer_retriever)(void);
typedef Dwarf_Unsigned (*next_bufsize_retriever)(void);

Dwarf_Ptr Em_Dwarf_Symbolic_Relocs_To_Elf(next_buffer_retriever,
					  next_bufsize_retriever,
					  advancer_to_next_stream,
					  Dwarf_Signed,
					  Dwarf_Relocation_Data,
					  Dwarf_Unsigned,
					  int,
					  Cg_Dwarf_Sym_To_Elfsym_Ofst,
					  Dwarf_Unsigned *);

#endif // __TARG_EM_DWARF_H__
