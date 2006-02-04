#include "W_alloca.h"
#define GCOV_LINKAGE
#include <time.h> // for time()
#include <W_alloca.h> // for alloca()
#include "gcov_coverage.h"
#include "gcov-io.h"
#include "gcov_hwint.h"
#include "gcov-io.c"
#include "srcpos.h"
#include "cgdwarf.h"
#include "bb.h"
#include "config_asm.h"
#include "exp_targ.h"
#include "data_layout.h"
#include "cgtarget.h"
#include "const.h" // for New_Const_Sym
#include <unistd.h> // for get_current_dir_name
#include "glob.h" // for Src_File_Name

struct function_list
{
  struct function_list *next;	 /* next function */
  unsigned ident;		 /* function ident */
  unsigned checksum;	         /* function checksum */
  unsigned n_ctrs[GCOV_COUNTERS];/* number of counters.  */
};


static struct function_list *functions_head = 0;
static struct function_list **functions_tail = &functions_head;
static unsigned no_coverage = 0;
/* Unique identifier for the current function */
static unsigned current_function_funcdef_no = 1;

/* Cumulative counter information for whole program.  */
static unsigned prg_ctr_mask; /* Mask of counter types generated.  */
static unsigned prg_n_ctrs[GCOV_COUNTERS]; /* Total counters allocated.  */

/* Counter information for current function.  */
static unsigned fn_ctr_mask; /* Mask of counters used.  */
static unsigned fn_n_ctrs[GCOV_COUNTERS]; /* Counters allocated.  */
static unsigned fn_b_ctrs[GCOV_COUNTERS]; /* Allocation base.  */

/* Name of the output file for coverage output file.  */
static char *bbg_file_name;
static unsigned bbg_file_opened;
static int bbg_function_announced;

/* Name of the count data file.  */
static char *da_file_name;

/* A random sequence of characters, unless overridden by user.  */
static const char *flag_random_seed;

/* A local time stamp derived from the time of compilation. It will be
   zero if the system cannot provide a time.  It will be -1u, if the
   user has specified a particular random seed.  */
static unsigned local_tick;

static SRCPOS last_srcpos;

/* Forward declarations.  */
static unsigned compute_checksum (void);
static unsigned coverage_checksum_string (unsigned, const char *);
static void create_coverage (FILE *file, TY_IDX counter_type);

/* The names of the counter tables.  */
static ST* ctr_labels[GCOV_COUNTERS];

/* The names of merge functions for counters.  */
static const char *const ctr_merge_functions[GCOV_COUNTERS] = GCOV_MERGE_FUNCTIONS;
static ST * ctr_merge_functions_st[GCOV_COUNTERS];
static const char *const ctr_names[GCOV_COUNTERS] = GCOV_COUNTER_NAMES;


// static ST     *gcov_init_st; //__gcov_init symbol
static ST     *gcov_init_thunk_st; //thunck symbol where __gcov_init is called
static char *gcov_init_thunk_name;//name of thunck symbol where __gcov_init is called

// memory for this module
static MEM_POOL MEM_coverage_pool;

#define FILE_FUNCTION_FORMAT "_GLOBAL__%s_%s"

/* 
   Since the basic blocks are expected to be numbered sequentially we
need a matching between BB_id and a gcov number
 */
static unsigned int *gcov_number;
#define BB_TO_GCOV_INDEX(bb)  (gcov_number[BB_id(bb)])
static unsigned LAST_BB_TO_GCOV_INDEX;
static unsigned FIRST_BB_TO_GCOV_INDEX;
/*
  Pass to compute a mapping between BB_ID of BBs and an internal gcov
  numbering
*/
static void  
gcov_prepass()
{
  BB *bb;
  unsigned int i = 1;
  int size = 0;
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    size = (size < BB_id(bb)) ? BB_id(bb) : size;
  }
  /* Create the successor and predecessor chain vectors.
   */
  gcov_number = TYPE_MEM_POOL_ALLOC_N(unsigned int,
				     &MEM_coverage_pool,
				      size);
  /* For each BB in the CFG, create a gcov numbering
   */
  for (bb = REGION_First_BB; bb != NULL; bb = BB_next(bb)) {
    gcov_number[BB_id(bb)] = i++;
  }
  LAST_BB_TO_GCOV_INDEX = i;
  FIRST_BB_TO_GCOV_INDEX = 0;
}


/* Set up a default flag_random_seed and local_tick, unless the user
   already specified one.  */
static void
randomize (void)
{
  if (!flag_random_seed)
    {
      unsigned HOST_WIDE_INT value;
      static char random_seed[HOST_BITS_PER_WIDE_INT / 4 + 3];

      /* Get some more or less random data.  */
#ifdef HAVE_GETTIMEOFDAY
      {
 	struct timeval tv;

 	gettimeofday (&tv, NULL);
	local_tick = tv.tv_sec * 1000 + tv.tv_usec / 1000;
      }
#else
      {
	time_t now = time (NULL);

	if (now != (time_t)-1)
	  local_tick = (unsigned) now;
      }
#endif
      value = local_tick ^ getpid ();

      sprintf (random_seed, HOST_WIDE_INT_PRINT_HEX, value);
      flag_random_seed = random_seed;
    }
  else if (!local_tick)
    local_tick = 0xFFFFFFFF;
}

/* Generate a crc32 of a string.  */

static unsigned
crc32_string (unsigned chksum, const char *string)
{
  do
    {
      unsigned value = *string << 24;
      unsigned ix;
      
      for (ix = 8; ix--; value <<= 1)
  	{
  	  unsigned feedback;
  	  
  	  feedback = (value ^ chksum) & 0x80000000 ? 0x04c11db7 : 0;
 	  chksum <<= 1;
 	  chksum ^= feedback;
  	}
    }
  while (*string++);
  return chksum;
}

/* Generate a checksum for a string.  CHKSUM is the current
   checksum.  */

static unsigned
coverage_checksum_string (unsigned chksum, const char *string)
{
  int i;
  char *dup = NULL;

  /* Look for everything that looks if it were produced by
     get_file_function_name_long and zero out the second part
     that may result from flag_random_seed.  This is not critical
     as the checksums are used only for sanity checking.  */
  for (i = 0; string[i]; i++)
   {
      if (!strncmp (string + i, "_GLOBAL__", 9))
	for (i = i + 9; string[i]; i++)
	  if (string[i]=='_')
	    {
	      int y;
	      unsigned seed;

	      for (y = 1; y < 9; y++)
		if (!(string[i + y] >= '0' && string[i + y] <= '9')
		    && !(string[i + y] >= 'A' && string[i + y] <= 'F'))
		  break;
	      if (y != 9 || string[i + 9] != '_')
		continue;
	      for (y = 10; y < 18; y++)
		if (!(string[i + y] >= '0' && string[i + y] <= '9')
		    && !(string[i + y] >= 'A' && string[i + y] <= 'F'))
		  break;
	      if (y != 18)
		continue;
	      if (!sscanf (string + i + 10, "%X", &seed))
		FmtAssert(FALSE, ("coverage_checksum_string: don't know what to do!"));
	      if (seed != crc32_string (0, flag_random_seed))
		continue;
	      string = dup = strdup (string);
	      for (y = 10; y < 18; y++)
		dup[i + y] = '0';
	      break;
	    }
      break;
    }

  chksum = crc32_string (chksum, string);
  if (dup)
    free (dup);

  return chksum;
}


// Create a type for gcov_ctr_info type
// /* Information about counters.  */
// struct gcov_ctr_info
// {
//   gcov_unsigned_t num;		/* number of counters.  */
//   gcov_type *values;		/* their values.  */
//   gcov_merge_fn merge;  	/* The function used to merge them.  */
// };
static TY_IDX
build_ctr_info_type(TY_IDX counter_type) {
  FLD_HANDLE first_field, field;
  TY_IDX gcov_ctr_info_idx;
  TY_IDX ptr_idx1, ptr_idx2;

  TY& gcov_ctr_info = New_TY(gcov_ctr_info_idx);

  first_field = New_FLD ();
  FLD_Init (first_field, Save_Str("num"), 
	    Be_Type_Tbl(MTYPE_U4), 0);

  ptr_idx1 = Make_Pointer_Type(counter_type);
  ptr_idx2 = Make_Pointer_Type(MTYPE_To_TY(MTYPE_V));

  field = New_FLD ();
  FLD_Init (field, Save_Str("values"), 
	   ptr_idx1, 4);

  field = New_FLD ();
  FLD_Init (field, Save_Str("merge"), 
	    ptr_idx2, 8);

  Set_FLD_last_field(field);
  TY_Init (gcov_ctr_info, TY_size(Be_Type_Tbl(MTYPE_U4)) + TY_size(ptr_idx1) + 
	   TY_size(ptr_idx2),
	   KIND_STRUCT, MTYPE_M, Save_Str ("gcov_ctr_info"));
  Set_TY_align(gcov_ctr_info_idx, 4);
  Set_TY_fld(gcov_ctr_info, first_field);

  return gcov_ctr_info_idx;
}

// Create a type for gcov_fn_info type
// struct gcov_fn_info
// {
//   gcov_unsigned_t ident;	/* unique ident of function */
//   gcov_unsigned_t checksum;	/* function checksum */
//   unsigned n_ctrs[0];		/* instrumented counters */
// };
static TY_IDX
build_fn_info_type() {
  unsigned ix;
  TY_IDX gcov_fn_info_idx;
  FLD_HANDLE first_field, field;
  TY& gcov_fn_info = New_TY(gcov_fn_info_idx);

  first_field = New_FLD ();
  FLD_Init (first_field, Save_Str("ident"), 
	    Be_Type_Tbl(MTYPE_U4), 0);

  field = New_FLD ();
  FLD_Init (field, Save_Str("checksum"), 
	    Be_Type_Tbl(MTYPE_U4), 4);

  for (ix = 0; ix != GCOV_COUNTERS; ix++)
    if (prg_ctr_mask & (1 << ix))
      {
	field = New_FLD ();
	FLD_Init (field, Save_Str("n_ctrs"), 
		  Be_Type_Tbl(MTYPE_U4), 8);
      }
  Set_FLD_last_field(field);
  TY_Init (gcov_fn_info, 3*TY_size(Be_Type_Tbl(MTYPE_U4)),
	   KIND_STRUCT, MTYPE_M, Save_Str ("gcov_fn_info"));
  Set_TY_align(gcov_fn_info_idx, 4);
  Set_TY_fld(gcov_fn_info, first_field);

  return gcov_fn_info_idx;
}

// Create a type for gcov_info type
// /* Information about a single object file.  */
// struct gcov_info
// {
//   gcov_unsigned_t version;	/* expected version number */
//   struct gcov_info *next;	/* link to next, used by libgcov */
//   gcov_unsigned_t stamp;	/* uniquifying time stamp */
//   const char *filename;		/* output file name */
//   unsigned n_functions;		/* number of functions */
//   const struct gcov_fn_info *functions; /* table of functions */
//   unsigned ctr_mask;		/* mask of counters instrumented.  */
//   struct gcov_ctr_info counts[0]; /* count data. The number of bits
// 				     set in the ctr_mask field
// 				     determines how big this array
// 				     is.  */
// };

static TY_IDX
build_gcov_info_type(TY_IDX gcov_fn_info_idx, TY_IDX gcov_ctr_info_idx) {
  FLD_HANDLE first_field, field;
  TY_IDX ptr_idx1, ptr_idx2;
  TY_IDX ptr_idx3;

  first_field = New_FLD ();
  FLD_Init (first_field, Save_Str ("version"), 
	    Be_Type_Tbl(MTYPE_U4), 0);
  
  ptr_idx1 = Make_Pointer_Type(MTYPE_To_TY(MTYPE_U4));
  ptr_idx2 = Make_Pointer_Type(MTYPE_To_TY(MTYPE_I1));
  ptr_idx3 = Make_Pointer_Type(gcov_fn_info_idx);
  
  field = New_FLD ();
  FLD_Init (field, Save_Str("next"), 
	    ptr_idx1, 4);
  
  field = New_FLD ();
  FLD_Init (field, Save_Str("stamp"), 
	    Be_Type_Tbl(MTYPE_U4), 8);
  
  field = New_FLD ();
  FLD_Init (field, Save_Str("filename"), 
	    ptr_idx2, 12);

  field = New_FLD ();
  FLD_Init (field, Save_Str("n_functions"), 
	    Be_Type_Tbl(MTYPE_U4), 12);
  
  field = New_FLD ();
  FLD_Init (field, Save_Str("functions"), 
	    ptr_idx3, 16);
  
  field = New_FLD ();
  FLD_Init (field, Save_Str("ctr_mask"), 
	    Be_Type_Tbl(MTYPE_U4), 20);
  
  field = New_FLD ();
  FLD_Init (field, Save_Str("counts"), 
	    gcov_ctr_info_idx, 24);
  
  Set_FLD_last_field(field);
  
  // declare a struct with the above field list 
  TY_IDX gcov_info_idx;
  TY& gcov_info = New_TY(gcov_info_idx);
  TY_Init (gcov_info, 4*TY_size(Be_Type_Tbl(MTYPE_U4)) + TY_size(ptr_idx1) 
	   + TY_size(ptr_idx2) + TY_size(ptr_idx3)+ TY_size(gcov_ctr_info_idx),
	   KIND_STRUCT, MTYPE_M, Save_Str ("gcov_info"));
  Set_TY_align(gcov_info_idx, 4);
  Set_TY_fld(gcov_info, first_field);
  return gcov_info_idx;
}

/* Creates a CONSTRUCTOR for a gcov_fn_info. FUNCTION is
   the function being processed and TYPE is the gcov_fn_info
   RECORD_TYPE.  */
static ST *
build_fn_info_value (TY_IDX fn_info_type_idx, unsigned *nfns)
{
  INITV_IDX inv;
  INITV_IDX prev_inv;
  const struct function_list *function;
  unsigned n_fns;
  ST *first_st;
  for (function = functions_head, n_fns = 0; function; function = function->next, 
	 n_fns++) {
    // create new constant
    ST *st = New_ST (GLOBAL_SYMTAB);
    // Create a new symbol 
    char *buf;
    /* Name this symbol: */
    buf = (char *)alloca(32);
    sprintf(buf, "%s%d", ".LPFI", n_fns);
    ST_Init (st, Save_Str(buf), CLASS_VAR
	     , SCLASS_FSTATIC, EXPORT_LOCAL, fn_info_type_idx);
    Set_ST_is_initialized (st);
    Set_ST_is_const_var (st);
    INITO_IDX inito = New_INITO(st);
    unsigned ix;
    /* ident */
    inv = New_INITV ();
    INITV_Init_Integer (inv, MTYPE_U4, function->ident);
    prev_inv = Append_INITV (inv, inito, INITV_IDX_ZERO);

    /* checksum */
    inv = New_INITV ();
    INITV_Init_Integer (inv, MTYPE_U4, function->checksum);
    prev_inv = Append_INITV (inv, inito, prev_inv);

    /* counters */
    for (ix = 0; ix != GCOV_COUNTERS; ix++)
      if (prg_ctr_mask & (1 << ix))
	{
  	  inv = New_INITV ();
	  INITV_Init_Integer (inv, MTYPE_U4, function->n_ctrs[ix]);
	  prev_inv = Append_INITV (inv, inito, prev_inv);
	}

    Allocate_Object(st);
    // Keep a trace on the start of this rodata area
    if (n_fns == 0)
      first_st = st;
  }
  *nfns = n_fns;
  return first_st;
}

/* Creates a CONSTRUCTOR for a gcov_ctr_info. COUNTER is
   the counter being processed and TYPE is the gcov_ctr_info
   TYPE.  */

static INITV_IDX
build_ctr_info_value (INITV_IDX inv, INITO_IDX inito, 
		       INITV_IDX prev_inv, unsigned counter)
{
  //   gcov_unsigned_t num;		/* number of counters.  */
  inv = New_INITV ();
  INITV_Init_Integer (inv, MTYPE_U4, prg_n_ctrs[counter]);
  prev_inv = Append_INITV (inv, inito, prev_inv);

  //   gcov_type *values;		/* their values.  */
  if (prg_n_ctrs[counter])
    {
      inv = New_INITV ();
      INITV_Init_Symoff (inv, ctr_labels[counter], 0, 1);
      prev_inv = Append_INITV (inv, inito, prev_inv);
    } else {
      // Null pointer
      inv = New_INITV ();
      INITV_Init_Integer (inv, MTYPE_U4, 0);
      prev_inv = Append_INITV (inv, inito, prev_inv);
    }

  //   gcov_merge_fn merge;  	/* The function used to merge them.  */
  // Create a Label for the merge function
  inv = New_INITV ();
  INITV_Init_Symoff (inv, ctr_merge_functions_st[counter], 0);
  prev_inv = Append_INITV (inv, inito, prev_inv);
  return prev_inv;
}

// Build the thunck function to invoke __gcov_init.  For the moment
// the chunk is hard coded. It would be smarter to create a New PU...
static void
generate_thunk_for_gcov_init(FILE * file){

  fprintf(file,"\t%s\t__gcov_init\n\n", AS_GLOBAL);

  fprintf(file,"\t%s .text\n\n", AS_SECTION);
  fprintf(file,"\t%s Program Unit: %s\n", ASM_CMNT_LINE, gcov_init_thunk_name);
  fprintf(file,"\t%s 8\n", AS_ALIGN);
  fprintf(file,"\t%s\t%s\n", AS_ENT, gcov_init_thunk_name);
  fprintf(file,"%s:\n", gcov_init_thunk_name); 

  fprintf(file,"\tmov $r16 = %s\n",".LPBX0");        
  fprintf(file,"\t%s\t__gcov_init, %s\n", AS_TYPE, AS_TYPE_FUNC);
  fprintf(file,"\tgoto __gcov_init                	## __gcov_init\n");
  fprintf(file,"\t%s\n",AS_STOP_BIT);
  fprintf(file,"\t%s\t%s\n", AS_END, gcov_init_thunk_name);
}

// Creates a symbol of type gcov_info_idx. Insert a reference to fn_st
//   for the functions field
static ST*
build_gcov_info_value (TY_IDX gcov_info_idx, ST *fn_st, unsigned n_fns)
{
  unsigned ix;
  char *buf;
  /* Name this symbol: */
  buf = (char *)alloca(32);
  sprintf(buf, "%s%d", ".LPBX", 0);

  ST *newst = New_ST(GLOBAL_SYMTAB);
  ST_Init(newst, 
	  Save_Str(buf),
	  CLASS_VAR, SCLASS_FSTATIC, EXPORT_LOCAL,  gcov_info_idx);
  Set_ST_is_initialized(newst);
  Allocate_Object(newst);

  INITO_IDX inito = New_INITO(newst);
  INITV_IDX inv;
  INITV_IDX prev_inv;

  /* Version ident */
  inv = New_INITV ();
  INITV_Init_Integer (inv, MTYPE_U4, GCOV_VERSION);
  prev_inv = Append_INITV (inv, inito, INITV_IDX_ZERO);

  /* next -- NULL */
  inv = New_INITV ();
  INITV_Init_Integer (inv, MTYPE_U4, 0);
  prev_inv = Append_INITV (inv, inito, prev_inv);

  /* stamp */
  inv = New_INITV ();
  INITV_Init_Integer (inv, MTYPE_U4, local_tick);
  prev_inv = Append_INITV (inv, inito, prev_inv);

  /* Filename */
  char *filename;
  char *pwdname = (char*) alloca(PATH_MAX);
  int filename_len;
  if (getcwd(pwdname, PATH_MAX) == NULL) {
    fprintf(stderr, "error getting current dir: pwd is too long");
    pwdname = NULL;
  }
  if (pwdname && da_file_name[0] != '/') {
    filename = TYPE_MEM_POOL_ALLOC_N(char,
				     &MEM_coverage_pool,
				     strlen(pwdname) + strlen(da_file_name) + 2);
    sprintf(filename,"%s/%s", pwdname, da_file_name);
  } else
    filename = da_file_name;  
  filename_len = strlen (filename);
  TCON tcon = Host_To_Targ_String (MTYPE_STRING,
				   filename,
				   filename_len + 1); 
  ST *const_st = New_Const_Sym(Enter_tcon(tcon), Be_Type_Tbl(TCON_ty(tcon)));
  inv = New_INITV ();
  INITV_Init_Symoff (inv, const_st, 0, 1);
  prev_inv = Append_INITV (inv, inito, prev_inv);
  
  /* number of functions */
  inv = New_INITV ();
  INITV_Init_Integer (inv, MTYPE_U4, n_fns);
  prev_inv = Append_INITV (inv, inito, prev_inv);

  /* fn_info table */
  /* add the fn_info type and initializer.  */
  inv = New_INITV ();
  INITV_Init_Symoff (inv, fn_st, 0, 1);
  prev_inv = Append_INITV (inv, inito, prev_inv);

  /* counter_mask */
  inv = New_INITV ();
  INITV_Init_Integer (inv, MTYPE_U4, prg_ctr_mask);
  prev_inv = Append_INITV (inv, inito, prev_inv);

  /* Build the ctr_info type and initializer.  */
  for (ix = 0; ix != GCOV_COUNTERS; ix++)
    if (prg_ctr_mask & (1 << ix))
      prev_inv = build_ctr_info_value(inv, inito, prev_inv, ix);
  return newst;
}

/* Write out the structure which libgcov uses to locate all the
   counters.  The structures used here must match those defined in
   gcov-io.h.  Write out the constructor to call __gcov_init.  */
static void
create_coverage (FILE *file, TY_IDX counter_type)
{
  unsigned n_fns;

  /* Create the structure gcov_fn_info*/
  TY_IDX gcov_fn_info_idx = build_fn_info_type();

  /* Create the structure gcov_ctr_info*/
  TY_IDX gcov_ctr_info_idx = build_ctr_info_type(counter_type);

  /* Create the struct gcov_info*/
  TY_IDX  gcov_info_idx = build_gcov_info_type(gcov_fn_info_idx, gcov_ctr_info_idx);

  /* Build the fn_info type and initializer.  */
  ST *fn_st = build_fn_info_value (gcov_fn_info_idx, &n_fns);

  // Create a new symbol of type gcov_info
  ST *gcov_info_st = build_gcov_info_value(gcov_info_idx, fn_st, n_fns);

  // Create constructor section
  ST *init_st = New_ST (GLOBAL_SYMTAB);
  ST_Init (init_st, Save_Str2i ("__ctors_for_cov", "_", 0),
           CLASS_VAR, SCLASS_FSTATIC,
           EXPORT_LOCAL, Make_Pointer_Type(MTYPE_To_TY(MTYPE_V)));
  Set_ST_is_initialized (init_st);
  INITO_IDX inito = New_INITO(init_st);
  INITV_IDX initv = New_INITV ();
  INITV_IDX prev_inv;
  INITV_Init_Symoff (initv, gcov_init_thunk_st, 0, 1);
  prev_inv = Append_INITV (initv, inito, INITV_IDX_ZERO);
  ST_ATTR_IDX st_attr_idx;
  ST_ATTR&    st_attr = New_ST_ATTR (GLOBAL_SYMTAB, st_attr_idx);
  ST_ATTR_Init (st_attr, ST_st_idx (init_st), ST_ATTR_SECTION_NAME,
                Save_Str (".ctors"));
  Set_ST_has_named_section (init_st);
  // to avoid Warning: Assigned section name (.ctors) has ambiguous
  // attributes (both .data and .rodata) when a ctors section has
  // already been created, ignore Set_ST_is_const_var.
  //   Set_ST_is_const_var (init_st);
  Allocate_Object (init_st);

  // Generate the function that calls __gcov_init
  generate_thunk_for_gcov_init(file);
}

/* Compute checksum for the current function.  We generate a CRC32.  */
static unsigned
compute_checksum (void)
{
/*   unsigned chksum = DECL_SOURCE_LINE (current_function_decl);
     source line of the current function */
  unsigned chksum = 3;

  chksum = coverage_checksum_string (chksum,
      				     "hello.c"/* DECL_SOURCE_FILE (current_function_decl) */);
  chksum = coverage_checksum_string
    (chksum, "main"/* IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (current_function_decl)) */);

  return chksum;
}



/* Perform file-level initialization. Read in data file, generate name
   of graph file.  */

void
coverage_init (const char *filename)
{
  int len = strlen (filename);

  /* Start a new local memory allocation space
   */
  MEM_POOL_Initialize(&MEM_coverage_pool,"coverage pool",FALSE);
  MEM_POOL_Push(&MEM_coverage_pool);

  randomize();
  //   Name of da file.
  da_file_name = TYPE_MEM_POOL_ALLOC_N(char,
				       &MEM_coverage_pool,
				       len + strlen (GCOV_DATA_SUFFIX) + 1);
  strcpy (da_file_name, filename);
  strcat (da_file_name, GCOV_DATA_SUFFIX);

  /* Name of bbg file.  */
  bbg_file_name = TYPE_MEM_POOL_ALLOC_N(char,
				       &MEM_coverage_pool,
					len + strlen (GCOV_NOTE_SUFFIX) + 1);
  strcpy (bbg_file_name, filename);
  strcat (bbg_file_name, GCOV_NOTE_SUFFIX);

/*   read_counts_file (); */
}


/* Performs file-level cleanup.  Generate coverage
   variables and constructor.  */
void
coverage_finish (FILE *file, unsigned num_instr_edges)
{
  
  if (!prg_ctr_mask)
    return;
  // Set size for gcov symbols
  // Recreate the symbol to force emition in Process_Bss_Data()
  ST *st_name = ctr_labels[GCOV_COUNTER_ARCS];
  TY_IDX ty_idx = Make_Array_Type(MTYPE_U4, 
				  1, sizeof(gcov_type)/4 * num_instr_edges);
  Set_TY_align(ty_idx, 8);
  ST *newst = New_ST(GLOBAL_SYMTAB);
  ST_Init(newst, 
	  Save_Str(ST_name(st_name)),
	  CLASS_VAR, SCLASS_FSTATIC, EXPORT_LOCAL_INTERNAL,  ty_idx);
  Allocate_Object(newst);

  // Create gcov ctor section
  create_coverage (file, ty_idx);
  if (bbg_file_opened)
    {
      int error = gcov_close ();

      if (error)
	unlink (bbg_file_name);
/*       if (!local_tick) */
/* 	/\* Only remove the da file, if we cannot stamp it. If we can */
/* 	   stamp it, libgcov will DTRT.  *\/ */
/* 	unlink (da_file_name); */
    }
  // free memory
  MEM_POOL_Pop(&MEM_coverage_pool);
}

/* Begin output to the graph file for the current function.
   Opens the output file, if not already done. Writes the
   function header, if not already done. Returns nonzero if data
   should be output.  
   pu: only to get name of current PU
   bb: only to get filename where pu is defined
*/
int
coverage_begin_output (ST *pu, BB *bb)
{
  if (no_coverage)
    return 0;

  if (!bbg_function_announced)
    {
      const char *file;
      const char *path; 
      char *complete_name;
      unsigned line;
      unsigned long offset;
      INT file_idx;
      ANNOTATION *ant;
      ENTRYINFO *ent;
      SRCPOS entry_srcpos;
      INT include_idx;
      USRCPOS usrcpos;
      
      /* Reset last_srcpos for new function */
      last_srcpos = 0;
      /* Compute a mapping between BB_id and gcov number */
      gcov_prepass();
      ant = ANNOT_Get (BB_annotations(bb), ANNOT_ENTRYINFO);
      ent = ANNOT_entryinfo(ant);
      entry_srcpos = ENTRYINFO_srcpos(ent);
      USRCPOS_srcpos(usrcpos) = entry_srcpos;
      file_idx = USRCPOS_filenum(usrcpos);
      include_idx = Cg_Dwarf_File_Table(file_idx).incl_index;
      path = Cg_Dwarf_Include_Table(include_idx).path_name;
      file = Cg_Dwarf_File_Table(file_idx).filename;
      line = USRCPOS_linenum(usrcpos);
      if (!bbg_file_opened)
	{
	  if (!gcov_open (bbg_file_name, -1))
	    fprintf (stderr, "cannot open %s", bbg_file_name);
	  else
	    {
	      gcov_write_unsigned (GCOV_NOTE_MAGIC);
	      gcov_write_unsigned (GCOV_VERSION);
	      gcov_write_unsigned (local_tick);
	    }
	  bbg_file_opened = 1;
	}

      /* Announce function */
      offset = gcov_write_tag (GCOV_TAG_FUNCTION);
      /* Write unique identificateur for the function  */
      gcov_write_unsigned ( current_function_funcdef_no);
      gcov_write_unsigned (compute_checksum ());
/*       gcov_write_string (IDENTIFIER_POINTER */
/* 			 (DECL_ASSEMBLER_NAME (current_function_decl))); */
      /* write function name TO BE DONE */
      gcov_write_string (ST_name(pu));

      complete_name = (char *)alloca(strlen (path) + strlen (file) + 2);
      strcpy (complete_name, path);
      strcat (complete_name, "/");
      strcat (complete_name, file);

      gcov_write_string (complete_name);
      gcov_write_unsigned (line);
      gcov_write_length (offset);

      bbg_function_announced = 1;
    }
  return !gcov_is_error ();
}

/* Finish coverage data for the current function. Verify no output
   error has occurred.  Save function coverage counts.  */

void
coverage_end_function (void)
{
  unsigned i;

  if (bbg_file_opened > 1 && gcov_is_error ())
    {
      fprintf(stderr, "error writing `%s'", bbg_file_name);
      bbg_file_opened = 0xFFFFFFFF;
    }

  if (fn_ctr_mask)
    {
      struct function_list *item;

      item = TYPE_MEM_POOL_ALLOC_N(struct function_list,
					   &MEM_coverage_pool,
					   sizeof (struct function_list));
      *functions_tail = item;
      functions_tail = &item->next;

      item->next = 0;
      /* Write unique identificateur for the function  */
      item->ident = current_function_funcdef_no++;
      item->checksum = compute_checksum ();
      for (i = 0; i != GCOV_COUNTERS; i++)
	{
	  item->n_ctrs[i] = fn_n_ctrs[i];
	  prg_n_ctrs[i] += fn_n_ctrs[i];
	  fn_n_ctrs[i] = fn_b_ctrs[i] = 0;
	}
      prg_ctr_mask |= fn_ctr_mask;
      fn_ctr_mask = 0;
    }
  bbg_function_announced = 0;
}

/* 
   This function writes basic block part of '.gcno' file for the
current function.
*/
void
coverage_blocks ()
{
  gcov_position_t offset;
  unsigned i;
  offset = gcov_write_tag (GCOV_TAG_BLOCKS);
  // BBs are expected to be sequentially numbered
  for (i = FIRST_BB_TO_GCOV_INDEX; i <= LAST_BB_TO_GCOV_INDEX; i++)
    gcov_write_unsigned (0);
  gcov_write_length (offset);
}

/*
 * Return arc (bb->succ) gcov properties
 */
static unsigned
gcov_arc_flags(edge_t e) {
  unsigned flag_bits = 0;
  if (e->fake)
    flag_bits |= GCOV_ARC_FAKE;
  if (e->fall_through)
    flag_bits |= GCOV_ARC_FALLTHROUGH;
  if (e->on_tree)
    flag_bits |= GCOV_ARC_ON_TREE;

  return flag_bits;
}

/* For each BB in the CFG, create one gcov EDGE 
 * for each successor. Returns number of instrumented edges
 */
unsigned 
coverage_arcs (struct edge_list *edges_list) {
  gcov_position_t offset = 0;
  unsigned nb_instr = 0 ;
  int x;
  unsigned flag_bits;
  BB* last_src = NULL;

  for (x = 0; x < edges_list->num_edges; x++) {
    edge_t e = edges_list->index_to_edge[x];
    // Compte nb of instrumented edges
    if (!e->fake && !e->on_tree)
      nb_instr++;
    if (last_src == NULL || e->src != last_src) {
      // Write previous offset
      if (offset)
	gcov_write_length (offset);

      offset = gcov_write_tag (GCOV_TAG_ARCS);
      if (e->from_entry)
	gcov_write_unsigned (FIRST_BB_TO_GCOV_INDEX);
      else
	gcov_write_unsigned (BB_TO_GCOV_INDEX (e->src));
    }
    flag_bits = gcov_arc_flags(e);
    if (e->to_exit) 
      gcov_write_unsigned (LAST_BB_TO_GCOV_INDEX);
    else
      gcov_write_unsigned (BB_TO_GCOV_INDEX (e->dest));
    gcov_write_unsigned (flag_bits);
    last_src = e->src;
  }
  if (offset)
    gcov_write_length (offset);
  return nb_instr;
}


/* for a given BB in the CFG, add gcov line info 
 */
void
coverage_lines (BB *bb) {
  OP *op;
  gcov_position_t offset;
  SRCPOS srcpos;
  USRCPOS usrcpos;

  offset = 0;
  
  // For entry block add line info from bb to fix a bug for empty
  // function. They had no line info otherwise
  if ( BB_entry(bb) ) {
    ANNOTATION *ant = ANNOT_Get (BB_annotations(bb), ANNOT_ENTRYINFO);
    ENTRYINFO *ent = ANNOT_entryinfo (ant);
    srcpos = ENTRYINFO_srcpos(ent);
    if (last_srcpos != srcpos) {
      if (!offset) {
	offset = gcov_write_tag (GCOV_TAG_LINES);
	gcov_write_unsigned (BB_TO_GCOV_INDEX (bb));
      }
      USRCPOS_srcpos(usrcpos) = srcpos;
      USRCPOS last_usrcpos;
      if (last_srcpos != 0) {
	USRCPOS_srcpos(last_usrcpos) = last_srcpos;
      }
      /* If this is a new source file, then output the
	 file's name to the .bb file.  */
      if ( last_srcpos == 0 || 
	   USRCPOS_filenum(last_usrcpos) != USRCPOS_filenum(usrcpos)) {
	INT file_idx, include_idx;
	char const *file_name;
	char const *path;
	char *complete_name;

	file_idx = USRCPOS_filenum(usrcpos);
	include_idx = Cg_Dwarf_File_Table(file_idx).incl_index;
	path = Cg_Dwarf_Include_Table(include_idx).path_name;
	file_name = Cg_Dwarf_File_Table(file_idx).filename;

	complete_name = (char *)alloca(strlen (path) + strlen (file_name) + 2);
	strcpy (complete_name, path);
	strcat (complete_name, "/");
	strcat (complete_name, file_name);
	gcov_write_unsigned (0);
	gcov_write_string (complete_name);
      }
      unsigned line;
      line = USRCPOS_linenum(usrcpos);
      gcov_write_unsigned (line);
      last_srcpos = srcpos;
    }
  }

  // For all op in the bb, add line info
  for (op = BB_first_op(bb);op != NULL;) {
    srcpos = OP_srcpos(op);
    USRCPOS_srcpos(usrcpos) = srcpos;
    unsigned line;
    line = USRCPOS_linenum(usrcpos);
    if (line == 0) 
      {
	op = OP_next(op);
	continue;
      }
    // last_srcpos is remanent within a function, put to 0 for a new
    // function
    // If the position is different from last one.
    if (last_srcpos != srcpos) {
      if (!offset) {
	offset = gcov_write_tag (GCOV_TAG_LINES);
	gcov_write_unsigned (BB_TO_GCOV_INDEX (bb));
      }
      USRCPOS last_usrcpos;
      if (last_srcpos != 0) {
	USRCPOS_srcpos(last_usrcpos) = last_srcpos;
      }
      /* If this is a new source file, then output the
	 file's name to the .bb file.  */
      if ( last_srcpos == 0 || 
	   USRCPOS_filenum(last_usrcpos) != USRCPOS_filenum(usrcpos)) {
	INT file_idx, include_idx;
	char const *file_name;
	char const *path;
	char *complete_name;

	file_idx = USRCPOS_filenum(usrcpos);
	include_idx = Cg_Dwarf_File_Table(file_idx).incl_index;
	path = Cg_Dwarf_Include_Table(include_idx).path_name;
	file_name = Cg_Dwarf_File_Table(file_idx).filename;

	complete_name = (char *)alloca(strlen (path) + strlen (file_name) + 2);
	strcpy (complete_name, path);
	strcat (complete_name, "/");
	strcat (complete_name, file_name);
	gcov_write_unsigned (0);
	gcov_write_string (complete_name);
      }
      gcov_write_unsigned (line);
      last_srcpos = srcpos;
    }
    op = OP_next(op);
  }
  if (offset)
    {
      /* A file of NULL indicates the end of run.  */
      gcov_write_unsigned (0);
      gcov_write_string (NULL);
      gcov_write_length (offset);
    }
}

/* P is a string that will be used in a symbol.  Mask out any characters
   that are not valid in that context.  */
    
static void
clean_symbol_name (char *p)
{
  for (; *p; p++)
    if ( !((*p >= 65 && *p <= 90 ) || (*p >= 97 && *p <= 122 )))
      *p = '_';
}

static char *
get_file_function_name_long (const char *type)
{
  char *buf;
  const char *p;
  char *q;
  
  /* We don't have anything that we know to be unique to this translation
     unit, so use what we do have and throw in some randomness.  */
  
  const char *name = "";
  const char *file = Src_File_Name;
  
  q = (char *) alloca (1 + strlen (name) + strlen (file));
  
  sprintf (q, "%s%s", name, file);
  p = q;

  buf = TYPE_MEM_POOL_ALLOC_N(char,
			      &MEM_coverage_pool,
			      sizeof (FILE_FUNCTION_FORMAT) + strlen (p)
			      + strlen (type));

  /* Set up the name of the file-level functions we may need.
     Use a global object (which is already required to be unique over
     the program) rather than the file name (which imposes extra
     constraints).  */
  sprintf (buf, FILE_FUNCTION_FORMAT, type, p);

  /* Don't need to pull weird characters out of global names.  */
  clean_symbol_name (buf + 11);

  return buf;
}

/* Allocate NUM counters of type COUNTER. Returns nonzero if the
   allocation succeeded.  */
int
coverage_counter_alloc (unsigned counter, unsigned num)
{
  if (no_coverage)
    return 0;

  if (!num)
    return 1;

  if (!ctr_labels[counter])
    {
      unsigned ix;
      /* Create a new symbol to access the counter.  */
      char *buf;
      ST *newst;
      /* Name this symbol: */
      buf = (char *)alloca(32);
      sprintf(buf, "%s%d", ".LPBX", counter + 1);

      // add a dummy st for instrumentation
      // the real symbol will be allocated at end of file
      newst = New_ST(GLOBAL_SYMTAB);
      ST_Init(newst, 
	      Save_Str(buf),
	      CLASS_NAME , SCLASS_UNKNOWN, EXPORT_LOCAL, 
	      Make_Pointer_Type(MTYPE_To_TY(MTYPE_U4)));
      ctr_labels[counter] = newst;
      // Create __gcov_init function now
      // no need for the moment. We don't generate a new pu for the
      // thunk, so no need to define __gcov_init as an intrinsic PU
#if 0
      TY_IDX  ty = Make_Function_Type( MTYPE_To_TY( MTYPE_V ) );
      gcov_init_st = Gen_Intrinsic_Function( ty, "__gcov_init" );
      Clear_PU_no_side_effects( Pu_Table[ST_pu( gcov_init_st )] );
      Clear_PU_is_pure( Pu_Table[ST_pu( gcov_init_st )] );
      Set_PU_no_delete( Pu_Table[ST_pu( gcov_init_st )] );
      Set_PU_no_instrument_function(Pu_Table[ST_pu( gcov_init_st )]);
#endif
      // Create thunk name where call to __gcov_init is done
      gcov_init_thunk_name = get_file_function_name_long ("I");
      TY_IDX   ty = Make_Function_Type( MTYPE_To_TY( MTYPE_V ) );
      gcov_init_thunk_st = Gen_Intrinsic_Function( ty, gcov_init_thunk_name);
      Clear_PU_no_side_effects( Pu_Table[ST_pu( gcov_init_thunk_st )] );
      Clear_PU_is_pure( Pu_Table[ST_pu( gcov_init_thunk_st )] );
      Set_PU_no_delete( Pu_Table[ST_pu( gcov_init_thunk_st )] );
      Set_PU_no_instrument_function(Pu_Table[ST_pu( gcov_init_thunk_st )]);

      // Create an intrinsic symbol for each merge function
      for (ix = 0; ix != GCOV_COUNTERS; ix++) {
	TY_IDX  ty = Make_Function_Type( MTYPE_To_TY( MTYPE_V ) );
	ctr_merge_functions_st[ix] = Gen_Intrinsic_Function( ty, (char*)ctr_merge_functions[ix] );
	Clear_PU_no_side_effects( Pu_Table[ST_pu( ctr_merge_functions_st[ix] )] );
	Clear_PU_is_pure( Pu_Table[ST_pu( ctr_merge_functions_st[ix] )] );
	Set_PU_no_delete( Pu_Table[ST_pu( ctr_merge_functions_st[ix] )] );
	Set_PU_no_instrument_function(Pu_Table[ST_pu( ctr_merge_functions_st[ix] )]);
      }
    }
  fn_b_ctrs[counter] = fn_n_ctrs[counter];
  fn_n_ctrs[counter] += num;
  fn_ctr_mask |= 1 << counter;
  return 1;
}

/* Output instructions  to increment the edge execution count.  */
void 
gen_edge_profiler (unsigned no, OPS *New_OPs )
{
  TYPE_ID mode = MTYPE_U4; /* don't know which MTYPE to use, fix later */

  FmtAssert(no < fn_n_ctrs[GCOV_COUNTER_ARCS] - fn_b_ctrs[GCOV_COUNTER_ARCS], 
	    ("gen_edge_profiler: Edge counter bigger than total number of edges"));
  no += prg_n_ctrs[GCOV_COUNTER_ARCS] + fn_b_ctrs[GCOV_COUNTER_ARCS];

  /* Generate an OPS to access COUNTER NO.  */
  /* Generate code to load memory location 'sym'+'ofst' into 'tgt_tn'. */
#define GCOV_COUNTER_SIZE_4 1
#if GCOV_COUNTER_SIZE_4
  /* Resulting register where is put the load */
  //    TN *tn = Gen_Register_TN (ISA_REGISTER_CLASS_integer, Pointer_Size);
  TN *tn = Gen_Register_TN(ISA_REGISTER_CLASS_integer,/* size */4);
  Expand_Load (OPCODE_make_op(OPR_LDID, mode, mode), 
	       tn, 
	       Zero_TN,
	       Gen_Symbol_TN(ctr_labels[GCOV_COUNTER_ARCS], 
			     sizeof(gcov_type) * no, 0), 
	       New_OPs);

  /* increment +1 the counter */
  //   extern void Expand_Add (TN *result, TN *src1, TN *src2, TYPE_ID mtype, OPS *ops);
  Expand_Add(tn, tn, Gen_Literal_TN(1, 4), mode, New_OPs);

  // Store the results in mem
  // extern void Expand_Store (TYPE_ID mtype, TN *src1, TN *src2, TN *src3, OPS *ops);
  Expand_Store (mode, tn, 
		Zero_TN, 
		Gen_Symbol_TN(ctr_labels[GCOV_COUNTER_ARCS], sizeof(gcov_type) * no, 0),
		New_OPs);
#else //#if GCOV_COUNTER_SIZE_8
  // Just a draft....
extern void Exp_Lda (
  TYPE_ID mtype, TN *tgt_tn, ST *sym, INT64 ofst, OPERATOR call_opr, OPS *ops);
  TN  *kids[4];
  TN  *res[2];
  TN *tn = Build_TN_Of_Mtype (MTYPE_U4);
  //	mov $r8 = toto                
  Exp_Lda (
	   OPCODE_rtype(OPC_U4LDA),
	   tn,
	   Gen_Symbol_TN(ctr_labels[GCOV_COUNTER_ARCS], 
			 sizeof(gcov_type) * no, 0), 
	   0,
	   OPR_STID,
	   New_OPs);
  TN *tn2 = Build_TN_Of_Mtype (MTYPE_U4);
  //	ldw $r15 = 0 [ $r8 ]          	## id:5 toto+0x0
     Exp_COPY (tn2, tn, &New_OPs);
  TN *tn3 = Build_TN_Of_Mtype (MTYPE_I4);
  TN *tn4 = Gen_Literal_TN ( 4, Pointer_Size);
  //	ldw $r10 = 4 [ $r8 ]          	## id:5 toto+0x0
  Expand_Load (OPC_I4I4ILOAD, tn3, tn1, tn4, New_OPs);
  res[0] = Build_TN_Of_Mtype (MTYPE_I4);
  res[1] = Build_TN_Of_Mtype (MTYPE_I4);
  for (i = 0; i < 3; i++) {
    kids[i] = Build_TN_Of_Mtype (MTYPE_I4);
  }
  kids[3] = Gen_Literal_TN ( 0, 4);;

  //	mtb $b0 = $r0                 
 Exp_Intrinsic_Op (INTRN_ADDL, 2, 4, res, kids, New_OPs);
 Print_OPs(New_OPs);

#endif


}

/* Output conditional instructions to increment the edge execution
   count. This used to add a conter on critical edge*/
void 
gen_critical_edge_profiler (unsigned no, OPS *New_OPs, BB *src, BB *dst )
{
  TYPE_ID mode = MTYPE_U4; /* don't know which MTYPE to use, fix later */

  if (no >= fn_n_ctrs[GCOV_COUNTER_ARCS] - fn_b_ctrs[GCOV_COUNTER_ARCS])
    FmtAssert(FALSE, ("gen_edge_profiler: Edge counter bigger than total number of edges"));
  no += prg_n_ctrs[GCOV_COUNTER_ARCS] + fn_b_ctrs[GCOV_COUNTER_ARCS];


  OP *br_op = BB_branch_op(src);
  TN *branch_tn;
  TN *dummy;
  VARIANT variant = CGTARG_Analyze_Branch(br_op, &branch_tn, &dummy);
  BOOL false_br = V_false_br(variant)?TRUE:FALSE;
  /* Generate an OPS to access COUNTER NO.  */
  /* Generate code to load memory location 'sym'+'ofst' into 'tgt_tn'. */
  /* Resulting register where is put the load */
  //    TN *tn = Gen_Register_TN (ISA_REGISTER_CLASS_integer, Pointer_Size);
  TN *tn = Gen_Register_TN(ISA_REGISTER_CLASS_integer,/* size */4);
  Expand_Load (OPCODE_make_op(OPR_LDID, mode, mode), 
	       tn, 
	       Zero_TN,
	       Gen_Symbol_TN(ctr_labels[GCOV_COUNTER_ARCS], 
			     sizeof(gcov_type) * no, 0), 
	       New_OPs);

  /* increment +1 the counter */
  TN *add_tn = Gen_Register_TN(ISA_REGISTER_CLASS_integer,/* size */4);
  Expand_Add(add_tn, tn, Gen_Literal_TN(1, 4), mode, New_OPs);

  TN *select_tn = Gen_Register_TN(ISA_REGISTER_CLASS_integer,/* size */4);
  /* Detect FALL THRU edges */
  if (BB_next(src) == dst) {
    // Fall Thru
    if (false_br)
      Expand_Select (select_tn, branch_tn, add_tn, tn, MTYPE_U4,
		     FALSE, New_OPs);
    else
      Expand_Select (select_tn, branch_tn, tn, add_tn, MTYPE_U4,
		     FALSE, New_OPs);
    }
  else {
    // Branch
    if (false_br)
      Expand_Select (select_tn, branch_tn, tn, add_tn, MTYPE_U4,
		     FALSE, New_OPs);
    else
      Expand_Select (select_tn, branch_tn, add_tn, tn, MTYPE_U4,
		     FALSE, New_OPs);
  }
  // Store the results in mem
  // extern void Expand_Store (TYPE_ID mtype, TN *src1, TN *src2, TN *src3, OPS *ops);
  Expand_Store (mode, select_tn, 
		Zero_TN, 
		Gen_Symbol_TN(ctr_labels[GCOV_COUNTER_ARCS], sizeof(gcov_type) * no, 0),
		New_OPs);
}

