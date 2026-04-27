/* peekpoke.h  -- exports of peekpoke.c */

#include "type.h"
struct cpu_state;  /* forward declaration to avoid circularity with cpu.h */
extern bool peek (struct cpu_state *cpu, uint phys_address, ushort *value);
extern void poke (struct cpu_state *cpu, uint phys_address, ushort value);

