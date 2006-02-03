/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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


#ifndef const_INCLUDED
#define const_INCLUDED


#ifndef targ_const_INCLUDED
#include "targ_const.h"
#endif /* targ_const_INCLUDED */

class WN;

BE_EXPORTED extern ST *
New_Const_Sym (TCON_IDX tcon, TY_IDX ty);


BE_EXPORTED extern ST *
Gen_String_Sym (TCON *val, TY_IDX ty, BOOL unique);


BE_EXPORTED extern TCON Const_Val (WN *n);

BE_EXPORTED extern WN *
Make_Const (TCON c);

static const INT32 MAX_SYMBOLIC_CONST_NAME_LEN = 1024;

#ifndef MONGOOSE_BE

BE_EXPORTED extern WN *
Make_Zerocon ( TYPE_ID mtype );

BE_EXPORTED extern WN *
Make_Comparison_Result_Const ( INT16 val );

BE_EXPORTED extern WN *
Make_Integer_Const ( INT16 mtype, TARG_INT val );

#endif /* MONGOOSE_BE */

BE_EXPORTED extern WN *Make_Reduction_Identity ( INT32 opr, TYPE_ID mtype );

#ifdef TARG_ST
/* ==================================================================
 * Arthur: moved targ_const_private.h here from target-specific directory
 *         -- looks rather target-independent, perhaps except for the
 *         TCON_I8U8I4U4(c) macro, which has been left target-specific.
 * ==================================================================
 */

#if defined(_COMPILER_VERSION) && (_COMPILER_VERSION >= 400) && _SGIAPI
#define QUAD_PRECISION_SUPPORTED
#else
#undef QUAD_PRECISION_SUPPORTED
#endif

#undef  TCON_ty
#define TCON_ty(c)	((c).ty)

#define TCON_v0(c)	((c).vals.ival.v0)
#define TCON_v1(c)	((c).vals.ival.v1)
#define TCON_v2(c)	((c).vals.ival.v2)
#define TCON_v3(c)	((c).vals.ival.v3)
#define TCON_iv0(c)	((c).cmplxval.ival.v0)
#define TCON_iv1(c)	((c).cmplxval.ival.v1)
#define TCON_iv2(c)	((c).cmplxval.ival.v2)
#define TCON_iv3(c)	((c).cmplxval.ival.v3)
#define TCON_u0(c)	((c).vals.uval.u0)
#define TCON_u1(c)	((c).vals.uval.u1)
#define TCON_u2(c)	((c).vals.uval.u2)
#define TCON_u3(c)	((c).vals.uval.u3)
#define TCON_I4(c)	TCON_v0(c)
#define TCON_U4(c)	TCON_u0(c)
#define TCON_I8(c)	((c).vals.i0)
#define TCON_U8(c)	((c).vals.k0)

/* The internal representation of 32-bit floats may be double: */
#ifdef TCON_R4_IS_DOUBLE
#define TCON_R4(c)	TCON_R8(c)
#define TCON_IR4(c)	TCON_IR8(c)
#else
#define TCON_R4(c)	((c).vals.f.fval)
#define TCON_IR4(c)	((c).cmplxval.fival)
#endif
/* WARNING:  We haven't carefully checked the implications of using
 * doubles for 32-bit floats -- must be done if turned on.
 */

#define TCON_R8(c)	((c).vals.dval)
#define TCON_R16(c)	((c).vals.qval)

/* Complex imaginary parts */
#define TCON_IR8(c)	((c).cmplxval.dival)
#define TCON_IR16(c)	((c).cmplxval.qival)

#define Set_TCON_R4(c,v)	(TCON_R4(c) = v)
#define Set_TCON_R8(c,v)	(TCON_R8(c) = v)
#define Set_TCON_R16(c,v)	(TCON_R16(c) = v)
#define Set_TCON_IR4(c,v)	(TCON_IR4(c) = v)
#define Set_TCON_IR8(c,v)	(TCON_IR8(c) = v)
#define Set_TCON_IR16(c,v)	(TCON_IR16(c) = v)

#define TCON_cp(c)	((c).vals.sval.cp)
#define TCON_len(c)	((c).vals.sval.len)

#endif /* TARG_ST */

#endif /* const_INCLUDED */
