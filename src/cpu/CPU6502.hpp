#pragma once
#include <cstdint>


class Bus;


class CPU6502
{
    public:
        CPU6502();

        void connectBus(Bus* bus);
        void reset();
        void step();
        uint8_t fetchByte();

        // CPU Registers
        uint8_t A = 0;      // Accumulator
        uint8_t X = 0;      // X Register
        uint8_t Y = 0;      // Y Register
        uint8_t SP = 0;     // Stack Pointer
        uint16_t PC = 0;    // Program Counter
        uint8_t status = 0; // Status Register

    private:
        Bus* _bus = nullptr;

        // Helper methods for flag manipulation, addressing modes, etc.
};