/*
   Copyright (C) 2010 PathScale Inc. All Rights Reserved.
*/
/*
 * Copyright (C) 2007, 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 * Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*


  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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

   Special thanks goes to SGI for their continued support to open source

*/


/*	This is here because the gnu basename() doesn't strip
    off multiple slashes.
*/

#define __STDC_LIMIT_MACROS             // Must be defined to get UINT16_MAX from stdint.h.
#include <stdint.h>

#include <fcntl.h>
#include <libgen.h>

#include "defs.h"
#include "ipc_defs.h"
#include "linker.h"  

#include "pu_info.h"
#include "ipc_symtab_merge.h"

#include "ipc_compile_helper.h"


const char* ipa_basename(char *name){
    const char *bname = basename(name);
    
    while (*bname == '/')
    	bname++;
    return bname;
}



const char* ipc_abi()
{
#ifdef TARG_MIPS
    // 12343: Use IPA_Target_Type instead of ld_ipa_opt[LD_IPA_TARGOS].flag
    // to distinguish between n32 and 64 in IPA
    return IPA_Target_Type == IP_64_bit_ABI ? "-64" : "-n32";
#endif

#ifdef TARG_IA64
    return "-i64";
#endif

#ifdef _TARG_IA32
    return "-i32";
#endif

#ifdef TARG_X8664
    return IPA_Target_Type == IP_64_bit_ABI ? "-m64" : "-m32";
#endif

  return "-n32";                // This line is never reached.
}



// Returns true if path refers to an ordinary file.
bool ipc_file_exists(const char* path)
{
  if (!path || strlen(path) == 0)
    return false;

  struct stat buf;
  return stat(path, &buf) == 0 && S_ISREG(buf.st_mode);
}

bool ipc_looks_like(const char *path, const char *base)
{
    int p = strlen(path), b = strlen(base);

    return (strcmp(path + p - b, base) == 0 &&
	    (path[p - b - 1] == '-' || path[p - b - 1] == '/'));
}

// temp. kludge:  ipacom_process_file should really take
// const IP_FILE_HDR& as argument instead of const PU_Info *
const IP_FILE_HDR&
ipc_get_ip_file_hdr (const PU_Info *pu)
{
  ST_IDX st_idx = PU_Info_proc_sym (pu);
  PU_IDX pu_idx = ST_pu (St_Table[st_idx]);
  return *AUX_PU_file_hdr (Aux_Pu_Table[pu_idx]);
}


char* ipc_copy_of (char *str)
{
  register int len;
  register char *p;

  len = strlen(str) + 1;
  p = (char *) MALLOC (len);
  MALLOC_ASSERT (p);
  memcpy(p, str, len);
  return p;
} /* ipc_copy_of */


// Helper function for get_extra_args.
void ipc_escape_char (char *str)
{
  char *p = str + 1;

  do {
    *str++ = *p++;
  } while (*str != 0);
} /* escape_char */


extern "C" {
// Allow ipa_link to call the error routines.

void
Ipalink_Set_Error_Phase (char *name)
{
  Set_Error_Phase (name);
}

void
Ipalink_ErrMsg_EC_infile (char *name)
{ 
  ErrMsg(EC_Ipa_Infile, name);
}

void
Ipalink_ErrMsg_EC_outfile (char *name)
{ 
  ErrMsg(EC_Ipa_Outfile, name);
}

}	// extern "C"
