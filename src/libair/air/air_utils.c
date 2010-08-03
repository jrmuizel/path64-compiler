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

#include <string.h>

#include "air.h"
#include "air_utils.h"

static AIR_Memblock* allocate_space_in_block(unsigned int,AIR_Memblock*,
                                             char**,unsigned int);
static void free_block(AIR_Memblock*);

char* AIR_xmalloc(unsigned int);
void  AIR_xfree(char*);

char* AIR_memory_pool_alloc(AIR_Memory_Pool*, mUINT32);
void AIR_memory_pool_free(AIR_Memory_Pool*);
AIR_Dynamic_Table* AIR_init_dynamic_table(char*,unsigned int,unsigned int);
void AIR_free_dynamic_table( AIR_Dynamic_Table* dyn );
char* AIR_get_new_item_in_dynamic_table(AIR_Dynamic_Table*);
char* AIR_get_item_in_dynamic_table(AIR_Dynamic_Table*, unsigned int);

/* See air_utils.h for details */
void
AIR_error( char *type, char * prefix, char * format, ... ) {
  va_list ap;
  FILE *f = stderr;
  va_start( ap, format );
  fprintf( f, "%s error  : (%s) ", AIR_CHECK_NULL_STRING( type ), AIR_CHECK_NULL_STRING( prefix ) );
  vfprintf( f, format, ap );
  fprintf( f, "\n" );
  fflush( f );
  va_end( ap );
}

void
AIR_fatal_error( char *type, char * prefix, char * format, ... ) {
  va_list ap;
  FILE *f = stderr;
  va_start( ap, format );
  fprintf( f, "%s fatal error  : (%s) ", AIR_CHECK_NULL_STRING( type ), AIR_CHECK_NULL_STRING( prefix ) );
  vfprintf( f, format, ap );
  fprintf( f, "\n" );
  fflush( f );
  va_end( ap );
  exit(1);
}

/* Redefinition of the libiberty routine */
char*
AIR_xmalloc( unsigned int size )
{
    char *ret ; 

    if(0==size)
      size = 1;
    ret = malloc(size);
    if(NULL==ret)
      { fprintf(stderr,"Fatal error: memory allocation error\n");
        exit(1);
      }

#ifdef TEST
     { static int count_alloc = 0;
       fprintf(stdout,"(%03d) Allocate %d bytes at address 0x%08x\n",
               count_alloc,size,(unsigned int) ret);
       count_alloc++;
     }
#endif
    return ret;
}

/* Redefinition of the libiberty routine */
void
AIR_xfree( char* ptr )
{
    free(ptr);
#ifdef TEST
    { static int count_free = 0;
      fprintf(stdout,"(%03d) Free pointer at address 0x%08x\n",
              count_free,(unsigned int) ptr);
      count_free++;
    }
#endif
    return; 
}

/* Memory allocation in a block
   First  param: size of memory to be allocated
   Second param: current memory block
   Third  param: out parameter. Returned pointer.
   Fourth param: default size for allocation of a new block
 */
static AIR_Memblock*
allocate_space_in_block( unsigned int len, AIR_Memblock *cur, char**res,
                         unsigned int def_size )
{
   unsigned int  length_alloc;
   AIR_Memblock *new_block;

   if(0U==len)
     len = 1U;

   /* if not enough space or no block, allocate a new one */
   if (NULL==cur || cur->len<cur->ptr+len)
    { length_alloc = len>def_size ? len + 10 : def_size;
      new_block = (AIR_Memblock*)AIR_xmalloc(sizeof(AIR_Memblock));
      new_block->buf = AIR_xmalloc(length_alloc);
      new_block->len = length_alloc;
      new_block->ptr = 0;
      new_block->next= NULL;
      if(cur)
        cur->next = new_block;
    }
   else
    { new_block = cur;
    }
    
  *res = new_block->buf+new_block->ptr;
   new_block->ptr += len;

   return new_block;
}

/* Free memory block*/
static void
free_block( AIR_Memblock *block )
{
   AIR_xfree((char*)block->buf);
   AIR_xfree((char*)block);
   return;
}

/* Allocation in a memory pool */
char*
AIR_memory_pool_alloc( AIR_Memory_Pool *pool, mUINT32 len )
{
    char *ret;

    pool->cur = allocate_space_in_block(len,pool->cur,&ret,
                                        pool->block_def_size);
    if(NULL==pool->init) 
      pool->init = pool->cur;
    (pool->nb_alloc)++;

    return ret;
}

/* Memory pool initialization
   First parameter is the pool name
   Second parameter is the default size for memory
   block allocation
*/
AIR_Memory_Pool*
AIR_memory_pool_init( char* name, int block_def_size )
{
    AIR_Memory_Pool *ret;

    name = AIR_CHECK_NULL_STRING(name);

    ret = (AIR_Memory_Pool*)AIR_xmalloc(sizeof(AIR_Memory_Pool));
    ret->name = AIR_xmalloc(strlen(name)+1);
    strcpy(ret->name,name);
    ret->nb_alloc = 0;
    ret->cur = ret->init = NULL;
    ret->block_def_size = block_def_size;

    return ret;
}

void
AIR_memory_pool_free( AIR_Memory_Pool *pool )
{
    AIR_Memblock *cur;
    AIR_Memblock *next;

    if(NULL==pool)
      return;

    /* free memory pool linked list */
    cur = pool->init;
    while(cur)
     { next = cur->next;
       free_block(cur);
       cur = next;
     }
 
    if(pool->name)
      AIR_xfree(pool->name);

    AIR_xfree((char*)pool);
    return;
}

/* Initialization of a dynamic table
   First parameter is the name assigned to the dynamic table,
   Second parameter is the maximum number of records
   to be stored in this table (table can be redimensioned
   but this operation is costly).
   Third parameter is the size of one item in the table
  */
AIR_Dynamic_Table* 
AIR_init_dynamic_table( char* name, unsigned int nb_items, unsigned int sz_item )
{
    AIR_Dynamic_Table* dyn = NULL;

    name = AIR_CHECK_NULL_STRING(name);

    dyn = (AIR_Dynamic_Table*) AIR_xmalloc(sizeof(AIR_Dynamic_Table));
    dyn->name = (char*) AIR_xmalloc(strlen(name)+1);
    strcpy(dyn->name,name);
    dyn->max_items = nb_items;
    dyn->sz_item  = sz_item;
    dyn->next_item= 0;
    dyn->buf      = (char*) AIR_xmalloc(sizeof(char)*nb_items*sz_item);

    return dyn;
}

void
AIR_free_dynamic_table( AIR_Dynamic_Table* dyn ) 
{
    if(NULL==dyn)
      return;

    AIR_xfree(dyn->name);
    AIR_xfree(dyn->buf);
    AIR_xfree((char*)dyn);
    return;
}

char*
AIR_get_new_item_in_dynamic_table( AIR_Dynamic_Table *dyn )
{
    char *buf;
    unsigned int new_size;
    char *ptr;

    /* If necessary, extend the table - very costly operation */
    if(dyn->next_item == dyn->max_items)
     { new_size = 2*dyn->max_items;
       buf = (char*) AIR_xmalloc(new_size*dyn->sz_item);
       memcpy(buf,dyn->buf,dyn->max_items*dyn->sz_item);
       AIR_xfree(dyn->buf);
       dyn->buf = buf;
       dyn->max_items =new_size;
     }

    ptr = dyn->buf + dyn->sz_item * dyn->next_item;
    (dyn->next_item)++;

    return ptr;
}

char*
AIR_get_item_in_dynamic_table( AIR_Dynamic_Table *dyn, unsigned int index )
{
    return dyn->buf + dyn->sz_item * index;
}

