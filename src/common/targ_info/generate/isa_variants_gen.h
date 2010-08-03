/*

  Copyright (C) 2002, 2006 ST Microelectronics, Inc.  All Rights Reserved. 

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


// isa_variants_gen.h
/////////////////////////////////////
//
//  Interface for specifying variants of instructions in the ISA.
//
//  An instruction variant can be seen as a directed and labeled 
//  link between an instruction and another. The label is a
//  combination of variant attributes. The available query is
//  then to retrieve from an instruction the instruction variant
//  that matches all the given variant attributes.
//  
//  Properties of variants:
//  - for a given combination of variant attributes, a single instruction 
//  as only one instruction matching the variant attributes.
//  - the maximum number of variant attributes is 64.
//  - variant attributes can be combined by OR-ing.
//
//  void ISA_Variants_Begin( const char* archname )
//      Initialize to generate variants information for the architecture 
//      with the given <archname>.  The information will be written to the 
//      files targ_isa_variants.[ch].  
//
//  TYPE ISA_VARIANT_ATT
//      An abstract type that represents a variant attribute.
//
//  ISA_VARIANT_ATT ISA_Variant_Attribute_Create( const char* name )
//      Used to create a new ISA_VARIANT_ATT.  <name> is the variant attribute name.
//      It will be used to label a variant link between two instructions.
//
//  void ISA_Instruction_Variant( TOP top_src, TOP top_variant, ISA_VARIANT_ATT att1, ... )
//      Defined the instruction top_variant as the variant of top_src matching all the
//      variant attributes given. One or more variant attributes can be given. 
//
//  void ISA_Variants_End(void)
//      Complete processing of variant.
//
//
/////////////////////////////////////



typedef struct isa_variant_attribute_t *ISA_VARIANT_ATT;

extern void ISA_Variants_Begin( const char* archname );
extern ISA_VARIANT_ATT ISA_Variant_Attribute_Create( const char* name );
extern void ISA_Instruction_Variant( TOP src, TOP variant, ISA_VARIANT_ATT att1, ... );
extern void ISA_Variants_End(void);
