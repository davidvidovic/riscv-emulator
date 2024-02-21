#include "../includes/cpu.hpp"

CPU::CPU()
{}

// ======================================
// CPU functions
// ======================================

void CPU::cpu_init()
{
	regs[0] = (int64_t)0;  		    // register x0 is hardwired to 0
	regs[2] = DRAM_SIZE;				// stack pointer points to the start of the stack
	pc = 0;			            // program counter points to the start of DRAM

	for(int i = 3; i < 32; i++) regs[i] = (int64_t)0;
}

uint32_t CPU::cpu_fetch()
{
	return this->bus.bus_load(pc, 32);
}

void CPU::cpu_increment_pc(int offset)
{
	this->pc += offset;
}


uint64_t CPU::cpu_load(uint64_t addr, uint64_t size)
{
	return this->bus.bus_load(addr, size);
}


void CPU::cpu_store(uint64_t addr, uint64_t size, uint64_t value)
{
	this->bus.bus_store(addr, size, value);
}


int CPU::cpu_execute(uint32_t instruction)
{
	int opcode = instruction & 0x7f;
	int funct3 = (instruction >> 12) & 0x7;
	int funct7 = (instruction >> 25) & 0x7f;
	//this->regs[0] = 0; // not necessary?

	//std::cout << "[cpu_execute] 		opcode: 0x" << std::hex << opcode << " funct3: 0x" << std::hex << funct3 << " funct7: 0x" << std::hex << funct7 << std::endl;

	switch(opcode)
	{
		case I_TYPE:
			switch(funct3)
			{
				case ADDI: 	exec_ADDI(instruction); break;
				case SLLI: 	exec_SLLI(instruction); break;
				case SLTI: 	exec_SLTI(instruction); break;
				case SLTIU: exec_SLTIU(instruction); break;
				case XORI: 	exec_XORI(instruction); break;
				case SRI:
				{
					switch(funct7)
					{
						case SRLI: exec_SRLI(instruction); break;
						case SRAI: exec_SRAI(instruction); break;
						default: break;
					}
				} break;
				case ORI: 	exec_ORI(instruction); break;
				case ANDI: 	exec_ANDI(instruction); break;
				default: break;
			}
		break;

		case R_TYPE:
			switch(funct3)
			{
				case ADD_SUB:
				{
					switch(funct7)
					{
						case ADD: exec_ADD(instruction); break;
						case SUB: exec_SUB(instruction); break;
						default: break;
					}
				} break;
				case SLT: 	exec_SLT(instruction); break;
				case SLTU: 	exec_SLTU(instruction); break;
				case XOR: 	exec_XOR(instruction); break;
				case OR: 	exec_OR(instruction); break;
				case AND: 	exec_AND(instruction); break;
			}
		break;

		case LOAD_TYPE:
			switch(funct3)
			{
				case LB:	exec_LB(instruction); break;
				case LH:	exec_LH(instruction); break;
				case LW:	exec_LW(instruction); break;
				case LD:	exec_LD(instruction); break;
			}
		break;

		case STORE_TYPE:
			switch(funct3)
			{
				case SB:	exec_SB(instruction); break;
				case SH:	exec_SH(instruction); break;
				case SW:	exec_SW(instruction); break;
				case SD:	exec_SD(instruction); break;
			}
		break;


		case B_TYPE:
			switch(funct3)
			{
				case BEQ:	exec_BEQ(instruction); break;
				case BNE:	exec_BNE(instruction); break;
				case BLT:	exec_BLT(instruction); break;
				case BGE:	exec_BGE(instruction); break;
				case BLTU:	exec_BLTU(instruction); break;
				case BGEU:	exec_BGEU(instruction); break;
			}
		break;


		default:
			std::cout << "[cpu_execute]		ERROR: Not found instruction.\n### PANIC ### opcode: 0x" << std::hex << opcode << " funct3: 0x" << std::hex << funct3 << " funct7: 0x" << std::hex << funct7 << std::endl;
			return -1;
		break;
	}

	return 0;
}


// ======================================
// Decoding functions
// ======================================

uint64_t CPU::rd(uint32_t instruction)
{
	return (instruction >> 7) & 0x1f;	// rd in bits 11..7
}

uint64_t CPU::rs1(uint32_t instruction)
{
	return (instruction >> 15) & 0x1f;	// rs1 in bits 19..15
}

uint64_t CPU::rs2(uint32_t instruction)
{
	return (instruction >> 20) & 0x1f;	// rs2 in bits 24..20
}

int16_t CPU::imm_I(uint32_t instruction)
{
	// I-type imm is in bits 31..20
	// Therfore, we are dealing with a 12 bit long number
	// Shift it right into a 16 bit number, so that the leading MSB 1 can be recognized as a negativ number
	// And return value divided by 16, or 2^4 (equal to shifting a 12 bit number 4 times to the left)
	return (signed int16_t)((signed int16_t)((instruction & 0xfff00000) >> 16)/16);
}

int16_t CPU::imm_S(uint32_t instruction)
{
	// S-type imm is in bits 31..25 | 11.7
	int imm = (signed int16_t)(((instruction & 0xfe000000) >> 20) | ((instruction >> 7) & 0x1f));
	return (signed int16_t)((signed int16_t)(imm << 4)/16);
}

int16_t CPU::imm_B(uint32_t instruction)
{
	// B-type imm is in bits 31 | 30..25 | 11..8 | 7
	int imm = (int16_t)(((instruction & 0x80000000) >> 19) | ((instruction & 0x80 << 4)) | ((instruction >> 20) & 0x7e0) | ((instruction >> 7) & 0x1e));

	// imm_B returns 16 bit integer, but only 13 LSB bits are important, 3 MSB bits are NOT important as immediate is 13 bits long
	// Those 3 MSB bits will always be zeros, hence that number is always going to be a positive 16 bit number
	// By shifting to the left 3 times the 3-bit offset is neutralized, and the number can have a leading 1 indicating negative signed numbers
	// However, since the number was shifted 3 bits to the left, it is equal to multiplaying that number but 2^3, or to say 8, hence dividing by 8
	// These kind of issue is not a problem in hardware implementation since wires can be 13 bits long. In software implementation however, it is and issue
	// Because memeory is byte-addressabile and it is not possilbe to have 13 bit integer as a data type
	return (signed int16_t)((signed int16_t)(imm << 3)/8);
}

uint64_t CPU::imm_U(uint32_t instruction)
{
	return (uint64_t)(instruction & 0xfffff999);
}

uint64_t CPU::imm_J(uint32_t instruction)
{
	return (uint64_t)((instruction & 0x80000000) >> 11) | (instruction & 0xff000) | ((instruction >> 9) & 0x800) | ((instruction >> 20) & 0x7fe);
}

/*
uint32_t shamt(uint32_t inst)
{
    return (uint32_t) (imm_I(inst) & 0x3f); // TODO: 0x1f / 0x3f ?
}
*/

// ======================================
// dump_regs
// ======================================

void CPU::dump_regs()
{
	std::cout << "\n################### Dump registers: ###################\n";
	std::cout << "ra  (x1):		0x" << std::hex << regs[1] << std::endl;
	std::cout << "sp  (x2):		0x" << std::hex << regs[2] << std::endl;
	std::cout << "gp  (x3): 		0x" << std::hex << regs[3] << std::endl;
	std::cout << "tp  (x4): 		0x" << std::hex << regs[4] << std::endl;
	std::cout << "t0  (x5):		0x" << std::hex << regs[5] << std::endl;
	std::cout << "t1  (x6):		0x" << std::hex << regs[6] << std::endl;
	std::cout << "t2  (x7):		0x" << std::hex << regs[7] << std::endl;
	std::cout << "s0  (x8):		0x" << std::hex << regs[8] << std::endl;
	std::cout << "s1  (x9):		0x" << std::hex << regs[9] << std::endl;
	std::cout << "a0  (x10):		0x" << std::hex << regs[10] << std::endl;
	std::cout << "a1  (x11):		0x" << std::hex << regs[11] << std::endl;
	std::cout << "a2  (x12):		0x" << std::hex << regs[12] << std::endl;
	std::cout << "a3  (x13):		0x" << std::hex << regs[13] << std::endl;
	std::cout << "a4  (x14):		0x" << std::hex << regs[14] << std::endl;
	std::cout << "a5  (x15):		0x" << std::hex << regs[15] << std::endl;
	std::cout << "a6  (x16):		0x" << std::hex << regs[16] << std::endl;
	std::cout << "a7  (x17):		0x" << std::hex << regs[17] << std::endl;
	std::cout << "s2  (x18):		0x" << std::hex << regs[18] << std::endl;
	std::cout << "s3  (x19):		0x" << std::hex << regs[19] << std::endl;
	std::cout << "s4  (x20):		0x" << std::hex << regs[20] << std::endl;
	std::cout << "s5  (x21):		0x" << std::hex << regs[21] << std::endl;
	std::cout << "s6  (x22):		0x" << std::hex << regs[22] << std::endl;
	std::cout << "s7  (x23):		0x" << std::hex << regs[23] << std::endl;
	std::cout << "s8  (x24):		0x" << std::hex << regs[24] << std::endl;
	std::cout << "s9  (x25):		0x" << std::hex << regs[25] << std::endl;
	std::cout << "s10 (x26):		0x" << std::hex << regs[26] << std::endl;
	std::cout << "s11 (x27):		0x" << std::hex << regs[27] << std::endl;
	std::cout << "t3  (x28):		0x" << std::hex << regs[28] << std::endl;
	std::cout << "t4  (x29):		0x" << std::hex << regs[29] << std::endl;
	std::cout << "t5  (x30):		0x" << std::hex << regs[30] << std::endl;
	std::cout << "t6  (x31):		0x" << std::hex << regs[31] << std::endl;
	std::cout << "#######################################################\n\n";
}
