#include "cpu_ctx.h"
#include <stdio.h>
#include <stdlib.h>

/* about memory layout.
    there are two kinds of pages: page (4096 words) and qpage (quarter page - 1024 words)
    1750a supports up to 256 pages (which is equivalent to 1024 qpages)
    quarter pages are relevant only for writing
*/
#ifdef RUNNING_TESTS
extern uint16_t mock_memory[0x10000];
static inline ushort* access_memory(struct cpu_state *cpu, uint16_t phys_page)
{
    return &mock_memory[(uint)phys_page<<12];
}
#else
static inline ushort* access_memory(struct cpu_state *cpu, uint16_t phys_page)
{
    return cpu->mem[phys_page]->word;
}
#endif

static inline uint16_t popcount16(uint16_t v) {
    /* 1. Count bits in each 2-bit field */
    v = v - ((v >> 1) & 0x5555);
    
    /* 2. Count bits in each 4-bit field */
    v = (v & 0x3333) + ((v >> 2) & 0x3333);
    
    /* 3. Count bits in each 8-bit field */
    v = (v + (v >> 4)) & 0x0F0F;
    
    /* 4. Multiply to sum the 8-bit fields into the top byte, then shift down */
    return (v * 0x0101) >> 8;
}

static inline uint get_quarter_page_address_write_data(struct cpu_state *cpu, uint16_t logical_qpage)
{
    uint16_t logical_page = logical_qpage >> 2;
    if (cpu->data_write_cache.valid & (0x8000000000000000ULL >> logical_qpage))
    {
        return (uint)cpu->data_write_cache.page[logical_page];
    }
    ushort as =  cpu->reg.sw & 0xF; /* Address State */
    ushort akc = (cpu->reg.sw >> 4) & 0xF; /* access key code */

    
    if (cpu->pagereg[DATA][as][logical_page].e_w != 0)
    {
        cpu->reg.pir |= INTR_MACHERR;
        cpu->reg.ft |= FT_MEMPROT;
        return 0xFFFFFFFF;
    }

    if (akc != 0) 
    {
        ushort al; /* access lock */
        al = cpu->pagereg[DATA][as][logical_page].al;
        if (al != 0xF && akc != al)
        {
            cpu->reg.pir |= INTR_MACHERR;
            cpu->reg.ft |= FT_MEMPROT;
            return 0xFFFFFFFF;
        }
    }
    uint phys_page = cpu->pagereg[DATA][as][logical_page].ppa;
    if (cpu->num_phys_mem_pages <= phys_page)
    {
        cpu->reg.pir |= INTR_MACHERR;
        cpu->reg.ft |= FT_ILL_ADDR;
        return 0xFFFFFFFF;
    }
    uint phys_qpage = (phys_page << 2) | (logical_qpage & 0x3);
    if ( (cpu->reg.sys & SYS_MEM_PROT) 
    &&
     (cpu->mem_protect[MP_PROC][phys_qpage/16] & (0x8000U >> (phys_qpage % 16))))
    {
        cpu->reg.pir |= INTR_MACHERR;
        cpu->reg.ft |= FT_MEMPROT;
        return 0xFFFFFFFF;
    }
    if (cpu->mem[phys_page] == NULL)
    {
        /* try to allocate*/
        if ((cpu->mem[phys_page] = (mem_t *) calloc (1, sizeof (mem_t))) == MNULL)
        {
            fprintf(stderr, "get_quarter_page_address_write_data: dynamic memory exhausted\n");
            exit(EXIT_FAILURE);
        }
    }
    cpu->data_write_cache.valid |= (0x8000000000000000ULL >> logical_qpage);
    cpu->data_write_cache.page[logical_page] = phys_page;
    //printf("get_quarter_page_address_write_data: translated 0x%05X to 0x%05X\n", logical_qpage<<10, phys_page<<12);
    return phys_page;
}
static inline uint get_page_address_read_data(struct cpu_state *cpu, uint16_t logical_page)
{
    if (cpu->data_read_cache.valid & (0x8000U >> logical_page))
    {
        return (uint)cpu->data_read_cache.page[logical_page];
    }
    ushort akc = (cpu->reg.sw >> 4) & 0xF; /* access key code */
    ushort as =  cpu->reg.sw & 0xF; /* Address State */

    if (akc != 0) 
    {
        /* 
          TABLE VII.  AL Code to Access Key Mapping

	     AL Code	   Acceptable Access Key Codes
	     -------       ---------------------------

		0			 0
		1			0,1	
		2			0,2
		3			0,3
		4			0,4
		5			0,5
		6			0,6
		7			0,7
		8			0,8
		9			0,9
		A			0,A
		B			0,B
		C			0,C
		D			0,D
		E			0,E
		F         0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F


		resulting from a DMA attempt shall set fault register bit 1 to 
		cause a machine error interrupt.  Note that the access lock and
		key codes defined in the above table have the following characteristics:

		    a.  An access lock code of F (Base 16) is an "unlocked"
		        lock code and allows any and all access key codes to
		        be acceptable.

		    b.  An access key code of 0 is a "master" key code and
			is acceptable to any and all access lock codes.

		    c.  Access key codes 1 through E (Base 16) are acceptable
			to only their own "matched" lock code or the "unlocked"
			lock code of F (Base 16).

		    d.  An access key code of F (Base 16) is acceptable to only
			the "unlocked" lock code of F (Base 16).
        */
        ushort al; /* access lock */
        al = cpu->pagereg[DATA][as][logical_page].al;
        if (al != 0xF && akc != al)
        {
            cpu->reg.pir |= INTR_MACHERR;
            cpu->reg.ft |= FT_MEMPROT;
            return 0xFFFFFFFF;
        }
    }
    uint phys_page = cpu->pagereg[DATA][as][logical_page].ppa;
    if (cpu->num_phys_mem_pages <= phys_page)
    {
        cpu->reg.pir |= INTR_MACHERR;
        cpu->reg.ft |= FT_ILL_ADDR;
        return 0xFFFFFFFF;
    }
    if (cpu->mem[phys_page] == NULL)
    {
        /* try to allocate*/
        if ((cpu->mem[phys_page] = (mem_t *) calloc (1, sizeof (mem_t))) == MNULL)
        {
            fprintf(stderr, "get_page_address_read_data: dynamic memory exhausted\n");
            exit(EXIT_FAILURE);
        }
    }
    cpu->data_read_cache.valid |= 0x8000U >> (logical_page);
    cpu->data_read_cache.page[logical_page] = phys_page;
    //printf("get_page_address_read_data: translated 0x%05X to 0x%05X\n", logical_page<<12, phys_page<<12);
    return phys_page;
}

static inline uint get_page_address_read_code(struct cpu_state *cpu, uint16_t logical_page)
{
    if (cpu->code_read_cache.valid & (0x8000U >> logical_page))
    {
        return (uint)cpu->code_read_cache.page[logical_page];
    }
    ushort as =  cpu->reg.sw & 0xF; /* Address State */
    ushort akc = (cpu->reg.sw >> 4) & 0xF; /* access key code */
    if (cpu->pagereg[CODE][as][logical_page].e_w != 0)
    {
        cpu->reg.pir |= INTR_MACHERR;
        cpu->reg.ft |= FT_MEMPROT;
        return 0xFFFFFFFF;
    }

    if (akc != 0) 
    {
        ushort al; /* access lock */
        al = cpu->pagereg[CODE][as][logical_page].al;
        if (al != 0xF && akc != al)
        {
            cpu->reg.pir |= INTR_MACHERR;
            cpu->reg.ft |= FT_MEMPROT;
            return 0xFFFFFFFF;
        }
    }
    uint phys_page = cpu->pagereg[CODE][as][logical_page].ppa;
    if (cpu->num_phys_mem_pages <= phys_page)
    {
        cpu->reg.pir |= INTR_MACHERR;
        cpu->reg.ft |= FT_ILL_ADDR;
        return 0xFFFFFFFF;
    }
    if (cpu->mem[phys_page] == NULL)
    {
        /* try to allocate*/
        if ((cpu->mem[phys_page] = (mem_t *) calloc (1, sizeof (mem_t))) == MNULL)
        {
            fprintf(stderr, "get_page_address_read_code: dynamic memory exhausted\n");
            exit(EXIT_FAILURE);
        }
    }
    cpu->code_read_cache.valid |= 0x8000U >> (logical_page);
    cpu->code_read_cache.page[logical_page] = phys_page;
    return phys_page;
}



/*
    returns remaining count
*/
uint16_t fetch_data_words(struct cpu_context *cpu_ctx, uint16_t addr, uint16_t count, uint16_t *data) {

    while (count)
    {
        uint phys_addr = get_page_address_read_data(&cpu_ctx->state, addr >> 12);
        if (phys_addr == 0xFFFFFFFF) return count;
        
        uint remaining = 4096 - (addr  & 0xFFF);
        if (remaining > count) remaining = count;
        
        for (uint i = 0; i < remaining; i++) {
            data[i] = access_memory(&cpu_ctx->state, phys_addr)[(addr + i) & 0xFFF];
        }
        count -= remaining;
        addr += remaining;
    }
    return 0;
}

uint16_t fetch_data_words_reg(struct cpu_context *cpu_ctx, uint16_t addr, uint16_t count, uint16_t RA) {

    while (count)
    {
        uint phys_addr = get_page_address_read_data(&cpu_ctx->state, addr >> 12);
        if (phys_addr == 0xFFFFFFFF) return count;
        
        uint remaining = 4096 - (addr  & 0xFFF);
        if (remaining > count) remaining = count;
        
        for (uint i = 0; i < remaining; i++) {
            cpu_ctx->state.reg.r[(RA+i)&0xF] = access_memory(&cpu_ctx->state, phys_addr)[(addr + i) & 0xFFF];
        }
        count -= remaining;
        addr += remaining;
    }
    return 0;
}

bool fetch_data_word(struct cpu_context *cpu_ctx, uint16_t addr, uint16_t *data) {

    uint phys_addr = get_page_address_read_data(&cpu_ctx->state, addr >> 12);
    if (phys_addr == 0xFFFFFFFF) return false;

    *data = access_memory(&cpu_ctx->state, phys_addr)[addr & 0xFFF];

    return true;
}
ushort* get_address_data(struct cpu_context *cpu_ctx, uint16_t addr, uint16_t *data) {
    uint phys_addr = get_quarter_page_address_write_data(&cpu_ctx->state, addr >> 10);
    if (phys_addr == 0xFFFFFFFF) return false;
    return &access_memory(&cpu_ctx->state, phys_addr)[(addr ) & 0xFFF];
}
/* get multiple address*/
uint16_t get_addresses_data(struct cpu_context *cpu_ctx, uint16_t addr, uint16_t count, uint16_t **pointers) {

    while (count)
    {
        uint phys_addr = get_quarter_page_address_write_data(&cpu_ctx->state, addr >> 10);
        if (phys_addr == 0xFFFFFFFF) return count;
        
        uint remaining = 1024 - (addr  & 0x3FF);
        if (remaining > count) remaining = count;
        
        for (uint i = 0; i < remaining; i++) {
            pointers[i] = &access_memory(&cpu_ctx->state, phys_addr)[(addr + i) & 0xFFF];
        }
        count -= remaining;
        addr += remaining;
    }
    return 0;
}

uint16_t store_data_words(struct cpu_context *cpu_ctx, uint16_t addr, uint16_t count, uint16_t *data) {

    while (count)
    {
        uint phys_addr = get_quarter_page_address_write_data(&cpu_ctx->state, addr >> 10);
        if (phys_addr == 0xFFFFFFFF) return count;
        
        uint remaining = 1024 - (addr  & 0x3FF);
        if (remaining > count) remaining = count;
        
        for (uint i = 0; i < remaining; i++) {
            access_memory(&cpu_ctx->state, phys_addr)[(addr + i) & 0xFFF] = data[i];
        }
        count -= remaining;
        addr += remaining;
    }
    return 0;
}

uint16_t store_data_words_reg(struct cpu_context *cpu_ctx, uint16_t addr, uint16_t count, uint16_t RA) {

    while (count)
    {
        uint phys_addr = get_quarter_page_address_write_data(&cpu_ctx->state, addr >> 10);
        if (phys_addr == 0xFFFFFFFF) return count;
        
        uint remaining = 1024 - (addr  & 0x3FF);
        if (remaining > count) remaining = count;
        
        for (uint i = 0; i < remaining; i++) {
            access_memory(&cpu_ctx->state, phys_addr)[(addr + i) & 0xFFF] = cpu_ctx->state.reg.r[(RA+i)&0xF];
        }
        count -= remaining;
        addr += remaining;
    }
    return 0;
}

bool store_data_word(struct cpu_context *cpu_ctx, uint16_t addr, uint16_t data) {

    uint phys_addr = get_quarter_page_address_write_data(&cpu_ctx->state, addr >> 10);
    if (phys_addr == 0xFFFFFFFF) return false;
    access_memory(&cpu_ctx->state, phys_addr)[(addr ) & 0xFFF] = data;
    return true;
}


uint16_t move_words(struct cpu_context *cpu_ctx, uint16_t from_addr, uint16_t to_addr, uint16_t count) {
    uint from_logical_page = 0xFFFFFFFF;
    uint to_logical_qpage = 0xFFFFFFFF;
    uint from_phys_addr;
    uint to_phys_addr;
    while (count)
    {
        if (from_logical_page != from_addr >> 12)
        {
            from_logical_page = from_addr >> 12;
            from_phys_addr = get_page_address_read_data(&cpu_ctx->state, from_logical_page);
            if (from_phys_addr == 0xFFFFFFFF)
            {
                return count;
            }
        
        }
        if (to_logical_qpage != to_addr >> 10)
        {
            to_logical_qpage = to_addr >> 10;
            to_phys_addr = get_quarter_page_address_write_data(&cpu_ctx->state, to_logical_qpage);
            if (to_phys_addr == 0xFFFFFFFF)
            {
                return count;
            }
        }
        /* because memory write protect is one bit for 1024 words, we work in 1024 word chuncks for writing*/
        uint remaining_from = 4096 - (from_addr  & 0xFFF);
        uint remaining_to = 1024 - (to_addr  & 0x3FF);
        uint remaining = remaining_from < remaining_to ? remaining_from : remaining_to;
        if (remaining > count) remaining = count;
        
        for (uint i = 0; i < remaining; i++) {
            cpu_ctx->state.mem[to_phys_addr]->word[(to_addr + i) & 0xFFF] = cpu_ctx->state.mem[from_phys_addr]->word[(from_addr + i) & 0xFFF];
        }
        count -= remaining;
        from_addr += remaining;
        to_addr += remaining;
    }
    return 0;
}


static inline void calculate_flags_16bit(struct cpu_context *cpu_ctx, int16_t result) {
    cpu_ctx->state.reg.sw &= 0x0FFF; /* resets carry too */
    if (result & 0x8000) cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    else if (result == 0) cpu_ctx->state.reg.sw |= CS_ZERO;
    else cpu_ctx->state.reg.sw |= CS_POSITIVE;
    // Note: Carry flag needs to be set by the specific operation logic
}

static inline void calculate_flags_32bit(struct cpu_context *cpu_ctx, int16_t result[2]) {
    cpu_ctx->state.reg.sw &= 0x0FFF; /* resets carry too */
    if (result[0] & 0x8000) cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    else if (result[0] == 0 && result[1] == 0) cpu_ctx->state.reg.sw |= CS_ZERO;
    else cpu_ctx->state.reg.sw |= CS_POSITIVE;
    // Note: Carry flag needs to be set by the specific operation logic
}

static inline void calculate_flags_32bit_reg(struct cpu_context *cpu_ctx, uint16_t reg_index) {
    cpu_ctx->state.reg.sw &= 0x0FFF; /* resets carry too */
    if (cpu_ctx->state.reg.r[reg_index] & 0x8000) cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    else if (cpu_ctx->state.reg.r[reg_index] == 0 && cpu_ctx->state.reg.r[(reg_index + 1)&0xF] == 0) cpu_ctx->state.reg.sw |= CS_ZERO;
    else cpu_ctx->state.reg.sw |= CS_POSITIVE;
    // Note: Carry flag needs to be set by the specific operation logic
}

static inline void calculate_flags_48bit(struct cpu_context *cpu_ctx, int16_t result[3]) {
    cpu_ctx->state.reg.sw &= 0x0FFF; /* resets carry too */
    if (result[0] & 0x8000) cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    else if (result[0] == 0 && result[1] == 0  && result[2] == 0) cpu_ctx->state.reg.sw |= CS_ZERO;
    else cpu_ctx->state.reg.sw |= CS_POSITIVE;
    // Note: Carry flag needs to be set by the specific operation logic
}

static inline void calculate_flags_48bit_reg(struct cpu_context *cpu_ctx, uint16_t reg_index) {
    cpu_ctx->state.reg.sw &= 0x0FFF; /* resets carry too */
    if (cpu_ctx->state.reg.r[reg_index] & 0x8000) cpu_ctx->state.reg.sw |= CS_NEGATIVE;
    else if (cpu_ctx->state.reg.r[reg_index] == 0 && cpu_ctx->state.reg.r[(reg_index + 1)&0xF] == 0  && cpu_ctx->state.reg.r[(reg_index + 2)&0xF] == 0) cpu_ctx->state.reg.sw |= CS_ZERO;
    else cpu_ctx->state.reg.sw |= CS_POSITIVE;
    // Note: Carry flag needs to be set by the specific operation logic
}

/* obsolete */
static inline int16_t fetch_code_word(struct cpu_context *cpu_ctx, uint16_t addr, bool *ok) {
    uint phys_addr =  get_phys_address (&cpu_ctx->state, CODE, cpu_ctx->state.reg.sw & 0xF, addr);
    ushort word;
    bool peek_ok = peek(&cpu_ctx->state, phys_addr, &word);
    if (!peek_ok) {
        *ok = peek_ok;
        printf("Failed to peek at address 0x%04X\n", phys_addr);
    }
    return (int16_t)word;
}



static inline void unpack_float32(int16_t w1, int16_t w2, int32_t *m, int16_t *e) {
    int32_t mantissa = ((uint32_t)w1 << 16) | ((uint16_t)w2 & 0xFF00);
    *m = mantissa >> 8;
    *e = (int8_t)(w2 & 0xFF);
}

static inline void pack_float32(struct cpu_context *cpu_ctx, uint16_t RA, int32_t M_res, int32_t E_res) {
    if (M_res == 0) {
        cpu_ctx->state.reg.r[(RA+0)&0xF] = 0;
        cpu_ctx->state.reg.r[(RA+1)&0xF] = 0;
    } else {
        uint32_t x = M_res ^ (M_res >> 1);
        if (x == 0) { M_res = 0xFF800000; E_res -= 23; } /* Handle exactly -1 */
        else {
            int clz = __builtin_clz(x);
            int shift = clz - 9;
            if (shift > 0) { M_res <<= shift; E_res -= shift; }
            else if (shift < 0) { M_res >>= -shift; E_res += -shift; }
        }
        if (E_res > 127) {
            cpu_ctx->state.reg.pir |= INTR_FLTOFL;
            if (M_res < 0) { cpu_ctx->state.reg.r[(RA+0)&0xF] = 0x8000; cpu_ctx->state.reg.r[(RA+1)&0xF] = 0x007F; }
            else { cpu_ctx->state.reg.r[(RA+0)&0xF] = 0x7FFF; cpu_ctx->state.reg.r[(RA+1)&0xF] = 0xFF7F; }
        } else if (E_res < -128) {
            cpu_ctx->state.reg.pir |= INTR_FLTUFL;
            cpu_ctx->state.reg.r[(RA+0)&0xF] = 0;
            cpu_ctx->state.reg.r[(RA+1)&0xF] = 0;
        } else {
            cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)(M_res >> 8);
            cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)((M_res & 0xFF) << 8) | (uint8_t)E_res;
        }
    }
    calculate_flags_32bit_reg(cpu_ctx, RA);
}

static inline void unpack_float48(int16_t w1, int16_t w2, int16_t w3, int64_t *m, int16_t *e) {
    int64_t mantissa = ((uint64_t)w1 << 24) | (((int64_t)w2 & 0xFF00) << 8) | ((uint16_t)w3);
    *m = (int64_t)((uint64_t)mantissa << 24) >> 24;
    *e = (int8_t)(w2 & 0xFF);
}

static inline void pack_float48(struct cpu_context *cpu_ctx, uint16_t RA, int64_t M_res, int32_t E_res) {
    if (M_res == 0) {
        cpu_ctx->state.reg.r[(RA+0)&0xF] = 0; cpu_ctx->state.reg.r[(RA+1)&0xF] = 0; cpu_ctx->state.reg.r[(RA+2)&0xF] = 0;
    } else {
        uint64_t x = M_res ^ (M_res >> 1);
        if (x == 0) { M_res = 0xFFFFFF8000000000ULL; E_res -= 39; } /* Handle exactly -1 */
        else {
            int clz = __builtin_clzll(x);
            int shift = clz - 25;
            if (shift > 0) { M_res <<= shift; E_res -= shift; }
            else if (shift < 0) { M_res >>= -shift; E_res += -shift; }
        }
        if (E_res > 127) {
            cpu_ctx->state.reg.pir |= INTR_FLTOFL;
            if (M_res < 0) { cpu_ctx->state.reg.r[(RA+0)&0xF] = 0x8000; cpu_ctx->state.reg.r[(RA+1)&0xF] = 0x007F; cpu_ctx->state.reg.r[(RA+2)&0xF] = 0x0000; }
            else { cpu_ctx->state.reg.r[(RA+0)&0xF] = 0x7FFF; cpu_ctx->state.reg.r[(RA+1)&0xF] = 0xFF7F; cpu_ctx->state.reg.r[(RA+2)&0xF] = 0xFFFF; }
        } else if (E_res < -128) {
            cpu_ctx->state.reg.pir |= INTR_FLTUFL;
            cpu_ctx->state.reg.r[(RA+0)&0xF] = 0; cpu_ctx->state.reg.r[(RA+1)&0xF] = 0; cpu_ctx->state.reg.r[(RA+2)&0xF] = 0;
        } else {
            cpu_ctx->state.reg.r[(RA+0)&0xF] = (uint16_t)((M_res >> 24) & 0xFFFF);
            cpu_ctx->state.reg.r[(RA+1)&0xF] = (uint16_t)((M_res >> 8) & 0xFF00) | (uint8_t)E_res;
            cpu_ctx->state.reg.r[(RA+2)&0xF] = (uint16_t)(M_res & 0xFFFF);
        }
    }
    calculate_flags_48bit_reg(cpu_ctx, RA);
}


static inline invalidate_mem_cache(struct cpu_context *cpu_ctx) {
    /* we need to invalidate after XIO, and after interrupt or LST*/
    cpu_ctx->state.data_read_cache.valid = 0x0000;
    cpu_ctx->state.code_read_cache.valid = 0x0000;
    cpu_ctx->state.data_write_cache.valid = 0x0000;
}

static void
realize_xio (struct cpu_context *cpu_ctx, ushort xio_address, ushort *transfer)
{
}