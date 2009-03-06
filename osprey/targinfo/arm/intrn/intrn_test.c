/* Test of intrinsics generated from MDS. */
/* This is not a functional test (auto check)
 * but just to test the compiler configuration
 * i.e. is the front-end recognized intrinsics, ...
 */

/* Note on dedicated registers: as these registers are not allocated by the compiler,
 * it is necessary to pass the index of such register. When it is passed as builtin
 * operand, an immediate of the form 'u[0..n]' is expected in place of dedicated register.
 * 'n' is the size of the register file minus one.
 * When dedicated register is the result, one additional operand of the same form 'u[0..n]'
 * is placed as first parameter to select the result register index.
 *
 * Note on immediates: some builtins may accept constant value as parameter. Programmer must
 * respect the immediate range given by the bits number and signess.
 * Immediates are given under the form '[su][0-9]+'
 * - [su] for signed and unsigned respectively.
 * - [0-9]+ the bits number associated to the immediate.
 */


#include "builtins_header.h"

/*  divu fpr , gpr , gpr
 * fpr divu( gpr, gpr);
 */
unsigned
test_fpx_divu(unsigned a, unsigned b) {
  return __builtin_fpx_divu(a, b);
}

/*  div fpr , gpr , gpr
 * fpr div( gpr, gpr);
 */
int
test_fpx_div(int a, int b) {
  return __builtin_fpx_div(a, b);
}

/*  fabs fpr , fpr
 * fpr fabs( fpr);
 */
float
test_fpx_fabs(float a) {
  return __builtin_fpx_fabs(a);
}

/*  faddaa fpr , fpr , fpr
 * fpr faddaa( fpr, fpr);
 */
float
test_fpx_faddaa(float a, float b) {
  return __builtin_fpx_faddaa(a, b);
}

/*  faddn fpr , fpr , fpr
 * fpr faddn( fpr, fpr);
 */
float
test_fpx_faddn(float a, float b) {
  return __builtin_fpx_faddn(a, b);
}

/*  fadd fpr , fpr , fpr
 * fpr fadd( fpr, fpr);
 */
float
test_fpx_fadd(float a, float b) {
  return __builtin_fpx_fadd(a, b);
}

/*  fasub fpr , fpr , fpr
 * fpr fasub( fpr, fpr);
 */
float
test_fpx_fasub(float a, float b) {
  return __builtin_fpx_fasub(a, b);
}

/*  fclrs <u6>
 * fclrs( <u6>);
 */
void
test_fpx_fclrs() {
  __builtin_fpx_fclrs(60);
}

/*  fcmpeqn gpr , fpr , fpr
 * gpr fcmpeqn( fpr, fpr);
 */
int
test_fpx_fcmpeqn(float a, float b) {
  return __builtin_fpx_fcmpeqn(a, b);
}

/*  fcmpeq gpr , fpr , fpr
 * gpr fcmpeq( fpr, fpr);
 */
int
test_fpx_fcmpeq(float a, float b) {
  return __builtin_fpx_fcmpeq(a, b);
}

/*  fcmpgen gpr , fpr , fpr
 * gpr fcmpgen( fpr, fpr);
 */
int
test_fpx_fcmpgen(float a, float b) {
  return __builtin_fpx_fcmpgen(a, b);
}

/*  fcmpge gpr , fpr , fpr
 * gpr fcmpge( fpr, fpr);
 */
int
test_fpx_fcmpge(float a, float b) {
  return __builtin_fpx_fcmpge(a, b);
}

/*  fcmpgtn gpr , fpr , fpr
 * gpr fcmpgtn( fpr, fpr);
 */
int
test_fpx_fcmpgtn(float a, float b) {
  return __builtin_fpx_fcmpgtn(a, b);
}

/*  fcmpgt gpr , fpr , fpr
 * gpr fcmpgt( fpr, fpr);
 */
int
test_fpx_fcmpgt(float a, float b) {
  return __builtin_fpx_fcmpgt(a, b);
}

/*  fcmplen gpr , fpr , fpr
 * gpr fcmplen( fpr, fpr);
 */
int
test_fpx_fcmplen(float a, float b) {
  return __builtin_fpx_fcmplen(a, b);
}

/*  fcmple gpr , fpr , fpr
 * gpr fcmple( fpr, fpr);
 */
int
test_fpx_fcmple(float a, float b) {
  return __builtin_fpx_fcmple(a, b);
}

/*  fcmpltn gpr , fpr , fpr
 * gpr fcmpltn( fpr, fpr);
 */
int
test_fpx_fcmpltn(float a, float b) {
  return __builtin_fpx_fcmpltn(a, b);
}

/*  fcmplt gpr , fpr , fpr
 * gpr fcmplt( fpr, fpr);
 */
int
test_fpx_fcmplt(float a, float b) {
  return __builtin_fpx_fcmplt(a, b);
}

/*  fcmpnen gpr , fpr , fpr
 * gpr fcmpnen( fpr, fpr);
 */
int
test_fpx_fcmpnen(float a, float b) {
  return __builtin_fpx_fcmpnen(a, b);
}

/*  fcmpne gpr , fpr , fpr
 * gpr fcmpne( fpr, fpr);
 */
int
test_fpx_fcmpne(float a, float b) {
  return __builtin_fpx_fcmpne(a, b);
}

/*  fcmpueqn gpr , fpr , fpr
 * gpr fcmpueqn( fpr, fpr);
 */
int
test_fpx_fcmpueqn(float a, float b) {
  return __builtin_fpx_fcmpueqn(a, b);
}

/*  fcmpueq gpr , fpr , fpr
 * gpr fcmpueq( fpr, fpr);
 */
int
test_fpx_fcmpueq(float a, float b) {
  return __builtin_fpx_fcmpueq(a, b);
}

/*  fcmpugen gpr , fpr , fpr
 * gpr fcmpugen( fpr, fpr);
 */
int
test_fpx_fcmpugen(float a, float b) {
  return __builtin_fpx_fcmpugen(a, b);
}

/*  fcmpuge gpr , fpr , fpr
 * gpr fcmpuge( fpr, fpr);
 */
int
test_fpx_fcmpuge(float a, float b) {
  return __builtin_fpx_fcmpuge(a, b);
}

/*  fcmpugtn gpr , fpr , fpr
 * gpr fcmpugtn( fpr, fpr);
 */
int
test_fpx_fcmpugtn(float a, float b) {
  return __builtin_fpx_fcmpugtn(a, b);
}

/*  fcmpugt gpr , fpr , fpr
 * gpr fcmpugt( fpr, fpr);
 */
int
test_fpx_fcmpugt(float a, float b) {
  return __builtin_fpx_fcmpugt(a, b);
}

/*  fcmpulen gpr , fpr , fpr
 * gpr fcmpulen( fpr, fpr);
 */
int
test_fpx_fcmpulen(float a, float b) {
  return __builtin_fpx_fcmpulen(a, b);
}

/*  fcmpule gpr , fpr , fpr
 * gpr fcmpule( fpr, fpr);
 */
int
test_fpx_fcmpule(float a, float b) {
  return __builtin_fpx_fcmpule(a, b);
}

/*  fcmpultn gpr , fpr , fpr
 * gpr fcmpultn( fpr, fpr);
 */
int
test_fpx_fcmpultn(float a, float b) {
  return __builtin_fpx_fcmpultn(a, b);
}

/*  fcmpult gpr , fpr , fpr
 * gpr fcmpult( fpr, fpr);
 */
int
test_fpx_fcmpult(float a, float b) {
  return __builtin_fpx_fcmpult(a, b);
}

/*  fcmpunen gpr , fpr , fpr
 * gpr fcmpunen( fpr, fpr);
 */
int
test_fpx_fcmpunen(float a, float b) {
  return __builtin_fpx_fcmpunen(a, b);
}

/*  fcmpune gpr , fpr , fpr
 * gpr fcmpune( fpr, fpr);
 */
int
test_fpx_fcmpune(float a, float b) {
  return __builtin_fpx_fcmpune(a, b);
}

/*  fcmpunn gpr , fpr , fpr
 * gpr fcmpunn( fpr, fpr);
 */
int
test_fpx_fcmpunn(float a, float b) {
  return __builtin_fpx_fcmpunn(a, b);
}

/*  fcmpun gpr , fpr , fpr
 * gpr fcmpun( fpr, fpr);
 */
int
test_fpx_fcmpun(float a, float b) {
  return __builtin_fpx_fcmpun(a, b);
}

/*  fcmpuunn gpr , fpr , fpr
 * gpr fcmpuunn( fpr, fpr);
 */
int
test_fpx_fcmpuunn(float a, float b) {
  return __builtin_fpx_fcmpuunn(a, b);
}

/*  fcmpuun gpr , fpr , fpr
 * gpr fcmpuun( fpr, fpr);
 */
int
test_fpx_fcmpuun(float a, float b) {
  return __builtin_fpx_fcmpuun(a, b);
}

/*  fcnst fpr , <u9>
 * fpr fcnst( <u9>);
 */
float
test_fpx_fcnst() {
  return __builtin_fpx_fcnst(508);
}

/*  fdiv fpr , fpr , fpr
 * fpr fdiv( fpr, fpr);
 */
float
test_fpx_fdiv(float a, float b) {
  return __builtin_fpx_fdiv(a, b);
}

/*  ff2in gpr , fpr
 * gpr ff2in( fpr);
 */
int
test_fpx_ff2in_r(float a) {
  return __builtin_fpx_ff2in_r(a);
}

/*  ff2in fpr , fpr
 * fpr ff2in( fpr);
 */
int
test_fpx_ff2in_f(float a) {
  return __builtin_fpx_ff2in_f(a);
}

/*  ff2i gpr , fpr
 * gpr ff2i( fpr);
 */
int
test_fpx_ff2i_r(float a) {
  return __builtin_fpx_ff2i_r(a);
}

/*  ff2i fpr , fpr
 * fpr ff2i( fpr);
 */
int
test_fpx_ff2i_f(float a) {
  return __builtin_fpx_ff2i_f(a);
}

/*  ff2un gpr , fpr
 * gpr ff2un( fpr);
 */
unsigned
test_fpx_ff2un_r(float a) {
  return __builtin_fpx_ff2un_r(a);
}

/*  ff2un fpr , fpr
 * fpr ff2un( fpr);
 */
unsigned
test_fpx_ff2un_f(float a) {
  return __builtin_fpx_ff2un_f(a);
}

/*  ff2u gpr , fpr
 * gpr ff2u( fpr);
 */
unsigned
test_fpx_ff2u_r(float a) {
  return __builtin_fpx_ff2u_r(a);
}

/*  ff2u fpr , fpr
 * fpr ff2u( fpr);
 */
unsigned
test_fpx_ff2u_f(float a) {
  return __builtin_fpx_ff2u_f(a);
}

/*  fi2f fpr , gpr
 * fpr fi2f( gpr);
 */
float
test_fpx_fi2f_r(int a) {
  return __builtin_fpx_fi2f_r(a);
}

/*  fi2f fpr , fpr
 * fpr fi2f( fpr);
 */
float
test_fpx_fi2f_f(int a) {
  return __builtin_fpx_fi2f_f(a);
}

/*  fmacn fpr , fpr , fpr
 * fpr fmacn( fpr, fpr, fpr);
 */
float
test_fpx_fmacn(float a, float b, float c) {
  return __builtin_fpx_fmacn(a, b, c);
}

/*  fmac fpr , fpr , fpr
 * fpr fmac( fpr, fpr, fpr);
 */
float
test_fpx_fmac(float a, float b, float c) {
  return __builtin_fpx_fmac(a, b, c);
}

/*  fmax fpr , fpr , fpr
 * fpr fmax( fpr, fpr);
 */
float
test_fpx_fmax(float a, float b) {
  return __builtin_fpx_fmax(a, b);
}

/*  fmin fpr , fpr , fpr
 * fpr fmin( fpr, fpr);
 */
float
test_fpx_fmin(float a, float b) {
  return __builtin_fpx_fmin(a, b);
}

/*  fmuln fpr , fpr , fpr
 * fpr fmuln( fpr, fpr);
 */
float
test_fpx_fmuln(float a, float b) {
  return __builtin_fpx_fmuln(a, b);
}

/*  fmul fpr , fpr , fpr
 * fpr fmul( fpr, fpr);
 */
float
test_fpx_fmul(float a, float b) {
  return __builtin_fpx_fmul(a, b);
}

/*  fmvf2f fpr , fpr
 * fpr fmvf2f( fpr);
 */
float
test_fpx_fmvf2f(float a) {
  return __builtin_fpx_fmvf2f(a);
}

/*  fmvf2r gpr , fpr
 * gpr fmvf2r( fpr);
 */
int
test_fpx_fmvf2r(float a) {
  return __builtin_fpx_fmvf2r(a);
}

/*  fmvr2f fpr , gpr
 * fpr fmvr2f( gpr);
 */
float
test_fpx_fmvr2f(int a) {
  return __builtin_fpx_fmvr2f(a);
}

/*  fmvr2st gpr
 * fmvr2st( gpr);
 */
void
test_fpx_fmvr2st(int a) {
  __builtin_fpx_fmvr2st(a);
}

/*  fmvr2s gpr
 * fmvr2s( gpr);
 */
void
test_fpx_fmvr2s(int a) {
  __builtin_fpx_fmvr2s(a);
}

/*  fmvs2r gpr
 * gpr fmvs2r( );
 */
int
test_fpx_fmvs2r(void) {
  return __builtin_fpx_fmvs2r();
}

/*  fneg fpr , fpr
 * fpr fneg( fpr);
 */
float
test_fpx_fneg(float a) {
  return __builtin_fpx_fneg(a);
}

/*  fscalb fpr , fpr , gpr
 * fpr fscalb( fpr, gpr);
 */
float
test_fpx_fscalb(float a, int b) {
  return __builtin_fpx_fscalb(a, b);
}

/*  fsqrt fpr , fpr
 * fpr fsqrt( fpr);
 */
float
test_fpx_fsqrt(float a) {
  return __builtin_fpx_fsqrt(a);
}

/*  fsub fpr , fpr , fpr
 * fpr fsub( fpr, fpr);
 */
float
test_fpx_fsub(float a, float b) {
  return __builtin_fpx_fsub(a, b);
}

/*  fu2f fpr , gpr
 * fpr fu2f( gpr);
 */
float
test_fpx_fu2f_r(unsigned a) {
  return __builtin_fpx_fu2f_r(a);
}

/*  fu2f fpr , fpr
 * fpr fu2f( fpr);
 */
float
test_fpx_fu2f_f(unsigned a) {
  return __builtin_fpx_fu2f_f(a);
}

/*  macnsshh fpr , gpr , gpr
 * fpr macnsshh( fpr, gpr, gpr);
 */
int
test_fpx_macnsshh(int a, int b, int c) {
  return __builtin_fpx_macnsshh(a, b, c);
}

/*  macnsshl fpr , gpr , gpr
 * fpr macnsshl( fpr, gpr, gpr);
 */
int
test_fpx_macnsshl(int a, int b, int c) {
  return __builtin_fpx_macnsshl(a, b, c);
}

/*  macnsslh fpr , gpr , gpr
 * fpr macnsslh( fpr, gpr, gpr);
 */
int
test_fpx_macnsslh(int a, int b, int c) {
  return __builtin_fpx_macnsslh(a, b, c);
}

/*  macnssll fpr , gpr , gpr
 * fpr macnssll( fpr, gpr, gpr);
 */
int
test_fpx_macnssll(int a, int b, int c) {
  return __builtin_fpx_macnssll(a, b, c);
}

/*  macnsuhh fpr , gpr , gpr
 * fpr macnsuhh( fpr, gpr, gpr);
 */
int
test_fpx_macnsuhh(int a, int b, unsigned c) {
  return __builtin_fpx_macnsuhh(a, b, c);
}

/*  macnsuhl fpr , gpr , gpr
 * fpr macnsuhl( fpr, gpr, gpr);
 */
int
test_fpx_macnsuhl(int a, int b, unsigned c) {
  return __builtin_fpx_macnsuhl(a, b, c);
}

/*  macnsulh fpr , gpr , gpr
 * fpr macnsulh( fpr, gpr, gpr);
 */
int
test_fpx_macnsulh(int a, int b, unsigned c) {
  return __builtin_fpx_macnsulh(a, b, c);
}

/*  macnsull fpr , gpr , gpr
 * fpr macnsull( fpr, gpr, gpr);
 */
int
test_fpx_macnsull(int a, int b, unsigned c) {
  return __builtin_fpx_macnsull(a, b, c);
}

/*  macnushh fpr , gpr , gpr
 * fpr macnushh( fpr, gpr, gpr);
 */
int
test_fpx_macnushh(int a, unsigned b, int c) {
  return __builtin_fpx_macnushh(a, b, c);
}

/*  macnushl fpr , gpr , gpr
 * fpr macnushl( fpr, gpr, gpr);
 */
int
test_fpx_macnushl(int a, unsigned b, int c) {
  return __builtin_fpx_macnushl(a, b, c);
}

/*  macnuslh fpr , gpr , gpr
 * fpr macnuslh( fpr, gpr, gpr);
 */
int
test_fpx_macnuslh(int a, unsigned b, int c) {
  return __builtin_fpx_macnuslh(a, b, c);
}

/*  macnusll fpr , gpr , gpr
 * fpr macnusll( fpr, gpr, gpr);
 */
int
test_fpx_macnusll(int a, unsigned b, int c) {
  return __builtin_fpx_macnusll(a, b, c);
}

/*  macnuuhh fpr , gpr , gpr
 * fpr macnuuhh( fpr, gpr, gpr);
 */
int
test_fpx_macnuuhh(int a, unsigned b, unsigned c) {
  return __builtin_fpx_macnuuhh(a, b, c);
}

/*  macnuuhl fpr , gpr , gpr
 * fpr macnuuhl( fpr, gpr, gpr);
 */
int
test_fpx_macnuuhl(int a, unsigned b, unsigned c) {
  return __builtin_fpx_macnuuhl(a, b, c);
}

/*  macnuulh fpr , gpr , gpr
 * fpr macnuulh( fpr, gpr, gpr);
 */
int
test_fpx_macnuulh(int a, unsigned b, unsigned c) {
  return __builtin_fpx_macnuulh(a, b, c);
}

/*  macnuull fpr , gpr , gpr
 * fpr macnuull( fpr, gpr, gpr);
 */
int
test_fpx_macnuull(int a, unsigned b, unsigned c) {
  return __builtin_fpx_macnuull(a, b, c);
}

/*  macn fpr , gpr , gpr
 * fpr macn( fpr, gpr, gpr);
 */
int
test_fpx_macn(int a, int b, int c) {
  return __builtin_fpx_macn(a, b, c);
}

/*  macsshh fpr , gpr , gpr
 * fpr macsshh( fpr, gpr, gpr);
 */
int
test_fpx_macsshh(int a, int b, int c) {
  return __builtin_fpx_macsshh(a, b, c);
}

/*  macsshl fpr , gpr , gpr
 * fpr macsshl( fpr, gpr, gpr);
 */
int
test_fpx_macsshl(int a, int b, int c) {
  return __builtin_fpx_macsshl(a, b, c);
}

/*  macsslh fpr , gpr , gpr
 * fpr macsslh( fpr, gpr, gpr);
 */
int
test_fpx_macsslh(int a, int b, int c) {
  return __builtin_fpx_macsslh(a, b, c);
}

/*  macssll fpr , gpr , gpr
 * fpr macssll( fpr, gpr, gpr);
 */
int
test_fpx_macssll(int a, int b, int c) {
  return __builtin_fpx_macssll(a, b, c);
}

/*  macsuhh fpr , gpr , gpr
 * fpr macsuhh( fpr, gpr, gpr);
 */
int
test_fpx_macsuhh(int a, int b, unsigned c) {
  return __builtin_fpx_macsuhh(a, b, c);
}

/*  macsuhl fpr , gpr , gpr
 * fpr macsuhl( fpr, gpr, gpr);
 */
int
test_fpx_macsuhl(int a, int b, unsigned c) {
  return __builtin_fpx_macsuhl(a, b, c);
}

/*  macsulh fpr , gpr , gpr
 * fpr macsulh( fpr, gpr, gpr);
 */
int
test_fpx_macsulh(int a, int b, unsigned c) {
  return __builtin_fpx_macsulh(a, b, c);
}

/*  macsull fpr , gpr , gpr
 * fpr macsull( fpr, gpr, gpr);
 */
int
test_fpx_macsull(int a, int b, unsigned c) {
  return __builtin_fpx_macsull(a, b, c);
}

/*  macushh fpr , gpr , gpr
 * fpr macushh( fpr, gpr, gpr);
 */
int
test_fpx_macushh(int a, unsigned b, int c) {
  return __builtin_fpx_macushh(a, b, c);
}

/*  macushl fpr , gpr , gpr
 * fpr macushl( fpr, gpr, gpr);
 */
int
test_fpx_macushl(int a, unsigned b, int c) {
  return __builtin_fpx_macushl(a, b, c);
}

/*  macuslh fpr , gpr , gpr
 * fpr macuslh( fpr, gpr, gpr);
 */
int
test_fpx_macuslh(int a, unsigned b, int c) {
  return __builtin_fpx_macuslh(a, b, c);
}

/*  macusll fpr , gpr , gpr
 * fpr macusll( fpr, gpr, gpr);
 */
int
test_fpx_macusll(int a, unsigned b, int c) {
  return __builtin_fpx_macusll(a, b, c);
}

/*  macuuhh fpr , gpr , gpr
 * fpr macuuhh( fpr, gpr, gpr);
 */
int
test_fpx_macuuhh(int a, unsigned b, unsigned c) {
  return __builtin_fpx_macuuhh(a, b, c);
}

/*  macuuhl fpr , gpr , gpr
 * fpr macuuhl( fpr, gpr, gpr);
 */
int
test_fpx_macuuhl(int a, unsigned b, unsigned c) {
  return __builtin_fpx_macuuhl(a, b, c);
}

/*  macuulh fpr , gpr , gpr
 * fpr macuulh( fpr, gpr, gpr);
 */
int
test_fpx_macuulh(int a, unsigned b, unsigned c) {
  return __builtin_fpx_macuulh(a, b, c);
}

/*  macuull fpr , gpr , gpr
 * fpr macuull( fpr, gpr, gpr);
 */
int
test_fpx_macuull(int a, unsigned b, unsigned c) {
  return __builtin_fpx_macuull(a, b, c);
}

/*  mac fpr , gpr , gpr
 * fpr mac( fpr, gpr, gpr);
 */
int
test_fpx_mac(int a, int b, int c) {
  return __builtin_fpx_mac(a, b, c);
}

/*  modu fpr , gpr , gpr
 * fpr modu( gpr, gpr);
 */
unsigned
test_fpx_modu(unsigned a, unsigned b) {
  return __builtin_fpx_modu(a, b);
}

/*  mod fpr , gpr , gpr
 * fpr mod( gpr, gpr);
 */
int
test_fpx_mod(int a, int b) {
  return __builtin_fpx_mod(a, b);
}

/*  mpnsshh gpr , gpr , gpr
 * gpr mpnsshh( gpr, gpr);
 */
int
test_fpx_mpnsshh(int a, int b) {
  return __builtin_fpx_mpnsshh(a, b);
}

/*  mpnsshl gpr , gpr , gpr
 * gpr mpnsshl( gpr, gpr);
 */
int
test_fpx_mpnsshl(int a, int b) {
  return __builtin_fpx_mpnsshl(a, b);
}

/*  mpnsslh gpr , gpr , gpr
 * gpr mpnsslh( gpr, gpr);
 */
int
test_fpx_mpnsslh(int a, int b) {
  return __builtin_fpx_mpnsslh(a, b);
}

/*  mpnssll gpr , gpr , gpr
 * gpr mpnssll( gpr, gpr);
 */
int
test_fpx_mpnssll(int a, int b) {
  return __builtin_fpx_mpnssll(a, b);
}

/*  mpnsuhh gpr , gpr , gpr
 * gpr mpnsuhh( gpr, gpr);
 */
int
test_fpx_mpnsuhh(int a, unsigned b) {
  return __builtin_fpx_mpnsuhh(a, b);
}

/*  mpnsuhl gpr , gpr , gpr
 * gpr mpnsuhl( gpr, gpr);
 */
int
test_fpx_mpnsuhl(int a, unsigned b) {
  return __builtin_fpx_mpnsuhl(a, b);
}

/*  mpnsulh gpr , gpr , gpr
 * gpr mpnsulh( gpr, gpr);
 */
int
test_fpx_mpnsulh(int a, unsigned b) {
  return __builtin_fpx_mpnsulh(a, b);
}

/*  mpnsull gpr , gpr , gpr
 * gpr mpnsull( gpr, gpr);
 */
int
test_fpx_mpnsull(int a, unsigned b) {
  return __builtin_fpx_mpnsull(a, b);
}

/*  mpnushh gpr , gpr , gpr
 * gpr mpnushh( gpr, gpr);
 */
int
test_fpx_mpnushh(unsigned a, int b) {
  return __builtin_fpx_mpnushh(a, b);
}

/*  mpnushl gpr , gpr , gpr
 * gpr mpnushl( gpr, gpr);
 */
int
test_fpx_mpnushl(unsigned a, int b) {
  return __builtin_fpx_mpnushl(a, b);
}

/*  mpnuslh gpr , gpr , gpr
 * gpr mpnuslh( gpr, gpr);
 */
int
test_fpx_mpnuslh(unsigned a, int b) {
  return __builtin_fpx_mpnuslh(a, b);
}

/*  mpnusll gpr , gpr , gpr
 * gpr mpnusll( gpr, gpr);
 */
int
test_fpx_mpnusll(unsigned a, int b) {
  return __builtin_fpx_mpnusll(a, b);
}

/*  mpnuuhh gpr , gpr , gpr
 * gpr mpnuuhh( gpr, gpr);
 */
int
test_fpx_mpnuuhh(unsigned a, unsigned b) {
  return __builtin_fpx_mpnuuhh(a, b);
}

/*  mpnuuhl gpr , gpr , gpr
 * gpr mpnuuhl( gpr, gpr);
 */
int
test_fpx_mpnuuhl(unsigned a, unsigned b) {
  return __builtin_fpx_mpnuuhl(a, b);
}

/*  mpnuulh gpr , gpr , gpr
 * gpr mpnuulh( gpr, gpr);
 */
int
test_fpx_mpnuulh(unsigned a, unsigned b) {
  return __builtin_fpx_mpnuulh(a, b);
}

/*  mpnuull gpr , gpr , gpr
 * gpr mpnuull( gpr, gpr);
 */
int
test_fpx_mpnuull(unsigned a, unsigned b) {
  return __builtin_fpx_mpnuull(a, b);
}

/*  mpn gpr , gpr , gpr
 * gpr mpn( gpr, gpr);
 */
int
test_fpx_mpn(int a, int b) {
  return __builtin_fpx_mpn(a, b);
}

/*  mpsshh gpr , gpr , gpr
 * gpr mpsshh( gpr, gpr);
 */
int
test_fpx_mpsshh(int a, int b) {
  return __builtin_fpx_mpsshh(a, b);
}

/*  mpsshl gpr , gpr , gpr
 * gpr mpsshl( gpr, gpr);
 */
int
test_fpx_mpsshl(int a, int b) {
  return __builtin_fpx_mpsshl(a, b);
}

/*  mpsslh gpr , gpr , gpr
 * gpr mpsslh( gpr, gpr);
 */
int
test_fpx_mpsslh(int a, int b) {
  return __builtin_fpx_mpsslh(a, b);
}

/*  mpssll gpr , gpr , gpr
 * gpr mpssll( gpr, gpr);
 */
int
test_fpx_mpssll(int a, int b) {
  return __builtin_fpx_mpssll(a, b);
}

/*  mpsuhh gpr , gpr , gpr
 * gpr mpsuhh( gpr, gpr);
 */
int
test_fpx_mpsuhh(int a, unsigned b) {
  return __builtin_fpx_mpsuhh(a, b);
}

/*  mpsuhl gpr , gpr , gpr
 * gpr mpsuhl( gpr, gpr);
 */
int
test_fpx_mpsuhl(int a, unsigned b) {
  return __builtin_fpx_mpsuhl(a, b);
}

/*  mpsulh gpr , gpr , gpr
 * gpr mpsulh( gpr, gpr);
 */
int
test_fpx_mpsulh(int a, unsigned b) {
  return __builtin_fpx_mpsulh(a, b);
}

/*  mpsull gpr , gpr , gpr
 * gpr mpsull( gpr, gpr);
 */
int
test_fpx_mpsull(int a, unsigned b) {
  return __builtin_fpx_mpsull(a, b);
}

/*  mpushh gpr , gpr , gpr
 * gpr mpushh( gpr, gpr);
 */
int
test_fpx_mpushh(unsigned a, int b) {
  return __builtin_fpx_mpushh(a, b);
}

/*  mpushl gpr , gpr , gpr
 * gpr mpushl( gpr, gpr);
 */
int
test_fpx_mpushl(unsigned a, int b) {
  return __builtin_fpx_mpushl(a, b);
}

/*  mpuslh gpr , gpr , gpr
 * gpr mpuslh( gpr, gpr);
 */
int
test_fpx_mpuslh(unsigned a, int b) {
  return __builtin_fpx_mpuslh(a, b);
}

/*  mpusll gpr , gpr , gpr
 * gpr mpusll( gpr, gpr);
 */
int
test_fpx_mpusll(unsigned a, int b) {
  return __builtin_fpx_mpusll(a, b);
}

/*  mpuuhh gpr , gpr , gpr
 * gpr mpuuhh( gpr, gpr);
 */
int
test_fpx_mpuuhh(unsigned a, unsigned b) {
  return __builtin_fpx_mpuuhh(a, b);
}

/*  mpuuhl gpr , gpr , gpr
 * gpr mpuuhl( gpr, gpr);
 */
int
test_fpx_mpuuhl(unsigned a, unsigned b) {
  return __builtin_fpx_mpuuhl(a, b);
}

/*  mpuulh gpr , gpr , gpr
 * gpr mpuulh( gpr, gpr);
 */
int
test_fpx_mpuulh(unsigned a, unsigned b) {
  return __builtin_fpx_mpuulh(a, b);
}

/*  mpuull gpr , gpr , gpr
 * gpr mpuull( gpr, gpr);
 */
int
test_fpx_mpuull(unsigned a, unsigned b) {
  return __builtin_fpx_mpuull(a, b);
}

/*  mp gpr , gpr , gpr
 * gpr mp( gpr, gpr);
 */
int
test_fpx_mp(int a, int b) {
  return __builtin_fpx_mp(a, b);
}

/*  absbp gpr, gpr
 * gpr absbp( gpr);
 */
unsigned
test_sx_absbp(int a) {
  return __builtin_sx_absbp(a);
}

/*  abshp gpr, gpr
 * gpr abshp( gpr);
 */
unsigned
test_sx_abshp(int a) {
  return __builtin_sx_abshp(a);
}

/*  absubp gpr, gpr
 * gpr absubp( gpr);
 */
unsigned
test_sx_absubp(int a) {
  return __builtin_sx_absubp(a);
}

/*  absuhp gpr, gpr
 * gpr absuhp( gpr);
 */
unsigned
test_sx_absuhp(int a) {
  return __builtin_sx_absuhp(a);
}

/*  absu gpr, gpr
 * gpr absu( gpr);
 */
unsigned
test_sx_absu(int a) {
  return __builtin_sx_absu(a);
}

/*  abs gpr, gpr
 * gpr abs( gpr);
 */
unsigned
test_sx_abs(int a) {
  return __builtin_sx_abs(a);
}

/*  addbp gpr, gpr, gpr
 * gpr addbp( gpr, gpr);
 */
int
test_sx_addbp_r(int a, int b) {
  return __builtin_sx_addbp_r(a, b);
}

/*  addbp gpr, gpr, <u8>
 * gpr addbp( gpr, <u8>);
 */
int
test_sx_addbp_i8(int a) {
  return __builtin_sx_addbp_i8(a, 252);
}

/*  addcu gpr, gpr, gpr
 * gpr addcu( gpr, gpr);
 */
unsigned
test_sx_addcu(unsigned a, unsigned b) {
  return __builtin_sx_addcu(a, b);
}

/*  addc gpr, gpr, gpr
 * gpr addc( gpr, gpr);
 */
int
test_sx_addc(int a, int b) {
  return __builtin_sx_addc(a, b);
}

/*  addhp gpr, gpr, gpr
 * gpr addhp( gpr, gpr);
 */
int
test_sx_addhp_r(int a, int b) {
  return __builtin_sx_addhp_r(a, b);
}

/*  addhp gpr, gpr, <u8>
 * gpr addhp( gpr, <u8>);
 */
int
test_sx_addhp_i8(int a) {
  return __builtin_sx_addhp_i8(a, 252);
}

/*  addubp gpr, gpr, gpr
 * gpr addubp( gpr, gpr);
 */
unsigned
test_sx_addubp_r(unsigned a, unsigned b) {
  return __builtin_sx_addubp_r(a, b);
}

/*  addubp gpr, gpr, <u8>
 * gpr addubp( gpr, <u8>);
 */
unsigned
test_sx_addubp_i8(unsigned a) {
  return __builtin_sx_addubp_i8(a, 252);
}

/*  addugp gpr, <u16>
 * gpr addugp( <u16>);
 */
unsigned
test_sx_addugp() {
  return __builtin_sx_addugp(65532);
}

/*  adduhp gpr, gpr, gpr
 * gpr adduhp( gpr, gpr);
 */
unsigned
test_sx_adduhp_r(unsigned a, unsigned b) {
  return __builtin_sx_adduhp_r(a, b);
}

/*  adduhp gpr, gpr, <u8>
 * gpr adduhp( gpr, <u8>);
 */
unsigned
test_sx_adduhp_i8(unsigned a) {
  return __builtin_sx_adduhp_i8(a, 252);
}

/*  addur gpr, gpr
 * gpr addur( gpr);
 */
int
test_sx_addur_r(int a) {
  return __builtin_sx_addur_r(a);
}

/*  addur gpr, <s16>
 * gpr addur( <s16>);
 */
int
test_sx_addur_i16() {
  return __builtin_sx_addur_i16(65532);
}

/*  addu gpr, gpr, gpr
 * gpr addu( gpr, gpr);
 */
unsigned
test_sx_addu_r(unsigned a, unsigned b) {
  return __builtin_sx_addu_r(a, b);
}

/*  addu gpr, gpr, <u8>
 * gpr addu( gpr, <u8>);
 */
unsigned
test_sx_addu_i8(unsigned a) {
  return __builtin_sx_addu_i8(a, 252);
}

/*  add gpr, gpr, gpr
 * gpr add( gpr, gpr);
 */
int
test_sx_add_r(int a, int b) {
  return __builtin_sx_add_r(a, b);
}

/*  add gpr, gpr, <u8>
 * gpr add( gpr, <u8>);
 */
int
test_sx_add_i8(int a) {
  return __builtin_sx_add_i8(a, 252);
}

/*  andg ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst andg( gr, gr);
 */
int
test_sx_andg(int a, int b) {
  return __builtin_sx_andg(a, b);
}

/*  andng ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst andng( gr, gr);
 */
int
test_sx_andng(int a, int b) {
  return __builtin_sx_andng(a, b);
}

/*  andn gpr, gpr, gpr
 * gpr andn( gpr, gpr);
 */
int
test_sx_andn(int a, int b) {
  return __builtin_sx_andn(a, b);
}

/*  and gpr, gpr, gpr
 * gpr and( gpr, gpr);
 */
int
test_sx_and_r(int a, int b) {
  return __builtin_sx_and_r(a, b);
}

/*  and gpr, gpr, <u8>
 * gpr and( gpr, <u8>);
 */
int
test_sx_and_i8(int a) {
  return __builtin_sx_and_i8(a, 252);
}

/* barrier
 * barrier( );
 */
void
test_sx_barrier(void) {
  __builtin_sx_barrier();
}

/*  bclr gpr, gpr, gpr
 * gpr bclr( gpr, gpr);
 */
int
test_sx_bclr_r(int a, int b) {
  return __builtin_sx_bclr_r(a, b);
}

/*  bclr gpr, gpr, <u5>
 * gpr bclr( gpr, <u5>);
 */
int
test_sx_bclr_i5(int a) {
  return __builtin_sx_bclr_i5(a, 28);
}

/* bkp
 * bkp( );
 */
void
test_sx_bkp(void) {
  __builtin_sx_bkp();
}

/*  bnot gpr, gpr, gpr
 * gpr bnot( gpr, gpr);
 */
int
test_sx_bnot_r(int a, int b) {
  return __builtin_sx_bnot_r(a, b);
}

/*  bnot gpr, gpr, <u5>
 * gpr bnot( gpr, <u5>);
 */
int
test_sx_bnot_i5(int a) {
  return __builtin_sx_bnot_i5(a, 28);
}

/*  boolbp gpr, gr
 * gpr boolbp( gr);
 */
int
test_sx_boolbp(int a) {
  return __builtin_sx_boolbp(a);
}

/*  boolhp gpr, gr
 * gpr boolhp( gr);
 */
int
test_sx_boolhp(int a) {
  return __builtin_sx_boolhp(a);
}

/*  bool gpr, gr
 * gpr bool( gr);
 */
int
test_sx_bool(int a) {
  return __builtin_sx_bool(a);
}

/*  bset gpr, gpr, gpr
 * gpr bset( gpr, gpr);
 */
int
test_sx_bset_r(int a, int b) {
  return __builtin_sx_bset_r(a, b);
}

/*  bset gpr, gpr, <u5>
 * gpr bset( gpr, <u5>);
 */
int
test_sx_bset_i5(int a) {
  return __builtin_sx_bset_i5(a, 28);
}

/*  clamph gpr, gpr
 * gpr clamph( gpr);
 */
int
test_sx_clamph(int a) {
  return __builtin_sx_clamph(a);
}

/*  extb gpr, gpr
 * gpr extb( gpr);
 */
int
test_sx_extb(int a) {
  return __builtin_sx_extb(a);
}

/*  exth gpr, gpr
 * gpr exth( gpr);
 */
int
test_sx_exth(int a) {
  return __builtin_sx_exth(a);
}

/*  extub gpr, gpr
 * gpr extub( gpr);
 */
unsigned
test_sx_extub(int a) {
  return __builtin_sx_extub(a);
}

/*  extuh gpr, gpr
 * gpr extuh( gpr);
 */
unsigned
test_sx_extuh(int a) {
  return __builtin_sx_extuh(a);
}

/*  idle <u2>
 * idle( <u2>);
 */
void
test_sx_idle() {
  __builtin_sx_idle(0);
}

/* loopdis lr
 * lr loopdis( u[0..1]);
 */
void
test_sx_loopdis() {
  __builtin_sx_loopdis(1);
}

/* loopena lr
 * lr loopena( u[0..1]);
 */
void
test_sx_loopena() {
  __builtin_sx_loopena(1);
}

/*  lzc gpr, gpr
 * gpr lzc( gpr);
 */
int
test_sx_lzc(int a) {
  return __builtin_sx_lzc(a);
}

/*  makehp gpr, <s16>
 * gpr makehp( <s16>);
 */
int
test_sx_makehp() {
  return __builtin_sx_makehp(65532);
}

/*  make gpr, <s16>
 * gpr make( <s16>);
 */
int
test_sx_make() {
  return __builtin_sx_make(65532);
}

/*  maxbp gpr, gpr, gpr
 * gpr maxbp( gpr, gpr);
 */
int
test_sx_maxbp_r(int a, int b) {
  return __builtin_sx_maxbp_r(a, b);
}

/*  maxbp gpr, gpr, <u8>
 * gpr maxbp( gpr, <u8>);
 */
int
test_sx_maxbp_i8(int a) {
  return __builtin_sx_maxbp_i8(a, 252);
}

/*  maxhp gpr, gpr, gpr
 * gpr maxhp( gpr, gpr);
 */
int
test_sx_maxhp_r(int a, int b) {
  return __builtin_sx_maxhp_r(a, b);
}

/*  maxhp gpr, gpr, <u8>
 * gpr maxhp( gpr, <u8>);
 */
int
test_sx_maxhp_i8(int a) {
  return __builtin_sx_maxhp_i8(a, 252);
}

/*  maxubp gpr, gpr, gpr
 * gpr maxubp( gpr, gpr);
 */
unsigned
test_sx_maxubp_r(unsigned a, unsigned b) {
  return __builtin_sx_maxubp_r(a, b);
}

/*  maxubp gpr, gpr, <u8>
 * gpr maxubp( gpr, <u8>);
 */
unsigned
test_sx_maxubp_i8(unsigned a) {
  return __builtin_sx_maxubp_i8(a, 252);
}

/*  maxuhp gpr, gpr, gpr
 * gpr maxuhp( gpr, gpr);
 */
unsigned
test_sx_maxuhp_r(unsigned a, unsigned b) {
  return __builtin_sx_maxuhp_r(a, b);
}

/*  maxuhp gpr, gpr, <u8>
 * gpr maxuhp( gpr, <u8>);
 */
unsigned
test_sx_maxuhp_i8(unsigned a) {
  return __builtin_sx_maxuhp_i8(a, 252);
}

/*  maxu gpr, gpr, gpr
 * gpr maxu( gpr, gpr);
 */
unsigned
test_sx_maxu_r(unsigned a, unsigned b) {
  return __builtin_sx_maxu_r(a, b);
}

/*  maxu gpr, gpr, <u8>
 * gpr maxu( gpr, <u8>);
 */
unsigned
test_sx_maxu_i8(unsigned a) {
  return __builtin_sx_maxu_i8(a, 252);
}

/*  max gpr, gpr, gpr
 * gpr max( gpr, gpr);
 */
int
test_sx_max_r(int a, int b) {
  return __builtin_sx_max_r(a, b);
}

/*  max gpr, gpr, <u8>
 * gpr max( gpr, <u8>);
 */
int
test_sx_max_i8(int a) {
  return __builtin_sx_max_i8(a, 252);
}

/*  minbp gpr, gpr, gpr
 * gpr minbp( gpr, gpr);
 */
int
test_sx_minbp_r(int a, int b) {
  return __builtin_sx_minbp_r(a, b);
}

/*  minbp gpr, gpr, <u8>
 * gpr minbp( gpr, <u8>);
 */
int
test_sx_minbp_i8(int a) {
  return __builtin_sx_minbp_i8(a, 252);
}

/*  minhp gpr, gpr, gpr
 * gpr minhp( gpr, gpr);
 */
int
test_sx_minhp_r(int a, int b) {
  return __builtin_sx_minhp_r(a, b);
}

/*  minhp gpr, gpr, <u8>
 * gpr minhp( gpr, <u8>);
 */
int
test_sx_minhp_i8(int a) {
  return __builtin_sx_minhp_i8(a, 252);
}

/*  minubp gpr, gpr, gpr
 * gpr minubp( gpr, gpr);
 */
unsigned
test_sx_minubp_r(unsigned a, unsigned b) {
  return __builtin_sx_minubp_r(a, b);
}

/*  minubp gpr, gpr, <u8>
 * gpr minubp( gpr, <u8>);
 */
unsigned
test_sx_minubp_i8(unsigned a) {
  return __builtin_sx_minubp_i8(a, 252);
}

/*  minuhp gpr, gpr, gpr
 * gpr minuhp( gpr, gpr);
 */
unsigned
test_sx_minuhp_r(unsigned a, unsigned b) {
  return __builtin_sx_minuhp_r(a, b);
}

/*  minuhp gpr, gpr, <u8>
 * gpr minuhp( gpr, <u8>);
 */
unsigned
test_sx_minuhp_i8(unsigned a) {
  return __builtin_sx_minuhp_i8(a, 252);
}

/*  minu gpr, gpr, gpr
 * gpr minu( gpr, gpr);
 */
unsigned
test_sx_minu_r(unsigned a, unsigned b) {
  return __builtin_sx_minu_r(a, b);
}

/*  minu gpr, gpr, <u8>
 * gpr minu( gpr, <u8>);
 */
unsigned
test_sx_minu_i8(unsigned a) {
  return __builtin_sx_minu_i8(a, 252);
}

/*  min gpr, gpr, gpr
 * gpr min( gpr, gpr);
 */
int
test_sx_min_r(int a, int b) {
  return __builtin_sx_min_r(a, b);
}

/*  min gpr, gpr, <u8>
 * gpr min( gpr, <u8>);
 */
int
test_sx_min_i8(int a) {
  return __builtin_sx_min_i8(a, 252);
}

/*  more gpr, <u16>
 * gpr more( gpr, <u16>);
 */
int
test_sx_more(int a) {
  return __builtin_sx_more(a, 65532);
}

/*  moveh2h gpr, gpr
 * gpr moveh2h( gpr, gpr);
 */
int
test_sx_moveh2h(int a, int b) {
  return __builtin_sx_moveh2h(a, b);
}

/*  moveh2l gpr, gpr
 * gpr moveh2l( gpr, gpr);
 */
int
test_sx_moveh2l(int a, int b) {
  return __builtin_sx_moveh2l(a, b);
}

/*  movel2h gpr, gpr
 * gpr movel2h( gpr, gpr);
 */
int
test_sx_movel2h(int a, int b) {
  return __builtin_sx_movel2h(a, b);
}

/*  movel2l gpr, gpr
 * gpr movel2l( gpr, gpr);
 */
int
test_sx_movel2l(int a, int b) {
  return __builtin_sx_movel2l(a, b);
}

/*  mover2sfr sfr, gpr
 * sfr mover2sfr( u[0..31], gpr);
 */
void
test_sx_mover2sfr(int a) {
  __builtin_sx_mover2sfr(31, a);
}

/*  movesfr2r gpr, sfr
 * gpr movesfr2r( u[0..31]);
 */
int
test_sx_movesfr2r() {
  return __builtin_sx_movesfr2r(31);
}

/*  negbp gpr, gpr
 * gpr negbp( gpr);
 */
int
test_sx_negbp(int a) {
  return __builtin_sx_negbp(a);
}

/*  neghp gpr, gpr
 * gpr neghp( gpr);
 */
int
test_sx_neghp(int a) {
  return __builtin_sx_neghp(a);
}

/*  negubp gpr, gpr
 * gpr negubp( gpr);
 */
int
test_sx_negubp(int a) {
  return __builtin_sx_negubp(a);
}

/*  neguhp gpr, gpr
 * gpr neguhp( gpr);
 */
int
test_sx_neguhp(int a) {
  return __builtin_sx_neguhp(a);
}

/*  negu gpr, gpr
 * gpr negu( gpr);
 */
int
test_sx_negu(int a) {
  return __builtin_sx_negu(a);
}

/*  neg gpr, gpr
 * gpr neg( gpr);
 */
int
test_sx_neg(int a) {
  return __builtin_sx_neg(a);
}

/* nop
 * nop( );
 */
void
test_sx_nop(void) {
  __builtin_sx_nop();
}

/*  norn gpr, gpr, <u8>
 * gpr norn( gpr, <u8>);
 */
int
test_sx_norn(int a) {
  return __builtin_sx_norn(a, 252);
}

/*  not gpr, gpr
 * gpr not( gpr);
 */
int
test_sx_not(int a) {
  return __builtin_sx_not(a);
}

/*  org ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst org( gr, gr);
 */
int
test_sx_org(int a, int b) {
  return __builtin_sx_org(a, b);
}

/*  or gpr, gpr, gpr
 * gpr or( gpr, gpr);
 */
int
test_sx_or_r(int a, int b) {
  return __builtin_sx_or_r(a, b);
}

/*  or gpr, gpr, <u8>
 * gpr or( gpr, <u8>);
 */
int
test_sx_or_i8(int a) {
  return __builtin_sx_or_i8(a, 252);
}

/*  prior gpr, gpr
 * gpr prior( gpr);
 */
int
test_sx_prior(int a) {
  return __builtin_sx_prior(a);
}

/*  rotl gpr, gpr, gpr
 * gpr rotl( gpr, gpr);
 */
int
test_sx_rotl_r(int a, int b) {
  return __builtin_sx_rotl_r(a, b);
}

/*  rotl gpr, gpr, <u5>
 * gpr rotl( gpr, <u5>);
 */
int
test_sx_rotl_i5(int a) {
  return __builtin_sx_rotl_i5(a, 28);
}

/* rte
 * rte( );
 */
void
test_sx_rte(void) {
  __builtin_sx_rte();
}

/*  shlmu gpr, gpr
 * gpr shlmu( gpr);
 */
unsigned
test_sx_shlmu_r(unsigned a) {
  return __builtin_sx_shlmu_r(a);
}

/*  shlmu gpr, <u5>
 * gpr shlmu( <u5>);
 */
unsigned
test_sx_shlmu_i5() {
  return __builtin_sx_shlmu_i5(28);
}

/*  shlnu gpr, gpr, gpr
 * gpr shlnu( gpr, gpr);
 */
unsigned
test_sx_shlnu(unsigned a, unsigned b) {
  return __builtin_sx_shlnu(a, b);
}

/*  shlu gpr, gpr, gpr
 * gpr shlu( gpr, gpr);
 */
unsigned
test_sx_shlu_r(unsigned a, unsigned b) {
  return __builtin_sx_shlu_r(a, b);
}

/*  shlu gpr, gpr, <u5>
 * gpr shlu( gpr, <u5>);
 */
unsigned
test_sx_shlu_i5(unsigned a) {
  return __builtin_sx_shlu_i5(a, 28);
}

/*  shl gpr, gpr, gpr
 * gpr shl( gpr, gpr);
 */
int
test_sx_shl_r(int a, int b) {
  return __builtin_sx_shl_r(a, b);
}

/*  shl gpr, gpr, <u5>
 * gpr shl( gpr, <u5>);
 */
int
test_sx_shl_i5(int a) {
  return __builtin_sx_shl_i5(a, 28);
}

/*  shrmu gpr, gpr
 * gpr shrmu( gpr);
 */
unsigned
test_sx_shrmu_r(unsigned a) {
  return __builtin_sx_shrmu_r(a);
}

/*  shrmu gpr, <u5>
 * gpr shrmu( <u5>);
 */
unsigned
test_sx_shrmu_i5() {
  return __builtin_sx_shrmu_i5(28);
}

/*  shrnu gpr, gpr, gpr
 * gpr shrnu( gpr, gpr);
 */
unsigned
test_sx_shrnu(unsigned a, unsigned b) {
  return __builtin_sx_shrnu(a, b);
}

/*  shru gpr, gpr, gpr
 * gpr shru( gpr, gpr);
 */
unsigned
test_sx_shru_r(unsigned a, unsigned b) {
  return __builtin_sx_shru_r(a, b);
}

/*  shru gpr, gpr, <u5>
 * gpr shru( gpr, <u5>);
 */
unsigned
test_sx_shru_i5(unsigned a) {
  return __builtin_sx_shru_i5(a, 28);
}

/*  shr gpr, gpr, gpr
 * gpr shr( gpr, gpr);
 */
int
test_sx_shr_r(int a, int b) {
  return __builtin_sx_shr_r(a, b);
}

/*  shr gpr, gpr, <u5>
 * gpr shr( gpr, <u5>);
 */
int
test_sx_shr_i5(int a) {
  return __builtin_sx_shr_i5(a, 28);
}

/*  subbp gpr, gpr, gpr
 * gpr subbp( gpr, gpr);
 */
int
test_sx_subbp_r(int a, int b) {
  return __builtin_sx_subbp_r(a, b);
}

/*  subbp gpr, gpr, <u8>
 * gpr subbp( gpr, <u8>);
 */
int
test_sx_subbp_i8(int a) {
  return __builtin_sx_subbp_i8(a, 252);
}

/*  subcu gpr, gpr, gpr
 * gpr subcu( gpr, gpr);
 */
unsigned
test_sx_subcu(unsigned a, unsigned b) {
  return __builtin_sx_subcu(a, b);
}

/*  subc gpr, gpr, gpr
 * gpr subc( gpr, gpr);
 */
int
test_sx_subc(int a, int b) {
  return __builtin_sx_subc(a, b);
}

/*  subhp gpr, gpr, gpr
 * gpr subhp( gpr, gpr);
 */
int
test_sx_subhp_r(int a, int b) {
  return __builtin_sx_subhp_r(a, b);
}

/*  subhp gpr, gpr, <u8>
 * gpr subhp( gpr, <u8>);
 */
int
test_sx_subhp_i8(int a) {
  return __builtin_sx_subhp_i8(a, 252);
}

/*  sububp gpr, gpr, gpr
 * gpr sububp( gpr, gpr);
 */
int
test_sx_sububp_r(int a, int b) {
  return __builtin_sx_sububp_r(a, b);
}

/*  sububp gpr, gpr, <u8>
 * gpr sububp( gpr, <u8>);
 */
int
test_sx_sububp_i8(int a) {
  return __builtin_sx_sububp_i8(a, 252);
}

/*  subuhp gpr, gpr, gpr
 * gpr subuhp( gpr, gpr);
 */
int
test_sx_subuhp_r(int a, int b) {
  return __builtin_sx_subuhp_r(a, b);
}

/*  subuhp gpr, gpr, <u8>
 * gpr subuhp( gpr, <u8>);
 */
int
test_sx_subuhp_i8(int a) {
  return __builtin_sx_subuhp_i8(a, 252);
}

/*  subu gpr, gpr, gpr
 * gpr subu( gpr, gpr);
 */
unsigned
test_sx_subu_r(unsigned a, unsigned b) {
  return __builtin_sx_subu_r(a, b);
}

/*  subu gpr, gpr, <u8>
 * gpr subu( gpr, <u8>);
 */
unsigned
test_sx_subu_i8(unsigned a) {
  return __builtin_sx_subu_i8(a, 252);
}

/*  sub gpr, gpr, gpr
 * gpr sub( gpr, gpr);
 */
int
test_sx_sub_r(int a, int b) {
  return __builtin_sx_sub_r(a, b);
}

/*  sub gpr, gpr, <u8>
 * gpr sub( gpr, <u8>);
 */
int
test_sx_sub_i8(int a) {
  return __builtin_sx_sub_i8(a, 252);
}

/*  tfandn ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst tfandn( gpr, <u8>);
 */
int
test_sx_tfandn(int a) {
  return __builtin_sx_tfandn(a, 252);
}

/*  tfand ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst tfand( gpr, <u8>);
 */
int
test_sx_tfand(int a) {
  return __builtin_sx_tfand(a, 252);
}

/*  tfbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, gpr
 * ISA_REGISTER_SUBCLASS_gr_dst tfbit( gpr, gpr);
 */
int
test_sx_tfbit_r(int a, int b) {
  return __builtin_sx_tfbit_r(a, b);
}

/*  tfbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u5>
 * ISA_REGISTER_SUBCLASS_gr_dst tfbit( gpr, <u5>);
 */
int
test_sx_tfbit_i5(int a) {
  return __builtin_sx_tfbit_i5(a, 28);
}

/* trap <u2>
 * trap( <u2>);
 */
void
test_sx_trap() {
  __builtin_sx_trap(0);
}

/*  ttandn ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst ttandn( gpr, <u8>);
 */
int
test_sx_ttandn(int a) {
  return __builtin_sx_ttandn(a, 252);
}

/*  ttand ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u8>
 * ISA_REGISTER_SUBCLASS_gr_dst ttand( gpr, <u8>);
 */
int
test_sx_ttand(int a) {
  return __builtin_sx_ttand(a, 252);
}

/*  ttbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, gpr
 * ISA_REGISTER_SUBCLASS_gr_dst ttbit( gpr, gpr);
 */
int
test_sx_ttbit_r(int a, int b) {
  return __builtin_sx_ttbit_r(a, b);
}

/*  ttbit ISA_REGISTER_SUBCLASS_gr_dst, gpr, <u5>
 * ISA_REGISTER_SUBCLASS_gr_dst ttbit( gpr, <u5>);
 */
int
test_sx_ttbit_i5(int a) {
  return __builtin_sx_ttbit_i5(a, 28);
}

/*  xorg ISA_REGISTER_SUBCLASS_gr_dst, gr, gr
 * ISA_REGISTER_SUBCLASS_gr_dst xorg( gr, gr);
 */
int
test_sx_xorg(int a, int b) {
  return __builtin_sx_xorg(a, b);
}

/*  xor gpr, gpr, gpr
 * gpr xor( gpr, gpr);
 */
int
test_sx_xor_r(int a, int b) {
  return __builtin_sx_xor_r(a, b);
}

/*  xor gpr, gpr, <u8>
 * gpr xor( gpr, <u8>);
 */
int
test_sx_xor_i8(int a) {
  return __builtin_sx_xor_i8(a, 252);
}

/*  cancelg gpr
 * cancelg( gpr);
 */
void
test_x3_cancelg_r_g(int a) {
  __builtin_x3_cancelg_r_g(a);
}

/*  cancelg B<u2>, <u8>
 * cancelg( <u2>, <u8>);
 */
void
test_x3_cancelg_i8_i2_g() {
  __builtin_x3_cancelg_i8_i2_g(0, 252);
}

/*  cancell gpr
 * cancell( gpr);
 */
void
test_x3_cancell_r_g(int a) {
  __builtin_x3_cancell_r_g(a);
}

/*  cancell B<u2>, <u8>
 * cancell( <u2>, <u8>);
 */
void
test_x3_cancell_i8_i2_g() {
  __builtin_x3_cancell_i8_i2_g(0, 252);
}

/* clrcc1
 * clrcc1( );
 */
void
test_x3_clrcc1(void) {
  __builtin_x3_clrcc1();
}

/* clrcca
 * clrcca( );
 */
void
test_x3_clrcca(void) {
  __builtin_x3_clrcca();
}

/* clrcc
 * clrcc( );
 */
void
test_x3_clrcc(void) {
  __builtin_x3_clrcc();
}

/*  clric itcr, gpr
 * itcr clric( u[0..31], gpr);
 */
void
test_x3_clric(int a) {
  __builtin_x3_clric(31, a);
}

/*  clrie gpr
 * gpr clrie( );
 */
int
test_x3_clrie(void) {
  return __builtin_x3_clrie();
}

/* dflushw
 * dflushw( );
 */
void
test_x3_dflushw(void) {
  __builtin_x3_dflushw();
}

/* dflush gpr
 * dflush( gpr);
 */
void
test_x3_dflush(int a) {
  __builtin_x3_dflush(a);
}

/* dinval gpr
 * dinval( gpr);
 */
void
test_x3_dinval(int a) {
  __builtin_x3_dinval(a);
}

/* dlock gpr
 * dlock( gpr);
 */
void
test_x3_dlock(int a) {
  __builtin_x3_dlock(a);
}

/* dstata gpr
 * dstata( gpr);
 */
void
test_x3_dstata(int a) {
  __builtin_x3_dstata(a);
}

/* dstatwi gpr
 * dstatwi( gpr);
 */
void
test_x3_dstatwi(int a) {
  __builtin_x3_dstatwi(a);
}

/* dtouch gpr
 * dtouch( gpr);
 */
void
test_x3_dtouch(int a) {
  __builtin_x3_dtouch(a);
}

/*  fork<u3> gpr, gpr, <u1>
 * fork( <u3>, gpr, gpr, <u1>);
 */
void
test_x3_fork_i3_g(int a, int b) {
  __builtin_x3_fork_i3_g(4, a, b, 0);
}

/*  fork gpr, gpr, <u1>
 * fork( gpr, gpr, <u1>);
 */
void
test_x3_fork_g(int a, int b) {
  __builtin_x3_fork_g(a, b, 0);
}

/*  movee2r gpr, gecr
 * gpr movee2r( u[0..31]);
 */
int
test_x3_movee2r() {
  return __builtin_x3_movee2r(31);
}

/*  moveic2ri gpr, gpr
 * gpr moveic2ri( gpr);
 */
int
test_x3_moveic2ri(int a) {
  return __builtin_x3_moveic2ri(a);
}

/*  moveic2r gpr, itcr
 * gpr moveic2r( u[0..31]);
 */
int
test_x3_moveic2r() {
  return __builtin_x3_moveic2r(31);
}

/*  moveie2r gpr
 * gpr moveie2r( );
 */
int
test_x3_moveie2r(void) {
  return __builtin_x3_moveie2r();
}

/*  mover2e gecr, gpr
 * gecr mover2e( u[0..31], gpr);
 */
void
test_x3_mover2e(int a) {
  __builtin_x3_mover2e(31, a);
}

/*  mover2ici gpr, gpr
 * mover2ici( gpr, gpr);
 */
void
test_x3_mover2ici(int a, int b) {
  __builtin_x3_mover2ici(a, b);
}

/*  mover2ic itcr, gpr
 * itcr mover2ic( u[0..31], gpr);
 */
void
test_x3_mover2ic(int a) {
  __builtin_x3_mover2ic(31, a);
}

/*  mover2ie gpr
 * mover2ie( gpr);
 */
void
test_x3_mover2ie(int a) {
  __builtin_x3_mover2ie(a);
}

/*  mpnsshh gpr, gpr, gpr
 * gpr mpnsshh( gpr, gpr);
 */
int
test_x3_mpnsshh(int a, int b) {
  return __builtin_x3_mpnsshh(a, b);
}

/*  mpnsshl gpr, gpr, gpr
 * gpr mpnsshl( gpr, gpr);
 */
int
test_x3_mpnsshl(int a, int b) {
  return __builtin_x3_mpnsshl(a, b);
}

/*  mpnsslh gpr, gpr, gpr
 * gpr mpnsslh( gpr, gpr);
 */
int
test_x3_mpnsslh(int a, int b) {
  return __builtin_x3_mpnsslh(a, b);
}

/*  mpnssll gpr, gpr, gpr
 * gpr mpnssll( gpr, gpr);
 */
int
test_x3_mpnssll(int a, int b) {
  return __builtin_x3_mpnssll(a, b);
}

/*  mpnsuhh gpr, gpr, gpr
 * gpr mpnsuhh( gpr, gpr);
 */
int
test_x3_mpnsuhh(int a, unsigned b) {
  return __builtin_x3_mpnsuhh(a, b);
}

/*  mpnsuhl gpr, gpr, gpr
 * gpr mpnsuhl( gpr, gpr);
 */
int
test_x3_mpnsuhl(int a, unsigned b) {
  return __builtin_x3_mpnsuhl(a, b);
}

/*  mpnsulh gpr, gpr, gpr
 * gpr mpnsulh( gpr, gpr);
 */
int
test_x3_mpnsulh(int a, unsigned b) {
  return __builtin_x3_mpnsulh(a, b);
}

/*  mpnsull gpr, gpr, gpr
 * gpr mpnsull( gpr, gpr);
 */
int
test_x3_mpnsull(int a, unsigned b) {
  return __builtin_x3_mpnsull(a, b);
}

/*  mpnushh gpr, gpr, gpr
 * gpr mpnushh( gpr, gpr);
 */
int
test_x3_mpnushh(unsigned a, int b) {
  return __builtin_x3_mpnushh(a, b);
}

/*  mpnushl gpr, gpr, gpr
 * gpr mpnushl( gpr, gpr);
 */
int
test_x3_mpnushl(unsigned a, int b) {
  return __builtin_x3_mpnushl(a, b);
}

/*  mpnuslh gpr, gpr, gpr
 * gpr mpnuslh( gpr, gpr);
 */
int
test_x3_mpnuslh(unsigned a, int b) {
  return __builtin_x3_mpnuslh(a, b);
}

/*  mpnusll gpr, gpr, gpr
 * gpr mpnusll( gpr, gpr);
 */
int
test_x3_mpnusll(unsigned a, int b) {
  return __builtin_x3_mpnusll(a, b);
}

/*  mpnuuhh gpr, gpr, gpr
 * gpr mpnuuhh( gpr, gpr);
 */
int
test_x3_mpnuuhh(unsigned a, unsigned b) {
  return __builtin_x3_mpnuuhh(a, b);
}

/*  mpnuuhl gpr, gpr, gpr
 * gpr mpnuuhl( gpr, gpr);
 */
int
test_x3_mpnuuhl(unsigned a, unsigned b) {
  return __builtin_x3_mpnuuhl(a, b);
}

/*  mpnuulh gpr, gpr, gpr
 * gpr mpnuulh( gpr, gpr);
 */
int
test_x3_mpnuulh(unsigned a, unsigned b) {
  return __builtin_x3_mpnuulh(a, b);
}

/*  mpnuull gpr, gpr, gpr
 * gpr mpnuull( gpr, gpr);
 */
int
test_x3_mpnuull(unsigned a, unsigned b) {
  return __builtin_x3_mpnuull(a, b);
}

/*  mpn gpr, gpr, gpr
 * gpr mpn( gpr, gpr);
 */
int
test_x3_mpn(int a, int b) {
  return __builtin_x3_mpn(a, b);
}

/*  mpsshh gpr, gpr, gpr
 * gpr mpsshh( gpr, gpr);
 */
int
test_x3_mpsshh(int a, int b) {
  return __builtin_x3_mpsshh(a, b);
}

/*  mpsshl gpr, gpr, gpr
 * gpr mpsshl( gpr, gpr);
 */
int
test_x3_mpsshl(int a, int b) {
  return __builtin_x3_mpsshl(a, b);
}

/*  mpsslh gpr, gpr, gpr
 * gpr mpsslh( gpr, gpr);
 */
int
test_x3_mpsslh(int a, int b) {
  return __builtin_x3_mpsslh(a, b);
}

/*  mpssll gpr, gpr, gpr
 * gpr mpssll( gpr, gpr);
 */
int
test_x3_mpssll(int a, int b) {
  return __builtin_x3_mpssll(a, b);
}

/*  mpsuhh gpr, gpr, gpr
 * gpr mpsuhh( gpr, gpr);
 */
int
test_x3_mpsuhh(int a, unsigned b) {
  return __builtin_x3_mpsuhh(a, b);
}

/*  mpsuhl gpr, gpr, gpr
 * gpr mpsuhl( gpr, gpr);
 */
int
test_x3_mpsuhl(int a, unsigned b) {
  return __builtin_x3_mpsuhl(a, b);
}

/*  mpsulh gpr, gpr, gpr
 * gpr mpsulh( gpr, gpr);
 */
int
test_x3_mpsulh(int a, unsigned b) {
  return __builtin_x3_mpsulh(a, b);
}

/*  mpsull gpr, gpr, gpr
 * gpr mpsull( gpr, gpr);
 */
int
test_x3_mpsull(int a, unsigned b) {
  return __builtin_x3_mpsull(a, b);
}

/*  mpushh gpr, gpr, gpr
 * gpr mpushh( gpr, gpr);
 */
int
test_x3_mpushh(unsigned a, int b) {
  return __builtin_x3_mpushh(a, b);
}

/*  mpushl gpr, gpr, gpr
 * gpr mpushl( gpr, gpr);
 */
int
test_x3_mpushl(unsigned a, int b) {
  return __builtin_x3_mpushl(a, b);
}

/*  mpuslh gpr, gpr, gpr
 * gpr mpuslh( gpr, gpr);
 */
int
test_x3_mpuslh(unsigned a, int b) {
  return __builtin_x3_mpuslh(a, b);
}

/*  mpusll gpr, gpr, gpr
 * gpr mpusll( gpr, gpr);
 */
int
test_x3_mpusll(unsigned a, int b) {
  return __builtin_x3_mpusll(a, b);
}

/*  mpuuhh gpr, gpr, gpr
 * gpr mpuuhh( gpr, gpr);
 */
int
test_x3_mpuuhh(unsigned a, unsigned b) {
  return __builtin_x3_mpuuhh(a, b);
}

/*  mpuuhl gpr, gpr, gpr
 * gpr mpuuhl( gpr, gpr);
 */
int
test_x3_mpuuhl(unsigned a, unsigned b) {
  return __builtin_x3_mpuuhl(a, b);
}

/*  mpuulh gpr, gpr, gpr
 * gpr mpuulh( gpr, gpr);
 */
int
test_x3_mpuulh(unsigned a, unsigned b) {
  return __builtin_x3_mpuulh(a, b);
}

/*  mpuull gpr, gpr, gpr
 * gpr mpuull( gpr, gpr);
 */
int
test_x3_mpuull(unsigned a, unsigned b) {
  return __builtin_x3_mpuull(a, b);
}

/*  mp gpr, gpr, gpr
 * gpr mp( gpr, gpr);
 */
int
test_x3_mp(int a, int b) {
  return __builtin_x3_mp(a, b);
}

/*  notifyg gpr
 * notifyg( gpr);
 */
void
test_x3_notifyg_r_g(int a) {
  __builtin_x3_notifyg_r_g(a);
}

/*  notifyg B<u2>, <u8>
 * notifyg( <u2>, <u8>);
 */
void
test_x3_notifyg_i8_i2_g() {
  __builtin_x3_notifyg_i8_i2_g(0, 252);
}

/*  notifyla gpr
 * notifyla( gpr);
 */
void
test_x3_notifyla_r_g(int a) {
  __builtin_x3_notifyla_r_g(a);
}

/*  notifyla B<u2>, <u8>
 * notifyla( <u2>, <u8>);
 */
void
test_x3_notifyla_i8_i2_g() {
  __builtin_x3_notifyla_i8_i2_g(0, 252);
}

/*  notifyl<u3> gpr
 * notifyl( <u3>, gpr);
 */
void
test_x3_notifyl_r_i3_g(int a) {
  __builtin_x3_notifyl_r_i3_g(4, a);
}

/*  notifyl<u3> B<u2>, <u8>
 * notifyl( <u3>, <u2>, <u8>);
 */
void
test_x3_notifyl_i8_i2_i3_g() {
  __builtin_x3_notifyl_i8_i2_i3_g(4, 0, 252);
}

/* pflushw
 * pflushw( );
 */
void
test_x3_pflushw(void) {
  __builtin_x3_pflushw();
}

/* pinvalr gpr
 * pinvalr( gpr);
 */
void
test_x3_pinvalr(int a) {
  __builtin_x3_pinvalr(a);
}

/* pinval gpr
 * pinval( gpr);
 */
void
test_x3_pinval(int a) {
  __builtin_x3_pinval(a);
}

/* plockr gpr
 * plockr( gpr);
 */
void
test_x3_plockr(int a) {
  __builtin_x3_plockr(a);
}

/* plock gpr
 * plock( gpr);
 */
void
test_x3_plock(int a) {
  __builtin_x3_plock(a);
}

/* pstata gpr
 * pstata( gpr);
 */
void
test_x3_pstata(int a) {
  __builtin_x3_pstata(a);
}

/* pstatwi gpr
 * pstatwi( gpr);
 */
void
test_x3_pstatwi(int a) {
  __builtin_x3_pstatwi(a);
}

/* ptouch gpr
 * ptouch( gpr);
 */
void
test_x3_ptouch(int a) {
  __builtin_x3_ptouch(a);
}

/*  setic itcr, gpr
 * itcr setic( u[0..31], gpr);
 */
void
test_x3_setic(int a) {
  __builtin_x3_setic(31, a);
}

/*  setie gpr
 * setie( gpr);
 */
void
test_x3_setie(int a) {
  __builtin_x3_setie(a);
}

/* startcc1
 * startcc1( );
 */
void
test_x3_startcc1(void) {
  __builtin_x3_startcc1();
}

/* startcca
 * startcca( );
 */
void
test_x3_startcca(void) {
  __builtin_x3_startcca();
}

/* startcc
 * startcc( );
 */
void
test_x3_startcc(void) {
  __builtin_x3_startcc();
}

/* stopcc1
 * stopcc1( );
 */
void
test_x3_stopcc1(void) {
  __builtin_x3_stopcc1();
}

/* stopcca
 * stopcca( );
 */
void
test_x3_stopcca(void) {
  __builtin_x3_stopcca();
}

/* stopcc
 * stopcc( );
 */
void
test_x3_stopcc(void) {
  __builtin_x3_stopcc();
}

/* terminate<u3>
 * terminate( <u3>);
 */
void
test_x3_terminate_i3() {
  __builtin_x3_terminate_i3(4);
}

/* terminate
 * terminate( );
 */
void
test_x3_terminate(void) {
  __builtin_x3_terminate();
}

/*  waitg gpr
 * waitg( gpr);
 */
void
test_x3_waitg_r_g(int a) {
  __builtin_x3_waitg_r_g(a);
}

/*  waitg B<u2>, <u8>
 * waitg( <u2>, <u8>);
 */
void
test_x3_waitg_i8_i2_g() {
  __builtin_x3_waitg_i8_i2_g(0, 252);
}

/*  waitl gpr
 * waitl( gpr);
 */
void
test_x3_waitl_r_g(int a) {
  __builtin_x3_waitl_r_g(a);
}

/*  waitl B<u2>, <u8>
 * waitl( <u2>, <u8>);
 */
void
test_x3_waitl_i8_i2_g() {
  __builtin_x3_waitl_i8_i2_g(0, 252);
}

/*  waitnbg gpr
 * waitnbg( gpr);
 */
void
test_x3_waitnbg_r_g(int a) {
  __builtin_x3_waitnbg_r_g(a);
}

/*  waitnbg B<u2>, <u8>
 * waitnbg( <u2>, <u8>);
 */
void
test_x3_waitnbg_i8_i2_g() {
  __builtin_x3_waitnbg_i8_i2_g(0, 252);
}

/*  waitnbl gpr
 * waitnbl( gpr);
 */
void
test_x3_waitnbl_r_g(int a) {
  __builtin_x3_waitnbl_r_g(a);
}

/*  waitnbl B<u2>, <u8>
 * waitnbl( <u2>, <u8>);
 */
void
test_x3_waitnbl_i8_i2_g() {
  __builtin_x3_waitnbl_i8_i2_g(0, 252);
}

/*  waitnb gpr, gpr
 * waitnb( gpr, gpr);
 */
void
test_x3_waitnb(int a, int b) {
  __builtin_x3_waitnb(a, b);
}

/*  wait gpr, gpr
 * wait( gpr, gpr);
 */
void
test_x3_wait(int a, int b) {
  __builtin_x3_wait(a, b);
}

