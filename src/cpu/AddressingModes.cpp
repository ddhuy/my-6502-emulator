#include "CPU6502.hpp"
#include "bus/Bus.hpp"
#include "cpu/Instructions.hpp"
#include "utils/Debug.hpp"


uint8_t CPU6502::ACC()
{
    _fetched = A;
    return 0;
}

uint8_t CPU6502::IMM()
{
    _addr_abs = PC++;
    return 0;
}

uint8_t CPU6502::ZP0()
{
    DBG_ASSERT(_bus != nullptr);

    _addr_abs = _bus->read(PC++);
    _addr_abs &= 0x00FF; // Zero page address
    return 0;
}

uint8_t CPU6502::ZPX()
{
    DBG_ASSERT(_bus != nullptr);

    _addr_abs = (_bus->read(PC++) + X) & 0x00FF; // Zero page address with X offset
    return 0;
}

uint8_t CPU6502::ZPY()
{
    DBG_ASSERT(_bus != nullptr);

    _addr_abs = (_bus->read(PC++) + Y) & 0x00FF; // Zero page address with Y offset
    return 0;
}

uint8_t CPU6502::ABS()
{
    uint16_t lo = _bus->read(PC++);
    uint16_t hi = _bus->read(PC++);
    _addr_abs = (hi << 8) | lo;
    return 0;
}

uint8_t CPU6502::ABX()
{
    uint16_t lo = _bus->read(PC++);
    uint16_t hi = _bus->read(PC++);
    _addr_abs = ((hi << 8) | lo) + X;

    // Check for page boundary crossing
    if ((_addr_abs & 0xFF00) != (hi << 8))
        return 1; // Add extra cycle

    return 0;
}

uint8_t CPU6502::ABY()
{
    uint16_t lo = _bus->read(PC++);
    uint16_t hi = _bus->read(PC++);
    _addr_abs = ((hi << 8) | lo) + Y;

    // Check for page boundary crossing
    if ((_addr_abs & 0xFF00) != (hi << 8))
        return 1; // Add extra cycle

    return 0;
}

uint8_t CPU6502::IMP()
{
    _fetched = A; // Implied mode often uses the accumulator
    return 0;
}

uint8_t CPU6502::REL()
{
    _addr_rel = _bus->read(PC++);

    if (_addr_rel & 0x80)
        _addr_rel |= 0xFF00; // Sign-extend 8-bit offset to 16-bit

    return 0;
}