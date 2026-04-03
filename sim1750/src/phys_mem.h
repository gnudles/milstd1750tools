/* phys_mem.h -- exports of phys_mem.c */

#ifndef _PHYS_MEM_H
#define _PHYS_MEM_H

#include "type.h"
struct cpu_state;  /* forward declaration to avoid circularity with cpu.h */
extern void   init_mem (struct cpu_state *cpu);
extern bool   was_written (struct cpu_state *cpu, uint phys_address);
/* simulation memory allocator */
extern void  *xalloc (uint number, uint size);
extern uint allocated;  /* total amount allocated by xalloc() */

/* mem.word[] contains one allocated memory page of the simulation.
   The mem.was_written[] array has one bit for each address within a page.
   It is set to `1' on a write operation to the respective address.
   Hence, read operations on uninitialized memory locations can be signalled.
 */

#include <stdint.h>

typedef struct
  {
    ushort word[4096];
    uint  was_written[128];  /* bit-packed, one bit per address */

    /* 2-Level Execution and Read Breakpoint / Watchpoint O(1) Mask */
    uint64_t read_exec_bp_summary; /* L1: 1 bit per 64-word block */
    uint64_t read_exec_bp[64];     /* L2: 1 bit per word */

    /* 2-Level Write Breakpoint / Watchpoint O(1) Mask */
    uint64_t write_bp_summary;     /* L1: 1 bit per 64-word block */
    uint64_t write_bp[64];         /* L2: 1 bit per word */
  } mem_t;

#define N_PAGES   256  /* 1 Mword address space */
/* extern mem_t *mem[N_PAGES]; */

#define MNULL  (mem_t *) 0

#endif

