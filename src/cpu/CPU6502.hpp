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
        void clock();
        uint8_t cycles() const { return _cycles; }

        // Flags manipulation methods would go here
        void setFlag(StatusFlag flag, bool value);
        bool getFlag(StatusFlag flag) const;
        void updateZN(uint8_t value);

        // Data fetch helper
        uint8_t fetch();
        uint8_t fetchByte();

        // Addressing modes
        uint8_t IMM(); // Immediate
        uint8_t ZP0(); // Zero Page
        uint8_t ZPX(); // Zero Page, X Indexed
        uint8_t ZPY(); // Zero Page, Y Indexed
        uint8_t ABS(); // Absolute
        uint8_t ABX(); // Absolute, X Indexed
        uint8_t ABY(); // Absolute, Y Indexed
        uint8_t IMP(); // Implied
        uint8_t REL(); // Relative
        uint8_t ACC(); // Accumulator Addressing Mode

        // Instructions
        uint8_t LDA(); // Load Accumulator
        uint8_t STA(); // Store Accumulator
        uint8_t TAX(); // Transfer Accumulator to X
        uint8_t INX(); // Increment X Register
        uint8_t BEQ(); // Branch if Equal
        uint8_t BNE(); // Branch if Not Equal
        uint8_t BMI(); // Branch if Minus
        uint8_t BPL(); // Branch if Positive
        uint8_t BCS(); // Branch if Carry Set
        uint8_t BCC(); // Branch if Carry Clear
        uint8_t BVS(); // Branch if Overflow Set
        uint8_t BVC(); // Branch if Overflow Clear
        uint8_t JSR(); // Jump to Subroutine
        uint8_t RTS(); // Return from Subroutine
        uint8_t ADC(); // Add with Carry
        uint8_t SBC(); // Subtract with Carry
        uint8_t CMP(); // Compare
        uint8_t CPX(); // Compare X Register
        uint8_t CPY(); // Compare Y Register
        uint8_t BIT(); // Bit Test
        uint8_t NOP(); // No Operation
        uint8_t ASL(); // Arithmetic Shift Left
        uint8_t LSR(); // Logical Shift Right
        uint8_t ROL(); // Rotate Left
        uint8_t ROR(); // Rotate Right
        uint8_t BRK(); // Force Interrupt
        uint8_t RTI(); // Return from Interrupt

        // CPU Registers
        uint8_t A = 0;      // Accumulator
        uint8_t X = 0;      // X Register
        uint8_t Y = 0;      // Y Register
        uint8_t SP = 0;     // Stack Pointer
        uint16_t PC = 0;    // Program Counter
        uint8_t P = 0; // Status Register

    private:
        Bus* _bus = nullptr;

        uint16_t _addr_abs = 0;   // absolute address
        uint16_t _addr_rel = 0;   // relative address for branches
        uint8_t  _fetched  = 0;   // data fetched from addr_abs

        // Internal helper variables
        uint8_t _opcode = 0;
        uint8_t _cycles = 0;

        // Helper methods for flag manipulation, addressing modes, etc.
        uint8_t branch(bool condition);
        void compare(uint8_t reg, uint8_t value);

        // Stack operations
        void push(uint8_t value);
        uint8_t pull();
};