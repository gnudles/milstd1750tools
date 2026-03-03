/***************************************************************************/
/*                                                                         */
/* Project   :        sim1750 -- Mil-Std-1750 Software Simulator           */
/*                                                                         */
/* Component :    smemacc.c -- simulation-level memory access functions    */
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


#include "cpu.h"
#include "peekpoke.h"

uint
get_phys_address (struct cpu_state *cpu, int bank, ushort as, ushort log_addr)
{
  ushort page = cpu->pagereg[bank][(int) as][(int) (log_addr >> 12) & 0xF].ppa;
  return ((uint) page << 12) | (uint) (log_addr & 0x0FFF);
}

bool
get_raw (struct cpu_state *cpu, int bank, ushort as, ushort address, ushort *value)
{
  uint phys_address = get_phys_address (cpu, bank, as, address);
  return peek (cpu, phys_address, value);
}

void
store_raw (struct cpu_state *cpu, int bank, ushort as, ushort address, ushort value)
{
  uint phys_address = get_phys_address (cpu, bank, as, address);
  poke (cpu, phys_address, value);
}

