
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


/* Implementation of a simple hash table. This file has been
   adapted from reconfiguration tool kit.
 */

#ifndef _AIR_HASH_H_
#define _AIR_HASH_H_

#include <stdio.h>

typedef struct Item
{
    void        *entry;           /* Item.                          */
    struct Item *next ;
}Item;

typedef struct
{
    char         *name;                /* Name of hash table (optionnal) */
    unsigned int  sz_entry;            /* Size of each object in hash    */
    unsigned int  nb_buckets;
    Item        **buckets;       
    unsigned int  nb_collisions;       /* For statistics only.           */
    unsigned int  nb_item_in_hash_tab; /* For statistics only.           */

    int  (*hash_fct)(void*);           /* Hashing function.              */
    int  (*cmp_fct) (void*,void*);     /* Comparison between two entries.*/
    void (*dump_fct)(FILE*,void*);     /* Dumping function for one entry.*/
    void (*free_fct) (void*);          /* Used to free one item in an    */
                                       /* table. Pointer might be NULL.  */
    void*(*hash_alloc_fct)(int);       /* Memory allocation routine      */
 
}AIR_Hashtab;

extern AIR_Hashtab* build_new_hash_table(
	       char * hashname,
	       int  (*cmp_fct)  (void*,void*),
	       int  (*hash_fct) (void*),
	       void (*dump_fct) (FILE*,void*),
               void (*free_fct)  (void*),
               void*(*hash_alloc)(int),
	       unsigned int sz_entry,
	       unsigned int nb_buckets);

extern void* research_item( AIR_Hashtab *hash, void *entry, int *found);
extern int   insert_new_item(AIR_Hashtab *hash, void *entry );
extern int   insert_new_item_force(AIR_Hashtab *hash, void *entry );
extern int   delete_item(AIR_Hashtab *hash, void *entry);
extern void  dump_hash_table(FILE *f, AIR_Hashtab *hash);
extern void  free_hash_table(AIR_Hashtab *hash);
extern unsigned int hash_modulo_reduction( unsigned char *str );
extern unsigned int hash_get_nb_buckets( void );

#endif                                             /* _AIR_HASH_H_    */

