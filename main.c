#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "includes/cpu.h"

int main()
{
	struct CPU cpu;
	cpu_init(&cpu);
	uint64_t instruction;
	
	// instruction = 00000000000100100000001000010011; = 1180179 = addi x4, x4, 1
	instruction = 1180179;
	
	cpu_store(&cpu, (uint64_t)0x80000001, (uint64_t)64, (uint64_t)instruction);
	printf("Instruction is 0x%x\n", cpu_load(&cpu, (uint64_t)0x80000001, (uint64_t)64));
	printf("rd is 0x%x\n", rd((uint32_t)instruction));
	printf("rs1 is 0x%x\n", rs1((uint32_t)instruction));
	printf("rs2 is 0x%x\n", rs2((uint32_t)instruction));
	return 0;
}
