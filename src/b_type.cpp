#include "../includes/cpu.hpp"


void CPU::exec_BEQ(uint32_t instruction)
{
    if((signed int64_t)(this->regs[rs1(instruction)]) == (signed int64_t)(this->regs[rs2(instruction)]))
    {
        // Minus 4 to compensate for already incremented PC in main
        // this->pc -= 4;
        //std::cout << "PC: "<< this->pc << " and offset is " << ((signed int16_t)((imm_B(instruction) << 3)))/8 << std::endl;

        //this->cpu_increment_pc((signed int16_t)(imm_B(instruction) << 3)/8 - 4);
        this->cpu_increment_pc(imm_B(instruction) - 4);
    }
}

void CPU::exec_BNE(uint32_t instruction)
{
    if((signed int64_t)(this->regs[rs1(instruction)]) != (signed int64_t)(this->regs[rs2(instruction)]))
    {
        this->cpu_increment_pc(imm_B(instruction) - 4);
    }
}

void CPU::exec_BLT(uint32_t instruction)
{
    if((signed int64_t)(this->regs[rs1(instruction)]) < (signed int64_t)(this->regs[rs2(instruction)]))
    {
        this->cpu_increment_pc(imm_B(instruction) - 4);
    }
}

void CPU::exec_BGE(uint32_t instruction)
{
    if((signed int64_t)(this->regs[rs1(instruction)]) >= (signed int64_t)(this->regs[rs2(instruction)]))
    {
        this->cpu_increment_pc(imm_B(instruction) - 4);
    }
}

void CPU::exec_BLTU(uint32_t instruction)
{
    if((uint64_t)(this->regs[rs1(instruction)]) < (uint64_t)(this->regs[rs2(instruction)]))
    {
        this->cpu_increment_pc(imm_B(instruction) - 4);
    }
}

void CPU::exec_BGEU(uint32_t instruction)
{
    if((uint64_t)(this->regs[rs1(instruction)]) >= (uint64_t)(this->regs[rs2(instruction)]))
    {
        this->cpu_increment_pc(imm_B(instruction) - 4);
    }
}
