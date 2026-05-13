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
#include <time.h>

#include "arch.h"
#include "status.h"
#include "cpu.h"
#include "smemacc.h"
#include "break.h"
#include "new_cpu.h"

/* Imports */

extern void  dis_reg ();	/* cmd.c */
extern int   sys_int (int);	/* cmd.c */
extern char *disassemble (struct cpu_state *cpu);	/* sdisasm.c */
extern int scan_instructions_from_address(struct cpu_context *cpu_ctx, ushort address); /* jit.c */

extern struct cpu_context *sim_cpu_ctx;

static bool
at_bpt_instruction (struct cpu_state *cpu)
{
  ushort opcode;
  if (! get_raw (cpu, CODE, cpu->reg.sw & 0xF, cpu->reg.ic, &opcode))
    return FALSE;
  return (opcode == 0xFFFF);
}

static int
execute_without_breakpt (struct cpu_context *cpu_ctx)
{
  int status;
  int bpi = cpu_ctx->bpindex;

  cpu_ctx->bpindex = -1;
  //set_bp_inactive (cpu_ctx, bpi);
  status = execute (cpu_ctx);
  //set_bp_active (cpu_ctx, bpi);
  return status;
}

int si_jit_scan (int argc, char *argv[])
{
  unsigned next;

  if (argc > 1)
    {
      sscanf (argv[1], "%x", &next);
      
    }
  else
    next = sim_cpu_ctx->state.reg.ic;
  printf("Scanning instructions starting from address 0x%04X\n", next);
  scan_instructions_from_address(sim_cpu_ctx, next);
  return OKAY;
}
// function to set halt to NO_HALT after debug conditions, so that we can continue execution. we also need to clear watchpoint hits, otherwise we will keep hitting the same watchpoint and never continue execution. we can not just set the watchpoint to inactive, because we want to keep track of it and show it in the watchpoint list, and step over BPT special instruction by incrementing IC, otherwise we will keep hitting the same BPT and never continue execution.
void clear_debug_halt(struct cpu_context *cpu_ctx)
{
  if (cpu_ctx->state.halt & (DBG_WATCHPOINT | DBG_BREAKPOINT | INST_BPT))
  {
    if (cpu_ctx->state.halt & DBG_WATCHPOINT)
    {
      clear_all_wp_hits(cpu_ctx);
      cpu_ctx->state.halt &= ~DBG_WATCHPOINT;
    }
    if (cpu_ctx->state.halt & INST_BPT)
    {
      cpu_ctx->state.reg.ic++;
      cpu_ctx->state.halt &= ~INST_BPT;
    }
    if (cpu_ctx->state.halt & DBG_BREAKPOINT)
    {
      cpu_ctx->state.halt &= ~DBG_BREAKPOINT;
    }
  }
}

void print_halt_reason(struct cpu_context *cpu_ctx)
{
  if (cpu_ctx->state.halt == NO_HALT)
    return;
  if (cpu_ctx->state.halt & DBG_BREAKPOINT)
  {
    lprintf("Hit breakpoint at address 0x%04X : %s\n", cpu_ctx->state.reg.ic, disassemble(&cpu_ctx->state));
    // now print label and info  from cpu_ctx->breakpt[cpu_ctx->bpindex]
    lprintf("Breakpoint %d: physical address 0x%05X, label: %s\n", cpu_ctx->bpindex, cpu_ctx->breakpt[cpu_ctx->bpindex].addr,
            cpu_ctx->breakpt[cpu_ctx->bpindex].label ? cpu_ctx->breakpt[cpu_ctx->bpindex].label : "N/A");
  }
  if (cpu_ctx->state.halt & DBG_WATCHPOINT)
  {
    // loop over all watch points and find all that are hit, and print them out.
    lprintf("Hit watchpoint at physical address 0x%05X : %s, ic after: 0x%04X \n", cpu_ctx->last_phys_ic , disassemble(&cpu_ctx->state), cpu_ctx->state.reg.ic);
    for (int i = 0; i < cpu_ctx->n_watchpts; i++)
    {
        if (cpu_ctx->watchpt[i].is_active && cpu_ctx->watchpt[i].hitted)
        {
            lprintf("Watchpoint %d: address 0x%04X, type: %s, label: %s\n", i, cpu_ctx->watchpt[i].addr,
                    cpu_ctx->watchpt[i].type == READ ? "READ" : (cpu_ctx->watchpt[i].type == WRITE ? "WRITE" : "READ/WRITE"),
                    cpu_ctx->watchpt[i].label ? cpu_ctx->watchpt[i].label : "N/A");
            if (cpu_ctx->watchpt[i].type == READ)
                lprintf("    Old value: 0x%04X\n", cpu_ctx->watchpt[i].old_value);
            else if (cpu_ctx->watchpt[i].type == WRITE)
              lprintf("    Old value: 0x%04X, New value: 0x%04X\n", cpu_ctx->watchpt[i].old_value, cpu_ctx->watchpt[i].new_value);
        }
      }
  }
  else
  {
    lprintf("CPU halted. Reason: ");
    switch (cpu_ctx->state.halt)
    {
        case HALT_ILL_INST:
            lprintf("Illegal Instruction");
            break;

        case INST_BPT:
            lprintf("Instruction Breakpoint at address 0x%04X : %s", cpu_ctx->state.reg.ic, disassemble(&cpu_ctx->state));
            break;
        case HALT_ILL_MEM:
            lprintf("Illegal Memory Access");
            break;
        case HALT_NON_EXEC:
            lprintf("Execution of Non-Executable Memory");
            break;
        case HALT_URS_EMPTY_STACK:
            lprintf("URS called with empty stack");
            break;
        case HALT_INF_LOOP:
            lprintf("Infinite Loop");
            break;
        default:
            lprintf("Unknown reason");
    }
    lprintf("\n");
  }
}

int
si_go_new (int argc, char *argv[])
{
  unsigned next;
  clock_t start, end;
  if (argc > 1)
    {
      sscanf (argv[1], "%x", &next);
      sim_cpu_ctx->state.reg.ic = (ushort) next;
    }
  start = clock();  
  // if we continue after hitting a watchpoint, we need to call clear_all_wp_hits, and also set halt to NO_HALT, otherwise we will keep hitting the same watchpoint and never continue execution. We can not just set the watchpoint to inactive, because we want to keep track of it and show it in the watchpoint list, and also we want to be able to hit it again if the same address is accessed again.
  clear_debug_halt(sim_cpu_ctx);

  while (1)
    {
      if (sys_int (1))
	      return INTERRUPT;
      cpu_mainloop (sim_cpu_ctx, sim_cpu_ctx->state.total_cycles + 1000);
      if (sim_cpu_ctx->state.halt)
      {
        print_halt_reason(sim_cpu_ctx);
        break;
      }
    }
  end = clock();
  printf("\nkick took:  %f seconds\n", ((double) (end - start)) / CLOCKS_PER_SEC);
  return OKAY;
}
const char* get_interrupt_name(ushort intnum);
int
si_snglstp_new (int argc, char *argv[])
{
  int    count = 0;
  bool   step_over = FALSE;
  ushort target_addr;

  if (argc > 1)
    {
      if (*argv[1] == '*')
	{
	  step_over = TRUE;
	  target_addr = sim_cpu_ctx->state.reg.ic + 2;
    
	}
      else
	sscanf (argv[1], "%d", &count);
    }
  else
  {
    count = 1;
  }

  clear_debug_halt(sim_cpu_ctx);

  while ((step_over && sim_cpu_ctx->state.reg.ic != target_addr) || (!step_over && count-- > 0))
  {
	  if (sys_int (1))
	    return (INTERRUPT);
      int old_interrupt_counter = sim_cpu_ctx->interrupt_counter;
    cpu_mainloop (sim_cpu_ctx, sim_cpu_ctx->state.total_cycles + 1); // at least one cycle will drive it to do exactly one instruction, but if there are pending interrupts, it will also process them, so we might execute more than one instruction, but that is fine for single step.
      if (sim_cpu_ctx->state.halt)
      {
        print_halt_reason(sim_cpu_ctx);
        break;
      }
      if (sim_cpu_ctx->interrupt_counter != old_interrupt_counter)
      {
        lprintf("Processing pending interrupt %s\n", get_interrupt_name(sim_cpu_ctx->last_processed_interrupt));
      }
      info ("\tStep at %04hX : %s", sim_cpu_ctx->state.reg.ic, disassemble (&sim_cpu_ctx->state));
  }

  return (OKAY);
}

int
si_go (int argc, char *argv[])
{
  unsigned next;

  if (argc > 1)
    {
      sscanf (argv[1], "%x", &next);
      sim_cpu_ctx->state.reg.ic = (ushort) next;
    }

  if (at_bpt_instruction (&sim_cpu_ctx->state))
    sim_cpu_ctx->state.reg.ic++;
  else if (sim_cpu_ctx->bpindex >= 0)
    execute_without_breakpt (sim_cpu_ctx);
  clock_t start, end;
  start = clock();
  while (1)
    {
      if (sys_int (1))
	return INTERRUPT;
      if (execute (sim_cpu_ctx) == MEMERR)
	break;
      if (at_bpt_instruction (&sim_cpu_ctx->state))
	{
	  lprintf ("\tBPT at %04hX", sim_cpu_ctx->state.reg.ic);
	  break;
	}
      else if (sim_cpu_ctx->bpindex >= 0)
	{
	  info ("\tBreakpoint at %04hX : %s",
		   sim_cpu_ctx->state.reg.ic, disassemble (&sim_cpu_ctx->state));
	  break;
	}
    }
    end = clock();
  printf("\ngo took:  %f seconds\n", ((double) (end - start)) / CLOCKS_PER_SEC);
  return OKAY;
}

void
dis_reg (struct cpu_state *cpu);
int
si_snglstp (int argc, char *argv[])
{
  int    count = 1;
  bool   step_over = FALSE;
  ushort target_addr;

  if (argc > 1)
    {
      if (*argv[1] == '*')
	{
	  step_over = TRUE;
	  target_addr = sim_cpu_ctx->state.reg.ic + 2;
	}
      else
	sscanf (argv[1], "%d", &count);
    }

  if (at_bpt_instruction (&sim_cpu_ctx->state))
  {
    sim_cpu_ctx->state.reg.ic++;
  }

  if (step_over)
    {
      if (sim_cpu_ctx->bpindex >= 0)
	{
	  info ("\tStepping past breakpoint at IC : %04hX   %s",
		   sim_cpu_ctx->state.reg.ic, disassemble (&sim_cpu_ctx->state));
	  execute_without_breakpt (sim_cpu_ctx);
	}
      while (sim_cpu_ctx->state.reg.ic != target_addr)
	{
	  if (sys_int (1))
	    return (INTERRUPT);
	  if (execute (sim_cpu_ctx) == MEMERR)
	    break;
	  if (at_bpt_instruction (&sim_cpu_ctx->state))
	    {
	      lprintf ("\tBPT at %04hX", sim_cpu_ctx->state.reg.ic);
	      break;
	    }
	  else if (sim_cpu_ctx->bpindex >= 0)
	    {
	      info ("\tBreakpoint at %04hX : %s",
		       sim_cpu_ctx->state.reg.ic, disassemble (&sim_cpu_ctx->state));
	      break;
	    }
	}
      if (! at_bpt_instruction (&sim_cpu_ctx->state) && sim_cpu_ctx->bpindex < 0)
	info ("\tStep at %04hX : %s", sim_cpu_ctx->state.reg.ic, disassemble (&sim_cpu_ctx->state));
    }
  else
    {
      while (count-- > 0)
	{
    dis_reg(&sim_cpu_ctx->state);
	  if (sys_int (1))
	    return (INTERRUPT);
	  if (at_bpt_instruction (&sim_cpu_ctx->state))
	    {
	      info ("\tStepping past BPT at IC : %04hX", sim_cpu_ctx->state.reg.ic);
	      sim_cpu_ctx->state.reg.ic++;
	    }
	  else if (sim_cpu_ctx->bpindex >= 0)
	    {
	      info ("\tStepping past breakpoint at IC : %04hX   %s",
		       sim_cpu_ctx->state.reg.ic, disassemble (&sim_cpu_ctx->state));
	      execute_without_breakpt (sim_cpu_ctx);
	    }
	  else if (execute (sim_cpu_ctx) == MEMERR)
	    break;
	  info ("\tIC : %04hX   %s", sim_cpu_ctx->state.reg.ic, disassemble (&sim_cpu_ctx->state));
	}
    }

  return (OKAY);
}


int
si_trace (int argc, char *argv[])
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

      if (at_bpt_instruction (&sim_cpu_ctx->state))
	sim_cpu_ctx->state.reg.ic++;
      else if (sim_cpu_ctx->bpindex >= 0)
	execute_without_breakpt (sim_cpu_ctx);
      else if (execute (sim_cpu_ctx) == MEMERR)
	break;
      lprintf ("\tIC : %04hX   %s", sim_cpu_ctx->state.reg.ic, disassemble (&sim_cpu_ctx->state));
      dis_reg (0);
    }

  return (OKAY);
}


int
si_bt (int argc, char *argv[])
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

  if (count > sim_cpu_ctx->bt_cnt)
    count = sim_cpu_ctx->bt_cnt;

  /* save current regs */
  save = sim_cpu_ctx->state.reg;
  /* step back through backtrace buffer */
  t = sim_cpu_ctx->bt_next - back;
  if (t < 0)
    t += BT_SIZE;

  while (count-- > 0)
    {
      sim_cpu_ctx->state.reg = sim_cpu_ctx->bt_buff [t];
      lprintf ("\tIC : %04hX   %s", sim_cpu_ctx->state.reg.ic, disassemble (&sim_cpu_ctx->state));
      dis_reg (0);

      t++;
      if (t >= BT_SIZE)
        t = 0;
    }

  /* restore old regs */
  sim_cpu_ctx->state.reg = save;

  return (OKAY);
}


