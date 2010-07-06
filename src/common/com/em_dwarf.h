/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/


#ifndef em_dwarf_INCLUDED
#define em_dwarf_INCLUDED

#include "dwarf_stuff.h"
#include "srcpos.h"
#include "targ_em_dwarf.h"

#if defined(__cplusplus)
extern Dwarf_P_Debug Em_Dwarf_Begin (BOOL is_64bit,
				     BOOL dwarf_trace,
				     BOOL is_cplus,
				     symbol_index_recorder record_symidx = NULL);

extern void Em_Dwarf_Write_Scns (Cg_Dwarf_Sym_To_Elfsym_Ofst translate_elfsym = NULL);
#else
extern Dwarf_P_Debug Em_Dwarf_Begin (BOOL is_64bit,
				     BOOL dwarf_trace,
				     BOOL is_cplus);

extern void Em_Dwarf_Write_Scns (void);
#endif

#if defined(__cplusplus)
extern "C" {
#endif

extern INT data_alignment_factor;

extern void Em_Dwarf_End (void);

extern pSCNINFO Em_Dwarf_Find_Dwarf_Scn (size_t scndx);

extern pSCNINFO Em_Dwarf_Find_Dwarf_Scn_By_Name (char *name);

extern INT Em_Dwarf_Prepare_Output (void);

extern void Em_Dwarf_Add_Line_Entry ( INT code_address, SRCPOS srcpos);

extern void Em_Dwarf_Add_Include (UINT16 incl_idx, const char *name);

extern void Em_Dwarf_Add_File (
    UINT16 file_idx,
    const char *name,
    UINT16 incl_idx,
    Dwarf_Unsigned modification_time,
    Dwarf_Unsigned file_size);


/* Given a file_index, return the filename and the path for the file. */
extern void Em_Dwarf_File_Index_To_Name (
    INT file_index, 
    char **filename, 
    char **path);

/* eh_offset should be -1 if no eh region */
extern void Em_Dwarf_Process_PU (Dwarf_Unsigned begin_label,
				 Dwarf_Unsigned end_label,
				 INT32          begin_offset,
				 INT32          end_offset,
				 Dwarf_P_Die    PU_die,
				 Dwarf_P_Fde    fde,
#ifdef KEY
				 Dwarf_P_Fde    eh_fde,
#endif
				 Elf64_Word     eh_symindex,
				 INT            eh_offset);
#ifdef TARG_X8664
/* To add FDEs for ALT ENTRY points for a PU (if any) */
extern void Em_Dwarf_Add_PU_Entries (Dwarf_Unsigned begin_label,
				     Dwarf_Unsigned end_label,
				     INT32          begin_offset,
				     INT32          end_offset,
				     Dwarf_P_Die    PU_die,
				     Dwarf_P_Fde    fde);
#endif

extern void Em_Dwarf_Start_Text_Region (pSCNINFO scninfo, INT start_offset);

extern void Em_Dwarf_Start_Text_Region_Semi_Symbolic (pSCNINFO,
						      INT,
						      Dwarf_Unsigned,
						      Dwarf_Addr);

extern void Em_Dwarf_End_Text_Region (pSCNINFO scninfo, INT end_offset);

extern void Em_Dwarf_End_Text_Region_Semi_Symbolic (pSCNINFO,
						    INT,
						    Dwarf_Unsigned,
						    Dwarf_Addr);
#if defined(__cplusplus)
}
#endif
#   ifdef TARG_ST
#       ifndef __cplusplus
#          error This file must be compiled in c++ mode
#       endif

#       include "mempool.h"           // For MEM_POOL definition
#       include "mempool_allocator.h" // For mempool_allocator definition
#       include "targ_isa_registers.h" // For ISA_REGISTER_CLASS definition
#       include "register.h"          // For REGISTER
#       include "tn.h"



#       if __GNUC__ >= 3
#           include <list>
using std::list;
#           include <map>
using std::map;
#           include <set>
using std::set;
#       else
#           include <list.h>
#           include <map.h>
#           include <set.h>
#       endif // __GNUC__ >= 3

// Uncomment this line
#       define DEBUG_CIE_EMIT

/**
 * Output use to print all debug trace.
 */
extern FILE* ciedbgOutput;

extern BOOL Trace_CIE;

// This piece of code was copied from targ_cgdwarf.cxx, I should create a
// "standard" trace library for all my developments
#       ifdef DEBUG_CIE_EMIT

/**
 * Print the given argument with a conditional call to fprintf.
 *
 * @param  args The arguments to be given to fprintf. Note that they must
 *         contain the parentheses
 *
 * @pre    true
 * @post   Trace_CIE implies fprintf has been called with args arguments
 *
 * @remarks This function is expansed in nothing unless the DEBUG_CIE_EMIT macro
 *          is defined
 */
#           define DbgPrintCIE(args) DbgCIE(fprintf, args)

/**
 * Conditionally assert the given argument.
 *
 * @param  args The arguments to be given to DevAssert. Note that they must
 *         contain the parentheses
 *
 * @pre    true
 * @post   Trace_CIE implies DevAssert has been called with args arguments
 *
 * @remarks This function is expansed in nothing unless the DEBUG_CIE_EMIT macro
 *          is defined
 */
#           define DbgAssertCIE(args) DbgCIE(DevAssert, args)

/**
 * Conditionally print a development warning with the given msg, if the check
 * condition is true.
 *
 * @param  check The condition to be checked
 * @param  msg The message to be printed in case of check argument is true
 *
 * @pre    true
 * @post   Trace_CIE and check implies DevWarn has been called with msg
 *
 * @remarks This function is expansed in nothing unless the DEBUG_CIE_EMIT macro
 *          is defined
 */
#           define DbgWarmCIE(check, msg) DbgCIE(if(check), DevWarn msg)

/**
 * Generic conditionnal 'action launcher' for debug purpose.
 *
 * @param  action Action to be performed. It should be a function name, the
 *         begin of a statement, etc. What is important is that action
 *         parameter followed by args parameter made a valid C/C++ statement.
 * @param  args Arguments to be given to the action
 *
 * @pre    true
 * @post   Trace_CIE implies action has been executed with args arguments.
 *
 * @remarks This function is expansed in nothing unless the DEBUG_CIE_EMIT macro
 *          is defined
 */
#           define DbgCIE(action, args) \
{ \
if(Trace_CIE) \
    { \
        action args; \
    } \
}
#       else
#           define DbgPrintCIE(args)
#           define DbgAssertCIE(args)
#           define DbgWarmCIE(check, msg)
#           define DbgCIE(action, args)
#       endif


/**
 * Type that represents a debug register identifier.
 * This type must have:
 * @li Have a copy constructor
 * @li Have an equal operator
 * @li Have a less operator
 * @li Have a cast operator to Dwarf_Unsigned type
 */
typedef Dwarf_Unsigned DebugRegId;

/**
 * Use to set a DebugRegId instance as an undefined value.
 */
extern const DebugRegId DebugRegId_undef;

// Forward declaration
struct PU;
class CCIEInfo;

//-----------------------------------------------------------------------------
// CIE initialization declarations
//-----------------------------------------------------------------------------
typedef unsigned char Byte;
typedef mempool_allocator<Byte> ByteAllocator;
typedef list<Byte, ByteAllocator> Bytes;
typedef Bytes::const_iterator CItBytes;

typedef map<Dwarf_Unsigned, CCIEInfo> CIEIndexToInfo;
typedef CIEIndexToInfo::const_iterator CItCIEIndexToInfo;
typedef CIEIndexToInfo::iterator ItCIEIndexToInfo;

/**
 * Contains the information of all emitted CIEs addressed by cie index
 */
extern CIEIndexToInfo CIEs;

/**
 * Getter for code alignment factor.
 * Return the code alignment factor used to represent debug information.
 *
 * @param  pu [in] Program unit
 *
 * @pre    true
 * @post   true
 *
 * @return The code alignment factor for given program unit
 *
 * @todo   Targeting for your target
 */
extern INT
CodeAlignmentFactor(PU& pu);

/**
 * Getter for data alignment factor.
 * Return the data alignment factor used to represent debug information.
 *
 * @param  pu [in] Program unit
 *
 * @pre    true
 * @post   true
 *
 * @return The data alignment factor for given program unit
 *
 * @todo   Targeting for your target
 */
extern INT
DataAlignmentFactor(PU& pu);

/**
 * Return a hash value for given program unit.
 * This hash value must be the same for two program units when they have the
 * same CIE information
 *
 * @param  pu Program unit
 *
 * @pre    true
 * @post   true
 *
 * @return A hash value that identify the kind of CIE for this program unit.
 *         I.e. two program units have the same CIE when the hash values are
 *         equal
 *
 * @todo   Targeting for your target
 */
extern INT
HashValue(PU& pu);

/**
 * Check whether given register is saved for given PU or not.
 * This information is used to initialize a CIE
 *
 * @param  a_id Debug identifier of the register
 * @param  regClass Register class of the register
 * @param  reg The register in regClass
 * @param  a_pu A program unit
 *
 * @pre    true
 * @post   true
 *
 * @return TRUE if a_id is a preserved register for a_pu, FALSE otherwise
 *
 * @todo   Targeting for your target
 */
extern BOOL
IsSaved(const DebugRegId& a_id, ISA_REGISTER_CLASS regClass, REGISTER reg,
        PU& a_pu);

/**
 * Specify whether a register class should be used to initialized a CIE.
 *
 * @param  register_class Register class to be checked
 *
 * @pre    true
 * @post   true
 *
 * @return TRUE if register_class callee saved, scratch register should be
 *         put in the CIE list, FALSE otherwise
 *
 * @todo   Targeting for your target
 */
extern BOOL
ShouldGenerateInformation(ISA_REGISTER_CLASS register_class, PU& pu);


/**
 * Get CFA definition for given program unit.
 *
 * @param  a_base [out] Id of the base for the cfa definition
 * @param  a_offset [out] offset used for cfa definition
 * @param  a_pu Program unit
 *
 * @pre    true
 * @post   true
 *
 * @todo   Targeting for your target
 */
extern void
CfaDef(DebugRegId& a_base, UINT& a_offset, PU& a_pu);

/**
 * Check whether given register has a special definition relatively to cfa
 * (DW_CFA_offset_extended).
 *
 * @param  a_id Debug identifier of the register
 * @param  offset Offset from CFA used for this special definition (used if
 *         result = TRUE)
 * @param  regClass Register class of the register
 * @param  reg Register in that register class
 * @param  a_pu Program unit
 *
 * @pre    true
 * @post   we use "offset" only when result = TRUE
 *
 * @return TRUE if given register has a special definition, FALSE otherwise
 *
 * @todo   Targeting for your target
 */
extern BOOL
HasSpecialDef(const DebugRegId& a_id, UINT& offset,
              ISA_REGISTER_CLASS regClass, REGISTER reg, PU& a_pu);

/**
 * Get the definition of the return address for given program unit.
 * If there is a special definition, i.e. a_id is addressed relatively to CFA
 * with offset, then this function returns TRUE. This special definition is
 * useful when return address register is virtual for instance
 *
 * @param  a_base [out] Id of the return address register
 * @param  a_offset [out] offset used from CFA to defined that register, if
 *         any (result = TRUE).
 * @param  a_pu Program unit
 *
 * @pre    true
 * @post   we use "offset" only when result = TRUE
 *
 * @return TRUE if given register has a special definition, FALSE otherwise
 */
extern BOOL
ReturnAddressDef(DebugRegId& a_id, UINT& offset, PU& a_pu);

/**
 * Get debug identifier for given register.
 * The debug identifier is a virtual representation of the hardware register.
 * This representation is understood by the debugger whereas hardware
 * identifier are not.
 *
 * @param  reg_class Register class of given reg
 * @param  reg Register to be translated
 * @param  bitSize Size in bit of the register
 *
 * @pre    true
 * @post   true
 *
 * @return Related debug identifier
 *
 * @todo   Targeting for your target
 */
extern DebugRegId
Get_Debug_Reg_Id(ISA_REGISTER_CLASS reg_class, REGISTER reg, INT bitSize);

/**
 * Same as Get_Debug_Reg_Id but from a CLASS_REG_PAIR.
 */
extern DebugRegId
Get_Debug_Reg_Id(const CLASS_REG_PAIR& regPair);

/**
 * Same as Get_Debug_Reg_Id but from a TN.
 */
extern DebugRegId
Get_Debug_Reg_Id(TN* tn);

/**
 * Get cie index for given program unit.
 * Returned cie index is a libdwarf cie index that can be used to bind a fde to
 * the related cie.
 *
 * @param  pu [in] Program unit for which you want the related cie
 *
 * @pre    Init_CIEs has been called
 * @post   CIEs[result] is set to the cie information
 *
 * @return An index to the libdwarf cie which represents given program unit
 *         CIE information
 *
 * @remarks A cie is created only when an equivalent one did not exists. I.e.
 *          two calls to this function with an equivalent program unit,
 *          return the same cie index.
 */
extern Dwarf_Unsigned
CIE_index(PU& pu);

/**
 * Get eh cie index for given program unit.
 * Returned eh cie index is a libdwarf cie index that can be used to bind a fde
 * to the related eh cie.
 *
 * @param  pu [in] Program unit for which you want the related eh cie
 *
 * @pre    Init_CIEs has been called
 * @post   CIEs[result] is set to related cie information
 *
 * @return An index to the libdwarf eh cie which represents given program unit
 *         CIE information for an EH frame
 *
 * @remarks A eh cie is created only when an equivalent one did not exists. I.e.
 *          two calls to this function with an equivalent program unit,
 *          return the same eh cie index.
 */
extern Dwarf_Unsigned
eh_CIE_index(PU& pu);

/**
 * Initialize CIEs global information.
 *
 * @param  dw_dbg [in] Global libdwarf debug information
 * @param  a_memPool [in] Memory pool used for all memory allocation done in CIE
 *         initialization process
 *
 * @pre    dw_dbg is a pointer on a valid libdwarf debug information and
 *         a_memPool is a pointer to a valid and initialiazed memory pool
 * @post   CIEs API can be called
 *
 */
extern void
Init_CIEs(Dwarf_P_Debug dw_dbg, MEM_POOL* a_memPool);

/**
 * Clear CIEs global information.
 *
 * @param  a_memPool [in/out] Memory pool used for all memory allocation done in
 *         CIE initialization process
 *
 * @pre    a_memPool is the same memory pool used at Init_CIEs time and all
 *         emitted initial bytes (with CCIEInfo::InitialBytes method) can be
 *         freed
 * @post   Resource used by CIE initialization process released
 *
 */
extern void
Clear_CIEs(MEM_POOL* a_memPool);


//------------------------------------------------------------------------------
// CIE initialization class declaration
//------------------------------------------------------------------------------

/**
 * @class CCIEInfo
 * Represents a CIE information and more particularly the inital bytes. It
 * supplies a convenient interface to know whether a register is saved
 * (DW_CFA_same_value) or not (DW_CFA_undefined) for the related CIE.
 */
class CCIEInfo
{
    // Private shortcut definition
    typedef list<unsigned char*> ListOfBytes;
    typedef ListOfBytes::const_iterator CItListOfBytes;
    typedef ListOfBytes::iterator ItListOfBytes;

 public:
    /**
     * Structure used to defined the comparison function needed for CCIEInfo
     * m_saved member definition. Even if the following definition is trivial,
     * i.e. we could keep the default one defined in the stl, it is requiered,
     * since we want to set the memory allocator
     */
    struct LessThanDebugRegId
    {
        /**
         * Debug register identifier comparison function.
         *
         * @param  a Left hand side of the comparison
         * @param  b Right hand side of the comparison
         *
         * @pre    true
         * @post   result = a < b
         *
         * @return true if a is less than b, false otherwise
         */
        bool
        operator()(const DebugRegId& a, const DebugRegId& b) const
        {
            return a < b;
        }
    };

    // Shortcut definition
    typedef mempool_allocator<DebugRegId> DebugRegIdAllocator;
    typedef set<DebugRegId, LessThanDebugRegId, DebugRegIdAllocator> SavedList;
    typedef SavedList::const_iterator CItSavedList;
    typedef SavedList::iterator ItSavedList;

    /**
     * Default constructor.
     * This constructor is used to initialize dummy object and is requiered for
     * stl map initialization used by CIEs global variable
     *
     * @pre    true
     * @post   true
     *
     * @warning A object initialized with that constructor must not be used
     */
    CCIEInfo()
    {}

    /**
     * Recommended constructor.
     * Creates CIE information for given program unit
     *
     * @param  a_pu [in] Program unit for which the CIE information are created
     *
     * @pre    Init_CIEs has been called
     * @post   InitBytes() contains CIE initial bytes and Saved() contains the
     *         debug register identifiers of saved registers
     *
     */
    CCIEInfo(PU& a_pu);

    /**
     * Copy constructor.
     * Initialized a new object with the value of the members of given object
     *
     * @param  a_cieInfo Object to be copied
     *
     * @pre    Init_CIEs has been called
     * @post   InitBytes() = a_cieInfo.InitBytes() and Saved() =
     *         a_cieInfo.Saved() and ReturnAddressRegId() =
     *         a_cieInfo.ReturnAddressRegId()
     *
     */
    CCIEInfo(const CCIEInfo& a_cieInfo);

    /**
     * Assignment operator.
     * Copy value of rhs' members to lhs one
     *
     * @param  a_cieInfo Object to be copied
     *
     * @pre    true
     * @post   InitBytes() = a_cieInfo.InitBytes() and Saved() =
     *         a_cieInfo.Saved() and ReturnAddressRegId() =
     *         a_cieInfo.ReturnAddressRegId()
     *
     * @return A reference to lhs
     */
    CCIEInfo&
    operator=(const CCIEInfo& a_cieInfo);

    /**
     * Desctructor.
     *
     * @pre    true
     * @post   true
     *
     */
    ~CCIEInfo();

    /**
     * Translate InitBytes() in a more covenient C representation.
     *
     * @param  sizeOfReturnedTab [out] Will contain the length of the returned
     *         array
     *
     * @pre    true
     * @post   result->length() = sizeOfReturnedTab and result->oclIsNew() and
     *         m_listOfInitialBytes = m_listOfInitialBytes@pre->append(result)
     *
     * @return A new allocated array that contains the initial cie bytes
     *
     * @remarks Allocated memory is freed at ReleaseListOfInitialBytes call
     *          time, which is called by Clear_CIEs function. Therefore, be sure
     *          to need not this array after the call to Clear_CIEs or made a
     *          copy of it
     */
    unsigned char*
    InitialBytes(Dwarf_Unsigned& sizeOfReturnedTab) const;

    /**
     * Getter for m_initBytes member.
     *
     * @pre    true
     * @post   true
     *
     * @return A constant reference to m_initBytes member
     *
     * @see m_initBytes member for more details
     */
    const Bytes&
    InitBytes() const;

    /**
     * Getter for m_cfaOffset member.
     *
     * @pre    true
     * @post   result = m_cfaOffset
     *
     * @return A copy of m_cfaOffset member
     *
     * @see m_cfaOffset member for more details
     */
    UINT
    CfaOffset() const;

    /**
     * Check whether given register identifier is in the list of saved registers
     * or not for current CIE information.
     *
     * @param  a_debugRegId Debug register identifier to be checked
     *
     * @pre    true
     * @post   result = a_debugRegId is member of Saved()
     *
     * @return TRUE if a_debugRegId is considered as a saved register in cie
     *         information (DW_CFA_same_value), FALSE otherwise
     */
    BOOL
    IsSaved(const DebugRegId& a_debugRegId) const;

    /**
     * Setter on global memory pool used for all dynamic allocation in CCIEInfo
     * object.
     *
     * @param  a_mempool [in] Memory pool used to set global memory allocation
     *         for CCIEInfo objects
     *
     * @pre    true
     * @post   m_memPool = a_mempool
     *
     */
    static void
    MemPool(MEM_POOL* a_mempool);

    /**
     * Getter on global memory pool used for all dynamic allocation in CCIEInfo
     * object.
     *
     * @pre    true
     * @post   true
     *
     * @return A copy of m_memPool member
     */
    static MEM_POOL*
    MemPool();

    /**
     * Getter for m_returnAddressRegId member.
     *
     * @pre    true
     * @post   true
     *
     * @return A constant reference to m_returnAddressRegId member
     *
     * @see m_returnAddressRegId member for more details
     */
    const DebugRegId&
    ReturnAddressRegId() const;

    /**
     * Release all initial bytes resources.
     * Initial bytes resources are the ones allocated by a call to InitialBytes
     * method (do not confuse with InitBytes method). This method releases all
     * memory resources pointed by pointer returned by InitalBytes method
     *
     * @pre    true
     * @post   m_listOfInitialBytes->empty() and m_listOfInitialBytes@pre->
     *         forall(pointer | memory pointed by pointer has been released)
     *
     */
    static void
    ReleaseListOfInitialBytes();

    /**
     * Getter for m_saved member.
     *
     * @pre    true
     * @post   true
     *
     * @return A constant reference to m_saved member
     *
     * @see m_saved member for more details
     */
    const SavedList&
    Saved() const;

 protected:

    /**
     * Copy members value of given object in this.
     *
     * @param  a_cieInfo Object to be copied
     *
     * @pre    true
     * @post   InitBytes() = a_cieInfo.InitBytes() and Saved() =
     *         a_cieInfo.Saved() and ReturnAddressRegId() =
     *         a_cieInfo.ReturnAddressRegId()
     *
     */
    void
    CopyMembers(const CCIEInfo& a_cieInfo);

    /**
     * Build the CIE list of dwarf instructions.
     * This function traverses all register classes and appends a
     * DW_CFA_same_value for callee saved registers and a DW_CFA_undefined for
     * the others to the given list of bytes.
     *
     * @param  pu [in] Reference to a program unit. The PU is needed to
     *         know which type of function we are dealing with. (interrupt or
     *         not)
     *
     * @pre    true
     * @post   m_initBytes contains the CIE list of dwarf instruction and
     *         m_saved the list of saved register
     *
     */
    void
    BuildCIEBytesList(PU& pu);

    /**
     * Emit given regId.
     * The function also checks the status of regId to emit the right debug
     * information.
     *
     * @param  regId Debug identifier of the register to be emitted
     * @param  register_class Register class of the register represented by
     *         regId
     * @param  i Index of the register represented by regId in register_class
     * @param  pu Current program unit
     *
     * @pre    true
     * @post   m_initBytes contains the dwarf intruction related to the
     *         location of regId
     */
    void
    EmitReg(const DebugRegId& regId, ISA_REGISTER_CLASS register_class,
            REGISTER i, PU& pu);

    /**
     * Translate given integer into dwarf2 LEB128 representation.
     * To have more details on used algorithm, see dwarf 2 norm appendix 4
     *
     * @param  a_regId Register identifier to be translated
     *
     * @pre    true
     * @post   ULEB128 bytes that represent regId has been appended to
     *         m_initBytes
     *
     */
    void
    EmitBytes(const DebugRegId& regId);

    /**
     * Translate a list of bytes in an array of bytes.
     *
     * @param  init_bytes The list of bytes
     * @param  mempool Memory pool used to allocate the returned array of bytes
     *
     * @pre    true
     * @post   result must be freed with a call to CXX_DELETE_ARRAY and
     *         result->size() = init_bytes->size()
     *
     * @return The translated array
     */
    static Byte*
    TranslateBytesList(const Bytes& init_bytes, MEM_POOL* mempool);

    /**
     * Add given register identifier to m_saved member.
     *
     * @param  a_debugRegId Debug register identifier to be added
     *
     * @pre    true
     * @post   m_saved->includes(a_debugRegId)
     *
     */
    void
    AddToSaved(const DebugRegId& a_debugRegId);

    /**
     * Global memory pool used for all CCIEInfo objects
     */
    static MEM_POOL* m_memPool;

    /**
     * Global list of allocated initial bytes (by InitalBytes method)
     */
    static ListOfBytes m_listOfInitialBytes;

    /**
     * Initial CIE bytes
     */
    Bytes m_initBytes;

    /**
     * Debug register identifier of CIE return address
     */
    DebugRegId m_returnAddressRegId;

    /**
     * List of CIE saved registers
     */
    SavedList m_saved;

    /**
     * CFA offset used in def_cfa
     */
    UINT m_cfaOffset;
};

//------------------------------------------------------------------------------
// Debug register identifier handling
//------------------------------------------------------------------------------

/**
 * @class CExtensionAndRegister
 * A CExtensionAndRegister represents a couple: extension (slot number) and
 * register. It contains the rules to convert a dwarf operand (in host
 * form, not in LEB128) in this couple and vice-versa.
 */
class CExtensionAndRegister
{
 public:

    /**
     * Encoding constructor.
     * Create a dwarf operand in host encoding from given triplet. We need
     * a triplet instead of the defined couple (slot, register) to know whether
     * the couple (0, register) defined a core register or a register at
     * extension on slot 0.
     *
     * @param  a_isExtension Specify whether a_extension parameter is meaningful
     *         or not
     * @param  a_register Dwarf register identifier
     * @param  a_extension Slot number of the extension
     *
     * @pre    true
     * @post   Operand() contains the dwarf operand that represents the couple
     *
     */
    CExtensionAndRegister(BOOL a_isExtension, UINT a_register,
                          UINT a_extension = 0)
        : m_extension(a_extension), m_register(a_register),
        m_isExtension(a_isExtension)
    {
        Encoding();
    }
    
    /**
     * Decoding constructor.
     * Create the couple (slot, register) for given dwarf operand. It also sets
     * the IsExtension() value
     *
     * @param  a_operand Dwarf operand that represents a couple (slot, register)
     *         encoded by the "encoding constructor"
     *
     * @pre    true
     * @post   IsExtension() and Register() and Extension() are set to the
     *         decoded values.
     *
     */
    CExtensionAndRegister(Dwarf_Unsigned a_operand)
        : m_operand(a_operand)
    {
        Decoding();
    }

    /**
     * Copy constructor.
     * Initialize all members with members' values of given class.
     *
     * @param  a_extAndReg Object to be copied
     *
     * @pre    true
     * @post   Register() = a_extAndReg.Register() and
     *         Extension() = a_extAndReg.Extension() and
     *         IsExtension() = a_extAndReg.IsExtension() and
     *         Operand() = a_extAndReg.Operand()
     *
     */
    CExtensionAndRegister(const CExtensionAndRegister& a_extAndReg)
    {
        CopyMembers(a_extAndReg);
    }

    /**
     * operator=.
     * Set all members with members' values of given class.
     *
     * @param  a_extAndReg Object to be copied
     *
     * @pre    true
     * @post   Register() = a_extAndReg.Register() and
     *         Extension() = a_extAndReg.Extension() and
     *         IsExtension() = a_extAndReg.IsExtension() and
     *         Operand() = a_extAndReg.Operand()
     *
     * @return The left hand side operand
     */
    CExtensionAndRegister&
        operator=(const CExtensionAndRegister& a_extAndReg)
    {
        if(this != &a_extAndReg)
            {
                CopyMembers(a_extAndReg);
            }
        return *this;
    }
    
    /**
     * Getter for m_extension member.
     *
     * @pre    Returned value is meaningless unless IsExtension() = true
     * @post   Returned value is meaningless unless IsExtension() = true
     *
     * @return The value of m_extension member
     */
    UINT
        Extension() const
    {
        return m_extension;
    }

    /**
     * Getter for m_register member.
     *
     * @pre    true
     * @post   true
     *
     * @return The value of m_register member
     */    
    UINT
        Register() const
    {
        return m_register;
    }
    
    /**
     * Getter for m_isExtension member.
     *
     * @pre    true
     * @post   true
     *
     * @return The value of m_isExtension member
     */
    BOOL
        IsExtension() const
    {
        return m_isExtension;
    }
    
    /**
     * Getter for m_operand member.
     *
     * @pre    true
     * @post   true
     *
     * @return The value of m_operand member
     */
    Dwarf_Unsigned
        Operand() const
    {
        return m_operand;
    }
    
 protected:

    /**
     * Decode current operand and set couple information.
     *
     * @pre    true
     * @post   IsExtension() and Extension() and Operand() are set
     *
     */    
    void
        Decoding()
    {
        m_extension = (UINT)((Operand() >> MASK_SHIFT_SIZE) &
                             EXTENSION_MASK);
        m_register = Operand() & ~(EXTENSION_MASK << MASK_SHIFT_SIZE);
        m_isExtension = m_extension != 0;
        // Slot number of the extension is given value minus 1.
        // It is done this way to distinguish core and extension at slot 0.
        // Note: m_extension is meaningless unless m_isExtension is true. So,
        // we have not to control the result of the next statement.
        --m_extension;
    }
    
    /**
     * Encode couple information in the operand.
     *
     * @pre    true
     * @post   Operand() contains the encoded couple information.
     *
     */
    void
        Encoding()
    {
        // We add 1 to the extension slot to distinguish core and
        // extension at slot 0
        Dwarf_Unsigned extension = IsExtension()?
            (Extension() + 1) & EXTENSION_MASK: 0;
        m_operand = (extension << MASK_SHIFT_SIZE) | Register();
    }

    /**
     * Copy members' values of given object.
     * Purpose is to have same value members between the two objects
     *
     * @param  a_extAndReg Object to be copied
     *
     * @pre    true
     * @post   Register() = a_extAndReg.Register() and
     *         Extension() = a_extAndReg.Extension() and
     *         IsExtension() = a_extAndReg.IsExtension() and
     *         Operand() = a_extAndReg.Operand()
     *
     */
    void
        CopyMembers(const CExtensionAndRegister& a_extAndReg)
    {
        m_extension = a_extAndReg.Extension();
        m_register = a_extAndReg.Register();
        m_isExtension = a_extAndReg.IsExtension();
        m_operand = a_extAndReg.Operand();
    }

    /**
     * Global class member used to defined the mask used to decode/encode the
     * extension slot.
     * Definition is in cgdwarf.cxx
     */
    static const Dwarf_Unsigned EXTENSION_MASK;

    /**
     * Global class member used to defined where, in the operand, the extension
     * slot can be found with a left shift.
     * Definition is in cgdwarf.cxx
     */
    static const UINT MASK_SHIFT_SIZE;

    /**
     * Slot number of an extension. This member is meaningless unless
     * IsExtension() is true
     */
    UINT m_extension;

    /**
     * Dwarf register identifier in current extension
     */
    Dwarf_Unsigned m_register;

    /**
     * Specify whether Extension() value is meaningful (true) or not (false).
     * IsExtension() is false implies current represented register is a core one
     */
    BOOL m_isExtension;

    /**
     * Dwarf operand that represents the couple (slot, register).
     */
    Dwarf_Unsigned m_operand;
};

#   endif /* TARG_ST */

#endif /* em_dwarf_INCLUDED */
