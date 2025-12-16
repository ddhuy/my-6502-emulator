#include <iostream>
#include "RAM.hpp"
#include "utils/Debug.hpp"


RAM::RAM(std::size_t size)
    : _data(size, 0x0)
{
    DBG_LOG("Initialized RAM with size: %d bytes\n", static_cast<int>(size));
}

uint8_t RAM::read(uint16_t address) const
{
    DBG_ASSERT(address < _data.size());
    return _data[address];
}

void RAM::write(uint16_t address, uint8_t value)
{
    DBG_ASSERT(address < _data.size());
    _data[address] = value;
}