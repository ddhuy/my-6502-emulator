#ifndef BUS_H
#define BUS_H

#include <cstdint>

#include "APU/APU.h"
#include "Cartridge.h"
#include "Controller.h"
#include "Mapper.h"
#include "Memory.h"
#include "PPU.h"


class Bus
{
public:

    Bus();
    Bus(PPU& ppu, APU& apu, Controller& ctrl1, Controller& ctrl2);
    virtual ~Bus();

    void AttachMemory(Memory* memory);

    uint8_t Read(uint16_t address) const;
    void Write(uint16_t address, uint8_t value);

private:
    APU*        _apu;
    PPU*        _ppu;
    Memory*     _memory;
    Controller* _controller1;
    Controller* _controller2;
    Mapper*     _mapper;

};


#endif // BUS_H