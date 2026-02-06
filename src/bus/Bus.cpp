#include "Bus.h"
#include "cpu/CPU.h"
#include "ppu/PPU.h"
#include "memory/Memory.h"


Bus::Bus() : _cpu(nullptr), _ppu(nullptr), _memory(nullptr), _systemClockCounter(0)
{}

Bus::~Bus()
{
}

void Bus::ConnectCPU(CPU* c)
{
    _cpu = c;
    if (_cpu)
        _cpu->ConnectBus(this);
}

void Bus::ConnectPPU(PPU* p)
{
    _ppu = p;
    if (_ppu)
        _ppu->ConnectBus(this);
}

void Bus::ConnectMemory(Memory* m)
{
    _memory = m;
}

uint8_t Bus::Read(uint16_t address) const
{
    if (0x2000 <= address && address < 0x4000)
        return _ppu ? _ppu->Read(address) : 0x00;
    return _memory->Read(address);
}

void Bus::Write(uint16_t address, uint8_t value)
{
    if (0x2000 <= address && address < 0x4000 && _ppu)
        _ppu->Write(address, value);
    else
        _memory->Write(address, value);
}

void Bus::Reset()
{
    _systemClockCounter = 0;
    if (_cpu) _cpu->Reset();
    if (_ppu) _ppu->Reset();
}

void Bus::Clock()
{
    // PPU runs 3 times faster than CPU
    _ppu->Clock();
    
    // CPU runs every 3 PPU clocks
    if (_systemClockCounter % 3 == 0)
        _cpu->Clock();

    // Check for NMI from PPU
    if (_ppu->NMIOccurred())
    {
        _ppu->ClearNMI();
        
        // Trigger NMI on CPU
        _cpu->NMI();
    }

    // Increase Clock
    _systemClockCounter++;
}