/* derived from stime.h */

/* Manufacturers' processor-timing info */

#if defined (PACE)
       /* Notes:
	  1. Figures given are for one wait state in both instruction fetch
	     and data read/write.
          2. Instructions appear in the same order as in Table 2.2
             of Performance Semiconductor's PACE1750A product description. */

/* Integer arithmetic and logic: */
#define CLK_CYC_AR      5
#define CLK_CYC_AB     13
#define CLK_CYC_ABX    13
#define CLK_CYC_AISP    8
#define CLK_CYC_A      15
#define CLK_CYC_AIM    10
#define CLK_CYC_DAR     9
#define CLK_CYC_DA     24
#define CLK_CYC_SR      5
#define CLK_CYC_SBB    13
#define CLK_CYC_SBBX   13
#define CLK_CYC_SISP    8
#define CLK_CYC_S      15
#define CLK_CYC_SIM    10
#define CLK_CYC_DSR     9
#define CLK_CYC_DS     24
#define CLK_CYC_MSR    23
#define CLK_CYC_MISP   26
#define CLK_CYC_MISN   26
#define CLK_CYC_MS     33
#define CLK_CYC_MSIM   28
#define CLK_CYC_MR     26
#define CLK_CYC_MB     34
#define CLK_CYC_MBX    34
#define CLK_CYC_M      36
#define CLK_CYC_MIM    31
#define CLK_CYC_DMR    69
#define CLK_CYC_DM     84
#define CLK_CYC_DVR    58
#define CLK_CYC_DISP   61
#define CLK_CYC_DISN   61
#define CLK_CYC_DV     68
#define CLK_CYC_DVIM   63
#define CLK_CYC_DR     73
#define CLK_CYC_DB     81
#define CLK_CYC_DBX    81
#define CLK_CYC_D      83
#define CLK_CYC_DIM    78
#define CLK_CYC_DDR   133
#define CLK_CYC_DD    148
#define CLK_CYC_INCM   18
#define CLK_CYC_DECM   20
#define CLK_CYC_ABS(negative)   (negative ?  9 : 6)
#define CLK_CYC_DABS(negative)  (negative ? 12 : 9)
#define CLK_CYC_NEG     5
#define CLK_CYC_DNEG    9
#define CLK_CYC_CR      5
#define CLK_CYC_CB     13
#define CLK_CYC_CBX    13
#define CLK_CYC_CISP    8
#define CLK_CYC_CISN    8
#define CLK_CYC_C      15
#define CLK_CYC_CIM    10
#define CLK_CYC_CBL    16
#define CLK_CYC_DCR     6
#define CLK_CYC_DC     21
#define CLK_CYC_ORR     5
#define CLK_CYC_ORB    13
#define CLK_CYC_ORBX   13
#define CLK_CYC_OR     15
#define CLK_CYC_ORIM   10
#define CLK_CYC_XORR    5
#define CLK_CYC_XOR    15
#define CLK_CYC_XORM   10
#define CLK_CYC_ANDR    5
#define CLK_CYC_ANDB   13
#define CLK_CYC_ANDX   13
#define CLK_CYC_AND    15
#define CLK_CYC_ANDM   10
#define CLK_CYC_NR      5
#define CLK_CYC_N      15
#define CLK_CYC_NIM    10
/* Floating point arithmetic */
#define CLK_CYC_FAR    28
#define CLK_CYC_FAB    41
#define CLK_CYC_FABX   41
#define CLK_CYC_FA     43
#define CLK_CYC_EFAR   50
#define CLK_CYC_EFA    70
#define CLK_CYC_FSR    28
#define CLK_CYC_FSB    41
#define CLK_CYC_FSBX   41
#define CLK_CYC_FS     43
#define CLK_CYC_EFSR   50
#define CLK_CYC_EFS    70
#define CLK_CYC_FMR    43
#define CLK_CYC_FMB    56
#define CLK_CYC_FMBX   56
#define CLK_CYC_FM     58
#define CLK_CYC_EFMR   99
#define CLK_CYC_EFM   116
#define CLK_CYC_FDR    89
#define CLK_CYC_FDB    96
#define CLK_CYC_FDBX   96
#define CLK_CYC_FD     98
#define CLK_CYC_EFDR  183
#define CLK_CYC_EFD   194
#define CLK_CYC_FCR     6
#define CLK_CYC_FCB    19
#define CLK_CYC_FCBX   19
#define CLK_CYC_FC     21
#define CLK_CYC_EFCR   17
#define CLK_CYC_EFC    37
#define CLK_CYC_FABS(negative)   (negative ? 21 : 9)
#define CLK_CYC_FNEG   18
#define CLK_CYC_FIX    23
#define CLK_CYC_FLT    17
#define CLK_CYC_EFIX   44
#define CLK_CYC_EFLT   26
/* Bit operations */
#define CLK_CYC_SBR     5
#define CLK_CYC_SB     15
#define CLK_CYC_SBI    20
#define CLK_CYC_RBR     5
#define CLK_CYC_RB     15
#define CLK_CYC_RBI    20
#define CLK_CYC_TBR     6
#define CLK_CYC_TB     16
#define CLK_CYC_TBI    21
#define CLK_CYC_TSB    21
#define CLK_CYC_SVBR    5
#define CLK_CYC_RVBR    5
#define CLK_CYC_TVBR    6
/* Shift operations */
#define CLK_CYC_SLL(n_shifts)    (8 + n_shifts)
#define CLK_CYC_SRL(n_shifts)    (8 + n_shifts)
#define CLK_CYC_SRA(n_shifts)    (8 + n_shifts)
#define CLK_CYC_SLC(n_shifts)    (8 + n_shifts)
#define CLK_CYC_DSLL(n_shifts)   (15 + n_shifts)
#define CLK_CYC_DSRL(n_shifts)   (15 + n_shifts)
#define CLK_CYC_DSRA(n_shifts)   (15 + n_shifts)
#define CLK_CYC_DSLC(n_shifts)   (15 + n_shifts)
#define CLK_CYC_SLR(n_shifts)    (n_shifts<0 ? 14-n_shifts : n_shifts>0 ? 17+n_shifts : 12)
#define CLK_CYC_SAR(n_shifts)    (n_shifts<0 ? 14-n_shifts : n_shifts>0 ? 17+n_shifts : 12)
#define CLK_CYC_SCR(n_shifts)    (n_shifts<0 ? 14-n_shifts : n_shifts>0 ? 17+n_shifts : 12)
#define CLK_CYC_DSLR(n_shifts)   (n_shifts<0 ? 20-n_shifts : n_shifts>0 ? 23+n_shifts : 12)
#define CLK_CYC_DSAR(n_shifts)   (n_shifts<0 ? 20-n_shifts : n_shifts>0 ? 23+n_shifts : 12)
#define CLK_CYC_DSCR(n_shifts)   (n_shifts<0 ? 20-n_shifts : n_shifts>0 ? 23+n_shifts : 12)
/* Load/store/exchange */
#define CLK_CYC_LR      5
#define CLK_CYC_LB     13
#define CLK_CYC_LBX    13
#define CLK_CYC_LISP    5
#define CLK_CYC_LISN    5
#define CLK_CYC_L      15
#define CLK_CYC_LIM    10
#define CLK_CYC_LI     20
#define CLK_CYC_DLR     9
#define CLK_CYC_DLB    17
#define CLK_CYC_DLBX   17
#define CLK_CYC_DL     19
#define CLK_CYC_DLI    29
#define CLK_CYC_EFL    32
#define CLK_CYC_LUB    16
#define CLK_CYC_LUBI   21
#define CLK_CYC_LLB    15
#define CLK_CYC_LLBI   20
#define CLK_CYC_STB    13
#define CLK_CYC_STBX   13
#define CLK_CYC_ST     15
#define CLK_CYC_STI    20
#define CLK_CYC_STC    15
#define CLK_CYC_STCI   20
#define CLK_CYC_DSTB   18
#define CLK_CYC_DSTX   18
#define CLK_CYC_DST    20
#define CLK_CYC_DSTI   25
#define CLK_CYC_SRM    24
#define CLK_CYC_EFST   25
#define CLK_CYC_STUB   20
#define CLK_CYC_SUBI   25
#define CLK_CYC_STLB   20
#define CLK_CYC_SLBI   25
#define CLK_CYC_XBR     6
#define CLK_CYC_XWR     6
/* Multiple load/store */
#define CLK_CYC_PSHM(n_pushes)   (10 + 11 * n_pushes)
#define CLK_CYC_POPM(n_pops)   (11 + 14 * n_pops)
#define CLK_CYC_LM(n_loads)     (15 +  5 * n_loads)
#define CLK_CYC_STM(n_stores)    (13 +  5 * n_stores)
#define CLK_CYC_MOV_INIT    (11)
#define CLK_CYC_MOV_STEP(n_moves)   (10 * n_moves)
#define CLK_CYC_MOV_FINI(n_moves)    (n_moves == 0 ? 0 : 18)
/* Program control */
#define CLK_CYC_JC(jump_taken)     (jump_taken ? 18 : 10)
#define CLK_CYC_JCI(jump_taken)    (jump_taken ? 25 : 15)
#define CLK_CYC_JS     15
#define CLK_CYC_SOJ(jump_taken)    (jump_taken ? 19 : 17)
#define CLK_CYC_BR     15
#define CLK_CYC_BRcc(branch_taken)   (branch_taken ? 15 : 5)    /* for all conditional branches */
#define CLK_CYC_BEX   109
#define CLK_CYC_LST    41
#define CLK_CYC_LSTI   47
#define CLK_CYC_SJS    19
#define CLK_CYC_URS    15
#define CLK_CYC_NOP     5
#define CLK_CYC_BPT    20
#define CLK_CYC_BIF    37
#define CLK_CYC_XIO    40	/* not very exact... */
#define CLK_CYC_VIO(data_words)   (30+45*data_words)	/* speculations... */

/****************************** end of PACE *******************************/

#elif defined (F9450)
	/* Source: Fairchild Semiconductor Corp.,
	   "F9450 High-Performance 16-Bit Bipolar Microprocessor",
	   Preliminary Data Sheet - November 1985  */

/* Integer arithmetic and logic: */
#define CLK_CYC_AR      5
#define CLK_CYC_AB     12 
#define CLK_CYC_ABX    12 
#define CLK_CYC_AISP    8
#define CLK_CYC_A      13 
#define CLK_CYC_AIM    12 
#define CLK_CYC_DAR    18 
#define CLK_CYC_DA     24
#define CLK_CYC_SR      5 
#define CLK_CYC_SBB    12 
#define CLK_CYC_SBBX   12
#define CLK_CYC_SISP    8
#define CLK_CYC_S      13
#define CLK_CYC_SIM    12
#define CLK_CYC_DSR    18
#define CLK_CYC_DS     24
#define CLK_CYC_MSR    39
#define CLK_CYC_MISP   42
#define CLK_CYC_MISN   42
#define CLK_CYC_MS     47
#define CLK_CYC_MSIM   46
#define CLK_CYC_MR     37
#define CLK_CYC_MB     44
#define CLK_CYC_MBX    44
#define CLK_CYC_M      45
#define CLK_CYC_MIM    44
#define CLK_CYC_DMR   126
#define CLK_CYC_DM    132
#define CLK_CYC_DVR    98
#define CLK_CYC_DISP   98
#define CLK_CYC_DISN   98
#define CLK_CYC_DV    103
#define CLK_CYC_DVIM  105
#define CLK_CYC_DR     97
#define CLK_CYC_DB    101
#define CLK_CYC_DBX   101
#define CLK_CYC_D     102
#define CLK_CYC_DIM   104
#define CLK_CYC_DDR   239
#define CLK_CYC_DD    245
#define CLK_CYC_INCM   17
#define CLK_CYC_DECM   17
#define CLK_CYC_ABS(negative)    (negative ? 10 : 5)
#define CLK_CYC_DABS(negative)   (negative ? 21 : 13)
#define CLK_CYC_NEG     5
#define CLK_CYC_DNEG   18
#define CLK_CYC_CR      8
#define CLK_CYC_CB     15
#define CLK_CYC_CBX    15
#define CLK_CYC_CISP   11
#define CLK_CYC_CISN   11
#define CLK_CYC_C      16
#define CLK_CYC_CIM    15
#define CLK_CYC_CBL    40
#define CLK_CYC_DCR    21
#define CLK_CYC_DC     27 
#define CLK_CYC_ORR     4
#define CLK_CYC_ORB    11
#define CLK_CYC_ORBX   11
#define CLK_CYC_OR     12
#define CLK_CYC_ORIM   11
#define CLK_CYC_XORR    4
#define CLK_CYC_XOR    12
#define CLK_CYC_XORM   11
#define CLK_CYC_ANDR    4
#define CLK_CYC_ANDB   11
#define CLK_CYC_ANDX   11
#define CLK_CYC_AND    12
#define CLK_CYC_ANDM   11
#define CLK_CYC_NR      7
#define CLK_CYC_N      15
#define CLK_CYC_NIM    14
/* Floating point arithmetic */
#define CLK_CYC_FAR    62
#define CLK_CYC_FAB    67
#define CLK_CYC_FABX   67
#define CLK_CYC_FA     68
#define CLK_CYC_EFAR   71
#define CLK_CYC_EFA    78
#define CLK_CYC_FSR    62
#define CLK_CYC_FSB    67
#define CLK_CYC_FSBX   67
#define CLK_CYC_FS     68
#define CLK_CYC_EFSR   71
#define CLK_CYC_EFS    78
#define CLK_CYC_FMR   120
#define CLK_CYC_FMB   125
#define CLK_CYC_FMBX  125
#define CLK_CYC_FM    126
#define CLK_CYC_EFMR  251
#define CLK_CYC_EFM   258
#define CLK_CYC_FDR   234
#define CLK_CYC_FDB   239
#define CLK_CYC_FDBX  239
#define CLK_CYC_FD    240
#define CLK_CYC_EFDR  480
#define CLK_CYC_EFD   487
#define CLK_CYC_FCR    52
#define CLK_CYC_FCB    57
#define CLK_CYC_FCBX   57
#define CLK_CYC_FC     58
#define CLK_CYC_EFCR   52
#define CLK_CYC_EFC    65
#define CLK_CYC_FABS(negative)   (negative ? 62 : 16)
#define CLK_CYC_FNEG   56
#define CLK_CYC_FIX    10
#define CLK_CYC_FLT    16
#define CLK_CYC_EFIX   21
#define CLK_CYC_EFLT   25
/* Bit operations */
#define CLK_CYC_SBR     7
#define CLK_CYC_SB     16
#define CLK_CYC_SBI    20
#define CLK_CYC_RBR     7
#define CLK_CYC_RB     16
#define CLK_CYC_RBI    20
#define CLK_CYC_TBR     7
#define CLK_CYC_TB     15
#define CLK_CYC_TBI    19
#define CLK_CYC_TSB    23
#define CLK_CYC_SVBR    7
#define CLK_CYC_RVBR    7
#define CLK_CYC_TVBR    7
/* Shift operations */
#define CLK_CYC_SLL(n_shifts)    (7 + 3 * n_shifts)
#define CLK_CYC_SRL(n_shifts)    (7 + 3 * n_shifts)
#define CLK_CYC_SRA(n_shifts)    (7 + 3 * n_shifts)
#define CLK_CYC_SLC(n_shifts)    (7 + 3 * n_shifts)
#define CLK_CYC_DSLL(n_shifts)   (16 + 6 * n_shifts)
#define CLK_CYC_DSRL(n_shifts)   (16 + 6 * n_shifts)
#define CLK_CYC_DSRA(n_shifts)   (16 + 6 * n_shifts)
#define CLK_CYC_DSLC(n_shifts)   (16 + 6 * n_shifts)
#define CLK_CYC_SLR(n_shifts)    (n_shifts<0 ? 21+n_shifts*3 : n_shifts>0 ? 38+n_shifts*5 : 11)
#define CLK_CYC_SAR(n_shifts)    (n_shifts<0 ? 21+n_shifts*3 : n_shifts>0 ? 29+n_shifts*5 : 11)
#define CLK_CYC_SCR(n_shifts)    (n_shifts<0 ? 21+n_shifts*3 : n_shifts>0 ? 24+n_shifts*3 : 11)
#define CLK_CYC_DSLR(n_shifts)   (n_shifts<0 ? 30+n_shifts*6 : n_shifts>0 ? 44+n_shifts*8 : 11)
#define CLK_CYC_DSAR(n_shifts)   (n_shifts<0 ? 30+n_shifts*6 : n_shifts>0 ? 35+n_shifts*8 : 11)
#define CLK_CYC_DSCR(n_shifts)   (n_shifts<0 ? 33+n_shifts*9 : n_shifts>0 ? 33+n_shifts*9 : 11)
/* Load/store/exchange */
#define CLK_CYC_LR      4
#define CLK_CYC_LB     11
#define CLK_CYC_LBX    11
#define CLK_CYC_LISP    7
#define CLK_CYC_LISN    7
#define CLK_CYC_L      12
#define CLK_CYC_LIM    12
#define CLK_CYC_LI     16
#define CLK_CYC_DLR    16
#define CLK_CYC_DLB    21
#define CLK_CYC_DLBX   21
#define CLK_CYC_DL     22
#define CLK_CYC_DLI    26
#define CLK_CYC_EFL    26
#define CLK_CYC_LUB    15
#define CLK_CYC_LUBI   19
#define CLK_CYC_LLB    12
#define CLK_CYC_LLBI   16
#define CLK_CYC_STB    11
#define CLK_CYC_STBX   11
#define CLK_CYC_ST     12
#define CLK_CYC_STI    16
#define CLK_CYC_STC    12
#define CLK_CYC_STCI   16
#define CLK_CYC_DSTB   15
#define CLK_CYC_DSTX   15
#define CLK_CYC_DST    16
#define CLK_CYC_DSTI   20
#define CLK_CYC_SRM    25
#define CLK_CYC_EFST   20
#define CLK_CYC_STUB   16
#define CLK_CYC_SUBI   20
#define CLK_CYC_STLB   16
#define CLK_CYC_SLBI   20
#define CLK_CYC_XBR     7
#define CLK_CYC_XWR    10
/* Multiple load/store */
#define CLK_CYC_PSHM(n_pushes)   (16 + 12 * n_pushes)
#define CLK_CYC_POPM(n_pops)   (20 + 16 * n_pops)
#define CLK_CYC_LM(n_loads)     (16 +  8 * n_loads)
#define CLK_CYC_STM(n_stores)    (17 +  9 * n_stores)
#define CLK_CYC_MOV_INIT    (9)
#define CLK_CYC_MOV_STEP(n_moves)   (13 * n_moves)
#define CLK_CYC_MOV_FINI(n_moves)    (n_moves == 0 ? 0 : 28)
/* Program control */
#define CLK_CYC_JC(jump_taken)     (jump_taken ? 17 :  9)
#define CLK_CYC_JCI(jump_taken)    (jump_taken ? 21 : 13)
#define CLK_CYC_JS     12
#define CLK_CYC_SOJ(jump_taken)    (jump_taken ? 17 : 13)
#define CLK_CYC_BR     14
#define CLK_CYC_BRcc(branch_taken)	  (branch_taken ? 15 : 4)  /* for all conditional branches */
#define CLK_CYC_BEX    87
#define CLK_CYC_LST    42
#define CLK_CYC_LSTI   46
#define CLK_CYC_SJS    22
#define CLK_CYC_URS    15
#define CLK_CYC_NOP     9
#define CLK_CYC_NOP_BPT    27
#define CLK_CYC_BIF    34
#define CLK_CYC_XIO    60  /* very roughly */
#define CLK_CYC_VIO(data_words)   (70+20*data_words) /* very roughly */


#elif defined (GVSC)
	/* 1. Source: Space Systems Loral,
	      "Software Programmer's Manual for the ASPS Computer",
	      Cage Code 52088, Document No. MO1.01, Rev. T-,
	      January 18, 1994
	   2. Notes:
	      a) For approx. half the instructions, SSL give a
	         fractional figure of estimated cycles. This is due to
	         the pipelining used in the GVSC. In these cases, cycles
	         have been *rounded up*, therefore figures given here are
	         slightly pessimistic.
	      b) The estimates are based on a 3 cycle memory read and a
	         3 cycle memory write. */

/* Integer arithmetic and logic: */
#define CLK_CYC_AR      3
#define CLK_CYC_AB      6
#define CLK_CYC_ABX     6
#define CLK_CYC_AISP    4
#define CLK_CYC_A       7
#define CLK_CYC_AIM     5
#define CLK_CYC_DAR     4
#define CLK_CYC_DA      8
#define CLK_CYC_SR      4
#define CLK_CYC_SBB     7
#define CLK_CYC_SBBX    7
#define CLK_CYC_SISP    5
#define CLK_CYC_S       8
#define CLK_CYC_SIM     8
#define CLK_CYC_DSR     5
#define CLK_CYC_DS      9
#define CLK_CYC_MSR     8
#define CLK_CYC_MISP    8
#define CLK_CYC_MISN    8
#define CLK_CYC_MS     12
#define CLK_CYC_MSIM    9
#define CLK_CYC_MR      8
#define CLK_CYC_MB     11
#define CLK_CYC_MBX    11
#define CLK_CYC_M      12
#define CLK_CYC_MIM     9
#define CLK_CYC_DMR    10
#define CLK_CYC_DM     14
#define CLK_CYC_DVR    39
#define CLK_CYC_DISP   39
#define CLK_CYC_DISN   42
#define CLK_CYC_DV     43
#define CLK_CYC_DVIM   40
#define CLK_CYC_DR     39
#define CLK_CYC_DB     42
#define CLK_CYC_DBX    43
#define CLK_CYC_D      43
#define CLK_CYC_DIM    40
#define CLK_CYC_DDR    54
#define CLK_CYC_DD     58
#define CLK_CYC_INCM   14
#define CLK_CYC_DECM   14
#define CLK_CYC_ABS(negative)     8
#define CLK_CYC_DABS(negative)    6
#define CLK_CYC_NEG     8
#define CLK_CYC_DNEG    9
#define CLK_CYC_CR      4
#define CLK_CYC_CB     10
#define CLK_CYC_CBX     7
#define CLK_CYC_CISP    5
#define CLK_CYC_CISN    4
#define CLK_CYC_C       8
#define CLK_CYC_CIM     6
#define CLK_CYC_CBL    10
#define CLK_CYC_DCR     5
#define CLK_CYC_DC      9
#define CLK_CYC_ORR     3
#define CLK_CYC_ORB     6
#define CLK_CYC_ORBX    7
#define CLK_CYC_OR      7
#define CLK_CYC_ORIM    5
#define CLK_CYC_XORR    3
#define CLK_CYC_XOR     7
#define CLK_CYC_XORM    4
#define CLK_CYC_ANDR    3
#define CLK_CYC_ANDB    6
#define CLK_CYC_ANDX    7
#define CLK_CYC_AND     7
#define CLK_CYC_ANDM    4
#define CLK_CYC_NR      3
#define CLK_CYC_N       7
#define CLK_CYC_NIM     5
/* Floating point arithmetic */
#define CLK_CYC_FAR     6
#define CLK_CYC_FAB     9
#define CLK_CYC_FABX    8
#define CLK_CYC_FA     10
#define CLK_CYC_EFAR    7
#define CLK_CYC_EFA    14
#define CLK_CYC_FSR     6
#define CLK_CYC_FSB     9
#define CLK_CYC_FSBX   10
#define CLK_CYC_FS     10
#define CLK_CYC_EFSR    7
#define CLK_CYC_EFS    14
#define CLK_CYC_FMR     9
#define CLK_CYC_FMB    12
#define CLK_CYC_FMBX   12
#define CLK_CYC_FM     13
#define CLK_CYC_EFMR   12
#define CLK_CYC_EFM    19
#define CLK_CYC_FDR    44
#define CLK_CYC_FDB    47
#define CLK_CYC_FDBX   48
#define CLK_CYC_FD     48
#define CLK_CYC_EFDR   60
#define CLK_CYC_EFD    67
#define CLK_CYC_FCR     6
#define CLK_CYC_FCB     9
#define CLK_CYC_FCBX   10
#define CLK_CYC_FC     10
#define CLK_CYC_EFCR   11
#define CLK_CYC_EFC    18
#define CLK_CYC_FABS(negative)    5
#define CLK_CYC_FNEG    8
#define CLK_CYC_FIX     5
#define CLK_CYC_FLT     5
#define CLK_CYC_EFIX    6
#define CLK_CYC_EFLT    5
/* Bit operations */
#define CLK_CYC_SBR     3
#define CLK_CYC_SB      7
#define CLK_CYC_SBI    12
#define CLK_CYC_RBR     3
#define CLK_CYC_RB     10
#define CLK_CYC_RBI    15
#define CLK_CYC_TBR     3
#define CLK_CYC_TB      7
#define CLK_CYC_TBI    12
#define CLK_CYC_TSB    14
#define CLK_CYC_SVBR    3
#define CLK_CYC_RVBR    3
#define CLK_CYC_TVBR    3
/* Shift operations */
#define CLK_CYC_SLL(n_shifts)     3
#define CLK_CYC_SRL(n_shifts)     3
#define CLK_CYC_SRA(n_shifts)     3
#define CLK_CYC_SLC(n_shifts)     3
#define CLK_CYC_DSLL(n_shifts)    3
#define CLK_CYC_DSRL(n_shifts)    3
#define CLK_CYC_DSRA(n_shifts)    3
#define CLK_CYC_DSLC(n_shifts)    3
#define CLK_CYC_SLR(n_shifts)     7
#define CLK_CYC_SAR(n_shifts)     6
#define CLK_CYC_SCR(n_shifts)     7
#define CLK_CYC_DSLR(n_shifts)    7
#define CLK_CYC_DSAR(n_shifts)    7
#define CLK_CYC_DSCR(n_shifts)    7
/* Load/store/exchange */
#define CLK_CYC_LR      3
#define CLK_CYC_LB      6
#define CLK_CYC_LBX     7
#define CLK_CYC_LISP    3
#define CLK_CYC_LISN    3
#define CLK_CYC_L       7
#define CLK_CYC_LIM     4
#define CLK_CYC_LI     12
#define CLK_CYC_DLR     4
#define CLK_CYC_DLB     7
#define CLK_CYC_DLBX    8
#define CLK_CYC_DL      8
#define CLK_CYC_DLI    14
#define CLK_CYC_EFL    12
#define CLK_CYC_LUB     7
#define CLK_CYC_LUBI   12
#define CLK_CYC_LLB     7
#define CLK_CYC_LLBI   12
#define CLK_CYC_STB     7
#define CLK_CYC_STBX    8
#define CLK_CYC_ST      8
#define CLK_CYC_STI    10
#define CLK_CYC_STC     8
#define CLK_CYC_STCI   13
#define CLK_CYC_DSTB    7
#define CLK_CYC_DSTX    8
#define CLK_CYC_DST     8
#define CLK_CYC_DSTI   13
#define CLK_CYC_SRM    10
#define CLK_CYC_EFST   12
#define CLK_CYC_STUB   14
#define CLK_CYC_SUBI   17
#define CLK_CYC_STLB    9
#define CLK_CYC_SLBI   10
#define CLK_CYC_XBR     3
#define CLK_CYC_XWR     3
/* Multiple load/store */
#define CLK_CYC_PSHM(n_pushes)   57
#define CLK_CYC_POPM(n_pops)   32
#define CLK_CYC_LM(n_loads)     32
#define CLK_CYC_STM(n_stores)    55
#define CLK_CYC_MOV_INIT 0
#define CLK_CYC_MOV_STEP(n_moves) (42 * n_moves)
#define CLK_CYC_MOV_FINI(n_moves) 0    42
/* Program control */
#define CLK_CYC_JC(jump_taken)      7
#define CLK_CYC_JCI(jump_taken)    12
#define CLK_CYC_JS      9
#define CLK_CYC_SOJ(jump_taken)     8
#define CLK_CYC_BR      6
#define CLK_CYC_BRcc(branch_taken)    6		/* for all conditional branches */
#define CLK_CYC_BEX    13
#define CLK_CYC_LST    11
#define CLK_CYC_LSTI   16
#define CLK_CYC_SJS     9
#define CLK_CYC_URS     8
#define CLK_CYC_NOP     3
#define CLK_CYC_BPT     6
#define CLK_CYC_BIF     2
#define CLK_CYC_XIO    15		/* very roughly */
#define CLK_CYC_VIO(data_words)    68		/* very roughly */

/****************************** end of GVSC *******************************/

#elif defined (MA31750)

/* MA31750 - 0 waitstates according to IMA31750 Data sheet (July 1994)  */
/* Average and weighted average timings are used (rounded upwards)      */
/* Added by J.Gaisler ESA/ESTEC/WS					*/

#define WSTATES 0		/* Number of waitstates */
#define MEM_CYCLE (2 + WSTATES)	/* Clocks per memory cycle */
#define INT_CYCLE 2		/* Clocks per internal cycle */

/* Integer arithmetic  and logic: */
#define CLK_CYC_AR      (1*MEM_CYCLE)
#define CLK_CYC_AB      (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_ABX     (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_AISP    (1*MEM_CYCLE)
#define CLK_CYC_A       (3*MEM_CYCLE)
#define CLK_CYC_AIM     (2*MEM_CYCLE)
#define CLK_CYC_DAR     (1*MEM_CYCLE)
#define CLK_CYC_DA      (4*MEM_CYCLE)
#define CLK_CYC_SR      (1*MEM_CYCLE)
#define CLK_CYC_SBB     (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SBBX    (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SISP    (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_S       (3*MEM_CYCLE)
#define CLK_CYC_SIM     (2*MEM_CYCLE)
#define CLK_CYC_DSR     (1*MEM_CYCLE)
#define CLK_CYC_DS      (4*MEM_CYCLE)
#define CLK_CYC_MSR     (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_MISP    (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_MISN    (1*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_MS      (3*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_MSIM    (2*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_MR      (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_MB      (2*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_MBX     (2*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_M       (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_MIM     (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_DMR     (1*MEM_CYCLE + 18*INT_CYCLE)
#define CLK_CYC_DM      (4*MEM_CYCLE + 18*INT_CYCLE)
#define CLK_CYC_DVR     (1*MEM_CYCLE + 24*INT_CYCLE)
#define CLK_CYC_DISP    (1*MEM_CYCLE + 24*INT_CYCLE)
#define CLK_CYC_DISN    (1*MEM_CYCLE + 24*INT_CYCLE)
#define CLK_CYC_DV      (3*MEM_CYCLE + 24*INT_CYCLE)
#define CLK_CYC_DVIM    (2*MEM_CYCLE + 24*INT_CYCLE)
#define CLK_CYC_DR      (1*MEM_CYCLE + 28*INT_CYCLE)
#define CLK_CYC_DB      (2*MEM_CYCLE + 29*INT_CYCLE)
#define CLK_CYC_DBX     (2*MEM_CYCLE + 29*INT_CYCLE)
#define CLK_CYC_D       (3*MEM_CYCLE + 28*INT_CYCLE)
#define CLK_CYC_DIM     (2*MEM_CYCLE + 28*INT_CYCLE)
#define CLK_CYC_DDR     (1*MEM_CYCLE + 41*INT_CYCLE)
#define CLK_CYC_DD      (4*MEM_CYCLE + 41*INT_CYCLE)
#define CLK_CYC_INCM    (4*MEM_CYCLE)
#define CLK_CYC_DECM    (4*MEM_CYCLE)
#define CLK_CYC_ABS(negative)     (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_DABS(negative)    (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_NEG     (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_DNEG    (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_CR      (1*MEM_CYCLE)
#define CLK_CYC_CB      (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_CBX     (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_CISP    (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_CISN    (1*MEM_CYCLE)
#define CLK_CYC_C       (3*MEM_CYCLE)
#define CLK_CYC_CIM     (2*MEM_CYCLE)
#define CLK_CYC_CBL     (4*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_DCR     (1*MEM_CYCLE)
#define CLK_CYC_DC      (4*MEM_CYCLE)
#define CLK_CYC_ORR     (1*MEM_CYCLE)
#define CLK_CYC_ORB     (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_ORBX    (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_OR      (3*MEM_CYCLE)
#define CLK_CYC_ORIM    (2*MEM_CYCLE)
#define CLK_CYC_XORR    (1*MEM_CYCLE)
#define CLK_CYC_XOR     (3*MEM_CYCLE)
#define CLK_CYC_XORM    (2*MEM_CYCLE)
#define CLK_CYC_ANDR    (1*MEM_CYCLE)
#define CLK_CYC_ANDB    (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_ANDX    (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_AND     (3*MEM_CYCLE)
#define CLK_CYC_ANDM    (2*MEM_CYCLE)
#define CLK_CYC_NR      (1*MEM_CYCLE)
#define CLK_CYC_N       (3*MEM_CYCLE)
#define CLK_CYC_NIM     (2*MEM_CYCLE)
/* Floating point arithmetic  */
#define CLK_CYC_FAR    (1*MEM_CYCLE + 7*INT_CYCLE)
#define CLK_CYC_FAB    (3*MEM_CYCLE + 9*INT_CYCLE)
#define CLK_CYC_FABX   (3*MEM_CYCLE + 9*INT_CYCLE)
#define CLK_CYC_FA     (4*MEM_CYCLE + 9*INT_CYCLE)
#define CLK_CYC_EFAR   (1*MEM_CYCLE + 21*INT_CYCLE)
#define CLK_CYC_EFA    (5*MEM_CYCLE + 20*INT_CYCLE)
#define CLK_CYC_FSR    (1*MEM_CYCLE + 9*INT_CYCLE)
#define CLK_CYC_FSB    (3*MEM_CYCLE + 10*INT_CYCLE)
#define CLK_CYC_FSBX   (3*MEM_CYCLE + 10*INT_CYCLE)
#define CLK_CYC_FS     (4*MEM_CYCLE + 9*INT_CYCLE)
#define CLK_CYC_EFSR   (1*MEM_CYCLE + 23*INT_CYCLE)
#define CLK_CYC_EFS    (5*MEM_CYCLE + 22*INT_CYCLE)
#define CLK_CYC_FMR    (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_FMB    (3*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_FMBX   (3*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_FM     (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_EFMR   (1*MEM_CYCLE + 34*INT_CYCLE)
#define CLK_CYC_EFM    (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_FDR    (1*MEM_CYCLE + 43*INT_CYCLE)
#define CLK_CYC_FDB    (3*MEM_CYCLE + 44*INT_CYCLE)
#define CLK_CYC_FDBX   (3*MEM_CYCLE + 44*INT_CYCLE)
#define CLK_CYC_FD     (4*MEM_CYCLE + 43*INT_CYCLE)
#define CLK_CYC_EFDR   (1*MEM_CYCLE + 113*INT_CYCLE)
#define CLK_CYC_EFD    (5*MEM_CYCLE + 113*INT_CYCLE)
#define CLK_CYC_FCR    (1*MEM_CYCLE + 5*INT_CYCLE)
#define CLK_CYC_FCB    (3*MEM_CYCLE + 5*INT_CYCLE)
#define CLK_CYC_FCBX   (3*MEM_CYCLE + 5*INT_CYCLE)
#define CLK_CYC_FC     (4*MEM_CYCLE + 5*INT_CYCLE)
#define CLK_CYC_EFCR   (1*MEM_CYCLE + 5*INT_CYCLE)
#define CLK_CYC_EFC    (5*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_FABS(negative)   (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_FNEG   (1*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_FIX    (1*MEM_CYCLE + 8*INT_CYCLE)
#define CLK_CYC_FLT    (1*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_EFIX   (1*MEM_CYCLE + 9*INT_CYCLE)
#define CLK_CYC_EFLT   (1*MEM_CYCLE + 9*INT_CYCLE)
/* Bit operations */
#define CLK_CYC_SBR    (1*MEM_CYCLE)
#define CLK_CYC_SB     (4*MEM_CYCLE)
#define CLK_CYC_SBI    (5*MEM_CYCLE)
#define CLK_CYC_RBR    (1*MEM_CYCLE)
#define CLK_CYC_RB     (4*MEM_CYCLE)
#define CLK_CYC_RBI    (5*MEM_CYCLE)
#define CLK_CYC_TBR    (1*MEM_CYCLE)
#define CLK_CYC_TB     (3*MEM_CYCLE)
#define CLK_CYC_TBI    (4*MEM_CYCLE)
#define CLK_CYC_TSB    (2*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_SVBR   (1*MEM_CYCLE)
#define CLK_CYC_RVBR   (1*MEM_CYCLE)
#define CLK_CYC_TVBR   (1*MEM_CYCLE)
/* Shift operations */
#define CLK_CYC_SLL(n_shifts)    (1*MEM_CYCLE)
#define CLK_CYC_SRL(n_shifts)    (1*MEM_CYCLE)
#define CLK_CYC_SRA(n_shifts)    (1*MEM_CYCLE)
#define CLK_CYC_SLC(n_shifts)    (1*MEM_CYCLE)
#define CLK_CYC_DSLL(n_shifts)   (1*MEM_CYCLE)
#define CLK_CYC_DSRL(n_shifts)   (1*MEM_CYCLE)
#define CLK_CYC_DSRA(n_shifts)   (1*MEM_CYCLE)
#define CLK_CYC_DSLC(n_shifts)   (1*MEM_CYCLE)
#define CLK_CYC_SLR(n_shifts)    (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_SAR(n_shifts)    (1*MEM_CYCLE + 5*INT_CYCLE)
#define CLK_CYC_SCR(n_shifts)    (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_DSLR(n_shifts)   (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_DSAR(n_shifts)   (1*MEM_CYCLE + 5*INT_CYCLE)
#define CLK_CYC_DSCR(n_shifts)   (1*MEM_CYCLE + 2*INT_CYCLE)
/* Load/store/exchange */
#define CLK_CYC_LR     (1*MEM_CYCLE)
#define CLK_CYC_LB     (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_LBX    (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_LISP   (1*MEM_CYCLE)
#define CLK_CYC_LISN   (1*MEM_CYCLE)
#define CLK_CYC_L      (3*MEM_CYCLE)
#define CLK_CYC_LIM    (2*MEM_CYCLE)
#define CLK_CYC_LI     (4*MEM_CYCLE)
#define CLK_CYC_DLR    (1*MEM_CYCLE)
#define CLK_CYC_DLB    (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_DLBX   (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_DL     (4*MEM_CYCLE)
#define CLK_CYC_DLI    (5*MEM_CYCLE)
#define CLK_CYC_EFL    (5*MEM_CYCLE)
#define CLK_CYC_LUB    (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_LUBI   (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_LLB    (3*MEM_CYCLE)
#define CLK_CYC_LLBI   (4*MEM_CYCLE)
#define CLK_CYC_STB    (2*MEM_CYCLE)
#define CLK_CYC_STBX   (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_ST     (3*MEM_CYCLE)
#define CLK_CYC_STI    (4*MEM_CYCLE)
#define CLK_CYC_STC    (3*MEM_CYCLE)
#define CLK_CYC_STCI   (4*MEM_CYCLE)
#define CLK_CYC_DSTB   (3*MEM_CYCLE)
#define CLK_CYC_DSTX   (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_DST    (4*MEM_CYCLE)
#define CLK_CYC_DSTI   (5*MEM_CYCLE)
#define CLK_CYC_SRM    (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_EFST   25
#define CLK_CYC_STUB   (4*MEM_CYCLE)
#define CLK_CYC_SUBI   (5*MEM_CYCLE)
#define CLK_CYC_STLB   (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SLBI   (5*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_XBR    (1*MEM_CYCLE)
#define CLK_CYC_XWR    (1*MEM_CYCLE + 2*INT_CYCLE)
/* Multiple load/store */
#define CLK_CYC_PSHM(n_pushes)   (n_pushes*MEM_CYCLE + 8*INT_CYCLE)
#define CLK_CYC_POPM(n_pops)   (n_pops*MEM_CYCLE + 4*INT_CYCLE)
#define CLK_CYC_LM(n_loads)     ((3+n_loads)*MEM_CYCLE)
#define CLK_CYC_STM(n_stores)    ((2+n_stores)*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_MOV_INIT 0
#define CLK_CYC_MOV_STEP(n_moves) (((1+2*n_moves)*MEM_CYCLE + 7*INT_CYCLE))
#define CLK_CYC_MOV_FINI(n_moves) 0    ((1+2*n_moves)*MEM_CYCLE + 7*INT_CYCLE)
/* Program control */
#define CLK_CYC_JC(jump_taken)     (3*MEM_CYCLE)
#define CLK_CYC_JCI(jump_taken)    (4*MEM_CYCLE)
#define CLK_CYC_JS     (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SOJ(jump_taken)    (3*MEM_CYCLE)
#define CLK_CYC_BR     (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_BRcc(branch_taken)   (2*MEM_CYCLE)   /* Not sure on this figure ... */
#define CLK_CYC_BEX    (11*MEM_CYCLE + 14*INT_CYCLE)
#define CLK_CYC_LST    (6*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_LSTI   (7*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SJS    (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_URS    (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_NOP    (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_BPT    (1*MEM_CYCLE + 6*INT_CYCLE)
#define CLK_CYC_BIF    (3*MEM_CYCLE + 5*INT_CYCLE)   /* Same as XIO ...?  */
#define CLK_CYC_XIO    (3*MEM_CYCLE + 5*INT_CYCLE)
#define CLK_CYC_VIO(data_words)   300	/* ...not even trying this one! */

/***************************** end of MA31750 *****************************/

#elif defined (MAS281)

/*
 * GEC-Plessey MAS281 timing from the GEC-Plessey data sheet, DS3563-3.3
 * dated July 1995. Added by Chris Nettleton, November 1996.
 */

#define MEM_CYCLE (8)	/* Clocks per memory cycle */
#define INT_CYCLE (11)	/* Clocks per internal cycle */

/* Integer arithmetic  and logic: */
#define CLK_CYC_AR      (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_AB      (2*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_ABX     (2*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_AISP    (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_A       (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_AIM     (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_DAR     (1*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_DA      (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SR      (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SBB     (2*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_SBBX    (2*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_SISP    (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_S       (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SIM     (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_DSR     (1*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_DS      (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_MSR     (1*MEM_CYCLE + 7*INT_CYCLE)
#define CLK_CYC_MISP    (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_MISN    (1*MEM_CYCLE + 8*INT_CYCLE)
#define CLK_CYC_MS      (3*MEM_CYCLE + 7*INT_CYCLE)
#define CLK_CYC_MSIM    (2*MEM_CYCLE + 7*INT_CYCLE)
#define CLK_CYC_MR      (1*MEM_CYCLE + 5*INT_CYCLE)
#define CLK_CYC_MB      (2*MEM_CYCLE + 7*INT_CYCLE)
#define CLK_CYC_MBX     (2*MEM_CYCLE + 7*INT_CYCLE)
#define CLK_CYC_M       (3*MEM_CYCLE + 5*INT_CYCLE)
#define CLK_CYC_MIM     (2*MEM_CYCLE + 5*INT_CYCLE)
#define CLK_CYC_DMR     (1*MEM_CYCLE + 41*INT_CYCLE)
#define CLK_CYC_DM      (4*MEM_CYCLE + 40*INT_CYCLE)
#define CLK_CYC_DVR     (1*MEM_CYCLE + 21*INT_CYCLE)
#define CLK_CYC_DISP    (1*MEM_CYCLE + 20*INT_CYCLE)
#define CLK_CYC_DISN    (1*MEM_CYCLE + 21*INT_CYCLE)
#define CLK_CYC_DV      (3*MEM_CYCLE + 21*INT_CYCLE)
#define CLK_CYC_DVIM    (2*MEM_CYCLE + 21*INT_CYCLE)
#define CLK_CYC_DR      (1*MEM_CYCLE + 22*INT_CYCLE)
#define CLK_CYC_DB      (2*MEM_CYCLE + 23*INT_CYCLE)
#define CLK_CYC_DBX     (2*MEM_CYCLE + 23*INT_CYCLE)
#define CLK_CYC_D       (3*MEM_CYCLE + 22*INT_CYCLE)
#define CLK_CYC_DIM     (2*MEM_CYCLE + 23*INT_CYCLE)
#define CLK_CYC_DDR     (1*MEM_CYCLE + 80*INT_CYCLE)
#define CLK_CYC_DD      (4*MEM_CYCLE + 78*INT_CYCLE)
#define CLK_CYC_INCM    (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_DECM    (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_ABS(negative)     (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_DABS(negative)    (1*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_NEG     (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_DNEG    (1*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_CR      (1*MEM_CYCLE)
#define CLK_CYC_CB      (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_CBX     (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_CISP    (1*MEM_CYCLE)
#define CLK_CYC_CISN    (1*MEM_CYCLE)
#define CLK_CYC_C       (3*MEM_CYCLE)
#define CLK_CYC_CIM     (2*MEM_CYCLE)
#define CLK_CYC_CBL     (4*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_DCR     (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_DC      (4*MEM_CYCLE)
#define CLK_CYC_ORR     (1*MEM_CYCLE)
#define CLK_CYC_ORB     (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_ORBX    (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_OR      (3*MEM_CYCLE)
#define CLK_CYC_ORIM    (2*MEM_CYCLE)
#define CLK_CYC_XORR    (1*MEM_CYCLE)
#define CLK_CYC_XOR     (3*MEM_CYCLE)
#define CLK_CYC_XORM    (2*MEM_CYCLE)
#define CLK_CYC_ANDR    (1*MEM_CYCLE)
#define CLK_CYC_ANDB    (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_ANDX    (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_AND     (3*MEM_CYCLE)
#define CLK_CYC_ANDM    (2*MEM_CYCLE)
#define CLK_CYC_NR      (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_N       (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_NIM     (2*MEM_CYCLE + 1*INT_CYCLE)
/* Floating point arithmetic  */
#define CLK_CYC_FAR    (1*MEM_CYCLE + 8*INT_CYCLE)
#define CLK_CYC_FAB    (3*MEM_CYCLE + 8*INT_CYCLE)
#define CLK_CYC_FABX   (3*MEM_CYCLE + 8*INT_CYCLE)
#define CLK_CYC_FA     (4*MEM_CYCLE + 8*INT_CYCLE)
#define CLK_CYC_EFAR   (1*MEM_CYCLE + 22*INT_CYCLE)
#define CLK_CYC_EFA    (5*MEM_CYCLE + 20*INT_CYCLE)
#define CLK_CYC_FSR    (1*MEM_CYCLE + 8*INT_CYCLE)
#define CLK_CYC_FSB    (3*MEM_CYCLE + 8*INT_CYCLE)
#define CLK_CYC_FSBX   (3*MEM_CYCLE + 8*INT_CYCLE)
#define CLK_CYC_FS     (4*MEM_CYCLE + 8*INT_CYCLE)
#define CLK_CYC_EFSR   (1*MEM_CYCLE + 23*INT_CYCLE)
#define CLK_CYC_EFS    (5*MEM_CYCLE + 21*INT_CYCLE)
#define CLK_CYC_FMR    (1*MEM_CYCLE + 13*INT_CYCLE)
#define CLK_CYC_FMB    (3*MEM_CYCLE + 13*INT_CYCLE)
#define CLK_CYC_FMBX   (3*MEM_CYCLE + 13*INT_CYCLE)
#define CLK_CYC_FM     (4*MEM_CYCLE + 12*INT_CYCLE)
#define CLK_CYC_EFMR   (1*MEM_CYCLE + 60*INT_CYCLE)
#define CLK_CYC_EFM    (5*MEM_CYCLE + 58*INT_CYCLE)
#define CLK_CYC_FDR    (1*MEM_CYCLE + 33*INT_CYCLE)
#define CLK_CYC_FDB    (3*MEM_CYCLE + 33*INT_CYCLE)
#define CLK_CYC_FDBX   (3*MEM_CYCLE + 33*INT_CYCLE)
#define CLK_CYC_FD     (4*MEM_CYCLE + 33*INT_CYCLE)
#define CLK_CYC_EFDR   (1*MEM_CYCLE + 103*INT_CYCLE)
#define CLK_CYC_EFD    (5*MEM_CYCLE + 101*INT_CYCLE)
#define CLK_CYC_FCR    (1*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_FCB    (2*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_FCBX   (2*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_FC     (3*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_EFCR   (1*MEM_CYCLE + 4*INT_CYCLE)
#define CLK_CYC_EFC    (5*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_FABS(negative)   (1*MEM_CYCLE + 4*INT_CYCLE)
#define CLK_CYC_FNEG   (1*MEM_CYCLE + 4*INT_CYCLE)
#define CLK_CYC_FIX    (1*MEM_CYCLE + 5*INT_CYCLE)
#define CLK_CYC_FLT    (1*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_EFIX   (1*MEM_CYCLE + 13*INT_CYCLE)
#define CLK_CYC_EFLT   (1*MEM_CYCLE + 8*INT_CYCLE)
/* Bit operations */
#define CLK_CYC_SBR    (1*MEM_CYCLE)
#define CLK_CYC_SB     (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SBI    (5*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_RBR    (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_RB     (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_RBI    (5*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_TBR    (1*MEM_CYCLE)
#define CLK_CYC_TB     (3*MEM_CYCLE)
#define CLK_CYC_TBI    (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_TSB    (4*MEM_CYCLE)
#define CLK_CYC_SVBR   (1*MEM_CYCLE)
#define CLK_CYC_RVBR   (1*MEM_CYCLE)
#define CLK_CYC_TVBR   (1*MEM_CYCLE)
/* Shift operations */
#define CLK_CYC_SLL(n_shifts)    (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SRL(n_shifts)    (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SRA(n_shifts)    (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SLC(n_shifts)    (1*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_DSLL(n_shifts)   (1*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_DSRL(n_shifts)   (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_DSRA(n_shifts)   (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_DSLC(n_shifts)   (1*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_SLR(n_shifts)    (1*MEM_CYCLE + 4*INT_CYCLE)
#define CLK_CYC_SAR(n_shifts)    (1*MEM_CYCLE + 4*INT_CYCLE)
#define CLK_CYC_SCR(n_shifts)    (1*MEM_CYCLE + 4*INT_CYCLE)
#define CLK_CYC_DSLR(n_shifts)   (1*MEM_CYCLE + 4*INT_CYCLE)
#define CLK_CYC_DSAR(n_shifts)   (1*MEM_CYCLE + 5*INT_CYCLE)
#define CLK_CYC_DSCR(n_shifts)   (1*MEM_CYCLE + 3*INT_CYCLE)
/* Load/store/exchange */
#define CLK_CYC_LR     (1*MEM_CYCLE)
#define CLK_CYC_LB     (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_LBX    (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_LISP   (1*MEM_CYCLE)
#define CLK_CYC_LISN   (1*MEM_CYCLE)
#define CLK_CYC_L      (3*MEM_CYCLE)
#define CLK_CYC_LIM    (2*MEM_CYCLE)
#define CLK_CYC_LI     (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_DLR    (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_DLB    (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_DLBX   (3*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_DL     (4*MEM_CYCLE)
#define CLK_CYC_DLI    (5*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_EFL    (5*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_LUB    (3*MEM_CYCLE)
#define CLK_CYC_LUBI   (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_LLB    (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_LLBI   (4*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_STB    (2*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_STBX   (2*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_ST     (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_STI    (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_STC    (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_STCI   (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_DSTB   (3*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_DSTX   (3*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_DST    (4*MEM_CYCLE)
#define CLK_CYC_DSTI   (5*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SRM    (4*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_EFST   (5*MEM_CYCLE)
#define CLK_CYC_STUB   (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SUBI   (5*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_STLB   (4*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_SLBI   (5*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_XBR    (1*MEM_CYCLE)
#define CLK_CYC_XWR    (1*MEM_CYCLE + 2*INT_CYCLE)
/* Multiple load/store */
#define CLK_CYC_PSHM(n_pushes)   (n_pushes*MEM_CYCLE + n_pushes*5*INT_CYCLE)
#define CLK_CYC_POPM(n_pops)   (n_pops*MEM_CYCLE + n_pops*3*INT_CYCLE)
#define CLK_CYC_LM(n_loads)     ((3+n_loads)*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_STM(n_stores)    ((3+n_stores)*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_MOV_INIT 0
#define CLK_CYC_MOV_STEP(n_moves) (((1+4*n_moves)*MEM_CYCLE + (1+7*n_moves)*INT_CYCLE))
#define CLK_CYC_MOV_FINI(n_moves) 0    ((1+4*n_moves)*MEM_CYCLE + (1+7*n_moves)*INT_CYCLE)
/* Program control */
#define CLK_CYC_JC(jump_taken)     (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_JCI(jump_taken)    (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_JS     (2*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_SOJ(jump_taken)    (2*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_BR     (2*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_BRcc(branch_taken)   (2*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_BEX    (16*MEM_CYCLE + 12*INT_CYCLE)
#define CLK_CYC_LST    (8*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_LSTI   (9*MEM_CYCLE + 4*INT_CYCLE)
#define CLK_CYC_SJS    (4*MEM_CYCLE + 3*INT_CYCLE)
#define CLK_CYC_URS    (3*MEM_CYCLE + 1*INT_CYCLE)
#define CLK_CYC_NOP    (1*MEM_CYCLE + 2*INT_CYCLE)
#define CLK_CYC_BPT    (3*MEM_CYCLE + 4*INT_CYCLE)
#define CLK_CYC_BIF    (3*MEM_CYCLE + 5*INT_CYCLE)   /* Same as XIO ...?  */
#define CLK_CYC_XIO    (3*MEM_CYCLE + 5*INT_CYCLE)
#define CLK_CYC_VIO(data_words)   300	/* ...not even trying this one! */

/**************************** end of MAS281 *******************************/


		/*************** DEFINE FURTHER BRANDS HERE ***************/


#else /* default to F9450*/
/* similar to F*/
/* Integer arithmetic and logic: */
#define CLK_CYC_AR      5
#define CLK_CYC_AB     12 
#define CLK_CYC_ABX    12 
#define CLK_CYC_AISP    8
#define CLK_CYC_A      13 
#define CLK_CYC_AIM    12 
#define CLK_CYC_DAR    18 
#define CLK_CYC_DA     24
#define CLK_CYC_SR      5 
#define CLK_CYC_SBB    12 
#define CLK_CYC_SBBX   12
#define CLK_CYC_SISP    8
#define CLK_CYC_S      13
#define CLK_CYC_SIM    12
#define CLK_CYC_DSR    18
#define CLK_CYC_DS     24
#define CLK_CYC_MSR    39
#define CLK_CYC_MISP   42
#define CLK_CYC_MISN   42
#define CLK_CYC_MS     47
#define CLK_CYC_MSIM   46
#define CLK_CYC_MR     37
#define CLK_CYC_MB     44
#define CLK_CYC_MBX    44
#define CLK_CYC_M      45
#define CLK_CYC_MIM    44
#define CLK_CYC_DMR   126
#define CLK_CYC_DM    132
#define CLK_CYC_DVR    98
#define CLK_CYC_DISP   98
#define CLK_CYC_DISN   98
#define CLK_CYC_DV    103
#define CLK_CYC_DVIM  105
#define CLK_CYC_DR     97
#define CLK_CYC_DB    101
#define CLK_CYC_DBX   101
#define CLK_CYC_D     102
#define CLK_CYC_DIM   104
#define CLK_CYC_DDR   239
#define CLK_CYC_DD    245
#define CLK_CYC_INCM   17
#define CLK_CYC_DECM   17
#define CLK_CYC_ABS(negative)    (negative ? 10 : 5)
#define CLK_CYC_DABS(negative)   (negative ? 21 : 13)
#define CLK_CYC_NEG     5
#define CLK_CYC_DNEG   18
#define CLK_CYC_CR      8
#define CLK_CYC_CB     15
#define CLK_CYC_CBX    15
#define CLK_CYC_CISP   11
#define CLK_CYC_CISN   11
#define CLK_CYC_C      16
#define CLK_CYC_CIM    15
#define CLK_CYC_CBL    40
#define CLK_CYC_DCR    21
#define CLK_CYC_DC     27 
#define CLK_CYC_ORR     4
#define CLK_CYC_ORB    11
#define CLK_CYC_ORBX   11
#define CLK_CYC_OR     12
#define CLK_CYC_ORIM   11
#define CLK_CYC_XORR    4
#define CLK_CYC_XOR    12
#define CLK_CYC_XORM   11
#define CLK_CYC_ANDR    4
#define CLK_CYC_ANDB   11
#define CLK_CYC_ANDX   11
#define CLK_CYC_AND    12
#define CLK_CYC_ANDM   11
#define CLK_CYC_NR      7
#define CLK_CYC_N      15
#define CLK_CYC_NIM    14
/* Floating point arithmetic */
#define CLK_CYC_FAR    62
#define CLK_CYC_FAB    67
#define CLK_CYC_FABX   67
#define CLK_CYC_FA     68
#define CLK_CYC_EFAR   71
#define CLK_CYC_EFA    78
#define CLK_CYC_FSR    62
#define CLK_CYC_FSB    67
#define CLK_CYC_FSBX   67
#define CLK_CYC_FS     68
#define CLK_CYC_EFSR   71
#define CLK_CYC_EFS    78
#define CLK_CYC_FMR   120
#define CLK_CYC_FMB   125
#define CLK_CYC_FMBX  125
#define CLK_CYC_FM    126
#define CLK_CYC_EFMR  251
#define CLK_CYC_EFM   258
#define CLK_CYC_FDR   234
#define CLK_CYC_FDB   239
#define CLK_CYC_FDBX  239
#define CLK_CYC_FD    240
#define CLK_CYC_EFDR  480
#define CLK_CYC_EFD   487
#define CLK_CYC_FCR    52
#define CLK_CYC_FCB    57
#define CLK_CYC_FCBX   57
#define CLK_CYC_FC     58
#define CLK_CYC_EFCR   52
#define CLK_CYC_EFC    65
#define CLK_CYC_FABS(negative)   (negative ? 62 : 16)
#define CLK_CYC_FNEG   56
#define CLK_CYC_FIX    10
#define CLK_CYC_FLT    16
#define CLK_CYC_EFIX   21
#define CLK_CYC_EFLT   25
/* Bit operations */
#define CLK_CYC_SBR     7
#define CLK_CYC_SB     16
#define CLK_CYC_SBI    20
#define CLK_CYC_RBR     7
#define CLK_CYC_RB     16
#define CLK_CYC_RBI    20
#define CLK_CYC_TBR     7
#define CLK_CYC_TB     15
#define CLK_CYC_TBI    19
#define CLK_CYC_TSB    23
#define CLK_CYC_SVBR    7
#define CLK_CYC_RVBR    7
#define CLK_CYC_TVBR    7
/* Shift operations */
#define CLK_CYC_SLL(n_shifts)    (7 + 3 * n_shifts)
#define CLK_CYC_SRL(n_shifts)    (7 + 3 * n_shifts)
#define CLK_CYC_SRA(n_shifts)    (7 + 3 * n_shifts)
#define CLK_CYC_SLC(n_shifts)    (7 + 3 * n_shifts)
#define CLK_CYC_DSLL(n_shifts)   (16 + 6 * n_shifts)
#define CLK_CYC_DSRL(n_shifts)   (16 + 6 * n_shifts)
#define CLK_CYC_DSRA(n_shifts)   (16 + 6 * n_shifts)
#define CLK_CYC_DSLC(n_shifts)   (16 + 6 * n_shifts)
#define CLK_CYC_SLR(n_shifts)    (n_shifts<0 ? 21+n_shifts*3 : n_shifts>0 ? 38+n_shifts*5 : 11)
#define CLK_CYC_SAR(n_shifts)    (n_shifts<0 ? 21+n_shifts*3 : n_shifts>0 ? 29+n_shifts*5 : 11)
#define CLK_CYC_SCR(n_shifts)    (n_shifts<0 ? 21+n_shifts*3 : n_shifts>0 ? 24+n_shifts*3 : 11)
#define CLK_CYC_DSLR(n_shifts)   (n_shifts<0 ? 30+n_shifts*6 : n_shifts>0 ? 44+n_shifts*8 : 11)
#define CLK_CYC_DSAR(n_shifts)   (n_shifts<0 ? 30+n_shifts*6 : n_shifts>0 ? 35+n_shifts*8 : 11)
#define CLK_CYC_DSCR(n_shifts)   (n_shifts<0 ? 33+n_shifts*9 : n_shifts>0 ? 33+n_shifts*9 : 11)
/* Load/store/exchange */
#define CLK_CYC_LR      4
#define CLK_CYC_LB     11
#define CLK_CYC_LBX    11
#define CLK_CYC_LISP    7
#define CLK_CYC_LISN    7
#define CLK_CYC_L      12
#define CLK_CYC_LIM    12
#define CLK_CYC_LI     16
#define CLK_CYC_DLR    16
#define CLK_CYC_DLB    21
#define CLK_CYC_DLBX   21
#define CLK_CYC_DL     22
#define CLK_CYC_DLI    26
#define CLK_CYC_EFL    26
#define CLK_CYC_LUB    15
#define CLK_CYC_LUBI   19
#define CLK_CYC_LLB    12
#define CLK_CYC_LLBI   16
#define CLK_CYC_STB    11
#define CLK_CYC_STBX   11
#define CLK_CYC_ST     12
#define CLK_CYC_STI    16
#define CLK_CYC_STC    12
#define CLK_CYC_STCI   16
#define CLK_CYC_DSTB   15
#define CLK_CYC_DSTX   15
#define CLK_CYC_DST    16
#define CLK_CYC_DSTI   20
#define CLK_CYC_SRM    25
#define CLK_CYC_EFST   20
#define CLK_CYC_STUB   16
#define CLK_CYC_SUBI   20
#define CLK_CYC_STLB   16
#define CLK_CYC_SLBI   20
#define CLK_CYC_XBR     7
#define CLK_CYC_XWR    10
/* Multiple load/store */
#define CLK_CYC_PSHM(n_pushes)   (16 + 12 * n_pushes)
#define CLK_CYC_POPM(n_pops)   (20 + 16 * n_pops)
#define CLK_CYC_LM(n_loads)     (16 +  8 * n_loads)
#define CLK_CYC_STM(n_stores)    (17 +  9 * n_stores)
#define CLK_CYC_MOV_INIT    (9)
#define CLK_CYC_MOV_STEP(n_moves)   (13 * n_moves)
#define CLK_CYC_MOV_FINI(n_moves)    (n_moves == 0 ? 0 : 28)
/* Program control */
#define CLK_CYC_JC(jump_taken)     (jump_taken ? 17 :  9)
#define CLK_CYC_JCI(jump_taken)    (jump_taken ? 21 : 13)
#define CLK_CYC_JS     12
#define CLK_CYC_SOJ(jump_taken)    (jump_taken ? 17 : 13)
#define CLK_CYC_BR     14
#define CLK_CYC_BRcc(branch_taken)	  (branch_taken ? 15 : 4)  /* for all conditional branches */
#define CLK_CYC_BEX    87
#define CLK_CYC_LST    42
#define CLK_CYC_LSTI   46
#define CLK_CYC_SJS    22
#define CLK_CYC_URS    15
#define CLK_CYC_NOP_BPT     9
#define CLK_CYC_BPT    27
#define CLK_CYC_BIF    34
#define CLK_CYC_XIO    60  /* very roughly */
#define CLK_CYC_VIO(data_words)   (70+20*data_words) /* very roughly */

#endif


/* The ERA MAS281 board has a 10MHz clock */
#define CYCLE_DURATION_IN_NS 100
#define TIMER_A_RES_IN_10uSEC 1
#define TIMER_B_RES_IN_10uSEC 10

/* Watchdog (GO Timer) period as measured in 10 microsecond units: */
#define GOTIMER_PERIOD_IN_10uSEC  25