#include <iostream>

#include "includes/cpu.hpp"

int main(int argv, char* argc)
{
    CPU cpu;
    cpu.cpu_init();
    cpu.dump_regs();
    return 0;
}