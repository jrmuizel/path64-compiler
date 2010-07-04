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
	/*
 * API used by both Front-end and Back-end to load extension dlls
 * and initialize common structures (mtypes, intrinsics)
 */

#include <string>

#include "extension_include.h"
#include "extension_intrinsic.h"


#include "dll_loader.h"
#include "loader.h"
#include "erglob.h"
//TB Extend PREG registers to extension
extern "C"
{
  BE_EXPORTED extern   INT Get_Static_Last_Dedicated_Preg_Offset(void);
  BE_EXPORTED extern   INT Get_Last_Dedicated_Preg_Offset_Func(void);
  BE_EXPORTED extern   void Set_Last_Dedicated_Preg_Offset(INT offset);
}

#ifdef BACK_END
#include "dyn_isa_api.h"
extern "C" { extern const ISA_EXT_Interface_t* get_ISA_extension_instance();};
#endif
#include "errors.h"
#include "config.h"
#include "intrn_info.h"
#include "tracing.h"
#include "dso.h"
#include "mtypes.h"
#include "wn.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "opcode.h"
#include "lai_loader_api.h"
#include "pixel_mtypes.h"
#include "ext_info.h"
#include "config_TARG.h"


#if defined(__MINGW32__) || defined(__CYGWIN__)
#define SO_EXT ".dll"
#else
#define SO_EXT ".so"
#endif

#if defined(BACK_END) 
#include "targ_isa_registers.h"
#include "targ_isa_subset.h"
#endif

// Extension rank is used to access a given extension in internal 
// array such as extension_tab and extension_extra_info_tab.
#define INVALID_EXTENSION_RANK (-1)

// Variables common to gcc and lai loader
// --------------------------------------
// Table containing MMODE to MTYPE mapping
static TYPE_ID *Extension_MachineMode_To_Mtype_Array;
static int Extension_MachineMode_To_Mtype_Array_Size=0;

/*
 * Table containing all loaded extension
 */
static Extension_dll_t *extension_tab = (Extension_dll_t*)NULL;
static int extension_count = 0;

Extension_dll_t * Get_Extension_dll_tab( ) { return extension_tab; }
int Get_Extension_dll_count( ) { return extension_count; }

#include <map>

/**
 * Variables containing the pattern recognition rules
 * 
 */
static ListOfRules*  extension_pattern_rules = NULL;
static std::map<OPCODE,ListOfRules*> rules_per_opcode;
static std::map<INTRINSIC,ListOfRules*> rules_per_intrinsic;

/** 
 * api to access to the pattern rec rules by opcode
 * 
 * @param opc 
 * 
 * @return 
 */
ListOfRules*  Get_rules_per_opcode(OPCODE opc){
  return rules_per_opcode[opc];
}

/** 
 * api to access to the pattern rec rules by opcode
 * 
 * @param opc 
 * 
 * @return 
 */
ListOfRules*  Get_rules_per_intrinsic(INTRINSIC intrn){
  return rules_per_intrinsic[intrn];
}
/** 
 * api to access all pattern rec rules
 * 
 * 
 * @return 
 */
ListOfRules*  Get_Extension_pattern_rules(){
  return extension_pattern_rules;
}

/** 
 * appends new rule to map.
 * 
 * @param opc 
 * @param rule 
 * 
 * @return 
 */
static bool
Add_Extension_PatternRule(recog_rule* rule) {
  FmtAssert((rule!=NULL), ("cannot add a NULL rule\n"));
  
  if (extension_pattern_rules == NULL) {
    extension_pattern_rules = new ListOfRules();
  }

  rule->intrn = EXTENSION_INTRINSIC_From_Name(rule->builtin_name);
  extension_pattern_rules->push_back(rule);

  if (rules_per_intrinsic[rule->intrn] == NULL) {
    rules_per_intrinsic[rule->intrn] = new ListOfRules();
  }
  rules_per_intrinsic[rule->intrn]->push_back(rule);

  OPCODE opc = rule->pattern->u.opc;  
  if (rules_per_opcode[opc] == NULL) {
    rules_per_opcode[opc] = new ListOfRules();
  }
  rules_per_opcode[opc]->push_back(rule);

  return true;
}

typedef std::map<OPCODE, INTRINSIC_Vector_t*> INTRINSIC_OPCODE_MAP_t;
static INTRINSIC_OPCODE_MAP_t Intrinsic_from_OPCODE;

/** 
 * return a vector of intrinsic ids corresponding to opcode.
 * 
 * @param opc 
 * 
 * @return 
 */
extern INTRINSIC_Vector_t* Get_Intrinsic_from_OPCODE(OPCODE opc) {
  if (Intrinsic_from_OPCODE.count(opc)>0)
    return Intrinsic_from_OPCODE[opc];
  return NULL;
}

/** 
 * add the intrinsic idx given in parameter to the
 * Intrinsic_from_OPCODE map
 * 
 * @param opc 
 * @param intrn 
 * 
 */

extern void Add_Intrinsic_for_OPCODE(OPCODE opc, INTRINSIC intrn) {
  if (! Intrinsic_from_OPCODE.count(opc)>0) {
    Intrinsic_from_OPCODE[opc] = new INTRINSIC_Vector_t();
  }
  Intrinsic_from_OPCODE[opc]->push_back(intrn);
}


mUINT8 pixel_size_per_type[MTYPE_MAX_LIMIT+1];

// Information on type equivalence and corresponding conversion builtins
// between C native types and extension
typedef struct {
  TYPE_ID   ctype;
  INTRINSIC intrn_to_ext;
  INTRINSIC intrn_to_c;
  INTRINSIC clr_intrn;
  INTRINSIC from_u32_intrn; // Unsigned 32bits to ext register
} equiv_type_t;
static equiv_type_t equiv_type_tab[MTYPE_MAX_LIMIT+1];

// Information about native codegen per extension
typedef struct {
  BOOL default_extgen_enabled;  // default status
  BOOL extgen_enabled;          // status for current PU
  BOOL equiv_type_enabled;      // status for current PU
  INT64 default_extoption_flags;    // option flags for extension
  INT64 extoption_flags;    // option flags for extension
} Extension_Extra_Info_t;
static Extension_Extra_Info_t *extension_extra_info_tab = NULL;

/*
 * Return TRUE if extension with name <extname> is known by the compiler.
 */
BOOL EXTENSION_Is_Defined(const char *extname) {
  int ext;
  for (ext=0; ext<extension_count; ext++) {
    if (strcmp(extension_tab[ext].handler->extname, extname)==0) {
      return TRUE;
    }
  }
  return FALSE;
}

/*
 * Return a unique id based on extension name
 */
INT32 EXTENSION_Get_ExtensionId_From_ExtensionName(const char *extname) {
  INT32 id = 0;
  const char *p = extname;
  int count = 0;
  while (*p != 0) {
    id = (id << 8) + *p;
    p++; count++;
  }
  if (count < 1 || count > 4) {
    DevWarn("Extension name must contain between 1 and 4 characters");
    id = INVALID_EXTENSION_ID;
  }
  return id;
}

/*
 * Return the rank used by extension with id <extid> in internal tables
 */
static int EXTENSION_Get_ExtensionRank_From_ExtensionId(INT32 extid) {
  int i;
  for (i=0; i<extension_count; i++) {
    if (extension_tab[i].extension_id == extid) {
      return i;
    }
  }
  return INVALID_EXTENSION_RANK;
}

static void Add_Mtype(machine_mode_t mmode, const char *name, 
		      enum mode_class mclass, unsigned short mbitsize, 
		      unsigned short msize, unsigned short munitsize,
                      /* [vcdv] add pixel_size info to mtype */
                      unsigned char pixel_size,
		      unsigned char mwidermode, machine_mode_t innermode, 
		      TYPE_ID mtype, unsigned short alignement) 
{
  /* Update open64 mtype*/
  if (mtype <= MTYPE_COUNT) {
    // MTYPE already exists, nothing to do
    return;
  }
  FmtAssert (mtype < MTYPE_MAX_LIMIT,
	     ("Too many MTYPES. Limit is %d", MTYPE_MAX_LIMIT));

  MTYPE_COUNT++;
  Machine_Types[MTYPE_COUNT].id = mtype;
  Machine_Types[MTYPE_COUNT].bit_size = mbitsize;
  Machine_Types[MTYPE_COUNT].byte_size = msize;
  Machine_Types[MTYPE_COUNT].dummy1 = msize;
  Machine_Types[MTYPE_COUNT].alignment = alignement;
  Machine_Types[MTYPE_COUNT].dummy2 = 0;
  Machine_Types[MTYPE_COUNT].dummy3 = 0;
  Machine_Types[MTYPE_COUNT].signed_type = 0;
  Machine_Types[MTYPE_COUNT].float_type = 0;
  Machine_Types[MTYPE_COUNT].dummy4 = 0;
  Machine_Types[MTYPE_COUNT].name = (char*)name;

  /* [vcdv] add pixel_size info to mtype */
  if (MTYPE_COUNT>MTYPE_STATIC_LAST)
    MTYPE_pixel_size(MTYPE_COUNT) = pixel_size;
   
  // MMODE=>MTYPE mapping
  // Set the associated MTYPE
  if (mmode != -1)
    Extension_MachineMode_To_Mtype_Array[mmode - STATIC_COUNT_MACHINE_MODE] = mtype;
  mUINT8 type_class = 0;
  switch (mclass) { 
  case  MODE_RANDOM:
    type_class |= MTYPE_CLASS_UNKNOWN;
    break;
  case MODE_INT:
    type_class |= MTYPE_CLASS_INTEGER;
    break;
  case MODE_FLOAT:
    type_class |= MTYPE_CLASS_FLOAT;
    break;
  case MODE_COMPLEX_INT:
    type_class |= MTYPE_CLASS_COMPLEX | MTYPE_CLASS_INTEGER;
    break;
  case MODE_COMPLEX_FLOAT:
    type_class |= MTYPE_CLASS_COMPLEX | MTYPE_CLASS_FLOAT;
    break;
  case MODE_VECTOR_INT:
    type_class |= MTYPE_CLASS_RANDOM;
    //    type_class |= MTYPE_CLASS_VECTOR | MTYPE_CLASS_INTEGER;
    break;
  case MODE_VECTOR_FLOAT:
    type_class |= MTYPE_CLASS_RANDOM;
    //type_class |= MTYPE_CLASS_VECTOR | MTYPE_CLASS_FLOAT;
    break;
  case MODE_PARTIAL_INT:
  case MODE_CC:
  case MAX_MODE_CLASS:
  default:
    FmtAssert (FALSE,
	       ("Unknown translation between GCC mode class and OPEN64 MTYPE class %d", mclass));
    break;
  }
  Machine_Types[MTYPE_COUNT].type_class = type_class;
  Machine_Types[MTYPE_COUNT].type_order = mbitsize;
  Machine_Types[MTYPE_COUNT].complement = mtype;
}


/*
 * Extend MType list with all mtype defined by the specified dll
 * instance.  Returns the base MTYPE for this extension and a mapping
 * between mtypes and local register class for this extension.
 */
TYPE_ID Add_MTypes(const Extension_dll_t *dll_instance,
		   int **mtype_to_locrclass,
		   int *mtype_count,
		   BOOL verbose) {
  unsigned int i;
  unsigned int count = dll_instance->hooks->get_modes_count();
  const extension_machine_types_t* modes = dll_instance->hooks->get_modes();
  //Local mapping between mtype and local rclass, needed bu the front
  //end to get the symbol associated to a PREG
  int *mtype_to_local_rclass;
  if (mtype_to_locrclass)
    mtype_to_local_rclass = TYPE_MEM_POOL_ALLOC_N(int, Malloc_Mem_Pool, count);
  TYPE_ID base_mtype = MTYPE_COUNT;
  for (i = 0; i < count; i++) {
    TYPE_ID new_mtype = DLL_TO_GLOBAL_MTYPE(dll_instance, modes[i].mtype);
    machine_mode_t new_mmode = DLL_TO_GLOBAL_MMODE(dll_instance, modes[i].mmode);
    
    if (verbose) {
      fprintf(TFile, "  Extension '%s': Associate dynamic Mtype '%s'"
	      " to Mtype id %d (machine mode: %d)\n",
	      dll_instance->handler->extname, modes[i].name, new_mtype, new_mmode);
    }
    Add_Mtype(new_mmode, 
	      modes[i].name, 
	      modes[i].mclass, 
	      modes[i].mbitsize,
	      modes[i].msize,
	      modes[i].munitsize,
#ifdef TARG_ST
              /* [vcdv] add pixel_size info to mtype */
              modes[i].mpixelsize,
#endif
	      modes[i].mwidermode, 
	      modes[i].innermode,
	      new_mtype,
	      modes[i].alignment);
    // Here, the new MTYPE has the id MTYPE_COUNT
    // Keep a mapping between this MTYPE and its local register file
    if (mtype_to_locrclass)
      mtype_to_local_rclass[i] = modes[i].local_REGISTER_CLASS_id;
  }
  /* [VCdV] reset pixel_size to zero for all static types */
  for (i=0; i<=MTYPE_STATIC_LAST; i++) {
    MTYPE_pixel_size(i)=0;
  }
  
  if (mtype_to_locrclass)
    *mtype_to_locrclass = mtype_to_local_rclass;
  if (mtype_count)
    *mtype_count = count;
  return base_mtype + 1;
}

/*
 * Add a single intrinsic to the intrinsic list
 */
static void
Add_Intrinsic(const Extension_dll_t *dll_instance,
	      const extension_builtins_t* btypes,
	      BOOL verbose) {
  INTRINSIC intrn_id = INTRINSIC_COUNT + 1;
  
  intrn_info[intrn_id].is_by_val           = btypes->is_by_val;
  intrn_info[intrn_id].is_pure             = btypes->is_pure;
  intrn_info[intrn_id].has_no_side_effects = btypes->has_no_side_effects;
  intrn_info[intrn_id].never_returns       = btypes->never_returns;
  intrn_info[intrn_id].is_actual           = btypes->is_actual;
  intrn_info[intrn_id].is_cg_intrinsic     = btypes->is_cg_intrinsic;
  intrn_info[intrn_id].c_name              = (char*)btypes->c_name;
  intrn_info[intrn_id].specific_name       = (char*)NULL;
  /* extension intrinsics cannot be called as runtimes */
  intrn_info[intrn_id].runtime_name        = (char*)NULL;


  EXTENSION_add_INTRINSIC_to_Map(intrn_info[intrn_id].c_name, intrn_id);

  // Complete proto info for this intrinsic
  proto_intrn_info_t *proto_info =
    &Proto_Intrn_Info_Array[INTRINSIC_COUNT - INTRINSIC_STATIC_COUNT];
  proto_info->argument_count = btypes->arg_count;
  
  FmtAssert(btypes->arg_count <= INTRN_MAX_ARG,
	    ("Intrinsic %s exceeds the limit of supported number of parameters (%d)\n.",
	     btypes->c_name, INTRN_MAX_ARG));
  // Add Mtype info: Convert machine mode proto info coming from the
  // dll into MTYPE. We need this info only for out and inout
  // parameters.
  TYPE_ID outmtype[INTRN_MAX_ARG];
  for (int arg = 0; arg < btypes->arg_count; arg++) {
    outmtype[arg] = MachineMode_To_Mtype(DLL_TO_GLOBAL_MMODE(dll_instance, btypes->arg_type[arg]));
  }
  INTRN_INOUT_TYPE inouttype[INTRN_MAX_ARG];
  int nb_out = 0;
  int nb_in = 0;
  for (int arg = 0; arg < btypes->arg_count; arg++) {
    FmtAssert(btypes->arg_inout[arg] != BUILTARG_UNDEF,
	      ("Undef parameter type for intrinsic %s\n.", btypes->c_name));
    if (btypes->arg_inout[arg] == BUILTARG_OUT || btypes->arg_inout[arg] == BUILTARG_INOUT)
      nb_out++;
    if (btypes->arg_inout[arg] == BUILTARG_IN || btypes->arg_inout[arg] == BUILTARG_INOUT)
      nb_in++;
    // enum INTRN_INOUT_TYPE and enum BUILTARG_INOUT_TYPE are in the
    // same order, so just cast to map.
    inouttype[arg] = (INTRN_INOUT_TYPE)btypes->arg_inout[arg];
  }
  
  proto_info->arg_type = TYPE_MEM_POOL_ALLOC_N(TYPE_ID, Malloc_Mem_Pool, btypes->arg_count);
  memcpy(proto_info->arg_type, outmtype, btypes->arg_count*sizeof(TYPE_ID));
  proto_info->arg_inout = TYPE_MEM_POOL_ALLOC_N(INTRN_INOUT_TYPE, Malloc_Mem_Pool, btypes->arg_count);
  memcpy(proto_info->arg_inout, inouttype, btypes->arg_count*sizeof(INTRN_INOUT_TYPE));
  
  proto_info->argument_count = btypes->arg_count;
  proto_info->arg_out_count  = nb_out;
  proto_info->arg_in_count   = nb_in;
    
  if (btypes->return_type != VOIDmode && (nb_out != 0)) {
    char err_msg[256];
    sprintf(err_msg,
	    "Incompatible prototype for built-in %s. "
	    "A built-in cannot have out parameters and not returning void.",
	    btypes->c_name);
    RaiseErrorIncompatibleLibrary(dll_instance->handler->dllname, err_msg);
  }
  
  // Initialize returned type
  TYPE_ID ret_mtype = MachineMode_To_Mtype(DLL_TO_GLOBAL_MMODE(dll_instance, btypes->return_type));
  proto_info->return_type = ret_mtype;
  intrn_info[intrn_id].return_kind = INTRN_return_kind_for_mtype(ret_mtype);

  // Build tables used for code generation
  {
    if (is_DYN_INTRN_CLR(*btypes)) {
      equiv_type_tab[ret_mtype].clr_intrn = intrn_id;
    }
    
    if (is_DYN_INTRN_CONVERT_FROM_U32(*btypes)) {
      TYPE_ID ext_ty =  MachineMode_To_Mtype(btypes->return_type);
      equiv_type_tab[ext_ty].from_u32_intrn = intrn_id;
    }
    
    if (is_DYN_INTRN_CONVERT_TO_CTYPE(*btypes)) {
      TYPE_ID ext_ty = proto_info->arg_type[0];
      TYPE_ID c_ty   = ret_mtype;
      equiv_type_tab[ext_ty].ctype = MTYPE_is_signed(c_ty)?MTYPE_complement(c_ty):c_ty; // Register unsigned type
      equiv_type_tab[ext_ty].intrn_to_c = intrn_id;
    }
    else if (is_DYN_INTRN_CONVERT_FROM_CTYPE(*btypes)) {
      TYPE_ID c_ty   = proto_info->arg_type[0];
      TYPE_ID ext_ty = ret_mtype;
      equiv_type_tab[ext_ty].ctype = MTYPE_is_signed(c_ty)?MTYPE_complement(c_ty):c_ty; // Register unsigned type
      equiv_type_tab[ext_ty].intrn_to_ext = intrn_id;
    }
    
    if (btypes->wn_table != NULL)  {
      int k=0;
      while ( (OPCODE)(btypes->wn_table[k].wn_opc) != OPCODE_UNKNOWN ) {
	OPCODE opc =  (OPCODE)(btypes->wn_table[k].wn_opc);
	Add_Intrinsic_for_OPCODE(opc, intrn_id);
	k++;
      }
    }
  }

  if (verbose) {
    fprintf(TFile, "    intrinsic[%d] = %s\n",
	    (intrn_id), (char*)btypes->runtime_name);
  }
  INTRINSIC_COUNT++;
}

/*
 * Extend intrinsic list with all intrinsics defined by the
 * specified dll instance
 */
void Add_Intrinsics(const Extension_dll_t *dll_instance, BOOL verbose)
{
  unsigned int i;
  unsigned int count = dll_instance->hooks->get_builtins_count();
  const extension_builtins_t* btypes = dll_instance->hooks->get_builtins();
  if (verbose) {
    fprintf(TFile, "  Extension '%s': Adding %d intrinsics starting at id %d\n",
	    dll_instance->handler->extname, count, INTRINSIC_COUNT + 1);
  }
  for (i = 0; i < count; i++) {
    Add_Intrinsic(dll_instance, &btypes[i], verbose);
  }
}

/*
 * Extend rules list with all instrinsics defined by the
 * specified dll instance
 */
static void
Add_RecRules(const Extension_dll_t *dll_instance, BOOL verbose)
{
  unsigned int i;

  char *extname = dll_instance->handler->extname;;
  
  recog_rule** rules = dll_instance->hooks->get_recrules();
  unsigned int rules_count = dll_instance->hooks->get_recrules_count();

  if ( rules != NULL ) {
    int k = 0;
    for (k=0; k<rules_count; k++) {
      recog_rule* rule = rules[k];
      /* append rule to rules_per_opcode map */        
      Add_Extension_PatternRule(rule);
    }
  }
}


//TB: Add a new pass to create new MTYPE for mutiple result intrinsics
void Add_Composed_Mtype() {
  //Run thru added intrinsics to create new composed mtype
  for (INTRINSIC intr = INTRINSIC_STATIC_COUNT+1; intr <= INTRINSIC_COUNT; intr++)
    {
      char *newmtype_name = TYPE_MEM_POOL_ALLOC_N(char, Malloc_Mem_Pool,
						  1024);
      newmtype_name[0]='.';
      newmtype_name[1]='\0';
      proto_intrn_info_t *proto = INTRN_proto_info(intr);
      //proto->return_type has already been initialized in
      //add_intrinsics For multiple results intrinsics, we update
      //return_type with the newly created composed MTYPE
      if (INTRN_number_of_out_param(proto) <= 1) {
	for (int arg = 0; arg < proto->argument_count; arg ++)
	  if (INTRN_is_out_param(arg, proto)) {
	    proto->return_type = proto->arg_type[arg];
	    //Set return kind
	    intrn_info[intr].return_kind = INTRN_return_kind_for_mtype(proto->return_type);
	  }
	continue;
      }
      for (int i = 0; i < proto->argument_count; i++) {
	if (INTRN_is_out_param(i, proto)) {
	  strcat(newmtype_name, MTYPE_name(proto->arg_type[i]));
	  strcat(newmtype_name, ".");
	}
      }
      //Add a new MTYPE (MTYPE_COUNT +1)
      proto->return_type = MTYPE_COUNT+1;
      Add_Mtype(/*machine_mode_t*/ -1 , newmtype_name, 
		/*enum mode_class*/ MODE_RANDOM, /* mbitsize*/ 0, 
		/*msize*/ 0,  /*munitsize*/ 0,
		/*mwidermode*/ 0,
#ifdef TARG_ST
                /* [vcdv] add pixel_size info to mtype */
                /*mpixelsize*/ 0,
#endif
                /*innermode*/-1, 
		/* mtype*/ MTYPE_COUNT+1, /*alignement*/TARG_NONE_ALIGN);

      //Set return kind
      intrn_info[intr].return_kind = INTRN_return_kind_for_mtype(proto->return_type);
    }
}


/*
 * OPEN64 Mtype init
 */
void Init_Mtypes(int nb_mtype_to_add) {
  MTYPE_COUNT = MTYPE_STATIC_LAST;
  FIRST_COMPOSED_MTYPE = MTYPE_COUNT + nb_mtype_to_add + 1;
  
  if (MTYPE_STATIC_LAST + nb_mtype_to_add > MTYPE_MAX_LIMIT) {
    FmtAssert(FALSE,
	      ("Too much dynamic MTYPEs to add. Compilation aborted."));
  }
  if (nb_mtype_to_add) {
    //TB: Allocate mtype to mmode mapping for extension
    Extension_MachineMode_To_Mtype_Array = TYPE_MEM_POOL_ALLOC_N(TYPE_ID, Malloc_Mem_Pool, nb_mtype_to_add);
    Extension_MachineMode_To_Mtype_Array_Size = nb_mtype_to_add;
  }
}


/*
 * OPEN64 Intrinsics init
 */
void Init_Intrinsics(int nb_builtins_to_add)
{
  INTRINSIC_COUNT = INTRINSIC_STATIC_COUNT;
  if (nb_builtins_to_add == 0) {
    // Nothing to allocate, juste reference static tables
    intrn_info = (intrn_info_t *)intrn_info_static;
  }else {
    intrn_info = TYPE_MEM_POOL_ALLOC_N(intrn_info_t,
				       Malloc_Mem_Pool,
				       (INTRINSIC_COUNT + nb_builtins_to_add + 1));
    memcpy(intrn_info, intrn_info_static, (INTRINSIC_STATIC_COUNT+1)*sizeof(intrn_info_t));
    Proto_Intrn_Info_Array = TYPE_MEM_POOL_ALLOC_N(proto_intrn_info_t,
						   Malloc_Mem_Pool,
						   nb_builtins_to_add);
  }
}





/*
 * Load extension dlls and return a pointer to the extension table
 * and the count of loaded extensions
 */
bool Load_Extension_dlls( bool verbose ) {

  int i;
  int nb_ext_mtypes;
  int nb_ext_intrinsics;
  int base_mtypes;
  int base_mmodes;
  int base_builtins;
  int base_intrinsics;
  char *ext_fullpath_ptr;
  char *next_ext_fullpath_ptr;

  if (!Load_Extension_dll_handlers(verbose)) { return false; }

  // make table of Extension_dll_t
  extension_count = Get_Extension_dll_handler_count( );
  if (extension_count==0) { return true; }
  extension_tab = (Extension_dll_t*)calloc(sizeof(Extension_dll_t),extension_count);
  FmtAssert((extension_tab!=NULL), ("Unable to allocate memory\n"));

  extension_extra_info_tab = new Extension_Extra_Info_t[extension_count];

  // Load extension dlls, compute base offsets for each element
  // (machine modes, mtypes, intrinsics and builtins)  of  each
  // extension, and count additionnal mtypes and intrinsics.
  base_mtypes     = MTYPE_STATIC_LAST + 1;
  base_mmodes     = MACHINE_MODE_STATIC_LAST + 1;
  base_builtins   = BUILT_IN_STATIC_LAST + 1;
  base_intrinsics = INTRINSIC_STATIC_LAST + 1;
  for (i=0; i<extension_count; i++) {

    extension_tab[i].handler = Get_Extension_dll_handler( i );

    /* Retrieve machine type and builtin interface */
    get_type_extension_instance_t get_instance;
    get_instance = (get_type_extension_instance_t)Get_dll_Symbol(extension_tab[i].handler, "get_type_extension_instance");
    FmtAssert((get_instance!=NULL), ("Symbol 'get_type_extension_instance' not found in extension dll '%s'\n", extension_tab[i].handler->dllname));
    const extension_hooks_t *hooks = (*get_instance)();
    FmtAssert((hooks!=NULL), ("Missing hooks in extension dll '%s'\n", extension_tab[i].handler->dllname));
  
    // Check extension hooks compatibility and initialize the access API
    INT magic = hooks->magic;
    if (EXTENSION_Is_Supported_HighLevel_Revision(magic)) {
      if (verbose) {
        fprintf(TFile, "  HL API revision: lib=%d, compiler=%d\n", magic, MAGIC_NUMBER_EXT_API);
      }
      extension_tab[i].hooks = new EXTENSION_HighLevel_Info(hooks);
    }
    else {
      char err_msg[256];
      sprintf(err_msg, "Incompatible HL API revision: lib=%d, compiler=%d.\n", magic, MAGIC_NUMBER_EXT_API);
      RaiseErrorIncompatibleLibrary(extension_tab[i].handler->dllname, err_msg);
    }

    extension_tab[i].extension_id = EXTENSION_Get_ExtensionId_From_ExtensionName(extension_tab[i].handler->extname);
    if (verbose) {
      fprintf(TFile, "Generated identifier from name '%s' -> %d\n",
	      extension_tab[i].handler->dllname, extension_tab[i].extension_id);
    }
  
    extension_tab[i].base_mtypes     = base_mtypes;
    extension_tab[i].base_mmodes     = base_mmodes;
    extension_tab[i].base_builtins   = base_builtins;
    extension_tab[i].base_intrinsics = base_intrinsics;
    nb_ext_mtypes     = extension_tab[i].hooks->get_modes_count();
    nb_ext_intrinsics = extension_tab[i].hooks->get_builtins_count();
    base_mtypes     += nb_ext_mtypes;
    base_mmodes     += nb_ext_mtypes;
    base_builtins   += nb_ext_intrinsics;
    base_intrinsics += nb_ext_intrinsics;

    extension_extra_info_tab[i].default_extgen_enabled  = TRUE;
    extension_extra_info_tab[i].extgen_enabled          = TRUE;
    extension_extra_info_tab[i].equiv_type_enabled      = TRUE;
    extension_extra_info_tab[i].extoption_flags         = 0;
    extension_extra_info_tab[i].default_extoption_flags = 0;
  }
  return true;
}

/*
 * Initialize PU specific options
 */
void Initialize_Extension_Loader_PU(WN *pu) {
  int ext;

  /*
   * Enable/disable extension type equivalence and native codegen support on a function basis,
   * based on both Enable_Extension_Native_Support value and pragma value
   * Note: currently type equivalence and native support are set accordingly but
   *       this might evolve in future development.
   */
  for (ext=0; ext < extension_count; ext++) {
    extension_extra_info_tab[ext].extgen_enabled = extension_extra_info_tab[ext].default_extgen_enabled;
    extension_extra_info_tab[ext].equiv_type_enabled = extension_extra_info_tab[ext].default_extgen_enabled;
    extension_extra_info_tab[ext].extoption_flags = extension_extra_info_tab[ext].default_extoption_flags;
  }
  if (WN_operator(pu) == OPR_FUNC_ENTRY && WN_func_pragmas(pu)) {
    WN *wn;
    for (wn = WN_first(WN_func_pragmas(pu)); wn; wn = WN_next(wn)) {
      if ((WN_opcode(wn) == OPC_PRAGMA) || (WN_opcode(wn) == OPC_XPRAGMA)) {

        WN_PRAGMA_ID pid = (WN_PRAGMA_ID)WN_pragma(wn);
        if (pid == WN_PRAGMA_DISABLE_EXTGEN || pid == WN_PRAGMA_FORCE_EXTGEN) {
          BOOL enable = (pid == WN_PRAGMA_FORCE_EXTGEN)?TRUE:FALSE;

          if (WN_pragma_arg1(wn) != 0) {
            // Extension codegen disabled for a single extension
            INT32 extid = WN_pragma_arg1(wn);
            int extrank = EXTENSION_Get_ExtensionRank_From_ExtensionId(extid);
            if (extrank == INVALID_EXTENSION_RANK) {
              DevWarn("WARNING: unknown extension id '%d' specified in pragma", extid);
            }
            else {
              extension_extra_info_tab[extrank].extgen_enabled = enable;
              extension_extra_info_tab[extrank].equiv_type_enabled = enable;
            }
          }
          else {
            for (ext=0; ext<extension_count; ext++) {
              extension_extra_info_tab[ext].extgen_enabled = enable;
              extension_extra_info_tab[ext].equiv_type_enabled = enable;
            }
          }
        } else if (pid == WN_PRAGMA_FORCE_EXTENSION_OPTION ||
                   pid == WN_PRAGMA_DISABLE_EXTENSION_OPTION) {
          
          if (WN_pragma_arg1(wn) == 0) {
            DevWarn("WARNING: unknown no extension id specified in pragma extension_option");
          } else {
            // Extension codegen disabled for a single extension
            INT32 extid = WN_pragma_arg1(wn);
            int extrank = EXTENSION_Get_ExtensionRank_From_ExtensionId(extid);
            if (extrank == INVALID_EXTENSION_RANK) {
              DevWarn("WARNING: unknown extension id '%d' specified in pragma", extid);
            } else {
              if (pid == WN_PRAGMA_FORCE_EXTENSION_OPTION) {
                extension_extra_info_tab[extrank].extoption_flags|=WN_pragma_arg2(wn);
              } else { // pid == WN_PRAGMA_DISABLE_EXTENSION_OPTION
                extension_extra_info_tab[extrank].extoption_flags&=~WN_pragma_arg2(wn);
              }
            }
          }
        }
      }
    }
  }
}

static std::map<std::string, long long> optflags_map;

long long
EXTENSION_Get_ExtOption_Flag_By_Name(int extid, const char* optionname) {
  if (strcmp(optionname, "noextlibs")==0) {
    return EXTOPT_noextlibs;
  }
  int extrank = EXTENSION_Get_ExtensionRank_From_ExtensionId(extid);

  long long flag = extension_tab[extrank].hooks->get_extoption_flag_by_name(optionname);
  return flag;
}


/*
 * Extension specific Initialization
 */
void Initialize_Extension_Support() {
  // Determine list of extensions with disabled native support.
  // The list is retrieved from command line option.
  if (Disabled_Native_Extensions_Set) {
    if (Disabled_Native_Extensions && Disabled_Native_Extensions[0] != 0) {
      char *extname_list = TYPE_MEM_POOL_ALLOC_N(char, Malloc_Mem_Pool, strlen(Disabled_Native_Extensions)+1);
      strcpy(extname_list, Disabled_Native_Extensions);
      char *extname = strtok(extname_list, ",");
      do {
	INT32 extid = EXTENSION_Get_ExtensionId_From_ExtensionName(extname);
	FmtAssert((extid!=INVALID_EXTENSION_ID), ("Cannot compute extension id from extension name '%s'", extname));
	int rank = EXTENSION_Get_ExtensionRank_From_ExtensionId(extid);
	if (rank == INVALID_EXTENSION_RANK) {
	  DevWarn("WARNING: unknown extension '%s' specified in pragma", extname);
	}
	else {
	  extension_extra_info_tab[rank].default_extgen_enabled = FALSE;
	}
	extname = strtok(NULL, ",");
      } while (extname);
    }
    else {
      // Option present but empty list of extension:
      // So by default, disable extgen for all extensions
      for (int i=0; i<extension_count; i++) {
	  extension_extra_info_tab[i].default_extgen_enabled = FALSE;
      }
    }
  }

  if ( Ext_Options_Set ) {
    if (Ext_Options && Ext_Options[0]!=0) {
      char *extoptions_list =
        TYPE_MEM_POOL_ALLOC_N(char, Malloc_Mem_Pool, strlen(Ext_Options)+1);
      
      strcpy(extoptions_list, Ext_Options);
      char *optname = strtok(extoptions_list, ",");
      do {
        // optname contains something like MP1x#enablefractgen
        char* ext = optname;
        char* tmp = strstr(ext,"#");
        if (tmp==NULL) {
          int i;
          long long mask;
          sscanf(optname, "%lld", &mask);
          for (i=0; i<extension_count; i++) {
            extension_extra_info_tab[i].default_extoption_flags|= mask;
          }
        } else {
          *tmp=0;
          char* name= tmp+1;
          
          INT32 extid = EXTENSION_Get_ExtensionId_From_ExtensionName(ext);
          FmtAssert((extid!=INVALID_EXTENSION_ID),
                    ("Cannot compute extension id from extension name '%s'",
                     ext));
          int rank = EXTENSION_Get_ExtensionRank_From_ExtensionId(extid);
          
          int flag = EXTENSION_Get_ExtOption_Flag_By_Name(extid, name);
          extension_extra_info_tab[rank].default_extoption_flags|= flag;
        }

        optname= strtok(NULL, ",");
      } while (optname);

    } else {
      DevWarn("WARNING: No extension specified. option '%s' ignored\n",
              Ext_Options);
    }
  }

}

#if defined(BACK_END) || defined(IR_TOOLS)

/*
 * Main function responsible for loading the extension library and
 * initializing dynamic mtypes and intrinsics
 */
void
Initialize_Extension_Loader ()
{
  int i;
  int nb_ext_mtypes;
  int nb_ext_intrinsics;

  CGTARG_InitializeMetaInstructionMap();

  for (i=0; i<=MTYPE_MAX_LIMIT; i++) {
    equiv_type_tab[i].ctype          = MTYPE_UNKNOWN;
    equiv_type_tab[i].intrn_to_c     = INTRINSIC_INVALID;
    equiv_type_tab[i].intrn_to_ext   = INTRINSIC_INVALID;
    equiv_type_tab[i].clr_intrn      = INTRINSIC_INVALID;
    equiv_type_tab[i].from_u32_intrn = INTRINSIC_INVALID;
  }

  if (Extension_Is_Present) {
 
    BOOL verbose = Get_Trace(TP_EXTENSION, TRACE_EXTENSION_LOADER_MASK);
    
    Load_Extension_dlls(verbose);

    // Compute total count for mtypes and intrinsics
    nb_ext_mtypes     = 0;
    nb_ext_intrinsics = 0;
    for (i=0; i<extension_count; i++) {
      nb_ext_mtypes     += extension_tab[i].hooks->get_modes_count();
      nb_ext_intrinsics += extension_tab[i].hooks->get_builtins_count();
    }

    Init_Mtypes(nb_ext_mtypes);
    Init_Intrinsics(nb_ext_intrinsics);

    for (i=0; i<extension_count; i++) {
      Add_MTypes(&extension_tab[i], NULL, NULL, verbose);
      Add_Intrinsics(&extension_tab[i], verbose);

      // add recog patterns
      Add_RecRules(&extension_tab[i], verbose);
    }
    //TB: Add a new pass to create new MTYPE for mutiple result intrinsics
    Add_Composed_Mtype();

    /* (Partially) Disable extension native support when:
     * - Compiling at optimization level < 2
     * - No opcode->intrinsic correspondence found (useless walk)
     */
    if (Opt_Level < 2 && !Enable_Extension_Native_Support_Set) {
      Enable_Extension_Native_Support&= ~(EXTENSION_NATIVE_CODEGEN |
					  EXTENSION_NATIVE_CVTGEN);
    }
    else if (Intrinsic_from_OPCODE.empty()) {
      Enable_Extension_Native_Support&= ~EXTENSION_NATIVE_CODEGEN;
    }

    Initialize_Extension_Support();
  }
  else {  // !Extension_Is_Present
    Init_Mtypes(0);
    Init_Intrinsics(0);
  }
}

/*
 * This function is responsible for setting PREG info. To be called by
 * the inliner or ir_b2a (neither the front end nor the back end).
 * Also called by ipl (IPA first pass - summary phase) and ipa_link.
 */
void
Initialize_Extension_Loader_Register () {
  int i;
  int nb_ext_mtypes;
  int nb_ext_intrinsics;

  if (Extension_Is_Present) {
    for (i=0; i < extension_count; i++) {
      int j;
      EXTENSION_ISA_Info* isa_ext_access;
      const ISA_REGISTER_CLASS_INFO* rc_tab;
      isa_ext_access = Generate_EXTENSION_ISA_Info(extension_tab[i].handler, 0);
      rc_tab = isa_ext_access->get_ISA_REGISTER_CLASS_tab();
      const mUINT32 rc_tab_size = isa_ext_access->get_ISA_REGISTER_CLASS_tab_sz();
      for (j=0; j < rc_tab_size; j++) {
	int first_isa_reg  = rc_tab[j].min_regnum;
	int last_isa_reg   = rc_tab[j].max_regnum;
	int register_count = last_isa_reg - first_isa_reg + 1;
	// Set the PREG max bound
	Set_Last_Dedicated_Preg_Offset( Get_Last_Dedicated_Preg_Offset_Func() + register_count);
      }
      delete isa_ext_access;
    }
  }
}
#endif
#ifdef BACK_END
extern "C" {
  void TI_Initialize_Extension_Loader() {
    if (Extension_Is_Present) {
      Lai_Initialize_Extension_Loader (extension_count, extension_tab);
    }
  }
}
#endif

void Cleanup_Extension_Loader() {
  int i;
  for (i=0; i<extension_count; i++) {
    delete extension_tab[i].hooks;
  }
  free(extension_tab);
}

/*
 *TB map between gcc machine mode preg and open64 MTYPE
 */
TYPE_ID MachineMode_To_Mtype(machine_mode_t mode) {
  if (mode >= STATIC_COUNT_MACHINE_MODE) {
    return (Extension_MachineMode_To_Mtype_Array[mode - STATIC_COUNT_MACHINE_MODE]);
  }
  /* Modes other than those standard ones should never be seen, 
     the current callers to this function are:
     - the loader when creating new dynamic types.
     - the intrinsics arguments results mapping for extension/multi result
     intrinsics. For the intrinsics we restrict the types to the simple ones below.
     Note that unsigned types and bool type are not present, 
     which is not a problem for extension intrinsics but may be necessary
     for standard intrinsics if the two flow are merged.
  */
  switch (mode) {
  case VOIDmode:
    return MTYPE_V;
  case QImode:
    return MTYPE_I1;
  case HImode:
    return MTYPE_I2;
  case PSImode:
  case SImode:
    return MTYPE_I4;
  case DImode:
    return MTYPE_I8;
  case SFmode:
    return MTYPE_F4;
  case DFmode:
    return MTYPE_F8;
  default:
    FmtAssert((0),("Unexpected mode to mtype conversion %d ", mode));
  }
  return MTYPE_UNKNOWN;
}

/* 
 * The following function is used by the IPA process during the
 * first pass. We need a read access to information enclosed
 * in dlls.
 */
void
Get_Loader_Extension_Table( Extension_dll_t **ext_tab, mUINT32 *ext_count )
{ 
    *ext_tab   = extension_tab;
    *ext_count = (mUINT32)extension_count;

     return;
}

machine_mode_t Mtype_To_MachineMode(TYPE_ID mtype) {
  if (mtype > MTYPE_STATIC_LAST) {
    int i;
    for (i = 0;
	 i < Extension_MachineMode_To_Mtype_Array_Size;
	 i++) {
      if (Extension_MachineMode_To_Mtype_Array[i] == mtype) {
	return (STATIC_COUNT_MACHINE_MODE + i);
      }
    }
  }
  FmtAssert(0, ("Mtype_To_MachineMode() does not support standard mtype yet"));
  return 0;
}

/*
 * Return the rank of the extension that contains mtype <ty>.
 * Return INVALID_EXTENSION_RANK if not found.
 */
int EXTENSION_Get_ExtensionRank_From_Mtype(TYPE_ID ty) {
  if (MTYPE_is_dynamic(ty)) {
    int ext=0;
    while (ext < extension_count-1 && ty >= extension_tab[ext+1].base_mtypes) {
      ext++;
    }
    return (ext);
  }
  return (INVALID_EXTENSION_RANK);
}

/*
 * Return the rank of the extension that contains intrinsic <intrn>.
 * Return INVALID_EXTENSION_RANK if not found.
 */
int EXTENSION_Get_ExtensionRank_From_Intrinsic(INTRINSIC intrn) {
  int ext=0;
  if (EXTENSION_Is_Extension_INTRINSIC(intrn)) {
    while (ext < extension_count-1 && intrn >= extension_tab[ext+1].base_intrinsics) {
      ext++;
    }
    return (ext);
  }
  return (INVALID_EXTENSION_RANK);
}

/*
 * Return TRUE if native codegen is enabled for at least one extension
 */
BOOL EXTENSION_Has_ExtGen_Enabled() {
  int ext;
  for (ext=0; ext<extension_count; ext++) {
    if (extension_extra_info_tab[ext].extgen_enabled) {
      return TRUE;
    }
  }
  return FALSE;
}

/*
 * Return TRUE if native codegen is enabled for the extension identified by <extid>
 */
BOOL EXTENSION_Is_ExtGen_Enabled(INT32 extid) {
  return (extension_extra_info_tab[EXTENSION_Get_ExtensionRank_From_ExtensionId(extid)].extgen_enabled);
}

/*
 * Return TRUE if native codegen is enabled for the extension that defines intrinsic <intrn>
 */
BOOL EXTENSION_Is_ExtGen_Enabled_For_Intrinsic(INTRINSIC intrn,
                                               long long flags) {

  INT32 extrank = EXTENSION_Get_ExtensionRank_From_Intrinsic(intrn);
  /* Get current extension flags mask */
  INT64 extflags = extension_extra_info_tab[extrank].extoption_flags;
  
  return (extension_extra_info_tab[extrank].extgen_enabled &
          ((flags&extflags)==flags));
}

/*
 * Return TRUE if type equivalence is enabled for the extension that defines mtype <ty>
 */
static BOOL EXTENSION_Is_TypeEquiv_Enabled_For_Mtype(TYPE_ID ty) {
  return (extension_extra_info_tab[EXTENSION_Get_ExtensionRank_From_Mtype(ty)].equiv_type_enabled);
}

/*
 * Return the INTRINSIC that map a conversion from <src_ty> to <tgt_ty>.
 * At least one of the type is expected to be an extension one.
 */
INTRINSIC EXTENSION_Get_Convert_Intrinsic(TYPE_ID src_ty, TYPE_ID tgt_ty, BOOL ignore_sign) {
  INTRINSIC id = INTRINSIC_INVALID;
  if (MTYPE_is_dynamic(src_ty) && EXTENSION_Is_TypeEquiv_Enabled_For_Mtype(src_ty)) {
    if (equiv_type_tab[src_ty].ctype == tgt_ty
	|| (ignore_sign
	    && MTYPE_complement(equiv_type_tab[src_ty].ctype) == tgt_ty)) {
      id = equiv_type_tab[src_ty].intrn_to_c;
    }
  }
  else if (MTYPE_is_dynamic(tgt_ty) && EXTENSION_Is_TypeEquiv_Enabled_For_Mtype(tgt_ty)) {
    if (equiv_type_tab[tgt_ty].ctype == src_ty
	|| (ignore_sign
	    && MTYPE_complement(equiv_type_tab[tgt_ty].ctype) == src_ty)) {
      id = equiv_type_tab[tgt_ty].intrn_to_ext;
    }
  }
  return id;
}

/*
 * Return TRUE if conversion from src to tgt is allowed, FALSE otherwise.
 * At least one of the type is expected to be an extension one.
 */
BOOL EXTENSION_Are_Equivalent_Mtype(TYPE_ID src_ty, TYPE_ID tgt_ty) {
  BOOL equiv = FALSE;
  /* identical mtypes are equivalent */
  if (src_ty == tgt_ty) {
    return TRUE;
  }
  BOOL ignore_sign = ! (Enable_Extension_Native_Support & EXTENSION_NATIVE_TYEQUIV_UNSIGNED_ONLY);
  equiv = (EXTENSION_Get_Convert_Intrinsic(src_ty, tgt_ty, ignore_sign) != INTRINSIC_INVALID);
  return equiv;
}

/*
 * Return the standard mtype associated to extension type <ext_ty>, if any.
 * Return MTYPE_UNKNOWN otherwise.
 */
TYPE_ID EXTENSION_Get_Equivalent_Mtype(TYPE_ID ext_ty) {
  TYPE_ID ty = MTYPE_UNKNOWN;
  if (MTYPE_is_dynamic(ext_ty) && EXTENSION_Is_TypeEquiv_Enabled_For_Mtype(ext_ty)) {
    ty = equiv_type_tab[ext_ty].ctype;
  }
  return ty;
}


/** 
 * return clr intrinsic for mtype, if defined.
 * 
 * @param ty type of the destination
 * 
 * @return 
 */
INTRINSIC EXTENSION_Get_CLR_Intrinsic(TYPE_ID ty) {
  if (MTYPE_is_dynamic(ty) && EXTENSION_Is_TypeEquiv_Enabled_For_Mtype(ty)) {
    return equiv_type_tab[ty].clr_intrn;
  }
  return INTRINSIC_INVALID;
}

/**
 * return intrinsic used to initialize an extension type variable
 * with a 32bits unsigned data. return INTRINSIC_INVALID if not
 * defined.
 * 
 * @param ty type of the destination
 *
 * @return
 */
INTRINSIC EXTENSION_Get_Convert_From_U32_Intrinsic(TYPE_ID ty) {
  if (MTYPE_is_dynamic(ty) && EXTENSION_Is_TypeEquiv_Enabled_For_Mtype(ty)) {
    return equiv_type_tab[ty].from_u32_intrn;
  }
  return INTRINSIC_INVALID;
}



