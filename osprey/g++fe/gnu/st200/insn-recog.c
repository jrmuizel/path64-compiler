/* Generated automatically by the program `genrecog' from the target
   machine description file.  */

#include "config.h"
#include "system.h"
#include "rtl.h"
#include "tm_p.h"
#include "function.h"
#include "insn-config.h"
#include "recog.h"
#include "real.h"
#include "output.h"
#include "flags.h"
#include "hard-reg-set.h"
#include "resource.h"



/* `recog' contains a decision tree that recognizes whether the rtx
   X0 is a valid instruction.

   recog returns -1 if the rtx is not valid.  If the rtx is valid, recog
   returns a nonnegative number which is the insn code number for the
   pattern that matched.  This is the same as the order in the machine
   description of the entry that matched.  This number can be used as an
   index into `insn_data' and other tables.

   The third argument to recog is an optional pointer to an int.  If
   present, recog will accept a pattern if it matches except for missing
   CLOBBER expressions at the end.  In that case, the value pointed to by
   the optional pointer will be set to the number of CLOBBERs that need
   to be added (it should be initialized to zero by the caller).  If it
   is set nonzero, the caller should allocate a PARALLEL of the
   appropriate size, copy the initial entries, and call add_clobbers
   (found in insn-emit.c) to fill in the CLOBBERs.


   The function split_insns returns 0 if the rtl could not
   be split or the split rtl in a SEQUENCE if it can be.

   The function peephole2_insns returns 0 if the rtl could not
   be matched. If there was a match, the new rtl is returned in a SEQUENCE,
   and LAST_INSN will point to the last recognized insn in the old sequence.
*/





static int recog_1 PARAMS ((rtx, rtx, int *));
static int
recog_1 (x0, insn, pnum_clobbers)
     register rtx x0;
     rtx insn ATTRIBUTE_UNUSED;
     int *pnum_clobbers ATTRIBUTE_UNUSED;
{
  register rtx * const operands ATTRIBUTE_UNUSED = &recog_data.operand[0];
  register rtx x1 ATTRIBUTE_UNUSED;
  register rtx x2 ATTRIBUTE_UNUSED;
  register rtx x3 ATTRIBUTE_UNUSED;
  register rtx x4 ATTRIBUTE_UNUSED;
  register rtx x5 ATTRIBUTE_UNUSED;
  int tem ATTRIBUTE_UNUSED;

  x1 = XEXP (x0, 1);
  switch (GET_CODE (x1))
    {
    case PLUS:
      goto L15;
    case MINUS:
      goto L20;
    case SMIN:
      goto L25;
    case SMAX:
      goto L30;
    case UMIN:
      goto L35;
    case UMAX:
      goto L40;
    case AND:
      goto L45;
    case IOR:
      goto L50;
    case XOR:
      goto L55;
    case ASHIFT:
      goto L60;
    case ASHIFTRT:
      goto L65;
    case LSHIFTRT:
      goto L70;
    case NEG:
      goto L75;
    case NOT:
      goto L79;
    case MULT:
      goto L95;
    case SIGN_EXTEND:
      goto L154;
    case ZERO_EXTEND:
      goto L162;
    case NE:
      goto L185;
    case IF_THEN_ELSE:
      goto L195;
    default:
     break;
   }
 L320: ATTRIBUTE_UNUSED_LABEL
  if (comparison_operator (x1, SImode))
    {
      operands[1] = x1;
      goto L180;
    }
  goto ret0;

 L15: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      goto L16;
    }
  goto ret0;

 L16: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[2] = x2;
      return 4;
    }
  goto ret0;

 L20: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      goto L21;
    }
  goto ret0;

 L21: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (register_operand (x2, SImode))
    {
      operands[2] = x2;
      return 5;
    }
  goto ret0;

 L25: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      goto L26;
    }
  goto ret0;

 L26: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[2] = x2;
      return 6;
    }
  goto ret0;

 L30: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      goto L31;
    }
  goto ret0;

 L31: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[2] = x2;
      return 7;
    }
  goto ret0;

 L35: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      goto L36;
    }
  goto ret0;

 L36: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[2] = x2;
      return 8;
    }
  goto ret0;

 L40: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      goto L41;
    }
  goto ret0;

 L41: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[2] = x2;
      return 9;
    }
  goto ret0;

 L45: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (GET_MODE (x2) == SImode)
    goto L324;
  goto ret0;

 L324: ATTRIBUTE_UNUSED_LABEL
  if (GET_CODE (x2) == NOT)
    goto L84;
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      goto L46;
    }
  goto ret0;

 L84: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (register_operand (x3, SImode))
    {
      operands[1] = x3;
      goto L85;
    }
  goto ret0;

 L85: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[2] = x2;
      return 18;
    }
  goto ret0;

 L46: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[2] = x2;
      return 10;
    }
  goto ret0;

 L50: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (GET_MODE (x2) == SImode)
    goto L326;
  goto ret0;

 L326: ATTRIBUTE_UNUSED_LABEL
  if (GET_CODE (x2) == NOT)
    goto L90;
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      goto L51;
    }
  goto ret0;

 L90: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (register_operand (x3, SImode))
    {
      operands[1] = x3;
      goto L91;
    }
  goto ret0;

 L91: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[2] = x2;
      return 19;
    }
  goto ret0;

 L51: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[2] = x2;
      return 11;
    }
  goto ret0;

 L55: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      goto L56;
    }
  goto ret0;

 L56: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[2] = x2;
      return 12;
    }
  goto ret0;

 L60: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      goto L61;
    }
  goto ret0;

 L61: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[2] = x2;
      return 13;
    }
  goto ret0;

 L65: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      goto L66;
    }
  goto ret0;

 L66: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[2] = x2;
      return 14;
    }
  goto ret0;

 L70: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      goto L71;
    }
  goto ret0;

 L71: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[2] = x2;
      return 15;
    }
  goto ret0;

 L75: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      return 16;
    }
  goto ret0;

 L79: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      return 17;
    }
  goto ret0;

 L95: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (GET_MODE (x2) == SImode)
    goto L328;
  goto ret0;

 L328: ATTRIBUTE_UNUSED_LABEL
  switch (GET_CODE (x2))
    {
    case SIGN_EXTEND:
      goto L119;
    case ZERO_EXTEND:
      goto L126;
    case SUBREG:
    case REG:
      goto L327;
    default:
      goto ret0;
   }
 L327: ATTRIBUTE_UNUSED_LABEL
  if (register_operand (x2, SImode))
    {
      operands[1] = x2;
      goto L96;
    }
  goto ret0;

 L119: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (register_operand (x3, HImode))
    {
      operands[1] = x3;
      goto L120;
    }
  goto ret0;

 L120: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (GET_MODE (x2) == SImode
      && GET_CODE (x2) == SIGN_EXTEND)
    goto L121;
  goto ret0;

 L121: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (nonmemory_operand (x3, HImode))
    {
      operands[2] = x3;
      return 23;
    }
 L135: ATTRIBUTE_UNUSED_LABEL
  if (const_int_operand (x3, VOIDmode))
    {
      operands[2] = x3;
      goto L136;
    }
  goto ret0;

 L136: ATTRIBUTE_UNUSED_LABEL
  if ((((unsigned int)INTVAL (operands[2]) < 65536)))
    {
      return 25;
    }
  goto ret0;

 L126: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (register_operand (x3, HImode))
    {
      operands[1] = x3;
      goto L127;
    }
  goto ret0;

 L127: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (GET_MODE (x2) == SImode
      && GET_CODE (x2) == ZERO_EXTEND)
    goto L128;
  goto ret0;

 L128: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (nonmemory_operand (x3, HImode))
    {
      operands[2] = x3;
      return 24;
    }
 L143: ATTRIBUTE_UNUSED_LABEL
  if (const_int_operand (x3, VOIDmode))
    {
      operands[2] = x3;
      goto L144;
    }
  goto ret0;

 L144: ATTRIBUTE_UNUSED_LABEL
  if ((((unsigned int)INTVAL (operands[2]) < 65536)))
    {
      return 26;
    }
  goto ret0;

 L96: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (GET_MODE (x2) == SImode)
    goto L330;
 L149: ATTRIBUTE_UNUSED_LABEL
  if (const_int_operand (x2, VOIDmode))
    {
      operands[2] = x2;
      goto L150;
    }
  goto ret0;

 L330: ATTRIBUTE_UNUSED_LABEL
  switch (GET_CODE (x2))
    {
    case AND:
      goto L97;
    case SIGN_EXTEND:
      goto L111;
    default:
     break;
   }
  goto L149;

 L97: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (nonmemory_operand (x3, SImode))
    {
      operands[2] = x3;
      goto L98;
    }
  goto L149;

 L98: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 1);
  if (GET_CODE (x3) == CONST_INT)
    goto L332;
  goto L149;

 L332: ATTRIBUTE_UNUSED_LABEL
  switch ((int) XWINT (x3, 0))
    {
    case -65536:
      goto L334;
    case 65535:
      goto L335;
    default:
      break;
    }
  goto L149;

 L334: ATTRIBUTE_UNUSED_LABEL
  return 20;

 L335: ATTRIBUTE_UNUSED_LABEL
  return 21;

 L111: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (GET_MODE (x3) == HImode
      && GET_CODE (x3) == SUBREG
      && XINT (x3, 1) == 0)
    goto L112;
  goto L149;

 L112: ATTRIBUTE_UNUSED_LABEL
  x4 = XEXP (x3, 0);
  if (GET_MODE (x4) == SImode
      && GET_CODE (x4) == PLUS)
    goto L113;
  goto L149;

 L113: ATTRIBUTE_UNUSED_LABEL
  x5 = XEXP (x4, 0);
  if (nonmemory_operand (x5, SImode))
    {
      operands[2] = x5;
      goto L114;
    }
  goto L149;

 L114: ATTRIBUTE_UNUSED_LABEL
  x5 = XEXP (x4, 1);
  if (GET_CODE (x5) == CONST_INT
      && XWINT (x5, 0) == 0)
    {
      return 22;
    }
  goto L149;

 L150: ATTRIBUTE_UNUSED_LABEL
  if ((((unsigned int)INTVAL (operands[2]) < 65536)))
    {
      return 27;
    }
  goto ret0;

 L154: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  switch (GET_MODE (x2))
    {
    case HImode:
      goto L336;
    case QImode:
      goto L337;
    default:
      break;
    }
  goto ret0;

 L336: ATTRIBUTE_UNUSED_LABEL
  if (nonimmediate_operand (x2, HImode))
    {
      operands[1] = x2;
      return 28;
    }
  goto ret0;

 L337: ATTRIBUTE_UNUSED_LABEL
  if (nonimmediate_operand (x2, QImode))
    {
      operands[1] = x2;
      return 29;
    }
  goto ret0;

 L162: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  switch (GET_MODE (x2))
    {
    case HImode:
      goto L338;
    case QImode:
      goto L339;
    default:
      break;
    }
  goto ret0;

 L338: ATTRIBUTE_UNUSED_LABEL
  if (memory_operand (x2, HImode))
    {
      operands[1] = x2;
      return 30;
    }
  goto ret0;

 L339: ATTRIBUTE_UNUSED_LABEL
  if (memory_operand (x2, QImode))
    {
      operands[1] = x2;
      return 31;
    }
  goto ret0;

 L185: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  switch (GET_MODE (x2))
    {
    case CCmode:
      goto L340;
    case CC_BRmode:
      goto L341;
    default:
      break;
    }
  goto L320;

 L340: ATTRIBUTE_UNUSED_LABEL
  if (register_operand (x2, CCmode))
    {
      operands[1] = x2;
      goto L186;
    }
  goto L320;

 L186: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == CONST_INT
      && XWINT (x2, 0) == 0)
    {
      return 35;
    }
  goto L320;

 L341: ATTRIBUTE_UNUSED_LABEL
  if (register_operand (x2, CC_BRmode))
    {
      operands[1] = x2;
      goto L191;
    }
  goto L320;

 L191: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == CONST_INT
      && XWINT (x2, 0) == 0)
    {
      return 36;
    }
  goto L320;

 L195: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (GET_MODE (x2) == CCmode)
    goto L342;
  goto ret0;

 L342: ATTRIBUTE_UNUSED_LABEL
  switch (GET_CODE (x2))
    {
    case NE:
      goto L196;
    case EQ:
      goto L204;
    default:
     break;
   }
  goto ret0;

 L196: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (register_operand (x3, CCmode))
    {
      operands[1] = x3;
      goto L197;
    }
  goto ret0;

 L197: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 1);
  if (GET_CODE (x3) == CONST_INT
      && XWINT (x3, 0) == 0)
    goto L198;
  goto ret0;

 L198: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (register_operand (x2, SImode))
    {
      operands[2] = x2;
      goto L199;
    }
  goto ret0;

 L199: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 2);
  if (nonmemory_operand (x2, SImode))
    {
      operands[3] = x2;
      return 37;
    }
  goto ret0;

 L204: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (register_operand (x3, CCmode))
    {
      operands[1] = x3;
      goto L205;
    }
  goto ret0;

 L205: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 1);
  if (GET_CODE (x3) == CONST_INT
      && XWINT (x3, 0) == 0)
    goto L206;
  goto ret0;

 L206: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (register_operand (x2, SImode))
    {
      operands[2] = x2;
      goto L207;
    }
  goto ret0;

 L207: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 2);
  if (nonmemory_operand (x2, SImode))
    {
      operands[3] = x2;
      return 38;
    }
  goto ret0;

 L180: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[2] = x2;
      goto L181;
    }
  goto ret0;

 L181: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[3] = x2;
      return 34;
    }
  goto ret0;
 ret0:
  return -1;
}

int recog PARAMS ((rtx, rtx, int *));
int
recog (x0, insn, pnum_clobbers)
     register rtx x0;
     rtx insn ATTRIBUTE_UNUSED;
     int *pnum_clobbers ATTRIBUTE_UNUSED;
{
  register rtx * const operands ATTRIBUTE_UNUSED = &recog_data.operand[0];
  register rtx x1 ATTRIBUTE_UNUSED;
  register rtx x2 ATTRIBUTE_UNUSED;
  register rtx x3 ATTRIBUTE_UNUSED;
  register rtx x4 ATTRIBUTE_UNUSED;
  register rtx x5 ATTRIBUTE_UNUSED;
  int tem ATTRIBUTE_UNUSED;
#if 0
  recog_data.insn = NULL_RTX;
#endif

  switch (GET_CODE (x0))
    {
    case SET:
      goto L1;
    case PARALLEL:
      goto L293;
    case CALL:
      goto L266;
    case CONST_INT:
      goto L294;
    case UNSPEC_VOLATILE:
      goto L295;
    default:
     break;
   }
  goto ret0;

 L1: ATTRIBUTE_UNUSED_LABEL
  x1 = XEXP (x0, 0);
  switch (GET_MODE (x1))
    {
    case SImode:
      goto L296;
    case HImode:
      goto L297;
    case QImode:
      goto L298;
    case SFmode:
      goto L299;
    case CCmode:
      goto L301;
    case CC_BRmode:
      goto L302;
    default:
      break;
    }
 L209: ATTRIBUTE_UNUSED_LABEL
  if (GET_CODE (x1) == PC)
    goto L250;
  if (register_operand (x1, VOIDmode))
    {
      operands[0] = x1;
      goto L281;
    }
  goto ret0;

 L296: ATTRIBUTE_UNUSED_LABEL
  if (nonimmediate_operand (x1, SImode))
    {
      operands[0] = x1;
      goto L2;
    }
 L300: ATTRIBUTE_UNUSED_LABEL
  if (register_operand (x1, SImode))
    {
      operands[0] = x1;
      goto L14;
    }
  goto L209;

 L2: ATTRIBUTE_UNUSED_LABEL
  x1 = XEXP (x0, 1);
  if (general_operand (x1, SImode))
    {
      operands[1] = x1;
      return 0;
    }
  x1 = XEXP (x0, 0);
  goto L300;

 L14: ATTRIBUTE_UNUSED_LABEL
  x1 = XEXP (x0, 1);
  if (GET_MODE (x1) == SImode)
    goto L303;
  x1 = XEXP (x0, 0);
  goto L209;

 L303: ATTRIBUTE_UNUSED_LABEL
  tem = recog_1 (x0, insn, pnum_clobbers);
  if (tem >= 0)
    return tem;
  x1 = XEXP (x0, 0);
  goto L209;

 L297: ATTRIBUTE_UNUSED_LABEL
  if (nonimmediate_operand (x1, HImode))
    {
      operands[0] = x1;
      goto L5;
    }
  goto L209;

 L5: ATTRIBUTE_UNUSED_LABEL
  x1 = XEXP (x0, 1);
  if (general_operand (x1, HImode))
    {
      operands[1] = x1;
      return 1;
    }
  x1 = XEXP (x0, 0);
  goto L209;

 L298: ATTRIBUTE_UNUSED_LABEL
  if (nonimmediate_operand (x1, QImode))
    {
      operands[0] = x1;
      goto L8;
    }
  goto L209;

 L8: ATTRIBUTE_UNUSED_LABEL
  x1 = XEXP (x0, 1);
  if (general_operand (x1, QImode))
    {
      operands[1] = x1;
      return 2;
    }
  x1 = XEXP (x0, 0);
  goto L209;

 L299: ATTRIBUTE_UNUSED_LABEL
  if (nonimmediate_operand (x1, SFmode))
    {
      operands[0] = x1;
      goto L11;
    }
  goto L209;

 L11: ATTRIBUTE_UNUSED_LABEL
  x1 = XEXP (x0, 1);
  if (general_operand (x1, SFmode))
    {
      operands[1] = x1;
      return 3;
    }
  x1 = XEXP (x0, 0);
  goto L209;

 L301: ATTRIBUTE_UNUSED_LABEL
  if (register_operand (x1, CCmode))
    {
      operands[0] = x1;
      goto L169;
    }
  goto L209;

 L169: ATTRIBUTE_UNUSED_LABEL
  x1 = XEXP (x0, 1);
  if (comparison_operator (x1, CCmode))
    {
      operands[1] = x1;
      goto L170;
    }
  x1 = XEXP (x0, 0);
  goto L209;

 L170: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[2] = x2;
      goto L171;
    }
  x1 = XEXP (x0, 0);
  goto L209;

 L171: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[3] = x2;
      return 32;
    }
  x1 = XEXP (x0, 0);
  goto L209;

 L302: ATTRIBUTE_UNUSED_LABEL
  if (register_operand (x1, CC_BRmode))
    {
      operands[0] = x1;
      goto L174;
    }
  goto L209;

 L174: ATTRIBUTE_UNUSED_LABEL
  x1 = XEXP (x0, 1);
  if (comparison_operator (x1, CC_BRmode))
    {
      operands[1] = x1;
      goto L175;
    }
  x1 = XEXP (x0, 0);
  goto L209;

 L175: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (register_operand (x2, SImode))
    {
      operands[2] = x2;
      goto L176;
    }
  x1 = XEXP (x0, 0);
  goto L209;

 L176: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (nonmemory_operand (x2, SImode))
    {
      operands[3] = x2;
      return 33;
    }
  x1 = XEXP (x0, 0);
  goto L209;

 L250: ATTRIBUTE_UNUSED_LABEL
  x1 = XEXP (x0, 1);
  if (register_operand (x1, SImode))
    {
      operands[0] = x1;
      return 44;
    }
  switch (GET_CODE (x1))
    {
    case IF_THEN_ELSE:
      goto L211;
    case LABEL_REF:
      goto L247;
    default:
     break;
   }
  goto ret0;

 L211: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (GET_MODE (x2) == CC_BRmode)
    goto L344;
  goto ret0;

 L344: ATTRIBUTE_UNUSED_LABEL
  switch (GET_CODE (x2))
    {
    case NE:
      goto L212;
    case EQ:
      goto L221;
    default:
     break;
   }
  goto ret0;

 L212: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (register_operand (x3, CC_BRmode))
    {
      operands[0] = x3;
      goto L213;
    }
  goto ret0;

 L213: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 1);
  if (GET_CODE (x3) == CONST_INT
      && XWINT (x3, 0) == 0)
    goto L214;
  goto ret0;

 L214: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  switch (GET_CODE (x2))
    {
    case LABEL_REF:
      goto L215;
    case PC:
      goto L233;
    default:
     break;
   }
  goto ret0;

 L215: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  operands[1] = x3;
  goto L216;

 L216: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 2);
  if (GET_CODE (x2) == PC)
    {
      return 39;
    }
  goto ret0;

 L233: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 2);
  if (GET_CODE (x2) == LABEL_REF)
    goto L234;
  goto ret0;

 L234: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  operands[1] = x3;
  return 41;

 L221: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (register_operand (x3, CC_BRmode))
    {
      operands[0] = x3;
      goto L222;
    }
  goto ret0;

 L222: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 1);
  if (GET_CODE (x3) == CONST_INT
      && XWINT (x3, 0) == 0)
    goto L223;
  goto ret0;

 L223: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  switch (GET_CODE (x2))
    {
    case LABEL_REF:
      goto L224;
    case PC:
      goto L242;
    default:
     break;
   }
  goto ret0;

 L224: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  operands[1] = x3;
  goto L225;

 L225: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 2);
  if (GET_CODE (x2) == PC)
    {
      return 40;
    }
  goto ret0;

 L242: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 2);
  if (GET_CODE (x2) == LABEL_REF)
    goto L243;
  goto ret0;

 L243: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  operands[1] = x3;
  return 42;

 L247: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  operands[0] = x2;
  return 43;

 L281: ATTRIBUTE_UNUSED_LABEL
  x1 = XEXP (x0, 1);
  if (GET_CODE (x1) == CALL)
    goto L282;
  goto ret0;

 L282: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == MEM)
    goto L283;
  goto ret0;

 L283: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (nonmemory_operand (x3, SImode))
    {
      operands[1] = x3;
      goto L284;
    }
  goto ret0;

 L284: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  operands[2] = x2;
  goto L285;

 L285: ATTRIBUTE_UNUSED_LABEL
  if (pnum_clobbers != NULL)
    {
      *pnum_clobbers = 1;
      return 47;
    }
  goto ret0;

 L293: ATTRIBUTE_UNUSED_LABEL
  if (XVECLEN (x0, 0) == 2)
    goto L252;
  goto ret0;

 L252: ATTRIBUTE_UNUSED_LABEL
  x1 = XVECEXP (x0, 0, 0);
  switch (GET_CODE (x1))
    {
    case SET:
      goto L253;
    case CALL:
      goto L260;
    case RETURN:
      goto L288;
    default:
     break;
   }
  goto ret0;

 L253: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == PC)
    goto L254;
  if (register_operand (x2, VOIDmode))
    {
      operands[0] = x2;
      goto L273;
    }
  goto ret0;

 L254: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (register_operand (x2, SImode))
    {
      operands[0] = x2;
      goto L255;
    }
  goto ret0;

 L255: ATTRIBUTE_UNUSED_LABEL
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == USE)
    goto L256;
  goto ret0;

 L256: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == LABEL_REF)
    goto L257;
  goto ret0;

 L257: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  operands[1] = x3;
  return 45;

 L273: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == CALL)
    goto L274;
  goto ret0;

 L274: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (GET_CODE (x3) == MEM)
    goto L275;
  goto ret0;

 L275: ATTRIBUTE_UNUSED_LABEL
  x4 = XEXP (x3, 0);
  if (nonmemory_operand (x4, SImode))
    {
      operands[1] = x4;
      goto L276;
    }
  goto ret0;

 L276: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 1);
  operands[2] = x3;
  goto L277;

 L277: ATTRIBUTE_UNUSED_LABEL
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == CLOBBER)
    goto L278;
  goto ret0;

 L278: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (GET_MODE (x2) == SImode
      && GET_CODE (x2) == REG
      && XINT (x2, 0) == 63)
    {
      return 47;
    }
  goto ret0;

 L260: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == MEM)
    goto L261;
  goto ret0;

 L261: ATTRIBUTE_UNUSED_LABEL
  x3 = XEXP (x2, 0);
  if (nonmemory_operand (x3, SImode))
    {
      operands[0] = x3;
      goto L262;
    }
  goto ret0;

 L262: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 1);
  operands[1] = x2;
  goto L263;

 L263: ATTRIBUTE_UNUSED_LABEL
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == CLOBBER)
    goto L264;
  goto ret0;

 L264: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (GET_MODE (x2) == SImode
      && GET_CODE (x2) == REG
      && XINT (x2, 0) == 63)
    {
      return 46;
    }
  goto ret0;

 L288: ATTRIBUTE_UNUSED_LABEL
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == USE)
    goto L289;
  goto ret0;

 L289: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (GET_MODE (x2) == SImode
      && GET_CODE (x2) == REG
      && XINT (x2, 0) == 63
      && (lx_direct_return ()))
    {
      return 48;
    }
  goto ret0;

 L266: ATTRIBUTE_UNUSED_LABEL
  x1 = XEXP (x0, 0);
  if (GET_CODE (x1) == MEM)
    goto L267;
  goto ret0;

 L267: ATTRIBUTE_UNUSED_LABEL
  x2 = XEXP (x1, 0);
  if (nonmemory_operand (x2, SImode))
    {
      operands[0] = x2;
      goto L268;
    }
  goto ret0;

 L268: ATTRIBUTE_UNUSED_LABEL
  x1 = XEXP (x0, 1);
  operands[1] = x1;
  goto L269;

 L269: ATTRIBUTE_UNUSED_LABEL
  if (pnum_clobbers != NULL)
    {
      *pnum_clobbers = 1;
      return 46;
    }
  goto ret0;

 L294: ATTRIBUTE_UNUSED_LABEL
  if (XWINT (x0, 0) == 0)
    {
      return 49;
    }
  goto ret0;

 L295: ATTRIBUTE_UNUSED_LABEL
  if (XVECLEN (x0, 0) == 1
      && XINT (x0, 1) == 2)
    goto L292;
  goto ret0;

 L292: ATTRIBUTE_UNUSED_LABEL
  x1 = XVECEXP (x0, 0, 0);
  if (GET_CODE (x1) == CONST_INT
      && XWINT (x1, 0) == 0)
    {
      return 50;
    }
  goto ret0;
 ret0:
  return -1;
}

rtx split_insns PARAMS ((rtx, rtx));
rtx
split_insns (x0, insn)
     register rtx x0;
     rtx insn ATTRIBUTE_UNUSED;
{
  register rtx * const operands ATTRIBUTE_UNUSED = &recog_data.operand[0];
  register rtx x1 ATTRIBUTE_UNUSED;
  register rtx x2 ATTRIBUTE_UNUSED;
  register rtx x3 ATTRIBUTE_UNUSED;
  register rtx x4 ATTRIBUTE_UNUSED;
  register rtx x5 ATTRIBUTE_UNUSED;
  rtx tem ATTRIBUTE_UNUSED;
#if 0
  recog_data.insn = NULL_RTX;
#endif
  goto ret0;
 ret0:
  return 0;
}

