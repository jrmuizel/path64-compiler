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

/* Standard C header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Targinfo header files */
#include "topcode.h"
#include "targ_isa_print.h"
#include "targ_abi_properties.h"
#include "ti_asm.h"

/* Libair header files */
#include "air.h"
#include "air_disasm.h"

#include "targ_air.h"

/* AIR is strictly in C */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    int  (*decode_preprocess)(AIR_Decode*);
    int  (*decode_process)(AIR_Decode*);       /* Mandatory - can't be NULL */
    int  (*decode_postprocess)(AIR_Decode*);

    int  (*print_preprocess)(AIR_Print*);
    int  (*print_process)(AIR_Print*);         /* Mandatory - can't be NULL */
    int  (*print_postprocess)(AIR_Print*);

} Disasm_Struct_Run;

/* We implement here a bfd-like mechanism
 * The various fields are initialized with generic
 * routines. However target specific routines
 * can be used by defining appropriate #define.
 */
#ifndef DISASM_DECODE_pre_process_fct
#define DISASM_DECODE_pre_process_fct   NULL
#endif
#ifndef DISASM_DECODE_process_fct
#define DISASM_DECODE_process_fct       AIR_decode_service   
#endif
#ifndef DISASM_DECODE_post_process_fct
#define DISASM_DECODE_post_process_fct  NULL
#endif

#ifndef DISASM_DECODE_pre_print_fct     
#define DISASM_DECODE_pre_print_fct     NULL
#endif
#ifndef DISASM_DECODE_print_fct
#define DISASM_DECODE_print_fct         AIR_print_service
#endif
#ifndef DISASM_DECODE_post_print_fct
#define DISASM_DECODE_post_print_fct    NULL 
#endif

static Disasm_Struct_Run disasm_run = {
    DISASM_DECODE_pre_process_fct,
    DISASM_DECODE_process_fct,
    DISASM_DECODE_post_process_fct,

    DISASM_DECODE_pre_print_fct,
    DISASM_DECODE_print_fct,
    DISASM_DECODE_post_print_fct,
};

int
AIR_disasm_run( AIR_Disasm *inst )
{
   AIR_Decode decode;
   AIR_Print  print;
   int        print_return;
   
   /* TODO: manage error cases */

   /* initialization of structure decode */
   decode.unit     = inst->unit;
   decode.inst     = inst->inst;
   decode.air_inst = NULL;

   if(disasm_run.decode_preprocess)
     disasm_run.decode_preprocess(&decode);

   disasm_run.decode_process(&decode);

   if(disasm_run.decode_postprocess)
     disasm_run.decode_postprocess(&decode);

   /* initialization of structure print */
   print.air_inst              = decode.air_inst;
   print.inst                  = inst->inst;
   print.stream                = inst->stream;
   print.fprintf               = inst->fprintf;
   print.hexaconstant          = 1;
   print.PC                    = inst->PC;
   print.get_symbol_at_address = inst->get_symbol_at_address;
   print.disasm_info           = inst->disasm_info;
   print.print_unknown         = inst->print_unknown;
   
   AIR_reset_tns_flags(print.air_inst);

   if(disasm_run.print_preprocess)
     disasm_run.print_preprocess(&print);

   print_return = disasm_run.print_process(&print);

   if(disasm_run.print_postprocess)
     disasm_run.print_postprocess(&print);
   
   return print_return;
}

int
AIR_disasm_init( void )
{
    ABI_PROPERTIES_ABI_Value = ABI_PROPERTIES_ABI_embedded;
    ABI_PROPERTIES_Initialize();
    AIR_init();
    AIR_target_init();
    return 0;
}

int
AIR_disasm_end( void )
{
    AIR_target_end();
    AIR_end();
    return 0;
}

#ifdef __cplusplus
}
#endif

