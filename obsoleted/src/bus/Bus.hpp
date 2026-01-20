#pragma once
#include <cstdint>


class Memory;


class Bus
{
    public:
        void attachMemory(Memory* memory);

        uint8_t read(uint16_t address) const;
        void write(uint16_t address, uint8_t value);

    private:
        Memory* _memory = nullptr;
};
