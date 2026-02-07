#ifndef BUS_H
#define BUS_H

#include <cstdint>
#include <memory>

// Forward Declarations
class CPU;
class PPU;
class Memory;
class Cartridge;


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
    uint8_t CPURead(uint16_t address) const;
    void CPUWrite(uint16_t address, uint8_t value);

    // System operations
    void Reset();
    void Clock();

    // Get component references
    PPU* GetPPU() { return _ppu; }
    Cartridge* GetCartridge() { return _cartridge; }

private:
    CPU*        _cpu;
    PPU*        _ppu;
    Memory*     _memory;
    Cartridge*  _cartridge;

    // System clock counter
    uint64_t _systemClockCounter;
};


#endif // BUS_H