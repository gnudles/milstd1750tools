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