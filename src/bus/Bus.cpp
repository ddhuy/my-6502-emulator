#include "Bus.h"
#include "cpu/CPU.h"
#include "ppu/PPU.h"
#include "memory/Memory.h"
#include "cartridge/Cartridge.h"


Bus::Bus()
    : _cpu(nullptr),
    _ppu(nullptr),
    _memory(nullptr),
    _cartridge(nullptr),
    _systemClockCounter(0)
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

void Bus::InsertCartridge(Cartridge* cart)
{
    _cartridge = cart;
    if (_ppu && _cartridge)
    {
        _ppu->ConnectCartridge(_cartridge);
    }
}

uint8_t Bus::CPURead(uint16_t address) const
{
    // Cartridge space ($4020-$FFFF, but mostly $6000-$FFFF)
    if (_cartridge && address >= 0x6000)
    {
        return _cartridge->CPURead(address);
    }
    // PPU registers ($2000-$3FFF, mirrored)
    else if (0x2000 <= address && address < 0x4000)
    {
        return _ppu ? _ppu->CPURead(address) : 0x00;
    }
    // APU and I/O registers ($4000-$4017)
    else if (0x4000 <= address && address < 0x4020)
    {
		// TODO: implement APU and controller readers
        return 0x00;
    }
    // RAM & mirrors ($0000-$1FFF)
    else
    {
        return _memory->Read(address);
    }
}

void Bus::CPUWrite(uint16_t address, uint8_t data)
{
    // Cartridge space
    if (address >= 0x6000)
    {
        _cartridge->CPUWrite(address, data);
    }
    // PPU registers ($2000-$3FFF, mirrored)
    else if (0x2000 <= address && address < 0x4000)
    {
        _ppu->CPUWrite(address, data);
    }
    // OAM DMA ($4014)
    else if (address == 0x4014)
    {
        uint16_t pageStart = data << 8;
        for (uint16_t i = 0; i < 256; ++i)
        {
            uint8_t value = CPURead(pageStart + i);
            _ppu->CPUWrite(0x2004, value); // Write to OAMData
        }
        
        // DMA takes 513 or 514 cycles
        // TODO: Add proper cycle counting.
    }
    // APU and I/O registers ($4000-$4017)
    else if (0x4000 <= address && address < 0x4020)
    {
        // TODO: implement APU & Controller writes
    }
    // RAM & mirrors ($0000-$1FFF)
    else
    {
        _memory->Write(address, data);
    }
}

void Bus::Reset()
{
    _systemClockCounter = 0;
    if (_cpu) _cpu->Reset();
    if (_ppu) _ppu->Reset();
    if (_cartridge) _cartridge->Reset();
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