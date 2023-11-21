#include "../includes/cpu.h"

// =======================================
// CPU functions
// =======================================

void cpu_init(CPU* cpu)
{
	cpu->regs[0] = 0x00;			// register x0 is hardwired to 0
	cpu->regs[2] = DRAM_BASE + DRAM_SIZE;	// stack pointer points to the start of the stack
	cpu->pc = DRAM_BASE;			// program counter points to the start of DRAM
	
	cpu->regs[0] = 0x00;
	for(int i = 3; i < 32; i++) cpu->regs[i] = (uint64_t)0;
}

uint32_t cpu_fetch(CPU* cpu)
{
	return bus_load(&(cpu->bus), cpu->pc, 32);
}

uint64_t cpu_load(CPU* cpu, uint64_t addr, uint64_t size)
{
	return bus_load(&(cpu->bus), addr, size);
}

void cpu_store(CPU* cpu, uint64_t addr, uint64_t size, uint64_t value)
{
	bus_store(&(cpu->bus), addr, size, value);
}

int cpu_execute(CPU* cpu, uint32_t instruction)
{
	
	int opcode = instruction & 0x7f;
	int funct3 = (instruction >> 12) & 0x7;
	int funct7 = (instruction >> 25) & 0x7f;
	cpu->regs[0] = 0;
	
	printf("[cpu_execute] 		opcode: 0x%x  funct3: 0x%x  funct7: 0x%x\n", opcode, funct3, funct7);

	switch(opcode)
	{
		case I_TYPE:
			switch(funct3)
			{
				case ADDI: exec_ADDI(cpu, instruction); break;
				case SLLI: exec_SLLI(cpu, instruction); break;
				case SLTI: exec_SLTI(cpu, instruction); break;
				case SLTIU: exec_SLTIU(cpu, instruction); break;
				case XORI: exec_XORI(cpu, instruction); break;
				case SRI: 
				{
					switch(funct7)
					{
						case SRLI: exec_SRLI(cpu, instruction); break;
						case SRAI: exec_SRAI(cpu, instruction); break;
						default: break;
					}
				} break;
				case ORI: exec_ORI(cpu, instruction); break;
				case ANDI: exec_ANDI(cpu, instruction); break;
				default: break;
			}
		break;
		
		default: 
			printf("[cpu_execute]		ERROR: Not found instruction.\n### PANIC ### opcode: 0x%x  funct3: 0x%x  funct7: 0x%x\n", opcode, funct3, funct7);
			return -1;
		break;
	}
	
	return 0;
}

// =======================================
// Decoding functions
// =======================================

uint64_t rd(uint32_t instruction) 
{
	return (instruction >> 7) & 0x1f;	// rd in bits 11..7
}

uint64_t rs1(uint32_t instruction) 
{
	return (instruction >> 15) & 0x1f;	// rs1 in bits 19..15
}

uint64_t rs2(uint32_t instruction)
{
	return (instruction >> 20) & 0x1f;	// rs2 in bits 24..20
}

uint64_t imm_I(uint32_t instruction)
{
	// I-type imm is in bits 31..20
	return (uint64_t)((instruction & 0xfff00000) >> 20);
}

uint64_t imm_S(uint32_t instruction)
{
	// S-type imm is in bits 31..25 | 11.7
	return (uint64_t)(((instruction & 0xfe000000) >> 20) | ((instruction >> 7) & 0x1f));
}

uint64_t imm_B(uint32_t instruction)
{
	// B-type imm is in bits 31 | 30..25 | 11..8 | 7
	return (uint64_t)(((instruction & 0x80000000) >> 19) | ((instruction & 0x80 << 4)) | ((instruction >> 20) & 0x7e0) | ((instruction >> 7) & 0x1e));
}

uint64_t imm_U(uint32_t instruction)
{
	return (uint64_t)(instruction & 0xfffff999);
}

uint64_t imm_J(uint32_t inst) 
{
	return (uint64_t)((inst & 0x80000000) >> 11) | (inst & 0xff000) | ((inst >> 9) & 0x800) | ((inst >> 20) & 0x7fe);
}

uint32_t shamt(uint32_t inst) 
{
    	return (uint32_t) (imm_I(inst) & 0x1f); // TODO: 0x1f / 0x3f ?
}

// =======================================
// Executing functions
// =======================================

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


// =======================================
// Executing functions
// =======================================

void dump_regs(CPU* cpu)
{
	printf("\n################### Dump registers: ###################\n");
	printf("ra  (x1):	0x%x\n", cpu->regs[1]);
	printf("sp  (x2):	0x%x\n", cpu->regs[2]);
	printf("gp  (x3):	0x%x\n", cpu->regs[3]);
	printf("tp  (x4):	0x%x\n", cpu->regs[4]);
	printf("t0  (x5):	0x%x\n", cpu->regs[5]);
	printf("t1  (x6):	0x%x\n", cpu->regs[6]);
	printf("t2  (x7):	0x%x\n", cpu->regs[7]);
	printf("s0  (x8):	0x%x\n", cpu->regs[8]);
	printf("s1  (x9):	0x%x\n", cpu->regs[9]);
	printf("a0  (x10):	0x%x\n", cpu->regs[10]);
	printf("a1  (x11):	0x%x\n", cpu->regs[11]);
	printf("a2  (x12):	0x%x\n", cpu->regs[12]);
	printf("a3  (x13):	0x%x\n", cpu->regs[13]);
	printf("a4  (x14):	0x%x\n", cpu->regs[14]);
	printf("a5  (x15):	0x%x\n", cpu->regs[15]);
	printf("a6  (x16):	0x%x\n", cpu->regs[16]);
	printf("a7  (x17):	0x%x\n", cpu->regs[17]);
	printf("s2  (x18):	0x%x\n", cpu->regs[18]);
	printf("s3  (x19):	0x%x\n", cpu->regs[19]);
	printf("s4  (x20):	0x%x\n", cpu->regs[20]);
	printf("s5  (x21):	0x%x\n", cpu->regs[21]);
	printf("s6  (x22):	0x%x\n", cpu->regs[22]);
	printf("s7  (x23):	0x%x\n", cpu->regs[23]);
	printf("s8  (x24):	0x%x\n", cpu->regs[24]);
	printf("s9  (x25):	0x%x\n", cpu->regs[25]);
	printf("s10 (x26):	0x%x\n", cpu->regs[26]);
	printf("s11 (x27):	0x%x\n", cpu->regs[27]);
	printf("t3  (x28):	0x%x\n", cpu->regs[28]);
	printf("t4  (x29):	0x%x\n", cpu->regs[29]);
	printf("t5  (x30):	0x%x\n", cpu->regs[30]);
	printf("t6  (x31):	0x%x\n", cpu->regs[31]);	
	printf("#######################################################\n\n");
}
