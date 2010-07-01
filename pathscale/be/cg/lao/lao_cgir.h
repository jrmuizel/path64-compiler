/*
  Copyright (C) 2002-2009, STMicroelectronics, All Rights Reserved.

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
*/

#ifndef lao_cgir_INCLUDED
#define lao_cgir_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------- Init/Fini --------------------------------*/
/* Need to be called only once per process execution. */
extern void CGIR_LAO_Init(void);
extern void CGIR_LAO_Fini(void);

/*-------------------- CGIR -> LIR Conversions ------------------*/
extern Operator CGIR_TOP_to_Operator(TOP top);
extern Operator CGIR_TOP_to_Operator(TOP top);
extern Register CGIR_CRP_to_Register(CLASS_REG_PAIR crp);
extern RegFile CGIR_IRC_to_RegFile(ISA_REGISTER_CLASS irc);
extern Immediate CGIR_LC_to_Immediate(ISA_LIT_CLASS ilc);
extern Processor CGIR_IS_to_Processor(ISA_SUBSET is);

/*-------------------- LIR -> CGIR Conversions ------------------*/
extern ISA_REGISTER_CLASS RegFile_to_CGIR_IRC(RegFile regClass);
extern CLASS_REG_PAIR Register_to_CGIR_CRP(Register registre);
extern TOP Operator_to_CGIR_TOP(Operator lir_operator);
extern TYPE_ID NativeType_to_CGIR_TYPE_ID(NativeType lir_nativeType);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* !lao_cgir_INCLUDED */
