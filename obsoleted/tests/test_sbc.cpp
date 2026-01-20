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

    // Test SBC no borrow
    cpu.setFlag(CPU6502::C, true);
    cpu.A = 0x05;
    ram.write(0x0, 0x02);
    cpu.SBC();
    assert(cpu.A == 0x03);
    assert(cpu.getFlag(CPU6502::C) == true);

    // Test SBC borrow occurred
    cpu.setFlag(CPU6502::C, true);
    cpu.A = 0x02;
    ram.write(0x0, 0x05);
    cpu.SBC();
    assert(cpu.A == 0xFD);
    assert(cpu.getFlag(CPU6502::C) == false);

    // Simple decimal mode test
    cpu.setFlag(CPU6502::D, true);
    cpu.setFlag(CPU6502::C, true);
    cpu.A = 0x45;
    ram.write(0x0, 0x12);
    cpu.SBC();
    assert(cpu.A == 0x33);
    assert(cpu.getFlag(CPU6502::C) == true);

    // Decimal mode with borrow
    cpu.setFlag(CPU6502::D, true);
    cpu.setFlag(CPU6502::C, true);
    cpu.A = 0x10;
    ram.write(0x0, 0x01);
    cpu.SBC();
    assert(cpu.A == 0x09);
    assert(cpu.getFlag(CPU6502::C) == true);

    // Full borrow in decimal mode
    cpu.setFlag(CPU6502::D, true);
    cpu.setFlag(CPU6502::C, true);
    cpu.A = 0x00;
    ram.write(0x0, 0x01);
    cpu.SBC();
    assert(cpu.A == 0x99);
    assert(cpu.getFlag(CPU6502::C) == false);

    // Decimal mode overflow
    cpu.setFlag(CPU6502::D, true);
    cpu.setFlag(CPU6502::C, true);
    cpu.A = 0x80;
    ram.write(0x0, 0x01);
    cpu.SBC();
    assert(cpu.A == 0x79);
    assert(cpu.getFlag(CPU6502::V) == true);
    assert(cpu.getFlag(CPU6502::C) == true);

    return 0;
}