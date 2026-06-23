#ifndef BUS_H
#define BUS_H

#include <cstdint>
#include <memory>

#include "cartridge/Cartridge.h"
#include "cpu/CPU.h"
#include "controller/Controller.h"
#include "memory/Memory.h"
#include "ppu/PPU.h"


class Bus
{
public:

    Bus();
    ~Bus();

    // Connect Components
    void ConnectCPU(CPU* c);
    void ConnectPPU(PPU* p);
    void ConnectMemory(Memory* m);
    void InsertCartridge(Cartridge* cart);

    // CPU Read/Write operations
    uint8_t CPURead(uint16_t address);
    void CPUWrite(uint16_t address, uint8_t value);

    // System operations
    void Reset();
    void Clock();

    // Get component references
    PPU* GetPPU() { return _ppu; }
    Cartridge* GetCartridge() { return _cartridge; }

    // Controller input
    void SetControllerState(int index, uint8_t state);

private:
    CPU*        _cpu;
    PPU*        _ppu;
    Memory*     _memory;
    Cartridge*  _cartridge;

    // System clock counter
    uint64_t _systemClockCounter;

    // Controller Input
    Controller _controllers[2];
};


#endif // BUS_H