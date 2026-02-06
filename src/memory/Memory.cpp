#include <cstring>
#include "Memory.h"


Memory::Memory()
{
    Clear();
}

Memory::~Memory()
{
    // Nothing to clean up
}

uint8_t Memory::Read(uint16_t address) const
{
    return ram[address];
}

void Memory::Write(uint16_t address, uint8_t value)
{
    ram[address] = value;
}

void Memory::Clear()
{
    std::memset(ram, 0, sizeof(ram));
}