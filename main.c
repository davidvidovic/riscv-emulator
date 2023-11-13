#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "includes/bus.h"

int main()
{
	struct BUS bus;
	bus_store(&bus, (uint64_t)0x80000002, (uint64_t)8, (uint64_t)1);
	printf("%lu\n", bus_load(&bus, (uint64_t)0x80000002, (uint64_t)8));
	return 0;
}
