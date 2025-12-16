#include <cassert>

#include "CPU6502.hpp"
#include "utils/Debug.hpp"
#include "bus/Bus.hpp"


CPU6502::CPU6502()
{
}

void CPU6502::connectBus(Bus* bus)
{
    _bus = bus;
}

void CPU6502::reset()
{
    DBG_ASSERT(_bus != nullptr);

    // Reset registers
    A = 0;
    X = 0;
    Y = 0;
    SP = 0xFD; // Stack Pointer starts at 0xFD
    status = 0x00 | 0x20; // Set unused flag

    // Set Program Counter to the address stored at the reset vector (0xFFFC)
    uint16_t lo = _bus->read(0xFFFC);
    uint16_t hi = _bus->read(0xFFFD);
    PC = (hi << 8) | lo;
}

void CPU6502::step()
{
    DBG_ASSERT(_bus != nullptr);

    // Fetch the next opcode
    uint8_t opcode = _bus->read(PC++);

    // Decode and execute the opcode
    // (Implementation of opcode handling would go here)
}

uint8_t CPU6502::fetchByte()
{
    DBG_ASSERT(_bus != nullptr);

    // Fetch a byte from the current PC location
    uint8_t data = _bus->read(PC++);

    return data;
}