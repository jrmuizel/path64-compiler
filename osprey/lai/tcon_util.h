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

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.st.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/

/* ====================================================================
 *   TCON Utilities
 *
 *   TCON utilities module supports mapping of TCONs to their
 *   symbolic names. Such mapping is necessary for the processors
 *   that do not grant a relocated access to the constant holding
 *   sections (eg. .rodata, etc.). These symbolic names are only
 *   used for lai/assembler emission purposes. It should work like
 *   so:
 *
 *     1. The CGIR OPs refer to TNs representing TCONs;
 *     2. When an OP referencing a TCON is emitted into a
 *        lai/assembly file, TCON's symbolic name is used
 *        to replace the TCON TN;
 *     3. When TCONs are emitted into the .rodata, etc.
 *        section, each TCON has a label emitted with its
 *        symbolic name.
 *
 *   NOTE: This file should only be included by the lai/cgexp.h
 * ====================================================================
 */

extern void Init_Tcon_Info ();
extern void Fini_Tcon_Info ();

extern char *Get_TCON_name (TCON_IDX tcon);
