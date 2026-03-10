/* arith.h  --  exports of arith.c */

#include "arch.h"
#include "cpu.h"

void update_cs (struct cpu_state *cpu, short *operand, datatype data_type);
void compare (struct cpu_state *cpu, datatype data_type, short *operand0, short *operand1);
void arith (struct cpu_state *cpu, operation_kind operation, datatype vartyp,
            short *operand0, short *operand1);

extern bool update_pir;  /* Defaults to TRUE.
                            Only required for SOJ which does not set PIR. */

