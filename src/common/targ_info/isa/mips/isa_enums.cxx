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



//
// Generate a list of enumeration classes and values for the ISA.
/////////////////////////////////////////////////////////

#include <stddef.h>
#include "isa_enums_gen.h"

main ()
{
  ISA_Enums_Begin();

  ISA_Create_Enum_Class ("pfhint",
      "load",		 0,
      "store",		 1,
      "load_streamed",	 4,
      "store_streamed",	 5,
      "load_retained",	 6,
      "store_retained",	 7,
      "nudge_L1",	24,
      "nudge",		25,
      "load_to_L2",	28,
      "store_to_L2",	29,
      "prepare_store",	30,
      NULL,	UNDEFINED);	// default value

  ISA_Create_Enum_Class ("perfctlcode",
      "stop",		0,
      "start",		1,
      NULL,	UNDEFINED);	// default value

  ISA_Enums_End();
}

