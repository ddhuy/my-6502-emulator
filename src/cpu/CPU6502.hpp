#pragma once
#include <cstdint>


class Bus;


class CPU6502
{
    public:
        enum StatusFlag : uint8_t
        {
            C = 1 << 0, // Carry
            Z = 1 << 1, // Zero
            I = 1 << 2, // Interrupt Disable
            D = 1 << 3, // Decimal
            B = 1 << 4, // Break
            U = 1 << 5, // Unused (always 1)
            V = 1 << 6, // Overflow
            N = 1 << 7  // Negative
        };

    public:
        CPU6502();

        void connectBus(Bus* bus);
        void reset();
        void step();
        uint8_t fetchByte();

        // Flags manipulation methods would go here
        void setFlag(StatusFlag flag, bool value);
        bool getFlag(StatusFlag flag) const;
        void updateZN(uint8_t value);

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