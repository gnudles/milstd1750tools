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
    OP_JUMP_SUBRTN,    /* JS, SJS, URS */
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
    OP_SPECIAL,  /* Special cases that don't fit the above categories (XIO, BEX, BIF) */
};


/* 1. The Global Constant Dictionary (Used only during Decode) */
typedef struct {
    const char * name;     /* Human-readable name of the instruction for debugging */
    uint16_t  base_cycles; /* Base cycles for the instruction */
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