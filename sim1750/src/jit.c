#include "jit.h"
#include "cpu_ctx.h"
#include "peekpoke.h"
#include "smemacc.h"
#include <stdio.h>

OpcodeDef opcode_defs_brx[16] = {
        {"LBX", 3, CS_W_ALL,  false, true, true, false, IF_BASE_REL_INDX, OP_LOAD, AM_BASE_REL_IDX_BX, OPERAND_INT16},
        {"DLBX", 3, CS_W_ALL,  false, true, true, false, IF_BASE_REL_INDX, OP_LOAD, AM_BASE_REL_IDX_BX, OPERAND_INT32},
        {"STBX", 3, CS_RW_NN,  false, true, true, false, IF_BASE_REL_INDX, OP_STORE, AM_BASE_REL_IDX_BX, OPERAND_INT16},
        {"DSTX", 3, CS_RW_NN,  false, true, true, false, IF_BASE_REL_INDX, OP_STORE, AM_BASE_REL_IDX_BX, OPERAND_INT32},
        {"ABX", 4, CS_W_ALL,  false, true, true, false, IF_BASE_REL_INDX, OP_ADD, AM_BASE_REL_IDX_BX, OPERAND_INT16},
        {"SBBX", 4, CS_W_ALL,  false, true, true, false, IF_BASE_REL_INDX, OP_SUB, AM_BASE_REL_IDX_BX, OPERAND_INT16},
        {"MBX", 5, CS_W_ALL,  false, true, true, false, IF_BASE_REL_INDX, OP_MULT_PROD32, AM_BASE_REL_IDX_BX, OPERAND_INT16},
        {"DBX", 5, CS_W_ALL,  false, true, true, false, IF_BASE_REL_INDX, OP_DIV_REM_32_16, AM_BASE_REL_IDX_BX, OPERAND_INT16},
        {"FABX", 6, CS_W_ALL,  false, true, true, false, IF_BASE_REL_INDX, OP_ADD_FLOAT, AM_BASE_REL_IDX_BX, OPERAND_FLOAT},
        {"FSBX", 6, CS_W_ALL,  false, true, true, false, IF_BASE_REL_INDX, OP_SUB_FLOAT, AM_BASE_REL_IDX_BX, OPERAND_FLOAT},
        {"FMBX", 6, CS_W_ALL,  false, true, true, false, IF_BASE_REL_INDX, OP_MULT_FLOAT, AM_BASE_REL_IDX_BX, OPERAND_FLOAT},
        {"FDBX", 7, CS_W_ALL,  false, true, true, false, IF_BASE_REL_INDX, OP_DIV_FLOAT, AM_BASE_REL_IDX_BX, OPERAND_FLOAT},
        {"CBX", 3, CS_W_ALL,  false, true, true, false, IF_BASE_REL_INDX, OP_COMPARE, AM_BASE_REL_IDX_BX, OPERAND_INT16},
        {"FCBX", 6, CS_W_ALL,  false, true, true, false, IF_BASE_REL_INDX, OP_COMPARE_FLOAT, AM_BASE_REL_IDX_BX, OPERAND_FLOAT},
        {"ANDX", 3, CS_W_ALL,  false, true, true, false, IF_BASE_REL_INDX, OP_AND, AM_BASE_REL_IDX_BX, OPERAND_INT16},
        {"ORBX", 3, CS_W_ALL,  false, true, true, false, IF_BASE_REL_INDX, OP_OR, AM_BASE_REL_IDX_BX, OPERAND_INT16},
};
OpcodeDef opcode_defs_imm[16] = {
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"AIM", 2, CS_W_ALL,  false, true, true, true, IF_IMMEDIATE, OP_ADD, AM_IMM_LONG_IM_IMX, OPERAND_INT16},
        {"SIM", 2, CS_W_ALL,  false, true, true, true, IF_IMMEDIATE, OP_SUB, AM_IMM_LONG_IM_IMX, OPERAND_INT16},
        {"MIM", 3, CS_W_ALL,  false, true, true, true, IF_IMMEDIATE, OP_MULT_PROD32, AM_IMM_LONG_IM_IMX, OPERAND_INT16},
        {"MSIM", 4, CS_W_ALL,  false, true, true, true, IF_IMMEDIATE, OP_MULT_PROD16, AM_IMM_LONG_IM_IMX, OPERAND_INT16},
        {"DIM", 4, CS_W_ALL,  false, true, true, true, IF_IMMEDIATE, OP_DIV_REM_32_16, AM_IMM_LONG_IM_IMX, OPERAND_INT16},
        {"DVIM", 5, CS_W_ALL,  false, true, true, true, IF_IMMEDIATE, OP_DIV_REM_16_16, AM_IMM_LONG_IM_IMX, OPERAND_INT16},
        {"ANDM", 3, CS_W_ALL,  false, true, true, true, IF_IMMEDIATE, OP_AND, AM_IMM_LONG_IM_IMX, OPERAND_INT16},
        {"ORIM", 3, CS_W_ALL,  false, true, true, true, IF_IMMEDIATE, OP_OR, AM_IMM_LONG_IM_IMX, OPERAND_INT16},
        {"XORM", 3, CS_W_ALL,  false, true, true, true, IF_IMMEDIATE, OP_XOR, AM_IMM_LONG_IM_IMX, OPERAND_INT16},
        {"CIM", 3, CS_W_ALL,  false, true, true, true, IF_IMMEDIATE, OP_COMPARE, AM_IMM_LONG_IM_IMX, OPERAND_INT16},
        {"NIM", 3, CS_W_ALL,  false, true, true, true, IF_IMMEDIATE, OP_NAND, AM_IMM_LONG_IM_IMX, OPERAND_INT16},
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false},
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false},
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false},
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false},
};
/* 0x00 - 0x43 */
OpcodeDef opcode_defs_6bit[17] = {
        /* 00 - 3F */
        {"LB", 3, CS_W_ALL,  false, true, true, false, IF_BASE_RELATIVE, OP_LOAD, AM_BASE_REL_B, OPERAND_INT16},
        {"DLB", 3, CS_W_ALL,  false, true, true, false, IF_BASE_RELATIVE, OP_LOAD, AM_BASE_REL_B, OPERAND_INT32},
        {"STB", 3, CS_RW_NN,  false, true, true, false, IF_BASE_RELATIVE, OP_STORE, AM_BASE_REL_B, OPERAND_INT16},
        {"DSTB", 3, CS_RW_NN,  false, true, true, false, IF_BASE_RELATIVE, OP_STORE, AM_BASE_REL_B, OPERAND_INT32},
        {"AB", 4, CS_W_ALL,  false, true, true, false, IF_BASE_RELATIVE, OP_ADD, AM_BASE_REL_B, OPERAND_INT16},
        {"SBB", 4, CS_W_ALL,  false, true, true, false, IF_BASE_RELATIVE, OP_SUB, AM_BASE_REL_B, OPERAND_INT16},
        {"MB", 5, CS_W_ALL,  false, true, true, false, IF_BASE_RELATIVE, OP_MULT_PROD32, AM_BASE_REL_B, OPERAND_INT16},
        {"DB", 5, CS_W_ALL,  false, true, true, false, IF_BASE_RELATIVE, OP_DIV_REM_32_16, AM_BASE_REL_B, OPERAND_INT16},
        {"FAB", 6, CS_W_ALL,  false, true, true, false, IF_BASE_RELATIVE, OP_ADD_FLOAT, AM_BASE_REL_B, OPERAND_FLOAT},
        {"FSB", 6, CS_W_ALL,  false, true, true, false, IF_BASE_RELATIVE, OP_SUB_FLOAT, AM_BASE_REL_B, OPERAND_FLOAT},
        {"FMB", 6, CS_W_ALL,  false, true, true, false, IF_BASE_RELATIVE, OP_MULT_FLOAT, AM_BASE_REL_B, OPERAND_FLOAT},
        {"FDB", 7, CS_W_ALL,  false, true, true, false, IF_BASE_RELATIVE, OP_DIV_FLOAT, AM_BASE_REL_B, OPERAND_FLOAT},
        {"ORB", 3, CS_W_ALL,  false, true, true, false, IF_BASE_RELATIVE, OP_OR, AM_BASE_REL_B, OPERAND_INT16},
        {"ANDB", 3, CS_W_ALL,  false, true, true, false, IF_BASE_RELATIVE, OP_AND, AM_BASE_REL_B, OPERAND_INT16},
        {"CB", 3, CS_W_ALL,  false, true, true, false, IF_BASE_RELATIVE, OP_COMPARE, AM_BASE_REL_B, OPERAND_INT16},
        {"FCB", 6, CS_W_ALL,  false, true, true, false, IF_BASE_RELATIVE, OP_COMPARE_FLOAT, AM_BASE_REL_B, OPERAND_FLOAT},
        {"BRX", 3, CS_RW_NN,  false, true, true, false, IF_BASE_REL_INDX, OP_SPECIAL, AM_BASE_REL_IDX_BX} /*special - 
        becomes LBX, DLBX, STBX, DSTX, ABX, SBBX, MBX, DBX,
                FABX, FSBX, FMBX, FDBX, CBX, FCBX, ANDX, ORBX */
};
/* starting from 0x44 */
OpcodeDef opcode_defs_8bit[188] = {
        /* 44 - 4F */
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false},
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false},
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false},
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false},
        {"XIO", 0, CS_RW_NN,  false, true, true, true, IF_LONG, OP_SPECIAL, AM_IMM_LONG_IM_IMX, OPERAND_INT16}, /* 0x48 */ /* we can jit it actually if RX is 0 */
        {"VIO", 0, CS_RW_NN,  false, false, true, true, IF_LONG, OP_SPECIAL, AM_MEM_DIRECT_D_DX, OPERAND_INT16}, /* 0x49 */
        {"IMM", 0, CS_RW_NN,  false, false, true, false}, /* 0x4A */ /* special, see opcode_defs_imm */
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false}, /* 0x4B */
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false}, /* 0x4C */
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false}, /* 0x4D */
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false}, /* 0x4E */
        {"BIF", 2, CS_W_ALL,  false, false, true, false, IF_SPECIAL, OP_SPECIAL, AM_SPECIAL_S, OPERAND_INT16}, /* 0x4F */ /* not jitable */


            /* 50 - 5F */
        {"SB", 2, CS_RW_NN,  true, true, true, true, IF_CONST_LONG, OP_SET_BIT, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"SBR", 2, CS_RW_NN,  true, true, true, false, IF_CONST_REG, OP_SET_BIT, AM_REG_DIRECT_R, OPERAND_INT16},
        {"SBI", 2, CS_RW_NN,  true, true, true, true, IF_CONST_LONG, OP_SET_BIT, AM_MEM_INDIRECT_I_IX, OPERAND_INT16},
        {"RB", 2, CS_RW_NN,  true, true, true, true, IF_CONST_LONG, OP_RESET_BIT, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"RBR", 2, CS_RW_NN,  true, true, true, false, IF_CONST_REG, OP_RESET_BIT, AM_REG_DIRECT_R, OPERAND_INT16},
        {"RBI", 2, CS_RW_NN,  true, true, true, true, IF_CONST_LONG, OP_RESET_BIT, AM_MEM_INDIRECT_I_IX, OPERAND_INT16},
        {"TB", 2, CS_W_ALL,  true, true, true, true, IF_CONST_LONG, OP_TEST_BIT, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"TBR", 2, CS_W_ALL,  true, true, true, false, IF_CONST_REG, OP_TEST_BIT, AM_REG_DIRECT_R, OPERAND_INT16},
        {"TBI", 2, CS_W_ALL,  true, true, true, true, IF_CONST_LONG, OP_TEST_BIT, AM_MEM_INDIRECT_I_IX, OPERAND_INT16},
        {"TSB", 2, CS_W_ALL,  true, true, true, true, IF_CONST_LONG, OP_TEST_SET_BIT, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"SVBR", 2, CS_RW_NN,  true, true, true, false, IF_REG_REG, OP_SET_VAR_BIT, AM_REG_DIRECT_R, OPERAND_INT16},
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false},

        {"RVBR", 2, CS_RW_NN,  true, true, true, false, IF_REG_REG, OP_RESET_VAR_BIT, AM_REG_DIRECT_R, OPERAND_INT16},
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false},
        {"TVBR", 2, CS_W_ALL,  true, true, true, false, IF_REG_REG, OP_TEST_VAR_BIT, AM_REG_DIRECT_R, OPERAND_INT16},
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false},

        /* 60 - 6F */
        {"SLL", 2, CS_W_ALL,  false, true, true, false, IF_CONST_REG, OP_SHIFT, AM_REG_DIRECT_R, OPERAND_INT16},
        {"SRL", 2, CS_W_ALL,  false, true, true, false, IF_CONST_REG, OP_SHIFT, AM_REG_DIRECT_R, OPERAND_INT16},
        {"SRA", 2, CS_W_ALL,  false, true, true, false, IF_CONST_REG, OP_SHIFT, AM_REG_DIRECT_R, OPERAND_INT16},
        {"SLC", 2, CS_W_ALL,  false, true, true, false, IF_CONST_REG, OP_SHIFT, AM_REG_DIRECT_R, OPERAND_INT16},
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false},
        {"DSLL", 3, CS_W_ALL,  false, true, true, false, IF_CONST_REG, OP_SHIFT, AM_REG_DIRECT_R, OPERAND_INT32},
        {"DSRL", 3, CS_W_ALL,  false, true, true, false, IF_CONST_REG, OP_SHIFT, AM_REG_DIRECT_R, OPERAND_INT32},
        {"DSRA", 3, CS_W_ALL,  false, true, true, false, IF_CONST_REG, OP_SHIFT, AM_REG_DIRECT_R, OPERAND_INT32},
        {"DSLC", 3, CS_W_ALL,  false, true, true, false, IF_CONST_REG, OP_SHIFT, AM_REG_DIRECT_R, OPERAND_INT32},
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false},
        {"SLR", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_SHIFT, AM_REG_DIRECT_R, OPERAND_INT16},
        {"SAR", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_SHIFT, AM_REG_DIRECT_R, OPERAND_INT16},
        {"SCR", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_SHIFT, AM_REG_DIRECT_R, OPERAND_INT16},
        {"DSLR", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_SHIFT, AM_REG_DIRECT_R, OPERAND_INT32},
        {"DSAR", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_SHIFT, AM_REG_DIRECT_R, OPERAND_INT32},
        {"DSCR", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_SHIFT, AM_REG_DIRECT_R, OPERAND_INT32},
        
        /* 70 - 7F */
        {"JC", 2, CS_R_ALL,  true, true, true, true, IF_CONST_LONG, OP_JUMP_COND, AM_MEM_DIRECT_D_DX, OPERAND_INT16}, /* we can jit it only if RX is 0*/
        {"JCI", 2, CS_R_ALL,  true, false, true, true, IF_CONST_LONG, OP_JUMP_COND, AM_MEM_INDIRECT_I_IX, OPERAND_INT16}, /* we cannot jit it */
        {"JS", 2, CS_RW_NN,  true, false, true, true, IF_LONG, OP_JUMP_SUBRTN, AM_MEM_DIRECT_D_DX, OPERAND_INT16}, /* we can jit it only if RX is 0*/
        {"SOJ", 2, CS_W_ALL,  true, true, true, true, IF_CONST_LONG, OP_SUBTR_JUMP, AM_MEM_DIRECT_D_DX, OPERAND_INT16}, /* we can jit it only if RX is 0*/
        {"BR", 2, CS_RW_NN,  true, true, true, false, IF_COUNTER_RELATIVE, OP_BRANCH, AM_CNT_REL_ICR, OPERAND_INT16},
        {"BEZ", 2, CS_R_Z,  true, true, true, false, IF_COUNTER_RELATIVE, OP_BRANCH, AM_CNT_REL_ICR, OPERAND_INT16},
        {"BLT", 2, CS_R_N,  true, true, true, false, IF_COUNTER_RELATIVE, OP_BRANCH, AM_CNT_REL_ICR, OPERAND_INT16},
        {"BEX", 2, CS_W_ALL,  true, false, true, false, IF_REG_CONST, OP_SPECIAL, AM_SPECIAL_S}, /* we can jit it only if RX is 0*/
        {"BLE", 2, CS_R_NZ,  true, true, true, false, IF_COUNTER_RELATIVE, OP_BRANCH, AM_CNT_REL_ICR, OPERAND_INT16},
        {"BGT", 2, CS_R_P,  true, true, true, false, IF_COUNTER_RELATIVE, OP_BRANCH, AM_CNT_REL_ICR, OPERAND_INT16},
        {"BNZ", 2, CS_R_Z,  true, true, true, false, IF_COUNTER_RELATIVE, OP_BRANCH, AM_CNT_REL_ICR, OPERAND_INT16},
        {"BGE", 2, CS_R_PZ,  true, true, true, false, IF_COUNTER_RELATIVE, OP_BRANCH, AM_CNT_REL_ICR, OPERAND_INT16},
        {"LSTI", 2, CS_W_ALL,  true, false, true, true, IF_CONST_LONG, OP_LOAD_STATUS, AM_MEM_INDIRECT_I_IX, OPERAND_INT16},
        {"LST", 2, CS_W_ALL,  true, false, true, true, IF_CONST_LONG, OP_LOAD_STATUS, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"SJS", 2, CS_RW_NN,  true, true, true, false, IF_LONG, OP_JUMP_SUBRTN, AM_MEM_DIRECT_D_DX, OPERAND_INT16}, /* we can jit it only if RX is 0*/
        {"URS", 2, CS_RW_NN,  true, true, true, false, IF_REG_CONST, OP_JUMP_SUBRTN, AM_SPECIAL_S, OPERAND_INT16},
        /* 80 - 8F */
        {"L", 2, CS_W_ALL,  false, true, true, true, IF_LONG, OP_LOAD, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"LR", 2, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_LOAD, AM_REG_DIRECT_R, OPERAND_INT16},
        {"LISP", 2, CS_W_ALL,  false, true, true, false, IF_REG_CONST, OP_LOAD, AM_IMM_SHRT_POS_ISP, OPERAND_INT16},
        {"LISN", 2, CS_W_ALL,  false, true, true, false, IF_REG_CONST, OP_LOAD, AM_IMM_SHRT_NEG_ISN, OPERAND_INT16},
        {"LI", 2, CS_W_ALL,  false, true, true, true, IF_LONG, OP_LOAD, AM_MEM_INDIRECT_I_IX, OPERAND_INT16},
        {"LIM", 2, CS_W_ALL,  false, true, true, true, IF_LONG, OP_LOAD, AM_IMM_LONG_IM_IMX, OPERAND_INT16},
        {"DL", 3, CS_W_ALL,  false, true, true, true, IF_LONG, OP_LOAD, AM_MEM_DIRECT_D_DX, OPERAND_INT32},
        {"DLR", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_LOAD, AM_REG_DIRECT_R, OPERAND_INT32},
        {"DLI", 3, CS_W_ALL,  false, true, true, true, IF_LONG, OP_LOAD, AM_MEM_INDIRECT_I_IX, OPERAND_INT32},
        {"LM", 3, CS_RW_NN,  false, true, true, true, IF_CONST_LONG, OP_MULT_REG, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"EFL", 2, CS_W_ALL,  false, true, true, true, IF_LONG, OP_LOAD, AM_MEM_DIRECT_D_DX, OPERAND_EXFLOAT},
        {"LUB", 3, CS_W_ALL,  false, true, true, true, IF_LONG, OP_LOAD_BYTE, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"LLB", 3, CS_W_ALL,  false, true, true, true, IF_LONG, OP_LOAD_BYTE, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"LUBI", 3, CS_W_ALL,  false, true, true, true, IF_LONG, OP_LOAD_BYTE, AM_MEM_INDIRECT_I_IX, OPERAND_INT16},
        {"LLBI", 3, CS_W_ALL,  false, true, true, true, IF_LONG, OP_LOAD_BYTE, AM_MEM_INDIRECT_I_IX, OPERAND_INT16},
        {"POPM", 3, CS_RW_NN,  false, true, true, false, IF_CONST_LONG, OP_MULT_REG, AM_SPECIAL_S, OPERAND_INT16},
        /* 90 - 9F */
        {"ST", 2, CS_RW_NN,  false, true, true, true, IF_LONG, OP_STORE, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"STC", 3, CS_RW_NN,  false, true, true, true, IF_CONST_LONG, OP_STORE, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"STCI", 3, CS_RW_NN,  false, true, true, true, IF_CONST_LONG, OP_STORE, AM_MEM_INDIRECT_I_IX, OPERAND_INT16},
        {"MOV", 2, CS_RW_NN,  false, true, true, false, IF_REG_REG, OP_MOVE, AM_SPECIAL_S, OPERAND_INT16},
        {"STI", 2, CS_RW_NN,  false, true, true, true, IF_LONG, OP_STORE, AM_MEM_INDIRECT_I_IX, OPERAND_INT16},
        {"ILLEGAL", 0, CS_RW_NN,  false, false, false, false},
        {"DST", 3, CS_RW_NN,  false, true, true, true, IF_LONG, OP_STORE, AM_MEM_DIRECT_D_DX, OPERAND_INT32},
        {"SRM", 3, CS_RW_NN,  false, true, true, true, IF_LONG, OP_STORE_MASK, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"DSTI", 3, CS_RW_NN,  false, true, true, true, IF_LONG, OP_STORE, AM_MEM_INDIRECT_I_IX, OPERAND_INT32},
        {"STM", 3, CS_RW_NN,  false, true, true, true, IF_CONST_LONG, OP_MULT_REG, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"EFST", 3, CS_RW_NN,  false, true, true, true, IF_LONG, OP_STORE, AM_MEM_DIRECT_D_DX, OPERAND_EXFLOAT},
        {"STUB", 3, CS_RW_NN,  false, true, true, true, IF_LONG, OP_STORE_BYTE, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"STLB", 3, CS_RW_NN,  false, true, true, true, IF_LONG, OP_STORE_BYTE, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"SUBI", 2, CS_RW_NN,  false, true, true, true, IF_LONG, OP_STORE_BYTE, AM_MEM_INDIRECT_I_IX, OPERAND_INT16},
        {"SLBI", 3, CS_RW_NN,  false, true, true, true, IF_LONG, OP_STORE_BYTE, AM_MEM_INDIRECT_I_IX, OPERAND_INT16},
        {"PSHM", 3, CS_RW_NN,  false, true, true, false, IF_REG_REG, OP_MULT_REG, AM_SPECIAL_S, OPERAND_INT16},
        /* A0 - AF */ /* ADDITION/ ABSOLUTE*/
        {"A", 2, CS_W_ALL,  false, true, true, true, IF_LONG, OP_ADD, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"AR", 2, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_ADD, AM_REG_DIRECT_R, OPERAND_INT16},
        {"AISP", 2, CS_W_ALL,  false, true, true, false, IF_REG_CONST, OP_ADD, AM_IMM_SHRT_POS_ISP, OPERAND_INT16},
        {"INCM", 2, CS_W_ALL,  false, true, true, true, IF_CONST_LONG, OP_INC_DEC_MEM, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"ABS", 2, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_ABS, AM_REG_DIRECT_R, OPERAND_INT16},
        {"DABS", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_ABS, AM_REG_DIRECT_R, OPERAND_INT32},
        {"DA", 3, CS_W_ALL,  false, true, true, true, IF_LONG, OP_ADD, AM_MEM_DIRECT_D_DX, OPERAND_INT32},
        {"DAR", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_ADD, AM_REG_DIRECT_R, OPERAND_INT32},
        {"FA", 6, CS_W_ALL,  false, true, true, true, IF_LONG, OP_ADD_FLOAT, AM_MEM_DIRECT_D_DX, OPERAND_FLOAT},
        {"FAR", 6, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_ADD_FLOAT, AM_REG_DIRECT_R, OPERAND_FLOAT},
        {"EFA", 6, CS_W_ALL,  false, true, true, true, IF_LONG, OP_ADD_EXFLOAT, AM_MEM_DIRECT_D_DX, OPERAND_EXFLOAT},
        {"EFAR", 6, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_ADD_EXFLOAT, AM_REG_DIRECT_R, OPERAND_EXFLOAT},
        {"FABS", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_ABS_FLOAT, AM_REG_DIRECT_R, OPERAND_FLOAT},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        /* B0 - BF */ /* SUBTRACTION/ NEGATION */
        {"S", 2, CS_W_ALL,  false, true, true, true, IF_LONG, OP_SUB, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"SR", 2, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_SUB, AM_REG_DIRECT_R, OPERAND_INT16},
        {"SISP", 2, CS_W_ALL,  false, true, true, false, IF_REG_CONST, OP_SUB, AM_IMM_SHRT_POS_ISP, OPERAND_INT16},
        {"DECM", 2, CS_W_ALL,  false, true, true, true, IF_CONST_LONG, OP_INC_DEC_MEM, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"NEG", 2, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_NEG, AM_REG_DIRECT_R, OPERAND_INT16},
        {"DNEG", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_NEG, AM_REG_DIRECT_R, OPERAND_INT32},
        {"DS", 3, CS_W_ALL,  false, true, true, true, IF_LONG, OP_SUB, AM_MEM_DIRECT_D_DX, OPERAND_INT32},
        {"DSR", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_SUB, AM_REG_DIRECT_R, OPERAND_INT32},
        {"FS", 6, CS_W_ALL,  false, true, true, true, IF_LONG, OP_SUB_FLOAT, AM_MEM_DIRECT_D_DX, OPERAND_FLOAT},
        {"FSR", 6, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_SUB_FLOAT, AM_REG_DIRECT_R, OPERAND_FLOAT},
        {"EFS", 6, CS_W_ALL,  false, true, true, true, IF_LONG, OP_SUB_EXFLOAT, AM_MEM_DIRECT_D_DX, OPERAND_EXFLOAT},
        {"EFSR", 6, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_SUB_EXFLOAT, AM_REG_DIRECT_R, OPERAND_EXFLOAT},
        {"FNEG", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_NEG_FLOAT, AM_REG_DIRECT_R, OPERAND_FLOAT},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        /* C0 - CF */ /* MULTIPLICATION */
        {"MS", 7, CS_W_ALL,  false, true, true, true, IF_LONG, OP_MULT_PROD16, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"MSR", 7, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_MULT_PROD16, AM_REG_DIRECT_R, OPERAND_INT16},
        {"MISP", 7, CS_W_ALL,  false, true, true, false, IF_REG_CONST, OP_MULT_PROD16, AM_IMM_SHRT_POS_ISP, OPERAND_INT16},
        {"MISN", 7, CS_W_ALL,  false, true, true, false, IF_REG_CONST, OP_MULT_PROD16, AM_IMM_SHRT_NEG_ISN, OPERAND_INT16},
        {"M", 7, CS_W_ALL,  false, true, true, true, IF_LONG, OP_MULT_PROD32, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"MR", 7, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_MULT_PROD32, AM_REG_DIRECT_R, OPERAND_INT16},
        {"DM", 7, CS_W_ALL,  false, true, true, true, IF_LONG, OP_DMULT_PROD32, AM_MEM_DIRECT_D_DX, OPERAND_INT32},
        {"DMR", 7, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_DMULT_PROD32, AM_REG_DIRECT_R, OPERAND_INT32},
        {"FM", 8, CS_W_ALL,  false, true, true, true, IF_LONG, OP_MULT_FLOAT, AM_MEM_DIRECT_D_DX, OPERAND_FLOAT},
        {"FMR", 8, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_MULT_FLOAT, AM_REG_DIRECT_R, OPERAND_FLOAT},
        {"EFM", 8, CS_W_ALL,  false, true, true, true, IF_LONG, OP_MULT_EXFLOAT, AM_MEM_DIRECT_D_DX, OPERAND_EXFLOAT},
        {"EFMR", 8, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_MULT_EXFLOAT, AM_REG_DIRECT_R, OPERAND_EXFLOAT},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        /* D0 - DF */ /* DIVISION */
        {"DV", 8, CS_W_ALL,  false, true, true, true, IF_LONG, OP_DIV_REM_16_16, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"DVR", 8, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_DIV_REM_16_16, AM_REG_DIRECT_R, OPERAND_INT16},
        {"DISP", 8, CS_W_ALL,  false, true, true, false, IF_REG_CONST, OP_DIV_REM_16_16, AM_IMM_SHRT_POS_ISP, OPERAND_INT16},
        {"DISN", 8, CS_W_ALL,  false, true, true, false, IF_REG_CONST, OP_DIV_REM_16_16, AM_IMM_SHRT_NEG_ISN, OPERAND_INT16},
        {"D", 8, CS_W_ALL,  false, true, true, true, IF_LONG, OP_DIV_REM_32_16, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"DR", 8, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_DIV_REM_32_16, AM_REG_DIRECT_R, OPERAND_INT16},
        {"DD", 8, CS_W_ALL,  false, true, true, true, IF_LONG, OP_DIV_32_32, AM_MEM_DIRECT_D_DX, OPERAND_INT32},
        {"DDR", 8, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_DIV_32_32, AM_REG_DIRECT_R, OPERAND_INT32},
        {"FD", 8, CS_W_ALL,  false, true, true, true, IF_LONG, OP_DIV_FLOAT, AM_MEM_DIRECT_D_DX, OPERAND_FLOAT},
        {"FDR", 8, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_DIV_FLOAT, AM_REG_DIRECT_R, OPERAND_FLOAT},
        {"EFD", 8, CS_W_ALL,  false, true, true, true, IF_LONG, OP_DIV_EXFLOAT, AM_MEM_DIRECT_D_DX, OPERAND_EXFLOAT},
        {"EFDR", 8, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_DIV_EXFLOAT, AM_REG_DIRECT_R, OPERAND_EXFLOAT},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        /* E0 - EF */ /* LOGICAL OPERATIONS */
        {"OR", 3, CS_W_ALL,  false, true, true, true, IF_LONG, OP_OR, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"ORR", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_OR, AM_REG_DIRECT_R, OPERAND_INT16},
        {"AND", 3, CS_W_ALL,  false, true, true, true, IF_LONG, OP_AND, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"ANDR", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_AND, AM_REG_DIRECT_R, OPERAND_INT16},
        {"XOR", 3, CS_W_ALL,  false, true, true, true, IF_LONG, OP_XOR, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"XORR", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_XOR, AM_REG_DIRECT_R, OPERAND_INT16},
        {"N", 3, CS_W_ALL,  false, true, true, true, IF_LONG, OP_NAND, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"NR", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_NAND, AM_REG_DIRECT_R, OPERAND_INT16},
        /* FLOAT CONVERSION */
        {"FIX", 2, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_FLT_TO_INT16, AM_REG_DIRECT_R, OPERAND_FLOAT},
        {"FLT", 2, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_INT16_TO_FLT, AM_REG_DIRECT_R, OPERAND_FLOAT},
        {"EFIX", 2, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_EFLT_TO_INT32, AM_REG_DIRECT_R, OPERAND_EXFLOAT},
        {"EFLT", 2, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_INT32_TO_EFLT, AM_REG_DIRECT_R, OPERAND_EXFLOAT},
        /* EXCHANGE */
        {"XBR", 2, CS_W_ALL,  true, true, true, false, IF_REG_CONST, OP_EXCHANGE_BYTE, AM_SPECIAL_S, OPERAND_INT16},
        {"XWR", 2, CS_W_ALL,  true, true, true, false, IF_REG_CONST, OP_EXCHANGE_WORD, AM_REG_DIRECT_R, OPERAND_INT16},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        /* F0 - FF */ /* COMPARISON */
        {"C", 2, CS_W_ALL,  false, true, true, true, IF_LONG, OP_COMPARE, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"CR", 2, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_COMPARE, AM_REG_DIRECT_R, OPERAND_INT16},
        {"CISP", 2, CS_W_ALL,  false, true, true, false, IF_REG_CONST, OP_COMPARE, AM_IMM_SHRT_POS_ISP, OPERAND_INT16},
        {"CISN", 2, CS_W_ALL,  false, true, true, false, IF_REG_CONST, OP_COMPARE, AM_IMM_SHRT_NEG_ISN, OPERAND_INT16},
        {"CBL", 3, CS_W_ALL,  false, true, true, true, IF_LONG, OP_COMP_LIM, AM_MEM_DIRECT_D_DX, OPERAND_INT16},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"DC", 3, CS_W_ALL,  false, true, true, true, IF_LONG, OP_COMPARE, AM_MEM_DIRECT_D_DX, OPERAND_INT32},
        {"DCR", 3, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_COMPARE, AM_REG_DIRECT_R, OPERAND_INT32},
        {"FC", 6, CS_W_ALL,  false, true, true, true, IF_LONG, OP_COMPARE_FLOAT, AM_MEM_DIRECT_D_DX, OPERAND_FLOAT},
        {"FCR", 6, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_COMPARE_FLOAT, AM_REG_DIRECT_R, OPERAND_FLOAT},
        {"EFC", 6, CS_W_ALL,  false, true, true, true, IF_LONG, OP_COMPARE_EXFLOAT, AM_MEM_DIRECT_D_DX, OPERAND_EXFLOAT},
        {"EFCR", 6, CS_W_ALL,  false, true, true, false, IF_REG_REG, OP_COMPARE_EXFLOAT, AM_REG_DIRECT_R, OPERAND_EXFLOAT},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"ILLEGAL", 0, 0x00,  false, false, false, false},
        {"NOP_BPT", 1, CS_RW_NN,  false, true, true, false, IF_SPECIAL, OP_EXTENSION, AM_SPECIAL_S, OPERAND_INT16} /* this is a special pseudo-opcode for JIT, it is not an actual opcode in the instruction set */
        
};

int scan_instructions_from_address(struct cpu_context* cpu_ctx, uint16_t addr) {
    while (true)
    {
        uint phys_addr =  get_phys_address (&cpu_ctx->state, CODE, cpu_ctx->state.reg.sw & 0xF, addr);
        ushort opcode;
        bool ok = peek(&cpu_ctx->state, phys_addr, &opcode);
        if (!ok) {
            printf("Failed to peek at address 0x%04X\n", phys_addr);
            break;
        }
        uint8_t high_6_bits = opcode >> 10;
        uint8_t high_8_bits = opcode >> 8;
        OpcodeDef *def = NULL;
        if (high_6_bits <= 16) /* use the 6 bit table */
        {
            if (high_6_bits == 16) {
                /* special case for BRX */
                uint8_t low_4_bits = opcode & 0xF;
                def = &opcode_defs_brx[low_4_bits];
            }
            else
            {
                def = &opcode_defs_6bit[high_6_bits];
            }
        }
        else
        {
            if (high_8_bits == 0x4A) {
                /* special case for IMM */
                uint8_t low_4_bits = opcode & 0xF;
                def = &opcode_defs_imm[low_4_bits];
            }
            else
            {
                def = &opcode_defs_8bit[high_8_bits - 0x44];
            }
        }
        if (!def->valid)
        {
            printf("Invalid opcode 0x%04X at address 0x%04X\n", opcode, phys_addr);
            break;
        }
        uint phys_addr_imm =  get_phys_address (&cpu_ctx->state, CODE, cpu_ctx->state.reg.sw & 0xF, addr + 1);
        ushort imm_value;
        ok = peek(&cpu_ctx->state, phys_addr_imm, &imm_value);
        if (!ok) {
            printf("Failed to peek at address 0x%04X\n", phys_addr_imm);
            break;
        }
        if (opcode == 0 && imm_value == 0) {
            printf("Reached end of code at address 0x%04X\n", phys_addr);
            break;
        }
        if (def->jitable) {
            printf("Found jitable opcode %s at address 0x%08X\n", def->name, phys_addr);
        }
        else {
            printf("Found non-jitable opcode %s at address 0x%08X\n", def->name, phys_addr);
            //break;
        }
        if (high_8_bits == 0x74) /* BR might be the last instruction, because it is unconditional */
        {
            break; /* meanwhile */
        }
        if (high_8_bits == 0x7F) /* URS is the last instruction */
        {
            break; /* meanwhile */
        }
        if (def->is_imm)
        {
            addr += 2;
            printf("Instruction has immediate value 0x%04X\n", imm_value);
        }
        else
        {
            addr ++;
            printf("Instruction has no immediate value\n");
        }
        
    }

    return 0; /* cannot be accessed with short offset */
}