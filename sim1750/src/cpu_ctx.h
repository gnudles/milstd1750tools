#ifndef _CPU_CTX_H
#define _CPU_CTX_H

#include "arch.h"
#include "bpt.h"
#include "phys_mem.h"

#define BT_SIZE (200)

struct cpu_context {
  struct cpu_state state;
  char name[32];
  /*back trace*/
  struct regs bt_buff[BT_SIZE];
  int bt_next;
  int bt_cnt;
  /* break points */
  int bpindex;
  int wpindex; /* for old cpu.c */
  struct breakpoint_t breakpt[MAX_BREAK];
  struct watchpoint_t watchpt[MAX_BREAK];
  uint last_phys_ic;
  uint interrupt_counter; // used for debugging, counts how many interrupts were processed, and can be printed in the status command
  uint last_processed_interrupt; // used for debugging
  int n_breakpts;	/* breakpoint counter */
  int n_watchpts;
};

#endif // _CPU_CTX_H