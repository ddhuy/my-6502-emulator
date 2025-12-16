#pragma once
#include <cstdint>


class Memory
{
    public:
        virtual ~Memory() = default;

        virtual uint8_t read(uint16_t address) const = 0;
        virtual void write(uint16_t address, uint8_t value) = 0;
};