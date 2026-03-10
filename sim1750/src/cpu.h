#ifndef _CPU_H
#define _CPU_H

#include "cpu_ctx.h"



extern void   init_cpu (struct cpu_state *cpu);
extern int    execute (struct cpu_context *cpu_ctx);

#define BREAKPT  -1
#define MEMERR   -2

#endif
