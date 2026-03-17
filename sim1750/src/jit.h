#include "type.h"

#define  CS_R_C     0x8 /* read carry */
#define  CS_R_P  0x4 /* read positive */
#define  CS_R_Z      0x2 /* read zero */
#define  CS_R_N  0x1 /* read negative */
#define  CS_W_C     0x80 /* write carry */
#define  CS_W_P  0x40 /* write positive */
#define  CS_W_Z      0x20 /* write zero */
#define  CS_W_N  0x10 /* write negative */

#define CS_W_ALL (CS_W_C | CS_W_P | CS_W_Z | CS_W_N)
#define CS_R_ALL (CS_R_C | CS_R_P | CS_R_Z | CS_R_N)
#define CS_RW_NN 0x00 /* No condition code bits read or written */
#define CS_R_NZ (CS_R_N | CS_R_Z) /* Reads N and Z, but not C or P */
#define CS_R_PZ (CS_R_P | CS_R_Z) /* Reads P and Z, but not C or N */
#define CS_R_PNZ (CS_R_P | CS_R_Z | CS_R_N) /* Reads P, Z, and N, but not C */

enum InstructionFormat {
    IF_REG_REG,        /* Register to Register (8bit opcode, 4bit register index, 4bit register index) */
    IF_CONST_REG,        /* 4 bit constant, Register (8bit opcode, 4bit constant, 4bit register index - like SBR) */
    IF_REG_CONST,        /* Register, 4bit constant (8bit opcode, 4bit register index, 4bit constant - like LISP, LISN, AISP) */
    IF_BASE_RELATIVE,  /* Base Relative (6bit opcode, 2bit register index 12-15, 8bit displacement) */
    IF_BASE_REL_INDX,  /* (BRX) Base Relative Indexed (6bit opcode, 2bit register index 12-15, 4bit sub-opcode, 4bit RX) */
    IF_COUNTER_RELATIVE,         /* Branch (8bit opcode, 8bit displacement) */
    IF_SPECIAL,          /* Special cases that don't fit the above formats (e.g. BIF, NOP, BPT) (8bit opcode, 8bit extension) */
    IF_IMMEDIATE,      /* (IMM) Immediate (8bit opcode, 4bit register index, 4bit sub-opcode, 16 bit immediate)*/
    IF_LONG,           /* Long (8bit opcode, 4bit register index, 4bit RX, 16 bit target address) */
    IF_CONST_LONG           /* Constant to Long (8bit opcode, 4bit constant, 4bit RX, 16 bit target address - like STC, STCI) */
    
};
enum OperandsType{
    OPERAND_INT16,
    OPERAND_INT32,
    OPERAND_FLOAT,
    OPERAND_EXFLOAT
};
enum AddressingMode {
    AM_REG_DIRECT_R, /* Register Direct (R)
        An addressing mode in which the instruction specified register
        contains the required operand. (With the exception of this address
        mode, DA denotes a memory address.) */
    AM_MEM_DIRECT_D_DX, /* Memory Direct (D)
        An addressing mode in which the instruction contains the memory
        address of the operand. */
                     /* Memory Direct-Indexed (DX)
        An addressing mode in which the memory address of the required
        operand is specified by the sum of the content of an index register
        and the instruction address field. Registers R1, R2, ..., R15 may
        be specified for indexing. */
    AM_MEM_INDIRECT_I_IX, /* Memory Indirect (I)
        An addressing mode in which the instruction specified memory
        address contains the address of the required operand. */
                         /* Memory Indirect with Pre-Indexing (IX)
        An addressing mode in which the sum of the content of a specified
        index register and the instruction address field is the address of the
        address of the required operand. Registers R1, R2, ..., R15 may be
        specified for pre-indexing. */
    AM_IMM_LONG_IM_IMX, /* Immediate Long (IM)
        There shall be two methods of Immediate Long addressing: one
        which allows indexing and one which does not. The indexable form
        of immediate addressing is defined in Table V, “Addressing Modes
        and Instruction Formats” [20]. If the specified index register, RX,
        is not equal to zero, the content of RX is added to the immediate
        field to form the required operand; otherwise the immediate field
        contains the required operand.*/
    AM_IMM_SHORT_IS, /* Immediate Short (IS)
        An addressing mode in which the required (4-bit) operand is
        contained within the (16-bit) instruction. There shall be two
        methods of Immediate Short addressing: one which interprets the
        content of the immediate field as positive data, and a second which
        interprets the content of immediate field as negative data. */
        AM_IMM_SHRT_POS_ISP, /* Immediate Short Positive (ISP)
            The immediate operand is treated as a positive integer between 1 and 16. */
        AM_IMM_SHRT_NEG_ISN, /* Immediate Short Negative (ISN)
            The immediate operand is treated as a negative integer between 1 and 16.
            Its internal form shall be a 2's complement, sign-extended 16-bit number. */
        AM_CNT_REL_ICR, /* Instruction Counter Relative (ICR)
            Used for 16-bit branch instructions. The contents of the instruction counter
            minus one is added to the sign extended 8-bit displacement field. */
        AM_BASE_REL_B, /* Base Relative (B)
            An addressing mode in which the content of an instruction specified
            base register is added to the 8-bit displacement field of the (16-bit)
            instruction. The displacement field is taken to be a positive number
            between 0 and 255. The sum points to the memory address of the
            required operand. This mode allows addressing within a memory
            region of 256 words beginning at the address pointed to by the base
            register. */
        AM_BASE_REL_IDX_BX, /* Base Relative-Indexed (BX)
            The sum of the contents of a specified index register and a specified base
            register is the address of the required operand. */
        AM_SPECIAL_S /* Special (S)
            Used where none of the other addressing modes are applicable. */
};
enum OperationType {
    OP_SET_BIT,     /* SB, SBI, SBR */
    OP_RESET_BIT,   /* RB, RBI, RBR */
    OP_TEST_BIT,    /* TB, TBI, TBR */
    OP_TEST_SET_BIT, /* TSB */
    OP_SET_VAR_BIT, /* SVBR */
    OP_RESET_VAR_BIT, /* RVBR */
    OP_TEST_VAR_BIT, /* TVBR */
    OP_SHIFT,       /* 16bit : SLL, SRL, SRA, SLC, SLR, SAR, SCR */
      /* 32bit :DSLL, DSRL, DSRA, DSLC, DSLR, DSAR, DSCR */
    OP_JUMP_COND,    /* Jump on Condition operations (JC, JCI) */
    OP_JUMP_SUBRTN,    /* JS, SJS */
    OP_RET_SUBRTN,    /* URS */
    OP_SUBTR_JUMP,    /* SOJ  - Subtract one and Jump */
    OP_BRANCH,  /* Branching operations (BR, BLT, BLE, BEZ, BGT, BGE, BNZ) */
    OP_LOAD_STATUS, /* LSTI, LST */
    OP_LOAD,    /* 16bit : L, LR, LISP, LISN, LI, LIM, LB, LBX */
                /* 32bit : DL, DLB, DLR, DLBX, DLI */
                /* 48bit : EFL */
    OP_MULT_REG, /* LM, POPM, STM, PSHM */
    OP_LOAD_BYTE, /* LUB, LUBI, LLB, LLBI */
    OP_STORE, /* 16bit : STB, ST, STBX, STI, STC, STCI (STZ, STZI are special case of STC, STCI where N is 0) */
                /* 32bit : DST, DSTI, DSTB, DSTX */
                /* 48bit : EFST */
    OP_MOVE, /* MOV */
    OP_STORE_MASK, /* SRM */
    OP_STORE_BYTE, /* STUB, SUBI, STLB, SLBI */
    OP_ADD, /* 16bit: AR, AB, ABX, AISP, A, AIM*/
            /* 32bit: DAR, DA*/
    OP_INC_DEC_MEM, /* INCM, DECM */
    OP_ABS, /* 16bit: ABS */
        /* 32bit: DABS */
    OP_ADD_FLOAT, /* FAR, FAB, FABX, FA */
    OP_ADD_EXFLOAT, /* EFAR, EFA */
    OP_ABS_FLOAT, /* FABS - Floating point absolute value */
    OP_SUB, /* 16bit: SR, SBB, SBBX, SISP, S, SIM*/
            /* 32bit: DSR, DS*/
    OP_NEG, /* 16bit: NEG */
        /* 32bit: DNEG */
    OP_NEG_FLOAT, /* FNEG */
    OP_SUB_FLOAT, /* FSR, FS, FSB, FSBX */
    OP_SUB_EXFLOAT, /* EFSR, EFS */
    OP_MULT_PROD16, /* 16bit: MSR, MISP, MISN, MS, MSIM */
    OP_MULT_PROD32, /* 16bit operands, 32bit product - MR, MB, MBX, M, MIM */
    OP_DMULT_PROD32, /* 32bit operands, 32bit product - DMR, DM */
    OP_MULT_FLOAT, /* FMR, FM, FMB, FMBX */
    OP_MULT_EXFLOAT, /* EFM, EFMR */
    OP_DIV_REM_16_16, /* DV, DVR, DISP, DISN, DVIM */
    OP_DIV_REM_32_16, /* 32bit dividend, 16bit divisor: DR, DB, DBX, D, DIM */
    OP_DIV_32_32, /* 32bit dividend, 32bit divisor: DDR, DD */
    OP_DIV_FLOAT, /* FDR, FDB, FDBX, FD */
    OP_DIV_EXFLOAT, /* EFDR, EFD */
    OP_OR, /* OR, ORR, ORB, ORBX, ORIM */
    OP_AND, /* AND, ANDR, ANDB, ANDX, ANDM */
    OP_XOR, /* XOR, XORR, XORM */
    OP_NAND, /* N, NR, NIM */
    OP_FLT_TO_INT16, /* FIX */
    OP_INT16_TO_FLT, /* FLT */
    OP_EFLT_TO_INT32, /* EFIX */
    OP_INT32_TO_EFLT, /* EFLT */
    OP_EXCHANGE_BYTE, /* XBR */
    OP_EXCHANGE_WORD, /* XWR */
    OP_COMPARE, /* 16bit: CR, CB, CBX, CISP, CISN, C, CIM */
                /* 32bit: DCR, DC */
    OP_COMP_LIM, /* Compare between limits - CBL */
    OP_COMPARE_FLOAT, /* FCR, FC, FCB, FCBX */
    OP_COMPARE_EXFLOAT, /* EFCR, EFC */
    OP_EXTENSION, /* NOP, BPT */
    OP_BR_EXECUTIVE, /* BEX */
    OP_XIO, /* XIO, VIO */
    OP_BIF, /* BIF */

    OP_SPECIAL,  /* Special cases that don't fit the above categories ( IMM, BRX) */
};
enum OpCode8bit
{   
    /* we keep the numbers tight with bit representation. illegals are not defined but reserves their number */
    
    OPC_LB = 0x00,
    OPC_DLB = 0x04,
    OPC_STB = 0x08,
    OPC_DSTB = 0x0C,
    OPC_AB = 0x10,
    OPC_SBB = 0x14,
    OPC_MB = 0x18,
    OPC_DB = 0x1C,
    OPC_FAB = 0x20,
    OPC_FSB = 0x24,
    OPC_FMB = 0x28,
    OPC_FDB = 0x2C,
    OPC_ORB = 0x30,
    OPC_ANDB = 0x34,
    OPC_CB = 0x38,
    OPC_FCB = 0x3C,
    OPC_BRX = 0x40, // a family of sub opcodes
    OPC_XIO = 0x48,
    OPC_VIO = 0x49,
    OPC_IMM = 0x4A,
    // ILLEGAL = 0x4B
    // ILLEGAL = 0x4C
#ifdef GVSC
    OPC_ESQR = 0x4D,
    OPC_SQRT = 0x4E,
#endif
    // ILLEGAL = 0x4D
    // ILLEGAL = 0x4E
    OPC_BIF = 0x4F,
    OPC_SB = 0x50,
    OPC_SBR = 0x51,
    OPC_SBI = 0x52,
    OPC_RB = 0x53,
    OPC_RBR = 0x54,
    OPC_RBI = 0x55,
    OPC_TB = 0x56,
    OPC_TBR = 0x57,
    OPC_TBI = 0x58,
    OPC_TSB = 0x59,
    OPC_SVBR = 0x5A,
    // ILLEGAL = 0x5B
    OPC_RVBR = 0x5C,
    // ILLEGAL = 0x5D
    OPC_TVBR = 0x5E,
    // ILLEGAL = 0x5F
    OPC_SLL = 0x60,
    OPC_SRL = 0x61,
    OPC_SRA = 0x62,
    OPC_SLC = 0x63, 
    // ILLEGAL = 0x64
    OPC_DSLL = 0x65,
    OPC_DSRL = 0x66,
    OPC_DSRA = 0x67,
    OPC_DSLC = 0x68,
    // ILLEGAL = 0x69
    OPC_SLR = 0x6A,
    OPC_SAR = 0x6B,
    OPC_SCR = 0x6C,
    OPC_DSLR = 0x6D,
    OPC_DSAR = 0x6E,
    OPC_DSCR = 0x6F,
    OPC_JC = 0x70,
    OPC_JCI = 0x71,
    OPC_JS = 0x72,
    OPC_SOJ = 0x73,
    OPC_BR = 0x74,
    OPC_BEZ = 0x75,
    OPC_BLT = 0x76,
    OPC_BEX = 0x77,
    OPC_BLE = 0x78,
    OPC_BGT = 0x79,
    OPC_BNZ = 0x7A,
    OPC_BGE = 0x7B,
    OPC_LSTI = 0x7C,
    OPC_LST = 0x7D,
    OPC_SJS = 0x7E,
    OPC_URS = 0x7F,
    OPC_L = 0x80,
    OPC_LR = 0x81,
    OPC_LISP = 0x82,
    OPC_LISN = 0x83,
    OPC_LI = 0x84,
    OPC_LIM = 0x85,
    OPC_DL = 0x86,
    OPC_DLR = 0x87,
    OPC_DLI = 0x88,
    OPC_LM = 0x89,
    OPC_EFL = 0x8A,
    OPC_LUB = 0x8B,
    OPC_LLB = 0x8C,
    OPC_LUBI = 0x8D,
    OPC_LLBI = 0x8E,
    OPC_POPM = 0x8F,
    OPC_ST = 0x90,
    OPC_STC = 0x91,
    OPC_STCI = 0x92,
    OPC_MOV = 0x93,
    OPC_STI = 0x94,
#ifdef GVSC
    OPC_SFBS = 0x95,
#endif
    // ILLEGAL = 0x95
    OPC_DST = 0x96,
    OPC_SRM = 0x97,
    OPC_DSTI = 0x98,
    OPC_STM = 0x99,
    OPC_EFST = 0x9A,
    OPC_STUB = 0x9B,
    OPC_STLB = 0x9C,
    OPC_SUBI = 0x9D,
    OPC_SLBI = 0x9E,
    OPC_PSHM = 0x9F,
    OPC_A = 0xA0,
    OPC_AR = 0xA1,
    OPC_AISP = 0xA2,
    OPC_INCM = 0xA3,
    OPC_ABS = 0xA4,
    OPC_DABS = 0xA5,
    OPC_DA = 0xA6,
    OPC_DAR = 0xA7,
    OPC_FA = 0xA8,
    OPC_FAR = 0xA9,
    OPC_EFA = 0xAA,
    OPC_EFAR = 0xAB,
    OPC_FABS = 0xAC,
#ifdef GVSC
    OPC_UAR = 0xAD,
    OPC_UA = 0xAE,
#endif
    // ILLEGAL = 0xAD
    // ILLEGAL = 0xAE
    // ILLEGAL = 0xAF
    OPC_S = 0xB0,
    OPC_SR = 0xB1,
    OPC_SISP = 0xB2,
    OPC_DECM = 0xB3,
    OPC_NEG = 0xB4,
    OPC_DNEG = 0xB5,
    OPC_DS = 0xB6,
    OPC_DSR = 0xB7,
    OPC_FS = 0xB8,
    OPC_FSR = 0xB9,
    OPC_EFS = 0xBA,
    OPC_EFSR = 0xBB,
    OPC_FNEG = 0xBC,
#ifdef GVSC
    OPC_USR = 0xBD,
    OPC_US = 0xBE,
#endif
    // ILLEGAL = 0xBD
    // ILLEGAL = 0xBE
    // ILLEGAL = 0xBF
    OPC_MS = 0xC0,
    OPC_MSR = 0xC1,
    OPC_MISP = 0xC2,
    OPC_MISN = 0xC3,
    OPC_M = 0xC4,
    OPC_MR = 0xC5,
    OPC_DM = 0xC6,
    OPC_DMR = 0xC7,
    OPC_FM = 0xC8,
    OPC_FMR = 0xC9,
    OPC_EFM = 0xCA,
    OPC_EFMR = 0xCB,
    // ILLEGAL = 0xCC
    // ILLEGAL = 0xCD
    // ILLEGAL = 0xCE
    // ILLEGAL = 0xCF
    OPC_DV = 0xD0,
    OPC_DVR = 0xD1,
    OPC_DISP = 0xD2,
    OPC_DISN = 0xD3,
    OPC_D = 0xD4,
    OPC_DR = 0xD5,
    OPC_DD = 0xD6,
    OPC_DDR = 0xD7,
    OPC_FD = 0xD8,
    OPC_FDR = 0xD9,
    OPC_EFD = 0xDA,
    OPC_EFDR = 0xDB,
#ifdef GVSC
    OPC_STE = 0xDC,
    OPC_DSTE = 0xDD,
    OPC_LE = 0xDE,
    OPC_DLE = 0xDF,
#endif
    // ILLEGAL = 0xDC
    // ILLEGAL = 0xDD
    // ILLEGAL = 0xDE
    // ILLEGAL = 0xDF
    OPC_OR = 0xE0,
    OPC_ORR = 0xE1,
    OPC_AND = 0xE2,
    OPC_ANDR = 0xE3,
    OPC_XOR = 0xE4,
    OPC_XORR = 0xE5,
    OPC_N = 0xE6,
    OPC_NR = 0xE7,
    OPC_FIX = 0xE8,
    OPC_FLT = 0xE9,
    OPC_EFIX = 0xEA,
    OPC_EFLT = 0xEB,
    OPC_XBR = 0xEC,
    OPC_XWR = 0xED,
    // ILLEGAL = 0xEE
    // ILLEGAL = 0xEF
    OPC_C = 0xF0,
    OPC_CR = 0xF1,
    OPC_CISP = 0xF2,
    OPC_CISN = 0xF3,
    OPC_CBL = 0xF4,
#ifdef GVSC
    OPC_UCIM = 0xF5,
#endif
    // ILLEGAL = 0xF5
    OPC_DC = 0xF6,
    OPC_DCR = 0xF7,
    OPC_FC = 0xF8,
    OPC_FCR = 0xF9,
    OPC_EFC = 0xFA,
    OPC_EFCR = 0xFB,
#ifdef GVSC
    OPC_UCR = 0xFC,
    OPC_UC = 0xFD,
#endif
    // ILLEGAL = 0xFC
    // ILLEGAL = 0xFD
    // ILLEGAL = 0xFE
    OPC_NOP_BPT = 0xFF,
    OPC_ILLEGAL = 0x100,

    OPC_BRX_LBX = 0x4000,
    OPC_BRX_DLBX = 0x4010,
    OPC_BRX_STBX = 0x4020,
    OPC_BRX_DSTX = 0x4030,
    OPC_BRX_ABX = 0x4040,
    OPC_BRX_SBBX = 0x4050,
    OPC_BRX_MBX = 0x4060,
    OPC_BRX_DBX = 0x4070,
    OPC_BRX_FABX = 0x4080,
    OPC_BRX_FSBX = 0x4090,
    OPC_BRX_FMBX = 0x40A0,
    OPC_BRX_FDBX = 0x40B0,
    OPC_BRX_CBX = 0x40C0,
    OPC_BRX_FCBX = 0x40D0,
    OPC_BRX_ANDX = 0x40E0,
    OPC_BRX_ORBX = 0x40F0,

    OPC_IMM_AIM = 0x4A01,
    OPC_IMM_SIM = 0x4A02,
    OPC_IMM_MIM = 0x4A03,
    OPC_IMM_MSIM = 0x4A04,
    OPC_IMM_DIM = 0x4A05,
    OPC_IMM_DVIM = 0x4A06,
    OPC_IMM_ANDM = 0x4A07,
    OPC_IMM_ORIM = 0x4A08,
    OPC_IMM_XORM = 0x4A09,
    OPC_IMM_CIM = 0x4A0A,
    OPC_IMM_NIM = 0x4A0B,



};


/* 1. The Global Constant Dictionary (Used only during Decode) */
typedef struct {
    const char * name;     /* Human-readable name of the instruction for debugging */
    enum OpCode8bit code;
    /* CS = Condition Status */
    uint8_t     rw_cs; /* Which condition codes are written (bitmask) for dead code elimination */
    bool     is_branch;    /* Whether the instruction is a branch */
    bool jitable;             /* Whether the instruction is suitable for JIT compilation */
    bool valid;              /* Whether this opcode is valid (for quick checks) */
    bool is_imm;             /* Whether this instruction has an immediate operand (for special handling) */
    enum InstructionFormat format; /* The instruction format for this opcode */
    enum OperationType op_type; /* The type of operation this instruction performs  */
    enum AddressingMode addr_mode; /* The addressing mode used by this instruction  */
    enum OperandsType operands_type; /* The type of operands this instruction operates on (to know how many words to fetch) */
    const char * description;
    
    /* Add format types here (Register-to-Register, Base Relative, etc.) */
} OpcodeDef;

/* Represents a single decoded 1750A instruction */
typedef struct {
    uint16_t ic;      /* Instruction counter */
    uint16_t  opcode;
    uint16_t  immediate;   /* Valid if is_imm == true */
    uint16_t target_ic;    /* Valid if is_branch == true */
    uint8_t  ra, rb, rx;
    uint16_t cycles;       /* Static cycle cost of this instruction */
    
    bool     is_branch;

} IR_Insn;

/****************** Global Opcode Definition Tables ******************/
extern OpcodeDef opcode_defs_brx[16];   /* For BRX */
extern OpcodeDef opcode_defs_imm[16];   /* For  IMM */
extern OpcodeDef opcode_defs_6bit[17];  /* For all 6-bit opcodes (including BRX) */
extern OpcodeDef opcode_defs_8bit[188];     /* For all 8-bit opcodes (including IMM) */