#include <stddef.h>

#include "cpu_ctx.h"
#include <stdlib.h>

/* Return breakpoint index if breakpoint found for given
   type/bank/address_state/logical_address, or -1 if no breakpoint found. */
int
find_breakpt (struct cpu_context *cpu_ctx, uint phys_address)
{
  int i = cpu_ctx->n_breakpts;

  while ((--i) >= 0)
    {
      if (cpu_ctx->breakpt[i].is_active && cpu_ctx->breakpt[i].addr == phys_address)
	break;
    }
  return (i);
}
/* Return breakpoint index if breakpoint found for given
   type/bank/address_state/logical_address, or -1 if no breakpoint found. */
int
find_watchpt (struct cpu_context *cpu_ctx, watchtype type, uint phys_address)
{
  int i = cpu_ctx->n_watchpts;

  while ((--i) >= 0)
    {
      if (cpu_ctx->watchpt[i].is_active && cpu_ctx->watchpt[i].addr == phys_address
	  && ((cpu_ctx->watchpt[i].type & type) == type))
	break;
    }
  return (i);
}

void
set_wp_inactive (struct cpu_context *cpu_ctx, int wp_index)
{
  if (wp_index < 0)
    return;
  cpu_ctx->watchpt[wp_index].is_active = FALSE;

  uint addr = cpu_ctx->watchpt[wp_index].addr;
  watchtype type = cpu_ctx->watchpt[wp_index].type;
  uint page = addr >> 12;
  uint offset = addr & 0xFFF;
  if (cpu_ctx->state.mem[page] != MNULL) {
      if (type == READ_WRITE || type == READ) {
          cpu_ctx->state.mem[page]->read_bp[offset >> 6] &= ~(1ULL << (offset & 63));
          if (cpu_ctx->state.mem[page]->read_bp[offset >> 6] == 0) {
              cpu_ctx->state.mem[page]->read_bp_summary &= ~(1ULL << (offset >> 6));
          }
          cpu_ctx->state.mem[page]->read_exec_bp[offset >> 6] = cpu_ctx->state.mem[page]->read_bp[offset >> 6]
          | cpu_ctx->state.mem[page]->exec_bp[offset >> 6];
      }
      if (type == READ_WRITE || type == WRITE) {
          cpu_ctx->state.mem[page]->write_bp[offset >> 6] &= ~(1ULL << (offset & 63));
          if (cpu_ctx->state.mem[page]->write_bp[offset >> 6] == 0) {
              cpu_ctx->state.mem[page]->write_bp_summary &= ~(1ULL << (offset >> 6));
          }
      }
  }
}

void
set_wp_active (struct cpu_context *cpu_ctx, int wp_index)
{
  if (wp_index < 0)
    return;
  cpu_ctx->watchpt[wp_index].is_active = TRUE;
  cpu_ctx->watchpt[wp_index].hitted = FALSE;

  uint addr = cpu_ctx->watchpt[wp_index].addr;
  watchtype type = cpu_ctx->watchpt[wp_index].type;
  uint page = addr >> 12;
  uint offset = addr & 0xFFF;
  if (cpu_ctx->state.mem[page] == MNULL) {
    // Allocate the page manually
    cpu_ctx->state.mem[page] = (mem_t *) calloc (1, sizeof (mem_t));
  }
  if (cpu_ctx->state.mem[page] != MNULL) {
      if (type == READ_WRITE || type == READ) {
          cpu_ctx->state.mem[page]->read_bp_summary |= (1ULL << (offset >> 6));
          cpu_ctx->state.mem[page]->read_bp[offset >> 6] |= (1ULL << (offset & 63));
          cpu_ctx->state.mem[page]->read_exec_bp[offset >> 6] = cpu_ctx->state.mem[page]->read_bp[offset >> 6]
          | cpu_ctx->state.mem[page]->exec_bp[offset >> 6];
      }
      if (type == READ_WRITE || type == WRITE) {
          cpu_ctx->state.mem[page]->write_bp_summary |= (1ULL << (offset >> 6));
          cpu_ctx->state.mem[page]->write_bp[offset >> 6] |= (1ULL << (offset & 63));
      }
  }
}

void set_bp_inactive (struct cpu_context *cpu_ctx, int bp_index)
{
  if (bp_index < 0)
    return;
  cpu_ctx->breakpt[bp_index].is_active = FALSE;
  uint addr = cpu_ctx->breakpt[bp_index].addr;
  uint page = addr >> 12;
  uint offset = addr & 0xFFF;
  if (cpu_ctx->state.mem[page] != MNULL) {
    cpu_ctx->state.mem[page]->exec_bp[offset >> 6] &= ~(1ULL << (offset & 63));
    cpu_ctx->state.mem[page]->read_exec_bp[offset >> 6] = cpu_ctx->state.mem[page]->read_bp[offset >> 6]
          | cpu_ctx->state.mem[page]->exec_bp[offset >> 6];
  }
}

void set_bp_active(struct cpu_context *cpu_ctx, int bp_index)
{
    if (bp_index < 0)
        return;
    cpu_ctx->breakpt[bp_index].is_active = TRUE;
    cpu_ctx->breakpt[bp_index].hitted = FALSE;
    uint addr = cpu_ctx->breakpt[bp_index].addr;
    uint page = addr >> 12;
    uint offset = addr & 0xFFF;
    if (cpu_ctx->state.mem[page] == MNULL) {
      // Allocate the page manually
      cpu_ctx->state.mem[page] = (mem_t *) calloc (1, sizeof (mem_t));
    }
    if (cpu_ctx->state.mem[page] != MNULL) {
        cpu_ctx->state.mem[page]->exec_bp[offset >> 6] |= (1ULL << (offset & 63));
        cpu_ctx->state.mem[page]->read_exec_bp[offset >> 6] = cpu_ctx->state.mem[page]->read_bp[offset >> 6]
                | cpu_ctx->state.mem[page]->exec_bp[offset >> 6];
    }

}


void clear_all_wp_hits(struct cpu_context *cpu_ctx) {
    for (int i = 0; i < cpu_ctx->n_watchpts; i++) {
        cpu_ctx->watchpt[i].hitted = false;
    }
}
int add_breakpoint(struct cpu_context *cpu_ctx, uint phys_address) {
    if (cpu_ctx->n_breakpts >= MAX_BREAK) {
        return -1;
    }

    if (find_breakpt(cpu_ctx, phys_address) >= 0) {
        return -1; // already exists
    }

    int bp_index = cpu_ctx->n_breakpts;
    cpu_ctx->breakpt[bp_index].label = NULL;
    cpu_ctx->breakpt[bp_index].addr = phys_address;
    cpu_ctx->breakpt[bp_index].is_active = TRUE;
    cpu_ctx->breakpt[bp_index].hitted = FALSE;
    cpu_ctx->n_breakpts++;

    set_bp_active(cpu_ctx, bp_index);

    return bp_index;
}

int add_watchpoint(struct cpu_context *cpu_ctx, uint phys_address, watchtype type) {
    if (cpu_ctx->n_watchpts >= MAX_BREAK) {
        return -1;
    }

    if (find_watchpt(cpu_ctx, READ_WRITE, phys_address) >= 0) {
        return -1; // already exists
    }

    int wp_index = cpu_ctx->n_watchpts;
    cpu_ctx->watchpt[wp_index].label = NULL;
    cpu_ctx->watchpt[wp_index].addr = phys_address;
    cpu_ctx->watchpt[wp_index].type = type;
    cpu_ctx->watchpt[wp_index].is_active = TRUE;
    cpu_ctx->watchpt[wp_index].hitted = FALSE;
    cpu_ctx->n_watchpts++;

    set_wp_active(cpu_ctx, wp_index);

    return wp_index;
}
