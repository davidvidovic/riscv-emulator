#pragma once

#include <cstdint>

#define DRAM_SIZE	1024*1024*1
#define DRAM_BASE	0x80000000

class DRAM
{
    private:
        uint8_t mem[DRAM_SIZE];

        // Private functions to be called internally after public function was called

        void dram_store_8(uint64_t addr, uint64_t value);
        void dram_store_16(uint64_t addr, uint64_t value);
        void dram_store_32(uint64_t addr, uint64_t value);
        void dram_store_64(uint64_t addr, uint64_t value);

        uint64_t dram_load_8(uint64_t addr);
        uint64_t dram_load_16(uint64_t addr);
        uint64_t dram_load_32(uint64_t addr);
        uint64_t dram_load_64(uint64_t addr);

    public:
        DRAM();
        uint64_t dram_load(uint64_t addr, uint64_t size);
        void dram_store(uint64_t addr, uint64_t size, uint64_t value);
};
