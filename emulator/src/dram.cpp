#include "../includes/dram.hpp"

DRAM::DRAM()
{
	for(int i = 0; i < DRAM_SIZE; i++)
		mem[i] = 0;
}

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
	mem[addr] = (value & 0xff);
}

void DRAM::dram_store_16(uint64_t addr, uint64_t value)
{
	mem[addr]   = (value & 0xff);
	mem[addr+1] = ((value >> 8) & 0xff);
}

void DRAM::dram_store_32(uint64_t addr, uint64_t value)
{
	mem[addr]   = (value & 0xff); 
	mem[addr+1] = ((value >> 8)  & 0xff); 
	mem[addr+2] = ((value >> 16) & 0xff); 
	mem[addr+3] = ((value >> 24) & 0xff); 
}

void DRAM::dram_store_64(uint64_t addr, uint64_t value)
{
	mem[addr]   = (value & 0xff);
	mem[addr+1] = ((value >> 8)  & 0xff);
	mem[addr+2] = ((value >> 16) & 0xff);
	mem[addr+3] = ((value >> 24) & 0xff);
	mem[addr+4] = ((value >> 32) & 0xff);
	mem[addr+5] = ((value >> 40) & 0xff);
	mem[addr+6] = ((value >> 48) & 0xff);
	mem[addr+7] = ((value >> 56) & 0xff);
}

uint64_t DRAM::dram_load_8(uint64_t addr)
{
	return  mem[addr];
}

uint64_t DRAM::dram_load_16(uint64_t addr)
{
	return  	(mem[addr] 			|
				(mem[addr+1] << 8));
}

uint64_t DRAM::dram_load_32(uint64_t addr)
{
	return  	(mem[addr] 			|
				(mem[addr+1] << 8) 	|
				(mem[addr+2] << 16) 	|
				(mem[addr+3] << 24));
}

uint64_t DRAM::dram_load_64(uint64_t addr)
{
	return  	(mem[addr] 			|
				(mem[addr+1] << 8) 	|
				(mem[addr+2] << 16) 	|
				(mem[addr+3] << 24) 	|
				(mem[addr+4] << 32) 	|
				(mem[addr+5] << 40) 	|
				(mem[addr+6] << 48) 	|
				(mem[addr+7] << 56));
}

void DRAM::stack_dump(int print_size)
{
	std::ofstream stack_dump_file("stack_dump.txt");

	for(long long i = DRAM_SIZE-1; i > DRAM_SIZE-print_size-1; i--)
	{
		stack_dump_file << std::hex << mem[i] << std::endl;
	}

	stack_dump_file.close();
}