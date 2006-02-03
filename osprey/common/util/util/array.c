#include <stdlib.h>
#include "array.h"

#define ARRAY_ALLOC(size) malloc(size)
#define ARRAY_FREE(ptr) free(ptr)
#define ARRAY_REALLOC(ptr, size) realloc(ptr, size)

#ifdef ARRAY_DEBUG
#define ARRAY_ASSERT(cond) assert(cond)
#else
#include <assert.h>
#define ARRAY_ASSERT(cond) ((void)0)
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef struct array_bucket_t_ {
  int defined;
  long long user[1];
} array_bucket_t_;
typedef array_bucket_t_ *array_bucket_t;

#define array_bucket_SIZE(elt_size) (sizeof(array_bucket_t_)-sizeof(long long)+(elt_size))

static void
array_bucket_dtor(array_bucket_t bucket) {
}

static void *
array_bucket_data(array_bucket_t bucket)
{
  return (void *)((char *)bucket + sizeof(array_bucket_t_) - sizeof(long long));
}

static int
array_bucket_defined(array_bucket_t bucket)
{
  return bucket->defined;
}


typedef struct array_t_ {
  int first;
  int last;
  int count;
  int size;
  int elt_size;
  char *buckets;
} array_t_;

#define array_SIZE(size, elt_size) (sizeof(array_t_))

static array_bucket_t
array_bucket(array_t array, int index)
{
  return (array_bucket_t)(array->buckets + index * array_bucket_SIZE(array->elt_size));
}

static array_bucket_t
array_buckets(array_t array)
{
  return (array_bucket_t)(array->buckets);
}

void
array_ctor(array_t array, int size, int elt_size)
{
  int alloc_size;
  char *buckets;
  if (size == 0) 
    size = 128;
  
  /* Allocates buckets and set to zero.  */
  alloc_size = size * array_bucket_SIZE(elt_size);
  buckets = (char *)ARRAY_ALLOC(alloc_size);
  memset (buckets, 0, alloc_size);

  array->size = size;
  array->elt_size = elt_size;
  array->count = 0;
  array->first = 0;
  array->last = -1;
  array->buckets = buckets;
}

void
array_dtor(array_t array)
{
  ARRAY_FREE(array->buckets);
  array->buckets = NULL;
}

array_t
array_new(int size, int elt_size)
{
  array_t array;

  /* Allocates and constructs.  */
  array = (array_t)ARRAY_ALLOC(array_SIZE(size, elt_size));
  array_ctor(array, size, elt_size);
  return array;
}

array_t
array_del(array_t array)
{
  array_dtor(array);
  ARRAY_FREE(array);
  return NULL;
}

static void
array_alloc(array_t array, int size)
{
  int old_alloc_size, alloc_size;
  int new_size;
  if (size <= array->size) return;
  new_size = size << 1;
  old_alloc_size = array->size * array_bucket_SIZE(array->elt_size);
  alloc_size = new_size * array_bucket_SIZE(array->elt_size);
  array->buckets = (char *)ARRAY_REALLOC(array->buckets, alloc_size);
  memset (array->buckets+old_alloc_size, 0, alloc_size-old_alloc_size);
  array->size = new_size;
}

int array_count(array_t array)
{
  return array->count;
}

int array_last(array_t array)
{
  return array->last;
}

int array_first(array_t array)
{
  return array->first;
}

static array_bucket_t
array_bucket_insert_at(array_t array, int idx)
{
  array_bucket_t bucket;
  array_alloc(array, idx+1);
  bucket = array_bucket(array, idx);
  if (!bucket->defined) {
    bucket->defined = 1;
    array->count++;
    if (idx > array->last)
      array->last = idx;
  }
  return bucket;
}

static array_bucket_t
array_bucket_access_at(array_t array, int idx)
{
  array_bucket_t bucket;
  if (idx > array->last) return NULL;
  bucket = array_bucket(array, idx);
  if (!bucket->defined) {
    return NULL;
  }
  return bucket;
}

static array_bucket_t
array_bucket_remove_at(array_t array, int idx)
{
  array_bucket_t bucket;
  if (idx > array->last) return NULL;
  bucket = array_bucket(array, idx);
  if (bucket->defined) {
    bucket->defined = 0;
    array->count--;
    if (idx == array->last) {
      int i = array->last-1;
      while(i >= 0 && !array_bucket(array, i)->defined) {
	i--;
      }
      array->last = i;
    }
    return bucket;
  }
  return NULL;
}

void *
array_set_at(array_t array, int idx)
{
  return array_bucket_data(array_bucket_insert_at(array, idx));
}

void *
array_get_at(array_t array, int idx)
{
  array_bucket_t bucket = array_bucket_access_at(array, idx);
  if (bucket == NULL) return NULL;
  return array_bucket_data(bucket);
}

void *
array_remove_at(array_t array, int idx)
{
  array_bucket_t bucket = array_bucket_remove_at(array, idx);
  if (bucket == NULL) return NULL;
  return array_bucket_data(bucket);
}

void *
array_append(array_t array)
{
  array_bucket_t bucket = array_bucket_insert_at(array, array->last+1);
  if (bucket == NULL) return NULL;
  return array_bucket_data(bucket);
}

static int (*array_data_compare)(const void *, const void *);

static int
array_bucket_sort(const array_bucket_t a, array_bucket_t b)
{
  int a_defined = array_bucket_defined(a);
  int b_defined = array_bucket_defined(b);
  if (a_defined && b_defined) return array_data_compare(array_bucket_data(a), array_bucket_data(b));
  return b_defined - a_defined;
}

static void
array_update_last(array_t array)
{
  int i;
  for (i = array->last; i >= array->first; i--) {
    if (array_bucket_defined(array_bucket(array, i))) break;
  }
  array->last = i;
}

void
array_qsort(array_t array, int (*compare)(const void *, const void *))
{
  if (array->last < array->first) return;
  array_data_compare = compare;
  qsort(array_bucket(array, array->first), array->last - array->first+1,
	array_bucket_SIZE(array->elt_size), (int (*)(const void *, const void *))array_bucket_sort);
  array_update_last(array);
}



#ifdef ARRAY_SELFTEST

#define TEST_AT(array, idx) { \
  int *v_ptr = (int *)array_get_at(array, idx); \
  if (v_ptr) { \
    printf ("idx %d: defined, val %d\n", idx, *v_ptr); \
  } else { \
    printf ("idx %d: undefined\n", idx); \
  }

#define TEST_ALL(array) { \
  int i; \
  for (i = array_first(array); i <= array_last(array); i++) { \
    TEST_AT(array, i); \
  } \
} \
}

#define STATS(array) { \
  printf("array first: %d\n", array->first); \
  printf("array last: %d\n", array->last); \
  printf("array size: %d\n", array->size); \
  printf("array count: %d\n", array->count); \
}
#define APPEND(array, v) printf("append\n"); *(int *)array_append(array) = v;
#define SET_AT(array, idx, v) printf("set at %d\n", idx); *(int *)array_set_at(array, idx) = v;
#define REMOVE_AT(array, idx) printf("remove at %d\n", idx); array_remove_at(array, idx);


static int compare_int_ptr(const int *ptr1, const int *ptr2)
{
  return *ptr1 - *ptr2;
}

int 
main(int argc, char *argv[]) {
  array_t array = array_new(4, sizeof(int));
  APPEND(array, 5);
  APPEND(array, 6);
  APPEND(array, 1);
  APPEND(array, 100);
  APPEND(array, -1);
  TEST_ALL(array);
  STATS(array);
  SET_AT(array, 12, 3);
  TEST_ALL(array);
  STATS(array);
  REMOVE_AT(array, 12);
  TEST_ALL(array);
  STATS(array);
  SET_AT(array, 30, 4);
  TEST_ALL(array);
  STATS(array);
  array_qsort(array, (int (*)(const void *, const void *))compare_int_ptr);
  TEST_ALL(array);
  STATS(array);
  array_del(array);
  return 0;
}
#endif
