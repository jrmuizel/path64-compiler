/*
  Copyright (C) 2005, STMicroelectronics, All Rights Reserved.

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


/* ====================================================================
 * ====================================================================
 *
 * Module: ipra.h
 *
 * Description:
 *
 *   Class for managing inter-procedural register allocation data.
 *
 * Types:
 *   IPRA_INFO encapsulates information saved for inter-procedural
 *   register allocation
 *
 * Methods:
 *
 *   IPRA () - constructor
 *   Note_Used_Registers (pu) - save information about the current PU
 *                              for later retrieval.
 *
 *   Set_Return_Alignment (pu) - save information on alignment of
 *                               returned value
 *
 *   Get_Info (pu) - Return stored info for pu
 *
 *   
 * ====================================================================
 * ====================================================================
 */

#ifndef IPRA_H_INCLUDED
#define IPRA_H_INCLUDED

#include "cxx_hash.h"

typedef struct ipra_info {
#ifdef TARG_ST
  REGISTER_SET used_regs[ISA_REGISTER_CLASS_MAX_LIMIT+1];
  // FdF ipa-align
  int alignment;
#else
  REGISTER_SET used_regs[ISA_REGISTER_CLASS_MAX+1];
#endif
} *IPRA_INFO;

class IPRA {
 private:
  typedef HASH_TABLE<const ST *,IPRA_INFO> ST_TO_IPRA_INFO_MAP;
  ST_TO_IPRA_INFO_MAP *st_to_ipra_info_map;
  IPRA_INFO Give_Info (const ST *pu);
 public:
  IPRA ();
  void Note_Used_Registers (const ST *pu);
  void Set_Return_Alignment(const ST *pu, int alignment);
  IPRA_INFO Get_Info (const ST *pu);
};

#endif /* IPRA_H_INCLUDED */
