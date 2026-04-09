#ifndef _BPT_H
#define _BPT_H

#include "type.h"
typedef enum { READ_WRITE = 3, READ = 1, WRITE = 2 } watchtype;
//typedef enum {  READ = 0x1, WRITE = 0x2, EXEC = 0x4 } breaktype;
/* Maximum number of breakpoints: */
#define MAX_BREAK 64

struct breakpoint_t
  {

    uint addr;
    char *label;
    bool is_active;
    bool hitted;
  };	/* breakpoint array */
struct watchpoint_t
  {
    watchtype type;
    uint addr;
    char *label;
    ushort old_value;
    ushort new_value;
    bool is_active;
    bool hitted;

  };	/* watchpoint array */
struct cpu_context; /* forward declaration to avoid circularity with cpu_ctx.h */
int
find_breakpt (struct cpu_context *cpu_ctx, uint phys_address);
int
find_watchpt (struct cpu_context *cpu_ctx, watchtype type, uint phys_address);
void
set_wp_inactive (struct cpu_context *cpu_ctx, int wp_index);
void
set_wp_active (struct cpu_context *cpu_ctx, int wp_index);
void set_bp_inactive (struct cpu_context *cpu_ctx, int bp_index);
void set_bp_active   (struct cpu_context *cpu_ctx, int bp_index);
void clear_all_wp_hits(struct cpu_context *cpu_ctx);

#endif // _BPT_H