/* exports */

#include "type.h"
#include "cpu.h"

extern bool  parse_address (struct cpu_state *cpu, char *str, uint *phys_address);
extern int   init_system (int mode);
extern int   init_simulator (struct cpu_state *cpu, int mode);
extern int   interpreter (struct cpu_state *cpu, char *startup_batchfile);
extern void  dis_reg (struct cpu_state *cpu);
extern void  int_handler_install ();
extern int   sys_int (int);
extern int   si_go (int argc, char *argv[]);

