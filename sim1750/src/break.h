/* break.h  --  exports of break.c */

#ifndef _BREAK_H
#define _BREAK_H


#include "cpu_ctx.h"




extern int  find_breakpt (struct cpu_context *cpu_ctx, breaktype type, uint phys_address);
extern void set_inactive (struct cpu_context *cpu_ctx, int bp_index);
extern void set_active   (struct cpu_context *cpu_ctx, int bp_index);

extern int  si_brkset   (int argc, char *argv[]);
extern int  si_brklist  (int argc, char *argv[]);
extern int  si_brkclear (int argc, char *argv[]);
extern int  si_brksave  (int argc, char *argv[]);

#endif
