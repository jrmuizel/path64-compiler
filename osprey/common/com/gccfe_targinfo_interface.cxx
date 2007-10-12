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

#ifdef FRONT_END

#include "gccfe_targinfo_interface.h"
#include "defs.h"
#include "symtab.h"
#include "targ_sim.h"
#include "targ_isa_registers.h"
#include "targ_abi_properties.h"
#include "register_preg.h"        // For CGTARG_Regclass_Preg_Min
#include "targ_register_common.h" // For CGTARG_Register_Class_Num_From_Name
#include <ctype.h>                // For tower()
extern "C" {
extern void error (char*,...);    // From gnu
}


//TB: for extension, make ADDITIONAL_REGISTER_NAMES a real array
static gcc_register_map_t *Additional_Register_Names_Var;
static int Additional_Register_Names_Size_Var;

static int Register_Class_To_gcc_Reg[ISA_REGISTER_CLASS_MAX_LIMIT+1];

static char *Initial_Call_Used_Regs_Var;
static char *Initial_Fixed_Regs_Var;
static int Initial_Number_Of_Registers_Var;


static int *Map_Reg_To_Preg_Var;
static int Map_Reg_To_Preg_Size_Var;

static void Initialize_Gcc_reg(void);

  //Return GCC register id
static int Gp_Arg_Regnum;
static int Return_Pointer_Regnum;
static int Stack_Pointer_Regnum;


extern int GCCTARG_Additional_Register_Names_Size() {
  return Additional_Register_Names_Size_Var;
}

extern gcc_register_map_t *GCCTARG_Additional_Register_Names() {
  return Additional_Register_Names_Var;
}

extern char *GCCTARG_Initial_Call_Used_Regs() {
  return Initial_Call_Used_Regs_Var;
}

extern char *GCCTARG_Initial_Fixed_Regs() {
  return Initial_Fixed_Regs_Var;
}

extern int GCCTARG_Initial_Number_Of_Registers() {
  return Initial_Number_Of_Registers_Var;
}

extern int *GCCTARG_Map_Reg_To_Preg() {
  return Map_Reg_To_Preg_Var;
}

extern int GCCTARG_Map_Reg_To_Preg_Size() {
  return Map_Reg_To_Preg_Size_Var;
}

//Return GCC register id
extern int GCCTARG_Gp_Arg_Regnum(void){
  return Gp_Arg_Regnum;
}
extern int GCCTARG_Return_Pointer_Regnum(void) {
  return Return_Pointer_Regnum;
}
extern int GCCTARG_Stack_Pointer_Regnum(void) {
  return Stack_Pointer_Regnum;
}

extern int GCCTARG_Static_last_Regnum() {
  return Get_Static_Last_Dedicated_Preg_Offset() - 1;
}

extern int GCCTARG_First_Branch_Regnum() {
  return Branch_Preg_Min_Offset - 1;
}

extern int GCCTARG_Last_Branch_Regnum() {
  return Branch_Preg_Max_Offset - 1;
}

extern int GCCTARG_First_Int_Regnum() {
  return Int_Preg_Min_Offset - 1;
}

extern int GCCTARG_Last_Int_Regnum() {
  return Int_Preg_Max_Offset - 1;
}

extern void
GCCTARG_Configure_Gcc_From_Targinfo(void) {
  // Initialize targinfo abi properties
  GCC_Configure_ABI ();

  //Targinfo initialization
  TI_Initialize_Extension_Loader();

  ABI_PROPERTIES_Initialize();

  //Initialize registers stuff
  Initialize_Gcc_reg();
}

//TB: Returns an array of registers num  
static void
Initialize_Gcc_reg(void) {
  ISA_REGISTER_CLASS rclass;
  //Copute total nb pf registers
  INT total_nb_of_regs = 0;
  FOR_ALL_ISA_REGISTER_CLASS( rclass ) {
    const ISA_REGISTER_CLASS_INFO *icinfo = ISA_REGISTER_CLASS_Info(rclass);
    INT first_isa_reg  = ISA_REGISTER_CLASS_INFO_First_Reg(icinfo);
    INT last_isa_reg   = ISA_REGISTER_CLASS_INFO_Last_Reg(icinfo);
    INT register_count = last_isa_reg - first_isa_reg + 1;
    total_nb_of_regs += register_count;
  }
  Additional_Register_Names_Var = TYPE_MEM_POOL_ALLOC_N(gcc_register_map_t, Malloc_Mem_Pool,
							 total_nb_of_regs);
  Additional_Register_Names_Size_Var = total_nb_of_regs;

  Initial_Call_Used_Regs_Var = TYPE_MEM_POOL_ALLOC_N(char, Malloc_Mem_Pool,
							 total_nb_of_regs);
  Initial_Fixed_Regs_Var = TYPE_MEM_POOL_ALLOC_N(char, Malloc_Mem_Pool,
							 total_nb_of_regs);
  Initial_Number_Of_Registers_Var = total_nb_of_regs;


  Map_Reg_To_Preg_Var = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool,
					       total_nb_of_regs);
  Map_Reg_To_Preg_Size_Var = total_nb_of_regs;

  int gcc_index = 0;
  // Compute arrays needed by gcc
  FOR_ALL_ISA_REGISTER_CLASS( rclass ) {
    INT32 i;
    const ISA_REGISTER_CLASS_INFO *icinfo = ISA_REGISTER_CLASS_Info(rclass);
    INT first_isa_reg  = ISA_REGISTER_CLASS_INFO_First_Reg(icinfo);
    INT last_isa_reg   = ISA_REGISTER_CLASS_INFO_Last_Reg(icinfo);
    INT register_count = last_isa_reg - first_isa_reg + 1;
    const char        *rcname         = ISA_REGISTER_CLASS_INFO_Name(icinfo);
    int preg_min = CGTARG_Regclass_Preg_Min(rclass);
    int preg_max = CGTARG_Regclass_Preg_Max(rclass);
    FmtAssert (((preg_max - preg_min) == register_count - 1),
      ("PREG range is not compatible with register class info for %d reg cladd", rclass));
    Register_Class_To_gcc_Reg[rclass] = gcc_index;
    for (i = 0; i < register_count; ++i) {
      INT      isa_reg        = i + first_isa_reg;
      const char *reg_name = ISA_REGISTER_CLASS_INFO_Reg_Name(icinfo, isa_reg);
      char *temp = (char*)malloc(strlen(rcname) + strlen(reg_name) + 3);
      sprintf(temp,"%%%s%%%s",rcname, reg_name);
      //lower reg_name
      int j;
      for (j = strlen(rcname) + 2; j < strlen(temp); j++) {
	temp[j] = tolower(temp[j]);
      }
      BOOL is_allocatable = ABI_PROPERTY_Is_allocatable(rclass, isa_reg);
      BOOL is_caller = ABI_PROPERTY_Is_caller(rclass, isa_reg);
      BOOL is_stack_ptr = ABI_PROPERTY_Is_stack_ptr(rclass, isa_reg);
      BOOL is_frame_ptr = ABI_PROPERTY_Is_frame_ptr(rclass, isa_reg);
      BOOL is_ret_addr = ABI_PROPERTY_Is_ret_addr(rclass, isa_reg);
      BOOL is_zero = ABI_PROPERTY_Is_zero(rclass, isa_reg);
      BOOL is_true_predicate = ABI_PROPERTY_Is_true_predicate(rclass, isa_reg);
      BOOL gcc_is_fixed = is_stack_ptr || is_ret_addr || is_zero || is_true_predicate || !is_allocatable;
      BOOL is_func_arg  = ABI_PROPERTY_Is_func_arg(rclass, isa_reg);

      Additional_Register_Names_Var[gcc_index].name = temp;
      Additional_Register_Names_Var[gcc_index].number = gcc_index;
      Additional_Register_Names_Var[gcc_index].disabled = 0;
      //For the moment all regs are not call used.
      Initial_Call_Used_Regs_Var[gcc_index] = gcc_is_fixed || is_caller;
      Initial_Fixed_Regs_Var[gcc_index] = gcc_is_fixed;

      if (is_ret_addr) Return_Pointer_Regnum = gcc_index;
      if (is_func_arg && Gp_Arg_Regnum == 0) Gp_Arg_Regnum = gcc_index;
      if (is_stack_ptr) Stack_Pointer_Regnum = gcc_index;

      Map_Reg_To_Preg_Var[gcc_index] = preg_min + i;
      gcc_index++;
    }
    FmtAssert (((Map_Reg_To_Preg_Var[gcc_index - 1]) == preg_max),
      ("PREG range is not compatible with register class info for %d reg cladd", rclass));
  }
}


//Return the open64 couple (rclass, reg number) associated to gcc
//register index gcc_index
void GCCTARG_Get_Rclass_From_Gcc_Reg(int gcc_index,ISA_REGISTER_CLASS *rclass, int *regnum) {
  *rclass = CGTARG_Register_Class_Num_From_Name((const char*)Additional_Register_Names_Var[gcc_index].name, regnum);
}


// From the register name passed on argument, retrieve the associated
// Gcc register id and mark it with 'disabled' flag.
static void Disable_Gcc_Reg(const char *regname) {
  int regnum;
  ISA_REGISTER_CLASS rclass = CGTARG_Register_Class_Num_From_Name(regname, &regnum);

  if (rclass == ISA_REGISTER_CLASS_UNDEFINED) {
    error("Invalid disabled register `%s'", regname);
    return;
  }

  int gcc_index = Register_Class_To_gcc_Reg[rclass] + regnum;

  Additional_Register_Names_Var[gcc_index].disabled = 1;
}

// From the 2 register names passed on argument, retrieve the associated
// Gcc register ids and mark all register in range [reg1, reg2] with
// 'disabled' flag.
static void Disable_Gcc_Reg_Range(const char *regname1, const char *regname2) {
  int i;
  int regnum1, regnum2;
  ISA_REGISTER_CLASS rclass1, rclass2;

  rclass1 = CGTARG_Register_Class_Num_From_Name(regname1, &regnum1);
  rclass2 = CGTARG_Register_Class_Num_From_Name(regname2, &regnum2);

  if (rclass1 == ISA_REGISTER_CLASS_UNDEFINED || rclass1 != rclass2 || regnum1 > regnum2) {
    error("Invalid disabled register range `%s-%s'", regname1, regname2);
    return;
  }

  int gcc_index_min = Register_Class_To_gcc_Reg[rclass1] + regnum1;
  int gcc_index_max = Register_Class_To_gcc_Reg[rclass1] + regnum2;

  for (i = gcc_index_min; i <= gcc_index_max; i++) {
    Additional_Register_Names_Var[i].disabled = 1;
  }
}

// Mark disabled Gcc registers based on command line option -mdisabled-reg
extern void GCCTARG_Mark_Disabled_Gcc_Reg() {
  OPTION_LIST *ol = Disabled_Registers;
  char *start;
  char *p;
  char regname[16];
  char regname2[16];

  // Go through command-line list
  if (ol == NULL) return;
  for ( ; ol != NULL; ol = OLIST_next(ol)) {

    /* Check for commas and ranges: */
    p = OLIST_val(ol);
    start = p;
    while ( *p != ':' && *p != 0 ) {
      if ( *p == ',') {
	strncpy (regname, start, p-start+1);
	regname[p-start] = '\0';
	Disable_Gcc_Reg(regname);
	++p;
	start = p;
      }
      else if (*p == '-' ) {
	strncpy (regname, start, p-start+1);
	regname[p-start] = '\0';
	++p;
	start = p;
	while (*p != ',' && *p != '\0') {
	  ++p;
	}
	strncpy (regname2, start, p-start+1);
	regname2[p-start] = '\0';
	Disable_Gcc_Reg_Range(regname, regname2);
	if (*p == 0) {
	  start = 0;
	  break;
	}
	++p;
	start = p;
      }
      else {
	++p;
      }
    }
    if (start != 0) {
      strncpy (regname, start, p-start+1);
      Disable_Gcc_Reg(regname);
    }
  }
  return;
}


//[TB] Return the dwarf id associated to gcc register gcc_reg
int GCCTARG_Dwarf_Get_Reg_Id_From_Gcc_Reg(int gcc_reg) {
  ISA_REGISTER_CLASS rclass;
  int regnum;
  //Mapping between gcc numbering and open64
  //regclass,regnum
  GCCTARG_Get_Rclass_From_Gcc_Reg(gcc_reg, &rclass, &regnum);
  // [TB]Get bit size of this register class This size is used
  // to get the dwarf ID. Indeed paired registers do not
  // have the same id than classical registers. bit size is
  // used to differenciate them.
  const ISA_REGISTER_CLASS_INFO *cinfo;
  cinfo = ISA_REGISTER_CLASS_Info(rclass);
  INT32 regclass_bit_size = ISA_REGISTER_CLASS_INFO_Bit_Size(cinfo);
  //Now get the dward id 
  int v = CGTARG_DW_DEBUG_Get_Reg_Id(rclass,
				     regnum,
				     regclass_bit_size);
  return v;
}


#endif //FRONT_END
