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

/* AIR stands for Assembly intermediate representation */
/* This file is the central source file for AIR        */

#ifndef __AIR_LOADER_CXX__
#define __AIR_LOADER_CXX__

#ifdef BINUTILS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "air_loader.h"
#include "dll_loader.h"
#include "isa_loader.h"

  char * Extension_Names = NULL;

  /* Connect extension list 
  */
extern "C" int
  AIR_connect_extensions( char ** extensions, int ext_nb )
  {
    int ext;
    int length;
    
    if(0==ext_nb) return TRUE;
    
    for(ext=0,length=0;ext<ext_nb;ext++) 
    {
    length+=strlen(extensions[ext])+2;
    }
    Extension_Names = (char*)calloc(length,sizeof(char));
    for(ext=0;ext<ext_nb;ext++) 
    {
	if (0!=ext) strcat(Extension_Names,",");
	strcat(Extension_Names,extensions[ext]);
    }
    
    if(!Load_Extension_dll_handlers(FALSE)) return FALSE;

    if (!Load_Lai_Loader_Info()) return FALSE;

    return TRUE;
  }

char *SBar =
"-----------------------------------------------------------------------\n";
char *DBar =
"=======================================================================\n";
char *Sharps =
"#######################################################################\n";

void
Fail_FmtAssertion ( const char *fmt, ... )
{
}
void Abort_Compiler_Location (
  char * file_name,
  INT    line_number )
{
}
void
DevWarn( const char *fmt, ... )
{
}
void
ErrMsg ( INT ecode, ... )
{
}
#endif				/* BINUTILS */


#endif                               /* __AIR_LOADER_CXX__ */
