/* sdisasm.c  --  sim1750 interface to dism1750.c */

#include "type.h"
#include "arch.h"
#include "smemacc.h"
#include "loadfile.h"  /* for function find_labelname() */

extern int dism1750 (char *, ushort *);

char *
disassemble ()
{
  static char disasm_text[80];
  ushort words[2], as = sim_cpu.reg.sw & 0xF;
  get_raw (CODE, as, sim_cpu.reg.ic, &words[0]);
  if (sim_cpu.reg.ic < 0xFFFF)
    get_raw (CODE, as, sim_cpu.reg.ic + 1, &words[1]);
  dism1750 (disasm_text, words);
  return disasm_text;
}

char *
find_label (int bank, ushort address)
{
  return find_labelname (get_phys_address (bank, sim_cpu.reg.sw & 0xF, address));
}

