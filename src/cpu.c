#include "../includes/cpu.h"

void cpu_init(CPU* cpu)
{
	cpu->regs[0] = 0x00;			// register x0 is hardwired to 0
	cpu->regs[2] = DRAM_BASE + DRAM_SIZE;	// stack pointer points to the start of the stack
	cpu->pc = DRAM_BASE;			// program counter points to the start of DRAM
}

uint32_t cpu_fetch(CPU* cpu)
{
	return bus_load(&(cpu->bus), cpu->pc, 32);
}

uint64_t cpu_load(CPU* cpu, uint64_t addr, uint64_t size)
{
	return bus_load(&(cpu->bus), addr, size);
}

void cpu_store(CPU* cpu, uint64_t addr, uint64_t size, uint64_t value)
{
	bus_store(&(cpu->bus), addr, size, value);
}


