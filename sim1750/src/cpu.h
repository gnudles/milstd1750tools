#ifndef _CPU_H
#define _CPU_H

#include "arch.h"
#include "phys_mem.h"

#define BT_SIZE (200)

struct cpu_state {
  mem_t *mem[N_PAGES];
  struct regs reg;
  struct mmureg pagereg[2][16][16];
  uint instcnt;
  char name[32];
  double total_time_in_us;
  bool disable_timers;
  struct regs bt_buff[BT_SIZE];
  int bt_next;
  int bt_cnt;
  int bpindex;
  struct {
    ushort one_tatick_in_ns, one_tbtick_in_tatix;
    ushort one_gotick_in_10usec;
  } timers;
};

extern void   init_cpu (struct cpu_state *cpu);
extern int    execute (struct cpu_state *cpu);

#define BREAKPT  -1
#define MEMERR   -2

#endif
