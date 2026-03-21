cat << 'INNER_EOF' >> sim1750/src/test_instructions.c

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
INNER_EOF
