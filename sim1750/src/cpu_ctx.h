#ifndef _CPU_CTX_H
#define _CPU_CTX_H

#include "arch.h"
#include "bpt.h"
#include "phys_mem.h"

#define BT_SIZE (200)

struct cpu_context {
  struct cpu_state state;
  char name[32];
  struct regs bt_buff[BT_SIZE];
  int bt_next;
  int bt_cnt;
  int bpindex;
  struct breakpoint_t breakpt[MAX_BREAK];
  int n_breakpts;	/* breakpoint counter */
};

#endif // _CPU_CTX_H