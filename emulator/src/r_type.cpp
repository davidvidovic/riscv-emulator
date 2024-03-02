#include "../includes/cpu.hpp"

// ######################################
// ############### R-TYPE ###############
// ######################################

void CPU::exec_ADD(uint32_t instruction)
{
	regs[rd(instruction)] = regs[rs1(instruction)] + regs[rs2(instruction)];
	//printf("[exec_ADD] 		rd: 0x%x  rs1: 0x%x  rs2: 0x%x\n", rd(instruction), rs1(instruction), imm);
}


void CPU::exec_SUB(uint32_t instruction)
{
	regs[rd(instruction)] = regs[rs1(instruction)] + ((~(regs[rs2(instruction)])) + 1);
}


void CPU::exec_SLT(uint32_t instruction)
{
	regs[rd(instruction)] = (int64_t)regs[rs1(instruction)] < (int64_t)regs[rs2(instruction)] ? (uint64_t)1 : (uint64_t)0;
}


void CPU::exec_SLTU(uint32_t instruction)
{
	regs[rd(instruction)] = (uint64_t)regs[rs1(instruction)] < (uint64_t)regs[rs2(instruction)] ? (uint64_t)1 : (uint64_t)0;
}


void CPU::exec_XOR(uint32_t instruction)
{
	regs[rd(instruction)] = regs[rs1(instruction)] ^ regs[rs2(instruction)];
}


void CPU::exec_OR(uint32_t instruction)
{
	regs[rd(instruction)] = regs[rs1(instruction)] | regs[rs2(instruction)];
}


void CPU::exec_AND(uint32_t instruction)
{
	regs[rd(instruction)] = regs[rs1(instruction)] & regs[rs2(instruction)];
}
