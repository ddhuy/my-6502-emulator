#include "Bus.h"
#include "memory/Memory.h"

Bus::Bus()
    : _memory(nullptr)
{
}

Bus::~Bus()
{
}

void Bus::AttachMemory(Memory* memory)
{
    _memory = memory;
}

uint8_t Bus::Read(uint16_t address) const
{
    return _memory->Read(address);
}

void Bus::Write(uint16_t address, uint8_t value)
{
    _memory->Write(address, value);
}