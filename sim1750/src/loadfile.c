/***************************************************************************/
/*                                                                         */
/* Project   :        sim1750 -- Mil-Std-1750 Software Simulator           */
/*                                                                         */
/* Component :        loadfile.c -- loadfile related functions             */
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

#include "status.h"
#include "phys_mem.h"
#include "cpu_ctx.h"
#include "utils.h"
#include "tekops.h"
#include "coffops.h"
#include "loadfile.h"
#include "peekpoke.h"

extern struct cpu_context *sim_cpu_ctx;
/* Put a binary 16-bit word in high byte, low byte order to a file.
   Return 0 for success, EOF on error. */
int
put_hilo16 (ushort word, FILE *fp)
{
  if (putc ((int) (word >> 8), fp) == EOF)
    return EOF;
  if (putc ((int) (word & 0xFF), fp) == EOF)
    return EOF;
  return 0;
}

/* Get a binary 16-bit word in high byte, low byte order from file.
   Return 0 for success, or EOF on end-of-file. */
int
get_hilo16 (ushort *wordpointer, FILE *fp)
{
  unsigned char two_bytes[2];

  if (fread (two_bytes, 1, 2, fp) != 2)
    return EOF;
  *wordpointer = ((ushort) two_bytes[0] << 8) | (ushort) two_bytes[1];
  return 0;
}


void
init_load_formats (struct cpu_context *ctx)
{
  loadfile_type = NONE;
  free_symtab(ctx);
  /* insert further loadfile specific initialization calls if needed */
}


loadfile_t loadfile_type = NONE;




char *find_labelname(struct cpu_context *ctx, uint address) {
    if (ctx->symtab.ops && ctx->symtab.ops->find_label)
        return ctx->symtab.ops->find_label(ctx->symtab.data, address);
    return NULL;
}


int find_address(struct cpu_context *ctx, const char *labelname) {
    if (ctx->symtab.ops && ctx->symtab.ops->find_address)
        return ctx->symtab.ops->find_address(ctx->symtab.data, labelname);
    return -1;
}

void free_symtab(struct cpu_context *ctx) {
    if (ctx->symtab.ops && ctx->symtab.ops->free_data) {
        ctx->symtab.ops->free_data(ctx->symtab.data);
        ctx->symtab.data = NULL;
        ctx->symtab.ops = NULL;
    }
}


/* display symbols from loadfile last loaded */

int si_dispsym(int argc, char *argv[]) {
    if (sim_cpu_ctx->symtab.ops && sim_cpu_ctx->symtab.ops->display_symbols)
        return sim_cpu_ctx->symtab.ops->display_symbols(sim_cpu_ctx->symtab.data);
    return OKAY;
}
#if 0
int
si_dispsym (int argc, char *argv[])
{
  switch (loadfile_type)
    {
    case TEK_HEX:
      return display_tek_symbols ();
    case COFF:
      return display_coff_symbols ();
    default:
      return OKAY;
    }
}
#endif


/* load absolute binary PROM image file */

int
si_prolo (int argc, char *argv[])
{
  ushort word;
  uint  abs_addr = 0;
  char  *filename = argv[1];
  FILE  *loadfile;
  bool   verbose_save = verbose;

  if (argc <= 1)
    return error ("filename missing");
  if (*filename == '"')
    {
      *filename++ = '\0';
      *(filename + strlen (filename) - 1) = '\0';
    }
  if ((loadfile = fopen (filename, "rb")) == NULL)
    return error ("cannot open load file '%s'", filename);
  verbose = FALSE;  /* avoid info messages from poke() */
  while (get_hilo16 (&word, loadfile) != EOF)
    {
      poke (&sim_cpu_ctx->state, abs_addr, word);
      abs_addr++;
    }
  fclose (loadfile);
  verbose = verbose_save;
  return (0);
}


/* load binary file in IBM User Console PS (Program Store) format */

int
si_pslo (int argc, char *argv[])
{
  ushort loadaddr_hiword, loadaddr_loword, n_words, i, word;
  uint  abs_addr = 0;
  bool status = OKAY, verbose_save = verbose;
  char *filename = argv[1];
  FILE  *loadfile;

  if (argc <= 1)
    return error ("filename missing");
  if (*filename == '"')
    {
      *filename++ = '\0';
      *(filename + strlen (filename) - 1) = '\0';
    }
  if ((loadfile = fopen (filename, "rb")) == NULL)
    return error ("cannot open load file '%s'", filename);
  verbose = FALSE;  /* avoid info messages from poke() */
  while (! feof (loadfile))
    {
      if (get_hilo16 (&loadaddr_hiword, loadfile) == EOF)
	{
/*	  error ("pslo: unexpected end-of-file reading load address high word");
 */
	  break;
	}
      if (loadaddr_hiword > 1)
	{
	  status = error ("pslo: cannot load code above 0x1FFFF");
	  break;
	}
      if (get_hilo16 (&loadaddr_loword, loadfile) == EOF)
	{
	  status = error ("pslo: unexpected EOF reading load address low word");
	  break;
	}
      abs_addr = (uint) loadaddr_hiword << 16 | (uint) loadaddr_loword;
      if (get_hilo16 (&n_words, loadfile) == EOF)
	{
	  status = error ("pslo: unexpected end-of-file reading wordcount");
	  break;
	}
      for (i = 0; i < n_words; i++)
	{
	  if (get_hilo16 (&word, loadfile) == EOF)
	    {
	      status = error ("pslo: unexpected EOF while reading data");
	      break;
	    }
	  poke (&sim_cpu_ctx->state, abs_addr++, word);
	}
      if (status == ERROR)
	break;
    }
/*
  if (status != OKAY)
    error ("  above error occurred at filepos = %ld\n", ftell (loadfile));
 */
  fclose (loadfile);
  verbose = verbose_save;
  return (status);
}
