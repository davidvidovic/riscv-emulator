#include "../includes/cpu.h"

// ######################################
// ############### R-TYPE ###############
// ######################################

void exec_ADD(CPU* cpu, uint32_t instruction)
{
	cpu->regs[rd(instruction)] = cpu->regs[rs1(instruction)] + cpu->regs[rs2(instruction)];
	//printf("[exec_ADD] 		rd: 0x%x  rs1: 0x%x  rs2: 0x%x\n", rd(instruction), rs1(instruction), imm);
}


void exec_SUB(CPU* cpu, uint32_t instruction)
{
	cpu->regs[rd(instruction)] = cpu->regs[rs1(instruction)] + ((~(cpu->regs[rs2(instruction)])) + 1);
}


void exec_SLT(CPU* cpu, uint32_t instruction)
{
	cpu->regs[rd(instruction)] = (int64_t)cpu->regs[rs1(instruction)] < (int64_t)cpu->regs[rs2(instruction)] ? (uint64_t)1 : (uint64_t)0;
}


void exec_SLTU(CPU* cpu, uint32_t instruction)
{
	cpu->regs[rd(instruction)] = (uint64_t)cpu->regs[rs1(instruction)] < (uint64_t)cpu->regs[rs2(instruction)] ? (uint64_t)1 : (uint64_t)0;
}


void exec_XOR(CPU* cpu, uint32_t instruction)
{
	cpu->regs[rd(instruction)] = cpu->regs[rs1(instruction)] ^ cpu->regs[rs2(instruction)];
}


void exec_OR(CPU* cpu, uint32_t instruction)
{
	cpu->regs[rd(instruction)] = cpu->regs[rs1(instruction)] | cpu->regs[rs2(instruction)];
}


void exec_AND(CPU* cpu, uint32_t instruction)
{
	cpu->regs[rd(instruction)] = cpu->regs[rs1(instruction)] & cpu->regs[rs2(instruction)];
}
