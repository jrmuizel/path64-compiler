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


// TODO POST MERGE: sync with targinfo/stxp70/cg/targ_cgdwarf.cxx

#include <stdlib.h>
#include <stdio.h>
#include <elf.h>
#include <elfaccess.h>
#include <libelf.h>
#include <libdwarf.h>
#include "targ_em_dwarf.h"
#include <assert.h>	// temporary
#define USE_STANDARD_TYPES 1
#include "dwarf_DST.h" // To access compilation unit's dwarf information
#include "register.h" // For REGISTER_MIN and CGTARG_DW_DEBUG_Get_Extension_Id
#include "cgir.h"
#include "tn_map.h"
#include "em_elf.h"
#include "dwarf_DST_mem.h"         /* for DST_IDX */
#include "em_dwarf.h"

/**
 * Debug information data alignment factor.
 * This variable is used to compute the information according to that unit
 * they are expected to be.
 */
static const INT Data_Alignment_Factor = -4;

/**
 * Debug information code alignment factor.
 * This variable is used to compute the information according to that unit
 * they are expected to be. It is set to 1, because we make label differences
 * for asm statement
 */
static const INT Code_Alignment_Factor = 1;

/**
 * Number of characters in the relocation string before the slot number.
 * __DWR<SlotNumber>
 */
static const INT NB_CHAR_RELOC = 5;

/**
 * Slot extension information.
 */
typedef struct
{
    /**
     * Specify whether current information is on an extension (TRUE) or not
     * (FALSE).
     */
    BOOL isExtension;
    /**
     * Slot number of the extension. This field is meaningless, unless
     * isExtension is true.
     */
    unsigned char slot;
} SlotInfo;

static SlotInfo
GetSlotInfo(ISA_REGISTER_CLASS reg_class);

static BOOL
Is_Interrupt(PU& pu);


INT
CodeAlignmentFactor(PU& pu)
{
    return Code_Alignment_Factor;
}

INT
DataAlignmentFactor(PU& pu)
{
    return Data_Alignment_Factor;
}

INT
HashValue(PU& pu)
{
    return Is_Interrupt(pu)? 1: 0;
}

BOOL
IsSaved(const DebugRegId& a_id, ISA_REGISTER_CLASS regClass, REGISTER reg,
        PU& a_pu)
{
    return REGISTER_SET_MemberP(REGISTER_CLASS_callee_saves(regClass), reg) ||
        // Finally, LK register will not be modelized as
        // callee saved, so we have to make a special case
        a_id == DBGREG_LK || Is_Interrupt(a_pu);
}

BOOL
ShouldGenerateInformation(ISA_REGISTER_CLASS register_class, PU& pu)
{
    BOOL isActivated;
    static_cast<void>(CGTARG_DW_DEBUG_Get_Extension_Id(register_class,
                                                       &isActivated));
    return isActivated;
}

void
CfaDef(DebugRegId& a_base, UINT& a_offset, PU& a_pu)
{
    // In interrupt mode, CFA is SP - 4
    // In normal mode, CFA is SP - 0
    a_base = DBGREG_SP;
    a_offset = Is_Interrupt(a_pu)? 4: 0;
}

BOOL
HasSpecialDef(const DebugRegId& a_id, UINT& offset,
              ISA_REGISTER_CLASS regClass, REGISTER reg, PU& a_pu)
{
    return FALSE;
}

extern BOOL
ReturnAddressDef(DebugRegId& a_id, UINT& offset, PU& a_pu)
{
    BOOL result = Is_Interrupt(a_pu);
    if(result)
        {
            a_id = DBGREG_RETADDR_INTERRUPT;
            offset = 0;
        }
    else
        {
            a_id = DBGREG_RETADDR;
        }
    return result;
}


DebugRegId
Get_Debug_Reg_Id(ISA_REGISTER_CLASS reg_class, REGISTER reg, INT bitSize)
{
    // Input register identifier are given for REGISTER API. To use it with
    // ISA_REGISTER API and so have a machine representation, we have to applied
    // the offset used between the two APIs.
    reg -= REGISTER_MIN;
    DbgPrintCIE((ciedbgOutput,
                 "Generate id for:\n\treg class: %s\n\treg: %s\n",
                 ISA_REGISTER_CLASS_INFO_Name
                 (ISA_REGISTER_CLASS_Info(reg_class)),
                 ISA_REGISTER_CLASS_INFO_Reg_Name
                 (ISA_REGISTER_CLASS_Info(reg_class), reg)));
    mINT32 regId = CGTARG_DW_DEBUG_Get_Reg_Id(reg_class, reg, bitSize);
    SlotInfo slotInfo = GetSlotInfo(reg_class);
    CExtensionAndRegister extAndReg(slotInfo.isExtension, (UINT)regId,
                                    slotInfo.slot);
    return DebugRegId(extAndReg.Operand());
}

/**
 * Check whether given function is an inturrption one or not.
 *
 * @param  pu [in] Program unit to be checked
 *
 * @pre    true
 * @post   true
 *
 * @return TRUE if pu is an inturrption function, FALSE otherwise
 */
static BOOL
Is_Interrupt(PU& pu)
{
    return BOOL(PU_is_interrupt(pu) || PU_is_interrupt_nostkaln(pu));
}

/**
 * Get related extension slot information.
 * The extension is the one that contains given reg_class
 *
 * @param  reg_class Register class for which we want the extension slot
 *         information
 *
 * @pre    true
 * @post   true
 *
 * @return The slot information of current reg_class
 */
static SlotInfo
GetSlotInfo(ISA_REGISTER_CLASS reg_class)
{
    // Information cache
    static ISA_REGISTER_CLASS prevClass = ISA_REGISTER_CLASS_integer;
    static SlotInfo result = {FALSE, 0};
    if(prevClass != reg_class)
        {
            const char* relocStr =
                CGTARG_DW_DEBUG_Get_Extension_Reloc_String(reg_class);
            prevClass = reg_class;
            result.isExtension = relocStr != NULL;
            if(result.isExtension)
                {
                    // FIXME: This computation has to be changed if the slot
                    // number is on more than one character
                    result.slot = *(relocStr + NB_CHAR_RELOC) - '0';
                }
            else
                {
                    result.slot = 0;
                }
        }
    return result;
}

const Dwarf_Unsigned CExtensionAndRegister::EXTENSION_MASK = 0xf;
const UINT CExtensionAndRegister::MASK_SHIFT_SIZE =
    14 - 4; // 14 is the max number of emitted bytes for a relocation and 4
// is the number of significant bits for EXTENSION_MASK. We set 14 because we do
// not want to modify the size of dwarf section for now

//------------------------------------------------------------------------------
// Other functions
//------------------------------------------------------------------------------

#pragma pack(1)
struct UINT32_unaligned {
  UINT32 val;
};

struct UINT64_unaligned {
  UINT64 val;
};
#pragma pack(0)

static Elf32_Rel *
translate_reloc32(Dwarf_Relocation_Data       rentry,
		  Cg_Dwarf_Sym_To_Elfsym_Ofst translate_symndx,
		  Dwarf_Ptr                   buffer,
		  Dwarf_Unsigned              bufsize)
{
  static Elf32_Rel retval;
  Dwarf_Unsigned   elf_symidx, elf_symoff;

  return &retval;
}

static Elf64_Rel *
translate_reloc64(Dwarf_Relocation_Data       rentry,
		  Cg_Dwarf_Sym_To_Elfsym_Ofst translate_symndx,
		  Dwarf_Ptr                   buffer,
		  Dwarf_Unsigned              bufsize)
{
  static Elf64_Rel retval;
  Dwarf_Unsigned   elf_symidx, elf_symoff;

  return &retval;
}

Dwarf_Ptr
Em_Dwarf_Symbolic_Relocs_To_Elf(next_buffer_retriever     get_buffer,
				next_bufsize_retriever    get_bufsize,
				advancer_to_next_stream   advance_stream,
				Dwarf_Signed              buffer_scndx,
				Dwarf_Relocation_Data     reloc_buf,
				Dwarf_Unsigned            entry_count,
				int                       is_64bit,
				Cg_Dwarf_Sym_To_Elfsym_Ofst translate_symndx,
				Dwarf_Unsigned           *result_buf_size)
{
  unsigned i;
  unsigned step_size = (is_64bit ? sizeof(Elf64_Rel) : sizeof(Elf32_Rel));

  Dwarf_Ptr  result_buf = (Dwarf_Ptr *) malloc(step_size * entry_count);
  char *cur_reloc = (char *) result_buf;

  Dwarf_Unsigned offset_offset = 0;
  Dwarf_Unsigned bufsize = 0;
  Dwarf_Ptr      buffer = NULL;

  return (Dwarf_Ptr) result_buf;
}

