/*

  Copyright (C) 2001 ST Microelectronics, Inc.  All Rights Reserved.

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

  Contact information:  or

  http://www.st.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


// TCON Utilities

#include <alloca.h>
#include "defs.h"
#include "mempool.h"
#include "symtab.h"
#include "tcon_util.h"

/* ====================================================================
 *   On the ST100, the .rodata items can not be accessed through
 *   a relocation or a dedicated pointer (like gp). Each constant
 *   in the .rodata section must be given a label and accessed
 *   through such label. It's a pain, but the processor is designed
 *   this way and crippled with the help of "experts" from GHS.
 * ====================================================================
 */

// Map TCON to a symbolic name:
typedef struct {
  char sym[20];
} tcon_info;

static tcon_info *tcon_info_array = NULL;
static INT32 tcon_info_array_size;
static INT32 current_idx;

/* ====================================================================
 *   Init_Tcon_Info ()
 * ====================================================================
 */
void
Init_Tcon_Info ()
{
  INT tcon_count = TCON_Table_Size() + 1;

  if (tcon_info_array == NULL) {
    //    tcon_count = MAX(8, tcon_count);
    // is supposed to clear out the values ?
    tcon_info_array = 
             (tcon_info*) Src_Alloc (sizeof(tcon_info) * tcon_count);
    bzero (tcon_info_array, sizeof(tcon_info) * tcon_count);
  }
  else {
    FmtAssert(FALSE,("tcon_info_array is not NULL ?"));
  }

  tcon_info_array_size = tcon_count;
  current_idx = 0;

  return;
}

/* ====================================================================
 *   Fini_Tcon_Info ()
 * ====================================================================
 */
void
Fini_Tcon_Info ()
{
  // should deallocate space ... ??

  tcon_info_array = NULL;
  tcon_info_array_size = 0;
  current_idx = 0;

  return;
}

/* ====================================================================
 *   make_name_for_tcon
 *
 *   This needs to be done for some symbols, eg. those kept in the
 *   .rodata. The .lai syntax does not allow accessing them through
 *   section name (making address like so): 
 *       addba a, gp, .rodata
 *       law   data, @(a + offset)
 *   Instead I must do:
 *       law data, @(LABEL + offset)
 *
 *   or must I ?
 * ====================================================================
 */
static void
make_name_for_tcon (
  TCON_IDX tcon
)
{
  Is_True(tcon > 0 && tcon < tcon_info_array_size,
	    ("TCON IDX %d out of range (%d)", tcon, tcon_info_array_size));

  char *buf = tcon_info_array[tcon].sym;

  sprintf(buf,"UNNAMED_CONST_%d", current_idx++);
  return;
}

/* ====================================================================
 *   Get_TCON_name 
 * ====================================================================
 */
char *
Get_TCON_name (
  TCON_IDX tcon
)
{
#if 0
  // Are any TCONs created in cgemit ??
  if (tcon >= current_idx) {
    // overflowed
    Realloc_Tcon_Info (i);
  }
#endif

  Is_True(tcon > 0 && tcon < tcon_info_array_size,
	    ("TCON IDX %d out of range (%d)", tcon, tcon_info_array_size));

  if (strncmp(tcon_info_array[tcon].sym, "UNNAMED_CONST", 13)) {
    make_name_for_tcon (tcon);
  }

  return tcon_info_array[tcon].sym;
}
