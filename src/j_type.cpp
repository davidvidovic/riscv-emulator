#include "../includes/cpu.hpp"

void CPU::exec_JAL(uint32_t instruction)
{
    this->regs[(int)(rd(instruction))] = this->pc + 4; // cast needed?
    this->pc = (uint64_t)(imm_J(instruction));
}

void CPU::exec_JALR(uint32_t instruction)
{
    this->regs[(int)(rd(instruction))] = this->pc + 4;
    this->pc = (uint64_t)(imm_J(instruction) + rs1(instruction));
}
