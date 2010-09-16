/*-------------------------------------------------------------------------*/
/*  
 *      Copyright 2005, STMicroelectronics, Incorporated.
 *      All rights reserved.
 *
 *      STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with STMicroelectronics and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 *  
 * File : utils_memory.c
 *
 * Description : this module is a memory manager.
............................................................................
*/ 
 
#include "utils_memory.h"

/*---------------------------------------------------------------------------*/
/* Memory management variables                                               */
/*---------------------------------------------------------------------------*/
int nb_alloc   = 0,                   /* Number of calls to malloc().        */
    nb_dealloc = 0;                   /* Number of calls to free().          */

typedef char   *PTR;

#define ANUM 3                        /* number of different areas supported.*/

static char    *areap[ANUM] = {NULL};
static int      avail[ANUM];
static int      curmemarea = 0;       /* Current memory area.                */

/* ========================================================================= */
/* Memory management                                                         */
/* History : the first releases of binary utilities (stobjdump, stobjcopy,   */
/* stelfdump) were based on an explicit allocation/deallocation memory       */
/* scheme. With introduction of new utilities (stnm,ststrip) and improvement */
/* of other one, we decided to use a more friendly scheme for developper. To */
/* do this, we reuse what has been done in stld.                             */
/* ========================================================================= */

#undef  SIZE
#define SIZE 2000               /* size in bytes of each block of memory
                                 * obtained from malloc */

#undef PTRSZ
#undef ALIGN
#define PTRSZ sizeof(PTR)
#define ALIGN(o) (((o) + (PTRSZ-1)) & (~(PTRSZ-1)))

/*-------------------------------------------------------------------------*/
/*
   Routine alloc_memory/dealloc_memory.

   Description : encapsulation of malloc/free
............................................................................
*/

static void* 
alloc_memory( uint_32 size )
{
     void *p = (char*)malloc(size);
     if(p == NULL)
         utilsPrintFatal( UTILS_PRINT_ERROR_MEMORY_CANNOT_ALLOCATE, size, utilsPrintSystemError() );
     nb_alloc++;
     return p;
}

static void
dealloc_memory( void *p )
{
     if (p != NULL)
      { free(p);
        nb_dealloc++;
      }
}

/*-------------------------------------------------------------------------*/
/*
   Routine explicit_alloc_memory, explicit_dealloc_memory

   Description : encapsulation of alloc_memory and dealloc_memory. These
                 routines must be used only if allocation/deallocation
............................................................................
*/
void*
explicit_alloc_memory( uint_32 size ) { return alloc_memory(size); }

void
explicit_dealloc_memory( void *p )    { dealloc_memory(p);         }

/*-------------------------------------------------------------------------*/
/*
   Routine getmem

   Description : Memory allocation in a given area. Imported from stld.
............................................................................
*/
static void*
getmem(int area, uint_32 size) /* size in bytes to be allocated */
{
    char *p;

    ASSERT( area >=0 && area < ANUM, "bad getmem() area",NULL);

    size = ALIGN(size);         /* round up to multiple of PTRSZ */

    if (areap[area] == NULL) {
        uint_32 sz = SIZE;
        if (size + PTRSZ > SIZE)
            sz = size + PTRSZ;
        p = (char*)alloc_memory(sz);
        areap[area] = p;
        *((PTR *) p) = NULL;
        avail[area] = PTRSZ;
    }else if (avail[area] + size > SIZE) {
        int sz = SIZE;
        if (size + PTRSZ > SIZE)
            sz = size + PTRSZ;
        p = (char*)alloc_memory(sz);
        *((PTR *) p) = areap[area];
        areap[area] = p;
        avail[area] = PTRSZ;
    }
    p = areap[area] + avail[area];
    avail[area] += size;

    return (void*) p;
}

/*-------------------------------------------------------------------------*/
/*
   Routine getmem_area0 / getmem_area1

   Description : Memory allocation in area 0,1.
............................................................................
*/
void*
getmem_area0( uint_32 size ) { return getmem(0,size); }

void*
getmem_area1( uint_32 size ) { return getmem(1,size); }

/*-------------------------------------------------------------------------*/
/*
   Routine getmem_curarea

   Description : Memory allocation in current memory area
............................................................................
*/
void*
getmem_curarea( uint_32 size ) {return getmem(curmemarea,size); }

void
set_curmemarea( uint_32 area )
{ 
   ASSERT(area<ANUM,"Wrong memory area",NULL);
   curmemarea = area;
}

/*-------------------------------------------------------------------------*/
/*
   Routine freemem

   Description : Memory deallocation of a given area. Imported from stld.
............................................................................
*/
static void
freemem( int area )
{
    char *p, *q;

    ASSERT( area >=0 && area < ANUM, "bad getmem() area",NULL);

    for( p = areap[area]; p != NULL; p = q ){
        q = *((PTR *) p);       /* get next before free!!! */
        dealloc_memory(p);
    }
    areap[area] = NULL;
    return;
}

/*-------------------------------------------------------------------------*/
/*
   Routine freemem_area0/free_mem_area1freeallmem

   Description : Memory deallocation of area 0,1 / memory allocation of 
                 all areas.
............................................................................
*/
void
freemem_area0( void )   { freemem(0);          }

void
freemem_area1( void )   { freemem(1);          }

void
freemem_curarea( void ) { freemem(curmemarea); }

void 
freeallmem( void )
{  int i;

   for(i=0;i<ANUM;i++)
     freemem(i);
}

/*-------------------------------------------------------------------------*/
/*
   Routine binassert

   Description : an assert specific to our tools. Mostly used through
                 BINASSERT macro.
............................................................................
*/
void 
binassert( int test,char *p2, char *p3, char *file, int line )
{
  /* Test is succesful : exit */
  if(test) return ;

#ifdef __RELEASE__

  if ( ( p2 == NULL ) || ( * p2 == '\0' ) ) {
    utilsPrintFatal( UTILS_PRINT_ERROR_ASSERT_0 );
  }
  else if ( ( p3 == NULL ) || ( * p3 == '\0' ) ) {
    utilsPrintFatal( UTILS_PRINT_ERROR_ASSERT_1, p2 );
  }
  else {
    utilsPrintFatal( UTILS_PRINT_ERROR_ASSERT_2, p2, p3 );
  }

#else

  if ( ( p2 == NULL ) || ( * p2 == '\0' ) ) {
    utilsPrintFatal( UTILS_PRINT_ERROR_ASSERT_0, file, line );
  }
  else if ( ( p3 == NULL ) || ( * p3 == '\0' ) ) {
    utilsPrintFatal( UTILS_PRINT_ERROR_ASSERT_1, file, line, p2 );
  }
  else {
    utilsPrintFatal( UTILS_PRINT_ERROR_ASSERT_2, file, line, p2, p3 );
  }

#endif

   exit(-1);
}
