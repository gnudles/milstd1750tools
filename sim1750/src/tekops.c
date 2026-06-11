/***************************************************************************/
/*                                                                         */
/* Project   :        sim1750 -- Mil-Std-1750 Software Simulator           */
/*                                                                         */
/* Component :   tekops.c -- Tektronix Extended Hex related functions      */
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
#include <stdlib.h>

#include "arch.h"
#include "cmd.h"  /* for sys_int() */
#include "phys_mem.h"
#include "smemacc.h"
#include "status.h"
#include "utils.h"
#include "peekpoke.h"
#include "loadfile.h"
#include "tekhex.h"


/*extern struct regs simreg;*/  /* from cpu.c */
extern struct cpu_context *sim_cpu_ctx;

/* Internal data */

#define MAX_SECTIONS 100

struct section
  {
    char  *name;
    uint base_addr;
    int   length;
  };

struct symbol
  {
    char   *name;
    int    type;
    uint  value;
    struct section *sect;
  };


struct tek_symtab {
    struct section section[MAX_SECTIONS];
    int n_sections;
    struct {
        int n_allocated;
        int n_used;
        struct symbol *sym;
    } symdata;
};

static void tek_free_data(void *data) {
    struct tek_symtab *tek = (struct tek_symtab *)data;
    for (int i = 0; i < tek->n_sections; i++)
        free(tek->section[i].name);
    for (int i = 0; i < tek->symdata.n_used; i++)
        free(tek->symdata.sym[i].name);

    /*if (tek->section) free(tek->section);*/
    if (tek->symdata.sym) free(tek->symdata.sym);
    free(tek);
}



/* TekHex symbol type codes */
#define GLOBAL_ADDRESS		1
#define GLOBAL_SCALAR		2
#define GLOBAL_CODE_ADDRESS	3
#define GLOBAL_DATA_ADDRESS	4
#define LOCAL_ADDRESS		5
#define LOCAL_SCALAR		6
#define LOCAL_CODE_ADDRESS	7
#define LOCAL_DATA_ADDRESS	8
static const char *typename[] =
     {
       "",
       "GLOBAL_ADDRESS",
       "GLOBAL_SCALAR",
       "GLOBAL_CODE_ADDRESS",
       "GLOBAL_DATA_ADDRESS",
       "LOCAL_ADDRESS",
       "LOCAL_SCALAR",
       "LOCAL_CODE_ADDRESS",
       "LOCAL_DATA_ADDRESS"
     };


void
init_tekops (void * data)	/* free up previous memory allocations; initialize */
{
    struct tek_symtab* tek = (struct tek_symtab*) data;
  while (tek->n_sections > 0)
    free ((void *) tek->section[--tek->n_sections].name);
  while (tek->symdata.n_used > 0)
    free ((void *) tek->symdata.sym[--tek->symdata.n_used].name);
  if (tek->symdata.n_allocated == 0)
    {
      tek->symdata.n_allocated = 42;  /* rat science */
      tek->symdata.sym = (struct symbol *) calloc (tek->symdata.n_allocated,
					      sizeof (struct symbol));
    }
}


int
find_tek_address (void * data, const char *labelname)
{
  int i;
  struct tek_symtab* tek = (struct tek_symtab*) data;

  for (i = 0; i < tek->symdata.n_used; i++)
    if (eq (labelname, tek->symdata.sym[i].name))
      return (int) tek->symdata.sym[i].value;
  return -1;
}


char *
find_tek_label (void * data, uint address)
{
  struct tek_symtab* tek = (struct tek_symtab*) data;
  int i = tek->symdata.n_used;

  while (i-- > 0)
    if (tek->symdata.sym[i].value == address)
      return tek->symdata.sym[i].name;
  return NULL;
}


#define x_nib(ch) (isdigit (ch) ? (ch) - '0' :		\
		   isupper (ch) ? (ch) - ('A' - 10) :	\
		   islower (ch) ? (ch) - ('a' - 10) : -1)

static int
get_xnum (char **string, int number)
{
  int nibble;
  uint retval = 0;
  char *p = *string;

  while (number-- > 0)
    {
      nibble = *p++;
      if ((nibble = x_nib (nibble)) < 0)
	return -1;
      retval = (retval << 4) | (uint) (nibble & 0xF);
    }
  *string = p;
  return (retval);
}


static void
add_symbol (struct tek_symtab* tek, char *name, int type, ushort value, struct section *section)
{
  int n;

  if ((n = tek->symdata.n_used) == tek->symdata.n_allocated)
    {
      tek->symdata.n_allocated *= 2;
      if ((tek->symdata.sym = (struct symbol *) realloc (tek->symdata.sym,
	 tek->symdata.n_allocated * sizeof (struct symbol))) == (struct symbol *) 0)
	problem ("tekhex: request for symbol space refused by OS");
    }
  tek->symdata.sym[n].name = strdup (name);
  tek->symdata.sym[n].type = type;
  tek->symdata.sym[n].sect = section;
  tek->symdata.sym[n].value = value;
  tek->symdata.n_used++;
}


static int linecount;

/* Analyze and load a line from a Tektronix Extended Hex file */

int
load_tekline (struct tek_symtab* tek, char *line)
{
  char  sectname[32], *linep;
  int   i, checksum, type, blk_len, addr_len, line_len;
  uint address;

  if (sys_int (1))
    return (INTERRUPT);

  if (line[0] != '%')
    return error ("illegal format");
  line_len = strlen (line) - 2;
  linep = line + 1;
  blk_len = (int) get_xnum (&linep, 2);
  type = (int) get_xnum (&linep, 1);
  checksum = (int) get_xnum (&linep, 2);
  addr_len = (int) get_xnum (&linep, 1);
  if (addr_len == 0)
    addr_len = 16;

  if (line_len == blk_len)
    {
      if (checksum != (check_tekline (line) & 0xff))
	info ("checksum error in line %d", linecount);
    }
  else
    return error ("line length error in line %d", linecount);
  switch (type)
    {
    case 3:		/* Symbol */
      strncpy (sectname, linep, addr_len);
      sectname[addr_len] = '\0';
      linep += addr_len;
      while (linep - line < line_len)
	{
	  type = (int) get_xnum (&linep, 1);	/* symbol type */
	  if (type == 0)	/* section definition */
	    {
	      int   baseaddr_len = (int) get_xnum (&linep, 1);
	      uint baseaddr     =       get_xnum (&linep, baseaddr_len);
	      int   sectlen_len  = (int) get_xnum (&linep, 1);
	      uint sectlen      =       get_xnum (&linep, sectlen_len);

	      /* lprintf ("\nSEC %s %s\n", sectname, linep); */
	      tek->section[tek->n_sections].name = strdup (sectname);
	      tek->section[tek->n_sections].base_addr = baseaddr;
	      tek->section[tek->n_sections].length = sectlen;
	      tek->n_sections++;
	    }
	  else			/* symbol definition */
	    {
	      int len, val_len;
	      char sym[40];
	      uint val;

	      len = (int) get_xnum (&linep, 1); /* symbol name length */
	      if (len == 0)
		len = 16;
	      strncpy (sym, linep, len);	/* symbol name */
	      sym[len] = '\0';
	      linep += len;
	      val_len = (int) get_xnum (&linep, 1);	/* value length */
	      val = get_xnum (&linep, val_len);		/* value */
	      /* lprintf ("ELM %s %s %s\n", sym, sectname, linep); */
	      add_symbol (tek, sym, type, val, &tek->section[tek->n_sections-1]);
	    }
	}
      break;

    case 6:		/* Data */
      address = get_xnum (&linep, addr_len);
      i = (ushort) address & 0x0001;
      address >>= 1;
      while (linep - line < line_len)
	{
	  ushort value;
	  if (i)
	    {
	      peek (&sim_cpu_ctx->state, address, &value);
	      poke (&sim_cpu_ctx->state, address, (value & 0xff00)
			   | (ushort) get_xnum (&linep, 2));
	      i = 0;
	      address++;
	    }
	  else
	    {
	      peek (&sim_cpu_ctx->state, address, &value);
	      poke (&sim_cpu_ctx->state, address, (value & 0x00ff)
			   | (ushort) (get_xnum (&linep, 2) << 8));
	      i++;
	    }
	}
      break;

    case 8:		/* Terminator */
      address = get_xnum (&linep, addr_len);
      sim_cpu_ctx->state.reg.ic = (ushort) ((address >> 1) & 0xffff);
      break;

    default:
      return error ("illegal type in line %d", linecount);
    }
  return (OKAY);
}


/* Display symbols loaded from a TekHex loadfile */

int
display_tek_symbols  (void * data)
{
  int i;
  struct tek_symtab* tek = (struct tek_symtab*) data;
  for (i = 0; i < tek->n_sections; i++)
    {
      if (i == 0)
	lprintf ("Section             BaseAddr Length\n");
      lprintf ("%-20s  %04hXh     %d\n", tek->section[i].name,
		tek->section[i].base_addr, tek->section[i].length);
    }
  for (i = 0; i < tek->symdata.n_used; i++)
    {
      if (i == 0)
	{
	  lprintf ("-------------------------------------------------------\n");
	  lprintf ("Symbolname           Value Type                 Section\n");
	}
      lprintf ("%-20s %05X %-20s %s\n", tek->symdata.sym[i].name,
		tek->symdata.sym[i].value, typename[tek->symdata.sym[i].type],
		tek->symdata.sym[i].sect->name);
    }
  return (OKAY);
}


static const struct symbol_ops tek_ops = {
    find_tek_address,
    find_tek_label,
    display_tek_symbols,
    tek_free_data
};


/* load file in Tektronix Extended Hex format */

int
si_lo (int argc, char *argv[])
{
  FILE *fpoint;
  char lline[132], *filename = argv[1];
  bool verbose_save = verbose;
  int retval = ERROR;

  if (argc <= 1)
    return error ("filename missing");

  if (*filename == '"')
    {
      *filename++ = '\0';
      *(filename + strlen (filename) - 1) = '\0';
    }
  if ((fpoint = fopen (filename, "r")) == NULL)
  {
    strcat (strcpy (lline, filename), ".hex");
    if ((fpoint = fopen (lline, "r")) == NULL)
      return error ("cannot open load file '%s'", filename);
  }
  verbose = FALSE;  /* avoid info message from poke() */
  loadfile_type = TEK_HEX;
  linecount = 0;

  free_symtab(sim_cpu_ctx);
  sim_cpu_ctx->symtab.ops = &tek_ops;
  struct tek_symtab * tek = (struct tek_symtab *)calloc(1, sizeof(struct tek_symtab));
  init_tekops(tek);
  sim_cpu_ctx->symtab.data = tek;


  while (fgets (lline, sizeof (lline), fpoint) != NULL)
    {
      ++linecount;
      if (strlen (lline) < 2)
	continue;
      if ((retval = load_tekline (tek, lline)) != OKAY)
	break;
    }
  fclose (fpoint);
  verbose = verbose_save;
  return retval;
}



int
si_save (int argc, char *argv[])
{
  int pgndx, locndx;
  mem_t *memptr;

  if (argc <= 1)
    return error ("filename missing");
  if (create_tekfile (argv[1]) != OKAY)
    return error ("cannot open save file '%s'", argv[1]);

  for (pgndx = 0; pgndx < N_PAGES; pgndx++)
    {
      uint phys_address = pgndx << 12;
      if ((memptr = sim_cpu_ctx->state.mem[pgndx]) == MNULL)
	continue;
      for (locndx = 0; locndx < 4096; locndx++)
	{
	  if (memptr->was_written[locndx / 32] & (1 << (locndx % 32)))
	    emit_tekword (phys_address + locndx, memptr->word[locndx]);
	}
    }

  close_tekfile ((uint) sim_cpu_ctx->state.reg.ic);  /* other regs are lost, to be improved */

  return OKAY;
}
