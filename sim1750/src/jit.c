#include "cpu_ctx.h"
#include "peekpoke.h"
#include "smemacc.h"
#include <stdio.h>
#include "generated_jit.h"

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
                uint8_t sub_opcode = (opcode >> 4) & 0xF;
                def = &opcode_defs_brx[sub_opcode];
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
typedef void (*jit_func_t)(struct cpu_context*);

jit_func_t compile_basic_block(struct cpu_context* cpu_ctx, uint16_t start_addr) {
    struct sljit_compiler *compiler = sljit_create_compiler(NULL);
    if (!compiler) return NULL;

    sljit_emit_enter(compiler, 0, SLJIT_ARGS1V(W), 1, 1, 0);

    uint16_t addr = start_addr;
    while (true) {
        uint phys_addr = get_phys_address(&cpu_ctx->state, CODE, cpu_ctx->state.reg.sw & 0xF, addr);
        ushort opcode;
        if (!peek(&cpu_ctx->state, phys_addr, &opcode)) break;

        uint8_t high_6_bits = opcode >> 10;
        uint8_t high_8_bits = opcode >> 8;
        OpcodeDef *def = NULL;

        if (high_6_bits <= 16) {
            if (high_6_bits == 16) def = &opcode_defs_brx[opcode & 0xF]; // BRX has rx mapped to lower 4 bits in opcode_defs setup
            else def = &opcode_defs_6bit[high_6_bits];
        } else {
            if (high_8_bits == 0x4A) def = &opcode_defs_imm[opcode & 0xF];
            else def = &opcode_defs_8bit[high_8_bits - 0x44];
        }

        if (!def->valid || !def->jitable) break;

        ushort imm_value = 0;
        if (def->is_imm) {
            uint phys_addr_imm = get_phys_address(&cpu_ctx->state, CODE, cpu_ctx->state.reg.sw & 0xF, addr + 1);
            if (!peek(&cpu_ctx->state, phys_addr_imm, &imm_value)) break;
            addr += 2;
        } else {
            addr++;
        }

        bool emit_flags = true; // Placeholder for liveness
        compile_jit_instruction(compiler, def, opcode, imm_value, emit_flags);

        if (high_8_bits == 0x74 || high_8_bits == 0x7F) break;
    }

    sljit_emit_return_void(compiler);

    void *code = sljit_generate_code(compiler, 0, NULL);
    sljit_free_compiler(compiler);

    return (jit_func_t)code;
}
