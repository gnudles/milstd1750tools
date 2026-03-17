/***************************************************************************/
/*                                                                         */
/* Project   :        sim1750 -- Mil-Std-1750 Software Simulator           */
/*                                                                         */
/* Component :       arch.h -- processor architectural definitions         */
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


#ifndef _ARCH_H
#define _ARCH_H

#include "type.h"
#include "phys_mem.h"

/* condition status bit masks (within Status Register) */
#define  CS_CARRY     0x8000
#define  CS_POSITIVE  0x4000
#define  CS_ZERO      0x2000
#define  CS_NEGATIVE  0x1000
#define  CS_ERROR     0x0000

#define MP_PROC 0 /* mem protect for processor */
#define MP_DMA 1 /* mem protect for dma */
/* mask for simreg.sys */
#define  SYS_INT   0x1
#define  SYS_DMA   0x2 /* DMA Enabled*/
#define  SYS_TA    0x4
#define  SYS_TB    0x8
#define  SYS_MEM_PROT   0x10 /* Mem Protect Enabled*/
#define  SYS_SUROM    0x20 /* currently SUROM is not implemented */



/* mask for simreg.ft */

/*
Bit 0:		CPU Memory Protection Fault.  The CPU has encountered an access
		fault, write protect fault, or execute protect fault.

Bit 1:		DMA Memory Protection Fault.  A DMA device has encountered an 
		access fault or a write protect fault.

Bit 2:		Memory Parity Fault.

Bit 3:		PIO Channel Parity Fault.

Bit 4: 	 	DMA Channel Parity Fault.

Bit 5:  	Illegal I/O Command Fault.  An attempt has been made to execute
		an unimplemented or reserved I/O command.

Bit 6:  	PIO Transmission Fault.  Other I/O error checking devices, if 
		used, may be ORed into this bit to indicate an error.

Bit 7:  	Spare.

Bit 8:  	Illegal Address Fault.  A memory location has been addressed 
		which is not physically present.

Bit 9:  	Illegal Instruction Fault.  An attempt has been made to execute
		a reserved code.

Bit 10:		Privileged Instruction Fault.  An attempt has been made to 
		execute a privileged instruction with PS != 0.

Bit 11: 	Address State Fault.  An attempt has been made to establish an
		AS value for an unimplemented page register set.

Bit 12: 	Reserved.

Bit 13: 	Built-in Test Fault.  Hardware built-in test equipment (BITE) 
		error has been detected.

Bit 14-15:	Spare BITE.  These bits are for use by the designer for future
		defining (coding, etc.) the BITE error which is detected.  This
		can be used with Bit 13 to give a more complete error description.

*/
#define  FT_MEMPROT	0x8000   /* CPU memory protection error */
#define  FT_ILL_IO	0x0400   /* illegal XIO address */
#define  FT_SYSFAULT0	0x0100   /* sysfault 0 watchdog (F9450/MODUS) */
#define  FT_ILL_ADDR	0x0080   /* illegal instruction/operand address */
#define  FT_ILL_INSTR	0x0040   /* illegal instruction opcode */
#define  FT_PRIV_INSTR	0x0020   /* privileged instruction (AK!=0) */

/* interrupts in PIR */
#define  INTR_NULL      0x0000
#define  INTR_PWRDWN    0x8000
#define  INTR_MACHERR   0x4000
#define  INTR_USER0     0x2000
#define  INTR_FLTOFL    0x1000
#define  INTR_FIXOFL    0x0800
#define  INTR_BEX       0x0400
#define  INTR_FLTUFL    0x0200
#define  INTR_TA        0x0100
#define  INTR_USER1     0x0080
#define  INTR_TB        0x0040
#define  INTR_USER2     0x0020
#define  INTR_USER3     0x0010
#define  INTR_IOLVL1    0x0008
#define  INTR_USER4     0x0004
#define  INTR_IOLVL2    0x0002
#define  INTR_USER5     0x0001

/* memory banks: instruction page = CODE, operand page = DATA */
#define  CODE     0
#define  DATA     1

/* define arithmetic operations for sim_arith */
typedef enum { ARI_ADD, ARI_SUB, ARI_MULS, ARI_MUL, ARI_DIVV, ARI_DIV }
	operation_kind;

/* define types of variables for sim_arith */
typedef enum { VAR_INT, VAR_LONG, VAR_FLOAT, VAR_DOUBLE } datatype;

#if defined(_MSC_VER)
#include <intrin.h>
#endif

/* Count Leading Zeros for 16-bit integers */
static inline int count_leading_zeros(uint16_t x) {
    if (x == 0) return 16;
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_clz(x) - 16;
#elif defined(_MSC_VER)
    unsigned long index;
    _BitScanReverse(&index, x);
    return 15 - index;
#else
    int n = 0;
    if ((x & 0xFF00) == 0) { n += 8; x <<= 8; }
    if ((x & 0xF000) == 0) { n += 4; x <<= 4; }
    if ((x & 0xC000) == 0) { n += 2; x <<= 2; }
    if ((x & 0x8000) == 0) { n += 1; }
    return n;
#endif
}
#define TIM_A 0
#define TIM_B 1


/* Simulator register file */
struct regs
  {
    short  r[16];  /* 0..15 */
    union{
    struct { /* These 3 are one unit in this specific order */
    ushort mk;     /* 16 */
    ushort sw;     /* 17 */
    ushort ic;     /* 18 */
    };
    ushort mk_sw_ic[3];
    };
    ushort pir;
    ushort pir_update;    
    ushort ft;
    ushort timer[2]; 
    ushort timer_reset_val[2]; // timers will reset to this value after roll over
    ushort go;     /* not a real register but handled like TA/TB */
    ushort timer_go_reset_val; // when issuing timer go reset, it will reset to this value
    ushort sys;    /* system configuration register */
    ushort sys_update; /* when interrupts are being enabled, the change only take effect after next instruction*/
    ushort ioic_l1; /* ioic level 1 */
    ushort ioic_l2; /* ioic level 2 */
    ushort dsctout; /* discretes output */
    ushort dsctin; /* discretes input */
    int64_t accumulator; /*used in PACE1750AE (48bit accumulator) */
  };

/* extern struct regs simreg; defined in cpu.c */

/* MMU related */
struct mmureg
  {
    union
      {
        struct /*swap if simulator runs on big endian system*/
          {
          ushort ppa      : 8;
          ushort reserved : 3;
          ushort e_w      : 1;
          ushort al       : 4;
          };
          ushort word;
      };
  };
  /* will allow us to skip checks*/
struct mem_access_cache_r
{
  uint8_t page[16]; /* maps logical to physical*/
  uint16_t valid; /*one bit per page*/
};
struct mem_access_cache_w
{
  uint8_t page[16]; /* maps logical to physical*/
  uint64_t valid; /*one bit per qpage*/
};
/* extern struct mmureg pagereg[2][16][16]; defined in cpu.c */
struct cpu_state {
  mem_t *mem[N_PAGES];
  ushort mem_protect[2][64]; // write protection. first for cpu, second for dma.
  struct mmureg pagereg[2][16][16];
  struct mem_access_cache_r data_read_cache;
  struct mem_access_cache_r data_read_cache_intr;
  struct mem_access_cache_r code_read_cache;
  struct mem_access_cache_w data_write_cache;
  struct regs reg;

  /* A quickie for communication between workout_interrupts() and ex_bex() */ 

  uint num_phys_mem_pages; // size of installed memory, in pages (4096 words per page)
  uint instcnt;

  uint64_t total_cycles;
  uint64_t next_scheduled_timer_calc_cycles;
  uint64_t total_cycles_timers_snap;
  uint64_t timer_ns_remainder;
  uint32_t global_10usec_timer_clock;
  uint32_t timer_b_global_snap;
  uint32_t timer_go_global_snap;


  ushort bex_index;
  bool halt;
  bool disable_timers;
  /*old timing mechanism */
  struct {
    ushort one_tatick_in_ns, one_tbtick_in_tatix;
    ushort one_gotick_in_10usec;
  } timers;
  int32_t  quantum_left; /* cycles left in current quantum, used for sequential execution of multiple cpus, every cpu gets to execute for a fixed number of cycles */
};

#endif

