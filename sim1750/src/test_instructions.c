#include <stdio.h>
#include <assert.h>
#include <string.h>

/* --- Stubs to link against the generated code --- */
#include "cpu_ctx.h"

/* Mock memory array */
uint16_t mock_memory[0x10000];

bool peek(struct cpu_state *state, uint phys_addr, ushort *word) {
    *word = mock_memory[phys_addr];
    return true;
}
bool poke(struct cpu_state *state, uint phys_addr, ushort value) {
    mock_memory[phys_addr] = value;
    return true;
}
uint get_phys_address(struct cpu_state *state, int space, int as, uint16_t addr) {
    return addr; /* Flat memory mapping for tests */
}

/* Include the generated functions (or compile together) */


#include "generated_cpu.h"


/* --- The Test Harness --- */
struct cpu_context ctx;

void reset_cpu() {
    memset(&ctx, 0, sizeof(struct cpu_context));
    memset(mock_memory, 0, sizeof(mock_memory));
}

void test_LB_Base_Relative() {
    reset_cpu();
    printf("Testing LB (Load Base Relative)... ");
    
    /* Setup: Base Register R12 = 0x1000 */
    ctx.state.reg.r[13] = 0x1000;
    
    /* Setup: Memory at 0x1005 holds -5 (0xFFFB) */
    mock_memory[0x1005] = 0xFFFB;
    
    /* Opcode: LB R13, 5 -> BR=13 (0x1 in field), Disp=0x05 */
    /* LB opcode is 0x00,  Disp=0x05 -> 0x0025 */
    uint16_t opcode = 0x0105; 
    
    interpret_LB(&ctx, opcode);
    
    /* Verifications */
    assert(ctx.state.reg.r[2] == (int16_t)0xFFFB); // Loaded correctly
    assert(ctx.state.reg.ic == 1);                 // IC incremented
    assert(ctx.state.total_cycles == 3);           // Cycles added
    
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
    
    interpret_SLL(&ctx, opcode);
    
    /* Verification: 0x0F00 << 4 = 0xF000 */
    assert(ctx.state.reg.r[5] == (int16_t)0xF000); 
    
    /* Flag Verification: 0xF000 is negative */
    assert(ctx.state.reg.sw & 0x1000); // CS_N_BIT
    printf("PASSED\n");
}

void test_STUB_Upper_Byte() {
    reset_cpu();
    printf("Testing STUB (Store Upper Byte)... ");
    
    /* Setup: R2 holds 0xAABB */
    ctx.state.reg.r[2] = 0xAABB;
    
    /* Setup: Memory at 0x2000 holds 0x1122 */
    mock_memory[0x2000] = 0x1122;
    
    /* Opcode: STUB R2, 0x2000 -> RA=2, RX=0. IMM=0x2000 */
    /* STUB opcode is 0x9B. RA=2 -> 0x9B20. */
    uint16_t opcode = 0x9B20;
    ctx.state.reg.ic = 0x0000;
    mock_memory[0x0001] = 0x2000; // Immediate address
    
    interpret_STUB(&ctx, opcode);
    
    /* Verification: Memory should be updated to (Lower byte of R2 in Upper byte of Memory)
       Lower byte of R2 is 0xBB. Memory upper byte becomes 0xBB. Lower byte preserved (0x22).
       Result: 0xBB22 */
    assert(mock_memory[0x2000] == 0xBB22);
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
    mock_memory[0x1000] = 0x4000;
    mock_memory[0x1001] = 0x0002;
    
    /* Opcode: FD R2, 0x1000 -> Opcode 0xD8. RA=2. RX=0. */
    uint16_t opcode = 0xD820; 
    mock_memory[0x0001] = 0x1000; /* Immediate Address */
    
    interpret_FD(&ctx, opcode);
    
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
    mock_memory[0x2000] = 0x6000;
    mock_memory[0x2001] = 0x0001;
    
    /* Opcode: FD R4, 0x2000 -> Opcode 0xD8. RA=4. */
    uint16_t opcode = 0xD840; 
    mock_memory[0x0001] = 0x2000;
    
    interpret_FD(&ctx, opcode);
    
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
    mock_memory[0x3000] = 0x4000;
    mock_memory[0x3001] = 0x0003;
    mock_memory[0x3002] = 0x0000;
    
    /* Opcode: EFD R6, 0x3000 -> Opcode 0xDA. RA=6. */
    uint16_t opcode = 0xDA60; 
    mock_memory[0x0001] = 0x3000;
    
    interpret_EFD(&ctx, opcode);
    
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
    mock_memory[0x1000] = 0x0000;
    mock_memory[0x1001] = 0x0000;
    
    uint16_t opcode = 0xD820; 
    mock_memory[0x0001] = 0x1000;
    
    interpret_FD(&ctx, opcode);
    
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
    *w2 = (uint16_t)(((mantissa << 8) & 0xFF00) | (exp & 0xFF));
}

double float_1750a_to_double(uint16_t w1, uint16_t w2) {
    int32_t mantissa = ((int32_t)w1 << 8) | ((w2 >> 8) & 0xFF);
    mantissa = (mantissa << 8) >> 8; /* Sign extend from 24-bits */
    if (mantissa == 0) return 0.0;
    int8_t exp = (int8_t)(w2 & 0xFF);
    return ldexp((double)mantissa, exp - 23);
}

void test_Extended_Float_Pi_Pipeline() {
    reset_cpu();
    printf("Testing Extended Float Pipeline (Pi Approx)...\n");

    /* --- 1. CONVERT 355 to EFLT (Numerator) --- */
    ctx.state.reg.r[0] = 0x0000;
    ctx.state.reg.r[1] = 355;
    /* OP_INT32_TO_EFLT RA=0, RB=0. Target is R[RA], Source is R[RB] */
    ctx.state.reg.ic = 0;
    interpret_EFLT(&ctx, 0x0000); 
    /* R0, R1, R2 now hold exactly 355.0 */

    /* --- 2. CONVERT 113 to EFLT (Denominator) --- */
    ctx.state.reg.r[4] = 0x0000;
    ctx.state.reg.r[5] = 113;
    /* OP_INT32_TO_EFLT RA=4, RB=4 */
    ctx.state.reg.ic = 0;
    interpret_EFLT(&ctx, 0x0044); 
    /* R4, R5, R6 now hold exactly 113.0 */

    /* --- 3. DIVIDE (Pi Approx: 355.0 / 113.0) --- */
    /* Push Denominator to Memory so we can use EFD (Divide by Memory) */
    mock_memory[0x1000] = ctx.state.reg.r[4];
    mock_memory[0x1001] = ctx.state.reg.r[5];
    mock_memory[0x1002] = ctx.state.reg.r[6];
    mock_memory[0x0001] = 0x1000; /* Instruction fetcher DO address */
    /* OP_DIV_EXFLOAT RA=0. R0 = R0 / Mem */
    ctx.state.reg.ic = 0;
    interpret_EFD(&ctx, 0x0000); 
    /* R0, R1, R2 now hold 3.1415929... */

    /* --- 4. ADD 1000.0 --- */
    ctx.state.reg.r[4] = 0x0000;
    ctx.state.reg.r[5] = 1000;
    ctx.state.reg.ic = 0;
    interpret_EFLT(&ctx, 0x0044); 
    mock_memory[0x2000] = ctx.state.reg.r[4];
    mock_memory[0x2001] = ctx.state.reg.r[5];
    mock_memory[0x2002] = ctx.state.reg.r[6];
    mock_memory[0x0001] = 0x2000;
    ctx.state.reg.ic = 0;
    /* OP_ADD_EXFLOAT RA=0. R0 = R0 + Mem */
    interpret_EFA(&ctx, 0x0000); 
    /* R0, R1, R2 now hold 1003.14159... */

    /* --- 5. SUBTRACT 500.0 --- */
    ctx.state.reg.r[4] = 0x0000;
    ctx.state.reg.r[5] = 500;
    ctx.state.reg.ic = 0;
    interpret_EFLT(&ctx, 0x0044); 
    mock_memory[0x3000] = ctx.state.reg.r[4];
    mock_memory[0x3001] = ctx.state.reg.r[5];
    mock_memory[0x3002] = ctx.state.reg.r[6];
    mock_memory[0x0001] = 0x3000;
    ctx.state.reg.ic = 0;
    /* OP_SUB_EXFLOAT RA=0. R0 = R0 - Mem */
    interpret_EFS(&ctx, 0x0000); 
    /* R0, R1, R2 now hold 503.14159... */

    /* --- 6. MULTIPLY BY 2.0 --- */
    ctx.state.reg.r[4] = 0x0000;
    ctx.state.reg.r[5] = 2;
    ctx.state.reg.ic = 0;
    interpret_EFLT(&ctx, 0x0044); 
    mock_memory[0x4000] = ctx.state.reg.r[4];
    mock_memory[0x4001] = ctx.state.reg.r[5];
    mock_memory[0x4002] = ctx.state.reg.r[6];
    mock_memory[0x0001] = 0x4000;
    ctx.state.reg.ic = 0;
    /* OP_MULT_EXFLOAT RA=0. R0 = R0 * Mem */
    interpret_EFM(&ctx, 0x0000); 
    /* R0, R1, R2 now hold 1006.283185... */

    /* --- VERIFICATION 1: Double Precision Threshold Check --- */
    double final_val = efloat_1750a_to_double(ctx.state.reg.r[0], ctx.state.reg.r[1], ctx.state.reg.r[2]);
    double expected = ((355.0 / 113.0) + 1000.0 - 500.0) * 2.0;
    
    /* Assert we are accurate to 5 decimal places against the host FPU */
    printf("(Float: %lf, expected: %lf)\n", final_val, expected);
    assert(fabs(final_val - expected) < 0.00001);

    /* --- 7. CONVERT BACK TO INT32 --- */
    /* OP_EFLT_TO_INT32 RA=4, RB=0. R4, R5 gets Int32 of Float R0 */
    interpret_EFIX(&ctx, 0x0040); 
    
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
    test_STUB_Upper_Byte();
    printf("All instruction tests passed.\n");
    test_FD_Basic_Division();
    test_FD_Negative_Normalization();
    test_EFD_48bit_Math();
    test_FD_Divide_By_Zero();
    printf("All floating point tests passed.\n");

    return 0;
}
