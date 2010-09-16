/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

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


// isa_lits_gen.cxx
/////////////////////////////////////
//
//  Generate a list of lit classes and their values.
//
/////////////////////////////////////
//
//  $Revision: 1.4 $
//  $Date: 05/05/06 08:20:23-07:00 $
//  $Author: bos@eng-25.pathscale.com $
//  $Source: common/targ_info/generate/SCCS/s.isa_lits_gen.cxx $

#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include "gen_util.h"
#include "isa_lits_gen.h"

struct lit_range {
  const char *name;
  long long min;
  long long max;
#ifdef TARG_ST
  // [JV] Add scaling info via bits mask on lowest bits
  long scaling_value;
  long long scaling_mask;
  long max_right_rotate;
  long long right_rotate_mask;
#endif
};

static const char * const interface[] = {
  "/* ====================================================================",
  " * ====================================================================",
  " *",
  " * Description:",
  " *",
  " *   A list of all the lit classes used in an ISA.",
  " *   It exports the following:",
  " *",
  " *   typedef (enum) ISA_LIT_CLASS",
  " *       An enumeration of the lit classes.",
  " *",
  " *   typedef (struct) ISA_LIT_CLASS_INFO",
  " *       Contains info about first and last ECV in the EC.",
  " *       The contents are private.",
  " *",
  " *   typedef (struct) ISA_LIT_CLASS_VALUE_INFO",
  " *       Contains info about name and min/max of the LC.",
  " *       The contents are private.",
  " *",
  " *   const char * ISA_LC_Name (ISA_LIT_CLASS lc)",
  " *       Returns name of <lc>.",
  " *",
  " *   INT64 ISA_LC_Min (ISA_LIT_CLASS lc)",
  " *       Returns the minimum value for the specified <lc>. For classes",
  " *       that have multiple sub-ranges, ISA_LC_Min returns the smallest",
  " *       minimum of all the sub-ranges.",
  " *",
  " *   INT64 ISA_LC_Max (ISA_LIT_CLASS lc)",
  " *       Returns the maximum value for the specified <lc>. For classes",
  " *       that have multiple sub-ranges, ISA_LC_Max returns the largest",
  " *       maximum of all the sub-ranges.",
  " *",
  " *   BOOL ISA_LC_Is_Signed (ISA_LIT_CLASS lc)",
  " *       Returns whether the lit-class <lc> is signed.",
  " *",
  " *   BOOL ISA_LC_Value_In_Class (INT64 val, ISA_LIT_CLASS lc)",
  " *       Returns whether <val> is a value that belongs to <lc>.",
  " *",
  " * ====================================================================",
  " * ====================================================================",
  " */",
  NULL
};

#ifdef TARG_ST
static FILE *hfile = NULL;
static FILE *cfile = NULL;
static FILE *sfile = NULL;
static FILE *efile = NULL;

static struct lit_range signed_range[65];
static struct lit_range unsigned_range[65];
static int max_ranges = 0;

static char *hfilename     = NULL;
static char *cfilename     = NULL;
static char *sfilename     = NULL;
static char *efilename     = NULL;

// Whether we generate code for the core or for
// an extension.
static bool gen_static_code = false;

// Literals classes are now much more flexible
// than before (multi ranges, scaling factor,
// non power of 2 bounds, ...).
// They are also heavily used by extension.
// The limit has then been raised from 255
// (fit on u8) to 65535 (fit on u16)
static       int lc_count     = 0;
static const int lc_max_limit = 65535;

// C file generated when static code generation is done,
// included in C ISA_LIT_info table when dynamic code
// generation is done.
static FILE *cincfile = NULL;
static char *cincfilename  = NULL;

/////////////////////////////////////
void ISA_Lits_Begin (void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  gen_static_code = Is_Static_Code();

  const char* const extname  = gen_static_code ? NULL: Get_Extension_Name();

  // Opening files
  // Header file
  hfilename = Gen_Build_Filename(FNAME_TARG_ISA_LITS,
                                 extname,
                                 gen_util_file_type_hfile);
  hfile     = Gen_Open_File_Handle(hfilename,"w");

  // C file
  cfilename = Gen_Build_Filename(FNAME_TARG_ISA_LITS,
                                 extname,
                                 gen_util_file_type_cfile);
  cfile     = Gen_Open_File_Handle(cfilename,"w");

  // Export file
  if(gen_static_code) {
    efilename = Gen_Build_Filename(FNAME_TARG_ISA_LITS,
				   extname,
				   gen_util_file_type_efile);
    efile     = Gen_Open_File_Handle(efilename,"w");
  }
  else {
    const char *headers[] = {
      "<stddef.h>",
      "\"dyn_" FNAME_ISA_LITS ".h\"",
      "",
    };

    sfilename = Gen_Build_Filename(FNAME_STUB_ISA_LITS,
				   extname,
				   gen_util_file_type_cfile);
    sfile     = Gen_Open_File_Handle(sfilename,"w");
    Emit_Stub_Header(sfile,headers);
  }

  // C file generated when static code generation is done,
  // included in C ISA_LIT_info table when dynamic code
  // generation is done.
  cincfilename = Gen_Build_Filename(FNAME_TARG_ISA_LITS,
                                    NULL, /* Build at static code gen. time*/
                                    gen_util_file_type_c_i_file);
  if(gen_static_code) {
    cincfile  = Gen_Open_File_Handle(cincfilename,"w");
    
    fprintf(cincfile,
	    "\n\n"
	    "/* This file has been create automatically\n"
	    " *  Do not modifiy it.\n"
	    " */\n\n"
	    "/* Including the static part of literal table.\n"
	    " */\n\n");
  }

  /* For dynamic extensions, we want to emit C and not C++     */
  if(!gen_static_code)      /* For a pure interface, start     */
    Emit_C_Header(cfile);   /* "C" block.                      */


  fprintf(cfile,"#include \"%s\"\n\n",
          gen_static_code ? hfilename : "dyn_isa_lits.h");

  Emit_Header (hfile, FNAME_TARG_ISA_LITS, interface,extname);

  if(gen_static_code)
    fprintf(hfile,"#define %-20s %d\n","ISA_LC_UNDEFINED",lc_count++); 

  if(gen_static_code)
   {fprintf(cfile, 
        "static const ISA_LIT_CLASS_INFO ISA_LIT_CLASS_static_info[] = {\n");
   }
  else
   { fprintf(cfile, 
        "mUINT32 ISA_LC_CLASS_dyn_offset = ISA_LC_STATIC_MAX+1;\n");
   
     fprintf(cfile, 
        "static const ISA_LIT_CLASS_INFO ISA_LIT_CLASS_dynamic_info [] = {\n");
   }

  // UNDEFINED entry is reserved to static table
  if(gen_static_code) {
    fprintf(cfile, "  { { { 0x0000000000000000LL, 0x0000000000000000LL, 0, 0, 0 } }, 0, 0, 0, \"ISA_LC_UNDEFINED\" },\n");
    fprintf(cincfile,"  { { { 0x0000000000000000LL, 0x0000000000000000LL, 0, 0, 0 } }, 0, 0, 0, \"ISA_LC_UNDEFINED\" },\n");
  }

  // For dynamic code generation, we include in the
  // table the static part of the table.
  if(!gen_static_code) {
    fprintf(cfile,
	    "\n"
	    "#include \"%s\"\n\n",
	    cincfilename
            );
  }

  for (int i = 1; i <= 64; ++i) {
    unsigned_range[i].min = 0;
    unsigned_range[i].max = (i == 64) ? -1ULL : (1ULL << i) - 1;
    unsigned_range[i].scaling_value = 0; // [JV] default 
    unsigned_range[i].scaling_mask = 0; // [JV] default 
    unsigned_range[i].max_right_rotate = 0; // [JV] default 
    unsigned_range[i].right_rotate_mask = 0; // [JV] default 

    signed_range[i].min = -1LL << (i - 1);
    signed_range[i].max = (1LL << (i - 1)) - 1;
    signed_range[i].scaling_value = 0; // [JV] default 
    signed_range[i].scaling_mask = 0; // [JV] default 
    signed_range[i].max_right_rotate = 0; // [JV] default 
    signed_range[i].right_rotate_mask = 0; // [JV] default 
  }
}


/////////////////////////////////////
LIT_RANGE SignedBitRange(unsigned int bit_size)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if (bit_size == 0 || bit_size > 64) {
    fprintf(stderr, "### Error: invalid signed bit range: %d\n", bit_size);
    exit(EXIT_FAILURE);
  }
  return &signed_range[bit_size];
}


/////////////////////////////////////
LIT_RANGE UnsignedBitRange(unsigned int bit_size)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if (bit_size == 0 || bit_size > 64) {
    fprintf(stderr, "### Error: invalid unsigned bit range: %d\n", bit_size);
    exit(EXIT_FAILURE);
  }
  return &unsigned_range[bit_size];
}


/////////////////////////////////////
LIT_RANGE ISA_Create_Lit_Range(const char *name, long long min, long long max, long scaling, long right_rotate_width)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  LIT_RANGE range = new lit_range;
  range->name = name;
  range->min = min;
  range->max = max;
  range->scaling_value = scaling;
  if(scaling > 31) {
    fprintf(stderr, "### Error: invalid scaling %ld (greater than 31)\n", scaling);
    exit(EXIT_FAILURE);
  }
  range->scaling_mask = (1<<scaling)-1;

  if(right_rotate_width==0) { // no rotate
    range->max_right_rotate = 0;
    range->right_rotate_mask = 0;
  } 
  else if((right_rotate_width + scaling) > 6) {
    fprintf(stderr, "### Error: invalid right rotate width %ld (greater than 6 bits)\n", right_rotate_width);
    exit(EXIT_FAILURE);
  }
  /* Width on which the rotation can be applied
   * Example: if scaling is 1, and rotate value width is 4, the rotate width
   * or bit field on which rotation can be applied is 1 << 4 << 1 = 32 bits.
   * So the rotate mask is 0xffffffff.
   */
  else if((right_rotate_width + scaling) == 6) {
    range->right_rotate_mask = 0xffffffffffffffffLL;
  }
  else {
    range->right_rotate_mask = (1LL << ((1 << right_rotate_width) << scaling)) - 1;
  }
  /* Maximum number of rotation used to test given immediate value */
  range->max_right_rotate = (1 << right_rotate_width)-1;
  return range;
}


/////////////////////////////////////
void ISA_Create_Lit_Class(const char* name, LIT_CLASS_TYPE type, ...)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  va_list ap;
  LIT_RANGE range;
  bool is_signed = (type != UNSIGNED);
  bool is_negative = (type == NEGATIVE);
  long long min;
  long long max;

  switch(type) {
  case SIGNED:
    min = LONG_MAX;
    max = LONG_MIN;
 //   min = LONGLONG_MAX;
 //   max = LONGLONG_MIN;
    break;
  case UNSIGNED:
    min = ULONG_MAX;
 ///   min = ULONGLONG_MAX;
    max = 0;
    break;
  case NEGATIVE:
    min = 0;
    max = LONG_MIN;
 //   max = LONGLONG_MIN;
    break;
  }
    
  int num_ranges = 0;
  const char *string_template;

  // Find the smallest min and largest max for all ranges, and
  // count the number of ranges.
  va_start(ap,type);
  while ((range = va_arg(ap,LIT_RANGE)) != LIT_RANGE_END) {
    ++num_ranges;
    switch(type) {
    case SIGNED:
      if (range->min < min) min = range->min;
      if (range->max > max) max = range->max;
      break;
    case UNSIGNED:
      if ((unsigned long long)range->min < (unsigned long long)min) {
	min = range->min;
      }
      if ((unsigned long long)range->max > (unsigned long long)max) {
	max = range->max;
      }
      break;
    case NEGATIVE:
      long long sw = range->min;
      range->min = - range->max;
      range->max = - sw;

      if (range->min < max) {
	min = range->min;
      }
      if (range->max > max) {
	max = range->max;
      }
      break;
    }
  }
  va_end(ap);
  if (num_ranges > max_ranges) max_ranges = num_ranges;
  assert (num_ranges <= 1 && 
	  "LIT_CLASS with multiple ranges not yet supported"
	  "(this will break extension compatibility).");

  if(gen_static_code)
   fprintf(hfile,"#define LC_%-17s %d\n",name,lc_count);
  else {
   fprintf(hfile,"#define LC_dyn_%-17s (ISA_LC_STATIC_MAX+1+%d)\n",name,lc_count);
   fprintf(hfile,"#define LC_dyn_%s_gbu (ISA_LC_CLASS_dyn_offset+%d)\n",name,lc_count);
  }

  // Increment the number of range.
  ++lc_count;
  if(lc_count>lc_max_limit)
   { fprintf(stderr, 
             "### Error: number of literal classes is greater than allowed limit (%d)\n", 
             lc_max_limit);
      exit(EXIT_FAILURE);
   }

  // Initialize ISA_LIT_CLASS_info for this class. Note that .range[0]
  // holds the smallest min/largest max; .range[1] is the first sub-range.
  fprintf(cfile, "  { { { " PRINTF_LONGLONG_HEXA ", " PRINTF_LONGLONG_HEXA " }", min, max);
  if(gen_static_code)
    fprintf(cincfile, "  { { { " PRINTF_LONGLONG_HEXA ", " PRINTF_LONGLONG_HEXA " }", min, max);

  va_start(ap,type);
  while ((range = va_arg(ap,LIT_RANGE)) != LIT_RANGE_END) {
    fprintf(cfile, ",\n      { " PRINTF_LONGLONG_HEXA ", " PRINTF_LONGLONG_HEXA ", %ld, " PRINTF_LONGLONG_HEXA ", %ld, " PRINTF_LONGLONG_HEXA " }", 
	    range->min, range->max, range->scaling_value, range->scaling_mask,
	    range->max_right_rotate, range->right_rotate_mask);
    
    if(gen_static_code)
    { fprintf(cincfile, ",\n      { " PRINTF_LONGLONG_HEXA ", " PRINTF_LONGLONG_HEXA ", %ld, " PRINTF_LONGLONG_HEXA ", %ld, " PRINTF_LONGLONG_HEXA " }", 
	      range->min, range->max, range->scaling_value, range->scaling_mask,
	      range->max_right_rotate, range->right_rotate_mask);
    }
  }
  va_end(ap);

  string_template = gen_static_code ?
                    " }, %d, %d, %d, \"LC_%s\" },\n":
                    " }, %d, %d, %d, \"LC_dyn_%s\" },\n";

  fprintf(cfile,string_template,num_ranges,is_signed,is_negative,name);
  if(gen_static_code)
    fprintf(cincfile,string_template,num_ranges,is_signed,is_negative,name);

  return;
}


/////////////////////////////////////
void ISA_Lits_End(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if(gen_static_code) {
    fprintf(hfile,
	    "\n"
	    "BE_EXPORTED extern mUINT32 ISA_LC_MAX;\n\n"
	    "#define %-20s %d\n" 
	    "#define %-20s %d\n\n"
	    "typedef mUINT32 ISA_LIT_CLASS; /* used to be an enum */\n\n",
	    "ISA_LC_STATIC_MAX",lc_count-1,"ISA_LC_MAX_LIMIT",lc_max_limit);
  }


  fprintf(cfile, "};\n\n");
  if(gen_static_code)
    fprintf(cincfile,"\n\n");

  if(gen_static_code) {
    fprintf(cfile,
	    "BE_EXPORTED const ISA_LIT_CLASS_INFO * ISA_LIT_CLASS_info = ISA_LIT_CLASS_static_info;\n");
  }
  else {
    const char *fct_name1= "dyn_get_ISA_LIT_CLASS_info_tab";
    const char *fct_name2= "dyn_get_ISA_LIT_CLASS_info_tab_sz";
    const char *fct_name3= "dyn_get_ISA_LIT_CLASS_static_max";

    fprintf(sfile,
	    "BE_EXPORTED const ISA_LIT_CLASS_INFO * ISA_LIT_CLASS_info = NULL;\n");
    
     fprintf(cfile,
             "\n\n"
             "const ISA_LIT_CLASS_INFO* %s ( void )\n"
             "{ return ISA_LIT_CLASS_dynamic_info;\n"
             "};\n\n",
             fct_name1);

     fprintf(cfile,
             "const mUINT32 %s ( void )\n"
             "{ return (const mUINT32) (ISA_LC_STATIC_MAX + 1/*UNDEFINED*/ + %d);\n"
             "}\n\n",
             fct_name2,lc_count);
     
     fprintf(cfile,
             "const mUINT32 %s ( void )\n"
             "{ return (const mUINT32) (ISA_LC_STATIC_MAX);\n"
             "}\n\n",
             fct_name3);
     
     fprintf(cfile,
             "void dyn_set_ISA_LIT_CLASS_offset (mUINT32 offs)\n"
             "{  ISA_LC_CLASS_dyn_offset = offs;\n"
             "}\n\n");
	     
     fprintf(hfile,
             "\n\n"
             "extern mUINT32 ISA_LC_CLASS_dyn_offset;\n"
             "extern void dyn_set_ISA_LIT_CLASS_offset ( mUINT32 );\n"
             "extern const ISA_LIT_CLASS_INFO* %s ( void );\n"
             "extern const mUINT32 %s ( void );\n"
             "extern const mUINT32 %s ( void );\n",
             fct_name1,fct_name2,fct_name3);
   }

  if(gen_static_code) {
    fprintf(cfile,"\n"
	    "BE_EXPORTED mUINT32 ISA_LC_MAX = ISA_LC_STATIC_MAX;\n");
  }
  else {
    fprintf(sfile,"\n"
	    "BE_EXPORTED mUINT32 ISA_LC_MAX = 0;\n");
  }

  if(gen_static_code) 
   { fprintf(hfile,"#define MAX_RANGE_STATIC %d\n",
                   max_ranges + 1);
     fprintf(hfile, 
             "\ntypedef struct {\n"
             "  struct {\n"
	     "    INT64 min;\n"
	     "    INT64 max;\n"
	     "    INT32 scaling_value;\n"
	     "    INT64 scaling_mask;\n"
	     "    INT32 max_right_rotate;\n"
	     "    INT64 right_rotate_mask;\n"
	     "  } range[MAX_RANGE_STATIC];\n"
             "  mUINT8 num_ranges;\n"
             "  mBOOL is_signed;\n"
             "  mBOOL is_negative;\n"
             "  const char *name;\n"
             "} ISA_LIT_CLASS_INFO;\n");

     fprintf(hfile,
             "\n"
             "BE_EXPORTED extern const ISA_LIT_CLASS_INFO * ISA_LIT_CLASS_info;\n");
     fprintf(efile, "ISA_LIT_CLASS_info\n");
    }

  if(gen_static_code) {
  fprintf(hfile, "\ninline const char * ISA_LC_Name (ISA_LIT_CLASS lc)\n"
		 "{\n"
		 "  return ISA_LIT_CLASS_info[lc].name;\n"
		 "}\n");

  fprintf(hfile, "\ninline INT64 ISA_LC_Min (ISA_LIT_CLASS lc)\n"
		 "{\n"
		 "  return ISA_LIT_CLASS_info[lc].range[0].min;\n"
		 "}\n");

  fprintf(hfile, "\ninline INT64 ISA_LC_Max (ISA_LIT_CLASS lc)\n"
		 "{\n"
		 "  return ISA_LIT_CLASS_info[lc].range[0].max;\n"
		 "}\n");

  fprintf(hfile, "\ninline INT64 ISA_LC_Scaling_Value (ISA_LIT_CLASS lc, mUINT8 range)\n"
		 "{\n"
		 "  return ISA_LIT_CLASS_info[lc].range[range].scaling_value;\n"
		 "}\n");

  fprintf(hfile, "\ninline INT64 ISA_LC_Scaling_Mask (ISA_LIT_CLASS lc, mUINT8 range)\n"
		 "{\n"
		 "  return ISA_LIT_CLASS_info[lc].range[range].scaling_mask;\n"
		 "}\n");

  fprintf(hfile, "\ninline mUINT8 ISA_LC_Ranges (ISA_LIT_CLASS lc)\n"
		 "{\n"
		 "  return ISA_LIT_CLASS_info[lc].num_ranges;\n"
		 "}\n");

  fprintf(hfile, "\ninline BOOL ISA_LC_Is_Signed (ISA_LIT_CLASS lc)\n"
		 "{\n"
		 "  return ISA_LIT_CLASS_info[lc].is_signed;\n"
		 "}\n");

  fprintf(hfile, "\ninline BOOL ISA_LC_Is_Negative (ISA_LIT_CLASS lc)\n"
		 "{\n"
		 "  return ISA_LIT_CLASS_info[lc].is_negative;\n"
		 "}\n");

  fprintf(hfile, "\ninline BOOL ISA_LC_Value_In_Class (INT64 val, ISA_LIT_CLASS lc)\n"
		 "{\n"
                 "  const ISA_LIT_CLASS_INFO *plc = ISA_LIT_CLASS_info + lc;\n"
		 "  INT i;\n"
		 "  for (i = 1; i <= plc->num_ranges; ++i) {\n"
		 "    INT64 min = plc->range[i].min;\n"
		 "    INT64 max = plc->range[i].max;\n"
	         "    if(plc->range[i].max_right_rotate) {\n"
	         "      INT64 rotate_mask = (1LL << (1 << plc->range[i].scaling_value))-1;\n"
                 "      INT64 rotate_amount = 1 << plc->range[i].scaling_value;\n"
	         "      INT32 rotate_nb = 0;\n"
	         "      val &= plc->range[i].right_rotate_mask;\n"
	         "      /* Remove trailing zeros as possible */\n"
	         "      while(rotate_nb <= plc->range[i].max_right_rotate && (val & rotate_mask) == 0) {\n"
                 "        val >>= rotate_amount;\n"
                 "        rotate_nb++;\n"
                 "      }\n"
                 "    }\n"
	         "    else {\n"
	         "      if ( (val & plc->range[i].scaling_mask) != 0 ) { return FALSE; }\n"
                 "    }\n"
		 "    if ( plc->is_signed ) {\n"
	         "      if(!plc->range[i].max_right_rotate) {\n"
	         "        val = val >> plc->range[i].scaling_value;\n"
                 "      }\n"
		 "      if (val >= min && val <= max) return TRUE;\n"
		 "    } else {\n"
                 "      UINT64 valu = (UINT64)val;\n"
	         "      if(!plc->range[i].max_right_rotate) {\n"
                 "        valu >>= ((UINT64)(plc->range[i].scaling_value));\n"
                 "      }\n"
		 "      if (valu >= (UINT64)min && valu <= (UINT64)max) return TRUE;\n"
		 "    }\n"
		 "  }\n"
		 "  return FALSE;\n"
		 "}\n");

  fprintf(hfile, "inline INT64 ISA_LC_Abstract_Value (INT64 val, ISA_LIT_CLASS lc)\n"
                 "{\n"
                 "  const ISA_LIT_CLASS_INFO *plc = ISA_LIT_CLASS_info + lc;\n"
		 "  INT i;\n"
                 "\n"
		 "  for (i = 1; i <= plc->num_ranges; ++i) {\n"
		 "    INT64 min = plc->range[i].min;\n"
		 "    INT64 max = plc->range[i].max;\n"
                 "\n"
                 "    /* Managing scaling factor */\n"
	         "    if(plc->range[i].scaling_value == 0)\n"
                 "        continue;\n"
                 "\n"
                 "    /* Left shift is not sensitive to sign extension\n"
                 "     * Thus, we can factorize both the signed and the unsigned cases.\n"
                 "     */\n"
                 "    if( (plc->is_signed && val>=min && val<= max) ||\n"
                 "       ((UINT64)val>=(UINT64)min && (UINT64)val<=(UINT64)max)) {\n"
                 "            return val <<=plc->range[i].scaling_value;\n"
                 "        }\n"
                 "\n"
                 "  }    /* End for */\n"
                 "\n"
                 "    return val;            /* Default value */\n"
                 "}\n\n");

  }                            // if (gen_static_code)


  fprintf(hfile, "#ifdef DYNAMIC_CODE_GEN\n");
  fprintf(hfile, "extern void ISA_LITS_Initialize_Stub(void);\n");
  fprintf(hfile, "#endif\n");

  if(!gen_static_code) {
    fprintf(sfile,
	    "/*\n"
	    " * Exported routine.\n"
	    " */\n"
	    "void \n"
	    "ISA_LITS_Initialize_Stub( void )\n"
	    "{\n"
	    "  ISA_LIT_CLASS_info = (ISA_LIT_CLASS_INFO*)dyn_get_ISA_LIT_CLASS_info_tab();\n"
	    "  ISA_LC_MAX = dyn_get_ISA_LIT_CLASS_info_tab_sz();\n"
	    "  return;\n"
	    "}\n");
  }

  Emit_Footer (hfile);
  if(!gen_static_code)
    Emit_C_Footer(cfile);          // Ending "C" block.

  // Closing file handlers
  Gen_Close_File_Handle(cfile,cfilename);
  Gen_Close_File_Handle(hfile,hfilename);
  if(efile)    Gen_Close_File_Handle(efile,efilename);
  if(sfile)    Gen_Close_File_Handle(sfile,sfilename);
  if(cincfile) Gen_Close_File_Handle(cincfile,cincfilename);

  // Memory deallocation
  if(cfilename)    Gen_Free_Filename(cfilename);
  if(hfilename)    Gen_Free_Filename(hfilename);
  if(efilename)    Gen_Free_Filename(efilename);
  if(sfilename)    Gen_Free_Filename(sfilename);
  if(cincfilename) Gen_Free_Filename(cincfilename);

  return;
}
#else
static FILE *hfile, *cfile, *efile;
static struct lit_range signed_range[65];
static struct lit_range unsigned_range[65];
static int max_ranges = 0;

/////////////////////////////////////
void ISA_Lits_Begin (void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
#define FNAME "targ_isa_lits"
  char buf[1000];
  sprintf (buf, "%s.h", FNAME);
  hfile = fopen(buf, "w");
  sprintf (buf, "%s.c", FNAME);
  cfile = fopen(buf, "w");
  sprintf (buf, "%s.Exported", FNAME);
  efile = fopen(buf, "w");

  fprintf(cfile,"#include \"%s.h\"\n\n", FNAME);

  sprintf (buf, "%s", FNAME);
  Emit_Header (hfile, buf, interface);

  fprintf(hfile, "\ntypedef enum {\n");
  // start with undefined value
  fprintf(hfile, "\tLC_UNDEFINED,\n");

  fprintf(cfile, "const ISA_LIT_CLASS_INFO ISA_LIT_CLASS_info[] = {\n");
  fprintf(cfile, "  { { { 0x0000000000000000LL, 0x0000000000000000LL } }, 0, 0, \"LC_UNDEFINED\" },\n");

  for (int i = 1; i <= 64; ++i) {
    unsigned_range[i].min = 0;
    unsigned_range[i].max = (i == 64) ? -1ULL : (1ULL << i) - 1;

    signed_range[i].min = -1LL << (i - 1);
    signed_range[i].max = (1LL << (i - 1)) - 1;
  }
}


/////////////////////////////////////
LIT_RANGE SignedBitRange(unsigned int bit_size)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if (bit_size == 0 || bit_size > 64) {
    fprintf(stderr, "### Error: invalid signed bit range: %d\n", bit_size);
    exit(EXIT_FAILURE);
  }
  return &signed_range[bit_size];
}


/////////////////////////////////////
LIT_RANGE UnsignedBitRange(unsigned int bit_size)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  if (bit_size == 0 || bit_size > 64) {
    fprintf(stderr, "### Error: invalid unsigned bit range: %d\n", bit_size);
    exit(EXIT_FAILURE);
  }
  return &unsigned_range[bit_size];
}

#ifdef TARG_ST
/////////////////////////////////////
LIT_RANGE ISA_Create_Lit_Range(const char *name, long long min, long long max, long scaling, long right_rotate_width)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  LIT_RANGE range = new lit_range;
  range->name = name;
  range->min = min;
  range->max = max;
  range->scaling_value = scaling;
  if(scaling > 31) {
    fprintf(stderr, "### Error: invalid scaling %ld (greater than 31)\n", scaling);
    exit(EXIT_FAILURE);
  }
  range->scaling_mask = (1<<scaling)-1;

  if(right_rotate_width==0) { // no rotate
    range->max_right_rotate = 0;
    range->right_rotate_mask = 0;
  } 
  else if((right_rotate_width + scaling) > 6) {
    fprintf(stderr, "### Error: invalid right rotate width %ld (greater than 6 bits)\n", right_rotate_width);
    exit(EXIT_FAILURE);
  }
  /* Width on which the rotation can be applied
   * Example: if scaling is 1, and rotate value width is 4, the rotate width
   * or bit field on which rotation can be applied is 1 << 4 << 1 = 32 bits.
   * So the rotate mask is 0xffffffff.
   */
  else if((right_rotate_width + scaling) == 6) {
    range->right_rotate_mask = 0xffffffffffffffffLL;
  }
  else {
    range->right_rotate_mask = (1LL << ((1 << right_rotate_width) << scaling)) - 1;
  }
  /* Maximum number of rotation used to test given immediate value */
  range->max_right_rotate = (1 << right_rotate_width)-1;
  return range;
}
#else

/////////////////////////////////////
LIT_RANGE ISA_Create_Lit_Range(const char *name, long long min, long long max)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  LIT_RANGE range = new lit_range;
  range->name = name;
  range->min = min;
  range->max = max;
  return range;
}
#endif

#ifndef LONG_LONG_MAX
#define LONG_LONG_MAX	__LONG_LONG_MAX__
#endif
#ifndef ULONG_LONG_MAX
#define ULONG_LONG_MAX	(__LONG_LONG_MAX__ * 2ULL + 1)
#endif
#ifndef LONG_LONG_MIN
#define LONG_LONG_MIN	(- __LONG_LONG_MAX__ - 1)
#endif

/////////////////////////////////////
void ISA_Create_Lit_Class(const char* name, LIT_CLASS_TYPE type, ...)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  va_list ap;
  LIT_RANGE range;
  bool is_signed = type == SIGNED;
  long long min = is_signed ? LONG_LONG_MAX : ULONG_LONG_MAX;
  long long max = is_signed ? LONG_LONG_MIN : 0;
  int num_ranges = 0;

  // Find the smallest min and largest max for all ranges, and
  // count the number of ranges.
  va_start(ap,type);
  while ((range = va_arg(ap,LIT_RANGE)) != LIT_RANGE_END) {
    ++num_ranges;
    if (is_signed) {
      if (range->min < min) min = range->min;
      if (range->max > max) max = range->max;
    } else {
      if ((unsigned long long)range->min < (unsigned long long)min) {
	min = range->min;
      }
      if ((unsigned long long)range->max > (unsigned long long)max) {
	max = range->max;
      }
    }
  }
  va_end(ap);
  if (num_ranges > max_ranges) max_ranges = num_ranges;

  // Initialize ISA_LIT_CLASS_info for this class. Note that .range[0]
  // holds the smallest min/largest max; .range[1] is the first sub-range.
  fprintf(hfile, "\tLC_%s,\n", name);
  fprintf(cfile, "  { { { 0x%016llxLL, 0x%016llxLL }", min, max);
  va_start(ap,type);
  while ((range = va_arg(ap,LIT_RANGE)) != LIT_RANGE_END) {
    fprintf(cfile, ",\n      { 0x%016llxLL, 0x%016llxLL }", 
		   range->min, range->max);
  }
  va_end(ap);
  fprintf(cfile, " }, %d, %d, \"LC_%s\" },\n",
		 num_ranges, is_signed, name);
}


/////////////////////////////////////
void ISA_Lits_End(void)
/////////////////////////////////////
//  See interface description.
/////////////////////////////////////
{
  fprintf(hfile, "\tLC_MAX\n");
  fprintf(hfile, "} ISA_LIT_CLASS;\n");

  fprintf(cfile, "};\n");

  fprintf(hfile, "\ntypedef struct {\n"
		"  struct { INT64 min; INT64 max; } range[%d];\n"
		"  mUINT8 num_ranges;\n"
		"  mBOOL is_signed;\n"
		"  const char *name;\n"
		"} ISA_LIT_CLASS_INFO;\n",
		max_ranges + 1);
  fprintf(efile, "ISA_LIT_CLASS_info\n");

  fprintf(hfile, "\ninline const char * ISA_LC_Name (ISA_LIT_CLASS lc)\n"
		 "{\n"
		 "  extern const ISA_LIT_CLASS_INFO ISA_LIT_CLASS_info[];\n"
		 "  return ISA_LIT_CLASS_info[lc].name;\n"
		 "}\n");

  fprintf(hfile, "\ninline INT64 ISA_LC_Min (ISA_LIT_CLASS lc)\n"
		 "{\n"
		 "  extern const ISA_LIT_CLASS_INFO ISA_LIT_CLASS_info[];\n"
		 "  return ISA_LIT_CLASS_info[lc].range[0].min;\n"
		 "}\n");

  fprintf(hfile, "\ninline INT64 ISA_LC_Max (ISA_LIT_CLASS lc)\n"
		 "{\n"
		 "  extern const ISA_LIT_CLASS_INFO ISA_LIT_CLASS_info[];\n"
		 "  return ISA_LIT_CLASS_info[lc].range[0].max;\n"
		 "}\n");

  fprintf(hfile, "\ninline BOOL ISA_LC_Is_Signed (ISA_LIT_CLASS lc)\n"
		 "{\n"
		 "  extern const ISA_LIT_CLASS_INFO ISA_LIT_CLASS_info[];\n"
		 "  return ISA_LIT_CLASS_info[lc].is_signed;\n"
		 "}\n");

  fprintf(hfile, "\ninline BOOL ISA_LC_Value_In_Class (INT64 val, ISA_LIT_CLASS lc)\n"
		 "{\n"
		 "  extern const ISA_LIT_CLASS_INFO ISA_LIT_CLASS_info[];\n"
		 "  const ISA_LIT_CLASS_INFO *plc = ISA_LIT_CLASS_info + lc;\n"
		 "  INT i;\n"
		 "  for (i = 1; i <= plc->num_ranges; ++i) {\n"
		 "    INT64 min = plc->range[i].min;\n"
		 "    INT64 max = plc->range[i].max;\n"
		 "    if ( plc->is_signed ) {\n"
		 "      if (val >= min && val <= max) return TRUE;\n"
		 "    } else {\n"
		 "      if ((UINT64)val >= (UINT64)min && (UINT64)val <= (UINT64)max) return TRUE;\n"
		 "    }\n"
		 "  }\n"
		 "  return FALSE;\n"
		 "}\n");

  Emit_Footer (hfile);
}
#endif
