/* sdisasm.c  --  sim1750 interface to dism1750.c */

#include "type.h"
#include "cpu.h"
#include "smemacc.h"
#include "loadfile.h"  /* for function find_labelname() */

extern int dism1750 (struct cpu_state *cpu, char *text, ushort *word);

char *
disassemble (struct cpu_state *cpu)
{
  static char disasm_text[80];
  ushort words[2], as = cpu->reg.sw & 0xF;
  get_raw (cpu, CODE, as, cpu->reg.ic, &words[0]);
  if (cpu->reg.ic < 0xFFFF)
    get_raw (cpu, CODE, as, cpu->reg.ic + 1, &words[1]);
  dism1750 (cpu, disasm_text, words);
  return disasm_text;
}

char *
find_label (struct cpu_state *cpu,int bank, ushort address)
{
  /* TODO - find_labelname also needs to work with cpu (every cpu has a different program and different symbols)*/
  return find_labelname (get_phys_address (cpu, bank, cpu->reg.sw & 0xF, address));
}

