#include <stdio.h>
#include <stdint.h>

#include "bus.h"

typedef struct CPU {
	uint64_t regs[32];	// 32 general purpose 64-bit registers
	uint64_t pc;		// 64-bit program counter
	struct BUS bus;		// BUS connected to the CPU
} CPU;

// Function to initialize CPU's registers
void cpu_init(CPU* cpu);

// Function to fetch next instruction from DRAM
uint32_t cpu_fetch(CPU* cpu);

// Function to execute fetched instruction
int cpu_execute(CPU* cpu, uint32_t instruction);

// Debug function to print state of all the regs when error accures
void dump_regs(CPU* cpu);
