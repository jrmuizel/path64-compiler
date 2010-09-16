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

#ifndef IPC_COMPILE_HELPER_INCLUDED
#define IPC_COMPILE_HELPER_INCLUDED


/*	This is here because the gnu basename() doesn't strip
    off multiple slashes.
*/
const char* ipa_basename(char *name);

const char* ipc_abi();

// Returns true if path refers to an ordinary file.
bool ipc_file_exists(const char* path);

bool ipc_looks_like(const char *path, const char *base);

const IP_FILE_HDR& ipc_get_ip_file_hdr (const PU_Info *pu);


char* ipc_copy_of (char *str);


// Helper function for get_extra_args.
void ipc_escape_char (char *str);

#endif //IPC_COMPILE_HELPER_INCLUDED
