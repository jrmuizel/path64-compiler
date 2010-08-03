/* 

  Copyright (C) 2008 ST Microelectronics, Inc.  All Rights Reserved. 

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
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
////                                                                             ////
////    Declaration of ExportFromBackEnd functions.                              ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/**
 * \file ExportFromBackEnd.h
 */

#include "defs.h"
#include "glob.h"
#include "bb.h"


#ifndef ExportFromBackEnd_h
#define ExportFromBackEnd_h


namespace DfgForIse {

  /** Function which initializes the exportation to DFGs.
   * Called from CG_Init.
   */
  extern void ExportFromBackEnd_Initialize();


  /** Function which finalizes the exportation to DFGs, generally dump
   *  the dfgs in a graphml file.
   * Called from CG_Fini.
   */
  extern void ExportFromBackEnd_Finalize();


  /** Function which initializes the exportation to DFGs for a PU.
   * Called from CG_PU_Initialize.
   */
  extern void ExportFromBackEnd_PU_Initialize();


  /** Function which finalizes the exportation to DFGs for a PU.
   * Called from CG_PU_Finalize.
   */
  extern void ExportFromBackEnd_PU_Finalize();


  /** Function which performs the exportation to DFGs for Instruction Set
   *  Identification.
   * Called from CG_Generate_Code, between any optimization phase.
   * Needs to have frequencies (feedback or statical profiling) computed for BBs.
   */
  extern void ExportFromBackEndForIse();


  /** Function which performs the exportation to DFGs for Debug and Code
   * Analysis.
   * Called from CG_Generate_Code, between any optimization phase.
   * @param phase INT indicating the phase after which we're printing.
   * @param pname char* indicating the name to print for phase.
   * @param cur_bb BB indicating the basic block to limit traces to.
   */
  extern void ExportFromBackEndForDebug(INT phase, const char *pname, BB *cur_bb);

};

#endif // ExportFromBackEnd_h
