#include <iostream>
#include <fstream>
#include <string>

#include "includes/cpu.hpp"

void read_input_file(CPU &cpu)
{
    std::ifstream input_file;
    input_file.open("binary_input.bin");

    std::string line;
    int offset = 0;

    if(input_file.is_open())
    {
        while(input_file)
        {
            std::getline(input_file, line);
            cpu.cpu_store(offset, 32, (uint64_t)(strtol(line.c_str(), NULL, 2)));
            offset += 4;
        }
    }
    else
    {
        std::cout << "ERROR: Could not open binary input file." << std::endl;
    }

}

int main(int argv, char* argc)
{
    CPU cpu;
    read_input_file(cpu);
    cpu.cpu_init();
    uint32_t instruction;

    while(true)
    {
        instruction = cpu.cpu_fetch();
        cpu.cpu_increment_pc(4);

        if(cpu.cpu_execute(instruction) != 0) break;
    }

    cpu.dump_regs();

    return 0;
}
