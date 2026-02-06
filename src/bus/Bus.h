#ifndef BUS_H
#define BUS_H

#include <cstdint>

// Forward Declarations
class CPU;
class PPU;
class Memory;


class Bus
{
public:

    Bus();
    ~Bus();

    // Connect Components
    void ConnectCPU(CPU* c);
    void ConnectPPU(PPU* p);
    void ConnectMemory(Memory* m);

    // CPU Read/Write operations
    uint8_t Read(uint16_t address) const;
    void Write(uint16_t address, uint8_t value);

    // System operations
    void Reset();
    void Clock();

    // Helpers
    PPU* GetPPU() { return _ppu; }

private:
    CPU*        _cpu;
    PPU*        _ppu;
    Memory*     _memory;

    // System clock counter
    uint64_t _systemClockCounter;
};


#endif // BUS_H