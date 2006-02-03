/*

  Copyright (C) 2004 STMicroelectronics.  All Rights Reserved.

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

*/

#ifndef aliasing_ptr_INCLUDED
#define aliasing_ptr_INCLUDED

/*
 * This template class redefines a const pointer type on some integral type
 * where dereferencing is safe concerning ISO strict aliasing rules.
 * For the strict alising rules refer to the ISO C99 standard or
 * -f[no-]strict-aliasing gcc option.
 *
 * For intance in the following code:
 * int64_t a = value;
 * const int32_t *p = &a;
 * int32_t b = *p;
 *
 * With the strict aliasing rules the assignation to a may be discarded
 * or may appear after the assignation to b as the lvalue *p is not
 * of the type of a.
 * The strict alising rule specify that an object of type T may be accessed
 * only by lvalue of a compatible type, of type aggregation containing
 * a compatible type, or of type char.
 *
 * This class can be used as follow for the above example:
 * int64_t a = value;
 * ALIASING_INT_PTR<int32_t> p = &a;
 * int32_t b = *p;
 * 
 * This class can be used only to redefine pointer on const integer type.
 * The template class is instasnciated with the pointed type (INT_T)
 * of the accessing pointer that may alias any object.
 * The implementation of the class holds a const unsigned char * pointer.
 * Arithmetic on the pointer is overloaded to match the arithmetic on
 * an INT_T * pointer type.
 * Dereference of the pointer is overloaded to match dereference of
 * and INT_T * pointer.
 *
 * Contructors:
 *	  ALIASING_INT_PTR(const void *p)
 *	Constructs an alias safe ptr from the the pointer p.
 *
 * Available overloaded operators are:
 * --, ++ post and prefix.
 * += (int), -= (int), + (int), - (int)
 * *(), [] (int)
 *
 */

#include "config_host.h"	// For endianness.

template <typename INT_T>
class ALIASING_INT_PTR
{
 private:
  const unsigned char *_p;
  static INT_T deref(const unsigned char *p) {
    INT_T res = 0;
    int i;
#if HOST_IS_LITTLE_ENDIAN == 1
    for (i = (int)sizeof(INT_T)-1; i >= 0; i--) {
      res <<= 8;
      res |= p[i] & 0xff;
    }
#else
    for (i = 0; i < (int)sizeof(INT_T); i++) {
      res <<= 8;
      res |= p[i] & 0xff;
    }
#endif
    return res;
  }

 public:
  /*
   * Constructors.
   */
  ALIASING_INT_PTR(const void *p) : _p((const unsigned char *)p) {}

  /*
   * Arithmetic operators.
   */
  ALIASING_INT_PTR<INT_T> &operator+=(int a) { 
    _p += a*sizeof(INT_T); return *this; 
  }
  ALIASING_INT_PTR<INT_T> &operator-=(int a) {
    _p -= a*sizeof(INT_T); return *this;
  }

  ALIASING_INT_PTR<INT_T> operator+(int a) {
    ALIASING_INT_PTR<INT_T> tmp(this->_p);
    tmp += a; return tmp;
  }
  ALIASING_INT_PTR<INT_T> operator-(int a) {
    ALIASING_INT_PTR<INT_T> tmp(this->_p);
    tmp -= a; return tmp;
  }
  
  ALIASING_INT_PTR<INT_T> operator++(int) { 
    ALIASING_INT_PTR<INT_T> tmp(this->_p); 
    _p += sizeof(INT_T); return tmp; 
  }
  ALIASING_INT_PTR<INT_T> operator++() {
    _p += sizeof(INT_T); return *this;
  }
  ALIASING_INT_PTR<INT_T> operator--(int) {
    ALIASING_INT_PTR<INT_T> tmp(this->_p);
    _p -= sizeof(INT_T); return tmp;
  }
  ALIASING_INT_PTR<INT_T> operator--() {
    _p -= sizeof(INT_T); return *this; 
  }
  
  /*
   * Dereference operators.
   */
  INT_T operator*() const { return deref(_p); }
  INT_T operator[](int a) const { return deref(_p + a*sizeof(INT_T)); }
};


#endif /* !aliasing_ptr_INCLUDED */
