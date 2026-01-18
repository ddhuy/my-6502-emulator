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
    P = StatusFlag::U; // Set unused flag

    // Set Program Counter to the address stored at the reset vector (0xFFFC)
    uint16_t lo = _bus->read(0xFFFC);
    uint16_t hi = _bus->read(0xFFFD);
    PC = (hi << 8) | lo;
}

void CPU6502::clock()
{
    if (_cycles == 0)
    {
        if (nmi_pending)
        {
            nmi_pending = false;
            interrupt(0xFFFA); // NMI vector
        }
        else if (irq_pending && !getFlag(I))
        {
            irq_pending = false;
            interrupt(0xFFFE); // IRQ vector
        }
        else
        {
            // Fetch the next opcode
            _opcode = _bus->read(PC++);
            auto& instruction = instructionTable[_opcode];

            _cycles = instruction.cycles;

            // Call the addressing mode and operation functions
            uint8_t extra_cycle1 = (this->*instruction.addrmode)();
            uint8_t extra_cycle2 = (this->*instruction.operate)();

            _cycles += (extra_cycle1 & extra_cycle2);
        }
    }

    _cycles--;
}

void CPU6502::step()
{
    DBG_ASSERT(_bus != nullptr);

    do {
        clock();
    } while (_cycles > 0);
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
    if (value)
        P |= flag;
    else
        P &= ~flag;
}

bool CPU6502::getFlag(StatusFlag flag) const
{
    return (P & flag) != 0;
}

void CPU6502::updateZN(uint8_t value)
{
    setFlag(StatusFlag::Z, value == 0);
    setFlag(StatusFlag::N, (value & 0x80) != 0);
}

uint8_t CPU6502::fetch()
{
    if (instructionTable[_opcode].addrmode != &CPU6502::ACC)
        _fetched = _bus->read(_addr_abs);
    return _fetched;
}

uint8_t CPU6502::LDA()
{
    fetch();
    A = _fetched;
    updateZN(A);
    return 1;
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

    _cycles++; // Branch taken, add a cycle

    uint16_t old_pc = PC;
    PC += _addr_rel;

    // Page boundary crossed?
    if ((old_pc & 0xFF00) != (PC & 0xFF00))
        _cycles++; // Add another cycle

    return 0;
}

uint8_t CPU6502::BEQ()
{
    return branch(getFlag(Z));
}

uint8_t CPU6502::BNE()
{
    return branch(!getFlag(Z));
}

uint8_t CPU6502::BMI()
{
    return branch(getFlag(N));
}

uint8_t CPU6502::BPL()
{
    return branch(!getFlag(N));
}

uint8_t CPU6502::BCS()
{
    return branch(getFlag(C));
}

uint8_t CPU6502::BCC()
{
    return branch(!getFlag(C));
}

uint8_t CPU6502::BVS()
{
    return branch(getFlag(V));
}

uint8_t CPU6502::BVC()
{
    return branch(!getFlag(V));
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
    uint16_t target_addr = _addr_abs;
    uint16_t return_addr = PC - 1;

    push((return_addr >> 8) & 0xFF); // high
    push(return_addr & 0xFF);        // low

    PC = target_addr;
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

uint8_t CPU6502::ADC()
{
    int value = fetch();
    uint16_t sum = (uint16_t)A + value + (getFlag(C) ? 1 : 0);
    
    setFlag(V, (~(A ^ value) & (A ^ sum)) & 0x80);

    if (getFlag(D)) {
        if (((A & 0x0F) + (value & 0x0F) + getFlag(C)) > 9)
            sum += 0x06;
        if (sum > 0x99)
            sum += 0x60;
    }

    setFlag(C, sum > 0xFF);

    A = sum & 0xFF;

    setFlag(Z, A == 0x00);
    setFlag(N, A & 0x80);

    return 1;
}

uint8_t CPU6502::SBC()
{
    uint16_t M = fetch();
    uint16_t value = M ^ 0x00FF;

    uint16_t sum = (uint16_t)A + value + getFlag(C);

    // Binary overflow (always binary)
    setFlag(V, (~(A ^ value) & (A ^ sum)) & 0x80);

    if (getFlag(D)) {
        // Low nibble borrow?
        if (((A & 0x0F) - (M & 0x0F) - (getFlag(C) ? 0 : 1)) < 0)
            sum -= 0x06;

        // High nibble borrow?
        if (sum <= 0xFF)
            sum -= 0x60;
    }

    setFlag(C, sum > 0xFF);

    A = sum & 0xFF;

    setFlag(Z, A == 0x00);
    setFlag(N, A & 0x80);

    return 1;
}

void CPU6502::compare(uint8_t reg, uint8_t value)
{
    uint16_t temp = (uint16_t)reg - (uint16_t)value;

    setFlag(C, reg >= value);
    setFlag(Z, (temp & 0x00FF) == 0);
    setFlag(N, temp & 0x80);
}

uint8_t CPU6502::CMP()
{
    uint8_t value = fetch();
    compare(A, value);
    return 1;
}

uint8_t CPU6502::CPX()
{
    uint8_t value = fetch();
    compare(X, value);
    return 1;
}

uint8_t CPU6502::CPY()
{
    uint8_t value = fetch();
    compare(Y, value);
    return 1;
}

uint8_t CPU6502::BIT()
{
    uint8_t value = fetch();

    setFlag(Z, (A & value) == 0);
    setFlag(N, value & 0x80);
    setFlag(V, value & 0x40);

    return 0;
}

uint8_t CPU6502::NOP()
{
    return 0;
}

uint8_t CPU6502::ASL()
{
    uint16_t value = fetch();
    uint16_t result = (uint16_t)value << 1;

    setFlag(C, (result & 0xFF00) != 0);
    setFlag(Z, (result & 0x00FF) == 0);
    setFlag(N, result & 0x80);

    if (instructionTable[_opcode].addrmode == &CPU6502::ACC)
        A = result & 0x00FF;
    else
        _bus->write(_addr_abs, result & 0x00FF);

    return 0;
}

uint8_t CPU6502::LSR()
{
    uint16_t value = fetch();
    setFlag(C, (value & 0x01) != 0);

    uint16_t result = (uint16_t)value >> 1;

    setFlag(Z, (result & 0x00FF) == 0);
    setFlag(N, false); // Bit 7 is always 0 after LSR

    if (instructionTable[_opcode].addrmode == &CPU6502::ACC)
        A = result & 0x00FF;
    else
        _bus->write(_addr_abs, result & 0x00FF);

    return 0;
}

uint8_t CPU6502::ROL()
{
    uint16_t value = fetch();
    uint16_t result = ((uint16_t)value << 1) | (getFlag(C) ? 1 : 0);

    setFlag(C, (result & 0x80) != 0);
    setFlag(Z, (result & 0x00FF) == 0);
    setFlag(N, result & 0x80);

    if (instructionTable[_opcode].addrmode == &CPU6502::ACC)
        A = result & 0x00FF;
    else
        _bus->write(_addr_abs, result & 0x00FF);

    return 0;
}

uint8_t CPU6502::ROR()
{
    uint16_t value = fetch();
    uint16_t result = ((getFlag(C) ? 0x80 : 0) | (value >> 1));

    setFlag(C, (value & 0x01) != 0);
    setFlag(Z, (result & 0x00FF) == 0);
    setFlag(N, result & 0x80);

    if (instructionTable[_opcode].addrmode == &CPU6502::ACC)
        A = result & 0x00FF;
    else
        _bus->write(_addr_abs, result & 0x00FF);

    return 0;
}

uint8_t CPU6502::BRK()
{
    PC++; // skip padding byte

    push((PC >> 8) & 0xFF); // Push high byte of PC
    push(PC & 0xFF);        // Push low byte of PC

    push(P | StatusFlag::B | StatusFlag::U); // Push status register

    setFlag(I, true); // Disable interrupts

    // Load IRQ vector
    uint16_t lo = _bus->read(0xFFFE); // IRQ vector low byte
    uint16_t hi = _bus->read(0xFFFF); // IRQ vector high byte
    PC = (hi << 8) | lo;

    return 0;
}

uint8_t CPU6502::RTI()
{
    P = pull();
    P &= ~StatusFlag::B; // Clear Break flag
    P |= StatusFlag::U;  // Set Unused flag

    uint16_t lo = pull();
    uint16_t hi = pull();
    PC = (hi << 8) | lo;

    return 0;
}

void CPU6502::interrupt(uint16_t vector)
{
    push((PC >> 8) & 0xFF); // Push high byte of PC
    push(PC & 0xFF);        // Push low byte of PC

    // Push status register with B=0, U=1
    push((P & ~StatusFlag::B) | StatusFlag::U);

    setFlag(I, true); // Disable further interrupts

    // Load vector
    uint16_t lo = _bus->read(vector);
    uint16_t hi = _bus->read(vector + 1);
    PC = (hi << 8) | lo;

    _cycles = 7; // Interrupt handling takes 7 cycles
}

uint8_t CPU6502::LAX()
{
    fetch();

    A = _fetched;
    X = _fetched;
    updateZN(A);
    return 1;
}

uint8_t CPU6502::SAX()
{
    _bus->write(_addr_abs, A & X);
    return 0;
}

uint8_t CPU6502::DCP()
{
    fetch();

    uint8_t decremented = _fetched - 1;
    _bus->write(_addr_abs, decremented);

    uint16_t temp = uint16_t(A) - uint16_t(decremented);
    setFlag(C, A >= decremented);
    setFlag(Z, (temp & 0x00FF) == 0);
    setFlag(N, temp & 0x80);

    return 0;
}

uint8_t CPU6502::ISC()
{
    fetch();

    // Increment the fetched value and write it back to memory
    uint8_t incremented = _fetched + 1;
    _bus->write(_addr_abs, incremented);

    // Perform SBC with the incremented value
    uint16_t value = incremented ^ 0x00FF;
    uint16_t sum = (uint16_t)A + value + (getFlag(C) ? 1 : 0);

    setFlag(V, (~(A ^ value) & (A ^ sum)) & 0x80);
    setFlag(C, sum > 0xFF);

    A = sum & 0xFF;

    setFlag(Z, A == 0x00);
    setFlag(N, A & 0x80);

    return 0;
}

uint8_t CPU6502::RLA()
{
    fetch();
    
    uint8_t value = (_fetched << 1) | (getFlag(C) ? 1 : 0);
    setFlag(C, _fetched & 0x80);

    _bus->write(_addr_abs, value);

    A &= value;
    updateZN(A);

    return 0;
}

uint8_t CPU6502::RRA()
{
    fetch();

    uint8_t value = (_fetched >> 1) | (getFlag(C) << 7);
    setFlag(C, _fetched & 0x01);

    _bus->write(_addr_abs, value);

    uint16_t sum = A + value + (getFlag(C) ? 1 : 0);

    setFlag(V, (~(A ^ value) & (A ^ sum)) & 0x80);
    setFlag(C, sum > 0xFF);

    A = sum & 0xFF;

    setFlag(Z, A == 0x00);
    setFlag(N, A & 0x80);

    return 0;
}

uint8_t CPU6502::SLO()
{
    fetch();

    uint8_t value = _fetched << 1;
    setFlag(C, _fetched & 0x80);

    _bus->write(_addr_abs, value);

    A |= value;
    updateZN(A);

    return 0;
}

uint8_t CPU6502::SRE()
{
    fetch();

    setFlag(C, _fetched & 0x01);
    uint8_t value = _fetched >> 1;

    _bus->write(_addr_abs, value);

    A ^= value;
    updateZN(A);

    return 0;
}

uint8_t CPU6502::SEC()
{
    setFlag(C, true);
    return 0;
}

uint8_t CPU6502::CLI()
{
    setFlag(I, false);
    return 0;
}

uint8_t CPU6502::SED()
{
    setFlag(D, true);
    return 0;
}

uint8_t CPU6502::TXA()
{
    A = X;
    updateZN(A);
    return 0;
}

uint8_t CPU6502::TAY()
{
    Y = A;
    updateZN(Y);
    return 0;
}

uint8_t CPU6502::TSX()
{
    X = SP;
    updateZN(X);
    return 0;
}

uint8_t CPU6502::TXS()
{
    SP = X;
    return 0;
}

uint8_t CPU6502::PHA()
{
    push(A);
    return 0;
}

uint8_t CPU6502::PLP()
{
    P = pull();
    P |= U; // bit 5 always set
    P &= ~B; // clear B internally
    return 0;
}

uint8_t CPU6502::DEX()
{
    X--;
    updateZN(X);
    return 0;
}

uint8_t CPU6502::DEY()
{
    Y--;
    updateZN(Y);
    return 0;
}

uint8_t CPU6502::INY()
{
    Y++;
    updateZN(Y);
    return 0;
}

uint8_t CPU6502::INC()
{
    fetch();
    uint8_t value = _fetched + 1;
    _bus->write(_addr_abs, value);
    updateZN(value);
    return 0;
}

uint8_t CPU6502::DEC()
{
    fetch();
    uint8_t value = _fetched - 1;
    _bus->write(_addr_abs, value);
    updateZN(value);
    return 0;
}

uint8_t CPU6502::LDX()
{
    fetch();
    X = _fetched;
    updateZN(X);
    return 1;
}

uint8_t CPU6502::LDY()
{
    fetch();
    Y = _fetched;
    updateZN(Y);
    return 1;
}

uint8_t CPU6502::STX()
{
    _bus->write(_addr_abs, X);
    return 0;
}

uint8_t CPU6502::STY()
{
    _bus->write(_addr_abs, Y);
    return 0;
}

uint8_t CPU6502::AND()
{
    fetch();
    A &= _fetched;
    updateZN(A);
    return 1;
}

uint8_t CPU6502::EOR()
{
    fetch();
    A ^= _fetched;
    updateZN(A);
    return 1;
}

uint8_t CPU6502::ANC()
{
    fetch();
    A &= _fetched;
    updateZN(A);
    setFlag(C, A & 0x80);
    return 1;
}

uint8_t CPU6502::ALR()
{
    fetch();
    uint8_t value = A & _fetched;
    setFlag(C, value & 0x01);
    A = value >> 1;
    updateZN(A);
    return 1;
}

uint8_t CPU6502::ARR()
{
    fetch();
    uint8_t value = A & _fetched;
    A = (value >> 1) | (getFlag(C) << 7);

    updateZN(A);
    setFlag(C, A & 0x80);
    setFlag(V, ((A >> 6) ^ (A >> 5)) & 1);

    return 1;
}

uint8_t CPU6502::JMP()
{
    PC = _addr_abs;
    return 0;
}

uint8_t CPU6502::ORA()
{
    fetch();
    A |= _fetched;
    updateZN(A);
    return 1;
}

uint8_t CPU6502::PHP()
{
    push(P | StatusFlag::B | StatusFlag::U);
    return 0;
}

uint8_t CPU6502::PLA()
{
    A = pull();
    updateZN(A);
    return 0;
}

uint8_t CPU6502::CLC()
{
    setFlag(C, false);
    return 0;
}

uint8_t CPU6502::CLD()
{
    setFlag(D, false);
    return 0;
}

uint8_t CPU6502::CLV()
{
    setFlag(V, false);
    return 0;
}

uint8_t CPU6502::TYA()
{
    A = Y;
    updateZN(A);
    return 0;
}

uint8_t CPU6502::SEI()
{
    setFlag(I, true);
    return 0;
}