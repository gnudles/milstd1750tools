#include "cpu_helpers.h"
#include "clock_cycles.h"
/* LB - Single precision load, Base Relative*/
void interpret_LB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    cpu_ctx->state.reg.r[(RA+0)&0xF] = DO[0];
    calculate_flags_16bit(cpu_ctx, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_LB;
    cpu_ctx->state.reg.ic += 1;
}

/* DLB - Double precision load, Base Relative*/
void interpret_DLB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 0;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    cpu_ctx->state.reg.r[(RA+0)&0xF] = DO[0];
    cpu_ctx->state.reg.r[(RA+1)&0xF] = DO[1];
    calculate_flags_32bit(cpu_ctx, DO);
    cpu_ctx->state.total_cycles += CLK_CYC_DLB;
    cpu_ctx->state.reg.ic += 1;
}

/* STB - Single precision store, Base Relative*/
void interpret_STB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    ok = store_data_word(cpu_ctx, DO_ADDR, cpu_ctx->state.reg.r[RA]);
    cpu_ctx->state.total_cycles += CLK_CYC_STB;
    cpu_ctx->state.reg.ic += 1;
}

/* DSTB - Double precision store, Base Relative*/
void interpret_DSTB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 0;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    ok = 0 == store_data_words_reg(cpu_ctx, DO_ADDR, 2, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_DSTB;
    cpu_ctx->state.reg.ic += 1;
}

/* AB - Single precision integer add, Base Relative*/
void interpret_AB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int32_t res = (int32_t)a + (int32_t)b;
    bool carry = ((uint32_t)(uint16_t)a + (uint32_t)(uint16_t)b) > 0xFFFF;
    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    calculate_flags_16bit(cpu_ctx, (int16_t)res);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_AB;
    cpu_ctx->state.reg.ic += 1;
}

/* SBB - Single precision integer subtract, Base Relative*/
void interpret_SBB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int32_t res = (int32_t)a - (int32_t)b;
    bool carry = (uint16_t)a >= (uint16_t)b;
    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    calculate_flags_16bit(cpu_ctx, (int16_t)res);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_SBB;
    cpu_ctx->state.reg.ic += 1;
}

/* MB - Single precision integer multiply with 32-bit product, Base Relative*/
void interpret_MB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    int32_t prod = (int32_t)A * (int32_t)B;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)((prod >> 16) & 0xFFFF);
    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(prod & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_MB;
    cpu_ctx->state.reg.ic += 1;
}

/* DB - Single precision integer divide with 32-bit dividend, Base Relative*/
void interpret_DB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int32_t A = (int32_t)(((uint32_t)(uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF]);
    int16_t B = (int16_t)DO[0];
    if (B == 0) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    } else {
        int32_t Q = A / B;
        int16_t Rem = A % B;
        if (Q > 32767 || Q < -32768) {
            cpu_ctx->state.reg.pir |= INTR_FIXOFL;
        } else {
            cpu_ctx->state.reg.r[(RA+0)&0xF] = Q;
            cpu_ctx->state.reg.r[(RA+1)&0xF] = Rem;
            calculate_flags_16bit(cpu_ctx, (int16_t)Q);
        }
    }
    cpu_ctx->state.total_cycles += CLK_CYC_DB;
    cpu_ctx->state.reg.ic += 1;
}

/* FAB - Floating point add, Base Relative*/
void interpret_FAB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 0;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    int32_t E_res;
    if (M_A == 0) { M_A = M_B; E_res = E_B; }
    else if (M_B == 0) { E_res = E_A; }
    else {
        int diff = E_A - E_B;
        if (diff > 0) { if (diff > 24) M_B = 0; else M_B >>= diff; E_res = E_A; }
        else if (diff < 0) { if (-diff > 24) M_A = 0; else M_A >>= -diff; E_res = E_B; }
        else { E_res = E_A; }
    }
    pack_float32(cpu_ctx, RA, M_A + M_B, E_res);
    cpu_ctx->state.total_cycles += CLK_CYC_FAB;
    cpu_ctx->state.reg.ic += 1;
}

/* FSB - Floating point subtract, Base Relative*/
void interpret_FSB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 0;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    M_B = -M_B;
    int32_t E_res;
    if (M_A == 0) { M_A = M_B; E_res = E_B; }
    else if (M_B == 0) { E_res = E_A; }
    else {
        int diff = E_A - E_B;
        if (diff > 0) { if (diff > 24) M_B = 0; else M_B >>= diff; E_res = E_A; }
        else if (diff < 0) { if (-diff > 24) M_A = 0; else M_A >>= -diff; E_res = E_B; }
        else { E_res = E_A; }
    }
    pack_float32(cpu_ctx, RA, M_A + M_B, E_res);
    cpu_ctx->state.total_cycles += CLK_CYC_FSB;
    cpu_ctx->state.reg.ic += 1;
}

/* FMB - Floating point multiply, Base Relative*/
void interpret_FMB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 0;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    int64_t P = (int64_t)M_A * (int64_t)M_B;
    pack_float32(cpu_ctx, RA, (int32_t)(P >> 23), E_A + E_B);
    cpu_ctx->state.total_cycles += CLK_CYC_FMB;
    cpu_ctx->state.reg.ic += 1;
}

/* FDB - Floating point divide, Base Relative*/
void interpret_FDB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 0;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    if (M_B == 0) { cpu_ctx->state.reg.pir |= INTR_FLTOFL; return; }
    uint64_t M_A_abs = (M_A < 0) ? -M_A : M_A;
    uint64_t M_B_abs = (M_B < 0) ? -M_B : M_B;
    uint64_t M_A_abs_shift = (M_A_abs << 23);
    uint64_t Q_abs = M_A_abs_shift / M_B_abs;
    int32_t RemNotZero = (M_A_abs_shift % M_B_abs != 0)?1:0;
    int32_t Q = ((M_A ^ M_B) < 0) ? -(int32_t)(Q_abs + RemNotZero) : (int32_t)Q_abs;
    pack_float32(cpu_ctx, RA, Q, E_A - E_B);
    cpu_ctx->state.total_cycles += CLK_CYC_FDB;
    cpu_ctx->state.reg.ic += 1;
}

/* ORB - Inclusive logical OR, Base Relative*/
void interpret_ORB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = a | b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_ORB;
    cpu_ctx->state.reg.ic += 1;
}

/* ANDB - Logical AND, Base Relative*/
void interpret_ANDB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = a & b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_ANDB;
    cpu_ctx->state.reg.ic += 1;
}

/* CB - Single precision compare, Base Relative*/
void interpret_CB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    if (A == B) cpu_ctx->state.reg.sw |= CS_ZERO;
    else if (A > B) cpu_ctx->state.reg.sw |= CS_POSITIVE;
    else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    cpu_ctx->state.total_cycles += CLK_CYC_CB;
    cpu_ctx->state.reg.ic += 1;
}

/* FCB - Floating point compare, Base Relative*/
void interpret_FCB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 0;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t DISPLACEMENT = opcode & 0x00FF;
    uint16_t DO_ADDR = DISPLACEMENT + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 3 and addressing mode 8 */
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[RA], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    if (M_A == 0 && M_B == 0) {
        cpu_ctx->state.reg.sw |= CS_ZERO;
    } else {
        int diff = E_A - E_B;
        int64_t m_a_ext = M_A;
        int64_t m_b_ext = M_B;
        if (diff > 0) { if (diff > 24) m_b_ext = 0; else m_b_ext >>= diff; }
        else if (diff < 0) { if (-diff > 24) m_a_ext = 0; else m_a_ext >>= -diff; }
        int64_t res = m_a_ext - m_b_ext;
        if (res == 0) cpu_ctx->state.reg.sw |= CS_ZERO;
        else if (res > 0) cpu_ctx->state.reg.sw |= CS_POSITIVE;
        else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_FCB;
    cpu_ctx->state.reg.ic += 1;
}

/* LBX - Single precision load, Base Relative Indexed*/
static inline void interpret_LBX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    cpu_ctx->state.reg.r[(RA+0)&0xF] = DO[0];
    calculate_flags_16bit(cpu_ctx, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_LBX;
    cpu_ctx->state.reg.ic += 1;
}

/* DLBX - Double precision load, Base Relative Indexed*/
static inline void interpret_DLBX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 0;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    cpu_ctx->state.reg.r[(RA+0)&0xF] = DO[0];
    cpu_ctx->state.reg.r[(RA+1)&0xF] = DO[1];
    calculate_flags_32bit(cpu_ctx, DO);
    cpu_ctx->state.total_cycles += CLK_CYC_DLBX;
    cpu_ctx->state.reg.ic += 1;
}

/* STBX - Single precision store, Base Relative Indexed*/
static inline void interpret_STBX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    ok = store_data_word(cpu_ctx, DO_ADDR, cpu_ctx->state.reg.r[RA]);
    cpu_ctx->state.total_cycles += CLK_CYC_STBX;
    cpu_ctx->state.reg.ic += 1;
}

/* DSTX - Double precision store, Base Relative Indexed*/
static inline void interpret_DSTX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 0;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    ok = 0 == store_data_words_reg(cpu_ctx, DO_ADDR, 2, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_DSTX;
    cpu_ctx->state.reg.ic += 1;
}

/* ABX - Single precision integer add, Base Relative Indexed*/
static inline void interpret_ABX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int32_t res = (int32_t)a + (int32_t)b;
    bool carry = ((uint32_t)(uint16_t)a + (uint32_t)(uint16_t)b) > 0xFFFF;
    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    calculate_flags_16bit(cpu_ctx, (int16_t)res);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_ABX;
    cpu_ctx->state.reg.ic += 1;
}

/* SBBX - Single precision integer subtract, Base Relative Indexed*/
static inline void interpret_SBBX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int32_t res = (int32_t)a - (int32_t)b;
    bool carry = (uint16_t)a >= (uint16_t)b;
    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    calculate_flags_16bit(cpu_ctx, (int16_t)res);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_SBBX;
    cpu_ctx->state.reg.ic += 1;
}

/* MBX - Single precision integer multiply with 32-bit product, Base Relative Indexed*/
static inline void interpret_MBX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    int32_t prod = (int32_t)A * (int32_t)B;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)((prod >> 16) & 0xFFFF);
    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(prod & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_MBX;
    cpu_ctx->state.reg.ic += 1;
}

/* DBX - Single precision integer divide with 32-bit dividend, 16-bit divisor, Base Relative Indexed*/
static inline void interpret_DBX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int32_t A = (int32_t)(((uint32_t)(uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF]);
    int16_t B = (int16_t)DO[0];
    if (B == 0) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    } else {
        int32_t Q = A / B;
        int16_t Rem = A % B;
        if (Q > 32767 || Q < -32768) {
            cpu_ctx->state.reg.pir |= INTR_FIXOFL;
        } else {
            cpu_ctx->state.reg.r[(RA+0)&0xF] = Q;
            cpu_ctx->state.reg.r[(RA+1)&0xF] = Rem;
            calculate_flags_16bit(cpu_ctx, (int16_t)Q);
        }
    }
    cpu_ctx->state.total_cycles += CLK_CYC_DBX;
    cpu_ctx->state.reg.ic += 1;
}

/* FABX - Floating Point Add, Base Relative Indexed*/
static inline void interpret_FABX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 0;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    int32_t E_res;
    if (M_A == 0) { M_A = M_B; E_res = E_B; }
    else if (M_B == 0) { E_res = E_A; }
    else {
        int diff = E_A - E_B;
        if (diff > 0) { if (diff > 24) M_B = 0; else M_B >>= diff; E_res = E_A; }
        else if (diff < 0) { if (-diff > 24) M_A = 0; else M_A >>= -diff; E_res = E_B; }
        else { E_res = E_A; }
    }
    pack_float32(cpu_ctx, RA, M_A + M_B, E_res);
    cpu_ctx->state.total_cycles += CLK_CYC_FABX;
    cpu_ctx->state.reg.ic += 1;
}

/* FSBX - Floating Point Subtract, Base Relative Indexed*/
static inline void interpret_FSBX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 0;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    M_B = -M_B;
    int32_t E_res;
    if (M_A == 0) { M_A = M_B; E_res = E_B; }
    else if (M_B == 0) { E_res = E_A; }
    else {
        int diff = E_A - E_B;
        if (diff > 0) { if (diff > 24) M_B = 0; else M_B >>= diff; E_res = E_A; }
        else if (diff < 0) { if (-diff > 24) M_A = 0; else M_A >>= -diff; E_res = E_B; }
        else { E_res = E_A; }
    }
    pack_float32(cpu_ctx, RA, M_A + M_B, E_res);
    cpu_ctx->state.total_cycles += CLK_CYC_FSBX;
    cpu_ctx->state.reg.ic += 1;
}

/* FMBX - Floating Point Multiply, Base Relative Indexed*/
static inline void interpret_FMBX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 0;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    int64_t P = (int64_t)M_A * (int64_t)M_B;
    pack_float32(cpu_ctx, RA, (int32_t)(P >> 23), E_A + E_B);
    cpu_ctx->state.total_cycles += CLK_CYC_FMBX;
    cpu_ctx->state.reg.ic += 1;
}

/* FDBX - Floating Point Divide, Base Relative Indexed*/
static inline void interpret_FDBX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 0;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    if (M_B == 0) { cpu_ctx->state.reg.pir |= INTR_FLTOFL; return; }
    uint64_t M_A_abs = (M_A < 0) ? -M_A : M_A;
    uint64_t M_B_abs = (M_B < 0) ? -M_B : M_B;
    uint64_t M_A_abs_shift = (M_A_abs << 23);
    uint64_t Q_abs = M_A_abs_shift / M_B_abs;
    int32_t RemNotZero = (M_A_abs_shift % M_B_abs != 0)?1:0;
    int32_t Q = ((M_A ^ M_B) < 0) ? -(int32_t)(Q_abs + RemNotZero) : (int32_t)Q_abs;
    pack_float32(cpu_ctx, RA, Q, E_A - E_B);
    cpu_ctx->state.total_cycles += CLK_CYC_FDBX;
    cpu_ctx->state.reg.ic += 1;
}

/* CBX - Single precision Compare, Base Relative Indexed*/
static inline void interpret_CBX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    if (A == B) cpu_ctx->state.reg.sw |= CS_ZERO;
    else if (A > B) cpu_ctx->state.reg.sw |= CS_POSITIVE;
    else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    cpu_ctx->state.total_cycles += CLK_CYC_CBX;
    cpu_ctx->state.reg.ic += 1;
}

/* FCBX - Floating Point Compare, Base Relative Indexed*/
static inline void interpret_FCBX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 0;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[RA], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    if (M_A == 0 && M_B == 0) {
        cpu_ctx->state.reg.sw |= CS_ZERO;
    } else {
        int diff = E_A - E_B;
        int64_t m_a_ext = M_A;
        int64_t m_b_ext = M_B;
        if (diff > 0) { if (diff > 24) m_b_ext = 0; else m_b_ext >>= diff; }
        else if (diff < 0) { if (-diff > 24) m_a_ext = 0; else m_a_ext >>= -diff; }
        int64_t res = m_a_ext - m_b_ext;
        if (res == 0) cpu_ctx->state.reg.sw |= CS_ZERO;
        else if (res > 0) cpu_ctx->state.reg.sw |= CS_POSITIVE;
        else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_FCBX;
    cpu_ctx->state.reg.ic += 1;
}

/* ANDX - Logical AND, Base Relative Indexed*/
static inline void interpret_ANDX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = a & b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_ANDX;
    cpu_ctx->state.reg.ic += 1;
}

/* ORBX - Logical OR, Base Relative Indexed*/
static inline void interpret_ORBX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    const uint16_t RA = 2;
    uint16_t BR = ((opcode & 0x0300)>>8)+12;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)cpu_ctx->state.reg.r[BR];
    /* TODO: Implement logic for instruction format 4 and addressing mode 9 */
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = a | b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_ORBX;
    cpu_ctx->state.reg.ic += 1;
}

/* XIO - Execute input/output*/
void interpret_XIO(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t ps = (cpu_ctx->state.reg.sw & 0x00F0) >> 4;
    if (ps != 0){
        cpu_ctx->state.reg.pir |= INTR_MACHERR;
        cpu_ctx->state.reg.ft |= FT_PRIV_INSTR;
        cpu_ctx->state.reg.ic += 2;
    }
    else {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = imm_value + ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0);
    process_xio (cpu_ctx, DO[0], (ushort *) &cpu_ctx->state.reg.r[RA]);
    cpu_ctx->state.total_cycles += CLK_CYC_XIO;
    }
    cpu_ctx->state.reg.ic += 2;
}

/* VIO - Vectored input/output*/
void interpret_VIO(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t ps = (cpu_ctx->state.reg.sw & 0x00F0) >> 4;
    if (ps != 0){
        cpu_ctx->state.reg.pir |= INTR_MACHERR;
        cpu_ctx->state.reg.ft |= FT_PRIV_INSTR;
        cpu_ctx->state.reg.ic += 2;
    }
    else {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    struct {
        uint16_t io_cmd;
        uint16_t vector_select;
    } vio;
    fetch_data_words(cpu_ctx, DO_ADDR, 2, (int16_t *)&vio);
    uint16_t vector = vio.vector_select;
    uint16_t cmd_inc = cpu_ctx->state.reg.r[RA];
    uint16_t current_cmd = vio.io_cmd;
    int n = 0;
    for (int i = 0; i < 16; i++) {
        if (vector & (0x8000 >> i)) {
            ushort* ptr = get_address_data(cpu_ctx, DO_ADDR+2+n);
            if (ptr == NULL) { break; }
            process_xio(cpu_ctx, current_cmd, ptr);
            current_cmd += cmd_inc;
            n++;
        }
    }
    cpu_ctx->state.total_cycles += CLK_CYC_VIO(n);
    }
    cpu_ctx->state.reg.ic += 2;
}

/* AIM - Single precision integer add, Immediate*/
static inline void interpret_AIM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = imm_value;
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int32_t res = (int32_t)a + (int32_t)b;
    bool carry = ((uint32_t)(uint16_t)a + (uint32_t)(uint16_t)b) > 0xFFFF;
    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    calculate_flags_16bit(cpu_ctx, (int16_t)res);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_AIM;
    cpu_ctx->state.reg.ic += 2;
}

/* SIM - Single precision integer subtract, Immediate*/
static inline void interpret_SIM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = imm_value;
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int32_t res = (int32_t)a - (int32_t)b;
    bool carry = (uint16_t)a >= (uint16_t)b;
    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    calculate_flags_16bit(cpu_ctx, (int16_t)res);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_SIM;
    cpu_ctx->state.reg.ic += 2;
}

/* MIM - Single precision integer multiply with 32-bit product, Immediate*/
static inline void interpret_MIM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = imm_value;
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    int32_t prod = (int32_t)A * (int32_t)B;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)((prod >> 16) & 0xFFFF);
    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(prod & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_MIM;
    cpu_ctx->state.reg.ic += 2;
}

/* MSIM - Single precision integer multiply with 16-bit product, Immediate*/
static inline void interpret_MSIM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = imm_value;
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    int32_t prod = (int32_t)A * (int32_t)B;
    if (prod > 32767 || prod < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(prod & 0xFFFF);
    calculate_flags_16bit(cpu_ctx, (int16_t)prod);
    cpu_ctx->state.total_cycles += CLK_CYC_MSIM;
    cpu_ctx->state.reg.ic += 2;
}

/* DIM - Single precision integer divide with 32-bit dividend, Immediate*/
static inline void interpret_DIM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = imm_value;
    int32_t A = (int32_t)(((uint32_t)(uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF]);
    int16_t B = (int16_t)DO[0];
    if (B == 0) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    } else {
        int32_t Q = A / B;
        int16_t Rem = A % B;
        if (Q > 32767 || Q < -32768) {
            cpu_ctx->state.reg.pir |= INTR_FIXOFL;
        } else {
            cpu_ctx->state.reg.r[(RA+0)&0xF] = Q;
            cpu_ctx->state.reg.r[(RA+1)&0xF] = Rem;
            calculate_flags_16bit(cpu_ctx, (int16_t)Q);
        }
    }
    cpu_ctx->state.total_cycles += CLK_CYC_DIM;
    cpu_ctx->state.reg.ic += 2;
}

/* DVIM - Single precision integer divide with 16-bit dividend, Immediate*/
static inline void interpret_DVIM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = imm_value;
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    if (B == 0) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    } else if (A == (int16_t)0x8000 && B == -1) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
        cpu_ctx->state.reg.r[(RA+0)&0xF] = 0x8000; /* Q */
        cpu_ctx->state.reg.r[(RA+1)&0xF] = 0;      /* Rem */
        calculate_flags_16bit(cpu_ctx, 0x8000);
    } else {
        int16_t Q = A / B;
        int16_t Rem = A % B;
        cpu_ctx->state.reg.r[(RA+0)&0xF] = Q;
        cpu_ctx->state.reg.r[(RA+1)&0xF] = Rem;
        calculate_flags_16bit(cpu_ctx, Q);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_DVIM;
    cpu_ctx->state.reg.ic += 2;
}

/* ANDM - Logical AND, Immediate*/
static inline void interpret_ANDM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = imm_value;
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = a & b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_ANDM;
    cpu_ctx->state.reg.ic += 2;
}

/* ORIM - Inclusive logical OR, Immediate*/
static inline void interpret_ORIM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = imm_value;
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = a | b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_ORIM;
    cpu_ctx->state.reg.ic += 2;
}

/* XORM - Exclusive logical OR, Immediate*/
static inline void interpret_XORM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = imm_value;
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = a ^ b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_XORM;
    cpu_ctx->state.reg.ic += 2;
}

/* CIM - Single precision compare, Immediate*/
static inline void interpret_CIM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = imm_value;
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    if (A == B) cpu_ctx->state.reg.sw |= CS_ZERO;
    else if (A > B) cpu_ctx->state.reg.sw |= CS_POSITIVE;
    else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    cpu_ctx->state.total_cycles += CLK_CYC_CIM;
    cpu_ctx->state.reg.ic += 2;
}

/* NIM - Logical NAND, Immediate*/
static inline void interpret_NIM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = imm_value;
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = ~(a & b);
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_NIM;
    cpu_ctx->state.reg.ic += 2;
}

/* ESQR - Extended precision floating point square root*/
void interpret_ESQR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int64_t M; int16_t E;
    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M, &E);
    if (M < 0) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    } else if (M == 0) {
        pack_float48(cpu_ctx, RA, 0, 0);
    } else {
        int adj_E = E;
        /* Left-align the 40-bit mantissa to the top of the 64-bit word (bit 62) */
        uint64_t x = (uint64_t)M << 24;
        if (adj_E & 1) {
            x <<= 1;
            adj_E -= 1;
        }
        /* 40 iterations generates a 40-bit root */
        uint64_t A = sqrt_n_bits(x, 40);
        pack_float48(cpu_ctx, RA, (int64_t)A, (adj_E / 2) - 1);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_ESQR;
    cpu_ctx->state.reg.ic += 1;
}

/* SQRT - Floating point square root*/
void interpret_SQRT(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int32_t M; int16_t E;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M, &E);
    if (M < 0) {
        /* Square root of a negative number triggers a Fixed Point Overflow trap */
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    } else if (M == 0) {
        pack_float32(cpu_ctx, RA, 0, 0);
    } else {
        int adj_E = E;
        /* Left-align the 24-bit mantissa to the top of the 64-bit word (bit 62) */
        uint64_t x = (uint64_t)M << 40;
        /* If the exponent is odd, multiply mantissa by 2 (shift to bit 63) to even it out */
        if (adj_E & 1) {
            x <<= 1;
            adj_E -= 1;
        }
        /* 24 iterations generates a 24-bit root (providing 1 guard bit for the normalizer) */
        uint64_t A = sqrt_n_bits(x, 24);
        /* Fixed scaling logic: mathematically equivalent to adj_E/2 - 1 */
        pack_float32(cpu_ctx, RA, (int32_t)A, (adj_E / 2) - 1);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_SQRT;
    cpu_ctx->state.reg.ic += 1;
}

/* BIF - Built-In-Function*/
void interpret_BIF(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    /* TODO: Implement logic for instruction format 6 and addressing mode 10 */
    BIF_logic(cpu_ctx, opcode, 0);
    cpu_ctx->state.total_cycles += CLK_CYC_BIF;
    cpu_ctx->state.reg.ic += 1;
}

/* SB - Set bit*/
void interpret_SB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    DO[0] |= 1 << (15 - N);
    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_SB;
    cpu_ctx->state.reg.ic += 2;
}

/* SBR - Set bit, Register*/
void interpret_SBR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    cpu_ctx->state.reg.r[RB] |= 1 << (15 - N);
    cpu_ctx->state.total_cycles += CLK_CYC_SBR;
    cpu_ctx->state.reg.ic += 1;
}

/* SBI - Set bit, Indirect*/
void interpret_SBI(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t DO_ADDR;
    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    DO[0] |= 1 << (15 - N);
    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_SBI;
    cpu_ctx->state.reg.ic += 2;
}

/* RB - Reset bit*/
void interpret_RB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    DO[0] &= ~(1 << (15 - N));
    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_RB;
    cpu_ctx->state.reg.ic += 2;
}

/* RBR - Reset bit, Register*/
void interpret_RBR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    cpu_ctx->state.reg.r[RB] &= ~(1 << (15 - N));
    cpu_ctx->state.total_cycles += CLK_CYC_RBR;
    cpu_ctx->state.reg.ic += 1;
}

/* RBI - Reset bit, Indirect*/
void interpret_RBI(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t DO_ADDR;
    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    DO[0] &= ~(1 << (15 - N));
    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_RBI;
    cpu_ctx->state.reg.ic += 2;
}

/* TB - Test bit*/
void interpret_TB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    bool is_on = DO[0] & (1 << (15 - N));
    cpu_ctx->state.reg.sw &= 0x0FFF;
    if (is_on) {
        cpu_ctx->state.reg.sw |= (N>0)?CS_POSITIVE:CS_NEGATIVE; // N is the bit number (0 is msb where the sign bit is), not the value
    }
    else {
        cpu_ctx->state.reg.sw |= CS_ZERO;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_TB;
    cpu_ctx->state.reg.ic += 2;
}

/* TBR - Test bit, Register*/
void interpret_TBR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    bool is_on = cpu_ctx->state.reg.r[RB] & (1 << (15 - N));
    cpu_ctx->state.reg.sw &= 0x0FFF;
    if (is_on) {
        cpu_ctx->state.reg.sw |= (N>0)?CS_POSITIVE:CS_NEGATIVE; // N is the bit number (0 is msb where the sign bit is), not the value
    }
    else {
        cpu_ctx->state.reg.sw |= CS_ZERO;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_TBR;
    cpu_ctx->state.reg.ic += 1;
}

/* TBI - Test bit, Indirect*/
void interpret_TBI(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t DO_ADDR;
    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    bool is_on = DO[0] & (1 << (15 - N));
    cpu_ctx->state.reg.sw &= 0x0FFF;
    if (is_on) {
        cpu_ctx->state.reg.sw |= (N>0)?CS_POSITIVE:CS_NEGATIVE; // N is the bit number (0 is msb where the sign bit is), not the value
    }
    else {
        cpu_ctx->state.reg.sw |= CS_ZERO;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_TBI;
    cpu_ctx->state.reg.ic += 2;
}

/* TSB - Test and set bit*/
void interpret_TSB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    bool is_on = DO[0] & (1 << (15 - N));
    DO[0] |= 1 << (15 - N);
    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);
    cpu_ctx->state.reg.sw &= 0x0FFF;
    if (is_on) {
        cpu_ctx->state.reg.sw |= (N>0)?CS_POSITIVE:CS_NEGATIVE; // N is the bit number (0 is msb where the sign bit is), not the value
    }
    else {
        cpu_ctx->state.reg.sw |= CS_ZERO;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_TSB;
    cpu_ctx->state.reg.ic += 2;
}

/* SVBR - Set variable bit in register*/
void interpret_SVBR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    uint16_t N = cpu_ctx->state.reg.r[RA] & 0x000F;
    cpu_ctx->state.reg.r[RB] |= 1 << (15 - N);
    cpu_ctx->state.total_cycles += CLK_CYC_SVBR;
    cpu_ctx->state.reg.ic += 1;
}

/* RVBR - Reset variable bit in register*/
void interpret_RVBR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    uint16_t N = cpu_ctx->state.reg.r[RA] & 0x000F;
    cpu_ctx->state.reg.r[RB] &= ~(1 << (15 - N));
    cpu_ctx->state.total_cycles += CLK_CYC_RVBR;
    cpu_ctx->state.reg.ic += 1;
}

/* TVBR - Test variable bit in register*/
void interpret_TVBR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    uint16_t N = cpu_ctx->state.reg.r[RA] & 0x000F;
    bool is_on = cpu_ctx->state.reg.r[RB] & (1 << (15 - N));
    cpu_ctx->state.reg.sw &= 0x0FFF;
    if (is_on) {
        cpu_ctx->state.reg.sw |= (N>0)?CS_POSITIVE:CS_NEGATIVE; // N is the bit number (0 is msb where the sign bit is), not the value
    }
    else {
        cpu_ctx->state.reg.sw |= CS_ZERO;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_TVBR;
    cpu_ctx->state.reg.ic += 1;
}

/* SLL - Shift left logical*/
void interpret_SLL(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RB = opcode & 0x000F;
    int16_t shift = ((opcode & 0x00F0) >> 4) + 1;
    uint16_t dest_reg = RB;
    int16_t val = cpu_ctx->state.reg.r[dest_reg];
    if (shift < 16) val = (uint16_t)val << shift; else val = 0;
    cpu_ctx->state.reg.r[dest_reg] = val;
    calculate_flags_16bit(cpu_ctx, val);
    cpu_ctx->state.reg.ic += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_SLL(shift);
}

/* SRL - Shift right logical*/
void interpret_SRL(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RB = opcode & 0x000F;
    int16_t shift = ((opcode & 0x00F0) >> 4) + 1;
    uint16_t dest_reg = RB;
    int16_t val = cpu_ctx->state.reg.r[dest_reg];
    if (shift < 16) val = (int16_t)((uint16_t)val >> shift); else val = 0;
    cpu_ctx->state.reg.r[dest_reg] = val;
    calculate_flags_16bit(cpu_ctx, val);
    cpu_ctx->state.reg.ic += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_SRL(shift);
}

/* SRA - Shift right arithmetic*/
void interpret_SRA(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RB = opcode & 0x000F;
    int16_t shift = ((opcode & 0x00F0) >> 4) + 1;
    uint16_t dest_reg = RB;
    int16_t val = cpu_ctx->state.reg.r[dest_reg];
    if (shift < 16) val >>= shift; else if (val < 0) val = ~0; else val = 0;
    cpu_ctx->state.reg.r[dest_reg] = val;
    calculate_flags_16bit(cpu_ctx, val);
    cpu_ctx->state.reg.ic += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_SRA(shift);
}

/* SLC - Shift left cyclic*/
void interpret_SLC(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RB = opcode & 0x000F;
    int16_t shift = ((opcode & 0x00F0) >> 4) + 1;
    uint16_t dest_reg = RB;
    int16_t val = cpu_ctx->state.reg.r[dest_reg];
    if (shift < 16) val = (int16_t)(((uint16_t)val << shift) | ((uint16_t)val >> (16 - shift)));
    cpu_ctx->state.reg.r[dest_reg] = val;
    calculate_flags_16bit(cpu_ctx, val);
    cpu_ctx->state.reg.ic += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_SLC(shift);
}

/* DSLL - Double shift left logical*/
void interpret_DSLL(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RB = opcode & 0x000F;
    int16_t shift = ((opcode & 0x00F0) >> 4) + 1;
    uint16_t dest_reg = RB;
    int32_t val = ((uint32_t)cpu_ctx->state.reg.r[dest_reg] << 16) | (uint16_t)cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF];
    val <<= shift;
    cpu_ctx->state.reg.r[dest_reg] = (uint16_t)(val >> 16);
    cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF] = (uint16_t)(val & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, dest_reg);
    cpu_ctx->state.reg.ic += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_DSLL(shift);
}

/* DSRL - Double shift right logical*/
void interpret_DSRL(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RB = opcode & 0x000F;
    int16_t shift = ((opcode & 0x00F0) >> 4) + 1;
    uint16_t dest_reg = RB;
    int32_t val = ((uint32_t)cpu_ctx->state.reg.r[dest_reg] << 16) | (uint16_t)cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF];
    val = (int32_t)((uint32_t)val >> shift);
    cpu_ctx->state.reg.r[dest_reg] = (uint16_t)(val >> 16);
    cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF] = (uint16_t)(val & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, dest_reg);
    cpu_ctx->state.reg.ic += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_DSRL(shift);
}

/* DSRA - Double shift right arithmetic*/
void interpret_DSRA(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RB = opcode & 0x000F;
    int16_t shift = ((opcode & 0x00F0) >> 4) + 1;
    uint16_t dest_reg = RB;
    int32_t val = ((uint32_t)cpu_ctx->state.reg.r[dest_reg] << 16) | (uint16_t)cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF];
    val >>= shift;
    cpu_ctx->state.reg.r[dest_reg] = (uint16_t)(val >> 16);
    cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF] = (uint16_t)(val & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, dest_reg);
    cpu_ctx->state.reg.ic += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_DSRA(shift);
}

/* DSLC - Double shift left cyclic*/
void interpret_DSLC(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RB = opcode & 0x000F;
    int16_t shift = ((opcode & 0x00F0) >> 4) + 1;
    uint16_t dest_reg = RB;
    int32_t val = ((uint32_t)cpu_ctx->state.reg.r[dest_reg] << 16) | (uint16_t)cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF];
    val = (int32_t)(((uint32_t)val << shift) | ((uint32_t)val >> (32 - shift)));
    cpu_ctx->state.reg.r[dest_reg] = (uint16_t)(val >> 16);
    cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF] = (uint16_t)(val & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, dest_reg);
    cpu_ctx->state.reg.ic += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_DSLC(shift);
}

/* SLR - Shift logical, count in register*/
void interpret_SLR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RB = opcode & 0x000F;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t shift = cpu_ctx->state.reg.r[RB];
    uint16_t dest_reg = RA;
    if (shift < -16 || shift > 16)
    {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    return;
    }
    int16_t val = cpu_ctx->state.reg.r[dest_reg];
    if (shift > 0) { if (shift < 16) val = (uint16_t)val << shift; else val = 0; }
    else if (shift < 0) {
        if (shift > -16) val = (int16_t)((uint16_t)val >> (-shift)); else val = 0;
    }
    cpu_ctx->state.reg.r[dest_reg] = val;
    calculate_flags_16bit(cpu_ctx, val);
    cpu_ctx->state.reg.ic += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_SLR(shift);
}

/* SAR - Shift arithmetic, count in register*/
void interpret_SAR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RB = opcode & 0x000F;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t shift = cpu_ctx->state.reg.r[RB];
    uint16_t dest_reg = RA;
    if (shift < -16 || shift > 16)
    {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    return;
    }
    int16_t val = cpu_ctx->state.reg.r[dest_reg];
    int32_t overflow_test = val;
    if (shift > 0) overflow_test = (uint32_t)overflow_test << shift;
    if (shift > 0) { if (shift < 16) val = (uint16_t)val << shift; else val = 0; }
    else if (shift < 0) {
        if (shift > -16) val >>= (-shift); else if (val < 0) val = ~0; else val = 0;
    }
    if (overflow_test > 32767 || overflow_test < -32768)  cpu_ctx->state.reg.pir |= INTR_FIXOFL;    cpu_ctx->state.reg.r[dest_reg] = val;
    calculate_flags_16bit(cpu_ctx, val);
    cpu_ctx->state.reg.ic += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_SAR(shift);
}

/* SCR - Shift cyclic, count in register*/
void interpret_SCR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RB = opcode & 0x000F;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t shift = cpu_ctx->state.reg.r[RB];
    uint16_t dest_reg = RA;
    if (shift < -16 || shift > 16)
    {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    return;
    }
    int16_t val = cpu_ctx->state.reg.r[dest_reg];
    if (shift > 0) {
        if (shift < 16) val = (int16_t)(((uint16_t)val << shift) | ((uint16_t)val >> (16 - shift)));
    } else if (shift < 0) {
        int16_t s = -shift;
        if (s < 16) val = (int16_t)(((uint16_t)val >> s) | ((uint16_t)val << (16 - s)));
    }
    cpu_ctx->state.reg.r[dest_reg] = val;
    calculate_flags_16bit(cpu_ctx, val);
    cpu_ctx->state.reg.ic += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_SCR(shift);
}

/* DSLR - Double shift logical, count in register*/
void interpret_DSLR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RB = opcode & 0x000F;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t shift = cpu_ctx->state.reg.r[RB];
    uint16_t dest_reg = RA;
    if (shift < -32 || shift > 32)
    {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    return;
    }
    int32_t val = ((uint32_t)cpu_ctx->state.reg.r[dest_reg] << 16) | (uint16_t)cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF];
    if (shift > 0) { if (shift < 32) val = (uint32_t)val << shift; else val = 0;}
    else if (shift < 0) {
        if (shift > -32) val = (int32_t)((uint32_t)val >> (-shift)); else val = 0;
    }
    cpu_ctx->state.reg.r[dest_reg] = (uint16_t)(val >> 16);
    cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF] = (uint16_t)(val & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, dest_reg);
    cpu_ctx->state.reg.ic += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_DSLR(shift);
}

/* DSAR - Double shift arithmetic, count in register*/
void interpret_DSAR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RB = opcode & 0x000F;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t shift = cpu_ctx->state.reg.r[RB];
    uint16_t dest_reg = RA;
    if (shift < -32 || shift > 32)
    {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    return;
    }
    int32_t val = ((uint32_t)cpu_ctx->state.reg.r[dest_reg] << 16) | (uint16_t)cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF];
    int64_t overflow_test = val;
    if (shift > 0) overflow_test = (uint64_t)overflow_test << shift;
    if (shift > 0) { if (shift < 32) val = (uint32_t)val << shift; else val = 0;}
    else if (shift < 0) {
        if (shift > -32) val >>= (-shift); else if (val < 0) val = ~0; else val = 0;
    }
    if (overflow_test < -2147483648LL || overflow_test > 2147483647LL)  cpu_ctx->state.reg.pir |= INTR_FIXOFL;    cpu_ctx->state.reg.r[dest_reg] = (uint16_t)(val >> 16);
    cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF] = (uint16_t)(val & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, dest_reg);
    cpu_ctx->state.reg.ic += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_DSAR(shift);
}

/* DSCR - Double shift cyclic, count in register*/
void interpret_DSCR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RB = opcode & 0x000F;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t shift = cpu_ctx->state.reg.r[RB];
    uint16_t dest_reg = RA;
    if (shift < -32 || shift > 32)
    {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    return;
    }
    int32_t val = ((uint32_t)cpu_ctx->state.reg.r[dest_reg] << 16) | (uint16_t)cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF];
    if (shift > 0) {
        if (shift < 32) val = (int32_t)(((uint32_t)val << shift) | ((uint32_t)val >> (32 - shift)));
    } else if (shift < 0) {
        int16_t s = -shift;
        if (s < 32) val = (int32_t)(((uint32_t)val >> s) | ((uint32_t)val << (32 - s)));
    }
    cpu_ctx->state.reg.r[dest_reg] = (uint16_t)(val >> 16);
    cpu_ctx->state.reg.r[(dest_reg + 1) & 0xF] = (uint16_t)(val & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, dest_reg);
    cpu_ctx->state.reg.ic += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_DSCR(shift);
}

/* JC - Jump on condition*/
void interpret_JC(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t mask = N << 12; /* align it with cs in sw reg */
    bool jump_taken = ((mask & 0x7000) == 0x7000 || (cpu_ctx->state.reg.sw & mask) != 0);
    if (jump_taken){
        cpu_ctx->state.reg.ic = DO_ADDR;
    } else {
        cpu_ctx->state.reg.ic += 2;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_JC(jump_taken);
    cpu_ctx->state.reg.ic += 2;
}

/* JCI - Jump on condition, Indirect*/
void interpret_JCI(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t DO_ADDR;
    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);
    uint16_t mask = N << 12; /* align it with cs in sw reg */
    bool jump_taken = ((mask & 0x7000) == 0x7000 || (cpu_ctx->state.reg.sw & mask) != 0);
    if (jump_taken){
        cpu_ctx->state.reg.ic = DO_ADDR;
    } else {
        cpu_ctx->state.reg.ic += 2;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_JCI(jump_taken);
    cpu_ctx->state.reg.ic += 2;
}

/* JS - Jump to subroutine*/
void interpret_JS(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    cpu_ctx->state.reg.r[RA] = cpu_ctx->state.reg.ic + 2;
    cpu_ctx->state.reg.ic = DO_ADDR;
    cpu_ctx->state.total_cycles += CLK_CYC_JS;
}

/* SOJ - Subtract one and jump*/
void interpret_SOJ(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    uint16_t val = cpu_ctx->state.reg.r[RA] - 1;
    cpu_ctx->state.reg.r[RA] = val;
    calculate_flags_16bit(cpu_ctx, (int16_t)val);
    if (val != 0) {
        cpu_ctx->state.reg.ic = DO_ADDR;
    } else {
        cpu_ctx->state.reg.ic += 2;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_SOJ(val != 0);
    cpu_ctx->state.reg.ic += 2;
}

/* BR - Branch unconditionally*/
void interpret_BR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    /* TODO: Implement logic for instruction format 5 and addressing mode 7 */
    int8_t displacement = (int8_t)(opcode & 0x00FF);
    cpu_ctx->state.reg.ic += displacement;
    cpu_ctx->state.total_cycles += CLK_CYC_BR;
}

/* BEZ - Branch if equal to (zero)*/
void interpret_BEZ(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    /* TODO: Implement logic for instruction format 5 and addressing mode 7 */
    int8_t displacement = (int8_t)(opcode & 0x00FF);
    bool branch_taken = cpu_ctx->state.reg.sw & CS_ZERO;
    if (branch_taken) {
        cpu_ctx->state.reg.ic += displacement;
    }
    else {
        cpu_ctx->state.reg.ic += 1;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_BRcc(branch_taken);
}

/* BLT - Branch if less than (zero)*/
void interpret_BLT(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    /* TODO: Implement logic for instruction format 5 and addressing mode 7 */
    int8_t displacement = (int8_t)(opcode & 0x00FF);
    bool branch_taken = cpu_ctx->state.reg.sw & CS_NEGATIVE;
    if (branch_taken) {
        cpu_ctx->state.reg.ic += displacement;
    }
    else {
        cpu_ctx->state.reg.ic += 1;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_BRcc(branch_taken);
}

/* BEX - Branch to executive*/
void interpret_BEX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    cpu_ctx->state.bex_index = (opcode & 0xF);
    cpu_ctx->state.reg.pir |= INTR_BEX;
    cpu_ctx->state.total_cycles += CLK_CYC_BEX;
    cpu_ctx->state.reg.ic += 1;
}

/* BLE - Branch if less than or equal to (zero)*/
void interpret_BLE(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    /* TODO: Implement logic for instruction format 5 and addressing mode 7 */
    int8_t displacement = (int8_t)(opcode & 0x00FF);
    bool branch_taken = cpu_ctx->state.reg.sw & (CS_NEGATIVE | CS_ZERO);
    if (branch_taken) {
        cpu_ctx->state.reg.ic += displacement;
    }
    else {
        cpu_ctx->state.reg.ic += 1;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_BRcc(branch_taken);
}

/* BGT - Branch if greater than (zero)*/
void interpret_BGT(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    /* TODO: Implement logic for instruction format 5 and addressing mode 7 */
    int8_t displacement = (int8_t)(opcode & 0x00FF);
    bool branch_taken = cpu_ctx->state.reg.sw & CS_POSITIVE;
    if (branch_taken) {
        cpu_ctx->state.reg.ic += displacement;
    }
    else {
        cpu_ctx->state.reg.ic += 1;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_BRcc(branch_taken);
}

/* BNZ - Branch if not equal to (zero)*/
void interpret_BNZ(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    /* TODO: Implement logic for instruction format 5 and addressing mode 7 */
    int8_t displacement = (int8_t)(opcode & 0x00FF);
    bool branch_taken = !(cpu_ctx->state.reg.sw & CS_ZERO);
    if (branch_taken) {
        cpu_ctx->state.reg.ic += displacement;
    }
    else {
        cpu_ctx->state.reg.ic += 1;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_BRcc(branch_taken);
}

/* BGE - Branch if greater than or equal to (zero)*/
void interpret_BGE(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    /* TODO: Implement logic for instruction format 5 and addressing mode 7 */
    int8_t displacement = (int8_t)(opcode & 0x00FF);
    bool branch_taken = cpu_ctx->state.reg.sw & (CS_POSITIVE | CS_ZERO);
    if (branch_taken) {
        cpu_ctx->state.reg.ic += displacement;
    }
    else {
        cpu_ctx->state.reg.ic += 1;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_BRcc(branch_taken);
}

/* LSTI - Load status, Indirect*/
void interpret_LSTI(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t ps = (cpu_ctx->state.reg.sw & 0x00F0) >> 4;
    if (ps != 0){
        cpu_ctx->state.reg.pir |= INTR_MACHERR;
        cpu_ctx->state.reg.ft |= FT_PRIV_INSTR;
        cpu_ctx->state.reg.ic += 2;
    }
    else {
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t DO_ADDR;
    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);
    int16_t DO[3];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    if ((DO[1] ^ cpu_ctx->state.reg.sw) & 0xFF ) { invalidate_mem_cache(cpu_ctx); }
    cpu_ctx->state.reg.mk = DO[0];
    cpu_ctx->state.reg.sw = DO[1];
    cpu_ctx->state.reg.ic = DO[2];
    }
    cpu_ctx->state.total_cycles += CLK_CYC_LSTI;
}

/* LST - Load status*/
void interpret_LST(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t ps = (cpu_ctx->state.reg.sw & 0x00F0) >> 4;
    if (ps != 0){
        cpu_ctx->state.reg.pir |= INTR_MACHERR;
        cpu_ctx->state.reg.ft |= FT_PRIV_INSTR;
        cpu_ctx->state.reg.ic += 2;
    }
    else {
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[3];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    if ((DO[1] ^ cpu_ctx->state.reg.sw) & 0xFF ) { invalidate_mem_cache(cpu_ctx); }
    cpu_ctx->state.reg.mk = DO[0];
    cpu_ctx->state.reg.sw = DO[1];
    cpu_ctx->state.reg.ic = DO[2];
    }
    cpu_ctx->state.total_cycles += CLK_CYC_LST;
}

/* SJS - Stack IC and jump to subroutine*/
void interpret_SJS(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    cpu_ctx->state.reg.r[RA] -= 1;
    ok = store_data_word(cpu_ctx, cpu_ctx->state.reg.r[RA], cpu_ctx->state.reg.ic + 2);
    cpu_ctx->state.reg.ic = DO_ADDR;
    cpu_ctx->state.total_cycles += CLK_CYC_SJS;
}

/* URS - Unstack IC and return from subroutine*/
void interpret_URS(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    ok = fetch_data_word(cpu_ctx, cpu_ctx->state.reg.r[RA], &cpu_ctx->state.reg.ic);
    cpu_ctx->state.reg.r[RA] += 1;
    cpu_ctx->state.total_cycles += CLK_CYC_URS;
}

/* L - Single precision load*/
void interpret_L(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    cpu_ctx->state.reg.r[(RA+0)&0xF] = DO[0];
    calculate_flags_16bit(cpu_ctx, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_L;
    cpu_ctx->state.reg.ic += 2;
}

/* LR - Single precision load, Register*/
void interpret_LR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    cpu_ctx->state.reg.r[(RA+0)&0xF] = DO[0];
    calculate_flags_16bit(cpu_ctx, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_LR;
    cpu_ctx->state.reg.ic += 1;
}

/* LISP - Single precision load, Immediate Short Positive*/
void interpret_LISP(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = (opcode & 0x000F) + 1;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = DO[0];
    calculate_flags_16bit(cpu_ctx, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_LISP;
    cpu_ctx->state.reg.ic += 1;
}

/* LISN - Single precision load, Immediate Short Negative*/
void interpret_LISN(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = -((opcode & 0x000F) + 1);
    cpu_ctx->state.reg.r[(RA+0)&0xF] = DO[0];
    calculate_flags_16bit(cpu_ctx, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_LISN;
    cpu_ctx->state.reg.ic += 1;
}

/* LI - Single precision load, Indirect*/
void interpret_LI(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t DO_ADDR;
    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    cpu_ctx->state.reg.r[(RA+0)&0xF] = DO[0];
    calculate_flags_16bit(cpu_ctx, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_LI;
    cpu_ctx->state.reg.ic += 2;
}

/* LIM - Single precision load, Immediate*/
void interpret_LIM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = imm_value + ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0);
    cpu_ctx->state.reg.r[(RA+0)&0xF] = DO[0];
    calculate_flags_16bit(cpu_ctx, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_LIM;
    cpu_ctx->state.reg.ic += 2;
}

/* DL - Double precision load*/
void interpret_DL(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    cpu_ctx->state.reg.r[(RA+0)&0xF] = DO[0];
    cpu_ctx->state.reg.r[(RA+1)&0xF] = DO[1];
    calculate_flags_32bit(cpu_ctx, DO);
    cpu_ctx->state.total_cycles += CLK_CYC_DL;
    cpu_ctx->state.reg.ic += 2;
}

/* DLR - Double precision load, Register*/
void interpret_DLR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    cpu_ctx->state.reg.r[(RA+0)&0xF] = DO[0];
    cpu_ctx->state.reg.r[(RA+1)&0xF] = DO[1];
    calculate_flags_32bit(cpu_ctx, DO);
    cpu_ctx->state.total_cycles += CLK_CYC_DLR;
    cpu_ctx->state.reg.ic += 1;
}

/* DLI - Double precision load, Indirect*/
void interpret_DLI(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t DO_ADDR;
    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    cpu_ctx->state.reg.r[(RA+0)&0xF] = DO[0];
    cpu_ctx->state.reg.r[(RA+1)&0xF] = DO[1];
    calculate_flags_32bit(cpu_ctx, DO);
    cpu_ctx->state.total_cycles += CLK_CYC_DLI;
    cpu_ctx->state.reg.ic += 2;
}

/* LM - Load multiple registers*/
void interpret_LM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    ok = 0 == fetch_data_words_reg(cpu_ctx, DO_ADDR, N +1, 0 /* reg 0 */);
    cpu_ctx->state.total_cycles += CLK_CYC_LM(N +1);
    cpu_ctx->state.reg.ic += 2;
}

/* EFL - Extended precision floating point load*/
void interpret_EFL(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[3];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    cpu_ctx->state.reg.r[(RA+0)&0xF] = DO[0];
    cpu_ctx->state.reg.r[(RA+1)&0xF] = DO[1];
    cpu_ctx->state.reg.r[(RA+2)&0xF] = DO[2];
    calculate_flags_48bit(cpu_ctx, DO);
    cpu_ctx->state.total_cycles += CLK_CYC_EFL;
    cpu_ctx->state.reg.ic += 2;
}

/* LUB - Load from upper byte*/
void interpret_LUB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    cpu_ctx->state.reg.r[RA] = (((uint16_t)DO[0] & 0xFF00) >> 8) | (cpu_ctx->state.reg.r[RA] & 0xFF00);
    calculate_flags_16bit(cpu_ctx, cpu_ctx->state.reg.r[RA]);
    cpu_ctx->state.total_cycles += CLK_CYC_LUB;
    cpu_ctx->state.reg.ic += 2;
}

/* LLB - Load from lower byte*/
void interpret_LLB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    cpu_ctx->state.reg.r[RA] = (DO[0] & 0x00FF) | (cpu_ctx->state.reg.r[RA] & 0xFF00);
    calculate_flags_16bit(cpu_ctx, cpu_ctx->state.reg.r[RA]);
    cpu_ctx->state.total_cycles += CLK_CYC_LLB;
    cpu_ctx->state.reg.ic += 2;
}

/* LUBI - Load from upper byte, Indirect*/
void interpret_LUBI(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t DO_ADDR;
    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    cpu_ctx->state.reg.r[RA] = (((uint16_t)DO[0] & 0xFF00) >> 8) | (cpu_ctx->state.reg.r[RA] & 0xFF00);
    calculate_flags_16bit(cpu_ctx, cpu_ctx->state.reg.r[RA]);
    cpu_ctx->state.total_cycles += CLK_CYC_LUBI;
    cpu_ctx->state.reg.ic += 2;
}

/* LLBI - Load from lower byte, Indirect*/
void interpret_LLBI(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t DO_ADDR;
    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    cpu_ctx->state.reg.r[RA] = (DO[0] & 0x00FF) | (cpu_ctx->state.reg.r[RA] & 0xFF00);
    calculate_flags_16bit(cpu_ctx, cpu_ctx->state.reg.r[RA]);
    cpu_ctx->state.total_cycles += CLK_CYC_LLBI;
    cpu_ctx->state.reg.ic += 2;
}

/* POPM - Pop multiple registers off the stack*/
void interpret_POPM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 10 */
    int count = RB - RA + 1;
    if (count < 0)
        count += 16;
    int stk_addr = cpu_ctx->state.reg.r[15];
    ok = 0 == fetch_data_words_reg(cpu_ctx, stk_addr, count, RA);
    cpu_ctx->state.reg.r[15] = stk_addr + count;
    cpu_ctx->state.total_cycles += CLK_CYC_POPM(count);
    cpu_ctx->state.reg.ic += 1;
}

/* ST - Single precision store*/
void interpret_ST(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    ok = store_data_word(cpu_ctx, DO_ADDR, cpu_ctx->state.reg.r[RA]);
    cpu_ctx->state.total_cycles += CLK_CYC_ST;
    cpu_ctx->state.reg.ic += 2;
}

/* STC - Store a non-negative constant*/
void interpret_STC(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    ok = store_data_word(cpu_ctx, DO_ADDR, N);
    cpu_ctx->state.total_cycles += CLK_CYC_STC;
    cpu_ctx->state.reg.ic += 2;
}

/* STCI - Store a non-negative constant, Indirect*/
void interpret_STCI(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t DO_ADDR;
    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);
    ok = store_data_word(cpu_ctx, DO_ADDR, N);
    cpu_ctx->state.total_cycles += CLK_CYC_STCI;
    cpu_ctx->state.reg.ic += 2;
}

/* MOV - Move multiple words, memory-to-memory*/
void interpret_MOV(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 10 */
    uint16_t count = (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF]; /* count range is 0 - 2^16-1*/
    uint16_t total_written = 0;
    const int move_steps_1_tick = (TIMER_A_RES_IN_10uSEC*10000)/(CLK_CYC_MOV_STEP(1)*CYCLE_DURATION_IN_NS);
    const int move_steps = move_steps_1_tick == 0 ? 1 : move_steps_1_tick;
    cpu_ctx->state.total_cycles += CLK_CYC_MOV_INIT;
    while(count > 0){
        uint16_t partial_count = (count > move_steps)? move_steps: count;
        uint16_t remaining = move_words(cpu_ctx, cpu_ctx->state.reg.r[RB], cpu_ctx->state.reg.r[RA], partial_count);
        total_written += partial_count - remaining;
        count -= partial_count - remaining;
        cpu_ctx->state.total_cycles += CLK_CYC_MOV_STEP(partial_count - remaining);
        calculate_timers(cpu_ctx);
        if (has_pending_interrupt(cpu_ctx)) break;
    }
    cpu_ctx->state.reg.r[(RA+1)&0xF] = count;
    cpu_ctx->state.reg.r[RA] += total_written;
    cpu_ctx->state.reg.r[RB] += total_written;
    if (count == 0){
        cpu_ctx->state.total_cycles += CLK_CYC_MOV_FINI(total_written);
        cpu_ctx->state.reg.ic +=1;
    }
}

/* STI - Single precision store, Indirect*/
void interpret_STI(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t DO_ADDR;
    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);
    ok = store_data_word(cpu_ctx, DO_ADDR, cpu_ctx->state.reg.r[RA]);
    cpu_ctx->state.total_cycles += CLK_CYC_STI;
    cpu_ctx->state.reg.ic += 2;
}

/* SFBS - Scan for first bit set*/
void interpret_SFBS(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    /* Fetch the word to search */
    uint16_t val = cpu_ctx->state.reg.r[RA];
    /* Calculate the number of leading zeros (0 to 16) */
    uint16_t zeros = count_leading_zeros(val);
    /* Store the result into RB */
    cpu_ctx->state.reg.r[RB] = zeros;
    calculate_flags_16bit(cpu_ctx, (int16_t)val);
    cpu_ctx->state.total_cycles += CLK_CYC_SFBS;
    cpu_ctx->state.reg.ic += 1;
}

/* DST - Double precision store*/
void interpret_DST(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    ok = 0 == store_data_words_reg(cpu_ctx, DO_ADDR, 2, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_DST;
    cpu_ctx->state.reg.ic += 2;
}

/* SRM - Store register through mask*/
void interpret_SRM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    uint16_t data = cpu_ctx->state.reg.r[RA];
    uint16_t mask = cpu_ctx->state.reg.r[(RA + 1) & 0xF];
    uint16_t new_val = (DO[0] & ~mask) | (data & mask);
    store_data_word(cpu_ctx, DO_ADDR, new_val);
    cpu_ctx->state.total_cycles += CLK_CYC_SRM;
    cpu_ctx->state.reg.ic += 2;
}

/* DSTI - Double precision store, Indirect*/
void interpret_DSTI(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t DO_ADDR;
    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);
    ok = 0 == store_data_words_reg(cpu_ctx, DO_ADDR, 2, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_DSTI;
    cpu_ctx->state.reg.ic += 2;
}

/* STM - Store multiple registers*/
void interpret_STM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    ok = 0 == store_data_words_reg(cpu_ctx, DO_ADDR, N +1, 0 /* reg 0 */);
    cpu_ctx->state.total_cycles += CLK_CYC_STM(N +1);
    cpu_ctx->state.reg.ic += 2;
}

/* EFST - Extended precision floating point store*/
void interpret_EFST(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    ok = 0 == store_data_words_reg(cpu_ctx, DO_ADDR, 3, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_EFST;
    cpu_ctx->state.reg.ic += 2;
}

/* STUB - Store into upper byte*/
void interpret_STUB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    DO[0] = (DO[0] & 0x00FF) | ((cpu_ctx->state.reg.r[RA] & 0x00FF) << 8);
    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_STUB;
    cpu_ctx->state.reg.ic += 2;
}

/* STLB - Store into lower byte*/
void interpret_STLB(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    DO[0] = (DO[0] & 0xFF00) | (cpu_ctx->state.reg.r[RA] & 0x00FF);
    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_STLB;
    cpu_ctx->state.reg.ic += 2;
}

/* SUBI - Store into upper byte, Indirect*/
void interpret_SUBI(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t DO_ADDR;
    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    DO[0] = (DO[0] & 0x00FF) | ((cpu_ctx->state.reg.r[RA] & 0x00FF) << 8);
    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_SUBI;
    cpu_ctx->state.reg.ic += 2;
}

/* SLBI - Store into lower byte, Indirect*/
void interpret_SLBI(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_INDIRECT = ((RX > 0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint16_t DO_ADDR;
    ok = fetch_data_word(cpu_ctx, (uint16_t)DO_INDIRECT, &DO_ADDR);
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    DO[0] = (DO[0] & 0xFF00) | (cpu_ctx->state.reg.r[RA] & 0x00FF);
    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);
    cpu_ctx->state.total_cycles += CLK_CYC_SLBI;
    cpu_ctx->state.reg.ic += 2;
}

/* PSHM - Push multiple registers onto the stack*/
void interpret_PSHM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 10 */
    int count = RB - RA + 1;
    if (count < 0)
        count += 16;
    int stk_addr = cpu_ctx->state.reg.r[15] - count;
    if (RA + count > 15) /* R15 will be inserted */
        cpu_ctx->state.reg.r[15] -= RA + count - 15;
    ok = 0 == store_data_words_reg(cpu_ctx, stk_addr, count, RA);
    cpu_ctx->state.reg.r[15] = stk_addr;
    cpu_ctx->state.total_cycles += CLK_CYC_PSHM(count);
    cpu_ctx->state.reg.ic += 1;
}

/* A - Single precision integer add*/
void interpret_A(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int32_t res = (int32_t)a + (int32_t)b;
    bool carry = ((uint32_t)(uint16_t)a + (uint32_t)(uint16_t)b) > 0xFFFF;
    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    calculate_flags_16bit(cpu_ctx, (int16_t)res);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_A;
    cpu_ctx->state.reg.ic += 2;
}

/* AR - Single precision integer add, Register*/
void interpret_AR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int32_t res = (int32_t)a + (int32_t)b;
    bool carry = ((uint32_t)(uint16_t)a + (uint32_t)(uint16_t)b) > 0xFFFF;
    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    calculate_flags_16bit(cpu_ctx, (int16_t)res);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_AR;
    cpu_ctx->state.reg.ic += 1;
}

/* AISP - Single precision integer add, Immediate Short Positive*/
void interpret_AISP(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = (opcode & 0x000F) + 1;
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int32_t res = (int32_t)a + (int32_t)b;
    bool carry = ((uint32_t)(uint16_t)a + (uint32_t)(uint16_t)b) > 0xFFFF;
    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    calculate_flags_16bit(cpu_ctx, (int16_t)res);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_AISP;
    cpu_ctx->state.reg.ic += 1;
}

/* INCM - Increment memory by a positive integer*/
void interpret_INCM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t mem_val = DO[0];
    int16_t amount = N + 1;
    int32_t res = (int32_t)mem_val + amount;
    bool carry = ((uint32_t)(uint16_t)mem_val + (uint32_t)amount) > 0xFFFF;
    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    DO[0] = (int16_t)res;
    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);
    calculate_flags_16bit(cpu_ctx, DO[0]);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_INCM;
    cpu_ctx->state.reg.ic += 2;
}

/* ABS - Single precision absolute value of register*/
void interpret_ABS(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int16_t val = DO[0];
    if (val == (int16_t)0x8000) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    int16_t res = (val < 0) ? -val : val;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_ABS(val < 0);
    cpu_ctx->state.reg.ic += 1;
}

/* DABS - Double precision absolute value of register*/
void interpret_DABS(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t val = ((int32_t)DO[0] << 16) | (uint16_t)DO[1];
    if (val == (int32_t)0x80000000) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    int32_t res = (val < 0) ? -val : val;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(res >> 16);
    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(res & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_DABS(val < 0);
    cpu_ctx->state.reg.ic += 1;
}

/* DA - Double precision integer add*/
void interpret_DA(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t a = ((int32_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF];
    int32_t b = ((int32_t)DO[0] << 16) | (uint16_t)DO[1];
    int64_t res = (int64_t)a + (int64_t)b;
    bool carry = ((uint64_t)(uint32_t)a + (uint64_t)(uint32_t)b) > 0xFFFFFFFFULL;
    if (res > 2147483647LL || res < -2147483648LL) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(res >> 16);
    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(res & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, RA);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_DA;
    cpu_ctx->state.reg.ic += 2;
}

/* DAR - Double precision integer add, Register*/
void interpret_DAR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t a = ((int32_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF];
    int32_t b = ((int32_t)DO[0] << 16) | (uint16_t)DO[1];
    int64_t res = (int64_t)a + (int64_t)b;
    bool carry = ((uint64_t)(uint32_t)a + (uint64_t)(uint32_t)b) > 0xFFFFFFFFULL;
    if (res > 2147483647LL || res < -2147483648LL) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(res >> 16);
    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(res & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, RA);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_DAR;
    cpu_ctx->state.reg.ic += 1;
}

/* FA - Floating point add*/
void interpret_FA(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    int32_t E_res;
    if (M_A == 0) { M_A = M_B; E_res = E_B; }
    else if (M_B == 0) { E_res = E_A; }
    else {
        int diff = E_A - E_B;
        if (diff > 0) { if (diff > 24) M_B = 0; else M_B >>= diff; E_res = E_A; }
        else if (diff < 0) { if (-diff > 24) M_A = 0; else M_A >>= -diff; E_res = E_B; }
        else { E_res = E_A; }
    }
    pack_float32(cpu_ctx, RA, M_A + M_B, E_res);
    cpu_ctx->state.total_cycles += CLK_CYC_FA;
    cpu_ctx->state.reg.ic += 2;
}

/* FAR - Floating point add, Register*/
void interpret_FAR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    int32_t E_res;
    if (M_A == 0) { M_A = M_B; E_res = E_B; }
    else if (M_B == 0) { E_res = E_A; }
    else {
        int diff = E_A - E_B;
        if (diff > 0) { if (diff > 24) M_B = 0; else M_B >>= diff; E_res = E_A; }
        else if (diff < 0) { if (-diff > 24) M_A = 0; else M_A >>= -diff; E_res = E_B; }
        else { E_res = E_A; }
    }
    pack_float32(cpu_ctx, RA, M_A + M_B, E_res);
    cpu_ctx->state.total_cycles += CLK_CYC_FAR;
    cpu_ctx->state.reg.ic += 1;
}

/* EFA - Extended precision floating point add*/
void interpret_EFA(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[3];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    int64_t M_A, M_B; int16_t E_A, E_B;
    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M_A, &E_A);
    unpack_float48(DO[0], DO[1], DO[2], &M_B, &E_B);
    int32_t E_res;
    if (M_A == 0) { M_A = M_B; E_res = E_B; }
    else if (M_B == 0) { E_res = E_A; }
    else {
        int diff = E_A - E_B;
        if (diff > 0) { if (diff > 40) M_B = 0; else M_B >>= diff; E_res = E_A; }
        else if (diff < 0) { if (-diff > 40) M_A = 0; else M_A >>= -diff; E_res = E_B; }
        else { E_res = E_A; }
    }
    pack_float48(cpu_ctx, RA, M_A + M_B, E_res);
    cpu_ctx->state.total_cycles += CLK_CYC_EFA;
    cpu_ctx->state.reg.ic += 2;
}

/* EFAR - Extended precision floating point add, Register*/
void interpret_EFAR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[3];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    DO[2] = cpu_ctx->state.reg.r[(RB+2)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int64_t M_A, M_B; int16_t E_A, E_B;
    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M_A, &E_A);
    unpack_float48(DO[0], DO[1], DO[2], &M_B, &E_B);
    int32_t E_res;
    if (M_A == 0) { M_A = M_B; E_res = E_B; }
    else if (M_B == 0) { E_res = E_A; }
    else {
        int diff = E_A - E_B;
        if (diff > 0) { if (diff > 40) M_B = 0; else M_B >>= diff; E_res = E_A; }
        else if (diff < 0) { if (-diff > 40) M_A = 0; else M_A >>= -diff; E_res = E_B; }
        else { E_res = E_A; }
    }
    pack_float48(cpu_ctx, RA, M_A + M_B, E_res);
    cpu_ctx->state.total_cycles += CLK_CYC_EFAR;
    cpu_ctx->state.reg.ic += 1;
}

/* FABS - Floating point absolute value of register*/
void interpret_FABS(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t M; int16_t E;
    unpack_float32(cpu_ctx->state.reg.r[(RB+0)&0xF], cpu_ctx->state.reg.r[(RB+1)&0xF], &M, &E);
    cpu_ctx->state.total_cycles += CLK_CYC_FABS(M < 0);
    if (M == -8388608) {
        E = E + 1; M = 4194304;
        if (E > 127) {
            cpu_ctx->state.reg.pir |= INTR_FLTOFL;
            E = 127; M = 0x7FFFFF;
        }
    } else if (M < 0) {
        M = -M;
    }
    pack_float32(cpu_ctx, RA, M, E);
    cpu_ctx->state.reg.ic += 1;
}

/* UAR - Unsigned add, Register*/
void interpret_UAR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    uint32_t a = (uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    uint32_t b = (uint16_t)DO[0];
    uint32_t res = a - b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    if (res > 0xFFFF) {
        cpu_ctx->state.reg.sw = CS_CARRY | (cpu_ctx->state.reg.sw & 0x0FFF);
    }
    else {
        calculate_flags_16bit(cpu_ctx, (int16_t)res);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_UAR;
    cpu_ctx->state.reg.ic += 1;
}

/* UA - Unsigned add*/
void interpret_UA(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    uint32_t a = (uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    uint32_t b = (uint16_t)DO[0];
    uint32_t res = a - b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    if (res > 0xFFFF) {
        cpu_ctx->state.reg.sw = CS_CARRY | (cpu_ctx->state.reg.sw & 0x0FFF);
    }
    else {
        calculate_flags_16bit(cpu_ctx, (int16_t)res);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_UA;
    cpu_ctx->state.reg.ic += 2;
}

/* S - Single precision integer subtract*/
void interpret_S(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int32_t res = (int32_t)a - (int32_t)b;
    bool carry = (uint16_t)a >= (uint16_t)b;
    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    calculate_flags_16bit(cpu_ctx, (int16_t)res);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_S;
    cpu_ctx->state.reg.ic += 2;
}

/* SR - Single precision integer subtract, Register*/
void interpret_SR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int32_t res = (int32_t)a - (int32_t)b;
    bool carry = (uint16_t)a >= (uint16_t)b;
    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    calculate_flags_16bit(cpu_ctx, (int16_t)res);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_SR;
    cpu_ctx->state.reg.ic += 1;
}

/* SISP - Single precision integer subtract, Immediate Short Positive*/
void interpret_SISP(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = (opcode & 0x000F) + 1;
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int32_t res = (int32_t)a - (int32_t)b;
    bool carry = (uint16_t)a >= (uint16_t)b;
    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    calculate_flags_16bit(cpu_ctx, (int16_t)res);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_SISP;
    cpu_ctx->state.reg.ic += 1;
}

/* DECM - Decrement memory by a positive integer*/
void interpret_DECM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t N = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t mem_val = DO[0];
    int16_t amount = N + 1;
    int32_t res = (int32_t)mem_val - amount;
    bool carry = (uint16_t)mem_val >= (uint16_t)amount;
    if (res > 32767 || res < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    DO[0] = (int16_t)res;
    ok = store_data_word(cpu_ctx, DO_ADDR, DO[0]);
    calculate_flags_16bit(cpu_ctx, DO[0]);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_DECM;
    cpu_ctx->state.reg.ic += 2;
}

/* NEG - Single precision negate register*/
void interpret_NEG(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int16_t val = DO[0];
    if (val == (int16_t)0x8000) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    int32_t res = -(int32_t)val;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    calculate_flags_16bit(cpu_ctx, (int16_t)res);
    cpu_ctx->state.total_cycles += CLK_CYC_NEG;
    cpu_ctx->state.reg.ic += 1;
}

/* DNEG - Double precision negate register*/
void interpret_DNEG(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t val = ((int32_t)DO[0] << 16) | (uint16_t)DO[1];
    if (val == (int32_t)0x80000000) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    int64_t res = -(int64_t)val;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(res >> 16);
    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(res & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_DNEG;
    cpu_ctx->state.reg.ic += 1;
}

/* DS - Double precision integer subtract*/
void interpret_DS(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t a = ((int32_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF];
    int32_t b = ((int32_t)DO[0] << 16) | (uint16_t)DO[1];
    int64_t res = (int64_t)a - (int64_t)b;
    bool carry = (uint32_t)a >= (uint32_t)b;
    if (res > 2147483647LL || res < -2147483648LL) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(res >> 16);
    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(res & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, RA);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_DS;
    cpu_ctx->state.reg.ic += 2;
}

/* DSR - Double precision integer subtract, Register*/
void interpret_DSR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t a = ((int32_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF];
    int32_t b = ((int32_t)DO[0] << 16) | (uint16_t)DO[1];
    int64_t res = (int64_t)a - (int64_t)b;
    bool carry = (uint32_t)a >= (uint32_t)b;
    if (res > 2147483647LL || res < -2147483648LL) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(res >> 16);
    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(res & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, RA);
    if (carry) cpu_ctx->state.reg.sw |= CS_CARRY;
    cpu_ctx->state.total_cycles += CLK_CYC_DSR;
    cpu_ctx->state.reg.ic += 1;
}

/* FS - Floating point subtract*/
void interpret_FS(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    M_B = -M_B;
    int32_t E_res;
    if (M_A == 0) { M_A = M_B; E_res = E_B; }
    else if (M_B == 0) { E_res = E_A; }
    else {
        int diff = E_A - E_B;
        if (diff > 0) { if (diff > 24) M_B = 0; else M_B >>= diff; E_res = E_A; }
        else if (diff < 0) { if (-diff > 24) M_A = 0; else M_A >>= -diff; E_res = E_B; }
        else { E_res = E_A; }
    }
    pack_float32(cpu_ctx, RA, M_A + M_B, E_res);
    cpu_ctx->state.total_cycles += CLK_CYC_FS;
    cpu_ctx->state.reg.ic += 2;
}

/* FSR - Floating point subtract, Register*/
void interpret_FSR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    M_B = -M_B;
    int32_t E_res;
    if (M_A == 0) { M_A = M_B; E_res = E_B; }
    else if (M_B == 0) { E_res = E_A; }
    else {
        int diff = E_A - E_B;
        if (diff > 0) { if (diff > 24) M_B = 0; else M_B >>= diff; E_res = E_A; }
        else if (diff < 0) { if (-diff > 24) M_A = 0; else M_A >>= -diff; E_res = E_B; }
        else { E_res = E_A; }
    }
    pack_float32(cpu_ctx, RA, M_A + M_B, E_res);
    cpu_ctx->state.total_cycles += CLK_CYC_FSR;
    cpu_ctx->state.reg.ic += 1;
}

/* EFS - Extended precision floating point subtract*/
void interpret_EFS(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[3];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    int64_t M_A, M_B; int16_t E_A, E_B;
    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M_A, &E_A);
    unpack_float48(DO[0], DO[1], DO[2], &M_B, &E_B);
    M_B = -M_B;
    int32_t E_res;
    if (M_A == 0) { M_A = M_B; E_res = E_B; }
    else if (M_B == 0) { E_res = E_A; }
    else {
        int diff = E_A - E_B;
        if (diff > 0) { if (diff > 40) M_B = 0; else M_B >>= diff; E_res = E_A; }
        else if (diff < 0) { if (-diff > 40) M_A = 0; else M_A >>= -diff; E_res = E_B; }
        else { E_res = E_A; }
    }
    pack_float48(cpu_ctx, RA, M_A + M_B, E_res);
    cpu_ctx->state.total_cycles += CLK_CYC_EFS;
    cpu_ctx->state.reg.ic += 2;
}

/* EFSR - Extended precision floating point subtract, Register*/
void interpret_EFSR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[3];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    DO[2] = cpu_ctx->state.reg.r[(RB+2)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int64_t M_A, M_B; int16_t E_A, E_B;
    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M_A, &E_A);
    unpack_float48(DO[0], DO[1], DO[2], &M_B, &E_B);
    M_B = -M_B;
    int32_t E_res;
    if (M_A == 0) { M_A = M_B; E_res = E_B; }
    else if (M_B == 0) { E_res = E_A; }
    else {
        int diff = E_A - E_B;
        if (diff > 0) { if (diff > 40) M_B = 0; else M_B >>= diff; E_res = E_A; }
        else if (diff < 0) { if (-diff > 40) M_A = 0; else M_A >>= -diff; E_res = E_B; }
        else { E_res = E_A; }
    }
    pack_float48(cpu_ctx, RA, M_A + M_B, E_res);
    cpu_ctx->state.total_cycles += CLK_CYC_EFSR;
    cpu_ctx->state.reg.ic += 1;
}

/* FNEG - Floating point negate register*/
void interpret_FNEG(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t M; int16_t E;
    unpack_float32(cpu_ctx->state.reg.r[(RB+0)&0xF], cpu_ctx->state.reg.r[(RB+1)&0xF], &M, &E);
    if (M == -8388608 && E == 127) {
        cpu_ctx->state.reg.pir |= INTR_FLTOFL;
        E = 127; M = 0x7FFFFF;
    } else if (M == 4194304 && E == -128) {
        cpu_ctx->state.reg.pir |= INTR_FLTUFL;
        E = 0; M = 0;
    } else if (M == -8388608) {
        E = E + 1; M = 4194304;
    } else if (M == 4194304) {
        E = E - 1; M = -8388608;
    } else {
        M = -M;
    }
    pack_float32(cpu_ctx, RA, M, E);
    cpu_ctx->state.total_cycles += CLK_CYC_FNEG;
    cpu_ctx->state.reg.ic += 1;
}

/* USR - Unsigned subtract, Register*/
void interpret_USR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    uint32_t a = (uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    uint32_t b = (uint16_t)DO[0];
    uint32_t res = a - b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    if (res > 0xFFFF) {
        cpu_ctx->state.reg.sw = CS_CARRY | (cpu_ctx->state.reg.sw & 0x0FFF);
    }
    else {
        calculate_flags_16bit(cpu_ctx, (int16_t)res);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_USR;
    cpu_ctx->state.reg.ic += 1;
}

/* US - Unsigned subtract*/
void interpret_US(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    uint32_t a = (uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    uint32_t b = (uint16_t)DO[0];
    uint32_t res = a - b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)res;
    if (res > 0xFFFF) {
        cpu_ctx->state.reg.sw = CS_CARRY | (cpu_ctx->state.reg.sw & 0x0FFF);
    }
    else {
        calculate_flags_16bit(cpu_ctx, (int16_t)res);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_US;
    cpu_ctx->state.reg.ic += 2;
}

/* MS - Single precision integer multiply with 16-bit product*/
void interpret_MS(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    int32_t prod = (int32_t)A * (int32_t)B;
    if (prod > 32767 || prod < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(prod & 0xFFFF);
    calculate_flags_16bit(cpu_ctx, (int16_t)prod);
    cpu_ctx->state.total_cycles += CLK_CYC_MS;
    cpu_ctx->state.reg.ic += 2;
}

/* MSR - Single precision integer multiply with 16-bit product, Register*/
void interpret_MSR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    int32_t prod = (int32_t)A * (int32_t)B;
    if (prod > 32767 || prod < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(prod & 0xFFFF);
    calculate_flags_16bit(cpu_ctx, (int16_t)prod);
    cpu_ctx->state.total_cycles += CLK_CYC_MSR;
    cpu_ctx->state.reg.ic += 1;
}

/* MISP - Single precision integer multiply with 16-bit product, Immediate Short Positive*/
void interpret_MISP(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = (opcode & 0x000F) + 1;
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    int32_t prod = (int32_t)A * (int32_t)B;
    if (prod > 32767 || prod < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(prod & 0xFFFF);
    calculate_flags_16bit(cpu_ctx, (int16_t)prod);
    cpu_ctx->state.total_cycles += CLK_CYC_MISP;
    cpu_ctx->state.reg.ic += 1;
}

/* MISN - Single precision integer multiply with 16-bit product, Immediate Short Negative*/
void interpret_MISN(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = -((opcode & 0x000F) + 1);
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    int32_t prod = (int32_t)A * (int32_t)B;
    if (prod > 32767 || prod < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(prod & 0xFFFF);
    calculate_flags_16bit(cpu_ctx, (int16_t)prod);
    cpu_ctx->state.total_cycles += CLK_CYC_MISN;
    cpu_ctx->state.reg.ic += 1;
}

/* M - Single precision integer multiply with 32-bit product*/
void interpret_M(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    int32_t prod = (int32_t)A * (int32_t)B;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)((prod >> 16) & 0xFFFF);
    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(prod & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_M;
    cpu_ctx->state.reg.ic += 2;
}

/* MR - Single precision integer multiply with 32-bit product, Register*/
void interpret_MR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    int32_t prod = (int32_t)A * (int32_t)B;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)((prod >> 16) & 0xFFFF);
    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(prod & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_MR;
    cpu_ctx->state.reg.ic += 1;
}

/* DM - Double precision integer multiply*/
void interpret_DM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t A = (int32_t)(((uint32_t)(uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF]);
    int32_t B = (int32_t)(((uint32_t)(uint16_t)DO[0] << 16) | (uint16_t)DO[1]);
    int64_t prod = (int64_t)A * (int64_t)B;
    if (prod > 2147483647LL || prod < -2147483648LL) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    int32_t res = (int32_t)prod;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)((res >> 16) & 0xFFFF);
    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(res & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_DM;
    cpu_ctx->state.reg.ic += 2;
}

/* DMR - Double precision integer multiply, Register*/
void interpret_DMR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t A = (int32_t)(((uint32_t)(uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF]);
    int32_t B = (int32_t)(((uint32_t)(uint16_t)DO[0] << 16) | (uint16_t)DO[1]);
    int64_t prod = (int64_t)A * (int64_t)B;
    if (prod > 2147483647LL || prod < -2147483648LL) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    int32_t res = (int32_t)prod;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)((res >> 16) & 0xFFFF);
    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(res & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_DMR;
    cpu_ctx->state.reg.ic += 1;
}

/* FM - Floating point multiply*/
void interpret_FM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    int64_t P = (int64_t)M_A * (int64_t)M_B;
    pack_float32(cpu_ctx, RA, (int32_t)(P >> 23), E_A + E_B);
    cpu_ctx->state.total_cycles += CLK_CYC_FM;
    cpu_ctx->state.reg.ic += 2;
}

/* FMR - Floating point multiply, Register*/
void interpret_FMR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    int64_t P = (int64_t)M_A * (int64_t)M_B;
    pack_float32(cpu_ctx, RA, (int32_t)(P >> 23), E_A + E_B);
    cpu_ctx->state.total_cycles += CLK_CYC_FMR;
    cpu_ctx->state.reg.ic += 1;
}

/* EFM - Extended precision floating point multiply*/
void interpret_EFM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[3];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    int64_t M_A, M_B; int16_t E_A, E_B;
    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M_A, &E_A);
    unpack_float48(DO[0], DO[1], DO[2], &M_B, &E_B);
    int64_t P = mult128_shift_0_63(M_A, M_B, 39);
    pack_float48(cpu_ctx, RA, P, E_A + E_B);
    cpu_ctx->state.total_cycles += CLK_CYC_EFM;
    cpu_ctx->state.reg.ic += 2;
}

/* EFMR - Extended precision floating point multiply, Register*/
void interpret_EFMR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[3];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    DO[2] = cpu_ctx->state.reg.r[(RB+2)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int64_t M_A, M_B; int16_t E_A, E_B;
    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M_A, &E_A);
    unpack_float48(DO[0], DO[1], DO[2], &M_B, &E_B);
    int64_t P = mult128_shift_0_63(M_A, M_B, 39);
    pack_float48(cpu_ctx, RA, P, E_A + E_B);
    cpu_ctx->state.total_cycles += CLK_CYC_EFMR;
    cpu_ctx->state.reg.ic += 1;
}

/* DV - Single precision integer divide with 16-bit dividend*/
void interpret_DV(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    if (B == 0) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    } else if (A == (int16_t)0x8000 && B == -1) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
        cpu_ctx->state.reg.r[(RA+0)&0xF] = 0x8000; /* Q */
        cpu_ctx->state.reg.r[(RA+1)&0xF] = 0;      /* Rem */
        calculate_flags_16bit(cpu_ctx, 0x8000);
    } else {
        int16_t Q = A / B;
        int16_t Rem = A % B;
        cpu_ctx->state.reg.r[(RA+0)&0xF] = Q;
        cpu_ctx->state.reg.r[(RA+1)&0xF] = Rem;
        calculate_flags_16bit(cpu_ctx, Q);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_DV;
    cpu_ctx->state.reg.ic += 2;
}

/* DVR - Single precision integer divide with 16-bit dividend, Register*/
void interpret_DVR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    if (B == 0) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    } else if (A == (int16_t)0x8000 && B == -1) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
        cpu_ctx->state.reg.r[(RA+0)&0xF] = 0x8000; /* Q */
        cpu_ctx->state.reg.r[(RA+1)&0xF] = 0;      /* Rem */
        calculate_flags_16bit(cpu_ctx, 0x8000);
    } else {
        int16_t Q = A / B;
        int16_t Rem = A % B;
        cpu_ctx->state.reg.r[(RA+0)&0xF] = Q;
        cpu_ctx->state.reg.r[(RA+1)&0xF] = Rem;
        calculate_flags_16bit(cpu_ctx, Q);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_DVR;
    cpu_ctx->state.reg.ic += 1;
}

/* DISP - Single precision integer divide with 16-bit dividend, Immediate Short Positive*/
void interpret_DISP(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = (opcode & 0x000F) + 1;
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    if (B == 0) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    } else if (A == (int16_t)0x8000 && B == -1) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
        cpu_ctx->state.reg.r[(RA+0)&0xF] = 0x8000; /* Q */
        cpu_ctx->state.reg.r[(RA+1)&0xF] = 0;      /* Rem */
        calculate_flags_16bit(cpu_ctx, 0x8000);
    } else {
        int16_t Q = A / B;
        int16_t Rem = A % B;
        cpu_ctx->state.reg.r[(RA+0)&0xF] = Q;
        cpu_ctx->state.reg.r[(RA+1)&0xF] = Rem;
        calculate_flags_16bit(cpu_ctx, Q);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_DISP;
    cpu_ctx->state.reg.ic += 1;
}

/* DISN - Single precision integer divide with 16-bit dividend, Immediate Short Negative*/
void interpret_DISN(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = -((opcode & 0x000F) + 1);
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    if (B == 0) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    } else if (A == (int16_t)0x8000 && B == -1) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
        cpu_ctx->state.reg.r[(RA+0)&0xF] = 0x8000; /* Q */
        cpu_ctx->state.reg.r[(RA+1)&0xF] = 0;      /* Rem */
        calculate_flags_16bit(cpu_ctx, 0x8000);
    } else {
        int16_t Q = A / B;
        int16_t Rem = A % B;
        cpu_ctx->state.reg.r[(RA+0)&0xF] = Q;
        cpu_ctx->state.reg.r[(RA+1)&0xF] = Rem;
        calculate_flags_16bit(cpu_ctx, Q);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_DISN;
    cpu_ctx->state.reg.ic += 1;
}

/* D - Single precision integer divide with 32-bit dividend*/
void interpret_D(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int32_t A = (int32_t)(((uint32_t)(uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF]);
    int16_t B = (int16_t)DO[0];
    if (B == 0) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    } else {
        int32_t Q = A / B;
        int16_t Rem = A % B;
        if (Q > 32767 || Q < -32768) {
            cpu_ctx->state.reg.pir |= INTR_FIXOFL;
        } else {
            cpu_ctx->state.reg.r[(RA+0)&0xF] = Q;
            cpu_ctx->state.reg.r[(RA+1)&0xF] = Rem;
            calculate_flags_16bit(cpu_ctx, (int16_t)Q);
        }
    }
    cpu_ctx->state.total_cycles += CLK_CYC_D;
    cpu_ctx->state.reg.ic += 2;
}

/* DR - Single precision integer divide with 32-bit dividend, Register*/
void interpret_DR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t A = (int32_t)(((uint32_t)(uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF]);
    int16_t B = (int16_t)DO[0];
    if (B == 0) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    } else {
        int32_t Q = A / B;
        int16_t Rem = A % B;
        if (Q > 32767 || Q < -32768) {
            cpu_ctx->state.reg.pir |= INTR_FIXOFL;
        } else {
            cpu_ctx->state.reg.r[(RA+0)&0xF] = Q;
            cpu_ctx->state.reg.r[(RA+1)&0xF] = Rem;
            calculate_flags_16bit(cpu_ctx, (int16_t)Q);
        }
    }
    cpu_ctx->state.total_cycles += CLK_CYC_DR;
    cpu_ctx->state.reg.ic += 1;
}

/* DD - Double precision integer divide*/
void interpret_DD(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t A = (int32_t)(((uint32_t)(uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF]);
    int32_t B = (int32_t)(((uint32_t)(uint16_t)DO[0] << 16) | (uint16_t)DO[1]);
    if (B == 0) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    } else if (A == (int32_t)0x80000000 && B == -1) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
        cpu_ctx->state.reg.r[(RA+0)&0xF] = 0x8000;
        cpu_ctx->state.reg.r[(RA+1)&0xF] = 0x0000; /* Wrap Q */
        calculate_flags_32bit_reg(cpu_ctx, RA);
    } else {
        int32_t Q = A / B;
        cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)((Q >> 16) & 0xFFFF);
        cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(Q & 0xFFFF);
        calculate_flags_32bit_reg(cpu_ctx, RA);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_DD;
    cpu_ctx->state.reg.ic += 2;
}

/* DDR - Double precision integer divide, Register*/
void interpret_DDR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t A = (int32_t)(((uint32_t)(uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF]);
    int32_t B = (int32_t)(((uint32_t)(uint16_t)DO[0] << 16) | (uint16_t)DO[1]);
    if (B == 0) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    } else if (A == (int32_t)0x80000000 && B == -1) {
        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
        cpu_ctx->state.reg.r[(RA+0)&0xF] = 0x8000;
        cpu_ctx->state.reg.r[(RA+1)&0xF] = 0x0000; /* Wrap Q */
        calculate_flags_32bit_reg(cpu_ctx, RA);
    } else {
        int32_t Q = A / B;
        cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)((Q >> 16) & 0xFFFF);
        cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(Q & 0xFFFF);
        calculate_flags_32bit_reg(cpu_ctx, RA);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_DDR;
    cpu_ctx->state.reg.ic += 1;
}

/* FD - Floating point divide*/
void interpret_FD(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    if (M_B == 0) { cpu_ctx->state.reg.pir |= INTR_FLTOFL; return; }
    uint64_t M_A_abs = (M_A < 0) ? -M_A : M_A;
    uint64_t M_B_abs = (M_B < 0) ? -M_B : M_B;
    uint64_t M_A_abs_shift = (M_A_abs << 23);
    uint64_t Q_abs = M_A_abs_shift / M_B_abs;
    int32_t RemNotZero = (M_A_abs_shift % M_B_abs != 0)?1:0;
    int32_t Q = ((M_A ^ M_B) < 0) ? -(int32_t)(Q_abs + RemNotZero) : (int32_t)Q_abs;
    pack_float32(cpu_ctx, RA, Q, E_A - E_B);
    cpu_ctx->state.total_cycles += CLK_CYC_FD;
    cpu_ctx->state.reg.ic += 2;
}

/* FDR - Floating point divide, Register*/
void interpret_FDR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    if (M_B == 0) { cpu_ctx->state.reg.pir |= INTR_FLTOFL; return; }
    uint64_t M_A_abs = (M_A < 0) ? -M_A : M_A;
    uint64_t M_B_abs = (M_B < 0) ? -M_B : M_B;
    uint64_t M_A_abs_shift = (M_A_abs << 23);
    uint64_t Q_abs = M_A_abs_shift / M_B_abs;
    int32_t RemNotZero = (M_A_abs_shift % M_B_abs != 0)?1:0;
    int32_t Q = ((M_A ^ M_B) < 0) ? -(int32_t)(Q_abs + RemNotZero) : (int32_t)Q_abs;
    pack_float32(cpu_ctx, RA, Q, E_A - E_B);
    cpu_ctx->state.total_cycles += CLK_CYC_FDR;
    cpu_ctx->state.reg.ic += 1;
}

/* EFD - Extended precision floating point divide*/
void interpret_EFD(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[3];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    int64_t M_A, M_B; int16_t E_A, E_B;
    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M_A, &E_A);
    unpack_float48(DO[0], DO[1], DO[2], &M_B, &E_B);
    if (M_B == 0) { cpu_ctx->state.reg.pir |= INTR_FLTOFL; return; }
    uint64_t M_A_abs = (M_A < 0) ? -M_A : M_A;
    uint64_t M_B_abs = (M_B < 0) ? -M_B : M_B;
    uint64_t div_hi = M_A_abs << 17;
    uint64_t Q_hi = (div_hi / M_B_abs << 22);
    uint64_t div_lo = (div_hi % M_B_abs << 22);
    uint64_t Q_abs = Q_hi + (div_lo / M_B_abs);
    int64_t RemNotZero = (div_lo % M_B_abs != 0)?1:0;
    int64_t Q = ((M_A ^ M_B) < 0) ? -(int64_t)(Q_abs + RemNotZero) : (int64_t)Q_abs;
    pack_float48(cpu_ctx, RA, Q, E_A - E_B);
    cpu_ctx->state.total_cycles += CLK_CYC_EFD;
    cpu_ctx->state.reg.ic += 2;
}

/* EFDR - Extended precision floating point divide, Register*/
void interpret_EFDR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[3];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    DO[2] = cpu_ctx->state.reg.r[(RB+2)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int64_t M_A, M_B; int16_t E_A, E_B;
    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M_A, &E_A);
    unpack_float48(DO[0], DO[1], DO[2], &M_B, &E_B);
    if (M_B == 0) { cpu_ctx->state.reg.pir |= INTR_FLTOFL; return; }
    uint64_t M_A_abs = (M_A < 0) ? -M_A : M_A;
    uint64_t M_B_abs = (M_B < 0) ? -M_B : M_B;
    uint64_t div_hi = M_A_abs << 17;
    uint64_t Q_hi = (div_hi / M_B_abs << 22);
    uint64_t div_lo = (div_hi % M_B_abs << 22);
    uint64_t Q_abs = Q_hi + (div_lo / M_B_abs);
    int64_t RemNotZero = (div_lo % M_B_abs != 0)?1:0;
    int64_t Q = ((M_A ^ M_B) < 0) ? -(int64_t)(Q_abs + RemNotZero) : (int64_t)Q_abs;
    pack_float48(cpu_ctx, RA, Q, E_A - E_B);
    cpu_ctx->state.total_cycles += CLK_CYC_EFDR;
    cpu_ctx->state.reg.ic += 1;
}

/* STE - Store effective*/
void interpret_STE(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint32_t eff_addr = imm_value;
    if (RX == 0) {
        eff_addr += cpu_ctx->state.reg.r[15];
    }
    else {
        eff_addr += cpu_ctx->state.reg.r[RX];
        eff_addr += (cpu_ctx->state.reg.r[RX-1] & 0x7F) << 16; /* ok to subtract one since RX != 0 */
    }
    if (eff_addr + 1 > cpu_ctx->state.num_phys_mem_pages * 4096) {
        cpu_ctx->state.reg.pir |= INTR_MACHERR;
        cpu_ctx->state.reg.ft |= FT_ILL_ADDR;
    } else {
        *access_effective_address(&cpu_ctx->state, eff_addr + 0) = cpu_ctx->state.reg.r[(RA+0)&0xF];
    }
    cpu_ctx->state.total_cycles += CLK_CYC_STE;
    cpu_ctx->state.reg.ic += 2;
}

/* DSTE - Double precision store effective*/
void interpret_DSTE(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint32_t eff_addr = imm_value;
    if (RX == 0) {
        eff_addr += cpu_ctx->state.reg.r[15];
    }
    else {
        eff_addr += cpu_ctx->state.reg.r[RX];
        eff_addr += (cpu_ctx->state.reg.r[RX-1] & 0x7F) << 16; /* ok to subtract one since RX != 0 */
    }
    if (eff_addr + 2 > cpu_ctx->state.num_phys_mem_pages * 4096) {
        cpu_ctx->state.reg.pir |= INTR_MACHERR;
        cpu_ctx->state.reg.ft |= FT_ILL_ADDR;
    } else {
        *access_effective_address(&cpu_ctx->state, eff_addr + 0) = cpu_ctx->state.reg.r[(RA+0)&0xF];
        *access_effective_address(&cpu_ctx->state, eff_addr + 1) = cpu_ctx->state.reg.r[(RA+1)&0xF];
    }
    cpu_ctx->state.total_cycles += CLK_CYC_DSTE;
    cpu_ctx->state.reg.ic += 2;
}

/* LE - Load effective*/
void interpret_LE(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint32_t eff_addr = imm_value;
    if (RX == 0) {
        eff_addr += cpu_ctx->state.reg.r[15];
    }
    else {
        eff_addr += cpu_ctx->state.reg.r[RX];
        eff_addr += (cpu_ctx->state.reg.r[RX-1] & 0x7F) << 16; /* ok to subtract one since RX != 0 */
    }
    if (eff_addr + 1 > cpu_ctx->state.num_phys_mem_pages * 4096) {
        cpu_ctx->state.reg.pir |= INTR_MACHERR;
        cpu_ctx->state.reg.ft |= FT_ILL_ADDR;
    } else {
        cpu_ctx->state.reg.r[(RA+0)&0xF] = *access_effective_address(&cpu_ctx->state, eff_addr + 0);
        calculate_flags_16bit(cpu_ctx, cpu_ctx->state.reg.r[RA]);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_LE;
    cpu_ctx->state.reg.ic += 2;
}

/* DLE - Double precision load effective*/
void interpret_DLE(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    uint32_t eff_addr = imm_value;
    if (RX == 0) {
        eff_addr += cpu_ctx->state.reg.r[15];
    }
    else {
        eff_addr += cpu_ctx->state.reg.r[RX];
        eff_addr += (cpu_ctx->state.reg.r[RX-1] & 0x7F) << 16; /* ok to subtract one since RX != 0 */
    }
    if (eff_addr + 2 > cpu_ctx->state.num_phys_mem_pages * 4096) {
        cpu_ctx->state.reg.pir |= INTR_MACHERR;
        cpu_ctx->state.reg.ft |= FT_ILL_ADDR;
    } else {
        cpu_ctx->state.reg.r[(RA+0)&0xF] = *access_effective_address(&cpu_ctx->state, eff_addr + 0);
        cpu_ctx->state.reg.r[(RA+1)&0xF] = *access_effective_address(&cpu_ctx->state, eff_addr + 1);
        calculate_flags_32bit_reg(cpu_ctx, RA);
    }
    cpu_ctx->state.total_cycles += CLK_CYC_DLE;
    cpu_ctx->state.reg.ic += 2;
}

/* OR - Inclusive logical OR*/
void interpret_OR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = a | b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_OR;
    cpu_ctx->state.reg.ic += 2;
}

/* ORR - Inclusive logical OR, Register*/
void interpret_ORR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = a | b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_ORR;
    cpu_ctx->state.reg.ic += 1;
}

/* AND - Logical AND*/
void interpret_AND(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = a & b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_AND;
    cpu_ctx->state.reg.ic += 2;
}

/* ANDR - Logical AND, Register*/
void interpret_ANDR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = a & b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_ANDR;
    cpu_ctx->state.reg.ic += 1;
}

/* XOR - Exclusive logical OR*/
void interpret_XOR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = a ^ b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_XOR;
    cpu_ctx->state.reg.ic += 2;
}

/* XORR - Exclusive logical OR, Register*/
void interpret_XORR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = a ^ b;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_XORR;
    cpu_ctx->state.reg.ic += 1;
}

/* N - Logical NAND*/
void interpret_N(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = ~(a & b);
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_N;
    cpu_ctx->state.reg.ic += 2;
}

/* NR - Logical NAND, Register*/
void interpret_NR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int16_t a = cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t b = DO[0];
    int16_t res;
    res = ~(a & b);
    cpu_ctx->state.reg.r[(RA+0)&0xF] = res;
    calculate_flags_16bit(cpu_ctx, res);
    cpu_ctx->state.total_cycles += CLK_CYC_NR;
    cpu_ctx->state.reg.ic += 1;
}

/* FIX - Convert floating point to 16-bit integer*/
void interpret_FIX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t M; int16_t E;
    unpack_float32(cpu_ctx->state.reg.r[(RB+0)&0xF], cpu_ctx->state.reg.r[(RB+1)&0xF], &M, &E);
    int shift = E - 23;
    int32_t int_val = (M == 0) ? 0 : ((shift >= 0) ? (M << shift) : ((-shift >= 24) ? 0 : (M / (1 << -shift))));
    if (int_val > 32767 || int_val < -32768) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (int16_t)int_val;
    calculate_flags_16bit(cpu_ctx, (int16_t)int_val);
    cpu_ctx->state.total_cycles += CLK_CYC_FIX;
    cpu_ctx->state.reg.ic += 1;
}

/* FLT - Convert 16-bit integer to floating point*/
void interpret_FLT(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    pack_float32(cpu_ctx, RA, (int16_t)cpu_ctx->state.reg.r[(RB+0)&0xF], 23);
    cpu_ctx->state.total_cycles += CLK_CYC_FLT;
    cpu_ctx->state.reg.ic += 1;
}

/* EFIX - Convert extended precision floating point to 32-bit integer*/
void interpret_EFIX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[3];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    DO[2] = cpu_ctx->state.reg.r[(RB+2)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int64_t M; int16_t E;
    unpack_float48(cpu_ctx->state.reg.r[(RB+0)&0xF], cpu_ctx->state.reg.r[(RB+1)&0xF], cpu_ctx->state.reg.r[(RB+2)&0xF], &M, &E);
    int shift = E - 39;
    int64_t int_val = (M == 0) ? 0 : ((shift >= 0) ? (M << shift) : ((-shift >= 40) ? 0 : (M / (1LL << -shift))));
    if (int_val > 2147483647LL || int_val < -2147483648LL) cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(int_val >> 16);
    cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)(int_val & 0xFFFF);
    calculate_flags_32bit_reg(cpu_ctx, RA);
    cpu_ctx->state.total_cycles += CLK_CYC_EFIX;
    cpu_ctx->state.reg.ic += 1;
}

/* EFLT - Convert 32-bit integer to extended precision floating point*/
void interpret_EFLT(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[3];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    DO[2] = cpu_ctx->state.reg.r[(RB+2)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int64_t val = ((int64_t)cpu_ctx->state.reg.r[(RB+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RB+1)&0xF];
    pack_float48(cpu_ctx, RA, (val << 32) >> 32, 39);
    cpu_ctx->state.total_cycles += CLK_CYC_EFLT;
    cpu_ctx->state.reg.ic += 1;
}

/* XBR - Exchange bytes in register*/
void interpret_XBR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t val = cpu_ctx->state.reg.r[RA];
    cpu_ctx->state.reg.r[RA] = ((val & 0xFF) << 8) | ((val >> 8) & 0xFF);
    cpu_ctx->state.total_cycles += CLK_CYC_XBR;
    cpu_ctx->state.reg.ic += 1;
}

/* XWR - Exchange words in registers*/
void interpret_XWR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    uint16_t tmp = cpu_ctx->state.reg.r[RA];
    cpu_ctx->state.reg.r[RA] = cpu_ctx->state.reg.r[RB];
    cpu_ctx->state.reg.r[RB] = tmp;
    cpu_ctx->state.total_cycles += CLK_CYC_XWR;
    cpu_ctx->state.reg.ic += 1;
}

/* C - Single precision compare*/
void interpret_C(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    if (A == B) cpu_ctx->state.reg.sw |= CS_ZERO;
    else if (A > B) cpu_ctx->state.reg.sw |= CS_POSITIVE;
    else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    cpu_ctx->state.total_cycles += CLK_CYC_C;
    cpu_ctx->state.reg.ic += 2;
}

/* CR - Single precision compare, Register*/
void interpret_CR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    if (A == B) cpu_ctx->state.reg.sw |= CS_ZERO;
    else if (A > B) cpu_ctx->state.reg.sw |= CS_POSITIVE;
    else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    cpu_ctx->state.total_cycles += CLK_CYC_CR;
    cpu_ctx->state.reg.ic += 1;
}

/* CISP - Single precision compare, Immediate Short Positive*/
void interpret_CISP(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = (opcode & 0x000F) + 1;
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    if (A == B) cpu_ctx->state.reg.sw |= CS_ZERO;
    else if (A > B) cpu_ctx->state.reg.sw |= CS_POSITIVE;
    else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    cpu_ctx->state.total_cycles += CLK_CYC_CISP;
    cpu_ctx->state.reg.ic += 1;
}

/* CISN - Single precision compare, Immediate Short Negative*/
void interpret_CISN(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = -((opcode & 0x000F) + 1);
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    int16_t A = (int16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    int16_t B = (int16_t)DO[0];
    if (A == B) cpu_ctx->state.reg.sw |= CS_ZERO;
    else if (A > B) cpu_ctx->state.reg.sw |= CS_POSITIVE;
    else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    cpu_ctx->state.total_cycles += CLK_CYC_CISN;
    cpu_ctx->state.reg.ic += 1;
}

/* CBL - Compare between limits*/
void interpret_CBL(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    int16_t L = (int16_t)DO[0];
    int16_t U = (int16_t)DO[1];
    if (L > U) cpu_ctx->state.reg.sw |= CS_CARRY;
    else {
        int16_t A = (int16_t)cpu_ctx->state.reg.r[RA];
        if (A < L) cpu_ctx->state.reg.sw |= CS_NEGATIVE;
        else if (A > U) cpu_ctx->state.reg.sw |= CS_POSITIVE;
        else cpu_ctx->state.reg.sw |= CS_ZERO;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_CBL;
    cpu_ctx->state.reg.ic += 2;
}

/* UCIM - Unsigned compare, Immediate*/
void interpret_UCIM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    int16_t DO[1];
    DO[0] = imm_value;
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    uint16_t A = (uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    uint16_t B = (uint16_t)DO[0];
    if (A == B) cpu_ctx->state.reg.sw |= CS_ZERO;
    else if (A > B) cpu_ctx->state.reg.sw |= CS_POSITIVE;
    else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    int16_t L = (int16_t)DO[0];
    int16_t U = (int16_t)DO[1];
    if (L > U) cpu_ctx->state.reg.sw |= CS_CARRY;
    else {
        int16_t A = (int16_t)cpu_ctx->state.reg.r[RA];
        if (A < L) cpu_ctx->state.reg.sw |= CS_NEGATIVE;
        else if (A > U) cpu_ctx->state.reg.sw |= CS_POSITIVE;
        else cpu_ctx->state.reg.sw |= CS_ZERO;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_UCIM;
    cpu_ctx->state.reg.ic += 2;
}

/* DC - Double precision compare*/
void interpret_DC(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    int32_t A = (int32_t)(((uint32_t)(uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF]);
    int32_t B = (int32_t)(((uint32_t)(uint16_t)DO[0] << 16) | (uint16_t)DO[1]);
    if (A == B) cpu_ctx->state.reg.sw |= CS_ZERO;
    else if (A > B) cpu_ctx->state.reg.sw |= CS_POSITIVE;
    else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    cpu_ctx->state.total_cycles += CLK_CYC_DC;
    cpu_ctx->state.reg.ic += 2;
}

/* DCR - Double precision compare, Register*/
void interpret_DCR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    int32_t A = (int32_t)(((uint32_t)(uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF] << 16) | (uint16_t)cpu_ctx->state.reg.r[(RA+1)&0xF]);
    int32_t B = (int32_t)(((uint32_t)(uint16_t)DO[0] << 16) | (uint16_t)DO[1]);
    if (A == B) cpu_ctx->state.reg.sw |= CS_ZERO;
    else if (A > B) cpu_ctx->state.reg.sw |= CS_POSITIVE;
    else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    cpu_ctx->state.total_cycles += CLK_CYC_DCR;
    cpu_ctx->state.reg.ic += 1;
}

/* FC - Floating point compare*/
void interpret_FC(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[2];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 2, DO);
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[RA], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    if (M_A == 0 && M_B == 0) {
        cpu_ctx->state.reg.sw |= CS_ZERO;
    } else {
        int diff = E_A - E_B;
        int64_t m_a_ext = M_A;
        int64_t m_b_ext = M_B;
        if (diff > 0) { if (diff > 24) m_b_ext = 0; else m_b_ext >>= diff; }
        else if (diff < 0) { if (-diff > 24) m_a_ext = 0; else m_a_ext >>= -diff; }
        int64_t res = m_a_ext - m_b_ext;
        if (res == 0) cpu_ctx->state.reg.sw |= CS_ZERO;
        else if (res > 0) cpu_ctx->state.reg.sw |= CS_POSITIVE;
        else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_FC;
    cpu_ctx->state.reg.ic += 2;
}

/* FCR - Floating point compare, Register*/
void interpret_FCR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[2];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int32_t M_A, M_B; int16_t E_A, E_B;
    unpack_float32(cpu_ctx->state.reg.r[RA], cpu_ctx->state.reg.r[(RA+1)&0xF], &M_A, &E_A);
    unpack_float32(DO[0], DO[1], &M_B, &E_B);
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    if (M_A == 0 && M_B == 0) {
        cpu_ctx->state.reg.sw |= CS_ZERO;
    } else {
        int diff = E_A - E_B;
        int64_t m_a_ext = M_A;
        int64_t m_b_ext = M_B;
        if (diff > 0) { if (diff > 24) m_b_ext = 0; else m_b_ext >>= diff; }
        else if (diff < 0) { if (-diff > 24) m_a_ext = 0; else m_a_ext >>= -diff; }
        int64_t res = m_a_ext - m_b_ext;
        if (res == 0) cpu_ctx->state.reg.sw |= CS_ZERO;
        else if (res > 0) cpu_ctx->state.reg.sw |= CS_POSITIVE;
        else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_FCR;
    cpu_ctx->state.reg.ic += 1;
}

/* EFC - Extended precision floating point compare*/
void interpret_EFC(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[3];
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    ok = 0 == fetch_data_words(cpu_ctx, DO_ADDR, 3, DO);
    int64_t M_A, M_B; int16_t E_A, E_B;
    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M_A, &E_A);
    unpack_float48(DO[0], DO[1], DO[2], &M_B, &E_B);
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    if (M_A == 0 && M_B == 0) {
        cpu_ctx->state.reg.sw |= CS_ZERO;
    } else {
        int diff = E_A - E_B;
        int64_t m_a_ext = M_A;
        int64_t m_b_ext = M_B;
        if (diff > 0) { if (diff > 40) m_b_ext = 0; else m_b_ext >>= diff; }
        else if (diff < 0) { if (-diff > 40) m_a_ext = 0; else m_a_ext >>= -diff; }
        int64_t res = m_a_ext - m_b_ext;
        if (res == 0) cpu_ctx->state.reg.sw |= CS_ZERO;
        else if (res > 0) cpu_ctx->state.reg.sw |= CS_POSITIVE;
        else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_EFC;
    cpu_ctx->state.reg.ic += 2;
}

/* EFCR - Extended precision floating point compare, Register*/
void interpret_EFCR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[3];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    DO[1] = cpu_ctx->state.reg.r[(RB+1)&0xF];
    DO[2] = cpu_ctx->state.reg.r[(RB+2)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    int64_t M_A, M_B; int16_t E_A, E_B;
    unpack_float48(cpu_ctx->state.reg.r[(RA+0)&0xF], cpu_ctx->state.reg.r[(RA+1)&0xF], cpu_ctx->state.reg.r[(RA+2)&0xF], &M_A, &E_A);
    unpack_float48(DO[0], DO[1], DO[2], &M_B, &E_B);
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    if (M_A == 0 && M_B == 0) {
        cpu_ctx->state.reg.sw |= CS_ZERO;
    } else {
        int diff = E_A - E_B;
        int64_t m_a_ext = M_A;
        int64_t m_b_ext = M_B;
        if (diff > 0) { if (diff > 40) m_b_ext = 0; else m_b_ext >>= diff; }
        else if (diff < 0) { if (-diff > 40) m_a_ext = 0; else m_a_ext >>= -diff; }
        int64_t res = m_a_ext - m_b_ext;
        if (res == 0) cpu_ctx->state.reg.sw |= CS_ZERO;
        else if (res > 0) cpu_ctx->state.reg.sw |= CS_POSITIVE;
        else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_EFCR;
    cpu_ctx->state.reg.ic += 1;
}

/* UCR - Unsigned compare, Register*/
void interpret_UCR(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RB = opcode & 0x000F;
    int16_t DO[1];
    DO[0] = cpu_ctx->state.reg.r[(RB+0)&0xF];
    /* TODO: Implement logic for instruction format 0 and addressing mode 0 */
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    uint16_t A = (uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    uint16_t B = (uint16_t)DO[0];
    if (A == B) cpu_ctx->state.reg.sw |= CS_ZERO;
    else if (A > B) cpu_ctx->state.reg.sw |= CS_POSITIVE;
    else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    int16_t L = (int16_t)DO[0];
    int16_t U = (int16_t)DO[1];
    if (L > U) cpu_ctx->state.reg.sw |= CS_CARRY;
    else {
        int16_t A = (int16_t)cpu_ctx->state.reg.r[RA];
        if (A < L) cpu_ctx->state.reg.sw |= CS_NEGATIVE;
        else if (A > U) cpu_ctx->state.reg.sw |= CS_POSITIVE;
        else cpu_ctx->state.reg.sw |= CS_ZERO;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_UCR;
    cpu_ctx->state.reg.ic += 1;
}

/* UC - Unsigned compare*/
void interpret_UC(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    bool ok = true;
    uint16_t RA = (opcode & 0x00F0) >> 4;
    uint16_t RX = opcode & 0x000F;
    uint16_t DO_ADDR = ((RX>0)?(uint16_t)cpu_ctx->state.reg.r[RX]:0) + (uint16_t)imm_value;
    int16_t DO[1];
    ok =  fetch_data_word(cpu_ctx, DO_ADDR, DO);
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    uint16_t A = (uint16_t)cpu_ctx->state.reg.r[(RA+0)&0xF];
    uint16_t B = (uint16_t)DO[0];
    if (A == B) cpu_ctx->state.reg.sw |= CS_ZERO;
    else if (A > B) cpu_ctx->state.reg.sw |= CS_POSITIVE;
    else cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    cpu_ctx->state.reg.sw &= 0x0FFF; /* Destroy Carry, P, Z, N */
    int16_t L = (int16_t)DO[0];
    int16_t U = (int16_t)DO[1];
    if (L > U) cpu_ctx->state.reg.sw |= CS_CARRY;
    else {
        int16_t A = (int16_t)cpu_ctx->state.reg.r[RA];
        if (A < L) cpu_ctx->state.reg.sw |= CS_NEGATIVE;
        else if (A > U) cpu_ctx->state.reg.sw |= CS_POSITIVE;
        else cpu_ctx->state.reg.sw |= CS_ZERO;
    }
    cpu_ctx->state.total_cycles += CLK_CYC_UC;
    cpu_ctx->state.reg.ic += 2;
}

/* NOP_BPT - No operation / Break point*/
void interpret_NOP_BPT(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /* imm_value */) {
    bool ok = true;
    /* TODO: Implement logic for instruction format 6 and addressing mode 10 */
    switch (opcode & 0xFF){
      case 0x00:  /*NOP*/
        cpu_ctx->state.reg.ic +=1;
        cpu_ctx->state.total_cycles += CLK_CYC_NOP;
        break;
      case 0xFF:  /* BPT */
        cpu_ctx->state.halt = true;
        cpu_ctx->state.reg.ic += 1;
        cpu_ctx->state.total_cycles += CLK_CYC_BPT;
        break;
      default:
        interpret_ILLEGAL(cpu_ctx, opcode, 0);
        break;
    }
}

void interpret_BRX(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    switch ((opcode & 0x00F0) >> 4) {
      case 0x0:
        interpret_LBX(cpu_ctx, opcode, imm_value);
      case 0x1:
        interpret_DLBX(cpu_ctx, opcode, imm_value);
      case 0x2:
        interpret_STBX(cpu_ctx, opcode, imm_value);
      case 0x3:
        interpret_DSTX(cpu_ctx, opcode, imm_value);
      case 0x4:
        interpret_ABX(cpu_ctx, opcode, imm_value);
      case 0x5:
        interpret_SBBX(cpu_ctx, opcode, imm_value);
      case 0x6:
        interpret_MBX(cpu_ctx, opcode, imm_value);
      case 0x7:
        interpret_DBX(cpu_ctx, opcode, imm_value);
      case 0x8:
        interpret_FABX(cpu_ctx, opcode, imm_value);
      case 0x9:
        interpret_FSBX(cpu_ctx, opcode, imm_value);
      case 0xA:
        interpret_FMBX(cpu_ctx, opcode, imm_value);
      case 0xB:
        interpret_FDBX(cpu_ctx, opcode, imm_value);
      case 0xC:
        interpret_CBX(cpu_ctx, opcode, imm_value);
      case 0xD:
        interpret_FCBX(cpu_ctx, opcode, imm_value);
      case 0xE:
        interpret_ANDX(cpu_ctx, opcode, imm_value);
      case 0xF:
        interpret_ORBX(cpu_ctx, opcode, imm_value);
      default:
        interpret_ILLEGAL(cpu_ctx, opcode, imm_value);
    }
}
void interpret_IMM(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) {
    switch (opcode & 0x000F) {
      case 0x0:
        interpret_ILLEGAL(cpu_ctx, opcode, imm_value);
      case 0x1:
        interpret_AIM(cpu_ctx, opcode, imm_value);
      case 0x2:
        interpret_SIM(cpu_ctx, opcode, imm_value);
      case 0x3:
        interpret_MIM(cpu_ctx, opcode, imm_value);
      case 0x4:
        interpret_MSIM(cpu_ctx, opcode, imm_value);
      case 0x5:
        interpret_DIM(cpu_ctx, opcode, imm_value);
      case 0x6:
        interpret_DVIM(cpu_ctx, opcode, imm_value);
      case 0x7:
        interpret_ANDM(cpu_ctx, opcode, imm_value);
      case 0x8:
        interpret_ORIM(cpu_ctx, opcode, imm_value);
      case 0x9:
        interpret_XORM(cpu_ctx, opcode, imm_value);
      case 0xA:
        interpret_CIM(cpu_ctx, opcode, imm_value);
      case 0xB:
        interpret_NIM(cpu_ctx, opcode, imm_value);
      case 0xC:
        interpret_ILLEGAL(cpu_ctx, opcode, imm_value);
      case 0xD:
        interpret_ILLEGAL(cpu_ctx, opcode, imm_value);
      case 0xE:
        interpret_ILLEGAL(cpu_ctx, opcode, imm_value);
      case 0xF:
        interpret_ILLEGAL(cpu_ctx, opcode, imm_value);
      default:
        interpret_ILLEGAL(cpu_ctx, opcode, imm_value);
    }
}
static void (*op_func_map[256])(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value) = {
interpret_LB, /* 0x00 */
interpret_LB, /* 0x01 */
interpret_LB, /* 0x02 */
interpret_LB, /* 0x03 */
interpret_DLB, /* 0x04 */
interpret_DLB, /* 0x05 */
interpret_DLB, /* 0x06 */
interpret_DLB, /* 0x07 */
interpret_STB, /* 0x08 */
interpret_STB, /* 0x09 */
interpret_STB, /* 0x0A */
interpret_STB, /* 0x0B */
interpret_DSTB, /* 0x0C */
interpret_DSTB, /* 0x0D */
interpret_DSTB, /* 0x0E */
interpret_DSTB, /* 0x0F */
interpret_AB, /* 0x10 */
interpret_AB, /* 0x11 */
interpret_AB, /* 0x12 */
interpret_AB, /* 0x13 */
interpret_SBB, /* 0x14 */
interpret_SBB, /* 0x15 */
interpret_SBB, /* 0x16 */
interpret_SBB, /* 0x17 */
interpret_MB, /* 0x18 */
interpret_MB, /* 0x19 */
interpret_MB, /* 0x1A */
interpret_MB, /* 0x1B */
interpret_DB, /* 0x1C */
interpret_DB, /* 0x1D */
interpret_DB, /* 0x1E */
interpret_DB, /* 0x1F */
interpret_FAB, /* 0x20 */
interpret_FAB, /* 0x21 */
interpret_FAB, /* 0x22 */
interpret_FAB, /* 0x23 */
interpret_FSB, /* 0x24 */
interpret_FSB, /* 0x25 */
interpret_FSB, /* 0x26 */
interpret_FSB, /* 0x27 */
interpret_FMB, /* 0x28 */
interpret_FMB, /* 0x29 */
interpret_FMB, /* 0x2A */
interpret_FMB, /* 0x2B */
interpret_FDB, /* 0x2C */
interpret_FDB, /* 0x2D */
interpret_FDB, /* 0x2E */
interpret_FDB, /* 0x2F */
interpret_ORB, /* 0x30 */
interpret_ORB, /* 0x31 */
interpret_ORB, /* 0x32 */
interpret_ORB, /* 0x33 */
interpret_ANDB, /* 0x34 */
interpret_ANDB, /* 0x35 */
interpret_ANDB, /* 0x36 */
interpret_ANDB, /* 0x37 */
interpret_CB, /* 0x38 */
interpret_CB, /* 0x39 */
interpret_CB, /* 0x3A */
interpret_CB, /* 0x3B */
interpret_FCB, /* 0x3C */
interpret_FCB, /* 0x3D */
interpret_FCB, /* 0x3E */
interpret_FCB, /* 0x3F */
interpret_BRX, /* 0x40 */
interpret_BRX, /* 0x41 */
interpret_BRX, /* 0x42 */
interpret_BRX, /* 0x43 */
interpret_ILLEGAL, /* 0x44 */
interpret_ILLEGAL, /* 0x45 */
interpret_ILLEGAL, /* 0x46 */
interpret_ILLEGAL, /* 0x47 */
interpret_XIO, /* 0x48 */
interpret_VIO, /* 0x49 */
interpret_IMM, /* 0x4A */
interpret_ILLEGAL, /* 0x4B */
interpret_ILLEGAL, /* 0x4C */
#ifdef GVSC
interpret_ESQR, /* 0x4D */
#else
interpret_ILLEGAL, /* 0x4D */
#endif
#ifdef GVSC
interpret_SQRT, /* 0x4E */
#else
interpret_ILLEGAL, /* 0x4E */
#endif
interpret_BIF, /* 0x4F */
interpret_SB, /* 0x50 */
interpret_SBR, /* 0x51 */
interpret_SBI, /* 0x52 */
interpret_RB, /* 0x53 */
interpret_RBR, /* 0x54 */
interpret_RBI, /* 0x55 */
interpret_TB, /* 0x56 */
interpret_TBR, /* 0x57 */
interpret_TBI, /* 0x58 */
interpret_TSB, /* 0x59 */
interpret_SVBR, /* 0x5A */
interpret_ILLEGAL, /* 0x5B */
interpret_RVBR, /* 0x5C */
interpret_ILLEGAL, /* 0x5D */
interpret_TVBR, /* 0x5E */
interpret_ILLEGAL, /* 0x5F */
interpret_SLL, /* 0x60 */
interpret_SRL, /* 0x61 */
interpret_SRA, /* 0x62 */
interpret_SLC, /* 0x63 */
interpret_ILLEGAL, /* 0x64 */
interpret_DSLL, /* 0x65 */
interpret_DSRL, /* 0x66 */
interpret_DSRA, /* 0x67 */
interpret_DSLC, /* 0x68 */
interpret_ILLEGAL, /* 0x69 */
interpret_SLR, /* 0x6A */
interpret_SAR, /* 0x6B */
interpret_SCR, /* 0x6C */
interpret_DSLR, /* 0x6D */
interpret_DSAR, /* 0x6E */
interpret_DSCR, /* 0x6F */
interpret_JC, /* 0x70 */
interpret_JCI, /* 0x71 */
interpret_JS, /* 0x72 */
interpret_SOJ, /* 0x73 */
interpret_BR, /* 0x74 */
interpret_BEZ, /* 0x75 */
interpret_BLT, /* 0x76 */
interpret_BEX, /* 0x77 */
interpret_BLE, /* 0x78 */
interpret_BGT, /* 0x79 */
interpret_BNZ, /* 0x7A */
interpret_BGE, /* 0x7B */
interpret_LSTI, /* 0x7C */
interpret_LST, /* 0x7D */
interpret_SJS, /* 0x7E */
interpret_URS, /* 0x7F */
interpret_L, /* 0x80 */
interpret_LR, /* 0x81 */
interpret_LISP, /* 0x82 */
interpret_LISN, /* 0x83 */
interpret_LI, /* 0x84 */
interpret_LIM, /* 0x85 */
interpret_DL, /* 0x86 */
interpret_DLR, /* 0x87 */
interpret_DLI, /* 0x88 */
interpret_LM, /* 0x89 */
interpret_EFL, /* 0x8A */
interpret_LUB, /* 0x8B */
interpret_LLB, /* 0x8C */
interpret_LUBI, /* 0x8D */
interpret_LLBI, /* 0x8E */
interpret_POPM, /* 0x8F */
interpret_ST, /* 0x90 */
interpret_STC, /* 0x91 */
interpret_STCI, /* 0x92 */
interpret_MOV, /* 0x93 */
interpret_STI, /* 0x94 */
#ifdef GVSC
interpret_SFBS, /* 0x95 */
#else
interpret_ILLEGAL, /* 0x95 */
#endif
interpret_DST, /* 0x96 */
interpret_SRM, /* 0x97 */
interpret_DSTI, /* 0x98 */
interpret_STM, /* 0x99 */
interpret_EFST, /* 0x9A */
interpret_STUB, /* 0x9B */
interpret_STLB, /* 0x9C */
interpret_SUBI, /* 0x9D */
interpret_SLBI, /* 0x9E */
interpret_PSHM, /* 0x9F */
interpret_A, /* 0xA0 */
interpret_AR, /* 0xA1 */
interpret_AISP, /* 0xA2 */
interpret_INCM, /* 0xA3 */
interpret_ABS, /* 0xA4 */
interpret_DABS, /* 0xA5 */
interpret_DA, /* 0xA6 */
interpret_DAR, /* 0xA7 */
interpret_FA, /* 0xA8 */
interpret_FAR, /* 0xA9 */
interpret_EFA, /* 0xAA */
interpret_EFAR, /* 0xAB */
interpret_FABS, /* 0xAC */
#ifdef GVSC
interpret_UAR, /* 0xAD */
#else
interpret_ILLEGAL, /* 0xAD */
#endif
#ifdef GVSC
interpret_UA, /* 0xAE */
#else
interpret_ILLEGAL, /* 0xAE */
#endif
interpret_ILLEGAL, /* 0xAF */
interpret_S, /* 0xB0 */
interpret_SR, /* 0xB1 */
interpret_SISP, /* 0xB2 */
interpret_DECM, /* 0xB3 */
interpret_NEG, /* 0xB4 */
interpret_DNEG, /* 0xB5 */
interpret_DS, /* 0xB6 */
interpret_DSR, /* 0xB7 */
interpret_FS, /* 0xB8 */
interpret_FSR, /* 0xB9 */
interpret_EFS, /* 0xBA */
interpret_EFSR, /* 0xBB */
interpret_FNEG, /* 0xBC */
#ifdef GVSC
interpret_USR, /* 0xBD */
#else
interpret_ILLEGAL, /* 0xBD */
#endif
#ifdef GVSC
interpret_US, /* 0xBE */
#else
interpret_ILLEGAL, /* 0xBE */
#endif
interpret_ILLEGAL, /* 0xBF */
interpret_MS, /* 0xC0 */
interpret_MSR, /* 0xC1 */
interpret_MISP, /* 0xC2 */
interpret_MISN, /* 0xC3 */
interpret_M, /* 0xC4 */
interpret_MR, /* 0xC5 */
interpret_DM, /* 0xC6 */
interpret_DMR, /* 0xC7 */
interpret_FM, /* 0xC8 */
interpret_FMR, /* 0xC9 */
interpret_EFM, /* 0xCA */
interpret_EFMR, /* 0xCB */
interpret_ILLEGAL, /* 0xCC */
interpret_ILLEGAL, /* 0xCD */
interpret_ILLEGAL, /* 0xCE */
interpret_ILLEGAL, /* 0xCF */
interpret_DV, /* 0xD0 */
interpret_DVR, /* 0xD1 */
interpret_DISP, /* 0xD2 */
interpret_DISN, /* 0xD3 */
interpret_D, /* 0xD4 */
interpret_DR, /* 0xD5 */
interpret_DD, /* 0xD6 */
interpret_DDR, /* 0xD7 */
interpret_FD, /* 0xD8 */
interpret_FDR, /* 0xD9 */
interpret_EFD, /* 0xDA */
interpret_EFDR, /* 0xDB */
#ifdef GVSC
interpret_STE, /* 0xDC */
#else
interpret_ILLEGAL, /* 0xDC */
#endif
#ifdef GVSC
interpret_DSTE, /* 0xDD */
#else
interpret_ILLEGAL, /* 0xDD */
#endif
#ifdef GVSC
interpret_LE, /* 0xDE */
#else
interpret_ILLEGAL, /* 0xDE */
#endif
#ifdef GVSC
interpret_DLE, /* 0xDF */
#else
interpret_ILLEGAL, /* 0xDF */
#endif
interpret_OR, /* 0xE0 */
interpret_ORR, /* 0xE1 */
interpret_AND, /* 0xE2 */
interpret_ANDR, /* 0xE3 */
interpret_XOR, /* 0xE4 */
interpret_XORR, /* 0xE5 */
interpret_N, /* 0xE6 */
interpret_NR, /* 0xE7 */
interpret_FIX, /* 0xE8 */
interpret_FLT, /* 0xE9 */
interpret_EFIX, /* 0xEA */
interpret_EFLT, /* 0xEB */
interpret_XBR, /* 0xEC */
interpret_XWR, /* 0xED */
interpret_ILLEGAL, /* 0xEE */
interpret_ILLEGAL, /* 0xEF */
interpret_C, /* 0xF0 */
interpret_CR, /* 0xF1 */
interpret_CISP, /* 0xF2 */
interpret_CISN, /* 0xF3 */
interpret_CBL, /* 0xF4 */
#ifdef GVSC
interpret_UCIM, /* 0xF5 */
#else
interpret_ILLEGAL, /* 0xF5 */
#endif
interpret_DC, /* 0xF6 */
interpret_DCR, /* 0xF7 */
interpret_FC, /* 0xF8 */
interpret_FCR, /* 0xF9 */
interpret_EFC, /* 0xFA */
interpret_EFCR, /* 0xFB */
#ifdef GVSC
interpret_UCR, /* 0xFC */
#else
interpret_ILLEGAL, /* 0xFC */
#endif
#ifdef GVSC
interpret_UC, /* 0xFD */
#else
interpret_ILLEGAL, /* 0xFD */
#endif
interpret_ILLEGAL, /* 0xFE */
interpret_NOP_BPT, /* 0xFF */
};
