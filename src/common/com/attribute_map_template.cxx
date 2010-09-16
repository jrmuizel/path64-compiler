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
 *
 * Support classes for named attributes, attribute masks, attribute maps and
 * attribute remappers.
 *
 * Attributes are named properties, for instance "speculative".
 * Attribute maps are used to map attributes to identifier, for instance "speculative" has
 * attribute identifier 10.
 * Attribute mask are used to represent a set of properties, for instance "speculative" and
 * "load" can be represented by an attribute mask.
 * Attribute remappers are used to remap by attribute name the identifiers of an attribute
 * map to the identifiers of another attribute map.
 *
 * As an example, a client of this module is the extension loader. 
 * The extension loader have on one side the attributes defined
 * in the static ISA with the static identifiers defined by the static targinfo.
 * On the other side it has to load at run time a set of extension such that, 
 * each extension uses a subset of the static ISA attributes and as a specific
 * numbering for them.
 * The work of the loader is to remap the attribute table of each extension to
 * the actual static ISA attribute mapping, such that the attribute 
 * mask (TOP properties) of each extension operator (TOP) can be correctly
 * interpreted by the compiler.
 * 
 */

#include <map>

#ifndef NAMESPACE
#define NAMESPACE /* anonymous */
#endif

namespace NAMESPACE {
  using namespace std;

  /*
   * NAME_VALUE
   * Associates a name with a value stored as an integer
   * array of type TYPE[WORD_COUNT]. 
   */
  template <typename TYPE, int WORD_COUNT = 1>
  struct NAME_VALUE { const char *name; TYPE value[WORD_COUNT]; };

  /*
   * ATTRIBUTE_MASK
   * Encapsulates a pointer to a mask of type TYPE[word_count].
   * The mask is not copied, it is just a reference to it.
   * This class provides operators on masks. 
   */
  template <typename TYPE, int WORD_COUNT = 1>
  class ATTRIBUTE_MASK {
  private:
    TYPE *mask_;
    int word_count_;
  public:
    typedef TYPE word_type;
    ATTRIBUTE_MASK(TYPE *mask, int word_count = WORD_COUNT) :
      mask_(mask), word_count_(word_count) {}

    int
    wordCount(void) const {
      return word_count_;
    }

    int
    bitCount(void) const {
      return word_count_ * sizeof(word_type) * 8;
    }

    void
    copyMask(const ATTRIBUTE_MASK &src_mask) {
      int i;
      for (i = 0; i < word_count_; i++) {
	mask_[i] = src_mask.mask_[i];
      }
      
    }

    void 
    assignToMask(TYPE *dst_mask) const {
      int i;
      for (i = 0; i < word_count_; i++) {
	dst_mask[i] = mask_[i];
      }
    }
      
    void 
    initFromMask(TYPE *src_mask) const {
      int i;
      for (i = 0; i < word_count_; i++) {
	mask_[i] = src_mask[i];
      }
    }

    void 
    clear(void) const {
      int i;
      for (i = 0; i < word_count_; i++) {
	mask_[i] = 0;
      }
    }

    void 
    setBit(int bit) const {
      int i;
      int word_idx = bit / (sizeof(TYPE)*8);
      int bit_idx = bit % (sizeof(TYPE)*8);
      mask_[word_idx] |= (TYPE)1 << bit_idx;
    }

    void 
    clearBit(int bit) const {
      int i;
      int word_idx = bit / (sizeof(TYPE)*8);
      int bit_idx = bit % (sizeof(TYPE)*8);
      mask_[word_idx] &= ~((TYPE)1 << bit_idx);
    }

    bool
    isBitSet(int bit) const {
      int i;
      int word_idx = bit / (sizeof(TYPE)*8);
      int bit_idx = bit % (sizeof(TYPE)*8);
      return (mask_[word_idx] & ((TYPE)1 << bit_idx)) != 0;
    }
  };

    
  /*
   * ATTRIBUTE_MAP
   * Maps an attribute name to an attribute identifier (or value).
   * The map is constructed from an initial array of NAME_VALUE.
   * The initial array can store: either the actual identifier or the 
   * value mask (1<<identifier). In the last case the mask is converted
   * into the attribute identifier before being stored.
   * the actual mapping is thus in both cases a mapping from the attribute
   * name to the attribute identifier.
   * The getValue() function returns -1 if the attribute name is not found.
   * The getName() function returns NULL if the attribute value i not found.
   */
  template <typename TYPE, typename NAME_ALLOCATOR = std::allocator<const char *>, typename VALUE_ALLOCATOR = std::allocator<int> >
  class ATTRIBUTE_MAP {
  private:
    struct attribute_name_lt {
      bool operator()(const char* s1, const char* s2) const
      {
	return strcmp(s1, s2) < 0;
      }
    };

    struct attribute_value_lt {
      bool operator()(const TYPE v1, const TYPE v2) const
      {
	return v1 < v2;
      }
    };
    
    typedef map<const char *, int, attribute_name_lt, VALUE_ALLOCATOR> map_t;
    typedef map<int, const char *, attribute_value_lt, NAME_ALLOCATOR > reverse_map_t;
      
    map_t map_;
    reverse_map_t rmap_;
      
    static int
    bitOf(const TYPE mask) {
      int i;
      for (i = 0; i < sizeof(TYPE) * 8; i++) {
	if ((mask & ((TYPE)1 << i)) != 0) {
	  return i;
	}
      }
      return -1;
    }

  public:
    typedef TYPE value_type;
    enum map_kind {
      KIND_VALUE,
      KIND_MASK
    };
    
    ATTRIBUTE_MAP(const NAME_VALUE<TYPE> *table, int table_size, map_kind kind = KIND_VALUE) {
      int i;
      for (i = 0; i < table_size; i++) {
	int value;
	TYPE datum = table[i].value[0];
	if ((kind == KIND_MASK && datum == 0) ||
	    (kind == KIND_VALUE && datum == -1)) continue; /* Ignore attribute with no value (all 0 as mask, or -1 as identifier). */
	value =  kind == KIND_MASK ? bitOf(datum) : (int)datum;
	map_.insert(typename map_t::value_type(table[i].name, value));
	rmap_.insert(typename reverse_map_t::value_type(value, table[i].name));
      }
    }
      
    int
    getValue(const char *name) const {
      typename map_t::const_iterator it = map_.find(name);
      if (it == map_.end()) return -1;
      return (*it).second;
    }

    const char *
    getName(int value) const {
      typename reverse_map_t::const_iterator it = rmap_.find(value);
      if (it == rmap_.end()) return NULL;
      return (*it).second;
    }
  };

    
  /*
   * ATTRIBUTE_REMAPPER
   * Remaps an attribute value by name from the src_map to its value
   * in the dst_map.
   * The complexity or remapValue is O(log(n)) (n is the number of attributes).
   * The complexity of remapMask is O(M*log(n)) (M is the size of the source mask).
   * The current implementation is not effective at there is no
   * constructed cache for the remapping. Complexity could be O(1) (resp. O(M) for remapMask)
   * with an additional size of O(n) under the assumption that the source attribute values are
   * "dense" identifiers.
   * The remapMask() and remapMaskPr() function return false if the remapping could not be done.
   * The functions allow overlapping mask when remapping.
   */
  template <typename SRC_MAP, typename DST_MAP, 
	    typename SRC_TYPE = typename SRC_MAP::value_type, typename DST_TYPE = typename DST_MAP::value_type,
	    int SRC_WORD_COUNT = 1, int DST_WORD_COUNT = 1>
  class ATTRIBUTE_REMAPPER {
  private:
    const SRC_MAP &src_map_;
    const DST_MAP &dst_map_;
  public:

    ATTRIBUTE_REMAPPER(const SRC_MAP& src_map, const DST_MAP& dst_map) :
      src_map_(src_map), dst_map_(dst_map) {}
      
    int
    remapValue(int value) const {
      const char *name = src_map_.getName(value);
      if (name == NULL) return -1;
      return dst_map_.getValue(name);
    }

    bool
    remapMask(const SRC_TYPE &src, DST_TYPE &dst) const {
      return remapMaskPtr(&src, &dst);
    }

    bool
    remapMaskPtr(const SRC_TYPE *src, DST_TYPE *dst, 
	      int src_word_count = SRC_WORD_COUNT, int dst_word_count = DST_WORD_COUNT) const {
      bool ok = true;
      int i;
      ATTRIBUTE_MASK<const SRC_TYPE> src_mask(src, src_word_count);
      ATTRIBUTE_MASK<DST_TYPE> dst_mask(dst, dst_word_count);
      DST_TYPE tmp[dst_word_count];
      ATTRIBUTE_MASK<DST_TYPE> tmp_mask(tmp, dst_word_count);

      tmp_mask.clear();
      for (i = 0; i < src_mask.bitCount(); i++) {
	if (src_mask.isBitSet(i)) {
	  int dst_bit = (int)remapValue(i);
	  if (dst_bit == -1) {
	    ok = false;
	    break;
	  }
	  tmp_mask.setBit(dst_bit);
	}
      }
      dst_mask.copyMask(tmp_mask);
      return ok;
    }
  };

} /* namepace NAMESPACE */

#ifdef TEST
#include <iostream>

namespace /* anonymous */ {
  void
  Test(void)
  {
    typedef unsigned char INT8;
    typedef unsigned int INT32;
    typedef unsigned long long INT64;
    typedef ATTRIBUTE_MAP<INT32> INT32_MAP;
    typedef ATTRIBUTE_MAP<INT64> INT64_MAP;
    NAME_VALUE<INT32> int_table[] = {{"one", 1}, {"two", 2}, 
				     {"large", 63}, {"very large", 255}};
    NAME_VALUE<INT64> int64_table[] = {{"one", 1LL}, {"two", 2LL},
				       {"large", 63LL}, {"very large", 255LL}};
    NAME_VALUE<INT64> mask_table[] = {{ "one", 1LL<<1}, {"two", 1LL<<2},
				      {"large", 1LL<<63}};
    NAME_VALUE<INT32> unordered_table[] = {{"one", 0}, {"two", 1}, 
					   {"large", 2}, {"very large", 3}};
    INT32_MAP int_map(int_table, sizeof(int_table)/sizeof(*int_table));
    INT64_MAP int64_map(int64_table, sizeof(int64_table)/sizeof(*int64_table));
    INT64_MAP mask_map(mask_table, sizeof(mask_table)/sizeof(*mask_table),
		       INT64_MAP::KIND_MASK);
    INT32_MAP unordered_map(unordered_table, 
			    sizeof(unordered_table)/sizeof(*unordered_table));
    ATTRIBUTE_REMAPPER<INT32_MAP, INT64_MAP > 
      remapper(unordered_map, int64_map);
    ATTRIBUTE_REMAPPER<INT64_MAP, INT32_MAP, INT64, INT8, 4, 4 > 
      remapper_2(int64_map, unordered_map);
    INT64 mask64;
    INT8 mask8_4[4];
    INT64 mask64_4[4];
    INT32 mask32;
    
    cout << "one is int " << int_map.getValue("one") << endl;
    cout << "two is int " << int_map.getValue("two") << endl;
    cout << "large is int " << int_map.getValue("large") << endl;
    cout << "very large is int " << int_map.getValue("very large") << endl;
    cout << "int 1 is named " << int_map.getName(1) << endl;
    cout << "int 2 is named " << int_map.getName(2) << endl;
    cout << "one is int64 " << int64_map.getValue("one") << endl;
    cout << "two is int64 " << int64_map.getValue("two") << endl;
    cout << "large is int " << int64_map.getValue("large") << endl;
    cout << "very large is int " << int64_map.getValue("very large") << endl;
    cout << "int64 1 is named " << int64_map.getName(1) << endl;
    cout << "int64 2 is named " << int64_map.getName(2) << endl;
    cout << "one in mask table is " << mask_map.getValue("one") << endl;
    cout << "two in mask table is " << mask_map.getValue("two") << endl;
    cout << "large in mask table is " << mask_map.getValue("large") << endl;
    cout << "mask 1 is named " << mask_map.getName(1) << endl;
    cout << "mask 2 is named " << mask_map.getName(2) << endl;
    cout << "mask 63 is named " << mask_map.getName(63) << endl;
    
    cout << "one is unordered " << unordered_map.getValue("one") << endl;
    cout << "two is unordered " << unordered_map.getValue("two") << endl;
    cout << "large is unordered " << unordered_map.getValue("large") << endl;

    cout << "1 in unordered is remapped to " << remapper.remapValue(1) << endl;
    cout << "2 in unordered is remapped to " << remapper.remapValue(2) << endl;
    mask32 = 1 << unordered_map.getValue("two");
    mask32 |= 1 << unordered_map.getValue("large");
    cout << "two|large in unordered map is " << mask32 << endl;
    remapper.remapMask(mask32, mask64);
    cout << "two|large in int64 map is " << mask64 << endl;
    mask32 |= 1 << unordered_map.getValue("very large");
    cout << "two|large|very large in unordered map is " << mask32 << endl;
    remapper.remapMaskPtr(&mask32, &mask64_4[0], 1, 4);
    cout << "two|large|very large in int64 map is " << mask64_4[0] << ":" <<
      mask64_4[1] << ":" << mask64_4[2] << ":" << mask64_4[3] << endl;

    remapper_2.remapMaskPtr(&mask64_4[0], &mask8_4[0]);
    cout << "two|large|very large in unordered map is " << (int)mask8_4[0] << ":" <<
      (int)mask8_4[1] << ":" << (int)mask8_4[2] << ":" << (int)mask8_4[3] << endl;
    
    
  }
} /* namespace */

int 
main() {
  Test();
  return 0;
}
#endif
