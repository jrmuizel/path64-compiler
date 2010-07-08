/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Copyright (C) STMicroelectronics All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

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
