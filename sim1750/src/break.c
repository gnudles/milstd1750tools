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
#include "loadfile.h"
#include "tekops.h"	/* for function find_tek_address() */
#include "coffops.h"	/* for function find_coff_address() */

#include "break.h"


extern struct cpu_context *sim_cpu_ctx;



int
si_brkset (int argc, char **argv)
{
  uint address;
  const char *label = NULL;

  if (argc <= 1)
    return error ("address argument missing");
  if (sim_cpu_ctx->n_breakpts >= MAX_BREAK)
    return error ("too many breakpoints");
 
  if (parse_address (sim_cpu_ctx, argv[1], &address) != OKAY)
    {
      if (isalpha (*argv[1]) || *argv[1] == '_')
        {
          int addr = find_address (sim_cpu_ctx, argv[1]);
          if (addr < 0)
            return error ("label name not found");
          address = (uint) addr;
          label = argv[1];
        }
      else
        return error ("invalid address syntax");
    }

  if (find_breakpt (sim_cpu_ctx, address) >= 0)
    return error ("breakpoint already set");
  int bp_index = sim_cpu_ctx->n_breakpts;
  if (label)
    sim_cpu_ctx->breakpt[bp_index].label = strdup(label);
  sim_cpu_ctx->breakpt[bp_index].addr = address;
  sim_cpu_ctx->n_breakpts++;
  set_bp_active(sim_cpu_ctx, bp_index);
  return OKAY;
}



int
si_wtchset (int argc, char **argv)
{
  uint address;
  watchtype type = READ_WRITE;
  const char *label = NULL;

  if (argc <= 1)
    return error ("address argument missing");
  if (sim_cpu_ctx->n_watchpts >= MAX_BREAK)
    return error ("too many watchpoints");
  if (parse_address (sim_cpu_ctx, argv[1], &address) != OKAY)
    {
      if (isalpha (*argv[1]) || *argv[1] == '_')
        {
          int addr = find_address (sim_cpu_ctx, argv[1]);
          if (addr < 0)
            return error ("label name not found");
          address = (uint) addr;
          label = argv[1];
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
  if (find_watchpt (sim_cpu_ctx, READ_WRITE, address) >= 0)
    return error ("watchpoint already set");
  int wp_index = sim_cpu_ctx->n_watchpts;
  sim_cpu_ctx->watchpt[wp_index].type = type;
  sim_cpu_ctx->watchpt[wp_index].addr = address;
  if (label)
    sim_cpu_ctx->watchpt[wp_index].label = strdup(label);
  sim_cpu_ctx->n_watchpts++;
  set_wp_active(sim_cpu_ctx, wp_index);
  return OKAY;
}

static const char *typestr[] = { "", "R", "W", "RW" };

int
si_brklist (int argc, char **argv)
{
  int i;

  if (sim_cpu_ctx->n_breakpts == 0 && sim_cpu_ctx->n_watchpts == 0)
    return error ("no breakpoints or watchpoints set");
  lprintf ("\n\t\tBreakpoint List\n");
  for (i = 0; i < sim_cpu_ctx->n_breakpts; i++)
    lprintf ("\t%05lX  EXEC (%s)\n", sim_cpu_ctx->breakpt[i].addr, sim_cpu_ctx->breakpt[i].label);
  // now list watchpoints
  for (i = 0; i < sim_cpu_ctx->n_watchpts; i++)
    lprintf ("\t%05lX  %s (%s)\n", sim_cpu_ctx->watchpt[i].addr, typestr[sim_cpu_ctx->watchpt[i].type], sim_cpu_ctx->watchpt[i].label);
  return (OKAY);
}


int
si_brkclear (int argc, char **argv)
{
  int i;
  uint addr;

  if (argc <= 1)
    return error ("address argument missing");
  if (sim_cpu_ctx->n_breakpts == 0 && sim_cpu_ctx->n_watchpts == 0)
    return error ("no breakpoints or watchpoints set");
  if (*argv[1] == '*')
    {
      /* free all label memories */
      for (int i = 0; i < sim_cpu_ctx->n_watchpts; i++) {
          if (sim_cpu_ctx->watchpt[i].label) {
              free (sim_cpu_ctx->watchpt[i].label);
          }
      }
      for (int i = 0; i < sim_cpu_ctx->n_breakpts; i++) {
          if (sim_cpu_ctx->breakpt[i].label) {
              free (sim_cpu_ctx->breakpt[i].label);
          }
      }
      sim_cpu_ctx->n_breakpts = 0;
      sim_cpu_ctx->n_watchpts = 0;
      // Clear O(1) structures
      for (uint p = 0; p < N_PAGES; p++) {
          if (sim_cpu_ctx->state.mem[p] != MNULL) {
              sim_cpu_ctx->state.mem[p]->read_bp_summary = 0;
              sim_cpu_ctx->state.mem[p]->write_bp_summary = 0;
              for (int b = 0; b < 64; b++) {
                  sim_cpu_ctx->state.mem[p]->read_exec_bp[b] = 0;
                  sim_cpu_ctx->state.mem[p]->read_bp[b] = 0;
                  sim_cpu_ctx->state.mem[p]->exec_bp[b] = 0;
                  sim_cpu_ctx->state.mem[p]->write_bp[b] = 0;
              }
          }
      }
      return OKAY;
    }
  if (parse_address (sim_cpu_ctx, argv[1], &addr))
    return info ("invalid address syntax");
  int bp_index = -1;
  int wp_index = -1;
  for (i = 0; i < sim_cpu_ctx->n_breakpts; i++)
    if (sim_cpu_ctx->breakpt[i].addr == addr)
    {
      bp_index = i;
      break;
    }
  for (i = 0; i < sim_cpu_ctx->n_watchpts; i++)
    if (sim_cpu_ctx->watchpt[i].addr == addr)
    {
      wp_index = i;
      break;
    }
  if (bp_index == -1 && wp_index == -1)
    return info ("\tno breakpoint or watchpoint at that address");
  watchtype type_to_clear;
  uint page = addr >> 12;
  uint offset = addr & 0xFFF;
  if (wp_index >=0)
  {
    type_to_clear = sim_cpu_ctx->watchpt[wp_index].type;
    if (sim_cpu_ctx->watchpt[wp_index].label)
    {
      free(sim_cpu_ctx->watchpt[wp_index].label);
    }
    if (sim_cpu_ctx->state.mem[page] != MNULL) {
        if (type_to_clear == READ_WRITE || type_to_clear == READ) {
            sim_cpu_ctx->state.mem[page]->read_bp[offset >> 6] &= ~(1ULL << (offset & 63));
            if (sim_cpu_ctx->state.mem[page]->read_bp[offset >> 6] == 0) {
                sim_cpu_ctx->state.mem[page]->read_bp_summary &= ~(1ULL << (offset >> 6));
            }
            sim_cpu_ctx->state.mem[page]->read_exec_bp[offset >> 6] = sim_cpu_ctx->state.mem[page]->read_bp[offset >> 6]
          | sim_cpu_ctx->state.mem[page]->exec_bp[offset >> 6];
        }
        if (type_to_clear == READ_WRITE || type_to_clear == WRITE) {
            sim_cpu_ctx->state.mem[page]->write_bp[offset >> 6] &= ~(1ULL << (offset & 63));
            if (sim_cpu_ctx->state.mem[page]->write_bp[offset >> 6] == 0) {
                sim_cpu_ctx->state.mem[page]->write_bp_summary &= ~(1ULL << (offset >> 6));
            }
        }
    }
    sim_cpu_ctx->n_watchpts--;
    while (wp_index < sim_cpu_ctx->n_watchpts)
    {
      sim_cpu_ctx->watchpt[wp_index] = sim_cpu_ctx->watchpt[wp_index+1];
      wp_index++;
    }
  }
  if (bp_index >= 0)
  {
    sim_cpu_ctx->state.mem[page]->exec_bp[offset >> 6] &= ~(1ULL << (offset & 63));
    sim_cpu_ctx->state.mem[page]->read_exec_bp[offset >> 6] = sim_cpu_ctx->state.mem[page]->read_bp[offset >> 6]
          | sim_cpu_ctx->state.mem[page]->exec_bp[offset >> 6];
    if (sim_cpu_ctx->breakpt[bp_index].label)
    {
      free(sim_cpu_ctx->breakpt[bp_index].label);
    }
    sim_cpu_ctx->n_breakpts--;
    while (bp_index < sim_cpu_ctx->n_breakpts)
    {
      sim_cpu_ctx->breakpt[bp_index] = sim_cpu_ctx->breakpt[bp_index+1];
      bp_index++;
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
    {
      if (sim_cpu_ctx->breakpt[i].label)
        fprintf (savefile, "br  %s\n",
         sim_cpu_ctx->breakpt[i].label);
      else
	    fprintf (savefile, "br  %05X\n",
		     sim_cpu_ctx->breakpt[i].addr);
    }
    for (i = 0; i < sim_cpu_ctx->n_watchpts; i++)
    {
      if (sim_cpu_ctx->watchpt[i].label)
        fprintf (savefile, "wt  %s %s\n",
         sim_cpu_ctx->watchpt[i].label, typestr[sim_cpu_ctx->watchpt[i].type]);
      else
      fprintf (savefile, "wt  %05X %s\n",
         sim_cpu_ctx->watchpt[i].addr, typestr[sim_cpu_ctx->watchpt[i].type]);
    }
	}
    }
  fclose (savefile);
  return (OKAY);
}

