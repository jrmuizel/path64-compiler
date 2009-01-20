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

#ifndef __AIR_DISASM_H__
#define __AIR_DISASM_H__

#include <stdio.h>
#include <stdlib.h>
#include "air.h"

#include "topcode.h"
#include "targ_isa_decode.h"
#include "targ_isa_pack.h"

/* AIR is strictly in C */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct{

   INT                  unit;
   ISA_DECODE_INST      decodeinst;
   ISA_PACK_INST        inst;
   AIR_OP              *air_inst;
   AIR_String           string;

   void                *stream;
   int                (*fprintf)(void*,const char*,...); 
   unsigned long long   PC;
   const char *       (*get_symbol_at_address)(unsigned long long addr, void * info);
   void                *disasm_info;
   unsigned int         print_unknown;
}AIR_Disasm;

/* Functions prototypes ......................................*/
extern int AIR_disasm_init(void);
extern int AIR_disasm_end(void);
extern int AIR_disasm_run( AIR_Disasm*);


#ifdef __cplusplus
}
#endif

#endif  /* __AIR_DISASM_H_ */
