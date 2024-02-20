#include "../includes/cpu.hpp"

// ######################################
// ############### I-TYPE ###############
// ######################################

void CPU::exec_ADDI(uint32_t instruction)
{
	int64_t imm = imm_I(instruction);
	regs[rd(instruction)] = regs[rs1(instruction)] + (int64_t)imm;
	//printf("[CPU::exec_ADDI] 		rd: 0x%x  rs1: 0x%x  rs2: 0x%x\n", rd(instruction), rs1(instruction), imm);
}

void CPU::exec_SLLI(uint32_t instruction)
{
	int64_t imm = imm_I(instruction);
	regs[rd(instruction)] = regs[rs1(instruction)] << (int64_t)imm;
	//printf("[CPU::exec_SLLI] 		rd: 0x%x  rs1: 0x%x  imm: 0x%x\n", rd(instruction), rs1(instruction), imm);
}

void CPU::exec_SLTI(uint32_t instruction)
{
	int64_t imm = imm_I(instruction);
	// This is signed instruction
	regs[rd(instruction)] = (int64_t)regs[rs1(instruction)] < imm ? (int64_t)0x1 : (int64_t)0x0;
	//printf("[CPU::exec_SLTI] 		rd: 0x%x  rs1: 0x%x  imm: 0x%x\n", rd(instruction), rs1(instruction), imm);
}

void CPU::exec_SLTIU(uint32_t instruction)
{
	uint64_t imm = imm_I(instruction);
	// This is unsigned instruction
	regs[rd(instruction)] = (uint64_t)regs[rs1(instruction)] < imm ? (int64_t)0x1 : (int64_t)0x0;
}

void CPU::exec_XORI(uint32_t instruction)
{
	int64_t imm = imm_I(instruction);
	// This is unsigned instruction
	regs[rd(instruction)] = (int64_t)regs[rs1(instruction)] ^ imm;
}

void CPU::exec_SRLI(uint32_t instruction)
{
	int64_t imm = imm_I(instruction);
	regs[rd(instruction)] = regs[rs1(instruction)] >> (uint64_t)imm;
}

void CPU::CPU::exec_SRAI(uint32_t instruction)
{
	int64_t imm = imm_I(instruction);
	int64_t msb = regs[rs1(instruction)] & 0x80000000;
	regs[rd(instruction)] = (regs[rs1(instruction)] >> (int64_t)imm) | msb;
}

void CPU::exec_ORI(uint32_t instruction)
{
	int64_t imm = imm_I(instruction);
	regs[rd(instruction)] = regs[rs1(instruction)] | (int64_t)imm;
}

void CPU::exec_ANDI(uint32_t instruction)
{
	int64_t imm = imm_I(instruction);
	regs[rd(instruction)] = regs[rs1(instruction)] & (int64_t)imm;
}
