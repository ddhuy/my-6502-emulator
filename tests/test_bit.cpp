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

    // Zero flag
    cpu.setFlag(CPU6502::Z, false);
    cpu.setFlag(CPU6502::N, false);
    cpu.setFlag(CPU6502::V, false);
    cpu.A = 0x0F;
    ram.write(0x0, 0xF0);
    cpu.BIT();
    assert(cpu.getFlag(CPU6502::Z) == true);
    assert(cpu.getFlag(CPU6502::N) == true);
    assert(cpu.getFlag(CPU6502::V) == true);

    // Non-zero AND
    cpu.setFlag(CPU6502::Z, true);
    cpu.setFlag(CPU6502::N, true);
    cpu.setFlag(CPU6502::V, true);
    cpu.A = 0xFF;
    ram.write(0x0, 0x01);
    cpu.BIT();
    assert(cpu.getFlag(CPU6502::Z) == false);
    assert(cpu.getFlag(CPU6502::N) == false);
    assert(cpu.getFlag(CPU6502::V) == false);

    // Only N flag set
    cpu.setFlag(CPU6502::Z, false);
    cpu.setFlag(CPU6502::N, false);
    cpu.setFlag(CPU6502::V, true);
    cpu.A = 0x7F;
    ram.write(0x0, 0x80);
    cpu.BIT();
    assert(cpu.getFlag(CPU6502::Z) == true);
    assert(cpu.getFlag(CPU6502::N) == true);
    assert(cpu.getFlag(CPU6502::V) == false);

    // Only V flag set
    cpu.setFlag(CPU6502::Z, false);
    cpu.setFlag(CPU6502::N, true);
    cpu.setFlag(CPU6502::V, false);
    cpu.A = 0x3C;
    ram.write(0x0, 0x40);
    cpu.BIT();
    assert(cpu.getFlag(CPU6502::Z) == true);
    assert(cpu.getFlag(CPU6502::N) == false);
    assert(cpu.getFlag(CPU6502::V) == true);

    return 0;
}
