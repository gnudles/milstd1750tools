#!/bin/bash
# Re-apply test_instructions.c changes securely

# 1. Includes and helpers
if ! grep -q "<math.h>" sim1750/src/test_instructions.c; then
    sed -i 's/#include <string.h>/#include <string.h>\n#include <math.h>/' sim1750/src/test_instructions.c
fi
if ! grep -q "void double_to_1750a_efloat(" sim1750/src/test_instructions.c; then
    sed -i 's/void test_BPT();/void test_BPT();\nvoid double_to_1750a_efloat(double val, uint16_t \*w1, uint16_t \*w2, uint16_t \*w3);\ndouble efloat_1750a_to_double(uint16_t w1, uint16_t w2, uint16_t w3);\nvoid double_to_1750a_float(double val, uint16_t \*w1, uint16_t \*w2);\ndouble float_1750a_to_double(uint16_t w1, uint16_t w2);/' sim1750/src/test_instructions.c
fi
sed -i 's/mantissa = (mantissa << 8) >> 8; \/\* Sign extend from 24-bits \*\//mantissa = (int32_t)((uint32_t)mantissa << 8) >> 8; \/\* Sign extend from 24-bits \*\//' sim1750/src/test_instructions.c
sed -i 's/\*w2 = (uint16_t)(((mantissa << 8) & 0xFF00) | (exp & 0xFF));/\*w2 = (uint16_t)((((uint32_t)mantissa << 8) \& 0xFF00) | (exp \& 0xFF));/' sim1750/src/test_instructions.c

# 2. Add test declarations
sed -i 's/void test_STUB_Upper_Byte() {/void test_SQRT();\nvoid test_ESQR();\nvoid test_Single_Shifts();\nvoid test_Double_Shifts();\n\nvoid test_STUB_Upper_Byte() {/' sim1750/src/test_instructions.c

# 3. Add test invocations
sed -i 's/test_STUB_Upper_Byte();/test_Single_Shifts();\n    test_Double_Shifts();\n    test_STUB_Upper_Byte();/' sim1750/src/test_instructions.c
sed -i 's/test_FD_Divide_By_Zero();/test_FD_Divide_By_Zero();\n    test_SQRT();\n    test_ESQR();/' sim1750/src/test_instructions.c
