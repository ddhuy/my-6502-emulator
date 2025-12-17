#pragma once
#include <cstdint>


class CPU6502;


struct Instruction
{
    const char* name;
    uint8_t (CPU6502::*operate)();
    uint8_t (CPU6502::*addrmode)();
    uint8_t cycles;
};


extern const Instruction instructionTable[256];
