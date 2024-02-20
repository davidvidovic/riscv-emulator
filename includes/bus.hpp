#pragma once

#include "dram.hpp"

class BUS {
    private:
        DRAM dram;
    public:
        BUS();
        uint64_t bus_load(uint64_t addr, uint64_t size);
        void bus_store(uint64_t addr, uint64_t size, uint64_t value);
};
