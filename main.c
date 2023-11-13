#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "includes/cpu.h"

int main()
{
	struct CPU cpu;
	cpu_init(&cpu);

	cpu_store(&cpu, (uint64_t)0x80000002, (uint64_t)8, (uint64_t)1);
	printf("%lu\n", cpu_load(&cpu, (uint64_t)0x80000002, (uint64_t)8));
	return 0;
}
