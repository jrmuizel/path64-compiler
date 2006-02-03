
#ifndef __ARRAY_H__
#define __ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif

/** The array type.  */
typedef struct array_t_ *array_t;
  
/** Initializes an array.
    @param array the array
    @param size the original size of the array or 0 [defaults to 128]
    @param elt_size the size of a user datum (an array element)
*/
void array_ctor(array_t array, int size, int elt_size);

/** Finalizes an array
    @param array the array
*/
void array_dtor(array_t array);

/** Allocates and initializes an array.
    @ref array_ctor
    @return the allocated array
*/
array_t array_new(int size, int elt_size);

/** Finalizes and frees an array.
    @param array the array to be freed
    @return always NULL
*/
array_t array_del(array_t array);

/** Put an alement into the array and returns a pointer to the element.
    It increments the array element count unless an element was already
    present at the given idx.
    It may increment the last element index and resize the array.
    @param array
    @param idx the index of the element
    @return pointer to the inserted element
*/
void *array_set_at(array_t array, int idx);

/** Get the element at the given idx.
    @param array
    @param idx the index of the element
    @return pointer to the element or NULL if the element is not defined
*/
void *array_get_at(array_t array, int idx);

/** Remove an element from the array. It sets the location as undefined.
    It may decrement the array count if the location was defined.
    It may cahnge the last element of the array if it is the last element.
    @param array
    @param idx the element idx
    @return pointer to the remove element or 
    NULL if the location was undefined
*/
void *array_remove_at(array_t array, int idx);

/** Append an element to the end of the array.
    Same as: array_set_at(array, array_last(array)+1).
    @param array
    @return pointer to the appended element.
*/
void *array_append(array_t array);

/** Returns the number of defined elements.
    @param array
    @return number of elements
*/
int array_count(array_t array);

/** Returns the index of the first element of an array.
    Always 0 in the current implementation.
    @param array
    @return index of the first element
*/
int array_first(array_t array);

/** Returns the index of the last element of an array.
    @param array
    @return index of the last element
*/
int array_last(array_t array);

/** Sorts the array.
    @param array
    @param sorting function
*/
void array_qsort(array_t array, int (*compare)(const void *, const void *));

#ifdef __cplusplus
}
#endif

#endif


    
