

/* ====================================================================
 * ====================================================================
 *
 * Module: gcov_profile.h
 *
 * Description:
 *
 * Utilities for GNU gcov .
 *
 * Utilities:
 *
 *    void (void)
 *
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef	GCOV_PROFILE_INCLUDED
#define	GCOV_PROFILE_INCLUDED
#include <stdio.h>

/* Initialisation of gcov module: need to be called once */
extern void gcov_init (const char *);
/* Finish gcno generation + instrument code: to be called once */
extern void gcov_finish (FILE *file);

/* Generate gcov info for the current pu. Need to be called once for each PU*/
extern void gcov_pu(void);



#endif /* GCOV_PROFILE_INCLUDED */
