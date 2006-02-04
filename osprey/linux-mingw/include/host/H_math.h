
#ifndef H_MATH_H
#define H_MATH_H

/* Under x-mingw32-gcc-3.4.2 math.h is bogus as it generates
 * multiple definitions (hypotf,sinhf).
 * We work-around it by defining __NO_ISOCEXT and redefining the missing
 * inputs.
 */
#if __GNUC__ == 3 && __GNUC_MINOR__ == 4
#define __NO_ISOCEXT
#endif

#include <math.h>

// [HK]
#if __GNUC__ < 3 || defined(__NO_ISOCEXT)

#ifndef expf
#define expf(x) ((float)(exp(x)))
#endif

#ifndef hypot
#if defined(__NO_ISOCEXT)
#define hypot(x,y) (_hypot(x,y))
#endif
#endif

#ifndef hypotf
#define hypotf(x,y) ((float)(hypot(x,y)))
#endif

#ifndef fabsf
#define fabsf(x) ((float)(fabs(x)))
#endif

#ifndef logf
#define logf(x) ((float)(log(x)))
#endif

#ifndef log10f
#define log10f(x) ((float)(log10(x)))
#endif

#ifndef tanf
#define tanf(x) ((float)(tan(x)))
#endif

#ifndef cosf
#define cosf(x) ((float)(cos(x)))
#endif

#ifndef sinf
#define sinf(x) ((float)(sin(x)))
#endif

#ifndef sqrtf
#define sqrtf(x) ((float)(sqrt(x)))
#endif

#ifndef acosf
#define acosf(x) ((float)(acos(x)))
#endif

#ifndef asinf
#define asinf(x) ((float)(asin(x)))
#endif

#ifndef atanf
#define atanf(x) ((float)(atan(x)))
#endif

#ifndef acoshf
#define acoshf(x) ((float)(acosh(x)))
#endif

#ifndef asinhf
#define asinhf(x) ((float)(asinh(x)))
#endif

#ifndef atanhf
#define atanhf(x) ((float)(atanh(x)))
#endif

#ifndef atan2hf
#define atan2hf(x) ((float)(atan2h(x)))
#endif

#ifndef atan2f
#define atan2f(x,y) ((float)(atan2(x,y)))
#endif

#ifndef coshf
#define coshf(x) ((float)(cosh(x)))
#endif

#ifndef sinhf
#define sinhf(x) ((float)(sinh(x)))
#endif

#ifndef tanhf
#define tanhf(x) ((float)(tanh(x)))
#endif

#endif // __GNUC__ < 3

#if __GNUC__ == 3 && __GNUC_MINOR__ == 4
#undef __NO_ISOCEXT
#endif

#endif /* H_MATH_H */
