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
