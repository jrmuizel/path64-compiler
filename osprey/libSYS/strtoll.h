/*
 * strtoll.h.
 * $Id$
 *
 * Note on the implementation :
 * - support determination of base as follow :
 *	0b...	-> binary (2)
 *	0x...	-> hexadecimal (16)
 * 	0...	-> octal (8)
 *	other	-> decimal (10)
 * - no support for wide char
 * - no support for locale settings
 */

#ifndef __STRTOLL_H__
#define __STRTOLL_H__

#include <W_stdint.h>

int64_t SYS_strtoll (const char *nptr, const char **endptr, int base);
uint64_t SYS_strtoull (const char *nptr, const char **endptr, int base);

#endif
