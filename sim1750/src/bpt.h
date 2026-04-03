#ifndef _BPT_H
#define _BPT_H

#include "type.h"
typedef enum { READ_WRITE, READ, WRITE } breaktype;
//typedef enum {  READ = 0x1, WRITE = 0x2, EXEC = 0x4 } breaktype;
/* Maximum number of breakpoints: */
#define MAX_BREAK 64

struct breakpoint_t
  {
    breaktype type;
    //int bp_type;
    uint addr;
    char *label;
    bool is_active;
  };	/* breakpoint array */

#endif // _BPT_H