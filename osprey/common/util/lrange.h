/*
  Copyright (C) 2006, STMicroelectronics, All Rights Reserved.

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

/* ====================================================================
 * ====================================================================
 *
 * Module: lrange.h
 *
 * Description:
 *
 * Implement a base class for polymorphic range type analysis, 
 * including lattice interface
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef LRANGE_H_INCLUDED
#define LRANGE_H_INCLUDED

#include "W_limits.h"
#include "zint.h"

class LRange {
public:
  // lattice constructors
  virtual LRange *makeUniverse() const {   FmtAssert(0, ("Universe () has not been implemented"));
  }
  virtual LRange *makeBottom () const {   FmtAssert(0, ("Bottom () has not been implemented"));
  }
  virtual LRange *makeTop () const { return makeBottom(); }
  virtual LRange *makeMeet (const LRange *a, const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeJoin (const LRange *a, const LRange *b) const {
    return makeBottom ();}

  // queries
  virtual LRange *makeSignExtend (INT width) const {
    return makeBottom ();}
  virtual LRange *makeZeroExtend (INT width) const {
    return makeBottom ();}
  virtual LRange *makeLeftShift (INT count) const {
    return makeBottom ();}
  virtual LRange *makeLeftShiftRange (const LRange *count) const {
    return makeBottom ();}
  virtual LRange *makeSignedRightShift (INT count) const {
    return makeBottom ();}
  virtual LRange *makeSignedRightShiftRange (const LRange *count) const {
    return makeBottom ();}
  virtual LRange *makeModulo (INT width) const {
    return makeBottom ();}
  virtual LRange *makeAdd (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeAddModulo (const LRange *b, INT width) const {
    return makeBottom ();}
  virtual LRange *makeSub (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeSubModulo (const LRange *b, INT width) const {
    return makeBottom ();}
  virtual LRange *makeMul (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeMulModulo (const LRange *b, INT width) const {
    return makeBottom ();}
  virtual LRange *makeDiv (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeMod (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeMin (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeMax (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeBitAnd (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeBitOr (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeBitXor (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeBitNot () const {
    return makeBottom ();}
  virtual LRange *makeBytePermute (INT mask) const {
    return makeBottom ();}
  virtual LRange *makeEq (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeNe (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeGe (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeGt (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeLe (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeLt (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeAndl (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeNandl (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeOrl (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeNorl (const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeNotl () const {
    return makeBottom ();}
  virtual LRange *makeHighestSetBit () const {
    return makeBottom ();}
/*   virtual LRange *makeByteSwap () const { */
/*     return makeBottom ();} */
  virtual LRange *makeExtract (INT start, INT width) const {
    return makeBottom ();}
  virtual LRange *makeInsert (INT start, INT width,
			     const LRange *b) const {
    return makeBottom ();}
  virtual LRange *makeMakeUnsigned (INT width) const {
    return makeBottom ();}
  virtual LRange *makeMakeSigned (INT width) const {
    return makeBottom ();}

  friend LRange *SignExtend (const LRange *a, INT width){
    return a->makeSignExtend(width);}
  friend LRange *ZeroExtend (const LRange *a, INT width){
    return a->makeZeroExtend(width);}
  friend LRange *LeftShift (const LRange *a, INT count){
    return a->makeLeftShift(count);}
  friend LRange *LeftShift (const LRange *a, const LRange *count){
    return a->makeLeftShiftRange(count);}
  friend LRange *SignedRightShift (const LRange *a, INT count){
    return a->makeSignedRightShift(count);}
  friend LRange *SignedRightShift (const LRange *a, const LRange *count){
    return a->makeSignedRightShiftRange(count);}
  friend LRange *Modulo (const LRange *a, INT width){
    return a->makeModulo(width);}
  friend LRange *Add (const LRange *a, const LRange *b){
    return a->makeAdd(b);}
  friend LRange *AddModulo (const LRange *a, const LRange *b, INT width){
    return a->makeAddModulo(b, width);}
  friend LRange *Sub (const LRange *a, const LRange *b){
    return a->makeSub(b);}
  friend LRange *SubModulo (const LRange *a, const LRange *b, INT width){
    return a->makeSubModulo(b, width);}
  friend LRange *Mul (const LRange *a, const LRange *b){
    return a->makeMul(b);}
  friend LRange *MulModulo (const LRange *a, const LRange *b, INT width){
    return a->makeMulModulo(b, width);}
  friend LRange *Div (const LRange *a, const LRange *b){
    return a->makeDiv(b);}
  friend LRange *Mod (const LRange *a, const LRange *b){
    return a->makeMod(b);}
  friend LRange *Min (const LRange *a, const LRange *b){
    return a->makeMin(b);}
  friend LRange *Max (const LRange *a, const LRange *b){
    return a->makeMax(b);}
  friend LRange *BitAnd (const LRange *a, const LRange *b){
    return a->makeBitAnd(b);}
  friend LRange *BitOr (const LRange *a, const LRange *b){
    return a->makeBitOr(b);}
  friend LRange *BitXor (const LRange *a, const LRange *b){
    return a->makeBitXor(b);}
  friend LRange *BitNot (const LRange *a){
    return a->makeBitNot();}
  friend LRange *BytePermute (const LRange *a, INT mask){
    return a->makeBytePermute(mask);}
  friend LRange *Eq (const LRange *a, const LRange *b){
    return a->makeEq(b);}
  friend LRange *Ne (const LRange *a, const LRange *b){
    return a->makeNe(b);}
  friend LRange *Ge (const LRange *a, const LRange *b){
    return a->makeGe(b);}
  friend LRange *Gt (const LRange *a, const LRange *b){
    return a->makeGt(b);}
  friend LRange *Le (const LRange *a, const LRange *b){
    return a->makeLe(b);}
  friend LRange *Lt (const LRange *a, const LRange *b){
    return a->makeLt(b);}
  friend LRange *Andl (const LRange *a, const LRange *b){
    return a->makeAndl(b);}
  friend LRange *Nandl (const LRange *a, const LRange *b){
    return a->makeNandl(b);}
  friend LRange *Orl (const LRange *a, const LRange *b){
    return a->makeOrl(b);}
  friend LRange *Norl (const LRange *a, const LRange *b){
    return a->makeNorl(b);}
  friend LRange *Notl (const LRange *a){
    return a->makeNotl();}
  friend LRange *HighestSetBit (const LRange *a){
    return a->makeHighestSetBit();}
/*   friend LRange *ByteSwap (const LRange *a){ */
/*     return a->makeByteSwap();} */
  friend LRange *Extract (const LRange *a, INT start, INT width){
    return a->makeExtract(start, width);}
  friend LRange *Insert (const LRange *a, INT start, INT width,
			     const LRange *b){
    return a->makeInsert(start, width, b);}
  friend LRange *MakeUnsigned (const LRange *a, INT width){
    return a->makeMakeUnsigned(width);}
  friend LRange *MakeSigned (const LRange *a, INT width){
    return a->makeMakeSigned(width);}



  // queries
  virtual BOOL hasValue () const { return false; }
  virtual INT64 getValue () const { return 0; }
  virtual BOOL StrictlyContains (const LRange *a) const{return 0;}
  virtual BOOL Equal (const LRange *a) const{return 0;}
  virtual BOOL ContainsOrEqual (const LRange *a) const{return 0;}
  virtual BOOL isNonZero () const{return 0;}
  virtual BOOL isNonNegative () const{return 0;}
  virtual BOOL isZero () const{return 0;}
  virtual BOOL isTop () const{return 0;}
  virtual INT bits() const{return INT_MAX;}
  virtual INT getTzcnt() const{return 0;}
  virtual UINT64 getBitMask() const{return 0;}
  virtual UINT64 getValMask() const{return 0;}
  virtual UINT64 getZeroMask() const{return 0;}
  virtual UINT64 getOneMask() const{return 0;}
  virtual UINT64 getBase() const{return 0;}
  virtual UINT64 getBias() const{return 0;}
  virtual void Print (FILE *f) const{}

  // range constructors
  // generic
  virtual LRange *makeRangeValue (INT64 value) const {
    return makeBottom ();}
  // specific Forward
  // Value Range
  virtual LRange *makeRangeWidth (INT Sign, INT bitwidth) const {
    return makeBottom ();}
  virtual LRange *makeRangeLowbit (INT Sign, INT lowbit, INT bitwidth) const {
    return makeBottom ();}
  virtual LRange *makeRangeMinMax (ZInt min, ZInt max) const {
    return makeBottom ();}
  // Bit-value mask
  virtual LRange *makeBitValue (UINT64 zeromask, UINT64 onemask) const {
    return makeBottom ();}
  // alignment range
  virtual LRange *makeAlign (ZInt base, ZInt bias) const {
    return makeBottom ();}

  // specific Backward
  virtual LRange *makeRangeBitWidth (INT bitwidth) const {
    return makeBottom ();}
  virtual LRange *makeRangeLowBitWidth (INT lowbit, INT bitwidth) const {
    return makeBottom ();}

  // cloning method
  virtual LRange *clone () const { return makeBottom (); }

};

typedef LRange *LRange_p;
typedef LRange *LRange_pc;

/*
 * Support macro for dynamic cast of a LRange * to a pointer to a base class.
 * To be used in place of:
 * Base *t = (Base *)lrange_ptr;
 * with:
 * Base *t = LRANGE_CAST(Base *, lrange_ptr);
 */
#if defined(__GNUC__)
#define LRANGE_CAST(t,e) ({ t tmp = dynamic_cast<t>(e); DevAssert(tmp != NULL,("Invalid cast to " #t)); tmp; })
#else
#define LRANGE_CAST(t,e) ((t)(e))
#endif


#endif /*RANGE_H_INCLUDED */
