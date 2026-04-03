/***************************************************************************/
/*                                                                         */
/* Project   :        sim1750 -- Mil-Std-1750 Software Simulator           */
/*                                                                         */
/* Component :        break.c -- breakpoint handling functions             */
/*                                                                         */
/* Copyright :         (C) Daimler-Benz Aerospace AG, 1994-97              */
/*                         (C) 2017 Oliver M. Kellogg                      */
/* Contact   :            okellogg@users.sourceforge.net                   */
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
#include <string.h>

#include "cpu_ctx.h"
#include "status.h"
#include "utils.h"
#include "cmd.h"	/* for function parse_address() */
#include "loadfile.h"	/* for the loadfile_type variable */
#include "tekops.h"	/* for function find_tek_address() */
#include "coffops.h"	/* for function find_coff_address() */

#include "break.h"


extern struct cpu_context *sim_cpu_ctx;



/* Return breakpoint index if breakpoint found for given
   type/bank/address_state/logical_address, or -1 if no breakpoint found. */
int
find_breakpt (struct cpu_context *cpu_ctx, breaktype type, uint phys_address)
{
  int i = cpu_ctx->n_breakpts;

  while (i-- > 0)
    {
      if (cpu_ctx->breakpt[i].is_active && cpu_ctx->breakpt[i].addr == phys_address
	  && (cpu_ctx->breakpt[i].type == READ_WRITE || cpu_ctx->breakpt[i].type == type))
	break;
    }
  return (i);
}

void
set_inactive (struct cpu_context *cpu_ctx, int bp_index)
{
  if (bp_index < 0)
    return;
  cpu_ctx->breakpt[bp_index].is_active = FALSE;

  uint addr = cpu_ctx->breakpt[bp_index].addr;
  breaktype type = cpu_ctx->breakpt[bp_index].type;
  uint page = addr >> 12;
  uint offset = addr & 0xFFF;
  if (cpu_ctx->state.mem[page] != MNULL) {
      if (type == READ_WRITE || type == READ) {
          cpu_ctx->state.mem[page]->read_exec_bp[offset >> 6] &= ~(1ULL << (offset & 63));
          if (cpu_ctx->state.mem[page]->read_exec_bp[offset >> 6] == 0) {
              cpu_ctx->state.mem[page]->read_exec_bp_summary &= ~(1ULL << (offset >> 6));
          }
      }
      if (type == READ_WRITE || type == WRITE) {
          cpu_ctx->state.mem[page]->write_bp[offset >> 6] &= ~(1ULL << (offset & 63));
          if (cpu_ctx->state.mem[page]->write_bp[offset >> 6] == 0) {
              cpu_ctx->state.mem[page]->write_bp_summary &= ~(1ULL << (offset >> 6));
          }
      }
  }
}

void
set_active (struct cpu_context *cpu_ctx, int bp_index)
{
  if (bp_index < 0)
    return;
  cpu_ctx->breakpt[bp_index].is_active = TRUE;

  uint addr = cpu_ctx->breakpt[bp_index].addr;
  breaktype type = cpu_ctx->breakpt[bp_index].type;
  uint page = addr >> 12;
  uint offset = addr & 0xFFF;
  if (cpu_ctx->state.mem[page] != MNULL) {
      if (type == READ_WRITE || type == READ) {
          cpu_ctx->state.mem[page]->read_exec_bp_summary |= (1ULL << (offset >> 6));
          cpu_ctx->state.mem[page]->read_exec_bp[offset >> 6] |= (1ULL << (offset & 63));
      }
      if (type == READ_WRITE || type == WRITE) {
          cpu_ctx->state.mem[page]->write_bp_summary |= (1ULL << (offset >> 6));
          cpu_ctx->state.mem[page]->write_bp[offset >> 6] |= (1ULL << (offset & 63));
      }
  }
}


int
si_brkset (int argc, char **argv)
{
  uint address;
  breaktype type = READ_WRITE;

  if (argc <= 1)
    return error ("address argument missing");
  if (sim_cpu_ctx->n_breakpts >= MAX_BREAK)
    return error ("too many breakpoints");
  if (parse_address (sim_cpu_ctx, argv[1], &address) != OKAY)
    {
      if (isalpha (*argv[1]) || *argv[1] == '_')
        {
          int addr = find_address (argv[1]);
          if (addr < 0)
            return error ("label name not found");
          address = (uint) addr;
        }
      else
        return error ("invalid address syntax");
    }
  if (argc > 2)
    {
      char c = *argv[2];
      if (c == 'r')
        {
          if (*(argv[2] + 1) != 'w')
            type = READ;
        }
      else if (c == 'w')
        type = WRITE;
      else
        return error ("unknown brkpt. type %s (allowed values: R or W)",
			 argv[2]);
    }
  if (find_breakpt (sim_cpu_ctx, READ_WRITE, address) >= 0)
    return error ("breakpoint already set");
  sim_cpu_ctx->breakpt[sim_cpu_ctx->n_breakpts].type = type;
  sim_cpu_ctx->breakpt[sim_cpu_ctx->n_breakpts].addr = address;
  sim_cpu_ctx->breakpt[sim_cpu_ctx->n_breakpts].is_active = TRUE;
  sim_cpu_ctx->n_breakpts++;

  /* Register O(1) Execution/Watchpoint Breakpoints */
  uint page = address >> 12;
  uint offset = address & 0xFFF;
  if (sim_cpu_ctx->state.mem[page] == MNULL) {
      // Allocate the page manually
      sim_cpu_ctx->state.mem[page] = (mem_t *) calloc (1, sizeof (mem_t));
  }
  if (sim_cpu_ctx->state.mem[page] != MNULL) {
      if (type == READ_WRITE || type == READ) {
          sim_cpu_ctx->state.mem[page]->read_exec_bp_summary |= (1ULL << (offset >> 6));
          sim_cpu_ctx->state.mem[page]->read_exec_bp[offset >> 6] |= (1ULL << (offset & 63));
      }
      if (type == READ_WRITE || type == WRITE) {
          sim_cpu_ctx->state.mem[page]->write_bp_summary |= (1ULL << (offset >> 6));
          sim_cpu_ctx->state.mem[page]->write_bp[offset >> 6] |= (1ULL << (offset & 63));
      }
  }

  return OKAY;
}

static const char *typestr[] = { "RW", "R", "W" };

int
si_brklist (int argc, char **argv)
{
  int i;

  if (sim_cpu_ctx->n_breakpts == 0)
    return error ("no breakpoints set");
  lprintf ("\n\t\tBreakpoint List\n");
  for (i = 0; i < sim_cpu_ctx->n_breakpts; i++)
    lprintf ("\t%05lX  %s\n", sim_cpu_ctx->breakpt[i].addr, typestr[sim_cpu_ctx->breakpt[i].type]);

  return (OKAY);
}


int
si_brkclear (int argc, char **argv)
{
  int i;
  uint addr;

  if (argc <= 1)
    return error ("address argument missing");
  if (sim_cpu_ctx->n_breakpts == 0)
    return error ("no breakpoints set");
  if (*argv[1] == '*')
    {
      sim_cpu_ctx->n_breakpts = 0;
      // Clear O(1) structures
      for (uint p = 0; p < N_PAGES; p++) {
          if (sim_cpu_ctx->state.mem[p] != MNULL) {
              sim_cpu_ctx->state.mem[p]->read_exec_bp_summary = 0;
              sim_cpu_ctx->state.mem[p]->write_bp_summary = 0;
              for (int b = 0; b < 64; b++) {
                  sim_cpu_ctx->state.mem[p]->read_exec_bp[b] = 0;
                  sim_cpu_ctx->state.mem[p]->write_bp[b] = 0;
              }
          }
      }
      return OKAY;
    }
  if (parse_address (sim_cpu_ctx, argv[1], &addr))
    return info ("invalid address syntax");
  for (i = 0; i < sim_cpu_ctx->n_breakpts; i++)
    if (sim_cpu_ctx->breakpt[i].addr == addr)
      break;
  if (i == sim_cpu_ctx->n_breakpts)
    return info ("\tno breakpoint at that address");

  breaktype type_to_clear = sim_cpu_ctx->breakpt[i].type;
  sim_cpu_ctx->n_breakpts--;
  while (i < sim_cpu_ctx->n_breakpts)
    {
      sim_cpu_ctx->breakpt[i].type = sim_cpu_ctx->breakpt[i+1].type;
      sim_cpu_ctx->breakpt[i].addr = sim_cpu_ctx->breakpt[i+1].addr;
      i++;
    }

  // Determine if there are any remaining breakpoints of the same type at this address
  bool still_has_read = false;
  bool still_has_write = false;
  for (i = 0; i < sim_cpu_ctx->n_breakpts; i++) {
      if (sim_cpu_ctx->breakpt[i].addr == addr && sim_cpu_ctx->breakpt[i].is_active) {
          if (sim_cpu_ctx->breakpt[i].type == READ_WRITE || sim_cpu_ctx->breakpt[i].type == READ) still_has_read = true;
          if (sim_cpu_ctx->breakpt[i].type == READ_WRITE || sim_cpu_ctx->breakpt[i].type == WRITE) still_has_write = true;
      }
  }

  // Clear O(1) structures if no active overlapping breakpoints remain
  uint page = addr >> 12;
  uint offset = addr & 0xFFF;
  if (sim_cpu_ctx->state.mem[page] != MNULL) {
      if ((type_to_clear == READ_WRITE || type_to_clear == READ) && !still_has_read) {
          sim_cpu_ctx->state.mem[page]->read_exec_bp[offset >> 6] &= ~(1ULL << (offset & 63));
          if (sim_cpu_ctx->state.mem[page]->read_exec_bp[offset >> 6] == 0) {
              sim_cpu_ctx->state.mem[page]->read_exec_bp_summary &= ~(1ULL << (offset >> 6));
          }
      }
      if ((type_to_clear == READ_WRITE || type_to_clear == WRITE) && !still_has_write) {
          sim_cpu_ctx->state.mem[page]->write_bp[offset >> 6] &= ~(1ULL << (offset & 63));
          if (sim_cpu_ctx->state.mem[page]->write_bp[offset >> 6] == 0) {
              sim_cpu_ctx->state.mem[page]->write_bp_summary &= ~(1ULL << (offset >> 6));
          }
      }
  }

  return (OKAY);
}


int
si_brksave (int argc, char **argv)
{
  int i;
  FILE *savefile;

  if (argc <= 1)
    return error ("filename missing");
  else
    {
      if ((savefile = fopen (argv[1], "w")) == NULL)
	return error (" can't open save-file  \" %s \"", argv[1]);
      else
	{
	  fprintf (savefile, "#\t\tsaved BREAKPOINT LIST\n");
	  for (i = 0; i < sim_cpu_ctx->n_breakpts; i++)
	    fprintf (savefile, "br  %05X  %s\n",
		     sim_cpu_ctx->breakpt[i].addr, typestr[sim_cpu_ctx->breakpt[i].type]);
	}
    }
  fclose (savefile);
  return (OKAY);
}

