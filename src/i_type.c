#include "../includes/cpu.h"

// ######################################
// ############### I-TYPE ###############
// ######################################

void exec_ADDI(CPU* cpu, uint32_t instruction)
{
	uint64_t imm = imm_I(instruction);
	cpu->regs[rd(instruction)] = cpu->regs[rs1(instruction)] + (int64_t)imm;
	//printf("[exec_ADDI] 		rd: 0x%x  rs1: 0x%x  rs2: 0x%x\n", rd(instruction), rs1(instruction), imm);
}

void exec_SLLI(CPU* cpu, uint32_t instruction)
{
	uint64_t imm = imm_I(instruction);
	cpu->regs[rd(instruction)] = cpu->regs[rs1(instruction)] << (uint64_t)imm;
	//printf("[exec_SLLI] 		rd: 0x%x  rs1: 0x%x  imm: 0x%x\n", rd(instruction), rs1(instruction), imm);
}

void exec_SLTI(CPU* cpu, uint32_t instruction)
{
	int64_t imm = imm_I(instruction);
	// This is signed instruction
	cpu->regs[rd(instruction)] = (int64_t)cpu->regs[rs1(instruction)] < imm ? (uint64_t)0x1 : (uint64_t)0x0;
	//printf("[exec_SLTI] 		rd: 0x%x  rs1: 0x%x  imm: 0x%x\n", rd(instruction), rs1(instruction), imm);
}

void exec_SLTIU(CPU* cpu, uint32_t instruction)
{
	uint64_t imm = imm_I(instruction);
	// This is unsigned instruction
	cpu->regs[rd(instruction)] = (uint64_t)cpu->regs[rs1(instruction)] < imm ? (uint64_t)0x1 : (uint64_t)0x0;
}

void exec_XORI(CPU* cpu, uint32_t instruction)
{
	int64_t imm = imm_I(instruction);
	// This is unsigned instruction
	cpu->regs[rd(instruction)] = (int64_t)cpu->regs[rs1(instruction)] ^ imm;
}

void exec_SRLI(CPU* cpu, uint32_t instruction)
{
	uint64_t imm = imm_I(instruction);
	cpu->regs[rd(instruction)] = cpu->regs[rs1(instruction)] >> (uint64_t)imm;
}

void exec_SRAI(CPU* cpu, uint32_t instruction)
{
	uint64_t imm = imm_I(instruction);
	uint64_t msb = cpu->regs[rs1(instruction)] & 0x80000000;
	cpu->regs[rd(instruction)] = (cpu->regs[rs1(instruction)] >> (uint64_t)imm) | msb;
}

void exec_ORI(CPU* cpu, uint32_t instruction)
{
	uint64_t imm = imm_I(instruction);
	cpu->regs[rd(instruction)] = cpu->regs[rs1(instruction)] | (uint64_t)imm;
}

void exec_ANDI(CPU* cpu, uint32_t instruction)
{
	uint64_t imm = imm_I(instruction);
	cpu->regs[rd(instruction)] = cpu->regs[rs1(instruction)] & (uint64_t)imm;
}
