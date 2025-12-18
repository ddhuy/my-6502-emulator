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

    const Instruction& instruction = instructionTable[_opcode];
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

uint8_t CPU6502::LDA()
{
    fetch();
    A = _fetched;
    updateZN(A);
    return 0;
}

uint8_t CPU6502::STA()
{
    DBG_ASSERT(_bus != nullptr);

    _bus->write(_addr_abs, A);
    return 0;
}

uint8_t CPU6502::TAX()
{
    X = A;
    updateZN(X);
    return 0;
}

uint8_t CPU6502::INX()
{
    X++;
    updateZN(X);
    return 0;
}

uint8_t CPU6502::branch(bool condition)
{
    if (!condition)
        return 0;

    uint16_t old_pc = PC;
    PC += _addr_rel;

    uint8_t extra_cycles = 1;

    // Page boundary crossed?
    if ((old_pc & 0xFF00) != (PC & 0xFF00)) {
        extra_cycles++;
    }

    return extra_cycles;
}

uint8_t CPU6502::BEQ()
{
    return branch(getFlag(Z));
}

uint8_t CPU6502::BNE()
{
    return branch(!getFlag(Z));
}

void CPU6502::push(uint8_t value)
{
    _bus->write(0x0100 | SP, value);
    SP--;
}

uint8_t CPU6502::pull()
{
    SP++;
    return _bus->read(0x0100 | SP);
}

uint8_t CPU6502::JSR()
{
    uint16_t target = _addr_abs;
    DBG_LOG("Jump to 0x%x", target);

    uint16_t return_addr = PC - 1;
    DBG_LOG("Return to 0x%x", return_addr);

    push((return_addr >> 8) & 0xFF); // high
    push(return_addr & 0xFF);        // low

    PC = target;
    return 0;
}


uint8_t CPU6502::RTS()
{
    uint16_t lo = pull();
    uint16_t hi = pull();
    PC = (hi << 8) | lo;
    PC++; // Increment PC to point to the next instruction

    return 0;
}
