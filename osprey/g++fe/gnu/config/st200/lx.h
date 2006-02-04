/* Definitions of target machine GNU compiler for the ST200.
   Christian Bruel <christian.bruel@st.com>


This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

extern int lx_delay_model;

/* Which architecture to generate code for.  */
extern const char *lx_cpu_string;    /* For -mcpu= */
extern int lx_cpu;

extern struct rtx_def * lx_compare_op0;
extern struct rtx_def * lx_compare_op1;

#define FIT_UI16(X)       ((X)>=0 && (X)<=0xFFFF)
#define FIT_SI16(X)       ((X)>=-0x8000 && (X)<=0x7FFF)
#define FIT_HI16(X)       (((X) & 0xFFFF) == 0)
#define FORCE_LONG_IMM(X) ((X)>255 || (X)<-256)

#define INCLUDE_DEFAULTS {{0, 0, 0, 0}}

/* ********** Controlling the Compilation Driver ********** */

/* A C string constant that tells the GNU CC driver program options to pass to
   CPP.  It can also specify how to translate options you give to GNU CC into
   options for GNU CC to pass to the CPP.  */
#define CPP_SPEC \
  "%{mcpu=st200:-D__st200__} \
   %{mcpu=st210:-D__st210__} \
   %{mcpu=st220:-D__st220__} \
   %{!mlittle-endian:-D__BIG_ENDIAN__} -D__ST200"

/* If this macro is defined, the preprocessor will not define the builtin macro
   `__SIZE_TYPE__'.  The macro `__SIZE_TYPE__' must then be defined by
   `CPP_SPEC' instead.

   this should be defined if `SIZE_TYPE' depends on target dependent flags
   which are not accessible to the preprocessor.  Otherwise, it should not be
   defined.  */
/* #define NO_BUILTIN_SIZE_TYPE */

/* If this macro is defined, the preprocessor will not define the builtin macro
   `__PTRDIFF_TYPE__'.  The macro `__PTRDIFF_TYPE__' must then be defined by
   `CPP_SPEC' instead.

   This should be defined if `PTRDIFF_TYPE' depends on target dependent flags
   which are not accessible to the preprocessor.  Otherwise, it should not be
   defined.  */
/* #define NO_BUILTIN_PTRDIFF_TYPE */

/* A C string constant that tells the GNU CC driver program options to pass to
   `cc1'.  It can also specify how to translate options you give to GNU CC into
   options for GNU CC to pass to the `cc1'.  */
/* #define CC1_SPEC "" */

/* A C string constant that tells the GNU CC driver program options to pass to
   `cc1plus'.  It can also specify how to translate options you give to GNU CC
   into options for GNU CC to pass to the `cc1plus'.  */
/* #define CC1PLUS_SPEC "" */

/* A C string constant that tells the GNU CC driver program options to pass to
   the assembler.  It can also specify how to translate options you give to GNU
   CC into options for GNU CC to pass to the assembler.  */
#define ASM_SPEC "%{mlittle-endian:-EL}"

/* A C string constant that tells the GNU CC driver program options to pass to
   the linker.  It can also specify how to translate options you give to GNU CC
   into options for GNU CC to pass to the linker.  */
#define LINK_SPEC "%{mlittle-endian:-EL}"

/* Define this macro as a C expression for the initializer of an array of
   string to tell the driver program which options are defaults for this
   target and thus do not need to be handled specially when using
   MULTILIB_OPTIONS. See also TARGET_DEFAULT value. */
#define MULTILIB_DEFAULTS { "mbig-endian" }

/* ********** Run-time target specifications ********** */

/* Define this to be a string constant containing `-D' options to define the
   predefined macros that identify this machine and system.  These macros will
   be predefined unless the `-ansi' option is specified.  */
#ifndef CPP_PREDEFINES
#define CPP_PREDEFINES "-Acpu(st200) -Amachine(st200)"
#endif

#define DONT_USE_BUILTIN_SETJMP

/* This declaration should be present.  */
extern int target_flags;

/* This series of macros is to allow compiler command arguments to enable or
   disable the use of optional features of the target machine.  */
#define MASK_BIG_ENDIAN	  0x0001	    /* Generate big endian code.  */
#define MASK_NO_BUNDLE    0x0002	    /* One instruction/bundle.    */
#define MASK_KERNEL_MODE  0x0004	    /* Kernel execution mode.     */

#define TARGET_BIG_ENDIAN	(target_flags & MASK_BIG_ENDIAN)
#define TARGET_NO_BUNDLE	(target_flags & MASK_NO_BUNDLE)
#define TARGET_KERNEL_MODE	(target_flags & MASK_KERNEL_MODE)

/* Default target_flags if no switches are specified  */
#ifndef TARGET_DEFAULT
#define TARGET_DEFAULT 0
#endif

#ifndef TARGET_CPU_DEFAULT
#define TARGET_CPU_DEFAULT 0
#endif

/* This macro defines names of command options to set and clear bits in
   `target_flags'.  Its definition is an initializer with a subgrouping for
   each command option.  */
#define TARGET_SWITCHES                         \
{					        \
  { "big-endian",	MASK_BIG_ENDIAN,        \
      "Generate big endian code" },	        \
  { "little-endian",	-MASK_BIG_ENDIAN,       \
      "Generate little endian code" },	        \
  { "bundle",	        -MASK_NO_BUNDLE,        \
      "Pack instructions into bundles" },       \
  { "no-bundle",	MASK_NO_BUNDLE,         \
      "Pack one instruction/bundle" },          \
  { "kernel",	        MASK_KERNEL_MODE,       \
      "Kernel execution mode" },                \
  { "no-kernel",	-MASK_KERNEL_MODE,      \
      "User execution mode" },                  \
  { "",			TARGET_DEFAULT | TARGET_CPU_DEFAULT,  \
      NULL }				        \
}

#define TARGET_OPTIONS			\
{					\
  { "cpu=", &lx_cpu_string,        \
 "Specify architecture for code generation.  Values are st210 and st220 " }\
}


/* This macro is a C statement to print on `stderr' a string describing the
   particular machine description choice.  */
#define TARGET_VERSION          { fputs ("ST200/ELF", stderr); }

/* Sometimes certain combinations of command options do not make sense on a
   particular target machine.  You can define a macro `OVERRIDE_OPTIONS' to
   take account of this.  This macro, if defined, is executed once just after
   all the command options have been parsed.  */
#define OVERRIDE_OPTIONS      override_options ();

/* Some machines may desire to change what optimizations are performed for
   various optimization levels.  This macro, if defined, is executed once just
   after the optimization level is determined and before the remainder of the
   command options have been parsed.  Values set in this macro are used as the
   default values for the other command line options.
#define OPTIMIZATION_OPTIONS(LEVEL,SIZE)		\
{							\
  if (! LEVEL)						\
    target_flags |= MASK_NO_BUNDLE;                     \
}
 */

/* Define this macro if debugging can be performed even without a frame
   pointer.  If this macro is defined, GNU CC will turn on the
   `-fomit-frame-pointer' option whenever `-O' is specified.  */
#define CAN_DEBUG_WITHOUT_FP

/* ********** Storage Layout ********** */

/* Define this macro to have the value 1 if the most significant bit in a byte
   has the lowest number; otherwise define it to have the value zero.  */
#define BITS_BIG_ENDIAN 0

/* Define this macro to have the value 1 if the most significant byte in a word
   has the lowest number.  This macro need not be a constant.  */
#define BYTES_BIG_ENDIAN (TARGET_BIG_ENDIAN != 0)

/* Define this macro to have the value 1 if, in a multiword object, the most
   significant word has the lowest number.  */
#define WORDS_BIG_ENDIAN (TARGET_BIG_ENDIAN != 0)

/* Define this macro if WORDS_BIG_ENDIAN is not constant.  This must be a
   constant value with the same meaning as WORDS_BIG_ENDIAN, which will be used
   only when compiling libgcc2.c.  Typically the value will be set based on
   preprocessor defines.  */
#if defined(__BIG_ENDIAN__)
#define LIBGCC2_WORDS_BIG_ENDIAN 1
#else
#define LIBGCC2_WORDS_BIG_ENDIAN 0
#endif

/* Define this macro to be the number of bits in an addressable storage unit
   (byte); normally 8.  */
#define BITS_PER_UNIT 8

/* Number of bits in a word; normally 32.  */
#define BITS_PER_WORD 32

/* Number of storage units in a word; normally 4.  */
#define UNITS_PER_WORD          (BITS_PER_WORD/BITS_PER_UNIT)

/* Width of a pointer, in bits.  You must specify a value no wider than the
   width of `Pmode'.  If it is not equal to the width of `Pmode', you must
   define `POINTERS_EXTEND_UNSIGNED'.  */
#define POINTER_SIZE 32

/* A C expression whose value is nonzero if pointers that need to be extended
   from being `POINTER_SIZE' bits wide to `Pmode' are sign-extended and zero if
   they are zero-extended.

   You need not define this macro if the `POINTER_SIZE' is equal to the width
   of `Pmode'.  */
/* #define POINTERS_EXTEND_UNSIGNED */

/* A macro to update MODE and UNSIGNEDP when an object whose type is TYPE and
   which has the specified mode and signedness is to be stored in a register.
   This macro is only called when TYPE is a scalar type.  */
/* *** investigate *** */
#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE)	\
  if (GET_MODE_CLASS (MODE) == MODE_INT		\
      && GET_MODE_SIZE (MODE) < 4)		\
    (MODE) = SImode;

/* Define this macro if the promotion described by `PROMOTE_MODE' should also
   be done for outgoing function arguments.  */
/* #define PROMOTE_FUNCTION_ARGS */

/* Define this macro if the promotion described by `PROMOTE_MODE' should also
   be done for the return value of functions.

   If this macro is defined, `FUNCTION_VALUE' must perform the same promotions
   done by `PROMOTE_MODE'.  */
/* #define PROMOTE_FUNCTION_RETURN */

/* Normal alignment required for function parameters on the stack, in bits.
   All stack parameters receive at least this much alignment regardless of data
   type.  On most machines, this is the same as the size of an integer.  */
#define PARM_BOUNDARY 32

/* Define this macro if you wish to preserve a certain alignment for the stack
   pointer.  The definition is a C expression for the desired alignment
   (measured in bits).  */
#define STACK_BOUNDARY 256

/* Alignment required for a function entry point, in bits.  */
#define FUNCTION_BOUNDARY 32

/* Biggest alignment that any data type can require on this machine,
   in bits.  */
#define BIGGEST_ALIGNMENT 64

/*  __attribute__ align might set one bigger that this  */
/* #define BIGGEST_FIELD_ALIGNMENT 64 */

/* If defined, a C expression to compute the alignment for a static variable.
   TYPE is the data type, and ALIGN is the alignment that the object
   would ordinarily have.  The value of this macro is used instead of that
   alignment to align the object. */
/* #define DATA_ALIGNMENT(TYPE, ALIGN) */

/* If defined, a C expression to compute the alignment given to a constant that
   is being placed in memory.  CONSTANT is the constant and ALIGN is the
   alignment that the object would ordinarily have.  The value of this macro is
   used instead of that alignment to align the object. */
/* #define CONSTANT_ALIGNMENT(EXP, ALIGN) */

/* Define this if instructions will fail to work if given data not
   on the nominal alignment.  If instructions will merely go slower
   in that case, do not define this macro. */
#define STRICT_ALIGNMENT  1

/* Define this macro to be the value 1 if unaligned accesses have a
   cost many times greater than aligned accesses, for example if they
   are emulated in a trap handler.

   When this macro is non-zero, the compiler will act as if
   `STRICT_ALIGNMENT' were non-zero when generating code for block
   moves.  This can cause significantly more instructions to be
   produced.  Therefore, do not set this macro non-zero if unaligned
   accesses only add a cycle or two to the time for a memory access.

   If the value of this macro is always zero, it need not be defined.  */
#define SLOW_UNALIGNED_ACCESS(MODE, ALIGN) 1

/* Define this if you wish to imitate the way many other C compilers handle
   alignment of bitfields and the structures that contain them.
   The behavior is that the type written for a bitfield (`int', `short', or
   other integer type) imposes an alignment for the entire structure, as if the
   structure really did contain an ordinary field of that type.  In addition,
   the bitfield is placed within the structure so that it would fit within such
   a field, not crossing a boundary for it. */
#define PCC_BITFIELD_TYPE_MATTERS 1

/* Define this macro as an expression for the overall size of a structure
   (given by STRUCT as a tree node) when the size computed from the fields is
   SIZE and the alignment is ALIGN.

   The default is to round SIZE up to a multiple of ALIGN.  */
/* #define ROUND_TYPE_SIZE(STRUCT, SIZE, ALIGN) */

/* Define this macro as an expression for the alignment of a structure (given
   by STRUCT as a tree node) if the alignment computed in the usual way is
   COMPUTED and the alignment explicitly specified was SPECIFIED.

   The default is to use SPECIFIED if it is larger; otherwise, use the smaller
   of COMPUTED and `BIGGEST_ALIGNMENT' */
/* #define ROUND_TYPE_ALIGN(STRUCT, COMPUTED, SPECIFIED) */

/* Allow pairs of registers to be used, which is the intent of the default.  */
#define MAX_FIXED_MODE_SIZE GET_MODE_BITSIZE (DImode)

/* A code distinguishing the floating point format of the target machine.  */
#define TARGET_FLOAT_FORMAT IEEE_FLOAT_FORMAT

/* ********** Layout of Source Language Data Types ********** */

/* A C expression for the size in bits of the type `int' on the target machine.
   If you don't define this, the default is one word.  */
#define INT_TYPE_SIZE 32

/* A C expression for the size in bits of the type `short' on the target
   machine.  If you don't define this, the default is half a word.  (If this
   would be less than one storage unit, it is rounded up to one unit.)  */
#define SHORT_TYPE_SIZE 16

/* A C expression for the size in bits of the type `long' on the target
   machine.  If you don't define this, the default is one word.  */
#define LONG_TYPE_SIZE 32

/* A C expression for the size in bits of the type `long long' on the target
   machine.  If you don't define this, the default is two words.  If you want
   to support GNU Ada on your machine, the value of macro must be at least 64.  */
#define LONG_LONG_TYPE_SIZE 64

/* A C expression for the size in bits of the type `char' on the target
   machine.  If you don't define this, the default is one quarter of a word.
   (If this would be less than one storage unit, it is rounded up to one unit.)  */
#define CHAR_TYPE_SIZE 8

/* A C expression for the size in bits of the type `float' on the target
   machine.  If you don't define this, the default is one word.  */
#define FLOAT_TYPE_SIZE 32

/* A C expression for the size in bits of the type `double' on the target
   machine.  If you don't define this, the default is two words.  */
#define DOUBLE_TYPE_SIZE 64

/* A C expression for the size in bits of the type `long double' on the target
   machine.  If you don't define this, the default is two words.  */
#define LONG_DOUBLE_TYPE_SIZE 64

/* An expression whose value is 1 or 0, according to whether the type `char'
   should be signed or unsigned by default.  The user can always override this
   default with the options `-fsigned-char' and `-funsigned-char'.  */
#define DEFAULT_SIGNED_CHAR 1

/* A C expression for a string describing the name of the data type to use for
   size values.  The typedef name `size_t' is defined using the contents of the
   string.  */
#define SIZE_TYPE "unsigned int"

/* A C expression for a string describing the name of the data type to use for
   the result of subtracting two pointers.  The typedef name `ptrdiff_t' is
   defined using the contents of the string.  See `SIZE_TYPE' above for more
   information.  */
#define PTRDIFF_TYPE "int"

/* A C expression for a string describing the name of the data type to use for
   wide characters.  The typedef name `wchar_t' is defined using the contents
   of the string.  See `SIZE_TYPE' above for more information.  */
/* #define WCHAR_TYPE */

/* A C expression for the size in bits of the data type for wide characters.
   This is used in `cpp', which cannot make use of `WCHAR_TYPE'.  */
/* #define WCHAR_TYPE_SIZE */

/* Maximum number for the size in bits of the data type for wide characters.
   If this is undefined, the default is `WCHAR_TYPE_SIZE'.  Otherwise, it is
   the constant value that is the largest value that `WCHAR_TYPE_SIZE' can have
   at run-time.  This is used in `cpp'.  */
/* #define MAX_WCHAR_TYPE_SIZE */

/* ********** Register Usage ********** */

/* Number of hardware registers known to the compiler.
   We have 64 general registers, 8 branch registers,
   and program counter register.  */
#define FIRST_PSEUDO_REGISTER 74

#define LAST_GR_REGNUM        63

#define GR_REGNO_P(REGNO)   ((REGNO) >= 0 &&  (REGNO) <= 62)
#define LINK_REGNO_P(REGNO) ((REGNO) == RETURN_POINTER_REGNUM)
#define BR_REGNO_P(REGNO)   ((REGNO) >= 64 && (REGNO) <= 71)


#define GENERAL_REGNO_P(REGNO)      \
  (GR_REGNO_P (REGNO) ||            \
   REGNO == FRAME_POINTER_REGNUM || \
   REGNO == ARG_POINTER_REGNUM)

/* An initializer that says which registers are used for fixed purposes all
   throughout the compiled code and are therefore not available for general
   allocation.

   r0: constant 0
   r12: stack pointer (sp)
   r63: link pointer (rp)
*/
#define FIXED_REGISTERS \
{ /* General registers.  */				\
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,	\
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	\
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	\
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	\
 /* Branch registers.  */				\
  0, 0, 0, 0, 0, 0, 0, 0,                               \
 /* virtual registers. */				\
  1, 1                                                  \
}

/* Like `FIXED_REGISTERS' but has 1 for each register that is clobbered (in
   general) by function calls as well as for fixed registers.  This macro
   therefore identifies the registers that are not available for general
   allocation of values that must live across function calls.  */
#if 1 
/* New ABI. */
/* [CG] 20040817: changed r13/r14 to be not CALL USED to conform
   new ABI. Noit doing this introduces spurious warnings when
   used as harwired register for variables, for instance in:
   register void *thread_ptr asm("r13"); 
   the front-end emits:
   warning: call-clobbered register used for global register variable.
*/
#define CALL_USED_REGISTERS \
{ /* General registers.  */				\
  1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1,	\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,	\
 /* Branch registers.  */				\
  1, 1, 1, 1, 1, 1, 1, 1,                               \
 /* virtual registers. */				\
  1, 1                                                  \
}
#else
#define CALL_USED_REGISTERS \
{ /* General registers.  */				\
  1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,	\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,	\
 /* Branch registers.  */				\
  1, 1, 1, 1, 1, 1, 1, 1,                               \
 /* virtual registers. */				\
  1, 1                                                  \
}
#endif

/* Replaces `FIXED_REGISTERS' when TARGET_KERNEL_MODE, since more regs
   are fixed.  */
#define KERNEL_FIXED_REGISTERS                          \
{ /* General registers.  */				\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,	\
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,	\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,	\
 /* Branch registers.  */				\
  0, 0, 0, 0, 0, 0, 0, 0,                               \
 /* virtual registers. */				\
  1, 1                                                  \
}

/* If TARGET_KERNEL_MODE, then more regs are fixed.  */
/* May be rewritten taking in account command line options '-ffixed-', ... */
#define CONDITIONAL_REGISTER_USAGE			      \
{							      \
  static char kernel_fixed_regs[] = KERNEL_FIXED_REGISTERS;   \
  if (TARGET_KERNEL_MODE)                                     \
    bcopy (kernel_fixed_regs, fixed_regs, sizeof fixed_regs); \
}

/* A C initializer containing the assembler's names for the machine registers,
   each one as a C string constant.  */
#define REGISTER_NAMES \
{							      \
  /* General registers.  */				      \
   ".0",  ".1",  ".2",  ".3",  ".4",  ".5",  ".6",  ".7",     \
   ".8",  ".9", ".10", ".11", ".12", ".13", ".14", ".15",     \
  ".16", ".17", ".18", ".19", ".20", ".21", ".22", ".23",     \
  ".24", ".25", ".26", ".27", ".28", ".29", ".30", ".31",     \
  ".32", ".33", ".34", ".35", ".36", ".37", ".38", ".39",     \
  ".40", ".41", ".42", ".43", ".44", ".45", ".46", ".47",     \
  ".48", ".49", ".50", ".51", ".52", ".53", ".54", ".55",     \
  ".56", ".57", ".58", ".59", ".60", ".61", ".62", ".63",     \
   ".0",  ".1",  ".2",  ".3",  ".4",  ".5",  ".6",  ".7",     \
  ".FP", ".AP",                                               \
}

/* Define additional names for use in asm clobbers and asm declarations.  */
#define ADDITIONAL_REGISTER_NAMES  \
{ {"r0", 0},   {"r1", 1},   {"r2", 2},   {"r3", 3},           \
  {"r4", 4},   {"r5", 5},   {"r6", 6},   {"r7", 7},           \
  {"r8", 8},   {"r9", 9},   {"r10", 10}, {"r11", 11},         \
  {"r12", 12}, {"r13", 13}, {"r14", 14}, {"r15", 15},         \
  {"r16", 16}, {"r17", 17}, {"r18", 18}, {"r19", 19},         \
  {"r20", 20}, {"r21", 21}, {"r22", 22}, {"r23", 23},         \
  {"r24", 24}, {"r25", 25}, {"r26", 26}, {"r27", 27},         \
  {"r28", 28}, {"r29", 29}, {"r30", 30}, {"r31", 31},         \
  {"r32", 32}, {"r33", 33}, {"r34", 34}, {"r35", 35},         \
  {"r36", 36}, {"r37", 37}, {"r38", 38}, {"r39", 39},         \
  {"r40", 40}, {"r41", 41}, {"r42", 42}, {"r43", 43},         \
  {"r44", 44}, {"r45", 45}, {"r46", 46}, {"r47", 47},         \
  {"r48", 48}, {"r49", 49}, {"r50", 50}, {"r51", 51},         \
  {"r52", 52}, {"r53", 53}, {"r54", 54}, {"r55", 55},         \
  {"r56", 56}, {"r57", 57}, {"r58", 58}, {"r59", 59},         \
  {"r60", 60}, {"r61", 61}, {"r62", 62}, {"r63", 63},         \
  {"b0",  64}, {"b1",  65}, {"b2",  66}, {"b3",  67},         \
  {"b4",  68}, {"b5",  69}, {"b6",  70}, {"b7",  71},         \
  { "sp", STACK_POINTER_REGNUM},                              \
  { "lp", RETURN_POINTER_REGNUM} }

#define MAX_ARGUMENT_SLOTS 8

/* REG_ALLOC_ORDER is to order in which to allocate registers.
   It's better to allocate scratch register first, so we don't need
   to save */
#define REG_ALLOC_ORDER						       \
{  8,  9, 10, 11, 13, 14, 15, 16,  1,  2,  3,  4,  5,  6,  7, 17,      \
  18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,      \
  34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,      \
  50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 12,  0,      \
  64, 65, 66, 67, 68, 69, 70, 71                                       \
}

/* A C expression for the number of consecutive hard registers, starting at
   register number REGNO, required to hold a value of mode MODE.  */
#define HARD_REGNO_NREGS(REGNO, MODE)                         \
   ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* Value is 1 if hard register REGNO can hold a value of machine-mode MODE. */
#if 0
#define HARD_REGNO_MODE_OK(REGNO, MODE)					\
   ( (GR_REGNO_P (REGNO)  && (GET_MODE_CLASS (MODE) == MODE_CC)) ? 0	\
   : (LINK_REGNO_P(REGNO) && (GET_MODE_CLASS (MODE) == MODE_CC)) ? 0	\
   : (BR_REGNO_P (REGNO) ? (GET_MODE_CLASS (MODE) == MODE_CC)		\
   : 1))
#else
#define HARD_REGNO_MODE_OK(REGNO, MODE)					\
   ( (GR_REGNO_P (REGNO)  && (GET_MODE_CLASS (MODE) == MODE_CC) && !reload_in_progress) ? 0	\
   : (LINK_REGNO_P(REGNO) && (GET_MODE_CLASS (MODE) == MODE_CC)) ? 0	\
   : (BR_REGNO_P (REGNO) ? (GET_MODE_CLASS (MODE) == MODE_CC)		\
   : 1))
#endif

/* A C expression that is nonzero if it is desirable to choose register
   allocation so as to avoid move instructions between a value of mode MODE1
   and a value of mode MODE2.

   If `HARD_REGNO_MODE_OK (R, MODE1)' and `HARD_REGNO_MODE_OK (R, MODE2)' are
   ever different for any R, then `MODES_TIEABLE_P (MODE1, MODE2)' must be
   zero.  */
#define MODES_TIEABLE_P(MODE1, MODE2)  (MODE1 == MODE2)

/* Define this macro if the compiler should avoid copies to/from CCmode
   registers.  You should only define this macro if support fo copying to/from
   CCmode is incomplete.  */
#define AVOID_CCMODE_COPIES

/* A C initializer for a vector, indexed by hard register number, which
   contains 1 for a register that is allowable in a candidate for leaf function
   treatment.  */
/* *** investigate *** */
/* #define LEAF_REGISTERS */

/* A C expression whose value is the register number to which REGNO should be
   renumbered, when a function is treated as a leaf function.  */
/* #define LEAF_REG_REMAP(REGNO) */


/* Register Classes */

/* An enumeral type that must be defined with all the register class names as
   enumeral values.  `NO_REGS' must be first.  `ALL_REGS' must be the last
   register class, followed by one more enumeral value, `LIM_REG_CLASSES',
   which is not a register class but rather tells how many classes there
   are.  */
enum reg_class
{
  NO_REGS,
  LINK_REG,
  BR_REGS,
  GR_REGS,
  ALL_REGS,
  LIM_REG_CLASSES
};

extern int lx_must_pass_in_stack ();
extern int lx_direct_return ();
extern struct rtx_def * lx_function_arg ();
extern int lx_function_arg_partial_nregs ();
extern void lx_function_prologue ();
extern void lx_function_epilogue ();
extern void lx_expand_prologue ();
extern void lx_expand_epilogue ();
extern void lx_function_arg_advance ();
extern void lx_print_operand ();
extern int lx_long_imm_operand ();
extern int lx_long_add_operand ();
extern enum reg_class lx_secondary_reload_class ();
extern int lx_elimination_offset ();


#define GENERAL_REGS GR_REGS

/* The number of distinct register classes.  */
#define N_REG_CLASSES ((int) LIM_REG_CLASSES)

/* Give names of register classes as strings for dump file.   */
#define REG_CLASS_NAMES   \
{                         \
  "NO_REGS",              \
  "LINK_REG",             \
  "BR_REGS",              \
  "GR_REGS",              \
  "ALL_REGS"              \
}

/* An initializer containing the contents of the register classes, as integers
   which are bit masks.  The Nth integer specifies the contents of class N.
   The way the integer MASK is interpreted is that register R is in the class
   if `MASK & (1 << R)' is 1.  */
#define REG_CLASS_CONTENTS                           \
{ 						     \
  { 0x00000000, 0x00000000, 0x00 },  /* NO_REGS  */  \
  { 0x00000000, 0x80000000, 0x00 },  /* LINK_REG */  \
  { 0x00000000, 0x00000000, 0xFF },  /* BR_REGS  */  \
  { 0xFFFFFFFF, 0x7FFFFFFF, 0x00 },  /* GR_REGS  */  \
  { 0xFFFFFFFF, 0xFFFFFFFF, 0xFF }   /* ALL_REGS */  \
}

/* A C expression whose value is a register class containing hard register
   REGNO.  In general there is more than one such class; choose a class which
   is "minimal", meaning that no smaller class also contains the register.  */
/* The NO_REGS case is primarily for the benefit of rws_access_reg, which
   may call here with private (invalid) register numbers, such as
   REG_VOLATILE.  */
#define REGNO_REG_CLASS(REGNO)          \
(  LINK_REGNO_P (REGNO)    ? LINK_REG	\
 : GENERAL_REGNO_P (REGNO) ? GR_REGS	\
 : BR_REGNO_P (REGNO)      ? BR_REGS	\
 : NO_REGS)

/* A macro whose definition is the name of the class to which a valid base
   register must belong.  A base register is one used in an address which is
   the register value plus a displacement.  */
#define BASE_REG_CLASS GENERAL_REGS

/* A macro whose definition is the name of the class to which a valid index
   register must belong.  An index register is one used in an address where its
   value is either multiplied by a scale factor or added to another register
   (as well as added to a displacement).  */
#define INDEX_REG_CLASS GENERAL_REGS

/* A C expression which defines the machine-dependent operand constraint
   letters for register classes.  If CHAR is such a letter, the value should be
   the register class corresponding to it.  Otherwise, the value should be
   `NO_REGS'.  The register letter `r', corresponding to class `GENERAL_REGS',
   will not be passed to this macro; you do not need to handle it.  */

#define REG_CLASS_FROM_LETTER(CHAR) \
(  (CHAR) == 'b' ? BR_REGS	    \
 : (CHAR) == 'l' ? LINK_REG	    \
 : NO_REGS)

/* A C expression which is nonzero if register number NUM is suitable for use
   as a base register in operand addresses.  It may be either a suitable hard
   register or a pseudo register that has been allocated such a hard reg.  */
#define REGNO_OK_FOR_BASE_P(REGNO) \
  (GENERAL_REGNO_P (REGNO) || GENERAL_REGNO_P (reg_renumber[REGNO]))

/* A C expression which is nonzero if register number NUM is suitable for use
   as an index register in operand addresses.  It may be either a suitable hard
   register or a pseudo register that has been allocated such a hard reg.  */
#define REGNO_OK_FOR_INDEX_P(REGNO) \
  (GENERAL_REGNO_P (REGNO) || GENERAL_REGNO_P (reg_renumber[REGNO]))

/* A C expression that places additional restrictions on the register class to
   use when it is necessary to copy value X into a register in class CLASS.
   The value is a register class; perhaps CLASS, or perhaps another, smaller
   class.  */
#define PREFERRED_RELOAD_CLASS(X, CLASS) CLASS

/* You should define this macro to indicate to the reload phase that it may
   need to allocate at least one register for a reload in addition to the
   register to contain the data.  Specifically, if copying X to a register
   CLASS in MODE requires an intermediate register, you should define this
   to return the largest register class all of whose registers can be used
   as intermediate registers or scratch registers.  */
#define SECONDARY_RELOAD_CLASS(CLASS, MODE, X) \
 lx_secondary_reload_class (CLASS, MODE, X)

/* Certain machines have the property that some registers cannot be copied to
   some other registers without using memory.  Define this macro on those
   machines to be a C expression that is non-zero if objects of mode M in
   registers of CLASS1 can only be copied to registers of class CLASS2 by
   storing a register of CLASS1 into memory and loading that memory location
   into a register of CLASS2.  */
/* #define SECONDARY_MEMORY_NEEDED(CLASS1, CLASS2, M) */

/* When SMALL_REGISTER_CLASSES is nonzero, the compiler allows
   registers explicitly used in the rtl to be used as spill registers
   but prevents the compiler from extending the lifetime of these
   registers. */
/* #define SMALL_REGISTER_CLASSES 1 */

/* Return the maximum number of consecutive registers needed to represent
   mode MODE in a register of class CLASS. */
#define CLASS_MAX_NREGS(CLASS, MODE)                                \
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* A C expression that defines the optional machine-dependent constraint
   letters (`I', `J', `K', `...', `P') that specify particular ranges
   of integer values */
#define CONST_OK_FOR_LETTER_P(VALUE, C)  0

/* Similar, but for floating constants, and defining letters G and H. */
#define CONST_DOUBLE_OK_FOR_LETTER_P(VALUE, C)  0

/* A C expression that defines the optional machine-dependent constraint
   letters (`Q', `R', `S', `T', `U') that can be used to segregate specific
   types of operands, usually memory references, for the target machine.  */
#define EXTRA_CONSTRAINT(VALUE, C)  0


/* Stack Layout */

/* Define this macro if pushing a word onto the stack moves the stack pointer
   to a smaller address.  */
#define STACK_GROWS_DOWNWARD 1

/* Define this macro if the addresses of local variable slots are at negative
   offsets from the frame pointer. */
#define FRAME_GROWS_DOWNWARD 1

/* Offset from the frame pointer to the first local variable slot to be
   allocated.  */
#define STARTING_FRAME_OFFSET 0

/* Offset from the stack pointer register to the first location at which
   outgoing arguments are placed. */
#define STACK_POINTER_OFFSET  16

/* Offset from the argument pointer register to the first argument's address.
   On some machines it may depend on the data type of the function.  */
#define FIRST_PARM_OFFSET(FUNDECL) 0

/* A C expression whose value is RTL representing the value of the return
   address for the frame COUNT steps up from the current frame, after the
   prologue.  */
#define RETURN_ADDR_RTX(COUNT, FRAMEADDR)       \
  ((count == 0)					\
   ? gen_rtx_REG (Pmode, RETURN_POINTER_REGNUM)	\
   : (rtx) 0)

/* A C expression whose value is an integer giving the offset, in bytes, from
   the value of the stack pointer register to the top of the stack frame at the
   beginning of any function, before the prologue.  The top of the frame is
   defined to be the value of the stack pointer in the previous frame, just
   before the call instruction.  */
#define INCOMING_FRAME_SP_OFFSET 16

#define GP_ARG_REGNUM          16
#define RETURN_POINTER_REGNUM  63

/* Register to use for pushing function arguments.  */
#define STACK_POINTER_REGNUM   12

/* Base register for access to local variables of the function.  */
#define FRAME_POINTER_REGNUM   72

/* Base register for access to arguments of the function.  */
#define ARG_POINTER_REGNUM     73

/* On some machines the offset between the frame pointer and starting
   offset of the automatic variables is not known until after register
   allocation has been done (for example, because the saved registers
   are between these two locations).  On those machines, define
   `FRAME_POINTER_REGNUM' the number of a special, fixed register to
   be used internally until the offset is known, and define
   `HARD_FRAME_POINTER_REGNUM' to be actual the hard register number
   used for the frame pointer.

   You should define this macro only in the very rare circumstances
   when it is not possible to calculate the offset between the frame
   pointer and the automatic variables until after register
   allocation has been completed.  When this macro is defined, you
   must also indicate in your definition of `ELIMINABLE_REGS' how to
   eliminate `FRAME_POINTER_REGNUM' into either
   `HARD_FRAME_POINTER_REGNUM' or `STACK_POINTER_REGNUM'.

   Do not define this macro if it would be the same as
   `FRAME_POINTER_REGNUM'. */
#undef  HARD_FRAME_POINTER_REGNUM
#define HARD_FRAME_POINTER_REGNUM 14

/* A C expression which is nonzero if a function must have and use a frame
   pointer.  This expression is evaluated in the reload pass.  If its value is
   nonzero the function will have a frame pointer.  */
#define FRAME_POINTER_REQUIRED 0

/* If defined, this macro specifies a table of register pairs used to eliminate
   unneeded registers that point into the stack frame.  */
#define ELIMINABLE_REGS				        \
{{ FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM },	\
 { ARG_POINTER_REGNUM,	 STACK_POINTER_REGNUM },	\
 { FRAME_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM },	\
 { ARG_POINTER_REGNUM,   HARD_FRAME_POINTER_REGNUM }}			

/* A C expression that returns non-zero if the compiler is allowed to try to
   replace register number FROM with register number TO. */
/* Should be reworked */
#define CAN_ELIMINATE(FROM, TO)  1

/* This macro is similar to `INITIAL_FRAME_POINTER_OFFSET'.  It specifies the
   initial difference between the specified pair of registers.  This macro must
   be defined if `ELIMINABLE_REGS' is defined.  */
#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)                    \
  OFFSET = lx_elimination_offset (FROM, TO)


/* Passing Function Arguments on the Stack */

/* Define this macro if an argument declared in a prototype as an integral type
   smaller than `int' should actually be passed as an `int'.  In addition to
   avoiding errors in certain cases of mismatch, it also makes for better code
   on certain machines.  */
#define PROMOTE_PROTOTYPES 1

/* If defined, the maximum amount of space required for outgoing arguments will
   be computed and placed into the variable
   `current_function_outgoing_args_size'.  */
#define ACCUMULATE_OUTGOING_ARGS 1

/* A C expression that should indicate the number of bytes of its own arguments
   that a function pops on returning, or 0 if the function pops no arguments
   and the caller must therefore pop them all after the function returns.  */
#define RETURN_POPS_ARGS(FUNDECL, FUNTYPE, STACK_SIZE) 0


/* Passing Arguments in Registers */

/* A C expression that controls whether a function argument is passed in a
   register, and which register.  */
#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) \
  lx_function_arg (&CUM, MODE, TYPE, NAMED)

/* For an arg passed partly in registers and partly in memory,
   this is the number of registers used.
   For args passed entirely in registers or entirely in memory, zero.  */

#define FUNCTION_ARG_PARTIAL_NREGS(CUM, MODE, TYPE, NAMED)      \
  lx_function_arg_partial_nregs (CUM, MODE, TYPE, NAMED)

/* Nonzero if we do not know how to pass TYPE solely in registers. */
#define MUST_PASS_IN_STACK(MODE,TYPE) lx_must_pass_in_stack (MODE, TYPE)

/* A C expression that indicates when an argument must be passed by reference.
   If nonzero for an argument, a copy of that argument is made in memory and a
   pointer to the argument is passed instead of the argument itself.  The
   pointer is passed in whatever way is appropriate for passing a pointer to
   that type.  */
#define FUNCTION_ARG_PASS_BY_REFERENCE(CUM, MODE, TYPE, NAMED) 0

/* A C type for declaring a variable that is used as the first argument of
   `FUNCTION_ARG' and other related values.  For some target machines, the type
   `int' suffices and can hold the number of bytes of argument so far.  */
#define CUMULATIVE_ARGS  unsigned int

/* A C statement (sans semicolon) for initializing the variable CUM for the
   state at the beginning of the argument list.  */
#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, INDIRECT)  (CUM) = 0

/* A C statement (sans semicolon) to update the summarizer variable CUM to
   advance past an argument in the argument list.  The values MODE, TYPE and
   NAMED describe that argument.  Once this is done, the variable CUM is
   suitable for analyzing the *following* argument with `FUNCTION_ARG'.  */
#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED) \
 lx_function_arg_advance (&CUM, MODE, TYPE, NAMED)

/* If defined, a C expression that gives the alignment boundary, in bits, of an
   argument with the specified mode and type. 
#define FUNCTION_ARG_BOUNDARY(MODE, TYPE) \
  (((((MODE) == BLKmode ? int_size_in_bytes (TYPE) : GET_MODE_SIZE (MODE)) \
     + UNITS_PER_WORD - 1) / UNITS_PER_WORD) > 1 ? 64 : PARM_BOUNDARY)
 */
#define FUNCTION_ARG_BOUNDARY(MODE, TYPE) \
  (((((MODE) == BLKmode ? (int_size_in_bytes (TYPE) * BITS_PER_UNIT)     \
     : GET_MODE_SIZE (MODE)) + UNITS_PER_WORD - 1) / UNITS_PER_WORD) > 1 \
     ? 64 : PARM_BOUNDARY)

/* A C expression that is nonzero if REGNO is the number of a hard register in
   which function arguments are sometimes passed.  This does *not* include
   implicit arguments such as the static chain and the structure-value address.
  must_pass_in_stack On many machines, no registers can be used for this purpose since all
   function arguments are pushed on the stack.
 */
#define FUNCTION_ARG_REGNO_P(REGNO)                        \
         ((REGNO >= GP_ARG_REGNUM &&                       \
	   (REGNO) < GP_ARG_REGNUM + MAX_ARGUMENT_SLOTS))


/* How Scalar Function Values Are Returned */

/* A C expression to create an RTX representing the place where a function
   returns a value of data type VALTYPE.  */
#define FUNCTION_VALUE(VALTYPE, FUNC) \
        gen_rtx (REG, TYPE_MODE(VALTYPE), GP_ARG_REGNUM)

/* A C expression to create an RTX representing the place where a library
   function returns a value of mode MODE.  */
#define LIBCALL_VALUE(MODE) \
        gen_rtx (REG, MODE, GP_ARG_REGNUM)

/* A C expression that is nonzero if REGNO is the number of a hard register in
   which the values of called function may come back.  */
#define FUNCTION_VALUE_REGNO_P(REGNO) ((REGNO) == GP_ARG_REGNUM)


/* How Large Values Are Returned */

/* A nonzero value says to return the function value in memory, just as large
   structures are always returned.  */
#define RETURN_IN_MEMORY(TYPE) \
  (TYPE_MODE (TYPE) == BLKmode && int_size_in_bytes(TYPE) > 32)

/* If you define this macro to be 0, then the conventions used for structure
   and union return values are decided by the `RETURN_IN_MEMORY' macro.  */
#define DEFAULT_PCC_STRUCT_RETURN 0

/* If the structure value address is passed in a register, then
   `STRUCT_VALUE_REGNUM' should be the number of that register.  */
#define STRUCT_VALUE_REGNUM 0


/* Caller-Saves Register Allocation */


/* Function Entry and Exit */

/* A C compound statement that outputs the assembler code for entry to a
   function.  */
#define FUNCTION_PROLOGUE(FILE, SIZE) \
  lx_function_prologue (FILE, SIZE)

/* Define this macro as a C expression that is nonzero if the return
   instruction or the function epilogue ignores the value of the stack pointer;
   in other words, if it is safe to delete an instruction to adjust the stack
   pointer before a return from the function.  */
#define EXIT_IGNORE_STACK 1

/* Define this macro as a C expression that is nonzero for registers
   used by the epilogue or the `return' pattern.  */
/* #define EPILOGUE_USES(REGNO) 0 */

/* A C compound statement that outputs the assembler code for exit from a
   function.  */
#define FUNCTION_EPILOGUE(FILE, SIZE) \
   lx_function_epilogue (FILE, SIZE)


/* Generating Code for Profiling  */

/* A C statement or compound statement to output to FILE some assembler code to
   call the profiling subroutine `mcount'.  */
#define FUNCTION_PROFILER(FILE, LABELNO) abort()


/* Implementing the Varargs Macros.  */

/* Define the `__builtin_va_list' type for the ABI. */
#define BUILD_VA_LIST_TYPE(VALIST) \
  (VALIST) = lx_build_va_list ()

extern struct rtx_def *lx_builtin_saveregs ();
#define EXPAND_BUILTIN_SAVEREGS() lx_builtin_saveregs ()

#define EXPAND_BUILTIN_VA_START(valist, nextarg) \
  lx_va_start (valist, nextarg)

/* Define this macro to store the anonymous register arguments into the stack
   so that all the arguments appear to have been passed consecutively on the
   stack.
#define SETUP_INCOMING_VARARGS(ARGS_SO_FAR, MODE, TYPE, PRETEND_ARGS_SIZE, NO_RTL) \
 lxr_setup_incoming_varargs (&ARGS_SO_FAR, MODE, TYPE, &PRETEND_SIZE, NO_RTL)
*/

/* Define this macro if the location where a function argument is passed
   depends on whether or not it is a named argument.
#define STRICT_ARGUMENT_NAMING  1
 */


/* Trampolines for Nested Functions.  */

/** Output assembler code for a block containing the constant parts
    of a trampoline, leaving space for the variable parts.*/
#undef TRAMPOLINE_TEMPLATE

/* Length in units of the trampoline for entering a nested function.  */
#define TRAMPOLINE_SIZE         28

/* Alignment required for a trampoline in bits .  */
#define TRAMPOLINE_ALIGNMENT     8

/* A C statement to initialize the variable parts of a trampoline.
   ADDR is an RTX for the address of the trampoline; FNADDR is an
   RTX for the address of the nested function; CXT is an
   RTX for the static chain value that should be passed to the
   function when it is called. */
extern void lx_initialize_trampoline (rtx, rtx, rtx);
#define INITIALIZE_TRAMPOLINE(TRAMP, FNADDR, CXT) \
  lx_initialize_trampoline ((TRAMP), (FNADDR), (CXT));


/* Implicit Calls to Library Routines */


/* Addressing Modes */

/* 1 if X is an rtx for a constant that is a valid address.  */
#define CONSTANT_ADDRESS_P(X)  (CONSTANT_P (X))

/* The max number of registers that can appear in a valid memory address.  */
#define MAX_REGS_PER_ADDRESS 1

#define REGISTER_ADDRESS_P(X) \
   (REG_P (X) && REG_OK_FOR_BASE_P (X))

#if !defined(MEM_P)
#define MEM_P(Op) (GET_CODE(Op) == MEM)
#endif

#define OFFSET_P(x)  (CONSTANT_P(x))

/* A C compound statement with a conditional `goto LABEL;' executed if X (an
   RTX) is a legitimate memory address on the target machine for a memory
   operand of mode MODE.  */
#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, LABEL)  \
do {					          \
  if (GET_CODE (X) == PLUS &&                     \
      REGISTER_ADDRESS_P (XEXP (X, 0)) &&         \
      OFFSET_P (XEXP (X, 1)))                     \
    goto LABEL;			                  \
  else if (GET_CODE (X) == PLUS &&                \
      REGISTER_ADDRESS_P (XEXP (X, 1)) &&         \
      OFFSET_P (XEXP (X, 0)))                     \
    goto LABEL;					  \
  else if (REGISTER_ADDRESS_P (X))                \
    goto LABEL;                                   \
  else if (CONSTANT_P (X))                        \
    goto LABEL;					  \
} while (0)

/* A C expression that is nonzero if X (assumed to be a `reg' RTX) is valid for
   use as a base register.  */
#ifdef REG_OK_STRICT
#define REG_OK_FOR_BASE_P(X) REGNO_OK_FOR_BASE_P (REGNO (X))
#else
#define REG_OK_FOR_BASE_P(X) \
  (GENERAL_REGNO_P (REGNO (X)) || (REGNO (X) >= FIRST_PSEUDO_REGISTER))
#endif

/* A C expression that is nonzero if X (assumed to be a `reg' RTX) is valid for
   use as an index register.  */
#define REG_OK_FOR_INDEX_P(regno) 0

/* A C compound statement that attempts to replace X with a valid memory
   address for an operand of mode MODE.*/
#define LEGITIMIZE_ADDRESS(X, OLDX, MODE, WIN)

/* A C statement or compound statement with a conditional `goto
   LABEL;' executed if memory address X (an RTX) can have different
   meanings depending on the machine mode of the memory reference it
   is used for.*/
#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR,LABEL) {}

/* A C expression that is nonzero if X is a legitimate constant for an
   immediate operand on the target machine.  */

#define LEGITIMATE_CONSTANT_P(X) 1


/* Condition Code Status */

/* Returns a mode from class MODE_CC to be used when comparison operation code
   OP is applied to rtx X and Y. */
#define SELECT_CC_MODE(OP,X,Y) CCmode

/* A C expression whose value is one if it is always safe to reverse a
   comparison whose mode is MODE. */
#define REVERSIBLE_CC_MODE(MODE) 1


/* Describing Relative Costs of Operations */

#define CONST_COSTS(RTX,CODE,OUTER_CODE)  \
  case CONST_INT:                                               \
    return (INTVAL(RTX) >= -256 && INTVAL(RTX) <= 255) ? 0 : 2; \
  case CONST:                                                   \
  case LABEL_REF:                                               \
  case SYMBOL_REF:                                              \
    return 2;                                                   \
  case CONST_DOUBLE:                                            \
    return 4;

#define REGISTER_MOVE_COST(MODE,FROM, TO)  \
   ((FROM) == GENERAL_REGS || (TO) == GENERAL_REGS ? 2 : 4)

#define MEMORY_MOVE_COST(MODE,CLASS,TO_P)  6

#define BRANCH_COST 3

/* Define this macro as a C expression which is nonzero if accessing less than
   a word of memory (i.e. a `char' or a `short') is no faster than accessing a
   word of memory.  */
#define SLOW_BYTE_ACCESS 1

/* Define this macro if it is as good or better to call a constant function
   address than to call an address kept in a register. */
#define NO_FUNCTION_CSE


/* Dividing the output into sections.  */

#define TEXT_SECTION_ASM_OP "\n.section\t.text"
#define DATA_SECTION_ASM_OP "\n.section\t.data"
#define BSS_SECTION_ASM_OP  "\n.section\t.bss"

/* Switch into a generic section.
   This is currently only used to support section attributes.
*/
#define ASM_OUTPUT_SECTION_NAME(FILE, DECL, NAME, RELOC) \
do {								\
  if ((DECL) && TREE_CODE (DECL) == FUNCTION_DECL)		\
    fprintf (FILE, "\t.section %s,\"ax\",@progbits\n", (NAME)); \
  else if ((DECL) && DECL_READONLY_SECTION (DECL, RELOC))	\
    fprintf (FILE, "\t.section %s,\"a\",@progbits\n", (NAME));	\
  else								\
    fprintf (FILE, "\t.section %s,\"aw\",@progbits\n", (NAME));	\
} while (0)

#define ASM_OUTPUT_INTERNAL_LABEL(FILE, PREFIX, NUM)		\
  do								\
    {								\
      fprintf (FILE, ".%s%u:\n", PREFIX, (unsigned) (NUM));	\
    }								\
  while (0)


#define ASM_GENERATE_INTERNAL_LABEL(LABEL, PREFIX, NUM)		\
  do								\
    {								\
      sprintf (LABEL, "*.%s%u", PREFIX, (unsigned) (NUM));	\
    }								\
  while (0)


#define SKIP_ASM_OP	"\t.zero\t"
#define ASM_OUTPUT_SKIP(FILE, SIZE) \
  fprintf (FILE, "%s%u\n", SKIP_ASM_OP, (SIZE))

/* Define this macro if jump tables (for `tablejump' insns) should be output in
   the text section, along with the assembler instructions.
#define JUMP_TABLES_IN_TEXT_SECTION 1
*/


/* Position Independent Code.  */


/* Output and Generation of Labels.  */

/* A C statement to ouput assembler commands which will identify the object
   file as having beeing compiler with GNU CC */
#define ASM_IDENTIFY_GCC(FILE)             \
do {					   \
        fputs ("gcc_compiled.:\n", FILE);  \
} while (0)

/* A C string constant describing how to begin a comment in the target
   assembler language.  The compiler assumes that the comment will end at the
   end of the line.  */
#define ASM_COMMENT_START "//"

/* Output the name of the file we are compiling.  */
#define ASM_OUTPUT_SOURCE_FILENAME(STREAM, NAME) \
  do {	fprintf (STREAM, "\t.file\t");			\
	output_quoted_string (STREAM, NAME);		\
	fprintf (STREAM, "\n");				\
  } while (0)

/* A C string constant for text to be output before each `asm' statement or
   group of consecutive ones.  */

/* ??? This won't work with the Intel assembler, because it does not accept
   # as a comment start character.  However, //APP does not work in gas, so we
   can't use that either.  Same problem for ASM_APP_OFF below.  */
#define ASM_APP_ON "#APP\n"

/* A C string constant for text to be output after each `asm' statement or
   group of consecutive ones.  */
#define ASM_APP_OFF "#NO_APP\n"

/* This is how to output an assembler line defining an `int' constant.  */
#define ASM_OUTPUT_INT(FILE,VALUE)      \
do {                                    \
  fputs("\t.data4\t", FILE);             \
  output_addr_const (FILE, (VALUE)),	\
  fprintf (FILE, "\n");                 \
} while(0)

/* Likewise for `char' and `short' constants.  */

#define ASM_OUTPUT_SHORT(FILE,VALUE)    \
do {                                    \
  fputs("\t.data2\t", FILE);             \
  output_addr_const (FILE, (VALUE)),	\
  fprintf (FILE, "\n");                 \
} while(0)

#define ASM_OUTPUT_CHAR(FILE,VALUE)     \
do {                                    \
  fputs("\t.data1\t", FILE);             \
  output_addr_const (FILE, (VALUE)),	\
  fprintf (FILE, "\n");                 \
} while(0)

/* A C statement to output to the stdio stream STREAM an assembler instruction
   to assemble a single byte containing the number VALUE.  */
#define ASM_OUTPUT_BYTE(FILE, VALUE) \
  fprintf (FILE, "\t.byte\t%d\n", (VALUE)); \

/* These macros are defined as C string constant, describing the syntax in the
   assembler for grouping arithmetic expressions.  */
#define ASM_OPEN_PAREN "("
#define ASM_CLOSE_PAREN ")"

/* A C statement (sans semicolon) to output to the stdio stream STREAM the
   assembler definition of a common-label named NAME whose size is SIZE
   bytes.  The variable ROUNDED is the size rounded up to whatever alignment
   the caller wants. */
#define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED)    \
do {                                                    \
        bss_section();                                  \
        fputs ("\t.comm\t", FILE);                      \
        assemble_name(FILE, NAME);                      \
        fprintf (FILE, ",%u\n", SIZE);                  \
} while (0)

/* This says how to output an assembler line to define a global common
   symbol. */
#define ASM_OUTPUT_LOCAL(FILE, NAME, SIZE,ROUNDED)	\
( fputs ("\t.lcomm ", (FILE)),				\
  assemble_name ((FILE), (NAME)),			\
  fprintf ((FILE), ",%d\n", (SIZE)))

/* A C statement (sans semicolon) to output to the stdio stream STREAM the
   assembler definition of a label named NAME.  */
#define ASM_OUTPUT_LABEL(FILE, NAME)			 \
  do {                                                   \
  assemble_name (FILE, NAME);                            \
  fputs (":\n", FILE);                                   \
} while (0)

/* Store in OUTPUT a string (made with alloca) containing
   an assembler-name for a local static variable named NAME.
   LABELNO is an integer which is different for each call.  
   Just return the name. the backend will do the renaming.
*/
#define ASM_FORMAT_PRIVATE_NAME(OUTPUT, NAME, LABELNO)	\
( (OUTPUT) = (char *) alloca (strlen ((NAME)) + 10),	\
  sprintf ((OUTPUT), "%s.%d", (NAME), (LABELNO)))

/* A C statement (sans semicolon) to output to the stdio stream STREAM some
   commands that will make the label NAME global; that is, available for
   reference from other files.  */
#define GLOBAL_ASM_OP "\t.globl "

#ifdef HOST_WORDS_BIG_ENDIAN
/* This is how to output an assembler line defining a `double' constant.  */
/* can we use .real8 ? */
#define ASM_OUTPUT_DOUBLE(FILE,VALUE)             \
do {                                              \
    long t[2];					  \
    REAL_VALUE_TO_TARGET_DOUBLE (VALUE, t);	  \
    fprintf (FILE, "\t.long 0x%x\n", t[0]);       \
    fprintf (FILE, "\t.long 0x%x\n", t[1]);       \
} while (0)
#else
#define ASM_OUTPUT_DOUBLE(FILE,VALUE)             \
do {                                              \
    long t[2];					  \
    REAL_VALUE_TO_TARGET_DOUBLE (VALUE, t);	  \
    fprintf (FILE, "\t.long 0x%x\n", t[1]);       \
    fprintf (FILE, "\t.long 0x%x\n", t[0]);       \
} while (0)
#endif

#define ASM_OUTPUT_FLOAT(FILE,VALUE)              \
do {                                              \
    long t;					  \
    REAL_VALUE_TO_TARGET_SINGLE (VALUE, t);	  \
    fprintf (FILE, "\t.long 0x%x\n", t);          \
} while (0)

/* If HAS_INIT_SECTION is defined, main will not call __main
   To be revisited when we generate g++ */
#define HAS_INIT_SECTION

/* A C compound statement to output to stdio stream STREAM the assembler syntax
   for an instruction operand X.  X is an RTL expression.  */
#define PRINT_OPERAND(FILE, X, CODE) \
    lx_print_operand (FILE, X, CODE)

/* A C expression which evaluates to true if CODE is a valid punctuation
   character for use in the `PRINT_OPERAND' macro.  */
/* ??? Keep this around for now, as we might need it later.  */
/* #define PRINT_OPERAND_PUNCT_VALID_P(CODE) */

/* A C compound statement to output to stdio stream STREAM the assembler syntax
   for an instruction operand that is a memory reference whose address is X.  X
   is an RTL expression.  */
#define PRINT_OPERAND_ADDRESS(STREAM, X) abort()

/* This is how to output an element of a case-vector that is relative. */
/* Check label generation in  ASM_OUTPUT_INTERNAL_LABEL */
#define ASM_OUTPUT_ADDR_DIFF_ELT(STREAM, BODY, VALUE, REL) \
  fprintf (STREAM, "\t.word LL%d-LL%d\n", VALUE, REL)

/* This is how to output an element of a case-vector that is absolute. */
/* Check label generation in  ASM_OUTPUT_INTERNAL_LABEL */
#define ASM_OUTPUT_ADDR_VEC_ELT(STREAM, VALUE) \
  fprintf (STREAM, "\t.word LL%d\n", VALUE)

/* This is how to output an assembler line that says to advance the
   location counter to a multiple of 2**LOG bytes. */
#define ASM_OUTPUT_ALIGN(FILE, LOG)  \
  fprintf (FILE, "\n\t.align %d\n", 1<<(LOG))


/* Controlling Debugging Information Format */

/* Use stabs debugging info by default.  */
#undef PREFERRED_DEBUGGING_TYPE
#define PREFERRED_DEBUGGING_TYPE DBX_DEBUG

/* Get the standard ELF stabs definitions.  */
#include "dbxelf.h"


/* Miscellaneous Parameters.  */

/* Define this if you have defined special-purpose predicates in the file
   `MACHINE.c'.  For each predicate, list all rtl codes that can be in
   expressions matched by the predicate.  */

#define PREDICATE_CODES                                                \
{ "lx_operand_long_imm",    { CONST_INT, CONST_DOUBLE, CONST,          \
                              SYMBOL_REF, LABEL_REF }},	               \
{ "lx_operand_long_add",    { REG, MEM}},                              \
/**/


/* An alias for a machine mode name.  This is the machine mode that elements of
   a jump-table should have.  */
#define CASE_VECTOR_MODE Pmode

/* Define as C expression which evaluates to nonzero if the tablejump
   instruction expects the table to contain offsets from the address of the
   table.  */
/* #define CASE_VECTOR_PC_RELATIVE 1 */

/* Define this macro if operations between registers with integral mode smaller
   than a word are always performed on the entire register.  */
#define WORD_REGISTER_OPERATIONS

/* An alias for a tree code that is the easiest kind of division to compile
   code for in the general case.  */
#define EASY_DIV_EXPR TRUNC_DIV_EXPR

/* The maximum number of bytes that a single instruction can move quickly from
   memory to memory.  */
#define MOVE_MAX 4

/* A C expression which is nonzero if on this machine it is safe to "convert"
   an integer of INPREC bits to one of OUTPREC bits (where OUTPREC is smaller
   than INPREC) by merely operating on it as if it had only OUTPREC bits.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

/* A C expression describing the value returned by a comparison operator with
   an integral mode and stored by a store-flag instruction (`sCOND') when the
   condition is true.  */
#define STORE_FLAG_VALUE 1

/* An alias for the machine mode for pointers.  */
#define Pmode SImode

/* An alias for the machine mode used for memory references to functions being
   called, in `call' RTL expressions.  */
#define FUNCTION_MODE Pmode

#undef EXTRA_SECTIONS

#undef EXTRA_SECTION_FUNCTIONS

#undef  MAKE_DECL_ONE_ONLY 
#define SUPPORTS_ONE_ONLY 1

#define SUPPORTS_INIT_PRIORITY 0

#define TARGET_64BIT 0

/* #define DEFAULT_USE_CXA_ATEXIT 1 */

/* The system headers under ST200 systems are generally C++-aware.  */
#define NO_IMPLICIT_EXTERN_C

  /* exceptions stuff */

#define TARGET_ASM_NAMED_SECTION  default_elf_asm_named_section

#if 0
#define DWARF2_UNWIND_INFO 1

#define EH_RETURN_STACKADJ_RTX	gen_rtx_REG (Pmode, 29)
#define EH_RETURN_HANDLER_RTX \
  gen_rtx_MEM (word_mode,						\
	       gen_rtx_PLUS (word_mode, frame_pointer_rtx, GEN_INT (-20)))
#endif

#define DWARF_FRAME_REGNUM(REG) (REG)

/* Describe how we implement __builtin_eh_return.  */
#define INCOMING_RETURN_ADDR_RTX gen_rtx_REG (Pmode, RETURN_POINTER_REGNUM)
#define DWARF_FRAME_RETURN_COLUMN  DWARF_FRAME_REGNUM (RETURN_POINTER_REGNUM)

#define EH_RETURN_DATA_REGNO(N)	((N) < 4 ? (N) + 8 : -1)
#define DWARF_FRAME_REGISTERS 63

/* assembly emission done by open64 code generator */
extern void default_lxopen64_assemble ();
#define TARGET_ASM_ASSEMBLE_VISIBILITY  default_lxopen64_assemble
