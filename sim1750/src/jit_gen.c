#include "jit.h"
#include <stdio.h>
#include <stdbool.h>

extern OpcodeDef opcode_defs_brx[16];
extern OpcodeDef opcode_defs_imm[16];
extern OpcodeDef opcode_defs_6bit[17];
extern OpcodeDef opcode_defs_8bit[188];

void emit_instruction (OpcodeDef *def)
{
    if (!def->jitable) return;

    if (def->code == OPC_XIO) {
        printf("void emit_jit_%s(struct sljit_compiler *compiler, uint16_t rx, uint16_t ra, uint16_t imm, bool emit_flags) {\n", def->name);
        printf("    if (rx == 0) {\n");
        printf("        // Emit sljit for internal XIO read\n");
        printf("        if (imm == 0xA000) {\n");
        printf("            sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.mk));\n");
        printf("        } else if (imm == 0xA004) {\n");
        printf("            sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.pir));\n");
        printf("        } else if (imm == 0xA00E) {\n");
        printf("            sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.sw));\n");
        printf("        } else if ((imm & 0xF000) == 0xD000) {\n");
        printf("            // 51XY (read is D1XY), 52XY (read is D2XY). Page register reads\n");
        printf("            sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);\n");
        printf("            sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);\n");
        printf("            sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R2, 0, SLJIT_S0, 0, SLJIT_IMM, offsetof(struct cpu_context, state.reg.r) + ra * 2);\n");
        printf("            sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(process_xio));\n");
        printf("        } else {\n");
        printf("            // Fallback for other XIO rx=0\n");
        printf("            sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);\n");
        printf("            sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);\n");
        printf("            sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R2, 0, SLJIT_S0, 0, SLJIT_IMM, offsetof(struct cpu_context, state.reg.r) + ra * 2);\n");
        printf("            sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(process_xio));\n");
        printf("        }\n");
        printf("    }\n");
        printf("}\n\n");
        return;
    }

    printf("void emit_jit_%s(struct sljit_compiler *compiler, uint16_t ra, uint16_t rb, uint16_t imm, bool emit_flags) {\n", def->name);

    int DO_SIZE = 1;
    if (def->operands_type == OPERAND_INT32) DO_SIZE = 2;
    else if (def->operands_type == OPERAND_EXFLOAT) DO_SIZE = 3;

    if (def->op_type == OP_LOAD || def->op_type == OP_LOAD_BYTE || def->op_type == OP_STORE || def->op_type == OP_STORE_BYTE || def->op_type == OP_ADD || def->op_type == OP_SUB || def->op_type == OP_AND || def->op_type == OP_OR || def->op_type == OP_XOR || def->op_type == OP_NAND) {
        if (def->format == IF_REG_REG) {
            if (DO_SIZE == 1) {
                printf("    // Load RB into SLJIT_R1\n");
                printf("    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);\n");
            } else if (DO_SIZE == 2) {
                printf("    // Load RB (32bit) into SLJIT_R1\n");
                printf("    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);\n");
                printf("    sljit_emit_op2(compiler, SLJIT_SHL, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, 16);\n");
                printf("    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ((rb + 1) & 0xF) * 2);\n");
                printf("    sljit_emit_op2(compiler, SLJIT_OR, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_R2, 0);\n");
            }
        }
        else if (def->format == IF_IMMEDIATE) {
            printf("    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_IMM, imm);\n");
        }
        else if (def->format == IF_LONG && def->op_type == OP_LOAD) {
             printf("    // Setup R0=cpu_ctx, R1=addr, R2=&dst\n");
             printf("    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);\n");
             printf("    // Calculate Effective Address: addr = imm + (RX > 0 ? R[RX] : 0)\n");
             printf("    if (rb > 0) {\n");
             printf("        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);\n");
             printf("        sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, imm);\n");
             printf("    } else {\n");
             printf("        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);\n");
             printf("    }\n");
             printf("    sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R2, 0, SLJIT_S0, 0, SLJIT_IMM, offsetof(struct cpu_context, state.reg.r) + ra * 2);\n");
             printf("    sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(fetch_data_word));\n");
        }
        else if (def->format == IF_LONG && def->op_type == OP_STORE) {
             printf("    // Setup R0=cpu_ctx, R1=addr, R2=val\n");
             printf("    sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);\n");
             printf("    // Calculate Effective Address: addr = imm + (RX > 0 ? R[RX] : 0)\n");
             printf("    if (rb > 0) {\n");
             printf("        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + rb * 2);\n");
             printf("        sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, imm);\n");
             printf("    } else {\n");
             printf("        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, imm);\n");
             printf("    }\n");
             printf("    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);\n");
             printf("    sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3V(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(store_data_word));\n");
        }
    }

    if (def->op_type == OP_ADD) {
        if (def->format == IF_REG_REG) {
            printf("    // AR rA, rB\n");
            printf("    sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,\n");
            printf("                   SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,\n");
            printf("                   SLJIT_R1, 0);\n");
        }
        printf("    if (emit_flags) {\n");
        printf("        // Call calculate_flags_16bit(cpu_ctx, R[RA])\n");
        printf("        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);\n");
        printf("        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);\n");
        printf("        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));\n");
        printf("    }\n");
    } else if (def->op_type == OP_SUB) {
        if (def->format == IF_REG_REG) {
            printf("    // SR rA, rB\n");
            printf("    sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,\n");
            printf("                   SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,\n");
            printf("                   SLJIT_R1, 0);\n");
        }
        printf("    if (emit_flags) {\n");
        printf("        // Call calculate_flags_16bit(cpu_ctx, R[RA])\n");
        printf("        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);\n");
        printf("        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);\n");
        printf("        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));\n");
        printf("    }\n");
    } else if (def->op_type == OP_LOAD) {
        if (def->format == IF_REG_REG) {
            printf("    // LR rA, rB\n");
            printf("    sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2, SLJIT_R1, 0);\n");
        }
        printf("    if (emit_flags) {\n");
        printf("        // Call calculate_flags_16bit(cpu_ctx, R[RA])\n");
        printf("        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);\n");
        printf("        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);\n");
        printf("        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));\n");
        printf("    }\n");
    } else if (def->op_type == OP_AND) {
        if (def->format == IF_REG_REG) {
            printf("    // ANDR rA, rB\n");
            printf("    sljit_emit_op2(compiler, SLJIT_AND, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,\n");
            printf("                   SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,\n");
            printf("                   SLJIT_R1, 0);\n");
        }
        printf("    if (emit_flags) {\n");
        printf("        // Call calculate_flags_16bit(cpu_ctx, R[RA])\n");
        printf("        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);\n");
        printf("        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);\n");
        printf("        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));\n");
        printf("    }\n");
    } else if (def->op_type == OP_OR) {
        if (def->format == IF_REG_REG) {
            printf("    // ORR rA, rB\n");
            printf("    sljit_emit_op2(compiler, SLJIT_OR, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,\n");
            printf("                   SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,\n");
            printf("                   SLJIT_R1, 0);\n");
        }
        printf("    if (emit_flags) {\n");
        printf("        // Call calculate_flags_16bit(cpu_ctx, R[RA])\n");
        printf("        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);\n");
        printf("        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);\n");
        printf("        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));\n");
        printf("    }\n");
    } else if (def->op_type == OP_XOR) {
        if (def->format == IF_REG_REG) {
            printf("    // XORR rA, rB\n");
            printf("    sljit_emit_op2(compiler, SLJIT_XOR, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,\n");
            printf("                   SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2,\n");
            printf("                   SLJIT_R1, 0);\n");
        }
        printf("    if (emit_flags) {\n");
        printf("        // Call calculate_flags_16bit(cpu_ctx, R[RA])\n");
        printf("        sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);\n");
        printf("        sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), offsetof(struct cpu_context, state.reg.r) + ra * 2);\n");
        printf("        sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2V(W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(calculate_flags_16bit));\n");
        printf("    }\n");
    } else {
        printf("    // TODO: implement %s in sljit\n", def->name);
    }
    printf("}\n\n");
}

void generate_jit_code()
{
    printf("#include \"jit.h\"\n");
    printf("#include \"cpu_ctx.h\"\n");
    printf("#include \"cpu_helpers.h\"\n");
    printf("#include \"sljit/sljit_src/sljitLir.h\"\n");
    printf("#include <stddef.h>\n\n");

    for (int i = 0; i < 16; i++) {
        if (!opcode_defs_6bit[i].valid) continue;
        emit_instruction(&opcode_defs_6bit[i]);
    }
    for (int i = 0; i < 16; i++) {
        if (!opcode_defs_brx[i].valid) continue;
        emit_instruction(&opcode_defs_brx[i]);
    }
    for (int i = 0; i < 16; i++) {
        if (!opcode_defs_imm[i].valid) continue;
        emit_instruction(&opcode_defs_imm[i]);
    }
    for (int i = 0; i < 188; i++) {
        if (!opcode_defs_8bit[i].valid) continue;
        emit_instruction(&opcode_defs_8bit[i]);
    }
}

void generate_dispatch_code()
{
    printf("void compile_jit_instruction(struct sljit_compiler *compiler, OpcodeDef *def, uint16_t opcode, uint16_t imm, bool emit_flags) {\n");
    printf("    uint16_t ra = (opcode >> 4) & 0xF;\n");
    printf("    uint16_t rb = opcode & 0xF;\n");
    printf("    switch (def->code) {\n");
    for (int i = 0; i < 16; i++) {
        if (!opcode_defs_6bit[i].valid || !opcode_defs_6bit[i].jitable) continue;
        printf("        case %d: emit_jit_%s(compiler, ra, rb, imm, emit_flags); break;\n", opcode_defs_6bit[i].code, opcode_defs_6bit[i].name);
    }
    for (int i = 0; i < 16; i++) {
        if (!opcode_defs_brx[i].valid || !opcode_defs_brx[i].jitable) continue;
        printf("        case %d: emit_jit_%s(compiler, ra, rb, imm, emit_flags); break;\n", opcode_defs_brx[i].code, opcode_defs_brx[i].name);
    }
    for (int i = 0; i < 16; i++) {
        if (!opcode_defs_imm[i].valid || !opcode_defs_imm[i].jitable) continue;
        printf("        case %d: emit_jit_%s(compiler, ra, rb, imm, emit_flags); break;\n", opcode_defs_imm[i].code, opcode_defs_imm[i].name);
    }
    for (int i = 0; i < 188; i++) {
        if (!opcode_defs_8bit[i].valid || !opcode_defs_8bit[i].jitable) continue;
        if (opcode_defs_8bit[i].code == OPC_XIO) {
            printf("        case %d: emit_jit_%s(compiler, opcode & 0xF, (opcode >> 4) & 0xF, imm, emit_flags); break;\n", opcode_defs_8bit[i].code, opcode_defs_8bit[i].name);
        } else {
            printf("        case %d: emit_jit_%s(compiler, ra, rb, imm, emit_flags); break;\n", opcode_defs_8bit[i].code, opcode_defs_8bit[i].name);
        }
    }
    printf("        default: break;\n");
    printf("    }\n");
    printf("}\n");
}

int main()
{
    generate_jit_code();
    generate_dispatch_code();
    return 0;
}
