/* Macros_new.h */

#define BF_range(value, msb, lsb) ((((value) >> lsb) & DATA_VALUE_MASK(msb-lsb+1)))
