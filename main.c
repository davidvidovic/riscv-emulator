#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "includes/cpu.h"

int main()
{
	struct CPU cpu;
	cpu_init(&cpu);
	uint32_t instruction;
	
	// instruction = 00000000000100100000001000010011; = 1180179 = addi x4, x4, 1
	instruction = 1180179;
	
	
	cpu_execute(&cpu, instruction);
	printf("After execution: 	regs[%d] = 0x%x\n", rd(instruction), (uint32_t)(cpu.regs[rd(instruction)]));
	return 0;
}
