/* 

  Copyright (C) 2006 ST Microelectronics, Inc.  All Rights Reserved. 

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

#ifndef _DYN_DLL_API_H_
#define _DYN_DLL_API_H_

#ifndef DLL_EXPORTED
#define DLL_EXPORTED 
#endif
#ifdef __cplusplus
extern "C" {
#endif

#define MAGIC_NUMBER_EXT_API   20080715  /* Magic number. Interface checking */

struct extension_machine_types
{
  /* GCC Machine mode*/
  machine_mode_t mmode; 
  /* mode attribute string */
  const char *name; 
  /*    This argument states the kind of representation: */
  /*    MODE_INT - integer */
  /*    MODE_FLOAT - floating */
  /*    MODE_PARTIAL_INT - PQImode, PHImode, PSImode and PDImode */
  /*    MODE_CC - modes used for representing the condition code in a register */
  /*    MODE_COMPLEX_INT, MODE_COMPLEX_FLOAT - complex number */
  /*    MODE_VECTOR_INT, MODE_VECTOR_FLOAT - vector */
  /*    MODE_RANDOM - anything else */
  enum mode_class mclass;
  /*    the relative size of the object, in bits, */
  /*    so we can have modes smaller than 1 byte. */
  unsigned short mbitsize; 
  /* gives the length of the mode, in bytes */
  unsigned char msize;
  /*    relative size of subunits of the object. */
  /*    It is same as the msize argument except for complexes and vectors, */
  /*    since they are really made of many equal size subunits. */
  unsigned char munitsize;
  /*    next wider natural mode of the same class.  0 if */
  /*    there is none.  Vector modes use this field to point to the next */
  /*    vector size, so we can iterate through the different vectors modes. */
  /*    The ordering is by increasing byte size, with QI coming before HI, */
  /*    HI before SI, etc. */
  unsigned char mwidermode; 
  /*    the mode of the internal elements in a vector or */
  /*    complex, and VOIDmode if not applicable. */
  machine_mode_t innermode;
  /* OPEN64 mtype */
  TYPE_ID mtype;
  unsigned short alignment;
  /* mtype to reg class association */
  int local_REGISTER_CLASS_id;
  /* mtype to reg sub class association */
  int local_REGISTER_SUBCLASS_id;
  unsigned char mpixelsize;
  /* size in bits of pixel type. zero for non-pixel types */
};

typedef struct extension_machine_types extension_machine_types_t;

  /* Available types of dynamic builtins */
  // 1 to 1 relation between current intrinsic and a TOP
#define DYN_INTRN_TOP 0x1
#define is_DYN_INTRN_TOP(b) (((b).type & DYN_INTRN_TOP) != 0)

  // Compiler known intrinsic used to partially compose a SIMD data
#define DYN_INTRN_PARTIAL_COMPOSE 0x2
#define is_DYN_INTRN_PARTIAL_COMPOSE(b) (((b).type & DYN_INTRN_PARTIAL_COMPOSE) != 0)
  // Compiler known intrinsic used to partially extract a SIMD data
#define DYN_INTRN_PARTIAL_EXTRACT 0x4
#define is_DYN_INTRN_PARTIAL_EXTRACT(b) (((b).type & DYN_INTRN_PARTIAL_EXTRACT) != 0)
  // Compiler known intrinsic used to fully compose a SIMD data
#define DYN_INTRN_COMPOSE 0x8
#define is_DYN_INTRN_COMPOSE(b) (((b).type & DYN_INTRN_COMPOSE) != 0)
  // Compiler known intrinsics used to convert between pixel
  // type (nx10/12/14bits) and corresponding native data type (nx16bits)
#define DYN_INTRN_CONVERT_TO_PIXEL 0x10
#define is_DYN_INTRN_CONVERT_TO_PIXEL(b) (((b).type & DYN_INTRN_CONVERT_TO_PIXEL ) != 0)
#define DYN_INTRN_CONVERT_FROM_PIXEL 0x20
#define is_DYN_INTRN_CONVERT_FROM_PIXEL(b) (((b).type & DYN_INTRN_CONVERT_FROM_PIXEL ) != 0)
  // Compiler known instrinsics used to convert between ctype (32bits 64bits)
  //  and extension types.
#define DYN_INTRN_CONVERT_TO_CTYPE 0x40
#define is_DYN_INTRN_CONVERT_TO_CTYPE(b) (((b).type & DYN_INTRN_CONVERT_TO_CTYPE ) != 0)
#define DYN_INTRN_CONVERT_FROM_CTYPE 0x80
#define is_DYN_INTRN_CONVERT_FROM_CTYPE(b) (((b).type & DYN_INTRN_CONVERT_FROM_CTYPE ) != 0)
  // type used for automatic code
  // generation when intrinsic match a
  // wn and intrinsic is meta (with
  // an asm-stmt behavior associated.
#define DYN_INTRN_WHIRLNODE_META 0x100
#define is_DYN_INTRN_WHIRLNODE_META(b) (((b).type & DYN_INTRN_WHIRLNODE_META) != 0)
  // marker specifying that intrinsic is a CLR.
#define DYN_INTRN_CLR 0x200
#define is_DYN_INTRN_CLR(b) (((b).type & DYN_INTRN_CLR) != 0)
typedef int DYN_INTRN_TYPE;


/*
 * Management of multi-results builtins. 
 *
 * At front-end level, multi-results builtins
 * are always called following a functional
 * style:
 *
 *    f(a,b,c,d);     // Intrinsic call
 *
 * Result arguments are passed by reference
 * in a C++ parameter style. The following enumeration
 * makes it possible to determine which parameter
 * is a result, which parameter is an input and
 * which parameter is both an input and a
 * result.
 *
 * The return type of multi-results builtins
 * is always VOIDmode.
 *
 */
typedef enum {
   BUILTARG_UNDEF = 0,      /* Invalid value   -- only for debugging purpose.        */
   BUILTARG_OUT      ,      /* Out argument    -- C++ style (reference) par. passing */
   BUILTARG_IN       ,      /* In argument     -- par. passed by value               */
   BUILTARG_INOUT    ,      /* In/out argument -- C++ style (reference) par. passing */
} BUILTARG_INOUT_TYPE;

  
struct wn_record {
  int wn_opc;               // whirl-node opcode
  int cycles;               // cost in cycles
  int size;                 // cost in size
  char*  asm_stmt_behavior; // asm statement code
  char** asm_stmt_results;  // asm statement "results": out, in/out and tmp
  char** asm_stmt_operands; // asm statement "operands": in
};
typedef struct wn_record wn_record_t;


struct extension_builtins
{
  enum built_in_function 		gcc_builtin_def;
  INTRINSIC	open64_intrincic;

  // Builtins flags follow the WHIRL semantic (opposed to the gcc one
  // for has_no_side_effects and is_pure)
  // has_no_side_effects means is_pure + no access to memory
  char		is_by_val;
  char		is_pure;
  char		has_no_side_effects;
  char		never_returns;
  char		is_actual;
  char		is_cg_intrinsic;
  const char   *c_name;
  const char   *runtime_name;

  /* Prototype information   */
  machine_mode_t             return_type;
  unsigned char	             arg_count;
  const machine_mode_t 	    *arg_type;     /* Number of items in table: arg_count */
  const BUILTARG_INOUT_TYPE *arg_inout;    /* Number of items in table: arg_count */

  // targ_info information
  DYN_INTRN_TYPE type; // standard TOP intrinsic or compose/extract
  union {
    int		 local_TOP_id;        // TOP id if TOP intrinsic
    int		 compose_extract_idx; // subpart access index if compose/extract
  } u1;

  const wn_record_t *wn_table;
};
  
typedef struct extension_builtins extension_builtins_t;
  

struct extension_hooks
{
  /* Interface identification */
  mUINT32  magic;

  /* ==========================
     ==  MTYPES information  ==
     ========================== */
  /* Return a pointer on the modes array.  */
  const extension_machine_types_t * (*get_modes) (void);

  /* Return the number of modes.  */
  unsigned int (*get_modes_count) (void);

  /* Return the base count for that extension dll. */
  machine_mode_t (*get_modes_base_count) (void);

  /* Return the base mtype count for that extension dll. */
  TYPE_ID (*get_mtypes_base_count) (void);

  /* ============================
     ==  Builtins information  ==
     ============================ */
  /* Return a pointer on the builtins array.  */
  const extension_builtins_t* (*get_builtins) (void);

  /* Return the number of builtins.  */
  unsigned int (*get_builtins_count) (void);

  /* Return the base count for that extension dll. */
  machine_mode_t (*get_builtins_base_count) (void);

  /* Return the base intrinsic count for that extension dll. */
  INTRINSIC (*get_intrinsics_base_count) (void);

};

typedef struct extension_hooks extension_hooks_t;

/* Function that returns an instance for the dll*/
typedef const extension_hooks_t *(*get_type_extension_instance_t)(void);
DLL_EXPORTED extern const extension_hooks_t   *get_type_extension_instance();

/* Function that returns the extension name from the dll */
typedef const char *(*get_extension_name_t)(void);
DLL_EXPORTED extern const char *get_extension_name();


#ifdef __cplusplus
}
#endif

#endif /* _DYN_DLL_API_H_ */
