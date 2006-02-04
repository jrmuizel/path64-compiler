/* Generated automatically by the program `genflags'
   from the machine description file `md'.  */

#ifndef GCC_INSN_FLAGS_H
#define GCC_INSN_FLAGS_H

#define HAVE_addcg_insn 1
#define HAVE_addsi3 1
#define HAVE_subsi3 1
#define HAVE_sminsi3 1
#define HAVE_smaxsi3 1
#define HAVE_uminsi3 1
#define HAVE_umaxsi3 1
#define HAVE_andsi3 1
#define HAVE_iorsi3 1
#define HAVE_xorsi3 1
#define HAVE_ashlsi3 1
#define HAVE_ashrsi3 1
#define HAVE_lshrsi3 1
#define HAVE_negsi2 1
#define HAVE_one_cmplsi2 1
#define HAVE_dummy_mulsi3 1
#define HAVE_extendhisi2 1
#define HAVE_extendqisi2 1
#define HAVE_ldhu_insn 1
#define HAVE_zero_extendhiqi2 1
#define HAVE_cmpcc_insn 1
#define HAVE_cmpcc_br_insn 1
#define HAVE_mfb_cc_insn 1
#define HAVE_mfb_cc_br_insn 1
#define HAVE_slctf_insn 1
#define HAVE_jump 1
#define HAVE_indirect_jump 1
#define HAVE_tablejump 1
#define HAVE_return (lx_direct_return ())
#define HAVE_nop 1
#define HAVE_bundle_barrier 1
#define HAVE_movsi 1
#define HAVE_movhi 1
#define HAVE_movqi 1
#define HAVE_movsf 1
#define HAVE_adddi3 1
#define HAVE_abssi2 1
#define HAVE_mulsi3 1
#define HAVE_zero_extendhisi2 1
#define HAVE_absdf2 1
#define HAVE_abssf2 1
#define HAVE_negsf2 1
#define HAVE_negdf2 1
#define HAVE_cmpsi 1
#define HAVE_reload_incc 1
#define HAVE_reload_incc_br 1
#define HAVE_reload_outcc 1
#define HAVE_reload_outcc_br 1
#define HAVE_movsicc 1
#define HAVE_seq 1
#define HAVE_sne 1
#define HAVE_sgt 1
#define HAVE_sgtu 1
#define HAVE_slt 1
#define HAVE_sltu 1
#define HAVE_sge 1
#define HAVE_sgeu 1
#define HAVE_sle 1
#define HAVE_sleu 1
#define HAVE_beq 1
#define HAVE_bne 1
#define HAVE_bgt 1
#define HAVE_bgtu 1
#define HAVE_blt 1
#define HAVE_bltu 1
#define HAVE_bge 1
#define HAVE_bgeu 1
#define HAVE_ble 1
#define HAVE_bleu 1
#define HAVE_call 1
#define HAVE_call_value 1
#define HAVE_prologue 1
#define HAVE_epilogue 1
extern rtx        gen_addcg_insn       PARAMS ((rtx, rtx, rtx, rtx, rtx));
extern rtx        gen_addsi3           PARAMS ((rtx, rtx, rtx));
extern rtx        gen_subsi3           PARAMS ((rtx, rtx, rtx));
extern rtx        gen_sminsi3          PARAMS ((rtx, rtx, rtx));
extern rtx        gen_smaxsi3          PARAMS ((rtx, rtx, rtx));
extern rtx        gen_uminsi3          PARAMS ((rtx, rtx, rtx));
extern rtx        gen_umaxsi3          PARAMS ((rtx, rtx, rtx));
extern rtx        gen_andsi3           PARAMS ((rtx, rtx, rtx));
extern rtx        gen_iorsi3           PARAMS ((rtx, rtx, rtx));
extern rtx        gen_xorsi3           PARAMS ((rtx, rtx, rtx));
extern rtx        gen_ashlsi3          PARAMS ((rtx, rtx, rtx));
extern rtx        gen_ashrsi3          PARAMS ((rtx, rtx, rtx));
extern rtx        gen_lshrsi3          PARAMS ((rtx, rtx, rtx));
extern rtx        gen_negsi2           PARAMS ((rtx, rtx));
extern rtx        gen_one_cmplsi2      PARAMS ((rtx, rtx));
extern rtx        gen_dummy_mulsi3     PARAMS ((rtx, rtx, rtx, rtx, rtx));
extern rtx        gen_extendhisi2      PARAMS ((rtx, rtx));
extern rtx        gen_extendqisi2      PARAMS ((rtx, rtx));
extern rtx        gen_ldhu_insn        PARAMS ((rtx, rtx));
extern rtx        gen_zero_extendhiqi2 PARAMS ((rtx, rtx));
extern rtx        gen_cmpcc_insn       PARAMS ((rtx, rtx, rtx, rtx));
extern rtx        gen_cmpcc_br_insn    PARAMS ((rtx, rtx, rtx, rtx));
extern rtx        gen_mfb_cc_insn      PARAMS ((rtx, rtx));
extern rtx        gen_mfb_cc_br_insn   PARAMS ((rtx, rtx));
extern rtx        gen_slctf_insn       PARAMS ((rtx, rtx, rtx, rtx));
extern rtx        gen_jump             PARAMS ((rtx));
extern rtx        gen_indirect_jump    PARAMS ((rtx));
extern rtx        gen_tablejump        PARAMS ((rtx, rtx));
extern rtx        gen_return           PARAMS ((void));
extern rtx        gen_nop              PARAMS ((void));
extern rtx        gen_bundle_barrier   PARAMS ((void));
extern rtx        gen_movsi            PARAMS ((rtx, rtx));
extern rtx        gen_movhi            PARAMS ((rtx, rtx));
extern rtx        gen_movqi            PARAMS ((rtx, rtx));
extern rtx        gen_movsf            PARAMS ((rtx, rtx));
extern rtx        gen_adddi3           PARAMS ((rtx, rtx, rtx));
extern rtx        gen_abssi2           PARAMS ((rtx, rtx));
extern rtx        gen_mulsi3           PARAMS ((rtx, rtx, rtx));
extern rtx        gen_zero_extendhisi2 PARAMS ((rtx, rtx));
extern rtx        gen_absdf2           PARAMS ((rtx, rtx));
extern rtx        gen_abssf2           PARAMS ((rtx, rtx));
extern rtx        gen_negsf2           PARAMS ((rtx, rtx));
extern rtx        gen_negdf2           PARAMS ((rtx, rtx));
extern rtx        gen_cmpsi            PARAMS ((rtx, rtx));
extern rtx        gen_reload_incc      PARAMS ((rtx, rtx, rtx));
extern rtx        gen_reload_incc_br   PARAMS ((rtx, rtx, rtx));
extern rtx        gen_reload_outcc     PARAMS ((rtx, rtx, rtx));
extern rtx        gen_reload_outcc_br  PARAMS ((rtx, rtx, rtx));
extern rtx        gen_movsicc          PARAMS ((rtx, rtx, rtx, rtx));
extern rtx        gen_seq              PARAMS ((rtx));
extern rtx        gen_sne              PARAMS ((rtx));
extern rtx        gen_sgt              PARAMS ((rtx));
extern rtx        gen_sgtu             PARAMS ((rtx));
extern rtx        gen_slt              PARAMS ((rtx));
extern rtx        gen_sltu             PARAMS ((rtx));
extern rtx        gen_sge              PARAMS ((rtx));
extern rtx        gen_sgeu             PARAMS ((rtx));
extern rtx        gen_sle              PARAMS ((rtx));
extern rtx        gen_sleu             PARAMS ((rtx));
extern rtx        gen_beq              PARAMS ((rtx));
extern rtx        gen_bne              PARAMS ((rtx));
extern rtx        gen_bgt              PARAMS ((rtx));
extern rtx        gen_bgtu             PARAMS ((rtx));
extern rtx        gen_blt              PARAMS ((rtx));
extern rtx        gen_bltu             PARAMS ((rtx));
extern rtx        gen_bge              PARAMS ((rtx));
extern rtx        gen_bgeu             PARAMS ((rtx));
extern rtx        gen_ble              PARAMS ((rtx));
extern rtx        gen_bleu             PARAMS ((rtx));
#define GEN_CALL(A, B, C, D) gen_call ((A), (B))
extern rtx        gen_call             PARAMS ((rtx, rtx));
#define GEN_CALL_VALUE(A, B, C, D, E) gen_call_value ((A), (B), (C))
extern rtx        gen_call_value       PARAMS ((rtx, rtx, rtx));
extern rtx        gen_prologue         PARAMS ((void));
extern rtx        gen_epilogue         PARAMS ((void));

#endif /* GCC_INSN_FLAGS_H */
