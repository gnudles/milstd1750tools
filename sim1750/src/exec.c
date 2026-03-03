/***************************************************************************/
/*                                                                         */
/* Project   :        sim1750 -- Mil-Std-1750 Software Simulator           */
/*                                                                         */
/* Component :         exec.c -- control over simulator execution          */
/*                                                                         */
/* Copyright :         (C) Daimler-Benz Aerospace AG, 1994-97              */
/*                         (C) 2017 Oliver M. Kellogg                      */
/* Contact   :           okellogg@users.sourceforge.net                    */
/*                                                                         */
/* Disclaimer:                                                             */
/*                                                                         */
/*  This program is free software; you can redistribute it and/or modify   */
/*  it under the terms of the GNU General Public License as published by   */
/*  the Free Software Foundation; either version 2 of the License, or      */
/*  (at your option) any later version.                                    */
/*                                                                         */
/*  This program is distributed in the hope that it will be useful,        */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          */
/*  GNU General Public License for more details.                           */
/*                                                                         */
/*  You should have received a copy of the GNU General Public License      */
/*  along with this program; if not, write to the Free Software            */
/*  Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.   */
/*                                                                         */
/***************************************************************************/


#include <stdio.h>
#include <ctype.h>

#include "arch.h"
#include "status.h"
#include "cpu.h"
#include "smemacc.h"
#include "break.h"

/* Imports */

extern void  dis_reg ();	/* cmd.c */
extern int   sys_int (int);	/* cmd.c */
extern char *disassemble (struct cpu_state *cpu);	/* sdisasm.c */


static bool
at_bpt_instruction (struct cpu_state *cpu)
{
  ushort opcode;
  if (! get_raw (CODE, cpu->reg.sw & 0xF, cpu->reg.ic, &opcode))
    return FALSE;
  return (opcode == 0xFFFF);
}

static int
execute_without_breakpt (struct cpu_state *cpu)
{
  int status;
  int bpi = cpu->bpindex;

  cpu->bpindex = -1;
  set_inactive (bpi);
  status = execute (cpu);
  set_active (bpi);
  return status;
}


int
si_go (struct cpu_state *cpu, int argc, char *argv[])
{
  unsigned next;

  if (argc > 1)
    {
      sscanf (argv[1], "%x", &next);
      cpu->reg.ic = (ushort) next;
    }

  if (at_bpt_instruction (cpu))
    cpu->reg.ic++;
  else if (cpu->bpindex >= 0)
    execute_without_breakpt (cpu);
  while (1)
    {
      if (sys_int (1))
	return INTERRUPT;
      if (execute (cpu) == MEMERR)
	break;
      if (at_bpt_instruction (cpu))
	{
	  lprintf ("\tBPT at %04hX", cpu->reg.ic);
	  break;
	}
      else if (cpu->bpindex >= 0)
	{
	  info ("\tBreakpoint at %04hX : %s",
		   cpu->reg.ic, disassemble (cpu));
	  break;
	}
    }
  return OKAY;
}


int
si_snglstp (struct cpu_state *cpu, int argc, char *argv[])
{
  int    count = 1;
  bool   step_over = FALSE;
  ushort target_addr;

  if (argc > 1)
    {
      if (*argv[1] == '*')
	{
	  step_over = TRUE;
	  target_addr = cpu->reg.ic + 2;
	}
      else
	sscanf (argv[1], "%d", &count);
    }

  if (at_bpt_instruction (cpu))
    cpu->reg.ic++;

  if (step_over)
    {
      if (cpu->bpindex >= 0)
	{
	  info ("\tStepping past breakpoint at IC : %04hX   %s",
		   cpu->reg.ic, disassemble (cpu));
	  execute_without_breakpt (cpu);
	}
      while (cpu->reg.ic != target_addr)
	{
	  if (sys_int (1))
	    return (INTERRUPT);
	  if (execute (cpu) == MEMERR)
	    break;
	  if (at_bpt_instruction (cpu))
	    {
	      lprintf ("\tBPT at %04hX", cpu->reg.ic);
	      break;
	    }
	  else if (cpu->bpindex >= 0)
	    {
	      info ("\tBreakpoint at %04hX : %s",
		       cpu->reg.ic, disassemble (cpu));
	      break;
	    }
	}
      if (! at_bpt_instruction (cpu) && cpu->bpindex < 0)
	info ("\tStep at %04hX : %s", cpu->reg.ic, disassemble (cpu));
    }
  else
    {
      while (count-- > 0)
	{
	  if (sys_int (1))
	    return (INTERRUPT);
	  if (at_bpt_instruction (cpu))
	    {
	      info ("\tStepping past BPT at IC : %04hX", cpu->reg.ic);
	      cpu->reg.ic++;
	    }
	  else if (cpu->bpindex >= 0)
	    {
	      info ("\tStepping past breakpoint at IC : %04hX   %s",
		       cpu->reg.ic, disassemble (cpu));
	      execute_without_breakpt (cpu);
	    }
	  else if (execute (cpu) == MEMERR)
	    break;
	  info ("\tIC : %04hX   %s", cpu->reg.ic, disassemble (cpu));
	}
    }

  return (OKAY);
}


int
si_trace (struct cpu_state *cpu, int argc, char *argv[])
{
  int count = 1;

  if (argc > 1)
    {
      if (argc > 2)
	error ("excess arguments ignored");
      else
	sscanf (argv[1], "%x", &count);
    }

  while (count-- > 0)
    {
      if (sys_int (1))
	return (INTERRUPT);

      if (at_bpt_instruction (cpu))
	cpu->reg.ic++;
      else if (cpu->bpindex >= 0)
	execute_without_breakpt (cpu);
      else if (execute (cpu) == MEMERR)
	break;
      lprintf ("\tIC : %04hX   %s", cpu->reg.ic, disassemble (cpu));
      dis_reg (0);
    }

  return (OKAY);
}


int
si_bt (struct cpu_state *cpu, int argc, char *argv[])
{
  /* Backtrace the given number of instructions */

  int count = 10;
  int back = 10;
  struct regs save;
  int t;

  if (argc > 2)
    {
      if (argc > 3)
        error ("excess arguments ignored");
      else
        sscanf (argv[1], "%x %x", &back, &count);
    }

  if (count > cpu->bt_cnt)
    count = cpu->bt_cnt;

  /* save current regs */
  save = cpu->reg;

  /* step back through backtrace buffer */
  t = cpu->bt_next - back;
  if (t < 0)
    t += BT_SIZE;

  while (count-- > 0)
    {
      cpu->reg = cpu->bt_buff [t];
      lprintf ("\tIC : %04hX   %s", cpu->reg.ic, disassemble (cpu));
      dis_reg (0);

      t++;
      if (t >= BT_SIZE)
        t = 0;
    }

  /* restore old regs */
  cpu->reg = save;

  return (OKAY);
}


