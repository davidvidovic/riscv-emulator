#pragma once

#include <iostream>
#include "bus.hpp"
#include "opcodes.h"

class CPU {
    private:
        int64_t regs[32];	// 32 general purpose 64-bit registers
        uint64_t pc;		// 64-bit program counter
        BUS bus;		    // BUS connected to the CPU

    public:
        CPU();

        // Function to initialize CPU's registers
        void cpu_init();

        // Function to fetch next instruction from DRAM
        uint32_t cpu_fetch();

        uint64_t cpu_load(uint64_t addr, uint64_t size);

        void cpu_store(uint64_t addr, uint64_t size, uint64_t value);

        // Function to execute fetched instruction
        int cpu_execute(uint32_t instruction);

        // Debug function to print state of all the regs when error accures
        void dump_regs();
};