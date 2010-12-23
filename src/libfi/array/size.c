/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation version 2.1

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/


#pragma ident "@(#) libfi/array/size.c	92.2	07/07/99 15:52:02"
#include <liberrno.h>
#include <stddef.h>
#include <cray/dopevec.h>
#include <limits.h>

/*
 *      SIZE    Returns the extent of an array along a specified dimension
 *                or the total number of elements in the array.
 *              If source pointer/allocatable array is not 
 *                associated/allocated, return an error.
 *              If DIM not present, return the total number of elements in
 *                array.  Otherwise, return extent of Dim specified.
 */

_f_int
_SIZE   (DopeVectorType * source,
	_f_int		*dimptr)
{
        int iresult;
        int dim;
        int rank;
        int loopj;

	/* If source is a pointer/allocatable array, it must be
	 * associated/allocated. */
	if (source->p_or_a  &&  !source->assoc)
		_lerror (_LELVL_ABORT, FENMPTAR, "SIZE");

	rank = source->n_dim;
	if (dimptr == NULL) {
		iresult = 1;
		/* Retrieve product of extents */
		for (loopj = 0; loopj < rank; loopj++)
			iresult = iresult * source->dimension[loopj].extent;
	}
	else {
	/* argument DIM must be within source array rank */
		dim = *dimptr - 1;
		if (dim < 0 || dim >= rank)
                	_lerror (_LELVL_ABORT, FENMSCDM, "SIZE");

		/* Return extent */
		iresult = source->dimension[dim].extent;
	}

        return(iresult);
}

/*
 *      SIZE_4    Returns the extent of an array along a specified dimension
 *                or the total number of elements in the array.
 *              If source pointer/allocatable array is not 
 *                associated/allocated, return an error.
 *              If DIM not present, return the total number of elements in
 *                array.  Otherwise, return extent of Dim specified.
 */

#if defined (_UNICOS)
#pragma duplicate _SIZE_4 as _SIZE_2
#pragma duplicate _SIZE_4 as _SIZE_1
#endif

_f_int4
_SIZE_4   (DopeVectorType * source,
	_f_int		*dimptr)
{
        _f_int4 iresult;
        int dim;
        int rank;
        int loopj;

	/* If source is a pointer/allocatable array, it must be
	 * associated/allocated. */
	if (source->p_or_a  &&  !source->assoc)
		_lerror (_LELVL_ABORT, FENMPTAR, "SIZE");

	rank = source->n_dim;
	if (dimptr == NULL) {
		iresult = 1;
		/* Retrieve product of extents */
		for (loopj = 0; loopj < rank; loopj++)
			iresult = iresult * source->dimension[loopj].extent;
	}
	else {
	/* argument DIM must be within source array rank */
		dim = *dimptr - 1;
		if (dim < 0 || dim >= rank)
                	_lerror (_LELVL_ABORT, FENMSCDM, "SIZE");

		/* Return extent */
		iresult = source->dimension[dim].extent;
	}

        return(iresult);
}

/*
 *      SIZE_8    Returns the extent of an array along a specified dimension
 *                or the total number of elements in the array.
 *              If source pointer/allocatable array is not 
 *                associated/allocated, return an error.
 *              If DIM not present, return the total number of elements in
 *                array.  Otherwise, return extent of Dim specified.
 */

_f_int8
_SIZE_8   (DopeVectorType * source,
	_f_int *dimptr)
{
        _f_int8 iresult;
        int dim;
        int rank;
        int loopj;

	/* If source is a pointer/allocatable array, it must be
	 * associated/allocated. */
	if (source->p_or_a  &&  !source->assoc)
		_lerror (_LELVL_ABORT, FENMPTAR, "SIZE");

	rank = source->n_dim;
	if (dimptr == NULL) {
		iresult = 1;
		/* Retrieve product of extents */
		for (loopj = 0; loopj < rank; loopj++)
			iresult = iresult * source->dimension[loopj].extent;
	}
	else {
	/* argument DIM must be within source array rank */
		dim = *dimptr - 1;
		if (dim < 0 || dim >= rank)
                	_lerror (_LELVL_ABORT, FENMSCDM, "SIZE");

		/* Return extent */
		iresult = source->dimension[dim].extent;
	}

        return(iresult);
}

/*
 *       SIZEOF_4  Returns the size of an array in bytes,
 *                 If source pointer/allocatable array is not 
 *                 associated/allocated, return an error.
 */

_f_int4
_SIZEOF_4   (DopeVectorType * source)
{
        _f_int4 iresult;
        int rank;
        int loopj;
        int type_len;

        /* If source is a pointer/allocatable array, it must be
         * associated/allocated. */

        if (source->p_or_a  &&  !source->assoc)
                _lerror (_LELVL_ABORT, FENMPTAR, "SIZEOF");

        rank = source->n_dim;
        iresult = 1;

        /* Retrieve product of extents */

        for (loopj = 0; loopj < rank; loopj++)
            iresult = iresult * source->dimension[loopj].extent;

        /* if type is Derived type */
        if (source->type_lens.type == DVTYPE_DERIVEDWORD || source->type_lens.type == DVTYPE_DERIVEDBYTE) {
           type_len = source->base_addr.a.el_len;
        }
        /* if type is character */
        else if (source->type_lens.type == DVTYPE_ASCII) {
            type_len = source->type_lens.int_len * source->base_addr.a.el_len;
        } else {
            type_len = source->type_lens.int_len;
        }


        /* convert iresult to bytes */

        iresult = (iresult * type_len)/CHAR_BIT;
        return(iresult);
}

/*
 *       SIZEOF_8  Returns the size of an array in bytes,
 *                 If source pointer/allocatable array is not 
 *                 associated/allocated, return an error.
 */

_f_int8
_SIZEOF_8   (DopeVectorType * source)
{
        _f_int8 iresult;
        int rank;
        int loopj;
        int type_len;

        /* If source is a pointer/allocatable array, it must be
         * associated/allocated. */
        if (source->p_or_a  &&  !source->assoc)
                _lerror (_LELVL_ABORT, FENMPTAR, "SIZEOF");

        rank = source->n_dim;
        iresult = 1;
        /* Retrieve product of extents */
        for (loopj = 0; loopj < rank; loopj++)
            iresult = iresult * source->dimension[loopj].extent;

        /* if type is Derived type */
        if (source->type_lens.type == DVTYPE_DERIVEDWORD || source->type_lens.type == DVTYPE_DERIVEDBYTE) {
           type_len = source->base_addr.a.el_len;
        }
        /* if type is character */
        else if (source->type_lens.type == DVTYPE_ASCII) {
            type_len = source->type_lens.int_len * source->base_addr.a.el_len;
        } else {
            type_len = source->type_lens.int_len;
        }

        /* convert iresult to bytes */
        iresult = (iresult * type_len)/CHAR_BIT;

        return(iresult);
}
