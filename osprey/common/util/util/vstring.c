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


/*
 *  String utilities that use varying length strings
 */
#define USE_STANDARD_TYPES
#include <stdarg.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "vstring.h"
#include "errors.h"

#define MAX(a,b)        ((a>=b)?a:b)

#define vstr_max(v)	        ((v).max+0)
#define vstr_is_vstring0(v)     ((v).is_vstring0+0)
#define set_vstr_max(v,m)	(v).max = m
#define set_vstr_len(v,l)	(v).len = l
#define set_vstr_str(v,s)	(v).str = s

static vstring vstring0 = {0,0,NULL,1};
static int vstring0_in_use = 0;

static void
Realloc_Vstring (vstring *v, int newlen)
{
	/* make sure realloc doesn't just add 1 char */
	newlen = MAX (newlen, vstr_max(*v)+80);
	set_vstr_max(*v, newlen);
	set_vstr_str(*v, (char*) realloc(vstr_str(*v), vstr_max(*v)));
}

/*
 * For now, can only have one vstring buffer live at a time.
 * Someday should enhance this, but not needed now.
 * To do that, we could keep freelist of vstrings, 
 * and reuse space that way.
 */
/* Stephen Clarke, 030906:
 * I have enhanced the implementation to allow multiple vstring
 * buffers.
 * One vstring buffer (vstring0) is held permanently allocated
 * just as in the original implementation.
 * But if this buffer is in use, extra
 * buffers will be dynamically allocated on demand in
 * vstr_begin, and dynamically freed in vstr_end.
 * The implementation is thus biased towards the use of multiple
 * buffers being rare.
 * Note: the previous implementation attempted to warn
 * if an attempt was made to use multiple vstring buffers.
 * Unfortunately the test was flawed: it would never produce a
 * warning.
 */

/* must call vstr_{begin,end} around use of a vstring buffer */

vstring
vstr_begin (int len) 
{
  if (!vstring0_in_use) {
	vstring0_in_use = 1;
	if (vstr_max(vstring0) == 0) {
		set_vstr_str(vstring0, (char*) malloc(len));
		set_vstr_max(vstring0, len);
	}
	if (vstr_max(vstring0) < len) {
		Realloc_Vstring (&vstring0, len);
	}
	return vstring0;
  }
  else {
        vstring new_vstring = { 0, 0, NULL, 0 };

	set_vstr_str(new_vstring, malloc(len));
        if (vstr_str(new_vstring) != 0) {
	  set_vstr_max (new_vstring, len);
	}
	return new_vstring;
  }
}

/* frees space used by vstring buffer */
void
vstr_end (vstring v)
{
  if (vstr_is_vstring0(v)) {
	set_vstr_len(v, 0);
	v.str[0] = '\0';
	vstring0 = v;	/* so reused later */
        vstring0_in_use = 0;
  } else {
    free (vstr_str (v));
  }
}

/* add char to vstring */
vstring
vstr_append (vstring v, const char c)
{
	if (vstr_len(v) + 1 > vstr_max(v)) {
		Realloc_Vstring (&v, vstr_len(v) + 1);
	}
	v.str[v.len] = c;
	v.len++;
	return v;
}


/* add string to vstring */
vstring
vstr_concat (vstring v, const char *s)
{
	int slen = strlen(s);
	if (vstr_len(v) + slen > vstr_max(v)) {
		Realloc_Vstring (&v, vstr_len(v) + slen);
	}
	/* may be nulls in vstr, so can't concat from beginning;
	 * instead just copy onto end of string. */
	strcpy(vstr_str(v)+vstr_len(v), s);
	set_vstr_len(v, vstr_len(v) + slen);
	return v;
}


int int_length(int size, char is_hexa)
{
    unsigned long long maxu = ((unsigned long long)1)<<(8*size);
    char buf[50];
    if (is_hexa)
        sprintf(buf, "%llx", maxu);
    else
        sprintf(buf, "%+lld", maxu);
    return strlen(buf);
}


/* 
 * sprintf that reallocs space if needed.
 * the string is formatted into the vstring v at index position.
 */
int
vstr_sprintf (vstring *v, int index, const char *format, ... /* args */)
{
    int len;
    va_list ap;
    char *p;
    len = strlen(format);
    va_start (ap, format);
    p = (char*) format;
    while (*p != '\0') {
        if (*p == '%') {
            p++;
            if (*p == '%') ;	/* ignore */
            else if (*p == 's') {
                /* max length = length of the string */
                len += strlen(va_arg(ap,char*));
            }
#ifdef TARG_ST
	    // [CL] add support for %c format
	    else if (*p == 'c') {
	      len++;
	      va_arg(ap,int);
	    }
#endif
            else {
                int lqualifier = 0;
                int hqualifier = 0;
                char recognized_format = 0;
                int size = 0;
                do {
                    char is_hexa = 0;
                    switch (*p) {
                        
                    /* FORMAT FLAGS */
                    case '+' :   /* generate a plus sign for signed
                                    values that are positive */
                    case ' ' : { /* generate a space for signed values
                                    that have neither a plus nor a
                                    minus sign */
                        len++;
                        p++; break;
                    }                    
                    case '#' : { /* to prefix 0 on an o conversion, to
                                    prefix 0x on an x conversion, to
                                    prefix 0X on an X conversion, or
                                    to generate a decimal point and
                                    fraction digits that are otherwise
                                    suppressed on a floating-point
                                    conversion */
                        len+=2;
                        p++; break;
                    }
                    case '0' :   /* pad a conversion with leading zeros */
                    case '-' : { /* left-justify a conversion */
                        Fatal_Error("unsupported format flags");
                    }
                    
                    /* QUALIFIERS */
                    case 'l' : { /* remember */
                        ++lqualifier; ++p;
                        break;
                    }
                    case 'h': { /* remember */
                        ++hqualifier; ++p;
                        break;
                    }
                    case 'j':
                    case 't':
                    case 'z':
                    case 'L': {
                        Fatal_Error("unsupported qualifier");
                        break;
                    }

                    /* FIELD WIDTH */
                    /* not supported yet ! */

                    /* PRINT CONVERSION SPECIFIER */
                    
                    case 'x' : /* integers */
                    case 'X' :
                    case 'd' :
                    case 'i' :
                    case 'u' : {
                        recognized_format++;
                        switch (lqualifier) {
                        case 0: {
                            
                            switch (hqualifier) {
                            case 0: {  /* int */
                                va_arg(ap,int);
                                size = sizeof(int);
                                break;
                            }
                            case 1: { /* short */
                                /* C promotion to int */
                                (short)va_arg(ap,int);
                                size = sizeof(short);
                                break;
                            }
                            case 2 : { /* char */
                                /* C promotion to int */
                                (char)va_arg(ap,int);
                                size = sizeof(char);
                                break;
                            }
                            default: { /* shorter than char ? */
                                DevWarn("Unexpected format treated as d \n");
                                va_arg(ap,int);
                                size = sizeof(int);
                                break;
                            }
                            } /* swith hqualifier */

                            break;
                        }
                        case 1: { /* long */
                            va_arg(ap,long);
                            size = sizeof(long);
                            break;
                        }
                        case 2: { /* long long */
                            va_arg(ap,long long);
                            size = sizeof(long);
                            break;
                        }
                        default: { /* longer than long long ? */
                            DevWarn("Unexpected format treated as lld \n");
                            va_arg(ap,long long);
                            size = sizeof(long long);
                            break;
                        }
                        } /* swith lqualifier */
                        len += int_length(size, *p=='x' || *p=='X');
                        break;
                    }
                    
                    case 'f':
                    case 'e':
                    case 'E':
                    case 'g':
                    case 'G': {
                        Fatal_Error("unsupported floating point formats");
                        break;
                    }

                    default : {
                        Fatal_Error("unrecognized format");
                        break;
                    }
                    } /* swith *p */
                } while (*p != '\0' && !recognized_format);
            }
        }
        p++;
    }
    va_end(ap);
    if (len > vstr_max(*v)) {
        Realloc_Vstring (v, len);
    }
    va_start (ap, format);
    len = vsprintf(v->str+index, format, ap);
    set_vstr_len(*v, index + len);
    va_end(ap);
    if (vstr_len(*v) > vstr_max(*v)) 
        Fatal_Error("vstr_sprintf overflowed");
    return len;
}
