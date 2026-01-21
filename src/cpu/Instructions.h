#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cstdint>


// Forward declaration
class CPU;

// Instruction function pointer type
typedef uint8_t (CPU::*OperateFunc)();
typedef uint8_t (CPU::*AddressModeFunc)();

// Instruction structure
struct Instruction
{
    const char* name;
    OperateFunc operate;
    AddressModeFunc addrMode;
    uint8_t cycles;
};

// Opcode lookup table (256 entries)
extern const Instruction INSTRUCTION_TABLE[256];

#endif // INSTRUCTIONS_H