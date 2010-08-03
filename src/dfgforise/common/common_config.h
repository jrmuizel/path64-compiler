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

/**
 * \file common_config.h
 */


#ifndef common_config_h
#define common_config_h



namespace DfgForIse {



  /** Default value for the bit size of the operand / results and operations.
   */
  const unsigned int DEFAULT_BIT_SIZE = 32;


  /** Default value for the frequency of the basic blocks.
   */
  const unsigned int DEFAULT_BASIC_BLOCK_FREQUENCY = 0;


  /** Default value for the clock period of the target processor
   *  (in nanosecond.10-2).
   */
  const unsigned int DEFAULT_CLOCK_PERIOD = 357;


  /** Indicates an estimation of the fraction of cycles lost by a changement
   *  of cycle in a macro-instruction.
   */
  const float CYCLE_COST_CHANGEMENT_CYCLE = 1/4;



};

#endif // common_config_h
