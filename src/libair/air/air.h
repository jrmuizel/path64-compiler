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

/* 
   AIR stands for Assembly Intermediate Representation
   This file is the central header file for AIR.
*/

#ifndef __AIR_H__
#define __AIR_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef DYNAMIC_CODE_GEN
#include "dyn_isa_topcode.h"           /* Definition of TOP */
#include "dyn_isa_operands.h"
#else
#include "topcode.h"
#include "targ_isa_operands.h"
#endif

/* AIR is strictly in C for compatibility with
   GNU binutils policy.
*/
#ifdef __cplusplus
extern "C" {
#endif

  /* Symbol table ..........................................................*/

  /* Symbol flags - these are the traditional ELF flags */
#define AIR_SYMBOL_NO_FLAGS          ((unsigned int)0x00000000)
#define AIR_SYMBOL_LOCAL             ((unsigned int)0x00000001)
#define AIR_SYMBOL_GLOBAL            ((unsigned int)0x00000002)
#define AIR_SYMBOL_WEAK	             ((unsigned int)0x00000004)
#define AIR_SYMBOL_FUNCTION          ((unsigned int)0x00000008)
#define AIR_SYMBOL_SECTION           ((unsigned int)0x00000010)
#define AIR_SYMBOL_FILE	             ((unsigned int)0x00000020)
#define AIR_SYMBOL_OBJECT            ((unsigned int)0x00000040)

  typedef char * AIR_String;        /* A normal NULL terminated string */

  typedef struct{
    AIR_String    name;
    mUINT32       size;
    mUINT64       val;
    mUINT32       flags;
    mUINT32       alignment;
    mUINT32       refcounter;
    mUINT32       id;              /* Symbol id is unique */
  } AIR_Symbol;

#define Get_AIR_symbol_name(this)       ((this)->name)
#define Set_AIR_symbol_name(this,name_) ((this)->name = name_)
#define Get_AIR_symbol_size(this)       ((this)->size)
#define Set_AIR_symbol_size(this,size_) ((this)->size = size_)
#define Get_AIR_symbol_val(this)        ((this)->val)
#define Set_AIR_symbol_val(this,val_)   ((this)->val = (long long) val_)
#define Get_AIR_symbol_flags(this)      ((this)->flags)
#define Set_AIR_symbol_flags(this,flags_)	\
  ((this)->flags = flags_)
#define Get_AIR_symbol_alignment(this)  ((this)->alignment)
#define Set_AIR_symbol_alignment(this,align_)	\
  ((this)->alignment = align_)
#define Get_AIR_symbol_refcounter(this) ((this)->refcounter)
#define Set_AIR_symbol_refcounter(this,count_)	\
  ((this)->refcounter = count_)
#define Get_AIR_symbol_id(this)         ((this)->id)
#define Set_AIR_symbol_id(this,id_)     ((this)->id = id_)

#define Is_AIR_symbol_noflags(this)   ((this)_>flags == AIR_SYMBOL_NO_FLAGS)
#define Is_AIR_symbol_local(this)     ((this)->flags & AIR_SYMBOL_LOCAL)
#define Is_AIR_symbol_global(this)    ((this)->flags & AIR_SYMBOL_GLOBAL)
#define Is_AIR_symbol_weak(this)      ((this)->flags & AIR_SYMBOL_WEAK)
#define Is_AIR_symbol_function(this)  ((this)->flags & AIR_SYMBOL_FUNCTION)
#define Is_AIR_symbol_section(this)   ((this)->flags & AIR_SYMBOL_SECTION)
#define Is_AIR_symbol_file(this)      ((this)->flags & AIR_SYMBOL_FILE)
#define Is_AIR_symbol_object(this)    ((this)->flags & AIR_SYMBOL_OBJECT)

  /* Definition of sections ................................................*/

  typedef enum {  
    AIR_Section_Nobits     = 0x0,
    AIR_Section_Writable   = 0x1,
    AIR_Section_Readable   = 0x2,
    AIR_Section_Executable = 0x4
  } AIR_Section_Attributes;

  typedef struct{
    mUINT32                id;
    mUINT32                size;
    AIR_Section_Attributes attributes;
  } AIR_Section;

#define Get_AIR_section_id(this)          ((this)->id)
#define Set_AIR_section_id(this,id_)      ((this)->id   = id_)
#define Get_AIR_section_size(this)        ((this)->size)
#define Set_AIR_section_size(this,size_)  ((this)->size = size_)
#define Get_AIR_section_attributes(this)  ((this)->attributes)
#define Set_AIR_section_attributes(this,att_)   \
  ((this)->attributes = att_)
#define Reset_AIR_section_attributes(this)	\
  ((this)->attributes = AIR_Section_Nobits)

#define Is_AIR_section_write(this) ((this)->attributes & AIR_Section_Writable)
#define Is_AIR_section_read(this)  ((this)->attributes & AIR_Section_Readable)
#define Is_AIR_section_exec(this)  ((this)->attributes & AIR_Section_Executable)

  /* Definition of tns (temporary name) corresponding to operand instances ......................*/

  typedef enum { /* Types of op result/tn */
    AIR_TN_unknown = 0,
    AIR_TN_expression,
    AIR_TN_register,
    AIR_TN_buffer,
  } AIR_TN_Kind ;

  /* Listing the various kind of expressions */
  typedef enum {
    AIR_Expression_immediate,
    AIR_Expression_reloc,
    AIR_Expression_enum,
    AIR_Expression_regmask,
  } AIR_Expression_Kind;

  typedef struct {
    mUINT32    relocid;/* Relocation ID known by the assembler and linker. */
    void      *symid;  /* Base symbol on which relocation is applied. */
    void      *symop;  /* Second symbol (if needed). The symbol id will be stored *
			* in place of the immediate field. */
    void      *expr;   /* Client expression can be stored here. */
    mUINT64    val;    /* Addend. */
  } AIR_RelocT;


  /* modelisation of an tn.
   */
  typedef struct {
    AIR_TN_Kind kind;
    
    /* String associated to the tn.
     * This string is normally NULL.        
     * Meaning of this string is controlled
     * by flags and flags are phase specific
     * (decode, encode, print, parse...).
     */
    AIR_String       string;
    mUINT32          flags;
    
    union {
      /* an immediate (from an architectural point of
	 view) can be the concrete view of:
	 - an immediate
	 - a relocation on a symbol
	 - a register mask
      */
      struct {
	AIR_Expression_Kind  kind;

	union {

	  struct {
            mUINT64           val;
	  } imm;

#define AIR_NO_RELOC     0
	  AIR_RelocT reloc;

	  struct {
	    ISA_ENUM_CLASS        ec;
	    ISA_ENUM_CLASS_VALUE  ecv;
	  } eenum;

	  struct{
            ISA_REGISTER_CLASS    regclass;
            ISA_REGISTER_SUBCLASS subclass;
            mUINT32               nb_registers;
            mUINT64               mask;
	  } regmask;
	}u2;

      } air_exp;

      /* modelisation of a register is strongly based
	 on targinfo representation.
      */
      struct {
	ISA_REGISTER_CLASS    regclass;
	ISA_REGISTER_SUBCLASS subclass;
	mUINT32               reg;
      } air_reg;

      struct {
	unsigned int len;
	char*        buf;
      } air_buf;
    } u1;
  } AIR_TN;

#define Get_AIR_TN_kind(this)			\
  ((this)->kind)
#define Set_AIR_TN_kind(this,kind_)		\
  ((this)->kind = kind_)
#define Is_AIR_TN_register(this)		\
  ((this->kind == AIR_TN_register))
#define Is_AIR_TN_expression(this)		\
  ((this->kind == AIR_TN_expression))
#define Is_AIR_TN_buffer(this)			\
  ((this->kind == AIR_TN_buffer))

#define Get_AIR_TN_string(this)          ((this)->string)
#define Set_AIR_TN_string(this,string_)  ((this)->string = string_)
#define Get_AIR_TN_flags(this)           ((this)->flags)
#define Set_AIR_TN_flags(this,flags_)    ((this)->flags = flags_)
#define Add_AIR_TN_flags(this,flags_)    ((this)->flags |= flags_)
#define Reset_AIR_TN_flags(this)         ((this)->flags = (mUINT32)0)
#define Test_AIR_TN_flags(this,flags_)		\
  ((this)->flags & (flags_))

#define Get_AIR_TN_reg_regnum(this)		\
  ((this)->u1.air_reg.reg)
#define Set_AIR_TN_reg_regnum(this,reg_)	\
  ((this)->u1.air_reg.reg = reg_)
#define Get_AIR_TN_reg_regclass(this)		\
  ((this)->u1.air_reg.regclass)
#define Set_AIR_TN_reg_regclass(this,class_)	\
  ((this)->u1.air_reg.regclass = class_)
#define Get_AIR_TN_reg_subclass(this)		\
  ((this)->u1.air_reg.subclass)
#define Set_AIR_TN_reg_subclass(this,subclass_)	\
  ((this)->u1.air_reg.subclass = subclass_)
 

#define Get_AIR_TN_exp_kind(this)		\
  ((this)->u1.air_exp.kind)
#define Set_AIR_TN_exp_kind(this,kind_)		\
  ((this)->u1.air_exp.kind = kind_)

#define Get_AIR_TN_exp_imm_val(this)		\
  ((this)->u1.air_exp.u2.imm.val)
#define Set_AIR_TN_exp_imm_val(this,val_)	\
  ((this)->u1.air_exp.u2.imm.val = val_)

#define Get_AIR_TN_exp_enum_ecv(this)		\
  ((this)->u1.air_exp.u2.eenum.ecv)
#define Set_AIR_TN_exp_enum_ecv(this,val_)	\
  ((this)->u1.air_exp.u2.eenum.ecv = val_)
#define Get_AIR_TN_exp_enum_ec(this)		\
  ((this)->u1.air_exp.u2.eenum.ec)
#define Set_AIR_TN_exp_enum_ec(this,val_)	\
  ((this)->u1.air_exp.u2.eenum.ec = val_)

#define Get_AIR_TN_exp_regmask_regclass(this)	\
  ((this)->u1.air_exp.u2.regmask.regclass)
#define Set_AIR_TN_exp_regmask_regclass(this,regclass_)	\
  ((this)->u1.air_exp.u2.regmask.regclass = regclass_)
#define Get_AIR_TN_exp_regmask_subclass(this)	\
  ((this)->u1.air_exp.u2.regmask.subclass)
#define Set_AIR_TN_exp_regmask_subclass(this,subclass_)	\
  ((this)->u1.air_exp.u2.regmask.subclass = subclass_)
#define Get_AIR_TN_exp_regmask_mask(this)	\
  ((this)->u1.air_exp.u2.regmask.mask)
#define Set_AIR_TN_exp_regmask_mask(this,mask_)	\
  ((this)->u1.air_exp.u2.regmask.mask = mask_)
#define Get_AIR_TN_exp_regmask_nbregisters(this)	\
  ((this)->u1.air_exp.u2.regmask.nb_registers)
#define Set_AIR_TN_exp_regmask_nbregisters(this,nb_reg_)	\
  ((this)->u1.air_exp.u2.regmask.nb_registers = nb_reg_)

#define Get_AIR_TN_exp_reloc_relocid(this)	\
  ((this)->u1.air_exp.u2.reloc.relocid)
#define Set_AIR_TN_exp_reloc_relocid(this,relocid_)	\
  ((this)->u1.air_exp.u2.reloc.relocid = relocid_)
#define Get_AIR_TN_exp_reloc_symid(this)	\
  ((this)->u1.air_exp.u2.reloc.symid)
#define Set_AIR_TN_exp_reloc_symid(this,symid_)	\
  ((this)->u1.air_exp.u2.reloc.symid = symid_)
#define Get_AIR_TN_exp_reloc_symop(this)	\
  ((this)->u1.air_exp.u2.reloc.symop)
#define Set_AIR_TN_exp_reloc_symop(this,symop_)	\
  ((this)->u1.air_exp.u2.reloc.symop = symop_)
#define Get_AIR_TN_exp_reloc_val(this)		\
  ((this)->u1.air_exp.u2.reloc.val)
#define Set_AIR_TN_exp_reloc_val(this,val_)	\
  ((this)->u1.air_exp.u2.reloc.val = val_)
#define Get_AIR_TN_exp_reloc_expr(this)		\
  ((this)->u1.air_exp.u2.reloc.expr)
#define Set_AIR_TN_exp_reloc_expr(this,expr_)	\
  ((this)->u1.air_exp.u2.reloc.expr = expr_)

#define Get_AIR_TN_buffer(this)			\
  ((this)->u1.air_buf.buf)
#define Set_AIR_TN_buffer(this,buf_)		\
  ((this)->u1.air_buf.buf = buf_)
#define Get_AIR_TN_buffer_length(this)		\
  ((this)->u1.air_buf.len)
#define Set_AIR_TN_buffer_length(this,len_)	\
  ((this)->u1.air_buf.len=len_)

  /* Definition of an op ..........................................*/

  typedef enum{
    AIR_is_unknown_kind    = 0,
    AIR_is_op_kind,
    AIR_is_directive_kind,
  } AIR_Op_Directive_Kind;

  typedef enum{
    AIR_directive_none = 0,
    AIR_directive_align   ,
    AIR_change_mode       ,
  } AIR_Directive_Kind;


  typedef struct AIR_OP{
    /* At AIR level, ops and directives can be
       mixed.
    */
    AIR_Op_Directive_Kind kind;

    union{
      struct{
	/* Description of an op or directive. 
	   By construction, id is unique. Id 0 is used to
	   simulate an invalid symbol.
	*/ 
	mUINT32 id;
	TOP     top;

	/* Flags.

	We can define two "modes" for op
	tns. In the first mode, conformance
	to CGIR is strict: there are as many
	tns and results in an AIR_OP as
	in the CGIR. In the second mode, tns
	and results are set only in they are encoded
	in the op (and described in PACK
	information).
	*/
#define AIR_OP_FLAG_NONE       0x0
#define AIR_OP_FLAG_CGIR_MODE  0x1
#define AIR_OP_FLAG_ASM_MODE   0x2
	mUINT32 flags;
            

	/* At AIR level, distinction between tns and
	   results is not relevant. However, we introduce these
	   two distincts tables so as to comply with Open64
	   targinfo description.
	*/
	mUINT32      nb_operands;
	AIR_TN       *tab_operands[ISA_OPERAND_max_operands];
	mUINT32      nb_results;
	AIR_TN       *tab_results[ISA_OPERAND_max_results];
      } inst;

      struct { AIR_Directive_Kind kind;
	AIR_String         string;
	/* TODO: complete */
      } dir;
    }u;

    struct AIR_OP  *next;
    struct AIR_OP  *prev;
  } AIR_OP;

  /* May be this should be deported in a target specific folder */
#define Set_AIR_kind_directive(this)		\
  ((this)->kind=AIR_is_directive_kind)
#define Set_AIR_kind_op(this)			\
  ((this)->kind=AIR_is_op_kind)
#define Is_AIR_OP(this)				\
  ((this)->kind==AIR_is_op_kind)
#define Is_AIR_directive(this)			\
  ((this)->kind==AIR_is_directive_kind)

#define Get_AIR_OP_TOP(this)             ((this)->u.inst.top)
#define Set_AIR_OP_TOP(this,top_)        ((this)->u.inst.top = top_)
#define Get_AIR_OP_id(this)              ((this)->u.inst.id)
#define Set_AIR_OP_id(this,id_)          ((this)->u.inst.id = id_)

#define Get_AIR_OP_flags(this)           ((this)->u.inst.flags)
#define Set_AIR_OP_flags(this,flags_)    ((this)->u.inst.flags = flags_)
#define Reset_AIR_OP_flags(this)   ((this)->u.inst.flags = AIR_OP_FLAG_NONE)

#define Get_AIR_OP_nb_operands(this)     ((this)->u.inst.nb_operands)
#define Set_AIR_OP_nb_operands(this,nb_) ((this)->u.inst.nb_operands = nb_)
#define Get_AIR_OP_nb_results(this)      ((this)->u.inst.nb_results)
#define Set_AIR_OP_nb_results(this,nb_)  ((this)->u.inst.nb_results = nb_)

#define Get_AIR_OP_operandn(this,i)		\
  ((this)->u.inst.tab_operands[i])
#define Set_AIR_OP_operandn(this,i,opn)		\
  ((this)->u.inst.tab_operands[i] = opn)
#define Get_AIR_OP_resultn(this,i)		\
  ((this)->u.inst.tab_results[i])
#define Set_AIR_OP_resultn(this,i,opn)		\
  ((this)->u.inst.tab_results[i] = opn)

#define Get_AIR_OP_next(this)            ((this)->next)
#define Set_AIR_OP_next(this,next_)      ((this)->next = next_)
#define Get_AIR_OP_prev(this)            ((this)->prev)
#define Set_AIR_OP_prev(this,prev_)      ((this)->prev = prev_)

  /* OP list definition ..........................................*/

  typedef struct AIR_OPS{
    AIR_OP  *first;
    AIR_OP  *last;
    mUINT16 length;    
  } AIR_OPS;

#define AIR_OPS_first(ops)  ((ops)->first+0)
#define AIR_OPS_last(ops)   ((ops)->last+0)
#define AIR_OPS_length(ops) ((ops)->length+0)
#define AIR_OPS_EMPTY { NULL, NULL, 0 }

#define FOR_ALL_AIR_OPS_OPs_FWD(ops,op)					\
  for (op = AIR_OPS_first(ops); op && op != Get_AIR_OP_next(AIR_OPS_last(ops)); \
       op = Get_AIR_OP_next(op))

#define FOR_ALL_AIR_OPS_OPs_REV(ops,op)					\
  for (op = AIR_OPS_last(ops); op && op != Get_AIR_OP_prev(AIR_OPS_first(ops)); \
       op = Get_AIR_OP_prev(op))

#define FOR_ALL_AIR_OPS_OPs FOR_ALL_AIR_OPS_OPs_FWD

  extern AIR_OPS *AIR_OPS_Create(void);
  extern void AIR_OPS_Remove_All(AIR_OPS *ops);
  extern AIR_OPS *AIR_OPS_Init(AIR_OPS *ops);
  extern void AIR_OPS_Remove_Op(AIR_OPS *ops, AIR_OP *op);
  extern void AIR_OPS_Remove_Ops(AIR_OPS *ops, AIR_OPS *remove_ops);
  extern void AIR_OPS_Insert_Op_Before(AIR_OPS *ops, AIR_OP *point, AIR_OP *op);
  extern void AIR_OPS_Insert_Op_After(AIR_OPS *ops, AIR_OP *point, AIR_OP *op);
  extern void AIR_OPS_Append_Op(AIR_OPS *ops, AIR_OP *op);
  extern void AIR_OPS_Prepend_Op(AIR_OPS *ops, AIR_OP *op);
  extern void AIR_OPS_Insert_Ops_Before(AIR_OPS *ops, AIR_OP *point, AIR_OPS *insert_ops);
  extern void AIR_OPS_Insert_Ops_After(AIR_OPS *ops, AIR_OP *point, AIR_OPS *insert_ops);
  extern void AIR_OPS_Append_Ops(AIR_OPS *ops, AIR_OPS *insert_ops);
  extern void AIR_OPS_Prepend_Ops(AIR_OPS *ops, AIR_OPS *insert_ops);
  extern void AIR_OPS_Insert_Op(AIR_OPS *ops, AIR_OP *point, AIR_OP *op, BOOL before);
  extern void AIR_OPS_Insert_Ops(AIR_OPS *ops, AIR_OP *point, AIR_OPS *insert_ops, BOOL before);

  /* Services - definition of return values ..................................*/

#define AIR_EVENT_END       -1    /* Normal end          */
#define AIR_EVENT_END_ERROR -2    /* Unrecoverable error */
#define AIR_EVENT_NEXT       0

  /* AIR generic routines ..................................................*/
  extern AIR_String   AIR_new_string      (mUINT32);
  extern AIR_String   AIR_duplicate_string(AIR_String);
  extern AIR_Section* AIR_new_section     (AIR_String,AIR_Section_Attributes);
  extern AIR_Symbol*  AIR_new_symbol      (AIR_String, mUINT32);

  extern AIR_TN* AIR_new_tn_reg (ISA_REGISTER_CLASS,ISA_REGISTER_SUBCLASS,
				 mUINT32);

  extern AIR_TN* AIR_new_tn_imm (mUINT64);

  extern AIR_TN* AIR_new_tn_enum (ISA_ENUM_CLASS ec, ISA_ENUM_CLASS_VALUE ecv);

  extern AIR_TN* AIR_new_tn_reloc ( mUINT32 relocid, void *symid, void *symop,
				    mUINT64 val, void *expr);
  extern AIR_TN* AIR_new_tn_regmask(ISA_REGISTER_CLASS,ISA_REGISTER_SUBCLASS,
				    mUINT64);
  extern AIR_TN* AIR_new_tn_regmask_2(ISA_REGISTER_CLASS,ISA_REGISTER_SUBCLASS,
				      int,int);
  extern AIR_TN* AIR_duplicate_tn(AIR_TN*);

  extern void         AIR_dump_symbol     (FILE*, AIR_Symbol *);
  extern AIR_OP* AIR_new_op (void);
  extern AIR_OP* AIR_new_op_with_TOP (TOP);
  extern AIR_Symbol*  AIR_symbol_by_id    (mUINT32);
  extern AIR_Symbol*  AIR_find_symbol_by_string(AIR_String,mUINT32);
  extern void         AIR_dump_tn    (FILE*, AIR_TN*);
  extern void         AIR_dump_op(FILE*, AIR_OP*);
  extern void         AIR_dump_ops(FILE*, AIR_OPS*);
  extern void         AIR_dump_symbol_table(FILE*);
  extern void         AIR_init            (void);
  extern void         AIR_end             (void);
  extern INT          AIR_check_op(FILE*, AIR_OP*,INT);
  extern INT          AIR_check_ops(FILE*, AIR_OPS*,INT);
  extern INT          AIR_check_tn   (FILE*,AIR_TN*,TOP,INT,INT,INT);
  extern INT          AIR_reset_tns_flags(AIR_OP*);
  extern AIR_OP* AIR_mk_full_op  (TOP,mUINT32,...);
  extern AIR_String   Get_AIR_section_name(AIR_Section*);

#ifdef __cplusplus
}
#endif

#endif /* __AIR_H__ */
