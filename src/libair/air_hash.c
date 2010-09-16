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

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "air_hash.h"

/* Note: this file has been adapted from an equivalent RTK module.
   If needed, it could be replaced by the equivalent libiberty module.
 */

/*
   Input:    hashname   name of hash table (optionnal)
	     cmp_fct    comparison function between two items
	     hash_fct   hashing function
	     dum_fct    dumping one entry of the hash table
             free_fct   memory deallocation for one item        
	     sz_entry   (unsigned int) size of one entry
	     nb_buckets (unsigned int) nb of buckets.

   Output:
*/
AIR_Hashtab*
build_new_hash_table( char    *hashname,
		      int    (*cmp_fct)  (void*, void*),
		      int    (*hash_fct) (void*),
		      void   (*dump_fct) (FILE *, void*),
                      void   (*free_fct)  (void*),
                      void*  (*hash_alloc_fct)(int),
		      unsigned int sz_entry,
		      unsigned int nb_buckets)
{
    AIR_Hashtab *hashtable;
    unsigned int sizealloc;

    assert(cmp_fct       !=NULL);
    assert(hash_fct      !=NULL);
    assert(hash_alloc_fct!=NULL);

    sizealloc = sizeof(AIR_Hashtab);
    hashtable = (AIR_Hashtab*) malloc(sizealloc);
    assert(hashtable!=NULL);

    hashtable->cmp_fct        = cmp_fct;
    hashtable->hash_fct       = hash_fct;
    hashtable->dump_fct       = dump_fct;
    hashtable->free_fct       = free_fct;
    hashtable->hash_alloc_fct = hash_alloc_fct;

    if(hashname)
     { hashtable->name = (char*)malloc(strlen(hashname)+1);
       assert(hashtable->name!=NULL);
       strcpy(hashtable->name,hashname);
     }
    else
     { hashtable->name           = hashname;
     }
    hashtable->sz_entry       = sz_entry;
    hashtable->nb_buckets     = nb_buckets;

    sizealloc = sizeof(Item*)*nb_buckets;
    hashtable->buckets        = (Item**) (*hashtable->hash_alloc_fct) (sizealloc);
    
    memset(hashtable->buckets,0,sizealloc);

    hashtable->nb_collisions       = 0;
    hashtable->nb_item_in_hash_tab = 0;

    return hashtable;
}


/*
   Research an item in hash table.

   Input:     hashtable (AIR_Hashtab*)
	      new_entry (void*     ) researched item

   Output: returns pointer on item or NULL.
	   Set found to 1/0.
*/
void *
research_item( AIR_Hashtab *hash, void *new_entry, int *found )
{
    unsigned int  key;
    Item         *entry; 
    void         *ret;

    key    = (*hash->hash_fct)(new_entry);

    *found = 0;                 /* Default : not found */
     ret   = NULL;

    for(entry = hash->buckets[key]; entry!=NULL; entry=entry->next)
      if((*hash->cmp_fct)(entry->entry,new_entry))
	 { *found = 1;
	    ret = entry->entry;
	   break;
	 }

    return ret;
}

/* 
   Insert a new entry in hash table if it does not exists
   already.

   Input:  hash  (AIR_Hashtab*)   hash table
	   entry (void*     )   new entry.
           force (bool      )   if set, this flag forces creation
                                of a new item and does not test if
                                a previous equivalent item already exists.

   Output: returns 0 if item was not found (or if third parameter
           has been set to 1), and has been inserted,

	   returns 1 if item already exists.
*/
static int
insert_new_item_intern( AIR_Hashtab *hash, void * new_entry, int force )
{
    unsigned int  key;
    Item         *entry; 
    int           found = 0;

    key    = (*hash->hash_fct)(new_entry);
    assert(key<hash->nb_buckets);

    if(!force)
      if(research_item(hash,new_entry,&found)!=NULL)
        goto end;

    /* Bucket is not empty: we've got a collision.*/
    if(hash->buckets[key]!=NULL)
       hash->nb_collisions++;
    hash->nb_item_in_hash_tab++;

    /* Allocate memory for the Item structure */
    entry = (Item*)(*hash->hash_alloc_fct)(sizeof(Item));
    assert(entry!=NULL);

    /* Allocate memory for the entry it self*/
    entry->entry = (*hash->hash_alloc_fct)(hash->sz_entry);
    assert(entry->entry!=NULL);

    /* Insert entry*/
    memcpy(entry->entry,new_entry,hash->sz_entry);

    /* Insert Item struct. in bucket linked list */
    entry->next        = hash->buckets[key];
    hash->buckets[key] = entry;

    end:
    return found;
}

int
insert_new_item( AIR_Hashtab *hash, void * new_entry )
{
    return insert_new_item_intern(hash,new_entry,0 /* force */);
}

int
insert_new_item_force( AIR_Hashtab *hash, void * new_entry )
{
    return insert_new_item_intern(hash,new_entry,1 /* force */);
}

/*
   Delete an item from an hash table.

   Input:  hash  (AIR_Hashtab*)   hash table
	   entry (void*     )   entry to be deleted.

   Output: returns 1 if item was not found.
	   returns 0 if item was found and deleted.
*/
int
delete_item( AIR_Hashtab *hash, void *elem )
{
    Item   *entry;
    Item   *previous;
    int     ret = 1;       /* Default is an error */
    unsigned int  key;

    key    = (*hash->hash_fct)(elem);
    assert(key<hash->nb_buckets);

    /* Remove item from linked list */
    for(entry = hash->buckets[key], previous=NULL; 
        entry!= NULL && ret == 1; 
        previous = entry, entry=entry->next)
          if((*hash->cmp_fct)(elem,entry->entry)) /* We have found the id */
	    { if(previous==NULL)
               hash->buckets[key] = entry->next;
              else
               previous->next = entry->next;

              /* Free memory item */
              if(NULL!=hash->free_fct)
                (*hash->free_fct)(entry); 
               
              ret = 0;
	    }

    return ret;
}

/* Dumping an hash table
 */
void
dump_hash_table( FILE *f, AIR_Hashtab *hash )
{
   Item    *entry;
   unsigned int i;
   unsigned int count;
   char    *str = "------------------------------------\n";

   fprintf(f,str);
   if(NULL!=hash->name)
    fprintf(f,"dumping hash table \"%s\"\n",hash->name);
   else
    fprintf(f,"dumping hash table\n");

   fprintf(f,"numbers of items in hash table %d\n",hash->nb_item_in_hash_tab);
   fprintf(f,"overall number of collisions %d\n",hash->nb_collisions);
   fprintf(f,"number of buckets %d\n",hash->nb_buckets);

   for(i=0;i<hash->nb_buckets;i++)
    { entry = hash->buckets[i];
      if(entry == NULL)
	continue;

      fprintf(f,"bucket %d\n",i);
      count = 0;
      while(entry!=NULL)
       { 
	 if(NULL!=hash->dump_fct)
	   (*hash->dump_fct)(f,entry->entry);
	 else
	   fprintf(f,"entry %d\n",count);

	 entry = entry->next;
	 fprintf(f,"\n");
	 count++;
       }
     fprintf(f,"\n");
    }

   fprintf(f,str);
   return;
}

void
free_hash_table( AIR_Hashtab *hash )
{
    unsigned int i;
    Item        *cur;
    Item        *next;
 
    /* Free memoy for each bucket list */
    if(NULL!=hash->free_fct)
    { for(i=0;i<hash->nb_buckets;i++)
        { cur = hash->buckets[i];
          while(cur)
           { next = cur->next;
            (*hash->free_fct)((void*)cur);
             cur = next;
           }
        }
       hash->buckets[i]=NULL;
     }

    if(hash->name)
       free(hash->name);
    free(hash); 
    return;
}

/*===================================================================*/
/* Simple hash routine.                                              */
/*===================================================================*/

/* 
   A Mersenne number is a number that can be written
   as:
      y = 2**n - 1

   These numbers have some remarkable algebraic properties
   and some of the greatest prime numbers that are currently
   known are Mersenne numbers.

   Publication from Benoit (Dupont de Dinechin) entitled 

     "A ultrafast euclidiam division algorithm for prime memory systems"
     International conference on Supercomputing, Albuquerque, Nov. 1991.

   gives a fast algorithm for division by integers of the form 2**n +-1.

   In particular, the following relation is derived:

     x%(2**n -1) = (x%2**n + x/2**n) % (2**n -1)

   In the following code, we have implemented very rapidly
   an hashing routine that simply returns:

      a%PRIME

   where PRIME is the prime mersenne number 8191 (2*13 -1),
   and a is the string interpreted a big number.

   The following implementation is certainly not as efficient as
   SGS implementation.
*/

#define MERSENNE_PRIME 8191U
#define MERSENNE_N       13U
#define MERSENNE_CARRY    8U  /*  2**16 mod 2**13-1
                               = (2**(16 mod 13))(mod 2**13-1)
                               = (2**3)(mod2**13-1) = 8 */


static unsigned int
modulo_mersenne( unsigned int number )
{
    /* See Benoit's publication and above explanation. */
    while(number>MERSENNE_PRIME)
      number = (number >> MERSENNE_N) + (number & MERSENNE_PRIME);

    if(MERSENNE_PRIME==number)
     number = 0;

    return number;
}

unsigned int
hash_modulo_reduction( unsigned char *str )
{
    unsigned int i;
    unsigned int part;
    unsigned int carry;
    unsigned int length;
    unsigned int result = 0;

    /* if n is odd, we take into account the final '\0' that doesn't
       contribute in the modulo reduction. As a result, length is
       always even, which simplifies the following loop.
     */
    length = strlen((char*)str);
    if((length&0x1))
      ++length;

    carry = MERSENNE_CARRY;

    for(i=0;i<length;i+=2)
     { part = (unsigned int)(str[i]) | 
              ((unsigned int)(unsigned short)(str[i+1])<<0x9);
       if(i)
        {part *= carry;
         carry = modulo_mersenne(carry*carry);
        }
       result = modulo_mersenne(result+part);
     }

    return result;
}

/* Interface function */
unsigned int
hash_get_nb_buckets( void )
{
   return MERSENNE_PRIME;
}
