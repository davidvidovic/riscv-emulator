#include "../includes/cpu.hpp"

void CPU::exec_SB(uint32_t instruction)
{
    int address;
    try
    {
        address = this->regs[(int)rs1(instruction)] + imm_S(instruction);
        if(address < 0 || address >= DRAM_SIZE)
        {
            throw(STORE_ADDRESS_OUT_OF_RANGE);
        }

        this->cpu_store(address, 8, (int64_t)(this->regs[rs2(instruction)]));

    }
    catch(int error)
    {
        std::cout << "  ### EXCEPTION THROWN ###    CODE " << error << std::endl;
    }
}

void CPU::exec_SH(uint32_t instruction)
{
    int address;
    try
    {
        address = this->regs[(int)rs1(instruction)] + imm_S(instruction);
        if(address < 0 || address >= DRAM_SIZE)
        {
            throw(STORE_ADDRESS_OUT_OF_RANGE);
        }

        this->cpu_store(address, 16, (int64_t)(this->regs[rs2(instruction)]));

    }
    catch(int error)
    {
        std::cout << "  ### EXCEPTION THROWN ###    CODE " << error << std::endl;
    }
}

void CPU::exec_SW(uint32_t instruction)
{
    int address;
    try
    {
        address = this->regs[(int)rs1(instruction)] + imm_S(instruction);
        if(address < 0 || address >= DRAM_SIZE)
        {
            throw(STORE_ADDRESS_OUT_OF_RANGE);
        }

        this->cpu_store(address, 32, (int64_t)(this->regs[rs2(instruction)]));

    }
    catch(int error)
    {
        std::cout << "  ### EXCEPTION THROWN ###    CODE " << error << std::endl;
    }
}

void CPU::exec_SD(uint32_t instruction)
{
    int address;
    try
    {
        address = this->regs[(int)rs1(instruction)] + imm_S(instruction);
        if(address < 0 || address >= DRAM_SIZE)
        {
            throw(STORE_ADDRESS_OUT_OF_RANGE);
        }

        this->cpu_store(address, 64, (int64_t)(this->regs[rs2(instruction)]));

    }
    catch(int error)
    {
        std::cout << "  ### EXCEPTION THROWN ###    CODE " << error << std::endl;
    }
}
