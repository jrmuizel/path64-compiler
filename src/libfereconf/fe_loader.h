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


#ifndef fe_loader_INCLUDED
#define fe_loader_INCLUDED

/* This file is in charge of providing extension sepcific feature */
/* Define the dynamique count: to be place in the loader section*/

#ifdef __cplusplus
extern "C" {
#endif
  extern int *Map_Reg_To_Preg;
  extern int Map_Reg_To_Preg_Size;

  void WFE_Loader_Initialize_Register (void);
  /* Loader initialization */
  extern void WFE_Init_Loader(void);

#ifdef __cplusplus
}
#endif

#endif /* fe_loader_INCLUDED */
