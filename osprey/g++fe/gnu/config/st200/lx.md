;; gcc machine description for the the ST200.
;; Christian Bruel <christian.bruel@st.com>
;; Antonio Borneo  <antonio.borneo@st.com>


;; This file is part of GNU CC.

;; GNU CC is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; GNU CC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GNU CC; see the file COPYING.  If not, write to
;; the Free Software Foundation, 59 Temple Place - Suite 330,
;; Boston, MA 02111-1307, USA.

;;- See file "rtl.def" for documentation on define_insn, match_*, et. al.



;; ::::::::::::::::::::
;; ::
;; :: Attributes
;; ::
;; ::::::::::::::::::::

;; Instruction type. Set inside each (define_insn ...).
;; All the other instruction attributes are derived from this one.
;; - LoadW	: Load Word (32 bits)
;; - LoadNW	: Load not Word (less than 32 bits), extra latency
;; - Store	: Store
;; - LoadLR	: Load word in Link Register ($r0.63), extra latency
;; - Mov2LR	: Move to Link Register ($r0.63), extra latency
;; - ALU1	: Alu insn with one operand (e.g. mov, not, ...)
;; - ALU2	: Alu insn with two operands (e.g. add, sub, ...)
;; - ALU3	: Alu insn with three operands (e.g. Shift&Add, Slct, ...)
;; - Mult	: Multiply insn
;; - Cmp2Slct	: CMP insn to drive any following ALU insn
;; - Cmp2Br	: CMP insn to drive branch decision (extra latency)
;; - Branch	: Conditional Branch
;; - Jump	: Un-Conditional Branch
;; - Call	: Procedure Call
;; - Return	: Procedure Return
;; - Misc	: Currently only NOP
;; - BundleStop	: Virtual insn to specify bundle separation

(define_attr "insn_type"
  "LoadW,LoadNW,Store,LoadLR,Mov2LR,ALU1,ALU2,ALU3,Mult,Cmp2Slct,Cmp2Br,Branch,Jump,Call,Return,Misc,BundleStop"
  (const_string "Misc")
)

;; Processor type (for scheduling, not code generation) -- this attribute
;; must exactly match the architecture_type enumeration in lx.h.
;;
(define_attr "cpu" "st210,st220" (const (symbol_ref "lx_cpu_attr")))

;; Long Immediate. Detect instructions that uses an additional issue.
(define_attr "LongImm" "no,yes"
  (cond [(eq_attr "insn_type" "LoadW,LoadNW,LoadLR")
         (if_then_else (match_operand 1 "lx_long_add_operand" "")
                       (const_string "yes") (const_string "no"))

         (eq_attr "insn_type" "Store")
         (if_then_else (match_operand 0 "lx_long_add_operand" "")
                       (const_string "yes") (const_string "no"))

         (eq_attr "insn_type" "ALU1")
         (if_then_else (match_operand 1 "lx_long_imm_operand" "")
                       (const_string "yes") (const_string "no"))

         (eq_attr "insn_type" "ALU2,Mult")
         (if_then_else (match_operand 2 "lx_long_imm_operand" "")
                       (const_string "yes") (const_string "no"))

         (eq_attr "insn_type" "ALU3,Cmp2Slct,Cmp2Br")
         (if_then_else (match_operand 3 "lx_long_imm_operand" "")
                       (const_string "yes") (const_string "no"))
        ]
        (const_string "no"))
)



;; issue_type. In order to collect insns in a bundle, we have to identify
;; which issue is used
;;
;; A Alu
;; B Branch
;; M Memory
;; U mUltiply
;; S Stop bit
(define_attr "issue_type" "A,B,M,U,S"
  (cond [(eq_attr "insn_type" "Branch,Jump,Call,Return")
         (const_string "B")

         (eq_attr "insn_type" "LoadW,LoadNW,LoadLR,Store")
         (const_string "M")

         (eq_attr "insn_type" "Mult")
         (const_string "U")

         (eq_attr "insn_type" "BundleStop")
         (const_string "S")
        ]
        (const_string "A"))
)



;; Unit_type. Specifies the function_unit it uses, in order to compute
;; the right pipeline delay
(define_attr "Unit_type" "ALU_1,ALU_2,ALU_3,MUL_2,MEM_1,MEM_2,MEM_3,MEM_4,BRANCH"
  (cond [(eq_attr "insn_type" "Store")
         (const_string "MEM_1")

	 (eq_attr "insn_type" "LoadW")
         (const_string "MEM_2")

         (eq_attr "insn_type" "LoadNW")
         (const_string "MEM_3")

         (eq_attr "insn_type" "LoadLR")
         (const_string "MEM_4")

         (eq_attr "insn_type" "Mov2LR")
         (const_string "ALU_3")

         (eq_attr "insn_type" "Mult")
         (const_string "MUL_2")

         (eq_attr "insn_type" "Cmp2Br")
         (const_string "ALU_2")

         (eq_attr "insn_type" "Branch,Jump,Call,Return")
         (const_string "BRANCH")
        ]
        (const_string "ALU_1"))
)



;; Insn length in bytes. Is it usefull?
(define_attr "length" ""
  (cond [(eq_attr "insn_type" "BundleStop")
         (const_int 0)

         (eq_attr "LongImm" "yes")
         (const_int 8)
        ]
        (const_int 4))
)


(define_attr "delay_model" "nobundle,bundle,reorg_bundle"
  (const (if_then_else (symbol_ref "TARGET_NO_BUNDLE")
		       (const_string "nobundle") (const_string "bundle"))))


;; ::::::::::::::::::::
;; ::
;; :: Function Units
;; ::
;; ::::::::::::::::::::

;; Each usage of a function units by a class of insns is specified with a
;; `define_function_unit' expression, which looks like this:
;; (define_function_unit NAME MULTIPLICITY SIMULTANEITY TEST READY-DELAY
;;   ISSUE-DELAY [CONFLICT-LIST])

(define_function_unit "int1" 1 0 (and (eq_attr "delay_model" "nobundle")
                                      (eq_attr "Unit_type" "ALU_1")) 1 1)
(define_function_unit "int1" 1 0 (and (eq_attr "delay_model" "nobundle")
                                      (eq_attr "Unit_type" "ALU_2")) 2 1)
(define_function_unit "int1" 1 0 (and (eq_attr "delay_model" "nobundle")
                                      (eq_attr "Unit_type" "ALU_3")) 3 1)
(define_function_unit "mul1" 1 0 (and (eq_attr "delay_model" "nobundle")
                                      (and (eq_attr "Unit_type" "MUL_2")
					   (eq_attr "cpu" "st210"))) 2 1)
(define_function_unit "mul1" 1 0 (and (eq_attr "delay_model" "nobundle")
                                      (and (eq_attr "Unit_type" "MUL_2")
					   (eq_attr "cpu" "st220"))) 3 1)
(define_function_unit "mem1" 1 0 (and (eq_attr "delay_model" "nobundle")
                                      (eq_attr "Unit_type" "MEM_1")) 2 1)
(define_function_unit "mem1" 1 0 (and (eq_attr "delay_model" "nobundle")
                                      (and (eq_attr "Unit_type" "MEM_2")
					   (eq_attr "cpu" "st210"))) 2 1)
(define_function_unit "mem1" 1 0 (and (eq_attr "delay_model" "nobundle")
                                      (and (eq_attr "Unit_type" "MEM_2")
					   (eq_attr "cpu" "st220"))) 3 1)
(define_function_unit "mem1" 1 0 (and (eq_attr "delay_model" "nobundle")
                                      (eq_attr "Unit_type" "MEM_3")) 3 1)
(define_function_unit "mem1" 1 0 (and (eq_attr "delay_model" "nobundle")
                                      (eq_attr "Unit_type" "MEM_4")) 4 1)
(define_function_unit "jmp1" 1 0 (and (eq_attr "delay_model" "nobundle")
                                      (eq_attr "Unit_type" "BRANCH")) 1 1)


(define_function_unit "int2" 4 0 (and (eq_attr "delay_model" "bundle")
                                      (eq_attr "Unit_type" "ALU_1")) 4 4)
(define_function_unit "int2" 4 0 (and (eq_attr "delay_model" "bundle")
                                      (eq_attr "Unit_type" "ALU_2")) 8 4)
(define_function_unit "int2" 4 0 (and (eq_attr "delay_model" "bundle")
                                      (eq_attr "Unit_type" "ALU_3")) 12 4)
(define_function_unit "mul2" 2 0 (and (eq_attr "delay_model" "bundle")
                                      (and (eq_attr "Unit_type" "MUL_2")
					   (eq_attr "cpu" "st210"))) 8 4)
(define_function_unit "mul2" 2 0 (and (eq_attr "delay_model" "bundle")
                                      (and (eq_attr "Unit_type" "MUL_2")
					   (eq_attr "cpu" "st220"))) 12 4)
(define_function_unit "mem2" 1 0 (and (eq_attr "delay_model" "bundle")
                                      (eq_attr "Unit_type" "MEM_1")) 8 4)
(define_function_unit "mem2" 1 0 (and (eq_attr "delay_model" "bundle")
                                      (and (eq_attr "Unit_type" "MEM_2")
					   (eq_attr "cpu" "st210"))) 8 4)
(define_function_unit "mem2" 1 0 (and (eq_attr "delay_model" "bundle")
                                      (and (eq_attr "Unit_type" "MEM_2")
					   (eq_attr "cpu" "st220"))) 12 4)
(define_function_unit "mem2" 1 0 (and (eq_attr "delay_model" "bundle")
                                      (eq_attr "Unit_type" "MEM_3")) 12 4)
(define_function_unit "mem2" 1 0 (and (eq_attr "delay_model" "bundle")
                                      (eq_attr "Unit_type" "MEM_4")) 16 4)
(define_function_unit "jmp2" 1 0 (and (eq_attr "delay_model" "bundle")
                                      (eq_attr "Unit_type" "BRANCH")) 4 4)


(define_function_unit "int3" 4 0 (and (eq_attr "delay_model" "reorg_bundle")
                                      (eq_attr "Unit_type" "ALU_1")) 1 1)
(define_function_unit "int3" 4 0 (and (eq_attr "delay_model" "reorg_bundle")
                                      (and (eq_attr "Unit_type" "ALU_2")
					   (eq_attr "cpu" "st210"))) 2 1)
(define_function_unit "int3" 4 0 (and (eq_attr "delay_model" "reorg_bundle")
                                      (and (eq_attr "Unit_type" "ALU_2")
					   (eq_attr "cpu" "st220"))) 3 1)
(define_function_unit "int3" 4 0 (and (eq_attr "delay_model" "reorg_bundle")
                                      (eq_attr "Unit_type" "ALU_3")) 3 1)
(define_function_unit "mul3" 2 0 (and (eq_attr "delay_model" "reorg_bundle")
                                      (and (eq_attr "Unit_type" "MUL_2")
					   (eq_attr "cpu" "st210"))) 2 1)
(define_function_unit "mul3" 2 0 (and (eq_attr "delay_model" "reorg_bundle")
                                      (and (eq_attr "Unit_type" "MUL_2")
					   (eq_attr "cpu" "st220"))) 3 1)
(define_function_unit "mem3" 1 0 (and (eq_attr "delay_model" "reorg_bundle")
                                      (eq_attr "Unit_type" "MEM_1")) 2 1)
(define_function_unit "mem3" 1 0 (and (eq_attr "delay_model" "reorg_bundle")
                                      (and (eq_attr "Unit_type" "MEM_2")
					   (eq_attr "cpu" "st210"))) 2 1)
(define_function_unit "mem3" 1 0 (and (eq_attr "delay_model" "reorg_bundle")
                                      (and (eq_attr "Unit_type" "MEM_2")
					   (eq_attr "cpu" "st220"))) 3 1)
(define_function_unit "mem3" 1 0 (and (eq_attr "delay_model" "reorg_bundle")
                                      (eq_attr "Unit_type" "MEM_3")) 3 1)
(define_function_unit "mem3" 1 0 (and (eq_attr "delay_model" "reorg_bundle")
                                      (eq_attr "Unit_type" "MEM_4")) 4 1)
(define_function_unit "jmp3" 1 0 (and (eq_attr "delay_model" "reorg_bundle")
                                      (eq_attr "Unit_type" "BRANCH")) 1 1)




;; ::::::::::::::::::::
;; ::
;; :: Moves instructions
;; ::
;; ::::::::::::::::::::

(define_expand "movsi"
  [(set (match_operand:SI 0 "general_operand" "")
	(match_operand:SI 1 "general_operand" ""))]
  ""
  "{
     if (! reload_in_progress && ! reload_completed
         && GET_CODE (operands[0]) == MEM
         && GET_CODE (operands[1]) == MEM)
       operands[1] = copy_to_mode_reg (SImode, operands[1]);

    /* (operand == 0) substituted with register #0 */
    if (GET_CODE (operands[1]) == CONST_INT && INTVAL (operands [1]) == 0)
       operands[1] = gen_rtx_REG (SImode, 0);
   }"
)

(define_insn "*movsi_internal"
  [(set (match_operand:SI 0 "nonimmediate_operand" "=l,l, r,r, m,r")
	(match_operand:SI 1 "general_operand"       "r,m,lr,m,lr,i"))]
  ""
  "@
  mov %0 = %1
  ldw %0 = %1
  mov %0 = %1
  ldw %0 = %1
  stw %0 = %1
  mov %0 = %1"
  [(set_attr "insn_type" "Mov2LR,LoadLR,ALU1,LoadW,Store,ALU1")]
)

(define_expand "movhi"
  [(set (match_operand:HI 0 "general_operand" "")
	(match_operand:HI 1 "general_operand" ""))]
  ""
  "
{
  if (! reload_in_progress && ! reload_completed
      && GET_CODE (operands[0]) == MEM
      && GET_CODE (operands[1]) == MEM)
    operands[1] = copy_to_mode_reg (HImode, operands[1]);

 if (GET_CODE (operands[1]) == CONST_INT && INTVAL (operands [1]) == 0)
    operands[1] = gen_rtx_REG (HImode, 0);
}")

(define_insn "*movhi_internal"
  [(set (match_operand:HI 0 "nonimmediate_operand" "=r,r,m,r")
	(match_operand:HI 1 "general_operand"       "r,m,r,i"))]
  ""
  "@
  mov %0 = %1
  ldh %0 = %1
  sth %0 = %1
  mov %0 = %1"
  [(set_attr "insn_type" "ALU1,LoadNW,Store,ALU1")]
)

(define_expand "movdi"
  [(set (match_operand:DI 0 "general_operand" "")
	(match_operand:DI 1 "general_operand" ""))]
  ""
  "
{
  if (! reload_in_progress && ! reload_completed
      && GET_CODE (operands[0]) == MEM
      && GET_CODE (operands[1]) == MEM)
    operands[1] = copy_to_mode_reg (DImode, operands[1]);

    emit_move_insn (operand_subword (operands[0], 0, 1, DImode),
 	      operand_subword (operands[1], 0, 1, DImode));
    emit_move_insn (operand_subword (operands[0], 1, 1, DImode),
	      operand_subword (operands[1], 1, 1, DImode));

    DONE;
}")

(define_expand "movqi"
  [(set (match_operand:QI 0 "general_operand" "")
	(match_operand:QI 1 "general_operand" ""))]
  ""
  "
{
  if (! reload_in_progress && ! reload_completed
      && GET_CODE (operands[0]) == MEM
      && GET_CODE (operands[1]) == MEM)
    operands[1] = copy_to_mode_reg (QImode, operands[1]);

 if (GET_CODE (operands[1]) == CONST_INT && INTVAL (operands [1]) == 0)
    operands[1] = gen_rtx_REG (QImode, 0);
}")

(define_insn "*movqi_internal"
  [(set (match_operand:QI 0 "nonimmediate_operand" "=r,r,m,r")
	(match_operand:QI 1 "general_operand"       "r,m,r,i"))]
  ""
  "@
  mov %0 = %1
  ldb %0 = %1
  stb %0 = %1
  mov %0 = %1"
  [(set_attr "insn_type" "ALU1,LoadNW,Store,ALU1")]
)

;; Floating Point Moves
;;
;; Note - Patterns for SF mode moves are compulsory, but
;; patterns for DF are optional, as GCC can synthesise them.

(define_expand "movsf"
  [(set (match_operand:SF 0 "general_operand" "")
	(match_operand:SF 1 "general_operand" ""))]
  ""
  "
{
  if (! reload_in_progress && ! reload_completed
      && GET_CODE (operands[0]) == MEM
      && GET_CODE (operands[1]) == MEM)
    operands[1] = copy_to_mode_reg (SFmode, operands[1]);
}")



(define_insn "*movsf_internal"
  [(set (match_operand:SF 0 "nonimmediate_operand" "=r,r,m,r")
	(match_operand:SF 1 "general_operand"       "r,m,r,i"))]
  ""
  "@
  mov %0 = %1
  ldw %0 = %1
  stw %0 = %1
  mov %0 = %1"
  [(set_attr "insn_type" "ALU1,LoadW,Store,ALU1")]
)




;; ::::::::::::::::::::
;; ::
;; :: 32 bit Integer arithmetic
;; ::
;; ::::::::::::::::::::

(define_insn "addsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (match_operand:SI 1 "register_operand" "%r")
		 (match_operand:SI 2 "nonmemory_operand" "ri")))]
  ""
  "add %0 = %1, %2"
  [(set_attr "insn_type" "ALU2")]
)



(define_insn "subsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(minus:SI (match_operand:SI 1 "register_operand" "r")
		  (match_operand:SI 2 "register_operand" "r")))]
  ""
  "sub %0 = %1, %2"
  [(set_attr "insn_type" "ALU2")]
)



(define_insn "sminsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(smin:SI (match_operand:SI 1 "register_operand" "%r")
		 (match_operand:SI 2 "nonmemory_operand" "ri")))]
  ""
  "min %0 = %1, %2"
  [(set_attr "insn_type" "ALU2")]
)



(define_insn "smaxsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(smax:SI (match_operand:SI 1 "register_operand" "%r")
		 (match_operand:SI 2 "nonmemory_operand" "ri")))]
  ""
  "max %0 = %1, %2"
  [(set_attr "insn_type" "ALU2")]
)



(define_insn "uminsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(umin:SI (match_operand:SI 1 "register_operand" "%r")
		 (match_operand:SI 2 "nonmemory_operand" "ri")))]
  ""
  "minu %0 = %1, %2"
  [(set_attr "insn_type" "ALU2")]
)



(define_insn "umaxsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(umax:SI (match_operand:SI 1 "register_operand" "%r")
		 (match_operand:SI 2 "nonmemory_operand" "ri")))]
  ""
  "maxu %0 = %1, %2"
  [(set_attr "insn_type" "ALU2")]
)



(define_insn "andsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (match_operand:SI 1 "register_operand" "%r")
		 (match_operand:SI 2 "nonmemory_operand" "ri")))]
  ""
  "and %0 = %1, %2"
  [(set_attr "insn_type" "ALU2")]
)



(define_insn "iorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ior:SI (match_operand:SI 1 "register_operand" "%r")
		(match_operand:SI 2 "nonmemory_operand" "ri")))]
  ""
  "or %0 = %1, %2"
  [(set_attr "insn_type" "ALU2")]
)



(define_insn "xorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(xor:SI (match_operand:SI 1 "register_operand" "%r")
		(match_operand:SI 2 "nonmemory_operand" "ri")))]
  ""
  "xor %0 = %1, %2"
  [(set_attr "insn_type" "ALU2")]
)



(define_insn "ashlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ashift:SI (match_operand:SI 1 "register_operand" "r")
		   (match_operand:SI 2 "nonmemory_operand" "ri")))]
  ""
  "shl %0 = %1, %2"
  [(set_attr "insn_type" "ALU2")]
)



(define_insn "ashrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ashiftrt:SI (match_operand:SI 1 "register_operand" "r")
		     (match_operand:SI 2 "nonmemory_operand" "ri")))]
  ""
  "shr %0 = %1, %2"
  [(set_attr "insn_type" "ALU2")]
)



(define_insn "lshrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(lshiftrt:SI (match_operand:SI 1 "register_operand" "r")
		     (match_operand:SI 2 "nonmemory_operand" "ri")))]
  ""
  "shru %0 = %1, %2"
  [(set_attr "insn_type" "ALU2")]
)



(define_insn "negsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (neg:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "sub %0 = $r0.0, %1"
  [(set_attr "insn_type" "ALU1")]
)



(define_expand "abssi2"
  [(set (match_dup 3)
        (neg:SI (match_operand:SI 1 "register_operand" "")))
   (set (match_operand:SI 0 "register_operand" "")
	(smax:SI (match_dup 1) (match_dup 3)))]
  ""
  "
{
    operands[3] = gen_reg_rtx (SImode);
}")



(define_insn "one_cmplsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(not:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "orc %0 = %1, $r0.0"
  [(set_attr "insn_type" "ALU1")]
)




;; ::::::::::::::::::::
;; ::
;; :: sh?add
;; ::
;; ::::::::::::::::::::

;; These instructions are generated during the "instruction combination" pass.
;; The current code in "combine.c" converts constant amount "left shifts" in
;; "multiplications". This is why the following template uses "mult" instead
;; of "ashift".
;; In a future gcc version this could change.

;;(define_insn "*shXadd_insn"
;;  [(set (match_operand:SI 0 "register_operand" "=r")
;;        (plus:SI (mult:SI (match_operand:SI 1 "register_operand" "r")
;;                          (match_operand:SI 2 "const_int_operand" "i"))
;;                 (match_operand:SI 3 "nonmemory_operand" "ri")))]
;;  "((INTVAL (operands [2]) ==  2) || (INTVAL (operands [2]) ==  4) ||
;;    (INTVAL (operands [2]) ==  8) || (INTVAL (operands [2]) ==  16))"
;;  "*
;;{
;;  switch (INTVAL (operands [2])) {
;;    case  2:   return \"sh1add %0 = %1, %3\";
;;    case  4:   return \"sh2add %0 = %1, %3\";
;;    case  8:   return \"sh3add %0 = %1, %3\";
;;    case 16:   return \"sh4add %0 = %1, %3\";
;;    default:   abort();
;;  }
;;}"
;;  [(set_attr "insn_type" "ALU3")]
;;)


;; ::::::::::::::::::::
;; ::
;; :: andc, orc
;; ::
;; ::::::::::::::::::::

(define_insn "*andc_insn"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (and:SI (not:SI (match_operand:SI 1 "register_operand" "r"))
                (match_operand:SI 2 "nonmemory_operand" "ri")))]
  ""
  "andc %0 = %1, %2"
  [(set_attr "insn_type" "ALU2")]
)



(define_insn "*orc_insn"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (ior:SI (not:SI (match_operand:SI 1 "register_operand" "r"))
                (match_operand:SI 2 "nonmemory_operand" "ri")))]
  ""
  "orc %0 = %1, %2"
  [(set_attr "insn_type" "ALU2")]
)




;; ::::::::::::::::::::
;; ::
;; :: Multiply
;; ::
;; ::::::::::::::::::::

;; mulsi3 is (mulhs + mullu + add)

(define_expand "mulsi3"
  [(set (match_dup 3)
        (mult:SI (match_operand:SI 1 "register_operand" "")
	         (and:SI (match_operand:SI 2 "nonmemory_operand" "")
                         (const_int -65536))))     ;; 0xffff0000
   (set (match_dup 4)
	 (mult:SI (match_dup 1) (and:SI (match_dup 2) (const_int 65535))))
   (set (match_operand:SI 0 "register_operand" "")
        (plus:SI (match_dup 3) (match_dup 4)))]
  ""
  "
{
    operands[3] = gen_reg_rtx (SImode);
    operands[4] = gen_reg_rtx (SImode);
}")


(define_insn "mulhs_insn"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(mult:SI (match_operand:SI 1 "register_operand" "r")
		 (and:SI (match_operand:SI 2 "nonmemory_operand" "ri")
                         (const_int -65536))))]  ;; 0xffff0000
  ""
  "mulhs %0 = %1, %2"
  [(set_attr "insn_type" "Mult")]
)

(define_insn "mullu_insn"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(mult:SI (match_operand:SI 1 "register_operand" "r")
		 (and:SI (match_operand:SI 2 "nonmemory_operand" "ri")
                         (const_int 65535))))]
  ""
  "mullu %0 = %1, %2"
  [(set_attr "insn_type" "Mult")]
)

;; Unreadable, but ... subreg:HI does not accept const_int argument
(define_insn "mull_insn"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(mult:SI (match_operand:SI 1 "register_operand" "r")
                 (sign_extend:SI
                   (subreg:HI
                     (plus:SI
                       (match_operand:SI 2 "nonmemory_operand" "ri")
                       (const_int 0))
                     0))))]
  ""
  "mull %0 = %1, %2"
  [(set_attr "insn_type" "Mult")]
)

(define_insn "mulhisi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(mult:SI
	 (sign_extend:SI (match_operand:HI 1 "register_operand" "%r"))
	 (sign_extend:SI (match_operand:HI 2 "nonmemory_operand" "ri"))))]
  ""
  "mulll %0 = %1, %2"
  [(set_attr "insn_type" "Mult")]
)

(define_insn "umulhisi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(mult:SI
	 (zero_extend:SI (match_operand:HI 1 "register_operand" "%r"))
	 (zero_extend:SI (match_operand:HI 2 "nonmemory_operand" "ri"))))]
  ""
  "mulllu %0 = %1, %2"
  [(set_attr "insn_type" "Mult")]
)

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(mult:SI
	 (sign_extend:SI (match_operand:HI 1 "register_operand" "%r"))
	 (sign_extend:SI (match_operand 2 "const_int_operand" "i"))))]
   "((unsigned int)INTVAL (operands[2]) < 65536)"
  "mulll %0 = %1, %2"
  [(set_attr "insn_type" "Mult")]
)

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(mult:SI
	 (zero_extend:SI (match_operand:HI 1 "register_operand" "%r"))
	 (zero_extend:SI (match_operand 2 "const_int_operand" "i"))))]
  "((unsigned int)INTVAL (operands[2]) < 65536)"
  "mulllu %0 = %1, %2"
  [(set_attr "insn_type" "Mult")]
)

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(mult:SI (match_operand:SI 1 "register_operand" "r")
		 (match_operand 2 "const_int_operand" "i")))]
   "((unsigned int)INTVAL (operands[2]) < 65536)"
   "mull %0 = %1, %2"
  [(set_attr "insn_type" "Mult")]
)


;; ::::::::::::::::::::
;; ::
;; :: extension and truncation
;; ::
;; ::::::::::::::::::::

(define_insn "extendhisi2"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(sign_extend:SI (match_operand:HI 1 "nonimmediate_operand" "r,m")))]
  ""
  "@
    sxth %0 = %1
    ldh %0 = %1"
  [(set_attr "insn_type" "ALU1,LoadNW")]
)



(define_insn "extendqisi2"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(sign_extend:SI (match_operand:QI 1 "nonimmediate_operand" "r,m")))]
  ""
  "@
    sxtb %0 = %1
    ldb %0 = %1"
  [(set_attr "insn_type" "ALU1,LoadNW")]
)



(define_expand "zero_extendhisi2"
  [(set (match_operand:SI 0 "register_operand" "")
	(zero_extend:SI (match_operand:HI 1 "nonimmediate_operand" "")))]
  ""
  "
{
    if (MEM_P (operands[1]))
      emit_insn (gen_ldhu_insn (operands[0], operands[1]));
    else {
      rtx op = gen_lowpart (SImode, operands[1]);
      emit_insn (gen_andsi3 (operands[0], op, GEN_INT (0xffff)));
    }
    DONE;
}")



(define_insn "ldhu_insn"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(zero_extend:SI (match_operand:HI 1 "memory_operand" "m")))]
  ""
  "ldhu %0 = %1"
  [(set_attr "insn_type" "LoadNW")]
)



(define_expand "zero_extendhiqi2"
  [(set (match_operand:SI 0 "register_operand" "")
	(zero_extend:SI (match_operand:QI 1 "register_operand" "")))]
  ""
  "
{
    if (MEM_P (operands[1]))
      emit_insn (gen_ldbu_insn (operands[0], operands[1]));
    else {
      rtx op = gen_lowpart (SImode, operands[1]);
      emit_insn (gen_andsi3 (operands[0], op, GEN_INT (0xff)));
    }
    DONE;
}")



(define_insn "ldbu_insn"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(zero_extend:SI (match_operand:QI 1 "memory_operand" "m")))]
  ""
  "ldbu %0 = %1"
  [(set_attr "insn_type" "LoadNW")]
)




;; ::::::::::::::::::::
;; ::
;; :: Comparisons
;; ::
;; ::::::::::::::::::::

(define_expand "cmpsi"
  [(set (cc0)
        (compare:CC (match_operand:SI 0 "register_operand" "")
		    (match_operand:SI 1 "nonmemory_operand" "")))]
  ""
  "
{
  lx_compare_op0 = operands[0];
  lx_compare_op1 = operands[1];
  DONE;
}")



(define_insn "cmpcc_insn"
  [(set (match_operand:CC  0 "register_operand" "=b")
	(match_operator:CC 1 "comparison_operator"
			   [(match_operand:SI 2 "register_operand" "r")
			    (match_operand:SI 3 "nonmemory_operand" "ri")]))]
  ""
  "cmp%j1 %0 = %2, %3"
  [(set_attr "insn_type" "Cmp2Slct")]
)



(define_insn "cmpcc_br_insn"
  [(set (match_operand:CC_BR  0 "register_operand" "=b")
	(match_operator:CC_BR 1 "comparison_operator"
                              [(match_operand:SI 2 "register_operand" "r")
                               (match_operand:SI 3 "nonmemory_operand" "ri")]))]
  ""
  "cmp%j1 %0 = %2, %3"
  [(set_attr "insn_type" "Cmp2Br")]
)



(define_insn "*cmpsi_insn"
  [(set (match_operand:SI  0 "register_operand" "=r")
	(match_operator:SI 1 "comparison_operator"
			   [(match_operand:SI 2 "register_operand" "r")
                            (match_operand:SI 3 "nonmemory_operand" "ri")]))]
  ""
  "cmp%j1 %0 = %2, %3"
  [(set_attr "insn_type" "ALU3")]
)



;; ??? We also need this if we run out of PR regs and need to spill some.

;; ??? We need this if a CCmode value does not get allocated to a hard
;; register.  This happens if we cse/gcse a CCmode value across a call, and the
;; function has a nonlocal goto.  This is because global does not allocate
;; call crossing pseudos to hard registers when current_function_has_
;; nonlocal_goto is true.  This is relatively common for C++ programs that
;; use exceptions.  See ia64_secondary_reload_class.

;; We use a define_expand here so that cse/gcse/combine can't accidentally
;; create movcc insns.  If this was a named define_insn, we would not be able
;; to make it conditional on reload.

;(define_expand "mov_cc"
;  [(set (match_operand:CC 0 "register_operand" "")
;	(match_operand:CC 1 "register_operand" ""))]
;  ""
;  "
;{
;  if (! reload_in_progress && ! reload_completed)
;    FAIL;
;}")

;(define_insn "*movcc_internal"
;  [(set (match_operand:CC 0 "register_operand" "=r,b")  ;;,r,m,m,b
;	(match_operand:CC 1 "register_operand" "b,r"))]      ;;,m,r,b,m
;  "reload_in_progress || reload_completed"
;  "boo %0 = %1")



(define_expand "reload_incc"
  [(set (match_operand:CC 0 "register_operand" "=b")
        (match_operand:CC 1 "" "b"))
   (clobber (match_operand:SI 2 "register_operand" "=&r"))]
  ""
  "
{
  rtx src, ne_rtx;
  ne_rtx = gen_rtx (NE, VOIDmode, NULL_RTX, NULL_RTX);
  if (GET_CODE(operands[1]) == MEM)
    src = change_address (operands[1], SImode, NULL_RTX);
  else if (GET_CODE (operands[1]) == REG || GET_CODE (operands[1]) == SUBREG)
    src = gen_rtx (REG, SImode, true_regnum (operands[1]));
  else
    src = operands[1];
  emit_insn(gen_move_insn (operands[2], src));
  emit_insn(gen_cmpcc_insn (operands[0], ne_rtx, operands[2], const0_rtx));
  DONE;
}")



(define_expand "reload_incc_br"
  [(set (match_operand:CC_BR 0 "register_operand" "=b")
        (match_operand:CC_BR 1 "" "b"))
   (clobber (match_operand:SI 2 "register_operand" "=&r"))]
  ""
  "
{
  rtx src, ne_rtx;
  ne_rtx = gen_rtx (NE, VOIDmode, NULL_RTX, NULL_RTX);
  if (GET_CODE(operands[1]) == MEM)
    src = change_address (operands[1], SImode, NULL_RTX);
  else if (GET_CODE (operands[1]) == REG || GET_CODE (operands[1]) == SUBREG)
    src = gen_rtx (REG, SImode, true_regnum (operands[1]));
  else
    src = operands[1];
  emit_insn(gen_move_insn (operands[2], src));
  emit_insn(gen_cmpcc_br_insn (operands[0], ne_rtx, operands[2], const0_rtx));
  DONE;
}")



(define_expand "reload_outcc"
  [(set (match_operand:CC_BR 0 "" "=b")
        (match_operand:CC_BR 1 "register_operand" "b"))
   (clobber (match_operand:SI 2 "register_operand" "=&r"))]
  ""
  "
{
  rtx dst;
  if (GET_CODE(operands[0]) == MEM)
    dst = change_address (operands[0], SImode, NULL_RTX);
  else if (GET_CODE (operands[0]) == REG || GET_CODE (operands[0]) == SUBREG)
    dst = gen_rtx (REG, SImode, true_regnum (operands[0]));
  else
    dst = operands[0];
  emit_insn(gen_mfb_cc_insn (operands[2], operands[1]));
  emit_insn(gen_move_insn (dst, operands[2]));
  DONE;
}")



(define_expand "reload_outcc_br"
  [(set (match_operand:CC_BR 0 "" "=b")
        (match_operand:CC_BR 1 "register_operand" "b"))
   (clobber (match_operand:SI 2 "register_operand" "=&r"))]
  ""
  "
{
  rtx dst;
  if (GET_CODE(operands[0]) == MEM)
    dst = change_address (operands[0], SImode, NULL_RTX);
  else if (GET_CODE (operands[0]) == REG || GET_CODE (operands[0]) == SUBREG)
    dst = gen_rtx (REG, SImode, true_regnum (operands[0]));
  else
    dst = operands[0];
  emit_insn(gen_mfb_cc_br_insn (operands[2], operands[1]));
  emit_insn(gen_move_insn (dst, operands[2]));
  DONE;
}")



(define_insn "mfb_cc_insn"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (ne:SI (match_operand:CC 1 "register_operand" "b")
               (const_int 0)))]
  ""
  "mfb %0 = %1"
  [(set_attr "insn_type" "ALU1")]
)



(define_insn "mfb_cc_br_insn"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (ne:SI (match_operand:CC_BR 1 "register_operand" "b")
               (const_int 0)))]
  ""
  "mfb %0 = %1"
  [(set_attr "insn_type" "ALU1")]
)




;; ::::::::::::::::::::
;; ::
;; :: Select
;; ::
;; ::::::::::::::::::::

(define_expand "movsicc"
  [(set (match_dup 4)
	(match_operator:CC 1 "comparison_operator"
          [(match_dup 5) (match_dup 6)]))
   (set (match_operand:SI 0 "register_operand" "")
        (if_then_else:SI (ne:CC (match_dup 4) (const_int 0))
                         (match_operand:SI 2 "register_operand" "")
                         (match_operand:SI 3 "nonmemory_operand" "")))]
  ""
  "
{
  operands[4] = gen_reg_rtx (CCmode);
  operands[5] = lx_compare_op0;
  operands[6] = lx_compare_op1;
}")



(define_insn "*slct_insn"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (if_then_else:SI (ne:CC (match_operand:CC 1 "register_operand" "b")
                                (const_int 0))
                         (match_operand:SI 2 "register_operand" "r")
                         (match_operand:SI 3 "nonmemory_operand" "ri")))]
  ""
  "slct %0 = %1, %2, %3"
  [(set_attr "insn_type" "ALU3")]
)



(define_insn "slctf_insn"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (if_then_else:SI (eq:CC (match_operand:CC 1 "register_operand" "b")
                                (const_int 0))
                         (match_operand:SI 2 "register_operand" "r")
                         (match_operand:SI 3 "nonmemory_operand" "ri")))]
  ""
  "slctf %0 = %1, %2, %3"
  [(set_attr "insn_type" "ALU3")]
)




;; ::::::::::::::::::::
;; ::
;; :: CMP in GP Regs
;; ::
;; ::::::::::::::::::::

(define_expand "seq"
  [(set (match_operand:SI 0 "register_operand" "")
        (eq:SI (match_dup 1) (match_dup 2)))]
  ""
  "
{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}")



(define_expand "sne"
  [(set (match_operand:SI 0 "register_operand" "")
        (ne:SI (match_dup 1) (match_dup 2)))]
  ""
  "
{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}")



(define_expand "sgt"
  [(set (match_operand:SI 0 "register_operand" "")
        (gt:SI (match_dup 1) (match_dup 2)))]
  ""
  "
{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}")



(define_expand "sgtu"
  [(set (match_operand:SI 0 "register_operand" "")
        (gtu:SI (match_dup 1) (match_dup 2)))]
  ""
  "
{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}")



(define_expand "slt"
  [(set (match_operand:SI 0 "register_operand" "")
        (lt:SI (match_dup 1) (match_dup 2)))]
  ""
  "
{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}")



(define_expand "sltu"
  [(set (match_operand:SI 0 "register_operand" "")
        (ltu:SI (match_dup 1) (match_dup 2)))]
  ""
  "
{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}")



(define_expand "sge"
  [(set (match_operand:SI 0 "register_operand" "")
        (ge:SI (match_dup 1) (match_dup 2)))]
  ""
  "
{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}")



(define_expand "sgeu"
  [(set (match_operand:SI 0 "register_operand" "")
        (geu:SI (match_dup 1) (match_dup 2)))]
  ""
  "
{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}")



(define_expand "sle"
  [(set (match_operand:SI 0 "register_operand" "")
        (le:SI (match_dup 1) (match_dup 2)))]
  ""
  "
{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}")



(define_expand "sleu"
  [(set (match_operand:SI 0 "register_operand" "")
        (leu:SI (match_dup 1) (match_dup 2)))]
  ""
  "
{
  operands[1] = lx_compare_op0;
  operands[2] = lx_compare_op1;
}")




;; ::::::::::::::::::::
;; ::
;; :: Branches
;; ::
;; ::::::::::::::::::::

(define_expand "beq"
  [(set (match_dup 1)
	(eq:CC_BR (match_dup 2)
                  (match_dup 3)))
   (set (pc)
	(if_then_else (ne:CC_BR (match_dup 1) (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "
{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}")



(define_expand "bne"
  [(set (match_dup 1)
	(ne:CC_BR (match_dup 2)
                  (match_dup 3)))
   (set (pc)
	(if_then_else (ne:CC_BR (match_dup 1) (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]

  ""
  "
{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}")



(define_expand "bgt"
  [(set (match_dup 1)
	(gt:CC_BR (match_dup 2)
                  (match_dup 3)))
   (set (pc)
	(if_then_else (ne:CC_BR (match_dup 1) (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "
{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}")



(define_expand "bgtu"
  [(set (match_dup 1)
	(gtu:CC_BR (match_dup 2)
                   (match_dup 3)))
   (set (pc)
	(if_then_else (ne:CC_BR (match_dup 1) (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "
{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}")



(define_expand "blt"
  [(set (match_dup 1)
	(lt:CC_BR (match_dup 2)
                  (match_dup 3)))
   (set (pc)
	(if_then_else (ne:CC_BR (match_dup 1) (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "
{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}")



(define_expand "bltu"
  [(set (match_dup 1)
	(ltu:CC_BR (match_dup 2)
                   (match_dup 3)))
   (set (pc)
	(if_then_else (ne:CC_BR (match_dup 1) (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "
{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}")



(define_expand "bge"
  [(set (match_dup 1)
	(ge:CC_BR (match_dup 2)
                  (match_dup 3)))
   (set (pc)
	(if_then_else (ne:CC_BR (match_dup 1) (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "
{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}")



(define_expand "bgeu"
  [(set (match_dup 1)
	(geu:CC_BR (match_dup 2)
                   (match_dup 3)))
   (set (pc)
	(if_then_else (ne:CC_BR (match_dup 1) (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "
{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}")



(define_expand "ble"
  [(set (match_dup 1)
	(le:CC_BR (match_dup 2)
                  (match_dup 3)))
   (set (pc)
	(if_then_else (ne:CC_BR (match_dup 1) (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "
{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}")



(define_expand "bleu"
  [(set (match_dup 1)
	(leu:CC_BR (match_dup 2)
                   (match_dup 3)))
   (set (pc)
	(if_then_else (ne:CC_BR (match_dup 1) (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "
{
  operands[1] = gen_reg_rtx (CC_BRmode);
  operands[2] = lx_compare_op0;
  operands[3] = lx_compare_op1;
}")



(define_insn "*branch_true"
  [(set (pc)
	(if_then_else (ne:CC_BR (match_operand:CC_BR 0 "register_operand" "b")
                                (const_int 0))
		      (label_ref (match_operand 1 "" ""))
		      (pc)))]
  ""
  "br %0, %1"
  [(set_attr "insn_type" "Branch")]
)



(define_insn "*branch_false"
  [(set (pc)
	(if_then_else (eq:CC_BR (match_operand:CC_BR 0 "register_operand" "b")
                                (const_int 0))
		      (label_ref (match_operand 1 "" ""))
		      (pc)))]
  ""
  "brf %0, %1"
  [(set_attr "insn_type" "Branch")]
)



(define_insn "*branch_false_rev"
  [(set (pc)
	(if_then_else (ne:CC_BR (match_operand:CC_BR 0 "register_operand" "b")
                                (const_int 0))
		      (pc)
		      (label_ref (match_operand 1 "" ""))))]
  ""
  "brf %0, %1"
  [(set_attr "insn_type" "Branch")]
)



(define_insn "*branch_true_rev"
  [(set (pc)
	(if_then_else (eq:CC_BR (match_operand:CC_BR 0 "register_operand" "b")
                                (const_int 0))
		      (pc)
		      (label_ref (match_operand 1 "" ""))))]
  ""
  "br %0, %1"
  [(set_attr "insn_type" "Branch")]
)




;; ::::::::::::::::::::
;; ::
;; :: Jump & Call
;; ::
;; ::::::::::::::::::::

(define_insn "jump"
  [(set (pc) (label_ref (match_operand 0 "" "")))]
  ""
  "goto %0"
  [(set_attr "insn_type" "Jump")]
)



(define_insn "indirect_jump"
  [(set (pc) (match_operand:SI 0 "register_operand" "l"))]
  ""
  "goto %0"
  [(set_attr "insn_type" "Jump")]
)



(define_insn "tablejump"
  [(set (pc) (match_operand:SI 0 "register_operand" "l"))
   (use (label_ref (match_operand 1 "" "")))]
  ""
  "goto %0"
  [(set_attr "insn_type" "Jump")]
)



(define_expand "call"
  [(parallel [(call (match_operand:SI 0 "" "")
                    (match_operand 1 "" ""))
              (clobber (reg:SI 63))])]
  ""
  "
{
  if (MEM_P (operands[0]) && REG_P (XEXP (operands[0], 0)))
    {
      rtx ret_rtx = gen_rtx_REG (SImode, RETURN_POINTER_REGNUM);
      emit_move_insn (ret_rtx, XEXP (operands[0], 0));
      operands[0] = gen_rtx (MEM, SImode, ret_rtx);
    }
}")



(define_insn "*call_internal"
  [(call (mem (match_operand:SI 0 "nonmemory_operand" "li"))
	 (match_operand 1 "" ""))
   (clobber (reg:SI 63))]
  ""
  "call $r0.63 = %0"
  [(set_attr "insn_type" "Call")]
)



(define_expand "call_value"
  [(parallel [(set (match_operand 0 "register_operand" "")
                   (call (match_operand:SI 1 "" "")
                         (match_operand 2 "" "")))
              (clobber (reg:SI 63))])]
  ""
  "
{
  if (MEM_P (operands[1]) && REG_P (XEXP (operands[1], 0)))
    {
      rtx ret_rtx = gen_rtx_REG (SImode, RETURN_POINTER_REGNUM);
      emit_move_insn (ret_rtx, XEXP (operands[1], 0));
      operands[1] = gen_rtx (MEM, SImode, ret_rtx);
    }
}")



(define_insn "*call_value_internal"
  [(parallel [(set (match_operand 0 "register_operand" "=r")
                   (call (mem (match_operand:SI 1 "nonmemory_operand" "li"))
                         (match_operand 2 "" "")))
              (clobber (reg:SI 63))])]
  ""
  "call $r0.63 = %1"
  [(set_attr "insn_type" "Call")]
)



(define_insn "return"
  [(parallel [(return)
	      (use (reg:SI 63))])]
  "lx_direct_return ()"
  "return $r0.63"
  [(set_attr "insn_type" "Return")]
)



(define_expand "prologue"
  [(const_int 1)]
  ""
  "
{
  lx_expand_prologue ();
  DONE;
}")



(define_expand "epilogue"
  [(const_int 2)]
  ""
  "
{
  lx_expand_epilogue ();
  DONE;
}")





;; ::::::::::::::::::::
;; ::
;; :: Miscellaneous instructions
;; ::
;; ::::::::::::::::::::

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop"
  [(set_attr "insn_type" "Misc")]
)



(define_insn "bundle_barrier"
  [(unspec_volatile [(const_int 0)] 2)]
  ""
  ";;"
  [(set_attr "insn_type" "BundleStop")]
)
