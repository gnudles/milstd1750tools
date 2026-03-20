#include "jit.h"
#include "cpu_ctx.h"
#include "cpu_helpers.h"
#include "sljit/sljit_src/sljitLir.h"
#include <stddef.h>

void emit_jit_LB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_DLB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_STB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement STB in sljit
}

void emit_jit_DSTB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DSTB in sljit
}

void emit_jit_AB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_SBB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_MB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement MB in sljit
}

void emit_jit_DB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DB in sljit
}

void emit_jit_FAB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FAB in sljit
}

void emit_jit_FSB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FSB in sljit
}

void emit_jit_FMB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FMB in sljit
}

void emit_jit_FDB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FDB in sljit
}

void emit_jit_ORB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_ANDB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_CB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement CB in sljit
}

void emit_jit_FCB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FCB in sljit
}

void emit_jit_LBX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_DLBX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_STBX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement STBX in sljit
}

void emit_jit_DSTX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DSTX in sljit
}

void emit_jit_ABX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_SBBX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_MBX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement MBX in sljit
}

void emit_jit_DBX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DBX in sljit
}

void emit_jit_FABX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FABX in sljit
}

void emit_jit_FSBX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FSBX in sljit
}

void emit_jit_FMBX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FMBX in sljit
}

void emit_jit_FDBX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FDBX in sljit
}

void emit_jit_CBX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement CBX in sljit
}

void emit_jit_FCBX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FCBX in sljit
}

void emit_jit_ANDX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_ORBX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_AIM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_IMM, imm);
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_SIM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_IMM, imm);
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_MIM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement MIM in sljit
}

void emit_jit_MSIM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement MSIM in sljit
}

void emit_jit_DIM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DIM in sljit
}

void emit_jit_DVIM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DVIM in sljit
}

void emit_jit_ANDM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_IMM, imm);
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_ORIM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_IMM, imm);
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_XORM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_IMM, imm);
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_CIM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement CIM in sljit
}

void emit_jit_NIM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_IMM, imm);
    // TODO: implement NIM in sljit
}

void emit_jit_XIO(struct sljit_compiler *compiler, uint16_t rx, uint16_t ra, uint16_t imm, bool emit_flags) {
    if (rx == 0) {
        // Emit sljit for internal XIO read
        if (imm == 0xA000) {
            sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.mk));
        } else if (imm == 0xA004) {
            sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.pir));
        } else if (imm == 0xA00E) {
            sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.sw));
        } else if ((imm & 0xF000) == 0xD000) {
            // 51XY (read is D1XY), 52XY (read is D2XY). Page register reads
            sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
            sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);
            sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R2, 0, SLJIT_S0, 0, SLJIT_IMM, offsetof(struct cpu_context, state.reg.r) + ra * 2);
            sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(process_xio));
        } else {
            // Fallback for other XIO rx=0
            sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
            sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);
            sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R2, 0, SLJIT_S0, 0, SLJIT_IMM, offsetof(struct cpu_context, state.reg.r) + ra * 2);
            sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(process_xio));
        }
    }
}

void emit_jit_ESQR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement ESQR in sljit
}

void emit_jit_SQRT(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SQRT in sljit
}

void emit_jit_SB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SB in sljit
}

void emit_jit_SBR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SBR in sljit
}

void emit_jit_SBI(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SBI in sljit
}

void emit_jit_RB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement RB in sljit
}

void emit_jit_RBR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement RBR in sljit
}

void emit_jit_RBI(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement RBI in sljit
}

void emit_jit_TB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement TB in sljit
}

void emit_jit_TBR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement TBR in sljit
}

void emit_jit_TBI(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement TBI in sljit
}

void emit_jit_TSB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement TSB in sljit
}

void emit_jit_SVBR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SVBR in sljit
}

void emit_jit_RVBR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement RVBR in sljit
}

void emit_jit_TVBR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement TVBR in sljit
}

void emit_jit_SLL(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SLL in sljit
}

void emit_jit_SRL(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SRL in sljit
}

void emit_jit_SRA(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SRA in sljit
}

void emit_jit_SLC(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SLC in sljit
}

void emit_jit_DSLL(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DSLL in sljit
}

void emit_jit_DSRL(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DSRL in sljit
}

void emit_jit_DSRA(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DSRA in sljit
}

void emit_jit_DSLC(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DSLC in sljit
}

void emit_jit_SLR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SLR in sljit
}

void emit_jit_SAR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SAR in sljit
}

void emit_jit_SCR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SCR in sljit
}

void emit_jit_DSLR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DSLR in sljit
}

void emit_jit_DSAR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DSAR in sljit
}

void emit_jit_DSCR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DSCR in sljit
}

void emit_jit_JC(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement JC in sljit
}

void emit_jit_SOJ(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SOJ in sljit
}

void emit_jit_BR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement BR in sljit
}

void emit_jit_BEZ(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement BEZ in sljit
}

void emit_jit_BLT(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement BLT in sljit
}

void emit_jit_BLE(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement BLE in sljit
}

void emit_jit_BGT(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement BGT in sljit
}

void emit_jit_BNZ(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement BNZ in sljit
}

void emit_jit_BGE(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement BGE in sljit
}

void emit_jit_SJS(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SJS in sljit
}

void emit_jit_URS(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement URS in sljit
}

void emit_jit_L(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Setup R0=cpu_ctx, R1=addr, R2=&dst
    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
    // Calculate Effective Address: addr = imm + (RX > 0 ? R[RX] : 0)
    if (rb > 0) {
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
        sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, imm);
    } else {
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);
    }
    sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R2, 0, SLJIT_S0, 0, SLJIT_IMM, offsetof(struct cpu_context, state.reg.r) + ra * 2);
    sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(fetch_data_word));
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_LR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Load RB into SLJIT_R1
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
    // LR rA, rB
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2, SLJIT_R1, 0);
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_LISP(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_LISN(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_LI(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Setup R0=cpu_ctx, R1=addr, R2=&dst
    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
    // Calculate Effective Address: addr = imm + (RX > 0 ? R[RX] : 0)
    if (rb > 0) {
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
        sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, imm);
    } else {
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);
    }
    sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R2, 0, SLJIT_S0, 0, SLJIT_IMM, offsetof(struct cpu_context, state.reg.r) + ra * 2);
    sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(fetch_data_word));
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_LIM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Setup R0=cpu_ctx, R1=addr, R2=&dst
    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
    // Calculate Effective Address: addr = imm + (RX > 0 ? R[RX] : 0)
    if (rb > 0) {
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
        sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, imm);
    } else {
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);
    }
    sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R2, 0, SLJIT_S0, 0, SLJIT_IMM, offsetof(struct cpu_context, state.reg.r) + ra * 2);
    sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(fetch_data_word));
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_DL(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Setup R0=cpu_ctx, R1=addr, R2=&dst
    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
    // Calculate Effective Address: addr = imm + (RX > 0 ? R[RX] : 0)
    if (rb > 0) {
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
        sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, imm);
    } else {
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);
    }
    sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R2, 0, SLJIT_S0, 0, SLJIT_IMM, offsetof(struct cpu_context, state.reg.r) + ra * 2);
    sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(fetch_data_word));
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_DLR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Load RB (32bit) into SLJIT_R1
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
    sljit_emit_op2(compiler, SLJIT_SHL, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, 16);
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ((rb + 1) & 0xF) * 2);
    sljit_emit_op2(compiler, SLJIT_OR, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_R2, 0);
    // LR rA, rB
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2, SLJIT_R1, 0);
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_DLI(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Setup R0=cpu_ctx, R1=addr, R2=&dst
    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
    // Calculate Effective Address: addr = imm + (RX > 0 ? R[RX] : 0)
    if (rb > 0) {
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
        sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, imm);
    } else {
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);
    }
    sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R2, 0, SLJIT_S0, 0, SLJIT_IMM, offsetof(struct cpu_context, state.reg.r) + ra * 2);
    sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(fetch_data_word));
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_LM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement LM in sljit
}

void emit_jit_EFL(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Setup R0=cpu_ctx, R1=addr, R2=&dst
    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
    // Calculate Effective Address: addr = imm + (RX > 0 ? R[RX] : 0)
    if (rb > 0) {
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
        sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, imm);
    } else {
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);
    }
    sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R2, 0, SLJIT_S0, 0, SLJIT_IMM, offsetof(struct cpu_context, state.reg.r) + ra * 2);
    sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(fetch_data_word));
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_LUB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement LUB in sljit
}

void emit_jit_LLB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement LLB in sljit
}

void emit_jit_LUBI(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement LUBI in sljit
}

void emit_jit_LLBI(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement LLBI in sljit
}

void emit_jit_POPM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement POPM in sljit
}

void emit_jit_ST(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Setup R0=cpu_ctx, R1=addr, R2=val
    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
    // Calculate Effective Address: addr = imm + (RX > 0 ? R[RX] : 0)
    if (rb > 0) {
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
        sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, imm);
    } else {
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);
    }
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
    sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(store_data_word));
    // TODO: implement ST in sljit
}

void emit_jit_STC(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement STC in sljit
}

void emit_jit_STCI(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement STCI in sljit
}

void emit_jit_MOV(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement MOV in sljit
}

void emit_jit_STI(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Setup R0=cpu_ctx, R1=addr, R2=val
    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
    // Calculate Effective Address: addr = imm + (RX > 0 ? R[RX] : 0)
    if (rb > 0) {
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
        sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, imm);
    } else {
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);
    }
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
    sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(store_data_word));
    // TODO: implement STI in sljit
}

void emit_jit_SFBS(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SFBS in sljit
}

void emit_jit_DST(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Setup R0=cpu_ctx, R1=addr, R2=val
    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
    // Calculate Effective Address: addr = imm + (RX > 0 ? R[RX] : 0)
    if (rb > 0) {
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
        sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, imm);
    } else {
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);
    }
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
    sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(store_data_word));
    // TODO: implement DST in sljit
}

void emit_jit_SRM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SRM in sljit
}

void emit_jit_DSTI(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Setup R0=cpu_ctx, R1=addr, R2=val
    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
    // Calculate Effective Address: addr = imm + (RX > 0 ? R[RX] : 0)
    if (rb > 0) {
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
        sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, imm);
    } else {
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);
    }
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
    sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(store_data_word));
    // TODO: implement DSTI in sljit
}

void emit_jit_STM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement STM in sljit
}

void emit_jit_EFST(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Setup R0=cpu_ctx, R1=addr, R2=val
    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
    // Calculate Effective Address: addr = imm + (RX > 0 ? R[RX] : 0)
    if (rb > 0) {
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
        sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, imm);
    } else {
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);
    }
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
    sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(store_data_word));
    // TODO: implement EFST in sljit
}

void emit_jit_STUB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement STUB in sljit
}

void emit_jit_STLB(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement STLB in sljit
}

void emit_jit_SUBI(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SUBI in sljit
}

void emit_jit_SLBI(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement SLBI in sljit
}

void emit_jit_PSHM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement PSHM in sljit
}

void emit_jit_A(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_AR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Load RB into SLJIT_R1
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
    // AR rA, rB
    sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,
                   SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,
                   SLJIT_R1, 0);
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_AISP(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_INCM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement INCM in sljit
}

void emit_jit_ABS(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement ABS in sljit
}

void emit_jit_DABS(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DABS in sljit
}

void emit_jit_DA(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_DAR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Load RB (32bit) into SLJIT_R1
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
    sljit_emit_op2(compiler, SLJIT_SHL, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, 16);
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ((rb + 1) & 0xF) * 2);
    sljit_emit_op2(compiler, SLJIT_OR, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_R2, 0);
    // AR rA, rB
    sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,
                   SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,
                   SLJIT_R1, 0);
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_FA(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FA in sljit
}

void emit_jit_FAR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FAR in sljit
}

void emit_jit_EFA(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement EFA in sljit
}

void emit_jit_EFAR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement EFAR in sljit
}

void emit_jit_FABS(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FABS in sljit
}

void emit_jit_UAR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement UAR in sljit
}

void emit_jit_UA(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement UA in sljit
}

void emit_jit_S(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_SR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Load RB into SLJIT_R1
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
    // SR rA, rB
    sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,
                   SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,
                   SLJIT_R1, 0);
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_SISP(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_DECM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DECM in sljit
}

void emit_jit_NEG(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement NEG in sljit
}

void emit_jit_DNEG(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DNEG in sljit
}

void emit_jit_DS(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_DSR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Load RB (32bit) into SLJIT_R1
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
    sljit_emit_op2(compiler, SLJIT_SHL, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, 16);
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ((rb + 1) & 0xF) * 2);
    sljit_emit_op2(compiler, SLJIT_OR, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_R2, 0);
    // SR rA, rB
    sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,
                   SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,
                   SLJIT_R1, 0);
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_FS(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FS in sljit
}

void emit_jit_FSR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FSR in sljit
}

void emit_jit_EFS(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement EFS in sljit
}

void emit_jit_EFSR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement EFSR in sljit
}

void emit_jit_FNEG(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FNEG in sljit
}

void emit_jit_USR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement USR in sljit
}

void emit_jit_US(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement US in sljit
}

void emit_jit_MS(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement MS in sljit
}

void emit_jit_MSR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement MSR in sljit
}

void emit_jit_MISP(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement MISP in sljit
}

void emit_jit_MISN(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement MISN in sljit
}

void emit_jit_M(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement M in sljit
}

void emit_jit_MR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement MR in sljit
}

void emit_jit_DM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DM in sljit
}

void emit_jit_DMR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DMR in sljit
}

void emit_jit_FM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FM in sljit
}

void emit_jit_FMR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FMR in sljit
}

void emit_jit_EFM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement EFM in sljit
}

void emit_jit_EFMR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement EFMR in sljit
}

void emit_jit_DV(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DV in sljit
}

void emit_jit_DVR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DVR in sljit
}

void emit_jit_DISP(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DISP in sljit
}

void emit_jit_DISN(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DISN in sljit
}

void emit_jit_D(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement D in sljit
}

void emit_jit_DR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DR in sljit
}

void emit_jit_DD(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DD in sljit
}

void emit_jit_DDR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DDR in sljit
}

void emit_jit_FD(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FD in sljit
}

void emit_jit_FDR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FDR in sljit
}

void emit_jit_EFD(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement EFD in sljit
}

void emit_jit_EFDR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement EFDR in sljit
}

void emit_jit_STE(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement STE in sljit
}

void emit_jit_DSTE(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DSTE in sljit
}

void emit_jit_LE(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement LE in sljit
}

void emit_jit_DLE(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DLE in sljit
}

void emit_jit_OR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_ORR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Load RB into SLJIT_R1
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
    // ORR rA, rB
    sljit_emit_op2(compiler, SLJIT_OR, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,
                   SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,
                   SLJIT_R1, 0);
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_AND(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_ANDR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Load RB into SLJIT_R1
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
    // ANDR rA, rB
    sljit_emit_op2(compiler, SLJIT_AND, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,
                   SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,
                   SLJIT_R1, 0);
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_XOR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_XORR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Load RB into SLJIT_R1
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
    // XORR rA, rB
    sljit_emit_op2(compiler, SLJIT_XOR, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,
                   SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,
                   SLJIT_R1, 0);
    if (emit_flags) {
        // Call calculate_flags_16bit(cpu_ctx, R[RA])
        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);
        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));
    }
}

void emit_jit_N(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement N in sljit
}

void emit_jit_NR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // Load RB into SLJIT_R1
    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);
    // TODO: implement NR in sljit
}

void emit_jit_FIX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FIX in sljit
}

void emit_jit_FLT(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FLT in sljit
}

void emit_jit_EFIX(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement EFIX in sljit
}

void emit_jit_EFLT(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement EFLT in sljit
}

void emit_jit_XBR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement XBR in sljit
}

void emit_jit_XWR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement XWR in sljit
}

void emit_jit_C(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement C in sljit
}

void emit_jit_CR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement CR in sljit
}

void emit_jit_CISP(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement CISP in sljit
}

void emit_jit_CISN(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement CISN in sljit
}

void emit_jit_CBL(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement CBL in sljit
}

void emit_jit_UCIM(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement UCIM in sljit
}

void emit_jit_DC(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DC in sljit
}

void emit_jit_DCR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement DCR in sljit
}

void emit_jit_FC(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FC in sljit
}

void emit_jit_FCR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement FCR in sljit
}

void emit_jit_EFC(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement EFC in sljit
}

void emit_jit_EFCR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement EFCR in sljit
}

void emit_jit_UCR(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement UCR in sljit
}

void emit_jit_UC(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement UC in sljit
}

void emit_jit_NOP_BPT(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {
    // TODO: implement NOP_BPT in sljit
}

void compile_jit_instruction(struct sljit_compiler *compiler, OpcodeDef *def, uint16_t opcode, uint16_t imm, bool emit_flags) {
    uint16_t ra = (opcode >> 4) & 0xF;
    uint16_t rb = opcode & 0xF;
    switch (def->code) {
        case 0: emit_jit_LB(compiler, ra, rb, imm, emit_flags); break;
        case 4: emit_jit_DLB(compiler, ra, rb, imm, emit_flags); break;
        case 8: emit_jit_STB(compiler, ra, rb, imm, emit_flags); break;
        case 12: emit_jit_DSTB(compiler, ra, rb, imm, emit_flags); break;
        case 16: emit_jit_AB(compiler, ra, rb, imm, emit_flags); break;
        case 20: emit_jit_SBB(compiler, ra, rb, imm, emit_flags); break;
        case 24: emit_jit_MB(compiler, ra, rb, imm, emit_flags); break;
        case 28: emit_jit_DB(compiler, ra, rb, imm, emit_flags); break;
        case 32: emit_jit_FAB(compiler, ra, rb, imm, emit_flags); break;
        case 36: emit_jit_FSB(compiler, ra, rb, imm, emit_flags); break;
        case 40: emit_jit_FMB(compiler, ra, rb, imm, emit_flags); break;
        case 44: emit_jit_FDB(compiler, ra, rb, imm, emit_flags); break;
        case 48: emit_jit_ORB(compiler, ra, rb, imm, emit_flags); break;
        case 52: emit_jit_ANDB(compiler, ra, rb, imm, emit_flags); break;
        case 56: emit_jit_CB(compiler, ra, rb, imm, emit_flags); break;
        case 60: emit_jit_FCB(compiler, ra, rb, imm, emit_flags); break;
        case 16384: emit_jit_LBX(compiler, ra, rb, imm, emit_flags); break;
        case 16400: emit_jit_DLBX(compiler, ra, rb, imm, emit_flags); break;
        case 16416: emit_jit_STBX(compiler, ra, rb, imm, emit_flags); break;
        case 16432: emit_jit_DSTX(compiler, ra, rb, imm, emit_flags); break;
        case 16448: emit_jit_ABX(compiler, ra, rb, imm, emit_flags); break;
        case 16464: emit_jit_SBBX(compiler, ra, rb, imm, emit_flags); break;
        case 16480: emit_jit_MBX(compiler, ra, rb, imm, emit_flags); break;
        case 16496: emit_jit_DBX(compiler, ra, rb, imm, emit_flags); break;
        case 16512: emit_jit_FABX(compiler, ra, rb, imm, emit_flags); break;
        case 16528: emit_jit_FSBX(compiler, ra, rb, imm, emit_flags); break;
        case 16544: emit_jit_FMBX(compiler, ra, rb, imm, emit_flags); break;
        case 16560: emit_jit_FDBX(compiler, ra, rb, imm, emit_flags); break;
        case 16576: emit_jit_CBX(compiler, ra, rb, imm, emit_flags); break;
        case 16592: emit_jit_FCBX(compiler, ra, rb, imm, emit_flags); break;
        case 16608: emit_jit_ANDX(compiler, ra, rb, imm, emit_flags); break;
        case 16624: emit_jit_ORBX(compiler, ra, rb, imm, emit_flags); break;
        case 18945: emit_jit_AIM(compiler, ra, rb, imm, emit_flags); break;
        case 18946: emit_jit_SIM(compiler, ra, rb, imm, emit_flags); break;
        case 18947: emit_jit_MIM(compiler, ra, rb, imm, emit_flags); break;
        case 18948: emit_jit_MSIM(compiler, ra, rb, imm, emit_flags); break;
        case 18949: emit_jit_DIM(compiler, ra, rb, imm, emit_flags); break;
        case 18950: emit_jit_DVIM(compiler, ra, rb, imm, emit_flags); break;
        case 18951: emit_jit_ANDM(compiler, ra, rb, imm, emit_flags); break;
        case 18952: emit_jit_ORIM(compiler, ra, rb, imm, emit_flags); break;
        case 18953: emit_jit_XORM(compiler, ra, rb, imm, emit_flags); break;
        case 18954: emit_jit_CIM(compiler, ra, rb, imm, emit_flags); break;
        case 18955: emit_jit_NIM(compiler, ra, rb, imm, emit_flags); break;
        case 72: emit_jit_XIO(compiler, opcode & 0xF, (opcode >> 4) & 0xF, imm, emit_flags); break;
        case 77: emit_jit_ESQR(compiler, ra, rb, imm, emit_flags); break;
        case 78: emit_jit_SQRT(compiler, ra, rb, imm, emit_flags); break;
        case 80: emit_jit_SB(compiler, ra, rb, imm, emit_flags); break;
        case 81: emit_jit_SBR(compiler, ra, rb, imm, emit_flags); break;
        case 82: emit_jit_SBI(compiler, ra, rb, imm, emit_flags); break;
        case 83: emit_jit_RB(compiler, ra, rb, imm, emit_flags); break;
        case 84: emit_jit_RBR(compiler, ra, rb, imm, emit_flags); break;
        case 85: emit_jit_RBI(compiler, ra, rb, imm, emit_flags); break;
        case 86: emit_jit_TB(compiler, ra, rb, imm, emit_flags); break;
        case 87: emit_jit_TBR(compiler, ra, rb, imm, emit_flags); break;
        case 88: emit_jit_TBI(compiler, ra, rb, imm, emit_flags); break;
        case 89: emit_jit_TSB(compiler, ra, rb, imm, emit_flags); break;
        case 90: emit_jit_SVBR(compiler, ra, rb, imm, emit_flags); break;
        case 92: emit_jit_RVBR(compiler, ra, rb, imm, emit_flags); break;
        case 94: emit_jit_TVBR(compiler, ra, rb, imm, emit_flags); break;
        case 96: emit_jit_SLL(compiler, ra, rb, imm, emit_flags); break;
        case 97: emit_jit_SRL(compiler, ra, rb, imm, emit_flags); break;
        case 98: emit_jit_SRA(compiler, ra, rb, imm, emit_flags); break;
        case 99: emit_jit_SLC(compiler, ra, rb, imm, emit_flags); break;
        case 101: emit_jit_DSLL(compiler, ra, rb, imm, emit_flags); break;
        case 102: emit_jit_DSRL(compiler, ra, rb, imm, emit_flags); break;
        case 103: emit_jit_DSRA(compiler, ra, rb, imm, emit_flags); break;
        case 104: emit_jit_DSLC(compiler, ra, rb, imm, emit_flags); break;
        case 106: emit_jit_SLR(compiler, ra, rb, imm, emit_flags); break;
        case 107: emit_jit_SAR(compiler, ra, rb, imm, emit_flags); break;
        case 108: emit_jit_SCR(compiler, ra, rb, imm, emit_flags); break;
        case 109: emit_jit_DSLR(compiler, ra, rb, imm, emit_flags); break;
        case 110: emit_jit_DSAR(compiler, ra, rb, imm, emit_flags); break;
        case 111: emit_jit_DSCR(compiler, ra, rb, imm, emit_flags); break;
        case 112: emit_jit_JC(compiler, ra, rb, imm, emit_flags); break;
        case 115: emit_jit_SOJ(compiler, ra, rb, imm, emit_flags); break;
        case 116: emit_jit_BR(compiler, ra, rb, imm, emit_flags); break;
        case 117: emit_jit_BEZ(compiler, ra, rb, imm, emit_flags); break;
        case 118: emit_jit_BLT(compiler, ra, rb, imm, emit_flags); break;
        case 120: emit_jit_BLE(compiler, ra, rb, imm, emit_flags); break;
        case 121: emit_jit_BGT(compiler, ra, rb, imm, emit_flags); break;
        case 122: emit_jit_BNZ(compiler, ra, rb, imm, emit_flags); break;
        case 123: emit_jit_BGE(compiler, ra, rb, imm, emit_flags); break;
        case 126: emit_jit_SJS(compiler, ra, rb, imm, emit_flags); break;
        case 127: emit_jit_URS(compiler, ra, rb, imm, emit_flags); break;
        case 128: emit_jit_L(compiler, ra, rb, imm, emit_flags); break;
        case 129: emit_jit_LR(compiler, ra, rb, imm, emit_flags); break;
        case 130: emit_jit_LISP(compiler, ra, rb, imm, emit_flags); break;
        case 131: emit_jit_LISN(compiler, ra, rb, imm, emit_flags); break;
        case 132: emit_jit_LI(compiler, ra, rb, imm, emit_flags); break;
        case 133: emit_jit_LIM(compiler, ra, rb, imm, emit_flags); break;
        case 134: emit_jit_DL(compiler, ra, rb, imm, emit_flags); break;
        case 135: emit_jit_DLR(compiler, ra, rb, imm, emit_flags); break;
        case 136: emit_jit_DLI(compiler, ra, rb, imm, emit_flags); break;
        case 137: emit_jit_LM(compiler, ra, rb, imm, emit_flags); break;
        case 138: emit_jit_EFL(compiler, ra, rb, imm, emit_flags); break;
        case 139: emit_jit_LUB(compiler, ra, rb, imm, emit_flags); break;
        case 140: emit_jit_LLB(compiler, ra, rb, imm, emit_flags); break;
        case 141: emit_jit_LUBI(compiler, ra, rb, imm, emit_flags); break;
        case 142: emit_jit_LLBI(compiler, ra, rb, imm, emit_flags); break;
        case 143: emit_jit_POPM(compiler, ra, rb, imm, emit_flags); break;
        case 144: emit_jit_ST(compiler, ra, rb, imm, emit_flags); break;
        case 145: emit_jit_STC(compiler, ra, rb, imm, emit_flags); break;
        case 146: emit_jit_STCI(compiler, ra, rb, imm, emit_flags); break;
        case 147: emit_jit_MOV(compiler, ra, rb, imm, emit_flags); break;
        case 148: emit_jit_STI(compiler, ra, rb, imm, emit_flags); break;
        case 149: emit_jit_SFBS(compiler, ra, rb, imm, emit_flags); break;
        case 150: emit_jit_DST(compiler, ra, rb, imm, emit_flags); break;
        case 151: emit_jit_SRM(compiler, ra, rb, imm, emit_flags); break;
        case 152: emit_jit_DSTI(compiler, ra, rb, imm, emit_flags); break;
        case 153: emit_jit_STM(compiler, ra, rb, imm, emit_flags); break;
        case 154: emit_jit_EFST(compiler, ra, rb, imm, emit_flags); break;
        case 155: emit_jit_STUB(compiler, ra, rb, imm, emit_flags); break;
        case 156: emit_jit_STLB(compiler, ra, rb, imm, emit_flags); break;
        case 157: emit_jit_SUBI(compiler, ra, rb, imm, emit_flags); break;
        case 158: emit_jit_SLBI(compiler, ra, rb, imm, emit_flags); break;
        case 159: emit_jit_PSHM(compiler, ra, rb, imm, emit_flags); break;
        case 160: emit_jit_A(compiler, ra, rb, imm, emit_flags); break;
        case 161: emit_jit_AR(compiler, ra, rb, imm, emit_flags); break;
        case 162: emit_jit_AISP(compiler, ra, rb, imm, emit_flags); break;
        case 163: emit_jit_INCM(compiler, ra, rb, imm, emit_flags); break;
        case 164: emit_jit_ABS(compiler, ra, rb, imm, emit_flags); break;
        case 165: emit_jit_DABS(compiler, ra, rb, imm, emit_flags); break;
        case 166: emit_jit_DA(compiler, ra, rb, imm, emit_flags); break;
        case 167: emit_jit_DAR(compiler, ra, rb, imm, emit_flags); break;
        case 168: emit_jit_FA(compiler, ra, rb, imm, emit_flags); break;
        case 169: emit_jit_FAR(compiler, ra, rb, imm, emit_flags); break;
        case 170: emit_jit_EFA(compiler, ra, rb, imm, emit_flags); break;
        case 171: emit_jit_EFAR(compiler, ra, rb, imm, emit_flags); break;
        case 172: emit_jit_FABS(compiler, ra, rb, imm, emit_flags); break;
        case 173: emit_jit_UAR(compiler, ra, rb, imm, emit_flags); break;
        case 174: emit_jit_UA(compiler, ra, rb, imm, emit_flags); break;
        case 176: emit_jit_S(compiler, ra, rb, imm, emit_flags); break;
        case 177: emit_jit_SR(compiler, ra, rb, imm, emit_flags); break;
        case 178: emit_jit_SISP(compiler, ra, rb, imm, emit_flags); break;
        case 179: emit_jit_DECM(compiler, ra, rb, imm, emit_flags); break;
        case 180: emit_jit_NEG(compiler, ra, rb, imm, emit_flags); break;
        case 181: emit_jit_DNEG(compiler, ra, rb, imm, emit_flags); break;
        case 182: emit_jit_DS(compiler, ra, rb, imm, emit_flags); break;
        case 183: emit_jit_DSR(compiler, ra, rb, imm, emit_flags); break;
        case 184: emit_jit_FS(compiler, ra, rb, imm, emit_flags); break;
        case 185: emit_jit_FSR(compiler, ra, rb, imm, emit_flags); break;
        case 186: emit_jit_EFS(compiler, ra, rb, imm, emit_flags); break;
        case 187: emit_jit_EFSR(compiler, ra, rb, imm, emit_flags); break;
        case 188: emit_jit_FNEG(compiler, ra, rb, imm, emit_flags); break;
        case 189: emit_jit_USR(compiler, ra, rb, imm, emit_flags); break;
        case 190: emit_jit_US(compiler, ra, rb, imm, emit_flags); break;
        case 192: emit_jit_MS(compiler, ra, rb, imm, emit_flags); break;
        case 193: emit_jit_MSR(compiler, ra, rb, imm, emit_flags); break;
        case 194: emit_jit_MISP(compiler, ra, rb, imm, emit_flags); break;
        case 195: emit_jit_MISN(compiler, ra, rb, imm, emit_flags); break;
        case 196: emit_jit_M(compiler, ra, rb, imm, emit_flags); break;
        case 197: emit_jit_MR(compiler, ra, rb, imm, emit_flags); break;
        case 198: emit_jit_DM(compiler, ra, rb, imm, emit_flags); break;
        case 199: emit_jit_DMR(compiler, ra, rb, imm, emit_flags); break;
        case 200: emit_jit_FM(compiler, ra, rb, imm, emit_flags); break;
        case 201: emit_jit_FMR(compiler, ra, rb, imm, emit_flags); break;
        case 202: emit_jit_EFM(compiler, ra, rb, imm, emit_flags); break;
        case 203: emit_jit_EFMR(compiler, ra, rb, imm, emit_flags); break;
        case 208: emit_jit_DV(compiler, ra, rb, imm, emit_flags); break;
        case 209: emit_jit_DVR(compiler, ra, rb, imm, emit_flags); break;
        case 210: emit_jit_DISP(compiler, ra, rb, imm, emit_flags); break;
        case 211: emit_jit_DISN(compiler, ra, rb, imm, emit_flags); break;
        case 212: emit_jit_D(compiler, ra, rb, imm, emit_flags); break;
        case 213: emit_jit_DR(compiler, ra, rb, imm, emit_flags); break;
        case 214: emit_jit_DD(compiler, ra, rb, imm, emit_flags); break;
        case 215: emit_jit_DDR(compiler, ra, rb, imm, emit_flags); break;
        case 216: emit_jit_FD(compiler, ra, rb, imm, emit_flags); break;
        case 217: emit_jit_FDR(compiler, ra, rb, imm, emit_flags); break;
        case 218: emit_jit_EFD(compiler, ra, rb, imm, emit_flags); break;
        case 219: emit_jit_EFDR(compiler, ra, rb, imm, emit_flags); break;
        case 220: emit_jit_STE(compiler, ra, rb, imm, emit_flags); break;
        case 221: emit_jit_DSTE(compiler, ra, rb, imm, emit_flags); break;
        case 222: emit_jit_LE(compiler, ra, rb, imm, emit_flags); break;
        case 223: emit_jit_DLE(compiler, ra, rb, imm, emit_flags); break;
        case 224: emit_jit_OR(compiler, ra, rb, imm, emit_flags); break;
        case 225: emit_jit_ORR(compiler, ra, rb, imm, emit_flags); break;
        case 226: emit_jit_AND(compiler, ra, rb, imm, emit_flags); break;
        case 227: emit_jit_ANDR(compiler, ra, rb, imm, emit_flags); break;
        case 228: emit_jit_XOR(compiler, ra, rb, imm, emit_flags); break;
        case 229: emit_jit_XORR(compiler, ra, rb, imm, emit_flags); break;
        case 230: emit_jit_N(compiler, ra, rb, imm, emit_flags); break;
        case 231: emit_jit_NR(compiler, ra, rb, imm, emit_flags); break;
        case 232: emit_jit_FIX(compiler, ra, rb, imm, emit_flags); break;
        case 233: emit_jit_FLT(compiler, ra, rb, imm, emit_flags); break;
        case 234: emit_jit_EFIX(compiler, ra, rb, imm, emit_flags); break;
        case 235: emit_jit_EFLT(compiler, ra, rb, imm, emit_flags); break;
        case 236: emit_jit_XBR(compiler, ra, rb, imm, emit_flags); break;
        case 237: emit_jit_XWR(compiler, ra, rb, imm, emit_flags); break;
        case 240: emit_jit_C(compiler, ra, rb, imm, emit_flags); break;
        case 241: emit_jit_CR(compiler, ra, rb, imm, emit_flags); break;
        case 242: emit_jit_CISP(compiler, ra, rb, imm, emit_flags); break;
        case 243: emit_jit_CISN(compiler, ra, rb, imm, emit_flags); break;
        case 244: emit_jit_CBL(compiler, ra, rb, imm, emit_flags); break;
        case 245: emit_jit_UCIM(compiler, ra, rb, imm, emit_flags); break;
        case 246: emit_jit_DC(compiler, ra, rb, imm, emit_flags); break;
        case 247: emit_jit_DCR(compiler, ra, rb, imm, emit_flags); break;
        case 248: emit_jit_FC(compiler, ra, rb, imm, emit_flags); break;
        case 249: emit_jit_FCR(compiler, ra, rb, imm, emit_flags); break;
        case 250: emit_jit_EFC(compiler, ra, rb, imm, emit_flags); break;
        case 251: emit_jit_EFCR(compiler, ra, rb, imm, emit_flags); break;
        case 252: emit_jit_UCR(compiler, ra, rb, imm, emit_flags); break;
        case 253: emit_jit_UC(compiler, ra, rb, imm, emit_flags); break;
        case 255: emit_jit_NOP_BPT(compiler, ra, rb, imm, emit_flags); break;
        default: break;
    }
}
