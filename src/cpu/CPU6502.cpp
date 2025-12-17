#include <cassert>

#include "CPU6502.hpp"
#include "bus/Bus.hpp"
#include "cpu/Instructions.hpp"
#include "utils/Debug.hpp"


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
    status = StatusFlag::U; // Set unused flag

    // Set Program Counter to the address stored at the reset vector (0xFFFC)
    uint16_t lo = _bus->read(0xFFFC);
    uint16_t hi = _bus->read(0xFFFD);
    PC = (hi << 8) | lo;
}

void CPU6502::step()
{
    DBG_ASSERT(_bus != nullptr);

    // Fetch the next opcode
    _opcode = _bus->read(PC++);

    Instruction& instruction = instructionTable[_opcode];
    DBG_ASSERT(instruction.operate != nullptr);
    DBG_ASSERT(instruction.addrmode != nullptr);
    
    // Call the addressing mode and operation functions
    uint8_t extraCycle1 = (this->*instruction.addrmode)();
    uint8_t extraCycle2 = (this->*instruction.operate)();

    _cycles = instruction.cycles + (extraCycle1 + extraCycle2);
}

uint8_t CPU6502::fetchByte()
{
    DBG_ASSERT(_bus != nullptr);

    // Fetch a byte from the current PC location
    uint8_t data = _bus->read(PC++);
    return data;
}

void CPU6502::setFlag(StatusFlag flag, bool value)
{
    if (value) {
        status |= flag;
    } else {
        status &= ~flag;
    }
}

bool CPU6502::getFlag(StatusFlag flag) const
{
    return (status & flag) != 0;
}

void CPU6502::updateZN(uint8_t value)
{
    setFlag(StatusFlag::Z, value == 0);
    setFlag(StatusFlag::N, (value & 0x80) != 0);
}

uint8_t CPU6502::fetch()
{
    DBG_ASSERT(_bus != nullptr);

    _fetched = _bus->read(_addr_abs);
    return _fetched;
}

uint8_t CPU6502::IMM()
{
    _addr_abs = PC++;
    return 0;
}

uint8_t CPU6502::ZP()
{
    DBG_ASSERT(_bus != nullptr);

    _addr_abs = _bus->read(PC++);
    _addr_abs &= 0x00FF; // Zero page address
    return 0;
}

uint8_t CPU6502::ABS()
{
    uint16_t lo = _bus->read(PC++);
    uint16_t hi = _bus->read(PC++);
    _addr_abs = (hi << 8) | lo;
    return 0;
}

uint8_t CPU6502::LDA()
{
    fetch();
    A = _fetched;
    updateZN(A);
    return 0;
}