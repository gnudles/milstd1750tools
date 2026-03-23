
#ifndef _CPU_HELPERS_H
#define _CPU_HELPERS_H
#include "cpu_ctx.h"
#include "clock_cycles.h"
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
    if (phys_page == 0xFFFFFFFF) return &mock_memory[0];
    return &mock_memory[((uint)phys_page<<12) & 0xFFFF];
}
#else
static inline ushort* access_memory(struct cpu_state *cpu, uint16_t phys_page)
{
    return cpu->mem[phys_page]->word;
}
#endif

static inline ushort* access_effective_address(struct cpu_state *cpu, uint32_t phys_addr)
{
    return &access_memory(cpu, phys_addr >> 12)[phys_addr & 0xFFF];
}

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
        cpu->halt = true;
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
        cpu->halt = true;
        fprintf(stderr, "Accessing non existing page!\n");
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



static inline uint get_page_address_read_data_intr(struct cpu_state *cpu, uint16_t logical_page)
{
    if (cpu->data_read_cache_intr.valid & (0x8000U >> logical_page))
    {
        return (uint)cpu->data_read_cache_intr.page[logical_page];
    }
    
    uint phys_page = cpu->pagereg[DATA][0][logical_page].ppa;
    if (cpu->num_phys_mem_pages <= phys_page)
    {
        cpu->reg.pir |= INTR_MACHERR;
        cpu->reg.ft |= FT_ILL_ADDR;
        cpu->halt = true;
        fprintf(stderr, "Accessing non existing page!\n");
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
    cpu->data_read_cache_intr.valid |= 0x8000U >> (logical_page);
    cpu->data_read_cache_intr.page[logical_page] = phys_page;
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
        cpu->halt = true;
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
        cpu->halt = true;
        fprintf(stderr, "Accessing non existing page!\n");
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
        #ifndef RUNNING_TESTS
        fprintf(stderr, "get_page_address_read_code: cannot execute unallocated page!\n");

        cpu->halt = true;
        #endif

        
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

uint16_t fetch_data_words_intr(struct cpu_context *cpu_ctx, uint16_t addr, uint16_t count, uint16_t *data) {

    while (count)
    {
        uint phys_addr = get_page_address_read_data_intr(&cpu_ctx->state, addr >> 12);
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
ushort* get_address_data(struct cpu_context *cpu_ctx, uint16_t addr) {
    uint phys_addr = get_quarter_page_address_write_data(&cpu_ctx->state, addr >> 10);
    if (phys_addr == 0xFFFFFFFF) return NULL;
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
    uint from_phys_addr = 0;
    uint to_phys_addr = 0;
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
            if (shift > 0) { M_res = (int32_t)((uint32_t)M_res << shift); E_res -= shift; }
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
            if (shift > 0) { M_res = (int64_t)((uint64_t)M_res << shift); E_res -= shift; }
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
/* mult and shift right */
static inline int64_t mult128_shift_0_63(int64_t a, int64_t b, int shift) {
    /* shift is guaranteed to be in range [0,63] */
    /* use __int128 if supported*/
    #ifdef __SIZEOF_INT128__
    int64_t res = (int64_t)(((__int128)a * (__int128)b) >> shift);
    //printf("mult128_shift_0_63: A= %ld (%012lX), B= %ld (%012lX), res = %ld (%012lX), shift= %d\n", a, a, b, b, res, res, shift);
    return res;
    #else
    uint32_t a_lo = (uint32_t)a, a_hi = (uint32_t)((uint64_t)a >> 32);
    uint32_t b_lo = (uint32_t)b, b_hi = (uint32_t)((uint64_t)b >> 32);

    uint64_t p0 = (uint64_t)a_lo * b_lo;
    uint64_t p1 = (uint64_t)a_hi * b_lo;
    uint64_t p2 = (uint64_t)a_lo * b_hi;
    uint64_t p3 = (uint64_t)a_hi * b_hi;

    uint64_t mid = (p0 >> 32) + (uint32_t)p1 + (uint32_t)p2;
    uint64_t lo = ((uint64_t)(uint32_t)mid << 32) | (uint32_t)p0;
    uint64_t hi = p3 + (p1 >> 32) + (p2 >> 32) + (mid >> 32);

    int64_t hi_s = (int64_t)hi;
    if (a < 0) hi_s -= b;
    if (b < 0) hi_s -= a;

    if (shift == 0) {
        return (int64_t)lo;
    }
    
    return (int64_t)((lo >> shift) | ((uint64_t)hi_s << (64 - shift)));
    #endif

}
/* mult and shift right */
static inline int64_t mult128_shift_64_127(int64_t a, int64_t b, int shift) {
    /* shift is guaranteed to be in range [64,127] */
    /* use __int128 if supported*/
    #ifdef __SIZEOF_INT128__
    return (int64_t)(((__int128)a * (__int128)b) >> shift);
    #else
    uint32_t a_lo = (uint32_t)a, a_hi = (uint32_t)((uint64_t)a >> 32);
    uint32_t b_lo = (uint32_t)b, b_hi = (uint32_t)((uint64_t)b >> 32);

    uint64_t p0 = (uint64_t)a_lo * b_lo;
    uint64_t p1 = (uint64_t)a_hi * b_lo;
    uint64_t p2 = (uint64_t)a_lo * b_hi;
    uint64_t p3 = (uint64_t)a_hi * b_hi;

    // We still need the carry from the lower multiplications
    uint64_t mid = (p0 >> 32) + (uint32_t)p1 + (uint32_t)p2;
    
    // Calculate the upper 64 bits (we skip assembling 'lo' entirely)
    uint64_t hi = p3 + (p1 >> 32) + (p2 >> 32) + (mid >> 32);

    // Apply signed correction
    int64_t hi_s = (int64_t)hi;
    if (a < 0) hi_s -= b;
    if (b < 0) hi_s -= a;

    // A shift of 64 maps to >> 0 on the high bits.
    // The compiler will naturally compile this down to an arithmetic right shift (sar)
    // preserving the sign bit across the result.
    return hi_s >> (shift - 64);
    #endif
}

/*
 * sqrt_n_bits:
 * Takes a 64-bit left-aligned radicand and computes 'iters' bits of the square root.
 * Since the 1750A mantissas are pre-normalized, we skip CLZ and dynamic scaling.
 */
static inline uint64_t sqrt_n_bits(uint64_t x, int iters)
{
    uint64_t a = 0ULL;   /* accumulator   */
    uint64_t r = 0ULL;   /* remainder     */
    uint64_t e = 0ULL;   /* trial product */

    for (int i = 0; i < iters; i++)
    {
        r = (r << 2) + (x >> 62); 
        x <<= 2;
        a <<= 1;
        e = (a << 1) + 1;
        if (r >= e)
        {
            r -= e;
            a++;
        }
    }
    return a;
}

static inline void invalidate_mem_cache(struct cpu_context *cpu_ctx) {
    /* we need to invalidate after XIO, and after interrupt or LST*/
    cpu_ctx->state.data_read_cache.valid = 0x0000;
    cpu_ctx->state.code_read_cache.valid = 0x0000;
    cpu_ctx->state.data_write_cache.valid = 0x0000;
    /* no need to invalidate cache for intr*/
}
/*should be called right after calculate_timers*/
static void calculate_next_scheduled_timers_check(struct cpu_context *cpu_ctx) {
    
    uint64_t time_to_timer_b_expiration_ns = 0;
    uint64_t go_timer_expiration_ns = 0;
    uint64_t nearest_time;

    go_timer_expiration_ns  = (0x10000 - cpu_ctx->state.reg.go) * 10000LL * GOTIMER_PERIOD_IN_10uSEC;
    go_timer_expiration_ns -= cpu_ctx->state.timer_ns_remainder;
    go_timer_expiration_ns = (go_timer_expiration_ns + CYCLE_DURATION_IN_NS-1) / CYCLE_DURATION_IN_NS;
    nearest_time = go_timer_expiration_ns;

    if (cpu_ctx->state.reg.sys & SYS_TA)
    {
        uint64_t time_to_timer_a_expiration_ns;
        time_to_timer_a_expiration_ns  = (0x10000 - cpu_ctx->state.reg.timer[TIM_A]) * 10000LL;
        time_to_timer_a_expiration_ns -= cpu_ctx->state.timer_ns_remainder;
        time_to_timer_a_expiration_ns = (time_to_timer_a_expiration_ns + CYCLE_DURATION_IN_NS-1) / CYCLE_DURATION_IN_NS;
        if (time_to_timer_a_expiration_ns < nearest_time)
        {
            nearest_time = time_to_timer_a_expiration_ns;
        }
    }
    if (cpu_ctx->state.reg.sys & SYS_TB)
    {
        uint64_t time_to_timer_b_expiration_ns;
        time_to_timer_b_expiration_ns  = (0x10000 - cpu_ctx->state.reg.timer[TIM_B]) * 100000LL;
        time_to_timer_b_expiration_ns -= cpu_ctx->state.timer_ns_remainder;
        time_to_timer_b_expiration_ns = (time_to_timer_b_expiration_ns + CYCLE_DURATION_IN_NS-1) / CYCLE_DURATION_IN_NS;
        if (time_to_timer_b_expiration_ns < nearest_time)
        {
            nearest_time = time_to_timer_b_expiration_ns;
        }
    }
    cpu_ctx->state.next_scheduled_timer_calc_cycles = cpu_ctx->state.total_cycles + nearest_time;

}
static void calculate_timers(struct cpu_context *cpu_ctx) {
  cpu_ctx->state.timer_ns_remainder += (cpu_ctx->state.total_cycles - cpu_ctx->state.total_cycles_timers_snap)*CYCLE_DURATION_IN_NS;
  cpu_ctx->state.total_cycles_timers_snap = cpu_ctx->state.total_cycles;
  uint32_t timer_a_inc = cpu_ctx->state.timer_ns_remainder / 10000;
  cpu_ctx->state.global_10usec_timer_clock += timer_a_inc;
  cpu_ctx->state.timer_ns_remainder = cpu_ctx->state.timer_ns_remainder % 10000;

  if (!timer_a_inc)
  {
    return;
  }

  if (cpu_ctx->state.reg.sys & SYS_TA)
  {
    if (cpu_ctx->state.reg.timer[TIM_A] <= 0xFFFF && (uint32_t)cpu_ctx->state.reg.timer[TIM_A] + timer_a_inc >= 0x10000)
    {
        cpu_ctx->state.reg.pir |= INTR_TA;
        cpu_ctx->state.reg.timer[TIM_A] += cpu_ctx->state.reg.timer_reset_val[TIM_A];
    }
    cpu_ctx->state.reg.timer[TIM_A] += timer_a_inc;
  }
  int timer_b_inc = (cpu_ctx->state.global_10usec_timer_clock - cpu_ctx->state.timer_b_global_snap) / 10;
  if (timer_b_inc)
  {
    cpu_ctx->state.timer_b_global_snap += timer_b_inc * 10;
    if (cpu_ctx->state.reg.sys & SYS_TB)
    {
        if (cpu_ctx->state.reg.timer[TIM_B] <= 0xFFFF && (uint32_t)cpu_ctx->state.reg.timer[TIM_B] + timer_b_inc >= 0x10000)
        {
            cpu_ctx->state.reg.pir |= INTR_TB;
            cpu_ctx->state.reg.timer[TIM_B] += cpu_ctx->state.reg.timer_reset_val[TIM_B];
        }
        cpu_ctx->state.reg.timer[TIM_B] += timer_b_inc;
    }
  }
  int timer_go_inc = (cpu_ctx->state.global_10usec_timer_clock - cpu_ctx->state.timer_go_global_snap) / GOTIMER_PERIOD_IN_10uSEC;
  if (timer_go_inc)
  {
    cpu_ctx->state.timer_go_global_snap += timer_go_inc * GOTIMER_PERIOD_IN_10uSEC;

    if (cpu_ctx->state.reg.go <= 0xFFFF && (uint32_t)cpu_ctx->state.reg.go + timer_go_inc >= 0x10000)
    {
        cpu_ctx->state.reg.pir |= INTR_MACHERR;   /* machine error         */
	    cpu_ctx->state.reg.ft |= FT_SYSFAULT0;    /* sysfault 0 : watchdog */
    }
    cpu_ctx->state.reg.go += timer_go_inc;
  }
}
static inline void
process_xio_0_8 (struct cpu_context *cpu_ctx, ushort xio_address, ushort *transfer)
{
/*
OYXX PO		Programmed Output:  This command outputs 16 bits of data from
		RA to a programmed I/O port.  Y may be from 0 through 3.
        */
/*
8YXX PI		Programmed Input:  This command inputs 16 bits of data into RA
		from the programmed I/O port.  Y may be from 0 through 3.
*/
}
static inline void
process_xio_1_9 (struct cpu_context *cpu_ctx, ushort xio_address, ushort *transfer)
{

}
static inline void
process_xio_2_A (struct cpu_context *cpu_ctx, ushort xio_address, ushort *transfer)
{
/*
2000 SMK	Set Interrupt Mask:  This command outputs the 16-bit contents
		of the register RA to the interrupt mask register.  A "1" in
		the corresponding bit position allows the interrupt to occur
		and a "0" prevents the interrupt from occurring except for those
		interrupts that are defined such that they cannot be masked.

2001 CLIR	Clear Interrupt Request:  All interrupts are cleared (i.e., the
		pending interrupt register is cleared to all zeros) and the contents
		of the fault register are reset to zero.

2002 ENBL	Enable Interrupts:  This command enables all interrupts which
		are not masked out.  The enable operation takes place after execution
		of the next instruction.

2003 DSBL	Disable Interrupts:  This command disables all interrupts (except
		those that are defined such that they cannot be disabled) at the
		beginning of the execution of the DSBL instruction.

2004 RPI	Reset Pending Interrupt:  The individual interrupt bit to be reset
		shall be designated in register RA as a right justified four bit
		code.  (0 (Base 16) represents interrupt number 0, F (Base 16)
		represents interrupt number 15).  If interrupt 1 (Base 16) is to
		be cleared, then the contents of the fault register shall also be
		set to zero.

2005 SPI	Set Pending Interrupt Register:  This command ORs the 16-bit 
		contents of RA with the pending interrupt register.  If there is a
		one in the corresponding bit position of the interrupt mask (same
		bit set in both the PI and the MK), and the interrupts are enabled,
		then an interrupt shall occur after execution of the next instruction.
		If PI  is set to 1, then N is assumed to be 0 (see paragraph 5.30).
		     5

200E WSW	Write Status Word:  This command transfers the contents of RA to
		the status word.

2008 OD		Output Discretes:  This command outputs the 16-bit contents of the 
		register RA to the discrete output buffer.  A "1" indicates an "on"
		condition and a "0" indicates an "off" condition.

200A RNS	Reset Normal Power Up Discrete:  This command resets the normal
		power up discrete bit.
*/
/*
A000 RMK	Read Interrupt Mask:  The current interrupt mask is transfered into
		register RA.  The interrupt mask is not altered.

A004 RPIR	Read Pending Interrupt Register:  This command transfers the contents
		of the pending interrupt register into RA.  The pending interrupt
		register is not altered.

A00E RSW 	Read Status Word:  This command transfers the 16-bit status word
		into register RA.  The status word remains unchanged.

A00F RCFR	Read and Clear Fault Register:  This command inputs the 16-bit fault
		register to register RA.  The contents of the fault register are
		reset to zero.  Bit 1 in the pending interrupt register is reset
		to zero.

A001 RIC1	Read Input/Output Interrupt Code, Level 1:  This command inputs
		the contents of the level 1 IOIC register into register RA.  The
		channel number is right justified.

A002 RIC2	Read Input/Output Interrupt Code, Level 2:  This command inputs
		the contents of the level 2 IOIC register into register RA.  The
		channel number is right justified.

A008 RDOR	Read Discrete Output Register:  This command inputs the 16-bit
		discrete output buffer into register RA.

A009 RDI	Read Discrete Input:  This command inputs the 16-bit discrete input
		word into register RA.  A "1" indicates an "on" condition and a
		"0" indicates an "off" condition.

A00B TPIO	Test Programmed Output:  This command inputs the 16-bit contents
		of the programmed output buffer into register RA.  This command
		may be used to test the PIO channel by means of a wrap around test.

A00D RMFS	Read Memory Fault Status:  This command transfers the 16-bit
		contents of the memory fault status register to RA.  The fields
		within the memory fault status register shall delineate memory
		related fault types and shall provide the page register designators
		associated with the designated fault.
*/
    switch(xio_address)
    {
        case 0x2000: /*set interrupt mask*/
            cpu_ctx->state.reg.mk = *transfer;
            break;
        case 0x2001: /*clear interrupt request*/
            cpu_ctx->state.reg.pir = 0;
            cpu_ctx->state.reg.ft = 0;
            break;
        case 0x2002: /*enable interrupts*/
            cpu_ctx->state.reg.sys_update |= SYS_INT;
            break;
        case 0x2003: /*disable interrupts*/
            cpu_ctx->state.reg.sys &= ~(SYS_INT);
            break;
        case 0x2004: /*reset pending interrupt*/            
        	cpu_ctx->state.reg.pir &= ~(0x8000 >> (*transfer & 0xF));
	        if ((*transfer & 0xF) == 1)
	            cpu_ctx->state.reg.ft = 0;
            break;
        case 0x2005: /*set pending interrupt register*/
            cpu_ctx->state.reg.pir_update |= *transfer;
            break;
        case 0x200E: /*write status word*/
            cpu_ctx->state.reg.sw = *transfer;
            break;
        case 0x2008: /*output discretes*/
            cpu_ctx->state.reg.dsctout = *transfer;
            break;
        case 0x200A: /*reset normal power up discrete*/
            cpu_ctx->state.reg.sys &= ~SYS_PWRUP;
            break;
        case 0xA000:
            *transfer = cpu_ctx->state.reg.mk;
            break;
        case 0xA004:
            *transfer = cpu_ctx->state.reg.pir;
            break;
        case 0xA00E:
            *transfer = cpu_ctx->state.reg.sw;
            break;
        case 0xA00F:
            *transfer = cpu_ctx->state.reg.ft;
            cpu_ctx->state.reg.ft = 0;
            cpu_ctx->state.reg.pir &= ~INTR_MACHERR;
            break;
        case 0xA001:
            *transfer = cpu_ctx->state.reg.ioic1;
            break;
        case 0xA002:
            *transfer = cpu_ctx->state.reg.ioic2;
            break;
        case 0xA008:
            *transfer = cpu_ctx->state.reg.dsctout;
            break;
        case 0xA009:
            *transfer = cpu_ctx->state.reg.dsctin;
            break;
        
    }
}
static inline void
process_xio_3_B (struct cpu_context *cpu_ctx, ushort xio_address, ushort *transfer)
{

}
static inline void
process_xio_4_C (struct cpu_context *cpu_ctx, ushort xio_address, ushort *transfer)
{
/*
4000 CO		Console Output:  The 16-bit contents (2 bytes) of register RA are
		output to the console.  The eight most significant bits (byte) are
		sent first.  If no console is present, then this command is treated
		as a NOP (see page 137).

4001 CLC	Clear Console:  This command clears the console interface.

4003 MPEN	Memory Protect Enable:  This command allows the memory protect
		RAM to control memory protection.

4004 ESUR	Enable Start Up ROM:  This command enables the start up ROM (i.e.,
		the ROM overlays main memory).

4005 DSUR	Disable Start Up ROM:  This command disables the start up ROM.

4006 DMAE	Direct Memory Access Enable:  This command enables direct memory
		access (DMA).

4007 DMAD	Direct Memory Access Disable:  This command disables DMA.

4008 TAS	Timer A, Start:  This command starts timer A from its current state.
		The timer is incremented every 10 microseconds.

4009 TAH	Timer A, Halt:  This command halts timer A at its current state.

400A OTA	Output Timer A:  The contents of register RA are loaded (i.e.,
		jam transfered) into timer A and the timer automatically starts
		operation by incrementing from the loaded timer in steps of ten
		microseconds.  Bit fifteen is the least significant bit and shall 
		represent ten microseconds.

400B GO		Trigger Go Indicator:  This command restarts a counter which is
		connected to a discrete output.  The period of time from restart
		to time-out shall be determined by the system requirements.  When
		the Go timer is started, the discrete output shall go high and
		remain high for
		TBD milliseconds, at which time the output shall go low unless 
		another GO is executed.  The Go discrete output signal may be 
		used as a software fault indicator.

400C TBS	Timer B, Start:  This command starts timer B from its current
		state.  The timer is incremented every 100 microseconds.

400D TBH	Timer B, Halt:  This command halts timer B at its current state.

400E OTB	Output Timer B:  The contents of register RA are loaded (i.e.,
		jam transfered) into timer B and the timer automatically starts
		operation by incrementing from the loaded timer in steps of one
		hundred microseconds.  Bit fifteen is the least significant bit
		and shall represent one hundred microseconds.
*/
/*
C000 CI		Console Input:  This command inputs the 16-bits (2 bytes) from
		the console into register RA.  The eight most significant bits
		of RA shall represent the first byte.

C001 RCS	Read Console Status:  This command inputs the console interface
		status into register RA.  The status is right justified.

C00A ITA	Input Timer A:  This command inputs the 16-bit contents of timer A
		into register RA.  Bit fifteen is the least significant bit and
		represents a time increment of ten microseconds.

C00E ITB	Input Timer B:  This command inputs the 16-bit contents of timer B
		into register RA.  Bit fifteen is the least significant bit and
		represents a time increment of one hundred microseconds.

*/

    int timer_indx =(xio_address&0x000F) >= 0x000C ? TIM_B: TIM_A;
    int sys_flag = (xio_address&0x000F) >= 0x000C ? SYS_TB: SYS_TA;
    switch(xio_address)
    {
        case 0x4000: /* CO: Console Output (Outputs 2 bytes, MSB first) */
        {
            char msb = (*transfer >> 8) & 0xFF;
            char lsb = *transfer & 0xFF;
            if (msb) putchar(msb); /* Only print if not null */
            if (lsb) putchar(lsb);
            fflush(stdout); /* Ensure it hits the screen immediately */
            break;
        }
        case 0xC000: /* CI: Console Input (Inputs 2 bytes, MSB first) */
        {
            /* Warning: Standard getchar() is blocking. For a real-time 
               emulator, you usually want to hook this into a non-blocking 
               keyboard queue. For now, we block. */
            int c1 = getchar();
            if (c1 == EOF) c1 = 0;
            
            int c2 = getchar();
            if (c2 == EOF) c2 = 0;
            
            *transfer = ((c1 & 0xFF) << 8) | (c2 & 0xFF);
            break;
        }
        case 0xC001: /* RCS: Read Console Status */
        {
            /* The OS polls this to see if it's safe to read/write.
               Bit 0 = Tx Ready (We are always ready to print to stdout)
               Bit 1 = Rx Data Available (Harder to mock without OS-specific non-blocking I/O)
               Let's tell the 1750A we are always ready to transmit, but have no input. */
            *transfer = 0x0001; /* 1 = Tx Ready, 0 = No Rx Data */
            break;
        }
        case 0x4001: /* CLC: Clear Console */
        {
            /* We don't really have a hardware buffer to clear, so NOP */
            break;
        }
        case 0x4003: /*memory protect enable*/
            cpu_ctx->state.reg.sys |= SYS_MEM_PROT;
            break;
        case 0x4004: /*enable start up rom*/
            cpu_ctx->state.reg.sys |= SYS_SUROM;
            break;
        case 0x4005: /*disable start up rom*/
            cpu_ctx->state.reg.sys &= ~SYS_SUROM;
            break;
        case 0x4006: /*direct memory access enable*/
            cpu_ctx->state.reg.sys |= SYS_DMA;
            break;
        case 0x4007: /*direct memory access disable*/
            cpu_ctx->state.reg.sys &= ~SYS_DMA;
            break;
        case 0x400B: /* go timer reset*/
            calculate_timers(cpu_ctx);
            cpu_ctx->state.reg.go = cpu_ctx->state.reg.timer_go_reset_val;
            calculate_next_scheduled_timers_check(cpu_ctx);
            break;
        case 0x4008: /*timer A start*/
        case 0x400C: /*timer B start*/
            calculate_timers(cpu_ctx);
            cpu_ctx->state.reg.sys |= sys_flag;
            calculate_next_scheduled_timers_check(cpu_ctx);
            break;
        case 0x4009: /*timer A halt*/
        case 0x400D: /*timer B halt*/
            calculate_timers(cpu_ctx);
            cpu_ctx->state.reg.sys &= ~sys_flag;
            calculate_next_scheduled_timers_check(cpu_ctx);
            break;
        case 0xC00A:
        case 0xC00E:
        case 0x400A:
        case 0x400E:
            calculate_timers(cpu_ctx);
            if (xio_address & 0x8000)
            {
                /* make sure timer values are correct */
                *transfer = cpu_ctx->state.reg.timer[timer_indx];
            }
            else
            {
                cpu_ctx->state.reg.sys |= sys_flag;
	            cpu_ctx->state.reg.timer[timer_indx] = *transfer;
                calculate_next_scheduled_timers_check(cpu_ctx);
            }
            break;
        case 0x401D:
            cpu_ctx->state.reg.timer_go_reset_val = *transfer;
            break;
    }
}
static inline void
process_xio_5_D (struct cpu_context *cpu_ctx, ushort xio_address, ushort *transfer)
{
/*
50XX LMP 	Load Memory Protect RAM (5000 + RAM address):  This command outputs
		the 16-bit contents of register RA to the memory protect RAM.
		A "1" in a bit provides write protection and a "0" in a bit permits
		writing to the corresponding 1024 word physical memory block.
		The RAM word MSB (bit 0) represents the lowest number block and
		the RAM word LSB (bit 15) represents the highest block (i.e., 
		bit 0 represents locations 0 through 1023 and bit 15 represents
		locations 15360 through 16383 for word zero).  Each word represents 
		consecutive 16K blocks of physical memory.  The RAM words of 0
		through 63 apply to processor write protect and words 64 through 
		127 apply to DMA write protect.

51XY WIPR	Write Instruction Page Register:  This command transfers the contents
		of register RA to page register Y of the instruction set group X.

52XY WOPR	Write Operand Page Register:  This command transfers the contents
		of register RA to page register Y of the operand set of group X.

*/
/*
D0XX RMP	Read Memory Protect RAM (D000 + RAM address):  This command inputs
		the appropriate memory protect word into register RA.  A "1" in
		a bit provides write protection and a "0" in a bit permits writing
		to the corresponding 1024 word physical memory block.  The RAM
		words MSB (bit 0) represents the lowest number block and the RAM
		word LSB (bit 15) represents the highest block (i.e., bit 0 
		represents locations 0 through 1023 and bit 15 represents locations 
		15360 through 16383 for word zero).  Each word represents consecutive 
		16K blocks of physical memory.  The RAM words of 0 through 63 
		apply to processor write protect and words 64 through 127 apply
		to DMA write protect.

D1XY RIPR	Read Instruction Page Register:  This command transfers the 16-bit
		contents of the page register Y of the instruction set of group X
		to register RA.

D2XY ROPR	Read Operand Page Register:  This command transfers the 16-bit contents
		of page register Y of the operand set of group X to register RA.
*/
    switch (xio_address & 0x0F00)
    {
        case 0x5000: // or 0xD000
            {
            ushort a = xio_address & 0x00FF;
            if (a > 127)
                break;
            if (xio_address&0x8000)
            {
                *transfer = cpu_ctx->state.mem_protect[a/64][a%64];
            }
            else
            {
                cpu_ctx->state.mem_protect[a/64][a%64] = *transfer;
                cpu_ctx->state.data_write_cache.valid = 0;
            }
            }
            break;
        case 0x5100: // or 0xD100
        case 0x5200: // or 0xD200
            {
                ushort bank = ((xio_address & 0x0F00) >> 8) - 1; /* CODE = 0, DATA = 1*/
                ushort grp = (xio_address & 0x00F0) >> 4;
                ushort page = xio_address &(xio_address & 0x000F);
                if (xio_address&0x8000)
                {
                    *transfer = cpu_ctx->state.pagereg[bank][grp][page].word;
                }
                else
                {
                    cpu_ctx->state.pagereg[bank][grp][page].word = *transfer;
                    if (bank == CODE)
                    {
                        cpu_ctx->state.code_read_cache.valid = 0;
                    }
                    else
                    {
                        if (grp == 0)
                            cpu_ctx->state.data_read_cache_intr.valid = 0;
                        cpu_ctx->state.data_read_cache.valid = 0;
                        cpu_ctx->state.data_write_cache.valid = 0;
                    }
                }
            }
            break;
            default:
            break;
    }

}
static inline void
process_xio_6_E (struct cpu_context *cpu_ctx, ushort xio_address, ushort *transfer)
{

}
static inline void
process_xio_7_F (struct cpu_context *cpu_ctx, ushort xio_address, ushort *transfer)
{

}
static void
process_xio (struct cpu_context *cpu_ctx, ushort xio_address, ushort *transfer)
{
    switch(xio_address & 0x7000)
    {
        case 0x0000:
            process_xio_0_8(cpu_ctx, xio_address, transfer);
            break;
        case 0x1000:
            process_xio_1_9(cpu_ctx, xio_address, transfer);
            break;
        case 0x2000:
            process_xio_2_A(cpu_ctx, xio_address, transfer);
            break;
        case 0x3000:
            process_xio_3_B(cpu_ctx, xio_address, transfer);
            break;
        case 0x4000:
            process_xio_4_C(cpu_ctx, xio_address, transfer);
            break;
        case 0x5000:
            process_xio_5_D(cpu_ctx, xio_address, transfer);
            break;
        case 0x6000:
            process_xio_6_E(cpu_ctx, xio_address, transfer);
            break;
        case 0x7000:
            process_xio_7_F(cpu_ctx, xio_address, transfer);
            break;
        default:
            break;
    }
}

static inline bool has_pending_interrupt(struct cpu_context *cpu_ctx)
{
  if (!cpu_ctx->state.reg.pir) return false;
  uint16_t unmaskable = cpu_ctx->state.reg.pir & 0x8400;
  if (unmaskable) return true;
  uint16_t maskable_undisable = (cpu_ctx->state.reg.pir & cpu_ctx->state.reg.mk) & 0x4000;
  if (maskable_undisable) return true;
  uint16_t maskable_disableable = (cpu_ctx->state.reg.sys & SYS_INT) ? (cpu_ctx->state.reg.pir & cpu_ctx->state.reg.mk & ~0xC400) : 0;
  if (maskable_disableable) return true;
  return false;

}

void process_interrupt(struct cpu_context *cpu_ctx)
{
    ushort intnum, pirmask;
    static char *intr_name[] =
        { "Power-Down", "Machine-Error", "User-0", "Floating-Overflow",
        "Integer-Overflow", "Executive-Call", "Floating-Underflow", "Timer-A",
        "User-1", "Timer-B", "User-2", "User-3",
        "IO-Level-1", "User-4", "IO-Level-2", "User-5" };


  uint16_t unmaskable = cpu_ctx->state.reg.pir & 0x8400;
  uint16_t maskable_undisable = (cpu_ctx->state.reg.pir & cpu_ctx->state.reg.mk) & 0x4000;
  uint16_t maskable_disableable = (cpu_ctx->state.reg.sys & SYS_INT) ? (cpu_ctx->state.reg.pir & cpu_ctx->state.reg.mk & ~0xC400) : 0;
  uint16_t active_interrupts = unmaskable | maskable_undisable | maskable_disableable;

  if (active_interrupts == 0)
    return;

  intnum = count_leading_zeros(active_interrupts);
  pirmask = 0x8000U >> intnum;
  cpu_ctx->state.reg.pir &= ~pirmask;
  cpu_ctx->state.reg.sys &= ~(SYS_INT);  /* clear the Master Interrupt Enable */
  struct mk_sw_ic_t{
    ushort mk, sw, ic;
  } old_mk_sw_ic = {cpu_ctx->state.reg.mk, cpu_ctx->state.reg.sw, cpu_ctx->state.reg.ic};

  struct 
  {
    /* data */
    ushort lp, svp;
  }v_entry;


  fetch_data_words_intr(cpu_ctx, 0x20 + intnum * 2, 2, (uint16_t*)&v_entry);
  struct mk_sw_ic_t new_mk_sw_ic;
  if (intnum == 5)
  {
    fetch_data_words_intr(cpu_ctx, v_entry.svp, 2, (uint16_t*)&new_mk_sw_ic);  
    fetch_data_words_intr(cpu_ctx, v_entry.svp + 2 + cpu_ctx->state.bex_index, 1, &new_mk_sw_ic.ic);
    cpu_ctx->state.bex_index = 0; /* we reset to 0, in case XIO SPI will turn this interrupt on.*/
  }
  else
  {
    fetch_data_words_intr(cpu_ctx, v_entry.svp, 3, (uint16_t*)&new_mk_sw_ic);
  }
  if ((old_mk_sw_ic.sw ^ new_mk_sw_ic.sw)& 0x00FF)
  {
    invalidate_mem_cache(cpu_ctx);
  }
  cpu_ctx->state.reg.mk = new_mk_sw_ic.mk;
  cpu_ctx->state.reg.sw = new_mk_sw_ic.sw;
  cpu_ctx->state.reg.ic = new_mk_sw_ic.ic;
  store_data_words(cpu_ctx, v_entry.lp, 3, (uint16_t*)&old_mk_sw_ic);

}
static void (*op_func_map[256])(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value);
static inline void process_instruction(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t imm_value)
{
    op_func_map[opcode>>8](cpu_ctx, opcode, imm_value);
}
static inline void apply_updates(struct cpu_state * cpu)
{
    cpu->reg.pir |= cpu->reg.pir_update;
    cpu->reg.pir_update = 0;
    cpu->reg.sys |= cpu->reg.sys_update;
    cpu->reg.sys_update = 0;
}
void print_cpu_state(struct cpu_context *cpu_ctx)
{
    printf("-------------------\n");
    uint phys_page = get_page_address_read_code(&cpu_ctx->state, cpu_ctx->state.reg.ic >> 12);
    if (phys_page != 0xFFFFFFFF)
    {
        ushort opcode = access_memory(&cpu_ctx->state, phys_page)[cpu_ctx->state.reg.ic  & 0xFFF];
        printf("opcode = %04X\n", opcode);
    }
    
    printf("ic = %04X\n", cpu_ctx->state.reg.ic);
    printf("sw = %04X\n", cpu_ctx->state.reg.sw);
    printf("mk = %04X\n", cpu_ctx->state.reg.mk);
    printf("pir = %04X\n", cpu_ctx->state.reg.pir);
    printf("sys = %04X\n", cpu_ctx->state.reg.sys);
    printf("ft = %04X\n", cpu_ctx->state.reg.ft);
    // general registers loop
    for(int i = 0; i < 16; i++)
    {
        printf("r[%d] = %04X\n", i, (ushort)cpu_ctx->state.reg.r[i]);
    }
    //printf("total cycles = %ld\n", cpu_ctx->state.total_cycles);
}
int cpu_mainloop(struct cpu_context *cpu_ctx, uint64_t up_to_cycles)
{
    uint64_t nearest_cycles_stop = up_to_cycles > cpu_ctx->state.next_scheduled_timer_calc_cycles ? cpu_ctx->state.next_scheduled_timer_calc_cycles: up_to_cycles;
    bool continue_loop = true;
    uint phys_page;
    ushort opcode;
    ushort immediate;
    while (continue_loop && cpu_ctx->state.halt == false)
    {   
        uint phys_page = get_page_address_read_code(&cpu_ctx->state, cpu_ctx->state.reg.ic >> 12);
        if (phys_page == 0xFFFFFFFF)
        {
            process_interrupt(cpu_ctx);
        }
        opcode = access_memory(&cpu_ctx->state, phys_page)[cpu_ctx->state.reg.ic  & 0xFFF];
        if (cpu_ctx->state.reg.ic & 0x0FFF == 0x0FFF)
        {
            phys_page = get_page_address_read_code(&cpu_ctx->state, (cpu_ctx->state.reg.ic + 1) >> 12);
            if (phys_page == 0xFFFFFFFF)
            {
                process_interrupt(cpu_ctx);
            }
        }
        immediate = access_memory(&cpu_ctx->state, phys_page)[(cpu_ctx->state.reg.ic + 1) & 0xFFF];
        //print_cpu_state(cpu_ctx);
        process_instruction(cpu_ctx, opcode, immediate);

        if (cpu_ctx->state.total_cycles >= nearest_cycles_stop)
        {
            if (cpu_ctx->state.total_cycles >= cpu_ctx->state.next_scheduled_timer_calc_cycles)
            {
                calculate_timers(cpu_ctx);
                calculate_next_scheduled_timers_check(cpu_ctx);
                nearest_cycles_stop = up_to_cycles > cpu_ctx->state.next_scheduled_timer_calc_cycles ? cpu_ctx->state.next_scheduled_timer_calc_cycles: up_to_cycles;            
            }
            if (cpu_ctx->state.total_cycles >= up_to_cycles)
            {
                continue_loop = false;
            }
        }
        if (has_pending_interrupt(cpu_ctx))
        {
            process_interrupt(cpu_ctx);
        }
        apply_updates(&cpu_ctx->state); // we can apply updates only after interrupts were processed

    }
    return 0;
}
void interpret_ILLEGAL(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /*imm_value*/) {
    cpu_ctx->state.reg.pir |= INTR_MACHERR;
    cpu_ctx->state.reg.ft |= FT_ILL_INSTR;
    cpu_ctx->state.halt = true;
    fprintf(stderr,"encountered illegal instruction\n");
}
extern void process_xio(struct cpu_context *cpu_ctx, ushort io_addr, ushort *transfer);

#ifdef PACE
static inline void BIF_logic(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /*imm_value*/) {
    uint16_t sub_opcode = opcode & 0x00FF;
    switch (sub_opcode & 0x00F0) {
        case 0x00:
            switch (sub_opcode & 0x000F)
            {

                case 0x03: // R3DP
                {
                    int64_t sum_product;
                    sum_product = (int32_t)cpu_ctx->state.reg.r[0] *
                            (int32_t)cpu_ctx->state.reg.r[3];
                    sum_product += (int32_t)cpu_ctx->state.reg.r[1] *
                            (int32_t)cpu_ctx->state.reg.r[4];
                    sum_product += (int32_t)cpu_ctx->state.reg.r[2] *
                            (int32_t)cpu_ctx->state.reg.r[5];
                    cpu_ctx->state.reg.accumulator += sum_product << 16;
                    /* check if we overflowed the signed 48 bit  accum*/
                    if (cpu_ctx->state.reg.accumulator > 0x7FFFFFFFFFFFLL || cpu_ctx->state.reg.accumulator < -0x800000000000LL )
                    {
                        cpu_ctx->state.reg.pir |= INTR_FIXOFL;
                    }
                    cpu_ctx->state.total_cycles += 6;
                    break;
                }
                case 0x02: // MACD multiply 32 bit (r0 msw, r1 lsw) * (r2 msw, r3 lsw) and accum
                {
                    // we need to shift r0, and r2 by 16 bit
                    int64_t sum_product;
                    sum_product = (int64_t)(((int32_t)cpu_ctx->state.reg.r[0] << 16) | (uint16_t)cpu_ctx->state.reg.r[1]) * 
                        (int64_t)(((int32_t)cpu_ctx->state.reg.r[2] << 16) | (uint16_t)cpu_ctx->state.reg.r[3]);
                    cpu_ctx->state.reg.accumulator += sum_product;
                    cpu_ctx->state.total_cycles += 8;
                    break;
                }
                
                case 0x04: // STAL (Store all 48 bits to R0, R1, R2)
                    cpu_ctx->state.reg.r[0] = (uint16_t)((cpu_ctx->state.reg.accumulator >> 32) & 0xFFFF);
                    cpu_ctx->state.reg.r[1] = (uint16_t)((cpu_ctx->state.reg.accumulator >> 16) & 0xFFFF);
                    cpu_ctx->state.reg.r[2] = (uint16_t)(cpu_ctx->state.reg.accumulator & 0xFFFF);
                    cpu_ctx->state.total_cycles += 10;
                    break;
                case 0x08:
                        // STA (Store MS 32 bits to R0, R1)
                    cpu_ctx->state.reg.r[0] = (uint16_t)((cpu_ctx->state.reg.accumulator >> 32) & 0xFFFF);
                    cpu_ctx->state.reg.r[1] = (uint16_t)((cpu_ctx->state.reg.accumulator >> 16) & 0xFFFF);
                    cpu_ctx->state.total_cycles += 7;
                    break;

                case 0x05: // LAC (Load MS 32 bits from R0, R1)
                    cpu_ctx->state.reg.accumulator = ((int64_t)(int16_t)cpu_ctx->state.reg.r[0] << 32) |
                                                ((int64_t)(uint16_t)cpu_ctx->state.reg.r[1] << 16);
                    cpu_ctx->state.total_cycles += 10;
                    break;
                case 0x07: // LACL (Load all 48 bits from R0, R1, R2)
                    cpu_ctx->state.reg.accumulator = ((int64_t)(int16_t)cpu_ctx->state.reg.r[0] << 32) |
                                                ((int64_t)(uint16_t)cpu_ctx->state.reg.r[1] << 16) |
                                                (uint16_t)cpu_ctx->state.reg.r[2];
                    cpu_ctx->state.total_cycles += 9;
                    break;
                case 0x0D: // LTAR (Load Timer A Reset Register)
                    cpu_ctx->state.reg.timer_reset_val[TIM_A] = cpu_ctx->state.reg.r[0];
                    cpu_ctx->state.total_cycles += 4;
                    break;
                case 0x0E: // LTBR (Load Timer B Reset Register)
                    cpu_ctx->state.reg.timer_reset_val[TIM_B] = cpu_ctx->state.reg.r[0];
                    cpu_ctx->state.total_cycles += 4;
                    break;
                case 0x0F: // IOMV (Fast block move memory to I/O)
                {
                    uint16_t io_addr = cpu_ctx->state.reg.r[0];
                    uint16_t count = cpu_ctx->state.reg.r[1];
                    uint16_t mem_addr = cpu_ctx->state.reg.r[2];
                    for (uint16_t i = 0; i < count; i++) {
                        int16_t val;
                        fetch_data_word(cpu_ctx, mem_addr + i, &val);
                        process_xio(cpu_ctx, io_addr + i, (uint16_t*)&val);
                    }
                    cpu_ctx->state.total_cycles += 8*(count)+22;
                    break;
                }
                case 0x06: // POLY
                {
                    uint16_t addr_An = cpu_ctx->state.reg.r[0];
                    int16_t x_val = cpu_ctx->state.reg.r[1];
                    uint16_t n = cpu_ctx->state.reg.r[2];

                    double x_frac = (double)x_val / 32768.0;
                    double result = 0.0;
                    uint16_t current_address = addr_An;

                    for (int i = n; i >= 0; i--) {
                        int16_t coef_val;
                        fetch_data_word(cpu_ctx, current_address, &coef_val);
                        double coef_frac = (double)coef_val / 32768.0;

                        result *= x_frac;
                        result += coef_frac;
                        current_address++;
                    }

                    int64_t res_int = (int64_t)(result * 8388608.0); // 2^23 for 24-bit fraction
                    cpu_ctx->state.reg.accumulator += res_int << 16;
                    cpu_ctx->state.total_cycles += 6*(n+1)+8;
                    break;
                }
                case 0x00: // CLAC
                    cpu_ctx->state.reg.accumulator = 0;
                    cpu_ctx->state.total_cycles += 4;
                    break;
                default:
                    // Illegal or unimplemented PACE BIF
                    cpu_ctx->state.reg.pir |= INTR_MACHERR;
                    cpu_ctx->state.reg.ft |= FT_ILL_INSTR;
                    cpu_ctx->state.total_cycles += 1;
                    break;
            }

            break;
        case 0x10: // VDPD
            {
            uint16_t RA = (opcode & 0x000F);
            uint16_t addr_M = cpu_ctx->state.reg.r[RA];
            uint16_t addr_N = cpu_ctx->state.reg.r[(RA + 1) & 0xF];
            uint16_t n = cpu_ctx->state.reg.r[(RA + 2) & 0xF];

            int64_t sum_product = 0;
            for (uint16_t i = 0; i < n; i++) {
                int16_t val_M[2], val_N[2];
                fetch_data_words(cpu_ctx, addr_M + i*2, 2, val_M);
                fetch_data_words(cpu_ctx, addr_N + i*2, 2, val_N);
                sum_product += (int64_t)(((int32_t)val_M[0] << 16) | (uint16_t)val_M[1]) * 
                        (int64_t)(((int32_t)val_N[0] << 16) | (uint16_t)val_N[1]);
            }

            cpu_ctx->state.reg.accumulator += sum_product;
            cpu_ctx->state.reg.r[(RA + 2) & 0xF] = 0;
            cpu_ctx->state.total_cycles += 16*(n)+10;
            break;
            }

        case 0x30: // VDPS
        {
            uint16_t RA = (opcode & 0x000F);
            uint16_t addr_M = cpu_ctx->state.reg.r[RA];
            uint16_t addr_N = cpu_ctx->state.reg.r[(RA + 1) & 0xF];
            uint16_t n = cpu_ctx->state.reg.r[(RA + 2) & 0xF];

            int64_t sum_product = 0;
            for (uint16_t i = 0; i < n; i++) {
                int16_t val_M, val_N;
                fetch_data_word(cpu_ctx, addr_M + i, &val_M);
                fetch_data_word(cpu_ctx, addr_N + i, &val_N);
                sum_product += (int64_t)val_M * (int64_t)val_N;
            }

            cpu_ctx->state.reg.accumulator += sum_product << 16; 
            if (cpu_ctx->state.reg.accumulator > 0x7FFFFFFFFFFFLL || cpu_ctx->state.reg.accumulator < -0x800000000000LL )
            {
                cpu_ctx->state.reg.pir |= INTR_FIXOFL;
            }
            cpu_ctx->state.reg.r[(RA + 2) & 0xF] = 0;
            cpu_ctx->state.total_cycles += 8*(n)+10;
            break;
        }
            
        
        
        default:
            // Illegal or unimplemented PACE BIF
            cpu_ctx->state.reg.pir |= INTR_MACHERR;
            cpu_ctx->state.reg.ft |= FT_ILL_INSTR;
            cpu_ctx->state.total_cycles += 1;
            break;
    }
}
#else
static inline void BIF_logic(struct cpu_context *cpu_ctx, uint16_t opcode, uint16_t /*imm_value*/) {
}
#endif // PACE

#endif // _CPU_HELPERS_H
