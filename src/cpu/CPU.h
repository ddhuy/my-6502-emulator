#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <memory>
#include "bus/Bus.h"


class CPU
{
private:
    Bus* _bus;

    // Current opcode being executed
    uint8_t _opcode;

    // Cycle counter;
    uint64_t _cycles;
    uint64_t _totalCycles;

    // Interrupt flags
    bool _nmiPending = false;
    bool _irqPending = false;

    // Fetched data and address for current instruction
    uint8_t _fetched;
    uint16_t _addrAbs;
    uint16_t _addrRel;

public:
    // Status flag bit positions
    enum StatusFlag
    {
        F_CARRY     = (1 << 0), // C
        F_ZERO      = (1 << 1), // Z
        F_INTERRUPT = (1 << 2), // I
        F_DECIMAL   = (1 << 3), // D (not used in NES, but implemented)
        F_BREAK     = (1 << 4), // B
        F_UNUSED    = (1 << 5), // U - always set to 1
        F_OVERFLOW  = (1 << 6), // V
        F_NEGATIVE  = (1 << 7)  // N
    };

    // Addressing modes
    enum AddressingMode
    {
        M_IMP, // Implied
        M_ACC, // Accumulator
        M_IMM, // Immediate
        M_ZP0, // Zero Page
        M_ZPX, // Zero Page,X
        M_ZPY, // Zero Page,Y
        M_REL, // Relative
        M_ABS, // Absolute
        M_ABX, // Absolute,X
        M_ABY, // Absolute,Y
        M_IND, // Indirect
        M_IZX, // (Indirect,X)
        M_IZY  // (Indirect),Y
    };

    CPU();
    virtual ~CPU();

    void ConnectBus(Bus* bus);

    // Registers
    uint8_t  A;   // Accumulator
    uint8_t  X;   // X Register
    uint8_t  Y;   // Y Register
    uint8_t  SP;  // Stack Pointer
    uint16_t PC;  // Program Counter
    uint8_t  P;   // Status Register

    // Reset CPU to initial state
    void Reset();

    // Execute one clock cycle
    void Clock();

    // Execute one full instruction
    void Step();

    // Load a program into memory at a specified address
    void LoadProgram(const uint8_t* program, size_t size, uint16_t address = 0x8000);

    // Status flag operations
    bool GetFlag(StatusFlag flag) const;
    void SetFlag(StatusFlag flag, bool value);
    void UpdateZN(uint8_t value);

    // Stack operations
    void PushStack(uint8_t value);
    uint8_t PopStack();
    void PushStack16(uint16_t value);
    uint16_t PopStack16();

    // Get current instruction info for debugging
    const char* GetCurrentInstruction() const;

    // Data read/write helpers
    uint8_t Fetch();
    void Commit(uint8_t value);
    void WriteMemory(uint16_t address, uint8_t value) { _bus->Write(address, value); }
    uint8_t ReadMemory(uint16_t address) const { return _bus->Read(address); }

    // Branch helper
    uint8_t Branch(bool condition);

    // Read a byte at PC & increment PC
    uint8_t ReadPC();

    // Read a 16-bit word at PC & increment PC by 2
    uint16_t ReadPC16();

    // Getters
    uint16_t GetAddressAbsolute() const { return _addrAbs; }
    uint16_t GetAddressRelative() const { return _addrRel; }
    uint64_t GetCycles() const { return _cycles; }
    uint64_t GetTotalCycles() const { return _totalCycles; }
    uint8_t GetOpcode() const { return _opcode; }

    // Interrupts
    void NMI() { _nmiPending = true; }
    void IRQ() { _irqPending = true; }

    // Addressing Modes - return 1 if additional cycle may be needed
    uint8_t IMP(); uint8_t ACC(); uint8_t IMM(); uint8_t REL();
    uint8_t ZP0(); uint8_t ZPX(); uint8_t ZPY();
    uint8_t ABS(); uint8_t ABX(); uint8_t ABY();
    uint8_t IND(); uint8_t IZX(); uint8_t IZY();

    // Instruction implementations - return 1 if additional cycle may be needed
    uint8_t LDA(); uint8_t LDX(); uint8_t LDY(); uint8_t STA();
    uint8_t STX(); uint8_t STY(); uint8_t TAX(); uint8_t TAY();
    uint8_t TXA(); uint8_t TYA(); uint8_t TSX(); uint8_t TXS();
    
    uint8_t ADC(); uint8_t SBC(); uint8_t INC(); uint8_t DEC();
    uint8_t INX(); uint8_t DEX(); uint8_t INY(); uint8_t DEY();
    
    uint8_t AND(); uint8_t ORA(); uint8_t EOR(); uint8_t BIT();
    
    uint8_t ASL(); uint8_t LSR(); uint8_t ROL(); uint8_t ROR();
    
    uint8_t JMP(); uint8_t JSR(); uint8_t RTS(); uint8_t RTI();
    uint8_t BRK();
    
    uint8_t BCC(); uint8_t BCS(); uint8_t BEQ(); uint8_t BNE();
    uint8_t BMI(); uint8_t BPL(); uint8_t BVC(); uint8_t BVS();
    
    uint8_t PHA(); uint8_t PHP(); uint8_t PLA(); uint8_t PLP();
    
    uint8_t CLC(); uint8_t SEC(); uint8_t CLI(); uint8_t SEI();
    uint8_t CLD(); uint8_t SED(); uint8_t CLV();
    
    uint8_t CMP(); uint8_t CPX(); uint8_t CPY();
    
    uint8_t NOP(); uint8_t XXX();

    // Illegal/Unofficial opcodes
    uint8_t LAX(); uint8_t SAX(); uint8_t DCP(); uint8_t ISC();
    uint8_t SLO(); uint8_t RLA(); uint8_t SRE(); uint8_t RRA();
    uint8_t ANC(); uint8_t ALR(); uint8_t ARR(); uint8_t SBX();
};

#endif // CPU_H