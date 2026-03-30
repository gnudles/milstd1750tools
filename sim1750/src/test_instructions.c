#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "targsys.h"
/* --- Stubs to link against the generated code --- */
#include "cpu_ctx.h"
#include "cpu_helpers.h"

/* Stubs */
bool peek(struct cpu_state *state, uint phys_addr, ushort *word) {
    *word = read_phys_memory(state, phys_addr & 0xFFFFF);
    return true;
}

bool poke(struct cpu_state *state, uint phys_addr, ushort value) {
    write_phys_memory(state, phys_addr & 0xFFFFF, value);
    return true;
}

uint get_phys_address(struct cpu_state *state, int space, int as, uint16_t addr) {
    return addr; /* Flat memory mapping for tests */
}

/* Include the generated functions (or compile together) */


#include "generated_cpu.h"


void test_BEX();
void test_BPT();
void test_XIO();
void test_VIO();
void test_Memory_Access();
void test_Memory_Cache();
void test_MOV();
void test_Arithmetic();
void test_BitLogic();
void test_Stack();
void test_ControlFlow();

/* --- The Test Harness --- */
struct cpu_context ctx;

void reset_cpu() {
    memset(&ctx, 0, sizeof(struct cpu_context));
      int as, i = 0;

  /* initialize pagereg.ppa to "quasi-non-MMU". */
  for (as = 0; as <= 15; as++)
    {
      int logaddr_hinibble = 0;
      for (; logaddr_hinibble <= 0xF; logaddr_hinibble++)
        {
	      ctx.state.pagereg[CODE][as][logaddr_hinibble].ppa = i;
	      ctx.state.pagereg[DATA][as][logaddr_hinibble].ppa = i++;
        }
    }
    for (int p = 0; p < 256; p++) {
        if (ctx.state.mem[p]) {
            free(ctx.state.mem[p]);
            ctx.state.mem[p] = NULL;
        }
    }
    ctx.state.data_read_cache.valid = 0;
    ctx.state.data_read_cache_intr.valid = 0;
    ctx.state.code_read_cache.valid = 0;
    ctx.state.data_write_cache.valid = 0;
    ctx.state.reg.sys = SYS_TA | SYS_TB;
    ctx.state.num_phys_mem_pages = 16;
    ctx.state.halt = NO_HALT;
    ctx.state.next_scheduled_timer_calc_cycles = 0;
}


void my_load_ldm(struct cpu_context *cpu, const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) {
        printf("Could not open %s\n", fname);
        exit(1);
    }
    char line[256];
    while(fgets(line, sizeof(line), f)) {
        if (line[0] == '/') {
            if (line[1] == 'M') {
                // The actual structure of the LDM address string is `00100`? No!
                // `/M00100FE97C85000000...`
                // `00100`: first digit `0` is Address State (AS), then `0100` is the actual address.
                // length is `F`, checksum is `E97C`, data starts at 8500
                char addr_str_fix[5] = {line[3], line[4], line[5], line[6], 0};
                uint16_t addr = strtoul(addr_str_fix, NULL, 16);

                char len_str[2] = {line[7], 0};
                int len = strtoul(len_str, NULL, 16);
                if (len == 0 && line[7] == '0') len = 1; // 0 means 1 word
                else if (len == 0 && line[7] != '0') len = 0; // fallback just in case
                else len += 1; // 1-15 hex means 2-16 words

                char *p = line + 12; // skip checksum line[8..11]
                for(int i=0; i<len; i++) {
                    if (*p == 0 || *p == '\n' || *p == '\r') break;
                    char data_str[5] = {p[0], p[1], p[2], p[3], 0};
                    uint16_t data = strtoul(data_str, NULL, 16);
                    poke(&ctx.state, addr & 0xFFFF, data);
                    addr++;
                    p += 4;
                }
            } else if (line[1] == 'T') {
                 char addr_str[6] = {line[2], line[3], line[4], line[5], line[6], 0};
                 cpu->state.reg.ic = strtoul(addr_str+1, NULL, 16);
            }
        }
    }
    fclose(f);
}

void run_ldm_test(const char *fname) {
    reset_cpu();

    my_load_ldm(&ctx, fname);

    printf("Executing %s starting at 0x%04X... ", fname, ctx.state.reg.ic);

    // Let the main loop handle interrupts via process_interrupt().
    // We just need to give it enough cycles.
    // Important: we must request more cycles than `next_scheduled_timer_calc_cycles`
    // to allow the internal loop to calculate and evaluate timers.
    while (ctx.state.halt == NO_HALT)
    {
        cpu_mainloop(&ctx, ctx.state.total_cycles + 1000);
    }

    uint16_t status = read_phys_memory(&ctx.state, 0x2000);
    if (status == 0xAAAA) {
        printf("PASSED\n");
    } else {
        fflush(stdout);
        fprintf(stderr, "FAILED! (status=0x%04X)\n", status);
        fprintf(stderr, "IC: 0x%04X\n", ctx.state.reg.ic);
        fprintf(stderr, "SW: 0x%04X\n", ctx.state.reg.sw);
        fprintf(stderr, "PIR: 0x%04X\n", ctx.state.reg.pir);

        assert(false);
    }
}


void test_LB_Base_Relative() {
    reset_cpu();
    printf("Testing LB (Load Base Relative)... ");
    
    /* Setup: Base Register R12 = 0x1000 */
    ctx.state.reg.r[13] = 0x1000;
    
    /* Setup: Memory at 0x1005 holds -5 (0xFFFB) */
    poke(&ctx.state, 0x1005, 0xFFFB);
    
    /* Opcode: LB R13, 5 -> BR=13 (0x1 in field), Disp=0x05 */
    /* LB opcode is 0x00,  Disp=0x05 -> 0x0025 */
    uint16_t opcode = 0x0105; 
    
    interpret_LB(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    
    /* Verifications */
    assert(ctx.state.reg.r[2] == (int16_t)0xFFFB); // Loaded correctly
    assert(ctx.state.reg.ic == 1);                 // IC incremented
    assert(ctx.state.total_cycles == CLK_CYC_LB);           // Cycles added
    
    /* Flag Verification: -5 should set the Negative (N) flag */
    assert(ctx.state.reg.sw & 0x1000); // CS_N_BIT
    assert(!(ctx.state.reg.sw & 0x2000)); // CS_Z_BIT should be 0
    printf("PASSED\n");
}

void test_SLL_Logical_Left() {
    reset_cpu();
    printf("Testing SLL (Shift Left Logical)... ");
    
    /* Setup: R5 holds 0x0F00 */
    ctx.state.reg.r[5] = 0x0F00;
    
    /* Opcode: SLL R5, 4 (Shift left by 4) */
    /* SLL opcode is 0x60. Shift is (count-1) in upper nibble. 
       Count=4 -> encoded as 3 -> 0x0030. RB=5 -> 0x0005. 
       Total: 0x6035 */
    uint16_t opcode = 0x6035;
    
    interpret_SLL(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    
    /* Verification: 0x0F00 << 4 = 0xF000 */
    assert(ctx.state.reg.r[5] == (int16_t)0xF000); 
    
    /* Flag Verification: 0xF000 is negative */
    assert(ctx.state.reg.sw & 0x1000); // CS_N_BIT
    printf("PASSED\n");
}

void test_SQRT();
void test_ESQR();
void test_Single_Shifts();
void test_Double_Shifts();
void test_FNEG_FABS();
void test_EFA_EFS();
void test_FA_FS();

void test_STUB_Upper_Byte() {
    reset_cpu();
    printf("Testing STUB (Store Upper Byte)... ");
    
    /* Setup: R2 holds 0xAABB */
    ctx.state.reg.r[2] = 0xAABB;
    
    /* Setup: Memory at 0x2000 holds 0x1122 */
    poke(&ctx.state, 0x2000, 0x1122);
    
    /* Opcode: STUB R2, 0x2000 -> RA=2, RX=0. IMM=0x2000 */
    /* STUB opcode is 0x9B. RA=2 -> 0x9B20. */
    uint16_t opcode = 0x9B20;
    ctx.state.reg.ic = 0x0000;
    poke(&ctx.state, 0x0001, 0x2000); // Immediate address
    
    interpret_STUB(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    
    /* Verification: Memory should be updated to (Lower byte of R2 in Upper byte of Memory)
       Lower byte of R2 is 0xBB. Memory upper byte becomes 0xBB. Lower byte preserved (0x22).
       Result: 0xBB22 */
    assert(read_phys_memory(&ctx.state, 0x2000) == 0xBB22);
    printf("PASSED\n");
}



/* * TEST 1: 32-bit Floating Point Divide (FD)
 * Math: 1.0 / 2.0 = 0.5
 */
void test_FD_Basic_Division() {
    reset_cpu();
    printf("Testing FD (1.0 / 2.0 = 0.5)... ");
    
    /* Setup Operand A in R2, R3 (1.0) 
     * 1.0 = 0.5 * 2^1. 
     * Mantissa: 0x400000. Exponent: 0x01. W1=0x4000, W2=0x0001 */
    ctx.state.reg.r[2] = 0x4000;
    ctx.state.reg.r[3] = 0x0001;
    
    /* Setup Operand B in Memory at 0x1000 (2.0)
     * 2.0 = 0.5 * 2^2. 
     * Mantissa: 0x400000. Exponent: 0x02. W1=0x4000, W2=0x0002 */
    poke(&ctx.state, 0x1000, 0x4000);
    poke(&ctx.state, 0x1001, 0x0002);
    
    /* Opcode: FD R2, 0x1000 -> Opcode 0xD8. RA=2. RX=0. */
    uint16_t opcode = 0xD820; 
    poke(&ctx.state, 0x0001, 0x1000); /* Immediate Address */
    
    interpret_FD(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    
    /* Verification: Result should be 0.5 (0.5 * 2^0)
     * Mantissa: 0x400000. Exponent: 0x00. W1=0x4000, W2=0x0000 */
    assert(ctx.state.reg.r[2] == 0x4000);
    assert(ctx.state.reg.r[3] == 0x0000);
    
    /* Flag Verification: Result is strictly positive */
    assert(ctx.state.reg.sw & 0x4000); /* CS_P_BIT */
    assert(!(ctx.state.reg.sw & 0x1000)); /* Not Negative */
    printf("PASSED\n");
}

/* * TEST 2: 32-bit Floating Point Divide (FD) with Normalization Shift
 * Math: -3.0 / 1.5 = -2.0
 * This tests the XOR CLZ trick and sign extension.
 */
void test_FD_Negative_Normalization() {
    reset_cpu();
    printf("Testing FD (-3.0 / 1.5 = -2.0)... ");
    
    /* Setup Operand A in R4, R5 (-3.0) 
     * 3.0 = 0.75 * 2^2 -> Mantissa 0x600000. 
     * -3.0 -> 2's comp of 0x600000 is 0xA00000. Exp: 0x02. */
    ctx.state.reg.r[4] = (int16_t)0xA000;
    ctx.state.reg.r[5] = 0x0002;
    
    /* Setup Operand B in Memory at 0x2000 (1.5)
     * 1.5 = 0.75 * 2^1. Mantissa: 0x600000. Exp: 0x01. */
    poke(&ctx.state, 0x2000, 0x6000);
    poke(&ctx.state, 0x2001, 0x0001);
    
    /* Opcode: FD R4, 0x2000 -> Opcode 0xD8. RA=4. */
    uint16_t opcode = 0xD840; 
    poke(&ctx.state, 0x0001, 0x2000);
    
    interpret_FD(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    
    /* Verification: Result should be -2.0 (-1.0 * 2^1)
     * -1.0 is exactly 0x800000 in 2's comp fractions. Exp: 0x01. */
    assert(ctx.state.reg.r[4] == (int16_t)0x8000);
    assert(ctx.state.reg.r[5] == 0x0001);
    
    /* Flag Verification: Result is strictly negative */
    assert(ctx.state.reg.sw & 0x1000); /* CS_N_BIT */
    printf("PASSED\n");
}

/* * TEST 3: 48-bit Extended Floating Point Divide (EFD)
 * Math: 5.0 / 4.0 = 1.25
 * This verifies the __int128 39-bit fractional math shift.
 */
void test_EFD_48bit_Math() {
    reset_cpu();
    printf("Testing EFD (5.0 / 4.0 = 1.25) [48-bit]... ");
    
    /* Setup Operand A in R6, R7, R8 (5.0) 
     * 5.0 = 0.625 * 2^3. Mantissa (0.101 binary) = 0x50 0000 0000. Exp: 0x03 */
    ctx.state.reg.r[6] = 0x5000;
    ctx.state.reg.r[7] = 0x0003; /* Lower byte is exponent */
    ctx.state.reg.r[8] = 0x0000;
    
    /* Setup Operand B in Memory at 0x3000 (4.0)
     * 4.0 = 0.5 * 2^3. Mantissa (0.100 binary) = 0x40 0000 0000. Exp: 0x03 */
    poke(&ctx.state, 0x3000, 0x4000);
    poke(&ctx.state, 0x3001, 0x0003);
    poke(&ctx.state, 0x3002, 0x0000);
    
    /* Opcode: EFD R6, 0x3000 -> Opcode 0xDA. RA=6. */
    uint16_t opcode = 0xDA60; 
    poke(&ctx.state, 0x0001, 0x3000);
    
    interpret_EFD(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    
    /* Verification: Result should be 1.25 (0.625 * 2^1)
     * 1.25 Mantissa = 0x50 0000 0000. Exp: 0x01 */
    assert(ctx.state.reg.r[6] == 0x5000);
    assert(ctx.state.reg.r[7] == 0x0001); /* Notice the exponent is exactly 1 */
    assert(ctx.state.reg.r[8] == 0x0000);
    printf("PASSED\n");
}

/* * TEST 4: Divide By Zero Hardware Trap
 */
void test_FD_Divide_By_Zero() {
    reset_cpu();
    printf("Testing FD (Divide by Zero Trap)... ");
    
    /* Operand A: 1.0 */
    ctx.state.reg.r[2] = 0x4000;
    ctx.state.reg.r[3] = 0x0001;
    
    /* Operand B: 0.0 (Mantissa is 0) */
    poke(&ctx.state, 0x1000, 0x0000);
    poke(&ctx.state, 0x1001, 0x0000);
    
    uint16_t opcode = 0xD820; 
    poke(&ctx.state, 0x0001, 0x1000);
    
    interpret_FD(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    
    /* Verification: Ensure the PIR (Pending Interrupt Register) caught the overflow */
    assert(ctx.state.reg.pir & INTR_FLTOFL); 
    printf("PASSED\n");
}

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>

/* ========================================================================= */
/* IEEE DOUBLE TO 1750A EXTENDED (48-BIT)                  */
/* ========================================================================= */
void double_to_1750a_efloat(double val, uint16_t *w1, uint16_t *w2, uint16_t *w3) {
    if (val == 0.0) {
        *w1 = 0; *w2 = 0; *w3 = 0;
        return;
    }

    int exp;
    /* frexp breaks the double into a fraction [0.5, 1.0) and a base-2 exponent */
    double frac = frexp(val, &exp);

    /* 1750A requires the top two bits of the mantissa to be mutually exclusive.
     * frexp returns (-1.0, -0.5]. The ONLY value that violates the 1750A mutual
     * exclusion rule is exactly -0.5 (which encodes as 1100... in 2's comp).
     * To fix this, we shift the fraction to -1.0 and decrement the exponent. */
    if (frac == -0.5) {
        frac = -1.0;
        exp -= 1;
    }

    /* 1750A Exponent boundary checks (-128 to 127) */
    if (exp > 127) {
        if (val > 0) {
            *w1 = 0x7FFF; *w2 = 0xFF7F; *w3 = 0xFFFF; /* Max Positive */
        } else {
            *w1 = 0x8000; *w2 = 0x007F; *w3 = 0x0000; /* Max Negative */
        }
        return;
    } else if (exp < -128) {
        *w1 = 0; *w2 = 0; *w3 = 0; /* Underflow to Zero */
        return;
    }

    /* Multiply the fraction by 2^39 to create a 40-bit integer mantissa.
     * Casting to int64_t implicitly truncates any remaining IEEE precision 
     * exactly like the 1750A hardware. */
    int64_t mantissa = (int64_t)ldexp(frac, 39);

    /* Pack the 40-bit mantissa and 8-bit exponent into three 16-bit words */
    *w1 = (uint16_t)((mantissa >> 24) & 0xFFFF);
    *w2 = (uint16_t)(((mantissa >> 8) & 0xFF00) | (exp & 0xFF));
    *w3 = (uint16_t)(mantissa & 0xFFFF);
}

/* ========================================================================= */
/* 1750A EXTENDED (48-BIT) TO IEEE DOUBLE                  */
/* ========================================================================= */
double efloat_1750a_to_double(uint16_t w1, uint16_t w2, uint16_t w3) {
    /* Reconstruct the 40-bit mantissa */
    int64_t mantissa = ((int64_t)w1 << 24) | ((int64_t)(w2 & 0xFF00) << 8) | w3;
    
    /* Sign-extend the 40-bit value to a full 64-bit integer */
    mantissa = (int64_t)((uint64_t)mantissa << 24) >> 24;

    if (mantissa == 0) return 0.0;

    /* Extract 8-bit 2's complement exponent */
    int8_t exp = (int8_t)(w2 & 0xFF);

    /* Since we treat the mantissa as a whole integer, it is scaled up by 2^39.
     * We reverse this by multiplying by 2^(exp - 39) using ldexp. 
     * The double format has 53 bits of precision, so the 40-bit 1750A 
     * mantissa fits completely without any rounding loss. */
    return ldexp((double)mantissa, exp - 39);
}

/* ========================================================================= */
/* BONUS: 32-BIT SINGLE PRECISION FLOAT CONVERSIONS               */
/* ========================================================================= */
void double_to_1750a_float(double val, uint16_t *w1, uint16_t *w2) {
    if (val == 0.0) { *w1 = 0; *w2 = 0; return; }
    int exp;
    double frac = frexp(val, &exp);
    if (frac == -0.5) { frac = -1.0; exp -= 1; }
    
    if (exp > 127) {
        if (val > 0) { *w1 = 0x7FFF; *w2 = 0xFF7F; } 
        else         { *w1 = 0x8000; *w2 = 0x007F; }
        return;
    } else if (exp < -128) {
        *w1 = 0; *w2 = 0; return;
    }

    int32_t mantissa = (int32_t)ldexp(frac, 23);
    *w1 = (uint16_t)((mantissa >> 8) & 0xFFFF);
    *w2 = (uint16_t)((((uint32_t)mantissa << 8) & 0xFF00) | (exp & 0xFF));
}

double float_1750a_to_double(uint16_t w1, uint16_t w2) {
    int32_t mantissa = ((int32_t)w1 << 8) | ((w2 >> 8) & 0xFF);
    mantissa = (int32_t)((uint32_t)mantissa << 8) >> 8; /* Sign extend from 24-bits */
    if (mantissa == 0) return 0.0;
    int8_t exp = (int8_t)(w2 & 0xFF);
    return ldexp((double)mantissa, exp - 23);
}

void test_FDR() {
    reset_cpu();
    printf("Testing FDR ... ");
    

    double epsilon = 0.0000005;
    double a;
    double b;
    for (int i = 0 ; i < 0x4000 ; i+=64)
    {

        for (int j = 0 ; j < 0x4000 ; j+=64)
        {
            ctx.state.reg.r[2] = 0x4000 + i; /* 0.5 */
            ctx.state.reg.r[3] = 0x0000;
            
            
            ctx.state.reg.r[4] = 0x8000 + j; /* -1 */
            ctx.state.reg.r[5] = 0x0000;
            a = (int)ctx.state.reg.r[2] / 32768.0;
            b = (int)ctx.state.reg.r[4] / 32768.0;
            uint16_t opcode = 0xD924;
            interpret_FDR(&ctx, opcode, 0);
            //printf ( " a/b = %lf/%lf = %lf  ?= %lf \n",a,b, a/b, float_1750a_to_double(ctx.state.reg.r[2],ctx.state.reg.r[3]));
            assert(fabs(float_1750a_to_double(ctx.state.reg.r[2],ctx.state.reg.r[3]) - a/b) < epsilon);
            ctx.state.reg.r[2] = 0x4000 + i; /* 0.5 */
            ctx.state.reg.r[3] = 0x0000;
            opcode = 0xD942;
            interpret_FDR(&ctx, opcode, 0);
            assert(fabs(float_1750a_to_double(ctx.state.reg.r[4],ctx.state.reg.r[5]) - b/a) < epsilon);


        }
    }

    for (int i = 0 ; i < 0x4000 ; i+=64)
    {
        

        for (int j = 0 ; j < 0x4000 ; j+=64)
        {
            ctx.state.reg.r[2] = 0x4000 + i; /* 0.5 */
            ctx.state.reg.r[3] = 0x0000;
            
            ctx.state.reg.r[4] = 0x4000 + j; /* 0.5 */
            ctx.state.reg.r[5] = 0x0000;
            a = (int)ctx.state.reg.r[2] / 32768.0;
            b = (int)ctx.state.reg.r[4] / 32768.0;
            uint16_t opcode = 0xD924;
            interpret_FDR(&ctx, opcode, 0);
            //printf ( " a/b = %lf/%lf = %lf  ?= %lf \n",a,b, a/b, float_1750a_to_double(ctx.state.reg.r[2],ctx.state.reg.r[3]));
            assert(fabs(float_1750a_to_double(ctx.state.reg.r[2],ctx.state.reg.r[3]) - a/b) < epsilon);
            b += 0.5/0x4000;

        }
        a+= 0.5/0x4000;
    }
    a = -1.0;
    for (int i = 0 ; i < 0x4000 ; i+=64)
    {
        
        b = -1.0;
        for (int j = 0 ; j < 0x4000 ; j+=64)
        {
            ctx.state.reg.r[2] = 0x8000 + i; /* -1.0 */
            ctx.state.reg.r[3] = 0x0000;
            
            ctx.state.reg.r[4] = 0x8000 + j; /* -1.0 */
            ctx.state.reg.r[5] = 0x0000;
            a = (int)ctx.state.reg.r[2] / 32768.0;
            b = (int)ctx.state.reg.r[4] / 32768.0;
            uint16_t opcode = 0xD924;
            interpret_FDR(&ctx, opcode, 0);
            //printf ( " a/b = %lf/%lf = %lf  ?= %lf \n",a,b, a/b, float_1750a_to_double(ctx.state.reg.r[2],ctx.state.reg.r[3]));
            assert(fabs(float_1750a_to_double(ctx.state.reg.r[2],ctx.state.reg.r[3]) - a/b) < epsilon);
            b += 0.5/0x4000;

        }
        a+= 0.5/0x4000;
    }
    printf("PASSED\n");
}

void test_EFDR() {
    reset_cpu();
    printf("Testing EFDR ... ");
    

    double epsilon = 0.000000000005;
    double a;
    double b;
    for (int i = 0 ; i < 0x4000 ; i+=64)
    {

        for (int j = 0 ; j < 0x4000 ; j+=64)
        {
            ctx.state.reg.r[2] = 0x4000 + i; /* 0.5 */
            ctx.state.reg.r[3] = 0x0000;
            ctx.state.reg.r[4] = 0x0000;
            
            
            ctx.state.reg.r[5] = 0x8000 + j; /* -1 */
            ctx.state.reg.r[6] = 0x0000;
            ctx.state.reg.r[7] = 0x0000;
            a = (int)ctx.state.reg.r[2] / 32768.0;
            b = (int)ctx.state.reg.r[5] / 32768.0;
            uint16_t opcode = 0xDB25;
            interpret_EFDR(&ctx, opcode, 0);
            //printf ( " a/b = %lf/%lf = %lf  ?= %lf \n",a,b, a/b, float_1750a_to_double(ctx.state.reg.r[2],ctx.state.reg.r[3]));
            assert(fabs(efloat_1750a_to_double(ctx.state.reg.r[2],ctx.state.reg.r[3],ctx.state.reg.r[4]) - a/b) < epsilon);
            ctx.state.reg.r[2] = 0x4000 + i; /* 0.5 */
            ctx.state.reg.r[3] = 0x0000;
            ctx.state.reg.r[4] = 0x0000;
            opcode = 0xDB52;
            interpret_EFDR(&ctx, opcode, 0);
            assert(fabs(efloat_1750a_to_double(ctx.state.reg.r[5],ctx.state.reg.r[6],ctx.state.reg.r[7]) - b/a) < epsilon);


        }
    }

    printf("PASSED\n");
}

void test_Extended_Float_Pi_Pipeline() {
    reset_cpu();
    printf("Testing Extended Float Pipeline (Pi Approx)...\n");

    /* --- 1. CONVERT 355 to EFLT (Numerator) --- */
    ctx.state.reg.r[0] = 0x0000;
    ctx.state.reg.r[1] = 355;
    /* OP_INT32_TO_EFLT RA=0, RB=0. Target is R[RA], Source is R[RB] */
    ctx.state.reg.ic = 0;
    interpret_EFLT(&ctx, 0x0000, read_phys_memory(&ctx.state, 0x0001));
    /* R0, R1, R2 now hold exactly 355.0 */

    /* --- 2. CONVERT 113 to EFLT (Denominator) --- */
    ctx.state.reg.r[4] = 0x0000;
    ctx.state.reg.r[5] = 113;
    /* OP_INT32_TO_EFLT RA=4, RB=4 */
    ctx.state.reg.ic = 0;
    interpret_EFLT(&ctx, 0x0044, read_phys_memory(&ctx.state, 0x0001));
    /* R4, R5, R6 now hold exactly 113.0 */

    /* --- 3. DIVIDE (Pi Approx: 355.0 / 113.0) --- */
    /* Push Denominator to Memory so we can use EFD (Divide by Memory) */
    poke(&ctx.state, 0x1000, ctx.state.reg.r[4]);
    poke(&ctx.state, 0x1001, ctx.state.reg.r[5]);
    poke(&ctx.state, 0x1002, ctx.state.reg.r[6]);
    poke(&ctx.state, 0x0001, 0x1000); /* Instruction fetcher DO address */
    /* OP_DIV_EXFLOAT RA=0. R0 = R0 / Mem */
    ctx.state.reg.ic = 0;
    interpret_EFD(&ctx, 0x0000, read_phys_memory(&ctx.state, 0x0001));
    /* R0, R1, R2 now hold 3.1415929... */

    /* --- 4. ADD 1000.0 --- */
    ctx.state.reg.r[4] = 0x0000;
    ctx.state.reg.r[5] = 1000;
    ctx.state.reg.ic = 0;
    interpret_EFLT(&ctx, 0x0044, read_phys_memory(&ctx.state, 0x0001));
    poke(&ctx.state, 0x2000, ctx.state.reg.r[4]);
    poke(&ctx.state, 0x2001, ctx.state.reg.r[5]);
    poke(&ctx.state, 0x2002, ctx.state.reg.r[6]);
    poke(&ctx.state, 0x0001, 0x2000);
    ctx.state.reg.ic = 0;
    /* OP_ADD_EXFLOAT RA=0. R0 = R0 + Mem */
    interpret_EFA(&ctx, 0x0000, read_phys_memory(&ctx.state, 0x0001));
    /* R0, R1, R2 now hold 1003.14159... */

    /* --- 5. SUBTRACT 500.0 --- */
    ctx.state.reg.r[4] = 0x0000;
    ctx.state.reg.r[5] = 500;
    ctx.state.reg.ic = 0;
    interpret_EFLT(&ctx, 0x0044, read_phys_memory(&ctx.state, 0x0001));
    poke(&ctx.state, 0x3000, ctx.state.reg.r[4]);
    poke(&ctx.state, 0x3001, ctx.state.reg.r[5]);
    poke(&ctx.state, 0x3002, ctx.state.reg.r[6]);
    poke(&ctx.state, 0x0001, 0x3000);
    ctx.state.reg.ic = 0;
    /* OP_SUB_EXFLOAT RA=0. R0 = R0 - Mem */
    interpret_EFS(&ctx, 0x0000, read_phys_memory(&ctx.state, 0x0001));
    /* R0, R1, R2 now hold 503.14159... */

    /* --- 6. MULTIPLY BY 2.0 --- */
    ctx.state.reg.r[4] = 0x0000;
    ctx.state.reg.r[5] = 2;
    ctx.state.reg.ic = 0;
    interpret_EFLT(&ctx, 0x0044, read_phys_memory(&ctx.state, 0x0001));
    poke(&ctx.state, 0x4000, ctx.state.reg.r[4]);
    poke(&ctx.state, 0x4001, ctx.state.reg.r[5]);
    poke(&ctx.state, 0x4002, ctx.state.reg.r[6]);
    poke(&ctx.state, 0x0001, 0x4000);
    ctx.state.reg.ic = 0;
    /* OP_MULT_EXFLOAT RA=0. R0 = R0 * Mem */
    interpret_EFM(&ctx, 0x0000, read_phys_memory(&ctx.state, 0x0001));
    /* R0, R1, R2 now hold 1006.283185... */

    /* --- VERIFICATION 1: Double Precision Threshold Check --- */
    double final_val = efloat_1750a_to_double(ctx.state.reg.r[0], ctx.state.reg.r[1], ctx.state.reg.r[2]);
    double expected = ((355.0 / 113.0) + 1000.0 - 500.0) * 2.0;
    
    /* Assert we are accurate to 5 decimal places against the host FPU */
    printf("(Float: %lf, expected: %lf)\n", final_val, expected);
    assert(fabs(final_val - expected) < 0.00001);

    /* --- 7. CONVERT BACK TO INT32 --- */
    /* OP_EFLT_TO_INT32 RA=4, RB=0. R4, R5 gets Int32 of Float R0 */
    ctx.state.reg.ic = 0;
    interpret_EFIX(&ctx, 0x0040, read_phys_memory(&ctx.state, 0x0001));
    
    /* --- VERIFICATION 2: Integer Truncation Check --- */
    int32_t final_int = ((int32_t)ctx.state.reg.r[4] << 16) | (uint16_t)ctx.state.reg.r[5];
    
    /* 1750A architecture dictates truncation toward zero. 1006.28 MUST become 1006 */
    assert(final_int == 1006);

    printf("PASSED (Final Float: %f, Final Int: %d)\n", final_val, final_int);
}

/* ========================================================================= */
/* UNIT TESTS                                 */
/* ========================================================================= */
int main() {
    uint16_t w1, w2, w3;

    printf("Testing 48-bit Extended Precision Conversions...\n");

    /* Test 1: Exactly 1.0 */
    double_to_1750a_efloat(1.0, &w1, &w2, &w3);
    assert(w1 == 0x4000 && w2 == 0x0001 && w3 == 0x0000);
    assert(efloat_1750a_to_double(w1, w2, w3) == 1.0);

    /* Test 2: Exactly -1.0 (The Mutual Exclusion Trap!) */
    double_to_1750a_efloat(-1.0, &w1, &w2, &w3);
    assert(w1 == 0x8000 && w2 == 0x0000 && w3 == 0x0000);
    assert(efloat_1750a_to_double(w1, w2, w3) == -1.0);
    

    /* Test 3: Fractional Value (5.0) -> 0.625 * 2^3 */
    double_to_1750a_efloat(5.0, &w1, &w2, &w3);
    assert(w1 == 0x5000 && w2 == 0x0003 && w3 == 0x0000);
    assert(efloat_1750a_to_double(w1, w2, w3) == 5.0);

    /* Test 4: Precision checking (A number requiring W3) */
    /* 1.0 + 2^-38 is a number that pushes a 1 into the lowest bit of W3 */
    double precision_val = 1.0 + pow(2.0, -38);
    double_to_1750a_efloat(precision_val, &w1, &w2, &w3);
    assert(w3 == 0x0001); 
    assert(efloat_1750a_to_double(w1, w2, w3) == precision_val);

    /* Test 5: Exactly -0.5  */
    double_to_1750a_efloat(-0.5, &w1, &w2, &w3);
    assert(efloat_1750a_to_double(w1, w2, w3) == -0.5);

    /* Test 6: Exactly -0.625  */
    double_to_1750a_efloat(-0.625, &w1, &w2, &w3);
    assert(efloat_1750a_to_double(w1, w2, w3) == -0.625);

    printf("All Extended Float tests passed!\n");

    printf("Testing 32-bit Single Precision Conversions...\n");
    double_to_1750a_float(1.5, &w1, &w2);
    assert(w1 == 0x6000 && w2 == 0x0001);
    assert(float_1750a_to_double(w1, w2) == 1.5);
    
    test_Extended_Float_Pi_Pipeline();
    printf("All Single Float tests passed!\n");
    test_LB_Base_Relative();
    test_SLL_Logical_Left();
    test_Single_Shifts();
    test_Double_Shifts();
    test_STUB_Upper_Byte();
    printf("All instruction tests passed.\n");
    test_FDR();
    test_EFDR();
    test_FD_Basic_Division();
    
    test_FD_Negative_Normalization();
    test_EFD_48bit_Math();
    test_FD_Divide_By_Zero();
    test_SQRT();
    test_ESQR();
    test_FNEG_FABS();
    test_EFA_EFS();
    test_FA_FS();
    printf("All floating point tests passed.\n");

    test_BEX();
    test_BPT();
    test_XIO();
    test_VIO();
    test_Memory_Access();
    test_Memory_Cache();
    test_MOV();
    test_Arithmetic();
    test_BitLogic();
    test_Stack();
    test_ControlFlow();
    printf("All additional instruction tests passed.\n");

    // We are inside sim1750 directory
    run_ldm_test("automated_tests/interrupt_test.ldm");
    run_ldm_test("automated_tests/simple_math.ldm");
    run_ldm_test("automated_tests/simple_logic.ldm");
    run_ldm_test("automated_tests/cmp_test.ldm");
    run_ldm_test("automated_tests/e_calc.ldm");
    run_ldm_test("automated_tests/pi_calc.ldm");
    run_ldm_test("automated_tests/timer_test.ldm");

    return 0;
}

void test_BEX() {
    reset_cpu();
    printf("Testing BEX (Branch Executive)... ");

    /* Opcode: BEX 0 -> 0x7700 */
    uint16_t opcode = 0x7700;

    interpret_BEX(&ctx, opcode, 0);

    /* BEX triggers INTR_BEX flag in pir */
    assert(ctx.state.reg.pir & INTR_BEX);
    assert(ctx.state.bex_index == 0);
    printf("PASSED\n");
}

void test_BPT() {
    reset_cpu();
    printf("Testing BPT (Breakpoint)... ");

    /* Opcode: BPT -> 0xFFFF */
    uint16_t opcode = 0xFFFF;

    interpret_NOP_BPT(&ctx, opcode, 0);

    /* Nothing explicitly verified for BPT here since its interpreter-level exit logic
       will be handled in the main execution loop/test runner later. */
    printf("PASSED\n");
}

void test_XIO() {
    reset_cpu();
    printf("Testing XIO... ");

    /* Test 1: XIO 0x2000 (SMK - Set Interrupt Mask) */
    ctx.state.reg.r[0] = 0xAAAA;
    uint16_t opcode = 0x4800; // XIO R0, 0x2000 -> RA=0, RX=0.

    poke(&ctx.state, 0x0001, 0x2000);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    assert(ctx.state.reg.mk == 0xAAAA);

    /* Test 2: XIO 0xA000 (RMK - Read Interrupt Mask) */
    ctx.state.reg.mk = 0x5555;
    ctx.state.reg.r[1] = 0x0000;
    opcode = 0x4810; // XIO R1, 0xA000 -> RA=1, RX=0.
    poke(&ctx.state, 0x0001, 0xA000);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    assert(ctx.state.reg.r[1] == 0x5555);

    /* Test 3: XIO 0x5000 (Write Memory Protect RAM) */
    ctx.state.reg.r[2] = 0x1234;
    opcode = 0x4820; // XIO R2, 0x500A -> RA=2, RX=0.
    poke(&ctx.state, 0x0001, 0x500A);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    assert(ctx.state.mem_protect[0][0x0A] == 0x1234);

    /* Test 4: XIO 0xD000 (Read Memory Protect RAM) */
    ctx.state.mem_protect[0][0x0B] = 0xABCD;
    ctx.state.reg.r[3] = 0x0000;
    opcode = 0x4830; // XIO R3, 0xD00B -> RA=3, RX=0.
    poke(&ctx.state, 0x0001, 0xD00B);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    assert((uint16_t)ctx.state.reg.r[3] == 0xABCD);

    /* Test 5: XIO 0x2001 (Clear Interrupt Request) */
    ctx.state.reg.pir = 0xFFFF;
    ctx.state.reg.ft = 0x1234;
    opcode = 0x4800; // XIO R0, 0x2001
    poke(&ctx.state, 0x0001, 0x2001);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    assert(ctx.state.reg.pir == 0);
    assert(ctx.state.reg.ft == 0);

    /* Test 6: XIO 0x2002 (Enable Interrupts) */
    ctx.state.reg.sys = 0;
    ctx.state.reg.sys_update = 0;
    opcode = 0x4800; // XIO R0, 0x2002
    poke(&ctx.state, 0x0001, 0x2002);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    assert(ctx.state.reg.sys_update & SYS_INT);

    /* Test 7: XIO 0x2003 (Disable Interrupts) */
    ctx.state.reg.sys = SYS_INT;
    opcode = 0x4800; // XIO R0, 0x2003
    poke(&ctx.state, 0x0001, 0x2003);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    assert(!(ctx.state.reg.sys & SYS_INT));

    /* Test 8: XIO 0x2004 (Reset Pending Interrupt) */
    ctx.state.reg.pir = 0xFFFF;
    ctx.state.reg.r[0] = 0x0005; // Reset interrupt 5
    opcode = 0x4800; // XIO R0, 0x2004
    poke(&ctx.state, 0x0001, 0x2004);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    assert((ctx.state.reg.pir & (0x8000 >> 5)) == 0);

    /* Test 9: XIO 0x2005 (Set Pending Interrupt) */
    ctx.state.reg.pir = 0x0000;
    ctx.state.reg.pir_update = 0x0000;
    ctx.state.reg.r[0] = 0x0400; // Set interrupt 5
    opcode = 0x4800; // XIO R0, 0x2005
    poke(&ctx.state, 0x0001, 0x2005);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    assert(ctx.state.reg.pir_update == 0x0400);

    /* Test 10: XIO 0x200E (Write Status Word) */
    ctx.state.reg.sw = 0x0000;
    ctx.state.reg.r[0] = 0x1234;
    opcode = 0x4800; // XIO R0, 0x200E
    poke(&ctx.state, 0x0001, 0x200E);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    assert(ctx.state.reg.sw == 0x1234);

    /* Test 11: XIO 0x4003 (Memory Protect Enable) */
    ctx.state.reg.sw = 0x0000; // Must be 0 to allow privileged instructions
    ctx.state.reg.sys = 0x0000;
    opcode = 0x4800; // XIO R0, 0x4003
    poke(&ctx.state, 0x0001, 0x4003);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    assert(ctx.state.reg.sys & SYS_MEM_PROT);

    /* Test 12: XIO 0xA004 (Read Pending Interrupt) */
    ctx.state.reg.pir = 0xDEAD;
    ctx.state.reg.r[1] = 0x0000;
    opcode = 0x4810; // XIO R1, 0xA004
    poke(&ctx.state, 0x0001, 0xA004);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    assert((uint16_t)ctx.state.reg.r[1] == 0xDEAD);

    /* Test 13: XIO 0x5100 (Write Instruction Page Register) */
    ctx.state.reg.r[0] = 0x1234;
    opcode = 0x4800; // XIO R0, 0x5123 -> group 2, page 3
    poke(&ctx.state, 0x0001, 0x5123);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    // bank CODE = 0, group = 2, page = 3
    assert(ctx.state.pagereg[CODE][2][3].word == 0x1234);

    /* Test 14: XIO 0xD100 (Read Instruction Page Register) */
    ctx.state.pagereg[CODE][5][15].word = 0x5678;
    ctx.state.reg.r[1] = 0x0000;
    opcode = 0x4810; // XIO R1, 0xD15F -> group 5, page 15
    poke(&ctx.state, 0x0001, 0xD15F);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    assert(ctx.state.reg.r[1] == 0x5678);

    /* Test 15: XIO 0x5200 (Write Operand Page Register) */
    ctx.state.reg.sw = 0x0000;
    ctx.state.reg.r[2] = 0x9ABC;
    opcode = 0x4820; // XIO R2, 0x5242 -> group 4, page 2
    poke(&ctx.state, 0x0001, 0x5242);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    // bank DATA = 1, group = 4, page = 2
    assert((uint16_t)ctx.state.pagereg[DATA][4][2].word == 0x9ABC);

    /* Test 16: XIO 0xD200 (Read Operand Page Register) */
    ctx.state.pagereg[DATA][10][1].word = 0xDEF0;
    ctx.state.reg.r[3] = 0x0000;
    opcode = 0x4830; // XIO R3, 0xD2A1 -> group A, page 1
    poke(&ctx.state, 0x0001, 0xD2A1);
    interpret_XIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));
    assert((uint16_t)ctx.state.reg.r[3] == 0xDEF0);

    printf("PASSED\n");
}

void test_VIO() {
    reset_cpu();
    printf("Testing VIO... ");

    /* Setup vector IO memory block */
    /* DO_ADDR points to vio_struct:
       word 0: io_cmd
       word 1: vector_select
       word 2+: data for selected bits
    */
    poke(&ctx.state, 0x2000, 0x5000); // Base IO cmd (Write Memory Protect)
    poke(&ctx.state, 0x2001, 0xA000); // Vector select: Bits 0 and 2 set (1010...)
    poke(&ctx.state, 0x2002, 0x1111); // Data for Bit 0
    poke(&ctx.state, 0x2003, 0x2222); // Data for Bit 2

    /* RA (cmd_inc) */
    ctx.state.reg.r[4] = 0x0001; // Increment command address by 1 per checked bit

    uint16_t opcode = 0x4940; // VIO R4, 0x2000 -> RA=4, RX=0
    poke(&ctx.state, 0x0001, 0x2000);

    interpret_VIO(&ctx, opcode, read_phys_memory(&ctx.state, 0x0001));

    /*
       Bit 0 was set:
       IO_cmd = 0x5000. Data = 0x1111 -> Write MP RAM at 0
       Bit 1 was NOT set:
       IO_cmd would be evaluated as 0x5001. No write.
       Bit 2 was set:
       IO_cmd = 0x5002. Data = 0x2222 -> Write MP RAM at 2
    */

    assert(ctx.state.mem_protect[0][0] == 0x1111);
    assert(ctx.state.mem_protect[0][1] == 0x0000); // Unchanged
    assert(ctx.state.mem_protect[0][2] == 0x2222);

    printf("PASSED\n");
}

void test_Memory_Access() {
    reset_cpu();
    printf("Testing Memory Access Functions... ");

    // Test store_data_word and fetch_data_word
    ctx.state.reg.sw = 0x0000;
    ctx.state.pagereg[DATA][0][1].ppa = 5; // Logical page 1 maps to Physical page 5
    bool ok = store_data_word(&ctx, 0x1050, 0x55AA);
    assert(ok);

    uint16_t fetched_word = 0;
    ok = fetch_data_word(&ctx, 0x1050, &fetched_word);
    assert(ok);
    assert(fetched_word == 0x55AA);
    assert(ctx.state.mem[5]->word[0x050] == 0x55AA);

    // Test fetch_data_words and store_data_words
    uint16_t words_to_store[3] = {0x1111, 0x2222, 0x3333};
    store_data_words(&ctx, 0x1051, 3, words_to_store);

    uint16_t fetched_words[3] = {0, 0, 0};
    fetch_data_words(&ctx, 0x1051, 3, fetched_words);
    assert(fetched_words[0] == 0x1111);
    assert(fetched_words[1] == 0x2222);
    assert(fetched_words[2] == 0x3333);

    // Test cross-page boundary operations
    ctx.state.pagereg[DATA][0][2].ppa = 6;
    uint16_t cross_words[2] = {0xAAAA, 0xBBBB};
    store_data_words(&ctx, 0x1FFF, 2, cross_words);

    uint16_t fetched_cross[2] = {0, 0};
    fetch_data_words(&ctx, 0x1FFF, 2, fetched_cross);
    assert(fetched_cross[0] == 0xAAAA);
    assert(fetched_cross[1] == 0xBBBB);

    // Verify physical layout directly
    assert(ctx.state.mem[5]->word[0xFFF] == 0xAAAA);
    assert(ctx.state.mem[6]->word[0x000] == 0xBBBB);

    printf("PASSED\n");
}

void test_Memory_Cache() {
    reset_cpu();
    printf("Testing Memory Caching Mechanism... ");

    ctx.state.reg.sw = 0x0000;
    ctx.state.pagereg[DATA][0][3].ppa = 8;
    ctx.state.pagereg[CODE][0][4].ppa = 9;

    // Initially caches are invalid
    assert(ctx.state.data_write_cache.valid == 0);
    assert(ctx.state.data_read_cache.valid == 0);
    assert(ctx.state.code_read_cache.valid == 0);

    // 1. Data Write Cache
    store_data_word(&ctx, 0x3100, 0x1234);
    // Logical qpage is 0x3100 >> 10 = 0x0C = 12
    assert(ctx.state.data_write_cache.valid & (0x8000000000000000ULL >> 12));
    assert(ctx.state.data_write_cache.page[12/4] == 8);

    // 2. Data Read Cache
    uint16_t fetched;
    fetch_data_word(&ctx, 0x3100, &fetched);
    // Logical page is 3
    assert(ctx.state.data_read_cache.valid & (0x8000U >> 3));
    assert(ctx.state.data_read_cache.page[3] == 8);

    // 3. Code Read Cache
    get_page_address_read_code(&ctx.state, 4);
    assert(ctx.state.code_read_cache.valid & (0x8000U >> 4));
    assert(ctx.state.code_read_cache.page[4] == 9);

    // 4. Writing to page registers should invalidate caches
    uint16_t opcode = 0x4800; // XIO R0, 0x5200 (Write Operand Page Register)
    ctx.state.reg.r[0] = 0x1234;
    poke(&ctx.state, 0x0001, 0x5200);
    uint16_t imm = 0;
    peek(&ctx.state, 0x0001, &imm);
    interpret_XIO(&ctx, opcode, imm);

    assert(ctx.state.data_read_cache.valid == 0);
    assert(ctx.state.data_write_cache.valid == 0);

    opcode = 0x4800; // XIO R0, 0x5100 (Write Instruction Page Register)
    poke(&ctx.state, 0x0001, 0x5100);
    peek(&ctx.state, 0x0001, &imm);
    interpret_XIO(&ctx, opcode, imm);

    assert(ctx.state.code_read_cache.valid == 0);

    printf("PASSED\n");
}

void test_Arithmetic() {
    reset_cpu();
    printf("Testing Arithmetic... ");

    // Test AR (Add Register)
    // Opcode A1xy -> AR R2, R3 (0xA123)
    ctx.state.reg.r[2] = 0x1111;
    ctx.state.reg.r[3] = 0x2222;
    interpret_AR(&ctx, 0xA123, 0);
    assert(ctx.state.reg.r[2] == 0x3333);
    assert(ctx.state.reg.sw & CS_POSITIVE);

    // Test SR (Subtract Register)
    // Opcode B1xy -> SR R4, R5 (0xB145)
    ctx.state.reg.r[4] = 0x5555;
    ctx.state.reg.r[5] = 0x1111;
    interpret_SR(&ctx, 0xB145, 0);
    assert(ctx.state.reg.r[4] == 0x4444);
    assert(ctx.state.reg.sw & CS_POSITIVE);

    // Test MR (Multiply Register)
    // Opcode C5xy -> MR R6, R7 (0xC567)
    // R6 gets high word, R7 gets low word (but RA must be even, so R6, R7 are the pair)
    ctx.state.reg.r[6] = 0x0002;
    ctx.state.reg.r[7] = 0xFFFF; // -1
    interpret_MR(&ctx, 0xC567, 0);
    // 2 * -1 = -2 = 0xFFFFFFFE -> R6=0xFFFF, R7=0xFFFE
    assert(ctx.state.reg.r[6] == (int16_t)0xFFFF);
    assert(ctx.state.reg.r[7] == (int16_t)0xFFFE);
    assert(ctx.state.reg.sw & CS_NEGATIVE);

    // Test DR (Divide Register)
    // Opcode D5xy -> DR R8, R10 (0xD58A)
    // A / B -> Quotient in RA, Remainder in RA+1
    // A is a 32-bit number formed by (R8 << 16) | R9
    ctx.state.reg.r[8] = 0;
    ctx.state.reg.r[9] = 10;
    ctx.state.reg.r[10] = 3;
    interpret_DR(&ctx, 0xD58A, 0);
    assert(ctx.state.reg.r[8] == 3);  // Quotient
    assert(ctx.state.reg.r[9] == 1);  // Remainder

    printf("PASSED\n");
}

void test_BitLogic() {
    reset_cpu();
    printf("Testing Bit and Logic Operations... ");

    // SBR (Set Bit Register) - 0x51xy
    ctx.state.reg.r[0] = 0x0000;
    interpret_SBR(&ctx, 0x5130, 0); // Set bit 3 in R0
    assert(ctx.state.reg.r[0] == 0x1000); // 0001 0000 ...

    // RBR (Reset Bit Register) - 0x54xy
    interpret_RBR(&ctx, 0x5430, 0);
    assert(ctx.state.reg.r[0] == 0x0000);

    // TBR (Test Bit Register) - 0x57xy
    ctx.state.reg.r[1] = 0x1000;
    ctx.state.reg.sw = 0x0000;
    interpret_TBR(&ctx, 0x5731, 0); // Test bit 3
    assert(ctx.state.reg.sw & CS_POSITIVE); // Bit is on -> POSITIVE because bit 3 > 0

    ctx.state.reg.sw = 0x0000;
    interpret_TBR(&ctx, 0x5741, 0); // Test bit 4
    assert(ctx.state.reg.sw & CS_ZERO); // Bit is off -> ZERO

    // ANDR (AND Register) - 0xE3xy
    ctx.state.reg.r[2] = 0xF0F0;
    ctx.state.reg.r[3] = 0x0FF0;
    interpret_ANDR(&ctx, 0xE323, 0);
    assert(ctx.state.reg.r[2] == 0x00F0);

    // ORR (OR Register) - 0xE1xy
    ctx.state.reg.r[4] = 0x0A00;
    ctx.state.reg.r[5] = 0x000B;
    interpret_ORR(&ctx, 0xE145, 0);
    assert(ctx.state.reg.r[4] == 0x0A0B);

    // XORR (XOR Register) - 0xE5xy
    ctx.state.reg.r[6] = 0xFFFF;
    ctx.state.reg.r[7] = 0x00FF;
    interpret_XORR(&ctx, 0xE567, 0);
    assert(ctx.state.reg.r[6] == (int16_t)0xFF00);

    // NR (NAND Register) - 0xE7xy
    ctx.state.reg.r[8] = 0xFFFF;
    ctx.state.reg.r[9] = 0xFFFF;
    interpret_NR(&ctx, 0xE789, 0);
    assert(ctx.state.reg.r[8] == 0x0000);

    printf("PASSED\n");
}

void test_Stack() {
    reset_cpu();
    printf("Testing Stack Operations... ");

    // Setup Stack Pointer
    ctx.state.reg.r[15] = 0x1000;

    // Fill registers R3 to R5
    ctx.state.reg.r[3] = 0x3333;
    ctx.state.reg.r[4] = 0x4444;
    ctx.state.reg.r[5] = 0x5555;

    // PSHM (Push Multiple) - 0x9Fxy
    // Push R3 through R5 (RA=3, RB=5)
    interpret_PSHM(&ctx, 0x9F35, 0);

    // R15 should be decremented by 3
    assert(ctx.state.reg.r[15] == 0x0FFD);

    // Check memory (R15 grows downwards, so R5 is at 0x0FFD, R4 at 0x0FFE, R3 at 0x0FFF)
    assert(read_phys_memory(&ctx.state, 0x0FFD) == 0x3333); // R3
    assert(read_phys_memory(&ctx.state, 0x0FFE) == 0x4444); // R4
    assert(read_phys_memory(&ctx.state, 0x0FFF) == 0x5555); // R5

    // Clear registers
    ctx.state.reg.r[3] = 0;
    ctx.state.reg.r[4] = 0;
    ctx.state.reg.r[5] = 0;

    // POPM (Pop Multiple) - 0x8Fxy
    // Pop R3 through R5 (RA=3, RB=5)
    interpret_POPM(&ctx, 0x8F35, 0);

    // R15 should be back to 0x1000
    assert(ctx.state.reg.r[15] == 0x1000);

    // Check registers are restored
    assert(ctx.state.reg.r[3] == 0x3333);
    assert(ctx.state.reg.r[4] == 0x4444);
    assert(ctx.state.reg.r[5] == 0x5555);

    // Test Wrap-around push (RA > RB)
    // Push R14 through R1 (RA=14, RB=1) -> 14, 15, 0, 1 (4 registers)
    ctx.state.reg.r[14] = 0xEEEE;
    ctx.state.reg.r[15] = 0x2000; // Reset SP
    ctx.state.reg.r[0] = 0xAAAA;
    ctx.state.reg.r[1] = 0xBBBB;

    interpret_PSHM(&ctx, 0x9FE1, 0);

    // SP decremented by 4
    assert(ctx.state.reg.r[15] == 0x1FFC);

    // Check wrap-around memory structure
    assert(read_phys_memory(&ctx.state, 0x1FFC) == 0xEEEE); // R14
    // R15 should be stored as the value it had AT THE MOMENT of its insertion!
    // At insertion (after R14), R15 would be 0x1FFE (since 2 more registers R0, R1 are pushed after it,
    // wait: R15 is inserted at offset 1! Initial SP=0x2000, count=4. Final SP=0x1FFC.
    // 0x1FFC -> R14, 0x1FFD -> R15, 0x1FFE -> R0, 0x1FFF -> R1
    // The value of R15 inserted should be 0x1FFD.
    assert(read_phys_memory(&ctx.state, 0x1FFD) == 0x1FFD); // R15
    assert(read_phys_memory(&ctx.state, 0x1FFE) == 0xAAAA); // R0
    assert(read_phys_memory(&ctx.state, 0x1FFF) == 0xBBBB); // R1

    printf("PASSED\n");
}

void test_ControlFlow() {
    reset_cpu();
    printf("Testing Control Flow Operations... ");

    // Setup typical environment
    ctx.state.reg.ic = 0x1000;

    // BR (Branch Unconditional) - 0x74xy
    // 0x7405 -> Branch +5 (forward)
    interpret_BR(&ctx, 0x7405, 0);
    assert(ctx.state.reg.ic == 0x1005);

    // 0x74F0 -> Branch -16 (backward)
    // 0xF0 signed is -16. From 0x1005, it should go to 0x0FF5
    interpret_BR(&ctx, 0x74F0, 0);
    assert(ctx.state.reg.ic == 0x0FF5);

    // JC (Jump Conditional) - 0x70xy
    // 0x7070 -> Condition 7 is unconditional jump
    // Needs DO_ADDR -> Memory fetch
    ctx.state.reg.ic = 0x1000;
    ctx.state.reg.r[0] = 0x0000; // Base relative RX=0
    write_phys_memory(&ctx.state, 0x1001, 0x5555); // Immediate address value
    interpret_JC(&ctx, 0x7070, read_phys_memory(&ctx.state, 0x1001));
    assert(ctx.state.reg.ic == 0x5555); // Jumped unconditionally

    // JS (Jump to Subroutine) - 0x72xy
    ctx.state.reg.ic = 0x1000;
    ctx.state.reg.r[2] = 0x0000;
    write_phys_memory(&ctx.state, 0x1001, 0x6666); // Target Address
    interpret_JS(&ctx, 0x7220, read_phys_memory(&ctx.state, 0x1001));
    assert(ctx.state.reg.ic == 0x6666); // PC changed to subroutine
    assert(ctx.state.reg.r[2] == 0x1002); // Return address saved in R2

    // URS (Return from Subroutine) - 0x7Fxy
    // Memory should hold the target address
    ctx.state.reg.ic = 0x6666;
    ctx.state.reg.r[3] = 0x1002;
    write_phys_memory(&ctx.state, 0x1002, 0x8888); // Target return IC
    interpret_URS(&ctx, 0x7F30, 0);
    assert(ctx.state.reg.ic == 0x8888);
    assert(ctx.state.reg.r[3] == 0x1003); // Register incremented

    printf("PASSED\n");
}

void test_MOV() {
    reset_cpu();
    printf("Testing MOV Instruction... ");

    // Setup memory mapping for MOV (cross-page boundaries)
    // Source: logical page 1 (phys 5) to logical page 2 (phys 6)
    ctx.state.pagereg[DATA][0][1].ppa = 5;
    ctx.state.pagereg[DATA][0][2].ppa = 6;

    // Dest: logical page 3 (phys 7) to logical page 4 (phys 8)
    ctx.state.pagereg[DATA][0][3].ppa = 7;
    ctx.state.pagereg[DATA][0][4].ppa = 8;

    // Fill source memory
    // Start at 0x1FF0 (logical page 1), count = 32 words -> crosses to 0x200F (logical page 2)
    for (int i = 0; i < 32; i++) {
        store_data_word(&ctx, 0x1FF0 + i, 0xA000 + i);
    }

    // Set up registers for MOV
    // Opcode: MOV R2, R4 -> 0x9324 (RA=2, RB=4)
    // R2 = Dest Address (0x3FF0)
    // R3 = Count (32)
    // R4 = Source Address (0x1FF0)
    ctx.state.reg.r[2] = 0x3FF0;
    ctx.state.reg.r[3] = 32;
    ctx.state.reg.r[4] = 0x1FF0;

    uint16_t opcode = 0x9324;
    interpret_MOV(&ctx, opcode, 0);

    // Assert that the instruction processed correctly
    // Count should be 0, and R2/R4 should be advanced by 32
    assert(ctx.state.reg.r[3] == 0);
    assert((uint16_t)ctx.state.reg.r[2] == 0x3FF0 + 32);
    assert((uint16_t)ctx.state.reg.r[4] == 0x1FF0 + 32);

    // Verify destination memory
    // 0x3FF0 is in physical page 7, from offset 0xFF0 to 0xFFF (16 words)
    // 0x4000 is in physical page 8, from offset 0x000 to 0x00F (16 words)
    for (int i = 0; i < 32; i++) {
        uint16_t val;
        fetch_data_word(&ctx, 0x3FF0 + i, &val);
        assert(val == 0xA000 + i);
    }

    printf("PASSED\n");
}

void test_SQRT() {
    reset_cpu();
    printf("Testing SQRT (sqrt(4.0) = 2.0)... ");

    double_to_1750a_float(4.0, &ctx.state.reg.r[6], &ctx.state.reg.r[7]);

    /* Opcode: SQRT R6 -> Opcode 0x4E. RA=6. */
    uint16_t opcode = 0x4E60;
    interpret_SQRT(&ctx, opcode, 0);

    /* Result should be 2.0 */
    double result = float_1750a_to_double(ctx.state.reg.r[6], ctx.state.reg.r[7]);
    if (fabs(result - 2.0) > 0.000001) {
        fprintf(stderr, "\nFAIL! SQRT(4.0) = %f, expected 2.0\n", result);
        fprintf(stderr, "R6: %04x, R7: %04x\n", ctx.state.reg.r[6], ctx.state.reg.r[7]);
        assert(0);
    }
    printf("PASSED\n");

    printf("Testing SQRT (sqrt(1.0) = 1.0)... ");
    double_to_1750a_float(1.0, &ctx.state.reg.r[6], &ctx.state.reg.r[7]);
    interpret_SQRT(&ctx, 0x4E60, 0);
    result = float_1750a_to_double(ctx.state.reg.r[6], ctx.state.reg.r[7]);
    assert(result == 1.0);
    printf("PASSED\n");

    printf("Testing SQRT (sqrt(0.5) = 0.707106...)... ");
    double_to_1750a_float(0.5, &ctx.state.reg.r[6], &ctx.state.reg.r[7]);
    interpret_SQRT(&ctx, 0x4E60, 0);
    result = float_1750a_to_double(ctx.state.reg.r[6], ctx.state.reg.r[7]);
    assert(fabs(result - 0.70710678) < 0.0001);
    printf("PASSED\n");

    printf("Testing SQRT (sqrt(0.25) = 0.5)... ");
    double_to_1750a_float(0.25, &ctx.state.reg.r[6], &ctx.state.reg.r[7]);
    interpret_SQRT(&ctx, 0x4E60, 0);
    result = float_1750a_to_double(ctx.state.reg.r[6], ctx.state.reg.r[7]);
    assert(result == 0.5);
    printf("PASSED\n");

    printf("Testing SQRT (sqrt(25.0) = 5.0)... ");
    double_to_1750a_float(25.0, &ctx.state.reg.r[6], &ctx.state.reg.r[7]);
    interpret_SQRT(&ctx, 0x4E60, 0);
    result = float_1750a_to_double(ctx.state.reg.r[6], ctx.state.reg.r[7]);
    assert(result == 5.0);
    printf("PASSED\n");

    printf("Testing SQRT (sqrt(5.0) = 2.236067...)... ");
    double_to_1750a_float(5.0, &ctx.state.reg.r[6], &ctx.state.reg.r[7]);
    interpret_SQRT(&ctx, 0x4E60, 0);
    result = float_1750a_to_double(ctx.state.reg.r[6], ctx.state.reg.r[7]);
    assert(fabs(result - 2.2360679) < 0.0001);
    printf("PASSED\n");

    printf("Testing SQRT (sqrt(0.0) = 0.0)... ");
    ctx.state.reg.r[0] = 0x0000;
    ctx.state.reg.r[1] = 0x0000;

    opcode = 0x4E00;
    interpret_SQRT(&ctx, opcode, 0);

    assert(ctx.state.reg.r[0] == 0x0000);
    assert(ctx.state.reg.r[1] == 0x0000);
    printf("PASSED\n");

    printf("Testing SQRT (sqrt(-4.0) -> Overflow Trap)... ");
    double_to_1750a_float(-4.0, &ctx.state.reg.r[4], &ctx.state.reg.r[5]);

    ctx.state.reg.pir &= ~INTR_FIXOFL;
    opcode = 0x4E40;
    interpret_SQRT(&ctx, opcode, 0);

    assert(ctx.state.reg.pir & INTR_FIXOFL);
    printf("PASSED\n");
}

void test_FNEG_FABS() {
    reset_cpu();
    printf("Testing FNEG and FABS... ");

    double test_vals[] = { 0.5, -0.5, -1.0, 1.0, 0.0, 3.14159, -2.71828, 42.0, -42.0 };
    int num_tests = sizeof(test_vals) / sizeof(test_vals[0]);

    for (int i = 0; i < num_tests; i++) {
        double val = test_vals[i];

        // --- TEST FNEG ---
        ctx.state.reg.r[2] = 0x0000;
        ctx.state.reg.r[3] = 0x0000;
        double_to_1750a_float(val, &ctx.state.reg.r[2], &ctx.state.reg.r[3]);

        // FNEG R2 -> Opcode 0xDB. RA=2, RB=2.
        uint16_t opcode = 0xDB22;
        interpret_FNEG(&ctx, opcode, 0);

        double result = float_1750a_to_double(ctx.state.reg.r[2], ctx.state.reg.r[3]);

        if (fabs(result - (-val)) > 0.000001) {
            fprintf(stderr, "\nFAIL! FNEG(%f) = %f, expected %f\n", val, result, -val);
            fprintf(stderr, "R2: %04x, R3: %04x\n", ctx.state.reg.r[2], ctx.state.reg.r[3]);
            assert(0);
        }

        // --- TEST FABS ---
        ctx.state.reg.r[2] = 0x0000;
        ctx.state.reg.r[3] = 0x0000;
        double_to_1750a_float(val, &ctx.state.reg.r[2], &ctx.state.reg.r[3]);

        // FABS R2 -> Opcode 0xEA. RA=2, RB=2.
        opcode = 0xEA22;
        interpret_FABS(&ctx, opcode, 0);

        result = float_1750a_to_double(ctx.state.reg.r[2], ctx.state.reg.r[3]);

        if (fabs(result - fabs(val)) > 0.000001) {
            fprintf(stderr, "\nFAIL! FABS(%f) = %f, expected %f\n", val, result, fabs(val));
            fprintf(stderr, "R2: %04x, R3: %04x\n", ctx.state.reg.r[2], ctx.state.reg.r[3]);
            assert(0);
        }
    }
    printf("PASSED\n");
}

void test_EFA_EFS() {
    reset_cpu();
    printf("Testing EFA and EFS... ");

    double test_cases[][2] = {
        {5.0, 0.0},
        {0.0, 5.0},
        {0.0, 0.0},
        {5.0, 3.0},
        {-5.0, 0.0},
        {0.0, -5.0},
        {-5.0, 3.0},
        {5.0, -3.0},
        {-5.0, -3.0},
        {0.5, 0.5},
        {-0.5, -0.5},
        {1.0, -1.0},
        {-1.0, 1.0}
    };
    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_tests; i++) {
        double a = test_cases[i][0];
        double b = test_cases[i][1];

        // --- TEST EFA (Addition) ---
        ctx.state.reg.r[2] = 0x0000; ctx.state.reg.r[3] = 0x0000; ctx.state.reg.r[4] = 0x0000;
        double_to_1750a_efloat(a, &ctx.state.reg.r[2], &ctx.state.reg.r[3], &ctx.state.reg.r[4]);

        uint16_t w1=0, w2=0, w3=0;
        double_to_1750a_efloat(b, &w1, &w2, &w3);
        poke(&ctx.state, 0x1000, w1);
        poke(&ctx.state, 0x1001, w2);
        poke(&ctx.state, 0x1002, w3);
        poke(&ctx.state, 0x0001, 0x1000);

        // EFA R2, 0x1000 -> Opcode 0xCC, RA=2
    interpret_EFA(&ctx, 0xCC20, read_phys_memory(&ctx.state, 0x0001));

        double res_add = efloat_1750a_to_double(ctx.state.reg.r[2], ctx.state.reg.r[3], ctx.state.reg.r[4]);
        double exp_add = a + b;
        if (fabs(res_add - exp_add) > 0.00001) {
            fprintf(stderr, "\nFAIL! EFA(%f + %f) = %f, expected %f\n", a, b, res_add, exp_add);
            assert(0);
        }

        // --- TEST EFS (Subtraction) ---
        ctx.state.reg.r[2] = 0x0000; ctx.state.reg.r[3] = 0x0000; ctx.state.reg.r[4] = 0x0000;
        double_to_1750a_efloat(a, &ctx.state.reg.r[2], &ctx.state.reg.r[3], &ctx.state.reg.r[4]);

        double_to_1750a_efloat(b, &w1, &w2, &w3);
        poke(&ctx.state, 0x1000, w1);
        poke(&ctx.state, 0x1001, w2);
        poke(&ctx.state, 0x1002, w3);
        poke(&ctx.state, 0x0001, 0x1000);

        // EFS R2, 0x1000 -> Opcode 0xCD, RA=2
    interpret_EFS(&ctx, 0xCD20, read_phys_memory(&ctx.state, 0x0001));

        double res_sub = efloat_1750a_to_double(ctx.state.reg.r[2], ctx.state.reg.r[3], ctx.state.reg.r[4]);
        double exp_sub = a - b;
        if (fabs(res_sub - exp_sub) > 0.00001) {
            fprintf(stderr, "\nFAIL! EFS(%f - %f) = %f, expected %f\n", a, b, res_sub, exp_sub);
            assert(0);
        }
    }

    printf("PASSED\n");
}


void test_FA_FS() {
    reset_cpu();
    printf("Testing FA and FS... ");

    double test_cases[][2] = {
        {5.0, 0.0},
        {0.0, 5.0},
        {0.0, 0.0},
        {5.0, 3.0},
        {-5.0, 0.0},
        {0.0, -5.0},
        {-5.0, 3.0},
        {5.0, -3.0},
        {-5.0, -3.0},
        {0.5, 0.5},
        {-0.5, -0.5},
        {1.0, -1.0},
        {-1.0, 1.0}
    };
    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_tests; i++) {
        double a = test_cases[i][0];
        double b = test_cases[i][1];

        // --- TEST FA (Addition) ---
        ctx.state.reg.r[2] = 0x0000; ctx.state.reg.r[3] = 0x0000;
        double_to_1750a_float(a, &ctx.state.reg.r[2], &ctx.state.reg.r[3]);

        uint16_t w1=0, w2=0;
        double_to_1750a_float(b, &w1, &w2);
        poke(&ctx.state, 0x1000, w1);
        poke(&ctx.state, 0x1001, w2);
        poke(&ctx.state, 0x0001, 0x1000);

        // FA R2, 0x1000 -> Opcode 0xC8, RA=2
    interpret_FA(&ctx, 0xC820, read_phys_memory(&ctx.state, 0x0001));

        double res_add = float_1750a_to_double(ctx.state.reg.r[2], ctx.state.reg.r[3]);
        double exp_add = a + b;
        if (fabs(res_add - exp_add) > 0.00001) {
            fprintf(stderr, "\nFAIL! FA(%f + %f) = %f, expected %f\n", a, b, res_add, exp_add);
            assert(0);
        }

        // --- TEST FS (Subtraction) ---
        ctx.state.reg.r[2] = 0x0000; ctx.state.reg.r[3] = 0x0000;
        double_to_1750a_float(a, &ctx.state.reg.r[2], &ctx.state.reg.r[3]);

        double_to_1750a_float(b, &w1, &w2);
        poke(&ctx.state, 0x1000, w1);
        poke(&ctx.state, 0x1001, w2);
        poke(&ctx.state, 0x0001, 0x1000);

        // FS R2, 0x1000 -> Opcode 0xC9, RA=2
    interpret_FS(&ctx, 0xC920, read_phys_memory(&ctx.state, 0x0001));

        double res_sub = float_1750a_to_double(ctx.state.reg.r[2], ctx.state.reg.r[3]);
        double exp_sub = a - b;
        if (fabs(res_sub - exp_sub) > 0.00001) {
            fprintf(stderr, "\nFAIL! FS(%f - %f) = %f, expected %f\n", a, b, res_sub, exp_sub);
            assert(0);
        }
    }

    printf("PASSED\n");
}

void test_ESQR() {
    reset_cpu();
    printf("Testing ESQR (sqrt(4.0) = 2.0)... ");

    double_to_1750a_efloat(4.0, &ctx.state.reg.r[6], &ctx.state.reg.r[7], &ctx.state.reg.r[8]);

    /* Opcode: ESQR R6 -> Opcode 0x4D. RA=6. */
    uint16_t opcode = 0x4D60;
    interpret_ESQR(&ctx, opcode, 0);

    /* Result should be close to 2.0 */
    double result = efloat_1750a_to_double(ctx.state.reg.r[6], ctx.state.reg.r[7], ctx.state.reg.r[8]);
    if (fabs(result - 2.0) > 0.000001) {
        fprintf(stderr, "\nFAIL! ESQR(4.0) = %f, expected 2.0\n", result);
        fprintf(stderr, "R6: %04x, R7: %04x, R8: %04x\n", ctx.state.reg.r[6], ctx.state.reg.r[7], ctx.state.reg.r[8]);
        assert(0);
    }
    printf("PASSED\n");

    printf("Testing ESQR (sqrt(1.0) = 1.0)... ");
    double_to_1750a_efloat(1.0, &ctx.state.reg.r[6], &ctx.state.reg.r[7], &ctx.state.reg.r[8]);
    interpret_ESQR(&ctx, 0x4D60, 0);
    result = efloat_1750a_to_double(ctx.state.reg.r[6], ctx.state.reg.r[7], ctx.state.reg.r[8]);
    assert(result == 1.0);
    printf("PASSED\n");

    printf("Testing ESQR (sqrt(0.5) = 0.707106...)... ");
    double_to_1750a_efloat(0.5, &ctx.state.reg.r[6], &ctx.state.reg.r[7], &ctx.state.reg.r[8]);
    interpret_ESQR(&ctx, 0x4D60, 0);
    result = efloat_1750a_to_double(ctx.state.reg.r[6], ctx.state.reg.r[7], ctx.state.reg.r[8]);
    assert(fabs(result - 0.70710678) < 0.000001);
    printf("PASSED\n");

    printf("Testing ESQR (sqrt(0.25) = 0.5)... ");
    double_to_1750a_efloat(0.25, &ctx.state.reg.r[6], &ctx.state.reg.r[7], &ctx.state.reg.r[8]);
    interpret_ESQR(&ctx, 0x4D60, 0);
    result = efloat_1750a_to_double(ctx.state.reg.r[6], ctx.state.reg.r[7], ctx.state.reg.r[8]);
    assert(result == 0.5);
    printf("PASSED\n");

    printf("Testing ESQR (sqrt(25.0) = 5.0)... ");
    double_to_1750a_efloat(25.0, &ctx.state.reg.r[6], &ctx.state.reg.r[7], &ctx.state.reg.r[8]);
    interpret_ESQR(&ctx, 0x4D60, 0);
    result = efloat_1750a_to_double(ctx.state.reg.r[6], ctx.state.reg.r[7], ctx.state.reg.r[8]);
    assert(result == 5.0);
    printf("PASSED\n");

    printf("Testing ESQR (sqrt(5.0) = 2.236067...)... ");
    double_to_1750a_efloat(5.0, &ctx.state.reg.r[6], &ctx.state.reg.r[7], &ctx.state.reg.r[8]);
    interpret_ESQR(&ctx, 0x4D60, 0);
    result = efloat_1750a_to_double(ctx.state.reg.r[6], ctx.state.reg.r[7], ctx.state.reg.r[8]);
    assert(fabs(result - 2.2360679) < 0.000001);
    printf("PASSED\n");

    printf("Testing ESQR (sqrt(0.0) = 0.0)... ");
    ctx.state.reg.r[0] = 0x0000;
    ctx.state.reg.r[1] = 0x0000;
    ctx.state.reg.r[2] = 0x0000;

    opcode = 0x4D00;
    interpret_ESQR(&ctx, opcode, 0);

    assert(ctx.state.reg.r[0] == 0x0000);
    assert(ctx.state.reg.r[1] == 0x0000);
    assert(ctx.state.reg.r[2] == 0x0000);
    printf("PASSED\n");

    printf("Testing ESQR (sqrt(-4.0) -> Overflow Trap)... ");
    double_to_1750a_efloat(-4.0, &ctx.state.reg.r[4], &ctx.state.reg.r[5], &ctx.state.reg.r[6]);

    ctx.state.reg.pir &= ~INTR_FIXOFL;
    opcode = 0x4D40;
    interpret_ESQR(&ctx, opcode, 0);

    assert(ctx.state.reg.pir & INTR_FIXOFL);
    printf("PASSED\n");
}

void test_Single_Shifts() {
    reset_cpu();
    printf("Testing SRL (Shift Right Logical)... ");
    ctx.state.reg.r[0] = 0x8000;
    interpret_SRL(&ctx, 0x6220, 0);
    assert(ctx.state.reg.r[0] == 0x1000);
    printf("PASSED\n");

    printf("Testing SRA (Shift Right Arithmetic)... ");
    ctx.state.reg.r[0] = 0x8000;
    interpret_SRA(&ctx, 0x6330, 0);
    assert(ctx.state.reg.r[0] == (int16_t)0xF800);
    printf("PASSED\n");

    printf("Testing SLC (Shift Left Cyclic)... ");
    ctx.state.reg.r[0] = 0xC001;
    interpret_SLC(&ctx, 0x6420, 0);
    assert(ctx.state.reg.r[0] == 0x000E);
    printf("PASSED\n");

    printf("Testing SLR (Shift Logical Register)... ");
    ctx.state.reg.r[1] = 0x1234;
    ctx.state.reg.r[2] = 4;
    interpret_SLR(&ctx, 0x7E12, 0);
    assert(ctx.state.reg.r[1] == 0x2340);

    ctx.state.reg.r[1] = 0x1234;
    ctx.state.reg.r[2] = (uint16_t)-4;
    interpret_SLR(&ctx, 0x7E12, 0);
    assert(ctx.state.reg.r[1] == 0x0123);
    printf("PASSED\n");

    printf("Testing SAR (Shift Arithmetic Register)... ");
    ctx.state.reg.r[1] = 0x8000;
    ctx.state.reg.r[2] = (uint16_t)-4;
    interpret_SAR(&ctx, 0x7F12, 0);
    assert(ctx.state.reg.r[1] == (int16_t)0xF800);
    printf("PASSED\n");

    printf("Testing SCR (Shift Cyclic Register)... ");
    ctx.state.reg.r[1] = 0x8001;
    ctx.state.reg.r[2] = 4;
    interpret_SCR(&ctx, 0x6C12, 0);
    assert(ctx.state.reg.r[1] == 0x0018);
    printf("PASSED\n");

    printf("Testing Single Shift Boundaries (-16, 0, 16, -33, 33)... ");
    ctx.state.reg.r[1] = 0x1234;
    ctx.state.reg.r[2] = 0;
    interpret_SLR(&ctx, 0x7E12, 0);
    assert(ctx.state.reg.r[1] == 0x1234);

    ctx.state.reg.r[2] = 16;
    interpret_SLR(&ctx, 0x7E12, 0);
    assert(ctx.state.reg.r[1] == 0x0000);

    ctx.state.reg.r[1] = 0x1234;
    ctx.state.reg.r[2] = (uint16_t)-16;
    interpret_SLR(&ctx, 0x7E12, 0);
    assert(ctx.state.reg.r[1] == 0x0000);

    ctx.state.reg.r[1] = 0x1234;
    ctx.state.reg.r[2] = 33;
    ctx.state.reg.pir &= ~INTR_FIXOFL;
    interpret_SLR(&ctx, 0x7E12, 0);
    assert(ctx.state.reg.pir & INTR_FIXOFL);

    ctx.state.reg.r[2] = (uint16_t)-33;
    ctx.state.reg.pir &= ~INTR_FIXOFL;
    interpret_SLR(&ctx, 0x7E12, 0);
    assert(ctx.state.reg.pir & INTR_FIXOFL);
    printf("PASSED\n");
}

void test_Double_Shifts() {
    reset_cpu();
    printf("Testing DSLL (Double Shift Left Logical)... ");
    ctx.state.reg.r[0] = 0x0123;
    ctx.state.reg.r[1] = 0x4567;
    interpret_DSLL(&ctx, 0x6830, 0);
    assert(ctx.state.reg.r[0] == 0x1234);
    assert(ctx.state.reg.r[1] == 0x5670);
    printf("PASSED\n");

    printf("Testing DSRL (Double Shift Right Logical)... ");
    ctx.state.reg.r[0] = 0x8123;
    ctx.state.reg.r[1] = 0x4567;
    interpret_DSRL(&ctx, 0x6A30, 0);
    assert(ctx.state.reg.r[0] == 0x0812);
    assert(ctx.state.reg.r[1] == 0x3456);
    printf("PASSED\n");

    printf("Testing DSRA (Double Shift Right Arithmetic)... ");
    ctx.state.reg.r[0] = 0x8123;
    ctx.state.reg.r[1] = 0x4567;
    interpret_DSRA(&ctx, 0x6B30, 0);
    assert(ctx.state.reg.r[0] == (int16_t)0xF812);
    assert(ctx.state.reg.r[1] == 0x3456);
    printf("PASSED\n");

    printf("Testing DSLC (Double Shift Left Cyclic)... ");
    ctx.state.reg.r[0] = 0x8123;
    ctx.state.reg.r[1] = 0x4567;
    interpret_DSLC(&ctx, 0x6C30, 0);
    assert(ctx.state.reg.r[0] == 0x1234);
    assert(ctx.state.reg.r[1] == 0x5678);
    printf("PASSED\n");

    printf("Testing DSLR (Double Shift Logical Register)... ");
    ctx.state.reg.r[1] = 0x1234;
    ctx.state.reg.r[2] = 0x5678;
    ctx.state.reg.r[3] = 8;
    interpret_DSLR(&ctx, 0x7E13, 0);
    assert(ctx.state.reg.r[1] == 0x3456);
    assert(ctx.state.reg.r[2] == 0x7800);

    ctx.state.reg.r[1] = 0x1234;
    ctx.state.reg.r[2] = 0x5678;
    ctx.state.reg.r[3] = (uint16_t)-8;
    interpret_DSLR(&ctx, 0x7E13, 0);
    assert(ctx.state.reg.r[1] == 0x0012);
    assert(ctx.state.reg.r[2] == 0x3456);
    printf("PASSED\n");

    printf("Testing DSAR (Double Shift Arithmetic Register)... ");
    ctx.state.reg.r[1] = 0x8234;
    ctx.state.reg.r[2] = 0x5678;
    ctx.state.reg.r[3] = (uint16_t)-8;
    interpret_DSAR(&ctx, 0x7F13, 0);
    assert(ctx.state.reg.r[1] == (int16_t)0xFF82);
    assert(ctx.state.reg.r[2] == 0x3456);
    printf("PASSED\n");

    printf("Testing DSCR (Double Shift Cyclic Register)... ");
    ctx.state.reg.r[1] = 0x8123;
    ctx.state.reg.r[2] = 0x4567;
    ctx.state.reg.r[3] = 4;
    interpret_DSCR(&ctx, 0x6D13, 0);
    assert(ctx.state.reg.r[1] == 0x1234);
    assert(ctx.state.reg.r[2] == 0x5678);
    printf("PASSED\n");

    printf("Testing Double Shift Boundaries (-32, 0, 32, -33, 33)... ");
    ctx.state.reg.r[1] = 0x1234;
    ctx.state.reg.r[2] = 0x5678;
    ctx.state.reg.r[3] = 0;
    interpret_DSLR(&ctx, 0x7E13, 0);
    assert(ctx.state.reg.r[1] == 0x1234);
    assert(ctx.state.reg.r[2] == 0x5678);

    ctx.state.reg.r[3] = 32;
    interpret_DSLR(&ctx, 0x7E13, 0);
    assert(ctx.state.reg.r[1] == 0x0000);
    assert(ctx.state.reg.r[2] == 0x0000);

    ctx.state.reg.r[1] = 0x1234;
    ctx.state.reg.r[2] = 0x5678;
    ctx.state.reg.r[3] = (uint16_t)-32;
    interpret_DSLR(&ctx, 0x7E13, 0);
    assert(ctx.state.reg.r[1] == 0x0000);
    assert(ctx.state.reg.r[2] == 0x0000);

    ctx.state.reg.r[1] = 0x1234;
    ctx.state.reg.r[2] = 0x5678;
    ctx.state.reg.r[3] = 33;
    ctx.state.reg.pir &= ~INTR_FIXOFL;
    interpret_DSLR(&ctx, 0x7E13, 0);
    assert(ctx.state.reg.pir & INTR_FIXOFL);

    ctx.state.reg.r[3] = (uint16_t)-33;
    ctx.state.reg.pir &= ~INTR_FIXOFL;
    interpret_DSLR(&ctx, 0x7E13, 0);
    assert(ctx.state.reg.pir & INTR_FIXOFL);
    printf("PASSED\n");
}
