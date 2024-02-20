#pragma once

#include <iostream>
#include "bus.hpp"
#include "opcodes.hpp"

class CPU {
    private:
        int64_t regs[32];	// 32 general purpose 64-bit registers
        uint64_t pc;		  // 64-bit program counter
        BUS bus;		      // BUS connected to the CPU

        // Decoding functions
        uint64_t rd(uint32_t instruction);
        uint64_t rs1(uint32_t instruction);
        uint64_t rs2(uint32_t instruction);
        uint64_t imm_I(uint32_t instruction);
        uint64_t imm_S(uint32_t instruction);
        uint64_t imm_B(uint32_t instruction);
        uint64_t imm_U(uint32_t instruction);
        uint64_t imm_J(uint32_t instruction);

        // I-type instructions
        void exec_ADDI(uint32_t instruction);
        void exec_SLLI(uint32_t instruction);
        void exec_SLTI(uint32_t instruction);
        void exec_SLTIU(uint32_t instruction);
        void exec_XORI(uint32_t instruction);
        void exec_SRLI(uint32_t instruction);
        void exec_SRAI(uint32_t instruction);
        void exec_ORI(uint32_t instruction);
        void exec_ANDI(uint32_t instruction);

        // R-type instructions
        void exec_ADD(uint32_t instruction);
        void exec_SUB(uint32_t instruction);
        void exec_SLT(uint32_t instruction);
        void exec_SLTU(uint32_t instruction);
        void exec_XOR(uint32_t instruction);
        void exec_OR(uint32_t instruction);
        void exec_AND(uint32_t instruction);

    public:
        CPU();

        // Function to initialize CPU's registers
        void cpu_init();

        // Function to fetch next instruction from DRAM
        uint32_t cpu_fetch();

        // FUnction to increment program counter by offset
        void cpu_increment_pc(int);

        // Function to execute fetched instruction
        int cpu_execute(uint32_t instruction);

        // Debug function to print state of all the regs when error accures
        void dump_regs();

        uint64_t cpu_load(uint64_t addr, uint64_t size);
        void cpu_store(uint64_t addr, uint64_t size, uint64_t value);


};
