/* Generated automatically by the program `genconditions' from the target
   machine description file.  */

#include "hconfig.h"
#include "insn-constants.h"

/* Do not allow checking to confuse the issue.  */
#undef ENABLE_CHECKING
#undef ENABLE_TREE_CHECKING
#undef ENABLE_RTL_CHECKING
#undef ENABLE_RTL_FLAG_CHECKING
#undef ENABLE_GC_CHECKING
#undef ENABLE_GC_ALWAYS_COLLECT

#include "system.h"
#include "rtl.h"
#include "tm_p.h"
#include "function.h"

/* Fake - insn-config.h doesn't exist yet.  */
#define MAX_RECOG_OPERANDS 10
#define MAX_DUP_OPERANDS 10
#define MAX_INSNS_PER_SPLIT 5

#include "regs.h"
#include "recog.h"
#include "real.h"
#include "output.h"
#include "flags.h"
#include "hard-reg-set.h"
#include "resource.h"
#include "toplev.h"
#include "reload.h"
#include "gensupport.h"

#include "except.h"

/* Dummy external declarations.  */
extern rtx insn;
extern rtx ins1;
extern rtx operands[];
extern int next_insn_tests_no_inequality PARAMS ((rtx));

/* If we don't have __builtin_constant_p, or it's not acceptable in
   array initializers, fall back to assuming that all conditions
   potentially vary at run time.  It works in 3.0.1 and later; 3.0
   only when not optimizing.  */
#if (GCC_VERSION >= 3001) || ((GCC_VERSION == 3000) && !__OPTIMIZE__)
# define MAYBE_EVAL(expr) (__builtin_constant_p(expr) ? (int) (expr) : -1)
#else
# define MAYBE_EVAL(expr) -1
#endif

/* This table lists each condition found in the machine description.
   Each condition is mapped to its truth value (0 or 1), or -1 if that
   cannot be calculated at compile time. */

const struct c_test insn_conditions[] = {
  { "reload_completed",
    MAYBE_EVAL (reload_completed) },
  { "lx_direct_return ()",
    MAYBE_EVAL (lx_direct_return ()) },
  { "(reload_completed)",
    MAYBE_EVAL ((reload_completed)) },
  { "reload_in_progress",
    MAYBE_EVAL (reload_in_progress) },
  { "(reload_completed == 0)",
    MAYBE_EVAL ((reload_completed == 0)) },
  { "(FIT_SI16 (INTVAL (operands [2])))",
    MAYBE_EVAL ((FIT_SI16 (INTVAL (operands [2])))) },
  { "(FIT_UI16 (INTVAL (operands [2])) ||\n\
    FIT_SI16 (INTVAL (operands [2])) ||\n\
    FIT_HI16 (INTVAL (operands [2])))",
    MAYBE_EVAL ((FIT_UI16 (INTVAL (operands [2])) ||
    FIT_SI16 (INTVAL (operands [2])) ||
    FIT_HI16 (INTVAL (operands [2])))) },
  { "((reload_completed == 0)\n\
    && (FIT_UI16 (INTVAL (operands [2])) ||\n\
        FIT_SI16 (INTVAL (operands [2])) ||\n\
        FIT_HI16 (INTVAL (operands [2]))))",
    MAYBE_EVAL (((reload_completed == 0)
    && (FIT_UI16 (INTVAL (operands [2])) ||
        FIT_SI16 (INTVAL (operands [2])) ||
        FIT_HI16 (INTVAL (operands [2]))))) },
  { "(optimize)",
    MAYBE_EVAL ((optimize)) },
  { "(FIT_UI16 (INTVAL (operands [2])))",
    MAYBE_EVAL ((FIT_UI16 (INTVAL (operands [2])))) },
};

const size_t n_insn_conditions = 10;
const int insn_elision_unavailable = 0;
