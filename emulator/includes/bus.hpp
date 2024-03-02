#pragma once

#include "dram.hpp"

class BUS {
    private:
        DRAM dram;
        
    public:
        BUS();
        uint64_t bus_load(uint64_t, uint64_t);
        void bus_store(uint64_t, uint64_t, uint64_t);
};
