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

/* Implementation of the generic services - decode, print, parse */

/* Standard C header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Targinfo header files */
#include "topcode.h"
#include "targ_isa_print.h"
#include "targ_abi_properties.h"
#include "targ_isa_pack.h"
#include "targ_isa_bundle.h"
#include "ti_asm.h"

/* Libair header files */
#include "air.h"
#include "air_services.h"
#include "air_utils.h"

/* AIR is strictly in C */
#ifdef __cplusplus
extern "C" {
#endif

  /* Decode service ................................................*/
  static AIR_TN*
  AIR_build_disasm_opnd( const ISA_OPERAND_VALTYP *vtype, INT64 val )
  {
    AIR_TN *a = NULL;

    if (ISA_OPERAND_VALTYP_Is_Register(vtype)) 
      { ISA_REGISTER_CLASS rc = ISA_OPERAND_VALTYP_Register_Class(vtype);
        ISA_REGISTER_SUBCLASS sub = ISA_OPERAND_VALTYP_Register_Subclass(vtype);
        a = AIR_new_tn_reg(rc,sub,(unsigned int)val);
        goto end;
      }

    if(ISA_OPERAND_VALTYP_Is_Literal(vtype))
      { a = AIR_new_tn_imm((unsigned long long int)val);
        goto end;
      }

    if(ISA_OPERAND_VALTYP_Is_Enum(vtype))
      { ISA_ENUM_CLASS rc = ISA_OPERAND_VALTYP_Enum_Class(vtype);
        a = AIR_new_tn_enum(rc,(unsigned long long int)val);
        goto end;
      }
    assert(0);   /* TODO: error message */

  end:
    return a;
  }

  /* Build TN for implicit operands */
  static AIR_TN*
  AIR_build_implicit_TN ( const ISA_OPERAND_VALTYP * vtype) {
    AIR_TN* a = NULL;

    if (ISA_OPERAND_VALTYP_Is_Register(vtype)) {
      ISA_REGISTER_CLASS rc = ISA_OPERAND_VALTYP_Register_Class(vtype);
      ISA_REGISTER_SUBCLASS sub = ISA_OPERAND_VALTYP_Register_Subclass(vtype);
      
      if ( (sub != ISA_REGISTER_SUBCLASS_UNDEFINED) && 
           (ISA_REGISTER_SUBCLASS_INFO_Count(ISA_REGISTER_SUBCLASS_Info(sub))==1)
         ) {
        return AIR_build_disasm_opnd(vtype,0);
      } else {
        return NULL;
      }
    }
    return NULL;
  }
  
  /* Decode a new instruction */
  int
  AIR_decode_service( AIR_Decode *inst )
  {
    TOP    top;
    INT64  result[ISA_OPERAND_max_results];
    INT64  opnd[ISA_OPERAND_max_operands];
    INT    comp;
    INT    index;
    INT    w;
    AIR_TN *a; 
    const  ISA_PACK_OPND_INFO *pinfo;
    const  ISA_OPERAND_INFO *oinfo;
    const  ISA_OPERAND_VALTYP *vtype;
    INT    nbresults;
    INT    nbopnds;

    /* Reuse TI_ASM_Unpack_Inst routine.
     */
    top = TI_ASM_Unpack_Inst(
			     &(inst->inst),
			     inst->unit,
			     result,
			     opnd,
			     0);

    /* Create a new air instruction */
    inst->air_inst = AIR_new_op_with_TOP(top);
    if(TOP_UNDEFINED==top)
      return AIR_EVENT_NEXT;

    pinfo     = ISA_PACK_OPND_Info(top);
    oinfo     = ISA_OPERAND_Info(top);
    comp      = ISA_PACK_OPND_INFO_Comp(pinfo);
    nbresults = 0;
    nbopnds   = 0;

    /* Create an AIR_opnd for each tn and attach it */
    /* to the op.                               */
    for(w = 0; w < ISA_PACK_Inst_Words(top); w++) {
      while(comp != ISA_PACK_COMP_end) { 
	if(ISA_PACK_Is_Component_Operand(comp)) {
	  index = comp - ISA_PACK_COMP_opnd;
	  vtype = ISA_OPERAND_INFO_Operand(oinfo, index);
	  a = AIR_build_disasm_opnd(vtype,opnd[index]);
	  Set_AIR_OP_operandn(inst->air_inst,index,a);
	  ++nbopnds;
	}
	else {
	  index = comp - ISA_PACK_COMP_result;
	  vtype = ISA_OPERAND_INFO_Result(oinfo, index);
	  a = AIR_build_disasm_opnd(vtype,result[index]);
	  Set_AIR_OP_resultn(inst->air_inst,index,a);
	  ++nbresults;
	}
      
	/* Iterate on next op */
	pinfo = ISA_PACK_OPND_Info_Iterate(pinfo);
	comp  = ISA_PACK_OPND_INFO_Comp(pinfo);
      }
      /* Next word */
      pinfo = ISA_PACK_OPND_Info_Iterate(pinfo);
      comp  = ISA_PACK_OPND_INFO_Comp(pinfo);
    }

    /* Fill in the last information */
    Set_AIR_OP_flags(inst->air_inst,AIR_OP_FLAG_CGIR_MODE);
    Set_AIR_OP_nb_operands(inst->air_inst,ISA_OPERAND_INFO_Operands(ISA_OPERAND_Info(top)));
    Set_AIR_OP_nb_results(inst->air_inst,ISA_OPERAND_INFO_Results(ISA_OPERAND_Info(top)));

    for (index=0;index<Get_AIR_OP_nb_operands(inst->air_inst);index++) {
       if (NULL == Get_AIR_OP_operandn(inst->air_inst,index)) {
         a = AIR_build_implicit_TN(ISA_OPERAND_INFO_Operand(ISA_OPERAND_Info(top),index));
         Set_AIR_OP_operandn(inst->air_inst,index,a);
       }
    }

    for (index=0;index<Get_AIR_OP_nb_results(inst->air_inst);index++) {
       mINT8 same_res;
       
       if (NULL == Get_AIR_OP_resultn(inst->air_inst,index)) {
         a = AIR_build_implicit_TN(ISA_OPERAND_INFO_Result(ISA_OPERAND_Info(top),index));
         Set_AIR_OP_resultn(inst->air_inst,index,a);
       }
       /* If this result is a "SameRes" then duplicate the link */
       if ((same_res = TOP_Same_Res_Operand(top,index)) != -1) {
         Set_AIR_OP_operandn(inst->air_inst,same_res,Get_AIR_OP_resultn(inst->air_inst,index));
       }
    }
    
    return AIR_EVENT_NEXT;
  }

/* ====================================================================
 *   r_value
 *
 *   Returns TN value depending of its type.
 * ====================================================================
 */
static INT64
r_value( AIR_TN *tn )
{
  if(NULL==tn)
    return 0;
  if (Is_AIR_TN_expression(tn)) {
    switch(Get_AIR_TN_exp_kind(tn)) {
    case AIR_Expression_immediate:
      return Get_AIR_TN_exp_imm_val(tn);
      break;
    case AIR_Expression_reloc:
      return Get_AIR_TN_exp_reloc_val(tn);
      break;
    case AIR_Expression_regmask:
      return Get_AIR_TN_exp_regmask_mask(tn);
      break;
    case AIR_Expression_enum:
      return Get_AIR_TN_exp_enum_ecv(tn);
      break;
    default:
      AIR_fatal_error("Internal","IE_004","Expression not yet printable\n");
    }
  }
  else if(Is_AIR_TN_register(tn)) {
     return Get_AIR_TN_reg_regnum(tn);
  }
  return 0;
}




  /* Print service .................................................*/
  static AIR_String
  AIR_print_format_opnd( TOP top, int is_opnd, int index, AIR_TN *tn,
			 BOOL hexaformat, AIR_Print * inst )
  {
    AIR_String a = NULL;
    int length;
    const char *rname;
    const ISA_OPERAND_INFO *oinfo = ISA_OPERAND_Info(top);
    const ISA_REGISTER_CLASS_INFO *rcinfo;
    const ISA_REGISTER_SUBCLASS_INFO *sub_rcinfo;
    const ISA_OPERAND_VALTYP *vtype;
          ISA_RELOC reloc;
          char symbname[50];
    ISA_REGISTER_CLASS rc;
    ISA_REGISTER_SUBCLASS sub_rc;
    mUINT32 regnum;
    mUINT64 val;
    INT     size;

    if(NULL==tn)
      return a;

    /* if a string has already been built */
    if(Test_AIR_TN_flags(tn, AIR_PRINT_OPERAND_STRING_PREPRINT))
      { a = Get_AIR_TN_string(tn);
	assert(a!=NULL);
	return a;
      }

    if(Is_AIR_TN_register(tn))
      {
	rc  = Get_AIR_TN_reg_regclass(tn);
	regnum = Get_AIR_TN_reg_regnum(tn); 
	sub_rc = Get_AIR_TN_reg_subclass(tn);
	if (sub_rc == ISA_REGISTER_SUBCLASS_UNDEFINED) {
	  rcinfo = ISA_REGISTER_CLASS_Info(rc);
	  rname = ISA_REGISTER_CLASS_INFO_Reg_Name(rcinfo, (INT)regnum);
	} else {
	  sub_rcinfo = ISA_REGISTER_SUBCLASS_Info(sub_rc);
	  rname = ISA_REGISTER_SUBCLASS_INFO_Reg_Name(sub_rcinfo, (INT)regnum);
	}
      
	length = strlen("%s") + strlen(rname) + 1;   /* overestimate */
	a = AIR_new_string(length);
	sprintf(a,"%s",rname);
	return a;
      }

    if(Is_AIR_TN_expression(tn))
      { vtype = is_opnd ? ISA_OPERAND_INFO_Operand(oinfo, index): 
	  ISA_OPERAND_INFO_Result(oinfo,index);

	switch(Get_AIR_TN_exp_kind(tn))
	  { case AIR_Expression_immediate:
	      val = Get_AIR_TN_exp_imm_val(tn);
	      size= ISA_OPERAND_VALTYP_Size(vtype);
              
              // [HC]: in case of literal, size of literal class must
              //       be used instead of size of operand in case of 
              //       scaling factor
              if (ISA_OPERAND_VALTYP_Is_Literal(vtype)) {
                 ISA_LIT_CLASS lc;
                 int i;
                
                 lc = ISA_OPERAND_VALTYP_Literal_Class(vtype);
                 for (i=0;size==ISA_OPERAND_VALTYP_Size(vtype) && i<=ISA_LC_Ranges(lc);i++) {
                    if (ISA_LC_Scaling_Value(lc,i) != 0) {
                      size = ISA_OPERAND_VALTYP_Size(vtype) + ISA_LC_Scaling_Value(lc,i);
                    }
                 }
		 if (ISA_LC_Is_Negative(lc)) {
		   val = -1 * val;
		 }
              }

              symbname[0] = 0;
	      if (ISA_OPERAND_VALTYP_Relocs(vtype)) {
                reloc = ISA_OPERAND_VALTYP_First(vtype);
	        if (reloc != ISA_RELOC_UNDEFINED) {
		  const char * name = NULL;
		  
                 if (NULL!=inst->get_symbol_at_address) {
	          if (ISA_RELOC_Is_PC_Rel(reloc)) {
	            if (inst->PC+val) {
		       name = inst->get_symbol_at_address(inst->PC+val,inst->disasm_info);
		    }
  		  } else {
		    if (val) {
    		       name = inst->get_symbol_at_address(val,inst->disasm_info);
		    }
		  }
	         }
		  if (NULL!=name) {
		    strncat(symbname,name,50);
		  }
	        }
	      }
	      
	      /* When printing in hexadecimal format 
	       * we take into account the size of the
	       * operand.
	       */
              /* Operand is displayed in nr of bytes 
               * even for scaled values (mask result with ~(-1LL<<size))
               */
              a = AIR_new_string((symbname[0]!=0)?strlen(symbname)+1:50);
	      if (symbname[0]) {
                sprintf(a,"%s",symbname);
	      } else if (hexaformat) { 
	        if (size<32) { 
		  INT32 val32s = (INT32)val;
		  
		  sprintf(a,"0x%x",val32s&~(-1<<size));
		} else {
		  sprintf(a,"0x%llx",val&~(-1LL<<size));
		}
	      } else {
	        if (ISA_OPERAND_VALTYP_Is_Signed(vtype)) {
		  sprintf(a,"%lld",val);
		} else {
		  sprintf(a,"%llu",val);
		}
	      }
	      break;

	  case AIR_Expression_regmask:
	    a = AIR_new_string(50);
	    snprintf(a,50,"0x%x",(UINT32)Get_AIR_TN_exp_regmask_mask(tn));
	    break;

	  case AIR_Expression_reloc:
	    a = AIR_new_string(50);
	    snprintf(a,50,"%lld",(UINT64)Get_AIR_TN_exp_reloc_val(tn));
	    break;

	  case AIR_Expression_enum:
 	    a = AIR_new_string(50);
            if (ISA_EC_Emit_Name(Get_AIR_TN_exp_enum_ec(tn))) {
              snprintf(a,50,"%s",ISA_ECV_Name(Get_AIR_TN_exp_enum_ecv(tn)));
            } else {
	      snprintf(a,50,"%lld",(UINT64)ISA_ECV_Intval(Get_AIR_TN_exp_enum_ecv(tn)));
	    }
	    break;
          
	  default:
	    assert(0);
	    break;
	  }

	return a;
      }

    assert(0);

    return a;
  }

  int
  AIR_print_service( AIR_Print *inst )
  {
    ISA_PRINT_OPND_INFO opnd[ISA_OPERAND_max_operands];
    ISA_PRINT_OPND_INFO result[ISA_OPERAND_max_results];
    int          i;
    int          index;
    AIR_TN       *tn;
    TOP          top;
    AIR_OP       *op = inst->air_inst;

    top = Get_AIR_OP_TOP(op); 

    if (TOP_UNDEFINED==top) {
       if (inst->print_unknown) {
          switch(sizeof(ISA_PACK_INST)) { 
            case 2: inst->fprintf(inst->stream,".half 0x%04x",
		  	          (mUINT32)(mUINT16)inst->inst);
	            break;
	    case 4: inst->fprintf(inst->stream,".word 0x%08x",inst->inst);
	            break;
            case 8:       /* Fall through */
	    default:inst->fprintf(inst->stream,"0x%016llx",
			          (UINT64)(inst->inst));
	            break;
	  }
          return AIR_EVENT_NEXT;
       }
       return AIR_EVENT_END_ERROR;
    }

    for (i = 0; i < Get_AIR_OP_nb_operands(op); i++) {
      tn = Get_AIR_OP_operandn(op,i);
      opnd[i].name = AIR_print_format_opnd(top,1/*is_opnd*/,i,tn,inst->hexaconstant,inst);
      opnd[i].value = r_value(tn);
    }  

    for (i = 0; i < Get_AIR_OP_nb_results(op); i++) {
      tn = Get_AIR_OP_resultn(op,i);
      result[i].name = AIR_print_format_opnd(top,0/*is_opnd*/,i,tn,inst->hexaconstant,inst);
      result[i].value = r_value(tn);
    }

    /* TODO: Error if return value < 0. */
    ISA_PRINT_Inst(inst->fprintf,inst->stream,top,ISA_PRINT_AsmName(top),result,opnd);

    return AIR_EVENT_NEXT;
  }

  int 
  AIR_find_matching_template (ISA_EXEC_UNIT_PROPERTY bundle_props[ISA_MAX_SLOTS],
	  		      int n_bundle_props)
  {
    int t;
    static int template_initialized = 0;
    static ISA_EXEC_UNIT_PROPERTY template_props[ISA_MAX_BUNDLES][ISA_BUNDLE_MAX_SLOTS];
  
    if(!template_initialized) {
      template_initialized=1;
      for(t = 0; t < ISA_MAX_BUNDLES; ++t) {
        int n_props = ISA_EXEC_Slot_Count(t);
        int i;

        for(i = 0; i < n_props; ++i) {
          template_props[t][i] = ISA_EXEC_Slot_Prop(t, i);
        }
      }
    }
    for(t = 0; t < ISA_MAX_BUNDLES; ++t) {
      const ISA_EXEC_UNIT_PROPERTY *t_props = template_props[t];
      int n_t_props = ISA_EXEC_Slot_Count(t);
      if(n_t_props > ISA_MAX_SLOTS) {
        continue;
      }
      if(n_bundle_props == n_t_props
	 && memcmp(bundle_props, t_props,
		   sizeof(ISA_EXEC_UNIT_PROPERTY) * n_bundle_props) == 0) {
        // Perfect match.
        return t;
      }
    }
    return -1;
  }

  unsigned int
  AIR_assemble_binary ( AIR_OP *op, ISA_PACK_INST *pinst )
  {
    unsigned int i;
    unsigned int words;
    long long    result[ISA_OPERAND_max_results];
    long long    opnd[ISA_OPERAND_max_operands];

    for (i = 0; i < Get_AIR_OP_nb_operands(op); i++) {
      long long val = 0;
      AIR_TN *t = Get_AIR_OP_operandn(op, i);

      if (Is_AIR_TN_expression(t)) {
        if (Get_AIR_TN_exp_kind(t) == AIR_Expression_immediate) {
	  ISA_LIT_CLASS lc = ISA_OPERAND_VALTYP_Literal_Class(ISA_OPERAND_INFO_Operand(ISA_OPERAND_Info(Get_AIR_OP_TOP(op)),i));
	  val = Get_AIR_TN_exp_imm_val(t);
	  if (ISA_LC_Is_Negative(lc)) {
	    val = -1 * val;
	  }
	  if (ISA_LC_Ranges(lc) != 1) {
            AIR_fatal_error("Internal","IE_011","Literal class has a number of ranges different than 1\n");
  	  }
	  if(ISA_LC_Scaling_Mask(lc,1) & val) {
            AIR_fatal_error("Internal","IE_011","Underflow on literal. Last bits must be cleared (scaling factor)\n");
 	  }
	  val >>= ISA_LC_Scaling_Value(lc,1);
        } else if (Get_AIR_TN_exp_kind(t) == AIR_Expression_regmask) {
	  val = Get_AIR_TN_exp_regmask_mask(t);
        } else if (Get_AIR_TN_exp_kind(t) == AIR_Expression_reloc) {
	  val = Get_AIR_TN_exp_reloc_val(t);
        } else if (Get_AIR_TN_exp_kind(t) == AIR_Expression_enum) {
	  val = ISA_ECV_Intval(Get_AIR_TN_exp_enum_ecv(t));
        } else {
          AIR_fatal_error("Internal","IE_012","Unknown expression kind\n");
        }
      } else if(Is_AIR_TN_register(t)) {
        val = Get_AIR_TN_reg_regnum(t);
      } else {
          AIR_fatal_error("Internal","IE_013","Unknown operand kind\n");
      }
      opnd[i] = val;
    }

    for (i = 0; i < Get_AIR_OP_nb_results(op); ++i) {
      int val;
      AIR_TN *t = Get_AIR_OP_resultn(op,i);

      if(!Is_AIR_TN_register(t)) {
        AIR_fatal_error("Internal","IE_014","Result is not of register type\n");
      }
      val = Get_AIR_TN_reg_regnum(t);
      result[i] = val;
    }

    words = TI_ASM_Pack_Inst(Get_AIR_OP_TOP(op), result, opnd, pinst);
    if(! (words > 0)) {
      AIR_fatal_error("Internal","IE_015","Instruction encoding failed\n");
    }    

    return words;
  }

#ifdef __cplusplus
}
#endif

