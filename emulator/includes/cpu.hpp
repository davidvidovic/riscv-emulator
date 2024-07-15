#pragma once

#include <iostream>
#include <fstream>
#include <iomanip>
#include "bus.hpp"
#include "opcodes.hpp"
#include "error_codes.hpp"

class CPU {
    private:
        int64_t regs[32];	// 32 general purpose 64-bit registers
        uint64_t pc;		  // 64-bit program counter
        BUS bus;		      // BUS connected to the CPU

        // Decoding functions
        uint16_t rd(uint32_t);
        uint16_t rs1(uint32_t);
        uint16_t rs2(uint32_t);
        int16_t imm_I(uint32_t);
        int16_t imm_S(uint32_t);
        int16_t imm_B(uint32_t);
        uint64_t imm_U(uint32_t);
        uint32_t imm_J(uint32_t);

        // I-type instructions
        void exec_ADDI(uint32_t);
        void exec_SLLI(uint32_t);
        void exec_SLTI(uint32_t);
        void exec_SLTIU(uint32_t);
        void exec_XORI(uint32_t);
        void exec_SRLI(uint32_t);
        void exec_SRAI(uint32_t);
        void exec_ORI(uint32_t);
        void exec_ANDI(uint32_t);

        // Load instructions are under I-type
        void exec_LB(uint32_t);
        void exec_LH(uint32_t);
        void exec_LW(uint32_t);
        void exec_LD(uint32_t);

        // S-type instructions
        void exec_SB(uint32_t);
        void exec_SH(uint32_t);
        void exec_SW(uint32_t);
        void exec_SD(uint32_t);

        // R-type instructions
        void exec_ADD(uint32_t);
        void exec_SUB(uint32_t);
        void exec_SLT(uint32_t);
        void exec_SLTU(uint32_t);
        void exec_XOR(uint32_t);
        void exec_OR(uint32_t);
        void exec_AND(uint32_t);

        // B-type instructions
        void exec_BEQ(uint32_t);
        void exec_BNE(uint32_t);
        void exec_BLT(uint32_t);
        void exec_BGE(uint32_t);
        void exec_BLTU(uint32_t);
        void exec_BGEU(uint32_t);

        // J-type instructions
        void exec_JAL(uint32_t);
        void exec_JALR(uint32_t);



    public:
        CPU();

        // Function to initialize CPU's registers
        void cpu_init();

        // Function to fetch next instruction from DRAM
        uint32_t cpu_fetch();

        // FUnction to increment program counter by offset
        void cpu_increment_pc(int);

        // Function to execute fetched instruction
        int cpu_execute(uint32_t);

        // Debug function to print state of all the regs when error accures
        void dump_regs();

        // Function to communicate through BUS with DRAM
        uint64_t cpu_load(uint64_t, uint64_t);
        void cpu_store(uint64_t, uint64_t, uint64_t);
        void stack_dump(int);
};
