#include "../includes/bus.hpp"

BUS::BUS()
{}

uint64_t BUS::bus_load(uint64_t addr, uint64_t size)
{
	return dram.dram_load(addr, size);
}

void BUS::bus_store(uint64_t addr, uint64_t size, uint64_t value)
{
	dram.dram_store(addr, size, value);
}

void BUS::stack_dump(int print_size)
{
	dram.stack_dump(print_size);
}