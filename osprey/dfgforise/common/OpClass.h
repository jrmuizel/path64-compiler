/* 

  Copyright (C) 2008 ST Microelectronics, Inc.  All Rights Reserved. 

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
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
////                                                                             ////
////    Declaration of OpClass types.                                            ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file OpClass.h
 */


#ifndef OpClass_h
#define OpClass_h



namespace DfgForIse {



  /** Enumeration of the op classes used in the DFG's representation.
   */
  enum OpClass {
    // Forbidden operations in selection
    OPCLASS_FORBIDDEN,
    OPCLASS_CALL,
    OPCLASS_COND,
    OPCLASS_DIV,
    OPCLASS_IJUMP,
    OPCLASS_LOAD,
    OPCLASS_MAX,
    OPCLASS_MIN,
    OPCLASS_STORE,

    // Authorized Logical : & | ! ^
    OPCLASS_AND,
    OPCLASS_OR,
    OPCLASS_NOT,
    OPCLASS_XOR,

    // Authorized Add Sub : + -
    OPCLASS_ADD,
    OPCLASS_SUB,

    // Authorized Mult : *
    OPCLASS_MUL,

    // Authorized Shifts : << >>
    OPCLASS_SHL,
    OPCLASS_SHR,

    // Authorized Compare : == != > >= < <=
    OPCLASS_CMP,

    // Other operations (authorized or not ?) :
    // MAKE, abs, barrier, bclr, bkp, bnot, bool, bset, clamph, ext, lzc, max, min,
    // move, neg, norn, org, rotl, setlc, setle, setls, tfand, tfbit, ttand, ttbit.
    
    // Other authorized operations in selection
    OPCLASS_MAKE,
    OPCLASS_OTHER,

    OPCLASS_NUMBER
  };


  /** Indicates the name of each opclass.
   */
  const char* const OpClassName[] = {
    // Forbidden operations in selection
    "FORBIDDEN",// OPCLASS_FORBIDDEN
    "CALL",	// OPCLASS_CALL
    "COND",	// OPCLASS_COND
    "DIV",	// OPCLASS_DIV
    "IJUMP",	// OPCLASS_IJUMP
    "LOAD",	// OPCLASS_LOAD
    "MAX",	// OPCLASS_MAX
    "MIN",	// OPCLASS_MIN
    "STORE",	// OPCLASS_STORE

    // Authorized Logical : & | ! ^
    "AND",	// OPCLASS_AND
    "OR",	// OPCLASS_OR
    "NOT",	// OPCLASS_NOT
    "XOR",	// OPCLASS_XOR

    // Authorized Add Sub : + -
    "ADD",	// OPCLASS_ADD
    "SUB",	// OPCLASS_SUB

    // Authorized Mult : *
    "MUL",	// OPCLASS_MUL

    // Authorized Shifts : << >>
    "SHL",	// OPCLASS_SHL
    "SHR",	// OPCLASS_SHR

    // Authorized Compare : == != > >= < <=
    "CMP",	// OPCLASS_CMP

    // Other authorized operations in selection
    "MAKE",	// OPCLASS_MAKE
    "OTHER"	// OPCLASS_OTHER
  };



  /** Indicates the string to dump for each opclass.
   */
  const char* const OpClassDump[] = {
    // Forbidden operations in selection
    "",	// OPCLASS_FORBIDDEN
    "call",	// OPCLASS_CALL
    "cond",	// OPCLASS_COND
    "/",	// OPCLASS_DIV
    "ijump",	// OPCLASS_IJUMP
    "load",	// OPCLASS_LOAD
    "max",	// OPCLASS_MAX
    "min",	// OPCLASS_MIN
    "store",	// OPCLASS_STORE

    // Authorized Logical : & | ! ^
    "&",	// OPCLASS_AND
    "|",	// OPCLASS_OR
    "!",	// OPCLASS_NOT
    "^",	// OPCLASS_XOR

    // Authorized Add Sub : + -
    "+",	// OPCLASS_ADD
    "-",	// OPCLASS_SUB

    // Authorized Mult : *
    "*",	// OPCLASS_MUL

    // Authorized Shifts : << >>
    "<<",	// OPCLASS_SHL
    ">>",	// OPCLASS_SHR

    // Authorized Compare : == != > >= < <=
    "comp",	// OPCLASS_CMP

    // Other authorized operations in selection
    "MAKE",	// OPCLASS_MAKE
    ""	// OPCLASS_OTHER
  };



  /** Indicates for each opclass if it is forbidden in selection.
   */
  const bool OpClassForbiddenInSelection[] = {
    // Forbidden operations in selection
    true,	// OPCLASS_FORBIDDEN
    true,	// OPCLASS_CALL
    true,	// OPCLASS_COND
    true,	// OPCLASS_DIV
    true,	// OPCLASS_IJUMP
    true,	// OPCLASS_LOAD
    true,	// OPCLASS_MAX
    true,	// OPCLASS_MIN
    true,	// OPCLASS_STORE

    // Authorized Logical : & | ! ^
    false,	// OPCLASS_AND
    false,	// OPCLASS_OR
    false,	// OPCLASS_NOT
    false,	// OPCLASS_XOR

    // Authorized Add Sub : + -
    false,	// OPCLASS_ADD
    false,	// OPCLASS_SUB

    // Authorized Mult : *
    false,	// OPCLASS_MUL

    // Authorized Shifts : << >>
    false,	// OPCLASS_SHL
    false,	// OPCLASS_SHR

    // Authorized Compare : == != > >= < <=
    false,	// OPCLASS_CMP

    // Other authorized operations in selection
    false,	// OPCLASS_MAKE
    false	// OPCLASS_OTHER
  };



  /** Enumeration of the CMP variants.
   */
  enum CmpVariant {
    CMPVARIANT_NONE,
    CMPVARIANT_EQ,
    CMPVARIANT_NE,
    CMPVARIANT_GT,
    CMPVARIANT_GTU,
    CMPVARIANT_GE,
    CMPVARIANT_GEU,
    CMPVARIANT_LT,
    CMPVARIANT_LTU,
    CMPVARIANT_LE,
    CMPVARIANT_LEU,
    CMPVARIANT_ANDL,
    CMPVARIANT_NANDL,
    CMPVARIANT_ORL,
    CMPVARIANT_NORL,
    CMPVARIANT_NUMBER
  };



  /** Indicates the name of each cmp variant.
   */
  const char* const CmpVariantName[] = {
    "NONE",	// CMPVARIANT_NONE
    "EQ",	// CMPVARIANT_EQ
    "NE",	// CMPVARIANT_NE
    "GT",	// CMPVARIANT_GT
    "GTU",	// CMPVARIANT_GTU
    "GE",	// CMPVARIANT_GE
    "GEU",	// CMPVARIANT_GEU
    "LT",	// CMPVARIANT_LT
    "LTU",	// CMPVARIANT_LTU
    "LE",	// CMPVARIANT_LE
    "LEU",	// CMPVARIANT_LEU
    "ANDL",	// CMPVARIANT_ANDL
    "NANDL",	// CMPVARIANT_NANDL
    "ORL",	// CMPVARIANT_ORL
    "NORL"	// CMPVARIANT_NORL
  };


  /** Indicates the string to dump for each cmp variant.
   */
  const char* const CmpVariantDump[] = {
    "CMP",     	// CMPVARIANT_NONE
    "==",	// CMPVARIANT_EQ
    "!=",	// CMPVARIANT_NE
    ">",	// CMPVARIANT_GT
    ">",	// CMPVARIANT_GTU
    ">=",	// CMPVARIANT_GE
    ">=",	// CMPVARIANT_GEU
    "<",	// CMPVARIANT_LT
    "<",	// CMPVARIANT_LTU
    "<=",	// CMPVARIANT_LE
    "<=",	// CMPVARIANT_LEU
    "andl",	// CMPVARIANT_ANDL
    "nandl",	// CMPVARIANT_NANDL
    "orl",	// CMPVARIANT_ORL
    "norl"	// CMPVARIANT_NORL
  };



};

#endif // OpClass_h
