#include <cstring>
#include "CPU.h"
#include "memory/Memory.h"
#include "Instructions.h"


CPU::CPU() : _bus(nullptr)
{
}

CPU::~CPU()
{
}

void CPU::ConnectBus(Bus* bus)
{
    _bus = bus;
}

void CPU::Reset()
{
    // Reset registers and flags
    A = X = Y = 0;
    SP = 0xFD; // Stack Pointer starts at 0xFD
    P = F_UNUSED | F_INTERRUPT; // Set unused and interrupt disable flags
    
    // Set Program Counter to the address stored at the reset vector (0xFFFC)
    PC = (_bus->Read(0xFFFD) << 8) | _bus->Read(0xFFFC);
    
    // Reset internal helper variables
    _addrAbs = 0;
    _addrRel = 0;
    _fetched = 0;

    // Reset takes time
    _cycles = 7; // Reset takes 7 _cycles
    _totalCycles = 0;
}

void CPU::LoadProgram(const uint8_t* program, size_t size, uint16_t address)
{
    for (size_t i = 0; i < size; ++i)
        _bus->Write(address + i, program[i]);
    
    // Set PC to start of program
    PC = address;
    
    // Set the reset vector to the start of the program
    _bus->Write(0xFFFC, address & 0xFF);
    _bus->Write(0xFFFD, (address >> 8) & 0xFF);
}

void CPU::SetFlag(StatusFlag flag, bool value)
{
    if (value)
        P |= flag;
    else
        P &= ~flag;
}

bool CPU::GetFlag(StatusFlag flag) const
{
    return (P & flag) != 0;
}

void CPU::UpdateZN(uint8_t value)
{
    SetFlag(F_ZERO, value == 0);
    SetFlag(F_NEGATIVE, value & 0x80);
}

void CPU::PushStack(uint8_t value)
{
    _bus->Write(0x0100 + SP, value);
    SP--;
}

uint8_t CPU::PopStack()
{
    SP++;
    return _bus->Read(0x0100 + SP);
}

void CPU::PushStack16(uint16_t value)
{
    PushStack((value >> 8) & 0xFF);
    PushStack(value & 0xFF);
}

uint16_t CPU::PopStack16()
{
    uint8_t low = PopStack();
    uint8_t high = PopStack();
    return (high << 8) | low;
}

uint8_t CPU::ReadPC()
{
    return _bus->Read(PC++);
}

uint16_t CPU::ReadPC16()
{
    uint8_t low = ReadPC();
    uint8_t high = ReadPC();
    return (high << 8) | low;
}

uint8_t CPU::Fetch()
{
    if (INSTRUCTION_TABLE[_opcode].addrMode != &CPU::ACC)
        _fetched = _bus->Read(_addrAbs);
    else
        _fetched = A;
    return _fetched;
}

void CPU::Commit(uint8_t value)
{
    // write result back
    if (INSTRUCTION_TABLE[_opcode].addrMode == &CPU::ACC)
        A = value;
    else
        _bus->Write(_addrAbs, value);
}

const char* CPU::GetCurrentInstruction() const
{
    return INSTRUCTION_TABLE[_opcode].name;
}


// ============================================================================
// CYCLE EXECUTION
// ============================================================================

void CPU::Clock()
{
    if (_cycles == 0)
    {
        // Fetch the next opcode
        _opcode = ReadPC();
        const Instruction& instr = INSTRUCTION_TABLE[_opcode];

        // Set base _cycles
        _cycles = instr.cycles;

        // Call the addressing mode function and operation function
        uint8_t additionalCycle1 = (this->*instr.addrMode)();
        uint8_t additionalCycle2 = (this->*instr.operate)();
        
        // Add extra cycle only if BOTH addressing mode AND operation require it
        _cycles += (additionalCycle1 & additionalCycle2);
    }

    _cycles--;
    _totalCycles++;
}

void CPU::Step()
{
    do {
        Clock();
    } while (_cycles > 0);
}


// ============================================================================
// ADDRESSING MODES
// ============================================================================

// Implied - no additional data needed
uint8_t CPU::IMP()
{
    _fetched = A;
    return 0;
}

// Accumulator - operates directly on accumulator
uint8_t CPU::ACC()
{
    _fetched = A;
    return 0;
}

// Immediate - next byte is the value
uint8_t CPU::IMM()
{
    _addrAbs = PC++;
    return 0;
}

// Zero Page - next byte is zero page address (0x00 - 0xFF)
uint8_t CPU::ZP0()
{
    _addrAbs = ReadPC();
    _addrAbs &= 0x00FF; // Zero page wrap-around
    return 0;
}

// Zero Page,X - next byte + X register, zero page wrap-around
uint8_t CPU::ZPX()
{
    _addrAbs = (ReadPC() + X) & 0x00FF; // Zero page wrap-around
    return 0;
}

// Zero Page,Y - next byte + Y register, zero page wrap-around
uint8_t CPU::ZPY()
{
    _addrAbs = (ReadPC() + Y) & 0x00FF; // Zero page wrap-around
    return 0;
}

// Relative - next byte is signed offset for branches
uint8_t CPU::REL()
{
    _addrRel = ReadPC();
    
    if (_addrRel & 0x80) // Negative offset
        _addrRel |= 0xFF00; // Sign extend to 16 bits
    
    return 0;
}

// Absolute - next two bytes are the full 16-bit address
uint8_t CPU::ABS()
{
    uint16_t lo = ReadPC();
    uint16_t hi = ReadPC();
    
    _addrAbs = (hi << 8) | lo;
    
    return 0;
}

// Absolute,X - next two bytes + X register
uint8_t CPU::ABX()
{
    uint16_t lo = ReadPC();
    uint16_t hi = ReadPC();
    
    _addrAbs = ((hi << 8) | lo) + X;

    // Return 1 if page boundary crossed
    return ((_addrAbs & 0xFF00) != (hi << 8)) ? 1 : 0;
}

// Absolute,Y - next two bytes + Y register
uint8_t CPU::ABY()
{
    uint16_t lo = ReadPC();
    uint16_t hi = ReadPC();
    
    _addrAbs = ((hi << 8) | lo) + Y;
    
    // Return 1 if page boundary crossed
    return ((_addrAbs & 0xFF00) != (hi << 8)) ? 1 : 0;
}

// Indirect - next two bytes are pointer to address (JMP only)
uint8_t CPU::IND()
{
    uint16_t ptrLo = ReadPC();
    uint16_t ptrHi = ReadPC();
    uint16_t ptr = (ptrHi << 8) | ptrLo;

    // Simulate 6502 page boundary hardware bug
    if ((ptr & 0x00FF) == 0x00FF)
        _addrAbs = (_bus->Read(ptr & 0xFF00) << 8) | _bus->Read(ptr);
    else
        _addrAbs = (_bus->Read(ptr + 1) << 8) | _bus->Read(ptr);

    return 0;
}

// (Indirect,X) - next byte + X register points to zero page address
uint8_t CPU::IZX()
{
    uint16_t t = ReadPC();
    uint16_t lo = _bus->Read((t + X) & 0x00FF);
    uint16_t hi = _bus->Read((t + X + 1) & 0x00FF);
    
    _addrAbs = (hi << 8) | lo;
    
    return 0;
}

// (Indirect),Y - next byte points to zero page address, add Y register
uint8_t CPU::IZY()
{
    uint16_t t = ReadPC();
    uint16_t lo = _bus->Read(t & 0x00FF);
    uint16_t hi = _bus->Read((t + 1) & 0x00FF);
    
    uint16_t base = (hi << 8) | lo;
    _addrAbs = base + Y;
 
    // Return 1 if page boundary crossed
    return ((base & 0xFF00) != (_addrAbs & 0xFF00)) ? 1 : 0;
}


// ============================================================================
// LOAD/STORE INSTRUCTIONS
// ============================================================================

uint8_t CPU::LDA()
{
    A = Fetch();
    UpdateZN(A);
    return 1;
}

uint8_t CPU::LDX()
{
    X = Fetch();
    UpdateZN(X);
    return 1;
}

uint8_t CPU::LDY()
{
    Y = Fetch();
    UpdateZN(Y);
    return 1;
}

uint8_t CPU::STA()
{
    _bus->Write(_addrAbs, A);
    return 0;
}

uint8_t CPU::STX()
{
    _bus->Write(_addrAbs, X);
    return 0;
}

uint8_t CPU::STY()
{
    _bus->Write(_addrAbs, Y);
    return 0;
}

uint8_t CPU::TAX()
{
    X = A;
    UpdateZN(X);
    return 0;
}

uint8_t CPU::TAY()
{
    Y = A;
    UpdateZN(Y);
    return 0;
}

uint8_t CPU::TXA()
{
    A = X;
    UpdateZN(A);
    return 0;
}

uint8_t CPU::TYA()
{
    A = Y;
    UpdateZN(A);
    return 0;
}

uint8_t CPU::TSX()
{
    X = SP;
    UpdateZN(X);
    return 0;
}

uint8_t CPU::TXS()
{
    SP = X;
    return 0;
}

// ============================================================================
// ARITHMETIC INSTRUCTIONS
// ============================================================================

uint8_t CPU::ADC()
{
    Fetch();
    uint16_t result = A + _fetched + (GetFlag(F_CARRY) ? 1 : 0);
    
    SetFlag(F_CARRY, result > 0xFF);
    SetFlag(F_OVERFLOW, (~(A ^ _fetched) & (A ^ result) & 0x80) != 0);
    
    A = result & 0xFF;
     UpdateZN(A);
    return 1;  // Can use extra cycle
}

uint8_t CPU::SBC()
{
    Fetch();
    uint16_t value = _fetched ^ 0xFF;  // Invert for subtraction
    uint16_t result = A + value + (GetFlag(F_CARRY) ? 1 : 0);
    
    SetFlag(F_CARRY, result > 0xFF);
    SetFlag(F_OVERFLOW, (~(A ^ value) & (A ^ result) & 0x80) != 0);
    
    A = result & 0xFF;
     UpdateZN(A);
    return 1;  // Can use extra cycle
}

uint8_t CPU::INC()
{
    Fetch();
    uint8_t result = _fetched + 1;
    _bus->Write(_addrAbs, result);
     UpdateZN(result);
    return 0;
}

uint8_t CPU::DEC()
{
    Fetch();
    uint8_t result = _fetched - 1;
    _bus->Write(_addrAbs, result);
     UpdateZN(result);
    return 0;
}

uint8_t CPU::INX()
{
    X++;
     UpdateZN(X);
    return 0;
}

uint8_t CPU::DEX()
{
    X--;
     UpdateZN(X);
    return 0;
}

uint8_t CPU::INY()
{
    Y++;
     UpdateZN(Y);
    return 0;
}

uint8_t CPU::DEY()
{
    Y--;
     UpdateZN(Y);
    return 0;
}

// ============================================================================
// LOGICAL INSTRUCTIONS
// ============================================================================

uint8_t CPU::AND()
{
    Fetch();
    A &= _fetched;
     UpdateZN(A);
    return 1;  // Can use extra cycle
}

uint8_t CPU::ORA()
{
    Fetch();
    A |= _fetched;
     UpdateZN(A);
    return 1;  // Can use extra cycle
}

uint8_t CPU::EOR()
{
    Fetch();
    A ^= _fetched;
     UpdateZN(A);
    return 1;  // Can use extra cycle
}

uint8_t CPU::BIT()
{
    Fetch();
    SetFlag(F_ZERO, (A & _fetched) == 0);
    SetFlag(F_OVERFLOW, (_fetched & F_OVERFLOW) != 0);
    SetFlag(F_NEGATIVE, (_fetched & F_NEGATIVE) != 0);
    return 0;
}

// ============================================================================
// SHIFT/ROTATE INSTRUCTIONS
// ============================================================================

uint8_t CPU::ASL()
{
    Fetch();
    uint16_t result = (uint16_t)_fetched << 1;
    
    SetFlag(F_CARRY, (result & 0xFF00) != 0);
    SetFlag(F_ZERO, (result & 0x00FF) == 0);
    SetFlag(F_NEGATIVE, (result & 0x80) != 0);
    
    Commit(result & 0xFF);
    return 0;
}

uint8_t CPU::LSR()
{
    Fetch();
    SetFlag(F_CARRY, (_fetched & 0x01) != 0);
    uint8_t result = _fetched >> 1;
     UpdateZN(result);
    
    Commit(result);
    return 0;
}

uint8_t CPU::ROL()
{
    Fetch();
    uint16_t result = (uint16_t)(_fetched << 1) | (GetFlag(F_CARRY) ? 1 : 0);
    SetFlag(F_CARRY, (result & 0xFF00) > 0);
    result &= 0xFF;
     UpdateZN(result);
    
    Commit(result);
    return 0;
}

uint8_t CPU::ROR()
{
    Fetch();
    uint16_t result = (GetFlag(F_CARRY) ? (1 << 7) : 0) | (_fetched >> 1);
    SetFlag(F_CARRY, (_fetched & 0x01) != 0);
     UpdateZN(result);
    
    Commit(result & 0xFF);
    return 0;
}

// ============================================================================
// JUMP/CALL INSTRUCTIONS
// ============================================================================

uint8_t CPU::JMP()
{
    PC = _addrAbs;
    return 0;
}

uint8_t CPU::JSR()
{
    PC--;  // Point to last byte of JSR instruction
    PushStack16(PC);
    PC = _addrAbs;
    return 0;
}

uint8_t CPU::RTS()
{
    PC = PopStack16();
    PC++;  // Move to next instruction
    return 0;
}

uint8_t CPU::RTI()
{
    P = PopStack();
    P &= ~F_BREAK;
    P |= F_UNUSED;
    PC = PopStack16();
    return 0;
}

uint8_t CPU::BRK()
{
    PC++;
    PushStack16(PC);
    SetFlag(F_BREAK, true);
    PushStack(P);
    SetFlag(F_INTERRUPT, true);
    PC = _bus->Read(0xFFFE) | (_bus->Read(0xFFFF) << 8);
    return 0;
}

// ============================================================================
// BRANCH INSTRUCTIONS
// ============================================================================

uint8_t CPU::Branch(bool condition)
{
    if (condition)
    {
        _cycles++;  // Branch taken
        
        _addrAbs = PC + _addrRel;
        
        // Check if page boundary crossed
        if ((_addrAbs & 0xFF00) != (PC & 0xFF00))
        {
            _cycles++;  // Page crossed
        }
        
        PC = _addrAbs;
    }
    return 0;  // Branches handle their own cycles
}

uint8_t CPU::BCC()
{
    return Branch(!GetFlag(F_CARRY));
}

uint8_t CPU::BCS()
{
    return Branch(GetFlag(F_CARRY));
}

uint8_t CPU::BEQ()
{
    return Branch(GetFlag(F_ZERO));
}

uint8_t CPU::BNE()
{
    return Branch(!GetFlag(F_ZERO));
}

uint8_t CPU::BMI()
{
    return Branch(GetFlag(F_NEGATIVE));
}

uint8_t CPU::BPL()
{
    return Branch(!GetFlag(F_NEGATIVE));
}

uint8_t CPU::BVC()
{
    return Branch(!GetFlag(F_OVERFLOW));
}

uint8_t CPU::BVS()
{
    return Branch(GetFlag(F_OVERFLOW));
}

// ============================================================================
// STACK INSTRUCTIONS
// ============================================================================

uint8_t CPU::PHA()
{
    PushStack(A);
    return 0;
}

uint8_t CPU::PHP()
{
    PushStack(P | F_BREAK | F_UNUSED);
    return 0;
}

uint8_t CPU::PLA()
{
    A = PopStack();
     UpdateZN(A);
    return 0;
}

uint8_t CPU::PLP()
{
    P = PopStack();
    P &= ~F_BREAK;
    P |= F_UNUSED;
    return 0;
}

// ============================================================================
// STATUS FLAG INSTRUCTIONS
// ============================================================================

uint8_t CPU::CLC()
{
    SetFlag(F_CARRY, false);
    return 0;
}

uint8_t CPU::SEC()
{
    SetFlag(F_CARRY, true);
    return 0;
}

uint8_t CPU::CLI()
{
    SetFlag(F_INTERRUPT, false);
    return 0;
}

uint8_t CPU::SEI()
{
    SetFlag(F_INTERRUPT, true);
    return 0;
}

uint8_t CPU::CLD()
{
    SetFlag(F_DECIMAL, false);
    return 0;
}

uint8_t CPU::SED()
{
    SetFlag(F_DECIMAL, true);
    return 0;
}

uint8_t CPU::CLV()
{
    SetFlag(F_OVERFLOW, false);
    return 0;
}

// ============================================================================
// COMPARISON INSTRUCTIONS
// ============================================================================

uint8_t CPU::CMP()
{
    Fetch();
    uint16_t result = (uint16_t)A - (uint16_t)_fetched;
    SetFlag(F_CARRY, A >= _fetched);
    SetFlag(F_ZERO, (result & 0xFF) == 0);
    SetFlag(F_NEGATIVE, (result & 0x80) != 0);
    return 1;  // Can use extra cycle
}

uint8_t CPU::CPX()
{
    Fetch();
    uint16_t result = (uint16_t)X - (uint16_t)_fetched;
    SetFlag(F_CARRY, X >= _fetched);
    SetFlag(F_ZERO, (result & 0xFF) == 0);
    SetFlag(F_NEGATIVE, (result & 0x80) != 0);
    return 0;
}

uint8_t CPU::CPY()
{
    Fetch();
    uint16_t result = (uint16_t)Y - (uint16_t)_fetched;
    SetFlag(F_CARRY, Y >= _fetched);
    SetFlag(F_ZERO, (result & 0xFF) == 0);
    SetFlag(F_NEGATIVE, (result & 0x80) != 0);
    return 0;
}

// ============================================================================
// SYSTEM INSTRUCTIONS
// ============================================================================

uint8_t CPU::NOP()
{
    // Some unofficial NOPs fetch
    if (INSTRUCTION_TABLE[_opcode].addrMode != &CPU::IMP) {
        Fetch();
    }
    return 1;  // Some NOPs can use extra cycle (like NOP $NNNN,X)
}

uint8_t CPU::XXX()
{
    // Illegal opcode - do nothing
    return 0;
}