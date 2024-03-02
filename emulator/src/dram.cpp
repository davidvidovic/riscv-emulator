#include "../includes/dram.hpp"

DRAM::DRAM()
{}

void DRAM::dram_store(uint64_t addr, uint64_t size, uint64_t value)
{
	switch(size) {
	case 8:  dram_store_8(addr, value);	    break;
	case 16: dram_store_16(addr, value); 	break;
	case 32: dram_store_32(addr, value);	break;
	case 64: dram_store_64(addr, value);	break;
	default: std::cout << "[dram_store] Wrong size.\n";	break;
	}
}

uint64_t DRAM::dram_load(uint64_t addr, uint64_t size)
{
	switch(size) {
	case 8:  return dram_load_8(addr);	    break;
	case 16: return dram_load_16(addr); 	break;
	case 32: return dram_load_32(addr);	    break;
	case 64: return dram_load_64(addr);	    break;
	default:
		{
			std::cout << "[dram_load] Wrong size.\n";
			return 0;
		}	break;
	}
}

void DRAM::dram_store_8(uint64_t addr, uint64_t value)
{
	mem[addr] = (uint8_t)(value & 0xff);
}

void DRAM::dram_store_16(uint64_t addr, uint64_t value)
{
	mem[addr]   = (uint8_t)(value & 0xff);
	mem[addr+1] = (uint8_t)((value >> 8) & 0xff);
}

void DRAM::dram_store_32(uint64_t addr, uint64_t value)
{
	mem[addr]   = (uint8_t)(value & 0xff);
	mem[addr+1] = (uint8_t)((value >> 8)  & 0xff);
	mem[addr+2] = (uint8_t)((value >> 16) & 0xff);
	mem[addr+3] = (uint8_t)((value >> 24) & 0xff);
}

void DRAM::dram_store_64(uint64_t addr, uint64_t value)
{
	mem[addr]   = (uint8_t)(value & 0xff);
	mem[addr+1] = (uint8_t)((value >> 8)  & 0xff);
	mem[addr+2] = (uint8_t)((value >> 16) & 0xff);
	mem[addr+3] = (uint8_t)((value >> 24) & 0xff);
	mem[addr+4] = (uint8_t)((value >> 32) & 0xff);
	mem[addr+5] = (uint8_t)((value >> 40) & 0xff);
	mem[addr+6] = (uint8_t)((value >> 48) & 0xff);
	mem[addr+7] = (uint8_t)((value >> 56) & 0xff);
}

uint64_t DRAM::dram_load_8(uint64_t addr)
{
	return (uint64_t) mem[addr];
}

uint64_t DRAM::dram_load_16(uint64_t addr)
{
	return (uint64_t) 	((uint64_t)mem[addr] 			|
				((uint64_t)mem[addr+1] << 8));
}

uint64_t DRAM::dram_load_32(uint64_t addr)
{
	return (uint64_t) 	((uint64_t)mem[addr] 			|
				((uint64_t)mem[addr+1] << 8) 	|
				((uint64_t)mem[addr+2] << 16) 	|
				((uint64_t)mem[addr+3] << 24));
}

uint64_t DRAM::dram_load_64(uint64_t addr)
{
	return (uint64_t) 	((uint64_t)mem[addr] 			|
				((uint64_t)mem[addr+1] << 8) 	|
				((uint64_t)mem[addr+2] << 16) 	|
				((uint64_t)mem[addr+3] << 24) 	|
				((uint64_t)mem[addr+4] << 32) 	|
				((uint64_t)mem[addr+5] << 40) 	|
				((uint64_t)mem[addr+6] << 48) 	|
				((uint64_t)mem[addr+7] << 56));
}
