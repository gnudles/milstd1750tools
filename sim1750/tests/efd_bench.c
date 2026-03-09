#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#define NUM_DIVISIONS 10000000  /* 10 Million Divisions */

/* --- DOUBLE CONVERSION HELPERS --- */
void double_to_1750a_efloat(double val, uint16_t *w1, uint16_t *w2, uint16_t *w3) {
    if (val == 0.0) { *w1 = *w2 = *w3 = 0; return; }
    int exp;
    double frac = frexp(val, &exp);
    if (frac == -0.5) { frac = -1.0; exp -= 1; }
    if (exp > 127 || exp < -128) { *w1 = *w2 = *w3 = 0; return; }
    int64_t mantissa = (int64_t)ldexp(frac, 39);
    *w1 = (uint16_t)((mantissa >> 24) & 0xFFFF);
    *w2 = (uint16_t)(((mantissa >> 8) & 0xFF00) | (exp & 0xFF));
    *w3 = (uint16_t)(mantissa & 0xFFFF);
}

double efloat_1750a_to_double(uint16_t w1, uint16_t w2, uint16_t w3) {
    int64_t mantissa = ((int64_t)w1 << 24) | ((int64_t)(w2 & 0xFF00) << 8) | w3;
    mantissa = (mantissa << 24) >> 24;
    if (mantissa == 0) return 0.0;
    int8_t exp = (int8_t)(w2 & 0xFF);
    return ldexp((double)mantissa, exp - 39);
}

/* --- THE 3 IMPLEMENTATIONS --- */

/* Method 1: IEEE 754 Double Cast */
void efd_double(uint16_t *r, uint16_t *DO) {
    double a = efloat_1750a_to_double(r[0], r[1], r[2]);
    double b = efloat_1750a_to_double(DO[0], DO[1], DO[2]);
    double res = a / b;
    double_to_1750a_efloat(res, &r[0], &r[1], &r[2]);
}

/* Base unpacking/normalization macro for the integer methods */
#define UNPACK_AND_DIVIDE(MATH_CORE) do { \
    int64_t W1_A = r[0], W2_A = r[1], W3_A = r[2]; \
    int64_t Q; \
    int64_t M_A = (W1_A << 24) | ((W2_A & 0xFF00) << 8) | (W3_A & 0xFFFF); \
    M_A = (M_A << 24) >> 24; \
    int16_t E_A = (int8_t)(W2_A & 0xFF); \
    int64_t W1_B = DO[0], W2_B = DO[1], W3_B = DO[2]; \
    int64_t M_B = (W1_B << 24) | ((W2_B & 0xFF00) << 8) | (W3_B & 0xFFFF); \
    M_B = (M_B << 24) >> 24; \
    int16_t E_B = (int8_t)(W2_B & 0xFF); \
    if (M_B == 0) return; \
    uint64_t M_A_abs = (M_A < 0) ? -M_A : M_A; \
    uint64_t M_B_abs = (M_B < 0) ? -M_B : M_B; \
    MATH_CORE \
    if (Q == 0) { r[0]=0; r[1]=0; r[2]=0; } else { \
        uint64_t x = Q ^ (Q >> 1); \
        int clz = __builtin_clzll(x); \
        int shift_amt = clz - 25; \
        int64_t M_res_top = (shift_amt > 0) ? (Q << shift_amt) : (Q >> -shift_amt); \
        int64_t M_final = M_res_top >> 24; \
        int E_final = E_A - E_B + 24 - shift_amt; \
        if (E_final > 127 || E_final < -128) { r[0]=0; r[1]=0; r[2]=0; } else { \
            r[0] = (uint16_t)((M_final >> 24) & 0xFFFF); \
            r[1] = (uint16_t)((M_final >> 8) & 0xFF00) | (uint8_t)E_final; \
            r[2] = (uint16_t)(M_final & 0xFFFF); \
        } \
    } \
} while(0)

/* Method 2: Compiler __int128 */
void efd_int128(uint16_t *r, uint16_t *DO) {
    UNPACK_AND_DIVIDE({
        unsigned __int128 dividend = (unsigned __int128)M_A_abs << 39;
        uint64_t Q_abs = (uint64_t)(dividend / M_B_abs);
        Q = ((M_A ^ M_B) < 0) ? -(int64_t)Q_abs : (int64_t)Q_abs;
    });
}

/* Method 3: Multi-Precision 64-bit Chunked */
void efd_chunked(uint16_t *r, uint16_t *DO) {
    UNPACK_AND_DIVIDE({
        uint64_t div_hi = M_A_abs << 17;
        uint64_t Q_hi = div_hi / M_B_abs;
        uint64_t rem = div_hi % M_B_abs;
        uint64_t div_lo = rem << 22;
        uint64_t Q_lo = div_lo / M_B_abs;
        uint64_t Q_abs = (Q_hi << 22) + Q_lo;
        Q = ((M_A ^ M_B) < 0) ? -(int64_t)Q_abs : (int64_t)Q_abs;
    });
}

/* --- BENCHMARK HARNESS --- */
typedef struct { uint16_t w[3]; } Float48;

Float48 rand_float48() {
    Float48 f;
    double val = ((double)rand() / RAND_MAX) * 2000.0 - 1000.0;
    if (val == 0.0) val = 1.0; // Avoid div by zero
    double_to_1750a_efloat(val, &f.w[0], &f.w[1], &f.w[2]);
    return f;
}

int main() {
    srand(1750);
    
    printf("Generating %d random 1750A float pairs...\n", NUM_DIVISIONS);
    Float48 *A = malloc(sizeof(Float48) * NUM_DIVISIONS);
    Float48 *B = malloc(sizeof(Float48) * NUM_DIVISIONS);
    for (int i = 0; i < NUM_DIVISIONS; i++) {
        A[i] = rand_float48();
        B[i] = rand_float48();
    }

    printf("\nVerifying mathematical equivalency...\n");
    for (int i = 0; i < 100; i++) {
        uint16_t r1[3]={A[i].w[0], A[i].w[1], A[i].w[2]};
        uint16_t r2[3]={A[i].w[0], A[i].w[1], A[i].w[2]};
        uint16_t r3[3]={A[i].w[0], A[i].w[1], A[i].w[2]};
        uint16_t d[3]={B[i].w[0], B[i].w[1], B[i].w[2]};
        
        efd_double(r1, d);
        efd_int128(r2, d);
        efd_chunked(r3, d);
        
        /* Note: Double might be off by 1 LSB due to IEEE rounding. 
           Chunked and Int128 must be identical. */
        if (r2[0] != r3[0] || r2[1] != r3[1] || r2[2] != r3[2]) {
            printf("Mismatch found! Math is broken.\n");
            return 1;
        }
    }
    printf("Math is 100%% identical. Starting Benchmark.\n\n");

    clock_t start, end;
    double cpu_time_used;
    volatile uint16_t sink = 0; // Prevents optimizer from dropping the loop

    // Test 1: Double Cast
    start = clock();
    for (int i = 0; i < NUM_DIVISIONS; i++) {
        uint16_t r[3]={A[i].w[0], A[i].w[1], A[i].w[2]};
        efd_double(r, B[i].w);
        sink ^= r[0];
    }
    end = clock();
    printf("1. IEEE 754 Double Cast:  %f seconds\n", ((double) (end - start)) / CLOCKS_PER_SEC);

    // Test 2: __int128
    start = clock();
    for (int i = 0; i < NUM_DIVISIONS; i++) {
        uint16_t r[3]={A[i].w[0], A[i].w[1], A[i].w[2]};
        efd_int128(r, B[i].w);
        sink ^= r[0];
    }
    end = clock();
    printf("2. __int128 Compiler:     %f seconds\n", ((double) (end - start)) / CLOCKS_PER_SEC);

    // Test 3: 64-bit Chunked
    start = clock();
    for (int i = 0; i < NUM_DIVISIONS; i++) {
        uint16_t r[3]={A[i].w[0], A[i].w[1], A[i].w[2]};
        efd_chunked(r, B[i].w);
        sink ^= r[0];
    }
    end = clock();
    printf("3. 64-bit Chunked Radix:  %f seconds\n", ((double) (end - start)) / CLOCKS_PER_SEC);

    free(A); free(B);
    return 0;
}
