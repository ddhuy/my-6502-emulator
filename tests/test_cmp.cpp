#include <cassert>
#include "cpu/CPU6502.hpp"
#include "bus/Bus.hpp"
#include "mem/RAM.hpp"


int main()
{
    RAM ram;
    Bus bus;
    CPU6502 cpu;

    bus.attachMemory(&ram);
    cpu.connectBus(&bus);

    // Equal
    cpu.A = 0x42;
    ram.write(0x0, 0x42);
    cpu.CMP();
    assert(cpu.getFlag(CPU6502::C) == true);
    assert(cpu.getFlag(CPU6502::Z) == true);
    assert(cpu.getFlag(CPU6502::N) == false);

    // Greater than
    cpu.A = 0x50;
    ram.write(0x0, 0x40);
    cpu.CMP();
    assert(cpu.getFlag(CPU6502::C) == true);
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == false);

    // Less than
    cpu.A = 0x30;
    ram.write(0x0, 0x40);
    cpu.CMP();
    assert(cpu.getFlag(CPU6502::C) == false);
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == true);

    // Boundary case: A = 0x00, M = 0x01
    cpu.A = 0x00;
    ram.write(0x0, 0x01);
    cpu.CMP();
    assert(cpu.getFlag(CPU6502::C) == false);
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == true);

    // Boundary case: A = 0xFF, M = 0xFE
    cpu.A = 0xFF;
    ram.write(0x0, 0xFE);
    cpu.CMP();
    assert(cpu.getFlag(CPU6502::C) == true);
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == false);

    // Signed vs Unsigned: A = 0x80, M = 0x7F
    cpu.A = 0x80;
    ram.write(0x0, 0x7F);
    cpu.CMP();
    assert(cpu.getFlag(CPU6502::C) == true); // Unsigned: A > M
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == false);

    return 0;
}