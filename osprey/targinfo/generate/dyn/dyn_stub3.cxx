
/* 

  Copyright (C) 2006 ST Microelectronics, Inc.  All Rights Reserved. 

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

/**
 *
 * This file is only useful for dynamic code extension.
 *
 * More precisely when building the shared object (dll),
 * the Open64 code generator needs some functions that have
 * been processed in earlier steps of its own process.
 *
 * The role devoted to file dyn_stubxxx.c is to emulates these
 * functions when dynamic code generation is activated.
 * 
 */
#ifndef DYNAMIC_CODE_GEN
#error "File " __FILE__ " can only be used for dynamic code generation\n"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "dyn_isa_registers.h"
#include "targ_isa_subset.h"

#include "dyn_stub3.h"


/* Merging dynamic and static table */
typedef struct {
  mUINT32     sz_item               ;         /* Size of one item         */
  void*      (*dyn_get_tab)(void)   ;         /* Pointer on dynamic table */
  mUINT32    (*dyn_get_tab_sz)(void);         /* Size of dynamic table    */
  mUINT32    nb_item_static         ;         /* Nb of items in static tab*/
} Concat;

static Concat rclass =           /* Register class table             */
{  
   (mUINT32)            sizeof(ISA_REGISTER_CLASS_INFO),
   (void*  (*)(void))   dyn_get_ISA_REGISTER_CLASS_tab,
   (mUINT32(*)(void))   dyn_get_ISA_REGISTER_CLASS_tab_sz,
   (mUINT32)            ISA_REGISTER_CLASS_STATIC_COUNT+1U,
};

static Concat rsubclass =        /* Register subclass table          */
{
   (mUINT32)            sizeof(ISA_REGISTER_SUBCLASS_INFO),
   (void*  (*)(void))   dyn_get_ISA_REGISTER_SUBCLASS_tab,
   (mUINT32(*)(void))   dyn_get_ISA_REGISTER_SUBCLASS_tab_sz,
   (mUINT32)            ISA_REGISTER_SUBCLASS_STATIC_COUNT+1U,
};

static Concat rclassindex =      /* Register class index table       */ 
{                                /* We assume that there is only one */
                                 /* register set by register class.  */
                                 /* See "isa_registers_gen.cxx"      */
    (mUINT32)            sizeof(mUINT8),
    (void*  (*)(void))   dyn_get_ISA_REGISTER_CLASS_index_tab,
    (mUINT32(*)(void))   dyn_get_ISA_REGISTER_CLASS_tab_sz,
    (mUINT32)            ISA_REGISTER_CLASS_STATIC_COUNT+1U,
};

/* Beginning of code ================================================ */

/*
 * Table concatenation procedure.
 */
static void 
concat_proc( Concat *in,     /* Concat structure */
             void** gbl_ptr  /* Ptr on static tab*/ )
{
   mUINT32 nb_added_item = in->dyn_get_tab_sz();
   void    *tmp_ptr;

   if(0==nb_added_item || NULL==in->dyn_get_tab())
    return;

   tmp_ptr = calloc(in->sz_item,nb_added_item+in->nb_item_static);

   if(NULL==tmp_ptr)
    { fprintf(stderr,"### Error: unable to allocate memory\n");
      exit(-1);
    }

   /* Copy static part of table */
   memcpy(tmp_ptr,
          *gbl_ptr,
          in->sz_item*in->nb_item_static);

   /* Copy dynamic part of table*/
   memcpy((char*)tmp_ptr+in->nb_item_static*in->sz_item,
          in->dyn_get_tab(),
          in->sz_item*nb_added_item);

   /* Update global pointer on the new table */
   *gbl_ptr = tmp_ptr;

    return;
}

/*
 * Exported routine.
 */
void 
Initialize_Register_Class_Stub( void )
{
  mUINT32  i;
 
  concat_proc(&rclass,     (void**)(&ISA_REGISTER_CLASS_info));
  concat_proc(&rclassindex,(void**)(&ISA_REGISTER_CLASS_info_index));
  concat_proc(&rsubclass,  (void**)(&ISA_REGISTER_SUBCLASS_info));

  /* Update index table */
  for(i=rclassindex.nb_item_static;    /* First index in "dyn. part of table */
      i<rclassindex.dyn_get_tab_sz()+rclassindex.nb_item_static;
      i++)
      { ISA_REGISTER_CLASS_info_index[i]+=rclassindex.nb_item_static;
      }

  return;
}


/* Beginning of referenced but not called objects ===================================== */

/*
 * Avoids some link problems due to reference in targ_isa_registers.cxx. 
 * these variables functions are not actually used by the extension generator.
 */
ISA_SUBSET_LIST *ISA_SUBSET_List = (ISA_SUBSET_LIST *)0;
UINT32 ISA_SUBSET_LIST_Mask(struct ISA_SUBSET_LIST *)
{
  return 0;
}

/* End of referenced but not called objects ===================================== */


