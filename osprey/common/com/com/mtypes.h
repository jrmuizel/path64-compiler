/* mtypes.h: This file automatically generated. */

/*

  Copyright (C) 2001 ST Microelectronics, Inc.  All Rights Reserved.

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
  address, or:

  http://www.st.com

  For further information regarding this notice, see:

  http://mcdt home page

*/

#ifndef mtypes_INCLUDED
#define mtypes_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#ifdef TARG_ST 
#include "config_target.h" 		 /* for ALIGN */
#else
#include "config_targ.h" 		 /* for ALIGN */
#endif

/* ==================================================================== 
 * ==================================================================== 
 * 
 * Module: mtypes.h 
 * 
 * Revision history: 
 *  11-Oct-89 - Original Version 
 *  19-May-01 - File generated automatically 
 * 
 * Description: 
 * 
 * Define IDs for the types supported by the target machine.  Not all 
 * of the predefined types will be supported on a given machine. 
 * 
 * ==================================================================== 
 * ==================================================================== 
 */ 

/* Type_class_bits */ 
#define MTYPE_CLASS_UNKNOWN 	 0x00
#define MTYPE_CLASS_INTEGER 	 0x01
#define MTYPE_CLASS_FLOAT 	 0x02
#define MTYPE_CLASS_COMPLEX 	 0x04
#define MTYPE_CLASS_UNSIGNED 	 0x08
#define MTYPE_CLASS_STR 	 0x10
#define MTYPE_CLASS_POINTER 	 0x20
#define MTYPE_CLASS_BOOLEAN 	 0x40
#define MTYPE_CLASS_UNSIGNED_INTEGER 	(MTYPE_CLASS_INTEGER|MTYPE_CLASS_UNSIGNED)
#define MTYPE_CLASS_COMPLEX_FLOAT 	(MTYPE_CLASS_FLOAT|MTYPE_CLASS_COMPLEX)
#define MTYPE_CLASS_UNSIGNED_INTEGER_POINTER 	(MTYPE_CLASS_POINTER|MTYPE_CLASS_INTEGER|MTYPE_CLASS_UNSIGNED)
#define MTYPE_CLASS_INTEGER_BOOLEAN 	(MTYPE_CLASS_BOOLEAN|MTYPE_CLASS_INTEGER)

/* The predefined machine data types, present on many machines: */
#define MTYPE_UNKNOWN	 0
#define MTYPE_FIRST	 1
#define MTYPE_B	 1	/* BOOL */
#define MTYPE_I1	 2	/* 8-bit integer */
#define MTYPE_I2	 3	/* 16-bit integer */
#define MTYPE_I4	 4	/* 32-bit integer */
#define MTYPE_I8	 5	/* 64-bit integer */
#define MTYPE_U1	 6	/* 8-bit unsigned integer */
#define MTYPE_U2	 7	/* 16-bit unsigned integer */
#define MTYPE_U4	 8	/* 32-bit unsigned integer */
#define MTYPE_U8	 9	/* 64-bit unsigned integer */
#define MTYPE_I5	 10	/* 40-bit integer */
#define MTYPE_U5	 11	/* 40-bit unsigned integer */
#define MTYPE_A4	 12	/* 32-bit address */
#define MTYPE_A8	 13	/* 64-bit address */
#define MTYPE_F4	 14	/* 32-bit IEEE floating point */
#define MTYPE_F8	 15	/* 64-bit IEEE floating point */
#define MTYPE_F10	 16	/* 80-bit IEEE floating point */
#define MTYPE_F16	 17	/* 128-bit IEEE floating point */
#define MTYPE_STR	 18	/* char strings - TCONs only */
#define MTYPE_STRING	 MTYPE_STR
#define MTYPE_FQ	 19	/* for SGI long double */
#define MTYPE_M	 20	/* memory chunk, for structures */
#define MTYPE_C4	 21	/* for 32-bit complex */
#define MTYPE_C8	 22	/* for 64-bit complex */
#define MTYPE_CQ	 23	/* for quad complex */
#define MTYPE_V	 24	/* for void type */
#define MTYPE_BS	 25	/* Bits */
#define MTYPE_C10	 26	/* 80-bit IEEE floating point complex */
#define MTYPE_C16	 27	/* 128-bit IEEE floating point complex */
#define MTYPE_I16	 28	/* 128-bit signed integer */
#define MTYPE_U16	 29	/* 128-bit unsigned integer */

/* must define MTYPE_LAST as the index of the last one defined. */
#define MTYPE_LAST	 29

/* Define the type: */ 
typedef UINT8	TYPE_ID; 
typedef mUINT8	mTYPE_ID; 

/* Define the type descriptor: */ 
typedef struct type_desc { 
  mCLASS_INDEX	 id; 	 /* Type index -- MTYPE_xxx above */
  mUINT16	 bit_size; 	 /* bit size */
  mUINT16	 byte_size; 	 /* byte size (NYI) */
  mUINT16	 dummy1; 	 /* remove when incompatible change */
  mUINT8	 alignment; 	 /* byte alignment defined in config_targ.h */
  mUINT8	 dummy2; 	 /* remove when incompatible change */
  mUINT8	 dummy3; 	 /* remove when incompatible change */
  mBOOL	 signed_type; 	 /* Signed numeric type? */
  mBOOL	 float_type; 	 /* Floating point type? */
  mCLASS_INDEX	 dummy4; 	 /* remove when incompatible change */
  char *	 name; 	 /* Print name */
  mUINT8	 type_class; 	 /* The classification bits used by the simplifier */
  mUINT8	 type_order; 	 /* The order of types (I8 > I4 for example) */
  mCLASS_INDEX	 complement; 	 /* complementary signed partner (ex. U1 -> I1) */
} TYPE_DESC; 

/* Types which are not supported in memory should have memory sizes 
 * and alignment values of 0. */ 

/* Declare the type descriptor table: */ 
BE_EXPORTED extern TYPE_DESC Machine_Types[]; 

/* Define the access functions: */ 
#define MTYPE_id(n)	 (Machine_Types[n].id)
#define MTYPE_bit_size(n)	 (Machine_Types[n].bit_size)
#define MTYPE_byte_size(n)	 (Machine_Types[n].byte_size)
#define MTYPE_dummy1(n)	 (Machine_Types[n].dummy1)
#define MTYPE_alignment(n)	 (Machine_Types[n].alignment)
#define MTYPE_dummy2(n)	 (Machine_Types[n].dummy2)
#define MTYPE_dummy3(n)	 (Machine_Types[n].dummy3)
#define MTYPE_signed_type(n)	 (Machine_Types[n].signed_type)
#define MTYPE_float_type(n)	 (Machine_Types[n].float_type)
#define MTYPE_dummy4(n)	 (Machine_Types[n].dummy4)
#define MTYPE_name(n)	 (Machine_Types[n].name)
#define MTYPE_type_class(n)	 (Machine_Types[n].type_class)
#define MTYPE_type_order(n)	 (Machine_Types[n].type_order)
#define MTYPE_complement(n)	 (Machine_Types[n].complement)
#define MTYPE_signed(n)	MTYPE_signed_type(n) 
#define MTYPE_float(n)	        MTYPE_float_type(n) 
#define MTYPE_size_reg(n)	MTYPE_bit_size(n) 
#define MTYPE_size_min(n)	MTYPE_bit_size(n) 
#define MTYPE_size_best(n)	MTYPE_bit_size(n) 
#define MTYPE_align_min(n)	MTYPE_alignment(n) 
#define MTYPE_align_req(n)	MTYPE_alignment(n) 
#define MTYPE_align_best(n)	MTYPE_alignment(n) 

/* define register classes */ 
#define MTYPE_is_integral(n)	(MTYPE_type_class(n) & MTYPE_CLASS_INTEGER) 
#define MTYPE_is_signed(n)	(MTYPE_type_class(n)==MTYPE_CLASS_INTEGER) 
#define MTYPE_is_unsigned(n)	(MTYPE_type_class(n)==MTYPE_CLASS_UNSIGNED_INTEGER) 
#define MTYPE_is_float(n)	(MTYPE_type_class(n) & MTYPE_CLASS_FLOAT) 
#define MTYPE_is_complex(n)	(MTYPE_type_class(n) & MTYPE_CLASS_COMPLEX) 
#define MTYPE_is_str(n)		(MTYPE_type_class(n)==MTYPE_STR) 
#define MTYPE_is_m(n)		((n)==MTYPE_M) 
#define MTYPE_is_void(n)	((n)==MTYPE_V) 

#define MTYPE_is_quad(n)	((n)==MTYPE_FQ || (n)==MTYPE_CQ) 
#define MTYPE_is_pointer(n)	((n)==Pointer_type || (n)==Pointer_type2) 
#define MTYPE_is_boolean(n)	((n)==Boolean_type || (n)==Boolean_type2) 

/* This is to distinguish INTEGER, POINTER and BOOLEAN classes */ 
#define MTYPE_is_class_boolean(n) (MTYPE_type_class(n) & MTYPE_CLASS_BOOLEAN) 
#define MTYPE_is_class_pointer(n) (MTYPE_type_class(n) & MTYPE_CLASS_POINTER) 
#define MTYPE_is_class_integer(n) ((MTYPE_type_class(n) & MTYPE_CLASS_INTEGER) && !(MTYPE_type_class(n) & MTYPE_CLASS_POINTER) && !(MTYPE_type_class(n) & MTYPE_CLASS_BOOLEAN)) 

/* 
 * These MTYPEs are split into I4/U4/F4 if
 * the flag Only_32_Bit_Ops is TRUE.
 */ 
#define MTYPE_is_longlong(n) (n == MTYPE_I8 || n == MTYPE_U8) 
#define MTYPE_is_double(n)   (n == MTYPE_F8) 
/* Register size in bytes */ 
#define	MTYPE_RegisterSize(n)	MTYPE_byte_size(n) 

#define MTYPE_is_size_double(n)	(MTYPE_bit_size(n) == MTYPE_bit_size(MTYPE_I8)) 

/* Define a mask of machine types, for use in register descriptors: */ 
typedef UINT32 MTYPE_MASK; 

/* Convert TYPE_ID to MTYPE_MASK: */ 
#define Type_Mask(t)	( ((MTYPE_MASK) 1) << (t) ) 

/* Operations on type masks: */ 
#define TMASK_Element(m,k)	(((m) & Type_Mask(k))!=0) 
#define TMASK_And(m1,m2)	((m1) & (m2)) 
#define TMASK_Or(m1,m2)		((m1) | (m2)) 
#define TMASK_Included(m1,m2)	(TMASK_And(m1,m2)==(m1)) 

/* Define which types are available on the target: */ 
BE_EXPORTED extern MTYPE_MASK Machine_Types_Available; 
#define MTYPE_Avail(k)	TMASK_Element(Machine_Types_Available,k) 

/* ======================================================================= 
 * 
 *  Exported Functions 
 * 
 *    char * Mtype_Name ( TYPE_ID b) 
 * 
 *	Routine to provide printable type name. 
 * 
 * 
 *    TYPE_ID Mtype_AlignmentClass(INT32 align, mUINT8 class) 
 * 
 *	Routine to return a type that matches exact byte alignment 
 *	and class 
 * 
 * 
 *    TYPE_ID Mtype_TransferSign(TYPE_ID type) 
 * 
 *	Routine to return the unsigned type corresponding to type 
 * 
 * ======================================================================= 
 */ 

BE_EXPORTED extern char * Mtype_Name( TYPE_ID ); 
BE_EXPORTED extern TYPE_ID Mtype_AlignmentClass( INT32 , mUINT8 ); 
BE_EXPORTED extern TYPE_ID Mtype_Promote_to_A4A8( TYPE_ID ); 
BE_EXPORTED extern TYPE_ID Mtype_TransferSign( TYPE_ID, TYPE_ID ); 
BE_EXPORTED extern TYPE_ID Mtype_TransferSize( TYPE_ID, TYPE_ID ); 
BE_EXPORTED extern TYPE_ID MTYPE_TransferSize( INT32, TYPE_ID ); 
BE_EXPORTED extern TYPE_ID Mtype_complex_to_real( TYPE_ID ); 
BE_EXPORTED extern TYPE_ID Mtype_comparison( TYPE_ID ); 
BE_EXPORTED extern TYPE_ID Mtype_next_alignment( TYPE_ID ); 
BE_EXPORTED extern TYPE_ID Mtype_prev_alignment( TYPE_ID ); 

#ifdef __cplusplus 
} 
#endif 
#endif /* mtypes_INCLUDED */ 
