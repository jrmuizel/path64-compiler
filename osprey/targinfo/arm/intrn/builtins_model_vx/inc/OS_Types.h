
#ifndef OS_Types_h
#define OS_Types_h

/*{{{  About OS_Types.h */
/**
 *
 * 1) OS-retargetted typedefs of signed and unsigned integer types of 8, 16, 32 and 64 bits size:
 *
 *   - uint8, uint16, uint32, uint64
 *
 *   - sint8, sint16, sint32, sint64
 *
 *
 *
 * 2) OS-retargetted min and max values for types defined in 1)
 *
 *   - UINT8_MIN, UINT16_MIN, UINT32_MIN and UINT64_MIN
 *
 *   - UINT8_MAX, UINT16_MAX, UINT32_MAX and UINT64_MAX
 *
 *   - SINT8_MIN, SINT16_MIN, SINT32_MIN and SINT64_MIN
 *
 *   - SINT8_MAX, SINT16_MAX, SINT32_MAX and SINT64_MAX
 *
 *
 *
 * 2) OS-retargetted constant setting (mainly point about 64 bits constants)
 *
 *   - SINT8_C(x), SINT16_C(x), SINT32_C(x) and SINT64_C(x)
 *     to define 8, 16, 32 and 64 bits signed constants
 *
 *   - UINT8_C(x), UINT16_C(x), UINT32_C(x) and UINT64_C(x)
 *     to define 8, 16, 32 and 64 bits unsigned constants
 *
 *   - Example :
 *
 *       uint64 x = UINT64(0xDDEEAADDBBEEEEFF);
 *
 *     Will be develop for windows Visual C++ as:
 *
 *       unsigned __int64 x = ( ( unsigned __int64 )( 0xDDEEAADDBBEEEEFF ) );
 *
 *     And will be develop for SunOS / gcc as:
 *
 *       unsigned long long int x = 0xDDEEAADDBBEEEEFFULL;
 *
 *
 *
 * 3) definition of macros to hide OS differences about printf( "%" ) format specifications.
 *
 *   - SINT_CFORMAT(f) SINT8_CFORMAT(f), SINT16_CFORMAT(f), SINT32_CFORMAT(f) and SINT64_CFORMAT(f)
 *     to print default, 8, 16, 32 and 64 bits signed integers as decimal signed values with sign prefix.
 *
 *   - UINT_CFORMAT(f) UINT8_CFORMAT(f), UINT16_CFORMAT(f), UINT32_CFORMAT(f) and UINT64_CFORMAT(f)
 *     to print default, 8, 16, 32 and 64 bits unsigned integers as decimal unsigned values (no prefix).
 *
 *   - XINT_CFORMAT(f) XINT8_CFORMAT(f), XINT16_CFORMAT(f), XINT32_CFORMAT(f) and XINT64_CFORMAT(f)
 *     to print default, 8, 16, 32 and 64 bits integers in hexadecimal with "0x" prefix.
 *
 *   - OINT_CFORMAT(f) OINT8_CFORMAT(f), OINT16_CFORMAT(f), OINT32_CFORMAT(f) and OINT64_CFORMAT(f)
 *     to print default, 8, 16, 32 and 64 bits integers in octal with "0" prefix.
 *
 *   - INT_CFORMAT(f) INT8_CFORMAT(f), INT16_CFORMAT(f), INT32_CFORMAT(f) and INT64_CFORMAT(f)
 *     to print default, 8, 16, 32 and 64 bits integers as decimal values with sign prefix when negative only.
 *
 *   - STRING_CFORMAT(f)
 *     to print zero-ended 8-bits character strings.
 *
 *   - POINTER_CFORMAT(f)
 *     to print host pointer values.
 *
 *   - Example :
 *
 *       sint64 x = -33;
 *       printf( "Variable " STRING_CFORMAT("-4")
 *               "at "       POINTER_CFORMAT("")
 *               " = "       XINT64_CFORMAT("016")
 *               " : "       SINT64_CFORMAT("")
 *               "\n",
 *               "x", &x, x );
 *
 *     Will be developped for windows as:
 *
 *       unsigned __int64 x;
 *       printf( "Variable " "%-4s"
 *               "at "       "0x%x"
 *               " = "       "0x%016I64x"
 *               " : "       "%+d"
 *               "\n",
 *               "x", &x, x, x );
 *
 *
 *     and for Linux systems as:
 *
 *       unsigned long long x;
 *       printf( "Variable " "%-4s"
 *               "at "       "%p"
 *               " = "       "0x%016llx"
 *               " : "       "%+d"
 *               "\n",
 *               "x", &x, x, x );
 *
 */
/*}}}*/

/*{{{ SunOS / gcc / 32 bits systems */
#ifdef __sun

	#include <limits.h>
	#include <inttypes.h>

	/*{{{  Basic types */

	typedef unsigned char          uint8;
	typedef unsigned short int     uint16;
	typedef unsigned long int      uint32;
	typedef unsigned long long int uint64;

	typedef   signed char          sint8;
	typedef   signed short int     sint16;
	typedef   signed long int      sint32;
	typedef   signed long long int sint64;

	/*}}}*/

	/*{{{  Limits */

	#define UINT8_MIN  0
	#define UINT16_MIN 0
	#define UINT32_MIN 0
	#define UINT64_MIN 0

	// #define UINT8_MAX  UCHAR_MAX  // already defined in int_limits.h included by inttypes.h
	// #define UINT16_MAX USHRT_MAX  // already defined in int_limits.h included by inttypes.h
	// #define UINT32_MAX ULONG_MAX  // already defined in int_limits.h included by inttypes.h
	// #define UINT64_MAX ULLONG_MAX // already defined in int_limits.h included by inttypes.h

	#define SINT8_MIN  INT8_MIN
	#define SINT16_MIN INT16_MIN
	#define SINT32_MIN INT32_MIN
	#define SINT64_MIN INT64_MIN

	#define SINT8_MAX  INT8_MAX
	#define SINT16_MAX INT16_MAX
	#define SINT32_MAX INT32_MAX
	#define SINT64_MAX INT64_MAX

	/*}}}*/

	/*{{{  Constant builders */

	// #define UINT8_C(c)  UINT8_C(c)  // already defined in int_const.h included by inttypes.h
	// #define UINT16_C(c) UINT16_C(c) // already defined in int_const.h included by inttypes.h
	// #define UINT32_C(c) UINT32_C(c) // already defined in int_const.h included by inttypes.h
	// #define UINT64_C(c) UINT64_C(c) // already defined in int_const.h included by inttypes.h

	#define SINT8_C(c)  INT8_C(c)
	#define SINT16_C(c) INT16_C(c)
	#define SINT32_C(c) INT32_C(c)
	#define SINT64_C(c) INT64_C(c)

	/*}}}*/

	/*{{{  C printf format strings */

	#define STRING_CFORMAT(f)  "%" f "s"

	#define POINTER_CFORMAT(f)  "0x%" f "p"

	#define INT8_CFORMAT(f)   "%" f "d"
	#define INT16_CFORMAT(f)  "%" f "hd"
	#define INT32_CFORMAT(f)  "%" f "ld"
	#define INT64_CFORMAT(f)  "%" f "lld"
	#define INT_CFORMAT(f)    "%" f "d"

	#define UINT8_CFORMAT(f)  "%" f "u"
	#define UINT16_CFORMAT(f) "%" f "hu"
	#define UINT32_CFORMAT(f) "%" f "lu"
	#define UINT64_CFORMAT(f) "%" f "llu"
	#define UINT_CFORMAT(f)   "%" f "u"

	#define SINT8_CFORMAT(f)  "%+" f "d"
	#define SINT16_CFORMAT(f) "%+" f "hd"
	#define SINT32_CFORMAT(f) "%+" f "ld"
	#define SINT64_CFORMAT(f) "%+" f "lld"
	#define SINT_CFORMAT(f)   "%+" f "d"

	#define XINT8_CFORMAT(f)  "0x%" f "x"
	#define XINT16_CFORMAT(f) "0x%" f "hx"
	#define XINT32_CFORMAT(f) "0x%" f "lx"
	#define XINT64_CFORMAT(f) "0x%" f "llx"
	#define XINT_CFORMAT(f)   "0x%" f "x"

	#define OINT8_CFORMAT(f)  "0%" f "o"
	#define OINT16_CFORMAT(f) "0%" f "ho"
	#define OINT32_CFORMAT(f) "0%" f "lo"
	#define OINT64_CFORMAT(f) "0%" f "llo"
	#define OINT_CFORMAT(f)   "0%" f "o"

	/*}}}*/

#endif
/*}}}*/

/*{{{ Linux / gcc / 32 bits systems */
#ifdef __linux

	#define __STDC_CONSTANT_MACROS 1
	#define __STDC_LIMIT_MACROS 1

	#include <limits.h>
	#include <stdint.h>

	/*{{{  Basic types */

	typedef unsigned char          uint8;
	typedef unsigned short int     uint16;
	typedef unsigned long int      uint32;
	typedef unsigned long long int uint64;
	typedef unsigned int           uint;

	typedef   signed char          sint8;
	typedef   signed short int     sint16;
	typedef   signed long int      sint32;
	typedef   signed long long int sint64;
	typedef   signed int           sint;

	/*}}}*/

	/*{{{  Limits */

	#define UINT8_MIN  0
	#define UINT16_MIN 0
	#define UINT32_MIN 0
	#define UINT64_MIN 0

	// #define UINT8_MAX  UCHAR_MAX  // already defined in stdint.h
	// #define UINT16_MAX USHRT_MAX  // already defined in stdint.h
	// #define UINT32_MAX ULONG_MAX  // already defined in stdint.h
	// #define UINT64_MAX ULLONG_MAX // already defined in stdint.h

	#define SINT8_MIN  INT8_MIN
	#define SINT16_MIN INT16_MIN
	#define SINT32_MIN INT32_MIN
	#define SINT64_MIN INT64_MIN

	#define SINT8_MAX  INT8_MAX
	#define SINT16_MAX INT16_MAX
	#define SINT32_MAX INT32_MAX
	#define SINT64_MAX INT64_MAX

	/*}}}*/

	/*{{{  Constant builders */

	// #define UINT8_C(c)  UINT8_C(c)  // already defined in stdint.h
	// #define UINT16_C(c) UINT16_C(c) // already defined in stdint.h
	// #define UINT32_C(c) UINT32_C(c) // already defined in stdint.h
	// #define UINT64_C(c) UINT64_C(c) // already defined in stdint.h

	#define SINT8_C(c)  INT8_C(c)
	#define SINT16_C(c) INT16_C(c)
	#define SINT32_C(c) INT32_C(c)
	#define SINT64_C(c) INT64_C(c)

	/*}}}*/

	/*{{{  C printf format strings */

	#define STRING_CFORMAT(f)  "%" f "s"

	#define POINTER_CFORMAT(f)  "%" f "p"

	#define INT8_CFORMAT(f)   "%" f "d"
	#define INT16_CFORMAT(f)  "%" f "hd"
	#define INT32_CFORMAT(f)  "%" f "ld"
	#define INT64_CFORMAT(f)  "%" f "lld"
	#define INT_CFORMAT(f)    "%" f "d"

	#define UINT8_CFORMAT(f)  "%" f "u"
	#define UINT16_CFORMAT(f) "%" f "hu"
	#define UINT32_CFORMAT(f) "%" f "lu"
	#define UINT64_CFORMAT(f) "%" f "llu"
	#define UINT_CFORMAT(f)   "%" f "u"

	#define SINT8_CFORMAT(f)  "%+" f "d"
	#define SINT16_CFORMAT(f) "%+" f "hd"
	#define SINT32_CFORMAT(f) "%+" f "ld"
	#define SINT64_CFORMAT(f) "%+" f "lld"
	#define SINT_CFORMAT(f)   "%+" f "d"

	#define XINT8_CFORMAT(f)  "0x%" f "x"
	#define XINT16_CFORMAT(f) "0x%" f "hx"
	#define XINT32_CFORMAT(f) "0x%" f "lx"
	#define XINT64_CFORMAT(f) "0x%" f "llx"
	#define XINT_CFORMAT(f)   "0x%" f "x"

	#define OINT8_CFORMAT(f)  "0%" f "o"
	#define OINT16_CFORMAT(f) "0%" f "ho"
	#define OINT32_CFORMAT(f) "0%" f "lo"
	#define OINT64_CFORMAT(f) "0%" f "llo"
	#define OINT_CFORMAT(f)   "0%" f "o"

	/*}}}*/

#endif
/*}}}*/

/*{{{ Windows / Visual C++ 6.0 / 32 bits systems */
#ifdef WIN32

	#include <limits.h>

	/*{{{  Basic types */

	typedef unsigned __int8  uint8;
	typedef unsigned __int16 uint16;
	typedef unsigned __int32 uint32;
	typedef unsigned __int64 uint64;

	typedef   signed __int8  sint8;
	typedef   signed __int16 sint16;
	typedef   signed __int32 sint32;
	typedef   signed __int64 sint64;

	/*}}}*/

	/*{{{  Limits */

	#define UINT8_MIN  0
	#define UINT16_MIN 0
	#define UINT32_MIN 0
	#define UINT64_MIN 0

	#define UINT8_MAX  UCHAR_MAX
	#define UINT16_MAX USHRT_MAX
	#define UINT32_MAX ULONG_MAX
	#define UINT64_MAX 18446744073709551615

	#define SINT8_MIN  SCHAR_MIN
	#define SINT16_MIN SHRT_MIN
	#define SINT32_MIN LONG_MIN
	#define SINT64_MIN (-9223372036854775807-1)

	#define SINT8_MAX  SCHAR_MAX
	#define SINT16_MAX SHRT_MAX
	#define SINT32_MAX LONG_MAX
	#define SINT64_MAX (9223372036854775807)

	/*}}}*/

	/*{{{  Constant builders */

	#define UINT8_C(c)  ((uint8 )(c))
	#define UINT16_C(c) ((uint16)(c))
	#define UINT32_C(c) ((uint32)(c))
	#define UINT64_C(c) ((uint64)(c))

	#define SINT8_C(c)  ((sint8 )(c))
	#define SINT16_C(c) ((sint16)(c))
	#define SINT32_C(c) ((sint32)(c))
	#define SINT64_C(c) ((sint64)(c))

	/*}}}*/

	/*{{{  C printf format strings */

	#define STRING_CFORMAT(f)  "%" f "s"

	#define POINTER_CFORMAT(f)  "0x%" f "x"

	#define INT8_CFORMAT(f)   "%" f "d"
	#define INT16_CFORMAT(f)  "%" f "hd"
	#define INT32_CFORMAT(f)  "%" f "ld"
	#define INT64_CFORMAT(f)  "%" f "I64d"
	#define INT_CFORMAT(f)    "%" f "d"

	#define UINT8_CFORMAT(f)  "%" f "u"
	#define UINT16_CFORMAT(f) "%" f "hu"
	#define UINT32_CFORMAT(f) "%" f "lu"
	#define UINT64_CFORMAT(f) "%" f "I64u"
	#define UINT_CFORMAT(f)   "%" f "u"

	#define SINT8_CFORMAT(f)  "%+" f "d"
	#define SINT16_CFORMAT(f) "%+" f "hd"
	#define SINT32_CFORMAT(f) "%+" f "ld"
	#define SINT64_CFORMAT(f) "%+" f "I64d"
	#define SINT_CFORMAT(f)   "%+" f "d"

	#define XINT8_CFORMAT(f)  "0x%" f "x"
	#define XINT16_CFORMAT(f) "0x%" f "hx"
	#define XINT32_CFORMAT(f) "0x%" f "lx"
	#define XINT64_CFORMAT(f) "0x%" f "I64x"
	#define XINT_CFORMAT(f)   "0x%" f "x"

	#define OINT8_CFORMAT(f)  "0%" f "o"
	#define OINT16_CFORMAT(f) "0%" f "ho"
	#define OINT32_CFORMAT(f) "0%" f "lo"
	#define OINT64_CFORMAT(f) "0%" f "I64o"
	#define OINT_CFORMAT(f)   "0%" f "o"

	/*}}}*/

#endif
/*}}}*/

#endif

