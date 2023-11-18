#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "includes/cpu.h"

void read_input_file(CPU* cpu)
{
	char file_line[32];
	int offset = 0;
	FILE* binary_input;
	
	binary_input = fopen("binary_input.txt", "r");
	if(binary_input == NULL) printf("ERROR opening binary input file!\n");
	
	while(fgets(file_line, 34, binary_input))
	{
		bus_store(&(cpu->bus), DRAM_BASE+offset, 32, (uint64_t)(strtol(file_line, NULL, 2)));
		offset += 4;
	}
	
	fclose(binary_input);
}

int main(int argv, char* argc)
{
	struct CPU cpu;
	cpu_init(&cpu);
	read_input_file(&cpu);
	uint32_t instruction;
	
	while(1)
	{
		instruction = cpu_fetch(&cpu);
		cpu.pc += 4;
		
		if(cpu_execute(&cpu, instruction) != 0) break;
		if(cpu.pc == 0) break;	
	}
	
	dump_regs(&cpu);
	
	return 0;
}
