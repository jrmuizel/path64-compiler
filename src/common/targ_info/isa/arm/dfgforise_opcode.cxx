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
////    Generation of DfgForIse_Opcode.h.                                        ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#include "dfgforise_opcode_gen.h"
#include "topcode.h"
#include "targ_isa_properties.h"

int main() {

  TOP topcode;

  for (topcode = 0;
       topcode < TOP_count;
       topcode++) {


    if ( TOP_is_call(topcode) ) {
      AssociateTopToClass(topcode, "CALL");
    }

    if ( TOP_is_cond(topcode) ) {
      AssociateTopToClass(topcode, "COND");
    }

    if ( TOP_is_div(topcode) ) {
      AssociateTopToClass(topcode, "DIV");
    }

    if ( TOP_is_ijump(topcode) ) {
      AssociateTopToClass(topcode, "IJUMP");
    }

    if ( TOP_is_load(topcode) ) {
      AssociateTopToClass(topcode, "LOAD");
    }

    if ( TOP_is_max(topcode) ) {
      AssociateTopToClass(topcode, "MAX");
    }

    if ( TOP_is_min(topcode) ) {
      AssociateTopToClass(topcode, "MIN");
    }

    if ( TOP_is_store(topcode) ) {
      AssociateTopToClass(topcode, "STORE");
    }

    if ( TOP_is_and(topcode) ) {
      AssociateTopToClass(topcode, "AND");
    }

    else if ( TOP_is_or(topcode) ) {
      AssociateTopToClass(topcode, "OR");
    }

    else if ( TOP_is_xor(topcode) ) {
      AssociateTopToClass(topcode, "XOR");
    }

    else if ( TOP_is_add(topcode) ) {
      AssociateTopToClass(topcode, "ADD");
    }

    else if ( TOP_is_sub(topcode) ) {
      AssociateTopToClass(topcode, "SUB");
    }

    else if ( TOP_is_mul(topcode) ) {
      AssociateTopToClass(topcode, "MUL");
    }

    else if ( TOP_is_shl(topcode) ) {
      AssociateTopToClass(topcode, "SHL");
    }

    else if ( TOP_is_shr(topcode)
	      || TOP_is_shru(topcode) ) {
      AssociateTopToClass(topcode, "SHR");
    }

    else if ( TOP_is_cmp(topcode) ) {
      AssociateTopToClass(topcode, "CMP");
    }

  }

  DfgForIse_OpCode_Create();

  return 0;
}
