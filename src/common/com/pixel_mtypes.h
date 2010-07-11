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
/*
 * Array and API used for pixel type.
 */

#include "mtypes.h"

/* Table that stores the pixel size for each mtypes. 0 means not a
 * pixel type
 */
BE_EXPORTED extern mUINT8 pixel_size_per_type[MTYPE_MAX_LIMIT+1];

/* Get pixel size for mtype of rank n */
#define MTYPE_pixel_size(n) ( pixel_size_per_type[n] )
