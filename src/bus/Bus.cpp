#include <cassert>

#include "utils/Debug.hpp"
#include "Bus.hpp"
#include "mem/Memory.hpp"


void Bus::attachMemory(Memory* memory)
{
    _memory = memory;
}

uint8_t Bus::read(uint16_t address) const
{
    DBG_ASSERT(_memory != nullptr);
    return _memory->read(address);
}

void Bus::write(uint16_t address, uint8_t value)
{
    DBG_ASSERT(_memory != nullptr);
    _memory->write(address, value);
}