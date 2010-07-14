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


/* ====================================================================
 * ====================================================================
 *
 * Module: variants.cxx
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Description:
 *
 * Functions related to OP variant field contents.
 *
 * ====================================================================
 * ====================================================================
 */

#ifdef _KEEP_RCS_ID
static const char rcs_id[] = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#include "defs.h"
#include "config.h"
#include "glob.h"
#include "erglob.h"
#include "erbe.h"
#include "tracing.h"

#include "variants.h"

/* ====================================================================
 *
 * Negate_BR_Variant
 *
 * See interface description.
 *
 * ====================================================================
 */
VARIANT
Negate_BR_Variant(VARIANT variant)
{
  BOOL is_false = V_false_br(variant);

  switch (V_br_condition(variant)) {
  case V_BR_NONE: break;

  case V_BR_I8EQ0: variant = V_BR_I8NE0; break;
  case V_BR_I8NE0: variant = V_BR_I8EQ0; break;
  case V_BR_I8GT0: variant = V_BR_I8LE0; break;
  case V_BR_I8GE0: variant = V_BR_I8LT0; break;
  case V_BR_I8LT0: variant = V_BR_I8GE0; break;
  case V_BR_I8LE0: variant = V_BR_I8GT0; break;

  case V_BR_I4EQ0: variant = V_BR_I4NE0; break;
  case V_BR_I4NE0: variant = V_BR_I4EQ0; break;
  case V_BR_I4GT0: variant = V_BR_I4LE0; break;
  case V_BR_I4GE0: variant = V_BR_I4LT0; break;
  case V_BR_I4LT0: variant = V_BR_I4GE0; break;
  case V_BR_I4LE0: variant = V_BR_I4GT0; break;

  case V_BR_I8EQ: variant = V_BR_I8NE; break;
  case V_BR_I8NE: variant = V_BR_I8EQ; break;
  case V_BR_I8GT: variant = V_BR_I8LE; break;
  case V_BR_I8GE: variant = V_BR_I8LT; break;
  case V_BR_I8LT: variant = V_BR_I8GE; break;
  case V_BR_I8LE: variant = V_BR_I8GT; break;

  case V_BR_U8EQ0: variant = V_BR_U8NE0; break;
  case V_BR_U8NE0: variant = V_BR_U8EQ0; break;
  case V_BR_U8GT0: variant = V_BR_U8LE0; break;
  case V_BR_U8GE0: variant = V_BR_U8LT0; break;
  case V_BR_U8LT0: variant = V_BR_U8GE0; break;
  case V_BR_U8LE0: variant = V_BR_U8GT0; break;

  case V_BR_U4EQ0: variant = V_BR_U4NE0; break;
  case V_BR_U4NE0: variant = V_BR_U4EQ0; break;
  case V_BR_U4GT0: variant = V_BR_U4LE0; break;
  case V_BR_U4GE0: variant = V_BR_U4LT0; break;
  case V_BR_U4LT0: variant = V_BR_U4GE0; break;
  case V_BR_U4LE0: variant = V_BR_U4GT0; break;

  case V_BR_U8EQ: variant = V_BR_U8NE; break;
  case V_BR_U8NE: variant = V_BR_U8EQ; break;
  case V_BR_U8GT: variant = V_BR_U8LE; break;
  case V_BR_U8GE: variant = V_BR_U8LT; break;
  case V_BR_U8LT: variant = V_BR_U8GE; break;
  case V_BR_U8LE: variant = V_BR_U8GT; break;
#ifdef TARG_ST 
  case V_BR_A8EQ: variant = V_BR_A8NE; break;
  case V_BR_A8NE: variant = V_BR_A8EQ; break;
  case V_BR_A8GT: variant = V_BR_A8LE; break;
  case V_BR_A8GE: variant = V_BR_A8LT; break;
  case V_BR_A8LT: variant = V_BR_A8GE; break;
  case V_BR_A8LE: variant = V_BR_A8GT; break;
#endif
  case V_BR_I4EQ: variant = V_BR_I4NE; break;
  case V_BR_I4NE: variant = V_BR_I4EQ; break;
  case V_BR_I4GT: variant = V_BR_I4LE; break;
  case V_BR_I4GE: variant = V_BR_I4LT; break;
  case V_BR_I4LT: variant = V_BR_I4GE; break;
  case V_BR_I4LE: variant = V_BR_I4GT; break;

  case V_BR_U4EQ: variant = V_BR_U4NE; break;
  case V_BR_U4NE: variant = V_BR_U4EQ; break;
  case V_BR_U4GT: variant = V_BR_U4LE; break;
  case V_BR_U4GE: variant = V_BR_U4LT; break;
  case V_BR_U4LT: variant = V_BR_U4GE; break;
  case V_BR_U4LE: variant = V_BR_U4GT; break;
#ifdef TARG_ST
  case V_BR_I5EQ: variant = V_BR_I5NE; break;
  case V_BR_I5NE: variant = V_BR_I5EQ; break;
  case V_BR_I5GT: variant = V_BR_I5LE; break;
  case V_BR_I5GE: variant = V_BR_I5LT; break;
  case V_BR_I5LT: variant = V_BR_I5GE; break;
  case V_BR_I5LE: variant = V_BR_I5GT; break;
  case V_BR_U5EQ: variant = V_BR_U5NE; break;
  case V_BR_U5NE: variant = V_BR_U5EQ; break;
  case V_BR_U5GT: variant = V_BR_U5LE; break;
  case V_BR_U5GE: variant = V_BR_U5LT; break;
  case V_BR_U5LT: variant = V_BR_U5GE; break;
  case V_BR_U5LE: variant = V_BR_U5GT; break;
  case V_BR_A4EQ: variant = V_BR_A4NE; break;
  case V_BR_A4NE: variant = V_BR_A4EQ; break;
  case V_BR_A4GT: variant = V_BR_A4LE; break;
  case V_BR_A4GE: variant = V_BR_A4LT; break;
  case V_BR_A4LT: variant = V_BR_A4GE; break;
  case V_BR_A4LE: variant = V_BR_A4GT; break;
#endif 
  case V_BR_F_FALSE: variant = V_BR_F_TRUE;  break;
  case V_BR_F_TRUE:  variant = V_BR_F_FALSE; break;

  case V_BR_PEQ: variant = V_BR_PNE; break;
  case V_BR_PNE: variant = V_BR_PEQ; break;

  case V_BR_FEQ:
  case V_BR_FNE:
  case V_BR_FGT:
  case V_BR_FGE:
  case V_BR_FLT:
  case V_BR_FLE:

  case V_BR_DEQ:
  case V_BR_DNE:
  case V_BR_DGT:
  case V_BR_DGE:
  case V_BR_DLT:
  case V_BR_DLE:

  case V_BR_QEQ:
  case V_BR_QNE:
  case V_BR_QGT:
  case V_BR_QGE:
  case V_BR_QLT:
  case V_BR_QLE:
    // changing the variant will effect the comparison if NaNs are
    // involved, so just flip the sense.
    is_false = !is_false;
    break;

  case V_BR_P_TRUE:
  case V_BR_CLOOP:
  case V_BR_CTOP:
  case V_BR_CEXIT:
  case V_BR_WTOP:
  case V_BR_WEXIT:
    // no inverse condition -- invert the sense
    is_false = !is_false;
    break;

  case V_BR_ALWAYS: variant = V_BR_NEVER;  break;
  case V_BR_NEVER:  variant = V_BR_ALWAYS; break;

  default:
    #pragma mips_frequency_hint NEVER
    FmtAssert(FALSE, ("unexpected variant %" SCNd64 "", variant));
    /*NOTREACHED*/
  }

  // set the sense.
  if (is_false) {
    Set_V_false_br(variant);
  } else {
    Set_V_true_br(variant);
  }

  return variant;
}


/* ====================================================================
 *
 * Invert_BR_Variant
 *
 * See interface description.
 *
 * ====================================================================
 */
VARIANT
Invert_BR_Variant(VARIANT variant)
{
  BOOL is_false = V_false_br(variant);

  switch (V_br_condition(variant)) {
  case V_BR_NONE: break;

  case V_BR_I8EQ: variant = V_BR_I8EQ; break;
  case V_BR_I8NE: variant = V_BR_I8NE; break;
  case V_BR_I8GT: variant = V_BR_I8LT; break;
  case V_BR_I8GE: variant = V_BR_I8LE; break;
  case V_BR_I8LT: variant = V_BR_I8GT; break;
  case V_BR_I8LE: variant = V_BR_I8GE; break;

  case V_BR_U8EQ: variant = V_BR_U8EQ; break;
  case V_BR_U8NE: variant = V_BR_U8NE; break;
  case V_BR_U8GT: variant = V_BR_U8LT; break;
  case V_BR_U8GE: variant = V_BR_U8LE; break;
  case V_BR_U8LT: variant = V_BR_U8GT; break;
  case V_BR_U8LE: variant = V_BR_U8GE; break;
#ifdef TARG_ST 
  case V_BR_A8EQ: variant = V_BR_A8EQ; break;
  case V_BR_A8NE: variant = V_BR_A8NE; break;
  case V_BR_A8GT: variant = V_BR_A8LT; break;
  case V_BR_A8GE: variant = V_BR_A8LE; break;
  case V_BR_A8LT: variant = V_BR_A8GT; break;
  case V_BR_A8LE: variant = V_BR_A8GE; break;
#endif
  case V_BR_FEQ: variant = V_BR_FEQ; break;
  case V_BR_FNE: variant = V_BR_FNE; break;
  case V_BR_FGT: variant = V_BR_FLT; break;
  case V_BR_FGE: variant = V_BR_FLE; break;
  case V_BR_FLT: variant = V_BR_FGT; break;
  case V_BR_FLE: variant = V_BR_FGE; break;

  case V_BR_DEQ: variant = V_BR_DEQ; break;
  case V_BR_DNE: variant = V_BR_DNE; break;
  case V_BR_DGT: variant = V_BR_DLT; break;
  case V_BR_DGE: variant = V_BR_DLE; break;
  case V_BR_DLT: variant = V_BR_DGT; break;
  case V_BR_DLE: variant = V_BR_DGE; break;

  case V_BR_QEQ: variant = V_BR_QEQ; break;
  case V_BR_QNE: variant = V_BR_QNE; break;
  case V_BR_QGT: variant = V_BR_QLT; break;
  case V_BR_QGE: variant = V_BR_QLE; break;
  case V_BR_QLT: variant = V_BR_QGT; break;
  case V_BR_QLE: variant = V_BR_QGE; break;

  case V_BR_I4EQ: variant = V_BR_I4EQ; break;
  case V_BR_I4NE: variant = V_BR_I4NE; break;
  case V_BR_I4GT: variant = V_BR_I4LT; break;
  case V_BR_I4GE: variant = V_BR_I4LE; break;
  case V_BR_I4LT: variant = V_BR_I4GT; break;
  case V_BR_I4LE: variant = V_BR_I4GE; break;

  case V_BR_U4EQ: variant = V_BR_U4EQ; break;
  case V_BR_U4NE: variant = V_BR_U4NE; break;
  case V_BR_U4GT: variant = V_BR_U4LT; break;
  case V_BR_U4GE: variant = V_BR_U4LE; break;
  case V_BR_U4LT: variant = V_BR_U4GT; break;
  case V_BR_U4LE: variant = V_BR_U4GE; break;
#ifdef TARG_ST
  case V_BR_I5EQ: variant = V_BR_I5EQ; break;
  case V_BR_I5NE: variant = V_BR_I5NE; break;
  case V_BR_I5GT: variant = V_BR_I5LT; break;
  case V_BR_I5GE: variant = V_BR_I5LE; break;
  case V_BR_I5LT: variant = V_BR_I5GT; break;
  case V_BR_I5LE: variant = V_BR_I5GE; break;

  case V_BR_U5EQ: variant = V_BR_U5EQ; break;
  case V_BR_U5NE: variant = V_BR_U5NE; break;
  case V_BR_U5GT: variant = V_BR_U5LT; break;
  case V_BR_U5GE: variant = V_BR_U5LE; break;
  case V_BR_U5LT: variant = V_BR_U5GT; break;
  case V_BR_U5LE: variant = V_BR_U5GE; break;

  case V_BR_A4EQ: variant = V_BR_A4EQ; break;
  case V_BR_A4NE: variant = V_BR_A4NE; break;
  case V_BR_A4GT: variant = V_BR_A4LT; break;
  case V_BR_A4GE: variant = V_BR_A4LE; break;
  case V_BR_A4LT: variant = V_BR_A4GT; break;
  case V_BR_A4LE: variant = V_BR_A4GE; break;
#endif
  case V_BR_PEQ: variant = V_BR_PEQ; break;
  case V_BR_PNE: variant = V_BR_PNE; break;

  case V_BR_I8EQ0:
  case V_BR_I8NE0:
  case V_BR_I8GT0:
  case V_BR_I8GE0:
  case V_BR_I8LT0:
  case V_BR_I8LE0:
  case V_BR_U8EQ0:
  case V_BR_U8NE0:
  case V_BR_U8GT0:
  case V_BR_U8GE0:
  case V_BR_U8LT0:
  case V_BR_U8LE0:
  case V_BR_I4EQ0:
  case V_BR_I4NE0:
  case V_BR_I4GT0:
  case V_BR_I4GE0:
  case V_BR_I4LT0:
  case V_BR_I4LE0:
  case V_BR_U4EQ0:
  case V_BR_U4NE0:
  case V_BR_U4GT0:
  case V_BR_U4GE0:
  case V_BR_U4LT0:
  case V_BR_U4LE0:
  case V_BR_F_FALSE:
  case V_BR_F_TRUE:
  case V_BR_P_TRUE:
    // these branches only have one operand, so swapping operands
    // makes no sense.
    break;

  case V_BR_ALWAYS:
  case V_BR_NEVER:
  case V_BR_CLOOP:
  case V_BR_CTOP:
  case V_BR_CEXIT:
  case V_BR_WTOP:
  case V_BR_WEXIT:
     // these branches effectively have no operands, so swapping
    // operands makes no sense.
    break;

  default:
    #pragma mips_frequency_hint NEVER
    FmtAssert(FALSE, ("unexpected variant %" SCNd64 "", variant));
    /*NOTREACHED*/
  }

  // set the sense.
  if (is_false) {
    Set_V_false_br(variant);
  } else {
    Set_V_true_br(variant);
  }

  return variant;
}


/* ====================================================================
 *
 * BR_Variant_Name
 *
 * See interface description.
 *
 * ====================================================================
 */
const char *
BR_Variant_Name(VARIANT variant)
{
  BOOL is_false = V_false_br(variant);
  switch (variant & V_BR_MASK) {
  case V_BR_NONE:    return is_false ? "!NONE"    : "NONE";

  case V_BR_I8EQ0:   return is_false ? "!I8EQ0"   : "I8EQ0";
  case V_BR_I8NE0:   return is_false ? "!I8NE0"   : "I8NE0";
  case V_BR_I8GT0:   return is_false ? "!I8GT0"   : "I8GT0";
  case V_BR_I8GE0:   return is_false ? "!I8GE0"   : "I8GE0";
  case V_BR_I8LT0:   return is_false ? "!I8LT0"   : "I8LT0";
  case V_BR_I8LE0:   return is_false ? "!I8LE0"   : "I8LE0";

  case V_BR_I4EQ0:   return is_false ? "!I4EQ0"   : "I4EQ0";
  case V_BR_I4NE0:   return is_false ? "!I4NE0"   : "I4NE0";
  case V_BR_I4GT0:   return is_false ? "!I4GT0"   : "I4GT0";
  case V_BR_I4GE0:   return is_false ? "!I4GE0"   : "I4GE0";
  case V_BR_I4LT0:   return is_false ? "!I4LT0"   : "I4LT0";
  case V_BR_I4LE0:   return is_false ? "!I4LE0"   : "I4LE0";

  case V_BR_I8EQ:    return is_false ? "!I8EQ"    : "I8EQ";
  case V_BR_I8NE:    return is_false ? "!I8NE"    : "I8NE";
  case V_BR_I8GT:    return is_false ? "!I8GT"    : "I8GT";
  case V_BR_I8GE:    return is_false ? "!I8GE"    : "I8GE";
  case V_BR_I8LT:    return is_false ? "!I8LT"    : "I8LT";
  case V_BR_I8LE:    return is_false ? "!I8LE"    : "I8LE";

  case V_BR_U8EQ0:   return is_false ? "!U8EQ0"   : "U8EQ0";
  case V_BR_U8NE0:   return is_false ? "!U8NE0"   : "U8NE0";
  case V_BR_U8GT0:   return is_false ? "!U8GT0"   : "U8GT0";
  case V_BR_U8GE0:   return is_false ? "!U8GE0"   : "U8GE0";
  case V_BR_U8LT0:   return is_false ? "!U8LT0"   : "U8LT0";
  case V_BR_U8LE0:   return is_false ? "!U8LE0"   : "U8LE0";
 
  case V_BR_U4EQ0:   return is_false ? "!U4EQ0"   : "U4EQ0";
  case V_BR_U4NE0:   return is_false ? "!U4NE0"   : "U4NE0";
  case V_BR_U4GT0:   return is_false ? "!U4GT0"   : "U4GT0";
  case V_BR_U4GE0:   return is_false ? "!U4GE0"   : "U4GE0";
  case V_BR_U4LT0:   return is_false ? "!U4LT0"   : "U4LT0";
  case V_BR_U4LE0:   return is_false ? "!U4LE0"   : "U4LE0";

  case V_BR_U8EQ:    return is_false ? "!U8EQ"    : "U8EQ";
  case V_BR_U8NE:    return is_false ? "!U8NE"    : "U8NE";
  case V_BR_U8GT:    return is_false ? "!U8GT"    : "U8GT";
  case V_BR_U8GE:    return is_false ? "!U8GE"    : "U8GE";
  case V_BR_U8LT:    return is_false ? "!U8LT"    : "U8LT";
  case V_BR_U8LE:    return is_false ? "!U8LE"    : "U8LE";
#ifdef TARG_ST
  case V_BR_A8EQ:    return is_false ? "!A8EQ"    : "A8EQ";
  case V_BR_A8NE:    return is_false ? "!A8NE"    : "A8NE";
  case V_BR_A8GT:    return is_false ? "!A8GT"    : "A8GT";
  case V_BR_A8GE:    return is_false ? "!A8GE"    : "A8GE";
  case V_BR_A8LT:    return is_false ? "!A8LT"    : "A8LT";
  case V_BR_A8LE:    return is_false ? "!A8LE"    : "A8LE";
#endif

  case V_BR_FEQ:     return is_false ? "!FEQ"     : "FEQ";
  case V_BR_FNE:     return is_false ? "!FNE"     : "FNE";
  case V_BR_FGT:     return is_false ? "!FGT"     : "FGT";
  case V_BR_FGE:     return is_false ? "!FGE"     : "FGE";
  case V_BR_FLT:     return is_false ? "!FLT"     : "FLT";
  case V_BR_FLE:     return is_false ? "!FLE"     : "FLE";

  case V_BR_DEQ:     return is_false ? "!DEQ"     : "DEQ";
  case V_BR_DNE:     return is_false ? "!DNE"     : "DNE";
  case V_BR_DGT:     return is_false ? "!DGT"     : "DGT";
  case V_BR_DGE:     return is_false ? "!DGE"     : "DGE";
  case V_BR_DLT:     return is_false ? "!DLT"     : "DLT";
  case V_BR_DLE:     return is_false ? "!DLE"     : "DLE";

  case V_BR_QEQ:     return is_false ? "!QEQ"     : "QEQ";
  case V_BR_QNE:     return is_false ? "!QNE"     : "QNE";
  case V_BR_QGT:     return is_false ? "!QGT"     : "QGT";
  case V_BR_QGE:     return is_false ? "!QGE"     : "QGE";
  case V_BR_QLT:     return is_false ? "!QLT"     : "QLT";
  case V_BR_QLE:     return is_false ? "!QLE"     : "QLE";

  case V_BR_I4EQ:    return is_false ? "!I4EQ"    : "I4EQ";
  case V_BR_I4NE:    return is_false ? "!I4NE"    : "I4NE";
  case V_BR_I4GT:    return is_false ? "!I4GT"    : "I4GT";
  case V_BR_I4GE:    return is_false ? "!I4GE"    : "I4GE";
  case V_BR_I4LT:    return is_false ? "!I4LT"    : "I4LT";
  case V_BR_I4LE:    return is_false ? "!I4LE"    : "I4LE";

  case V_BR_U4EQ:    return is_false ? "!U4EQ"    : "U4EQ";
  case V_BR_U4NE:    return is_false ? "!U4NE"    : "U4NE";
  case V_BR_U4GT:    return is_false ? "!U4GT"    : "U4GT";
  case V_BR_U4GE:    return is_false ? "!U4GE"    : "U4GE";
  case V_BR_U4LT:    return is_false ? "!U4LT"    : "U4LT";
  case V_BR_U4LE:    return is_false ? "!U4LE"    : "U4LE";
#ifdef TARG_ST      
  case V_BR_I5EQ:    return is_false ? "!I5EQ"    : "I5EQ";
  case V_BR_I5NE:    return is_false ? "!I5NE"    : "I4NE";
  case V_BR_I5GT:    return is_false ? "!I5GT"    : "I4GT"; 
  case V_BR_I5GE:    return is_false ? "!I5GE"    : "I4GE";
  case V_BR_I5LT:    return is_false ? "!I5LT"    : "I4LT";
  case V_BR_I5LE:    return is_false ? "!I5LE"    : "I4LE";

  case V_BR_U5EQ:    return is_false ? "!U5EQ"    : "U5EQ";
  case V_BR_U5NE:    return is_false ? "!U5NE"    : "U5NE";
  case V_BR_U5GT:    return is_false ? "!U5GT"    : "U5GT";
  case V_BR_U5GE:    return is_false ? "!U5GE"    : "U5GE";
  case V_BR_U5LT:    return is_false ? "!U5LT"    : "U5LT";
  case V_BR_U5LE:    return is_false ? "!U5LE"    : "U5LE";

  case V_BR_A4EQ:    return is_false ? "!A4EQ"    : "A4EQ";
  case V_BR_A4NE:    return is_false ? "!A4NE"    : "A4NE";
  case V_BR_A4GT:    return is_false ? "!A4GT"    : "A4GT";
  case V_BR_A4GE:    return is_false ? "!A4GE"    : "A4GE";
  case V_BR_A4LT:    return is_false ? "!A4LT"    : "A4LT";
  case V_BR_A4LE:    return is_false ? "!A4LE"    : "A4LE";
#endif
  case V_BR_F_FALSE: return is_false ? "!F_FALSE" : "F_FALSE";
  case V_BR_F_TRUE:  return is_false ? "!F_TRUE"  : "F_TRUE";

  case V_BR_P_TRUE:  return is_false ? "!P_TRUE"  : "P_TRUE";
  case V_BR_PEQ:     return is_false ? "!PEQ"     : "PEQ";
  case V_BR_PNE:     return is_false ? "!PNE"     : "PNE";

  case V_BR_CLOOP:   return is_false ? "!CLOOP"   : "CLOOP";
  case V_BR_CTOP:    return is_false ? "!CTOP"    : "CTOP";
  case V_BR_CEXIT:   return is_false ? "!CEXIT"   : "CEXIT";
  case V_BR_WTOP:    return is_false ? "!WTOP"    : "WTOP";
  case V_BR_WEXIT:   return is_false ? "!WEXIT"   : "WEXIT";

  case V_BR_ALWAYS:  return is_false ? "!ALWAYS"  : "ALWAYS";
  case V_BR_NEVER:   return is_false ? "!NEVER"   : "NEVER";
  }

  return "--UNKNOWN--";
}

#ifdef TARG_ST

static const struct {
  VARIANT v;
  const char *name;
  VARIANT inverted;
  VARIANT negated;
} CMP_Variant_Info [] =
  {
    { V_CMP_NONE,   "NONE",  V_CMP_NONE,   V_CMP_NONE  },  
    { V_CMP_EQ,     "EQ",    V_CMP_EQ,     V_CMP_NE    },
    { V_CMP_NE,     "NE",    V_CMP_NE,     V_CMP_EQ    },
    { V_CMP_GT,     "GT",    V_CMP_LT,     V_CMP_LE    },
    { V_CMP_GTU,    "GTU",   V_CMP_LTU,    V_CMP_LEU   },
    { V_CMP_GE,     "GE",    V_CMP_LE,     V_CMP_LT    },
    { V_CMP_GEU,    "GEU",   V_CMP_LEU,    V_CMP_LTU   },
    { V_CMP_LT,     "LT",    V_CMP_GT,     V_CMP_GE    },
    { V_CMP_LTU,    "LTU",   V_CMP_GTU,    V_CMP_GEU   },
    { V_CMP_LE,     "LE",    V_CMP_GE,     V_CMP_GT    },
    { V_CMP_LEU,    "LEU",   V_CMP_GEU,    V_CMP_GTU   },
    { V_CMP_ANDL,   "ANDL",  V_CMP_ANDL,   V_CMP_NANDL },
    { V_CMP_NANDL,  "NANDL", V_CMP_NANDL,  V_CMP_ANDL  },
    { V_CMP_ORL,    "ORL",   V_CMP_ORL,    V_CMP_NORL  },
    { V_CMP_NORL,   "NORL",  V_CMP_NORL,   V_CMP_ORL   }
  };

/* ====================================================================
 *   Invert_CMP_Variant
 *
 *   See interface description.
 * ====================================================================
 */
VARIANT
Invert_CMP_Variant (
  VARIANT variant
)
{
  FmtAssert (CMP_Variant_Info[variant].v == variant,
	     ("variant mismatch in CMP_Variant_Info (%d)", variant));
  return CMP_Variant_Info[variant].inverted;
}

/* ====================================================================
 *   Negate_CMP_Variant
 *
 *   See interface description.
 * ====================================================================
 */
VARIANT
Negate_CMP_Variant (
  VARIANT variant
)
{
  FmtAssert (CMP_Variant_Info[variant].v == variant,
	     ("variant mismatch in CMP_Variant_Info (%d)", variant));
  return CMP_Variant_Info[variant].negated;
}

const char *
CMP_Variant_Name (
  VARIANT variant
)
{
  return CMP_Variant_Info[variant].name;
}
#endif

