/* sdisasm.c  --  sim1750 interface to dism1750.c */

#include "type.h"
#include "cpu.h"
#include "smemacc.h"
#include "loadfile.h"  /* for function find_labelname() */

extern int dism1750 (struct cpu_context *cpu, char *text, ushort *word);

char *
disassemble (struct cpu_context *cpu)
{
  static char disasm_text[80];
  ushort words[2], as = cpu->state.reg.sw & 0xF;
  get_raw (&cpu->state, CODE, as, cpu->state.reg.ic, &words[0]);
  if (cpu->state.reg.ic < 0xFFFF)
    get_raw (&cpu->state, CODE, as, cpu->state.reg.ic + 1, &words[1]);
  dism1750 (cpu, disasm_text, words);
  return disasm_text;
}

char *
find_label (struct cpu_context *cpu,int bank, ushort address)
{
  /* TODO - find_labelname also needs to work with cpu (every cpu has a different program and different symbols)*/
  return find_labelname (cpu, get_phys_address (&cpu->state, bank, cpu->state.reg.sw & 0xF, address));
}
