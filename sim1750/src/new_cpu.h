#include "cpu_ctx.h"

extern int cpu_mainloop(struct cpu_context *cpu_ctx, uint64_t up_to_cycles);
extern void print_cpu_state(struct cpu_context *cpu_ctx);