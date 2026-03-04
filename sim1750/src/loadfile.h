/* loadfile.h  --  exports of loadfile.c */

#ifndef _LOADFILE_H
#define _LOADFILE_H

extern char *find_labelname (unsigned int address);
extern int  find_address (char *labelname);
extern void init_load_formats ();
extern int  si_dispsym (int argc, char *argv[]);
extern int  si_prolo (struct cpu_state *cpu, int argc, char *argv[]);
extern int  si_pslo  (struct cpu_state *cpu, int argc, char *argv[]);
extern int  si_lo    (struct cpu_state *cpu, int argc, char *argv[]);	/* tekops.c */
extern int  si_save  (struct cpu_state *cpu, int argc, char *argv[]);	/* tekops.c */
extern int  si_lcf (struct cpu_state *cpu, int argc, char *argv[]);	/* load_coff.c */
extern int  si_tld (struct cpu_state *cpu, int argc, char *argv[]);	/* ldm.c */
extern int  si_xtc (struct cpu_state *cpu, int argc, char *argv[]);	/* ldm.c */

typedef enum { TLD_LDM, XTC_LDM, TEK_HEX, COFF, NONE} loadfile_t;

extern loadfile_t loadfile_type;

#endif

