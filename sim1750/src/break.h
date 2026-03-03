/* break.h  --  exports of break.c */

#ifndef _BREAK_H
#define _BREAK_H
#include "cpu.h"
#define _BREAK_H

#include "type.h"
#include "cpu.h"

typedef enum { READ_WRITE, READ, WRITE } breaktype;

extern int  find_breakpt (breaktype type, uint phys_address);
extern void set_inactive (int bp_index);
extern void set_active   (int bp_index);

extern int  si_brkset   (struct cpu_state *cpu, int argc, char *argv[]);
extern int  si_brklist  (struct cpu_state *cpu, int argc, char *argv[]);
extern int  si_brkclear (struct cpu_state *cpu, int argc, char *argv[]);
extern int  si_brksave  (struct cpu_state *cpu, int argc, char *argv[]);

#endif
