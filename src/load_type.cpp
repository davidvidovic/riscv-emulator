#include "../includes/cpu.hpp"

void CPU::exec_LB(uint32_t instruction)
{
    int address;
    try
    {
        address = this->regs[(int)rs1(instruction)] + imm_I(instruction);
        if(address < 0 || address >= DRAM_SIZE)
        {
            throw(LOAD_ADDRESS_OUT_OF_RANGE);
        }

        this->regs[rd(instruction)] = (int64_t)(this->cpu_load(address, 8));

    }
    catch(int error)
    {
        std::cout << "  ### EXCEPTION THROWN ###    CODE " << error << std::endl;
    }
}

void CPU::exec_LH(uint32_t instruction)
{
    int address;
    try
    {
        address = this->regs[(int)rs1(instruction)] + imm_I(instruction);
        if(address < 0 || address >= DRAM_SIZE)
        {
            throw(LOAD_ADDRESS_OUT_OF_RANGE);
        }

        this->regs[rd(instruction)] = (int64_t)(this->cpu_load(address, 16));

    }
    catch(int error)
    {
        std::cout << "  ### EXCEPTION THROWN ###    CODE " << error << std::endl;
    }
}

void CPU::exec_LW(uint32_t instruction)
{
    int address;
    try
    {
        address = this->regs[(int)rs1(instruction)] + imm_I(instruction);
        if(address < 0 || address >= DRAM_SIZE)
        {
            throw(LOAD_ADDRESS_OUT_OF_RANGE);
        }

        this->regs[rd(instruction)] = (int64_t)(this->cpu_load(address, 32));

    }
    catch(int error)
    {
        std::cout << "  ### EXCEPTION THROWN ###    CODE " << error << std::endl;
    }
}

void CPU::exec_LD(uint32_t instruction)
{
    int address;
    try
    {
        address = this->regs[(int)rs1(instruction)] + imm_I(instruction);
        if(address < 0 || address >= DRAM_SIZE)
        {
            throw(LOAD_ADDRESS_OUT_OF_RANGE);
        }

        this->regs[rd(instruction)] = (int64_t)(this->cpu_load(address, 64));

    }
    catch(int error)
    {
        std::cout << "  ### EXCEPTION THROWN ###    CODE " << error << std::endl;
    }
}
