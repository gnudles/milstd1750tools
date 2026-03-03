/* smemacc.h  --  export prototypes of memacc.c */

#include "type.h"

extern uint get_phys_address (struct cpu_state *cpu, int bank, ushort as, ushort log_addr);
extern bool  get_raw (struct cpu_state *cpu, int bank, ushort as, ushort address, ushort *value);
extern void  store_raw (struct cpu_state *cpu, int bank, ushort as, ushort address, ushort value);
