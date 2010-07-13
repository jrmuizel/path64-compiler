/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/

#ifndef defs_exported_INCLUDED
#define defs_exported_INCLUDED
#include <inttypes.h>
/* ====================================================================
 *
 * Type mapping
 *
 * The following type names are to be used in general to avoid host
 * dependencies.  Two sets are provided.  The first, without a prefix,
 * specifies a minimum bit length for the object being defined of 8,
 * 16, or 32 bits.  It is to be interpreted as a host-efficient type
 * of at least that size.  The second, with a "m" prefix, also gives
 * a minimum bit length, but that bit length is preferred (minimizing
 * memory usage is the priority) if remotely reasonable on the host.
 * The latter types should be used only for objects which will be
 * replicated extensively.
 *
 * ====================================================================
 */

#if (defined(HOST_SGI) || defined(__GNUC__) || defined(_MSC_VER))
typedef signed int	INT;	/* The natural integer on the host */
typedef signed int	INT8;	/* Use the natural integer FIXME: Not portable */
typedef signed int 	INT16;	/* Use the natural integer - FIXME: This is not correct or portable */
typedef int32_t 	INT32;	/* The natural integer matches */
typedef int64_t 	INT64;	
typedef uintptr_t 	INTPTR;	/* Integer the same size as pointer*/
typedef unsigned int	UINT;	/* The natural integer on the host */
typedef unsigned int 	UINT8;	/* Use the natural integer FIXME: Not portable */
typedef unsigned int	UINT16;	/* Use the natural integer */
typedef uint32_t	UINT32;	/* The natural integer matches */
typedef uint64_t	UINT64;
typedef int		BOOL;	/* Natural size Boolean value... which should be bool, but was defined int a long time ago */ 
typedef int8_t		mINT8;	/* Avoid - often very inefficient */
typedef int16_t		mINT16;	/* Use a 16-bit integer */
typedef int32_t		mINT32;	/* The natural integer matches */
typedef int64_t		mINT64;
typedef uint8_t		mUINT8;	/* Use the natural integer */
typedef uint16_t	mUINT16;/* Use a 16-bit integer */
typedef uint32_t	mUINT32;/* The natural integer matches */
typedef uint64_t	mUINT64;
typedef unsigned char	mBOOL;	/* Minimal size Boolean value */

/* Define largest efficient integers for the host machine: */
typedef signed long	INTSC;	/* Scaled integer */
typedef unsigned long	UINTSC;	/* Scaled integer */

/* Define pointer-sized integers for the host machine: */
typedef intptr_t	INTPS;	/* Pointer-sized integer */
typedef uintptr_t	UINTPS;	/* Pointer-sized integer */

#define	INTSC_MAX	INT32_MAX	/* Max scaled int */
#define	INTSC_MIN	INT32_MIN	/* Min scaled int */
#define	UINTSC_MAX	UINT32_MAX	/* Max scaled uint */
#define	INTPS_MAX	INT32_MAX	/* Max pointer-sized int */
#define	INTPS_MIN	INT32_MIN	/* Min pointer-sized int */
#define	UINTPS_MAX	UINT32_MAX	/* Max pointer-sized uint */

/* Define quad-precision floating point for the host machine.
 * WARNING: Depending on the host, this type need not be usable.
 * Instead, see QUAD_TYPE in targ_const.h and its reference to
 * HOST_SUPPORTS_QUAD_FLOAT in config_host.h.  We do this this way to
 * avoid needing to allow the standard type names whenever
 * targ_const.h is included.
 */
#if (defined(_COMPILER_VERSION) && (_COMPILER_VERSION >= 400) && _SGIAPI) || (defined(__GNUC__) && !defined(TARG_MIPS))
#define HOST_SUPPORTS_QUAD_FLOAT 1
#else
#define HOST_SUPPORTS_QUAD_FLOAT 0
#endif

#if HOST_SUPPORTS_QUAD_FLOAT
/* Temporarily remove this to get rid of warnings: */
typedef long double	QUADFP;		/* 128-bit floating point */
#else 
typedef double	QUADFP;		/* 128-bit floating point */
#endif


#endif /* HOST_SGI || __GNUC__ || _MSC_VER */

/* We would like a generic memory pointer type, e.g. for use in the
 * memory allocation routines.  Ideally, it is (void *), but some
 * hosts, e.g. ca. 1988 MIPS, can't handle that...
 */
#if(defined(HOST_SGI) || defined(__GNUC__) || defined(_MSC_VER))
typedef void	*MEM_PTR;
#endif /* HOST_SGI || __GNUC__ || _MSC_VER */

/* Short hand for those who don't like "char *" */
typedef char *STRING;

/* Define standard values: */
#ifndef TRUE
#define TRUE	((BOOL) 1)
#endif
#ifndef FALSE
#define FALSE	((BOOL) 0)
#endif

/* Something to use to represent undefined positive integers.  Perahps we
 * could generalize this somehow, but it is useful the way it is.
 */
#ifndef UNDEFINED
#define UNDEFINED -1
#endif

/* Finally, eliminate the standard type names to prevent their
 * accidental use.  We must, however, allow this to be overridden for
 * files which need them, e.g. to use /usr/include expansions which
 * require them.
 */
#if 0 /* !defined(USE_STANDARD_TYPES) && !defined(_NEW_SYMTAB) */
# define short	SYNTAX_ERROR_short
# define int	SYNTAX_ERROR_int
# define long	SYNTAX_ERROR_long
#endif /* USE_STANDARD_TYPES */

/* Define the general TDT table index type: */
typedef UINT16	CLASS_INDEX;	/* Individual objects */
typedef mUINT16	mCLASS_INDEX;	/* Table components */

/* Define the PREG offset type: */
typedef INT32 PREG_NUM;		/* Individual objects */
typedef mINT32 mPREG_NUM;	/* Table components */

/* Define the IDTYPE used by wopt */
typedef INTPTR IDTYPE;

#endif /* defs_exported_INCLUDED */
