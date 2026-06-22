#include "Bus.h"
#include "cpu/CPU.h"
#include "ppu/PPU.h"
#include "memory/Memory.h"
#include "cartridge/Cartridge.h"
#include "utils/Logger.h"
#include "utils/Disassembler.h"


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

uint8_t Bus::CPURead(uint16_t address)
{
    LOG_DEBUG("address=0x%04x", address);

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
    // Controller readers: $4016 for controller 1, $4017 for controller 2
    else if (address == 0x4016 || address == 0x4017)
    {
        uint8_t index = address & 0x0001; // 0 for $4016, 1 for $4017
        return _controllers[index].Read();
    }
    // APU and I/O registers ($4000-$4017)
    else if (0x4000 <= address && address < 0x4020)
    {

    }
    // RAM & mirrors ($0000-$1FFF)
    else
    {
        return _memory->Read(address);
    }

    return 0x00; // Default return value for unmapped addresses
}

void Bus::CPUWrite(uint16_t address, uint8_t data)
{
    LOG_DEBUG("address=0x%04x data=0x%02x", address, data);

    // Cartridge space
    if (_cartridge && address >= 0x6000)
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
    // Controller strobe ($4016)
    else if (address == 0x4016)
    {
        _controllers[0].Write(data);
        _controllers[1].Write(data);
    }
    // APU and I/O registers ($4000-$4017)
    else if (0x4000 <= address && address < 0x4020)
    {
        // TODO: implement APU writes
    }
    // RAM & mirrors ($0000-$1FFF)
    else
    {
        _memory->Write(address, data);
    }
}

void Bus::Reset()
{
    LOG_INFO("Bus Reset");

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
    {
        _cpu->Clock();
    }
    
    // Check for NMI from PPU
    if (_ppu->NMIOccurred())
    {
        _ppu->ClearNMI();
        
        // Trigger NMI on CPU
        _cpu->NMI();
        // _cpu->Interrupt(0xFFFA);
    }

    // Increase Clock
    _systemClockCounter++;
}

void Bus::SetControllerState(int index, uint8_t state)
{
    if (index == 0 || index == 1)
        _controllers[index].SetButtons(state);
}