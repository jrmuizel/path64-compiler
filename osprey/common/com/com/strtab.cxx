/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/



#ifdef TARG_ST
// The string table is implemented as a single character buffer, re-allocated
// if necessary.  Byte 0 of this buffer is alwasy NULL.  Each string is
// copied to the buffer once.  Duplicates are not entered.  A simple hash
// table is used to for quick lookup.  The hash table is a fixed size
// array of STR_BUCKET, each of which a dynamic array of <idx,size> pair.
// The size field is truncated to 8 bits.  It is used for 
// quick comparision to minimize the need to call strcmp.

// we support two types of strings: C-style null-terminated string and
// character array with a specified size (may contain null character within
// the array.


// TARG_ST [CG]:
// The strtab has been reimplemented to not reallocate the buffer
// containing the strings.
// Indeed with the old implementation the string pointers were not
// persistent due to the reallocation of the string buffer.
// This caused and may have cause problem in a lot of places in the compiler,
// for instance:
// const char *sym_name = ST_name(sym);
// ...
// new_idx = Save_Str("string");
// ...
// return sym_name; // maybe invalid!
//
// The sym_name may not be preserved if the call to Save_Str
// induced a reallocation of the string buffer.
//
// The new implementation use a chained list of segmented buffers. Thus
// old string pointers are persistent and no reallocation is performed.
// New support functions have been added to generate a continuous
// array of strings (STR_TAB_BUFFER) from a STR_TAB string table.
// The continuous STR_TAB_BUFFER is used in the WHIRL write phases that
// directly write a buffer the string buffer to file.
//

#ifdef USE_PCH
#include "common_com_pch.h"
#endif /* USE_PCH */
#pragma hdrstop

#include "W_alloca.h"
#ifndef __MINGW32__
#include <strings.h>
#endif

// [HK]
#if __GNUC__ >= 3
#include <ext/hash_map>
#else
#include <hash_map>			// stl hash table
#endif
#include "namespace_trans.h"

//#define STRTAB_VERBOSE
#ifdef STRTAB_VERBOSE
#define DEV_OUTPUT(args) printf args
#else
#define DEV_OUTPUT(args) (void)0
#endif

#ifdef TEST
#define FMT_ASSERT(x, args) { if (!(x)) printf args;  assert(x); }
#define DEV_ASSERT(x, args) { if (!(x)) printf args;  assert(x); }
#define CXX_NEW(args, pool) new args
#define CXX_NEW_ARRAY(type, size, pool) new type[size]
#define CXX_DELETE(ptr,pool) delete ptr
#define CXX_DELETE_ARRAY(ptr, pool) delete[] ptr
#else
#include "defs.h"
#include "errors.h"
#include "cxx_memory.h"			// for CXX_NEW

#define FMT_ASSERT(x, args) FmtAssert(x, args)
#define DEV_ASSERT(x, args) DevAssert(x, args)
#endif // TEST

#include "strtab.h"

typedef UINT32 STR_INDEX;

#if defined(linux) || defined(sun) || defined(__CYGWIN__) || defined(__MINGW32__)

#define STR_INDEX_size(xxx) (xxx & 0xff)
#define STR_INDEX_index(xxx) (xxx >> 8)

#else

static inline mUINT8
STR_INDEX_size (STR_INDEX idx)		{ return idx & 0xff; }
static inline mUINT32
STR_INDEX_index (STR_INDEX idx)		{ return idx >> 8; }

#endif /* linux || sun */

static inline STR_INDEX
make_STR_INDEX (UINT32 size, UINT32 idx)
{
    // if string length larger than 0xff, just use 0xff
    if (size > 0xff)
	size = 0xff;
    return (STR_INDEX) ((idx << 8) | size);
}

struct NULL_TERMINATED_STRING
{
    static char *get_str (char *buffer) {
	return buffer;
    }

    static const char *get_str (const char *buffer) {
	return buffer;
    }

    static UINT32 get_length (const char *buffer) {
	return strlen (buffer);
    }

    static UINT32 get_buffer_length (UINT32 length) {
	return length + 1;		// length + null character
    }


    static void copy (const char *str, UINT32 length, char *buffer) {
	memcpy (buffer, str, length+1);
    }
};


union UNALIGN_INT32
{
    char ch[sizeof(UINT32)];
    UINT32 n;

    UNALIGN_INT32 (UINT32 x) : n (x) {}

    UNALIGN_INT32 (const char *s) {
	for (INT32 i = 0; i < sizeof(UINT32); ++i)
	    ch[i] = *s++;
    }
};


struct CHARACTER_ARRAY
{
    // If the character array is less then 0xff bytes, we store the size in
    // The First Byte Followed By The Array.  Otherwise, The First Byte Is Set
    // To 0xff And The Next 4 Bytes Hold The Size Of The Array As An Unsigned
    // Integer, And The Array Follows.

    static const char *get_str (const char *buffer) {
	if (*buffer != 0xff)
	    return buffer + 1;
	else
	    return buffer + sizeof(UINT32) + 1;
    }

    static char *get_str (char *buffer) {
	if (*buffer != 0xff)
	    return buffer + 1;
	else
	    return buffer + sizeof(UINT32) + 1;
    }

    static UINT32 get_length (const char *buffer) {
	if (*buffer != 0xff)
	    return *buffer;
	else {
	    UNALIGN_INT32 unalign (buffer + 1);
	    return unalign.n;
	}
    }

    static UINT32 get_buffer_length (UINT32 length) {
	return length < 0xff ? length + 1 : length + 1 + sizeof(UINT32);
    }

    static void copy (const char *str, UINT32 length, char *buffer) {
	if (length < 0xff) {
	    *buffer++ = length;
	} else {
	    *buffer++ = 0xff;
	    UNALIGN_INT32 unalign (length);
	    for (INT32 i = 0; i < sizeof(UINT32); ++i)
		*buffer++ = unalign.ch[i];
	}
	memcpy (buffer, str, length);
    }

}; // CHARACTER_ARRAY


/*
 * This type implements a growing buffer. The buffer is a list
 * of buffer segments. Once the buffer overflows, a new segment is
 * allocated. The growing strategy is to double the buffer size
 * at each growing step up to 1Mb, after 1Mb the buffer grows 
 * by 1Mb steps. 
 * Pointers to string in the buffer are preserved along the object 
 * life. 
 * The default initial size is 64Kb unless specified 
 * when constructing the object.
 */
struct STR_BUFFER
{

  struct buffer_segment {
    buffer_segment *bs_next;
    UINT32 bs_size;
    char *bs_buffer;
  };

  static const UINT32 sb_default_hint = 1<<16;
  
  struct buffer_segment *sb_first;
  struct buffer_segment *sb_last;
  UINT32 sb_size;
  UINT32 sb_count;
  UINT32 sb_hint;
  MEM_POOL *sb_pool;
  UINT32 sb_unreserve_count;

  STR_BUFFER(UINT32 hint, MEM_POOL *pool) : 
    sb_first(NULL),
    sb_last(NULL),
    sb_size(0),
    sb_count(0),
    sb_unreserve_count(0),
    sb_hint(hint == 0 ? sb_default_hint: hint),
    sb_pool(pool) {};
  
  ~STR_BUFFER() { delete_all_segments(sb_first); }

  buffer_segment *new_segment(UINT32 size) {
    DEV_OUTPUT(("call to new_segment(%u)\n", size));
    buffer_segment *segment = CXX_NEW(buffer_segment, sb_pool);
    segment->bs_buffer = CXX_NEW_ARRAY(char, size, sb_pool);
    segment->bs_size = size;
    segment->bs_next = NULL;
    return segment;
  }

  void push_segment(buffer_segment *segment) {
    if (sb_first == NULL) sb_first = segment;
    else sb_last->bs_next = segment;
    sb_last = segment;
  }
    
  void delete_segment(buffer_segment *segment) {
    CXX_DELETE_ARRAY(segment->bs_buffer, sb_pool);
    CXX_DELETE(segment, sb_pool);
  }

  void delete_all_segments(struct buffer_segment *segment) {
    if (segment != NULL) {
      if (segment->bs_next != NULL) delete_all_segments(segment->bs_next);
      delete_segment(segment);
    }
  }

  void enlarge(UINT32 size) {
    DEV_OUTPUT(("call to enlarge(%u)\n", size));
    if (size <= sb_size) return;
    UINT32 new_size = sb_size == 0 ? sb_hint : sb_size;
    while (size > new_size) {
      if (new_size < 1024*1024) new_size *= 2;
      else new_size += 1024*1024;
    }
    push_segment(new_segment(new_size - sb_size));
    sb_size = new_size;
  }
  
  UINT32 reserve(UINT32 size) {
    UINT32 idx;
    if (size > sb_size - sb_count) {
      sb_count = sb_size + size;
      enlarge(sb_count);
    } else {
      sb_count += size;
    }
    idx = sb_count - size;
    sb_unreserve_count = sb_count - size;
    return idx;
  }

  void unreserve() {
    sb_count = sb_unreserve_count;
  }

  /* This function takes log(idx) time to find the buffer start. */
  char *get_buffer_ptr(UINT32 idx) const {
    struct buffer_segment *segment = sb_first;
    while (idx >= segment->bs_size) {
      idx -= segment->bs_size;
      segment = segment->bs_next;
    }
    return &segment->bs_buffer[idx];
  }
  
  void clear() {
    delete_all_segments(sb_first);
    sb_first = sb_last = NULL;
    sb_size = sb_count = sb_unreserve_count = 0;
  }
}; // STR_BUFFER

template <class STR>
struct STR_TAB
{
    static int const str_default_hash_hint = 1024;
    static int const str_default_buffer_hint = 1024*64;

    STR_BUFFER str_buffer;
    MEM_POOL *str_pool;

    // the following 4 function objects are required for setting up the
    // hash table declared after them.

    // hash key, which is a pair of string pointer and string length
    struct hash_key {
	const char *str;
	mUINT32 size;

	hash_key () {}
	
	hash_key (const char *s, UINT32 l) : str (s), size (l) {}
	
	hash_key (const hash_key& p) : str (p.str), size (p.size) {}
    };


    // hash function
    struct hash {
	UINT32 operator() (const hash_key &key) const {
	    UINT32 h = 0;
	    const UINT32 length = key.size;
	    const char *str = key.str;
	    for (UINT32 i = 0; i < length; ++i)
		h = 5 * h + str[i];
	    return h;
	}
    };
	    

    // how the hashtable convert a STR_INDEX into a hash_key
    struct extract_key {
	const STR_TAB<STR>& strtab;

	extract_key (const STR_TAB<STR>& tab) : strtab (tab) {}
	
        hash_key operator() (STR_INDEX str_index) const {
	    const char *str = strtab.str_buffer.get_buffer_ptr(STR_INDEX_index (str_index));
	    if (STR_INDEX_size (str_index) < 0xff) {
		return hash_key (STR::get_str (str),
				      STR_INDEX_size (str_index));
	    } else {
		return hash_key (STR::get_str (str), STR::get_length (str));
	    }
	}
    };


    // equality of two hash_keys
    struct equal {
	BOOL operator() (const hash_key& key1, const hash_key& key2) const {
	    return (key1.size == key2.size &&
		    key1.str[0] == key2.str[0] &&
		    memcmp (key1.str, key2.str, key1.size) == 0);
	}
    };


// [HK]
//     typedef std::hashtable<STR_INDEX, hash_key, hash, extract_key, equal> HASHTABLE;
    typedef __GNUEXT::hashtable<STR_INDEX, hash_key, hash, extract_key, equal> HASHTABLE;

    HASHTABLE hash_table;

    // constructor
    STR_TAB (UINT hash_hint, UINT buffer_hint, MEM_POOL *pool) : 
      str_buffer (buffer_hint == 0 ? str_default_buffer_hint: buffer_hint, pool),
      hash_table (hash_hint == 0 ? str_default_hash_hint: hash_hint,
		  hash (), equal (), extract_key (*this)),
      str_pool(pool)
       {}

    // operations
    
    UINT32 reserve (UINT32 size) {
      return str_buffer.reserve(size);
    }

    void unreserve () {
      str_buffer.unreserve();
    }
    
    void enlarge(UINT32 size) {
      str_buffer.enlarge(size);
    }

    char *get_buffer_ptr(UINT32 idx) {
      return str_buffer.get_buffer_ptr(idx);
    }

    UINT32 insert (const char *str, UINT32 size);

    UINT32 insert (const char *str) {
	return insert (str, strlen (str));
    }

    UINT32 count() { return str_buffer.sb_count; }

    void init_hash ();

    void clear_hash() { hash_table.clear(); }
    void clear_buffer() { str_buffer.clear(); }
    void clear() { clear_buffer(); clear_hash(); }

    HASHTABLE& get_hashtable() { return hash_table; }

    STR_TAB_BUFFER *new_str_tab_buffer();
}; // STR_TAB


template <class STR>
UINT32
STR_TAB<STR>::insert (const char *str, UINT32 size)
{
    UINT32 buffer_size = STR::get_buffer_length (size);
    UINT32 idx = reserve(buffer_size);
    STR::copy(str, size, get_buffer_ptr(idx));

    STR_INDEX new_index = make_STR_INDEX (size, idx);
    STR_INDEX old_index = hash_table.find_or_insert (new_index);

    if (new_index != old_index) {
	// already inserted, reset buffer and return
	unreserve();
	return STR_INDEX_index (old_index);
    } else
	return idx;

} // STR_TAB<STR>::insert

template <class STR>
void
STR_TAB<STR>::init_hash ()
{
  FMT_ASSERT(count() > 0, ("STR_TAB::init_hash: strtab incorrectly initialized"));
  FMT_ASSERT(str_buffer.sb_first == str_buffer.sb_last, ("STR_TAB::init_hash: non continuous buffer"));
  char *buffer = get_buffer_ptr(0);
  UINT32 size = count();
  UINT32 idx = 1; // first entry always null
  while (idx < size) {
    UINT32 length = STR::get_length (buffer+idx);
    hash_table.insert_unique (make_STR_INDEX (length, idx));
    idx += STR::get_buffer_length (length);
  }
}

template <class STR>
STR_TAB_BUFFER *
STR_TAB<STR>::new_str_tab_buffer ()
{
  STR_TAB_BUFFER *str_tab_buffer = CXX_NEW(STR_TAB_BUFFER(count(), str_pool), str_pool);
  STR_BUFFER::buffer_segment *segment = str_buffer.sb_first;
  char *ptr = str_tab_buffer->buffer();
  UINT32 current_count = 0;
  while (segment != str_buffer.sb_last) {
    memcpy(ptr, segment->bs_buffer, segment->bs_size*sizeof(char));
    ptr += segment->bs_size;
    current_count += segment->bs_size;
    segment = segment->bs_next;
  }
  if (segment != NULL) {
    memcpy(ptr, segment->bs_buffer, count() - current_count);
  }
  return str_tab_buffer;
}

template <class STRTAB>
static inline void
initialize_strtab (STRTAB& strtab, UINT32 size)
{
  FMT_ASSERT(size > 0, ("initialize_strtab():0 size for initialization"));
  FMT_ASSERT(strtab.count() == 0, ("initialize_strtab():Invalid initialization, non empty buffer"));
  FMT_ASSERT(strtab.hash_table.size() == 0, ("initialize_strtab():Invalid initialization, non empty hash"));
  strtab.enlarge(size);
  strtab.reserve(1);
  strtab.get_buffer_ptr(0)[0] = 0;
}


template <class STRTAB>
static inline void
initialize_strtab (STRTAB& strtab, const char *buf, UINT32 size)
{
  FMT_ASSERT(size > 0, ("initialize_strtab():0 size for initialization"));
  FMT_ASSERT(strtab.count() == 0, ("initialize_strtab():Invalid initialization, non empty buffer"));
  FMT_ASSERT(strtab.hash_table.size() == 0, ("initialize_strtab():Invalid initialization, non empty hash"));
  strtab.enlarge(size);
  strtab.reserve(size);
  BCOPY (buf, strtab.get_buffer_ptr(0), size);
  strtab.init_hash ();
  
} // Initialize_Strtab
    

STR_TAB_BUFFER::STR_TAB_BUFFER(UINT32 size, MEM_POOL *pool)
{
  stb_size = size; 
  stb_buffer = CXX_NEW_ARRAY(char, size, pool);
  stb_pool = pool;
}

STR_TAB_BUFFER::~STR_TAB_BUFFER()
{
  CXX_DELETE_ARRAY(stb_buffer, stb_pool);
}


// 
// Define TEST on the command line to create an executable test
// from the compilation of this file:
// $ g++ -DTEST strtab.cxx -o strtab_test
// $ ./strtab_test
//
#ifndef TEST

// Memory pool for string tables
#define STRTAB_POOL Malloc_Mem_Pool

// Global String table
static const UINT32 STRTAB_HASH_SIZE = 1024;
static const UINT32 STRTAB_BUFFER_SIZE = 1024*64;
static STR_TAB<NULL_TERMINATED_STRING> Strtab (STRTAB_HASH_SIZE, STRTAB_BUFFER_SIZE, STRTAB_POOL);			       

BE_EXPORTED STRING_TABLE Str_Table;

UINT32 
STR_Table_Size()
{
  return Strtab.count();
}

STR_TAB_BUFFER *
Strtab_new_str_tab_buffer()
{
  return Strtab.new_str_tab_buffer();
}

// init an empty table; use by producer (e.g., front end)

void
Initialize_Strtab (UINT32 size)
{
    initialize_strtab (Strtab, size);
} 

// initialized the string table with the strtab from an input file
void
Initialize_Strtab (const char *buf, UINT32 size)
{
    initialize_strtab (Strtab, buf, size);
} // Initialize_Strtab


STR_IDX
Save_Str (const char *str)
{
    if (str == NULL)
	return 0;

    return Strtab.insert (str);

} // Save_Str


STR_IDX
Save_Str2 (const char *s1, const char *s2)
{
    UINT len = strlen (s1) + strlen(s2) + 1;
    char *new_str = (char *) alloca (len);
    strcpy (new_str, s1);
    strcat (new_str, s2);
    return Save_Str (new_str);
} // Save_Str2

STR_IDX
Save_Str2i (const char *s1, const char *s2, UINT i)
{
    UINT len = strlen (s1) + strlen(s2) + 17;
    char *new_str = (char *) alloca (len);
    sprintf(new_str, "%s%s%d", s1, s2, i);
    return Save_Str (new_str);
} // Save_Str2


char *
Index_To_Str (STR_IDX idx)
{
    Is_True (idx < Strtab.count(), ("Invalid STR_IDX"));
    return NULL_TERMINATED_STRING::get_str (Strtab.get_buffer_ptr(idx));
}


// character array table
static const UINT32 TCON_STRTAB_HASH_SIZE = 512;
static const UINT32 TCON_STRTAB_BUFFER_SIZE = 512*64;
static STR_TAB<CHARACTER_ARRAY> TCON_strtab (TCON_STRTAB_HASH_SIZE, TCON_STRTAB_BUFFER_SIZE, STRTAB_POOL);

UINT32
TCON_strtab_size()
{
  return TCON_strtab.count();
}

STR_TAB_BUFFER *
TCON_new_str_tab_buffer()
{
  return TCON_strtab.new_str_tab_buffer();
}

// init an empty table; use by producer
void
Initialize_TCON_strtab (UINT32 size)
{
    initialize_strtab (TCON_strtab, size);
}

// init the TCON strtab from an input file
void
Initialize_TCON_strtab (const char *buf, UINT32 size)
{
    initialize_strtab (TCON_strtab, buf, size);
} 



// add string of length "len"; string might not be null-terminated
UINT32
Save_StrN (const char *s1, UINT32 len)
{
    if (len == 0)
	return 0;

    return TCON_strtab.insert (s1, len);
} // Save_StrN

char *
Index_to_char_array (UINT32 idx)
{
    Is_True (idx < TCON_strtab.count(), ("Invalid TCON str index"));
    return CHARACTER_ARRAY::get_str (TCON_strtab.get_buffer_ptr(idx));
}



#ifdef MONGOOSE_BE
template <class STR, class MAP>
void
merge_strtab (STR_TAB<STR>& strtab, const char *buf, UINT32 size, MAP& map)
{
    map[0] = 0;
    UINT32 idx = 1;
    while (idx < size) {
	const char *str = STR::get_str (buf + idx);
	UINT32 length = STR::get_length (buf + idx);
	UINT32 new_idx = strtab.insert (str, length);
	map[idx] = new_idx;
	idx += STR::get_buffer_length (length);
    }
} // merge_strtab


// merge in a string table buffer from an input file, return a map from
// the old STR_IDX to the new (merged) STR_IDX

void
Merge_Strtab (const char *buf, UINT32 size, STR_IDX_MAP& map)
{
    merge_strtab (Strtab, buf, size, map);
}

// merge in a string table buffer from an input file, return a map from
// the old STR_IDX to the new (merged) STR_IDX

void
Merge_TCON_Strtab (const char *buf, UINT32 size, STR_IDX_MAP& map)
{
    merge_strtab (TCON_strtab, buf, size, map);
}

#endif // MONGOOSE_BE

#endif // !defined(TEST)

#ifdef TEST

static char *
gen_string(int length)
{
  static char buffer[65536];
  int i;
  for (i = 0; i < length && i < sizeof(buffer)-1; i++) {
    buffer[i] = (char)rand();
    if (buffer[i] == 0) buffer[i]++;
    
  }
  buffer[i] = '\0';
  return buffer;
}

int main() {
  int i;
  STR_TAB<NULL_TERMINATED_STRING> strtab(16, 256, NULL);

  for (i = 0; i < 256; i++) {
    int length = i;
    strtab.insert(gen_string(length));
  }

  for (i = 0; i < 100; i++) {
    int length = rand() & 0xffff;
    strtab.insert(gen_string(length));
  }
  
  STR_TAB_BUFFER *buffer = strtab.new_str_tab_buffer();
  printf("Buffer size: %u\n", buffer->size());
  FMT_ASSERT(buffer->size() == strtab.count(), ("buffer size mistmatch\n"));
  CXX_DELETE(buffer, buffer->pool());

  return 0;
}

#endif // TEST

#else //!defined(TARG_ST)
// [CG]: Below is the old implementation

#ifdef USE_PCH
#include "common_com_pch.h"
#endif /* USE_PCH */
#pragma hdrstop

#include "W_alloca.h"
#ifndef __MINGW32__
#include <strings.h>
#endif

// [HK]
#if __GNUC__ >= 3
#include <ext/hash_map>
#else
#include <hash_map>			// stl hash table
#endif
#include "namespace_trans.h"

#include "defs.h"
#include "errors.h"
#include "cxx_memory.h"			// for CXX_NEW

#include "strtab.h"

// The string table is implmeneted as a single character buffer, re-allocated
// if necessary.  Byte 0 of this buffer is alwasy NULL.  Each string is
// copied to the buffer once.  Duplicates are not entered.  A simple hash
// table is used to for quick lookup.  The hash table is a fixed size
// array of STR_BUCKET, each of which a dynamic array of <idx,size> pair.
// The size field is truncated to 8 bits.  It is used for 
// quick comparision to minimize the need to call strcmp.

// we support two types of strings: C-style null-terminated string and
// character array with a specified size (may contain null character within
// the array.

typedef UINT32 STR_INDEX;

#if defined(linux) || defined(sun) || defined(__CYGWIN__) || defined(__MINGW32__)

#define STR_INDEX_size(xxx) (xxx & 0xff)
#define STR_INDEX_index(xxx) (xxx >> 8)

#else

static inline mUINT8
STR_INDEX_size (STR_INDEX idx)		{ return idx & 0xff; }
static inline mUINT32
STR_INDEX_index (STR_INDEX idx)		{ return idx >> 8; }

#endif /* linux || sun */

static inline STR_INDEX
make_STR_INDEX (UINT32 size, UINT32 idx)
{
    // if string length larger than 0xff, just use 0xff
    if (size > 0xff)
	size = 0xff;
    return (STR_INDEX) ((idx << 8) | size);
}

struct NULL_TERMINATED_STRING
{
    static char *get_str (char *buffer) {
	return buffer;
    }

    static const char *get_str (const char *buffer) {
	return buffer;
    }

    static UINT32 get_length (const char *buffer) {
	return strlen (buffer);
    }

    static UINT32 get_buffer_length (UINT32 length) {
	return length + 1;		// length + null character
    }


    static void copy (const char *str, UINT32 length, char *buffer) {
	memcpy (buffer, str, length+1);
    }
};


union UNALIGN_INT32
{
    char ch[sizeof(UINT32)];
    UINT32 n;

    UNALIGN_INT32 (UINT32 x) : n (x) {}

    UNALIGN_INT32 (const char *s) {
	for (INT32 i = 0; i < sizeof(UINT32); ++i)
	    ch[i] = *s++;
    }
};


struct CHARACTER_ARRAY
{
    // If the character array is less then 0xff bytes, we store the size in
    // The First Byte Followed By The Array.  Otherwise, The First Byte Is Set
    // To 0xff And The Next 4 Bytes Hold The Size Of The Array As An Unsigned
    // Integer, And The Array Follows.

    static const char *get_str (const char *buffer) {
	if (*buffer != 0xff)
	    return buffer + 1;
	else
	    return buffer + sizeof(UINT32) + 1;
    }

    static char *get_str (char *buffer) {
	if (*buffer != 0xff)
	    return buffer + 1;
	else
	    return buffer + sizeof(UINT32) + 1;
    }

    static UINT32 get_length (const char *buffer) {
	if (*buffer != 0xff)
	    return *buffer;
	else {
	    UNALIGN_INT32 unalign (buffer + 1);
	    return unalign.n;
	}
    }

    static UINT32 get_buffer_length (UINT32 length) {
	return length < 0xff ? length + 1 : length + 1 + sizeof(UINT32);
    }

    static void copy (const char *str, UINT32 length, char *buffer) {
	if (length < 0xff) {
	    *buffer++ = length;
	} else {
	    *buffer++ = 0xff;
	    UNALIGN_INT32 unalign (length);
	    for (INT32 i = 0; i < sizeof(UINT32); ++i)
		*buffer++ = unalign.ch[i];
	}
	memcpy (buffer, str, length);
    }

}; // CHARACTER_ARRAY


template <class STR>
struct STR_TAB
{
    char *buffer;
    STR_IDX last_idx;
    UINT32 buffer_size;

    // the following 4 function objects are required for setting up the
    // hash table declared after them.

    // hash key, which is a pair of string pointer and string length
    struct hash_key {
	const char *str;
	mUINT32 size;

	hash_key () {}
	
	hash_key (const char *s, UINT32 l) : str (s), size (l) {}
	
	hash_key (const hash_key& p) : str (p.str), size (p.size) {}
    };


    // hash function
    struct hash {
	UINT32 operator() (const hash_key &key) const {
	    UINT32 h = 0;
	    const UINT32 length = key.size;
	    const char *str = key.str;
	    for (UINT32 i = 0; i < length; ++i)
		h = 5 * h + str[i];
	    return h;
	}
    };
	    

    // how the hashtable convert a STR_INDEX into a hash_key
    struct extract_key {
	const STR_TAB<STR>& strtab;

	extract_key (const STR_TAB<STR>& tab) : strtab (tab) {}
	
	hash_key operator() (STR_INDEX str_index) const {
	    const char *str = strtab.buffer + STR_INDEX_index (str_index);
	    if (STR_INDEX_size (str_index) < 0xff) {
		return hash_key (STR::get_str (str),
				      STR_INDEX_size (str_index));
	    } else {
		return hash_key (STR::get_str (str), STR::get_length (str));
	    }
	}
    };


    // equality of two hash_keys
    struct equal {
	BOOL operator() (const hash_key& key1, const hash_key& key2) const {
	    return (key1.size == key2.size &&
		    key1.str[0] == key2.str[0] &&
		    memcmp (key1.str, key2.str, key1.size) == 0);
	}
    };

    // [HK]
//     typedef std::hashtable<STR_INDEX, hash_key, hash, extract_key, equal> HASHTABLE;
    typedef __GNUEXT::hashtable<STR_INDEX, hash_key, hash, extract_key, equal> HASHTABLE;

    HASHTABLE hash_table;

    // constructor
    STR_TAB (UINT bucket_size) : hash_table (bucket_size, hash (), equal (),
					     extract_key (*this)),
	buffer (NULL), last_idx (0), buffer_size (0) {}
    // operations
    
    void reserve (UINT32 size) {
	if (size > buffer_size - last_idx) {
	    // realloc
	    while (size > buffer_size - last_idx) {
		if (buffer_size < 0x100000)
		    buffer_size *= 2;
		else
		    buffer_size += 0x80000;
	    }
	    buffer = (char *) MEM_POOL_Realloc (Malloc_Mem_Pool, buffer, 0,
						buffer_size); 
	}
    }
    
    void init_hash ();

    UINT32 insert (const char *str, UINT32 size);

    UINT32 insert (const char *str) {
	return insert (str, strlen (str));
    }
    
    void copy_str (const char *str, UINT32 size);

    HASHTABLE& get_hashtable() { return hash_table; }

}; // STR_TAB


template <class STR>
UINT32
STR_TAB<STR>::insert (const char *str, UINT32 size)
{
    UINT32 index = last_idx;

    copy_str (str, size);

    STR_INDEX new_index = make_STR_INDEX (size, index);
    STR_INDEX old_index = hash_table.find_or_insert (new_index);

    if (new_index != old_index) {
	// already inserted, reset buffer and return
	last_idx = index;
	return STR_INDEX_index (old_index);
    } else
	return index;

} // STR_TAB<STR>::insert


template <class STR>
void
STR_TAB<STR>::copy_str (const char *str, UINT32 size)
{
    UINT32 buffer_size = STR::get_buffer_length (size);
    reserve (buffer_size);
    STR::copy (str, size, buffer + last_idx);
    last_idx += buffer_size;
} // STR_TAB::copy_str


template <class STR>
void
STR_TAB<STR>::init_hash ()
{
    UINT32 idx = 1;			// first entry always null
    while (idx < last_idx) {

	UINT32 length = STR::get_length (buffer + idx);
	hash_table.insert_unique (make_STR_INDEX (length, idx));
	idx += STR::get_buffer_length (length);
    }
} // STR_TAB<STR>::init_hash


template <class STRTAB>
static inline void
initialize_strtab (STRTAB& strtab, UINT32 size)
{
    strtab.buffer_size = size;
    strtab.buffer = (char *) MEM_POOL_Alloc (Malloc_Mem_Pool, size);
    strtab.buffer[0] = 0;
    strtab.last_idx = 1;
}


template <class STRTAB>
static inline void
initialize_strtab (STRTAB& strtab, const char *buf, UINT32 size)
{
    if (strtab.get_hashtable().size() != 0)
        strtab.get_hashtable().erase(strtab.get_hashtable().begin(),
                                strtab.get_hashtable().end());
    strtab.buffer_size = size;
    strtab.buffer = (char *) MEM_POOL_Alloc (Malloc_Mem_Pool, size);
    BCOPY (buf, strtab.buffer, size);
    strtab.last_idx = size;
    strtab.init_hash ();
    
} // Initialize_Strtab
    

// Global String table
static STR_TAB<NULL_TERMINATED_STRING> Strtab (1000);			       

BE_EXPORTED STRING_TABLE Str_Table;

UINT32
STR_Table_Size ()
{
    return Strtab.last_idx;
}

// init an empty table; use by producer (e.g., front end)

void
Initialize_Strtab (UINT32 size)
{
    initialize_strtab (Strtab, size);
} 

// initialized the string table with the strtab from an input file
void
Initialize_Strtab (const char *buf, UINT32 size)
{
    initialize_strtab (Strtab, buf, size);
} // Initialize_Strtab


STR_IDX
Save_Str (const char *str)
{
    if (str == NULL)
	return 0;

    return Strtab.insert (str);

} // Save_Str


STR_IDX
Save_Str2 (const char *s1, const char *s2)
{
    UINT len = strlen (s1) + strlen(s2) + 1;
    char *new_str = (char *) alloca (len);
    strcpy (new_str, s1);
    strcat (new_str, s2);
    return Save_Str (new_str);
} // Save_Str2

STR_IDX
Save_Str2i (const char *s1, const char *s2, UINT i)
{
    UINT len = strlen (s1) + strlen(s2) + 17;
    char *new_str = (char *) alloca (len);
    sprintf(new_str, "%s%s%d", s1, s2, i);
    return Save_Str (new_str);
} // Save_Str2


char *
Index_To_Str (STR_IDX idx)
{
    Is_True (idx < Strtab.last_idx, ("Invalid STR_IDX"));
    return NULL_TERMINATED_STRING::get_str (Strtab.buffer + idx);
}

// character array table
const UINT32 TCON_STRTAB_HASH_SIZE = 512;
static STR_TAB<CHARACTER_ARRAY> TCON_strtab (TCON_STRTAB_HASH_SIZE);

UINT32
TCON_strtab_size ()
{
    return TCON_strtab.last_idx;
}

char *
TCON_strtab_buffer ()
{
    return TCON_strtab.buffer;
}

// init an empty table; use by producer
void
Initialize_TCON_strtab (UINT32 size)
{
    initialize_strtab (TCON_strtab, size);
}

// init the TCON strtab from an input file
void
Initialize_TCON_strtab (const char *buf, UINT32 size)
{
    initialize_strtab (TCON_strtab, buf, size);
} 



// add string of length "len"; string might not be null-terminated
UINT32
Save_StrN (const char *s1, UINT32 len)
{
    if (len == 0)
	return 0;

    return TCON_strtab.insert (s1, len);
} // Save_StrN

char *
Index_to_char_array (UINT32 idx)
{
    Is_True (idx < TCON_strtab.last_idx, ("Invalid TCON str index"));
    return CHARACTER_ARRAY::get_str (TCON_strtab.buffer + idx);
}


#ifdef MONGOOSE_BE

template <class STR, class MAP>
void
merge_strtab (STR_TAB<STR>& strtab, const char *buf, UINT32 size, MAP& map)
{
    map[0] = 0;
    UINT32 idx = 1;
    while (idx < size) {
	const char *str = STR::get_str (buf + idx);
	UINT32 size = STR::get_length (buf + idx);
	UINT32 new_idx = strtab.insert (str, size);
	map[idx] = new_idx;
	idx += STR::get_buffer_length (size);
    }
} // merge_strtab


// merge in a string table buffer from an input file, return a map from
// the old STR_IDX to the new (merged) STR_IDX

void
Merge_Strtab (const char *buf, UINT32 size, STR_IDX_MAP& map)
{
    merge_strtab (Strtab, buf, size, map);
}

// merge in a string table buffer from an input file, return a map from
// the old STR_IDX to the new (merged) STR_IDX

void
Merge_TCON_Strtab (const char *buf, UINT32 size, STR_IDX_MAP& map)
{
    merge_strtab (TCON_strtab, buf, size, map);
}

#endif // MONGOOSE_BE

#endif //!defined(TARG_ST)

