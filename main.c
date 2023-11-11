#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "includes/dram.h"

int main()
{
	struct DRAM dram;
	dram_store(&dram, (uint64_t)0x80000002, (uint64_t)8, (uint64_t)1);
	printf("%lu\n", dram_load(&dram, (uint64_t)0x80000002, (uint64_t)8));
	return 0;
}
