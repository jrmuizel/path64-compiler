/* 

  Copyright (C) 2007 ST Microelectronics, Inc.  All Rights Reserved. 

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

  Contact information:  ST Microelectronics, Inc., 
  , or: 

  http://www.st.com 

  For further information regarding this notice, see: 

  http: 
*/

/* 
   AIR utilities used principally for error messages and memory management.
*/

#ifndef __AIR_UTILS_H__
#define __AIR_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define AIR_CHECK_NULL_STRING(str) ((NULL==str) ? "null" : str)

/* Currently, memory management is implemented in a
   very simple way. If needed, we could implement more
   sophisticated routines in the forthcoming releases.
   Another (and probably better) solution would be
   to rely on libiberty library routines for memory
   management.
 */
typedef struct AIR_Memblock{
  struct AIR_Memblock *next;        /* Chained list              */
  unsigned int         len;         /* Buffer length             */
  unsigned int         ptr;         /* Current pointer in buffer */
  char                *buf;         /* Buffer                    */
} AIR_Memblock;

typedef struct{
  char            *name;
  unsigned int     nb_alloc;
  unsigned int     block_def_size; /* Default size for memory   */
  
  struct AIR_Memblock *init;       /* Initial memory block      */
  struct AIR_Memblock *cur;        /* Current memory block      */
} AIR_Memory_Pool;

/* A rudimentary model for dynamic table. If buffer is
   full, a reallocation is done.
 */
typedef struct{
  char            *name;         /* name of symbol table      */
  unsigned int     next_item;    /* index of next item        */
  unsigned int     sz_item;      /* size of one item          */
  unsigned int     max_items;    /* maximum number of item    */
  char            *buf;          /* buffer. Size of buffer is */
                                 /* nb_tot * size_item.       */
} AIR_Dynamic_Table;

  /* AIR_error and AIR_fatal_error are used to print formatted error
     messages and exit if the error is unrecoverable.
     type: gives the type of error: 'syntax, internal, memory allocation errors'.
     prefix: gives a number that refers the error message.
     format: gives the error message.
  */
extern void AIR_error( char *type, char * prefix, char * format, ... );
extern void AIR_fatal_error( char *type, char * prefix, char * format, ... );

extern char* AIR_xmalloc(unsigned int);
extern void  AIR_xfree(char*);

extern AIR_Memory_Pool *AIR_memory_pool_init( char* name, int block_def_size );
extern char* AIR_memory_pool_alloc(AIR_Memory_Pool*, mUINT32);
extern void AIR_memory_pool_free(AIR_Memory_Pool*);

extern AIR_Dynamic_Table *AIR_init_dynamic_table(char*,unsigned int,unsigned int);
extern void AIR_free_dynamic_table( AIR_Dynamic_Table* dyn );
extern char* AIR_get_new_item_in_dynamic_table(AIR_Dynamic_Table*);
extern char* AIR_get_item_in_dynamic_table(AIR_Dynamic_Table*, unsigned int);


#ifdef __cplusplus
}
#endif

#endif
