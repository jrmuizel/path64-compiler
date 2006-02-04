/* Generated automatically by the program `genattr'
   from the machine description file `md'.  */

#ifndef GCC_INSN_ATTR_H
#define GCC_INSN_ATTR_H

#define HAVE_ATTR_alternative
#define get_attr_alternative(insn) which_alternative
#define HAVE_ATTR_insn_type
enum attr_insn_type {INSN_TYPE_LOADW, INSN_TYPE_LOADNW, INSN_TYPE_STORE, INSN_TYPE_LOADLR, INSN_TYPE_MOV2LR, INSN_TYPE_ALU1, INSN_TYPE_ALU2, INSN_TYPE_ALU3, INSN_TYPE_MULT, INSN_TYPE_CMP2SLCT, INSN_TYPE_CMP2BR, INSN_TYPE_BRANCH, INSN_TYPE_JUMP, INSN_TYPE_CALL, INSN_TYPE_RETURN, INSN_TYPE_MISC, INSN_TYPE_BUNDLESTOP};
extern enum attr_insn_type get_attr_insn_type PARAMS ((rtx));

#define HAVE_ATTR_cpu
enum attr_cpu {CPU_ST210, CPU_ST220};
extern enum attr_cpu get_attr_cpu PARAMS ((void));

#define HAVE_ATTR_LongImm
enum attr_LongImm {LONGIMM_NO, LONGIMM_YES};
extern enum attr_LongImm get_attr_LongImm PARAMS ((rtx));

#define HAVE_ATTR_issue_type
enum attr_issue_type {ISSUE_TYPE_A, ISSUE_TYPE_B, ISSUE_TYPE_M, ISSUE_TYPE_U, ISSUE_TYPE_S};
extern enum attr_issue_type get_attr_issue_type PARAMS ((rtx));

#define HAVE_ATTR_Unit_type
enum attr_Unit_type {UNIT_TYPE_ALU_1, UNIT_TYPE_ALU_2, UNIT_TYPE_ALU_3, UNIT_TYPE_MUL_2, UNIT_TYPE_MUL_3, UNIT_TYPE_MEM_1, UNIT_TYPE_MEM_2, UNIT_TYPE_MEM_3, UNIT_TYPE_MEM_4, UNIT_TYPE_BRANCH};
extern enum attr_Unit_type get_attr_Unit_type PARAMS ((rtx));

#define HAVE_ATTR_length
extern int get_attr_length PARAMS ((rtx));
extern void shorten_branches PARAMS ((rtx));
extern int insn_default_length PARAMS ((rtx));
extern int insn_variable_length_p PARAMS ((rtx));
extern int insn_current_length PARAMS ((rtx));

#include "insn-addr.h"

#define HAVE_ATTR_delay_model
enum attr_delay_model {DELAY_MODEL_NOBUNDLE, DELAY_MODEL_BUNDLE, DELAY_MODEL_REORG_BUNDLE};
extern enum attr_delay_model get_attr_delay_model PARAMS ((void));

#define TRADITIONAL_PIPELINE_INTERFACE 1
#define INSN_SCHEDULING

extern int result_ready_cost PARAMS ((rtx));
extern int function_units_used PARAMS ((rtx));

extern const struct function_unit_desc
{
  const char *const name;
  const int bitmask;
  const int multiplicity;
  const int simultaneity;
  const int default_cost;
  const int max_issue_delay;
  int (*const ready_cost_function) PARAMS ((rtx));
  int (*const conflict_cost_function) PARAMS ((rtx, rtx));
  const int max_blockage;
  unsigned int (*const blockage_range_function) PARAMS ((rtx));
  int (*const blockage_function) PARAMS ((rtx, rtx));
} function_units[];

#define FUNCTION_UNITS_SIZE 12
#define MIN_MULTIPLICITY 1
#define MAX_MULTIPLICITY 4
#define MIN_SIMULTANEITY 0
#define MAX_SIMULTANEITY 0
#define MIN_READY_COST 1
#define MAX_READY_COST 16
#define MIN_ISSUE_DELAY 1
#define MAX_ISSUE_DELAY 4
#define MIN_BLOCKAGE 1
#define MAX_BLOCKAGE 13
#define BLOCKAGE_BITS 5
#define INSN_QUEUE_SIZE 32

/* DFA based pipeline interface.  */
#ifndef AUTOMATON_STATE_ALTS
#define AUTOMATON_STATE_ALTS 0
#endif

#ifndef CPU_UNITS_QUERY
#define CPU_UNITS_QUERY 0
#endif

extern int max_dfa_issue_rate;

/* The following macro value is calculated from the
   automaton based pipeline description and is equal to
   maximal number of all insns described in constructions
   `define_insn_reservation' which can be issued on the
   same processor cycle. */
#define MAX_DFA_ISSUE_RATE max_dfa_issue_rate

/* Insn latency time defined in define_insn_reservation. */
extern int insn_default_latency PARAMS ((rtx));

/* Return nonzero if there is a bypass for given insn
   which is a data producer.  */
extern int bypass_p PARAMS ((rtx));

/* Insn latency time on data consumed by the 2nd insn.
   Use the function if bypass_p returns nonzero for
   the 1st insn. */
extern int insn_latency PARAMS ((rtx, rtx));

/* The following function returns number of alternative
   reservations of given insn.  It may be used for better
   insns scheduling heuristics. */
extern int insn_alts PARAMS ((rtx));

/* Maximal possible number of insns waiting results being
   produced by insns whose execution is not finished. */
extern int max_insn_queue_index;

/* Pointer to data describing current state of DFA.  */
typedef void *state_t;

/* Size of the data in bytes.  */
extern int state_size PARAMS ((void));

/* Initiate given DFA state, i.e. Set up the state
   as all functional units were not reserved.  */
extern void state_reset PARAMS ((state_t));
/* The following function returns negative value if given
   insn can be issued in processor state described by given
   DFA state.  In this case, the DFA state is changed to
   reflect the current and future reservations by given
   insn.  Otherwise the function returns minimal time
   delay to issue the insn.  This delay may be zero
   for superscalar or VLIW processors.  If the second
   parameter is NULL the function changes given DFA state
   as new processor cycle started.  */
extern int state_transition PARAMS ((state_t, rtx));

#if AUTOMATON_STATE_ALTS
/* The following function returns number of possible
   alternative reservations of given insn in given
   DFA state.  It may be used for better insns scheduling
   heuristics.  By default the function is defined if
   macro AUTOMATON_STATE_ALTS is defined because its
   implementation may require much memory.  */
extern int state_alts PARAMS ((state_t, rtx));
#endif

extern int min_issue_delay PARAMS ((state_t, rtx));
/* The following function returns nonzero if no one insn
   can be issued in current DFA state. */
extern int state_dead_lock_p PARAMS ((state_t));
/* The function returns minimal delay of issue of the 2nd
   insn after issuing the 1st insn in given DFA state.
   The 1st insn should be issued in given state (i.e.
    state_transition should return negative value for
    the insn and the state).  Data dependencies between
    the insns are ignored by the function.  */
extern int min_insn_conflict_delay PARAMS ((state_t, rtx, rtx));
/* The following function outputs reservations for given
   insn as they are described in the corresponding
   define_insn_reservation.  */
extern void print_reservation PARAMS ((FILE *, rtx));

#if CPU_UNITS_QUERY
/* The following function returns code of functional unit
   with given name (see define_cpu_unit). */
extern int get_cpu_unit_code PARAMS ((const char *));
/* The following function returns nonzero if functional
   unit with given code is currently reserved in given
   DFA state.  */
extern int cpu_unit_reservation_p PARAMS ((state_t, int));
#endif

/* Initiate and finish work with DFA.  They should be
   called as the first and the last interface
   functions.  */
extern void dfa_start PARAMS ((void));
extern void dfa_finish PARAMS ((void));

#define ATTR_FLAG_forward	0x1
#define ATTR_FLAG_backward	0x2
#define ATTR_FLAG_likely	0x4
#define ATTR_FLAG_very_likely	0x8
#define ATTR_FLAG_unlikely	0x10
#define ATTR_FLAG_very_unlikely	0x20

#endif /* GCC_INSN_ATTR_H */
