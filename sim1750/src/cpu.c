/***************************************************************************/
/*                                                                         */
/* Project   :        sim1750 -- Mil-Std-1750 Software Simulator           */
/*                                                                         */
/* Component :         cpu.c -- Central Processing Unit engine             */
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
/*#include <stdlib.h>*/
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "xiodef.h"
#include "targsys.h"
#include "status.h"
#include "stime.h"
#include "arith.h"
#include "flt1750.h"
#include "peekpoke.h"
#include "smemacc.h"
#ifndef BSVC
#include "break.h"
#endif
#include "cpu.h"

/* Exports */

//int   execute (void);
//uint instcnt;		     /* Total number of instructions executed */
//struct regs simreg;	     /* The 1750 register file */
int   bpindex = -1;	     /* Index of breakpoint when hitting one */
			     /* (unused in BSVC) */

/* Total execution time in uSec since go command */
//double total_time_in_us = 0.0;

/* 
 * Back trace buffer
 */
//struct regs bt_buff [BT_SIZE];
//int bt_next;
//int bt_cnt;


static void
add_to_backtrace (struct cpu_context *cpu_ctx)
{
  cpu_ctx->bt_buff [cpu_ctx->bt_next] = cpu_ctx->state.reg;
  cpu_ctx->bt_next++;
  if (cpu_ctx->bt_next >= BT_SIZE)
    cpu_ctx->bt_next = 0;

  if (cpu_ctx->bt_cnt < BT_SIZE)
    cpu_ctx->bt_cnt++;
}



/* Each MMU page points to 4k words of memory.
   Indexing of a page is done by: pagereg[D/I][AS][logaddr_hinibble].ppa
   where D/I is the processsor Data/Instruction signal, AS is the
   Address State (least significant nibble of the Status Word), and
   logaddr_hinibble is the most significant nibble of the 16 bit logical
   address. The pagereg array, thus indexed, yields an 8-bit number, which
   becomes the most significant byte in the 20-bit physical address.
   The remaining bits of the physical address, i.e. the lower 12 bits,
   are a copy of the least significant 12 bits of the logical address.

   Default (startup) setting of pagereg[][][].ppa is done in
   init_cpu(). It can be changed in any desired way.
   By default, the MMU behaves just as though it were not there at all.
 */

/*struct mmureg pagereg[2][16][16];*/

/* CPU & MMU initialization function */

void
init_cpu (struct cpu_state *cpu)
{
  int as, i = 0;

  /* initialize MMU regs */
  memset ((void *) cpu->pagereg, 0, 512 * sizeof (ushort));
  /* initialize pagereg.ppa to "quasi-non-MMU". */
  for (as = 0; as <= 15; as++)
    {
      int logaddr_hinibble = 0;
      for (; logaddr_hinibble <= 0xF; logaddr_hinibble++)
        {
	      cpu->pagereg[CODE][as][logaddr_hinibble].ppa = i;
	      cpu->pagereg[DATA][as][logaddr_hinibble].ppa = i++;
        }
    }
  /* Write zeros to 1750 regs */
  memset ((void *) &cpu->reg, 0, sizeof (struct regs));
  /* Reset counter of total instructions executed */
  cpu->instcnt = 0;
  /* Reset counter of total simulation time */
  cpu->total_cycles = 0;
  cpu->timers.one_tatick_in_ns = 0;
  cpu->timers.one_tbtick_in_tatix = 0;
  cpu->timers.one_gotick_in_10usec = 0;
}


/********** functions for handling simulation time and interrupts ***********/

static void
workout_timing (struct cpu_state *cpu, int cycles)
{

#ifdef MAS281
#define TIMER_A_LIMIT_IN_NS 20000
  /* CCFN: Timer A limit modified to allow for 50KHz clock on ERA board */
#else
#define TIMER_A_LIMIT_IN_NS 10000
#endif

  if (cpu->disable_timers)
    return;

  cpu->timers.one_tatick_in_ns += uP_CYCLE_IN_NS * cycles;

  while (cpu->timers.one_tatick_in_ns >= TIMER_A_LIMIT_IN_NS)
    {
      cpu->timers.one_tatick_in_ns -= TIMER_A_LIMIT_IN_NS;
      if (cpu->reg.sys & SYS_TA)
	{
	  if (cpu->reg.timer[TIM_A] == 0xFFFF)
	    {
	      cpu->reg.timer[TIM_A] = 0;
	      cpu->reg.pir |= INTR_TA;
	    }
	  else
	    cpu->reg.timer[TIM_A]++;
	}

	  if (cpu->timers.one_tbtick_in_tatix == 9)
	    {
	      cpu->timers.one_tbtick_in_tatix = 0;
        if (cpu->reg.sys & SYS_TB)
        {
          if (cpu->reg.timer[TIM_B] == 0xFFFF)
          {
            cpu->reg.timer[TIM_B] = 0;
            cpu->reg.pir |= INTR_TB;
          }
          else
            cpu->reg.timer[TIM_B]++;
        }
	    }
      else
        cpu->timers.one_tbtick_in_tatix++;
	

      if (++cpu->timers.one_gotick_in_10usec >= GOTIMER_PERIOD_IN_10uSEC)
	{
	  cpu->timers.one_gotick_in_10usec = 0;
	  if (!++cpu->reg.go)         /* GO Watchdog */
	    {
	      cpu->reg.pir |= INTR_MACHERR;   /* machine error         */
	      cpu->reg.ft |= FT_SYSFAULT0;    /* sysfault 0 : watchdog */
	      cpu->reg.go = 0;
	      info ("BARK! goes the watchdog\n");
	    }
	}
    }
}



static int
workout_interrupts (struct cpu_state *cpu)
{
  ushort intnum, pirmask;
  static char *intr_name[] =
    { "Power-Down", "Machine-Error", "User-0", "Floating-Overflow",
      "Integer-Overflow", "Executive-Call", "Floating-Underflow", "Timer-A",
      "User-1", "Timer-B", "User-2", "User-3",
      "IO-Level-1", "User-4", "IO-Level-2", "User-5" };

  if (cpu->reg.pir == 0)
    return 0;

  uint16_t unmaskable = cpu->reg.pir & 0x8400;
  uint16_t maskable_undisable = (cpu->reg.pir & cpu->reg.mk) & 0x4000;
  uint16_t maskable_disableable = (cpu->reg.sys & SYS_INT) ? (cpu->reg.pir & cpu->reg.mk & ~0xC400) : 0;
  uint16_t active_interrupts = unmaskable | maskable_undisable | maskable_disableable;

  if (active_interrupts == 0)
    return 0;

  intnum = count_leading_zeros(active_interrupts);
  pirmask = 1 << (15 - intnum);

  info ("\tInterrupt %2d (%s)", (unsigned) intnum, intr_name[intnum]);
  ushort old_mk = cpu->reg.mk, old_sw = cpu->reg.sw, old_ic = cpu->reg.ic;
  ushort lp, svp;
  ushort as;
  cpu->reg.pir &= ~pirmask;
  cpu->reg.sys &= ~SYS_INT;  /* clear the Master Interrupt Enable */
  /************** Switch to the interrupt context ***************/
  cpu->reg.sw &= 0xFF00;      /* LP and SVP in AS 0, PS 0 */
  get_raw (cpu, DATA, 0, 0x20 + intnum * 2, &lp);
  get_raw (cpu, DATA, 0, 0x21 + intnum * 2, &svp);
  /* get new MK/SW/IC */
  get_raw (cpu, DATA, 0, svp, &cpu->reg.mk);
  get_raw (cpu, DATA, 0, svp + 1, &cpu->reg.sw);
  get_raw (cpu, DATA, 0, svp + 2 + (intnum == 5 ? cpu->bex_index : 0),
           &cpu->reg.ic); /* bex_index: see ex_bex() */
  /* save old MK/SW/IC in new AS */
  as = cpu->reg.sw & 0x000F;
  store_raw (cpu, DATA, as, lp, old_mk);
  store_raw (cpu, DATA, as, lp + 1, old_sw);
  store_raw (cpu, DATA, as, lp + 2, old_ic);
  return 1;
}


/************************** Internal Definitions ****************************/

static char *bankname[] = { "Code", "Data" };

/* Return value of get_word and store_word is one of:
   OKAY, BREAKPT, or MEMERR. */

static int
get_word (struct cpu_context *cpu_ctx, int bank, ushort address, short *data)
{
  ushort al, ak = (cpu_ctx->state.reg.sw >> 4) & 0xF, as = cpu_ctx->state.reg.sw & 0xF;
  uint phys_address;

  if (bank != CODE && bank != DATA)
    {
      error ("FATAL ERR:  bank-number %d invalid\n", bank);
      return MEMERR;
    }
  if (ak != 0)
    {
      al = cpu_ctx->state.pagereg[bank][(int) as][(int)(address >> 12)].al;
      if (al != 0xF && ak != al)
	{
	  cpu_ctx->state.reg.pir |= INTR_MACHERR;
	  cpu_ctx->state.reg.ft |= FT_MEMPROT;
	  error
	   ("MACHINE ERR: AL %hX / AK %hX during %s fetch from %hX:%04hX\n",
		    al, ak, bankname[bank], as, address);
	  return MEMERR;
	}
    }
  if (cpu_ctx->state.pagereg[bank][(int) as][(int)(address >> 12)].e_w && bank == CODE)
    {
      cpu_ctx->state.reg.pir |= INTR_MACHERR;
      cpu_ctx->state.reg.ft |= FT_MEMPROT;
      error
	("MACHINE ERR: attempt to fetch %s from protected address %hX:%04hX\n",
		bankname[bank], as, address);
      return MEMERR;
    }
  phys_address = get_phys_address (&cpu_ctx->state, bank, as, address);
#ifndef BSVC
  /* Check for breakpoint */
  if ((bpindex = find_breakpt (cpu_ctx, READ, phys_address)) >= 0)
    return BREAKPT;
#endif
  if (peek (&cpu_ctx->state, phys_address, (ushort *) data) == 0)
    {
      error ("read error at ic = %04X\n", cpu_ctx->state.reg.ic);
      return MEMERR;
    }
  return OKAY;
}


static int
store_word_data(struct cpu_context *cpu_ctx, ushort address, ushort data)
{
  ushort al, ak = (cpu_ctx->state.reg.sw >> 4) & 0xF, as = cpu_ctx->state.reg.sw & 0xF;
  uint phys_address;

  if (ak != 0)
    {
      al = cpu_ctx->state.pagereg[DATA][(int) as][(int)(address >> 12)].al;
      if (al != 0xF && ak != al)
	{
	  cpu_ctx->state.reg.pir |= INTR_MACHERR;
	  cpu_ctx->state.reg.ft |= FT_MEMPROT;
	  error
	   ("MACHINE ERR: AL=%hX / AK=%hX on storing %s to address %hX:%04hX\n",
		    al, ak, bankname[DATA], as, address);
	  return MEMERR;
	}
    }
  if (cpu_ctx->state.pagereg[DATA][(int) as][(int)(address >> 12)].e_w)
    {
      cpu_ctx->state.reg.pir |= INTR_MACHERR;
      cpu_ctx->state.reg.ft |= FT_MEMPROT;
      error
	("MACHINE ERR: attempt to store %s to protected address %hX:%04hX\n",
		bankname[DATA], as, address);
      return MEMERR;
    }
  phys_address = get_phys_address (&cpu_ctx->state, DATA, as, address);
#ifndef BSVC
  /* Check for breakpoint */
  if ((bpindex = find_breakpt (cpu_ctx, WRITE, phys_address)) >= 0)
    return BREAKPT;
#endif
  poke (&cpu_ctx->state, phys_address, data);
  return OKAY;
}


#define BASEREG(opcode) (ushort) cpu_ctx->state.reg.r[12 + (((opcode) & 0x0300) >> 8)]
#define CHK_RX()        (lower > 0 ? (ushort) cpu_ctx->state.reg.r[lower] : 0)
static int ans;
#define GET(bank,addr,receiver) \
	  if ((ans = get_word (cpu_ctx, bank, addr, receiver)) != OKAY) return ans
#define PUT(addr,emittee)  \
	  if ((ans = store_word_data (cpu_ctx, addr, emittee)) != OKAY) return ans

//static ushort opcode, upper, lower;
/* `upper' and `lower' are bits 8..11 and 12..15 respectively of the opcode */

/*************************** CPU instructions *******************************/

static int
ex_ill (struct cpu_context *cpu_ctx, ushort opcode) /* illegal opcode */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  info ("opcode %04hX not implemented (IC=%04hX)\n", opcode, cpu_ctx->state.reg.ic);
  return (MEMERR);
}

static int
ex_lb (struct cpu_context *cpu_ctx, ushort opcode) /* 0[0-3]xy */
{
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);

  GET (DATA, addr, &cpu_ctx->state.reg.r[2]);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[2], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_LB);
}

static int
ex_dlb (struct cpu_context *cpu_ctx, ushort opcode) /* 0[4-7]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);

  GET (DATA, addr, &cpu_ctx->state.reg.r[0]);
  GET (DATA, addr + 1, &cpu_ctx->state.reg.r[1]);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[0], VAR_LONG);

  cpu_ctx->state.reg.ic++;
  return (nc_DLB);
}

static int
ex_stb (struct cpu_context *cpu_ctx, ushort opcode) /* 0[9-B]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);

  PUT (addr, cpu_ctx->state.reg.r[2]);

  cpu_ctx->state.reg.ic++;
  return (nc_STB);
}

static int
ex_dstb (struct cpu_context *cpu_ctx, ushort opcode) /* 0[A-F]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);

  PUT (addr, cpu_ctx->state.reg.r[0]);
  PUT (addr + 1, cpu_ctx->state.reg.r[1]);

  cpu_ctx->state.reg.ic++;
  return (nc_DSTB);
}

static int
ex_ab (struct cpu_context *cpu_ctx, ushort opcode) /* 1[0-3]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);
  short help;

  GET (DATA, addr, &help);
  arith (&cpu_ctx->state, ARI_ADD, VAR_INT, &cpu_ctx->state.reg.r[2], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_AB);
}

static int
ex_sbb (struct cpu_context *cpu_ctx, ushort opcode) /* 1[4-7]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);
  short help;

  GET (DATA, addr, &help);
  arith (&cpu_ctx->state, ARI_SUB, VAR_INT, &cpu_ctx->state.reg.r[2], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_SBB);
}

static int
ex_mb (struct cpu_context *cpu_ctx, ushort opcode) /* 1[8-B]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);
  short help;

  GET (DATA, addr, &help);
  arith (&cpu_ctx->state, ARI_MUL, VAR_INT, &cpu_ctx->state.reg.r[2], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_MB);
}

static int
ex_db (struct cpu_context *cpu_ctx, ushort opcode) /* 1[C-F]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);
  short help;

  GET (DATA, addr, &help);
  arith (&cpu_ctx->state, ARI_DIV, VAR_INT, &cpu_ctx->state.reg.r[2], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_DB);
}

static int
ex_fab (struct cpu_context *cpu_ctx, ushort opcode) /* 2[0-3]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);
  short help[2];

  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);
  arith (&cpu_ctx->state, ARI_ADD, VAR_FLOAT, &cpu_ctx->state.reg.r[0], help);

  cpu_ctx->state.reg.ic++;
  return (nc_FAB);
}

static int
ex_fsb (struct cpu_context *cpu_ctx, ushort opcode) /* 2[4-7]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);
  short help[2];

  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);
  arith (&cpu_ctx->state, ARI_SUB, VAR_FLOAT, &cpu_ctx->state.reg.r[0], help);

  cpu_ctx->state.reg.ic++;
  return (nc_FSB);
}

static int
ex_fmb (struct cpu_context *cpu_ctx, ushort opcode) /* 2[8-B]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);
  short help[2];

  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);
  arith (&cpu_ctx->state, ARI_MUL, VAR_FLOAT, &cpu_ctx->state.reg.r[0], help);

  cpu_ctx->state.reg.ic++;
  return (nc_FMB);
}

static int
ex_fdb (struct cpu_context *cpu_ctx, ushort opcode) /* 2[C-F]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);
  short help[2];

  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);
  arith (&cpu_ctx->state, ARI_DIV, VAR_FLOAT, &cpu_ctx->state.reg.r[0], help);

  cpu_ctx->state.reg.ic++;
  return (nc_FDB);
}

static int
ex_orb (struct cpu_context *cpu_ctx, ushort opcode) /* 3[0-3]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);
  short help;

  GET (DATA, addr, &help);
  cpu_ctx->state.reg.r[2] |= help;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[2], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_ORB);
}

static int
ex_andb (struct cpu_context *cpu_ctx, ushort opcode) /* 3[4-7]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);
  short help;

  GET (DATA, addr, &help);
  cpu_ctx->state.reg.r[2] &= help;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[2], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_ANDB);
}

static int
ex_cb (struct cpu_context *cpu_ctx, ushort opcode) /* 3[8-B]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);
  short help;

  GET (DATA, addr, &help);
  compare (&cpu_ctx->state, VAR_INT, &cpu_ctx->state.reg.r[2], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_CB);
}

static int
ex_fcb (struct cpu_context *cpu_ctx, ushort opcode) /* 3[C-F]xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + (opcode & 0xFF);
  short help[2];

  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);
  compare (&cpu_ctx->state, VAR_FLOAT, &cpu_ctx->state.reg.r[0], help);

  cpu_ctx->state.reg.ic++;
  return (nc_FCB);
}


static int
ex_lbx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]0y */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();

  GET (DATA, addr, &cpu_ctx->state.reg.r[2]);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[2], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_LBX);
}

static int
ex_dlbx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]1y */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();

  GET (DATA, addr, &cpu_ctx->state.reg.r[0]);
  GET (DATA, addr + 1, &cpu_ctx->state.reg.r[1]);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[0], VAR_LONG);

  cpu_ctx->state.reg.ic++;
  return (nc_DLBX);
}

static int
ex_stbx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]2y */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();

  PUT (addr, cpu_ctx->state.reg.r[2]);

  cpu_ctx->state.reg.ic++;
  return (nc_STBX);
}

static int
ex_dstx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]3y */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();

  PUT (addr, cpu_ctx->state.reg.r[0]);
  PUT (addr + 1, cpu_ctx->state.reg.r[1]);

  cpu_ctx->state.reg.ic++;
  return (nc_DSTX);
}

static int
ex_abx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]4y */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();
  short help;

  GET (DATA, addr, &help);
  arith (&cpu_ctx->state, ARI_ADD, VAR_INT, &cpu_ctx->state.reg.r[2], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_ABX);
}

static int
ex_sbbx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]5y */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();
  short help;

  GET (DATA, addr, &help);
  arith (&cpu_ctx->state, ARI_SUB, VAR_INT, &cpu_ctx->state.reg.r[2], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_SBBX);
}

static int
ex_mbx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]6y */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();
  short help;

  GET (DATA, addr, &help);
  arith (&cpu_ctx->state, ARI_MUL, VAR_INT, &cpu_ctx->state.reg.r[2], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_MBX);
}

static int
ex_dbx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]7y */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();
  short help;

  GET (DATA, addr, &help);
  arith (&cpu_ctx->state, ARI_DIV, VAR_INT, &cpu_ctx->state.reg.r[2], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_DBX);
}

static int
ex_fabx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]8y */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();
  short help[2];

  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);
  arith (&cpu_ctx->state, ARI_ADD, VAR_FLOAT, &cpu_ctx->state.reg.r[0], help);

  cpu_ctx->state.reg.ic++;
  return (nc_FABX);
}

static int
ex_fsbx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]9y */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();
  short help[2];

  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);
  arith (&cpu_ctx->state, ARI_SUB, VAR_FLOAT, &cpu_ctx->state.reg.r[0], help);

  cpu_ctx->state.reg.ic++;
  return (nc_FSBX);
}

static int
ex_fmbx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]Ay */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();
  short help[2];

  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);
  arith (&cpu_ctx->state, ARI_MUL, VAR_FLOAT, &cpu_ctx->state.reg.r[0], help);

  cpu_ctx->state.reg.ic++;
  return (nc_FMBX);
}

static int
ex_fdbx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]By */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();
  short help[2];

  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);
  arith (&cpu_ctx->state, ARI_DIV, VAR_FLOAT, &cpu_ctx->state.reg.r[0], help);

  cpu_ctx->state.reg.ic++;
  return (nc_FDBX);
}

static int
ex_cbx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]Cy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();
  short help;
  
  GET (DATA, addr, &help);
  compare (&cpu_ctx->state, VAR_INT, &cpu_ctx->state.reg.r[2], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_CBX);
}

static int
ex_fcbx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]Dy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();
  short help[2];

  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);
  compare (&cpu_ctx->state, VAR_FLOAT, &cpu_ctx->state.reg.r[0], help);

  cpu_ctx->state.reg.ic++;
  return (nc_FCBX);
}

static int
ex_andx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]Ey */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();
  short help;

  GET (DATA, addr, &help);
  cpu_ctx->state.reg.r[2] &= help;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[2], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_ANDX);
}

static int
ex_orbx (struct cpu_context *cpu_ctx, ushort opcode) /* 4[0-3]Fy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr = BASEREG (opcode) + CHK_RX ();
  short help;

  GET (DATA, addr, &help);
  cpu_ctx->state.reg.r[2] |= help;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[2], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_ORBX);
}

static int
ex_brx (struct cpu_context *cpu_ctx, ushort opcode) /* opcode 4x distributor */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  static int (*start_4x[16]) (struct cpu_context *cpu_ctx, ushort opcode) =
    {
       ex_lbx,  ex_dlbx, ex_stbx, ex_dstx,
       ex_abx,  ex_sbbx, ex_mbx,  ex_dbx,
       ex_fabx, ex_fsbx, ex_fmbx, ex_fdbx,
       ex_cbx,  ex_fcbx, ex_andx, ex_orbx
    };
  return (*start_4x[(unsigned) upper]) (cpu_ctx, opcode);
}


/* auxiliary to ex_xio() : */

extern void do_xio (ushort, ushort *);	/* User XIO definition, do_xio.c */

static void
realize_xio (struct cpu_context *cpu_ctx, ushort xio_address, ushort *transfer)
{
  int i, bank;
  ushort addr_hibyte = xio_address & 0xFF00;
  ushort xio_upper = (xio_address & 0x00F0) >> 4;
  ushort xio_lower =  xio_address & 0x000F;

  switch (addr_hibyte)
    {
    case X_WIPR:
    case X_WOPR:
      bank = (addr_hibyte == X_WIPR) ? CODE : DATA;
      cpu_ctx->state.pagereg[bank][xio_upper][xio_lower].ppa = *transfer & 0xff;
      return;
    case X_RIPR:
    case X_ROPR:
      bank = (addr_hibyte == X_RIPR) ? CODE : DATA;
      *transfer = cpu_ctx->state.pagereg[bank][xio_upper][xio_lower].ppa;
      return;
    }
  for (i = 0; xio[i].value; i++)
    if (xio[i].value == xio_address)
      break;

  if (xio[i].value)
    {
      switch (xio_address)
	{
	case     X_ENBL:
	  cpu_ctx->state.reg.sys |= SYS_INT;
	elsecase X_DSBL:
	  cpu_ctx->state.reg.sys &= ~SYS_INT;
	elsecase X_DMAE:
	  cpu_ctx->state.reg.sys |= SYS_DMA;
	elsecase X_DMAD:
	  cpu_ctx->state.reg.sys &= ~SYS_DMA;
	elsecase X_TAH:
	  cpu_ctx->state.reg.sys &= ~SYS_TA;
	elsecase X_TBH:
	  cpu_ctx->state.reg.sys &= ~SYS_TB;
	elsecase X_TAS:
	  cpu_ctx->state.reg.sys |= SYS_TA;
	elsecase X_OTA:
	  cpu_ctx->state.reg.sys |= SYS_TA;
	  cpu_ctx->state.reg.timer[TIM_A] = *transfer;
	elsecase X_ITA:
	  *transfer = cpu_ctx->state.reg.timer[TIM_A];
	elsecase X_TBS:
	  cpu_ctx->state.reg.sys |= SYS_TB;
	elsecase X_OTB:
	  cpu_ctx->state.reg.sys |= SYS_TB;
	  cpu_ctx->state.reg.timer[TIM_B] = *transfer;
	elsecase X_ITB:
	  *transfer = cpu_ctx->state.reg.timer[TIM_B];
	elsecase X_GO:
	  cpu_ctx->state.reg.go = 0;
	elsecase X_RSW:
	  *transfer = cpu_ctx->state.reg.sw;
	elsecase X_WSW:
	  cpu_ctx->state.reg.sw = *transfer;
#ifdef NO_EXTENDED_MEMORY
          if (cpu_ctx->state.reg.sw & 0xF)
            {
              cpu_ctx->state.reg.ft |= 0x10;  /* address state fault: bit 11 */
              cpu_ctx->state.reg.sw &= 0xFFF0;
            }
#endif
	elsecase X_RPI:
	  cpu_ctx->state.reg.pir &= ~(0x8000 >> *transfer);
	  if (*transfer == 1)
	    cpu_ctx->state.reg.ft = 0;
	elsecase X_SPI:
	  cpu_ctx->state.reg.pir |= *transfer;
	elsecase X_RMK:
	  *transfer = cpu_ctx->state.reg.mk;
	elsecase X_SMK:
	  cpu_ctx->state.reg.mk = *transfer;
	elsecase X_RPIR:
	  *transfer = cpu_ctx->state.reg.pir;
	elsecase X_RCFR:
	  *transfer = cpu_ctx->state.reg.ft;
	  cpu_ctx->state.reg.ft = 0;
	  cpu_ctx->state.reg.pir &= ~INTR_MACHERR;
	elsecase X_CLIR:
	  cpu_ctx->state.reg.ft = cpu_ctx->state.reg.pir = 0;
	elsecase X_CO:
#ifdef BSVC
	  info (".");
#endif
	  i = (*transfer >> 8) & 0xFF;
	  if (i)
	    {
	      info ("XIO CO highbyte 0x%02X\n", i);
	      *transfer &= 0xFF;
	    }
	  if (isprint (*transfer) || isspace(*transfer))
	    lprintf ("%c", *transfer);
	  else
	    info ("XIO CO: 0x%02hX\n", *transfer);
	  break;

	default:
	  /* info ("XIO %s: 0x%4hX\n", xio[i].name, *transfer); */
	  break;
	}
    }
  else
    do_xio (xio_address, transfer);	/* user defined XIO */
}

static int
ex_xio (struct cpu_context *cpu_ctx, ushort opcode) /* 48xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  unsigned ak = (unsigned) (cpu_ctx->state.reg.sw >> 4) & 0xF; /* privileged instruction */
  ushort xio_address;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &xio_address);
  xio_address += CHK_RX ();

  if (ak != 0)
    {
      cpu_ctx->state.reg.pir |= INTR_MACHERR;
      cpu_ctx->state.reg.ft |= FT_PRIV_INSTR;
    }
  else
    realize_xio (cpu_ctx, xio_address, (ushort *) &cpu_ctx->state.reg.r[upper]);

  cpu_ctx->state.reg.ic += 2;
  return (nc_XIO);
}

static int
ex_vio (struct cpu_context *cpu_ctx, ushort opcode) /* 49xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  unsigned ak = (unsigned) (cpu_ctx->state.reg.sw >> 4) & 0xF; /* privileged instruction */
  ushort vio_address, vec_sel, i = 0, n, transfer, iocmd, orig_iocmd, iodata;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &vio_address);
  vio_address += CHK_RX ();
  GET (DATA, vio_address + 1, (short *) &vec_sel);

  if (ak != 0)
    {
      cpu_ctx->state.reg.pir |= INTR_MACHERR;
      cpu_ctx->state.reg.ft |= FT_PRIV_INSTR;
    }
  else
    {
      if (vec_sel != 0)
      {
        GET (DATA, vio_address, (short *) &orig_iocmd);
      }
      for (n = 0; n <= 15; n++)
	{
	  if (vec_sel & (1 << (15 - n)))
	    {
	      iocmd = orig_iocmd + n * (ushort) cpu_ctx->state.reg.r[upper];
	      transfer = vio_address + 2 + i;
	      GET (DATA, transfer, (short *) &iodata);
	      realize_xio (cpu_ctx, iocmd, &iodata);
	      if (iocmd & 0x8000)  /* XIO read */
		    PUT (transfer, iodata);
	      i++;
	    }
	}
    }

  cpu_ctx->state.reg.ic += 2;
  return (nc_VIO);
}


static int
ex_aim (struct cpu_context *cpu_ctx, ushort opcode) /* 4Ax1 */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, &help);
  arith (&cpu_ctx->state, ARI_ADD, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_AIM);
}

static int
ex_sim (struct cpu_context *cpu_ctx, ushort opcode) /* 4Ax2 */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, &help);
  arith (&cpu_ctx->state, ARI_SUB, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_SIM);
}

static int
ex_mim (struct cpu_context *cpu_ctx, ushort opcode) /* 4Ax3 */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, &help);
  arith (&cpu_ctx->state, ARI_MUL, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_MIM);
}

static int
ex_msim (struct cpu_context *cpu_ctx, ushort opcode) /* 4Ax4 */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, &help);
  arith (&cpu_ctx->state, ARI_MULS, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_MSIM);
}

static int
ex_dim (struct cpu_context *cpu_ctx, ushort opcode) /* 4Ax5 */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, &help);
  arith (&cpu_ctx->state, ARI_DIV, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_DIM);
}

static int
ex_dvim (struct cpu_context *cpu_ctx, ushort opcode) /* 4Ax6 */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, &help);
  arith (&cpu_ctx->state, ARI_DIVV, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_DVIM);
}

static int
ex_andm (struct cpu_context *cpu_ctx, ushort opcode) /* 4Ax7 */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, &help);
  cpu_ctx->state.reg.r[upper] &= help;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_ANDM);
}

static int
ex_orim (struct cpu_context *cpu_ctx, ushort opcode) /* 4Ax8 */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, &help);
  cpu_ctx->state.reg.r[upper] |= help;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_ORIM);
}

static int
ex_xorm (struct cpu_context *cpu_ctx, ushort opcode) /* 4Ax9 */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, &help);
  cpu_ctx->state.reg.r[upper] ^= help;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_XORM);
}

static int
ex_cim (struct cpu_context *cpu_ctx, ushort opcode) /* 4AxA */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, &help);
  compare (&cpu_ctx->state, VAR_INT, &cpu_ctx->state.reg.r[upper], &help); /* side effect on CS */

  cpu_ctx->state.reg.ic += 2;
  return (nc_CIM);
}

static int
ex_nim (struct cpu_context *cpu_ctx, ushort opcode) /* 4AxB */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, &help);
  cpu_ctx->state.reg.r[upper] = ~(cpu_ctx->state.reg.r[upper] & help);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_NIM);
}

static int
ex_imml (struct cpu_context *cpu_ctx, ushort opcode) /* opcode 4A distributor */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  static int (*start_4a[16])(struct cpu_context *cpu_ctx, ushort opcode) =
    {
       ex_ill,  ex_aim,  ex_sim,  ex_mim,
       ex_msim, ex_dim,  ex_dvim, ex_andm,
       ex_orim, ex_xorm, ex_cim,  ex_nim,
       ex_ill,  ex_ill,  ex_ill,  ex_ill
    };
  return (*start_4a[(unsigned) lower]) (cpu_ctx, opcode);
}


static int
ex_esqr (struct cpu_context *cpu_ctx, ushort opcode) /* 4Dxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  double input = from_1750eflt (&cpu_ctx->state.reg.r[upper]);

  if (input < 0.0)
    cpu_ctx->state.reg.pir |= INTR_FIXOFL;
  else
    {
      to_1750eflt (sqrt (input), &cpu_ctx->state.reg.r[upper]);
      update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_DOUBLE);
    }

  cpu_ctx->state.reg.ic++;
  return (180);  /* GVSC, a wild guess */
}


static int
ex_sqrt (struct cpu_context *cpu_ctx, ushort opcode) /* 4Exy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  double input = from_1750flt (&cpu_ctx->state.reg.r[upper]);

  if (input < 0.0)
    cpu_ctx->state.reg.pir |= INTR_FIXOFL;
  else
    {
      to_1750flt (sqrt (input), &cpu_ctx->state.reg.r[upper]);
      update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_FLOAT);
    }

  cpu_ctx->state.reg.ic++;
  return (130);  /* GVSC, a wild guess */
}


static int
ex_bif (struct cpu_context *cpu_ctx, ushort opcode) /* 4Fxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  return (nc_BIF);
}


static int
ex_sb (struct cpu_context *cpu_ctx, ushort opcode) /* 50xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short data;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &data);

  data |= 1 << (15 - upper);
  PUT (addr, data);

  cpu_ctx->state.reg.ic += 2;
  return (nc_SB);
}

static int
ex_sbr (struct cpu_context *cpu_ctx, ushort opcode) /* 51xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  cpu_ctx->state.reg.r[lower] |= 1 << (15 - upper);

  cpu_ctx->state.reg.ic++;
  return (nc_SBR);
}

static int
ex_sbi (struct cpu_context *cpu_ctx, ushort opcode) /* 52xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short data;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, (short *) &addr);
  GET (DATA, addr, &data);

  data |= 1 << (15 - upper);
  PUT (addr, data);

  cpu_ctx->state.reg.ic += 2;
  return (nc_SBI);
}

static int
ex_rb (struct cpu_context *cpu_ctx, ushort opcode) /* 53xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short data;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &data);

  data &= ~(1 << (15 - upper));
  PUT (addr, data);

  cpu_ctx->state.reg.ic += 2;
  return (nc_RB);
}

static int
ex_rbr (struct cpu_context *cpu_ctx, ushort opcode) /* 54xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  cpu_ctx->state.reg.r[lower] &= ~(1 << (15 - upper));

  cpu_ctx->state.reg.ic++;
  return (nc_RBR);
}

static int
ex_rbi (struct cpu_context *cpu_ctx, ushort opcode) /* 55xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short data;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, (short *) &addr);
  GET (DATA, addr, &data);

  data &= ~(1 << (15 - upper));
  PUT (addr, data);

  cpu_ctx->state.reg.ic += 2;
  return (nc_RBI);
}

static int
ex_tb (struct cpu_context *cpu_ctx, ushort opcode) /* 56xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr, sw_save = cpu_ctx->state.reg.sw & 0x0FFF;
  short data;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &data);

  if (data & (1 << (15 - upper)))
    cpu_ctx->state.reg.sw = sw_save | (upper ? CS_POSITIVE : CS_NEGATIVE);
  else
    cpu_ctx->state.reg.sw = sw_save | CS_ZERO;

  cpu_ctx->state.reg.ic += 2;
  return (nc_TB);
}

static int
ex_tbr (struct cpu_context *cpu_ctx, ushort opcode) /* 57xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort sw_save = cpu_ctx->state.reg.sw & 0x0FFF;

  if (cpu_ctx->state.reg.r[lower] & (1 << (15 - upper)))
    cpu_ctx->state.reg.sw = sw_save | (upper ? CS_POSITIVE : CS_NEGATIVE);
  else
    cpu_ctx->state.reg.sw = sw_save | CS_ZERO;

  cpu_ctx->state.reg.ic++;
  return (nc_TBR);
}

static int
ex_tbi (struct cpu_context *cpu_ctx, ushort opcode) /* 58xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr, sw_save = cpu_ctx->state.reg.sw & 0x0FFF;
  short data;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, (short *) &addr);
  GET (DATA, addr, &data);

  if (data & (1 << (15 - upper)))
    cpu_ctx->state.reg.sw = sw_save | (upper ? CS_POSITIVE : CS_NEGATIVE);
  else
    cpu_ctx->state.reg.sw = sw_save | CS_ZERO;

  cpu_ctx->state.reg.ic += 2;
  return (nc_TBI);
}

static int
ex_tsb (struct cpu_context *cpu_ctx, ushort opcode) /* 59xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr, sw_save = cpu_ctx->state.reg.sw & 0x0FFF;
  short data, bit_set = 1 << (15 - upper);

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &data);
  PUT (addr, data | bit_set);

  if (data & bit_set)
    cpu_ctx->state.reg.sw = sw_save | (upper ? CS_POSITIVE : CS_NEGATIVE);
  else
    cpu_ctx->state.reg.sw = sw_save | CS_ZERO;

  cpu_ctx->state.reg.ic += 2;
  return (nc_TSB);
}

static int
ex_svbr (struct cpu_context *cpu_ctx, ushort opcode) /* 5Axy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  cpu_ctx->state.reg.r[lower] |= 1 << (15 - (cpu_ctx->state.reg.r[upper] & 0xF));

  cpu_ctx->state.reg.ic++;
  return (nc_SVBR);
}

static int
ex_rvbr (struct cpu_context *cpu_ctx, ushort opcode) /* 5Cxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  cpu_ctx->state.reg.r[lower] &= ~(1 << (15 - (cpu_ctx->state.reg.r[upper] & 0xF)));

  cpu_ctx->state.reg.ic++;
  return (nc_RVBR);
}

static int
ex_tvbr (struct cpu_context *cpu_ctx, ushort opcode) /* 5Exy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort data = cpu_ctx->state.reg.r[lower] & (1 << (15 - (cpu_ctx->state.reg.r[upper] & 0xF)));
  ushort sw_save = cpu_ctx->state.reg.sw & 0x0FFF;

  if (data)
    cpu_ctx->state.reg.sw = sw_save | (cpu_ctx->state.reg.r[upper] & 0xF ? CS_POSITIVE : CS_NEGATIVE);
  else
    cpu_ctx->state.reg.sw = sw_save | CS_ZERO;

  cpu_ctx->state.reg.ic++;
  return (nc_TVBR);
}


static int
ex_sll (struct cpu_context *cpu_ctx, ushort opcode) /* 60xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int n_shifts = (int) upper + 1;

  cpu_ctx->state.reg.r[lower] <<= n_shifts;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[lower], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_SLL);
}

static int
ex_srl (struct cpu_context *cpu_ctx, ushort opcode) /* 61xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int n_shifts = (int) upper + 1;
/* ATTENTION:  To solicit a `logical shift right' operation (instead of 
   `arithmetic shift right'), it is NOT enough to cast a signed to an
   unsigned in the shift expression.
   But a separate buffer variable of unsigned type does the job! */
  ushort buf;

  buf = (ushort) cpu_ctx->state.reg.r[lower];  /* Needed to get a `logical shift right' */
  cpu_ctx->state.reg.r[lower] = buf >> n_shifts;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[lower], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_SRL);
}

static int
ex_sra (struct cpu_context *cpu_ctx, ushort opcode) /* 62xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int n_shifts = (int) upper + 1;

  cpu_ctx->state.reg.r[lower] >>= n_shifts;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[lower], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_SRA);
}

static int
ex_slc (struct cpu_context *cpu_ctx, ushort opcode) /* 63xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int n_shifts = (int) upper + 1;
  ushort buf;

  buf = (ushort) cpu_ctx->state.reg.r[lower];  /* Needed to get a `logical shift right' */
  cpu_ctx->state.reg.r[lower] = (buf << n_shifts) | (buf >> (16 - n_shifts));
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[lower], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_SLC);
}

static int
ex_dsll (struct cpu_context *cpu_ctx, ushort opcode) /* 65xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int n_shifts = (int) upper + 1;
  uint buf = ((uint) cpu_ctx->state.reg.r[lower] << 16)
  | ((uint) cpu_ctx->state.reg.r[lower + 1] & 0xFFFF);

  buf <<= n_shifts;
  cpu_ctx->state.reg.r[lower] = (short) (buf >> 16);
  cpu_ctx->state.reg.r[lower + 1] = (short) (buf & 0xFFFF);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[lower], VAR_LONG);

  cpu_ctx->state.reg.ic++;
  return (nc_DSLL);
}

static int
ex_dsrl (struct cpu_context *cpu_ctx, ushort opcode) /* 66xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int n_shifts = (int) upper + 1;
  uint buf = ((uint) cpu_ctx->state.reg.r[lower] << 16)
  | ((uint) cpu_ctx->state.reg.r[lower + 1] & 0xFFFF);

  buf >>= n_shifts;                       /* Attention: buf is unsigned, */
  cpu_ctx->state.reg.r[lower] = (short) (buf >> 16);   /* i.e. *logical* shift right */
  cpu_ctx->state.reg.r[lower + 1] = (short) (buf & 0xFFFF);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[lower], VAR_LONG);

  cpu_ctx->state.reg.ic++;
  return (nc_DSRL);
}

static int
ex_dsra (struct cpu_context *cpu_ctx, ushort opcode) /* 67xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int n_shifts = (int) upper + 1;
  int buf = ((int) cpu_ctx->state.reg.r[lower] << 16)
          | ((int) cpu_ctx->state.reg.r[lower + 1] & 0xFFFF);

  buf >>= n_shifts;
  cpu_ctx->state.reg.r[lower] = (short) (buf >> 16);
  cpu_ctx->state.reg.r[lower + 1] = (short) (buf & 0xFFFF);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[lower], VAR_LONG);

  cpu_ctx->state.reg.ic++;
  return (nc_DSRA);
}

static int
ex_dslc (struct cpu_context *cpu_ctx, ushort opcode) /* 68xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int n_shifts = (int) upper + 1;
  uint buf = ((uint) cpu_ctx->state.reg.r[lower] << 16)  /* This variable is needed to */
  | ((uint) cpu_ctx->state.reg.r[lower + 1] & 0xFFFF);   /* get a `logical shift right' */

  buf = (buf << n_shifts) | (buf >> (32 - n_shifts));
  cpu_ctx->state.reg.r[lower] = (short) (buf >> 16);
  cpu_ctx->state.reg.r[lower + 1] = (short) (buf & 0xFFFF);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[lower], VAR_LONG);

  cpu_ctx->state.reg.ic++;
  return (nc_DSLC);
}

static int
ex_slr (struct cpu_context *cpu_ctx, ushort opcode) /* 6Axy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short shc = cpu_ctx->state.reg.r[lower];
  int n_shifts = (int) shc;
  ushort buf;

  buf = (ushort) cpu_ctx->state.reg.r[upper];  /* Needed to get a `logical shift right' */
  if (shc < 0)                  /* negative => shift right */
    {
      if ((shc = -shc) > 16)
	{
	  cpu_ctx->state.reg.pir |= INTR_FIXOFL;
	  /* behavior to be verified */
	}
      cpu_ctx->state.reg.r[upper] = buf >> shc;
    }
  else
    /* positive => shift left  */
    {
      if (shc > 16)
	{
	  cpu_ctx->state.reg.pir |= INTR_FIXOFL;
	  /* behavior to be verified */
	}
      cpu_ctx->state.reg.r[upper] = buf << shc;
    }
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_SLR);
}

static int
ex_sar (struct cpu_context *cpu_ctx, ushort opcode) /* 6Bxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short shc = cpu_ctx->state.reg.r[lower];
  int n_shifts = (int) shc;

  if (shc < 0)                  /* negative => shift right */
    {
      if ((shc = -shc) > 16)
	{
	  cpu_ctx->state.reg.pir |= INTR_FIXOFL;
	  /*  behavior to be verified */
	}
  else
  {
    if (shc == 16)
      {
        if (cpu_ctx->state.reg.r[upper] < 0)
          cpu_ctx->state.reg.r[upper] = 0xFFFF;
        else
          cpu_ctx->state.reg.r[upper] = 0;
      }
      else
        cpu_ctx->state.reg.r[upper] >>= shc;

      update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);
  }
    }
  else
    /* positive => shift left  */
    {
      if (shc > 16)
	{
	  cpu_ctx->state.reg.pir |= INTR_FIXOFL;
	  /* behavior to be verified */
	}
  else
  {
      if (shc == 16)
	      cpu_ctx->state.reg.r[upper] = 0;
      else
	      cpu_ctx->state.reg.r[upper] <<= shc;
      update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);
  }
    }
  

  cpu_ctx->state.reg.ic++;
  return (nc_SAR);
}

static int
ex_scr (struct cpu_context *cpu_ctx, ushort opcode) /* 6Cxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short shc = cpu_ctx->state.reg.r[lower];
  int n_shifts = (int) shc;
  ushort buf;

  buf = (ushort) cpu_ctx->state.reg.r[upper];   /* Needed to get a `logical shift right' */
  if (shc < 0)                  /* negative , rotate right */
    {
      if ((shc = -shc) > 16)
	{
	  cpu_ctx->state.reg.pir |= INTR_FIXOFL;
	  /* behavior to be verified */
	}
      cpu_ctx->state.reg.r[upper] = (buf >> shc) | (buf << (16 - shc));
    }
  else
    /* positive , rotate left  */
    {
      if (shc > 16)
	{
	  cpu_ctx->state.reg.pir |= INTR_FIXOFL;
	  /* behavior to be verified */
	}
      cpu_ctx->state.reg.r[upper] = (buf << shc) | (buf >> (16 - shc));
    }
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_SCR);
}

static int
ex_dslr (struct cpu_context *cpu_ctx, ushort opcode) /* 6Dxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short shc = cpu_ctx->state.reg.r[lower];
  int n_shifts = (int) shc;
  uint buf = ((uint) cpu_ctx->state.reg.r[upper] << 16)   /* This variable is needed */
  | ((uint) cpu_ctx->state.reg.r[upper + 1] & 0xFFFF);     /* to get a LOGICAL shift */

  if (shc < 0)                  /* negative => shift right */
    {
      if ((shc = -shc) > 32)
	cpu_ctx->state.reg.pir |= INTR_FIXOFL;
      else
	buf >>= shc;
    }
  else				/* positive => shift left  */
    {
      if (shc > 32)
	cpu_ctx->state.reg.pir |= INTR_FIXOFL;
      else
	buf <<= shc;
    }
  cpu_ctx->state.reg.r[upper] = (short) (buf >> 16);
  cpu_ctx->state.reg.r[upper + 1] = (short) (buf & 0xFFFF);

  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_LONG);

  cpu_ctx->state.reg.ic++;
  return (nc_DSLR);
}

static int
ex_dsar (struct cpu_context *cpu_ctx, ushort opcode) /* 6Exy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short shc = cpu_ctx->state.reg.r[lower];
  int n_shifts = (int) shc;
  int buf = ((int) cpu_ctx->state.reg.r[upper] << 16)
	  | ((int) cpu_ctx->state.reg.r[upper + 1] & 0xFFFF);

  if (shc < 0)                  /* negative => shift right */
    {
      if ((shc = -shc) > 32)
	cpu_ctx->state.reg.pir |= INTR_FIXOFL;
      else if (shc == 32)
	{
	 if (buf < 0)
	   buf = 0xFFFFFFFF;
	 else
	   buf = 0;
	}
      else
        buf >>= shc;
    }
  else				/* positive => shift left  */
    {
      if (shc > 32)
	cpu_ctx->state.reg.pir |= INTR_FIXOFL;
      else if (shc == 32)
	buf = 0;
      else
	buf <<= shc;
    }
  cpu_ctx->state.reg.r[upper] = (short) (buf >> 16);
  cpu_ctx->state.reg.r[upper + 1] = (short) (buf & 0xFFFF);

  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_LONG);

  cpu_ctx->state.reg.ic++;
  return (nc_DSAR);
}

static int
ex_dscr (struct cpu_context *cpu_ctx, ushort opcode) /* 6Fxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short shc = cpu_ctx->state.reg.r[lower];
  int n_shifts = (int) shc;
  uint buf = ((uint) cpu_ctx->state.reg.r[upper] << 16)   /* This variable is needed */
  | ((uint) cpu_ctx->state.reg.r[upper + 1] & 0xFFFF);     /* to get a LOGICAL shift */

  if (shc < 0)                  /* negative => rotate right */
    {
      if ((shc = -shc) > 32)
	{
	  cpu_ctx->state.reg.pir |= INTR_FIXOFL;    /* behavior to be verified */
	}
      buf = (buf >> shc) | (buf << (32 - shc));
    }
  else
    /* positive => rotate left  */
    {
      if (shc > 32)
	{
	  cpu_ctx->state.reg.pir |= INTR_FIXOFL;    /* behavior to be verified */
	}
      buf = (buf << shc) | (buf >> (32 - shc));
    }
  cpu_ctx->state.reg.r[upper] = (short) (buf >> 16);
  cpu_ctx->state.reg.r[upper + 1] = (short) (buf & 0xFFFF);

  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_LONG);

  cpu_ctx->state.reg.ic++;
  return (nc_DSCR);
}

/* auxiliary to ex_jc and ex_jci */

static bool
flag_test (ushort sw, ushort condition)
{
  if ((condition & 0x0007) == 0x0007)
    return (TRUE);
  if (condition & ((sw >> 12) & 0x000F))
    return (TRUE);
  else
    return (FALSE);
}

static int
ex_jc (struct cpu_context *cpu_ctx, ushort opcode) /* 70xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  bool jump_taken = flag_test (cpu_ctx->state.reg.sw, upper);

  if (jump_taken)
    {
      GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &cpu_ctx->state.reg.ic);
      cpu_ctx->state.reg.ic += CHK_RX ();
    }
  else
    cpu_ctx->state.reg.ic += 2;

  return (nc_JC);
}


static int
ex_jci (struct cpu_context *cpu_ctx, ushort opcode) /* 71xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  bool jump_taken = flag_test (cpu_ctx->state.reg.sw, upper);

  if (jump_taken)
    {
      GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
      addr += CHK_RX ();
      GET (DATA, addr, (short *) &cpu_ctx->state.reg.ic);
    }
  else
    cpu_ctx->state.reg.ic += 2;

  return (nc_JCI);
}

static int
ex_js (struct cpu_context *cpu_ctx, ushort opcode) /* 72xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  cpu_ctx->state.reg.r[upper] = cpu_ctx->state.reg.ic + 2;
  cpu_ctx->state.reg.ic = addr + CHK_RX ();

  return (nc_JS);
}

static int
ex_soj (struct cpu_context *cpu_ctx, ushort opcode) /* 73xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help;
  int jump_taken = 0;

  help = 1;
  update_pir = FALSE;
  arith (&cpu_ctx->state, ARI_SUB, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);
  update_pir = TRUE;

  if (CS_ZERO & cpu_ctx->state.reg.sw)
    cpu_ctx->state.reg.ic += 2;                     /* end of loop */
  else
    {
      GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &cpu_ctx->state.reg.ic);
      cpu_ctx->state.reg.ic += CHK_RX ();
      jump_taken = 1;
    }

  return (nc_SOJ);
}

static int
ex_br (struct cpu_context *cpu_ctx, ushort opcode) /* 74xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort distance = opcode & 0x00FF;

  if (distance >= 0x80)		/* branch backwards */
    cpu_ctx->state.reg.ic -= 0x100 - distance;
  else
    cpu_ctx->state.reg.ic += distance;

  return (nc_BR);
}

static int
ex_bez (struct cpu_context *cpu_ctx, ushort opcode) /* 75xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  bool branch_taken = FALSE;

  if (CS_ZERO & cpu_ctx->state.reg.sw)
    {
      ushort distance = opcode & 0x00FF;
      if (distance >= 0x80)
	cpu_ctx->state.reg.ic -= 0x100 - distance;
      else
	cpu_ctx->state.reg.ic += distance;
      branch_taken = TRUE;
    }
  else
    cpu_ctx->state.reg.ic++;

  return (nc_BRcc);
}

static int
ex_blt (struct cpu_context *cpu_ctx, ushort opcode) /* 76xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  bool branch_taken = FALSE;

  if (CS_NEGATIVE & cpu_ctx->state.reg.sw)
    {
      ushort distance = opcode & 0x00FF;
      if (distance >= 0x80)
	cpu_ctx->state.reg.ic -= 0x100 - distance;
      else
	cpu_ctx->state.reg.ic += distance;
      branch_taken = TRUE;
    }
  else
    cpu_ctx->state.reg.ic++;

  return (nc_BRcc);
}

static int
ex_bex (struct cpu_context *cpu_ctx, ushort opcode) /* 77xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  cpu_ctx->state.bex_index = (int) lower;
  cpu_ctx->state.reg.pir |= INTR_BEX;

  cpu_ctx->state.reg.ic++;
  return (nc_BEX);
}

static int
ex_ble (struct cpu_context *cpu_ctx, ushort opcode) /* 78xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  bool branch_taken = FALSE;

  if (CS_ZERO & cpu_ctx->state.reg.sw || CS_NEGATIVE & cpu_ctx->state.reg.sw)
    {
      ushort distance = opcode & 0x00FF;
      if (distance >= 0x80)
	cpu_ctx->state.reg.ic -= 0x100 - distance;
      else
	cpu_ctx->state.reg.ic += distance;
      branch_taken = TRUE;
    }
  else
    cpu_ctx->state.reg.ic++;

  return (nc_BRcc);
}

static int
ex_bgt (struct cpu_context *cpu_ctx, ushort opcode) /* 79xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  bool branch_taken = FALSE;

  if (CS_POSITIVE & cpu_ctx->state.reg.sw)
    {
      ushort distance = opcode & 0x00FF;
      if (distance >= 0x80)
	cpu_ctx->state.reg.ic -= 0x100 - distance;
      else
	cpu_ctx->state.reg.ic += distance;
      branch_taken = TRUE;
    }
  else
    cpu_ctx->state.reg.ic++;

  return (nc_BRcc);
}

static int
ex_bnz (struct cpu_context *cpu_ctx, ushort opcode) /* 7Axy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  bool branch_taken = FALSE;

  if ((CS_ZERO & cpu_ctx->state.reg.sw) == 0)
    {
      ushort distance = opcode & 0x00FF;
      if (distance >= 0x80)
	cpu_ctx->state.reg.ic -= 0x100 - distance;
      else
	cpu_ctx->state.reg.ic += distance;
      branch_taken = TRUE;
    }
  else
    cpu_ctx->state.reg.ic++;

  return (nc_BRcc);
}

static int
ex_bge (struct cpu_context *cpu_ctx, ushort opcode) /* 7Bxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  bool branch_taken = FALSE;

  if (CS_ZERO & cpu_ctx->state.reg.sw || CS_POSITIVE & cpu_ctx->state.reg.sw)
    {
      ushort distance = opcode & 0x00FF;
      if (distance >= 0x80)
	cpu_ctx->state.reg.ic -= 0x100 - distance;
      else
	cpu_ctx->state.reg.ic += distance;
      branch_taken = TRUE;
    }
  else
    cpu_ctx->state.reg.ic++;

  return (nc_BRcc);
}

static int
ex_lsti (struct cpu_context *cpu_ctx, ushort opcode) /* 7Cxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  /* privileged instruction  */
  ushort ak = (cpu_ctx->state.reg.sw >> 4) & 0xF, source;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &source);
  source += CHK_RX ();

  if (ak != 0)
    {
      cpu_ctx->state.reg.pir |= INTR_MACHERR;
      cpu_ctx->state.reg.ft |= FT_PRIV_INSTR;
    }
  else
    {
      GET (DATA, source, (short *) &source);
      GET (DATA, source,     (short *) &cpu_ctx->state.reg.mk);
      GET (DATA, source + 2, (short *) &cpu_ctx->state.reg.ic);
      GET (DATA, source + 1, (short *) &cpu_ctx->state.reg.sw);
    }

  return (nc_LSTI);
}

static int
ex_lst (struct cpu_context *cpu_ctx, ushort opcode) /* 7Dxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  /* privileged instruction  */
  ushort ak = (cpu_ctx->state.reg.sw >> 4) & 0xF, source;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &source);
  source += CHK_RX ();

  if (ak != 0)
    {
      cpu_ctx->state.reg.pir |= INTR_MACHERR;
      cpu_ctx->state.reg.ft |= FT_PRIV_INSTR;
    }
  else
    {
      GET (DATA, (ushort) source,     (short *) &cpu_ctx->state.reg.mk);
      GET (DATA, (ushort) source + 2, (short *) &cpu_ctx->state.reg.ic);
      GET (DATA, (ushort) source + 1, (short *) &cpu_ctx->state.reg.sw);
    }

  return (nc_LST);
}

static int
ex_sjs (struct cpu_context *cpu_ctx, ushort opcode) /* 7Exy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();  /* needs to be BEFORE decrementing r[upper] ... */
  cpu_ctx->state.reg.r[upper]--;          /* ... for the case of (lower == upper) */
  PUT ((ushort) cpu_ctx->state.reg.r[upper], (short) cpu_ctx->state.reg.ic + 2);
  cpu_ctx->state.reg.ic = addr;

  return (nc_SJS);
}

static int
ex_urs (struct cpu_context *cpu_ctx, ushort opcode) /* 7Fxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  if (lower)
    return (ex_ill (cpu_ctx, opcode)); /* error */

  GET (DATA, (ushort) cpu_ctx->state.reg.r[upper], (short *) &cpu_ctx->state.reg.ic);
  cpu_ctx->state.reg.r[upper]++;

  return (nc_URS);
}


static int
ex_l (struct cpu_context *cpu_ctx, ushort opcode) /* 80xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &cpu_ctx->state.reg.r[upper]);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_L);
}

static int
ex_lr (struct cpu_context *cpu_ctx, ushort opcode) /* 81xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  cpu_ctx->state.reg.r[upper] = cpu_ctx->state.reg.r[lower];
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_LR);
}

static int
ex_lisp (struct cpu_context *cpu_ctx, ushort opcode) /* 82xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  cpu_ctx->state.reg.r[upper] = (short) lower + 1;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_LISP);
}

static int
ex_lisn (struct cpu_context *cpu_ctx, ushort opcode) /* 83xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  cpu_ctx->state.reg.r[upper] = -(short) (lower + 1);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_LISN);
}

static int
ex_li (struct cpu_context *cpu_ctx, ushort opcode) /* 84xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, (short *) &addr);
  GET (DATA, addr, &cpu_ctx->state.reg.r[upper]);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_LI);
}

static int
ex_lim (struct cpu_context *cpu_ctx, ushort opcode) /* 85xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort immed;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &immed);
  immed += CHK_RX ();		/* IMX */

  cpu_ctx->state.reg.r[upper] = (short) immed;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_LIM);
}

static int
ex_dl (struct cpu_context *cpu_ctx, ushort opcode) /* 86xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();

  if (upper == 15)
    {
      short help[2];
      GET (DATA, addr, &cpu_ctx->state.reg.r[15]);
      help[0] = cpu_ctx->state.reg.r[15];
      GET (DATA, addr + 1, &cpu_ctx->state.reg.r[0]);
      help[1] = cpu_ctx->state.reg.r[0];
      update_cs (&cpu_ctx->state, help, VAR_LONG);
    }
  else
    {
      GET (DATA, addr, &cpu_ctx->state.reg.r[upper]);
      GET (DATA, addr + 1, &cpu_ctx->state.reg.r[upper + 1]);
      update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_LONG);
    }

  cpu_ctx->state.reg.ic += 2;
  return (nc_DL);
}

static int
ex_dlr (struct cpu_context *cpu_ctx, ushort opcode) /* 87xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help[2];

  if (upper == 15)
    {
      help[0] = cpu_ctx->state.reg.r[15] = cpu_ctx->state.reg.r[lower];
      help[1] = cpu_ctx->state.reg.r[0] = cpu_ctx->state.reg.r[lower + 1];
      update_cs (&cpu_ctx->state, help, VAR_LONG);
    }
  else
    {
      if (upper == lower + 1)
	{
	  cpu_ctx->state.reg.r[upper + 1] = cpu_ctx->state.reg.r[lower + 1];
	  cpu_ctx->state.reg.r[upper] = cpu_ctx->state.reg.r[lower];
	}
      else
	{
	  cpu_ctx->state.reg.r[upper] = cpu_ctx->state.reg.r[lower];
	  cpu_ctx->state.reg.r[upper + 1] = cpu_ctx->state.reg.r[lower + 1];
	}
      update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_LONG);
    }

  cpu_ctx->state.reg.ic++;
  return (nc_DLR);
}

static int
ex_dli (struct cpu_context *cpu_ctx, ushort opcode) /* 88xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, (short *) &addr);

  if (upper == 15)
    {
      short help[2];
      GET (DATA, addr, &cpu_ctx->state.reg.r[15]);
      help[0] = cpu_ctx->state.reg.r[15];
      GET (DATA, addr + 1, &cpu_ctx->state.reg.r[0]);
      help[1] = cpu_ctx->state.reg.r[0];
      update_cs (&cpu_ctx->state, help, VAR_LONG);
    }
  else
    {
      GET (DATA, addr, &cpu_ctx->state.reg.r[upper]);
      GET (DATA, addr + 1, &cpu_ctx->state.reg.r[upper + 1]);
      update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_LONG);
    }

  cpu_ctx->state.reg.ic += 2;
  return (nc_DLI);
}

static int
ex_lm (struct cpu_context *cpu_ctx, ushort opcode) /* 89xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort i, addr;
  int n_loads = (int) upper + 1;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  for (i = 0; i <= upper; i++)
    GET (DATA, addr + i, &cpu_ctx->state.reg.r[i]);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_LM);
}

static int
ex_efl (struct cpu_context *cpu_ctx, ushort opcode) /* 8Axy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr, i;
  short help[3];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  for (i = 0; i < 3; i++)
    {
      GET (DATA, addr + i, (short *) &help[i]);
      cpu_ctx->state.reg.r[(upper + i) % 16] = help[i];
    }
  update_cs (&cpu_ctx->state, help, VAR_DOUBLE);

  cpu_ctx->state.reg.ic += 2;
  return (nc_EFL);
}

static int
ex_lub (struct cpu_context *cpu_ctx, ushort opcode) /* 8Bxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  cpu_ctx->state.reg.r[upper] = (cpu_ctx->state.reg.r[upper] & 0xFF00) | ((help >> 8) & 0x00FF);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_LUB);
}

static int
ex_llb (struct cpu_context *cpu_ctx, ushort opcode) /* 8Cxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  cpu_ctx->state.reg.r[upper] = (cpu_ctx->state.reg.r[upper] & 0xFF00) | (help & 0x00FF);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_LLB);
}

static int
ex_lubi (struct cpu_context *cpu_ctx, ushort opcode) /* 8Dxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, (short *) &addr);
  GET (DATA, addr, &help);

  cpu_ctx->state.reg.r[upper] = (cpu_ctx->state.reg.r[upper] & 0xFF00) | ((help >> 8) & 0x00FF);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_LUBI);
}

static int
ex_llbi (struct cpu_context *cpu_ctx, ushort opcode) /* 8Exy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, (short *) &addr);
  GET (DATA, addr, &help);

  cpu_ctx->state.reg.r[upper] = (cpu_ctx->state.reg.r[upper] & 0xFF00) | (help & 0x00FF);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_LLBI);
}

static int
ex_popm (struct cpu_context *cpu_ctx, ushort opcode) /* 8Fxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int count, n_pops;

  if (upper <= lower)
    {
      n_pops = lower - upper + 1;
      for (count = upper; count <= lower; count++)
	{
	  if (count != 15)
	    GET (DATA, (ushort) cpu_ctx->state.reg.r[15], &cpu_ctx->state.reg.r[count]);
	  cpu_ctx->state.reg.r[15]++;
	}
    }
  else
    {
      n_pops = 16 - upper + lower + 1;
      for (count = upper; count <= 15; count++)
	{
	  if (count != 15)
	    GET (DATA, (ushort) cpu_ctx->state.reg.r[15], &cpu_ctx->state.reg.r[count]);
	  cpu_ctx->state.reg.r[15]++;
	}
      for (count = 0; count <= lower; count++)
	{
	  GET (DATA, (ushort) cpu_ctx->state.reg.r[15], &cpu_ctx->state.reg.r[count]);
	  cpu_ctx->state.reg.r[15]++;
	}
    }

  cpu_ctx->state.reg.ic++;
  return (nc_POPM);
}

static int
ex_st (struct cpu_context *cpu_ctx, ushort opcode) /* 90xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();

  PUT (addr, cpu_ctx->state.reg.r[upper]);

  cpu_ctx->state.reg.ic += 2;
  return (nc_ST);
}

static int
ex_stc (struct cpu_context *cpu_ctx, ushort opcode) /* 91xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();

  PUT (addr, (short) upper);

  cpu_ctx->state.reg.ic += 2;
  return (nc_STC);
}

static int
ex_stci (struct cpu_context *cpu_ctx, ushort opcode) /* 92xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, (short *) &addr);

  PUT (addr, (short) upper);

  cpu_ctx->state.reg.ic += 2;
  return (nc_STCI);
}

static int
ex_mov (struct cpu_context *cpu_ctx, ushort opcode) /* 93xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short data;
  ushort source  = (ushort) cpu_ctx->state.reg.r[lower];
  ushort destin  = (ushort) cpu_ctx->state.reg.r[upper];
  ushort n_moves = 1;
  int single_cycle = nc_MOV;  /* computed on the basis of n_moves = 1 */

  n_moves = (ushort) cpu_ctx->state.reg.r[upper + 1];
  while (n_moves)
    {
      GET (DATA, source, &data);
      PUT (destin, data);
      cpu_ctx->state.reg.r[upper] = ++destin;	/* unsigned op */
      cpu_ctx->state.reg.r[lower] = ++source;	/* unsigned op */
      cpu_ctx->state.reg.r[upper + 1] = --n_moves;	/* unsigned op */
      workout_timing (&cpu_ctx->state, single_cycle);
      if (workout_interrupts (&cpu_ctx->state))
        return (nc_MOV);
    }

  cpu_ctx->state.reg.ic++;
  return (nc_MOV);
}

static int
ex_sti (struct cpu_context *cpu_ctx, ushort opcode) /* 94xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, (short *) &addr);

  PUT (addr, cpu_ctx->state.reg.r[upper]);

  cpu_ctx->state.reg.ic += 2;
  return (nc_STI);
}

static int
ex_sfbs (struct cpu_context *cpu_ctx, ushort opcode) /* 95xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort i;

  for (i = 0; i < 16; i++)
    if (cpu_ctx->state.reg.r[upper] & (1 << (15 - i)))
      break;
  cpu_ctx->state.reg.r[lower] = i;
  if (i == 16)
    cpu_ctx->state.reg.sw = (cpu_ctx->state.reg.sw & 0x0FFF) | CS_ZERO;
  else
    update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[lower], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (6);  /* that's what GVSC takes, anyways... */
}

static int
ex_dst (struct cpu_context *cpu_ctx, ushort opcode) /* 96xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();

  PUT (addr, cpu_ctx->state.reg.r[upper]);
  PUT (addr + 1, cpu_ctx->state.reg.r[upper + 1]);

  cpu_ctx->state.reg.ic += 2;
  return (nc_DST);
}

static int
ex_srm (struct cpu_context *cpu_ctx, ushort opcode) /* 97xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort destin;
  short help1, help2, mask;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &destin);
  destin += CHK_RX ();

  mask = cpu_ctx->state.reg.r[upper + 1];
  help1 = cpu_ctx->state.reg.r[upper] & mask;
  GET (DATA, destin, &help2);
  help2 &= ~mask;

  PUT (destin, help1 | help2);

  cpu_ctx->state.reg.ic += 2;
  return (nc_SRM);
}

static int
ex_dsti (struct cpu_context *cpu_ctx, ushort opcode) /* 98xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, (short *) &addr);

  PUT (addr, cpu_ctx->state.reg.r[upper]);
  PUT (addr + 1, cpu_ctx->state.reg.r[upper + 1]);

  cpu_ctx->state.reg.ic += 2;
  return (nc_DSTI);
}

static int
ex_stm (struct cpu_context *cpu_ctx, ushort opcode) /* 99xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort i, addr;
  int n_stores = (int) upper + 1;  /* for stime.h calculation of nc_STM */

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();

  for (i = 0; i <= upper; i++)
    PUT (addr + i, cpu_ctx->state.reg.r[i]);

  cpu_ctx->state.reg.ic += 2;
  return (nc_STM);
}

static int
ex_efst (struct cpu_context *cpu_ctx, ushort opcode) /* 9Axy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort i;
  ushort addr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();

  for (i = 0; i < 3; i++)
    PUT (addr + i, cpu_ctx->state.reg.r[(upper + i) % 16]);

  cpu_ctx->state.reg.ic += 2;
  return (nc_EFST);
}

static int
ex_stub (struct cpu_context *cpu_ctx, ushort opcode) /* 9Bxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);
  
  PUT (addr, (cpu_ctx->state.reg.r[upper] << 8) | (help & 0x00FF));

  cpu_ctx->state.reg.ic += 2;
  return (nc_STUB);
}

static int
ex_stlb (struct cpu_context *cpu_ctx, ushort opcode) /* 9Cxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);
  
  PUT (addr, (cpu_ctx->state.reg.r[upper] & 0x00FF) | (help & 0xFF00));

  cpu_ctx->state.reg.ic += 2;
  return (nc_STLB);
}

static int
ex_subi (struct cpu_context *cpu_ctx, ushort opcode) /* 9Dxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, (short *) &addr);
  GET (DATA, addr, &help);
  
  PUT (addr, (cpu_ctx->state.reg.r[upper] << 8) | (help & 0x00FF));

  cpu_ctx->state.reg.ic += 2;
  return (nc_SUBI);
}

static int
ex_slbi (struct cpu_context *cpu_ctx, ushort opcode) /* 9Exy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, (short *) &addr);
  GET (DATA, addr, &help);
  
  PUT (addr, (cpu_ctx->state.reg.r[upper] & 0x00FF) | (help & 0xFF00));

  cpu_ctx->state.reg.ic += 2;
  return (nc_SLBI);
}

static int
ex_pshm (struct cpu_context *cpu_ctx, ushort opcode) /* 9Fxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int count = (int) lower, n_pushes;
  ushort stkptr = (ushort) cpu_ctx->state.reg.r[15];

  if (upper <= lower)
    {
      n_pushes = lower - upper + 1;
      while (count >= (int) upper)
	{
	  PUT (--stkptr, cpu_ctx->state.reg.r[count]);
	  count--;
	}
    }
  else
    {
      n_pushes = 16 - upper + lower + 1;
      while (count >= 0)
	{
	  PUT (--stkptr, cpu_ctx->state.reg.r[count]);
	  count--;
	}
      count = 15;
      while (count >= (int) upper)
	{
	  PUT (--stkptr, cpu_ctx->state.reg.r[count]);
	  count--;
	}
    }
  cpu_ctx->state.reg.r[15] = (short) stkptr;

  cpu_ctx->state.reg.ic++;
  return (nc_PSHM);
}

static int
ex_a (struct cpu_context *cpu_ctx, ushort opcode) /* A0xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  arith (&cpu_ctx->state, ARI_ADD, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_A);
}

static int
ex_ar (struct cpu_context *cpu_ctx, ushort opcode) /* A1xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_ADD, VAR_INT, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_AR);
}

static int
ex_aisp (struct cpu_context *cpu_ctx, ushort opcode) /* A2xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help = (short) lower + 1;

  arith (&cpu_ctx->state, ARI_ADD, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_AISP);
}

static int
ex_incm (struct cpu_context *cpu_ctx, ushort opcode) /* A3xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help1, help2 = upper + 1;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help1);

  arith (&cpu_ctx->state, ARI_ADD, VAR_INT, &help1, &help2);
  PUT (addr, help1);

  cpu_ctx->state.reg.ic += 2;
  return (nc_INCM);
}

static int
ex_abs (struct cpu_context *cpu_ctx, ushort opcode) /* A4xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int negative = 0;  /* for stime.h cycle computation */

  if (cpu_ctx->state.reg.r[lower] == -32768)
    {
      cpu_ctx->state.reg.r[upper] = -32768;
      cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    }
  else if (cpu_ctx->state.reg.r[upper] < 0)
    {
      negative = 1;
      cpu_ctx->state.reg.r[upper] = -cpu_ctx->state.reg.r[lower];
    }
  else
    cpu_ctx->state.reg.r[upper] = cpu_ctx->state.reg.r[lower];
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_ABS);
}

static int
ex_dabs (struct cpu_context *cpu_ctx, ushort opcode) /* A5xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int help = ((int) cpu_ctx->state.reg.r[lower] << 16)
	   | ((int) cpu_ctx->state.reg.r[lower+1] & 0x0000FFFF);
  int negative = 0;  /* for stime.h cycle computation */

  if (help == -0x80000000)
    {
      cpu_ctx->state.reg.r[upper] = -0x8000;
      cpu_ctx->state.reg.r[upper + 1] = 0;
      cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    }
  else if (help < 0)
    {
      negative = 1;
      help = -help;
    }
  cpu_ctx->state.reg.r[upper] = (short) (help >> 16);
  cpu_ctx->state.reg.r[upper + 1] = (short) (help & 0xFFFF);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_LONG);

  cpu_ctx->state.reg.ic++;
  return (nc_DABS);
}

static int
ex_da (struct cpu_context *cpu_ctx, ushort opcode) /* A6xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[2];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);

  arith (&cpu_ctx->state, ARI_ADD, VAR_LONG, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_DA);
}

static int
ex_dar (struct cpu_context *cpu_ctx, ushort opcode) /* A7xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_ADD, VAR_LONG, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_DAR);
}

static int
ex_fa (struct cpu_context *cpu_ctx, ushort opcode) /* A8xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[2];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);

  arith (&cpu_ctx->state, ARI_ADD, VAR_FLOAT, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_FA);
}

static int
ex_far (struct cpu_context *cpu_ctx, ushort opcode) /* A9xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_ADD, VAR_FLOAT, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_FAR);
}

static int
ex_efa (struct cpu_context *cpu_ctx, ushort opcode) /* AAxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[3];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr,     &help[0]);
  GET (DATA, addr + 1, &help[1]);
  GET (DATA, addr + 2, &help[2]);

  arith (&cpu_ctx->state, ARI_ADD, VAR_DOUBLE, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_EFA);
}

static int
ex_efar (struct cpu_context *cpu_ctx, ushort opcode) /* ABxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_ADD, VAR_DOUBLE, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_EFAR);
}

static int
ex_fabs (struct cpu_context *cpu_ctx, ushort opcode) /* ACxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort mant_signword = (ushort) cpu_ctx->state.reg.r[lower];
  short help[2];
  int negative = 0;

  if (mant_signword & 0x8000)   /* Look at the mantissa signbit */
    {
      negative = 1;
      help[0] = help[1] = 0;
      arith (&cpu_ctx->state, ARI_SUB, VAR_FLOAT, help, &cpu_ctx->state.reg.r[lower]);
    }
  else
      /* Going through a help variable guards against situations such as:  */
    { /* "FABS R2,R1" , where a needed register would otherwise be clobbered. */
      help[0] = cpu_ctx->state.reg.r[lower];
      help[1] = cpu_ctx->state.reg.r[lower + 1];
      update_cs (&cpu_ctx->state, help, VAR_FLOAT);
    }
  cpu_ctx->state.reg.r[upper] = help[0];
  cpu_ctx->state.reg.r[upper + 1] = help[1];

  cpu_ctx->state.reg.ic++;
  return (nc_FABS);
}

static int
ex_uar (struct cpu_context *cpu_ctx, ushort opcode) /* ADxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  uint lhelp = (uint) cpu_ctx->state.reg.r[upper] + (uint) cpu_ctx->state.reg.r[lower];

  cpu_ctx->state.reg.r[upper] = (short) lhelp;
  if (lhelp > 0xFFFF)
    cpu_ctx->state.reg.sw = (cpu_ctx->state.reg.sw & 0x0FFF) | CS_CARRY;
  else
    update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_AR);
}

static int
ex_ua (struct cpu_context *cpu_ctx, ushort opcode) /* AExy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;
  uint lhelp;  /* need it for some brain damaged C compilers */

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  lhelp = (uint) cpu_ctx->state.reg.r[upper] + (uint) help;
  cpu_ctx->state.reg.r[upper] = (short) lhelp;
  if (lhelp > 0xFFFF)
    cpu_ctx->state.reg.sw = (cpu_ctx->state.reg.sw & 0x0FFF) | CS_CARRY;
  else
    update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_A);
}

static int
ex_s (struct cpu_context *cpu_ctx, ushort opcode) /* B0xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  arith (&cpu_ctx->state, ARI_SUB, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_S);
}

static int
ex_sr (struct cpu_context *cpu_ctx, ushort opcode) /* B1xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_SUB, VAR_INT, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_SR);
}

static int
ex_sisp (struct cpu_context *cpu_ctx, ushort opcode) /* B2xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help;

  help = (short) (lower + 1);
  arith (&cpu_ctx->state, ARI_SUB, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_SISP);
}

static int
ex_decm (struct cpu_context *cpu_ctx, ushort opcode) /* B3xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help1, help2 = upper + 1;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help1);

  arith (&cpu_ctx->state, ARI_SUB, VAR_INT, &help1, &help2);
  PUT (addr, help1);

  cpu_ctx->state.reg.ic += 2;
  return (nc_DECM);
}

static int
ex_neg (struct cpu_context *cpu_ctx, ushort opcode) /* B4xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  if (cpu_ctx->state.reg.r[lower] == 0x8000)
    {
      cpu_ctx->state.reg.r[upper] = -32768;
      cpu_ctx->state.reg.pir |= INTR_FIXOFL;
    }
  else
    cpu_ctx->state.reg.r[upper] = -cpu_ctx->state.reg.r[lower];
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_NEG);
}

static int
ex_dneg (struct cpu_context *cpu_ctx, ushort opcode) /* B5xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int help = ((int) cpu_ctx->state.reg.r[lower] << 16)
           | ((int) cpu_ctx->state.reg.r[(lower+1)&0xF] & 0xFFFF);
  if (help == 0x80000000)
  {
    cpu_ctx->state.reg.r[upper] = 0x8000;
    cpu_ctx->state.reg.r[upper + 1] = 0;
    cpu_ctx->state.reg.pir |= INTR_FIXOFL;
  }
  else
  {
    help = -help;
    cpu_ctx->state.reg.r[upper] = (short) (help >> 16);
    cpu_ctx->state.reg.r[upper + 1] = (short) (help & 0xFFFF);
    update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_LONG);
  }

  cpu_ctx->state.reg.ic++;
  return (nc_DNEG);
}

static int
ex_ds (struct cpu_context *cpu_ctx, ushort opcode) /* B6xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[2];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);

  arith (&cpu_ctx->state, ARI_SUB, VAR_LONG, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_DS);
}

static int
ex_dsr (struct cpu_context *cpu_ctx, ushort opcode) /* B7xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_SUB, VAR_LONG, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_DSR);
}

static int
ex_fs (struct cpu_context *cpu_ctx, ushort opcode) /* B8xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[2];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);

  arith (&cpu_ctx->state, ARI_SUB, VAR_FLOAT, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_FS);
}

static int
ex_fsr (struct cpu_context *cpu_ctx, ushort opcode) /* B9xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_SUB, VAR_FLOAT, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_FSR);
}

static int
ex_efs (struct cpu_context *cpu_ctx, ushort opcode) /* BAxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[3];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr,     &help[0]);
  GET (DATA, addr + 1, &help[1]);
  GET (DATA, addr + 2, &help[2]);

  arith (&cpu_ctx->state, ARI_SUB, VAR_DOUBLE, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_EFS);
}

static int
ex_efsr (struct cpu_context *cpu_ctx, ushort opcode) /* BBxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_SUB, VAR_DOUBLE, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_EFSR);
}

static int
ex_fneg (struct cpu_context *cpu_ctx, ushort opcode) /* BCxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help[2];

  help[0] = help[1] = 0;    /* happens to be a floating zero */
  arith (&cpu_ctx->state, ARI_SUB, VAR_FLOAT, help, &cpu_ctx->state.reg.r[lower]);
  cpu_ctx->state.reg.r[upper] = help[0];
  cpu_ctx->state.reg.r[upper+1] = help[1];

  cpu_ctx->state.reg.ic++;
  return (nc_FNEG);
}

static int
ex_usr (struct cpu_context *cpu_ctx, ushort opcode) /* BDxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  uint lhelp = (uint) cpu_ctx->state.reg.r[upper] - (uint) cpu_ctx->state.reg.r[lower];

  cpu_ctx->state.reg.r[upper] = (short) lhelp;
  if (lhelp > 0xFFFF)
    cpu_ctx->state.reg.sw = (cpu_ctx->state.reg.sw & 0x0FFF) | CS_CARRY;
  else
    update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_SR);
}

static int
ex_us (struct cpu_context *cpu_ctx, ushort opcode) /* BExy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;
  uint lhelp;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  lhelp = (uint) cpu_ctx->state.reg.r[upper] - (uint) help;
  cpu_ctx->state.reg.r[upper] = (short) lhelp;
  if (lhelp > 0xFFFF)
    cpu_ctx->state.reg.sw = (cpu_ctx->state.reg.sw & 0x0FFF) | CS_CARRY;
  else
    update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_S);
}

static int
ex_ms (struct cpu_context *cpu_ctx, ushort opcode) /* C0xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  arith (&cpu_ctx->state, ARI_MULS, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_MS);
}

static int
ex_msr (struct cpu_context *cpu_ctx, ushort opcode) /* C1xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_MULS, VAR_INT, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_MSR);
}

static int
ex_misp (struct cpu_context *cpu_ctx, ushort opcode) /* C2xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help = (short) (lower + 1);
  arith (&cpu_ctx->state, ARI_MULS, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_MISP);
}

static int
ex_misn (struct cpu_context *cpu_ctx, ushort opcode) /* C3xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help = -(short) (lower + 1);
  arith (&cpu_ctx->state, ARI_MULS, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_MISN);
}

static int
ex_m (struct cpu_context *cpu_ctx, ushort opcode) /* C4xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  arith (&cpu_ctx->state, ARI_MUL, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_M);
}

static int
ex_mr (struct cpu_context *cpu_ctx, ushort opcode) /* C5xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_MUL, VAR_INT, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_MR);
}

static int
ex_dm (struct cpu_context *cpu_ctx, ushort opcode) /* C6xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[2];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);

  arith (&cpu_ctx->state, ARI_MUL, VAR_LONG, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_DM);
}

static int
ex_dmr (struct cpu_context *cpu_ctx, ushort opcode) /* C7xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_MUL, VAR_LONG, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_DMR);
}

static int
ex_fm (struct cpu_context *cpu_ctx, ushort opcode) /* C8xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[2];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);

  arith (&cpu_ctx->state, ARI_MUL, VAR_FLOAT, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_FM);
}

static int
ex_fmr (struct cpu_context *cpu_ctx, ushort opcode) /* C9xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_MUL, VAR_FLOAT, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_FMR);
}

static int
ex_efm (struct cpu_context *cpu_ctx, ushort opcode) /* CAxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[3];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr,     &help[0]);
  GET (DATA, addr + 1, &help[1]);
  GET (DATA, addr + 2, &help[2]);

  arith (&cpu_ctx->state, ARI_MUL, VAR_DOUBLE, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_EFM);
}

static int
ex_efmr (struct cpu_context *cpu_ctx, ushort opcode) /* CBxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_MUL, VAR_DOUBLE, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_EFMR);
}

static int
ex_dv (struct cpu_context *cpu_ctx, ushort opcode) /* D0xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  arith (&cpu_ctx->state, ARI_DIVV, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_DV);
}

static int
ex_dvr (struct cpu_context *cpu_ctx, ushort opcode) /* D1xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_DIVV, VAR_INT, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_DVR);
}

static int
ex_disp (struct cpu_context *cpu_ctx, ushort opcode) /* D2xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help = (short) (lower + 1);

  arith (&cpu_ctx->state, ARI_DIVV, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_DISP);
}

static int
ex_disn (struct cpu_context *cpu_ctx, ushort opcode) /* D3xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help = -(short) (lower + 1);

  arith (&cpu_ctx->state, ARI_DIVV, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_DISN);
}

static int
ex_d (struct cpu_context *cpu_ctx, ushort opcode) /* D4xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  arith (&cpu_ctx->state, ARI_DIV, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_D);
}

static int
ex_dr (struct cpu_context *cpu_ctx, ushort opcode) /* D5xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_DIV, VAR_INT, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_DR);
}

static int
ex_dd (struct cpu_context *cpu_ctx, ushort opcode) /* D6xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[2];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);

  arith (&cpu_ctx->state, ARI_DIV, VAR_LONG, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_DD);
}

static int
ex_ddr (struct cpu_context *cpu_ctx, ushort opcode)		/* D7xy */       /* Heute Ostdeutschland! */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_DIV, VAR_LONG, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_DDR);
}

static int
ex_fd (struct cpu_context *cpu_ctx, ushort opcode) /* D8xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[2];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help[0]);
  GET (DATA, addr + 1, &help[1]);

  arith (&cpu_ctx->state, ARI_DIV, VAR_FLOAT, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_FD);
}

static int
ex_fdr (struct cpu_context *cpu_ctx, ushort opcode) /* D9xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_DIV, VAR_FLOAT, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_FDR);
}

static int
ex_efd (struct cpu_context *cpu_ctx, ushort opcode) /* DAxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[3];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr,     &help[0]);
  GET (DATA, addr + 1, &help[1]);
  GET (DATA, addr + 2, &help[2]);

  arith (&cpu_ctx->state, ARI_DIV, VAR_DOUBLE, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_EFD);
}

static int
ex_efdr (struct cpu_context *cpu_ctx, ushort opcode) /* DBxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  arith (&cpu_ctx->state, ARI_DIV, VAR_DOUBLE, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_EFDR);
}

static int
ex_ste (struct cpu_context *cpu_ctx, ushort opcode) /* DCxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  uint laddr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  laddr = (uint) addr;

  if (lower > 0)
    {
      laddr += (uint) cpu_ctx->state.reg.r[lower];
      laddr += (uint) (cpu_ctx->state.reg.r[lower-1] & 0x7F) << 16;
    }
  else
    laddr += (uint) cpu_ctx->state.reg.r[15];

  poke (&cpu_ctx->state, cpu_ctx->state.reg.r[upper], laddr);

  cpu_ctx->state.reg.ic += 2;
  return (nc_ST);  /* in lack of the right timing figure */
}

static int
ex_dste (struct cpu_context *cpu_ctx, ushort opcode) /* DDxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  uint laddr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  laddr = (uint) addr;

  if (lower > 0)
    {
      laddr += (uint) cpu_ctx->state.reg.r[lower];
      laddr += (uint) (cpu_ctx->state.reg.r[lower-1] & 0x7F) << 16;
    }
  else
    laddr += (uint) cpu_ctx->state.reg.r[15];

  poke (&cpu_ctx->state, cpu_ctx->state.reg.r[upper], laddr);
  poke (&cpu_ctx->state, cpu_ctx->state.reg.r[upper+1], laddr + 1);

  cpu_ctx->state.reg.ic += 2;
  return (nc_DST);  /* in lack of the right timing figure */
}

static int
ex_le (struct cpu_context *cpu_ctx, ushort opcode) /* DExy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  uint laddr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  laddr = (uint) addr;

  if (lower > 0)
    {
      laddr += (uint) cpu_ctx->state.reg.r[lower];
      laddr += (uint) (cpu_ctx->state.reg.r[lower-1] & 0x7F) << 16;
    }
  else
    laddr += (uint) cpu_ctx->state.reg.r[15];

  if (peek (&cpu_ctx->state, laddr, (ushort *) &cpu_ctx->state.reg.r[upper]) == 0)
    error ("read error at ic = %04hX\n", cpu_ctx->state.reg.ic);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_L);  /* in lack of the right timing figure */
}

static int
ex_dle (struct cpu_context *cpu_ctx, ushort opcode) /* DFxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  uint laddr;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  laddr = (uint) addr;

  if (lower > 0)
    {
      laddr += (uint) cpu_ctx->state.reg.r[lower];
      laddr += (uint) (cpu_ctx->state.reg.r[lower-1] & 0x7F) << 16;
    }
  else
    laddr += (uint) cpu_ctx->state.reg.r[15];

  if (peek (&cpu_ctx->state, laddr, (ushort *) &cpu_ctx->state.reg.r[upper]) == 0)
    error ("read error at ic = %04hX\n", cpu_ctx->state.reg.ic);
  if (peek (&cpu_ctx->state, laddr + 1, (ushort *) &cpu_ctx->state.reg.r[upper+1]) == 0)
    error ("read error at ic = %04hX\n", cpu_ctx->state.reg.ic);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_LONG);

  cpu_ctx->state.reg.ic += 2;
  return (nc_DL);  /* in lack of the right timing figure */
}

static int
ex_or (struct cpu_context *cpu_ctx, ushort opcode) /* E0xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  cpu_ctx->state.reg.r[upper] |= help;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_OR);
}

static int
ex_orr (struct cpu_context *cpu_ctx, ushort opcode) /* E1xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  cpu_ctx->state.reg.r[upper] |= cpu_ctx->state.reg.r[lower];
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_ORR);
}

static int
ex_and (struct cpu_context *cpu_ctx, ushort opcode) /* E2xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  cpu_ctx->state.reg.r[upper] &= help;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_AND);
}

static int
ex_andr (struct cpu_context *cpu_ctx, ushort opcode) /* E3xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  cpu_ctx->state.reg.r[upper] &= cpu_ctx->state.reg.r[lower];
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_ANDR);
}

static int
ex_xor (struct cpu_context *cpu_ctx, ushort opcode) /* E4xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  cpu_ctx->state.reg.r[upper] ^= help;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_XOR);
}

static int
ex_xorr (struct cpu_context *cpu_ctx, ushort opcode) /* E5xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  cpu_ctx->state.reg.r[upper] ^= cpu_ctx->state.reg.r[lower];
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_XORR);
}

static int
ex_n (struct cpu_context *cpu_ctx, ushort opcode) /* E6xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  cpu_ctx->state.reg.r[upper] = ~(cpu_ctx->state.reg.r[upper] & help);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic += 2;
  return (nc_N);
}

static int
ex_nr (struct cpu_context *cpu_ctx, ushort opcode) /* E7xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  cpu_ctx->state.reg.r[upper] = ~(cpu_ctx->state.reg.r[upper] & cpu_ctx->state.reg.r[lower]);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_NR);
}

static int
ex_fix (struct cpu_context *cpu_ctx, ushort opcode) /* E8xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int help = (int) from_1750flt (&cpu_ctx->state.reg.r[lower]);

  /* To Be Clarified:
     The following behavior is what this operation SHOULD do, but
     *not* what the Fairchild F9450 manual prescribes (the manual says
     FIXOFL occurs, regardless of mantissa, if the exponent of the addr
     number exceeds 2^15). */
  if (help < -32768 || help > 32767)
    cpu_ctx->state.reg.pir |= INTR_FIXOFL;
  else
    cpu_ctx->state.reg.r[upper] = (short) help;
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_FIX);
}

static int
ex_flt (struct cpu_context *cpu_ctx, ushort opcode) /* E9xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  /* To Be Tested  (depends on C hostcompiler cast). */
  to_1750flt ((double) cpu_ctx->state.reg.r[lower], &cpu_ctx->state.reg.r[upper]);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_FLOAT);

  cpu_ctx->state.reg.ic++;
  return (nc_FLT);
}

static int
ex_efix (struct cpu_context *cpu_ctx, ushort opcode) /* EAxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int help;
  /* This time, we do exactly as the F9450 manual prescribes! */
  if ((char) (cpu_ctx->state.reg.r[lower + 1] & 0xFF) > 0x1F)
    cpu_ctx->state.reg.pir |= INTR_FIXOFL;
  else
    {
      help = (int) from_1750eflt (&cpu_ctx->state.reg.r[lower]);
      cpu_ctx->state.reg.r[upper] = (short) (help >> 16);
      cpu_ctx->state.reg.r[upper + 1] = (short) (help & 0xFFFF);
    }
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_LONG);

  cpu_ctx->state.reg.ic++;
  return (nc_EFIX);
}

static int
ex_eflt (struct cpu_context *cpu_ctx, ushort opcode) /* EBxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  int help = ((int) cpu_ctx->state.reg.r[lower] << 16)
	   | ((int) cpu_ctx->state.reg.r[lower+1] & 0xFFFF);

  to_1750eflt ((double) help, &cpu_ctx->state.reg.r[upper]);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_DOUBLE);

  cpu_ctx->state.reg.ic++;
  return (nc_EFLT);
}

static int
ex_xbr (struct cpu_context *cpu_ctx, ushort opcode) /* ECxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  cpu_ctx->state.reg.r[upper] = (cpu_ctx->state.reg.r[upper] << 8) | ((cpu_ctx->state.reg.r[upper] >> 8) & 0x00FF);
  update_cs (&cpu_ctx->state, &cpu_ctx->state.reg.r[upper], VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_XBR);
}

static int
ex_xwr (struct cpu_context *cpu_ctx, ushort opcode) /* EDxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help = cpu_ctx->state.reg.r[lower];

  cpu_ctx->state.reg.r[lower] = cpu_ctx->state.reg.r[upper];
  cpu_ctx->state.reg.r[upper] = help;
  update_cs (&cpu_ctx->state, &help, VAR_INT);

  cpu_ctx->state.reg.ic++;
  return (nc_XWR);
}

static int
ex_c (struct cpu_context *cpu_ctx, ushort opcode) /* F0xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &help);

  compare (&cpu_ctx->state, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_C);
}

static int
ex_cr (struct cpu_context *cpu_ctx, ushort opcode) /* F1xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  compare (&cpu_ctx->state, VAR_INT, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_CR);
}

static int
ex_cisp (struct cpu_context *cpu_ctx, ushort opcode) /* F2xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help = (short) (lower + 1);

  compare (&cpu_ctx->state, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_CISP);
}

static int
ex_cisn (struct cpu_context *cpu_ctx, ushort opcode) /* F3xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  short help = -(short) (lower + 1);

  compare (&cpu_ctx->state, VAR_INT, &cpu_ctx->state.reg.r[upper], &help);

  cpu_ctx->state.reg.ic++;
  return (nc_CISN);
}

static int
ex_cbl (struct cpu_context *cpu_ctx, ushort opcode) /* F4xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr, sw_save = cpu_ctx->state.reg.sw & 0x0FFF;
  short lowlim, highlim;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr, &lowlim);
  GET (DATA, addr + 1, &highlim);

  if (lowlim > highlim)
    cpu_ctx->state.reg.sw = sw_save | CS_CARRY;
  else if (cpu_ctx->state.reg.r[upper] < lowlim)
    cpu_ctx->state.reg.sw = sw_save | CS_NEGATIVE;
  else if (cpu_ctx->state.reg.r[upper] > highlim)
    cpu_ctx->state.reg.sw = sw_save | CS_POSITIVE;
  else
    cpu_ctx->state.reg.sw = sw_save | CS_ZERO;

  cpu_ctx->state.reg.ic += 2;
  return (nc_CBL);
}

static int
ex_ucim (struct cpu_context *cpu_ctx, ushort opcode) /* F5xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort sw_save = cpu_ctx->state.reg.sw & 0x0FFF;
  ushort help1, help2;

  help1 = (ushort) cpu_ctx->state.reg.r[upper];
  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &help2);

  if (help1 < help2)
    cpu_ctx->state.reg.sw = sw_save | CS_NEGATIVE;
  else if (help1 > help2)
    cpu_ctx->state.reg.sw = sw_save | CS_POSITIVE;
  else
    cpu_ctx->state.reg.sw = sw_save | CS_ZERO;

  cpu_ctx->state.reg.ic += 2;
  return (nc_CIM);
}

static int
ex_dc (struct cpu_context *cpu_ctx, ushort opcode) /* F6xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[2];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr,     &help[0]);
  GET (DATA, addr + 1, &help[1]);

  compare (&cpu_ctx->state, VAR_LONG, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_DC);
}

static int
ex_dcr (struct cpu_context *cpu_ctx, ushort opcode) /* F7xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  compare (&cpu_ctx->state, VAR_LONG, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_DCR);
}

static int
ex_fc (struct cpu_context *cpu_ctx, ushort opcode) /* F8xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[2];

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  GET (DATA, addr,     &help[0]);
  GET (DATA, addr + 1, &help[1]);

  compare (&cpu_ctx->state, VAR_FLOAT, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_FC);
}

static int
ex_fcr (struct cpu_context *cpu_ctx, ushort opcode) /* F9xy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  compare (&cpu_ctx->state, VAR_FLOAT, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_FCR);
}

static int
ex_efc (struct cpu_context *cpu_ctx, ushort opcode) /* FAxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort addr;
  short help[3];
  ushort i;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  for (i = 0; i < 3; i++)
    GET (DATA, addr + i, &help[i]);

  compare (&cpu_ctx->state, VAR_DOUBLE, &cpu_ctx->state.reg.r[upper], help);

  cpu_ctx->state.reg.ic += 2;
  return (nc_EFC);
}

static int
ex_efcr (struct cpu_context *cpu_ctx, ushort opcode) /* FBxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  compare (&cpu_ctx->state, VAR_DOUBLE, &cpu_ctx->state.reg.r[upper], &cpu_ctx->state.reg.r[lower]);

  cpu_ctx->state.reg.ic++;
  return (nc_EFCR);
}

static int
ex_ucr (struct cpu_context *cpu_ctx, ushort opcode) /* FCxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort sw_save = cpu_ctx->state.reg.sw & 0x0FFF;
  ushort help1, help2;

  help1 = (ushort) cpu_ctx->state.reg.r[upper];
  help2 = (ushort) cpu_ctx->state.reg.r[lower];
  if (help1 < help2)
    cpu_ctx->state.reg.sw = sw_save | CS_NEGATIVE;
  else if (help1 > help2)
    cpu_ctx->state.reg.sw = sw_save | CS_POSITIVE;
  else
    cpu_ctx->state.reg.sw = sw_save | CS_ZERO;

  cpu_ctx->state.reg.ic++;
  return (nc_CR);
}

static int
ex_uc (struct cpu_context *cpu_ctx, ushort opcode) /* FDxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  ushort sw_save = cpu_ctx->state.reg.sw & 0x0FFF;
  ushort addr;
  ushort help1, help2;

  GET (CODE, cpu_ctx->state.reg.ic + 1, (short *) &addr);
  addr += CHK_RX ();
  help1 = (ushort) cpu_ctx->state.reg.r[upper];
  GET (DATA, addr, (short *) &help2);

  if (help1 < help2)
    cpu_ctx->state.reg.sw = sw_save | CS_NEGATIVE;
  else if (help1 > help2)
    cpu_ctx->state.reg.sw = sw_save | CS_POSITIVE;
  else
    cpu_ctx->state.reg.sw = sw_save | CS_ZERO;

  cpu_ctx->state.reg.ic += 2;
  return (nc_C);
}

static int
ex_nop_bpt (struct cpu_context *cpu_ctx, ushort opcode) /* FFxy */
{
  ushort upper = (opcode & 0x00F0) >> 4;
  ushort lower =  opcode & 0x000F;
  if (upper == 0 && lower == 0)         /* NOP */
    {
      cpu_ctx->state.reg.ic++;
      return (nc_NOP);
    }

  if (upper == 0xF && lower == 0xF)     /* BPT */
    {
      return (BREAKPT);
    }

  return (ex_ill (cpu_ctx, opcode));
}


static int (*exfunc[256])(struct cpu_context *cpu_ctx, ushort opcode) =
  {
    /* 00 - 0F */
    ex_lb,   ex_lb,   ex_lb,   ex_lb,
    ex_dlb,  ex_dlb,  ex_dlb,  ex_dlb,
    ex_stb,  ex_stb,  ex_stb,  ex_stb,
    ex_dstb, ex_dstb, ex_dstb, ex_dstb,
    /* 10 - 1F */
    ex_ab,   ex_ab,   ex_ab,   ex_ab,
    ex_sbb,  ex_sbb,  ex_sbb,  ex_sbb,
    ex_mb,   ex_mb,   ex_mb,   ex_mb,
    ex_db,   ex_db,   ex_db,   ex_db,
    /* 20 - 2F */
    ex_fab,  ex_fab,  ex_fab,  ex_fab,
    ex_fsb,  ex_fsb,  ex_fsb,  ex_fsb,
    ex_fmb,  ex_fmb,  ex_fmb,  ex_fmb,
    ex_fdb,  ex_fdb,  ex_fdb,  ex_fdb,
    /* 30 - 3F */
    ex_orb,  ex_orb,  ex_orb,  ex_orb,
    ex_andb, ex_andb, ex_andb, ex_andb,
    ex_cb,   ex_cb,   ex_cb,   ex_cb,
    ex_fcb,  ex_fcb,  ex_fcb,  ex_fcb,
    /* 40 - 4F */
    ex_brx,  ex_brx,  ex_brx,  ex_brx,
    ex_ill,  ex_ill,  ex_ill,  ex_ill,
    ex_xio,  ex_vio,  ex_imml, ex_ill,
    ex_ill,
#ifdef GVSC
	     ex_esqr, ex_sqrt,
#else
	     ex_ill,  ex_ill,
#endif
			       ex_bif,
    /* 50 - 5F */
    ex_sb,   ex_sbr,  ex_sbi,  ex_rb,
    ex_rbr,  ex_rbi,  ex_tb,   ex_tbr,
    ex_tbi,  ex_tsb,  ex_svbr, ex_ill,
    ex_rvbr, ex_ill,  ex_tvbr, ex_ill,
    /* 60 - 6F */
    ex_sll,  ex_srl,  ex_sra,  ex_slc,
    ex_ill,  ex_dsll, ex_dsrl, ex_dsra,
    ex_dslc, ex_ill,  ex_slr,  ex_sar,
    ex_scr,  ex_dslr, ex_dsar, ex_dscr,
    /* 70 - 7F */
    ex_jc,   ex_jci,  ex_js,   ex_soj,
    ex_br,   ex_bez,  ex_blt,  ex_bex,
    ex_ble,  ex_bgt,  ex_bnz,  ex_bge,
    ex_lsti, ex_lst,  ex_sjs,  ex_urs,
    /* 80 - 8F */
    ex_l,    ex_lr,   ex_lisp, ex_lisn,
    ex_li,   ex_lim,  ex_dl,   ex_dlr,
    ex_dli,  ex_lm,   ex_efl,  ex_lub,
    ex_llb,  ex_lubi, ex_llbi, ex_popm,
    /* 90 - 9F */
    ex_st,   ex_stc,  ex_stci, ex_mov,
    ex_sti,
#ifdef GVSC
	     ex_sfbs,
#else
	     ex_ill,
#endif
		      ex_dst,  ex_srm,
    ex_dsti, ex_stm,  ex_efst, ex_stub,
    ex_stlb, ex_subi, ex_slbi, ex_pshm,
    /* A0 - AF */
    ex_a,    ex_ar,   ex_aisp, ex_incm,
    ex_abs,  ex_dabs, ex_da,   ex_dar,
    ex_fa,   ex_far,  ex_efa,  ex_efar,
    ex_fabs,
#ifdef GVSC
	     ex_uar,  ex_ua,
#else
	     ex_ill,  ex_ill,
#endif
			       ex_ill,
    /* B0 - BF */
    ex_s,    ex_sr,   ex_sisp, ex_decm,
    ex_neg,  ex_dneg, ex_ds,   ex_dsr,
    ex_fs,   ex_fsr,  ex_efs,  ex_efsr,
    ex_fneg,
#ifdef GVSC
	     ex_usr,  ex_us,
#else
	     ex_ill,  ex_ill,
#endif
			       ex_ill,
    /* C0 - CF */
    ex_ms,   ex_msr,  ex_misp, ex_misn,
    ex_m,    ex_mr,   ex_dm,   ex_dmr,
    ex_fm,   ex_fmr,  ex_efm,  ex_efmr,
    ex_ill,  ex_ill,  ex_ill,  ex_ill,
    /* D0 - DF */
    ex_dv,   ex_dvr,  ex_disp, ex_disn,
    ex_d,    ex_dr,   ex_dd,   ex_ddr,
    ex_fd,   ex_fdr,  ex_efd,  ex_efdr,
#ifdef GVSC
    ex_ste,  ex_dste, ex_le,   ex_dle,
#else
    ex_ill,  ex_ill,  ex_ill,  ex_ill,
#endif
    /* E0 - EF */
    ex_or,   ex_orr,  ex_and,  ex_andr,
    ex_xor,  ex_xorr, ex_n,    ex_nr,
    ex_fix,  ex_flt,  ex_efix, ex_eflt,
    ex_xbr,  ex_xwr,  ex_ill,  ex_ill,
    /* F0 - FF */
    ex_c,    ex_cr,   ex_cisp, ex_cisn,
    ex_cbl,
#ifdef GVSC
	     ex_ucim,
#else
	     ex_ill,
#endif
		      ex_dc,   ex_dcr,
    ex_fc,   ex_fcr,  ex_efc,  ex_efcr,
#ifdef GVSC
    ex_ucr, ex_uc,
#else
    ex_ill, ex_ill,
#endif
		      ex_ill,  ex_nop_bpt
  };


int 
execute (struct cpu_context *cpu_ctx)
{
  unsigned opc_hibyte;
  int cycles;

  if (! need_speed)
    add_to_backtrace (cpu_ctx);
  struct cpu_state *cpu = &cpu_ctx->state;
  ushort opcode;
  GET (CODE, cpu_ctx->state.reg.ic, (short *) &opcode);
  opc_hibyte = opcode >> 8;

  cycles = (*exfunc[opc_hibyte]) (cpu_ctx, opcode);
  if (cycles < 0)
    return cycles;  /* BREAKPT or MEMERR */

  cpu_ctx->state.instcnt++;
  //cpu_ctx->state.total_time_in_us += (double)(uP_CYCLE_IN_NS * cycles) / 1000.0;
  cpu_ctx->state.total_cycles += cycles;
  workout_timing (cpu, cycles);
  workout_interrupts (cpu);
  return OKAY;
}

