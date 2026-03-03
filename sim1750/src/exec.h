/* exec.h  --  exports of exec.c */

extern int  si_go (struct cpu_state *cpu, int argc, char *argv[]);
extern int  si_snglstp (struct cpu_state *cpu, int argc, char *argv[]);
extern int  si_trace (struct cpu_state *cpu, int argc, char *argv[]);

extern int  si_bt (struct cpu_state *cpu, int argc, char *argv[]);

