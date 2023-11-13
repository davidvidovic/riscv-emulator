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

uint64_t rd(uint32_t instruction) 
{
	return (instruction >> 7) & 0x1f;	// rd in bits 11..7
}

uint64_t rs1(uint32_t instruction) 
{
	return (instruction >> 15) & 0x1f;	// rs1 in bits 19..15
}

uint64_t rs2(uint32_t instruction)
{
	return (instruction >> 20) & 0x1f;	// rs2 in bits 24..20
}

uint64_t imm_I(uint32_t instruction)
{
	// I-type imm is in bits 31..20
	return (uint64_t)((instruction & 0xfff00000) >> 20);
}

uint64_t imm_S(uint32_t instruction)
{
	// S-type imm is in bits 31..25 | 11.7
	return (uint64_t)(((instruction & 0xfe000000) >> 20) | ((instruction >> 7) & 0x1f));
}

uint64_t imm_B(uint32_t instruction)
{
	// B-type imm is in bits 31 | 30..25 | 11..8 | 7
	return (uint64_t)(((instruction & 0x80000000) >> 19) | ((instruction & 0x80 << 4)) | ((instruction >> 20) & 0x7e0) | ((instruction >> 7) & 0x1e));
}

uint64_t imm_U(uint32_t instruction)
{
	
}
