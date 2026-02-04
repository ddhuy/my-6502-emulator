#include "PPU.h"
#include "Bus.h"

PPU::PPU() : _bus(nullptr)
{}

PPU::~PPU()
{}

void PPU::ConnectBus(Bus* b)
{
    _bus = b;
}

uint8_t PPU::Read(uint16_t address)
{
    return 0;
}

void PPU::Write(uint16_t address, uint8_t value)
{
    
}

void PPU::Reset()
{}

void PPU::Clock()
{}

bool PPU::NMIOccurred()
{
    return false;
}

void PPU::ClearNMI()
{
    
}