#include <stdio.h>
#include <stdint.h>
#include <cstdint>

#define DRAM_SIZE	1024*1024*1
#define DRAM_BASE	0x80000000

typedef struct DRAM {
	uint8_t mem[DRAM_SIZE];
} DRAM;

uint64_t dram_load(DRAM* dram, uint64_t addr, uint64_t size);
void dram_store(DRAM* dram, uint64_t addr, uint64_t size, uint64_t value);
