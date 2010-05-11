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


/*
 * This defines the ABI subprogram interface,
 * and is used to determine how parameters and results are passed.
 * We have an array of tables, where each table describes the info
 * for one abi.  The array is indexed by the TARGET_ABI enumeration.
 * The register values are the PREG offsets, so these values can be
 * used in WHIRL.
 */

#define TRACE_ENTRY(x)
#define TRACE_EXIT(x)
#define TRACE_EXIT_i(x,i)

#include "W_limits.h"
#include "defs.h"
#include "mtypes.h"
#include "errors.h"
#include "erglob.h"
#include "stab.h"
#include "config_target.h"
#include "config_TARG.h"
#include "targ_sim.h"
#include "tracing.h"
#ifdef TARG_ST
#include "register_preg.h" // TB: For CGTARG_Regclass_Preg_Min
#endif

#ifdef Is_True_On
/* Use a macro here such that each use effectively calls the Get_Trace() function. 
   Indeed this function must be called whenever Current_PU_name changes for -tf.. to work. */
#define TRACE_ON Get_Trace(TP_DATALAYOUT, 1)
#else
#define TRACE_ON FALSE
#endif

#include "targ_sim_body.h"

static mDED_PREG_NUM Input_Base_Preg = 0;
static mDED_PREG_NUM Output_Base_Preg = 0;

/* [JV] struct by technical reference. */
static BOOL CG_struct_by_ref = FALSE;

#define I0 Int_Preg_Min_Offset
#define P0 Ptr_Preg_Min_Offset
#define F0 Float_Preg_Min_Offset

INT Default_Max_Formal_Save_Area_Size;
INT Default_Vararg_Formal_Save_Area_Size;

/* some definitions for the dedicated hardware pregs: */
INT Int_Preg_Min_Offset;
INT Int_Preg_Max_Offset;
INT Ptr_Preg_Min_Offset;
INT Ptr_Preg_Max_Offset;
INT Float_Preg_Min_Offset;
INT Float_Preg_Max_Offset;
INT Branch_Preg_Min_Offset;
INT Branch_Preg_Max_Offset;
INT Fcc_Preg_Min_Offset;
INT Fcc_Preg_Max_Offset;
//TB Extend PREG registers to extension
//TB: make a static var for preg linits. Only functional accesses
static INT Last_Dedicated_Preg_Offset_Var;
static INT Static_Last_Dedicated_Preg_Offset;

/* The offsets for return registers are fixed: */
INT First_Int_Preg_Return_Offset;
INT Last_Int_Preg_Return_Offset;
INT First_Ptr_Preg_Return_Offset;
INT Last_Ptr_Preg_Return_Offset;
INT First_Float_Preg_Return_Offset;
INT Last_Float_Preg_Return_Offset;

INT First_Int_Preg_Param_Offset;
INT First_Ptr_Preg_Param_Offset;
INT First_Float_Preg_Param_Offset;

INT Stack_Pointer_Preg_Offset;
INT Frame_Pointer_Preg_Offset;
INT Static_Link_Preg_Offset;
INT Struct_Return_Preg_Offset;
#ifdef TARG_ST //[TB]
INT Function_Link_Preg_Offset;
INT Exc_Ptr_Param_Offset;           /* exception struct ptr */
INT Exc_Filter_Param_Offset;   /* exception filter value */
#endif

static void
Init_Pregs( void ) {

  if (Static_Last_Dedicated_Preg_Offset != 0)
    //TB: Init already done
    return;

  // In some cases we may reach this point with an already
  // modified value in ISA_REGISTER_CLASS_MAX, which is not
  // compliant with initial assumption (#97483)
  ISA_REGISTER_CLASS_MAX=ISA_REGISTER_CLASS_STATIC_MAX;
  ISA_REGISTER_CLASS_COUNT=ISA_REGISTER_CLASS_STATIC_COUNT;

  /* some definitions for the dedicated hardware pregs: */

  Int_Preg_Min_Offset =             CGTARG_Regclass_Preg_Min(ISA_REGISTER_CLASS_integer);
  Int_Preg_Max_Offset =             CGTARG_Regclass_Preg_Max(ISA_REGISTER_CLASS_integer);
  Ptr_Preg_Min_Offset =             1;
  Ptr_Preg_Max_Offset =             0;

  Float_Preg_Min_Offset =           1;//1
  Float_Preg_Max_Offset =           0;//0
  Branch_Preg_Min_Offset =          1;
  Branch_Preg_Max_Offset =          0;
   
  Fcc_Preg_Min_Offset =             1;
  Fcc_Preg_Max_Offset =             0;
  Last_Dedicated_Preg_Offset_Var    = CGTARG_Regclass_Preg_Max(ISA_REGISTER_CLASS_STATIC_MAX);
  //[TB]: Keep a trace of last dedicated offset of the core
  //Last_Dedicated_Preg_Offset_Var will be incremented by the extension loader
  Static_Last_Dedicated_Preg_Offset = CGTARG_Regclass_Preg_Max(ISA_REGISTER_CLASS_STATIC_MAX);

/* The offsets for return registers are fixed: */
  First_Int_Preg_Return_Offset =    CGTARG_Regclass_Preg_Min(ISA_REGISTER_CLASS_integer) + 0;	/* register r0 */
  Last_Int_Preg_Return_Offset =	    CGTARG_Regclass_Preg_Min(ISA_REGISTER_CLASS_integer) + 1;	/* register r3 */
  First_Ptr_Preg_Return_Offset =    0;	/* undef */
  Last_Ptr_Preg_Return_Offset =     0;	/* undef */
  First_Float_Preg_Return_Offset =  0;	/* undef */	
  Last_Float_Preg_Return_Offset =   0;	/* undef */	

  First_Int_Preg_Param_Offset   =   I0;  /* register r0 */
  First_Ptr_Preg_Param_Offset   =   0;  /* undef */
  First_Float_Preg_Param_Offset =  0; /* undef */
  
  Stack_Pointer_Preg_Offset =	    CGTARG_Regclass_Preg_Min(ISA_REGISTER_CLASS_integer) + 13;	/* register sp (r13) */
  Frame_Pointer_Preg_Offset =	    0;  /* undef */
  Static_Link_Preg_Offset =	    0;	/* undef */
  Struct_Return_Preg_Offset =       CGTARG_Regclass_Preg_Min(ISA_REGISTER_CLASS_integer) + 0;  	/* register r0 */
#ifdef TARG_ST //[TB]
  Function_Link_Preg_Offset =       CGTARG_Regclass_Preg_Min(ISA_REGISTER_CLASS_integer) + 14; 	/* r14 function link register (for mcount call) */
#endif

  Default_Max_Formal_Save_Area_Size =   16;
  Default_Vararg_Formal_Save_Area_Size = 16;

  return;
}

INT Get_Static_Last_Dedicated_Preg_Offset(){
  if (Static_Last_Dedicated_Preg_Offset == 0)
    Init_Pregs();
  return Static_Last_Dedicated_Preg_Offset;
}

INT Get_Last_Dedicated_Preg_Offset_Func(){
  if (Static_Last_Dedicated_Preg_Offset == 0)
    Init_Pregs();
  return Last_Dedicated_Preg_Offset_Var;
}

void Set_Last_Dedicated_Preg_Offset(INT offset){
  if (Static_Last_Dedicated_Preg_Offset == 0)
    Init_Pregs();
  Last_Dedicated_Preg_Offset_Var = offset;
}



// [HK]
#if __GNUC__ >= 3
#else
static
#endif
SIM SIM_Info[] = {
	/* flags */
	/* int args, ptr args, flt args, dbl args */
	/* int res , ptr res, flt res, dbl res */
	/* int type, ptr type, flt type, dbl type */
	/* save area, formal-area, var ofst */
	/* struct arg, struct res, slink, pic */
  {/* ABI_UNDEF */
	0,
	{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
	{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
	0, 0, 0, 0,
	0, 0, 0, 
	0, 0, 0, 0
  },
  { /* ABI_ARM_ver1 */
        SIM_COORD_MEM_REG | SIM_REG_STRUCTS | SIM_RET_ADDR_VIA_INT_RET,
	{I0+0,I0+3,1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
	{I0+0,I0+1,1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
	MTYPE_I4, MTYPE_U4, MTYPE_U4, MTYPE_U4,
	0, Default_Vararg_Formal_Save_Area_Size, -Default_Vararg_Formal_Save_Area_Size, 
	16, 16, I0+0, 0
  },
  { /* ABI_ARM_ver2 */
        SIM_COORD_MEM_REG | SIM_REG_STRUCTS | SIM_RET_ADDR_VIA_INT_RET,
	{I0+0,I0+3,1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
	{I0+0,I0+1,1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
	MTYPE_I4, MTYPE_U4, MTYPE_U4, MTYPE_U4,
	0, Default_Vararg_Formal_Save_Area_Size, -Default_Vararg_Formal_Save_Area_Size, 
	16, 16, I0+0, 0
  }
};

static void
Init_Sim_Info( void ) {

  static BOOL sim_info_initialized = FALSE;
  if (sim_info_initialized) return;
  sim_info_initialized = TRUE;
  /* [JV] I am not happy with this ... perhaps it should be generated */
  if(Int_Preg_Min_Offset < Int_Preg_Max_Offset) {
    PR_first_reg(SIM_INFO.int_args) += I0;
    PR_last_reg(SIM_INFO.int_args) += I0;
    PR_first_reg(SIM_INFO.int_results) += I0;
    PR_last_reg(SIM_INFO.int_results) += I0;
  }

  if(Ptr_Preg_Min_Offset < Ptr_Preg_Max_Offset) {
    PR_first_reg(SIM_INFO.ptr_args) += P0;
    PR_last_reg(SIM_INFO.ptr_args) += P0;
    PR_first_reg(SIM_INFO.ptr_results) += P0;
    PR_last_reg(SIM_INFO.ptr_results) += P0;
  }

  if(Float_Preg_Min_Offset < Float_Preg_Max_Offset) {
    /* Case when Enable_Fpx is true. */
    PR_first_reg(SIM_INFO.flt_args)     += F0;
    PR_last_reg(SIM_INFO.flt_args)      += F0;
    PR_first_reg(SIM_INFO.flt_results)  += F0;
    PR_last_reg(SIM_INFO.flt_results)   += F0;
  }
}


void
Set_struct_by_ref_for_parameter_location( BOOL _CG_struct_by_ref ) {
  CG_struct_by_ref = _CG_struct_by_ref;
  return;
}

/* ====================================================================
 *   Is_Return_Preg
 *
 *   return whether preg is a return preg
 * ====================================================================
 */
BOOL 
Is_Return_Preg (
  PREG_NUM preg
)
{
  return ((preg >= First_Int_Preg_Return_Offset &&
	   preg <= Last_Int_Preg_Return_Offset) ||
	  (preg >= First_Float_Preg_Return_Offset &&
	   preg <= Last_Float_Preg_Return_Offset) );
}

/* ====================================================================
 *   Is_Int_Output_Preg
 *
 *   return whether preg is an output preg (rotating regs)
 * ====================================================================
 */
BOOL 
Is_Int_Output_Preg (
  PREG_NUM preg
)
{
  return FALSE;
}

/* ====================================================================
 *   Is_Formal_Preg
 *
 *   return whether preg is an input preg 
 * ====================================================================
 */
extern BOOL
Is_Formal_Preg (
  PREG_NUM preg
)
{
  return ((preg >= PR_first_reg(SIM_INFO.int_args) &&
	     preg <= PR_last_reg(SIM_INFO.int_args)));
}

/* ====================================================================
 *   Is_Simulated_Type
 * ====================================================================
 */
static BOOL
Is_Simulated_Type (
  TYPE_ID mtype
)
{
  switch (mtype) {
    case MTYPE_FQ: return TRUE;
    case MTYPE_C4: return TRUE;
    case MTYPE_C8: return TRUE;
    case MTYPE_CQ: return TRUE;
    default:	return FALSE;
  }
}

/* ====================================================================
 *   Get_Return_Info
 *
 *   This routine figures out the mtypes of the return registers that are 
 *   used for returning an object of the given type.
 *   This returns the mtypes to use for the CALL opcode in high-level whirl.
 *   This means that returns of simulated objects, like FQ, are just shown
 *   as returning FQ, which will later be split into F8F8.
 *   However, structures that return in registers are specified explicitly.
 *
 *   This routine figures out which return registers are to be used
 *   for returning an object with the given mtypes.
 *   It is assumed that the mtypes will be determined by calling
 *   Get_Return_Mtypes.
 * ====================================================================
 */
RETURN_INFO
Get_Return_Info (
  TY_IDX rtype, 
  Mtype_Return_Level level
)
{
  TYPE_ID mtype = TY_mtype (rtype);
  PREG_NUM reg;
  RETURN_INFO info;
  INT32 i; 

  info.return_via_first_arg = FALSE;
  //info.return_via_return_reg = FALSE;

  switch (mtype) {

    case MTYPE_UNKNOWN:

      // FORTRAN character array
      info.count = 0;
      // f90 already has made visible the arg for arrays
      // info.return_via_first_arg = TRUE;
      break;

    case MTYPE_V:

      info.count = 0;
      break;

    case MTYPE_F4:
      // Floating point types are mapped to integer regs
      info.count = 1;
      info.mtype [0] = MTYPE_U4;
      info.preg  [0] = PR_first_reg(SIM_INFO.int_results);
      break;

    case MTYPE_I1:
    case MTYPE_I2:
    case MTYPE_I4:
    case MTYPE_U1:
    case MTYPE_U2:
    case MTYPE_U4:
    case MTYPE_A4:

      info.count = 1;
      info.mtype [0] = mtype;
      info.preg  [0] = PR_first_reg(SIM_INFO.int_results);
      break;

    // Floating-point types are mapped to integer regs
    case MTYPE_F8:

      reg = PR_first_reg(SIM_INFO.int_results);
      if (level == Use_Simulated) {
	info.count = 1;
	info.mtype [0] = MTYPE_I8;
	info.preg  [0] = reg;
      }
      else {
	info.count = 2;
	info.mtype [0] = SIM_INFO.int_type;
	info.preg  [0] = reg++;
	info.mtype [1] = SIM_INFO.int_type;
	info.preg  [1] = reg++;
      }
      break;

    case MTYPE_I8:
    case MTYPE_U8:

      reg = PR_first_reg(SIM_INFO.int_results);
      if (level == Use_Simulated) {
	info.count = 1;
	info.mtype [0] = mtype;
	info.preg  [0] = reg;
      }
      else {
	info.count = 2;
	info.mtype [0] = SIM_INFO.int_type;
	info.preg  [0] = reg++;
	info.mtype [1] = SIM_INFO.int_type;
	info.preg  [1] = reg++;
      }
      break;

    case MTYPE_M:

      //
      // Return Structure or Array.

      // make sure it's a struct
      Is_True (TY_kind (rtype) == KIND_STRUCT, ("expecting KIND_STRUCT"));

      // In all cases return via callee buffer pointed by r12
      // returning via $r12
      info.count = 0;
      //info.return_via_return_reg = TRUE;
      info.return_via_first_arg = TRUE;

      break;

    case MTYPE_FQ:
    case MTYPE_C4:
    case MTYPE_C8:

    default:

      info.count = 0;
      FmtAssert (FALSE, ("Invalid return mtype %s encountered",
                                                (MTYPE_name(mtype))));
      break;
  } /* switch (mtype) */

  for (i = info.count; i < MAX_NUMBER_OF_REGISTERS_FOR_RETURN; i++) {
    info.mtype [i] = MTYPE_V;
    info.preg  [i] = 0;
  }

  return info;
} /* Get_Return_Info */

static INT Current_Int_Param_Num = 0;
static INT Current_Flt_Param_Num = 0;
static INT Current_Formal_Offset;
static INT Current_UpFormal_Offset;
static BOOL First_Param_In_Return_Reg = FALSE;
static INT Formal_Size = 0;

/* ====================================================================
 *   Setup_Parameter_Locations
 * ====================================================================
 */
static PLOC
Setup_Parameter_Locations (
#ifdef TARG_ST
			   TY_IDX pu_type,
			   BOOL   first_hidden_param_is_lowered
#else
  TY_IDX pu_type
#endif
)
{
  static PLOC plocNULL;

  //
  // If we're returning a structure/union via $r0.15, set the
  // First_Param_Is_Return_Reg
  //
  TY_IDX ret_type = (TY_kind(pu_type) == KIND_FUNCTION ? TY_ret_type(pu_type)
			: pu_type);
  RETURN_INFO info = Get_Return_Info (ret_type, No_Simulated);
#ifdef TARG_ST
  First_Param_In_Return_Reg = (RETURN_INFO_return_via_first_arg(info) & SIM_return_addr_via_int_return_reg)
    && first_hidden_param_is_lowered;
#else
  First_Param_In_Return_Reg = (RETURN_INFO_return_via_first_arg(info) 
			       & SIM_return_addr_via_int_return_reg);
#endif

  if (TY_is_varargs (pu_type)) {
    // find last fixed parameter
    TYLIST_IDX idx = TY_tylist (pu_type);
    Last_Fixed_Param = -1;
    for (++idx; Tylist_Table[idx] != 0; ++idx)
      ++Last_Fixed_Param;

    // old style varargs is counting va_alist and should not
    if ( ! TY_has_prototype(pu_type)) {
      --Last_Fixed_Param;
    }

    // account for functions returning to first parameter
    if (TY_return_to_param (pu_type)) {
      ++Last_Fixed_Param;
    }
  }
  else {
    Last_Fixed_Param = INT_MAX;
  }

  Current_Int_Param_Num = 0;
  Current_Flt_Param_Num = 0;
  Current_Formal_Offset = 0;
  Current_UpFormal_Offset = 0;
  Formal_Size = 0;

  return plocNULL;
} 

/* ====================================================================
 *   Get_Current_Int_Preg_Num
 * ====================================================================
 */
static inline PREG_NUM
Get_Current_Int_Preg_Num ( Preg_Range pr ) {
  PREG_NUM i;

  if(TRACE_ON) {
    fprintf(TFile,"%s: preg = %d\n",__FUNCTION__,pr);
  }
  i = PR_first_reg(pr) + (Current_Int_Param_Num * PR_skip_value(pr));
  if (i > PR_last_reg(pr)) {
    if(TRACE_ON) {
      fprintf(TFile,"%s: return null, i = %d, PR_last_reg(pr) = %d, Current_Int_Param_Num\n",
	      __FUNCTION__,i,PR_last_reg(pr),Current_Int_Param_Num);
    }
    return 0;
  }
  else {
    if(TRACE_ON) {
      fprintf(TFile,"%s: return %d, i = %d, PR_last_reg(pr) = %d, Current_Int_Param_Num\n",
	      __FUNCTION__,i,i,PR_last_reg(pr),Current_Int_Param_Num);
    }
    return i;
  }
}

/* ====================================================================
 *   Get_Current_Flt_Preg_Num
 * ====================================================================
 */
static inline PREG_NUM
Get_Current_Flt_Preg_Num ( Preg_Range pr ) {
  PREG_NUM i;

  i = PR_first_reg(pr) + (Current_Flt_Param_Num * PR_skip_value(pr));
  if (i > PR_last_reg(pr)) {
    return 0;
  }
  else {
    return i;
  }
}

#define ROUNDUP(val,align) 	( (-(INT64)align) & (INT64)(val+align-1) )

/* ====================================================================
 *   Update_Stack_Offsets
 * ====================================================================
 */
static void
Update_Stack_Offsets( TYPE_ID mtype, PLOC *ploc ) {
  INT alignment = MTYPE_alignment(mtype);

  if(mtype == MTYPE_M) {
    // Struct are passed by ref
    alignment = MTYPE_alignment(MTYPE_A4);
  }

  if(PLOC_on_stack(*ploc)) {
    ploc->formal_offset = Current_Formal_Offset;
    ploc->upformal_offset = ROUNDUP(Current_UpFormal_Offset + PLOC_lpad(*ploc), alignment);
    Current_UpFormal_Offset = ROUNDUP(ploc->upformal_offset + PLOC_size(*ploc) + PLOC_rpad(*ploc), alignment);
  }
  else {
    ploc->formal_offset = ROUNDUP(Current_Formal_Offset + PLOC_lpad(*ploc), alignment);
    Current_Formal_Offset = ROUNDUP(ploc->formal_offset + PLOC_size(*ploc) + PLOC_rpad(*ploc), alignment);
  }
}


/* ====================================================================
 *   Get_Parameter_Location
 * ====================================================================
 */
static PLOC
Get_Parameter_Location (
  TY_IDX ty, 
  BOOL is_output
)
{
  PLOC ploc;				// return location

  // Init PLOC from current state
  PLOC_clear(ploc);
  ploc.formal_offset = Current_Formal_Offset;
  ploc.upformal_offset = Current_UpFormal_Offset;
  ploc.size = 0;
  ploc.vararg_reg = 0;               // to silence purify
  ploc.lpad = 0;
  ploc.rpad = 0;

  ploc.by_reference = 0;

  //
  // If we're returning a structure/union via $r0.15, just
  // return the register number, no ploc size/offset changes
  //
  if (First_Param_In_Return_Reg) {
    First_Param_In_Return_Reg = FALSE;
    ploc.reg = Struct_Return_Preg_Offset;
    return ploc;
  }

  /* check for array case where fe doesn't fill in right btype */
  TYPE_ID pmtype = Fix_TY_mtype (ty);	/* Target type */

  ploc.size = MTYPE_RegisterSize(pmtype);

  INT lpad = 0;                 /* padding to left of object */
  INT rpad = 0;			/* padding to right of object */

  switch (pmtype) {
	
    case MTYPE_I1:
    case MTYPE_U1:
    case MTYPE_I2:
    case MTYPE_U2:
    case MTYPE_I4:
    case MTYPE_U4:

      if (Target_Byte_Sex == BIG_ENDIAN) {
	/* want to right-justify the object */
	lpad = MTYPE_RegisterSize(SIM_INFO.int_type) - ploc.size;
      }
      else {
	/* Pad to word; leave address alone */
	rpad = (MTYPE_RegisterSize(SIM_INFO.int_type) - ploc.size);
      }
      ploc.reg = Get_Current_Int_Preg_Num (SIM_INFO.int_args);
      Current_Int_Param_Num++;
      break;

    case MTYPE_F4:
      // Floating-point types are mapped to integer regs
      ploc.reg = Get_Current_Int_Preg_Num (SIM_INFO.int_args);
      Current_Int_Param_Num++;
      break;

    case MTYPE_A4:

      ploc.reg = Get_Current_Int_Preg_Num (SIM_INFO.int_args);
      Current_Int_Param_Num++;
      break;

    case MTYPE_I8:
    case MTYPE_U8:

      // Floating-point types are mapped to integer regs
    case MTYPE_F8:

      //
      // These occupy two registers. This must be aligned on an
      // 8-bytes boundary, so may skip a slot
      //
      ploc.reg = Get_Current_Int_Preg_Num (SIM_INFO.int_args);

      // [CG] Check if we are on an odd number of consumed registers
      if(!PLOC_on_stack(ploc) && (Current_Int_Param_Num % 2 != 0)) {
	Current_Int_Param_Num++;
	ploc.reg = Get_Current_Int_Preg_Num (SIM_INFO.int_args);
      }

      // takes two registers:
      Current_Int_Param_Num += 2;

      //
      // TODO: depending on endianness, the First_PLOC_reg() and
      //       Next_PLOC_reg(), see targ_sim_body.h, should take
      //       either first or second of these two.
      //

      /* adjust Last_Fixed_Param in varargs case */
      if (Last_Fixed_Param < INT_MAX)
	++Last_Fixed_Param;

      break;
	
    case MTYPE_FQ:

      FmtAssert(FALSE,("FQ are passed"));
      break;
      
    case MTYPE_C4:
    case MTYPE_C8:
    case MTYPE_CQ:

      FmtAssert(FALSE,("Complex are passed"));
      break;
	
    case MTYPE_M:
        {
	  if(CG_struct_by_ref) {
	    ploc.by_reference = 1;

	    //
	    // When passed by reference, we reserve one pointer register slot
	    //
	    ploc.size = MTYPE_RegisterSize(MTYPE_A4);
	    
	    ploc.reg = Get_Current_Int_Preg_Num (SIM_INFO.int_args);
	    Current_Int_Param_Num++;
	  }
	  else {
	    //
	    // When passed by value, parameters will be put in the
	    // register slots as many as can fit and the rest copied
	    // on stack.
	    //
	    ploc.size = TY_size (ty);
	    
	    // Calculate the number of parameter slots needed
	    // A parameter slot is 4 bytes in size
	    //
	    INT psize = TY_size (ty) / 4;
	    // round up
	    if ((TY_size (ty) % MTYPE_RegisterSize(SIM_INFO.int_type)) != 0)
	      psize++;
	    
	    ploc.reg = Get_Current_Int_Preg_Num (SIM_INFO.int_args);
	    
	    // Structures are padded to a multiple of 32 bit.
	    rpad = (psize * 4) - ploc.size;
	    
	    // Structures over 4 bytes are aligned on a 8-byte boundary
	    //if (psize > 1 && Get_Preg_Alignment(ploc.reg) == 4) {
	    
	    if (psize > 1 && PLOC_offset(ploc) % 8 == 4) {
	      //
	      // skip one slot if the next still fits the register list
	      //
	      Current_Int_Param_Num++;
	      lpad = 4;
	      ploc.reg = Get_Current_Int_Preg_Num (SIM_INFO.int_args);
#if 0
	      /* [JV] Now offsets are computed in Update_Stack_Offsets.
	       * In case of struct passed by value. It is necessary to 
	       * compute the offset in formal area and also in up formal
	       * area if the struct is bigger than formal area free space.
	       * Allocation of object on stack should not be performed via
	       * up formal size update in case of overlap between formal and upformal,
	       * because this does not work in case of multiple register files with 
	       * alignment constraints.
	       * TODO: Set formal and upformal offsets correctly in Update_Stack_Offsets
	       * when MTYPE_M is seen.
	       */
	      if(PLOC_on_stack(ploc)) {
		Current_UpFormal_Offset += lpad;
	      }
	      else {
		Current_Formal_Offset += lpad;
	      }
#endif
	    }
	    
	    // takes psize parameter slots:
	    Current_Int_Param_Num += psize;
	    
	    /* adjust Last_Fixed_Param in varargs case */
	    if (Last_Fixed_Param < INT_MAX)
	      Last_Fixed_Param += psize - 1;
	  }
	}
	break;
	
    default:
	FmtAssert (FALSE, ("Get_Parameter_Location:  unknown mtype %s",
			   MTYPE_name(pmtype)));
  }

  ploc.lpad = lpad;
  ploc.rpad = rpad;

  Update_Stack_Offsets(pmtype ,&ploc);
#if 0
  if(PLOC_on_stack(ploc)) {
    ploc.formal_offset = Current_Formal_Offset;
    ploc.upformal_offset = Current_UpFormal_Offset;
    Current_UpFormal_Offset = ploc.upformal_offset + ploc.size + rpad;
  }
  else {
    ploc.formal_offset = Current_Formal_Offset;
    Current_Formal_Offset = ploc.formal_offset + ploc.size + rpad;
  }
#endif

  if(TRACE_ON) {
    fprintf(TFile,"Parameter %s: preg = %d, lpad = %d, rpad = %d,"
	    "offset = %d, size = %d, Current_Formal_Offset = %d,"
	    "Current_UpFormal_Offset = %d\n",
	    MTYPE_name(pmtype),ploc.reg,ploc.lpad,ploc.rpad,
	    PLOC_offset(ploc),ploc.size,
	    Current_Formal_Offset,Current_UpFormal_Offset);
  }

  return ploc;
} 

/* ====================================================================
 *           Structure stuff:
 * ====================================================================
 */
struct PSTRUCT {
    BOOL	is_struct;
    BOOL        first_call;
    INT64	offset;			// offset from beginning of struct
    INT64	size;

    PSTRUCT () : is_struct (FALSE), first_call (TRUE),
		 offset (0), size (0) {}
};

static PSTRUCT pstruct;

#define	PSTRUCT_struct		pstruct.is_struct
#define	PSTRUCT_first_call	pstruct.first_call
#define	PSTRUCT_offset		pstruct.offset
#define	PSTRUCT_size		pstruct.size

/* ====================================================================
 *   Setup_Struct_Parameter_Locations
 * ====================================================================
 */
static BOOL
Is_Struct_Parameter (TY_IDX struct_ty)
{
  return MTYPE_is_m(TY_mtype(Ty_Table[struct_ty])) && !CG_struct_by_ref;
}

static void
Setup_Struct_Parameter_Locations (TY_IDX struct_ty)
{
    PSTRUCT_struct = ! TY_is_union (struct_ty);
    PSTRUCT_first_call = TRUE;
    PSTRUCT_offset = 0;
    PSTRUCT_size = TY_size (struct_ty);
}

/* ====================================================================
 *   Get_Struct_Parameter_Location
 * ====================================================================
 */
static PLOC 
Get_Struct_Parameter_Location (PLOC prev)
{
  /* [JV] When struct is passed by reference, it takes only one reg
     marked by_reference. 
   */
  PLOC next;
  TYPE_ID pmtype;
  INT ireg_size = MTYPE_RegisterSize(SIM_INFO.int_type);
  BOOL	onStack = (prev.reg == 0);
  
  // [CG]{
  next.vararg_reg = 0;               // to silence purify
  next.lpad = 0;
  next.rpad = 0;
  next.by_reference = CG_struct_by_ref;
  //}
  
  if (PSTRUCT_first_call) {
    if(onStack) {
      PLOC_upformal_offset(next) = PLOC_upformal_offset(prev);
    }
    else {
      PLOC_formal_offset(next) = PLOC_formal_offset(prev);
    }
  }
  else {
    if(onStack) {
      PLOC_upformal_offset(next) = PLOC_upformal_offset(prev) + PLOC_size(prev);
    }
    else {
      PLOC_formal_offset(next) = PLOC_formal_offset(prev) + PLOC_size(prev);
    }
  }
  
  if (PSTRUCT_offset >= PSTRUCT_size) {
    PLOC_size(next) = 0;

    return next;
  }
  
  PLOC_size(next) = ireg_size;
  PSTRUCT_offset += ireg_size;

  if (onStack) {
    PLOC_reg(next) = 0;
    PSTRUCT_first_call = FALSE;
  } else if (PSTRUCT_first_call) {
    PSTRUCT_first_call = FALSE;
    PLOC_reg(next) = PLOC_reg(prev);
    if (!IS_INT_PREG(PLOC_reg(next)))
      PLOC_reg(next) = 0;
  } else if (IS_INT_PREG(PLOC_reg(prev))) {
    PLOC_reg(next) =  PLOC_reg(prev) + PR_skip_value(SIM_INFO.int_args);
    if (!IS_INT_PREG(PLOC_reg(next)))
      PLOC_reg(next) = 0;
  }

  return next;
} // Get_Struct_Parameter_Location

/* ====================================================================
 *   Get_Vararg_Parameter_Location
 *
 *   Iterate over vararg non-fixed parameters 
 * ====================================================================
 */
static PLOC
Get_Vararg_Parameter_Location (
  PLOC prev
)
{
  PLOC next;
  TYPE_ID mtype = MTYPE_I4;

  next.reg = Get_Current_Int_Preg_Num (SIM_INFO.int_args);
  if(next.reg == 0) {
    next.reg = Get_Current_Flt_Preg_Num (SIM_INFO.flt_args);
    Current_Flt_Param_Num++;
  }
  else {
    Current_Int_Param_Num++;
  }

  if (PLOC_on_stack(next)) {
    next.size = 0;
  }
  else {
    if(Preg_Offset_Is_Int(next.reg)) {
      next.size = MTYPE_RegisterSize(SIM_INFO.int_type);
      mtype = SIM_INFO.int_type;
    }
    else if(Preg_Offset_Is_Float(next.reg)) {
      next.size = MTYPE_RegisterSize(SIM_INFO.flt_type);
      mtype = SIM_INFO.flt_type;
    }
  }

  next.lpad = 0;
  next.rpad = 0;

  Update_Stack_Offsets(mtype, &next);

  if(TRACE_ON) {
    fprintf(TFile,"Vararg parameter: preg = %d, lpad = %d, rpad = %d, start_offset = %d, size = %d\n",
	    next.reg,next.lpad,next.rpad,PLOC_offset(next),next.size);
  }

  return next;}


#ifdef TARG_ST
// [SC] Return the type of a nested function trampoline.
TYPE_ID Get_Nested_Fn_Trampoline_Type ()
{
  return Make_Array_Type (MTYPE_U8, 1, 5);
}
#endif

/* ====================================================================
 *   Init_Targ_Sim
 * ====================================================================
 */

BOOL Is_Caller_Save_GP;  /* whether GP is caller-save */

// Used to account for push/pop area that may be written before the
// stack pointer is set:
// INT Stack_Offset_Adjustment;

extern void 
Init_Targ_Sim (void)
{
  Is_Caller_Save_GP = FALSE; //SIM_caller_save_gp;

  Init_Pregs();

  Init_Sim_Info();
}

