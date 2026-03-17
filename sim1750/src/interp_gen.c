#include "jit.h"
#include <stdio.h>

extern OpcodeDef opcode_defs_brx[16];
extern OpcodeDef opcode_defs_imm[16];
extern OpcodeDef opcode_defs_6bit[17];
extern OpcodeDef opcode_defs_8bit[188];

void emit_shift_instruction (OpcodeDef *def)
{
    printf("    uint16_t RB = opcode & 0x000F;\n");
    if (def->format == IF_CONST_REG)
    {
        printf("    int16_t shift = ((opcode & 0x00F0) >> 4) + 1;\n");
        printf("    uint16_t dest_reg = RB;\n");
    }
    else
    {
        printf("    uint16_t RA = (opcode & 0x00F0) >> 4;\n");
        printf("    int16_t shift = cpu_ctx->state.reg.r[RB];\n");
        printf("    uint16_t dest_reg = RA;\n");
        if (def->operands_type == OPERAND_INT32)
        {
        /* check bounds of shift -32 to 32 inclusive */
            printf("    if (shift < -32 || shift > 32)\n");
        }
        else
        {
            printf("    if (shift < -16 || shift > 16)\n");
        }
        printf("    {\n        cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n    return;\n    }\n");
        

    
    }
    enum shift_dir { SHIFT_RIGHT, SHIFT_LEFT, SHIFT_BIDIRECTIONAL} dir = SHIFT_LEFT;
    bool cyclic = false;
    bool logical = false;
    if (def->code == OPC_SLL || def->code == OPC_DSLL)
    {
        dir = SHIFT_LEFT;
        logical = true;
    }
    else if (def->code == OPC_SRL || def->code == OPC_DSRL)
    {
        dir = SHIFT_RIGHT;
        logical = true;
    }
    
    else if (def->code == OPC_SRA || def->code == OPC_DSRA)
    {
        dir = SHIFT_RIGHT;
        logical = false; // arithmetic
    }
    else if (def->code == OPC_SLC || def->code == OPC_DSLC)
    {
        dir = SHIFT_LEFT;
        logical = true;
        cyclic = true;
    }
    else if (def->code == OPC_SLR || def->code == OPC_DSLR)
    {
        dir = SHIFT_BIDIRECTIONAL;
        logical = true;
        cyclic = false;
    }
    else if (def->code == OPC_SAR || def->code == OPC_DSAR)
    {
        dir = SHIFT_BIDIRECTIONAL;
        logical = false;
        cyclic = false;
    }
    else if (def->code == OPC_SCR || def->code == OPC_DSCR)
    {
        dir = SHIFT_BIDIRECTIONAL;
        logical = true;
        cyclic = true;
    }
    /* we need to be careful when shift is 16 or -16 in 16 bit, or -32,32 in 32 bit */
    /* arithmetic shift add ones when shifting right negative number*/
    if (def->operands_type == OPERAND_INT32)
    {
        /* 1750a is a 16 bit big endian system */
        printf("    int32_t val = ((uint32_t)cpu_ctx->state.reg.r[dest_reg] << 16) | (uint16_t)cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF];\n");
        if (!cyclic && !logical && dir == SHIFT_BIDIRECTIONAL)
        {
            printf("    int64_t overflow_test = val;\n");
            printf("    if (shift > 0) overflow_test = (uint64_t)overflow_test << shift;\n");
        }
        if (dir == SHIFT_LEFT) {
            if (cyclic)
            {
                printf("    val = (int32_t)(((uint32_t)val << shift) | ((uint32_t)val >> (32 - shift)));\n");
            }
            else
                printf("    val <<= shift;\n");
            
        } else if (dir == SHIFT_RIGHT) {
            if (cyclic)
            {
                printf("    val = (int32_t)(((uint32_t)val << shift) | ((uint32_t)val >> (32 - shift)));\n");
            }
            else if (logical)
                printf("    val = (int32_t)((uint32_t)val >> shift);\n");
            else printf("    val >>= shift;\n");
        } else { // Bidirectional
            if (cyclic)
            {
                printf("    if (shift > 0) {\n");
                printf("        if (shift < 32) val = (int32_t)(((uint32_t)val << shift) | ((uint32_t)val >> (32 - shift)));\n");
                printf("    } else if (shift < 0) {\n");
                printf("        int16_t s = -shift;\n");
                printf("        if (s < 32) val = (int32_t)(((uint32_t)val >> s) | ((uint32_t)val << (32 - s)));\n");
                printf("    }\n");
            }
            else
            {
                printf("    if (shift > 0) { if (shift < 32) val = (uint32_t)val << shift; else val = 0;}\n");
                printf("    else if (shift < 0) {\n");
                if (logical) printf("        if (shift > -32) val = (int32_t)((uint32_t)val >> (-shift)); else val = 0;\n");
                else printf("        if (shift > -32) val >>= (-shift); else if (val < 0) val = ~0; else val = 0;\n");
                printf("    }\n");
            }
        }
        if (!cyclic && !logical && dir == SHIFT_BIDIRECTIONAL)
        {
            // turn on overflow interrupt if shift left changed sign.
            printf ("    if (overflow_test < -2147483648LL || overflow_test > 2147483647LL)  cpu_ctx->state.reg.pir |= INTR_FIXOFL;");
        }
        printf("    cpu_ctx->state.reg.r[dest_reg] = (uint16_t)(val >> 16);\n");
        printf("    cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF] = (uint16_t)(val & 0xFFFF);\n");
        printf("    calculate_flags_32bit_reg(cpu_ctx, dest_reg);\n");
    }
    else
    {
        /* process 16 bit value */
        printf("    int16_t val = cpu_ctx->state.reg.r[dest_reg];\n");
        if (!cyclic && !logical && dir == SHIFT_BIDIRECTIONAL)
        {
            printf("    int32_t overflow_test = val;\n");
            printf("    if (shift > 0) overflow_test = (uint32_t)overflow_test << shift;\n");
        }
        if (dir == SHIFT_LEFT) {
            if (cyclic)
                printf("    if (shift < 16) val = (int16_t)(((uint16_t)val << shift) | ((uint16_t)val >> (16 - shift)));\n");
            else
                printf("    if (shift < 16) val = (uint16_t)val << shift; else val = 0;\n");
        } else if (dir == SHIFT_RIGHT) {
            if (cyclic)
                printf("    if (shift < 16) val = (int16_t)(((uint16_t)val << shift) | ((uint16_t)val >> (16 - shift)));\n");
            else if (logical)
                printf("    if (shift < 16) val = (int16_t)((uint16_t)val >> shift); else val = 0;\n");
            else
                printf("    if (shift < 16) val >>= shift; else if (val < 0) val = ~0; else val = 0;\n");
        } else { // Bidirectional
            if (cyclic) {
                printf("    if (shift > 0) {\n");
                printf("        if (shift < 16) val = (int16_t)(((uint16_t)val << shift) | ((uint16_t)val >> (16 - shift)));\n");
                printf("    } else if (shift < 0) {\n");
                printf("        int16_t s = -shift;\n");
                printf("        if (s < 16) val = (int16_t)(((uint16_t)val >> s) | ((uint16_t)val << (16 - s)));\n");
                printf("    }\n");
            }
            else
            {
                printf("    if (shift > 0) { if (shift < 16) val = (uint16_t)val << shift; else val = 0; }\n");
                printf("    else if (shift < 0) {\n");
                if (logical) printf("        if (shift > -16) val = (int16_t)((uint16_t)val >> (-shift)); else val = 0;\n");
                else printf("        if (shift > -16) val >>= (-shift); else if (val < 0) val = ~0; else val = 0;\n");
                printf("    }\n");
            }
        }
        if (!cyclic && !logical && dir == SHIFT_BIDIRECTIONAL)
        {
            // turn on overflow interrupt if sign changed during left shift.
            printf ("    if (overflow_test > 32767 || overflow_test < -32768)  cpu_ctx->state.reg.pir |= INTR_FIXOFL;");
        }
        printf("    cpu_ctx->state.reg.r[dest_reg] = val;\n");
        printf("    calculate_flags_16bit(cpu_ctx, val);\n");
    }

    printf("    cpu_ctx->state.reg.ic += 1;\n");
    printf("    cpu_ctx->state.total_cycles += CLK_CYC_%s(shift);\n", def->name);
    printf("}\n\n");
}
/* this file will generate C code for new instruction interpreter */
void emit_instruction (OpcodeDef *def)
{
    bool complicated_clock_cycles_calc = def->op_type == OP_MOVE ||
        def->op_type == OP_ABS || def->op_type == OP_ABS_FLOAT || def->op_type == OP_BRANCH ||
        def->op_type == OP_SUBTR_JUMP || def->op_type == OP_MULT_REG ||
        def->op_type == OP_JUMP_COND || def->op_type == OP_XIO;
    printf("/* %s - %s*/\n", def->name, def->description);
    if ((def->code >= OPC_IMM_AIM && def->code <= OPC_IMM_NIM) || (def->code >= OPC_BRX_LBX && def->code <= OPC_BRX_ORBX)) // imm and brx sub opcodes will be inlined
        printf("static inline ");
    printf("void interpret_%s(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t %s) {\n", def->name, def->is_imm?"imm_value":"/* imm_value */");
    if (def->op_type == OP_SHIFT) {
        emit_shift_instruction(def);
        return;
    }
    int DO_SIZE = 1;
    if (def->operands_type == OPERAND_INT32 || def->operands_type == OPERAND_FLOAT || def->code == OPC_CBL)
    {
        DO_SIZE = 2;
    }
    else if (def->operands_type == OPERAND_EXFLOAT || def->op_type == OP_LOAD_STATUS)
    {
        DO_SIZE = 3;
    }
    if (def->addr_mode != AM_REG_DIRECT_R && def->addr_mode != AM_IMM_SHRT_NEG_ISN
     && def->addr_mode != AM_IMM_SHRT_POS_ISP && def->addr_mode != AM_CNT_REL_ICR)
    {
        printf("    bool ok = true;\n"); // we are going to fetch/set memory, so we need that var;
    }
    bool has_DO_ADDR = false;
    bool priviledged = false;
    if (def->code == OPC_XIO || def->code == OPC_VIO || def->code == OPC_LST || def->code == OPC_LSTI) /* priviledged */
    {
        priviledged = true;
        printf("    uint16_t ps = (cpu_ctx->state.reg.sw & 0x00F0) >> 4;\n");
        /* if ps is not 0, we turn on INTR_MACHERR in pir, and set ft bit 10 (FT_PRIV_INSTR)*/
        printf("    if (ps != 0){\n");
        printf("        cpu_ctx->state.reg.pir |= INTR_MACHERR;\n");
        printf("        cpu_ctx->state.reg.ft |= FT_PRIV_INSTR;\n");
        printf("        cpu_ctx->state.reg.ic += %d;\n", def->is_imm? 2 : 1);
        printf("    }\n");
        printf("    else {\n");
    }

    // we will write the logic for calculating effective address and loading data for IF_BASE_RELATIVE and IF_BASE_REL_INDX formats, and for loading immediate value for IF_IMMEDIATE format, and for loading register values for IF_REG_REG format. For other formats we will just put a TODO comment.
    if (def->format == IF_BASE_RELATIVE || def->format == IF_BASE_REL_INDX) {
        if (def->operands_type == OPERAND_INT16) {
            printf("    const uint16_t RA = 2;\n");
        }
        else
        {
            printf("    const uint16_t RA = 0;\n");
        }
        printf("    uint16_t BR = ((opcode & 0x0300)>>8)+12;\n");
        if (def->format == IF_BASE_REL_INDX && def->addr_mode == AM_BASE_REL_IDX_BX)
        {
            printf("    uint16_t RX = opcode & 0x000F;\n");
            printf("    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];\n");
        }
        else if ( def->format == IF_BASE_RELATIVE && def->addr_mode == AM_BASE_REL_B)
        {
            printf("    uint16_t DISPLACEMENT = opcode & 0x00FF;\n");
            printf("    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];\n");
        }
        has_DO_ADDR = true;

    }

    
    if (def->format == IF_REG_REG) {
        printf("    uint16_t RA = (opcode & 0x00F0) >> 4;\n");
        printf("    uint16_t RB = opcode & 0x000F;\n");
        printf("    int16_t DO[%d];\n", DO_SIZE);
        for (int i = 0 ; i < DO_SIZE; ++i)
        {
            printf ("    DO[%d] = cpu_ctx->state.reg.r[(RB+%d)&0xF];\n", i, i);
        }
    }
    if (def->format == IF_REG_CONST) {
        printf("    uint16_t RA = (opcode & 0x00F0) >> 4;\n");
        if (def->addr_mode == AM_IMM_SHRT_POS_ISP || def->addr_mode == AM_IMM_SHRT_NEG_ISN)
            printf("    int16_t DO[1];\n");
        if (def->addr_mode == AM_IMM_SHRT_POS_ISP)
        {
            printf("    DO[0] = (opcode & 0x000F) + 1;\n");
        }
        else if (def->addr_mode == AM_IMM_SHRT_NEG_ISN)
        {
            printf("    DO[0] = -((opcode & 0x000F) + 1);\n");
        }

    }
    else if (def->format == IF_LONG || def->format == IF_CONST_LONG)
    {
        if (def->format == IF_CONST_LONG)
        {
            printf("    uint16_t N = (opcode & 0x00F0) >> 4;\n");
        }
        else
        {
            printf("    uint16_t RA = (opcode & 0x00F0) >> 4;\n");
        }
        printf("    uint16_t RX = opcode & 0x000F;\n");

        if (def->addr_mode == AM_IMM_LONG_IM_IMX)
        {
            printf("    int16_t DO[1];\n");
            printf("    DO[0] = imm_value + ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0);\n");
        }
        else if (def->addr_mode == AM_MEM_INDIRECT_I_IX)
        {
            printf("    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;\n");
            printf("    uint16_t DO_ADDR;\n");
            printf("    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);\n");
            has_DO_ADDR = true;
        }
        else if (def->addr_mode == AM_MEM_DIRECT_D_DX)
        {
            printf("    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;\n");
            has_DO_ADDR = true;
        }
    }
    else if (def->format == IF_IMMEDIATE) {
        printf("    uint16_t RA = (opcode & 0x00F0) >> 4;\n");
        printf("    int16_t DO[1];\n");
        printf("    DO[0] = imm_value;\n");
    }
    else if (def->format == IF_CONST_REG)
    {
        printf("    uint16_t N = (opcode & 0x00F0) >> 4;\n");
        printf("    uint16_t RB = opcode & 0x000F;\n");
    }
    else {
        printf("    /* TODO: Implement logic for instruction format %d and addressing mode %d */\n", def->format, def->addr_mode);
    }
    if (has_DO_ADDR)
    {
        if (def->op_type != OP_STORE && def->op_type != OP_JUMP_COND && def->op_type != OP_JUMP_SUBRTN && def->op_type != OP_RET_SUBRTN && def->op_type != OP_XIO)
        {
            //if not store operation, we need to fetch derived operand (DO) from memory
            
            printf ("    int16_t DO[%d];\n", DO_SIZE);
            if (DO_SIZE == 1)
            {
                printf ("    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);\n");
            }
            else
            {
                for (int i = 0 ; i < DO_SIZE; ++i)
                {
                    printf ("    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, %d, DO);\n", DO_SIZE);
                }
            }
        }
    }
    
    switch (def->op_type)
    {
        case OP_LOAD:
            for (int i = 0 ; i < DO_SIZE; ++i)
            {
                printf ("    cpu_ctx->state.reg.r[(RA+%d)&0xF] = DO[%d];\n", i, i);
            }
            if (DO_SIZE == 1)
                printf("    calculate_flags_16bit(cpu_ctx, DO[0]);\n");
            else if (DO_SIZE == 2)
                printf("    calculate_flags_32bit(cpu_ctx, DO);\n");
            else if (DO_SIZE == 3)
                printf("    calculate_flags_48bit(cpu_ctx, DO);\n");
            break;
        case OP_LOAD_BYTE: /* LUB, LUBI, LLB, LLBI */
            if (def->code == OPC_LUB || def->code == OPC_LUBI) /* upper */
            {
                printf ("    cpu_ctx->state.reg.r[RA] = (((uint16_t)DO[0] & 0xFF00) >> 8) | (cpu_ctx->state.reg.r[RA] & 0xFF00);\n");
            }
            else /* lower */
            {
                printf ("    cpu_ctx->state.reg.r[RA] = (DO[0] & 0x00FF) | (cpu_ctx->state.reg.r[RA] & 0xFF00);\n");
            }
            printf("    calculate_flags_16bit(cpu_ctx, cpu_ctx->state.reg.r[RA]);\n");
        
            break;

        case OP_STORE:
            /* store contents of RA into DO_ADDR*/
            if (def->format == IF_CONST_LONG) /* STC, STCI*/
            {
                printf ("    ok = store_data_word(cpu_ctx, DO_ADDR, N);\n");
            }
            else
            {
                if (DO_SIZE == 1)
                {
                    printf ("    ok = store_data_word(cpu_ctx, DO_ADDR, cpu_ctx->state.reg.r[RA]);\n");
                }
                else if (DO_SIZE >= 2)
                {
                    printf ("    ok = 0 == store_data_words_reg(cpu_ctx, DO_ADDR, %d, RA);\n", DO_SIZE);
                }
            }
            break;
        case OP_STORE_BYTE: /* STUB, SUBI, STLB, SLBI */
            if (def->code == OPC_STUB || def->code == OPC_SUBI) /* upper */
            {
                printf ("    DO[0] = (DO[0] & 0x00FF) | ((cpu_ctx->state.reg.r[RA] & 0x00FF) << 8);\n");
            } 
            
            else /* lower */
            {
                printf ("    DO[0] = (DO[0] & 0xFF00) | (cpu_ctx->state.reg.r[RA] & 0x00FF);\n");
            }
            printf ("    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);\n");
            break;
        case OP_BRANCH:
            /* displacement is signed, range -128 to 127*/
            printf("    int8_t displacement = (int8_t)(opcode & 0x00FF);\n");
            if (def->code == OPC_BR)
            {
                printf("    cpu_ctx->state.reg.ic += displacement;\n");
            }
            else
            {
                switch(def->code)
                {
                    case OPC_BLT: // less then
                        printf("    bool branch_taken = cpu_ctx->state.reg.sw & CS_NEGATIVE;\n");
                        break;
                    case OPC_BLE: // less then or equal
                        printf("    bool branch_taken = cpu_ctx->state.reg.sw & (CS_NEGATIVE | CS_ZERO);\n");
                        break;
                    case OPC_BGT: // greater then
                        printf("    bool branch_taken = cpu_ctx->state.reg.sw & CS_POSITIVE;\n");
                        break;
                    case OPC_BGE: // greater then or equal
                        printf("    bool branch_taken = cpu_ctx->state.reg.sw & (CS_POSITIVE | CS_ZERO);\n");
                        break;
                    case OPC_BEZ: // equal
                        printf("    bool branch_taken = cpu_ctx->state.reg.sw & CS_ZERO;\n");
                        break;
                    case OPC_BNZ: // not equal
                        printf("    bool branch_taken = !(cpu_ctx->state.reg.sw & CS_ZERO);\n");
                        break;
                    default:
                        break;

                }
                printf("    if (branch_taken) {\n");
                printf("        cpu_ctx->state.reg.ic += displacement;\n");
                printf("    }\n");
                printf("    else {\n");
                printf("        cpu_ctx->state.reg.ic += 1;\n");
                printf("    }\n");
            }
            break;
        case OP_SET_BIT:
        case OP_SET_VAR_BIT:
            if (def->op_type == OP_SET_VAR_BIT)
            {
                printf("    uint16_t N = cpu_ctx->state.reg.r[RA] & 0x000F;\n");
            }
            if (def->format == IF_CONST_REG || def->format == IF_REG_REG)
            {
                printf("    cpu_ctx->state.reg.r[RB] |= 1 << (15 - N);\n");
            }
            else
            {
                printf("    DO[0] |= 1 << (15 - N);\n");
                printf("    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);\n");
            }
            break;
        case OP_RESET_BIT:
        case OP_RESET_VAR_BIT:
            if (def->op_type == OP_RESET_VAR_BIT)
            {
                printf("    uint16_t N = cpu_ctx->state.reg.r[RA] & 0x000F;\n");
            }
            if (def->format == IF_CONST_REG || def->format == IF_REG_REG)
            {
                printf("    cpu_ctx->state.reg.r[RB] &= ~(1 << (15 - N));\n");
            }
            else
            {
                printf("    DO[0] &= ~(1 << (15 - N));\n");
                printf("    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);\n");
            }
            break;
        case OP_TEST_BIT:
        case OP_TEST_SET_BIT:
        case OP_TEST_VAR_BIT:
            if (def->op_type == OP_TEST_VAR_BIT)
            {
                printf("    uint16_t N = cpu_ctx->state.reg.r[RA] & 0x000F;\n");
            }
            if (def->format == IF_CONST_REG || def->format == IF_REG_REG)
            {
                printf("    bool is_on = cpu_ctx->state.reg.r[RB] & (1 << (15 - N));\n");
                if (def->op_type == OP_TEST_SET_BIT)
                {
                    printf("    cpu_ctx->state.reg.r[RB] |= 1 << (15 - N);\n");
                }

            }
            else
            {
                printf("    bool is_on = DO[0] & (1 << (15 - N));\n");
                if (def->op_type == OP_TEST_SET_BIT)
                {
                    printf("    DO[0] |= 1 << (15 - N);\n");
                    printf("    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);\n");
                }
            }
            printf("    cpu_ctx->state.reg.sw &= 0x0FFF;\n"); // Clear condition flags
            printf("    if (is_on) {\n");
            printf("        cpu_ctx->state.reg.sw |= (N>0)?CS_POSITIVE:CS_NEGATIVE; // N is the bit number (0 is msb where the sign bit is), not the value\n");
            printf("    }\n");
            printf("    else {\n");
            printf("        cpu_ctx->state.reg.sw |= CS_ZERO;\n");
            printf("    }\n");
            break;
            
        case OP_ADD:
        case OP_SUB:
            if (DO_SIZE == 1)
            {
                printf("    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];\n");
                printf("    int16_t b = DO[0];\n");
                if (def->op_type == OP_ADD) {
                    printf("    int32_t res = (int32_t)a + (int32_t)b;\n");
                    /* Unsigned boundary check for Carry */
                    printf("    bool carry = ((uint32_t)(uint16_t)a + (uint32_t)(uint16_t)b) > 0xFFFF;\n");
                } else {
                    printf("    int32_t res = (int32_t)a - (int32_t)b;\n");
                    /* Unsigned subtraction check for Carry (No Borrow) */
                    printf("    bool carry = (uint16_t)a >= (uint16_t)b;\n");
                }
                
                /* Check for signed Fixed-Point Overflow */
                printf("    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
                
                printf("    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;\n");
                printf("    calculate_flags_16bit(cpu_ctx, (int16_t)res);\n");
                
                /* Apply carry bit AFTER calculate_flags wipes the upper nibble */
                printf("    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;\n");
            }
            else if (DO_SIZE == 2)
            {
                /* 32-bit values spread across two 16-bit registers */
                printf("    int32_t a = ((int32_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF];\n");
                printf("    int32_t b = ((int32_t)DO[0] << 16) | (uint16_t)DO[1];\n");
                
                if (def->op_type == OP_ADD) {
                    printf("    int64_t res = (int64_t)a + (int64_t)b;\n");
                    printf("    bool carry = ((uint64_t)(uint32_t)a + (uint64_t)(uint32_t)b) > 0xFFFFFFFFULL;\n");
                } else {
                    printf("    int64_t res = (int64_t)a - (int64_t)b;\n");
                    printf("    bool carry = (uint32_t)a >= (uint32_t)b;\n");
                }
                
                /* Check for 32-bit signed Fixed-Point Overflow */
                printf("    if (res > 2147483647LL || res < -2147483648LL) cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
                
                printf("    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(res >> 16);\n");
                printf("    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(res & 0xFFFF);\n");
                
                printf("    calculate_flags_32bit_reg(cpu_ctx, RA);\n");
                
                /* Apply 32-bit carry out */
                printf("    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;\n");
            }
            break;
        case OP_INC_DEC_MEM:
            /* N is 0-15, but represents 1-16 logically */
            printf("    int16_t mem_val = DO[0];\n");
            printf("    int16_t amount = N + 1;\n");
            if (def->code == OPC_INCM) {
                printf("    int32_t res = (int32_t)mem_val + amount;\n");
                printf("    bool carry = ((uint32_t)(uint16_t)mem_val + (uint32_t)amount) > 0xFFFF;\n");
            } else { /* OPC_DECM */
                printf("    int32_t res = (int32_t)mem_val - amount;\n");
                printf("    bool carry = (uint16_t)mem_val >= (uint16_t)amount;\n");
            }
            /* Check for signed Fixed-Point Overflow */
            printf("    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
            
            printf("    DO[0] = (int16_t)res;\n");
            printf("    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);\n");
            
            printf("    calculate_flags_16bit(cpu_ctx, DO[0]);\n");
            printf("    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;\n");
            break;

        case OP_ABS:
            if (DO_SIZE == 1) {
                printf("    int16_t val = DO[0];\n");
                /* Taking the absolute value of the most negative number causes an overflow */
                printf("    if (val == (int16_t)0x8000) cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
                printf("    int16_t res = (val < 0) ? -val : val;\n");
                printf("    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;\n");
                printf("    calculate_flags_16bit(cpu_ctx, res);\n");
                /* Carry is unconditionally cleared by calculate_flags_16bit, which is correct for ABS */
            } else if (DO_SIZE == 2) {
                printf("    int32_t val = ((int32_t)DO[0] << 16) | (uint16_t)DO[1];\n");
                printf("    if (val == (int32_t)0x80000000) cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
                printf("    int32_t res = (val < 0) ? -val : val;\n");
                printf("    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(res >> 16);\n");
                printf("    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(res & 0xFFFF);\n");
                printf("    calculate_flags_32bit_reg(cpu_ctx, RA);\n");
            }
            break;

        case OP_NEG:
            if (DO_SIZE == 1) {
                printf("    int16_t val = DO[0];\n");
                /* Taking the negative of the most negative number causes an overflow */
                printf("    if (val == (int16_t)0x8000) cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
                printf("    int32_t res = -(int32_t)val;\n");
                
                printf("    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;\n");
                
                /* This function automatically clears the Carry bit to 0, 
                   perfectly satisfying the manual's (CS) <-- 0010 / 0001 / 0100 rules */
                printf("    calculate_flags_16bit(cpu_ctx, (int16_t)res);\n");
            } else if (DO_SIZE == 2) {
                printf("    int32_t val = ((int32_t)DO[0] << 16) | (uint16_t)DO[1];\n");
                printf("    if (val == (int32_t)0x80000000) cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
                printf("    int64_t res = -(int64_t)val;\n");
                
                printf("    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(res >> 16);\n");
                printf("    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(res & 0xFFFF);\n");
                
                printf("    calculate_flags_32bit_reg(cpu_ctx, RA);\n");
            }
            break;
        case OP_MULT_PROD16:
            /* 16-bit * 16-bit = 16-bit Product (MSR, MS, MISP, etc.) */
            printf("    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];\n");
            printf("    int16_t B = (int16_t)DO[0];\n");
            printf("    int32_t prod = (int32_t)A * (int32_t)B;\n");
            
            /* If the product exceeds a 16-bit boundary, trigger Overflow */
            printf("    if (prod > 32767 || prod < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
            
            printf("    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(prod & 0xFFFF);\n");
            printf("    calculate_flags_16bit(cpu_ctx, (int16_t)prod);\n");
            break;

        case OP_MULT_PROD32:
            /* 16-bit * 16-bit = 32-bit Product (MR, M, MB, etc.) */
            printf("    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];\n");
            printf("    int16_t B = (int16_t)DO[0];\n");
            printf("    int32_t prod = (int32_t)A * (int32_t)B;\n");
            
            /* Fits perfectly, no overflow possible. Pack into RA (High) and RA+1 (Low) */
            printf("    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)((prod >> 16) & 0xFFFF);\n");
            printf("    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(prod & 0xFFFF);\n");
            printf("    calculate_flags_32bit_reg(cpu_ctx, RA);\n");
            break;

        case OP_DMULT_PROD32:
            /* 32-bit * 32-bit = 32-bit Product (DMR, DM) */
            printf("    int32_t A = ((int32_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF];\n");
            printf("    int32_t B = ((int32_t)DO[0] << 16) | (uint16_t)DO[1];\n");
            printf("    int64_t prod = (int64_t)A * (int64_t)B;\n");
            
            /* Overflow if product exceeds 32-bit bounds */
            printf("    if (prod > 2147483647LL || prod < -2147483648LL) cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
            
            printf("    int32_t res = (int32_t)prod;\n");
            printf("    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)((res >> 16) & 0xFFFF);\n");
            printf("    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(res & 0xFFFF);\n");
            printf("    calculate_flags_32bit_reg(cpu_ctx, RA);\n");
            break;
        case OP_DIV_REM_16_16:
            /* 16-bit / 16-bit = 16-bit Quotient (RA), 16-bit Remainder (RA+1) */
            printf("    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];\n");
            printf("    int16_t B = (int16_t)DO[0];\n");
            printf("    if (B == 0) {\n");
            printf("        cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
            printf("    } else if (A == (int16_t)0x8000 && B == -1) {\n");
            printf("        cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
            printf("        cpu_ctx->state.reg.r[(RA+0)&0xF] = 0x8000; /* Q */\n");
            printf("        cpu_ctx->state.reg.r[(RA+1)&0xF] = 0;      /* Rem */\n");
            printf("        calculate_flags_16bit(cpu_ctx, 0x8000);\n");
            printf("    } else {\n");
            printf("        int16_t Q = A / B;\n");
            printf("        int16_t Rem = A %% B;\n");
            printf("        cpu_ctx->state.reg.r[(RA+0)&0xF] = Q;\n");
            printf("        cpu_ctx->state.reg.r[(RA+1)&0xF] = Rem;\n");
            printf("        calculate_flags_16bit(cpu_ctx, Q);\n");
            printf("    }\n");
            break;

        case OP_DIV_REM_32_16:
            /* 32-bit / 16-bit = 16-bit Quotient (RA), 16-bit Remainder (RA+1) */
            printf("    int32_t A = ((int32_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF];\n");
            printf("    int16_t B = (int16_t)DO[0];\n");
            printf("    if (B == 0) {\n");
            printf("        cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
            printf("    } else {\n");
            printf("        int32_t Q = A / B;\n");
            printf("        int16_t Rem = A %% B;\n");
            printf("        if (Q > 32767 || Q < -32768) {\n");
            printf("            cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
            printf("        } else {\n");
            printf("            cpu_ctx->state.reg.r[(RA+0)&0xF] = Q;\n");
            printf("            cpu_ctx->state.reg.r[(RA+1)&0xF] = Rem;\n");
            printf("            calculate_flags_16bit(cpu_ctx, (int16_t)Q);\n");
            printf("        }\n");
            printf("    }\n");
            break;

        case OP_DIV_32_32:
            /* 32-bit / 32-bit = 32-bit Quotient (RA, RA+1). Remainder is LOST. */
            printf("    int32_t A = ((int32_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF];\n");
            printf("    int32_t B = ((int32_t)DO[0] << 16) | (uint16_t)DO[1];\n");
            printf("    if (B == 0) {\n");
            printf("        cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
            printf("    } else if (A == (int32_t)0x80000000 && B == -1) {\n");
            printf("        cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
            printf("        cpu_ctx->state.reg.r[(RA+0)&0xF] = 0x8000;\n        cpu_ctx->state.reg.r[(RA+1)&0xF] = 0x0000; /* Wrap Q */\n");
            printf("        calculate_flags_32bit_reg(cpu_ctx, RA);\n");
            printf("    } else {\n");
            printf("        int32_t Q = A / B;\n");
            printf("        cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)((Q >> 16) & 0xFFFF);\n");
            printf("        cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(Q & 0xFFFF);\n");
            printf("        calculate_flags_32bit_reg(cpu_ctx, RA);\n");
            printf("    }\n");
            break;

        case OP_AND:
        case OP_OR:
        case OP_XOR:
        case OP_NAND:
            printf("    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];\n");
            printf("    int16_t b = DO[0];\n");
            printf("    int16_t res;\n");
            
            if (def->op_type == OP_AND) {
                printf("    res = a & b;\n");
            } else if (def->op_type == OP_OR) {
                printf("    res = a | b;\n");
            } else if (def->op_type == OP_XOR) {
                printf("    res = a ^ b;\n");
            } else if (def->op_type == OP_NAND) {
                printf("    res = ~(a & b);\n");
            }
            
            printf("    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;\n");
            /* calculate_flags_16bit handles evaluating Pos/Neg/Zero and clearing Carry */
            printf("    calculate_flags_16bit(cpu_ctx, res);\n");
            break;
        case OP_ADD_FLOAT:
        case OP_SUB_FLOAT:
            printf("    int32_t M_A, M_B; int16_t E_A, E_B;\n");
            printf("    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);\n");
            printf("    unpack_float32(DO[0], DO[1], &M_B, &E_B);\n");
            if (def->op_type == OP_SUB_FLOAT)
            {
                printf("    M_B = -M_B;\n");
            }
            printf("    int32_t E_res;\n");
            printf("    if (M_A == 0) { M_A = M_B; E_res = E_B; }\n");
            printf("    else if (M_B == 0) { E_res = E_A; }\n");
            printf("    else {\n");
            printf("        int diff = E_A - E_B;\n");
            printf("        if (diff > 0) { if (diff > 24) M_B = 0; else M_B >>= diff; E_res = E_A; }\n");
            printf("        else if (diff < 0) { if (-diff > 24) M_A = 0; else M_A >>= -diff; E_res = E_B; }\n");
            printf("        else { E_res = E_A; }\n");
            printf("    }\n");
            printf("    pack_float32(cpu_ctx, RA, M_A + M_B, E_res);\n");
            break;

        case OP_MULT_FLOAT:
            printf("    int32_t M_A, M_B; int16_t E_A, E_B;\n");
            printf("    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);\n");
            printf("    unpack_float32(DO[0], DO[1], &M_B, &E_B);\n");
            printf("    int64_t P = (int64_t)M_A * (int64_t)M_B;\n");
            printf("    pack_float32(cpu_ctx, RA, (int32_t)(P >> 23), E_A + E_B);\n");
            break;

        case OP_DIV_FLOAT:
            printf("    int32_t M_A, M_B; int16_t E_A, E_B;\n");
            printf("    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);\n");
            printf("    unpack_float32(DO[0], DO[1], &M_B, &E_B);\n");
            printf("    if (M_B == 0) { cpu_ctx->state.reg.pir |= INTR_FLTOFL; return; }\n");
            printf("    uint64_t M_A_abs = (M_A < 0) ? -M_A : M_A;\n");
            printf("    uint64_t M_B_abs = (M_B < 0) ? -M_B : M_B;\n");
            printf("    uint64_t M_A_abs_shift = (M_A_abs << 23);\n");
            printf("    uint64_t Q_abs = M_A_abs_shift / M_B_abs;\n");
            printf("    int32_t RemNotZero = (M_A_abs_shift %% M_B_abs != 0)?1:0;\n"); // used to truncate toward negative infinity
            printf("    int32_t Q = ((M_A ^ M_B) < 0) ? -(int32_t)(Q_abs + RemNotZero) : (int32_t)Q_abs;\n");
            printf("    pack_float32(cpu_ctx, RA, Q, E_A - E_B);\n");
            break;
        case OP_ADD_EXFLOAT:
        case OP_SUB_EXFLOAT:
            printf("    int64_t M_A, M_B; int16_t E_A, E_B;\n");
            printf("    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M_A, &E_A);\n");
            printf("    unpack_float48(DO[0], DO[1], DO[2], &M_B, &E_B);\n");
            if (def->op_type == OP_SUB_EXFLOAT)
            {
                printf("    M_B = -M_B;\n");
            }
            printf("    int32_t E_res;\n");
            printf("    if (M_A == 0) { M_A = M_B; E_res = E_B; }\n");
            printf("    else if (M_B == 0) { E_res = E_A; }\n");
            printf("    else {\n");
            printf("        int diff = E_A - E_B;\n");
            printf("        if (diff > 0) { if (diff > 40) M_B = 0; else M_B >>= diff; E_res = E_A; }\n");
            printf("        else if (diff < 0) { if (-diff > 40) M_A = 0; else M_A >>= -diff; E_res = E_B; }\n");
            printf("        else { E_res = E_A; }\n");
            printf("    }\n");
            printf("    pack_float48(cpu_ctx, RA, M_A + M_B, E_res);\n");
            break;

        case OP_MULT_EXFLOAT:
            printf("    int64_t M_A, M_B; int16_t E_A, E_B;\n");
            printf("    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M_A, &E_A);\n");
            printf("    unpack_float48(DO[0], DO[1], DO[2], &M_B, &E_B);\n");
            printf("    __int128 P = (__int128)M_A * (__int128)M_B;\n");
            printf("    pack_float48(cpu_ctx, RA, (int64_t)(P >> 39), E_A + E_B);\n");
            break;

        case OP_DIV_EXFLOAT:
            printf("    int64_t M_A, M_B; int16_t E_A, E_B;\n");
            printf("    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M_A, &E_A);\n");
            printf("    unpack_float48(DO[0], DO[1], DO[2], &M_B, &E_B);\n");
            printf("    if (M_B == 0) { cpu_ctx->state.reg.pir |= INTR_FLTOFL; return; }\n");
            printf("    uint64_t M_A_abs = (M_A < 0) ? -M_A : M_A;\n");
            printf("    uint64_t M_B_abs = (M_B < 0) ? -M_B : M_B;\n");
            printf("    uint64_t div_hi = M_A_abs << 17;\n");
            printf("    uint64_t Q_hi = (div_hi / M_B_abs << 22);\n");
            printf("    uint64_t div_lo = (div_hi %% M_B_abs << 22);\n");
            printf("    uint64_t Q_abs = Q_hi + (div_lo / M_B_abs);\n");
            printf("    int64_t RemNotZero = (div_lo %% M_B_abs != 0)?1:0;\n");
            printf("    int64_t Q = ((M_A ^ M_B) < 0) ? -(int64_t)(Q_abs + RemNotZero) : (int64_t)Q_abs;\n");
            printf("    pack_float48(cpu_ctx, RA, Q, E_A - E_B);\n");
            break;
        case OP_INT16_TO_FLT:
            printf("    pack_float32(cpu_ctx, RA, (int16_t)cpu_ctx->state.reg.r[(RB+0)&0xF], 23);\n");
            break;

        case OP_INT32_TO_EFLT:
            printf("    int64_t val = ((int64_t)cpu_ctx->state.reg.r[(RB+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RB+1)&0xF];\n");
            printf("    pack_float48(cpu_ctx, RA, (val << 32) >> 32, 39);\n");
            break;

        case OP_FLT_TO_INT16:
            printf("    int32_t M; int16_t E;\n");
            printf("    unpack_float32(cpu_ctx->state.reg.r[(RB+0)&0xF], cpu_ctx->state.reg.r[(RB+1)&0xF], &M, &E);\n");
            printf("    int shift = E - 23;\n");
            printf("    int32_t int_val = (M == 0) ? 0 : ((shift >= 0) ? (M << shift) : ((-shift >= 24) ? 0 : (M / (1 << -shift))));\n");
            printf("    if (int_val > 32767 || int_val < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
            printf("    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)int_val;\n");
            printf("    calculate_flags_16bit(cpu_ctx, (int16_t)int_val);\n");
            break;

        case OP_EFLT_TO_INT32:
            printf("    int64_t M; int16_t E;\n");
            printf("    unpack_float48(cpu_ctx->state.reg.r[(RB+0)&0xF], cpu_ctx->state.reg.r[(RB+1)&0xF], cpu_ctx->state.reg.r[(RB+2)&0xF], &M, &E);\n");
            printf("    int shift = E - 39;\n");
            printf("    int64_t int_val = (M == 0) ? 0 : ((shift >= 0) ? (M << shift) : ((-shift >= 40) ? 0 : (M / (1LL << -shift))));\n");
            printf("    if (int_val > 2147483647LL || int_val < -2147483648LL) cpu_ctx->state.reg.pir |= INTR_FIXOFL;\n");
            printf("    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(int_val >> 16);\n");
            printf("    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(int_val & 0xFFFF);\n");
            printf("    calculate_flags_32bit_reg(cpu_ctx, RA);\n");
            break;
        case OP_EXCHANGE_WORD:
            /* XWR RA, RB - Exchange Word Register */
            printf("    uint16_t RB = opcode & 0x000F;\n");
            printf("    uint16_t tmp = cpu_ctx->state.reg.r[RA];\n");
            printf("    cpu_ctx->state.reg.r[RA] = cpu_ctx->state.reg.r[RB];\n");
            printf("    cpu_ctx->state.reg.r[RB] = tmp;\n");
            break;

        case OP_EXCHANGE_BYTE:
            /* XBR RA - Exchange Byte Register (Swaps high and low bytes of RA) */
            printf("    uint16_t val = cpu_ctx->state.reg.r[RA];\n");
            printf("    cpu_ctx->state.reg.r[RA] = ((val & 0xFF) << 8) | ((val >> 8) & 0xFF);\n");
            break;
        case OP_COMPARE:
            printf("    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */\n");
            if (def->operands_type == OPERAND_INT32) {
                printf("    int32_t A = ((int32_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF];\n");
                printf("    int32_t B = ((int32_t)DO[0] << 16) | (uint16_t)DO[1];\n");
                printf("    if (A == B) cpu_ctx->state.reg.sw |= CS_ZERO;\n");
                printf("    else if (A > B) cpu_ctx->state.reg.sw |= CS_POSITIVE;\n");
                printf("    else cpu_ctx->state.reg.sw |= CS_NEGATIVE;\n");
            } else {
                printf("    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];\n");
                printf("    int16_t B = (int16_t)DO[0];\n");
                printf("    if (A == B) cpu_ctx->state.reg.sw |= CS_ZERO;\n");
                printf("    else if (A > B) cpu_ctx->state.reg.sw |= CS_POSITIVE;\n");
                printf("    else cpu_ctx->state.reg.sw |= CS_NEGATIVE;\n");
            }
            break;

        case OP_COMP_LIM:
            /* Compare Between Limits (CBL) */
            printf("    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */\n");


                printf("    int16_t L = (int16_t)DO[0];\n");
                printf("    int16_t U = (int16_t)DO[1];\n");
                printf("    if (L > U) cpu_ctx->state.reg.sw |= CS_CARRY;\n");
                printf("    else {\n");
                printf("        int16_t A = (int16_t)cpu_ctx->state.reg.r[RA];\n");
                printf("        if (A < L) cpu_ctx->state.reg.sw |= CS_NEGATIVE;\n");
                printf("        else if (A > U) cpu_ctx->state.reg.sw |= CS_POSITIVE;\n");
                printf("        else cpu_ctx->state.reg.sw |= CS_ZERO;\n");
                printf("    }\n");
            break;
        case OP_COMPARE_FLOAT:
            printf("    int32_t M_A, M_B; int16_t E_A, E_B;\n");
            printf("    unpack_float32(cpu_ctx->state.reg.r[RA], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);\n");
            printf("    unpack_float32(DO[0], DO[1], &M_B, &E_B);\n");
            printf("    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */\n");
            printf("    if (M_A == 0 && M_B == 0) {\n");
            printf("        cpu_ctx->state.reg.sw |= CS_ZERO;\n");
            printf("    } else {\n");
            printf("        int diff = E_A - E_B;\n");
            printf("        int64_t m_a_ext = M_A;\n");
            printf("        int64_t m_b_ext = M_B;\n");
            printf("        if (diff > 0) { if (diff > 24) m_b_ext = 0; else m_b_ext >>= diff; }\n");
            printf("        else if (diff < 0) { if (-diff > 24) m_a_ext = 0; else m_a_ext >>= -diff; }\n");
            printf("        int64_t res = m_a_ext - m_b_ext;\n");
            printf("        if (res == 0) cpu_ctx->state.reg.sw |= CS_ZERO;\n");
            printf("        else if (res > 0) cpu_ctx->state.reg.sw |= CS_POSITIVE;\n");
            printf("        else cpu_ctx->state.reg.sw |= CS_NEGATIVE;\n");
            printf("    }\n");
            break;

        case OP_COMPARE_EXFLOAT:
            printf("    int64_t M_A, M_B; int16_t E_A, E_B;\n");
            printf("    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M_A, &E_A);\n");
            printf("    unpack_float48(DO[0], DO[1], DO[2], &M_B, &E_B);\n");
            printf("    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */\n");
            printf("    if (M_A == 0 && M_B == 0) {\n");
            printf("        cpu_ctx->state.reg.sw |= CS_ZERO;\n");
            printf("    } else {\n");
            printf("        int diff = E_A - E_B;\n");
            printf("        int64_t m_a_ext = M_A;\n");
            printf("        int64_t m_b_ext = M_B;\n");
            printf("        if (diff > 0) { if (diff > 40) m_b_ext = 0; else m_b_ext >>= diff; }\n");
            printf("        else if (diff < 0) { if (-diff > 40) m_a_ext = 0; else m_a_ext >>= -diff; }\n");
            printf("        int64_t res = m_a_ext - m_b_ext;\n");
            printf("        if (res == 0) cpu_ctx->state.reg.sw |= CS_ZERO;\n");
            printf("        else if (res > 0) cpu_ctx->state.reg.sw |= CS_POSITIVE;\n");
            printf("        else cpu_ctx->state.reg.sw |= CS_NEGATIVE;\n");
            printf("    }\n");
            break;
        case OP_JUMP_COND:
            printf("    uint16_t mask = N << 12; /* align it with cs in sw reg */\n");
            /*
            0000   0     NOP                                --   --   --
            0001   1     less than (zero)                   LT   LZ   M
            0010   2     equal to (zero)                    EQ   EZ   --
            0011   3     less than or equal to (zero)       LE   LEZ  NP
            0100   4     greater than (zero)                GT   GZ   P
            0101   5     not equal to (zero)                NE   NZ   --
            0110   6     greater than or equal to (zero)    GE   GEZ  NM
            0111   7     unconditional                      --   --   --
            1000   8     carry                              CY   --   --
            1001   9     carry or LT                        --   --   --
            1010   A     carry or EQ                        --   --   --
            1011   B     carry or LE                        --   --   --
            1100   C     carry or GT                        --   --   --
            1101   D     carry or NE                        --   --   --
            1110   E     carry or GE                        --   --   --
            1111   F     unconditional                      --   --   --
            */
            
            printf("    if ((mask & 0x7000) == 0x7000 || (cpu_ctx->state.reg.sw & mask) != 0) {\n");
            printf("        cpu_ctx->state.reg.ic = DO_ADDR;\n");
            printf("    } else {\n");
            printf("        cpu_ctx->state.reg.ic += 2;\n"); // JC and JCI has an immediate
            printf("    }\n");
            break;
        case OP_SUBTR_JUMP:
            printf("    uint16_t val = cpu_ctx->state.reg.r[RA] - 1;\n");
            printf("    cpu_ctx->state.reg.r[RA] = val;\n");
            printf("    calculate_flags_16bit(cpu_ctx, (int16_t)val);\n");
            
            /* If the loop counter hasn't hit 0, jump to the loop start */
            printf("    if (val != 0) {\n");
            printf("        cpu_ctx->state.reg.ic = DO_ADDR;\n");
            printf("    } else {\n");
            /* Loop finished: fall through to the next instruction */
            printf("        cpu_ctx->state.reg.ic += 2;\n");
            printf("    }\n");
            break;
        case OP_JUMP_SUBRTN:
            if (def->code == OPC_SJS)
            {
                printf("    cpu_ctx->state.reg.r[RA] -= 1;\n");
                printf("    ok = store_data_word(cpu_ctx, cpu_ctx->state.reg.r[RA], cpu_ctx->state.reg.ic + 2);\n");
            }
            if (def->code == OPC_JS)
            {
                printf("    cpu_ctx->state.reg.r[RA] = cpu_ctx->state.reg.ic + 2;\n");
                
            }
            printf("    cpu_ctx->state.reg.ic = DO_ADDR;\n");
            break;
        case OP_RET_SUBRTN:
            printf("    ok = fetch_data_word(cpu_ctx, cpu_ctx->state.reg.r[RA], &cpu_ctx->state.reg.ic);\n");
            printf("    cpu_ctx->state.reg.r[RA] += 1;\n");
            break;
        case OP_LOAD_STATUS:
            /* DO[0] -> Status Word, DO[1] -> Mask Register */
            printf("    if ((DO[1] ^ cpu_ctx->state.reg.sw) & 0xFF ) { invalidate_mem_cache(cpu_ctx); }\n");
            printf("    cpu_ctx->state.reg.mk = DO[0];\n");
            printf("    cpu_ctx->state.reg.sw = DO[1];\n");
            printf("    cpu_ctx->state.reg.ic = DO[2];\n");
            break;
        case OP_MULT_REG:
            switch(def->code)
            {
                case OPC_STM:
                    printf("    ok = 0 == store_data_words_reg(cpu_ctx, DO_ADDR, N +1, 0 /* reg 0 */);\n");
                    break;
                case OPC_LM:
                    printf("    ok = 0 == fetch_data_words_reg(cpu_ctx, DO_ADDR, N +1, 0 /* reg 0 */);\n");
                    break;
                case OPC_PSHM:
                /*
                R15 is decremented before each push.
                If R15 is going to be pushed, we need to calculate
                it's value at the moment of insertion
                we start from RB backwards toward RA
                if RA > RB than after R0 we skip to R15.
                this is perfect to our store_data_words_reg
                function.
                if RA = RB we only push one register
                int count = RB - RA + 1;
                
                if (count < 0)
                    count += 16;
                int addr = r[15] - count;
                if (RA + count > 15) // R15 will be inserted
                    r[15] -= RA + count - 15; // this will be it's value of R15 on the stack
                do store from RA with count to addr
                    r[15] = addr;
                */
                    printf("    int count = RB - RA + 1;\n");
                    printf("    if (count < 0)\n");
                    printf("        count += 16;\n");
                    printf("    int stk_addr = cpu_ctx->state.reg.r[15] - count;\n");
                    printf("    if (RA + count > 15) /* R15 will be inserted */\n");
                    printf("        cpu_ctx->state.reg.r[15] -= RA + count - 15;\n");
                    printf("    ok = 0 == store_data_words_reg(cpu_ctx, stk_addr, count, RA);\n");
                    /* set R15 value*/
                    printf("    cpu_ctx->state.reg.r[15] = stk_addr;\n");
                    break;
                case OPC_POPM: /* the exact mirror of PSHM*/
                /* POPM is the exact mirror of PSHM. We start reading at the current R15.
                    Registers are popped in ascending order, from RA up to RB.
                    If R15 is popped, it is skipped
                    */
                    /* POPM: R15 is the stack pointer. Registers are popped sequentially. */
                    printf("    int count = RB - RA + 1;\n");
                    printf("    if (count < 0)\n");
                    printf("        count += 16;\n");
                    printf("    int stk_addr = cpu_ctx->state.reg.r[15];\n");
                    
                    /* Fetch all data words into registers. 
                       If R15 is in the list, it gets temporarily overwritten here... */
                    printf("    ok = 0 == fetch_data_words_reg(cpu_ctx, stk_addr, count, RA);\n");
                    
                    /* ...but the manual states R15 is effectively ignored as a destination. 
                       We fix it by unconditionally advancing the stack pointer by the total count! */
                    printf("    cpu_ctx->state.reg.r[15] = stk_addr + count;\n");
                    break;
                default:
                    break;
            }
            break;
        case OP_STORE_MASK:
            /* SRM: Store Register through Mask */
            /* RA holds the data. RA+1 holds the mask. */
            printf("    uint16_t data = cpu_ctx->state.reg.r[RA];\n");
            printf("    uint16_t mask = cpu_ctx->state.reg.r[(RA + 1) & 0xF];\n");
            
            
            /* * For each 1 in the mask, use the bit from 'data'.
             * For each 0 in the mask, keep the bit from 'mem_val'.
             */
            printf("    uint16_t new_val = (DO[0] & ~mask) | (data & mask);\n");
            
            /* Store the modified word back to memory */
            printf("    store_data_word(cpu_ctx, DO_ADDR, new_val);\n"); 
            /* although we've been in this address when we read, we still need to check write permissions */
            break;
        case OP_NEG_FLOAT:
            printf("    int32_t M; int16_t E;\n");
            printf("    unpack_float32(cpu_ctx->state.reg.r[(RB+0)&0xF], cpu_ctx->state.reg.r[(RB+1)&0xF], &M, &E);\n");
            
            printf("    if (M == -8388608 && E == 127) {\n");
            printf("        cpu_ctx->state.reg.pir |= INTR_FLTOFL;\n");
            printf("        E = 127; M = 0x7FFFFF;\n");
            printf("    } else if (M == 4194304 && E == -128) {\n");
            printf("        cpu_ctx->state.reg.pir |= INTR_FLTUFL;\n");
            printf("        E = 0; M = 0;\n");
            printf("    } else if (M == -8388608) {\n");
            printf("        E = E + 1; M = 4194304;\n");
            printf("    } else if (M == 4194304) {\n");
            printf("        E = E - 1; M = -8388608;\n");
            printf("    } else {\n");
            printf("        M = -M;\n");
            printf("    }\n");

            /* Let the pack function handle the CS flags! */
            printf("    pack_float32(cpu_ctx, RA, M, E);\n");
            break;
        case OP_ABS_FLOAT:
            printf("    int32_t M; int16_t E;\n");
            printf("    unpack_float32(cpu_ctx->state.reg.r[(RB+0)&0xF], cpu_ctx->state.reg.r[(RB+1)&0xF], &M, &E);\n");
            
            printf("    if (M == -8388608) {\n");
            printf("        E = E + 1; M = 4194304;\n");
            printf("        if (E > 127) {\n");
            printf("            cpu_ctx->state.reg.pir |= INTR_FLTOFL;\n");
            printf("            E = 127; M = 0x7FFFFF;\n");
            printf("        }\n");
            printf("    } else if (M < 0) {\n");
            printf("        M = -M;\n");
            printf("    }\n");

            /* Let the pack function handle the CS flags! */
            printf("    pack_float32(cpu_ctx, RA, M, E);\n");
            break;
        case OP_XIO:
            if(def->code == OPC_XIO)
            {
                printf("    process_xio (cpu_ctx, DO[0], (ushort *) &cpu_ctx->state.reg.r[RA]);\n");
            }
            else /*VIO*/
            {
                /*for VIO we will use get_addresses_data for the IO data word */
                printf("    struct {\n");
                printf("        uint16_t io_cmd;\n");
                printf("        uint16_t vector_select;\n");
                printf("    } vio;\n");
                printf("    fetch_data_words(cpu_ctx, DO_ADDR, 2, (int16_t *)&vio);\n");
                printf("    uint16_t vector = vio.vector_select;\n");
                printf("    uint16_t cmd_inc = cpu_ctx->state.reg.r[RA];\n");
                printf("    uint16_t current_cmd = vio.io_cmd;\n");
                printf("    for (int i = 0; i < 16; i++) {\n");
                printf("        if (vector & (0x8000 >> i)) {\n");
                printf("            process_xio(cpu_ctx, current_cmd, &cpu_ctx->state.reg.r[i]);\n");
                printf("            current_cmd += cmd_inc;\n");
                printf("        }\n");
                printf("    }\n");
            }
            break;
        case OP_MOVE:
        /*TODO: check how many we can move until we have timer interrupt */
            printf("    uint16_t count = (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF]; /* count range is 0 - 2^16-1*/\n");
            printf("    uint16_t total_written = 0;\n");
            printf("    const int move_steps_1_tick = (TIMER_A_RES_IN_10uSEC*10000)/(CLK_CYC_MOV_STEP(1)*CYCLE_DURATION_IN_NS);\n");
            printf("    const int move_steps = move_steps_1_tick == 0 ? 1 : move_steps_1_tick;\n");
            printf("    cpu_ctx->state.total_cycles += CLK_CYC_MOV_INIT;\n");
            printf("    while(count > 0){\n");
            printf("        uint16_t partial_count = (count > move_steps)? move_steps: count;\n");
            printf("        uint16_t remaining = move_words(cpu_ctx, cpu_ctx->state.reg.r[RB], cpu_ctx->state.reg.r[RA], partial_count);\n");
            printf("        total_written += partial_count - remaining;\n");
            printf("        count -= partial_count - remaining;\n");
            printf("        cpu_ctx->state.total_cycles += CLK_CYC_MOV_STEP(partial_count - remaining);\n");
            printf("        calculate_timers(cpu_ctx);\n");
            printf("        if (has_pending_interrupt(cpu_ctx)) break;\n");
            printf("    }\n");
            printf("    cpu_ctx->state.reg.r[(RA+1)&0xF] = count;\n");
            printf("    cpu_ctx->state.reg.r[RA] += total_written;\n");
            printf("    cpu_ctx->state.reg.r[RB] += total_written;\n");
            printf("    if (count == 0){\n");
            printf("        cpu_ctx->state.total_cycles += CLK_CYC_MOV_FINI(total_written);\n");
            printf("        cpu_ctx->state.reg.ic +=1;\n");
            printf("    }\n");
            break;
        case OP_BIF:
            printf("    BIF_logic(cpu_ctx, opcode, %s);\n", def->is_imm? "imm_value" : "0");
            break;
        case OP_BR_EXECUTIVE:
              printf("    cpu_ctx->state.bex_index = (opcode & 0xF);\n");
              printf("    cpu_ctx->state.reg.pir |= INTR_BEX;\n");
            break;
        case OP_EXTENSION:
            printf("    switch (opcode & 0xFF){\n");
            printf("      case 0x00:  /*NOP*/\n");
            printf("        cpu_ctx->state.reg.ic +=1;\n");
            printf("        break;\n");
            printf("      case 0xFF:  /* BPT */\n");
            printf("        cpu_ctx->state.reg.ic +=1;\n");
            printf("        cpu_ctx->state.halt = true;\n");
            printf("        break;\n");
            printf("      default:\n");
            printf("        interpret_ILLEGAL(cpu_ctx, opcode, 0);\n");
            printf("        break;\n");
            printf("    }\n");

        default:
            printf("    /* TODO: Implement interpretation logic for %s */\n", def->name);
            break; 
    }
    if (priviledged) /* close the brackets of the else */
    {
        printf("    }\n");
    }
    
    if (!complicated_clock_cycles_calc)
        printf("    cpu_ctx->state.total_cycles += CLK_CYC_%s;\n", def->name);
      
    if (def->op_type != OP_BRANCH && def->op_type != OP_JUMP_SUBRTN && def->op_type != OP_RET_SUBRTN && def->op_type != OP_LOAD_STATUS 
        && def->op_type != OP_EXTENSION && def->op_type != OP_SPECIAL && def->op_type != OP_LOAD_STATUS &&
        def->op_type != OP_MOVE)
    {
        if (def->is_imm)
        {
            printf("    cpu_ctx->state.reg.ic += 2;\n");
        }
        else
        {
            printf("    cpu_ctx->state.reg.ic += 1;\n");
        }
    }



    printf("}\n\n");
}

void generate_interpreter_code()
{
    printf("#include \"cpu_helpers.h\"\n");
    printf("#include \"clock_cycles.h\"\n");
    
    for (int i = 0; i < 16; i++) {
        OpcodeDef *def = &opcode_defs_6bit[i];
        if (!def->valid) continue;
        emit_instruction(def);
    }
    for (int i = 0; i < 16; i++) {
        OpcodeDef *def = &opcode_defs_brx[i];
        if (!def->valid) continue;
        emit_instruction(def);
    }
    emit_instruction(&opcode_defs_8bit[4]); //XIO
    emit_instruction(&opcode_defs_8bit[5]); //VIO
    for (int i = 0; i < 16; i++) {
        OpcodeDef *def = &opcode_defs_imm[i];
        if (!def->valid) continue;
        emit_instruction(def);
    }

    for (int i = 11 ; i < 188; i++) {
        
        OpcodeDef *def = &opcode_defs_8bit[i];
        if (!def->valid) continue;
        emit_instruction(def);
    }
    
    {
        
    printf("void interpret_BRX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {\n");
    printf("    switch ((opcode & 0x00F0) >> 4) {\n");
    for (int i = 0; i < 16; i++) {
        printf("      case 0x%1X:\n", i);
        OpcodeDef *def = &opcode_defs_brx[i];
        if (!def->valid) continue;
        printf("        interpret_%s(cpu_ctx, opcode, imm_value);\n", def->name);
    }
    printf("      default:\n");
    printf("        interpret_ILLEGAL(cpu_ctx, opcode, imm_value);\n");
    printf("    }\n");
    printf("}\n");
    }
    // now for imm
    {
    printf("void interpret_IMM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {\n");
    printf("    switch (opcode & 0x000F) {\n");
    for (int i = 0; i < 16; i++) {
        printf("      case 0x%1X:\n", i);
        OpcodeDef *def = &opcode_defs_imm[i];
        if (!def->valid) continue;
        printf("        interpret_%s(cpu_ctx, opcode, imm_value);\n", def->name);
    }
    printf("      default:\n");
    printf("        interpret_ILLEGAL(cpu_ctx, opcode, imm_value);\n");
    printf("    }\n");
    printf("}\n");
    }
    /* now create the opcode func map*/
    printf("static void (*op_func_map[256])(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) = {\n");
    int opcode = 0;
    for (int i = 0; i < 17; i++) {
        OpcodeDef *def = &opcode_defs_6bit[i];
        /* because these are 6 bit opcodes, we print them 4 times to fit 8bit opcodes*/
        for (int j = 0 ; j < 4; j++)
        {            
            if (!def->valid) 
            printf("interpret_ILLEGAL, /* 0x%02X */\n",opcode);
            else
            printf("interpret_%s, /* 0x%02X */\n", def->name, opcode);
            opcode++;
        }
    }
    for (int i = 0 ; i < 188; i++) {
        
        OpcodeDef *def = &opcode_defs_8bit[i];
        if (!def->valid) 
        printf("interpret_ILLEGAL, /* 0x%02X */\n",opcode);
        else
        printf("interpret_%s, /* 0x%02X */\n", def->name, opcode);
        opcode++;
    }
    printf("};\n");
    
}



int main()
{
    generate_interpreter_code();
    return 0;
}
