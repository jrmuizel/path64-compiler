/* 

Copyright (C) 2007 ST Microelectronics, Inc.  All Rights Reserved. 

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

/* AIR stands for Assembly intermediate representation */
/* This file is the central source file for AIR        */

#ifndef __AIR_C__
#define __AIR_C__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef DYNAMIC_CODE_GEN
#include "dyn_topcode.h"       /* Dynamic TOP for extensions */
#else
#include "topcode.h"           /* Definition of TOP          */
#include "targ_isa_operands.h"
#endif

#include "air.h"
#include "air_hash.h"
#include "air_utils.h"

/* AIR is strictly in C */
#ifdef __cplusplus
extern "C" {
#endif

  /* Declaration of internal structures
   */

  typedef struct{
    char          *name;
    AIR_Memory_Pool   *pool_symbol;
    unsigned int   nb_symbols_by_block;     /* always a power of 2 */
    unsigned int   log_nb_symbols_by_block;
    AIR_Dynamic_Table *memblock_tab;
    unsigned int   count_symbol;

    AIR_Hashtab   *hash;

  } AIR_Symtab;

  /* Declaration of static and global variables.
     All these variables are (re)initialized in AIR_Init() routine.
  */
  static unsigned int count_op = 0;
  static unsigned int count_section= 0;

  static AIR_Memory_Pool *pool_string = NULL;
  static AIR_Memory_Pool *pool_inst   = NULL;
  static AIR_Memory_Pool *pool_section= NULL;
  static AIR_Memory_Pool *pool_hash   = NULL;

  static AIR_Symtab *symtab = NULL;


  /* Declaration of internal routines.
   */
  static void  init_symbol_table(char *,unsigned int);
  static void  free_symbol_table(void);
  static void  AIR_symbol_hash_dump(FILE*, void *);
  static int   AIR_symbol_hash_compare(void *, void *);
  static void* AIR_symbol_hash_alloc_mem(int);
  static int   AIR_symbol_hash_fct(void*);
  static unsigned int get_log_in_basis_2(unsigned int);

  /* Beginning of code .......................................................*/

  /* Auxiliary function. Works when nb is a power of 2 */
  static unsigned int 
  get_log_in_basis_2( unsigned int nb )
  {
    unsigned int log = 0;

    if(nb==0)
      return log;
    while(nb!=1U)
      { nb>>=1U;
	++log;
      }

    return log;
  }

  /* Initialization of symbols table. Must be called after
     initialization of string pool.
  */
  static void
  init_symbol_table( char *name, unsigned int nb_sym_by_blocks )
  {
    name = AIR_CHECK_NULL_STRING(name);

    /* number of symbols by block must be a power of 2 */
    if(0==nb_sym_by_blocks || (nb_sym_by_blocks & (nb_sym_by_blocks-1))) {
      AIR_fatal_error("Internal","IE000","data must be a power of 2\n");
    }

    symtab = (AIR_Symtab*)AIR_xmalloc(sizeof(AIR_Symtab));
    symtab->name = (char*) AIR_xmalloc(strlen(name)+1); 
    strcpy(symtab->name,name);

    /* 
       The following dynamic table is just used
       as an additional indirection in order to garantee
       constant time for research of a symbol whose identifier is
       known.

       |-----|         |---------------------|     -------------
       |     |-------->| block of nb symbols |-----| next block...
       |-----|         |---------------------|     ----^--------
       |     |-----------------------------------------|
       |-----|
       dynamic table

    */
    symtab->nb_symbols_by_block     = nb_sym_by_blocks;
    symtab->log_nb_symbols_by_block = get_log_in_basis_2(nb_sym_by_blocks);
    symtab->pool_symbol  = AIR_memory_pool_init("symbols",
						symtab->nb_symbols_by_block*sizeof(AIR_Symbol));
    symtab->count_symbol = 0;

    symtab->memblock_tab= AIR_init_dynamic_table(
						 "symbols",
						 64,
						 sizeof(AIR_Memblock*));

    /*
      Initialization of an hash table. This hash table is used in
      order to get a symbol whose name and flags are given.
    */
    symtab->hash = build_new_hash_table(
					"symbol hash table",
					AIR_symbol_hash_compare,
					AIR_symbol_hash_fct,
					AIR_symbol_hash_dump,
					NULL,
					AIR_symbol_hash_alloc_mem,
					sizeof(AIR_Symbol),
					hash_get_nb_buckets()     /* number of buckets for the hashing routine
								     enclosed in air_hash.c. See usage of 
								     'modulo_reduction' in AIR_symbol_hash_fct
								  */
					);
  
    return;
  }

  static void
  free_symbol_table( void )
  {
    AIR_xfree((char*)symtab->name);
    AIR_memory_pool_free(symtab->pool_symbol);
    AIR_free_dynamic_table(symtab->memblock_tab);
    free_hash_table(symtab->hash);

    AIR_xfree((char*)symtab);
    symtab = NULL;
   
    return;
  }

  /* Find a symbol whose identifier is known
     Return NULL if symbol has not been found
     or if symbol table has not been initialized
  */
  AIR_Symbol*
  AIR_symbol_by_id( mUINT32 id )
  {
    mUINT32        index_block;
    mUINT32        modulo;
    AIR_Memblock **cur;

    if(NULL==symtab || id>=symtab->count_symbol)
      return NULL;

    /* Division by a power of 2*/
    index_block = id >> symtab->log_nb_symbols_by_block;
    cur = (AIR_Memblock**)AIR_get_item_in_dynamic_table(symtab->memblock_tab, 
							index_block);
    /* Modulo for a power of 2*/
    modulo = id & (symtab->nb_symbols_by_block -1U);
    return (AIR_Symbol*)((*cur)->buf + modulo * sizeof(AIR_Symbol));
  }

  /* AIR symbol comparison routine. This routine is used by
     symbol hash table in order to decide whether a new
     symbol is to be inserted or not in the symbol table.
     Symbols a and b are not totally equivalent: symbol a
     is supposed to be the reference symbol.
  */
  static int
  AIR_symbol_hash_compare( void *a_, void *b_ )
  {
    AIR_Symbol *a = (AIR_Symbol*)a_;
    AIR_Symbol *b = (AIR_Symbol*)b_;
    AIR_String a_name, b_name;
    const int cmp_ok = 1;
    const int cmp_ko = 0;
    int tmp;
    
    a_name = Get_AIR_symbol_name(a);
    b_name = Get_AIR_symbol_name(b);

    /* In what follows, we handle the case where
       at least one of the string is NULL */
    tmp = 0;
    if(NULL==a_name)
      ++tmp;
    if(NULL==b_name)
      ++tmp;
    if(2==tmp)
      return cmp_ok;
    if(1==tmp)
      return cmp_ko;

    /* If 2 symbols have the same string, we should normally
       consider them as being identical. However, we have to
       take care of flags and related properties.
    */
    if(!strcmp(a_name,b_name))
      { mUINT32 a_flags, b_flags;

	a_flags = Get_AIR_symbol_flags(a);
	b_flags = Get_AIR_symbol_flags(b);

	/* Case 1 : both flags are equal */
	if(a_flags == b_flags)
	  return cmp_ok;
	/* Case 2 : no flags for b. a is a "normal symbol" */
	else if ((b_flags == AIR_SYMBOL_NO_FLAGS))
	  { if(!(a_flags & AIR_SYMBOL_SECTION) &&
	       !(a_flags & AIR_SYMBOL_OBJECT)  &&
	       !(a_flags & AIR_SYMBOL_FILE))
	      return cmp_ok;
	  }
      }
    return cmp_ko;
  }

  static int
  AIR_symbol_hash_fct( void *a_ )
  {
    AIR_Symbol *a = (AIR_Symbol*)a_;
    AIR_String  a_string = Get_AIR_symbol_name(a);
    return (int) hash_modulo_reduction((unsigned char*) a_string);
  }

  static void
  AIR_symbol_hash_dump(FILE *f, void *a_)
  {
    AIR_dump_symbol(f,(AIR_Symbol*)a_);
    return;
  }

  static void*
  AIR_symbol_hash_alloc_mem( int size )
  {
    return (void*)AIR_memory_pool_alloc(pool_hash,(mUINT32)size);
  }

#if 0
  /* We don't free any memory since memory has been allocated by pool.
     When pool gets free, all memory will be released.
  */
  static void
  AIR_symbol_hash_free_fct( void* in )
  {
    return;
  }
#endif

  /* Allocate space for a new empty string */
  AIR_String
  AIR_new_string( mUINT32 len )
  {
    AIR_String a;

    ++len;
    a = (AIR_String)AIR_memory_pool_alloc(pool_string,len);

    a[len-1]='\0';         /* protection */
    a[0]  ='\0';

    return a;
  }

  /* String duplication - memory is allocated in 
     string memory pool
  */
  AIR_String
  AIR_duplicate_string( AIR_String a )
  {
    AIR_String b;

    if(NULL==a)
      { b = a;
      }
    else
      { b = AIR_new_string(strlen(a)+1);
	strcpy(b,a);
      }
    return b;
  }

  /* Create a new section */
  AIR_Section*
  AIR_new_section( AIR_String a, AIR_Section_Attributes attr )
  {
    AIR_Section *sect;
    AIR_Symbol  *sym;
    mUINT32      sectid;

    if(NULL==a)
      return NULL;

    sect = (AIR_Section*)AIR_memory_pool_alloc(pool_section,
					       sizeof(AIR_Section));
    Set_AIR_section_size(sect,0);
    Set_AIR_section_attributes(sect,attr);

    /* new symbol in the section table */
    sym = AIR_new_symbol(a,AIR_SYMBOL_SECTION | AIR_SYMBOL_GLOBAL);
    sectid = Get_AIR_symbol_id(sym);
    Set_AIR_section_id(sect,sectid);
    
    count_section++;
    return sect;
  }

  AIR_String
  Get_AIR_section_name( AIR_Section *a )
  {
    AIR_String ret = NULL;
    AIR_Symbol *sym;

    if(NULL==a)
      return ret;

    sym = AIR_symbol_by_id(Get_AIR_symbol_id(a));
    if(sym)
      ret = Get_AIR_symbol_name(sym);

    return ret;
  }

  /* Create a new symbol */
  AIR_Symbol*
  AIR_new_symbol( AIR_String a, mUINT32 flags )
  {
    AIR_Symbol     *b ;
    AIR_String      a_copy;
    AIR_Memblock  **ref;
    AIR_Memblock   *prev;

    if(NULL==a)
      return NULL;

    b = AIR_find_symbol_by_string(a,flags);
    if(b)
      return b;

    /* Allocate memory for a new symbol */
    b = (AIR_Symbol*)AIR_memory_pool_alloc(symtab->pool_symbol,sizeof(AIR_Symbol));
    a_copy = AIR_duplicate_string(a);

    /* Fill in the various fields */
    Set_AIR_symbol_name(b,a_copy);
    Set_AIR_symbol_size(b,0);
    Set_AIR_symbol_val(b,0ULL);
    Set_AIR_symbol_flags(b,flags);
    Set_AIR_symbol_alignment(b,0);
    Set_AIR_symbol_refcounter(b,0);
    Set_AIR_symbol_id(b,symtab->count_symbol);

    /* Now update dynamic table used for indirection.
       We compare the address of the previous block
       with the address of the current one. If they
       differ, we allocate one new item in the dynamic
       table.
    */
    if(0==symtab->count_symbol)
      prev=(AIR_Memblock*)NULL;
    else
      prev=*(AIR_Memblock**)AIR_get_item_in_dynamic_table(
							  symtab->memblock_tab,
							  (symtab->count_symbol-1U) >> symtab->log_nb_symbols_by_block);
    if(symtab->pool_symbol->cur!= prev)
      { ref=(AIR_Memblock**)AIR_get_new_item_in_dynamic_table(symtab->memblock_tab);
	*ref=symtab->pool_symbol->cur;
      }

    /* Update hash table associated to the symbol table.
       We force creation of a new symbol since existence
       of an equivalent symbol has already been tested.
    */
    insert_new_item_force(symtab->hash,(void*)b);


    /* Update symbol table counter.
     */
    symtab->count_symbol++;

    return b;
  }

  AIR_Symbol*
  AIR_find_symbol_by_string( AIR_String a, mUINT32 flags )
  {
    AIR_Symbol   *b;
    AIR_Symbol    b_;
    int           found;

    if(NULL==a)
      return NULL;

    b = &b_;
    Set_AIR_symbol_name(b,a);
    Set_AIR_symbol_flags(b,flags);
    Set_AIR_symbol_id(b,0);
    Set_AIR_symbol_alignment(b,0);
    Set_AIR_symbol_refcounter(b,0);

    return (AIR_Symbol*) research_item(symtab->hash,(void*)b,&found);
  }

  void
  AIR_dump_symbol( FILE *f, AIR_Symbol *a )
  {
    AIR_String string = Get_AIR_symbol_name(a);
    char flag_string[20];
    int  ptr;

    if(NULL==string || string[0]=='\0')
      string = "<undef>";

    ptr = 0;
    if(Is_AIR_symbol_local(a))
      flag_string[ptr++]='L';
    if(Is_AIR_symbol_global(a))
      flag_string[ptr++]='G';
    if(Is_AIR_symbol_weak(a)) 
      flag_string[ptr++]='W';
    if(Is_AIR_symbol_section(a))
      flag_string[ptr++]='S';
    if(Is_AIR_symbol_file(a))
      flag_string[ptr++]='F';
    if(Is_AIR_symbol_object(a))
      flag_string[ptr++]='O';

    if(ptr)
      flag_string[ptr]='\0';            /* Put NULL terminating character */
    else
      strcpy(flag_string,"<noflags>");
 
    (void)fprintf(
		  f,"sym %s (size:%d, val:%lld, flags:%s, align:%d, id:%d)\n",
		  string,
		  Get_AIR_symbol_size(a),
		  Get_AIR_symbol_val(a),
		  (char*)flag_string,
		  Get_AIR_symbol_alignment(a),
		  Get_AIR_symbol_id(a));

    return;
  }

  void 
  AIR_dump_symbol_table( FILE *f )
  {
    unsigned int i;

    if(NULL==symtab)
      (void)fprintf(f,"No symbol table yet\n");
    else if(0==symtab->count_symbol)
      (void)fprintf(f,"Symbol table is empty\n");
    else
      { (void)fprintf(f,
		      "\"%s\" symbol table -- %d symbols currently stored\n\n",
		      symtab->name,symtab->count_symbol);
	for(i=0;i<symtab->count_symbol;i++)
	  AIR_dump_symbol(f, AIR_symbol_by_id(i));
	(void)fprintf(f,"\nEnd symbol table\n\n");
      }

    return;
  }

  /* Create a new register for an op register */
  AIR_TN*
  AIR_new_tn_reg( ISA_REGISTER_CLASS regclass, 
		  ISA_REGISTER_SUBCLASS subclass,
		  mUINT32 regnum )
  {
    AIR_TN *tn;

    tn = (AIR_TN*)AIR_memory_pool_alloc(pool_inst,sizeof(AIR_TN));

    Set_AIR_TN_kind(tn,AIR_TN_register);
    Set_AIR_TN_string(tn,NULL);
    Reset_AIR_TN_flags(tn);
    Set_AIR_TN_reg_regclass(tn,regclass);
    Set_AIR_TN_reg_subclass(tn,subclass);
    Set_AIR_TN_reg_regnum(tn,regnum);

    return tn;
  }

  /* Create a new immediate for an op tn */
  AIR_TN*
  AIR_new_tn_imm( mUINT64 immval ) 
  {
    AIR_TN  *tn;

    tn = (AIR_TN*)AIR_memory_pool_alloc(pool_inst,sizeof(AIR_TN));

    Set_AIR_TN_kind(tn,AIR_TN_expression);
    Set_AIR_TN_string(tn,NULL);
    Reset_AIR_TN_flags(tn);
    Set_AIR_TN_exp_kind(tn,AIR_Expression_immediate);
    Set_AIR_TN_exp_imm_val(tn,immval);

    return tn;
  }

  AIR_TN*
  AIR_new_tn_reloc ( mUINT32 relocid, void *symid, void *symop, mUINT64 val, void *expr )
  {
    AIR_TN  *tn;

    tn = (AIR_TN*)AIR_memory_pool_alloc(pool_inst,sizeof(AIR_TN));

    Set_AIR_TN_kind(tn,AIR_TN_expression);
    Set_AIR_TN_string(tn,NULL);
    Reset_AIR_TN_flags(tn);
    Set_AIR_TN_exp_kind(tn,AIR_Expression_reloc);
    Set_AIR_TN_exp_reloc_relocid(tn,relocid);
    Set_AIR_TN_exp_reloc_symid(tn,symid);
    Set_AIR_TN_exp_reloc_symop(tn,symop);
    Set_AIR_TN_exp_reloc_val(tn,val);
    Set_AIR_TN_exp_reloc_expr(tn,expr);

    return tn;
  }

  /* Create a new enum TN */
  AIR_TN*
  AIR_new_tn_enum( ISA_ENUM_CLASS ec, 
		  ISA_ENUM_CLASS_VALUE ecv)
  {
    AIR_TN *tn;

    tn = (AIR_TN*)AIR_memory_pool_alloc(pool_inst,sizeof(AIR_TN));

    Set_AIR_TN_kind(tn,AIR_TN_expression);
    Set_AIR_TN_string(tn,NULL);
    Reset_AIR_TN_flags(tn);
    Set_AIR_TN_exp_kind(tn,AIR_Expression_enum);
    Set_AIR_TN_exp_enum_ec(tn,ec);
    Set_AIR_TN_exp_enum_ecv(tn,ecv);

    return tn;
  }

  AIR_TN*
  AIR_new_tn_regmask( ISA_REGISTER_CLASS class,
		      ISA_REGISTER_SUBCLASS subclass,
		      mUINT64 mask )
  {
    AIR_TN *tn;
    mUINT32      nb_registers;
    mUINT32      i;
     
    for(i=0,nb_registers=0;i<sizeof(mUINT64);++i)
      { if(mask&0x1)
          ++nb_registers;
        mask>>=1U;
      }

    tn = (AIR_TN*)AIR_memory_pool_alloc(pool_inst,sizeof(AIR_TN));

    Set_AIR_TN_kind(tn,AIR_TN_expression);
    Set_AIR_TN_string(tn,NULL);
    Reset_AIR_TN_flags(tn);
    Set_AIR_TN_exp_kind(tn,AIR_Expression_regmask);
    Set_AIR_TN_exp_regmask_regclass(tn,class);
    Set_AIR_TN_exp_regmask_subclass(tn,subclass);
    Set_AIR_TN_exp_regmask_mask(tn,mask);
    Set_AIR_TN_exp_regmask_nbregisters(tn,nb_registers);

    return tn;
  }

  /* Second constructor */
  AIR_TN*
  AIR_new_tn_regmask_2( ISA_REGISTER_CLASS class,
			ISA_REGISTER_SUBCLASS subclass,
			INT regmin, INT regmax )
  {
    INT tmp;
    mUINT32 i;
    mUINT64 mask;

    if(regmin<0 || regmin >=sizeof(mUINT64) || 
       regmax<0 || regmax >=sizeof(mUINT64)) {
      AIR_fatal_error("Internal","IE001","uncorrect parameters\n");
    }

    if(regmax<regmin)
      { tmp = regmax;
	regmax=regmin;
	regmin = tmp;
      }

    for(i=0,mask=0ULL;i<sizeof(mUINT64);++i)
      { if(i>=regmin && i<=regmax)
	  mask |= ((mUINT64)1<<(mUINT64)i);
      }

    return AIR_new_tn_regmask(class,subclass,mask);
  }

  AIR_TN*
  AIR_duplicate_tn( AIR_TN *tn )                         
  {
    if(NULL==tn)
      return (AIR_TN*)NULL;

    if(Is_AIR_TN_register(tn))
      { return AIR_new_tn_reg(
			      Get_AIR_TN_reg_regclass(tn),
			      Get_AIR_TN_reg_subclass(tn),
			      Get_AIR_TN_reg_regnum(tn));
      }

    if(Is_AIR_TN_expression(tn))
      { switch(Get_AIR_TN_exp_kind(tn))
	  { case AIR_Expression_immediate:
	      return AIR_new_tn_imm(Get_AIR_TN_exp_imm_val(tn));
	      break;

          case AIR_Expression_regmask:
            return AIR_new_tn_regmask(
				      Get_AIR_TN_exp_regmask_regclass(tn),
				      Get_AIR_TN_exp_regmask_subclass(tn),
				      Get_AIR_TN_exp_regmask_mask(tn));
            break;

          case AIR_Expression_reloc:
	    return AIR_new_tn_reloc(Get_AIR_TN_exp_reloc_relocid(tn),
				    Get_AIR_TN_exp_reloc_symid(tn),
				    Get_AIR_TN_exp_reloc_symop(tn),
				    Get_AIR_TN_exp_reloc_val(tn),
				    Get_AIR_TN_exp_reloc_expr(tn));
          default:
            AIR_fatal_error("Internal","IE002","unknown expression type\n");
            break;
	  }
      }

    if(Is_AIR_TN_buffer(tn))
      { /* TODO */
      }

    return (AIR_TN*)NULL;
  }

  /* Dumping an tn */
  void
  AIR_dump_tn( FILE *f, AIR_TN *tn )
  {
    ISA_REGISTER_CLASS                regclass;
    ISA_REGISTER_SUBCLASS             subclass;
    mUINT32                           regnum;
    const ISA_REGISTER_CLASS_INFO    *regclassinfo;
    const ISA_REGISTER_SUBCLASS_INFO *subclassinfo;
    mUINT64                           regmask;

    if(NULL==tn)
      { (void) fprintf(f,"<null> pointer for AIR_TN\n");
      }
    else if(Is_AIR_TN_register(tn))
      { regclass = Get_AIR_TN_reg_regclass(tn);
	subclass = Get_AIR_TN_reg_subclass(tn);
	regnum   = Get_AIR_TN_reg_regnum(tn);
	regclassinfo = ISA_REGISTER_CLASS_Info(regclass);
	subclassinfo = ISA_REGISTER_SUBCLASS_Info(subclass);

	(void)fprintf(f,"reg. class %s, reg. subclass %s, register %s (%d)\n",
		      ISA_REGISTER_CLASS_INFO_Name(regclassinfo),
		      ISA_REGISTER_SUBCLASS_INFO_Name(subclassinfo),
		      ISA_REGISTER_CLASS_INFO_Reg_Name(regclassinfo,regnum),
		      regnum);
      }
    else if(Is_AIR_TN_expression(tn))
      { switch(Get_AIR_TN_exp_kind(tn))
	  { case AIR_Expression_immediate:
	      (void)fprintf(f,"imm %lld\n", Get_AIR_TN_exp_imm_val(tn));
	      break;
          

          case AIR_Expression_reloc:
	    (void)fprintf(f,"reloc %d, symbolid %p, symbolop = %p, val = %lld, expr = %p\n",
			  Get_AIR_TN_exp_reloc_relocid(tn), Get_AIR_TN_exp_reloc_symid(tn),
			  Get_AIR_TN_exp_reloc_symop(tn), Get_AIR_TN_exp_reloc_val(tn),
			  Get_AIR_TN_exp_reloc_expr(tn));
	    break;

          case AIR_Expression_regmask:
	    regclass = Get_AIR_TN_exp_regmask_regclass(tn);
	    subclass = Get_AIR_TN_exp_regmask_subclass(tn);
	    regmask  = Get_AIR_TN_exp_regmask_mask(tn);

	    regclassinfo = ISA_REGISTER_CLASS_Info(regclass);
	    subclassinfo = ISA_REGISTER_SUBCLASS_Info(subclass);

	    (void)fprintf(f,
			  "reg. mask, reg. class %s, reg. subclass %s, mask 0x%lld\n",
			  ISA_REGISTER_CLASS_INFO_Name(regclassinfo),
			  ISA_REGISTER_SUBCLASS_INFO_Name(subclassinfo),
			  regmask);
   
	    break;

          case AIR_Expression_enum:
	    (void)fprintf(f,
			  "enum of ecv %s, ec %s\n",
			  ISA_ECV_Name(Get_AIR_TN_exp_enum_ecv(tn)),
			  ISA_EC_Name(Get_AIR_TN_exp_enum_ec(tn)));
	    break;

          default:
	    (void)fprintf(f,"tn - expression of unknown type\n");
	    exit(1);
	    break;
            
	  }
      }
    else if(Is_AIR_TN_buffer(tn))
      { (void)fprintf(f,"buf of len %d at address 0x%08x\n",
		      Get_AIR_TN_buffer_length(tn),
		      (unsigned int)Get_AIR_TN_buffer(tn));
      }
    else 
      { (void)fprintf(f,"tn of unknown type\n");
        exit(1);
      }
       
    return;
  }

  /* Create a new empty op */
  AIR_OP*
  AIR_new_op( void )
  {
    AIR_OP *op;
    mUINT32 instid = ++count_op;    /* 0 can't be a valid id */
    mUINT32 i;
 
    op=(AIR_OP*)AIR_memory_pool_alloc(pool_inst,sizeof(AIR_OP));
    memset(op,0,sizeof(AIR_OP));

    Set_AIR_kind_op(op);
    Set_AIR_OP_TOP(op,TOP_UNDEFINED);
    Set_AIR_OP_id(op,instid);
    Reset_AIR_OP_flags(op);
    Set_AIR_OP_nb_operands(op,0);
    Set_AIR_OP_nb_results(op,0);
    Set_AIR_OP_next(op,(AIR_OP*)NULL);
    Set_AIR_OP_prev(op,(AIR_OP*)NULL);

    for(i=0;i<ISA_OPERAND_max_operands;++i)
      { Set_AIR_OP_operandn(op,i,((AIR_TN*)NULL));
      }
    for(i=0;i<ISA_OPERAND_max_results;++i)
      { Set_AIR_OP_resultn(op,i,((AIR_TN*)NULL));
      }
   
    return op;
  }

  /* Create a new op and set the TOP */
  AIR_OP*
  AIR_new_op_with_TOP( TOP top )
  {
    AIR_OP *op = AIR_new_op();

    Set_AIR_OP_TOP(op,top);
    return op;
  }

  /* Build a "full" op: all results and operands have to
     be specified. 
     - first parameter: op TOP
     - next parameters: op results
     - next parameters: op operands
     This routine is very similar to the Mk_OP routine in the Open64 compiler
     backend (which explains why the CGIR flag is set).
  */
  AIR_OP*
  AIR_mk_full_op( TOP top, mUINT32 tn_nb, ...)
  {
    va_list ap;
    AIR_OP *op;
    AIR_TN *tn;
    mUINT32 nb_fixed_results = ISA_OPERAND_INFO_Results(ISA_OPERAND_Info(top));
    mUINT32 nb_fixed_opnds   = ISA_OPERAND_INFO_Operands(ISA_OPERAND_Info(top));
    mUINT32 i;

    op = AIR_new_op_with_TOP(top);

    if(tn_nb != nb_fixed_results + nb_fixed_opnds) {
      AIR_error("Internal","AIR_IE_000","Operand TN number %d differs from expected %d for %s\n",
		tn_nb,nb_fixed_results + nb_fixed_opnds,TOP_Name(top));
    }

    va_start(ap,tn_nb);

    for(i=0;i<nb_fixed_results;i++)
      { tn = va_arg(ap,AIR_TN*);
	Set_AIR_OP_resultn(op,i,tn);
      }

    for(i=0;i<nb_fixed_opnds;i++)
      { tn = va_arg(ap,AIR_TN*);
	Set_AIR_OP_operandn(op,i,tn);
      }

    va_end(ap);

    Set_AIR_OP_nb_operands(op,nb_fixed_opnds);
    Set_AIR_OP_nb_results(op,nb_fixed_results);

    Set_AIR_OP_flags(op,AIR_OP_FLAG_CGIR_MODE);

    return op;
  }

  /* AIR_OPS stuff */

  inline void prepend_ops(AIR_OPS *ops, AIR_OP *first, AIR_OP *last)
  {
    AIR_OP **pprev = AIR_OPS_first(ops) ? &AIR_OPS_first(ops)->prev : &ops->last;
    first->prev = NULL;
    last->next = AIR_OPS_first(ops);
    ops->first = first;
    *pprev = last;
  }


  inline void append_ops(AIR_OPS *ops, AIR_OP *first, AIR_OP *last)
  {
    AIR_OP **pnext = AIR_OPS_last(ops) ? &AIR_OPS_last(ops)->next : &ops->first;
    last->next = NULL;
    first->prev = AIR_OPS_last(ops);
    ops->last = last;
    *pnext = first;
  }

  inline void insert_ops_before(AIR_OPS *ops, AIR_OP *point, AIR_OP *first, AIR_OP *last)
  {
    AIR_OP **prevp = Get_AIR_OP_prev(point) ? &Get_AIR_OP_prev(point)->next : &ops->first;
    *prevp = first;
    last->next = point;
    first->prev = Get_AIR_OP_prev(point);
    point->prev = last;
  }

  inline void insert_ops_after(AIR_OPS *ops, AIR_OP *point, AIR_OP *first, AIR_OP *last)
  {
    AIR_OP **nextp = Get_AIR_OP_next(point) ? &Get_AIR_OP_next(point)->prev : &ops->last;
    *nextp = last;
    first->prev = point;
    last->next = Get_AIR_OP_next(point);
    point->next = first;
  }

  inline void insert_ops(AIR_OPS *ops, AIR_OP *point, AIR_OP *first, AIR_OP *last, BOOL before)
  {
    if (point == NULL) {
      if (before)
	prepend_ops(ops, first, last);
      else
	append_ops(ops, first, last);
    } else {
      if (before)
	insert_ops_before(ops, point, first, last);
      else
	insert_ops_after(ops, point, first, last);
    }
  }

  AIR_OPS *AIR_OPS_Create(void)
  {
    AIR_OPS *result = (AIR_OPS*)AIR_memory_pool_alloc(pool_inst,sizeof(AIR_OPS));
    result->first = result->last = NULL;
    result->length = 0;
    return result;
  }

  void AIR_OPS_Remove_All(AIR_OPS *ops)
  {
    ops->first = ops->last = NULL;
    ops->length = 0;
  }
  
  AIR_OPS *AIR_OPS_Init(AIR_OPS *ops)
  {
    ops->first = ops->last = NULL;
    ops->length = 0;
    return ops;
  }

  void AIR_OPS_Remove_Op(AIR_OPS *ops, AIR_OP *op)
  {
    AIR_OP **prevp = Get_AIR_OP_next(op) ? &Get_AIR_OP_next(op)->prev : &ops->last;
    AIR_OP **nextp = Get_AIR_OP_prev(op) ? &Get_AIR_OP_prev(op)->next : &ops->first;
  
    *prevp = Get_AIR_OP_prev(op);
    *nextp = Get_AIR_OP_next(op);

    op->prev = op->next = NULL;
    ops->length--;
  }

  void AIR_OPS_Remove_Ops(AIR_OPS *ops, AIR_OPS *remove_ops)
  {
    AIR_OP *first = AIR_OPS_first(remove_ops);
    AIR_OP *last = AIR_OPS_last(remove_ops);
    AIR_OP **prevp, **nextp;

    if (first == NULL) return;

    prevp = Get_AIR_OP_next(last) ? &Get_AIR_OP_next(last)->prev : &ops->last;
    nextp = Get_AIR_OP_prev(first) ? &Get_AIR_OP_prev(first)->next : &ops->first;
  
    *prevp = Get_AIR_OP_prev(first);
    *nextp = Get_AIR_OP_next(last);

    first->prev = last->next = NULL;
    ops->length -= AIR_OPS_length(remove_ops);
  }

  void AIR_OPS_Insert_Op(AIR_OPS *ops, AIR_OP *point, AIR_OP *op, BOOL before)
  {
    insert_ops(ops, point, op, op, before);
    ops->length++;
  }


  void AIR_OPS_Insert_Op_Before(AIR_OPS *ops, AIR_OP *point, AIR_OP *op)
  {
    insert_ops_before(ops, point, op, op);
    ops->length++;
  }


  void AIR_OPS_Insert_Op_After(AIR_OPS *ops, AIR_OP *point, AIR_OP *op)
  {
    insert_ops_after(ops, point, op, op);
    ops->length++;
  }


  void AIR_OPS_Append_Op(AIR_OPS *ops, AIR_OP *op)
  {
    append_ops(ops, op, op);
    ops->length++;
  }


  void AIR_OPS_Prepend_Op(AIR_OPS *ops, AIR_OP *op)
  {
    prepend_ops(ops, op, op);
    ops->length++;
  }


  void AIR_OPS_Insert_Ops(AIR_OPS *ops, AIR_OP *point, AIR_OPS *new_ops, BOOL before)
  {
    if (AIR_OPS_first(new_ops) == NULL) return;
    insert_ops(ops, point, AIR_OPS_first(new_ops), AIR_OPS_last(new_ops), before);
    ops->length += AIR_OPS_length(new_ops);
  }


  void AIR_OPS_Insert_Ops_Before(AIR_OPS *ops, AIR_OP *point, AIR_OPS *new_ops)
  {
    if (AIR_OPS_first(new_ops) == NULL) return;
    insert_ops_before(ops, point, AIR_OPS_first(new_ops), AIR_OPS_last(new_ops));
    ops->length += AIR_OPS_length(new_ops);
  }


  void AIR_OPS_Insert_Ops_After(AIR_OPS *ops, AIR_OP *point, AIR_OPS *new_ops)
  {
    if (AIR_OPS_first(new_ops) == NULL) return;
    insert_ops_after(ops, point, AIR_OPS_first(new_ops), AIR_OPS_last(new_ops));
    ops->length += AIR_OPS_length(new_ops);
  }


  void AIR_OPS_Append_Ops(AIR_OPS *ops, AIR_OPS *new_ops)
  {
    if (AIR_OPS_first(new_ops) == NULL) return;
    append_ops(ops, AIR_OPS_first(new_ops), AIR_OPS_last(new_ops));
    ops->length += AIR_OPS_length(new_ops);
  }


  void AIR_OPS_Prepend_Ops(AIR_OPS *ops, AIR_OPS *new_ops)
  {
    if (AIR_OPS_first(new_ops) == NULL) return;
    prepend_ops(ops, AIR_OPS_first(new_ops), AIR_OPS_last(new_ops));
    ops->length += AIR_OPS_length(new_ops);
  }

  /* Checks that an tn is conform to its targinfo description.
     Open64 routine Verify_Tn (in cgemit.cxx) performs the
     same kind of check on backend opcodes.
     Returns an error for a NULL tn.
  */
  INT
  AIR_check_tn( FILE *f, AIR_TN* tn, TOP top, INT opid,
		INT is_result, INT verbose )
  {
    const ISA_OPERAND_INFO   *info;
    const ISA_OPERAND_VALTYP *valtyp;
    typedef enum {
      is_ok = 0,
      not_a_register,
      wrong_regclass,
      wrong_regsubclass,
      not_a_litteral,
      not_in_range,
    } Report_Error;

    Report_Error error = is_ok;

    if(NULL==tn)
      goto error_case;

    info = ISA_OPERAND_Info(top);
    if(is_result)
      valtyp = ISA_OPERAND_INFO_Result(info,opid);
    else
      valtyp = ISA_OPERAND_INFO_Operand(info,opid);

    if(ISA_OPERAND_VALTYP_Is_Register(valtyp))
      { ISA_REGISTER_CLASS rc_targ;
	ISA_REGISTER_CLASS rc;
	ISA_REGISTER_SUBCLASS subclass_targ;
	ISA_REGISTER_SUBCLASS subclass;

	if (!Is_AIR_TN_register(tn))
	  { error = not_a_register;
	    goto error_case;
	  }

	rc_targ = ISA_OPERAND_VALTYP_Register_Class(valtyp);
	subclass_targ = ISA_OPERAND_VALTYP_Register_Subclass(valtyp);
	rc       = Get_AIR_TN_reg_regclass(tn); 
	subclass = Get_AIR_TN_reg_subclass(tn);

	/* Check that register class is OK
	 */
	if(rc!=rc_targ)
	  { error = wrong_regclass;
	    goto error_case; 
	  }

	/* Check that register subclass is OK
	 */
	if(subclass!=subclass_targ)
	  { error = wrong_regsubclass;
	    goto error_case;
	  }

	/* we could also check that register number is OK */
      }
    else if (ISA_OPERAND_VALTYP_Is_Literal(valtyp))
      { ISA_LIT_CLASS lc;
	mINT64 value;   /* sign is important in range analysis */

	if(!(Is_AIR_TN_expression(tn))) {
	  error = not_a_litteral;
	  goto error_case;
	}

	if(AIR_Expression_immediate == Get_AIR_TN_exp_kind(tn)) {
	  lc = ISA_OPERAND_VALTYP_Literal_Class(valtyp);
	  value = (mINT64)Get_AIR_TN_exp_imm_val(tn);
	  if(!ISA_LC_Value_In_Class(value, lc)) {
	    error = not_in_range;
	    goto error_case;
	  }
	}

	if(AIR_Expression_reloc == Get_AIR_TN_exp_kind(tn)) {
	  lc = ISA_OPERAND_VALTYP_Literal_Class(valtyp);
	  value = (mINT64)Get_AIR_TN_exp_reloc_val(tn);
	  if(!ISA_LC_Value_In_Class(value, lc)) {
	    error = not_in_range;
	    goto error_case;
	  }
	}
      }
    else if (ISA_OPERAND_VALTYP_Is_Enum(valtyp)) {
      ISA_ENUM_CLASS_VALUE ecv;
      ISA_ENUM_CLASS ec = ISA_OPERAND_VALTYP_Enum_Class(valtyp);
      ISA_ENUM_CLASS_VALUE first_ecv = ISA_EC_First_Value(ec);
      ISA_ENUM_CLASS_VALUE last_ecv = ISA_EC_Last_Value(ec);
      
      if(!(Is_AIR_TN_expression(tn))) {
	error = not_a_litteral;
	goto error_case;
      }
      
      if(AIR_Expression_immediate == Get_AIR_TN_exp_kind(tn)) {
	ecv = (ISA_ENUM_CLASS_VALUE)Get_AIR_TN_exp_imm_val(tn);
	if(!(ecv>=first_ecv && ecv<=last_ecv)) {
	  error = not_in_range;
	  goto error_case;
	}
      }
    }
 
    return 0;

  error_case: /* TODO: error message */
    return 1;
  }

  /* Check that an op is conform to its targinfo description.
     If verbose mode is activated, print some information on f file.
     Return 0 if everthing's OK, 1 otherwise.
  */
  INT 
  AIR_check_op( FILE *f, AIR_OP* op, INT verbose )
  {
    INT     out = 0;
    mUINT32 nb_ops;
    mUINT32 nb_results;
    mUINT32 nb_ops_targ;
    mUINT32 nb_results_targ;
    mUINT32 i;
    mUINT32 flags;
    mUINT32 id;
    TOP     top;
    AIR_TN *tn;

    if(NULL==op)
      return out;
    if(!Is_AIR_OP(op))
      return out;

    /* Check the number of parameters */
    id  = Get_AIR_OP_id(op);
    top = Get_AIR_OP_TOP(op);
    nb_ops = Get_AIR_OP_nb_operands(op);
    nb_results = Get_AIR_OP_nb_results(op);
    flags = Get_AIR_OP_flags(op);

    if(TOP_UNDEFINED==top)
      return out;

    if(top>=TOP_count)
      { if(verbose)
	  (void)fprintf(f,"invalid topcode %d for op %d\n",
			top,id);
	return 1;
      }

    /* it is only in CGIR mode that we required a strict
       conformance to ISA_OPERAND information.
    */
    if((flags & AIR_OP_FLAG_CGIR_MODE))
      { nb_results_targ = ISA_OPERAND_INFO_Results(ISA_OPERAND_Info(top));
	nb_ops_targ = ISA_OPERAND_INFO_Operands(ISA_OPERAND_Info(top));

	if(nb_results!=nb_results_targ)
	  { if(verbose)
	      (void) fprintf(f,
			     "invalid number of results for op %s (id %d)\n",
			     TOP_Name(top),id);
	    out =1;
	  }

	if(nb_ops_targ!=nb_ops)
	  { if(verbose)
	      (void) fprintf(f,
			     "invalid number of operands for op %s (id %d)\n",
			     TOP_Name(top),id);
	    out = 1;
	  }
      }
    else if ((flags & AIR_OP_FLAG_ASM_MODE))
      { /* TODO */
      }

    if(out)
      return out;

    /* In the two following loops, the number of iterations
     * has to be modified in case of 'asm mode'.
     */ 
    if((flags & AIR_OP_FLAG_ASM_MODE))
      { nb_results = ISA_OPERAND_max_results;
	nb_ops     = ISA_OPERAND_max_operands;
      }

    /* Check that the results are correct
     * Note that in CGIR mode, we call AIR_check_operand
     * even if op==NULL. Indeed, in this last case, we have to
     * report an error.
     */
    for(i=0;i<nb_results;i++)
      { tn = Get_AIR_OP_resultn(op,i);
	if((flags & AIR_OP_FLAG_CGIR_MODE) || NULL!=tn)
          out |= AIR_check_tn(f,tn,top,i,1 /* is_result */,verbose);
      }

    /* Check that the operands are correct */
    for(i=0;i<nb_ops;i++)
      { tn = Get_AIR_OP_operandn(op,i);
	if((flags & AIR_OP_FLAG_CGIR_MODE) || NULL!=tn)
	  out |=  AIR_check_tn(f,tn,top,i,0 /* is_result */, verbose);
      }

    return out;
  }

  INT
  AIR_check_ops( FILE *f, AIR_OPS* ops, INT verbose )
  {
    int out = 0;
    AIR_OP *op = NULL;

    if(NULL==ops)
      return out;

    FOR_ALL_AIR_OPS_OPs(ops,op) {
      if(verbose) {
	fprintf(f,"Check op:%s\n",TOP_Name(Get_AIR_OP_TOP(op)));
      }
      out |= AIR_check_op(f,op,verbose);
    }
   
    return out;
  }

  /* Reset tn flags for all ops in
   * op list 'a'.
   */
  INT
  AIR_reset_tns_flags( AIR_OP *op)
  {
    INT    i;
    AIR_TN *tn;

    while(op)
      { for(i=0;i<ISA_OPERAND_max_results;i++)
	  { tn = Get_AIR_OP_resultn(op,i);
	    if(tn)
	      { Reset_AIR_TN_flags(tn);
	      }
            
	  }
	for(i=0;i<ISA_OPERAND_max_operands;i++)
	  { tn = Get_AIR_OP_operandn(op,i);
	    if(tn)
	      { Reset_AIR_TN_flags(tn);
	      }
	  }
	op = Get_AIR_OP_next(op);
      }
    return 0;
  }

  /* Dumping an op or a directive */
  void
  AIR_dump_op( FILE *f, AIR_OP *op )
  {
    if(NULL==op)
      (void) fprintf(f,"<null> pointer for AIR_OP\n");
    else if(Is_AIR_OP(op))
      { TOP top = Get_AIR_OP_TOP(op);
	mUINT32  nb_op;
	mUINT32  i;
	mUINT32  flags;
	mUINT32  count;
	static char buf[100];
	int cgir_mode = 0;
	AIR_TN *tn;


	flags = Get_AIR_OP_flags(op);
	(void)sprintf(buf,"flags %d",flags);
	if((flags & AIR_OP_FLAG_CGIR_MODE))
	  { strcat(buf, " - CGIR mode");
	    cgir_mode = 1;
	  }
	if((flags & AIR_OP_FLAG_ASM_MODE))
	  strcat(buf, " - ASM mode");

	(void)fprintf(f,"op id %d, TOP %d (%s), %s\n",
		      Get_AIR_OP_id(op),
		      top,
		      TOP_Name(top),
		      buf);

	nb_op = Get_AIR_OP_nb_results(op);
	(void)fprintf(f,"%d results\n",nb_op);
	count = 0;
	for(i=0; i<ISA_OPERAND_max_operands && count<nb_op; i++)
	  { tn = Get_AIR_OP_resultn(op,i);
	    if(tn)
	      ++count;
	    AIR_dump_tn(f,tn);
	  }

	nb_op = Get_AIR_OP_nb_operands(op);
	(void)fprintf(f,"%d operands\n",nb_op);
	count = (mUINT32)0;
	for(i=0; i<ISA_OPERAND_max_results && count<nb_op; i++)
	  { tn = Get_AIR_OP_operandn(op,i);
	    if(tn)
	      ++count;
	    AIR_dump_tn(f,tn);
	  }
	(void)fprintf(f,"\n");
      }
    else if (Is_AIR_directive(op))
      { (void)fprintf(f,"air directive\n"); /* TODO */
      }
    else
      (void)fprintf(f,"unknown op or directive\n");
    
    return;
  }

  void
  AIR_dump_ops( FILE *f, AIR_OPS *ops )
  {
    AIR_OP *op = NULL;
    FOR_ALL_AIR_OPS_OPs(ops,op) {
      AIR_dump_op(f,op);
    }
    return;
  }

  /* Initialization and termination steps */
  void
  AIR_init( void )
  {
    count_op = 0;
    count_section= 0;

    pool_string = AIR_memory_pool_init("string pool",4096);
    pool_inst   = AIR_memory_pool_init("op pool",4096);
    pool_section= AIR_memory_pool_init("section pool",4096);
    pool_hash   = AIR_memory_pool_init("pool for hash table",
				       1024*sizeof(AIR_Symbol));

    init_symbol_table("AIR symbol table",256);

    return;
  }

  void
  AIR_end( void )
  {
    /* Reset counters */
    count_op = 0;
    count_section= 0;

    /* Free memory */
    AIR_memory_pool_free(pool_string);
    pool_string = NULL;
    AIR_memory_pool_free(pool_inst);
    pool_inst   = NULL;
    AIR_memory_pool_free(pool_section);
    pool_section= NULL;
    AIR_memory_pool_free(pool_hash);
    pool_hash   = NULL;

    free_symbol_table();

    return;
  }

#ifdef TEST
  static void
  test( void )
  {
    {
      int i;
      char tab[50];
      AIR_init();

      for(i=0;i<10000;i++)
	{ sprintf(tab,"a%07d",i);
	  (void)AIR_new_symbol((AIR_String)tab,AIR_SYMBOL_NO_FLAGS);
	}

      AIR_dump_symbol_table(stdout);
      dump_hash_table(stdout,symtab->hash);
      AIR_end();
    }

#ifdef TARG_STxP70
    { AIR_OP* a1,*a2;
      AIR_OPS ops = AIR_OPS_EMPTY;
      AIR_init();

      a1 = AIR_mk_op(TOP_nop);
      a2 = AIR_mk_op(
		     TOP_negu,
		     AIR_new_tn_reg(ISA_REGISTER_CLASS_gpr,
				    ISA_REGISTER_SUBCLASS_UNDEFINED,
				    0),
		     AIR_new_tn_reg(ISA_REGISTER_CLASS_gr,
				    ISA_REGISTER_SUBCLASS_UNDEFINED,
				    7),
		     AIR_new_tn_reg(ISA_REGISTER_CLASS_gpr,
				    ISA_REGISTER_SUBCLASS_UNDEFINED,
				    2)
		     );

      AIR_OPS_Prepend_Op(&ops,a1);
      AIR_OPS_Prepend_Op(&ops,a2);
      AIR_check_op(stdout,a2,1/*verbose*/);
 
      AIR_dump_op(stdout,a1);
      AIR_dump_ops(stdout,a2);

      AIR_end();
    }
#endif
  }

  int 
  main( int argc, char* argv[] )
  {
    test();
    exit(0);
  }
#endif                                /* _TEST_ */

#ifdef __cplusplus
}
#endif
#endif                               /* __AIR_C__ */
