#include "jit.h"
#include "cpu_ctx.h"
#include "peekpoke.h"
#include "smemacc.h"
#include <stdio.h>

extern OpcodeDef opcode_defs_brx[16];
extern OpcodeDef opcode_defs_imm[16];
extern OpcodeDef opcode_defs_6bit[17];
extern OpcodeDef opcode_defs_8bit[188];

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