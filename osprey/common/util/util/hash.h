/*
 * simple hash module
 */

#ifndef __HASH_H__
#define __HASH_H__

#ifdef __cplusplus
extern "C" {
#endif

/** The hash bucket type.  */
typedef struct hash_bucket_s *hash_bucket_t;

/** Returns a pointer to the user data from a hash bucket. */
extern void *hash_bucket_data(hash_bucket_t);

/** Returns a the hash bucket key. */
extern const char *hash_bucket_key(hash_bucket_t);

/** Returns a the hash bucket key size. */
extern int hash_bucket_key_size(hash_bucket_t);

/** The hash table type.  */
typedef struct hash_s *hash_t;

/** Allocates a new hash table.
   @param size the size of the hash array
   @param user_size the size of the user data to be hashed
   @return the hash map
   @pre size must ba a power of 2.
*/
extern hash_t hash_new (int size, int user_size);

/** Free a hash table.
   @param hash the hash to free
   @return always NULL
*/
extern hash_t hash_del (hash_t hash);

/** Returns the number of elements.
    @param hash 
    @return the elements count
*/
extern int hash_count(hash_t hash);

/*
 * Statistics on the hash table. 
 */

/** Returns the size of the hash array.
    @param hash 
    @return the size
*/
extern int hash_size(hash_t hash);

/** Returns the number of used slots (in the range 0..hash_size).
    For instance to have the mean of slots length:
    hash_count/hash_used_slots.

    @param hash 
    @return the count of taken slots
*/
extern int hash_used_slots(hash_t hash);

/** Inserts or return an element into the hash.
   Returns the new (or already hashed) bucket.
   Use hash_bucket_data(bucket) to get user data.
   @param hash the hash table
   @param key a pointer to the key
   @param ky_size the key size in bytes
   @return the hash bucket
*/
extern hash_bucket_t hash_insert_bucket (hash_t hash, const char *key, int key_size);

/** Access an element of the hash.
   Use hash_bucket_data(bucket) to get user data.
   @param hash the hash table
   @param key a pointer to the key
   @param ky_size the key size in bytes
   @return the hash bucket or NULL if not inserted
*/
extern hash_bucket_t hash_access_bucket (hash_t hash, const char *key, int key_size);

/** Removes and returns an element from the hash.
   Use hash_bucket_data(bucket) to get user data to be destructed.
   @param hash the hash table
   @param key a pointer to the key
   @param ky_size the key size in bytes
   @return the hash bucket or NULL if not inserted
*/
extern hash_bucket_t hash_remove_bucket (hash_t hash, const char *key, int key_size);

/** The hash iterator type.  */
typedef struct hash_iterator_s *hash_iterator_t;

/** Creates a new iterator from a hash.
    The returned iterator must be released with hash_iterator_del.
    After this call hash_iterator_current returns the first element.
    @param hash the hash table
    @return the new iterator
*/
extern hash_iterator_t hash_iterator_new(hash_t hash);

/** Releases a hash iterator
    @param iterator the iterator
    @return always NULL
*/
extern hash_iterator_t hash_iterator_del(hash_iterator_t iterator);

/** Returns true if at end of iteration
    @param iterator the iterator
    @return true is at end of iteration, otherwise hash_iterator_current and hash_iterator_advance may be called.
*/
extern int hash_iterator_at_end(hash_iterator_t iterator);

/** Advance to the next element
    @param iterator the iterator
*/
extern void hash_iterator_advance(hash_iterator_t iterator);

/** Get the current hash bucket of the iterator.
    @param iterator the iterator
    @return the current bucket or NULL if at end of iteration
*/
extern hash_bucket_t hash_iterator_current(hash_iterator_t iterator);

/** Rewinds an hash iterator.
    After this call hash_iterator_current returns the first element.
    @param iterator the iterator
*/
extern void hash_iterator_rewind(hash_iterator_t iterator);

#ifdef __cplusplus
}
#endif

#endif

