/**
***			Interface PRO64/LAO
***			-------------------
***
*** Description:
***
***	This is the interface for the LAO stub entry points
***
**/

#ifndef laoinit_INCLUDED
#define laoinit_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/*
  lao_init -- LAO initialization function.
*/
void
lao_init(void);

/*
  lao_fini -- LAO finalization function.
*/
void
lao_fini(void);

#ifdef __cplusplus
}
#endif

#endif /* laoinit_INCLUDED */
