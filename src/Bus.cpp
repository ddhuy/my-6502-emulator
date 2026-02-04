#include "Bus.h"


Bus::Bus()
{}

Bus::Bus(PPU& ppu, APU& apu, Controller& ctrl1, Controller& ctrl2)
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